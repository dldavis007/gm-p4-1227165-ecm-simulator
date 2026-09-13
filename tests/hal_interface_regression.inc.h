/* Steps 158-164 regress the normal FMD/VOLT and full lifecycle HAL boundary. */
static bua_u32 step160_hash_byte(bua_u32 hash,bua_u8 value)
{
    hash^=(bua_u32)value;
    return (hash*16777619ul)&0xFFFFFFFFul;
}

static bua_u32 step160_hash_word(bua_u32 hash,bua_u16 value)
{
    hash=step160_hash_byte(hash,(bua_u8)(value>>8));
    return step160_hash_byte(hash,(bua_u8)value);
}

static bua_u8 step161_all_pwm_outputs_equal(bua_u16 value)
{
    return (bua_u8)(bua_hal_get_mpu16be(0x3FCCu)==value &&
                    bua_hal_get_mpu16be(0x3FD2u)==value &&
                    bua_hal_get_mpu16be(0x3FD4u)==value &&
                    bua_hal_get_mpu16be(0x3FD6u)==value &&
                    bua_hal_get_mpu16be(0x3FD8u)==value);
}

static bua_u8 step162_active_outputs_match(void)
{
    return (bua_u8)(bua_hal_get_mpu16be(0x3FCCu)==0xDFFFu &&
                    bua_hal_get_mpu16be(0x3FD2u)==0xD003u &&
                    bua_hal_get_mpu16be(0x3FD4u)==0xDFFFu &&
                    bua_hal_get_mpu16be(0x3FD6u)==0xDFFFu &&
                    bua_hal_get_mpu16be(0x3FD8u)==0xD3FFu &&
                    (bua_hal_get_output_4004()&0x02u)!=0u);
}

static bua_u8 step163_power_from_raw_hal(bua_u8 battery,bua_u8 pump,
                                          bua_u8 diagnostic,bua_u8 fmd1,
                                          bua_u16 vector_address,
                                          BuaPowerOnInput120 in)
{
    ecm_reset();
    bua_power_valid_retained_step120();
    bua_hal_set_volt_adc(battery);
    bua_hal_set_pumpvolt_adc(pump);
    bua_hal_set_diag_adc(diagnostic);
    bua_hal_set_normal_fmd_byte1(fmd1);
    return bua_lifecycle_power_cycle_from_hal_step163(vector_address,in);
}

static void run_step164_raw_hal_factory_irq_test(void)
{
    unsigned int passed=0u;
    unsigned int total=10u;
    bua_u8 outcomes[6];
    BuaMemory saved_mem=mem;
    BuaStats saved_stats=stats;
    BuaLifecycleTrace121 saved_lifecycle=bua_lifecycle_trace121;
    BuaVectorTrace119 saved_vector=bua_vector_trace119;
    BuaPowerOnTrace120 saved_power=bua_power_on_trace120;
    BuaStartupTrace114 saved_startup=bua_startup_trace114;
    BuaStartupResult112 saved_startup_result=bua_startup_last112;
    BuaFactoryTrace117 saved_factory117=bua_factory_trace117;
    BuaFactoryTrace118 saved_factory118=bua_factory_trace118;
    bua_u8 saved_volt=sim_volt_adc;
    bua_u8 saved_pump=sim_pumpvolt_adc;
    bua_u8 saved_diag=sim_diag_adc;
    bua_u8 saved_fmd1=sim_normal_fmd_byte1;
    bua_u8 saved_fmd2=sim_normal_fmd_byte2;
    bua_u8 saved_fmd_enabled=sim_normal_fmd_enabled;
    bua_u8 saved_startup_fmd=sim_startup_fmd_status;
    bua_u8 saved_powerdown=sim_soft_powerdown_latched;
    bua_u8 saved_factory_battery=sim_factory_battery_adc117;
    bua_u8 saved_factory_diag=sim_factory_diagnostic_adc117;
    bua_u8 saved_factory_fmd1=sim_factory_fmd_byte1_117;
    bua_u8 saved_factory_fmd2=sim_factory_fmd_byte2_117;
    bua_u8 saved_factory_swi=sim_factory_swi_reason117;
    BuaPowerOnInput120 in;
    bua_u32 ordinary_before;
    bua_u32 signature;
    unsigned int i;
#define STEP164_CHECK(c,t) do { if(c) ++passed; printf("  %-84s %s\n",t,(c)?"PASS":"FAIL"); } while(0)

    printf("\nStep-164 raw-HAL factory IRQ composition regression:\n");
    in=bua_power_input_step120();
    in.whole_rom_checksum=0x1234u;
    outcomes[0]=step163_power_from_raw_hal(99u,160u,40u,1u,0xFFFEu,in);
    STEP164_CHECK(outcomes[0]==POWER120_OUTCOME_FACTORY,
                  "Step-163 raw inputs enter the established factory IRQ route");
    bua_hal_set_volt_adc(90u);
    bua_hal_set_diag_adc(40u);
    bua_hal_set_normal_fmd_byte1(3u);
    bua_hal_set_normal_fmd_byte2(0xA5u);
    sim_factory_battery_adc117=1u;
    sim_factory_diagnostic_adc117=200u;
    sim_factory_fmd_byte1_117=0u;
    sim_factory_fmd_byte2_117=0u;
    RAM8(0x0032u)=12u;
    RAM8(0x0048u)=0u;
    ordinary_before=stats.irq_ticks;
    bua_lifecycle_irq_from_hal_step164();
    STEP164_CHECK(RAM8(0x0049u)==3u && RAM8(0x004Au)==0xA5u &&
                  sim_factory_battery_adc117==90u &&
                  sim_factory_diagnostic_adc117==40u,
                  "factory IRQ refreshes both FMD bytes, VOLT, and DIAG from named raw HAL state");
    STEP164_CHECK(RAM8(0x0048u)==3u && RAM8(0x0032u)==0u &&
                  bua_factory_trace117.mode_changes==1ul,
                  "refreshed FMD mode and ignition-on VOLT drive the existing factory state change");
    STEP164_CHECK(stats.irq_ticks==ordinary_before &&
                  bua_factory_trace117.irq_routes==1ul,
                  "HAL-aware factory IRQ remains isolated from the ordinary scheduler");

    outcomes[1]=step163_power_from_raw_hal(99u,160u,40u,1u,0xFFFEu,in);
    bua_hal_set_diag_adc(39u);
    bua_lifecycle_irq_from_hal_step164();
    STEP164_CHECK(sim_factory_swi_reason117==FACTORY117_SWI_DIAGNOSTIC &&
                  bua_vector_trace119.last_swi_source==0xFDB3u &&
                  bua_factory_trace117.fmd_exchanges==0ul,
                  "raw DIAG 39 reaches the factory diagnostic SWI before FMD exchange");

    outcomes[2]=step163_power_from_raw_hal(99u,160u,40u,1u,0xFFFEu,in);
    bua_hal_set_diag_adc(100u);
    bua_lifecycle_irq_from_hal_step164();
    STEP164_CHECK(sim_factory_swi_reason117==FACTORY117_SWI_DIAGNOSTIC &&
                  bua_vector_trace119.last_swi_source==0xFDB3u,
                  "raw DIAG equality 100 reaches the same listing-backed SWI boundary");

    outcomes[3]=step163_power_from_raw_hal(99u,160u,40u,1u,0xFFFEu,in);
    bua_hal_set_volt_adc(40u);
    RAM8(0x0032u)=7u;
    bua_lifecycle_irq_from_hal_step164();
    STEP164_CHECK(RAM8(0x0032u)==8u &&
                  sim_factory_swi_reason117==FACTORY117_SWI_NONE,
                  "raw VOLT equality 40 increments the factory ignition-off timer");
    RAM8(0x0032u)=160u;
    bua_lifecycle_irq_from_hal_step164();
    STEP164_CHECK(sim_factory_swi_reason117==FACTORY117_SWI_POWERDOWN &&
                  bua_vector_trace119.last_swi_source==0xFD03u,
                  "raw low VOLT with timer 160 reaches the factory powerdown SWI boundary");

    outcomes[4]=step163_power_from_raw_hal(99u,160u,40u,1u,0xFFFEu,in);
    RAM8(0x0032u)=12u;
    bua_hal_set_volt_adc(89u);
    bua_lifecycle_irq_from_hal_step164();
    outcomes[5]=RAM8(0x0032u);
    bua_hal_set_volt_adc(90u);
    bua_lifecycle_irq_from_hal_step164();
    STEP164_CHECK(outcomes[5]==12u && RAM8(0x0032u)==0u,
                  "raw VOLT 89 preserves and equality 90 clears the factory ignition timer");

    outcomes[5]=step163_power_from_raw_hal(128u,0u,200u,0x7Eu,0xFFFEu,in);
    sim_factory_battery_adc117=0x11u;
    sim_factory_diagnostic_adc117=0x22u;
    sim_factory_fmd_byte1_117=0x33u;
    sim_factory_fmd_byte2_117=0x44u;
    ordinary_before=stats.irq_ticks;
    bua_lifecycle_irq_from_hal_step164();
    STEP164_CHECK(outcomes[5]==POWER120_OUTCOME_NORMAL &&
                  stats.irq_ticks==ordinary_before+1ul &&
                  sim_factory_battery_adc117==0x11u &&
                  sim_factory_diagnostic_adc117==0x22u &&
                  sim_factory_fmd_byte1_117==0x33u &&
                  sim_factory_fmd_byte2_117==0x44u,
                  "ordinary IRQ does not refresh or enter the factory-only HAL shadow state");

    signature=2166136261ul;
    for(i=0u;i<6u;++i)
        signature=step160_hash_byte(signature,outcomes[i]);
    signature=step160_hash_byte(signature,sim_factory_battery_adc117);
    signature=step160_hash_byte(signature,sim_factory_diagnostic_adc117);
    signature=step160_hash_byte(signature,sim_factory_fmd_byte1_117);
    signature=step160_hash_byte(signature,sim_factory_fmd_byte2_117);
    signature=step160_hash_word(signature,(bua_u16)stats.irq_ticks);
    printf("  Step-164 raw-HAL factory IRQ signature: %08lX\n",
           (unsigned long)signature);
    printf("  step-164 raw-HAL factory IRQ regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);

    mem=saved_mem;
    stats=saved_stats;
    bua_lifecycle_trace121=saved_lifecycle;
    bua_vector_trace119=saved_vector;
    bua_power_on_trace120=saved_power;
    bua_startup_trace114=saved_startup;
    bua_startup_last112=saved_startup_result;
    bua_factory_trace117=saved_factory117;
    bua_factory_trace118=saved_factory118;
    sim_volt_adc=saved_volt;
    sim_pumpvolt_adc=saved_pump;
    sim_diag_adc=saved_diag;
    sim_normal_fmd_byte1=saved_fmd1;
    sim_normal_fmd_byte2=saved_fmd2;
    sim_normal_fmd_enabled=saved_fmd_enabled;
    sim_startup_fmd_status=saved_startup_fmd;
    sim_soft_powerdown_latched=saved_powerdown;
    sim_factory_battery_adc117=saved_factory_battery;
    sim_factory_diagnostic_adc117=saved_factory_diag;
    sim_factory_fmd_byte1_117=saved_factory_fmd1;
    sim_factory_fmd_byte2_117=saved_factory_fmd2;
    sim_factory_swi_reason117=saved_factory_swi;
#undef STEP164_CHECK
}

static void run_step163_raw_hal_power_on_test(void)
{
    unsigned int passed=0u;
    unsigned int total=10u;
    bua_u8 outcomes[8];
    BuaMemory saved_mem=mem;
    BuaStats saved_stats=stats;
    BuaLifecycleTrace121 saved_lifecycle=bua_lifecycle_trace121;
    BuaVectorTrace119 saved_vector=bua_vector_trace119;
    BuaPowerOnTrace120 saved_power=bua_power_on_trace120;
    BuaStartupTrace114 saved_startup=bua_startup_trace114;
    BuaStartupResult112 saved_startup_result=bua_startup_last112;
    BuaFactoryTrace117 saved_factory117=bua_factory_trace117;
    BuaFactoryTrace118 saved_factory118=bua_factory_trace118;
    bua_u8 saved_volt=sim_volt_adc;
    bua_u8 saved_pump=sim_pumpvolt_adc;
    bua_u8 saved_diag=sim_diag_adc;
    bua_u8 saved_fmd1=sim_normal_fmd_byte1;
    bua_u8 saved_fmd2=sim_normal_fmd_byte2;
    bua_u8 saved_fmd_enabled=sim_normal_fmd_enabled;
    bua_u8 saved_powerdown=sim_soft_powerdown_latched;
    bua_u8 saved_startup_fmd=sim_startup_fmd_status;
    bua_u8 saved_factory_battery=sim_factory_battery_adc117;
    bua_u8 saved_factory_diag=sim_factory_diagnostic_adc117;
    bua_u8 saved_factory_fmd1=sim_factory_fmd_byte1_117;
    bua_u8 saved_factory_fmd2=sim_factory_fmd_byte2_117;
    bua_u8 saved_factory_swi=sim_factory_swi_reason117;
    BuaPowerOnInput120 in;
    bua_u32 signature;
    unsigned int i;
#define STEP163_CHECK(c,t) do { if(c) ++passed; printf("  %-84s %s\n",t,(c)?"PASS":"FAIL"); } while(0)

    printf("\nStep-163 raw-HAL power-on composition regression:\n");
    in=bua_power_input_step120();
    in.battery_adc=0u;
    in.pump_adc=255u;
    in.diagnostic_adc=50u;
    in.initial_fmd_byte1=0u;
    outcomes[0]=step163_power_from_raw_hal(128u,0u,200u,0x7Eu,0xFFFEu,in);
    STEP163_CHECK(outcomes[0]==POWER120_OUTCOME_NORMAL &&
                  bua_power_on_trace120.normal_handoffs==1ul,
                  "raw powered inputs select normal startup despite poisoned duplicate input fields");
    STEP163_CHECK(RAM8(0x007Eu)==128u && RAM8(0x0049u)==0x7Eu,
                  "normal startup receives raw VOLT and FMD byte 1 from named HAL backing");
    STEP163_CHECK(bua_startup_trace114.scheduler_handoff!=0u &&
                  bua_lifecycle_trace121.power_cycle_acknowledgements==1ul,
                  "raw-HAL composition retains the established reset-vector lifecycle handoff");

    in=bua_power_input_step120();
    in.whole_rom_checksum=0x1234u;
    outcomes[1]=step163_power_from_raw_hal(99u,160u,40u,0xA5u,0xFFF8u,in);
    STEP163_CHECK(outcomes[1]==POWER120_OUTCOME_FACTORY &&
                  bua_power_on_trace120.factory_handoffs==1ul,
                  "raw low VOLT plus threshold PUMPVOLT and DIAG select factory boot");
    STEP163_CHECK(ram16be_get(0x0173u)==0x1234u &&
                  sim_factory_battery_adc117==99u &&
                  sim_factory_diagnostic_adc117==40u &&
                  sim_factory_fmd_byte1_117==0xA5u,
                  "factory boot receives HAL bytes while the explicit ROM checksum remains separate");

    in=bua_power_input_step120();
    outcomes[2]=step163_power_from_raw_hal(100u,160u,40u,0u,0xFFFEu,in);
    STEP163_CHECK(outcomes[2]==POWER120_OUTCOME_NORMAL,
                  "raw VOLT equality 100 excludes factory selection");
    outcomes[3]=step163_power_from_raw_hal(99u,159u,40u,0u,0xFFFEu,in);
    STEP163_CHECK(outcomes[3]==POWER120_OUTCOME_NORMAL,
                  "raw PUMPVOLT 159 remains below the factory threshold");

    outcomes[4]=step163_power_from_raw_hal(99u,160u,39u,0u,0xFFFEu,in);
    outcomes[5]=step163_power_from_raw_hal(99u,160u,40u,0u,0xFFFEu,in);
    STEP163_CHECK(outcomes[4]==POWER120_OUTCOME_NORMAL &&
                  outcomes[5]==POWER120_OUTCOME_FACTORY,
                  "raw DIAG lower boundary changes factory selection exactly at 40");
    outcomes[6]=step163_power_from_raw_hal(99u,160u,99u,0u,0xFFFEu,in);
    outcomes[7]=step163_power_from_raw_hal(99u,160u,100u,0u,0xFFFEu,in);
    STEP163_CHECK(outcomes[6]==POWER120_OUTCOME_FACTORY &&
                  outcomes[7]==POWER120_OUTCOME_NORMAL,
                  "raw DIAG upper boundary includes 99 and excludes 100");
    STEP163_CHECK(step163_power_from_raw_hal(99u,160u,40u,0u,0xFFF2u,in)==
                  POWER120_OUTCOME_INVALID,
                  "raw-HAL sourcing cannot bypass the established reset-vector boundary");

    signature=2166136261ul;
    for(i=0u;i<8u;++i)
        signature=step160_hash_byte(signature,outcomes[i]);
    signature=step160_hash_byte(signature,sim_volt_adc);
    signature=step160_hash_byte(signature,sim_pumpvolt_adc);
    signature=step160_hash_byte(signature,sim_diag_adc);
    signature=step160_hash_byte(signature,sim_normal_fmd_byte1);
    signature=step160_hash_byte(signature,bua_lifecycle_trace121.last_power_on_outcome);
    printf("  Step-163 raw-HAL power-on signature: %08lX\n",
           (unsigned long)signature);
    printf("  step-163 raw-HAL power-on regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);

    mem=saved_mem;
    stats=saved_stats;
    bua_lifecycle_trace121=saved_lifecycle;
    bua_vector_trace119=saved_vector;
    bua_power_on_trace120=saved_power;
    bua_startup_trace114=saved_startup;
    bua_startup_last112=saved_startup_result;
    bua_factory_trace117=saved_factory117;
    bua_factory_trace118=saved_factory118;
    sim_volt_adc=saved_volt;
    sim_pumpvolt_adc=saved_pump;
    sim_diag_adc=saved_diag;
    sim_normal_fmd_byte1=saved_fmd1;
    sim_normal_fmd_byte2=saved_fmd2;
    sim_normal_fmd_enabled=saved_fmd_enabled;
    sim_soft_powerdown_latched=saved_powerdown;
    sim_startup_fmd_status=saved_startup_fmd;
    sim_factory_battery_adc117=saved_factory_battery;
    sim_factory_diagnostic_adc117=saved_factory_diag;
    sim_factory_fmd_byte1_117=saved_factory_fmd1;
    sim_factory_fmd_byte2_117=saved_factory_fmd2;
    sim_factory_swi_reason117=saved_factory_swi;
#undef STEP163_CHECK

    run_step164_raw_hal_factory_irq_test();
}

static void run_step162_raw_volt_output_safety_test(void)
{
    unsigned int passed=0u;
    unsigned int total=10u;
    unsigned int i;
    static const bua_u16 addresses[5]={
        0x3FCCu,0x3FD2u,0x3FD4u,0x3FD6u,0x3FD8u
    };
    bua_u16 stage_outputs[4][5];
    bua_u8 stage35[4];
    bua_u8 stage3e[4];
    bua_u8 stage4f[4];
    bua_u8 stage4004[4];
    BuaMemory saved_mem=mem;
    BuaStats saved_stats=stats;
    BuaLifecycleTrace121 saved_lifecycle=bua_lifecycle_trace121;
    BuaVectorTrace119 saved_vector=bua_vector_trace119;
    BuaPowerOnTrace120 saved_power=bua_power_on_trace120;
    BuaStartupTrace114 saved_startup=bua_startup_trace114;
    BuaOutputStage91 saved_outputs=step91_outputs;
    bua_u8 saved_volt=sim_volt_adc;
    bua_u8 saved_powerdown=sim_soft_powerdown_latched;
    BuaPowerOnInput120 in;
    bua_u8 outcome;
    bua_u32 signature;
#define STEP162_CHECK(c,t) do { if(c) ++passed; printf("  %-84s %s\n",t,(c)?"PASS":"FAIL"); } while(0)

    printf("\nStep-162 raw-VOLT output-safety regression:\n");
    ecm_reset();
    bua_lifecycle_reset_trace_step121();
    bua_power_valid_retained_step120();
    in=bua_power_input_step120();
    outcome=bua_lifecycle_power_cycle_step121(0xFFFEu,in);
    RAM8(0x0034u)=0x88u;
    RAM8(0x003Eu)=0x06u;
    RAM8(0x0037u)=0x20u;
    RAM8(0x0113u)=0u;
    RAM8(0x0112u)=255u;
    RAM8(0x00F4u)=255u;

    bua_hal_set_volt_adc(170u);
    MINOR_COUNT=0x0Du;
    bua_lifecycle_irq_step121();
    STEP162_CHECK(outcome==POWER120_OUTCOME_NORMAL && RAM8(0x007Eu)==170u &&
                  (RAM8(0x0033u)&STEP111_IGNITION_OFF_BIT)==0u,
                  "ordinary Segment E acquires raw VOLT 170 below the overvoltage threshold");
    MINOR_COUNT=0u;
    bua_lifecycle_irq_step121();
    STEP162_CHECK(step162_active_outputs_match()!=0u &&
                  step91_outputs.forced_off==0u,
                  "below-threshold Segment 1 emits the configured active raw outputs");
    for(i=0u;i<5u;++i)
        stage_outputs[0][i]=bua_hal_get_mpu16be(addresses[i]);
    stage35[0]=RAM8(0x0035u); stage3e[0]=RAM8(0x003Eu);
    stage4f[0]=RAM8(0x004Fu); stage4004[0]=bua_hal_get_output_4004();

    bua_hal_set_volt_adc(171u);
    MINOR_COUNT=0x0Du;
    bua_lifecycle_irq_step121();
    STEP162_CHECK(RAM8(0x007Eu)==171u,
                  "ordinary Segment E acquires the exact raw VOLT 171 threshold");
    MINOR_COUNT=0u;
    bua_lifecycle_irq_step121();
    STEP162_CHECK((RAM8(0x0035u)&0x40u)!=0u &&
                  (RAM8(0x003Eu)&0x10u)!=0u &&
                  (RAM8(0x004Fu)&0x40u)==0u &&
                  step91_outputs.forced_off==0u,
                  "first high-VOLT Segment 1 arms the source two-pass qualification");
    STEP162_CHECK(step162_active_outputs_match()!=0u,
                  "first high-VOLT pass does not prematurely force the raw outputs off");
    for(i=0u;i<5u;++i)
        stage_outputs[1][i]=bua_hal_get_mpu16be(addresses[i]);
    stage35[1]=RAM8(0x0035u); stage3e[1]=RAM8(0x003Eu);
    stage4f[1]=RAM8(0x004Fu); stage4004[1]=bua_hal_get_output_4004();

    MINOR_COUNT=0u;
    bua_lifecycle_irq_step121();
    STEP162_CHECK((RAM8(0x004Fu)&0x40u)!=0u &&
                  step91_outputs.forced_off!=0u,
                  "second consecutive high-VOLT Segment 1 latches the error and forced-off state");
    STEP162_CHECK(step161_all_pwm_outputs_equal(0xD000u)!=0u,
                  "qualified overvoltage writes $D000 to all five raw PWM outputs");
    STEP162_CHECK((bua_hal_get_output_4004()&0x02u)==0u,
                  "qualified overvoltage clears the raw fan-output bit");
    for(i=0u;i<5u;++i)
        stage_outputs[2][i]=bua_hal_get_mpu16be(addresses[i]);
    stage35[2]=RAM8(0x0035u); stage3e[2]=RAM8(0x003Eu);
    stage4f[2]=RAM8(0x004Fu); stage4004[2]=bua_hal_get_output_4004();

    bua_hal_set_volt_adc(170u);
    MINOR_COUNT=0x0Du;
    bua_lifecycle_irq_step121();
    MINOR_COUNT=0u;
    bua_lifecycle_irq_step121();
    STEP162_CHECK((RAM8(0x0035u)&0x40u)==0u &&
                  step91_outputs.forced_off==0u &&
                  step162_active_outputs_match()!=0u,
                  "raw VOLT recovery clears qualification and restores ordinary output staging");
    STEP162_CHECK((RAM8(0x003Eu)&0x10u)!=0u &&
                  (RAM8(0x004Fu)&0x40u)!=0u,
                  "source-latched burnoff-disable and error bits survive voltage recovery");
    for(i=0u;i<5u;++i)
        stage_outputs[3][i]=bua_hal_get_mpu16be(addresses[i]);
    stage35[3]=RAM8(0x0035u); stage3e[3]=RAM8(0x003Eu);
    stage4f[3]=RAM8(0x004Fu); stage4004[3]=bua_hal_get_output_4004();

    signature=2166136261ul;
    for(i=0u;i<4u;++i) {
        unsigned int j;
        for(j=0u;j<5u;++j)
            signature=step160_hash_word(signature,stage_outputs[i][j]);
        signature=step160_hash_byte(signature,stage35[i]);
        signature=step160_hash_byte(signature,stage3e[i]);
        signature=step160_hash_byte(signature,stage4f[i]);
        signature=step160_hash_byte(signature,stage4004[i]);
    }
    printf("  Step-162 raw-VOLT output-safety signature: %08lX\n",
           (unsigned long)signature);
    printf("  step-162 raw-VOLT output-safety regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);

    mem=saved_mem;
    stats=saved_stats;
    bua_lifecycle_trace121=saved_lifecycle;
    bua_vector_trace119=saved_vector;
    bua_power_on_trace120=saved_power;
    bua_startup_trace114=saved_startup;
    step91_outputs=saved_outputs;
    sim_volt_adc=saved_volt;
    sim_soft_powerdown_latched=saved_powerdown;
#undef STEP162_CHECK

    run_step163_raw_hal_power_on_test();
}

static void run_step161_raw_hal_output_lifecycle_test(void)
{
    unsigned int passed=0u;
    unsigned int total=9u;
    unsigned int guard=0u;
    unsigned int i;
    static const bua_u16 addresses[5]={
        0x3FCCu,0x3FD2u,0x3FD4u,0x3FD6u,0x3FD8u
    };
    bua_u16 shutdown_outputs[5];
    bua_u8 shutdown_io;
    bua_u16 shutdown_injector;
    bua_u8 shutdown_iac;
    BuaMemory saved_mem=mem;
    BuaStats saved_stats=stats;
    BuaLifecycleTrace121 saved_lifecycle=bua_lifecycle_trace121;
    BuaVectorTrace119 saved_vector=bua_vector_trace119;
    BuaPowerOnTrace120 saved_power=bua_power_on_trace120;
    BuaStartupTrace114 saved_startup=bua_startup_trace114;
    BuaOutputStage91 saved_outputs=step91_outputs;
    bua_u8 saved_fmd1=sim_normal_fmd_byte1;
    bua_u8 saved_fmd2=sim_normal_fmd_byte2;
    bua_u8 saved_fmd_enabled=sim_normal_fmd_enabled;
    bua_u8 saved_volt=sim_volt_adc;
    bua_u8 saved_powerdown=sim_soft_powerdown_latched;
    bua_u8 saved_iac_motor=sim_iac_motor_on;
    BuaPowerOnInput120 in;
    bua_u8 outcome;
    bua_u32 blocked_before;
    bua_u32 signature;
#define STEP161_CHECK(c,t) do { if(c) ++passed; printf("  %-84s %s\n",t,(c)?"PASS":"FAIL"); } while(0)

    printf("\nStep-161 raw-HAL output lifecycle regression:\n");
    ecm_reset();
    bua_lifecycle_reset_trace_step121();
    bua_power_valid_retained_step120();
    in=bua_power_input_step120();
    in.initial_fmd_byte1=0x7Eu;
    outcome=bua_lifecycle_power_cycle_step121(0xFFFEu,in);
    STEP161_CHECK(outcome==POWER120_OUTCOME_NORMAL &&
                  step161_all_pwm_outputs_equal(0u)!=0u &&
                  bua_hal_get_injector_pw_counts()==0u,
                  "source-ordered power-on clears the raw MPU output window");
    STEP161_CHECK(bua_hal_get_output_4004()==0x98u,
                  "normal power-on leaves the parallel-I/O control byte at the established $98");

    bua_hal_set_normal_fmd_byte1(0x7Eu);
    bua_hal_set_normal_fmd_byte2(0x18u);
    bua_hal_set_volt_adc(128u);
    MINOR_COUNT=0x0Du;
    bua_lifecycle_irq_step121();
    RAM8(0x0034u)|=STEP111_ENGINE_RUNNING_BIT;
    RAM8(0x000Au)=160u;
    RAM8(0x000Cu)=110u;
    RAM8(0x002Cu)=50u;
    RAM8(0x00F3u)=0x80u;
    bua_hal_set_volt_adc(20u);
    MINOR_COUNT=0x1Du;
    bua_lifecycle_irq_step121();
    while(sim_soft_powerdown_latched==0u && guard<2500u) {
        bua_lifecycle_irq_step121();
        ++guard;
    }
    STEP161_CHECK(sim_soft_powerdown_latched!=0u && guard<2500u,
                  "raw key-off reaches powerdown before output-boundary inspection");
    STEP161_CHECK(step161_all_pwm_outputs_equal(0xD000u)!=0u,
                  "last engine-off Segment 1 leaves all five raw PWM counters at $D000");
    STEP161_CHECK(bua_hal_get_output_4004()==0x98u,
                  "engine-off output staging clears fan bit 1 without disturbing control bits");
    STEP161_CHECK(bua_hal_get_injector_pw_counts()==0u &&
                  bua_hal_get_iac_position()==STEP111_IAC_PARK_POSITION,
                  "injector command is zero and IAC bookkeeping is parked at shutdown");

    for(i=0u;i<5u;++i)
        shutdown_outputs[i]=bua_hal_get_mpu16be(addresses[i]);
    shutdown_io=bua_hal_get_output_4004();
    shutdown_injector=bua_hal_get_injector_pw_counts();
    shutdown_iac=bua_hal_get_iac_position();
    blocked_before=bua_lifecycle_trace121.blocked_irq_requests;
    bua_lifecycle_irq_step121();
    STEP161_CHECK(bua_lifecycle_trace121.blocked_irq_requests==blocked_before+1ul &&
                  step161_all_pwm_outputs_equal(shutdown_outputs[0])!=0u &&
                  bua_hal_get_output_4004()==shutdown_io &&
                  bua_hal_get_injector_pw_counts()==shutdown_injector &&
                  bua_hal_get_iac_position()==shutdown_iac,
                  "latched powerdown blocks IRQ-driven changes at every observed raw output");

    bua_hal_set_volt_adc(128u);
    outcome=bua_lifecycle_power_cycle_step121(0xFFF8u,in);
    STEP161_CHECK(outcome==POWER120_OUTCOME_NORMAL &&
                  step161_all_pwm_outputs_equal(0u)!=0u &&
                  bua_hal_get_injector_pw_counts()==0u,
                  "acknowledged restart source-clears the raw MPU output window again");
    STEP161_CHECK(bua_hal_get_output_4004()==0x98u,
                  "retained restart re-establishes the same parallel-I/O control byte");

    signature=2166136261ul;
    signature=step160_hash_word(signature,(bua_u16)guard);
    for(i=0u;i<5u;++i)
        signature=step160_hash_word(signature,shutdown_outputs[i]);
    signature=step160_hash_byte(signature,shutdown_io);
    signature=step160_hash_word(signature,shutdown_injector);
    signature=step160_hash_byte(signature,shutdown_iac);
    for(i=0u;i<5u;++i)
        signature=step160_hash_word(signature,bua_hal_get_mpu16be(addresses[i]));
    signature=step160_hash_byte(signature,bua_hal_get_output_4004());
    signature=step160_hash_word(signature,bua_hal_get_injector_pw_counts());
    printf("  Step-161 raw-HAL output lifecycle signature: %08lX\n",
           (unsigned long)signature);
    printf("  step-161 raw-HAL output lifecycle regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);

    mem=saved_mem;
    stats=saved_stats;
    bua_lifecycle_trace121=saved_lifecycle;
    bua_vector_trace119=saved_vector;
    bua_power_on_trace120=saved_power;
    bua_startup_trace114=saved_startup;
    step91_outputs=saved_outputs;
    sim_normal_fmd_byte1=saved_fmd1;
    sim_normal_fmd_byte2=saved_fmd2;
    sim_normal_fmd_enabled=saved_fmd_enabled;
    sim_volt_adc=saved_volt;
    sim_soft_powerdown_latched=saved_powerdown;
    sim_iac_motor_on=saved_iac_motor;
#undef STEP161_CHECK

    run_step162_raw_volt_output_safety_test();
}

static void run_step160_raw_hal_full_lifecycle_test(void)
{
    unsigned int passed=0u;
    unsigned int total=13u;
    unsigned int guard=0u;
    BuaMemory saved_mem=mem;
    BuaStats saved_stats=stats;
    BuaLifecycleTrace121 saved_lifecycle=bua_lifecycle_trace121;
    BuaVectorTrace119 saved_vector=bua_vector_trace119;
    BuaPowerOnTrace120 saved_power=bua_power_on_trace120;
    BuaStartupTrace114 saved_startup=bua_startup_trace114;
    bua_u8 saved_fmd1=sim_normal_fmd_byte1;
    bua_u8 saved_fmd2=sim_normal_fmd_byte2;
    bua_u8 saved_fmd_enabled=sim_normal_fmd_enabled;
    bua_u8 saved_volt=sim_volt_adc;
    BuaPowerOnInput120 in;
    bua_u8 outcome;
    bua_u32 blocked_before;
    bua_u32 signature;
#define STEP160_CHECK(c,t) do { if(c) ++passed; printf("  %-84s %s\n",t,(c)?"PASS":"FAIL"); } while(0)

    printf("\nStep-160 full raw-HAL lifecycle regression:\n");
    ecm_reset();
    bua_lifecycle_reset_trace_step121();
    bua_power_valid_retained_step120();
    in=bua_power_input_step120();
    in.initial_fmd_byte1=0x7Eu;
    outcome=bua_lifecycle_power_cycle_step121(0xFFFEu,in);
    STEP160_CHECK(outcome==POWER120_OUTCOME_NORMAL &&
                  bua_startup_trace114.scheduler_handoff!=0u,
                  "reset vector and retained startup reach the ordinary scheduler handoff");

    bua_hal_set_normal_fmd_byte1(0x7Eu);
    bua_hal_set_normal_fmd_byte2(0x18u);
    bua_hal_set_volt_adc(128u);
    MINOR_COUNT=0x0Du;
    bua_lifecycle_irq_step121();
    STEP160_CHECK(RAM8(0x007Eu)==128u &&
                  (RAM8(0x0033u)&STEP111_IGNITION_OFF_BIT)==0u,
                  "raw powered VOLT is acquired by ordinary Segment E");
    STEP160_CHECK((RAM8(0x0037u)&0x81u)==0x81u &&
                  RAM8(0x002Eu)==0x7Eu && RAM8(0x002Fu)==0x18u,
                  "raw FMD produces P/N and compressor-not-on status before shutdown");

    RAM8(0x0034u)|=STEP111_ENGINE_RUNNING_BIT;
    RAM8(0x000Au)=160u;
    RAM8(0x000Cu)=110u;
    RAM8(0x002Cu)=50u;
    RAM8(0x00F3u)=0x80u;
    bua_hal_set_volt_adc(20u);
    MINOR_COUNT=0x1Du;
    bua_lifecycle_irq_step121();
    STEP160_CHECK(RAM8(0x007Eu)==20u &&
                  (RAM8(0x0033u)&STEP111_IGNITION_OFF_BIT)!=0u,
                  "raw low VOLT is acquired by Segment E and asserts ignition-off");

    while(sim_soft_powerdown_latched==0u && guard<2500u) {
        bua_lifecycle_irq_step121();
        ++guard;
    }
    STEP160_CHECK(sim_soft_powerdown_latched!=0u && guard<2500u,
                  "raw key-off runs naturally to the D6EA software-powerdown boundary");
    STEP160_CHECK(ram16be_get(0x008Bu)==STEP111_POWERDOWN_COUNT &&
                  stats.soft_powerdown_events==1ul,
                  "shutdown timer reaches exact LC012 terminal count with one powerdown event");
    STEP160_CHECK(bua_vector_trace119.last_swi_source==0xD6EAu,
                  "terminal shutdown records the listing-exact D6EA SWI source");
    STEP160_CHECK(RAM8(0x001Cu)==150u && RAM8(0x001Du)==118u &&
                  RAM8(0x002Bu)==118u,
                  "key-off commits bounded SAM values into retained BLM cells");
    STEP160_CHECK(RAM8(0x002Cu)==STEP111_IAC_PARK_POSITION &&
                  (RAM8(0x0002u)&STEP111_SKIP_IAC_RESET_BIT)!=0u,
                  "IAC completes close-and-reopen homing to calibrated park before powerdown");
    printf("  IAC terminal state: position=%u mode=%02X command=%02X reset=%02X guard=%u\n",
           (unsigned int)RAM8(0x002Cu),(unsigned int)RAM8(0x00F3u),
           (unsigned int)RAM8(0x0101u),(unsigned int)RAM8(0x0002u),guard);

    blocked_before=bua_lifecycle_trace121.blocked_irq_requests;
    bua_lifecycle_irq_step121();
    STEP160_CHECK(bua_lifecycle_trace121.blocked_irq_requests==blocked_before+1ul,
                  "powerdown latch blocks a subsequent host IRQ request");

    bua_hal_set_volt_adc(128u);
    outcome=bua_lifecycle_power_cycle_step121(0xFFF8u,in);
    STEP160_CHECK(outcome==POWER120_OUTCOME_NORMAL &&
                  sim_soft_powerdown_latched==0u,
                  "host reset acknowledgement releases powerdown and re-enters normal startup");
    STEP160_CHECK(bua_power_on_trace120.retained_checksum_valid!=0u &&
                  bua_power_on_trace120.retained_recoveries==0ul &&
                  RAM8(0x001Cu)==150u && RAM8(0x002Bu)==118u,
                  "restart accepts retained checksum and preserves shutdown-committed BLM state");

    MINOR_COUNT=0x2Du;
    bua_lifecycle_irq_step121();
    STEP160_CHECK(RAM8(0x007Eu)==128u &&
                  (RAM8(0x0033u)&STEP111_IGNITION_OFF_BIT)==0u &&
                  (RAM8(0x0037u)&0x81u)==0x81u,
                  "post-reset Segment E reacquires powered VOLT and independent raw FMD state");

    signature=2166136261ul;
    signature=step160_hash_word(signature,(bua_u16)guard);
    signature=step160_hash_word(signature,ram16be_get(0x008Bu));
    signature=step160_hash_byte(signature,RAM8(0x001Cu));
    signature=step160_hash_byte(signature,RAM8(0x002Bu));
    signature=step160_hash_byte(signature,RAM8(0x002Cu));
    signature=step160_hash_byte(signature,RAM8(0x0037u));
    signature=step160_hash_byte(signature,RAM8(0x007Eu));
    signature=step160_hash_word(signature,(bua_u16)stats.soft_powerdown_events);
    signature=step160_hash_word(signature,(bua_u16)bua_lifecycle_trace121.blocked_irq_requests);
    printf("  Step-160 full raw-HAL lifecycle signature: %08lX\n",
           (unsigned long)signature);
    printf("  step-160 full raw-HAL lifecycle regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);

    mem=saved_mem;
    stats=saved_stats;
    bua_lifecycle_trace121=saved_lifecycle;
    bua_vector_trace119=saved_vector;
    bua_power_on_trace120=saved_power;
    bua_startup_trace114=saved_startup;
    sim_normal_fmd_byte1=saved_fmd1;
    sim_normal_fmd_byte2=saved_fmd2;
    sim_normal_fmd_enabled=saved_fmd_enabled;
    sim_volt_adc=saved_volt;
#undef STEP160_CHECK

    run_step161_raw_hal_output_lifecycle_test();
}

static void run_step159_raw_hal_lifecycle_test(void)
{
    unsigned int passed=0u;
    unsigned int total=8u;
    BuaMemory saved_mem=mem;
    BuaStats saved_stats=stats;
    bua_u8 saved_fmd1=sim_normal_fmd_byte1;
    bua_u8 saved_fmd2=sim_normal_fmd_byte2;
    bua_u8 saved_fmd_enabled=sim_normal_fmd_enabled;
    bua_u8 saved_volt=sim_volt_adc;
#define STEP159_CHECK(c,t) do { if(c) ++passed; printf("  %-84s %s\n",t,(c)?"PASS":"FAIL"); } while(0)

    printf("\nStep-159 raw HAL lifecycle integration regression:\n");
    ecm_reset();

    /* Raw FMD1 $7E complements to P/N b0 plus compressor-not-on b7. */
    bua_hal_set_normal_fmd_byte1(0x7Eu);
    bua_hal_set_normal_fmd_byte2(0x18u);
    irq_6p25ms();
    STEP159_CHECK((RAM8(0x0037u)&0x81u)==0x81u,
                  "ordinary IRQ decodes raw FMD into P/N plus A/C-compressor-not-on status");
    STEP159_CHECK(RAM8(0x002Eu)==0x7Eu && RAM8(0x002Fu)==0x18u,
                  "ordinary IRQ preserves both raw FMD replies at listing RAM $002E/$002F");

    /* Raw FMD1 $FF complements those two decoded states clear. */
    bua_hal_set_normal_fmd_byte1(0xFFu);
    irq_6p25ms();
    STEP159_CHECK((RAM8(0x0037u)&0x81u)==0u,
                  "next IRQ changes decoded state to Drive plus compressor-on from raw FMD only");

    /* Put the scheduler immediately before Segment E so the normal major
     * dispatch, rather than a direct helper call, acquires raw VOLT. */
    RAM8(0x0033u)&=(bua_u8)~STEP111_IGNITION_OFF_BIT;
    bua_hal_set_volt_adc(20u);
    MINOR_COUNT=0x0Du;
    irq_6p25ms();
    STEP159_CHECK(RAM8(0x007Eu)==20u &&
                  (RAM8(0x0033u)&STEP111_IGNITION_OFF_BIT)!=0u,
                  "raw VOLT below 40 reaches Segment E and asserts decoded ignition-off state");

    irq_6p25ms();
    STEP159_CHECK(ram16be_get(0x008Bu)==1u &&
                  stats.ignition_shutdown_calls!=0ul,
                  "following odd IRQ enters the existing shutdown front without a RAM key-off poke");

    bua_hal_set_volt_adc(128u);
    MINOR_COUNT=0x1Du;
    irq_6p25ms();
    STEP159_CHECK(RAM8(0x007Eu)==128u &&
                  (RAM8(0x0033u)&STEP111_IGNITION_OFF_BIT)==0u,
                  "raw powered VOLT reaches Segment E and clears decoded ignition-off state");

    irq_6p25ms();
    STEP159_CHECK(ram16be_get(0x008Bu)==0u,
                  "following odd IRQ takes ignition-on branch and resets shutdown timer");
    STEP159_CHECK((RAM8(0x0037u)&0x81u)==0u && RAM8(0x002Eu)==0xFFu,
                  "FMD-derived Drive/A/C state remains independent of VOLT ignition transitions");

    printf("  step-159 raw-HAL lifecycle regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);

    mem=saved_mem;
    stats=saved_stats;
    sim_normal_fmd_byte1=saved_fmd1;
    sim_normal_fmd_byte2=saved_fmd2;
    sim_normal_fmd_enabled=saved_fmd_enabled;
    sim_volt_adc=saved_volt;
#undef STEP159_CHECK

    run_step160_raw_hal_full_lifecycle_test();
}

static void run_step158_normal_fmd_hal_test(void)
{
    unsigned int passed=0u;
    unsigned int total=7u;
    bua_u8 saved1=sim_normal_fmd_byte1;
    bua_u8 saved2=sim_normal_fmd_byte2;
    bua_u8 saved_enabled=sim_normal_fmd_enabled;
    bua_u8 saved2e=RAM8(0x002Eu);
    bua_u8 saved2f=RAM8(0x002Fu);
    bua_u8 saved37=RAM8(0x0037u);
#define STEP158_CHECK(c,t) do { if(c) ++passed; printf("  %-84s %s\n",t,(c)?"PASS":"FAIL"); } while(0)

    printf("\nStep-158 normal FMD raw input HAL regression:\n");
    sim_normal_fmd_enabled=0u;
    bua_hal_set_normal_fmd_byte1(0xFEu);
    bua_hal_set_normal_fmd_byte2(0xA5u);
    STEP158_CHECK(sim_normal_fmd_enabled!=0u && sim_normal_fmd_byte1==0xFEu &&
                  sim_normal_fmd_byte2==0xA5u,
                  "raw setters enable and retain the two normal FMD/SPI reply bytes");

    RAM8(0x0037u)=0x20u;
    bua_normal_fmd_refresh_step158();
    STEP158_CHECK(RAM8(0x002Eu)==0xFEu && RAM8(0x002Fu)==0xA5u,
                  "normal refresh stores LF1E0/LF1E5 replies at listing RAM $002E/$002F");
    STEP158_CHECK((RAM8(0x0037u)&0x01u)!=0u,
                  "raw FMD1 bit 0 clear produces listing-documented P/N status bit 0 set");
    STEP158_CHECK((RAM8(0x0037u)&0x20u)!=0u,
                  "FMD refresh preserves firmware-owned TCC status bit 5");

    bua_hal_set_normal_fmd_byte1(0x7Fu);
    RAM8(0x0037u)=0u;
    bua_normal_fmd_refresh_step158();
    STEP158_CHECK((RAM8(0x0037u)&0x80u)!=0u,
                  "raw FMD1 bit 7 clear produces documented A/C-compressor-not-on status");
    STEP158_CHECK((RAM8(0x0037u)&0x01u)==0u,
                  "raw FMD1 bit 0 set produces Drive status without a synthetic PRNDL input");
    STEP158_CHECK((RAM8(0x0037u)&0x08u)==0u,
                  "actual LC017=$00 suppresses power-steering status rather than enabling it");

    printf("  step-158 normal-FMD HAL regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);

    sim_normal_fmd_byte1=saved1;
    sim_normal_fmd_byte2=saved2;
    sim_normal_fmd_enabled=saved_enabled;
    RAM8(0x002Eu)=saved2e;
    RAM8(0x002Fu)=saved2f;
    RAM8(0x0037u)=saved37;
#undef STEP158_CHECK

    run_step159_raw_hal_lifecycle_test();
}

/* Step 128 regression for the explicit raw HAL boundary. */
static void run_step157_remaining_u10_hal_test(void)
{
    unsigned int passed=0u;
    unsigned int total=5u;
    bua_u8 saved_map2;
    bua_u8 saved_volt;
    bua_u8 saved_map;
    bua_u8 saved_pumpvolt;
    bua_u8 saved_esc;
#define STEP157_CHECK(c,t) do { if(c) ++passed; printf("  %-84s %s\n",t,(c)?"PASS":"FAIL"); } while(0)

    printf("\nStep-157 remaining known U10 raw input HAL regression:\n");
    saved_map2=sim_map2_adc;
    saved_volt=sim_volt_adc;
    saved_map=sim_map_adc;
    saved_pumpvolt=sim_pumpvolt_adc;
    saved_esc=sim_esc_adc;

    bua_hal_set_map2_adc(0x11u);
    STEP157_CHECK(hw_adc(0x00u)==0x11u,
                  "MAP2 setter reaches raw U10/AN0 selector $00 without inventing a normal consumer");

    bua_hal_set_volt_adc(0x22u);
    STEP157_CHECK(hw_adc(0x10u)==0x22u,
                  "VOLT setter reaches raw U10/AN1 selector $10 before firmware stores $007E");

    bua_hal_set_map_adc(0x33u);
    STEP157_CHECK(hw_adc(0x30u)==0x33u,
                  "MAP setter reaches raw U10/AN3 selector $30 without redefining the MAF load path");

    bua_hal_set_pumpvolt_adc(0x44u);
    STEP157_CHECK(hw_adc(0x60u)==0x44u,
                  "PUMPVOLT setter reaches distinct raw U10/AN6 selector $60");

    bua_hal_set_esc_adc(0x55u);
    STEP157_CHECK(hw_adc(0x90u)==0x55u,
                  "ESC setter reaches raw U10/AN9 selector $90 without replacing U9 knock events");

    printf("  step-157 remaining-U10 HAL regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);

    sim_map2_adc=saved_map2;
    sim_volt_adc=saved_volt;
    sim_map_adc=saved_map;
    sim_pumpvolt_adc=saved_pumpvolt;
    sim_esc_adc=saved_esc;
#undef STEP157_CHECK

    run_step158_normal_fmd_hal_test();
}

static void run_step156_cts_mat_hal_test(void)
{
    unsigned int passed=0u;
    unsigned int total=4u;
    bua_u8 saved_cts;
    bua_u8 saved_mat;
#define STEP156_CHECK(c,t) do { if(c) ++passed; printf("  %-84s %s\n",t,(c)?"PASS":"FAIL"); } while(0)

    printf("\nStep-156 CTS/MAT raw input HAL regression:\n");
    saved_cts=sim_cts_adc;
    saved_mat=sim_mat_adc;

    STEP156_CHECK(hw_adc(0x40u)==120u,
                  "CTS raw backing preserves the historical default selector value 120");
    STEP156_CHECK(hw_adc(0x80u)==0u,
                  "MAT raw backing preserves the historical default selector value 0");

    bua_hal_set_cts_adc(0x6Du);
    STEP156_CHECK(hw_adc(0x40u)==0x6Du,
                  "CTS setter reaches raw U10/CTS selector $40 without bypassing conversion");

    bua_hal_set_mat_adc(0x93u);
    STEP156_CHECK(hw_adc(0x80u)==0x93u,
                  "MAT setter reaches raw U10/MAT selector $80 before firmware complementing");

    printf("  step-156 CTS/MAT HAL regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);

    /* This HAL regression is invoked before the frozen dynamic-drive baselines.
     * Restore the raw stimuli so the test itself cannot perturb later behavior.
     */
    sim_cts_adc=saved_cts;
    sim_mat_adc=saved_mat;
#undef STEP156_CHECK

    run_step157_remaining_u10_hal_test();
}

static void run_step155_named_input_hal_test(void)
{
    unsigned int passed=0u;
    unsigned int total=4u;
#define STEP155_CHECK(c,t) do { if(c) ++passed; printf("  %-84s %s\n",t,(c)?"PASS":"FAIL"); } while(0)

    printf("\nStep-155 named raw input HAL regression:\n");
    ecm_reset();

    bua_hal_set_maf_adc(0x5Au);
    STEP155_CHECK(hw_adc(0xA0u)==0x5Au,
                  "MAF setter reaches the existing raw U10/VMAF selector source");

    bua_hal_set_tps_adc(0x46u);
    STEP155_CHECK(hw_adc(0x50u)==0x46u,
                  "TPS setter reaches the existing raw U10/TPS selector source");

    bua_hal_set_diag_adc(0xC3u);
    STEP155_CHECK(hw_adc(0x70u)==0xC3u,
                  "DIAG setter reaches the existing raw U10/DIAG selector source");

    bua_hal_set_u9_knock_counter(0x1234u);
    STEP155_CHECK(bua_hal_get_mpu16be(0x3FCAu)==0x1234u,
                  "knock-counter setter reaches raw U9 $3FCA without assigning edge semantics");

    printf("  step-155 named-input HAL regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP155_CHECK

    run_step156_cts_mat_hal_test();
}

static void run_step128_hal_interface_test(void)
{
    unsigned int passed=0u;
    unsigned int total=15u;
#define STEP128_CHECK(c,t) do { if(c) ++passed; printf("  %-84s %s\n",t,(c)?"PASS":"FAIL"); } while(0)

    printf("\nStep-128 explicit HAL interface regression:\n");
    ecm_reset();

    bua_hal_set_o2_adc(0x5Au);
    STEP128_CHECK(sim_o2_adc==0x5Au,
                  "raw O2 setter reaches the existing ECM-facing O2 stimulus byte");

    bua_hal_set_battery_adc(0x77u);
    STEP128_CHECK(BATTERY_AD==0x77u,
                  "raw battery setter reaches the existing ECM-facing battery A/D byte");

    bua_hal_set_ram8(0x0010u,0xA5u);
    STEP128_CHECK(bua_hal_get_ram8(0x0010u)==0xA5u,
                  "generic low-RAM setter/getter preserves a processor-visible raw byte");
    STEP128_CHECK(bua_hal_get_ram8(0x0200u)==0u,
                  "generic low-RAM observer rejects addresses outside the translated low-RAM window");

    mem.mpu[0x0Cu]=0x11u; /* $3FCC */
    mem.mpu[0x10u]=0x12u; /* $3FD0 high byte */
    mem.mpu[0x11u]=0x34u; /* $3FD0 low byte */
    mem.mpu[0x12u]=0x22u; /* $3FD2 */
    mem.mpu[0x14u]=0x33u; /* $3FD4 */
    mem.mpu[0x16u]=0x44u; /* $3FD6 */
    mem.mpu[0x18u]=0x55u; /* $3FD8 */
    mem.io4000[4u]=0x66u;
    mem.io5000=0x77u;

    STEP128_CHECK(bua_hal_get_output_3fcc()==0x11u,
                  "named $3FCC observer returns the raw translated MPU byte");
    STEP128_CHECK(bua_hal_get_output_3fd2()==0x22u &&
                  bua_hal_get_output_3fd4()==0x33u,
                  "named $3FD2/$3FD4 observers return raw translated MPU bytes");
    STEP128_CHECK(bua_hal_get_output_3fd6()==0x44u &&
                  bua_hal_get_output_3fd8()==0x55u,
                  "named $3FD6/$3FD8 observers return raw translated MPU bytes");
    STEP128_CHECK(bua_hal_get_output_4004()==0x66u,
                  "named $4004 observer returns the raw translated parallel-I/O byte");
    STEP128_CHECK(bua_hal_get_mpu16be(0x3FD0u)==0x1234u,
                  "generic 16-bit MPU observer preserves the translated big-endian register word");
    STEP128_CHECK(bua_hal_get_injector_pw_counts()==0x1234u,
                  "named injector observer reports the existing raw synchronous-fuel command word");
    STEP128_CHECK(bua_hal_get_io5000()==0x77u,
                  "raw $5000 observer reports the translated one-byte I/O window");

    RAM8(0x002Cu)=0x39u;
    STEP128_CHECK(bua_hal_get_iac_position()==0x39u,
                  "named IAC observer reports the existing raw position bookkeeping byte");

    bua_hal_set_vss_mph(60u);
    STEP128_CHECK(sim_vss_period_ticks!=0u,
                  "VSS HAL command delegates to the existing phase/timing source");

    bua_hal_set_reference_rpm(2400u);
    STEP128_CHECK(ram16be_get(REF_PERIOD_ADDR)!=0u || stats.ref_pulses==0ul,
                  "reference-RPM HAL command is accepted without inventing a new timing model");

    STEP128_CHECK(bua_hal_get_mpu8(0x3FBFu)==0u &&
                  bua_hal_get_io4000(0x4010u)==0u,
                  "raw observers reject addresses outside their translated hardware windows");

    printf("  step-128 HAL-interface regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP128_CHECK

    run_step155_named_input_hal_test();
}
