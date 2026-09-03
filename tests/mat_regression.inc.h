static void run_step84_mat_test(void)
{
    unsigned int pass=0u,total=0u;
    BuaMat84 r;
#define S84(C) do { ++total; if(C) ++pass; else printf("  step84 FAIL line %d\\n",__LINE__); } while(0)
    printf("Step-84 segment-A MAT producer / ERR23/25 regression:\n");
    r=bua_mat84_step(0x55u,0u,0u,7u,8u,0u,0u,0u,0u);
    S84(r.ad_inverted==0xAAu && r.mat==0xAAu);
    S84(r.low_timer==7u && r.high_timer==8u); /* diagnostic delay */
    r=bua_mat84_step(0xFBu,241u,0u,0u,0u,0u,0u,0u,0u); /* inverted=4 */
    S84(r.mat==4u && r.low_timer==0u); /* equality does not low-fail */
    r=bua_mat84_step(0xFCu,241u,0u,0u,0u,0u,0u,0u,0u); /* inverted=3 */
    S84(r.low_timer==1u);
    r=bua_mat84_step(0xFCu,241u,1u,120u,0u,0u,0u,0u,0u);
    S84(r.low_timer==121u && (r.af_mode43&0x10u)==0u);
    r=bua_mat84_step(0xFCu,241u,1u,121u,0u,0u,0u,0u,0u);
    S84((r.af_mode43&0x10u)!=0u && r.mat==29u);
    S84((r.err23_latched4c&0x01u)!=0u);
    r=bua_mat84_step(0xFCu,241u,2u,50u,0u,0u,0u,0u,0u);
    S84(r.low_timer==0u); /* executable ERR23 VSS sense */
    r=bua_mat84_step(0x0Cu,241u,2u,0u,0u,0u,0u,0u,0u); /* inverted=243 */
    S84(r.high_timer==1u); /* equality enters high path */
    r=bua_mat84_step(0x0Cu,241u,1u,0u,44u,0u,0u,0u,0u);
    S84(r.high_timer==0u); /* ERR25 requires VSS > 1 */
    r=bua_mat84_step(0x0Cu,241u,2u,0u,120u,0u,0u,0u,0u);
    S84(r.high_timer==121u && (r.diag_mode40&0x02u)==0u);
    r=bua_mat84_step(0x0Cu,241u,2u,0u,121u,0u,0u,0u,0u);
    S84((r.diag_mode40&0x02u)!=0u && r.mat==29u);
    S84((r.err25_latched4d&0x40u)!=0u);
    r=bua_mat84_step(0x0Du,241u,2u,0u,55u,0u,0x02u,0u,0u); /* inverted=242 */
    S84(r.high_timer==0u && (r.diag_mode40&0x02u)==0u);
    r=bua_mat84_step(0xFCu,240u,0u,119u,119u,0u,0u,0u,0u);
    S84(r.low_timer==119u && r.high_timer==119u); /* delay equality */
    r=bua_mat84_step(0xFCu,241u,0u,121u,0u,0u,0x02u,0u,0x40u);
    S84((r.err23_latched4c&1u)!=0u && r.mat==29u);
    S84((r.err25_latched4d&0x40u)!=0u); /* existing latched bits preserved */
    S84(STEP84_MAT_LOW_THRESH==4u && STEP84_MAT_HIGH_THRESH==243u);
    S84(STEP84_MAT_LOW_TIME==120u && STEP84_MAT_HIGH_TIME==120u);
    S84(STEP84_MAT_DIAG_DELAY==240u && STEP84_MAT_DEFAULT==29u);
    printf("  result: %s (%u/%u)\n",pass==total?"PASS":"FAIL",pass,total);
#undef S84
}


