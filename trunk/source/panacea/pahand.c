/*
 * PAHAND.C - handlers for a PANACEA generator
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "panacea_int.h"

typedef void (*PFSargs)(PA_command *cp);
typedef void (*PFStrarg)(char *s);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_ZARGS - handler for zero argument commands */

void PA_zargs(PA_command *cp)
   {PFVoid fnc;

    fnc = (PFVoid) cp->proc;

    (*(fnc))();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_SARGS - handler for single argument commands */

void PA_sargs(PA_command *cp)
   {

    (*(PFSargs)(cp->proc))(cp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_STRARG - handler for single string argument commands */

void PA_strarg(PA_command *cp)
   {

    (*(PFStrarg)(cp->proc))(PA_get_field("FIELD", "PA_STRARG", OPTL));

    return;}

/*--------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
