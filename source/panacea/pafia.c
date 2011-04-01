/*
 * PAFIA.C - FORTRAN interface routines for PANACEA
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "panacea_int.h"

/*--------------------------------------------------------------------------*/

/*                          DATABASE ACCESSORS                              */

/*--------------------------------------------------------------------------*/

/* PAMCON - give FORTRAN routines access to the database
 *        - via pointers
 *        -   NAME is the package name
 *        -   PVN is the array of variable names
 *        -   PP is the array of pointers to connect
 *        -   PN is the number of variables to connect
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pamcon, PAMCON)(FIXNUM *pnc, char **pvn, void **pp, FIXNUM *pn)
   {int i, n;
    char t[MAXLINE];
    void **ptr;
    PA_package *pck;

    pck = PA_current_package();
    SC_ASSERT(pck != NULL);

    n = *pn;
    for (i = 0; i < n; i++)
        {SC_strncpy(t, MAXLINE, pvn[i], pnc[i]);

         ptr = pp + i;
         PA_get_access(ptr, t, -1L, 0L, TRUE);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PANTRN - intern to a single variable
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pantrn, PANTRN)(void *pv, FIXNUM *pnc, F77_string vname)
   {char t[MAXLINE];

    SC_FORTRAN_STR_C(t, vname, *pnc);

    PA_intern(pv, t);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PACONN - connect to a single variable
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(paconn, PACONN)(void **pv, FIXNUM *pnc, F77_string vname,
			     FIXNUM *pf)
   {char t[MAXLINE];

    SC_FORTRAN_STR_C(t, vname, *pnc);

    PA_get_access(pv, t, -1L, 0L, *pf);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PADCON - disconnect a single variable
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(padcon, PADCON)(void **pv, FIXNUM *pnc, F77_string vname)
   {char t[MAXLINE], s[MAXLINE];

    SC_FORTRAN_STR_C(t, vname, *pnc);

    PA_rel_access(pv, s, -1L, 0L);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PALOCA - allocate some space and hand it back
 *        - this is a FORTRAN version of MAKE_N
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(paloca, PALOCA)(void **pv, FIXNUM *pbpi, FIXNUM *pn, ...)
   {long i, n, size;
    FIXNUM *dim;

    SC_VA_START(pn);

    n    = *pn;
    size = 1L;
    for (i = 0L; i < n; i++)
        {dim   = SC_VA_ARG(FIXNUM *);
         size *= *dim;};

    SC_VA_END;

    *pv = CMAKE_N(char, size*(*pbpi));

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PALLOC - complete the definition and allocate the named variable
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(palloc, PALLOC)(void **pv, FIXNUM *pnc,
			     F77_string vname, FIXNUM *pf, ...)
   {char s[MAXLINE];
    int *pdima;
    PA_variable *pp;
    dimdes *dims, *pd;
    PA_dimens *vdims, *pvd;

    SC_FORTRAN_STR_C(s, vname, *pnc);

    pp = PA_inquire_variable(s);
    PA_ERR(pp == NULL,
           "VARIABLE %s NOT IN DATABASE - PALLOC", vname);

    SC_VA_START(pf);

/* GOTCHA: there is real trouble if a sizeof(FIXNUM) != sizeof(int)
 *         FORTRAN naturally uses FIXNUM as ints
 *         C dimension variable are ints
 *
 *         Document this as a requirement and remove
 */

    vdims = PA_VARIABLE_DIMS(pp);
    dims  = PA_VARIABLE_DESC(pp)->dimensions;
    pd    = dims;
    pvd   = vdims;
    while (pvd != NULL)
       {pdima = (int *) SC_VA_ARG(FIXNUM *);
        if (pdima == PA_DUL)
           {pvd->index_min = (int *) SC_VA_ARG(FIXNUM *);
            pvd->index_max = (int *) SC_VA_ARG(FIXNUM *);
            pd->index_min  = *pvd->index_min;
            pd->index_max  = *pvd->index_max;
            pd->number     = pd->index_max - pd->index_min + 1L;}

        else if (pdima == PA_DON)
           {pvd->index_min = (int *) SC_VA_ARG(FIXNUM *);
            pvd->index_max = (int *) SC_VA_ARG(FIXNUM *);
            pd->index_min  = *pvd->index_min;
            pd->index_max  = *pvd->index_min + *pvd->index_max - 1L;
            pd->number     = *pvd->index_max;}

        else
           {switch (pvd->method)
               {case PA_UPPER_LOWER :
                     pvd->index_max = pdima;
                     pd->index_max  = *pdima;
                     pd->number     = pd->index_max - pd->index_min + 1L;
                     break;
                case PA_OFFSET_NUMBER :
                     pvd->index_max = pdima;
                     pd->number     = *pdima;
                     pd->index_max  = *pdima + pd->index_min - 1L;};};

        pvd = pvd->next;
        pd  = pd->next;};

    SC_VA_END;

    PA_get_access(pv, s, -1L, 0L, *pf);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PASPCK - set the current package to be the named one
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(paspck, PASPCK)(FIXNUM *pnc, F77_string pname)
   {char t[MAXLINE];

    SC_FORTRAN_STR_C(t, pname, *pnc);

    PA_control_set(t);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAERR - handle errors */

void F77_FUNC(paerr, PAERR)(FIXNUM *pnc, F77_string pname)
   {char t[MAXLINE];

    SC_FORTRAN_STR_C(t, pname, *pnc);

    PA_error(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAGDOF - get the file default offset */

void F77_FUNC(pagdof, PAGDOF)(FIXNUM *pd)
   {

    *pd = PA_get_default_offset();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PASDOF - set the file default offset */

void F77_FUNC(pasdof, PASDOF)(FIXNUM *pd)
   {

    PA_set_default_offset(*pd);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PASEQV - set an equivalenced value */

void F77_FUNC(paseqv, PASEQV)(FIXNUM *pnc, F77_string name, void *vl)
   {char t[MAXLINE];

    SC_FORTRAN_STR_C(t, name, *pnc);

    PA_set_equiv(t, vl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PADEQV - define an equivalence */

void F77_FUNC(padeqv, PADEQV)(void *vr, FIXNUM *pnc, F77_string name)
   {char t[MAXLINE];

    SC_FORTRAN_STR_C(t, name, *pnc);

    PA_equivalence(vr, t);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
