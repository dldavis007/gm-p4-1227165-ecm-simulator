static void run_step108_diagnostic_qualification_test(void)
{
    unsigned int pass=0u,total=0u;
#define S108(C) do { ++total; if(C) ++pass; else printf("  step108 FAIL line %d\n",__LINE__); } while(0)
    printf("Step-108 LE4F7 diagnostic-qualification regression:\n");

    ecm_reset();
    RAM8(0x003Fu)=0x10u; RAM8(0x0041u)=0x40u;
    RAM8(0x006Fu)=100u; RAM8(0x00AAu)=31u;
    bua_diag_qualification_step108();
    S108((RAM8(0x004Cu)&0x40u)!=0u && (RAM8(0x0041u)&0x10u)!=0u);
    RAM8(0x006Fu)=125u; RAM8(0x00AAu)=9u;
    bua_diag_qualification_step108();
    S108(RAM8(0x00AAu)==0u);

    ecm_reset();
    RAM8(0x0081u)=200u; RAM8(0x00A7u)=21u;
    bua_diag_qualification_step108();
    S108((RAM8(0x004Cu)&0x04u)!=0u && (RAM8(0x003Fu)&0x04u)!=0u);
    RAM8(0x0081u)=128u; RAM8(0x00A7u)=7u;
    bua_diag_qualification_step108();
    S108(RAM8(0x00A7u)==0u && (RAM8(0x003Fu)&0x04u)==0u);

    ecm_reset();
    RAM8(0x003Fu)=0x08u; RAM8(0x0065u)=0u; RAM8(0x0063u)=0u;
    RAM8(0x0082u)=0u; RAM8(0x0037u)=0u; RAM8(0x0056u)=100u;
    bua_diag_qualification_step108();
    S108(RAM8(0x00ABu)==1u);
    RAM8(0x00ABu)=3u;
    bua_diag_qualification_step108();
    S108((RAM8(0x004Du)&0x80u)!=0u && (RAM8(0x0044u)&0x10u)!=0u);

    ecm_reset();
    RAM8(0x002Eu)=0x20u; RAM8(0x0063u)=100u; RAM8(0x005Bu)=180u;
    RAM8(0x0112u)=200u; RAM8(0x0082u)=20u;
    bua_diag_qualification_step108();
    S108(ram16be_get(0x00AEu)==1u);
    ram16be_set(0x00AEu,2561u);
    bua_diag_qualification_step108();
    S108((RAM8(0x004Du)&0x08u)!=0u && ram16be_get(0x00AEu)==2561u);
    ecm_reset();
    RAM8(0x002Eu)=0x20u; ram16be_set(0x00AEu,10u);
    RAM8(0x0063u)=128u; RAM8(0x005Bu)=180u;
    bua_diag_qualification_step108();
    S108(ram16be_get(0x00AEu)==8u);
    ecm_reset();
    RAM8(0x0041u)=0x20u; ram16be_set(0x00AEu,0x1234u);
    bua_diag_qualification_step108();
    S108((RAM8(0x004Du)&0x08u)!=0u && ram16be_get(0x00AEu)==0x1234u);

    ecm_reset();
    RAM8(0x007Fu)=101u; RAM8(0x00B6u)=46u; RAM8(0x00B0u)=9u;
    RAM8(0x0082u)=0u; RAM8(0x0056u)=100u;
    bua_diag_qualification_step108();
    S108((RAM8(0x004Du)&0x04u)!=0u && (RAM8(0x003Fu)&0x80u)!=0u);

    ecm_reset();
    ram16be_set(0x00EFu,50u); RAM8(0x00B5u)=1u;
    RAM8(0x0056u)=24u; RAM8(0x0082u)=16u; RAM8(0x0063u)=100u;
    bua_diag_qualification_step108();
    S108(RAM8(0x00B5u)==2u);
    bua_diag_qualification_step108();
    S108((RAM8(0x004Du)&0x02u)!=0u && (RAM8(0x003Fu)&0x40u)!=0u);

    ecm_reset();
    RAM8(0x0040u)=1u;
    bua_diag_qualification_step108();
    S108((RAM8(0x004Eu)&0x40u)!=0u && (RAM8(0x0040u)&1u)==0u);

    ecm_reset();
    RAM8(0x0034u)=0x40u; RAM8(0x0056u)=5u; RAM8(0x00B4u)=0u;
    bua_diag_qualification_step108();
    S108((RAM8(0x003Fu)&1u)!=0u && (RAM8(0x0001u)&0x80u)==0u);
    bua_diag_qualification_step108();
    S108((RAM8(0x0001u)&0x80u)!=0u);

    ecm_reset();
    RAM8(0x003Bu)=0x20u;
    bua_diag_qualification_step108();
    S108((RAM8(0x004Eu)&0x10u)!=0u);

    ecm_reset();
    RAM8(0x0073u)=44u; RAM8(0x00A8u)=21u;
    bua_diag_qualification_step108();
    S108((RAM8(0x004Eu)&0x08u)!=0u && (RAM8(0x0041u)&0x02u)!=0u);

    ecm_reset();
    RAM8(0x0073u)=159u; RAM8(0x00A9u)=51u;
    bua_diag_qualification_step108();
    S108((RAM8(0x004Eu)&0x04u)!=0u && (RAM8(0x0041u)&0x02u)!=0u);

    ecm_reset();
    RAM8(0x003Fu)=0x08u; RAM8(0x0044u)=0x80u;
    RAM8(0x0073u)=44u; RAM8(0x00A8u)=0u;
    bua_diag_qualification_step108();
    S108(RAM8(0x00A8u)==1u);

    S108(D108_ERR13_RUN_HALF_MIN==60u && D108_ERR32_LIMIT==2560u);
    S108(D108_ERR44_TIMER==20u && D108_ERR45_TIMER==50u);
    printf("  step-108 diagnostic-qualification regression result: %s (%u/%u)\n",
           pass==total?"PASS":"FAIL",pass,total);
#undef S108
}
