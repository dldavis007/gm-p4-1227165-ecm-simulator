/* Processor-facing operations supplied by the host peripheral model. */
static bua_u8 adc_sample(bua_u8 channel);
static bua_u8 bua_hal_fmd_enabled(void);
static bua_u8 bua_hal_fmd_byte1(void);
static bua_u8 bua_hal_fmd_byte2(void);
static bua_u8 bua_hal_startup_fmd(void);
static void bua_hal_set_startup_fmd(bua_u8 status);

static bua_u8 bua_hal_minor_irq_status(void);
static bua_u16 bua_hal_vss_capture(void);
static bua_u8 bua_hal_take_injector_event(void);
static bua_u8 bua_hal_iac_motor_on(void);
static void bua_hal_drive_iac_motor(bua_u8 on);
static void serial_line_low_at(bua_u32 time_count);
static void serial_line_high_at(bua_u32 time_count);

static bua_u8 bua_hal_powerdown_latched(void);
static void bua_hal_request_soft_powerdown_step111(void);
static void bua_hal_seed_factory_inputs(bua_u8 battery, bua_u8 diagnostic, bua_u8 fmd);
static void bua_hal_refresh_factory_adc(unsigned int channels);
static bua_u8 bua_hal_factory_adc(unsigned int channel);
static bua_u8 bua_hal_factory_stopped(void);
static void bua_hal_factory_stop(bua_u8 reason);
