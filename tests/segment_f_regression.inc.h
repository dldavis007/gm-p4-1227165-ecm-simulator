static void run_step88_segment_f_test(void)
{
    BuaSegF87 s;
    unsigned int pass=0u,total=0u;
#define S87(x) do { ++total; if(x) ++pass; } while(0)
    S87(STEP87_SAM_IDLE_CELL==0u && STEP87_SAM_NONIDLE_CELL==9u);
    S87(STEP87_SAM_IDLE_LIMIT==75u);
    S87(STEP87_SAM_COOL_LOW==171u && STEP87_SAM_COOL_HIGH==187u);
    S87(STEP87_SAM_A_COEF==16u && STEP87_SAM_B_COEF==24u);
    S87(step87_lf2bf9(step87_cool_mult,0u)==45u);
    S87(step87_lf2bf9(step87_cool_mult,128u)==19u);
    S87(step87_lf2bf9(step87_cool_mult,192u)==0u);
    S87(step87_lf2bf9(step87_ae_decay,0u)==64u);
    S87(step87_lf2bf9(step87_ae_decay,128u)==64u);
    S87(step87_lf2bf9(step87_ae_decay,192u)==128u);
    memset(&s,0,sizeof(s)); s.major_counter=0u; s.cell0_timer=74u; bua_segf87(&s);
    S87(s.cell0_timer==75u);
    memset(&s,0,sizeof(s)); s.major_counter=0u; s.af44=2u; s.coolant=171u;
    s.blm_cell=0u; s.cell0_timer=10u; s.cell_value=140u; s.sam_a=0x7800u; bua_segf87(&s);
    S87(s.sam_a>0x7800u);
    memset(&s,0,sizeof(s)); s.major_counter=0u; s.af44=2u; s.coolant=187u;
    s.blm_cell=0u; s.cell0_timer=10u; s.cell_value=140u; s.sam_a=0x7800u; bua_segf87(&s);
    S87(s.sam_a==0x7800u);
    memset(&s,0,sizeof(s)); s.major_counter=0u; s.af44=2u; s.coolant=180u;
    s.blm_cell=0u; s.cell0_timer=75u; s.cell_value=140u; s.sam_a=0x7800u; bua_segf87(&s);
    S87(s.sam_a==0x7800u);
    memset(&s,0,sizeof(s)); s.major_counter=0u; s.af44=2u; s.coolant=180u;
    s.blm_cell=9u; s.cell_value=140u; s.sam_b=0x7800u; bua_segf87(&s);
    S87(s.sam_b>0x7800u);
    memset(&s,0,sizeof(s)); s.major_counter=0x10u; s.coolant=0u; bua_segf87(&s);
    S87(s.ae_cool_factor==45u && s.ae_decay_factor==64u);
    memset(&s,0,sizeof(s)); s.major_counter=0x10u; s.coolant=128u; bua_segf87(&s);
    S87(s.ae_cool_factor==19u && s.ae_decay_factor==64u);
    memset(&s,0,sizeof(s)); s.major_counter=0x10u; s.coolant=192u; bua_segf87(&s);
    S87(s.ae_cool_factor==0u && s.ae_decay_factor==128u);
    /* Step 88 scheduler-facing reconciliation. */
    ecm_reset();
    MINOR_COUNT=0x10u;
    COOLANT=96u;
    RAM8(0x005Eu)=96u;
    RAM8(0x005Fu)=96u;
    ENGINE_MODE_WORD=0u;
    MODE_WORD1=0u;
    RAM8(0x00CCu)=77u;
    STARTUP_SPARK_REPEAT=55u;
    SPARK_AUX_MODE_WORD=(bua_u8)(SPARK_AUX_MODE_WORD|STARTUP_SPARK_PHASE_BIT);
    RAM8(0x0109u)=120u;
    segF_fuel_air_major();
    S87(ram16be_get(0x000Eu)==(bua_u16)(36u<<8));
    S87(ram16be_get(0x0010u)==240u);
    S87(RAM8(0x00CDu)==25u);
    S87(ram16be_get(STARTUP_SPARK_ADDR)==(bua_u16)(28u<<8));
    S87(ram16be_get(STARTUP_SPARK_DELAY_ADDR)==100u);
    S87(RAM8(0x00CCu)==77u && STARTUP_SPARK_REPEAT==55u);
    S87((SPARK_AUX_MODE_WORD&STARTUP_SPARK_PHASE_BIT)!=0u);
    S87(RAM8(0x010Cu)==90u && RAM8(0x0109u)==90u);
    S87(RAM8(0x00E7u)==45u && RAM8(0x00E3u)==44u);

    ram16be_set(0x000Eu,0x1234u);
    ram16be_set(0x0010u,0x0055u);
    ram16be_set(STARTUP_SPARK_ADDR,0x2222u);
    ram16be_set(STARTUP_SPARK_DELAY_ADDR,0x0033u);
    ENGINE_MODE_WORD=ENGINE_RUNNING_BIT;
    COOLANT=160u; RAM8(0x005Eu)=160u; RAM8(0x005Fu)=160u;
    RAM8(0x0109u)=40u;
    segF_fuel_air_major();
    S87(ram16be_get(0x000Eu)==0x1234u && ram16be_get(0x0010u)==0x0055u);
    S87(ram16be_get(STARTUP_SPARK_ADDR)==0x2222u && ram16be_get(STARTUP_SPARK_DELAY_ADDR)==0x0033u);
    S87(RAM8(0x00CDu)==25u);
    S87(RAM8(0x010Cu)==65u && RAM8(0x0109u)==40u);
    printf("Step 88 Segment-F integration regression: PASS %u/%u\n",pass,total);
#undef S87
}

#define STEP86_BATT_ON_RAW        90u
#define STEP86_BATT_OFF_RAW       40u
#define STEP86_BATT_HIGH_RAW     171u
#define STEP86_BURNOFF_TIME       10u
#define STEP86_BURNOFF_DELAY      50u

typedef struct BuaBatt86Tag {
    bua_u8 battery_7e;
    bua_u8 mode33;
    bua_u8 minor35;
    bua_u8 major3e;
    bua_u8 err4f;
    bua_u16 old_spark_b2;
    bua_u8 spark_fb_b4;
    bua_u8 burnoff_time_b7;
    bua_u8 burnoff_delay_b8;
    bua_u8 burn_diag_11f;
    bua_u8 burn_diag_120;
    bua_u8 shifter_read;
    bua_u8 force_discretes_off;
} BuaBatt86;

static BuaBatt86 bua_battery86_segment_e(bua_u8 raw,bua_u8 mode33,
                                          bua_u8 major3e,bua_u16 spark_period,
                                          bua_u16 old_spark,bua_u8 spark_fb,
                                          bua_u8 burn_time,bua_u8 burn_delay,
                                          bua_u8 burn_diag1,bua_u8 burn_diag2)
{
    BuaBatt86 r;
    r.battery_7e=raw;
    r.mode33=mode33;
    r.minor35=0u;
    r.major3e=major3e;
    r.err4f=0u;
    r.old_spark_b2=old_spark;
    r.spark_fb_b4=spark_fb;
    r.burnoff_time_b7=burn_time;
    r.burnoff_delay_b8=burn_delay;
    r.burn_diag_11f=burn_diag1;
    r.burn_diag_120=burn_diag2;
    r.shifter_read=0u;
    r.force_discretes_off=0u;

    if(raw>=STEP86_BATT_ON_RAW) {
        r.major3e=(bua_u8)(r.major3e&0xBFu);
        if((r.mode33&0x10u)!=0u) {
            r.old_spark_b2=spark_period;
            r.spark_fb_b4=0u;
        }
        /* LDD LC69B loads adjacent bytes LC69B=10 and LC69C=50. */
        r.burnoff_time_b7=STEP86_BURNOFF_TIME;
        r.burnoff_delay_b8=STEP86_BURNOFF_DELAY;
        r.burn_diag_11f=0u;
        r.burn_diag_120=0u;
        r.mode33=(bua_u8)(r.mode33&0xEFu);
        r.shifter_read=1u;
    } else if(raw<STEP86_BATT_OFF_RAW) {
        r.mode33=(bua_u8)(r.mode33|0x10u);
    }
    return r;
}

static BuaBatt86 bua_battery86_seg1_gate(bua_u8 raw,bua_u8 minor35,
                                          bua_u8 major3e,bua_u8 err4f)
{
    BuaBatt86 r;
    r.battery_7e=raw;
    r.mode33=0u;
    r.minor35=minor35;
    r.major3e=major3e;
    r.err4f=err4f;
    r.old_spark_b2=0u;
    r.spark_fb_b4=0u;
    r.burnoff_time_b7=0u;
    r.burnoff_delay_b8=0u;
    r.burn_diag_11f=0u;
    r.burn_diag_120=0u;
    r.shifter_read=0u;
    r.force_discretes_off=0u;

    if(raw>=STEP86_BATT_HIGH_RAW) {
        r.major3e=(bua_u8)(r.major3e|0x10u);
        if((r.minor35&0x40u)!=0u) {
            r.err4f=(bua_u8)(r.err4f|0x40u);
            r.force_discretes_off=1u;
        } else {
            r.minor35=(bua_u8)(r.minor35|0x40u);
        }
    } else {
        r.minor35=(bua_u8)(r.minor35&0xBFu);
    }
    return r;
}

