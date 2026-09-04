/* ------------------------------------------------------------------------- */
/* Step 108: LE4F7..LE75C diagnostic qualification, literal listing order.   */
/*                                                                           */
/* The semantic error names are secondary.  Comparisons below follow the     */
/* emitted unsigned branches, including places where source comments suggest  */
/* the opposite sense.  All counters retain eight/sixteen-bit wrap behavior. */
/* ------------------------------------------------------------------------- */
#define D108_ERR13_RUN_HALF_MIN  60u
#define D108_ERR13_O2_LOW        79u
#define D108_ERR13_O2_HIGH      124u
#define D108_ERR13_TIMER         30u
#define D108_ERR13_COOL_MIN     146u
#define D108_ERR13_TPS_MIN       13u

#define D108_ERR21_TPS_CLEAR    128u
#define D108_ERR21_TIMER         20u
#define D108_ERR21_AIR_CLEAR     12u

#define D108_ERR24_VSS_CLEAR      3u
#define D108_ERR24_TIMER          2u
#define D108_ERR24_LOAD_CLEAR    26u
#define D108_ERR24_TPS_CLEAR      5u
#define D108_ERR24_RPM_LOW       40u
#define D108_ERR24_RPM_HIGH     240u

#define D108_ERR32_LOAD_MAX     128u
#define D108_ERR32_COOL_MIN     173u
#define D108_ERR32_EGR_MIN      133u
#define D108_ERR32_TPS_MAX       77u
#define D108_ERR32_TPS_MIN       15u
#define D108_ERR32_LIMIT       2560u
#define D108_ERR32_INCREMENT      1u

#define D108_ERR33_PUMP_MIN     100u
#define D108_ERR33_AIR_MIN       45u
#define D108_ERR33_TIMER          8u
#define D108_ERR33_TPS_MAX       38u
#define D108_ERR33_RPM_MAX      128u

#define D108_ERR34_DIFF_MAX      55u
#define D108_ERR34_TIMER          2u
#define D108_ERR34_RPM_MIN       24u
#define D108_ERR34_TPS_MIN       16u
#define D108_ERR34_LOAD_MIN       0u
#define D108_ERR34_LOAD_MAX     130u

#define D108_ERR42_RPM_MIN        4u
#define D108_ERR44_O2_CLEAR      45u
#define D108_ERR44_TIMER         20u
#define D108_ERR45_O2_MIN       158u
#define D108_ERR45_TIMER         50u
#define D108_ERR45_TPS_MIN        5u

static void bua_diag_qualification_step108(void)
{
    bua_u8 a,b;
    bua_u16 x;
    bua_u8 err32_set;

    /* $E4F7..$E551: ERR13 O2 qualification. */
    b=RAM8(0x003Fu);
    if((b&0x24u)!=0u) {
        RAM8(0x00AAu)=0u;
    } else {
        if((RAM8(0x0041u)&0x40u)==0u) {
            b=(bua_u8)((ram16be_get(0x001Au)>>1)&0x00FFu);
            if(b<D108_ERR13_RUN_HALF_MIN) {
                RAM8(0x00AAu)=0u;
            } else {
                RAM8(0x0041u)=(bua_u8)(RAM8(0x0041u)|0x40u);
                b=RAM8(0x006Fu);
                if(b>D108_ERR13_O2_HIGH || b<=D108_ERR13_O2_LOW) {
                    RAM8(0x00AAu)=0u;
                } else if(RAM8(0x00AAu)>D108_ERR13_TIMER) {
                    RAM8(0x004Cu)=(bua_u8)(RAM8(0x004Cu)|0x40u);
                    RAM8(0x0041u)=(bua_u8)(RAM8(0x0041u)|0x10u);
                } else if((RAM8(0x003Fu)&0x10u)!=0u &&
                          RAM8(0x005Bu)>D108_ERR13_COOL_MIN) {
                    if(RAM8(0x0082u)>D108_ERR13_TPS_MIN)
                        RAM8(0x00AAu)=(bua_u8)(RAM8(0x00AAu)+1u);
                    else if(RAM8(0x00AAu)!=0u)
                        RAM8(0x00AAu)=(bua_u8)(RAM8(0x00AAu)-1u);
                }
            }
        } else {
            b=RAM8(0x006Fu);
            if(b>D108_ERR13_O2_HIGH || b<=D108_ERR13_O2_LOW) {
                RAM8(0x00AAu)=0u;
            } else if(RAM8(0x00AAu)>D108_ERR13_TIMER) {
                RAM8(0x004Cu)=(bua_u8)(RAM8(0x004Cu)|0x40u);
                RAM8(0x0041u)=(bua_u8)(RAM8(0x0041u)|0x10u);
            } else if((RAM8(0x003Fu)&0x10u)!=0u &&
                      RAM8(0x005Bu)>D108_ERR13_COOL_MIN) {
                if(RAM8(0x0082u)>D108_ERR13_TPS_MIN)
                    RAM8(0x00AAu)=(bua_u8)(RAM8(0x00AAu)+1u);
                else if(RAM8(0x00AAu)!=0u)
                    RAM8(0x00AAu)=(bua_u8)(RAM8(0x00AAu)-1u);
            }
        }
    }

    /* $E551..$E57D: ERR21 high-TPS qualification. */
    b=RAM8(0x003Fu);
    if((b&0xC0u)==0u) {
        a=RAM8(0x0081u);
        if(a<=D108_ERR21_TPS_CLEAR) {
            b=(bua_u8)(b&0xFBu);
            RAM8(0x00A7u)=0u;
        } else if(RAM8(0x00A7u)>D108_ERR21_TIMER) {
            RAM8(0x004Cu)=(bua_u8)(RAM8(0x004Cu)|0x04u);
            b=(bua_u8)(b|0x04u);
        } else if(RAM8(0x00B6u)>=D108_ERR21_AIR_CLEAR) {
            RAM8(0x00A7u)=0u;
        } else {
            RAM8(0x00A7u)=(bua_u8)(RAM8(0x00A7u)+1u);
        }
    }
    RAM8(0x003Fu)=b;

    /* $E57F..$E5C7: ERR24 VSS qualification. */
    if((b&0xE4u)!=0u || RAM8(0x0065u)>D108_ERR24_VSS_CLEAR) {
        RAM8(0x00ABu)=0u;
    } else if(RAM8(0x00ABu)>D108_ERR24_TIMER) {
        RAM8(0x004Du)=(bua_u8)(RAM8(0x004Du)|0x80u);
        RAM8(0x0044u)=(bua_u8)(RAM8(0x0044u)|0x10u);
    } else if(RAM8(0x0063u)>=D108_ERR24_LOAD_CLEAR ||
              RAM8(0x0082u)>=D108_ERR24_TPS_CLEAR ||
              (RAM8(0x0037u)&0x01u)!=0u ||
              RAM8(0x0056u)<=D108_ERR24_RPM_LOW ||
              RAM8(0x0056u)>D108_ERR24_RPM_HIGH) {
        RAM8(0x00ABu)=0u;
    } else if((b&0x08u)!=0u) {
        RAM8(0x00ABu)=(bua_u8)(RAM8(0x00ABu)+1u);
    }

    /* $E5C7..$E61F: ERR32 EGR qualification. */
    b=RAM8(0x0041u);
    err32_set=0u;
    x=ram16be_get(0x00AEu);
    if((RAM8(0x002Eu)&0x20u)==0u) {
        if((b&0x20u)!=0u)
            err32_set=1u;
        else
            x=0u;
    } else {
        b=(bua_u8)(b&0xDFu);
        RAM8(0x0041u)=b;
        if((RAM8(0x003Fu)&0xE4u)!=0u) {
            x=0u;
        } else if(x>D108_ERR32_LIMIT) {
            err32_set=1u;
        } else if(RAM8(0x005Bu)<D108_ERR32_COOL_MIN) {
            x=0u;
        } else if(RAM8(0x0063u)>=D108_ERR32_LOAD_MAX ||
                  RAM8(0x0112u)<=D108_ERR32_EGR_MIN ||
                  RAM8(0x0082u)>D108_ERR32_TPS_MAX ||
                  RAM8(0x0082u)<=D108_ERR32_TPS_MIN) {
            if(x<=1u) x=0u;
            else x=(bua_u16)(x-2u);
        } else {
            x=(bua_u16)(x+D108_ERR32_INCREMENT);
        }
    }
    if(err32_set!=0u)
        RAM8(0x004Du)=(bua_u8)(RAM8(0x004Du)|0x08u);
    else
        ram16be_set(0x00AEu,x);

    /* $E61F..$E65F: ERR33 high-MAF qualification. */
    b=RAM8(0x003Fu);
    if((b&0x80u)!=0u) {
        RAM8(0x004Du)=(bua_u8)(RAM8(0x004Du)|0x04u);
        b=(bua_u8)(b|0x80u);
    } else if((b&0x24u)!=0u) {
        RAM8(0x00B0u)=0u;
    } else if(RAM8(0x007Fu)<=D108_ERR33_PUMP_MIN) {
        RAM8(0x00B0u)=0u;
    } else if((RAM8(0x0040u)&0x20u)==0u) {
        if(RAM8(0x00B6u)<=D108_ERR33_AIR_MIN) {
            RAM8(0x00B0u)=0u;
        } else if(RAM8(0x00B0u)>D108_ERR33_TIMER) {
            RAM8(0x004Du)=(bua_u8)(RAM8(0x004Du)|0x04u);
            b=(bua_u8)(b|0x80u);
        } else if(RAM8(0x0082u)>=D108_ERR33_TPS_MAX ||
                  RAM8(0x0056u)>D108_ERR33_RPM_MAX) {
            RAM8(0x00B0u)=0u;
        } else {
            RAM8(0x00B0u)=(bua_u8)(RAM8(0x00B0u)+1u);
        }
    }
    RAM8(0x003Fu)=b;

    /* $E65F..$E6A1: ERR34 low-MAF qualification. */
    if((RAM8(0x003Fu)&0x40u)!=0u) {
        RAM8(0x004Du)=(bua_u8)(RAM8(0x004Du)|0x02u);
        RAM8(0x003Fu)=(bua_u8)(RAM8(0x003Fu)|0x40u);
    } else {
        x=ram16be_get(0x00EFu);
        if(x>D108_ERR34_DIFF_MAX) {
            RAM8(0x00B5u)=0u;
        } else {
            a=RAM8(0x00B5u);
            if(a>=D108_ERR34_TIMER) {
                RAM8(0x004Du)=(bua_u8)(RAM8(0x004Du)|0x02u);
                RAM8(0x003Fu)=(bua_u8)(RAM8(0x003Fu)|0x40u);
            } else if((RAM8(0x003Fu)&0xA4u)==0u &&
                      RAM8(0x0056u)>=D108_ERR34_RPM_MIN &&
                      RAM8(0x0082u)>=D108_ERR34_TPS_MIN &&
                      RAM8(0x0063u)>=D108_ERR34_LOAD_MIN &&
                      RAM8(0x0063u)<=D108_ERR34_LOAD_MAX) {
                RAM8(0x00B5u)=(bua_u8)(a+1u);
            }
        }
    }

    /* $E6A1..$E6AC: ERR41 consumes L0040 bit 0 and clears that pulse flag. */
    a=RAM8(0x0040u);
    if((a&0x01u)!=0u)
        RAM8(0x004Eu)=(bua_u8)(RAM8(0x004Eu)|0x40u);
    RAM8(0x0040u)=(bua_u8)(a&0xFEu);

    /* $E6AC..$E6DF: ERR42 spark-monitor two-pass state.  ASLA/BPL at $E6B8
       tests original L0034 bit 6; this intentionally follows flags, not the
       nearby engine-running comment. */
    if((RAM8(0x0036u)&0x01u)==0u &&
       (RAM8(0x0000u)&0x10u)==0u &&
       (RAM8(0x0034u)&0x40u)!=0u) {
        b=RAM8(0x003Fu);
        if(RAM8(0x0056u)>D108_ERR42_RPM_MIN && RAM8(0x00B4u)==0u) {
            if((b&0x01u)==0u) {
                b=(bua_u8)(b|0x01u);
                RAM8(0x003Fu)=b;
            } else {
                RAM8(0x0001u)=(bua_u8)(RAM8(0x0001u)|0x80u);
            }
        } else {
            RAM8(0x003Fu)=(bua_u8)(b&0xFEu);
        }
        RAM8(0x00B4u)=0u;
    }

    /* $E6DF..$E6EC: ERR43 knock-failure sources. */
    if((RAM8(0x003Bu)&0x20u)!=0u || (RAM8(0x0002u)&0x80u)!=0u)
        RAM8(0x004Eu)=(bua_u8)(RAM8(0x004Eu)|0x10u);

    /* $E6EC..$E75C: ERR44 lean and ERR45 rich O2 qualification. */
    b=(bua_u8)(RAM8(0x0041u)&0xFDu);
    if((RAM8(0x003Fu)&0xC0u)==0u) {
        if(RAM8(0x0073u)>=D108_ERR44_O2_CLEAR) {
            RAM8(0x00A8u)=0u;
        } else if(RAM8(0x00A8u)>D108_ERR44_TIMER) {
            RAM8(0x004Eu)=(bua_u8)(RAM8(0x004Eu)|0x08u);
            b=(bua_u8)(b|0x02u);
        } else if((RAM8(0x0044u)&0x80u)==0u || (b&0x04u)!=0u) {
            RAM8(0x00A8u)=0u;
        } else if((RAM8(0x003Fu)&0x08u)!=0u) {
            RAM8(0x00A8u)=(bua_u8)(RAM8(0x00A8u)+1u);
        }

        if(RAM8(0x0073u)<=D108_ERR45_O2_MIN) {
            RAM8(0x00A9u)=0u;
        } else if(RAM8(0x00A9u)>D108_ERR45_TIMER) {
            RAM8(0x004Eu)=(bua_u8)(RAM8(0x004Eu)|0x04u);
            b=(bua_u8)(b|0x02u);
        } else if((RAM8(0x0044u)&0x80u)==0u || (b&0x04u)!=0u ||
                  RAM8(0x0082u)<=D108_ERR45_TPS_MIN) {
            RAM8(0x00A9u)=0u;
        } else if((RAM8(0x003Fu)&0x08u)!=0u) {
            RAM8(0x00A9u)=(bua_u8)(RAM8(0x00A9u)+1u);
        }
    }
    RAM8(0x0041u)=(bua_u8)(b&0xFBu);
}
