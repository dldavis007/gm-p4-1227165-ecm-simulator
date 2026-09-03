static void run_step89_scheduler_wiring_test(void)
{
    BuaMemory saved_mem;
    BuaStats saved_stats;
    unsigned int pass=0u,total=0u,i;
#define S89(C) do { ++total; if(C) ++pass; else printf("  step89 FAIL line %d\n",__LINE__); } while(0)
    saved_mem=mem; saved_stats=stats;
    memset(&mem,0,sizeof(mem)); memset(&stats,0,sizeof(stats));
    printf("Step-89 major-loop Segment 9/B/C scheduler wiring regression:\n");

    /* Segment 9: ordinary warm/open-loop path writes both raw AIR request bits. */
    RAM8(0x005Bu)=100u; RAM8(0x0034u)=0u; RAM8(0x0056u)=100u;
    RAM8(0x0063u)=50u; RAM8(0x0073u)=100u; RAM8(0x0065u)=30u;
    major_segment(0x09u);
    S89((RAM8(0x003Eu)&0x06u)==0x06u);
    S89(RAM8(0x00FBu)==50u && stats.major_segment_calls[9]==1u);

    /* Segment B: qualified warm EGR is full command with this calibration. */
    RAM8(0x0037u)=0u; RAM8(0x0035u)=0u; RAM8(0x0060u)=100u;
    RAM8(0x0082u)=50u; RAM8(0x0043u)=0u; RAM8(0x0057u)=60u;
    RAM8(0x0063u)=80u; RAM8(0x005Bu)=160u; RAM8(0x0112u)=0u;
    major_segment(0x0Bu);
    S89(RAM8(0x0112u)==255u && stats.major_segment_calls[11]==1u);

    /* Segment C: runtime latch and filtered purge command become live RAM state. */
    RAM8(0x001Bu)=81u; RAM8(0x005Bu)=200u; RAM8(0x0043u)=0u;
    RAM8(0x0044u)=0x80u; RAM8(0x0041u)=0u; RAM8(0x0067u)=30u;
    RAM8(0x0082u)=50u; ram16be_set(0x00EAu,0x1000u); RAM8(0x0063u)=100u;
    major_segment(0x0Cu);
    S89((RAM8(0x0001u)&0x10u)!=0u);
    S89(RAM8(0x0113u)==20u && (RAM8(0x003Bu)&0x01u)!=0u);
    S89(stats.major_segment_calls[12]==1u);

    /* Dispatcher geometry: ten complete 0..F sweeps call every major segment ten times. */
    memset(&stats,0,sizeof(stats));
    for(i=0u;i<160u;++i) major_segment((bua_u8)(i&0x0Fu));
    for(i=0u;i<16u;++i) S89(stats.major_segment_calls[i]==10u);

    printf("  result: %s (%u/%u)\n",pass==total?"PASS":"FAIL",pass,total);
    mem=saved_mem; stats=saved_stats;
#undef S89
}

/* ---------------------------------------------------------------------- */
/* Step 92: IRQ/minor-loop cadence and scheduler-facing IAC service.      */
/* ---------------------------------------------------------------------- */
static void run_step92_minor_cadence_test(void)
{
    unsigned int passed = 0u;
    unsigned int total = 12u;
    unsigned int i;
    bua_u32 air0, spark0, iac0, steps0;
    bua_u32 majors0[16];
#define STEP92_CHECK(c,t) do { if (c) ++passed; printf("  %-84s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-92 IRQ/minor-loop cadence + scheduler IAC-service regression:\n");
    ecm_reset();
    /* Keep the normal-running IAC hardware gate valid and install four
     * closing steps in the literal source RAM locations. */
    ENGINE_MODE_WORD |= ENGINE_RUNNING_BIT;
    RAM8(0x007Eu) = 124u;
    RAM8(0x002Cu) = 50u;
    RAM8(0x010Au) = 10u;
    RAM8(0x0101u) = 0x84u;
    sim_iac_motor_on = 1u;
    air0=stats.air_fuel_loops; spark0=stats.spark_vss_loops;
    iac0=stats.iac_minor_services; steps0=stats.iac_steps_consumed;
    for(i=0u;i<16u;++i) majors0[i]=stats.major_segment_calls[i];
    for(i=0u;i<16u;++i) irq_6p25ms();
    STEP92_CHECK(stats.irq_ticks==16u,"16 calls produce 16 ordinary 6.25-ms IRQ services");
    STEP92_CHECK(stats.iac_minor_services-iac0==16u,"LF4DF/IAC service is entered on every ordinary IRQ before minor split");
    STEP92_CHECK(stats.air_fuel_loops-air0==8u,"odd minor counts execute Air/Fuel exactly eight times in 16 IRQs");
    STEP92_CHECK(stats.spark_vss_loops-spark0==8u,"even minor counts execute Spark/O2/VSS exactly eight times in 16 IRQs");
    { int allone=1; for(i=0u;i<16u;++i) if(stats.major_segment_calls[i]-majors0[i]!=1u) allone=0;
      STEP92_CHECK(allone,"low nibble dispatch executes each major segment once per 16 IRQs"); }
    STEP92_CHECK(stats.iac_steps_consumed-steps0==4u,"persistent packed L0101 request consumes one IAC step per 6.25-ms service");
    STEP92_CHECK(RAM8(0x002Cu)==46u,"four closing steps move literal L002C present position 50 -> 46");
    STEP92_CHECK(RAM8(0x010Au)==6u,"four closing steps move literal L010A running counter 10 -> 6");
    STEP92_CHECK(RAM8(0x0101u)==0x80u,"DEC L0101 preserves direction bit after four requested closing steps");
    STEP92_CHECK((MINOR_COUNT&1u)==0u,"after 16 IRQs odd/even phase returns to its starting parity");
    STEP92_CHECK((stats.air_fuel_loops+stats.spark_vss_loops)==16u,"exactly one 12.5-ms-side branch is selected on every IRQ");
    STEP92_CHECK(stats.iac_minor_services==stats.irq_ticks,"normal test path has one IAC service for every ordinary IRQ");
    printf("  step-92 IRQ/minor-loop cadence regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP92_CHECK
}

/* ---------------------------------------------------------------------- */
/* Step 93: LCD05 slow-O2 front end and even-path cadence.                */
/* ---------------------------------------------------------------------- */
static void run_step93_even_path_test(void)
{
    unsigned int passed = 0u;
    unsigned int total = 14u;
    unsigned int i;
    bua_u16 o2_before;
    bua_u16 o2_after_odd;
    bua_u32 air0, spark0, iac0, o2calls0, rpm0, dwell0;
    bua_u32 majors0[16];
#define STEP93_CHECK(c,t) do { if (c) ++passed; printf("  %-84s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-93 LCD05 diagnostic merge + slow-O2 + even reference/RPM/dwell cadence regression:\n");

    ecm_reset();
    O2_LOOKUP_AIRFLOW = 0u;
    COOLANT = 100u;
    O2_FILTERED_FAST = 200u;
    sim_o2_adc = 200u;
    ram16be_set(0x0071u, (bua_u16)(100u << 8));
    MINOR_MODE_WORD2 = (bua_u8)(MINOR_MODE_WORD2 | 0x04u);
    DIAG_MODE_WORD2 = 0x80u;
    o2_before = ram16be_get(0x0071u);
    irq_6p25ms();                 /* 0x0E -> 0x0F: odd Air/Fuel branch */
    o2_after_odd = ram16be_get(0x0071u);
    STEP93_CHECK(o2_after_odd == o2_before,
                 "odd Air/Fuel loop does not alter LCD05 L0071 slow-O2 state");
    STEP93_CHECK(stats.slow_o2_filter_calls == 0u,
                 "odd Air/Fuel loop does not call the even-path slow-O2 filter");
    STEP93_CHECK(DIAG_MODE_WORD2 == 0x80u,
                 "odd Air/Fuel loop does not perform the LCD05 diagnostic-word merge");
    /* The now-connected odd fuel path legitimately refreshes L00D2 from
     * current airflow.  Restore the explicit LC58B=16 arithmetic vector
     * before testing the following even LCD05 operation. */
    O2_LOOKUP_AIRFLOW = 0u;
    irq_6p25ms();                 /* 0x0F -> 0x10: even Spark/O2 branch */
    STEP93_CHECK(stats.slow_o2_filter_calls == 1u,
                 "slow-O2 front end executes once on the following even minor loop");
    STEP93_CHECK(ram16be_get(0x0071u) == 26600u,
                 "LC58B=16, coolant scaling=160, LF250 coefficient=10 gives exact L0071 result");
    STEP93_CHECK(DIAG_MODE_WORD2 == 0x84u,
                 "even LCD05 entry ORs L0035 b2 into L0040 while preserving prior bits");

    ecm_reset();
    sim_set_ecm_reference_rpm(2400u);
    ENGINE_MODE_WORD |= ENGINE_RUNNING_BIT;
    O2_LOOKUP_AIRFLOW = 64u;
    COOLANT = 160u;
    O2_FILTERED_FAST = 180u;
    ram16be_set(0x0071u, (bua_u16)(100u << 8));
    air0=stats.air_fuel_loops; spark0=stats.spark_vss_loops;
    iac0=stats.iac_minor_services; o2calls0=stats.slow_o2_filter_calls;
    rpm0=stats.rpm_calculations; dwell0=stats.dwell_calculations;
    for(i=0u;i<16u;++i) majors0[i]=stats.major_segment_calls[i];
    for(i=0u;i<16u;++i) irq_6p25ms();
    STEP93_CHECK(stats.air_fuel_loops-air0==8u,
                 "16 IRQs retain eight odd Air/Fuel selections");
    STEP93_CHECK(stats.spark_vss_loops-spark0==8u,
                 "16 IRQs retain eight even Spark/O2 selections");
    STEP93_CHECK(stats.slow_o2_filter_calls-o2calls0==8u,
                 "slow-O2 filter executes exactly at the 12.5-ms even cadence");
    STEP93_CHECK(stats.iac_minor_services-iac0==16u,
                 "16 IRQs retain sixteen scheduler-facing IAC services");
    { int allone=1; for(i=0u;i<16u;++i) if(stats.major_segment_calls[i]-majors0[i]!=1u) allone=0;
      STEP93_CHECK(allone,"16 IRQs retain one call to each major segment"); }
    STEP93_CHECK(stats.rpm_calculations-rpm0==8u,
                 "connected reference/RPM helper updates at the even 12.5-ms cadence");
    STEP93_CHECK(stats.dwell_calculations-dwell0==8u,
                 "connected dwell helper updates at the even 12.5-ms cadence");
    STEP93_CHECK(ram16be_get(0x0071u)!=(bua_u16)(100u<<8),
                 "repeated even loops evolve persistent L0071 filter state");
    printf("  step-93 even-path regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP93_CHECK
}

/* ---------------------------------------------------------------------- */
/* Step 94: common LF76A MAF plus odd normal-running fuel-chain cadence.  */
/* ---------------------------------------------------------------------- */
static void run_step94_odd_fuel_path_test(void)
{
    unsigned int passed=0u,total=12u,i;
    bua_u32 maf0,odd0,spark0,iac0,writes0;
    bua_u16 o2_after_odd;
    bua_u16 fuel_low,fuel_high,flow_low,flow_high;
#define STEP94_CHECK(c,t) do { if(c) ++passed; printf("  %-84s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-94 LF76A common MAF + LD6D1 normal odd-fuel-chain regression:\n");
    ecm_reset();
    sim_maf_adc=128u; sim_o2_adc=180u;
    ENGINE_MODE_WORD|=ENGINE_RUNNING_BIT;
    MODE_WORD3|=FUEL_VATS_OK_BIT;
    ram16be_set(REF_PERIOD_ADDR,410u);
    RPM_DIV25=96u; RPM_SPECIAL=96u; FILTERED_LOAD=128u; TPS_LOAD_AXIS=80u;
    COOLANT=160u; RAM8(0x00CDu)=25u; BLM=128u;
    CLOSED_LOOP_INTEGRATOR=128u; CLOSED_LOOP_PW_CORR=128u;
    AF_MODE_WORD2|=AF_CLOSED_LOOP_BIT;
    ram16be_set(0x006Fu,(bua_u16)(100u<<8));
    ram16be_set(0x0071u,(bua_u16)(100u<<8));
    irq_6p25ms(); /* odd */
    o2_after_odd=ram16be_get(0x006Fu);
    STEP94_CHECK(stats.common_maf_calls==1u,"first IRQ executes LF76A common MAF before branch selection");
    STEP94_CHECK(stats.odd_fuel_chain_calls==1u,"first odd IRQ executes one LD6D1 normal fuel chain");
    STEP94_CHECK(ram16be_get(AIRFLOW_WORD_ADDR)>CAL_MAF_MIN_FLOW_Q8_8,
                 "common analog-MAF producer updates source RAM L00EA");
    STEP94_CHECK(o2_after_odd!=(bua_u16)(100u<<8),"odd LD9A9 fast-O2 filter updates L006F");
    STEP94_CHECK(ram16be_get(TOTAL_AFR_ADDR)==445u,"closed-loop normal AFR selection writes stoichiometric L00CE=445");
    STEP94_CHECK(ram16be_get(0x00C2u)>0u && mpu16be_get(0x3FD0u)>0u,
                 "base/correction/output chain writes nonzero L00C2 and MPU L3FD0");
    irq_6p25ms(); /* even */
    STEP94_CHECK(stats.common_maf_calls==2u && stats.odd_fuel_chain_calls==1u,
                 "following even IRQ repeats common MAF but not the odd fuel chain");
    STEP94_CHECK(ram16be_get(0x006Fu)==o2_after_odd,
                 "even LCD05 path does not execute odd LD9A9 fast-O2 filtering");

    maf0=stats.common_maf_calls; odd0=stats.odd_fuel_chain_calls;
    spark0=stats.spark_vss_loops; iac0=stats.iac_minor_services;
    writes0=stats.odd_mpu_fuel_writes;
    for(i=0u;i<16u;++i) irq_6p25ms();
    STEP94_CHECK(stats.common_maf_calls-maf0==16u && stats.iac_minor_services-iac0==16u,
                 "16 IRQs execute 16 common MAF and 16 IAC services");
    STEP94_CHECK(stats.odd_fuel_chain_calls-odd0==8u && stats.odd_mpu_fuel_writes-writes0==8u,
                 "16 IRQs execute eight odd fuel chains and eight MPU fuel writes");
    STEP94_CHECK(stats.spark_vss_loops-spark0==8u,
                 "16-IRQ cadence retains eight even Spark/O2 selections");

    ecm_reset(); ENGINE_MODE_WORD|=ENGINE_RUNNING_BIT; MODE_WORD3|=FUEL_VATS_OK_BIT;
    ram16be_set(REF_PERIOD_ADDR,410u); RPM_DIV25=96u; RPM_SPECIAL=96u;
    FILTERED_LOAD=128u; TPS_LOAD_AXIS=80u; COOLANT=160u; BLM=128u;
    CLOSED_LOOP_PW_CORR=128u; sim_maf_adc=64u;
    bua_maf_filter_seed(&step94_maf_state,
                        bua_maf_analog_unlimited_lf7ac(sim_maf_adc,0,0,0,0));
    irq_6p25ms();
    flow_low=ram16be_get(AIRFLOW_WORD_ADDR); fuel_low=mpu16be_get(0x3FD0u);
    ecm_reset(); ENGINE_MODE_WORD|=ENGINE_RUNNING_BIT; MODE_WORD3|=FUEL_VATS_OK_BIT;
    ram16be_set(REF_PERIOD_ADDR,410u); RPM_DIV25=96u; RPM_SPECIAL=96u;
    FILTERED_LOAD=128u; TPS_LOAD_AXIS=80u; COOLANT=160u; BLM=128u;
    CLOSED_LOOP_PW_CORR=128u; sim_maf_adc=160u;
    bua_maf_filter_seed(&step94_maf_state,
                        bua_maf_analog_unlimited_lf7ac(sim_maf_adc,0,0,0,0));
    irq_6p25ms();
    flow_high=ram16be_get(AIRFLOW_WORD_ADDR); fuel_high=mpu16be_get(0x3FD0u);
    STEP94_CHECK(flow_high>flow_low && fuel_high>fuel_low,
                 "same odd pass consumes current common MAF: higher ADC raises L00EA and MPU fuel");
    printf("  step-94 odd-path regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP94_CHECK
}

/* ---------------------------------------------------------------------- */
/* Step 95: LD769..LD7A0 source-ordered load production on odd loops.     */
/* ---------------------------------------------------------------------- */
static void run_step95_load_producer_test(void)
{
    unsigned int passed=0u,total=10u,i;
    bua_u32 load0,odd0,even0,maf0,iac0;
    bua_u8 load_after_odd;
#define STEP95_CHECK(c,t) do { if(c) ++passed; printf("  %-84s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-95 LD769..LD7A0 odd-loop load-producer regression:\n");

    ecm_reset();
    ENGINE_MODE_WORD|=ENGINE_RUNNING_BIT;
    TPS_LOAD_AXIS=73u; RAM8(0x0083u)=11u;
    FILTERED_LOAD=33u; RAM8(0x0061u)=22u; RAM8(0x0062u)=11u;
    ram16be_set(AIRFLOW_WORD_ADDR,16185u);
    ram16be_set(REF_PERIOD_ADDR,410u);
    bua_odd_load_producer_12p5ms();
    STEP95_CHECK(FILTERED_LOAD==127u,
                 "LF2F0 + factor-80 + double/round vector yields L0063=127");
    STEP95_CHECK(RAM8(0x0061u)==33u && RAM8(0x0062u)==22u,
                 "STD L0061 shifts old L0063 and old L0061 through load history");
    STEP95_CHECK(RAM8(0x0083u)==73u,
                 "LD769 preserves the current L0082 TPS-axis byte in L0083");

    ram16be_set(AIRFLOW_WORD_ADDR,65535u);
    ram16be_set(REF_PERIOD_ADDR,65535u);
    bua_odd_load_producer_12p5ms();
    STEP95_CHECK(FILTERED_LOAD==255u,
                 "overflowing LF472/double path saturates the 8-bit load at 255");

    ecm_reset(); sim_maf_adc=128u; sim_o2_adc=180u;
    ENGINE_MODE_WORD|=ENGINE_RUNNING_BIT; MODE_WORD3|=FUEL_VATS_OK_BIT;
    ram16be_set(REF_PERIOD_ADDR,410u); RPM_DIV25=96u; RPM_SPECIAL=96u;
    TPS_LOAD_AXIS=80u; COOLANT=160u; BLM=128u; CLOSED_LOOP_PW_CORR=128u;
    bua_maf_filter_seed(&step94_maf_state,
                        bua_maf_analog_unlimited_lf7ac(sim_maf_adc,0,0,0,0));
    irq_6p25ms();
    load_after_odd=FILTERED_LOAD;
    STEP95_CHECK(stats.odd_load_producer_calls==1u && load_after_odd!=0u,
                 "first odd IRQ consumes the current common-MAF result into L0063");
    irq_6p25ms();
    STEP95_CHECK(stats.odd_load_producer_calls==1u && FILTERED_LOAD==load_after_odd,
                 "following even IRQ does not execute or alter the odd load producer");

    load0=stats.odd_load_producer_calls; odd0=stats.air_fuel_loops;
    even0=stats.spark_vss_loops; maf0=stats.common_maf_calls;
    iac0=stats.iac_minor_services;
    for(i=0u;i<16u;++i) irq_6p25ms();
    STEP95_CHECK(stats.odd_load_producer_calls-load0==8u,
                 "16 IRQs execute the connected load producer exactly eight times");
    STEP95_CHECK(stats.air_fuel_loops-odd0==8u && stats.spark_vss_loops-even0==8u,
                 "16 IRQs retain eight odd and eight even minor-loop selections");
    STEP95_CHECK(stats.common_maf_calls-maf0==16u && stats.iac_minor_services-iac0==16u,
                 "16 IRQs retain sixteen common-MAF and sixteen IAC services");

    ecm_reset(); ENGINE_MODE_WORD&=(bua_u8)~ENGINE_RUNNING_BIT;
    ram16be_set(AIRFLOW_WORD_ADDR,65535u); ram16be_set(REF_PERIOD_ADDR,400u);
    FILTERED_LOAD=77u;
    bua_odd_load_producer_12p5ms();
    STEP95_CHECK(FILTERED_LOAD==0u,
                 "not-running LC357=0 clamp produces zero load without hardware inference");
    printf("  step-95 load-producer regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP95_CHECK
}

/* ---------------------------------------------------------------------- */
/* Step 96: normal LD86B..LD99E transient/DFCO front-end integration.     */
/* ---------------------------------------------------------------------- */
static void run_step96_transient_front_test(void)
{
    unsigned int passed=0u,total=13u,i;
    bua_u32 front0,odd0,even0;
    bua_u8 factor_after_odd;
#define STEP96_CHECK(c,t) do { if(c) ++passed; printf("  %-84s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-96 LD86B..LD99E odd transient/DFCO front-end regression:\n");

    ecm_reset();
    FILTERED_LOAD=128u; RAM8(0x00E5u)=128u;
    TPS_LOAD_AXIS=120u; RAM8(0x00DDu)=100u;
    RAM8(0x018Eu)=9u; RAM8(0x0190u)=9u; RAM8(0x01ACu)=9u;
    bua_odd_transient_front_12p5ms();
    STEP96_CHECK((RAM8(0x0038u)&0xE0u)==0xE0u && RAM8(0x00D4u)==20u,
                 "LFA1E uses L0082-L00DD and arms b7/b6/b5 on a +20 TPS step");
    STEP96_CHECK(RAM8(0x018Eu)==0u && RAM8(0x0190u)==0u && RAM8(0x01ACu)==0u,
                 "first TPS-AE trigger clears all sixteen alternating L018E..L01AC bytes");
    RAM8(0x0038u)|=0x80u; RAM8(0x00DFu)=3u;
    TPS_LOAD_AXIS=88u; RAM8(0x00DDu)=100u;
    bua_odd_transient_front_12p5ms();
    STEP96_CHECK((RAM8(0x0038u)&0x80u)==0u && RAM8(0x00DFu)==0u,
                 "negative TPS excursion beyond LC30F cancels TPS AE and async count");

    ecm_reset(); FILTERED_LOAD=100u; RAM8(0x00E5u)=0u;
    RAM8(0x00E7u)=128u; ACCEL_LOAD_FACTOR=10u;
    TPS_LOAD_AXIS=0u; RAM8(0x00DDu)=0u;
    bua_odd_transient_front_12p5ms();
    STEP96_CHECK(RAM8(0x00E1u)==80u && ACCEL_LOAD_FACTOR==44u,
                 "delta-load 100 stores E1=80 and C33D/E7 adds 34 to existing E2");
    STEP96_CHECK((RAM8(0x0038u)&0x08u)!=0u,
                 "qualifying differential-load enrichment sets L0038 b3");
    RAM8(0x00E5u)=101u;
    bua_odd_transient_front_12p5ms();
    STEP96_CHECK(ACCEL_LOAD_FACTOR==0u,
                 "current load below transient load clears the differential-load factor");

    ecm_reset(); FILTERED_LOAD=20u; RAM8(0x00E5u)=20u;
    VSS_MPH_HI=30u; TPS_LOAD_AXIS=0u; RAM8(0x00DDu)=0u;
    RPM_DIV25=60u; RPM_DIV12P5=100u; RAM8(0x005Au)=100u;
    RAM8(0x00D3u)=20u; AIR_MODE_WORD=0x10u;
    for(i=0u;i<20u;++i) bua_odd_transient_front_12p5ms();
    STEP96_CHECK(RAM8(0x00D3u)==0u && (AF_MODE_WORD&0x02u)==0u,
                 "twenty valid passes exhaust the DFCO qualification counter without entry");
    bua_odd_transient_front_12p5ms();
    STEP96_CHECK((AF_MODE_WORD&0x02u)!=0u && ram16be_get(0x0088u)==511u,
                 "next valid pass enters DFCO and loads the 511-count lockout");
    STEP96_CHECK((AIR_MODE_WORD&0x10u)==0u,
                 "DFCO entry preserves literal LD918 clearing of L003D b4");
    TPS_LOAD_AXIS=9u;
    bua_odd_transient_front_12p5ms();
    STEP96_CHECK((AF_MODE_WORD&0x02u)==0u && ram16be_get(0x0088u)==0u,
                 "TPS above LC600 exits DFCO and explicitly clears its lockout");

    ecm_reset(); sim_maf_adc=0u; sim_o2_adc=128u;
    ENGINE_MODE_WORD|=ENGINE_RUNNING_BIT; MODE_WORD3|=FUEL_VATS_OK_BIT;
    ram16be_set(0x0086u,(bua_u16)(35u<<8));
    ram16be_set(REF_PERIOD_ADDR,100u); RPM_DIV25=60u; RPM_DIV12P5=100u;
    RAM8(0x005Au)=100u; VSS_MPH_HI=30u; TPS_LOAD_AXIS=0u;
    RAM8(0x00DDu)=0u; RAM8(0x00D3u)=0u; COOLANT=160u;
    BLM=128u; CLOSED_LOOP_PW_CORR=128u;
    bua_maf_filter_seed(&step94_maf_state,CAL_MAF_MIN_FLOW_Q8_8);
    irq_6p25ms();
    STEP96_CHECK((AF_MODE_WORD&0x02u)!=0u && mpu16be_get(0x3FD0u)==0u,
                 "same odd pass enters DFCO before LF92A and therefore writes zero fuel");
    factor_after_odd=ACCEL_LOAD_FACTOR;
    irq_6p25ms();
    STEP96_CHECK(stats.odd_transient_front_calls==1u && ACCEL_LOAD_FACTOR==factor_after_odd,
                 "following even IRQ does not execute the odd transient front end");

    front0=stats.odd_transient_front_calls; odd0=stats.air_fuel_loops;
    even0=stats.spark_vss_loops;
    for(i=0u;i<16u;++i) irq_6p25ms();
    STEP96_CHECK(stats.odd_transient_front_calls-front0==8u &&
                 stats.air_fuel_loops-odd0==8u && stats.spark_vss_loops-even0==8u,
                 "16 IRQs retain eight transient fronts, eight odd, and eight even selections");
    printf("  step-96 transient-front regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP96_CHECK
}

/* ---------------------------------------------------------------------- */
/* Step 97: post-fuel 50-ms BLM/transient producer tail.                  */
/* ---------------------------------------------------------------------- */
static void run_step97_50ms_tail_test(void)
{
    unsigned int passed=0u,total=12u,i;
    bua_u32 tail0,filter0;
    bua_u16 load_before;
#define STEP97_CHECK(c,t) do { if(c) ++passed; printf("  %-84s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-97 LDDC2..LDE6A 50-ms BLM/transient-tail regression:\n");

    ecm_reset();
    MINOR_COUNT=3u; RAM8(0x00E4u)=0u;
    ram16be_set(TRANSIENT_LOAD_ADDR,0x8000u);
    FILTERED_LOAD=160u; RAM8(0x00E9u)=100u; TPS_LOAD_AXIS=120u;
    bua_odd_50ms_tail_lddc2();
    STEP97_CHECK(stats.odd_50ms_tail_calls==0u &&
                 ram16be_get(TRANSIENT_LOAD_ADDR)==0x8000u,
                 "odd count 3 bypasses the LDDC2 tail because L0000&6 is nonzero");

    MINOR_COUNT=1u; RPM_DIV12P5=77u; RAM8(0x005Au)=12u;
    RAM8(0x00C1u)=55u; RAM8(0x00BFu)=3u;
    AF_MODE_WORD|=AF_BLM_UPDATE_DELAY_BIT;
    bua_odd_50ms_tail_lddc2();
    STEP97_CHECK(stats.odd_50ms_tail_calls==1u,
                 "odd count 1 executes one source-shaped 50-ms tail");
    STEP97_CHECK(ram16be_get(TRANSIENT_LOAD_ADDR)==0x8400u,
                 "LC33A=32 filters transient load from 128.0 to 132.0 toward load160");
    STEP97_CHECK(RAM8(0x00E9u)==119u,
                 "LC5F3=255 filters transient TPS whole byte from 100 to 119");
    STEP97_CHECK(RAM8(0x00E4u)==1u && stats.transient_filter_updates==1u,
                 "negative DEC result services filters and reloads L00E4 from LC339 A=1");
    STEP97_CHECK(RAM8(0x005Au)==77u && RAM8(0x00C1u)==0u &&
                 (AF_MODE_WORD&AF_BLM_UPDATE_DELAY_BIT)==0u,
                 "tail stores old RPM, clears non-idle L00C1, and clears L0043 b3");
    MINOR_COUNT=9u;
    bua_odd_50ms_tail_lddc2();
    STEP97_CHECK(RAM8(0x00E4u)==0u && stats.transient_filter_updates==1u,
                 "next 50-ms tail decrements timer to zero and skips filter service");
    MINOR_COUNT=1u; load_before=ram16be_get(TRANSIENT_LOAD_ADDR);
    bua_odd_50ms_tail_lddc2();
    STEP97_CHECK(stats.transient_filter_updates==2u &&
                 ram16be_get(TRANSIENT_LOAD_ADDR)>load_before,
                 "following 50-ms tail wraps DEC negative and advances transient load again");

    ecm_reset(); MINOR_COUNT=1u; RAM8(0x00BFu)=3u; RAM8(0x001Fu)=128u;
    AF_MODE_WORD=0u; AF_MODE_WORD2=AF_BLM_STORE_ENABLE_BIT;
    CLOSED_LOOP_INTEGRATOR=134u; RAM8(0x00CAu)=27u;
    bua_odd_50ms_tail_lddc2();
    STEP97_CHECK(RAM8(0x001Fu)==130u && BLM==130u &&
                 stats.scheduler_blm_updates==1u,
                 "qualified tail raises selected BLM cell and current L00C0 by two");

    ecm_reset(); ENGINE_MODE_WORD|=ENGINE_RUNNING_BIT;
    MODE_WORD3|=FUEL_VATS_OK_BIT; ram16be_set(REF_PERIOD_ADDR,410u);
    RPM_DIV25=96u; RPM_DIV12P5=192u; RAM8(0x005Au)=192u;
    VSS_MPH_HI=30u; COOLANT=160u; BLM=128u; CLOSED_LOOP_PW_CORR=128u;
    TPS_LOAD_AXIS=90u; RAM8(0x00DDu)=90u; sim_maf_adc=150u;
    bua_maf_filter_seed(&step94_maf_state,
                        bua_maf_analog_unlimited_lf7ac(sim_maf_adc,0,0,0,0));
    tail0=stats.odd_50ms_tail_calls; filter0=stats.transient_filter_updates;
    for(i=0u;i<16u;++i) irq_6p25ms();
    STEP97_CHECK(stats.odd_50ms_tail_calls-tail0==2u,
                 "16 IRQs execute the post-fuel tail twice at a true 50-ms cadence");
    STEP97_CHECK(stats.transient_filter_updates-filter0==1u,
                 "LC339 timer services transient load/TPS once per two 50-ms tails");
    STEP97_CHECK(ram16be_get(TRANSIENT_LOAD_ADDR)!=0x8000u && RAM8(0x00E9u)>0u,
                 "live MAF/load and throttle now produce L00E5/L00E9 without manual seeding");
    printf("  step-97 50-ms-tail regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP97_CHECK
}

/* ---------------------------------------------------------------------- */
/* Step 98: common LF67B injector-service bookkeeping integration.        */
/* ---------------------------------------------------------------------- */
static void run_step98_injector_bookkeeping_test(void)
{
    unsigned int passed=0u,total=12u,i;
#define STEP98_CHECK(c,t) do { if(c) ++passed; printf("  %-84s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-98 LF67B..LF768 common injector-bookkeeping regression:\n");

    ecm_reset(); RAM8(0x00A0u)=0x40u;
    bua_common_injector_service_6p25ms();
    STEP98_CHECK(stats.common_injector_bookkeeping==0u &&
                 (RAM8(0x00A0u)&0x40u)==0u,
                 "no PC-HAL service clears sampled b6 and bypasses LF67B bookkeeping");

    ENGINE_MODE_WORD|=ENGINE_RUNNING_BIT;
    sim_ecm_ref.injector_services_pending=1u;
    ram16be_set(0x000Eu,0x2400u); ram16be_set(0x0010u,2u);
    RAM8(0x00CCu)=0u; STARTUP_COOLANT=96u;
    ram16be_set(STARTUP_SPARK_ADDR,0x1C00u);
    ram16be_set(STARTUP_SPARK_DELAY_ADDR,2u);
    ram16be_set(0x00C2u,300u); ram16be_set(0x011Cu,50u);
    ram16be_set(0x011Au,25u); mpu16be_set(0x3FD0u,1234u);
    bua_common_injector_service_6p25ms();
    STEP98_CHECK(stats.common_injector_bookkeeping==1u &&
                 sim_ecm_ref.injector_services_pending==0u &&
                 (RAM8(0x00A0u)&0x40u)!=0u,
                 "one pending physical service raises b6 and executes LF67B exactly once");
    STEP98_CHECK(ram16be_get(0x0010u)==1u && ram16be_get(0x000Eu)==0x2400u,
                 "running LF695 decrements startup-AFR initial delay before enrichment");
    STEP98_CHECK(ram16be_get(STARTUP_SPARK_DELAY_ADDR)==1u,
                 "same running service decrements startup-spark delay exactly once");
    STEP98_CHECK(RAM8(0x018Du)==1u && stats.running_injector_bookkeeping==1u,
                 "running injector occurrence increments L018D and running-service count");
    STEP98_CHECK(ram16be_get(0x00D5u)==300u && ram16be_get(0x011Au)==75u &&
                 (RAM8(0x0038u)&0x14u)==0x14u,
                 "LF71C saves base PW, accumulates pending fuel, and sets event bits");
    STEP98_CHECK(mpu16be_get(0x3FD0u)==1234u,
                 "bookkeeping does not duplicate or overwrite physical MPU fuel output");

    ecm_reset(); sim_ecm_ref.injector_services_pending=1u;
    ram16be_set(0x000Eu,0x2400u); ram16be_set(0x0010u,2u);
    ram16be_set(STARTUP_SPARK_DELAY_ADDR,2u);
    bua_common_injector_service_6p25ms();
    STEP98_CHECK(ram16be_get(0x0010u)==2u &&
                 ram16be_get(STARTUP_SPARK_DELAY_ADDR)==2u && RAM8(0x018Du)==0u,
                 "non-running service performs LF71C but skips startup decay and L018D count");

    ecm_reset(); RAM8(0x002Fu)=0x08u;
    sim_ecm_ref.injector_services_pending=1u;
    bua_common_injector_service_6p25ms();
    STEP98_CHECK((DIAG_MODE_WORD2&0x01u)!=0u,
                 "LF682 flags an L002F cylinder-mode mismatch against LC225=0");

    ecm_reset(); ENGINE_MODE_WORD|=ENGINE_RUNNING_BIT;
    MODE_WORD3|=FUEL_VATS_OK_BIT; STARTUP_COOLANT=96u; COOLANT=160u;
    ram16be_set(0x000Eu,0x2400u); ram16be_set(0x0010u,100u);
    ram16be_set(STARTUP_SPARK_DELAY_ADDR,100u);
    BLM=128u; CLOSED_LOOP_PW_CORR=128u; TPS_LOAD_AXIS=20u;
    VSS_MPH_HI=30u; sim_maf_adc=100u; sim_set_ecm_reference_rpm(600u);
    for(i=0u;i<160u;++i) irq_6p25ms();
    STEP98_CHECK(stats.ref_pulses==40u && stats.physical_injector_services==10u,
                 "one simulated second at 600 RPM yields 40 DRPs and ten batch services");
    STEP98_CHECK(stats.common_injector_bookkeeping==10u &&
                 stats.running_injector_bookkeeping==10u && RAM8(0x018Du)==10u,
                 "all ten physical services clock one common and one running bookkeeping pass");
    STEP98_CHECK(ram16be_get(0x0010u)==90u &&
                 ram16be_get(STARTUP_SPARK_DELAY_ADDR)==90u,
                 "ten services decrement both startup delays by ten, independent of IRQ count");
    printf("  step-98 injector-bookkeeping regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP98_CHECK
}

