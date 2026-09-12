/*
 * Step 128: explicit processor-visible HAL boundary.
 * Step 155: named raw setters for additional already-backed ECM inputs.
 *
 * This layer intentionally exposes raw ECM-facing stimuli and observations.
 * It does not convert engineering units other than the two already-existing
 * pulse-source helpers (VSS MPH and reference RPM), and it does not assign
 * electrical polarity or physical-device behavior to raw output state.
 *
 * Keep this file C89-compatible and free of vehicle-plant assumptions.
 */

#define BUA_HAL_MPU_BASE   0x3FC0u
#define BUA_HAL_MPU_END    0x3FFFu
#define BUA_HAL_IO_BASE    0x4000u
#define BUA_HAL_IO_END     0x400Fu

/* Established raw/processor-facing input stimuli. */
static void bua_hal_set_o2_adc(bua_u8 raw)
{
    sim_o2_adc=raw;
}

static void bua_hal_set_battery_adc(bua_u8 raw)
{
    BATTERY_AD=raw;
}

/* Step 155 names the raw U10-facing simulator bytes that already exist in the
 * translated harness. These setters deliberately do not inject processed TPS,
 * airflow/load, or diagnostic mode state; firmware still performs the
 * established selector/storage/normalization work after hw_adc() sees them.
 */
static void bua_hal_set_maf_adc(bua_u8 raw)
{
    sim_maf_adc=raw;
}

static void bua_hal_set_tps_adc(bua_u8 raw)
{
    sim_tps_adc=raw;
}

static void bua_hal_set_diag_adc(bua_u8 raw)
{
    sim_diag_adc=raw;
}

/* $3FCA is the processor-visible U9 counter whose changes are consumed as the
 * knock-event quantity by the listing-backed firmware. This setter exposes the
 * raw register state only. It does NOT claim that one external KNOCK edge maps
 * one-for-one to one counter increment, because that U9 transformation remains
 * an unresolved custom-device boundary.
 */
static void bua_hal_set_u9_knock_counter(bua_u16 raw)
{
    mpu16be_set(0x3FCAu,raw);
}

/* These delegate to the existing phase/timing sources. The MPH/RPM values are
 * simulator commands, while the resulting pulse periods are what the ECM sees.
 */
static void bua_hal_set_vss_mph(unsigned int mph)
{
    sim_set_vss_mph(mph);
}

static void bua_hal_set_reference_rpm(unsigned int rpm)
{
    sim_set_ecm_reference_rpm(rpm);
}

/* Raw RAM injection is deliberately generic. It is useful for hardware-facing
 * tests where the translated firmware already names the processor-visible RAM
 * byte but the external electrical transfer is still unresolved.
 */
static void bua_hal_set_ram8(bua_u16 address,bua_u8 value)
{
    if(address<0x0200u)
        RAM8(address)=value;
}

static bua_u8 bua_hal_get_ram8(bua_u16 address)
{
    if(address<0x0200u)
        return RAM8(address);
    return 0u;
}

/* Raw MPU/I/O observers. They report the translated register windows without
 * interpreting electrical polarity, driver current, waveform shape, or load.
 */
static bua_u8 bua_hal_get_mpu8(bua_u16 address)
{
    if(address>=BUA_HAL_MPU_BASE && address<=BUA_HAL_MPU_END)
        return mem.mpu[(unsigned int)(address-BUA_HAL_MPU_BASE)];
    return 0u;
}

static bua_u16 bua_hal_get_mpu16be(bua_u16 address)
{
    if(address>=BUA_HAL_MPU_BASE && address<BUA_HAL_MPU_END)
        return mpu16be_get(address);
    return 0u;
}

static bua_u8 bua_hal_get_io4000(bua_u16 address)
{
    if(address>=BUA_HAL_IO_BASE && address<=BUA_HAL_IO_END)
        return mem.io4000[(unsigned int)(address-BUA_HAL_IO_BASE)];
    return 0u;
}

static bua_u8 bua_hal_get_io5000(void)
{
    return mem.io5000;
}

/* Named observers for high-value hardware boundaries already established by
 * listing/schematic work. These remain raw software-visible quantities.
 */
static bua_u16 bua_hal_get_injector_pw_counts(void)
{
    return mpu16be_get(0x3FD0u);
}

static bua_u8 bua_hal_get_iac_position(void)
{
    return RAM8(0x002Cu);
}

static bua_u8 bua_hal_get_output_3fcc(void)
{
    return bua_hal_get_mpu8(0x3FCCu);
}

static bua_u8 bua_hal_get_output_3fd2(void)
{
    return bua_hal_get_mpu8(0x3FD2u);
}

static bua_u8 bua_hal_get_output_3fd4(void)
{
    return bua_hal_get_mpu8(0x3FD4u);
}

static bua_u8 bua_hal_get_output_3fd6(void)
{
    return bua_hal_get_mpu8(0x3FD6u);
}

static bua_u8 bua_hal_get_output_3fd8(void)
{
    return bua_hal_get_mpu8(0x3FD8u);
}

static bua_u8 bua_hal_get_output_4004(void)
{
    return bua_hal_get_io4000(0x4004u);
}
