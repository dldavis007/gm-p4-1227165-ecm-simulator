/* Step 115: listing-backed 8192-baud SCI/message core, $C9F4 and $FA58..$FC71. */
#define SCI115_DEVICE_ID 0x80u
#define SCI115_MAX_FRAME 80u
#define SCI115_OK 0u
#define SCI115_ERR_DEVICE 1u
#define SCI115_ERR_LENGTH 2u
#define SCI115_ERR_CHECKSUM 3u
#define SCI115_ERR_ADDRESS 4u

typedef struct BuaSci115Tag {
    bua_u8 input[SCI115_MAX_FRAME];
    bua_u8 output[SCI115_MAX_FRAME];
    bua_u8 input_count;
    bua_u8 output_count;
    bua_u8 status;
    bua_u8 mode;
    bua_u8 rom_read_boundary;
} BuaSci115;
static BuaSci115 bua_sci115;

static void bua_sci115_init(void)
{
    memset(&bua_sci115,0,sizeof(bua_sci115));
    RAM8(0x012Du)=0u;
    RAM8(0x012Fu)=0u;
    RAM8(0x0130u)=0u;
    ram16be_set(0x0131u,0u);
    RAM8(0x0133u)=0u;
    RAM8(0x0150u)=0u;
    RAM8(0x016Fu)=0u;
    mem.io4000[7]=0x27u;
    mpu16be_set(0x3FFCu,(bua_u16)(mpu16be_get(0x3FFCu)|0x0004u));
    SERIAL_MODE_WORD=(bua_u8)(SERIAL_MODE_WORD|0x04u);
}

static const bua_u16 sci115_mode1_addresses[63] = {
    0xC000u,0xC001u,0x0005u,0x0006u,0x0007u,0x0008u,0x0009u,
    0x005Du,0x005Fu,0x0081u,0x0057u,0x0095u,0x0096u,0x0065u,
    0x0066u,0x008Fu,0x006Fu,0x00F1u,0x00C6u,0x00C0u,0x00BFu,
    0x00C9u,0x002Cu,0x0101u,0x0102u,0x0063u,0x0064u,0x0061u,
    0x0060u,0x012Bu,0x0112u,0x0113u,0x00F4u,0x007Eu,0x007Fu,
    0x00EAu,0x00EBu,0x00B6u,0x0115u,0x0116u,0x009Du,0x009Eu,
    0x00A1u,0x00A5u,0x00D5u,0x00D6u,0x00CEu,0x00CFu,0x011Au,
    0x011Bu,0x011Eu,0x001Au,0x001Bu,0x0035u,0x0037u,0x002Eu,
    0x0034u,0x0001u,0x0004u,0x003Cu,0x003Eu,0x0046u,0x0044u
};

static bua_u8 bua_sci115_read(bua_u16 address,bua_u8 *valid)
{
    *valid=1u;
    if(address<0x0500u)
        return RAM8(address);
    if(address==0xC000u)
        return 0x25u;
    if(address==0xC001u)
        return 0xE5u;
    *valid=0u;
    bua_sci115.rom_read_boundary=1u;
    return 0u;
}

static bua_u8 bua_sci115_length_valid(bua_u8 mode,bua_u8 length)
{
    if(mode<=1u)
        return (bua_u8)(length==1u);
    if(mode==2u)
        return (bua_u8)(length==3u);
    if(mode==3u)
        return (bua_u8)(length>=3u && length<=17u && (length&1u)!=0u);
    if(mode==4u)
        return (bua_u8)(length>=13u && length<=27u && (length&1u)!=0u);
    return 0u;
}

static void bua_sci115_finish_output(bua_u8 data_count)
{
    bua_u8 i;
    bua_u8 sum=0u;
    bua_sci115.output[0]=SCI115_DEVICE_ID;
    bua_sci115.output[1]=(bua_u8)(data_count+0x55u);
    for(i=0u;i<(bua_u8)(data_count+2u);++i)
        sum=(bua_u8)(sum+bua_sci115.output[i]);
    bua_sci115.output[(bua_u8)(data_count+2u)]=(bua_u8)(0u-sum);
    bua_sci115.output_count=(bua_u8)(data_count+3u);
    RAM8(0x012Fu)=0u;
    RAM8(0x0130u)=0u;
    ram16be_set(0x0131u,0u);
    RAM8(0x012Du)=(bua_u8)(RAM8(0x012Du)&0x70u);
    mem.io4000[7]=0x41u;
}

static bua_u8 bua_sci115_make_response(bua_u8 mode,bua_u8 length)
{
    bua_u8 i;
    bua_u8 n;
    bua_u8 valid;
    bua_u16 address;
    bua_sci115.output[2]=mode;
    if(mode==0u) {
        SERIAL_MODE_WORD=(bua_u8)(SERIAL_MODE_WORD&0xF3u);
        MINOR_MODE_WORD2=(bua_u8)(MINOR_MODE_WORD2&0xF7u);
        bua_sci115_finish_output(1u);
        return SCI115_OK;
    }
    if(mode==1u) {
        for(i=0u;i<63u;++i) {
            bua_sci115.output[(bua_u8)(3u+i)]=
                bua_sci115_read(sci115_mode1_addresses[i],&valid);
            if(valid==0u)
                return SCI115_ERR_ADDRESS;
        }
        bua_sci115_finish_output(64u);
        return SCI115_OK;
    }
    if(mode==2u) {
        address=(bua_u16)(((bua_u16)bua_sci115.input[3]<<8)|
                          bua_sci115.input[4]);
        for(i=0u;i<63u;++i) {
            bua_sci115.output[(bua_u8)(3u+i)]=bua_sci115_read(address,&valid);
            if(valid==0u)
                return SCI115_ERR_ADDRESS;
            address=(bua_u16)(address+1u);
        }
        bua_sci115_finish_output(64u);
        return SCI115_OK;
    }
    n=(bua_u8)((length-1u)>>1);
    if(mode==4u) {
        n=(bua_u8)((length-11u)>>1);
        for(i=0u;i<10u;++i)
            RAM8((bua_u16)(0x0152u+i))=bua_sci115.input[(bua_u8)(3u+i)];
        SERIAL_MODE_WORD=(bua_u8)(SERIAL_MODE_WORD|0x08u);
        MINOR_MODE_WORD2=(bua_u8)(MINOR_MODE_WORD2|0x08u);
    }
    for(i=0u;i<n;++i) {
        bua_u8 p=(bua_u8)(3u+(mode==4u?10u:0u)+(bua_u8)(i*2u));
        address=(bua_u16)(((bua_u16)bua_sci115.input[p]<<8)|
                          bua_sci115.input[(bua_u8)(p+1u)]);
        bua_sci115.output[(bua_u8)(3u+i)]=bua_sci115_read(address,&valid);
        if(valid==0u)
            return SCI115_ERR_ADDRESS;
    }
    bua_sci115_finish_output((bua_u8)(n+1u));
    return SCI115_OK;
}

static bua_u8 bua_sci115_receive(const bua_u8 *frame,bua_u8 count)
{
    bua_u8 i;
    bua_u8 sum=0u;
    bua_u8 length;
    bua_u8 mode;
    memset(&bua_sci115,0,sizeof(bua_sci115));
    if(count<4u || count>SCI115_MAX_FRAME)
        return bua_sci115.status=SCI115_ERR_LENGTH;
    for(i=0u;i<count;++i) {
        bua_sci115.input[i]=frame[i];
        sum=(bua_u8)(sum+frame[i]);
    }
    bua_sci115.input_count=count;
    RAM8(0x0130u)=sum;
    if(frame[0]!=SCI115_DEVICE_ID)
        return bua_sci115.status=SCI115_ERR_DEVICE;
    length=(bua_u8)(frame[1]-0x55u);
    if(count!=(bua_u8)(length+3u))
        return bua_sci115.status=SCI115_ERR_LENGTH;
    mode=frame[2];
    if(bua_sci115_length_valid(mode,length)==0u)
        return bua_sci115.status=SCI115_ERR_LENGTH;
    if(sum!=0u)
        return bua_sci115.status=SCI115_ERR_CHECKSUM;
    bua_sci115.mode=mode;
    ram16be_set(0x0131u,(bua_u16)(0xC73Du+(bua_u16)(mode*2u)));
    RAM8(0x012Du)=0xD0u;
    RAM8(0x0133u)=0x40u;
    for(i=0u;i<length;++i)
        RAM8((bua_u16)(0x0134u+i))=frame[(bua_u8)(2u+i)];
    for(i=0u;i<length;++i)
        RAM8((bua_u16)(0x0151u+i))=RAM8((bua_u16)(0x0134u+i));
    RAM8(0x0171u)=0u;
    mem.io4000[4]=(bua_u8)(mem.io4000[4]|0x08u);
    mem.io4000[7]=0x89u;
    bua_sci115.status=bua_sci115_make_response(mode,length);
    return bua_sci115.status;
}

static void bua_sci115_build_request(bua_u8 *frame,const bua_u8 *data,
                                     bua_u8 length)
{
    bua_u8 i;
    bua_u8 sum;
    frame[0]=SCI115_DEVICE_ID;
    frame[1]=(bua_u8)(length+0x55u);
    sum=(bua_u8)(frame[0]+frame[1]);
    for(i=0u;i<length;++i) {
        frame[(bua_u8)(2u+i)]=data[i];
        sum=(bua_u8)(sum+data[i]);
    }
    frame[(bua_u8)(length+2u)]=(bua_u8)(0u-sum);
}
