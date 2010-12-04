/*
 * PADSYS.C - routines to manage the packages in an abstract fashion
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "panacea_int.h"

PA_package
 *Packages;

int
 N_Packages = 0;

char
 *PAN_PACKAGE = NULL;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_INTERNAL_INIT - do some important internal initializations */

void _PA_internal_init(void)
   {

    PA_set_n_threads(1);

    PA_variable_tab = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY);

    PAN_VARIABLE     = SC_strsavef("PA_variable",
                                  "char*:_PA_INTERNAL_INIT:var");
    PAN_PACKAGE      = SC_strsavef("PA_package",
                                  "char*:_PA_INTERNAL_INIT:pkg");
    PAN_ATTRIBUTE    = SC_strsavef(SC_PCONS_P_S, "char*:_PA_INTERNAL_INIT:attr");
    PAN_UNIT         = SC_strsavef(SC_PCONS_P_S, "char*:_PA_INTERNAL_INIT:unit");
    PAN_DIMENSION    = SC_strsavef("PA_dimens",
                                  "char*:_PA_INTERNAL_INIT:dimens");
    PAN_DOMAIN       = SC_strsavef("PA_edit_domain",
                                  "char*:_PA_INTERNAL_INIT:domain");
    PAN_EDIT_REQUEST = SC_strsavef("PA_edit_request",
                                  "char*:_PA_INTERNAL_INIT:request");
    PAN_EDIT_OUT     = SC_strsavef("PA_edit_out",
                                  "char*:_PA_INTERNAL_INIT:out");

    SC_permanent(PAN_VARIABLE);
    SC_permanent(PAN_PACKAGE);
    SC_permanent(PAN_ATTRIBUTE);
    SC_permanent(PAN_UNIT);
    SC_permanent(PAN_DIMENSION);
    SC_permanent(PAN_DOMAIN);
    SC_permanent(PAN_EDIT_REQUEST);
    SC_permanent(PAN_EDIT_OUT);

    PA_DUL = FMAKE(int, "_PA_INTERNAL_INIT:PA_DUL");
    PA_DON = FMAKE(int, "_PA_INTERNAL_INIT:PA_DON");

    *PA_DUL = PA_UPPER_LOWER;
    *PA_DON = PA_OFFSET_NUMBER;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_GEN_PACKAGE - install the package variables
 *                - the generator only needs to do this much of a complete
 *                - package installation
 */

PA_package *PA_gen_package(char *name, PFPkgGencmd cmd, PFPkgDfstrc dfs,
                           PFPkgDefun dfu, PFPkgDefvar dfr, PFPkgDefcnt cnt,
                           PFPkgIntrn inr, char *fname)
   {PA_package *pck;

    N_Packages++;

    pck = FMAKE(PA_package, "PA_GEN_PACKAGE:pck");
    SC_MEM_INIT(PA_package, pck);

/* make the variable hash table if it doesn't exist yet */
    if (PA_variable_tab == NULL)
       _PA_internal_init();

/* if this is the first package set Packages to it */
    if (_PA.last_gen == NULL)
       Packages = pck;

    pck->name    = SC_strsavef(name, "char*:PA_GEN_PACKAGE:name");
    pck->gencmd  = cmd;
    pck->dfstrc  = dfs;
    pck->defun   = dfu;
    pck->defvar  = dfr;
    pck->defcnt  = cnt;
    pck->intrn   = inr;
    pck->inizer  = NULL;
    pck->main    = NULL;
    pck->ppsor   = NULL;
    pck->finzer  = NULL;
    pck->ppcmd   = NULL;
    pck->next    = NULL;
    pck->db_file = fname;

/* if a database file is being used process it now */
    if (fname != NULL)
       PA_read_def(pck, fname);

/* install the package controls in the data base and conect them */
    else if (cnt != NULL)
       (*cnt)(pck);

/* if this is one of chain, set last to the next link */
    if (_PA.last_gen != NULL)
       _PA.last_gen->next = pck;
    _PA.last_gen = pck;

/* install the package in the data base */
    SC_hasharr_install(PA_variable_tab, name, pck, PAN_PACKAGE, TRUE, TRUE);

    return(pck);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_RUN_TIME_PACKAGE - install a package for a PANACEA runtime code
 *                     - use gen_package to install the variable definers
 *                     - then install the runtime routines
 */

void PA_run_time_package(char *name, PFPkgDfstrc dfs, PFPkgDefun dfu,
                         PFPkgDefvar dfr, PFPkgDefcnt cnt, PFPkgInizer izr,
                         PFPkgMain mn, PFPkgPpsor psr, PFPkgFinzer fzr,
                         char *fname)
   {PA_package *pck;

    N_Packages++;
    pck = FMAKE(PA_package, "PA_RUN_TIME_PACKAGE:pck");

/* make the variable hash table if it doesn't exist yet */
    if (PA_variable_tab == NULL)
       _PA_internal_init();

/* if this is the first package set Packages to it */
    if (_PA.last_run == NULL)
       Packages = pck;

    pck->name    = SC_strsavef(name, "char*:PA_RUN_TIME_PACKAGE:name");
    pck->gencmd  = NULL;
    pck->intrn   = NULL;
    pck->dfstrc  = dfs;
    pck->defun   = dfu;
    pck->defvar  = dfr;
    pck->defcnt  = cnt;
    pck->inizer  = izr;
    pck->main    = mn;
    pck->ppsor   = psr;
    pck->finzer  = fzr;
    pck->ppcmd   = NULL;
    pck->next    = NULL;
    pck->db_file = fname;

/* if a database file is being used process it now */
    if (fname != NULL)
       PA_read_def(pck, fname);

/* install the package controls in the data base and conect them */
    else if (cnt != NULL)
       (*cnt)(pck);

/* if this is one of chain, set last to the next link */
    if (_PA.last_run != NULL)
       _PA.last_run->next = pck;
    _PA.last_run = pck;

/* install the package in the data base */
    SC_hasharr_install(PA_variable_tab, name, pck, PAN_PACKAGE, TRUE, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DEF_PACKAGE - install a package for a PANACEA code
 *                - use gen_package to install the variable definers
 *                - then install the runtime routines
 */

void PA_def_package(char *name, PFPkgGencmd gcmd, PFPkgDfstrc dfs,
                    PFPkgDefun dfu, PFPkgDefvar dfr, PFPkgDefcnt cnt,
                    PFPkgIntrn inr, PFPkgInizer izr, PFPkgMain mn,
                    PFPkgPpsor psr, PFPkgFinzer fzr, PFPkgPpcmd pcmd,
                    char *fname)
   {PA_package *pck;

    pck = PA_gen_package(name, gcmd, dfs, dfu, dfr, cnt, inr, fname);

    pck->inizer = izr;
    pck->main   = mn;
    pck->ppsor  = psr;
    pck->finzer = fzr;
    pck->ppcmd  = pcmd;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_CONTROL_SET - connect the control array pointers: iswtch, rparam, and
 *                - name to the named package control arrays
 */

void PA_control_set(char *s)
   {PA_package *pck;
    PA_variable *pp;
    PA_thread_state *ps;
    char bf[MAXLINE];

    pck = PA_INQUIRE_PACKAGE(s);
    PA_ERR((pck == NULL),
           "UNKNOWN PACKAGE %s - PA_CONTROL_SET", s);

    ps = PA_get_thread(-1);

    snprintf(bf, MAXLINE, "%s-names", s);
    pp = PA_INQUIRE_VARIABLE(bf);
    if (pp != NULL)
       {PA_CONNECT(ps->aname, bf, FALSE);
        ps->aname -= 1;};

    snprintf(bf, MAXLINE, "%s-params", s);
    pp = PA_INQUIRE_VARIABLE(bf);
    if (pp != NULL)
       {PA_CONNECT(ps->rparam, bf, FALSE);
        ps->rparam -= 1;};

    snprintf(bf, MAXLINE, "%s-swtchs", s);
    pp = PA_INQUIRE_VARIABLE(bf);
    if (pp != NULL)
       {PA_CONNECT(ps->iswtch, bf, FALSE);
        ps->iswtch -= 1;};

    ps->current = pck;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_CURRENT_PACKAGE - return a pointer to the current package
 *                    - i.e. the last package selected with PA_control_set
 */

PA_package *PA_current_package(void)
   {PA_thread_state *ps;
    PA_package *pck;

    ps = PA_get_thread(-1);

    pck = ps->current;

    return(pck);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
