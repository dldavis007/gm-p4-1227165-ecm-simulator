/* Step 119: raw vector table and exceptional-entry boundaries, $FFF0..$FFFF. */
#define VECTOR119_FIRST_ADDR 0xFFF0u
#define VECTOR119_LAST_ADDR  0xFFFEu
#define VECTOR119_COUNT      8u
#define VECTOR119_OUTCOME_INVALID   0u
#define VECTOR119_OUTCOME_EXTERNAL  1u
#define VECTOR119_OUTCOME_IRQ       2u
#define VECTOR119_OUTCOME_RTI       3u
#define VECTOR119_OUTCOME_RESET     4u

static const bua_u16 bua_vector_table119[VECTOR119_COUNT] = {
    0x6000u,0xC9F4u,0xF27Bu,0x6000u,
    0xC800u,0xC800u,0xC800u,0xC800u
};

typedef struct BuaVectorTrace119Tag {
    bua_u32 dispatches;
    bua_u32 invalid_slots;
    bua_u32 external_6000_boundaries;
    bua_u32 irq_entries;
    bua_u32 immediate_rti_entries;
    bua_u32 reset_requests;
    bua_u32 unresolved_swi_boundaries;
    bua_u16 last_vector_address;
    bua_u16 last_target;
    bua_u16 last_swi_source;
    bua_u8 reset_requested;
    bua_u8 last_outcome;
} BuaVectorTrace119;
static BuaVectorTrace119 bua_vector_trace119;

/* Defined later by the scheduler fragment; $FFF2 points to its IRQ front. */
static void irq_6p25ms(void);

static void bua_vector_reset_step119(void)
{
    memset(&bua_vector_trace119,0,sizeof(bua_vector_trace119));
}

/*
 * SWI instructions are observable at their call sites. The listing comments
 * alone do not prove which raw vector slot the processor assigns to SWI, so
 * record the source without inventing a slot or a $6000 implementation.
 */
static void bua_vector_note_swi_step119(bua_u16 source_address)
{
    ++bua_vector_trace119.unresolved_swi_boundaries;
    bua_vector_trace119.last_swi_source=source_address;
}

static bua_u16 bua_vector_dispatch_step119(bua_u16 vector_address)
{
    bua_u16 index;
    bua_u16 target;
    ++bua_vector_trace119.dispatches;
    bua_vector_trace119.last_vector_address=vector_address;
    bua_vector_trace119.last_target=0u;
    bua_vector_trace119.last_outcome=VECTOR119_OUTCOME_INVALID;
    if(vector_address<VECTOR119_FIRST_ADDR ||
       vector_address>VECTOR119_LAST_ADDR ||
       (vector_address&1u)!=0u) {
        ++bua_vector_trace119.invalid_slots;
        return 0u;
    }
    index=(bua_u16)((vector_address-VECTOR119_FIRST_ADDR)>>1);
    target=bua_vector_table119[index];
    bua_vector_trace119.last_target=target;
    if(target==0x6000u) {
        bua_vector_trace119.last_outcome=VECTOR119_OUTCOME_EXTERNAL;
        ++bua_vector_trace119.external_6000_boundaries;
    } else if(target==0xC9F4u) {
        bua_vector_trace119.last_outcome=VECTOR119_OUTCOME_IRQ;
        ++bua_vector_trace119.irq_entries;
        irq_6p25ms();
    } else if(target==0xF27Bu) {
        /* The target consists solely of the emitted RTI opcode at $F27B. */
        bua_vector_trace119.last_outcome=VECTOR119_OUTCOME_RTI;
        ++bua_vector_trace119.immediate_rti_entries;
    } else if(target==0xC800u) {
        /* Request startup/reset; do not fabricate CPU/COP/clock semantics. */
        bua_vector_trace119.last_outcome=VECTOR119_OUTCOME_RESET;
        bua_vector_trace119.reset_requested=1u;
        ++bua_vector_trace119.reset_requests;
    }
    return target;
}

