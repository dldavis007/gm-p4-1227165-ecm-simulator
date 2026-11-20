typedef struct {
    bua_u8 cell_number;
    bua_u8 table_index_before;
    bua_u8 bit_count_before;
    bua_u8 xmit_before;
    bua_u8 table_index_after;
    bua_u8 bit_count_after;
    bua_u8 xmit_after;
    bua_u8 line_at_first;
    bua_u8 shifted_data_valid;
    bua_u8 shifted_data_bit;
} BuaSerialCellTrace;
/* ---------------------------------------------------------------------- */
/* Independent PC receiver.                                               */
/*                                                                        */
/* It does not inspect ALCL_XMIT_BYTE, ALCL_TABLE_INDEX, or bit counters. */
/* It receives only physical line transitions with P4 timer timestamps.   */
/*                                                                        */
/* The observed transmitter has one pulse-width-coded framing cell, then  */
/* eight pulse-width-coded data cells.  A short low pulse is data 0 and   */
/* a long low pulse is data 1.  The first pulse of each 9-pulse group is  */
/* treated only as framing and is not assigned a semantic meaning.        */
/* ---------------------------------------------------------------------- */
#define RX_WIDTH_THRESHOLD ((SERIAL_T_FIRST + SERIAL_T_SECOND) / 2u)
static void rx_reset(void)
{
    memset(&rx, 0, sizeof(rx));
}
static void rx_low_edge(bua_u32 time_count)
{
    if (rx_enabled == 0u)
        return;
    rx.have_low_start = 1u;
    rx.low_start_time = time_count;
}
static void rx_high_edge(bua_u32 time_count)
{
    bua_u32 width;
    bua_u8 bit;
    if (rx_enabled == 0u || rx.have_low_start == 0u)
        return;
    width = time_count - rx.low_start_time;
    rx.have_low_start = 0u;
    ++stats.receiver_pulses;
    if (!((width == SERIAL_T_FIRST) || (width == SERIAL_T_SECOND)))
        ++stats.receiver_bad_widths;
    if (rx.pulse_in_frame == 0u) {
        if (rx.byte_count < (bua_u8)(sizeof(rx.frame_widths) / sizeof(rx.frame_widths[0])))
            rx.frame_widths[rx.byte_count] = (bua_u16)width;
        rx.assembling = 0u;
        rx.pulse_in_frame = 1u;
        ++stats.receiver_frames;
        return;
    }
    bit = (width > (bua_u32)RX_WIDTH_THRESHOLD) ? 1u : 0u;
    rx.assembling = (bua_u8)((rx.assembling << 1) | bit);
    ++rx.pulse_in_frame;
    if (rx.pulse_in_frame == 9u) {
        if (rx.byte_count < (bua_u8)sizeof(rx.bytes))
            rx.bytes[rx.byte_count] = rx.assembling;
        ++rx.byte_count;
        ++stats.receiver_bytes;
        rx.pulse_in_frame = 0u;
    }
}
static void serial_line_low_at(bua_u32 time_count)
{
    if (sim_serial_line_high != 0u) {
        sim_serial_line_high = 0u;
        rx_low_edge(time_count);
    }
}
static void serial_line_high_at(bua_u32 time_count)
{
    if (sim_serial_line_high == 0u) {
        sim_serial_line_high = 1u;
        rx_high_edge(time_count);
    }
}
/* One complete 6.25-ms cell, corresponding to LCAA4 then LCA2C then LCA3B. */
static void bua_160_cell_at(BuaSerialCellTrace *trace, bua_u32 cell_start)
{
    int bit;
    if (trace != 0) {
        trace->table_index_before = ALCL_TABLE_INDEX;
        trace->bit_count_before = ALCL_BIT_COUNT;
        trace->xmit_before = ALCL_XMIT_BYTE;
        trace->shifted_data_valid = 0u;
        trace->shifted_data_bit = 0u;
    }
    /* LCAA4: first match +11; b0+b1 set; serial line goes low now. */
    SERIAL_MODE_WORD |= (SERIAL_FLAG_FIRST | SERIAL_FLAG_SECOND);
    serial_line_low_at(cell_start);
    /* LCA2C occurs at cell_start + 11 in this normalized trace. */
    SERIAL_MODE_WORD &= (bua_u8)~SERIAL_FLAG_FIRST;
    bit = bua_lf880_manager(cell_start + (bua_u32)SERIAL_T_FIRST);
    if (trace != 0) {
        trace->line_at_first = sim_serial_line_high;
        if (bit >= 0) {
            trace->shifted_data_valid = 1u;
            trace->shifted_data_bit = (bua_u8)bit;
        }
    }
    /* LCA3B: later match; line high regardless of data. */
    SERIAL_MODE_WORD &= (bua_u8)~SERIAL_FLAG_SECOND;
    serial_line_high_at(cell_start + (bua_u32)SERIAL_T_SECOND);
    ++stats.serial_cells;
    if (trace != 0 && trace->line_at_first != 0u)
        ++stats.serial_short_low_cells;
    else
        ++stats.serial_long_low_cells;
    if (trace != 0) {
        trace->table_index_after = ALCL_TABLE_INDEX;
        trace->bit_count_after = ALCL_BIT_COUNT;
        trace->xmit_after = ALCL_XMIT_BYTE;
    }
}
/* Step 170: cell-level raw HAL boundary for the complete LF880 manager. */
static void bua_hal_aldl160_init(void)
{
    ALCL_TABLE_INDEX=0u;
    ALCL_BIT_COUNT=0u;
    ALCL_XMIT_BYTE=0xFFu;
    SERIAL_MODE_WORD&=(bua_u8)~(SERIAL_FLAG_FIRST|SERIAL_FLAG_SECOND);
    sim_serial_line_high=1u;
}
static void bua_hal_aldl160_cell(BuaSerialCellTrace *trace,bua_u32 cell_start)
{
    bua_160_cell_at(trace,cell_start);
}
static bua_u8 bua_hal_aldl160_line_high(void)
{
    return sim_serial_line_high;
}
static bua_u8 bua_hal_aldl160_table_index(void)
{
    return ALCL_TABLE_INDEX;
}
static bua_u8 bua_hal_aldl160_bit_count(void)
{
    return ALCL_BIT_COUNT;
}
static bua_u8 bua_hal_aldl160_xmit_byte(void)
{
    return ALCL_XMIT_BYTE;
}
static void print_serial_trace(void)
{
    BuaSerialCellTrace tr;
    bua_u8 original_index;
    bua_u8 original_bits;
    bua_u8 original_xmit;
    bua_u8 original_serial_word;
    bua_u8 original_line;
    bua_u32 original_loaded;
    bua_u32 original_cells;
    bua_u32 original_short;
    bua_u32 original_long;
    bua_u32 original_calls;
    bua_u8 original_rx_enabled;
    unsigned int i;
    /* Preserve scheduler state; create a clean normal-stream observation. */
    original_index = ALCL_TABLE_INDEX;
    original_bits = ALCL_BIT_COUNT;
    original_xmit = ALCL_XMIT_BYTE;
    original_serial_word = SERIAL_MODE_WORD;
    original_line = sim_serial_line_high;
    original_loaded = stats.display_bytes_loaded;
    original_cells = stats.serial_cells;
    original_short = stats.serial_short_low_cells;
    original_long = stats.serial_long_low_cells;
    original_calls = stats.serial_manager_calls;
    original_rx_enabled = rx_enabled;
    ALCL_TABLE_INDEX = 0u;
    ALCL_BIT_COUNT = 0u;
    ALCL_XMIT_BYTE = 0xFFu;
    SERIAL_MODE_WORD &= (bua_u8)~0x03u;
    sim_serial_line_high = 1u;
    rx_enabled = 0u;
    printf("160-baud normal-path waveform trace (P4 timer counts):\n");
    printf("  Each cell: line LOW at +0, LF880 at +%u, forced HIGH at +%u, next cell +%u\n",
           (unsigned int)SERIAL_T_FIRST,
           (unsigned int)SERIAL_T_SECOND,
           (unsigned int)SERIAL_T_CELL);
    printf("  A data 0 raises the line at +11; a data 1 stays low until +143.\n\n");
    printf(" cell  idx/bits before  xmit  action        low-until  idx/bits after xmit\n");
    /* Five loaded bytes use 45 cells.  LF8CF then creates an $FF byte
     * with bit-count=8; trace its eight shifted bits and the following
     * load cell as well: 45 + 1 + 8 + 1 = 55 cells. */
    for (i = 0u; i < 55u; ++i) {
        memset(&tr, 0, sizeof(tr));
        tr.cell_number = (bua_u8)i;
        bua_160_cell_at(&tr, (bua_u32)i * (bua_u32)SERIAL_T_CELL);
        printf(" %3u    %u/%u           %02X    ",
               i,
               (unsigned int)tr.table_index_before,
               (unsigned int)tr.bit_count_before,
               (unsigned int)tr.xmit_before);
        if (tr.shifted_data_valid != 0u)
            printf("data bit %u     +%3u      ",
                   (unsigned int)tr.shifted_data_bit,
                   (unsigned int)(tr.shifted_data_bit ? SERIAL_T_SECOND : SERIAL_T_FIRST));
        else if (tr.table_index_before > DISPLAY_TABLE_LENGTH && tr.bit_count_before == 0u)
            printf("table reset    +%3u      ", (unsigned int)SERIAL_T_SECOND);
        else
            printf("load/framing   +%3u      ", (unsigned int)SERIAL_T_FIRST);
        printf("%u/%u          %02X\n",
               (unsigned int)tr.table_index_after,
               (unsigned int)tr.bit_count_after,
               (unsigned int)tr.xmit_after);
    }
    ALCL_TABLE_INDEX = original_index;
    ALCL_BIT_COUNT = original_bits;
    ALCL_XMIT_BYTE = original_xmit;
    SERIAL_MODE_WORD = original_serial_word;
    sim_serial_line_high = original_line;
    stats.display_bytes_loaded = original_loaded;
    stats.serial_cells = original_cells;
    stats.serial_short_low_cells = original_short;
    stats.serial_long_low_cells = original_long;
    stats.serial_manager_calls = original_calls;
    rx_enabled = original_rx_enabled;
}
static void print_receiver_round_trip(void)
{
    bua_u8 expected[6];
    bua_u8 saved_index;
    bua_u8 saved_bits;
    bua_u8 saved_xmit;
    bua_u8 saved_serial_word;
    bua_u8 saved_line;
    BuaStats saved_stats;
    unsigned int i;
    unsigned int pass_count;
    expected[0] = MINOR_MODE_WORD2;
    expected[1] = (bua_u8)CAL_NUM_CYL_CODE;
    expected[2] = RAM8(0x011Au);
    expected[3] = RAM8(0x011Eu);
    expected[4] = (bua_u8)CAL_GPH_SCALE_FACTOR;
    expected[5] = 0xFFu;
    saved_index = ALCL_TABLE_INDEX;
    saved_bits = ALCL_BIT_COUNT;
    saved_xmit = ALCL_XMIT_BYTE;
    saved_serial_word = SERIAL_MODE_WORD;
    saved_line = sim_serial_line_high;
    saved_stats = stats;
    ALCL_TABLE_INDEX = 0u;
    ALCL_BIT_COUNT = 0u;
    ALCL_XMIT_BYTE = 0xFFu;
    SERIAL_MODE_WORD &= (bua_u8)~0x03u;
    sim_serial_line_high = 1u;
    rx_reset();
    rx_enabled = 1u;
    /* Six complete 9-cell groups = five normal values plus literal $FF group. */
    for (i = 0u; i < 54u; ++i)
        bua_160_cell_at(0, (bua_u32)i * (bua_u32)SERIAL_T_CELL);
    rx_enabled = 0u;
    printf("Independent line receiver round-trip:\n");
    printf("  Receiver input: serial transitions + P4 timer timestamps only\n");
    printf("  Pulse threshold: <=%u => 0, >%u => 1; data assembled MSB-first\n",
           (unsigned int)RX_WIDTH_THRESHOLD,
           (unsigned int)RX_WIDTH_THRESHOLD);
    printf("\n  group  frame-low  decoded  expected  result\n");
    pass_count = 0u;
    for (i = 0u; i < 6u; ++i) {
        int pass;
        pass = (i < (unsigned int)rx.byte_count && rx.bytes[i] == expected[i]);
        if (pass)
            ++pass_count;
        printf("   %u      %3u       %02X       %02X     %s\n",
               i,
               (unsigned int)rx.frame_widths[i],
               (unsigned int)((i < (unsigned int)rx.byte_count) ? rx.bytes[i] : 0u),
               (unsigned int)expected[i],
               pass ? "PASS" : "FAIL");
    }
    printf("\n  pulses=%lu frames=%lu bytes=%lu badWidths=%lu\n",
           (unsigned long)(stats.receiver_pulses - saved_stats.receiver_pulses),
           (unsigned long)(stats.receiver_frames - saved_stats.receiver_frames),
           (unsigned long)(stats.receiver_bytes - saved_stats.receiver_bytes),
           (unsigned long)(stats.receiver_bad_widths - saved_stats.receiver_bad_widths));
    printf("  round-trip result: %s (%u/6 bytes match)\n",
           (pass_count == 6u && rx.byte_count == 6u) ? "PASS" : "FAIL",
           pass_count);
    ALCL_TABLE_INDEX = saved_index;
    ALCL_BIT_COUNT = saved_bits;
    ALCL_XMIT_BYTE = saved_xmit;
    SERIAL_MODE_WORD = saved_serial_word;
    sim_serial_line_high = saved_line;
    stats = saved_stats;
}
