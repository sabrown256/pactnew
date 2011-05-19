/*
 * MLFIA.C - FORTRAN interface to PML
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMSZFT - return the largest M such that 2^M <= N  */

FIXNUM F77_FUNC(pmszft, PMSZFT)(FIXNUM *pn)
   {

    *pn = (FIXNUM) PM_next_exp_two((int) *pn);

    return((FIXNUM) TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMRFFT - perform an FFT on real data
 *        - this a wrapper for PM_fft_sc_real_data
 */

FIXNUM F77_FUNC(pmrfft, PMRFFT)(double *outyr, double *outyi, double *outx,
				FIXNUM *pn, double *inx, double *iny,
				double *pxn, double *pxx, FIXNUM *po)
   {int i, n, np, ordr;
    FIXNUM rv;
    double xmn, xmx;
    double *rx;
    complex *cy;

    n    = *pn;
    xmn  = *pxn;
    xmx  = *pxx;
    ordr = *po;

    if (!PM_fft_sc_real_data(&cy, &rx, inx, iny, n, xmn, xmx, ordr))
       rv = FALSE;

    else
       {np = n + 1;
	for (i = 0; i < np; i++)
	    {outyr[i] = PM_REAL_C(cy[i]);
	     outyi[i] = PM_IMAGINARY_C(cy[i]);
	     outx[i]  = rx[i];};

	CFREE(rx);
	CFREE(cy);

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMCFFT - perform an FFT on complex data
 *        - this a wrapper for PM_fft_sc_complex_data
 */

FIXNUM F77_FUNC(pmcfft, PMCFFT)(double *outyr, double *outyi, double *outx,
				FIXNUM *pn, double *inx, double *inyr,
				double *inyi, double *pxn, double *pxx,
				FIXNUM *pf, FIXNUM *po)
   {int i, n, np, flag, ordr;
    FIXNUM rv;
    double xmn, xmx;
    double *rx;
    complex *cy, *incy;

    n    = *pn;
    xmn  = *pxn;
    xmx  = *pxx;
    flag = *pf;
    ordr = *po;

    incy = CMAKE_N(complex, n);
    for (i = 0; i < n; i++)
        incy[i] = PM_COMPLEX(inyr[i], inyi[i]);

    if (!PM_fft_sc_complex_data(&cy, &rx, inx, incy, n,
				xmn, xmx, flag, ordr))
       rv = FALSE;

    else
       {np = n + 1;
	for (i = 0; i < np; i++)
	    {outyr[i] = PM_REAL_C(cy[i]);
	     outyi[i] = PM_IMAGINARY_C(cy[i]);
	     outx[i]  = rx[i];};

	CFREE(rx);
	CFREE(cy);
	CFREE(incy);

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMBSET - begin making a set */

FIXNUM F77_FUNC(pmbset, PMBSET)(FIXNUM *pn, char *fname, FIXNUM *pt,
				char *ftype, FIXNUM *pcp, FIXNUM *pnd,
				FIXNUM *pnde, FIXNUM *pmx, FIXNUM *ptp,
				FIXNUM *inxt)
   {int i, id, cp, nd, nde, bpi;
    int *maxes;
    long ne, tp, d;
    FIXNUM rv;
    char name[MAXLINE], type[MAXLINE], bf[MAXLINE], *topt, *s;
    void **elem;
    pcons *info;
    PM_set *set, *next;
    PM_mesh_topology *top;

    SC_FORTRAN_STR_C(name, fname, *pn);
    SC_FORTRAN_STR_C(type, ftype, *pt);

    next = SC_GET_POINTER(PM_set, *inxt);
    cp   = *pcp;
    nd   = *pnd;
    nde  = *pnde;
    tp   = *ptp;

    if (tp == -1)
       {maxes = CMAKE_N(int, nd);
	ne    = 1L;
        for (i = 0; i < nd; i++)
	    {d = pmx[i];
	     maxes[i] = d;
	     ne *= d;};
        topt = NULL;
	top  = NULL;}

    else
       {topt  = PM_MESH_TOPOLOGY_P_S;
        top   = SC_GET_POINTER(PM_mesh_topology, *ptp);
	maxes = NULL;
        ne    = top->n_cells[0];};

    info = NULL;
    SC_CHANGE_VALUE_ALIST(info, int, SC_INT_P_S, "COPY-MEMORY", cp);

    elem = CMAKE_N(void *, nde);

    rv = -1;

/* build the set */
    set = CMAKE(PM_set);
    if (set != NULL)
       {set->name           = CSTRSAVE(name);
	set->n_elements     = ne;
	set->dimension      = nd;
	set->dimension_elem = nde;
	set->max_index      = maxes;
	set->elements       = (void *) elem;
	set->extrema        = NULL;
	set->scales         = NULL;
	set->opers          = NULL;
	set->metric         = NULL;
	set->symmetry_type  = NULL;
	set->symmetry       = NULL;
	set->topology_type  = topt;
	set->topology       = (void *) top;
	set->info_type      = SC_PCONS_P_S;
	set->info           = (void *) info;
	set->next           = next;
    
	strcpy(bf, type);
	SC_strtok(bf, " *", s);
	if (bf != NULL)
	   {id = SC_type_id(bf, FALSE);
	    if ((id == SC_BOOL_I) && (SC_is_type_num(id) == TRUE))
	       bpi = SC_type_size_a(bf);

	    if (bpi > 0)
	       {set->extrema = CMAKE_N(char, 2*bpi*nde);
		set->scales  = CMAKE_N(char, bpi*nde);};}

	SC_strcat(bf, MAXLINE, " *");
	set->es_type = CSTRSAVE(bf);

	SC_strcat(bf, MAXLINE, "*");
	set->element_type = CSTRSAVE(bf);

	rv = SC_ADD_POINTER(set);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMESET - complete making a set */

FIXNUM F77_FUNC(pmeset, PMESET)(FIXNUM *iset)
   {PM_set *set;

    set = SC_GET_POINTER(PM_set, *iset);

    PM_find_extrema(set);

    return((FIXNUM) TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMASET - add a component to a set */

FIXNUM F77_FUNC(pmaset, PMASET)(FIXNUM *iset, FIXNUM *pie, void *px)
   {int ie, cp;
    pcons *info;
    void **elem;
    PM_set *set;
    char *s;

    set  = SC_GET_POINTER(PM_set, *iset);
    ie   = *pie - 1;
    elem = (void **) set->elements;
    info = (pcons *) set->info;
    SC_assoc_info(info,
		  "COPY-MEMORY", &cp,
		  NULL);

/* if requested copy the incoming data */
    if (cp)
       {int bpi;
        long ne;
	void *nv;
        char bf[MAXLINE], *type;

	strcpy(bf, set->es_type);
	type = SC_strtok(bf, " *", s);
        bpi  = SIZEOF(type);
	ne   = set->n_elements;

	nv = CMAKE_N(char, ne*bpi);
	elem[ie] = nv;
	memcpy(nv, px, ne*bpi);}

    else
       elem[ie] = px;

    return((FIXNUM) TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMMTOP - make a PM_mesh_topology */

FIXNUM F77_FUNC(pmmtop, PMMTOP)(FIXNUM *pnd, FIXNUM *pnc, FIXNUM *pbp,
				FIXNUM *pbnd)
   {int i, j, n, nd, ndp;
    int *nc, *nbp;
    long **bnd, *pbd;
    FIXNUM rv;
    FIXNUM *pbs;
    PM_mesh_topology *mt;
    
    nd  = *pnd;
    ndp = nd + 1;

/* setup the number of cells array */
    nc = CMAKE_N(int, ndp);
    for (i = 0; i < ndp; i++)
        nc[i] = pnc[i];

/* setup the number of boundary parameters array */
    nbp = CMAKE_N(int, ndp);
    for (i = 0; i < ndp; i++)
        nbp[i] = pbp[i];

    pbs = pbnd;
    bnd = CMAKE_N(long *, ndp);
    for (i = 1; i < ndp; i++)
        {n = nbp[i]*nc[i];

	 bnd[i] = CMAKE_N(long, n);

         pbd = bnd[i];
	 for (j = 0; j < n; j++)
	     *pbd++ = *pbs++;};

    bnd[0] = NULL;

/* put it all together */
    mt = PM_make_topology(nd, nbp, nc, bnd);

    if (mt == NULL)
       rv = -1;

    else
       rv = SC_ADD_POINTER(mt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMEFPE - enable/disable FPE handling */

void F77_FUNC(pmefpe, PMEFPE)(FIXNUM *pflg, PFSignal_handler hnd)
   {int flg;

    flg = *pflg;

    PM_enable_fpe_n(flg, hnd, NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMFPTF - classify float type wrt NaN */

FIXNUM F77_FUNC(pmfptf, PMFPTF)(float *pf)
   {FIXNUM rv;

    rv = PM_fp_typef(*pf);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMFPTD - classify double type wrt NaN */

FIXNUM F77_FUNC(pmfptd, PMFPTD)(double *pd)
   {FIXNUM rv;

    rv = PM_fp_typed(*pd);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMFXNF - fix float NaNs */

FIXNUM F77_FUNC(pmfxnf, PMFXNF)(FIXNUM *pn, float *pf, FIXNUM *pmsk, double *pv)
   {int msk;
    long n;
    FIXNUM rv;
    float v;

    n   = *pn;
    msk = *pmsk;
    v   = *pv;

    rv = PM_fix_nanf(n, pf, msk, v);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMCNNF - count float NaNs */

FIXNUM F77_FUNC(pmcnnf, PMCNNF)(FIXNUM *pn, float *pf, FIXNUM *pmsk)
   {int msk;
    long n;
    FIXNUM rv;

    n   = *pn;
    msk = *pmsk;

    rv = PM_count_nanf(n, pf, msk);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMFXND - fix double NaNs */

FIXNUM F77_FUNC(pmfxnd, PMFXND)(FIXNUM *pn, double *pd,
				FIXNUM *pmsk, double *pv)
   {int msk;
    long n;
    FIXNUM rv;
    double v;

    n   = *pn;
    msk = *pmsk;
    v   = *pv;

    rv = PM_fix_nand(n, pd, msk, v);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMCNND - count double NaNs */

FIXNUM F77_FUNC(pmcnnd, PMCNND)(FIXNUM *pn, double *pd, FIXNUM *pmsk)
   {int msk;
    long n;
    FIXNUM rv;

    n   = *pn;
    msk = *pmsk;

    rv = PM_count_nand(n, pd, msk);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
