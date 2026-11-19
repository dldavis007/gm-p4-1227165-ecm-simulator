/* LE87A..LEA25, fan, cold-start and throttle-follower motor requests. */
static void bua_idle_motor_request(void)
{
    bua_u8 control = RAM8(0xF2u);
    bua_u8 steer, ac, small_close, request;
    BuaFollower46 follower;
    BuaIacTfDesiredStep39 demand;
    if (!(RAM8(0xF3u) & 8u))
        return;
    if (!(ENGINE_MODE_WORD & 8u) && RAM8(0xF4u)) {
        if (RAM8(0x10Eu) < 10u) { /* twice LC632 */
            RAM8(0x101u) = 2u;
            RAM8(0xF2u) = (bua_u8)((control | 8u) & ~16u);
            return;
        }
        ENGINE_MODE_WORD |= 8u;
        RAM8(0x10Bu) = 20u; /* LC676 */
    } else if (!(MODE_WORD1 & 0x40u) && (RAM8(0x3Bu) & 0x40u)) {
        if (RAM8(0x10Eu) < 10u) { /* twice LC633 */
            RAM8(0x101u) = 0x82u;
            RAM8(0xF2u) = (bua_u8)((control | 8u) & ~16u);
            return;
        }
        MODE_WORD1 |= 0x40u;
        RAM8(0x3Bu) &= (bua_u8)~0x40u;
        RAM8(0x10Bu) = 20u; /* LC677 */
    }
    control &= (bua_u8)~8u;
    RAM8(0xF2u) = control;
    RAM8(0x10Eu) = 0u;
    if (RAM8(0xF3u) & 0x20u)
        return;
    follower = bua_dfco_follower_step46(RAM8(0x84u), AIR_MODE_WORD,
                                        RAM8(0xC5u), AF_MODE_WORD);
    RAM8(0xC5u) = follower.duration_left;
    steer = RAM8(0x37u) & 8u;
    if (follower.virtual_tps <= STEP39_TF_TPS_THRESHOLD &&
        RAM8(0x85u) > STEP39_TF_TPS_THRESHOLD)
        RAM8(0x10Bu) = 30u;
    if (follower.virtual_tps > STEP39_TF_TPS_THRESHOLD || (control & 0x50u) ||
        steer || RAM8(0x105u)) {
        /* LE961 is bypassed when power steering owns the load. */
        ac = (bua_u8)(!steer && !(control & 0x80u));
        demand = bua_iac_tf_desired_step39(follower.virtual_tps, RAM8(0xF3u) & 0x80u,
                                           RAM8(0x67u), ac, RAM8(0x2Du), steer);
        if (demand.ac_learn_reset) {
            RAM8(0x2Du) = demand.ac_steps;
            AIR_MODE_WORD |= 0x40u;
        }
        if (steer)
            control = (bua_u8)((control | 0x40u) & ~1u);
        else if ((control & 0xC0u) == 0xC0u)
            control |= 0x20u;
        control |= 0x10u;
        request = bua_tf_pack_request_step66(demand.desired_steps, RAM8(0x105u), &small_close);
        RAM8(0x101u) = request;
        if (small_close)
            RAM8(0x10Bu) = 30u;
        if (request & 0x7Fu) {
            if ((request & 0x80u) || RAM8(0x2Cu) < 145u) {
                RAM8(0xF2u) = control;
                return;
            }
            control &= (bua_u8)~0x10u;
        } else {
            control &= (bua_u8)~0x30u;
            if ((control & 0x80u) && !steer)
                control &= (bua_u8)~0x41u;
        }
    }
    RAM8(0xF2u) = control;
    if ((MINOR_MODE_WORD2 & 8u) && (RAM8(0x154u) & 2u)) {
        control &= (bua_u8)~4u;
        if (!(RAM8(0x155u) & 2u))
            control |= 4u;
        RAM8(0xF2u) = control;
        return;
    }
    if (!(control & 4u)) {
        request = 0u;
        if (!RAM8(0x67u) && RAM8(0x84u) < 3u) {
            if (RPM_DIV12P5 > RAM8(0x102u)) {
                request = 0x82u;
                control |= 2u;
            } else if (control & 2u) {
                control |= 4u;
                RAM8(0x10Bu) = 40u;
            } else {
                request = 2u;
            }
        }
        RAM8(0x101u) = request;
        RAM8(0xF2u) = control;
    }
}

/* LD4CD doubles D and rotates B through carry. On negative overflow the
 * CLRB at LD4D3 also clears carry, so the returned byte is zero. */
static int bua_idle_signed_byte(long value)
{
    if (value < -128)
        return 0;
    return value > 127 ? 127 : (int)value;
}

/* LD59A..LD6C3. RPM and the previous RPM are both 8.8 fixed point. */
static void bua_idle_feedback(int error)
{
    long change = (long)ram16be_get(0xFFu) - ram16be_get(0x58u);
    int rate = bua_idle_signed_byte(change < 0 ? -((-change + 15) / 16) : change / 16);
    int neutral = RAM8(0xF3u) & 128u;
    int deadband = neutral ? STEP37_PN_DEADBAND_RAW : STEP37_DRIVE_DEADBAND_RAW;
    unsigned int magnitude = (unsigned int)abs(error);
    int linear = 0, fraction, residual, steps;
    unsigned int product, word, gain;
    bua_u8 coolant_gain;
    if (magnitude > (unsigned int)deadband)
        linear = step37_mul_gain_signed(error, STEP37_PROP_UNDER_GAIN);
    if (abs(rate) >= 16) { /* LC67C */
        if (error > 4 && rate > 22) /* LC658, LC656, LC654+LC657 */
            linear += step37_clamp127(rate + step37_mul_gain_signed(rate, 15u));
        else
            linear += step37_mul_gain_signed(rate, rate < 0 ? STEP37_DERIV_NEG_GAIN : STEP37_DERIV_POS_GAIN);
    }
    linear = bua_idle_signed_byte(linear);
    magnitude = abs(linear);
    if (neutral)
        magnitude = step37_mul_gain_signed(magnitude, STEP37_PN_SCALE);
    RAM8(0x101u) = 0u;
    if (magnitude >= STEP59_INV_QUANT) {
        RAM8(0x103u) = 0u;
    } else {
        if (abs(rate) > STEP59_RATE_LIMIT || !error) {
            RAM8(0x103u) = 0u;
            return;
        }
        gain = STEP59_INT_GAIN_PN;
        if (abs(error) <= deadband)
            gain = neutral ? STEP59_LOW_INT_GAIN_PN : STEP59_LOW_INT_GAIN_DRIVE;
        fraction = RAM8(0x103u);
        if (fraction >= 128)
            fraction -= 256;
        magnitude = (unsigned int)abs(error) * gain / 128u;
        linear = fraction + (error < 0 ? -(int)magnitude : (int)magnitude);
        if (linear > 127 || linear < -128)
            linear = linear < 0 ? -127 : 127;
        RAM8(0x103u) = (bua_u8)linear;
        magnitude = abs(linear);
        if (magnitude < STEP59_INV_QUANT)
            return;
    }
    product = magnitude * STEP59_QUANT_GAIN;
    word = (step59_round_high_byte(product) << 8) | (product & 255u);
    residual = step59_round_high_byte((product & 255u) * STEP59_INV_QUANT);
    coolant_gain = bua_interp_u8_lf2d6(step37_iac_cool_mult[COOLANT >> 5],
                    step37_iac_cool_mult[(COOLANT >> 5) + 1], (bua_u8)(COOLANT << 3));
    steps = step59_round_high_byte(bua_mul_u8_u16_lf266(coolant_gain, (bua_u16)word));
    RAM8(0x101u) = (bua_u8)(linear < 0 ? steps | 128 : steps);
    if (steps)
        RAM8(0xF3u) |= 0x20u;
    if (RAM8(0x103u))
        RAM8(0x103u) = (bua_u8)(linear < 0 ? -residual : residual);
}

/* LD3F0..LD450 and LD6CA. The commanded idle update runs every eighth
 * minor loop. LD468 compares A after the Mode-4 option tests. */
static void bua_running_idle_speed(void)
{
    BuaIacCommandStep36 target;
    bua_u8 compare;
    int error, idle, inhibited;
    RAM8(0x85u) = RAM8(0x84u);
    if (RAM8(0x82u) != RAM8(0x84u))
        RAM8(0x84u) = RAM8(0x82u);
    if (RAM8(0x3Fu) & 0x24u)
        RAM8(0x84u) = 2u * 3u;
    bua_idle_motor_request();
    if (MINOR_COUNT & 7u)
        return;
    RAM8(0xF3u) &= (bua_u8)~0x20u;
    target = bua_iac_command_step36(COOLANT, RAM8(0xF3u), RAM8(0xF2u),
                                    RAM8(0x3Au), MINOR_MODE_WORD2);
    compare = RAM8(0x102u) = target.command_raw;
    if (MINOR_MODE_WORD2 & 8u) {
        compare = RAM8(0x158u);
        if ((compare & 3u) == 3u)
            compare = RAM8(0x102u) = RAM8(0x159u);
    }
    if (compare < RAM8(0x58u))
        RAM8(0xF2u) |= 2u;
    error = bua_idle_signed_byte((int)RAM8(0x102u) - RAM8(0x58u));
    if (abs(error) > 2)
        RAM8(0x107u) = 0u;
    else if (RAM8(0x107u) != 255u)
        ++RAM8(0x107u);
    /* LD4AE..LD503: count down the transition inhibit and sample the
     * park/neutral switch while the throttle is closed and speed is zero. */
    if (RAM8(0x10Bu))
        --RAM8(0x10Bu);
    idle = RAM8(0x84u) < 3u && RAM8(0x67u) <= 6u;
    inhibited = idle && !RAM8(0x67u) && RAM8(0x10Bu);
    if (idle && !inhibited && !RAM8(0x67u)) {
        if ((RAM8(0xF3u) & 0x90u) == 0x10u) {
            RAM8(0x10Bu) = 16u;
            RAM8(0xF3u) &= (bua_u8)~0x10u;
            RAM8(0x107u) = 0u;
        } else if ((RAM8(0xF3u) & 0x90u) == 0x80u) {
            RAM8(0xF3u) |= 0x10u;
            RAM8(0x107u) = 0u;
        }
        if (!(RAM8(0xF3u) & 0x81u) && RAM8(0x107u) > 20u &&
            !(RAM8(0x3Au) & 0x20u) && MINOR_COUNT == 0x30u && RAM8(0x10Fu)) {
            if (RAM8(0x10Fu) & 0x80u) {
                if (--RAM8(0x109u) == 0u)
                    ++RAM8(0x109u);
            } else if (++RAM8(0x109u) > RAM8(0x10Cu)) {
                RAM8(0x109u) = RAM8(0x10Cu);
            }
        }
    }
    if (!idle)
        RAM8(0xF2u) &= (bua_u8)~1u;
    if (idle && (RAM8(0xF2u) & 0x1Cu) == 4u && !(RAM8(0x37u) & 8u) &&
        !RAM8(0x106u) && !inhibited) {
        bua_idle_feedback(error);
    } else if (error > 4 && (!idle || inhibited || (RAM8(0xF2u) & 4u))) {
        RAM8(0xF2u) &= (bua_u8)~0x18u;
        bua_idle_feedback(error);
    } else {
        RAM8(0x107u) = 0u;
        if (RAM8(0x84u) >= 8u && RAM8(0x67u) > 6u &&
            RAM8(0x10Fu) && RAM8(0x10Fu) < 128u &&
            (MINOR_COUNT & 0x18u) == 0x18u && !(RAM8(0xF2u) & 8u)) {
            RAM8(0xF2u) &= (bua_u8)~16u;
            RAM8(0x101u) = 0x81u;
            RAM8(0xF3u) |= 0x20u;
        }
    }
    ram16be_set(0xFFu, ram16be_get(0x58u));
}

/* ---------------------------------------------------------------------- */
/* Step 90: scheduler-facing wiring for Segments 1/5/6/A/E.               */
/*                                                                        */
/* These wrappers deliberately expose only source behavior already proven */
/* by earlier regressions.  Hardware-only output polarity and the damaged  */
/* downstream Segment-1 discrete staging remain outside this step.         */
/* ---------------------------------------------------------------------- */
/* Step 100: LEA28..LEA94.  Channel 7 is the diagnostic-pin voltage at the
 * HAL boundary; the default PC stimulus (200) selects ordinary operation. */
static void seg3_misc_100ms(void)
{
    bua_u8 raw;
    bua_u8 mode;
    bua_u16 filtered;

    mode=(bua_u8)(RAM8(0x0035u)&0xCFu);
    raw=hw_adc(0x70u);
    if(raw<40u)
        mode|=0x10u;
    else if(raw>=100u && raw<152u && (RAM8(0x0046u)&0x08u)==0u)
        mode|=0x20u;
    RAM8(0x0035u)=mode;
    if(RAM8(0x01B2u)!=0u)
        RAM8(0x0033u)|=0x04u;
    mpu16be_set(0x3FCEu,0u);
    if((ENGINE_MODE_WORD&ENGINE_RUNNING_BIT)==0u) {
        RAM8(0x0043u)&=0xD5u;
        RAM8(0x005Fu)=RAM8(0x005Bu);
        if((RAM8(0x0001u)&0x40u)==0u && RAM8(0x005Bu)>=113u)
            RAM8(0x0001u)|=0x40u;
        ram16be_set(0x006Fu,(bua_u16)(102u<<8));
        RAM8(0x0073u)=102u;
    } else {
        RAM8(0x0002u)&=0xEFu;
        filtered=bua_lag_filter_8_8(ram16be_get(0x0073u),RAM8(0x006Fu),5u);
        ram16be_set(0x0073u,filtered);
    }
    mem.io4000[3]=0x8Fu;
}

/* Step 100: LEA95..LEB39.  The coolant-derived normal producers are always
 * updated.  The key-off MAF burn-off branch is retained as raw ECM/MPU state;
 * no electrical behavior is assigned to the MPU counter value. */
static void seg7_coolant_variables(void)
{
    bua_u8 coolant;
    bua_u8 limited;
    bua_u8 raw;
    bua_u8 counter;

    coolant=RAM8(0x005Bu);
    limited=(coolant>208u)?208u:coolant;
    RAM8(0x005Eu)=limited;
    RAM8(0x00D1u)=bua_lookup_spacing16(cal_o2_lean_offset_c5bb,14u,limited);
    RAM8(0x00D7u)=bua_lookup_spacing32(cal_async_pulse_count,9u,coolant);
    RAM8(0x00D8u)=bua_lookup_spacing32(cal_async_cool_factor,9u,coolant);
    RAM8(0x00D0u)=bua_lookup_spacing32(cal_pe_coolant_change,9u,coolant);

    if((RAM8(0x003Bu)&0x02u)!=0u && (RAM8(0x0033u)&0x32u)!=0x32u) {
        RAM8(0x0472u)=(bua_u8)(RAM8(0x0472u)+1u);
        if(RAM8(0x0472u)==0u)
            RAM8(0x0472u)=0xFFu;
    }
    if((RAM8(0x0033u)&0x10u)==0u || (RAM8(0x003Eu)&0x40u)==0u) {
        mpu16be_set(0x3FDAu,0xD000u);
        if((RAM8(0x0003u)&0x40u)!=0u)
            RAM8(0x004Eu)|=0x80u;
        return;
    }
    if(RAM8(0x00B8u)!=0u) {
        --RAM8(0x00B8u);
        mpu16be_set(0x3FDAu,0xD000u);
        if((RAM8(0x0003u)&0x40u)!=0u)
            RAM8(0x004Eu)|=0x80u;
        return;
    }
    if(RAM8(0x00B7u)==0u) {
        mpu16be_set(0x3FDAu,0xD000u);
        if((RAM8(0x0003u)&0x40u)!=0u)
            RAM8(0x004Eu)|=0x80u;
        return;
    }
    --RAM8(0x00B7u);
    mpu16be_set(0x3FDAu,0xDFFFu);
    if((RAM8(0x003Fu)&0x40u)!=0u)
        return;
    counter=RAM8(0x011Fu);
    if(counter<3u) {
        RAM8(0x011Fu)=(bua_u8)(counter+1u);
        return;
    }
    raw=hw_adc(0xA0u);
    if(raw<=95u && raw>=20u) {
        RAM8(0x0003u)&=0xBFu;
        return;
    }
    counter=RAM8(0x0120u);
    if(counter<6u)
        RAM8(0x0120u)=(bua_u8)(counter+1u);
    else
        RAM8(0x0003u)|=0x40u;
}

static void seg6_coolant_adc(void)
{
    BuaCool85 r;
    /* LF3B6 literally calls the Segment-4/H.U. hook first. */
    seg4_log_ram_to_hu();
    r=bua_cool85_step(hw_adc(0x40u),ram16be_get(0x001Au),
                      ram16be_get(0x005Bu),RAM8(0x0030u),RAM8(0x003Bu),
                      RAM8(0x0035u),RAM8(0x004Cu),
                      (bua_u8)((RAM8(0x003Cu)&0x80u)!=0u));
    RAM8(0x005Du)=r.converted_5d;
    ram16be_set(0x005Bu,r.coolant_5b);
    if (!r.failure && !(RAM8(0x3Bu) & 0x18u))
        RAM8(0x005Fu)=r.startup_cool_5f; /* Only LF42B captures startup coolant. */
    RAM8(0x0030u)=r.mode30;
    RAM8(0x003Bu)=r.mode3b;
    RAM8(0x0035u)=r.minor35;
    RAM8(0x004Cu)=r.err4c;
    RAM8(0x003Cu)=(bua_u8)(RAM8(0x003Cu)&0x7Fu);
}

static void bua_error43_service(void)
{
    /* LEB5B..LEBB3. Error 43 is enabled in LC1D2 of the BUA image. */
    if (MINOR_COUNT == 26u && !(MODE_WORD2 & 0x40u)) {
        if (MODE_WORD2 & 1u) {
            MODE_WORD2 &= (bua_u8)~1u;
            if (COOLANT > CAL_ERR43_TEST_MIN_COOLANT)
                MODE_WORD2 |= 0xC0u;
        } else if (!RAM8(0xB0u) && RAM8(0x63u) >= 170u) {
            MODE_WORD2 |= 1u;
        }
    }
    if (++RAM8(0xA4u) == 39u) {
        bua_u8 sample = (bua_u8)(mpu16be_get(0x3FCAu) >> 8);
        RAM8(0xA4u) = 0u;
        RAM8(0x3Bu) &= (bua_u8)~0x20u;
        if ((bua_u8)(sample - RAM8(0xA3u)) >= 235u)
            RAM8(0x3Bu) |= 0x20u;
        RAM8(0xA3u) = sample;
    }
}

static void segA_mat_variables(void)
{
    BuaMat84 r;
    bua_knock_recovery_segment_a();
    bua_error43_service();
    r=bua_mat84_step(hw_adc(0x80u),ram16be_get(0x001Au),RAM8(0x0065u),
                     RAM8(0x00ACu),RAM8(0x00ADu),RAM8(0x0043u),
                     RAM8(0x0040u),RAM8(0x004Cu),RAM8(0x004Du));
    RAM8(0x012Bu)=r.ad_inverted;
    RAM8(0x0060u)=r.mat;
    RAM8(0x00ACu)=r.low_timer;
    RAM8(0x00ADu)=r.high_timer;
    RAM8(0x0043u)=r.af_mode43;
    RAM8(0x0040u)=r.diag_mode40;
    RAM8(0x004Cu)=r.err23_latched4c;
    RAM8(0x004Du)=r.err25_latched4d;
}

/* LE193..LE28F. The leading byte of each TCC curve is also its
 * minimum road speed. LE1CD advances X past it on the enabled path. */
static bua_u8 bua_warm_tcc_candidate(bua_u8 mode)
{
    const bua_u8 *curve;
    BuaTccCoast72 coast;
    bua_u8 mph = VSS_MPH_HI;
    bua_u8 argument, threshold;
    unsigned int index;
    if (mode & 0x20u) {
        mode &= (bua_u8)~0x20u;
        mph = mph >= 26u ? (bua_u8)(mph - 26u) : 0u;
    }
    if (mode & 0xC0u)
        curve = mode & 0x12u ? step67_non4_unlock : step67_4th_unlock;
    else
        curve = mode & 8u ? step67_4th_lock : step67_non4_lock;
    if (RAM8(0x90u))
        return mode;
    if (!(mode & 1u) || mph < curve[0]) {
        if (mode & 1u)
            mode |= 6u;
        RAM8(0x8Du) = STEP73_LOCK_DELAY_CAL;
        return (bua_u8)(mode & 0x7Fu);
    }
    if (mode & 2u) {
        mode |= 4u;
        if (!(mode & 0xC0u))
            goto coast;
    }
    if (mode & 4u) {
        mode = (bua_u8)((mode | 0x80u) & ~4u);
        curve = step67_4th_unlock;
    }
coast:
    coast.shift45 = RAM8(0x45u);
    coast.mode04 = mode;
    bua_tcc_coast72(&coast, VSS_MPH_HI, TPS_LOAD_AXIS);
    RAM8(0x45u) = coast.shift45;
    if (!coast.coast_qualified) {
        argument = VSS_MPH_HI > 100u ? 200u : (bua_u8)(2u * VSS_MPH_HI);
        argument = argument > 24u ? (bua_u8)(argument - 24u) : 0u;
        index = 1u + (argument >> 4);
        threshold = curve[index];
        if (argument & 15u)
            threshold = bua_interp_u8_lf2d6(threshold, curve[index+1u],
                                            (bua_u8)(argument << 4));
        if ((mode & 0xC0u) ? threshold >= TPS_LOAD_AXIS : threshold > TPS_LOAD_AXIS)
            return (bua_u8)(mode | 0x81u);
    }
    return bua_tcc_tail73(mode, RPM_DIV25, VSS_MPH_HI, &RAM8(0x8Du));
}

/* LE2D1..LE34D: qualify and delay the raw shift-light request. */
static void bua_shift_light_service(void)
{
    static const bua_u8 on_tps[7] = {13,46,118,192,192,255,255};
    static const bua_u8 hold_tps[7] = {18,51,123,197,197,255,255};
    bua_u8 active = MINOR_MODE_WORD2 & 0x80u;
    bua_u8 rpm = RPM_DIV25;
    bua_u8 arg;
    bua_u16 delay;

    if (COOLANT <= 120u || VSS_MPH_HI <= 40u || FILTERED_LOAD == 0u ||
        RAM8(0x8Fu) <= 30u || rpm <= (active ? 52u : 56u))
        goto disqualify;
    if (rpm > (active ? 204u : 208u)) {
        ram16be_set(0x91u, 0u);
        SPARK_MODE_WORD |= 0x10u;
    } else {
        arg = (bua_u8)((rpm >> 1) - 16u);
        if (TPS_LOAD_AXIS > bua_lookup_spacing16(active ? hold_tps : on_tps, 7u, arg))
            goto disqualify;
        delay = ram16be_get(0x91u);
        if (!(SPARK_MODE_WORD & 0x10u)) {
            SPARK_MODE_WORD |= 0x10u;
            /* Every point at LC2EB is 150, with multiplier 2 at LC2EA. */
            delay = 300u;
        }
        if (delay != 0u) {
            ram16be_set(0x91u, (bua_u16)(delay - 1u));
            MINOR_MODE_WORD2 &= (bua_u8)~0x80u;
            return;
        }
    }
    MINOR_MODE_WORD2 |= 0x80u;
    return;

disqualify:
    SPARK_MODE_WORD &= (bua_u8)~0x10u;
    MINOR_MODE_WORD2 &= (bua_u8)~0x80u;
}

/* LE10F..LE2B9 automatic-transmission path after shifter sampling. */
static void bua_tcc_after_shifter(void)
{
    BuaGear68 gear;
    BuaTccGate68 gate;
    bua_u8 mode = RAM8(4u);
    bua_u8 nv = (bua_u8)(bua_fractional_divide((bua_u16)(RPM_DIV25 * 25u),
                                                ram16be_get(0x65u)) >> 8);
    RAM8(0x8Fu) = nv;
    if (SPARK_MODE_WORD & 8u)
        return;
    if (mode & STEP68_MODE_OD_ACTIVE) {
        BuaOdNv69 od;
        od.mode04 = mode;
        od.first_delay = RAM8(0x12Cu);
        od.downshift_inhibit = RAM8(0x90u);
        bua_od_nv_branch69(&od, nv);
        mode = od.mode04;
        RAM8(0x12Cu) = od.first_delay;
        RAM8(0x90u) = od.downshift_inhibit;
        RAM8(0x8Eu) = STEP68_FOURTH_CONFIRM;
    } else {
        gear.mode04 = mode;
        gear.fourth_timer = RAM8(0x8Eu);
        bua_nv_gear_update68(&gear, nv);
        mode = gear.mode04;
        RAM8(0x8Eu) = gear.fourth_timer;
        RAM8(0x12Cu) = STEP69_FIRST_DELAY;
    }
    if (COOLANT >= STEP67_TCC_COOL_MIN) {
        mode = bua_warm_tcc_candidate(mode);
    } else {
        mode &= (bua_u8)~0x20u;
        if ((mode & 0x80u) &&
            (RPM_DIV25 > 128u /* LC293 and LC294 */ || VSS_MPH_HI > STEP73_FORCE_MPH)) {
            RAM8(0x8Du) = 0u;
            mode |= 0x81u;
        } else {
            mode &= (bua_u8)~0x80u;
            RAM8(0x8Du) = STEP73_LOCK_DELAY_CAL;
        }
    }
    gate.status37 = RAM8(0x37u);
    gate.candidate_mode = mode;
    gate.lock_delay = RAM8(0x8Du);
    gate.downshift_inhibit = RAM8(0x90u);
    bua_tcc_final_gate68(&gate);
    RAM8(0x37u) = gate.status37;
    RAM8(0x8Du) = gate.lock_delay;
    RAM8(0x90u) = gate.downshift_inhibit;
    RAM8(4u) = (bua_u8)(mode & 0x9Fu);
    MINOR_MODE_WORD2 = (bua_u8)((MINOR_MODE_WORD2 & 0xFEu) | (mode & 1u));
}
static void bua_battery_service(bua_u8 sample)
{
    BuaBatt86 r;
    bua_u8 sh;
    r=bua_battery86_segment_e(sample,RAM8(0x0033u),RAM8(0x003Eu),
                              mpu16be_get(0x3FC8u),ram16be_get(0x00B2u),
                              RAM8(0x00B4u),RAM8(0x00B7u),RAM8(0x00B8u),
                              RAM8(0x011Fu),RAM8(0x0120u));
    RAM8(0x007Eu)=r.battery_7e;
    RAM8(0x0033u)=r.mode33;
    RAM8(0x003Eu)=r.major3e;
    ram16be_set(0x00B2u,r.old_spark_b2);
    RAM8(0x00B4u)=r.spark_fb_b4;
    RAM8(0x00B7u)=r.burnoff_time_b7;
    RAM8(0x00B8u)=r.burnoff_delay_b8;
    RAM8(0x011Fu)=r.burn_diag_11f;
    RAM8(0x0120u)=r.burn_diag_120;
    if(r.shifter_read!=0u) {
        sh=RAM8(0x0045u);
        RAM8(0x0004u)=bua_prndl78(RAM8(0x0037u),&sh,RAM8(0x0004u),
                                  RAM8(0x0065u),0u);
        RAM8(0x0045u)=sh;
        bua_tcc_after_shifter();
        if (!sim_legacy_drive_trace)
            bua_shift_light_service();
    }
}

static void segE_tcc_adc(void)
{
    bua_battery_service(hw_adc(0x10u));
}

static void seg1_output_bits(void)
{
    BuaBatt86 r;
    bua_u16 air_count;
    bua_u16 enrich_count;
    bua_u16 tcc_count;
    bua_u16 purge_count;
    bua_u16 egr_count;
    bua_u16 all_count;
    bua_u8 a;
    bua_u8 mode4;
    bua_u8 mode4_control;
    bua_u8 mode4_value;
    bua_u8 fan_bit;

    r=bua_battery86_seg1_gate(RAM8(0x007Eu),RAM8(0x0035u),
                               RAM8(0x003Eu),RAM8(0x004Fu));
    RAM8(0x0035u)=r.minor35;
    RAM8(0x003Eu)=r.major3e;
    RAM8(0x004Fu)=r.err4f;
    if(sim_legacy_segment1_output_freeze!=0u) {
        step91_outputs=bua_output_stage91(RAM8(0x003Eu),RAM8(0x0037u),
                                          RAM8(0x0034u),RAM8(0x0113u),
                                          RAM8(0x0112u),RAM8(0x00F4u),
                                          r.force_discretes_off);
        return;
    }

    /* $EDBD..$EDF9: engine-off and high-voltage all-output path. */
    if(r.force_discretes_off!=0u || (RAM8(0x0034u)&0x80u)==0u) {
        if(r.force_discretes_off!=0u)
            all_count=0xD000u;
        else if((RAM8(0x0041u)&0x80u)!=0u)
            all_count=0xDFFFu;
        else
            all_count=0xD000u;
        mpu16be_set(0x3FD2u,all_count);
        mpu16be_set(0x3FD6u,all_count);
        mpu16be_set(0x3FD8u,all_count);
        mpu16be_set(0x3FCCu,all_count);
        mpu16be_set(0x3FD4u,all_count);
        if(all_count==0xD000u)
            mem.io4000[4]=(bua_u8)(mem.io4000[4]&0xFDu);
        else
            mem.io4000[4]=(bua_u8)(mem.io4000[4]|0x02u);
        step91_outputs.air_arc_count=all_count;
        step91_outputs.enrich_count=all_count;
        step91_outputs.tcc_count=all_count;
        step91_outputs.purge_count=all_count;
        step91_outputs.egr_count=all_count;
        step91_outputs.fan_parallel_b1=(bua_u8)((mem.io4000[4]&2u)!=0u);
        step91_outputs.forced_off=r.force_discretes_off;
        return;
    }

    /* $EDFD..$EE4F: AIR and acceleration-enrichment raw counters. */
    mode4=(bua_u8)(RAM8(0x0035u)&0x08u);
    mode4_control=RAM8(0x0156u);
    mode4_value=RAM8(0x0157u);
    if(mode4!=0u && (mode4_control&0x20u)!=0u)
        air_count=(bua_u16)(((bua_u16)(0x3400u|mode4_value)<<2)|3u);
    else if((RAM8(0x003Eu)&0x02u)!=0u)
        air_count=0xDFFFu;
    else
        air_count=0xD000u;
    mpu16be_set(0x3FCCu,air_count);

    if(mode4!=0u && (mode4_control&0x40u)!=0u)
        enrich_count=(bua_u16)(((bua_u16)(0x3400u|mode4_value)<<2)|3u);
    else if((RAM8(0x003Eu)&0x04u)!=0u) /* LC015=$04 */
        enrich_count=0xDFFFu;
    else
        enrich_count=0xD000u;
    mpu16be_set(0x3FD4u,enrich_count);

    /* $EE52..$EE9B: LC017=$00 selects the TCC request, not A/C output. */
    if(mode4!=0u && (mode4_control&0x04u)!=0u)
        tcc_count=(bua_u16)(((bua_u16)(0x3400u|mode4_value)<<2)|3u);
    else if((RAM8(0x0037u)&0x20u)!=0u)
        tcc_count=0xDFFFu;
    else
        tcc_count=0xD000u;
    mpu16be_set(0x3FD6u,tcc_count);

    /* $EEB1 complements the selected duty for both purge sources. */
    purge_count=bua_pwm_count91((mode4!=0u && (mode4_control&0x08u)!=0u)
                                ? mode4_value : RAM8(0x0113u));
    mpu16be_set(0x3FD8u,purge_count);

    /* $EEBB..$EEE3: fan bit is staged at raw parallel-I/O bit 1. */
    a=(bua_u8)(mem.io4000[4]&0xFDu);
    if(mode4!=0u && (RAM8(0x0152u)&0x02u)!=0u)
        a=(bua_u8)(a|(RAM8(0x0153u)&0x02u));
    else if(RAM8(0x00F4u)!=0u && (RAM8(0x0034u)&0x08u)!=0u)
        a=(bua_u8)(a|0x02u);
    mem.io4000[4]=a;
    fan_bit=(bua_u8)((a&0x02u)!=0u);

    /* $EEE6..$EF03: EGR normal duty is complemented; Mode 4 is literal. */
    if(mode4!=0u && (mode4_control&0x01u)!=0u)
        egr_count=(bua_u16)(((bua_u16)(0x3400u|mode4_value)<<2)|3u);
    else
        egr_count=bua_pwm_count91(RAM8(0x0112u));
    mpu16be_set(0x3FD2u,egr_count);

    step91_outputs.air_arc_count=air_count;
    step91_outputs.enrich_count=enrich_count;
    step91_outputs.tcc_count=tcc_count;
    step91_outputs.purge_count=purge_count;
    step91_outputs.egr_count=egr_count;
    step91_outputs.fan_parallel_b1=fan_bit;
    step91_outputs.forced_off=0u;
}

static void seg5_ac_closed_loop_fan(void)
{
    /* L0000 b4 selects the source's alternating fan/A-C versus closed-loop half. */
    if((MINOR_COUNT&0x10u)==0u) {
        bua_u8 timer=RAM8(0x00F5u);
        RAM8(0x00F4u)=bua_fan79((bua_u8)((RAM8(0x0035u)&0x02u)!=0u),
                                RAM8(0x0065u),RAM8(0x0037u),RAM8(0x005Bu),
                                RAM8(0x00F4u),&timer);
        RAM8(0x00F5u)=timer;
        bua_ac_front_tail_ram91();
    } else {
        BuaClEntryResult cl;
        cl=bua_closed_loop_entry_step34((bua_u8)((RAM8(0x0034u)&0x80u)!=0u),
                 RAM8(0x0001u),RAM8(0x0041u),RAM8(0x0035u),RAM8(0x0033u),
                 RAM8(0x004Bu),ram16be_get(0x001Au),RAM8(0x005Fu),
                 RAM8(0x005Bu),RAM8(0x00BEu),RAM8(0x0044u),
                 RAM8(0x0154u),RAM8(0x0155u),RAM8(0x0158u));
        RAM8(0x0044u)=cl.af_mode_word2;
        RAM8(0x0001u)=cl.nv_mode_word;
        RAM8(0x0033u)=cl.minor_mode_word1;
        RAM8(0x00BEu)=cl.o2_not_ready_timer;
        /* LED7B..LED9D follows closed-loop state production and controls
         * the L0044 b1 permission consumed by the 50-ms BLM learner. */
        AF_MODE_WORD2=bua_blm_store_qualify_step35(AF_MODE_WORD2,COOLANT,
                                      ram16be_get(TOTAL_AFR_ADDR),FILTERED_LOAD);
        ++stats.scheduler_blm_store_qualifications;
        /* LDF9D..LE07B is called after the closed-loop qualification tail as well. */
        bua_ac_front_tail_ram91();
    }
}
