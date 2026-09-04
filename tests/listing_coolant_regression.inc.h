/* Step 106: zero-error listing resolution of LF42A and the COOLS8 spelling. */
static void run_step106_listing_coolant_test(void)
{
    unsigned int pass=0u,total=0u;
    BuaCool85 r;
    bua_u16 old_coolant;
#define S106(C) do { ++total; if(C) ++pass; else printf("  step106 FAIL line %d\\n",__LINE__); } while(0)
    printf("Step-106 listing-resolved coolant exits ($F3ED..$F42A):\n");

    /* $F3EE: L003B b3 branches to the real LF42A RTS after the lookup and
       range-state update, but before ERR14/15 and coolant filtering. */
    old_coolant=(bua_u16)0x6420u;
    r=bua_cool85_step(70u,10u,old_coolant,0u,0x08u,0u,0u,0u);
    S106(r.lookup_arg==80u && r.converted_5d==106u);
    S106(r.coolant_5b==old_coolant);
    S106(r.startup_cool_5f==100u);
    S106(r.mode3b==0x08u);
    S106(r.failure==0u);
    S106((r.mode30&0x01u)==0u);

    /* $F404..$F418: ERR14 stores the default coolant and returns at LF42A.
       It does not initialize the normal-path coolant filter or L005F. */
    old_coolant=(bua_u16)0x6320u;
    r=bua_cool85_step(0u,10u,old_coolant,0u,0u,0u,0u,0u);
    S106(r.failure==14u);
    S106(r.coolant_5b==(bua_u16)(STEP85_COOL_DEFAULT<<8));
    S106(r.startup_cool_5f==99u && (r.mode3b&0x10u)==0u);
    S106((r.minor35&0x02u)!=0u && (r.err4c&0x20u)!=0u);

    /* The low-temperature ERR15 branch reaches the same listing-proven RTS. */
    old_coolant=(bua_u16)0x5840u;
    r=bua_cool85_step(251u,10u,old_coolant,1u,0u,0u,0u,0u);
    S106(r.failure==15u &&
         r.coolant_5b==(bua_u16)(STEP85_COOL_DEFAULT<<8));
    S106(r.startup_cool_5f==88u &&
         (r.minor35&0x02u)!=0u && (r.err4c&0x10u)!=0u);

    printf("  step-106 listing-coolant regression result: %s (%u/%u)\n",
           pass==total?"PASS":"FAIL",pass,total);
#undef S106
}
