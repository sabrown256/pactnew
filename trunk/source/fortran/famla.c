/*
 * FAMLA.C - FORTRAN interface to PML
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "fpact.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMSZFT - return the largest M such that 2^M <= N  */

FIXNUM FF_ID(pmszft, PMSZFT)(FIXNUM *sn)
   {FIXNUM rv;

    *sn = (FIXNUM) PM_next_exp_two((int) *sn);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMRFFT - perform an FFT on real data
 *        - this a wrapper for PM_fft_sc_real_data
 */

FIXNUM FF_ID(pmrfft, PMRFFT)(double *aoutyr, double *aoutyi, double *aoutx,
			     FIXNUM *sn, double *ainx, double *ainy,
			     double *sxn, double *sxx, FIXNUM *so)
   {int i, n, np, ordr;
    FIXNUM rv;
    double xmn, xmx;
    double *rx;
    complex *cy;

    n    = *sn;
    xmn  = *sxn;
    xmx  = *sxx;
    ordr = *so;

    rv = FALSE;

    if (PM_fft_sc_real_data(&cy, &rx, ainx, ainy, n, xmn, xmx, ordr) &&
	(cy != NULL) && (rx != NULL))
       {np = n + 1;
	for (i = 0; i < np; i++)
	    {aoutyr[i] = creal(cy[i]);
	     aoutyi[i] = cimag(cy[i]);
	     aoutx[i]  = rx[i];};

	CFREE(rx);
	CFREE(cy);

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMCFFT - perform an FFT on complex data
 *        - this a wrapper for PM_fft_sc_complex_data
 */

FIXNUM FF_ID(pmcfft, PMCFFT)(double *aoutyr, double *aoutyi, double *aoutx,
			     FIXNUM *sn, double *ainx, double *ainyr,
			     double *ainyi, double *sxn, double *sxx,
			     FIXNUM *sf, FIXNUM *so)
   {int i, n, np, flag, ordr;
    FIXNUM rv;
    double xmn, xmx;
    double *rx;
    complex *cy, *incy;

    n    = *sn;
    xmn  = *sxn;
    xmx  = *sxx;
    flag = *sf;
    ordr = *so;

    incy = CMAKE_N(complex, n);
    for (i = 0; i < n; i++)
        incy[i] = CMPLX(ainyr[i], ainyi[i]);

    if (!PM_fft_sc_complex_data(&cy, &rx, ainx, incy, n,
				xmn, xmx, flag, ordr))
       rv = FALSE;

    else
       {np = n + 1;
	for (i = 0; i < np; i++)
	    {aoutyr[i] = creal(cy[i]);
	     aoutyi[i] = cimag(cy[i]);
	     aoutx[i]  = rx[i];};

	CFREE(rx);
	CFREE(cy);
	CFREE(incy);

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMBSET - begin making a set */

FIXNUM FF_ID(pmbset, PMBSET)(FIXNUM *sncn, char *fname,
			     FIXNUM *snct, char *ftype,
			     FIXNUM *scp, FIXNUM *snd,
			     FIXNUM *snde, FIXNUM *amx, FIXNUM *stp,
			     FIXNUM *sinxt)
   {int i, id, cp, nd, nde, bpi;
    int *maxes;
    long ne, tp, d;
    FIXNUM rv;
    char name[MAXLINE], type[MAXLINE], bf[MAXLINE], *topt, *s;
    void **elem;
    pcons *info;
    PM_set *set, *next;
    PM_mesh_topology *top;

    SC_FORTRAN_STR_C(name, fname, *sncn);
    SC_FORTRAN_STR_C(type, ftype, *snct);

    next = SC_GET_POINTER(PM_set, *sinxt);
    cp   = *scp;
    nd   = *snd;
    nde  = *snde;
    tp   = *stp;

    if (tp == -1)
       {maxes = CMAKE_N(int, nd);
	ne    = 1L;
        for (i = 0; i < nd; i++)
	    {d = amx[i];
	     maxes[i] = d;
	     ne *= d;};
        topt = NULL;
	top  = NULL;}

    else
       {topt  = PM_MESH_TOPOLOGY_P_S;
        top   = SC_GET_POINTER(PM_mesh_topology, *stp);
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
	   {bpi = 0;

	    id = SC_type_id(bf, FALSE);
	    if ((id == SC_BOOL_I) || (SC_is_type_num(id) == TRUE))
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

FIXNUM FF_ID(pmeset, PMESET)(FIXNUM *siset)
   {FIXNUM rv;
    PM_set *set;

    set = SC_GET_POINTER(PM_set, *siset);

    PM_find_extrema(set);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMASET - add a component to a set */

FIXNUM FF_ID(pmaset, PMASET)(FIXNUM *siset, FIXNUM *sie, void *ax)
   {int ie, cp;
    FIXNUM rv;
    pcons *info;
    void **elem;
    PM_set *set;
    char *s;

    set  = SC_GET_POINTER(PM_set, *siset);
    ie   = *sie - 1;
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
	memcpy(nv, ax, ne*bpi);}

    else
       elem[ie] = ax;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMMTOP - make a PM_mesh_topology */

FIXNUM FF_ID(pmmtop, PMMTOP)(FIXNUM *snd, FIXNUM *anc, FIXNUM *abp,
			     FIXNUM *abnd)
   {int i, j, n, nd, ndp;
    int *nc, *nbp;
    long **bnd, *pbd;
    FIXNUM rv;
    FIXNUM *pbs;
    PM_mesh_topology *mt;
    
    nd  = *snd;
    ndp = nd + 1;

/* setup the number of cells array */
    nc = CMAKE_N(int, ndp);
    for (i = 0; i < ndp; i++)
        nc[i] = anc[i];

/* setup the number of boundary parameters array */
    nbp = CMAKE_N(int, ndp);
    for (i = 0; i < ndp; i++)
        nbp[i] = abp[i];

    pbs = abnd;
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

void FF_ID(pmefpe, PMEFPE)(FIXNUM *sflg, PFSignal_handler hnd)
   {int flg;

    flg = *sflg;

    PM_enable_fpe_n(flg, hnd, NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMFPTF - classify float type wrt NaN */

FIXNUM FF_ID(pmfptf, PMFPTF)(float *sf)
   {FIXNUM rv;

    rv = PM_fp_typef(*sf);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMFPTD - classify double type wrt NaN */

FIXNUM FF_ID(pmfptd, PMFPTD)(double *sd)
   {FIXNUM rv;

    rv = PM_fp_typed(*sd);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMFXNF - fix float NaNs */

FIXNUM FF_ID(pmfxnf, PMFXNF)(FIXNUM *sn, float *sf, FIXNUM *smsk, double *sv)
   {int msk;
    long n;
    FIXNUM rv;
    float v;

    n   = *sn;
    msk = *smsk;
    v   = *sv;

    rv = PM_fix_nanf(n, sf, msk, v);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMCNNF - count float NaNs */

FIXNUM FF_ID(pmcnnf, PMCNNF)(FIXNUM *sn, float *sf, FIXNUM *smsk)
   {int msk;
    long n;
    FIXNUM rv;

    n   = *sn;
    msk = *smsk;

    rv = PM_count_nanf(n, sf, msk);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMFXND - fix double NaNs */

FIXNUM FF_ID(pmfxnd, PMFXND)(FIXNUM *sn, double *ad,
			     FIXNUM *smsk, double *sv)
   {int msk;
    long n;
    FIXNUM rv;
    double v;

    n   = *sn;
    msk = *smsk;
    v   = *sv;

    rv = PM_fix_nand(n, ad, msk, v);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMCNND - count double NaNs */

FIXNUM FF_ID(pmcnnd, PMCNND)(FIXNUM *sn, double *ad, FIXNUM *smsk)
   {int msk;
    long n;
    FIXNUM rv;

    n   = *sn;
    msk = *smsk;

    rv = PM_count_nand(n, ad, msk);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMRNDF - Fortran interface to PM_random_48 */

double FF_ID(pmrndf, PMRNDF)(double *px)
   {double rv;

    rv = PM_random_48(*px);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMRNDF - Fortran interface to PM_random_s */

double FF_ID(pmrnds, PMRNDS)(double *px)
   {double rv;

    rv = PM_random_s(*px);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMCSMA - Fortran callable wrapper for PM_checksum_array */

FIXNUM FF_ID(pmcsma, PMCSMA)(void *arr, FIXNUM *sni, FIXNUM *sbpi,
			     FIXNUM *snbts)
   {int ni, bpi, nbts;
    FIXNUM cs;

    ni   = *sni;
    bpi  = *sbpi;
    nbts = *snbts;

    cs = PM_checksum_array(arr, ni, bpi, nbts);

    return(cs);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
