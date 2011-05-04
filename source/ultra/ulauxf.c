/*
 * ULAUXF.C - auxillary functions used by ultra
 *
 * Source Version: 4.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "ultra.h"

#define MAX_ARY(a, ap, n, amax)                                              \
    for (ap = a, amax = *a; ap-a < n; amax = max(amax, *a++))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_FFT - work horse for complex FFT's in ULTRA */

static object *_UL_fft(SS_psides *si, object *argl,
		       char *type, int no, int flag, int ordr)
   {int n, i, jr, ji, n0;
    double xmn, xmx;
    double *y, *x, *ypr, *ypi;
    complex *cy, *icy;
    char *lbl;
    curve *crvr, *crvi;
    object *ch1, *ch2, *rv;

    SX_prep_arg(si, argl);

    jr = -1;
    ji = -1;
    SS_args(si, argl,
            UL_CURVE_INDEX_I, &jr,
            UL_CURVE_INDEX_I, &ji,
            0);

    if ((jr < 0) || (ji < 0))
       SS_error(si, "BAD CURVES - _UL_FFT", argl);

    crvr = &SX_dataset[jr];
    crvi = &SX_dataset[ji];

    xmn = crvr->wc[0];
    xmx = crvr->wc[1];
    ypr = crvr->x[1];
    ypi = crvi->x[1];

    n0 = max(crvr->n, crvi->n);
    n  = PM_near_power(n0, 2);

/* make an array of complex values out of the real and imaginary values */
    icy = CMAKE_N(complex, n0);
    for (i = 0; i < n0; i++)
        icy[i] = PM_COMPLEX(ypr[i], ypi[i]);

/* GOTCHA: this assumes a common set of domain values for the curves */
    n = PM_fft_sc_complex_data(&cy, &x, crvr->x[0], icy, n0,
                               xmn, xmx, flag, ordr);

    no += n;

    y = CMAKE_N(double, no);

/* extract the real part */
    for (i = 0; i < no; i++)
        y[i] = PM_REAL_C(cy[i]);
    lbl = SC_dsnprintf(FALSE, "Real part %s %c %c", type, crvr->id, crvi->id);
    ch1 = SX_mk_curve(si, no, x, y, lbl, NULL, UL_plot);

/* extract the imaginary part */
    for (i = 0; i < no; i++)
        y[i] = PM_IMAGINARY_C(cy[i]);
    lbl = SC_dsnprintf(FALSE, "Imaginary part %s %c %c", type, crvr->id, crvi->id);
    ch2 = SX_mk_curve(si, no, x, y, lbl, NULL, UL_plot);

    CFREE(x);
    CFREE(y);
    CFREE(icy);
    CFREE(cy);

    rv = SS_make_list(si, SS_OBJECT_I, ch1,
		      SS_OBJECT_I, ch2,
		      0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_FFT - the main controlling routine for the FFT command */

object *UL_fft(SS_psides *si, int j)
   {int i, n;
    double xmn, xmx;
    double *y, *x;
    complex *cy;
    char *lbl;
    curve *crv;
    object *cre, *cim, *rv;

    crv = &SX_dataset[j];
    xmn = crv->wc[0];
    xmx = crv->wc[1];

    n = PM_fft_sc_real_data(&cy, &x, crv->x[0], crv->x[1], crv->n,
                            xmn, xmx, _SX.fft_order);
    if (n == 0)
       SS_error(si, "FFT FAILED - UL_FFT", SS_null);

    y = CMAKE_N(double, n);
    if (y == NULL)
       SS_error(si, "INSUFFICIENT MEMORY - UL_FFT", SS_null);

/* extract the real part */
    for (i = 0; i < n; i++)
        y[i] = PM_REAL_C(cy[i]);
    lbl = SC_dsnprintf(FALSE, "Real part FFT %c", crv->id);
    cre = SX_mk_curve(si, n, x, y, lbl, NULL, UL_plot);

/* extract the imaginary part */
    for (i = 0; i < n; i++)
        y[i] = PM_IMAGINARY_C(cy[i]);
    lbl = SC_dsnprintf(FALSE, "Imaginary part FFT %c", crv->id);
    cim = SX_mk_curve(si, n, x, y, lbl, NULL, UL_plot);

    CFREE(x);
    CFREE(y);
    CFREE(cy);

    rv = SS_make_list(si, SS_OBJECT_I, cre,
		      SS_OBJECT_I, cim,
		      0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_IFFT - main controlling routine for IFFT */

static object *_ULI_ifft(SS_psides *si, object *argl)
   {object *rv;

    rv = _UL_fft(si, argl, "IFFT", 0, -1, _SX.fft_order);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_CFFT - main controlling routine for complex FFT */

static object *_ULI_cfft(SS_psides *si, object *argl)
   {object *rv;

    rv = _UL_fft(si, argl, "CFFT", 0, 1, _SX.fft_order);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_EFFECTIVE_DX - estimate a sampling rate for an FFT
 *                  - dx(eff) is the summed part of the domain
 *                  - where f > 0.5*fmax
 *                  - (for a gaussian this would be the half width)
 *                  - return dx(eff)/16
 *                  - the 16 allows for the Nyquist factor of 2
 *                  - and 8 effective widths
 */

static double _UL_effective_dx(double *x, double *y, int n)
   {int i;
    double yx, dxe, v, rv;

    yx = 0.0;
    for (i = 0; i < n; i++)
        {v  = ABS(y[i]);
	 yx = max(yx, v);};
    yx *= 0.5;

    dxe = 0.0;
    for (i = 1; i < n; i++)
        {v = y[i];
         if (v > yx)
            dxe += (x[i] - x[i-1]);};

    rv = 0.0625*dxe;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_CONVLV - compute and return the convolution of two curves */

static object *_ULI_convlv(SS_psides *si, object *argl)
   {int jg, jh, n, gn, hn;
    double *y, *x, *gx, *gy, *hx, *hy, dt, dxeg, dxeh;
    char *lbl;
    curve *crvg, *crvh;
    object *ch;

    SX_prep_arg(si, argl);

    jg = -1;
    jh = -1;
    dt = HUGE;
    SS_args(si, argl,
            UL_CURVE_INDEX_I, &jg,
            UL_CURVE_INDEX_I, &jh,
            SC_DOUBLE_I, &dt,
            0);

    if ((jg < 0) || (jh < 0))
       SS_error(si, "BAD CURVES - _ULI_CONVLV", argl);

    crvg = &SX_dataset[jg];
    crvh = &SX_dataset[jh];

    gx = crvg->x[0];
    gy = crvg->x[1];
    gn = crvg->n;
    hx = crvh->x[0];
    hy = crvh->x[1];
    hn = crvh->n;

/* estimate dt */
    dxeg = _UL_effective_dx(gx, gy, gn);
    dxeh = _UL_effective_dx(hx, hy, hn);
    if (dt == HUGE)
       dt = min(dxeg, dxeh);

    PM_convolve(gx, gy, gn, hx, hy, hn, dt, &x, &y, &n);

    lbl = SC_dsnprintf(FALSE, "Convolution %c %c", crvg->id, crvh->id);
    ch = SX_mk_curve(si, n, x, y, lbl, NULL, UL_plot);

    return(ch);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_GET_VALUE - get a list of values from the given curve and return it */

static object *_UL_get_value(SS_psides *si, double *sp, double *vp,
			     double val, int n, int id)
   {int i;
    char flag;
    double test, y;
    double *z, *z1;
    object *ret;

    flag = 'f';

    if (si->interactive == ON)
       PRINT(stdout, "\nCurve %c\n", id);

    ret = SS_null;
    for (i = 0, z = sp, z1 = sp+1; i < n-1; i++, z++, z1++)
        {flag = 'f';
         test = (*z-val)*(*z1-val);
         if (test < 0)
            {PM_interp(y, val, *z, vp[i], *z1, vp[i+1]);
             flag = 't';}
         else if (PM_CLOSETO_REL(test, 0.0))
            {if ((*z == val) || ((i == 0) && PM_CLOSETO_REL(*z - val, 0.0)))
                {y = vp[i];
                 flag = 't';}
             else if (i == (n-2) && PM_CLOSETO_REL(*z1 - val, 0.0))
                {y = vp[i+1];
                 flag = 't';};};
         if (flag == 't')
            {if (si->interactive == ON)
                {PRINT(stdout, "    ");
                 PRINT(stdout, SX_text_output_format, val);
                 PRINT(stdout, "    ");
                 PRINT(stdout, SX_text_output_format, y);
                 PRINT(stdout, "\n");};
                 
             SS_assign(si, ret, SS_mk_cons(si, SS_mk_float(si, y), ret));};};

    if (!SS_nullobjp(ret))
       {SS_assign(si, ret, SS_reverse(si, ret));
        SC_mark(ret, -1);}
    else
       ret = SS_f;

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_CURVE_EVAL - find the apropriate y value for the given x value */

object *UL_curve_eval(SS_psides *si, object *arg)
   {int i;
    char *s;
    double value;
    object *ret;

    s = SS_GET(procedure, si->fun)->name;
    i = SX_get_data_index(s);
    if (i < 0)
       SS_error(si, "CURVE DELETED, NO PROCEDURE - CURVE-EVAL", si->fun);

    if (SS_integerp(arg))
       value = (double) *SS_GET(int64_t, arg);
    else
       value = (double) *SS_GET(double, arg);

    ret = _UL_get_value(si, SX_dataset[i].x[0], SX_dataset[i].x[1], value,
			SX_dataset[i].n, SX_dataset[i].id);
    SS_mark(ret);

    UL_pause(si, FALSE);

    if (SS_true(ret))
       {SX_prep_ret(si, ret);}
    else
       SS_error(si, "ARGUMENT OUT OF DOMAIN - CURVE-EVAL", arg);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_GETX - return the x values associated with the given y value */

object *UL_getx(SS_psides *si, object *obj, object *tok)
   {int i;
    double value;
    object *tmp, *o;

    i = SX_get_crv_index_i(obj);
    if (SS_integerp(tok))
       value = (double) *SS_GET(int64_t, tok);
    else
       value = (double) *SS_GET(double, tok);

    tmp = _UL_get_value(si, SX_dataset[i].x[1], SX_dataset[i].x[0], value,
			SX_dataset[i].n, SX_dataset[i].id);

    o = SS_make_list(si, SS_OBJECT_I, obj,
		     SS_OBJECT_I, tok,
		     SS_OBJECT_I, tmp,
		     0);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_GETY - return the y values associated with the given x value */

object *UL_gety(SS_psides *si, object *obj, object *tok)
   {int i;
    double value;
    object *tmp, *o;

    i = SX_get_crv_index_i(obj);
    if (SS_integerp(tok))
       value = (double) *SS_GET(int64_t, tok);
    else
       value = (double) *SS_GET(double, tok);

    tmp = _UL_get_value(si, SX_dataset[i].x[0], SX_dataset[i].x[1], value,
			SX_dataset[i].n, SX_dataset[i].id);

    o = SS_make_list(si, SS_OBJECT_I, obj,
		     SS_OBJECT_I, tok,
		     SS_OBJECT_I, tmp,
		     0);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_LSQ - find least squares fit */

PM_matrix *UL_lsq(PM_matrix *a, PM_matrix *ay)
   {PM_matrix *at, *b, *by;

    at = PM_transpose(a);
    b  = PM_times(at, a);
    by = PM_times(at, ay);
        
/* solution is returned in by */
    PM_solve(b, by);

/* clean up the mess */
    PM_destroy(at);
    PM_destroy(b);

    return(by);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_FIT - curve fitting routine */

static object *_ULI_fit(SS_psides *si, object *obj, object *tok)
   {int i, j, n, sgn, order, aord;
    double wc[PG_BOXSZ];
    double *x[PG_SPACEDM];
    double *cf, **p;
    char *lbl;
    object *ret;

    order = 0;

    if (SS_integerp(tok))
       order = SS_INTEGER_VALUE(tok);

    else
       SS_error(si, "FIT POWER MUST BE INTEGER - _ULI_FIT", tok);

    aord = abs(order) + 1;
    j    = SX_get_crv_index_i(obj);

    n     = SX_dataset[j].n;
    x[0]  = SX_dataset[j].x[0];
    x[1]  = SX_dataset[j].x[1];
    wc[0] = SX_dataset[j].wc[0];
    wc[1] = SX_dataset[j].wc[1];

    cf = PM_lsq_fit(2, n, x, wc, order);

/* display coefficients and cons up the return list */
    if (si->interactive == ON)
       {if ((SX_dataset[j].id >= 'A') &&
            (SX_dataset[j].id <= 'Z'))
           {PRINT(stdout, "\nCurve %c\n", SX_dataset[j].id);}
        else
           {PRINT(stdout, "\nCurve @%d\n", SX_dataset[j].id);};}

    ret = SS_null;
    sgn = (order < 0) ? -1 : 1;
    for (i = 0 ; i < aord; i++)
        {if (si->interactive == ON)
	    {PRINT(stdout, "    ");
	     PRINT(stdout, SX_text_output_format, cf[i]);
	     PRINT(stdout, " *x^%d\n", sgn*i);};
         ret = SS_mk_cons(si, SS_mk_float(si, cf[i]), ret);};
        
/* create curve of fit */
    p = PM_lsq_polynomial(SX_default_npts, order, cf, wc);

    if ((SX_dataset[i].id >= 'A') &&
        (SX_dataset[i].id <= 'Z'))
        {lbl = SC_dsnprintf(FALSE, "Fit %c %d", SX_dataset[j].id, order);}
    else
        {lbl = SC_dsnprintf(FALSE, "Fit @%d %d", SX_dataset[j].id, order);}

    ret = SS_mk_cons(si, SS_reverse(si, ret),
                     SX_mk_curve(si, SX_default_npts, p[0], p[1],
				 lbl, NULL, UL_plot));

    CFREE(cf);

    PM_free_vectors(2, p);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_FIT_CURVE - generate least squares fits to the given curves */

static object *_ULI_fit_curve(SS_psides *si, object *argl)
   {int j, i, k, n, order, aord, id;
    int alpha_id;
    int *curid;
    double yv;
    double *x[PG_SPACEDM], *xp0, *yp0;
    char local[MAXLINE];
    PM_matrix *ay, *a, *solution;
    object *ch;

    SS_args(si, argl,
            UL_CURVE_INDEX_I, &j,
            0);

    if (j < 0)
       SS_error(si, "BAD CURVE - _ULI_FIT_CURVE", argl);
    n = SX_dataset[j].n;

    argl  = SS_cdr(si, argl);
    order = SS_length(si, argl);
    aord  = abs(order) + 1;
    curid = CMAKE_N(int, order);
    for (i = 0; i < order; i++, argl = SS_cdr(si, argl))
        curid[i] = SX_get_crv_index_i(SS_car(si, argl));
        
    ay = PM_create(n, 1);
    a  = PM_create(n, order);

    xp0 = SX_dataset[j].x[0] - 1;
    yp0 = SX_dataset[j].x[1] - 1;
    for (i = 1; i <= n; i++)
        PM_element(ay, i, 1) = yp0[i];

    for (k = 1; k < aord; k++)
        {x[0] = SX_dataset[curid[k-1]].x[0];
         x[1] = SX_dataset[curid[k-1]].x[1];
         for (i = 1; i <= n; i++)
             {while (xp0[i] > x[0][1])
                 {x[0]++;
                  x[1]++;};
              PM_interp(yv, xp0[i], *x[0], *x[1], x[0][1], x[1][1]);
              PM_element(a, i, k) = yv;};};

    if (si->interactive == ON)
       PRINT(stdout, "\n    Fit curves\n\n");

    solution = UL_lsq(a, ay);

/* display coefficients */
    id = SX_dataset[j].id;
    alpha_id = ((SX_dataset[i].id >= 'A') &&
                (SX_dataset[i].id <= 'Z'));

    if (si->interactive == ON)
       {if (alpha_id)
           {PRINT(stdout, "Fit to curve %c\n\n", id);}
        else
           {PRINT(stdout, "Fit to curve @%d\n\n", id);};}

    if (alpha_id)
       {snprintf(local, MAXLINE, "Fit(%c ;", id);}
    else
       {snprintf(local, MAXLINE, "Fit(%c ;", id);}

    for (i = 1; i <= order; i++)
        {id = SX_dataset[curid[i-1]].id;
         alpha_id = ((SX_dataset[i].id >= 'A') &&
                     (SX_dataset[i].id <= 'Z'));
	 if (si->interactive == ON)
	    {PRINT(stdout, "    ");
	     PRINT(stdout, SX_text_output_format, PM_element(solution, i, 1));
             if (alpha_id)
	        {PRINT(stdout, " * curve %c\n", id);}
             else
	        {PRINT(stdout, " * curve @%d\n", id);};};

         if (alpha_id)
            SC_vstrcat(local, MAXLINE, " %c", id);
         else
            SC_vstrcat(local, MAXLINE, " @%d", id);};

    SC_strcat(local, MAXLINE, ")");

/* find fit */
    PM_destroy(ay);
    ay = PM_times(a, solution);

    UL_buf1y = CMAKE_N(double, n);
    for (i = 1; i <= n; i++)
        UL_buf1y[i-1] = PM_element(ay, i, 1);

    if (si->interactive == ON)
       PRINT(stdout, "\n");

    ch = SX_mk_curve(si, SX_dataset[j].n, SX_dataset[j].x[0], UL_buf1y,
		     local, NULL, UL_plot);

/* clean up */
    PM_destroy(a);
    PM_destroy(ay);
    PM_destroy(solution);
    CFREE(UL_buf1y);
    CFREE(curid);
        
    return(ch);}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_ERROR_PLOT - plot error bars on the specified curve */

static object *_ULI_error_plot(SS_psides *si, object *argl)
   {pcons *info;
    object *c, *xp[PG_SPACEDM], *xm[PG_SPACEDM], *o;
    curve *crv;
    void *xpc[PG_SPACEDM], *xmc[PG_SPACEDM];

    xp[1] = NULL;
    xm[1] = NULL;
    xp[0] = NULL;
    xm[0] = NULL;
    SS_args(si, argl,
            SS_OBJECT_I, &c,
            SS_OBJECT_I, &xp[1],
            SS_OBJECT_I, &xm[1],
            SS_OBJECT_I, &xp[0],
            SS_OBJECT_I, &xm[0],
	    0);

    xpc[0] = ((xp[0] == NULL) || SS_nullobjp(xp[0])) ?
             NULL : (void *) (SX_dataset + SX_get_crv_index_i(xp[0]))->x[1];

    xpc[1] = ((xp[1] == NULL) || SS_nullobjp(xp[1])) ?
             NULL : (void *) (SX_dataset + SX_get_crv_index_i(xp[1]))->x[1];

    xmc[0] = ((xm[0] == NULL) || SS_nullobjp(xm[0])) ?
             NULL : (void *) (SX_dataset + SX_get_crv_index_i(xm[0]))->x[1];

    xmc[1] = ((xm[1] == NULL) || SS_nullobjp(xm[1])) ?
             NULL : (void *) (SX_dataset + SX_get_crv_index_i(xm[1]))->x[1];

    crv  = SX_dataset + SX_get_crv_index_i(c);
    info = crv->info;

    info = PG_set_plot_type(info, PLOT_ERROR_BAR, CARTESIAN_2D);

    info = PG_set_attrs_alist(info,
			      "DX-PLUS",  SC_DOUBLE_I, TRUE, xpc[0],
			      "DX-MINUS", SC_DOUBLE_I, TRUE, xmc[0],
			      "DY-PLUS",  SC_DOUBLE_I, TRUE, xpc[1],
			      "DY-MINUS", SC_DOUBLE_I, TRUE, xmc[1],
			      NULL);
    crv->info = info;

    o = (object *) crv->obj;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_MK_PALETTE - make a new palette */

static object *_ULI_mk_palette(SS_psides *si, object *argl)
   {int nc, wbck;
    char *name, *fname;
    PG_device *dev;
    PG_palette *pal;

    name = NULL;
    nc   = 8;
    wbck = TRUE;
    SS_args(si, argl,
            SC_STRING_I, &name,
            SC_INT_I, &nc,
            SC_INT_I, &wbck,
            0);

    dev = SX_graphics_device;
    pal = PG_make_palette(dev, name, nc, wbck);

/* write the palette for future reference */
    if (pal != NULL)
       {dev->current_palette = pal;
	fname = SC_dsnprintf(FALSE, "%s.pal", name);
        if (SX_current_palette != NULL)
               CFREE(SX_current_palette);
        SX_current_palette = CSTRSAVE(fname);
	PG_wr_palette(dev, pal, fname);};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_RD_PALETTE - read a new palette */

static object *_ULI_rd_palette(SS_psides *si, object *argl)
   {char *name;
    PG_device *dev;
    PG_palette *pal;

    name = NULL;
    SS_args(si, argl,
            SC_STRING_I, &name,
            0);

    dev = SX_graphics_device;
    if (dev != NULL)

/* write the palette for future reference */
       {pal = PG_rd_palette(dev, name);
	if (pal != NULL)
	   {dev->current_palette = pal;
            if (SX_current_palette != NULL)
               CFREE(SX_current_palette);
            SX_current_palette = 
               CSTRSAVE(name);};};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_MARK_CURVE_POINTS - graphically mark curve points for some later
 *                      - operation
 */

static void UL_mark_curve_points(double **x, int n, char *indx)
   {int i, ok, btn, mod, color, off;
    int ir[PG_SPACEDM];
    double xm[PG_SPACEDM], im[PG_SPACEDM], xc[PG_SPACEDM];
    double *r[PG_SPACEDM];
    PG_device *dev;
    PM_polygon *py;

    dev = SX_graphics_device;
    if (dev == NULL)
       return;

    py = PM_make_polygon(2, n, x[0], x[1]);

    i  = 0;
    xc[0] = x[0][0];
    xc[1] = x[1][0];

    ok = TRUE;
    while (ok == TRUE)
       {PG_query_pointer(dev, ir, &btn, &mod);
	PG_print_pointer_location(dev,
				  SX_show_mouse_location_x,
				  SX_show_mouse_location_y,
				  TRUE);
        if (btn)
	   {im[0] = ir[0];
	    im[1] = ir[1];
	    PG_trans_point(dev, 2, PIXELC, im, WORLDC, xm);

	    PM_nearest_point(py, xm, xc, &i);

	    off = (mod & KEY_SHIFT) ? FALSE : TRUE;
	    switch (btn)
	       {case MOUSE_LEFT :
		     indx[i] = off;
                     color   = off ? dev->RED : dev->GREEN;

/* toggle the color of the point between green and red */
                     PG_set_line_color(dev, color);
		     r[0] = xc;
		     r[1] = xc + 1;
                     PG_draw_markers_n(dev, 2, WORLDC, 1, r, 0);
		     break;

		case MOUSE_MIDDLE :
                     break;

		case MOUSE_RIGHT :
		     ok = FALSE;
		     break;};};};

    PM_free_polygon(py);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_EDIT - graphically edit a curve */

static object *_ULI_edit(SS_psides *si, int ie)
   {int n, i, k, oldc;
    double *x[PG_SPACEDM];
    char *indx;
    PG_device *dev;
    curve *crv, *ocv;
    object *o;

    dev = SX_graphics_device;

    ocv  = SX_dataset + ie;

    crv  = SX_dataset + SX_get_crv_index_i(UL_copy_curve(si, ie));
    n    = crv->n;
    x[0]   = crv->x[0];
    x[1]   = crv->x[1];

    indx = CMAKE_N(char, n);

/* remember the original curve's color */
    PG_get_attrs_alist(ocv->info,
		       "LINE-COLOR", SC_INT_I, &oldc, dev->WHITE,
		       NULL);

/* redraw original curve in gray */
    ocv->info = PG_set_attrs_alist(ocv->info,
				   "LINE-COLOR", SC_INT_I, FALSE, dev->GRAY,
				   NULL);

/* draw scatter version of curve in green */
    crv->info = PG_set_attrs_alist(crv->info,
				   "SCATTER",    SC_INT_I, FALSE, TRUE,
				   "LINE-COLOR", SC_INT_I, FALSE, dev->GREEN,
				   NULL);
    UL_plot(si);

/* mark the points to remove */
    UL_mark_curve_points(x, n, indx);

/* remove the marked points */
    for (i = 0; i < n; i++)
        {if (indx[i])
            {n--;
             for (k = i--; k < n; k++)
	         {x[0][k]   = x[0][k+1];
		  x[1][k]   = x[1][k+1];
                  indx[k] = indx[k+1];};};};

    CFREE(indx);

/* resize the xp and yp arrays */
    CREMAKE(crv->x[0], double, n);
    CREMAKE(crv->x[1], double, n);

    crv->n = n;

/* GOTCHA: a compiler rightly complained about scn,
 *         cln, and clo not being initialized
 *         are these necessary any more?
 */
#if 0
    int *scn, *cln, *clo;

/* change rendering of new curve */
    *scn  = FALSE;
    *cln  = SX_next_color(dev); 

/* restore original curve color */
    *clo = oldc;
#endif

    o = (object *) crv->obj;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_DUPX - make a new curve consisting of x vs. x for the given curve */

object *UL_dupx(SS_psides *si, int j)
   {char *lbl;
   object *o;

    if ((SX_dataset[j].id >= 'A') && (SX_dataset[j].id <= 'Z'))
       lbl = SC_dsnprintf(FALSE, "Dupx %c", SX_dataset[j].id);
    else
       lbl = SC_dsnprintf(FALSE, "Dupx @%d", SX_dataset[j].id);

    o = SX_mk_curve(si, SX_dataset[j].n, SX_dataset[j].x[0],
		    SX_dataset[j].x[0], lbl, NULL, UL_plot);
        
    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_STAT - return a list of relevant statistics for the given curve */

static object *_UL_stat(SS_psides *si, int j)
   {int n;
    double xmean, xstd, ymean, ystd;
    object *ret;

    if (si->interactive == ON)
       {if ((SX_dataset[j].id >= 'A') &&
            (SX_dataset[j].id <= 'Z'))
           {PRINT(stdout, "\nCurve %c\n", SX_dataset[j].id);}
        else
           {PRINT(stdout, "\nCurve @%d\n", SX_dataset[j].id);};}

    n = SX_dataset[j].n;
    PM_stats_mean(n, SX_dataset[j].x[0], &xmean, NULL, NULL, &xstd);
    PM_stats_mean(n, SX_dataset[j].x[1], &ymean, NULL, NULL, &ystd);

    ret = SS_make_list(si, SC_DOUBLE_I, &xmean,
                       SC_DOUBLE_I, &xstd,
                       SC_DOUBLE_I, &ymean,
                       SC_DOUBLE_I, &ystd,
                       0);

    if (si->interactive == ON)
       {PRINT(stdout, "\nX Mean =               ");
	PRINT(stdout, SX_text_output_format, xmean);
	PRINT(stdout, "\nX Standard deviation = ");
	PRINT(stdout, SX_text_output_format, xstd);
	PRINT(stdout, "\nY Mean =               ");
	PRINT(stdout, SX_text_output_format, ymean);
	PRINT(stdout, "\nY Standard deviation = ");
	PRINT(stdout, SX_text_output_format, ystd);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_STATS - control the computation of means and standard deviations for
 *            - a specified set of curves
 */

static object *_ULI_stats(SS_psides *si, object *argl)
   {int i;
    object *ret, *o;

    SX_prep_arg(si, argl);
    ret = SS_null;
    for ( ; SS_consp(argl); argl = SS_cdr(si, argl))
       {i = SX_get_crv_index_i(SS_car(si, argl));
        if (i != -1)
           ret = SS_mk_cons(si, _UL_stat(si, i), ret);};
         
    UL_pause(si, FALSE);
   
    if (SS_length(si, ret) == 1)
       o = SS_car(si, ret);
    else
       o = ret;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_DISP - display the curve points in the given domain */

static object *_ULI_disp(SS_psides *si, int j, double xmin, double xmax)
   {int n, i;
    double *x[PG_SPACEDM];
    object *o;

    x[0]   = SX_dataset[j].x[0];
    x[1]   = SX_dataset[j].x[1];
    n    = SX_dataset[j].n;

    if ((SX_dataset[j].id >= 'A') &&
        (SX_dataset[j].id <= 'Z'))
       {PRINT(stdout, "\n Curve %c (%s) from ", SX_dataset[j].id, SX_dataset[j].text);}
    else
       {PRINT(stdout, "\n Curve @%d (%s) from ", SX_dataset[j].id, SX_dataset[j].text);}

    PRINT(stdout, SX_text_output_format, xmin);
    PRINT(stdout, " to ");
    PRINT(stdout, SX_text_output_format, xmax);
    PRINT(stdout, "\n\n");

    for (i = 0; i < n; i++)
        {if ((x[0][i] >= xmin) && (x[0][i] <= xmax))
            {PRINT(stdout, "    ");
             PRINT(stdout, SX_text_output_format, x[0][i]);
             PRINT(stdout, " ");
             PRINT(stdout, SX_text_output_format, x[1][i]);
             PRINT(stdout, "\n");};};

    PRINT(stdout, "\n");
        
    o = (object *) SX_dataset[j].obj;
                
    return(o);}
                
/*--------------------------------------------------------------------------*/

/*                       SCHEME CURVE ACCESS ROUTINES                       */

/*--------------------------------------------------------------------------*/

/* _ULI_CRV_LABEL - return the label for the given curve */

static object *_ULI_crv_label(SS_psides *si, object *obj)
   {int i;
    object *o;

    if (!SX_curvep_a(obj))
       SS_error(si, "BAD CURVE - _ULI_CRV_LABEL", obj);

    o = SS_null;
    i = SX_get_crv_index_i(obj);
    if (i != -1)
       {if (si->interactive == ON)
           PRINT(stdout, "\n Label: %s\n\n", SX_dataset[i].text);
        o = SS_mk_string(si, SX_dataset[i].text);};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_CRV_DOMAIN - return a cons whose car and cdr are the min and max of
 *                 - the given curve's domain
 */

static object *_ULI_crv_domain(SS_psides *si, object *obj)
   {int j;
    object *o;

    j = -1;
    SS_args(si, obj,
            UL_CURVE_INDEX_I, &j,
            0);

    o = SS_null;
    if (j != -1)
       {if (si->interactive == ON)
           {PRINT(stdout, "\n Domain: (");
            PRINT(stdout, SX_text_output_format, SX_dataset[j].wc[0]);
            PRINT(stdout, " . ");
            PRINT(stdout, SX_text_output_format, SX_dataset[j].wc[1]);
            PRINT(stdout, ")\n\n");};

        o = SS_mk_cons(si, SS_mk_float(si, SX_dataset[j].wc[0]),
		       SS_mk_float(si, SX_dataset[j].wc[1]));};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_CRV_RANGE - return a cons whose car and cdr are the min and max of
 *                - the given curve's range
 */

static object *_ULI_crv_range(SS_psides *si, object *obj)
   {int j;
    object *o;

    j = -1;
    SS_args(si, obj,
            UL_CURVE_INDEX_I, &j,
            0);

    o = SS_null;
    if (j != -1)
       {if (si->interactive == ON)
           {PRINT(stdout, "\n Range: (");
            PRINT(stdout, SX_text_output_format, SX_dataset[j].wc[2]);
            PRINT(stdout, " . ");
            PRINT(stdout, SX_text_output_format, SX_dataset[j].wc[3]);
            PRINT(stdout, ")\n\n");};

        o = SS_mk_cons(si, SS_mk_float(si, SX_dataset[j].wc[2]),
		       SS_mk_float(si, SX_dataset[j].wc[3]));};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_CRV_NPTS - return the number of points in the given curve */

static object *_ULI_crv_npts(SS_psides *si, object *obj)
   {int j;
    object *o;

    j = -1;
    SS_args(si, obj,
            UL_CURVE_INDEX_I, &j,
            0);

    o = SS_null;

    if (j != -1)
       {if (si->interactive == ON)
           PRINT(stdout, "\n Number of points: %ld\n\n", SX_dataset[j].n);

        o = SS_mk_integer(si, SX_dataset[j].n);};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_CRV_ATTR - return a list of the given curve's attributes:
 *               -      (color width style)
 */

static object *_ULI_crv_attr(SS_psides *si, object *obj)
   {int j, lncol, lnsty, dfcol;
    double lnwid;
    pcons *info;
    object *o;

    j = -1;
    SS_args(si, obj,
            UL_CURVE_INDEX_I, &j,
            0);

    o = SS_null;

    if (j != -1)
       {info  = SX_dataset[j].info;
	dfcol = (SX_graphics_device == NULL) ? 4 : SX_graphics_device->BLUE;
	   
	PG_get_attrs_alist(info,
			   "LINE-COLOR", SC_INT_I, &lncol, dfcol,
			   "LINE-STYLE", SC_INT_I, &lnsty, LINE_SOLID,
			   "LINE-WIDTH", SC_DOUBLE_I,    &lnwid, 0.0,
			   NULL);

        if (si->interactive == ON)
           {PRINT(stdout, "\n Color, width, style: (%ld ", lncol);
            PRINT(stdout, SX_text_output_format, lnwid);
            PRINT(stdout, " %ld)\n\n", lnsty);};

        o = SS_make_list(si, SC_INT_I, &lncol,
			 SC_DOUBLE_I,  &lnwid,
			 SC_INT_I, &lnsty,
			 0);};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_WRITE_ABS - let ULTRA make print stuff on the screen
 *                - specified in NDC
 */

static object *_ULI_write_abs(SS_psides *si, object *argl)
   {double x[PG_SPACEDM];
    char *text;

    x[0] = 0.0;
    x[1] = 0.0;
    text = NULL;
    SS_args(si, argl,
            SC_DOUBLE_I, &x[0],
            SC_DOUBLE_I, &x[1],
            SC_STRING_I, &text,
            0);

    PG_write_n(SX_graphics_device, 2, NORMC, x, text);
   
    return(SS_f);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_INSTALL_AUX_FUNCS - install the functions defined in this file */

void UL_install_aux_funcs(SS_psides *si)
   {

    SS_install(si, "cfft",
               "Procedure: Compute Complex FFT. Return real and imaginary parts.\n     Usage: cfft <real-curve> <imaginary-curve>",
               SS_nargs,
               _ULI_cfft, SS_PR_PROC);

    SS_install(si, "convolve",
               "Procedure: Convolve the two functions\n     Usage: convolve <curve> <curve> [<dt>]",
               SS_nargs,
               _ULI_convlv, SS_PR_PROC);

    SS_install(si, "edit",
               "Procedure: Graphically edit a curve\n     Usage: edit <curve>",
               UL_uc,
               _ULI_edit, SS_PR_PROC);

    SS_install(si, "error-bar",
               "Procedure: Do a scatter plot with error bars on the given curve\n     Usage: error-bar <curve> <y+curve> <y-curve> [<x+curve> [<x-curve>]]",
               SS_nargs,
               _ULI_error_plot, SS_PR_PROC);

    SS_install(si, "fitcurve",
               "Procedure: Fit first curve to other curves\n     Usage: fitcurve <curve to fit> <curve-list for fit>",
               SS_nargs,
               _ULI_fit_curve, SS_PR_PROC);

    SS_install(si, "get-label",
               "Procedure: Return the given curve's label\n     Usage: get-label <curve>",
               SS_sargs,
               _ULI_crv_label, SS_PR_PROC);

    SS_install(si, "get-domain",
               "Procedure: Return the given curve's domain\n     Usage: get-domain <curve>",
               SS_sargs,
               _ULI_crv_domain, SS_PR_PROC);

    SS_install(si, "get-range",
               "Procedure: Return the given curve's range\n     Usage: get-range <curve>",
               SS_sargs,
               _ULI_crv_range, SS_PR_PROC);

    SS_install(si, "get-number-points",
               "Procedure: Return the given curve's number of points\n     Usage: get-number-points <curve>",
               SS_sargs,
               _ULI_crv_npts, SS_PR_PROC);

    SS_install(si, "get-attributes",
               "Procedure: Return the given curve's attributes, (color width style)\n     Usage: get-attributes <curve>",
               SS_sargs,
               _ULI_crv_attr, SS_PR_PROC);

    SS_install(si, "ifft",
               "Procedure: Compute Inverse FFT. Return real and imaginary parts.\n     Usage: ifft <real-curve> <imaginary-curve>",
               SS_nargs,
               _ULI_ifft, SS_PR_PROC);

    SS_install(si, "mk-pal",
               "Macro: Make a new palette and write it to a file\n     Usage: mk-pal <file-name> <integer> [on | off]",
               SS_nargs,
               _ULI_mk_palette, SS_UR_MACRO);

    SS_install(si, "rd-pal",
               "Macro: Read palette file\n     Usage: rd-pal <file-name>",
               SS_sargs,
               _ULI_rd_palette, SS_UR_MACRO);

    SS_install(si, "stats",
               "Procedure: Calculate mean and standard deviation for curves\n     Usage: stats <curve-list>",
               SS_nargs,
               _ULI_stats, SS_PR_PROC);

    SS_install(si, "write-text",
               "Procedure: Write text to screen\n     Usage: write_text <x> <y> <text>",
               SS_nargs,
               _ULI_write_abs, SS_PR_PROC);


    SS_install_cf(si, "fft",
                  "Procedure: Compute Fast Fourier Transform of real curve. Return real and imaginary parts.\n     Usage: fft <curve>",
                  UL_uc, 
                  UL_fft);
    SS_install_cf(si, "getx",
                  "Procedure: Return x values for a given y\n     Usage: getx <curve-list> <value>",
                  UL_bltocnp, 
                  UL_getx);
    SS_install_cf(si, "gety",
                  "Procedure: Return y values for a given x\n     Usage: gety <curve-list> <value>",
                  UL_bltocnp, 
                  UL_gety);
    SS_install_cf(si, "fit",
                  "Procedure: Find least-squares fit to the specified curves for a polynomial of order n\n     Usage: fit <curve-list> n",
                  UL_bltocnp, 
                  _ULI_fit);
    SS_install_cf(si, "dupx",
                  "Procedure: Duplicate x values so that y = x for each of the specified curves\n     Usage: dupx <curve-list>",
                  UL_uc, 
                  UL_dupx);
    SS_install_cf(si, "disp",
                  "Procedure: Display actual values in specified curves between min and max points\n     Usage: disp <curve-list> <xmin> <xmax>",
                  UL_ul2tocnp, 
                  _ULI_disp);

    SS_install_cf(si, "fft-order",
                  "Variable: Control the frequency space order of FFTs.\n     Usage: fft_order 0 | 1",
                  SS_acc_int,
                  &_SX.fft_order);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
