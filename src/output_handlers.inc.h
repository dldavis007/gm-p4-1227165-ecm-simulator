/* ---------------------------------------------------------------------- */
/* Step 89: source-shaped major-loop wiring for translated Segments 9/B/C. */
/*                                                                        */
/* Steps 81-83 proved these executable blocks in isolation.  Step 89      */
/* makes the real major-segment dispatcher operate directly on the        */
/* original RAM locations so their state evolves at the scheduler cadence */
/* instead of remaining test-only helpers.  No physical output polarity   */
/* is assigned here; L003E/L0112/L0113 remain raw ECM command state.       */
/* ---------------------------------------------------------------------- */
static void seg9_inj_air_management(void)
{
    BuaAir81 a;
    a.af43=RAM8(0x0043u);
    a.af44=RAM8(0x0044u);
    a.history3d=RAM8(0x003Du);
    a.air3e=RAM8(0x003Eu);
    a.drop_fc=RAM8(0x00FCu);
    a.open_fd=RAM8(0x00FDu);
    a.pe_fe=RAM8(0x00FEu);
    a.rpm_f6=RAM8(0x00F6u);
    a.rich_f7=ram16be_get(0x00F7u);
    a.lean_f9=ram16be_get(0x00F9u);
    a.old_load_fb=RAM8(0x00FBu);
    bua_air81(&a,RAM8(0x005Bu),RAM8(0x003Bu),RAM8(0x0034u),
              RAM8(0x0056u),RAM8(0x0063u),RAM8(0x0073u),RAM8(0x0065u));
    RAM8(0x0043u)=a.af43;
    RAM8(0x003Du)=a.history3d;
    RAM8(0x003Eu)=a.air3e;
    RAM8(0x00FCu)=a.drop_fc;
    RAM8(0x00FDu)=a.open_fd;
    RAM8(0x00FEu)=a.pe_fe;
    RAM8(0x00F6u)=a.rpm_f6;
    ram16be_set(0x00F7u,a.rich_f7);
    ram16be_set(0x00F9u,a.lean_f9);
    RAM8(0x00FBu)=a.old_load_fb;
}

static void segB_egr(void)
{
    BuaEgr82Result r;
    r=bua_egr82(RAM8(0x0037u),RAM8(0x0035u),RAM8(0x0060u),
                RAM8(0x0082u),RAM8(0x0043u),RAM8(0x0057u),
                RAM8(0x0063u),RAM8(0x005Bu),RAM8(0x0112u));
    RAM8(0x0112u)=r.pw112;
}

static void segC_canister_purge(void)
{
    BuaPurge83 c;
    c.runtime_latched=(bua_u8)((RAM8(0x0001u)&0x10u)!=0u);
    c.status3b=RAM8(0x003Bu);
    c.pw113=RAM8(0x0113u);
    c.min114=RAM8(0x0114u);
    c.requested=0u;
    bua_purge83(&c,RAM8(0x001Bu),RAM8(0x0035u),RAM8(0x005Bu),
                RAM8(0x0043u),RAM8(0x0044u),RAM8(0x0041u),0xB4u,
                RAM8(0x0067u),RAM8(0x0082u),ram16be_get(0x00EAu),
                RAM8(0x0063u));
    if(c.runtime_latched) RAM8(0x0001u)=(bua_u8)(RAM8(0x0001u)|0x10u);
    else RAM8(0x0001u)=(bua_u8)(RAM8(0x0001u)&0xEFu);
    RAM8(0x003Bu)=c.status3b;
    RAM8(0x0113u)=c.pw113;
    RAM8(0x0114u)=c.min114;
}

/* Step 128: make the explicit hardware abstraction boundary part of the
 * single-translation-unit build without changing existing call ordering. */
#include "hal_interface.inc.h"
