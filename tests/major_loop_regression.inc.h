static void run_step90_major_wiring_test(void)
{
    BuaMemory saved_mem=mem;
    BuaStats saved_stats=stats;
    unsigned int pass=0u,total=0u;
#define S90(C) do { ++total; if(C) ++pass; else printf("  step90 FAIL line %d\n",__LINE__); } while(0)
    memset(&mem,0,sizeof(mem)); memset(&stats,0,sizeof(stats));
    printf("Step-90 Segment 1/5/6/A/E scheduler wiring regression:\n");

    /* Segment 6 consumes A/D channel 4 through the exact dual-range producer. */
    major_segment(0x06u);
    S90(RAM8(0x005Du)==80u); /* hw raw120 -> +10 -> FEA7 interpolation */
    S90(RAM8(0x005Bu)==80u && (RAM8(0x003Bu)&0x10u)!=0u);

    /* Segment A now produces MAT as complemented A/D data in L012B/L0060. */
    major_segment(0x0Au);
    S90(RAM8(0x012Bu)==255u && RAM8(0x0060u)==255u);

    /* Segment E refreshes battery and clears an already-set ignition-off bit. */
    RAM8(0x0033u)=0x10u;
    major_segment(0x0Eu);
    S90(RAM8(0x007Eu)==128u && (RAM8(0x0033u)&0x10u)==0u);
    S90(RAM8(0x00B7u)==10u && RAM8(0x00B8u)==50u);

    /* Segment 1 high-voltage qualifier is live RAM state (manually seed 171). */
    RAM8(0x007Eu)=171u; RAM8(0x0035u)=0u;
    major_segment(0x01u);
    S90((RAM8(0x0035u)&0x40u)!=0u && (RAM8(0x003Eu)&0x10u)!=0u);
    major_segment(0x01u);
    S90((RAM8(0x004Fu)&0x40u)!=0u);

    /* Segment 5: bit4 clear executes fan/A-C half; set executes CL half. */
    MINOR_COUNT=0x05u; RAM8(0x0035u)=0x02u; RAM8(0x005Bu)=100u;
    major_segment(0x05u);
    S90(RAM8(0x00F4u)==255u && RAM8(0x00F5u)==75u);
    MINOR_COUNT=0x15u; RAM8(0x0034u)=0x80u; RAM8(0x0001u)=0x03u;
    RAM8(0x005Bu)=120u; RAM8(0x005Fu)=120u; RAM8(0x0041u)=0u;
    RAM8(0x0035u)=0u; RAM8(0x0033u)=0u; RAM8(0x00BEu)=0u;
    major_segment(0x05u);
    S90((RAM8(0x0044u)&0x80u)!=0u);

    S90(stats.major_segment_calls[1]==2u && stats.major_segment_calls[5]==2u);
    S90(stats.major_segment_calls[6]==1u && stats.major_segment_calls[10]==1u &&
        stats.major_segment_calls[14]==1u);
    printf("  result: %s (%u/%u)\n",pass==total?"PASS":"FAIL",pass,total);
    mem=saved_mem; stats=saved_stats;
#undef S90
}

static void run_step100_remaining_normal_major_test(void)
{
    BuaMemory saved_mem;
    BuaStats saved_stats;
    bua_u8 saved_diag;
    bua_u8 saved_maf;
    bua_u16 expected;
    unsigned int passed;
    unsigned int total;
#define S100(C,T) do { ++total; if(C) ++passed; printf("  %-82s %s\n",T,(C)?"PASS":"FAIL"); } while(0)
    saved_mem=mem;
    saved_stats=stats;
    saved_diag=sim_diag_adc;
    saved_maf=sim_maf_adc;
    passed=0u;
    total=0u;
    memset(&mem,0,sizeof(mem));
    memset(&stats,0,sizeof(stats));
    printf("\nStep-100 remaining normal major-producer regression (LEA28..LEB39 / LF3B6):\n");

    sim_diag_adc=20u;
    RAM8(0x005Bu)=120u;
    RAM8(0x0074u)=0x55u;
    seg3_misc_100ms();
    S100((RAM8(0x0035u)&0x10u)!=0u && (RAM8(0x0035u)&0x20u)==0u,
         "channel-7 below 40 selects grounded diagnostic-switch bit only");
    S100(ram16be_get(0x006Fu)==0x6600u && ram16be_get(0x0073u)==0x6655u &&
         RAM8(0x005Fu)==120u && (RAM8(0x0001u)&0x40u)!=0u,
         "not-running Segment 3 initializes O2/startup-coolant state literally");

    sim_diag_adc=120u;
    RAM8(0x0046u)=0u;
    seg3_misc_100ms();
    S100((RAM8(0x0035u)&0x20u)!=0u && (RAM8(0x0035u)&0x10u)==0u,
         "channel-7 100..151 selects ALDL mode when 8192 mode is unlocked");
    RAM8(0x0046u)=0x08u;
    seg3_misc_100ms();
    S100((RAM8(0x0035u)&0x30u)==0u,
         "locked 8192 mode suppresses the channel-7 ALDL selection");

    sim_diag_adc=200u;
    ENGINE_MODE_WORD=ENGINE_RUNNING_BIT;
    RAM8(0x0002u)=0x10u;
    ram16be_set(0x006Fu,0xC800u);
    ram16be_set(0x0073u,0x6400u);
    expected=bua_lag_filter_8_8(0x6400u,200u,5u);
    seg3_misc_100ms();
    S100(ram16be_get(0x0073u)==expected && (RAM8(0x0002u)&0x10u)==0u &&
         mem.io4000[3]==0x8Fu,
         "running Segment 3 applies LC39F=5 O2 filter and writes L4003=$8F");

    RAM8(0x005Bu)=100u;
    RAM8(0x0033u)=0u;
    seg7_coolant_variables();
    S100(RAM8(0x005Eu)==100u &&
         RAM8(0x00D1u)==bua_lookup_spacing16(cal_o2_lean_offset_c5bb,14u,100u) &&
         RAM8(0x00D7u)==bua_lookup_spacing32(cal_async_pulse_count,9u,100u) &&
         RAM8(0x00D8u)==bua_lookup_spacing32(cal_async_cool_factor,9u,100u) &&
         RAM8(0x00D0u)==bua_lookup_spacing32(cal_pe_coolant_change,9u,100u),
         "Segment 7 produces lean offset, async count/factor, and PE coolant term");
    S100(mpu16be_get(0x3FDAu)==0xD000u,
         "ordinary ignition-on Segment 7 leaves the raw burn-off MPU count at $D000");
    RAM8(0x005Bu)=250u;
    seg7_coolant_variables();
    S100(RAM8(0x005Eu)==208u,
         "Segment 7 clamps only the C5BB rich/lean lookup argument at 208");

    RAM8(0x0033u)=0x10u;
    RAM8(0x003Eu)=0x40u;
    RAM8(0x00B8u)=1u;
    RAM8(0x00B7u)=2u;
    seg7_coolant_variables();
    S100(RAM8(0x00B8u)==0u && RAM8(0x00B7u)==2u &&
         mpu16be_get(0x3FDAu)==0xD000u,
         "key-off burn-off consumes delay before burn duration");
    seg7_coolant_variables();
    S100(RAM8(0x00B7u)==1u && mpu16be_get(0x3FDAu)==0xDFFFu &&
         RAM8(0x011Fu)==1u,
         "active burn-off writes raw MPU $DFFF and advances its diagnostic delay");
    RAM8(0x00B7u)=1u;
    RAM8(0x011Fu)=3u;
    RAM8(0x0003u)=0x40u;
    sim_maf_adc=50u;
    seg7_coolant_variables();
    S100((RAM8(0x0003u)&0x40u)==0u,
         "in-range burn-off MAF sample clears the source error-36 flag");

    memset(&stats,0,sizeof(stats));
    IGN_MODE_WORD=0x80u;
    major_segment(0x06u);
    S100(stats.major_segment_calls[6]==1u && stats.hu_5806_calls==1u,
         "LF3B6 invokes the Segment-4/H.U. hook before coolant conversion");
    printf("  step-100 remaining-normal-major regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
    mem=saved_mem;
    stats=saved_stats;
    sim_diag_adc=saved_diag;
    sim_maf_adc=saved_maf;
#undef S100
}

