static bua_u32 step109_diag_hash_byte(bua_u32 hash,bua_u8 value)
{
    hash^=(bua_u32)value;
    hash=(hash*16777619ul)&0xFFFFFFFFul;
    return hash;
}

static bua_u32 step109_diag_state_signature(void)
{
    bua_u32 hash;
    bua_u16 i;

    hash=2166136261ul;
    for(i=0u;i<0x0500u;++i)
        hash=step109_diag_hash_byte(hash,mem.low[i]);
    for(i=0u;i<0x0040u;++i)
        hash=step109_diag_hash_byte(hash,mem.mpu[i]);
    hash=step109_diag_hash_byte(hash,(bua_u8)stats.diagnostic_segment_calls);
    hash=step109_diag_hash_byte(hash,(bua_u8)stats.irq_ticks);
    return hash;
}

static void run_step109_diagnostic_integration_test(void)
{
    unsigned int pass=0u,total=0u;
    unsigned int i;
    bua_u8 route;
    bua_u32 signature;
#define S109(C) do { ++total; if(C) ++pass; else printf("  step109 FAIL line %d\n",__LINE__); } while(0)
    printf("Step-109 complete Segment-D/flash/scheduler regression:\n");

    ecm_reset();
    RAM8(0x0035u)=0x10u; RAM8(0x0034u)=0x80u;
    RAM8(0x0044u)=0u; RAM8(0x003Cu)=0u;
    route=bua_diag_front_step107();
    S109(route==DIAG107_ROUTE_QUALIFY_LOG && RAM8(0x003Cu)==0x44u);
    S109(RAM8(0x0042u)==0u && RAM8(0x00B1u)==0u &&
         (RAM8(0x0041u)&0x80u)!=0u);

    ecm_reset();
    RAM8(0x0035u)=0x10u; RAM8(0x0034u)=0x80u;
    RAM8(0x0044u)=0u; RAM8(0x003Cu)=0x40u;
    mpu16be_set(0x3FFCu,0x0008u);
    route=bua_diag_front_step107();
    S109(route==DIAG107_ROUTE_QUALIFY_LOG && RAM8(0x003Cu)==0x04u);
    S109(mpu16be_get(0x3FFCu)==0xFB02u);

    ecm_reset();
    RAM8(0x0035u)=0x10u; RAM8(0x0034u)=0x80u;
    RAM8(0x0044u)=0xC0u; RAM8(0x003Fu)=0x08u;
    mpu16be_set(0x3FFCu,0u);
    route=bua_diag_front_step107();
    S109(route==DIAG107_ROUTE_QUALIFY_LOG && RAM8(0x003Cu)==0x0Cu);
    S109(mpu16be_get(0x3FFCu)==0xFB02u);

    ecm_reset();
    mpu16be_set(0x3FFCu,0u);
    bua_diag_flash_step109();
    S109(RAM8(0x0042u)==0x42u && RAM8(0x00BBu)==2u &&
         RAM8(0x00B9u)==1u && RAM8(0x00BAu)==2u);
    S109(RAM8(0x00BDu)==0x1Cu && mpu16be_get(0x3FFCu)==0xFB0Au);

    RAM8(0x0042u)=0x42u; RAM8(0x00BDu)=2u;
    RAM8(0x004Cu)=0xFFu;
    bua_diag_flash_step109();
    S109(RAM8(0x00BDu)==1u && RAM8(0x0042u)==0x42u);
    S109(RAM8(0x004Cu)==0u);

    RAM8(0x0042u)=0x42u; RAM8(0x00BDu)=1u; RAM8(0x00B9u)=1u;
    mpu16be_set(0x3FFCu,0u);
    bua_diag_flash_step109();
    S109(RAM8(0x0042u)==0x52u && RAM8(0x00B9u)==0u &&
         RAM8(0x00BDu)==4u);
    S109(mpu16be_get(0x3FFCu)==0xFB02u);

    RAM8(0x0042u)=0x52u; RAM8(0x00BDu)=1u;
    bua_diag_flash_step109();
    S109(RAM8(0x0042u)==0x42u && RAM8(0x00BDu)==4u &&
         (mpu16be_get(0x3FFCu)&0x0008u)!=0u);

    RAM8(0x0042u)=0x42u; RAM8(0x00BDu)=1u; RAM8(0x00B9u)=0u;
    bua_diag_flash_step109();
    S109(RAM8(0x0042u)==0x62u && RAM8(0x00BDu)==8u);

    RAM8(0x0042u)=0x62u; RAM8(0x00BDu)=1u; RAM8(0x00BAu)=2u;
    bua_diag_flash_step109();
    S109(RAM8(0x0042u)==0x72u && RAM8(0x00BAu)==1u &&
         RAM8(0x00BDu)==4u);

    RAM8(0x0042u)=0x62u; RAM8(0x00BDu)=1u; RAM8(0x00BAu)=0u;
    RAM8(0x00BBu)=2u;
    bua_diag_flash_step109();
    S109(RAM8(0x0042u)==0x41u && RAM8(0x00B9u)==1u &&
         RAM8(0x00BAu)==2u && RAM8(0x00BDu)==0x1Cu);

    ecm_reset();
    RAM8(0x0042u)=0x60u; RAM8(0x00BDu)=1u;
    RAM8(0x00BBu)=2u; RAM8(0x00BCu)=0u; RAM8(0x0006u)=0x80u;
    bua_diag_flash_step109();
    S109(RAM8(0x0042u)==0x42u && RAM8(0x00BBu)==10u);
    S109(RAM8(0x00B9u)==2u && RAM8(0x00BAu)==4u);

    ecm_reset();
    RAM8(0x0042u)=0x60u; RAM8(0x00BDu)=1u;
    RAM8(0x00BBu)=36u; RAM8(0x00BCu)=0u;
    bua_diag_flash_step109();
    S109(RAM8(0x0042u)==0u && RAM8(0x00BDu)==37u);

    ecm_reset();
    sim_legacy_segment_d_freeze=1u;
    major_segment(0x0Du);
    S109(stats.major_segment_calls[0x0Du]==1ul &&
         stats.diagnostic_segment_calls==0ul);

    ecm_reset();
    sim_legacy_segment_d_freeze=0u;
    RAM8(0x0035u)=0x10u;
    major_segment(0x0Du);
    S109(stats.diagnostic_segment_calls==1ul && RAM8(0x0042u)==0x42u);

    ecm_reset();
    sim_legacy_segment_d_freeze=0u;
    sim_legacy_segment1_output_freeze=1u;
    sim_legacy_ignition_shutdown_freeze=1u;
    for(i=0u;i<16u;++i)
        irq_6p25ms();
    S109(stats.irq_ticks==16ul && stats.air_fuel_loops==8ul &&
         stats.spark_vss_loops==8ul && stats.iac_minor_services==16ul);
    S109(stats.diagnostic_segment_calls==1ul &&
         stats.major_segment_calls[0x0Du]==1ul);
    for(i=0u;i<16u;++i)
        S109(stats.major_segment_calls[i]==1ul);
    signature=step109_diag_state_signature();
    printf("  Step-109 integrated Segment-D signature: %08lX\n",
           (unsigned long)signature);
    S109(signature==0xF357A5F2ul);

    printf("  step-109 Segment-D integration regression result: %s (%u/%u)\n",
           pass==total?"PASS":"FAIL",pass,total);
#undef S109
}
