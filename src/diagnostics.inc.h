/* ------------------------------------------------------------------------- */
/* Steps 107/109: Segment-D front end and diagnostic logging/debounce stage. */
/*                                                                           */
/* This file translates the listing blocks $EF04..$EFFA and $EFFE..$F10D.    */
/* Step 109 adds the engine-running field-service path at $EF40..$EF90.       */
/* LE4F7 qualification and LF10E flash sequencing remain separate helpers.    */
/* ------------------------------------------------------------------------- */
#define DIAG107_ROUTE_LOG             1u
#define DIAG107_ROUTE_QUALIFY_LOG     2u
#define DIAG107_ROUTE_FLASH_PENDING   4u
#define DIAG107_ROUTE_COMPLETE        5u

#define DIAG107_MASK_ERR1 0xF7u       /* LC1D0 */
#define DIAG107_MASK_ERR2 0xCEu       /* LC1D1 */
#define DIAG107_MASK_ERR3 0xFFu       /* LC1D2 */
#define DIAG107_MASK_ERR4 0xE0u       /* LC1D3 */
#define DIAG107_MASK_ERR5 0x00u       /* LC1D4 */

#define DIAG107_NO_ERROR_LIMIT  50u   /* LC1D5 */
#define DIAG107_SHORT_NO_ERR    10u   /* LC1D6 */
#define DIAG107_SHORT_HAS_ERR   50u   /* LC1D7 */
#define DIAG107_LONG_NO_ERR    100u   /* LC1D8 */
#define DIAG107_LONG_HAS_ERR   120u   /* LC1D9 */

static void bua_diag_checksum_step107(void)
{
    bua_u16 sum;
    bua_u8 i;
    sum=1u;
    for(i=0u;i<5u;++i)
        sum=(bua_u16)(sum+(bua_u16)RAM8((bua_u16)(0x0005u+i)));
    ram16be_set(0x0018u,sum);
}

/* LF498 and LF4A6 expressed at the raw MPU boundary.  The names follow the
   listing comments; electrical lamp polarity remains a HAL concern. */
static void bua_diag_lamp_on_step107(void)
{
    bua_u16 word;
    bua_u8 hi,lo;
    word=mpu16be_get(0x3FFCu);
    hi=(bua_u8)(word>>8);
    lo=(bua_u8)word;
    lo=(bua_u8)((lo&0xF7u)&0xFEu);
    lo=(bua_u8)(lo|0x02u);
    hi=(bua_u8)(hi|0xFBu);
    mpu16be_set(0x3FFCu,(bua_u16)(((bua_u16)hi<<8)|lo));
}

static void bua_diag_lamp_off_step107(void)
{
    bua_u16 word;
    bua_u8 hi,lo;
    word=mpu16be_get(0x3FFCu);
    hi=(bua_u8)(word>>8);
    lo=(bua_u8)word;
    lo=(bua_u8)((lo|0x08u)&0xFEu);
    lo=(bua_u8)(lo|0x02u);
    hi=(bua_u8)(hi|0xFBu);
    mpu16be_set(0x3FFCu,(bua_u16)(((bua_u16)hi<<8)|lo));
}

/* LF0FF..LF10D. */
static void bua_diag_common_exit_step107(void)
{
    RAM8(0x003Fu)=(bua_u8)(RAM8(0x003Fu)&0xE7u);
    RAM8(0x004Cu)=0u;
    RAM8(0x004Du)=0u;
    RAM8(0x004Eu)=0u;
    RAM8(0x004Fu)=0u;
    RAM8(0x0050u)=0u;
}

/* $EF40..$EF90: engine-running field-service lamp selection. */
static void bua_diag_field_running_step109(void)
{
    bua_u8 field_word;
    bua_u8 cadence_word;
    bua_u8 display_word;
    bua_u8 use_mpu_lamp;
    bua_u8 use_diag_lamp;

    field_word=RAM8(0x0044u);
    cadence_word=RAM8(0x003Fu);
    display_word=RAM8(0x003Cu);
    use_mpu_lamp=0u;
    use_diag_lamp=0u;

    if((field_word&0x80u)==0u) {
        display_word=(bua_u8)((display_word&0xF7u)^0x40u);
        RAM8(0x003Cu)=display_word;
        if((display_word&0x40u)==0u)
            use_mpu_lamp=1u;
    } else if((display_word&0x08u)==0u) {
        if((cadence_word&0x08u)==0u) {
            display_word=(bua_u8)((display_word&0xF7u)^0x40u);
            RAM8(0x003Cu)=display_word;
            if((display_word&0x40u)==0u)
                use_mpu_lamp=1u;
        } else {
            display_word=(bua_u8)(display_word|0x08u);
            RAM8(0x003Cu)=display_word;
            use_diag_lamp=1u;
        }
    } else if((cadence_word&0x08u)!=0u) {
        if((display_word&0x10u)!=0u) {
            display_word=(bua_u8)(display_word&0xEFu);
            RAM8(0x003Cu)=display_word;
            use_mpu_lamp=1u;
        } else
            use_diag_lamp=1u;
    }

    if(use_mpu_lamp!=0u) {
        if((mpu16be_get(0x3FFCu)&0x0008u)!=0u)
            bua_diag_lamp_on_step107();
        else
            bua_diag_lamp_off_step107();
    } else if(use_diag_lamp!=0u) {
        if((field_word&0x40u)!=0u)
            bua_diag_lamp_on_step107();
        else
            bua_diag_lamp_off_step107();
    }

    RAM8(0x0042u)=0u;
    RAM8(0x003Cu)=(bua_u8)(RAM8(0x003Cu)|0x04u);
}

/* $EF04..$EFFA.  The return value identifies the literal next listing block. */
static bua_u8 bua_diag_front_step107(void)
{
    bua_u8 a,b;
    bua_u8 i;

    b=(bua_u8)(RAM8(0x0000u)&0xF0u);
    a=RAM8(0x003Fu);
    if(b==0u) {
        a=(bua_u8)(a|0x08u);
        if((RAM8(0x001Bu)&0x01u)!=0u)
            a=(bua_u8)(a|0x10u);
    }
    RAM8(0x003Fu)=a;

    b=RAM8(0x0041u);
    if((RAM8(0x0035u)&0x10u)!=0u) {
        RAM8(0x0041u)=(bua_u8)(b|0x80u);
        RAM8(0x00B1u)=0u;
        if((RAM8(0x0034u)&0x80u)==0u) {
            RAM8(0x003Cu)=(bua_u8)(RAM8(0x003Cu)&0xE3u);
            return DIAG107_ROUTE_LOG;
        }
        bua_diag_field_running_step109();
        goto running_diagnostics;
    }

    RAM8(0x0041u)=(bua_u8)(b&0x7Fu);
    RAM8(0x003Cu)=(bua_u8)(RAM8(0x003Cu)&0xE3u);
    RAM8(0x0042u)=0u;

    a=(bua_u8)(RAM8(0x004Eu)&RAM8(0x0053u)&0x01u);
    if(a!=0u) {
        RAM8(0x0007u)=(bua_u8)(RAM8(0x0007u)|a);
        bua_diag_checksum_step107();
        bua_diag_lamp_on_step107();
        bua_diag_common_exit_step107();
        return DIAG107_ROUTE_COMPLETE;
    }

    if((RAM8(0x0034u)&0x80u)==0u) {
        static const bua_u8 timer_addr[10]={
            0xA7u,0xABu,0xA8u,0xA9u,0xAAu,
            0xB5u,0xACu,0xADu,0xAEu,0xAFu
        };
        for(i=0u;i<10u;++i)
            RAM8(timer_addr[i])=0u;
        if((RAM8(0x0033u)&0x10u)!=0u)
            RAM8(0x00B0u)=0u;
        return DIAG107_ROUTE_LOG;
    }

running_diagnostics:
    if((RAM8(0x003Fu)&0x02u)==0u) {
        RAM8(0x003Fu)=(bua_u8)(RAM8(0x003Fu)|0x02u);
        RAM8(0x0017u)=(bua_u8)(RAM8(0x0017u)+1u);
        if(RAM8(0x0017u)>DIAG107_NO_ERROR_LIMIT) {
            for(i=0u;i<5u;++i)
                RAM8((bua_u16)(0x0005u+i))=0u;
            RAM8(0x0017u)=0u;
            RAM8(0x0042u)=0u;
            bua_diag_checksum_step107();
        }
    }
    return DIAG107_ROUTE_QUALIFY_LOG;
}

/* $EFFE..$F10D.  This consumes the new-error bytes only after qualification. */
static bua_u8 bua_diag_logger_step107(void)
{
    static const bua_u8 masks[5]={
        DIAG107_MASK_ERR1,DIAG107_MASK_ERR2,DIAG107_MASK_ERR3,
        DIAG107_MASK_ERR4,DIAG107_MASK_ERR5
    };
    bua_u8 a,b;
    bua_u8 i;
    bua_u8 active;
    bua_u8 threshold;

    if((RAM8(0x003Cu)&0x04u)!=0u) {
        bua_diag_common_exit_step107();
        return DIAG107_ROUTE_COMPLETE;
    }
    if((RAM8(0x0041u)&0x80u)!=0u)
        return DIAG107_ROUTE_FLASH_PENDING;
    if((RAM8(0x0033u)&0x10u)!=0u) {
        bua_diag_common_exit_step107();
        return DIAG107_ROUTE_COMPLETE;
    }

    active=0u;
    for(i=0u;i<5u;++i)
        active=(bua_u8)(active|RAM8((bua_u16)(0x0051u+i)));

    if(active==0u) {
        RAM8(0x0051u)=(bua_u8)(RAM8(0x004Cu)&DIAG107_MASK_ERR1);
        RAM8(0x0052u)=(bua_u8)(RAM8(0x004Du)&DIAG107_MASK_ERR2);
        RAM8(0x0053u)=(bua_u8)(RAM8(0x004Eu)&DIAG107_MASK_ERR3);
        RAM8(0x0054u)=(bua_u8)(RAM8(0x004Fu)&DIAG107_MASK_ERR4);
        RAM8(0x0055u)=(bua_u8)(RAM8(0x0050u)&DIAG107_MASK_ERR5);
        if(RAM8(0x00B1u)!=0u)
            RAM8(0x00B1u)=(bua_u8)(RAM8(0x00B1u)-1u);
        else {
            RAM8(0x0041u)=(bua_u8)(RAM8(0x0041u)&0xFEu);
            RAM8(0x003Bu)=(bua_u8)(RAM8(0x003Bu)&0xFBu);
        }
    } else {
        active=0u;
        for(i=0u;i<5u;++i) {
            b=(bua_u8)(masks[i]&RAM8((bua_u16)(0x004Cu+i))&
                       RAM8((bua_u16)(0x0051u+i)));
            RAM8((bua_u16)(0x0051u+i))=b;
            if(b!=0u)
                active=(bua_u8)(active+1u);
        }
        RAM8(0x00B1u)=(bua_u8)(RAM8(0x00B1u)+1u);
        if(active==0u) {
            threshold=((RAM8(0x0041u)&0x01u)!=0u)?
                      DIAG107_LONG_NO_ERR:DIAG107_SHORT_NO_ERR;
            if(RAM8(0x00B1u)>=threshold)
                RAM8(0x00B1u)=threshold;
        } else {
            threshold=((RAM8(0x0041u)&0x01u)!=0u)?
                      DIAG107_LONG_HAS_ERR:DIAG107_SHORT_HAS_ERR;
            if(RAM8(0x00B1u)>=threshold) {
                RAM8(0x00B1u)=DIAG107_LONG_NO_ERR;
                RAM8(0x0041u)=(bua_u8)(RAM8(0x0041u)|0x01u);
                for(i=0u;i<5u;++i) {
                    RAM8((bua_u16)(0x0005u+i))=
                        (bua_u8)(RAM8((bua_u16)(0x0005u+i))|
                                 RAM8((bua_u16)(0x0051u+i)));
                    RAM8((bua_u16)(0x0051u+i))=0u;
                }
                RAM8(0x0017u)=0u;
                bua_diag_checksum_step107();
                RAM8(0x003Bu)=(bua_u8)(RAM8(0x003Bu)|0x04u);
            }
        }
    }

    a=RAM8(0x0034u);
    if((a&0x80u)==0u) {
        if((a&0x02u)!=0u) {
            bua_diag_lamp_on_step107();
            bua_diag_common_exit_step107();
            return DIAG107_ROUTE_COMPLETE;
        }
        a=(bua_u8)(a|0x02u);
    } else {
        a=(bua_u8)(a&0xFDu);
    }
    RAM8(0x0034u)=a;

    if((RAM8(0x0035u)&0x08u)!=0u &&
       (RAM8(0x0152u)&0x01u)!=0u) {
        if((RAM8(0x0153u)&0x01u)!=0u)
            bua_diag_lamp_on_step107();
        else
            bua_diag_lamp_off_step107();
    } else if((RAM8(0x0041u)&0x01u)!=0u ||
              (RAM8(0x003Fu)&0x02u)==0u) {
        bua_diag_lamp_on_step107();
    } else {
        bua_diag_lamp_off_step107();
    }
    bua_diag_common_exit_step107();
    return DIAG107_ROUTE_COMPLETE;
}
