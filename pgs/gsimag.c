/*
 * GSIMAG.C - image handling routines for PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "pgs_int.h"

#include "gsimag.h"

typedef void (*PFImgZC)(PG_device *dev,
			const char *name, const char *type, void *f,
                        double *frm, void *cnnct, pcons *alist);

typedef void (*PFImgNC)(PG_device *dev,
			const char *name, const char *type, void *f,
                        double *frm, void *cnnct, pcons *alist);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_MAP_TYPE_IMAGE - map the image data from the specified
 *                    - TYPE in Z to UNSIGNED CHAR in BF
 */

static void _PG_map_type_image(PG_device *dev, const char *type,
			       unsigned char *bf,
			       int kmax, int lmax, int n, int nc,
			       void *z, double zmin, double zmax)
   {int id, nd;

/* scale the data into the buffer */
    if (type == NULL)
       memcpy(bf, z, n);

    else
       {id = SC_deref_id(type, TRUE);
	if (SC_is_type_prim(id) == TRUE)
	   {nd = sizeof(_PG_map_type_image_fnc)/sizeof(PF_PG_map_type_image);
	    if ((0 <= id) && (id < nd) &&
		(_PG_map_type_image_fnc[id] != NULL))
	       _PG_map_type_image_fnc[id](dev, bf, kmax, lmax, n, nc,
					  z, zmin, zmax);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_INVERT_IMAGE_DATA - flip the image through the y axis */

void PG_invert_image_data(unsigned char *bf, int kx, int lx, int bpi)
   {int i, l, lm, lh, bc;
    long nb;
    unsigned char *ri, *ro;

    nb = kx*bpi;

    lm = lx - 1;
    lh = lx >> 1;

    for (l = 0; l < lh; l++)
        {ri = bf + l*nb;
	 ro = bf + (lm - l)*nb;
	 for (i = 0; i < nb; i++)
	     {bc    = ri[i];
	      ri[i] = ro[i];
	      ro[i] = bc;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_INVERT_IMAGE - flip the image through the y axis */

void PG_invert_image(PG_image *im)
   {int kx, lx;
    unsigned char *pb;
    char type[MAXLINE];

    SC_strncpy(type, MAXLINE, im->element_type, -1);
    PD_dereference(type);

    kx = im->kmax;
    lx = im->lmax;
    pb = im->buffer;

    PG_invert_image_data(pb, kx, lx, SIZEOF(type));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_IMAGE_HAND - image plot handler */

static void PG_image_hand(PG_device *dev, PG_graph *g,
			  double fmn, double fmx,
			  PFImgZC fnc_zc, PFImgNC fnc_nc)
   {int npts, autodomain;
    char *name;
    double *f, x[4], wc[PG_BOXSZ];
    PM_centering centering;
    PM_mapping *h;
    PM_set *domain, *range;
    void *cnnct;
    pcons *alst;

    autodomain = dev->autodomain;
    dev->autodomain = FALSE;

    for (h = g->f; h != NULL; h = h->next)
	{PG_get_viewspace(dev, WORLDC, wc);

	 domain = h->domain;

	 PM_array_real(domain->element_type, domain->extrema, 4, x);
	 PG_box_copy(2, wc, x);

	 range = h->range;
	 npts  = range->n_elements;

	 f = PM_array_real(range->element_type, DEREF(range->elements), npts, NULL);

/* find the additional mapping information */
	 centering = N_CENT;
	 alst = PM_mapping_info(h,
				"CENTERING", &centering,
				NULL);

	 cnnct = PM_connectivity(h);

         if (h == g->f)
	    name = h->name;
	 else
	    alst = PG_set_attrs_alist(alst,
				      "DRAW-AXIS",    G_INT_I, FALSE, FALSE,
				      "DRAW-LABEL",   G_INT_I, FALSE, FALSE,
				      "DRAW-PALETTE", G_INT_I, FALSE, FALSE,
				      NULL);

	 wc[4] = fmn;
	 wc[5] = fmx;

/* this is done consistently with PG_draw_vector */
	 switch (centering)
	    {case Z_CENT :
                  (*fnc_zc)(dev, name, G_DOUBLE_S, f, wc, cnnct, alst);
		  break;

             case N_CENT :
                  (*fnc_nc)(dev, name, G_DOUBLE_S, f, wc, cnnct, alst);
		  break;

             case F_CENT :
             case U_CENT :
             default     :
	          break;};

	 PG_draw_domain_boundary(dev, h);

	 CFREE(f);};

    dev->autodomain = autodomain;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_RENDER_DATA_TYPE - return B_T iff the specified graph DATA has
 *                     - a mapping for its data and B_F
 *                     - if it has a cast image
 */

pboolean PG_render_data_type(PG_graph *data)
   {const char *s;
    pboolean rv;

    s = data->use;

    rv = ((s == NULL) || (strcmp(s, G_PG_IMAGE_S) != 0));

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_IMAGE_PICTURE_INFO - return the picture info that an image might
 *                       - contain
 */

void PG_image_picture_info(PG_graph *data, int *pnd, double *dbx, double *rbx)
   {PG_image *im;

    im = (PG_image *) data->f;

    dbx[0] = im->xmin;
    dbx[1] = im->xmax;
    dbx[2] = im->ymin;
    dbx[3] = im->ymax;
    if ((dbx[0] == 0) && (dbx[1] == 0) &&
	(dbx[2] == 0) && (dbx[3] == 0))
       {dbx[1] = im->kmax;
	dbx[3] = im->lmax;};

    rbx[0] = im->zmin;
    rbx[1] = im->zmax;

    if (pnd != NULL)
       *pnd = 1;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SETUP_PICTURE_IMAGE - setup a window for a image rendering
 *                        - NOTE: no drawing of any kind is to be done here
 */

PG_picture_desc *PG_setup_picture_image(PG_device *dev, PG_graph *data,
				        int save, int clear)
   {int nde, nre, hvf, change;
    double ndc[PG_BOXSZ];
    double *dpex, *ddex, *pdx, *rpex, *rdex, *prx;
    double *vwprt;
    PG_picture_desc *pd;
    PG_device *dd;
    PG_par_rend_info *pri;
    pcons *alst;

    if (dev == NULL)
       return(NULL);

    change = !dev->supress_setup;

    pd = PG_get_rendering_properties(dev, data);
    if (pd == NULL)
       return(NULL);

    alst = pd->alist;
    pri  = dev->pri;
    if (pri != NULL)
       {dd = pri->dd;
	if (dd != NULL)
	   {dd->pri->alist  = alst;
	    dd->pri->render = PLOT_IMAGE;};};

    pd->legend_contour_fl = FALSE;

/* check on any special attributes */
    PG_get_attrs_alist(alst,
		       "POLAR-IMAGE", G_POINTER_I, &_PG.pol_extr, NULL,
		       NULL);

    if (change)

/* setup the viewport */
       {if (_PG.ok_change_view)
	   {hvf   = _PG_gattrs.palette_orientation;
	    vwprt = pd->viewport;
	    if (vwprt != NULL)
	       PG_box_copy(2, ndc, vwprt);

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
	       PG_box_init(2, ndc, 0.0, 1.0);

	    PG_set_viewspace(dev, 2, NORMC, ndc);};

/* find the extrema for this frame */
	PG_find_extrema(data, 0.0, &dpex, &rpex, &nde, &ddex, &nre, &rdex);

/* setup the range limits */
	prx = ((dev->autorange == TRUE) || (rpex == NULL)) ? rdex : rpex;
	PG_register_range_extrema(dev, nre, prx);

/* setup the domain limits */
	pdx = ((dev->autodomain == TRUE) || (dpex == NULL)) ? ddex : dpex;
	PG_set_viewspace(dev, 2, WORLDC, pdx);

	CFREE(ddex);
	CFREE(rdex);

	PG_fset_clipping(dev, FALSE);};

    return(pd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_IMAGE_CORE - image plot core routine */

static void _PG_image_core(PG_device *dev, PG_graph *data,
			   PFImgZC fnc_zc, PFImgNC fnc_nc)
   {double amin, amax, *extr;
    PG_graph *g;
    PG_picture_desc *pd;

    pd = PG_setup_picture(dev, data, FALSE, TRUE, TRUE);

    PG_adorn_before(dev, pd, data);

    _PG.ok_change_view = FALSE;

    extr = dev->range_extrema;
    amin = extr[0];
    amax = extr[1];

/* plot all of the current functions */
    for (g = data; g != NULL; g = g->next)
        PG_image_hand(dev, g, amin, amax, fnc_zc, fnc_nc);

    CFREE(pd);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DRAW_IMAGE_NC_LR - draw an image base on a node centered set of values
 *                      - on a Logical-Rectangular mesh
 */

void _PG_draw_image_nc_lr(PG_device *dev,
			  const char *name, const char *type,
			  void *f, double *frm,
			  void *cnnct, pcons *alist)
   {int *maxes, w, h, bpp;
    PG_image *im;

    maxes = (int *) cnnct;
    w     = maxes[0];
    h     = maxes[1];

    bpp = log((double) (dev->absolute_n_color))/log(2.0) + 0.5;

    im = PG_make_image_n(name, type, f, 2, WORLDC,
			 frm, frm+4, w, h, bpp, NULL);
    if (im != NULL)
       {PG_draw_image(dev, im, name, alist);
	PG_rl_image(im);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DRAW_IMAGE_ZC_LR - draw an image base on an zone centered set of values
 *                      - on a Logical-Rectangular mesh
 */

static void _PG_draw_image_zc_lr(PG_device *dev,
				 const char *name, const char *type,
				 void *f, double *frm,
				 void *cnnct, pcons *alist)
   {double *fp;

    fp = PM_z_n_lr_2d(f, NULL, NULL, FALSE, cnnct, alist);

    _PG_draw_image_nc_lr(dev, name, type, fp, frm, cnnct, alist);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_IMAGE_PIXELS - draw an image assuming colocated data */

void PG_draw_image_pixels(PG_device *dev, const char *name, const char *type,
			  void *f, double *dbx, double *ext,
			  void *cnnct, pcons *alist)
   {double frm[PG_BOXSZ];

    PG_box_copy(2, frm, dbx);
    PG_box_copy(1, frm+4, ext);

    _PG_draw_image_nc_lr(dev, name, type, f, frm, cnnct, alist);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_IMAGE_PLOT - main image plot control routine */

void PG_image_plot(PG_device *dev, PG_graph *data, ...)
   {

    _PG.im_temp = data;

    SC_VA_START(data);

    if (strcmp(data->f->category, PM_LR_S) == 0)
       _PG_image_core(dev, data, _PG_draw_image_zc_lr, _PG_draw_image_nc_lr);

    SC_VA_END;

    _PG.ok_change_view = TRUE;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_POLAR_IMAGE - convert the image whose data is on a polar mesh
 *                - from cartesian to polar form
 *                - the limits in IM are already in cartesian form
 */

static void PG_polar_image(PG_image *im, double *ext, int bc)
   {int i, j, k, l, kp, lp, ns, ir, ith;
    double dr, dth, dx, dy;
    double r, th, x, y;
    double box[PG_BOXSZ];
    unsigned char *cart, *polar;

    box[0] = im->xmin;
    box[2] = im->ymin;
    box[1] = im->xmax;
    box[3] = im->ymax;

    kp = im->kmax;
    lp = im->lmax;
    ns = kp*lp;

    dr  = (ext[1] - ext[0])/((double) kp - 1.0);
    dth = (ext[3] - ext[2])/((double) lp - 1.0);

    dx = (box[1] - box[0])/((double) kp - 1.0);
    dy = (box[3] - box[2])/((double) lp - 1.0);

    polar = im->buffer;
    cart  = im->buffer = CMAKE_N(unsigned char, ns);

    memset(cart, bc, ns);

/* poll the dst points and load the src points */
    for (l = 0; l < lp; l++)
        {y = (box[2] + l*dy);
	 for (k = 0; k < kp; k++)
             {x = (box[0] + k*dx);

	      r  = sqrt(x*x + y*y);
	      th = PM_atan(x, y)*RAD_DEG;

	      ir  = (r - ext[0])/dr;
	      ith = (th - ext[2])/dth;
	      if (((ir < 0) || (kp <= ir)) ||
		  ((ith < 0) || (lp <= ith)))
		 continue;

	      i = ith*kp + ir;
	      j = (lp - 1 - l)*kp + k;

              cart[j] = polar[i];};};

    CFREE(polar);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CROP_IMAGE - crop the image to the domain limits */

static void PG_crop_image(PG_device *dev, PG_image *im)
   {int k, k1, k2, kx, l, l1, l2, lx;
    int i, kmax, lmax;
    double wc[PG_BOXSZ], xmin, ymin;
    double dx, dy;
    unsigned char *nb, *bf;

    xmin = im->xmin;
    ymin = im->ymin;
    kmax = im->kmax;
    lmax = im->lmax;

    dx = 1.00001*(kmax - 1)/(fabs(im->xmax - xmin));
    dy = 1.00001*(lmax - 1)/(fabs(im->ymax - ymin));

    PG_get_viewspace(dev, WORLDC, wc);
    k1 = (wc[0] - xmin)*dx;
    k2 = (wc[1] - xmin)*dx;
    l1 = (wc[2] - ymin)*dy;
    l2 = (wc[3] - ymin)*dy;

/* the above assumes the domain limits are outside the viewport
 * they may in fact be inside - so compensate for that case
 */
    k1 = max(k1, 0);
    l1 = max(l1, 0);
    k2 = min(k2, kmax - 1);
    l2 = min(l2, lmax - 1);

    kx = k2 - k1 + 1;
    lx = l2 - l1 + 1;

    im->kmax = kx;
    im->lmax = lx;
    im->size = kx*lx;
    im->xmin = wc[0];
    im->xmax = wc[1];
    im->ymin = wc[2];
    im->ymax = wc[3];

    bf = im->buffer;
    nb = im->buffer = CMAKE_N(unsigned char, im->size);
    for (l = l1; l <= l2; l++)
        for (k = k1; k <= k2; k++)
            {i = l*kmax + k;
             *nb++ = bf[i];};

    CFREE(bf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_BUILD_IMAGE - map arrays to an image and return it */

PG_image *PG_build_image(PG_device *dev, const char *name,
			 const char *type, void *z,
			 int w, int h, int nd, PG_coord_sys cs,
			 double *dbx, double *rbx)
   {int n, nc, bpp;
    unsigned char *bf;
    PG_image *im;

    bpp = log((double) (dev->absolute_n_color))/log(2.0) + 0.5;

    im = PG_make_image_n(name, G_CHAR_S, NULL, nd, cs,
			 dbx, rbx, w, h, bpp, NULL);
    if (im != NULL)

/* scale the data into the buffer */
       {bf = im->buffer;
	n  = w*h;
	nc = dev->current_palette->n_pal_colors;

	_PG_map_type_image(dev, type, bf, w, h, n, nc, z, rbx[0], rbx[0]);

/* crop the image to the domain limits */
	PG_crop_image(dev, im);

	im->zmin = rbx[0];
	im->zmax = rbx[1];};

    return(im);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_VIEWPORT_SIZE - return the origin and size of the viewport
 *                      - for the purpose of image display and extraction
 */

void PG_get_viewport_size(PG_device *dev,
			  int *pix, int *piy, int *pnx, int *pny)
   {int ix, iy, nx, ny;
    double pc[PG_BOXSZ];

    PG_get_viewspace(dev, PIXELC, pc);

    ix = pc[0];
    iy = pc[2];
    nx = pc[1];
    ny = pc[3];

    nx = nx - ix + 1;
    if (ny > iy)
       ny = ny - iy + 1;
    else
       ny = iy - ny + 1;

    *pix = ix;
    *piy = iy;
    *pnx = nx;
    *pny = ny;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_PREP_IMAGE - take a raw image of any type and return an image 
 *               - of pixels ready to be rendered in the current viewport
 *               - return a pointer to the new image iff successful
 *               - return NULL otherwise
 */

static PG_image *PG_prep_image(PG_device *dev, PG_image *im,
			       int nx, int ny)
   {int w, h, n, nc;
    double zmin, zmax;
    double wc[PG_BOXSZ], dbx[PG_BOXSZ], rbx[PG_BOXSZ];
    void *z;
    char *type;
    unsigned char *bf;
    PG_palette *pal;
    PG_image *nim, wrk;

/* NOTE: don't use the zmin and zmax in the image those are only to label
 * the axis by the palette
 * NOTE: for a linked list mapping you must get the limits this way
 */
    zmin = im->zmin;
    zmax = im->zmax;

/* make a temporary copy of the image */
    wrk  = *im;
    w    = im->kmax;
    h    = im->lmax;
    type = im->element_type;
    z    = (void *) im->buffer;
    n    = w*h;

    pal = im->palette;
    if (pal == NULL)
       pal = dev->current_palette;

    nc = pal->n_pal_colors;

    bf = CMAKE_N(unsigned char, n);
    wrk.buffer = bf;

    _PG_map_type_image(dev, type, bf, w, h, n, nc, z, zmin, zmax);

/* crop the image to the domain limits */
    PG_crop_image(dev, &wrk);

    PG_get_viewspace(dev, WORLDC, wc);
    if (wrk.xmax == wrk.xmin)
       {wrk.xmax = wc[1];
	wrk.xmin = wc[0];};

    if (wrk.ymax == wrk.ymin)
       {wrk.ymax = wc[3];
	wrk.ymin = wc[2];};

    wrk.bits_pixel = log((double) (dev->absolute_n_color))/log(2.0) + 0.5;

    dbx[0] = wrk.xmin;
    dbx[1] = wrk.xmax;
    dbx[2] = wrk.ymin;
    dbx[3] = wrk.ymax;
    rbx[0] = wrk.zmin;
    rbx[1] = wrk.zmax;

    nim = PG_make_image_n(dev->title, G_STRING_S, NULL, 2, WORLDC,
			  dbx, rbx, nx, ny, wrk.bits_pixel, wrk.palette);
    if (nim != NULL)
       PG_place_image(nim, &wrk, TRUE);

    CFREE(wrk.buffer);

    return(nim);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_IMAGE - given an image structure map the pixels to the
 *               - screen
 */

void PG_draw_image(PG_device *dev, PG_image *im, 
		   const char *label, pcons *alist)
   {int nx, ny, do_par_set;
    double pc[PG_BOXSZ];
    PG_image *nim;
    PG_graph g, *data;
    PG_picture_desc *pd;

    if ((dev != NULL) && (im != NULL))
       {im->palette = dev->current_palette;

	if (_PG.im_temp == NULL)
	   {data = &g;
	    SC_MEM_INIT(PG_graph, data);

	    g.f         = (PM_mapping *) im;
	    g.use       = G_PG_IMAGE_S;
	    g.rendering = PLOT_IMAGE;
	    g.info_type = G_PCONS_P_S;
	    g.info      = (void *) alist;
	    do_par_set  = TRUE;}

	else
	   {data        = _PG.im_temp;
	    do_par_set  = FALSE;};

	pd = PG_setup_picture(dev, data, TRUE, TRUE, do_par_set);

	PG_adorn_before(dev, pd, data);

	PG_get_viewspace(dev, PIXELC, pc);
	nx = pc[1] - pc[0];
	ny = pc[3] - pc[2];

	if (!_PG_allocate_image_buffer(dev, NULL, &nx, &ny))
	   return;

	pc[1] = pc[0] + nx;
	pc[3] = pc[2] + ny;

/* scale the image to fit its part of the viewport */
	nim = PG_prep_image(dev, im, nx, ny);
	if (nim != NULL)
	   {if (_PG.pol_extr != NULL)
	       PG_polar_image(nim, _PG.pol_extr, dev->BLACK);

	    PG_put_image_n(dev, nim->buffer, PIXELC, pc);

	    PG_rl_image(nim);};

	pd->mesh_fl &= (_PG.im_temp != NULL);
	PG_finish_picture(dev, data, pd);

	_PG.im_temp = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_EXTRACT_IMAGE - extract a PG_image from the region of DEV
 *                  - specified by the box DBX
 */
 
PG_image *PG_extract_image(PG_device *dev, double *dbx, double *rbx)
   {int nx, ny, bpp;
    unsigned char *bf;
    double wc[PG_BOXSZ];
    PG_image *im;

    im = NULL;
    if (dev != NULL)
       {if ((dbx[2] == 0.0) || (dbx[3] == 0.0))
	   PG_get_viewspace(dev, PIXELC, dbx);

	PG_get_viewspace(dev, WORLDC, wc);

	nx = dbx[1] - dbx[0];
	ny = dbx[3] - dbx[2];

	bpp = log((double) (dev->absolute_n_color))/log(2.0) + 0.5;
	im  = PG_make_image_n(dev->title, G_CHAR_S, NULL, 2, WORLDC,
			      wc, rbx, nx, ny, bpp, NULL);

/* load up the image data */
	if (im != NULL)
	   {bf = im->buffer;
	    PG_get_image_n(dev, bf, PIXELC, dbx);};};
 
    return(im);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PAR_RESET_VIEWPORT - reset the viewport to the actual image size
 *                        - NOTE: temporary until roundoff problems in
 *                        -       original parallel viewport computation
 *                        -       are fixed
 */

static void _PG_par_reset_viewport(PG_device *dev, PG_image *im,
				   int ix, int iy, int *pnx, int *pny)
   {int mx, my, nx, ny;
    double ndc[PG_BOXSZ], pc[PG_BOXSZ];

    if (dev != NULL)
       {nx = im->kmax;
	ny = im->lmax;
    
	mx = ix + nx;
	my = iy + ny;

	pc[0] = ix;
	pc[1] = mx;
	pc[2] = iy;
	pc[3] = my;

	PG_trans_box(dev, 2, PIXELC, pc, NORMC, ndc);

	PG_set_viewspace(dev, 2, NORMC, ndc);

	*pnx = nx;
	*pny = ny;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_RENDER_PARALLEL - complete the parallel rendering of a picture */

void PG_render_parallel(PG_device *dd, PG_image *nim, int np, PG_image *pim)
   {int ix, iy, nx, ny;
    double pc[PG_BOXSZ];
    PG_graph g, *data;
    PG_picture_desc *pd;
    PG_palette *pl;

    if ((dd == NULL) || (nim == NULL))
       return;

    data = &g;
    SC_MEM_INIT(PG_graph, data);

    g.f         = (PM_mapping *) nim;
    g.use       = G_PG_IMAGE_S;
    g.info_type = G_PCONS_P_S;
    g.info      = (void *) dd->pri->alist;
    g.rendering = dd->pri->render;

    pd = PG_setup_picture(dd, data, TRUE, FALSE, FALSE);

    PG_adorn_before(dd, pd, data);

    PG_get_viewspace(dd, PIXELC, pc);
    ix = pc[0];
    iy = pc[2];
    nx = pc[1] - pc[0];
    ny = pc[3] - pc[2];

/* GOTCHA: this is only necessary because the round off issues
 *         are not dealt with completely in PG_setup_parallel
 *         hence nx and nim->kmax et. al. differ by 2-5 pixels
 */
    _PG_par_reset_viewport(dd, nim, ix, iy, &nx, &ny);
    pc[2] = nx + pc[0];
    pc[3] = ny + pc[1];

    pl = dd->current_palette;
    dd->current_palette = dd->color_table;

    PG_put_image_n(dd, nim->buffer, PIXELC, pc);

    dd->current_palette = pl;

    PG_finish_picture(dd, data, pd);

/* release nim only now because the label printed in PG_finish_picture
 * comes from here
 */
    PG_rl_image(nim);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FILL_PALETTE_IMAGE - fill in the pixels for the palette image
 *                        - add 2 to each color to allow for the mandatory
 *                        - black and white included in each palette
 */

static void _PG_fill_palette_image(PG_device *dev, unsigned char *bf,
				   int nx, int ny, int hvf,
				   int nc, int ncx, int ncy)
   {int l, k, ls;
    unsigned char *pbf;
    double a[2], extr[4];
    double scale;

    pbf = bf;

/* 1-d color table */
    if (ncx == nc) 
       {if (hvf == VERTICAL)
           {scale = 0.9999*nc/((double) ny);
            for (l = 0; l < ny; l++)
                {ls = (ny - l)*scale;
                 for (k = 0; k < nx; k++)
                     *pbf++ = ls + 2;};}

        else if (hvf == HORIZONTAL)
           {scale = 0.9999*nc/((double) nx);
            for (l = 0; l < ny; l++)
                {for (k = 0; k < nx; k++)
                     *pbf++ = k*scale + 2;};};}

/* 2-d color table */
    else if((ncx*ncy == nc) && (ncy > 1)) 
       {extr[0] = 0.0;
	extr[1] = (double) (nx - 1);
        extr[2] = 0.0;
	extr[3] = (double) (ny - 1);
        for (l = ny - 1; l >= 0; l--)
            {a[1] = (double) ((ny - 1) - l);
             for (k = 0; k < nx; k++)
                {a[0] = (double) k;
                 pbf[l*nx + k] = (unsigned char) PG_select_color(dev, 2, a, extr);};};}

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RENDER_PALETTE - draw a 1D or 2D palette */

static void _PG_render_palette(PG_device *dev, PG_palette *pal,
			       int hvf, int nd, int *nc, double *ndc)
   {int n_color;
    int ix[PG_SPACEDM], nx[PG_SPACEDM];
    double pc[PG_BOXSZ];
    unsigned char *bf;

    PG_trans_box(dev, 2, NORMC, ndc, PIXELC, pc);

    ix[0] = pc[0];
    nx[0] = pc[1];
    ix[1] = pc[2];
    nx[1] = pc[3];

    nx[0] = nx[0] - ix[0] + 1;
    if (nx[1] > ix[1])
       nx[1] = nx[1] - ix[1] + 1;
    else
       nx[1] = ix[1] - nx[1] + 1;

    if (!_PG_allocate_image_buffer(dev, &bf, &nx[0], &nx[1]))
       return;

    pc[1] = pc[0] + nx[0];
    pc[3] = pc[2] + nx[1];

/* compute a palette image */
    n_color = pal->n_pal_colors;
    _PG_fill_palette_image(dev, bf, nx[0], nx[1], hvf, n_color, nc[0], nc[1]);

/* move image off axis */
    if (hvf == VERTICAL)
       ix[0] -= 1;
    else
       ix[1] += 1;

    PG_invert_image_data(bf, nx[0], nx[1], 1);

/* draw the palette image */
    PG_put_image_n(dev, bf, PIXELC, pc);

    CFREE(bf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_PALETTE_N - Draw the current palette of the device DEV.
 *                   - Place it exactly where requested if EXACT is TRUE,
 *                   - otherwise place it according to the rendering
 *                   - and the viewport size and shape.
 *                   - DBX specifies where the palette image is to be drawn.
 *                   - RBX specifies where the axis labelling the range
 *                   - values is to be drawn.
 *
 * #bind PG_draw_palette_n fortran() scheme() python()
 */

void PG_draw_palette_n(PG_device *dev ARG(,,cls),
		       double *dbx ARG([0.0,1.0,0.0,1.0,0.0,1.0]),
                       double *rbx ARG([0.0,1.0,0.0,1.0,0.0,1.0]),
		       double wid ARG(0.0),
		       int exact ARG(FALSE))
   {int hvf;
    int nc[PG_SPACEDM];
    char format[20];
    double scale, toler;
    double tn[2], vw[2];
    double ndc[PG_BOXSZ];
    double xl[PG_SPACEDM], xr[PG_SPACEDM];
    PG_palette *pal;

    if (dev == NULL)
       return;

    PG_get_viewspace(dev, NORMC, ndc);

/* move palette outside of the viewport if asked */
    if (exact == FALSE)

/* adjust vertical palette to be outside the viewport if not already */
       {if ((dbx[0] == dbx[1]) && (dbx[0] <= ndc[1]))
	   {dbx[1] = ndc[1] + 0.25*(1.0 - ndc[1]);
	    dbx[0] = dbx[1];
	    dbx[2] = ndc[2];
	    dbx[3] = ndc[3];}

/* adjust horizontal palette to be outside the viewport if not already */
        else if ((dbx[2] == dbx[3]) && (dbx[2] >= ndc[2]))
	   {dbx[0] = ndc[0];
	    dbx[1] = ndc[1];
	    dbx[2] = 0.4*ndc[2];
	    dbx[3] = dbx[2];};};

    PG_frame_viewport(dev, 2, dbx);

/* reduce number of off-by-one pixel cases */
    PG_trans_box(dev, 2, NORMC, dbx, PIXELC, dbx);
    PG_trans_box(dev, 2, PIXELC, dbx, NORMC, dbx);

    PG_box_copy(2, ndc, dbx);

/* set up the palette image */
    PG_trans_box(dev, 2, NORMC, dbx, WORLDC, dbx);

    pal = dev->current_palette;
    dev->current_palette = dev->palettes;

    PG_fset_line_color(dev, dev->WHITE, TRUE);
    SC_strncpy(format, 20, "%10.2g", -1);

/* NOTE: this value is chosen to avoid problems with coordinate conversions
 *       and clipping for arbitrary size windows
 */
    toler = 10.0*PG_window_width(dev)/((double) INT_MAX);
    if (fabs(rbx[1] - rbx[0]) < toler*(fabs(rbx[0]) + fabs(rbx[1])))
       {if (rbx[0] == 0.0)
	   {rbx[0] -= 0.1;
	    rbx[1] += 0.1;}
        else
	   {rbx[0] -= 0.5*toler*rbx[0];
	    rbx[1] += 0.5*toler*rbx[1];};};

    if (ndc[0] == ndc[1])
       {ndc[0] = ndc[1] - wid;
        hvf    = VERTICAL;
        scale  = (dbx[3] - dbx[2])/(rbx[1] - rbx[0] + SMALL);

	xl[0] = dbx[0];
	xl[1] = dbx[2];
	xr[0] = dbx[0];
	xr[1] = dbx[3];
	tn[0] = 0.0;
	tn[1] = 1.0;
	vw[0] = rbx[0];
	vw[1] = rbx[1];
        PG_draw_axis_n(dev, xl, xr, tn, vw, scale, format,
		       AXIS_TICK_RIGHT, AXIS_TICK_RIGHT, FALSE,
		       AXIS_TICK_MAJOR | AXIS_TICK_MINOR | AXIS_TICK_LABEL, 0);

        dev->current_palette = pal;}

    else if (ndc[2] == ndc[3])
       {ndc[3]  = ndc[2] + wid;
        hvf   = HORIZONTAL;
    
        scale = (dbx[1] - dbx[0])/(rbx[1] - rbx[0] + SMALL);

	xl[0] = dbx[0];
	xl[1] = dbx[2];
	xr[0] = dbx[1];
	xr[1] = dbx[2];
	tn[0] = 0.0;
	tn[1] = 1.0;
	vw[0] = rbx[0];
	vw[1] = rbx[1];
        PG_draw_axis_n(dev, xl, xr, tn, vw, scale, format,
		       AXIS_TICK_RIGHT, AXIS_TICK_RIGHT, FALSE,
		       AXIS_TICK_MAJOR | AXIS_TICK_MINOR | AXIS_TICK_LABEL, 0);

        dev->current_palette = pal;}

    else
       {dev->current_palette = pal;
        return;};

    nc[0] = pal->n_pal_colors;
    nc[1] = 0;

    _PG_render_palette(dev, pal, hvf, 1, nc, ndc);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_2DPALETTE - display the palette next to the image */

void PG_draw_2dpalette(PG_device *dev, double *frm, double *rex, double wid)
   {int **pd, *shape, hvf, fsize;
    int nc[PG_SPACEDM];
    char format[20], *face, *style;
    double scale, toler;
    double tn[2], vw[2];
    double ndc[PG_BOXSZ];
    double xl[PG_SPACEDM], xr[PG_SPACEDM];
    PG_palette *pal;

    if (dev == NULL)
       return;

    pal = dev->current_palette;
    pd  = pal->pal_dims;

    if ((pd == NULL) || (pal->max_pal_dims != 2))
       {PG_draw_palette_n(dev, frm, rex, wid, FALSE);
	return;};

    PG_frame_viewport(dev, 2, frm);

/* reduce number of off-by-one pixel cases */
    PG_trans_box(dev, 2, NORMC, frm, PIXELC, frm);
    PG_trans_box(dev, 2, PIXELC, frm, NORMC, frm);

    PG_box_copy(2, ndc, frm);

/* set up the palette image */
    PG_trans_box(dev, 2, NORMC, frm, WORLDC, frm);

    dev->current_palette = dev->palettes;

    PG_fset_line_color(dev, dev->WHITE, TRUE);
    SC_strncpy(format, 20, "%10.2g", -1);

/* NOTE: this value is chosen to avoid problems with coordinate conversions
 *       and clipping for arbitrary size windows
 */
    toler = 10.0*PG_window_width(dev)/((double) INT_MAX);
    if (fabs(rex[1] - rex[0]) < toler*(fabs(rex[0]) + fabs(rex[1])))
       {if (rex[0] == 0.0)
	   {rex[0] -= 0.1;
	    rex[1] += 0.1;}
        else
	   {rex[0] -= 0.5*toler*rex[0];
	    rex[1] += 0.5*toler*rex[1];};};

    if (fabs(rex[3] - rex[2]) < toler*(fabs(rex[2]) + fabs(rex[3])))
       {if (rex[2] == 0.0)
	   {rex[2] -= 0.1;
	    rex[3] += 0.1;}
        else
	   {rex[2] -= 0.5*toler*rex[2];
	    rex[3] += 0.5*toler*rex[3];};};

    shape = pal->pal_dims[pal->max_pal_dims - 1];
    nc[0] = shape[0];
    nc[1] = shape[1];

    if (ndc[0] == ndc[1])
/*
       {ndc[0]  = ndc[1] - wid;
        hvf   = VERTICAL;
        scale = (frm[3] - frm[2])/(zmx - zmn + SMALL);

	xl[0] = frm[0];
	xl[1] = frm[2];
	xr[0] = frm[0];
	xr[1] = frm[3];
	tn[0] = 0.0;
	tn[1] = 1.0;
	vw[0] = zmn;
	vw[1] = zmx;
        PG_draw_axis_n(dev, xl, xr, tn, vw, scale, format,
                       AXIS_TICK_RIGHT, AXIS_TICK_RIGHT, FALSE,
		       AXIS_TICK_MAJOR | AXIS_TICK_MINOR | AXIS_TICK_LABEL, 0);
*/
       {dev->current_palette = pal;
        return;} /* VERTICAL not supported yet */

    else
       {hvf   = HORIZONTAL;
        scale = (frm[1] - frm[0])/(rex[1] - rex[0] + SMALL);

        PG_fget_font(dev, &face, &style, &fsize);
        PG_fset_font(dev, face, style, 8);
        
	xl[0] = frm[0];
	xl[1] = frm[2];
	xr[0] = frm[1];
	xr[1] = frm[2];
	tn[0] = 0.0;
	tn[1] = 1.0;
	vw[0] = rex[0];
	vw[1] = rex[1];
        PG_draw_axis_n(dev, xl, xr, tn, vw, scale, format,
		       AXIS_TICK_RIGHT, AXIS_TICK_RIGHT, FALSE,
		       AXIS_TICK_MAJOR | AXIS_TICK_MINOR | AXIS_TICK_LABEL, 0);

        scale = (frm[3] - frm[2])/(rex[3] - rex[2] + SMALL);   

	xl[0] = frm[0];
	xl[1] = frm[2];
	xr[0] = frm[0];
	xr[1] = frm[3];
	tn[0] = 0.0;
	tn[1] = 1.0;
	vw[0] = rex[2];
	vw[1] = rex[3];
        PG_draw_axis_n(dev, xl, xr, tn, vw, scale, format,
		       AXIS_TICK_LEFT, AXIS_TICK_LEFT, FALSE,
		       AXIS_TICK_MAJOR | AXIS_TICK_NONE | AXIS_TICK_LABEL, 0);

        PG_fset_font(dev, face, style, fsize);

        dev->current_palette = pal;}

    _PG_render_palette(dev, pal, hvf, 2, nc, ndc);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_BYTE_BIT_MAP - compress a byte map to a bit map
 *                  - return the effective row width of the bit map
 */

int _PG_byte_bit_map(unsigned char *bf, int nx, int ny, int complmnt)
   {int i, k, l;
    unsigned int mask, xbyte;
    unsigned char *pbt, *pbf;

    pbf = bf;
    pbt = bf;
    mask  = 0x80;
    xbyte = 0;

    if (complmnt)
       {for (l = 0; l < ny; l++)
            {for (k = 0; k < nx; k++)
                 {i = *pbf++;
                  xbyte |= mask*i;
                  mask >>= 1;
                  if (mask == 0)
                     {mask   = 0x80;
                      *pbt++ = ~xbyte;
                      xbyte  = 0;};};
             mask   = 0x80;
             *pbt++ = ~xbyte;
             xbyte  = 0;};}

    else
       {for (l = 0; l < ny; l++)
            {for (k = 0; k < nx; k++)
                 {i = *pbf++;
                  xbyte |= mask*i;
                  mask >>= 1;
                  if (mask == 0)
                     {mask   = 0x80;
                      *pbt++ = xbyte;
                      xbyte  = 0;};};
             mask   = 0x80;
             *pbt++ = xbyte;
             xbyte  = 0;};};

    return((nx + 7) >> 3);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MAKE_IMAGE_N - Allocate, initialize and return an image with
 *                 - width W, height, H, and bits per pixel, BPP.
 *                 - The pixel values are contained in the array Z
 *                 - which is of type, TYPE.
 *                 - Other arguments specifying the image:
 *                 -    LABEL    image name
 *                 -    ND       dimension of image data
 *                 -    CS       coordinate system of the image domain
 *                 -    DBX      specifies where the palette image
 *                 -             is to be drawn
 *                 -    RBX      specifies where the axis labelling the range
 *                 -    PALETTE  palette in which the image is to be rendered
 *
 * #bind PG_make_image_n fortran() scheme() python()
 */

PG_image *PG_make_image_n(const char *label, const char *type, void *z,
			  int nd, PG_coord_sys cs, double *dbx, double *rbx,
			  int w, int h, int bpp,
			  PG_palette *palette)
   {int byp;
    long width, height, size;
    char bf[MAXLINE];
    PG_image *im;

    if (dbx[0] == dbx[1])
       {dbx[0] = 0.0;
	dbx[1] = (double) w;};

    if (dbx[2] == dbx[3])
       {dbx[2] = 0.0;
	dbx[3] = (double) h;};

    byp = bpp >> 3;
    byp = max(byp, 1);

    width  = w;
    height = h;
    size   = width*height*byp;

    im = CMAKE(PG_image);
    if (im == NULL)
       return(NULL);

    if (z == NULL)
       {im->buffer = CMAKE_N(unsigned char, size*SC_sizeof(type));
	if (im->buffer == NULL)
	   {CFREE(im);
	    return(NULL);};}
    else
       im->buffer = (unsigned char *) z;

    snprintf(bf, MAXLINE, "%s *", type);

    im->version_id   = PG_IMAGE_VERSION;
    im->label        = CSTRSAVE(label);
    im->element_type = CSTRSAVE(bf);
    im->xmin         = dbx[0];
    im->xmax         = dbx[1];
    im->ymin         = dbx[2];
    im->ymax         = dbx[3];
    im->zmin         = rbx[0];
    im->zmax         = rbx[1];
    im->kmax         = w;
    im->lmax         = h;
    im->size         = size;
    im->bits_pixel   = bpp;
    im->palette      = palette;

    SC_mark(im->label, 1);
    SC_mark(im->element_type, 1);
    SC_mark(im->buffer, 1);

    return(im);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_ALLOCATE_IMAGE_BUFFER - allocate an appropriately sized buffer */

int _PG_allocate_image_buffer(PG_device *dev, unsigned char **pbf,
			      int *pnx, int *pny)
   {int i;
    int ix[PG_SPACEDM], nx[PG_SPACEDM];
    unsigned char *bf;

    ix[0] = 0;
    ix[1] = 0;
    nx[0] = *pnx;
    nx[1] = *pny;

/* allocate the working buffer according to the scale factor
 * or largest array the system will give us
 */
    bf = NULL;
    for (i = dev->resolution_scale_factor; i < 512; i <<= 2)
        {ix[0] = nx[0]/i;
         ix[1] = nx[1]/i;

	 if (CGM_DEVICE(dev))
	    {if (ix[0] & 1)
	        ix[0]++;

	     if (ix[1] & 1)
	        ix[1]++;};

	 bf = CMAKE_N(unsigned char, ix[0]*ix[1]);
         if (bf != NULL)
            break;};

    if (bf != NULL)
       memset(bf, dev->BLACK, ix[0]*ix[1]);

    dev->resolution_scale_factor = i;

    if (pbf != NULL)
       *pbf = bf;
    else
       CFREE(bf);

    *pnx = ix[0];
    *pny = ix[1];

    return(i < 512);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_RL_IMAGE - free the space associated with an image */

void PG_rl_image(PG_image *im)
   {

    if (im != NULL)
       {if (im->label)
           {CFREE(im->label);
            im->label = NULL;};

        if (im->element_type)
           {CFREE(im->element_type);
            im->element_type = NULL;};

        if (im->buffer)
           {CFREE(im->buffer);
            im->buffer = NULL;};

	CFREE(im);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SHIFT_IMAGE_RANGE - shift the data values of B to match
 *                      - the range of A and add on offset OFF
 */

int PG_shift_image_range(PG_image *a, PG_image *b, int off)
   {int i, nb, ok;
    unsigned int ac;
    unsigned char *pb;
    double azn, azx, bzn, bzx;
    double a0, b0, as, ai, bc, nca;
    PG_palette *pal;

    ok = TRUE;

    azn = a->zmin;
    azx = a->zmax;
    pal = a->palette;
    nca = pal->n_pal_colors - off;

    nb  = b->size;
    pb  = b->buffer;
    bzn = b->zmin;
    bzx = b->zmax;

    if (azn == azx)
       {as = 0.0;
	ai = off;}
    else
       {a0 = nca/(azx - azn);
	b0 = (bzx - bzn)/nca;
	as = a0*b0;
	ai = a0*(bzn - azn) + off;};

    for (i = 0; i < nb; i++)
        {bc = (double) (pb[i] - off);
	 ac = (unsigned char) (as*bc + ai);

	 pb[i] = ac;};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_PLACE_IMAGE - translate, scale, and copy the data from SIM to DIM
 *                - in the region of overlap
 */

int PG_place_image(PG_image *dim, PG_image *sim, int scale)
   {int sx, sy, dx, dy;
    int k, kmn, kmx, l, lmn, lmx;
    int ix, iy, id, is, z1, z2, z3, z4;
    unsigned char *iz1, *iz2, *iz3, *iz4;
    unsigned char *sp, *dp;
    double x, y, fx, fy, dxp, dyp;
    double ubx[PG_BOXSZ], dbx[PG_BOXSZ], sbx[PG_BOXSZ];

    sx = sim->kmax;
    sy = sim->lmax;
    sp = sim->buffer;

    dx = dim->kmax;
    dy = dim->lmax;
    dp = dim->buffer;

/* define the region of physical overlap */
    sbx[0] = sim->xmin;
    sbx[1] = sim->xmax;
    sbx[2] = sim->ymin;
    sbx[3] = sim->ymax;

    dbx[0] = dim->xmin;
    dbx[1] = dim->xmax;
    dbx[2] = dim->ymin;
    dbx[3] = dim->ymax;

    ubx[0] = max(sbx[0], dbx[0]);
    ubx[1] = min(sbx[1], dbx[1]);
    ubx[2] = max(sbx[2], dbx[2]);
    ubx[3] = min(sbx[3], dbx[3]);

    dxp = (dbx[1] - dbx[0])/((double) dx);
    dyp = (dbx[3] - dbx[2])/((double) dy);

    kmn = (ubx[0] - dbx[0])/dxp + 0.5;
    kmx = (ubx[1] - dbx[0])/dxp + 0.5;
    lmn = (ubx[2] - dbx[2])/dyp + 0.5;
    lmx = (ubx[3] - dbx[2])/dyp + 0.5;

    if (scale)
       {fx = ((double) (sx-2))/((double) (dx-1));
	fy = ((double) (sy-2))/((double) (dy-1));}
    else
       {fx = 1.0;
	fy = 1.0;};

/* set up the cell corner pointers of the source data */
    iz1 = sp - sx;
    iz2 = sp;
    iz3 = iz2 - 1;
    iz4 = iz1 - 1;

/* map it all in */
    for (l = lmn; l < lmx; l++)
        {for (k = kmn; k < kmx; k++)
             {x  = k*fx;
              y  = l*fy;
              ix = (int) floor(x) + 1;
              iy = (int) floor(y) + 1;

              id = l*dx + k;
              is = iy*sx + ix;

              x -= floor(x);
              y -= floor(y);

              z1 = iz1[is];
              z2 = iz2[is];
              z3 = iz3[is];
              z4 = iz4[is];

              dp[id] = ((z4 - z3 + z2 - z1)*x + (z3 - z4))*y +
                       (z1 - z4)*x + z4;};};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_COPY_IMAGE - copy the data from SIM to DIM
 *               - in the region of overlap
 *               - NOTE: do not copy background color (BCK) pixels from
 *               -       the source image SIM
 */

int PG_copy_image(PG_image *dim, PG_image *sim, int bck)
   {int ic, id, is, od, os, c, bpp;
    int k, kmnd, kmxd, lmnd, lmxd, dk;
    int l, kmns, lmns, dl;
    unsigned char *sp, *dp;
    double dx, dy, dxp, dyp, sx, sy, sxp, syp;
    double ubx[PG_BOXSZ], dbx[PG_BOXSZ], sbx[PG_BOXSZ];

    sx = sim->kmax;
    sy = sim->lmax;
    sp = sim->buffer;

    dx = dim->kmax;
    dy = dim->lmax;
    dp = dim->buffer;

/* define the region of physical overlap */
    sbx[0] = sim->xmin;
    sbx[1] = sim->xmax;
    sbx[2] = sim->ymin;
    sbx[3] = sim->ymax;

    dbx[0] = dim->xmin;
    dbx[1] = dim->xmax;
    dbx[2] = dim->ymin;
    dbx[3] = dim->ymax;

    ubx[0] = max(sbx[0], dbx[0]);
    ubx[1] = min(sbx[1], dbx[1]);
    ubx[2] = max(sbx[2], dbx[2]);
    ubx[3] = min(sbx[3], dbx[3]);

    dxp = (dbx[1] - dbx[0])/dx;
    dyp = (dbx[3] - dbx[2])/dy;
    sxp = (sbx[1] - sbx[0])/sx;
    syp = (sbx[3] - sbx[2])/sy;

    kmnd = (ubx[0] - dbx[0])/dxp;
    kmxd = (ubx[1] - dbx[0])/dxp;
    lmnd = (ubx[2] - dbx[2])/dyp;
    lmxd = (ubx[3] - dbx[2])/dyp;

    kmns = (ubx[0] - sbx[0])/sxp;
    lmns = (ubx[2] - sbx[2])/syp;

    dk = kmxd - kmnd;
    dl = lmxd - lmnd;
    od = lmnd*dx + kmnd;
    os = lmns*sx + kmns;

    bpp = sim->bits_pixel >> 3;

    for (l = 0; l < dl; l++)
        {for (k = 0; k < dk; k++)
             {id = l*dx + k + od;
              is = l*sx + k + os;

	      if (bpp == 1)
		 {c = sp[is];
		  if (c != bck)
		     dp[id] = c;}
	      else
		 {for (ic = 0; ic < bpp; ic++)
		      dp[bpp*id+ic] = sp[bpp*is+ic];};};};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_INTP_BYTE - intepolate one single byte array into another
 *               - the length of each array is provided as well as
 *               - a stride for each array
 */

void _PG_intp_byte(unsigned char *op, unsigned char *np,
		   int ox, int nx, int os, int ns)
   {int i, io1, ax;
    double scale, si, ov1, ov2;

    scale = 0.999*((double) ((ox-os)*ns)) / ((double) ((nx-ns)*os) + SMALL);
    ax    = nx/ns;
    for (i = 0; i < ax; i++)
        {si  = i*scale;
         io1 = si;
         ov1 = op[io1*os];
         ov2 = op[(io1 + 1)*os];

         np[i*ns] = ov1*(io1 + 1 - si) + ov2*(si - io1) + 0.5;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

