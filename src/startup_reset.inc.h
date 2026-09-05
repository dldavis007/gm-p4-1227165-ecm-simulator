/*
 * Step 112: source-ordered reset/startup front, BUA listing $C800..$C9F3.
 *
 * This model records the executable decisions made before the ordinary IRQ
 * scheduler starts.  Hardware-only actions are reported as boundary events;
 * it does not invent the internals of the MPU, A/D converter, FMD, optional
 * HUD ROM, SWI, or 8192-baud service.
 */

#define STEP112_MPU_BOOT_NORMAL 0x000Au
#define STEP112_MPU_BOOT_ERR51  0x0002u
#define STEP112_MPU_RUN_NORMAL  0xFB1Au
#define STEP112_MPU_RUN_ERR51   0xFB12u

typedef struct BuaStartupInput112Tag {
    bua_u8 err51_at_entry;
    bua_u8 socket_checksum_valid;
    bua_u8 battery_adc;
    bua_u8 pump_adc;
    bua_u8 diagnostic_adc;
    bua_u8 hud_present;
    bua_u8 retained_checksum_valid;
    bua_u8 retained_error_latched;
    bua_u8 fmd_status_low2;
} BuaStartupInput112;

typedef struct BuaStartupResult112Tag {
    bua_u16 boot_mpu_word;
    bua_u16 run_mpu_word;
    bua_u8 io_prescaler;
    bua_u8 io_direction;
    bua_u8 io_control;
    bua_u8 socket_error51;
    bua_u8 factory_test;
    bua_u8 factory_fill_aa;
    bua_u8 hud_call;
    bua_u8 retained_reinitialized;
    bua_u8 swi_wait;
    bua_u8 serial_8192_boundary;
    bua_u8 scheduler_entered;
} BuaStartupResult112;

static BuaStartupResult112 bua_startup_last112;

static BuaStartupResult112 bua_startup_evaluate_step112(BuaStartupInput112 in)
{
    BuaStartupResult112 r;
    memset(&r, 0, sizeof(r));

    /* $C800..$C832: stack/delay, MPU mapping and peripheral setup. */
    r.boot_mpu_word = in.err51_at_entry ? STEP112_MPU_BOOT_ERR51 :
                                         STEP112_MPU_BOOT_NORMAL;
    r.io_prescaler = 0x8Cu;
    r.io_direction = 0x8Fu;
    r.io_control = 0x98u; /* $90 at C830, then bit 3 set at C848. */

    /* $C85F..$C88B: socket checksum decision followed by run MPU word. */
    r.socket_error51 = in.socket_checksum_valid ? 0u : 1u;
    r.run_mpu_word = in.err51_at_entry ? STEP112_MPU_RUN_ERR51 :
                                        STEP112_MPU_RUN_NORMAL;

    /*
     * $C88E..$C8B5 uses unsigned branches exactly as emitted.  Factory mode
     * is reached only for battery <100, pump >=160 and 40<=diagnostic<100.
     * Several listing comments describe the comparisons oppositely; the
     * opcodes and branch destinations control this implementation.
     */
    if (in.battery_adc < 100u && in.pump_adc >= 160u &&
        in.diagnostic_adc >= 40u && in.diagnostic_adc < 100u) {
        r.factory_test = 1u;
        if ((in.fmd_status_low2 & 0x03u) == 0u)
            r.factory_fill_aa = 1u;
        r.scheduler_entered = 1u; /* factory path jumps directly to $C9E3 */
        return r;
    }

    /* $C8E2..$C90C: optional-ROM boundary and retained-RAM recovery. */
    r.hud_call = in.hud_present ? 1u : 0u;
    r.retained_reinitialized = in.retained_checksum_valid ? 0u : 1u;

    /* $C90E..$C939: latched error plus diagnostic input can enter SWI wait. */
    if ((in.retained_error_latched != 0u || r.socket_error51 != 0u) &&
        in.diagnostic_adc >= 40u) {
        r.swi_wait = 1u;
        return r;
    }

    /* $C93B..$C9F3: normal initializers, 8192 service, timer, scheduler. */
    r.serial_8192_boundary = 1u;
    r.scheduler_entered = 1u;
    return r;
}

static void bua_startup_apply_normal_step112(void)
{
    BuaStartupInput112 in;
    memset(&in, 0, sizeof(in));
    in.socket_checksum_valid = 1u;
    in.battery_adc = hw_adc(0x10u);
    in.pump_adc = 0u;
    in.diagnostic_adc = hw_adc(0x70u);
    in.retained_checksum_valid = 0u;
    bua_startup_last112 = bua_startup_evaluate_step112(in);

    /* Preserve the established PC reset state while making the source-order
     * decisions observable.  These assignments already belong to ecm_reset's
     * Step-111 normal outcome and therefore do not broaden runtime behavior. */
    mem.io5000 = 0x08u;
    RAM8(0x00C6u) = 128u;
    RAM8(0x00C9u) = 128u;
    RAM8(0x002Cu) = 144u;
    RAM8(0x010Cu) = 144u;
    ALCL_TABLE_INDEX = 0x19u;
}

/* LF3A7: D starts at one and sums exactly L0005..L0009 with 16-bit wrap. */
static bua_u16 bua_error_word_checksum_step113(void)
{
    bua_u16 sum;
    bua_u16 address;
    sum = 1u;
    for (address = 0x0005u; address <= 0x0009u; ++address)
        sum = (bua_u16)(sum + (bua_u16)RAM8(address));
    return sum;
}

/* LF434: emitted LDAA #$80 initializes both SAM bytes and all 16 BLM cells. */
static void bua_blm_initialize_step113(void)
{
    unsigned int i;
    RAM8(0x000Au) = 0x80u;
    RAM8(0x000Bu) = 0u;
    RAM8(0x000Cu) = 0x80u;
    RAM8(0x000Du) = 0u;
    for (i = 0u; i < 16u; ++i)
        RAM8((bua_u16)(0x001Cu + i)) = 0x80u;
}

/* $C8ED..$C90C invalid-retained-RAM recovery in emitted order. */
static void bua_retained_recovery_step113(bua_u8 checksum_valid)
{
    bua_u16 address;
    if (checksum_valid != 0u)
        return;
    /* LC8F8 clears $002D down through $0001; address zero is not written. */
    address = 0x002Du;
    while (address != 0u) {
        RAM8(address) = 0u;
        --address;
    }
    ram16be_set(0x0018u, bua_error_word_checksum_step113());
    bua_blm_initialize_step113();
    RAM8(0x002Cu) = 144u; /* LC62F in the verified 9340 listing. */
    RAM8(0x003Du) = 0x40u;
}
