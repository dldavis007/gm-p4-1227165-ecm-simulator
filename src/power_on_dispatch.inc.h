/* Step 120: unified source-ordered power-on composition, $C800..$C9F3. */
#define POWER120_OUTCOME_NONE     0u
#define POWER120_OUTCOME_NORMAL   1u
#define POWER120_OUTCOME_FACTORY  2u
#define POWER120_OUTCOME_SWI      3u
#define POWER120_OUTCOME_INVALID  4u

typedef struct BuaPowerOnInput120Tag {
    bua_u8 err51_at_entry;
    bua_u8 socket_checksum_valid;
    bua_u8 battery_adc;
    bua_u8 pump_adc;
    bua_u8 diagnostic_adc;
    bua_u8 hud_present;
    bua_u8 initial_fmd_byte1;
    bua_u16 whole_rom_checksum;
} BuaPowerOnInput120;

typedef struct BuaPowerOnTrace120Tag {
    bua_u32 calls;
    bua_u32 volatile_ram_clears;
    bua_u32 retained_recoveries;
    bua_u32 hud_boundaries;
    bua_u32 normal_handoffs;
    bua_u32 factory_handoffs;
    bua_u32 swi_boundaries;
    bua_u32 vector_acknowledgements;
    bua_u8 retained_checksum_valid;
    bua_u8 outcome;
} BuaPowerOnTrace120;
static BuaPowerOnTrace120 bua_power_on_trace120;

static void bua_power_error_lamp_step120(void)
{
    bua_u16 csr;
    bua_u8 hi;
    bua_u8 lo;
    csr=mpu16be_get(0x3FFCu);
    hi=(bua_u8)(csr>>8);
    lo=(bua_u8)csr;
    lo=(bua_u8)(lo&0xF7u);
    lo=(bua_u8)((lo&0xFEu)|0x02u);
    hi=(bua_u8)(hi|0xFBu);
    mpu16be_set(0x3FFCu,(bua_u16)(((bua_u16)hi<<8)|lo));
}

static bua_u8 bua_power_on_step120(BuaPowerOnInput120 in)
{
    bua_u16 address;
    bua_u8 retained_valid;
    BuaStartupInput112 audit_input;
    memset(&bua_power_on_trace120,0,sizeof(bua_power_on_trace120));
    ++bua_power_on_trace120.calls;

    /* $C800..$C824: retain low RAM, initialize mapped MPU state. */
    if(in.err51_at_entry!=0u)
        MODE_WORD3=(bua_u8)(MODE_WORD3|0x08u);
    else
        MODE_WORD3=(bua_u8)(MODE_WORD3&0xF7u);
    mem.io5000=0x08u;
    mpu16be_set(0x3FFCu,in.err51_at_entry ? STEP112_MPU_BOOT_ERR51 :
                                             STEP112_MPU_BOOT_NORMAL);
    address=0x3FC0u;
    while(address<0x3FFAu) {
        mpu16be_set(address,0u);
        address=(bua_u16)(address+2u);
    }

    /* $C826..$C85D, including the exact volatile-RAM clear endpoints. */
    mem.io4000[1]=0x8Cu;
    mem.io4000[3]=0x8Fu;
    mem.io4000[4]=0x90u;
    address=0x01B2u;
    while(address!=0x002Du) {
        RAM8(address)=0u;
        --address;
        ++bua_power_on_trace120.volatile_ram_clears;
    }
    mem.io4000[4]=(bua_u8)(mem.io4000[4]|0x08u);
    mem.io4000[2]=(bua_u8)(mem.io4000[2]&0xFBu);
    RAM8(0x0030u)=0x04u;
    RAM8(0x0049u)=in.initial_fmd_byte1;

    /* $C85F..$C88B: ROM result is supplied at the explicit ROM boundary. */
    if(in.socket_checksum_valid!=0u)
        RAM8(0x0047u)=(bua_u8)(RAM8(0x0047u)&0xEFu);
    else
        RAM8(0x0047u)=(bua_u8)(RAM8(0x0047u)|0x10u);
    mpu16be_set(0x3FFCu,in.err51_at_entry ? STEP112_MPU_RUN_ERR51 :
                                             STEP112_MPU_RUN_NORMAL);
    RAM8(0x007Eu)=in.battery_adc;

    retained_valid=(bua_u8)(bua_error_word_checksum_step113()==
                            ram16be_get(0x0018u));
    bua_power_on_trace120.retained_checksum_valid=retained_valid;
    memset(&audit_input,0,sizeof(audit_input));
    audit_input.err51_at_entry=in.err51_at_entry;
    audit_input.socket_checksum_valid=in.socket_checksum_valid;
    audit_input.battery_adc=in.battery_adc;
    audit_input.pump_adc=in.pump_adc;
    audit_input.diagnostic_adc=in.diagnostic_adc;
    audit_input.hud_present=in.hud_present;
    audit_input.retained_checksum_valid=retained_valid;
    audit_input.fmd_status_low2=(bua_u8)(in.initial_fmd_byte1&0x03u);
    bua_startup_last112=bua_startup_evaluate_step112(audit_input);

    /* $C895..$C8DF: executable unsigned tests choose factory first. */
    if(in.battery_adc<100u) {
        RAM8(0x0040u)=(bua_u8)(RAM8(0x0040u)|0x20u);
        if(in.pump_adc>=160u && in.diagnostic_adc>=40u &&
           in.diagnostic_adc<100u) {
            bua_factory_boot_step117(in.whole_rom_checksum,
                                     in.initial_fmd_byte1);
            sim_factory_battery_adc117=in.battery_adc;
            sim_factory_diagnostic_adc117=in.diagnostic_adc;
            sim_factory_fmd_byte1_117=in.initial_fmd_byte1;
            ++bua_power_on_trace120.factory_handoffs;
            bua_power_on_trace120.outcome=POWER120_OUTCOME_FACTORY;
            return bua_power_on_trace120.outcome;
        }
    }

    /* $C8E2 optional ROM is absent; record only the verified call boundary. */
    if(in.hud_present!=0u)
        ++bua_power_on_trace120.hud_boundaries;

    /* $C8ED..$C90C: validate current retained bytes, then recover if bad. */
    if(retained_valid==0u) {
        bua_retained_recovery_step113(0u);
        ++bua_power_on_trace120.retained_recoveries;
    }

    /* $C90E..$C939: Error 51 handling and the source's SWI wait decision. */
    if((RAM8(0x0047u)&0x10u)!=0u) {
        MODE_WORD3=(bua_u8)(MODE_WORD3|0x08u);
        bua_power_error_lamp_step120();
        RAM8(0x0007u)=(bua_u8)(RAM8(0x0007u)|0x01u);
        RAM8(0x0047u)=(bua_u8)(RAM8(0x0047u)|0x10u);
        ram16be_set(0x0018u,bua_error_word_checksum_step113());
        if((RAM8(0x0047u)&0x80u)==0u && in.diagnostic_adc>=40u) {
            bua_vector_note_swi_step119(0xC938u);
            ++bua_power_on_trace120.swi_boundaries;
            bua_power_on_trace120.outcome=POWER120_OUTCOME_SWI;
            return bua_power_on_trace120.outcome;
        }
    }

    sim_startup_fmd_status=in.initial_fmd_byte1;
    bua_startup_normal_step114();
    ++bua_power_on_trace120.normal_handoffs;
    bua_power_on_trace120.outcome=POWER120_OUTCOME_NORMAL;
    return bua_power_on_trace120.outcome;
}

/* Explicit host acknowledgement of a Step-119 vector request to $C800. */
static bua_u8 bua_power_on_from_vector_step120(bua_u16 vector_address,
                                               BuaPowerOnInput120 in)
{
    bua_u8 outcome;
    if(vector_address<0xFFF8u || vector_address>0xFFFEu ||
       (vector_address&1u)!=0u)
        return POWER120_OUTCOME_INVALID;
    if(bua_vector_dispatch_step119(vector_address)!=0xC800u)
        return POWER120_OUTCOME_INVALID;
    outcome=bua_power_on_step120(in);
    ++bua_power_on_trace120.vector_acknowledgements;
    return outcome;
}
