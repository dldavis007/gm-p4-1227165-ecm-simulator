static bua_u32 step111_hash_byte(bua_u32 hash,bua_u8 value)
{
    hash^=(bua_u32)value;
    return (hash*16777619ul)&0xFFFFFFFFul;
}

static bua_u32 step111_lifecycle_signature(void)
{
    static const bua_u16 addresses[21]={
        0x0001u,0x0002u,0x0003u,0x000Au,0x000Cu,
        0x001Cu,0x001Du,0x002Cu,0x0034u,0x003Eu,
        0x003Fu,0x0083u,0x0086u,0x0087u,0x008Bu,0x008Cu,
        0x0093u,0x00F2u,0x00F3u,0x0101u,0x018Du
    };
    bua_u32 hash;
    unsigned int i;

    hash=2166136261ul;
    for(i=0u;i<21u;++i)
        hash=step111_hash_byte(hash,RAM8(addresses[i]));
    hash=step111_hash_byte(hash,(bua_u8)stats.ignition_shutdown_calls);
    hash=step111_hash_byte(hash,(bua_u8)stats.iac_shutdown_homing_calls);
    hash=step111_hash_byte(hash,(bua_u8)stats.soft_powerdown_events);
    hash=step111_hash_byte(hash,sim_soft_powerdown_latched);
    return hash;
}

static void run_step111_ignition_shutdown_test(void)
{
    unsigned int pass=0u,total=0u;
    unsigned int i;
    bua_u32 before_irq;
    bua_u32 signature;
#define S111(C) do { ++total; if(C) ++pass; else printf("  step111 FAIL line %d\n",__LINE__); } while(0)
    printf("Step-111 listing-exact ignition shutdown/IAC homing regression:\n");

    ecm_reset();
    ram16be_set(0x008Bu,123u); RAM8(0x0033u)=0u;
    RAM8(0x0034u)=0x80u; RAM8(0x0081u)=12u; RAM8(0x003Fu)=0x20u;
    S111(bua_ignition_shutdown_odd_step111()==0u &&
         ram16be_get(0x008Bu)==0u && stats.ignition_shutdown_calls==1ul &&
         (RAM8(0x003Fu)&0x20u)==0u);

    ecm_reset();
    RAM8(0x0033u)=0x10u; RAM8(0x0001u)=0xA5u;
    ram16be_set(0x008Bu,0u);
    bua_ignition_shutdown_odd_step111();
    S111(ram16be_get(0x008Bu)==1u && RAM8(0x0001u)==0xA5u);
    ram16be_set(0x008Bu,7u);
    bua_ignition_shutdown_odd_step111();
    S111(ram16be_get(0x008Bu)==8u && RAM8(0x0001u)==0xA5u);

    ecm_reset();
    RAM8(0x0033u)=0x30u; RAM8(0x0034u)=0xA5u;
    RAM8(0x0001u)=0xFFu; RAM8(0x0002u)=0x5Fu; RAM8(0x0003u)=0xFFu;
    RAM8(0x000Au)=200u; RAM8(0x000Bu)=0x55u;
    RAM8(0x000Cu)=100u; RAM8(0x000Du)=0x66u;
    RAM8(0x003Eu)=0u; RAM8(0x0065u)=0u; RAM8(0x018Du)=9u;
    RAM8(0x0093u)=8u; RAM8(0x003Fu)=1u; RAM8(0x0039u)=0x80u;
    ram16be_set(0x001Au,0x1234u); ram16be_set(0x008Bu,8u);
    bua_ignition_shutdown_odd_step111();
    S111(ram16be_get(0x008Bu)==9u && RAM8(0x0001u)==0u &&
         RAM8(0x0002u)==0x10u && RAM8(0x0003u)==0x40u);
    S111(RAM8(0x000Au)==150u && RAM8(0x000Bu)==0x55u &&
         RAM8(0x000Cu)==118u && RAM8(0x000Du)==0x66u);
    S111(RAM8(0x001Cu)==150u);
    for(i=1u;i<16u;++i)
        S111(RAM8((bua_u16)(0x001Cu+i))==118u);
    S111(ram16be_get(0x001Au)==0u && (RAM8(0x0034u)&0x80u)==0u &&
         (RAM8(0x003Eu)&0x40u)!=0u);
    S111(RAM8(0x018Du)==0u && RAM8(0x0093u)==0u &&
         (RAM8(0x003Fu)&1u)==0u && (RAM8(0x0039u)&0x80u)==0u);

    ecm_reset();
    RAM8(0x0034u)=0x80u; RAM8(0x0081u)=11u;
    bua_ignition_shutdown_odd_step111();
    S111((RAM8(0x004Cu)&0x02u)!=0u && (RAM8(0x003Fu)&0x20u)!=0u);
    ecm_reset();
    RAM8(0x0086u)=1u; RAM8(0x0087u)=2u;
    bua_ignition_shutdown_odd_step111();
    S111(RAM8(0x0086u)==35u && RAM8(0x0087u)==0u);
    RAM8(0x0082u)=255u; RAM8(0x0083u)=0u; RAM8(0x0106u)=0u;
    bua_ignition_shutdown_odd_step111();
    S111((RAM8(0x0034u)&0x20u)!=0u && RAM8(0x0083u)==255u);

    ecm_reset();
    RAM8(0x0033u)=0x10u; ram16be_set(0x008Bu,0x0385u);
    S111(bua_ignition_shutdown_odd_step111()!=0u &&
         sim_soft_powerdown_latched!=0u && stats.soft_powerdown_events==1ul);
    before_irq=stats.irq_ticks;
    irq_6p25ms();
    S111(stats.irq_ticks==before_irq && stats.soft_powerdown_events==1ul);

    ecm_reset();
    RAM8(0x0033u)=0x10u; RAM8(0x002Cu)=50u;
    bua_iac_shutdown_homing_even_step111();
    S111((RAM8(0x00F3u)&0x04u)!=0u && RAM8(0x002Cu)==255u &&
         RAM8(0x0101u)==255u);
    RAM8(0x002Cu)=10u;
    bua_iac_shutdown_homing_even_step111();
    S111(RAM8(0x0101u)==255u);
    RAM8(0x002Cu)=0u;
    bua_iac_shutdown_homing_even_step111();
    S111((RAM8(0x00F3u)&0x02u)!=0u && RAM8(0x0101u)==144u);
    RAM8(0x002Cu)=100u;
    bua_iac_shutdown_homing_even_step111();
    S111(RAM8(0x0101u)==44u);
    RAM8(0x002Cu)=145u;
    bua_iac_shutdown_homing_even_step111();
    S111(RAM8(0x0101u)==127u);
    RAM8(0x002Cu)=144u;
    bua_iac_shutdown_homing_even_step111();
    S111(RAM8(0x0101u)==0u && (RAM8(0x0002u)&0x10u)!=0u);

    ecm_reset();
    RAM8(0x0033u)=0u; RAM8(0x003Du)=0x40u; RAM8(0x0037u)=1u;
    RAM8(0x00F3u)=0x06u; RAM8(0x002Cu)=144u;
    bua_iac_shutdown_homing_even_step111();
    S111(RAM8(0x002Du)==32u && RAM8(0x0109u)==255u &&
         RAM8(0x0102u)==255u && RAM8(0x00F2u)==0x80u &&
         RAM8(0x00F3u)==0x90u);

    ecm_reset();
    MINOR_COUNT=0u; RAM8(0x0033u)=0x10u; ram16be_set(0x008Bu,8u);
    irq_6p25ms();
    S111(stats.ignition_shutdown_calls==1ul && stats.air_fuel_loops==1ul &&
         ram16be_get(0x008Bu)==9u && stats.major_segment_calls[1]==1ul);
    irq_6p25ms();
    S111(stats.iac_shutdown_homing_calls==1ul && stats.spark_vss_loops==1ul &&
         stats.iac_minor_services==2ul && stats.major_segment_calls[2]==1ul);

    ecm_reset();
    MINOR_COUNT=0u; RAM8(0x0033u)=0x10u; ram16be_set(0x008Bu,0x0385u);
    irq_6p25ms();
    S111(sim_soft_powerdown_latched!=0u && stats.odd_fuel_chain_calls==0ul &&
         stats.major_segment_calls[1]==0ul);
    before_irq=stats.irq_ticks;
    irq_6p25ms();
    S111(stats.irq_ticks==before_irq);

    /* Deterministic lifecycle endpoint for the Step-111 frozen signature. */
    ecm_reset();
    RAM8(0x0033u)=0x30u; RAM8(0x0034u)=0x80u;
    RAM8(0x000Au)=160u; RAM8(0x000Cu)=110u;
    RAM8(0x003Eu)=0u; RAM8(0x0065u)=0u;
    RAM8(0x002Cu)=37u; ram16be_set(0x008Bu,8u); MINOR_COUNT=0u;
    irq_6p25ms();
    irq_6p25ms();
    ram16be_set(0x008Bu,0x0385u);
    irq_6p25ms();
    signature=step111_lifecycle_signature();
    printf("  Step-111 ignition lifecycle signature: %08lX\n",
           (unsigned long)signature);
    S111(signature==0x16D17C9Cul);

    printf("  step-111 ignition shutdown regression result: %s (%u/%u)\n",
           pass==total?"PASS":"FAIL",pass,total);
#undef S111
}
