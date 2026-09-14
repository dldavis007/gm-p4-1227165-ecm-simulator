static void run_step169_aldl160_factory_test(void)
{
 unsigned int p=0u,n=8u,i,m; bua_u8 v[16]; BuaMemory sm=mem; BuaStats ss=stats;
#define C169(c) do{if(c)++p;else printf("  step169 FAIL line %d\n",__LINE__);}while(0)
 printf("\nStep-169 LF880 factory 160-baud manager regression:\n");
 ecm_reset(); RAM8(0x0047u)=0x80u;
 for(i=0u;i<14u;++i) RAM8((bua_u16)(0x017Bu+i))=(bua_u8)(0x20u+i);
 RAM8(0x0049u)=0xA1u;RAM8(0x004Au)=0xB2u;
 for(m=1u;m<=3u;++m){
  RAM8(0x0048u)=(bua_u8)m;ALCL_TABLE_INDEX=0u;ALCL_BIT_COUNT=0u;
  for(i=0u;i<16u;++i){(void)bua_lf880_manager((bua_u32)i);v[i]=ALCL_XMIT_BYTE;ALCL_BIT_COUNT=0u;}
  if(m==1u) C169(v[0]==RAM8(0x017Bu)&&v[13]==RAM8(0x0188u)&&v[14]==0xA1u&&v[15]==0xB2u);
  if(m==2u) C169(v[0]==0x25u&&v[1]==0xE5u&&v[2]==0x09u&&v[3]==0x79u&&v[4]==0x02u&&v[5]==0xEEu);
  if(m==3u) C169(v[0]==mem.mpu[0]&&v[1]==mem.mpu[1]&&v[14]==mem.mpu[0x38u]&&v[15]==mem.mpu[0x39u]);
  C169(ALCL_TABLE_INDEX==16u);
 }
 (void)bua_lf880_manager(17ul);
 C169(ALCL_TABLE_INDEX==0u&&ALCL_XMIT_BYTE==0xFFu);
 RAM8(0x0048u)=0u;ALCL_TABLE_INDEX=7u;ALCL_XMIT_BYTE=0x5Au;ALCL_BIT_COUNT=0u;
 (void)bua_lf880_manager(18ul);
 C169(ALCL_TABLE_INDEX==7u&&ALCL_XMIT_BYTE==0x5Au);
 printf("  step-169 factory-160 manager regression result: %s (%u/%u)\n",p==n?"PASS":"FAIL",p,n);
 mem=sm;stats=ss;
#undef C169
}
