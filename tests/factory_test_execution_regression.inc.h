static void run_step118_factory_execution_test(void)
{
    unsigned int passed=0u;
    unsigned int total=31u;
    unsigned int i;
    bua_u32 ordinary_irqs;
#define S118(c,t) do { if(c) ++passed; printf("  %-78s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-118 factory-test execution regression:\n");

    ecm_reset();
    RAM8(0x0048u)=0u; RAM8(0x0049u)=0u; RAM8(0x0032u)=0u;
    RAM8(0x0031u)=0x04u; mpu16be_set(0x3FFCu,0x1200u);
    mem.io4000[4]=0xFFu; mpu16be_set(0x3FD0u,0x1234u);
    bua_factory_execute_step118();
    S118(bua_factory_trace118.raw_zero_paths==1ul &&
         bua_factory_trace118.calls==1ul,
         "complete-byte zero at $FD31 selects the raw-zero path");
    S118(RAM8(0x0031u)==0x04u,
         "zero timer and clear FMD low bits suppress the conditional L0031 toggle");
    S118(mpu16be_get(0x3FFCu)==0x1208u,
         "raw-zero all-off path clears async/spark bits and sets lamp-off bit");
    S118(mpu16be_get(0x3FD0u)==0u && mem.io4000[4]==0xF9u,
         "all-off path clears synchronous fuel and parallel-I/O bits 1 and 2");
    S118(mpu16be_get(0x3FCCu)==0x7000u &&
         mpu16be_get(0x3FD2u)==0x7000u &&
         mpu16be_get(0x3FD4u)==0x7000u &&
         mpu16be_get(0x3FD6u)==0x7000u &&
         mpu16be_get(0x3FD8u)==0x7000u &&
         mpu16be_get(0x3FDAu)==0x7000u,
         "$FD98 all-off call writes $7000 to all six emitted PWM addresses");

    ecm_reset(); RAM8(0x0048u)=0u; RAM8(0x0049u)=0x04u;
    RAM8(0x0032u)=1u; RAM8(0x0031u)=0x04u; mpu16be_set(0x3FFCu,0u);
    bua_factory_execute_step118();
    S118(RAM8(0x0031u)==0x06u && (mpu16be_get(0x3FFCu)&0x0004u)!=0u,
         "nonzero timer stores toggled FMD output and FMD bit 2 retains CSR bit 2");

    ecm_reset();
    for(i=0u;i<0x002Eu;++i) RAM8((bua_u16)i)=1u;
    RAM8(0x0048u)=2u; RAM8(0x0031u)=0x04u;
    mpu16be_set(0x3FFCu,0u);
    bua_factory_execute_step118();
    S118(bua_factory_trace118.raw_even_nonzero_paths==1ul &&
         RAM8(0x0031u)==0x06u,
         "nonzero raw mode with bit 0 clear toggles L0031 and selects misc path");
    S118(mpu16be_get(0x3FF2u)==0x00C5u && mpu16be_get(0x3FFCu)==0x0008u,
         "misc path writes async count 197 before the common all-off CSR state");
    S118(ram16be_get(0x0175u)==46u && bua_factory_trace118.nv_checksums==1ul,
         "LF32E-equivalent checksum sums exactly 46 bytes at $0000..$002D");
    S118(mem.io4000[0x0Bu]==0xFFu && mem.io4000[0x0Cu]==0u &&
         bua_factory_trace118.checksum_watchdog_strobes==1ul,
         "46-byte LF32E checksum reaches its single emitted COP restart");
    S118(bua_factory_trace118.all_off_paths==1ul &&
         bua_factory_trace118.last_pwm_word==0x7000u,
         "nonzero-even path reaches the same six-channel all-off output state");

    ecm_reset();
    for(i=0u;i<12u;++i) sim_factory_adc118[i]=(bua_u8)(0x20u+i);
    RAM8(0x0048u)=1u; RAM8(0x0172u)=1u; RAM8(0x0031u)=0u;
    RAM8(0x0187u)=0xA5u; RAM8(0x0188u)=0x5Au;
    mpu16be_set(0x3FC0u,400u);
    bua_factory_execute_step118();
    S118(bua_factory_trace118.raw_low_bit_paths==1ul,
         "set raw mode bit 0 selects the A/D and fuel/spark path");
    for(i=0u;i<12u && RAM8((bua_u16)(0x017Bu+i))==(bua_u8)(0x20u+i);++i) { }
    S118(i==12u && bua_factory_trace118.adc_sweeps==1ul,
         "$FDB5..$FDC5 captures all twelve HAL A/D channels in source order");
    S118(RAM8(0x0187u)==0xA5u && RAM8(0x0188u)==0x5Au &&
         bua_factory_trace118.pwm_exercises==0ul,
         "non-quarter/non-100-ms count skips coolant and PWM exercises");
    S118(mpu16be_get(0x3FD0u)==327u && mpu16be_get(0x3FDCu)==262u &&
         mpu16be_get(0x3FF6u)==0u && mpu16be_get(0x3FCEu)==0u,
         "400-count reference selects the emitted middle fuel/spark branch");

    RAM8(0x0172u)=4u; RAM8(0x0031u)=0u;
    sim_factory_adc118[4]=0x66u; bua_factory_execute_step118();
    S118(RAM8(0x0188u)==0x66u && bua_factory_trace118.coolant_0188_captures==1ul,
         "clear L0031 bit 0 stores the sampled coolant byte at L0188");
    S118((RAM8(0x0031u)&0x01u)!=0u,
         "quarter-count coolant service toggles L0031 bit 0 after capture");
    RAM8(0x0172u)=8u; sim_factory_adc118[4]=0x77u;
    bua_factory_execute_step118();
    S118(RAM8(0x0187u)==0x77u && (RAM8(0x0031u)&0x01u)==0u &&
         bua_factory_trace118.coolant_0187_captures==1ul,
         "set pull-up bit stores L0187 then toggles back to the alternate state");

    S118(bua_factory_pwm_word_step118(15u)==0xD200u &&
         bua_factory_pwm_word_step118(16u)==0xD066u &&
         bua_factory_pwm_word_step118(17u)==0xD39Au,
         "PWM selector preserves the strict below/equal/above-16 boundaries");
    S118(bua_factory_pwm_word_step118(33u)==0xD39Au &&
         bua_factory_pwm_word_step118(48u)==0xD200u &&
         bua_factory_pwm_word_step118(64u)==0xD066u &&
         bua_factory_pwm_word_step118(65u)==0xD39Au,
         "counter values above 32 use the emitted unsigned subtract-$30 fold");

    ecm_reset(); RAM8(0x0048u)=1u; RAM8(0x0172u)=0u;
    RAM8(0x0179u)=0x81u; mem.io4000[2]=0xA0u; mem.io4000[4]=0x06u;
    mpu16be_set(0x3FC0u,400u); mpu16be_set(0x3FFCu,0u);
    bua_factory_execute_step118();
    S118(bua_factory_trace118.last_pwm_word==0xD200u &&
         mpu16be_get(0x3FCCu)==0xD200u && mpu16be_get(0x3FDAu)==0xD200u,
         "100-ms counter zero writes selected duty to the entire six-PWM set");
    S118((mpu16be_get(0x3FFCu)&0x0018u)==0x0010u &&
         (mem.io4000[4]&0x06u)==0x04u,
         "clear counter bit 4 sets CSR bit 4, clears bit 3 and clears I/O bit 1 only");
    S118(RAM8(0x0179u)==0x03u && (mem.io4000[2]&0x03u)==0x03u &&
         (mem.io4000[2]&0xFCu)==0xA0u,
         "factory IAC uses an 8-bit rotate-left and replaces only latch bits 0/1");

    RAM8(0x0172u)=16u; mem.io4000[4]=0u; mpu16be_set(0x3FFCu,0u);
    bua_factory_execute_step118();
    S118(bua_factory_trace118.last_pwm_word==0xD066u &&
         (mpu16be_get(0x3FFCu)&0x0018u)==0x0018u &&
         (mem.io4000[4]&0x06u)==0x06u,
         "set counter bit 4 selects $D066 and sets emitted lamp/fan bits");

    mpu16be_set(0x3FC0u,656u); bua_factory_fuel_spark_step118();
    S118(ram16be_get(0x0177u)==164u && mpu16be_get(0x3FD0u)==655u &&
         mpu16be_get(0x3FDCu)==327u && mpu16be_get(0x3FF6u)==164u &&
         mpu16be_get(0x3FCEu)==0u,
         "reference 656 selects 10-ms fuel, 5-ms dwell and quarter-period fire");
    mpu16be_set(0x3FC0u,655u); bua_factory_fuel_spark_step118();
    S118(ram16be_get(0x0177u)==163u && mpu16be_get(0x3FD0u)==327u &&
         mpu16be_get(0x3FDCu)==262u && mpu16be_get(0x3FF6u)==0u,
         "reference equality 655 selects the middle branch");
    mpu16be_set(0x3FC0u,328u); bua_factory_fuel_spark_step118();
    S118(ram16be_get(0x0177u)==82u && mpu16be_get(0x3FD0u)==327u &&
         mpu16be_get(0x3FDCu)==262u && mpu16be_get(0x3FCEu)==0u,
         "reference equality 328 remains in the middle branch");
    mpu16be_set(0x3FC0u,327u); bua_factory_fuel_spark_step118();
    S118(ram16be_get(0x0177u)==81u && mpu16be_get(0x3FD0u)==66u &&
         mpu16be_get(0x3FDCu)==197u && mpu16be_get(0x3FF6u)==65455u &&
         mpu16be_get(0x3FCEu)==66u,
         "reference 327 selects 1-ms fuel/delay and wrapped negative fire offset");

    ecm_reset(); bua_factory_boot_step117(0u,1u);
    RAM8(0x0048u)=1u; RAM8(0x0172u)=1u;
    sim_factory_fmd_byte1_117=1u; sim_factory_battery_adc117=90u;
    sim_factory_diagnostic_adc117=40u; mpu16be_set(0x3FC0u,400u);
    ordinary_irqs=stats.irq_ticks; irq_6p25ms();
    S118(bua_factory_trace117.irq_routes==1ul &&
         bua_factory_trace118.calls==1ul && stats.irq_ticks==ordinary_irqs,
         "factory IRQ flows directly from Step 117 into Step 118, not normal scheduler");
    S118(bua_factory_trace118.wait_loop_handoffs==1ul,
         "$FEA3 wait-loop handoff is recorded after the integrated IRQ execution");

    ecm_reset(); RAM8(0x0048u)=0u; bua_factory_execute_step118();
    RAM8(0x0048u)=2u; bua_factory_execute_step118();
    RAM8(0x0048u)=3u; RAM8(0x0172u)=1u; bua_factory_execute_step118();
    S118(bua_factory_trace118.wait_loop_handoffs==3ul &&
         bua_factory_trace118.raw_zero_paths==1ul &&
         bua_factory_trace118.raw_even_nonzero_paths==1ul &&
         bua_factory_trace118.raw_low_bit_paths==1ul,
         "all three raw mode branches converge on one wait-loop handoff each");

    printf("  step-118 factory-test execution regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef S118
}
