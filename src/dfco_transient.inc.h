/* ---------------------------------------------------------------------- */
/* Step 45: LD86B..LD96D decel enleanment / DFCO state machine.          */
/*                                                                        */
/* This is kept in raw executable-domain units.  Of particular note,     */
/* LC5F6=0 makes the separate decel-enlean latch effectively momentary,   */
/* while DFCO uses 1200/900-RPM and 30/40-load hysteresis plus a 20-count */
/* entry timer.  LC608=511 is loaded on DFCO entry, but the executable    */
/* can explicitly clear that lockout when TPS rises above LC600.          */
/* ---------------------------------------------------------------------- */
#define STEP45_DFCO_BIT              0x02u
#define STEP45_STALL_SAVE_BIT        0x10u
#define STEP45_PN_BIT                0x01u
#define STEP45_VSS_FAIL_BIT          0x10u
#define STEP45_MAF_BYPASS_MASK       0xC0u
#define STEP45_TPS_BYPASS_MASK       0x24u
#define STEP45_DECEL_ENLEAN_BITS     0xA0u
#define STEP45_DECEL_ENLEAN_ACTIVE   0x80u
#define STEP45_DFCO_RPM_ENTER        48u
#define STEP45_DFCO_RPM_STAY         36u
#define STEP45_DFCO_LOAD_ENTER       30u
#define STEP45_DFCO_LOAD_STAY        40u
#define STEP45_DFCO_RPM_DROP_MAX     7u
#define STEP45_DFCO_ENTRY_COUNT      20u
#define STEP45_DFCO_TPS_MAX          8u
#define STEP45_DFCO_COOL_MIN         0u
#define STEP45_DFCO_VSS_MIN          15u
#define STEP45_STALL_TF_COUNT        30u
#define STEP45_STALL_AE_PULSES       1u
#define STEP45_DFCO_LOCKOUT_COUNT    511u
#define STEP45_DECEL_TPS_DELTA       240u
#define STEP45_DECEL_LOAD_DELTA      245u
#define STEP45_DECEL_HOLD_COUNT      0u
typedef struct {
    bua_u8 af_mode_word;       /* L0043 */
    bua_u8 mode3d;             /* L003D */
    bua_u8 mode39;             /* L0039 */
    bua_u8 qualify_count;      /* L00D3 */
    bua_u16 lockout_count;     /* L0088 */
    bua_u8 stall_ae_pulses;    /* L008A */
    bua_u8 stall_tf_count;     /* L00C5 */
} BuaDfcoState45;
typedef struct {
    bua_u8 active;
    bua_u8 hold_count;
} BuaDecelEnlean45;
static BuaDecelEnlean45 bua_decel_enlean_step45(BuaDecelEnlean45 s,
                                                 bua_u8 transient_load,
                                                 bua_u8 current_load,
                                                 bua_u8 transient_tps,
                                                 bua_u8 current_tps)
{
    bua_u8 dl;
    bua_u8 dt;
    if (transient_load < current_load) {
        s.active=0u;
        s.hold_count=(bua_u8)STEP45_DECEL_HOLD_COUNT;
        return s;
    }
    dl=(bua_u8)(transient_load-current_load);
    if (s.active!=0u && s.hold_count!=0u) {
        --s.hold_count;
        return s;
    }
    if (dl < (bua_u8)STEP45_DECEL_LOAD_DELTA || transient_tps < current_tps) {
        s.active=0u;
        s.hold_count=(bua_u8)STEP45_DECEL_HOLD_COUNT;
        return s;
    }
    dt=(bua_u8)(transient_tps-current_tps);
    if (dt < (bua_u8)STEP45_DECEL_TPS_DELTA) {
        s.active=0u;
        s.hold_count=(bua_u8)STEP45_DECEL_HOLD_COUNT;
        return s;
    }
    s.active=1u;
    s.hold_count=(bua_u8)STEP45_DECEL_HOLD_COUNT;
    return s;
}
static void bua_dfco_disqualify_step45(BuaDfcoState45 *s,
                                        bua_u8 rpm25,
                                        bua_u8 old_rpm12p5,
                                        bua_u8 current_rpm12p5)
{
    bua_u8 drop;
    s->qualify_count=(bua_u8)STEP45_DFCO_ENTRY_COUNT;
    s->af_mode_word &= (bua_u8)~STEP45_DFCO_BIT;
    if (s->lockout_count==0u) {
        s->mode3d &= (bua_u8)~STEP45_STALL_SAVE_BIT;
        return;
    }
    --s->lockout_count;
    /* LD946..LD95B: arm stall saver at <=900 RPM, or on a sufficiently
       rapid positive RPM drop while the lockout is counting. */
    if (rpm25 > (bua_u8)STEP45_DFCO_RPM_STAY) {
        if (old_rpm12p5 < current_rpm12p5)
            return;
        drop=(bua_u8)(old_rpm12p5-current_rpm12p5);
        if (drop < (bua_u8)STEP45_DFCO_RPM_DROP_MAX)
            return;
    }
    if ((s->mode3d & STEP45_STALL_SAVE_BIT)==0u) {
        s->mode3d |= STEP45_STALL_SAVE_BIT;
        s->stall_ae_pulses=(bua_u8)STEP45_STALL_AE_PULSES;
        s->stall_tf_count=(bua_u8)STEP45_STALL_TF_COUNT;
    }
}
static void bua_dfco_tick_step45(BuaDfcoState45 *s,
                                  bua_u8 coolant,
                                  bua_u8 pn_status,
                                  bua_u8 af_mode2,
                                  bua_u8 vss,
                                  bua_u8 fault_word,
                                  bua_u8 tps,
                                  bua_u8 filtered_load,
                                  bua_u8 rpm25,
                                  bua_u8 old_rpm12p5,
                                  bua_u8 current_rpm12p5)
{
    bua_u8 active;
    bua_u8 load_limit;
    bua_u8 rpm_limit;
    bua_u8 drop;
    active=(bua_u8)((s->af_mode_word & STEP45_DFCO_BIT)!=0u);
    (void)coolant; /* LC601=0; unsigned coolant cannot be below it in this image. */
    if ((pn_status & STEP45_PN_BIT)!=0u) {
        bua_dfco_disqualify_step45(s,rpm25,old_rpm12p5,current_rpm12p5);
        return;
    }
    if ((af_mode2 & STEP45_VSS_FAIL_BIT)==0u &&
        vss <= (bua_u8)STEP45_DFCO_VSS_MIN) {
        bua_dfco_disqualify_step45(s,rpm25,old_rpm12p5,current_rpm12p5);
        return;
    }
    /* With no selected fault bypass, TPS above LC600 explicitly clears
       the inter-DFCO timer before exiting through LD928. */
    if ((fault_word & STEP45_TPS_BYPASS_MASK)==0u &&
        tps > (bua_u8)STEP45_DFCO_TPS_MAX) {
        s->lockout_count=0u;
        bua_dfco_disqualify_step45(s,rpm25,old_rpm12p5,current_rpm12p5);
        return;
    }
    if (old_rpm12p5 >= current_rpm12p5) {
        drop=(bua_u8)(old_rpm12p5-current_rpm12p5);
        if (drop >= (bua_u8)STEP45_DFCO_RPM_DROP_MAX) {
            bua_dfco_disqualify_step45(s,rpm25,old_rpm12p5,current_rpm12p5);
            return;
        }
    }
    load_limit=active ? (bua_u8)STEP45_DFCO_LOAD_STAY
                      : (bua_u8)STEP45_DFCO_LOAD_ENTER;
    if ((fault_word & STEP45_MAF_BYPASS_MASK)==0u && filtered_load > load_limit) {
        bua_dfco_disqualify_step45(s,rpm25,old_rpm12p5,current_rpm12p5);
        return;
    }
    rpm_limit=active ? (bua_u8)STEP45_DFCO_RPM_STAY
                     : (bua_u8)STEP45_DFCO_RPM_ENTER;
    if (rpm25 <= rpm_limit) {
        bua_dfco_disqualify_step45(s,rpm25,old_rpm12p5,current_rpm12p5);
        return;
    }
    if (!active && s->qualify_count!=0u) {
        --s->qualify_count;
        s->af_mode_word &= (bua_u8)~STEP45_DFCO_BIT;
        return;
    }
    if (!active && s->lockout_count!=0u) {
        bua_dfco_disqualify_step45(s,rpm25,old_rpm12p5,current_rpm12p5);
        return;
    }
    s->af_mode_word |= STEP45_DFCO_BIT;
    s->lockout_count=(bua_u16)STEP45_DFCO_LOCKOUT_COUNT;
}

/* Step 96 scheduler wrapper for normal-running LD86B..LD99E.  The Step-45
 * helper covers LD872..LD96D state arithmetic; this wrapper maps every
 * affected byte back to its original address and restores the literal
 * L0039 and L003D side effects omitted from that standalone model.  LFA1E
 * is then applied before the differential-load AE calculation, exactly as
 * in the executable. */
static bua_u8 bua_load_ae_factor_step96(bua_u8 delta_above_min)
{
    static const bua_u8 table[5] = {32u,32u,40u,52u,52u};
    bua_u16 position;
    bua_u8 index;
    bua_u8 fraction;
    position=(bua_u16)((bua_u16)delta_above_min*4u);
    index=(bua_u8)(position>>8);
    fraction=(bua_u8)position;
    if(index>=4u) return table[4];
    return bua_interp_u8_lf2d6(table[index],table[(bua_u8)(index+1u)],fraction);
}

static void bua_odd_transient_front_12p5ms(void)
{
    BuaDecelEnlean45 e;
    BuaDfcoState45 d;
    BuaTpsAeTriggerResult t;
    bua_u8 delta;
    bua_u8 factor;
    bua_u16 product;
    bua_u16 doubled;
    bua_u16 addr;
    unsigned int sum;

    /* LD86B arms the single-fire toggle only when single fire is active. */
    if((INJECT_MODE_WORD&0x80u)!=0u)
        INJECT_MODE_WORD|=0x10u;

    e.active=(bua_u8)((RAM8(0x003Eu)&0x80u)!=0u);
    e.hold_count=RAM8(0x00E8u);
    e=bua_decel_enlean_step45(e,RAM8(0x00E5u),FILTERED_LOAD,
                               RAM8(0x00E9u),TPS_LOAD_AXIS);
    RAM8(0x00E8u)=e.hold_count;
    if(e.active!=0u) {
        RAM8(0x003Eu)|=0xA0u;
        RAM8(0x0038u)&=(bua_u8)~0x10u;
    } else {
        RAM8(0x003Eu)&=(bua_u8)~0x80u;
    }

    d.af_mode_word=AF_MODE_WORD;
    d.mode3d=AIR_MODE_WORD;
    d.mode39=INJECT_MODE_WORD;
    d.qualify_count=RAM8(0x00D3u);
    d.lockout_count=ram16be_get(0x0088u);
    d.stall_ae_pulses=RAM8(0x008Au);
    d.stall_tf_count=RAM8(0x00C5u);
    bua_dfco_tick_step45(&d,COOLANT,RAM8(0x0037u),AF_MODE_WORD2,
                          VSS_MPH_HI,RAM8(0x003Fu),TPS_LOAD_AXIS,
                          FILTERED_LOAD,RPM_DIV25,RAM8(0x005Au),
                          RPM_DIV12P5);
    /* LC014 bit 1 is clear, so DFCO entry does not select single fire. */
    if((d.af_mode_word&STEP45_DFCO_BIT)!=0u)
        d.mode3d&=(bua_u8)~0x10u;
    if((d.af_mode_word&STEP45_DFCO_BIT)==0u &&
       (d.mode39&0x20u)!=0u)
        d.mode39&=(bua_u8)~0x60u;
    AF_MODE_WORD=d.af_mode_word;
    AIR_MODE_WORD=d.mode3d;
    INJECT_MODE_WORD=d.mode39;
    RAM8(0x00D3u)=d.qualify_count;
    ram16be_set(0x0088u,d.lockout_count);
    RAM8(0x008Au)=d.stall_ae_pulses;
    RAM8(0x00C5u)=d.stall_tf_count;

    t=bua_tps_ae_lfa1e(TPS_LOAD_AXIS,RAM8(0x00DDu),RAM8(0x003Fu),
                        RAM8(0x0038u),RAM8(0x00D4u),RAM8(0x00DFu));
    if(t.triggered!=0u) {
        for(addr=0x018Eu;addr<0x01AEu;addr=(bua_u16)(addr+2u))
            RAM8(addr)=0u;
    }
    RAM8(0x0038u)=t.flags;
    RAM8(0x00D4u)=t.stored_diff;
    RAM8(0x00DFu)=t.async_count;

    /* LD97C..LD99E: the C0 fault mask or negative load delta clears E2.
     * Otherwise subtract LC33C, interpolate C33D, apply E7, double, and
     * saturating-add the result to the existing differential-load factor. */
    if((RAM8(0x003Fu)&0xC0u)!=0u || FILTERED_LOAD<RAM8(0x00E5u)) {
        ACCEL_LOAD_FACTOR=0u;
    } else {
        delta=(bua_u8)(FILTERED_LOAD-RAM8(0x00E5u));
        if(delta>=(bua_u8)20u) {
            delta=(bua_u8)(delta-20u);
            RAM8(0x00E1u)=delta;
            RAM8(0x0038u)|=0x08u;
            factor=bua_load_ae_factor_step96(delta);
            product=(bua_u16)((bua_u16)factor*(bua_u16)RAM8(0x00E7u));
            doubled=(bua_u16)(product<<1);
            if((product&0x8000u)!=0u)
                ACCEL_LOAD_FACTOR=255u;
            else {
                sum=(unsigned int)(doubled>>8)+(unsigned int)ACCEL_LOAD_FACTOR;
                ACCEL_LOAD_FACTOR=(sum>255u)?255u:(bua_u8)sum;
            }
        }
    }
    if(ACCEL_LOAD_FACTOR==0u && ram16be_get(ACCEL_FUEL_ADDR)==0u)
        RAM8(0x0038u)&=(bua_u8)~0x0Eu;
    ++stats.odd_transient_front_calls;
}
/* ---------------------------------------------------------------------- */
/* Step 46: connect DFCO/stall-saver state to actual fuel and IAC effects.*/
/*                                                                        */
/* LF92A sends VATS-fail, ignition-off, or L0043 b1 (DFCO) directly to    */
/* LF957, which zeros both accumulated accel fuel and ordinary base PW.   */
/* LE4B1 consumes LC607=1 stall-save async pulse and adds LC605=256 raw   */
/* counts to L00DB; LE8E3 consumes LC604=30 follower counts and adds      */
/* LC603=8 to the virtual TPS axis.  Active DFCO itself adds LC60A=10.    */
/* ---------------------------------------------------------------------- */
#define STEP46_VATS_OK_BIT            0x20u
#define STEP46_IGN_OFF_BIT            0x10u
#define STEP46_DFCO_BIT               0x02u
#define STEP46_STALL_SAVE_BIT         0x10u
#define STEP46_STALL_ASYNC_PW         256u
#define STEP46_STALL_TF_TPS_ADD       8u
#define STEP46_DFCO_TF_TPS_ADD        10u
typedef struct {
    bua_u16 base_pw;
    bua_u16 accel_accum;
} BuaFuelGate46;
typedef struct {
    bua_u16 new_async_pw;
    bua_u16 accel_accum;
    bua_u8 pulses_left;
} BuaStallAsync46;
typedef struct {
    bua_u8 virtual_tps;
    bua_u8 duration_left;
} BuaFollower46;
static BuaFuelGate46 bua_lf92a_dfco_gate_step46(bua_u16 base_pw,
                                                 bua_u16 accel_accum,
                                                 bua_u8 mode3,
                                                 bua_u8 minor_mode1,
                                                 bua_u8 af_mode_word)
{
    BuaFuelGate46 r;
    r.base_pw=base_pw;
    r.accel_accum=accel_accum;
    if ((mode3 & STEP46_VATS_OK_BIT)==0u ||
        (minor_mode1 & STEP46_IGN_OFF_BIT)!=0u ||
        (af_mode_word & STEP46_DFCO_BIT)!=0u) {
        r.base_pw=0u;
        r.accel_accum=0u;
    }
    return r;
}
static BuaStallAsync46 bua_stall_async_step46(bua_u16 new_async_pw,
                                               bua_u16 accel_accum,
                                               bua_u8 mode3d,
                                               bua_u8 pulses_left)
{
    BuaStallAsync46 r;
    bua_u32 sum;
    r.new_async_pw=new_async_pw;
    r.accel_accum=accel_accum;
    r.pulses_left=pulses_left;
    if ((mode3d & STEP46_STALL_SAVE_BIT)==0u || pulses_left==0u)
        return r;
    --r.pulses_left;
    /* LE4BA has no carry branch: LC605 addition wraps as a 16-bit ADDD. */
    r.new_async_pw=(bua_u16)(new_async_pw+
                              (bua_u16)STEP46_STALL_ASYNC_PW);
    sum=(bua_u32)r.new_async_pw+(bua_u32)accel_accum;
    if (sum>65535ul) sum=65535ul;
    r.accel_accum=(bua_u16)sum;
    return r;
}
static BuaFollower46 bua_dfco_follower_step46(bua_u8 tps_axis,
                                               bua_u8 mode3d,
                                               bua_u8 stall_duration,
                                               bua_u8 af_mode_word)
{
    BuaFollower46 r;
    unsigned int a=(unsigned int)tps_axis;
    r.duration_left=stall_duration;
    if ((mode3d & STEP46_STALL_SAVE_BIT)!=0u && r.duration_left!=0u) {
        --r.duration_left;
        a += STEP46_STALL_TF_TPS_ADD;
        if (a>255u) a=255u;
    }
    if ((af_mode_word & STEP46_DFCO_BIT)!=0u) {
        a += STEP46_DFCO_TF_TPS_ADD;
        if (a>255u) a=255u;
    }
    r.virtual_tps=(bua_u8)a;
    return r;
}
