/* Step 114: normal startup initializer, listing $C93B..$C9F3. */
#define STEP114_O2_INITIAL             102u /* LC3A0 */
#define STEP114_MIN_TPS                 35u /* LC39B */
#define STEP114_HOT_RESTART_LIMIT      113u /* LC02C */
#define STEP114_ERR32_COOL_LIMIT        94u /* LC1FA */

typedef struct BuaStartupTrace114Tag {
    bua_u8 segment6_called;
    bua_u8 enrichment_initialized;
    bua_u8 fmd_read_boundary;
    bua_u8 segment_e_called;
    bua_u8 sci_device_boundary;
    bua_u8 csr_updated;
    bua_u8 timer_armed;
    bua_u8 scheduler_handoff;
} BuaStartupTrace114;
static BuaStartupTrace114 bua_startup_trace114;
/* PC/HAL stimulus for LF1E0. Bit 5 is the only bit consumed at $C986. */
static bua_u8 sim_startup_fmd_status=0x20u;

static void bua_startup_sci_direct_init_step114(void)
{
    RAM8(0x012Du)=0u;
    ram16be_set(0x0131u,0u);
    RAM8(0x012Fu)=0u;
    RAM8(0x0130u)=0u;
    /* LFA69's device-control-block walk belongs to the later SCI audit. */
    bua_startup_trace114.sci_device_boundary=1u;
}

static void bua_startup_csr_set_step114(bua_u8 mask)
{
    bua_u16 csr;
    bua_u8 hi;
    bua_u8 lo;
    csr=mpu16be_get(0x3FFCu);
    hi=(bua_u8)(csr>>8);
    lo=(bua_u8)csr;
    lo=(bua_u8)(lo|mask);
    lo=(bua_u8)((lo&0xFEu)|0x02u);
    hi=(bua_u8)(hi|0xFBu);
    mpu16be_set(0x3FFCu,(bua_u16)(((bua_u16)hi<<8)|lo));
    bua_startup_trace114.csr_updated=1u;
}

static void bua_startup_normal_step114(void)
{
    bua_u8 coolant;
    bua_u8 restart_coolant;
    bua_u8 enrich;
    bua_u8 spark;
    memset(&bua_startup_trace114,0,sizeof(bua_startup_trace114));

    ALDL_MODE_WORD=(bua_u8)(ALDL_MODE_WORD|0x80u);
    seg6_coolant_adc();
    bua_startup_trace114.segment6_called=1u;

    if((MODE_WORD1&0x08u)==0u) {
        coolant=COOLANT;
        if(coolant>208u)
            coolant=208u;
        enrich=bua_lookup_spacing16(cal_startup_enrich_step48,14u,coolant);
        ram16be_set(0x000Eu,(bua_u16)((bua_u16)enrich<<8));
        RAM8(0x00CDu)=bua_lookup_spacing16(cal_open_loop_coolant_step48,14u,
                                           coolant);
        spark=bua_lookup_fixed16_table(cal_startup_spark,
                                       (bua_u8)CAL_STARTUP_SPARK_POINTS,
                                       coolant);
        RAM8(0x0013u)=spark;
        bua_startup_trace114.enrichment_initialized=1u;
    }

    restart_coolant=RAM8(0x005Du);
    SPARK_AUX_MODE_WORD=(bua_u8)(SPARK_AUX_MODE_WORD&0xEFu);
    if(restart_coolant>=STEP114_HOT_RESTART_LIMIT)
        MODE_WORD1=(bua_u8)(MODE_WORD1|0x40u);
    if(restart_coolant<STEP114_ERR32_COOL_LIMIT &&
       ram16be_get(0x001Au)==0u) {
        bua_startup_trace114.fmd_read_boundary=1u;
        if((sim_startup_fmd_status&0x20u)==0u)
            DIAG_MODE_WORD3=(bua_u8)(DIAG_MODE_WORD3|0x20u);
    }

    RAM8(0x006Fu)=STEP114_O2_INITIAL;
    RAM8(0x0071u)=STEP114_O2_INITIAL;
    RAM8(0x0073u)=STEP114_O2_INITIAL;
    RAM8(0x0086u)=STEP114_MIN_TPS;
    bua_common_tps_6p25ms();

    segE_tcc_adc();
    bua_startup_trace114.segment_e_called=1u;
    ram16be_set(0x00B2u,mpu16be_get(0x3FC8u));
    ram16be_set(0x00CEu,(bua_u16)CAL_STOICH_CODE);
    if((MODE_WORD1&0x08u)==0u)
        RAM8(0x0004u)=7u;

    bua_startup_sci_direct_init_step114();
    bua_startup_csr_set_step114(0x04u);
    MINOR_COUNT=0x0Eu;
    BLM=128u;
    RAM8(0x00C6u)=128u;
    RAM8(0x00C9u)=128u;
    ALCL_TABLE_INDEX=0x19u;
    RAM8(0x0068u)=160u;
    RAM8(0x010Cu)=144u;

    mem.io4000[6]=(bua_u8)(mem.io4000[5]+2u);
    mem.io4000[7]=(bua_u8)(mem.io4000[7]|0x01u);
    bua_startup_trace114.timer_armed=1u;
    bua_startup_trace114.scheduler_handoff=1u;
}

/* A real power-on path, distinct from ecm_reset's frozen PC-test fixture. */
static void ecm_power_on_step114(void)
{
    ecm_reset();
    bua_startup_normal_step114();
}
