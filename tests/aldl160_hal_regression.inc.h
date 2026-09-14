static void run_step170_aldl160_hal_test(void)
{
 unsigned int p=0u,n=9u,i; bua_u8 decoded; BuaSerialCellTrace tr;
 BuaMemory sm=mem;BuaStats ss=stats;bua_u8 sl=sim_serial_line_high;
#define C170(c) do{if(c)++p;else printf("  step170 FAIL line %d\n",__LINE__);}while(0)
 printf("\nStep-170 complete 160-baud raw-HAL regression:\n");
 ecm_reset();MINOR_MODE_WORD2=0u;RAM8(0x0047u)=0u;
 bua_hal_aldl160_init();
 C170(bua_hal_aldl160_line_high()!=0u&&bua_hal_aldl160_table_index()==0u&&
      bua_hal_aldl160_bit_count()==0u&&bua_hal_aldl160_xmit_byte()==0xFFu);
 bua_hal_aldl160_cell(&tr,0ul);
 C170(tr.line_at_first!=0u&&bua_hal_aldl160_table_index()==1u&&
      bua_hal_aldl160_bit_count()==8u);
 decoded=0u;
 for(i=0u;i<8u;++i){bua_hal_aldl160_cell(&tr,(bua_u32)((i+1u)*SERIAL_T_CELL));decoded=(bua_u8)((decoded<<1)|tr.shifted_data_bit);}
 C170(decoded==0u&&bua_hal_aldl160_bit_count()==0u);
 C170(bua_hal_aldl160_line_high()!=0u&&(SERIAL_MODE_WORD&3u)==0u);
 MINOR_MODE_WORD2=0x10u;RAM8(0x0047u)=0u;bua_hal_aldl160_init();
 bua_hal_aldl160_cell(&tr,0ul);
 C170(bua_hal_aldl160_xmit_byte()==0x10u&&bua_hal_aldl160_table_index()==1u);
 RAM8(0x0047u)=0x80u;RAM8(0x0048u)=1u;RAM8(0x017Bu)=0xA5u;RAM8(0x017Cu)=0x5Au;
 bua_hal_aldl160_init();bua_hal_aldl160_cell(&tr,0ul);
 C170(bua_hal_aldl160_xmit_byte()==0xA5u&&RAM8(0x017Au)==0x5Au);
 for(i=0u;i<8u;++i)bua_hal_aldl160_cell(&tr,(bua_u32)((i+1u)*SERIAL_T_CELL));
 bua_hal_aldl160_cell(&tr,9ul*SERIAL_T_CELL);
 C170(bua_hal_aldl160_xmit_byte()==0x5Au&&bua_hal_aldl160_table_index()==2u);
 C170(tr.line_at_first!=0u&&tr.shifted_data_valid==0u);
 C170(bua_hal_aldl160_line_high()!=0u&&(SERIAL_MODE_WORD&3u)==0u);
 printf("  step-170 complete-160 raw-HAL regression result: %s (%u/%u)\n",p==n?"PASS":"FAIL",p,n);
 mem=sm;stats=ss;sim_serial_line_high=sl;
#undef C170
}
