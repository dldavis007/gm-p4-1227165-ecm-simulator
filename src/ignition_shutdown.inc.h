/* ------------------------------------------------------------------------- */
/* Step 111: live key-off, IAC homing, and software-powerdown boundary.       */
/* Listing blocks: $D387..$D3D2 and $D6D1..$D769.                            */
/* ------------------------------------------------------------------------- */
#define STEP111_IGNITION_OFF_BIT       0x10u
#define STEP111_ENGINE_RUNNING_BIT     0x80u
#define STEP111_SKIP_IAC_RESET_BIT     0x10u
#define STEP111_IAC_CLOSE_PHASE_BIT    0x04u
#define STEP111_IAC_OPEN_PHASE_BIT     0x02u
#define STEP111_HOUSEKEEPING_COUNT     8u
#define STEP111_POWERDOWN_COUNT        0x0385u /* LC012 */
#define STEP111_IAC_PARK_POSITION      144u    /* LC62F */
#define STEP111_SAM_LOW                118u    /* LC5E6/LC5E8 */
#define STEP111_SAM_HIGH               150u    /* LC5E7/LC5E9 */
#define STEP111_TPS_LOW_ERROR_LIMIT    12u     /* LC1EB */

static bua_u8 bua_limit_step111(bua_u8 value,bua_u8 low,bua_u8 high)
{
    if(value<=low)
        return low;
    if(value>high)
        return high;
    return value;
}

/* LF447: commit bounded SAM high bytes into the sixteen persistent BLM cells. */
static void bua_ignition_commit_blm_lf447_step111(void)
{
    bua_u8 sam_a;
    bua_u8 sam_b;
    unsigned int i;

    sam_b=bua_limit_step111(RAM8(0x000Cu),STEP111_SAM_LOW,STEP111_SAM_HIGH);
    RAM8(0x000Cu)=sam_b;
    for(i=0u;i<16u;++i)
        RAM8((bua_u16)(0x001Cu+i))=sam_b;
    sam_a=bua_limit_step111(RAM8(0x000Au),STEP111_SAM_LOW,STEP111_SAM_HIGH);
    RAM8(0x000Au)=sam_a;
    RAM8(0x001Cu)=sam_a; /* LC5EA selects cell zero. */
}

/* The HAL owns physical keep-alive power. The PC records and latches the
   exact point where $D6EA enters the software-interrupt loop. */
static void bua_hal_request_soft_powerdown_step111(void)
{
    if(sim_soft_powerdown_latched==0u) {
        sim_soft_powerdown_latched=1u;
        ++stats.soft_powerdown_events;
        bua_vector_note_swi_step119(0xD6EAu);
    }
}

/* $D727..$D769, which executes after either ignition-on timer reset or the
   nonterminal key-off path and before the load/fuel producer. */
static void bua_ignition_tail_ld727_step111(void)
{
    bua_u8 diag_word;
    bua_u8 current_tps;
    bua_u8 previous_tps;
    bua_u8 delta;

    diag_word=(bua_u8)(RAM8(0x003Fu)&0xDFu);
    if((RAM8(0x0034u)&STEP111_ENGINE_RUNNING_BIT)!=0u) {
        if(RAM8(0x0081u)<STEP111_TPS_LOW_ERROR_LIMIT) {
            RAM8(0x004Cu)=(bua_u8)(RAM8(0x004Cu)|0x02u);
            diag_word=(bua_u8)(diag_word|0x20u);
        }
    } else {
        RAM8(0x0086u)=35u; /* LC39B */
        RAM8(0x0087u)=0u;
    }
    RAM8(0x003Fu)=diag_word;

    current_tps=RAM8(0x0082u);
    previous_tps=RAM8(0x0083u);
    if(current_tps>=previous_tps) {
        delta=(bua_u8)(current_tps-previous_tps);
        /* LC66E=$FF and LC66F=$00 in this image. */
        if(delta>=255u && RAM8(0x0106u)==0u) {
            RAM8(0x0106u)=0u;
            RAM8(0x0034u)=(bua_u8)(RAM8(0x0034u)|0x20u);
        }
    }
    RAM8(0x0083u)=current_tps; /* $D769..$D76B: save current load axis. */
}

/* Live $D6D1 front. Return one only when the SWI/power-hold endpoint stops
   this odd-loop invocation before $D727 and the fuel chain. */
static bua_u8 bua_ignition_shutdown_odd_step111(void)
{
    bua_u16 timer;
    bua_u8 engine_mode;

    ++stats.ignition_shutdown_calls;
    timer=ram16be_get(0x008Bu);
    if((RAM8(0x0033u)&STEP111_IGNITION_OFF_BIT)==0u) {
        ram16be_set(0x008Bu,0u); /* LDX #$FFFF; INX */
        bua_ignition_tail_ld727_step111();
        return 0u;
    }

    if(timer<STEP111_HOUSEKEEPING_COUNT) {
        ram16be_set(0x008Bu,(bua_u16)(timer+1u));
        bua_ignition_tail_ld727_step111();
        return 0u;
    }

    RAM8(0x0001u)=0u;
    RAM8(0x0002u)=(bua_u8)(RAM8(0x0002u)&STEP111_SKIP_IAC_RESET_BIT);
    RAM8(0x0003u)=(bua_u8)(RAM8(0x0003u)&0x40u);
    if(timer>=STEP111_POWERDOWN_COUNT) {
        bua_hal_request_soft_powerdown_step111();
        return 1u;
    }

    timer=(bua_u16)(timer+1u);
    ram16be_set(0x008Bu,timer);
    ram16be_set(0x001Au,0u);
    engine_mode=RAM8(0x0034u);
    if((engine_mode&STEP111_ENGINE_RUNNING_BIT)!=0u) {
        bua_ignition_commit_blm_lf447_step111();
        engine_mode=RAM8(0x0034u);
        if((RAM8(0x0033u)&0x20u)!=0u && RAM8(0x0065u)==0u &&
           (RAM8(0x003Eu)&0x10u)==0u)
            RAM8(0x003Eu)=(bua_u8)(RAM8(0x003Eu)|0x40u);
    }
    RAM8(0x0034u)=(bua_u8)(engine_mode&0x7Fu);
    RAM8(0x018Du)=0u;
    RAM8(0x0093u)=0u;
    RAM8(0x003Fu)=(bua_u8)(RAM8(0x003Fu)&0xFEu);
    RAM8(0x0039u)=(bua_u8)(RAM8(0x0039u)&0x7Fu);
    bua_ignition_tail_ld727_step111();
    return 0u;
}

static bua_u8 bua_iac_move_to_park_step111(bua_u8 position)
{
    bua_u8 magnitude;
    if(position>=STEP111_IAC_PARK_POSITION) {
        magnitude=(bua_u8)(position-STEP111_IAC_PARK_POSITION);
        if(magnitude>127u)
            magnitude=127u;
        if(magnitude!=0u)
            magnitude=(bua_u8)(magnitude|0x80u);
        return magnitude;
    }
    return (bua_u8)(STEP111_IAC_PARK_POSITION-position);
}

/* LD335 startup/reset initializer used if homing completes with ignition on. */
static void bua_iac_restart_ld335_step111(void)
{
    bua_u8 command;
    if((RAM8(0x003Du)&0x40u)!=0u)
        RAM8(0x002Du)=32u; /* LC665 */
    command=bua_iac_move_to_park_step111(RAM8(0x002Cu));
    RAM8(0x0101u)=command;
    RAM8(0x0109u)=0xFFu;
    RAM8(0x0102u)=0xFFu;
    RAM8(0x00F2u)=0x80u;
    RAM8(0x00F3u)=((RAM8(0x0037u)&0x01u)!=0u)?0x90u:0u;
}

/* $D370..$D3DB selection and homing state machine. It runs on the even
   12.5-ms branch; physical step consumption remains the 6.25-ms service. */
static void bua_iac_shutdown_homing_even_step111(void)
{
    bua_u8 iac_mode;
    bua_u8 position;
    bua_u8 command;
    bua_u8 reset_requested;

    iac_mode=RAM8(0x00F3u);
    reset_requested=(bua_u8)((iac_mode&STEP111_IAC_CLOSE_PHASE_BIT)!=0u);
    if(reset_requested==0u) {
        if((RAM8(0x0035u)&0x20u)!=0u &&
           (RAM8(0x003Cu)&0x01u)==0u && RAM8(0x0057u)>=80u)
            reset_requested=1u;
        else if((RAM8(0x0002u)&STEP111_SKIP_IAC_RESET_BIT)==0u &&
                (RAM8(0x0033u)&STEP111_IGNITION_OFF_BIT)!=0u)
            reset_requested=1u;
    }

    if(reset_requested==0u) {
        if((RAM8(0x0034u)&STEP111_ENGINE_RUNNING_BIT)==0u) {
            RAM8(0x0105u)=0u;
            RAM8(0x00F2u)=(bua_u8)(RAM8(0x00F2u)&0xEFu);
        }
        return;
    }

    ++stats.iac_shutdown_homing_calls;
    position=RAM8(0x002Cu);
    if((iac_mode&STEP111_IAC_OPEN_PHASE_BIT)!=0u) {
        /* SUBA LC62F; NEGA; BPL in the listing.  An unexpected position
           above park therefore saturates to +127 rather than stopping. */
        command=(position<=STEP111_IAC_PARK_POSITION)?
                (bua_u8)(STEP111_IAC_PARK_POSITION-position):127u;
    } else if((iac_mode&STEP111_IAC_CLOSE_PHASE_BIT)!=0u) {
        if(position!=0u)
            command=0xFFu;
        else {
            iac_mode=(bua_u8)(iac_mode|STEP111_IAC_OPEN_PHASE_BIT);
            command=STEP111_IAC_PARK_POSITION;
        }
    } else {
        iac_mode=(bua_u8)(iac_mode|STEP111_IAC_CLOSE_PHASE_BIT);
        RAM8(0x002Cu)=0xFFu;
        command=0xFFu;
    }
    RAM8(0x0101u)=command;

    if(command==0u) {
        RAM8(0x0002u)=(bua_u8)(RAM8(0x0002u)|STEP111_SKIP_IAC_RESET_BIT);
        if((RAM8(0x0035u)&0x20u)!=0u)
            RAM8(0x003Cu)=(bua_u8)(RAM8(0x003Cu)|0x01u);
        if((RAM8(0x0033u)&STEP111_IGNITION_OFF_BIT)==0u) {
            bua_iac_restart_ld335_step111();
            return;
        }
    }
    RAM8(0x00F3u)=iac_mode;
    RAM8(0x0105u)=0u;
    RAM8(0x00F2u)=(bua_u8)(RAM8(0x00F2u)&0xEFu);
}
