/* ---------------------------------------------------------------------- */
/* Step 48: startup enrichment initialization and injector-event decay.    */
/*                                                                        */
/* LEC72 initializes L000E from C3E4 while the engine is not running      */
/* (unless NV mode b3 bypasses it), and initializes L0010 from C3F4*2.    */
/* Once running, LF695 services the 16-bit L000E value on injection       */
/* events: first L0010 counts down, then L00CC supplies the repeat delay,  */
/* and C410 is subtracted with 16-bit underflow clamped to zero. LDA2D    */
/* consumes only the high byte of L000E as the startup AFR contribution.  */
/* L00CD is independently refreshed from C41F by the AFR major loop.      */
/* ---------------------------------------------------------------------- */
typedef struct BuaStartupAfr48Tag {
    bua_u16 startup_enrich;  /* L000E:L000F */
    bua_u16 initial_delay;   /* L0010:L0011 */
    bua_u8 repeat_count;     /* L00CC */
    bua_u8 coolant_afr;      /* L00CD */
} BuaStartupAfr48;
static bua_u8 bua_step48_cool_arg(bua_u8 coolant)
{
    if (coolant > (bua_u8)STEP48_STARTUP_COOL_MAX)
        return (bua_u8)STEP48_STARTUP_COOL_MAX;
    return coolant;
}
static BuaStartupAfr48 bua_startup_afr_init_step48(bua_u8 filtered_coolant,
                                                    bua_u8 engine_running,
                                                    bua_u8 nv_mode_word,
                                                    BuaStartupAfr48 prior)
{
    BuaStartupAfr48 r;
    bua_u8 arg;
    bua_u8 enrich;
    bua_u8 delay;
    r = prior;
    arg = bua_step48_cool_arg(filtered_coolant);
    /* LECC2 is reached in all cases: L00CD follows current filtered coolant. */
    r.coolant_afr = bua_lookup_spacing16(cal_open_loop_coolant_step48,14u,arg);
    /* LEC72 skips startup reinitialization once running or when NV b3 is set. */
    if (engine_running != 0u || (nv_mode_word & STEP48_NVRAM_VALID_BIT) != 0u)
        return r;
    enrich = bua_lookup_spacing16(cal_startup_enrich_step48,14u,arg);
    r.startup_enrich = (bua_u16)((bua_u16)enrich << 8);
    delay = bua_lookup_spacing16(cal_startup_delay_step48,14u,arg);
    /* C3F2=512 passed to LF266 -> floor(delay*512/256) = delay*2. */
    r.initial_delay = (bua_u16)((bua_u16)delay * 2u);
    r.repeat_count = 0u;
    return r;
}
static BuaStartupAfr48 bua_startup_afr_inject_step48(BuaStartupAfr48 s,
                                                      bua_u8 startup_coolant,
                                                      bua_u8 engine_running)
{
    bua_u8 arg;
    bua_u8 amount;
    if (engine_running == 0u || (s.startup_enrich & 0xFF00u) == 0u)
        return s;
    /* LF695 gives the initial L0010 delay priority. */
    if (s.initial_delay != 0u) {
        --s.initial_delay;
        return s;
    }
    /* Then L00CC is a test-before-decrement repeat counter. */
    if (s.repeat_count != 0u) {
        --s.repeat_count;
        return s;
    }
    arg = bua_step48_cool_arg(startup_coolant);
    s.repeat_count = bua_lookup_spacing16(cal_startup_repeat_step48,14u,arg);
    amount = bua_lookup_spacing16(cal_startup_decay_step48,14u,arg);
    if (s.startup_enrich < (bua_u16)amount)
        s.startup_enrich = 0u;
    else
        s.startup_enrich = (bua_u16)(s.startup_enrich - (bua_u16)amount);
    return s;
}

/* Step 98: scheduler-facing LF67B..LF768 injector-service path.  The PC HAL
 * raises the simulated L00A0 b6 at the already-established physical cadence
 * of one all-eight batch service per crank revolution.  This routine consumes
 * that status exactly once, performs startup AFR/spark service only while
 * running, then maps the proven LF71C bookkeeping result back to source RAM.
 * It does not emit another physical pulse or write L3FD0. */
static void bua_common_injector_service_6p25ms(void)
{
    BuaStartupAfr48 a;
    BuaInjectorEventResult e;

    RAM8(0x00A0u)&=(bua_u8)~0x40u;
    if(sim_ecm_ref.injector_services_pending==0u)
        return;
    --sim_ecm_ref.injector_services_pending;
    RAM8(0x00A0u)|=0x40u;
    ++stats.common_injector_bookkeeping;

    /* LF682: LC225=0 selects the expected eight-cylinder PFI mode. */
    if((RAM8(0x002Fu)&0x18u)!=0u)
        DIAG_MODE_WORD2|=0x01u;

    if((ENGINE_MODE_WORD&ENGINE_RUNNING_BIT)!=0u) {
        a.startup_enrich=ram16be_get(0x000Eu);
        a.initial_delay=ram16be_get(0x0010u);
        a.repeat_count=RAM8(0x00CCu);
        a.coolant_afr=RAM8(0x00CDu);
        a=bua_startup_afr_inject_step48(a,STARTUP_COOLANT,1u);
        ram16be_set(0x000Eu,a.startup_enrich);
        ram16be_set(0x0010u,a.initial_delay);
        RAM8(0x00CCu)=a.repeat_count;
        bua_startup_spark_on_inject(STARTUP_COOLANT);
        ++stats.running_injector_bookkeeping;
    }

    e=bua_injector_event_lf71c(INJECT_MODE_WORD,AIR_MODE_WORD,
                                RAM8(0x0038u),RAM8(0x003Eu),
                                ENGINE_MODE_WORD,ram16be_get(0x00C2u),
                                ram16be_get(0x011Cu),ram16be_get(0x011Au),
                                ram16be_get(0x00D9u),RAM8(0x00E0u),
                                RAM8(0x00DFu),RAM8(0x018Du));
    INJECT_MODE_WORD=e.single_fire_word;
    AIR_MODE_WORD=e.air_mode_word;
    RAM8(0x0038u)=e.ae_flags;
    RAM8(0x003Eu)=e.decel_flags;
    RAM8(0x00E0u)=e.fuel_limit_inject_count;
    RAM8(0x00DFu)=e.async_count;
    RAM8(0x018Du)=e.running_inject_counter;
    ram16be_set(0x00D5u,e.last_base_pw);
    ram16be_set(0x011Au,e.ae_pw_accumulator);
    ram16be_set(0x00D9u,e.accumulated_ae);
}

