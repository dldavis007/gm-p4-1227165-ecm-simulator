static void run_step168_aldl160_diagnostic_test(void)
{
 unsigned int p=0u,n=6u,i; bua_u8 v[25]; BuaMemory sm=mem; BuaStats ss=stats;
#define C168(c) do{if(c)++p;else printf("  step168 FAIL line %d\n",__LINE__);}while(0)
 printf("\nStep-168 LF880 diagnostic 160-baud manager regression:\n");
 ecm_reset(); MINOR_MODE_WORD2=0x30u;
 for(i=2u;i<24u;++i) RAM8(bua_160_diagnostic_addresses[i])=(bua_u8)(0x40u+i);
 MINOR_MODE_WORD2=0x30u; ALCL_TABLE_INDEX=0u; ALCL_BIT_COUNT=0u;
 for(i=0u;i<25u;++i){(void)bua_lf880_manager((bua_u32)i);v[i]=ALCL_XMIT_BYTE;ALCL_BIT_COUNT=0u;}
 C168(v[0]==0x30u&&v[1]==0x25u&&v[2]==0xE5u);
 C168(v[3]==RAM8(0x002Cu)&&v[10]==RAM8(0x006Fu));
 C168(v[11]==RAM8(0x0005u)&&v[14]==RAM8(0x0008u));
 C168(v[15]==RAM8(0x0044u)&&v[20]==RAM8(0x00F1u));
 C168(v[21]==RAM8(0x00EAu)&&v[24]==RAM8(0x00D6u)&&ALCL_TABLE_INDEX==25u);
 (void)bua_lf880_manager(26ul);
 C168(ALCL_TABLE_INDEX==0u&&ALCL_XMIT_BYTE==0xFFu);
 printf("  step-168 diagnostic-160 manager regression result: %s (%u/%u)\n",p==n?"PASS":"FAIL",p,n);
 mem=sm;stats=ss;
#undef C168
}
