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

/* PD_WRT_CURVE_ALT - Write an ULTRA curve as specified by: a label, LABL;
 *                  - number of points, N; x values X; and y values Y.
 *                  - The curve is written out to the PDBfile FILE as
 *                  - as the SIC'th curve in the file.
 *                  - Return TRUE if successful and FALSE otherwise.
 *
 * #bind PD_wrt_curve_alt fortran() python()
 */

int PD_wrt_curve_alt(PDBfile *file ARG(,,cls), const char *labl,
		     int n, double *x, double *y, int *sic)
   {int rv;

    rv = PD_wrt_pdb_curve(file, labl, n, x, y, *sic);

/* increment the curve count if no error */
    if (rv)
       (*sic)++;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRT_CURVE_Y_ALT - Write an ULTRA curve as specified by: a label, LABL;
 *                    - number of points, N; y values, Y; and x values
 *                    - from curve IX in the file.
 *                    - The curve is written out to the PDBfile FILE as
 *                    - as the SIC'th curve in the file.
 *                    - NOTE: no checking is done to see whether the number
 *                    - of points for the specified x values is correct.
 *                    - Return TRUE if successful and FALSE otherwise.
 *
 * #bind PD_wrt_curve_y_alt fortran() python()
 */

int PD_wrt_curve_y_alt(PDBfile *file ARG(,,cls), const char *labl,
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

static PM_set *_PD_build_set(int *ai, double *ad, const char *sname)
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

    set = PM_mk_set(sname, G_DOUBLE_S, FALSE,
		    ne, nd, nde, maxes, elem,
		    PM_gs.fp_opers, NULL,
		    NULL, NULL, NULL, NULL, NULL, NULL,
		    NULL);

    return(set);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRT_SET - Write a PM_set into a PDBfile FILE.
 *            - The set information DNAME, ADP, ADM is structured
 *            - as follows:
 *            -
 *            - DNAME : the FORTRAN version of the set name left justified
 *            -
 *            - ADP[1]               : the number of characters in dname
 *            - ADP[2]               : the dimensionality of the set - nd
 *            - ADP[3]               : the dimensionality of the elements - nde
 *            - ADP[4]               : the number of elements in the set - ne
 *            - ADP[5] ... ADP[5+nd] : the sizes in each dimension
 *            -
 *            - ADM[1]      - ADM[ne]          : values of first component of
 *            -            .                     elements
 *            -            .
 *            -            .
 *            -
 *            - ADM[nde*ne] - ADM[nde*ne + ne] : values of nde'th component of
 *            -                                  elements
 *            - where nd is the number of dimensions of the set,
 *            - ne is the number of elements, and nde is the number of
 *            - dimensions of each element.
 *            -
 *            - Return TRUE if successful and FALSE otherwise.
 *
 * #bind PD_wrt_set fortran() python()
 */

int PD_wrt_set(PDBfile *file ARG(,,cls),
	       const char *dname, int *adp, double *adm)
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

/* PD_WRT_MAP - WRITE an PM_mapping into a PDBfile FILE.
 *            - The domain information DNAME, ADP, ADM and range 
 *            - information RNAME, ARP, ARM are structured the same
 *            - and are as follows:
 *            -
 *            - DNAME : the FORTRAN version of the set name left justified
 *            -
 *            - ADP[1]              : the number of characters in dname
 *            - ADP[2]              : the dimensionality of the set - nd
 *            - ADP[3]              : the dimensionality of the elements - nde
 *            - ADP[4]              : the number of elements in the set - ne
 *            - ADP[5] ... ADP[5+nd] : the sizes in each dimension
 *            -
 *            - ADM[1]      - ADM[ne]          : values of first component of
 *            -            .                     elements
 *            -            .
 *            -            .
 *            -
 *            - ADM[nde*ne] - ADM[nde*ne + ne] : values of nde'th component of
 *            -                                  elements
 *            - where nd is the number of dimensions of the set,
 *            - ne is the number of elements, and nde is the number of
 *            - dimensions of each element.
 *            -
 *            - Return TRUE if successful and FALSE otherwise.
 *
 * #bind PD_wrt_map fortran() python()
 */

int PD_wrt_map(PDBfile *file ARG(,,cls),
	       const char *dname, int *adp, double *adm,
	       const char *rname, int *arp, double *arm,
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

/* PD_WRT_MAP_RAN - Write a PM_mapping into a PDBfile FILE.
 *                - Only the range part of the mapping is given,
 *                - but additional information is given in INFO such as
 *                - an existence map and centering.
 *                - The mapping is written out to the PDBfile FILE as
 *                - as the SIM'th mapping in the file.
 *                - The domain (common to many mappings) is written separately.
 *                - The domain name is the only part of the domain specified.
 *                - The range information (RNAME, ARP, ARM) is structured
 *                - as follows:
 *                -
 *                - RNAME : the FORTRAN version of the set name left justified
 *                -
 *                - ARP[1]               : the number of characters in RNAME
 *                - ARP[2]               : the dimensionality of the set - nd
 *                - ARP[3]               : the dimensionality of the elements - nde
 *                - ARP[4]               : the number of elements in the set - ne
 *                - ARP[5] ... ARP[5+nd] : the sizes in each dimension
 *                -
 *                - ARM[1]      - ARM[ne]          : values of first component of
 *                -            .                     elements
 *                -            .
 *                -            .
 *                -            .
 *                -
 *                - ARM[nde*ne] - ARM[nde*ne + ne] : values of nde'th component of
 *                -                                 elements
 *                - where nd is the number of dimensions of the set,
 *                - ne is the number of elements, and nde is the number of
 *                - dimensions of each element.
 *                -
 *                - Return TRUE if successful and FALSE otherwise.
 *
 * #bind PD_wrt_map_ran fortran() python()
 */

int PD_wrt_map_ran(PDBfile *file ARG(,,cls), const char *dname,
		   const char *rname, int *arp, double *arm,
		   pcons *info, int *sim)
   {int i;
    int rv;
    char s[MAXLINE], name[MAXLINE];
    PM_centering cent, *pcent;
    PM_mapping *f, *lnk, *pm;
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

    lnk = f;
    if (info != NULL)

/* check for mappings to overlay */
       {for (i = 0; TRUE; i++)
	    {snprintf(name, MAXLINE, "OVERLAY[%d]", i);
	     pm = (PM_mapping *) SC_assoc(info, name);
	     if (pm == NULL)
	        break;

	     else
	        {lnk->next = pm;
		 lnk       = pm;
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

		 lnk->next = pm;
		 lnk       = pm;
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

