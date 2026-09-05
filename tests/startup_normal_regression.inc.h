static void run_step114_startup_normal_test(void)
{
    unsigned int passed=0u;
    unsigned int total=14u;
    bua_u8 coolant;
#define STEP114_CHECK(c,t) do { if(c) ++passed; printf("  %-78s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-114 source-ordered normal-startup regression:\n");

    ecm_power_on_step114();
    coolant=(COOLANT>208u)?208u:COOLANT;
    STEP114_CHECK(bua_startup_trace114.segment6_called!=0u &&
                  bua_startup_trace114.segment_e_called!=0u,
                  "LC93B calls LF3B6 before the later LDE72 Segment-E call");
    STEP114_CHECK(bua_startup_trace114.enrichment_initialized!=0u,
                  "clear retained-mode b3 executes the three coolant startup lookups");
    STEP114_CHECK(ram16be_get(0x000Eu)==
                  (bua_u16)((bua_u16)bua_lookup_spacing16(
                    cal_startup_enrich_step48,14u,coolant)<<8),
                  "C3E4 startup enrichment is stored as a high-byte value");
    STEP114_CHECK(RAM8(0x00CDu)==bua_lookup_spacing16(
                    cal_open_loop_coolant_step48,14u,coolant),
                  "C41F open-loop coolant AFR lookup is stored at L00CD");
    STEP114_CHECK(RAM8(0x0013u)==bua_lookup_fixed16_table(
                    cal_startup_spark,(bua_u8)CAL_STARTUP_SPARK_POINTS,coolant),
                  "C17A startup spark lookup is stored at L0013 without extra side effects");
    STEP114_CHECK(RAM8(0x006Fu)==102u && RAM8(0x0071u)==102u &&
                  RAM8(0x0073u)==102u,
                  "LC990 initializes all three O2 filter bytes from LC3A0");
    STEP114_CHECK(RAM8(0x0081u)==sim_tps_adc && RAM8(0x0086u)==35u,
                  "TPS A/D is sampled and LF35A begins from LC39B minimum TPS");
    STEP114_CHECK(ram16be_get(0x00B2u)==mpu16be_get(0x3FC8u) &&
                  ram16be_get(0x00CEu)==445u,
                  "spark-period history and LC3CB stoichiometric AFR are initialized");
    STEP114_CHECK(RAM8(0x0004u)==7u,
                  "non-retained startup stores transmission mode seven at L0004");
    STEP114_CHECK(RAM8(0x012Du)==0u && RAM8(0x012Fu)==0u &&
                  RAM8(0x0130u)==0u && ram16be_get(0x0131u)==0u,
                  "LFA58 clears directly addressed SCI status, count, checksum and pointer RAM");
    STEP114_CHECK(bua_startup_trace114.sci_device_boundary==0u &&
                  RAM8(0x0133u)==0u && RAM8(0x016Fu)==0u,
                  "Step 115 resolves LFA58's sole-device ICB/OCB clear boundary");
    STEP114_CHECK(mpu16be_get(0x3FFCu)==0xFB06u,
                  "LF4A8 with A=$04 emits the listing-exact MPU CSR update");
    STEP114_CHECK(MINOR_COUNT==0x0Eu && BLM==128u &&
                  RAM8(0x00C6u)==128u && RAM8(0x00C9u)==128u &&
                  ALCL_TABLE_INDEX==0x19u && RAM8(0x0068u)==160u &&
                  RAM8(0x010Cu)==144u,
                  "LC9C8..LC9E0 installs the final normal-startup RAM constants");
    STEP114_CHECK(mem.io4000[6]==(bua_u8)(mem.io4000[5]+2u) &&
                  (mem.io4000[7]&0x01u)!=0u &&
                  bua_startup_trace114.timer_armed!=0u &&
                  bua_startup_trace114.scheduler_handoff!=0u,
                  "LC9E3 arms the timer match and reaches the scheduler handoff");

    printf("  step-114 normal-startup regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP114_CHECK
}
