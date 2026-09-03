/* ---------------------------------------------------------------------- */
/* Step 91: exact Segment-5 A/C/IAC continuation, LE005..LE07B.           */
/*                                                                        */
/* The Step-80 front half leaves a stack marker that is $80 while the     */
/* source status says the compressor is not on, and zero when it is on.   */
/* This continuation carries that marker through the original IAC control */
/* word transitions, A/C learned-step bounds, learning-rate limit, and    */
/* P/N gain.  Arithmetic intentionally keeps 8-bit wrap semantics.        */
/* ---------------------------------------------------------------------- */
#define STEP91_AC_LEARN_MAX        32u   /* LC666 = C62E+$38 */
#define STEP91_AC_LEARN_MIN         1u   /* LC667 = C62E+$39 */
#define STEP91_AC_LEARN_PLUS_MAX    5u   /* LC668 = C62E+$3A */
#define STEP91_AC_LEARN_TIME       20u   /* LC669 = C62E+$3B */
#define STEP91_AC_PN_GAIN         128u   /* LC66A = C62E+$3C */
#define STEP91_AC_PN_TF_GAIN      255u   /* LC664 = C62E+$36 */

typedef struct BuaAcTail91Tag {
    bua_u8 iac_control_f2; /* L00F2 */
    bua_u8 learn_timer107; /* L0107 */
    bua_u8 learn_pos108;   /* L0108 */
    bua_u8 learned_2d;     /* L002D */
    bua_u8 learn_accum105; /* L0105 */
} BuaAcTail91;

static bua_u8 bua_ac_signed_gain91(bua_u8 a)
{
    unsigned int mag,prod;
    bua_u8 hi;
    if((a&0x80u)!=0u) {
        mag=(unsigned int)((bua_u8)(0u-a));
        prod=mag*(unsigned int)STEP91_AC_PN_GAIN;
        hi=(bua_u8)(prod>>8);
        return (bua_u8)(0u-hi);
    }
    prod=(unsigned int)a*(unsigned int)STEP91_AC_PN_GAIN;
    return (bua_u8)(prod>>8);
}

static BuaAcTail91 bua_ac_tail91(bua_u8 stack_marker,bua_u8 iac_control_f2,
                                  bua_u8 iac_mode_f3,bua_u8 current_pos_2c,
                                  bua_u8 learn_timer107,bua_u8 learn_pos108,
                                  bua_u8 learned_2d,bua_u8 learn_accum105)
{
    BuaAcTail91 r;
    bua_u8 a,b,candidate,diff;
    r.iac_control_f2=iac_control_f2;
    r.learn_timer107=learn_timer107;
    r.learn_pos108=learn_pos108;
    r.learned_2d=learned_2d;
    r.learn_accum105=learn_accum105;
    a=r.iac_control_f2;

    /* LE005..LE016: marker negative = compressor-not-on source state. */
    if((stack_marker&0x80u)!=0u) {
        a=(bua_u8)(a&0xFEu);
        if((a&0x80u)!=0u) goto merge_marker;
        a=(bua_u8)(a|0x20u);
        r.learn_timer107=0u;
        goto merge_marker;
    }

    /* LE016: compressor-on side clears removal b5 and sets controlled b6. */
    a=(bua_u8)((a&0xDFu)|0x40u);
    if((a&0x81u)==0u) goto store_direct;
    b=r.learn_timer107;
    if(b<STEP91_AC_LEARN_TIME) goto merge_marker;

    a=(bua_u8)(a^0x01u);
    if((a&0x80u)!=0u) {
        r.learn_pos108=current_pos_2c;
        r.learn_timer107=0u;
        goto merge_marker;
    }

    /* LE033..LE05E: derive learned anticipation from IAC movement. */
    if(current_pos_2c>=r.learn_pos108)
        candidate=(bua_u8)(current_pos_2c-r.learn_pos108);
    else
        candidate=0u;
    if((iac_mode_f3&0x80u)!=0u) {
        unsigned int d=(unsigned int)candidate*(unsigned int)STEP91_AC_PN_GAIN*2u;
        candidate=(bua_u8)(d>>8);
    }
    if(candidate>STEP91_AC_LEARN_MAX) candidate=STEP91_AC_LEARN_MAX;
    if(candidate<=STEP91_AC_LEARN_MIN) candidate=STEP91_AC_LEARN_MIN;

    b=candidate;
    diff=(bua_u8)(candidate-r.learned_2d);
    if(candidate>=r.learned_2d && diff>STEP91_AC_LEARN_PLUS_MAX) {
        b=(bua_u8)(r.learned_2d+STEP91_AC_LEARN_PLUS_MAX);
        diff=STEP91_AC_LEARN_PLUS_MAX;
    }
    r.learned_2d=b;

    /* LE05E..LE06F: P/N mode scales the signed learned-step change. */
    if((iac_mode_f3&0x80u)!=0u) diff=bua_ac_signed_gain91(diff);
    r.learn_accum105=(bua_u8)(r.learn_accum105+diff);

merge_marker:
    a=(bua_u8)(a&0x7Fu);
    a=(bua_u8)(a|stack_marker);
store_direct:
    r.iac_control_f2=a;
    return r;
}

static void bua_ac_front_tail_ram91(void)
{
    BuaAcFront80 ac;
    BuaAcTail91 t;
    ac=bua_ac_front80(RAM8(0x0034u),RAM8(0x00F3u),RAM8(0x00F2u),
                      RAM8(0x010Bu),RAM8(0x0084u),RAM8(0x005Bu),
                      RAM8(0x0037u),RAM8(0x0106u));
    RAM8(0x0034u)=ac.mode34;
    RAM8(0x00F2u)=ac.iac_control_f2;
    RAM8(0x0106u)=ac.timer106;
    if(ac.early_exit!=0u) return; /* source jumps directly to LE07B */
    t=bua_ac_tail91(ac.stack_marker,RAM8(0x00F2u),RAM8(0x00F3u),
                    RAM8(0x002Cu),RAM8(0x0107u),RAM8(0x0108u),
                    RAM8(0x002Du),RAM8(0x0105u));
    RAM8(0x00F2u)=t.iac_control_f2;
    RAM8(0x0107u)=t.learn_timer107;
    RAM8(0x0108u)=t.learn_pos108;
    RAM8(0x002Du)=t.learned_2d;
    RAM8(0x0105u)=t.learn_accum105;
}

/* Step-91 normal 9340 Segment-1 output staging.  These are software-side
   counter/bit requests only; the later HAL decides physical polarity and
   register addresses.  Mode-4 forcing remains a separate diagnostic/HAL path. */
typedef struct BuaOutputStage91Tag {
    bua_u16 air_arc_count;
    bua_u16 enrich_count;
    bua_u16 tcc_count;
    bua_u16 purge_count;
    bua_u16 egr_count;
    bua_u8 fan_parallel_b1;
    bua_u8 forced_off;
} BuaOutputStage91;
static BuaOutputStage91 step91_outputs;

static bua_u16 bua_pwm_count91(bua_u8 duty)
{
    bua_u16 d=(bua_u16)(0x3400u|(bua_u16)((bua_u8)(~duty)));
    d=(bua_u16)(d<<2);
    d=(bua_u16)(d|0x0003u);
    return d;
}

static BuaOutputStage91 bua_output_stage91(bua_u8 air3e,bua_u8 status37,
                                            bua_u8 mode34,bua_u8 purge113,
                                            bua_u8 egr112,bua_u8 fan_f4,
                                            bua_u8 force_off)
{
    BuaOutputStage91 r;
    r.forced_off=force_off?1u:0u;
    if(force_off) {
        r.air_arc_count=0xD000u;
        r.enrich_count=0xD000u;
        r.tcc_count=0xD000u;
        r.purge_count=0xD000u;
        r.egr_count=0xD000u;
        r.fan_parallel_b1=0u;
        return r;
    }
    r.air_arc_count=((air3e&0x02u)!=0u)?0xDFFFu:0xD000u;
    r.enrich_count=(4u!=0u && (air3e&0x04u)!=0u)?0xDFFFu:0xD000u;
    r.tcc_count=((status37&0x20u)!=0u)?0xDFFFu:0xD000u;
    r.purge_count=bua_pwm_count91(purge113);
    r.egr_count=bua_pwm_count91(egr112);
    r.fan_parallel_b1=(bua_u8)((fan_f4!=0u && (mode34&0x08u)!=0u)?1u:0u);
    return r;
}

