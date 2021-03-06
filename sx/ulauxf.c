/*
 * ULAUXF.C - auxillary functions used by ultra
 *
 * Source Version: 4.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "scope_ultra.h"

#define MAX_ARY(a, ap, n, amax)                                              \
    for (ap = a, amax = *a; ap-a < n; amax = max(amax, *a++))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_FFT - work horse for complex FFT's in ULTRA */

static object *_UL_fft(SS_psides *si, object *argl,
		       const char *type, int no, int flag, int ordr)
   {int n, i, jr, ji, n0;
    double xmn, xmx;
    double *x[PG_SPACEDM], *ypr, *ypi;
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

    crvr = &SX_gs.dataset[jr];
    crvi = &SX_gs.dataset[ji];

    xmn = crvr->wc[0];
    xmx = crvr->wc[1];
    ypr = crvr->x[1];
    ypi = crvi->x[1];

    n0 = max(crvr->n, crvi->n);
    n  = PM_near_power(n0, 2);

/* make an array of complex values out of the real and imaginary values */
    icy = CMAKE_N(complex, n0);
    for (i = 0; i < n0; i++)
        icy[i] = CMPLX(ypr[i], ypi[i]);

/* GOTCHA: this assumes a common set of domain values for the curves */
    n = PM_fft_sc_complex_data(&cy, &x[0], crvr->x[0], icy, n0,
                               xmn, xmx, flag, ordr);

    no += n;

    x[1] = CMAKE_N(double, no);

/* extract the real part */
    for (i = 0; i < no; i++)
        x[1][i] = creal(cy[i]);
    lbl = SC_dsnprintf(FALSE, "Real part %s %c %c", type, crvr->id, crvi->id);
    ch1 = SX_mk_curve(si, no, x, lbl, NULL, UL_plot);

/* extract the imaginary part */
    for (i = 0; i < no; i++)
        x[1][i] = cimag(cy[i]);
    lbl = SC_dsnprintf(FALSE, "Imaginary part %s %c %c", type, crvr->id, crvi->id);
    ch2 = SX_mk_curve(si, no, x, lbl, NULL, UL_plot);

    CFREE(x[0]);
    CFREE(x[1]);
    CFREE(icy);
    CFREE(cy);

    rv = SS_make_list(si, G_OBJECT_I, ch1,
		      G_OBJECT_I, ch2,
		      0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_FFT - the main controlling routine for the FFT command */

object *UL_fft(SS_psides *si, int j)
   {int i, n;
    double xmn, xmx;
    double *x[PG_SPACEDM];
    complex *cy;
    char *lbl;
    curve *crv;
    object *cre, *cim, *rv;

    rv  = SS_null;
    crv = &SX_gs.dataset[j];
    xmn = crv->wc[0];
    xmx = crv->wc[1];

    n = PM_fft_sc_real_data(&cy, &x[0], crv->x[0], crv->x[1], crv->n,
                            xmn, xmx, _SX.fft_order);
    if ((n == 0) || (cy == NULL))
       SS_error(si, "FFT FAILED - UL_FFT", SS_null);

    else
       {x[1] = CMAKE_N(double, n);
	if (x[1] == NULL)
	   SS_error(si, "INSUFFICIENT MEMORY - UL_FFT", SS_null);

/* extract the real part */
	for (i = 0; i < n; i++)
	    x[1][i] = creal(cy[i]);
	lbl = SC_dsnprintf(FALSE, "Real part FFT %c", crv->id);
	cre = SX_mk_curve(si, n, x, lbl, NULL, UL_plot);

/* extract the imaginary part */
	for (i = 0; i < n; i++)
	    x[1][i] = cimag(cy[i]);
	lbl = SC_dsnprintf(FALSE, "Imaginary part FFT %c", crv->id);
	cim = SX_mk_curve(si, n, x, lbl, NULL, UL_plot);

	CFREE(x[0]);
	CFREE(x[1]);
	CFREE(cy);

	rv = SS_make_list(si, G_OBJECT_I, cre,
			  G_OBJECT_I, cim,
			  0);};

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
        {v  = fabs(y[i]);
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
    double *gx, *gy, *hx, *hy, dt, dxeg, dxeh;
    double *x[PG_SPACEDM];
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
            G_DOUBLE_I, &dt,
            0);

    if ((jg < 0) || (jh < 0))
       SS_error(si, "BAD CURVES - _ULI_CONVLV", argl);

    crvg = &SX_gs.dataset[jg];
    crvh = &SX_gs.dataset[jh];

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

    PM_convolve(gx, gy, gn, hx, hy, hn, dt, &x[0], &x[1], &n);

    lbl = SC_dsnprintf(FALSE, "Convolution %c %c", crvg->id, crvh->id);
    ch = SX_mk_curve(si, n, x, lbl, NULL, UL_plot);

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
                {long double x[2];

		 x[0] = val;
		 x[1] = y;
		 PRINT(stdout, "    ");
		 PRINT_DOUBLE(x[0]);
                 PRINT(stdout, "    ");
		 PRINT_DOUBLE(x[1]);
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

    s = SS_GET(SS_procedure, si->fun)->name;
    i = SX_get_data_index(s);
    if (i < 0)
       SS_error(si, "CURVE DELETED, NO PROCEDURE - CURVE-EVAL", si->fun);

    if (SS_integerp(arg))
       value = (double) *SS_GET(int64_t, arg);
    else
       value = (double) *SS_GET(double, arg);

    ret = _UL_get_value(si, SX_gs.dataset[i].x[0], SX_gs.dataset[i].x[1], value,
			SX_gs.dataset[i].n, SX_gs.dataset[i].id);
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

    tmp = _UL_get_value(si, SX_gs.dataset[i].x[1], SX_gs.dataset[i].x[0], value,
			SX_gs.dataset[i].n, SX_gs.dataset[i].id);

    o = SS_make_list(si, G_OBJECT_I, obj,
		     G_OBJECT_I, tok,
		     G_OBJECT_I, tmp,
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

    tmp = _UL_get_value(si, SX_gs.dataset[i].x[0], SX_gs.dataset[i].x[1], value,
			SX_gs.dataset[i].n, SX_gs.dataset[i].id);

    o = SS_make_list(si, G_OBJECT_I, obj,
		     G_OBJECT_I, tok,
		     G_OBJECT_I, tmp,
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

    ret   = SS_null;
    order = 0;

    if (SS_integerp(tok))
       order = SS_INTEGER_VALUE(tok);

    else
       SS_error(si, "FIT POWER MUST BE INTEGER - _ULI_FIT", tok);

    aord = abs(order) + 1;
    j    = SX_get_crv_index_i(obj);

    n     = SX_gs.dataset[j].n;
    x[0]  = SX_gs.dataset[j].x[0];
    x[1]  = SX_gs.dataset[j].x[1];
    wc[0] = SX_gs.dataset[j].wc[0];
    wc[1] = SX_gs.dataset[j].wc[1];

    cf = PM_lsq_fit(2, n, (const double **) x, wc, order);
    if (cf != NULL)

/* display coefficients and cons up the return list */
       {if (si->interactive == ON)
	   PRINT(stdout, "\nCurve %s\n", _UL_id_str(j, j));

	ret = SS_null;
	sgn = (order < 0) ? -1 : 1;
	for (i = 0 ; i < aord; i++)
	    {if (si->interactive == ON)
                {long double xv;

		 xv = cf[i];
		 PRINT(stdout, "    ");
		 PRINT_DOUBLE(xv);
		 PRINT(stdout, " *x^%d\n", sgn*i);};

	     ret = SS_mk_cons(si, SS_mk_float(si, cf[i]), ret);};
        
/* create curve of fit */
	p = PM_lsq_polynomial(SX_gs.default_npts, order, cf, wc);

	lbl = SC_dsnprintf(FALSE, "Fit %s %d", _UL_id_str(i, j), order);

	ret = SS_mk_cons(si, SS_reverse(si, ret),
			 SX_mk_curve(si, SX_gs.default_npts, p,
				     lbl, NULL, UL_plot));

	PM_free_vectors(2, p);

	CFREE(cf);};

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

    ch = SS_null;

    SS_args(si, argl,
            UL_CURVE_INDEX_I, &j,
            0);

    if (j < 0)
       SS_error(si, "BAD CURVE - _ULI_FIT_CURVE", argl);
    n = SX_gs.dataset[j].n;

    argl  = SS_cdr(si, argl);
    order = SS_length(si, argl);
    aord  = abs(order) + 1;
    curid = CMAKE_N(int, order);
    for (i = 0; i < order; i++, argl = SS_cdr(si, argl))
        curid[i] = SX_get_crv_index_i(SS_car(si, argl));
        
    ay = PM_create(n, 1);
    a  = PM_create(n, order);

    xp0 = SX_gs.dataset[j].x[0] - 1;
    yp0 = SX_gs.dataset[j].x[1] - 1;
    for (i = 1; i <= n; i++)
        PM_element(ay, i, 1) = yp0[i];

    for (k = 1; k < aord; k++)
        {x[0] = SX_gs.dataset[curid[k-1]].x[0];
         x[1] = SX_gs.dataset[curid[k-1]].x[1];
         for (i = 1; i <= n; i++)
             {while (xp0[i] > x[0][1])
                 {x[0]++;
                  x[1]++;};
              PM_interp(yv, xp0[i], *x[0], *x[1], x[0][1], x[1][1]);
              PM_element(a, i, k) = yv;};};

    if (si->interactive == ON)
       PRINT(stdout, "\n    Fit curves\n\n");

    solution = UL_lsq(a, ay);
    if (solution != NULL)

/* display coefficients */
       {id       = SX_gs.dataset[j].id;
	alpha_id = ((SX_gs.dataset[i].id >= 'A') &&
		    (SX_gs.dataset[i].id <= 'Z'));

	if (si->interactive == ON)
	   PRINT(stdout, "Fit to curve %s\n\n", _UL_id_str(i, j));

	if (alpha_id)
	   snprintf(local, MAXLINE, "Fit(%c ;", id);
	else
	   snprintf(local, MAXLINE, "Fit(%c ;", id);

	for (i = 1; i <= order; i++)
	    {id = SX_gs.dataset[curid[i-1]].id;
	     alpha_id = ((SX_gs.dataset[i].id >= 'A') &&
			 (SX_gs.dataset[i].id <= 'Z'));
	     if (si->interactive == ON)
                {long double xv;

		 xv = PM_element(solution, i, 1);
		 PRINT(stdout, "    ");
		 PRINT_DOUBLE(xv);
		 PRINT(stdout, " * curve %s\n", _UL_id_str(i, j));};

	     SC_vstrcat(local, MAXLINE, " %s", _UL_id_str(i, j));};

	SC_strcat(local, MAXLINE, ")");

/* find fit */
	PM_destroy(ay);
	ay = PM_times(a, solution);

	if (ay != NULL)
	   {UL_gs.bfa[1] = CMAKE_N(double, n);
	    for (i = 1; i <= n; i++)
	        UL_gs.bfa[1][i-1] = PM_element(ay, i, 1);};

	if (si->interactive == ON)
	   PRINT(stdout, "\n");

	x[0] = SX_gs.dataset[j].x[0];
	x[1] = UL_gs.bfa[1];

	ch = SX_mk_curve(si, SX_gs.dataset[j].n, x, local, NULL, UL_plot);

/* clean up */
	PM_destroy(a);
	PM_destroy(ay);
	PM_destroy(solution);
	CFREE(UL_gs.bfa[1]);
	CFREE(curid);};
        
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
            G_OBJECT_I, &c,
            G_OBJECT_I, &xp[1],
            G_OBJECT_I, &xm[1],
            G_OBJECT_I, &xp[0],
            G_OBJECT_I, &xm[0],
	    0);

    xpc[0] = ((xp[0] == NULL) || SS_nullobjp(xp[0])) ?
             NULL : (void *) (SX_gs.dataset + SX_get_crv_index_i(xp[0]))->x[1];

    xpc[1] = ((xp[1] == NULL) || SS_nullobjp(xp[1])) ?
             NULL : (void *) (SX_gs.dataset + SX_get_crv_index_i(xp[1]))->x[1];

    xmc[0] = ((xm[0] == NULL) || SS_nullobjp(xm[0])) ?
             NULL : (void *) (SX_gs.dataset + SX_get_crv_index_i(xm[0]))->x[1];

    xmc[1] = ((xm[1] == NULL) || SS_nullobjp(xm[1])) ?
             NULL : (void *) (SX_gs.dataset + SX_get_crv_index_i(xm[1]))->x[1];

    crv  = SX_gs.dataset + SX_get_crv_index_i(c);
    info = crv->info;

    info = PG_set_plot_type(info, PLOT_ERROR_BAR, CARTESIAN_2D);

    info = PG_set_attrs_alist(info,
			      "DX-PLUS",  G_DOUBLE_I, TRUE, xpc[0],
			      "DX-MINUS", G_DOUBLE_I, TRUE, xmc[0],
			      "DY-PLUS",  G_DOUBLE_I, TRUE, xpc[1],
			      "DY-MINUS", G_DOUBLE_I, TRUE, xmc[1],
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
            G_STRING_I, &name,
            G_INT_I, &nc,
            G_INT_I, &wbck,
            0);

    dev = SX_gs.graphics_device;
    pal = PG_make_palette(dev, name, nc, wbck);

/* write the palette for future reference */
    if (pal != NULL)
       {dev->current_palette = pal;
	fname = SC_dsnprintf(FALSE, "%s.pal", name);
        if (SX_gs.current_palette != NULL)
               CFREE(SX_gs.current_palette);
        SX_gs.current_palette = CSTRSAVE(fname);
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
            G_STRING_I, &name,
            0);

    dev = SX_gs.graphics_device;
    if (dev != NULL)

/* write the palette for future reference */
       {pal = PG_rd_palette(dev, name);
	if (pal != NULL)
	   {dev->current_palette = pal;
            if (SX_gs.current_palette != NULL)
               CFREE(SX_gs.current_palette);
            SX_gs.current_palette = 
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

    dev = SX_gs.graphics_device;
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
				  SX_gs.show_mouse_x[0],
				  SX_gs.show_mouse_x[1],
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
                     PG_fset_line_color(dev, color, TRUE);
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

    dev = SX_gs.graphics_device;

    ocv  = SX_gs.dataset + ie;

    crv  = SX_gs.dataset + SX_get_crv_index_i(UL_copy_curve(si, ie));
    n    = crv->n;
    x[0]   = crv->x[0];
    x[1]   = crv->x[1];

    indx = CMAKE_N(char, n);

/* remember the original curve's color */
    PG_get_attrs_alist(ocv->info,
		       "LINE-COLOR", G_INT_I, &oldc, dev->WHITE,
		       NULL);

/* redraw original curve in gray */
    ocv->info = PG_set_attrs_alist(ocv->info,
				   "LINE-COLOR", G_INT_I, FALSE, dev->GRAY,
				   NULL);

/* draw scatter version of curve in green */
    crv->info = PG_set_attrs_alist(crv->info,
				   "SCATTER",    G_INT_I, FALSE, TRUE,
				   "LINE-COLOR", G_INT_I, FALSE, dev->GREEN,
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

    o = (object *) crv->obj;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_DUPX - make a new curve consisting of x vs. x for the given curve */

object *UL_dupx(SS_psides *si, int j)
   {char *lbl;
    double *x[PG_SPACEDM];
    curve *ds;
    object *o;

    ds = SX_gs.dataset + j;

    lbl = SC_dsnprintf(FALSE, "Dupx %s", _UL_id_str(j, j));

    x[0] = ds->x[0];
    x[1] = ds->x[0];

    o = SX_mk_curve(si, ds->n, x, lbl, NULL, UL_plot);
        
    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_STAT - return a list of relevant statistics for the given curve */

static object *_UL_stat(SS_psides *si, int j)
   {int n;
    double xmean, xstd, ymean, ystd;
    object *ret;

    if (si->interactive == ON)
       PRINT(stdout, "\nCurve %s\n", _UL_id_str(j, j));

    n = SX_gs.dataset[j].n;
    PM_stats_mean(n, SX_gs.dataset[j].x[0], &xmean, NULL, NULL, &xstd);
    PM_stats_mean(n, SX_gs.dataset[j].x[1], &ymean, NULL, NULL, &ystd);

    ret = SS_make_list(si, G_DOUBLE_I, &xmean,
                       G_DOUBLE_I, &xstd,
                       G_DOUBLE_I, &ymean,
                       G_DOUBLE_I, &ystd,
                       0);

    if (si->interactive == ON)
       {long double lxm, lxs, lym, lys;

	lxm = xmean;
	lxs = xstd;
	lym = ymean;
	lys = ystd;

        PRINT(stdout, "\nX Mean =               ");
	PRINT_DOUBLE(lxm);
	PRINT(stdout, "\nX Standard deviation = ");
	PRINT_DOUBLE(lxs);
	PRINT(stdout, "\nY Mean =               ");
	PRINT_DOUBLE(lym);
	PRINT(stdout, "\nY Standard deviation = ");
	PRINT_DOUBLE(lys);};

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
    long double lx[2];
    double *x[PG_SPACEDM];
    object *o;

    x[0]   = SX_gs.dataset[j].x[0];
    x[1]   = SX_gs.dataset[j].x[1];
    n    = SX_gs.dataset[j].n;

    PRINT(stdout, "\n Curve %s (%s) from ",
	  _UL_id_str(j, j), SX_gs.dataset[j].text);

    lx[0] = xmin;
    lx[1] = xmax;

    PRINT_DOUBLE(lx[0]);
    PRINT(stdout, " to ");
    PRINT_DOUBLE(lx[1]);
    PRINT(stdout, "\n\n");

    for (i = 0; i < n; i++)
        {if ((x[0][i] >= xmin) && (x[0][i] <= xmax))
	    {lx[0] = x[0][i];
	     lx[1] = x[1][i];

	     PRINT(stdout, "    ");
	     PRINT_DOUBLE(lx[0]);
             PRINT(stdout, " ");
	     PRINT_DOUBLE(lx[1]);
             PRINT(stdout, "\n");};};

    PRINT(stdout, "\n");
        
    o = (object *) SX_gs.dataset[j].obj;
                
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
           PRINT(stdout, "\n Label: %s\n\n", SX_gs.dataset[i].text);
        o = SS_mk_string(si, SX_gs.dataset[i].text);};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_CRV_DOMAIN - return a cons whose car and cdr are the min and max of
 *                 - the given curve's domain
 */

static object *_ULI_crv_domain(SS_psides *si, object *obj)
   {int j;
    long double x[2];
    object *o;

    j = -1;
    SS_args(si, obj,
            UL_CURVE_INDEX_I, &j,
            0);

    o = SS_null;
    if (j != -1)
       {if (si->interactive == ON)
           {x[0] = SX_gs.dataset[j].wc[0];
	    x[1] = SX_gs.dataset[j].wc[1];

	    PRINT(stdout, "\n Domain: (");
	    PRINT_DOUBLE(x[0]);
            PRINT(stdout, " . ");
	    PRINT_DOUBLE(x[1]);
            PRINT(stdout, ")\n\n");};

        o = SS_mk_cons(si, SS_mk_float(si, SX_gs.dataset[j].wc[0]),
		       SS_mk_float(si, SX_gs.dataset[j].wc[1]));};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_CRV_RANGE - return a cons whose car and cdr are the min and max of
 *                - the given curve's range
 */

static object *_ULI_crv_range(SS_psides *si, object *obj)
   {int j;
    long double x[2];
    object *o;

    j = -1;
    SS_args(si, obj,
            UL_CURVE_INDEX_I, &j,
            0);

    o = SS_null;
    if (j != -1)
       {if (si->interactive == ON)
           {x[0] = SX_gs.dataset[j].wc[2];
	    x[1] = SX_gs.dataset[j].wc[3];

            PRINT(stdout, "\n Range: (");
	    PRINT_DOUBLE(x[0]);
            PRINT(stdout, " . ");
	    PRINT_DOUBLE(x[1]);
            PRINT(stdout, ")\n\n");};

        o = SS_mk_cons(si, SS_mk_float(si, SX_gs.dataset[j].wc[2]),
		       SS_mk_float(si, SX_gs.dataset[j].wc[3]));};

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
           PRINT(stdout, "\n Number of points: %ld\n\n", SX_gs.dataset[j].n);

        o = SS_mk_integer(si, SX_gs.dataset[j].n);};

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
       {info  = SX_gs.dataset[j].info;
	dfcol = (SX_gs.graphics_device == NULL) ? 4 : SX_gs.graphics_device->BLUE;
	   
	PG_get_attrs_alist(info,
			   "LINE-COLOR", G_INT_I, &lncol, dfcol,
			   "LINE-STYLE", G_INT_I, &lnsty, LINE_SOLID,
			   "LINE-WIDTH", G_DOUBLE_I,    &lnwid, 0.0,
			   NULL);

        if (si->interactive == ON)
           {long double wd;

	    wd = lnwid;

	    PRINT(stdout, "\n Color, width, style: (%ld ", lncol);
	    PRINT_DOUBLE(wd);
            PRINT(stdout, " %ld)\n\n", lnsty);};

        o = SS_make_list(si, G_INT_I, &lncol,
			 G_DOUBLE_I,  &lnwid,
			 G_INT_I, &lnsty,
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
            G_DOUBLE_I, &x[0],
            G_DOUBLE_I, &x[1],
            G_STRING_I, &text,
            0);

    PG_write_n(SX_gs.graphics_device, 2, NORMC, x, text);
   
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
