#
# SXPDBD.T - value difference templates
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

#--------------------------------------------------------------------------

template<char|long long>
int _SX_diff_primitives(char *indx, void *bfa, void *bfb, long n,
			long double tol, int ipt)
    {int ok, ret;
     long i, ne;
     <CTYPE> *a, *b;

     a = (<CTYPE> *) bfa;
     b = (<CTYPE> *) bfb;

     ne = ipt*n;
     if (SX_disp_individ_diff == TRUE)
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

#--------------------------------------------------------------------------

template<float|quaternion>
int _SX_diff_primitives(char *indx, void *bfa, void *bfb, long n,
			long double tol, int ipt)
    {int ok, ret;
     long i, ne;
     <CTYPE> *a, *b;

     a = (<CTYPE> *) bfa;
     b = (<CTYPE> *) bfb;

     ne = ipt*n;
     if (SX_disp_individ_diff == TRUE)
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

#--------------------------------------------------------------------------

