/*
 * Step 117: factory-test boot and control path.
 * Authoritative listing ranges: $C8B2..$C8DF, $CA6F..$CA73,
 * $FC72..$FD28.  The custom-control/output exercises at $FD29 are deferred.
 */

#define FACTORY117_MODE_BIT          0x80u
#define FACTORY117_SPARK_ENABLE_BIT  0x04u
#define FACTORY117_MINOR_LIMIT         96u
#define FACTORY117_LOW_BATTERY_LIMIT   40u
#define FACTORY117_IGN_ON_LIMIT         90u
#define FACTORY117_IGN_OFF_LIMIT       160u
#define FACTORY117_SWI_NONE              0u
#define FACTORY117_SWI_DIAGNOSTIC        1u
#define FACTORY117_SWI_POWERDOWN         2u

/* Literal pointer words emitted at $FC72..$FCA0. */
static const bua_u16 bua_factory_table117[24] = {
    0x017Bu,0x017Du,0x017Fu,0x0181u,0x0183u,0x0185u,0x0187u,0x0049u,
    0xC000u,0xC002u,0xC004u,0x0173u,0x0175u,0x4002u,0x0047u,0x0000u,
    0x3FC0u,0x3FC2u,0x3FC4u,0x3FC6u,0x3FC8u,0x3FCAu,0x3FE0u,0x3FF8u
};

typedef struct BuaFactoryTrace117Tag {
    bua_u32 boot_entries;
    bua_u32 aa_fills;
    bua_u32 irq_routes;
    bua_u32 fmd_exchanges;
    bua_u32 mode_changes;
    bua_u32 serial_160_cadence;
    bua_u32 serial_8192_cadence;
    bua_u32 diagnostic_swi_boundaries;
    bua_u32 powerdown_swi_boundaries;
    bua_u32 watchdog_strobes;
    bua_u32 deferred_output_boundaries;
    bua_u8 last_match;
    bua_u8 last_fmd_tx1;
    bua_u8 last_fmd_tx2;
} BuaFactoryTrace117;

static BuaFactoryTrace117 bua_factory_trace117;
static bua_u8 sim_factory_battery_adc117=128u;
static bua_u8 sim_factory_diagnostic_adc117=40u;
static bua_u8 sim_factory_fmd_byte1_117=0u;
static bua_u8 sim_factory_fmd_byte2_117=0u;
static bua_u8 sim_factory_swi_reason117=FACTORY117_SWI_NONE;

/* Defined by the following Step-118 implementation fragment. */
static void bua_factory_execute_step118(void);

/* LF49A with A=$FB, used at $FCB7, clears low-byte CSR bit 2. */
static void bua_factory_select_8192_step117(void)
{
    bua_u16 csr;
    bua_u8 hi;
    bua_u8 lo;
    csr=mpu16be_get(0x3FFCu);
    hi=(bua_u8)(csr>>8);
    lo=(bua_u8)csr;
    lo=(bua_u8)(lo&0xFBu);
    lo=(bua_u8)((lo&0xFEu)|0x02u);
    hi=(bua_u8)(hi|0xFBu);
    mpu16be_set(0x3FFCu,(bua_u16)(((bua_u16)hi<<8)|lo));
}

/* $C8B2..$C8DF, followed by the shared $C9E3 timer arm. */
static void bua_factory_boot_step117(bua_u16 eprom_checksum,
                                     bua_u8 initial_fmd_byte1)
{
    bua_u16 address;
    ++bua_factory_trace117.boot_entries;
    RAM8(0x0047u)=(bua_u8)(RAM8(0x0047u)|FACTORY117_MODE_BIT);
    RAM8(0x0031u)=(bua_u8)(RAM8(0x0031u)|FACTORY117_SPARK_ENABLE_BIT);

    /* LF32E's whole-ROM result is supplied at the ROM/HAL boundary. */
    ram16be_set(0x0173u,eprom_checksum);
    RAM8(0x0179u)=204u;
    RAM8(0x0032u)=204u;
    RAM8(0x0049u)=initial_fmd_byte1;
    if((initial_fmd_byte1&0x03u)==0u) {
        for(address=0u;address<0x002Eu;++address)
            RAM8(address)=0xAAu;
        ++bua_factory_trace117.aa_fills;
    }
    mem.io4000[6]=(bua_u8)(mem.io4000[5]+2u);
    mem.io4000[7]=(bua_u8)(mem.io4000[7]|0x01u);
}

/* FDA7: diagnostic A/D values outside the emitted [40,100) interval SWI. */
static bua_u8 bua_factory_diag_valid_step117(bua_u8 diagnostic_adc)
{
    if(diagnostic_adc<40u || diagnostic_adc>=100u) {
        sim_factory_swi_reason117=FACTORY117_SWI_DIAGNOSTIC;
        bua_vector_note_swi_step119(0xFDB3u);
        ++bua_factory_trace117.diagnostic_swi_boundaries;
        return 0u;
    }
    return 1u;
}

/*
 * $FCA2..$FD28.  SPI replies and A/D results are HAL stimuli.  After the
 * emitted COP restart, the listing continues into custom-control and physical
 * output exercises; Step 117 records that boundary without inventing them.
 */
static void bua_factory_irq_step117(void)
{
    bua_u8 timer;
    bua_u8 mode;
    bua_u8 counter;
    bua_u8 battery;

    if(sim_factory_swi_reason117!=FACTORY117_SWI_NONE)
        return;
    ++bua_factory_trace117.irq_routes;
    RAM8(0x018Cu)=mem.io4000[6];
    mode=(bua_u8)(RAM8(0x0048u)&0x03u);
    if(mode!=0u) {
        mem.io4000[6]=(bua_u8)(mem.io4000[6]+11u);
        SERIAL_MODE_WORD=(bua_u8)(SERIAL_MODE_WORD|0x03u);
        bua_factory_select_8192_step117();
        ++bua_factory_trace117.serial_8192_cadence;
    } else {
        mem.io4000[6]=(bua_u8)(mem.io4000[6]+205u);
        SERIAL_MODE_WORD=(bua_u8)(SERIAL_MODE_WORD&0xFCu);
        ++bua_factory_trace117.serial_160_cadence;
    }
    bua_factory_trace117.last_match=mem.io4000[6];

    if(bua_factory_diag_valid_step117(sim_factory_diagnostic_adc117)==0u)
        return;

    bua_factory_trace117.last_fmd_tx1=(bua_u8)(RAM8(0x0031u)|0x80u);
    bua_factory_trace117.last_fmd_tx2=(bua_u8)(RAM8(0x0031u)&0x7Fu);
    RAM8(0x0049u)=sim_factory_fmd_byte1_117;
    RAM8(0x004Au)=sim_factory_fmd_byte2_117;
    mem.io4000[2]=(bua_u8)(mem.io4000[2]&0xFBu);
    ++bua_factory_trace117.fmd_exchanges;

    counter=(bua_u8)(RAM8(0x0172u)+1u);
    if(counter>=FACTORY117_MINOR_LIMIT)
        counter=0u;
    RAM8(0x0172u)=counter;

    battery=sim_factory_battery_adc117;
    timer=RAM8(0x0032u);
    if(battery<=FACTORY117_LOW_BATTERY_LIMIT) {
        if(timer>=FACTORY117_IGN_OFF_LIMIT) {
            sim_factory_swi_reason117=FACTORY117_SWI_POWERDOWN;
            bua_vector_note_swi_step119(0xFD03u);
            ++bua_factory_trace117.powerdown_swi_boundaries;
            return;
        }
        timer=(bua_u8)(timer+1u);
    } else if(timer!=0u && battery>=FACTORY117_IGN_ON_LIMIT) {
        timer=0u;
        mode=(bua_u8)(RAM8(0x0049u)&0x03u);
        /* CMPA $48 compares the masked new value with the complete old byte. */
        if(mode!=RAM8(0x0048u)) {
            RAM8(0x0048u)=mode;
            RAM8(0x0189u)=0u;
            RAM8(0x0172u)=0u;
            RAM8(0x018Au)=0xFFu;
            ++bua_factory_trace117.mode_changes;
        }
    }
    RAM8(0x0032u)=timer;
    mem.io4000[0x0Bu]=0xFFu;
    mem.io4000[0x0Cu]=0x00u;
    ++bua_factory_trace117.watchdog_strobes;
    ++bua_factory_trace117.deferred_output_boundaries;
    bua_factory_execute_step118();
}
