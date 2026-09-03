/* --------------------------------------------------------------------------
 * Step 67: 700R4 torque-converter-clutch baseline
 *
 * This first TCC step translates the executable-proven warm automatic path
 * around LE17A..LE2B1 and the production output selection at LEE92..LEE9B.
 * It deliberately does NOT invent a brake-switch input.  The source reads
 * FMD discrete bytes, but the brake semantic has not yet been proven.
 * -------------------------------------------------------------------------- */
#define STEP67_TCC_STATUS_BIT       0x20u
#define STEP67_TCC_OUTPUT_ON        0xDFFFu
#define STEP67_TCC_OUTPUT_OFF       0xD000u
#define STEP67_TCC_COOL_MIN         120u
#define STEP67_TCC_LOCK_DELAY       0u
#define STEP67_TCC_HYST             3u
/* LF2B9 is entered with X at LC2A0/LC2AD/LC2BA/LC2C7, not at the
 * following commented data label.  Therefore the leading calibration byte is
 * interpolation point zero.  LE2BE caps VSS at 100, doubles it, and LF2B9
 * subtracts $18 before the fixed-16 lookup. */
static const bua_u8 step67_non4_unlock[13] =
    {14u,59u,72u,92u,128u,166u,218u,236u,255u,255u,255u,255u,255u};
static const bua_u8 step67_non4_lock[13] =
    {15u,51u,59u,67u,87u,115u,166u,210u,210u,210u,210u,210u,210u};
static const bua_u8 step67_4th_unlock[13] =
    {14u,255u,113u,141u,172u,200u,225u,255u,255u,255u,255u,255u,255u};
static const bua_u8 step67_4th_lock[13] =
    {15u,0u,77u,105u,136u,174u,215u,255u,255u,255u,255u,255u,255u};
typedef struct BuaTcc67 {
    bua_u8 status37;
    bua_u8 delay;
    bua_u8 fourth_gear;
    bua_u8 overdrive_active;
    bua_u16 output_counter;
} BuaTcc67;
static bua_u8 bua_tcc_interp67(const bua_u8 *table, bua_u8 mph)
{
    bua_u8 a;
    bua_u8 index;
    bua_u8 fraction;
    /* LE2BE: min(VSS,100), ASLA; LF2B9: SBA with B=$18, saturate at 0. */
    if (mph > 100u) mph = 100u;
    a = (bua_u8)(mph << 1);
    if (a < 0x18u) a = 0u;
    else a = (bua_u8)(a - 0x18u);
    index = (bua_u8)(a >> 4);
    fraction = (bua_u8)(a << 4);
    return bua_interp_u8_lf2d6(table[index], table[(bua_u8)(index + 1u)],
                               fraction);
}
static bua_u8 bua_tcc_lock_threshold67(bua_u8 mph, bua_u8 fourth_gear)
{
    return bua_tcc_interp67(fourth_gear ? step67_4th_lock : step67_non4_lock, mph);
}
static bua_u8 bua_tcc_unlock_threshold67(bua_u8 mph, bua_u8 fourth_gear)
{
    return bua_tcc_interp67(fourth_gear ? step67_4th_unlock : step67_non4_unlock, mph);
}
static void bua_tcc_output67(BuaTcc67 *s)
{
    s->output_counter = ((s->status37 & STEP67_TCC_STATUS_BIT) != 0u) ?
                        STEP67_TCC_OUTPUT_ON : STEP67_TCC_OUTPUT_OFF;
}
static void bua_tcc_warm_auto67(BuaTcc67 *s, bua_u8 mph, bua_u8 tps,
                                bua_u8 coolant)
{
    bua_u8 threshold;
    bua_u8 locked;
    locked = (bua_u8)((s->status37 & STEP67_TCC_STATUS_BIT) != 0u);
    if (coolant < STEP67_TCC_COOL_MIN) {
        s->status37 &= (bua_u8)~STEP67_TCC_STATUS_BIT;
        s->delay = STEP67_TCC_LOCK_DELAY;
        bua_tcc_output67(s);
        return;
    }
    if (s->fourth_gear) {
        if (!locked) {
            threshold = bua_tcc_lock_threshold67(mph, 1u);
            if (mph >= 15u && tps < threshold) {
                if (s->delay != 0u) --s->delay;
                if (s->delay == 0u) s->status37 |= STEP67_TCC_STATUS_BIT;
            }
        } else {
            threshold = bua_tcc_unlock_threshold67(mph, 1u);
            if (tps >= threshold || mph < 14u)
                s->status37 &= (bua_u8)~STEP67_TCC_STATUS_BIT;
        }
    } else {
        threshold = bua_tcc_lock_threshold67(mph, 0u);
        if (!locked && mph >= 15u && tps < threshold)
            s->status37 |= STEP67_TCC_STATUS_BIT;
        else if (locked && (mph < 14u || tps >= threshold))
            s->status37 &= (bua_u8)~STEP67_TCC_STATUS_BIT;
    }
    bua_tcc_output67(s);
}
static void run_step67_tcc_test(void)
{
    BuaTcc67 s;
    bua_u8 lock40;
    bua_u8 unlock40;
    int passed;
    int total;
    passed = 0;
    total = 0;
    memset(&s, 0, sizeof(s));
    s.fourth_gear = 1u;
    s.overdrive_active = 1u;
    s.delay = STEP67_TCC_LOCK_DELAY;
    lock40 = bua_tcc_lock_threshold67(40u, 1u);
    unlock40 = bua_tcc_unlock_threshold67(40u, 1u);
    printf("\nStep-67 700R4 TCC baseline regression:\n");
    ++total; if (lock40 == 121u) ++passed;
    printf("  4th lock table @40 MPH       : %u  %s\n", (unsigned int)lock40,
           (lock40 == 121u) ? "PASS" : "FAIL");
    ++total; if (unlock40 == 157u) ++passed;
    printf("  4th unlock table @40 MPH     : %u  %s\n", (unsigned int)unlock40,
           (unlock40 == 157u) ? "PASS" : "FAIL");
    bua_tcc_warm_auto67(&s, 40u, 100u, 119u);
    ++total; if ((s.status37 & STEP67_TCC_STATUS_BIT) == 0u) ++passed;
    printf("  coolant 119 inhibits lock    : %s\n",
           ((s.status37 & STEP67_TCC_STATUS_BIT) == 0u) ? "PASS" : "FAIL");
    bua_tcc_warm_auto67(&s, 40u, 100u, 120u);
    ++total; if ((s.status37 & STEP67_TCC_STATUS_BIT) != 0u) ++passed;
    printf("  40 MPH moderate TPS locks    : %s\n",
           ((s.status37 & STEP67_TCC_STATUS_BIT) != 0u) ? "PASS" : "FAIL");
    ++total; if (s.output_counter == STEP67_TCC_OUTPUT_ON) ++passed;
    printf("  locked output = $DFFF        : %s\n",
           (s.output_counter == STEP67_TCC_OUTPUT_ON) ? "PASS" : "FAIL");
    bua_tcc_warm_auto67(&s, 40u, 200u, 120u);
    ++total; if ((s.status37 & STEP67_TCC_STATUS_BIT) == 0u) ++passed;
    printf("  high TPS unlocks             : %s\n",
           ((s.status37 & STEP67_TCC_STATUS_BIT) == 0u) ? "PASS" : "FAIL");
    ++total; if (s.output_counter == STEP67_TCC_OUTPUT_OFF) ++passed;
    printf("  unlocked output = $D000      : %s\n",
           (s.output_counter == STEP67_TCC_OUTPUT_OFF) ? "PASS" : "FAIL");
    bua_tcc_warm_auto67(&s, 44u, 120u, 120u);
    ++total; if ((s.status37 & STEP67_TCC_STATUS_BIT) != 0u) ++passed;
    printf("  44 MPH TPS120 re-locks       : %s\n",
           ((s.status37 & STEP67_TCC_STATUS_BIT) != 0u) ? "PASS" : "FAIL");
    printf("  source boundary: brake-switch semantic remains unresolved; no synthetic brake bit added.\n");
    printf("Step-67 TCC regression: %s (%d/%d)\n",
           (passed == total) ? "PASS" : "FAIL", passed, total);
}
/* --------------------------------------------------------------------------
 * Step 68: N/V gear classifier and final TCC qualification gate
 *
 * Source block LE122..LE17A first clears the transient gear bits, then uses
 * L008F (RPM/VSS or N/V ratio) to identify first/direct and fourth gear.
 * For the normal non-pass-by path, fourth gear is accepted only after the
 * ratio remains inside LC288..LC289 while L008E counts down.  The executable
 * resets that counter to LC28A whenever the ratio leaves the window.
 *
 * LE28F..LE2B1 clears the public TCC status every evaluation, decrements the
 * lock-delay and 4->3 inhibit timers, and restores TCC-locked only when the
 * candidate state is active and both timers have expired.
 * -------------------------------------------------------------------------- */
#define STEP68_MODE_OD_ACTIVE       0x80u
#define STEP68_MODE_FOURTH          0x08u
#define STEP68_MODE_FIRST_DIRECT    0x02u
#define STEP68_NV_FIRST_DIRECT      41u
#define STEP68_NV_FOURTH_MIN        30u
#define STEP68_NV_FOURTH_MAX        35u
#define STEP68_FOURTH_CONFIRM       8u
typedef struct BuaGear68 {
    bua_u8 mode04;
    bua_u8 fourth_timer;
    bua_u8 nv_ratio;
} BuaGear68;
static void bua_nv_gear_update68(BuaGear68 *g, bua_u8 nv_ratio)
{
    bua_u8 b;
    bua_u8 old;
    old = g->mode04;
    g->nv_ratio = nv_ratio;
    b = (bua_u8)(old & 0xE5u); /* executable ANDB #$E5 */
    /* Step 68 intentionally models the normal direct/non-pass-by branch.
       The OD-active first-gear branch remains for the next TCC pass. */
    if ((b & STEP68_MODE_OD_ACTIVE) == 0u) {
        if (nv_ratio >= STEP68_NV_FIRST_DIRECT)
            b = (bua_u8)(b | STEP68_MODE_FIRST_DIRECT);
        if (nv_ratio >= STEP68_NV_FOURTH_MIN &&
            nv_ratio <= STEP68_NV_FOURTH_MAX) {
            if (g->fourth_timer == 0u)
                b = (bua_u8)(b | STEP68_MODE_FOURTH);
            else
                --g->fourth_timer;
        } else {
            g->fourth_timer = STEP68_FOURTH_CONFIRM;
        }
    }
    g->mode04 = b;
}
typedef struct BuaTccGate68 {
    bua_u8 status37;
    bua_u8 candidate_mode;
    bua_u8 lock_delay;
    bua_u8 downshift_inhibit;
} BuaTccGate68;
static void bua_tcc_final_gate68(BuaTccGate68 *s)
{
    s->status37 = (bua_u8)(s->status37 & (bua_u8)~STEP67_TCC_STATUS_BIT);
    if (s->lock_delay != 0u)
        --s->lock_delay;
    if (s->downshift_inhibit != 0u) {
        --s->downshift_inhibit;
        return;
    }
    if (s->lock_delay == 0u && (s->candidate_mode & 0x80u) != 0u)
        s->status37 = (bua_u8)(s->status37 | STEP67_TCC_STATUS_BIT);
}
static void run_step68_tcc_nv_test(void)
{
    BuaGear68 g;
    BuaTcc67 t;
    BuaTccGate68 q;
    unsigned int passed=0u,total=18u,i;
#define S68(x) do { if (x) ++passed; } while (0)
    memset(&g,0,sizeof(g));
    g.fourth_timer=STEP68_FOURTH_CONFIRM;
    printf("\nStep-68 N/V gear + TCC final-gate regression:\n");
    bua_nv_gear_update68(&g,29u);
    S68((g.mode04&STEP68_MODE_FOURTH)==0u); S68(g.fourth_timer==8u);
    for(i=0u;i<8u;++i) bua_nv_gear_update68(&g,32u);
    S68((g.mode04&STEP68_MODE_FOURTH)==0u); S68(g.fourth_timer==0u);
    bua_nv_gear_update68(&g,32u);
    S68((g.mode04&STEP68_MODE_FOURTH)!=0u);
    bua_nv_gear_update68(&g,36u);
    S68((g.mode04&STEP68_MODE_FOURTH)==0u); S68(g.fourth_timer==8u);
    bua_nv_gear_update68(&g,41u);
    S68((g.mode04&STEP68_MODE_FIRST_DIRECT)!=0u);
    bua_nv_gear_update68(&g,40u);
    S68((g.mode04&STEP68_MODE_FIRST_DIRECT)==0u);
    memset(&g,0,sizeof(g)); g.fourth_timer=8u;
    for(i=0u;i<9u;++i) bua_nv_gear_update68(&g,32u);
    memset(&t,0,sizeof(t));
    t.fourth_gear=(bua_u8)((g.mode04&STEP68_MODE_FOURTH)!=0u);
    bua_tcc_warm_auto67(&t,40u,100u,120u);
    S68(t.fourth_gear==1u); S68((t.status37&STEP67_TCC_STATUS_BIT)!=0u);
    S68(t.output_counter==STEP67_TCC_OUTPUT_ON);
    memset(&q,0,sizeof(q)); q.candidate_mode=0x80u; q.lock_delay=2u;
    bua_tcc_final_gate68(&q); S68((q.status37&STEP67_TCC_STATUS_BIT)==0u); S68(q.lock_delay==1u);
    bua_tcc_final_gate68(&q); S68((q.status37&STEP67_TCC_STATUS_BIT)!=0u); S68(q.lock_delay==0u);
    q.status37=STEP67_TCC_STATUS_BIT; q.downshift_inhibit=1u;
    bua_tcc_final_gate68(&q); S68((q.status37&STEP67_TCC_STATUS_BIT)==0u); S68(q.downshift_inhibit==0u);
    printf("  N/V 30..35 requires eight decrements plus next evaluation; outside resets LC28A\n");
    printf("  N/V >=41 marks direct first-gear state; 40 clears it on the next classifier pass\n");
    printf("  derived 4th gear feeds Step-67 40-MPH lock path; final delay/inhibit gate preserved\n");
    printf("Step-68 TCC/N-V regression: %s (%u/%u)\n",(passed==total)?"PASS":"FAIL",passed,total);
#undef S68
}
/* --------------------------------------------------------------------------
 * Step 69: dormant pass-by TCC path and OD-active N/V branch
 *
 * LCF1A..LCF7F contains a special pass-by lock mechanism.  With the actual
 * 9340 calibration LC29C=$FFFF, the code uses CPX $FFFF followed by BHI.
 * No unsigned 16-bit X can be greater than $FFFF, so the completion branch
 * cannot be reached; INX simply wraps $FFFF to zero.  The capability remains
 * in the common executable but is calibration-disabled.
 *
 * LE122..LE16A also has a distinct branch when L0004 b7 (OD ACTIVE) is set.
 * It compares N/V against LC287=35, services L012C, then sets b4 when the
 * ratio/delay qualification is satisfied.  This helper preserves that branch
 * without assigning stronger gear semantics to b4 than the executable proves.
 * -------------------------------------------------------------------------- */
#define STEP69_PASSBY_TPS_MIN       255u
#define STEP69_PASSBY_VSS_MIN       28u
#define STEP69_PASSBY_VSS_MAX       32u
#define STEP69_PASSBY_RPM25_MIN     255u
#define STEP69_PASSBY_DELAY         65535u
#define STEP69_PASSBY_HOLD          240u
#define STEP69_PASSBY_NOISE_BIT     0x08u
#define STEP69_OD_BRANCH_BIT        0x10u
#define STEP69_OD_NV_THRESHOLD      35u
#define STEP69_FIRST_DELAY          0u
#define STEP69_DOWN_INHIBIT         0u
typedef struct BuaPassby69 {
    bua_u16 delay;
    bua_u16 hold;
    bua_u8 status37;
    bua_u8 minor36;
    bua_u8 mode04;
    bua_u16 output_counter;
} BuaPassby69;
static bua_u8 bua_passby_quals69(bua_u8 tps,bua_u8 mph,bua_u8 rpm25)
{
    if(tps<STEP69_PASSBY_TPS_MIN) return 0u;
    if(mph<=STEP69_PASSBY_VSS_MIN) return 0u;
    if(mph>STEP69_PASSBY_VSS_MAX) return 0u;
    if(rpm25<STEP69_PASSBY_RPM25_MIN) return 0u;
    return 1u;
}
static void bua_passby_tick69(BuaPassby69 *s,bua_u8 tps,bua_u8 mph,
                              bua_u8 rpm25,bua_u16 delay_cal)
{
    if(s->hold!=0u) {
        --s->hold;
        if(s->hold==0u) {
            s->mode04=(bua_u8)(s->mode04|0x81u);
            s->minor36=(bua_u8)(s->minor36&~STEP69_PASSBY_NOISE_BIT);
            return;
        }
        s->output_counter=STEP67_TCC_OUTPUT_ON;
        s->status37=(bua_u8)(s->status37|STEP67_TCC_STATUS_BIT);
        s->minor36=(bua_u8)(s->minor36|STEP69_PASSBY_NOISE_BIT);
        return;
    }
    if(!bua_passby_quals69(tps,mph,rpm25)) {
        s->delay=0u;
        return;
    }
    /* Exact unsigned CPX/BHI decision: start hold only if delay > calibration. */
    if(s->delay>delay_cal) {
        s->hold=STEP69_PASSBY_HOLD;
        s->delay=0u;
        return;
    }
    s->delay=(bua_u16)(s->delay+1u);
}
typedef struct BuaOdNv69 {
    bua_u8 mode04;
    bua_u8 first_delay;
    bua_u8 downshift_inhibit;
} BuaOdNv69;
static void bua_od_nv_branch69(BuaOdNv69 *s,bua_u8 nv)
{
    bua_u8 old=s->mode04;
    bua_u8 b=(bua_u8)(old&0xE5u);
    if((b&STEP68_MODE_OD_ACTIVE)==0u) { s->mode04=b; return; }
    if(nv<STEP69_OD_NV_THRESHOLD) {
        s->first_delay=STEP69_FIRST_DELAY;
    } else if(s->first_delay!=0u) {
        --s->first_delay;
    } else {
        b=(bua_u8)(b|STEP69_OD_BRANCH_BIT);
        if((old&0x12u)==0u) s->downshift_inhibit=STEP69_DOWN_INHIBIT;
    }
    s->mode04=b;
}
static void run_step69_tcc_passby_test(void)
{
    BuaPassby69 p;
    BuaOdNv69 o;
    unsigned int passed=0u,total=15u;
#define S69(x) do { if(x) ++passed; } while(0)
    memset(&p,0,sizeof(p));
    printf("\nStep-69 pass-by/OD-active TCC regression:\n");
    S69(bua_passby_quals69(255u,29u,255u)==1u);
    S69(bua_passby_quals69(254u,29u,255u)==0u);
    S69(bua_passby_quals69(255u,28u,255u)==0u);
    S69(bua_passby_quals69(255u,33u,255u)==0u);
    p.delay=65535u;
    bua_passby_tick69(&p,255u,29u,255u,STEP69_PASSBY_DELAY);
    S69(p.delay==0u); S69(p.hold==0u);
    /* Regression-only smaller delay proves the dormant branch arithmetic. */
    memset(&p,0,sizeof(p)); p.delay=4u;
    bua_passby_tick69(&p,255u,29u,255u,3u);
    S69(p.hold==240u); S69(p.delay==0u);
    bua_passby_tick69(&p,255u,29u,255u,3u);
    S69((p.status37&STEP67_TCC_STATUS_BIT)!=0u);
    S69(p.output_counter==STEP67_TCC_OUTPUT_ON);
    memset(&o,0,sizeof(o)); o.mode04=STEP68_MODE_OD_ACTIVE;
    bua_od_nv_branch69(&o,34u);
    S69((o.mode04&STEP69_OD_BRANCH_BIT)==0u);
    bua_od_nv_branch69(&o,35u);
    S69((o.mode04&STEP69_OD_BRANCH_BIT)!=0u);
    o.mode04=STEP68_MODE_OD_ACTIVE; o.first_delay=2u;
    bua_od_nv_branch69(&o,35u); S69(o.first_delay==1u);
    bua_od_nv_branch69(&o,35u); S69(o.first_delay==0u);
    bua_od_nv_branch69(&o,35u); S69((o.mode04&STEP69_OD_BRANCH_BIT)!=0u);
    printf("  actual LC29C=$FFFF: CPX/BHI completion is unreachable; $FFFF INX wraps to zero\n");
    printf("  pass-by also requires TPS=255, VSS 29..32 MPH, and RPM/25=255 before that timer test\n");
    printf("  OD-active N/V branch: <35 reloads delay; >=35 eventually asserts L0004 b4\n");
    printf("  brake-switch semantic still unresolved; no synthetic brake input added\n");
    printf("Step-69 TCC pass-by/OD regression: %s (%u/%u)\n",(passed==total)?"PASS":"FAIL",passed,total);
#undef S69
}
/* --------------------------------------------------------------------------
 * Step 70: brake release at the TCC hardware boundary
 *
 * The executable rebuilds L0037 from complemented FMD byte 1 while preserving
 * prior bit 5 (TCC).  No brake-labelled software input has been found in this
 * image.  The physical 700R4 circuit is therefore represented separately:
 * ECM command grounds the TCC side; brake-switch power must also be present.
 * The small RPM plant below is PC-only and is an acceptance signature, not
 * claimed ECM code.
 * -------------------------------------------------------------------------- */
typedef struct BuaTccHardware70 {
    bua_u8 ecm_command;
    bua_u8 brake_power;
    bua_u8 clutch_energized;
} BuaTccHardware70;
static bua_u8 bua_status37_from_fmd70(bua_u8 fmd1, bua_u8 old37)
{
    bua_u8 a;
    a=(bua_u8)~fmd1;
    a=(bua_u8)(a&0xDFu);
    a=(bua_u8)(a|(old37&STEP67_TCC_STATUS_BIT));
    return a;
}
static void bua_tcc_hardware70(BuaTccHardware70 *h)
{
    h->clutch_energized=(bua_u8)(h->ecm_command && h->brake_power);
}
static double bua_coast_rpm70(double rpm, double road_rpm, double idle_rpm,
                              bua_u8 clutch, double dt)
{
    double target,tau;
    target=clutch?road_rpm:idle_rpm;
    tau=clutch?0.12:0.65;
    return rpm+(target-rpm)*(dt/tau);
}
static void run_step70_tcc_brake_boundary_test(void)
{
    BuaTccHardware70 h;
    bua_u8 st;
    double rpm,locked_rpm,after_brake;
    unsigned int i,passed,total;
    passed=0u; total=10u;
    printf("\nStep-70 TCC brake/hardware-boundary regression:\n");
    st=bua_status37_from_fmd70(0xFFu,STEP67_TCC_STATUS_BIT);
    if((st&STEP67_TCC_STATUS_BIT)!=0u) ++passed;
    if((st&0xDFu)==0u) ++passed;
    st=bua_status37_from_fmd70(0xFEu,0u);
    if((st&0x01u)!=0u) ++passed;
    if((st&STEP67_TCC_STATUS_BIT)==0u) ++passed;
    memset(&h,0,sizeof(h)); h.ecm_command=1u; h.brake_power=1u;
    bua_tcc_hardware70(&h); if(h.clutch_energized) ++passed;
    h.brake_power=0u; bua_tcc_hardware70(&h);
    if(!h.clutch_energized) ++passed;
    if(h.ecm_command) ++passed;
    rpm=1600.0; h.brake_power=1u; bua_tcc_hardware70(&h);
    for(i=0u;i<20u;++i) rpm=bua_coast_rpm70(rpm,1450.0,650.0,h.clutch_energized,0.05);
    locked_rpm=rpm; if(locked_rpm>1300.0) ++passed;
    h.brake_power=0u; bua_tcc_hardware70(&h);
    for(i=0u;i<20u;++i) rpm=bua_coast_rpm70(rpm,1450.0,650.0,h.clutch_energized,0.05);
    after_brake=rpm; if(after_brake<900.0) ++passed;
    if((locked_rpm-after_brake)>400.0) ++passed;
    printf("  FMD refresh preserves independent TCC status bit 5; no brake bit invented\n");
    printf("  ECM command=1, brake power=1 -> physical clutch=%u\n",1u);
    printf("  ECM command remains 1, brake power=0 -> physical clutch=%u\n",0u);
    printf("  PC coast signature: locked %.0f RPM -> brake/released %.0f RPM\n",locked_rpm,after_brake);
    printf("  NOTE: RPM dynamics are simulator-only; hardware brake-power separation is the modeled boundary.\n");
    printf("Step-70 TCC brake-boundary regression: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
}
/* -------------------------------------------------------------------------
 * Step 72: TCC road-speed/TPS coast selector (LE1FB..LE227)
 * -------------------------------------------------------------------------
 * Executable behavior:
 *   LC28F = 32 MPH.  L0045 bit 4 supplies 5 MPH of state hysteresis:
 *     bit4 clear -> high-speed state enters at >=37 MPH
 *     bit4 set   -> high-speed state remains down to >=32 MPH
 *   Low-speed coast TPS threshold is LC290=8.  High-speed threshold is
 *   LC291=0.  When L0004 bit7 (OD active) is set, LC28E=3 is subtracted
 *   from that TPS threshold with saturation at zero.
 *   The branch at LE227 is taken only when TPS < threshold; equality does
 *   not qualify.  This helper deliberately preserves the RAM-bit behavior.
 */
#define STEP72_SHFT_COAST_BIT 0x10u
#define STEP72_OD_ACTIVE_BIT  0x80u
#define STEP72_COAST_MPH      32u
#define STEP72_COAST_TPS_LOW  8u
#define STEP72_COAST_TPS_HIGH 0u
#define STEP72_TCC_HYST       3u
typedef struct {
    bua_u8 shift45;
    bua_u8 mode04;
    bua_u8 threshold;
    bua_u8 coast_qualified;
} BuaTccCoast72;
static void bua_tcc_coast72(BuaTccCoast72 *s, bua_u8 mph, bua_u8 tps)
{
    bua_u16 switch_mph;
    bua_u8 threshold;
    switch_mph=STEP72_COAST_MPH;
    if((s->shift45&STEP72_SHFT_COAST_BIT)==0u) switch_mph+=5u;
    if((bua_u16)mph>=switch_mph) {
        s->shift45=(bua_u8)(s->shift45|STEP72_SHFT_COAST_BIT);
        threshold=STEP72_COAST_TPS_HIGH;
    } else {
        s->shift45=(bua_u8)(s->shift45&(bua_u8)~STEP72_SHFT_COAST_BIT);
        threshold=STEP72_COAST_TPS_LOW;
    }
    if((s->mode04&STEP72_OD_ACTIVE_BIT)!=0u) {
        if(threshold>=STEP72_TCC_HYST) threshold=(bua_u8)(threshold-STEP72_TCC_HYST);
        else threshold=0u;
    }
    s->threshold=threshold;
    s->coast_qualified=(bua_u8)(tps<threshold);
}
static void run_step72_tcc_coast_test(void)
{
    BuaTccCoast72 s;
    unsigned int passed=0u,total=0u;
#define S72(x) do { ++total; if(x) ++passed; } while(0)
    memset(&s,0,sizeof(s));
    printf("\nStep-72 TCC coast-selector regression:\n");
    bua_tcc_coast72(&s,36u,7u); S72((s.shift45&0x10u)==0u); S72(s.threshold==8u); S72(s.coast_qualified==1u);
    bua_tcc_coast72(&s,37u,0u); S72((s.shift45&0x10u)!=0u); S72(s.threshold==0u); S72(s.coast_qualified==0u);
    bua_tcc_coast72(&s,33u,0u); S72((s.shift45&0x10u)!=0u);
    bua_tcc_coast72(&s,31u,7u); S72((s.shift45&0x10u)==0u); S72(s.coast_qualified==1u);
    s.mode04=0x80u; bua_tcc_coast72(&s,31u,4u); S72(s.threshold==5u); S72(s.coast_qualified==1u);
    bua_tcc_coast72(&s,31u,5u); S72(s.coast_qualified==0u);
    s.shift45=0x10u; bua_tcc_coast72(&s,40u,0u); S72(s.threshold==0u); S72(s.coast_qualified==0u);
    printf("  low-speed entry/exit hysteresis = 37/32 MPH\n");
    printf("  low-speed TPS threshold = 8 raw, OD-active threshold = 5 raw\n");
    printf("  high-speed TPS threshold = 0 raw; TPS equality does not qualify\n");
    printf("Step-72 TCC coast regression: %s (%u/%u)\n",(passed==total)?"PASS":"FAIL",passed,total);
#undef S72
}
/* -------------------------------------------------------------------------
 * Step 73: TCC kickdown / force-upshift tail (LE25E..LE285)
 * -------------------------------------------------------------------------
 * This helper begins at LE25E, the branch reached by the coast/TPS decision.
 * It preserves the executable comparisons before the final Step-68 gate.
 * With the 9340 calibration LC293=LC294=128 (RPM/25 = 3200 RPM), LC295=60
 * MPH, and LC28C=0:
 *   - candidate b7 set and RPM/25 > 128 forces b7|b0 and clears lock delay;
 *   - otherwise VSS > 60 MPH also forces b7|b0 and clears lock delay;
 *   - VSS <= 60 clears b7 and reloads the lock delay with zero;
 *   - if b5 is set while b7 is clear, b0 is cleared before b5/b6 are stripped.
 * Bit names remain deliberately mechanical where semantics are not proven.
 */
#define STEP73_RPM25_FORCE       128u
#define STEP73_FORCE_MPH          60u
#define STEP73_LOCK_DELAY_CAL      0u
#define STEP73_B0                 0x01u
#define STEP73_B5                 0x20u
#define STEP73_B6                 0x40u
#define STEP73_B7                 0x80u
static bua_u8 bua_tcc_tail73(bua_u8 mode04, bua_u8 rpm25, bua_u8 mph,
                             bua_u8 *lock_delay)
{
    bua_u8 b;
    b=mode04;
    if((b&STEP73_B7)!=0u) {
        if(rpm25>STEP73_RPM25_FORCE) {
            *lock_delay=0u;
            b=(bua_u8)(b|0x81u);
            goto final_mode;
        }
    }
    if(mph>STEP73_FORCE_MPH) {
        *lock_delay=0u;
        b=(bua_u8)(b|0x81u);
    } else {
        b=(bua_u8)(b&0x7Fu);
        *lock_delay=STEP73_LOCK_DELAY_CAL;
    }
final_mode:
    if((b&STEP73_B5)!=0u && (b&STEP73_B7)==0u)
        b=(bua_u8)(b&0xFEu);
    b=(bua_u8)(b&0x9Fu);
    return b;
}
static void run_step73_tcc_tail_test(void)
{
    bua_u8 b,d;
    unsigned int passed=0u,total=0u;
#define S73(x) do { ++total; if(x) ++passed; } while(0)
    printf("\nStep-73 TCC kickdown/force-upshift tail regression:\n");
    d=9u; b=bua_tcc_tail73(0x80u,129u,40u,&d);
    S73((b&0x80u)!=0u); S73((b&0x01u)!=0u); S73(d==0u);
    d=9u; b=bua_tcc_tail73(0x80u,128u,60u,&d);
    S73((b&0x80u)==0u); S73(d==0u);
    d=9u; b=bua_tcc_tail73(0x80u,128u,61u,&d);
    S73((b&0x80u)!=0u); S73((b&0x01u)!=0u); S73(d==0u);
    d=9u; b=bua_tcc_tail73(0x00u,200u,61u,&d);
    S73((b&0x80u)!=0u); S73((b&0x01u)!=0u);
    d=9u; b=bua_tcc_tail73(0x21u,100u,40u,&d);
    S73((b&0x80u)==0u); S73((b&0x01u)==0u); S73((b&0x60u)==0u);
    d=9u; b=bua_tcc_tail73(0xC1u,100u,61u,&d);
    S73((b&0x80u)!=0u); S73((b&0x60u)==0u); S73((b&0x01u)!=0u);
    printf("  candidate b7 + RPM/25 >128 forces lock candidate even below 60 MPH\n");
    printf("  RPM/25 =128 does not; VSS boundary is exact: 60 unlocks, 61 forces\n");
    printf("  b7-clear path ignores RPM force test and still force-locks above 60 MPH\n");
    printf("  LE285 b5/b7 cleanup and final b5/b6 stripping are preserved\n");
    printf("Step-73 TCC tail regression: %s (%u/%u)\n",(passed==total)?"PASS":"FAIL",passed,total);
#undef S73
}

/* Step 77: exact LF2B9/TCC table-address regression. */
static void run_step77_lf2b9_tcc_test(void)
{
    unsigned int passed=0u,total=0u;
#define S77(x) do { ++total; if(x) ++passed; } while(0)
    printf("\nStep-77 exact LF2B9/TCC table regression:\n");
    S77(bua_tcc_lock_threshold67(12u,1u)==15u);
    S77(bua_tcc_lock_threshold67(20u,1u)==0u);
    S77(bua_tcc_lock_threshold67(40u,1u)==121u);
    S77(bua_tcc_unlock_threshold67(40u,1u)==157u);
    S77(bua_tcc_lock_threshold67(40u,0u)==77u);
    S77(bua_tcc_unlock_threshold67(40u,0u)==110u);
    S77(bua_tcc_lock_threshold67(100u,1u)==255u);
    printf("  LF2B9 uses the byte at LC2A0/AD/BA/C7 as interpolation point zero\n");
    printf("  4th @40 MPH: lock=121 unlock=157; non-4th: lock=77 unlock=110\n");
    printf("Step-77 LF2B9/TCC regression: %s (%u/%u)\n",(passed==total)?"PASS":"FAIL",passed,total);
#undef S77
}

/* Step 78: exact LE0D7..LE10A PRNDL/state-bit preamble. */
static bua_u8 bua_rora78(bua_u8 a, bua_u8 *carry)
{
    bua_u8 nc=(bua_u8)(a&1u);
    a=(bua_u8)((a>>1)|((*carry&1u)<<7));
    *carry=nc;
    return a;
}
static bua_u8 bua_prndl78(bua_u8 status37, bua_u8 *shift45,
                           bua_u8 mode04, bua_u8 mph, bua_u8 initial_carry)
{
    bua_u8 a=status37,b=*shift45,c=(bua_u8)(initial_carry&1u);
    unsigned int i;
    for(i=0u;i<4u;++i) a=bua_rora78(a,&c);
    if((a&0x80u)==0u) {
        b=(bua_u8)(b&0xF7u); *shift45=b;
    } else {
        if((b&0x08u)!=0u) a=(bua_u8)(a&0x7Fu);
        b=(bua_u8)(b|0x08u); *shift45=b;
        if((a&0x02u)!=0u) a=(bua_u8)(a>>1);
        else if((mode04&1u)!=0u && mph<=15u) a=(bua_u8)(a>>1);
    }
    a=(bua_u8)(a>>1);
    a=(bua_u8)(a&0x60u);
    if((a&0x40u)!=0u) a=(bua_u8)(a+1u);
    return (bua_u8)(mode04|a);
}
static void run_step78_prndl_test(void)
{
    unsigned int passed=0u,total=12u;
    bua_u8 sh,m0,m1;
#define S78(x) do{if(x)++passed;}while(0)
    sh=0x18u; m0=bua_prndl78(0x00u,&sh,0x00u,20u,0u);
    S78(sh==0x10u); S78(m0==0x00u);
    sh=0u; m0=bua_prndl78(0x04u,&sh,0u,20u,0u);
    S78(sh==0x08u); S78(m0==0x41u);
    sh=0u; m0=bua_prndl78(0x24u,&sh,0u,20u,0u);
    S78(m0==0x20u);
    sh=0x08u; m0=bua_prndl78(0x04u,&sh,0u,20u,0u);
    S78(sh==0x08u); S78(m0==0x00u);
    sh=0u; m0=bua_prndl78(0x04u,&sh,0x01u,15u,0u);
    S78(m0==0x21u);
    sh=0u; m0=bua_prndl78(0x04u,&sh,0x01u,16u,0u);
    S78(m0==0x41u);
    sh=0u; m0=bua_prndl78(0x06u,&sh,0u,20u,0u);
    S78(m0==0x61u);
    sh=0u; m0=bua_prndl78(0x04u,&sh,0u,20u,0u);
    sh=0u; m1=bua_prndl78(0x04u,&sh,0u,20u,1u);
    S78(m0==m1);
    sh=0u; m0=bua_prndl78(0x02u,&sh,0u,20u,0u);
    S78(m0==0x20u);
    printf("Step-78 PRNDL/state-bit regression: %s (%u/%u)\n",
           passed==total?"PASS":"FAIL",passed,total);
#undef S78
}

/* Step 79: exact LDF4A..LDF9C cooling-fan executable core. */
static bua_u8 bua_fan79(bua_u8 err1415, bua_u8 mph, bua_u8 status37,
                         bua_u8 coolant, bua_u8 duty, bua_u8 *min_timer)
{
    bua_u8 a,thresh,demand=0u;
    if(err1415!=0u) { a=255u; demand=1u; }
    else if((status37&0x40u)==0u && mph<35u) { a=255u; demand=1u; }
    else {
        if((status37&0x80u)!=0u && mph>11u)
            thresh=(duty==0u)?207u:200u; /* C2F6/C2F7 */
        else
            thresh=(duty==0u)?197u:193u; /* C2F4/C2F5 */
        if(coolant>=thresh) { a=255u; demand=1u; }
        else a=0u;
    }
    if(demand!=0u) { *min_timer=75u; return a; }
    if(*min_timer!=0u) { --*min_timer; return duty; }
    return 0u;
}
/* ---------------------------------------------------------------------- */
/* Step 80: executable A/C clutch qualification/request front half.       */
/*                                                                        */
/* This is LDF9D..LE005, before the already-translated IAC anticipation   */
/* tail.  C62E+$3F/$42/$43/$44 are LC66D/LC670/LC671/LC672.              */
/* Keep L0037 b7 in raw source polarity: b7=1 is documented as            */
/* "A/C compressor not on".                                               */
/* ---------------------------------------------------------------------- */
#define STEP80_AC_TPS_DISABLE       255u /* LC66D */
#define STEP80_AC_ENGAGE_DELAY        1u /* LC670 */
#define STEP80_AC_DISENGAGE_DELAY     1u /* LC671 */
#define STEP80_AC_COOL_DISABLE      255u /* LC672 */
typedef struct BuaAcFront80Tag {
    bua_u8 mode34;
    bua_u8 iac_control_f2;
    bua_u8 timer106;
    bua_u8 stack_marker;
    bua_u8 early_exit;
} BuaAcFront80;
static BuaAcFront80 bua_ac_front80(bua_u8 mode34,bua_u8 iac_mode_f3,
                                    bua_u8 iac_control_f2,bua_u8 pid_disable_10b,
                                    bua_u8 tps_tf_84,bua_u8 coolant_5b,
                                    bua_u8 status37,bua_u8 timer106)
{
    BuaAcFront80 r;
    bua_u8 a;
    r.mode34=mode34; r.iac_control_f2=iac_control_f2; r.timer106=timer106;
    r.stack_marker=0x80u; r.early_exit=0u;
    /* LDF9D: modes without the expected b3=1,b2=0 combination go LDFFF. */
    if ((iac_mode_f3&0x04u)!=0u || (iac_mode_f3&0x08u)==0u) goto force_request;
    if (pid_disable_10b!=0u) {
        r.iac_control_f2=(bua_u8)(r.iac_control_f2&0xFEu);
        r.early_exit=1u;
        return r;
    }
    a=(bua_u8)STEP80_AC_TPS_DISABLE;
    if ((r.mode34&0x20u)!=0u) a=(bua_u8)(a-16u);
    if (a<tps_tf_84) goto force_request;
    a=((r.mode34&0x20u)!=0u)?0xF9u:0u;
    a=(bua_u8)(a+(bua_u8)STEP80_AC_COOL_DISABLE);
    if (a<coolant_5b) goto force_request;
    a=(bua_u8)(status37&0x80u);
    if (a==0u) r.stack_marker=0u; /* INS/PSHA replaces initial $80 marker. */
    a=(bua_u8)(a+r.iac_control_f2);
    if ((a&0x80u)!=0u) {
        a=((status37&0x80u)!=0u)?(bua_u8)STEP80_AC_DISENGAGE_DELAY:
                                   (bua_u8)STEP80_AC_ENGAGE_DELAY;
        if (a<r.timer106) return r; /* BCS LE005: preserve current timer. */
    } else {
        a=r.timer106;
        if (a==0u) return r;
        --a;
        if (a!=0u) goto store_timer;
        r.mode34=(bua_u8)(r.mode34&0xDFu);
        if ((status37&0x80u)==0u) goto store_timer;
force_request:
        a=0u;
        r.mode34=(bua_u8)(r.mode34|0x20u);
    }
store_timer:
    r.timer106=a;
    return r;
}
static void run_step80_ac_front_test(void)
{
    BuaAcFront80 r;
    unsigned int passed=0u,total=16u;
#define S80(c) do { if(c) ++passed; printf("  %-84s %s\n",#c,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-80 A/C clutch qualification/request regression:\n");
    S80(STEP80_AC_TPS_DISABLE==255u);
    S80(STEP80_AC_ENGAGE_DELAY==1u && STEP80_AC_DISENGAGE_DELAY==1u);
    S80(STEP80_AC_COOL_DISABLE==255u);
    r=bua_ac_front80(0u,0u,0u,0u,0u,100u,0x80u,7u);
    S80((r.mode34&0x20u)!=0u && r.timer106==0u);
    r=bua_ac_front80(0u,0x0Cu,0u,0u,0u,100u,0x80u,7u);
    S80((r.mode34&0x20u)!=0u && r.timer106==0u);
    r=bua_ac_front80(0u,0x08u,0x81u,5u,0u,100u,0x80u,7u);
    S80(r.early_exit==1u && r.iac_control_f2==0x80u && r.timer106==7u);
    r=bua_ac_front80(0u,0x08u,0u,0u,255u,100u,0x80u,0u);
    S80((r.mode34&0x20u)==0u); /* equality does not trip CMPA/BCS */
    r=bua_ac_front80(0x20u,0x08u,0u,0u,240u,100u,0x80u,3u);
    S80((r.mode34&0x20u)!=0u && r.timer106==0u); /* active threshold is 239 */
    r=bua_ac_front80(0u,0x08u,0u,0u,0u,255u,0x80u,0u);
    S80((r.mode34&0x20u)==0u); /* coolant equality does not trip */
    r=bua_ac_front80(0u,0x08u,0u,0u,0u,255u,0x00u,0u);
    S80(r.stack_marker==0u);
    r=bua_ac_front80(0u,0x08u,0u,0u,0u,100u,0x80u,2u);
    S80(r.timer106==2u); /* requested delay 1 < current 2 -> preserve */
    r=bua_ac_front80(0u,0x08u,0u,0u,0u,100u,0x80u,1u);
    S80(r.timer106==1u); /* equality also preserves */
    r=bua_ac_front80(0u,0x08u,0u,0u,0u,100u,0x00u,2u);
    S80(r.timer106==1u && r.stack_marker==0u);
    r=bua_ac_front80(0u,0x08u,0u,0u,0u,100u,0x00u,1u);
    S80(r.timer106==0u && r.stack_marker==0u);
    r=bua_ac_front80(0x20u,0x08u,0u,0u,0u,100u,0x80u,1u);
    S80((r.mode34&0x20u)!=0u && r.timer106==1u);
    r=bua_ac_front80(0x20u,0x08u,0u,0u,0u,100u,0x00u,1u);
    S80((r.mode34&0x20u)==0u && r.timer106==0u);
    printf("Step-80 A/C front regression: %s (%u/%u)\n",passed==total?"PASS":"FAIL",passed,total);
#undef S80
}

