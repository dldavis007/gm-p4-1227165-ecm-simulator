/* Host bridge for the independent ROM differential. */
#include <assert.h>
static unsigned int addresses[128], values[128], count;
static void record_write(unsigned int address, unsigned int value)
{
    assert(count + 2 <= 128);
    addresses[count] = address;
    values[count++] = value >> 8;
    addresses[count] = address + 1;
    values[count++] = value & 255;
}
#define BUA_TRACE_MPU_WRITE(a, v) record_write(a, v)
#define main regression_main
#include "../main.c"
#undef main

void probe_reset(void)
{
    memset(&mem, 0, sizeof(mem));
}
unsigned char *probe_io(void) { return mem.io4000; }
unsigned char *probe_ram(void) { return mem.low; }
unsigned char *probe_mpu(void) { return mem.mpu; }
unsigned int probe_trace_size(void) { return count; }
int probe_address(unsigned int i) { return addresses[i]; }
int probe_value(unsigned int i) { return values[i]; }
int probe_fmd_reads(void) { return (int)stats.spark_fmd_reads; }
void probe_tick(void)
{
    count = 0;
    stats.spark_fmd_reads = 0;
    bua_reference_state_12p5ms();
}

void probe_irq(void) { count = 0; irq_6p25ms(); }
void probe_irq_setup(void)
{
    ecm_reset();
    sim_normal_fmd_enabled = 1u;
}
int probe_adc(unsigned int channel)
{
    return channel == 0xB0u ? sim_factory_adc118[11] : adc_sample((bua_u8)channel);
}
int probe_fmd(unsigned int command)
{
    return (command & 128u) ? sim_normal_fmd_byte2 : sim_normal_fmd_byte1;
}
int probe_filter(unsigned int old, unsigned int value, unsigned int coef)
{
    return bua_lag_filter_8_8((bua_u16)old, (bua_u8)value, (bua_u8)coef);
}

void probe_error43(void) { bua_error43_service(); }

void probe_rpm(unsigned int rpm) { sim_set_ecm_reference_rpm(rpm); }
int probe_capture(void) { return sim_ecm_ref.period_reg; }
int probe_reference_seen(void) { return sim_ecm_ref.pulse_since_irq; }
int probe_injections(void) { return (int)stats.physical_injector_services; }

int probe_product(unsigned int a, unsigned int b)
{
    return bua_mul_u16_u16_lf2f0((bua_u16)a, (bua_u16)b, 0);
}

void probe_follower(void) { count = 0; bua_idle_motor_request(); }
void probe_qualification(void) { count = 0; bua_diag_qualification_step108(); }

void probe_major(unsigned int segment)
{
    count = 0;
    major_segment((bua_u8)segment);
}

void probe_cold_boot(void)
{
    BuaPowerOnInput120 in;
    memset(&in, 0, sizeof(in));
    memset(&mem, 0, sizeof(mem));
    count = 0;
    in.socket_checksum_valid = 1u;
    bua_lifecycle_power_cycle_from_hal_step163(0xFFFEu, in);
}

void probe_boot_inputs(unsigned int battery, unsigned int pump,
                       unsigned int diagnostic, unsigned int fmd)
{
    sim_volt_adc = (bua_u8)battery;
    sim_pumpvolt_adc = (bua_u8)pump;
    sim_diag_adc = (bua_u8)diagnostic;
    sim_normal_fmd_byte1 = (bua_u8)fmd;
}
int probe_seeded_boot(unsigned int socket_valid, unsigned int checksum)
{
    BuaPowerOnInput120 in;
    memset(&in, 0, sizeof(in));
    in.err51_at_entry = RAM8(3u) & 8u;
    in.socket_checksum_valid = (bua_u8)socket_valid;
    in.whole_rom_checksum = (bua_u16)checksum;
    count = 0;
    return bua_lifecycle_power_cycle_from_hal_step163(0xFFFEu, in);
}

int probe_map(void) { return mem.io5000; }

int probe_feedback(void) { count = 0; return bua_live_o2_feedback(); }
void probe_blm_correction(unsigned int base, unsigned int step)
{
    bua_u16 result = bua_apply_blm_closed_loop_ldd7a((bua_u16)base, BLM,
        CLOSED_LOOP_PW_CORR, (bua_u8)step, AF_MODE_WORD2, AIR_MODE_WORD);
    ram16be_set(0xC2u, result);
}

void probe_blm_select(void) { count = 0; bua_scheduler_blm_select_step103(); }
void probe_afr(void) { count = 0; bua_select_live_afr(); }
void probe_async(void) { count = 0; bua_scheduler_async_le436_step102(); }
void probe_accel(void) { count = 0; bua_odd_transient_front_12p5ms(); }
void probe_sync_fuel(void) { count = 0; bua_write_lf92a(ram16be_get(0xC2u)); }
void probe_odd_fuel(void) { count = 0; air_fuel_12p5ms(); }
void probe_keyoff(void) { count = 0; bua_ignition_shutdown_odd_step111(); }
void probe_homing(void) { count = 0; bua_iac_shutdown_homing_even_step111(); }
void probe_injector(unsigned int event)
{
    count = 0;
    sim_ecm_ref.injector_services_pending = event;
    bua_common_injector_service_6p25ms();
}
void probe_motor(void) { count = 0; bua_iac_motor_service_6p25ms(); }
void probe_mode4(void) { count = 0; bua_mode4_scheduler_step116(); }
void probe_flash(void) { count = 0; bua_diag_flash_step109(); }
void probe_factory(void)
{
    count = 0;
    sim_factory_diagnostic_adc117 = sim_diag_adc;
    sim_factory_battery_adc117 = sim_volt_adc;
    bua_factory_irq_step117();
}
void probe_factory_hal(void) { count = 0; bua_lifecycle_irq_from_hal_step165(); }
void probe_factory_inputs(void)
{
    sim_diag_adc = 60u;
    sim_normal_fmd_byte1 = 0u;
    sim_factory_adc118[11] = 0xA5u;
}
void probe_factory_mode(unsigned int mode) { sim_normal_fmd_byte1 = (bua_u8)mode; }

int probe_frame(const bua_u8 *data, unsigned int length)
{
    return bua_sci115_receive(data, (bua_u8)length);
}
int probe_response_length(void) { return bua_sci115.output_count; }
int probe_response(unsigned int i) { return bua_sci115.output[i]; }
void probe_maf_service(void) { count = 0; bua_common_maf_6p25ms(); }
void probe_stopped_airflow(unsigned int maf, unsigned int pump)
{
    sim_maf_adc = (bua_u8)maf;
    sim_pumpvolt_adc = (bua_u8)pump;
}
void probe_o2_input(unsigned int sample) { sim_o2_adc = (bua_u8)sample; }
void probe_warm_fuel(void) { count = 0; bua_odd_normal_fuel_after_load_12p5ms(); }
void probe_blm_tail(void) { count = 0; bua_odd_50ms_tail_lddc2(); }
void probe_road(unsigned int mph)
{
    sim_set_vss_mph(mph);
    sim_normal_fmd_byte1 = mph ? 1u : 0u;
}
int probe_vss_capture(void) { return sim_vss_capture_reg; }
void probe_temperature_inputs(unsigned int cts, unsigned int mat)
{
    sim_cts_adc = (bua_u8)cts;
    sim_mat_adc = (bua_u8)mat;
}
