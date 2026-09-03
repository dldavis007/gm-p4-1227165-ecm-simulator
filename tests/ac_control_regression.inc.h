static void run_step91_loose_ends_test(void)
{
    BuaAcTail91 t;
    BuaOutputStage91 o;
    BuaMemory saved_mem=mem;
    BuaStats saved_stats=stats;
    unsigned int pass=0u,total=0u;
#define S91(C) do { ++total; if(C) ++pass; else printf("  step91 FAIL line %d\n",__LINE__); } while(0)
    printf("Step-91 A/C tail + Segment-1 output-stage regression:\n");

    t=bua_ac_tail91(0x80u,0x00u,0u,20u,7u,0u,20u,0u);
    S91(t.iac_control_f2==0xA0u && t.learn_timer107==0u);
    t=bua_ac_tail91(0x80u,0x80u,0u,20u,7u,0u,20u,0u);
    S91(t.iac_control_f2==0x80u && t.learn_timer107==7u);
    t=bua_ac_tail91(0x00u,0x00u,0u,20u,0u,0u,20u,0u);
    S91(t.iac_control_f2==0x40u);
    t=bua_ac_tail91(0x00u,0x01u,0u,30u,20u,10u,10u,0u);
    S91(t.learned_2d==15u && t.learn_accum105==5u && t.iac_control_f2==0x40u);
    t=bua_ac_tail91(0x00u,0x01u,0x80u,30u,20u,10u,10u,0u);
    S91(t.learned_2d==15u && t.learn_accum105==2u);
    t=bua_ac_tail91(0x00u,0x81u,0u,25u,20u,7u,10u,0u);
    S91(t.learn_pos108==25u && t.learn_timer107==0u && t.iac_control_f2==0x40u);

    o=bua_output_stage91(0x06u,0x20u,0x08u,0u,255u,255u,0u);
    S91(o.air_arc_count==0xDFFFu && o.enrich_count==0xDFFFu && o.tcc_count==0xDFFFu);
    S91(o.purge_count==0xD3FFu && o.egr_count==0xD003u);
    S91(o.fan_parallel_b1==1u && o.forced_off==0u);
    o=bua_output_stage91(0x06u,0x20u,0x08u,128u,128u,255u,1u);
    S91(o.air_arc_count==0xD000u && o.tcc_count==0xD000u && o.purge_count==0xD000u && o.forced_off==1u);

    /* Scheduler-facing A/C tail: compressor-on marker drives b6/load-controlled state. */
    memset(&mem,0,sizeof(mem)); memset(&stats,0,sizeof(stats));
    MINOR_COUNT=0x05u; RAM8(0x00F3u)=0x08u; RAM8(0x0037u)=0x00u;
    RAM8(0x005Bu)=100u; RAM8(0x0084u)=0u; RAM8(0x002Du)=20u;
    major_segment(0x05u);
    S91((RAM8(0x00F2u)&0x40u)!=0u);

    /* Segment-1 software stage consumes live RAM requests. */
    RAM8(0x007Eu)=120u; RAM8(0x003Eu)=0x06u; RAM8(0x0037u)=0x20u;
    RAM8(0x0034u)=0x08u; RAM8(0x0113u)=0u; RAM8(0x0112u)=255u; RAM8(0x00F4u)=255u;
    major_segment(0x01u);
    S91(step91_outputs.air_arc_count==0xDFFFu && step91_outputs.enrich_count==0xDFFFu);
    S91(step91_outputs.tcc_count==0xDFFFu && step91_outputs.fan_parallel_b1==1u);

    printf("  result: %s (%u/%u)\n",pass==total?"PASS":"FAIL",pass,total);
    mem=saved_mem; stats=saved_stats;
#undef S91
}

