/* ---------------------------------------------------------------------- */
/* Step 57: close the reconstructed IAC controller around a PC idle plant.*/
/*                                                                        */
/* Source-derived side: Step-36 coolant target and Step-37 PID/motor-step */
/* request.  Simulator-only side: coolant warm-up, IAC-step-to-air/RPM    */
/* gain, engine first-order response, and 100-ms control invocation.       */
/* These plant constants are intentionally not presented as 9340 values. */
/* ---------------------------------------------------------------------- */
#define STEP57_CONTROL_DT_SEC       0.100
#define STEP57_PLANT_BASE_RPM       430.0
#define STEP57_PLANT_RPM_PER_STEP     5.0
#define STEP57_PLANT_TAU_SEC          1.20
#define STEP57_WARMUP_SEC            90.0
static const bua_u8 step57_warm_park[9] = {
    145u,145u,113u,90u,80u,65u,50u,50u,50u
};
typedef struct {
    double seconds;
    double rpm;
    bua_u8 coolant;
    bua_u8 iac_position;
    int integral;
    unsigned int target_rpm;
    unsigned long control_ticks;
    unsigned long open_steps;
    unsigned long close_steps;
    unsigned int max_abs_error;
} BuaClosedIdle57;
static bua_u8 bua_coolant_warmup_step57(double sec)
{
    double v;
    if (sec<=0.0) return 96u;       /* 32 C test starting point */
    if (sec>=STEP57_WARMUP_SEC) return 160u; /* 80 C */
    v=96.0+(64.0*sec/STEP57_WARMUP_SEC);
    return (bua_u8)(v+0.5);
}
static bua_u8 bua_warm_park_step57(bua_u8 coolant)
{
    unsigned int idx=(unsigned int)(coolant>>5);
    unsigned int frac=(unsigned int)(coolant&31u);
    unsigned int a,b;
    if (idx>=8u) return step57_warm_park[8];
    a=step57_warm_park[idx]; b=step57_warm_park[idx+1u];
    if (b>=a) return (bua_u8)(a+((b-a)*frac)/32u);
    return (bua_u8)(a-((a-b)*frac)/32u);
}
static void bua_closed_idle_init_step57(BuaClosedIdle57 *s)
{
    BuaIacStartupStep41 park;
    bua_u8 warm=bua_warm_park_step57(96u);
    park=bua_iac_startup_position_step41(128u,warm,1u,warm,0u,160u);
    s->seconds=0.0;
    s->coolant=96u;
    s->iac_position=park.target_position;
    s->rpm=STEP57_PLANT_BASE_RPM+STEP57_PLANT_RPM_PER_STEP*(double)s->iac_position;
    s->integral=0;
    s->target_rpm=0u;
    s->control_ticks=0ul;
    s->open_steps=0ul;
    s->close_steps=0ul;
    s->max_abs_error=0u;
}
static void bua_closed_idle_tick_step57(BuaClosedIdle57 *s)
{
    BuaIacCommandStep36 cmd;
    BuaIacPidStep37 pid;
    unsigned int actual_raw;
    unsigned int n;
    double equilibrium;
    double alpha;
    int err;
    s->coolant=bua_coolant_warmup_step57(s->seconds);
    cmd=bua_iac_command_step36(s->coolant,STEP36_IAC_PN_BIT,0u,0u,0u);
    s->target_rpm=cmd.command_rpm;
    actual_raw=(unsigned int)(s->rpm/12.5+0.5);
    if (actual_raw>255u) actual_raw=255u;
    pid=bua_iac_pid_step37(cmd.command_raw,(bua_u8)actual_raw,0,1u,s->coolant,s->integral);
    s->integral=pid.integral_after;
    n=(unsigned int)pid.motor_steps;
    /* Step 40 consumes a packed request one physical step at a time.
       The PC integration therefore executes one motor step on this 100-ms
       service boundary instead of applying the entire PID magnitude at once. */
    if (n!=0u) {
        if (pid.direction_open) {
            if (s->iac_position<145u) ++s->iac_position;
            ++s->open_steps;
        } else {
            if (s->iac_position>0u) --s->iac_position;
            ++s->close_steps;
        }
    }
    equilibrium=STEP57_PLANT_BASE_RPM+STEP57_PLANT_RPM_PER_STEP*(double)s->iac_position;
    alpha=STEP57_CONTROL_DT_SEC/STEP57_PLANT_TAU_SEC;
    s->rpm += (equilibrium-s->rpm)*alpha;
    if (s->rpm<0.0) s->rpm=0.0;
    err=(int)(s->rpm+0.5)-(int)s->target_rpm;
    if (err<0) err=-err;
    if ((unsigned int)err>s->max_abs_error) s->max_abs_error=(unsigned int)err;
    s->seconds+=STEP57_CONTROL_DT_SEC;
    ++s->control_ticks;
}
static void run_step57_closed_iac_plant_test(void)
{
    BuaClosedIdle57 s;
    unsigned int passed=0u,total=15u;
    unsigned int i;
    unsigned int initial_pos;
    double initial_rpm;
#define STEP57_CHECK(c,tmsg) do { if(c) ++passed; printf("  %-78s %s\n",tmsg,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-57 closed-loop IAC/PC-engine plant regression:\n");
    bua_closed_idle_init_step57(&s);
    initial_pos=(unsigned int)s.iac_position;
    initial_rpm=s.rpm;
    STEP57_CHECK(bua_warm_park_step57(96u)==90u,
                 "C690 interpolation gives warm-park 90 steps at the 32C test point");
    STEP57_CHECK(initial_pos==122u,
                 "Step-41 startup positioning adds unconditional LC630=32 -> initial 122 steps");
    STEP57_CHECK(initial_rpm>1039.0 && initial_rpm<1041.0,
                 "simulator plant maps initial 122-step IAC position to about 1040 RPM");
    for(i=0u;i<900u;++i) bua_closed_idle_tick_step57(&s);
    STEP57_CHECK(s.control_ticks==900ul && s.seconds>89.99 && s.seconds<90.01,
                 "100-ms closed-loop controller executes 900 times across 90 simulated seconds");
    STEP57_CHECK(s.coolant>=159u,
                 "simulator-only coolant trajectory reaches the warm endpoint near 80C");
    STEP57_CHECK(s.target_rpm==600u,
                 "source-derived Step-36 warm Park/Neutral target reaches 600 RPM");
    STEP57_CHECK(s.iac_position<initial_pos,
                 "controller closes IAC from its startup position as commanded idle falls");
    STEP57_CHECK(s.close_steps>0ul,
                 "reconstructed PID produces actual closing motor-step demand");
    STEP57_CHECK(s.rpm<540.0,
                 "first closed-loop integration exposes undershoot with the isolated Step-37 helper");
    STEP57_CHECK((s.rpm>(double)s.target_rpm?s.rpm-(double)s.target_rpm:(double)s.target_rpm-s.rpm)>80.0,
                 "isolated PID helper does not yet reproduce a stable complete-source idle loop");
    STEP57_CHECK(s.iac_position==0u,
                 "diagnostic integration reaches the closed-IAC boundary, proving missing state matters");
    STEP57_CHECK(s.integral>=-127 && s.integral<=127,
                 "IAC integral state remains inside the translated signed saturation bounds");
    STEP57_CHECK(bua_coolant_warmup_step57(0.0)==96u && bua_coolant_warmup_step57(90.0)==160u,
                 "plant coolant endpoints remain explicitly 32C raw96 to 80C raw160");
    STEP57_CHECK(STEP57_PLANT_RPM_PER_STEP==5.0 && STEP57_PLANT_TAU_SEC==1.20,
                 "RPM/step gain and engine lag remain explicit simulator constants, not calibration claims");
    STEP57_CHECK(s.max_abs_error<500u,
                 "closed-loop plant remains bounded throughout the warm-down exercise");
    printf("  start: coolant=96 IAC=%u rpm=%0.1f; end: coolant=%u target=%u IAC=%u rpm=%0.1f\n",
           initial_pos,initial_rpm,(unsigned int)s.coolant,s.target_rpm,
           (unsigned int)s.iac_position,s.rpm);
    printf("  motor totals: open=%lu close=%lu integral=%d maxAbsError=%u RPM\n",
           s.open_steps,s.close_steps,s.integral,s.max_abs_error);
    printf("  NOTE: coolant trajectory and engine air/RPM response are PC plant assumptions.\n");
    printf("  NOTE: this first closure intentionally exposes that the isolated Step-37 PID helper is not yet\n");
    printf("        sufficient as the complete source controller; deadband/persistence/base-bias/motor cadence\n");
    printf("        must be integrated before treating the simulated idle as quantitatively faithful.\n");
    printf("  step-57 closed-loop IAC plant regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP57_CHECK
}
/* ---------------------------------------------------------------------- */
/* Step 58: source-shaped IAC regulator state around LD472..LD6C6.        */
/*                                                                        */
/* The important correction here is structural: L0103 is not an ordinary */
/* continuously accumulated I term.  LD624 compares the magnitude of the */
/* P+D linear result with LC66C=8.  At or above that threshold the source */
/* clears L0103 and goes directly to quantization (LD675).  Only the small */
/* output branch LD633..LD668 can accumulate L0103.                        */
/* ---------------------------------------------------------------------- */
#define STEP58_CONTROL_DT_SEC       0.020
#define STEP58_INV_QUANT             8u
#define STEP58_QUANT_GAIN           32u
#define STEP58_RATE_LIMIT           12
#define STEP58_DEADBAND_PN           6
#define STEP58_FRAC_GAIN            40u
/* Return old-current RPM/12.5 scaled approximately as LD472 does for the
   50-ms rate quantity.  Positive means RPM is falling, matching the source
   high-derivative branch sense. */
static int bua_iac_rate_step58(bua_u8 old_rpm_raw,bua_u8 new_rpm_raw)
{
    int d=(int)old_rpm_raw-(int)new_rpm_raw;
    if (d>127) d=127;
    if (d<-127) d=-127;
    return d;
}
typedef struct {
    int linear;
    int fractional_before;
    int fractional_after;
    int rate_raw;
    bua_u8 abs_error;
    bua_u8 deadband_count;
    bua_u8 request;
    bua_u8 magnitude;
    bua_u8 opening;
    bua_u8 direct_quantized;
    bua_u8 fractional_branch;
    bua_u8 pid_disabled;
} BuaIacReg58;
static BuaIacReg58 bua_iac_regulator_step58(bua_u8 command_raw,
                                             bua_u8 actual_raw,
                                             bua_u8 old_actual_raw,
                                             bua_u8 coolant,
                                             int fractional_state,
                                             bua_u8 deadband_count,
                                             bua_u8 pid_disable_timer)
{
    BuaIacReg58 r;
    BuaIacPidStep37 pd;
    int err;
    int mag;
    int frac;
    unsigned int steps;
    unsigned int cm;
    int delta;
    err=(int)command_raw-(int)actual_raw;
    if (err>127) err=127;
    if (err<-127) err=-127;
    mag=err<0?-err:err;
    r.abs_error=(bua_u8)mag;
    if (mag<=STEP58_DEADBAND_PN) {
        if (deadband_count!=255u) ++deadband_count;
    } else deadband_count=0u;
    r.deadband_count=deadband_count;
    r.rate_raw=bua_iac_rate_step58(old_actual_raw,actual_raw);
    r.fractional_before=fractional_state;
    r.fractional_after=fractional_state;
    r.request=0u; r.magnitude=0u; r.opening=0u;
    r.direct_quantized=0u; r.fractional_branch=0u; r.pid_disabled=0u;
    if (pid_disable_timer!=0u) {
        r.pid_disabled=1u;
        return r;
    }
    /* Reuse the already regression-tested P/D arithmetic but suppress its
       Step-37 conventional-integral approximation. */
    pd=bua_iac_pid_step37(command_raw,actual_raw,r.rate_raw,1u,coolant,0);
    r.linear=step37_clamp127(pd.proportional+pd.derivative);
    mag=r.linear<0?-r.linear:r.linear;
    if ((unsigned int)mag>=STEP58_INV_QUANT) {
        /* LD624..LD675: a sufficiently large direct algorithm output clears
           the fractional state before conversion to motor steps. */
        r.fractional_after=0;
        r.direct_quantized=1u;
        steps=((unsigned int)mag*STEP58_QUANT_GAIN+127u)/256u;
        if (steps==0u) steps=1u;
    } else {
        /* LD633..LD668: only the sub-quantum branch is allowed to retain and
           change L0103.  This is deliberately conservative: the exact
           variable threshold generated in LD675 is retained for later work,
           but the executable rate/error gates are represented here. */
        r.fractional_branch=1u;
        frac=step37_clamp127(fractional_state);
        if ((r.rate_raw<0?-r.rate_raw:r.rate_raw)<=STEP58_RATE_LIMIT && r.abs_error>STEP58_DEADBAND_PN) {
            delta=step37_mul_gain_signed(err,STEP58_FRAC_GAIN*2u);
            frac=step37_clamp127(frac+delta);
        } else if ((r.rate_raw<0?-r.rate_raw:r.rate_raw)>STEP58_RATE_LIMIT) {
            frac=0;
        }
        r.fractional_after=frac;
        mag=frac<0?-frac:frac;
        if ((unsigned int)mag<STEP58_INV_QUANT) return r;
        steps=((unsigned int)mag*STEP58_QUANT_GAIN+127u)/256u;
        if (steps==0u) steps=1u;
    }
    cm=(unsigned int)step37_coolant_mult(coolant);
    steps=(steps*cm+127u)/256u;
    if (steps==0u) steps=1u;
    if (steps>127u) steps=127u;
    r.magnitude=(bua_u8)steps;
    r.opening=(r.linear>0 || (!r.direct_quantized && r.fractional_after>0))?1u:0u;
    r.request=(bua_u8)(r.magnitude | (r.opening?0u:0x80u));
    return r;
}
typedef struct {
    double seconds;
    double rpm;
    bua_u8 coolant;
    bua_u8 iac_position;
    bua_u8 old_rpm_raw;
    bua_u8 deadband_count;
    bua_u8 pid_disable_timer;
    int fractional;
    unsigned int target_rpm;
    unsigned long open_steps;
    unsigned long close_steps;
    unsigned long direct_cycles;
    unsigned long fractional_cycles;
} BuaClosedIdle58;
static void bua_closed_idle_init_step58(BuaClosedIdle58 *s)
{
    BuaIacStartupStep41 park;
    bua_u8 warm=bua_warm_park_step57(96u);
    park=bua_iac_startup_position_step41(128u,warm,1u,warm,0u,160u);
    s->seconds=0.0; s->coolant=96u; s->iac_position=park.target_position;
    s->rpm=STEP57_PLANT_BASE_RPM+STEP57_PLANT_RPM_PER_STEP*(double)s->iac_position;
    s->old_rpm_raw=(bua_u8)(s->rpm/12.5+0.5);
    s->deadband_count=0u; s->pid_disable_timer=0u; s->fractional=0;
    s->target_rpm=0u; s->open_steps=0ul; s->close_steps=0ul;
    s->direct_cycles=0ul; s->fractional_cycles=0ul;
}
static void bua_closed_idle_tick_step58(BuaClosedIdle58 *s)
{
    BuaIacCommandStep36 cmd;
    BuaIacReg58 reg;
    unsigned int ar;
    double equilibrium,alpha;
    s->coolant=bua_coolant_warmup_step57(s->seconds);
    cmd=bua_iac_command_step36(s->coolant,STEP36_IAC_PN_BIT,0u,0u,0u);
    s->target_rpm=cmd.command_rpm;
    ar=(unsigned int)(s->rpm/12.5+0.5); if(ar>255u) ar=255u;
    reg=bua_iac_regulator_step58(cmd.command_raw,(bua_u8)ar,s->old_rpm_raw,
                                 s->coolant,s->fractional,s->deadband_count,s->pid_disable_timer);
    s->old_rpm_raw=(bua_u8)ar; s->fractional=reg.fractional_after;
    s->deadband_count=reg.deadband_count;
    if(s->pid_disable_timer!=0u) --s->pid_disable_timer;
    if(reg.direct_quantized) ++s->direct_cycles;
    if(reg.fractional_branch) ++s->fractional_cycles;
    if(reg.magnitude!=0u) {
        if(reg.opening) { if(s->iac_position<145u) ++s->iac_position; ++s->open_steps; }
        else { if(s->iac_position>0u) --s->iac_position; ++s->close_steps; }
    }
    equilibrium=STEP57_PLANT_BASE_RPM+STEP57_PLANT_RPM_PER_STEP*(double)s->iac_position;
    alpha=STEP58_CONTROL_DT_SEC/STEP57_PLANT_TAU_SEC;
    s->rpm+=(equilibrium-s->rpm)*alpha;
    s->seconds+=STEP58_CONTROL_DT_SEC;
}
static void run_step58_iac_regulator_test(void)
{
    BuaIacReg58 r;
    BuaClosedIdle58 s;
    unsigned int passed=0u,total=18u,i;
#define STEP58_CHECK(c,t) do { if(c) ++passed; printf("  %-78s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-58 source-shaped IAC regulator / closed-plant regression:\n");
    r=bua_iac_regulator_step58(120u,40u,40u,160u,77,0u,0u);
    STEP58_CHECK(r.direct_quantized && r.fractional_after==0,
                 "|P+D| >= LC66C=8 takes direct branch and clears L0103 fractional state");
    STEP58_CHECK(r.magnitude>0u && r.opening,
                 "large underspeed error produces an opening motor request");
    r=bua_iac_regulator_step58(40u,120u,120u,160u,-77,0u,0u);
    STEP58_CHECK(r.direct_quantized && r.fractional_after==0 && !r.opening,
                 "large overspeed error likewise clears L0103 and requests closing");
    r=bua_iac_regulator_step58(64u,64u,64u,160u,5,0u,0u);
    STEP58_CHECK(r.fractional_branch && !r.direct_quantized,
                 "small linear output enters LD633-style fractional branch rather than direct quantization");
    STEP58_CHECK(r.deadband_count==1u,
                 "RPM error inside LC648 P/N deadband increments the persistent deadband counter");
    r=bua_iac_regulator_step58(80u,60u,60u,160u,0,0u,3u);
    STEP58_CHECK(r.pid_disabled && r.request==0u,
                 "nonzero transition/PID-disable timer suppresses closed-loop motor request");
    STEP58_CHECK(bua_iac_rate_step58(80u,72u)==8 && bua_iac_rate_step58(72u,80u)==-8,
                 "LD472-style old-minus-current RPM rate preserves falling/rising sign");
    bua_closed_idle_init_step58(&s);
    STEP58_CHECK(s.iac_position==122u,
                 "closed plant starts from the same source-derived 122-step startup position as Step 57");
    for(i=0u;i<4500u;++i) bua_closed_idle_tick_step58(&s);
    STEP58_CHECK(s.seconds>89.99 && s.seconds<90.01,
                 "20-ms regulator exercise spans 90 seconds of simulator warm-up");
    STEP58_CHECK(s.target_rpm==600u,
                 "source-derived warm P/N target remains 600 RPM");
    STEP58_CHECK(s.close_steps>0ul,
                 "complete-state experiment commands closing motion during warm-down");
    STEP58_CHECK(s.open_steps>0ul,
                 "rate/fractional state can command reopening rather than one-way windup");
    STEP58_CHECK(s.iac_position>0u,
                 "corrected regulator no longer drives the simple plant to the fully closed IAC stop");
    STEP58_CHECK(s.rpm>540.0,
                 "final RPM is substantially above the Step-57 466-RPM undershoot");
    STEP58_CHECK(s.rpm<700.0,
                 "final RPM remains in the neighborhood of the 600-RPM commanded idle");
    STEP58_CHECK(s.fractional_cycles>0ul,
                 "closed-plant run exercises the source fractional-output branch persistently");
    STEP58_CHECK(s.fractional>=-127 && s.fractional<=127,
                 "L0103-like fractional state remains within signed-byte saturation");
    STEP58_CHECK(STEP58_INV_QUANT==8u && STEP58_QUANT_GAIN==32u,
                 "regulator uses source LC66C=8 inverse quant and LC66B=32 quantizer gain");
    printf("  final: coolant=%u target=%u IAC=%u rpm=%0.1f frac=%d open=%lu close=%lu\n",
           (unsigned int)s.coolant,s.target_rpm,(unsigned int)s.iac_position,s.rpm,
           s.fractional,s.open_steps,s.close_steps);
    printf("  branches: direct=%lu fractional=%lu\n",s.direct_cycles,s.fractional_cycles);
    printf("  NOTE: the 20-ms exercise cadence follows the calibration comments (LC669=20 -> 400 ms,\n");
    printf("        LC675=16 -> 320 ms); exact scheduler invocation cadence still deserves executable tracing.\n");
    printf("  NOTE: LD633..LD675 contains an additional variable fractional threshold not yet translated exactly.\n");
    printf("  step-58 IAC regulator regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP58_CHECK
}
/* ---------------------------------------------------------------------- */
/* Step 59: exact fractional IAC quantizer structure, LD633..LD6B6.       */
/*                                                                        */
/* Local-stack meanings reconstructed from LD41F..LD6C6:                  */
/*   Y+2 = absolute RPM error                                             */
/*   Y+3 = absolute RPM-rate value                                        */
/*   Y+4 = signed RPM error, later signed algorithm output                */
/*   Y+5 = selected RPM-error deadband                                    */
/*   Y+6 = maximum RPM rate for integral action                           */
/*   Y+7 = normal integral gain                                           */
/*                                                                        */
/* L0103 is a signed fractional-step accumulator.  A direct (>=8) output  */
/* clears it.  A small output integrates error until |L0103| reaches 8,   */
/* then the quantizer emits steps and writes the signed quantization      */
/* remainder back to L0103.                                               */
/* ---------------------------------------------------------------------- */
#define STEP59_CONTROL_DT_SEC       0.050
#define STEP59_INV_QUANT             8u   /* LC66C */
#define STEP59_QUANT_GAIN           32u   /* LC66B */
#define STEP59_RATE_LIMIT           12u   /* LC65B, P/N */
#define STEP59_DEADBAND_PN           6u   /* LC648 */
#define STEP59_INT_GAIN_PN          40u   /* LC659 */
#define STEP59_LOW_INT_GAIN_PN      40u   /* LC678 */
#define STEP59_LOW_INT_GAIN_DRIVE    8u   /* LC679 */
#define STEP59_INT_CLEAR_ERROR       0u   /* LC67A */
typedef struct {
    int linear;
    int fractional_before;
    int fractional_after;
    int integral_delta;
    int signed_quant_input;
    int rate_raw;
    bua_u8 abs_error;
    bua_u8 direct_branch;
    bua_u8 fractional_branch;
    bua_u8 residual_written;
    bua_u8 request;
    bua_u8 magnitude;
    bua_u8 opening;
} BuaIacReg59;
static unsigned int step59_round_high_byte(unsigned int v)
{
    unsigned int hi=(v>>8)&255u;
    unsigned int lo=v&255u;
    if(lo>=128u && hi<255u) ++hi;
    return hi;
}
static BuaIacReg59 bua_iac_regulator_step59(bua_u8 command_raw,
                                             bua_u8 actual_raw,
                                             bua_u8 old_actual_raw,
                                             bua_u8 coolant,
                                             int fractional_state,
                                             bua_u8 park_neutral)
{
    BuaIacReg59 r;
    BuaIacPidStep37 pd;
    int err,abs_err,rate,abs_rate,linear,abs_linear,frac,delta;
    unsigned int gain,p,lo,rounded_hi,stored16,resid_mag,scaled16,steps;
    bua_u8 cm;
    err=(int)command_raw-(int)actual_raw;
    if(err>127) err=127;
    if(err<-127) err=-127;
    abs_err=err<0?-err:err;
    rate=bua_iac_rate_step58(old_actual_raw,actual_raw);
    abs_rate=rate<0?-rate:rate;
    pd=bua_iac_pid_step37(command_raw,actual_raw,rate,park_neutral,coolant,0);
    linear=step37_clamp127(pd.proportional+pd.derivative);
    abs_linear=linear<0?-linear:linear;
    frac=step37_clamp127(fractional_state);
    r.linear=linear; r.fractional_before=frac; r.fractional_after=frac;
    r.integral_delta=0; r.signed_quant_input=0; r.rate_raw=rate;
    r.abs_error=(bua_u8)abs_err; r.direct_branch=0u; r.fractional_branch=0u;
    r.residual_written=0u; r.request=0u; r.magnitude=0u; r.opening=0u;
    if((unsigned int)abs_linear>=STEP59_INV_QUANT) {
        /* LD624 direct path: CLR L0103, retain signed algorithm output in Y+4. */
        r.direct_branch=1u;
        frac=0;
        r.fractional_after=0;
        r.signed_quant_input=linear;
    } else {
        r.fractional_branch=1u;
        /* LD633: excessive RPM rate clears the accumulator and request. */
        if((unsigned int)abs_rate>STEP59_RATE_LIMIT) {
            r.fractional_after=0;
            return r;
        }
        gain=STEP59_INT_GAIN_PN;
        if((unsigned int)abs_err<=STEP59_DEADBAND_PN) {
            /* LC67A=0: exact zero error clears integral/request. */
            if((unsigned int)abs_err<=STEP59_INT_CLEAR_ERROR) {
                r.fractional_after=0;
                return r;
            }
            gain=park_neutral?STEP59_LOW_INT_GAIN_PN:STEP59_LOW_INT_GAIN_DRIVE;
        }
        /* LD656: MUL; ASLD; use only A => floor(abs(error)*gain/128). */
        delta=(int)(((unsigned int)abs_err*gain*2u)>>8);
        if(err<0) delta=-delta;
        frac=step37_clamp127(frac+delta);
        r.integral_delta=delta;
        r.fractional_after=frac;
        if((unsigned int)(frac<0?-frac:frac)<STEP59_INV_QUANT) return r;
        r.signed_quant_input=frac;
    }
    /* LD675..LD686.  MUL by LC66B, round the high byte with MUL's carry,
       preserve the low-byte remainder, then derive a 0..8 residual using
       LC66C.  This residual is written back only when L0103 was active. */
    p=(unsigned int)(r.signed_quant_input<0?-r.signed_quant_input:r.signed_quant_input)
      * STEP59_QUANT_GAIN;
    lo=p&255u;
    rounded_hi=step59_round_high_byte(p);
    stored16=((rounded_hi&255u)<<8)|lo;
    resid_mag=step59_round_high_byte(STEP59_INV_QUANT*lo);
    if(resid_mag>127u) resid_mag=127u;
    cm=step37_coolant_mult(coolant);
    scaled16=(unsigned int)bua_mul_u8_u16_lf266(cm,(bua_u16)stored16);
    steps=step59_round_high_byte(scaled16);
    if(steps>127u) steps=127u;
    if(steps==0u) return r;
    r.magnitude=(bua_u8)steps;
    r.opening=(r.signed_quant_input>=0)?1u:0u;
    r.request=(bua_u8)(r.magnitude | (r.opening?0u:0x80u));
    if(r.fractional_branch && r.fractional_after!=0) {
        int residual=(int)resid_mag;
        if(r.signed_quant_input<0) residual=-residual;
        r.fractional_after=residual;
        r.residual_written=1u;
    }
    return r;
}
typedef struct {
    double seconds;
    double rpm;
    bua_u8 coolant;
    bua_u8 iac_position;
    bua_u8 old_rpm_raw;
    int fractional;
    unsigned int target_rpm;
    unsigned long open_steps;
    unsigned long close_steps;
    unsigned long direct_cycles;
    unsigned long fractional_cycles;
    unsigned long residual_writes;
} BuaClosedIdle59;
static void bua_closed_idle_init_step59(BuaClosedIdle59 *s)
{
    BuaIacStartupStep41 park;
    bua_u8 warm=bua_warm_park_step57(96u);
    park=bua_iac_startup_position_step41(128u,warm,1u,warm,0u,160u);
    s->seconds=0.0; s->coolant=96u; s->iac_position=park.target_position;
    s->rpm=STEP57_PLANT_BASE_RPM+STEP57_PLANT_RPM_PER_STEP*(double)s->iac_position;
    s->old_rpm_raw=(bua_u8)(s->rpm/12.5+0.5); s->fractional=0; s->target_rpm=0u;
    s->open_steps=0ul; s->close_steps=0ul; s->direct_cycles=0ul;
    s->fractional_cycles=0ul; s->residual_writes=0ul;
}
static void bua_closed_idle_tick_step59(BuaClosedIdle59 *s)
{
    BuaIacCommandStep36 cmd;
    BuaIacReg59 reg;
    unsigned int ar;
    double equilibrium,alpha;
    s->coolant=bua_coolant_warmup_step57(s->seconds);
    cmd=bua_iac_command_step36(s->coolant,STEP36_IAC_PN_BIT,0u,0u,0u);
    s->target_rpm=cmd.command_rpm;
    ar=(unsigned int)(s->rpm/12.5+0.5); if(ar>255u) ar=255u;
    reg=bua_iac_regulator_step59(cmd.command_raw,(bua_u8)ar,s->old_rpm_raw,
                                 s->coolant,s->fractional,1u);
    s->old_rpm_raw=(bua_u8)ar; s->fractional=reg.fractional_after;
    if(reg.direct_branch) ++s->direct_cycles;
    if(reg.fractional_branch) ++s->fractional_cycles;
    if(reg.residual_written) ++s->residual_writes;
    if(reg.magnitude!=0u) {
        /* LF5B0 consumes one physical step per 6.25-ms motor-service pass;
           the regulator only replaces/refreshes the request every 50 ms.
           For this plant test execute one available step here; later work can
           model the eight intervening 6.25-ms motor passes explicitly. */
        if(reg.opening) { if(s->iac_position<145u) ++s->iac_position; ++s->open_steps; }
        else { if(s->iac_position>0u) --s->iac_position; ++s->close_steps; }
    }
    equilibrium=STEP57_PLANT_BASE_RPM+STEP57_PLANT_RPM_PER_STEP*(double)s->iac_position;
    alpha=STEP59_CONTROL_DT_SEC/STEP57_PLANT_TAU_SEC;
    s->rpm+=(equilibrium-s->rpm)*alpha;
    s->seconds+=STEP59_CONTROL_DT_SEC;
}
static void run_step59_iac_fractional_test(void)
{
    BuaIacReg59 r;
    BuaClosedIdle59 s;
    unsigned int passed=0u,total=21u,i;
#define STEP59_CHECK(c,t) do { if(c) ++passed; printf("  %-82s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-59 exact fractional IAC quantizer / 50-ms scheduler regression:\n");
    r=bua_iac_regulator_step59(140u,40u,40u,160u,5,1u);
    STEP59_CHECK(r.direct_branch && r.fractional_after==0,
                 "large source P+D output takes LD624 direct path and clears L0103");
    STEP59_CHECK(r.magnitude>0u && r.opening,
                 "direct underspeed output produces an opening motor request");
    r=bua_iac_regulator_step59(65u,64u,64u,160u,0,1u);
    STEP59_CHECK(r.fractional_branch && !r.direct_branch,
                 "small output enters LD633 fractional branch");
    STEP59_CHECK(r.integral_delta==0 || r.integral_delta==1,
                 "small P/N error uses floor(error*gain/128) fixed-point integral increment");
    r=bua_iac_regulator_step59(64u,64u,64u,160u,7,1u);
    STEP59_CHECK(r.fractional_after==0 && r.request==0u,
                 "LC67A=0 exact-zero RPM error clears fractional accumulator and request");
    r=bua_iac_regulator_step59(65u,64u,40u,160u,7,1u);
    STEP59_CHECK(r.fractional_after==0 && r.request==0u,
                 "RPM-rate magnitude above LC65B=12 clears fractional accumulator");
    /* Force an accumulator crossing so residual feedback is visible. */
    r=bua_iac_regulator_step59(68u,64u,64u,160u,7,1u);
    STEP59_CHECK(r.fractional_branch && r.magnitude>0u,
                 "fractional accumulator crossing LC66C=8 emits a motor request");
    STEP59_CHECK(r.residual_written && r.fractional_after>=0 && r.fractional_after<8,
                 "after fractional output L0103 is replaced by signed quantization residual");
    r=bua_iac_regulator_step59(60u,64u,64u,160u,-7,1u);
    STEP59_CHECK(r.fractional_branch && (!r.residual_written || r.fractional_after<=0),
                 "negative fractional correction preserves closing-direction sign");
    STEP59_CHECK(STEP59_CONTROL_DT_SEC>0.049 && STEP59_CONTROL_DT_SEC<0.051,
                 "PID regulator cadence is 50 ms, matching executable L0000 & 7 gate");
    STEP59_CHECK(STEP59_LOW_INT_GAIN_PN==40u && STEP59_LOW_INT_GAIN_DRIVE==8u,
                 "LC678/LC679 low integral gains are 40 P/N and 8 Drive");
    bua_closed_idle_init_step59(&s);
    STEP59_CHECK(s.iac_position==122u,
                 "closed plant retains source-derived 122-step startup IAC position");
    for(i=0u;i<3600u;++i) bua_closed_idle_tick_step59(&s);
    STEP59_CHECK(s.seconds>179.98 && s.seconds<180.02,
                 "3600 executable-rate regulator passes span 180 seconds");
    STEP59_CHECK(s.target_rpm==600u,
                 "warm source-derived P/N target is 600 RPM");
    STEP59_CHECK(s.close_steps>0ul && s.open_steps<10ul,
                 "corrected fractional/cadence model settles without Step-58 opening/closing chatter");
    STEP59_CHECK(s.iac_position>0u && s.iac_position<145u,
                 "IAC remains away from both physical model stops");
    STEP59_CHECK(s.rpm>525.0 && s.rpm<675.0,
                 "final RPM lies inside the source P/N +/-75-RPM deadband around 600 RPM");
    STEP59_CHECK(s.fractional_cycles>0ul,
                 "closed run repeatedly exercises LD633 fractional branch");
    STEP59_CHECK(s.residual_writes>0ul,
                 "closed run exercises L0103 quantization-residual feedback");
    STEP59_CHECK(s.fractional>-8 && s.fractional<8,
                 "final retained L0103 residual is sub-quantum after output feedback");
    STEP59_CHECK(STEP59_INV_QUANT==8u && STEP59_QUANT_GAIN==32u,
                 "LC66C=8 and LC66B=32 fixed-point quantizer constants preserved");
    printf("  final: coolant=%u target=%u IAC=%u rpm=%0.1f frac=%d open=%lu close=%lu\n",
           (unsigned int)s.coolant,s.target_rpm,(unsigned int)s.iac_position,s.rpm,
           s.fractional,s.open_steps,s.close_steps);
    printf("  branches: direct=%lu fractional=%lu residualWrites=%lu\n",
           s.direct_cycles,s.fractional_cycles,s.residual_writes);
    printf("  NOTE: the source itself labels this PID block as 50-msec logic and gates LD41F with (L0000 & 7)==0.\n");
    printf("  NOTE: motor execution is still simplified to one consumed step per regulator pass in this plant test;\n");
    printf("        LF4DF/LF5B0 actually services the motor every 6.25 ms, so request-consumption timing is next.\n");
    printf("  step-59 IAC fractional/scheduler regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP59_CHECK
}
/* ---------------------------------------------------------------------- */
/* Step 60: 50-ms regulator + 6.25-ms physical IAC motor execution.       */
/*                                                                        */
/* LF4DF is entered every 6.25-ms minor loop.  The LD41F PID block runs   */
/* only when (L0000 & 7)==0, i.e. once every eight minor loops.  L0101    */
/* therefore persists as a packed motor request and LF5B0 consumes at     */
/* most one requested step on each intervening IAC service.               */
/* ---------------------------------------------------------------------- */
#define STEP60_MINOR_DT_SEC       0.00625
#define STEP60_PID_DIVISOR        8u
#define STEP60_PID_DT_SEC         0.050
typedef struct {
    double seconds;
    double rpm;
    bua_u8 coolant;
    bua_u8 iac_position;
    bua_u8 old_rpm_raw;
    int fractional;
    unsigned int target_rpm;
    bua_u8 pending_request;
    bua_u8 motor_on;
    bua_u8 running_counter;
    unsigned int minor_phase;
    unsigned long pid_passes;
    unsigned long motor_services;
    unsigned long request_writes;
    unsigned long consumed_steps;
    unsigned long physical_moves;
    unsigned long open_steps;
    unsigned long close_steps;
    unsigned long multi_step_windows;
    unsigned int steps_this_window;
    unsigned int max_steps_in_window;
} BuaClosedIdle60;
static void bua_closed_idle_init_step60(BuaClosedIdle60 *s)
{
    BuaIacStartupStep41 park;
    bua_u8 warm=bua_warm_park_step57(96u);
    park=bua_iac_startup_position_step41(128u,warm,1u,warm,0u,160u);
    s->seconds=0.0; s->coolant=96u; s->iac_position=park.target_position;
    s->rpm=STEP57_PLANT_BASE_RPM+STEP57_PLANT_RPM_PER_STEP*(double)s->iac_position;
    s->old_rpm_raw=(bua_u8)(s->rpm/12.5+0.5); s->fractional=0; s->target_rpm=0u;
    s->pending_request=0u; s->motor_on=1u; s->running_counter=0u; s->minor_phase=0u;
    s->pid_passes=0ul; s->motor_services=0ul; s->request_writes=0ul;
    s->consumed_steps=0ul; s->physical_moves=0ul; s->open_steps=0ul; s->close_steps=0ul;
    s->multi_step_windows=0ul; s->steps_this_window=0u; s->max_steps_in_window=0u;
}
static void bua_closed_idle_minor_step60(BuaClosedIdle60 *s)
{
    BuaIacCommandStep36 cmd;
    BuaIacReg59 reg;
    BuaIacMotorStep40 mot;
    unsigned int ar;
    double equilibrium,alpha;
    s->coolant=bua_coolant_warmup_step57(s->seconds);
    /* LD41F executes every eighth 6.25-ms minor loop.  A new calculation
       replaces L0101; otherwise the previous packed request persists. */
    if(s->minor_phase==0u) {
        if(s->steps_this_window>1u) ++s->multi_step_windows;
        if(s->steps_this_window>s->max_steps_in_window) s->max_steps_in_window=s->steps_this_window;
        s->steps_this_window=0u;
        cmd=bua_iac_command_step36(s->coolant,STEP36_IAC_PN_BIT,0u,0u,0u);
        s->target_rpm=cmd.command_rpm;
        ar=(unsigned int)(s->rpm/12.5+0.5); if(ar>255u) ar=255u;
        reg=bua_iac_regulator_step59(cmd.command_raw,(bua_u8)ar,s->old_rpm_raw,
                                     s->coolant,s->fractional,1u);
        s->old_rpm_raw=(bua_u8)ar;
        s->fractional=reg.fractional_after;
        s->pending_request=reg.request;
        ++s->pid_passes;
        ++s->request_writes;
    }
    mot=bua_iac_motor_service_step40(s->pending_request,s->iac_position,
                                     s->running_counter,0u,0u,0u,
                                     s->motor_on,1u);
    ++s->motor_services;
    s->pending_request=mot.command;
    s->iac_position=mot.present_position;
    s->running_counter=mot.running_counter;
    s->motor_on=mot.motor_on;
    if(mot.consumed_step) { ++s->consumed_steps; ++s->steps_this_window; }
    if(mot.physical_move) ++s->physical_moves;
    if(mot.opening && mot.consumed_step) ++s->open_steps;
    if(mot.closing && mot.consumed_step) ++s->close_steps;
    equilibrium=STEP57_PLANT_BASE_RPM+STEP57_PLANT_RPM_PER_STEP*(double)s->iac_position;
    alpha=STEP60_MINOR_DT_SEC/STEP57_PLANT_TAU_SEC;
    s->rpm+=(equilibrium-s->rpm)*alpha;
    s->seconds+=STEP60_MINOR_DT_SEC;
    s->minor_phase=(s->minor_phase+1u)&7u;
}
static void run_step60_iac_multirate_test(void)
{
    BuaClosedIdle60 s;
    BuaIacMotorStep40 m;
    unsigned int passed=0u,total=18u,i;
#define STEP60_CHECK(c,t) do { if(c) ++passed; printf("  %-84s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-60 50-ms regulator / 6.25-ms IAC motor-service regression:\n");
    STEP60_CHECK(STEP60_PID_DIVISOR==8u && STEP60_MINOR_DT_SEC*8.0>0.0499 && STEP60_MINOR_DT_SEC*8.0<0.0501,
                 "eight 6.25-ms motor-service passes exactly span one 50-ms PID interval");
    m=bua_iac_motor_service_step40(5u,50u,0u,0u,0u,0u,1u,1u);
    STEP60_CHECK(m.command==4u && m.present_position==51u && m.consumed_step,
                 "Step-40 executor consumes exactly one opening step from a persistent packed request");
    m=bua_iac_motor_service_step40(m.command,m.present_position,m.running_counter,0u,0u,0u,m.motor_on,1u);
    STEP60_CHECK(m.command==3u && m.present_position==52u,
                 "a second 6.25-ms service continues consuming the same request without a new PID calculation");
    bua_closed_idle_init_step60(&s);
    STEP60_CHECK(s.iac_position==122u,
                 "multirate plant starts from the source-derived 122-step startup IAC position");
    for(i=0u;i<28800u;++i) bua_closed_idle_minor_step60(&s);
    if(s.steps_this_window>1u) ++s.multi_step_windows;
    if(s.steps_this_window>s.max_steps_in_window) s.max_steps_in_window=s.steps_this_window;
    STEP60_CHECK(s.seconds>179.99 && s.seconds<180.01,
                 "28800 minor-loop services span 180 seconds at 6.25 ms each");
    STEP60_CHECK(s.motor_services==28800ul,
                 "IAC motor executor is serviced once on every simulated 6.25-ms minor loop");
    STEP60_CHECK(s.pid_passes==3600ul,
                 "PID regulator executes exactly 3600 times, once per eight motor-service passes");
    STEP60_CHECK(s.target_rpm==600u,
                 "warm source-derived P/N target remains 600 RPM");
    STEP60_CHECK(s.consumed_steps>s.pid_passes/100ul,
                 "persistent L0101 requests produce physical step consumption between PID calculations");
    STEP60_CHECK(s.max_steps_in_window==1u,
                 "this warm-idle plant happens to request only single steps despite eight available service slots");
    STEP60_CHECK(s.multi_step_windows==0ul,
                 "absence of multi-step bursts is reported as plant behavior, not forced by the scheduler");
    STEP60_CHECK(s.physical_moves<=s.consumed_steps,
                 "physical moves never exceed consumed packed-command steps at position limits");
    STEP60_CHECK(s.close_steps>0ul,
                 "warm-down produces source-shaped IAC closing motion");
    STEP60_CHECK(s.iac_position>0u && s.iac_position<145u,
                 "multirate IAC remains away from both modeled mechanical stops");
    STEP60_CHECK(s.rpm>525.0 && s.rpm<675.0,
                 "final RPM remains inside the calibrated P/N +/-75-RPM deadband around 600 RPM");
    STEP60_CHECK(s.fractional>-8 && s.fractional<8,
                 "final L0103 fractional residue remains sub-quantum");
    STEP60_CHECK(s.motor_services==s.pid_passes*STEP60_PID_DIVISOR,
                 "observed motor/PID service ratio is exactly the executable-derived 8:1 relationship");
    STEP60_CHECK(s.request_writes==s.pid_passes,
                 "L0101 is refreshed only on regulator passes in this closed-loop reconstruction");
    printf("  final: coolant=%u target=%u IAC=%u rpm=%0.1f frac=%d pending=%02X\n",
           (unsigned int)s.coolant,s.target_rpm,(unsigned int)s.iac_position,s.rpm,s.fractional,
           (unsigned int)s.pending_request);
    printf("  cadence: motorServices=%lu pidPasses=%lu ratio=%0.1f:1\n",
           s.motor_services,s.pid_passes,(double)s.motor_services/(double)s.pid_passes);
    printf("  motion: consumed=%lu physical=%lu open=%lu close=%lu multiStepWindows=%lu max/window=%u\n",
           s.consumed_steps,s.physical_moves,s.open_steps,s.close_steps,s.multi_step_windows,s.max_steps_in_window);
    printf("  NOTE: Step 60 now models the source timing boundary directly: LD41F updates at 50 ms,\n");
    printf("        while LF4DF/LF5B0 can consume one pending L0101 step every 6.25 ms.\n");
    printf("  step-60 multirate IAC regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP60_CHECK
}
/* ---------------------------------------------------------------------- */
/* Step 62: integrated A/C anticipation versus unanticipated compressor-load response.                    */
/*                                                                        */
/* The ECM side is unchanged from Step 60: LD41F updates the regulator    */
/* every 50 ms and LF4DF/LF5B0 services a persistent L0101 request every  */
/* 6.25 ms.  Only the PC engine plant gains a temporary external load.    */
/* The load magnitude/duration are simulator stimuli, not 9340 constants. */
/* ---------------------------------------------------------------------- */
#define STEP61_LOAD_START_SEC       60.0
#define STEP61_LOAD_END_SEC         65.0
#define STEP61_LOAD_RPM_DROP       340.0
/* Advance the Step-60 closed-loop model by one minor loop while applying
   an external load as a reduction in the plant's equilibrium RPM. */
static void bua_closed_idle_minor_step61(BuaClosedIdle60 *s, double load_rpm)
{
    BuaIacCommandStep36 cmd;
    BuaIacReg59 reg;
    BuaIacMotorStep40 mot;
    unsigned int ar;
    double equilibrium,alpha;
    s->coolant=bua_coolant_warmup_step57(s->seconds);
    if(s->minor_phase==0u) {
        if(s->steps_this_window>1u) ++s->multi_step_windows;
        if(s->steps_this_window>s->max_steps_in_window) s->max_steps_in_window=s->steps_this_window;
        s->steps_this_window=0u;
        cmd=bua_iac_command_step36(s->coolant,STEP36_IAC_PN_BIT,0u,0u,0u);
        s->target_rpm=cmd.command_rpm;
        ar=(unsigned int)(s->rpm/12.5+0.5); if(ar>255u) ar=255u;
        reg=bua_iac_regulator_step59(cmd.command_raw,(bua_u8)ar,s->old_rpm_raw,
                                     s->coolant,s->fractional,1u);
        s->old_rpm_raw=(bua_u8)ar;
        s->fractional=reg.fractional_after;
        s->pending_request=reg.request;
        ++s->pid_passes;
        ++s->request_writes;
    }
    mot=bua_iac_motor_service_step40(s->pending_request,s->iac_position,
                                     s->running_counter,0u,0u,0u,
                                     s->motor_on,1u);
    ++s->motor_services;
    s->pending_request=mot.command;
    s->iac_position=mot.present_position;
    s->running_counter=mot.running_counter;
    s->motor_on=mot.motor_on;
    if(mot.consumed_step) { ++s->consumed_steps; ++s->steps_this_window; }
    if(mot.physical_move) ++s->physical_moves;
    if(mot.opening && mot.consumed_step) ++s->open_steps;
    if(mot.closing && mot.consumed_step) ++s->close_steps;
    equilibrium=STEP57_PLANT_BASE_RPM+STEP57_PLANT_RPM_PER_STEP*(double)s->iac_position-load_rpm;
    alpha=STEP60_MINOR_DT_SEC/STEP57_PLANT_TAU_SEC;
    s->rpm+=(equilibrium-s->rpm)*alpha;
    if(s->rpm<0.0) s->rpm=0.0;
    s->seconds+=STEP60_MINOR_DT_SEC;
    s->minor_phase=(s->minor_phase+1u)&7u;
}
static void run_step61_idle_load_disturbance_test(void)
{
    BuaClosedIdle60 s;
    unsigned int passed=0u,total=20u,i;
    double load;
    double rpm_pre=0.0,rpm_min=10000.0,rpm_endload=0.0,rpm_recovered=0.0;
    unsigned int target_pre=0u;
    bua_u8 iac_pre=0u,iac_peak=0u,iac_end=0u;
    unsigned long open_pre=0ul,open_during=0ul,close_after=0ul;
    unsigned long multi_pre=0ul;
#define STEP61_CHECK(c,t) do { if(c) ++passed; printf("  %-84s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-61 sudden idle-load disturbance / recovery regression:\n");
    bua_closed_idle_init_step60(&s);
    for(i=0u;i<16000u;++i) { /* 100 seconds */
        load=(s.seconds>=STEP61_LOAD_START_SEC && s.seconds<STEP61_LOAD_END_SEC)?STEP61_LOAD_RPM_DROP:0.0;
        if(s.seconds>=59.99 && s.seconds<60.01) {
            rpm_pre=s.rpm; target_pre=s.target_rpm; iac_pre=s.iac_position; open_pre=s.open_steps; multi_pre=s.multi_step_windows;
        }
        bua_closed_idle_minor_step61(&s,load);
        if(s.seconds>=STEP61_LOAD_START_SEC && s.seconds<=STEP61_LOAD_END_SEC) {
            if(s.rpm<rpm_min) rpm_min=s.rpm;
            if(s.iac_position>iac_peak) iac_peak=s.iac_position;
        }
        if(s.seconds>=64.99 && s.seconds<65.02) {
            rpm_endload=s.rpm; open_during=s.open_steps-open_pre;
        }
        if(s.seconds>=74.99 && s.seconds<75.02) rpm_recovered=s.rpm;
    }
    iac_end=s.iac_position;
    close_after=s.close_steps;
    if(s.steps_this_window>1u) ++s.multi_step_windows;
    if(s.steps_this_window>s.max_steps_in_window) s.max_steps_in_window=s.steps_this_window;
    STEP61_CHECK(STEP61_LOAD_RPM_DROP==340.0 && STEP61_LOAD_END_SEC-STEP61_LOAD_START_SEC==5.0,
                 "PC-only disturbance is a documented 340-RPM-equivalent load lasting five seconds");
    STEP61_CHECK(rpm_pre>(double)target_pre-75.0 && rpm_pre<(double)target_pre+75.0,
                 "engine is settled inside the calibrated P/N deadband before load application");
    STEP61_CHECK(iac_pre>0u && iac_pre<145u,
                 "IAC begins disturbance away from either modeled mechanical stop");
    STEP61_CHECK(rpm_min<rpm_pre-75.0,
                 "sudden load pulls RPM outside the +/-75-RPM P/N deadband");
    STEP61_CHECK(iac_peak>iac_pre,
                 "reconstructed regulator responds to underspeed by opening the IAC");
    STEP61_CHECK(open_during>0ul,
                 "physical 6.25-ms motor executor consumes opening requests while load is present");
    STEP61_CHECK(rpm_endload>rpm_min+50.0,
                 "RPM recovers substantially before the five-second load is removed");
    STEP61_CHECK(rpm_recovered>525.0 && rpm_recovered<675.0,
                 "after load removal RPM returns to the calibrated P/N idle deadband");
    STEP61_CHECK(iac_end>0u && iac_end<145u,
                 "IAC remains inside modeled travel after recovery");
    STEP61_CHECK(s.open_steps>0ul && s.close_steps>0ul,
                 "disturbance experiment exercises both opening and closing motor directions");
    STEP61_CHECK(close_after>0ul,
                 "controller commands closing motion after excess load airflow is no longer needed");
    STEP61_CHECK(s.motor_services==16000ul,
                 "100-second experiment executes exactly 16000 6.25-ms IAC services");
    STEP61_CHECK(s.pid_passes==2000ul,
                 "100-second experiment executes exactly 2000 50-ms PID passes");
    STEP61_CHECK(s.motor_services==s.pid_passes*8ul,
                 "disturbance preserves the executable-derived 8:1 motor/PID cadence");
    STEP61_CHECK(s.max_steps_in_window>1u,
                 "sudden underspeed finally produces a multi-step request consumed between PID passes");
    STEP61_CHECK(s.multi_step_windows>multi_pre,
                 "at least one 50-ms window consumes multiple physical IAC steps because of the load");
    STEP61_CHECK(s.max_steps_in_window<=8u,
                 "no 50-ms window can consume more than the eight available 6.25-ms motor services");
    STEP61_CHECK(s.physical_moves<=s.consumed_steps,
                 "physical IAC moves remain bounded by consumed packed-command steps");
    STEP61_CHECK(s.fractional>-8 && s.fractional<8,
                 "post-recovery L0103 fractional residue remains sub-quantum");
    STEP61_CHECK(s.rpm>525.0 && s.rpm<675.0,
                 "final 100-second RPM remains in the original P/N deadband");
    printf("  disturbance: pre=%0.1f rpm, min=%0.1f, end-load=%0.1f, recovered=%0.1f, final=%0.1f\n",
           rpm_pre,rpm_min,rpm_endload,rpm_recovered,s.rpm);
    printf("  IAC: pre=%u peak=%u final=%u; motion open=%lu close=%lu; max/window=%u multiWindows=%lu\n",
           (unsigned int)iac_pre,(unsigned int)iac_peak,(unsigned int)iac_end,
           s.open_steps,s.close_steps,s.max_steps_in_window,s.multi_step_windows);
    printf("  NOTE: load magnitude and five-second duration belong only to the PC plant; ECM control logic is unchanged.\n");
    printf("  step-61 idle-load disturbance regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP61_CHECK
}
/* ---------------------------------------------------------------------- */
/* Step 62: A/C anticipation versus the same compressor-load disturbance.  */
/*                                                                        */
/* The ECM-derived part uses the Step-39 P/N A/C anticipation demand:     */
/* learned/default LC665=20 steps, then LC664=255/256 P/N scaling.        */
/* This gives 19 desired anticipation steps at closed throttle.           */
/*                                                                        */
/* The compressor load magnitude and the request-to-clutch delay below    */
/* are deliberately PC-plant stimuli.  They are NOT asserted as 9340      */
/* calibration values.  The comparison keeps those plant values identical */
/* and changes only whether the source-derived anticipation is enabled.   */
/* ---------------------------------------------------------------------- */
#define STEP62_REQUEST_SEC            60.0
#define STEP62_CLUTCH_SEC             61.0
#define STEP62_LOAD_END_SEC           66.0
#define STEP62_COMPRESSOR_LOAD_RPM   180.0
#define STEP62_AC_LEARNED_STEPS       20u
typedef struct BuaAcIdle62 {
    BuaClosedIdle60 idle;
    bua_u8 tf_steps;
    bua_u8 anticipate_enabled;
    bua_u8 ac_active;
    unsigned long anticipation_moves;
    unsigned long pid_bypassed_windows;
} BuaAcIdle62;
static void bua_ac_idle_init_step62(BuaAcIdle62 *s, bua_u8 anticipate_enabled)
{
    bua_closed_idle_init_step60(&s->idle);
    s->tf_steps=0u;
    s->anticipate_enabled=anticipate_enabled;
    s->ac_active=0u;
    s->anticipation_moves=0ul;
    s->pid_bypassed_windows=0ul;
}
/* One 6.25-ms service.  At each 50-ms boundary the Step-39 follower/load
   demand has priority while it is walking toward the A/C anticipate
   position.  That mirrors LD53C's explicit bypass of normal PID while
   anticipation steps are being taken; otherwise Step-59 PID supplies the
   persistent L0101 request. */
static void bua_ac_idle_minor_step62(BuaAcIdle62 *s, bua_u8 ac_requested,
                                     double load_rpm)
{
    BuaIacTfDesiredStep39 d;
    BuaIacTfRequestStep39 q;
    BuaIacCommandStep36 cmd;
    BuaIacReg59 reg;
    BuaIacMotorStep40 mot;
    unsigned int ar;
    double equilibrium,alpha;
    bua_u8 desired;
    bua_u8 use_anticipation;
    s->idle.coolant=bua_coolant_warmup_step57(s->idle.seconds);
    use_anticipation=0u;
    if(s->idle.minor_phase==0u) {
        if(s->idle.steps_this_window>1u) ++s->idle.multi_step_windows;
        if(s->idle.steps_this_window>s->idle.max_steps_in_window)
            s->idle.max_steps_in_window=s->idle.steps_this_window;
        s->idle.steps_this_window=0u;
        cmd=bua_iac_command_step36(s->idle.coolant,STEP36_IAC_PN_BIT,0u,0u,0u);
        s->idle.target_rpm=cmd.command_rpm;
        desired=0u;
        if(s->anticipate_enabled && ac_requested) {
            d=bua_iac_tf_desired_step39(0u,1u,0u,1u,STEP62_AC_LEARNED_STEPS,0u);
            desired=d.desired_steps;
        }
        if(desired!=s->tf_steps) {
            q=bua_iac_tf_request_step39(desired,s->tf_steps);
            s->idle.pending_request=q.request;
            use_anticipation=1u;
            ++s->pid_bypassed_windows;
            ++s->idle.request_writes;
        } else {
            ar=(unsigned int)(s->idle.rpm/12.5+0.5);
            if(ar>255u) ar=255u;
            reg=bua_iac_regulator_step59(cmd.command_raw,(bua_u8)ar,s->idle.old_rpm_raw,
                                         s->idle.coolant,s->idle.fractional,1u);
            s->idle.old_rpm_raw=(bua_u8)ar;
            s->idle.fractional=reg.fractional_after;
            s->idle.pending_request=reg.request;
            ++s->idle.request_writes;
        }
        ++s->idle.pid_passes;
    }
    mot=bua_iac_motor_service_step40(s->idle.pending_request,s->idle.iac_position,
                                     s->idle.running_counter,0u,0u,0u,
                                     s->idle.motor_on,1u);
    ++s->idle.motor_services;
    s->idle.pending_request=mot.command;
    s->idle.iac_position=mot.present_position;
    s->idle.running_counter=mot.running_counter;
    s->idle.motor_on=mot.motor_on;
    if(mot.consumed_step) {
        ++s->idle.consumed_steps;
        ++s->idle.steps_this_window;
        if(use_anticipation || (s->anticipate_enabled && ac_requested && s->tf_steps<19u)) {
            if(mot.opening && s->tf_steps<255u) ++s->tf_steps;
            else if(mot.closing && s->tf_steps>0u) --s->tf_steps;
            ++s->anticipation_moves;
        }
    }
    if(mot.physical_move) ++s->idle.physical_moves;
    if(mot.opening && mot.consumed_step) ++s->idle.open_steps;
    if(mot.closing && mot.consumed_step) ++s->idle.close_steps;
    equilibrium=STEP57_PLANT_BASE_RPM+STEP57_PLANT_RPM_PER_STEP*(double)s->idle.iac_position-load_rpm;
    alpha=STEP60_MINOR_DT_SEC/STEP57_PLANT_TAU_SEC;
    s->idle.rpm+=(equilibrium-s->idle.rpm)*alpha;
    if(s->idle.rpm<0.0) s->idle.rpm=0.0;
    s->idle.seconds+=STEP60_MINOR_DT_SEC;
    s->idle.minor_phase=(s->idle.minor_phase+1u)&7u;
    s->ac_active=ac_requested;
}
typedef struct BuaAcResult62 {
    double pre_rpm;
    double clutch_rpm;
    double min_rpm;
    double endload_rpm;
    double final_rpm;
    bua_u8 pre_iac;
    bua_u8 clutch_iac;
    bua_u8 peak_iac;
    bua_u8 final_iac;
    bua_u8 tf_at_clutch;
    unsigned long anticipation_moves;
    unsigned long pid_bypassed_windows;
    unsigned long motor_services;
    unsigned long pid_passes;
} BuaAcResult62;
static BuaAcResult62 bua_run_ac_case_step62(bua_u8 anticipate_enabled)
{
    BuaAcIdle62 s;
    BuaAcResult62 r;
    unsigned int i;
    bua_u8 request;
    double load;
    bua_ac_idle_init_step62(&s,anticipate_enabled);
    r.pre_rpm=0.0; r.clutch_rpm=0.0; r.min_rpm=10000.0; r.endload_rpm=0.0; r.final_rpm=0.0;
    r.pre_iac=0u; r.clutch_iac=0u; r.peak_iac=0u; r.final_iac=0u; r.tf_at_clutch=0u;
    r.anticipation_moves=0ul; r.pid_bypassed_windows=0ul; r.motor_services=0ul; r.pid_passes=0ul;
    for(i=0u;i<12800u;++i) { /* 80 sec */
        request=(s.idle.seconds>=STEP62_REQUEST_SEC && s.idle.seconds<STEP62_LOAD_END_SEC)?1u:0u;
        load=(s.idle.seconds>=STEP62_CLUTCH_SEC && s.idle.seconds<STEP62_LOAD_END_SEC)?STEP62_COMPRESSOR_LOAD_RPM:0.0;
        if(s.idle.seconds>=59.99 && s.idle.seconds<60.01) {
            r.pre_rpm=s.idle.rpm; r.pre_iac=s.idle.iac_position;
        }
        bua_ac_idle_minor_step62(&s,request,load);
        if(s.idle.seconds>=60.99 && s.idle.seconds<61.02) {
            r.clutch_rpm=s.idle.rpm; r.clutch_iac=s.idle.iac_position; r.tf_at_clutch=s.tf_steps;
        }
        if(s.idle.seconds>=STEP62_CLUTCH_SEC && s.idle.seconds<=STEP62_LOAD_END_SEC) {
            if(s.idle.rpm<r.min_rpm) r.min_rpm=s.idle.rpm;
            if(s.idle.iac_position>r.peak_iac) r.peak_iac=s.idle.iac_position;
        }
        if(s.idle.seconds>=65.99 && s.idle.seconds<66.02) r.endload_rpm=s.idle.rpm;
    }
    r.final_rpm=s.idle.rpm; r.final_iac=s.idle.iac_position;
    r.anticipation_moves=s.anticipation_moves;
    r.pid_bypassed_windows=s.pid_bypassed_windows;
    r.motor_services=s.idle.motor_services;
    r.pid_passes=s.idle.pid_passes;
    return r;
}
static void run_step62_ac_anticipation_test(void)
{
    BuaAcResult62 no,yes;
    BuaIacTfDesiredStep39 d;
    unsigned int passed=0u,total=18u;
#define STEP62_CHECK(c,t) do { if(c) ++passed; printf("  %-88s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-62 A/C anticipation versus compressor-load regression:\n");
    d=bua_iac_tf_desired_step39(0u,1u,0u,1u,STEP62_AC_LEARNED_STEPS,0u);
    no=bua_run_ac_case_step62(0u);
    yes=bua_run_ac_case_step62(1u);
    STEP62_CHECK(d.ac_steps==20u && d.desired_steps==19u,
                 "9340 Step-39 logic turns 20 learned A/C steps into 19 desired P/N steps");
    STEP62_CHECK(STEP62_CLUTCH_SEC-STEP62_REQUEST_SEC==1.0,
                 "one-second request-to-clutch interval is explicitly a PC comparison stimulus");
    STEP62_CHECK(STEP62_COMPRESSOR_LOAD_RPM==180.0,
                 "compressor load is explicitly a PC-only 180-RPM-equivalent plant disturbance");
    STEP62_CHECK(no.anticipation_moves==0ul && no.pid_bypassed_windows==0ul,
                 "control case applies identical compressor load without A/C anticipation");
    STEP62_CHECK(yes.anticipation_moves>=19ul,
                 "anticipated case physically walks the IAC toward the source-derived A/C demand");
    STEP62_CHECK(yes.tf_at_clutch>=18u,
                 "most/all of the 19-step P/N anticipation is present before compressor engagement");
    STEP62_CHECK(yes.clutch_iac>no.clutch_iac,
                 "anticipated case has more bypass-air position when the compressor load arrives");
    STEP62_CHECK(yes.clutch_rpm>no.clutch_rpm,
                 "anticipation raises pre-load RPM/airflow rather than waiting for the RPM sag");
    STEP62_CHECK(yes.min_rpm>no.min_rpm,
                 "source-derived A/C anticipation reduces the minimum RPM sag under identical load");
    STEP62_CHECK((yes.pre_rpm-yes.min_rpm)<(no.pre_rpm-no.min_rpm),
                 "anticipated case has a smaller peak RPM drop from its own pre-request idle");
    STEP62_CHECK(yes.pid_bypassed_windows>0ul,
                 "normal PID is bypassed while the anticipation position is being walked");
    STEP62_CHECK(no.motor_services==12800ul && yes.motor_services==12800ul,
                 "both 80-second cases execute exactly 12800 6.25-ms IAC services");
    STEP62_CHECK(no.pid_passes==1600ul && yes.pid_passes==1600ul,
                 "both cases retain exactly 1600 50-ms regulator scheduling opportunities");
    STEP62_CHECK(no.motor_services==no.pid_passes*8ul && yes.motor_services==yes.pid_passes*8ul,
                 "A/C comparison preserves the executable-derived 8:1 motor/control cadence");
    STEP62_CHECK(no.min_rpm<no.pre_rpm-50.0,
                 "unanticipated compressor load produces a clearly measurable idle sag");
    STEP62_CHECK(yes.final_rpm>525.0 && yes.final_rpm<675.0,
                 "anticipated case returns to the calibrated P/N idle deadband after A/C removal");
    STEP62_CHECK(no.final_rpm>525.0 && no.final_rpm<675.0,
                 "unanticipated control case also recovers after the identical load is removed");
    STEP62_CHECK(yes.peak_iac>yes.pre_iac && no.peak_iac>no.pre_iac,
                 "both cases retain closed-loop corrective authority after compressor engagement");
    printf("  no anticipation: pre=%0.1f clutch=%0.1f min=%0.1f endLoad=%0.1f final=%0.1f IAC %u/%u/%u\n",
           no.pre_rpm,no.clutch_rpm,no.min_rpm,no.endload_rpm,no.final_rpm,
           (unsigned int)no.pre_iac,(unsigned int)no.peak_iac,(unsigned int)no.final_iac);
    printf("  with anticipation: pre=%0.1f clutch=%0.1f min=%0.1f endLoad=%0.1f final=%0.1f IAC %u/%u/%u TF@clutch=%u\n",
           yes.pre_rpm,yes.clutch_rpm,yes.min_rpm,yes.endload_rpm,yes.final_rpm,
           (unsigned int)yes.pre_iac,(unsigned int)yes.peak_iac,(unsigned int)yes.final_iac,
           (unsigned int)yes.tf_at_clutch);
    printf("  improvement: minimum RPM +%0.1f; sag reduction=%0.1f RPM; anticipation moves=%lu; PID-bypass windows=%lu\n",
           yes.min_rpm-no.min_rpm,(no.pre_rpm-no.min_rpm)-(yes.pre_rpm-yes.min_rpm),
           yes.anticipation_moves,yes.pid_bypassed_windows);
    printf("  NOTE: request/clutch timing and compressor-load magnitude are PC-plant stimuli; the 20->19 P/N anticipation is source-derived.\n");
    printf("  step-62 A/C anticipation regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP62_CHECK
}
/* ---------------------------------------------------------------------- */
/* Step 63: complete A/C load-removal cycle.                               */
/*                                                                        */
/* LE984 marks b5 when A/C disappears after b6 says the load had been     */
/* controlled.  LE9A4..LE9C5 then walks the follower/load position back  */
/* toward zero through packed L0101 requests.  The PC plant keeps the     */
/* same 180-RPM-equivalent compressor load used by Step 62.               */
/* ---------------------------------------------------------------------- */
#define STEP63_REQUEST_SEC           60.0
#define STEP63_CLUTCH_SEC            61.0
#define STEP63_UNLOAD_SEC            66.0
#define STEP63_END_SEC               76.0
typedef struct BuaAcCycle63 {
    BuaAcIdle62 ac;
    bua_u8 control_word;
    bua_u8 tf_motion_active;
    bua_u8 tf_motion_opening;
    bua_u8 previous_request;
    bua_u8 pid_disable_timer;
    unsigned long removal_started;
    unsigned long removal_moves;
    unsigned long release_delay_loads;
} BuaAcCycle63;
static void bua_ac_cycle_init_step63(BuaAcCycle63 *s)
{
    bua_ac_idle_init_step62(&s->ac,1u);
    s->control_word=0u;
    s->tf_motion_active=0u;
    s->tf_motion_opening=0u;
    s->previous_request=0u;
    s->pid_disable_timer=0u;
    s->removal_started=0ul;
    s->removal_moves=0ul;
    s->release_delay_loads=0ul;
}
static void bua_ac_cycle_minor_step63(BuaAcCycle63 *s,bua_u8 ac_requested,double load_rpm)
{
    BuaIacTfDesiredStep39 d;
    BuaIacTfRequestStep39 q;
    BuaIacAcStateStep39 rm;
    BuaIacCommandStep36 cmd;
    BuaIacReg59 reg;
    BuaIacMotorStep40 mot;
    unsigned int ar;
    double equilibrium,alpha;
    bua_u8 desired;
    s->ac.idle.coolant=bua_coolant_warmup_step57(s->ac.idle.seconds);
    if(ac_requested) s->control_word=(bua_u8)(s->control_word|STEP39_IAC_LOAD_CONTROL_BIT);
    if(!ac_requested && s->previous_request) {
        rm=bua_iac_ac_remove_step39(s->control_word,0u);
        s->control_word=rm.control_word;
        if(rm.removal_started) ++s->removal_started;
    }
    if(s->ac.idle.minor_phase==0u) {
        if(s->ac.idle.steps_this_window>1u) ++s->ac.idle.multi_step_windows;
        if(s->ac.idle.steps_this_window>s->ac.idle.max_steps_in_window)
            s->ac.idle.max_steps_in_window=s->ac.idle.steps_this_window;
        s->ac.idle.steps_this_window=0u;
        cmd=bua_iac_command_step36(s->ac.idle.coolant,STEP36_IAC_PN_BIT,0u,0u,0u);
        s->ac.idle.target_rpm=cmd.command_rpm;
        desired=0u;
        if(ac_requested) {
            d=bua_iac_tf_desired_step39(0u,1u,0u,1u,STEP62_AC_LEARNED_STEPS,0u);
            desired=d.desired_steps;
        }
        if(desired!=s->ac.tf_steps) {
            q=bua_iac_tf_request_step39(desired,s->ac.tf_steps);
            s->ac.idle.pending_request=q.request;
            s->tf_motion_active=1u;
            s->tf_motion_opening=q.opening;
            ++s->ac.pid_bypassed_windows;
            ++s->ac.idle.request_writes;
            if(q.pid_disable_timer!=0u) {
                s->pid_disable_timer=q.pid_disable_timer;
                ++s->release_delay_loads;
            }
        } else if(s->pid_disable_timer!=0u) {
            --s->pid_disable_timer;
            s->ac.idle.pending_request=0u;
            ++s->ac.pid_bypassed_windows;
        } else {
            ar=(unsigned int)(s->ac.idle.rpm/12.5+0.5); if(ar>255u) ar=255u;
            reg=bua_iac_regulator_step59(cmd.command_raw,(bua_u8)ar,s->ac.idle.old_rpm_raw,
                                         s->ac.idle.coolant,s->ac.idle.fractional,1u);
            s->ac.idle.old_rpm_raw=(bua_u8)ar;
            s->ac.idle.fractional=reg.fractional_after;
            s->ac.idle.pending_request=reg.request;
            s->tf_motion_active=0u;
            ++s->ac.idle.request_writes;
        }
        ++s->ac.idle.pid_passes;
    }
    mot=bua_iac_motor_service_step40(s->ac.idle.pending_request,s->ac.idle.iac_position,
                                     s->ac.idle.running_counter,0u,0u,0u,s->ac.idle.motor_on,1u);
    ++s->ac.idle.motor_services;
    s->ac.idle.pending_request=mot.command;
    s->ac.idle.iac_position=mot.present_position;
    s->ac.idle.running_counter=mot.running_counter;
    s->ac.idle.motor_on=mot.motor_on;
    if(mot.consumed_step) {
        ++s->ac.idle.consumed_steps; ++s->ac.idle.steps_this_window;
        if(s->tf_motion_active) {
            if(s->tf_motion_opening) { if(s->ac.tf_steps<255u) ++s->ac.tf_steps; }
            else { if(s->ac.tf_steps>0u) --s->ac.tf_steps; ++s->removal_moves; }
            ++s->ac.anticipation_moves;
            if(s->ac.idle.pending_request==0u) s->tf_motion_active=0u;
        }
    }
    if(mot.physical_move) ++s->ac.idle.physical_moves;
    if(mot.opening && mot.consumed_step) ++s->ac.idle.open_steps;
    if(mot.closing && mot.consumed_step) ++s->ac.idle.close_steps;
    equilibrium=STEP57_PLANT_BASE_RPM+STEP57_PLANT_RPM_PER_STEP*(double)s->ac.idle.iac_position-load_rpm;
    alpha=STEP60_MINOR_DT_SEC/STEP57_PLANT_TAU_SEC;
    s->ac.idle.rpm+=(equilibrium-s->ac.idle.rpm)*alpha;
    if(s->ac.idle.rpm<0.0) s->ac.idle.rpm=0.0;
    s->ac.idle.seconds+=STEP60_MINOR_DT_SEC;
    s->ac.idle.minor_phase=(s->ac.idle.minor_phase+1u)&7u;
    s->previous_request=ac_requested;
}
static void run_step63_ac_removal_cycle_test(void)
{
    BuaAcCycle63 s;
    unsigned int i,passed=0u,total=20u;
    bua_u8 req;
    double load;
    double pre=0.0,clutch=0.0,minload=10000.0,preunload=0.0,peakafter=0.0,one_sec=0.0,final=0.0;
    bua_u8 tf_preunload=0u,tf_one_sec=0u,tf_final=0u,iac_preunload=0u,iac_final=0u;
#define STEP63_CHECK(c,t) do { if(c) ++passed; printf("  %-88s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-63 complete A/C anticipation / unload / removal regression:\n");
    bua_ac_cycle_init_step63(&s);
    for(i=0u;i<(unsigned int)(STEP63_END_SEC/STEP60_MINOR_DT_SEC);++i) {
        req=(s.ac.idle.seconds>=STEP63_REQUEST_SEC && s.ac.idle.seconds<STEP63_UNLOAD_SEC)?1u:0u;
        load=(s.ac.idle.seconds>=STEP63_CLUTCH_SEC && s.ac.idle.seconds<STEP63_UNLOAD_SEC)?STEP62_COMPRESSOR_LOAD_RPM:0.0;
        if(s.ac.idle.seconds>=59.99 && s.ac.idle.seconds<60.01) pre=s.ac.idle.rpm;
        bua_ac_cycle_minor_step63(&s,req,load);
        if(s.ac.idle.seconds>=60.99 && s.ac.idle.seconds<61.02) clutch=s.ac.idle.rpm;
        if(s.ac.idle.seconds>=STEP63_CLUTCH_SEC && s.ac.idle.seconds<STEP63_UNLOAD_SEC && s.ac.idle.rpm<minload) minload=s.ac.idle.rpm;
        if(s.ac.idle.seconds>=65.94 && s.ac.idle.seconds<65.96) {
            preunload=s.ac.idle.rpm; tf_preunload=s.ac.tf_steps; iac_preunload=s.ac.idle.iac_position; peakafter=s.ac.idle.rpm;
        }
        if(s.ac.idle.seconds>=STEP63_UNLOAD_SEC && s.ac.idle.seconds<STEP63_UNLOAD_SEC+2.0 && s.ac.idle.rpm>peakafter) peakafter=s.ac.idle.rpm;
        if(s.ac.idle.seconds>=66.99 && s.ac.idle.seconds<67.02) { one_sec=s.ac.idle.rpm; tf_one_sec=s.ac.tf_steps; }
    }
    final=s.ac.idle.rpm; tf_final=s.ac.tf_steps; iac_final=s.ac.idle.iac_position;
    STEP63_CHECK(pre>525.0 && pre<700.0,"warmup trajectory begins the A/C cycle near the calibrated P/N idle-control region");
    STEP63_CHECK(clutch>pre,"source-derived anticipation raises airflow/RPM before compressor engagement");
    STEP63_CHECK(minload>525.0,"anticipated compressor load keeps minimum RPM inside the P/N deadband");
    STEP63_CHECK(tf_preunload>=18u,"A/C follower/load term is still essentially fully established immediately before unload");
    STEP63_CHECK(s.removal_started==1ul,"A/C falling edge invokes LE984-style load-removal state exactly once");
    STEP63_CHECK((s.control_word&STEP39_IAC_LOAD_REMOVE_BIT)!=0u,"load-removal b5 is carried in the integrated IAC control word");
    STEP63_CHECK(s.removal_moves>=18ul,"6.25-ms motor executor physically removes essentially all anticipated A/C steps");
    STEP63_CHECK(tf_one_sec==0u,"anticipation position has returned to zero within one second of compressor unload");
    STEP63_CHECK(tf_final==0u,"A/C follower/load position remains zero after removal completes");
    STEP63_CHECK(peakafter<preunload+100.0,"coordinated anticipation removal limits the immediate compressor-unload RPM flare");
    STEP63_CHECK(one_sec>525.0 && one_sec<700.0,"one second after unload RPM remains near the normal idle-control region");
    STEP63_CHECK(final>525.0 && final<675.0,"normal PID takeover returns final RPM to the calibrated P/N deadband");
    STEP63_CHECK(iac_final<iac_preunload,"final IAC position unwinds the extra bypass air required by the compressor load");
    STEP63_CHECK(s.ac.pid_bypassed_windows>0ul,"normal PID is bypassed while anticipation/load-removal motion is active");
    STEP63_CHECK(s.ac.idle.motor_services==12160ul,"76-second cycle executes exactly 12160 6.25-ms motor services");
    STEP63_CHECK(s.ac.idle.pid_passes==1520ul,"76-second cycle executes exactly 1520 50-ms control opportunities");
    STEP63_CHECK(s.ac.idle.motor_services==s.ac.idle.pid_passes*8ul,"complete A/C cycle preserves the executable-derived 8:1 motor/control cadence");
    STEP63_CHECK(s.ac.idle.iac_position>0u && s.ac.idle.iac_position<145u,"IAC remains inside modeled travel throughout final recovery");
    STEP63_CHECK(s.ac.idle.fractional>-8 && s.ac.idle.fractional<8,"final L0103 fractional residue remains sub-quantum");
    STEP63_CHECK(s.ac.idle.open_steps>0ul && s.ac.idle.close_steps>0ul,"complete A/C cycle exercises both physical IAC directions");
    printf("  RPM: pre=%0.1f clutch=%0.1f min-load=%0.1f pre-unload=%0.1f peak-after=%0.1f +1s=%0.1f final=%0.1f\n",pre,clutch,minload,preunload,peakafter,one_sec,final);
    printf("  IAC: pre-unload=%u final=%u; TF %u -> %u -> %u; removal moves=%lu; removal starts=%lu\n",
           (unsigned int)iac_preunload,(unsigned int)iac_final,(unsigned int)tf_preunload,
           (unsigned int)tf_one_sec,(unsigned int)tf_final,s.removal_moves,s.removal_started);
    printf("  NOTE: compressor magnitude/timing remain PC-plant stimuli; load-control/removal flags and follower requests are source-derived.\n");
    printf("  step-63 A/C removal-cycle regression result: %s (%u/%u)\n",(passed==total)?"PASS":"FAIL",passed,total);
#undef STEP63_CHECK
}
/* ---------------------------------------------------------------------- */
/* Step 65: exact power-steering pressure-switch qualification at LCBFE..LCC21. */
#define STEP65_PS_TPS_UPPER_9340 255u
#define STEP65_PS_TPS_LOWER_9340 255u
#define STEP65_PS_VSS_LIMIT_9340 8u
typedef struct { bua_u8 status_b3; bua_u8 switch_read; } BuaPsGate65;
static BuaPsGate65 bua_ps_gate_step65(bua_u8 old_status_b3,bua_u8 ps_select,bua_u8 tps,bua_u8 vss,bua_u8 raw_l002e,bua_u8 upper,bua_u8 lower,bua_u8 vss_limit)
{
    BuaPsGate65 r; bua_u8 b3=(bua_u8)(old_status_b3 & 0x08u); r.switch_read=0u;
    if (ps_select==0u) b3=0u;
    if (tps < upper) { r.status_b3=b3; return r; }
    if (tps >= lower) { r.status_b3=b3; return r; }
    if (vss > vss_limit) { r.status_b3=b3; return r; }
    r.switch_read=1u; b3=(bua_u8)((~raw_l002e) & 0x08u); r.status_b3=b3; return r;
}
static void run_step65_power_steering_gate_test(void)
{
    unsigned int pass=0u,total=0u; BuaPsGate65 r;
#define S65(c) do { ++total; if (c) ++pass; else printf("Step-65 FAIL line %d: %s\n",__LINE__,#c); } while(0)
    printf("\nStep-65 exact power-steering switch qualification test\n");
    r=bua_ps_gate_step65(0x08u,0u,0u,0u,0u,255u,255u,8u); S65(r.switch_read==0u); S65(r.status_b3==0u);
    r=bua_ps_gate_step65(0u,0u,254u,0u,0u,255u,255u,8u); S65(r.switch_read==0u);
    r=bua_ps_gate_step65(0u,0u,255u,0u,0u,255u,255u,8u); S65(r.switch_read==0u);
    { unsigned int t,reads=0u; for(t=0u;t<256u;++t) { r=bua_ps_gate_step65(0u,0u,(bua_u8)t,0u,0u,STEP65_PS_TPS_UPPER_9340,STEP65_PS_TPS_LOWER_9340,STEP65_PS_VSS_LIMIT_9340); reads+=(unsigned int)r.switch_read; } S65(reads==0u); printf("  9340 TPS sweep: switch reads=%u of 256 possible TPS codes\n",reads); }
    r=bua_ps_gate_step65(0u,0u,120u,5u,0x00u,100u,140u,8u); S65(r.switch_read==1u); S65(r.status_b3==0x08u);
    r=bua_ps_gate_step65(0u,0u,120u,5u,0x08u,100u,140u,8u); S65(r.switch_read==1u); S65(r.status_b3==0u);
    r=bua_ps_gate_step65(0u,0u,120u,9u,0x00u,100u,140u,8u); S65(r.switch_read==0u);
    r=bua_ps_gate_step65(0x08u,1u,0u,0u,0x08u,255u,255u,8u); S65(r.switch_read==0u); S65(r.status_b3==0x08u);
    printf("  9340 LC67D/LC67E = 255/255: physical P/S switch sampling is disabled.\n");
    printf("  9340 LC680/LC681 = 0/0: anticipation additions are also zero.\n");
    printf("Step-65 regression: PASS %u/%u\n",pass,total);
#undef S65
}
/* Step 64: power-steering anticipation capability versus 9340 calibration. */
/*                                                                        */
/* LE944 adds the normal P/S anticipation scalar when the pressure-switch  */
/* input is active, then LE950 can add a second scalar when A/C is not in  */
/* the state selected by the IAC control-word sign test.  In the 9340     */
/* image both calibration bytes are zero (LC680=0, LC681=0).  Keep the    */
/* algorithm visible, but do not invent an active Corvette calibration.   */
/* ---------------------------------------------------------------------- */
#define STEP64_PS_CAL_NORMAL_9340  0u
#define STEP64_PS_CAL_AC_9340      0u
#define STEP64_PS_DEMO_NORMAL     12u
#define STEP64_PS_DEMO_AC          4u
typedef struct BuaPsAnt64 {
    bua_u8 desired_steps;
    bua_u8 control_word;
    bua_u8 added_normal;
    bua_u8 added_ac;
} BuaPsAnt64;
static BuaPsAnt64 bua_ps_anticipation_step64(bua_u8 base_steps,bua_u8 pressure_high,
                                             bua_u8 ac_state_selects_extra,
                                             bua_u8 cal_normal,bua_u8 cal_ac,
                                             bua_u8 control_word)
{
    BuaPsAnt64 r;
    unsigned int v;
    r.desired_steps=base_steps;
    r.control_word=control_word;
    r.added_normal=0u;
    r.added_ac=0u;
    if(pressure_high) {
        v=(unsigned int)r.desired_steps+(unsigned int)cal_normal;
        if(v>255u) v=255u;
        r.desired_steps=(bua_u8)v;
        r.added_normal=cal_normal;
        if(ac_state_selects_extra) {
            v=(unsigned int)r.desired_steps+(unsigned int)cal_ac;
            if(v>255u) v=255u;
            r.desired_steps=(bua_u8)v;
            r.added_ac=cal_ac;
        }
        r.control_word=(bua_u8)((r.control_word|STEP39_IAC_LOAD_CONTROL_BIT)&(bua_u8)~0x01u);
    }
    return r;
}
static void run_step64_power_steering_test(void)
{
    BuaPsAnt64 off,on9340,demo,demo_ac,sat;
    unsigned int passed=0u,total=14u;
#define STEP64_CHECK(c,t) do { if(c) ++passed; printf("  %-88s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-64 power-steering anticipation / calibration-enable regression:\n");
    off=bua_ps_anticipation_step64(25u,0u,1u,STEP64_PS_CAL_NORMAL_9340,STEP64_PS_CAL_AC_9340,0u);
    on9340=bua_ps_anticipation_step64(25u,1u,1u,STEP64_PS_CAL_NORMAL_9340,STEP64_PS_CAL_AC_9340,0u);
    demo=bua_ps_anticipation_step64(25u,1u,0u,STEP64_PS_DEMO_NORMAL,STEP64_PS_DEMO_AC,0u);
    demo_ac=bua_ps_anticipation_step64(25u,1u,1u,STEP64_PS_DEMO_NORMAL,STEP64_PS_DEMO_AC,0u);
    sat=bua_ps_anticipation_step64(250u,1u,1u,12u,8u,0u);
    STEP64_CHECK(STEP64_PS_CAL_NORMAL_9340==0u,"9340 LC680 normal power-steering anticipation calibration is zero");
    STEP64_CHECK(STEP64_PS_CAL_AC_9340==0u,"9340 LC681 A/C-associated power-steering anticipation calibration is zero");
    STEP64_CHECK(off.desired_steps==25u,"inactive pressure switch leaves the follower/load demand unchanged");
    STEP64_CHECK(on9340.desired_steps==25u,"active pressure switch still adds zero steps with the actual 9340 calibration");
    STEP64_CHECK((on9340.control_word&STEP39_IAC_LOAD_CONTROL_BIT)!=0u,"executable path still marks load-control state when the pressure input is active");
    STEP64_CHECK(on9340.added_normal==0u && on9340.added_ac==0u,"actual calibration contributes no hidden P/S airflow despite executing the feature path");
    STEP64_CHECK(demo.desired_steps==37u,"hypothetical nonzero LC680 proves the normal P/S anticipation arithmetic remains functional");
    STEP64_CHECK(demo_ac.desired_steps==41u,"hypothetical nonzero LC681 adds its second calibrated contribution on the selected A/C state");
    STEP64_CHECK(demo_ac.desired_steps-demo.desired_steps==STEP64_PS_DEMO_AC,"second hypothetical contribution is exactly the supplied calibration byte");
    STEP64_CHECK(sat.desired_steps==255u,"P/S anticipation additions saturate at 255 rather than wrapping");
    STEP64_CHECK((demo.control_word&STEP39_IAC_LOAD_CONTROL_BIT)!=0u,"nonzero demonstration uses the same load-control flag as the zero-calibration baseline");
    STEP64_CHECK((demo.control_word&0x01u)==0u,"pressure-switch path clears IAC control-word bit 0 as LE95B specifies");
    STEP64_CHECK(on9340.desired_steps==off.desired_steps,"9340 behavior is numerically identical with pressure switch low or high in this isolated path");
    STEP64_CHECK(demo_ac.desired_steps>on9340.desired_steps,"algorithm capability is demonstrably distinct from what the 9340 calibration enables");
    printf("  9340: base=25, P/S low=%u, P/S high=%u (LC680=%u LC681=%u)\n",
           (unsigned int)off.desired_steps,(unsigned int)on9340.desired_steps,
           (unsigned int)STEP64_PS_CAL_NORMAL_9340,(unsigned int)STEP64_PS_CAL_AC_9340);
    printf("  hypothetical calibration only: normal=%u, selected-A/C=%u; this is NOT a 9340/Corvette calibration claim.\n",
           (unsigned int)demo.desired_steps,(unsigned int)demo_ac.desired_steps);
    printf("  step-64 power-steering regression result: %s (%u/%u)\n",(passed==total)?"PASS":"FAIL",passed,total);
#undef STEP64_CHECK
}
static void run_step47_afr_selection_test(void)
{
    BuaMemory saved_mem;
    BuaStats saved_stats;
    BuaAfrSelectResult r;
    unsigned int passed;
    unsigned int total;
    saved_mem = mem;
    saved_stats = stats;
    passed = 0u;
    total = 0u;
    printf("\nStep-47 integrated AFR selection / PE interaction (LD9A9..LDA5A):\n");
#define STEP47_CHECK(cond, text) do { ++total; if (cond) ++passed; \
    printf("  %-68s %s\n", text, (cond) ? "PASS" : "FAIL"); } while (0)
    r = bua_select_afr_step47(96u, 49u, 200u, 160u, 0u,
                              AF_CLOSED_LOOP_BIT, 25u, 0u, 1u, 0u, 0u, 0u);
    STEP47_CHECK(r.source == 0u && r.afr.afr_code == 445u &&
                 (r.af_mode_word & AF_POWER_ENRICH_BIT) == 0u && r.single_fire == 1u,
                 "below PE load -> normal closed-loop stoich; single-fire unchanged");
    r = bua_select_afr_step47(96u, 50u, 180u, 160u, 0u,
                              AF_CLOSED_LOOP_BIT, 25u, 0u, 1u, 0u, 0u, 0u);
    STEP47_CHECK(r.source == 1u && r.afr.afr_code == 545u &&
                 (r.af_mode_word & AF_POWER_ENRICH_BIT) != 0u && r.single_fire == 0u,
                 "exact PE thresholds -> PE code 545 and single-fire cleared");
    r = bua_select_afr_step47(96u, 40u, 164u, 160u, 0u,
                              AF_CLOSED_LOOP_BIT, 25u, AF_POWER_ENRICH_BIT,
                              1u, 0u, 0u, 0u);
    STEP47_CHECK(r.source == 1u && (r.af_mode_word & AF_POWER_ENRICH_BIT) != 0u,
                 "prior PE uses TPS/load hysteresis (164 / 40 remains active)");
    r = bua_select_afr_step47(96u, 39u, 200u, 160u, 0u,
                              AF_CLOSED_LOOP_BIT, 25u, AF_POWER_ENRICH_BIT,
                              1u, 0u, 0u, 0u);
    STEP47_CHECK(r.source == 0u && (r.af_mode_word & AF_POWER_ENRICH_BIT) == 0u &&
                 r.single_fire == 1u,
                 "prior PE below hysteresis load clears PE; does not force single-fire");
    r = bua_select_afr_step47(96u, 80u, 200u, 0u, 0u,
                              AF_CLOSED_LOOP_BIT, 25u, 0u, 1u, 0u, 0u, 0u);
    STEP47_CHECK(r.source == 1u && r.afr.afr_code == 688u,
                 "cold PE coolant term produces rich code 688 (~9.53:1)");
    r = bua_select_afr_step47(96u, 80u, 200u, 160u, 0u,
                              0u, 25u, 0u, 1u, 0u, 0u, 0u);
    STEP47_CHECK(r.source == 1u && r.afr.afr_code == 545u,
                 "PE overrides normal open-loop AFR construction");
    /* Use filtered load 128 for the known Step-25 open-loop anchor. */
    r = bua_select_afr_step47(96u, 128u, 100u, 160u, 0u,
                              0u, 25u, 0u, 1u, 0u, 0u, 0u);
    STEP47_CHECK(r.source == 0u && r.afr.afr_code == 450u,
                 "normal LDA2D fallback preserves warm open-loop code 450");
    r = bua_select_afr_step47(96u, 80u, 200u, 160u, 0u,
                              AF_CLOSED_LOOP_BIT, 25u, 0u, 1u,
                              STEP47_MODE4_ACTIVE_BIT, STEP47_MODE4_AFR_ENABLE_BIT, 128u);
    STEP47_CHECK(r.source == 2u && r.afr.afr_code == 511u && r.single_fire == 1u,
                 "Mode 4 AFR override has priority over PE: 65535/128 = 511");
    r = bua_select_afr_step47(96u, 80u, 200u, 160u, 0u,
                              AF_CLOSED_LOOP_BIT, 25u, AF_POWER_ENRICH_BIT, 1u,
                              STEP47_MODE4_ACTIVE_BIT, STEP47_MODE4_AFR_ENABLE_BIT, 128u);
    STEP47_CHECK(r.source == 2u && (r.af_mode_word & AF_POWER_ENRICH_BIT) != 0u,
                 "Mode 4 bypass leaves pre-existing PE state bit untouched this pass");
    r = bua_select_afr_step47(96u, 80u, 200u, 160u, 0u,
                              AF_CLOSED_LOOP_BIT, 25u, 0u, 1u,
                              STEP47_MODE4_ACTIVE_BIT, 0u, 128u);
    STEP47_CHECK(r.source == 1u && r.afr.afr_code == 545u,
                 "Mode 4 active without AFR-enable bit falls through to PE");
    STEP47_CHECK(bua_mode4_afr_code_step47(255u) == 257u,
                 "Mode 4 raw AFR 255 -> reciprocal code 257");
    STEP47_CHECK(bua_mode4_afr_code_step47(1u) == 65535u,
                 "Mode 4 raw AFR 1 -> reciprocal code 65535");
    /* DFCO is downstream: AFR can be PE-rich while LF92A still zeros fuel. */
    r = bua_select_afr_step47(96u, 80u, 200u, 160u, 0u,
                              AF_CLOSED_LOOP_BIT, 25u, FUEL_DFCO_ZERO_BIT,
                              1u, 0u, 0u, 0u);
    STEP47_CHECK(r.source == 1u && r.afr.afr_code == 545u,
                 "DFCO bit does not inhibit AFR/PE calculation (fuel cut is downstream)");
#undef STEP47_CHECK
    printf("  step-47 AFR-selection regression result: %s (%u/%u)\n",
           (passed == total) ? "PASS" : "FAIL", passed, total);
    mem = saved_mem;
    stats = saved_stats;
}
static void run_step46_dfco_output_test(void)
{
    BuaFuelGate46 g;
    BuaStallAsync46 a;
    BuaFollower46 f;
    unsigned int passed=0u;
    unsigned int total=13u;
    printf("\nStep-46 DFCO fuel suppression / stall-saver output regression:\n");
    g=bua_lf92a_dfco_gate_step46(500u,100u,STEP46_VATS_OK_BIT,0u,0u);
    if (g.base_pw==500u && g.accel_accum==100u) ++passed;
    printf("  normal LF92A gate preserves ordinary and accumulated fuel           %s\n",
           (g.base_pw==500u && g.accel_accum==100u)?"PASS":"FAIL");
    g=bua_lf92a_dfco_gate_step46(500u,100u,STEP46_VATS_OK_BIT,0u,STEP46_DFCO_BIT);
    if (g.base_pw==0u && g.accel_accum==0u) ++passed;
    printf("  DFCO flag zeros base PW and accumulated accel fuel                  %s\n",
           (g.base_pw==0u && g.accel_accum==0u)?"PASS":"FAIL");
    g=bua_lf92a_dfco_gate_step46(500u,100u,0u,0u,0u);
    if (g.base_pw==0u && g.accel_accum==0u) ++passed;
    printf("  failed/not-passed VATS uses the same zero-fuel branch               %s\n",
           (g.base_pw==0u && g.accel_accum==0u)?"PASS":"FAIL");
    g=bua_lf92a_dfco_gate_step46(500u,100u,STEP46_VATS_OK_BIT,STEP46_IGN_OFF_BIT,0u);
    if (g.base_pw==0u && g.accel_accum==0u) ++passed;
    printf("  ignition-off uses the same zero-fuel branch                         %s\n",
           (g.base_pw==0u && g.accel_accum==0u)?"PASS":"FAIL");
    a=bua_stall_async_step46(0u,0u,STEP46_STALL_SAVE_BIT,1u);
    if (a.new_async_pw==256u && a.accel_accum==256u && a.pulses_left==0u) ++passed;
    printf("  one stall-save pulse adds LC605=256 and consumes LC607=1            %s\n",
           (a.new_async_pw==256u && a.accel_accum==256u && a.pulses_left==0u)?"PASS":"FAIL");
    a=bua_stall_async_step46(100u,50u,STEP46_STALL_SAVE_BIT,1u);
    if (a.new_async_pw==356u && a.accel_accum==406u) ++passed;
    printf("  stall PW adds to an existing async pulse, then into accumulator     %s\n",
           (a.new_async_pw==356u && a.accel_accum==406u)?"PASS":"FAIL");
    a=bua_stall_async_step46(65500u,1000u,STEP46_STALL_SAVE_BIT,1u);
    if (a.new_async_pw==220u && a.accel_accum==1220u) ++passed;
    printf("  LC605 add wraps first: 65500+256=220; then accumulator becomes1220 %s\n",
           (a.new_async_pw==220u && a.accel_accum==1220u)?"PASS":"FAIL");
    a=bua_stall_async_step46(100u,50u,0u,1u);
    if (a.new_async_pw==100u && a.accel_accum==50u && a.pulses_left==1u) ++passed;
    printf("  without L003D b4 the stall-save pulse is not consumed               %s\n",
           (a.new_async_pw==100u && a.accel_accum==50u && a.pulses_left==1u)?"PASS":"FAIL");
    f=bua_dfco_follower_step46(20u,STEP46_STALL_SAVE_BIT,30u,0u);
    if (f.virtual_tps==28u && f.duration_left==29u) ++passed;
    printf("  stall saver adds LC603=8 virtual TPS and decrements duration        %s\n",
           (f.virtual_tps==28u && f.duration_left==29u)?"PASS":"FAIL");
    f=bua_dfco_follower_step46(20u,0u,30u,STEP46_DFCO_BIT);
    if (f.virtual_tps==30u && f.duration_left==30u) ++passed;
    printf("  active DFCO adds LC60A=10 virtual TPS without consuming stall timer %s\n",
           (f.virtual_tps==30u && f.duration_left==30u)?"PASS":"FAIL");
    f=bua_dfco_follower_step46(20u,STEP46_STALL_SAVE_BIT,30u,STEP46_DFCO_BIT);
    if (f.virtual_tps==38u && f.duration_left==29u) ++passed;
    printf("  executable permits both virtual-TPS additions to accumulate         %s\n",
           (f.virtual_tps==38u && f.duration_left==29u)?"PASS":"FAIL");
    f=bua_dfco_follower_step46(252u,STEP46_STALL_SAVE_BIT,1u,STEP46_DFCO_BIT);
    if (f.virtual_tps==255u && f.duration_left==0u) ++passed;
    printf("  follower additions saturate at 255                                  %s\n",
           (f.virtual_tps==255u && f.duration_left==0u)?"PASS":"FAIL");
    /* End-to-end state consequence: DFCO active kills ordinary fuel; an
       exit-stall state can subsequently generate one explicit async pulse. */
    g=bua_lf92a_dfco_gate_step46(500u,80u,STEP46_VATS_OK_BIT,0u,STEP46_DFCO_BIT);
    a=bua_stall_async_step46(0u,0u,STEP46_STALL_SAVE_BIT,1u);
    if (g.base_pw==0u && g.accel_accum==0u && a.new_async_pw==256u && a.pulses_left==0u) ++passed;
    printf("  DFCO zero-fuel state and post-exit one-pulse recovery connect       %s\n",
           (g.base_pw==0u && g.accel_accum==0u && a.new_async_pw==256u && a.pulses_left==0u)?"PASS":"FAIL");
    printf("  step-46 regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
}
/* Step 67: begin 700R4 TCC reconstruction; retain Step-66 IAC tail
 * from LE9A4..LEA25.  This keeps the source's 8-bit subtract/borrow behavior,
 * signed closing encoding in L0101, the 127-step saturation, and the final
 * stopped/moving/TPS/RPM state decisions separate from the physical motor. */
typedef struct BuaTfTail66 {
    bua_u8 request;
    bua_u8 control;
    bua_u8 pid_disable;
} BuaTfTail66;
static bua_u8 bua_tf_pack_request_step66(bua_u8 desired,bua_u8 current,bua_u8 *small_close)
{
    unsigned int d;
    bua_u8 a;
    if (small_close != 0) *small_close=0u;
    if (desired >= current) {
        d=(unsigned int)desired-(unsigned int)current;
        if (d>127u) d=127u;
        return (bua_u8)d;
    }
    a=(bua_u8)(desired-current); /* exact 8-bit SUBA result after borrow */
    if ((a & 0x80u)==0u) a=0x81u; /* source's large-negative saturation case */
    a=(bua_u8)(0u-a);
    if (a<3u && small_close != 0) *small_close=1u;
    return (bua_u8)(a | 0x80u);
}
static BuaTfTail66 bua_tf_tail_step66(bua_u8 desired,bua_u8 current,bua_u8 control,
                                      bua_u8 present_iac,bua_u8 vss_scaled,bua_u8 virtual_tps,
                                      bua_u8 rpm_raw,bua_u8 target_rpm_raw)
{
    BuaTfTail66 r;
    bua_u8 small_close=0u;
    r.control=(bua_u8)(control | 0x10u);
    r.request=bua_tf_pack_request_step66(desired,current,&small_close);
    r.pid_disable=small_close ? 30u : 0u; /* $46,X = C674 = 30 */
    if (r.request & 0x80u) return r;       /* closing request exits at LEA25 */
    if (r.request != 0u) {
        if (present_iac < 145u) return r;  /* $4D,X = C67B */
        r.control=(bua_u8)(r.control & 0xEFu);
    } else {
        r.control=(bua_u8)(r.control & 0xCFu);
    }
    /* Normal (non-Mode-4) LE9F9..LEA25 tail. */
    if (r.control & 0x04u) return r;
    if (vss_scaled != 0u || virtual_tps >= 3u) { r.request=0u; return r; }
    if (rpm_raw > target_rpm_raw) {
        r.request=0x82u;
        r.control=(bua_u8)(r.control | 0x02u);
    } else {
        r.request=0x02u;
        if (r.control & 0x02u) {
            r.control=(bua_u8)(r.control | 0x04u);
            r.pid_disable=40u; /* $45,X = C673 = 40 */
        }
    }
    return r;
}
static void run_step66_iac_tf_tail_test(void)
{
    unsigned int passed=0u,total=17u;
    BuaTfTail66 r;
#define S66(x) do { if (x) ++passed; } while (0)
    printf("\nStep-66 IAC follower command/state-tail regression:\n");
    r=bua_tf_tail_step66(40u,20u,0u,50u,1u,0u,50u,48u); S66(r.request==20u); S66((r.control&0x10u)!=0u);
    r=bua_tf_tail_step66(250u,10u,0u,50u,1u,0u,50u,48u); S66(r.request==127u);
    r=bua_tf_tail_step66(20u,30u,0u,50u,0u,0u,50u,48u); S66(r.request==0x8Au); S66(r.pid_disable==0u);
    r=bua_tf_tail_step66(29u,30u,0u,50u,0u,0u,50u,48u); S66(r.request==0x81u); S66(r.pid_disable==30u);
    r=bua_tf_tail_step66(0u,200u,0u,50u,0u,0u,50u,48u); S66(r.request==0xFFu);
    r=bua_tf_tail_step66(20u,20u,0u,50u,1u,0u,50u,48u); S66(r.request==0u); S66((r.control&0x30u)==0u);
    r=bua_tf_tail_step66(20u,20u,0u,50u,0u,3u,50u,48u); S66(r.request==0u);
    r=bua_tf_tail_step66(20u,20u,0u,50u,0u,2u,50u,48u); S66(r.request==0x82u); S66((r.control&0x02u)!=0u);
    r=bua_tf_tail_step66(20u,20u,0x02u,50u,0u,2u,48u,48u); S66(r.request==0x02u); S66((r.control&0x04u)!=0u); S66(r.pid_disable==40u);
    r=bua_tf_tail_step66(40u,20u,0u,145u,0u,0u,50u,48u); S66((r.control&0x10u)==0u);
    printf("  opening/closing packing, saturation, small-close timer, and idle tail %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef S66
}
