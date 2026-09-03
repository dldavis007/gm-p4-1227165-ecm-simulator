/* ---------------------------------------------------------------------- */
/* Step 83: major-loop segment C, LE75D..LE816 canister purge (CCP).       */
/* 9340 C27C load-gain table is uniformly 128, so load cannot alter the   */
/* requested purge duty in this calibration.  C272 remains active and     */
/* maps MAF to duty.  L0114 is the stateful minimum requested-duty latch;  */
/* L0113 is the LF250-filtered actual command and L003B b0 is its status.  */
/* ---------------------------------------------------------------------- */
static const bua_u8 step83_ccp_maf[9]={16u,32u,48u,80u,160u,208u,255u,255u,255u};
#define STEP83_RUN_TIME 80u
#define STEP83_MIN_COOL 147u
#define STEP83_VSS_ON   16u
#define STEP83_TPS_ON   10u
#define STEP83_FLOW_ON   0u
#define STEP83_VSS_HOLD 10u
#define STEP83_TPS_HOLD  5u
#define STEP83_FLOW_HOLD 0u
#define STEP83_FILTER    32u

typedef struct BuaPurge83Tag {
    bua_u8 runtime_latched; /* L0001 b4 represented as boolean */
    bua_u8 status3b;        /* L003B, b0 = prior/final CCP-on state */
    bua_u8 pw113;           /* L0113 filtered CCP command */
    bua_u8 min114;          /* L0114 minimum requested duty latch */
    bua_u8 requested;       /* pre-filter A from C272 * C27C */
} BuaPurge83;

static bua_u8 bua_ccp_maf83(bua_u16 flow_q8_8)
{
    bua_u16 d;
    bua_u8 a,index,fraction;
    /* LE7A2: cap integer MAF at 32, then ASLD x3. A is high byte of D. */
    if((bua_u8)(flow_q8_8>>8)>=32u) d=0xFFFFu;
    else d=flow_q8_8;
    d=(bua_u16)(d<<3);
    a=(bua_u8)(d>>8);
    /* LF2BF with C272 header 8: MUL by 8; high byte selects table point. */
    index=(bua_u8)(((unsigned int)a*8u)>>8);
    fraction=(bua_u8)((unsigned int)a*8u);
    if(index>=8u) return step83_ccp_maf[8];
    return bua_interp_u8_lf2d6(step83_ccp_maf[index],step83_ccp_maf[index+1u],fraction);
}

static void bua_purge83(BuaPurge83 *s,bua_u8 runtime,bua_u8 minor35,
                        bua_u8 coolant,bua_u8 af43,bua_u8 af44,bua_u8 err41,
                        bua_u8 cal014,bua_u8 vss_scaled,bua_u8 tps,
                        bua_u16 flow_q8_8,bua_u8 load)
{
    bua_u8 prior_on,qual=1u,req=0u,maf,load_gain;
    bua_u16 filtered;
    prior_on=(bua_u8)(s->status3b&1u);

    if(!s->runtime_latched) {
        if(runtime<=STEP83_RUN_TIME) qual=0u; /* BLS: equality still disabled */
        else s->runtime_latched=1u;
    }
    if((minor35&0x20u)!=0u) qual=0u;          /* ALDL inhibits purge */
    if(coolant<STEP83_MIN_COOL) qual=0u;
    if((af43&0x02u)!=0u) qual=0u;             /* DFCO inhibits */
    /* Optional closed-loop CCP gate: when enabled, open loop requires err44/55 or err13. */
    if(qual && (cal014&0x20u)!=0u && (af44&0x80u)==0u && (err41&0x12u)==0u) qual=0u;

    if(qual) {
        bua_u8 vo=prior_on?STEP83_VSS_HOLD:STEP83_VSS_ON;
        bua_u8 to=prior_on?STEP83_TPS_HOLD:STEP83_TPS_ON;
        bua_u8 fo=prior_on?STEP83_FLOW_HOLD:STEP83_FLOW_ON;
        bua_u8 flow_hi=(bua_u8)(flow_q8_8>>8);
        /* All three executable tests use BLS: each input must be strictly greater. */
        if(vss_scaled<=vo || tps<=to || flow_hi<=fo) qual=0u;
    }
    if(qual) {
        maf=bua_ccp_maf83(flow_q8_8);
        (void)load; /* C27C is 128 at every calibrated point. */
        load_gain=128u;
        /* MUL then ROLB/ROLA is product*2, saturating on 17-bit overflow. */
        {
            unsigned int z=(unsigned int)maf*(unsigned int)load_gain*2u;
            req=(z>65535u)?255u:(bua_u8)((z>>8)&0xFFu);
        }
        if(s->pw113==0u || s->min114==0u || req<s->min114) s->min114=req;
        filtered=bua_lag_filter_8_8((bua_u16)((bua_u16)s->pw113<<8),req,STEP83_FILTER);
        s->pw113=(bua_u8)(filtered>>8);
        if(s->pw113>=s->min114) s->min114=0u;
    } else s->pw113=0u; /* LE808 */
    s->requested=req;
    s->status3b=(bua_u8)((s->status3b&0xFEu)|(s->pw113!=0u?1u:0u));
}

