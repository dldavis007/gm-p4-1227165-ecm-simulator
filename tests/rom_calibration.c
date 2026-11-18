/* Compare compiled tables, including macro-expanded rows, to fixed ROM addresses. */
#define main regression_main
#include "../main.c"
#undef main

static const struct {
    const char *name;
    const void *data;
    size_t length;
    unsigned int address, stride;
} tables[] = {
    {"cal_o2_fast_threshold", cal_o2_fast_threshold, sizeof(cal_o2_fast_threshold), 0xC579u, 1u},
    {"cal_o2_int_delay_airflow", cal_o2_int_delay_airflow, sizeof(cal_o2_int_delay_airflow), 0xC582u, 1u},
    {"cal_o2_error_reduction", cal_o2_error_reduction, sizeof(cal_o2_error_reduction), 0xC594u, 1u},
    {"cal_o2_prop_width_rpm", cal_o2_prop_width_rpm, sizeof(cal_o2_prop_width_rpm), 0xC59Du, 1u},
    {"cal_o2_prop_step_error", cal_o2_prop_step_error, sizeof(cal_o2_prop_step_error), 0xC5A3u, 1u},
    {"cal_o2_prop_width_error", cal_o2_prop_width_error, sizeof(cal_o2_prop_width_error), 0xC5AFu, 1u},
    {"cal_o2_int_delay_error", cal_o2_int_delay_error, sizeof(cal_o2_int_delay_error), 0xC5C9u, 1u},
    {"cal_o2_slow_upper", cal_o2_slow_upper, sizeof(cal_o2_slow_upper), 0xC567u, 1u},
    {"cal_o2_slow_lower", cal_o2_slow_lower, sizeof(cal_o2_slow_lower), 0xC570u, 1u},
    {"cal_o2_slow_filter_coef", cal_o2_slow_filter_coef, sizeof(cal_o2_slow_filter_coef), 0xC58Bu, 1u},
    {"cal_open_loop_load_afr", cal_open_loop_load_afr, sizeof(cal_open_loop_load_afr), 0xC3D3u, 1u},
    {"cal_pe_coolant_change", cal_pe_coolant_change, sizeof(cal_pe_coolant_change), 0xC614u, 1u},
    {"cal_pe_rpm_change", cal_pe_rpm_change, sizeof(cal_pe_rpm_change), 0xC61Du, 1u},
    {"cal_startup_enrich_step48", cal_startup_enrich_step48, sizeof(cal_startup_enrich_step48), 0xC3E4u, 1u},
    {"cal_startup_delay_step48", cal_startup_delay_step48, sizeof(cal_startup_delay_step48), 0xC3F4u, 1u},
    {"cal_startup_repeat_step48", cal_startup_repeat_step48, sizeof(cal_startup_repeat_step48), 0xC402u, 1u},
    {"cal_startup_decay_step48", cal_startup_decay_step48, sizeof(cal_startup_decay_step48), 0xC410u, 1u},
    {"cal_open_loop_coolant_step48", cal_open_loop_coolant_step48, sizeof(cal_open_loop_coolant_step48), 0xC41Fu, 1u},
    {"cal_small_pw_offset", cal_small_pw_offset, sizeof(cal_small_pw_offset), 0xC3BCu, 1u},
    {"cal_fuel_limit_coolant", cal_fuel_limit_coolant, sizeof(cal_fuel_limit_coolant), 0xC331u, 1u},
    {"cal_async_bpwpct", cal_async_bpwpct, sizeof(cal_async_bpwpct), 0xC315u, 1u},
    {"cal_async_pulse_count", cal_async_pulse_count, sizeof(cal_async_pulse_count), 0xC31Eu, 1u},
    {"cal_async_cool_factor", cal_async_cool_factor, sizeof(cal_async_cool_factor), 0xC328u, 1u},
    {"cal_o2_lean_offset_c5bb", cal_o2_lean_offset_c5bb, sizeof(cal_o2_lean_offset_c5bb), 0xC5BBu, 1u},
    {"cal_maf_scalar", cal_maf_scalar, sizeof(cal_maf_scalar), 0xC69Du, 11u},
    {"cal_maf_table1", cal_maf_table1, sizeof(cal_maf_table1), 0xC69Fu, 1u},
    {"cal_maf_table2", cal_maf_table2, sizeof(cal_maf_table2), 0xC6AAu, 1u},
    {"cal_maf_table3", cal_maf_table3, sizeof(cal_maf_table3), 0xC6B5u, 1u},
    {"cal_maf_table4", cal_maf_table4, sizeof(cal_maf_table4), 0xC6C0u, 1u},
    {"cal_maf_table5", cal_maf_table5, sizeof(cal_maf_table5), 0xC6CBu, 1u},
    {"cal_maf_table6", cal_maf_table6, sizeof(cal_maf_table6), 0xC6D6u, 1u},
    {"cal_maf_max_flow", cal_maf_max_flow, sizeof(cal_maf_max_flow), 0xC6ECu, 1u},
    {"cal_maf_default_rpm", cal_maf_default_rpm, sizeof(cal_maf_default_rpm), 0xC218u, 1u},
    {"cal_base_fuel", cal_base_fuel, sizeof(cal_base_fuel), 0xC430u, 1u},
    {"cal_main_spark", cal_main_spark, sizeof(cal_main_spark), 0xC031u, 1u},
    {"cal_coolant_spark", cal_coolant_spark, sizeof(cal_coolant_spark), 0xC102u, 1u},
    {"cal_highway_spark", cal_highway_spark, sizeof(cal_highway_spark), 0xC172u, 1u},
    {"cal_startup_spark", cal_startup_spark, sizeof(cal_startup_spark), 0xC17Au, 1u},
    {"cal_startup_spark_delay", cal_startup_spark_delay, sizeof(cal_startup_spark_delay), 0xC18Au, 1u},
    {"cal_startup_spark_repeat", cal_startup_spark_repeat, sizeof(cal_startup_spark_repeat), 0xC198u, 1u},
    {"cal_startup_spark_decay", cal_startup_spark_decay, sizeof(cal_startup_spark_decay), 0xC1ABu, 1u},
    {"cal_knock_attack_rate", cal_knock_attack_rate, sizeof(cal_knock_attack_rate), 0xC1BEu, 1u},
    {"cal_knock_recovery_rate", cal_knock_recovery_rate, sizeof(cal_knock_recovery_rate), 0xC1C3u, 1u},
    {"cal_knock_wot_limit", cal_knock_wot_limit, sizeof(cal_knock_wot_limit), 0xC1C8u, 1u},
    {"cal_crank_rpm_mult", cal_crank_rpm_mult, sizeof(cal_crank_rpm_mult), 0xC35Bu, 1u},
    {"cal_crank_coolant_pw", cal_crank_coolant_pw, sizeof(cal_crank_coolant_pw), 0xC36Eu, 1u},
    {"cal_crank_drp_mult", cal_crank_drp_mult, sizeof(cal_crank_drp_mult), 0xC37Fu, 1u},
    {"cal_crank_tps_mult", cal_crank_tps_mult, sizeof(cal_crank_tps_mult), 0xC391u, 1u},
};

int main(void)
{
    unsigned char rom[16384];
    size_t i, j;
    FILE *file = fopen("evidence/firmware/bua-hac.bin", "rb");
    if (!file || fread(rom, 1, sizeof(rom), file) != sizeof(rom)) return 1;
    fclose(file);
    for (i = 0; i < sizeof(tables)/sizeof(tables[0]); ++i) {
        for (j = 0; j < tables[i].length; ++j) {
            unsigned int address = tables[i].address + (unsigned int)j * tables[i].stride;
            if (((const unsigned char *)tables[i].data)[j] != rom[address - 0xC000u]) {
                fprintf(stderr, "%s mismatch at $%04X\n", tables[i].name, address);
                return 1;
            }
        }
    }
    printf("%u calibration tables match the ROM\n", (unsigned int)i);
    return 0;
}
