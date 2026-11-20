/* ---------------------------------------------------------------------- */
/* Phase-continuous dynamic vehicle drive-cycle stimulus.          */
/*                                                                        */
/* The command profile is evaluated every 6.25 ms.  VSS and HEI waveform  */
/* generators change frequency without resetting phase.  This avoids the  */
/* Step-8 boundary artifact and better represents a physical rotating      */
/* source whose pulse spacing continuously changes during acceleration.    */
/*                                                                        */
/* The profile itself remains a PC test stimulus, not a vehicle-dynamics   */
/* model and not a claim about original cluster filtering.                 */
/* ---------------------------------------------------------------------- */
typedef struct {
    unsigned int duration_ticks;  /* 6.25 ms ticks */
    unsigned int start_speed_mph;
    unsigned int end_speed_mph;
    unsigned int start_rpm;
    unsigned int end_rpm;
    unsigned int start_tps_adc;
    unsigned int end_tps_adc;
    unsigned int start_maf_adc;
    unsigned int end_maf_adc;
    bua_u8 in_drive;
    const char *name;
} DriveProfileSegment;
static const DriveProfileSegment drive_profile[] = {
    {160u,  0u,  0u,  350u,  800u, 38u,35u, 50u, 60u,0u,"cranking / catch"},
    {320u,  0u,  0u,  800u,  800u, 35u,35u, 55u, 55u,0u,"warm idle"},
    {960u,  0u, 60u,  800u, 2600u, 75u,110u,55u,170u,1u,"acceleration"},
    {640u, 60u, 60u, 2200u, 2200u, 60u,60u,110u,110u,1u,"60 MPH cruise"},
    {320u, 60u, 45u, 2200u, 1800u, 35u,35u,  0u,  0u,1u,"throttle lift / DFCO"},
    {640u, 45u,  0u, 1800u,  800u, 35u,35u,  0u, 55u,1u,"deceleration"},
    {1920u, 0u,  0u,  800u,  800u, 35u,35u, 55u, 55u,0u,"returned idle"}
};
#define DRIVE_PROFILE_COUNT ((unsigned int)(sizeof(drive_profile) / sizeof(drive_profile[0])))
static unsigned int interpolate_u16(unsigned int start_value,
                                    unsigned int end_value,
                                    unsigned int tick,
                                    unsigned int duration_ticks)
{
    long delta;
    long value;
    long numerator;
    if (duration_ticks <= 1u)
        return end_value;
    delta = (long)end_value - (long)start_value;
    numerator = delta * (long)tick;
    if (numerator >= 0l)
        numerator += (long)(duration_ticks - 1u) / 2l;
    else
        numerator -= (long)(duration_ticks - 1u) / 2l;
    value = (long)start_value + numerator / (long)(duration_ticks - 1u);
    if (value < 0l)
        value = 0l;
    return (unsigned int)value;
}
static void dash_apply_no_pulse_timeout(void)
{
    /* PC receiver convenience only: after 1 second without a new edge,
     * report zero speed/RPM.  This is NOT claimed to reproduce the 1986
     * cluster firmware timeout. */
    if (dash.vss_valid != 0u &&
        (sim_vehicle_time_us - dash.vss_last_rise_us) >= 1000000ul)
        dash.speed_mph = 0u;
    if (dash.tach_valid != 0u &&
        (sim_vehicle_time_us - dash.tach_last_rise_us) >= 1000000ul)
        dash.rpm = 0u;
}
static void apply_dynamic_command(unsigned int speed_mph, unsigned int rpm,
                                  unsigned int tps_adc,unsigned int maf_adc,
                                  bua_u8 in_drive)
{
    /* All three generators receive the same vehicle command but retain
     * independent timing state.  No decoder state is cleared here. */
    sim_set_vss_mph(speed_mph);
    sim_set_dash_vss_mph(speed_mph);
    sim_set_dash_tach_rpm(rpm);
    sim_set_ecm_reference_rpm(rpm);
    sim_tps_adc=(bua_u8)tps_adc;
    sim_maf_adc=(bua_u8)maf_adc;
    if(in_drive!=0u)
        RAM8(0x0037u)&=(bua_u8)~0x01u;
    else
        RAM8(0x0037u)|=0x01u;
}
