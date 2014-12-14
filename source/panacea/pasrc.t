#
# PASRC.T - source interpolation template
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
# 

#--------------------------------------------------------------------------

template<int8_t|long double>
void PA_interp_src(void *v, PA_src_variable *svp, int ni, int nf,
		   double t, double dt,
		   double t0, double t1, double t2, double t3)
    {int i;
     <TYPE> *pv;
     <TYPE> *d0, *d1, *d2, *d3, **pd;
     double s1, s2;
     double dt10, dt21, dt32, xd1, xd2;
     double xm1, xm2, xm3;
     double xf0, xf1, xf2, xf3;

     pv = (<TYPE> *) v;
     pd = (<TYPE> **) svp->queue;
     d0 = pd[0];
     d1 = pd[1];
     d2 = pd[2];
     d3 = pd[3];

     if (svp->interpolate)
        {dt10 = t1 - t0;
         dt21 = t2 - t1;
         dt32 = t3 - t2;

         s1 = (t - t1)/dt21;
         s2 = 1.0 - s1;
         for (i = ni; i < nf; i++)
             {xd1 = 0.0;
              xd2 = 0.0;
              xf0 = (double) d0[i];
              xf1 = (double) d1[i];
              xf2 = (double) d2[i];
              xf3 = (double) d3[i];
              xm1 = (xf1 - xf0)/dt10;
              xm2 = (xf2 - xf1)/dt21;
              xm3 = (xf3 - xf2)/dt32;

              if (xm1*xm2 > 0.0)
                 {xd1 = (dt21*xm1 + dt10*xm2)/(dt10 + dt21);
                  if (xd1/xm1 > 3.0)
                     xd1 = 3.0*xm1;
                  if (xd1/xm2 > 3.0)
                     xd1 = 3.0*xm2;};

              if (xm2*xm3 > 0.0)
                 {xd2 = (dt32*xm2 + dt21*xm3)/(dt21 + dt32);
                  if (xd2/xm2 > 3.0)
                     xd2 = 3.0*xm2;
                  if (xd2/xm3 > 3.0)
                     xd2 = 3.0*xm3;};

              pv[i] = (<TYPE>) ((xf1 + (2.0*xf1 + dt21*xd1)*s1)*s2*s2 +
                              (xf2 + (2.0*xf2 - dt21*xd2)*s2)*s1*s1);};}

     else if ((t <= t2) && (t2 < t+dt))
        {for (i = ni; i < nf; i++)
             pv[i] = (<TYPE>) d2[i];};

     return;}

#--------------------------------------------------------------------------


