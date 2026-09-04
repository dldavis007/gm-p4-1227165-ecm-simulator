static bua_u32 step110_output_hash_byte(bua_u32 hash,bua_u8 value)
{
    hash^=(bua_u32)value;
    return (hash*16777619ul)&0xFFFFFFFFul;
}

static bua_u32 step110_output_signature(void)
{
    static const bua_u16 addresses[5]={
        0x3FCCu,0x3FD2u,0x3FD4u,0x3FD6u,0x3FD8u
    };
    bua_u32 hash;
    bua_u16 value;
    unsigned int i;

    hash=2166136261ul;
    for(i=0u;i<5u;++i) {
        value=mpu16be_get(addresses[i]);
        hash=step110_output_hash_byte(hash,(bua_u8)(value>>8));
        hash=step110_output_hash_byte(hash,(bua_u8)value);
    }
    hash=step110_output_hash_byte(hash,mem.io4000[4]);
    hash=step110_output_hash_byte(hash,RAM8(0x0035u));
    hash=step110_output_hash_byte(hash,RAM8(0x003Eu));
    hash=step110_output_hash_byte(hash,RAM8(0x004Fu));
    return hash;
}

static void run_step110_output_stage_test(void)
{
    unsigned int pass=0u,total=0u;
    unsigned int i;
    bua_u16 forced_count;
    bua_u32 signature;
#define S110(C) do { ++total; if(C) ++pass; else printf("  step110 FAIL line %d\n",__LINE__); } while(0)
    printf("Step-110 listing-exact Segment-1 raw-output regression:\n");

    ecm_reset();
    RAM8(0x0034u)=0x88u; RAM8(0x003Eu)=0x06u;
    RAM8(0x0037u)=0x20u; RAM8(0x0113u)=0u;
    RAM8(0x0112u)=255u; RAM8(0x00F4u)=255u;
    mem.io4000[4]=0xA5u;
    seg1_output_bits();
    S110(mpu16be_get(0x3FCCu)==0xDFFFu &&
         mpu16be_get(0x3FD4u)==0xDFFFu);
    S110(mpu16be_get(0x3FD6u)==0xDFFFu);
    S110(mpu16be_get(0x3FD8u)==0xD3FFu &&
         mpu16be_get(0x3FD2u)==0xD003u);
    S110(mem.io4000[4]==0xA7u && step91_outputs.fan_parallel_b1==1u);

    ecm_reset();
    RAM8(0x0034u)=0x80u; RAM8(0x003Eu)=0u;
    RAM8(0x0113u)=255u; RAM8(0x0112u)=0u;
    mem.io4000[4]=0xFFu;
    seg1_output_bits();
    S110(mpu16be_get(0x3FCCu)==0xD000u &&
         mpu16be_get(0x3FD4u)==0xD000u &&
         mpu16be_get(0x3FD6u)==0xD000u);
    S110(mpu16be_get(0x3FD8u)==0xD003u &&
         mpu16be_get(0x3FD2u)==0xD3FFu);
    S110(mem.io4000[4]==0xFDu);

    ecm_reset();
    RAM8(0x0034u)=0x80u; RAM8(0x0035u)=0x08u;
    RAM8(0x0156u)=0x6Du; RAM8(0x0157u)=0x55u;
    RAM8(0x0152u)=0x02u; RAM8(0x0153u)=0x02u;
    mem.io4000[4]=0x80u;
    seg1_output_bits();
    forced_count=0xD157u;
    S110(mpu16be_get(0x3FCCu)==forced_count &&
         mpu16be_get(0x3FD4u)==forced_count);
    S110(mpu16be_get(0x3FD6u)==forced_count &&
         mpu16be_get(0x3FD8u)==forced_count &&
         mpu16be_get(0x3FD2u)==forced_count);
    S110(mem.io4000[4]==0x82u);

    ecm_reset();
    RAM8(0x0034u)=0u; RAM8(0x0041u)=0u;
    mem.io4000[4]=0xFFu;
    seg1_output_bits();
    S110(mpu16be_get(0x3FCCu)==0xD000u &&
         mpu16be_get(0x3FD2u)==0xD000u &&
         mpu16be_get(0x3FD8u)==0xD000u);
    S110(mem.io4000[4]==0xFDu && step91_outputs.forced_off==0u);

    RAM8(0x0041u)=0x80u;
    seg1_output_bits();
    S110(mpu16be_get(0x3FCCu)==0xDFFFu &&
         mpu16be_get(0x3FD2u)==0xDFFFu &&
         mpu16be_get(0x3FD8u)==0xDFFFu);
    S110((mem.io4000[4]&0x02u)!=0u);

    ecm_reset();
    RAM8(0x0034u)=0x80u; RAM8(0x007Eu)=171u;
    seg1_output_bits();
    S110((RAM8(0x0035u)&0x40u)!=0u &&
         (RAM8(0x003Eu)&0x10u)!=0u && step91_outputs.forced_off==0u);
    seg1_output_bits();
    S110((RAM8(0x004Fu)&0x40u)!=0u && step91_outputs.forced_off==1u);
    S110(mpu16be_get(0x3FCCu)==0xD000u &&
         mpu16be_get(0x3FD2u)==0xD000u &&
         mpu16be_get(0x3FD8u)==0xD000u);

    ecm_reset();
    sim_legacy_segment1_output_freeze=1u;
    RAM8(0x0034u)=0x88u; RAM8(0x003Eu)=0x06u;
    mpu16be_set(0x3FCCu,0x1234u);
    seg1_output_bits();
    S110(mpu16be_get(0x3FCCu)==0x1234u &&
         step91_outputs.air_arc_count==0xDFFFu);

    ecm_reset();
    MINOR_COUNT=0u; RAM8(0x0034u)=0x88u;
    sim_legacy_ignition_shutdown_freeze=1u;
    RAM8(0x003Eu)=0x06u; RAM8(0x0037u)=0x20u;
    RAM8(0x0113u)=0x44u; RAM8(0x0112u)=0x99u;
    RAM8(0x00F4u)=1u;
    for(i=0u;i<16u;++i)
        irq_6p25ms();
    S110(stats.major_segment_calls[1]==1ul);
    S110(mpu16be_get(0x3FCCu)==0xDFFFu &&
         mpu16be_get(0x3FD4u)==0xDFFFu);
    S110(mpu16be_get(0x3FD6u)==0xDFFFu);
    signature=step110_output_signature();
    printf("  Step-110 integrated output-stage signature: %08lX\n",
           (unsigned long)signature);
    S110(signature==0xFAADF8A6ul);

    printf("  step-110 Segment-1 output regression result: %s (%u/%u)\n",
           pass==total?"PASS":"FAIL",pass,total);
#undef S110
}
