static void bua_common_maf_6p25ms(void);
static void bua_common_tps_6p25ms(void);
static void bua_common_injector_service_6p25ms(void);
static void bua_odd_fuel_chain_12p5ms(void);
static void bua_step94_fuel_state_reset(void);
static void bua_odd_transient_front_12p5ms(void);
static void bua_odd_50ms_tail_lddc2(void);
static bua_u8 bua_ignition_shutdown_odd_step111(void);
static void bua_iac_shutdown_homing_even_step111(void);
static void air_fuel_12p5ms(void)
{
    ++stats.air_fuel_loops;
    if(sim_legacy_ignition_shutdown_freeze!=0u ||
       bua_ignition_shutdown_odd_step111()==0u)
        bua_odd_fuel_chain_12p5ms();
}
/* LCD05: merge the current L0035 reference/diagnostic bit into L0040 before
 * any even-path O2 or spark work.  The source sequence is LDAA L0035;
 * ANDA $04; ORAA L0040; STAA L0040, so existing L0040 bits are retained. */
static void bua_lcd05_diagnostic_merge(void)
{
    DIAG_MODE_WORD2 = (bua_u8)(DIAG_MODE_WORD2 |
                               (MINOR_MODE_WORD2 & 0x04u));
}
/* LCD05..LCD22: source-ordered slow-O2 filter front end.  LF2C6 performs
 * the 16-count-spaced interpolation of LC58B using L00D2.  The source then
 * forms min(255, coolant + high_byte(coolant * LC565)), multiplies that byte
 * by the interpolated coefficient, and passes the high product byte to
 * LF250.  L006F is the whole-byte input and L0071 is the 8.8 filter state. */
static void bua_slow_o2_filter_12p5ms(void)
{
    bua_u8 table_coef;
    bua_u8 coolant_coef;
    bua_u8 final_coef;
    bua_u16 product;
    bua_u16 sum;
    table_coef = bua_lookup_spacing16(cal_o2_slow_filter_coef, 9u,
                                      O2_LOOKUP_AIRFLOW);
    product = (bua_u16)((bua_u16)COOLANT *
                        (bua_u16)CAL_O2_COLD_FILTER_COEF);
    sum = (bua_u16)COOLANT + (bua_u16)(product >> 8);
    if (sum > 255u)
        coolant_coef = 255u;
    else
        coolant_coef = (bua_u8)sum;
    product = (bua_u16)((bua_u16)coolant_coef * (bua_u16)table_coef);
    final_coef = (bua_u8)(product >> 8);
    ram16be_set(0x0071u,
                bua_lag_filter_8_8(ram16be_get(0x0071u),
                                   O2_FILTERED_FAST, final_coef));
    ++stats.slow_o2_filter_calls;
}
/* LCD4B..LCDDD: reference qualification / engine-running portion of the
 * even 12.5-ms spark path.  Spark-feedback, stall-saver hardware writes,
 * and the later dwell/spark tables remain for subsequent translation. */
static void bua_reference_state_12p5ms(void)
{
    bua_u16 ref_period;
    bua_u16 dwell;
    bua_u16 decayed;
    ref_period = ram16be_get(REF_PERIOD_ADDR);
    /* Before ENGINE RUNNING is established, reference period must be faster
     * than the LC018 threshold.  Otherwise LCD4B clears b2 and the startup
     * qualification counter. */
    if ((ENGINE_MODE_WORD & ENGINE_RUNNING_BIT) == 0u &&
        ref_period >= CAL_STARTUP_REF_PERIOD) {
        MINOR_MODE_WORD2 &= (bua_u8)~REF_SEEN_BIT;
        STARTUP_COUNTER = 0u;
        return;
    }
    if ((MINOR_MODE_WORD2 & REF_SEEN_BIT) != 0u) {
        MINOR_MODE_WORD2 &= (bua_u8)~REF_SEEN_BIT;
        /* LCD5F..LCD74: DYNAMIC DWELL = old - old/8 - 1, with negative
         * result clamped to zero.  Expressed via the exact COMD/add sequence. */
        dwell = ram16be_get(DYNAMIC_DWELL_ADDR);
        decayed = (bua_u16)((bua_u16)~(dwell >> 3) + dwell);
        if ((decayed & 0x8000u) != 0u)
            decayed = 0u;
        ram16be_set(DYNAMIC_DWELL_ADDR, decayed);
        NO_REF_MINOR_COUNT = 0u;
        if ((ENGINE_MODE_WORD & ENGINE_RUNNING_BIT) == 0u) {
            if ((IGN_MODE_WORD & SECOND_REF_VALID_BIT) == 0u) {
                IGN_MODE_WORD |= SECOND_REF_VALID_BIT;
                return;
            }
            if (STARTUP_COUNTER < CAL_STARTUP_LOOPS) {
                STARTUP_COUNTER = (bua_u8)(STARTUP_COUNTER + 1u);
                return;
            }
            ENGINE_MODE_WORD |= ENGINE_RUNNING_BIT;
            RAM8(0x003Bu) &= (bua_u8)~0x02u;
            RAM8(0x00B0u) = 0u;
            RAM8(0x0001u) |= 0x08u;
            ++stats.engine_run_transitions;
        }
        /* LCDDD is reached after startup qualification as well as during
         * ordinary running.  The RPM calculation is therefore an every-even-
         * loop operation once ENGINE RUNNING is set, not only a new-edge task. */
        if ((ENGINE_MODE_WORD & ENGINE_RUNNING_BIT) != 0u &&
            sim_ecm_ref.period_valid != 0u) {
            bua_calculate_rpm_from_reference();
            bua_compute_dwell_12p5ms();
        }
        return;
    }
    /* LCDC1..LCDCD only counts missing references after ENGINE RUNNING.
     * The original reaches a SWI wait at 23 loops.  We record that boundary
     * but deliberately do not invent the interrupt-side recovery yet. */
    if ((ENGINE_MODE_WORD & ENGINE_RUNNING_BIT) != 0u) {
        if (NO_REF_MINOR_COUNT >= CAL_NO_REF_WAIT_LOOPS) {
            ++stats.reference_wait_events;
            return;
        }
        NO_REF_MINOR_COUNT = (bua_u8)(NO_REF_MINOR_COUNT + 1u);
        if (sim_ecm_ref.period_valid != 0u) {
            bua_calculate_rpm_from_reference();
            bua_compute_dwell_12p5ms();
        }
    }
}
static void spark_vss_12p5ms(void)
{
    ++stats.spark_vss_loops;
    bua_lcd05_diagnostic_merge();
    bua_slow_o2_filter_12p5ms();
    bua_reference_state_12p5ms();
    if(sim_legacy_ignition_shutdown_freeze==0u)
        bua_iac_shutdown_homing_even_step111();
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
/* ---------------------------------------------------------------------- */
/* LDEA4 - called every 6.25 ms: check whether the VSS capture changed.    */
/* ---------------------------------------------------------------------- */
static void bua_vss_capture_check_6p25ms(void)
{
    bua_u16 capture;
    bua_u8 b;
    capture = sim_vss_capture_reg;
    if ((bua_u16)(capture - ram16be_get(VSS_TIME_NEW)) == 0u) {
        b = (bua_u8)(VSS_NO_EDGE_COUNT + 1u);
        if (b > 0x9Fu) {
            ENGINE_MODE_WORD &= (bua_u8)~0x10u;
            b = 0xA0u;
        }
        VSS_NO_EDGE_COUNT = b;
        return;
    }
    ++DISPLAY_DISTANCE_COUNT;
    ++stats.vss_capture_changes;
    if ((ENGINE_MODE_WORD & 0x10u) == 0u) {
        ENGINE_MODE_WORD |= 0x10u;
        ram16be_set(VSS_TIME_NEW, (bua_u16)(capture + 1u));
        ram16be_set(VSS_TIME_MID, (bua_u16)(capture + 2u));
    }
    VSS_NO_EDGE_COUNT = 0u;
    ram16be_set(VSS_TIME_OLD, ram16be_get(VSS_TIME_MID));
    ram16be_set(VSS_TIME_MID, ram16be_get(VSS_TIME_NEW));
    ram16be_set(VSS_TIME_NEW, capture);
}
/* ---------------------------------------------------------------------- */
/* LE07F - actual BUA major-loop segment 2: calculate road speed.          */
/* ---------------------------------------------------------------------- */
static void bua_major_seg2_vss(void)
{
    bua_u16 d1;
    bua_u16 d2;
    bua_u32 sum17;
    bua_u16 average_period;
    bua_u16 quotient;
    bua_u8 new_mph;
    bua_u16 filtered;
    bua_u16 d;
    bua_u8 a;
    bua_u8 scaled_a;
    /* b4 of L0033: ignition off. */
    if ((IGN_MODE_WORD & 0x10u) != 0u)
        return;
    if (VSS_NO_EDGE_COUNT >= 160u) {
        ram16be_set(0x0065u, 0u);
        d = 0u;
        a = 0u;
    } else {
        d1 = (bua_u16)(ram16be_get(VSS_TIME_MID) -
                       ram16be_get(VSS_TIME_OLD));
        d2 = (bua_u16)(ram16be_get(VSS_TIME_NEW) -
                       ram16be_get(VSS_TIME_MID));
        /* RORA/RORB after ADDD preserves the carry, i.e. 17-bit / 2. */
        sum17 = (bua_u32)d1 + (bua_u32)d2;
        average_period = (bua_u16)(sum17 >> 1);
        quotient = bua_fractional_divide(0x01CCu, average_period);
        new_mph = (bua_u8)(quotient >> 8);
        filtered = bua_lag_filter_8_8(ram16be_get(0x0065u),
                                      new_mph,
                                      (bua_u8)CAL_VSS_FILTER_COEF);
        ram16be_set(0x0065u, filtered);
        /* Literal translation of LE0AF..LE0C7 conversion for L0067. */
        d = (bua_u16)(filtered + 0x0080u);
        a = (bua_u8)(d >> 8);
        if (d < filtered)
            a = (bua_u8)(a - 1u);
        d = (bua_u16)((bua_u16)a * 0x00CDu);
        d = (bua_u16)(d + 0x0020u);
        if ((bua_u8)(d >> 8) > 0x2Fu)
            d = 0x3000u;
        d = (bua_u16)(d << 1);
        d = (bua_u16)(d << 1);
        a = (bua_u8)(d >> 8);
    }
    if (VSS_ERROR_COUNT > (bua_u8)CAL_VSS_ERR_TIME)
        a = (bua_u8)(CAL_CMD_SPEED_PARAM + 1u);
    scaled_a = a;
    VSS_SCALED = scaled_a;
}
/* ---------------------------------------------------------------------- */
/* Optional $5800 H.U. expansion hooks.                                    */
/*                                                                          */
/* BUA tests [$5800] for the word $7E58 and, if present, calls entry points */
/* at $5800, $5803, $5806, ... $581E.  Those routines are not contained in */
/* the supplied BUA image, so the PC build cannot translate their internals.*/
/* ---------------------------------------------------------------------- */
static void bua_hu_rom_5806(void)
{
    ++stats.hu_5806_calls;
}
/* LF8F9 - major segment 4 exactly reduces to this dispatch in the BUA ROM. */
static void seg4_log_ram_to_hu(void)
{
    if ((IGN_MODE_WORD & 0x80u) != 0u)
        bua_hu_rom_5806();
}
/* ---------------------------------------------------------------------- */
/* Normal 160-baud path: literal LF880 + LCAA4/LCA2C/LCA3B behavior.       */
/*                                                                          */
/* At each ordinary 6.25-ms IRQ, LCAA4 schedules a match at +11 timer       */
/* counts, sets L0046 b0+b1, and forces the serial output low.               */
/* The +11 interrupt (LCA2C) clears b0, schedules the +143 match, and calls  */
/* LF880.  LF880 either loads a byte or shifts the current byte left.        */
/* If the shifted-out MSB is zero it raises the serial line at +11; if the   */
/* MSB is one the line remains low.  The +143 interrupt (LCA3B) clears b1    */
/* and raises the line unconditionally.  Thus the executable behavior is a  */
/* pulse-width-coded 6.25-ms cell, not an ordinary UART byte waveform.       */
/* ---------------------------------------------------------------------- */
#define CAL_NUM_CYL_CODE       0x00u
#define CAL_GPH_SCALE_FACTOR   122u
#define DISPLAY_TABLE_LENGTH   4u
#define SERIAL_FLAG_FIRST      0x01u
#define SERIAL_FLAG_SECOND     0x02u
#define SERIAL_OUTPUT_MASK     0x04u
#define SERIAL_T_FIRST         11u
#define SERIAL_T_SECOND        143u
#define SERIAL_T_CELL          205u
typedef struct {
    bua_u8 cell_number;
    bua_u8 table_index_before;
    bua_u8 bit_count_before;
    bua_u8 xmit_before;
    bua_u8 table_index_after;
    bua_u8 bit_count_after;
    bua_u8 xmit_after;
    bua_u8 line_at_first;
    bua_u8 shifted_data_valid;
    bua_u8 shifted_data_bit;
} BuaSerialCellTrace;
/* ---------------------------------------------------------------------- */
/* Independent PC receiver.                                               */
/*                                                                        */
/* It does not inspect ALCL_XMIT_BYTE, ALCL_TABLE_INDEX, or bit counters. */
/* It receives only physical line transitions with P4 timer timestamps.   */
/*                                                                        */
/* The observed transmitter has one pulse-width-coded framing cell, then  */
/* eight pulse-width-coded data cells.  A short low pulse is data 0 and   */
/* a long low pulse is data 1.  The first pulse of each 9-pulse group is  */
/* treated only as framing and is not assigned a semantic meaning.        */
/* ---------------------------------------------------------------------- */
#define RX_WIDTH_THRESHOLD ((SERIAL_T_FIRST + SERIAL_T_SECOND) / 2u)
static void rx_reset(void)
{
    memset(&rx, 0, sizeof(rx));
}
static void rx_low_edge(bua_u32 time_count)
{
    if (rx_enabled == 0u)
        return;
    rx.have_low_start = 1u;
    rx.low_start_time = time_count;
}
static void rx_high_edge(bua_u32 time_count)
{
    bua_u32 width;
    bua_u8 bit;
    if (rx_enabled == 0u || rx.have_low_start == 0u)
        return;
    width = time_count - rx.low_start_time;
    rx.have_low_start = 0u;
    ++stats.receiver_pulses;
    if (!((width == SERIAL_T_FIRST) || (width == SERIAL_T_SECOND)))
        ++stats.receiver_bad_widths;
    if (rx.pulse_in_frame == 0u) {
        if (rx.byte_count < (bua_u8)(sizeof(rx.frame_widths) / sizeof(rx.frame_widths[0])))
            rx.frame_widths[rx.byte_count] = (bua_u16)width;
        rx.assembling = 0u;
        rx.pulse_in_frame = 1u;
        ++stats.receiver_frames;
        return;
    }
    bit = (width > (bua_u32)RX_WIDTH_THRESHOLD) ? 1u : 0u;
    rx.assembling = (bua_u8)((rx.assembling << 1) | bit);
    ++rx.pulse_in_frame;
    if (rx.pulse_in_frame == 9u) {
        if (rx.byte_count < (bua_u8)sizeof(rx.bytes))
            rx.bytes[rx.byte_count] = rx.assembling;
        ++rx.byte_count;
        ++stats.receiver_bytes;
        rx.pulse_in_frame = 0u;
    }
}
static void serial_line_low_at(bua_u32 time_count)
{
    if (sim_serial_line_high != 0u) {
        sim_serial_line_high = 0u;
        rx_low_edge(time_count);
    }
}
static void serial_line_high_at(bua_u32 time_count)
{
    if (sim_serial_line_high == 0u) {
        sim_serial_line_high = 1u;
        rx_high_edge(time_count);
    }
}
/* LF8B6/LF8E6 byte manager, normal non-diagnostic path only. */
static void bua_160_load_next_normal_byte(void)
{
    bua_u8 index;
    bua_u8 value;
    index = ALCL_TABLE_INDEX;
    if (index == 0u) {
        /* BEQ LF8ED leaves A holding L0035. */
        value = MINOR_MODE_WORD2;
    } else {
        if (index > DISPLAY_TABLE_LENGTH) {
            /* LF8CF: CLR A; table index=0; DECA; xmit byte=$FF; exit. */
            ALCL_TABLE_INDEX = 0u;
            ALCL_XMIT_BYTE = 0xFFu;
            return;
        }
        switch (index) {
            case 1u: value = (bua_u8)CAL_NUM_CYL_CODE; break;
            case 2u: value = RAM8(0x011Au);            break;
            case 3u: value = RAM8(0x011Eu);            break;
            default: value = (bua_u8)CAL_GPH_SCALE_FACTOR; break;
        }
    }
    ALCL_XMIT_BYTE = value;
    ALCL_TABLE_INDEX = (bua_u8)(index + 1u);
    ++stats.display_bytes_loaded;
}
/*
 * LF880, restricted only by design to the normal path used when L0047 b7 is
 * clear and L0035 b4/b5 are clear.  Diagnostic and factory branches remain
 * for later steps rather than being guessed here.
 *
 * Return value:
 *   -1 : no data bit was shifted this call (byte load/reset cell)
 *    0 : shifted-out MSB was zero
 *    1 : shifted-out MSB was one
 */
static int bua_lf880_normal(bua_u32 event_time)
{
    bua_u8 old_msb;
    ++stats.serial_manager_calls;
    if (ALCL_BIT_COUNT != 0u) {
        ALCL_BIT_COUNT = (bua_u8)(ALCL_BIT_COUNT - 1u);
        old_msb = (bua_u8)((ALCL_XMIT_BYTE & 0x80u) != 0u);
        ALCL_XMIT_BYTE = (bua_u8)(ALCL_XMIT_BYTE << 1);
        if (old_msb == 0u)
            serial_line_high_at(event_time);
        return (old_msb != 0u) ? 1 : 0;
    }
    ALCL_BIT_COUNT = 8u;
    /* L0047 b7 / L0048 select another path not yet translated. */
    if ((RAM8(0x0047u) & 0x80u) != 0u)
        return -1;
    /* LF8B6: normal display manager only while diagnostic switch bits clear. */
    if ((MINOR_MODE_WORD2 & 0x30u) != 0u)
        return -1;
    bua_160_load_next_normal_byte();
    /* Ordinary LF8ED path raises the line at +11.  LF8CF reset exits early. */
    if (ALCL_TABLE_INDEX != 0u)
        serial_line_high_at(event_time);
    return -1;
}
/* One complete 6.25-ms cell, corresponding to LCAA4 then LCA2C then LCA3B. */
static void bua_160_cell_at(BuaSerialCellTrace *trace, bua_u32 cell_start)
{
    int bit;
    if (trace != 0) {
        trace->table_index_before = ALCL_TABLE_INDEX;
        trace->bit_count_before = ALCL_BIT_COUNT;
        trace->xmit_before = ALCL_XMIT_BYTE;
        trace->shifted_data_valid = 0u;
        trace->shifted_data_bit = 0u;
    }
    /* LCAA4: first match +11; b0+b1 set; serial line goes low now. */
    SERIAL_MODE_WORD |= (SERIAL_FLAG_FIRST | SERIAL_FLAG_SECOND);
    serial_line_low_at(cell_start);
    /* LCA2C occurs at cell_start + 11 in this normalized trace. */
    SERIAL_MODE_WORD &= (bua_u8)~SERIAL_FLAG_FIRST;
    bit = bua_lf880_normal(cell_start + (bua_u32)SERIAL_T_FIRST);
    if (trace != 0) {
        trace->line_at_first = sim_serial_line_high;
        if (bit >= 0) {
            trace->shifted_data_valid = 1u;
            trace->shifted_data_bit = (bua_u8)bit;
        }
    }
    /* LCA3B: later match; line high regardless of data. */
    SERIAL_MODE_WORD &= (bua_u8)~SERIAL_FLAG_SECOND;
    serial_line_high_at(cell_start + (bua_u32)SERIAL_T_SECOND);
    ++stats.serial_cells;
    if (trace != 0 && trace->line_at_first != 0u)
        ++stats.serial_short_low_cells;
    else
        ++stats.serial_long_low_cells;
    if (trace != 0) {
        trace->table_index_after = ALCL_TABLE_INDEX;
        trace->bit_count_after = ALCL_BIT_COUNT;
        trace->xmit_after = ALCL_XMIT_BYTE;
    }
}
static void print_serial_trace(void)
{
    BuaSerialCellTrace tr;
    bua_u8 original_index;
    bua_u8 original_bits;
    bua_u8 original_xmit;
    bua_u8 original_serial_word;
    bua_u8 original_line;
    bua_u32 original_loaded;
    bua_u32 original_cells;
    bua_u32 original_short;
    bua_u32 original_long;
    bua_u32 original_calls;
    bua_u8 original_rx_enabled;
    unsigned int i;
    /* Preserve scheduler state; create a clean normal-stream observation. */
    original_index = ALCL_TABLE_INDEX;
    original_bits = ALCL_BIT_COUNT;
    original_xmit = ALCL_XMIT_BYTE;
    original_serial_word = SERIAL_MODE_WORD;
    original_line = sim_serial_line_high;
    original_loaded = stats.display_bytes_loaded;
    original_cells = stats.serial_cells;
    original_short = stats.serial_short_low_cells;
    original_long = stats.serial_long_low_cells;
    original_calls = stats.serial_manager_calls;
    original_rx_enabled = rx_enabled;
    ALCL_TABLE_INDEX = 0u;
    ALCL_BIT_COUNT = 0u;
    ALCL_XMIT_BYTE = 0xFFu;
    SERIAL_MODE_WORD &= (bua_u8)~0x03u;
    sim_serial_line_high = 1u;
    rx_enabled = 0u;
    printf("160-baud normal-path waveform trace (P4 timer counts):\n");
    printf("  Each cell: line LOW at +0, LF880 at +%u, forced HIGH at +%u, next cell +%u\n",
           (unsigned int)SERIAL_T_FIRST,
           (unsigned int)SERIAL_T_SECOND,
           (unsigned int)SERIAL_T_CELL);
    printf("  A data 0 raises the line at +11; a data 1 stays low until +143.\n\n");
    printf(" cell  idx/bits before  xmit  action        low-until  idx/bits after xmit\n");
    /* Five loaded bytes use 45 cells.  LF8CF then creates an $FF byte
     * with bit-count=8; trace its eight shifted bits and the following
     * load cell as well: 45 + 1 + 8 + 1 = 55 cells. */
    for (i = 0u; i < 55u; ++i) {
        memset(&tr, 0, sizeof(tr));
        tr.cell_number = (bua_u8)i;
        bua_160_cell_at(&tr, (bua_u32)i * (bua_u32)SERIAL_T_CELL);
        printf(" %3u    %u/%u           %02X    ",
               i,
               (unsigned int)tr.table_index_before,
               (unsigned int)tr.bit_count_before,
               (unsigned int)tr.xmit_before);
        if (tr.shifted_data_valid != 0u)
            printf("data bit %u     +%3u      ",
                   (unsigned int)tr.shifted_data_bit,
                   (unsigned int)(tr.shifted_data_bit ? SERIAL_T_SECOND : SERIAL_T_FIRST));
        else if (tr.table_index_before > DISPLAY_TABLE_LENGTH && tr.bit_count_before == 0u)
            printf("table reset    +%3u      ", (unsigned int)SERIAL_T_SECOND);
        else
            printf("load/framing   +%3u      ", (unsigned int)SERIAL_T_FIRST);
        printf("%u/%u          %02X\n",
               (unsigned int)tr.table_index_after,
               (unsigned int)tr.bit_count_after,
               (unsigned int)tr.xmit_after);
    }
    ALCL_TABLE_INDEX = original_index;
    ALCL_BIT_COUNT = original_bits;
    ALCL_XMIT_BYTE = original_xmit;
    SERIAL_MODE_WORD = original_serial_word;
    sim_serial_line_high = original_line;
    stats.display_bytes_loaded = original_loaded;
    stats.serial_cells = original_cells;
    stats.serial_short_low_cells = original_short;
    stats.serial_long_low_cells = original_long;
    stats.serial_manager_calls = original_calls;
    rx_enabled = original_rx_enabled;
}
static void print_receiver_round_trip(void)
{
    bua_u8 expected[6];
    bua_u8 saved_index;
    bua_u8 saved_bits;
    bua_u8 saved_xmit;
    bua_u8 saved_serial_word;
    bua_u8 saved_line;
    BuaStats saved_stats;
    unsigned int i;
    unsigned int pass_count;
    expected[0] = MINOR_MODE_WORD2;
    expected[1] = (bua_u8)CAL_NUM_CYL_CODE;
    expected[2] = RAM8(0x011Au);
    expected[3] = RAM8(0x011Eu);
    expected[4] = (bua_u8)CAL_GPH_SCALE_FACTOR;
    expected[5] = 0xFFu;
    saved_index = ALCL_TABLE_INDEX;
    saved_bits = ALCL_BIT_COUNT;
    saved_xmit = ALCL_XMIT_BYTE;
    saved_serial_word = SERIAL_MODE_WORD;
    saved_line = sim_serial_line_high;
    saved_stats = stats;
    ALCL_TABLE_INDEX = 0u;
    ALCL_BIT_COUNT = 0u;
    ALCL_XMIT_BYTE = 0xFFu;
    SERIAL_MODE_WORD &= (bua_u8)~0x03u;
    sim_serial_line_high = 1u;
    rx_reset();
    rx_enabled = 1u;
    /* Six complete 9-cell groups = five normal values plus literal $FF group. */
    for (i = 0u; i < 54u; ++i)
        bua_160_cell_at(0, (bua_u32)i * (bua_u32)SERIAL_T_CELL);
    rx_enabled = 0u;
    printf("Independent line receiver round-trip:\n");
    printf("  Receiver input: serial transitions + P4 timer timestamps only\n");
    printf("  Pulse threshold: <=%u => 0, >%u => 1; data assembled MSB-first\n",
           (unsigned int)RX_WIDTH_THRESHOLD,
           (unsigned int)RX_WIDTH_THRESHOLD);
    printf("\n  group  frame-low  decoded  expected  result\n");
    pass_count = 0u;
    for (i = 0u; i < 6u; ++i) {
        int pass;
        pass = (i < (unsigned int)rx.byte_count && rx.bytes[i] == expected[i]);
        if (pass)
            ++pass_count;
        printf("   %u      %3u       %02X       %02X     %s\n",
               i,
               (unsigned int)rx.frame_widths[i],
               (unsigned int)((i < (unsigned int)rx.byte_count) ? rx.bytes[i] : 0u),
               (unsigned int)expected[i],
               pass ? "PASS" : "FAIL");
    }
    printf("\n  pulses=%lu frames=%lu bytes=%lu badWidths=%lu\n",
           (unsigned long)(stats.receiver_pulses - saved_stats.receiver_pulses),
           (unsigned long)(stats.receiver_frames - saved_stats.receiver_frames),
           (unsigned long)(stats.receiver_bytes - saved_stats.receiver_bytes),
           (unsigned long)(stats.receiver_bad_widths - saved_stats.receiver_bad_widths));
    printf("  round-trip result: %s (%u/6 bytes match)\n",
           (pass_count == 6u && rx.byte_count == 6u) ? "PASS" : "FAIL",
           pass_count);
    ALCL_TABLE_INDEX = saved_index;
    ALCL_BIT_COUNT = saved_bits;
    ALCL_XMIT_BYTE = saved_xmit;
    SERIAL_MODE_WORD = saved_serial_word;
    sim_serial_line_high = saved_line;
    stats = saved_stats;
}
/* Named placeholders matching the remaining actual BUA major-loop table. */
static void seg0_nop(void)                 { }
static void seg1_output_bits(void);
static void seg3_misc_100ms(void);
static void seg5_ac_closed_loop_fan(void);
static void seg6_coolant_adc(void);
static void seg7_coolant_variables(void);
static void seg8_nop(void)                 { }
static void seg9_inj_air_management(void);
static void segA_mat_variables(void);
static void segB_egr(void);
static void segC_canister_purge(void);
/* Defined after the diagnostic source units have been included by main.c. */
static void segD_diagnostics(void);
static void segE_tcc_adc(void);
static void segF_fuel_air_major(void);
static void major_segment(bua_u8 seg)
{
    seg &= 0x0Fu;
    ++stats.major_segment_calls[seg];
    switch (seg) {
        case 0x0u: seg0_nop();                break;
        case 0x1u: seg1_output_bits();        break;
        case 0x2u: bua_major_seg2_vss();      break;
        case 0x3u: seg3_misc_100ms();         break;
        case 0x4u: seg4_log_ram_to_hu();      break;
        case 0x5u: seg5_ac_closed_loop_fan(); break;
        case 0x6u: seg6_coolant_adc();        break;
        case 0x7u: seg7_coolant_variables();  break;
        case 0x8u: seg8_nop();                break;
        case 0x9u: seg9_inj_air_management(); break;
        case 0xAu: segA_mat_variables();      break;
        case 0xBu: segB_egr();                break;
        case 0xCu: segC_canister_purge();     break;
        case 0xDu:
            if(sim_legacy_segment_d_freeze==0u)
                segD_diagnostics();
            break;
        case 0xEu: segE_tcc_adc();            break;
        default:   segF_fuel_air_major();      break;
    }
}
static void ecm_reset(void)
{
    memset(&mem, 0, sizeof(mem));
    memset(&stats, 0, sizeof(stats));
    sim_legacy_segment_d_freeze = 0u;
    sim_legacy_segment1_output_freeze = 0u;
    sim_legacy_ignition_shutdown_freeze = 0u;
    sim_soft_powerdown_latched = 0u;
    sim_timer8 = 0u;
    sim_iac_motor_on = 1u;
    sim_maf_adc = 128u;
    sim_o2_adc = 128u;
    sim_tps_adc = 35u;
    sim_diag_adc = 200u;
    bua_step94_fuel_state_reset();
    sim_vss_abs_ticks = 0ul;
    sim_vss_next_edge = 0ul;
    sim_vss_capture_reg = 0u;
    sim_vss_period_ticks = 0u;
    sim_serial_line_high = 1u;
    sim_vehicle_time_us = 0ul;
    memset(&sim_ecm_ref, 0, sizeof(sim_ecm_ref));
    memset(&dash, 0, sizeof(dash));
    memset(&sim_dash_vss, 0, sizeof(sim_dash_vss));
    memset(&sim_dash_tach, 0, sizeof(sim_dash_tach));
    rx_enabled = 0u;
    rx_reset();
    mem.io5000 = 0x08u;
    MINOR_COUNT = 0x0Eu;
    BLM = 128u;
    /* LF434 cold-start initialization: all sixteen BLM cells receive 120. */
    memset(&mem.low[0x001Cu], 120, 16u);
    RAM8(0x00C6u) = 128u;
    RAM8(0x00C9u) = 128u;
    ALCL_TABLE_INDEX = 0x19u;
    VSS_NO_EDGE_COUNT = 160u;
    RAM8(0x002Cu) = 144u;
    RAM8(0x010Cu) = 144u;
    ram16be_set(0x00CEu, 445u);
    RAM8(0x007Eu) = hw_adc(0x10u);
    RAM8(0x0081u) = hw_adc(0x50u);
    /* Cold reset leaves ENGINE RUNNING clear; LCD4B..LCD8F qualifies it. */
}
static void irq_6p25ms(void)
{
    bua_u8 count;
    if(sim_soft_powerdown_latched!=0u)
        return;
    ++stats.irq_ticks;
    sim_advance_dash_signals_6p25ms();
    sim_advance_ecm_reference_6p25ms();
    sim_advance_vss_6p25ms();
    bua_vss_capture_check_6p25ms();
    count = (bua_u8)(MINOR_COUNT + 1u);
    if (count == 160u) {
        one_second_event();
        count = 0u;
    }
    MINOR_COUNT = count;
    TIMER_SNAPSHOT = hw_timer8();
    /* LCC21 calls LF4DF before the odd/even minor-loop dispatch. */
    bua_iac_motor_service_6p25ms();
    /* LCC21 reads/normalizes TPS and updates its transient filter. */
    bua_common_tps_6p25ms();
    /* LF67B services an MPU-reported injection before falling into LF76A. */
    bua_common_injector_service_6p25ms();
    /* LF76A completes before LCC65 selects the odd/even minor path. */
    bua_common_maf_6p25ms();
    if ((count & 1u) != 0u)
        air_fuel_12p5ms();
    else
        spark_vss_12p5ms();
    if(sim_soft_powerdown_latched!=0u)
        return;
    major_segment((bua_u8)(count & 0x0Fu));
}
/* ---------------------------------------------------------------------- */
/* Phase-continuous dynamic vehicle drive-cycle stimulus.          */
/*                                                                        */
/* The command profile is evaluated every 6.25 ms.  VSS and HEI waveform  */
/* generators change frequency without resetting phase.  This avoids the  */
/* Step-8 boundary artifact and better represents a physical rotating      */
/* source whose pulse spacing continuously changes during acceleration.    */
/*                                                                        */
/* The profile itself remains a PC test stimulus, not a vehicle-dynamics   */
/* model and not a claim about original cluster filtering.                 */
/* ---------------------------------------------------------------------- */
typedef struct {
    unsigned int duration_ticks;  /* 6.25 ms ticks */
    unsigned int start_speed_mph;
    unsigned int end_speed_mph;
    unsigned int start_rpm;
    unsigned int end_rpm;
    unsigned int start_tps_adc;
    unsigned int end_tps_adc;
    unsigned int start_maf_adc;
    unsigned int end_maf_adc;
    bua_u8 in_drive;
    const char *name;
} DriveProfileSegment;
static const DriveProfileSegment drive_profile[] = {
    {160u,  0u,  0u,  350u,  800u, 38u,35u, 50u, 60u,0u,"cranking / catch"},
    {320u,  0u,  0u,  800u,  800u, 35u,35u, 55u, 55u,0u,"warm idle"},
    {960u,  0u, 60u,  800u, 2600u, 75u,110u,55u,170u,1u,"acceleration"},
    {640u, 60u, 60u, 2200u, 2200u, 60u,60u,110u,110u,1u,"60 MPH cruise"},
    {320u, 60u, 45u, 2200u, 1800u, 35u,35u,  0u,  0u,1u,"throttle lift / DFCO"},
    {640u, 45u,  0u, 1800u,  800u, 35u,35u,  0u, 55u,1u,"deceleration"},
    {1920u, 0u,  0u,  800u,  800u, 35u,35u, 55u, 55u,0u,"returned idle"}
};
#define DRIVE_PROFILE_COUNT ((unsigned int)(sizeof(drive_profile) / sizeof(drive_profile[0])))
static unsigned int interpolate_u16(unsigned int start_value,
                                    unsigned int end_value,
                                    unsigned int tick,
                                    unsigned int duration_ticks)
{
    long delta;
    long value;
    long numerator;
    if (duration_ticks <= 1u)
        return end_value;
    delta = (long)end_value - (long)start_value;
    numerator = delta * (long)tick;
    if (numerator >= 0l)
        numerator += (long)(duration_ticks - 1u) / 2l;
    else
        numerator -= (long)(duration_ticks - 1u) / 2l;
    value = (long)start_value + numerator / (long)(duration_ticks - 1u);
    if (value < 0l)
        value = 0l;
    return (unsigned int)value;
}
static void dash_apply_no_pulse_timeout(void)
{
    /* PC receiver convenience only: after 1 second without a new edge,
     * report zero speed/RPM.  This is NOT claimed to reproduce the 1986
     * cluster firmware timeout. */
    if (dash.vss_valid != 0u &&
        (sim_vehicle_time_us - dash.vss_last_rise_us) >= 1000000ul)
        dash.speed_mph = 0u;
    if (dash.tach_valid != 0u &&
        (sim_vehicle_time_us - dash.tach_last_rise_us) >= 1000000ul)
        dash.rpm = 0u;
}
static void apply_dynamic_command(unsigned int speed_mph, unsigned int rpm,
                                  unsigned int tps_adc,unsigned int maf_adc,
                                  bua_u8 in_drive)
{
    /* All three generators receive the same vehicle command but retain
     * independent timing state.  No decoder state is cleared here. */
    sim_set_vss_mph(speed_mph);
    sim_set_dash_vss_mph(speed_mph);
    sim_set_dash_tach_rpm(rpm);
    sim_set_ecm_reference_rpm(rpm);
    sim_tps_adc=(bua_u8)tps_adc;
    sim_maf_adc=(bua_u8)maf_adc;
    if(in_drive!=0u)
        RAM8(0x0037u)&=(bua_u8)~0x01u;
    else
        RAM8(0x0037u)|=0x01u;
}
/* ---------------------------------------------------------------------- */
/* Step 26: differential-load AE application and ordinary fuel output.    */
/*                                                                        */
/* The limiter subpaths entered through LF9E5/F9FC are deliberately not   */
/* flattened here.  This helper translates the no-limiter ordinary path,  */
/* plus the explicit hard fuel-cut gates and pulse-width conditioning.     */
/* ---------------------------------------------------------------------- */
typedef struct {
    bua_u16 corrected_base_pw;
    bua_u16 pending_pw;
    bua_u16 hardware_pw;
    bua_u8 ae_factor;
    bua_u8 ae_flags;
    bua_u8 af_mode_word;
    bua_u8 ign_mode_word;
    bua_u8 fuel_cut;
    bua_u8 low_pw_forced;
    bua_u8 small_pw_offset;
} BuaFuelOutputResult;
static bua_u8 bua_small_pw_offset_lf2b9(bua_u16 pending_pw)
{
    bua_u8 arg;
    if (pending_pw >= 256u)
        return 0u;
    arg = (bua_u8)pending_pw;
    if (arg < 32u)
        arg = 0u;
    else
        arg = (bua_u8)(arg - 32u);
    return bua_lookup_spacing16(cal_small_pw_offset, 14u, arg);
}
static bua_u16 bua_ae_apply_lf901(bua_u16 base_pw,
                                  bua_u8 *ae_factor_io,
                                  bua_u8 ae_decay,
                                  bua_u8 *ae_flags_io)
{
    bua_u8 factor;
    bua_u8 flags;
    bua_u16 add;
    bua_u32 sum;
    factor = *ae_factor_io;
    flags = *ae_flags_io;
    if ((flags & AE_FIRST_TIME_BIT) != 0u &&
        (flags & AE_INJECT_SINCE_BIT) != 0u) {
        if (factor >= ae_decay) {
            factor = (bua_u8)(factor - ae_decay);
        } else {
            factor = 0u;
            flags &= (bua_u8)~AE_ACTIVE_BIT;
        }
    }
    flags &= (bua_u8)~AE_INJECT_SINCE_BIT;
    flags |= AE_FIRST_TIME_BIT;
    add = bua_mul_u8_u16_lf266(factor, base_pw);
    sum = (bua_u32)base_pw + (bua_u32)add;
    if (sum > 65535ul)
        sum = 65535ul;
    *ae_factor_io = factor;
    *ae_flags_io = flags;
    return (bua_u16)sum;
}
static BuaFuelOutputResult bua_fuel_output_lf92a(bua_u16 corrected_base_pw,
                                                 bua_u8 mode_word3,
                                                 bua_u8 ign_mode_word,
                                                 bua_u8 af_mode_word,
                                                 bua_u8 spark_mode_word,
                                                 bua_u8 single_fire_word,
                                                 bua_u8 speed_raw,
                                                 bua_u16 ref_period,
                                                 bua_u8 injector_batt_offset)
{
    BuaFuelOutputResult r;
    bua_u16 pending;
    bua_u16 shaped;
    bua_u8 small_offset;
    bua_u8 cut;
    bua_u8 high_cut_active;
    bua_u8 speed_limit;
    bua_u16 ref_limit;
    memset(&r, 0, sizeof(r));
    r.corrected_base_pw = corrected_base_pw;
    r.af_mode_word = af_mode_word;
    r.ign_mode_word = ign_mode_word;
    pending = corrected_base_pw;
    cut = 0u;
    if ((mode_word3 & FUEL_VATS_OK_BIT) == 0u ||
        (ign_mode_word & FUEL_IGN_OFF_BIT) != 0u ||
        (af_mode_word & FUEL_DFCO_ZERO_BIT) != 0u) {
        cut = 1u;
    } else if ((spark_mode_word & FUEL_ENGINE_RUNNING_BIT) != 0u) {
        high_cut_active = (bua_u8)((af_mode_word & FUEL_HI_CUTOFF_BIT) != 0u);
        if (high_cut_active != 0u) {
            speed_limit = (bua_u8)CAL_FUEL_CUTOFF_SPEED_REENABLE;
            ref_limit = (bua_u16)CAL_FUEL_CUTOFF_REF_REENABLE;
        } else {
            speed_limit = (bua_u8)CAL_FUEL_CUTOFF_SPEED_HI;
            ref_limit = (bua_u16)CAL_FUEL_CUTOFF_REF_HI;
        }
        r.af_mode_word &= (bua_u8)~FUEL_HI_CUTOFF_BIT;
        if (speed_raw > speed_limit || ref_period < ref_limit) {
            r.af_mode_word |= FUEL_HI_CUTOFF_BIT;
            cut = 1u;
        }
    }
    if (cut != 0u)
        pending = 0u;
    if ((single_fire_word & SINGLE_FIRE_BIT) != 0u &&
        (single_fire_word & SINGLE_FIRE_ZERO_PW_BIT) != 0u)
        pending = 0u;
    r.pending_pw = pending;
    r.fuel_cut = cut;
    if (pending == 0u) {
        r.hardware_pw = 0u;
        return r;
    }
    small_offset = bua_small_pw_offset_lf2b9(pending);
    shaped = pending;
    if (pending < 256u)
        shaped = (bua_u16)(shaped + (bua_u16)small_offset);
    shaped = (bua_u16)(shaped + (bua_u16)injector_batt_offset * 2u);
    r.small_pw_offset = small_offset;
    r.ign_mode_word &= (bua_u8)~FORCE_LOW_PW_BIT;
    if (shaped <= (bua_u16)CAL_MIN_BASE_PW) {
        shaped = (bua_u16)CAL_DEFAULT_BASE_PW;
        r.ign_mode_word |= FORCE_LOW_PW_BIT;
        r.low_pw_forced = 1u;
    }
    if (shaped >= 0x7FFFu)
        shaped = 0x7FFFu;
    r.hardware_pw = shaped;
    return r;
}
/* ---------------------------------------------------------------------- */
/* Step 27: LF95E/LF9E5 special fuel limiting.                            */
/*                                                                        */
/* LF9E5 converts LC331's packed nibbles to two 8-bit factors.  The high */
/* nibble is used while L0038 b4 is clear (trigger-to-next-inject); the  */
/* low nibble, shifted into bits 7..4, is used when b4 is set             */
/* (after-next-inject).  Entry at $F9FC skips the coolant unpacking and   */
/* uses LC5F8/LC5F9 = 128/128 directly.                                  */
/* ---------------------------------------------------------------------- */
typedef struct {
    bua_u16 input_pw;
    bua_u16 output_pw;
    bua_u16 allowed_fuel;
    bua_u16 supplied_fuel;
    bua_u16 excess_fuel;
    bua_u8 factor;
    bua_u8 packed_cal;
    bua_u8 applied;
    bua_u8 used_fixed_decel_pair;
} BuaFuelLimitResult;
static BuaFuelLimitResult bua_fuel_limit_lf95e(bua_u16 current_pw,
                                                bua_u16 last_inject_pw,
                                                bua_u16 accumulated_ae,
                                                bua_u8 coolant,
                                                bua_u8 l0038_flags,
                                                bua_u8 l003e_flags)
{
    BuaFuelLimitResult r;
    bua_u32 supplied32;
    bua_u16 scaled;
    bua_u8 trigger_factor;
    bua_u8 after_factor;
    bua_u8 packed;
    memset(&r, 0, sizeof(r));
    r.input_pw = current_pw;
    r.output_pw = current_pw;
    if ((l003e_flags & FUEL_LIMIT_DECEL_MASK) != 0u) {
        trigger_factor = (bua_u8)CAL_DECEL_LIMIT_TRIGGER;
        after_factor = (bua_u8)CAL_DECEL_LIMIT_AFTER;
        packed = 0u;
        r.used_fixed_decel_pair = 1u;
    } else if ((l0038_flags & FUEL_LIMIT_AE_MASK) != 0u) {
        packed = cal_fuel_limit_coolant[(bua_u8)(coolant >> 5)];
        trigger_factor = (bua_u8)(packed & 0xF0u);
        after_factor = (bua_u8)((packed & 0x0Fu) << 4);
    } else {
        return r;
    }
    r.applied = 1u;
    r.packed_cal = packed;
    if ((l0038_flags & FUEL_LIMIT_TRIGGER_STATE_BIT) != 0u)
        r.factor = after_factor;
    else
        r.factor = trigger_factor;
    scaled = bua_mul_u8_u16_lf266(r.factor, current_pw);
    if (scaled > 0x7FFFu)
        r.allowed_fuel = 0xFFFFu;
    else
        r.allowed_fuel = (bua_u16)(scaled << 1);
    supplied32 = (bua_u32)last_inject_pw + (bua_u32)accumulated_ae;
    if (supplied32 > 65535ul)
        r.supplied_fuel = 65535u;
    else
        r.supplied_fuel = (bua_u16)supplied32;
    if (r.supplied_fuel < r.allowed_fuel)
        return r;
    r.excess_fuel = (bua_u16)(r.supplied_fuel - r.allowed_fuel);
    if (r.excess_fuel > current_pw) {
        r.output_pw = 0u;
    } else {
        r.output_pw = (bua_u16)(current_pw - r.excess_fuel);
        if (r.output_pw < (bua_u16)CAL_MIN_BASE_PW)
            r.output_pw = 0u;
    }
    return r;
}
/* ---------------------------------------------------------------------- */
/* Step 29: injector-event state machine, LF71C..LF768.                    */
/*                                                                        */
/* Steps 29 and 98 keep this as a reusable event update.  Step 98 wires it */
/* into LF67B's common scheduler call point when the PC HAL supplies the   */
/* established once-per-revolution physical batch-service indication.     */
/* ---------------------------------------------------------------------- */
typedef struct {
    bua_u8 single_fire_word;
    bua_u8 air_mode_word;
    bua_u8 ae_flags;
    bua_u8 decel_flags;
    bua_u8 fuel_limit_inject_count;
    bua_u8 async_count;
    bua_u8 running_inject_counter;
    bua_u16 last_base_pw;
    bua_u16 ae_pw_accumulator;
    bua_u16 accumulated_ae;
    bua_u8 single_fire_zero_pw_toggled;
    bua_u8 round_toggle_changed;
    bua_u8 limiter_count_incremented;
    bua_u8 limiter_cleared;
    bua_u8 hold_gate_taken;
} BuaInjectorEventResult;
static BuaInjectorEventResult bua_injector_event_lf71c(
    bua_u8 single_fire_word_in,
    bua_u8 air_mode_word_in,
    bua_u8 ae_flags_in,
    bua_u8 decel_flags_in,
    bua_u8 engine_mode_word,
    bua_u16 base_pw,
    bua_u16 pending_fuel,
    bua_u16 ae_pw_accumulator_in,
    bua_u16 accumulated_ae_in,
    bua_u8 fuel_limit_inject_count_in,
    bua_u8 async_count_in,
    bua_u8 running_inject_counter_in)
{
    BuaInjectorEventResult r;
    bua_u8 next_count;
    bua_u8 limiter_active;
    memset(&r, 0, sizeof(r));
    r.single_fire_word = single_fire_word_in;
    r.air_mode_word = air_mode_word_in;
    r.ae_flags = ae_flags_in;
    r.decel_flags = decel_flags_in;
    r.fuel_limit_inject_count = fuel_limit_inject_count_in;
    r.async_count = async_count_in;
    r.running_inject_counter = running_inject_counter_in;
    r.accumulated_ae = accumulated_ae_in;
    /* LF71C: if L0039 b4 is set, toggle b6 (zero-PW half of SF pair). */
    if ((r.single_fire_word & SINGLE_FIRE_TOGGLE_ENABLE_BIT) != 0u) {
        r.single_fire_word ^= SINGLE_FIRE_ZERO_PW_BIT;
        r.single_fire_zero_pw_toggled = 1u;
    }
    /* LF726: alternate L003D b0 every injector event. */
    r.air_mode_word ^= INJECT_ROUND_TOGGLE_BIT;
    r.round_toggle_changed = 1u;
    /* Mark an injector event and move the fuel bookkeeping forward. */
    r.ae_flags |= INJECT_EVENT_STATE_BITS;
    r.last_base_pw = base_pw;
    r.ae_pw_accumulator = (bua_u16)(ae_pw_accumulator_in + pending_fuel);
    /* L018D is an 8-bit saturating event counter while running. */
    if ((engine_mode_word & FUEL_ENGINE_RUNNING_BIT) != 0u) {
        r.running_inject_counter = (bua_u8)(r.running_inject_counter + 1u);
        if (r.running_inject_counter == 0u)
            r.running_inject_counter = 255u;
    }
    /* LF748/LF750: either transient limiter state keeps a two-inject count. */
    limiter_active = 0u;
    if ((r.decel_flags & DECEL_LIMIT_ACTIVE_BIT) != 0u ||
        (r.ae_flags & TPS_AE_LIMIT_BIT) != 0u)
        limiter_active = 1u;
    if (limiter_active != 0u) {
        next_count = (bua_u8)(r.fuel_limit_inject_count + 1u);
        r.limiter_count_incremented = 1u;
        if (next_count < (bua_u8)CAL_FUEL_LIMIT_INJECTS) {
            r.fuel_limit_inject_count = next_count;
            ++stats.injector_event_updates;
            return r;
        }
        /* Literal BRSET L003E,$80,LF76A occurs before STAB L00E0. */
        if ((r.decel_flags & DECEL_STATE_HOLD_BIT) != 0u) {
            r.hold_gate_taken = 1u;
            ++stats.injector_event_updates;
            return r;
        }
    } else if ((r.decel_flags & DECEL_STATE_HOLD_BIT) != 0u) {
        /* With no limiter active, b7 still bypasses the clearing block. */
        r.hold_gate_taken = 1u;
        ++stats.injector_event_updates;
        return r;
    }
    /* LF758: end the transient-fuel limiting window. */
    r.ae_flags &= (bua_u8)~TPS_AE_LIMIT_BIT;
    r.decel_flags &= (bua_u8)~DECEL_LIMIT_ACTIVE_BIT;
    r.async_count = 0u;
    r.accumulated_ae = 0u;
    r.fuel_limit_inject_count = 0u;
    r.limiter_cleared = 1u;
    ++stats.injector_event_updates;
    return r;
}
