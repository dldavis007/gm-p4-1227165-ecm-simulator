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

/* Step 164: factory control repeatedly samples the same battery, diagnostic,
 * and two-byte FMD resources used by the named raw HAL. Refresh only while the
 * factory IRQ route is selected; ordinary scheduler behavior is unchanged. */
static void bua_lifecycle_irq_from_hal_step164(void)
{
    if((RAM8(0x0047u)&FACTORY117_MODE_BIT)!=0u) {
        sim_factory_battery_adc117=hw_adc(0x10u);
        sim_factory_diagnostic_adc117=hw_adc(0x70u);
        sim_factory_fmd_byte1_117=sim_normal_fmd_byte1;
        sim_factory_fmd_byte2_117=sim_normal_fmd_byte2;
    }
    bua_lifecycle_irq_step121();
}

/* Step 165: the factory low-bit path scans U10 selectors $00 through $B0.
 * Refresh the eleven schematic-identified selector slots from their named raw
 * HAL backing before factory execution.  Slot $B0 remains an explicit unnamed
 * factory boundary and is deliberately preserved. */
static void bua_lifecycle_irq_from_hal_step165(void)
{
    bua_u8 i;
    if((RAM8(0x0047u)&FACTORY117_MODE_BIT)!=0u) {
        for(i=0u;i<11u;++i)
            sim_factory_adc118[i]=hw_adc((bua_u8)(i<<4));
    }
    bua_lifecycle_irq_from_hal_step164();
}
