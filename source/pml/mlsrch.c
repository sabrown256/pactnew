/*
 * MLSRCH.C - search routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

#include "mlsrch.h"

SC_THREAD_LOCK(PM_search_lock); /* Lock around initialization */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MINMAX - return the min, max and and index min, max
 *           - for the given array
 */

void PM_minmax(void *p, int n, void *pn, void *px, int *imin, int *imax)
   {int id;

    id = SC_arrtype(p, -1);
    if (SC_is_type_prim(id) == FALSE)
       id = SC_DOUBLE_I;

    if (PM_min_max_fnc[id] != NULL)
       PM_min_max_fnc[id](p, n, pn, px, imin, imax);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MAXMIN - return the max and min value of the given array */

void PM_maxmin(double *xp, double *xmin, double *xmax, int n)
   {int i1, i2;

    PM_minmax(xp, n, xmin, xmax, &i1, &i2);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DMINMAX - print the max and min value of the given SCORE allocated array */

void dminmax(void *p, int n)
   {int ix[2];
    double x[2];

    PM_minmax(p, n, &x[0], &x[1], &ix[0], &ix[1]);

    io_printf(stdout, "Min = %12g at %d\n", x[0], ix[0]);
    io_printf(stdout, "Max = %12g at %d\n", x[1], ix[1]);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MAX_VALUE - return the maximum value from the given array of doubles */

double PM_max_value(double *p, int imin, int n)
   {int i;
    double u, v;

    v = -HUGE;

    if (p != NULL)
       {p += imin;
        for (i = 0; i < n; i++)
            {u = *p++;
             v = max(v, u);};};

    return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_INDEX_MIN - return the index of the minimum value from the
 *              - given array of doubles
 */

int PM_index_min(double *p, int n)
   {int i, j;
    double u, v;

    j = -1;

    if (p != NULL)
       {v = HUGE;
        for (i = 0; i < n; i++)
            {u = *p++;
             v = min(v, u);
             if (u == v)
                j = i;};};

    return(j);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FIND_INDEX - return the index into ordered array P such that 
 *               - P[i-1] < F <= P[i]
 *               - return 0 if F <= P[0] and N if P[N-1] < F
 */

int PM_find_index(void *p, double f, int n)
   {int id, indx;

    indx = 0;

    id = SC_arrtype(p, -1);
    if (SC_is_type_prim(id) == FALSE)
       id = SC_DOUBLE_I;

    if (PM_find_index_fnc[id] != NULL)
       indx = PM_find_index_fnc[id](p, f, n);

    return(indx);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SEARCH - this routine computes an array of index locations INDX of the
 *           - array XV in the table Y such that
 *           -             Y[INDX[I]-1] < X[I] <= Y[INDX[I]]
 *           - Given an increasing table Y[0] < Y[1] < ... < Y[NY-1],
 *           - interval indices 0, 1, ..., NY-1 correspond to (Y[0],Y[1]],
 *           - (Y[1],Y[2]], ..., (Y[NY-2],Y[NY-1]]
 *           -   
 *           - It is the user's responsibility to ensure that the values
 *           - of X gathered are all interior to the table of Y's; that is,
 *           - Y[0] < X[I] <= Y[NY-1], 0 <= I < NY
 *           -
 *           - Input:
 *           -    NX   = the number of elements in the array X
 *           -    X    = an array of length NX whose locations in the
 *           -           Y table are to be determined
 *           -    NY   = the number of elements in the table Y
 *           -    Y    = a table of increasing elements to be searched
 *           -    OFFS = an offset that facilitates searching part of Y
 *           -
 *           - Output:
 *           -    INDX = an array of length NX of integers locating the position
 *           -           of each element of X in the Y table
 */

void PM_search(int nx, double *x, int ny, double *y, int offs, int *indx)
   {long ix, iy, my;
    PM_smp_state *ma;
    
    ma = _PM_get_state(-1);

    y += offs;

    my       = ny - 1L;
    ma->last = min(ma->last, my);
    ma->last = max(0L, ma->last);

    for (ix = 0; ix < nx; ix++)
        {iy = _PM_find_index_fast_fnc[SC_DOUBLE_I](y, x[ix], ny, &(ma->last));

	 indx[ix] = iy + offs;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_PERMUTE - given a list of dimension specifications return a permuted
 *             - set of them.
 */

void _PM_permute(long *region, long **strtind, long noffs)
   {long ndims, i, j, k, count, stride = 1, start, end;

    ndims = region[0];

    for (i = 0; i < noffs; i++)
        strtind[i][0] = region[1];

    for (i = 1; i < ndims; i++)
        {k = start = region[2*i+1];
         end       = region[2*i+2];
         stride = (i == 1) ? 1 : 
                          (region[2*i] - region[2*i-1] + 1) * stride; 
         count = 1;
         for (j = 0; j < noffs; j++)
             {strtind[j][i] = k;
              if ((count + 1) > stride)
                 {count = 1;
                  k = (k + 1) <= end ? (k + 1) : start;}
              else
                  count++;};}
    
#if 0
    printf("The permuted start indices are:\n");
    for (i = 0; i < noffs; i++)
        for (j = 0; j < ndims; j++)
            {printf("%2ld ", strtind[i][j]);
             if (j == (ndims - 1))
	        printf("\n");}
#endif

    return;}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_OFFSETS - given a list of multi-dimensional index specifications
 *               generate a list of 1-d offsets
 */

void _PM_offsets(long *dims, long **strtind, long *start, long *stride,
		 long noffs)
   {long *min_index, ndims;
    long i, j;

    ndims = dims[0];
    min_index = CMAKE_N(long, ndims);

    for (i = 1; i <= ndims; i++)
        min_index[i-1] = dims[2*i-1];

    for (i = 0; i < noffs; i++)
        {start[i] = 0;
         for (j = 0; j < ndims; j++)
	    {start[i] += (strtind[i][j] - min_index[j]) 
	                 * stride[j];};}

    CFREE(min_index);

    return;} 

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_HYPER_INDICES - calculate starting offsets and chunk size
 *                   - for carving a sub-array out of an array
 */

long _PM_hyper_indices(long *dims, long *reg, long **pstart, long *chunk)
   {long ndims, *start, *stride, **strtind;
    long i, j, noffs;

    noffs = -1;
    start = NULL;
    if ((dims != NULL) && (reg != NULL))
       {ndims = dims[0];

	stride = CMAKE_N(long, ndims);

/* compute stride for each dimension */
	stride[0] = 1;
	for (i = 1, j = 1; j < ndims; i += 2, j++)
	    stride[j] = (dims[i+1] - dims[i] + 1) * stride[j-1];
    
/* compute number of start offsets*/
	noffs = 1;
	for (i = ndims * 2; i > 2; i -= 2)
	    noffs *= reg[i] - reg[i-1] + 1;

/* allocate the start array */
	start = CMAKE_N(long, noffs);

/* allocate temporary to store permuted indices */
	strtind = CMAKE_N(long *, noffs);

	ndims = reg[0];
	for (i = 0; i < noffs; i++)
	    strtind[i] = CMAKE_N(long, ndims);

/* permute the indices */
	_PM_permute(reg, strtind, noffs);

/* calculate 1-d starting offsets */
	_PM_offsets(dims, strtind, start, stride, noffs);

	*chunk = reg[2] - reg[1] + 1;

/* free up memory */
	CFREE(stride)

	  for (i = 0; i < noffs; i++)
	      CFREE(strtind[i]);

	CFREE(strtind);};
 
    if (*pstart != NULL)
       *pstart = start;
    else
       CFREE(start);

    return(noffs);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SUB_ARRAY - given array in with dimensions dims, with bpi
 *                bytes per item, return a sub-array out, specified
 *                by reg.
 */

void PM_sub_array(void *in, void *out, long *dims, long *reg, long bpi)
   {long *start, chunk, i, n;
    char *pout, *pin;

    n = _PM_hyper_indices(dims, reg, &start, &chunk);

    pout = (char *)out;

    for (i = 0; i < n; i++)
        {pin = (char *)in + (start[i] * bpi);
         memcpy(pout, pin, chunk*bpi);
         pout = pout + chunk * bpi;}

    CFREE(start);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_FIND_VALUE - this routine searches an array, X, for value, VAL
 *                - it returns an array of zero-based index(es), OUT,
 *                - which is NOUT long
 */

int _PM_find_value(int *nout, int **out, int nx, char *type, void *x,
		   int (*prd)(double u, double v), double val,
		   int nin, int *in)
   {int id, ipt, n;

    id = SC_type_id(type, FALSE);

    if (SC_is_type_qut(id) == TRUE)
       ipt = 4;
    else if (SC_is_type_cx(id) == TRUE)
       ipt = 2;
    else
       ipt = 1;

    n = sizeof(_PM_find_value_fnc)/sizeof(PF_PM_find_value);
    if ((0 <= id) && (id < n) && (_PM_find_value_fnc[id] != NULL))
       _PM_find_value_fnc[id](nx, x, prd, val, nout, out, nin, in, ipt);

    else
       {*nout = -1;
	*out  = NULL;};

    return(*nout);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

