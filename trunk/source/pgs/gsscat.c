/*
 * GSSCAT.C - scatter plot rendering routines for PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SETUP_POINTS - limit the range and domain then allocate
 *                  - and return spaces to hold the correct number
 *                  - of points
 */

static double **_PG_setup_points(int *pnp,
				 double *rl, double *dl, double *rx, double *ry,
				 double *rf)
   {int i, id, n, np;
    double xc[PG_SPACEDM], box[PG_BOXSZ];
    double *r[PG_SPACEDM];
    double **x;

    np = *pnp;
    
    PG_box_init(3, box, -HUGE, HUGE);

    if (dl != NULL)
       PG_box_copy(2, box, dl);

    if (rl != NULL)
       {box[4] = rl[0];
	box[5] = rl[1];};

    r[0] = rx;
    r[1] = ry;
    r[2] = rf;

    x = PM_make_vectors(3, np);
    n = 0L;
    for (i = 0L; i < np; i++)
        {for (id = 0; id < PG_SPACEDM; id++)
	     xc[id] = r[id][i];

	 if (PG_box_contains(PG_SPACEDM, box, xc) == TRUE)
	    {for (id = 0; id < PG_SPACEDM; id++)
	         x[id][n] = xc[id];
	     n++;};};

    for (id = 0; id < PG_SPACEDM; id++)
        {CREMAKE(x[id], double, n);};

    *pnp = n;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DRAW_SCATTER - draw a scatter plot given
 *                  - a scalar array and view angle
 */

static void _PG_draw_scatter(PG_device *dev, int nd, double **f,
			     double *rextr, double *dextr,
			     double *x, double *y, int nn)
   {int i, j, k, id, ig;
    int color;
    double fc, z1, z2, rc;
    double lextr[2], bx[PG_BOXSZ];
    double **t, **r, *rd[PG_SPACEDM];
    PG_palette *pl;

    if (dev == NULL)
       return;

    pl = dev->current_palette;

    nn = max(nn, 12);

/* allocate space for the range/domain limited points */
    t = _PG_setup_points(&nn, rextr, dextr, x, y, f[0]);

/* allocate space for the rotated points */
    r = PM_copy_vectors(3, nn, t);

    for (i = 0; i < 4; i++)
        bx[i] = dextr[i];

    for (i = 0; i < 2; i++)
        bx[i+4] = rextr[i];

/* rotate X into rX */
    PG_rotate_vectors(dev, 3, nn, r);

    PG_fset_clipping(dev, FALSE);

/* sort f, r[0], and r[1] into ascending r[2] order */
    for (ig = nn >> 1; ig > 0; ig >>= 1)
        for (i = ig; i < nn; i++)
            for (j = i-ig; j >= 0; j -= ig)
	        {k = j + ig;
		 z1 = r[2][j];
		 z2 = r[2][k];
		 if (z1 <= z2)
                    break;

		 r[2][j] = z2;
		 r[2][k] = z1;

		 for (id = 0; id < 2; id++)
		     {rc       = r[id][j];
		      r[id][j] = r[id][k];
		      r[id][k] = rc;};};

    lextr[0] = r[2][0];
    lextr[1] = r[2][nn-1];

/* draw a dot at each point */
    for (i = 0; i < nn; i++)
        {fc = r[2][i];

	 color = PG_select_color(dev, 1, &fc, lextr);
         
	 PG_fset_line_color(dev, color, FALSE);

	 for (id = 0; id < 2; id++)
	     rd[id] = r[id] + i;
	 PG_draw_markers_n(dev, 2, WORLDC, 1, rd, 1);};

    PG_fset_palette(dev, "standard");
    PG_fset_line_color(dev, dev->WHITE, TRUE);

    PG_draw_box_n(dev, 3, WORLDC, bx);

    dev->current_palette = pl;

/* release the temporaries */
    PM_free_vectors(3, r);
    PM_free_vectors(3, t);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SCATTER_HAND - draw a scatter plot */

static void PG_scatter_hand(PG_device *dev, PG_graph *g)
   {int i, n_nodes, npts, nd, same, rexfl, sid;
    char bf[MAXLINE], *mtype, *s, *pn;
    double theta, phi, chi, dl[4];
    double *pm;
    double **r, **d, *dextr, *rextr, **afd;
    void **afs;
    PM_centering centering;
    PM_mapping *h;
    PM_set *domain, *range;
    pcons *alst;

    if ((dev == NULL) || (g == NULL))
       return;

    PG_get_attrs_graph(g, FALSE,
		       "THETA",        SC_DOUBLE_I,    &theta, 0.0,
		       "PHI",          SC_DOUBLE_I,    &phi,   0.0,
		       "CHI",          SC_DOUBLE_I,    &chi,   0.0,
		       "PALETTE",      SC_POINTER_I,   &pn,    NULL,
		       "MARKER-SCALE", SC_POINTER_I,   &pm,    NULL,
		       NULL);

    if (pn != NULL)
       PG_fset_palette(dev, pn);

    if (pm != NULL)
       PG_fset_marker_scale(dev, *pm);

    h      = g->f;
    domain = h->domain;

    strcpy(bf, domain->element_type);
    mtype   = SC_strtok(bf, " *", s);
    npts    = domain->n_elements;
    r       = (double **) domain->elements;
    d       = PM_convert_vectors(2, npts, r, domain->element_type);

    n_nodes = npts;

/* setup the domain limits */
    if (dev->autodomain)
       {PG_get_attrs_graph(g, FALSE,
			   "LIMITS", SC_POINTER_I, &dextr, NULL,
			   NULL);
	if (dextr == NULL)
	   dextr = PM_array_real(domain->element_type, domain->extrema, 4, NULL);}
    else
       {PG_get_viewspace(dev, WORLDC, dl);
	dextr = dl;};

    range = h->range;
    if (range == NULL)
       return;

    strcpy(bf, range->element_type);
    npts  = range->n_elements;
    nd    = range->dimension_elem;
    mtype = SC_strtok(bf, " *", s);
    sid   = SC_type_id(mtype, FALSE);
    same  = (sid == SC_DOUBLE_I);

    afd   = CMAKE_N(double *, nd);
    afs   = (void **) range->elements;

/* find the range limits if any */
    rextr = PM_get_limits(range);

/* setup the range limits */
    rexfl = (rextr == NULL);
    if (same == TRUE)
       {if (rexfl)
           rextr = (double *) range->extrema;
        for (i = 0; i < nd; i++)
            afd[i] = (double *) afs[i];}
    else
       {for (i = 0; i < nd; i++)
            afd[i] = SC_convert_id(SC_DOUBLE_I, NULL, 0, 1,
				   sid, afs[i], 0, 1, npts, FALSE);

        if (rexfl == TRUE)
           rextr = SC_convert_id(SC_DOUBLE_I, NULL, 0, 1,
				 sid, range->extrema, 0, 1, 2*nd, FALSE);};

    PG_register_range_extrema(dev, nd, rextr);

/* find the additional mapping information */
    centering = N_CENT;
    alst = PM_mapping_info(h,
			   "CENTERING", &centering,
			   NULL);
    SC_ASSERT(alst != NULL);

    if (centering == N_CENT)
       _PG_draw_scatter(dev, nd, afd, rextr, dextr,
			d[0], d[1], n_nodes);

    PM_free_vectors(2, d);

    if (same == FALSE)
       {for (i = 0; i < nd; i++)
            CFREE(afd[i]);

        if (rexfl)
           CFREE(rextr);};

    CFREE(afd);
    CFREE(dextr);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SETUP_PICTURE_SCATTER - setup a window for a scatter rendering
 *                          - NOTE: no drawing of any kind is to be done here
 */

PG_picture_desc *PG_setup_picture_scatter(PG_device *dev, PG_graph *data,
					  int save, int clear)
   {int nde, nre, change;
    double ndc[PG_BOXSZ];
    double *dpex, *ddex, *pdx, *rpex, *rdex, *prx;
    PG_rendering pty;
    PG_picture_desc *pd;
    PG_par_rend_info *pri;
    PG_device *dd;
    pcons *alst;

    if ((dev == NULL) || (data == NULL))
       return(NULL);

    change = !dev->supress_setup;

    pd = PG_get_rendering_properties(dev, data);

    pd->legend_contour_fl = FALSE;

    pty  = pd->rendering;
    alst = pd->alist;
    pri  = dev->pri;
    if (pri != NULL)
       {dd = pri->dd;
	if (dd != NULL)
	   {dd->pri->alist  = alst;
	    dd->pri->render = pty;};};

/* setup the viewport */
    if (change)
       {ndc[0] = 0.175;
	ndc[1] = 0.725 - _PG_gattrs.palette_width;
	ndc[2] = 0.175;
	ndc[3] = 0.825;

	PG_set_viewspace(dev, 2, NORMC, ndc);};

    if (change)

/* find the extrema for this frame */
       {PG_find_extrema(data, 0.0, &dpex, &rpex, &nde, &ddex, &nre, &rdex);

/* setup the range limits */
	prx = ((dev->autorange == TRUE) || (rpex == NULL)) ? rdex : rpex;
	PG_register_range_extrema(dev, nre, prx);

/* setup the domain limits */
	pdx = ((dev->autodomain == TRUE) || (dpex == NULL)) ? ddex : dpex;
	PG_set_viewspace(dev, 2, WORLDC, pdx);

	CFREE(ddex);
	CFREE(rdex);

/* GOTCHA: this isn't the only time we want 3d axes */
	if (((data->f->range != NULL) && (pd->va[0] != HUGE)) || (nde == 3))
	   pd->ax_type = CARTESIAN_3D;

/* set scatter plot attribute values */
	PG_fset_clipping(dev, FALSE);};

    return(pd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_SCATTER_PLOT - main scatter plot control routine */

void PG_scatter_plot(PG_device *dev, PG_graph *data, ...)
   {PG_graph *g;
    PG_picture_desc *pd;

    if ((dev != NULL) && (data != NULL))
       {data->rendering = PLOT_SCATTER;

	pd = PG_setup_picture(dev, data, TRUE, TRUE, TRUE);

	PG_adorn_before(dev, pd, data);

	switch (_PG_gattrs.hl_clear_mode)
	   {case CLEAR_SCREEN :
	         PG_clear_window(dev);
		 break;
	    case CLEAR_VIEWPORT :
		 PG_clear_viewport(dev);
		 break;
	    case CLEAR_FRAME :
		 PG_clear_frame(dev);
		 break;};

	SC_VA_START(data);

/* plot all of the current functions */
	for (g = data; g != NULL; g = g->next)
	    PG_scatter_hand(dev, g);

	SC_VA_END;

	PG_finish_picture(dev, data, pd);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_SCATTER - draw a scatter plot given a known mesh type, a
 *                 - suitable scanning function, and a scalar array
 *                 - rotate to view angle, plot points
 */

void PG_draw_scatter(PG_device *dev, double *a1, double *a2,
		     int nn, int ne, double **x,
		     double theta, double phi, double chi, double width,
		     int color, int style, int type, char *name,
		     pcons *alist)
   {int diml[1];
    PM_set *domain, *range;
    PG_graph *g;
    pcons *alst;

/* build the domain set */
    diml[0] = nn;
    domain = PM_make_set_alt("{D}", SC_DOUBLE_S, FALSE,
			     1, diml, ne, (void **) x);

/* build the range set */
    if (a2 == NULL)
       range = PM_make_set("{a}", SC_DOUBLE_S, FALSE, 1, nn, 1, a1);
    else
       range = PM_make_set("{u,v}", SC_DOUBLE_S, FALSE, 1, nn, 2, a1, a2);

    alst = SC_copy_alist(alist);

    g = PG_make_graph_from_sets(name, domain, range, N_CENT,
				SC_PCONS_P_S, alst, 'A', NULL);

    g->rendering = PLOT_SCATTER;

/* add the attributes */
    PG_set_attrs_graph(g,
		       "LINE-COLOR", SC_INT_I, FALSE, color,
		       "LINE-STYLE", SC_INT_I, FALSE, style,
		       "LINE-WIDTH", SC_DOUBLE_I,  FALSE, width,
		       "THETA",      SC_DOUBLE_I,  FALSE, theta,
		       "PHI",        SC_DOUBLE_I,  FALSE, phi,
		       "CHI",        SC_DOUBLE_I,  FALSE, chi,
		       NULL);

    PG_scatter_plot(dev, g);

    PG_rl_graph(g, FALSE, FALSE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
