/*
 * GSPOLF.C - polygon fill rendering routines for PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

#define BILIN(_xa, _ya, _x1, _y1, _x2, _y2, _x3, _y3, _x4, _y4, _fi, _fj)  \
   {_xa = (1.0 - _fi)*(1.0 - _fj)*_x1 + _fi*(1.0 - _fj)*_x2 +              \
          (1.0 - _fi)*_fj*_x4 + _fi*_fj*_x3;                               \
    _ya = (1.0 - _fi)*(1.0 - _fj)*_y1 + _fi*(1.0 - _fj)*_y2 +              \
          (1.0 - _fi)*_fj*_y4 + _fi*_fj*_y3;}

typedef struct s_PG_poly_fill_data PG_poly_fill_data;

struct s_PG_poly_fill_data
   {PG_device *dev;
    int nd;
    double **a;
    double *x;
    double *y;
    double *aext;
    void *cnnct;
    pcons *alist;};

typedef void (*PFPolfZC)(PG_device *dev, int nd, double **a,
                         double *x, double *y, double *aext,
                         void *cnnct, pcons *alist);

typedef void (*PFPolfNC)(PG_device *dev, int nd, double **a,
                         double *x, double *y, double *aext,
                         void *cnnct, pcons *alist);


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SETUP_PICTURE_FILL_POLY - setup a window for a fill-poly rendering
 *                            - NOTE: no drawing of any kind is to be done here
 */

PG_picture_desc *PG_setup_picture_fill_poly(PG_device *dev, PG_graph *data,
					    int save, int clear)
   {int nde, nre, hvf, change;
    double ndc[PG_BOXSZ];
    double *dpex, *ddex, *pdx, *rpex, *rdex, *prx, *vwprt;
    PG_picture_desc *pd;
    PG_par_rend_info *pri;
    PG_device *dd;
    pcons *alst;

    change = !dev->supress_setup;

    pd = PG_get_rendering_properties(dev, data);

    pd->legend_contour_fl = FALSE;

    alst = pd->alist;
    pri  = dev->pri;
    if (pri != NULL)
       {dd = pri->dd;
	if (dd != NULL)
	   {dd->pri->alist  = alst;
	    dd->pri->render = PLOT_FILL_POLY;};};

/* setup the viewport */
    vwprt = pd->viewport;
    hvf   = _PG_gattrs.palette_orientation;
    if (vwprt != NULL)
       {ndc[0] = vwprt[0];
        ndc[1] = vwprt[1];
        ndc[2] = vwprt[2];
        ndc[3] = vwprt[3];}
    else if (hvf == VERTICAL)
       {ndc[0] = 0.175;
	ndc[1] = 0.725 - _PG_gattrs.palette_width;
	ndc[2] = 0.175;
	ndc[3] = 0.825;}

    else if (hvf == HORIZONTAL)
       {ndc[0] = 0.175;
	ndc[1] = 0.85;
	ndc[2] = 0.23 + _PG_gattrs.palette_width;
	ndc[3] = 0.825;}

    else
       {ndc[0] = 0.0;
	ndc[1] = 1.0;
	ndc[2] = 0.0;
	ndc[3] = 1.0;};

    if (change)
       {PG_set_viewspace(dev, 2, NORMC, ndc);

/* find the extrema for this frame */
	PG_find_extrema(data, 0.0, &dpex, &rpex, &nde, &ddex, &nre, &rdex);

/* setup the range limits */
	prx = ((dev->autorange == TRUE) || (rpex == NULL)) ? rdex : rpex;
	PG_register_range_extrema(dev, nre, prx);

/* setup the domain limits */
	pdx = ((dev->autodomain == TRUE) || (dpex == NULL)) ? ddex : dpex;
	PG_set_viewspace(dev, 2, WORLDC, pdx);

	CFREE(ddex);
	CFREE(rdex);};

    return(pd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FILL_HAND - draw and fill the polygons for the given data set */

static void PG_fill_hand(PG_device *dev, PG_graph *g,
			 PFPolfZC fnc_zc, PFPolfNC fnc_nc)
   {int i, npts, clip, prec, nd, same, sid;
    char bf[MAXLINE], *mtype, *s;
    double charspace;
    double dextr[4], chup[PG_SPACEDM], chpth[PG_SPACEDM];
    double **afs, **afd, **r, **d;
    double *fextr, *rextr, *aext;
    void *cnnct;
    PM_centering centering;
    PM_mapping *h;
    PM_set *domain, *range;
    PG_dev_attributes *attr;
    pcons *alst;

    if ((dev == NULL) || (g == NULL))
       return;

    fextr = dev->range_extrema;

    for (h = g->f; h != NULL; h = h->next)
        {domain = h->domain;
	 npts   = domain->n_elements;
	 r      = (double **) domain->elements;
	 d      = PM_convert_vectors(2, npts, r, domain->element_type);

	 PM_array_real(domain->element_type, domain->extrema, 4, dextr);

	 range = h->range;
	 npts  = range->n_elements;
	 nd    = range->dimension_elem;
	 strcpy(bf, range->element_type);
	 mtype = SC_strtok(bf, " *", s);
	 sid   = SC_type_id(mtype, FALSE);

	 same = (sid == SC_DOUBLE_I);
	 if (same == TRUE)
	    afd  = (double **) range->elements;
	 else
	    {afs = (double **) range->elements;
	     afd = CMAKE_N(double *, nd);
	     for (i = 0; i < nd; i++)
	         afd[i] = SC_convert_id(SC_DOUBLE_I, NULL, 0, 1, 
					sid, afs[i], 0, 1, npts, FALSE);};

/* find the range limits if any */
	 rextr = PM_get_limits(range);
	 aext  = ((rextr != NULL) && !dev->autorange) ? rextr : fextr;

/* find the additional mapping information */
	 centering = N_CENT;
	 alst = PM_mapping_info(h,
				"CENTERING", &centering,
				NULL);

/* setup the graph's info list */
	 alst = SC_copy_alist(alst);
	 alst = SC_append_alist(SC_copy_alist((pcons *) range->info), alst);

/* save user's values for various attributes */
	 attr = PG_get_attributes(dev);

/* set attribute values */
	 clip      = TRUE;
	 prec      = TEXT_CHARACTER_PRECISION;
	 charspace = 0.0;
	 chup[0]   = 0.0;
	 chup[1]   = 1.0;
	 chpth[0]  = 1.0;
	 chpth[1]  = 0.0;
	 PG_fset_clipping(dev, clip);
	 PG_fset_char_path(dev, chpth);
	 PG_set_char_precision(dev, prec);
	 PG_fset_char_up(dev, chup);
	 PG_set_char_space(dev, charspace);
  
	 cnnct = PM_connectivity(h);

	 switch (centering)
	    {case Z_CENT :
	          (*fnc_zc)(dev, nd, afd, d[0], d[1], aext, cnnct, alst);
		  break;

	     case N_CENT :
                  (*fnc_nc)(dev, nd, afd, d[0], d[1], aext, cnnct, alst);
		  break;

             case F_CENT :
             case U_CENT :
             default     :
                  break;};

         SC_free_alist(alst, 3);

	 PG_draw_domain_boundary(dev, h);

	 PG_update_vs(dev);

	 PG_fset_clipping(dev, FALSE);

/* reset user's values for various attributes */
	 PG_set_attributes(dev, attr);
	 CFREE(attr);

	 PM_free_vectors(2, d);

	 if (same == FALSE)
	    {for (i = 0; i < nd; i++)
	         CFREE(afd[i]);
	     CFREE(afd);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FILL_CELL - fill a cell defined by RX, RY, and NP with
 *               - color define by ND, A, and AEXT
 */

static void _PG_fill_cell(PG_device *dev, int nd, double *a, double *aext,
			  double *rx, double *ry, int np)
   {int color;
    double *r[2];
    PG_palette *pal;

    r[0] = rx;
    r[1] = ry;

    color = PG_select_color(dev, nd, a, aext);
    if (color < 2)
       {pal = dev->current_palette;
	PG_set_palette(dev, "standard");

	color = (color == 0) ? dev->GRAY : dev->DARK_GRAY;
	PG_fill_polygon_n(dev, color, TRUE, 2, WORLDC, np, r);

	dev->current_palette = pal;}

    else
       PG_fill_polygon_n(dev, color, TRUE, 2, WORLDC, np, r);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FILL_GLYPH - draw the glyph defined by ND, A, PAL, and GLYPH in
 *                - the polygon defined by RX, RY, and NP
 */

static void _PG_fill_glyph(PG_device *dev, int nd, double *a, double *aext,
			   PG_palette *pal, int *glyph,
			   double *rx, double *ry, int np)
   {int i, j, l;
    int nx, ny, nc;
    double xa, xb, xc, xd, dx;
    double ya, yb, yc, yd, dy;
    double fi, fx1, fx2, fy1, fy2;
    double v, vx[2], px[5], py[5];

    nx = glyph[0];
    ny = glyph[1];
    nc = nx*ny;

    SC_ASSERT(nc > 0);

    switch (np)

/* triangles */
       {case 4:

/* quads */
	case 5:
             fi = 0.05;

             BILIN(xa, ya,
		   rx[3], ry[3], rx[0], ry[0], rx[1], ry[1], rx[2], ry[2],
		   fi, fi);
             BILIN(xb, yb,
		   rx[3], ry[3], rx[0], ry[0], rx[1], ry[1], rx[2], ry[2],
		   (1.0 - fi), fi);
             BILIN(xc, yc,
		   rx[3], ry[3], rx[0], ry[0], rx[1], ry[1], rx[2], ry[2],
		   (1.0 - fi), (1.0 - fi));
             BILIN(xd, yd,
		   rx[3], ry[3], rx[0], ry[0], rx[1], ry[1], rx[2], ry[2],
		   fi, (1.0 - fi));

	     dx = 1.0/((double) nx);
	     dy = 1.0/((double) ny);

	     for (j = 0; j < ny; j++)
	         {fy1 = j*dy;
		  fy2 = fy1 + dy;
		  for (i = 0; i < nx; i++)
		      {l = j*nx + i;

		       v     = a[l];
		       vx[0] = aext[2*l];
		       vx[1] = aext[2*l + 1];

		       fx1 = i*dx;
		       fx2 = fx1 + dx;

		       BILIN(px[0], py[0], xa, ya, xb, yb, xc, yc, xd, yd, fx1, fy1);
		       BILIN(px[1], py[1], xa, ya, xb, yb, xc, yc, xd, yd, fx2, fy1);
		       BILIN(px[2], py[2], xa, ya, xb, yb, xc, yc, xd, yd, fx2, fy2);
		       BILIN(px[3], py[3], xa, ya, xb, yb, xc, yc, xd, yd, fx1, fy2);

		       px[4] = px[0];
		       py[4] = py[0];

		       _PG_fill_cell(dev, 1, &v, vx, px, py, 5);};};
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FILL_CHUNK_ZC_LR - do a chunk of the fill poly work */

static void *_PG_fill_chunk_zc_lr(void *arg)
   {int in, iz, j, k, l, nn, nmap, nz, kz, lz;
    int xok, yok, xso, yso;
    int lmn, lmx, nd;
    int kmax, lmax, eflag;
    int *maxes, *glyph;
    double wc[PG_BOXSZ];
    double rx[5], ry[5], *vl;
    double *x1, *x2, *x3, *x4;
    double *y1, *y2, *y3, *y4;
    double **a, *x, *y, *aext;
    char *emap;
    void *cnnct, *rv;
    pcons *alist;
    PG_device *dev;
    PG_palette *pal;
    PG_poly_fill_data *par;

    par = (PG_poly_fill_data *) arg;

    dev   = par->dev;
    nd    = par->nd;
    a     = par->a;
    x     = par->x;
    y     = par->y;
    aext  = par->aext;
    cnnct = par->cnnct;
    alist = par->alist;

    maxes = (int *) cnnct;
    kmax  = maxes[0];
    lmax  = maxes[1];
    nn    = kmax*lmax;
    nmap  = (kmax - 1) * (lmax - 1);
    nz    = SC_MEM_GET_N(double, a[0]);

    SC_ASSERT(nn > 0);
    SC_ASSERT(nz > 0);

    glyph = SC_assoc(alist, "GLYPH");

    emap = PM_check_emap(&eflag, alist, nmap);

    PM_LOGICAL_ZONE(x, x1, x2, x3, x4, kmax);
    PM_LOGICAL_ZONE(y, y1, y2, y3, y4, kmax);

    PG_get_viewspace(dev, WORLDC, wc);

    kz = kmax - 1;
    lz = lmax - 1;

    SC_ASSERT(lz > 0);

    SC_chunk_split(&lmn, &lmx, &rv);

    vl = CMAKE_N(double, nd);

    pal = dev->current_palette;

    for (l = lmn; l < lmx; l++)
        for (k = 0; k < kz; k++)
            {in = l*kmax + k;
	     iz = l*kz + k;
	     if (emap[iz] == 0)
	        continue;

	     PG_SET_WITH_CLIP_INIT(xok, xso);
	     PG_SET_WITH_CLIP_INIT(yok, yso);

	     PG_SET_WITH_CLIP(rx[0], x1[in], xok, xso, wc[0], wc[1]);
	     PG_SET_WITH_CLIP(ry[0], y1[in], yok, yso, wc[2], wc[3]);

	     PG_SET_WITH_CLIP(rx[1], x2[in], xok, xso, wc[0], wc[1]);
	     PG_SET_WITH_CLIP(ry[1], y2[in], yok, yso, wc[2], wc[3]);

	     PG_SET_WITH_CLIP(rx[2], x3[in], xok, xso, wc[0], wc[1]);
	     PG_SET_WITH_CLIP(ry[2], y3[in], yok, yso, wc[2], wc[3]);

	     PG_SET_WITH_CLIP(rx[3], x4[in], xok, xso, wc[0], wc[1]);
	     PG_SET_WITH_CLIP(ry[3], y4[in], yok, yso, wc[2], wc[3]);

	     if (xok && yok)
	        {rx[4] = x1[in];
		 ry[4] = y1[in];

		 for (j = 0; j < nd; j++)
		     vl[j] = a[j][iz];
		     
		 if (glyph != NULL)
		    _PG_fill_glyph(dev, nd, vl, aext,
				   pal, glyph, rx, ry, 5);
		 else
                    _PG_fill_cell(dev, nd, vl, aext,
				  rx, ry, 5);};};

    CFREE(vl);
    if (eflag)
        CFREE(emap);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FILL_POLY_ZC_LR - fill polygons assuming a zone centered
 *                    - Logical-Rectangular mesh
 *                    - the mesh topology is always ignored and is there
 *                    - to be call compatible with the AC version
 */

void PG_fill_poly_zc_lr(PG_device *dev, int nd, double **a,
			double *x, double *y, double *aext,
			void *cnnct, pcons *alist)
   {int in, iz, k, l, nn, nmap, nz, kz, lz;
    int xok, yok, xso, yso;
    double wc[PG_BOXSZ];
    double rx[5], ry[5];
    double *x1, *x2, *x3, *x4;
    double *y1, *y2, *y3, *y4;
    double *r[PG_SPACEDM];
    int *maxes, kmax, lmax, eflag;
    char *emap;
    PG_poly_fill_data par;

    if (dev == NULL)
       return;

    maxes = (int *) cnnct;
    kmax  = maxes[0];
    lmax  = maxes[1];
    nn    = kmax*lmax;
    nmap  = (kmax - 1) * (lmax - 1);
    nz    = SC_MEM_GET_N(double, a[0]);

    SC_ASSERT(nn > 0);
    SC_ASSERT(nz > 0);

    kz = kmax - 1;
    lz = lmax - 1;

    emap = PM_check_emap(&eflag, alist, nmap);

    PM_LOGICAL_ZONE(x, x1, x2, x3, x4, kmax);
    PM_LOGICAL_ZONE(y, y1, y2, y3, y4, kmax);

    PG_get_viewspace(dev, WORLDC, wc);

    par.dev   = dev;
    par.nd    = nd;
    par.a     = a;
    par.x     = x;
    par.y     = y;
    par.aext  = aext;
    par.cnnct = cnnct;
    par.alist = alist;

    SC_chunk_loop(_PG_fill_chunk_zc_lr, 0, lz, TRUE, &par);

    r[0] = rx;
    r[1] = ry;

    if (dev->draw_fill_bound)
       {PG_set_color_fill(dev, dev->WHITE, TRUE);

	for (l = 0; l < lz; l++)
	    for (k = 0; k < kz; k++)
	        {in = l*kmax + k;
		 iz = l*kz + k;
		 if (emap[iz] == 0)
		    continue;

		 PG_SET_WITH_CLIP_INIT(xok, xso);
		 PG_SET_WITH_CLIP_INIT(yok, yso);

		 PG_SET_WITH_CLIP(rx[0], x1[in], xok, xso, wc[0], wc[1]);
		 PG_SET_WITH_CLIP(ry[0], y1[in], yok, yso, wc[2], wc[3]);

		 PG_SET_WITH_CLIP(rx[1], x2[in], xok, xso, wc[0], wc[1]);
		 PG_SET_WITH_CLIP(ry[1], y2[in], yok, yso, wc[2], wc[3]);

		 PG_SET_WITH_CLIP(rx[2], x3[in], xok, xso, wc[0], wc[1]);
		 PG_SET_WITH_CLIP(ry[2], y3[in], yok, yso, wc[2], wc[3]);

		 PG_SET_WITH_CLIP(rx[3], x4[in], xok, xso, wc[0], wc[1]);
		 PG_SET_WITH_CLIP(ry[3], y4[in], yok, yso, wc[2], wc[3]);

		 if (xok && yok)
		    {rx[4] = x1[in];
		     ry[4] = y1[in];

		     PG_draw_polyline_n(dev, 2, WORLDC, 5, r, TRUE);};};};

    if (eflag)
        CFREE(emap);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FILL_CHUNK_NC_LR - do a chunk of the fill poly work */

static void *_PG_fill_chunk_nc_lr(void *arg)
   {int i, i1, j, k, l, nn, nmap, color, km, lm;
    int xok, yok, xso, yso;
    int lmn, lmx, nd;
    int *maxes, kmax, lmax, eflag;
    double wc[PG_BOXSZ];
    double rx[5], ry[5], *vl, *at;
    double *x1, *x2, *x3, *x4;
    double *y1, *y2, *y3, *y4;
    double **a, *x, *y, *aext;
    char *emap;
    void *cnnct, *rv;
    pcons *alist;
    PG_device *dev;
    PG_poly_fill_data *par;

    par = (PG_poly_fill_data *) arg;

    dev   = par->dev;
    nd    = par->nd;
    a     = par->a;
    x     = par->x;
    y     = par->y;
    aext  = par->aext;
    cnnct = par->cnnct;
    alist = par->alist;

    maxes = (int *) cnnct;
    kmax  = maxes[0];
    lmax  = maxes[1];
    nn    = kmax*lmax;

    SC_ASSERT(nn > 0);

    km   = kmax - 1;
    lm   = lmax - 1;
    nmap = km*lm;

    emap = PM_check_emap(&eflag, alist, nmap);

    PM_LOGICAL_ZONE(x, x1, x2, x3, x4, kmax);
    PM_LOGICAL_ZONE(y, y1, y2, y3, y4, kmax);

    PG_get_viewspace(dev, WORLDC, wc);

    SC_chunk_split(&lmn, &lmx, &rv);

    vl = CMAKE_N(double, nd);

    for (l = lmn; l < lmx; l++)
        for (k = 0; k < km; k++)
            {i  = l*kmax + k;
             i1 = l*km +k;
	     if (emap[i1] == 0)
	        continue;

	     PG_SET_WITH_CLIP_INIT(xok, xso);
	     PG_SET_WITH_CLIP_INIT(yok, yso);

	     PG_SET_WITH_CLIP(rx[0], x1[i], xok, xso, wc[0], wc[1]);
	     PG_SET_WITH_CLIP(ry[0], y1[i], yok, yso, wc[2], wc[3]);

	     PG_SET_WITH_CLIP(rx[1], x2[i], xok, xso, wc[0], wc[1]);
	     PG_SET_WITH_CLIP(ry[1], y2[i], yok, yso, wc[2], wc[3]);

	     PG_SET_WITH_CLIP(rx[2], x3[i], xok, xso, wc[0], wc[1]);
	     PG_SET_WITH_CLIP(ry[2], y3[i], yok, yso, wc[2], wc[3]);

	     PG_SET_WITH_CLIP(rx[3], x4[i], xok, xso, wc[0], wc[1]);
	     PG_SET_WITH_CLIP(ry[3], y4[i], yok, yso, wc[2], wc[3]);

	     if (xok && yok)
	        {rx[4] = x1[i];
		 ry[4] = y1[i];

		 for (j = 0; j < nd; j++)
		     {at    = a[j];
		      vl[j] = 0.25*(at[i+1] + at[i+kmax+1] +
				    at[i+kmax] + at[i]);};

		 color = PG_select_color(dev, nd, vl, aext);
		 PG_fill_polygon(dev, color, TRUE, rx, ry, 5);};};

    CFREE(vl);
    if (eflag)
        CFREE(emap);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FILL_POLY_NC_LR - draw filled polygons for the variable A on the
 *                    - Logical-Rectangular mesh specified by coordinates
 *                    - X and Y which is KMAX by LMAX
 */

void PG_fill_poly_nc_lr(PG_device *dev, int nd, double **a,
			double *x, double *y, double *aext,
			void *cnnct, pcons *alist)
   {int i, k, l, n, nmap, km, lm;
    int xok, yok, xso, yso;
    int *maxes, kmax, lmax, eflag;
    double wc[PG_BOXSZ];
    double rx[5], ry[5];
    double *x1, *x2, *x3, *x4;
    double *y1, *y2, *y3, *y4;
    double *r[PG_SPACEDM];
    char *emap;
    PG_poly_fill_data par;

    if (dev == NULL)
       return;

    maxes = (int *) cnnct;
    kmax  = maxes[0];
    lmax  = maxes[1];
    n     = kmax*lmax;

    SC_ASSERT(n > 0);

    km   = kmax - 1;
    lm   = lmax - 1;
    nmap = km*lm;

    emap = PM_check_emap(&eflag, alist, nmap);

    PM_LOGICAL_ZONE(x, x1, x2, x3, x4, kmax);
    PM_LOGICAL_ZONE(y, y1, y2, y3, y4, kmax);

    PG_get_viewspace(dev, WORLDC, wc);

    par.dev   = dev;
    par.nd    = nd;
    par.a     = a;
    par.x     = x;
    par.y     = y;
    par.aext  = aext;
    par.cnnct = cnnct;
    par.alist = alist;

    SC_chunk_loop(_PG_fill_chunk_nc_lr, 0, lm, TRUE, &par);

    r[0] = rx;
    r[1] = ry;

    if (dev->draw_fill_bound)
       {PG_set_color_fill(dev, dev->WHITE, TRUE);

	for (l = 0; l < lm; l++)
	    for (k = 0; k < km; k++)
	        {i = l*kmax + k;
		 if (emap[i] == 0)
		    continue;

		 PG_SET_WITH_CLIP_INIT(xok, xso);
		 PG_SET_WITH_CLIP_INIT(yok, yso);

		 PG_SET_WITH_CLIP(rx[0], x1[i], xok, xso, wc[0], wc[1]);
		 PG_SET_WITH_CLIP(ry[0], y1[i], yok, yso, wc[2], wc[3]);

		 PG_SET_WITH_CLIP(rx[1], x2[i], xok, xso, wc[0], wc[1]);
		 PG_SET_WITH_CLIP(ry[1], y2[i], yok, yso, wc[2], wc[3]);

		 PG_SET_WITH_CLIP(rx[2], x3[i], xok, xso, wc[0], wc[1]);
		 PG_SET_WITH_CLIP(ry[2], y3[i], yok, yso, wc[2], wc[3]);

		 PG_SET_WITH_CLIP(rx[3], x4[i], xok, xso, wc[0], wc[1]);
		 PG_SET_WITH_CLIP(ry[3], y4[i], yok, yso, wc[2], wc[3]);

		 if (xok && yok)
		    {rx[4] = x1[i];
		     ry[4] = y1[i];

		     PG_draw_polyline_n(dev, 2, WORLDC, 5, r, TRUE);};};};

    if (eflag)
        CFREE(emap);

    return;}
  
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FILL_POLY_ZC_AC - fill polygons assuming a zone centered
 *                    - Arbitrarily-Connected mesh of with mesh topology MT
 */

void PG_fill_poly_zc_ac(PG_device *dev, int nd, double **a,
			double *x, double *y, double *aext,
			void *cnnct, pcons *alist)
   {int j, is, os, iz, oz, is1, is2, in1, in2;
    int xok, yok, xso, yso, ofs, ofz;
    int nz, nzp, nsp, npt;
    int *nc, *np, *glyph;
    long **cells, *zones, *sides;
    double wc[PG_BOXSZ];
    double **r, *vl;
    PM_mesh_topology *mt;
    PG_palette *pal;

    if (dev == NULL)
       return;

    glyph = SC_assoc(alist, "GLYPH");
    mt    = (PM_mesh_topology *) cnnct;
    pal   = dev->current_palette;

    cells = mt->boundaries;
    zones = cells[2];
    sides = cells[1];

    nc = mt->n_cells;
    nz = nc[2] - 1;

    np  = mt->n_bound_params;
    nzp = np[2];
    nsp = np[1];

    vl = CMAKE_N(double, nd);

    PG_get_viewspace(dev, WORLDC, wc);

/* can compute this by looping over the sides */
    r = PM_make_vectors(2, 20);

    ofz = (nzp < 2);
    ofs = (nsp < 2);
    for (iz = 0; iz <= nz; iz++)
        {oz  = iz*nzp;
	 is1 = zones[oz];
	 is2 = zones[oz+1] - ofz;

	 npt = 0;
	 in2 = 0;
	 PG_SET_WITH_CLIP_INIT(xok, xso);
	 PG_SET_WITH_CLIP_INIT(yok, yso);
	 for (is = is1; is <= is2; is++)
	     {os  = is*nsp;
	      in1 = sides[os];
	      in2 = (ofs && (is == is2)) ? sides[is1] : sides[os+1];

	      PG_SET_WITH_CLIP(r[0][npt], x[in1], xok, xso, wc[0], wc[1]);
	      PG_SET_WITH_CLIP(r[1][npt], y[in1], yok, yso, wc[2], wc[3]);

              npt++;};

	 PG_SET_WITH_CLIP(r[0][npt], x[in2], xok, xso, wc[0], wc[1]);
	 PG_SET_WITH_CLIP(r[1][npt], y[in2], yok, yso, wc[2], wc[3]);

	 npt++;

	 if (xok && yok)
	    {for (j = 0; j < nd; j++)
	         vl[j] = a[j][iz];

	     if (glyph != NULL)
	        _PG_fill_glyph(dev, nd, vl, aext,
			       pal, glyph, r[0], r[1], npt);
	     else
	        _PG_fill_cell(dev, nd, vl, aext,
			      r[0], r[1], npt);};};

    if (dev->draw_fill_bound)
       {PG_set_color_fill(dev, dev->WHITE, TRUE);

	for (iz = 0; iz <= nz; iz++)
	    {oz  = iz*nzp;
	     is1 = zones[oz];
	     is2 = zones[oz+1] - ofz*(iz != nz);

	     npt = 0;
	     PG_SET_WITH_CLIP_INIT(xok, xso);
	     PG_SET_WITH_CLIP_INIT(yok, yso);
	     for (is = is1; is <= is2; is++)
	         {os  = is*nsp;
		  in1 = sides[os];
		  in2 = (ofs && (is == is2)) ? sides[is1] : sides[os+1];

		  PG_SET_WITH_CLIP(r[0][npt], x[in1], xok, xso, wc[0], wc[1]);
		  PG_SET_WITH_CLIP(r[1][npt], y[in1], yok, yso, wc[2], wc[3]);

		  npt++;};

	     PG_SET_WITH_CLIP(r[0][npt], x[in2], xok, xso, wc[0], wc[1]);
	     PG_SET_WITH_CLIP(r[1][npt], y[in2], yok, yso, wc[2], wc[3]);

	     npt++;

	     if (xok && yok)
	        PG_draw_polyline_n(dev, 2, WORLDC, npt, r, TRUE);};};

    CFREE(vl);
    PM_free_vectors(2, r);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FILL_POLY_NC_AC - draw filled polygons for the variable A on the
 *                    - Arbitrarily-Connected mesh specified by coordinates
 *                    - X and Y with topology MT
 */

void PG_fill_poly_nc_ac(PG_device *dev, int nd, double **a,
			double *x, double *y, double *aext,
			void *cnnct, pcons *alist)
   {double **ap;
    int j;

    ap = CMAKE_N(double *, nd);
    for (j = 0; j < nd; j++)
        ap[j] = PM_node_zone_ac_2d(a[j], cnnct, alist);

    PG_fill_poly_zc_ac(dev, nd, ap, x, y, aext, cnnct, alist);

    for (j = 0; j < nd; j++)
        CFREE(ap[j]);
    CFREE(ap);

    return;}
  
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_POLY_FILL_CORE - polygon fill plot skeleton routine */

static void _PG_poly_fill_core(PG_device *dev, PG_graph *data,
			       PFPolfZC fnc_zc, PFPolfNC fnc_nc)
   {PG_graph *g;
    PG_picture_desc *pd;

    if ((dev != NULL) && (data != NULL))
       {data->rendering = PLOT_FILL_POLY;

	pd = PG_setup_picture(dev, data, TRUE, TRUE, TRUE);

	PG_adorn_before(dev, pd, data);

/* plot all of the current functions */
	for (g = data; g != NULL; g = g->next)
	    PG_fill_hand(dev, g, fnc_zc, fnc_nc);

	PG_finish_picture(dev, data, pd);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_POLY_FILL_PLOT - main polygon fill plot control routine */

void PG_poly_fill_plot(PG_device *dev, PG_graph *data, ...)
   {

    SC_VA_START(data);

    if (strcmp(data->f->category, PM_LR_S) == 0)
       _PG_poly_fill_core(dev, data, PG_fill_poly_zc_lr, PG_fill_poly_nc_lr);

    else if (strcmp(data->f->category, PM_AC_S) == 0)
       _PG_poly_fill_core(dev, data, PG_fill_poly_zc_ac, PG_fill_poly_nc_ac);

    SC_VA_END;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
