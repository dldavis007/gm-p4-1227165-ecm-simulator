/* Step 121: PC/HAL composition of reset, run, shutdown, and restart. */
typedef struct BuaLifecycleTrace121Tag {
    bua_u32 power_cycle_acknowledgements;
    bua_u32 power_on_outcomes;
    bua_u32 irq_requests;
    bua_u32 blocked_irq_requests;
    bua_u8 last_power_on_outcome;
} BuaLifecycleTrace121;
static BuaLifecycleTrace121 bua_lifecycle_trace121;

static void bua_lifecycle_reset_trace_step121(void)
{
    memset(&bua_lifecycle_trace121,0,sizeof(bua_lifecycle_trace121));
}

/*
 * Physical power/reset behavior is outside the PROM.  The host explicitly
 * acknowledges that boundary by releasing the Step-111 software-powerdown
 * latch, then dispatches one of the listing's four $C800 reset vectors.
 * RAM is deliberately not cleared here; $C800 owns its source-ordered clears.
 */
static bua_u8 bua_lifecycle_power_cycle_step121(bua_u16 vector_address,
                                                BuaPowerOnInput120 in)
{
    bua_u8 outcome;
    sim_soft_powerdown_latched=0u;
    ++bua_lifecycle_trace121.power_cycle_acknowledgements;
    outcome=bua_power_on_from_vector_step120(vector_address,in);
    bua_lifecycle_trace121.last_power_on_outcome=outcome;
    ++bua_lifecycle_trace121.power_on_outcomes;
    return outcome;
}

/* Step 163: source the four established processor-facing startup values from
 * the named raw HAL backing. ROM/checksum, HUD, entry-error and vector choices
 * remain explicit inputs because they are separate evidence boundaries. */
static bua_u8 bua_lifecycle_power_cycle_from_hal_step163(
                                                bua_u16 vector_address,
                                                BuaPowerOnInput120 in)
{
    in.battery_adc=hw_adc(0x10u);
    in.pump_adc=hw_adc(0x60u);
    in.diagnostic_adc=hw_adc(0x70u);
    in.initial_fmd_byte1=sim_normal_fmd_byte1;
    return bua_lifecycle_power_cycle_step121(vector_address,in);
}

static void bua_lifecycle_irq_step121(void)
{
    bua_u32 before;
    before=stats.irq_ticks;
    ++bua_lifecycle_trace121.irq_requests;
    irq_6p25ms();
    if(stats.irq_ticks==before)
        ++bua_lifecycle_trace121.blocked_irq_requests;
}
