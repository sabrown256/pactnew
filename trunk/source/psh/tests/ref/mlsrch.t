/*
 * MLSRCH_T.H - generated type handling routines - do not edit
 *
 */

#ifndef PCK_MLSRCH_T

#define PCK_MLSRCH_T

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

static int PM_find_index_chr(void *p, double f, int n)
   {int indx;
    char *d, v;
    long i, j;
    d    = (char *) p;
    v    = (char) f;
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

/*--------------------------------------------------------------------------*/

static int PM_find_index_wchr(void *p, double f, int n)
   {int indx;
    wchar_t *d, v;
    long i, j;
    d    = (wchar_t *) p;
    v    = (wchar_t) f;
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

/*--------------------------------------------------------------------------*/

static int PM_find_index_int8(void *p, double f, int n)
   {int indx;
    int8_t *d, v;
    long i, j;
    d    = (int8_t *) p;
    v    = (int8_t) f;
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

/*--------------------------------------------------------------------------*/

static int PM_find_index_shrt(void *p, double f, int n)
   {int indx;
    short *d, v;
    long i, j;
    d    = (short *) p;
    v    = (short) f;
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

/*--------------------------------------------------------------------------*/

static int PM_find_index_int(void *p, double f, int n)
   {int indx;
    int *d, v;
    long i, j;
    d    = (int *) p;
    v    = (int) f;
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

/*--------------------------------------------------------------------------*/

static int PM_find_index_lng(void *p, double f, int n)
   {int indx;
    long *d, v;
    long i, j;
    d    = (long *) p;
    v    = (long) f;
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

/*--------------------------------------------------------------------------*/

static int PM_find_index_ll(void *p, double f, int n)
   {int indx;
    long long *d, v;
    long i, j;
    d    = (long long *) p;
    v    = (long long) f;
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

/*--------------------------------------------------------------------------*/

static int PM_find_index_flt(void *p, double f, int n)
   {int indx;
    float *d, v;
    long i, j;
    d    = (float *) p;
    v    = (float) f;
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

/*--------------------------------------------------------------------------*/

static int PM_find_index_dbl(void *p, double f, int n)
   {int indx;
    double *d, v;
    long i, j;
    d    = (double *) p;
    v    = (double) f;
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

/*--------------------------------------------------------------------------*/

static int PM_find_index_ldbl(void *p, double f, int n)
   {int indx;
    long double *d, v;
    long i, j;
    d    = (long double *) p;
    v    = (long double) f;
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

/*--------------------------------------------------------------------------*/

static int _PM_find_index_fast_chr(void *p, double f, int n, long *plast)
   {int last, indx;
    char *d, v;
    long imn, imx;
    last = *plast;
    d   = (char *) p;
    v   = (char) f;
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

/*--------------------------------------------------------------------------*/

static int _PM_find_index_fast_wchr(void *p, double f, int n, long *plast)
   {int last, indx;
    wchar_t *d, v;
    long imn, imx;
    last = *plast;
    d   = (wchar_t *) p;
    v   = (wchar_t) f;
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

/*--------------------------------------------------------------------------*/

static int _PM_find_index_fast_int8(void *p, double f, int n, long *plast)
   {int last, indx;
    int8_t *d, v;
    long imn, imx;
    last = *plast;
    d   = (int8_t *) p;
    v   = (int8_t) f;
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

/*--------------------------------------------------------------------------*/

static int _PM_find_index_fast_shrt(void *p, double f, int n, long *plast)
   {int last, indx;
    short *d, v;
    long imn, imx;
    last = *plast;
    d   = (short *) p;
    v   = (short) f;
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

/*--------------------------------------------------------------------------*/

static int _PM_find_index_fast_int(void *p, double f, int n, long *plast)
   {int last, indx;
    int *d, v;
    long imn, imx;
    last = *plast;
    d   = (int *) p;
    v   = (int) f;
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

/*--------------------------------------------------------------------------*/

static int _PM_find_index_fast_lng(void *p, double f, int n, long *plast)
   {int last, indx;
    long *d, v;
    long imn, imx;
    last = *plast;
    d   = (long *) p;
    v   = (long) f;
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

/*--------------------------------------------------------------------------*/

static int _PM_find_index_fast_ll(void *p, double f, int n, long *plast)
   {int last, indx;
    long long *d, v;
    long imn, imx;
    last = *plast;
    d   = (long long *) p;
    v   = (long long) f;
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

/*--------------------------------------------------------------------------*/

static int _PM_find_index_fast_flt(void *p, double f, int n, long *plast)
   {int last, indx;
    float *d, v;
    long imn, imx;
    last = *plast;
    d   = (float *) p;
    v   = (float) f;
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

/*--------------------------------------------------------------------------*/

static int _PM_find_index_fast_dbl(void *p, double f, int n, long *plast)
   {int last, indx;
    double *d, v;
    long imn, imx;
    last = *plast;
    d   = (double *) p;
    v   = (double) f;
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

/*--------------------------------------------------------------------------*/

static int _PM_find_index_fast_ldbl(void *p, double f, int n, long *plast)
   {int last, indx;
    long double *d, v;
    long imn, imx;
    last = *plast;
    d   = (long double *) p;
    v   = (long double) f;
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

/*--------------------------------------------------------------------------*/

static void _PM_find_value_chr(int nx, void *x, int (*prd)(double u, double v),
		    double v, int *nout, int **out, int nin, int *in, int ipt)
   {int i, no, init;
    int *oind;
    long ne;
    char *f;
    SC_array *ta;
    double u;
    init = FALSE;
    f    = (char *) x;
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

/*--------------------------------------------------------------------------*/

static void _PM_find_value_wchr(int nx, void *x, int (*prd)(double u, double v),
		    double v, int *nout, int **out, int nin, int *in, int ipt)
   {int i, no, init;
    int *oind;
    long ne;
    wchar_t *f;
    SC_array *ta;
    double u;
    init = FALSE;
    f    = (wchar_t *) x;
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

/*--------------------------------------------------------------------------*/

static void _PM_find_value_int8(int nx, void *x, int (*prd)(double u, double v),
		    double v, int *nout, int **out, int nin, int *in, int ipt)
   {int i, no, init;
    int *oind;
    long ne;
    int8_t *f;
    SC_array *ta;
    double u;
    init = FALSE;
    f    = (int8_t *) x;
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

/*--------------------------------------------------------------------------*/

static void _PM_find_value_shrt(int nx, void *x, int (*prd)(double u, double v),
		    double v, int *nout, int **out, int nin, int *in, int ipt)
   {int i, no, init;
    int *oind;
    long ne;
    short *f;
    SC_array *ta;
    double u;
    init = FALSE;
    f    = (short *) x;
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

/*--------------------------------------------------------------------------*/

static void _PM_find_value_int(int nx, void *x, int (*prd)(double u, double v),
		    double v, int *nout, int **out, int nin, int *in, int ipt)
   {int i, no, init;
    int *oind;
    long ne;
    int *f;
    SC_array *ta;
    double u;
    init = FALSE;
    f    = (int *) x;
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

/*--------------------------------------------------------------------------*/

static void _PM_find_value_lng(int nx, void *x, int (*prd)(double u, double v),
		    double v, int *nout, int **out, int nin, int *in, int ipt)
   {int i, no, init;
    int *oind;
    long ne;
    long *f;
    SC_array *ta;
    double u;
    init = FALSE;
    f    = (long *) x;
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

/*--------------------------------------------------------------------------*/

static void _PM_find_value_ll(int nx, void *x, int (*prd)(double u, double v),
		    double v, int *nout, int **out, int nin, int *in, int ipt)
   {int i, no, init;
    int *oind;
    long ne;
    long long *f;
    SC_array *ta;
    double u;
    init = FALSE;
    f    = (long long *) x;
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

/*--------------------------------------------------------------------------*/

static void _PM_find_value_flt(int nx, void *x, int (*prd)(double u, double v),
		    double v, int *nout, int **out, int nin, int *in, int ipt)
   {int i, no, init;
    int *oind;
    long ne;
    float *f;
    SC_array *ta;
    double u;
    init = FALSE;
    f    = (float *) x;
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

/*--------------------------------------------------------------------------*/

static void _PM_find_value_dbl(int nx, void *x, int (*prd)(double u, double v),
		    double v, int *nout, int **out, int nin, int *in, int ipt)
   {int i, no, init;
    int *oind;
    long ne;
    double *f;
    SC_array *ta;
    double u;
    init = FALSE;
    f    = (double *) x;
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

/*--------------------------------------------------------------------------*/

static void _PM_find_value_ldbl(int nx, void *x, int (*prd)(double u, double v),
		    double v, int *nout, int **out, int nin, int *in, int ipt)
   {int i, no, init;
    int *oind;
    long ne;
    long double *f;
    SC_array *ta;
    double u;
    init = FALSE;
    f    = (long double *) x;
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

/*--------------------------------------------------------------------------*/

static void _PM_find_value_fcx(int nx, void *x, int (*prd)(double u, double v),
		    double v, int *nout, int **out, int nin, int *in, int ipt)
   {int i, no, init;
    int *oind;
    long ne;
    float _Complex *f;
    SC_array *ta;
    double u;
    init = FALSE;
    f    = (float _Complex *) x;
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

/*--------------------------------------------------------------------------*/

static void _PM_find_value_dcx(int nx, void *x, int (*prd)(double u, double v),
		    double v, int *nout, int **out, int nin, int *in, int ipt)
   {int i, no, init;
    int *oind;
    long ne;
    double _Complex *f;
    SC_array *ta;
    double u;
    init = FALSE;
    f    = (double _Complex *) x;
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

/*--------------------------------------------------------------------------*/

static void _PM_find_value_ldcx(int nx, void *x, int (*prd)(double u, double v),
		    double v, int *nout, int **out, int nin, int *in, int ipt)
   {int i, no, init;
    int *oind;
    long ne;
    long double _Complex *f;
    SC_array *ta;
    double u;
    init = FALSE;
    f    = (long double _Complex *) x;
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

/*--------------------------------------------------------------------------*/

static void PM_min_max_chr(void *p, int n, void *pn, void *px, int *imn, int *imx)
   {int i, in, ix;
    signed char *d, *pvn, *pvx, vn, vx, v;
    d   = (signed char *) p;
    pvn = (signed char *) pn;
    pvx = (signed char *) px;
    vn  =  SCHAR_MAX;
    vx  = -SCHAR_MAX;
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

/*--------------------------------------------------------------------------*/

static void PM_min_max_wchr(void *p, int n, void *pn, void *px, int *imn, int *imx)
   {int i, in, ix;
    wchar_t *d, *pvn, *pvx, vn, vx, v;
    d   = (wchar_t *) p;
    pvn = (wchar_t *) pn;
    pvx = (wchar_t *) px;
    vn  =  WCHAR_MAX;
    vx  = -WCHAR_MAX;
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

/*--------------------------------------------------------------------------*/

static void PM_min_max_int8(void *p, int n, void *pn, void *px, int *imn, int *imx)
   {int i, in, ix;
    int8_t *d, *pvn, *pvx, vn, vx, v;
    d   = (int8_t *) p;
    pvn = (int8_t *) pn;
    pvx = (int8_t *) px;
    vn  =  INT8_MAX;
    vx  = -INT8_MAX;
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

/*--------------------------------------------------------------------------*/

static void PM_min_max_shrt(void *p, int n, void *pn, void *px, int *imn, int *imx)
   {int i, in, ix;
    signed short *d, *pvn, *pvx, vn, vx, v;
    d   = (signed short *) p;
    pvn = (signed short *) pn;
    pvx = (signed short *) px;
    vn  =  SHRT_MAX;
    vx  = -SHRT_MAX;
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

/*--------------------------------------------------------------------------*/

static void PM_min_max_int(void *p, int n, void *pn, void *px, int *imn, int *imx)
   {int i, in, ix;
    signed int *d, *pvn, *pvx, vn, vx, v;
    d   = (signed int *) p;
    pvn = (signed int *) pn;
    pvx = (signed int *) px;
    vn  =  INT_MAX;
    vx  = -INT_MAX;
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

/*--------------------------------------------------------------------------*/

static void PM_min_max_lng(void *p, int n, void *pn, void *px, int *imn, int *imx)
   {int i, in, ix;
    signed long *d, *pvn, *pvx, vn, vx, v;
    d   = (signed long *) p;
    pvn = (signed long *) pn;
    pvx = (signed long *) px;
    vn  =  LONG_MAX;
    vx  = -LONG_MAX;
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

/*--------------------------------------------------------------------------*/

static void PM_min_max_ll(void *p, int n, void *pn, void *px, int *imn, int *imx)
   {int i, in, ix;
    signed long long *d, *pvn, *pvx, vn, vx, v;
    d   = (signed long long *) p;
    pvn = (signed long long *) pn;
    pvx = (signed long long *) px;
    vn  =  LLONG_MAX;
    vx  = -LLONG_MAX;
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

/*--------------------------------------------------------------------------*/

static void PM_min_max_flt(void *p, int n, void *pn, void *px, int *imn, int *imx)
   {int i, in, ix;
    float *d, *pvn, *pvx, vn, vx, v;
    d   = (float *) p;
    pvn = (float *) pn;
    pvx = (float *) px;
    vn  =  FLT_MAX;
    vx  = -FLT_MAX;
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

/*--------------------------------------------------------------------------*/

static void PM_min_max_dbl(void *p, int n, void *pn, void *px, int *imn, int *imx)
   {int i, in, ix;
    double *d, *pvn, *pvx, vn, vx, v;
    d   = (double *) p;
    pvn = (double *) pn;
    pvx = (double *) px;
    vn  =  DBL_MAX;
    vx  = -DBL_MAX;
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

/*--------------------------------------------------------------------------*/

static void PM_min_max_ldbl(void *p, int n, void *pn, void *px, int *imn, int *imx)
   {int i, in, ix;
    long double *d, *pvn, *pvx, vn, vx, v;
    d   = (long double *) p;
    pvn = (long double *) pn;
    pvx = (long double *) px;
    vn  =  LDBL_MAX;
    vx  = -LDBL_MAX;
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

/*--------------------------------------------------------------------------*/

typedef int (*PFPM_find_index)(void *p, double f, int n);

static PFPM_find_index
 PM_find_index_fnc[] = {
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                PM_find_index_chr,
                NULL,
                PM_find_index_wchr,
                NULL,
                PM_find_index_int8,
                NULL,
                PM_find_index_shrt,
                NULL,
                PM_find_index_int,
                NULL,
                PM_find_index_lng,
                NULL,
                PM_find_index_ll,
                NULL,
                PM_find_index_flt,
                NULL,
                PM_find_index_dbl,
                NULL,
                PM_find_index_ldbl,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
   };

/*--------------------------------------------------------------------------*/

typedef int (*PF_PM_find_index_fast)(void *p, double f, int n, long *plast);

static PF_PM_find_index_fast
 _PM_find_index_fast_fnc[] = {
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                _PM_find_index_fast_chr,
                NULL,
                _PM_find_index_fast_wchr,
                NULL,
                _PM_find_index_fast_int8,
                NULL,
                _PM_find_index_fast_shrt,
                NULL,
                _PM_find_index_fast_int,
                NULL,
                _PM_find_index_fast_lng,
                NULL,
                _PM_find_index_fast_ll,
                NULL,
                _PM_find_index_fast_flt,
                NULL,
                _PM_find_index_fast_dbl,
                NULL,
                _PM_find_index_fast_ldbl,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
   };

/*--------------------------------------------------------------------------*/

typedef void (*PF_PM_find_value)(int nx, void *x, int (*prd)(double u, double v),
		    double v, int *nout, int **out, int nin, int *in, int ipt);

static PF_PM_find_value
 _PM_find_value_fnc[] = {
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                _PM_find_value_chr,
                NULL,
                _PM_find_value_wchr,
                NULL,
                _PM_find_value_int8,
                NULL,
                _PM_find_value_shrt,
                NULL,
                _PM_find_value_int,
                NULL,
                _PM_find_value_lng,
                NULL,
                _PM_find_value_ll,
                NULL,
                _PM_find_value_flt,
                NULL,
                _PM_find_value_dbl,
                NULL,
                _PM_find_value_ldbl,
                NULL,
                _PM_find_value_fcx,
                NULL,
                _PM_find_value_dcx,
                NULL,
                _PM_find_value_ldcx,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
   };

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

typedef void (*PFPM_min_max)(void *p, int n, void *pn, void *px, int *imn, int *imx);

static PFPM_min_max
 PM_min_max_fnc[] = {
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                PM_min_max_chr,
                NULL,
                PM_min_max_wchr,
                NULL,
                PM_min_max_int8,
                NULL,
                PM_min_max_shrt,
                NULL,
                PM_min_max_int,
                NULL,
                PM_min_max_lng,
                NULL,
                PM_min_max_ll,
                NULL,
                PM_min_max_flt,
                NULL,
                PM_min_max_dbl,
                NULL,
                PM_min_max_ldbl,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
   };


#endif

