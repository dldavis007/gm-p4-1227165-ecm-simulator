static void run_step112_startup_reset_test(void)
{
    unsigned int passed = 0u;
    unsigned int total = 19u;
    BuaStartupInput112 in;
    BuaStartupResult112 r;
#define STEP112_CHECK(c,t) do { if(c) ++passed; printf("  %-78s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-112 source-ordered reset/startup regression:\n");

    memset(&in, 0, sizeof(in));
    in.socket_checksum_valid=1u; in.battery_adc=128u;
    in.diagnostic_adc=200u; in.retained_checksum_valid=1u;
    r=bua_startup_evaluate_step112(in);
    STEP112_CHECK(r.boot_mpu_word==0x000Au && r.run_mpu_word==0xFB1Au,
                  "$C800/$C881 normal MPU control words are 000A then FB1A");
    STEP112_CHECK(r.io_prescaler==0x8Cu && r.io_direction==0x8Fu && r.io_control==0x98u,
                  "$C826..$C84A peripheral writes retain literal values and bit set");
    STEP112_CHECK(r.socket_error51==0u,
                  "valid socket checksum clears the modeled Error-51 result");
    STEP112_CHECK(r.factory_test==0u && r.hud_call==0u,
                  "battery 128 branches at $C897 to the non-factory optional-ROM check");
    STEP112_CHECK(r.retained_reinitialized==0u,
                  "matching retained-error checksum bypasses $C8F5 RAM recovery");
    STEP112_CHECK(r.serial_8192_boundary==1u && r.scheduler_entered==1u,
                  "normal path reaches $C9C0 boundary and $C9F1 scheduler transfer");

    in.err51_at_entry=1u; in.socket_checksum_valid=0u;
    r=bua_startup_evaluate_step112(in);
    STEP112_CHECK(r.boot_mpu_word==0x0002u && r.run_mpu_word==0xFB12u,
                  "entry Error 51 selects alternate MPU words 0002 then FB12");
    STEP112_CHECK(r.socket_error51==1u,
                  "invalid socket checksum records the $C87E Error-51 outcome");
    STEP112_CHECK(r.swi_wait==1u && r.scheduler_entered==0u,
                  "$C87E Error 51 feeds the later $C90E diagnostic SWI decision");

    memset(&in,0,sizeof(in)); in.socket_checksum_valid=1u;
    in.battery_adc=99u; in.pump_adc=160u; in.diagnostic_adc=40u;
    r=bua_startup_evaluate_step112(in);
    STEP112_CHECK(r.factory_test==1u,
                  "exact emitted boundaries 99/160/40 enter factory test at $C8B2");
    STEP112_CHECK(r.factory_fill_aa==1u && r.scheduler_entered==1u,
                  "zero FMD low bits select AA fill and direct $C9E3 transfer");
    in.fmd_status_low2=1u; r=bua_startup_evaluate_step112(in);
    STEP112_CHECK(r.factory_fill_aa==0u,
                  "nonzero FMD low bits bypass the factory AA-fill loop");
    in.battery_adc=100u; r=bua_startup_evaluate_step112(in);
    STEP112_CHECK(r.factory_test==0u,
                  "battery equality takes BCC at $C897 and bypasses factory test");
    in.battery_adc=99u; in.pump_adc=159u; r=bua_startup_evaluate_step112(in);
    STEP112_CHECK(r.factory_test==0u,
                  "pump 159 takes BCS at $C8A3 and bypasses factory test");
    in.pump_adc=160u; in.diagnostic_adc=100u; r=bua_startup_evaluate_step112(in);
    STEP112_CHECK(r.factory_test==0u,
                  "diagnostic equality 100 takes BCC at $C8AC");
    in.diagnostic_adc=39u; r=bua_startup_evaluate_step112(in);
    STEP112_CHECK(r.factory_test==0u,
                  "diagnostic 39 takes BCS at $C8B0");

    memset(&in,0,sizeof(in)); in.socket_checksum_valid=1u;
    in.battery_adc=128u; in.diagnostic_adc=200u; in.hud_present=1u;
    r=bua_startup_evaluate_step112(in);
    STEP112_CHECK(r.hud_call==1u,
                  "matching $5800 identity records the external $5812 HUD call boundary");
    STEP112_CHECK(r.retained_reinitialized==1u,
                  "invalid retained checksum selects RAM, checksum, BLM and IAC recovery");
    in.retained_error_latched=1u; r=bua_startup_evaluate_step112(in);
    STEP112_CHECK(r.swi_wait==1u && r.scheduler_entered==0u,
                  "latched error with diagnostic input records $C938 SWI wait boundary");

    printf("  step-112 reset/startup regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP112_CHECK
}
