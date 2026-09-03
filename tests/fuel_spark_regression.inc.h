static void run_step29_injector_event_test(void)
{
    BuaMemory saved_mem;
    BuaStats saved_stats;
    BuaInjectorEventResult r;
    unsigned int passed;
    saved_mem = mem;
    saved_stats = stats;
    passed = 0u;
    printf("\nInjector-event state regression (LF71C..LF768):\n");
    r = bua_injector_event_lf71c(0x10u, 0u, 0u, 0u,
                                 FUEL_ENGINE_RUNNING_BIT,
                                 300u, 120u, 100u, 0u, 0u, 0u, 0u);
    if ((r.single_fire_word & SINGLE_FIRE_ZERO_PW_BIT) != 0u &&
        r.single_fire_zero_pw_toggled != 0u) ++passed;
    printf("  L0039 b4 toggles single-fire zero-PW b6  %s\n",
           ((r.single_fire_word & SINGLE_FIRE_ZERO_PW_BIT) != 0u &&
            r.single_fire_zero_pw_toggled != 0u) ? "PASS" : "FAIL");
    r = bua_injector_event_lf71c(0u, INJECT_ROUND_TOGGLE_BIT, 0u, 0u,
                                 0u, 300u, 0u, 0u, 0u, 0u, 0u, 0u);
    if ((r.single_fire_word & SINGLE_FIRE_ZERO_PW_BIT) == 0u &&
        (r.air_mode_word & INJECT_ROUND_TOGGLE_BIT) == 0u) ++passed;
    printf("  no b4 leaves SF b6 alone; every event toggles L003D b0  %s\n",
           ((r.single_fire_word & SINGLE_FIRE_ZERO_PW_BIT) == 0u &&
            (r.air_mode_word & INJECT_ROUND_TOGGLE_BIT) == 0u) ? "PASS" : "FAIL");
    r = bua_injector_event_lf71c(0u, 0u, 0u, DECEL_STATE_HOLD_BIT,
                                 0u, 345u, 50u, 25u, 700u, 9u, 4u, 0u);
    if ((r.ae_flags & INJECT_EVENT_STATE_BITS) == INJECT_EVENT_STATE_BITS &&
        r.last_base_pw == 345u && r.ae_pw_accumulator == 75u) ++passed;
    printf("  event sets L0038 b4/b2, saves PW345, adds pending50 ->75  %s\n",
           ((r.ae_flags & INJECT_EVENT_STATE_BITS) == INJECT_EVENT_STATE_BITS &&
            r.last_base_pw == 345u && r.ae_pw_accumulator == 75u) ? "PASS" : "FAIL");
    r = bua_injector_event_lf71c(0u, 0u, 0u, DECEL_STATE_HOLD_BIT,
                                 0u, 0u, 0x0030u, 0xFFF0u, 0u, 0u, 0u, 0u);
    if (r.ae_pw_accumulator == 0x0020u) ++passed;
    printf("  L011A + L011C preserves 16-bit wrap: FFF0+0030 ->0020  %s\n",
           (r.ae_pw_accumulator == 0x0020u) ? "PASS" : "FAIL");
    r = bua_injector_event_lf71c(0u, 0u, 0u, DECEL_STATE_HOLD_BIT,
                                 FUEL_ENGINE_RUNNING_BIT,
                                 0u, 0u, 0u, 0u, 0u, 0u, 255u);
    if (r.running_inject_counter == 255u) ++passed;
    printf("  running L018D counter saturates at 255  %s\n",
           (r.running_inject_counter == 255u) ? "PASS" : "FAIL");
    r = bua_injector_event_lf71c(0u, 0u, TPS_AE_LIMIT_BIT, 0u,
                                 FUEL_ENGINE_RUNNING_BIT,
                                 300u, 0u, 0u, 152u, 0u, 3u, 0u);
    if (r.fuel_limit_inject_count == 1u &&
        (r.ae_flags & TPS_AE_LIMIT_BIT) != 0u &&
        r.accumulated_ae == 152u && r.async_count == 3u &&
        r.limiter_cleared == 0u) ++passed;
    printf("  AE limiter first injector: count0->1, transient state retained  %s\n",
           (r.fuel_limit_inject_count == 1u &&
            (r.ae_flags & TPS_AE_LIMIT_BIT) != 0u &&
            r.accumulated_ae == 152u && r.async_count == 3u &&
            r.limiter_cleared == 0u) ? "PASS" : "FAIL");
    r = bua_injector_event_lf71c(0u, 0u, TPS_AE_LIMIT_BIT, 0u,
                                 FUEL_ENGINE_RUNNING_BIT,
                                 300u, 0u, 0u, 152u, 1u, 3u, 0u);
    if (r.fuel_limit_inject_count == 0u &&
        (r.ae_flags & TPS_AE_LIMIT_BIT) == 0u &&
        r.accumulated_ae == 0u && r.async_count == 0u &&
        r.limiter_cleared != 0u) ++passed;
    printf("  AE limiter second injector: clears b6/count/async accumulated fuel  %s\n",
           (r.fuel_limit_inject_count == 0u &&
            (r.ae_flags & TPS_AE_LIMIT_BIT) == 0u &&
            r.accumulated_ae == 0u && r.async_count == 0u &&
            r.limiter_cleared != 0u) ? "PASS" : "FAIL");
    r = bua_injector_event_lf71c(0u, 0u, 0u, DECEL_LIMIT_ACTIVE_BIT,
                                 FUEL_ENGINE_RUNNING_BIT,
                                 300u, 0u, 0u, 90u, 1u, 2u, 0u);
    if ((r.decel_flags & DECEL_LIMIT_ACTIVE_BIT) == 0u &&
        r.fuel_limit_inject_count == 0u && r.accumulated_ae == 0u &&
        r.limiter_cleared != 0u) ++passed;
    printf("  decel b5 uses same two-inject lifetime and clears on second  %s\n",
           ((r.decel_flags & DECEL_LIMIT_ACTIVE_BIT) == 0u &&
            r.fuel_limit_inject_count == 0u && r.accumulated_ae == 0u &&
            r.limiter_cleared != 0u) ? "PASS" : "FAIL");
    r = bua_injector_event_lf71c(0u, 0u, TPS_AE_LIMIT_BIT,
                                 DECEL_STATE_HOLD_BIT,
                                 FUEL_ENGINE_RUNNING_BIT,
                                 300u, 0u, 0u, 90u, 1u, 2u, 0u);
    if (r.hold_gate_taken != 0u && r.fuel_limit_inject_count == 1u &&
        (r.ae_flags & TPS_AE_LIMIT_BIT) != 0u && r.accumulated_ae == 90u) ++passed;
    printf("  L003E b7 hold gate bypasses clear and even bypasses count store  %s\n",
           (r.hold_gate_taken != 0u && r.fuel_limit_inject_count == 1u &&
            (r.ae_flags & TPS_AE_LIMIT_BIT) != 0u && r.accumulated_ae == 90u) ? "PASS" : "FAIL");
    r = bua_injector_event_lf71c(0u, 0u, 0u, 0u,
                                 FUEL_ENGINE_RUNNING_BIT,
                                 300u, 0u, 0u, 80u, 7u, 5u, 0u);
    if (r.limiter_cleared != 0u && r.fuel_limit_inject_count == 0u &&
        r.async_count == 0u && r.accumulated_ae == 0u) ++passed;
    printf("  no active limiter immediately clears stale transient bookkeeping  %s\n",
           (r.limiter_cleared != 0u && r.fuel_limit_inject_count == 0u &&
            r.async_count == 0u && r.accumulated_ae == 0u) ? "PASS" : "FAIL");
    printf("  step-29 injector-event regression result: %s (%u/10)\n",
           (passed == 10u) ? "PASS" : "FAIL", passed);
    mem = saved_mem;
    stats = saved_stats;
}
/* ---------------------------------------------------------------------- */
/* Step 28: TPS-triggered asynchronous acceleration enrichment.           */
/*                                                                        */
/* LFA1E qualifies a positive TPS transient and sets L0038 b7/b6/b5.      */
/* LE436..LE4EE then emits coolant-scaled async pulses, with the same      */
/* fuel-headroom limiter translated in Step 27.                           */
/* ---------------------------------------------------------------------- */
typedef struct {
    bua_u8 flags;
    bua_u8 stored_diff;
    bua_u8 async_count;
    bua_u8 triggered;
    bua_u8 canceled;
} BuaTpsAeTriggerResult;
typedef struct {
    bua_u16 raw_async_pw;
    bua_u16 limited_async_pw;
    bua_u16 hardware_async_pw;
    bua_u16 accumulated_ae;
    bua_u16 fuel_headroom;
    bua_u8 flags;
    bua_u8 async_count;
    bua_u8 pulse_index;
    bua_u8 pulse_factor;
    bua_u8 coolant_factor;
    bua_u8 pulse_generated;
} BuaAsyncAeResult;
static BuaTpsAeTriggerResult bua_tps_ae_lfa1e(bua_u8 current_tps,
                                               bua_u8 transient_tps,
                                               bua_u8 inhibit_word,
                                               bua_u8 flags_in,
                                               bua_u8 stored_diff_in,
                                               bua_u8 async_count_in)
{
    BuaTpsAeTriggerResult r;
    bua_u8 diff;
    bua_u8 neg_mag_minus_one;
    memset(&r, 0, sizeof(r));
    r.flags = flags_in;
    r.stored_diff = stored_diff_in;
    r.async_count = async_count_in;
    if ((inhibit_word & TPS_AE_INHIBIT_MASK) != 0u) {
        r.async_count = 0u;
        r.flags &= (bua_u8)~TPS_AE_ACTIVE_BIT;
        r.canceled = 1u;
        ++stats.tps_ae_cancels;
        return r;
    }
    diff = (bua_u8)(current_tps - transient_tps);
    if (current_tps < transient_tps) {
        /* Literal COMA after the borrowed SUBA: magnitude minus one. */
        neg_mag_minus_one = (bua_u8)~diff;
        if (neg_mag_minus_one > (bua_u8)CAL_TPS_AE_NEG_THRESHOLD) {
            r.async_count = 0u;
            r.flags &= (bua_u8)~TPS_AE_ACTIVE_BIT;
            r.canceled = 1u;
            ++stats.tps_ae_cancels;
        }
        return r;
    }
    if (diff < (bua_u8)CAL_TPS_AE_POS_THRESHOLD) {
        r.async_count = 0u;
        r.flags &= (bua_u8)~TPS_AE_ACTIVE_BIT;
        r.canceled = 1u;
        ++stats.tps_ae_cancels;
        return r;
    }
    if ((r.flags & TPS_AE_ACTIVE_BIT) == 0u) {
        r.flags &= (bua_u8)~FUEL_LIMIT_TRIGGER_STATE_BIT;
        r.stored_diff = diff;
        r.triggered = 1u;
        ++stats.tps_ae_triggers;
    }
    r.flags |= (bua_u8)(TPS_AE_ACTIVE_BIT | TPS_AE_LIMIT_BIT | TPS_AE_PHASE_BIT);
    return r;
}
static bua_u16 bua_async_headroom_lf9e5(bua_u16 current_pw,
                                        bua_u16 last_inject_pw,
                                        bua_u16 accumulated_ae,
                                        bua_u8 coolant,
                                        bua_u8 flags)
{
    bua_u8 packed;
    bua_u8 factor;
    bua_u16 scaled;
    bua_u16 allowed;
    bua_u32 supplied32;
    bua_u16 supplied;
    packed = cal_fuel_limit_coolant[(bua_u8)(coolant >> 5)];
    if ((flags & FUEL_LIMIT_TRIGGER_STATE_BIT) != 0u)
        factor = (bua_u8)((packed & 0x0Fu) << 4);
    else
        factor = (bua_u8)(packed & 0xF0u);
    scaled = bua_mul_u8_u16_lf266(factor, current_pw);
    if (scaled > 0x7FFFu)
        allowed = 0xFFFFu;
    else
        allowed = (bua_u16)(scaled << 1);
    supplied32 = (bua_u32)last_inject_pw + (bua_u32)accumulated_ae;
    supplied = (supplied32 > 65535ul) ? 65535u : (bua_u16)supplied32;
    if (allowed <= supplied)
        return 0u;
    return (bua_u16)(allowed - supplied);
}
static BuaAsyncAeResult bua_async_ae_le436(bua_u16 base_pw,
                                           bua_u16 last_inject_pw,
                                           bua_u16 accumulated_ae_in,
                                           bua_u8 coolant,
                                           bua_u8 battery_offset,
                                           bua_u8 single_fire_word,
                                           bua_u8 engine_mode_word,
                                           bua_u8 flags_in,
                                           bua_u8 async_count_in,
                                           bua_u8 async_pulses_to_do,
                                           bua_u8 coolant_factor)
{
    BuaAsyncAeResult r;
    bua_u16 work_pw;
    bua_u16 v;
    bua_u32 widened;
    bua_u16 headroom;
    bua_u8 old_count;
    bua_u8 index;
    memset(&r, 0, sizeof(r));
    r.flags = flags_in;
    r.async_count = async_count_in;
    /* All no-pulse paths converge at LE4AD and clear D before L00D9. */
    r.accumulated_ae = 0u;
    r.coolant_factor = coolant_factor;
    if ((engine_mode_word & FUEL_ENGINE_RUNNING_BIT) == 0u ||
        (r.flags & TPS_AE_ACTIVE_BIT) == 0u)
        return r;
    if (r.async_count >= async_pulses_to_do) {
        if ((r.flags & TPS_AE_LIMIT_BIT) == 0u) {
            r.async_count = 0u;
            r.flags &= (bua_u8)~TPS_AE_ACTIVE_BIT;
        }
        return r;
    }
    old_count = r.async_count;
    r.async_count = (bua_u8)(r.async_count + 1u);
    index = old_count;
    if (index > 7u)
        index = 7u;
    r.pulse_index = index;
    r.pulse_factor = cal_async_bpwpct[index];
    work_pw = base_pw;
    if ((single_fire_word & SINGLE_FIRE_BIT) != 0u)
        work_pw >>= 1;
    v = bua_mul_u8_u16_lf266(r.pulse_factor, work_pw);
    widened = (bua_u32)v << 2;
    if (widened > 65535ul)
        v = 65535u;
    else
        v = (bua_u16)widened;
    v = bua_factor_u8_u16_lf472(coolant_factor, v);
    if (v > (bua_u16)CAL_ASYNC_MAX_PW)
        v = (bua_u16)CAL_ASYNC_MAX_PW;
    r.raw_async_pw = v;
    if ((r.flags & TPS_AE_LIMIT_BIT) != 0u) {
        headroom = bua_async_headroom_lf9e5(base_pw, last_inject_pw,
                                            accumulated_ae_in, coolant, r.flags);
        r.fuel_headroom = headroom;
        if (headroom < v)
            v = headroom;
    }
    if (v < (bua_u16)CAL_ASYNC_MIN_PW)
        v = 0u;
    r.limited_async_pw = v;
    /* Ordinary LE4B1 path (no DFCO stall-saver addition): L00D9 is     */
    /* replaced by this pulse value, then injector offset is added twice */
    /* only to the hardware async counter.                               */
    r.accumulated_ae = v;
    if (v != 0u) {
        /* Two ABX instructions add the 8-bit offset with 16-bit wrap. */
        r.hardware_async_pw = (bua_u16)(v +
                                  (bua_u16)((bua_u16)battery_offset << 1));
        r.pulse_generated = 1u;
    }
    return r;
}

/* Step 102 thin HAL boundary for LE4D0..LE4E4.  The source writes the raw
 * asynchronous counter, sets high-byte CSR bit 2 with LF4C3, then clears it
 * with LF4CE.  No electrical injector polarity or MPU-internal behavior is
 * inferred here. */
static void bua_async_hw_trigger_step102(bua_u16 hardware_pw)
{
    bua_u16 csr;
    mpu16be_set(0x3FF2u, hardware_pw);
    csr = mpu16be_get((bua_u16)MPU_CPU_CONTROL_ADDR);
    mpu16be_set((bua_u16)MPU_CPU_CONTROL_ADDR,
                (bua_u16)(csr | 0x0400u));
    csr = mpu16be_get((bua_u16)MPU_CPU_CONTROL_ADDR);
    mpu16be_set((bua_u16)MPU_CPU_CONTROL_ADDR,
                (bua_u16)(csr & 0xFBFFu));
    ++stats.scheduler_async_hw_pulses;
    ++stats.async_csr_triggers;
}

/* LE436..LE4EE scheduler-facing RAM wrapper.  The already-proven arithmetic
 * helper covers the TPS-AE pulse.  LE4B1's independently translated stall
 * saver is applied afterward because the executable block can generate an
 * async pulse even when the TPS-AE portion produced zero. */
static void bua_scheduler_async_le436_step102(void)
{
    BuaAsyncAeResult a;
    bua_u16 old_accumulated;
    bua_u16 new_async;
    bua_u16 accumulated;
    bua_u32 sum;
    bua_u16 hardware_pw;

    ++stats.scheduler_async_calls;
    old_accumulated = ram16be_get(0x00D9u);
    a = bua_async_ae_le436(ram16be_get(0x00C2u),
                            ram16be_get(0x00D5u), old_accumulated,
                            COOLANT, RAM8(0x00CBu), INJECT_MODE_WORD,
                            ENGINE_MODE_WORD, RAM8(0x0038u),
                            RAM8(0x00DFu), RAM8(0x00D7u), RAM8(0x00D8u));
    RAM8(0x0038u) = a.flags;
    RAM8(0x00DFu) = a.async_count;
    new_async = a.limited_async_pw;
    accumulated = a.accumulated_ae;

    /* LE4B1: the first ADDD is intentionally 16-bit wrapping; only the
     * following addition to old L00D9 has an explicit carry saturation. */
    if ((AIR_MODE_WORD & 0x10u) != 0u && RAM8(0x008Au) != 0u) {
        --RAM8(0x008Au);
        new_async = (bua_u16)(new_async + (bua_u16)256u); /* LC605 */
        sum = (bua_u32)new_async + (bua_u32)old_accumulated;
        accumulated = (sum > 65535ul) ? 65535u : (bua_u16)sum;
    }
    ram16be_set(0x00DBu, new_async);
    ram16be_set(0x00D9u, accumulated);

    if (new_async != 0u) {
        hardware_pw = (bua_u16)(new_async +
                          (bua_u16)((bua_u16)RAM8(0x00CBu) << 1));
        bua_async_hw_trigger_step102(hardware_pw);
        ++stats.async_ae_pulses;
    }
    /* L5818 is the optional H.U. ROM hook and remains outside the PC core. */
    ram16be_set(0x011Au,
                (bua_u16)(ram16be_get(0x011Au) + new_async));
}
static void run_step28_async_ae_test(void)
{
    BuaMemory saved_mem;
    BuaStats saved_stats;
    BuaTpsAeTriggerResult t;
    BuaAsyncAeResult a;
    bua_u8 cool_factor;
    bua_u8 pulse_count;
    unsigned int passed;
    saved_mem = mem;
    saved_stats = stats;
    passed = 0u;
    printf("\nTPS / asynchronous accel-enrichment regression (LFA1E, LE436..LE4EE):\n");
    t = bua_tps_ae_lfa1e(120u, 100u, 0u, 0u, 0u, 0u);
    if (t.triggered != 0u && t.stored_diff == 20u &&
        (t.flags & 0xE0u) == 0xE0u &&
        (t.flags & FUEL_LIMIT_TRIGGER_STATE_BIT) == 0u) ++passed;
    printf("  +TPS diff20 starts AE, stores diff, sets L0038 E0  %s\n",
           (t.triggered != 0u && t.stored_diff == 20u &&
            (t.flags & 0xE0u) == 0xE0u &&
            (t.flags & FUEL_LIMIT_TRIGGER_STATE_BIT) == 0u) ? "PASS" : "FAIL");
    t = bua_tps_ae_lfa1e(109u, 100u, 0u, TPS_AE_ACTIVE_BIT, 33u, 4u);
    if (t.canceled != 0u && (t.flags & TPS_AE_ACTIVE_BIT) == 0u &&
        t.async_count == 0u) ++passed;
    printf("  +TPS diff9 cancels active async AE and clears pulse count  %s\n",
           (t.canceled != 0u && (t.flags & TPS_AE_ACTIVE_BIT) == 0u &&
            t.async_count == 0u) ? "PASS" : "FAIL");
    t = bua_tps_ae_lfa1e(88u, 100u, 0u, TPS_AE_ACTIVE_BIT, 33u, 3u);
    if (t.canceled != 0u && (t.flags & TPS_AE_ACTIVE_BIT) == 0u) ++passed;
    printf("  -TPS delta12: COMA result11 > LC30F10 cancels AE  %s\n",
           (t.canceled != 0u && (t.flags & TPS_AE_ACTIVE_BIT) == 0u) ? "PASS" : "FAIL");
    t = bua_tps_ae_lfa1e(89u, 100u, 0u, TPS_AE_ACTIVE_BIT, 33u, 3u);
    if (t.canceled == 0u && (t.flags & TPS_AE_ACTIVE_BIT) != 0u &&
        t.async_count == 3u) ++passed;
    printf("  -TPS delta11: literal COMA gives10, does not cancel  %s\n",
           (t.canceled == 0u && (t.flags & TPS_AE_ACTIVE_BIT) != 0u &&
            t.async_count == 3u) ? "PASS" : "FAIL");
    pulse_count = bua_lookup_spacing32(cal_async_pulse_count, 9u, 100u);
    cool_factor = bua_lookup_spacing32(cal_async_cool_factor, 9u, 100u);
    a = bua_async_ae_le436(300u, 0u, 0u, 100u, 30u, 0u,
                           FUEL_ENGINE_RUNNING_BIT, TPS_AE_ACTIVE_BIT,
                           0u, pulse_count, cool_factor);
    if (pulse_count == 12u && cool_factor == 128u && a.raw_async_pw == 152u &&
        a.limited_async_pw == 152u && a.hardware_async_pw == 212u &&
        a.accumulated_ae == 152u && a.async_count == 1u) ++passed;
    printf("  raw async: PW300, factor32, cool128 ->152; +2*30 offset=212  %s\n",
           (pulse_count == 12u && cool_factor == 128u && a.raw_async_pw == 152u &&
            a.limited_async_pw == 152u && a.hardware_async_pw == 212u &&
            a.accumulated_ae == 152u && a.async_count == 1u) ? "PASS" : "FAIL");
    a = bua_async_ae_le436(300u, 0u, 0u, 100u, 0u, SINGLE_FIRE_BIT,
                           FUEL_ENGINE_RUNNING_BIT, TPS_AE_ACTIVE_BIT,
                           0u, 12u, 128u);
    if (a.raw_async_pw == 76u && a.limited_async_pw == 0u &&
        a.pulse_generated == 0u) ++passed;
    printf("  single-fire halves base first: raw76 < min111 -> no async pulse  %s\n",
           (a.raw_async_pw == 76u && a.limited_async_pw == 0u &&
            a.pulse_generated == 0u) ? "PASS" : "FAIL");
    a = bua_async_ae_le436(1000u, 0u, 0u, 100u, 0u, 0u,
                           FUEL_ENGINE_RUNNING_BIT, TPS_AE_ACTIVE_BIT,
                           0u, 12u, 128u);
    if (a.raw_async_pw == 364u && a.limited_async_pw == 364u) ++passed;
    printf("  async pulse clamps at LC312=364 counts  %s\n",
           (a.raw_async_pw == 364u && a.limited_async_pw == 364u) ? "PASS" : "FAIL");
    a = bua_async_ae_le436(300u, 300u, 0u, 100u, 0u, 0u,
                           FUEL_ENGINE_RUNNING_BIT,
                           (bua_u8)(TPS_AE_ACTIVE_BIT | TPS_AE_LIMIT_BIT | TPS_AE_PHASE_BIT),
                           0u, 12u, 128u);
    if (a.fuel_headroom == 188u && a.raw_async_pw == 152u &&
        a.limited_async_pw == 152u) ++passed;
    printf("  trigger limiter headroom488-300=188 leaves async152 intact  %s\n",
           (a.fuel_headroom == 188u && a.raw_async_pw == 152u &&
            a.limited_async_pw == 152u) ? "PASS" : "FAIL");
    a = bua_async_ae_le436(300u, 300u, 152u, 100u, 0u, 0u,
                           FUEL_ENGINE_RUNNING_BIT,
                           (bua_u8)(TPS_AE_ACTIVE_BIT | TPS_AE_LIMIT_BIT | TPS_AE_PHASE_BIT),
                           1u, 12u, 128u);
    if (a.fuel_headroom == 36u && a.limited_async_pw == 0u) ++passed;
    printf("  accumulated fuel reduces headroom to36; below min -> pulse suppressed  %s\n",
           (a.fuel_headroom == 36u && a.limited_async_pw == 0u) ? "PASS" : "FAIL");
    printf("  step-28 async-AE regression result: %s (%u/9)\n",
           (passed == 9u) ? "PASS" : "FAIL", passed);
    mem = saved_mem;
    stats = saved_stats;
}
static void run_step27_fuel_limit_test(void)
{
    BuaMemory saved_mem;
    BuaStats saved_stats;
    BuaFuelLimitResult r;
    unsigned int passed;
    saved_mem = mem;
    saved_stats = stats;
    passed = 0u;
    printf("\nSpecial fuel-limit regression (LF95E/LF9E5 and $F9FC entry):\n");
    r = bua_fuel_limit_lf95e(300u, 400u, 100u, 100u,
                             FUEL_LIMIT_AE_MASK, 0u);
    if (r.applied != 0u && r.packed_cal == 217u && r.factor == 208u &&
        r.allowed_fuel == 488u && r.supplied_fuel == 500u &&
        r.excess_fuel == 12u && r.output_pw == 288u) ++passed;
    printf("  cool LC331=D9 trigger nibble D0: limit488, supplied500 -> PW288  %s\n",
           (r.applied != 0u && r.packed_cal == 217u && r.factor == 208u &&
            r.allowed_fuel == 488u && r.supplied_fuel == 500u &&
            r.excess_fuel == 12u && r.output_pw == 288u) ? "PASS" : "FAIL");
    r = bua_fuel_limit_lf95e(300u, 400u, 100u, 100u,
                             (bua_u8)(FUEL_LIMIT_AE_MASK | FUEL_LIMIT_TRIGGER_STATE_BIT), 0u);
    if (r.factor == 144u && r.allowed_fuel == 338u &&
        r.excess_fuel == 162u && r.output_pw == 138u) ++passed;
    printf("  cool LC331=D9 after-next nibble 90: limit338 -> PW138  %s\n",
           (r.factor == 144u && r.allowed_fuel == 338u &&
            r.excess_fuel == 162u && r.output_pw == 138u) ? "PASS" : "FAIL");
    r = bua_fuel_limit_lf95e(300u, 350u, 0u, 200u,
                             FUEL_LIMIT_AE_MASK, 0u);
    if (r.packed_cal == 169u && r.factor == 160u &&
        r.allowed_fuel == 376u && r.supplied_fuel == 350u && r.output_pw == 300u) ++passed;
    printf("  warm LC331=A9 trigger nibble A0: supplied350 < limit376, unchanged  %s\n",
           (r.packed_cal == 169u && r.factor == 160u &&
            r.allowed_fuel == 376u && r.supplied_fuel == 350u && r.output_pw == 300u) ? "PASS" : "FAIL");
    r = bua_fuel_limit_lf95e(300u, 400u, 100u, 200u,
                             (bua_u8)(FUEL_LIMIT_AE_MASK | FUEL_LIMIT_TRIGGER_STATE_BIT), 0u);
    if (r.packed_cal == 169u && r.factor == 144u &&
        r.allowed_fuel == 338u && r.output_pw == 138u) ++passed;
    printf("  warm LC331=A9 after-next still uses nibble 90 -> PW138  %s\n",
           (r.packed_cal == 169u && r.factor == 144u &&
            r.allowed_fuel == 338u && r.output_pw == 138u) ? "PASS" : "FAIL");
    r = bua_fuel_limit_lf95e(300u, 400u, 100u, 100u,
                             0u, FUEL_LIMIT_DECEL_MASK);
    if (r.used_fixed_decel_pair != 0u && r.factor == 128u &&
        r.allowed_fuel == 300u && r.excess_fuel == 200u && r.output_pw == 0u) ++passed;
    printf("  $F9FC fixed 128 trigger: limit300, excess200 leaves100 < min111 -> zero  %s\n",
           (r.used_fixed_decel_pair != 0u && r.factor == 128u &&
            r.allowed_fuel == 300u && r.excess_fuel == 200u && r.output_pw == 0u) ? "PASS" : "FAIL");
    r = bua_fuel_limit_lf95e(300u, 350u, 0u, 100u,
                             FUEL_LIMIT_TRIGGER_STATE_BIT, FUEL_LIMIT_DECEL_MASK);
    if (r.used_fixed_decel_pair != 0u && r.factor == 128u &&
        r.allowed_fuel == 300u && r.excess_fuel == 50u && r.output_pw == 250u) ++passed;
    printf("  $F9FC fixed 128 after-next: supplied350, excess50 -> PW250  %s\n",
           (r.used_fixed_decel_pair != 0u && r.factor == 128u &&
            r.allowed_fuel == 300u && r.excess_fuel == 50u && r.output_pw == 250u) ? "PASS" : "FAIL");
    r = bua_fuel_limit_lf95e(300u, 65500u, 1000u, 100u,
                             FUEL_LIMIT_AE_MASK, 0u);
    if (r.supplied_fuel == 65535u && r.output_pw == 0u) ++passed;
    printf("  previous-PW + accumulated-AE overflow saturates supplied fuel -> zero current  %s\n",
           (r.supplied_fuel == 65535u && r.output_pw == 0u) ? "PASS" : "FAIL");
    r = bua_fuel_limit_lf95e(300u, 400u, 100u, 100u, 0u, 0u);
    if (r.applied == 0u && r.output_pw == 300u) ++passed;
    printf("  no limiter state bits -> ordinary PW300 unchanged  %s\n",
           (r.applied == 0u && r.output_pw == 300u) ? "PASS" : "FAIL");
    printf("  step-27 special fuel-limit regression result: %s (%u/8)\n",
           (passed == 8u) ? "PASS" : "FAIL", passed);
    mem = saved_mem;
    stats = saved_stats;
}
static void run_step26_fuel_output_test(void)
{
    BuaMemory saved_mem;
    BuaStats saved_stats;
    BuaFuelOutputResult r;
    bua_u8 factor;
    bua_u8 flags;
    bua_u16 pw;
    unsigned int passed;
    saved_mem = mem;
    saved_stats = stats;
    passed = 0u;
    printf("\nAccel-enrichment / fuel-output regression (LF901..LF9E4 ordinary path):\n");
    factor = 128u;
    flags = 0u;
    pw = bua_ae_apply_lf901(300u, &factor, 20u, &flags);
    if (pw == 450u && factor == 128u && (flags & AE_FIRST_TIME_BIT) != 0u) ++passed;
    printf("  AE apply: factor=128 adds 50%% to PW300 -> %u  %s\n",
           (unsigned int)pw,
           (pw == 450u && factor == 128u && (flags & AE_FIRST_TIME_BIT) != 0u) ? "PASS" : "FAIL");
    factor = 50u;
    flags = (bua_u8)(AE_FIRST_TIME_BIT | AE_INJECT_SINCE_BIT | AE_ACTIVE_BIT);
    pw = bua_ae_apply_lf901(300u, &factor, 20u, &flags);
    if (factor == 30u && pw == 335u && (flags & AE_INJECT_SINCE_BIT) == 0u) ++passed;
    printf("  AE decay after inject: factor 50-20=30 -> PW=%u  %s\n",
           (unsigned int)pw,
           (factor == 30u && pw == 335u && (flags & AE_INJECT_SINCE_BIT) == 0u) ? "PASS" : "FAIL");
    factor = 10u;
    flags = (bua_u8)(AE_FIRST_TIME_BIT | AE_INJECT_SINCE_BIT | AE_ACTIVE_BIT);
    pw = bua_ae_apply_lf901(300u, &factor, 20u, &flags);
    if (factor == 0u && pw == 300u && (flags & AE_ACTIVE_BIT) == 0u) ++passed;
    printf("  AE decay underflow: factor 10-20 -> 0, active clears  %s\n",
           (factor == 0u && pw == 300u && (flags & AE_ACTIVE_BIT) == 0u) ? "PASS" : "FAIL");
    r = bua_fuel_output_lf92a(300u, FUEL_VATS_OK_BIT, 0u, 0u,
                              FUEL_ENGINE_RUNNING_BIT, 0u, 60u, 410u, 30u);
    if (r.pending_pw == 300u && r.hardware_pw == 360u && r.fuel_cut == 0u) ++passed;
    printf("  ordinary output: PW300 + 2*battOffset30 -> hardware=%u  %s\n",
           (unsigned int)r.hardware_pw,
           (r.pending_pw == 300u && r.hardware_pw == 360u && r.fuel_cut == 0u) ? "PASS" : "FAIL");
    r = bua_fuel_output_lf92a(96u, FUEL_VATS_OK_BIT, 0u, 0u,
                              FUEL_ENGINE_RUNNING_BIT, 0u, 60u, 410u, 0u);
    if (r.small_pw_offset == 8u && r.hardware_pw == 111u && r.low_pw_forced == 1u) ++passed;
    printf("  small PW: 96 + C3BC offset8 =104 <=111 -> default111  %s\n",
           (r.small_pw_offset == 8u && r.hardware_pw == 111u && r.low_pw_forced == 1u) ? "PASS" : "FAIL");
    r = bua_fuel_output_lf92a(300u, 0u, 0u, 0u,
                              FUEL_ENGINE_RUNNING_BIT, 0u, 60u, 410u, 30u);
    if (r.hardware_pw == 0u && r.fuel_cut == 1u) ++passed;
    printf("  VATS gate clear -> zero fuel  %s\n",
           (r.hardware_pw == 0u && r.fuel_cut == 1u) ? "PASS" : "FAIL");
    r = bua_fuel_output_lf92a(300u, FUEL_VATS_OK_BIT, 0u, 0u,
                              FUEL_ENGINE_RUNNING_BIT, 0u, 60u, 97u, 30u);
    if (r.hardware_pw == 0u && (r.af_mode_word & FUEL_HI_CUTOFF_BIT) != 0u) ++passed;
    printf("  high-RPM cut: refPeriod97 < LC3A6=98 -> zero fuel  %s\n",
           (r.hardware_pw == 0u && (r.af_mode_word & FUEL_HI_CUTOFF_BIT) != 0u) ? "PASS" : "FAIL");
    r = bua_fuel_output_lf92a(300u, FUEL_VATS_OK_BIT, 0u, FUEL_HI_CUTOFF_BIT,
                              FUEL_ENGINE_RUNNING_BIT, 0u, 60u, 103u, 30u);
    if (r.hardware_pw == 360u && (r.af_mode_word & FUEL_HI_CUTOFF_BIT) == 0u) ++passed;
    printf("  high-RPM hysteresis: prior cut, refPeriod103 re-enables fuel  %s\n",
           (r.hardware_pw == 360u && (r.af_mode_word & FUEL_HI_CUTOFF_BIT) == 0u) ? "PASS" : "FAIL");
    printf("  step-26 fuel-output regression result: %s (%u/8)\n",
           (passed == 8u) ? "PASS" : "FAIL", passed);
    mem = saved_mem;
    stats = saved_stats;
}
static void run_step25_afr_test(void)
{
    BuaMemory saved_mem;
    BuaStats saved_stats;
    BuaAfrResult r;
    unsigned int passed;
    saved_mem = mem;
    saved_stats = stats;
    passed = 0u;
    printf("\nAFR / open-loop / power-enrichment regression (LD9D7..LDA51):\n");
    r = bua_afr_lda2d(0u, AF_CLOSED_LOOP_BIT, 25u, 128u);
    if (r.factor == 256u && r.afr_code == 445u) ++passed;
    printf("  closed loop neutral: factor=256 -> code=%u AFR=%0.2f  %s\n",
           (unsigned int)r.afr_code, bua_afr_code_to_ratio(r.afr_code),
           (r.factor == 256u && r.afr_code == 445u) ? "PASS" : "FAIL");
    r = bua_afr_lda2d(0u, 0u, 25u, 128u);
    if (r.load_change == 13u && r.factor == 259u && r.afr_code == 450u) ++passed;
    printf("  warm open loop @load128: 221+25+13=259 -> code=%u AFR=%0.2f  %s\n",
           (unsigned int)r.afr_code, bua_afr_code_to_ratio(r.afr_code),
           (r.load_change == 13u && r.factor == 259u && r.afr_code == 450u) ? "PASS" : "FAIL");
    r = bua_afr_lda2d(32u, AF_CLOSED_LOOP_BIT, 25u, 128u);
    if (r.factor == 288u && r.afr_code == 500u) ++passed;
    printf("  startup enrichment in closed loop: factor=288 -> code=%u  %s\n",
           (unsigned int)r.afr_code,
           (r.factor == 288u && r.afr_code == 500u) ? "PASS" : "FAIL");
    r = bua_afr_power_enrich_ld9d7(96u, 50u, 180u, 160u, 0u);
    if (r.pe_active == 1u && r.factor == 314u && r.afr_code == 545u) ++passed;
    printf("  PE threshold exact: TPS=180 load=50 -> factor=%u code=%u AFR=%0.2f  %s\n",
           (unsigned int)r.factor, (unsigned int)r.afr_code,
           bua_afr_code_to_ratio(r.afr_code),
           (r.pe_active == 1u && r.factor == 314u && r.afr_code == 545u) ? "PASS" : "FAIL");
    r = bua_afr_power_enrich_ld9d7(96u, 80u, 179u, 160u, 0u);
    if (r.pe_active == 0u) ++passed;
    printf("  PE TPS qualifier: TPS=179 < 180 -> inactive  %s\n",
           (r.pe_active == 0u) ? "PASS" : "FAIL");
    r = bua_afr_power_enrich_ld9d7(96u, 49u, 200u, 160u, 0u);
    if (r.pe_active == 0u) ++passed;
    printf("  PE load qualifier: load=49 < 50 -> inactive  %s\n",
           (r.pe_active == 0u) ? "PASS" : "FAIL");
    r = bua_afr_power_enrich_ld9d7(96u, 40u, 164u, 160u, 1u);
    if (r.pe_active == 1u) ++passed;
    printf("  PE hysteresis: prior-active accepts TPS=164 load=40  %s\n",
           (r.pe_active == 1u) ? "PASS" : "FAIL");
    r = bua_afr_power_enrich_ld9d7(96u, 80u, 200u, 0u, 0u);
    if (r.pe_active == 1u && r.factor == 396u && r.afr_code == 688u) ++passed;
    printf("  cold PE coolant term: C613=140 -> factor=396 code=%u AFR=%0.2f  %s\n",
           (unsigned int)r.afr_code, bua_afr_code_to_ratio(r.afr_code),
           (r.pe_active == 1u && r.factor == 396u && r.afr_code == 688u) ? "PASS" : "FAIL");
    printf("  step-25 AFR regression result: %s (%u/8)\n",
           (passed == 8u) ? "PASS" : "FAIL", passed);
    mem = saved_mem;
    stats = saved_stats;
}
static void run_step24_o2_control_test(void)
{
    BuaMemory saved_mem;
    BuaStats saved_stats;
    BuaO2ControlResult r;
    bua_u8 ready;
    bua_u8 rich;
    unsigned int passed;
    saved_mem = mem;
    saved_stats = stats;
    passed = 0u;
    printf("\nClosed-loop O2 / integrator / proportional regression (LDB35..LDD27):\n");
    rich = bua_o2_fast_rich_ldb35(0x2000u, 170u, 160u, 0u, 1u, &ready);
    if (rich == 1u && ready == 1u) ++passed;
    printf("  fast O2 above upper window -> rich=1 ready=1  %s\n",
           (rich == 1u && ready == 1u) ? "PASS" : "FAIL");
    rich = bua_o2_fast_rich_ldb35(0x2000u, 100u, 110u, 0u, 1u, &ready);
    if (rich == 0u && ready == 0u) ++passed;
    printf("  fast O2 below lower window -> lean=1, ready remains 0  %s\n",
           (rich == 0u && ready == 0u) ? "PASS" : "FAIL");
    rich = bua_o2_fast_rich_ldb35(0x2000u, 140u, 130u, 0u, 1u, &ready);
    if (rich == 1u) ++passed;
    printf("  fast O2 inside hysteresis, rising -> rich  %s\n",
           (rich == 1u) ? "PASS" : "FAIL");
    r = bua_closed_loop_o2_step24(0x2000u, 170u, 160u, 170u, 0u, 1u,
                                   20u, 10u, 160u, 160u, 0u, 128u, 0u);
    if (r.fast_rich == 1u && r.slow_rich == 1u && r.limited_error > 0u &&
        r.closed_corr < 128u) ++passed;
    printf("  rich agreement -> slow rich, nonzero error, C6 below 128: err=%u C6=%u  %s\n",
           (unsigned int)r.limited_error, (unsigned int)r.closed_corr,
           (r.fast_rich == 1u && r.slow_rich == 1u && r.limited_error > 0u &&
            r.closed_corr < 128u) ? "PASS" : "FAIL");
    r = bua_closed_loop_o2_step24(0x2000u, 90u, 100u, 90u, 0u, 1u,
                                   20u, 10u, 160u, 160u, 0u, 128u, 0u);
    if (r.fast_rich == 0u && r.slow_rich == 0u && r.limited_error > 0u &&
        r.closed_corr > 128u) ++passed;
    printf("  lean agreement -> slow lean, nonzero error, C6 above 128: err=%u C6=%u  %s\n",
           (unsigned int)r.limited_error, (unsigned int)r.closed_corr,
           (r.fast_rich == 0u && r.slow_rich == 0u && r.limited_error > 0u &&
            r.closed_corr > 128u) ? "PASS" : "FAIL");
    r = bua_closed_loop_o2_step24(0x2000u, 170u, 160u, 170u, 0u, 1u,
                                   20u, 10u, 160u, 160u, 0u, 128u, 255u);
    if (r.integrator == 127u && r.int_timer == 0u) ++passed;
    printf("  expired integrator delay while rich: INT 128->%u timer=%u  %s\n",
           (unsigned int)r.integrator, (unsigned int)r.int_timer,
           (r.integrator == 127u && r.int_timer == 0u) ? "PASS" : "FAIL");
    r = bua_closed_loop_o2_step24(0x2000u, 90u, 100u, 90u, 0u, 1u,
                                   20u, 10u, 160u, 160u, 0u, 128u, 255u);
    if (r.integrator == 129u && r.int_timer == 0u) ++passed;
    printf("  expired integrator delay while lean: INT 128->%u timer=%u  %s\n",
           (unsigned int)r.integrator, (unsigned int)r.int_timer,
           (r.integrator == 129u && r.int_timer == 0u) ? "PASS" : "FAIL");
    r = bua_closed_loop_o2_step24(0x2000u, 140u, 130u, 125u, 0u, 1u,
                                   20u, 10u, 160u, 160u, 250u, 128u, 0u);
    if (r.prop_step == 0u) ++passed;
    printf("  long mismatched/low-error duration suppresses proportional step -> %u  %s\n",
           (unsigned int)r.prop_step, (r.prop_step == 0u) ? "PASS" : "FAIL");
    printf("  step-24 O2-control regression result: %s (%u/8)\n",
           (passed == 8u) ? "PASS" : "FAIL", passed);
    mem = saved_mem;
    stats = saved_stats;
}
static void run_step23_blm_closed_loop_test(void)
{
    BuaMemory saved_mem;
    BuaStats saved_stats;
    bua_u16 v;
    unsigned int passed;
    saved_mem = mem;
    saved_stats = stats;
    passed = 0u;
    printf("\nBLM + closed-loop PW correction regression (LDD7A..LDDB9):\n");
    v = bua_factor_u8_u16_lf472(128u, 287u);
    if (v == 287u) ++passed;
    printf("  LF472 unity: factor=128, PW=287 -> %u  %s\n",
           (unsigned int)v, (v == 287u) ? "PASS" : "FAIL");
    v = bua_factor_u8_u16_lf472(160u, 287u);
    if (v == 358u) ++passed;
    printf("  BLM rich limit example: factor=160, PW=287 -> %u  %s\n",
           (unsigned int)v, (v == 358u) ? "PASS" : "FAIL");
    v = bua_factor_u8_u16_lf472(108u, 287u);
    if (v == 242u) ++passed;
    printf("  BLM lean limit example: factor=108, PW=287 -> %u  %s\n",
           (unsigned int)v, (v == 242u) ? "PASS" : "FAIL");
    v = bua_apply_blm_closed_loop_ldd7a(287u, 128u, 140u, 0u, 0u, 0u);
    if (v == 299u) ++passed;
    printf("  closed-loop integrator richward: neutral BLM, C6=140 -> %u  %s\n",
           (unsigned int)v, (v == 299u) ? "PASS" : "FAIL");
    v = bua_apply_blm_closed_loop_ldd7a(287u, 128u, 116u, 0u, 0u, 0u);
    if (v == 275u) ++passed;
    printf("  closed-loop integrator leanward: neutral BLM, C6=116 -> %u  %s\n",
           (unsigned int)v, (v == 275u) ? "PASS" : "FAIL");
    v = bua_apply_blm_closed_loop_ldd7a(287u, 128u, 128u, 11u, 0u, 0u);
    if (v == 282u) ++passed;
    printf("  odd proportional step, L003D b0=0: half=5, subtract -> %u  %s\n",
           (unsigned int)v, (v == 282u) ? "PASS" : "FAIL");
    v = bua_apply_blm_closed_loop_ldd7a(287u, 128u, 128u, 11u,
                                        AF_RICH_LEAN_BIT,
                                        AIR_INJECT_TOGGLE_BIT);
    if (v == 293u) ++passed;
    printf("  odd proportional step, L003D b0=1 + L0044 b6=1: half=6, add -> %u  %s\n",
           (unsigned int)v, (v == 293u) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    ENGINE_MODE_WORD = ENGINE_RUNNING_BIT;
    ram16be_set(REF_PERIOD_ADDR, 410u);
    ram16be_set(TOTAL_AFR_ADDR, 445u);
    ram16be_set(AIRFLOW_WORD_ADDR, 0x3C00u);
    RPM_DIV25 = 96u;
    FILTERED_LOAD = 128u;
    BLM = 128u;
    CLOSED_LOOP_PW_CORR = 128u;
    AF_MODE_WORD2 = 0u;
    AIR_MODE_WORD = 0u;
    v = bua_fuel_base_and_corrections_step23(0u);
    if (v == 287u && ram16be_get(0x00C2u) == 287u) ++passed;
    printf("  live chain: base=287, neutral BLM/C-L -> L00C2=%u  %s\n",
           (unsigned int)v,
           (v == 287u && ram16be_get(0x00C2u) == 287u) ? "PASS" : "FAIL");
    printf("  step-23 BLM/closed-loop regression result: %s (%u/8)\n",
           (passed == 8u) ? "PASS" : "FAIL", passed);
    mem = saved_mem;
    stats = saved_stats;
}
static void run_step22_base_fuel_test(void)
{
    BuaMemory saved_mem;
    BuaStats saved_stats;
    bua_u16 v;
    bua_u16 mid;
    bua_u16 upper;
    unsigned int passed;
    saved_mem = mem;
    saved_stats = stats;
    passed = 0u;
    printf("\nBase-fuel regression (LDD2D..LDD60, before BLM/closed-loop correction):\n");
    /* LF2F0 known arithmetic: 445*735 = 0x0004FDA3, so middle bytes are
     * 0x04FD and upper word 0x0004. */
    mid = bua_mul_u16_u16_lf2f0(445u, 735u, &upper);
    if (mid == 1277u && upper == 4u) ++passed;
    printf("  LF2F0: 445*735 -> middle=%u upper=%u  %s\n",
           (unsigned int)mid, (unsigned int)upper,
           (mid == 1277u && upper == 4u) ? "PASS" : "FAIL");
    /* C42D is flat with RPM in this calibration.  At load 128 its stored
     * table value is 61; with stoich AFR 445 the alternate table path
     * produces 304 timer-domain PW counts. */
    v = bua_base_pw_table_path(96u, 128u, 445u);
    if (bua_lookup_base_fuel_c42d(96u, 128u) == 61u && v == 304u) ++passed;
    printf("  disabled C42D path: rpmArg=96 load=128 raw=61 -> PW=%u  %s\n",
           (unsigned int)v, (v == 304u) ? "PASS" : "FAIL");
    v = bua_base_pw_table_path(152u, 136u, 445u);
    if (bua_lookup_base_fuel_c42d(152u, 136u) == 65u && v == 324u) ++passed;
    printf("  C42D interpolation: load=136 -> raw=65, PW=%u  %s\n",
           (unsigned int)v, (v == 324u) ? "PASS" : "FAIL");
    /* Use explicit raw airflow 0x3C00 so this is an instruction-level test,
     * not a claim about the final physical g/s scaling of L00EA. */
    v = bua_base_pw_calculated_path(0x3C00u, 410u, 445u, 0u);
    if (v == 287u) ++passed;
    printf("  calculated double-fire: air=3C00 ref=410 AFR=445 -> PW=%u  %s\n",
           (unsigned int)v, (v == 287u) ? "PASS" : "FAIL");
    v = bua_base_pw_calculated_path(0x3C00u, 410u, 445u, 1u);
    if (v == 575u) ++passed;
    printf("  calculated single-fire uses LC3D1=882 -> PW=%u  %s\n",
           (unsigned int)v, (v == 575u) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    ENGINE_MODE_WORD = ENGINE_RUNNING_BIT;
    ram16be_set(REF_PERIOD_ADDR, 410u);
    ram16be_set(TOTAL_AFR_ADDR, 445u);
    ram16be_set(AIRFLOW_WORD_ADDR, 0x3C00u);
    RPM_DIV25 = 96u;
    FILTERED_LOAD = 128u;
    v = bua_base_pw_ldd2d();
    if ((CAL_AF_MODE3 & CAL_BASE_PW_CALC_BIT) != 0u && v == 287u) ++passed;
    printf("  actual LC016=%02X selects calculated path -> PW=%u  %s\n",
           (unsigned int)CAL_AF_MODE3, (unsigned int)v,
           (((CAL_AF_MODE3 & CAL_BASE_PW_CALC_BIT) != 0u) && v == 287u) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    ram16be_set(REF_PERIOD_ADDR, 410u);
    ram16be_set(TOTAL_AFR_ADDR, 445u);
    ram16be_set(AIRFLOW_WORD_ADDR, 0x3C00u);
    v = bua_base_pw_ldd2d();
    if (v == 0u) ++passed;
    printf("  not-running LC357=0 forces calculated BPW to zero -> %u  %s\n",
           (unsigned int)v, (v == 0u) ? "PASS" : "FAIL");
    printf("  step-22 base-fuel regression result: %s (%u/7)\n",
           (passed == 7u) ? "PASS" : "FAIL", passed);
    mem = saved_mem;
    stats = saved_stats;
}
static void run_step21_ignition_finish_test(void)
{
    BuaMemory saved_mem;
    BuaStats saved_stats;
    BuaEstOutputResult e;
    BuaStallSaverResult s;
    unsigned int passed;
    saved_mem = mem;
    saved_stats = stats;
    passed = 0u;
    printf("\nIgnition finish regression (LD25x..LD306 + stall saver):\n");
    memset(&mem, 0, sizeof(mem));
    IGN_MODE_WORD = IGN_FIRST_REF_BIT;
    mpu16be_set((bua_u16)MPU_LAST_REF_ADDR, 0x2345u);
    e = bua_est_output_finish_ld25x();
    if (e.first_ref_consumed && e.est_enabled &&
        mpu16be_get((bua_u16)MPU_NEXT_DWELL_ADDR) == 0x2345u &&
        SPARK_BLEND_MULT == 0u && (SPARK_MODE_WORD & SPARK_BLEND_BIT)) ++passed;
    printf("  first valid ref seeds next-dwell and enables EST/blend start  %s\n",
           (e.first_ref_consumed && e.est_enabled &&
            mpu16be_get((bua_u16)MPU_NEXT_DWELL_ADDR) == 0x2345u) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    IGN_MODE_WORD = IGN_FIRST_REF_BIT;
    SPARK_FEEDBACK_COUNT = 3u;
    e = bua_est_output_finish_ld25x();
    if (e.error42_set && (IGN_MODE_WORD & IGN_ERR42_LATCH_BIT) && SPARK_FEEDBACK_COUNT == 0u) ++passed;
    printf("  Error-42 feedback count 3 > LC227 2 latches fault  %s\n",
           (e.error42_set && (IGN_MODE_WORD & IGN_ERR42_LATCH_BIT) && SPARK_FEEDBACK_COUNT == 0u) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    MINOR_MODE_WORD2 = MINOR_MODE4_BIT;
    ALDL_MODE4_WORD4 = MODE4_EST_BYPASS_BIT;
    SPARK_INPUT_MODE_WORD = EST_ENABLE_BIT;
    SPARK_FEEDBACK_COUNT = 5u;
    mpu16be_set((bua_u16)MPU_SPARK_PERIOD_ADDR, 0x3456u);
    mpu16be_set((bua_u16)MPU_CPU_CONTROL_ADDR, CPU_EST_BYPASS_BIT);
    e = bua_est_output_finish_ld25x();
    if (e.mode4_bypass && !e.est_enabled && SPARK_FEEDBACK_COUNT == 0u &&
        ram16be_get(OLD_SPARK_PERIOD_ADDR) == 0x3456u &&
        (mpu16be_get((bua_u16)MPU_CPU_CONTROL_ADDR) & CPU_EST_BYPASS_BIT) == 0u) ++passed;
    printf("  Mode-4 EST bypass disables EST and snapshots spark period  %s\n",
           (e.mode4_bypass && !e.est_enabled && ram16be_get(OLD_SPARK_PERIOD_ADDR) == 0x3456u) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    SPARK_INPUT_MODE_WORD = EST_ENABLE_BIT;
    mpu16be_set((bua_u16)MPU_SPARK_PERIOD_ADDR, 0x1111u);
    s = bua_stall_saver_update(31u, 0u);
    if (s.entered && s.active && (SPARK_MODE_WORD & 0x03u) == 0x03u &&
        !(SPARK_INPUT_MODE_WORD & EST_ENABLE_BIT) && ram16be_get(OLD_SPARK_PERIOD_ADDR) == 0x1111u) ++passed;
    printf("  stall saver enters below 400 RPM and drops EST  %s\n",
           (s.entered && s.active && (SPARK_MODE_WORD & 0x03u) == 0x03u) ? "PASS" : "FAIL");
    s = bua_stall_saver_update(44u, 0u);
    if (s.active && !s.exited) ++passed;
    printf("  stall saver remains active at exactly 550 RPM  %s\n",
           (s.active && !s.exited) ? "PASS" : "FAIL");
    mpu16be_set((bua_u16)MPU_LAST_REF_ADDR, 0x2222u);
    mpu16be_set((bua_u16)MPU_SPARK_OLD_DWELL_ADDR, 100u);
    mpu16be_set((bua_u16)MPU_SPARK_DWELL_DELTA_ADDR, 101u);
    mpu16be_set((bua_u16)MPU_SPARK_FALL_DELTA_ADDR, 102u);
    mpu16be_set((bua_u16)MPU_SPARK_FIRE_OFFSET_ADDR, 103u);
    s = bua_stall_saver_update(45u, 0u);
    if (s.exited && !s.active && SPARK_FEEDBACK_COUNT == 1u &&
        mpu16be_get((bua_u16)MPU_SPARK_OLD_DWELL_ADDR) == 0u &&
        mpu16be_get((bua_u16)MPU_SPARK_FIRE_OFFSET_ADDR) == 0u &&
        mpu16be_get((bua_u16)MPU_NEXT_DWELL_ADDR) == 0x2222u) ++passed;
    printf("  stall saver exits above 550 RPM and zeros EST timing state  %s\n",
           (s.exited && !s.active && SPARK_FEEDBACK_COUNT == 1u &&
            mpu16be_get((bua_u16)MPU_SPARK_FIRE_OFFSET_ADDR) == 0u) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    SPARK_INPUT_MODE_WORD = EST_ENABLE_BIT;
    ram16be_set(ENGINE_RUN_TIME_ADDR, 123u);
    bua_low_rpm_spark_reset_ld306();
    if (ram16be_get(0x0468u) == 0xFFFFu && ram16be_get(0x0056u) == 0u &&
        ram16be_get(0x0058u) == 0u && ram16be_get(ENGINE_RUN_TIME_ADDR) == 0u &&
        ram16be_get(TRANSIENT_LOAD_ADDR) == 0x8000u && !(SPARK_INPUT_MODE_WORD & EST_ENABLE_BIT)) ++passed;
    printf("  LD306 low-RPM reset clears RPM/EST and initializes transient load  %s\n",
           (ram16be_get(0x0468u) == 0xFFFFu && ram16be_get(0x0056u) == 0u &&
            ram16be_get(TRANSIENT_LOAD_ADDR) == 0x8000u && !(SPARK_INPUT_MODE_WORD & EST_ENABLE_BIT)) ? "PASS" : "FAIL");
    printf("  step-21 ignition-finish regression result: %s (%u/7)\n",
           (passed == 7u) ? "PASS" : "FAIL", passed);
    mem = saved_mem;
    stats = saved_stats;
}
static void run_step20_spark_timing_test(void)
{
    BuaMemory saved_mem;
    BuaStats saved_stats;
    BuaSparkTimingResult r;
    bua_u16 limited;
    bua_u8 slew;
    unsigned int passed;
    saved_mem = mem;
    saved_stats = stats;
    passed = 0u;
    printf("\nSpark timing/output regression (LD1F6..LD25x):\n");
    /* 410 counts/reference is the established approximately-2400-RPM case.
     * 99 raw angle units -> round(410*99/256)=159 timer counts. */
    memset(&mem, 0, sizeof(mem));
    ram16be_set(REF_PERIOD_ADDR, 410u);
    ram16be_set(DWELL_COUNTS_ADDR, 284u);
    mpu16be_set((bua_u16)MPU_SPARK_FIRE_OFFSET_ADDR, (bua_u16)(0u - 172u));
    mpu16be_set((bua_u16)MPU_SPARK_OLD_DWELL_ADDR, 284u);
    ENGINE_MODE_WORD &= (bua_u8)~SPARK_ADV_RETARD_FLAG;
    r = bua_spark_timing_output_ld1f6(99u);
    if (r.angle_time_counts == 159u && r.requested_fire_offset == -172 &&
        r.limited_fire_offset == -172 && r.fire_delta == 0 &&
        r.dwell_delta == 0) ++passed;
    printf("  angle->time: P=410, raw=99 -> %u counts; advance with -13 corr = %d  %s\n",
           (unsigned int)r.angle_time_counts, (int)r.limited_fire_offset,
           (r.angle_time_counts == 159u && r.requested_fire_offset == -172 &&
            r.limited_fire_offset == -172 && r.fire_delta == 0 &&
            r.dwell_delta == 0) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    ram16be_set(REF_PERIOD_ADDR, 410u);
    ram16be_set(DWELL_COUNTS_ADDR, 284u);
    mpu16be_set((bua_u16)MPU_SPARK_FIRE_OFFSET_ADDR, 0u);
    mpu16be_set((bua_u16)MPU_SPARK_OLD_DWELL_ADDR, 284u);
    ENGINE_MODE_WORD &= (bua_u8)~SPARK_ADV_RETARD_FLAG;
    r = bua_spark_timing_output_ld1f6(99u);
    if (r.requested_fire_offset == -172 && r.limited_fire_offset == -26 &&
        r.fire_delta == -26 &&
        (bua_s16)mpu16be_get((bua_u16)MPU_SPARK_FIRE_OFFSET_ADDR) == -26) ++passed;
    printf("  advance slew: request=-172, old=0, -P/16 step -> %d  %s\n",
           (int)r.limited_fire_offset,
           (r.requested_fire_offset == -172 && r.limited_fire_offset == -26 &&
            r.fire_delta == -26) ? "PASS" : "FAIL");
    r = bua_spark_timing_output_ld1f6(99u);
    if (r.limited_fire_offset == -52 && r.fire_delta == -26) ++passed;
    printf("  next execution continues slew: -26 -> %d (delta %d)  %s\n",
           (int)r.limited_fire_offset, (int)r.fire_delta,
           (r.limited_fire_offset == -52 && r.fire_delta == -26) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    ram16be_set(REF_PERIOD_ADDR, 410u);
    ram16be_set(DWELL_COUNTS_ADDR, 284u);
    mpu16be_set((bua_u16)MPU_SPARK_OLD_DWELL_ADDR, 284u);
    ENGINE_MODE_WORD |= SPARK_ADV_RETARD_FLAG;
    r = bua_spark_timing_output_ld1f6(11u);
    if (r.angle_time_counts == 18u && r.requested_fire_offset == 5 &&
        r.limited_fire_offset == 5) ++passed;
    printf("  retard path: raw=11 -> +18 counts, minus 13 -> +%d  %s\n",
           (int)r.limited_fire_offset,
           (r.angle_time_counts == 18u && r.limited_fire_offset == 5) ? "PASS" : "FAIL");
    limited = bua_spark_limit_fire_offset((bua_u16)(0u - 500u),
                                           (bua_u16)(0u - 500u),
                                           410u, &slew);
    if ((bua_s16)limited == -410) ++passed;
    printf("  absolute advance limit: -500 counts -> %d (= -one ref period)  %s\n",
           (int)(bua_s16)limited,
           ((bua_s16)limited == -410) ? "PASS" : "FAIL");
    limited = bua_spark_limit_fire_offset(500u, 500u, 410u, &slew);
    if (limited == 410u) ++passed;
    printf("  absolute retard limit: +500 counts -> %u (= +one ref period)  %s\n",
           (unsigned int)limited, (limited == 410u) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    ram16be_set(REF_PERIOD_ADDR, 410u);
    ram16be_set(DWELL_COUNTS_ADDR, 280u);
    mpu16be_set((bua_u16)MPU_SPARK_FIRE_OFFSET_ADDR, (bua_u16)(0u - 172u));
    mpu16be_set((bua_u16)MPU_SPARK_OLD_DWELL_ADDR, 300u);
    ENGINE_MODE_WORD &= (bua_u8)~SPARK_ADV_RETARD_FLAG;
    r = bua_spark_timing_output_ld1f6(99u);
    if (r.fire_delta == 0 && r.dwell_delta == 20 &&
        mpu16be_get((bua_u16)MPU_SPARK_DWELL_DELTA_ADDR) == 20u &&
        mpu16be_get((bua_u16)MPU_SPARK_OLD_DWELL_ADDR) == 280u) ++passed;
    printf("  dwell delta: fire delta 0 + old dwell 300 - new 280 -> %d  %s\n",
           (int)r.dwell_delta,
           (r.fire_delta == 0 && r.dwell_delta == 20) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    ram16be_set(REF_PERIOD_ADDR, 410u);
    ram16be_set(DWELL_COUNTS_ADDR, 300u);
    mpu16be_set((bua_u16)MPU_SPARK_FIRE_OFFSET_ADDR, (bua_u16)(0u - 146u));
    mpu16be_set((bua_u16)MPU_SPARK_OLD_DWELL_ADDR, 284u);
    ENGINE_MODE_WORD &= (bua_u8)~SPARK_ADV_RETARD_FLAG;
    r = bua_spark_timing_output_ld1f6(99u);
    if (r.limited_fire_offset == -172 && r.fire_delta == -26 &&
        r.dwell_delta == -42) ++passed;
    printf("  combined MPU update: fire -26 + dwell 284-300 -> %d  %s\n",
           (int)r.dwell_delta,
           (r.limited_fire_offset == -172 && r.fire_delta == -26 &&
            r.dwell_delta == -42) ? "PASS" : "FAIL");
    printf("  step-20 timing/output regression result: %s (%u/8)\n",
           (passed == 8u) ? "PASS" : "FAIL", passed);
    mem = saved_mem;
    stats = saved_stats;
}
static void run_step19_mode4_blend_test(void)
{
    BuaMemory saved_mem;
    BuaStats saved_stats;
    unsigned int passed;
    bua_u8 magnitude;
    bua_u16 raw;
    saved_mem = mem;
    saved_stats = stats;
    passed = 0u;
    printf("\nMode-4 / spark-blend regression (LD192..LD1EE):\n");
    memset(&mem, 0, sizeof(mem));
    ram16be_set(SPARK_REL_REF_ADDR, 100u);
    SPARK_BLEND_MULT = 128u;
    magnitude = bua_spark_mode4_blend_ld192();
    raw = ram16be_get(SPARK_REL_REF_ADDR);
    if (raw == 50u && magnitude == 50u &&
        (ENGINE_MODE_WORD & SPARK_ADV_RETARD_FLAG) == 0u) ++passed;
    printf("  normal positive blend: 100 * 128/256 -> %u, magnitude=%u  %s\n",
           (unsigned int)raw, (unsigned int)magnitude,
           (raw == 50u && magnitude == 50u &&
            (ENGINE_MODE_WORD & SPARK_ADV_RETARD_FLAG) == 0u) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    ram16be_set(SPARK_REL_REF_ADDR, 119u);
    SPARK_BLEND_MULT = 255u;
    magnitude = bua_spark_mode4_blend_ld192();
    raw = ram16be_get(SPARK_REL_REF_ADDR);
    if (raw == 119u && magnitude == 119u) ++passed;
    printf("  LF266 rounding at blend=255: 119 -> %u  %s\n",
           (unsigned int)raw, (raw == 119u && magnitude == 119u) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    ram16be_set(SPARK_REL_REF_ADDR, (bua_u16)65525u); /* -11 */
    SPARK_BLEND_MULT = 0u;
    magnitude = bua_spark_mode4_blend_ld192();
    raw = ram16be_get(SPARK_REL_REF_ADDR);
    if (raw == 65525u && magnitude == 11u &&
        (ENGINE_MODE_WORD & SPARK_ADV_RETARD_FLAG) != 0u) ++passed;
    printf("  negative retard bypasses blend: L009D=%d magnitude=%u flag=%u  %s\n",
           (int)(bua_s16)raw, (unsigned int)magnitude,
           (unsigned int)((ENGINE_MODE_WORD & SPARK_ADV_RETARD_FLAG) != 0u),
           (raw == 65525u && magnitude == 11u &&
            (ENGINE_MODE_WORD & SPARK_ADV_RETARD_FLAG) != 0u) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    ram16be_set(SPARK_REL_REF_ADDR, 100u);
    SPARK_BLEND_MULT = 128u;
    DIAG_MODE_WORD3 = DIAG_FIXED_SPARK_BIT;
    magnitude = bua_spark_mode4_blend_ld192();
    raw = ram16be_get(SPARK_REL_REF_ADDR);
    if (raw == 100u && magnitude == 100u) ++passed;
    printf("  diagnostics bypass blend: L009D=%u  %s\n",
           (unsigned int)raw, (raw == 100u && magnitude == 100u) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    ram16be_set(SPARK_REL_REF_ADDR, 83u);
    MINOR_MODE_WORD2 = MINOR_MODE4_BIT;
    ALDL_FUNC_MOD_ENABLE = ALDL_SPARK_ENABLE_BIT;
    ALDL_SPARK_COMMAND = 23u;
    SPARK_BLEND_MULT = 128u;
    magnitude = bua_spark_mode4_blend_ld192();
    raw = ram16be_get(SPARK_REL_REF_ADDR);
    if (raw == 0u && magnitude == 0u) ++passed;
    printf("  Mode-4 direct non-retard branch: L009D=%04X magnitude=%u  %s\n",
           (unsigned int)raw, (unsigned int)magnitude,
           (raw == 0u && magnitude == 0u) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    ram16be_set(SPARK_REL_REF_ADDR, 83u);
    MINOR_MODE_WORD2 = MINOR_MODE4_BIT;
    ALDL_FUNC_MOD_ENABLE = (bua_u8)(ALDL_SPARK_ENABLE_BIT | ALDL_SPARK_RETARD_BIT);
    ALDL_SPARK_COMMAND = 23u;
    magnitude = bua_spark_mode4_blend_ld192();
    raw = ram16be_get(SPARK_REL_REF_ADDR);
    if (raw == 0x80E9u && magnitude == 23u &&
        (ENGINE_MODE_WORD & SPARK_ADV_RETARD_FLAG) != 0u) ++passed;
    printf("  Mode-4 direct retard: L009D=%04X magnitude=%u flag=%u  %s\n",
           (unsigned int)raw, (unsigned int)magnitude,
           (unsigned int)((ENGINE_MODE_WORD & SPARK_ADV_RETARD_FLAG) != 0u),
           (raw == 0x80E9u && magnitude == 23u &&
            (ENGINE_MODE_WORD & SPARK_ADV_RETARD_FLAG) != 0u) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    ram16be_set(SPARK_REL_REF_ADDR, 83u);
    MINOR_MODE_WORD2 = MINOR_MODE4_BIT;
    ALDL_FUNC_MOD_ENABLE = (bua_u8)(ALDL_SPARK_ENABLE_BIT |
                                     ALDL_SPARK_MODIFY_BIT |
                                     ALDL_SPARK_RETARD_BIT);
    ALDL_SPARK_COMMAND = 10u;
    magnitude = bua_spark_mode4_blend_ld192();
    raw = ram16be_get(SPARK_REL_REF_ADDR);
    if (raw == 73u && magnitude == 73u) ++passed;
    printf("  Mode-4 modify/subtract: 83-10 -> L009D=%u  %s\n",
           (unsigned int)raw, (raw == 73u && magnitude == 73u) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    ram16be_set(SPARK_REL_REF_ADDR, 83u);
    MINOR_MODE_WORD2 = MINOR_MODE4_BIT;
    ALDL_FUNC_MOD_ENABLE = (bua_u8)(ALDL_SPARK_ENABLE_BIT | ALDL_SPARK_MODIFY_BIT);
    ALDL_SPARK_COMMAND = 10u;
    magnitude = bua_spark_mode4_blend_ld192();
    raw = ram16be_get(SPARK_REL_REF_ADDR);
    if (raw == 0x6F5Du && magnitude == 93u) ++passed;
    printf("  Mode-4 modify/add literal ADCA #0111: L009D=%04X magnitude=%u  %s\n",
           (unsigned int)raw, (unsigned int)magnitude,
           (raw == 0x6F5Du && magnitude == 93u) ? "PASS" : "FAIL");
    printf("  step-19 Mode-4/blend regression result: %s (%u/8)\n",
           (passed == 8u) ? "PASS" : "FAIL", passed);
    mem = saved_mem;
    stats = saved_stats;
}
static void run_step18_spark_sum_test(void)
{
    BuaMemory saved_mem;
    BuaStats saved_stats;
    unsigned int passed;
    bua_s16 v;
    bua_u8 ktest;
    saved_mem = mem;
    saved_stats = stats;
    passed = 0u;
    printf("\nSpark summation regression (LD063..LD192, before Mode 4/blend):\n");
    memset(&mem, 0, sizeof(mem));
    v = bua_spark_sum_ld096(100u, 57u, 0u, 0u, 0u, 0u, 0u);
    KNOCK_RETARD = 0u;
    v = bua_spark_relative_to_reference_pre_mode4();
    if ((bua_s16)ram16be_get(FINAL_SPARK_ADV_ADDR) == 100 && v == 83) ++passed;
    printf("  clean stack: L0115=100, minus ref lead 17 -> L009D=%d  %s\n",
           (int)v, ((bua_s16)ram16be_get(FINAL_SPARK_ADV_ADDR) == 100 && v == 83) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    v = bua_spark_sum_ld096(134u, 85u, 28u, 0u, 57u, 23u, 28u);
    if (v == 298 && ram16be_get(FINAL_SPARK_ADV_ADDR) == 298u) ++passed;
    printf("  16-bit X accumulator: 134+85+28+57+23+28-57 = %d  %s\n",
           (int)v, (v == 298) ? "PASS" : "FAIL");
    KNOCK_RETARD = 40u;
    DIAG_MODE_WORD3 = 0u;
    v = bua_spark_relative_to_reference_pre_mode4();
    if (v == 99) ++passed;
    printf("  max advance then knock: (298-17) capped 119, -20 knock -> %d  %s\n",
           (int)v, (v == 99) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    v = bua_spark_sum_ld096(0u, 57u, 0u, 28u, 0u, 0u, 0u);
    KNOCK_RETARD = 0u;
    v = bua_spark_relative_to_reference_pre_mode4();
    if (v == -11 && ram16be_get(SPARK_REL_REF_ADDR) == (bua_u16)65525u) ++passed;
    printf("  minimum retard: L0115=-28, lead gives -45, clamp -> %d  %s\n",
           (int)v, (v == -11) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    (void)bua_spark_sum_ld096(100u, 57u, 0u, 0u, 0u, 0u, 0u);
    KNOCK_RETARD = 114u;
    DIAG_MODE_WORD3 = DIAG_FIXED_SPARK_BIT;
    v = bua_spark_relative_to_reference_pre_mode4();
    if (v == 57) ++passed;
    printf("  diagnostic override: fixed L009D=57 after knock path -> %d  %s\n",
           (int)v, (v == 57) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    SPARK_INPUT_MODE_WORD = SPARK_INPUT_TEST_ENABLE_BIT;
    MODE_WORD2 = MODE2_ERR43_TEST_BIT;
    COOLANT = 174u;
    ktest = bua_knock_test_spark_eval(0u);
    if (ktest == 57u) ++passed;
    printf("  error-43 test spark qualified: raw=%u  %s\n",
           (unsigned int)ktest, (ktest == 57u) ? "PASS" : "FAIL");
    MODE_WORD2 = MODE2_ERR43_TEST_BIT;
    ktest = bua_knock_test_spark_eval(28u);
    if (ktest == 0u && (MODE_WORD2 & MODE2_ERR43_TEST_BIT) == 0u) ++passed;
    printf("  hot-restart disqualifies test spark and clears b0: raw=%u b0=%u  %s\n",
           (unsigned int)ktest, (unsigned int)(MODE_WORD2 & MODE2_ERR43_TEST_BIT),
           (ktest == 0u && (MODE_WORD2 & MODE2_ERR43_TEST_BIT) == 0u) ? "PASS" : "FAIL");
    MINOR_MODE_WORD2 = MINOR_ALDL_SPARK_BIT;
    if (bua_aldl_added_spark_eval() == 23u) ++passed;
    printf("  ALDL added spark when L0035 b5 set: raw=%u  %s\n",
           (unsigned int)bua_aldl_added_spark_eval(),
           (bua_aldl_added_spark_eval() == 23u) ? "PASS" : "FAIL");
    printf("  step-18 spark-sum regression result: %s (%u/8)\n",
           (passed == 8u) ? "PASS" : "FAIL", passed);
    mem = saved_mem;
    stats = saved_stats;
}
static void run_step17_knock_test(void)
{
    BuaMemory saved_mem;
    BuaStats saved_stats;
    unsigned int passed;
    bua_u8 v;
    saved_mem = mem;
    saved_stats = stats;
    passed = 0u;
    printf("\nKnock-control regression (LD0D1..LD157 + LEB3A..LEB59):\n");
    memset(&mem, 0, sizeof(mem));
    COOLANT = 141u;
    STARTUP_COOLANT = 100u;
    RPM_DIV12P5 = 192u;
    RPM_DIV25 = 96u;
    RPM_SPECIAL = 160u;
    BATTERY_AD = 128u;
    v = bua_knock_attack_eval(64u);
    if (v == 0u && (DIAG_MODE_WORD2 & KNOCK_ENABLED_FLAG) == 0u) ++passed;
    printf("  coolant qualification: cool=141 start=100 -> retard=%u enabled=%u  %s\n",
           (unsigned int)v,
           (unsigned int)((DIAG_MODE_WORD2 & KNOCK_ENABLED_FLAG) != 0u),
           (v == 0u && (DIAG_MODE_WORD2 & KNOCK_ENABLED_FLAG) == 0u) ? "PASS" : "FAIL");
    memset(&mem, 0, sizeof(mem));
    COOLANT = 140u;
    STARTUP_COOLANT = 80u;
    RPM_DIV12P5 = 192u;
    RPM_DIV25 = 96u;
    RPM_SPECIAL = 160u;
    BATTERY_AD = 128u;
    v = bua_knock_attack_eval(128u);
    if (v == 15u && (MODE_WORD3 & KNOCK_DIFF_COOL_LATCH) != 0u) ++passed;
    printf("  attack @2400-ish: knockCount=128 -> retard=%u (spark subtraction=%u raw) latch=%u  %s\n",
           (unsigned int)v, (unsigned int)bua_knock_spark_subtract_raw(),
           (unsigned int)((MODE_WORD3 & KNOCK_DIFF_COOL_LATCH) != 0u),
           (v == 15u && (MODE_WORD3 & KNOCK_DIFF_COOL_LATCH) != 0u) ? "PASS" : "FAIL");
    KNOCK_RETARD = 110u;
    v = bua_knock_attack_eval(128u);
    if (v == 114u) ++passed;
    printf("  normal maximum: old=110 + attack -> retard=%u (LC1BB=114)  %s\n",
           (unsigned int)v, (v == 114u) ? "PASS" : "FAIL");
    AF_MODE_WORD |= POWER_ENRICH_ACTIVE;
    KNOCK_RETARD = 60u;
    v = bua_knock_attack_eval(128u);
    if (v == 65u) ++passed;
    printf("  WOT limit @2400 RPM: C1C8 lookup -> retard=%u (expected 65)  %s\n",
           (unsigned int)v, (v == 65u) ? "PASS" : "FAIL");
    AF_MODE_WORD = 0u;
    MODE_WORD2 |= KNOCK_FAIL_MODE_BIT;
    KNOCK_RETARD = 3u;
    v = bua_knock_attack_eval(0u);
    if (v == 45u) ++passed;
    printf("  fail-safe path: L0002 b7 -> retard=%u (LC22A=45)  %s\n",
           (unsigned int)v, (v == 45u) ? "PASS" : "FAIL");
    MODE_WORD2 = 0u;
    KNOCK_RETARD = 114u;
    RPM_SPECIAL = 160u;
    MINOR_COUNT = 0x0Au;
    bua_knock_recovery_segment_a();
    if (KNOCK_RETARD == 101u) ++passed;
    printf("  Segment-A recovery: 114 -> %u at RPM-special=160  %s\n",
           (unsigned int)KNOCK_RETARD, (KNOCK_RETARD == 101u) ? "PASS" : "FAIL");
    KNOCK_RETARD = 1u;
    MINOR_COUNT = 0x0Au;
    bua_knock_recovery_segment_a();
    if (KNOCK_RETARD == 0u) ++passed;
    printf("  minimum recovery decrement: 1 -> %u  %s\n",
           (unsigned int)KNOCK_RETARD, (KNOCK_RETARD == 0u) ? "PASS" : "FAIL");
    KNOCK_RETARD = 100u;
    MINOR_COUNT = 0x1Au;
    bua_knock_recovery_segment_a();
    if (KNOCK_RETARD == 100u) ++passed;
    printf("  Segment-A b4 gating: count=1A leaves retard=%u  %s\n",
           (unsigned int)KNOCK_RETARD, (KNOCK_RETARD == 100u) ? "PASS" : "FAIL");
    printf("  step-17 knock regression result: %s (%u/8)\n",
           (passed == 8u) ? "PASS" : "FAIL", passed);
    mem = saved_mem;
    stats = saved_stats;
}

