static void run_step119_vector_boundaries_test(void)
{
    unsigned int passed=0u;
    unsigned int total=19u;
    unsigned int i;
    bua_u16 target;
    bua_u32 ordinary_irqs;
#define S119(c,t) do { if(c) ++passed; printf("  %-78s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-119 vector/exception-boundary regression:\n");

    S119((sizeof(bua_vector_table119)/sizeof(bua_vector_table119[0]))==8u &&
         bua_vector_table119[0]==0x6000u &&
         bua_vector_table119[1]==0xC9F4u &&
         bua_vector_table119[2]==0xF27Bu &&
         bua_vector_table119[3]==0x6000u &&
         bua_vector_table119[4]==0xC800u &&
         bua_vector_table119[5]==0xC800u &&
         bua_vector_table119[6]==0xC800u &&
         bua_vector_table119[7]==0xC800u,
         "$FFF0..$FFFE preserves all eight emitted vector words exactly");

    ecm_reset(); RAM8(0x0020u)=0x5Au;
    target=bua_vector_dispatch_step119(0xFFF0u);
    S119(target==0x6000u &&
         bua_vector_trace119.last_outcome==VECTOR119_OUTCOME_EXTERNAL &&
         RAM8(0x0020u)==0x5Au,
         "$FFF0 target $6000 remains external and causes no invented RAM effect");
    target=bua_vector_dispatch_step119(0xFFF6u);
    S119(target==0x6000u &&
         bua_vector_trace119.external_6000_boundaries==2ul,
         "$FFF6 independently records the second emitted $6000 boundary");

    ecm_reset(); ordinary_irqs=stats.irq_ticks;
    target=bua_vector_dispatch_step119(0xFFF2u);
    S119(target==0xC9F4u && stats.irq_ticks==ordinary_irqs+1ul &&
         bua_vector_trace119.irq_entries==1ul,
         "$FFF2->$C9F4 dispatches the represented ordinary IRQ front");

    ecm_reset(); bua_factory_boot_step117(0u,1u);
    RAM8(0x0048u)=1u; sim_factory_fmd_byte1_117=1u;
    sim_factory_battery_adc117=90u; sim_factory_diagnostic_adc117=40u;
    ordinary_irqs=stats.irq_ticks;
    target=bua_vector_dispatch_step119(0xFFF2u);
    S119(target==0xC9F4u && stats.irq_ticks==ordinary_irqs &&
         bua_factory_trace117.irq_routes==1ul &&
         bua_vector_trace119.irq_entries==1ul,
         "the same $C9F4 entry reaches factory routing when L0047 bit 7 is set");

    ecm_reset(); RAM8(0x0021u)=0xA5u; ordinary_irqs=stats.irq_ticks;
    target=bua_vector_dispatch_step119(0xFFF4u);
    S119(target==0xF27Bu &&
         bua_vector_trace119.last_outcome==VECTOR119_OUTCOME_RTI &&
         bua_vector_trace119.immediate_rti_entries==1ul,
         "$FFF4 resolves to the listing-proven immediate RTI at $F27B");
    S119(RAM8(0x0021u)==0xA5u && stats.irq_ticks==ordinary_irqs,
         "immediate-RTI model adds no fabricated state changes");

    ecm_reset(); RAM8(0x0022u)=0x3Cu;
    for(i=0u;i<4u;++i) {
        target=bua_vector_dispatch_step119((bua_u16)(0xFFF8u+(bua_u16)(i*2u)));
        if(target!=0xC800u) break;
    }
    S119(i==4u,
         "$FFF8/$FFFA/$FFFC/$FFFE all resolve to the emitted $C800 target");
    S119(bua_vector_trace119.reset_requests==4ul &&
         bua_vector_trace119.reset_requested!=0u &&
         bua_vector_trace119.last_outcome==VECTOR119_OUTCOME_RESET,
         "all four $C800 slots produce reset/startup requests");
    S119(RAM8(0x0022u)==0x3Cu,
         "a reset request does not pretend to execute undocumented CPU recovery");

    ecm_reset();
    target=bua_vector_dispatch_step119(0xFFF1u);
    S119(target==0u &&
         bua_vector_trace119.invalid_slots==1ul,
         "odd vector-table address is rejected without a target");
    target=bua_vector_dispatch_step119(0xFFEEu);
    S119(target==0u &&
         bua_vector_trace119.invalid_slots==2ul,
         "address outside the emitted vector table is rejected");

    ecm_reset(); bua_hal_request_soft_powerdown_step111();
    S119(bua_vector_trace119.unresolved_swi_boundaries==1ul &&
         bua_vector_trace119.last_swi_source==0xD6EAu,
         "ignition powerdown records its exact $D6EA SWI source address");
    bua_hal_request_soft_powerdown_step111();
    S119(bua_vector_trace119.unresolved_swi_boundaries==1ul,
         "latched powerdown prevents duplicate SWI-boundary recording");

    ecm_reset(); bua_factory_boot_step117(0u,1u);
    sim_factory_diagnostic_adc117=39u; irq_6p25ms();
    S119(bua_vector_trace119.unresolved_swi_boundaries==1ul &&
         bua_vector_trace119.last_swi_source==0xFDB3u,
         "factory diagnostic rejection records the exact $FDB3 SWI source");

    ecm_reset(); bua_factory_boot_step117(0u,1u);
    sim_factory_diagnostic_adc117=40u; sim_factory_battery_adc117=40u;
    RAM8(0x0032u)=160u; irq_6p25ms();
    S119(bua_vector_trace119.unresolved_swi_boundaries==1ul &&
         bua_vector_trace119.last_swi_source==0xFD03u,
         "factory low-power timeout records the exact $FD03 SWI source");

    ecm_reset(); ENGINE_MODE_WORD=(bua_u8)(ENGINE_MODE_WORD|ENGINE_RUNNING_BIT);
    NO_REF_MINOR_COUNT=CAL_NO_REF_WAIT_LOOPS;
    bua_reference_state_12p5ms();
    S119(bua_vector_trace119.unresolved_swi_boundaries==1ul &&
         bua_vector_trace119.last_swi_source==0xCDCDu,
         "missing-reference wait records the exact $CDCD SWI source");
    S119(bua_vector_trace119.last_target==0u &&
         bua_vector_trace119.last_vector_address==0u,
         "SWI source recording does not guess a vector slot from comments");

    ecm_reset();
    for(i=0u;i<8u;++i)
        (void)bua_vector_dispatch_step119((bua_u16)(0xFFF0u+(bua_u16)(i*2u)));
    S119(bua_vector_trace119.dispatches==8ul &&
         bua_vector_trace119.external_6000_boundaries==2ul &&
         bua_vector_trace119.irq_entries==1ul &&
         bua_vector_trace119.immediate_rti_entries==1ul &&
         bua_vector_trace119.reset_requests==4ul,
         "full vector sweep accounts for two external, IRQ, RTI and four reset slots");

    printf("  step-119 vector-boundary regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef S119
}
