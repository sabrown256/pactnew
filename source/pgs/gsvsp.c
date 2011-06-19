/*
 * GSVSP.C - view space geometry and transformations
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "pgs_int.h"

/*--------------------------------------------------------------------------*/

/*                            SPACE SCALE ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* _PG_SCALE_CP - copy N ND XI to XO for identity transform */

static INLINE void _PG_scale_cp(int n, int nd, double **xi, double **xo)
   {int i, id;
    double *pi, *po;

    for (id = 0; id < nd; id++)
        {pi = xi[id];
	 po = xo[id];
	 for (i = 0; i < n; i++, *po++ = *pi++);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SC_WC_TO_NDC_A - convert one WC component X to NDC absolute */

static INLINE double _PG_sc_wc_to_ndc_a(PG_device *dev, int id,
					int nd, double x)
   {int l;
    PG_dev_geometry *g;

    g = &dev->g;

    l = 2*id;
    x = g->w_nd[l+1]*x;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SC_NDC_TO_WC_A - convert one NDC component X to WC absolute */

static INLINE double _PG_sc_ndc_to_wc_a(PG_device *dev, int id,
					int nd, double x)
   {int l;
    PG_dev_geometry *g;

    g = &dev->g;

    l = 2*id;
    x = g->nd_w[l+1]*x;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SC_NDC_TO_PC_A - convert one NDC component X to PC absolute */

static INLINE double _PG_sc_ndc_to_pc_a(PG_device *dev, int id,
					int nd, double x)
   {int l;
    double w, p;
    PG_dev_geometry *g;

    g = &dev->g;

    l = 2*id;
    w = g->pc[l+1] - g->pc[l];
    p = x*w + 0.5;
    p = floor(p);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SC_PC_TO_NDC_A - convert one PC component IX to NDC absolute */

static INLINE double _PG_sc_pc_to_ndc_a(PG_device *dev, int id,
					int nd, int ix)
   {int l, w;
    double x;
    PG_dev_geometry *g;

    g = &dev->g;
    l = 2*id;
    w = g->pc[l+1] - g->pc[l];
    x = ix/w;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SC_WC_TO_PC_A - convert one WC component X to PC absolute */

static INLINE double _PG_sc_wc_to_pc_a(PG_device *dev, int id,
				       int nd, double x)
   {int l;
    double w, p;
    PG_dev_geometry *g;

    g = &dev->g;

    l = 2*id;
    w = g->pc[l+1] - g->pc[l];
	      
/* WORLDC to NDC */
    x = g->w_nd[l+1]*x;

/* NDC to PC */
    p = x*w + 0.5;
    p = floor(p);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SC_PC_TO_WC_A - convert one PC component IX to WC absolute */

static INLINE double _PG_sc_pc_to_wc_a(PG_device *dev, int id,
				       int nd, int ix)
   {int l, w;
    double x;
    PG_dev_geometry *g;

    g = &dev->g;
    l = 2*id;
    w = g->pc[l+1] - g->pc[l];

/* from PC to NDC */
    x = ix/w;

/* from NDC to WC */
    x = g->nd_w[l+1]*x;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SC_WC_TO_NDC - convert N ND XI in WC to XO in NDC */

static INLINE void _PG_sc_wc_to_ndc(PG_device *dev, int n, int nd,
				    double **xi, double **xo)
   {int i, id;
    double *pi, *po;

    for (id = 0; id < nd; id++)
        {pi = xi[id];
	 po = xo[id];
	 for (i = 0; i < n; i++)
	     po[i] = _PG_sc_wc_to_ndc_a(dev, id, nd, pi[i]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SC_NDC_TO_WC - convert N ND XI in NDC to XO in WC */

static INLINE void _PG_sc_ndc_to_wc(PG_device *dev, int n, int nd,
				    double **xi, double **xo)
   {int i, id;
    double *pi, *po;

    for (id = 0; id < nd; id++)
        {pi = xi[id];
	 po = xo[id];
	 for (i = 0; i < n; i++)
	     po[i] = _PG_sc_ndc_to_wc_a(dev, id, nd, pi[i]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SC_NDC_TO_PC - convert N ND XI in NDC to XO in PC */

static INLINE void _PG_sc_ndc_to_pc(PG_device *dev, int n, int nd,
				    double **xi, double **xo)
   {int i, id;
    double *pi, *po;

    for (id = 0; id < nd; id++)
        {pi = xi[id];
	 po = xo[id];
	 for (i = 0; i < n; i++)
	     po[i] = _PG_sc_ndc_to_pc_a(dev, id, nd, pi[i]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SC_PC_TO_NDC - convert N ND XI in PC to XO in NDC */

static INLINE void _PG_sc_pc_to_ndc(PG_device *dev, int n, int nd,
				    double **xi, double **xo)
   {int i, id;
    double *pi, *po;

    for (id = 0; id < nd; id++)
        {pi = xi[id];
	 po = xo[id];
	 for (i = 0; i < n; i++)
	     po[i] = _PG_sc_pc_to_ndc_a(dev, id, nd, pi[i]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SC_WC_TO_PC - convert N ND XI in WC to XO in PC */

static INLINE void _PG_sc_wc_to_pc(PG_device *dev, int n, int nd,
				   double **xi, double **xo)
   {int i, id;
    double *pi, *po;

    for (id = 0; id < nd; id++)
        {pi = xi[id];
	 po = xo[id];
	 for (i = 0; i < n; i++)
	     po[i] = _PG_sc_wc_to_pc_a(dev, id, nd, pi[i]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SC_PC_TO_WC - convert N ND XI in PC to XO in WC */

static INLINE void _PG_sc_pc_to_wc(PG_device *dev, int n, int nd,
				   double **xi, double **xo)
   {int i, id;
    double *pi, *po;

    for (id = 0; id < nd; id++)
        {pi = xi[id];
	 po = xo[id];
	 for (i = 0; i < n; i++)
	     po[i] = _PG_sc_pc_to_wc_a(dev, id, nd, pi[i]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SC_WC_TO - convert N ND XI in WC to XO in OCS */

static INLINE void _PG_sc_wc_to(PG_device *dev, int n, int nd, PG_coord_sys ocs,
				double **xi, double **xo)
   {

    switch (ocs)
       {case WORLDC :
	     _PG_scale_cp(n, nd, xi, xo);
             break;

	case NORMC :
	     _PG_sc_wc_to_ndc(dev, n, nd, xi, xo);
             break;

	case PIXELC :
	     _PG_sc_wc_to_pc(dev, n, nd, xi, xo);
             break;

        default :
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SC_NDC_TO - convert N ND XI in NDC to XO in OCS */

static INLINE void _PG_sc_ndc_to(PG_device *dev, int n, int nd, PG_coord_sys ocs,
				 double **xi, double **xo)
   {

    switch (ocs)
       {case WORLDC :
             _PG_sc_ndc_to_wc(dev, n, nd, xi, xo);
             break;

	case NORMC :
	     _PG_scale_cp(n, nd, xi, xo);
             break;

	case PIXELC :
	     _PG_sc_ndc_to_pc(dev, n, nd, xi, xo);
             break;

        default :
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SC_PC_TO - convert N ND XI in PC to XO in OCS */

static INLINE void _PG_sc_pc_to(PG_device *dev, int n, int nd, PG_coord_sys ocs,
				double **xi, double **xo)
   {

    switch (ocs)
       {case WORLDC :
             _PG_sc_pc_to_wc(dev, n, nd, xi, xo);
             break;

	case NORMC :
             _PG_sc_pc_to_ndc(dev, n, nd, xi, xo);
             break;

	case PIXELC :
	     _PG_scale_cp(n, nd, xi, xo);
             break;

        default :
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SCALE_POINTS - convert N ND points XI in ICS to XO in OCS
 *                 - NOTE: vectors in PC are really integer but are
 *                 - represented here as double
 */

void PG_scale_points(PG_device *dev, int n, int nd, PG_coord_sys ics, double **xi,
		     PG_coord_sys ocs, double **xo)
   {

    switch (ics)
       {case WORLDC :
             _PG_sc_wc_to(dev, n, nd, ocs, xi, xo);
             break;

	case NORMC :
             _PG_sc_ndc_to(dev, n, nd, ocs, xi, xo);
             break;

	case PIXELC :
             _PG_sc_pc_to(dev, n, nd, ocs, xi, xo);
             break;

        default :
             break;};

    return;}

/*--------------------------------------------------------------------------*/

/*                         SPACE TRANSFORM ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* _PG_TRANS_CP - copy N ND XI to XO for identity transform */

static INLINE void _PG_trans_cp(int n, int nd, double **xi, double **xo)
   {int i, id;
    double *pi, *po;

    for (id = 0; id < nd; id++)
        {pi = xi[id];
	 po = xo[id];
	 for (i = 0; i < n; i++, *po++ = *pi++);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_WC_TO_NDC_A - convert one WC component X to NDC absolute */

static INLINE double _PG_tr_wc_to_ndc_a(PG_device *dev, int id,
					int nd, double x)
   {int l;
    PG_dev_geometry *g;

    g = &dev->g;

    l = 2*id;
    x = g->w_nd[l] + g->w_nd[l+1]*x;
    x = max(x, -10.0);
    x = min(x, 11.0);

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_NDC_TO_WC_A - convert one NDC component X to WC absolute */

static INLINE double _PG_tr_ndc_to_wc_a(PG_device *dev, int id,
					int nd, double x)
   {int l;
    PG_dev_geometry *g;

    g = &dev->g;

    l = 2*id;
    x = g->nd_w[l] + g->nd_w[l+1]*x;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_NDC_TO_FRM_A - convert one NDC component X to FRAMEC absolute */

static INLINE double _PG_tr_ndc_to_frm_a(PG_device *dev, int id,
					 int nd, double x)
   {int l;
    double f, df;
    PG_dev_geometry *g;

    g = &dev->g;

    l  = 2*id;
    f  = g->fr[l];
    df = g->fr[l+1] - f;

    x = f + x*df;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_NDC_TO_PC_A - convert one NDC component X to PC absolute */

static INLINE double _PG_tr_ndc_to_pc_a(PG_device *dev, int id, int nd,
					double x, double *wo)
   {int l, ix;
    double w, p;
    PG_dev_geometry *g;

    g = &dev->g;

    l = 2*id;
    w = g->pc[l+1] - g->pc[l];
	      
    x = max(x, -10.0);
    x = min(x, 11.0);

    p  = x*w + 0.5;
    ix = floor(p) + wo[id];

    ix = max(ix, g->cpc[l]);
    ix = min(ix, g->cpc[l+1]);

    p = ix;

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_NDC_TO_PC_D - convert one NDC delta (XL,XU) to PC */

static INLINE int _PG_tr_ndc_to_pc_d(PG_device *dev, int id, int nd,
				     double xl, double xu)
   {int l, idx;
    double w, dx;
    PG_dev_geometry *g;

    g = &dev->g;

    l = 2*id;
    w = g->pc[l+1] - g->pc[l];
	      
    xl = max(xl, -10.0);
    xu = min(xu,  11.0);
    dx = xu - xl;

    idx = floor(w*dx + 0.5);

    return(idx);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_PC_TO_NDC_A - convert one PC component IX to NDC absolute */

static INLINE double _PG_tr_pc_to_ndc_a(PG_device *dev, int id, int nd,
					int ix, double *wo)
   {int l, w;
    double x;
    PG_dev_geometry *g;

    g = &dev->g;
    l = 2*id;
    w = g->pc[l+1] - g->pc[l];

    x = (ix - wo[id])/w;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_WC_TO_PC_A - convert one WC component X to PC absolute */

static INLINE double _PG_tr_wc_to_pc_a(PG_device *dev, int id, int nd,
				       double x, double *wo)
   {int l, ix;
    double w, p;
    PG_dev_geometry *g;

    g = &dev->g;

    l = 2*id;
    w = g->pc[l+1] - g->pc[l];
	      
/* WORLDC to NDC */
    x = g->w_nd[l] + g->w_nd[l+1]*x;

    x = max(x, -10.0);
    x = min(x, 11.0);

/* NDC to PC */
    p  = x*w + 0.5;
    ix = floor(p) + wo[id];

    ix = max(ix, g->cpc[l]);
    ix = min(ix, g->cpc[l+1]);

    p = ix;

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_PC_TO_WC_A - convert one PC component IX to WC absolute */

static INLINE double _PG_tr_pc_to_wc_a(PG_device *dev, int id, int nd,
				       int ix, double *wo)
   {int l, w;
    double x;
    PG_dev_geometry *g;

    g = &dev->g;
    l = 2*id;
    w = g->pc[l+1] - g->pc[l];

/* from PC to NDC */
    x = (ix - wo[id])/w;

/* from NDC to WC */
    x = g->nd_w[l] + g->nd_w[l+1]*x;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_WC_TO_NDC - convert N ND XI in WC to XO in NDC */

static INLINE void _PG_tr_wc_to_ndc(PG_device *dev, int n, int nd,
				    double **xi, double **xo)
   {int i, id;
    double *pi, *po;

    for (id = 0; id < nd; id++)
        {pi = xi[id];
	 po = xo[id];
	 for (i = 0; i < n; i++)
	     po[i] = _PG_tr_wc_to_ndc_a(dev, id, nd, pi[i]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_NDC_TO_WC - convert N ND XI in NDC to XO in WC */

static INLINE void _PG_tr_ndc_to_wc(PG_device *dev, int n, int nd,
				    double **xi, double **xo)
   {int i, id;
    double *pi, *po;

    for (id = 0; id < nd; id++)
        {pi = xi[id];
	 po = xo[id];
	 for (i = 0; i < n; i++)
	     po[i] = _PG_tr_ndc_to_wc_a(dev, id, nd, pi[i]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_NDC_TO_FRM - convert N ND XI in NDC to XO in FRAMEC */

static INLINE void _PG_tr_ndc_to_frm(PG_device *dev, int n, int nd,
				     double **xi, double **xo)
   {int i, id;
    double *pi, *po;

    for (id = 0; id < nd; id++)
        {pi = xi[id];
	 po = xo[id];
	 for (i = 0; i < n; i++)
	     po[i] = _PG_tr_ndc_to_frm_a(dev, id, nd, pi[i]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_NDC_TO_PC - convert N ND XI in NDC to XO in PC */

static INLINE void _PG_tr_ndc_to_pc(PG_device *dev, int n, int nd,
				    double **xi, double **xo)
   {int i, id;
    double wo[PG_SPACEDM];
    double *pi, *po;

    for (id = 0; id < nd; id++)
        wo[id] = dev->window_x[2*id];

    for (id = 0; id < nd; id++)
        {pi = xi[id];
	 po = xo[id];
	 for (i = 0; i < n; i++)
	     po[i] = _PG_tr_ndc_to_pc_a(dev, id, nd, pi[i], wo);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_PC_TO_NDC - convert N ND XI in PC to XO in NDC */

static INLINE void _PG_tr_pc_to_ndc(PG_device *dev, int n, int nd,
				    double **xi, double **xo)
   {int i, id;
    double wo[PG_SPACEDM];
    double *pi, *po;

    for (id = 0; id < nd; id++)
        wo[id] = dev->window_x[2*id];

    for (id = 0; id < nd; id++)
        {pi = xi[id];
	 po = xo[id];
	 for (i = 0; i < n; i++)
	     po[i] = _PG_tr_pc_to_ndc_a(dev, id, nd, pi[i], wo);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_WC_TO_PC - convert N ND XI in WC to XO in PC */

static INLINE void _PG_tr_wc_to_pc(PG_device *dev, int n, int nd,
				   double **xi, double **xo)
   {int i, id;
    double wo[PG_SPACEDM];
    double *pi, *po;

    for (id = 0; id < nd; id++)
        wo[id] = dev->window_x[2*id];

    for (id = 0; id < nd; id++)
        {pi = xi[id];
	 po = xo[id];
	 for (i = 0; i < n; i++)
	     po[i] = _PG_tr_wc_to_pc_a(dev, id, nd, pi[i], wo);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_PC_TO_WC - convert N ND XI in PC to XO in WC */

static INLINE void _PG_tr_pc_to_wc(PG_device *dev, int n, int nd,
				   double **xi, double **xo)
   {int i, id;
    double wo[PG_SPACEDM];
    double *pi, *po;

    for (id = 0; id < nd; id++)
        wo[id] = dev->window_x[2*id];

    for (id = 0; id < nd; id++)
        {pi = xi[id];
	 po = xo[id];
	 for (i = 0; i < n; i++)
	     po[i] = _PG_tr_pc_to_wc_a(dev, id, nd, pi[i], wo);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_WC_TO - convert N ND XI in WC to XO in OCS */

static INLINE void _PG_tr_wc_to(PG_device *dev, int n, int nd, PG_coord_sys ocs,
				double **xi, double **xo)
   {

    switch (ocs)
       {case WORLDC :
	     _PG_trans_cp(n, nd, xi, xo);
             break;

	case NORMC :
	     _PG_tr_wc_to_ndc(dev, n, nd, xi, xo);
             break;

	case PIXELC :
	     _PG_tr_wc_to_pc(dev, n, nd, xi, xo);
             break;

        default :
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_NDC_TO - convert N ND XI in NDC to XO in OCS */

static INLINE void _PG_tr_ndc_to(PG_device *dev, int n, int nd, PG_coord_sys ocs,
				 double **xi, double **xo)
   {

    switch (ocs)
       {case WORLDC :
             _PG_tr_ndc_to_wc(dev, n, nd, xi, xo);
             break;

	case FRAMEC :
	     _PG_tr_ndc_to_frm(dev, n, nd, xi, xo);
             break;

	case NORMC :
	     _PG_trans_cp(n, nd, xi, xo);
             break;

	case PIXELC :
	     _PG_tr_ndc_to_pc(dev, n, nd, xi, xo);
             break;

        default :
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_PC_TO - convert N ND XI in PC to XO in OCS */

static INLINE void _PG_tr_pc_to(PG_device *dev, int n, int nd, PG_coord_sys ocs,
				double **xi, double **xo)
   {

    switch (ocs)
       {case WORLDC :
             _PG_tr_pc_to_wc(dev, n, nd, xi, xo);
             break;

	case NORMC :
             _PG_tr_pc_to_ndc(dev, n, nd, xi, xo);
             break;

	case PIXELC :
	     _PG_trans_cp(n, nd, xi, xo);
             break;

        default :
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TRANS_POINTS - convert N ND points XI in ICS to XO in OCS
 *                 - NOTE: vectors in PC are really integer but are
 *                 - represented here as double
 */

void PG_trans_points(PG_device *dev, int n, int nd, PG_coord_sys ics, double **xi,
		     PG_coord_sys ocs, double **xo)
   {

    switch (ics)
       {case WORLDC :
             _PG_tr_wc_to(dev, n, nd, ocs, xi, xo);
             break;

	case NORMC :
             _PG_tr_ndc_to(dev, n, nd, ocs, xi, xo);
             break;

	case PIXELC :
             _PG_tr_pc_to(dev, n, nd, ocs, xi, xo);
             break;

        default :
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TRANS_POINT - convert a ND point XI in ICS to XO in OCS
 *                - NOTE: vectors in PC are really integer but are
 *                - represented here as double
 *
 * #bind PG_trans_point fortran() scheme()
 */

void PG_trans_point(PG_device *dev, int nd, PG_coord_sys ics, double *xi,
		    PG_coord_sys ocs, double *xo)
   {int id;
    double *o[PG_SPACEDM], *n[PG_SPACEDM];

    for (id = 0; id < nd; id++)
        {o[id] = &xi[id];
         n[id] = &xo[id];};

    PG_trans_points(dev, 1, nd, ics, o, ocs, n);

    return;}

/*--------------------------------------------------------------------------*/

/*                              BOX ROUTINES                                */

/*--------------------------------------------------------------------------*/

/* _PG_BOX_PC - convert a box BI in CS coordinates to PC */

static void _PG_box_pc(PG_device *dev, int nd, PG_coord_sys cs,
		       double *bi, double *pc)
   {int id, l, n, ix, idx;
    double wo[PG_SPACEDM];
    double bx[PG_BOXSZ];

    n = nd << 1;

/* guard against conversions in place */
    if (bi == pc)
       {PG_box_copy(nd, bx, bi);
	bi = bx;};

    for (id = 0; id < nd; id++)
        wo[id] = dev->window_x[2*id];

    switch (cs)
       {case WORLDC :
             for (l = 0; l < n; l++)
	         {id = l >> 1;
		  pc[l] = _PG_tr_wc_to_pc_a(dev, id, nd, bi[l], wo);};
	     break;

	case NORMC :

/* transforming the rectangle this way is less sensitive to FPU
 * differences (especially the PowerPC chips).
 * frequent uses of this take the lower point and the size,
 * i.e. the difference of the two points
 * each point could be a pixel off in the PostScript comparison
 * and worst case in the opposite direction so the size would
 * be two pixels off.
 * here we try to compute the size directly in a hopefully less
 * sensitive way and return the second point as the sum of the
 * first point and the size (integer arithmetic rather than
 * floating point arithmetic).
 */
	     for (id = 0; id < nd; id++)
	         {l = 2*id;

		  ix  = _PG_tr_ndc_to_pc_a(dev, id, nd, bi[l], wo);
		  idx = _PG_tr_ndc_to_pc_d(dev, id, nd, bi[l], bi[l+1]);

		  pc[l]   = ix;
		  pc[l+1] = ix + idx;};
	     break;

        default     :
	case PIXELC :
             for (l = 0; l < n; l++)
	         pc[l] = floor(bi[l] + 0.5);
	     break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_BOX_NDC - convert a box BI in CS coordinates to NDC */

static void _PG_box_ndc(PG_device *dev, int nd, PG_coord_sys cs,
			double *bi, double *ndc)
   {int id, l, n;
    double wo[PG_SPACEDM];
    double bx[PG_BOXSZ];

    n = nd << 1;

/* guard against conversions in place */
    if (bi == ndc)
       {PG_box_copy(nd, bx, bi);
	bi = bx;};

    switch (cs)
       {case WORLDC :
             for (l = 0; l < n; l++)
	         {id     = l >> 1;
		  ndc[l] = _PG_tr_wc_to_ndc_a(dev, id, nd, bi[l]);};
	     break;

	case NORMC :
	     PG_box_copy(nd, ndc, bi);
	     break;

	case PIXELC :
	     for (id = 0; id < nd; id++)
	         wo[id] = dev->window_x[2*id];
	     for (l = 0; l < n; l++)
	         {id     = l >> 1;
		  ndc[l] = _PG_tr_pc_to_ndc_a(dev, id, nd, bi[l], wo);};
	     break;

        default :
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_BOX_WC - convert a box BI in CS coordinates to WC */

static void _PG_box_wc(PG_device *dev, int nd, PG_coord_sys cs,
		       double *bi, double *wc)
   {int id, l, n;
    double wo[PG_SPACEDM];
    double bx[PG_BOXSZ];

    n = nd << 1;

/* guard against conversions in place */
    if (bi == wc)
       {PG_box_copy(nd, bx, bi);
	bi = bx;};

    switch (cs)
       {case WORLDC :
	     PG_box_copy(nd, wc, bi);
	     break;

	case PIXELC :
	     for (id = 0; id < nd; id++)
	         wo[id] = dev->window_x[2*id];
             for (l = 0; l < n; l++)
	         {id    = l >> 1;
		  wc[l] = _PG_tr_pc_to_wc_a(dev, id, nd, bi[l], wo);};
	     break;

	case NORMC :
             for (l = 0; l < n; l++)
	         {id    = l >> 1;
		  wc[l] = _PG_tr_ndc_to_wc_a(dev, id, nd, bi[l]);};
	     break;

        default :
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TRANS_BOX - convert a box BI in ICS coordinates to
 *              - OCS coordinates BO
 */

void PG_trans_box(PG_device *dev, int nd, PG_coord_sys ics, double *bi,
		  PG_coord_sys ocs, double *bo)
   {

    switch (ocs)
       {case WORLDC :
	     _PG_box_wc(dev, nd, ics, bi, bo);
	     break;

	case NORMC :
	     _PG_box_ndc(dev, nd, ics, bi, bo);
	     break;

	case PIXELC :
	     _PG_box_pc(dev, nd, ics, bi, bo);
             break;

        default :
	     break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TRANS_INTERVAL - convert a vector interval DI in ICS coordinates to
 *                   - OCS coordinates DO
 */

void PG_trans_interval(PG_device *dev, int nd, PG_coord_sys ics, double *dxi,
		       PG_coord_sys ocs, double *dxo)
   {int id, l;
    double d;
    double bx[PG_BOXSZ];

    if (ics == ocs)
       {for (id = 0; id < nd; id++)
	    dxo[id] = dxi[id];}

    else
       {PG_box_init(nd, bx, 0.0, 0.0);

	for (id = 0; id < nd; id++)
	     bx[2*id] = dxi[id];

	PG_trans_box(dev, nd, ics, bx, ocs, bx);

	for (id = 0; id < nd; id++)
	    {l = 2*id;
	     d = bx[l] - bx[l+1];
	     dxo[id] = ABS(d);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_BOX_INIT - initialize a box with lower corner (MN,MN,MN) and
 *             - uppper corner (MX,MX,MX)
 */

void PG_box_init(int nd, double *bx, double mn, double mx)
   {int i, n;

    if (bx != NULL)
       {n = nd << 1;
	for (i = 0; i < n; i += 2)
	    {bx[i]   = mn;
	     bx[i+1] = mx;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_BOX_COPY - copy box S to D */

void PG_box_copy(int nd, double *d, double *s)
   {int l, n;

    n = 2*nd;
    for (l = 0; l < n; l++)
        d[l] = s[l];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_BOX_CONTAINS - return TRUE if BOX contains P */

int PG_box_contains(int nd, double *box, double *p)
   {int id, l, ok;

    ok = TRUE;

    for (id = 0; id < nd; id++)
        {l   = 2*id;
	 ok &= ((box[l] <= p[id]) && (p[id] <= box[l+1]));};

    return(ok);}

/*--------------------------------------------------------------------------*/

/*                              LIN/LOG ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* _PG_LOG_TRANSFORM - get WC values correctly transformed for
 *                   - log scales
 */

static void _PG_log_transform(PG_device *dev, int nd, double *box)
   {int id, l, *iflog;
    double mn, mx, ma;

    iflog = dev->g.iflog;

    for (id = 0; id < nd; id++)
        {l = 2*id;

	 mn = box[l];
	 mx = box[l+1];

/* if log axis options have been used
 * take logs of min and max
 */
	 if (iflog[id])
	    {mx = log10(ABS(mx) + SMALL);
	     mn = log10(ABS(mn) + SMALL);
	     ma = mx - _PG_gattrs.axis_n_decades;

/* limit mn to be within PG_axis_n_decades of the max value */
	     mn = max(mn, ma);};

	 box[l]   = mn;
	 box[l+1] = mx;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_LOG_SPACE - get WC BOX values correctly transformed for log scales */

void PG_log_space(PG_device *dev, int nd, int dec, double *box)
   {int id, l, n;
    int *iflog;
    double xc;

    if (dec == TRUE)
       _PG_log_transform(dev, nd, box);

    else
       {iflog = dev->g.iflog;

	n = nd << 1;

	for (l = 0; l < n; l++)
	    {id = l >> 1;

	     xc = box[l];

/* if log axis options have been used take log */
	     if (iflog[id])
	        xc = log10(ABS(xc) + SMALL);

	     box[l] = xc;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_LOG_POINT - get WC point P values correctly transformed for log scales */

void PG_log_point(PG_device *dev, int nd, double *p)
   {int id, *iflog;
    double pc;

    iflog = dev->g.iflog;

    for (id = 0; id < nd; id++)
        {pc = p[id];

/* if log axis options have been used take log */
	 if (iflog[id] == TRUE)
	    pc = log10(ABS(pc) + SMALL);

	 p[id] = pc;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_LIN_SPACE - get WC BOX values correctly transformed
 *              - for linear scales
 */

void PG_lin_space(PG_device *dev, int nd, double *box)
   {int id, l, n;
    int *iflog;
    double xc;

    iflog = dev->g.iflog;

    n = nd << 1;

    for (l = 0; l < n; l++)
        {id = l >> 1;

	 xc = box[l];

/* if log axis options have been used transform back to linear */
	 if (iflog[id])
	    xc = POW(10.0, xc);

	 box[l] = xc;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_LIN_POINT - get WC point P values correctly transformed
 *              - for linear scales
 */

void PG_lin_point(PG_device *dev, int nd, double *p)
   {int id, *iflog;
    double pc;

    iflog = dev->g.iflog;

    for (id = 0; id < nd; id++)
        {pc = p[id];

/* if log axis options have been used transform back to linear */
	 if (iflog[id] == TRUE)
	    pc = POW(10.0, pc);

	 p[id] = pc;};

    return;}

/*--------------------------------------------------------------------------*/

/*                         STATE QUERY ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* _PG_FIND_CLIP_REGION - convert a rectangle to pixel coordinates
 *                      - depending on the value of CLIP return the
 *                      - whole window (FALSE) or the WC region (TRUE)
 */

void _PG_find_clip_region(PG_device *dev, int *pc, int clip, int swflag)
   {int l, pad;
    double wc[PG_BOXSZ], px[PG_BOXSZ];

    if (dev == NULL)
       return;

    pad = 8;

    if (clip)
       {PG_get_viewspace(dev, WORLDC, wc);
	_PG_log_transform(dev, 2, wc);
 	PG_trans_box(dev, 2, WORLDC, wc, PIXELC, px);}

/* if we have a single window device such as a PostScript device do this */
    else if (swflag)
       {px[0] = -pad;
	px[2] = -pad;
	px[1] = px[0] + PG_window_width(dev) + 2*pad;
	px[3] = px[2] + PG_window_height(dev) + 2*pad;}

/* otherwise for a multi-window device do this */
    else
       {px[0] = dev->window_x[0] - pad;
	px[2] = dev->window_x[2] - pad;
	px[1] = px[0] + PG_window_width(dev) + 2*pad;
	px[3] = px[2] + PG_window_height(dev) + 2*pad;};

    if (clip == FALSE)
       PG_frame_box(dev, 2, PIXELC, px);

    for (l = 0; l < 4; l++)
        pc[l] = px[l];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_CURVE_EXTENT - compute the rectangle in which the curve
 *                     - in inscribed (in PC)
 */

void PG_get_curve_extent(PG_device *dev, PG_curve *crv, PG_coord_sys cs, double *bx)
   {int i, n;
    int *x, *y;
    double pc[PG_BOXSZ];

    n = crv->n;
    x = crv->x;
    y = crv->y;

    PG_box_init(2, pc, INT_MAX, INT_MIN);

    for (i = 0; i < n; i++)
        {pc[0] = min(pc[0], x[i]);
         pc[1] = max(pc[1], x[i]);
         pc[2] = min(pc[2], y[i]);
         pc[3] = max(pc[3], y[i]);};

    PG_trans_box(dev, 2, PIXELC, pc, cs, bx);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_LIMIT - return the limits of the device viewspace */

void PG_get_limit(PG_device *dev, PG_coord_sys cs, double *lim)
   {int i, l, id;
    double x;
    PG_dev_geometry *g;

    if (dev == NULL)
       return;

    g = &dev->g;

    switch (cs)
       {case BOUNDC :
        case WORLDC :
	     PG_get_limit(dev, NORMC, lim);

	     for (id = 0; id < PG_SPACEDM; id++)
	         {l = 2*id;
		  for (i = 0; i < 2; i++)
		      {x = lim[l+i];
		       x = g->nd_w[l] + g->nd_w[l+1]*x;
		       lim[l+i] = x;};};

/* since the internals are not in log space don't return it that way */
	     PG_lin_space(dev, PG_SPACEDM, lim);
             break;

        case NORMC :
	     for (id = 0; id < PG_SPACEDM; id++)
	         {l        = 2*id;
		  lim[l]   = 0.0;
		  lim[l+1] = 1.0;};
             break;

        case PIXELC :
	     for (l = 0; l < PG_BOXSZ; l++)
	         lim[l] = g->pc[l];
             break;

        default :
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FIX_WTOS - compute the WC to NDC transformation coefficients */

void _PG_fix_wtos(PG_device *dev, int wh)
   {int id, l;
    double dw;
    double wc[PG_BOXSZ];
    PG_dev_geometry *g;

    g = &dev->g;

/* use the WC limits of the viewport */
    if (wh == TRUE)
       {for (l = 0; l < PG_BOXSZ; l++)
	    wc[l] = g->wc[l];

	_PG_log_transform(dev, PG_SPACEDM, wc);}

/* use the WC limits of the boundary */
    else
       {for (l = 0; l < PG_BOXSZ; l++)
	    wc[l] = g->bnd[l];};

    for (id = 0; id < PG_SPACEDM; id++)
        {l = 2*id;
	 if (wc[l] == wc[l+1])
	    {dw = TOLERANCE*(wc[l+1] + wc[l]);
	     dw = max(dw, 1.0);
	     wc[l+1] = wc[l] + dw;};

/* WC to NDC transformations */
	 g->w_nd[l+1] = (g->ndc[l+1] - g->ndc[l])/(wc[l+1] - wc[l]);
	 g->w_nd[l]   = g->ndc[l] - wc[l]*g->w_nd[l+1];
 
/* NDC to WC transformations */
	 g->nd_w[l+1] = (wc[l+1] - wc[l])/(g->ndc[l+1] - g->ndc[l]);
	 g->nd_w[l]   = wc[l] - g->ndc[l]*g->nd_w[l+1];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_AXIS_LOG_SCALE - return the settings of the log axis flags
 *
 * #bind PG_fget_axis_log_scale fortran() scheme(pg-get-axis-log-scale)
 */

void PG_fget_axis_log_scale(PG_device *dev, int nd, int *iflg)
   {int id;
    PG_dev_geometry *g;

    if (dev != NULL)
       {g = &dev->g;

	for (id = 0; id < nd; id++)
	    iflg[id] = g->iflog[id];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_AXIS_LOG_SCALE - make sure that a log scale is handled correctly
 *
 * #bind PG_fset_axis_log_scale fortran() scheme(pg-set-axis-log-scale!)
 */

void PG_fset_axis_log_scale(PG_device *dev, int nd, int *iflg)
   {int id;
    double wc[PG_BOXSZ];
    PG_dev_geometry *g;

    if (dev != NULL)
       {g = &dev->g;

        PG_get_viewspace(dev, WORLDC, wc);

	for (id = 0; id < nd; id++)
	    g->iflog[id] = iflg[id];

	PG_set_viewspace(dev, 2, WORLDC, wc);};

    return;}

/*--------------------------------------------------------------------------*/

/*                          VIEWSPACE ROUTINES                              */

/*--------------------------------------------------------------------------*/
 
/* _PG_SET_SPACE_BND - set the boundary limits for the viewspace in WC
 *                   - the axes are drawn on the boundary
 *                   - and almost nobody else should use this routine
 */
 
static void INLINE _PG_set_space_BND(PG_device *dev, int nd, double *bnd)
   {int l, n;
    PG_dev_geometry *g;

    g = &dev->g;
    n = 2*nd;

    for (l = 0; l < n; l++)
        g->bnd[l] = bnd[l];
 
/* set transformations */
    _PG_fix_wtos(dev, TRUE);
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_SET_SPACE_NDC - set up the NDC for the viewspace of DEV
 *                   - the viewspace is that part of the window or screen
 *                   - which can be drawn on when clipping is on
 *                   - that is the viewspace and clipping space are synonomous
 *                   - viewspace is specified by two points XL and XU
 *                   - which are opposing corners of the space
 *                   - viewspace is specified in NDC
 */
 
static INLINE void _PG_set_space_NDC(PG_device *dev, int nd, double *ndc)
   {PG_dev_geometry *g;

    if (dev != NULL)
       {g = &dev->g;

	PG_frame_viewport(dev, nd, ndc);
	PG_box_copy(nd, g->ndc, ndc);

/* set transformations */
	_PG_fix_wtos(dev, TRUE);};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_SET_SPACE_FRM - set the frame for the device */
 
void _PG_set_space_FRM(PG_device *dev, int nd, double *frm)
   {double ndc[PG_BOXSZ];
    PG_dev_geometry *g;

    if (dev != NULL)
       {g = &dev->g;

	PG_get_viewspace(dev, NORMC, ndc);

	PG_box_copy(nd, g->fr, frm);

	PG_set_viewspace(dev, nd, NORMC, ndc);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_SET_SPACE_WC - set up the WC for the viewspace of DEV
 *                  - the viewspace is that part of the window or screen
 *                  - which can be drawn on when clipping is on
 *                  - that is the viewspace and clipping space are synonomous
 *                  - viewspace is specified by two points XL and XU
 *                  - which are opposing corners of the space
 */
 
static INLINE void _PG_set_space_WC(PG_device *dev, int nd, double *wc)
   {int l, n;
    double box[PG_BOXSZ];
    double dx;
    PG_dev_geometry *g;

    if (dev != NULL)
       {g = &dev->g;

	n = 2*nd;

/* copy WC so that we don't change it later on */
	PG_box_copy(nd, box, wc);
	    
/* for 2D spaces use the first dimension of the range extrema
 * to define a full 3D view space - this is consistent with
 * the multi-dimensional range surface plot
 */
	if ((nd < 3) && (dev->range_extrema != NULL))
	   {box[4] = dev->range_extrema[0];
	    box[5] = dev->range_extrema[1];

	    nd = 3;
	    n  = 2*nd;};

/* fill the WC limits */
	for (l = 0; l < n; l++)
	    g->wc[l] = box[l];
 
	_PG_log_transform(dev, nd, box);

/* pad the WC limits to form the BND limits */
	for (l = 0; l < n; l += 2)
	    {dx = box[l+1] - box[l];
	     if (dx < 0.0)
	        dx = min(dx, -SMALL);
	     else
	        dx = max(dx, SMALL);

	     box[l]   -= dx*g->pad[l];
	     box[l+1] += dx*g->pad[l+1];};

	_PG_set_space_BND(dev, nd, box);};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_VIEWSPACE - return the viewspace limits
 *
 * #bind PG_get_viewspace fortran() scheme()
 */

void PG_get_viewspace(PG_device *dev, PG_coord_sys cs, double *box)
   {PG_dev_geometry *g;

    g = &dev->g;

    switch (cs)
       {case BOUNDC :
	     PG_box_copy(PG_SPACEDM, box, g->bnd);

/* since this was not given to PG_set_viewspace as logs
 * don't return it that way
 */
	     PG_lin_space(dev, PG_SPACEDM, box);
             break;

        case WORLDC :
	     PG_box_copy(PG_SPACEDM, box, g->wc);
             break;

        case FRAMEC :
	     PG_box_copy(PG_SPACEDM, box, g->fr);
             break;

        case NORMC :
	     PG_box_copy(PG_SPACEDM, box, g->ndc);
             break;

        case PIXELC :
             _PG_box_pc(dev, PG_SPACEDM, NORMC, g->ndc, box);
	     if (box[2] > box[3])
	        {SC_SWAP_VALUE(double, box[2], box[3]);};
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_VIEWSPACE - set the view space from the given extremal box
 *                  - if CS == BOUNDC set BND
 *                  - if CS == WORLDC set WC
 *                  - if CS == NORMC  set NDC
 *
 * #bind PG_set_viewspace fortran() scheme()
 */

void PG_set_viewspace(PG_device *dev, int nd, PG_coord_sys cs, double *box)
   {double bx[PG_BOXSZ];

    if (box == NULL)
       {PG_box_init(nd, bx, 0.0, 1.0);
	box = bx;};

    switch (cs)
       {case BOUNDC :
	     _PG_set_space_BND(dev, nd, box);
	     break;

        case WORLDC :
	     _PG_set_space_WC(dev, nd, box);
	     break;

        case FRAMEC :
	     _PG_set_space_FRM(dev, nd, box);
	     break;

	case NORMC :
	     _PG_set_space_NDC(dev, nd, box);
	     break;

        default :
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_INIT_VIEWSPACE - initialize the viewspace from the
 *                   - device view members
 */

void PG_init_viewspace(PG_device *dev, int setw)
   {double ndc[PG_BOXSZ];

    PG_box_copy(2, ndc, dev->view_x);

    PG_set_viewspace(dev, 2, NORMC, ndc);

    if (setw == TRUE)
       PG_set_viewspace(dev, 2, WORLDC, NULL);
 
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_HULL - return the vector positions of the corners
 *             - of the box EXTR
 *             - also include the absolute position of the low corner
 */

double **PG_get_hull(PG_device *dev, double *extr, int offs)
   {int i, l;
    double dx[PG_SPACEDM];
    double **xc;
    PG_dev_geometry *g;

/* find offset/pad for extremal box */
    g = &dev->g;
    if (offs == TRUE)
       {for (i = 0; i < PG_SPACEDM; i++)
	    {l = 2*i;
	     dx[i] = (extr[l+1] - extr[l])*(g->pad[l] + g->pad[l+1]);};}
    else
       {for (i = 0; i < PG_SPACEDM; i++)
	    dx[i] = 0.0;};

    xc = PM_make_vectors(3, 8);

/* left down back */
    xc[0][0] = extr[0] - dx[0];
    xc[1][0] = extr[2] - dx[1];
    xc[2][0] = extr[4] - dx[2];

/* right down back */
    xc[0][1] = extr[1] + dx[0];
    xc[1][1] = extr[2] - dx[1];
    xc[2][1] = extr[4] - dx[2];

/* left up back */
    xc[0][2] = extr[0] - dx[0];
    xc[1][2] = extr[3] + dx[1];
    xc[2][2] = extr[4] - dx[2];

/* right up back */
    xc[0][3] = extr[1] + dx[0];
    xc[1][3] = extr[3] + dx[1];
    xc[2][3] = extr[4] - dx[2];

/* left down front */
    xc[0][4] = extr[0] - dx[0];
    xc[1][4] = extr[2] - dx[1];
    xc[2][4] = extr[5] + dx[2];

/* right down front */
    xc[0][5] = extr[1] + dx[0];
    xc[1][5] = extr[2] - dx[1];
    xc[2][5] = extr[5] + dx[2];

/* left up front */
    xc[0][6] = extr[0] - dx[0];
    xc[1][6] = extr[3] + dx[1];
    xc[2][6] = extr[5] + dx[2];

/* right up front */
    xc[0][7] = extr[1] + dx[0];
    xc[1][7] = extr[3] + dx[1];
    xc[2][7] = extr[5] + dx[2];

    return(xc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DVIEWBOX - diagnostic print for sorting out geometrical transformations */

void dviewbox(PG_dev_geometry *g, double *vbx, int n, double **x)
   {int i;
    double dbx[PG_BOXSZ];
    static char *nm[PG_SPACEDM] = {"x", "y", "z"};

    printf("view   : %10.3e %10.3e %10.3e %10.3e %10.3e %10.3e\n",
	   g->wc[0], g->wc[1], g->wc[2], g->wc[3], g->wc[4], g->wc[5]);

    printf("rotated: %10.3e %10.3e %10.3e %10.3e %10.3e %10.3e\n",
	   vbx[0], vbx[1], vbx[2], vbx[3], vbx[4], vbx[5]);

    PM_vector_extrema(3, n, x, dbx);

    printf("data   : %10.3e %10.3e %10.3e %10.3e %10.3e %10.3e\n",
	   dbx[0], dbx[1], dbx[2], dbx[3], dbx[4], dbx[5]);

    for (i = 0; i < PG_BOXSZ; i += 2)
        if ((dbx[i] < vbx[i]) || (vbx[i+1] < dbx[i]) ||
	    (dbx[i+1] < vbx[i]) || (vbx[i+1] < dbx[i+1]))
	    printf("   data outside view limits in %s direction\n",
		   nm[i >> 1]);

    printf("\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RIGHT_SPACE_BOX - get the vectors into the right place */

static void _PG_right_space_box(PG_device *dev, int n, double **x,
				double *xo)
   {double vbx[PG_BOXSZ];
    double **xc;
    PG_dev_geometry *g;

    g  = &dev->g;
    xc = PG_get_hull(dev, g->wc, FALSE);

/* rotate the box corner vectors
 * NOTE: use PM_rotate_vectors instead of PG_rotate_vectors to
 * avoid infinite recursion
 */
    PM_rotate_vectors(3, 8, xc, xo, g->view_angle);

    PM_vector_extrema(3, 8, xc, vbx);

    PG_conform_view(dev, 3, WORLDC, vbx, n, x);

    PM_free_vectors(3, xc);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_SPACE_BOX - return the vector positions of the corners
 *                  - of the view space box in WC and rotated
 *                  - also include the absolute position of the low corner
 */

double **PG_get_space_box(PG_device *dev, double *extr, int offs)
   {double box[PG_BOXSZ];
    double **xc;

    xc = PG_get_hull(dev, extr, offs);

/* rotate the box corner vectors */
    PG_rotate_vectors(dev, 3, 8, xc);

    PM_vector_extrema(3, 8, xc, box);

    PG_conform_view(dev, 3, WORLDC, box, 8, xc);

    return(xc);}

/*--------------------------------------------------------------------------*/

/*                                FRAME ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* _PG_TR_NDC_TO_FRM_B - convert one NDC component X to FRAMEC absolute */

static INLINE double _PG_tr_ndc_to_frm_b(PG_device *dev, int id,
					 int nd, PG_coord_sys cs, double x)
   {int l;
    double f, df, ua, ub, x1, x2, f1, f2;
    PG_dev_geometry *g;

    g = &dev->g;

    l  = 2*id;
    f  = g->fr[l];
    df = g->fr[l+1] - f;

    switch (cs)
       {case WORLDC :
             x1 = g->wc[l];
             x2 = g->wc[l+1];
             f1 = g->ndc[l];
             f2 = g->ndc[l+1];
	     ua = (f2*x1 - f1*x2)/(f2 - f1);
	     if (f1 != 0.0)
	        ub = (x1 + (1.0 + f1)*ua)/f1;
	     else
		ub = (x2 + (1.0 + f2)*ua)/f2;
             break;
        case NORMC :
             ua = 0.0;
             ub = 1.0;
             break;
        case PIXELC :
             ua = 0.0;
             ub = g->pc[l+1];
             break;
        default :
             break;};

    x = ua + (ub - ua)*f + (x - ua)*df;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FRAME_POINTS - shift N ND points XI from window relative to
 *                 - XO in frame relative values
 */

void PG_frame_points(PG_device *dev, int nd, PG_coord_sys cs, long n, double **xi,
		     double **xo)
   {int i, id;
    double *pi, *po;

    for (id = 0; id < nd; id++)
        {pi = xi[id];
	 po = xo[id];
	 for (i = 0; i < n; i++)
	     po[i] = _PG_tr_ndc_to_frm_b(dev, id, nd, cs, pi[i]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FRAME_POINT - shift an ND point XI from window relative to
 *                - XO in frame relative values
 */

void PG_frame_point(PG_device *dev, int nd, PG_coord_sys cs, double *xi, double *xo)
   {int id;
    double *o[PG_SPACEDM], *n[PG_SPACEDM];

    for (id = 0; id < nd; id++)
        {o[id] = &xi[id];
         n[id] = &xo[id];};

    PG_frame_points(dev, nd, cs, 1, o, n);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FRAME_INTERVAL - scale an ND point DXI from window relative to
 *                   - DXO in frame relative values
 */

void PG_frame_interval(PG_device *dev, int nd, double *dxi, double *dxo)
   {int id, l;
    double df;
    PG_dev_geometry *g;

    if (dev != NULL)
       {g = &dev->g;

	for (id = 0; id < nd; id++)
	    {l  = 2*id;
	     df = g->fr[l+1] - g->fr[l];
	     dxo[id] = df*dxi[id];};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_FRAME_BOX - shift BOX from window relative to frame
 *              - relative values
 */
 
void PG_frame_box(PG_device *dev, int nd, PG_coord_sys cs, double *box)
   {int id, l;
    double mn, mx;

    if (dev != NULL)
       {for (id = 0; id < nd; id++)
	    {l = 2*id;

	     mn = min(box[l], box[l+1]);
	     mx = max(box[l], box[l+1]);

	     box[l]   = _PG_tr_ndc_to_frm_b(dev, id, nd, cs, mn);
	     box[l+1] = _PG_tr_ndc_to_frm_b(dev, id, nd, cs, mx);};}

    else
       PG_box_init(nd, box, 0.0, 1.0);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_FRAME_VIEWPORT - shift a viewport from window relative to frame
 *                   - relative values
 */
 
void PG_frame_viewport(PG_device *dev, int nd, double *ndc)
   {int id, l;
    double mn, mx, fn, df;
    PG_dev_geometry *g;

    if (dev != NULL)
       {g = &dev->g;

	for (id = 0; id < nd; id++)
	    {l = 2*id;

	     mn = min(ndc[l], ndc[l+1]);
	     mx = max(ndc[l], ndc[l+1]);

	     fn = g->fr[l];
	     df = g->fr[l+1] - fn;

	     ndc[l]   = fn + mn*df;
	     ndc[l+1] = fn + mx*df;};}

    else
       {for (id = 0; id < nd; id++)
	    {l = 2*id;
	     ndc[l]   = 0.0;
	     ndc[l+1] = 1.0;};};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_VIEWPORT_FRAME - shift a viewport from frame relative to window
 *                   - relative values
 */
 
void PG_viewport_frame(PG_device *dev, int nd, double *ndc)
   {int id, l;
    double fn, df;
    PG_dev_geometry *g;

    if (dev != NULL)
       {g = &dev->g;

	for (id = 0; id < PG_SPACEDM; id++)
	    {l  = 2*id;
	     fn = g->fr[l];
	     df = 1.0/(g->fr[l+1] - fn + SMALL);

	     ndc[l]   = (g->ndc[l] - fn)*df;
	     ndc[l+1] = (g->ndc[l+1] - fn)*df;};}

    else
       {for (id = 0; id < nd; id++)
	    {l = 2*id;
	     ndc[l]   = 0.0;
	     ndc[l+1] = 1.0;};};

    return;}
 
/*--------------------------------------------------------------------------*/

/*                        VIEW/LIGHT ANGLE ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* PG_FGET_VIEW_ANGLE - get the view angle in degrees if CNV is TRUE
 *                    - otherwise in radians
 *
 * #bind PG_fget_view_angle fortran() scheme()
 */

void PG_fget_view_angle(PG_device *dev, int cnv,
		       double *pt, double *pp, double *pc)
   {double cf;
    PG_dev_geometry *g;

    g  = &dev->g;
    cf = (cnv == TRUE) ? RAD_DEG : 1.0;

    if (pt != NULL)
       *pt = cf*g->view_angle[0];

    if (pp != NULL)
       *pp = cf*g->view_angle[1];

    if (pc != NULL)
       *pc = cf*g->view_angle[2];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_VIEW_ANGLE - set the view angle in degrees for DEV
 *
 * #bind PG_fset_view_angle fortran() scheme()
 */

void PG_fset_view_angle(PG_device *dev,
			double theta ARG(0.0,in),
			double phi ARG(0.0,in),
			double chi ARG(0.0,in))
   {PG_dev_geometry *g;

    g = &dev->g;

    if (theta != HUGE)
       g->view_angle[0] = DEG_RAD*theta;

    if (phi != HUGE)
       g->view_angle[1] = DEG_RAD*phi;

    if (chi != HUGE)
       g->view_angle[2] = DEG_RAD*chi;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_LIGHT_ANGLE - get the lighting angle in degrees if CNV is TRUE
 *                     - otherwise in radians
 *
 * #bind PG_fget_light_angle fortran() scheme()
 */

void PG_fget_light_angle(PG_device *dev, int cnv, double *pt, double *pp)
   {double cf;

    cf = (cnv == TRUE) ? RAD_DEG : 1.0;

    if (pt != NULL)
       *pt = cf*dev->light_angle[0];

    if (pp != NULL)
       *pp = cf*dev->light_angle[1];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_LIGHT_ANGLE - set the light angle in degrees for DEV
 *
 * #bind PG_fset_light_angle fortran() scheme()
 */

void PG_fset_light_angle(PG_device *dev,
			 double theta ARG(0.0,in),
			 double phi ARG(0.0,in))
   {

    if (theta != HUGE)
       dev->light_angle[0] = DEG_RAD*theta;

    if (phi != HUGE)
       dev->light_angle[1] = DEG_RAD*phi;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CONFORM_TRANS - do the tranformation to make X in EXTR fit in DST */

static INLINE void _PG_conform_trans(int nd, int n, double **x,
				     double *sb, double *db)
   {int l, id, dos, dot;
    double smn, smx, dmn, dmx, sf, tf;
    double tr[PG_SPACEDM], sc[PG_SPACEDM], box[PG_BOXSZ];

    if (sb == NULL)
       {PM_vector_extrema(nd, n, x, box);
	sb = box;};

    dos = FALSE;
    dot = FALSE;

    for (id = 0; id < nd; id++)
        {l = 2*id;

         smn = sb[l];
	 smx = sb[l+1];

	 dmn = db[l];
	 dmx = db[l+1];

	 sf = (smn == smx) ? 0.0 : (dmx - dmn)/(smx - smn);

#if 1
	 sf = min(sf, 1.0);

/* translate from the scaled center */
	 tf = 0.5*((dmn + dmx) - sf*(smn + smx));

#else
	 tf = 0.5*((dmn + dmx) - (smn + smx));
#endif

	 dos |= (sf != 1.0);
	 dot |= (tf != 0.0);

	 sc[id] = sf;
	 tr[id] = tf;};

/* scale to db */
    if (dos == TRUE)
       PM_scale_vectors(nd, n, x, sc);

/* translate to db */
    if (dot == TRUE)
       PM_translate_vectors(nd, n, x, tr);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CONFORM_WC - translate and scale a set of vectors in a box in WC
 *                - so that it fits in view space
 */

static INLINE void _PG_conform_wc(PG_device *dev, int nd, double *box,
				  int n, double **x)
   {PG_dev_geometry *g;

    g = &dev->g;

    _PG_conform_trans(nd, n, x, box, g->wc);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CONFORM_NDC - translate and scale a set of vectors in a box in NDC
 *                 - so that it fits in view space
 */

static INLINE void _PG_conform_ndc(PG_device *dev, int nd, double *box,
				   int n, double **x)
   {PG_dev_geometry *g;

    g = &dev->g;

    _PG_conform_trans(nd, n, x, box, g->ndc);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CONFORM_PC - translate and scale a set of vectors in a box in PC
 *                - so that it fits in view space
 */

static INLINE void _PG_conform_pc(PG_device *dev, int nd, double *box,
				  int n, double **x)
   {PG_dev_geometry *g;

    g = &dev->g;

    _PG_conform_trans(nd, n, x, box, g->pc);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CONFORM_VIEW - translate and scale a set of vectors in a box
 *                 - so that it fits in view space
 *                 - typically done after rotations
 */

void PG_conform_view(PG_device *dev, int nd, PG_coord_sys cs, double *box,
		     int n, double **x)
   {

    switch (cs)
       {case WORLDC :
	     _PG_conform_wc(dev, nd, box, n, x);
             break;
	case NORMC :
	     _PG_conform_ndc(dev, nd, box, n, x);
             break;
	case PIXELC :
	     _PG_conform_pc(dev, nd, box, n, x);
             break;
	default :
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_ROTATE_VECTORS - rotate N ND vectors X about X0 by Euler angles VA */

void PG_rotate_vectors(PG_device *dev, int nd, int n, double **x)
   {int id, l;
    double xo[PG_SPACEDM];
    PG_dev_geometry *g;

    g = &dev->g;
    
    for (id = 0; id < nd; id++)
        {l      = 2*id;
	 xo[id] = 0.5*(g->wc[l] + g->wc[l+1]);};

    PM_rotate_vectors(nd, n, x, xo, g->view_angle);

    _PG_right_space_box(dev, n, x, xo);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

