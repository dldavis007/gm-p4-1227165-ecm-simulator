static void run_step85_coolant_test(void)
{
    unsigned int pass=0u,total=0u;
    BuaCool85 r;
#define S85(C) do { ++total; if(C) ++pass; else printf("  step85 FAIL line %d\\n",__LINE__); } while(0)
    printf("Step-85 coolant producer / ERR14-15 regression:\n");
    r=bua_cool85_step(0u,10u,0u,0u,0u,0u,0u,0u);
    S85(r.used_348==0u && r.lookup_arg==10u);
    S85(r.converted_5d==230u); /* interpolation 255->215 at arg 10 */
    S85((r.mode30&1u)!=0u);    /* >120 selects 348-ohm next pass */
    S85(r.failure==14u && r.coolant_5b==(bua_u16)(135u<<8));
    S85((r.minor35&2u)!=0u && (r.err4c&0x20u)!=0u);

    r=bua_cool85_step(70u,10u,(bua_u16)(100u<<8),0u,0u,0u,0u,0u);
    S85(r.lookup_arg==80u && r.converted_5d==106u);
    S85((r.mode30&1u)==0u); /* equality 106 selects 3840-ohm */
    S85(r.failure==0u && r.coolant_5b==(bua_u16)(106u<<8));
    S85((r.mode3b&0x10u)!=0u && r.startup_cool_5f==106u);

    r=bua_cool85_step(112u,10u,(bua_u16)(100u<<8),1u,0u,0u,0u,0u);
    S85(r.used_348==1u && r.lookup_arg==112u && r.converted_5d==168u);
    S85((r.mode30&1u)!=0u); /* >120 remains 348-ohm */

    r=bua_cool85_step(208u,10u,(bua_u16)(100u<<8),1u,0u,0u,0u,0u);
    S85(r.converted_5d==105u && (r.mode30&1u)==0u); /* <=106 returns to 4k */

    r=bua_cool85_step(200u,10u,(bua_u16)(100u<<8),0u,0x10u,0u,0u,0u);
    S85(r.converted_5d==43u && r.failure==0u);
    S85(r.coolant_5b==(bua_u16)24688u); /* LF250 coef16: 100.0 -> 43.0 = 96.4375 */
    S85(r.startup_cool_5f==96u);

    r=bua_cool85_step(251u,1u,0u,1u,0u,0u,0u,0u);
    S85(r.failure==15u && r.coolant_5b==(bua_u16)(135u<<8));
    S85((r.err4c&0x10u)!=0u && (r.minor35&2u)!=0u);
    r=bua_cool85_step(251u,0u,0u,1u,0u,0u,0u,0u);
    S85(r.failure==15u && r.err4c==0u && r.minor35==0u); /* default even before latch time */

    r=bua_cool85_step(250u,1u,0u,1u,0u,0u,0u,0u);
    S85(r.failure!=15u); /* equality does not ERR15 */
    r=bua_cool85_step(245u,1u,0u,0u,0u,0u,0u,0u);
    S85(r.lookup_arg==255u); /* +10 no overflow */
    r=bua_cool85_step(246u,1u,0u,0u,0u,0u,0u,0u);
    S85(r.lookup_arg==255u); /* +10 overflow saturates */
    S85(STEP85_ERR14_HIGH==227u && STEP85_ERR15_RAW_HIGH==250u);
    S85(STEP85_COOL_DEFAULT==135u && STEP85_COOL_FILTER_COEF==16u);
    printf("  result: %s (%u/%u)\n",pass==total?"PASS":"FAIL",pass,total);
#undef S85
}


