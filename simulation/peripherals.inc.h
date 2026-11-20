/* HAL-visible endpoint corresponding to the source's software-interrupt loop. */
static bua_u8 sim_soft_powerdown_latched = 0u;
static bua_u8 sim_iac_motor_on = 1u;

static bua_u8 sim_map2_adc = 0u;
static bua_u8 sim_volt_adc = 128u;
static bua_u8 sim_o2_adc = 128u;
static bua_u8 sim_map_adc = 0u;
static bua_u8 sim_cts_adc = 120u;
static bua_u8 sim_tps_adc = 35u;
static bua_u8 sim_pumpvolt_adc = 0u;
static bua_u8 sim_diag_adc = 200u;
static bua_u8 sim_mat_adc = 0u;
static bua_u8 sim_esc_adc = 0u;
static bua_u8 sim_maf_adc = 128u;
/* Step 158 raw replies from the normal-operation FMD/SPI boundary.  The
 * enable keeps historical simulations unchanged until a caller explicitly
 * supplies this input pair through the HAL. */
static bua_u8 sim_normal_fmd_byte1 = 0u;
static bua_u8 sim_normal_fmd_byte2 = 0u;
static bua_u8 sim_normal_fmd_enabled = 0u;
/* PC-only VSS signal generator.  Capture register emulates P4 L3FC2. */
static bua_u32 sim_vss_abs_ticks;
static bua_u32 sim_vss_next_edge;
static bua_u16 sim_vss_capture_reg;
static bua_u16 sim_vss_period_ticks;
static bua_u8 sim_serial_line_high;
typedef struct {
    bua_u8 have_low_start;
    bua_u32 low_start_time;
    bua_u8 pulse_in_frame;
    bua_u8 assembling;
    bua_u8 bytes[16];
    bua_u16 frame_widths[16];
    bua_u8 byte_count;
} BuaSerialReceiver;
static BuaSerialReceiver rx;
static bua_u8 rx_enabled;
/* ---------------------------------------------------------------------- */
/* PC-only 1986 Corvette cluster input model.                              */
/*                                                                        */
/* These are SIGNAL SOURCES, not a claimed reproduction of the cluster    */
/* firmware.  The uploaded schematic establishes separate direct inputs   */
/* for VSS, HEI tach, fuel, coolant, oil pressure/temp and battery, plus   */
/* the ECM serial-data input.  Unknown electrical transfer functions are   */
/* deliberately left outside this model for now.                           */
/* ---------------------------------------------------------------------- */
/*
 * Pulse calibration constants are intentionally isolated here.
 * They are PC/HARDWARE-SIMULATOR parameters, not yet claims about the
 * exact 1986 Corvette electrical scaling.  When we verify the factory
 * VSS pulses/mile and HEI tach pulses/revolution, only these constants
 * need to change.
 */
#define SIM_VSS_PULSES_PER_MILE  4000ul
#define SIM_TACH_PULSES_PER_REV     4ul
typedef struct {
    unsigned int fuel_sender_raw;    /* arbitrary PC stimulus units */
    unsigned int coolant_raw;
    unsigned int oil_pressure_raw;
    unsigned int oil_temp_raw;
    unsigned int battery_mv;
} DashDirectInputs;
typedef struct {
    bua_u32 period_us;
    bua_u32 high_us;
    bua_u32 next_edge_us;
    bua_u8 level;
    bua_u32 rising_edges;
    bua_u32 falling_edges;
} SimPulseSource;
typedef struct {
    unsigned int speed_mph;
    unsigned int rpm;
    bua_u32 vss_last_rise_us;
    bua_u32 vss_period_us;
    bua_u32 tach_last_rise_us;
    bua_u32 tach_period_us;
    bua_u32 vss_rising_edges;
    bua_u32 tach_rising_edges;
    bua_u8 vss_valid;
    bua_u8 tach_valid;
    unsigned int fuel_sender_raw;
    unsigned int coolant_raw;
    unsigned int oil_pressure_raw;
    unsigned int oil_temp_raw;
    unsigned int battery_mv;
    bua_u8 serial_bytes[16];
    bua_u8 serial_count;
} DashModel;
static DashDirectInputs dash_in;
static DashModel dash;
static SimPulseSource sim_dash_vss;
static SimPulseSource sim_dash_tach;
static bua_u32 sim_vehicle_time_us;
/* PC-only P4 distributor-reference model.
 * For the 8-cylinder BUA calibration (LC009 == 0), the executable RPM math
 * is consistent with four reference pulses per crankshaft revolution and a
 * 65.536-kHz period counter.  Keep this separate from the cluster HEI tach
 * generator so neither decoder can accidentally share computed RPM. */
#define SIM_ECM_REF_PULSES_PER_REV 4ul
#define SIM_P4_REF_TIMER_HZ       65536ul
typedef struct {
    double phase;
    double frequency_hz;
    bua_u32 absolute_timer_ticks;
    bua_u32 last_edge_ticks;
    bua_u16 period_reg;
    bua_u8 pulse_since_irq;
    bua_u8 injector_ref_phase;
    bua_u8 injector_services_pending;
} SimEcmReference;
static SimEcmReference sim_ecm_ref;
static bua_u32 round_div_u32(bua_u32 numerator, bua_u32 denominator)
{
    if (denominator == 0ul)
        return 0ul;
    return (numerator + denominator / 2ul) / denominator;
}
static bua_u32 scale_ratio_u32(bua_u32 value, bua_u32 multiplier,
                               bua_u32 divisor)
{
    double scaled;
    if (divisor == 0ul)
        return 0ul;
    /* PC-only waveform scheduling helper.  Double is intentional here to
     * avoid 32-bit overflow in value*multiplier at very low VSS rates.
     * No ECM algorithm uses this helper. */
    scaled = ((double)value * (double)multiplier) / (double)divisor;
    if (scaled < 0.5)
        return 0ul;
    return (bua_u32)(scaled + 0.5);
}
static void pulse_source_set_period(SimPulseSource *p, bua_u32 period_us)
{
    bua_u32 old_interval;
    bua_u32 new_interval;
    bua_u32 remaining;
    bua_u32 scaled_remaining;
    /*
     * Step 9 preserves waveform phase when frequency changes.  The old
     * Step-8 helper restarted the waveform at every command change, which
     * produced artificial zero-speed/zero-RPM samples at profile boundaries.
     *
     * A square-wave source has one edge every half-period.  Preserve the
     * fraction of the current half-cycle that remains, then scale that
     * fraction to the new half-period.  This is a PC signal-generator model;
     * the production hardware implementation can use an integer phase
     * accumulator or timer-compare rescheduling to obtain the same behavior.
     */
    if (period_us == 0ul) {
        p->period_us = 0ul;
        p->high_us = 0ul;
        p->next_edge_us = 0ul;
        p->level = 0u;
        return;
    }
    new_interval = period_us / 2ul;
    if (new_interval == 0ul)
        new_interval = 1ul;
    if (p->period_us == 0ul || p->next_edge_us <= sim_vehicle_time_us) {
        /* Starting from rest: wait one half-cycle before the first edge. */
        p->level = 0u;
        p->next_edge_us = sim_vehicle_time_us + new_interval;
    } else {
        old_interval = p->period_us / 2ul;
        if (old_interval == 0ul)
            old_interval = 1ul;
        remaining = p->next_edge_us - sim_vehicle_time_us;
        if (remaining > old_interval)
            remaining = old_interval;
        scaled_remaining = scale_ratio_u32(remaining, new_interval,
                                           old_interval);
        if (scaled_remaining == 0ul)
            scaled_remaining = 1ul;
        p->next_edge_us = sim_vehicle_time_us + scaled_remaining;
    }
    p->period_us = period_us;
    p->high_us = new_interval;
}
static void sim_set_dash_vss_mph(unsigned int mph)
{
    bua_u32 denominator;
    bua_u32 period;
    if (mph == 0u) {
        pulse_source_set_period(&sim_dash_vss, 0ul);
        return;
    }
    denominator = (bua_u32)mph * SIM_VSS_PULSES_PER_MILE;
    period = round_div_u32(3600000000ul, denominator);
    pulse_source_set_period(&sim_dash_vss, period);
}
static void sim_set_dash_tach_rpm(unsigned int rpm)
{
    bua_u32 denominator;
    bua_u32 period;
    if (rpm == 0u) {
        pulse_source_set_period(&sim_dash_tach, 0ul);
        return;
    }
    denominator = (bua_u32)rpm * SIM_TACH_PULSES_PER_REV;
    period = round_div_u32(60000000ul, denominator);
    pulse_source_set_period(&sim_dash_tach, period);
}
static void dash_vss_edge(bua_u8 high, bua_u32 when_us)
{
    if (high != 0u) {
        ++dash.vss_rising_edges;
        if (dash.vss_valid != 0u) {
            dash.vss_period_us = when_us - dash.vss_last_rise_us;
            if (dash.vss_period_us != 0ul)
                dash.speed_mph = (unsigned int)round_div_u32(
                    3600000000ul,
                    dash.vss_period_us * SIM_VSS_PULSES_PER_MILE);
        }
        dash.vss_last_rise_us = when_us;
        dash.vss_valid = 1u;
    }
}
static void dash_tach_edge(bua_u8 high, bua_u32 when_us)
{
    if (high != 0u) {
        ++dash.tach_rising_edges;
        if (dash.tach_valid != 0u) {
            dash.tach_period_us = when_us - dash.tach_last_rise_us;
            if (dash.tach_period_us != 0ul)
                dash.rpm = (unsigned int)round_div_u32(
                    60000000ul,
                    dash.tach_period_us * SIM_TACH_PULSES_PER_REV);
        }
        dash.tach_last_rise_us = when_us;
        dash.tach_valid = 1u;
    }
}
static void pulse_source_advance(SimPulseSource *p, bua_u32 end_us,
                                 void (*edge_fn)(bua_u8, bua_u32))
{
    bua_u32 low_us;
    if (p->period_us == 0ul)
        return;
    low_us = p->period_us - p->high_us;
    if (low_us == 0ul)
        low_us = 1ul;
    while (p->next_edge_us < end_us) {
        p->level = (bua_u8)(p->level == 0u);
        if (p->level != 0u) {
            ++p->rising_edges;
            edge_fn(1u, p->next_edge_us);
            p->next_edge_us += p->high_us;
        } else {
            ++p->falling_edges;
            edge_fn(0u, p->next_edge_us);
            p->next_edge_us += low_us;
        }
    }
}
static void sim_advance_dash_signals_6p25ms(void)
{
    bua_u32 end_us;
    end_us = sim_vehicle_time_us + 6250ul;
    pulse_source_advance(&sim_dash_vss, end_us, dash_vss_edge);
    pulse_source_advance(&sim_dash_tach, end_us, dash_tach_edge);
    sim_vehicle_time_us = end_us;
}
static void dash_set_test_inputs(void)
{
    dash_in.fuel_sender_raw = 128u;
    dash_in.coolant_raw = 96u;
    dash_in.oil_pressure_raw = 160u;
    dash_in.oil_temp_raw = 88u;
    dash_in.battery_mv = 14200u;
}
static void dash_update_direct_inputs(void)
{
    /* Analog sender transfer functions remain intentionally unscaled. */
    dash.fuel_sender_raw = dash_in.fuel_sender_raw;
    dash.coolant_raw = dash_in.coolant_raw;
    dash.oil_pressure_raw = dash_in.oil_pressure_raw;
    dash.oil_temp_raw = dash_in.oil_temp_raw;
    dash.battery_mv = dash_in.battery_mv;
}
static void dash_accept_serial_receiver(void)
{
    bua_u8 i;
    dash.serial_count = rx.byte_count;
    if (dash.serial_count > 16u)
        dash.serial_count = 16u;
    for (i = 0u; i < dash.serial_count; ++i)
        dash.serial_bytes[i] = rx.bytes[i];
}
static bua_u8 sim_factory_battery_adc117=128u;
static bua_u8 sim_factory_diagnostic_adc117=40u;
static bua_u8 sim_factory_fmd_byte1_117=0u;
static bua_u8 sim_factory_fmd_byte2_117=0u;
static bua_u8 sim_factory_swi_reason117;

static bua_u8 sim_factory_adc118[12];

/* PC/HAL stimulus for LF1E0. Bit 5 is the only bit consumed at $C986. */
static bua_u8 sim_startup_fmd_status=0x20u;

static bua_u8 adc_sample(bua_u8 channel)
{
    switch (channel & 0xF0u) {
        case 0x00u: return sim_map2_adc;
        case 0x10u: return sim_volt_adc;
        case 0x20u: return sim_o2_adc;
        case 0x30u: return sim_map_adc;
        case 0x40u: return sim_cts_adc;
        case 0x50u: return sim_tps_adc;
        case 0x60u: return sim_pumpvolt_adc;
        case 0x70u: return sim_diag_adc;
        case 0x80u: return sim_mat_adc;
        case 0x90u: return sim_esc_adc;
        case 0xA0u: return sim_maf_adc;
        default:    return 0u;
    }
}

/* ---------------------------------------------------------------------- */
/* Step 10: P4 distributor-reference input and original RPM arithmetic.     */
/* ---------------------------------------------------------------------- */
static void sim_set_ecm_reference_rpm(unsigned int rpm)
{
    sim_ecm_ref.frequency_hz = ((double)rpm *
                                (double)SIM_ECM_REF_PULSES_PER_REV) / 60.0;
}
static void sim_advance_ecm_reference_6p25ms(void)
{
    double pulses;
    double period_ticks_d;
    bua_u32 edge_tick;
    bua_u32 delta;
    sim_ecm_ref.pulse_since_irq = 0u;
    sim_ecm_ref.absolute_timer_ticks += 410ul; /* 6.25 ms * 65536 ~= 409.6 */
    if (sim_ecm_ref.frequency_hz <= 0.0)
        return;
    pulses = sim_ecm_ref.frequency_hz * 0.00625;
    sim_ecm_ref.phase += pulses;
    period_ticks_d = (double)SIM_P4_REF_TIMER_HZ / sim_ecm_ref.frequency_hz;
    while (sim_ecm_ref.phase >= 1.0) {
        /* Locate the edge within the current 6.25-ms interval from the
         * remaining phase.  PC-only double arithmetic schedules the input;
         * the ECM RPM calculation below remains integer/fixed-point. */
        double excess = sim_ecm_ref.phase - 1.0;
        double back_ticks = excess * period_ticks_d;
        if (back_ticks < 0.0)
            back_ticks = 0.0;
        if (back_ticks > 409.6)
            back_ticks = 409.6;
        edge_tick = sim_ecm_ref.absolute_timer_ticks - (bua_u32)(back_ticks + 0.5);
        if (sim_ecm_ref.last_edge_ticks != 0ul) {
            delta = edge_tick - sim_ecm_ref.last_edge_ticks;
            sim_ecm_ref.period_reg = (bua_u16)delta;
            /* Peripheral capture, independent of CPU writes to the MPU. */
            mem.mpu[0] = (bua_u8)(delta >> 8);
            mem.mpu[1] = (bua_u8)delta;
        }
        sim_ecm_ref.last_edge_ticks = edge_tick;
        sim_ecm_ref.pulse_since_irq = 1u;
        sim_ecm_ref.injector_ref_phase =
            (bua_u8)(sim_ecm_ref.injector_ref_phase + 1u);
        if (sim_ecm_ref.injector_ref_phase >= 4u) {
            sim_ecm_ref.injector_ref_phase = 0u;
            if (sim_ecm_ref.injector_services_pending != 255u)
                ++sim_ecm_ref.injector_services_pending;
            ++stats.physical_injector_services;
        }
        MINOR_MODE_WORD2 |= REF_SEEN_BIT;
        ++stats.ref_pulses;
        sim_ecm_ref.phase -= 1.0;
    }
}
/* ---------------------------------------------------------------------- */
/* PC VSS signal generator                                                */
/* ---------------------------------------------------------------------- */
/*
 * BUA segment 2 reduces to approximately:
 *
 *      MPH ~= 117760 / VSS_period_ticks
 *
 * before filtering.  Therefore the simulator can generate a desired road
 * speed without making assumptions about the physical DG128 timer yet.
 */
static void sim_set_vss_mph(unsigned int mph)
{
    bua_u32 p;
    bua_u32 old_p;
    bua_u32 remaining;
    bua_u32 scaled_remaining;
    if (mph == 0u) {
        sim_vss_period_ticks = 0u;
        sim_vss_next_edge = 0ul;
        return;
    }
    p = (117760ul + ((bua_u32)mph / 2ul)) / (bua_u32)mph;
    if (p < 1ul)
        p = 1ul;
    if (p > 65535ul)
        p = 65535ul;
    old_p = (bua_u32)sim_vss_period_ticks;
    if (old_p == 0ul || sim_vss_next_edge <= sim_vss_abs_ticks) {
        /* Starting from rest: first capture occurs one full period later. */
        sim_vss_next_edge = sim_vss_abs_ticks + p;
    } else {
        /* Preserve fractional phase to the next capture while changing rate. */
        remaining = sim_vss_next_edge - sim_vss_abs_ticks;
        if (remaining > old_p)
            remaining = old_p;
        scaled_remaining = scale_ratio_u32(remaining, p, old_p);
        if (scaled_remaining == 0ul)
            scaled_remaining = 1ul;
        sim_vss_next_edge = sim_vss_abs_ticks + scaled_remaining;
    }
    sim_vss_period_ticks = (bua_u16)p;
}
static void sim_advance_vss_6p25ms(void)
{
    /*
     * 818 abstract timer ticks per 6.25 ms is consistent with the BUA VSS
     * conversion constant closely enough for this deterministic PC test.
     * The embedded DG128 hardware layer will use real timer captures.
     */
    sim_vss_abs_ticks += 818ul;
    if (sim_vss_period_ticks == 0u)
        return;
    while (sim_vss_next_edge <= sim_vss_abs_ticks) {
        sim_vss_capture_reg = (bua_u16)sim_vss_next_edge;
        sim_vss_next_edge += (bua_u32)sim_vss_period_ticks;
    }
}

/* Advance the host pulse sources and sample the MPU event bits for this IRQ. */
static bua_u8 bua_hal_minor_irq_status(void)
{
    bua_u8 status;
    sim_advance_dash_signals_6p25ms();
    sim_advance_ecm_reference_6p25ms();
    sim_advance_vss_6p25ms();
    status = sim_ecm_ref.injector_services_pending ? 0x40u : 0u;
    if (sim_ecm_ref.pulse_since_irq)
        status |= 8u;
    return status;
}

static bua_u8 bua_hal_take_injector_event(void)
{
    if (sim_ecm_ref.injector_services_pending == 0u)
        return 0u;
    --sim_ecm_ref.injector_services_pending;
    return 1u;
}

static bua_u16 bua_hal_vss_capture(void) { return sim_vss_capture_reg; }
static bua_u8 bua_hal_powerdown_latched(void) { return sim_soft_powerdown_latched; }
static bua_u8 bua_hal_iac_motor_on(void) { return sim_iac_motor_on; }
static void bua_hal_drive_iac_motor(bua_u8 on) { sim_iac_motor_on = on; }
static bua_u8 bua_hal_fmd_enabled(void) { return sim_normal_fmd_enabled; }
static bua_u8 bua_hal_fmd_byte1(void) { return sim_normal_fmd_byte1; }
static bua_u8 bua_hal_fmd_byte2(void) { return sim_normal_fmd_byte2; }

static void bua_hal_seed_factory_inputs(bua_u8 battery, bua_u8 diagnostic, bua_u8 fmd)
{
    sim_factory_battery_adc117 = battery;
    sim_factory_diagnostic_adc117 = diagnostic;
    sim_factory_fmd_byte1_117 = fmd;
}

static void bua_hal_refresh_factory_adc(unsigned int channels)
{
    unsigned int i;
    for (i = 0u; i < channels; ++i)
        sim_factory_adc118[i] = hw_adc((bua_u8)(i << 4));
}

static bua_u8 bua_hal_factory_adc(unsigned int channel) { return sim_factory_adc118[channel]; }
static bua_u8 bua_hal_factory_stopped(void) { return sim_factory_swi_reason117 != 0u; }
static void bua_hal_factory_stop(bua_u8 reason) { sim_factory_swi_reason117 = reason; }
static bua_u8 bua_hal_startup_fmd(void) { return sim_startup_fmd_status; }
static void bua_hal_set_startup_fmd(bua_u8 status) { sim_startup_fmd_status = status; }
