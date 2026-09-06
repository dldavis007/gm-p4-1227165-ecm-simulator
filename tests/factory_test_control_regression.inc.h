static void run_step117_factory_control_test(void)
{
    unsigned int passed=0u;
    unsigned int total=24u;
    unsigned int i;
    bua_u32 ordinary_irqs;
#define S117(c,t) do { if(c) ++passed; printf("  %-78s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-117 factory-test boot/control regression:\n");

    S117((sizeof(bua_factory_table117)/sizeof(bua_factory_table117[0]))==24u &&
         bua_factory_table117[0]==0x017Bu && bua_factory_table117[7]==0x0049u &&
         bua_factory_table117[8]==0xC000u && bua_factory_table117[15]==0x0000u &&
         bua_factory_table117[16]==0x3FC0u && bua_factory_table117[23]==0x3FF8u,
         "$FC72..$FCA0 preserves all 24 emitted factory-data pointer words");

    ecm_reset();
    memset(&bua_factory_trace117,0,sizeof(bua_factory_trace117));
    RAM8(0x002Eu)=0x5Au;
    bua_factory_boot_step117(0x1234u,0u);
    S117((RAM8(0x0047u)&0x80u)!=0u && (RAM8(0x0031u)&0x04u)!=0u,
         "$C8B2 sets factory mode and enables the listing spark-control bit");
    S117(ram16be_get(0x0173u)==0x1234u && RAM8(0x0179u)==204u &&
         RAM8(0x0032u)==204u,
         "ROM-checksum boundary, IAC state and ignition timer retain boot values");
    for(i=0u;i<0x002Eu && RAM8((bua_u16)i)==0xAAu;++i) { }
    S117(i==0x002Eu && RAM8(0x002Eu)==0x5Au &&
         bua_factory_trace117.aa_fills==1ul,
         "zero FMD low bits fill exactly RAM $0000..$002D with $AA");
    S117(mem.io4000[6]==(bua_u8)(mem.io4000[5]+2u) &&
         (mem.io4000[7]&0x01u)!=0u,
         "factory boot reaches the shared $C9E3 timer-arm handoff");

    ecm_reset(); RAM8(0x0005u)=0x39u;
    bua_factory_boot_step117(0x5678u,1u);
    S117(RAM8(0x0005u)==0x39u && bua_factory_trace117.aa_fills==0ul,
         "nonzero FMD low bits bypass the optional AA RAM fill");

    memset(&bua_factory_trace117,0,sizeof(bua_factory_trace117));
    sim_factory_swi_reason117=0u; sim_factory_diagnostic_adc117=40u;
    sim_factory_battery_adc117=128u; sim_factory_fmd_byte1_117=0u;
    sim_factory_fmd_byte2_117=0xA5u; RAM8(0x0048u)=0u;
    mem.io4000[6]=10u; ordinary_irqs=stats.irq_ticks;
    irq_6p25ms();
    S117(bua_factory_trace117.irq_routes==1ul && stats.irq_ticks==ordinary_irqs,
         "$CA6F..$CA73 routes factory IRQs away from the ordinary scheduler");
    S117(RAM8(0x018Cu)==10u && mem.io4000[6]==215u &&
         (SERIAL_MODE_WORD&0x03u)==0u,
         "factory mode zero schedules +205 and clears both serial cadence bits");
    S117(RAM8(0x0049u)==0u && RAM8(0x004Au)==0xA5u &&
         bua_factory_trace117.fmd_exchanges==1ul,
         "$FCC9..$FCE6 stores both HAL-supplied FMD reply bytes");
    S117(bua_factory_trace117.last_fmd_tx1==
           (bua_u8)(RAM8(0x0031u)|0x80u) &&
         bua_factory_trace117.last_fmd_tx2==(bua_u8)(RAM8(0x0031u)&0x7Fu),
         "factory SPI exchange transmits the emitted set-bit7/clear-bit7 pair");
    S117(RAM8(0x0172u)==1u &&
         bua_factory_trace117.deferred_output_boundaries==1ul,
         "factory minor count advances before the custom-output boundary");
    S117(mem.io4000[0x0Bu]==0xFFu && mem.io4000[0x0Cu]==0u &&
         bua_factory_trace117.watchdog_strobes==1ul,
         "$FD23..$FD28 writes the literal $FF00 COP restart word");

    RAM8(0x0172u)=95u; bua_factory_irq_step117();
    S117(RAM8(0x0172u)==0u,
         "$FCEA..$FCF3 wraps the factory minor count at 96");

    RAM8(0x0048u)=2u; mem.io4000[6]=250u; mpu16be_set(0x3FFCu,0x120Fu);
    bua_factory_irq_step117();
    S117(mem.io4000[6]==5u && (SERIAL_MODE_WORD&0x03u)==0x03u &&
         mpu16be_get(0x3FFCu)==0xFB0Au,
         "nonzero factory mode schedules wrapped +11 and selects 8192 cadence");

    RAM8(0x0048u)=0u; RAM8(0x0032u)=7u;
    sim_factory_battery_adc117=40u; bua_factory_irq_step117();
    S117(RAM8(0x0032u)==8u,
         "battery equality 40 follows emitted BHI fall-through and increments timer");
    RAM8(0x0032u)=160u; bua_factory_irq_step117();
    S117(sim_factory_swi_reason117==FACTORY117_SWI_POWERDOWN &&
         bua_factory_trace117.powerdown_swi_boundaries==1ul,
         "low battery with timer 160 records the $FD03 SWI hardware boundary");

    sim_factory_swi_reason117=0u; sim_factory_battery_adc117=89u;
    RAM8(0x0032u)=12u; bua_factory_irq_step117();
    S117(RAM8(0x0032u)==12u,
         "battery 89 preserves a nonzero ignition-off timer");
    sim_factory_battery_adc117=90u; sim_factory_fmd_byte1_117=0u;
    RAM8(0x0032u)=12u; RAM8(0x0048u)=0u; bua_factory_irq_step117();
    S117(RAM8(0x0032u)==0u,
         "battery equality 90 clears the ignition-off timer");

    sim_factory_fmd_byte1_117=3u; RAM8(0x0032u)=9u;
    RAM8(0x0172u)=44u; RAM8(0x0189u)=8u; RAM8(0x018Au)=8u;
    bua_factory_irq_step117();
    S117(RAM8(0x0048u)==3u && RAM8(0x0032u)==0u &&
         bua_factory_trace117.mode_changes==1ul,
         "changed FMD low bits update factory mode while leaving ignition timer clear");
    S117(RAM8(0x0189u)==0u && RAM8(0x0172u)==0u && RAM8(0x018Au)==0xFFu,
         "mode change resets serial count/minor count and primes table index to $FF");

    sim_factory_swi_reason117=0u; sim_factory_diagnostic_adc117=39u;
    i=(unsigned int)bua_factory_trace117.fmd_exchanges;
    bua_factory_irq_step117();
    S117(sim_factory_swi_reason117==FACTORY117_SWI_DIAGNOSTIC &&
         bua_factory_trace117.fmd_exchanges==(bua_u32)i,
         "diagnostic 39 reaches SWI before any FMD exchange");
    sim_factory_swi_reason117=0u; sim_factory_diagnostic_adc117=100u;
    bua_factory_irq_step117();
    S117(sim_factory_swi_reason117==FACTORY117_SWI_DIAGNOSTIC,
         "diagnostic equality 100 is outside FDA7's accepted interval");
    sim_factory_swi_reason117=0u; sim_factory_diagnostic_adc117=40u;
    bua_factory_irq_step117();
    S117(sim_factory_swi_reason117==FACTORY117_SWI_NONE,
         "diagnostic equality 40 is accepted by FDA7");

    S117(bua_factory_trace117.serial_160_cadence!=0ul &&
         bua_factory_trace117.serial_8192_cadence!=0ul,
         "focused run observes both factory serial-cadence branches");

    printf("  step-117 factory-test control regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef S117
}
