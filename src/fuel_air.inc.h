/* -------------------------------------------------------------------------- */
/* Step 31: integrated MAF -> fuel -> ordinary injector-output chain.          */
/*                                                                            */
/* This is deliberately an arithmetic/data-flow integration test, not a       */
/* scheduler claim.  It uses the already translated source routines in their   */
/* executable order and writes the same RAM values consumed downstream.        */
/* -------------------------------------------------------------------------- */
typedef struct {
    BuaMafResult maf;
    BuaAfrResult afr;
    bua_u16 base_pw;
    bua_u16 corrected_pw;
    BuaFuelOutputResult output;
} BuaSensorFuelResult;
static void bua_maf_filter_seed(BuaMafFilterState *st, bua_u16 q)
{
    unsigned int i;
    for (i = 0u; i < 4u; ++i)
        st->samples[i] = q;
    st->next = 0u;
    st->filtered = q;
}
static BuaSensorFuelResult bua_sensor_to_injector_step31(BuaMafFilterState *st,
                                                          bua_u8 maf_adc,
                                                          bua_u8 rpm25,
                                                          bua_u16 ref_period,
                                                          bua_u8 startup_enrich_hi,
                                                          bua_u8 closed_loop,
                                                          bua_u8 coolant_afr_term,
                                                          bua_u8 load_axis,
                                                          bua_u8 blm,
                                                          bua_u8 closed_corr,
                                                          bua_u8 prop_step,
                                                          bua_u8 injector_batt_offset)
{
    BuaSensorFuelResult r;
    bua_u8 cl_word;
    memset(&r, 0, sizeof(r));
    r.maf = bua_maf_step30_analog(st, maf_adc, rpm25);
    ram16be_set(AIRFLOW_WORD_ADDR, r.maf.final_q8_8);
    cl_word = closed_loop ? AF_CLOSED_LOOP_BIT : 0u;
    r.afr = bua_afr_lda2d(startup_enrich_hi, cl_word,
                          coolant_afr_term, load_axis);
    ram16be_set(TOTAL_AFR_ADDR, r.afr.afr_code);
    ram16be_set(REF_PERIOD_ADDR, ref_period);
    ENGINE_MODE_WORD |= ENGINE_RUNNING_BIT;
    BLM = blm;
    CLOSED_LOOP_PW_CORR = closed_corr;
    r.base_pw = bua_base_pw_ldd2d();
    r.corrected_pw = bua_apply_blm_closed_loop_ldd7a(r.base_pw, blm,
                                                      closed_corr, prop_step,
                                                      0u, AIR_MODE_WORD);
    ram16be_set(0x00C2u, r.corrected_pw);
    r.output = bua_fuel_output_lf92a(r.corrected_pw,
                                      FUEL_VATS_OK_BIT,
                                      0u,
                                      0u,
                                      ENGINE_RUNNING_BIT,
                                      0u,
                                      0u,
                                      ref_period,
                                      injector_batt_offset);
    return r;
}

/* -------------------------------------------------------------------------- */
/* Steps 94-95/101/103: scheduler-facing LF76A MAF and LD6D1 fuel selection. */
/*                                                                            */
/* LF76A is reached from the common LF67B injection/IAC tail before LCC65     */
/* selects odd LD6D1 or even LCD05.  Therefore MAF production is 6.25-ms      */
/* common work, not an odd-loop operation.  Step 95 adds LD769..LD7A0 at the  */
/* head of the odd wrapper; the remaining path preserves executable order:    */
/* fast O2 filter, AFR selection, BLM cell selection, closed-loop correction, */
/* base PW/BLM correction, LF901 AE addition, then LF92A MPU output shaping.   */
/* Step 101 adds the literal LD7A0 crank/normal selection after load production. */
/* Step 103 connects LDA73..LDB35 BLM selection. Ignition-off remains outside. */
/* Steps 96-97 connect DFCO and the 50-ms BLM update tail.                     */
/* -------------------------------------------------------------------------- */
#define STEP94_FAST_O2_COEF 240u       /* LC39E */
#define STEP103_BLM_DEFAULT             120u /* LF434 */
#define STEP103_BLM_CELL_COUNT           16u
#define STEP103_BLM_MAX                 160u /* LC5E2 */
#define STEP103_BLM_MIN                 108u /* LC5E3 */
#define STEP103_BLM_RPM_HYST               4u /* LC5DB */
#define STEP103_BLM_FLOW_HYST              2u /* LC5DC */
static const bua_u8 step103_blm_rpm_bounds[3] = {28u,48u,80u};
static const bua_u8 step103_blm_flow_bounds[3] = {12u,22u,34u};
typedef struct {
    bua_u8 cell;
    bua_u8 current_blm;
    bua_u8 af_mode_word;
    bua_u8 air_mode_word;
    bua_u8 changed;
    bua_u8 reinitialized;
} BuaBlmSelect103;

/* LDA73..LDB35. LC017=0 selects the high byte of L00EA (whole g/s), not
 * filtered load.  Cell layout is flow_band*4 + rpm_band, yielding 0..15. */
static BuaBlmSelect103 bua_blm_select_step103(bua_u8 current_cell,
                                               bua_u8 rpm25,
                                               bua_u8 flow_whole,
                                               bua_u8 af_mode_word,
                                               bua_u8 air_mode_word,
                                               bua_u8 *cells)
{
    BuaBlmSelect103 r;
    bua_u8 rpm_band;
    bua_u8 flow_band;
    bua_u8 stays;
    bua_u8 value;
    unsigned int i;
    r.cell=current_cell;
    r.current_blm=128u;
    r.af_mode_word=(bua_u8)(af_mode_word &
                             (bua_u8)~AF_BLM_CELL_CHANGE_BIT);
    r.air_mode_word=air_mode_word;
    r.changed=0u;
    r.reinitialized=0u;
    if(cells==(bua_u8 *)0 || current_cell>=STEP103_BLM_CELL_COUNT)
        return r;

    rpm_band=(bua_u8)(current_cell&0x03u);
    flow_band=(bua_u8)((current_cell&0x0Cu)>>2);
    stays=1u;
    if(rpm_band!=0u &&
       rpm25<(bua_u8)(step103_blm_rpm_bounds[rpm_band-1u]-
                      STEP103_BLM_RPM_HYST))
        stays=0u;
    if(rpm_band!=3u &&
       rpm25>(bua_u8)(step103_blm_rpm_bounds[rpm_band]+
                      STEP103_BLM_RPM_HYST))
        stays=0u;
    if(flow_band!=0u &&
       flow_whole<(bua_u8)(step103_blm_flow_bounds[flow_band-1u]-
                           STEP103_BLM_FLOW_HYST))
        stays=0u;
    if(flow_band!=3u &&
       flow_whole>(bua_u8)(step103_blm_flow_bounds[flow_band]+
                           STEP103_BLM_FLOW_HYST))
        stays=0u;

    if(stays==0u) {
        rpm_band=0u;
        while(rpm_band<3u && rpm25>=step103_blm_rpm_bounds[rpm_band])
            ++rpm_band;
        flow_band=0u;
        while(flow_band<3u && flow_whole>=step103_blm_flow_bounds[flow_band])
            ++flow_band;
        r.cell=(bua_u8)(rpm_band+(bua_u8)(flow_band<<2));
        r.af_mode_word|=0x0Cu; /* b2 cell change + b3 learning delay */
        r.changed=1u;
    }

    value=cells[r.cell];
    if(value>STEP103_BLM_MAX || value<STEP103_BLM_MIN) {
        /* LDB1C calls LF434. LF434 starts with CLRB, so LDB25 then selects
         * cell zero as well as replacing every matrix byte with 120. */
        r.air_mode_word|=0x40u;
        for(i=0u;i<STEP103_BLM_CELL_COUNT;++i)
            cells[i]=STEP103_BLM_DEFAULT;
        r.cell=0u;
        value=STEP103_BLM_DEFAULT;
        r.reinitialized=1u;
    }
    r.current_blm=value;
    /* LDB25 stores the raw cell first. During PE only, LDB2D forces a
     * sub-128 selected value to neutral 128 for current fuel correction. */
    if((r.af_mode_word&AF_POWER_ENRICH_BIT)!=0u && value<128u)
        r.current_blm=128u;
    return r;
}

static void bua_scheduler_blm_select_step103(void)
{
    BuaBlmSelect103 r;
    r=bua_blm_select_step103(RAM8(0x00BFu),RPM_DIV25,
                              RAM8(AIRFLOW_WORD_ADDR),AF_MODE_WORD,
                              AIR_MODE_WORD,&mem.low[0x001Cu]);
    RAM8(0x00BFu)=r.cell;
    BLM=r.current_blm;
    AF_MODE_WORD=r.af_mode_word;
    AIR_MODE_WORD=r.air_mode_word;
    ++stats.scheduler_blm_select_calls;
    if(r.changed!=0u) ++stats.scheduler_blm_cell_changes;
    if(r.reinitialized!=0u) ++stats.scheduler_blm_reinitializations;
}

/* LDB35..LDB9B executes before the closed-loop gate, so O2-ready production
 * and the fast rich/lean state must run even while L0044 b7 is still clear. */
static bua_u8 bua_scheduler_fast_o2_front_step103(void)
{
    bua_u8 ready;
    bua_u8 rich;
    bua_u8 old_word;
    bua_u8 new_word;
    old_word=AF_MODE_WORD2;
    rich=bua_o2_fast_rich_ldb35(ram16be_get(AIRFLOW_WORD_ADDR),
                    O2_FILTERED_FAST,FAST_O2_PREVIOUS,O2_LEAN_OFFSET,
                    (bua_u8)((RAM8(0x003Eu)&0x02u)!=0u),&ready);
    if(ready!=0u) {
        RAM8(0x00BEu)=0u;
        RAM8(0x0001u)|=0x01u;
    }
    new_word=(bua_u8)(old_word&0xBFu);
    if(rich!=0u) new_word|=0x40u;
    if(new_word!=old_word) {
        RAM8(0x00F1u)=(bua_u8)(RAM8(0x00F1u)+1u);
        RAM8(0x003Cu)|=0x10u;
        CLOSED_LOOP_DURATION=0u;
    } else if(CLOSED_LOOP_DURATION!=255u) {
        ++CLOSED_LOOP_DURATION;
    }
    AF_MODE_WORD2=new_word;
    return rich;
}
static BuaMafFilterState step94_maf_state;

static void bua_step94_fuel_state_reset(void)
{
    memset(&step94_maf_state, 0, sizeof(step94_maf_state));
    step94_maf_state.filtered = CAL_MAF_MIN_FLOW_Q8_8;
}

static void bua_common_maf_6p25ms(void)
{
    BuaMafResult r;
    bua_u8 raw;
    raw = hw_adc(0xA0u);
    RAM8(0x00EDu) = raw;
    r = bua_maf_step30_analog(&step94_maf_state, raw, RPM_DIV25);
    ram16be_set(AIRFLOW_WORD_ADDR, r.final_q8_8);
    ++stats.common_maf_calls;
}

/* Step 99, LCC21/LF35A common TPS producer plus LC30D transient filter.
 * The physical A/D source remains in the HAL.  L0086 is the 8.8 learned
 * closed-throttle voltage, L0082 the normalized load-axis byte, and L00DD
 * the 8.8 transient TPS consumed by LFA1E. */
static void bua_common_tps_6p25ms(void)
{
    bua_u8 raw;
    bua_u8 used;
    bua_u8 baseline;
    bua_u8 delta;
    bua_u16 d;
    bua_u16 filtered;

    raw=hw_adc(0x50u);
    RAM8(0x0081u)=raw;
    if(raw>246u) {
        RAM8(0x003Fu)|=0x04u;
        RAM8(0x004Cu)|=0x04u;
    }
    used=((RAM8(0x003Fu)&0x24u)!=0u)?48u:raw;
    baseline=RAM8(0x0086u);
    if(used<=baseline) {
        filtered=bua_lag_filter_8_8(ram16be_get(0x0086u),used,1u);
        ram16be_set(0x0086u,filtered);
    }
    d=(bua_u16)(ram16be_get(0x0086u)+0x0080u);
    baseline=(bua_u8)(d>>8);
    if(used<baseline)
        delta=0u;
    else
        delta=(bua_u8)(used-baseline);
    d=(bua_u16)((bua_u16)delta*112u+32u);
    if((d&0x8000u)!=0u) {
        TPS_LOAD_AXIS=255u;
    } else {
        d=(bua_u16)(d<<1);
        if((d&0x8000u)!=0u)
            TPS_LOAD_AXIS=255u;
        else {
            d=(bua_u16)(d<<1);
            TPS_LOAD_AXIS=(bua_u8)(d>>8);
        }
    }
    filtered=bua_lag_filter_8_8(ram16be_get(0x00DDu),TPS_LOAD_AXIS,32u);
    ram16be_set(0x00DDu,filtered);
    ++stats.common_tps_calls;
}

/* Step 95, LD769..LD7A0.  L0083 receives the previous TPS load-axis byte,
 * then LF2F0 forms the middle word of L00EA * L0095.  The source shifts the
 * prior load samples through L0061/L0062 before applying LC69A through LF472.
 * Its final ROL/ASL/ADC sequence doubles, saturates, and rounds the result to
 * the new 8-bit L0063 load value. */
static void bua_odd_load_producer_12p5ms(void)
{
    bua_u16 ref_period;
    bua_u16 middle;
    bua_u16 scaled;
    bua_u16 doubled;
    bua_u16 rounded_high;
    bua_u16 upper;
    bua_u8 old_load;
    bua_u8 old_history;

    /* LD744 initializes the closed-throttle filter on not-running passes. */
    if((ENGINE_MODE_WORD&ENGINE_RUNNING_BIT)==0u)
        ram16be_set(0x0086u,(bua_u16)(35u<<8));
    RAM8(0x0083u) = TPS_LOAD_AXIS;
    ref_period = ram16be_get(REF_PERIOD_ADDR);
    /* LC357 is literally zero in this image.  The unsigned CPX/BCS test can
     * never branch around the load, so every not-running pass selects zero. */
    if ((ENGINE_MODE_WORD & ENGINE_RUNNING_BIT) == 0u)
        ref_period = (bua_u16)CAL_CRANK_REF_LIMIT;

    middle = bua_mul_u16_u16_lf2f0(ram16be_get(AIRFLOW_WORD_ADDR),
                                    ref_period, &upper);
    old_load = FILTERED_LOAD;
    old_history = RAM8(0x0061u);
    RAM8(0x0061u) = old_load;
    RAM8(0x0062u) = old_history;

    scaled = bua_factor_u8_u16_lf472((bua_u8)CAL_LOAD_SCALE, middle);
    if (scaled > 0x7FFFu)
        doubled = 0xFFFFu;
    else
        doubled = (bua_u16)(scaled << 1);
    rounded_high = (bua_u16)(doubled >> 8);
    if ((doubled & 0x0080u) != 0u)
        ++rounded_high;
    if (rounded_high > 255u)
        rounded_high = 255u;
    FILTERED_LOAD = (bua_u8)rounded_high;
    ++stats.odd_load_producer_calls;
}

static void bua_odd_write_lf92a(bua_u16 pulse_width)
{
    BuaFuelOutputResult out;
    out = bua_fuel_output_lf92a(pulse_width, MODE_WORD3, IGN_MODE_WORD,
                                AF_MODE_WORD, ENGINE_MODE_WORD,
                                INJECT_MODE_WORD, VSS_MPH_HI,
                                ram16be_get(REF_PERIOD_ADDR), RAM8(0x00CBu));
    AF_MODE_WORD = out.af_mode_word;
    IGN_MODE_WORD = out.ign_mode_word;
    ram16be_set(0x011Cu, out.pending_pw);
    mpu16be_set(0x3FD0u, out.hardware_pw);
    if ((INJECT_MODE_WORD & 0xC0u) != 0xC0u)
        ram16be_set(0x0466u, out.hardware_pw);
    ++stats.odd_mpu_fuel_writes;
}

/* Step 101: LD7B1..LD865 scheduler-facing cranking branch.  C35B is all
 * zero in this image, so the post-construction RPM multiplier cannot alter
 * the pulse, but L0127 filtering and L0129 clearing remain real side effects. */
static void bua_odd_crank_fuel_12p5ms(void)
{
    bua_u16 pw;
    bua_u16 ref_period;
    bua_u16 doubled;
    bua_u16 quotient;
    bua_u16 filtered;

    pw=bua_crank_pw_step43(COOLANT,TPS_LOAD_AXIS,COLD_START_REF_COUNT);
    ram16be_set(0x00C2u,pw);
    if(pw!=0u) {
        ref_period=ram16be_get(REF_PERIOD_ADDR);
        doubled=(bua_u16)(ref_period<<1);
        if(doubled==0u || doubled<ref_period)
            quotient=0xFFFFu;
        else
            quotient=bua_fractional_divide(2457u,doubled);
        filtered=bua_lag_filter_8_8(ram16be_get(CRANK_RPM_FILTER_ADDR),
                                     (bua_u8)(quotient>>8),
                                     (bua_u8)CAL_CRANK_RPM_COEF);
        ram16be_set(CRANK_RPM_FILTER_ADDR,filtered);
        /* C35B's selected byte is always zero in 9340. */
        CRANK_FF_COUNTER=0u;
    }
    bua_odd_write_lf92a(pw);
    ++stats.odd_crank_fuel_calls;
    bua_odd_50ms_tail_lddc2();
}

static void bua_odd_normal_fuel_after_load_12p5ms(void)
{
    BuaAfrSelectResult afr;
    BuaO2ControlResult o2;
    bua_u16 filtered_o2;
    bua_u16 corrected;
    bua_u8 prop_step;
    bua_u8 ae_factor;
    bua_u8 ae_flags;
    bua_u8 fast_rich;

    bua_odd_transient_front_12p5ms();

    /* LD9A9: save the previous whole-byte sample, read channel 2, then
     * LF250-filter the new sample into L006F with LC39E=240. */
    FAST_O2_PREVIOUS = O2_FILTERED_FAST;
    filtered_o2 = bua_lag_filter_8_8(ram16be_get(0x006Fu),
                                      hw_adc(0x20u),
                                      (bua_u8)STEP94_FAST_O2_COEF);
    ram16be_set(0x006Fu, filtered_o2);

    /* LD9D7..LDA5A: Mode-4 has priority, then PE, then normal AFR. */
    afr = bua_select_afr_step47(RPM_DIV25, FILTERED_LOAD, TPS_LOAD_AXIS,
                                COOLANT, RAM8(0x000Eu), AF_MODE_WORD2,
                                RAM8(0x00CDu), AF_MODE_WORD, INJECT_MODE_WORD,
                                MINOR_MODE_WORD2, ALDL_FUNC_MOD_ENABLE,
                                RAM8(0x015Au));
    AF_MODE_WORD = afr.af_mode_word;
    INJECT_MODE_WORD = afr.single_fire;
    ram16be_set(TOTAL_AFR_ADDR, afr.afr.afr_code);

    /* LDA73..LDB35 selects/validates the current BLM before O2 and BPW use. */
    bua_scheduler_blm_select_step103();

    O2_LOOKUP_AIRFLOW = bua_o2_airflow_argument(ram16be_get(AIRFLOW_WORD_ADDR));
    fast_rich=bua_scheduler_fast_o2_front_step103();
    prop_step = 0u;
    if ((AF_MODE_WORD2 & AF_CLOSED_LOOP_BIT) != 0u) {
        o2 = bua_closed_loop_o2_core_step103(ram16be_get(AIRFLOW_WORD_ADDR),
                                       fast_rich,O2_FILTERED_SLOW,O2_LEAN_OFFSET,
                                       (bua_u8)((RAM8(0x003Eu)&0x02u)!=0u),
                                       TPS_LOAD_AXIS, VSS_SCALED, RPM_SPECIAL,
                                       COOLANT, CLOSED_LOOP_DURATION,
                                       CLOSED_LOOP_INTEGRATOR,
                                       CLOSED_LOOP_INT_TIMER);
        CLOSED_LOOP_INTEGRATOR = o2.integrator;
        CLOSED_LOOP_INT_TIMER = o2.int_timer;
        CLOSED_LOOP_PW_CORR = o2.closed_corr;
        O2_ERROR_LIMITED = o2.limited_error;
        O2_PROPORTIONAL_STEP = o2.prop_step;
        AF_MODE_WORD2 = (bua_u8)(AF_MODE_WORD2 & (bua_u8)~0x20u);
        if (o2.slow_rich != 0u) AF_MODE_WORD2 |= 0x20u;
        prop_step = o2.prop_step;
    }

    corrected = bua_fuel_base_and_corrections_step23(prop_step);
    ae_factor = ACCEL_LOAD_FACTOR;
    ae_flags = RAM8(0x0038u);
    corrected = bua_ae_apply_lf901(corrected, &ae_factor,
                                   RAM8(0x00E3u), &ae_flags);
    ACCEL_LOAD_FACTOR = ae_factor;
    RAM8(0x0038u) = ae_flags;
    ram16be_set(0x00C2u, corrected);

    bua_odd_write_lf92a(corrected);
    /* LDD9C executable order: LF901/LF92A, LE436, then LDDC2. */
    bua_scheduler_async_le436_step102();
    ++stats.odd_normal_fuel_calls;
    bua_odd_50ms_tail_lddc2();
}

static void bua_odd_fuel_chain_12p5ms(void)
{
    bua_odd_load_producer_12p5ms();
    if(bua_select_fuel_path_step44(ENGINE_MODE_WORD,CRANK_FF_COUNTER)==
       BUA_FUEL_PATH_CRANK44)
        bua_odd_crank_fuel_12p5ms();
    else
        bua_odd_normal_fuel_after_load_12p5ms();
    ++stats.odd_fuel_chain_calls;
}

