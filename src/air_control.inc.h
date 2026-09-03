/* ---------------------------------------------------------------------- */
/* Step 81: major-loop segment 9, LE34E..LE432 injection-air management.   */
/*                                                                        */
/* This is a behavior-first translation.  L003E b1/b2 are kept as raw     */
/* output-request bits; no electrical valve polarity is invented here.    */
/* The actual 9340 LC015 value is $04, so the LE41E path represents the   */
/* source's two-air-valve configuration.                                  */
/* ---------------------------------------------------------------------- */
#define STEP81_AIR_PE_COUNT          0u   /* LC2FF */
#define STEP81_AIR_HIGH_RPM_COUNT   10u   /* LC300 */
#define STEP81_AIR_O2_RICH          170u  /* LC301 */
#define STEP81_AIR_O2_LEAN           56u  /* LC302 */
#define STEP81_AIR_O2_TIME          200u  /* LC303 */
#define STEP81_AIR_HIGH_RPM         160u  /* LC305, RPM/25 */
#define STEP81_AIR_DROP_HOLD         10u  /* LC306 */
#define STEP81_AIR_LOW_LOAD          25u  /* LC307 */
#define STEP81_AIR_HIGH_LOAD        100u  /* LC308 */
#define STEP81_AIR_HIGH_VSS          60u  /* LC309 */
#define STEP81_AIR_LOAD_DROP         64u  /* LC30A */
#define STEP81_AIR_MIN_COOL          73u  /* LC30B */
#define STEP81_AIR_OPEN_DELAY       254u  /* LC30C */
#define STEP81_AIR_VALVE_CAL          4u  /* LC015 actual 9340 value */

typedef struct BuaAir81Tag {
    bua_u8 af43;       /* L0043: PE b5, AIR DIVERT b0 */
    bua_u8 af44;       /* L0044: closed-loop b7 */
    bua_u8 history3d;  /* L003D: sticky closed-loop history in b7 */
    bua_u8 air3e;      /* L003E: raw air output/request bits b1/b2 */
    bua_u8 drop_fc;    /* L00FC */
    bua_u8 open_fd;    /* L00FD */
    bua_u8 pe_fe;      /* L00FE */
    bua_u8 rpm_f6;     /* L00F6 */
    bua_u16 rich_f7;   /* L00F7 */
    bua_u16 lean_f9;   /* L00F9 */
    bua_u8 old_load_fb;/* L00FB */
} BuaAir81;

static void bua_air81(BuaAir81 *s,bua_u8 coolant,bua_u8 mode3b,
                      bua_u8 mode34,bua_u8 rpm25,bua_u8 load,
                      bua_u8 o2,bua_u8 mph)
{
    bua_u8 b,a;
    bua_u16 x;
    /* LE34E: retain prior L003D and latch current closed-loop state. */
    b=(bua_u8)(s->af44&0x80u);
    b=(bua_u8)(b|s->history3d);
    s->history3d=b;
    a=coolant;
    if(a<STEP81_AIR_MIN_COOL || (mode3b&0x04u)!=0u) goto divert;

    a=s->af44;
    if((a&0x80u)!=0u) {
        s->drop_fc=0u;
        s->open_fd=STEP81_AIR_OPEN_DELAY;
        goto rpm_check;
    }
    if((b&0x80u)!=0u) {
        a=s->open_fd;
        if(a==0u) goto divert;
        --a;
        s->open_fd=a;
    }
    /* LE37F: load-drop hold is only serviced when L0034 b7 is set. */
    if((mode34&0x80u)!=0u) {
        b=s->drop_fc;
        a=s->old_load_fb;
        if(a>load) {
            a=(bua_u8)(a-load);
            if(a>STEP81_AIR_LOAD_DROP) b=STEP81_AIR_DROP_HOLD;
        }
        if(b!=0u) {
            --b;
            s->drop_fc=b;
            goto one_valve;
        }
    }

rpm_check:
    b=rpm25;
    if(b>STEP81_AIR_HIGH_RPM) {
        b=s->rpm_f6;
        if(b>STEP81_AIR_HIGH_RPM_COUNT) goto divert;
        ++b;
        if(b!=0u) s->rpm_f6=b;
    } else {
        s->rpm_f6=0u;
    }

    a=s->af43;
    if((a&0x20u)!=0u) {
        b=s->pe_fe;
        if(b==STEP81_AIR_PE_COUNT) goto divert;
        ++b;
        s->pe_fe=b;
        goto both_valves;
    }
    s->pe_fe=0u;

    x=s->rich_f7;
    b=o2;
    if(b>STEP81_AIR_O2_RICH) {
        if(x>=STEP81_AIR_O2_TIME) {
            s->af43=(bua_u8)(s->af43|0x01u);
            goto divert;
        }
        ++x;
    } else {
        if(x==0u) {
            s->af43=(bua_u8)(s->af43&0xFEu);
        } else {
            --x;
        }
    }
    s->rich_f7=x;

    /* RORA at LE3E6 tests the AIR DIVERT bit just written/retained in b0. */
    if((s->af43&0x01u)!=0u) goto divert;

    if(b<STEP81_AIR_O2_LEAN && (s->af44&0x80u)!=0u) {
        x=s->lean_f9;
        if(x>STEP81_AIR_O2_TIME) goto divert;
        ++x;
    } else {
        x=0u;
    }
    s->lean_f9=x;

    if(load<STEP81_AIR_LOW_LOAD) goto divert;
    if(load>STEP81_AIR_HIGH_LOAD && mph>STEP81_AIR_HIGH_VSS) goto divert;

    if((s->history3d&0x80u)!=0u) goto one_valve;

both_valves:
    s->air3e=(bua_u8)(s->air3e|0x06u);
    goto done;

one_valve:
    if(STEP81_AIR_VALVE_CAL==0u) goto divert;
    s->air3e=(bua_u8)((s->air3e&0xFBu)|0x02u);
    goto done;

divert:
    s->air3e=(bua_u8)(s->air3e&0xF9u);

done:
    s->old_load_fb=load;
}

