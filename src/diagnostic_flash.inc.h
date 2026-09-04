/* ------------------------------------------------------------------------- */
/* Step 109: field-service malfunction-code flash sequencer, $F10E..$F1BB.   */
/*                                                                           */
/* The source emits code 12 first on every entry into a new flash sequence.  */
/* Each code is repeated three times; the stored-error scan then advances     */
/* through masked error words $05..$09.                                      */
/* ------------------------------------------------------------------------- */
static void bua_diag_flash_finish_step109(bua_u8 lamp_on)
{
    if(lamp_on!=0u)
        bua_diag_lamp_on_step107();
    else
        bua_diag_lamp_off_step107();
    bua_diag_common_exit_step107();
}

static void bua_diag_flash_digits_step109(bua_u8 code_index,bua_u8 control)
{
    bua_u8 first_digit;
    bua_u8 second_digit;

    first_digit=0u;
    second_digit=code_index;
    for(;;) {
        first_digit=(bua_u8)(first_digit+1u);
        if(second_digit<=6u)
            break;
        second_digit=(bua_u8)(second_digit-6u);
    }
    RAM8(0x0042u)=control;
    RAM8(0x00B9u)=first_digit;
    RAM8(0x00BAu)=second_digit;
    RAM8(0x00BDu)=0x1Cu;
    bua_diag_flash_finish_step109(0u);
}

static void bua_diag_flash_step109(void)
{
    bua_u8 a;
    bua_u8 b;
    bua_u8 carry_in;
    bua_u8 found;

    a=RAM8(0x0042u);
    if((a&0x40u)!=0u) {
        RAM8(0x00BDu)=(bua_u8)(RAM8(0x00BDu)-1u);
        if(RAM8(0x00BDu)!=0u) {
            bua_diag_common_exit_step107();
            return;
        }
        if((a&0x10u)!=0u) {
            RAM8(0x0042u)=(bua_u8)(a&0xEFu);
            RAM8(0x00BDu)=4u;
            bua_diag_flash_finish_step109(0u);
            return;
        }
        if((a&0x20u)==0u) {
            b=RAM8(0x00B9u);
            if(b==0u) {
                RAM8(0x0042u)=(bua_u8)(a|0x20u);
                RAM8(0x00BDu)=8u;
                bua_diag_flash_finish_step109(0u);
                return;
            }
            RAM8(0x00B9u)=(bua_u8)(b-1u);
            RAM8(0x0042u)=(bua_u8)(a|0x10u);
            RAM8(0x00BDu)=4u;
            bua_diag_flash_finish_step109(1u);
            return;
        }
        b=RAM8(0x00BAu);
        if(b!=0u) {
            RAM8(0x00BAu)=(bua_u8)(b-1u);
            RAM8(0x0042u)=(bua_u8)(a|0x10u);
            RAM8(0x00BDu)=4u;
            bua_diag_flash_finish_step109(1u);
            return;
        }

        b=RAM8(0x00BBu);
        if((a&0x03u)!=0u) {
            a=(bua_u8)((a-1u)&0xDFu);
            bua_diag_flash_digits_step109(b,a);
            return;
        }
        a=RAM8(0x00BCu);
    } else {
        b=2u;
        a=(bua_u8)((RAM8(0x0005u)&DIAG107_MASK_ERR1)<<1);
        RAM8(0x00BCu)=a;
        RAM8(0x00BBu)=b;
        bua_diag_flash_digits_step109(b,0x42u);
        return;
    }

    found=0u;
    while(found==0u) {
        b=(bua_u8)(b+1u);
        if(b>=0x25u) {
            RAM8(0x0042u)=0u;
            RAM8(0x00BDu)=b;
            bua_diag_flash_finish_step109(0u);
            return;
        }
        if(b==10u)
            a=(bua_u8)(RAM8(0x0006u)&DIAG107_MASK_ERR2);
        else if(b==18u)
            a=(bua_u8)(RAM8(0x0007u)&DIAG107_MASK_ERR3);
        else if(b==26u)
            a=(bua_u8)(RAM8(0x0008u)&DIAG107_MASK_ERR4);
        else if(b==34u)
            a=(bua_u8)(RAM8(0x0009u)&DIAG107_MASK_ERR5);
        found=(bua_u8)(a&0x80u);
        /* For non-group indices below 34, the final CMPB #$22 leaves
           6801 carry set (unsigned borrow); ROLA shifts that carry into b0.
           It cannot reach carry-out before the next group reload, but it is
           retained here because $BC is observable source RAM. */
        carry_in=0u;
        if(b<34u && b!=10u && b!=18u && b!=26u)
            carry_in=1u;
        a=(bua_u8)((a<<1)|carry_in);
    }
    RAM8(0x00BCu)=a;
    RAM8(0x00BBu)=b;
    bua_diag_flash_digits_step109(b,0x42u);
}
