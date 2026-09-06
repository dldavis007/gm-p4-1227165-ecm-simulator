static void run_step121_lifecycle_integration_test(void)
{
    unsigned int passed=0u;
    unsigned int total=18u;
    BuaPowerOnInput120 in;
    bua_u8 outcome;
    bua_u32 irq_before;
#define S121(c,t) do { if(c) ++passed; printf("  %-78s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-121 end-to-end ignition-lifecycle regression:\n");

    ecm_reset();
    bua_lifecycle_reset_trace_step121();
    bua_power_valid_retained_step120();
    in=bua_power_input_step120();
    outcome=bua_lifecycle_power_cycle_step121(0xFFFEu,in);
    S121(outcome==POWER120_OUTCOME_NORMAL &&
         bua_lifecycle_trace121.power_cycle_acknowledgements==1ul,
         "host reset acknowledgement enters the real C800 power-on path");
    S121(bua_power_on_trace120.vector_acknowledgements==1ul &&
         bua_startup_trace114.scheduler_handoff!=0u,
         "reset vector reaches normal initialization and scheduler handoff");
    S121(RAM8(0x0005u)==1u && RAM8(0x0009u)==5u &&
         bua_power_on_trace120.retained_checksum_valid!=0u,
         "first power-on preserves checksum-valid retained error bytes");

    bua_lifecycle_irq_step121();
    bua_lifecycle_irq_step121();
    S121(stats.irq_ticks==2ul && stats.air_fuel_loops==1ul &&
         stats.spark_vss_loops==1ul,
         "normal power-on executes one odd and one even IRQ branch");

    RAM8(0x0033u)=0x30u;
    RAM8(0x0034u)=(bua_u8)(RAM8(0x0034u)|0x80u);
    RAM8(0x000Au)=160u;
    RAM8(0x000Cu)=110u;
    RAM8(0x002Cu)=50u;
    ram16be_set(0x008Bu,8u);
    bua_lifecycle_irq_step121();
    S121(stats.ignition_shutdown_calls!=0ul &&
         (RAM8(0x0034u)&0x80u)==0u,
         "key-off odd branch clears engine-running state through Step 111");
    S121(RAM8(0x001Cu)==150u && RAM8(0x001Du)==118u &&
         RAM8(0x002Bu)==118u,
         "key-off commits bounded SAM values into retained BLM cells");
    S121(ram16be_get(0x0018u)==bua_error_word_checksum_step113(),
         "shutdown leaves the retained error-word checksum valid");

    bua_lifecycle_irq_step121();
    S121(stats.iac_shutdown_homing_calls!=0ul &&
         (RAM8(0x00F3u)&0x04u)!=0u && RAM8(0x0101u)==0xFFu,
         "following even branch begins the listing-backed IAC close phase");

    ram16be_set(0x008Bu,STEP111_POWERDOWN_COUNT);
    bua_lifecycle_irq_step121();
    S121(sim_soft_powerdown_latched!=0u &&
         stats.soft_powerdown_events==1ul &&
         bua_vector_trace119.last_swi_source==0xD6EAu,
         "terminal key-off reaches the exact D6EA software-powerdown boundary");
    irq_before=stats.irq_ticks;
    bua_lifecycle_irq_step121();
    S121(stats.irq_ticks==irq_before &&
         bua_lifecycle_trace121.blocked_irq_requests==1ul,
         "latched software powerdown blocks subsequent IRQ execution");

    outcome=bua_lifecycle_power_cycle_step121(0xFFF8u,in);
    S121(outcome==POWER120_OUTCOME_NORMAL &&
         sim_soft_powerdown_latched==0u &&
         bua_lifecycle_trace121.power_cycle_acknowledgements==2ul,
         "acknowledged power cycle releases shutdown and performs a restart");
    S121(bua_power_on_trace120.retained_checksum_valid!=0u &&
         bua_power_on_trace120.retained_recoveries==0ul,
         "restart accepts the retained checksum without recovery");
    S121(RAM8(0x0005u)==1u && RAM8(0x0009u)==5u &&
         RAM8(0x001Cu)==150u && RAM8(0x002Bu)==118u,
         "restart preserves retained errors and shutdown-committed BLM cells");
    S121(bua_startup_trace114.scheduler_handoff!=0u,
         "retained restart returns to the normal scheduler handoff");

    RAM8(0x0005u)=0x77u;
    sim_soft_powerdown_latched=1u;
    outcome=bua_lifecycle_power_cycle_step121(0xFFFAu,in);
    S121(outcome==POWER120_OUTCOME_NORMAL &&
         bua_power_on_trace120.retained_recoveries==1ul,
         "corrupted retained error byte triggers recovery on later restart");
    S121(RAM8(0x0005u)==0u && RAM8(0x0009u)==0u &&
         RAM8(0x001Cu)==128u && RAM8(0x002Bu)==128u,
         "corrupt restart restores cleared errors and default BLM cells");
    S121(ram16be_get(0x0018u)==bua_error_word_checksum_step113(),
         "corrupt restart stores a new valid retained checksum");

    ecm_reset();
    bua_lifecycle_reset_trace_step121();
    in=bua_power_input_step120();
    in.battery_adc=99u;
    in.pump_adc=160u;
    in.diagnostic_adc=40u;
    outcome=bua_lifecycle_power_cycle_step121(0xFFFCu,in);
    S121(outcome==POWER120_OUTCOME_FACTORY &&
         bua_power_on_trace120.normal_handoffs==0ul &&
         (RAM8(0x0047u)&0x80u)!=0u,
         "factory lifecycle outcome remains isolated from normal startup");

    printf("  step-121 ignition-lifecycle integration result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef S121
}
