/* ---------------------------------------------------------------------- */
/* Step 105: deterministic transmission-aware driving simulation.         */
/*                                                                        */
/* This is a PC plant model, not translated ECM code and not a claim that */
/* the selected shift speeds, axle/tire factor, or converter slip are the */
/* exact production values for a particular Corvette.  The four 700R4     */
/* internal ratios are explicit inputs.  The combined axle/tire factor is */
/* chosen so locked fourth gear produces N/V=32, centered in the source's */
/* executable LC288..LC289 fourth-gear recognition window of 30..35.      */
/*                                                                        */
/* Vehicle speed remains a deterministic test command.  In Drive, engine  */
/* RPM is never independently commanded: gear, speed, converter slip, and */
/* clutch state produce it.  The ordinary reference-pulse path then makes */
/* the ECM calculate RPM.                                                  */
/* ---------------------------------------------------------------------- */
#define STEP105_GEAR_COUNT              4u
#define STEP105_FOURTH_NV_TARGET       32.0
#define STEP105_FOURTH_RATIO            0.70
#define STEP105_RPM_PER_MPH_OUTPUT     (STEP105_FOURTH_NV_TARGET / STEP105_FOURTH_RATIO)
#define STEP105_EXPECTED_SIGNATURE      0x9732D09Bul

static const double step105_gear_ratio[5] = {
    0.0, 3.06, 1.62, 1.00, 0.70
};

/* PC-only schedule assumptions.  These are deliberately visible here. */
static const unsigned int step105_upshift_mph[5] = {0u,16u,30u,45u,999u};
static const unsigned int step105_downshift_mph[5] = {0u,0u,12u,25u,38u};

typedef struct Step105Plant {
    bua_u8 gear;
    bua_u8 in_drive;
    bua_u8 brake_power;
    bua_u8 physical_tcc;
    double engine_rpm;
    unsigned int upshifts;
    unsigned int downshifts;
    unsigned int gears_seen;
    unsigned int rpm_commands_in_drive;
    unsigned int fourth_classified;
    unsigned int tcc_command_seen;
    unsigned int tcc_command_in_fourth;
    unsigned int tcc_physical_seen;
    unsigned int brake_open_seen;
    unsigned int brake_released_tcc;
    unsigned int relock_seen;
    BuaGear68 classifier;
    BuaTcc67 tcc;
    BuaTccHardware70 hardware;
} Step105Plant;

typedef struct Step105ProfileSegment {
    unsigned int duration_ticks;
    unsigned int start_speed_mph;
    unsigned int end_speed_mph;
    unsigned int start_non_drive_rpm;
    unsigned int end_non_drive_rpm;
    unsigned int start_tps_adc;
    unsigned int end_tps_adc;
    unsigned int start_maf_adc;
    unsigned int end_maf_adc;
    bua_u8 in_drive;
    bua_u8 brake_power;
    const char *name;
} Step105ProfileSegment;

static const Step105ProfileSegment step105_profile[] = {
    {160u, 0u, 0u,350u,800u,38u, 35u, 50u, 60u,0u,1u,"cranking / catch"},
    {320u, 0u, 0u,800u,800u,35u, 35u, 55u, 55u,0u,1u,"warm idle"},
    {1280u,0u,60u,  0u,  0u,75u,110u, 55u,170u,1u,1u,"four-gear acceleration"},
    {640u,60u,60u,  0u,  0u,60u, 60u,110u,110u,1u,1u,"fourth-gear cruise"},
    {160u,60u,60u,  0u,  0u,60u, 60u,110u,110u,1u,0u,"brake opens TCC power"},
    {320u,60u,60u,  0u,  0u,60u, 60u,110u,110u,1u,1u,"brake release / relock"},
    {320u,60u,45u,  0u,  0u,35u, 35u,  0u,  0u,1u,1u,"throttle lift / DFCO"},
    {800u,45u, 0u,  0u,  0u,35u, 35u,  0u, 55u,1u,1u,"three downshifts"},
    {640u, 0u, 0u,800u,800u,35u, 35u, 55u, 55u,0u,1u,"returned idle"}
};
#define STEP105_PROFILE_COUNT \
    ((unsigned int)(sizeof(step105_profile)/sizeof(step105_profile[0])))

static void step105_plant_init(Step105Plant *p)
{
    memset(p,0,sizeof(*p));
    p->gear=1u;
    p->brake_power=1u;
    p->engine_rpm=800.0;
    p->classifier.fourth_timer=STEP68_FOURTH_CONFIRM;
    p->hardware.brake_power=1u;
    p->gears_seen=1u<<1;
}

static void step105_select_gear(Step105Plant *p,unsigned int mph)
{
    bua_u8 old_gear;
    old_gear=p->gear;
    if(p->in_drive==0u) {
        p->gear=1u;
        return;
    }
    if(p->gear<STEP105_GEAR_COUNT &&
       mph>=step105_upshift_mph[p->gear]) {
        ++p->gear;
        ++p->upshifts;
    } else if(p->gear>1u && mph<step105_downshift_mph[p->gear]) {
        --p->gear;
        ++p->downshifts;
    }
    if(p->gear!=old_gear)
        p->gears_seen|=(1u<<p->gear);
}

static double step105_unlocked_slip(unsigned int mph,unsigned int tps_adc)
{
    unsigned int opening;
    double slip;
    if(mph<3u)
        return 0.0;
    opening=(tps_adc>35u)?(tps_adc-35u):0u;
    slip=80.0+(double)opening*2.0;
    if(slip>250.0)
        slip=250.0;
    return slip;
}

static unsigned int step105_drive_rpm_target(const Step105Plant *p,
                                              unsigned int mph,
                                              unsigned int tps_adc)
{
    double target;
    target=(double)mph*STEP105_RPM_PER_MPH_OUTPUT*
           step105_gear_ratio[p->gear];
    if(p->physical_tcc==0u)
        target+=step105_unlocked_slip(mph,tps_adc);
    if(target<800.0)
        target=800.0;
    return (unsigned int)(target+0.5);
}

static unsigned int step105_filter_engine_rpm(Step105Plant *p,
                                               unsigned int target)
{
    /* PC driveline smoothing only: 0.125 response per 6.25-ms tick. */
    p->engine_rpm+=((double)target-p->engine_rpm)*0.125;
    if(p->engine_rpm<0.0)
        p->engine_rpm=0.0;
    return (unsigned int)(p->engine_rpm+0.5);
}

static bua_u8 step105_source_nv(void)
{
    bua_u32 numerator;
    bua_u16 vss_q8_8;
    bua_u32 nv;
    vss_q8_8=ram16be_get(0x0065u);
    if(vss_q8_8==0u)
        return 255u;
    /* High byte returned by LF215 FDIV for D~=RPM and X=VSS Q8.8. */
    numerator=(bua_u32)RPM_DIV25*25ul*256ul;
    nv=numerator/(bua_u32)vss_q8_8;
    if(nv>255ul)
        nv=255ul;
    return (bua_u8)nv;
}

static void step105_ecm_transmission_pass(Step105Plant *p)
{
    bua_u8 nv;
    bua_u8 was_physical;
    nv=step105_source_nv();
    RAM8(0x008Fu)=nv;
    bua_nv_gear_update68(&p->classifier,nv);
    if((p->classifier.mode04&STEP68_MODE_FOURTH)!=0u)
        ++p->fourth_classified;

    p->tcc.fourth_gear=(bua_u8)
        ((p->classifier.mode04&STEP68_MODE_FOURTH)!=0u);
    p->tcc.overdrive_active=(bua_u8)(p->gear==4u);
    bua_tcc_warm_auto67(&p->tcc,VSS_MPH_HI,TPS_LOAD_AXIS,COOLANT);
    if((p->tcc.status37&STEP67_TCC_STATUS_BIT)!=0u)
        ++p->tcc_command_seen;
    if((p->classifier.mode04&STEP68_MODE_FOURTH)!=0u &&
       (p->tcc.status37&STEP67_TCC_STATUS_BIT)!=0u)
        ++p->tcc_command_in_fourth;

    was_physical=p->physical_tcc;
    p->hardware.ecm_command=(bua_u8)(p->tcc.output_counter==STEP67_TCC_OUTPUT_ON);
    p->hardware.brake_power=p->brake_power;
    bua_tcc_hardware70(&p->hardware);
    p->physical_tcc=p->hardware.clutch_energized;
    if(p->physical_tcc!=0u)
        ++p->tcc_physical_seen;
    if(p->brake_power==0u) {
        ++p->brake_open_seen;
        if((p->tcc.status37&STEP67_TCC_STATUS_BIT)!=0u &&
           p->physical_tcc==0u)
            ++p->brake_released_tcc;
    }
    if(was_physical==0u && p->physical_tcc!=0u &&
       p->brake_open_seen!=0u)
        ++p->relock_seen;
}

static void run_step105_transmission_drive(void)
{
    Step105Plant plant;
    const Step105ProfileSegment *s;
    unsigned int si,tick,total_tick,speed,tps,maf,rpm,target;
    unsigned int passed,total,print_div;
    bua_u32 signature;
    bua_u8 dfco_seen;
#define STEP105_CHECK(C,T) do { ++total; if(C) ++passed; \
    printf("  %-84s %s\n",T,(C)?"PASS":"FAIL"); } while(0)

    printf("\nStep-105 transmission-aware driving simulation:\n");
    printf("  PC plant: speed command -> selected 700R4 ratio -> slip/TCC -> engine RPM.\n");
    printf("  ECM path: VSS/reference pulses -> measured RPM/VSS -> N/V gear and TCC logic.\n");
    printf("  Shift speeds, slip, and effective axle/tire factor are explicit test parameters.\n\n");
    printf(" time mph gear rpm  N/V  4th cmd phys brake TPS load fuel DFCO phase\n");

    step105_plant_init(&plant);
    MODE_WORD3|=FUEL_VATS_OK_BIT;
    sim_o2_adc=128u;
    BLM=128u;
    CLOSED_LOOP_PW_CORR=128u;
    BATTERY_AD=128u;
    RAM8(0x0030u)|=0x01u;
    RAM8(0x003Bu)|=0x10u;
    ram16be_set(0x005Bu,(bua_u16)(160u<<8));
    RAM8(0x005Fu)=160u;
    RAM8(0x0001u)|=0x02u;
    total_tick=0u;
    print_div=0u;
    passed=0u;
    total=0u;
    dfco_seen=0u;
    signature=STEP104_FNV_OFFSET;

    for(si=0u;si<STEP105_PROFILE_COUNT;++si) {
        s=&step105_profile[si];
        for(tick=0u;tick<s->duration_ticks;++tick) {
            speed=interpolate_u16(s->start_speed_mph,s->end_speed_mph,
                                  tick,s->duration_ticks);
            tps=interpolate_u16(s->start_tps_adc,s->end_tps_adc,
                                tick,s->duration_ticks);
            maf=interpolate_u16(s->start_maf_adc,s->end_maf_adc,
                                tick,s->duration_ticks);
            plant.in_drive=s->in_drive;
            plant.brake_power=s->brake_power;
            step105_select_gear(&plant,speed);
            if(s->in_drive!=0u) {
                target=step105_drive_rpm_target(&plant,speed,tps);
                rpm=step105_filter_engine_rpm(&plant,target);
            } else {
                rpm=interpolate_u16(s->start_non_drive_rpm,
                                    s->end_non_drive_rpm,
                                    tick,s->duration_ticks);
                plant.engine_rpm=(double)rpm;
            }
            apply_dynamic_command(speed,rpm,tps,maf,s->in_drive);
            irq_6p25ms();
            if((MINOR_COUNT&0x0Fu)==0x0Eu)
                step105_ecm_transmission_pass(&plant);
            if((AF_MODE_WORD&FUEL_DFCO_ZERO_BIT)!=0u)
                dfco_seen=1u;
            signature=step104_sig_tick(signature,si,tick);
            ++total_tick;
            ++print_div;
            if(tick==0u || print_div>=160u || tick+1u==s->duration_ticks) {
                printf("%5.1f %3u   %u %4u %3u    %u   %u    %u     %u %3u %4u %4u   %u %s\n",
                       (double)total_tick*0.00625,speed,(unsigned int)plant.gear,
                       (unsigned int)RPM_DIV25*25u,(unsigned int)RAM8(0x008Fu),
                       (unsigned int)((plant.classifier.mode04&STEP68_MODE_FOURTH)!=0u),
                       (unsigned int)((plant.tcc.status37&STEP67_TCC_STATUS_BIT)!=0u),
                       (unsigned int)plant.physical_tcc,
                       (unsigned int)plant.brake_power,
                       (unsigned int)TPS_LOAD_AXIS,(unsigned int)FILTERED_LOAD,
                       (unsigned int)mpu16be_get(0x3FD0u),
                       (unsigned int)((AF_MODE_WORD&FUEL_DFCO_ZERO_BIT)!=0u),
                       s->name);
                print_div=0u;
            }
        }
    }
    signature=step104_sig_final_stats(signature);

    printf("\n  gear-mask=%02X upshifts=%u downshifts=%u fourth-samples=%u\n",
           plant.gears_seen,plant.upshifts,plant.downshifts,
           plant.fourth_classified);
    printf("  TCC command-samples=%u physical-samples=%u brake-open=%u relocks=%u\n",
           plant.tcc_command_seen,plant.tcc_physical_seen,
           plant.brake_open_seen,plant.relock_seen);
    printf("  Step-105 transmission trace signature: %08lX\n",
           (unsigned long)signature);
    printf("\nStep-105 transmission-aware regression:\n");
    STEP105_CHECK(plant.rpm_commands_in_drive==0u,
                  "Drive RPM is derived by the plant and is never an independent profile command");
    STEP105_CHECK(plant.gears_seen==0x1Eu,
                  "the acceleration and deceleration trajectory visits all four forward gears");
    STEP105_CHECK(plant.upshifts==3u,
                  "acceleration produces exactly the expected 1-2, 2-3, and 3-4 upshifts");
    STEP105_CHECK(plant.downshifts==3u,
                  "deceleration produces exactly the expected 4-3, 3-2, and 2-1 downshifts");
    STEP105_CHECK(plant.fourth_classified>0u,
                  "measured RPM/VSS remains in the source fourth-gear window long enough to confirm");
    STEP105_CHECK(plant.tcc_command_in_fourth>0u,
                  "confirmed fourth gear remains present while translated TCC command is active");
    STEP105_CHECK(plant.tcc_physical_seen>0u,
                  "the HAL-side brake-power boundary permits physical TCC engagement");
    STEP105_CHECK(plant.brake_released_tcc>0u,
                  "opening brake power releases the physical clutch while the ECM still commands it");
    STEP105_CHECK(plant.relock_seen>0u,
                  "restoring brake power permits the commanded clutch to re-engage");
    STEP105_CHECK(dfco_seen!=0u,
                  "the mechanically coherent lift/deceleration trajectory still reaches DFCO");
    STEP105_CHECK(total_tick==4640u,
                  "the deterministic transmission profile contains exactly 4,640 IRQ ticks");
    STEP105_CHECK(signature==STEP105_EXPECTED_SIGNATURE,
                  "the complete transmission-aware RAM/MPU/I/O/statistics trace matches its baseline");
    printf("  step-105 transmission-aware regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP105_CHECK
}
