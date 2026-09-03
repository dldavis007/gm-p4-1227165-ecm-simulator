static void run_step30_maf_test(void)
{
    unsigned int passed;
    bua_u16 q;
    bua_u8 tn;
    bua_u8 raw;
    bua_u8 sc;
    bua_u16 axis;
    BuaMafFilterState st;
    BuaMafResult r;
    unsigned int i;
    passed = 0u;
    printf("\nMAF / airflow regression (LF76A..LF87F):\n");
    q = bua_maf_analog_unlimited_lf7ac(64u, &tn, &raw, &sc, &axis);
    if (axis == 448u && tn == 1u && raw == 197u && sc == 23u && q == 4531u) ++passed;
    printf("  ADC64: axis448 -> table1 raw197*23 = 4531 = %u.%03u g/s  %s\n",
           (unsigned int)(q >> 8), (unsigned int)(((q & 255u) * 1000u) / 256u),
           (axis == 448u && tn == 1u && raw == 197u && sc == 23u && q == 4531u) ? "PASS" : "FAIL");
    q = bua_maf_analog_unlimited_lf7ac(96u, &tn, &raw, &sc, &axis);
    if (tn == 2u && raw == 198u && sc == 48u && q == 9504u) ++passed;
    printf("  ADC96: table2 anchor -> 198*48 = 9504 = %u.%03u g/s  %s\n",
           (unsigned int)(q >> 8), (unsigned int)(((q & 255u) * 1000u) / 256u),
           (tn == 2u && raw == 198u && sc == 48u && q == 9504u) ? "PASS" : "FAIL");
    q = bua_maf_analog_unlimited_lf7ac(128u, &tn, &raw, &sc, &axis);
    if (tn == 3u && raw == 195u && sc == 83u && q == 16185u) ++passed;
    printf("  ADC128: table3 anchor -> 195*83 = 16185 = %u.%03u g/s  %s\n",
           (unsigned int)(q >> 8), (unsigned int)(((q & 255u) * 1000u) / 256u),
           (tn == 3u && raw == 195u && sc == 83u && q == 16185u) ? "PASS" : "FAIL");
    q = bua_maf_analog_unlimited_lf7ac(160u, &tn, &raw, &sc, &axis);
    if (tn == 4u && raw == 187u && sc == 135u && q == 25245u) ++passed;
    printf("  ADC160: table4 anchor -> 187*135 = 25245 = %u.%03u g/s  %s\n",
           (unsigned int)(q >> 8), (unsigned int)(((q & 255u) * 1000u) / 256u),
           (tn == 4u && raw == 187u && sc == 135u && q == 25245u) ? "PASS" : "FAIL");
    q = bua_maf_analog_unlimited_lf7ac(192u, &tn, &raw, &sc, &axis);
    if (tn == 5u && raw == 186u && sc == 207u && q == 38502u) ++passed;
    printf("  ADC192: table5 anchor -> 186*207 = 38502 = %u.%03u g/s  %s\n",
           (unsigned int)(q >> 8), (unsigned int)(((q & 255u) * 1000u) / 256u),
           (tn == 5u && raw == 186u && sc == 207u && q == 38502u) ? "PASS" : "FAIL");
    q = bua_maf_analog_unlimited_lf7ac(224u, &tn, &raw, &sc, &axis);
    if (tn == 6u && raw == 219u && sc == 255u && q == 55845u) ++passed;
    printf("  ADC224: table6 anchor -> 219*255 = 55845 = %u.%03u g/s  %s\n",
           (unsigned int)(q >> 8), (unsigned int)(((q & 255u) * 1000u) / 256u),
           (tn == 6u && raw == 219u && sc == 255u && q == 55845u) ? "PASS" : "FAIL");
    if ((CAL_AF_MODE1 & AF_ANALOG_MAF_BIT) != 0u &&
        (CAL_AF_MODE1 & AF_USE_FLOW_FILTER_BIT) != 0u) ++passed;
    printf("  LC014=%02X selects analog MAF and sliding filter  %s\n",
           (unsigned int)CAL_AF_MODE1,
           (((CAL_AF_MODE1 & AF_ANALOG_MAF_BIT) != 0u) &&
            ((CAL_AF_MODE1 & AF_USE_FLOW_FILTER_BIT) != 0u)) ? "PASS" : "FAIL");
    for (i = 0u; i < 4u; ++i) st.samples[i] = 0u;
    st.next = 0u;
    st.filtered = 0u;
    for (i = 0u; i < 4u; ++i) (void)bua_maf_filter_lf80x(&st, 8192u);
    q = bua_maf_filter_lf80x(&st, 8192u);
    if (q > 0u && q < 8192u) ++passed;
    printf("  enabled filter moves gradually toward steady 32 g/s: q=%u  %s\n",
           (unsigned int)q, (q > 0u && q < 8192u) ? "PASS" : "FAIL");
    q = bua_maf_apply_limits_lf862(100u, 160u);
    if (q == 768u) ++passed;
    printf("  minimum airflow clamp: 100 -> 768 (3.0 g/s)  %s\n",
           (q == 768u) ? "PASS" : "FAIL");
    q = bua_maf_apply_limits_lf862(60000u, 160u);
    if (q == (bua_u16)(220u << 8)) ++passed;
    printf("  4000-RPM max-flow clamp -> 220 g/s  %s\n",
           (q == (bua_u16)(220u << 8)) ? "PASS" : "FAIL");
    q = bua_maf_default_lf7c1(80u, 200u, 20u);
    if (q > CAL_MAF_DEFAULT_OFFSET_Q8_8) ++passed;
    printf("  ERR33/34 fallback uses TPS cap + IAC + 4-g/s offset: q=%u  %s\n",
           (unsigned int)q, (q > CAL_MAF_DEFAULT_OFFSET_Q8_8) ? "PASS" : "FAIL");
    for (i = 0u; i < 4u; ++i) st.samples[i] = 0u;
    st.next = 0u;
    st.filtered = CAL_MAF_MIN_FLOW_Q8_8;
    r = bua_maf_step30_analog(&st, 128u, 160u);
    if (r.raw_maf_axis == 896u && r.table_number == 3u &&
        r.unlimited_q8_8 == 16185u && r.final_q8_8 >= CAL_MAF_MIN_FLOW_Q8_8) ++passed;
    printf("  integrated analog step: ADC128 -> axis896 -> L00EA candidate %u  %s\n",
           (unsigned int)r.final_q8_8,
           (r.raw_maf_axis == 896u && r.table_number == 3u &&
            r.unlimited_q8_8 == 16185u && r.final_q8_8 >= CAL_MAF_MIN_FLOW_Q8_8) ? "PASS" : "FAIL");
    printf("  step-30 MAF regression result: %s (%u/12)\n",
           (passed == 12u) ? "PASS" : "FAIL", passed);
}
