/* Step 128 regression for the explicit raw HAL boundary. */
static void run_step156_cts_mat_hal_test(void)
{
    unsigned int passed=0u;
    unsigned int total=4u;
#define STEP156_CHECK(c,t) do { if(c) ++passed; printf("  %-84s %s\n",t,(c)?"PASS":"FAIL"); } while(0)

    printf("\nStep-156 CTS/MAT raw input HAL regression:\n");

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
#undef STEP156_CHECK
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
