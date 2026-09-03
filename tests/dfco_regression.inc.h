static void run_step45_dfco_test(void)
{
    BuaDfcoState45 s;
    BuaDecelEnlean45 e;
    unsigned int i;
    unsigned int passed=0u;
    unsigned int total=18u;
    printf("\nStep-45 decel-enlean / DFCO state-machine regression:\n");
    memset(&e,0,sizeof(e));
    e=bua_decel_enlean_step45(e,255u,10u,255u,15u);
    if (e.active!=0u) ++passed;
    printf("  extreme simultaneous load/TPS drop can set decel-enlean             %s\n",e.active?"PASS":"FAIL");
    e=bua_decel_enlean_step45(e,200u,10u,200u,10u);
    if (e.active==0u) ++passed;
    printf("  LC5F6=0 removes decel-enlean on next nonqualifying pass             %s\n",(!e.active)?"PASS":"FAIL");
    memset(&s,0,sizeof(s)); s.qualify_count=STEP45_DFCO_ENTRY_COUNT;
    bua_dfco_tick_step45(&s,100u,0u,0u,30u,0u,0u,20u,60u,100u,100u);
    if (s.qualify_count==19u && (s.af_mode_word&STEP45_DFCO_BIT)==0u) ++passed;
    printf("  first valid DFCO pass decrements 20-count qualification timer       %s\n",
           (s.qualify_count==19u && (s.af_mode_word&STEP45_DFCO_BIT)==0u)?"PASS":"FAIL");
    for (i=0u;i<19u;++i)
        bua_dfco_tick_step45(&s,100u,0u,0u,30u,0u,0u,20u,60u,100u,100u);
    if (s.qualify_count==0u && (s.af_mode_word&STEP45_DFCO_BIT)==0u) ++passed;
    printf("  twentieth valid pass reaches zero but has not entered yet           %s\n",
           (s.qualify_count==0u && (s.af_mode_word&STEP45_DFCO_BIT)==0u)?"PASS":"FAIL");
    bua_dfco_tick_step45(&s,100u,0u,0u,30u,0u,0u,20u,60u,100u,100u);
    if ((s.af_mode_word&STEP45_DFCO_BIT)!=0u && s.lockout_count==511u) ++passed;
    printf("  next valid pass enters DFCO and loads LC608=511                     %s\n",
           ((s.af_mode_word&STEP45_DFCO_BIT)!=0u && s.lockout_count==511u)?"PASS":"FAIL");
    bua_dfco_tick_step45(&s,100u,0u,0u,30u,0u,0u,35u,40u,100u,100u);
    if ((s.af_mode_word&STEP45_DFCO_BIT)!=0u) ++passed;
    printf("  active DFCO stays on at load 35 and 1000 RPM via hysteresis         %s\n",
           (s.af_mode_word&STEP45_DFCO_BIT)?"PASS":"FAIL");
    bua_dfco_tick_step45(&s,100u,0u,0u,30u,0u,0u,41u,40u,100u,100u);
    if ((s.af_mode_word&STEP45_DFCO_BIT)==0u && s.qualify_count==20u) ++passed;
    printf("  load above stay-in threshold exits DFCO and reloads timer           %s\n",
           ((s.af_mode_word&STEP45_DFCO_BIT)==0u && s.qualify_count==20u)?"PASS":"FAIL");
    memset(&s,0,sizeof(s)); s.qualify_count=0u;
    bua_dfco_tick_step45(&s,100u,STEP45_PN_BIT,0u,30u,0u,0u,20u,60u,100u,100u);
    if ((s.af_mode_word&STEP45_DFCO_BIT)==0u) ++passed;
    printf("  Park/Neutral disqualifies DFCO                                     %s\n",
           ((s.af_mode_word&STEP45_DFCO_BIT)==0u)?"PASS":"FAIL");
    memset(&s,0,sizeof(s)); s.qualify_count=0u;
    bua_dfco_tick_step45(&s,100u,0u,0u,15u,0u,0u,20u,60u,100u,100u);
    if ((s.af_mode_word&STEP45_DFCO_BIT)==0u) ++passed;
    printf("  VSS exactly 15 with valid VSS disqualifies entry                    %s\n",
           ((s.af_mode_word&STEP45_DFCO_BIT)==0u)?"PASS":"FAIL");
    memset(&s,0,sizeof(s)); s.qualify_count=0u;
    bua_dfco_tick_step45(&s,100u,0u,STEP45_VSS_FAIL_BIT,0u,0u,0u,20u,60u,100u,100u);
    if ((s.af_mode_word&STEP45_DFCO_BIT)!=0u) ++passed;
    printf("  VSS-failure bit bypasses the normal speed gate                      %s\n",
           ((s.af_mode_word&STEP45_DFCO_BIT)!=0u)?"PASS":"FAIL");
    memset(&s,0,sizeof(s)); s.qualify_count=0u; s.lockout_count=300u;
    bua_dfco_tick_step45(&s,100u,0u,0u,30u,0u,9u,20u,60u,100u,100u);
    if (s.lockout_count==0u) ++passed;
    printf("  TPS above LC600 explicitly clears the re-entry lockout              %s\n",
           (s.lockout_count==0u)?"PASS":"FAIL");
    memset(&s,0,sizeof(s)); s.qualify_count=0u; s.lockout_count=2u;
    bua_dfco_tick_step45(&s,100u,0u,0u,30u,0u,0u,20u,60u,100u,100u);
    if (s.lockout_count==1u && (s.af_mode_word&STEP45_DFCO_BIT)==0u) ++passed;
    printf("  nonzero lockout prevents new DFCO and counts downward               %s\n",
           (s.lockout_count==1u && (s.af_mode_word&STEP45_DFCO_BIT)==0u)?"PASS":"FAIL");
    memset(&s,0,sizeof(s)); s.qualify_count=0u;
    bua_dfco_tick_step45(&s,100u,0u,0u,30u,0u,0u,20u,48u,100u,100u);
    if ((s.af_mode_word&STEP45_DFCO_BIT)==0u) ++passed;
    printf("  entry threshold requires RPM strictly above 1200                    %s\n",
           ((s.af_mode_word&STEP45_DFCO_BIT)==0u)?"PASS":"FAIL");
    memset(&s,0,sizeof(s)); s.qualify_count=0u;
    bua_dfco_tick_step45(&s,100u,0u,0u,30u,0u,0u,30u,60u,100u,100u);
    if ((s.af_mode_word&STEP45_DFCO_BIT)!=0u) ++passed;
    printf("  load exactly 30 is accepted for entry                               %s\n",
           ((s.af_mode_word&STEP45_DFCO_BIT)!=0u)?"PASS":"FAIL");
    memset(&s,0,sizeof(s)); s.qualify_count=0u;
    bua_dfco_tick_step45(&s,100u,0u,0u,30u,0u,0u,20u,60u,110u,103u);
    if ((s.af_mode_word&STEP45_DFCO_BIT)==0u) ++passed;
    printf("  RPM drop of exactly 7 raw counts disqualifies                       %s\n",
           ((s.af_mode_word&STEP45_DFCO_BIT)==0u)?"PASS":"FAIL");
    memset(&s,0,sizeof(s)); s.qualify_count=0u;
    bua_dfco_tick_step45(&s,100u,0u,0u,30u,0u,0u,20u,60u,106u,100u);
    if ((s.af_mode_word&STEP45_DFCO_BIT)!=0u) ++passed;
    printf("  RPM drop below 7 raw counts remains eligible                        %s\n",
           ((s.af_mode_word&STEP45_DFCO_BIT)!=0u)?"PASS":"FAIL");
    memset(&s,0,sizeof(s)); s.lockout_count=2u;
    bua_dfco_disqualify_step45(&s,36u,100u,100u);
    if ((s.mode3d&STEP45_STALL_SAVE_BIT)!=0u && s.stall_ae_pulses==1u && s.stall_tf_count==30u) ++passed;
    printf("  <=900-RPM exit arms one AE pulse and 30-count follower support      %s\n",
           ((s.mode3d&STEP45_STALL_SAVE_BIT)!=0u && s.stall_ae_pulses==1u && s.stall_tf_count==30u)?"PASS":"FAIL");
    memset(&s,0,sizeof(s)); s.lockout_count=2u;
    bua_dfco_disqualify_step45(&s,50u,110u,100u);
    if ((s.mode3d&STEP45_STALL_SAVE_BIT)!=0u) ++passed;
    printf("  rapid RPM drop during exit also arms stall saver                    %s\n",
           ((s.mode3d&STEP45_STALL_SAVE_BIT)!=0u)?"PASS":"FAIL");
    printf("  step-45 regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
}
