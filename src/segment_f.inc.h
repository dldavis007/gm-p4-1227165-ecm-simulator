/* ------------------------------------------------------------------------- */
/* Step 86: battery producer LDE6D..LDEA3 and Segment-1 high-voltage gate.   */
/*                                                                           */
/* Segment E reads battery A/D channel 1.  Raw >=90 takes the powered path,  */
/* clears the ignition-off state, clears burn-off-active L003E b6, reloads  */
/* the contiguous LC69B/LC69C burn-off time/delay bytes (10,50), clears the  */
/* burn-off diagnostic counters, and calls the shifter-read routine. Raw     */
/* <40 sets ignition-off; 40..89 leaves the prior ignition state unchanged.  */
/*                                                                           */
/* Segment 1 separately checks raw battery >=171.  The first qualifying      */
/* pass sets L0035 b6; a second consecutive qualifying pass sees b6 already  */
/* set, latches L004F b6 and jumps to the all-discretes-off path.  A sample  */
/* below 171 clears the one-pass qualifier. L003E b4 (skip MAF burn-off this */
/* startup) is set on every >=171 sample and is not cleared by this routine. */
/* ------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------
 * Step 88: Segment-F LEC23..LECE9 source-shaped integration
 * --------------------------------------------------------------------------
 * Step 87 translated the 200-ms SAM half and recurring coolant-derived AE
 * producers. Step 88 connects the remaining unambiguous LEC72..LECE9 stores
 * to the already-tested startup AFR/spark and warm-Park IAC implementations.
 *
 * LEC72 itself does not clear L00CC, L0012, or L003B b7. The standalone
 * startup helpers initialize those state items as part of their larger models,
 * so this scheduler wrapper preserves them around those calls. This reuses the
 * tested arithmetic while reproducing only Segment-F's actual side effects.
 * -------------------------------------------------------------------------- */
#define STEP87_SAM_IDLE_CELL       0u
#define STEP87_SAM_NONIDLE_CELL    9u
#define STEP87_SAM_IDLE_LIMIT     75u
#define STEP87_SAM_COOL_LOW      171u
#define STEP87_SAM_COOL_HIGH     187u
#define STEP87_SAM_A_COEF         16u
#define STEP87_SAM_B_COEF         24u

static const bua_u8 step87_cool_mult[9] =
    {45u,45u,45u,45u,19u,6u,0u,0u,0u};
static const bua_u8 step87_ae_decay[9] =
    {64u,64u,44u,44u,64u,102u,128u,153u,153u};

static bua_u8 step87_lf2bf9(const bua_u8 *table,bua_u8 arg)
{
    unsigned int pos,idx8,frac,a,b;
    pos=(unsigned int)arg*8u;
    idx8=pos>>8;
    frac=pos&255u;
    if(idx8>=8u) return table[8];
    a=table[idx8]; b=table[idx8+1u];
    if(b>=a) return (bua_u8)(a+(((b-a)*frac)>>8));
    return (bua_u8)(a-(((a-b)*frac)>>8));
}

typedef struct BuaSegF87Tag {
    bua_u8 major_counter;
    bua_u8 af44;
    bua_u8 coolant;
    bua_u8 blm_cell;
    bua_u8 cell0_timer;
    bua_u8 cell_value;
    bua_u16 sam_a;
    bua_u16 sam_b;
    bua_u8 ae_cool_factor;
    bua_u8 ae_decay_factor;
} BuaSegF87;

static bua_u16 step87_filter_word(bua_u16 oldv,bua_u8 input,bua_u8 coef)
{
    long target,diff,v;
    target=(long)((bua_u16)input<<8);
    v=(long)oldv;
    diff=target-v;
    v += (diff*(long)coef)/256L;
    if(v<0L) v=0L;
    if(v>65535L) v=65535L;
    return (bua_u16)v;
}

static void bua_segf87(BuaSegF87 *s)
{
    bua_u8 t;
    if((s->major_counter&0x10u)==0u) {
        t=(bua_u8)(s->cell0_timer+1u);
        if(t!=0u) s->cell0_timer=t;
        if((s->af44&0x02u)==0u) return;
        if(s->coolant<STEP87_SAM_COOL_LOW || s->coolant>=STEP87_SAM_COOL_HIGH) return;
        if(s->blm_cell==STEP87_SAM_IDLE_CELL) {
            if(s->cell0_timer>=STEP87_SAM_IDLE_LIMIT) return;
            s->sam_a=step87_filter_word(s->sam_a,s->cell_value,STEP87_SAM_A_COEF);
        } else if(s->blm_cell==STEP87_SAM_NONIDLE_CELL) {
            s->sam_b=step87_filter_word(s->sam_b,s->cell_value,STEP87_SAM_B_COEF);
        }
        return;
    }
    s->ae_cool_factor=step87_lf2bf9(step87_cool_mult,s->coolant);
    s->ae_decay_factor=step87_lf2bf9(step87_ae_decay,s->coolant);
}

static void bua_segf88_startup_and_iac(void)
{
    BuaStartupAfr48 a;
    bua_u8 saved_afr_repeat;
    bua_u8 saved_spark_repeat;
    bua_u8 saved_spark_phase;
    bua_u8 engine_running;
    bua_u8 warm;

    a.startup_enrich=ram16be_get(0x000Eu);
    a.initial_delay=ram16be_get(0x0010u);
    a.repeat_count=RAM8(0x00CCu);
    a.coolant_afr=RAM8(0x00CDu);
    saved_afr_repeat=a.repeat_count;
    engine_running=(bua_u8)(((ENGINE_MODE_WORD&ENGINE_RUNNING_BIT)!=0u)?1u:0u);

    a=bua_startup_afr_init_step48(RAM8(0x005Eu),engine_running,MODE_WORD1,a);
    ram16be_set(0x000Eu,a.startup_enrich);
    ram16be_set(0x0010u,a.initial_delay);
    RAM8(0x00CDu)=a.coolant_afr;
    RAM8(0x00CCu)=saved_afr_repeat;

    if(engine_running==0u && (MODE_WORD1&STEP48_NVRAM_VALID_BIT)==0u) {
        saved_spark_repeat=STARTUP_SPARK_REPEAT;
        saved_spark_phase=(bua_u8)(SPARK_AUX_MODE_WORD&STARTUP_SPARK_PHASE_BIT);
        bua_startup_spark_initialize(RAM8(0x005Fu));
        STARTUP_SPARK_REPEAT=saved_spark_repeat;
        SPARK_AUX_MODE_WORD=(bua_u8)((SPARK_AUX_MODE_WORD&
                                  (bua_u8)~STARTUP_SPARK_PHASE_BIT)|saved_spark_phase);
    }

    warm=bua_warm_park_step57(COOLANT);
    RAM8(0x010Cu)=warm;
    if(warm<RAM8(0x0109u)) RAM8(0x0109u)=warm;
}

static void segF_fuel_air_major(void)
{
    BuaSegF87 s;
    memset(&s,0,sizeof(s));
    s.major_counter=MINOR_COUNT;
    s.af44=RAM8(0x0044u);
    s.coolant=COOLANT;
    s.blm_cell=RAM8(0x00BFu);
    s.cell0_timer=RAM8(0x00C1u);
    s.sam_a=ram16be_get(0x000Au);
    s.sam_b=ram16be_get(0x000Cu);
    if(s.blm_cell<16u) s.cell_value=RAM8((bua_u16)(0x001Cu+s.blm_cell));
    bua_segf87(&s);
    RAM8(0x00C1u)=s.cell0_timer;
    ram16be_set(0x000Au,s.sam_a);
    ram16be_set(0x000Cu,s.sam_b);
    if((MINOR_COUNT&0x10u)!=0u) {
        RAM8(0x00E7u)=s.ae_cool_factor;
        RAM8(0x00E3u)=s.ae_decay_factor;
        bua_segf88_startup_and_iac();
    }
}

