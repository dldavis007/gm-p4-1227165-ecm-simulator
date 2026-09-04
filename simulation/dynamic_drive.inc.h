/* ---------------------------------------------------------------------- */
/* Step 104 deterministic normal-operation behavioral signature.          */
/*                                                                        */
/* Hash bytes explicitly so the frozen value is independent of host       */
/* structure padding, byte order, and whether unsigned long is 32 or 64   */
/* bits.  Every multiply is reduced modulo 2^32.                           */
/* ---------------------------------------------------------------------- */
#define STEP104_FNV_OFFSET 2166136261ul
#define STEP104_FNV_PRIME    16777619ul
#define STEP104_EXPECTED_SIGNATURE 0x4BA6B7C6ul
static bua_u32 step104_sig_byte(bua_u32 sig,bua_u8 value)
{
    sig^=(bua_u32)value;
    sig*=STEP104_FNV_PRIME;
    return sig&0xFFFFFFFFul;
}
static bua_u32 step104_sig_u16(bua_u32 sig,bua_u16 value)
{
    sig=step104_sig_byte(sig,(bua_u8)(value>>8));
    return step104_sig_byte(sig,(bua_u8)value);
}
static bua_u32 step104_sig_u32(bua_u32 sig,bua_u32 value)
{
    sig=step104_sig_byte(sig,(bua_u8)((value>>24)&0xFFul));
    sig=step104_sig_byte(sig,(bua_u8)((value>>16)&0xFFul));
    sig=step104_sig_byte(sig,(bua_u8)((value>>8)&0xFFul));
    return step104_sig_byte(sig,(bua_u8)(value&0xFFul));
}
static bua_u32 step104_sig_tick(bua_u32 sig,unsigned int segment_index,
                                 unsigned int segment_tick)
{
    unsigned int i;
    sig=step104_sig_u16(sig,(bua_u16)segment_index);
    sig=step104_sig_u16(sig,(bua_u16)segment_tick);
    /* Source RAM used by the normal core, raw MPU $3FC0-$3FFF, and the
     * thin raw I/O windows. Untranslated storage above $01FF is excluded. */
    for(i=0u;i<0x0200u;++i)
        sig=step104_sig_byte(sig,mem.low[i]);
    for(i=0u;i<0x0040u;++i)
        sig=step104_sig_byte(sig,mem.mpu[i]);
    for(i=0u;i<0x0010u;++i)
        sig=step104_sig_byte(sig,mem.io4000[i]);
    sig=step104_sig_byte(sig,mem.io5000);
    return sig;
}
static bua_u32 step104_sig_final_stats(bua_u32 sig)
{
    unsigned int i;
    sig=step104_sig_u32(sig,stats.irq_ticks);
    sig=step104_sig_u32(sig,stats.air_fuel_loops);
    sig=step104_sig_u32(sig,stats.spark_vss_loops);
    sig=step104_sig_u32(sig,stats.iac_minor_services);
    sig=step104_sig_u32(sig,stats.odd_crank_fuel_calls);
    sig=step104_sig_u32(sig,stats.odd_normal_fuel_calls);
    sig=step104_sig_u32(sig,stats.scheduler_async_calls);
    sig=step104_sig_u32(sig,stats.scheduler_async_hw_pulses);
    sig=step104_sig_u32(sig,stats.scheduler_blm_select_calls);
    sig=step104_sig_u32(sig,stats.scheduler_blm_cell_changes);
    sig=step104_sig_u32(sig,stats.scheduler_blm_updates);
    sig=step104_sig_u32(sig,stats.physical_injector_services);
    sig=step104_sig_u32(sig,stats.common_injector_bookkeeping);
    sig=step104_sig_u32(sig,stats.ref_pulses);
    sig=step104_sig_u32(sig,stats.rpm_calculations);
    for(i=0u;i<16u;++i)
        sig=step104_sig_u32(sig,stats.major_segment_calls[i]);
    return sig;
}
static void run_dynamic_drive_cycle(void)
{
    unsigned int segment_index,tick,total_tick,print_div;
    unsigned int speed_mph,rpm,tps_adc,maf_adc;
    unsigned int passed,total_checks;
    unsigned int idle_load_reference,idle_fuel_reference,accel_load_max,accel_fuel_max;
    unsigned int max_tps_axis,cruise_fuel_samples;
    unsigned int cruise_vss,cruise_rpm,final_vss,final_rpm,final_fuel;
    unsigned int i,all_major,blm_cell_count;
    unsigned int freeze_pass,freeze_total;
    bua_u32 blm_cells_seen;
    bua_u32 behavior_signature;
    bua_u8 engine_caught,dfco_seen,dfco_zero_seen,fuel_resumed,blm_store_seen;
    const DriveProfileSegment *segment;
#define STEP99_CHECK(c,t) do { if(c) ++passed; printf("  %-84s %s\n",t,(c)?"PASS":"FAIL"); } while(0)

    /* Replay the frozen Step-104 signature at its historical Segment-D
       boundary; Step-109 has a separate full-diagnostics signature. */
    sim_legacy_segment_d_freeze=1u;
    sim_legacy_segment1_output_freeze=1u;
    sim_legacy_ignition_shutdown_freeze=1u;

    printf("\nStep-99 integrated Corvette ECM driving simulation:\n");
    printf("  PC commands: TPS/MAF A-D stimuli plus phase-continuous reference and VSS sources.\n");
    printf("  ECM results: normalized TPS, RPM, VSS, load, fuel, DFCO, injector and IAC state.\n");
    printf("  This is a behavioral integration profile, not a vehicle-dynamics claim.\n\n");
    printf(" time  cmdMPH/RPM TPSin/axis MAF  ECMmph RPM  load fuel DFCO IAC phase\n");

    MODE_WORD3|=FUEL_VATS_OK_BIT;
    sim_o2_adc=128u;
    BLM=128u;
    CLOSED_LOOP_PW_CORR=128u;
    BATTERY_AD=128u;
    /* PC warm-engine stimulus: seed the already-selected 348-ohm coolant
     * range and its startup sample. The normal Segment-6 producer continues
     * to own subsequent coolant updates. */
    RAM8(0x0030u)|=0x01u;
    RAM8(0x003Bu)|=0x10u;
    ram16be_set(0x005Bu,(bua_u16)(160u<<8));
    RAM8(0x005Fu)=160u;
    /* The profile represents a warm restart, so retain the source NVRAM
     * closed-loop run-delay-complete state instead of simulating 50+ seconds. */
    RAM8(0x0001u)|=0x02u;
    total_tick=0u; print_div=0u; passed=0u; total_checks=26u;
    idle_load_reference=0u; idle_fuel_reference=0u;
    accel_load_max=0u; accel_fuel_max=0u;
    max_tps_axis=0u; cruise_fuel_samples=0u;
    cruise_vss=0u; cruise_rpm=0u; final_vss=0u; final_rpm=0u; final_fuel=0u;
    engine_caught=0u; dfco_seen=0u; dfco_zero_seen=0u; fuel_resumed=0u;
    blm_cells_seen=0ul; blm_store_seen=0u;
    behavior_signature=STEP104_FNV_OFFSET;

    for(segment_index=0u;segment_index<DRIVE_PROFILE_COUNT;++segment_index) {
        segment=&drive_profile[segment_index];
        for(tick=0u;tick<segment->duration_ticks;++tick) {
            speed_mph=interpolate_u16(segment->start_speed_mph,segment->end_speed_mph,
                                      tick,segment->duration_ticks);
            rpm=interpolate_u16(segment->start_rpm,segment->end_rpm,
                                tick,segment->duration_ticks);
            tps_adc=interpolate_u16(segment->start_tps_adc,segment->end_tps_adc,
                                    tick,segment->duration_ticks);
            maf_adc=interpolate_u16(segment->start_maf_adc,segment->end_maf_adc,
                                    tick,segment->duration_ticks);
            /* Alternating PC O2 voltage provides actual ready crossings and
             * rich/lean activity; it is a stimulus, not a plant model. */
            if(segment_index>=1u)
                sim_o2_adc=((total_tick/40u)&1u)!=0u ? 180u : 30u;
            apply_dynamic_command(speed_mph,rpm,tps_adc,maf_adc,segment->in_drive);
            irq_6p25ms();
            behavior_signature=step104_sig_tick(behavior_signature,
                                                 segment_index,tick);
            dash_apply_no_pulse_timeout();
            ++total_tick; ++print_div;
            if(RAM8(0x00BFu)<16u)
                blm_cells_seen|=((bua_u32)1ul<<RAM8(0x00BFu));
            if((AF_MODE_WORD2&0x02u)!=0u) /* LED9D BLM-store permission */
                blm_store_seen=1u;
            if((ENGINE_MODE_WORD&ENGINE_RUNNING_BIT)!=0u) engine_caught=1u;
            if((unsigned int)TPS_LOAD_AXIS>max_tps_axis) max_tps_axis=TPS_LOAD_AXIS;
            if(segment_index==1u && tick+1u==segment->duration_ticks) {
                idle_load_reference=FILTERED_LOAD;
                idle_fuel_reference=mpu16be_get(0x3FD0u);
            }
            if(segment_index==2u) {
                if((unsigned int)FILTERED_LOAD>accel_load_max) accel_load_max=FILTERED_LOAD;
                if((unsigned int)mpu16be_get(0x3FD0u)>accel_fuel_max)
                    accel_fuel_max=mpu16be_get(0x3FD0u);
            }
            if(segment_index==3u) {
                if(mpu16be_get(0x3FD0u)!=0u) ++cruise_fuel_samples;
                cruise_vss=VSS_MPH_HI;
                cruise_rpm=(unsigned int)RPM_DIV25*25u;
            }
            if(segment_index==4u || segment_index==5u) {
                if((AF_MODE_WORD&FUEL_DFCO_ZERO_BIT)!=0u) {
                    dfco_seen=1u;
                    if(mpu16be_get(0x3FD0u)==0u) dfco_zero_seen=1u;
                } else if(dfco_seen!=0u && mpu16be_get(0x3FD0u)!=0u) {
                    fuel_resumed=1u;
                }
            }
            if(segment_index==6u) {
                final_vss=VSS_MPH_HI;
                final_rpm=(unsigned int)RPM_DIV25*25u;
                final_fuel=mpu16be_get(0x3FD0u);
                if(dfco_seen!=0u && final_fuel!=0u) fuel_resumed=1u;
            }
            if(tick==0u || print_div>=80u || tick+1u==segment->duration_ticks) {
                printf("%5.2f %3u/%4u  %3u/%3u   %3u  %3u  %4u %4u %4u   %u  %3u %s\n",
                       (double)total_tick*0.00625,speed_mph,rpm,tps_adc,
                       (unsigned int)TPS_LOAD_AXIS,maf_adc,(unsigned int)VSS_MPH_HI,
                       (unsigned int)RPM_DIV25*25u,(unsigned int)FILTERED_LOAD,
                       (unsigned int)mpu16be_get(0x3FD0u),
                       (unsigned int)((AF_MODE_WORD&FUEL_DFCO_ZERO_BIT)!=0u),
                       (unsigned int)RAM8(0x002Cu),segment->name);
                print_div=0u;
            }
        }
    }

    behavior_signature=step104_sig_final_stats(behavior_signature);

    printf("\n  BLM/closed-loop trace: cell=%u BLM=%u cells-seen=%04lX changes=%lu "
           "AF43=%02X AF44=%02X AFR=%u load=%u NV01=%02X coolant=%u startup=%u runsec=%u O2wait=%u\n",
           (unsigned int)RAM8(0x00BFu),(unsigned int)BLM,
           (unsigned long)blm_cells_seen,
           (unsigned long)stats.scheduler_blm_cell_changes,(unsigned int)AF_MODE_WORD,
           (unsigned int)AF_MODE_WORD2,(unsigned int)ram16be_get(TOTAL_AFR_ADDR),
           (unsigned int)FILTERED_LOAD,(unsigned int)RAM8(0x0001u),
           (unsigned int)COOLANT,(unsigned int)RAM8(0x005Fu),
           (unsigned int)ram16be_get(0x001Au),(unsigned int)RAM8(0x00BEu));
    printf("  Step-104 normal-operation behavioral signature: %08lX\n",
           (unsigned long)behavior_signature);
    printf("\nStep-99 integrated-drive regression:\n");
    STEP99_CHECK(engine_caught!=0u,"reference qualification catches the engine during the cranking phase");
    STEP99_CHECK(stats.odd_crank_fuel_calls>0u,
                 "pre-catch odd passes execute calibrated LD7B1 scheduler cranking fuel");
    STEP99_CHECK(stats.odd_normal_fuel_calls>stats.odd_crank_fuel_calls,
                 "post-catch drive hands off to and remains on the normal LD86B fuel path");
    STEP99_CHECK(stats.common_tps_calls==(bua_u32)total_tick,
                 "every 6.25-ms IRQ executes the common TPS producer");
    STEP99_CHECK(max_tps_axis>100u,"acceleration voltage is normalized to a substantial L0082 opening");
    STEP99_CHECK(accel_load_max>idle_load_reference,"acceleration produces more filtered load than settled warm idle");
    STEP99_CHECK(accel_fuel_max>idle_fuel_reference,"acceleration produces a larger MPU fuel command than settled warm idle");
    STEP99_CHECK(stats.scheduler_async_hw_pulses>0u,
                 "acceleration tip-in reaches LE436 and issues raw asynchronous MPU triggers");
    STEP99_CHECK(stats.scheduler_blm_select_calls==stats.odd_normal_fuel_calls,
                 "every normal odd fuel pass performs source-ordered BLM cell selection");
    blm_cell_count=0u;
    for(i=0u;i<16u;++i)
        if((blm_cells_seen&((bua_u32)1ul<<i))!=0ul) ++blm_cell_count;
    STEP99_CHECK(blm_cell_count>=3u && stats.scheduler_blm_cell_changes>0u,
                 "RPM/airflow trajectory moves through multiple hysteretic BLM cells");
    STEP99_CHECK(blm_store_seen!=0u && stats.scheduler_blm_store_qualifications>0u,
                 "Segment-5 closed-loop continuation enables live BLM-store permission");
    STEP99_CHECK(stats.scheduler_blm_updates>0u && BLM!=120u, /* LF434 default */
                 "continued closed-loop idle performs a real selected-cell BLM update");
    STEP99_CHECK(behavior_signature==STEP104_EXPECTED_SIGNATURE,
                 "complete normal-operation trace matches the frozen Step-104 signature");
    STEP99_CHECK(cruise_vss>=55u && cruise_vss<=65u,"ECM VSS settles near the commanded 60-MPH cruise");
    STEP99_CHECK(cruise_rpm>=2100u && cruise_rpm<=2300u,"ECM reference calculation settles near 2200 RPM at cruise");
    STEP99_CHECK(cruise_fuel_samples>500u,"cruise maintains nonzero synchronous fuel rather than a scripted value");
    STEP99_CHECK(dfco_seen!=0u,"closed-throttle lift qualifies and enters translated DFCO");
    STEP99_CHECK(dfco_zero_seen!=0u,"LF92A writes zero MPU fuel while DFCO is active");
    STEP99_CHECK(fuel_resumed!=0u,"fuel resumes after DFCO as speed/RPM return toward idle");
    STEP99_CHECK(stats.physical_injector_services==stats.common_injector_bookkeeping,
                 "every physical batch opportunity receives exactly one LF67B service");
    STEP99_CHECK(final_rpm>=700u && final_rpm<=900u,"returned-idle ECM RPM settles near 800 RPM");
    STEP99_CHECK(final_vss<=5u,"returned-idle ECM VSS decays to essentially zero");
    STEP99_CHECK(final_fuel!=0u,"returned idle has nonzero fuel after DFCO recovery");
    STEP99_CHECK(stats.odd_50ms_tail_calls==(bua_u32)(total_tick/8u),
                 "50-ms Air/Fuel tail retains exact integrated-drive cadence");
    STEP99_CHECK(stats.iac_minor_services==(bua_u32)total_tick,
                 "IAC motor service remains active on every drive-cycle IRQ");
    all_major=1u;
    for(i=0u;i<16u;++i)
        if(stats.major_segment_calls[i]!=(bua_u32)(total_tick/16u)) all_major=0u;
    STEP99_CHECK(all_major!=0u,"all sixteen major segments execute equally across the complete drive");
    printf("  step-99 integrated driving regression result: %s (%u/%u)\n",
           (passed==total_checks)?"PASS":"FAIL",passed,total_checks);

    freeze_pass=0u;
    freeze_total=0u;
#define STEP104_CHECK(C,T) do { ++freeze_total; if(C) ++freeze_pass; \
    printf("  %-84s %s\n",T,(C)?"PASS":"FAIL"); } while(0)
    printf("\nStep-104 normal-operation baseline-freeze regression:\n");
    STEP104_CHECK(behavior_signature==STEP104_EXPECTED_SIGNATURE,
                  "canonical RAM/MPU/I/O/statistics trace equals 4BA6B7C6");
    STEP104_CHECK(total_tick==4960u && stats.irq_ticks==4960ul,
                  "fixed warm-restart profile contains exactly 4,960 ordinary IRQ ticks");
    STEP104_CHECK(stats.air_fuel_loops==2480ul && stats.spark_vss_loops==2480ul,
                  "frozen profile retains exact 2,480 odd and 2,480 even selections");
    STEP104_CHECK(stats.iac_minor_services==4960ul,
                  "IAC physical service remains once per frozen-profile IRQ");
    STEP104_CHECK(all_major!=0u && stats.major_segment_calls[0]==310ul,
                  "every major segment executes exactly 310 times");
    STEP104_CHECK(stats.odd_crank_fuel_calls+stats.odd_normal_fuel_calls==
                  stats.air_fuel_loops,
                  "every odd selection resolves to exactly one crank or normal fuel path");
    STEP104_CHECK(stats.scheduler_blm_select_calls==stats.odd_normal_fuel_calls &&
                  stats.scheduler_blm_updates>0ul,
                  "every normal fuel pass selects BLM and the profile performs learning");
    STEP104_CHECK(stats.scheduler_async_hw_pulses>0ul &&
                  stats.async_csr_triggers==stats.scheduler_async_hw_pulses,
                  "every frozen async MPU pulse has exactly one CSR trigger");
    STEP104_CHECK(stats.physical_injector_services==
                  stats.common_injector_bookkeeping,
                  "physical batch opportunities and LF67B bookkeeping remain one-to-one");
    STEP104_CHECK(RAM8(0x00BFu)==1u && BLM==122u &&
                  ram16be_get(TOTAL_AFR_ADDR)==445u &&
                  (AF_MODE_WORD2&0x82u)==0x82u,
                  "frozen final state is cell1, BLM122, stoich, closed-loop/store enabled");
    printf("  step-104 baseline-freeze regression result: %s (%u/%u)\n",
           (freeze_pass==freeze_total)?"PASS":"FAIL",freeze_pass,freeze_total);
#undef STEP104_CHECK
#undef STEP99_CHECK
}
