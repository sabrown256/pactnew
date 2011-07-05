/*
 * PDMAP.C - sugar to write curves, mappings, sets, and images
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRT_CURVE_ALT - write an ULTRA curve into a PDB file
 *
 * #bind PD_wrt_curve_alt fortran() python(wrt_curve_alt)
 */

int PD_wrt_curve_alt(PDBfile *file ARG(,,cls), char *labl,
		     int n, double *x, double *y, int *sic)
   {int rv;

    rv = PD_wrt_pdb_curve(file, labl, n, x, y, *sic);

/* increment the curve count if no error */
    if (rv)
       (*sic)++;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRT_CURVE_Y_ALT - write the y values for an ULTRA curve into a PDB file
 *
 * #bind PD_wrt_curve_y_alt fortran() python(wrt_curve_y_alt)
 */

int PD_wrt_curve_y_alt(PDBfile *file ARG(,,cls), char *labl,
		       int n, int ix, double *y, int *sic)
   {int rv;

    rv = PD_wrt_pdb_curve_y(file, labl, n, ix, y, *sic);

/* increment the curve count if no error */
    if (rv)
       (*sic)++;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BUILD_SET - build and return a set from FORTRAN type information */

static PM_set *_PD_build_set(int *ai, double *ad, char *sname)
   {int i, j, nd, nde, ne;
    int *maxes;
    void **elem;
    double *data;
    double tmp;
    PM_set *set;

    nd  = ai[0];
    nde = ai[1];
    ne  = ai[2];

    maxes = CMAKE_N(int, nd);
    for (i = 0; i < nd; i++)
        maxes[i] = (int) ai[i+3];

    elem = CMAKE_N(void *, nde);
    for (i = 0; i < nde; i++)
        {data    = CMAKE_N(double, ne);
         elem[i] = (void *) data;
         for (j = 0; j < ne; j++)
             {tmp = *ad++;
              *data++ = tmp;};};

    set = PM_mk_set(sname, SC_DOUBLE_S, FALSE,
		    ne, nd, nde, maxes, elem,
		    PM_fp_opers, NULL,
		    NULL, NULL, NULL, NULL, NULL, NULL,
		    NULL);

    return(set);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRT_SET - write a PM_set into a PDB file
 *            - the set information (dname, dp, dm) is structured
 *            - as follows:
 *            -
 *            - dname : the FORTRAN version of the set name left justified
 *            -
 *            - dp[1]              : the number of characters in dname
 *            - dp[2]              : the dimensionality of the set - nd
 *            - dp[3]              : the dimensionality of the elements - nde
 *            - dp[4]              : the number of elements in the set - ne
 *            - dp[5] ... dp[5+nd] : the sizes in each dimension
 *            -
 *            - dm[1]      - dm[ne]          : values of first component of
 *            -                                elements
 *            -            .
 *            -            .
 *            -            .
 *            -
 *            - dm[nde*ne] - dm[nde*ne + ne] : values of nde'th component of
 *            -                                elements
 *
 * #bind PD_wrt_set fortran() python(wrt_set)
 */

int PD_wrt_set(PDBfile *file ARG(,,cls),
	       char *dname, int *adp, double *adm)
   {int rv;
    PM_set *set;

    rv = TRUE;

    if ((SC_hasharr_lookup(file->chart, "PM_set") == NULL) &&
	(!PD_def_mapping(file)))
       rv = FALSE;

    else
       {set = _PD_build_set(adp+1, adm, dname);

/* disconnect the function pointers or undefined structs/members */
	set->opers = NULL;

	if (!PD_put_set(file, set))
	   rv = FALSE;
	else
	   PM_rel_set(set, FALSE);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRT_MAP - write an PM_mapping into a PDB file
 *            - the domain information (dname, dp, dm) and range 
 *            - information (rname, rp, rm) are structured the same
 *            - and are as follows:
 *            -
 *            - dname : the FORTRAN version of the set name left justified
 *            -
 *            - dp[1]              : the number of characters in dname
 *            - dp[2]              : the dimensionality of the set - nd
 *            - dp[3]              : the dimensionality of the elements - nde
 *            - dp[4]              : the number of elements in the set - ne
 *            - dp[5] ... dp[5+nd] : the sizes in each dimension
 *            -
 *            - dm[1]      - dm[ne]          : values of first component of
 *            -                                elements
 *            -            .
 *            -            .
 *            -            .
 *            -
 *            - dm[nde*ne] - dm[nde*ne + ne] : values of nde'th component of
 *            -                                elements
 *
 * #bind PD_wrt_map fortran() python(wrt_map)
 */

int PD_wrt_map(PDBfile *file ARG(,,cls),
	       char *dname, int *adp, double *adm,
	       char *rname, int *arp, double *arm,
	       int *sim)
   {int rv;
    char s[MAXLINE];
    PM_mapping *f;
    PM_set *domain, *range;

    rv = TRUE;

    if ((SC_hasharr_lookup(file->chart, "PM_mapping") == NULL) &&
	(!PD_def_mapping(file)))
       rv = FALSE;

    else
       {domain = _PD_build_set(adp+1, adm, dname);

	range  = _PD_build_set(arp+1, arm, rname);

	snprintf(s, MAXLINE, "%s->%s", domain->name, range->name);
	f = PM_make_mapping(s, PM_LR_S, domain, range, N_CENT, NULL);

/* disconnect the function pointers or undefined structs/members */
	f->domain->opers = NULL;
	f->range->opers  = NULL;

	if (!PD_put_mapping(file, f, (*sim)++))
	   rv = FALSE;
	else
	   PM_rel_mapping(f, TRUE, TRUE);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRT_MAP_RAN - write a PM_mapping into a PDB file
 *                - only the range part of the mapping is given
 *                - but additional information is given in INFO such as
 *                - an existence map and centering are included
 *                - the domain (common to many mappings) is written separately
 *                -
 *                - the domain name is the only part of the domain specified
 *                -
 *                - the range information (rname, rp, rm) is structured
 *                - as follows:
 *                -
 *                - rname : the FORTRAN version of the set name left justified
 *                -
 *                - rp[1]              : the number of characters in rname
 *                - rp[2]              : the dimensionality of the set - nd
 *                - rp[3]              : the dimensionality of the elements - nde
 *                - rp[4]              : the number of elements in the set - ne
 *                - rp[5] ... rp[5+nd] : the sizes in each dimension
 *                -
 *                - rm[1]      - rm[ne]          : values of first component of
 *                -                                elements
 *                -            .
 *                -            .
 *                -            .
 *                -
 *                - rm[nde*ne] - rm[nde*ne + ne] : values of nde'th component of
 *                -                                elements
 *
 * #bind PD_wrt_map_ran fortran() python(wrt_map_ran)
 */

int PD_wrt_map_ran(PDBfile *file ARG(,,cls), char *dname,
		   char *rname, int *arp, double *arm,
		   pcons *info, int *sim)
   {int i;
    int rv;
    char s[MAXLINE], name[MAXLINE];
    PM_centering cent, *pcent;
    PM_mapping *f, *link, *pm;
    PM_set *range, *bs;
    pcons *alst;

    rv = TRUE;

    if (SC_hasharr_lookup(file->chart, "PM_mapping") == NULL)
       {if (!PD_def_mapping(file))
           return(FALSE);};

    pcent = (PM_centering *) SC_assoc(info, "CENTERING");
    cent  = (pcent == NULL) ? N_CENT : *pcent;
    range = _PD_build_set(arp+1, arm, rname);

    snprintf(s, MAXLINE, "%s->%s", dname, range->name);
    f = PM_make_mapping(s, PM_LR_S, NULL, range, cent, NULL);

/* disconnect the function pointers or undefined structs/members */
    f->range->opers = NULL;

    link = f;
    if (info != NULL)

/* check for mappings to overlay */
       {for (i = 0; TRUE; i++)
	    {snprintf(name, MAXLINE, "OVERLAY[%d]", i);
	     pm = (PM_mapping *) SC_assoc(info, name);
	     if (pm == NULL)
	        break;

	     else
	        {link->next = pm;
		 link       = pm;
		 SC_mark(pm, 1);

		 info = SC_rem_alist(info, name);};};

/* check for boundary sets to overlay */
	for (i = 0; TRUE; i++)
	    {snprintf(name, MAXLINE, "BND[%d]", i);
	     bs = (PM_set *) SC_assoc(info, name);
	     if (bs == NULL)
	        break;

	     else
	        {pm = PM_make_mapping(name, PM_AC_S, bs, NULL,
				      N_CENT, NULL);

		 link->next = pm;
		 link       = pm;
		 SC_mark(pm, 1);

		 info = SC_rem_alist(info, name);};};

/* append all other info to the mapping's info list */
	alst   = (pcons *) f->map;
	alst   = SC_append_alist(alst, info);
	f->map = (void *) alst;};

    if (!PD_put_mapping(file, f, (*sim)++))
       rv = FALSE;
    else
       PM_rel_mapping(f, TRUE, TRUE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#if 0

#define PG_SPACEDM 3

/* PD_WRT_IMAGE - write a PG_image into a PDB file
 *
 * #bind PD_wrt_image fortran()
 */

int PD_wrt_image(PDBfile *file, char *name, double *dbx, double *lbx,
		 double *data, int *sim)
   {int i, n, kx, lx, k, l, rv;
    double z;
    double rbx[PG_SPACEDM];
    double *d;
    PD_image *im;

    rv = TRUE;

    if ((SC_hasharr_lookup(file->chart, "PG_image") == NULL) &&
	(!PD_def_mapping(file)))
       rv = FALSE;

    else
       {kx = dbx[1] - dbx[0] + 1;
	lx = dbx[3] - dbx[2] + 1;
	n  = kx*lx;
	d  = CMAKE_N(double, n);

	rbx[1] = -HUGE;
	rbx[0] =  HUGE;
	for (l = dbx[2]; l <= dbx[3]; l++)
	    for (k = dbx[0]; k <= dbx[1]; k++)
	        {i      = (l - lbx[2])*(lbx[1] - lbx[0] + 1) + k - lbx[0];
		 z      = data[i];
		 rbx[1] = max(rbx[1], z);
		 rbx[0] = min(rbx[0], z);
		 d[i]   = z;}

	im = PD_make_image_n(name, SC_DOUBLE_P_S, d,
			     2, WORLDC, dbx, rbx,
			     kx, lx, 8, NULL);

	if (!PD_put_image(file, im, *sim))
	   rv = FALSE;
	else
	   PD_rel_image(im);};

    return(rv);}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

