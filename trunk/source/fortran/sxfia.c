/*
 * SXFIA.C - FORTRAN interface routines for SCORE/SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "fpact.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SSCHEM - do a Fortran version of SS_call_scheme */

FIXNUM FF_ID(sschem, SSCHEM)(FIXNUM *snc, char *name, ...)
   {int i, type[MAXLINE];
    FIXNUM rv;
    void *ptr[MAXLINE];
    char func[80];
    SC_address ret;
    object *fnc, *expr;
    SS_psides *si;

    si = SS_get_current_scheme(-1);
    
    SC_FORTRAN_STR_C(func, name, *snc);

    SC_VA_START(name);

    fnc = (object *) SC_hasharr_def_lookup(si->symtab, func);
    if (fnc == NULL)
       SS_error(si, "UNKNOWN PROCEDURE - SSCHEM", SS_mk_string(si, func));

    for (i = 0; i < MAXLINE; i++)
        {type[i] = *SC_VA_ARG(int *);
         if (type[i] == 0)
            break;

         ptr[i] = (void *) *SC_VA_ARG(char **);
         if (ptr[i] == (void *) LAST)
            break;};

    SC_VA_END;

    expr = SS_null;
    SS_assign(si, expr, SS_mk_cons(si, fnc, _SS_make_list(si, i, type, ptr)));

    SS_eval(si, expr);

    SS_assign(si, expr, SS_null);

    ret.memaddr = (char *) si->val;

    rv = ret.diskaddr;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SXDTAC - Fortran interface to SX_rep_to_ac */

FIXNUM FF_ID(sxdtac, SXDTAC)(FIXNUM *sncn, char *pname,
			     double *arx, double *ary,
			     FIXNUM *snn, FIXNUM *snz, FIXNUM *azones)
   {int n_nodes, n_zones;
    FIXNUM rv;
    char name[MAXLINE];
    PM_set *set;

    SC_FORTRAN_STR_C(name, pname, *sncn);

    n_nodes = *snn;
    n_zones = *snz;

    set = SX_rep_to_ac(name, arx, ary, n_nodes, n_zones, (int *) azones);

    rv = SC_ADD_POINTER(set);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

