/*
 * BUA / ECM 1227165 - Step-121 integrated lifecycle PC harness.
 *
 * The implementation fragments below intentionally form one translation
 * unit.  This preserves the frozen Step-104 static linkage, declaration
 * order, C89 behavior, and simple "gcc main.c" / OnlineGDB build.
 *
 * Directory roles:
 *   src/        translated ECM behavior and scheduler-facing support
 *   simulation/ PC-only vehicle/engine driving model
 *   tests/      regression models and regression entry points
 */

/* Base platform, RAM/MPU model, translated algorithms, and scheduler. */
#include "src/ecm_core.inc.h"
#include "src/startup_reset.inc.h"
#include "src/vector_boundaries.inc.h"
#include "src/factory_test_control.inc.h"
#include "src/factory_test_execution.inc.h"
#include "src/scheduler_serial.inc.h"
#include "src/ignition_shutdown.inc.h"
#include "src/diagnostics.inc.h"
#include "tests/diagnostics_regression.inc.h"
#include "src/diagnostic_qualification.inc.h"
#include "tests/diagnostic_qualification_regression.inc.h"
#include "src/diagnostic_flash.inc.h"
#include "src/diagnostic_integration.inc.h"

/* Earlier fuel/spark regressions and the integrated drive simulator. */
#include "tests/fuel_spark_regression.inc.h"
#include "simulation/dynamic_drive.inc.h"
#include "tests/spark_reference_regression.inc.h"
#include "tests/maf_regression.inc.h"

/* Fuel, closed-loop, idle-air, startup, and transmission behavior. */
#include "src/fuel_air.inc.h"
#include "tests/closed_loop_iac_regression.inc.h"
#include "src/dfco_transient.inc.h"
#include "tests/dfco_regression.inc.h"
#include "src/injector_service.inc.h"
#include "tests/startup_fuel_regression.inc.h"
#include "tests/iac_ac_regression.inc.h"
#include "tests/tcc_transmission_regression.inc.h"

/* Major-loop feature implementations followed by their regressions. */
#include "src/ac_control.inc.h"
#include "tests/ac_control_regression.inc.h"
#include "src/air_control.inc.h"
#include "tests/air_regression.inc.h"
#include "src/purge_control.inc.h"
#include "tests/purge_regression.inc.h"
#include "src/egr_control.inc.h"
#include "tests/egr_fan_regression.inc.h"
#include "src/mat_control.inc.h"
#include "tests/mat_regression.inc.h"
#include "src/coolant_control.inc.h"
#include "tests/coolant_regression.inc.h"
#include "tests/listing_coolant_regression.inc.h"
#include "src/segment_f.inc.h"
#include "tests/segment_f_regression.inc.h"
#include "tests/battery_regression.inc.h"

/* Final major-loop wiring, scheduler wiring, and integration tests. */
#include "src/major_loop.inc.h"
#include "src/startup_normal.inc.h"
#include "src/power_on_dispatch.inc.h"
#include "src/lifecycle_integration.inc.h"
#include "src/sci_8192.inc.h"
#include "src/mode4_lifecycle.inc.h"
#include "tests/major_loop_regression.inc.h"
#include "tests/output_stage_regression.inc.h"
#include "tests/ignition_shutdown_regression.inc.h"
#include "tests/startup_reset_regression.inc.h"
#include "tests/startup_memory_regression.inc.h"
#include "tests/startup_normal_regression.inc.h"
#include "tests/sci_8192_regression.inc.h"
#include "tests/mode4_lifecycle_regression.inc.h"
#include "tests/factory_test_control_regression.inc.h"
#include "tests/factory_test_execution_regression.inc.h"
#include "tests/vector_boundaries_regression.inc.h"
#include "tests/power_on_dispatch_regression.inc.h"
#include "tests/lifecycle_integration_regression.inc.h"
#include "tests/scheduler_crank_blm_regression.inc.h"
#include "src/output_handlers.inc.h"
#include "tests/scheduler_regression.inc.h"
#include "simulation/transmission_drive.inc.h"
#include "tests/diagnostic_integration_regression.inc.h"

int main(void)
{
    unsigned int i;
    ecm_reset();
    sim_set_vss_mph(60u);
    sim_set_dash_vss_mph(60u);
    sim_set_dash_tach_rpm(2400u);
    sim_set_ecm_reference_rpm(2400u);
    dash_set_test_inputs();
    dash_update_direct_inputs();
    printf("BUA / 1227165 + 1986 Corvette cluster PC step-117 test\n");
    printf("Initial minor count = %u (0x%02X)\n",
           (unsigned int)MINOR_COUNT,
           (unsigned int)MINOR_COUNT);
    printf("Simulated ECM VSS target = 60 MPH, period = %u BUA timer ticks\n",
           (unsigned int)sim_vss_period_ticks);
    printf("Simulated cluster VSS = 60 MPH, pulse period = %lu us\n",
           (unsigned long)sim_dash_vss.period_us);
    printf("Simulated HEI tach = 2400 RPM, pulse period = %lu us\n",
           (unsigned long)sim_dash_tach.period_us);
    printf("Simulated ECM reference = 2400 RPM, 4 ref/rev, 65.536-kHz timer\n\n");
    print_segment_table();
    printf("\nFirst second, VSS segment-2 results:\n");
    for (i = 0u; i < 160u; ++i) {
        irq_6p25ms();
        if ((MINOR_COUNT & 0x0Fu) == 2u) {
            printf("t=%6.3f s count=%3u VSS=%3u.%03u mph"
                   " scaled=%3u noEdge=%3u captures=%lu\n",
                   ((double)(i + 1u) * 0.00625),
                   (unsigned int)MINOR_COUNT,
                   (unsigned int)VSS_MPH_HI,
                   (unsigned int)(((unsigned int)VSS_MPH_LO * 1000u) / 256u),
                   (unsigned int)VSS_SCALED,
                   (unsigned int)VSS_NO_EDGE_COUNT,
                   (unsigned long)stats.vss_capture_changes);
        }
    }
    printf("\nAfter 1 second:\n");
    printf("IRQ=%lu AirFuel=%lu SparkVSS=%lu oneSec=%lu runSec=%u count=%u\n",
           (unsigned long)stats.irq_ticks,
           (unsigned long)stats.air_fuel_loops,
           (unsigned long)stats.spark_vss_loops,
           (unsigned long)stats.one_second_events,
           (unsigned int)ram16be_get(0x001Au),
           (unsigned int)MINOR_COUNT);
    printf("ECM RPM from reference pulses = %u RPM (L0057=%u, L0058=%u.%03u x12.5)\n",
           (unsigned int)RPM_DIV25 * 25u,
           (unsigned int)RPM_DIV25,
           (unsigned int)RPM_DIV12P5,
           (unsigned int)(((unsigned int)RAM8(0x0059u) * 1000u) / 256u));
    printf("Reference pulses=%lu RPM calculations=%lu period=%u timer counts\n",
           (unsigned long)stats.ref_pulses,
           (unsigned long)stats.rpm_calculations,
           (unsigned int)ram16be_get(REF_PERIOD_ADDR));
    printf("Engine-running=%u startupCount=%u secondRefValid=%u transitions=%lu L0056=%u\n",
           (unsigned int)((ENGINE_MODE_WORD & ENGINE_RUNNING_BIT) != 0u),
           (unsigned int)STARTUP_COUNTER,
           (unsigned int)((IGN_MODE_WORD & SECOND_REF_VALID_BIT) != 0u),
           (unsigned long)stats.engine_run_transitions,
           (unsigned int)RPM_SPECIAL);
    printf("Normal-path dwell L009B=%u counts, dynamic L0099=%u, calculations=%lu\n",
           (unsigned int)ram16be_get(DWELL_COUNTS_ADDR),
           (unsigned int)ram16be_get(DYNAMIC_DWELL_ADDR),
           (unsigned long)stats.dwell_calculations);
    {
        bua_u8 base_spark;
        bua_u8 coolant_spark;
        bua_s16 partial_spark;
        partial_spark = bua_main_plus_coolant_raw(RPM_SPECIAL, FILTERED_LOAD, COOLANT,
                                                   &base_spark, &coolant_spark);
        printf("C02E main raw=%u (%0.2f deg), C0FF coolant raw=%u (%+0.2f deg corr)\n",
               (unsigned int)base_spark, bua_spark_raw_to_degrees(base_spark),
               (unsigned int)coolant_spark,
               bua_spark_signed_raw_to_degrees((bua_s16)coolant_spark -
                                                (bua_s16)CAL_COOL_SPARK_BIAS_RAW));
        printf("Main + coolant partial spark = raw %d = %0.2f deg (not final commanded spark)\n",
               (int)partial_spark, bua_spark_signed_raw_to_degrees(partial_spark));
        {
            bua_u8 highway_timer;
            bua_u8 highway_raw;
            bua_u8 hot_mode;
            bua_u8 hot_raw;
            bua_s16 step15_raw;
            highway_raw = bua_highway_spark_eval(FILTERED_LOAD, COOLANT, RPM_SPECIAL,
                                                  MINOR_COUNT, HIGHWAY_SPARK_TIMER,
                                                  &highway_timer);
            hot_raw = bua_hot_restart_retard_eval(MODE_WORD1, STARTUP_COOLANT,
                                                   RAM8(0x001Bu), &hot_mode);
            step15_raw = (bua_s16)(partial_spark + (bua_s16)highway_raw -
                                   (bua_s16)hot_raw);
            printf("Step-15 additions: highway=%u (%+0.2f deg), hot-restart=-%u (%0.2f deg)\n",
                   (unsigned int)highway_raw, bua_spark_raw_to_degrees(highway_raw),
                   (unsigned int)hot_raw, bua_spark_raw_to_degrees(hot_raw));
            printf("Main + coolant + highway - hot-restart = raw %d = %0.2f deg (still not final spark)\n",
                   (int)step15_raw, bua_spark_signed_raw_to_degrees(step15_raw));
            printf("Current knock retard state = %u (subtract %u raw spark = %0.2f deg when active)\n",
                   (unsigned int)KNOCK_RETARD,
                   (unsigned int)bua_knock_spark_subtract_raw(),
                   bua_spark_raw_to_degrees(bua_knock_spark_subtract_raw()));
            {
                bua_u8 startup_raw;
                bua_u8 aldl_raw;
                bua_s16 l0115;
                bua_s16 relref;
                startup_raw = bua_startup_spark_sum_byte();
                aldl_raw = bua_aldl_added_spark_eval();
                l0115 = bua_spark_sum_ld096(base_spark, coolant_spark, highway_raw,
                                             hot_raw, 0u, aldl_raw, startup_raw);
                relref = bua_spark_relative_to_reference_pre_mode4();
                printf("Step-18 LD096 sum: startup=%u ALDL=%u -> L0115=%d raw (%0.2f deg)\n",
                       (unsigned int)startup_raw, (unsigned int)aldl_raw, (int)l0115,
                       bua_spark_signed_raw_to_degrees(l0115));
                printf("Step-18 pre-Mode-4 L009D=%d raw relative to reference (%0.2f deg raw scale)\n",
                       (int)relref, bua_spark_signed_raw_to_degrees(relref));
            }
        }
    }
    printf("Final VSS = %u.%03u MPH (raw 8.8 = 0x%04X), scaled L0067=%u\n",
           (unsigned int)VSS_MPH_HI,
           (unsigned int)(((unsigned int)VSS_MPH_LO * 1000u) / 256u),
           (unsigned int)ram16be_get(0x0065u),
           (unsigned int)VSS_SCALED);
    printf("Major segment calls:");
    for (i = 0u; i < 16u; ++i)
        printf(" %X=%lu", i, (unsigned long)stats.major_segment_calls[i]);
    printf("\n\n");
    printf("Normal byte sources selected by LF8B6/LF8E6:\n");
    printf("  L0035=%02X  C009=%02X  L011A=%02X  L011E=%02X  C70C=%02X\n\n",
           (unsigned int)MINOR_MODE_WORD2,
           (unsigned int)CAL_NUM_CYL_CODE,
           (unsigned int)RAM8(0x011Au),
           (unsigned int)RAM8(0x011Eu),
           (unsigned int)CAL_GPH_SCALE_FACTOR);
    print_serial_trace();
    printf("\n");
    print_receiver_round_trip();
    dash_accept_serial_receiver();
    printf("\n1986 Corvette cluster-input model, measured from simulated signals:\n");
    printf("  VSS pulse period         = %lu us\n",
           (unsigned long)dash.vss_period_us);
    printf("  VSS rising edges         = %lu\n",
           (unsigned long)dash.vss_rising_edges);
    printf("  Decoded speed            = %u MPH\n", dash.speed_mph);
    printf("  HEI tach pulse period    = %lu us\n",
           (unsigned long)dash.tach_period_us);
    printf("  HEI tach rising edges    = %lu\n",
           (unsigned long)dash.tach_rising_edges);
    printf("  Decoded engine speed     = %u RPM\n", dash.rpm);
    printf("  Fuel sender raw          = %u (unscaled test stimulus)\n", dash.fuel_sender_raw);
    printf("  Coolant raw              = %u (unscaled test stimulus)\n", dash.coolant_raw);
    printf("  Oil pressure raw         = %u (unscaled test stimulus)\n", dash.oil_pressure_raw);
    printf("  Oil temperature raw      = %u (unscaled test stimulus)\n", dash.oil_temp_raw);
    printf("  Battery                  = %u mV\n", dash.battery_mv);
    printf("  ECM serial receiver      =");
    for (i = 0u; i < dash.serial_count; ++i)
        printf(" %02X", (unsigned int)dash.serial_bytes[i]);
    printf("\n");
    printf("  NOTE: no MPG/range interpretation is applied yet.\n");
    printf("  NOTE: VSS %lu pulses/mile and tach %lu pulses/rev are simulator\n",
           (unsigned long)SIM_VSS_PULSES_PER_MILE,
           (unsigned long)SIM_TACH_PULSES_PER_REV);
    printf("        calibration parameters pending exact 1986 hardware verification.\n");
    printf("\nSegment-4 $5806 hook calls = %lu (H.U. flag currently %s)\n",
           (unsigned long)stats.hu_5806_calls,
           ((IGN_MODE_WORD & 0x80u) != 0u) ? "set" : "clear");
    run_main_spark_lookup_test();
    run_coolant_spark_lookup_test();
    run_step15_spark_terms_test();
    run_step16_startup_spark_test();
    run_step18_spark_sum_test();
    run_step19_mode4_blend_test();
    run_step20_spark_timing_test();
    run_step21_ignition_finish_test();
    run_step22_base_fuel_test();
    run_step26_fuel_output_test();
    run_step27_fuel_limit_test();
    run_step28_async_ae_test();
    run_step29_injector_event_test();
    run_step30_maf_test();
    run_step31_sensor_to_injector_test();
    run_step32_closed_loop_gate_test();
    run_step33_decel_integrator_test();
    run_step34_closed_loop_entry_test();
    run_step35_blm_learning_test();
    run_step36_iac_command_test();
    run_step37_iac_pid_test();
    run_step38_iac_transition_learning_test();
    run_step39_iac_tf_anticipation_test();
    run_step40_iac_motor_test();
    run_step41_iac_startup_reset_test();
    run_step42_iac_shutdown_test();
    run_step45_dfco_test();
    run_step47_afr_selection_test();
    run_step61_idle_load_disturbance_test();
    run_step62_ac_anticipation_test();
    run_step63_ac_removal_cycle_test();
    run_step64_power_steering_test();
    run_step65_power_steering_gate_test();
    run_step66_iac_tf_tail_test();
    run_step67_tcc_test();
    run_step68_tcc_nv_test();
    run_step69_tcc_passby_test();
    run_step70_tcc_brake_boundary_test();
    run_step72_tcc_coast_test();
    run_step73_tcc_tail_test();
    run_step77_lf2b9_tcc_test();
    run_step78_prndl_test();
    run_step79_fan_test();
    run_step80_ac_front_test();
    run_step81_air_test();
    run_step82_egr_test();
    run_step83_purge_test();
    run_step84_mat_test();
    run_step85_coolant_test();
    run_step106_listing_coolant_test();
    run_step107_diagnostics_stage1_test();
    run_step108_diagnostic_qualification_test();
    run_step109_diagnostic_integration_test();
    run_step110_output_stage_test();
    run_step111_ignition_shutdown_test();
    run_step112_startup_reset_test();
    run_step113_startup_memory_test();
    run_step114_startup_normal_test();
    run_step115_sci_test();
    run_step116_mode4_lifecycle_test();
    run_step117_factory_control_test();
    run_step118_factory_execution_test();
    run_step119_vector_boundaries_test();
    run_step120_power_on_dispatch_test();
    run_step121_lifecycle_integration_test();
    run_step86_battery_test();
    run_step89_scheduler_wiring_test();
    run_step90_major_wiring_test();
    run_step91_loose_ends_test();
    run_step92_minor_cadence_test();
    run_step93_even_path_test();
    run_step94_odd_fuel_path_test();
    run_step95_load_producer_test();
    run_step96_transient_front_test();
    run_step97_50ms_tail_test();
    run_step98_injector_bookkeeping_test();
    run_step100_remaining_normal_major_test();
    run_step101_scheduler_crank_fuel_test();
    run_step102_scheduler_async_ae_test();
    run_step103_blm_cell_selection_test();
    run_step88_segment_f_test();
    run_step60_iac_multirate_test();
    run_step59_iac_fractional_test();
    run_step58_iac_regulator_test();
    run_step57_closed_iac_plant_test();
    run_step56_dynamic_start_trajectory_test();
    run_step55_physical_injector_scheduler_test();
    run_step54_ref_batch_geometry_test();
    run_step53_mpu_control_test();
    run_step52_mpu_interface_test();
    run_step51_injector_cadence_test();
    run_step50_single_fire_test();
    run_step49_continuous_start_fuel_test();
    run_step48_startup_afr_decay_test();
    run_step46_dfco_output_test();
    run_step44_crank_to_run_test();
    run_step43_cranking_fuel_test();
    run_step25_afr_test();
    run_step24_o2_control_test();
    run_step23_blm_closed_loop_test();
    run_step17_knock_test();
    run_dwell_arithmetic_test();
    run_reference_startup_test();
    /* Re-start from a clean ECM/dash state for the dynamic profile. */
    ecm_reset();
    dash_set_test_inputs();
    dash_update_direct_inputs();
    run_dynamic_drive_cycle();
    /* Preserve the Step-104 drive above, then run the independent Step-105
     * transmission-aware scenario from another clean ECM/dash state. */
    ecm_reset();
    dash_set_test_inputs();
    dash_update_direct_inputs();
    run_step105_transmission_drive();
    return 0;
}
