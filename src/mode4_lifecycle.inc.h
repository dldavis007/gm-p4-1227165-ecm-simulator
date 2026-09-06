/* Step 116: source-ordered Mode-4 lifecycle, listing $CB5F..$CBB4/$CBCA..$CBD9. */
typedef struct BuaMode4Trace116Tag {
    bua_u32 scheduler_calls;
    bua_u32 entries;
    bua_u32 exits;
    bua_u32 error_resets;
    bua_u32 iac_resets;
    bua_u32 blm_resets;
    bua_u32 timeout_forces;
} BuaMode4Trace116;
static BuaMode4Trace116 bua_mode4_trace116;

static void bua_mode4_scheduler_step116(void)
{
    bua_u8 command3;
    ++bua_mode4_trace116.scheduler_calls;
    if((SERIAL_MODE_WORD&0x08u)==0u && (MINOR_MODE_WORD2&0x20u)==0u)
        return;
    if(RAM8(0x0151u)==4u) {
        if((MINOR_MODE_WORD2&0x08u)!=0u)
            return;
        MINOR_MODE_WORD2=(bua_u8)(MINOR_MODE_WORD2|0x08u);
        ++bua_mode4_trace116.entries;
        command3=RAM8(0x0154u);
        if((command3&0x40u)!=0u) {
            RAM8(0x0005u)=0u;
            RAM8(0x0006u)=0u;
            RAM8(0x0007u)=0u;
            RAM8(0x0008u)=0u;
            RAM8(0x0009u)=0u;
            ram16be_set(0x0018u,bua_error_word_checksum_step113());
            ++bua_mode4_trace116.error_resets;
        }
        if((command3&0x20u)!=0u) {
            RAM8(0x00F3u)=(bua_u8)(RAM8(0x00F3u)|0x04u);
            RAM8(0x002Cu)=255u;
            ++bua_mode4_trace116.iac_resets;
        }
        if((command3&0x10u)!=0u) {
            bua_blm_initialize_step113();
            ++bua_mode4_trace116.blm_resets;
        }
        return;
    }
    if((MINOR_MODE_WORD2&0x08u)!=0u) {
        MINOR_MODE_WORD2=(bua_u8)(MINOR_MODE_WORD2&0xF7u);
        SPARK_MODE_WORD=(bua_u8)(SPARK_MODE_WORD&0xFEu);
        RAM8(0x003Fu)=(bua_u8)(RAM8(0x003Fu)&0xFEu);
        RAM8(0x00B4u)=0u;
        ++bua_mode4_trace116.exits;
    }
}

static void bua_mode4_timeout_step116(void)
{
    bua_u8 timer;
    if(sim_legacy_mode4_lifecycle_freeze!=0u)
        return;
    timer=(bua_u8)(RAM8(0x0171u)+1u);
    RAM8(0x0171u)=timer;
    if(timer>30u) {
        RAM8(0x0151u)=0u;
        ++bua_mode4_trace116.timeout_forces;
    }
}
