/*
 * PDGS.C - gather/scatter calls for PDB
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"
#include "scope_mpi.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_N_ELEMENTS - return the number of elements in the whole array */

long _PD_n_elements(long *ind, int mn, int mx)
   {long i, j, ne;

    ne = 1;
    for (i = mn; i < mx; i++)
        {j   = 4*i;
	 ne *= ind[j+3];};

    return(ne);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_N_SPEC_ELEMENTS - return the number of elements specified */

long _PD_n_spec_elements(int n, long *ind)
   {long i, j, ne;
    long ia, ib, di;

    ne = 1;
    for (i = 0; i < n; i++)
        {j   = 4*i;
	 ia  = ind[j];
	 ib  = ind[j+1];
	 di  = ind[j+2];
	 ne *= (1 + (ib - ia)/di);};

    return(ne);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_HYPER_GATHER - coalesce part of a dimensioned array in memory
 *                  - into a contiguous array in memory
 *                  - return the number of contiguous items
 */

static int _PD_hyper_gather(char *dst, char *src,
			    int is, int ns, long *sind, int bpi, int mo)
   {int ng, mn, mx;
    long i, j;
    long offset, stride, start, stop, step;
    char *pd, *ps;

    if (mo == COLUMN_MAJOR_ORDER)
       {mn = 0;
	mx = is;}
    else
       {mn = is + 1;
	mx = ns;};

    stride  = _PD_n_elements(sind, mn, mx);
    stride *= bpi;

/* for each index specification compute the range and recurse */
    start = stride*sind[4*is];
    stop  = stride*sind[4*is + 1];
    step  = stride*sind[4*is + 2];

    ng = 0;

/* at the bottom of the recursion do the actual operations */
    if (is == ns - 1)
       {for (i = start, j = 0; i <= stop; i += step, j += bpi)
	    {memcpy(dst + j, src + i, bpi);
	     ng += bpi;};}

    else
       {for (offset = start; offset <= stop; offset += step)
	    {pd  = dst + ng;
	     ps  = src + offset;
	     ng += _PD_hyper_gather(pd, ps, is+1, ns, sind,
				    bpi, mo);};};

    return(ng);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_HYPER_SCATTER - distribute a contiguous array in memory out to
 *                   - specified locations in a dimensioned array in memory
 *                   - return the number of items moved
 */

static int _PD_hyper_scatter(char *dst, int id, int nd, long *dind, char *src,
			     int bpi, int mo)
   {int ng, mn, mx;
    long i, j;
    long offset, stride, start, stop, step;
    char *pd, *ps;

    if (mo == COLUMN_MAJOR_ORDER)
       {mn = 0;
	mx = id;}
    else
       {mn = id + 1;
	mx = nd;};

    stride  = _PD_n_elements(dind, mn, mx);
    stride *= bpi;

/* for each index specification compute the range and recurse */
    start = stride*dind[4*id];
    stop  = stride*dind[4*id + 1];
    step  = stride*dind[4*id + 2];

    ng = 0;

/* at the bottom of the recursion do the actual operations */
    if (id == nd - 1)
       {for (i = start, j = 0; i <= stop; i += step, j += bpi)
	    {memcpy(dst + i, src + j, bpi);
	     ng += bpi;};}

    else
       {for (offset = start; offset <= stop; offset += step)
	    {pd  = dst + offset;
	     ps  = src + ng;
	     ng += _PD_hyper_scatter(pd, id+1, nd, dind,
				     ps, bpi, mo);};};

    return(ng);}

/*--------------------------------------------------------------------------*/

/*                             GATHER ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* PD_GATHER_AS - read part of an entry from the PDB file pointed to by
 *              - the symbol table into the location pointed to by VR
 *              - IND contains one triplet of long ints per variable
 *              - dimension specifying start, stop, and step for the index
 *              - return the number of item successfully read
 *              -
 *              - NOTE: the entry MUST be an array (either a static
 *              - array or a pointer)
 *              -
 *              - NOTE: VR must be a pointer to an object with the type
 *              - of the object associated with NAME (PDBLib will
 *              - allocated space if necessary)!
 */

int PD_gather_as(PDBfile *file, char *name, char *type,
		 void *vr, long *sind, int ndst, long *dind)
   {int nr, ng, ni, bpi, mo;
    char *tv;
    syment *ep;

    ng = 0;
    if (file != NULL)
       {if (type == NULL)
	   {ep   = PD_inquire_entry(file, name, FALSE, NULL);
	    type = PD_entry_type(ep);};

	mo  = PD_get_major_order(file);
	bpi = _PD_lookup_size(type, file->host_chart);
	ni  = _PD_n_spec_elements(ndst, dind);
	tv  = FMAKE_N(char, ni*bpi, "PD_GATHER_AS:tv");

	nr = PD_read_as_alt(file, name, type, tv, sind);
	SC_ASSERT(nr > 0);

	ng = _PD_hyper_scatter((char *) vr, 0, ndst, dind, (char *) tv, bpi, mo);

	SFREE(tv);};

    return(ng);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_GATHER - read a variable from a file with one dimension expression
 *           - into a variable in memory with another dimension expression
 *           - read part of an entry from the PDB file pointed to by
 *           - IND contains one triplet of long ints per variable
 *           - dimension specifying start, stop, and step for the index
 *           - return the number of item successfully read
 *           -
 *           - NOTE: VR must be a pointer to an object with the type
 *           - given by TYPE (PDBLib will allocated space if necessary)!
 */

int PD_gather(PDBfile *file, char *name, void *vr, long *sind,
	      int ndst, long *dind)
   {int rv;

    rv = PD_gather_as(file, name, NULL, vr, sind, ndst, dind);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                             SCATTER ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* PD_SCATTER_AS - scatter an entry of type INTYPE to the PDB file, FILE
 *               - as type OUTTYPE
 *               - make an entry in the file's symbol table
 *               - return TRUE iff successful
 *               - the entry has name, NAME, ND dimensions, and the ranges
 *               - of the dimensions are given (min, max) pairwise in IND
 *               - if successful and otherwise
 *               - return FALSE
 *               -
 *               - NOTE: VR must be a pointer to an object with the type
 *               - given by TYPE!!!!
 */

int PD_scatter_as(PDBfile *file, char *name, char *intype, char *outtype,
		  void *vr, int nsrc, long *sind, int ndst, long *dind)
   {int ni, ng, bpi, mo, ret;
    char *tv;

    ret = FALSE;
    if (file != NULL)
       {mo  = PD_get_major_order(file);
	ni  = _PD_n_spec_elements(nsrc, sind);
	bpi = _PD_lookup_size(intype, file->host_chart);
	tv  = FMAKE_N(char, ni*bpi, "PD_SCATTER_AS:tv");

	ng = _PD_hyper_gather((char *) tv, (char *) vr, 0, nsrc, sind, bpi, mo);
	SC_ASSERT(ng == TRUE);

	ret = PD_write_as_alt(file, name, intype, outtype, tv, ndst, dind);

	SFREE(tv);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_SCATTER - write a variable to a file with one dimension expression
 *            - using a variable in memory with another dimension expression
 *            - the entry is named by NAME has ND dimensions and IND
 *            - contains the min and max (pairwise) of each dimensions
 *            - range
 *            - return the new syment if successful and NULL otherwise
 *            -
 *            - NOTE: VR must be a pointer to an object with the type
 *            - given by TYPE!!!!
 */

int PD_scatter(PDBfile *file, char *name, char *type, void *vr,
	       int nsrc, long *sind, int ndst, long *dind)
   {int rv;

    rv = PD_scatter_as(file, name, type, type, vr, nsrc, sind, ndst, dind);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

