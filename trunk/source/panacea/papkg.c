/*
 * PAPKG.C - routines that define the panacea package
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "panacea_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DEFCNT - define PANACEA controls */

void PA_defcnt(void)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_GENCMD - install PANACEA commands */

void PA_gencmd(void)
   {static int *none = NULL;

/* install the commands which control the generator */
    PA_inst_c("end",    none,     FALSE,     0, (PFVoid) PA_done,     PA_zargs);
    PA_inst_c("define", none,     FALSE,     0, (PFVoid) PA_defh,     PA_zargs);
    PA_inst_c("dump",   none,     FALSE,     0, (PFVoid) PA_wrrstrth, PA_zargs);

    PA_inst_c("read", _PA.input_line, SC_CHAR_I, 1, (PFVoid) PA_readh, PA_strarg);

/* install the commands for initial value specification */
    PA_inst_c("specify", none, FALSE, 0, (PFVoid) PA_specifyh, PA_zargs);
    PA_inst_c("s",       none, FALSE, 0, (PFVoid) PA_sh,       PA_zargs);

/* install the commands for plot requests */
    PA_inst_c("graph",      none, FALSE, 0, (PFVoid) PA_nploth,      PA_zargs);

/* install the commands which specify the runtime controls */
    PA_inst_c("package",    none, FALSE, 0, (PFVoid) PA_packh,  PA_zargs);
    PA_inst_c("switch",     none, FALSE, 0, (PFVoid) PA_pshand, PA_sargs);
    PA_inst_c("parameter",  none, FALSE, 0, (PFVoid) PA_pshand, PA_sargs);
    PA_inst_c("unit",       none, FALSE, 0, (PFVoid) PA_pshand, PA_sargs);
    PA_inst_c("conversion", none, FALSE, 0, (PFVoid) PA_pshand, PA_sargs);
    PA_inst_c("name",       none, FALSE, 0, (PFVoid) PA_pshand, PA_sargs);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DEFUN - define units for PANACEA */

void PA_defunc(void )
   {

    PA_gs.radian          = PA_def_unit(1.0, UNITS);
    PA_gs.steradian       = PA_def_unit(1.0, UNITS);
    PA_gs.mole            = PA_def_unit(1.0, UNITS);
    PA_gs.electric_charge = PA_def_unit(1.0, UNITS);
    PA_gs.cm              = PA_def_unit(1.0, UNITS);
    PA_gs.sec             = PA_def_unit(1.0, UNITS);
    PA_gs.gram            = PA_def_unit(1.0, UNITS);
    PA_gs.eV              = PA_def_unit(1.0, UNITS);
    PA_gs.kelvin          = PA_def_unit(1.0, UNITS);
  
    PA_gs.erg             = PA_def_unit(1.0, G, CM, CM, PER, SEC, SEC, UNITS);
    PA_gs.cc              = PA_def_unit(1.0, CM, CM, CM, UNITS);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
