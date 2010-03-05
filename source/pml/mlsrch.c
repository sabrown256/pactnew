/*
 * MLSRCH.C - search routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

#define FIND_INDEX(type, p, f, n, indx)                                      \
   {type *d, v;                                                              \
    long i, j;                                                               \
    d    = (type *) p;                                                       \
    v    = (type) f;                                                         \
    indx = 0L;                                                               \
    if (v > d[0])                                                            \
       {indx = n - 1;                                                        \
	if (v < d[indx])                                                     \
	   {i = 0L;                                                          \
	    j = indx;                                                        \
	    for (indx = (i+j) >> 1L; indx > i; indx = (i+j) >> 1L)           \
	        {if (v < d[indx])                                            \
		    j = indx;                                                \
		 else                                                        \
		    i = indx;};};};                                          \
    indx++;}

#define FIND_INDEX_FAST(type, p, f, n, indx, last)                           \
   {type *d, v;                                                              \
    long imn, imx;                                                           \
    d   = (type *) p;                                                        \
    v   = (type) f;                                                          \
    imn = 0L;                                                                \
    imx = n - 1L;                                                            \
    for (indx = last; indx > imn; indx = (imn + imx) >> 1L)                  \
        {if (v < d[indx])                                                    \
	    imx = indx;                                                      \
	 else                                                                \
	    imn = indx;};                                                    \
    last = ++indx;}

#define MIN_MAX(type, p, n, pn, px, imn, imx, lim)                           \
   {type *d, *pvn, *pvx, vn, vx, v;                                          \
    int i, in, ix;                                                           \
    d   = (type *) p;                                                        \
    pvn = (type *) pn;                                                       \
    pvx = (type *) px;                                                       \
    vn  =  lim;                                                              \
    vx  = -lim;                                                              \
    in  = -1;                                                                \
    ix  = n;                                                                 \
    for (i = 0; i < n; i++)                                                  \
        {v = *d++;                                                           \
	 if (v < vn)                                                         \
            {vn = v;                                                         \
             in = i;};                                                       \
	 if (v > vx)                                                         \
            {vx = v;                                                         \
             ix = i;};};                                                     \
    if (imn != NULL)                                                         \
       *imn = in;                                                            \
    if (imx != NULL)                                                         \
       *imx = ix;                                                            \
    if (pvn != NULL)                                                         \
       *pvn = vn;                                                            \
    if (pvx != NULL)                                                         \
       *pvx = vx;}

#define FIND_VALUE(type, nx, x, prd, val, _no, _o, _ni, _in)                 \
   {type *_f;                                                                \
    SC_array *temp;                                                          \
    int initted = FALSE;                                                     \
    int _i;                                                                  \
    double _u, _v;                                                           \
    _f   = (type *) (x);                                                     \
    _v   = (val);                                                            \
    *_no = 0;                                                                \
    if ((_ni > 0) && (_in != NULL))                                          \
       {for (_i = 0; _i < _ni; _i++)                                         \
            {_u = (double) (_f[_in[_i]]);                                    \
             if ((*prd)(_u, _v))                                             \
                {if (!initted)                                               \
                    {temp = SC_MAKE_ARRAY("FIND_VALUE", int, NULL);          \
                     initted = TRUE;}                                        \
                 SC_array_push(temp, (_in)+(_i));};};}                       \
    else                                                                     \
       {for (_i = 0; _i < nx; _i++)                                          \
            {_u = (double) (_f[_i]);                                         \
             if ((*prd)(_u, _v))                                             \
                {if (!initted)                                               \
                    {temp = SC_MAKE_ARRAY("FIND_VALUE", int, NULL);          \
                     initted = TRUE;}                                        \
                 SC_array_push(temp, &(_i));};};}                            \
    if (initted)                                                             \
       {*(_no) = SC_array_get_n(temp);                                       \
        *(_o)  = FMAKE_N(int, *_no, "FIND_VALUE:_o");                        \
        memcpy(*(_o), temp->array, (*(_no))*sizeof(int));                    \
        SC_free_array(temp, NULL);};}

SC_THREAD_LOCK(PM_search_lock); /* Lock around initialization */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MINMAX - return the min, max and and index min, max
 *           - for the given array
 */

void PM_minmax(void *p, int n, void *pn, void *px, int *imin, int *imax)
   {int type;

    type = SC_arrtype(p, -1);

    if (type == SC_DOUBLE_I)
       {MIN_MAX(double, p, n, pn, px, imin, imax, DBL_MAX);}

    else if (type == SC_FLOAT_I)
       {MIN_MAX(float, p, n, pn, px, imin, imax, FLT_MAX);}

    else if (type == SC_INTEGER_I)
       {MIN_MAX(int, p, n, pn, px, imin, imax, INT_MAX);}

    else if (type == SC_LONG_I)
       {MIN_MAX(long, p, n, pn, px, imin, imax, LONG_MAX);}

    else if (type == SC_SHORT_I)
       {MIN_MAX(short, p, n, pn, px, imin, imax, SHRT_MAX);}

    else if (type == SC_CHAR_I)
       {MIN_MAX(SIGNED char, p, n, pn, px, imin, imax, 127);}

    else
       {MIN_MAX(double, p, n, pn, px, imin, imax, DBL_MAX);};

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

/* PM_FIND_INDEX - return the index into array P such that 
 *               - P[i-1] < F <= P[i]
 *               - return 0 if F <= P[0] and N if P[N-1] < F
 */

int PM_find_index(void *p, double f, int n)
   {int indx, type;

    type = SC_arrtype(p, -1);

    if (type == SC_DOUBLE_I)
       {FIND_INDEX(double, p, f, n, indx);}

    else if (type == SC_FLOAT_I)
       {FIND_INDEX(float, p, f, n, indx);}

    else if (type == SC_INTEGER_I)
       {FIND_INDEX(int, p, f, n, indx);}

    else if (type == SC_LONG_I)
       {FIND_INDEX(long, p, f, n, indx);}

    else if (type == SC_SHORT_I)
       {FIND_INDEX(short, p, f, n, indx);}

    else if (type == SC_CHAR_I)
       {FIND_INDEX(char, p, f, n, indx);}

    else
       {FIND_INDEX(double, p, f, n, indx);};

    return(indx);}

/*--------------------------------------------------------------------------*/

#if 0

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

void PM_search_slow(int nx double *x, int ny, double *y, int offs, int *indx)
   {long ix, iy;

    y += offs;

    for (ix = 0; ix < nx; ix++)
        {FIND_INDEX(double, y, x[ix], ny, iy);

	 indx[ix] = iy + offs;};

    return;}

/*--------------------------------------------------------------------------*/

#endif

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
        {FIND_INDEX_FAST(double, y, x[ix], ny, iy, ma->last);

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
    min_index = FMAKE_N(long, ndims, "offsets:min_index");

    for (i = 1; i <= ndims; i++)
        min_index[i-1] = dims[2*i-1];

    for (i = 0; i < noffs; i++)
        {start[i] = 0;
         for (j = 0; j < ndims; j++)
	    {start[i] += (strtind[i][j] - min_index[j]) 
	                 * stride[j];};}

    SFREE(min_index);

    return;} 

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_HYPER_INDICES - calculate starting offsets and chunk size
 *                   - for carving a sub-array out of an array
 */

long _PM_hyper_indices(long *dims, long *region, long **start, long *chunk)
   {long ndims, *stride, **strtind;
    long i, j, noffs;

    ndims = dims[0];

    stride = FMAKE_N(long, ndims, "_PM_hyper_indices:stride");

/* compute stride for each dimension */
    stride[0] = 1;
    for (i = 1, j = 1; j < ndims; i += 2, j++)
        stride[j] = (dims[i+1] - dims[i] + 1) * stride[j-1];
    
/* compute number of start offsets*/
    noffs = 1;
    for (i = ndims * 2; i > 2; i -= 2)
        noffs *= region[i] - region[i-1] + 1;

/* allocate the start array */
    *start = FMAKE_N(long, noffs, "_PM_hyper_indices:start");

/* allocate temporary to store permuted indices */
    strtind = FMAKE_N(long *, noffs, "_PM_hyper_indices:strtind");

    ndims = region[0];
    for (i = 0; i < noffs; i++)
        strtind[i] = FMAKE_N(long, ndims, "_PM_hyper_indices:strtind[i]");

/* permute the indices */
    _PM_permute(region, strtind, noffs);

/* calculate 1-d starting offsets */
    _PM_offsets(dims, strtind, *start, stride, noffs);

    *chunk = region[2] - region[1] + 1;

/* free up memory */
    SFREE(stride)

    for (i = 0; i < noffs; i++)
        SFREE(strtind[i]);

    SFREE(strtind);
 
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

    SFREE(start);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_FIND_VALUE - this routine searches an array, X, for value, VAL
 *                - it returns an array of zero-based index(es), OUT,
 *                - which is NOUT long
 */

int _PM_find_value(int *nout, int **out, int nx, char *type, void *x,
		   int (*prd)(double u, double v), double val, int nin, int *in)
   {

    if (strcmp(type, SC_DOUBLE_S) == 0)
       {FIND_VALUE(double, nx, x, prd, val, nout, out, nin, in);}

    else if (strcmp(type, SC_INTEGER_S) == 0)
       {FIND_VALUE(int, nx, x, prd, val, nout, out, nin, in);}

    else if (strcmp(type, SC_LONG_S) == 0)
       {FIND_VALUE(long, nx, x, prd, val, nout, out, nin, in);}

    else if (strcmp(type, SC_SHORT_S) == 0)
       {FIND_VALUE(short, nx, x, prd, val, nout, out, nin, in);}

    else if (strcmp(type, SC_CHAR_S) == 0)
       {FIND_VALUE(char, nx, x, prd, val, nout, out, nin, in);}

    else if (strcmp(type, SC_FLOAT_S) == 0)
       {FIND_VALUE(float, nx, x, prd, val, nout, out, nin, in);}

    else
       {*nout = -1;
	*out  = NULL;};

    return(*nout);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

