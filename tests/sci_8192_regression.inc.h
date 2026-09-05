static bua_u8 bua_sci115_sum(const bua_u8 *p,bua_u8 count)
{
    bua_u8 i;
    bua_u8 sum=0u;
    for(i=0u;i<count;++i)
        sum=(bua_u8)(sum+p[i]);
    return sum;
}

static void run_step115_sci_test(void)
{
    unsigned int passed=0u;
    unsigned int total=20u;
    bua_u8 frame[SCI115_MAX_FRAME];
    bua_u8 data[27];
    bua_u8 i;
#define S115(c,t) do { if(c) ++passed; printf("  %-78s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-115 8192-baud SCI/message regression:\n");

    ecm_reset();
    mpu16be_set(0x3FFCu,0x1200u);
    bua_sci115_init();
    S115(mem.io4000[7]==0x27u && (SERIAL_MODE_WORD&0x04u)!=0u,
         "LFA7A selects receive/wakeup control and records 8192 search mode");
    S115(mpu16be_get(0x3FFCu)==0x1204u && RAM8(0x0133u)==0u &&
         RAM8(0x0150u)==0u && RAM8(0x016Fu)==0u,
         "LFA58/LFA7A clear the sole device ICB/OCBs and set MPU CSR bit 2");

    data[0]=0u;
    bua_sci115_build_request(frame,data,1u);
    S115(bua_sci115_receive(frame,4u)==SCI115_OK,
         "valid device-$80 Mode-0 request passes emitted checksum framing");
    S115(bua_sci115.output_count==4u && bua_sci115.output[0]==0x80u &&
         bua_sci115.output[1]==0x56u && bua_sci115.output[2]==0u,
         "Mode 0 response is $80,$56,$00 followed by checksum");
    S115(bua_sci115_sum(bua_sci115.output,bua_sci115.output_count)==0u,
         "transmit path emits the two's-complement checksum");
    S115((SERIAL_MODE_WORD&0x0Cu)==0u && (MINOR_MODE_WORD2&0x08u)==0u,
         "Mode 0 returns both SCI lock and Mode-4 state to normal");

    bua_sci115_build_request(frame,data,1u); frame[0]=0x81u;
    S115(bua_sci115_receive(frame,4u)==SCI115_ERR_DEVICE,
         "unknown device code is rejected before table selection");
    bua_sci115_build_request(frame,data,1u); frame[3]++;
    S115(bua_sci115_receive(frame,4u)==SCI115_ERR_CHECKSUM,
         "nonzero accumulated receive checksum selects checksum error");
    bua_sci115_build_request(frame,data,1u);
    S115(bua_sci115_receive(frame,5u)==SCI115_ERR_LENGTH,
         "physical frame count must equal biased message length plus three");

    data[0]=1u;
    bua_sci115_build_request(frame,data,1u);
    S115(bua_sci115_receive(frame,4u)==SCI115_OK &&
         bua_sci115.output_count==67u && bua_sci115.output[1]==0x95u,
         "Mode 1 emits mode plus 63 table-selected bytes");
    S115(bua_sci115.output[3]==0x25u && bua_sci115.output[4]==0xE5u,
         "Mode-1 table begins with listing EPROM ID $25E5");
    S115(bua_sci115.output[5]==RAM8(0x0005u) &&
         bua_sci115_sum(bua_sci115.output,bua_sci115.output_count)==0u,
         "Mode-1 RAM selection and final checksum follow the C75B table");

    for(i=0u;i<63u;++i)
        RAM8((bua_u16)(0x0020u+i))=(bua_u8)(i+1u);
    data[0]=2u; data[1]=0u; data[2]=0x20u;
    bua_sci115_build_request(frame,data,3u);
    S115(bua_sci115_receive(frame,6u)==SCI115_OK &&
         bua_sci115.output[3]==1u && bua_sci115.output[65]==63u,
         "Mode 2 emits 63 sequential bytes from the requested address");
    S115(bua_sci115.output[1]==0x95u &&
         bua_sci115_sum(bua_sci115.output,bua_sci115.output_count)==0u,
         "Mode-2 response length follows emitted NOUT=64 arithmetic");

    RAM8(0x0020u)=0xA1u; RAM8(0x0040u)=0xB2u;
    data[0]=3u; data[1]=0u; data[2]=0x20u; data[3]=0u; data[4]=0x40u;
    bua_sci115_build_request(frame,data,5u);
    S115(bua_sci115_receive(frame,8u)==SCI115_OK &&
         bua_sci115.output[1]==0x58u && bua_sci115.output[3]==0xA1u &&
         bua_sci115.output[4]==0xB2u,
         "Mode 3 returns the contents of each requested address");

    data[0]=4u;
    for(i=0u;i<10u;++i) data[(bua_u8)(1u+i)]=(bua_u8)(0x10u+i);
    data[11]=0u; data[12]=0x20u;
    bua_sci115_build_request(frame,data,13u);
    S115(bua_sci115_receive(frame,16u)==SCI115_OK && RAM8(0x0152u)==0x10u &&
         RAM8(0x015Bu)==0x19u,
         "Mode 4 copies all ten command words into the listing ICB mirror");
    S115((SERIAL_MODE_WORD&0x08u)!=0u && (MINOR_MODE_WORD2&0x08u)!=0u,
         "accepted Mode 4 locks 8192 mode and raises the scheduler Mode-4 flag");
    S115(bua_sci115.output[1]==0x57u && bua_sci115.output[2]==4u &&
         bua_sci115.output[3]==RAM8(0x0020u),
         "Mode 4 derives its one-address response length from NIN minus 11");

    data[0]=3u; data[1]=0xC1u; data[2]=0u;
    bua_sci115_build_request(frame,data,3u);
    S115(bua_sci115_receive(frame,6u)==SCI115_ERR_ADDRESS &&
         bua_sci115.rom_read_boundary!=0u,
         "unrepresented ROM reads stop at a named boundary instead of inventing bytes");
    S115(mem.io4000[7]==0x89u && (mem.io4000[4]&0x08u)!=0u,
         "accepted receive path performs the emitted transmitter-enable register writes");

    printf("  step-115 SCI regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef S115
}
