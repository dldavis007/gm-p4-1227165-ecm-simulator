static void bua_write_lf92a(bua_u16 pulse_width);
static void bua_common_maf_6p25ms(void);
static void bua_common_tps_6p25ms(void);
static void bua_common_injector_service_6p25ms(void);
static void bua_odd_fuel_chain_12p5ms(void);
static void bua_odd_transient_front_12p5ms(void);
static void bua_odd_50ms_tail_lddc2(void);
static bua_u8 bua_ignition_shutdown_odd_step111(void);
static void bua_iac_shutdown_homing_even_step111(void);
static void bua_mode4_scheduler_step116(void);
static void air_fuel_12p5ms(void)
{
    ++stats.air_fuel_loops;
    if(bua_ignition_shutdown_odd_step111()==0u)
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
#define STEP69_PASSBY_TPS_MIN       255u
#define STEP69_PASSBY_VSS_MIN       28u
#define STEP69_PASSBY_VSS_MAX       32u
#define STEP69_PASSBY_RPM25_MIN     255u
#define STEP69_PASSBY_DELAY         65535u
#define STEP69_PASSBY_HOLD          240u
#define STEP69_PASSBY_NOISE_BIT     0x08u
typedef struct BuaPassby69 {
    bua_u16 delay;
    bua_u16 hold;
    bua_u8 status37;
    bua_u8 minor36;
    bua_u8 mode04;
    bua_u16 output_counter;
} BuaPassby69;
static bua_u8 bua_passby_quals69(bua_u8 tps,bua_u8 mph,bua_u8 rpm25)
{
    if(tps<STEP69_PASSBY_TPS_MIN) return 0u;
    if(mph<=STEP69_PASSBY_VSS_MIN) return 0u;
    if(mph>STEP69_PASSBY_VSS_MAX) return 0u;
    if(rpm25<STEP69_PASSBY_RPM25_MIN) return 0u;
    return 1u;
}
/* LCF1A..LCF85. Return true for every commanded MPU write, including
 * repeated writes of the same value while the hold timer runs. */
static int bua_passby_tick69(BuaPassby69 *s, bua_u8 tps, bua_u8 mph,
                             bua_u8 rpm25, bua_u8 battery, bua_u16 delay_cal)
{
    if (s->hold) {
        if (--s->hold == 0u) {
            s->mode04 |= 0x81u;
            s->minor36 &= (bua_u8)~STEP69_PASSBY_NOISE_BIT;
            return 0;
        }
    } else {
        if (!bua_passby_quals69(tps, mph, rpm25)) {
            s->delay = 0u;
            return 0;
        }
        if (s->delay <= delay_cal) {
            ++s->delay;
            return 0;
        }
        s->hold = STEP69_PASSBY_HOLD;
        s->delay = 0u;
    }
    if (battery >= 171u)
        return 0;
    s->output_counter = 0xDFFFu;
    s->status37 |= 0x20u;
    s->minor36 |= STEP69_PASSBY_NOISE_BIT;
    return 1;
}

static void bua_passby_service(void)
{
    BuaPassby69 s;
    s.delay = ram16be_get(0x123u);
    s.hold = ram16be_get(0x125u);
    s.status37 = RAM8(0x37u);
    s.minor36 = RAM8(0x36u);
    s.mode04 = RAM8(4u);
    if (bua_passby_tick69(&s, TPS_LOAD_AXIS, VSS_MPH_HI, RPM_DIV25,
                          BATTERY_AD, STEP69_PASSBY_DELAY))
        mpu16be_set(0x3FD6u, s.output_counter);
    ram16be_set(0x123u, s.delay);
    ram16be_set(0x125u, s.hold);
    RAM8(0x37u) = s.status37;
    RAM8(0x36u) = s.minor36;
    RAM8(4u) = s.mode04;
}

static void bua_tps_accel_trigger(void);

static void bua_running_spark_12p5ms(void)
{
    bua_u8 load, main_raw, cool_raw, highway, hot, test, magnitude;
    bua_u16 counter, delta;
    bua_calculate_rpm_from_reference();
    bua_stall_saver_update(RAM8(0x58u), RAM8(0x5Au));
    bua_compute_dwell_12p5ms();
    bua_passby_service();
    load = FILTERED_LOAD;
    if ((MODE_WORD1 & 0x40u) == 0u) {
        if (COOLANT >= 113u || (COOLANT >= STARTUP_COOLANT &&
            COOLANT - STARTUP_COOLANT >= 27))
            RAM8(0x3Bu) |= 0x40u;
        load = 208u;
    }
    main_raw = bua_lookup_main_spark_raw(RPM_SPECIAL, load > 208u ? 208u : load);
    cool_raw = bua_lookup_coolant_spark_raw(COOLANT, FILTERED_LOAD);
    highway = bua_highway_spark_eval(FILTERED_LOAD, COOLANT, RPM_SPECIAL,
                                     RAM8(0), RAM8(0xA6u), &RAM8(0xA6u));
    hot = bua_hot_restart_retard_eval(MODE_WORD1, STARTUP_COOLANT,
                                      RAM8(0x1Bu), &MODE_WORD1);
    RAM8(0x045Du) = main_raw;
    RAM8(0x045Eu) = cool_raw;
    RAM8(0x045Fu) = highway;
    RAM8(0x0460u) = hot;
    RAM8(0x3Au) = (bua_u8)((RAM8(0x3Au) & 0xCFu) |
                       (highway ? 0x10u : 0u) | (hot ? 0x20u : 0u));
    test = bua_knock_test_spark_eval(hot);
    bua_spark_sum_ld096(main_raw, cool_raw, highway, hot, test,
                        bua_aldl_added_spark_eval(), bua_startup_spark_sum_byte());
    bua_tps_accel_trigger(); /* LD0D1 calls LFA1E before sampling knock. */
    counter = mpu16be_get(0x3FCAu);
    delta = (bua_u16)(counter - ram16be_get(0xA1u));
    ram16be_set(0xA1u, counter);
    bua_knock_attack_eval(delta > 255u ? 255u : (bua_u8)delta);
    bua_spark_relative_to_reference_pre_mode4();
    if (delta != 0u) { /* LD16D consumes the saved knock-event count. */
        if (MODE_WORD2 & MODE2_ERR43_TEST_BIT)
            MODE_WORD2 = (bua_u8)((MODE_WORD2 & ~MODE2_ERR43_TEST_BIT) | 0x40u);
        MODE_WORD2 &= (bua_u8)~MODE2_ERR43_FAIL_BIT;
    }
    magnitude = bua_spark_mode4_blend_ld192();
    bua_spark_timing_output_ld1f6(magnitude);
    bua_est_output_finish_ld25x();
    (void)bua_read_fmd_byte2();
}
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
        bua_low_rpm_spark_reset_ld306();
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
                bua_low_rpm_spark_reset_ld306();
                return;
            }
            if (STARTUP_COUNTER < CAL_STARTUP_LOOPS) {
                STARTUP_COUNTER = (bua_u8)(STARTUP_COUNTER + 1u);
                bua_low_rpm_spark_reset_ld306();
                return;
            }
            ENGINE_MODE_WORD |= ENGINE_RUNNING_BIT;
            RAM8(0x003Bu) &= (bua_u8)~0x02u;
            RAM8(0x00B0u) = 0u;
            if ((IGN_MODE_WORD & 0x10u) == 0u && ram16be_get(0xEFu) < 85u)
                RAM8(0x3Fu) |= 0x40u;
            RAM8(0x0001u) |= 0x08u;
            ++stats.engine_run_transitions;
        }
        bua_running_spark_12p5ms();
        return;
    }
    /* LCDC1..LCDCD only counts missing references after ENGINE RUNNING.
     * The original reaches a SWI wait at 23 loops.  We record that boundary
     * but deliberately do not invent the interrupt-side recovery yet. */
    if ((ENGINE_MODE_WORD & ENGINE_RUNNING_BIT) != 0u) {
        if (NO_REF_MINOR_COUNT >= CAL_NO_REF_WAIT_LOOPS) {
            mpu16be_set(0x3FC0u, 0xFFFFu);
            ++stats.reference_wait_events;
            bua_vector_note_swi_step119(0xCDCDu);
            return;
        }
        NO_REF_MINOR_COUNT = (bua_u8)(NO_REF_MINOR_COUNT + 1u);
        bua_running_spark_12p5ms();
    } else {
        bua_low_rpm_spark_reset_ld306();
    }
}
static void spark_vss_12p5ms(void)
{
    ++stats.spark_vss_loops;
    /* LCC7B..LCC92, Mode 4 can bypass the LF21A watchdog transfer. */
    RAM8(0x3Bu) &= (bua_u8)~8u;
    if ((MINOR_MODE_WORD2 & 8u) && (RAM8(0x154u) & 128u)) {
        RAM8(0x3Bu) |= 8u;
        RAM8(0x36u) |= 1u;
    } else {
        RAM8(0x30u) ^= 2u;
    }
    bua_lcd05_diagnostic_merge();
    bua_slow_o2_filter_12p5ms();
    if ((ENGINE_MODE_WORD & ENGINE_RUNNING_BIT) &&
        (MINOR_MODE_WORD2 & REF_SEEN_BIT) == 0u &&
        NO_REF_MINOR_COUNT >= CAL_NO_REF_WAIT_LOOPS) {
        bua_reference_state_12p5ms();
        return;
    }
    bua_reference_state_12p5ms();
    bua_write_lf92a(ram16be_get(0x00C2u));
    bua_iac_shutdown_homing_even_step111();
}
/* ---------------------------------------------------------------------- */
/* LDEA4 - called every 6.25 ms: check whether the VSS capture changed.    */
/* ---------------------------------------------------------------------- */
static void bua_vss_capture_check_6p25ms(void)
{
    bua_u16 capture;
    bua_u8 b;
    capture = bua_hal_vss_capture();
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
/* LF8B6/LF8E6 normal and diagnostic byte sources (Steps 89/168). */
static const bua_u16 bua_160_diagnostic_addresses[24]={
0xC000u,0xC001u,0x002Cu,0x005Du,0x0065u,0x0112u,0x0057u,0x0081u,
0x00C6u,0x006Fu,0x0005u,0x0006u,0x0007u,0x0008u,0x0044u,0x0060u,
0x0037u,0x00A1u,0x00C0u,0x00F1u,0x00EAu,0x00EBu,0x00D5u,0x00D6u};
static bua_u8 bua_160_diag_read(bua_u16 p)
{
    if(p==0xC000u) return 0x25u;
    if(p==0xC001u) return 0xE5u;
    return RAM8(p);
}
static void bua_160_load_next_normal_byte(void)
{
    bua_u8 i=ALCL_TABLE_INDEX,v;
    if(i==0u) v=MINOR_MODE_WORD2;
    else {
        if(i>DISPLAY_TABLE_LENGTH) {
            ALCL_TABLE_INDEX=0u; ALCL_XMIT_BYTE=0xFFu; return;
        }
        switch(i) {
        case 1u:v=(bua_u8)CAL_NUM_CYL_CODE;break;
        case 2u:v=RAM8(0x011Au);break;
        case 3u:v=RAM8(0x011Eu);break;
        default:v=(bua_u8)CAL_GPH_SCALE_FACTOR;break;
        }
    }
    ALCL_XMIT_BYTE=v; ALCL_TABLE_INDEX=(bua_u8)(i+1u);
    ++stats.display_bytes_loaded;
}
static void bua_160_load_next_diagnostic_byte(void)
{
    bua_u8 i=ALCL_TABLE_INDEX;
    if(i==0u) ALCL_XMIT_BYTE=MINOR_MODE_WORD2;
    else {
        if(i==25u) {
            ALCL_TABLE_INDEX=0u; ALCL_XMIT_BYTE=0xFFu; return;
        }
        ALCL_XMIT_BYTE=bua_160_diag_read(bua_160_diagnostic_addresses[i-1u]);
    }
    ALCL_TABLE_INDEX=(bua_u8)(i+1u); ++stats.display_bytes_loaded;
}
/* F88E..F8B4: three 16-byte factory streams rooted at FC72/FC82/FC92. */
static const bua_u16 bua_160_factory_addresses[3][8]={
 {0x017Bu,0x017Du,0x017Fu,0x0181u,0x0183u,0x0185u,0x0187u,0x0049u},
 {0xC000u,0xC002u,0xC004u,0x0173u,0x0175u,0x4002u,0x0047u,0x0000u},
 {0x3FC0u,0x3FC2u,0x3FC4u,0x3FC6u,0x3FC8u,0x3FCAu,0x3FE0u,0x3FF8u}
};
static bua_u8 bua_160_factory_read(bua_u16 p)
{
    if(p<0x0500u) return RAM8(p);
    if(p>=0x3FC0u&&p<0x4000u) return mem.mpu[p-0x3FC0u];
    if(p>=0x4000u&&p<0x4010u) return mem.io4000[p-0x4000u];
    switch(p) {
    case 0xC000u:return 0x25u; case 0xC001u:return 0xE5u;
    case 0xC002u:return 0x09u; case 0xC003u:return 0x79u;
    case 0xC004u:return 0x02u; case 0xC005u:return 0xEEu;
    default:return 0u;
    }
}
static bua_u8 bua_160_load_next_factory_byte(void)
{
    bua_u8 mode=RAM8(0x0048u),i=ALCL_TABLE_INDEX;
    bua_u16 p;
    if(mode==0u||mode>3u) return 0u;
    if(i>=16u) {
        ALCL_TABLE_INDEX=0u; ALCL_XMIT_BYTE=0xFFu; return 0u;
    }
    if((i&1u)!=0u) ALCL_XMIT_BYTE=RAM8(0x017Au);
    else {
        p=bua_160_factory_addresses[mode-1u][i>>1];
        ALCL_XMIT_BYTE=bua_160_factory_read(p);
        RAM8(0x017Au)=bua_160_factory_read((bua_u16)(p+1u));
    }
    ALCL_TABLE_INDEX=(bua_u8)(i+1u); ++stats.display_bytes_loaded;
    return 1u;
}
/* LF8F3 drives the line high through LF4A8 on this cell. */
static void bua_160_first_high(bua_u32 t)
{
    bua_write_control_low((bua_u8)(mpu16be_get(MPU_CPU_CONTROL_ADDR) | 4u));
    serial_line_high_at(t);
}

static int bua_lf880_manager(bua_u32 t)
{
    bua_u8 msb;
    ++stats.serial_manager_calls;
    if(ALCL_BIT_COUNT!=0u) {
        ALCL_BIT_COUNT=(bua_u8)(ALCL_BIT_COUNT-1u);
        msb=(bua_u8)((ALCL_XMIT_BYTE&0x80u)!=0u);
        ALCL_XMIT_BYTE=(bua_u8)(ALCL_XMIT_BYTE<<1);
        if(msb==0u) bua_160_first_high(t);
        return msb!=0u?1:0;
    }
    ALCL_BIT_COUNT=8u;
    if((RAM8(0x0047u)&0x80u)!=0u) {
        if(bua_160_load_next_factory_byte()==0u) return -1;
    } else if((MINOR_MODE_WORD2&0x30u)!=0u) bua_160_load_next_diagnostic_byte();
    else bua_160_load_next_normal_byte();
    if(ALCL_TABLE_INDEX!=0u) bua_160_first_high(t);
    return -1;
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
        case 0xDu: segD_diagnostics();         break;
        case 0xEu: segE_tcc_adc();            break;
        default:   segF_fuel_air_major();      break;
    }
}
/* Finish the preceding cell's timer compares, then arm this minor loop. */
static void bua_serial_timer_irq(bua_u32 tick)
{
    bua_u32 start = tick * SERIAL_T_CELL;
    bua_u8 low;
    if (SERIAL_MODE_WORD & SERIAL_FLAG_FIRST) {
        mem.io4000[6] = (bua_u8)(TIMER_SNAPSHOT + 143u);
        SERIAL_MODE_WORD &= (bua_u8)~SERIAL_FLAG_FIRST;
        (void)bua_lf880_manager(start - SERIAL_T_CELL + SERIAL_T_FIRST);
    }
    if (SERIAL_MODE_WORD & SERIAL_FLAG_SECOND) {
        mem.io4000[6] = (bua_u8)(TIMER_SNAPSHOT + 205u);
        SERIAL_MODE_WORD &= (bua_u8)~SERIAL_FLAG_SECOND;
        bua_write_control_low((bua_u8)(mpu16be_get(MPU_CPU_CONTROL_ADDR) | 4u));
        serial_line_high_at(start - SERIAL_T_CELL + SERIAL_T_SECOND);
        if ((MINOR_MODE_WORD2 & 0x20u) && ALCL_TABLE_INDEX == 25u &&
            ALCL_BIT_COUNT == 0u) {
            RAM8(0x012Eu) = 9u;
            SERIAL_MODE_WORD |= 4u;
            mem.io4000[7] = 0x27u;
            mpu16be_set(MPU_CPU_CONTROL_ADDR, mpu16be_get(MPU_CPU_CONTROL_ADDR) | 4u);
        }
    }
    TIMER_SNAPSHOT = mem.io4000[6];
    if (SERIAL_MODE_WORD & 4u) {
        if ((SERIAL_MODE_WORD & 8u) || --RAM8(0x012Eu) != 0u) {
            mem.io4000[6] = (bua_u8)(TIMER_SNAPSHOT + 205u);
            return;
        }
        SERIAL_MODE_WORD &= (bua_u8)~4u;
    }
    SERIAL_MODE_WORD |= SERIAL_FLAG_FIRST | SERIAL_FLAG_SECOND;
    low = (bua_u8)mpu16be_get(MPU_CPU_CONTROL_ADDR);
    bua_write_control_low((bua_u8)(low & 0xFBu));
    serial_line_low_at(start);
    mem.io4000[7] = 1u;
    mem.io4000[6] = (bua_u8)(TIMER_SNAPSHOT + 11u);
}
/* LCB1D..LCB5A, VATS is enabled in the BUA calibration. */
static void bua_vats_reference_check(void)
{
    bua_u16 capture, delta;
    if (MODE_WORD3 & FUEL_VATS_OK_BIT)
        return;
    RAM8(7u) |= 2u;
    ram16be_set(0x18u, bua_error_word_checksum_step113());
    capture = mpu16be_get(0x3FF8u);
    delta = (bua_u16)(capture - ram16be_get(0x121u));
    if (delta != 0u) {
        ram16be_set(0x121u, capture);
        if (delta >= 1638u && delta <= 3277u) {
            MODE_WORD3 |= FUEL_VATS_OK_BIT;
            RAM8(7u) &= (bua_u8)~2u;
            ram16be_set(0x18u, bua_error_word_checksum_step113());
        }
    }
}
static void irq_6p25ms(void)
{
    bua_u8 count;
    if(bua_hal_powerdown_latched()!=0u)
        return;
    if((RAM8(0x0047u)&FACTORY117_MODE_BIT)!=0u) {
        bua_factory_irq_step117();
        return;
    }
    bua_serial_timer_irq(stats.irq_ticks);
    ++stats.irq_ticks;
    RAM8(0xA0u) = bua_hal_minor_irq_status();
    if (RAM8(0xA0u) & 8u) {
        COLD_START_REF_COUNT = bua_cold_start_ref_count_step44(COLD_START_REF_COUNT,
            ram16be_get(ENGINE_RUN_TIME_ADDR), TPS_LOAD_AXIS);
        if ((SPARK_INPUT_MODE_WORD & EST_ENABLE_BIT) && !(RAM8(0x9Du) & 0x80u)) {
            if (SPARK_BLEND_MULT > 253u) {
                SPARK_BLEND_MULT = 255u;
                SPARK_MODE_WORD &= (bua_u8)~SPARK_BLEND_BIT;
            } else {
                SPARK_BLEND_MULT += 2u;
            }
        }
        if (!(RAM8(0x3Bu) & 2u) && !(ENGINE_MODE_WORD & ENGINE_RUNNING_BIT)) {
            RAM8(0x3Bu) |= 2u;
            RAM8(0x472u) = 0u;
        }
    }
    if (IGN_MODE_WORD & 0x10u)
        MINOR_MODE_WORD2 &= (bua_u8)~REF_SEEN_BIT;
    bua_vats_reference_check();
    ram16be_set(REF_PERIOD_ADDR, mpu16be_get(0x3FC0u)); /* LCB5A */
    bua_vss_capture_check_6p25ms();
    /* $CB5F..$CBB4 consumes the completed SCI mode before count dispatch. */
    bua_mode4_scheduler_step116();
    count = (bua_u8)(MINOR_COUNT + 1u);
    if (count == 160u) {
        one_second_event();
        count = 0u;
    }
    MINOR_COUNT = count;

    if(bua_hal_fmd_enabled()!=0u)
        bua_normal_fmd_refresh_step158();
    /* LCC21 calls LF4DF before the odd/even minor-loop dispatch. */
    bua_iac_motor_service_6p25ms();
    /* LF67B services an MPU-reported injection before falling into LF76A. */
    bua_common_injector_service_6p25ms();
    /* LF76A completes before LCC65 selects the odd/even minor path. */
    bua_common_maf_6p25ms();
    /* LCC21 reads/normalizes TPS and updates its transient filter. */
    bua_common_tps_6p25ms();
    /* LCC3A..LCC63 qualifies high airflow on each stopped-engine tick.
     * LC205 is 96 ticks. The following pass sets the high-MAF latch. */
    if (!(ENGINE_MODE_WORD & ENGINE_RUNNING_BIT)) {
        if ((RAM8(0x33u) & 0x10u) || RAM8(0x7Fu) <= 100u)
            RAM8(0xB0u) = 0u;
        else if (RAM8(0xB0u) > 96u)
            RAM8(0x3Fu) |= 0x80u;
        else if (RAM8(0xB6u) > 45u)
            ++RAM8(0xB0u);
        else
            RAM8(0xB0u) = 0u;
    }
    if ((count & 1u) != 0u)
        air_fuel_12p5ms();
    else
        spark_vss_12p5ms();
    if(bua_hal_powerdown_latched()!=0u)
        return;
    major_segment((bua_u8)(count & 0x0Fu));
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
static BuaFuelOutputResult bua_fuel_cutoff(bua_u16 corrected_base_pw,
                                                 bua_u8 mode_word3,
                                                 bua_u8 ign_mode_word,
                                                 bua_u8 af_mode_word,
                                                 bua_u8 spark_mode_word,
                                                 bua_u8 speed_raw,
                                                 bua_u16 ref_period)
{
    BuaFuelOutputResult r;
    bua_u8 cut;
    bua_u8 high_cut_active;
    bua_u8 speed_limit;
    bua_u16 ref_limit;
    memset(&r, 0, sizeof(r));
    r.corrected_base_pw = corrected_base_pw;
    r.af_mode_word = af_mode_word;
    r.ign_mode_word = ign_mode_word;
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
    r.fuel_cut = cut;
    return r;
}

static BuaFuelOutputResult bua_shape_fuel_output(BuaFuelOutputResult r,
                                               bua_u16 pending,
                                               bua_u8 single_fire_word,
                                               bua_u8 injector_batt_offset)
{
    bua_u16 shaped;
    bua_u8 small_offset;
    if ((single_fire_word & SINGLE_FIRE_BIT) != 0u &&
        (single_fire_word & SINGLE_FIRE_ZERO_PW_BIT) != 0u)
        pending = 0u;
    r.pending_pw = pending;
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
    BuaFuelOutputResult r = bua_fuel_cutoff(corrected_base_pw, mode_word3,
                                           ign_mode_word, af_mode_word,
                                           spark_mode_word, speed_raw, ref_period);
    return bua_shape_fuel_output(r, r.fuel_cut ? 0u : corrected_base_pw,
                                 single_fire_word, injector_batt_offset);
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
