/*
 * FASCM.C - memory management Fortran wrappers
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "fpact.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MAKE_FF - allocate a chunk of memory and set the pointer
 *             - keep a name attached to this piece of memory
 *             - WARNING: only for FORTRAN with pointers
 *             - return TRUE if successful and FALSE otherwise
 */

FIXNUM _SC_make_ff(void **pm, FIXNUM *pni, FIXNUM *pnb, FIXNUM *pnc,
		   char *pname, int zsp)
   {long id;
    void *s;
    FIXNUM ok;
    mem_header *space;
    mem_descriptor *desc;
    mem_inf *info;
    SC_mem_opt opt;

    opt.perm = FALSE;
    opt.na   = FALSE;
    opt.zsp  = zsp;
    opt.typ  = -1;
    opt.where.pfunc = pname;
    opt.where.pfile = NULL;
    opt.where.line  = -1;

    s = _SC_ALLOC_N((long) *pni, (long) *pnb, &opt);

/* mark this block as having been allocated to FORTRAN
 * the memory map will need to know how to interpret the
 * char *pname correctly
 */
    if (s != NULL)
       {space = ((mem_header *) s) - 1;
	desc  = &space->block;
	info  = &desc->desc.info;
	
	id  = SC_GET_BLOCK_ID(desc);
	id &= ~SC_FF_NAME_MASK;
	SC_SET_BLOCK_ID(desc, id);};

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

FIXNUM FF_ID(scmakz, SCMAKZ)(void **am, FIXNUM *sni, FIXNUM *snb,
			     FIXNUM *sncn, char *pname, FIXNUM *szsp)
   {FIXNUM ok;

    ok = _SC_make_ff(am, sni, snb, sncn, pname, (int) *szsp);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCMAKF - allocate a chunk of memory and set the pointer
 *        - keep a name attached to this piece of memory
 *        - WARNING: only for FORTRAN with pointers
 *        - return TRUE if successful and FALSE otherwise
 */

FIXNUM FF_ID(scmakf, SCMAKF)(void **am, FIXNUM *sni, FIXNUM *snb,
			     FIXNUM *sncn, char *pname)
   {FIXNUM ok;

    ok = _SC_make_ff(am, sni, snb, sncn, pname, -1);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCMAKE - allocate a chunk of memory and set the pointer
 *        - WARNING: only for FORTRAN with pointers
 *        - return TRUE if successful and FALSE otherwise
 */

FIXNUM FF_ID(scmake, SCMAKE)(void **am, FIXNUM *sni, FIXNUM *snb)
   {FIXNUM ok;

    ok = _SC_make_ff(am, sni, snb, NULL, NULL, -1);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCREMZ - reallocate a chunk of memory and set the pointer
 *        - WARNING: only for Fortran with CRAY pointer extensions
 *        - return TRUE if successful and FALSE otherwise
 */

FIXNUM FF_ID(scremz, SCREMZ)(void **am, FIXNUM *sni,
			     FIXNUM *snb, FIXNUM *szsp)
   {FIXNUM rv;
    void *s;

    s = SC_gs.mm.realloc(*am, (long) *sni, (long) *snb, FALSE, (int) *szsp);

    rv = ((*am = s) != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCREMA - reallocate a chunk of memory and set the pointer
 *        - WARNING: only for Fortran with CRAY pointer extensions
 *        - return TRUE if successful and FALSE otherwise
 */

FIXNUM FF_ID(screma, SCREMA)(void **am, FIXNUM *sni, FIXNUM *snb)
   {long nb;
    FIXNUM rv;
    void *s;

    nb = (*sni)*(*snb);
    s  = CREMAKE(*am, char, nb);

    rv = ((*am = s) != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCFREZ - free a chunk of memory */

FIXNUM FF_ID(scfrez, SCFREZ)(void **am, FIXNUM *szsp)
   {FIXNUM ok;

    ok = SC_gs.mm.free(*am, (int) *szsp);
    if (ok == TRUE)
       *am = NULL;

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCFREE - free a chunk of memory */

FIXNUM FF_ID(scfree, SCFREE)(void **am)
   {FIXNUM ok;

    ok = _SC_FREE_N(*am, NULL);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCFPTE - free a chunk of memory */

FIXNUM FF_ID(scfpte, SCFPTE)(void *am)
   {FIXNUM rv;

    rv = FF_ID(scfree, SCFREE)(&am);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCISSP - is the space SCORE allocated */

FIXNUM FF_ID(scissp, SCISSP)(void **am)
   {FIXNUM ok;

    ok = SC_is_score_ptr(*am);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCMEMT - FORTRAN interface to SC_mem_trace */

FIXNUM FF_ID(scmemt, SCMEMT)(void)
   {FIXNUM rv;

    rv = SC_mem_trace();

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCMEMC - FORTRAN interface to SC_mem_chk */

FIXNUM FF_ID(scmemc, SCMEMC)(FIXNUM *st)
   {int typ;
    FIXNUM rv;

    typ = *st;

    rv = SC_mem_chk(typ);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCMEMS - FORTRAN interface to SC_mem_stats */

FIXNUM FF_ID(scmems, SCMEMS)(double *sal, double *sfr,
			     double *sdf, double *smx)
   {int64_t pl, pf, pd, pm;
    FIXNUM rv;

    SC_mem_stats(&pl, &pf, &pd, &pm);

    *sal = pl;
    *sfr = pf;
    *sdf = pd;
    *smx = pm;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCMEMM - FORTRAN interface to SC_mem_monitor */

FIXNUM FF_ID(scmemm, SCMEMM)(FIXNUM *so, FIXNUM *sl, FIXNUM *sni,
			     char *pid, FIXNUM *sncm, char *pmsg)
   {int old, lev, ni, nc, n;
    FIXNUM rv;
    char msg[MAXLINE], id[MAXLINE];

    old = *so;
    lev = *sl;
    ni  = *sni;
    nc  = *sncm;

    SC_FORTRAN_STR_C(id, pid, ni);

    rv = SC_mem_monitor(old, lev, id, msg);

    n = strlen(msg);
    n = min(n, nc);
    SC_strncpy(pmsg, *sncm, msg, n);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCMEML - FORTRAN interface to SC_mem_lookup */

FIXNUM FF_ID(scmeml, SCMEML)(FIXNUM *so, FIXNUM *sncm, char *pmsg)
   {int n, nc;
    FIXNUM ok;
    char *name;

    name = SC_mem_lookup((void *) so);
    if (name != NULL)
       {n  = strlen(name);
	nc = *sncm;
	n  = min(n, nc);
	SC_strncpy(pmsg, *sncm, name, n);
	ok = TRUE;}
    else
       {n  = 0;
	ok = FALSE;};

    *sncm = n;

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCMEMP - FORTRAN interface to SC_mem_map */

FIXNUM FF_ID(scmemp, SCMEMP)(FIXNUM *so)
   {int ord;
    FIXNUM rv;

    ord = *so;
    rv  = SC_mem_map(stdout, ord);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCMMCF - FORTRAN interface to SC_configure_mm */

FIXNUM FF_ID(scmmcf, SCMMCF)(FIXNUM *sxl, FIXNUM *sxm, FIXNUM *ssz,
			     REAL *sr)
   {long mxl, mxm, bsz;
    double r;
    FIXNUM rv;

    mxl = *sxl;
    mxm = *sxm;
    bsz = *ssz;
    r   = *sr;

    SC_configure_mm(mxl, mxm, bsz, r);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCMARK - fortran interface for SC_mark */

FIXNUM FF_ID(scmark, SCMARK)(void *am, FIXNUM *sn)
   {FIXNUM rv;

    rv = SC_mark(*(char **) am, *sn);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCMPNT - fortran interface for SC_mem_print */

FIXNUM FF_ID(scmpnt, SCMPNT)(void *am)
   {FIXNUM rv;

    SC_mem_print(*(char **) am);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCZRSP - fortran interface for SC_zero_space */

FIXNUM FF_ID(sczrsp, SCZRSP)(FIXNUM *sf)
   {FIXNUM zsp;

    zsp = SC_zero_space_n((int) *sf, -2);

    return(zsp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCGMEM - setup to use mprotect'd memory */

FIXNUM FF_ID(scgmem, SCGMEM)(FIXNUM *son)
   {int on;
    FIXNUM rv;

    on = *son;
    SC_use_guarded_mem(on);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
