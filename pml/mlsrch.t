#
# MLSRCH.T - value search templates
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

#--------------------------------------------------------------------------

template<char|long double>
int PM_find_index(const void *p, double f, int n)
   {int indx;
    <TYPE> *d, v;
    long i, j;

    d    = (<TYPE> *) p;
    v    = (<TYPE>) f;
    indx = 0L;
    if (v > d[0])
       {indx = n - 1;
	if (v < d[indx])
	   {i = 0L;
	    j = indx;
	    for (indx = (i+j) >> 1L; indx > i; indx = (i+j) >> 1L)
	        {if (v < d[indx])
		    j = indx;
		 else
		    i = indx;};};};
    indx++;

    return(indx);}

#--------------------------------------------------------------------------

template<char|long double>
int _PM_find_index_fast(const void *p, double f, int n, long *plast)
   {int last, indx;
    <TYPE> *d, v;
    long imn, imx;

    last = *plast;

    d   = (<TYPE> *) p;
    v   = (<TYPE>) f;
    imn = 0L;
    imx = n - 1L;
    for (indx = last; indx > imn; indx = (imn + imx) >> 1L)
        {if (v < d[indx])
	    imx = indx;
	 else
	    imn = indx;};
    last = ++indx;

    *plast = last;

    return(indx);}

#--------------------------------------------------------------------------

template<char|long double _Complex>
void _PM_find_value(int nx, const void *x, int (*prd)(double u, double v),
		    double v, int *nout, int **out, int nin, int *in, int ipt)
   {int i, no, init;
    int *oind;
    long ne;
    <TYPE> *f;
    SC_array *ta;
    double u;

    init = FALSE;
    f    = (<TYPE> *) x;
    no   = 0;
    oind = NULL;
    if ((nin > 0) && (in != NULL))
       {ne = ipt*nin;
	for (i = 0; i < ne; i++)
            {u = f[in[i]];
             if ((*prd)(u, v))
                {if (!init)
                    {ta   = CMAKE_ARRAY(int, NULL, 0);
                     init = TRUE;}
                 SC_array_push(ta, in+i);};};}
    else
       {ne = ipt*nx;
	for (i = 0; i < ne; i++)
            {u = f[i];
             if ((*prd)(u, v))
                {if (init == FALSE)
                    {ta   = CMAKE_ARRAY(int, NULL, 0);
                     init = TRUE;}
                 SC_array_push(ta, &i);};};};

    if (init == TRUE)
       {no   = SC_array_get_n(ta);
        oind = CMAKE_N(int, no);
        memcpy(oind, ta->array, no*sizeof(int));
        SC_free_array(ta, NULL);};

    *nout = no;
    *out  = oind;

    return;}

#--------------------------------------------------------------------------

template<quaternion|quaternion>
void _PM_find_value(int nx, const void *x, int (*prd)(double u, double v),
		    double v, int *nout, int **out, int nin, int *in, int ipt)
   {int i, no, init;
    int *oind;
    long ne;
    <TYPE> *f;
    SC_array *ta;
    double u;

    init = FALSE;
    f    = (<TYPE> *) x;
    no   = 0;
    oind = NULL;
    if ((nin > 0) && (in != NULL))
       {ne = ipt*nin;
	for (i = 0; i < ne; i++)
            {u = f[in[i]].s;
             if ((*prd)(u, v))
                {if (!init)
                    {ta   = CMAKE_ARRAY(int, NULL, 0);
                     init = TRUE;}
                 SC_array_push(ta, in+i);};};}
    else
       {ne = ipt*nx;
	for (i = 0; i < ne; i++)
            {u = f[i].s;
             if ((*prd)(u, v))
                {if (init == FALSE)
                    {ta   = CMAKE_ARRAY(int, NULL, 0);
                     init = TRUE;}
                 SC_array_push(ta, &i);};};};

    if (init == TRUE)
       {no   = SC_array_get_n(ta);
        oind = CMAKE_N(int, no);
        memcpy(oind, ta->array, no*sizeof(int));
        SC_free_array(ta, NULL);};

    *nout = no;
    *out  = oind;

    return;}

#--------------------------------------------------------------------------

template<char|long double>
void PM_min_max(const void *p, int n, void *pn, void *px, int *imn, int *imx)
   {int i, in, ix;
    <SIGNED> *d, *pvn, *pvx, vn, vx, v;

    d   = (<SIGNED> *) p;
    pvn = (<SIGNED> *) pn;
    pvx = (<SIGNED> *) px;
    vn  =  <MAX>;
    vx  = -<MAX>;
    in  = -1;
    ix  = n;
    for (i = 0; i < n; i++)
        {v = *d++;
	 if (v < vn)
            {vn = v;
             in = i;};
	 if (v > vx)
            {vx = v;
             ix = i;};};
    if (imn != NULL)
       *imn = in;
    if (imx != NULL)
       *imx = ix;
    if (pvn != NULL)
       *pvn = vn;
    if (pvx != NULL)
       *pvx = vx;

    return;}

#--------------------------------------------------------------------------

