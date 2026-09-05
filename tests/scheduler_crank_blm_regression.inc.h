static void run_step101_scheduler_crank_fuel_test(void)
{
    BuaMemory saved_mem;
    BuaStats saved_stats;
    BuaFuelOutputResult expected_output;
    bua_u16 expected_pw;
    bua_u16 expected_filter;
    bua_u16 quotient;
    bua_u16 fast_before;
    bua_u32 crank0;
    bua_u32 normal0;
    bua_u32 front0;
    unsigned int passed;
    unsigned int total;
    unsigned int i;
    bua_u8 saved_o2;
#define S101(C,T) do { ++total; if(C) ++passed; printf("  %-82s %s\n",T,(C)?"PASS":"FAIL"); } while(0)
    saved_mem=mem;
    saved_stats=stats;
    saved_o2=sim_o2_adc;
    passed=0u;
    total=0u;
    memset(&mem,0,sizeof(mem));
    memset(&stats,0,sizeof(stats));
    printf("\nStep-101 LD7A0 scheduler crank/normal fuel-selection regression:\n");

    MINOR_COUNT=1u;
    COOLANT=80u;
    TPS_LOAD_AXIS=0u;
    COLD_START_REF_COUNT=4u;
    MODE_WORD3=FUEL_VATS_OK_BIT;
    RAM8(0x00CBu)=20u;
    ram16be_set(REF_PERIOD_ADDR,2808u);
    ram16be_set(AIRFLOW_WORD_ADDR,8000u);
    ram16be_set(0x006Fu,(bua_u16)(100u<<8));
    expected_pw=bua_crank_pw_step43(COOLANT,TPS_LOAD_AXIS,COLD_START_REF_COUNT);
    expected_output=bua_fuel_output_lf92a(expected_pw,MODE_WORD3,IGN_MODE_WORD,
                                           AF_MODE_WORD,ENGINE_MODE_WORD,
                                           INJECT_MODE_WORD,VSS_MPH_HI,
                                           ram16be_get(REF_PERIOD_ADDR),RAM8(0x00CBu));
    quotient=bua_fractional_divide(2457u,(bua_u16)(2808u<<1));
    expected_filter=bua_lag_filter_8_8(0u,(bua_u8)(quotient>>8),128u);
    fast_before=ram16be_get(0x006Fu);
    bua_odd_fuel_chain_12p5ms();
    S101(stats.odd_crank_fuel_calls==1u && stats.odd_normal_fuel_calls==0u,
         "not-running LD7A0 selects the cranking branch, not LD86B");
    S101(ram16be_get(0x00C2u)==expected_pw && expected_pw!=0u,
         "scheduler cranking branch writes calibrated LD7B1 pulse to L00C2");
    S101(mpu16be_get(0x3FD0u)==expected_output.hardware_pw &&
         ram16be_get(0x011Cu)==expected_output.pending_pw,
         "cranking pulse reaches the shared LF92A pending/MPU output path");
    S101(ram16be_get(0x006Fu)==fast_before && stats.odd_transient_front_calls==0u,
         "cranking branch skips LD86B transient and LD9A9 fast-O2 work");
    S101(ram16be_get(CRANK_RPM_FILTER_ADDR)==expected_filter && CRANK_FF_COUNTER==0u,
         "nonzero crank pulse preserves L0127 filtering and all-zero C35B side effect");
    S101(stats.odd_50ms_tail_calls==1u,
         "cranking LF92A rejoins the common LDDC2 50-ms tail");

    ENGINE_MODE_WORD|=ENGINE_RUNNING_BIT;
    ram16be_set(0x006Fu,(bua_u16)(100u<<8));
    sim_o2_adc=180u;
    BLM=128u;
    CLOSED_LOOP_PW_CORR=128u;
    RAM8(0x00CDu)=25u;
    crank0=stats.odd_crank_fuel_calls;
    normal0=stats.odd_normal_fuel_calls;
    front0=stats.odd_transient_front_calls;
    bua_odd_fuel_chain_12p5ms();
    S101(stats.odd_crank_fuel_calls==crank0 &&
         stats.odd_normal_fuel_calls==normal0+1u,
         "ENGINE RUNNING with LC35A=0 hands the next odd pass directly to LD86B");
    S101(stats.odd_transient_front_calls==front0+1u &&
         ram16be_get(0x006Fu)!=(bua_u16)(100u<<8),
         "first running pass executes transient and fast-O2 normal-path work");

    memset(&mem,0,sizeof(mem));
    memset(&stats,0,sizeof(stats));
    MINOR_COUNT=1u;
    COOLANT=160u;
    TPS_LOAD_AXIS=224u;
    MODE_WORD3=FUEL_VATS_OK_BIT;
    ram16be_set(REF_PERIOD_ADDR,2808u);
    ram16be_set(CRANK_RPM_FILTER_ADDR,0x3456u);
    bua_odd_fuel_chain_12p5ms();
    S101(ram16be_get(0x00C2u)==0u && mpu16be_get(0x3FD0u)==0u,
         "C390 clear-flood TPS produces zero scheduler and MPU cranking fuel");
    S101(ram16be_get(CRANK_RPM_FILTER_ADDR)==0x3456u,
         "zero crank PW takes LD863 and skips the crank-RPM filter side effect");

    memset(&mem,0,sizeof(mem));
    memset(&stats,0,sizeof(stats));
    MINOR_COUNT=0x0Eu;
    MODE_WORD3=FUEL_VATS_OK_BIT;
    COOLANT=80u;
    RAM8(0x00CBu)=20u;
    ram16be_set(REF_PERIOD_ADDR,4000u);
    for(i=0u;i<16u;++i)
        irq_6p25ms();
    S101(stats.odd_crank_fuel_calls==8u && stats.odd_normal_fuel_calls==0u &&
         stats.odd_mpu_fuel_writes==8u,
         "16 no-catch IRQs produce eight crank selections and eight LF92A writes");
    S101(stats.odd_50ms_tail_calls==2u && stats.air_fuel_loops==8u &&
         stats.spark_vss_loops==8u,
         "crank selection retains 50-ms tail and exact odd/even scheduler cadence");
    printf("  step-101 scheduler cranking-fuel regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
    mem=saved_mem;
    stats=saved_stats;
    sim_o2_adc=saved_o2;
#undef S101
}

static void run_step102_scheduler_async_ae_test(void)
{
    BuaMemory saved_mem;
    BuaStats saved_stats;
    bua_u32 calls_before;
    bua_u16 l011a_before;
    unsigned int passed;
    unsigned int total;
    unsigned int i;
#define S102(C,T) do { ++total; if(C) ++passed; printf("  %-82s %s\n",T,(C)?"PASS":"FAIL"); } while(0)
    saved_mem=mem;
    saved_stats=stats;
    passed=0u;
    total=0u;
    memset(&mem,0,sizeof(mem));
    memset(&stats,0,sizeof(stats));
    printf("\nStep-102 LE436 scheduler/MPU asynchronous-AE regression:\n");

    ENGINE_MODE_WORD=FUEL_ENGINE_RUNNING_BIT;
    RAM8(0x0038u)=TPS_AE_ACTIVE_BIT;
    RAM8(0x00D7u)=12u;
    RAM8(0x00D8u)=128u;
    COOLANT=100u;
    RAM8(0x00CBu)=30u;
    ram16be_set(0x00C2u,300u);
    ram16be_set(0x011Au,65530u);
    mpu16be_set((bua_u16)MPU_CPU_CONTROL_ADDR,0x1634u);
    bua_scheduler_async_le436_step102();
    S102(RAM8(0x00DFu)==1u && ram16be_get(0x00DBu)==152u &&
         ram16be_get(0x00D9u)==152u,
         "first eligible call advances count and commits the calibrated 152-count pulse");
    S102(mpu16be_get(0x3FF2u)==212u && stats.scheduler_async_hw_pulses==1u,
         "LE4D0 adds injector offset twice and writes the raw L3FF2 counter");
    S102(mpu16be_get((bua_u16)MPU_CPU_CONTROL_ADDR)==0x1234u &&
         stats.async_csr_triggers==1u,
         "LF4C3/LF4CE set then clear high-byte CSR bit 2 at the HAL boundary");
    S102(ram16be_get(0x011Au)==146u && stats.async_ae_pulses==1u,
         "LE4EE adds L00DB into L011A with literal 16-bit wrap");

    RAM8(0x0038u)=0u;
    ram16be_set(0x00D9u,99u);
    ram16be_set(0x00DBu,77u);
    l011a_before=ram16be_get(0x011Au);
    bua_scheduler_async_le436_step102();
    S102(ram16be_get(0x00DBu)==0u && ram16be_get(0x00D9u)==0u,
         "inactive LE44E path converges on LE4AD and clears new/accumulated AE");
    S102(stats.scheduler_async_hw_pulses==1u && mpu16be_get(0x3FF2u)==212u &&
         ram16be_get(0x011Au)==l011a_before,
         "zero L00DB neither retriggers hardware nor changes accumulated output");

    AIR_MODE_WORD=0x10u;
    RAM8(0x008Au)=1u;
    ram16be_set(0x00D9u,50u);
    ram16be_set(0x011Au,1000u);
    bua_scheduler_async_le436_step102();
    S102(RAM8(0x008Au)==0u && ram16be_get(0x00DBu)==256u &&
         ram16be_get(0x00D9u)==306u,
         "LE4B1 consumes the one stall-save pulse and accumulates LC605=256");
    S102(mpu16be_get(0x3FF2u)==316u && ram16be_get(0x011Au)==1256u &&
         stats.scheduler_async_hw_pulses==2u,
         "stall saver uses the same physical async counter/CSR dispatch path");

    calls_before=stats.scheduler_async_calls;
    ENGINE_MODE_WORD=0u;
    MODE_WORD3=FUEL_VATS_OK_BIT;
    COOLANT=80u;
    ram16be_set(REF_PERIOD_ADDR,2808u);
    bua_odd_crank_fuel_12p5ms();
    S102(stats.scheduler_async_calls==calls_before,
         "literal cranking LF92A branch rejoins LDDC2 without calling LE436");

    ecm_reset();
    memset(&stats,0,sizeof(stats));
    sim_set_ecm_reference_rpm(1600u);
    ENGINE_MODE_WORD|=ENGINE_RUNNING_BIT;
    MODE_WORD3|=FUEL_VATS_OK_BIT;
    sim_tps_adc=35u;
    sim_maf_adc=90u;
    COOLANT=160u;
    BLM=128u;
    CLOSED_LOOP_PW_CORR=128u;
    for(i=0u;i<16u;++i)
        irq_6p25ms();
    S102(stats.scheduler_async_calls==8u && stats.odd_normal_fuel_calls==8u &&
         stats.odd_crank_fuel_calls==0u,
         "16 running IRQs call LE436 on exactly eight normal odd fuel passes");
    S102(stats.air_fuel_loops==8u && stats.spark_vss_loops==8u &&
         stats.iac_minor_services==16u,
         "Step-102 wiring preserves 8 odd / 8 even / 16 IAC scheduler cadence");
    { int allone=1; for(i=0u;i<16u;++i)
          if(stats.major_segment_calls[i]!=1u) allone=0;
      S102(allone,"all sixteen major segments still execute once per 16 IRQs"); }

    printf("  step-102 scheduler asynchronous-AE regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
    mem=saved_mem;
    stats=saved_stats;
#undef S102
}

static void run_step103_blm_cell_selection_test(void)
{
    BuaMemory saved_mem;
    BuaStats saved_stats;
    BuaBlmSelect103 r;
    unsigned int passed;
    unsigned int total;
    unsigned int i;
    int all_default;
#define S103(C,T) do { ++total; if(C) ++passed; printf("  %-82s %s\n",T,(C)?"PASS":"FAIL"); } while(0)
    saved_mem=mem;
    saved_stats=stats;
    passed=0u;
    total=0u;
    memset(&mem,0,sizeof(mem));
    memset(&stats,0,sizeof(stats));
    memset(&mem.low[0x001Cu],128,16u);
    printf("\nStep-103 LDA73..LDB35 BLM cell-selection/wiring regression:\n");

    r=bua_blm_select_step103(15u,27u,11u,0u,0u,&mem.low[0x001Cu]);
    S103(r.cell==0u && r.changed!=0u && (r.af_mode_word&0x0Cu)==0x0Cu,
         "below first RPM/flow boundaries selects cell 0 and sets b2/b3");
    r=bua_blm_select_step103(15u,28u,12u,0u,0u,&mem.low[0x001Cu]);
    S103(r.cell==5u,"exact 28 RPM-code / 12-flow boundaries select matrix cell 5");
    r=bua_blm_select_step103(0u,48u,22u,0u,0u,&mem.low[0x001Cu]);
    S103(r.cell==10u,"exact middle 48/22 boundaries select matrix cell 10");
    r=bua_blm_select_step103(0u,80u,34u,0u,0u,&mem.low[0x001Cu]);
    S103(r.cell==15u,"exact upper 80/34 boundaries select matrix cell 15");

    r=bua_blm_select_step103(5u,24u,10u,0x0Cu,0u,&mem.low[0x001Cu]);
    S103(r.cell==5u && r.changed==0u && (r.af_mode_word&0x0Cu)==0x08u,
         "lower hysteresis equalities retain cell 5, clear b2, and preserve b3");
    r=bua_blm_select_step103(5u,23u,10u,0u,0u,&mem.low[0x001Cu]);
    S103(r.cell==0u && r.changed!=0u,
         "one code below RPM hysteresis reselects both raw bands, yielding cell 0");
    r=bua_blm_select_step103(5u,52u,24u,0u,0u,&mem.low[0x001Cu]);
    S103(r.cell==5u && r.changed==0u,
         "upper RPM/flow hysteresis equalities still retain current cell 5");
    r=bua_blm_select_step103(5u,53u,25u,0u,0u,&mem.low[0x001Cu]);
    S103(r.cell==10u && r.changed!=0u,
         "one code above both upper hysteresis limits reselects cell 10");

    mem.low[0x001Cu+10u]=107u;
    r=bua_blm_select_step103(0u,48u,22u,0u,0u,&mem.low[0x001Cu]);
    all_default=1;
    for(i=0u;i<16u;++i)
        if(mem.low[0x001Cu+i]!=STEP103_BLM_DEFAULT) all_default=0;
    S103(r.reinitialized!=0u && r.cell==0u && r.current_blm==128u &&
         (r.air_mode_word&0x40u)!=0u && all_default,
         "out-of-range selected value invokes LF434, all cells=$80, and cell 0");

    memset(&mem.low[0x001Cu],120,16u);
    r=bua_blm_select_step103(0u,20u,5u,0u,0u,&mem.low[0x001Cu]);
    S103(r.current_blm==120u,"ordinary fuel uses a valid sub-neutral cell value unchanged");
    r=bua_blm_select_step103(0u,20u,5u,AF_POWER_ENRICH_BIT,0u,
                              &mem.low[0x001Cu]);
    S103(r.current_blm==128u && mem.low[0x001Cu]==120u,
         "PE forces only current L00C0 to 128; stored cell remains 120");

    ecm_reset();
    ENGINE_MODE_WORD=ENGINE_RUNNING_BIT;
    RAM8(0x0001u)=(bua_u8)(NV_O2_READY_BIT|NV_CL_TIMER_DONE_BIT);
    COOLANT=160u;
    RAM8(0x005Fu)=160u;
    FILTERED_LOAD=20u;
    ram16be_set(TOTAL_AFR_ADDR,CAL_STOICH_CODE);
    MINOR_COUNT=0x15u;
    seg5_ac_closed_loop_fan();
    S103((AF_MODE_WORD2&(AF_CLOSED_LOOP_BIT|AF_BLM_STORE_ENABLE_BIT))==
                       (AF_CLOSED_LOOP_BIT|AF_BLM_STORE_ENABLE_BIT),
         "Segment-5 closed-loop continuation scheduler-wires LED7B store permission");
    S103(stats.scheduler_blm_store_qualifications==1u,
         "one selected Segment-5 half performs one BLM-store qualification");

    memset(&mem.low[0x001Cu],128,16u);
    RAM8(0x00BFu)=0u;
    RPM_DIV25=80u;
    ram16be_set(AIRFLOW_WORD_ADDR,(bua_u16)(34u<<8));
    AF_MODE_WORD=0u;
    AIR_MODE_WORD=0u;
    bua_scheduler_blm_select_step103();
    S103(RAM8(0x00BFu)==15u && BLM==128u &&
         (AF_MODE_WORD&0x0Cu)==0x0Cu,
         "scheduler selector writes L00BF/L00C0 and both cell-change delay bits");
    MINOR_COUNT=1u;
    AF_MODE_WORD2=(bua_u8)(AF_BLM_STORE_ENABLE_BIT|AF_CLOSED_LOOP_BIT);
    CLOSED_LOOP_INTEGRATOR=134u;
    RAM8(0x00CAu)=27u;
    bua_odd_50ms_tail_lddc2();
    S103(mem.low[0x001Cu+15u]==128u && RAM8(0x00CAu)==0u &&
         (AF_MODE_WORD&AF_BLM_UPDATE_DELAY_BIT)==0u,
         "first 50-ms tail honors new-cell b3 delay and clears its timer/state");
    bua_scheduler_blm_select_step103();
    RAM8(0x00CAu)=27u;
    bua_odd_50ms_tail_lddc2();
    S103(mem.low[0x001Cu+15u]==130u && BLM==130u &&
         stats.scheduler_blm_updates==1u,
         "next qualified tail learns the selected cell and mirrors it to L00C0");

    ecm_reset();
    memset(&stats,0,sizeof(stats));
    sim_set_ecm_reference_rpm(1600u);
    ENGINE_MODE_WORD|=ENGINE_RUNNING_BIT;
    MODE_WORD3|=FUEL_VATS_OK_BIT;
    sim_maf_adc=100u;
    BLM=128u;
    CLOSED_LOOP_PW_CORR=128u;
    for(i=0u;i<16u;++i) irq_6p25ms();
    S103(stats.scheduler_blm_select_calls==8u &&
         stats.odd_normal_fuel_calls==8u,
         "16 running IRQs perform eight source-ordered normal BLM selections");
    S103(stats.air_fuel_loops==8u && stats.spark_vss_loops==8u &&
         stats.iac_minor_services==16u,
         "BLM wiring preserves 8 odd / 8 even / 16 IAC cadence");
    { int allone=1; for(i=0u;i<16u;++i)
          if(stats.major_segment_calls[i]!=1u) allone=0;
      S103(allone,"all sixteen major segments remain once-per-16-IRQ aligned"); }

    printf("  step-103 BLM cell-selection/wiring regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
    mem=saved_mem;
    stats=saved_stats;
#undef S103
}
