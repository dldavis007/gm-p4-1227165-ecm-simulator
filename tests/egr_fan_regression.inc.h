static void run_step82_egr_test(void)
{
    BuaEgr82Result r;
    unsigned int passed=0u,total=22u;
#define S82(c) do { if(c) ++passed; printf("  %-84s %s\n",#c,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-82 EGR LDEE5..LDF49 regression:\n");
    S82(STEP82_EGR_TPS_ON==8u && STEP82_EGR_TPS_HOLD==5u);
    S82(bua_egr_cool_mult82(64u)==0u && bua_egr_cool_mult82(112u)==0u);
    S82(bua_egr_cool_mult82(120u)==64u && bua_egr_cool_mult82(128u)==128u);
    S82(bua_egr_cool_mult82(160u)==128u && bua_egr_cool_mult82(255u)==128u);

    r=bua_egr82(1u,0u,100u,100u,0u,60u,80u,160u,0u);
    S82(r.pw112==0u); /* P/N inhibits outside diagnostic mode */
    r=bua_egr82(1u,0x20u,100u,100u,0u,60u,80u,160u,0u);
    S82(r.pw112==255u); /* ALDL diagnostic mode bypasses that inhibit */
    r=bua_egr82(0u,0u,29u,100u,0u,60u,80u,160u,0u);
    S82(r.pw112==0u);
    r=bua_egr82(0u,0u,30u,8u,0u,60u,80u,160u,0u);
    S82(r.pw112==255u); /* MAT and TPS equality qualify */
    r=bua_egr82(0u,0u,30u,7u,0u,60u,80u,160u,0u);
    S82(r.pw112==0u); /* zero-PW turn-on threshold is 8 */
    r=bua_egr82(0u,0u,30u,5u,0u,60u,80u,160u,1u);
    S82(r.pw112==255u); /* nonzero prior PW uses hold threshold 5 */
    r=bua_egr82(0u,0u,30u,4u,0u,60u,80u,160u,1u);
    S82(r.pw112==0u);
    r=bua_egr82(0u,0u,100u,100u,0x20u,60u,80u,160u,0u);
    S82(r.pw112==0u); /* PE inhibits */

    r=bua_egr82(0u,0u,100u,100u,0u,100u,200u,160u,0u);
    S82(r.rpm_used==80u && r.load_used==160u);
    S82(r.base_duty==255u && r.cool_mult==128u && r.pw112==255u);
    r=bua_egr82(0u,0u,100u,100u,0u,60u,80u,112u,0u);
    S82(r.pw112==0u);
    r=bua_egr82(0u,0u,100u,100u,0u,60u,80u,120u,0u);
    S82(r.cool_mult==64u && r.pw112==127u);
    r=bua_egr82(0u,0u,100u,100u,0u,60u,80u,127u,0u);
    S82(r.cool_mult==120u && r.pw112==239u);
    r=bua_egr82(0u,0u,100u,100u,0u,60u,80u,128u,0u);
    S82(r.cool_mult==128u && r.pw112==255u);
    r=bua_egr82(0u,0u,100u,100u,0u,60u,80u,255u,0u);
    S82(r.pw112==255u);

    /* Calibrated C23B collapse means ordinary RPM/load variation cannot change base duty. */
    r=bua_egr82(0u,0u,100u,100u,0u,32u,32u,160u,0u);
    S82(r.base_duty==255u && r.pw112==255u);
    r=bua_egr82(0u,0u,100u,100u,0u,79u,159u,160u,0u);
    S82(r.base_duty==255u && r.pw112==255u);
    S82(STEP82_EGR_RPM_MAX==80u && STEP82_EGR_LOAD_MAX==160u);
    printf("Step-82 EGR regression: %s (%u/%u)\n",passed==total?"PASS":"FAIL",passed,total);
#undef S82
}

static void run_step79_fan_test(void)
{
    unsigned int passed=0u,total=14u,i; bua_u8 t,d;
#define S79(x) do{if(x)++passed;}while(0)
    t=0u; d=bua_fan79(1u,80u,0xC0u,0u,0u,&t); S79(d==255u); S79(t==75u);
    t=0u; d=bua_fan79(0u,34u,0x80u,0u,0u,&t); S79(d==255u);
    t=0u; d=bua_fan79(0u,35u,0x80u,0u,0u,&t); S79(d==0u);
    t=0u; d=bua_fan79(0u,11u,0xC0u,197u,0u,&t); S79(d==255u);
    t=0u; d=bua_fan79(0u,12u,0xC0u,206u,0u,&t); S79(d==0u);
    t=0u; d=bua_fan79(0u,12u,0xC0u,207u,0u,&t); S79(d==255u);
    t=0u; d=bua_fan79(0u,12u,0xC0u,205u,255u,&t); S79(d==255u);
    t=0u; d=bua_fan79(0u,12u,0xC0u,199u,255u,&t); S79(d==0u);
    t=0u; d=bua_fan79(0u,50u,0x40u,197u,0u,&t); S79(d==255u);
    d=bua_fan79(0u,50u,0xC0u,0u,d,&t); S79(d==255u&&t==74u);
    for(i=0u;i<74u;++i) d=bua_fan79(0u,50u,0xC0u,0u,d,&t);
    S79(d==255u&&t==0u); d=bua_fan79(0u,50u,0xC0u,0u,d,&t); S79(d==0u);
    t=0u; d=bua_fan79(0u,12u,0xC0u,200u,0u,&t); S79(d==0u);
    printf("Step-79 cooling-fan regression: %s (%u/%u)\n",passed==total?"PASS":"FAIL",passed,total);
#undef S79
}



