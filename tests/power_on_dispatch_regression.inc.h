static BuaPowerOnInput120 bua_power_input_step120(void)
{
    BuaPowerOnInput120 in;
    memset(&in,0,sizeof(in));
    in.socket_checksum_valid=1u;
    in.battery_adc=128u;
    in.diagnostic_adc=200u;
    return in;
}

static void bua_power_valid_retained_step120(void)
{
    RAM8(0x0005u)=1u;
    RAM8(0x0006u)=2u;
    RAM8(0x0007u)=3u;
    RAM8(0x0008u)=4u;
    RAM8(0x0009u)=5u;
    ram16be_set(0x0018u,bua_error_word_checksum_step113());
}

static void run_step120_power_on_dispatch_test(void)
{
    unsigned int passed=0u;
    unsigned int total=24u;
    BuaPowerOnInput120 in;
    bua_u8 outcome;
#define S120(c,t) do { if(c) ++passed; printf("  %-78s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-120 unified power-on-dispatch regression:\n");

    ecm_reset();
    bua_power_valid_retained_step120();
    RAM8(0x002Du)=0x5Au;
    RAM8(0x002Eu)=0xA5u;
    RAM8(0x01B2u)=0xC3u;
    mpu16be_set(0x3FC0u,0x1234u);
    mpu16be_set(0x3FF8u,0x5678u);
    mpu16be_set(0x3FFAu,0x9ABCu);
    in=bua_power_input_step120();
    in.initial_fmd_byte1=0x20u;
    outcome=bua_power_on_step120(in);
    S120(outcome==POWER120_OUTCOME_NORMAL &&
         bua_power_on_trace120.calls==1ul &&
         bua_power_on_trace120.normal_handoffs==1ul,
         "valid retained state reaches the normal-startup handoff");
    S120(bua_power_on_trace120.volatile_ram_clears==389ul &&
         RAM8(0x002Du)==0x5Au && RAM8(0x002Eu)==0u &&
         RAM8(0x01B2u)==0u,
         "C83B clears exactly L002E..L01B2 and retains L002D");
    S120(mpu16be_get(0x3FC0u)==0u && mpu16be_get(0x3FF8u)==0u &&
         mpu16be_get(0x3FFAu)==0x9ABCu,
         "C813 clears MPU words through 3FF8 but does not clear 3FFA");
    S120(mem.io5000==0x08u && mem.io4000[1]==0x8Cu &&
         mem.io4000[3]==0x8Fu && mem.io4000[4]==0x98u,
         "power-on emits the listing-exact initial I/O register values");
    S120(RAM8(0x0005u)==1u && RAM8(0x0009u)==5u &&
         bua_power_on_trace120.retained_checksum_valid!=0u,
         "valid retained error bytes survive the unified power-on path");
    S120(bua_startup_trace114.scheduler_handoff!=0u &&
         mpu16be_get(0x3FFCu)==0xFB1Eu,
         "normal startup arms its scheduler from the C800 run-state CSR");

    ecm_reset();
    RAM8(0x0005u)=0x44u;
    ram16be_set(0x0018u,0u);
    in=bua_power_input_step120();
    outcome=bua_power_on_step120(in);
    S120(outcome==POWER120_OUTCOME_NORMAL &&
         bua_power_on_trace120.retained_recoveries==1ul,
         "bad retained checksum executes recovery before normal startup");
    S120(RAM8(0x0005u)==0u && RAM8(0x0009u)==0u &&
         RAM8(0x002Cu)==144u && RAM8(0x001Cu)==128u &&
         RAM8(0x002Bu)==128u,
         "recovery clears retained errors and initializes IAC/BLM state");
    S120(ram16be_get(0x0018u)==bua_error_word_checksum_step113(),
         "recovery stores the checksum of the recovered error words");

    ecm_reset();
    bua_power_valid_retained_step120();
    memset(&bua_startup_trace114,0,sizeof(bua_startup_trace114));
    in=bua_power_input_step120();
    in.battery_adc=99u;
    in.pump_adc=160u;
    in.diagnostic_adc=40u;
    in.whole_rom_checksum=0x1234u;
    outcome=bua_power_on_step120(in);
    S120(outcome==POWER120_OUTCOME_FACTORY &&
         bua_power_on_trace120.factory_handoffs==1ul &&
         bua_power_on_trace120.normal_handoffs==0ul,
         "low battery plus pump and diagnostic thresholds select factory boot");
    S120(bua_startup_trace114.scheduler_handoff==0u &&
         (RAM8(0x0047u)&0x80u)!=0u,
         "factory selection cannot fall through into normal initialization");
    S120(ram16be_get(0x0173u)==0x1234u && RAM8(0x0001u)==0xAAu &&
         RAM8(0x002Du)==0xAAu,
         "factory boot receives the explicit ROM checksum and performs AA fill");
    S120(bua_power_on_trace120.retained_recoveries==0ul,
         "factory branch precedes retained-checksum recovery");

    ecm_reset();
    bua_power_valid_retained_step120();
    in=bua_power_input_step120();
    in.battery_adc=99u;
    in.pump_adc=159u;
    in.diagnostic_adc=40u;
    outcome=bua_power_on_step120(in);
    S120(outcome==POWER120_OUTCOME_NORMAL && (RAM8(0x0040u)&0x20u)!=0u,
         "battery flag is set even when pump threshold rejects factory boot");

    ecm_reset();
    bua_power_valid_retained_step120();
    in=bua_power_input_step120();
    in.socket_checksum_valid=0u;
    in.diagnostic_adc=40u;
    outcome=bua_power_on_step120(in);
    S120(outcome==POWER120_OUTCOME_SWI &&
         bua_power_on_trace120.swi_boundaries==1ul,
         "socket error with diagnostic input at 40 reaches the C938 SWI boundary");
    S120(bua_vector_trace119.last_swi_source==0xC938u &&
         (RAM8(0x0007u)&1u)!=0u && (RAM8(0x0047u)&0x10u)!=0u,
         "Error 51 latch, lamp request, and exact SWI source are recorded");
    S120(bua_power_on_trace120.normal_handoffs==0ul &&
         bua_power_on_trace120.factory_handoffs==0ul,
         "SWI outcome does not execute either startup handoff");

    ecm_reset();
    bua_power_valid_retained_step120();
    in=bua_power_input_step120();
    in.socket_checksum_valid=0u;
    in.diagnostic_adc=39u;
    outcome=bua_power_on_step120(in);
    S120(outcome==POWER120_OUTCOME_NORMAL,
         "socket error below diagnostic threshold continues to normal startup");

    ecm_reset();
    bua_power_valid_retained_step120();
    RAM8(0x0047u)=0x80u;
    in=bua_power_input_step120();
    in.socket_checksum_valid=0u;
    in.diagnostic_adc=40u;
    outcome=bua_power_on_step120(in);
    S120(outcome==POWER120_OUTCOME_SWI && (RAM8(0x0047u)&0x80u)==0u,
         "volatile clear removes a stale factory flag before the SWI decision");

    ecm_reset();
    bua_power_valid_retained_step120();
    in=bua_power_input_step120();
    in.err51_at_entry=1u;
    outcome=bua_power_on_step120(in);
    S120(outcome==POWER120_OUTCOME_NORMAL &&
         (MODE_WORD3&0x08u)!=0u && mpu16be_get(0x3FFCu)==0xFB16u,
         "entry Error 51 selects the alternate boot/run CSR words");

    ecm_reset();
    bua_power_valid_retained_step120();
    in=bua_power_input_step120();
    in.hud_present=1u;
    outcome=bua_power_on_step120(in);
    S120(outcome==POWER120_OUTCOME_NORMAL &&
         bua_power_on_trace120.hud_boundaries==1ul,
         "HUD-present input records the absent-ROM call boundary only");

    ecm_reset();
    bua_power_valid_retained_step120();
    in=bua_power_input_step120();
    outcome=bua_power_on_from_vector_step120(0xFFF8u,in);
    S120(outcome==POWER120_OUTCOME_NORMAL &&
         bua_power_on_trace120.vector_acknowledgements==1ul &&
         bua_vector_trace119.last_target==0xC800u,
         "reset vector request is explicitly acknowledged by real power-on");
    S120(bua_power_on_from_vector_step120(0xFFF2u,in)==
         POWER120_OUTCOME_INVALID,
         "non-reset vector cannot enter the Step-120 power-on dispatcher");

    ecm_reset();
    bua_power_valid_retained_step120();
    in=bua_power_input_step120();
    in.battery_adc=100u;
    in.pump_adc=160u;
    in.diagnostic_adc=40u;
    S120(bua_power_on_step120(in)==POWER120_OUTCOME_NORMAL,
         "battery threshold is unsigned and excludes the equality value 100");

    printf("  step-120 power-on-dispatch regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef S120
}
