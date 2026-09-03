static void run_step83_purge_test(void)
{
    BuaPurge83 s; unsigned int passed=0u,total=24u;
#define Z83() memset(&s,0,sizeof(s))
#define S83(c) do{if(c)++passed; printf("  %-84s %s\n",#c,(c)?"PASS":"FAIL");}while(0)
    printf("\nStep-83 canister-purge LE75D..LE816 regression:\n");
    S83(bua_ccp_maf83(0x0000u)==16u);
    S83(bua_ccp_maf83(0x0400u)==32u);
    S83(bua_ccp_maf83(0x0800u)==48u);
    S83(bua_ccp_maf83(0x0C00u)==80u);
    S83(bua_ccp_maf83(0x1000u)==160u);
    S83(bua_ccp_maf83(0x1800u)==255u && bua_ccp_maf83(0x2000u)==255u);
    Z83(); bua_purge83(&s,80u,0u,200u,0u,0x80u,0u,0u,30u,50u,0x1000u,100u); S83(s.pw113==0u&&!s.runtime_latched);
    Z83(); bua_purge83(&s,81u,0u,200u,0u,0x80u,0u,0u,30u,50u,0x1000u,100u); S83(s.runtime_latched&&s.pw113>0u);
    bua_purge83(&s,0u,0u,200u,0u,0x80u,0u,0u,30u,50u,0x1000u,100u); S83(s.runtime_latched&&s.pw113>0u);
    Z83(); s.runtime_latched=1u; bua_purge83(&s,0u,0x20u,200u,0u,0x80u,0u,0u,30u,50u,0x1000u,100u); S83(s.pw113==0u);
    Z83(); s.runtime_latched=1u; bua_purge83(&s,0u,0u,146u,0u,0x80u,0u,0u,30u,50u,0x1000u,100u); S83(s.pw113==0u);
    Z83(); s.runtime_latched=1u; bua_purge83(&s,0u,0u,147u,0u,0x80u,0u,0u,30u,50u,0x1000u,100u); S83(s.pw113>0u);
    Z83(); s.runtime_latched=1u; bua_purge83(&s,0u,0u,200u,0x02u,0x80u,0u,0u,30u,50u,0x1000u,100u); S83(s.pw113==0u);
    Z83(); s.runtime_latched=1u; bua_purge83(&s,0u,0u,200u,0u,0u,0u,0x20u,30u,50u,0x1000u,100u); S83(s.pw113==0u);
    Z83(); s.runtime_latched=1u; bua_purge83(&s,0u,0u,200u,0u,0u,0x02u,0x20u,30u,50u,0x1000u,100u); S83(s.pw113>0u);
    Z83(); s.runtime_latched=1u; bua_purge83(&s,0u,0u,200u,0u,0u,0u,0u,16u,50u,0x1000u,100u); S83(s.pw113==0u);
    Z83(); s.runtime_latched=1u; bua_purge83(&s,0u,0u,200u,0u,0u,0u,0u,17u,10u,0x1000u,100u); S83(s.pw113==0u); /* TPS equality off */
    Z83(); s.runtime_latched=1u; bua_purge83(&s,0u,0u,200u,0u,0u,0u,0u,17u,11u,0x1000u,100u); S83(s.pw113>0u);
    Z83(); s.runtime_latched=1u; s.status3b=1u; bua_purge83(&s,0u,0u,200u,0u,0u,0u,0u,10u,6u,0x1000u,100u); S83(s.pw113==0u); /* VSS hold equality */
    Z83(); s.runtime_latched=1u; s.status3b=1u; bua_purge83(&s,0u,0u,200u,0u,0u,0u,0u,11u,5u,0x1000u,100u); S83(s.pw113==0u); /* TPS hold equality */
    Z83(); s.runtime_latched=1u; s.status3b=1u; bua_purge83(&s,0u,0u,200u,0u,0u,0u,0u,11u,6u,0x1000u,100u); S83(s.pw113>0u);
    Z83(); s.runtime_latched=1u; bua_purge83(&s,0u,0u,200u,0u,0u,0u,0u,30u,50u,0x1000u,0u); S83(s.requested==160u); /* C27C load gain unity */
    S83(s.pw113==20u && s.min114==160u && (s.status3b&1u)!=0u); /* coef32 first filter step */
    bua_purge83(&s,0u,0u,200u,0u,0u,0u,0u,30u,50u,0x1000u,255u); S83(s.requested==160u); /* load still no effect */
    printf("Step-83 canister-purge regression: %s (%u/%u)\n",passed==total?"PASS":"FAIL",passed,total);
#undef S83
#undef Z83
}

