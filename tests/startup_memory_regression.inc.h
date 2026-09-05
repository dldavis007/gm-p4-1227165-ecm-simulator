static void run_step113_startup_memory_test(void)
{
    unsigned int passed=0u;
    unsigned int total=10u;
    unsigned int i;
#define STEP113_CHECK(c,t) do { if(c) ++passed; printf("  %-78s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-113 retained-memory/startup initialization regression:\n");

    memset(&mem.low[0],0,sizeof(mem.low));
    RAM8(0x0005u)=1u; RAM8(0x0006u)=2u; RAM8(0x0007u)=3u;
    RAM8(0x0008u)=4u; RAM8(0x0009u)=5u;
    STEP113_CHECK(bua_error_word_checksum_step113()==16u,
                  "LF3A7 starts at one and sums L0005 through L0009");
    RAM8(0x0005u)=255u; RAM8(0x0006u)=255u; RAM8(0x0007u)=255u;
    RAM8(0x0008u)=255u; RAM8(0x0009u)=255u;
    STEP113_CHECK(bua_error_word_checksum_step113()==1276u,
                  "LF3A7 retains the emitted 16-bit accumulation result");

    bua_blm_initialize_step113();
    STEP113_CHECK(RAM8(0x000Au)==128u && RAM8(0x000Bu)==0u,
                  "LF434 stores $8000 at L000A:L000B");
    STEP113_CHECK(RAM8(0x000Cu)==128u && RAM8(0x000Du)==0u,
                  "LF434 stores $8000 at L000C:L000D");
    for(i=0u;i<16u && RAM8((bua_u16)(0x001Cu+i))==128u;++i) { }
    STEP113_CHECK(i==16u,
                  "LF434 emitted $80 initializes all sixteen BLM cells to 128");

    memset(&mem.low[0],0xA5,sizeof(mem.low));
    bua_retained_recovery_step113(1u);
    STEP113_CHECK(RAM8(0x0001u)==0xA5u && RAM8(0x002Du)==0xA5u,
                  "valid retained checksum bypasses the recovery writes");

    memset(&mem.low[0],0xA5,sizeof(mem.low));
    RAM8(0x0000u)=0x5Au;
    bua_retained_recovery_step113(0u);
    STEP113_CHECK(RAM8(0x0000u)==0x5Au && RAM8(0x0001u)==0u,
                  "LC8F8 preserves L0000 while clearing L0001 upward");
    STEP113_CHECK(ram16be_get(0x0018u)==1u,
                  "cleared error bytes produce LF3A7 checksum one at L0018");
    for(i=0u;i<16u && RAM8((bua_u16)(0x001Cu+i))==128u;++i) { }
    STEP113_CHECK(i==16u && RAM8(0x002Cu)==144u,
                  "recovery initializes BLM cells then startup IAC position 144");
    STEP113_CHECK(RAM8(0x003Du)==0x40u,
                  "$C90A records the invalid-retained-RAM flag byte $40");

    printf("  step-113 retained-memory regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP113_CHECK
}
