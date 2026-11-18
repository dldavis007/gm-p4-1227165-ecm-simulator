/* Host bridge for the independent ROM differential. */
#include <assert.h>
static unsigned int addresses[128], values[128], count;
static void record_write(unsigned int address, unsigned int value)
{
    assert(count + 2 <= 128);
    addresses[count] = address;
    values[count++] = value >> 8;
    addresses[count] = address + 1;
    values[count++] = value & 255;
}
#define BUA_TRACE_MPU_WRITE(a, v) record_write(a, v)
#define main regression_main
#include "../main.c"
#undef main

void probe_reset(void)
{
    memset(&mem, 0, sizeof(mem));
    sim_ecm_ref.period_valid = 1u;
}
unsigned char *probe_io(void) { return mem.io4000; }
unsigned char *probe_ram(void) { return mem.low; }
unsigned char *probe_mpu(void) { return mem.mpu; }
unsigned int probe_trace_size(void) { return count; }
int probe_address(unsigned int i) { return addresses[i]; }
int probe_value(unsigned int i) { return values[i]; }
int probe_fmd_reads(void) { return (int)stats.spark_fmd_reads; }
void probe_tick(void)
{
    count = 0;
    stats.spark_fmd_reads = 0;
    bua_reference_state_12p5ms();
}

void probe_irq(void) { count = 0; irq_6p25ms(); }
void probe_irq_setup(void)
{
    ecm_reset();
    sim_normal_fmd_enabled = 1u;
}
int probe_adc(unsigned int channel) { return hw_adc((bua_u8)channel); }
int probe_fmd(unsigned int command)
{
    return (command & 128u) ? sim_normal_fmd_byte2 : sim_normal_fmd_byte1;
}
int probe_filter(unsigned int old, unsigned int value, unsigned int coef)
{
    return bua_lag_filter_8_8((bua_u16)old, (bua_u8)value, (bua_u8)coef);
}
