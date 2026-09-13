/* Step 158 regression for the normal-operation FMD/SPI raw boundary. */
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
