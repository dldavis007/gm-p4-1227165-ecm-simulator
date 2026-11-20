static void bua_factory_irq_step117(void)
{
    bua_factory_service(sim_factory_battery_adc117, sim_factory_diagnostic_adc117,
                        sim_factory_fmd_byte1_117, sim_factory_fmd_byte2_117);
}

/* The HAL owns physical keep-alive power. The PC records and latches the
   exact point where $D6EA enters the software-interrupt loop. */
static void bua_hal_request_soft_powerdown_step111(void)
{
    if(sim_soft_powerdown_latched==0u) {
        sim_soft_powerdown_latched=1u;
        ++stats.soft_powerdown_events;
        bua_vector_note_swi_step119(0xD6EAu);
    }
}
