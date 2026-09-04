/* ---------------------------------------------------------------------- */
/* Step 90: scheduler-facing wiring for Segments 1/5/6/A/E.               */
/*                                                                        */
/* These wrappers deliberately expose only source behavior already proven */
/* by earlier regressions.  Hardware-only output polarity and the damaged  */
/* downstream Segment-1 discrete staging remain outside this step.         */
/* ---------------------------------------------------------------------- */
/* Step 100: LEA28..LEA94.  Channel 7 is the diagnostic-pin voltage at the
 * HAL boundary; the default PC stimulus (200) selects ordinary operation. */
static void seg3_misc_100ms(void)
{
    bua_u8 raw;
    bua_u8 mode;
    bua_u16 filtered;

    mode=(bua_u8)(RAM8(0x0035u)&0xCFu);
    raw=hw_adc(0x70u);
    if(raw<40u)
        mode|=0x10u;
    else if(raw>=100u && raw<152u && (RAM8(0x0046u)&0x08u)==0u)
        mode|=0x20u;
    RAM8(0x0035u)=mode;
    if(RAM8(0x01B2u)!=0u)
        RAM8(0x0033u)|=0x04u;
    mpu16be_set(0x3FCEu,0u);
    if((ENGINE_MODE_WORD&ENGINE_RUNNING_BIT)==0u) {
        RAM8(0x0043u)&=0xD5u;
        RAM8(0x005Fu)=RAM8(0x005Bu);
        if((RAM8(0x0001u)&0x40u)==0u && RAM8(0x005Bu)>=113u)
            RAM8(0x0001u)|=0x40u;
        ram16be_set(0x006Fu,(bua_u16)(102u<<8));
        RAM8(0x0073u)=102u;
    } else {
        RAM8(0x0002u)&=0xEFu;
        filtered=bua_lag_filter_8_8(ram16be_get(0x0073u),RAM8(0x006Fu),5u);
        ram16be_set(0x0073u,filtered);
    }
    mem.io4000[3]=0x8Fu;
}

/* Step 100: LEA95..LEB39.  The coolant-derived normal producers are always
 * updated.  The key-off MAF burn-off branch is retained as raw ECM/MPU state;
 * no electrical behavior is assigned to the MPU counter value. */
static void seg7_coolant_variables(void)
{
    bua_u8 coolant;
    bua_u8 limited;
    bua_u8 raw;
    bua_u8 counter;

    coolant=RAM8(0x005Bu);
    limited=(coolant>208u)?208u:coolant;
    RAM8(0x005Eu)=limited;
    RAM8(0x00D1u)=bua_lookup_spacing16(cal_o2_lean_offset_c5bb,14u,limited);
    RAM8(0x00D7u)=bua_lookup_spacing32(cal_async_pulse_count,9u,coolant);
    RAM8(0x00D8u)=bua_lookup_spacing32(cal_async_cool_factor,9u,coolant);
    RAM8(0x00D0u)=bua_lookup_spacing32(cal_pe_coolant_change,9u,coolant);

    if((RAM8(0x003Bu)&0x02u)!=0u && (RAM8(0x0033u)&0x32u)==0u) {
        RAM8(0x0472u)=(bua_u8)(RAM8(0x0472u)+1u);
        if(RAM8(0x0472u)==0u)
            RAM8(0x0472u)=0xFFu;
    }
    if((RAM8(0x0033u)&0x10u)==0u || (RAM8(0x003Eu)&0x40u)==0u) {
        mpu16be_set(0x3FDAu,0xD000u);
        if((RAM8(0x0003u)&0x40u)!=0u)
            RAM8(0x004Eu)|=0x80u;
        return;
    }
    if(RAM8(0x00B8u)!=0u) {
        --RAM8(0x00B8u);
        mpu16be_set(0x3FDAu,0xD000u);
        if((RAM8(0x0003u)&0x40u)!=0u)
            RAM8(0x004Eu)|=0x80u;
        return;
    }
    if(RAM8(0x00B7u)==0u) {
        mpu16be_set(0x3FDAu,0xD000u);
        if((RAM8(0x0003u)&0x40u)!=0u)
            RAM8(0x004Eu)|=0x80u;
        return;
    }
    --RAM8(0x00B7u);
    mpu16be_set(0x3FDAu,0xDFFFu);
    if((RAM8(0x003Fu)&0x40u)!=0u)
        return;
    counter=RAM8(0x011Fu);
    if(counter<3u) {
        RAM8(0x011Fu)=(bua_u8)(counter+1u);
        return;
    }
    raw=hw_adc(0xA0u);
    if(raw<=95u && raw>=20u) {
        RAM8(0x0003u)&=0xBFu;
        return;
    }
    counter=RAM8(0x0120u);
    if(counter<6u)
        RAM8(0x0120u)=(bua_u8)(counter+1u);
    else
        RAM8(0x0003u)|=0x40u;
}

static void seg6_coolant_adc(void)
{
    BuaCool85 r;
    /* LF3B6 literally calls the Segment-4/H.U. hook first. */
    seg4_log_ram_to_hu();
    r=bua_cool85_step(hw_adc(0x40u),ram16be_get(0x001Au),
                      ram16be_get(0x005Bu),RAM8(0x0030u),RAM8(0x003Bu),
                      RAM8(0x0035u),RAM8(0x004Cu),
                      (bua_u8)((RAM8(0x003Cu)&0x80u)!=0u));
    RAM8(0x005Du)=r.converted_5d;
    ram16be_set(0x005Bu,r.coolant_5b);
    RAM8(0x005Fu)=r.startup_cool_5f;
    RAM8(0x0030u)=r.mode30;
    RAM8(0x003Bu)=r.mode3b;
    RAM8(0x0035u)=r.minor35;
    RAM8(0x004Cu)=r.err4c;
    RAM8(0x003Cu)=(bua_u8)(RAM8(0x003Cu)&0x7Fu);
}

static void segA_mat_variables(void)
{
    BuaMat84 r;
    bua_knock_recovery_segment_a();
    r=bua_mat84_step(hw_adc(0x80u),ram16be_get(0x001Au),RAM8(0x0065u),
                     RAM8(0x00ACu),RAM8(0x00ADu),RAM8(0x0043u),
                     RAM8(0x0040u),RAM8(0x004Cu),RAM8(0x004Du));
    RAM8(0x012Bu)=r.ad_inverted;
    RAM8(0x0060u)=r.mat;
    RAM8(0x00ACu)=r.low_timer;
    RAM8(0x00ADu)=r.high_timer;
    RAM8(0x0043u)=r.af_mode43;
    RAM8(0x0040u)=r.diag_mode40;
    RAM8(0x004Cu)=r.err23_latched4c;
    RAM8(0x004Du)=r.err25_latched4d;
}

static void segE_tcc_adc(void)
{
    BuaBatt86 r;
    bua_u8 sh;
    r=bua_battery86_segment_e(hw_adc(0x10u),RAM8(0x0033u),RAM8(0x003Eu),
                              mpu16be_get(0x3FC8u),ram16be_get(0x00B2u),
                              RAM8(0x00B4u),RAM8(0x00B7u),RAM8(0x00B8u),
                              RAM8(0x011Fu),RAM8(0x0120u));
    RAM8(0x007Eu)=r.battery_7e;
    RAM8(0x0033u)=r.mode33;
    RAM8(0x003Eu)=r.major3e;
    ram16be_set(0x00B2u,r.old_spark_b2);
    RAM8(0x00B4u)=r.spark_fb_b4;
    RAM8(0x00B7u)=r.burnoff_time_b7;
    RAM8(0x00B8u)=r.burnoff_delay_b8;
    RAM8(0x011Fu)=r.burn_diag_11f;
    RAM8(0x0120u)=r.burn_diag_120;
    if(r.shifter_read!=0u) {
        sh=RAM8(0x0045u);
        RAM8(0x0004u)=bua_prndl78(RAM8(0x0037u),&sh,RAM8(0x0004u),
                                  RAM8(0x0065u),0u);
        RAM8(0x0045u)=sh;
    }
}

static void seg1_output_bits(void)
{
    BuaBatt86 r;
    bua_u16 air_count;
    bua_u16 enrich_count;
    bua_u16 tcc_count;
    bua_u16 purge_count;
    bua_u16 egr_count;
    bua_u16 all_count;
    bua_u8 a;
    bua_u8 mode4;
    bua_u8 mode4_control;
    bua_u8 mode4_value;
    bua_u8 fan_bit;

    r=bua_battery86_seg1_gate(RAM8(0x007Eu),RAM8(0x0035u),
                               RAM8(0x003Eu),RAM8(0x004Fu));
    RAM8(0x0035u)=r.minor35;
    RAM8(0x003Eu)=r.major3e;
    RAM8(0x004Fu)=r.err4f;
    if(sim_legacy_segment1_output_freeze!=0u) {
        step91_outputs=bua_output_stage91(RAM8(0x003Eu),RAM8(0x0037u),
                                          RAM8(0x0034u),RAM8(0x0113u),
                                          RAM8(0x0112u),RAM8(0x00F4u),
                                          r.force_discretes_off);
        return;
    }

    /* $EDBD..$EDF9: engine-off and high-voltage all-output path. */
    if(r.force_discretes_off!=0u || (RAM8(0x0034u)&0x80u)==0u) {
        if(r.force_discretes_off!=0u)
            all_count=0xD000u;
        else if((RAM8(0x0041u)&0x80u)!=0u)
            all_count=0xDFFFu;
        else
            all_count=0xD000u;
        mpu16be_set(0x3FD2u,all_count);
        mpu16be_set(0x3FD6u,all_count);
        mpu16be_set(0x3FD8u,all_count);
        mpu16be_set(0x3FCCu,all_count);
        mpu16be_set(0x3FD4u,all_count);
        if(all_count==0xD000u)
            mem.io4000[4]=(bua_u8)(mem.io4000[4]&0xFDu);
        else
            mem.io4000[4]=(bua_u8)(mem.io4000[4]|0x02u);
        step91_outputs.air_arc_count=all_count;
        step91_outputs.enrich_count=all_count;
        step91_outputs.tcc_count=all_count;
        step91_outputs.purge_count=all_count;
        step91_outputs.egr_count=all_count;
        step91_outputs.fan_parallel_b1=(bua_u8)((mem.io4000[4]&2u)!=0u);
        step91_outputs.forced_off=r.force_discretes_off;
        return;
    }

    /* $EDFD..$EE4F: AIR and acceleration-enrichment raw counters. */
    mode4=(bua_u8)(RAM8(0x0035u)&0x08u);
    mode4_control=RAM8(0x0156u);
    mode4_value=RAM8(0x0157u);
    if(mode4!=0u && (mode4_control&0x20u)!=0u)
        air_count=(bua_u16)(((bua_u16)(0x3400u|mode4_value)<<2)|3u);
    else if((RAM8(0x003Eu)&0x02u)!=0u)
        air_count=0xDFFFu;
    else
        air_count=0xD000u;
    mpu16be_set(0x3FCCu,air_count);

    if(mode4!=0u && (mode4_control&0x40u)!=0u)
        enrich_count=(bua_u16)(((bua_u16)(0x3400u|mode4_value)<<2)|3u);
    else if((RAM8(0x003Eu)&0x04u)!=0u) /* LC015=$04 */
        enrich_count=0xDFFFu;
    else
        enrich_count=0xD000u;
    mpu16be_set(0x3FD4u,enrich_count);

    /* $EE52..$EE9B: LC017=$00 selects the TCC request, not A/C output. */
    if(mode4!=0u && (mode4_control&0x04u)!=0u)
        tcc_count=(bua_u16)(((bua_u16)(0x3400u|mode4_value)<<2)|3u);
    else if((RAM8(0x0037u)&0x20u)!=0u)
        tcc_count=0xDFFFu;
    else
        tcc_count=0xD000u;
    mpu16be_set(0x3FD6u,tcc_count);

    /* $EE9E..$EEB8: canister-purge counter; normal value is complemented. */
    if(mode4!=0u && (mode4_control&0x08u)!=0u)
        purge_count=(bua_u16)(((bua_u16)(0x3400u|mode4_value)<<2)|3u);
    else
        purge_count=bua_pwm_count91(RAM8(0x0113u));
    mpu16be_set(0x3FD8u,purge_count);

    /* $EEBB..$EEE3: fan bit is staged at raw parallel-I/O bit 1. */
    a=(bua_u8)(mem.io4000[4]&0xFDu);
    if(mode4!=0u && (RAM8(0x0152u)&0x02u)!=0u)
        a=(bua_u8)(a|(RAM8(0x0153u)&0x02u));
    else if(RAM8(0x00F4u)!=0u && (RAM8(0x0034u)&0x08u)!=0u)
        a=(bua_u8)(a|0x02u);
    mem.io4000[4]=a;
    fan_bit=(bua_u8)((a&0x02u)!=0u);

    /* $EEE6..$EF03: EGR normal duty is complemented; Mode 4 is literal. */
    if(mode4!=0u && (mode4_control&0x01u)!=0u)
        egr_count=(bua_u16)(((bua_u16)(0x3400u|mode4_value)<<2)|3u);
    else
        egr_count=bua_pwm_count91(RAM8(0x0112u));
    mpu16be_set(0x3FD2u,egr_count);

    step91_outputs.air_arc_count=air_count;
    step91_outputs.enrich_count=enrich_count;
    step91_outputs.tcc_count=tcc_count;
    step91_outputs.purge_count=purge_count;
    step91_outputs.egr_count=egr_count;
    step91_outputs.fan_parallel_b1=fan_bit;
    step91_outputs.forced_off=0u;
}

static void seg5_ac_closed_loop_fan(void)
{
    /* L0000 b4 selects the source's alternating fan/A-C versus closed-loop half. */
    if((MINOR_COUNT&0x10u)==0u) {
        bua_u8 timer=RAM8(0x00F5u);
        RAM8(0x00F4u)=bua_fan79((bua_u8)((RAM8(0x0035u)&0x02u)!=0u),
                                RAM8(0x0065u),RAM8(0x0037u),RAM8(0x005Bu),
                                RAM8(0x00F4u),&timer);
        RAM8(0x00F5u)=timer;
        bua_ac_front_tail_ram91();
    } else {
        BuaClEntryResult cl;
        cl=bua_closed_loop_entry_step34((bua_u8)((RAM8(0x0034u)&0x80u)!=0u),
                 RAM8(0x0001u),RAM8(0x0041u),RAM8(0x0035u),RAM8(0x0033u),
                 RAM8(0x004Bu),ram16be_get(0x001Au),RAM8(0x005Fu),
                 RAM8(0x005Bu),RAM8(0x00BEu),RAM8(0x0044u),
                 RAM8(0x0154u),RAM8(0x0155u),RAM8(0x0158u));
        RAM8(0x0044u)=cl.af_mode_word2;
        RAM8(0x0001u)=cl.nv_mode_word;
        RAM8(0x0033u)=cl.minor_mode_word1;
        RAM8(0x00BEu)=cl.o2_not_ready_timer;
        /* LED7B..LED9D follows closed-loop state production and controls
         * the L0044 b1 permission consumed by the 50-ms BLM learner. */
        AF_MODE_WORD2=bua_blm_store_qualify_step35(AF_MODE_WORD2,COOLANT,
                                      ram16be_get(TOTAL_AFR_ADDR),FILTERED_LOAD);
        ++stats.scheduler_blm_store_qualifications;
        /* LDF9D..LE07B is called after the closed-loop qualification tail as well. */
        bua_ac_front_tail_ram91();
    }
}
