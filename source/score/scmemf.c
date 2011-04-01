/*
 * SCMEMF.C - memory management Fortran wrappers
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "scope_mem.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _SC_MAKE_F77 - allocate a chunk of memory and set the pointer
 *              - keep a name attached to this piece of memory
 *              - WARNING: only for FORTRAN with pointers
 *              - return TRUE if successful and FALSE otherwise
 */

FIXNUM _SC_make_f77(void **pm, FIXNUM *pni, FIXNUM *pnb, FIXNUM *pnc,
		    F77_string pname, int zsp)
   {void *s;
    FIXNUM ok;
    mem_header *space;
    mem_descriptor *desc;
    SC_mem_opt opt;

    opt.na   = FALSE;
    opt.zsp  = zsp;
    opt.typ  = -1;
    opt.fnc  = pname;
    opt.file = NULL;
    opt.line = -1;

    s = SC_alloc_nzt((long) *pni, (long) *pnb, &opt);

/* mark this block as having been allocated to FORTRAN
 * the memory map will need to know how to interpret the
 * F77_string pname correctly
 */
    if (s != NULL)
       {space = ((mem_header *) s) - 1;
	desc  = &space->block;
	
	desc->id &= ~SC_FTN_NAME_MASK;};

    ok  = (s != NULL);
    *pm = s;

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCMAKZ - allocate a chunk of memory and set the pointer
 *        - keep a name attached to this piece of memory
 *        - WARNING: only for FORTRAN with pointers
 *        - return TRUE if successful and FALSE otherwise
 */

FIXNUM F77_FUNC(scmakz, SCMAKZ)(void **pm, FIXNUM *pni, FIXNUM *pnb,
				FIXNUM *pnc, F77_string pname, FIXNUM *pzsp)
   {FIXNUM ok;

    ok = _SC_make_f77(pm, pni, pnb, pnc, pname, (int) *pzsp);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCMAKF - allocate a chunk of memory and set the pointer
 *        - keep a name attached to this piece of memory
 *        - WARNING: only for FORTRAN with pointers
 *        - return TRUE if successful and FALSE otherwise
 */

FIXNUM F77_FUNC(scmakf, SCMAKF)(void **pm, FIXNUM *pni, FIXNUM *pnb,
				FIXNUM *pnc, F77_string pname)
   {FIXNUM ok;

    ok = _SC_make_f77(pm, pni, pnb, pnc, pname, _SC_zero_space);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCMAKE - allocate a chunk of memory and set the pointer
 *        - WARNING: only for FORTRAN with pointers
 *        - return TRUE if successful and FALSE otherwise
 */

FIXNUM F77_FUNC(scmake, SCMAKE)(void **pm, FIXNUM *pni, FIXNUM *pnb)
   {FIXNUM ok;

    ok = _SC_make_f77(pm, pni, pnb, NULL, NULL, _SC_zero_space);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCREMZ - reallocate a chunk of memory and set the pointer
 *        - WARNING: only for F77 with CRAY pointer extensions
 *        - return TRUE if successful and FALSE otherwise
 */

FIXNUM F77_FUNC(scremz, SCREMZ)(void **pm, FIXNUM *pni,
				FIXNUM *pnb, FIXNUM *pzsp)
   {FIXNUM rv;
    void *s;

    s = SC_realloc_nz(*pm, (long) *pni, (long) *pnb, FALSE, (int) *pzsp);

    rv = ((*pm = s) != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCREMA - reallocate a chunk of memory and set the pointer
 *        - WARNING: only for F77 with CRAY pointer extensions
 *        - return TRUE if successful and FALSE otherwise
 */

FIXNUM F77_FUNC(screma, SCREMA)(void **pm, FIXNUM *pni, FIXNUM *pnb)
   {FIXNUM rv;
    void *s;

    s = SC_realloc_na(*pm, (long) *pni, (long) *pnb, FALSE);

    rv = ((*pm = s) != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCFREZ - free a chunk of memory */

FIXNUM F77_FUNC(scfrez, SCFREZ)(void **pm, FIXNUM *pzsp)
   {FIXNUM ok;

    ok = SC_free_z(*pm, (int) *pzsp);
    if (ok)
       *pm = NULL;

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCFREE - free a chunk of memory */

FIXNUM F77_FUNC(scfree, SCFREE)(void **pm)
   {FIXNUM ok;

    ok = SC_free(*pm);
    if (ok)
       *pm = NULL;

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCFPTE - free a chunk of memory */

FIXNUM F77_FUNC(scfpte, SCFPTE)(void *pm)
   {FIXNUM rv;

    rv = F77_FUNC(scfree, SCFREE)(&pm);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCISSP - is the space SCORE allocated */

FIXNUM F77_FUNC(scissp, SCISSP)(void **pm)
   {FIXNUM ok;

    ok = SC_is_score_ptr(*pm);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCMEMT - FORTRAN interface to SC_mem_trace */

FIXNUM F77_FUNC(scmemt, SCMEMT)(void)
   {FIXNUM rv;

    rv = SC_mem_trace();

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCMEMC - FORTRAN interface to SC_mem_chk */

FIXNUM F77_FUNC(scmemc, SCMEMC)(FIXNUM *pt)
   {int typ;
    FIXNUM rv;

    typ = *pt;

    rv = SC_mem_chk(typ);

    return(rv);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* SCMEMS - FORTRAN interface to SC_mem_stats */

FIXNUM F77_FUNC(scmems, SCMEMS)(FIXNUM *pal, FIXNUM *pfr,
				FIXNUM *pdf, FIXNUM *pmx)
   {long pl, pf, pd, pm;
    FIXNUM rv;

    SC_mem_stats(&pl, &pf, &pd, &pm);

    *pal = pl;
    *pfr = pf;
    *pdf = pd;
    *pmx = pm;

    rv = TRUE;

    return(rv);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* SCMEMM - FORTRAN interface to SC_mem_monitor */

FIXNUM F77_FUNC(scmemm, SCMEMM)(FIXNUM *po, FIXNUM *pl, FIXNUM *pni,
				F77_string pid, FIXNUM *pnc, F77_string pmsg)
   {int old, lev, ni, nc, n;
    FIXNUM rv;
    char msg[MAXLINE], id[MAXLINE];

    old = *po;
    lev = *pl;
    ni  = *pni;
    nc  = *pnc;

    SC_FORTRAN_STR_C(id, pid, ni);

    rv = SC_mem_monitor(old, lev, id, msg);

    n = strlen(msg);
    n = min(n, nc);
    SC_strncpy(SC_F77_C_STRING(pmsg), *pnc, msg, n);

    return(rv);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* SCMEML - FORTRAN interface to SC_mem_lookup */

FIXNUM F77_FUNC(scmeml, SCMEML)(FIXNUM *po, FIXNUM *pnc, F77_string pmsg)
   {int n, nc;
    FIXNUM ok;
    char *name;

    name = SC_mem_lookup((void *) po);
    if (name != NULL)
       {n  = strlen(name);
	nc = *pnc;
	n  = min(n, nc);
	SC_strncpy(SC_F77_C_STRING(pmsg), *pnc, name, n);
	ok = TRUE;}
    else
       {n  = 0;
	ok = FALSE;};

    *pnc = n;

    return(ok);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* SCMEMP - FORTRAN interface to SC_mem_map */

FIXNUM F77_FUNC(scmemp, SCMEMP)(FIXNUM *po)
   {int ord;
    FIXNUM rv;

    ord = *po;
    rv  = SC_mem_map(stdout, ord);

    return(rv);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* SCMMCF - FORTRAN interface to SC_configure_mm */

FIXNUM F77_FUNC(scmmcf, SCMMCF)(FIXNUM *pxl, FIXNUM *pxm, FIXNUM *psz,
				REAL *pr)
   {long mxl, mxm, bsz;
    double r;
    FIXNUM rv;

    mxl = *pxl;
    mxm = *pxm;
    bsz = *psz;
    r   = *pr;

    SC_configure_mm(mxl, mxm, bsz, r);

    rv = TRUE;

    return(rv);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* SCMARK - fortran interface for SC_mark */

FIXNUM F77_FUNC(scmark, SCMARK)(void *pm, FIXNUM *pn)
   {FIXNUM rv;

    rv = SC_mark(*(char **) pm, *pn);

    return(rv);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* SCMPNT - fortran interface for SC_mem_print */

FIXNUM F77_FUNC(scmpnt, SCMPNT)(void *pm)
   {FIXNUM rv;

    SC_mem_print(*(char **) pm);

    rv = TRUE;

    return(rv);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
