/* ------------------------------------------------------------------------- */
/* Step 109: complete source-ordered Segment-D scheduler implementation.      */
/* ------------------------------------------------------------------------- */
static void segD_diagnostics(void)
{
    bua_u8 route;

    ++stats.diagnostic_segment_calls;
    route=bua_diag_front_step107();
    if(route==DIAG107_ROUTE_QUALIFY_LOG) {
        bua_diag_qualification_step108();
        route=bua_diag_logger_step107();
    } else if(route==DIAG107_ROUTE_LOG)
        route=bua_diag_logger_step107();

    if(route==DIAG107_ROUTE_FLASH_PENDING)
        bua_diag_flash_step109();
}
