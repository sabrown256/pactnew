/*
 * PGS.JS - PGS drawing routines for HTML5 canvas and JavaScript
 *
 * #include "cpyright.h"
 *
 */

/*
enum e_PG_coord_sys
   {WORLDC = 1, NORMC, PIXELC, BOUNDC, FRAMEC};
typedef enum e_PG_coord_sys PG_coord_sys;
*/
var WORLDC = 1;
var NORMC  = 2;
var PIXELC = 3;
var BOUNDC = 4;
var FRAMEC = 5;

var PG_SPACEDM = 3;
var PG_BOXSZ   = 6;

var LINE_SOLID     = 1;
var LINE_DASHED    = 2;
var LINE_DOTTED    = 3;
var LINE_DOTDASHED = 4;

var BLACK        = '#000000';
var WHITE        = '#FFFFFF';
var GRAY         = '#AAAAAA';
var DARK_GRAY    = '#444444';
var BLUE         = '#0000FF';
var GREEN        = '#00FF00';
var CYAN         = '#00FFFF';
var RED          = '#FF0000';
var MAGENTA      = '#FF00FF';
var BROWN        = '#AAAA00';
var DARK_BLUE    = '#0000AA';
var DARK_GREEN   = '#00AA00';
var DARK_CYAN    = '#00AAAA';
var DARK_RED     = '#AA0000';
var YELLOW       = '#FFFF00';
var DARK_MAGENTA = '#AA00AA';

var TRUE      = 1;
var FALSE     = 0;
var NULL      = 0x0;
var TOLERANCE = 1.0e-10;
var SMALL     = 1.0e-100;
var HUGE      = 1.0e100;

var INT_MIN = -(1 << 20);
var INT_MAX = (1 << 20);

var gs_phys_width     = 0;
var gs_phys_height    = 0;
var gs_phys_n_colors  = 0;
var gs_axis_n_decades = 8;
var gs_clipping       = FALSE;
var gs_stdscr         = -1;

var gs_fr   = new Array(0.0, 1.0, 0.0, 1.0);
var gs_bnd  = new Array(0.0, 1.0, 0.0, 1.0);
var gs_wc   = new Array(0.0, 1.0, 0.0, 1.0);
var gs_ndc  = new Array(0.0, 1.0, 0.0, 1.0);
var gs_pc   = new Array(0, 1000, 0, 1000);
var gs_cpc  = new Array(0, 100, 0, 100);
var gs_nd_w = new Array(0.0, 1.0, 0.0, 1.0);
var gs_w_nd = new Array(0.0, 1.0, 0.0, 1.0);

var gs_hwin     = new Array(0, 100, 0, 100);
var gs_iflog    = new Array(FALSE, FALSE, FALSE);
var gs_pad      = new Array(0.01, 0.01, 0.01, 0.01);
var gs_view_x   = new Array(0.0, 1.0, 0.0, 1.0);
var gs_window_x = new Array(0.0, 1.0, 0.0, 1.0);

var gs_gcur = new Array(0.0, 0.0, 0.0);
var gs_tcur = new Array(0.0, 0.0, 0.0);

var gs_text_color = BLACK;
var gs_fill_color = BLACK;
var gs_line_color = BLACK;
var gs_line_style = LINE_SOLID;
var gs_line_width = 0.0;

var gs_font_name  = "Helvetica";
var gs_font_style = "medium";
var gs_font_size  = 12;

// var person={firstname:"John", lastname:"Doe", id:5566};
// name=person.lastname;
// name=person["lastname"]; 
// var carname=new String;
// var x=      new Number;
// var y=      new Boolean;
// var cars=   new Array;
// var person= new Object;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COPY_ARRAY - synatically clean array copier */

function copy_array(x)
   {var y;

    y = x.slice(0);

    return(y);}

/*--------------------------------------------------------------------------*/

/*                      COORDINATE SYSTEM ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* PG_LOG_POINT - get WC point P values correctly transformed for log scales */

function PG_log_point(dev, nd, p)
   {var id, pc;

    for (id = 0; id < nd; id++)
        {pc = p[id];

/* if log axis options have been used take log */
	 if (gs_iflog[id] == TRUE)
	    pc = log10(Math.abs(pc) + SMALL);

	 p[id] = pc;};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SET_PC_FROM_HWIN = set the device PC limits */

function SET_PC_FROM_HWIN()
   {

    gs_pc[0] = 0.0;
    gs_pc[1] = gs_hwin[1] - gs_hwin[0];
    gs_pc[2] = 0.0;
    gs_pc[3] = gs_hwin[3] - gs_hwin[2];
    gs_pc[4] = 0.0;
    gs_pc[5] = Math.min(gs_pc[1], gs_pc[3]);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_QUAD_FOUR_POINT - convert X to quadrant 4 */

function PG_QUAD_FOUR_POINT(dev, x)
   {

    x[1] = gs_pc[3] - x[1];

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_QUAD_FOUR_BOX - convert box B to be quadrant 4 */

function PG_QUAD_FOUR_BOX(dev, bx)
   {

    bx[2] = gs_pc[3] - bx[3];

    return(bx);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_LOG_TRANSFORM - get WC values correctly transformed for
 *                   - log scales
 */

function _PG_log_transform(dev, nd, box)
   {var id, l, mn, mx, ma;

    for (id = 0; id < nd; id++)
        {l = 2*id;

	 mn = box[l];
	 mx = box[l+1];

/* if log axis options have been used
 * take logs of min and max
 */
	 if (gs_iflog[id])
	    {mx = log10(Math.abs(mx) + SMALL);
	     mn = log10(Math.abs(mn) + SMALL);
	     ma = mx - gs_axis_n_decades;

/* limit mn to be within PG_axis_n_decades of the max value */
	     mn = Math.max(mn, ma);};

	 box[l]   = mn;
	 box[l+1] = mx;};

    return(box);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FIX_WTOS - compute the WC to NDC transformation coefficients */

function _PG_fix_wtos(dev, wh)
   {var l, dw, wc;

    wc = new Array();

/* use the WC limits of the viewport */
    if (wh == TRUE)
       {for (l = 0; l < PG_BOXSZ; l++)
	    wc[l] = gs_wc[l];

	wc = _PG_log_transform(dev, PG_SPACEDM, wc);}

/* use the WC limits of the boundary */
    else
       {for (l = 0; l < PG_BOXSZ; l++)
	    wc[l] = gs_bnd[l];};

    for (id = 0; id < PG_SPACEDM; id++)
        {l = 2*id;
	 if (wc[l] == wc[l+1])
	    {dw = TOLERANCE*(wc[l+1] + wc[l]);
	     dw = Math.max(dw, 1.0);
	     wc[l+1] = wc[l] + dw;};

/* WC to NDC transformations */
	 gs_w_nd[l+1] = (gs_ndc[l+1] - gs_ndc[l])/(wc[l+1] - wc[l]);
	 gs_w_nd[l]   = gs_ndc[l] - wc[l]*gs_w_nd[l+1];
 
/* NDC to WC transformations */
	 gs_nd_w[l+1] = (wc[l+1] - wc[l])/(gs_ndc[l+1] - gs_ndc[l]);
	 gs_nd_w[l]   = wc[l] - gs_ndc[l]*gs_nd_w[l+1];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TRANS_CP - copy N ND XI to XO for identity transform */

function _PG_trans_cp(n, nd, xi)
   {var id, pi, po, xo;

    xo = new Array();

    for (id = 0; id < nd; id++)
        {pi = xi[id];
         po = new Array();

         for (i = 0; i < n; i++)
             po[i] = pi[i];

	 xo[id] = po;};

    return(xo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_WC_TO_NDC_A - convert one WC component X to NDC absolute */

function _PG_tr_wc_to_ndc_a(dev, id, nd, x)
   {var l;

    l = 2*id;
    x = gs_w_nd[l] + gs_w_nd[l+1]*x;
    x = Math.max(x, -10.0);
    x = Math.min(x, 11.0);

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_NDC_TO_WC_A - convert one NDC component X to WC absolute */

function _PG_tr_ndc_to_wc_a(dev, id, nd, x)
   {var l;

    l = 2*id;
    x = gs_nd_w[l] + gs_nd_w[l+1]*x;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_NDC_TO_FRM_A - convert one NDC component X to FRAMEC absolute */

function _PG_tr_ndc_to_frm_a(dev, id, nd, x)
   {var l, f, df;

    l  = 2*id;
    f  = gs_fr[l];
    df = gs_fr[l+1] - f;

    x = f + x*df;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_NDC_TO_PC_A - convert one NDC component X to PC absolute */

function _PG_tr_ndc_to_pc_a(dev, id, nd, x, wo)
   {var l, w, p, ix;

    l = 2*id;
    w = gs_pc[l+1] - gs_pc[l];
              
    x = Math.max(x, -10.0);
    x = Math.min(x, 11.0);

    p  = x*w + 0.5;
    ix = Math.floor(p) + wo[id];

    ix = Math.max(ix, gs_cpc[l]);
    ix = Math.min(ix, gs_cpc[l+1]);

    p = ix;

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_NDC_TO_PC_D - convert one NDC delta (XL,XU) to PC */

function _PG_tr_ndc_to_pc_d(dev, id, nd, xl, xu)
   {var l, w, dx, idx;

    l = 2*id;
    w = gs_pc[l+1] - gs_pc[l];
              
    xl = Math.max(xl, -10.0);
    xu = Math.min(xu,  11.0);
    dx = xu - xl;

    idx = Math.floor(w*dx + 0.5);

    return(idx);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_PC_TO_NDC_A - convert one PC component IX to NDC absolute */

function _PG_tr_pc_to_ndc_a(dev, id, nd, ix, wo)
   {var l, w, x;

    l = 2*id;
    w = gs_pc[l+1] - gs_pc[l];

    x = (ix - wo[id])/w;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_WC_TO_PC_A - convert one WC component X to PC absolute */

function _PG_tr_wc_to_pc_a(dev, id, nd, x, wo)
   {var l, w, p, ix, y;

    l = 2*id;
    w = gs_pc[l+1] - gs_pc[l];
              
/* WORLDC to NDC */
    x = gs_w_nd[l] + gs_w_nd[l+1]*x;

    x = Math.max(x, -10.0);
    x = Math.min(x, 11.0);

/* NDC to PC */
    p  = x*w + 0.5;
    ix = Math.floor(p) + wo[id];

    ix = Math.max(ix, gs_cpc[l]);
    ix = Math.min(ix, gs_cpc[l+1]);

    return(ix);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_PC_TO_WC_A - convert one PC component IX to WC absolute */

function _PG_tr_pc_to_wc_a(dev, id, nd, ix, wo)
   {var l, w, x;

    l = 2*id;
    w = gs_pc[l+1] - gs_pc[l];

/* from PC to NDC */
    x = (ix - wo[id])/w;

/* from NDC to WC */
    x = gs_nd_w[l] + gs_nd_w[l+1]*x;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_WC_TO_NDC - convert N ND XI in WC to XO in NDC */

function _PG_tr_wc_to_ndc(dev, n, nd, xi)
   {var id, i, pi, po;
    var xo;

    xo = new Array();

    for (id = 0; id < nd; id++)
        {pi = xi[id];
         po = new Array();

         for (i = 0; i < n; i++)
             po[i] = _PG_tr_wc_to_ndc_a(dev, id, nd, pi[i]);

	 xo[id] = po;};

    return(xo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_NDC_TO_WC - convert N ND XI in NDC to XO in WC */

function _PG_tr_ndc_to_wc(dev, n, nd, xi)
   {var id, i, pi, po, xo;

    xo = new Array();

    for (id = 0; id < nd; id++)
        {pi = xi[id];
	 po = new Array();
         for (i = 0; i < n; i++)
             po[i] = _PG_tr_ndc_to_wc_a(dev, id, nd, pi[i]);
	 xo[id] = po;};

    return(xo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_NDC_TO_FRM - convert N ND XI in NDC to XO in FRAMEC */

function _PG_tr_ndc_to_frm(dev, n, nd, xi)
   {var id, i, pi, po, xo;

    xo = new Array();

    for (id = 0; id < nd; id++)
        {pi = xi[id];
         po = new Array();

         for (i = 0; i < n; i++)
             po[i] = _PG_tr_ndc_to_frm_a(dev, id, nd, pi[i]);

	 xo[id] = po;};

    return(xo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_NDC_TO_PC - convert N ND XI in NDC to XO in PC */

function _PG_tr_ndc_to_pc(dev, n, nd, xi)
   {var id, i, pi, po;
    var wo, xo;

    wo = new Array();
    xo = new Array();

    for (id = 0; id < nd; id++)
        wo[id] = gs_window_x[2*id];

    for (id = 0; id < nd; id++)
        {pi = xi[id];
         po = new Array();

         for (i = 0; i < n; i++)
             po[i] = _PG_tr_ndc_to_pc_a(dev, id, nd, pi[i], wo);

	 xo[id] = po;};

    return(xo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_PC_TO_NDC - convert N ND XI in PC to XO in NDC */

function _PG_tr_pc_to_ndc(dev, n, nd, xi)
   {var id, i, pi, po;
    var wo, xo;

    wo = new Array();
    xo = new Array();

    for (id = 0; id < nd; id++)
        wo[id] = gs_window_x[2*id];

    for (id = 0; id < nd; id++)
        {pi = xi[id];
         po = new Array();

         for (i = 0; i < n; i++)
             po[i] = _PG_tr_pc_to_ndc_a(dev, id, nd, pi[i], wo);

	 xo[id] = po;};

    return(xo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_WC_TO_PC - convert N ND XI in WC to XO in PC */

function _PG_tr_wc_to_pc(dev, n, nd, xi)
   {var id, i, po;
    var wo, xo;

    wo = new Array();
    xo = new Array();

    for (id = 0; id < nd; id++)
        wo[id] = gs_window_x[2*id];

    for (id = 0; id < nd; id++)
        {po = new Array();
         
         for (i = 0; i < n; i++)
             po[i] = _PG_tr_wc_to_pc_a(dev, id, nd, xi[id][i], wo);

	 xo[id] = po;};

    return(xo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_PC_TO_WC - convert N ND XI in PC to XO in WC */

function _PG_tr_pc_to_wc(dev, n, nd, xi)
   {var id, i, pi, po;
    var wo, xo;

    wo = new Array();
    xo = new Array();

    for (id = 0; id < nd; id++)
        wo[id] = gs_window_x[2*id];

    for (id = 0; id < nd; id++)
        {pi = xi[id];
         po = new Array();

         for (i = 0; i < n; i++)
             po[i] = _PG_tr_pc_to_wc_a(dev, id, nd, pi[i], wo);

         xo[id] = po;};

    return(xo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_WC_TO - convert N ND XI in WC to XO in OCS */

function _PG_tr_wc_to(dev, n, nd, ocs, xi)
   {var xo;

    switch (ocs)
       {case WORLDC :
             xo = _PG_trans_cp(n, nd, xi);
             break;

        case NORMC :
             xo = _PG_tr_wc_to_ndc(dev, n, nd, xi);
             break;

        case PIXELC :
             xo = _PG_tr_wc_to_pc(dev, n, nd, xi);
             break;

        default :
             xo = NULL;
             break;};

    return(xo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_NDC_TO - convert N ND XI in NDC to XO in OCS */

function _PG_tr_ndc_to(dev, n, nd, ocs, xi)
   {var xo;

    switch (ocs)
       {case WORLDC :
             xo = _PG_tr_ndc_to_wc(dev, n, nd, xi);
             break;

        case FRAMEC :
             xo = _PG_tr_ndc_to_frm(dev, n, nd, xi);
             break;

        case NORMC :
             xo = _PG_trans_cp(n, nd, xi);
             break;

        case PIXELC :
             xo = _PG_tr_ndc_to_pc(dev, n, nd, xi);
             break;

        default :
             xo = NULL;
             break;};

    return(xo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TR_PC_TO - convert N ND XI in PC to XO in OCS */

function _PG_tr_pc_to(dev, n, nd, ocs, xi)
   {var xo;

    switch (ocs)
       {case WORLDC :
             xo = _PG_tr_pc_to_wc(dev, n, nd, xi);
             break;

        case NORMC :
             xo = _PG_tr_pc_to_ndc(dev, n, nd, xi);
             break;

        case PIXELC :
             xo = _PG_trans_cp(n, nd, xi);
             break;

        default :
             xo = NULL;
             break;};

    return(xo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TRANS_POINTS - convert N ND points XI in ICS to XO in OCS
 *                 - NOTE: vectors in PC are really integer but are
 *                 - represented here as double
 */

function PG_trans_points(dev, n, nd, ics, xi, ocs)
   {var xo;

    switch (ics)
       {case WORLDC :
             xo = _PG_tr_wc_to(dev, n, nd, ocs, xi);
             break;

        case NORMC :
             xo = _PG_tr_ndc_to(dev, n, nd, ocs, xi);
             break;

        case PIXELC :
             xo = _PG_tr_pc_to(dev, n, nd, ocs, xi);
             break;

        default :
             xo = NULL;
             break;};

    return(xo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TRANS_POINT - convert a ND point XI in ICS to XO in OCS
 *                - NOTE: vectors in PC are really integer but are
 *                - represented here as double
 */

function PG_trans_point(dev, nd, ics, xi, ocs)
   {var id;
    var o, n, p, xo;

    o = new Array();
    n = new Array();

    for (id = 0; id < nd; id++)
        {p = new Array();
         p[0]  = xi[id];
         o[id] = p;};

    xo = PG_trans_points(dev, 1, nd, ics, o, ocs);

    for (id = 0; id < nd; id++)
        n[id] = xo[id][0];

    return(n);}

/*--------------------------------------------------------------------------*/

/*                              BOX ROUTINES                                */

/*--------------------------------------------------------------------------*/

/* PG_BOX_COPY - copy box S to D */

function PG_box_copy(nd, s)
   {var l, n;
    var d;

    d = new Array();

    n = 2*nd;
    for (l = 0; l < n; l++)
        d[l] = s[l];

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_BOX_PC - convert a box BI in CS coordinates to PC */

function _PG_box_pc(dev, nd, cs, bi)
   {var n, l, id, ix, idx;
    var wo, pc;

    wo = new Array();
    pc = new Array();

    n = nd << 1;

/* guard against conversions in place */
    if (bi == pc)
       bi = PG_box_copy(nd, bi);

    for (id = 0; id < nd; id++)
        wo[id] = gs_window_x[2*id];

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
                 pc[l] = Math.floor(bi[l] + 0.5);
             break;};

    return(pc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_BOX_NDC - convert a box BI in CS coordinates to NDC */

function _PG_box_ndc(dev, nd, cs, bi)
   {var n, l, id, ix, idx;
    var wo, ndc;

    wo  = new Array();
    ndc = new Array();

    n = nd << 1;

/* guard against conversions in place */
    if (bi == ndc)
       bi = PG_box_copy(nd, bi);

    switch (cs)
       {case WORLDC :
             for (l = 0; l < n; l++)
                 {id     = l >> 1;
                  ndc[l] = _PG_tr_wc_to_ndc_a(dev, id, nd, bi[l]);};
             break;

        case NORMC :
             ndc = PG_box_copy(nd, bi);
             break;

        case PIXELC :
             for (id = 0; id < nd; id++)
                 wo[id] = gs_window_x[2*id];
             for (l = 0; l < n; l++)
                 {id     = l >> 1;
                  ndc[l] = _PG_tr_pc_to_ndc_a(dev, id, nd, bi[l], wo);};
             break;

        default :
             break;};

    return(ndc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_BOX_WC - convert a box BI in CS coordinates to WC */

function _PG_box_wc(dev, nd, cs, bi)
   {var n, l, id;
    var wo, wc;

    wo = new Array();
    wc = new Array();

    n = nd << 1;

/* guard against conversions in place */
    if (bi == wc)
       bi = PG_box_copy(nd, bi);

    switch (cs)
       {case WORLDC :
             wc = PG_box_copy(nd, bi);
             break;

        case PIXELC :
             for (id = 0; id < nd; id++)
                 wo[id] = gs_window_x[2*id];
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

    return(wc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TRANS_BOX - convert a box BI in ICS coordinates to
 *              - OCS coordinates BO
 */

function PG_trans_box(dev, nd, ics, bi, ocs)
   {

    switch (ocs)
       {case WORLDC :
             bo = _PG_box_wc(dev, nd, ics, bi);
             break;

        case NORMC :
             bo = _PG_box_ndc(dev, nd, ics, bi);
             break;

        case PIXELC :
             bo = _PG_box_pc(dev, nd, ics, bi);
             break;

        default :
             break;};

    return(bo);}

/*--------------------------------------------------------------------------*/

/*                              FRAME ROUTINES                              */

/*--------------------------------------------------------------------------*/
 
/* PG_FRAME_VIEWPORT - shift a viewport from window relative to frame
 *                   - relative values
 */
 
function PG_frame_viewport(dev, nd, ndc)
   {var id, l, mn, mx, fn, df;

    if (dev != NULL)
       {

	for (id = 0; id < nd; id++)
	    {l = 2*id;

	     mn = Math.min(ndc[l], ndc[l+1]);
	     mx = Math.max(ndc[l], ndc[l+1]);

	     fn = gs_fr[l];
	     df = gs_fr[l+1] - fn;

	     ndc[l]   = fn + mn*df;
	     ndc[l+1] = fn + mx*df;};}

    else
       {for (id = 0; id < nd; id++)
	    {l = 2*id;
	     ndc[l]   = 0.0;
	     ndc[l+1] = 1.0;};};

    return(ndc);}
 
/*--------------------------------------------------------------------------*/

/*                          VIEWSPACE ROUTINES                              */

/*--------------------------------------------------------------------------*/
 
/* _PG_SET_SPACE_BND - set the boundary limits for the viewspace in WC
 *                   - the axes are drawn on the boundary
 *                   - and almost nobody else should use this routine
 */
 
function _PG_set_space_BND(dev, nd, bnd)
   {var n, l;

    n = 2*nd;

    for (l = 0; l < n; l++)
        gs_bnd[l] = bnd[l];
 
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
 
function _PG_set_space_NDC(dev, nd, ndc)
   {

    if (dev != NULL)
       {PG_frame_viewport(dev, nd, ndc);
	gs_ndc = PG_box_copy(nd, ndc);

/* set transformations */
	_PG_fix_wtos(dev, TRUE);};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_SET_SPACE_FRM - set the frame for the device */
 
function _PG_set_space_FRM(dev, nd, frm)
   {

    if (dev != NULL)
       {var ndc;

        ndc = new Array();

	PG_get_viewspace(dev, NORMC, ndc);

	gs_fr = PG_box_copy(nd, frm);

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
 
function _PG_set_space_WC(dev, nd, wc)
   {var n, l, dx;
    var box;

    if (dev != NULL)
       {box = new Array();

	n = 2*nd;

/* copy WC so that we don't change it later on */
	box = PG_box_copy(nd, wc);
	    
/* for 2D spaces use the first dimension of the range extrema
 * to define a full 3D view space - this is consistent with
 * the multi-dimensional range surface plot
 */
/*
	if ((nd < 3) && (dev->range_extrema != NULL))
	   {box[4] = dev->range_extrema[0];
	    box[5] = dev->range_extrema[1];

	    nd = 3;
	    n  = 2*nd;};
*/

/* fill the WC limits */
	for (l = 0; l < n; l++)
	    gs_wc[l] = box[l];
 
	box = _PG_log_transform(dev, nd, box);

/* pad the WC limits to form the BND limits */
	for (l = 0; l < n; l += 2)
	    {dx = box[l+1] - box[l];
	     if (dx < 0.0)
	        dx = Math.min(dx, -SMALL);
	     else
	        dx = Math.max(dx, SMALL);

	     box[l]   -= dx*gs_pad[l];
	     box[l+1] += dx*gs_pad[l+1];};

	_PG_set_space_BND(dev, nd, box);};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_VIEWSPACE - return the viewspace limits */

function PG_get_viewspace(dev, cs, box)
   {

    switch (cs)
       {case BOUNDC :
             box = PG_box_copy(PG_SPACEDM, gs_bnd);

/* since this was not given to PG_set_viewspace as logs
 * don't return it that way
 */
             PG_lin_space(dev, PG_SPACEDM, box);
             break;

        case WORLDC :
             box = PG_box_copy(PG_SPACEDM, gs_wc);
             break;

        case FRAMEC :
             box = PG_box_copy(PG_SPACEDM, gs_fr);
             break;

        case NORMC :
             box = PG_box_copy(PG_SPACEDM, gs_ndc);
             break;

        case PIXELC :
             box = _PG_box_pc(dev, PG_SPACEDM, NORMC, gs_ndc);
             if (box[2] > box[3])
                {SC_SWAP_VALUE(double, box[2], box[3]);};
             break;};

    return(box);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_VIEWSPACE - set the view space from the given extremal box
 *                  - if CS == BOUNDC set BND
 *                  - if CS == WORLDC set WC
 *                  - if CS == NORMC  set NDC
 */

function PG_set_viewspace(dev, nd, cs, box)
   {var bx;

    bx = new Array();

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

function PG_init_viewspace(dev, setw)
   {var ndc;

    ndc = new Array();

    ndc = PG_box_copy(2, gs_view_x);

    PG_set_viewspace(dev, 2, NORMC, ndc);

    if (setw == TRUE)
       PG_set_viewspace(dev, 2, WORLDC, NULL);
 
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CLIP_LINE_SEG - clip the line L1 to L2 */

function PG_clip_line_seg(dev, l1, l2)
   {

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                            DEVICE ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* PG_OPEN - open the JS device */

function PG_open(name, type)
   {var screen, dev;

    screen = document.getElementById(name);
    dev    = screen.getContext(type);

    return(dev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_QUERY_SCREEN - query screen of DEV */

function PG_query_screen(dev)
   {var dx, dy, nc;
    var res;

    res = new Array();

    dx = dev.width;
    dy = dev.height;
    nc = 256;
        
    gs_phys_width    = dx;
    gs_phys_height   = dy;
    gs_phys_n_colors = nc;

    res[0] = dx;
    res[1] = dy;
    res[2] = nc;

    return(res);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_OPEN_SCREEN - open_screen the JS device */

function PG_open_screen(dev, xf, yf, dxf, dyf)
   {var xdsp, xscr;

    xdsp = new Array();
    xscr = new Array();

    xdsp = PG_query_screen(dev);
    xdsp[0] = xf;
    xdsp[1] = yf;

    xscr[0] = xdsp[0]*dxf;
    xscr[1] = xdsp[1]*dyf;

    gs_cpc[0] = INT_MIN + xdsp[0];
    gs_cpc[1] = INT_MAX - xdsp[0];
    gs_cpc[2] = INT_MIN + xdsp[1];
    gs_cpc[3] = INT_MAX - xdsp[1];
    gs_cpc[4] = INT_MIN;
    gs_cpc[5] = INT_MAX;
 
    gs_hwin[0] = 0.0;
    gs_hwin[1] = gs_hwin[0] + xscr[0];
    gs_hwin[2] = 0.0;
    gs_hwin[3] = gs_hwin[2] + xscr[1];

    SET_PC_FROM_HWIN();

    gs_window_x[0] = gs_hwin[0];
    gs_window_x[2] = gs_hwin[2];

    return(dev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_OPEN_DEVICE - open the graphics device DEV at (XF,YF) on
 *                - the physical device
 *                - the device shape is DXF by DYF
 *                - all quantities are fractions of the physical device
 *                - size
 */
 
function PG_open_device(dev, xf, yf, dxf, dyf)
   {

    PG_open_screen(dev, xf, yf, dxf, dyf);

    return(dev);}
 
/*--------------------------------------------------------------------------*/

/*                        LINE DRAWING ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* PG_FGET_LINE_COLOR - return the line color */

function PG_fget_line_color(dev)
   {

    return(gs_line_color);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_LINE_COLOR - set the line color to CLR */

function PG_fset_line_color(dev, clr, mapped)
   {

    gs_line_color = clr;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_LINE_WIDTH - return the line width */

function PG_fget_line_width(dev)
   {

    return(gs_line_width);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_LINE_WIDTH - set the line width to W */

function PG_fset_line_width(dev, w)
   {

    gs_line_width = w;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_LINE_STYLE - return the line style */

function PG_fget_line_style(dev)
   {var sty;

    sty = 0;

    return(sty);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_LINE_STYLE - set the line style to STY */

function PG_fset_line_style(dev, sty)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_MOVE_GR_ABS - move the current graphics cursor position to the
 *                 - given absolute coordinates in WC
 */
 
function _PG_move_gr_abs(dev, p)
   {

    p = PG_log_point(dev, 2, p);
 
    gs_gcur[0] = p[0];
    gs_gcur[1] = p[1];

    p[0] = gs_gcur[0];
    p[1] = gs_gcur[1];

    return(p);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MOVE_GR_ABS_N - move the graphics point to P */

function PG_move_gr_abs_n(dev, p)
   {

    p = _PG_move_gr_abs(dev, p);

    p = PG_trans_point(dev, 2, WORLDC, p, PIXELC);
 
    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MOVE_GR_REL_N - move the graphics point to P */

function PG_move_gr_rel_n(dev, p)
   {

    p = PG_log_point(dev, 2, p);

    gs_gcur += p;
    p = gs_gcur;

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_TO_ABS_N - draw from the graphics point to P */

function PG_draw_to_abs_n(dev, p)
   {var o;

    o = copy_array(gs_gcur);
    p = _PG_move_gr_abs(dev, p);

    o = PG_trans_point(dev, 2, WORLDC, o, PIXELC);
    p = PG_trans_point(dev, 2, WORLDC, p, PIXELC);
 
    PG_QUAD_FOUR_POINT(dev, o);
    PG_QUAD_FOUR_POINT(dev, p);

    dev.lineWidth   = gs_line_width;
    dev.strokeStyle = gs_line_color;

    dev.beginPath();
    dev.moveTo(o[0], o[1]);
    dev.lineTo(p[0], p[1]);
    dev.stroke();

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_TO_REL_N - draw from the graphics point to P */

function PG_draw_to_rel_n(dev, p)
   {

    o = gs_gcur;
    p = _PG_move_gr_rel(dev, p);

    o = PG_trans_point(dev, 2, WORLDC, o, PIXELC);
    p = PG_trans_point(dev, 2, WORLDC, p, PIXELC);
 
    PG_QUAD_FOUR_POINT(dev, o);
    PG_QUAD_FOUR_POINT(dev, p);

    dev.lineWidth   = gs_line_width;
    dev.strokeStyle = gs_line_color;

    dev.beginPath();
    dev.moveTo(o[0], o[1]);
    dev.lineTo(p[0], p[1]);
    dev.stroke();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_LINE_N - draw a line on DEV from X1 to X2 */

function PG_draw_line_n(dev, nd, cs, x1, x2, clip)
   {var id;
    var xi, p, l1, l2;

    dev.beginPath();

    if (dev != NULL)
       {xi = new Array();
	l1 = new Array();
	l2 = new Array();

	for (id = 0; id < nd; id++)
            xi[id] = new Array(x1[id], x2[id]);

/* draw */
	if (nd == 2)
	   {p = PG_trans_points(dev, 2, nd, cs, xi, WORLDC);

/* make a copy so as to avoid clobbering X1 and X2 */
	    for (id = 0; id < nd; id++)
	        {l1[id] = p[id][0];
		 l2[id] = p[id][1];};

	    if (PG_clip_line_seg(dev, l1, l2))
	       {PG_move_gr_abs_n(dev, l1);
		PG_draw_to_abs_n(dev, l2);};}

//	else
//	   _PG_rst_draw_disjoint_polyline_3(dev, cs, 1, p, clip, FALSE);

       };

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_DRAW_BOX_N - draw the BOX in the specified coordinates */

function PG_draw_box_n(dev, nd, cs, box)
   {var x, y, bx;

    x  = new Array();
    y  = new Array();
    bx = PG_trans_box(dev, nd, cs, box, WORLDC);

    switch (nd)
       {default :
	case 2  :
	     x[0] = bx[0];
	     y[0] = bx[2];
	     x[1] = bx[0];
	     y[1] = bx[3];
	     x[2] = bx[1];
	     y[2] = bx[3];
	     x[3] = bx[1];
	     y[3] = bx[2];

	     PG_draw_line_n(dev, 2, WORLDC, [x[0],y[0]], [x[1],y[1]], TRUE);
	     PG_draw_line_n(dev, 2, WORLDC, [x[1],y[1]], [x[2],y[2]], TRUE);
	     PG_draw_line_n(dev, 2, WORLDC, [x[2],y[2]], [x[3],y[3]], TRUE);
	     PG_draw_line_n(dev, 2, WORLDC, [x[3],y[3]], [x[0],y[0]], TRUE);
	     break;};

    return;}
 
/*--------------------------------------------------------------------------*/

/*                        TEXT DRAWING ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* PG_FGET_TEXT_COLOR - return the text color */

function PG_fget_text_color(dev)
   {

    return(gs_text_color);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_TEXT_COLOR - set the text color to CLR */

function PG_fset_text_color(dev, clr, mapped)
   {

    gs_text_color = clr;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_FONT - set the text FACE, STYLE, and SIZE */

function PG_set_font(dev, face, style, size)
   {var sz;

    sz = size.toString();

    dev.font=face.concat(sz, "px", face);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MOVE_TX_ABS_N - move the text point to P */

function PG_move_tx_abs_n(dev, p)
   {

    gs_tcur = p;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MOVE_TX_REL_N - move the text point to P */

function PG_move_tx_rel_n(dev, p)
   {

    gs_tcur = gs_tcur + p;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_WRITE_TEXT - draw test S on DEV */

function PG_write_text(dev, fp, s)
   {var x;

    if (fp == gs_stdscr)
       {x = PG_trans_point(dev, 2, WORLDC, gs_tcur, PIXELC);
	x = PG_QUAD_FOUR_POINT(dev, x);

	dev.fillStyle   = gs_text_color;
	dev.strokeStyle = gs_text_color;
	dev.fillText(s, x[0], x[1]);}

    else
       document.write(s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_WRITE_N - draw test S on DEV at X */

function PG_write_n(dev, nd, cs, x, s)
   {

    y = PG_trans_point(dev, nd, cs, x, WORLDC);
    y = PG_move_tx_abs_n(dev, y);
    PG_write_text(dev, gs_stdscr, s);

    return;}

/*--------------------------------------------------------------------------*/

/*                              FILL ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* PG_FGET_FILL_COLOR - return the fill color */

function PG_fget_fill_color(dev)
   {

    return(gs_fill_color);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_FILL_COLOR - set the fill color to CLR */

function PG_fset_fill_color(dev, clr, mapped)
   {

    gs_text_color = clr;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SHADE_POLY_N - fill the ND polygon defined by the N points R */

function PG_shade_poly_n(dev, nd, n, r)
   {

    return;}

/*--------------------------------------------------------------------------*/

/*                             IMAGE ROUTINES                               */

/*--------------------------------------------------------------------------*/

function PG_put_image_n(dev, bf, cs, irg)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

