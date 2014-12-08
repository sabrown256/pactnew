/*
 * SXPDBD.H - generated type handling routines - do not edit
 *
 */

#ifndef PCK_SXPDBD

#define PCK_SXPDBD

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

static int _SX_diff_primitives_char(char *indx, void *bfa, void *bfb, long n,
			long double tol, int ipt)
    {int ok, ret;
     long i, ne;
     char *a, *b;
     a = (char *) bfa;
     b = (char *) bfb;
     ret = TRUE;
     ne = ipt*n;
     if (SX_gs.disp_individ_diff == TRUE)
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FIX(ok, a[i], b[i]);
              if (ok == TRUE)
                 {ret    &= FALSE;
                  indx[i] = TRUE;};};}
     else
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FIX(ok, a[i], b[i]);
              ret &= ok;};};
     return(ret);}

/*--------------------------------------------------------------------------*/

static int _SX_diff_primitives_wchr(char *indx, void *bfa, void *bfb, long n,
			long double tol, int ipt)
    {int ok, ret;
     long i, ne;
     wchar_t *a, *b;
     a = (wchar_t *) bfa;
     b = (wchar_t *) bfb;
     ret = TRUE;
     ne = ipt*n;
     if (SX_gs.disp_individ_diff == TRUE)
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FIX(ok, a[i], b[i]);
              if (ok == TRUE)
                 {ret    &= FALSE;
                  indx[i] = TRUE;};};}
     else
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FIX(ok, a[i], b[i]);
              ret &= ok;};};
     return(ret);}

/*--------------------------------------------------------------------------*/

static int _SX_diff_primitives_int8(char *indx, void *bfa, void *bfb, long n,
			long double tol, int ipt)
    {int ok, ret;
     long i, ne;
     int8_t *a, *b;
     a = (int8_t *) bfa;
     b = (int8_t *) bfb;
     ret = TRUE;
     ne = ipt*n;
     if (SX_gs.disp_individ_diff == TRUE)
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FIX(ok, a[i], b[i]);
              if (ok == TRUE)
                 {ret    &= FALSE;
                  indx[i] = TRUE;};};}
     else
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FIX(ok, a[i], b[i]);
              ret &= ok;};};
     return(ret);}

/*--------------------------------------------------------------------------*/

static int _SX_diff_primitives_shrt(char *indx, void *bfa, void *bfb, long n,
			long double tol, int ipt)
    {int ok, ret;
     long i, ne;
     short *a, *b;
     a = (short *) bfa;
     b = (short *) bfb;
     ret = TRUE;
     ne = ipt*n;
     if (SX_gs.disp_individ_diff == TRUE)
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FIX(ok, a[i], b[i]);
              if (ok == TRUE)
                 {ret    &= FALSE;
                  indx[i] = TRUE;};};}
     else
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FIX(ok, a[i], b[i]);
              ret &= ok;};};
     return(ret);}

/*--------------------------------------------------------------------------*/

static int _SX_diff_primitives_int(char *indx, void *bfa, void *bfb, long n,
			long double tol, int ipt)
    {int ok, ret;
     long i, ne;
     int *a, *b;
     a = (int *) bfa;
     b = (int *) bfb;
     ret = TRUE;
     ne = ipt*n;
     if (SX_gs.disp_individ_diff == TRUE)
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FIX(ok, a[i], b[i]);
              if (ok == TRUE)
                 {ret    &= FALSE;
                  indx[i] = TRUE;};};}
     else
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FIX(ok, a[i], b[i]);
              ret &= ok;};};
     return(ret);}

/*--------------------------------------------------------------------------*/

static int _SX_diff_primitives_lng(char *indx, void *bfa, void *bfb, long n,
			long double tol, int ipt)
    {int ok, ret;
     long i, ne;
     long *a, *b;
     a = (long *) bfa;
     b = (long *) bfb;
     ret = TRUE;
     ne = ipt*n;
     if (SX_gs.disp_individ_diff == TRUE)
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FIX(ok, a[i], b[i]);
              if (ok == TRUE)
                 {ret    &= FALSE;
                  indx[i] = TRUE;};};}
     else
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FIX(ok, a[i], b[i]);
              ret &= ok;};};
     return(ret);}

/*--------------------------------------------------------------------------*/

static int _SX_diff_primitives_ll(char *indx, void *bfa, void *bfb, long n,
			long double tol, int ipt)
    {int ok, ret;
     long i, ne;
     long long *a, *b;
     a = (long long *) bfa;
     b = (long long *) bfb;
     ret = TRUE;
     ne = ipt*n;
     if (SX_gs.disp_individ_diff == TRUE)
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FIX(ok, a[i], b[i]);
              if (ok == TRUE)
                 {ret    &= FALSE;
                  indx[i] = TRUE;};};}
     else
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FIX(ok, a[i], b[i]);
              ret &= ok;};};
     return(ret);}

/*--------------------------------------------------------------------------*/

static int _SX_diff_primitives_flt(char *indx, void *bfa, void *bfb, long n,
			long double tol, int ipt)
    {int ok, ret;
     long i, ne;
     float *a, *b;
     a = (float *) bfa;
     b = (float *) bfb;
     ret = TRUE;
     ne = ipt*n;
     if (SX_gs.disp_individ_diff == TRUE)
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FLOAT(ok, a[i], b[i], tol);
              if (ok == TRUE)
                 {ret    &= FALSE;
                  indx[i] = TRUE;};};}
     else
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FLOAT(ok, a[i], b[i], tol);
              ret &= ok;};};
     return(ret);}

/*--------------------------------------------------------------------------*/

static int _SX_diff_primitives_dbl(char *indx, void *bfa, void *bfb, long n,
			long double tol, int ipt)
    {int ok, ret;
     long i, ne;
     double *a, *b;
     a = (double *) bfa;
     b = (double *) bfb;
     ret = TRUE;
     ne = ipt*n;
     if (SX_gs.disp_individ_diff == TRUE)
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FLOAT(ok, a[i], b[i], tol);
              if (ok == TRUE)
                 {ret    &= FALSE;
                  indx[i] = TRUE;};};}
     else
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FLOAT(ok, a[i], b[i], tol);
              ret &= ok;};};
     return(ret);}

/*--------------------------------------------------------------------------*/

static int _SX_diff_primitives_ldbl(char *indx, void *bfa, void *bfb, long n,
			long double tol, int ipt)
    {int ok, ret;
     long i, ne;
     long double *a, *b;
     a = (long double *) bfa;
     b = (long double *) bfb;
     ret = TRUE;
     ne = ipt*n;
     if (SX_gs.disp_individ_diff == TRUE)
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FLOAT(ok, a[i], b[i], tol);
              if (ok == TRUE)
                 {ret    &= FALSE;
                  indx[i] = TRUE;};};}
     else
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FLOAT(ok, a[i], b[i], tol);
              ret &= ok;};};
     return(ret);}

/*--------------------------------------------------------------------------*/

static int _SX_diff_primitives_fcx(char *indx, void *bfa, void *bfb, long n,
			long double tol, int ipt)
    {int ok, ret;
     long i, ne;
     float *a, *b;
     a = (float *) bfa;
     b = (float *) bfb;
     ret = TRUE;
     ne = ipt*n;
     if (SX_gs.disp_individ_diff == TRUE)
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FLOAT(ok, a[i], b[i], tol);
              if (ok == TRUE)
                 {ret    &= FALSE;
                  indx[i] = TRUE;};};}
     else
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FLOAT(ok, a[i], b[i], tol);
              ret &= ok;};};
     return(ret);}

/*--------------------------------------------------------------------------*/

static int _SX_diff_primitives_dcx(char *indx, void *bfa, void *bfb, long n,
			long double tol, int ipt)
    {int ok, ret;
     long i, ne;
     double *a, *b;
     a = (double *) bfa;
     b = (double *) bfb;
     ret = TRUE;
     ne = ipt*n;
     if (SX_gs.disp_individ_diff == TRUE)
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FLOAT(ok, a[i], b[i], tol);
              if (ok == TRUE)
                 {ret    &= FALSE;
                  indx[i] = TRUE;};};}
     else
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FLOAT(ok, a[i], b[i], tol);
              ret &= ok;};};
     return(ret);}

/*--------------------------------------------------------------------------*/

static int _SX_diff_primitives_ldcx(char *indx, void *bfa, void *bfb, long n,
			long double tol, int ipt)
    {int ok, ret;
     long i, ne;
     long double *a, *b;
     a = (long double *) bfa;
     b = (long double *) bfb;
     ret = TRUE;
     ne = ipt*n;
     if (SX_gs.disp_individ_diff == TRUE)
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FLOAT(ok, a[i], b[i], tol);
              if (ok == TRUE)
                 {ret    &= FALSE;
                  indx[i] = TRUE;};};}
     else
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FLOAT(ok, a[i], b[i], tol);
              ret &= ok;};};
     return(ret);}

/*--------------------------------------------------------------------------*/

static int _SX_diff_primitives_qut(char *indx, void *bfa, void *bfb, long n,
			long double tol, int ipt)
    {int ok, ret;
     long i, ne;
     double *a, *b;
     a = (double *) bfa;
     b = (double *) bfb;
     ret = TRUE;
     ne = ipt*n;
     if (SX_gs.disp_individ_diff == TRUE)
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FLOAT(ok, a[i], b[i], tol);
              if (ok == TRUE)
                 {ret    &= FALSE;
                  indx[i] = TRUE;};};}
     else
        {for (i = 0L; i < ne; i++)
             {PM_CLOSETO_FLOAT(ok, a[i], b[i], tol);
              ret &= ok;};};
     return(ret);}

/*--------------------------------------------------------------------------*/

typedef int (*PF_SX_diff_primitives)(char *indx, void *bfa, void *bfb, long n,
			long double tol, int ipt);

static PF_SX_diff_primitives
 _SX_diff_primitives_fnc[] = {
                NULL,
                NULL,
                NULL,
                _SX_diff_primitives_char,
                _SX_diff_primitives_wchr,
                _SX_diff_primitives_int8,
                _SX_diff_primitives_shrt,
                _SX_diff_primitives_int,
                _SX_diff_primitives_lng,
                _SX_diff_primitives_ll,
                _SX_diff_primitives_flt,
                _SX_diff_primitives_dbl,
                _SX_diff_primitives_ldbl,
                _SX_diff_primitives_fcx,
                _SX_diff_primitives_dcx,
                _SX_diff_primitives_ldcx,
                _SX_diff_primitives_qut,
                NULL,
                NULL,
                NULL
};

/*--------------------------------------------------------------------------*/


#endif

