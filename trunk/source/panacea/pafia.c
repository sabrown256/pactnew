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
 *        -   AVN is the array of variable names
 *        -   AP is the array of pointers to connect
 *        -   SN is the number of variables to connect
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(pamcon, PAMCON)(FIXNUM *anc, char **avn, void **ap, FIXNUM *sn)
   {int i, n;
    char t[MAXLINE];
    void **ptr;
    PA_package *pck;

    pck = PA_current_package();
    SC_ASSERT(pck != NULL);

    n = *sn;
    for (i = 0; i < n; i++)
        {SC_strncpy(t, MAXLINE, avn[i], anc[i]);

         ptr = ap + i;
         PA_get_access(ptr, t, -1L, 0L, TRUE);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PANTRN - intern to a single variable
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(pantrn, PANTRN)(void *av, FIXNUM *sncn, char *vname)
   {char t[MAXLINE];

    SC_FORTRAN_STR_C(t, vname, *sncn);

    PA_intern(av, t);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PACONN - connect to a single variable
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(paconn, PACONN)(void **av, FIXNUM *sncn, char *vname,
			     FIXNUM *sf)
   {char t[MAXLINE];

    SC_FORTRAN_STR_C(t, vname, *sncn);

    PA_get_access(av, t, -1L, 0L, *sf);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PADCON - disconnect a single variable
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(padcon, PADCON)(void **av, FIXNUM *sncn, char *vname)
   {char t[MAXLINE], s[MAXLINE];

    SC_FORTRAN_STR_C(t, vname, *sncn);

    PA_rel_access(av, s, -1L, 0L);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PALOCA - allocate some space and hand it back
 *        - this is a FORTRAN version of MAKE_N
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(paloca, PALOCA)(void **av, FIXNUM *sbpi, FIXNUM *sn, ...)
   {long i, n, size;
    FIXNUM *dim;

    SC_VA_START(sn);

    n    = *sn;
    size = 1L;
    for (i = 0L; i < n; i++)
        {dim   = SC_VA_ARG(FIXNUM *);
         size *= *dim;};

    SC_VA_END;

    *av = CMAKE_N(char, size*(*sbpi));

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PALLOC - complete the definition and allocate the named variable
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(palloc, PALLOC)(void **av, FIXNUM *sncn,
			     char *vname, FIXNUM *sf, ...)
   {char s[MAXLINE];
    int *pdima;
    PA_variable *pp;
    dimdes *dims, *pd;
    PA_dimens *vdims, *pvd;

    SC_FORTRAN_STR_C(s, vname, *sncn);

    pp = PA_inquire_variable(s);
    PA_ERR(pp == NULL,
           "VARIABLE %s NOT IN DATABASE - PALLOC", vname);

    SC_VA_START(sf);

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

    PA_get_access(av, s, -1L, 0L, *sf);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PASPCK - set the current package to be the named one
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(paspck, PASPCK)(FIXNUM *sncn, char *pname)
   {char t[MAXLINE];

    SC_FORTRAN_STR_C(t, pname, *sncn);

    PA_control_set(t);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAERR - handle errors */

void FF_ID(paerr, PAERR)(FIXNUM *sncn, char *pname)
   {char t[MAXLINE];

    SC_FORTRAN_STR_C(t, pname, *sncn);

    PA_error(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAGDOF - get the file default offset */

void FF_ID(pagdof, PAGDOF)(FIXNUM *sd)
   {

    *sd = PA_get_default_offset();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PASDOF - set the file default offset */

void FF_ID(pasdof, PASDOF)(FIXNUM *sd)
   {

    PA_set_default_offset(*sd);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PASEQV - set an equivalenced value */

void FF_ID(paseqv, PASEQV)(FIXNUM *sncn, char *name, void *avl)
   {char t[MAXLINE];

    SC_FORTRAN_STR_C(t, name, *sncn);

    PA_set_equiv(t, avl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PADEQV - define an equivalence */

void FF_ID(padeqv, PADEQV)(void *avr, FIXNUM *sncn, char *name)
   {char t[MAXLINE];

    SC_FORTRAN_STR_C(t, name, *sncn);

    PA_equivalence(avr, t);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
