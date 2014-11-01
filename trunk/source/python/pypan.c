/*
 * PYPAN.C - PANACEA type supports
 *
 */

#include "cpyright.h"
#include "py_int.h"

/*--------------------------------------------------------------------------*/

/*                             PACKAGE ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* _PY_OPT_PA_PACKAGE - handle BLANG binding related operations */

void *_PY_opt_PA_package(PA_package *x, bind_opt wh, void *a)
   {void *rv;

    rv = NULL;
    switch (wh)
       {case BIND_ARG :
        case BIND_LABEL :
        case BIND_PRINT :
        case BIND_FREE :
        case BIND_ALLOC :
	default:
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                             VARIABLE ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* _PY_OPT_PA_VARIABLE - handle BLANG binding related operations */

void *_PY_opt_PA_variable(PA_variable *x, bind_opt wh, void *a)
   {void *rv;

    rv = NULL;
    switch (wh)
       {case BIND_ARG :
        case BIND_LABEL :
        case BIND_PRINT :
        case BIND_FREE :
        case BIND_ALLOC :
	default:
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                         IV_SPECIFICATION ROUTINES                        */

/*--------------------------------------------------------------------------*/

/* _PY_OPT_PA_IV_SPECIFICATION - handle BLANG binding related operations */

void *_PY_opt_PA_iv_specification(PA_iv_specification *x, bind_opt wh, void *a)
   {void *rv;

    rv = NULL;
    switch (wh)
       {case BIND_ARG :
        case BIND_LABEL :
        case BIND_PRINT :
        case BIND_FREE :
        case BIND_ALLOC :
	default:
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
