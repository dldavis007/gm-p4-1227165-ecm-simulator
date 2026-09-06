static void run_step116_mode4_lifecycle_test(void)
{
    unsigned int passed=0u;
    unsigned int total=16u;
    bua_u8 frame[SCI115_MAX_FRAME];
    bua_u8 data[13];
    bua_u8 i;
    bua_u32 entries;
#define S116(c,t) do { if(c) ++passed; printf("  %-78s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-116 Mode-4 lifecycle/scheduler regression:\n");

    ecm_reset();
    memset(&bua_mode4_trace116,0,sizeof(bua_mode4_trace116));
    for(i=0u;i<5u;++i) RAM8((bua_u16)(0x0005u+i))=0xA5u;
    for(i=0u;i<16u;++i) RAM8((bua_u16)(0x001Cu+i))=0x44u;
    RAM8(0x002Cu)=100u; RAM8(0x00F3u)=0u;
    data[0]=4u;
    data[1]=0u; data[2]=0u; data[3]=0x70u; data[4]=0u;
    data[5]=0x7Du; data[6]=0x55u; data[7]=0u; data[8]=0u;
    data[9]=0u; data[10]=0u; data[11]=0u; data[12]=0x20u;
    bua_sci115_build_request(frame,data,13u);
    S116(bua_sci115_receive(frame,16u)==SCI115_OK,
         "valid Mode-4 frame completes SCI receive and ICB transfer");
    S116((SERIAL_MODE_WORD&0x08u)!=0u && (MINOR_MODE_WORD2&0x08u)==0u,
         "SCI completion locks 8192 but does not activate Mode 4 early");

    bua_mode4_scheduler_step116();
    S116((MINOR_MODE_WORD2&0x08u)!=0u && bua_mode4_trace116.entries==1ul,
         "$CB67..$CB72 activates Mode 4 on the later scheduler pass");
    S116(RAM8(0x0005u)==0u && RAM8(0x0006u)==0u && RAM8(0x0007u)==0u &&
         RAM8(0x0008u)==0u && RAM8(0x0009u)==0u,
         "command-word-3 bit 6 clears all five retained error bytes");
    S116(ram16be_get(0x0018u)==1u && bua_mode4_trace116.error_resets==1ul,
         "error reset immediately stores the LF3A7 checksum");
    S116((RAM8(0x00F3u)&0x04u)!=0u && RAM8(0x002Cu)==255u &&
         bua_mode4_trace116.iac_resets==1ul,
         "command-word-3 bit 5 requests IAC reset and writes position 255");
    for(i=0u;i<16u && RAM8((bua_u16)(0x001Cu+i))==128u;++i) { }
    S116(i==16u && RAM8(0x000Au)==128u && RAM8(0x000Cu)==128u &&
         bua_mode4_trace116.blm_resets==1ul,
         "command-word-3 bit 4 executes listing-exact LF434 BLM/SAM reset");
    entries=bua_mode4_trace116.entries;
    bua_mode4_scheduler_step116();
    S116(bua_mode4_trace116.entries==entries &&
         bua_mode4_trace116.error_resets==1ul &&
         bua_mode4_trace116.iac_resets==1ul && bua_mode4_trace116.blm_resets==1ul,
         "prior Mode-4 flag prevents all one-time entry commands from repeating");

    ENGINE_MODE_WORD|=0x80u;
    seg1_output_bits();
    S116(mpu16be_get(0x3FD4u)==(bua_u16)(((0x3400u|0x55u)<<2)|3u) &&
         mpu16be_get(0x3FD6u)==(bua_u16)(((0x3400u|0x55u)<<2)|3u),
         "received command words reach existing enrichment and TCC output consumers");
    S116(mpu16be_get(0x3FD8u)==(bua_u16)(((0x3400u|0x55u)<<2)|3u) &&
         mpu16be_get(0x3FD2u)==(bua_u16)(((0x3400u|0x55u)<<2)|3u),
         "received command words reach existing purge and EGR output consumers");

    SPARK_MODE_WORD|=0x01u; RAM8(0x003Fu)|=0x01u; RAM8(0x00B4u)=9u;
    data[0]=0u;
    bua_sci115_build_request(frame,data,1u);
    S116(bua_sci115_receive(frame,4u)==SCI115_OK &&
         (MINOR_MODE_WORD2&0x08u)!=0u,
         "non-Mode-4 receive leaves prior mode active until scheduler dispatch");
    bua_mode4_scheduler_step116();
    S116((MINOR_MODE_WORD2&0x08u)==0u && (SPARK_MODE_WORD&0x01u)==0u &&
         (RAM8(0x003Fu)&0x01u)==0u && RAM8(0x00B4u)==0u,
         "$CBA4 exit clears Mode-4 and EST-bypass diagnostic state");
    S116(bua_mode4_trace116.exits==1ul,
         "Mode-4 exit is recorded exactly once");

    RAM8(0x0151u)=4u; RAM8(0x0171u)=29u;
    one_second_event();
    S116(RAM8(0x0171u)==30u && RAM8(0x0151u)==4u,
         "serial timeout equality at 30 seconds does not force Mode 0");
    one_second_event();
    S116(RAM8(0x0171u)==31u && RAM8(0x0151u)==0u &&
         bua_mode4_trace116.timeout_forces==1ul,
         "serial timeout greater than 30 seconds clears the received mode byte");

    MINOR_MODE_WORD2|=0x08u;
    bua_mode4_scheduler_step116();
    S116((MINOR_MODE_WORD2&0x08u)==0u && bua_mode4_trace116.exits==2ul,
         "forced Mode 0 takes effect on the following scheduler pass");

    printf("  step-116 Mode-4 lifecycle regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef S116
}
