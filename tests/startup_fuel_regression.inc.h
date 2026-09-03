static void run_step48_startup_afr_decay_test(void)
{
    BuaStartupAfr48 s;
    BuaStartupAfr48 p;
    BuaAfrResult a;
    unsigned int passed=0u;
    unsigned int total=14u;
    unsigned int i;
    printf("\nStep-48 startup/open-loop AFR initialization and decay regression:\n");
#define STEP48_CHECK(c,t) do { if (c) ++passed; printf("  %-70s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    memset(&s,0,sizeof(s));
    s=bua_startup_afr_init_step48(96u,0u,0u,s); /* 32 C */
    STEP48_CHECK(s.startup_enrich==9216u && s.initial_delay==240u,
                 "32C pre-run init: C3E4=36 -> 0x2400; C3F4=120 -> delay 240");
    STEP48_CHECK(s.coolant_afr==25u,
                 "32C AFR-major coolant term C41F is 25");
    p=s; p.startup_enrich=1234u; p.initial_delay=77u;
    p=bua_startup_afr_init_step48(96u,1u,0u,p);
    STEP48_CHECK(p.startup_enrich==1234u && p.initial_delay==77u && p.coolant_afr==25u,
                 "engine-running major loop refreshes L00CD but does not reload L000E/L0010");
    p=s; p.startup_enrich=2222u; p.initial_delay=66u;
    p=bua_startup_afr_init_step48(96u,0u,STEP48_NVRAM_VALID_BIT,p);
    STEP48_CHECK(p.startup_enrich==2222u && p.initial_delay==66u,
                 "NV mode b3 bypasses startup-enrichment reinitialization");
    p=bua_startup_afr_init_step48(255u,0u,0u,s);
    STEP48_CHECK(p.startup_enrich==(bua_u16)(26u<<8) && p.initial_delay==134u,
                 "startup coolant is capped at raw 208 for the 116C table endpoint");
    p=s;
    p=bua_startup_afr_inject_step48(p,96u,1u);
    STEP48_CHECK(p.initial_delay==239u && p.startup_enrich==9216u,
                 "first running injection decrements initial delay without changing enrichment");
    p=s;
    for(i=0u;i<240u;++i) p=bua_startup_afr_inject_step48(p,96u,1u);
    STEP48_CHECK(p.initial_delay==0u && p.startup_enrich==9216u,
                 "240 delayed injection events reach zero with enrichment still unchanged");
    p=bua_startup_afr_inject_step48(p,96u,1u);
    STEP48_CHECK(p.startup_enrich==8964u && p.repeat_count==20u,
                 "next injection subtracts C410=252 and loads C402=20 repeat count");
    for(i=0u;i<20u;++i) p=bua_startup_afr_inject_step48(p,96u,1u);
    STEP48_CHECK(p.startup_enrich==8964u && p.repeat_count==0u,
                 "20 repeat-count events expire without a second decay");
    p=bua_startup_afr_inject_step48(p,96u,1u);
    STEP48_CHECK(p.startup_enrich==8712u && p.repeat_count==20u,
                 "test-before-decrement semantics make successive decays 21 injections apart");
    p.startup_enrich=100u; p.initial_delay=0u; p.repeat_count=0u;
    p=bua_startup_afr_inject_step48(p,96u,1u);
    STEP48_CHECK(p.startup_enrich==100u,
                 "LF6D5 sees L000E high byte zero and leaves the low-byte residual untouched");
    p=s;
    p=bua_startup_afr_inject_step48(p,96u,0u);
    STEP48_CHECK(p.initial_delay==240u && p.startup_enrich==9216u,
                 "cranking/non-running injector events do not decay startup enrichment");
    a=bua_afr_lda2d((bua_u8)(s.startup_enrich>>8),AF_CLOSED_LOOP_BIT,s.coolant_afr,128u);
    STEP48_CHECK(a.factor==292u && a.afr_code==507u,
                 "closed loop still retains startup high byte: factor 256+36 -> AFR code 507");
    a=bua_afr_lda2d((bua_u8)(s.startup_enrich>>8),0u,s.coolant_afr,128u);
    STEP48_CHECK(a.factor==295u && a.afr_code==512u,
                 "open loop combines startup 36 + bias 221 + coolant 25 + load 13 = 295");
    printf("  step-48 startup-AFR regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP48_CHECK
}
static double bua_event_count_time_estimate_step51(unsigned long events, unsigned int rpm);
/* ---------------------------------------------------------------------- */
/* Step 54: V8 DRP geometry and physical batch-injection cadence.         */
/*                                                                        */
/* Source-derived geometry:                                               */
/*   - LC009=$00 selects the 8-cylinder path.                             */
/*   - L3FC0 is repeatedly described as "Last 2 Ref Periods".             */
/*   - The factory test divides L3FC0 by four and explicitly labels the   */
/*     result as 45 crank degrees of retard.                              */
/* Therefore L3FC0 spans 180 crank degrees, each DRP interval is 90       */
/* crank degrees, and the V8 produces four DRPs per crank revolution.     */
/*                                                                        */
/* Independent 1227165 hardware evidence (schematic/board inspection)     */
/* shows D15/D16 tied to one injector-driver transistor.  That resolves   */
/* the physical meaning of the normal "double fire" calibration: all 8    */
/* injectors are pulsed together once per crank revolution, i.e. twice    */
/* per 720-degree four-stroke cycle.  The dormant single-fire software    */
/* retains the same service opportunity but suppresses alternate pulses, */
/* yielding one delivered batch pulse per 720-degree cycle.               */
/*                                                                        */
/* The GM MPU's internal logic that generates status b6 is still unknown; */
/* this block models only the now-supported observable cadence.           */
/* ---------------------------------------------------------------------- */
#define STEP54_CYL_CODE_8             0x00u
#define STEP54_REF_INTERVALS_L3FC0       2u
#define STEP54_L3FC0_CRANK_DEG         180u
#define STEP54_REF_CRANK_DEG            90u
#define STEP54_REFS_PER_REV              4u
#define STEP54_DOUBLE_BATCHES_PER_REV     1u
typedef struct {
    bua_u8 ref_phase;
    unsigned long services;
} BuaBatchCadence54;
static void bua_batch_cadence_init_step54(BuaBatchCadence54 *s)
{
    s->ref_phase=0u;
    s->services=0ul;
}
/* Hardware-backed observable scheduler model: one service per four DRPs. */
static bua_u8 bua_batch_cadence_ref_step54(BuaBatchCadence54 *s)
{
    ++s->ref_phase;
    if (s->ref_phase>=STEP54_REFS_PER_REV) {
        s->ref_phase=0u;
        ++s->services;
        return 1u;
    }
    return 0u;
}
static double bua_ref_rate_step54(unsigned int rpm)
{
    return ((double)rpm*(double)STEP54_REFS_PER_REV)/60.0;
}
static double bua_double_batch_rate_step54(unsigned int rpm)
{
    return ((double)rpm*(double)STEP54_DOUBLE_BATCHES_PER_REV)/60.0;
}
static double bua_single_delivered_rate_step54(unsigned int rpm)
{
    return ((double)rpm)/120.0;
}
static void run_step54_ref_batch_geometry_test(void)
{
    unsigned int passed=0u;
    unsigned int total=16u;
    BuaBatchCadence54 s;
    unsigned int i;
    unsigned int hits=0u;
    double r;
#define STEP54_CHECK(c,t) do { if(c) ++passed; printf("  %-76s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-54 V8 DRP geometry / batch-cadence regression:\n");
    STEP54_CHECK(STEP54_CYL_CODE_8==0u,
                 "LC009=$00 is the executable 8-cylinder selection code");
    STEP54_CHECK(STEP54_REF_INTERVALS_L3FC0==2u,
                 "L3FC0 is documented in-source as the last TWO reference periods");
    STEP54_CHECK((STEP54_L3FC0_CRANK_DEG/4u)==45u,
                 "factory-test L3FC0/4 = 45 degrees implies L3FC0 spans 180 degrees");
    STEP54_CHECK((STEP54_L3FC0_CRANK_DEG/STEP54_REF_INTERVALS_L3FC0)==90u,
                 "two reference intervals in 180 degrees gives 90 degrees per DRP interval");
    STEP54_CHECK((360u/STEP54_REF_CRANK_DEG)==STEP54_REFS_PER_REV,
                 "90-degree DRP spacing gives exactly four distributor references/revolution");
    bua_batch_cadence_init_step54(&s);
    for (i=0u;i<3u;++i) hits+=(unsigned int)bua_batch_cadence_ref_step54(&s);
    STEP54_CHECK(hits==0u && s.services==0ul,
                 "first three DRPs do not yet complete one physical batch-service revolution");
    hits+=(unsigned int)bua_batch_cadence_ref_step54(&s);
    STEP54_CHECK(hits==1u && s.services==1ul && s.ref_phase==0u,
                 "fourth DRP completes one crank revolution and one double-fire batch service");
    for (i=0u;i<4u;++i) hits+=(unsigned int)bua_batch_cadence_ref_step54(&s);
    STEP54_CHECK(hits==2u && s.services==2ul,
                 "eight DRPs correspond to two crank revolutions and two batch services");
    r=bua_ref_rate_step54(600u);
    STEP54_CHECK(r>39.99 && r<40.01,
                 "600 RPM produces 40 DRPs/sec with four references per revolution");
    r=bua_ref_rate_step54(1000u);
    STEP54_CHECK(r>66.66 && r<66.68,
                 "1000 RPM produces about 66.67 DRPs/sec");
    r=bua_double_batch_rate_step54(600u);
    STEP54_CHECK(r>9.99 && r<10.01,
                 "normal double-fire physical batch rate at 600 RPM is 10 pulses/sec");
    r=bua_double_batch_rate_step54(1000u);
    STEP54_CHECK(r>16.66 && r<16.68,
                 "normal double-fire physical batch rate at 1000 RPM is about 16.67/sec");
    r=bua_single_delivered_rate_step54(600u);
    STEP54_CHECK(r>4.99 && r<5.01,
                 "single-fire delivered-pulse rate at 600 RPM is 5/sec (alternate services zeroed)");
    STEP54_CHECK(bua_event_count_time_estimate_step51(240ul,600u)>23.99 &&
                 bua_event_count_time_estimate_step51(240ul,600u)<24.01,
                 "corrected Step-51 estimate: 240 injector services at 600 RPM = 24.0 s");
    STEP54_CHECK(bua_event_count_time_estimate_step51(240ul,1000u)>14.39 &&
                 bua_event_count_time_estimate_step51(240ul,1000u)<14.41,
                 "corrected Step-51 estimate: 240 injector services at 1000 RPM = 14.4 s");
    STEP54_CHECK(bua_event_count_time_estimate_step51(997ul,1000u)>59.81 &&
                 bua_event_count_time_estimate_step51(997ul,1000u)<59.83,
                 "corrected Step-49 full-decay example: 997 services at 1000 RPM = 59.82 s");
    printf("\n  Source-derived geometry: L3FC0 = two DRP intervals = 180 crank degrees,\n");
    printf("  so the 8-cylinder 9340 path has one DRP every 90 degrees (4/revolution).\n");
    printf("  Hardware cross-check: the 1227165 has one common injector driver, so\n");
    printf("  double-fire means one simultaneous all-8 pulse/revolution; single-fire\n");
    printf("  suppresses alternate scheduled pulses and delivers one pulse/720 degrees.\n");
    printf("  The MPU-internal mechanism creating status b6 remains undocumented.\n");
    printf("  step-54 DRP/batch-cadence regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP54_CHECK
}
/* ---------------------------------------------------------------------- */
/* Step 53: MPU control-word reconstruction boundary.                     */
/*                                                                        */
/* Startup first writes a small control value (000A normally, 0002 in    */
/* the ERR51 alternate mode), clears MPU RAM, and later replaces it with */
/* FB1A normally or FB12 in that alternate mode.  The only calibrated    */
/* difference between the two final words is low-byte bit 3.             */
/*                                                                        */
/* Runtime spark code independently manipulates low-byte bit 4: it sets  */
/* the bit while EST is enabled and clears it for the bypass/disable     */
/* path.  Serial/ALDL code manipulates low-byte bit 2.  Error-lamp code  */
/* manipulates low-byte bit 3.                                           */
/*                                                                        */
/* Therefore the executable lets us assign several control-bit roles,    */
/* but it does NOT expose an MPU bit that can safely be named             */
/* double-fire/bank-select/synchronous-fuel cadence.  Those remain inside*/
/* the MPU implementation unless external MPU documentation is found.    */
/* ---------------------------------------------------------------------- */
#define STEP53_MPU_BOOT_NORMAL       0x000Au
#define STEP53_MPU_BOOT_ERR51        0x0002u
#define STEP53_MPU_RUN_NORMAL        0xFB1Au
#define STEP53_MPU_RUN_ERR51         0xFB12u
#define STEP53_MPU_LOW_SERIAL_BIT    0x04u
#define STEP53_MPU_LOW_LAMP_BIT      0x08u
#define STEP53_MPU_LOW_SPARK_BIT     0x10u
typedef struct BuaMpuControl53Tag {
    bua_u16 word;
} BuaMpuControl53;
static bua_u16 bua_mpu_boot_word_step53(bua_u8 err51_mode)
{
    return err51_mode ? STEP53_MPU_BOOT_ERR51 : STEP53_MPU_BOOT_NORMAL;
}
static bua_u16 bua_mpu_run_word_step53(bua_u8 err51_mode)
{
    return err51_mode ? STEP53_MPU_RUN_ERR51 : STEP53_MPU_RUN_NORMAL;
}
static bua_u16 bua_mpu_low_set_step53(bua_u16 word,bua_u8 mask)
{
    return (bua_u16)(word | (bua_u16)mask);
}
static bua_u16 bua_mpu_low_clear_step53(bua_u16 word,bua_u8 mask)
{
    return (bua_u16)(word & (bua_u16)(0xFFFFu-(bua_u16)mask));
}
static void run_step53_mpu_control_test(void)
{
    unsigned int passed=0u;
    unsigned int total=13u;
    bua_u16 w;
#define STEP53_CHECK(c,t) do { if(c) ++passed; printf("  %-78s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-53 MPU control-word regression:\n");
    STEP53_CHECK(bua_mpu_boot_word_step53(0u)==0x000Au,
                 "normal early startup writes 000A to L3FFC");
    STEP53_CHECK(bua_mpu_boot_word_step53(1u)==0x0002u,
                 "ERR51 alternate early startup writes 0002 to L3FFC");
    STEP53_CHECK(bua_mpu_run_word_step53(0u)==0xFB1Au,
                 "normal post-checksum MPU initialization writes FB1A");
    STEP53_CHECK(bua_mpu_run_word_step53(1u)==0xFB12u,
                 "ERR51 alternate post-checksum MPU initialization writes FB12");
    STEP53_CHECK((STEP53_MPU_RUN_NORMAL ^ STEP53_MPU_RUN_ERR51)==0x0008u,
                 "FB1A versus FB12 differs only in low-byte bit 3");
    STEP53_CHECK((STEP53_MPU_BOOT_NORMAL ^ STEP53_MPU_BOOT_ERR51)==0x0008u,
                 "000A versus 0002 startup words likewise differ only in bit 3");
    w=bua_mpu_low_set_step53(STEP53_MPU_RUN_ERR51,STEP53_MPU_LOW_LAMP_BIT);
    STEP53_CHECK(w==STEP53_MPU_RUN_NORMAL,
                 "setting low-byte bit 3 converts the alternate final word to FB1A");
    w=bua_mpu_low_clear_step53(STEP53_MPU_RUN_NORMAL,STEP53_MPU_LOW_LAMP_BIT);
    STEP53_CHECK(w==STEP53_MPU_RUN_ERR51,
                 "clearing low-byte bit 3 converts FB1A to FB12");
    w=bua_mpu_low_clear_step53(STEP53_MPU_RUN_NORMAL,STEP53_MPU_LOW_SPARK_BIT);
    STEP53_CHECK(w==0xFB0Au,
                 "runtime spark-disable operation clears low-byte bit 4");
    w=bua_mpu_low_set_step53(w,STEP53_MPU_LOW_SPARK_BIT);
    STEP53_CHECK(w==STEP53_MPU_RUN_NORMAL,
                 "runtime spark-enable operation restores low-byte bit 4");
    w=bua_mpu_low_set_step53(STEP53_MPU_RUN_NORMAL,STEP53_MPU_LOW_SERIAL_BIT);
    STEP53_CHECK(w==0xFB1Eu,
                 "serial/ALDL operation can independently set low-byte bit 2");
    STEP53_CHECK((STEP53_MPU_RUN_NORMAL & 0xFF00u)==0xFB00u,
                 "normal initialized high byte is FB and is unchanged by these low-byte controls");
    STEP53_CHECK((STEP53_MPU_RUN_NORMAL & 0x0012u)==0x0012u,
                 "normal low byte has bits 4 and 1 set; source only proves bit 4's spark role here");
    printf("\n  Evidence boundary: L3FFC exposes several host-control bits, but the source\n");
    printf("  does not identify any one of them as the double-fire/bank/cadence selector.\n");
    printf("  LC014 b1 remains the software single-fire calibration selector; normal 9340\n");
    printf("  leaves it clear.  MPU synchronous-fuel cadence is still an internal rule.\n");
    printf("  step-53 MPU-control regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP53_CHECK
}
/* ---------------------------------------------------------------------- */
/* Step 52: MPU register interface and synchronous-fuel scheduling boundary.*/
/*                                                                        */
/* The executable gives unusually strong names/usages for several MPU     */
/* registers.  The normal fuel path writes the calculated synchronous PW  */
/* to L3FD0.  Normal startup/major-loop code explicitly writes zero to    */
/* L3FCE, while the only nonzero L3FCE write found is in the factory-test  */
/* spark/fuel routine.  In contrast, production spark code actively       */
/* updates L3FDC (dwell) and L3FF6 (ref-to-fire).                         */
/*                                                                        */
/* This means the 6801 specifies spark phase explicitly, but in normal     */
/* operation it does NOT program a per-event nonzero EFI delay.  The      */
/* synchronous-fuel firing phase/cadence therefore remains an MPU-internal*/
/* behavior; the host supplies PW and later observes the MPU service flag.*/
/* ---------------------------------------------------------------------- */
#define STEP52_ADDR_REF_PERIOD       0x3FC0u
#define STEP52_ADDR_EFI_DELAY        0x3FCEu
#define STEP52_ADDR_SYNC_FUEL        0x3FD0u
#define STEP52_ADDR_SPARK_DWELL      0x3FDCu
#define STEP52_ADDR_REF_TO_FIRE      0x3FF6u
#define STEP52_ADDR_STATUS           0x3FFAu
#define STEP52_FACTORY_10MS_COUNTS    655u
#define STEP52_FACTORY_5MS_COUNTS     327u
#define STEP52_FACTORY_1MS_COUNTS      66u
#define STEP52_FACTORY_EFI_DELAY_1MS   66u
typedef struct BuaMpuRegs52Tag {
    bua_u16 ref_period;
    bua_u16 efi_delay;
    bua_u16 sync_fuel_pw;
    bua_u16 spark_dwell;
    bua_u16 ref_to_fire;
    bua_u8 status_high;
} BuaMpuRegs52;
typedef struct BuaFactoryFuel52Tag {
    bua_u16 sync_fuel_pw;
    bua_u16 efi_delay;
} BuaFactoryFuel52;
static void bua_mpu_runtime_fuel_write_step52(BuaMpuRegs52 *m,bua_u16 pw)
{
    m->sync_fuel_pw=pw;
}
static void bua_mpu_runtime_efi_delay_clear_step52(BuaMpuRegs52 *m)
{
    m->efi_delay=0u;
}
static void bua_mpu_runtime_spark_write_step52(BuaMpuRegs52 *m,
                                                bua_u16 dwell,
                                                bua_u16 ref_to_fire)
{
    m->spark_dwell=dwell;
    m->ref_to_fire=ref_to_fire;
}
/* Factory-test LFE4F behavior only; this is not the production fuel path. */
static BuaFactoryFuel52 bua_factory_fuel_from_ref_step52(bua_u16 ref_period)
{
    BuaFactoryFuel52 r;
    r.efi_delay=0u;
    if (ref_period>655u) {
        r.sync_fuel_pw=STEP52_FACTORY_10MS_COUNTS;
    } else if (ref_period<328u) {
        r.sync_fuel_pw=STEP52_FACTORY_1MS_COUNTS;
        r.efi_delay=STEP52_FACTORY_EFI_DELAY_1MS;
    } else {
        r.sync_fuel_pw=STEP52_FACTORY_5MS_COUNTS;
    }
    return r;
}
static void run_step52_mpu_interface_test(void)
{
    unsigned int passed=0u;
    unsigned int total=14u;
    BuaMpuRegs52 m;
    BuaFactoryFuel52 f;
#define STEP52_CHECK(c,t) do { if(c) ++passed; printf("  %-78s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-52 MPU register-interface regression:\n");
    STEP52_CHECK(STEP52_ADDR_REF_PERIOD==0x3FC0u,
                 "L3FC0 is the MPU reference-period timer exposed to the 6801");
    STEP52_CHECK(STEP52_ADDR_EFI_DELAY==0x3FCEu,
                 "L3FCE is the EFI-delay register");
    STEP52_CHECK(STEP52_ADDR_SYNC_FUEL==0x3FD0u,
                 "L3FD0 is the synchronous-fuel pulse-width register");
    STEP52_CHECK(STEP52_ADDR_SPARK_DWELL==0x3FDCu && STEP52_ADDR_REF_TO_FIRE==0x3FF6u,
                 "L3FDC/L3FF6 are explicit spark dwell and ref-to-fire registers");
    STEP52_CHECK(STEP52_ADDR_STATUS==0x3FFAu,
                 "L3FFA is the MPU status register sampled by the 6.25-ms interrupt");
    m.ref_period=410u; m.efi_delay=123u; m.sync_fuel_pw=0u;
    m.spark_dwell=0u; m.ref_to_fire=0u; m.status_high=0u;
    bua_mpu_runtime_fuel_write_step52(&m,348u);
    STEP52_CHECK(m.sync_fuel_pw==348u && m.efi_delay==123u,
                 "ordinary LF9DA-style fuel write changes PW without inventing fuel phase");
    bua_mpu_runtime_efi_delay_clear_step52(&m);
    STEP52_CHECK(m.efi_delay==0u,
                 "normal LEA54 behavior explicitly clears EFI delay to zero");
    bua_mpu_runtime_spark_write_step52(&m,284u,900u);
    STEP52_CHECK(m.spark_dwell==284u && m.ref_to_fire==900u,
                 "production spark path actively programs both dwell and firing phase");
    f=bua_factory_fuel_from_ref_step52(700u);
    STEP52_CHECK(f.sync_fuel_pw==655u && f.efi_delay==0u,
                 "factory test below 100 Hz uses 10-ms fuel with zero EFI delay");
    f=bua_factory_fuel_from_ref_step52(655u);
    STEP52_CHECK(f.sync_fuel_pw==327u && f.efi_delay==0u,
                 "factory test exact 655-count boundary selects the 5-ms branch");
    f=bua_factory_fuel_from_ref_step52(400u);
    STEP52_CHECK(f.sync_fuel_pw==327u && f.efi_delay==0u,
                 "factory test 100-200 Hz region uses 5-ms fuel and zero delay");
    f=bua_factory_fuel_from_ref_step52(328u);
    STEP52_CHECK(f.sync_fuel_pw==327u && f.efi_delay==0u,
                 "factory test exact 328-count boundary remains in the 5-ms branch");
    f=bua_factory_fuel_from_ref_step52(327u);
    STEP52_CHECK(f.sync_fuel_pw==66u && f.efi_delay==66u,
                 "factory test above 200 Hz uses 1-ms fuel and a nonzero 66-count EFI delay");
    STEP52_CHECK(f.efi_delay!=0u,
                 "nonzero factory-test write proves L3FCE can schedule an EFI delay when requested");
    printf("\n  Executable-safe conclusion: production code supplies sync PW at L3FD0 while\n");
    printf("  leaving L3FCE at zero; spark phase is separately and actively programmed.\n");
    printf("  Therefore normal synchronous-injection phase/cadence remains MPU-internal,\n");
    printf("  not a per-event delay computed by the 6801 software.\n");
    printf("  step-52 MPU-interface regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP52_CHECK
}
/* ---------------------------------------------------------------------- */
/* Step 51: injector-service qualification and cadence boundary.          */
/*                                                                        */
/* The 6.25-ms interrupt copies the MPU status high byte from L3FFA into  */
/* L00A0.  The main injection routine LF67B performs its injection-event  */
/* bookkeeping only when L00A0 b6 is set.  The same MPU status byte uses  */
/* b3 for the DRP/reference-pulse indication, proving that "reference     */
/* occurred" and "injector service" are distinct hardware indications.   */
/*                                                                        */
/* The source does not expose the MPU's internal rule that generates b6,  */
/* so the exact b6-to-crank-angle relationship is intentionally NOT made  */
/* part of the executable-faithful core.  A separate estimator below uses*/
/* the Step-54 hardware-backed double-fire cadence (one all-injector batch */
/* service per crank revolution) to turn event counts into physical time. */
/* The MPU-internal generation of status b6 itself remains unmodeled.     */
/* ---------------------------------------------------------------------- */
#define STEP51_MPU_DRP_BIT            0x08u
#define STEP51_MPU_INJECT_SERVICE_BIT 0x40u
#define STEP51_IRQ_HZ                  160u
static bua_u8 bua_injector_service_due_step51(bua_u8 mpu_status_high)
{
    return (bua_u8)(((mpu_status_high&STEP51_MPU_INJECT_SERVICE_BIT)!=0u)?1u:0u);
}
static bua_u8 bua_drp_seen_step51(bua_u8 mpu_status_high)
{
    return (bua_u8)(((mpu_status_high&STEP51_MPU_DRP_BIT)!=0u)?1u:0u);
}
/*
 * Physical timing estimator.  Step 54 resolves the earlier ambiguity: on the
 * 1227165 the normal double-fire scheme is one all-injector batch service per
 * crank revolution (two delivered injections per 720-degree engine cycle).
 * The exact MPU gate-generation internals remain unknown, but the service rate
 * is now supported by source geometry plus independent 1227165 hardware evidence.
 */
static double bua_double_fire_event_rate_estimate_step51(unsigned int rpm)
{
    return ((double)rpm)/60.0;
}
static double bua_event_count_time_estimate_step51(unsigned long events,
                                                    unsigned int rpm)
{
    double rate=bua_double_fire_event_rate_estimate_step51(rpm);
    if (rate<=0.0) return 0.0;
    return (double)events/rate;
}
static void run_step51_injector_cadence_test(void)
{
    unsigned int passed=0u;
    unsigned int total=12u;
    double t240_600;
    double t240_1000;
    double t997_600;
    double t997_1000;
#define STEP51_CHECK(c,t) do { if(c) ++passed; printf("  %-72s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-51 injector-service qualification/cadence regression:\n");
    STEP51_CHECK(bua_injector_service_due_step51(0x00u)==0u,
                 "MPU status b6 clear: LF67B skips injector-event bookkeeping");
    STEP51_CHECK(bua_injector_service_due_step51(0x40u)==1u,
                 "MPU status b6 set: LF67B enters the injector-event path");
    STEP51_CHECK(bua_drp_seen_step51(0x08u)==1u,
                 "MPU status b3 independently denotes a DRP/reference occurrence");
    STEP51_CHECK(bua_injector_service_due_step51(0x08u)==0u,
                 "a DRP indication alone does not satisfy the LF67B b6 injection gate");
    STEP51_CHECK(bua_drp_seen_step51(0x40u)==0u,
                 "an injector-service indication need not also assert the DRP bit");
    STEP51_CHECK(bua_drp_seen_step51(0x48u)==1u &&
                 bua_injector_service_due_step51(0x48u)==1u,
                 "both hardware indications may coexist in the same sampled status byte");
    STEP51_CHECK(STEP51_IRQ_HZ==160u,
                 "software samples MPU status on the established 160-Hz/6.25-ms IRQ");
    STEP51_CHECK((1000u/STEP51_IRQ_HZ)==6u,
                 "integer check retains the 6.25-ms polling-domain interpretation");
    t240_600=bua_event_count_time_estimate_step51(240ul,600u);
    t240_1000=bua_event_count_time_estimate_step51(240ul,1000u);
    t997_600=bua_event_count_time_estimate_step51(997ul,600u);
    t997_1000=bua_event_count_time_estimate_step51(997ul,1000u);
    STEP51_CHECK(t240_600>23.99 && t240_600<24.01,
                 "hardware-backed: 240 DF services at 600 RPM are about 24.0 s");
    STEP51_CHECK(t240_1000>14.39 && t240_1000<14.41,
                 "hardware-backed: 240 DF services at 1000 RPM are about 14.4 s");
    STEP51_CHECK(t997_600>99.69 && t997_600<99.71,
                 "hardware-backed: 997 DF services at 600 RPM are about 99.70 s");
    STEP51_CHECK(t997_1000>59.81 && t997_1000<59.83,
                 "hardware-backed: 997 DF services at 1000 RPM are about 59.82 s");
    printf("\n  Executable-safe conclusion: LF67B is gated by sampled MPU-status b6.\n");
    printf("  DRP/ref uses separate MPU-status b3; source does not reveal MPU internal b6 scheduling.\n");
    printf("  Step 54 corrects the earlier provisional cadence: normal double-fire is one\n");
    printf("  all-injector batch service per crank revolution, not two services/revolution.\n");
    printf("    240 events: 600 RPM=%0.2f s, 1000 RPM=%0.2f s\n",t240_600,t240_1000);
    printf("    997 events: 600 RPM=%0.2f s, 1000 RPM=%0.2f s\n",t997_600,t997_1000);
    printf("  step-51 injector-cadence regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP51_CHECK
}
/* ---------------------------------------------------------------------- */
/* Step 50: single-fire / double-fire injector-event behavior.            */
/*                                                                        */
/* LCD8F selects single-fire only when LC014 b1 is set.  In the actual    */
/* 9340 image LC014=$B4, so b1 is clear and normal operation is double    */
/* fire.  The dormant executable path is retained faithfully: single     */
/* fire selects injector-flow constant 882 instead of 441, LD86B arms    */
/* L0039 b4, LF71C toggles b6 each injection service, and LF99B forces    */
/* pending fuel to zero whenever b7(single-fire) and b6(zero-PW phase)    */
/* are both set.                                                          */
/* ---------------------------------------------------------------------- */
#define STEP50_AF_MODE1_9340             0xB4u
#define STEP50_SINGLE_FIRE_SELECT_BIT    0x02u
#define STEP50_SF_ACTIVE_BIT             0x80u
#define STEP50_SF_TOGGLE_ENABLE_BIT      0x10u
#define STEP50_SF_ZERO_PW_BIT            0x40u
#define STEP50_INJ_FLOW_DOUBLE            441u
#define STEP50_INJ_FLOW_SINGLE            882u
typedef struct BuaSingleFire50Tag {
    bua_u8 mode_word;
    bua_u16 calculated_pw;
    bua_u16 pending_pw;
} BuaSingleFire50;
static BuaSingleFire50 bua_single_fire_enter_step50(bua_u8 cal_af_mode1)
{
    BuaSingleFire50 r;
    r.mode_word=0u;
    r.calculated_pw=0u;
    r.pending_pw=0u;
    if ((cal_af_mode1&STEP50_SINGLE_FIRE_SELECT_BIT)!=0u) {
        r.mode_word|=STEP50_SF_ACTIVE_BIT;
        r.mode_word&=(bua_u8)~(STEP50_SF_TOGGLE_ENABLE_BIT|STEP50_SF_ZERO_PW_BIT);
    }
    return r;
}
static bua_u16 bua_single_fire_flow_step50(bua_u8 mode_word)
{
    return ((mode_word&STEP50_SF_ACTIVE_BIT)!=0u) ?
        (bua_u16)STEP50_INJ_FLOW_SINGLE : (bua_u16)STEP50_INJ_FLOW_DOUBLE;
}
static BuaSingleFire50 bua_single_fire_event_step50(BuaSingleFire50 r,
                                                     bua_u16 double_fire_pw)
{
    /* LD86B: every normal running-fuel pass arms the SF toggle. */
    if ((r.mode_word&STEP50_SF_ACTIVE_BIT)!=0u)
        r.mode_word|=STEP50_SF_TOGGLE_ENABLE_BIT;
    /* Calculated base PW doubles because LC3D1 is exactly 2*LC3CF. */
    r.calculated_pw=((r.mode_word&STEP50_SF_ACTIVE_BIT)!=0u) ?
        (bua_u16)(double_fire_pw*2u) : double_fire_pw;
    /* LF71C: b4 is tested, not cleared here, so it remains armed. */
    if ((r.mode_word&STEP50_SF_TOGGLE_ENABLE_BIT)!=0u)
        r.mode_word^=STEP50_SF_ZERO_PW_BIT;
    /* LF99B: zero alternating pending fuel while b7+b6 are both set. */
    if ((r.mode_word&(STEP50_SF_ACTIVE_BIT|STEP50_SF_ZERO_PW_BIT))==
        (STEP50_SF_ACTIVE_BIT|STEP50_SF_ZERO_PW_BIT))
        r.pending_pw=0u;
    else
        r.pending_pw=r.calculated_pw;
    return r;
}
static void run_step50_single_fire_test(void)
{
    BuaSingleFire50 d;
    BuaSingleFire50 s;
    unsigned int passed=0u;
    unsigned int total=12u;
#define STEP50_CHECK(c,t) do { if(c) ++passed; printf("  %-72s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-50 single-fire/double-fire injector cadence regression:\n");
    d=bua_single_fire_enter_step50((bua_u8)STEP50_AF_MODE1_9340);
    STEP50_CHECK((d.mode_word&STEP50_SF_ACTIVE_BIT)==0u,
                 "actual 9340 LC014=$B4 has b1 clear: single-fire is not selected");
    STEP50_CHECK(bua_single_fire_flow_step50(d.mode_word)==441u,
                 "actual-image double-fire path selects LC3CF=441 injector-flow factor");
    d=bua_single_fire_event_step50(d,300u);
    STEP50_CHECK(d.calculated_pw==300u && d.pending_pw==300u,
                 "double-fire event passes the calculated pulse on every service");
    d=bua_single_fire_event_step50(d,300u);
    STEP50_CHECK(d.pending_pw==300u,
                 "double-fire second event also carries fuel; there is no alternating zero");
    s=bua_single_fire_enter_step50((bua_u8)(STEP50_AF_MODE1_9340|STEP50_SINGLE_FIRE_SELECT_BIT));
    STEP50_CHECK((s.mode_word&STEP50_SF_ACTIVE_BIT)!=0u &&
                 (s.mode_word&(STEP50_SF_TOGGLE_ENABLE_BIT|STEP50_SF_ZERO_PW_BIT))==0u,
                 "hypothetical b1-set calibration enters SF with toggle/zero phase cleared");
    STEP50_CHECK(bua_single_fire_flow_step50(s.mode_word)==882u,
                 "single-fire selects LC3D1=882, exactly twice double-fire calibration");
    s=bua_single_fire_event_step50(s,300u);
    STEP50_CHECK(s.calculated_pw==600u && s.pending_pw==0u &&
                 (s.mode_word&STEP50_SF_ZERO_PW_BIT)!=0u,
                 "first SF service doubles calculated PW then LF71C/LF99B suppress it");
    s=bua_single_fire_event_step50(s,300u);
    STEP50_CHECK(s.calculated_pw==600u && s.pending_pw==600u &&
                 (s.mode_word&STEP50_SF_ZERO_PW_BIT)==0u,
                 "second SF service toggles phase back and passes the doubled pulse");
    s=bua_single_fire_event_step50(s,300u);
    STEP50_CHECK(s.pending_pw==0u,
                 "third SF service is suppressed again: zero/fuel alternation persists");
    s=bua_single_fire_event_step50(s,300u);
    STEP50_CHECK(s.pending_pw==600u,
                 "fourth SF service passes fuel again");
    STEP50_CHECK((unsigned long)600u*2ul==(unsigned long)300u*4ul,
                 "over four services SF 0+600+0+600 equals DF 300+300+300+300");
    STEP50_CHECK((STEP50_AF_MODE1_9340&STEP50_SINGLE_FIRE_SELECT_BIT)==0u,
                 "therefore this alternating path is executable capability, not 9340 baseline");
    printf("  step-50 single-fire regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP50_CHECK
}
/* ---------------------------------------------------------------------- */
/* Step 49: continuous crank -> catch -> running-fuel integration.         */
/*                                                                        */
/* This is still a deterministic behavioral integration test rather than  */
/* a scheduler/timing claim.  It joins already translated executable      */
/* blocks in their established order: LD7B1 crank PW, LCD4B..LCD8F run    */
/* qualification, LEC72 startup-AFR initialization, LD9D7/LDA2D AFR,      */
/* calculated base PW, and LF695 injector-event startup decay.             */
/* ---------------------------------------------------------------------- */
typedef struct BuaStartFuel49Tag {
    BuaRunQual44 runq;
    BuaStartupAfr48 afr_start;
    BuaMafFilterState maf_filter;
    bua_u16 crank_pw;
    BuaSensorFuelResult running;
    bua_u8 fuel_path;
    bua_u32 injector_events;
} BuaStartFuel49;
static void bua_start49_init(BuaStartFuel49 *s, bua_u8 coolant)
{
    memset(s,0,sizeof(*s));
    s->afr_start=bua_startup_afr_init_step48(coolant,0u,0u,s->afr_start);
    s->crank_pw=bua_crank_pw_step43(coolant,0u,0u);
    s->fuel_path=(bua_u8)BUA_FUEL_PATH_CRANK44;
    bua_maf_filter_seed(&s->maf_filter,16185u);
}
static void bua_start49_reference(BuaStartFuel49 *s,bua_u16 period)
{
    bua_run_qual_ref_event_step44(&s->runq,period);
    s->fuel_path=(bua_u8)bua_select_fuel_path_core_step44(
        s->runq.engine_running ? ENGINE_RUNNING_BIT : 0u,0u,
        (bua_u8)CAL_CRANK_FF_SKIP_COUNT);
}
static BuaSensorFuelResult bua_start49_running_fuel(BuaStartFuel49 *s,
                                                    bua_u8 coolant,
                                                    bua_u8 maf_adc,
                                                    bua_u8 rpm25,
                                                    bua_u16 ref_period)
{
    s->afr_start=bua_startup_afr_init_step48(coolant,1u,0u,s->afr_start);
    s->running=bua_sensor_to_injector_step31(&s->maf_filter,maf_adc,rpm25,
        ref_period,(bua_u8)(s->afr_start.startup_enrich>>8),0u,
        s->afr_start.coolant_afr,128u,128u,128u,0u,0u);
    return s->running;
}
static void bua_start49_inject_event(BuaStartFuel49 *s,bua_u8 startup_coolant)
{
    if (s->runq.engine_running!=0u) {
        s->afr_start=bua_startup_afr_inject_step48(s->afr_start,startup_coolant,1u);
        ++s->injector_events;
    }
}
static void run_step49_continuous_start_fuel_test(void)
{
    BuaStartFuel49 s;
    BuaSensorFuelResult first;
    BuaSensorFuelResult after_delay;
    BuaSensorFuelResult later;
    BuaSensorFuelResult steady;
    bua_u16 crank_before;
    unsigned int i;
    unsigned int passed=0u;
    unsigned int total=16u;
#define STEP49_CHECK(c,t) do { if(c) ++passed; printf("  %-72s %s\n",t,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-49 continuous crank/catch/running-fuel regression:\n");
    bua_start49_init(&s,96u); /* 32 C */
    crank_before=s.crank_pw;
    STEP49_CHECK(s.fuel_path==(bua_u8)BUA_FUEL_PATH_CRANK44 && crank_before>0u,
                 "pre-catch state selects calibrated LD7B1 cranking fuel");
    STEP49_CHECK(s.afr_start.startup_enrich==9216u && s.afr_start.initial_delay==240u,
                 "pre-run major-loop state holds 32C startup term 0x2400 and delay 240");
    for(i=0u;i<9u;++i)
        bua_start49_reference(&s,(bua_u16)(CAL_STARTUP_REF_PERIOD-1u));
    STEP49_CHECK(s.runq.engine_running==0u && s.fuel_path==(bua_u8)BUA_FUEL_PATH_CRANK44,
                 "after nine qualifying references the ECM still selects crank fuel");
    bua_start49_reference(&s,(bua_u16)(CAL_STARTUP_REF_PERIOD-1u));
    STEP49_CHECK(s.runq.engine_running==1u && s.fuel_path==(bua_u8)BUA_FUEL_PATH_NORMAL44,
                 "tenth qualifying reference immediately selects the normal running path");
    first=bua_start49_running_fuel(&s,96u,128u,96u,410u);
    STEP49_CHECK(first.maf.final_q8_8==16185u,
                 "first running pass uses real analog-MAF path: ADC128 -> 63.22 g/s Q8.8");
    STEP49_CHECK(first.afr.factor==295u && first.afr.afr_code==512u,
                 "first running AFR retains startup 36 with open-loop coolant/load terms");
    STEP49_CHECK(first.base_pw>0u && first.output.hardware_pw>0u,
                 "post-catch MAF/AFR path produces nonzero calculated running injector PW");
    STEP49_CHECK(s.afr_start.startup_enrich==9216u && s.afr_start.initial_delay==240u,
                 "entering running fuel does not itself consume the injector-event decay delay");
    for(i=0u;i<240u;++i) bua_start49_inject_event(&s,96u);
    after_delay=bua_start49_running_fuel(&s,96u,128u,96u,410u);
    STEP49_CHECK(s.afr_start.initial_delay==0u && s.afr_start.startup_enrich==9216u,
                 "240 running injector events expire initial delay without enrichment decay");
    STEP49_CHECK(after_delay.afr.afr_code==first.afr.afr_code,
                 "AFR remains unchanged at the exact end of the initial-delay interval");
    bua_start49_inject_event(&s,96u);
    later=bua_start49_running_fuel(&s,96u,128u,96u,410u);
    STEP49_CHECK(s.afr_start.startup_enrich==8964u && s.afr_start.repeat_count==20u,
                 "next injector event performs first 252-count startup-enrichment decay");
    STEP49_CHECK(later.afr.afr_code<first.afr.afr_code && later.base_pw<=first.base_pw,
                 "first decay leans AFR; integer PW quantization may hold base PW unchanged");
    for(i=0u;i<2000u && (s.afr_start.startup_enrich & 0xFF00u)!=0u;++i)
        bua_start49_inject_event(&s,96u);
    steady=bua_start49_running_fuel(&s,96u,128u,96u,410u);
    STEP49_CHECK((s.afr_start.startup_enrich & 0xFF00u)==0u && s.afr_start.startup_enrich<256u,
                 "continued injector events clear the active high byte while preserving source low-byte residual");
    STEP49_CHECK(steady.afr.factor==259u && steady.afr.afr_code==450u,
                 "steady 32C open-loop factor becomes 221+25+13=259 (AFR code 450)");
    STEP49_CHECK(steady.base_pw<later.base_pw && later.base_pw<=first.base_pw,
                 "integrated base PW is non-increasing and ultimately falls as enrichment decays");
    STEP49_CHECK(s.injector_events>240u,
                 "startup decay is driven by counted running injector events, not wall time");
    printf("  example raw PW: crank=%u first-run=%u first-decay=%u steady=%u\n",
           (unsigned int)crank_before,(unsigned int)first.base_pw,
           (unsigned int)later.base_pw,(unsigned int)steady.base_pw);
    printf("  example AFR codes: first=%u first-decay=%u steady=%u; decay events=%lu\n",
           (unsigned int)first.afr.afr_code,(unsigned int)later.afr.afr_code,
           (unsigned int)steady.afr.afr_code,(unsigned long)s.injector_events);
    printf("  step-49 continuous-start regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP49_CHECK
}
/* ---------------------------------------------------------------------- */
/* Step 55: reference-driven physical injector scheduler.                 */
/*                                                                        */
/* This removes the synthetic "injector event" stimulus from the start   */
/* integration.  The physical model consumes one distributor-reference   */
/* event at a time.  Step 54 established four 90-degree DRPs/revolution   */
/* and one observable all-8 batch pulse/revolution for the normal 9340    */
/* double-fire calibration.  Therefore every fourth DRP produces one      */
/* injector-service opportunity and directly clocks LF695 startup decay.  */
/*                                                                        */
/* The exact phase of that once/revolution batch relative to a particular */
/* distributor reference remains MPU-internal/unknown.  ref_phase=0 here  */
/* is a simulator phase origin only; cadence, counts and elapsed timing do */
/* not depend on that arbitrary phase choice.                             */
/* ---------------------------------------------------------------------- */
typedef struct {
    BuaBatchCadence54 batch;
    unsigned long drps;
    unsigned long revolutions;
    unsigned long injector_services;
} BuaPhysicalInjector55;
static void bua_physical_injector_init_step55(BuaPhysicalInjector55 *s)
{
    bua_batch_cadence_init_step54(&s->batch);
    s->drps=0ul;
    s->revolutions=0ul;
    s->injector_services=0ul;
}
/* Return 1 exactly when this DRP completes a normal double-fire service. */
static bua_u8 bua_physical_injector_ref_step55(BuaPhysicalInjector55 *s)
{
    bua_u8 service;
    ++s->drps;
    service=bua_batch_cadence_ref_step54(&s->batch);
    if (service!=0u) {
        ++s->revolutions;
        ++s->injector_services;
    }
    return service;
}
static void bua_start49_physical_ref_step55(BuaStartFuel49 *start,
                                             BuaPhysicalInjector55 *inj,
                                             bua_u16 ref_period,
                                             bua_u8 startup_coolant)
{
    bua_start49_reference(start,ref_period);
    if (bua_physical_injector_ref_step55(inj)!=0u)
        bua_start49_inject_event(start,startup_coolant);
}
static double bua_drps_time_step55(unsigned long drps,unsigned int rpm)
{
    double rate=bua_ref_rate_step54(rpm);
    if (rate<=0.0) return 0.0;
    return (double)drps/rate;
}
static void run_step55_physical_injector_scheduler_test(void)
{
    BuaPhysicalInjector55 inj;
    BuaStartFuel49 start;
    unsigned int passed=0u;
    unsigned int total=21u;
    unsigned long i;
    double t;
#define STEP55_CHECK(c,tmsg) do { if(c) ++passed; printf("  %-78s %s\n",tmsg,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-55 reference-driven physical injector scheduler regression:\n");
    bua_physical_injector_init_step55(&inj);
    for(i=0ul;i<3ul;++i)
        (void)bua_physical_injector_ref_step55(&inj);
    STEP55_CHECK(inj.drps==3ul && inj.injector_services==0ul,
                 "three 90-degree DRPs produce no completed once/revolution batch service");
    {
        bua_u8 fourth_service=bua_physical_injector_ref_step55(&inj);
        STEP55_CHECK(fourth_service==1u &&
                     inj.drps==4ul && inj.revolutions==1ul && inj.injector_services==1ul,
                     "fourth DRP completes one revolution and generates one physical batch service");
    }
    for(i=0ul;i<396ul;++i)
        (void)bua_physical_injector_ref_step55(&inj);
    STEP55_CHECK(inj.drps==400ul && inj.revolutions==100ul && inj.injector_services==100ul,
                 "400 DRPs deterministically generate 100 normal 9340 injector services");
    t=bua_drps_time_step55(400ul,1000u);
    STEP55_CHECK(t>5.999 && t<6.001,
                 "400 DRPs at 1000 RPM represent exactly 100 revolutions = 6.0 seconds");
    STEP55_CHECK(bua_event_count_time_estimate_step51(100ul,1000u)>5.999 &&
                 bua_event_count_time_estimate_step51(100ul,1000u)<6.001,
                 "physical DRP timing agrees with corrected Step-51 one-service/revolution estimator");
    /* Integrate the scheduler with the actual Step-49 startup state. */
    bua_start49_init(&start,96u); /* 32 C: enrich 0x2400, delay 240 */
    bua_physical_injector_init_step55(&inj);
    for(i=0ul;i<10ul;++i)
        bua_start49_physical_ref_step55(&start,&inj,
            (bua_u16)(CAL_STARTUP_REF_PERIOD-1u),96u);
    STEP55_CHECK(start.runq.engine_running==1u,
                 "ten qualifying DRPs still establish ENGINE RUNNING through Step 44");
    STEP55_CHECK(inj.injector_services==2ul,
                 "the same ten DRPs contain two physical once/revolution batch services");
    STEP55_CHECK(start.injector_events==0ul && start.afr_start.initial_delay==240u,
                 "both services before the tenth-DRP catch are ignored by running startup decay");
    /* First post-catch service is at DRP 12; 240 running services end at DRP 968. */
    for(i=0ul;i<958ul;++i)
        bua_start49_physical_ref_step55(&start,&inj,
            (bua_u16)(CAL_STARTUP_REF_PERIOD-1u),96u);
    STEP55_CHECK(start.injector_events==240ul && start.afr_start.initial_delay==0u &&
                 start.afr_start.startup_enrich==9216u,
                 "after 240 running physical services the initial delay is exhausted, enrichment unchanged");
    STEP55_CHECK(inj.drps==968ul && inj.injector_services==242ul,
                 "catch plus delay consumed 968 total DRPs and 242 physical services (two pre-catch)");
    /* Four more DRPs produce running service 241, the first actual decay. */
    for(i=0ul;i<4ul;++i)
        bua_start49_physical_ref_step55(&start,&inj,
            (bua_u16)(CAL_STARTUP_REF_PERIOD-1u),96u);
    STEP55_CHECK(start.injector_events==241ul && start.afr_start.startup_enrich==8964u,
                 "next revolution clocks running service 241 and performs first 0x2400-252 decay");
    STEP55_CHECK(start.afr_start.repeat_count==20u,
                 "first physical decay reloads the calibrated 20-service repeat counter");
    /* 20 services decrement repeat to zero; the 21st performs next decay. */
    for(i=0ul;i<80ul;++i)
        bua_start49_physical_ref_step55(&start,&inj,
            (bua_u16)(CAL_STARTUP_REF_PERIOD-1u),96u);
    STEP55_CHECK(start.injector_events==261ul && start.afr_start.repeat_count==0u &&
                 start.afr_start.startup_enrich==8964u,
                 "20 more revolutions decrement repeat 20->0 without a second enrichment subtraction");
    for(i=0ul;i<4ul;++i)
        bua_start49_physical_ref_step55(&start,&inj,
            (bua_u16)(CAL_STARTUP_REF_PERIOD-1u),96u);
    STEP55_CHECK(start.injector_events==262ul && start.afr_start.startup_enrich==8712u,
                 "21st subsequent revolution performs the next 252-count startup-enrichment decay");
    STEP55_CHECK(inj.injector_services*4ul==inj.drps-(unsigned long)inj.batch.ref_phase,
                 "scheduler maintains exact four-DRP/service cadence independent of integration state");
    STEP55_CHECK(start.injector_events+2ul==inj.injector_services,
                 "running event count differs by exactly the two pre-catch physical services");
    t=bua_drps_time_step55(960ul,1000u);
    STEP55_CHECK(t>14.399 && t<14.401,
                 "240 physical service intervals = 960 DRPs = 14.4 seconds at fixed 1000 RPM");
    t=bua_drps_time_step55(960ul,600u);
    STEP55_CHECK(t>23.999 && t<24.001,
                 "the same 240 service intervals = 24.0 seconds at fixed 600 RPM");
    /* Phase origin test: cadence count is invariant over complete revolutions. */
    bua_physical_injector_init_step55(&inj);
    inj.batch.ref_phase=2u; /* arbitrary MPU phase origin */
    for(i=0ul;i<40ul;++i)
        (void)bua_physical_injector_ref_step55(&inj);
    STEP55_CHECK(inj.injector_services==10ul,
                 "arbitrary two-DRP phase origin still yields ten services across forty DRPs");
    STEP55_CHECK(inj.batch.ref_phase==2u,
                 "complete-revolution window preserves the arbitrary starting phase");
    STEP55_CHECK(STEP54_REFS_PER_REV==4u && STEP54_DOUBLE_BATCHES_PER_REV==1u,
                 "physical scheduler is explicitly parameterized as 4 DRPs/rev and 1 batch/rev");
    printf("\n  Step 55 removes manual injector-event stimulation from the start model.\n");
    printf("  Distributor references now drive both run qualification and physical batch cadence;\n");
    printf("  each fourth DRP creates the normal 9340 all-8 injector-service opportunity, and\n");
    printf("  only services occurring after ENGINE RUNNING clock the LF695 startup decay state.\n");
    printf("  Absolute batch phase relative to a named DRP remains intentionally unspecified.\n");
    printf("  step-55 physical injector scheduler regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP55_CHECK
}
/* ---------------------------------------------------------------------- */
/* Step 56: changing-RPM crank/catch/idle trajectory.                     */
/*                                                                        */
/* The trajectory itself is a PC-side plant stimulus, not a claim about  */
/* a calibrated 9340 idle profile.  What is source-derived is the 65.536 */
/* kHz DRP period representation, LC018=3277 run threshold, ten qualified*/
/* references from Step 44, four DRPs/revolution, and one normal batch   */
/* service/revolution established in Steps 54-55.                         */
/* ---------------------------------------------------------------------- */
#define STEP56_TIMER_HZ 65536.0
#define STEP56_CRANK_LOW_RPM 250u
#define STEP56_CATCH_RPM 400u
#define STEP56_HIGH_IDLE_RPM 1200u
#define STEP56_WARM_IDLE_RPM 650u
#define STEP56_WARMDOWN_SECONDS 90.0
typedef struct {
    BuaStartFuel49 start;
    BuaPhysicalInjector55 inj;
    double seconds;
    unsigned long refs;
    unsigned long running_services;
    double catch_seconds;
    unsigned long catch_ref;
    bua_u16 last_ref_period;
    unsigned int last_rpm;
} BuaStartTrajectory56;
static bua_u16 bua_ref_period_from_rpm_step56(unsigned int rpm)
{
    double counts;
    if (rpm==0u) return 65535u;
    counts=(STEP56_TIMER_HZ*60.0)/((double)STEP54_REFS_PER_REV*(double)rpm);
    if (counts>65535.0) return 65535u;
    if (counts<1.0) return 1u;
    return (bua_u16)(counts+0.5);
}
static double bua_ref_seconds_from_rpm_step56(unsigned int rpm)
{
    if (rpm==0u) return 0.0;
    return 60.0/((double)STEP54_REFS_PER_REV*(double)rpm);
}
static unsigned int bua_warmdown_rpm_step56(double since_catch)
{
    double f;
    double rpm;
    if (since_catch<=0.0) return STEP56_HIGH_IDLE_RPM;
    if (since_catch>=STEP56_WARMDOWN_SECONDS) return STEP56_WARM_IDLE_RPM;
    f=since_catch/STEP56_WARMDOWN_SECONDS;
    rpm=(double)STEP56_HIGH_IDLE_RPM-
        ((double)(STEP56_HIGH_IDLE_RPM-STEP56_WARM_IDLE_RPM)*f);
    return (unsigned int)(rpm+0.5);
}
static void bua_start_trajectory_init_step56(BuaStartTrajectory56 *s,bua_u8 coolant)
{
    bua_start49_init(&s->start,coolant);
    bua_physical_injector_init_step55(&s->inj);
    s->seconds=0.0;
    s->refs=0ul;
    s->running_services=0ul;
    s->catch_seconds=-1.0;
    s->catch_ref=0ul;
    s->last_ref_period=65535u;
    s->last_rpm=0u;
}
static bua_u8 bua_start_trajectory_ref_step56(BuaStartTrajectory56 *s,
                                               unsigned int rpm,
                                               bua_u8 coolant)
{
    bua_u8 was_running=s->start.runq.engine_running;
    bua_u8 service;
    bua_u16 period=bua_ref_period_from_rpm_step56(rpm);
    s->seconds+=bua_ref_seconds_from_rpm_step56(rpm);
    ++s->refs;
    s->last_rpm=rpm;
    s->last_ref_period=period;
    bua_start49_reference(&s->start,period);
    if (was_running==0u && s->start.runq.engine_running!=0u) {
        s->catch_seconds=s->seconds;
        s->catch_ref=s->refs;
    }
    service=bua_physical_injector_ref_step55(&s->inj);
    if (service!=0u && s->start.runq.engine_running!=0u) {
        bua_start49_inject_event(&s->start,coolant);
        ++s->running_services;
    }
    return service;
}
static void run_step56_dynamic_start_trajectory_test(void)
{
    BuaStartTrajectory56 s;
    unsigned int passed=0u,total=19u;
    unsigned long i;
    unsigned int rpm;
    double since_catch;
    bua_u16 p250,p400,p1200,p650;
#define STEP56_CHECK(c,tmsg) do { if(c) ++passed; printf("  %-78s %s\n",tmsg,(c)?"PASS":"FAIL"); } while(0)
    printf("\nStep-56 changing-RPM crank/catch/idle trajectory regression:\n");
    p250=bua_ref_period_from_rpm_step56(250u);
    p400=bua_ref_period_from_rpm_step56(400u);
    p1200=bua_ref_period_from_rpm_step56(1200u);
    p650=bua_ref_period_from_rpm_step56(650u);
    STEP56_CHECK(p250>CAL_STARTUP_REF_PERIOD && p400<CAL_STARTUP_REF_PERIOD,
                 "250-RPM DRP period is below run speed; 400-RPM period qualifies for catch");
    STEP56_CHECK(p1200<p650 && p650<p400,
                 "MPU-style reference period decreases monotonically as RPM increases");
    bua_start_trajectory_init_step56(&s,96u); /* 32 C */
    for(i=0ul;i<12ul;++i)
        (void)bua_start_trajectory_ref_step56(&s,STEP56_CRANK_LOW_RPM,96u);
    STEP56_CHECK(s.start.runq.engine_running==0u && s.start.runq.startup_counter==0u,
                 "twelve 250-RPM references leave ENGINE RUNNING clear and reset qualification count");
    STEP56_CHECK(s.inj.injector_services==3ul && s.start.injector_events==0ul,
                 "low-speed cranking has physical batch opportunities but no running decay events");
    for(i=0ul;i<10ul;++i)
        (void)bua_start_trajectory_ref_step56(&s,STEP56_CATCH_RPM,96u);
    STEP56_CHECK(s.start.runq.engine_running!=0u && s.catch_ref==22ul,
                 "ten consecutive 400-RPM references after low cranking establish catch at reference 22");
    STEP56_CHECK(s.catch_seconds>1.094 && s.catch_seconds<1.096,
                 "catch occurs at 1.095 s from twelve 250-RPM plus ten 400-RPM reference intervals");
    STEP56_CHECK(s.start.injector_events==0ul,
                 "catch at DRP 22 occurs between batch opportunities; first running service waits for DRP 24");
    /* Run until 100 s after catch with a linear 1200 -> 650 RPM PC-side plant trajectory. */
    while ((s.seconds-s.catch_seconds)<100.0) {
        since_catch=s.seconds-s.catch_seconds;
        rpm=bua_warmdown_rpm_step56(since_catch);
        (void)bua_start_trajectory_ref_step56(&s,rpm,96u);
    }
    STEP56_CHECK(s.last_rpm==STEP56_WARM_IDLE_RPM,
                 "after the 90-second stimulus warm-down the generated engine speed holds 650 RPM");
    STEP56_CHECK(s.running_services>1000ul,
                 "variable-RPM trajectory generates more than one thousand physical running batch services");
    STEP56_CHECK(s.start.injector_events==s.running_services,
                 "every post-catch physical batch service clocks exactly one Step-49 injector event");
    STEP56_CHECK((s.start.afr_start.startup_enrich & 0xFF00u)==0u,
                 "after 100 seconds the startup-enrichment high byte has decayed to zero contribution");
    STEP56_CHECK(s.start.afr_start.initial_delay==0u,
                 "variable-RPM physical services exhaust the calibrated 240-service initial delay");
    STEP56_CHECK(s.refs==s.inj.drps,
                 "trajectory reference count and physical scheduler DRP count remain identical");
    STEP56_CHECK(s.inj.injector_services*4ul<=s.inj.drps &&
                 s.inj.drps-s.inj.injector_services*4ul<4ul,
                 "all physical injector services retain the exact once-per-four-DRP cadence");
    STEP56_CHECK(s.seconds>100.0 && s.seconds<102.0,
                 "elapsed simulation time is integrated from each instantaneous-RPM DRP interval");
    STEP56_CHECK(bua_warmdown_rpm_step56(0.0)==1200u &&
                 bua_warmdown_rpm_step56(45.0)==925u &&
                 bua_warmdown_rpm_step56(90.0)==650u,
                 "PC plant trajectory is 1200 RPM at catch, 925 at 45 s, and 650 at 90 s");
    STEP56_CHECK(bua_ref_period_from_rpm_step56(300u)==3277u,
                 "300 RPM rounds to LC018=3277 counts, matching the source run-threshold boundary");
    STEP56_CHECK(s.start.fuel_path==(bua_u8)BUA_FUEL_PATH_NORMAL44,
                 "fuel path remains on normal running fuel throughout the post-catch idle trajectory");
    STEP56_CHECK(s.start.runq.startup_counter==8u,
                 "run qualification preserves its eight-count terminal state after ENGINE RUNNING is set");
    printf("  catch: ref=%lu time=%0.3f s; final time=%0.3f s refs=%lu services=%lu\n",
           s.catch_ref,s.catch_seconds,s.seconds,s.refs,s.running_services);
    printf("  final: rpm=%u refPeriod=%u startup=0x%04X delay=%u repeat=%u\n",
           s.last_rpm,(unsigned int)s.last_ref_period,
           (unsigned int)s.start.afr_start.startup_enrich,
           (unsigned int)s.start.afr_start.initial_delay,
           (unsigned int)s.start.afr_start.repeat_count);
    printf("  NOTE: the 1200->650 RPM warm-down is a simulator plant stimulus, not a claimed 9340 calibration.\n");
    printf("  step-56 dynamic start trajectory regression result: %s (%u/%u)\n",
           (passed==total)?"PASS":"FAIL",passed,total);
#undef STEP56_CHECK
}
