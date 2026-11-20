static void ecm_reset(void)
{
    memset(&mem, 0, sizeof(mem));
    memset(&stats, 0, sizeof(stats));
    bua_vector_reset_step119();
    memset(&bua_factory_trace117, 0, sizeof(bua_factory_trace117));
    sim_factory_battery_adc117 = 128u;
    sim_factory_diagnostic_adc117 = 40u;
    sim_factory_fmd_byte1_117 = 0u;
    sim_factory_fmd_byte2_117 = 0u;
    sim_factory_swi_reason117 = FACTORY117_SWI_NONE;
    sim_soft_powerdown_latched = 0u;
    sim_iac_motor_on = 1u;
    sim_maf_adc = 128u;
    sim_o2_adc = 128u;
    sim_tps_adc = 35u;
    sim_diag_adc = 200u;
    bua_factory_reset_step118();
    sim_vss_abs_ticks = 0ul;
    sim_vss_next_edge = 0ul;
    sim_vss_capture_reg = 0u;
    sim_vss_period_ticks = 0u;
    sim_serial_line_high = 1u;
    sim_vehicle_time_us = 0ul;
    memset(&sim_ecm_ref, 0, sizeof(sim_ecm_ref));
    memset(&dash, 0, sizeof(dash));
    memset(&sim_dash_vss, 0, sizeof(sim_dash_vss));
    memset(&sim_dash_tach, 0, sizeof(sim_dash_tach));
    rx_enabled = 0u;
    rx_reset();
    mem.io5000 = 0x08u;
    MINOR_COUNT = 0x0Eu;
    BLM = 128u;
    /* Step 113: LF434 emits LDAA #$80, not 120. */
    bua_blm_initialize_step113();
    RAM8(0x00C6u) = 128u;
    RAM8(0x00C9u) = 128u;
    ALCL_TABLE_INDEX = 0x19u;
    VSS_NO_EDGE_COUNT = 160u;
    RAM8(0x002Cu) = 144u;
    RAM8(0x010Cu) = 144u;
    ram16be_set(0x00CEu, 445u);
    RAM8(0x007Eu) = hw_adc(0x10u);
    RAM8(0x0081u) = hw_adc(0x50u);
    bua_startup_apply_normal_step112();
    /* Cold reset leaves ENGINE RUNNING clear; LCD4B..LCD8F qualifies it. */
}
