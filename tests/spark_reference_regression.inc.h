static void run_main_spark_lookup_test(void)
{
    bua_u8 v;
    unsigned int pass_count;
    printf("\nMain spark C02E / LF27C lookup regression:\n");
    pass_count = 0u;
    v = bua_lookup_main_spark_raw(160u, 128u);
    if (v == 100u)
        ++pass_count;
    printf("  axis point: L0056=160 (2400 RPM), load=128 -> raw=%u, %0.2f deg  %s\n",
           (unsigned int)v, bua_spark_raw_to_degrees(v),
           v == 100u ? "PASS" : "FAIL");
    v = bua_lookup_main_spark_raw(152u, 152u);
    if (v == 93u)
        ++pass_count;
    printf("  bilinear midpoint: L0056=152, load=152 -> raw=%u, %0.2f deg  %s\n",
           (unsigned int)v, bua_spark_raw_to_degrees(v),
           v == 93u ? "PASS" : "FAIL");
    v = bua_lookup_main_spark_raw(255u, 208u);
    if (v == 92u)
        ++pass_count;
    printf("  high-axis limit: L0056=255, load=208 -> raw=%u, %0.2f deg  %s\n",
           (unsigned int)v, bua_spark_raw_to_degrees(v),
           v == 92u ? "PASS" : "FAIL");
    v = bua_lookup_main_spark_raw(0u, 0u);
    if (v == 57u)
        ++pass_count;
    printf("  below-load minimum clamps to first column -> raw=%u, %0.2f deg  %s\n",
           (unsigned int)v, bua_spark_raw_to_degrees(v),
           v == 57u ? "PASS" : "FAIL");
    printf("  main-spark lookup result: %s (%u/4)\n",
           pass_count == 4u ? "PASS" : "FAIL", pass_count);
}
static void run_coolant_spark_lookup_test(void)
{
    bua_u8 main_raw;
    bua_u8 cool_raw;
    bua_s16 partial_raw;
    unsigned int pass_count;
    printf("\nCoolant spark C0FF / LF27C regression:\n");
    pass_count = 0u;
    cool_raw = bua_lookup_coolant_spark_raw(32u, 32u);
    if (cool_raw == 81u)
        ++pass_count;
    printf("  -16 C axis, load=32: raw=%u correction=%0.2f deg  %s\n",
           (unsigned int)cool_raw,
           bua_spark_signed_raw_to_degrees((bua_s16)cool_raw - 57),
           cool_raw == 81u ? "PASS" : "FAIL");
    cool_raw = bua_lookup_coolant_spark_raw(72u, 136u);
    if (cool_raw == 66u)
        ++pass_count;
    printf("  bilinear midpoint: coolantArg=72, load=136 -> raw=%u correction=%0.2f deg  %s\n",
           (unsigned int)cool_raw,
           bua_spark_signed_raw_to_degrees((bua_s16)cool_raw - 57),
           cool_raw == 66u ? "PASS" : "FAIL");
    cool_raw = bua_lookup_coolant_spark_raw(208u, 112u);
    if (cool_raw == 40u)
        ++pass_count;
    printf("  116 C axis, load=112: raw=%u correction=%0.2f deg  %s\n",
           (unsigned int)cool_raw,
           bua_spark_signed_raw_to_degrees((bua_s16)cool_raw - 57),
           cool_raw == 40u ? "PASS" : "FAIL");
    cool_raw = bua_lookup_coolant_spark_raw(208u, 200u);
    if (cool_raw == 40u)
        ++pass_count;
    printf("  load limit: coolantArg=208, load=200 -> uses load=160, raw=%u  %s\n",
           (unsigned int)cool_raw, cool_raw == 40u ? "PASS" : "FAIL");
    partial_raw = bua_main_plus_coolant_raw(160u, 128u, 32u,
                                             &main_raw, &cool_raw);
    if (main_raw == 100u && cool_raw == 81u && partial_raw == 124)
        ++pass_count;
    printf("  combine: main=%u + coolant=%u - bias=%u -> raw=%d = %0.2f deg  %s\n",
           (unsigned int)main_raw, (unsigned int)cool_raw,
           (unsigned int)CAL_COOL_SPARK_BIAS_RAW, (int)partial_raw,
           bua_spark_signed_raw_to_degrees(partial_raw),
           (main_raw == 100u && cool_raw == 81u && partial_raw == 124) ? "PASS" : "FAIL");
    printf("  coolant-spark regression result: %s (%u/5)\n",
           pass_count == 5u ? "PASS" : "FAIL", pass_count);
}
static void run_step15_spark_terms_test(void)
{
    unsigned int pass_count;
    bua_u8 timer;
    bua_u8 raw;
    bua_u8 mode;
    unsigned int i;
    pass_count = 0u;
    printf("\nHighway-mode spark + hot-restart retard regression:\n");
    raw = bua_highway_spark_eval(80u, 133u, 254u, 0u, 7u, &timer);
    if (raw == 0u && timer == 0u)
        ++pass_count;
    printf("  highway coolant threshold: cool=133 -> raw=%u timer=%u  %s\n",
           (unsigned int)raw, (unsigned int)timer,
           (raw == 0u && timer == 0u) ? "PASS" : "FAIL");
    timer = 0u;
    raw = 0u;
    for (i = 0u; i < 12u; ++i)
        raw = bua_highway_spark_eval(80u, 134u, 254u, 0u, timer, &timer);
    if (raw == 28u && timer == 11u)
        ++pass_count;
    printf("  highway delay/table: 12 qualified 1-sec opportunities, load=80 -> raw=%u (%0.2f deg), timer=%u  %s\n",
           (unsigned int)raw, bua_spark_raw_to_degrees(raw),
           (unsigned int)timer,
           (raw == 28u && timer == 11u) ? "PASS" : "FAIL");
    raw = bua_highway_spark_eval(100u, 134u, 254u, 2u, 11u, &timer);
    if (raw == 27u && timer == 11u)
        ++pass_count;
    printf("  highway interpolation: load=100 -> raw=%u (%0.2f deg)  %s\n",
           (unsigned int)raw, bua_spark_raw_to_degrees(raw),
           (raw == 27u && timer == 11u) ? "PASS" : "FAIL");
    raw = bua_highway_spark_eval(101u, 134u, 254u, 2u, 11u, &timer);
    if (raw == 0u && timer == 0u)
        ++pass_count;
    printf("  highway load disqualifier: load=101 -> raw=%u timer=%u  %s\n",
           (unsigned int)raw, (unsigned int)timer,
           (raw == 0u && timer == 0u) ? "PASS" : "FAIL");
    mode = 0u;
    raw = bua_hot_restart_retard_eval(0u, 113u, 0u, &mode);
    if (raw == 28u && mode == 0u)
        ++pass_count;
    printf("  hot restart active: startCool=113 runSec=0 -> retard=%u (%0.2f deg), mode=%02X  %s\n",
           (unsigned int)raw, bua_spark_raw_to_degrees(raw),
           (unsigned int)mode,
           (raw == 28u && mode == 0u) ? "PASS" : "FAIL");
    mode = 0u;
    raw = bua_hot_restart_retard_eval(0u, 113u, 25u, &mode);
    if (raw == 0u && (mode & MODE1_IAC_KICKDOWN_ENABLE) != 0u)
        ++pass_count;
    printf("  hot restart expiry: runSec=25 -> retard=%u mode=%02X  %s\n",
           (unsigned int)raw, (unsigned int)mode,
           (raw == 0u && (mode & MODE1_IAC_KICKDOWN_ENABLE) != 0u) ? "PASS" : "FAIL");
    mode = 0u;
    raw = bua_hot_restart_retard_eval(0u, 112u, 0u, &mode);
    if (raw == 0u && (mode & MODE1_IAC_KICKDOWN_ENABLE) != 0u)
        ++pass_count;
    printf("  hot restart coolant disqualifier: startCool=112 -> retard=%u mode=%02X  %s\n",
           (unsigned int)raw, (unsigned int)mode,
           (raw == 0u && (mode & MODE1_IAC_KICKDOWN_ENABLE) != 0u) ? "PASS" : "FAIL");
    printf("  step-15 spark-term regression result: %s (%u/7)\n",
           pass_count == 7u ? "PASS" : "FAIL", pass_count);
}
static void run_step16_startup_spark_test(void)
{
    unsigned int pass_count;
    bua_u16 initial;
    bua_u16 after1;
    bua_u16 after2;
    bua_u16 after3;
    pass_count = 0u;
    printf("\nStartup-spark initialization/decay regression (C17A/C18A/C198/C1AB):\n");
    ecm_reset();
    bua_startup_spark_initialize(160u); /* encoded 80 C */
    if (ram16be_get(STARTUP_SPARK_ADDR) == 0x1C00u &&
        ram16be_get(STARTUP_SPARK_DELAY_ADDR) == 40u)
        ++pass_count;
    printf("  initialize at coolantArg=160: L0013:14=%04X, delay=%u  %s\n",
           (unsigned int)ram16be_get(STARTUP_SPARK_ADDR),
           (unsigned int)ram16be_get(STARTUP_SPARK_DELAY_ADDR),
           (ram16be_get(STARTUP_SPARK_ADDR) == 0x1C00u &&
            ram16be_get(STARTUP_SPARK_DELAY_ADDR) == 40u) ? "PASS" : "FAIL");
    /* C18A interpolates 100->80 halfway between encoded 56 C and 68 C. */
    ecm_reset();
    bua_startup_spark_initialize(136u);
    if (ram16be_get(STARTUP_SPARK_DELAY_ADDR) == 90u)
        ++pass_count;
    printf("  delay interpolation coolantArg=136: delay=%u (LF266 -> 90)  %s\n",
           (unsigned int)ram16be_get(STARTUP_SPARK_DELAY_ADDR),
           ram16be_get(STARTUP_SPARK_DELAY_ADDR) == 90u ? "PASS" : "FAIL");
    ecm_reset();
    bua_startup_spark_initialize(160u);
    ram16be_set(STARTUP_SPARK_DELAY_ADDR, 2u);
    initial = ram16be_get(STARTUP_SPARK_ADDR);
    bua_startup_spark_on_inject(160u);
    after1 = ram16be_get(STARTUP_SPARK_ADDR);
    bua_startup_spark_on_inject(160u);
    after2 = ram16be_get(STARTUP_SPARK_ADDR);
    if (initial == 0x1C00u && after1 == initial && after2 == initial &&
        ram16be_get(STARTUP_SPARK_DELAY_ADDR) == 0u)
        ++pass_count;
    printf("  delay countdown: 2 injector events -> delay=%u spark=%04X  %s\n",
           (unsigned int)ram16be_get(STARTUP_SPARK_DELAY_ADDR),
           (unsigned int)after2,
           (initial == 0x1C00u && after1 == initial && after2 == initial &&
            ram16be_get(STARTUP_SPARK_DELAY_ADDR) == 0u) ? "PASS" : "FAIL");
    /* With C198=1 and C1AB=128, decay is applied, one injector is skipped,
     * then decay is applied again.  Preserve the 16-bit fixed-point bytes. */
    bua_startup_spark_on_inject(160u);
    after1 = ram16be_get(STARTUP_SPARK_ADDR);
    bua_startup_spark_on_inject(160u);
    after2 = ram16be_get(STARTUP_SPARK_ADDR);
    bua_startup_spark_on_inject(160u);
    after3 = ram16be_get(STARTUP_SPARK_ADDR);
    if (after1 == 0x1B80u && after2 == 0x1B80u && after3 == 0x1B00u &&
        STARTUP_SPARK_REPEAT == 1u)
        ++pass_count;
    printf("  decay cadence: %04X -> %04X -> %04X, sumByte=%u  %s\n",
           (unsigned int)after1, (unsigned int)after2, (unsigned int)after3,
           (unsigned int)bua_startup_spark_sum_byte(),
           (after1 == 0x1B80u && after2 == 0x1B80u && after3 == 0x1B00u &&
            STARTUP_SPARK_REPEAT == 1u) ? "PASS" : "FAIL");
    ecm_reset();
    bua_startup_spark_initialize(160u);
    ram16be_set(STARTUP_SPARK_DELAY_ADDR, 3u);
    SPARK_MODE_WORD |= SPARK_BLEND_BIT;
    bua_startup_spark_on_inject(160u);
    if (ram16be_get(STARTUP_SPARK_DELAY_ADDR) == 3u &&
        ram16be_get(STARTUP_SPARK_ADDR) == 0x1C00u)
        ++pass_count;
    printf("  blend hold (L0036 b5): delay=%u spark=%04X  %s\n",
           (unsigned int)ram16be_get(STARTUP_SPARK_DELAY_ADDR),
           (unsigned int)ram16be_get(STARTUP_SPARK_ADDR),
           (ram16be_get(STARTUP_SPARK_DELAY_ADDR) == 3u &&
            ram16be_get(STARTUP_SPARK_ADDR) == 0x1C00u) ? "PASS" : "FAIL");
    ecm_reset();
    bua_startup_spark_initialize(208u);
    ram16be_set(STARTUP_SPARK_DELAY_ADDR, 0u);
    bua_startup_spark_on_inject(208u);
    if (ram16be_get(STARTUP_SPARK_ADDR) == 0x1B01u)
        ++pass_count;
    printf("  116-C endpoint decay C1AB=255: 1C00 -> %04X  %s\n",
           (unsigned int)ram16be_get(STARTUP_SPARK_ADDR),
           ram16be_get(STARTUP_SPARK_ADDR) == 0x1B01u ? "PASS" : "FAIL");
    printf("  step-16 startup-spark regression result: %s (%u/6)\n",
           pass_count == 6u ? "PASS" : "FAIL", pass_count);
}
static void run_dwell_arithmetic_test(void)
{
    bua_u16 dwell;
    bua_u16 dynamic;
    unsigned int pass_count;
    printf("\nDwell arithmetic regression (LCEB3..LCF1A):\n");
    pass_count = 0u;
    ecm_reset();
    ram16be_set(REF_PERIOD_ADDR, 410u);
    ram16be_set(REF_PERIOD_OLD_ADDR, 410u);
    ram16be_set(DYNAMIC_DWELL_ADDR, 0u);
    FILTERED_LOAD = 80u;
    LOAD_25MS_OLD = 80u;
    BATTERY_AD = 128u;
    bua_compute_dwell_12p5ms();
    dwell = ram16be_get(DWELL_COUNTS_ADDR);
    dynamic = ram16be_get(DYNAMIC_DWELL_ADDR);
    if (dwell == 284u && dynamic == 0u)
        ++pass_count;
    printf("  2400-RPM-equivalent ref=410, steady load, battAD=128: dwell=%u dynamic=%u  %s\n",
           (unsigned int)dwell, (unsigned int)dynamic,
           (dwell == 284u && dynamic == 0u) ? "PASS" : "FAIL");
    ecm_reset();
    ram16be_set(REF_PERIOD_ADDR, 410u);
    ram16be_set(REF_PERIOD_OLD_ADDR, 410u);
    ram16be_set(DYNAMIC_DWELL_ADDR, 0u);
    FILTERED_LOAD = 100u;
    LOAD_25MS_OLD = 50u;
    BATTERY_AD = 128u;
    bua_compute_dwell_12p5ms();
    dwell = ram16be_get(DWELL_COUNTS_ADDR);
    dynamic = ram16be_get(DYNAMIC_DWELL_ADDR);
    if (dwell == 335u && dynamic == 51u)
        ++pass_count;
    printf("  +50 load step: dynamic limited to ref/8 => dwell=%u dynamic=%u  %s\n",
           (unsigned int)dwell, (unsigned int)dynamic,
           (dwell == 335u && dynamic == 51u) ? "PASS" : "FAIL");
    ecm_reset();
    ram16be_set(REF_PERIOD_ADDR, 410u);
    ram16be_set(REF_PERIOD_OLD_ADDR, 410u);
    ram16be_set(DYNAMIC_DWELL_ADDR, 0u);
    FILTERED_LOAD = 80u;
    LOAD_25MS_OLD = 80u;
    BATTERY_AD = 100u;
    bua_compute_dwell_12p5ms();
    dwell = ram16be_get(DWELL_COUNTS_ADDR);
    dynamic = ram16be_get(DYNAMIC_DWELL_ADDR);
    if (dwell == 371u && dynamic == 0u)
        ++pass_count;
    printf("  low battery compensation: dwell=%u (ref-39 cap=%u)  %s\n",
           (unsigned int)dwell, 371u,
           (dwell == 371u && dynamic == 0u) ? "PASS" : "FAIL");
    printf("  dwell regression result: %s (%u/3)\n",
           pass_count == 3u ? "PASS" : "FAIL", pass_count);
}
static void run_reference_startup_test(void)
{
    unsigned int i;
    unsigned int first_running_tick;
    printf("\nReference qualification / engine-run startup test:\n");
    printf("  LC018=%u timer counts, LC01A=%u qualifying 12.5-ms loops.\n",
           (unsigned int)CAL_STARTUP_REF_PERIOD,
           (unsigned int)CAL_STARTUP_LOOPS);
    ecm_reset();
    sim_set_ecm_reference_rpm(250u);
    first_running_tick = 0u;
    for (i = 0u; i < 160u; ++i) {
        irq_6p25ms();
        if (first_running_tick == 0u &&
            (ENGINE_MODE_WORD & ENGINE_RUNNING_BIT) != 0u)
            first_running_tick = i + 1u;
    }
    printf("  250 RPM for 1.0 s: running=%u startupCount=%u refPeriod=%u  %s\n",
           (unsigned int)((ENGINE_MODE_WORD & ENGINE_RUNNING_BIT) != 0u),
           (unsigned int)STARTUP_COUNTER,
           (unsigned int)ram16be_get(REF_PERIOD_ADDR),
           first_running_tick == 0u ? "PASS (below threshold)" : "FAIL");
    ecm_reset();
    sim_set_ecm_reference_rpm(400u);
    first_running_tick = 0u;
    for (i = 0u; i < 160u; ++i) {
        irq_6p25ms();
        if (first_running_tick == 0u &&
            (ENGINE_MODE_WORD & ENGINE_RUNNING_BIT) != 0u)
            first_running_tick = i + 1u;
    }
    printf("  400 RPM for 1.0 s: running=%u startupCount=%u refPeriod=%u firstRun=%0.4f s  %s\n",
           (unsigned int)((ENGINE_MODE_WORD & ENGINE_RUNNING_BIT) != 0u),
           (unsigned int)STARTUP_COUNTER,
           (unsigned int)ram16be_get(REF_PERIOD_ADDR),
           first_running_tick == 0u ? 0.0 : (double)first_running_tick * 0.00625,
           first_running_tick != 0u ? "PASS" : "FAIL");
}
static void print_segment_table(void)
{
    static const char *names[16] = {
        "NOP",
        "Output bits",
        "VSS calc",
        "Misc 100 ms",
        "Log RAM to H.U.",
        "A/C, closed loop, fan",
        "Coolant A/D",
        "Coolant variables",
        "NOP",
        "Injection air management",
        "MAT variables",
        "EGR",
        "Canister purge",
        "Diagnostics",
        "TCC A/D",
        "Fuel/Air major"
    };
    unsigned int i;
    printf("BUA major-loop table:\n");
    for (i = 0u; i < 16u; ++i)
        printf("  %X  %s\n", i, names[i]);
}
/* -------------------------------------------------------------------------- */
/* Step 30: MAF to airflow, LF76A..LF87F.                                    */
/* -------------------------------------------------------------------------- */
typedef struct {
    bua_u16 samples[4];
    bua_u8 next;
    bua_u16 filtered;
} BuaMafFilterState;
typedef struct {
    bua_u16 raw_maf_axis;
    bua_u16 unlimited_q8_8;
    bua_u16 filtered_q8_8;
    bua_u16 final_q8_8;
    bua_u8 table_number;
    bua_u8 table_raw;
    bua_u8 scalar;
    bua_u8 used_default;
} BuaMafResult;
static bua_u8 bua_interp_u8_segment(const bua_u8 *table,
                                     bua_u8 intervals,
                                     bua_u8 arg)
{
    unsigned int p;
    unsigned int index;
    unsigned int frac;
    unsigned int y0;
    unsigned int y1;
    unsigned int delta;
    unsigned int v;
    p = (unsigned int)arg * (unsigned int)intervals;
    index = p >> 8;
    frac = p & 0xFFu;
    if (index >= (unsigned int)intervals)
        return table[intervals];
    y0 = table[index];
    y1 = table[index + 1u];
    if (y1 >= y0) {
        delta = y1 - y0;
        /* LF2D6 labels this the increasing-table round-up path. */
        v = y0 + ((delta * frac + 255u) >> 8);
    } else {
        delta = y0 - y1;
        v = y0 - ((delta * frac) >> 8);
    }
    if (v > 255u) v = 255u;
    return (bua_u8)v;
}
static bua_u16 bua_maf_unlimited_from_axis_lf7ee(bua_u16 maf_axis,
                                                  bua_u8 *table_number,
                                                  bua_u8 *table_raw,
                                                  bua_u8 *scalar_out)
{
    bua_u8 hi;
    bua_u8 lo;
    bua_u8 n;
    bua_u8 raw;
    bua_u8 scalar;
    const bua_u8 *table;
    bua_u8 intervals;
    bua_u16 work;
    work = maf_axis;
    hi = (bua_u8)(work >> 8);
    if (hi > 6u) work = 0x06FFu;
    hi = (bua_u8)(work >> 8);
    if (hi < 2u) {
        work = (bua_u16)(work >> 1);
        hi = 1u;
    }
    lo = (bua_u8)work;
    n = (bua_u8)(hi - 1u);
    if (n > 5u) n = 5u;
    scalar = cal_maf_scalar[n];
    table = cal_maf_table1;
    intervals = 8u;
    if (n == 1u) table = cal_maf_table2;
    else if (n == 2u) table = cal_maf_table3;
    else if (n == 3u) table = cal_maf_table4;
    else if (n == 4u) table = cal_maf_table5;
    else if (n == 5u) { table = cal_maf_table6; intervals = 16u; }
    raw = bua_interp_u8_segment(table, intervals, lo);
    if (table_number != 0) *table_number = (bua_u8)(n + 1u);
    if (table_raw != 0) *table_raw = raw;
    if (scalar_out != 0) *scalar_out = scalar;
    return (bua_u16)((unsigned int)raw * (unsigned int)scalar);
}
static bua_u16 bua_maf_analog_unlimited_lf7ac(bua_u8 maf_adc,
                                               bua_u8 *table_number,
                                               bua_u8 *table_raw,
                                               bua_u8 *scalar_out,
                                               bua_u16 *axis_out)
{
    bua_u16 axis;
    axis = (bua_u16)((unsigned int)maf_adc * 7u);
    if (axis_out != 0) *axis_out = axis;
    return bua_maf_unlimited_from_axis_lf7ee(axis, table_number,
                                             table_raw, scalar_out);
}
static bua_u16 bua_maf_filter_lf80x(BuaMafFilterState *st, bua_u16 unlimited)
{
    unsigned long sum;
    bua_u16 avg;
    bua_u16 old_part;
    bua_u16 new_part;
    st->samples[st->next] = unlimited;
    st->next = (bua_u8)((st->next + 1u) & 3u);
    sum = (unsigned long)st->samples[0] + (unsigned long)st->samples[1] +
          (unsigned long)st->samples[2] + (unsigned long)st->samples[3];
    avg = (bua_u16)(sum >> 2);
    old_part = bua_mul_u8_u16_lf266((bua_u8)(0u - CAL_MAF_SLIDING_FILTER),
                                    st->filtered);
    new_part = bua_mul_u8_u16_lf266(CAL_MAF_SLIDING_FILTER, avg);
    st->filtered = (bua_u16)(old_part + new_part);
    return st->filtered;
}
static bua_u16 bua_maf_apply_limits_lf862(bua_u16 airflow_q8_8, bua_u8 rpm25)
{
    bua_u8 max_gps;
    bua_u16 max_q8_8;
    if (airflow_q8_8 < CAL_MAF_MIN_FLOW_Q8_8)
        airflow_q8_8 = CAL_MAF_MIN_FLOW_Q8_8;
    max_gps = bua_lookup_spacing16(cal_maf_max_flow, 17u, rpm25);
    max_q8_8 = (bua_u16)((bua_u16)max_gps << 8);
    if ((bua_u8)(airflow_q8_8 >> 8) >= max_gps)
        airflow_q8_8 = max_q8_8;
    return airflow_q8_8;
}
static bua_u16 bua_maf_default_lf7c1(bua_u8 rpm_special,
                                      bua_u8 tps_load,
                                      bua_u8 iac_position)
{
    bua_u8 per_tps;
    bua_u8 tps_used;
    unsigned long d;
    per_tps = bua_interp_u8_segment(cal_maf_default_rpm, 8u, rpm_special);
    tps_used = tps_load;
    if (tps_used > CAL_MAF_DEFAULT_TPS_LIMIT)
        tps_used = CAL_MAF_DEFAULT_TPS_LIMIT;
    d = (unsigned long)per_tps * (unsigned long)tps_used;
    d += (unsigned long)iac_position * (unsigned long)CAL_MAF_DEFAULT_IAC_SCALE;
    d += (unsigned long)CAL_MAF_DEFAULT_OFFSET_Q8_8;
    if (d > 65535ul) d = 65535ul;
    return (bua_u16)d;
}
static BuaMafResult bua_maf_step30_analog(BuaMafFilterState *st,
                                           bua_u8 maf_adc,
                                           bua_u8 rpm25)
{
    BuaMafResult r;
    r.used_default = 0u;
    r.unlimited_q8_8 = bua_maf_analog_unlimited_lf7ac(maf_adc,
                                                       &r.table_number,
                                                       &r.table_raw,
                                                       &r.scalar,
                                                       &r.raw_maf_axis);
    if ((CAL_AF_MODE1 & AF_USE_FLOW_FILTER_BIT) != 0u)
        r.filtered_q8_8 = bua_maf_filter_lf80x(st, r.unlimited_q8_8);
    else
        r.filtered_q8_8 = r.unlimited_q8_8;
    r.final_q8_8 = bua_maf_apply_limits_lf862(r.filtered_q8_8, rpm25);
    return r;
}
