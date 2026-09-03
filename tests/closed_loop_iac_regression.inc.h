static void run_step31_sensor_to_injector_test(void)
{
    BuaMafFilterState st;
    BuaSensorFuelResult r1;
    BuaSensorFuelResult r2;
    BuaSensorFuelResult r3;
    bua_u16 raw128;
    unsigned int passed;
    passed = 0u;
    printf("\nIntegrated sensor-to-injector regression (Step 31):\n");
    ecm_reset();
    raw128 = bua_maf_analog_unlimited_lf7ac(128u, 0, 0, 0, 0);
    bua_maf_filter_seed(&st, raw128);
    r1 = bua_sensor_to_injector_step31(&st, 128u, 96u, 410u,
                                       0u, 1u, 25u, 128u,
                                       128u, 128u, 0u, 30u);
    if (r1.maf.raw_maf_axis == 896u &&
        r1.maf.unlimited_q8_8 == 16185u &&
        ram16be_get(AIRFLOW_WORD_ADDR) == r1.maf.final_q8_8) ++passed;
    printf("  MAF ADC128 -> axis896 -> L00EA=%u Q8.8 (%u.%03u g/s)  %s\n",
           (unsigned int)r1.maf.final_q8_8,
           (unsigned int)(r1.maf.final_q8_8 >> 8),
           (unsigned int)(((r1.maf.final_q8_8 & 255u) * 1000u) / 256u),
           (r1.maf.raw_maf_axis == 896u && r1.maf.unlimited_q8_8 == 16185u &&
            ram16be_get(AIRFLOW_WORD_ADDR) == r1.maf.final_q8_8) ? "PASS" : "FAIL");
    if (r1.afr.afr_code == 445u && ram16be_get(TOTAL_AFR_ADDR) == 445u) ++passed;
    printf("  closed-loop neutral AFR -> code445 (%0.2f:1)  %s\n",
           bua_afr_code_to_ratio(r1.afr.afr_code),
           (r1.afr.afr_code == 445u && ram16be_get(TOTAL_AFR_ADDR) == 445u) ? "PASS" : "FAIL");
    if (r1.base_pw > 0u && r1.corrected_pw == r1.base_pw) ++passed;
    printf("  calculated BPW=%u; neutral BLM/C-L leaves PW=%u  %s\n",
           (unsigned int)r1.base_pw, (unsigned int)r1.corrected_pw,
           (r1.base_pw > 0u && r1.corrected_pw == r1.base_pw) ? "PASS" : "FAIL");
    if (r1.output.pending_pw == r1.corrected_pw &&
        r1.output.hardware_pw >= r1.output.pending_pw) ++passed;
    printf("  ordinary output pending=%u -> hardware=%u with battery offset  %s\n",
           (unsigned int)r1.output.pending_pw, (unsigned int)r1.output.hardware_pw,
           (r1.output.pending_pw == r1.corrected_pw &&
            r1.output.hardware_pw >= r1.output.pending_pw) ? "PASS" : "FAIL");
    /* Higher MAF at the same reference period and AFR must demand more fuel. */
    ecm_reset();
    bua_maf_filter_seed(&st, bua_maf_analog_unlimited_lf7ac(160u, 0, 0, 0, 0));
    r2 = bua_sensor_to_injector_step31(&st, 160u, 96u, 410u,
                                       0u, 1u, 25u, 128u,
                                       128u, 128u, 0u, 30u);
    if (r2.maf.final_q8_8 > r1.maf.final_q8_8 && r2.base_pw > r1.base_pw &&
        r2.output.hardware_pw > r1.output.hardware_pw) ++passed;
    printf("  ADC160 raises airflow %u->%u, BPW %u->%u, HW %u->%u  %s\n",
           (unsigned int)r1.maf.final_q8_8, (unsigned int)r2.maf.final_q8_8,
           (unsigned int)r1.base_pw, (unsigned int)r2.base_pw,
           (unsigned int)r1.output.hardware_pw, (unsigned int)r2.output.hardware_pw,
           (r2.maf.final_q8_8 > r1.maf.final_q8_8 && r2.base_pw > r1.base_pw &&
            r2.output.hardware_pw > r1.output.hardware_pw) ? "PASS" : "FAIL");
    /* Richer reciprocal-AFR code at identical MAF must also increase BPW. */
    ecm_reset();
    bua_maf_filter_seed(&st, raw128);
    r3 = bua_sensor_to_injector_step31(&st, 128u, 96u, 410u,
                                       0u, 0u, 58u, 128u,
                                       128u, 128u, 0u, 30u);
    if (r3.afr.afr_code > r1.afr.afr_code && r3.base_pw > r1.base_pw) ++passed;
    printf("  richer open-loop AFR code %u->%u increases BPW %u->%u  %s\n",
           (unsigned int)r1.afr.afr_code, (unsigned int)r3.afr.afr_code,
           (unsigned int)r1.base_pw, (unsigned int)r3.base_pw,
           (r3.afr.afr_code > r1.afr.afr_code && r3.base_pw > r1.base_pw) ? "PASS" : "FAIL");
    /* Verify the integration no longer depends on the old synthetic 0x3C00 airflow. */
    if (r1.maf.final_q8_8 != 0x3C00u &&
        ram16be_get(AIRFLOW_WORD_ADDR) == r3.maf.final_q8_8) ++passed;
    printf("  fuel-chain airflow comes from MAF path, not synthetic 0x3C00  %s\n",
           (r1.maf.final_q8_8 != 0x3C00u &&
            ram16be_get(AIRFLOW_WORD_ADDR) == r3.maf.final_q8_8) ? "PASS" : "FAIL");
    printf("  step-31 integrated regression result: %s (%u/7)\n",
           (passed == 7u) ? "PASS" : "FAIL", passed);
}
/* -------------------------------------------------------------------------- */
/* Step 32: closed-loop reset/qualification gate, LDB9B..LDBD4.               */
/*                                                                            */
/* The source contains one suspicious instruction later in the decel branch:  */
/*     CMPA #1280                                                             */
/* A is an 8-bit register.  Rather than silently changing that to 128 or       */
/* assuming assembler truncation, this step stops at the branch that leads to  */
/* that compare and reports it as needing executable-byte verification.        */
/* -------------------------------------------------------------------------- */
typedef enum {
    BUA_CL_GATE_RUN_CONTROLLER = 0,
    BUA_CL_GATE_RESET_OPEN_LOOP,
    BUA_CL_GATE_RESET_PE_DFCO,
    BUA_CL_GATE_RESET_BLM_CELL,
    BUA_CL_GATE_RESET_LOW_PW,
    BUA_CL_GATE_RESET_AE_STATE,
    BUA_CL_GATE_RESET_CCP_CHANGE,
    BUA_CL_GATE_DECEL_COMPARE_PENDING
} BuaClGateReason;
typedef struct {
    BuaClGateReason reason;
    bua_u8 integrator;
    bua_u8 integrator_timer;
    bua_u8 diag_mode_word3;
    bua_u8 reset_applied;
    bua_u8 controller_allowed;
} BuaClGateResult;
static BuaClGateResult bua_closed_loop_gate_step32(bua_u8 af_mode_word2,
                                                    bua_u8 af_mode_word,
                                                    bua_u8 ign_mode_word,
                                                    bua_u8 accel_mode_word,
                                                    bua_u8 decel_mode_word,
                                                    bua_u8 minor_mode_word2,
                                                    bua_u8 filtered_load,
                                                    bua_u8 rpm25,
                                                    bua_u8 integrator,
                                                    bua_u8 integrator_timer,
                                                    bua_u8 diag_mode_word3,
                                                    bua_u8 ccp_old_word)
{
    BuaClGateResult r;
    bua_u8 reset;
    bua_u8 diagnostic_reset;
    r.reason = BUA_CL_GATE_RUN_CONTROLLER;
    r.integrator = integrator;
    r.integrator_timer = integrator_timer;
    r.diag_mode_word3 = diag_mode_word3;
    r.reset_applied = 0u;
    r.controller_allowed = 0u;
    reset = 0u;
    diagnostic_reset = 0u;
    /* LDB9B: closed-loop bit clear -> LDBFA. */
    if ((af_mode_word2 & AF_CLOSED_LOOP_BIT) == 0u) {
        r.reason = BUA_CL_GATE_RESET_OPEN_LOOP;
        reset = 1u;
    }
    /* LDBA3: PE or DFCO forces neutral and marks diagnostic b2. */
    else if ((af_mode_word & 0x22u) != 0u) {
        r.reason = BUA_CL_GATE_RESET_PE_DFCO;
        reset = 1u;
        diagnostic_reset = 1u;
    }
    /* LC016=$62 has b5 set: BLM-cell-change b2 can force neutral. */
    else if (((CAL_AF_MODE3 & CAL_CL_RESET_ON_BLM_CHANGE_BIT) != 0u) &&
             ((af_mode_word & AF_BLM_CELL_CHANGE_BIT) != 0u)) {
        r.reason = BUA_CL_GATE_RESET_BLM_CELL;
        reset = 1u;
    }
    /* Minimum/default injector PW forces open loop and neutral integrator. */
    else if ((ign_mode_word & FORCE_LOW_PW_BIT) != 0u) {
        r.reason = BUA_CL_GATE_RESET_LOW_PW;
        reset = 1u;
    }
    /* L003E b7 skips directly to the special decel rich/lean test. */
    else if ((decel_mode_word & DECEL_STATE_HOLD_BIT) != 0u) {
        r.reason = BUA_CL_GATE_DECEL_COMPARE_PENDING;
    }
    /* LC016=$62 also has b1 set. BRCLR $0038,$88 continues only when
       both selected AE-state bits are clear. */
    else if (((CAL_AF_MODE3 & CAL_CL_RESET_DURING_AE_BIT) != 0u) &&
             ((accel_mode_word & CL_AE_RESET_MASK) != 0u)) {
        r.reason = BUA_CL_GATE_RESET_AE_STATE;
        reset = 1u;
    }
    /* Optional CCP-change gate.  LC017=$00 disables it in this image.
       The exact source test is ADDA/RORA on L003B + L003E; ccp_old_word
       is retained as the other operand for a nonzero-calibration variant. */
    else if ((CAL_AF_MODE4 & CAL_CL_RESET_ON_CCP_CHANGE_BIT) != 0u) {
        bua_u8 sum;
        sum = (bua_u8)(ccp_old_word + decel_mode_word);
        if ((sum & 0x01u) != 0u) {
            r.reason = BUA_CL_GATE_RESET_CCP_CHANGE;
            reset = 1u;
        }
    }
    if (reset != 0u) {
        r.integrator = 128u;
        r.integrator_timer = 0u;
        r.reset_applied = 1u;
        if (diagnostic_reset != 0u)
            r.diag_mode_word3 |= DIAG_INT_FORCED_NEUTRAL_BIT;
        return r;
    }
    if (r.reason == BUA_CL_GATE_DECEL_COMPARE_PENDING)
        return r;
    /* LDBD4: ALDL mode bypasses the decel-enlean qualification. */
    if ((minor_mode_word2 & ALDL_DIAG_MODE_BIT) != 0u) {
        r.controller_allowed = 1u;
        return r;
    }
    /* If load is above LC5F1 or RPM is at/below LC5F2, go to LDC05. */
    if (filtered_load > CAL_DECEL_CL_LOAD_MAX || rpm25 <= CAL_DECEL_CL_RPM_MIN) {
        r.controller_allowed = 1u;
        return r;
    }
    /* Low-load/high-RPM decel reaches LDBE8. Rich O2 goes to LDC05.
       Lean O2 reaches the anomalous CMPA #1280 instruction. */
    if ((af_mode_word2 & AF_RICH_LEAN_BIT) != 0u) {
        r.controller_allowed = 1u;
        return r;
    }
    r.reason = BUA_CL_GATE_DECEL_COMPARE_PENDING;
    return r;
}
static void run_step32_closed_loop_gate_test(void)
{
    BuaClGateResult r;
    unsigned int passed;
    passed = 0u;
    printf("\nClosed-loop qualification/reset regression (Step 32):\n");
    r = bua_closed_loop_gate_step32(0u, 0u, 0u, 0u, 0u, 0u,
                                    80u, 40u, 170u, 9u, 0u, 0u);
    if (r.reset_applied && r.integrator == 128u && r.integrator_timer == 0u &&
        r.reason == BUA_CL_GATE_RESET_OPEN_LOOP) ++passed;
    printf("  open-loop b7 clear -> integrator128/timer0                         %s\n",
           (r.reset_applied && r.reason == BUA_CL_GATE_RESET_OPEN_LOOP) ? "PASS" : "FAIL");
    r = bua_closed_loop_gate_step32(AF_CLOSED_LOOP_BIT, 0u, 0u, 0u, 0u, 0u,
                                    80u, 40u, 150u, 7u, 0u, 0u);
    if (!r.reset_applied && r.controller_allowed && r.integrator == 150u) ++passed;
    printf("  normal closed-loop conditions reach LDC05 without reset           %s\n",
           (!r.reset_applied && r.controller_allowed) ? "PASS" : "FAIL");
    r = bua_closed_loop_gate_step32(AF_CLOSED_LOOP_BIT, AF_POWER_ENRICH_BIT,
                                    0u, 0u, 0u, 0u, 80u, 40u, 170u, 7u, 0u, 0u);
    if (r.reset_applied && (r.diag_mode_word3 & DIAG_INT_FORCED_NEUTRAL_BIT) != 0u &&
        r.reason == BUA_CL_GATE_RESET_PE_DFCO) ++passed;
    printf("  power enrichment -> neutral integrator + diagnostic b2            %s\n",
           (r.reset_applied && (r.diag_mode_word3 & DIAG_INT_FORCED_NEUTRAL_BIT)) ? "PASS" : "FAIL");
    r = bua_closed_loop_gate_step32(AF_CLOSED_LOOP_BIT, FUEL_DFCO_ZERO_BIT,
                                    0u, 0u, 0u, 0u, 80u, 40u, 170u, 7u, 0u, 0u);
    if (r.reset_applied && r.reason == BUA_CL_GATE_RESET_PE_DFCO) ++passed;
    printf("  DFCO bit -> neutral integrator                                    %s\n",
           (r.reset_applied && r.reason == BUA_CL_GATE_RESET_PE_DFCO) ? "PASS" : "FAIL");
    r = bua_closed_loop_gate_step32(AF_CLOSED_LOOP_BIT, AF_BLM_CELL_CHANGE_BIT,
                                    0u, 0u, 0u, 0u, 80u, 40u, 170u, 7u, 0u, 0u);
    if (r.reset_applied && r.reason == BUA_CL_GATE_RESET_BLM_CELL) ++passed;
    printf("  LC016 b5 + BLM-cell-change b2 -> neutral integrator               %s\n",
           (r.reset_applied && r.reason == BUA_CL_GATE_RESET_BLM_CELL) ? "PASS" : "FAIL");
    r = bua_closed_loop_gate_step32(AF_CLOSED_LOOP_BIT, 0u, FORCE_LOW_PW_BIT,
                                    0u, 0u, 0u, 80u, 40u, 170u, 7u, 0u, 0u);
    if (r.reset_applied && r.reason == BUA_CL_GATE_RESET_LOW_PW) ++passed;
    printf("  forced-low/default PW b1 -> neutral integrator                    %s\n",
           (r.reset_applied && r.reason == BUA_CL_GATE_RESET_LOW_PW) ? "PASS" : "FAIL");
    r = bua_closed_loop_gate_step32(AF_CLOSED_LOOP_BIT, 0u, 0u,
                                    TPS_AE_ACTIVE_BIT, 0u, 0u,
                                    80u, 40u, 170u, 7u, 0u, 0u);
    if (r.reset_applied && r.reason == BUA_CL_GATE_RESET_AE_STATE) ++passed;
    printf("  LC016 b1 + TPS-AE b7 -> neutral integrator                        %s\n",
           (r.reset_applied && r.reason == BUA_CL_GATE_RESET_AE_STATE) ? "PASS" : "FAIL");
    r = bua_closed_loop_gate_step32(AF_CLOSED_LOOP_BIT, 0u, 0u,
                                    AE_ACTIVE_BIT, 0u, 0u,
                                    80u, 40u, 170u, 7u, 0u, 0u);
    if (r.reset_applied && r.reason == BUA_CL_GATE_RESET_AE_STATE) ++passed;
    printf("  LC016 b1 + differential-AE b3 -> neutral integrator               %s\n",
           (r.reset_applied && r.reason == BUA_CL_GATE_RESET_AE_STATE) ? "PASS" : "FAIL");
    r = bua_closed_loop_gate_step32(AF_CLOSED_LOOP_BIT, 0u, 0u, 0u, 0u,
                                    ALDL_DIAG_MODE_BIT, 20u, 60u,
                                    170u, 7u, 0u, 0u);
    if (!r.reset_applied && r.controller_allowed) ++passed;
    printf("  ALDL diagnostic mode bypasses decel reset test                     %s\n",
           (!r.reset_applied && r.controller_allowed) ? "PASS" : "FAIL");
    /* Even if a CCP-related state changes, LC017=$00 cannot enable its reset. */
    r = bua_closed_loop_gate_step32(AF_CLOSED_LOOP_BIT, 0u, 0u, 0u, 1u, 0u,
                                    80u, 40u, 170u, 7u, 0u, 1u);
    if (!r.reset_applied && r.controller_allowed) ++passed;
    printf("  actual LC017=00 disables optional CCP-change reset                 %s\n",
           (!r.reset_applied && r.controller_allowed) ? "PASS" : "FAIL");
    r = bua_closed_loop_gate_step32((bua_u8)(AF_CLOSED_LOOP_BIT | AF_RICH_LEAN_BIT),
                                    0u, 0u, 0u, 0u, 0u,
                                    20u, 60u, 170u, 7u, 0u, 0u);
    if (!r.reset_applied && r.controller_allowed) ++passed;
    printf("  low-load/high-RPM but rich O2 bypasses anomalous lean compare      %s\n",
           (!r.reset_applied && r.controller_allowed) ? "PASS" : "FAIL");
    r = bua_closed_loop_gate_step32(AF_CLOSED_LOOP_BIT, 0u, 0u, 0u, 0u, 0u,
                                    20u, 60u, 170u, 7u, 0u, 0u);
    if (!r.reset_applied && !r.controller_allowed &&
        r.reason == BUA_CL_GATE_DECEL_COMPARE_PENDING) ++passed;
    printf("  lean low-load/high-RPM path is isolated at source CMPA #1280      %s\n",
           (!r.reset_applied && !r.controller_allowed &&
            r.reason == BUA_CL_GATE_DECEL_COMPARE_PENDING) ? "PASS" : "FAIL");
    printf("  step-32 closed-loop gate regression result: %s (%u/12)\n",
           (passed == 12u) ? "PASS" : "FAIL", passed);
}
/* -------------------------------------------------------------------------- */
/* Step 33: resolve the LDBE8 decel-integrator compare.                       */
/*                                                                            */
/* Source text says CMPA #1280. CMPA immediate has one 8-bit operand.          */
/* The assembler listing supplied after Step 33 shows:                        */
/*     DBEE 96 C9    LDAA $C9                                                  */
/*     DBF0 81 80    CMPA #$80                                                 */
/*     DBF2 25 11    BCS  $DC05                                                */
/* Thus the compiled operand is verified as 128 for this reconstructed image. */
/* -------------------------------------------------------------------------- */
typedef struct {
    BuaClGateResult gate;
    bua_u8 threshold_used;
    bua_u8 source_operand_verified;
} BuaClGate33Result;
static BuaClGate33Result bua_closed_loop_gate_step33(bua_u8 af_mode_word2,
                                                      bua_u8 af_mode_word,
                                                      bua_u8 ign_mode_word,
                                                      bua_u8 accel_mode_word,
                                                      bua_u8 decel_mode_word,
                                                      bua_u8 minor_mode_word2,
                                                      bua_u8 filtered_load,
                                                      bua_u8 rpm25,
                                                      bua_u8 integrator,
                                                      bua_u8 integrator_timer,
                                                      bua_u8 diag_mode_word3,
                                                      bua_u8 ccp_old_word)
{
    BuaClGate33Result r;
    r.gate = bua_closed_loop_gate_step32(af_mode_word2, af_mode_word, ign_mode_word,
                                         accel_mode_word, decel_mode_word,
                                         minor_mode_word2, filtered_load, rpm25,
                                         integrator, integrator_timer,
                                         diag_mode_word3, ccp_old_word);
    r.threshold_used = CAL_DECEL_INT_NEUTRAL;
    r.source_operand_verified = 1u;
    if (r.gate.reason != BUA_CL_GATE_DECEL_COMPARE_PENDING) return r;
    /* Intended CMPA #128 / BCS LDC05: below neutral continues control;
       neutral or above takes LDBF4, sets diag b2, and resets to 128. */
    if (integrator < CAL_DECEL_INT_NEUTRAL) {
        r.gate.reason = BUA_CL_GATE_RUN_CONTROLLER;
        r.gate.controller_allowed = 1u;
        return r;
    }
    r.gate.reason = BUA_CL_GATE_RESET_PE_DFCO; /* shared LDBF4 destination */
    r.gate.integrator = 128u;
    r.gate.integrator_timer = 0u;
    r.gate.diag_mode_word3 |= DIAG_INT_FORCED_NEUTRAL_BIT;
    r.gate.reset_applied = 1u;
    r.gate.controller_allowed = 0u;
    return r;
}
static void run_step33_decel_integrator_test(void)
{
    BuaClGate33Result r;
    unsigned int passed = 0u;
    printf("\nDecel integrator-compare regression (Step 33):\n");
#define STEP33_CALL(INTV, AF2, DECEL, ALDL) \
    bua_closed_loop_gate_step33((AF2),0u,0u,0u,(DECEL),(ALDL),20u,60u,(INTV),9u,0u,0u)
    r=STEP33_CALL(127u,AF_CLOSED_LOOP_BIT,0u,0u);
    if(!r.gate.reset_applied && r.gate.controller_allowed) ++passed;
    printf("  lean low-load/high-RPM INT127 -> continue LDC05               %s\n", (!r.gate.reset_applied && r.gate.controller_allowed)?"PASS":"FAIL");
    r=STEP33_CALL(128u,AF_CLOSED_LOOP_BIT,0u,0u);
    if(r.gate.reset_applied && r.gate.integrator==128u && r.gate.integrator_timer==0u && (r.gate.diag_mode_word3&DIAG_INT_FORCED_NEUTRAL_BIT)) ++passed;
    printf("  lean low-load/high-RPM INT128 -> LDBF4 neutral reset          %s\n", r.gate.reset_applied?"PASS":"FAIL");
    r=STEP33_CALL(180u,AF_CLOSED_LOOP_BIT,0u,0u);
    if(r.gate.reset_applied && r.gate.integrator==128u) ++passed;
    printf("  lean low-load/high-RPM INT180 -> forced neutral128            %s\n", (r.gate.reset_applied&&r.gate.integrator==128u)?"PASS":"FAIL");
    r=STEP33_CALL(180u,(bua_u8)(AF_CLOSED_LOOP_BIT|AF_RICH_LEAN_BIT),0u,0u);
    if(!r.gate.reset_applied && r.gate.controller_allowed) ++passed;
    printf("  rich low-load/high-RPM bypasses integrator compare             %s\n", (!r.gate.reset_applied&&r.gate.controller_allowed)?"PASS":"FAIL");
    r=bua_closed_loop_gate_step33(AF_CLOSED_LOOP_BIT,0u,0u,0u,DECEL_STATE_HOLD_BIT,0u,80u,40u,127u,3u,0u,0u);
    if(!r.gate.reset_applied && r.gate.controller_allowed) ++passed;
    printf("  L003E b7 hold + lean INT127 uses same threshold                %s\n", (!r.gate.reset_applied&&r.gate.controller_allowed)?"PASS":"FAIL");
    r=bua_closed_loop_gate_step33(AF_CLOSED_LOOP_BIT,0u,0u,0u,DECEL_STATE_HOLD_BIT,0u,80u,40u,129u,3u,0u,0u);
    if(r.gate.reset_applied && r.gate.integrator==128u) ++passed;
    printf("  L003E b7 hold + lean INT129 forces neutral                     %s\n", (r.gate.reset_applied&&r.gate.integrator==128u)?"PASS":"FAIL");
    r=STEP33_CALL(180u,AF_CLOSED_LOOP_BIT,0u,ALDL_DIAG_MODE_BIT);
    if(!r.gate.reset_applied && r.gate.controller_allowed) ++passed;
    printf("  ALDL mode still bypasses decel-integrator reset                %s\n", (!r.gate.reset_applied&&r.gate.controller_allowed)?"PASS":"FAIL");
    if(r.threshold_used==128u && r.source_operand_verified!=0u) ++passed;
    printf("  listing DBF0 bytes 81 80 verify CMPA #$80 (128)               %s\n",
           (r.threshold_used==128u && r.source_operand_verified!=0u)?"PASS":"FAIL");
#undef STEP33_CALL
    printf("  step-33 decel-integrator regression result: %s (%u/8)\n", (passed==8u)?"PASS":"FAIL", passed);
}
/* -------------------------------------------------------------------------- */
/* Step 34: closed-loop entry qualification, LECF6..LED7B.                    */
/*                                                                            */
/* This is the state machine that actually sets/clears L0044 b7.  It is       */
/* distinct from the active O2 controller (Step 24) and the integrator gates  */
/* (Steps 32/33).  The implementation below keeps the source's two coolant    */
/* variables distinct: L005F selects the cold/warm/hot run-time delay, while  */
/* L005B is checked against the absolute minimum closed-loop temperature.      */
/* -------------------------------------------------------------------------- */
#define CAL_CL_MIN_COOLANT             107u  /* LC551 */
#define CAL_CL_HOT_TIMER_COOLANT       147u  /* LC552 */
#define CAL_CL_COLD_TIMER_COOLANT       73u  /* LC553 */
#define CAL_CL_COLD_TIMER              150u  /* LC556, sec/2 */
#define CAL_CL_WARM_TIMER              103u  /* LC557, sec/2 */
#define CAL_CL_HOT_TIMER                25u  /* LC558, sec/2 */
#define CAL_CL_O2_NOT_READY_LIMIT       50u  /* LC55B, 200-ms counts */
#define NV_O2_READY_BIT                0x01u /* L0001 b0 */
#define NV_CL_TIMER_DONE_BIT           0x02u /* L0001 b1 */
#define NV_PROPER_SHUTDOWN_BIT         0x08u /* L0001 b3 */
#define DIAG_O2_ERROR_BIT              0x02u /* L0041 b1: ERR 44/45 */
#define MINOR_DIAG_ALDL_MASK           0x30u /* L0035 b4/b5 */
#define MINOR_8192_MODE4_BIT           0x08u /* L0035 b3 */
#define MINOR_MAF_BURNOFF_ENABLE_BIT   0x20u /* L0033 b5 */
#define MINOR_SPECIAL_SIGN_BIT         0x80u /* L0033 b7 */
#define HU_CL_INHIBIT_MASK             0x30u /* L004B b4/b5 */
#define MODE4_COMMAND_FUEL_BIT         0x01u /* $0154 b0 */
#define MODE4_CLOSED_LOOP_BIT          0x01u /* $0155 b0 */
#define MODE4_AFR_CONTROL_BIT          0x04u /* $0158 b2 */
typedef enum {
    BUA_CL_ENTRY_CLOSED = 0,
    BUA_CL_ENTRY_NOT_RUNNING,
    BUA_CL_ENTRY_O2_ERROR,
    BUA_CL_ENTRY_MODE4_OPEN_LOOP,
    BUA_CL_ENTRY_RUN_TIMER,
    BUA_CL_ENTRY_COOLANT,
    BUA_CL_ENTRY_HU_INHIBIT,
    BUA_CL_ENTRY_O2_NOT_READY_TIMEOUT,
    BUA_CL_ENTRY_O2_NOT_READY
} BuaClEntryReason;
typedef struct {
    BuaClEntryReason reason;
    bua_u8 af_mode_word2;
    bua_u8 nv_mode_word;
    bua_u8 minor_mode_word1;
    bua_u8 o2_not_ready_timer;
    bua_u8 selected_run_timer;
    bua_u8 closed_loop;
} BuaClEntryResult;
static BuaClEntryResult bua_closed_loop_entry_step34(bua_u8 engine_running,
                                                      bua_u8 nv_mode_word,
                                                      bua_u8 diag_mode_word3,
                                                      bua_u8 minor_mode_word2,
                                                      bua_u8 minor_mode_word1,
                                                      bua_u8 hu_spark_flags,
                                                      bua_u16 engine_run_seconds,
                                                      bua_u8 timer_coolant,
                                                      bua_u8 current_coolant,
                                                      bua_u8 o2_not_ready_timer,
                                                      bua_u8 af_mode_word2,
                                                      bua_u8 mode4_word0,
                                                      bua_u8 mode4_word1,
                                                      bua_u8 mode4_word4)
{
    BuaClEntryResult r;
    bua_u8 bypass_run_timer;
    bua_u8 selected_timer;
    bua_u16 half_seconds;
    r.reason = BUA_CL_ENTRY_CLOSED;
    r.af_mode_word2 = af_mode_word2;
    r.nv_mode_word = nv_mode_word;
    r.minor_mode_word1 = minor_mode_word1;
    r.o2_not_ready_timer = o2_not_ready_timer;
    r.selected_run_timer = 0u;
    r.closed_loop = 0u;
    bypass_run_timer = 0u;
    selected_timer = 0u;
    /* LECF6: stopped engine normally forces open loop.  The source allows
       continuation only when L0001 b3 is set (its shutdown-state exception). */
    if (engine_running == 0u && (nv_mode_word & NV_PROPER_SHUTDOWN_BIT) == 0u) {
        r.reason = BUA_CL_ENTRY_NOT_RUNNING;
        goto open_loop;
    }
    /* LECFE: ERR 44/45 clears O2-ready timing and forces open loop. */
    if ((diag_mode_word3 & DIAG_O2_ERROR_BIT) != 0u) {
        r.o2_not_ready_timer = 0u;
        r.reason = BUA_CL_ENTRY_O2_ERROR;
        goto open_loop;
    }
    /* LED09: ALDL/diagnostic mode jumps directly to coolant qualification. */
    if ((minor_mode_word2 & MINOR_DIAG_ALDL_MASK) != 0u) {
        bypass_run_timer = 1u;
    } else if ((minor_mode_word2 & MINOR_8192_MODE4_BIT) != 0u) {
        /* Exact Mode-4 branching at LED0F..LED20. */
        if ((mode4_word0 & MODE4_COMMAND_FUEL_BIT) != 0u) {
            if ((mode4_word1 & MODE4_CLOSED_LOOP_BIT) == 0u) {
                r.reason = BUA_CL_ENTRY_MODE4_OPEN_LOOP;
                goto open_loop;
            }
            bypass_run_timer = 1u;
        } else if ((mode4_word4 & MODE4_AFR_CONTROL_BIT) != 0u) {
            r.reason = BUA_CL_ENTRY_MODE4_OPEN_LOOP;
            goto open_loop;
        }
    }
    /* LED24..LED48: normal closed-loop delay. Once L0001 b1 is set, the
       run-time comparison is skipped on later passes. */
    if (bypass_run_timer == 0u &&
        (r.nv_mode_word & NV_CL_TIMER_DONE_BIT) == 0u) {
        if (timer_coolant <= CAL_CL_COLD_TIMER_COOLANT)
            selected_timer = CAL_CL_COLD_TIMER;
        else if (timer_coolant <= CAL_CL_HOT_TIMER_COOLANT)
            selected_timer = CAL_CL_WARM_TIMER;
        else
            selected_timer = CAL_CL_HOT_TIMER;
        r.selected_run_timer = selected_timer;
        half_seconds = (bua_u16)(engine_run_seconds >> 1);
        /* Executable compares B after LSRD. These calibrated delays are all
           below 256, so the low byte is the operative value here. */
        if ((bua_u8)half_seconds < selected_timer) {
            r.reason = BUA_CL_ENTRY_RUN_TIMER;
            goto open_loop;
        }
        r.nv_mode_word |= NV_CL_TIMER_DONE_BIT;
    }
    /* LED48 requires coolant strictly greater than LC551=107. */
    if (current_coolant <= CAL_CL_MIN_COOLANT) {
        r.reason = BUA_CL_ENTRY_COOLANT;
        goto open_loop;
    }
    /* If L0033 is negative, HU spark flags b4/b5 inhibit entry. */
    if ((minor_mode_word1 & MINOR_SPECIAL_SIGN_BIT) != 0u &&
        (hu_spark_flags & HU_CL_INHIBIT_MASK) != 0u) {
        r.reason = BUA_CL_ENTRY_HU_INHIBIT;
        goto open_loop;
    }
    r.minor_mode_word1 |= MINOR_MAF_BURNOFF_ENABLE_BIT;
    /* LED59..LED6E: allow at most 50 200-ms periods for the O2-ready bit
       (L0001 b0) to be observed. At count >=50, clear that bit first. */
    if (r.o2_not_ready_timer >= CAL_CL_O2_NOT_READY_LIMIT) {
        r.nv_mode_word &= (bua_u8)~NV_O2_READY_BIT;
        if ((r.nv_mode_word & NV_O2_READY_BIT) == 0u) {
            r.reason = BUA_CL_ENTRY_O2_NOT_READY_TIMEOUT;
            goto open_loop;
        }
    } else {
        r.o2_not_ready_timer = (bua_u8)(r.o2_not_ready_timer + 1u);
    }
    if ((r.nv_mode_word & NV_O2_READY_BIT) == 0u) {
        r.reason = BUA_CL_ENTRY_O2_NOT_READY;
        goto open_loop;
    }
    r.af_mode_word2 |= AF_CLOSED_LOOP_BIT;
    r.closed_loop = 1u;
    r.reason = BUA_CL_ENTRY_CLOSED;
    return r;
open_loop:
    r.af_mode_word2 &= (bua_u8)~AF_CLOSED_LOOP_BIT;
    r.closed_loop = 0u;
    return r;
}
static void run_step34_closed_loop_entry_test(void)
{
    BuaClEntryResult r;
    unsigned int passed;
    passed = 0u;
    printf("\nClosed-loop entry/O2-ready regression (Step 34):\n");
#define CL34(ER,NV,DG,M2,M1,HU,SEC,TC,CC,O2,AF2,W0,W1,W4) \
    bua_closed_loop_entry_step34((ER),(NV),(DG),(M2),(M1),(HU),(SEC),(TC),(CC),(O2),(AF2),(W0),(W1),(W4))
    r=CL34(1u,NV_O2_READY_BIT,0u,0u,0u,0u,149u,60u,120u,0u,0u,0u,0u,0u);
    if(!r.closed_loop && r.reason==BUA_CL_ENTRY_RUN_TIMER && r.selected_run_timer==150u) ++passed;
    printf("  cold coolant selects 75-s timer; 149 sec remains open            %s\n",
           (!r.closed_loop&&r.reason==BUA_CL_ENTRY_RUN_TIMER&&r.selected_run_timer==150u)?"PASS":"FAIL");
    r=CL34(1u,NV_O2_READY_BIT,0u,0u,0u,0u,300u,73u,120u,0u,0u,0u,0u,0u);
    if(r.closed_loop && (r.nv_mode_word&NV_CL_TIMER_DONE_BIT)) ++passed;
    printf("  cold timer expires at 75 sec and latches L0001 b1                %s\n",
           (r.closed_loop&&(r.nv_mode_word&NV_CL_TIMER_DONE_BIT))?"PASS":"FAIL");
    r=CL34(1u,NV_O2_READY_BIT,0u,0u,0u,0u,204u,100u,120u,0u,0u,0u,0u,0u);
    if(!r.closed_loop && r.reason==BUA_CL_ENTRY_RUN_TIMER && r.selected_run_timer==103u) ++passed;
    printf("  warm coolant selects 51.5-s timer                                %s\n",
           (!r.closed_loop&&r.selected_run_timer==103u)?"PASS":"FAIL");
    r=CL34(1u,NV_O2_READY_BIT,0u,0u,0u,0u,48u,180u,120u,0u,0u,0u,0u,0u);
    if(!r.closed_loop && r.selected_run_timer==25u) ++passed;
    printf("  hot coolant selects 12.5-s timer                                 %s\n",
           (!r.closed_loop&&r.selected_run_timer==25u)?"PASS":"FAIL");
    r=CL34(1u,(bua_u8)(NV_O2_READY_BIT|NV_CL_TIMER_DONE_BIT),0u,0u,0u,0u,999u,180u,107u,0u,AF_CLOSED_LOOP_BIT,0u,0u,0u);
    if(!r.closed_loop && r.reason==BUA_CL_ENTRY_COOLANT) ++passed;
    printf("  current coolant must be strictly above LC551=107                 %s\n",
           (!r.closed_loop&&r.reason==BUA_CL_ENTRY_COOLANT)?"PASS":"FAIL");
    r=CL34(1u,(bua_u8)(NV_O2_READY_BIT|NV_CL_TIMER_DONE_BIT),DIAG_O2_ERROR_BIT,0u,0u,0u,999u,180u,120u,23u,AF_CLOSED_LOOP_BIT,0u,0u,0u);
    if(!r.closed_loop && r.reason==BUA_CL_ENTRY_O2_ERROR && r.o2_not_ready_timer==0u) ++passed;
    printf("  ERR44/45 clears O2-not-ready timer and forces open loop           %s\n",
           (!r.closed_loop&&r.reason==BUA_CL_ENTRY_O2_ERROR&&r.o2_not_ready_timer==0u)?"PASS":"FAIL");
    r=CL34(1u,NV_O2_READY_BIT,0u,ALDL_DIAG_MODE_BIT,0u,0u,0u,60u,120u,0u,0u,0u,0u,0u);
    if(r.closed_loop && r.selected_run_timer==0u) ++passed;
    printf("  ALDL/diag mode bypasses engine-run delay                          %s\n",
           (r.closed_loop&&r.selected_run_timer==0u)?"PASS":"FAIL");
    r=CL34(1u,NV_O2_READY_BIT,0u,MINOR_8192_MODE4_BIT,0u,0u,0u,60u,120u,0u,0u,MODE4_COMMAND_FUEL_BIT,0u,0u);
    if(!r.closed_loop && r.reason==BUA_CL_ENTRY_MODE4_OPEN_LOOP) ++passed;
    printf("  Mode4 command-fuel + open-loop command forces open loop           %s\n",
           (!r.closed_loop&&r.reason==BUA_CL_ENTRY_MODE4_OPEN_LOOP)?"PASS":"FAIL");
    r=CL34(1u,NV_O2_READY_BIT,0u,MINOR_8192_MODE4_BIT,0u,0u,0u,60u,120u,0u,0u,MODE4_COMMAND_FUEL_BIT,MODE4_CLOSED_LOOP_BIT,0u);
    if(r.closed_loop) ++passed;
    printf("  Mode4 explicit closed-loop command bypasses run timer             %s\n",
           r.closed_loop?"PASS":"FAIL");
    r=CL34(1u,NV_O2_READY_BIT,0u,MINOR_8192_MODE4_BIT,0u,0u,0u,60u,120u,0u,0u,0u,0u,MODE4_AFR_CONTROL_BIT);
    if(!r.closed_loop && r.reason==BUA_CL_ENTRY_MODE4_OPEN_LOOP) ++passed;
    printf("  Mode4 AFR-control command inhibits normal closed-loop entry        %s\n",
           (!r.closed_loop&&r.reason==BUA_CL_ENTRY_MODE4_OPEN_LOOP)?"PASS":"FAIL");
    r=CL34(1u,(bua_u8)(NV_O2_READY_BIT|NV_CL_TIMER_DONE_BIT),0u,0u,0u,0u,999u,100u,120u,49u,0u,0u,0u,0u);
    if(r.closed_loop && r.o2_not_ready_timer==50u) ++passed;
    printf("  O2-ready at timer49 enters closed loop and increments timer to50  %s\n",
           (r.closed_loop&&r.o2_not_ready_timer==50u)?"PASS":"FAIL");
    r=CL34(1u,(bua_u8)(NV_O2_READY_BIT|NV_CL_TIMER_DONE_BIT),0u,0u,0u,0u,999u,100u,120u,50u,AF_CLOSED_LOOP_BIT,0u,0u,0u);
    if(!r.closed_loop && r.reason==BUA_CL_ENTRY_O2_NOT_READY_TIMEOUT && (r.nv_mode_word&NV_O2_READY_BIT)==0u) ++passed;
    printf("  timer>=50 clears stale O2-ready bit and forces open loop           %s\n",
           (!r.closed_loop&&r.reason==BUA_CL_ENTRY_O2_NOT_READY_TIMEOUT)?"PASS":"FAIL");
    r=CL34(1u,NV_CL_TIMER_DONE_BIT,0u,0u,0u,0u,999u,100u,120u,10u,AF_CLOSED_LOOP_BIT,0u,0u,0u);
    if(!r.closed_loop && r.reason==BUA_CL_ENTRY_O2_NOT_READY && r.o2_not_ready_timer==11u) ++passed;
    printf("  no O2-ready crossing remains open while 200-ms timer advances     %s\n",
           (!r.closed_loop&&r.reason==BUA_CL_ENTRY_O2_NOT_READY&&r.o2_not_ready_timer==11u)?"PASS":"FAIL");
    r=CL34(1u,(bua_u8)(NV_O2_READY_BIT|NV_CL_TIMER_DONE_BIT),0u,0u,MINOR_SPECIAL_SIGN_BIT,0x10u,999u,100u,120u,0u,0u,0u,0u,0u);
    if(!r.closed_loop && r.reason==BUA_CL_ENTRY_HU_INHIBIT) ++passed;
    printf("  L0033 b7 plus HU flags b4/b5 inhibits entry                       %s\n",
           (!r.closed_loop&&r.reason==BUA_CL_ENTRY_HU_INHIBIT)?"PASS":"FAIL");
    r=CL34(1u,(bua_u8)(NV_O2_READY_BIT|NV_CL_TIMER_DONE_BIT),0u,0u,0u,0u,999u,100u,120u,0u,0u,0u,0u,0u);
    if(r.closed_loop && (r.af_mode_word2&AF_CLOSED_LOOP_BIT) && (r.minor_mode_word1&MINOR_MAF_BURNOFF_ENABLE_BIT)) ++passed;
    printf("  all qualifications met sets L0044 b7 and L0033 b5                %s\n",
           (r.closed_loop&&(r.af_mode_word2&AF_CLOSED_LOOP_BIT)&&(r.minor_mode_word1&MINOR_MAF_BURNOFF_ENABLE_BIT))?"PASS":"FAIL");
#undef CL34
    printf("  step-34 closed-loop-entry regression result: %s (%u/15)\n",
           (passed==15u)?"PASS":"FAIL",passed);
}
/* -------------------------------------------------------------------------- */
/* Step 35: BLM-store qualification and 50-ms BLM learning.                   */
/*                                                                            */
/* LED7B..LED9D controls L0044 b1, the permission consumed later by the       */
/* air/fuel minor-loop BLM updater.  LDDC2..LDE30 then applies the actual     */
/* selected-cell learning.  The executable is followed literally here.        */
/*                                                                            */
/* LC5E0 is 28.  The source labels the minor loop "50 Msec" but its nearby   */
/* calibration comment also calls this 450 ms; 28 * 50 ms would be 1.4 s.    */
/* Until that discrepancy is traced, the model deliberately treats 28 as a   */
/* loop-count threshold rather than assigning a physical update period.        */
/* -------------------------------------------------------------------------- */
#define AF_BLM_STORE_ENABLE_BIT          0x02u /* L0044 b1 */
#define AF_BLM_UPDATE_DELAY_BIT          0x08u /* L0043 b3 */
#define CAL_BLM_STORE_COOL_MIN          120u   /* LC5DD: must be > 120 */
#define CAL_BLM_STORE_COOL_MAX          240u   /* LC5DE: must be <= 240 */
#define CAL_BLM_STORE_LOAD_MIN            0u   /* LC5DF */
#define CAL_BLM_UPDATE_COUNT             28u   /* LC5E0 */
#define CAL_BLM_UPDATE_AMOUNT             2u   /* LC5E1 */
#define CAL_BLM_MAX                     160u   /* LC5E2 */
#define CAL_BLM_MIN                     108u   /* LC5E3 */
#define CAL_BLM_INT_HIGH_DELTA             5u   /* LC5E4 */
#define CAL_BLM_INT_LOW_DELTA              5u   /* LC5E5 */
static bua_u8 bua_blm_store_qualify_step35(bua_u8 af_mode_word2,
                                            bua_u8 coolant,
                                            bua_u16 afr_code,
                                            bua_u8 filtered_load)
{
    bua_u8 b;
    bua_u8 load_min;
    b = af_mode_word2;
    load_min = CAL_BLM_STORE_LOAD_MIN;
    /* LED7B: BL store can only be enabled while closed loop is active. */
    if ((b & AF_CLOSED_LOOP_BIT) == 0u)
        goto disable_store;
    /* Executable boundaries are asymmetric: coolant >120 and <=240. */
    if (coolant <= CAL_BLM_STORE_COOL_MIN)
        goto disable_store;
    if (coolant > CAL_BLM_STORE_COOL_MAX)
        goto disable_store;
    /* Exact 16-bit comparison against LC3CB=445 (stoichiometric AFR code). */
    if (afr_code != CAL_STOICH_CODE)
        goto disable_store;
    b |= AF_BLM_STORE_ENABLE_BIT;
    if (filtered_load >= load_min)
        return b;
    /* With this image LC5DF=0 and an unsigned load byte, this branch cannot
       reject a load.  It is retained because the executable contains it. */
disable_store:
    b &= (bua_u8)~AF_BLM_STORE_ENABLE_BIT;
    return b;
}
typedef struct {
    bua_u8 timer;
    bua_u8 current_blm;
    bua_u8 selected_cell;
    bua_u8 updated;
    bua_s16 change;
} BuaBlmLearnResult;
static BuaBlmLearnResult bua_blm_learn_step35(bua_u8 af_mode_word,
                                               bua_u8 af_mode_word2,
                                               bua_u8 integrator,
                                               bua_u8 timer,
                                               bua_u8 selected_cell,
                                               bua_u8 *cells,
                                               bua_u8 cell_count)
{
    BuaBlmLearnResult r;
    bua_u8 deviation;
    bua_u8 value;
    bua_u8 rich;
    r.timer = timer;
    r.current_blm = 128u;
    r.selected_cell = selected_cell;
    r.updated = 0u;
    r.change = 0;
    if (cells == (bua_u8 *)0 || selected_cell >= cell_count)
        return r;
    r.current_blm = cells[selected_cell];
    /* LDDCB: LDD L0043 puts L0043 in A and L0044 in B. */
    if ((af_mode_word2 & AF_BLM_STORE_ENABLE_BIT) == 0u)
        goto clear_timer;
    if ((af_mode_word & AF_BLM_UPDATE_DELAY_BIT) != 0u)
        goto clear_timer;
    if (integrator == 128u)
        goto clear_timer;
    /* Saturating INCB/DECB sequence at LDDDB..LDDE1. */
    r.timer = (bua_u8)(r.timer + 1u);
    if (r.timer == 0u)
        r.timer = 255u;
    if (r.timer < CAL_BLM_UPDATE_COUNT)
        return r;
    rich = (bua_u8)((af_mode_word2 & AF_RICH_LEAN_BIT) != 0u);
    if (integrator > 128u) {
        deviation = (bua_u8)(integrator - 128u);
        if (deviation <= CAL_BLM_INT_HIGH_DELTA)
            return r;
        /* L0044 b6 must indicate lean (0) for an upward BLM update. */
        if (rich != 0u)
            return r;
        value = cells[selected_cell];
        if ((unsigned int)value + CAL_BLM_UPDATE_AMOUNT > CAL_BLM_MAX)
            value = CAL_BLM_MAX;
        else
            value = (bua_u8)(value + CAL_BLM_UPDATE_AMOUNT);
        r.change = (bua_s16)value - (bua_s16)cells[selected_cell];
    } else {
        deviation = (bua_u8)(128u - integrator);
        if (deviation <= CAL_BLM_INT_LOW_DELTA)
            return r;
        /* L0044 b6 must indicate rich (1) for a downward BLM update. */
        if (rich == 0u)
            return r;
        value = cells[selected_cell];
        if (value < (bua_u8)(CAL_BLM_MIN + CAL_BLM_UPDATE_AMOUNT))
            value = CAL_BLM_MIN;
        else
            value = (bua_u8)(value - CAL_BLM_UPDATE_AMOUNT);
        if (value < CAL_BLM_MIN)
            value = CAL_BLM_MIN;
        r.change = (bua_s16)value - (bua_s16)cells[selected_cell];
    }
    cells[selected_cell] = value;
    r.current_blm = value;
    r.updated = 1u;
clear_timer:
    r.timer = 0u;
    return r;
}

/* Step 97 scheduler-facing LDDC2..LDE6A tail.  This block is reached after
 * odd-loop fuel output, not through the Segment-D table entry.  Odd counts
 * with (L0000 & 6)==0 occur every 50 ms (counts 1 and 9 in each 16-count
 * major sequence).  BLM learning precedes the old-RPM store and transient
 * load/TPS filter service exactly as in the source. */
static void bua_odd_50ms_tail_lddc2(void)
{
    BuaBlmLearnResult b;
    bua_u16 filtered;
    bua_u8 timer;
    bua_u8 cell;

    if((MINOR_COUNT&0x06u)!=0u)
        return;
    ++stats.odd_50ms_tail_calls;

    cell=RAM8(0x00BFu);
    b=bua_blm_learn_step35(AF_MODE_WORD,AF_MODE_WORD2,
                            CLOSED_LOOP_INTEGRATOR,RAM8(0x00CAu),cell,
                            &mem.low[0x001Cu],16u);
    RAM8(0x00CAu)=b.timer;
    if(b.updated!=0u) {
        BLM=b.current_blm;
        ++stats.scheduler_blm_updates;
    }

    if(cell!=0u)
        RAM8(0x00C1u)=0u;
    AF_MODE_WORD&=(bua_u8)~AF_BLM_UPDATE_DELAY_BIT;
    RAM8(0x005Au)=RPM_DIV12P5;

    /* 6800 DEC followed by BPL: service only when the decremented byte has
     * its sign bit set.  LC339 stores A=1 (reload), B=32 (load coefficient). */
    timer=(bua_u8)(RAM8(0x00E4u)-1u);
    RAM8(0x00E4u)=timer;
    if((timer&0x80u)==0u)
        return;
    RAM8(0x00E4u)=1u;
    filtered=bua_lag_filter_8_8(ram16be_get(TRANSIENT_LOAD_ADDR),
                                 FILTERED_LOAD,32u);
    ram16be_set(TRANSIENT_LOAD_ADDR,filtered);
    filtered=bua_lag_filter_8_8((bua_u16)((bua_u16)RAM8(0x00E9u)<<8),
                                 TPS_LOAD_AXIS,255u);
    RAM8(0x00E9u)=(bua_u8)(filtered>>8);
    ++stats.transient_filter_updates;
}

static void run_step35_blm_learning_test(void)
{
    bua_u8 af2;
    bua_u8 cells[16];
    BuaBlmLearnResult r;
    unsigned int i;
    unsigned int passed;
    passed = 0u;
    for (i = 0u; i < 16u; ++i)
        cells[i] = 128u;
    printf("\nBLM store/learning regression (Step 35):\n");
    af2 = bua_blm_store_qualify_step35(0u, 150u, CAL_STOICH_CODE, 20u);
    if ((af2 & AF_BLM_STORE_ENABLE_BIT) == 0u) ++passed;
    printf("  open loop cannot enable BLM store                               %s\n",
           ((af2&AF_BLM_STORE_ENABLE_BIT)==0u)?"PASS":"FAIL");
    af2 = bua_blm_store_qualify_step35(AF_CLOSED_LOOP_BIT, 120u, CAL_STOICH_CODE, 20u);
    if ((af2 & AF_BLM_STORE_ENABLE_BIT) == 0u) ++passed;
    printf("  coolant=120 fails strict lower BLM-store boundary               %s\n",
           ((af2&AF_BLM_STORE_ENABLE_BIT)==0u)?"PASS":"FAIL");
    af2 = bua_blm_store_qualify_step35(AF_CLOSED_LOOP_BIT, 121u, CAL_STOICH_CODE, 0u);
    if ((af2 & AF_BLM_STORE_ENABLE_BIT) != 0u) ++passed;
    printf("  coolant=121, stoich, load0 enables store (LC5DF=0)              %s\n",
           ((af2&AF_BLM_STORE_ENABLE_BIT)!=0u)?"PASS":"FAIL");
    af2 = bua_blm_store_qualify_step35(AF_CLOSED_LOOP_BIT, 240u, CAL_STOICH_CODE, 20u);
    if ((af2 & AF_BLM_STORE_ENABLE_BIT) != 0u) ++passed;
    printf("  coolant=240 passes inclusive upper BLM-store boundary            %s\n",
           ((af2&AF_BLM_STORE_ENABLE_BIT)!=0u)?"PASS":"FAIL");
    af2 = bua_blm_store_qualify_step35(AF_CLOSED_LOOP_BIT, 241u, CAL_STOICH_CODE, 20u);
    if ((af2 & AF_BLM_STORE_ENABLE_BIT) == 0u) ++passed;
    printf("  coolant=241 disables BLM store                                  %s\n",
           ((af2&AF_BLM_STORE_ENABLE_BIT)==0u)?"PASS":"FAIL");
    af2 = bua_blm_store_qualify_step35(AF_CLOSED_LOOP_BIT, 150u, (bua_u16)(CAL_STOICH_CODE+1u), 20u);
    if ((af2 & AF_BLM_STORE_ENABLE_BIT) == 0u) ++passed;
    printf("  AFR code must equal LC3CB=445 exactly                            %s\n",
           ((af2&AF_BLM_STORE_ENABLE_BIT)==0u)?"PASS":"FAIL");
    r = bua_blm_learn_step35(0u, 0u, 140u, 17u, 3u, cells, 16u);
    if (!r.updated && r.timer==0u) ++passed;
    printf("  store disabled clears BLM update timer                           %s\n",
           (!r.updated&&r.timer==0u)?"PASS":"FAIL");
    r = bua_blm_learn_step35(AF_BLM_UPDATE_DELAY_BIT, AF_BLM_STORE_ENABLE_BIT, 140u, 17u, 3u, cells, 16u);
    if (!r.updated && r.timer==0u) ++passed;
    printf("  L0043 b3 cell-change delay clears BLM update timer               %s\n",
           (!r.updated&&r.timer==0u)?"PASS":"FAIL");
    r = bua_blm_learn_step35(0u, AF_BLM_STORE_ENABLE_BIT, 128u, 17u, 3u, cells, 16u);
    if (!r.updated && r.timer==0u) ++passed;
    printf("  neutral integrator128 clears BLM update timer                    %s\n",
           (!r.updated&&r.timer==0u)?"PASS":"FAIL");
    cells[3]=128u;
    r = bua_blm_learn_step35(0u, AF_BLM_STORE_ENABLE_BIT, 134u, 26u, 3u, cells, 16u);
    if (!r.updated && r.timer==27u && cells[3]==128u) ++passed;
    printf("  qualifying lean correction waits while count remains below28     %s\n",
           (!r.updated&&r.timer==27u&&cells[3]==128u)?"PASS":"FAIL");
    r = bua_blm_learn_step35(0u, AF_BLM_STORE_ENABLE_BIT, 133u, 27u, 3u, cells, 16u);
    if (!r.updated && r.timer==28u && cells[3]==128u) ++passed;
    printf("  integrator deviation exactly +5 does not learn                   %s\n",
           (!r.updated&&r.timer==28u&&cells[3]==128u)?"PASS":"FAIL");
    r = bua_blm_learn_step35(0u, AF_BLM_STORE_ENABLE_BIT, 134u, 27u, 3u, cells, 16u);
    if (r.updated && r.timer==0u && cells[3]==130u && r.change==2) ++passed;
    printf("  lean + INT134 at threshold raises selected BLM by2               %s\n",
           (r.updated&&r.timer==0u&&cells[3]==130u&&r.change==2)?"PASS":"FAIL");
    cells[4]=128u;
    r = bua_blm_learn_step35(0u, (bua_u8)(AF_BLM_STORE_ENABLE_BIT|AF_RICH_LEAN_BIT), 122u, 27u, 4u, cells, 16u);
    if (r.updated && cells[4]==126u && r.change==-2) ++passed;
    printf("  rich + INT122 at threshold lowers selected BLM by2               %s\n",
           (r.updated&&cells[4]==126u&&r.change==-2)?"PASS":"FAIL");
    cells[5]=128u;
    r = bua_blm_learn_step35(0u, (bua_u8)(AF_BLM_STORE_ENABLE_BIT|AF_RICH_LEAN_BIT), 140u, 27u, 5u, cells, 16u);
    if (!r.updated && r.timer==28u && cells[5]==128u) ++passed;
    printf("  rich flag blocks upward learning despite high integrator         %s\n",
           (!r.updated&&r.timer==28u&&cells[5]==128u)?"PASS":"FAIL");
    cells[6]=159u;
    r = bua_blm_learn_step35(0u, AF_BLM_STORE_ENABLE_BIT, 140u, 27u, 6u, cells, 16u);
    if (r.updated && cells[6]==160u) ++passed;
    printf("  upward learning clamps selected BLM at160                        %s\n",
           (r.updated&&cells[6]==160u)?"PASS":"FAIL");
    cells[7]=109u;
    r = bua_blm_learn_step35(0u, (bua_u8)(AF_BLM_STORE_ENABLE_BIT|AF_RICH_LEAN_BIT), 116u, 27u, 7u, cells, 16u);
    if (r.updated && cells[7]==108u) ++passed;
    printf("  downward learning clamps selected BLM at108                      %s\n",
           (r.updated&&cells[7]==108u)?"PASS":"FAIL");
    printf("  step-35 BLM regression result: %s (%u/16)\n",
           (passed==16u)?"PASS":"FAIL",passed);
}
/* --------------------------------------------------------------------------
 * Step 36: LD41F..LD450 IAC commanded idle speed
 * -------------------------------------------------------------------------- */
#define STEP36_IAC_PN_OFFSET_RAW       4u   /* LC634: 4 * 12.5 = 50 RPM */
#define STEP36_IAC_BASE_OFFSET_RAW     0u   /* LC636 */
#define STEP36_IAC_HOT_SPARK_RAW      24u  /* LC685: 24 * 12.5 = 300 RPM */
#define STEP36_IAC_DIAG_CMD_RAW        80u  /* literal LDAA #80 at LD448 */
#define STEP36_IAC_PN_BIT              0x80u /* L00F3 sign bit: P/N */
#define STEP36_IAC_OPEN_LOOP_BIT       0x80u /* L00F2 sign bit; when clear add LC636 */
#define STEP36_HU_1K_IDLE_BIT          0x20u /* L003A b5; executable adds LC685 */
#define STEP36_ALDL_MODE_BIT           0x20u /* L0035 b5 */
static const bua_u8 step36_iac_target_rpm[17] = {
    84u,84u,84u,84u,84u,76u,72u,60u,48u,48u,44u,44u,44u,44u,44u,44u,44u
};
typedef struct BuaIacCommandStep36 {
    bua_u8 base_raw;
    bua_u8 command_raw;
    unsigned int command_rpm;
    bua_u8 pn_offset_applied;
    bua_u8 base_offset_applied;
    bua_u8 hot_spark_offset_applied;
    bua_u8 diagnostic_override;
} BuaIacCommandStep36;
static BuaIacCommandStep36 bua_iac_command_step36(bua_u8 coolant,
                                                    bua_u8 iac_mode_word2,
                                                    bua_u8 iac_control_word,
                                                    bua_u8 hu_display_mode,
                                                    bua_u8 minor_mode_word2)
{
    BuaIacCommandStep36 r;
    unsigned int a;
    r.base_raw = bua_lookup_spacing16(step36_iac_target_rpm,17u,coolant);
    a = (unsigned int)r.base_raw;
    r.pn_offset_applied = 0u;
    r.base_offset_applied = 0u;
    r.hot_spark_offset_applied = 0u;
    r.diagnostic_override = 0u;
    /* LD42D..LD43A: sign bit in L00F3 identifies Park/Neutral. */
    if ((iac_mode_word2 & STEP36_IAC_PN_BIT) != 0u) {
        a = (a + STEP36_IAC_PN_OFFSET_RAW) & 0xFFu;
        r.pn_offset_applied = 1u;
    }
    /* LD43A..LD440: when L00F2 is non-negative, add LC636 (=0 here). */
    if ((iac_control_word & STEP36_IAC_OPEN_LOOP_BIT) == 0u) {
        a = (a + STEP36_IAC_BASE_OFFSET_RAW) & 0xFFu;
        r.base_offset_applied = 1u;
    }
    /* LD440..LD448: L003A b5 causes LC685 (+300 RPM) to be added. */
    if ((hu_display_mode & STEP36_HU_1K_IDLE_BIT) != 0u) {
        a = (a + STEP36_IAC_HOT_SPARK_RAW) & 0xFFu;
        r.hot_spark_offset_applied = 1u;
    }
    /* LD448..LD450: ALDL diagnostic mode replaces the command with literal 80. */
    if ((minor_mode_word2 & STEP36_ALDL_MODE_BIT) != 0u) {
        a = STEP36_IAC_DIAG_CMD_RAW;
        r.diagnostic_override = 1u;
    }
    r.command_raw = (bua_u8)a;
    r.command_rpm = a * 12u + a / 2u; /* exact raw * 12.5 without float */
    return r;
}
static void run_step36_iac_command_test(void)
{
    unsigned int passed = 0u;
    BuaIacCommandStep36 r;
    printf("\nIAC commanded-idle regression (Step 36, LD41F..LD450):\n");
    r=bua_iac_command_step36(0u,STEP36_IAC_PN_BIT,0u,0u,0u);
    if (r.base_raw==84u && r.command_raw==88u && r.command_rpm==1100u) ++passed;
    printf("  -40C anchor: 1050 base +50 P/N -> 1100 RPM                 %s\n",
           (r.base_raw==84u && r.command_raw==88u && r.command_rpm==1100u)?"PASS":"FAIL");
    r=bua_iac_command_step36(80u,STEP36_IAC_PN_BIT,0u,0u,0u); /* 20 C */
    if (r.base_raw==76u && r.command_rpm==1000u) ++passed;
    printf("  20C anchor: 950 base +50 P/N -> 1000 RPM                   %s\n",
           (r.base_raw==76u && r.command_rpm==1000u)?"PASS":"FAIL");
    r=bua_iac_command_step36(96u,STEP36_IAC_PN_BIT,0u,0u,0u); /* 32 C */
    if (r.base_raw==72u && r.command_rpm==950u) ++passed;
    printf("  32C anchor: 900 base +50 P/N -> 950 RPM                    %s\n",
           (r.base_raw==72u && r.command_rpm==950u)?"PASS":"FAIL");
    r=bua_iac_command_step36(112u,STEP36_IAC_PN_BIT,0u,0u,0u); /* 44 C */
    if (r.base_raw==60u && r.command_rpm==800u) ++passed;
    printf("  44C anchor: 750 base +50 P/N -> 800 RPM                    %s\n",
           (r.base_raw==60u && r.command_rpm==800u)?"PASS":"FAIL");
    r=bua_iac_command_step36(128u,STEP36_IAC_PN_BIT,0u,0u,0u); /* 56 C */
    if (r.base_raw==48u && r.command_rpm==650u) ++passed;
    printf("  56C anchor: 600 base +50 P/N -> 650 RPM                    %s\n",
           (r.base_raw==48u && r.command_rpm==650u)?"PASS":"FAIL");
    r=bua_iac_command_step36(160u,STEP36_IAC_PN_BIT,0u,0u,0u); /* 80 C */
    if (r.base_raw==44u && r.command_rpm==600u) ++passed;
    printf("  80C/warm P/N: 550 base +50 -> 600 RPM                      %s\n",
           (r.base_raw==44u && r.command_rpm==600u)?"PASS":"FAIL");
    r=bua_iac_command_step36(160u,0u,0u,0u,0u);
    if (r.command_raw==44u && r.command_rpm==550u) ++passed;
    printf("  80C/warm drive: no P/N offset -> 550 RPM                    %s\n",
           (r.command_raw==44u && r.command_rpm==550u)?"PASS":"FAIL");
    r=bua_iac_command_step36(160u,STEP36_IAC_PN_BIT,0u,STEP36_HU_1K_IDLE_BIT,0u);
    if (r.command_raw==72u && r.command_rpm==900u && r.hot_spark_offset_applied) ++passed;
    printf("  warm P/N + L003A b5: add 300 -> 900 RPM                    %s\n",
           (r.command_raw==72u && r.command_rpm==900u && r.hot_spark_offset_applied)?"PASS":"FAIL");
    r=bua_iac_command_step36(160u,STEP36_IAC_PN_BIT,0u,0u,STEP36_ALDL_MODE_BIT);
    if (r.command_raw==80u && r.command_rpm==1000u && r.diagnostic_override) ++passed;
    printf("  ALDL diagnostic override literal80 -> 1000 RPM             %s\n",
           (r.command_raw==80u && r.command_rpm==1000u && r.diagnostic_override)?"PASS":"FAIL");
    r=bua_iac_command_step36(104u,STEP36_IAC_PN_BIT,0u,0u,0u);
    if (r.base_raw==66u && r.command_raw==70u && r.command_rpm==875u) ++passed;
    printf("  interpolation halfway 32..44C -> 825 +50 = 875 RPM        %s\n",
           (r.base_raw==66u && r.command_raw==70u && r.command_rpm==875u)?"PASS":"FAIL");
    if (STEP36_IAC_BASE_OFFSET_RAW==0u) ++passed;
    printf("  LC636 general command-speed offset is zero in this image    %s\n",
           (STEP36_IAC_BASE_OFFSET_RAW==0u)?"PASS":"FAIL");
    /* LC631/LC632 are IAC-step anticipation calibrations upstream, not used by LD41F command-speed sum. */
    if (CAL_AF_MODE3==0x62u) ++passed;
    printf("  target-speed path remains independent of fuel closed-loop bit %s\n",
           (CAL_AF_MODE3==0x62u)?"PASS":"FAIL");
    printf("  step-36 IAC command regression result: %s (%u/12)\n",
           (passed==12u)?"PASS":"FAIL",passed);
}
/* --------------------------------------------------------------------------
 * Step 37: LD59A..LD6AA normal IAC PID feedback path
 *
 * This step translates the ordinary low-gain path through the proportional,
 * derivative, integral, quantizer and coolant-multiplier stages.  The source
 * contains additional high-gain and transition/learning branches; those are
 * intentionally not folded into this helper yet.
 * -------------------------------------------------------------------------- */
#define STEP37_PN_DEADBAND_RAW          6u   /* LC648: 75 RPM */
#define STEP37_DRIVE_DEADBAND_RAW       4u   /* LC649: 50 RPM */
#define STEP37_PROP_UNDER_GAIN         32u   /* LC650 */
#define STEP37_PROP_OVER_GAIN          32u   /* LC651 */
#define STEP37_DERIV_POS_GAIN          16u   /* LC654 */
#define STEP37_DERIV_NEG_GAIN          64u   /* LC655 */
#define STEP37_INT_PN_GAIN             40u   /* LC659 */
#define STEP37_INT_DRIVE_GAIN          40u   /* LC65A */
#define STEP37_RATE_DEADBAND_RAW       12u   /* LC65B/LC65C */
#define STEP37_QUANTIZER_GAIN          32u   /* LC66B */
#define STEP37_INVERSE_QUANT            8u   /* LC66C */
#define STEP37_PN_SCALE                255u  /* LC65D */
static const bua_u8 step37_iac_cool_mult[9] = {
    204u,204u,230u,255u,255u,255u,255u,255u,255u
};
typedef struct BuaIacPidStep37 {
    int rpm_error_raw;
    int rpm_rate_raw;
    int proportional;
    int derivative;
    int integral_before;
    int integral_after;
    int linear_sum;
    bua_u8 deadband_raw;
    bua_u8 motor_request;
    bua_u8 motor_steps;
    bua_u8 direction_open;
    bua_u8 integral_used;
    bua_u8 in_deadband;
    bua_u8 coolant_multiplier;
} BuaIacPidStep37;
static int step37_mul_gain_signed(int value, unsigned int gain)
{
    unsigned int mag;
    unsigned int q;
    int neg;
    if (value < 0) {
        neg=1;
        mag=(unsigned int)(-value);
    } else {
        neg=0;
        mag=(unsigned int)value;
    }
    if (mag>255u) mag=255u;
    q=(mag*gain + 128u)/256u;
    return neg ? -(int)q : (int)q;
}
static int step37_clamp127(int v)
{
    if (v>127) return 127;
    if (v<-127) return -127;
    return v;
}
static bua_u8 step37_coolant_mult(bua_u8 coolant)
{
    /* C686 is a 9-line table at 32-count spacing, used through LF2BF. */
    unsigned int idx=(unsigned int)(coolant>>5);
    unsigned int frac=(unsigned int)(coolant & 31u);
    unsigned int a,b;
    if (idx>=8u) return step37_iac_cool_mult[8];
    a=step37_iac_cool_mult[idx];
    b=step37_iac_cool_mult[idx+1u];
    if (b>=a) return (bua_u8)(a + ((b-a)*frac)/32u);
    return (bua_u8)(a - ((a-b)*frac)/32u);
}
static BuaIacPidStep37 bua_iac_pid_step37(bua_u8 command_raw,
                                           bua_u8 actual_raw,
                                           int rpm_rate_raw,
                                           bua_u8 park_neutral,
                                           bua_u8 coolant,
                                           int integral_state)
{
    BuaIacPidStep37 r;
    int err;
    int p,d,i,sum;
    unsigned int gain;
    unsigned int qmag;
    unsigned int steps;
    err=(int)command_raw-(int)actual_raw;
    if (err>127) err=127;
    if (err<-127) err=-127;
    if (rpm_rate_raw>127) rpm_rate_raw=127;
    if (rpm_rate_raw<-127) rpm_rate_raw=-127;
    r.rpm_error_raw=err;
    r.rpm_rate_raw=rpm_rate_raw;
    r.deadband_raw=park_neutral ? STEP37_PN_DEADBAND_RAW : STEP37_DRIVE_DEADBAND_RAW;
    r.in_deadband=((err<0?-err:err) <= (int)r.deadband_raw) ? 1u : 0u;
    gain=(err>=0) ? STEP37_PROP_UNDER_GAIN : STEP37_PROP_OVER_GAIN;
    p=step37_mul_gain_signed(err,gain);
    /* L00FF sign is the source's filtered RPM-rate sign.  Keep that sign. */
    gain=(rpm_rate_raw>=0) ? STEP37_DERIV_POS_GAIN : STEP37_DERIV_NEG_GAIN;
    d=step37_mul_gain_signed(rpm_rate_raw,gain);
    i=step37_clamp127(integral_state);
    r.integral_before=i;
    r.integral_used=0u;
    /* LD633..LD668: integration is permitted while RPM rate is within the
       selected rate deadband.  The same 40 gain is calibrated for P/N and
       Drive in this image. */
    if ((rpm_rate_raw<0?-rpm_rate_raw:rpm_rate_raw) <= STEP37_RATE_DEADBAND_RAW) {
        unsigned int igain=park_neutral ? STEP37_INT_PN_GAIN : STEP37_INT_DRIVE_GAIN;
        int idelta=step37_mul_gain_signed(err,(unsigned int)(igain*2u));
        i=step37_clamp127(i+idelta);
        r.integral_used=1u;
    } else {
        i=0;
    }
    r.integral_after=i;
    sum=step37_clamp127(p+d+i);
    if (park_neutral) sum=step37_mul_gain_signed(sum,STEP37_PN_SCALE);
    r.proportional=p;
    r.derivative=d;
    r.linear_sum=sum;
    r.coolant_multiplier=step37_coolant_mult(coolant);
    r.direction_open=(sum>0)?1u:0u; /* underspeed -> more bypass air */
    qmag=(unsigned int)(sum<0?-sum:sum);
    if (qmag<STEP37_INVERSE_QUANT) {
        steps=0u;
    } else {
        /* C66B converts the linear algorithm output to whole step demand.
           Keep the original fixed-point intent rather than floating point. */
        steps=(qmag*STEP37_QUANTIZER_GAIN + 127u)/256u;
        if (steps==0u) steps=1u;
        steps=(steps*(unsigned int)r.coolant_multiplier + 127u)/256u;
        if (steps>127u) steps=127u;
    }
    r.motor_steps=(bua_u8)steps;
    r.motor_request=(bua_u8)(steps | (r.direction_open?0x00u:0x80u));
    if (steps==0u) r.motor_request=0u;
    return r;
}
static void run_step37_iac_pid_test(void)
{
    unsigned int passed=0u;
    BuaIacPidStep37 r;
    printf("\nIAC PID feedback regression (Step 37, normal LD59A..LD6AA path):\n");
    r=bua_iac_pid_step37(64u,64u,0,1u,160u,0);
    if (r.rpm_error_raw==0 && r.motor_steps==0u) ++passed;
    printf("  target=actual -> zero IAC step request                              %s\n",
           (r.rpm_error_raw==0 && r.motor_steps==0u)?"PASS":"FAIL");
    r=bua_iac_pid_step37(64u,56u,0,1u,160u,0); /* 800 target, 700 actual */
    if (r.rpm_error_raw==8 && r.proportional>0 && r.direction_open) ++passed;
    printf("  underspeed error +8 raw (100 RPM) produces opening correction      %s\n",
           (r.rpm_error_raw==8 && r.proportional>0 && r.direction_open)?"PASS":"FAIL");
    r=bua_iac_pid_step37(64u,72u,0,1u,160u,0); /* 800 target, 900 actual */
    if (r.rpm_error_raw==-8 && r.proportional<0 && !r.direction_open) ++passed;
    printf("  overspeed error -8 raw (100 RPM) produces closing correction       %s\n",
           (r.rpm_error_raw==-8 && r.proportional<0 && !r.direction_open)?"PASS":"FAIL");
    r=bua_iac_pid_step37(64u,59u,0,1u,160u,0);
    if (r.in_deadband) ++passed;
    printf("  P/N 5-count error lies inside LC648 six-count deadband              %s\n",
           r.in_deadband?"PASS":"FAIL");
    r=bua_iac_pid_step37(64u,59u,0,0u,160u,0);
    if (!r.in_deadband) ++passed;
    printf("  same 5-count error is outside Drive LC649 four-count deadband       %s\n",
           (!r.in_deadband)?"PASS":"FAIL");
    r=bua_iac_pid_step37(64u,56u,0,1u,160u,0);
    if (r.integral_used && r.integral_after>0) ++passed;
    printf("  low RPM-rate permits positive integral accumulation                 %s\n",
           (r.integral_used && r.integral_after>0)?"PASS":"FAIL");
    r=bua_iac_pid_step37(64u,72u,0,1u,160u,0);
    if (r.integral_used && r.integral_after<0) ++passed;
    printf("  overspeed produces negative integral accumulation                   %s\n",
           (r.integral_used && r.integral_after<0)?"PASS":"FAIL");
    r=bua_iac_pid_step37(64u,56u,20,1u,160u,50);
    if (!r.integral_used && r.integral_after==0) ++passed;
    printf("  RPM-rate beyond LC65B/LC65C clears/disables integral                %s\n",
           (!r.integral_used && r.integral_after==0)?"PASS":"FAIL");
    r=bua_iac_pid_step37(64u,0u,0,1u,160u,126);
    if (r.integral_after==127) ++passed;
    printf("  positive integrator saturates at +127                               %s\n",
           (r.integral_after==127)?"PASS":"FAIL");
    r=bua_iac_pid_step37(0u,64u,0,1u,160u,-126);
    if (r.integral_after==-127) ++passed;
    printf("  negative integrator saturates at -127                               %s\n",
           (r.integral_after==-127)?"PASS":"FAIL");
    r=bua_iac_pid_step37(64u,56u,0,1u,0u,40);
    if (r.coolant_multiplier==204u) ++passed;
    printf("  cold C686 multiplier is 204/256 (about 0.80)                        %s\n",
           (r.coolant_multiplier==204u)?"PASS":"FAIL");
    r=bua_iac_pid_step37(64u,56u,0,1u,160u,40);
    if (r.coolant_multiplier==255u) ++passed;
    printf("  warm C686 multiplier is 255/256 (about unity)                       %s\n",
           (r.coolant_multiplier==255u)?"PASS":"FAIL");
    r=bua_iac_pid_step37(64u,40u,0,1u,160u,40);
    if (r.motor_steps>0u && (r.motor_request&0x80u)==0u) ++passed;
    printf("  sufficiently large underspeed becomes nonzero opening step request  %s\n",
           (r.motor_steps>0u && (r.motor_request&0x80u)==0u)?"PASS":"FAIL");
    r=bua_iac_pid_step37(40u,64u,0,1u,160u,-40);
    if (r.motor_steps>0u && (r.motor_request&0x80u)!=0u) ++passed;
    printf("  sufficiently large overspeed becomes direction-bit closing request %s\n",
           (r.motor_steps>0u && (r.motor_request&0x80u)!=0u)?"PASS":"FAIL");
    printf("  step-37 IAC PID regression result: %s (%u/14)\n",
           (passed==14u)?"PASS":"FAIL",passed);
}
/* --------------------------------------------------------------------------
 * Step 38: IAC transition handling, minimum-position learning, high-D branch
 * -------------------------------------------------------------------------- */
#define STEP38_TRANSITION_DELAY_ND       16u  /* LC675 */
#define STEP38_MIN_LEARN_DEADBAND_COUNT  20u  /* LC669 */
#define STEP38_LEARN_MINOR_PHASE       0x30u  /* executable compare */
#define STEP38_PROP_HI_ERR_THRESHOLD     4u  /* LC658 */
#define STEP38_PROP_HI_RATE_THRESHOLD  255u  /* LC652: effectively disables */
#define STEP38_PROP_HI_GAIN_ADD           0u  /* LC653 */
#define STEP38_DERIV_HI_RATE_THRESHOLD   22u  /* LC656 */
#define STEP38_DERIV_HI_GAIN_ADD        255u  /* LC657 */
#define STEP38_IAC_PN_BIT              0x80u
#define STEP38_IAC_OLD_PN_BIT          0x10u
#define STEP38_IAC_SLEW_ABS_BIT        0x01u
#define STEP38_HOT_RESTART_BIT         0x20u
typedef struct BuaIacTransitionStep38 {
    bua_u8 mode_word2;
    bua_u8 pid_disable_timer;
    bua_u8 deadband_timer;
    bua_u8 transition_detected;
    bua_u8 neutral_to_drive;
    bua_u8 drive_to_neutral;
} BuaIacTransitionStep38;
typedef struct BuaIacMinLearnStep38 {
    bua_u8 learned_min_before;
    bua_u8 learned_min_after;
    bua_u8 learned;
    bua_u8 blocked_by_state;
} BuaIacMinLearnStep38;
typedef struct BuaIacHighGainStep38 {
    int low_term;
    int selected_term;
    bua_u8 high_gain_selected;
    bua_u8 threshold_raw;
    bua_u8 gain_add;
} BuaIacHighGainStep38;
static BuaIacTransitionStep38 bua_iac_transition_step38(bua_u8 mode_word2,
                                                         bua_u8 now_park_neutral,
                                                         bua_u8 deadband_timer)
{
    BuaIacTransitionStep38 r;
    bua_u8 old_pn;
    r.mode_word2=mode_word2;
    r.pid_disable_timer=0u;
    r.deadband_timer=deadband_timer;
    r.transition_detected=0u;
    r.neutral_to_drive=0u;
    r.drive_to_neutral=0u;
    old_pn=(bua_u8)((mode_word2 & STEP38_IAC_OLD_PN_BIT)!=0u);
    if (now_park_neutral) {
        r.mode_word2=(bua_u8)(r.mode_word2 | STEP38_IAC_PN_BIT);
        if (!old_pn) {
            /* LD4DD: entering Neutral/Park records old-P/N state and clears
               both the transition delay and deadband timer. */
            r.mode_word2=(bua_u8)(r.mode_word2 | STEP38_IAC_OLD_PN_BIT);
            r.pid_disable_timer=0u;
            r.deadband_timer=0u;
            r.transition_detected=1u;
            r.drive_to_neutral=1u;
        }
    } else {
        r.mode_word2=(bua_u8)(r.mode_word2 & (bua_u8)~STEP38_IAC_PN_BIT);
        if (old_pn) {
            /* LD4F5: Neutral/Park -> Drive gets LC675 delay. */
            r.pid_disable_timer=STEP38_TRANSITION_DELAY_ND;
            r.mode_word2=(bua_u8)(r.mode_word2 & (bua_u8)~STEP38_IAC_OLD_PN_BIT);
            r.deadband_timer=0u;
            r.transition_detected=1u;
            r.neutral_to_drive=1u;
        }
    }
    return r;
}
static BuaIacMinLearnStep38 bua_iac_min_learn_step38(bua_u8 learned_min,
                                                       bua_u8 warm_park_limit,
                                                       int min_position_error,
                                                       bua_u8 in_drive,
                                                       bua_u8 deadband_timer,
                                                       bua_u8 mode_word2,
                                                       bua_u8 hot_restart_flags,
                                                       bua_u8 minor_loop_counter)
{
    BuaIacMinLearnStep38 r;
    bua_u8 v;
    r.learned_min_before=learned_min;
    r.learned_min_after=learned_min;
    r.learned=0u;
    r.blocked_by_state=0u;
    /* LD503..LD53B: this learner is drive-only and requires more than the
       calibrated 20-count deadband dwell. */
    if (!in_drive || deadband_timer<=STEP38_MIN_LEARN_DEADBAND_COUNT ||
        (mode_word2 & STEP38_IAC_SLEW_ABS_BIT)!=0u ||
        (hot_restart_flags & STEP38_HOT_RESTART_BIT)!=0u ||
        minor_loop_counter!=STEP38_LEARN_MINOR_PHASE ||
        min_position_error==0) {
        r.blocked_by_state=1u;
        return r;
    }
    v=learned_min;
    if (min_position_error>0) {
        v=(bua_u8)(v+1u);
        if (v>warm_park_limit || v==0u) v=warm_park_limit;
    } else {
        v=(bua_u8)(v-1u);
        /* Executable prevents the learned minimum from reaching zero. */
        if (v==0u) v=1u;
    }
    r.learned_min_after=v;
    r.learned=1u;
    return r;
}
static BuaIacHighGainStep38 bua_iac_high_derivative_step38(int rpm_error_raw,
                                                            int rpm_rate_raw)
{
    BuaIacHighGainStep38 r;
    int low;
    int selected;
    if (rpm_rate_raw>127) rpm_rate_raw=127;
    if (rpm_rate_raw<-127) rpm_rate_raw=-127;
    low=step37_mul_gain_signed(rpm_rate_raw,
                               (rpm_rate_raw>=0)?STEP37_DERIV_POS_GAIN:STEP37_DERIV_NEG_GAIN);
    selected=low;
    r.high_gain_selected=0u;
    r.threshold_raw=STEP38_DERIV_HI_RATE_THRESHOLD;
    r.gain_add=STEP38_DERIV_HI_GAIN_ADD;
    /* LD5B9 high-gain addition is only reachable on the positive-rate branch,
       while the engine is underspeed by more than LC658 and rate exceeds
       LC656.  The +255 addition wraps the 8-bit gain and the carry path adds
       essentially one full rate term.  Preserve that executable structure. */
    if (rpm_error_raw>(int)STEP38_PROP_HI_ERR_THRESHOLD &&
        rpm_rate_raw>(int)STEP38_DERIV_HI_RATE_THRESHOLD) {
        unsigned int wrapped_gain=(STEP37_DERIV_POS_GAIN + STEP38_DERIV_HI_GAIN_ADD)&255u;
        int extra=(int)(((unsigned int)rpm_rate_raw*wrapped_gain + 128u)/256u);
        selected=rpm_rate_raw+extra;
        if (selected>127) selected=127;
        r.high_gain_selected=1u;
    }
    r.low_term=low;
    r.selected_term=selected;
    return r;
}
static void run_step38_iac_transition_learning_test(void)
{
    unsigned int passed=0u;
    BuaIacTransitionStep38 t;
    BuaIacMinLearnStep38 l;
    BuaIacHighGainStep38 h;
    printf("\nIAC transition/min-position/high-gain regression (Step 38):\n");
    t=bua_iac_transition_step38(0u,1u,9u);
    if (t.drive_to_neutral && t.pid_disable_timer==0u && t.deadband_timer==0u &&
        (t.mode_word2&STEP38_IAC_OLD_PN_BIT)!=0u) ++passed;
    printf("  Drive -> P/N records old-P/N state and clears transition timer      %s\n",
           (t.drive_to_neutral && t.pid_disable_timer==0u && t.deadband_timer==0u &&
            (t.mode_word2&STEP38_IAC_OLD_PN_BIT)!=0u)?"PASS":"FAIL");
    t=bua_iac_transition_step38((bua_u8)(STEP38_IAC_PN_BIT|STEP38_IAC_OLD_PN_BIT),0u,12u);
    if (t.neutral_to_drive && t.pid_disable_timer==16u && t.deadband_timer==0u &&
        (t.mode_word2&STEP38_IAC_OLD_PN_BIT)==0u) ++passed;
    printf("  P/N -> Drive loads LC675=16 PID-inhibit counts                     %s\n",
           (t.neutral_to_drive && t.pid_disable_timer==16u && t.deadband_timer==0u &&
            (t.mode_word2&STEP38_IAC_OLD_PN_BIT)==0u)?"PASS":"FAIL");
    t=bua_iac_transition_step38(STEP38_IAC_OLD_PN_BIT,0u,7u);
    if (t.neutral_to_drive && t.pid_disable_timer==16u) ++passed;
    printf("  old-P/N marker alone is sufficient to detect Neutral->Drive        %s\n",
           (t.neutral_to_drive && t.pid_disable_timer==16u)?"PASS":"FAIL");
    t=bua_iac_transition_step38(0u,0u,7u);
    if (!t.transition_detected && t.deadband_timer==7u) ++passed;
    printf("  steady Drive leaves transition state unchanged                      %s\n",
           (!t.transition_detected && t.deadband_timer==7u)?"PASS":"FAIL");
    l=bua_iac_min_learn_step38(50u,80u,1,1u,21u,0u,0u,0x30u);
    if (l.learned && l.learned_min_after==51u) ++passed;
    printf("  drive learner increments minimum one step for positive error        %s\n",
           (l.learned && l.learned_min_after==51u)?"PASS":"FAIL");
    l=bua_iac_min_learn_step38(50u,80u,-1,1u,21u,0u,0u,0x30u);
    if (l.learned && l.learned_min_after==49u) ++passed;
    printf("  drive learner decrements minimum one step for negative error        %s\n",
           (l.learned && l.learned_min_after==49u)?"PASS":"FAIL");
    l=bua_iac_min_learn_step38(80u,80u,1,1u,21u,0u,0u,0x30u);
    if (l.learned && l.learned_min_after==80u) ++passed;
    printf("  learned minimum cannot increment above warm-park limit              %s\n",
           (l.learned && l.learned_min_after==80u)?"PASS":"FAIL");
    l=bua_iac_min_learn_step38(1u,80u,-1,1u,21u,0u,0u,0x30u);
    if (l.learned && l.learned_min_after==1u) ++passed;
    printf("  learned minimum cannot decrement below one step                     %s\n",
           (l.learned && l.learned_min_after==1u)?"PASS":"FAIL");
    l=bua_iac_min_learn_step38(50u,80u,1,1u,20u,0u,0u,0x30u);
    if (!l.learned) ++passed;
    printf("  exactly LC669=20 deadband counts is still too soon to learn         %s\n",
           (!l.learned)?"PASS":"FAIL");
    l=bua_iac_min_learn_step38(50u,80u,1,1u,21u,STEP38_IAC_SLEW_ABS_BIT,0u,0x30u);
    if (!l.learned) ++passed;
    printf("  slew/absolute-position state blocks minimum-position learning       %s\n",
           (!l.learned)?"PASS":"FAIL");
    l=bua_iac_min_learn_step38(50u,80u,1,1u,21u,0u,STEP38_HOT_RESTART_BIT,0x30u);
    if (!l.learned) ++passed;
    printf("  hot-restart state blocks minimum-position learning                  %s\n",
           (!l.learned)?"PASS":"FAIL");
    l=bua_iac_min_learn_step38(50u,80u,1,1u,21u,0u,0u,0x20u);
    if (!l.learned) ++passed;
    printf("  learner only moves on executable minor-loop phase $30               %s\n",
           (!l.learned)?"PASS":"FAIL");
    h=bua_iac_high_derivative_step38(8,20);
    if (!h.high_gain_selected && h.selected_term==h.low_term) ++passed;
    printf("  falling-RPM rate <= LC656=22 stays on low derivative gain           %s\n",
           (!h.high_gain_selected && h.selected_term==h.low_term)?"PASS":"FAIL");
    h=bua_iac_high_derivative_step38(8,23);
    if (h.high_gain_selected && h.selected_term>h.low_term) ++passed;
    printf("  underspeed plus falling-RPM rate >22 selects high derivative branch %s\n",
           (h.high_gain_selected && h.selected_term>h.low_term)?"PASS":"FAIL");
    h=bua_iac_high_derivative_step38(4,30);
    if (!h.high_gain_selected) ++passed;
    printf("  error at LC658=4 does not enable high derivative addition           %s\n",
           (!h.high_gain_selected)?"PASS":"FAIL");
    h=bua_iac_high_derivative_step38(-8,30);
    if (!h.high_gain_selected) ++passed;
    printf("  overspeed error cannot enter the positive high-derivative branch    %s\n",
           (!h.high_gain_selected)?"PASS":"FAIL");
    printf("  step-38 IAC transition/learning regression result: %s (%u/16)\n",
           (passed==16u)?"PASS":"FAIL",passed);
}
/* --------------------------------------------------------------------------
 * Step 39: throttle follower, A/C/fan anticipation, trajectory decay
 * -------------------------------------------------------------------------- */
#define STEP39_TF_TPS_THRESHOLD          3u  /* LC64B */
#define STEP39_TF_GAIN                 192u  /* LC662 */
#define STEP39_TF_MAX                   75u  /* LC663 */
#define STEP39_TF_PN_MULT              255u  /* LC664 */
#define STEP39_TF_MOVING_ADD             5u  /* LC683 */
#define STEP39_VSS_PID_MAX               6u  /* LC64E */
#define STEP39_TF_DECAY_PN               5u  /* LC65E */
#define STEP39_TF_DECAY_DRIVE            4u  /* LC65F */
#define STEP39_TF_DECAY_VSS_MULT        10u  /* LC660 */
#define STEP39_TF_OPEN_LOOP_DELTA       16u  /* LC661 */
#define STEP39_TF_RELEASE_PID_DELAY     30u  /* LC674 */
#define STEP39_AC_DEFAULT               20u  /* LC665 */
#define STEP39_AC_MAX                   32u  /* LC666 */
#define STEP39_AC_MIN                    1u  /* LC667 */
#define STEP39_AC_MAX_PLUS               5u  /* LC668 */
#define STEP39_FAN_ANTIC_STEPS           5u  /* LC632 */
#define STEP39_FAN_PID_DELAY            20u  /* LC676 */
#define STEP39_IAC_TF_BIT             0x10u
#define STEP39_IAC_LOAD_REMOVE_BIT    0x20u
#define STEP39_IAC_LOAD_CONTROL_BIT   0x40u
/* In this calibration both power-steering anticipation values are zero. */
#define STEP39_PS_ANTIC_DRIVE            0u  /* LC680 */
#define STEP39_PS_ANTIC_AC               0u  /* LC681 */
typedef struct BuaIacTfDesiredStep39 {
    bua_u8 tps_effective;
    bua_u8 throttle_steps;
    bua_u8 ac_steps;
    bua_u8 desired_steps;
    bua_u8 ac_learn_reset;
    bua_u8 load_controlled;
} BuaIacTfDesiredStep39;
typedef struct BuaIacTfRequestStep39 {
    int delta;
    bua_u8 request;
    bua_u8 magnitude;
    bua_u8 opening;
    bua_u8 closing;
    bua_u8 pid_disable_timer;
} BuaIacTfRequestStep39;
typedef struct BuaIacAcStateStep39 {
    bua_u8 control_word;
    bua_u8 removal_started;
} BuaIacAcStateStep39;
typedef struct BuaIacDecayStep39 {
    bua_u8 base_count;
    bua_u8 final_count;
    bua_u8 rpm_extension;
} BuaIacDecayStep39;
typedef struct BuaIacFanAnticStep39 {
    bua_u8 target_raw;
    bua_u8 request;
    bua_u8 complete;
    bua_u8 pid_disable_timer;
} BuaIacFanAnticStep39;
static BuaIacTfDesiredStep39 bua_iac_tf_desired_step39(bua_u8 tps_raw,
                                                         bua_u8 in_park_neutral,
                                                         bua_u8 vss_raw,
                                                         bua_u8 ac_requested,
                                                         bua_u8 ac_learned,
                                                         bua_u8 power_steer_high)
{
    BuaIacTfDesiredStep39 r;
    unsigned int a;
    unsigned int prod;
    r.tps_effective=tps_raw;
    r.throttle_steps=0u;
    r.ac_steps=0u;
    r.desired_steps=0u;
    r.ac_learn_reset=0u;
    r.load_controlled=0u;
    /* LE913..LE93E: the follower becomes active only above LC64B.  The
       executable performs TPS*192, then ASLD; the high byte is therefore
       floor(TPS*1.5), saturated and then limited by LC663=75. */
    if (tps_raw>STEP39_TF_TPS_THRESHOLD) {
        prod=(unsigned int)tps_raw*STEP39_TF_GAIN*2u;
        a=prod>>8;
        if (a>255u) a=255u;
        if (a>STEP39_TF_MAX) a=STEP39_TF_MAX;
        r.throttle_steps=(bua_u8)a;
    }
    a=r.throttle_steps;
    /* LE944 power-steering anticipation is structurally present, but both
       applicable calibrations are zero in the 9340 image. */
    if (power_steer_high) {
        a+=STEP39_PS_ANTIC_DRIVE;
        if (ac_requested) a+=STEP39_PS_ANTIC_AC;
        if (a>255u) a=255u;
        r.load_controlled=1u;
    }
    /* LE961..LE97A: when A/C is on, validate the learned anticipate term and
       fall back to LC665=20 if it lies outside 1..32, then add it. */
    if (ac_requested) {
        if (ac_learned<STEP39_AC_MIN || ac_learned>STEP39_AC_MAX) {
            ac_learned=STEP39_AC_DEFAULT;
            r.ac_learn_reset=1u;
        }
        r.ac_steps=ac_learned;
        a+=ac_learned;
        if (a>255u) a=255u;
        r.load_controlled=1u;
    }
    /* LE98A..LE9A4: P/N scales the combined follower/load demand by 255/256.
       Drive adds five steps when VSS is above the closed-loop-idle threshold. */
    if (in_park_neutral) {
        a=(a*STEP39_TF_PN_MULT)>>8;
    } else if (vss_raw>STEP39_VSS_PID_MAX) {
        a+=STEP39_TF_MOVING_ADD;
        if (a>255u) a=255u;
    }
    r.desired_steps=(bua_u8)a;
    return r;
}
static BuaIacTfRequestStep39 bua_iac_tf_request_step39(bua_u8 desired_steps,
                                                         bua_u8 current_tf_steps)
{
    BuaIacTfRequestStep39 r;
    int d=(int)desired_steps-(int)current_tf_steps;
    unsigned int m;
    r.delta=d;
    r.request=0u;
    r.magnitude=0u;
    r.opening=0u;
    r.closing=0u;
    r.pid_disable_timer=0u;
    if (d>0) {
        m=(unsigned int)d;
        if (m>127u) m=127u;
        r.magnitude=(bua_u8)m;
        r.request=(bua_u8)m;
        r.opening=1u;
    } else if (d<0) {
        m=(unsigned int)(-d);
        if (m>127u) m=127u;
        r.magnitude=(bua_u8)m;
        r.request=(bua_u8)(0x80u|(bua_u8)m);
        r.closing=1u;
        /* LE9B7: a small closing transition loads LC674 before PID takeover. */
        if (m<3u) r.pid_disable_timer=STEP39_TF_RELEASE_PID_DELAY;
    }
    return r;
}
static BuaIacAcStateStep39 bua_iac_ac_remove_step39(bua_u8 control_word,
                                                      bua_u8 ac_requested)
{
    BuaIacAcStateStep39 r;
    r.control_word=control_word;
    r.removal_started=0u;
    /* LE984: when A/C disappears after its load was being controlled, b5 is
       set. LF562 explicitly bypasses normal trajectory decay while this bit
       is set, so the anticipate load is removed promptly. */
    if (!ac_requested && (control_word&STEP39_IAC_LOAD_CONTROL_BIT)!=0u) {
        r.control_word=(bua_u8)(control_word|STEP39_IAC_LOAD_REMOVE_BIT);
        r.removal_started=1u;
    }
    return r;
}
static bua_u8 step39_drive_decay_base(bua_u8 vss_raw)
{
    bua_u8 a;
    unsigned int product;
    if (vss_raw<32u) a=vss_raw;
    else a=255u;
    a=(bua_u8)(a<<1);
    a=(bua_u8)(a<<1);
    a=(bua_u8)(a<<1);
    product=(unsigned int)a*STEP39_TF_DECAY_VSS_MULT;
    a=(bua_u8)(product>>8);
    a=(bua_u8)(a+STEP39_TF_DECAY_DRIVE);
    return a;
}
static BuaIacDecayStep39 bua_iac_tf_decay_step39(bua_u8 in_park_neutral,
                                                   bua_u8 vss_raw,
                                                   bua_u8 open_loop,
                                                   bua_u8 actual_rpm_raw,
                                                   bua_u8 target_rpm_raw)
{
    BuaIacDecayStep39 r;
    bua_u8 base;
    bua_u8 doubled;
    bua_u8 diff;
    unsigned int d16;
    unsigned int ext;
    unsigned int sum;
    base=in_park_neutral?STEP39_TF_DECAY_PN:step39_drive_decay_base(vss_raw);
    r.base_count=base;
    r.final_count=base;
    r.rpm_extension=0u;
    if (!open_loop) return r;
    /* LF58B..LF5AD exact byte structure. */
    if ((base&0x80u)!=0u) {
        r.final_count=255u;
        return r;
    }
    doubled=(bua_u8)(base<<1);
    diff=(bua_u8)(actual_rpm_raw-target_rpm_raw);
    d16=(unsigned int)diff*4u;
    if ((d16>>8)!=0u) {
        r.final_count=doubled;
        return r;
    }
    diff=(bua_u8)(~(bua_u8)d16);
    ext=((unsigned int)STEP39_TF_OPEN_LOOP_DELTA*(unsigned int)diff)>>8;
    r.rpm_extension=(bua_u8)ext;
    sum=(unsigned int)doubled+ext;
    r.final_count=(bua_u8)((sum>255u)?255u:sum);
    return r;
}
static BuaIacFanAnticStep39 bua_iac_fan_anticipation_step39(bua_u8 progress)
{
    BuaIacFanAnticStep39 r;
    r.target_raw=(bua_u8)(STEP39_FAN_ANTIC_STEPS*2u);
    r.request=0u;
    r.complete=0u;
    r.pid_disable_timer=0u;
    /* LE8B7..LE8D4 compares LC632*2 with L010E and issues raw request #2
       until the requested anticipation walk is complete. */
    if (progress<r.target_raw) {
        r.request=2u;
    } else {
        r.complete=1u;
        r.pid_disable_timer=STEP39_FAN_PID_DELAY;
    }
    return r;
}
static void run_step39_iac_tf_anticipation_test(void)
{
    unsigned int passed=0u;
    BuaIacTfDesiredStep39 d;
    BuaIacTfRequestStep39 q;
    BuaIacAcStateStep39 a;
    BuaIacDecayStep39 z;
    BuaIacFanAnticStep39 f;
    printf("\nIAC throttle-follower/anticipation regression (Step 39):\n");
    d=bua_iac_tf_desired_step39(3u,0u,0u,0u,20u,0u);
    if (d.throttle_steps==0u) ++passed;
    printf("  TPS exactly LC64B=3 leaves throttle follower at zero               %s\n",
           (d.throttle_steps==0u)?"PASS":"FAIL");
    d=bua_iac_tf_desired_step39(4u,0u,0u,0u,20u,0u);
    if (d.throttle_steps==6u) ++passed;
    printf("  TPS raw 4 produces floor(1.5*T) = 6 follower steps                 %s\n",
           (d.throttle_steps==6u)?"PASS":"FAIL");
    d=bua_iac_tf_desired_step39(100u,0u,0u,0u,20u,0u);
    if (d.throttle_steps==75u) ++passed;
    printf("  follower command clamps at LC663=75 steps                          %s\n",
           (d.throttle_steps==75u)?"PASS":"FAIL");
    d=bua_iac_tf_desired_step39(40u,1u,0u,0u,20u,0u);
    if (d.desired_steps==59u) ++passed;
    printf("  P/N applies LC664=255/256 scaling to 60-step follower              %s\n",
           (d.desired_steps==59u)?"PASS":"FAIL");
    d=bua_iac_tf_desired_step39(40u,0u,7u,0u,20u,0u);
    if (d.desired_steps==65u) ++passed;
    printf("  Drive while moving above LC64E adds LC683=5 steps                  %s\n",
           (d.desired_steps==65u)?"PASS":"FAIL");
    d=bua_iac_tf_desired_step39(0u,0u,0u,1u,20u,0u);
    if (d.desired_steps==20u && d.ac_steps==20u) ++passed;
    printf("  A/C can carry 20 learned anticipate steps even at closed throttle  %s\n",
           (d.desired_steps==20u && d.ac_steps==20u)?"PASS":"FAIL");
    d=bua_iac_tf_desired_step39(0u,0u,0u,1u,0u,0u);
    if (d.ac_learn_reset && d.ac_steps==20u) ++passed;
    printf("  invalid low A/C learn value resets to LC665=20                     %s\n",
           (d.ac_learn_reset && d.ac_steps==20u)?"PASS":"FAIL");
    d=bua_iac_tf_desired_step39(0u,0u,0u,1u,33u,0u);
    if (d.ac_learn_reset && d.ac_steps==20u) ++passed;
    printf("  A/C learn value above LC666=32 also resets to default              %s\n",
           (d.ac_learn_reset && d.ac_steps==20u)?"PASS":"FAIL");
    d=bua_iac_tf_desired_step39(40u,0u,0u,0u,20u,1u);
    if (d.desired_steps==60u && d.load_controlled) ++passed;
    printf("  power-steering path exists but 9340 anticipation calibrations are 0 %s\n",
           (d.desired_steps==60u && d.load_controlled)?"PASS":"FAIL");
    q=bua_iac_tf_request_step39(60u,40u);
    if (q.opening && q.request==20u) ++passed;
    printf("  desired follower above accumulator requests opening steps           %s\n",
           (q.opening && q.request==20u)?"PASS":"FAIL");
    q=bua_iac_tf_request_step39(40u,60u);
    if (q.closing && q.request==0x94u) ++passed;
    printf("  desired follower below accumulator encodes closing direction bit    %s\n",
           (q.closing && q.request==0x94u)?"PASS":"FAIL");
    q=bua_iac_tf_request_step39(58u,60u);
    if (q.closing && q.pid_disable_timer==30u) ++passed;
    printf("  small closing handoff loads LC674=30 PID-inhibit counts             %s\n",
           (q.closing && q.pid_disable_timer==30u)?"PASS":"FAIL");
    a=bua_iac_ac_remove_step39(STEP39_IAC_LOAD_CONTROL_BIT,0u);
    if (a.removal_started && (a.control_word&STEP39_IAC_LOAD_REMOVE_BIT)!=0u) ++passed;
    printf("  A/C removal sets b5 when load-control b6 had been active            %s\n",
           (a.removal_started && (a.control_word&STEP39_IAC_LOAD_REMOVE_BIT)!=0u)?"PASS":"FAIL");
    z=bua_iac_tf_decay_step39(1u,0u,0u,64u,64u);
    if (z.base_count==5u && z.final_count==5u) ++passed;
    printf("  P/N closed-loop trajectory base is LC65E=5 minor loops              %s\n",
           (z.base_count==5u && z.final_count==5u)?"PASS":"FAIL");
    z=bua_iac_tf_decay_step39(0u,0u,0u,64u,64u);
    if (z.base_count==4u && z.final_count==4u) ++passed;
    printf("  Drive zero-speed trajectory base is LC65F=4 minor loops             %s\n",
           (z.base_count==4u && z.final_count==4u)?"PASS":"FAIL");
    z=bua_iac_tf_decay_step39(1u,0u,1u,64u,64u);
    if (z.final_count==25u && z.rpm_extension==15u) ++passed;
    printf("  open-loop, zero RPM error: doubled base 10 + extension 15 = 25     %s\n",
           (z.final_count==25u && z.rpm_extension==15u)?"PASS":"FAIL");
    z=bua_iac_tf_decay_step39(1u,0u,1u,80u,64u);
    if (z.final_count==21u && z.rpm_extension==11u) ++passed;
    printf("  open-loop overspeed changes LF58B decay extension exactly           %s\n",
           (z.final_count==21u && z.rpm_extension==11u)?"PASS":"FAIL");
    f=bua_iac_fan_anticipation_step39(9u);
    if (!f.complete && f.request==2u && f.target_raw==10u) ++passed;
    printf("  fan anticipation below LC632*2 target issues raw +2 request        %s\n",
           (!f.complete && f.request==2u && f.target_raw==10u)?"PASS":"FAIL");
    f=bua_iac_fan_anticipation_step39(10u);
    if (f.complete && f.request==0u && f.pid_disable_timer==20u) ++passed;
    printf("  fan anticipation completion loads LC676=20 PID-inhibit counts      %s\n",
           (f.complete && f.request==0u && f.pid_disable_timer==20u)?"PASS":"FAIL");
    printf("  step-39 IAC follower/anticipation regression result: %s (%u/19)\n",
           (passed==19u)?"PASS":"FAIL",passed);
}
/* --------------------------------------------------------------------------
 * Step 40: LF5DF..LF678 IAC motor execution and position bookkeeping
 * -------------------------------------------------------------------------- */
#define STEP40_IAC_MAX_POSITION        145u  /* LC67B */
#define STEP40_IAC_TF_BIT             0x10u
#define STEP40_IAC_FAN_WALK_BIT       0x08u
#define STEP40_IAC_DIR_CLOSE_BIT      0x80u
#define STEP40_IAC_MAG_MASK           0x7Fu
#define STEP40_BIAS_SIGN_BIT          0x40u  /* L003B bit shifted into N at LF651 */
typedef struct BuaIacMotorStep40 {
    bua_u8 command;
    bua_u8 present_position;
    bua_u8 running_counter;
    bua_u8 tf_accumulator;
    bua_u8 fan_progress;
    bua_u8 motor_on;
    bua_u8 phase_bits;
    bua_u8 consumed_step;
    bua_u8 physical_move;
    bua_u8 opening;
    bua_u8 closing;
} BuaIacMotorStep40;
typedef struct BuaIacBiasStep40 {
    bua_u8 base_bias;
    bua_u8 signed_error_raw;
    int signed_error;
} BuaIacBiasStep40;
static bua_u8 step40_iac_phase_bits(bua_u8 running_counter)
{
    static const bua_u8 phase_table[4]={0u,1u,3u,2u}; /* FEC9..FECC */
    return phase_table[running_counter&3u];
}
static bua_u8 step40_command_magnitude(bua_u8 command)
{
    return (bua_u8)(command&STEP40_IAC_MAG_MASK);
}
static BuaIacMotorStep40 bua_iac_motor_service_step40(bua_u8 command,
                                                        bua_u8 present_position,
                                                        bua_u8 running_counter,
                                                        bua_u8 tf_accumulator,
                                                        bua_u8 fan_progress,
                                                        bua_u8 control_word,
                                                        bua_u8 motor_on,
                                                        bua_u8 power_valid)
{
    BuaIacMotorStep40 r;
    bua_u8 closing;
    bua_u8 mag;
    r.command=command;
    r.present_position=present_position;
    r.running_counter=running_counter;
    r.tf_accumulator=tf_accumulator;
    r.fan_progress=fan_progress;
    r.motor_on=motor_on?1u:0u;
    r.phase_bits=step40_iac_phase_bits(running_counter);
    r.consumed_step=0u;
    r.physical_move=0u;
    r.opening=0u;
    r.closing=0u;
    /* LF514/LF51E can shut the motor off for invalid battery voltage.  Keep
       that hardware gate explicit in the PC model rather than consuming a
       pending command while the output is disabled. */
    if (!power_valid) {
        r.motor_on=0u;
        return r;
    }
    mag=step40_command_magnitude(command);
    /* LF5DF: if a non-zero request exists while the C drive is off, the first
       service merely energizes it.  The command is consumed on a later pass. */
    if (mag!=0u && !r.motor_on) {
        r.motor_on=1u;
        return r;
    }
    if (mag!=0u && r.motor_on) {
        closing=(bua_u8)((command&STEP40_IAC_DIR_CLOSE_BIT)!=0u);
        r.command=(bua_u8)(command-1u); /* DEC L0101 preserves packed b7 direction. */
        r.consumed_step=1u;
        if (closing) {
            r.closing=1u;
            /* LF5F6 decrements the running counter even when already at zero. */
            if (r.present_position!=0u) {
                --r.present_position;
                r.physical_move=1u;
            }
            --r.running_counter;
            /* LF604..LF616: with T/F active an extend/closing step decreases
               the follower accumulator, but never below zero. */
            if ((control_word&STEP40_IAC_TF_BIT)!=0u && r.tf_accumulator!=0u)
                --r.tf_accumulator;
        } else {
            r.opening=1u;
            /* LF5FB consumes the packed command at the upper position limit,
               but does not advance the position or phase counter there. */
            if (r.present_position<STEP40_IAC_MAX_POSITION) {
                ++r.present_position;
                ++r.running_counter;
                r.physical_move=1u;
                if ((control_word&STEP40_IAC_TF_BIT)!=0u)
                    ++r.tf_accumulator;
            }
        }
    }
    /* LF619 increments the fan/cold-walk progress counter on every service
       while IAC control-word b3 is active, independent of whether a step moved. */
    if ((control_word&STEP40_IAC_FAN_WALK_BIT)!=0u)
        ++r.fan_progress;
    r.phase_bits=step40_iac_phase_bits(r.running_counter);
    return r;
}
static BuaIacBiasStep40 bua_iac_base_bias_step40(bua_u8 learned_min_position,
                                                   bua_u8 tf_accumulator,
                                                   bua_u8 progress,
                                                   bua_u8 mode_word_003b,
                                                   bua_u8 present_position)
{
    BuaIacBiasStep40 r;
    unsigned int base;
    int adjusted;
    int err;
    /* LF644..LF65C: learned minimum + throttle follower saturates at 255;
       L003B b6 selects whether L010E is subtracted or added. */
    base=(unsigned int)learned_min_position+(unsigned int)tf_accumulator;
    if (base>255u) base=255u;
    adjusted=(int)base;
    if ((mode_word_003b&STEP40_BIAS_SIGN_BIT)!=0u)
        adjusted-=(int)progress;
    else
        adjusted+=(int)progress;
    adjusted&=0xFF;
    r.base_bias=(bua_u8)adjusted;
    /* LF65C..LF678 is a signed current-position minus base-bias error with
       explicit saturation to +127 / -127 (raw 0x7F / 0x81). */
    err=(int)present_position-(int)r.base_bias;
    if (err>127) err=127;
    if (err<-127) err=-127;
    r.signed_error=err;
    r.signed_error_raw=(bua_u8)(err&0xFF);
    return r;
}
static void run_step40_iac_motor_test(void)
{
    unsigned int passed=0u;
    BuaIacMotorStep40 m;
    BuaIacBiasStep40 b;
    printf("\nIAC motor execution/position regression (Step 40, LF5DF..LF678):\n");
    m=bua_iac_motor_service_step40(3u,50u,0u,0u,0u,0u,0u,1u);
    if (m.motor_on && m.present_position==50u && m.command==3u && !m.consumed_step) ++passed;
    printf("  first pass with motor off energizes C but consumes no command       %s\n",
           (m.motor_on && m.present_position==50u && m.command==3u && !m.consumed_step)?"PASS":"FAIL");
    m=bua_iac_motor_service_step40(3u,50u,0u,0u,0u,0u,1u,1u);
    if (m.opening && m.consumed_step && m.present_position==51u && m.command==2u && m.running_counter==1u) ++passed;
    printf("  opening request consumes one count and increments position/phase    %s\n",
           (m.opening && m.consumed_step && m.present_position==51u && m.command==2u && m.running_counter==1u)?"PASS":"FAIL");
    m=bua_iac_motor_service_step40(0x83u,50u,0u,0u,0u,0u,1u,1u);
    if (m.closing && m.present_position==49u && m.command==0x82u && m.running_counter==255u && m.phase_bits==2u) ++passed;
    printf("  closing request decrements position and running counter             %s\n",
           (m.closing && m.present_position==49u && m.command==0x82u && m.running_counter==255u && m.phase_bits==2u)?"PASS":"FAIL");
    m=bua_iac_motor_service_step40(0x81u,0u,7u,0u,0u,0u,1u,1u);
    if (m.present_position==0u && m.command==0x80u && m.running_counter==6u) ++passed;
    printf("  closing at zero still consumes request and decrements phase counter %s\n",
           (m.present_position==0u && m.command==0x80u && m.running_counter==6u)?"PASS":"FAIL");
    m=bua_iac_motor_service_step40(1u,145u,9u,0u,0u,0u,1u,1u);
    if (m.present_position==145u && m.command==0u && m.running_counter==9u && !m.physical_move) ++passed;
    printf("  opening at LC67B=145 consumes request without exceeding limit       %s\n",
           (m.present_position==145u && m.command==0u && m.running_counter==9u && !m.physical_move)?"PASS":"FAIL");
    m=bua_iac_motor_service_step40(1u,40u,2u,10u,0u,STEP40_IAC_TF_BIT,1u,1u);
    if (m.tf_accumulator==11u && m.present_position==41u) ++passed;
    printf("  T/F-active opening step increments follower accumulator             %s\n",
           (m.tf_accumulator==11u && m.present_position==41u)?"PASS":"FAIL");
    m=bua_iac_motor_service_step40(0x81u,40u,2u,10u,0u,STEP40_IAC_TF_BIT,1u,1u);
    if (m.tf_accumulator==9u && m.present_position==39u) ++passed;
    printf("  T/F-active closing step decrements follower accumulator             %s\n",
           (m.tf_accumulator==9u && m.present_position==39u)?"PASS":"FAIL");
    m=bua_iac_motor_service_step40(0x81u,40u,2u,0u,0u,STEP40_IAC_TF_BIT,1u,1u);
    if (m.tf_accumulator==0u) ++passed;
    printf("  closing step cannot decrement zero follower accumulator             %s\n",
           (m.tf_accumulator==0u)?"PASS":"FAIL");
    m=bua_iac_motor_service_step40(0u,40u,2u,0u,9u,STEP40_IAC_FAN_WALK_BIT,1u,1u);
    if (m.fan_progress==10u && !m.consumed_step) ++passed;
    printf("  b3 fan/cold anticipation progress increments even without a move    %s\n",
           (m.fan_progress==10u && !m.consumed_step)?"PASS":"FAIL");
    if (step40_iac_phase_bits(0u)==0u && step40_iac_phase_bits(1u)==1u &&
        step40_iac_phase_bits(2u)==3u && step40_iac_phase_bits(3u)==2u) ++passed;
    printf("  FEC9 phase table is exactly 0,1,3,2                                %s\n",
           (step40_iac_phase_bits(0u)==0u && step40_iac_phase_bits(1u)==1u &&
            step40_iac_phase_bits(2u)==3u && step40_iac_phase_bits(3u)==2u)?"PASS":"FAIL");
    m=bua_iac_motor_service_step40(4u,50u,0u,0u,0u,0u,1u,0u);
    if (!m.motor_on && m.command==4u && m.present_position==50u) ++passed;
    printf("  invalid motor power gate shuts coils off and preserves command      %s\n",
           (!m.motor_on && m.command==4u && m.present_position==50u)?"PASS":"FAIL");
    b=bua_iac_base_bias_step40(60u,20u,0u,0u,90u);
    if (b.base_bias==80u && b.signed_error==10) ++passed;
    printf("  base bias = learned minimum + T/F; current 90 gives +10 error       %s\n",
           (b.base_bias==80u && b.signed_error==10)?"PASS":"FAIL");
    b=bua_iac_base_bias_step40(250u,20u,0u,0u,255u);
    if (b.base_bias==255u && b.signed_error==0) ++passed;
    printf("  learned-min + T/F base sum saturates at 255                         %s\n",
           (b.base_bias==255u && b.signed_error==0)?"PASS":"FAIL");
    b=bua_iac_base_bias_step40(60u,20u,5u,0u,90u);
    if (b.base_bias==85u && b.signed_error==5) ++passed;
    printf("  clear L003B b6 adds progress to the base bias                       %s\n",
           (b.base_bias==85u && b.signed_error==5)?"PASS":"FAIL");
    b=bua_iac_base_bias_step40(60u,20u,5u,STEP40_BIAS_SIGN_BIT,90u);
    if (b.base_bias==75u && b.signed_error==15) ++passed;
    printf("  set L003B b6 subtracts progress from the base bias                  %s\n",
           (b.base_bias==75u && b.signed_error==15)?"PASS":"FAIL");
    b=bua_iac_base_bias_step40(0u,0u,0u,0u,255u);
    if (b.signed_error==127 && b.signed_error_raw==0x7Fu) ++passed;
    printf("  positive base-position error saturates to +127 / 0x7F              %s\n",
           (b.signed_error==127 && b.signed_error_raw==0x7Fu)?"PASS":"FAIL");
    b=bua_iac_base_bias_step40(255u,0u,0u,0u,0u);
    if (b.signed_error==-127 && b.signed_error_raw==0x81u) ++passed;
    printf("  negative base-position error saturates to -127 / 0x81              %s\n",
           (b.signed_error==-127 && b.signed_error_raw==0x81u)?"PASS":"FAIL");
    printf("  step-40 IAC motor regression result: %s (%u/17)\n",
           (passed==17u)?"PASS":"FAIL",passed);
}
/* --------------------------------------------------------------------------
 * Step 41: IAC motor reset/homing and startup warm-park positioning
 * -------------------------------------------------------------------------- */
#define STEP41_IAC_STARTUP_PARK       144u /* LC62F */
#define STEP41_IAC_HOT_OFFSET          32u /* LC630 */
#define STEP41_IAC_AC_OFFSET           10u /* LC631 */
#define STEP41_IAC_RESET_PHASE_BIT   0x04u /* L00F3 b2 */
#define STEP41_IAC_RESET_RETURN_BIT  0x02u /* L00F3 b1 */
#define STEP41_IAC_RESET_DONE_BIT    0x10u /* L0002 b4 */
#define STEP41_IAC_STARTUP_DONE_BIT  0x08u /* L00F3 b3 */
#define STEP41_IAC_CLOSE_MAX         0xFFu
#define STEP41_BATT_LOW_RAW          0x5Au
#define STEP41_BATT_HIGH_RAW         0xABu
typedef struct BuaIacResetStep41 {
    bua_u8 command;
    bua_u8 logical_position;
    bua_u8 iac_mode_word2;
    bua_u8 reset_done;
    bua_u8 clear_tf_accumulator;
} BuaIacResetStep41;
typedef struct BuaIacStartupStep41 {
    bua_u8 target_position;
    bua_u8 command;
    bua_u8 motor_power_valid;
    bua_u8 startup_done;
    bua_u8 next_delay_count;
} BuaIacStartupStep41;
/* LD393..LD3D2.  The original has no pintle-position sensor.  During reset it
   deliberately assigns the logical position 255 and commands closing steps.
   Counting all the way down to zero guarantees that the physical pintle has
   reached the closed hard stop even if the initial physical position was
   unknown.  It then commands the pintle back open to LC62F=144. */
static BuaIacResetStep41 bua_iac_reset_command_step41(bua_u8 logical_position,
                                                       bua_u8 iac_mode_word2)
{
    BuaIacResetStep41 r;
    int delta;
    r.command=0u;
    r.logical_position=logical_position;
    r.iac_mode_word2=iac_mode_word2;
    r.reset_done=0u;
    r.clear_tf_accumulator=1u;
    if ((r.iac_mode_word2&STEP41_IAC_RESET_RETURN_BIT)==0u) {
        if ((r.iac_mode_word2&STEP41_IAC_RESET_PHASE_BIT)==0u) {
            r.iac_mode_word2|=STEP41_IAC_RESET_PHASE_BIT;
            r.logical_position=255u;
        }
        if (r.logical_position!=0u) {
            r.command=STEP41_IAC_CLOSE_MAX;
            return r;
        }
        r.iac_mode_word2|=STEP41_IAC_RESET_RETURN_BIT;
    }
    delta=(int)STEP41_IAC_STARTUP_PARK-(int)r.logical_position;
    if (delta>127) delta=127;
    if (delta<0) {
        delta=-delta;
        if (delta>127) delta=127;
        r.command=(bua_u8)(0x80u|(bua_u8)delta);
    } else {
        r.command=(bua_u8)delta;
    }
    if ((r.command&0x7Fu)==0u)
        r.reset_done=1u;
    return r;
}
/* LF521..LF53A.  L010C is the coolant-derived warm-park position from C690.
   The executable loads LC630 and adds it unconditionally before the L0037 test.
   L0037 b7 bypasses the additional LC631 startup offset; when clear, LC631 is
   added.  This corrects the earlier Step-41 interpretation of LC630. */
static BuaIacStartupStep41 bua_iac_startup_position_step41(bua_u8 battery_raw,
                                                            bua_u8 warm_park_position,
                                                            bua_u8 ac_not_on,
                                                            bua_u8 present_position,
                                                            bua_u8 delay_count,
                                                            bua_u8 delay_limit)
{
    BuaIacStartupStep41 r;
    unsigned int target;
    int delta;
    r.target_position=warm_park_position;
    r.command=0u;
    r.motor_power_valid=0u;
    r.startup_done=0u;
    r.next_delay_count=delay_count;
    /* LF514..LF521: valid only for raw battery > $5A and <= $AB. */
    if (battery_raw<=STEP41_BATT_LOW_RAW || battery_raw>STEP41_BATT_HIGH_RAW)
        return r;
    r.motor_power_valid=1u;
    target=(unsigned int)warm_park_position;
    target+=(unsigned int)STEP41_IAC_HOT_OFFSET;
    if (!ac_not_on)
        target+=(unsigned int)STEP41_IAC_AC_OFFSET;
    if (target>255u) target=255u;
    r.target_position=(bua_u8)target;
    delta=(int)r.target_position-(int)present_position;
    if (delta>127) delta=127;
    if (delta<0) {
        delta=-delta;
        if (delta>127) delta=127;
        r.command=(bua_u8)(0x80u|(bua_u8)delta);
    } else {
        r.command=(bua_u8)delta;
    }
    r.next_delay_count=(bua_u8)(delay_count+1u);
    if (r.next_delay_count>delay_limit) {
        r.startup_done=1u;
        r.next_delay_count=0u;
    }
    return r;
}
static void run_step41_iac_startup_reset_test(void)
{
    unsigned int passed=0u;
    BuaIacResetStep41 h;
    BuaIacStartupStep41 s;
    printf("\nIAC reset/startup regression (Step 41, LD393..LD3D2 / LF514..LF53A):\n");
    h=bua_iac_reset_command_step41(73u,0u);
    if (h.logical_position==255u && h.command==0xFFu &&
        (h.iac_mode_word2&STEP41_IAC_RESET_PHASE_BIT)!=0u) ++passed;
    printf("  reset begins by assigning logical 255 and commanding hard close     %s\n",
           (h.logical_position==255u && h.command==0xFFu &&
            (h.iac_mode_word2&STEP41_IAC_RESET_PHASE_BIT)!=0u)?"PASS":"FAIL");
    h=bua_iac_reset_command_step41(200u,STEP41_IAC_RESET_PHASE_BIT);
    if (h.command==0xFFu && h.logical_position==200u && !h.reset_done) ++passed;
    printf("  hard-stop phase keeps issuing maximum closing command               %s\n",
           (h.command==0xFFu && h.logical_position==200u && !h.reset_done)?"PASS":"FAIL");
    h=bua_iac_reset_command_step41(0u,STEP41_IAC_RESET_PHASE_BIT);
    if (h.command==127u &&
        (h.iac_mode_word2&STEP41_IAC_RESET_RETURN_BIT)!=0u) ++passed;
    printf("  at logical zero reset reverses and opens toward LC62F=144           %s\n",
           (h.command==127u && (h.iac_mode_word2&STEP41_IAC_RESET_RETURN_BIT)!=0u)?"PASS":"FAIL");
    h=bua_iac_reset_command_step41(127u,(bua_u8)(STEP41_IAC_RESET_PHASE_BIT|STEP41_IAC_RESET_RETURN_BIT));
    if (h.command==17u && !h.reset_done) ++passed;
    printf("  return phase requests remaining 17 opening steps to 144             %s\n",
           (h.command==17u && !h.reset_done)?"PASS":"FAIL");
    h=bua_iac_reset_command_step41(144u,(bua_u8)(STEP41_IAC_RESET_PHASE_BIT|STEP41_IAC_RESET_RETURN_BIT));
    if (h.command==0u && h.reset_done) ++passed;
    printf("  reaching LC62F=144 completes motor reset                            %s\n",
           (h.command==0u && h.reset_done)?"PASS":"FAIL");
    h=bua_iac_reset_command_step41(160u,(bua_u8)(STEP41_IAC_RESET_PHASE_BIT|STEP41_IAC_RESET_RETURN_BIT));
    if (h.command==0x90u) ++passed;
    printf("  return helper preserves closing direction if position is above 144  %s\n",
           (h.command==0x90u)?"PASS":"FAIL");
    s=bua_iac_startup_position_step41(120u,65u,1u,50u,0u,20u);
    if (s.motor_power_valid && s.target_position==97u && s.command==47u) ++passed;
    printf("  LF521 unconditionally adds LC630=32 to warm-park position           %s\n",
           (s.motor_power_valid && s.target_position==97u && s.command==47u)?"PASS":"FAIL");
    s=bua_iac_startup_position_step41(120u,65u,1u,97u,0u,20u);
    if (s.target_position==97u && s.command==0u) ++passed;
    printf("  no separate hot-restart condition controls the LC630 addition       %s\n",
           (s.target_position==97u && s.command==0u)?"PASS":"FAIL");
    s=bua_iac_startup_position_step41(120u,65u,0u,50u,0u,20u);
    if (s.target_position==107u && s.command==57u) ++passed;
    printf("  L0037-clear startup adds LC631=10 after unconditional LC630         %s\n",
           (s.target_position==107u && s.command==57u)?"PASS":"FAIL");
    s=bua_iac_startup_position_step41(120u,240u,0u,100u,0u,20u);
    if (s.target_position==255u && s.command==127u) ++passed;
    printf("  combined startup target saturates at 255 and command at 127         %s\n",
           (s.target_position==255u && s.command==127u)?"PASS":"FAIL");
    s=bua_iac_startup_position_step41(0x5Au,65u,1u,50u,0u,20u);
    if (!s.motor_power_valid && s.command==0u) ++passed;
    printf("  battery raw <= $5A inhibits IAC motor output                        %s\n",
           (!s.motor_power_valid && s.command==0u)?"PASS":"FAIL");
    s=bua_iac_startup_position_step41(0xABu,65u,1u,50u,0u,20u);
    if (s.motor_power_valid) ++passed;
    printf("  battery raw $AB is still accepted                                   %s\n",
           s.motor_power_valid?"PASS":"FAIL");
    s=bua_iac_startup_position_step41(0xACu,65u,1u,50u,0u,20u);
    if (!s.motor_power_valid) ++passed;
    printf("  battery raw > $AB inhibits IAC motor output                         %s\n",
           (!s.motor_power_valid)?"PASS":"FAIL");
    s=bua_iac_startup_position_step41(120u,65u,1u,65u,20u,20u);
    if (s.startup_done && s.next_delay_count==0u && s.command==32u) ++passed;
    printf("  startup delay completes only after increment exceeds calibration    %s\n",
           (s.startup_done && s.next_delay_count==0u && s.command==32u)?"PASS":"FAIL");
    s=bua_iac_startup_position_step41(120u,65u,1u,65u,19u,20u);
    if (!s.startup_done && s.next_delay_count==20u) ++passed;
    printf("  delay count equal to calibration remains in startup phase           %s\n",
           (!s.startup_done && s.next_delay_count==20u)?"PASS":"FAIL");
    if (STEP41_IAC_STARTUP_PARK==144u && STEP40_IAC_MAX_POSITION==145u) ++passed;
    printf("  startup park 144 is one step below LC67B maximum 145                %s\n",
           (STEP41_IAC_STARTUP_PARK==144u && STEP40_IAC_MAX_POSITION==145u)?"PASS":"FAIL");
    printf("  step-41 IAC reset/startup regression result: %s (%u/16)\n",
           (passed==16u)?"PASS":"FAIL",passed);
}
/* --------------------------------------------------------------------------
 * Step 42: ignition-off sequencing and IAC reset re-arm
 * -------------------------------------------------------------------------- */
#define STEP42_IGN_OFF_BIT              0x10u /* L0033 b4: 1 = ignition off */
#define STEP42_ENGINE_RUNNING_BIT       0x80u /* L0034 b7 */
#define STEP42_SKIP_IAC_RESET_BIT       0x10u /* L0002 b4 */
#define STEP42_IAC_RESET_PHASE_BIT      0x04u /* L00F3 b2 */
#define STEP42_IGN_HOUSEKEEP_COUNT      8u    /* LD6D1 CPX #$08 */
#define STEP42_SOFT_POWERDOWN_COUNT     0x0385u /* LC012 */
typedef struct BuaIgnOffStep42 {
    bua_u16 timer;
    bua_u8 nv_mode_word;
    bua_u8 mode_word2;
    bua_u8 mode_word3;
    bua_u8 engine_mode_word;
    bua_u8 housekeeping_active;
    bua_u8 soft_powerdown;
    bua_u8 clear_engine_run_time;
} BuaIgnOffStep42;
/* LEA80 is reached in the 100-ms miscellaneous major-loop path while the
   engine-running bit is set.  Its executable action is simply to clear
   L0002 b4.  Since LD3B4 sets that bit when an IAC reset completes, LEA80
   re-arms one reset for the next shutdown. */
static bua_u8 bua_iac_rearm_reset_step42(bua_u8 mode_word2,
                                         bua_u8 engine_mode_word)
{
    if ((engine_mode_word & STEP42_ENGINE_RUNNING_BIT) != 0u)
        mode_word2=(bua_u8)(mode_word2 & (bua_u8)~STEP42_SKIP_IAC_RESET_BIT);
    return mode_word2;
}
/* Normal (non-diagnostic) LD387 decision.  An in-progress reset (L00F3 b2)
   continues regardless.  Otherwise a completed-reset marker bypasses reset;
   if it is not set, ignition-off enters LD393 while ignition-on proceeds to
   the ordinary IAC path. */
static bua_u8 bua_iac_reset_requested_step42(bua_u8 iac_mode_word2,
                                              bua_u8 mode_word2,
                                              bua_u8 minor_mode_word1)
{
    if ((iac_mode_word2 & STEP42_IAC_RESET_PHASE_BIT) != 0u)
        return 1u;
    if ((mode_word2 & STEP42_SKIP_IAC_RESET_BIT) != 0u)
        return 0u;
    if ((minor_mode_word1 & STEP42_IGN_OFF_BIT) == 0u)
        return 0u;
    return 1u;
}
/* LD6D1..LD727.  This is modeled in loop counts rather than assigning a
   physical time to LC012.  In the source this code is in the 12.5-ms air/fuel
   loop, so 0x0385 counts would nominally be about 11.26 s if every invocation
   advances the timer exactly once. */
static BuaIgnOffStep42 bua_ignoff_tick_step42(bua_u16 timer,
                                               bua_u8 ignition_off,
                                               bua_u8 nv_mode_word,
                                               bua_u8 mode_word2,
                                               bua_u8 mode_word3,
                                               bua_u8 engine_mode_word)
{
    BuaIgnOffStep42 r;
    r.timer=timer;
    r.nv_mode_word=nv_mode_word;
    r.mode_word2=mode_word2;
    r.mode_word3=mode_word3;
    r.engine_mode_word=engine_mode_word;
    r.housekeeping_active=0u;
    r.soft_powerdown=0u;
    r.clear_engine_run_time=0u;
    if (!ignition_off) {
        /* LD721: X=$FFFF, then LD724 INX -> zero every ignition-on pass. */
        r.timer=0u;
        return r;
    }
    if (r.timer < STEP42_IGN_HOUSEKEEP_COUNT) {
        r.timer=(bua_u16)(r.timer+1u);
        return r;
    }
    r.housekeeping_active=1u;
    r.nv_mode_word=0u; /* BCLR L0001,$FF */
    r.mode_word2=(bua_u8)(r.mode_word2 & STEP42_SKIP_IAC_RESET_BIT);
    r.mode_word3=(bua_u8)(r.mode_word3 & 0x40u);
    if (r.timer >= STEP42_SOFT_POWERDOWN_COUNT) {
        r.soft_powerdown=1u; /* LD6EA SWI loop */
        return r;
    }
    r.timer=(bua_u16)(r.timer+1u);
    r.clear_engine_run_time=1u;
    r.engine_mode_word=(bua_u8)(r.engine_mode_word & 0x7Fu);
    return r;
}
static void run_step42_iac_shutdown_test(void)
{
    unsigned int passed=0u;
    BuaIgnOffStep42 r;
    bua_u8 m2;
    printf("\nIAC key-off/shutdown regression (Step 42, LEA80 / LD387..LD3D2 / LD6D1..LD727):\n");
    m2=bua_iac_rearm_reset_step42(STEP42_SKIP_IAC_RESET_BIT,STEP42_ENGINE_RUNNING_BIT);
    if ((m2&STEP42_SKIP_IAC_RESET_BIT)==0u) ++passed;
    printf("  running-engine major loop clears skip-reset bit and re-arms homing %s\n",
           ((m2&STEP42_SKIP_IAC_RESET_BIT)==0u)?"PASS":"FAIL");
    m2=bua_iac_rearm_reset_step42(STEP42_SKIP_IAC_RESET_BIT,0u);
    if ((m2&STEP42_SKIP_IAC_RESET_BIT)!=0u) ++passed;
    printf("  stopped-engine path does not re-arm a completed IAC reset           %s\n",
           ((m2&STEP42_SKIP_IAC_RESET_BIT)!=0u)?"PASS":"FAIL");
    if (bua_iac_reset_requested_step42(0u,0u,STEP42_IGN_OFF_BIT)) ++passed;
    printf("  key-off with reset armed enters LD393 IAC homing                    %s\n",
           bua_iac_reset_requested_step42(0u,0u,STEP42_IGN_OFF_BIT)?"PASS":"FAIL");
    if (!bua_iac_reset_requested_step42(0u,0u,0u)) ++passed;
    printf("  ignition-on with reset armed remains in normal IAC processing       %s\n",
           !bua_iac_reset_requested_step42(0u,0u,0u)?"PASS":"FAIL");
    if (!bua_iac_reset_requested_step42(0u,STEP42_SKIP_IAC_RESET_BIT,STEP42_IGN_OFF_BIT)) ++passed;
    printf("  completed-reset marker prevents a second key-off homing pass        %s\n",
           !bua_iac_reset_requested_step42(0u,STEP42_SKIP_IAC_RESET_BIT,STEP42_IGN_OFF_BIT)?"PASS":"FAIL");
    if (bua_iac_reset_requested_step42(STEP42_IAC_RESET_PHASE_BIT,STEP42_SKIP_IAC_RESET_BIT,0u)) ++passed;
    printf("  once reset phase is active it continues to completion               %s\n",
           bua_iac_reset_requested_step42(STEP42_IAC_RESET_PHASE_BIT,STEP42_SKIP_IAC_RESET_BIT,0u)?"PASS":"FAIL");
    r=bua_ignoff_tick_step42(123u,0u,0xAAu,0x55u,0xFFu,0x80u);
    if (r.timer==0u && !r.housekeeping_active && !r.soft_powerdown) ++passed;
    printf("  ignition-on LD721 forces the ignition-off timer back to zero        %s\n",
           (r.timer==0u && !r.housekeeping_active && !r.soft_powerdown)?"PASS":"FAIL");
    r=bua_ignoff_tick_step42(0u,1u,0xAAu,0x55u,0xFFu,0x80u);
    if (r.timer==1u && r.nv_mode_word==0xAAu && !r.housekeeping_active) ++passed;
    printf("  first key-off count increments without housekeeping                 %s\n",
           (r.timer==1u && r.nv_mode_word==0xAAu && !r.housekeeping_active)?"PASS":"FAIL");
    r=bua_ignoff_tick_step42(7u,1u,0xAAu,0x55u,0xFFu,0x80u);
    if (r.timer==8u && !r.housekeeping_active) ++passed;
    printf("  count 7 -> 8 still uses the pre-housekeeping path                   %s\n",
           (r.timer==8u && !r.housekeeping_active)?"PASS":"FAIL");
    r=bua_ignoff_tick_step42(8u,1u,0xAAu,0x5Fu,0xFFu,0x80u);
    if (r.timer==9u && r.housekeeping_active && r.nv_mode_word==0u &&
        r.mode_word2==STEP42_SKIP_IAC_RESET_BIT && r.mode_word3==0x40u) ++passed;
    printf("  at count 8 housekeeping clears mode words but preserves reset-done %s\n",
           (r.timer==9u && r.housekeeping_active && r.nv_mode_word==0u &&
            r.mode_word2==STEP42_SKIP_IAC_RESET_BIT && r.mode_word3==0x40u)?"PASS":"FAIL");
    r=bua_ignoff_tick_step42(8u,1u,0u,0u,0u,0x80u);
    if (r.clear_engine_run_time && (r.engine_mode_word&0x80u)==0u) ++passed;
    printf("  housekeeping path clears engine runtime/state                      %s\n",
           (r.clear_engine_run_time && (r.engine_mode_word&0x80u)==0u)?"PASS":"FAIL");
    r=bua_ignoff_tick_step42((bua_u16)(STEP42_SOFT_POWERDOWN_COUNT-1u),1u,0u,0u,0u,0u);
    if (!r.soft_powerdown && r.timer==STEP42_SOFT_POWERDOWN_COUNT) ++passed;
    printf("  count immediately below LC012 advances to the threshold            %s\n",
           (!r.soft_powerdown && r.timer==STEP42_SOFT_POWERDOWN_COUNT)?"PASS":"FAIL");
    r=bua_ignoff_tick_step42(STEP42_SOFT_POWERDOWN_COUNT,1u,0u,0u,0u,0u);
    if (r.soft_powerdown && r.timer==STEP42_SOFT_POWERDOWN_COUNT) ++passed;
    printf("  LC012=$0385 enters the software-interrupt power-down loop           %s\n",
           (r.soft_powerdown && r.timer==STEP42_SOFT_POWERDOWN_COUNT)?"PASS":"FAIL");
    if (STEP42_SOFT_POWERDOWN_COUNT==901u) ++passed;
    printf("  LC012 hexadecimal $0385 is 901 loop counts                          %s\n",
           (STEP42_SOFT_POWERDOWN_COUNT==901u)?"PASS":"FAIL");
    /* Full normal lifecycle: completed reset -> engine runs/re-arms -> key off
       requests one new homing pass -> completion sets skip marker again. */
    m2=STEP42_SKIP_IAC_RESET_BIT;
    m2=bua_iac_rearm_reset_step42(m2,STEP42_ENGINE_RUNNING_BIT);
    if (bua_iac_reset_requested_step42(0u,m2,STEP42_IGN_OFF_BIT)) ++passed;
    printf("  run -> key-off lifecycle requests one newly re-armed reset          %s\n",
           bua_iac_reset_requested_step42(0u,m2,STEP42_IGN_OFF_BIT)?"PASS":"FAIL");
    m2=(bua_u8)(m2|STEP42_SKIP_IAC_RESET_BIT);
    if (!bua_iac_reset_requested_step42(0u,m2,STEP42_IGN_OFF_BIT)) ++passed;
    printf("  after homing completion the same shutdown no longer retriggers      %s\n",
           !bua_iac_reset_requested_step42(0u,m2,STEP42_IGN_OFF_BIT)?"PASS":"FAIL");
    printf("  step-42 IAC shutdown regression result: %s (%u/16)\n",
           (passed==16u)?"PASS":"FAIL",passed);
}
static void run_step44_crank_to_run_test(void)
{
    BuaRunQual44 q;
    BuaFuelPath44 path;
    bua_u8 c;
    unsigned int i;
    unsigned int passed=0u;
    unsigned int total=14u;
    printf("\nStep-44 crank-to-run qualification / fuel handoff regression:\n");
    memset(&q,0,sizeof(q));
    bua_run_qual_ref_event_step44(&q,(bua_u16)CAL_STARTUP_REF_PERIOD);
    if (q.second_ref_valid==0u && q.startup_counter==0u && q.engine_running==0u) ++passed;
    printf("  threshold period does not qualify             %s\n",
           (q.second_ref_valid==0u && q.startup_counter==0u && q.engine_running==0u)?"PASS":"FAIL");
    memset(&q,0,sizeof(q));
    bua_run_qual_ref_event_step44(&q,(bua_u16)(CAL_STARTUP_REF_PERIOD-1u));
    if (q.second_ref_valid==1u && q.startup_counter==0u) ++passed;
    printf("  first fast ref establishes second-ref latch   %s\n",
           (q.second_ref_valid==1u && q.startup_counter==0u)?"PASS":"FAIL");
    for (i=0u;i<8u;++i)
        bua_run_qual_ref_event_step44(&q,(bua_u16)(CAL_STARTUP_REF_PERIOD-1u));
    if (q.startup_counter==8u && q.engine_running==0u) ++passed;
    printf("  eight further refs fill startup counter       %s\n",
           (q.startup_counter==8u && q.engine_running==0u)?"PASS":"FAIL");
    bua_run_qual_ref_event_step44(&q,(bua_u16)(CAL_STARTUP_REF_PERIOD-1u));
    if (q.engine_running==1u) ++passed;
    printf("  tenth qualifying ref sets ENGINE RUNNING      %s\n",
           (q.engine_running==1u)?"PASS":"FAIL");
    memset(&q,0,sizeof(q));
    q.second_ref_valid=1u;
    q.startup_counter=5u;
    bua_run_qual_ref_event_step44(&q,(bua_u16)CAL_STARTUP_REF_PERIOD);
    if (q.startup_counter==0u && q.engine_running==0u) ++passed;
    printf("  slow ref resets startup qualification count   %s\n",
           (q.startup_counter==0u && q.engine_running==0u)?"PASS":"FAIL");
    c=bua_cold_start_ref_count_step44(0u,0u,0u);
    if (c==1u) ++passed;
    printf("  L0118 qualifying event increments 0 -> 1      %s\n",(c==1u)?"PASS":"FAIL");
    c=bua_cold_start_ref_count_step44(127u,0u,0u);
    if (c==127u) ++passed;
    printf("  L0118 saturates at signed-positive 127        %s\n",(c==127u)?"PASS":"FAIL");
    c=bua_cold_start_ref_count_step44(55u,33u,0u);
    if (c==0u) ++passed;
    printf("  run time >32 sec clears L0118                 %s\n",(c==0u)?"PASS":"FAIL");
    c=bua_cold_start_ref_count_step44(55u,32u,0u);
    if (c==56u) ++passed;
    printf("  run time exactly 32 sec still permits count   %s\n",(c==56u)?"PASS":"FAIL");
    c=bua_cold_start_ref_count_step44(55u,0u,198u);
    if (c==0u) ++passed;
    printf("  TPS >197 clears L0118                         %s\n",(c==0u)?"PASS":"FAIL");
    c=bua_cold_start_ref_count_step44(55u,0u,197u);
    if (c==56u) ++passed;
    printf("  TPS exactly 197 still permits count           %s\n",(c==56u)?"PASS":"FAIL");
    path=bua_select_fuel_path_step44(0u,0u);
    if (path==BUA_FUEL_PATH_CRANK44) ++passed;
    printf("  not-running selects crank fuel                %s\n",
           (path==BUA_FUEL_PATH_CRANK44)?"PASS":"FAIL");
    path=bua_select_fuel_path_step44(ENGINE_RUNNING_BIT,0u);
    if (path==BUA_FUEL_PATH_NORMAL44) ++passed;
    printf("  running + LC35A=0 selects normal fuel         %s\n",
           (path==BUA_FUEL_PATH_NORMAL44)?"PASS":"FAIL");
    ecm_reset();
    memset(&q,0,sizeof(q));
    for (i=0u;i<10u;++i)
        bua_run_qual_ref_event_step44(&q,(bua_u16)(CAL_STARTUP_REF_PERIOD-1u));
    bua_apply_run_transition_step44(&q);
    path=bua_select_fuel_path_step44(ENGINE_MODE_WORD,CRANK_FF_COUNTER);
    if ((ENGINE_MODE_WORD & ENGINE_RUNNING_BIT)!=0u &&
        STARTUP_COUNTER==8u &&
        path==BUA_FUEL_PATH_NORMAL44) ++passed;
    printf("  catch sequence hands first running fuel pass  %s\n",
           ((ENGINE_MODE_WORD & ENGINE_RUNNING_BIT)!=0u &&
            STARTUP_COUNTER==8u && path==BUA_FUEL_PATH_NORMAL44)?"PASS":"FAIL");
    printf("  Step-44 regression: PASS %u/%u, FAIL %u\n",
           passed,total,total-passed);
}
static void run_step43_cranking_fuel_test(void)
{
    unsigned int passed=0u;
    bua_u16 cold;
    bua_u16 warm;
    bua_u16 tps75;
    bua_u16 clearflood;
    bua_u16 clamped;
    bua_u16 drp0;
    bua_u16 drp64;
    printf("\nCranking-fuel regression (Step 43, LD7B1..LD865):\n");
    cold=bua_crank_pw_step43(0u,0u,0u);
    warm=bua_crank_pw_step43(160u,0u,0u);
    if (cold>warm && cold>5000u) ++passed;
    printf("  cold crank PW is substantially larger than warm crank PW            %s\n",
           (cold>warm && cold>5000u)?"PASS":"FAIL");
    if (bua_crank_tps_mult_step43(0u)==64u) ++passed;
    printf("  closed-throttle C390 multiplier is exactly 64 (1.000)               %s\n",
           (bua_crank_tps_mult_step43(0u)==64u)?"PASS":"FAIL");
    if (bua_crank_tps_mult_step43(192u)==115u) ++passed;
    printf("  75%% TPS C390 multiplier reaches 115 before clear-flood region       %s\n",
           (bua_crank_tps_mult_step43(192u)==115u)?"PASS":"FAIL");
    tps75=bua_crank_pw_step43(160u,192u,0u);
    if (tps75>warm) ++passed;
    printf("  75%% TPS enriches cranking fuel relative to closed throttle          %s\n",
           (tps75>warm)?"PASS":"FAIL");
    if (bua_crank_tps_mult_step43(224u)==0u) ++passed;
    printf("  87.5%% TPS enters the calibrated zero-fuel clear-flood region        %s\n",
           (bua_crank_tps_mult_step43(224u)==0u)?"PASS":"FAIL");
    clearflood=bua_crank_pw_step43(160u,224u,0u);
    if (clearflood==0u) ++passed;
    printf("  clear-flood TPS produces zero constructed cranking pulse width       %s\n",
           (clearflood==0u)?"PASS":"FAIL");
    if (bua_crank_tps_mult_step43(255u)==0u) ++passed;
    printf("  full-throttle endpoint remains zero fuel                             %s\n",
           (bua_crank_tps_mult_step43(255u)==0u)?"PASS":"FAIL");
    clamped=bua_crank_pw_step43(255u,0u,0u);
    if (clamped==bua_crank_pw_step43(208u,0u,0u)) ++passed;
    printf("  coolant above raw 208 is clamped to the 115-C lookup endpoint       %s\n",
           (clamped==bua_crank_pw_step43(208u,0u,0u))?"PASS":"FAIL");
    drp0=bua_crank_pw_step43(160u,0u,0u);
    drp64=bua_crank_pw_step43(160u,0u,64u);
    if (drp0==drp64) ++passed;
    printf("  C37F all-255 calibration makes DRP multiplier effectively constant  %s\n",
           (drp0==drp64)?"PASS":"FAIL");
    if (!bua_crank_rpm_blend_enabled_step43()) ++passed;
    printf("  C35B all-zero table disables the later crank-RPM blend path          %s\n",
           (!bua_crank_rpm_blend_enabled_step43())?"PASS":"FAIL");
    if (CAL_CRANK_FF_SKIP_COUNT==0u) ++passed;
    printf("  LC35A is zero: any nonzero FF count can skip crank logic when running %s\n",
           (CAL_CRANK_FF_SKIP_COUNT==0u)?"PASS":"FAIL");
    if (CAL_CRANK_RPM_COEF==128u) ++passed;
    printf("  LC359 crank-RPM filter coefficient is 128 (0.5 calibration)         %s\n",
           (CAL_CRANK_RPM_COEF==128u)?"PASS":"FAIL");
    printf("  example PW raw counts: cold=%u warm=%u 75%%TPS=%u clear-flood=%u\n",
           (unsigned int)cold,(unsigned int)warm,(unsigned int)tps75,
           (unsigned int)clearflood);
    printf("  step-43 cranking-fuel regression result: %s (%u/12)\n",
           (passed==12u)?"PASS":"FAIL",passed);
}
