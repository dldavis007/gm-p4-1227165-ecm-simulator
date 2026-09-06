/*
 * Step 118: remainder of the factory-test loop, listing $FD29..$FEA3.
 * Raw mode-bit behavior is preserved without assigning unproven semantics.
 */

typedef struct BuaFactoryTrace118Tag {
    bua_u32 calls;
    bua_u32 raw_zero_paths;
    bua_u32 raw_even_nonzero_paths;
    bua_u32 raw_low_bit_paths;
    bua_u32 nv_checksums;
    bua_u32 checksum_watchdog_strobes;
    bua_u32 all_off_paths;
    bua_u32 adc_sweeps;
    bua_u32 coolant_0187_captures;
    bua_u32 coolant_0188_captures;
    bua_u32 pwm_exercises;
    bua_u32 lamp_fan_exercises;
    bua_u32 iac_exercises;
    bua_u32 fuel_spark_exercises;
    bua_u32 wait_loop_handoffs;
    bua_u32 delay_boundaries;
    bua_u16 last_pwm_word;
} BuaFactoryTrace118;

static BuaFactoryTrace118 bua_factory_trace118;
static bua_u8 sim_factory_adc118[12];

static void bua_factory_reset_step118(void)
{
    bua_u8 i;
    memset(&bua_factory_trace118,0,sizeof(bua_factory_trace118));
    for(i=0u;i<12u;++i)
        sim_factory_adc118[i]=hw_adc((bua_u8)(i<<4));
}

/* LF32E over the only Step-118 range, X=$0000 and D=$002E. */
static bua_u16 bua_factory_nv_checksum_step118(void)
{
    bua_u16 sum;
    bua_u16 address;
    sum=0u;
    for(address=0u;address<0x002Eu;++address)
        sum=(bua_u16)(sum+(bua_u16)RAM8(address));
    /* LF32E reaches LF348 once for this 46-byte invocation. */
    mem.io4000[0x0Bu]=0xFFu;
    mem.io4000[0x0Cu]=0x00u;
    ++bua_factory_trace118.checksum_watchdog_strobes;
    return sum;
}

static void bua_factory_six_pwm_step118(bua_u16 value)
{
    static const bua_u16 addresses[6] = {
        0x3FD2u,0x3FD4u,0x3FD6u,0x3FD8u,0x3FDAu,0x3FCCu
    };
    bua_u8 i;
    for(i=0u;i<6u;++i)
        mpu16be_set(addresses[i],value);
    bua_factory_trace118.last_pwm_word=value;
}

static bua_u16 bua_factory_pwm_word_step118(bua_u8 counter)
{
    bua_u8 folded;
    folded=counter;
    if(folded>0x20u)
        folded=(bua_u8)(folded-0x30u);
    if(folded<0x10u)
        return 0xD200u;
    if(folded==0x10u)
        return 0xD066u;
    return 0xD39Au;
}

static void bua_factory_all_off_step118(void)
{
    bua_u16 csr;
    bua_u8 hi;
    bua_u8 lo;
    csr=mpu16be_get(0x3FFCu);
    hi=(bua_u8)(csr>>8);
    lo=(bua_u8)csr;
    hi=(bua_u8)(hi&0xFBu);
    lo=(bua_u8)((lo&0xEFu)|0x08u);
    ++bua_factory_trace118.delay_boundaries;
    mpu16be_set(0x3FFCu,(bua_u16)(((bua_u16)hi<<8)|lo));
    ++bua_factory_trace118.delay_boundaries;
    mpu16be_set(0x3FD0u,0u);
    mem.io4000[4]=(bua_u8)(mem.io4000[4]&0xF9u);
    ++bua_factory_trace118.delay_boundaries;
    bua_factory_six_pwm_step118(0x7000u);
    ++bua_factory_trace118.all_off_paths;
}

static void bua_factory_adc_step118(void)
{
    bua_u8 i;
    bua_u8 counter;
    bua_u8 coolant;
    for(i=0u;i<12u;++i)
        RAM8((bua_u16)(0x017Bu+i))=sim_factory_adc118[i];
    ++bua_factory_trace118.adc_sweeps;

    counter=RAM8(0x0172u);
    if((counter&0x03u)==0u) {
        coolant=RAM8(0x017Fu);
        if((RAM8(0x0031u)&0x01u)!=0u) {
            RAM8(0x0187u)=coolant;
            ++bua_factory_trace118.coolant_0187_captures;
        } else {
            RAM8(0x0188u)=coolant;
            ++bua_factory_trace118.coolant_0188_captures;
        }
        RAM8(0x0031u)=(bua_u8)(RAM8(0x0031u)^0x01u);
    }
}

static void bua_factory_100ms_outputs_step118(void)
{
    bua_u8 counter;
    bua_u8 phase;
    bua_u16 csr;
    bua_u8 hi;
    bua_u8 lo;
    counter=RAM8(0x0172u);
    bua_factory_six_pwm_step118(bua_factory_pwm_word_step118(counter));
    ++bua_factory_trace118.pwm_exercises;
    ++bua_factory_trace118.delay_boundaries;

    csr=mpu16be_get(0x3FFCu);
    hi=(bua_u8)(csr>>8);
    lo=(bua_u8)((bua_u8)csr|0x10u);
    if((counter&0x10u)!=0u) {
        lo=(bua_u8)(lo|0x08u);
        mem.io4000[4]=(bua_u8)(mem.io4000[4]|0x06u);
    } else {
        lo=(bua_u8)(lo&0xF7u);
        mem.io4000[4]=(bua_u8)(mem.io4000[4]&0xFDu);
    }
    mpu16be_set(0x3FFCu,(bua_u16)(((bua_u16)hi<<8)|lo));
    ++bua_factory_trace118.lamp_fan_exercises;

    phase=RAM8(0x0179u);
    phase=(bua_u8)((bua_u8)(phase<<1)|(bua_u8)(phase>>7));
    RAM8(0x0179u)=phase;
    mem.io4000[2]=(bua_u8)((mem.io4000[2]&0xFCu)|(phase&0x03u));
    ++bua_factory_trace118.iac_exercises;
}

static void bua_factory_fuel_spark_step118(void)
{
    bua_u16 reference;
    bua_u16 quarter;
    bua_u16 fuel;
    bua_u16 dwell;
    bua_u16 fire;
    bua_u16 delay;
    reference=mpu16be_get(0x3FC0u);
    quarter=(bua_u16)(reference>>2);
    ram16be_set(0x0177u,quarter);
    delay=0u;
    if(reference>655u) {
        fuel=655u;
        dwell=327u;
        fire=quarter;
    } else if(reference<328u) {
        fuel=66u;
        dwell=197u;
        fire=(bua_u16)(0u-quarter);
        delay=66u;
    } else {
        fuel=327u;
        dwell=262u;
        fire=0u;
    }
    mpu16be_set(0x3FD0u,fuel);
    mpu16be_set(0x3FDCu,dwell);
    ++bua_factory_trace118.delay_boundaries;
    mpu16be_set(0x3FF6u,fire);
    ++bua_factory_trace118.delay_boundaries;
    mpu16be_set(0x3FCEu,delay);
    ++bua_factory_trace118.fuel_spark_exercises;
}

static void bua_factory_execute_step118(void)
{
    bua_u8 mode;
    bua_u8 toggled;
    bua_u16 csr;
    bua_u8 hi;
    bua_u8 lo;
    ++bua_factory_trace118.calls;
    mode=RAM8(0x0048u);
    toggled=(bua_u8)(RAM8(0x0031u)^0x02u);
    csr=mpu16be_get(0x3FFCu);
    hi=(bua_u8)(csr>>8);
    lo=(bua_u8)csr;

    if(mode==0u) {
        lo=(bua_u8)(lo|0x04u);
        if((RAM8(0x0049u)&0x04u)==0u)
            lo=(bua_u8)(lo&0xFBu);
        if(RAM8(0x0032u)!=0u || (RAM8(0x0049u)&0x03u)!=0u)
            RAM8(0x0031u)=toggled;
        mpu16be_set(0x3FFCu,(bua_u16)(((bua_u16)hi<<8)|lo));
        ++bua_factory_trace118.delay_boundaries;
        ++bua_factory_trace118.raw_zero_paths;
        bua_factory_all_off_step118();
        ++bua_factory_trace118.wait_loop_handoffs;
        return;
    }

    RAM8(0x0031u)=toggled;
    mpu16be_set(0x3FFCu,csr);
    if((mode&0x01u)==0u) {
        mpu16be_set(0x3FF2u,0x00C5u);
        ++bua_factory_trace118.delay_boundaries;
        csr=mpu16be_get(0x3FFCu);
        csr=(bua_u16)(csr|0x0400u);
        ++bua_factory_trace118.delay_boundaries;
        mpu16be_set(0x3FFCu,csr);
        ram16be_set(0x0175u,bua_factory_nv_checksum_step118());
        ++bua_factory_trace118.nv_checksums;
        ++bua_factory_trace118.raw_even_nonzero_paths;
        bua_factory_all_off_step118();
        ++bua_factory_trace118.wait_loop_handoffs;
        return;
    }

    ++bua_factory_trace118.raw_low_bit_paths;
    bua_factory_adc_step118();
    if((RAM8(0x0172u)&0x0Fu)==0u)
        bua_factory_100ms_outputs_step118();
    bua_factory_fuel_spark_step118();
    ++bua_factory_trace118.wait_loop_handoffs;
}
