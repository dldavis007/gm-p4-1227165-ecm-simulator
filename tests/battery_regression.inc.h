static void run_step86_battery_test(void)
{
    unsigned int pass=0u,total=0u;
    BuaBatt86 r;
#define S86(C) do { ++total; if(C) ++pass; else printf("  step86 FAIL line %d\\n",__LINE__); } while(0)
    printf("Step-86 battery producer / high-voltage gate regression:\n");

    r=bua_battery86_segment_e(90u,0x10u,0x40u,0x1234u,0x9999u,7u,1u,2u,3u,4u);
    S86(r.battery_7e==90u && (r.mode33&0x10u)==0u);
    S86((r.major3e&0x40u)==0u);
    S86(r.old_spark_b2==0x1234u && r.spark_fb_b4==0u);
    S86(r.burnoff_time_b7==10u && r.burnoff_delay_b8==50u);
    S86(r.burn_diag_11f==0u && r.burn_diag_120==0u && r.shifter_read==1u);

    r=bua_battery86_segment_e(89u,0x10u,0x40u,0x1111u,0x2222u,9u,6u,7u,8u,9u);
    S86((r.mode33&0x10u)!=0u && (r.major3e&0x40u)!=0u);
    S86(r.old_spark_b2==0x2222u && r.spark_fb_b4==9u);
    S86(r.burnoff_time_b7==6u && r.burnoff_delay_b8==7u && r.shifter_read==0u);

    r=bua_battery86_segment_e(40u,0u,0u,0u,0u,0u,0u,0u,0u,0u);
    S86((r.mode33&0x10u)==0u); /* equality 40 is dead band, not OFF */
    r=bua_battery86_segment_e(39u,0u,0u,0u,0u,0u,0u,0u,0u,0u);
    S86((r.mode33&0x10u)!=0u);
    r=bua_battery86_segment_e(40u,0x10u,0u,0u,0u,0u,0u,0u,0u,0u);
    S86((r.mode33&0x10u)!=0u); /* dead band retains prior state */
    r=bua_battery86_segment_e(89u,0u,0u,0u,0u,0u,0u,0u,0u,0u);
    S86((r.mode33&0x10u)==0u); /* dead band also retains ON */

    r=bua_battery86_seg1_gate(170u,0x40u,0x00u,0x00u);
    S86((r.minor35&0x40u)==0u && r.force_discretes_off==0u);
    r=bua_battery86_seg1_gate(171u,0x00u,0x00u,0x00u);
    S86((r.minor35&0x40u)!=0u && (r.major3e&0x10u)!=0u);
    S86(r.force_discretes_off==0u && (r.err4f&0x40u)==0u);
    r=bua_battery86_seg1_gate(171u,r.minor35,r.major3e,r.err4f);
    S86(r.force_discretes_off==1u && (r.err4f&0x40u)!=0u);
    S86((r.major3e&0x10u)!=0u);

    r=bua_battery86_seg1_gate(170u,0x40u,0x10u,0x40u);
    S86((r.minor35&0x40u)==0u);
    S86((r.major3e&0x10u)!=0u && (r.err4f&0x40u)!=0u); /* latched elsewhere */
    r=bua_battery86_seg1_gate(171u,r.minor35,r.major3e,r.err4f);
    S86(r.force_discretes_off==0u && (r.minor35&0x40u)!=0u);

    S86(STEP86_BATT_OFF_RAW==40u && STEP86_BATT_ON_RAW==90u);
    S86(STEP86_BATT_HIGH_RAW==171u);
    S86(STEP86_BURNOFF_TIME==10u && STEP86_BURNOFF_DELAY==50u);
    printf("  result: %s (%u/%u)\n",pass==total?"PASS":"FAIL",pass,total);
#undef S86
}


