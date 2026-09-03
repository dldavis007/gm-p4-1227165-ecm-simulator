static void run_step81_air_test(void)
{
    BuaAir81 s;
    unsigned int passed=0u,total=25u,i;
#define S81(c) do { if(c) ++passed; printf("  %-84s %s\n",#c,(c)?"PASS":"FAIL"); } while(0)
#define Z81() do { memset(&s,0,sizeof(s)); } while(0)
    printf("\nStep-81 injection-air LE34E..LE432 regression:\n");
    S81(STEP81_AIR_PE_COUNT==0u && STEP81_AIR_VALVE_CAL==4u);
    S81(STEP81_AIR_O2_TIME==200u && STEP81_AIR_OPEN_DELAY==254u);

    Z81(); bua_air81(&s,72u,0u,0x80u,100u,50u,100u,30u);
    S81((s.air3e&0x06u)==0u); /* coolant below C30B diverts */
    Z81(); bua_air81(&s,73u,0x04u,0x80u,100u,50u,100u,30u);
    S81((s.air3e&0x06u)==0u); /* mode3b b2 also diverts */

    Z81(); s.af44=0x80u; bua_air81(&s,100u,0u,0x80u,100u,50u,100u,30u);
    S81((s.history3d&0x80u)!=0u && s.open_fd==254u);
    S81((s.air3e&0x06u)==0x02u); /* actual LC015 -> LE41E b1 only */

    /* Once closed loop has occurred, L003D b7 remains latched in this routine. */
    s.af44=0u; bua_air81(&s,100u,0u,0u,100u,50u,100u,30u);
    S81((s.history3d&0x80u)!=0u && s.open_fd==253u);

    /* Before any closed-loop history, the ordinary path sets both bits. */
    Z81(); bua_air81(&s,100u,0u,0u,100u,50u,100u,30u);
    S81((s.air3e&0x06u)==0x06u);

    /* Actual LC2FF=0 makes PE divert immediately from the normal pe_fe=0 state. */
    Z81(); s.af43=0x20u; bua_air81(&s,100u,0u,0u,100u,50u,100u,30u);
    S81((s.air3e&0x06u)==0u && s.pe_fe==0u);

    /* RPM counter: 11 qualifying calls store 11; the next call diverts. */
    Z81(); for(i=0u;i<11u;++i) bua_air81(&s,100u,0u,0u,161u,50u,100u,30u);
    S81(s.rpm_f6==11u && (s.air3e&0x06u)==0x06u);
    bua_air81(&s,100u,0u,0u,161u,50u,100u,30u);
    S81((s.air3e&0x06u)==0u);
    bua_air81(&s,100u,0u,0u,160u,50u,100u,30u);
    S81(s.rpm_f6==0u); /* equality is not high-RPM */

    /* Rich O2 counter asserts AF43 b0 at x>=200 on the next rich evaluation. */
    Z81(); for(i=0u;i<200u;++i) bua_air81(&s,100u,0u,0u,100u,50u,171u,30u);
    S81(s.rich_f7==200u && (s.af43&0x01u)==0u);
    bua_air81(&s,100u,0u,0u,100u,50u,171u,30u);
    S81((s.af43&0x01u)!=0u && (s.air3e&0x06u)==0u);
    bua_air81(&s,100u,0u,0u,100u,50u,170u,30u);
    S81(s.rich_f7==199u && (s.af43&0x01u)!=0u);

    /* Lean closed-loop timer uses BHI, so x=200 increments to 201; x=201 diverts. */
    Z81(); s.af44=0x80u; for(i=0u;i<201u;++i) bua_air81(&s,100u,0u,0x80u,100u,50u,55u,30u);
    S81(s.lean_f9==201u);
    bua_air81(&s,100u,0u,0x80u,100u,50u,55u,30u);
    S81((s.air3e&0x06u)==0u && s.lean_f9==201u);
    bua_air81(&s,100u,0u,0x80u,100u,50u,56u,30u);
    S81(s.lean_f9==0u); /* equality resets lean timer */

    Z81(); bua_air81(&s,100u,0u,0u,100u,24u,100u,30u);
    S81((s.air3e&0x06u)==0u);
    Z81(); bua_air81(&s,100u,0u,0u,100u,25u,100u,30u);
    S81((s.air3e&0x06u)==0x06u); /* low-load equality allowed */
    Z81(); bua_air81(&s,100u,0u,0u,100u,101u,100u,61u);
    S81((s.air3e&0x06u)==0u);
    Z81(); bua_air81(&s,100u,0u,0u,100u,100u,100u,61u);
    S81((s.air3e&0x06u)==0x06u); /* load equality allowed */
    Z81(); bua_air81(&s,100u,0u,0u,100u,101u,100u,60u);
    S81((s.air3e&0x06u)==0x06u); /* VSS equality allowed */

    /* >64 load drop loads 10, then immediately decrements to 9 and takes LE41E. */
    Z81(); s.old_load_fb=120u; bua_air81(&s,100u,0u,0x80u,100u,50u,100u,30u);
    S81(s.drop_fc==9u && (s.air3e&0x06u)==0x02u);
    S81(s.old_load_fb==50u);

    printf("Step-81 injection-air regression: %s (%u/%u)\n",passed==total?"PASS":"FAIL",passed,total);
#undef Z81
#undef S81
}
