/* ---------------------------------------------------------------------- */
/* Step 82: major-loop segment B, LDEE5..LDF49 EGR.                       */
/*                                                                        */
/* The supplied source has an obvious unrelated pasted fuel-code fragment */
/* between LDF15 and the surviving CMPA #80 / LDF1D sequence.  The EGR    */
/* path around it is still unambiguous: cap RPM/25 at 80, call LF27C on   */
/* C23B, then apply the C262 coolant multiplier.  In the 9340 image every */
/* reachable C23B EGR-duty table cell is 255, so the corrupted call-site  */
/* text cannot change the calibrated table result.                        */
/* ---------------------------------------------------------------------- */
#define STEP82_EGR_TPS_ON       8u   /* LC239, used when prior EGR PW is zero */
#define STEP82_EGR_TPS_HOLD     5u   /* LC238, used when prior EGR PW != zero */
#define STEP82_EGR_MIN_MAT     30u   /* LC23A */
#define STEP82_EGR_RPM_MAX     80u   /* executable cap, RPM/25 */
#define STEP82_EGR_LOAD_MAX   160u   /* executable cap */

static const bua_u8 step82_egr_cool_mult[7] =
    {0u,0u,0u,0u,128u,128u,128u}; /* C262, points at raw coolant 64..160 */

typedef struct BuaEgr82ResultTag {
    bua_u8 pw112;       /* L0112 final EGR duty/PW byte */
    bua_u8 base_duty;   /* C23B LF27C result; 255 throughout 9340 table */
    bua_u8 cool_mult;   /* C262 LF2B9 result */
    bua_u8 rpm_used;    /* capped executable table argument */
    bua_u8 load_used;   /* capped executable table argument */
} BuaEgr82Result;

static bua_u8 bua_egr_cool_mult82(bua_u8 coolant)
{
    bua_u8 a,index,fraction;
    if(coolant>160u) coolant=160u;
    a=(coolant<64u)?0u:(bua_u8)(coolant-64u); /* LF2B9 saturating subtract */
    index=(bua_u8)(a>>4);
    fraction=(bua_u8)(a<<4);
    if(index>=6u) return step82_egr_cool_mult[6];
    return bua_interp_u8_lf2d6(step82_egr_cool_mult[index],
                               step82_egr_cool_mult[(bua_u8)(index+1u)],fraction);
}

static BuaEgr82Result bua_egr82(bua_u8 status37,bua_u8 minor35,bua_u8 mat,
                                bua_u8 tps,bua_u8 af43,bua_u8 rpm25,
                                bua_u8 load,bua_u8 coolant,bua_u8 prior_pw)
{
    BuaEgr82Result r;
    unsigned int product,shifted;
    bua_u8 threshold;
    memset(&r,0,sizeof(r));

    /* LDEE5: P/N status b0 normally inhibits EGR; ALDL diagnostic mode bypasses. */
    if((status37&0x01u)!=0u && (minor35&0x20u)==0u) return r;
    if(mat<STEP82_EGR_MIN_MAT) return r;

    /* LDD LC238 loads A=5/B=8.  Zero prior PW executes TBA and therefore uses 8. */
    threshold=(prior_pw!=0u)?STEP82_EGR_TPS_HOLD:STEP82_EGR_TPS_ON;
    if(tps<threshold) return r;              /* BHI: equality qualifies */
    if((af43&0x20u)!=0u) return r;           /* PE inhibits EGR */

    r.load_used=(load<STEP82_EGR_LOAD_MAX)?load:STEP82_EGR_LOAD_MAX;
    r.rpm_used=(rpm25<=STEP82_EGR_RPM_MAX)?rpm25:STEP82_EGR_RPM_MAX;

    /* Actual 9340 C23B table: all 36 reachable cells are 255. */
    r.base_duty=255u;
    r.cool_mult=bua_egr_cool_mult82(coolant);

    /* LDF2B: MUL, ASLD; carry on ASLD saturates A to 255. */
    product=(unsigned int)r.cool_mult*(unsigned int)r.base_duty;
    shifted=product<<1;
    if((shifted&0x10000u)!=0u) r.pw112=255u;
    else r.pw112=(bua_u8)((shifted>>8)&0xFFu);
    return r;
}

