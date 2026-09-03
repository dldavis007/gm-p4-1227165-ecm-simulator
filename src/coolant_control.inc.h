/* ------------------------------------------------------------------------- */
/* Step 85: LF3B6 coolant producer, dual-range lookup and ERR14/15 fallback.  */
/*                                                                           */
/* The original hardware changes the thermistor series resistance.  The      */
/* executable uses L0030 b0 as the range state, with a 106/120 converted-     */
/* coolant hysteresis.  The 3840-ohm path adds 10 to the raw A/D argument     */
/* (saturating at 255); the 348-ohm path does not.  This helper intentionally */
/* models the ordinary path where L003B b3 is clear.  The source branches to  */
/* an otherwise undefined LF42A when that bit is set, so that exceptional     */
/* path remains an audit item rather than being guessed here.                 */
/* ------------------------------------------------------------------------- */
#define STEP85_ERR14_HIGH        227u  /* LC1E1 */
#define STEP85_COOL_DEFAULT      135u  /* LC1E2 */
#define STEP85_ERR15_RAW_HIGH    250u  /* LC1E5 */
#define STEP85_COOL_FILTER_COEF   16u  /* LC39D */

static const bua_u8 step85_cool_3840[17] = {
    255u,215u,155u,132u,117u,106u,97u,88u,81u,
    74u,67u,60u,52u,44u,34u,22u,0u
};
static const bua_u8 step85_cool_348[17] = {
    255u,255u,250u,223u,205u,191u,179u,168u,157u,
    147u,138u,128u,117u,105u,90u,67u,0u
};

typedef struct BuaCool85Tag {
    bua_u8 raw_adc;
    bua_u8 lookup_arg;
    bua_u8 converted_5d;
    bua_u16 coolant_5b;
    bua_u8 startup_cool_5f;
    bua_u8 mode30;
    bua_u8 mode3b;
    bua_u8 minor35;
    bua_u8 err4c;
    bua_u8 failure;       /* 0 normal, 14 ERR14 path, 15 ERR15 path */
    bua_u8 used_348;
} BuaCool85;

static bua_u8 bua_cool_lookup85(const bua_u8 *table,bua_u8 arg)
{
    bua_u8 index=(bua_u8)(arg>>4);
    bua_u8 fraction=(bua_u8)(arg<<4);
    if(index>=16u) return table[16];
    return bua_interp_u8_lf2d6(table[index],table[(bua_u8)(index+1u)],fraction);
}

static BuaCool85 bua_cool85_step(bua_u8 raw_adc,bua_u16 run_time,
                                  bua_u16 old_coolant,bua_u8 mode30,
                                  bua_u8 mode3b,bua_u8 minor35,bua_u8 err4c,
                                  bua_u8 cop2_not_toggled)
{
    BuaCool85 r;
    bua_u8 arg;
    bua_u8 converted;
    bua_u8 use348;
    r.raw_adc=raw_adc; r.mode30=mode30; r.mode3b=mode3b;
    r.minor35=minor35; r.err4c=err4c; r.failure=0u;
    r.coolant_5b=old_coolant; r.startup_cool_5f=(bua_u8)(old_coolant>>8);

    /* LF3B6..LF3D5: select table before updating the hysteretic range bit. */
    use348=((cop2_not_toggled==0u) && ((mode30&0x01u)!=0u))?1u:0u;
    r.used_348=use348;
    arg=raw_adc;
    if(use348==0u) {
        if(arg>245u) arg=255u;
        else arg=(bua_u8)(arg+10u);
        converted=bua_cool_lookup85(step85_cool_3840,arg);
    } else {
        converted=bua_cool_lookup85(step85_cool_348,arg);
    }
    r.lookup_arg=arg;
    r.converted_5d=converted;

    /* LF3D5..LF3EA: 106/120 hysteresis chooses the range for a future pass. */
    if(converted>120u) r.mode30=(bua_u8)(r.mode30|0x01u);
    else if(converted<=106u) r.mode30=(bua_u8)(r.mode30&0xFEu);

    /* L003C b7 is cleared by the source here; represented by the caller's
       cop2_not_toggled input being one-shot rather than retained in r. */

    /* L003B b3 jumps to an undefined LF42A in the supplied source.  Do not
       invent that diagnostic/special path.  Ordinary operation has b3 clear. */
    if((mode3b&0x08u)!=0u) return r;

    /* ERR14 is based on converted coolant; ERR15 is based on the original
       raw A/D byte restored from the stack. */
    if(converted>=STEP85_ERR14_HIGH) {
        r.failure=14u;
    } else if(raw_adc>STEP85_ERR15_RAW_HIGH) {
        r.failure=15u;
    }

    if(r.failure!=0u) {
        if(run_time>0u) {
            r.minor35=(bua_u8)(r.minor35|0x02u);
            if(r.failure==14u) r.err4c=(bua_u8)(r.err4c|0x20u);
            else r.err4c=(bua_u8)(r.err4c|0x10u);
        }
        r.coolant_5b=(bua_u16)((bua_u16)STEP85_COOL_DEFAULT<<8);
        /* The supplied source then branches to undefined label COOLS8.  The
           default L005B store is certain; downstream side effects are not. */
        return r;
    }

    if((r.mode3b&0x10u)!=0u)
        r.coolant_5b=bua_lag_filter_8_8(old_coolant,converted,
                                        (bua_u8)STEP85_COOL_FILTER_COEF);
    else
        r.coolant_5b=(bua_u16)((bua_u16)converted<<8);
    r.startup_cool_5f=(bua_u8)(r.coolant_5b>>8);
    r.mode3b=(bua_u8)(r.mode3b|0x10u);
    return r;
}

