/* ------------------------------------------------------------------------- */
/* Step 84: segment-A MAT producer / ERR23 and ERR25, LEBB3..LEC22.          */
/* ------------------------------------------------------------------------- */
#define STEP84_MAT_LOW_THRESH      4u   /* LC1EC */
#define STEP84_MAT_LOW_TIME      120u   /* LC1ED */
#define STEP84_MAT_DIAG_DELAY    240u   /* LC1EE, raw engine-run-time word */
#define STEP84_MAT_VSS_THRESH      1u   /* LC1F0 */
#define STEP84_MAT_DEFAULT        29u   /* LC1F1 */
#define STEP84_MAT_HIGH_THRESH   243u   /* LC1F8 */
#define STEP84_MAT_HIGH_TIME     120u   /* LC1F9 */

typedef struct BuaMat84Tag {
    bua_u8 ad_inverted;
    bua_u8 mat;
    bua_u8 low_timer;
    bua_u8 high_timer;
    bua_u8 af_mode43;
    bua_u8 diag_mode40;
    bua_u8 err23_latched4c;
    bua_u8 err25_latched4d;
} BuaMat84;

static BuaMat84 bua_mat84_step(bua_u8 ad_raw, bua_u16 run_time, bua_u8 vss,
                               bua_u8 low_timer, bua_u8 high_timer,
                               bua_u8 af_mode43, bua_u8 diag_mode40,
                               bua_u8 err23_latched4c, bua_u8 err25_latched4d)
{
    BuaMat84 r;
    bua_u8 mat=(bua_u8)(~ad_raw);
    r.ad_inverted=mat; r.mat=mat; r.low_timer=low_timer; r.high_timer=high_timer;
    r.af_mode43=af_mode43; r.diag_mode40=diag_mode40;
    r.err23_latched4c=err23_latched4c; r.err25_latched4d=err25_latched4d;

    /* CPX LC1EE / BLS LEC22: no ERR23/25 work through equality. */
    if(run_time<=STEP84_MAT_DIAG_DELAY) return r;

    /* ERR23 low-MAT path.  Preserve the executable exactly: its BHI after
       CMPB LC1F0 clears the low timer when VSS > 1, despite the source
       calibration comment describing LC1F0 as a minimum-speed enable. */
    if(mat>=STEP84_MAT_LOW_THRESH) {
        r.af_mode43=(bua_u8)(r.af_mode43&0xEFu);
        r.low_timer=0u;
    } else if(r.low_timer>STEP84_MAT_LOW_TIME) {
        r.af_mode43=(bua_u8)(r.af_mode43|0x10u);
    } else if(vss>STEP84_MAT_VSS_THRESH) {
        r.low_timer=0u;
    } else {
        r.low_timer=(bua_u8)(r.low_timer+1u);
    }

    /* ERR25 high-MAT path: here the executable uses the opposite VSS sense;
       VSS <= 1 clears the timer and VSS > 1 permits accumulation. */
    if(mat<STEP84_MAT_HIGH_THRESH) {
        r.diag_mode40=(bua_u8)(r.diag_mode40&0xFDu);
        r.high_timer=0u;
    } else if(r.high_timer>STEP84_MAT_HIGH_TIME) {
        r.diag_mode40=(bua_u8)(r.diag_mode40|0x02u);
    } else if(vss<=STEP84_MAT_VSS_THRESH) {
        r.high_timer=0u;
    } else {
        r.high_timer=(bua_u8)(r.high_timer+1u);
    }

    if((r.af_mode43&0x10u)!=0u) {
        r.err23_latched4c=(bua_u8)(r.err23_latched4c|0x01u);
        r.mat=(bua_u8)STEP84_MAT_DEFAULT;
    } else if((r.diag_mode40&0x02u)!=0u) {
        r.err25_latched4d=(bua_u8)(r.err25_latched4d|0x40u);
        r.mat=(bua_u8)STEP84_MAT_DEFAULT;
    }
    return r;
}

