static void run_step107_diagnostics_stage1_test(void)
{
    unsigned int pass=0u,total=0u;
    bua_u8 route;
#define S107(C) do { ++total; if(C) ++pass; else printf("  step107 FAIL line %d\n",__LINE__); } while(0)
    printf("Step-107 Segment-D front/logger regression ($EF04..$F10D):\n");

    ecm_reset();
    RAM8(0x0000u)=0x0Du; RAM8(0x001Bu)=1u;
    RAM8(0x0034u)=0u; RAM8(0x0033u)=0x10u;
    RAM8(0x003Fu)=0u; RAM8(0x0041u)=0xFFu;
    RAM8(0x003Cu)=0xFFu; RAM8(0x0042u)=0xAAu;
    RAM8(0x00A7u)=1u; RAM8(0x00ABu)=2u; RAM8(0x00AFu)=3u;
    RAM8(0x00B0u)=4u;
    route=bua_diag_front_step107();
    S107(route==DIAG107_ROUTE_LOG && RAM8(0x003Fu)==0x18u);
    S107(RAM8(0x0041u)==0x7Fu && RAM8(0x003Cu)==0xE3u &&
         RAM8(0x0042u)==0u);
    S107(RAM8(0x00A7u)==0u && RAM8(0x00ABu)==0u &&
         RAM8(0x00AFu)==0u && RAM8(0x00B0u)==0u);

    ecm_reset();
    RAM8(0x0000u)=0x1Du; RAM8(0x0034u)=0x80u;
    RAM8(0x0017u)=50u; RAM8(0x0005u)=1u; RAM8(0x0009u)=2u;
    route=bua_diag_front_step107();
    S107(route==DIAG107_ROUTE_QUALIFY_LOG &&
         (RAM8(0x003Fu)&0x02u)!=0u);
    S107(RAM8(0x0017u)==0u && RAM8(0x0005u)==0u && RAM8(0x0009u)==0u);
    S107(ram16be_get(0x0018u)==1u);

    ecm_reset();
    RAM8(0x0034u)=0x80u; RAM8(0x004Eu)=1u; RAM8(0x0053u)=1u;
    mpu16be_set(0x3FFCu,0x120Fu);
    route=bua_diag_front_step107();
    S107(route==DIAG107_ROUTE_COMPLETE && RAM8(0x0007u)==1u);
    S107(ram16be_get(0x0018u)==2u && mpu16be_get(0x3FFCu)==0xFB06u);
    S107(RAM8(0x004Eu)==0u && (RAM8(0x003Fu)&0x18u)==0u);

    ecm_reset();
    RAM8(0x0034u)=0x80u; RAM8(0x003Fu)=0x02u;
    RAM8(0x004Cu)=0xFFu; RAM8(0x004Du)=0xFFu;
    RAM8(0x004Eu)=0xA5u; RAM8(0x004Fu)=0xFFu; RAM8(0x0050u)=0xFFu;
    RAM8(0x00B1u)=2u; mpu16be_set(0x3FFCu,0x0000u);
    route=bua_diag_logger_step107();
    S107(route==DIAG107_ROUTE_COMPLETE);
    S107(RAM8(0x0051u)==0xF7u && RAM8(0x0052u)==0xCEu &&
         RAM8(0x0053u)==0xA5u && RAM8(0x0054u)==0xE0u &&
         RAM8(0x0055u)==0u);
    S107(RAM8(0x00B1u)==1u && mpu16be_get(0x3FFCu)==0xFB0Au);
    S107(RAM8(0x004Cu)==0u && RAM8(0x0050u)==0u);

    ecm_reset();
    RAM8(0x0034u)=0x80u; RAM8(0x003Fu)=0x02u;
    RAM8(0x0051u)=0x04u; RAM8(0x004Cu)=0x04u;
    RAM8(0x00B1u)=49u;
    route=bua_diag_logger_step107();
    S107(route==DIAG107_ROUTE_COMPLETE && RAM8(0x0005u)==0x04u);
    S107(RAM8(0x0051u)==0u && RAM8(0x00B1u)==100u &&
         (RAM8(0x0041u)&0x01u)!=0u);
    S107(ram16be_get(0x0018u)==5u && (RAM8(0x003Bu)&0x04u)!=0u);

    ecm_reset();
    RAM8(0x0034u)=0x80u; RAM8(0x0041u)=0x80u;
    RAM8(0x004Cu)=0x55u;
    route=bua_diag_logger_step107();
    S107(route==DIAG107_ROUTE_FLASH_PENDING && RAM8(0x004Cu)==0x55u);

    printf("  step-107 diagnostic-stage1 regression result: %s (%u/%u)\n",
           pass==total?"PASS":"FAIL",pass,total);
#undef S107
}
