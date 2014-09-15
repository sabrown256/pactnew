/*
 * GSDV_RST.C - PGS raster device routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
  
#include "pgs_int.h"

#define _PG_RST_N_FONTS 1

static char
 _PG_rst_char_list[] = "!\"#$%&\'()*+,-./0123456789:;<=>\?@ABCDEFG\
HIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";


/* from the GSTX_X.C */

extern int
 _PG_rst_draw_text(PG_device *dev, char *s);


/* from the corresponding PR file */

extern int
 _PG_rst_set_font(PG_device *dev, char *face, char *style, int size);

extern void
 _PG_rst_draw_disjoint_polyline_2(PG_device *dev, double **r,
				  long n, int clip, int coord),
 _PG_rst_draw_curve(PG_device *dev, PG_curve *crv, int clip),
 _PG_rst_draw_to_abs(PG_device *dev, double x, double y),
 _PG_rst_draw_to_rel(PG_device *dev, double x, double y),
 _PG_rst_get_text_ext(PG_device *dev, int nd, PG_coord_sys cs, char *s, double *p),
 _PG_rst_get_image(PG_device *dev, unsigned char *bf,
		   int ix, int iy, int nx, int ny),
 _PG_rst_set_clipping(PG_device *dev, bool flag),
 _PG_rst_set_char_line(PG_device *dev, int n),
 _PG_rst_set_char_path(PG_device *dev, double x, double y),
 _PG_rst_set_char_precision(PG_device *dev, int p),
 _PG_rst_set_char_space(PG_device *dev, double s),
 _PG_rst_set_char_size(PG_device *dev, int nd, PG_coord_sys cs, double *x),
 _PG_rst_set_char_up(PG_device *dev, double x, double y),
 _PG_rst_set_fill_color(PG_device *dev, int color, int mapped),
 _PG_rst_set_line_color(PG_device *dev, int color, int mapped),
 _PG_rst_set_line_style(PG_device *dev, int style),
 _PG_rst_set_line_width(PG_device *dev, double width),
 _PG_rst_set_text_color(PG_device *dev, int color, int mapped),
 _PG_rst_shade_poly(PG_device *dev, int nd, int n, double **r),
 _PG_rst_fill_curve(PG_device *dev, PG_curve *crv),
 _PG_rst_move_gr_abs(PG_device *dev, double x, double y),
 _PG_rst_move_tx_abs(PG_device *dev, double x, double y),
 _PG_rst_move_tx_rel(PG_device *dev, double x, double y),
 _PG_rst_put_image(PG_device *dev, unsigned char *bf, int ix, int iy,
		   int nx, int ny);

/*--------------------------------------------------------------------------*/

/*                         GENERAL RASTER FUNCTIONS                         */

/*--------------------------------------------------------------------------*/

/* PG_MAKE_RASTER_DEVICE - make a raster device struct */

PG_RAST_device *PG_make_raster_device(int w, int h, char *name,
				      frame *infr, int rgb, FILE *fp)
   {PG_RAST_device *mdv;

    mdv = CMAKE(PG_RAST_device);

    mdv->nf         = 0;
    mdv->width      = w;
    mdv->height     = h;
    mdv->rgb_mode   = rgb;
    mdv->text_scale = 1.0;
    mdv->out_fname  = CSTRSAVE(name);
    mdv->fp         = fp;
    mdv->raster     = infr;

    return(mdv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FREE_RASTER_DEVICE - release a raster device struct */

void PG_free_raster_device(PG_RAST_device *mdv)
   {

    if (mdv != NULL)
       {PG_free_frame(mdv->raster);

        CFREE(mdv->out_fname);
        CFREE(mdv);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MAKE_RASTER_FAMILY_NAME - make a file name for raster devices which
 *                            - do one frame per file
 */

void PG_make_raster_family_name(PG_RAST_device *mdv, char *fname, int nc)
   {char t[MAXLINE];
    char *ext;

    if (mdv != NULL)
       {SC_strncpy(t, MAXLINE, mdv->out_fname, -1);
	ext    = strrchr(t, '.');
	if (ext != NULL)
	   *ext++ = '\0';

	snprintf(fname, nc, "%s.%03d.%s", t, mdv->nf++, ext);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CLEAR_RASTER_BOX - examine the pixel limits and possibly reset them
 *                      - for a clear region type operation
 *                      - the given limits are in NDC
 *                      - GOTCHA: do not know the right thing to do about Z
 */

static void _PG_clear_raster_box(PG_device *dev, frame *fr,
				 int nd, double *sbx, int force)
   {int i, m, nx, ny, clr, emp, non;
    int ix, iy;
    int ibx[PG_BOXSZ];
    double nbx[PG_BOXSZ];
    double *obx;
    PG_RAST_device *mdv;
    RGB_color_map c;

    mdv = NULL;
    if (dev != NULL)
       GET_RAST_DEVICE(dev, mdv);

    if ((fr == NULL) && (mdv != NULL))
       fr = mdv->raster;

    if (fr != NULL)
       {nx  = fr->width;
	ny  = fr->height;
	obx = fr->box;

	nbx[0] = nx*sbx[0];
	nbx[1] = nx*sbx[1];
	nbx[2] = ny*sbx[2];
	nbx[3] = ny*sbx[3];
	nbx[4] = sbx[4];
	nbx[5] = sbx[5];

	nd <<= 1;

/* see if specified limits contain the old limits */
	clr = TRUE;
	for (i = 0; i < nd; i += 2)
	    {clr &= (nbx[i] < obx[i]);
	     clr &= (obx[i+1] < nbx[i+1]);};

/* see if old limits specify non-empty box */
	emp = FALSE;
	for (i = 0; i < nd; i += 2)
	    emp |= (obx[i+1] <= obx[i]);

/* if specified limits contain the old limits then clear the limits */
	if ((clr == TRUE) || (emp == TRUE))
	   PG_box_init(3, obx, HUGE, -HUGE);

/* now clear the pixels */
	non = fr->n_on;
	if ((non > 0) || (force == TRUE))
	   {ibx[0] = ((obx[0] <= 0.0) || (obx[0] >= nx)) ? 0    : obx[0];
	    ibx[1] = ((obx[1] <= 0.0) || (obx[1] >= nx)) ? nx-1 : obx[1];
	    ibx[2] = ((obx[2] <= 0.0) || (obx[2] >= nx)) ? 0    : obx[2];
	    ibx[3] = ((obx[3] <= 0.0) || (obx[3] >= nx)) ? ny-1 : obx[3];

	    c = fr->bc;
	    for (iy = ibx[2]; iy <= ibx[3]; iy++)
	        {for (ix = ibx[0]; ix <= ibx[1]; ix++)
		     {m = iy*nx + ix;
		      fr->r[m]  = c.red;
		      fr->g[m]  = c.green;
		      fr->b[m]  = c.blue;
		      fr->zb[m] = -HUGE;
		      non--;};};

	    fr->n_on = max(non, 0);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CLEAR_RASTER_DEVICE - clear the raster device memory */

void PG_clear_raster_device(PG_device *dev)
   {double box[6];
    frame *fr;
    PG_RAST_device *mdv;

    GET_RAST_DEVICE(dev, mdv);

    if (mdv != NULL)
       {fr = mdv->raster;
	if (fr != NULL)
	   {PG_box_init(3, box, 0.0, 1.0);
	    _PG_clear_raster_box(dev, fr, 3, box, FALSE);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CLEAR_RASTER_REGION - clear the rectangular region specified in NDC */
 
void _PG_clear_raster_region(PG_device *dev, int nd, PG_coord_sys cs,
			     double *bx, int pad)
   {int id, l, h, w;
    double p;
    double box[6];
    PG_RAST_device *mdv;

    if (dev != NULL)
       {GET_RAST_DEVICE(dev, mdv);

	if (mdv != NULL)
	   {w = PG_window_width(dev);
	    h = PG_window_height(dev);

	    if ((0 < w) && (0 < h))
	       {p = ((double) pad)/((double) w);

		PG_box_init(3, box, HUGE, -HUGE);
		PG_box_copy(nd, box, bx);
		PG_trans_box(dev, nd, cs, box, NORMC, box);

		for (id = 0; id < nd; id++)
		    {l         = 2*id;
		     box[l]   -= p;
		     box[l+1] += p;};

		_PG_clear_raster_box(dev, NULL, 3, box, FALSE);};};};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_MAKE_FRAME - make a raster frame */

static frame *_PG_make_frame(PG_device *dev, int w, int h)
   {int np;
    double box[6];
    frame *fr;

    fr = CMAKE(frame);
    if (fr != NULL)
       {SC_MEM_INIT(frame, fr);

	np = w*h;

/* NOTE: we need np/4 for Cb, Cr but the frame will be computed as
 *       RGB and converted to YCbCr at the end so have all the
 *       spaces be np
 */
	fr->r = CMAKE_N(unsigned char, np);
	fr->g = CMAKE_N(unsigned char, np);
	fr->b = CMAKE_N(unsigned char, np);

	fr->width  = w;
	fr->height = h;
	fr->n_on   = 0;
	fr->zb     = NULL;

	_PG_frame_z_buffer(fr);

	PG_rgb_color(&fr->bc, dev->BLACK, TRUE, dev->palettes);

	PG_box_init(3, box, 0.0, 1.0);

	_PG_clear_raster_box(dev, fr, 3, box, TRUE);};

    return(fr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FREE_FRAME - release a raster frame */

void PG_free_frame(frame *fr)
   {

    if (fr == NULL)
       return;

    if (fr->zb != NULL)
       CFREE(fr->zb);

    if (fr->r != NULL)
       CFREE(fr->r);

    if (fr->g != NULL)
       CFREE(fr->g);

    if (fr->b != NULL)
       CFREE(fr->b);

    CFREE(fr);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_COPY_FRAME - make a copy of the frame data */

void _PG_copy_frame(frame *fr1, frame *fr2)
   {int n1, n2;

    n1 = fr1->width*fr1->height;
    n2 = n1/4;

    memcpy(fr1->r, fr2->r, n1);
    memcpy(fr1->g, fr2->g, n2);
    memcpy(fr1->b, fr2->b, n2);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GET_RASTER - return the raster device from DEV
 *                - creating it if need be
 */

PG_RAST_device *_PG_get_raster(PG_device *dev, int resz)
   {int nx, ny;
    frame *fr;
    PG_RAST_device *mdv;

    nx = PG_window_width(dev);
    ny = PG_window_height(dev);
    if ((POSTSCRIPT_DEVICE(dev) == TRUE) || (CGM_DEVICE(dev) == TRUE))
       {nx /= dev->resolution_scale_factor;
	ny /= dev->resolution_scale_factor;};

    GET_RAST_DEVICE(dev, mdv);

/* if we do not have a raster device now is the time to make one */
    if (mdv == NULL)
       {fr  = _PG_make_frame(dev, nx, ny);
	mdv = PG_make_raster_device(nx, ny, dev->title, fr, TRUE, NULL);
	SET_RAST_DEVICE(dev, mdv);};

    fr = mdv->raster;
    if (fr == NULL)
       {fr          = _PG_make_frame(dev, nx, ny);
	mdv->raster = fr;};

/* if the raster device is the wrong size remake it */
    if ((resz == TRUE) && ((fr->width != nx) || (fr->height != ny)))
       {PG_free_raster_device(mdv);
	fr  = _PG_make_frame(dev, nx, ny);
	mdv = PG_make_raster_device(nx, ny, dev->title, fr, TRUE, NULL);
	SET_RAST_DEVICE(dev, mdv);};

    return(mdv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FRAME_Z_BUFFER - setup the Z buffer for the frame */

double *_PG_frame_z_buffer(frame *fr)
   {int nx, ny, np;
    double *zb;

    zb = fr->zb;
    if (zb == NULL)
       {nx = fr->width;
	ny = fr->height;
	np = nx*ny;

        zb = CMAKE_N(double, np);
        PM_array_set(zb, np, -HUGE);

	fr->zb = zb;};

    return(zb);}

/*--------------------------------------------------------------------------*/

/*                        FONT MANAGEMENT ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* _PG_RST_INIT_FONTS - set up the fonts */

static int _PG_rst_init_fonts(PG_device *dev)
   {int i;

    if (_PG.rst_marker_fonts != NULL)
       return(TRUE);
    
    _PG.rst_marker_fonts = CMAKE_N(int *, _PG_RST_N_FONTS);
    if (_PG.rst_marker_fonts == NULL)
        return(FALSE);

    for (i = 0; i < _PG_RST_N_FONTS; i++)
        {_PG.rst_marker_fonts[i] = CMAKE_N(int, strlen(_PG_rst_char_list));
         if (_PG.rst_marker_fonts[i] == NULL)
             return(FALSE);}

/* set up the default (and for now, only) raster font */
    PG_def_stroke_font(_PG.rst_marker_fonts[0]);

    _PG.rst_current_font = _PG.rst_marker_fonts[0];

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RL_RST_FONTS - release the raster fonts */

void _PG_rl_rst_fonts(void)
   {int i;

    if (_PG.rst_marker_fonts != NULL)
       {for (i = 0; i < _PG_RST_N_FONTS; i++)
	    {CFREE(_PG.rst_marker_fonts[i]);};

	CFREE(_PG.rst_marker_fonts);

	_PG.rst_current_font = NULL;};

    return;}

/*--------------------------------------------------------------------------*/

/*                            PAGE STATE ROUTINES                           */

/*--------------------------------------------------------------------------*/
 
/* _PG_RST_CLEAR_WINDOW - clear the screen */
 
static void _PG_rst_clear_window(PG_device *dev)
   {int w, h;
    PG_RAST_device *mdv;

    GET_RAST_DEVICE(dev, mdv);
    if (mdv != NULL)
       {if (mdv->raster == NULL)
	   {w  = PG_window_width(dev);
	    h  = PG_window_height(dev);
	    mdv->raster = _PG_make_frame(dev, w, h);};

	PG_clear_raster_device(dev);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_RST_CLEAR_VIEWPORT - clear the viewport */
 
static void _PG_rst_clear_viewport(PG_device *dev)
   {PG_dev_geometry *g;

    g = &dev->g;

    PG_clear_region(dev, 2, NORMC, g->ndc, 1);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_RST_CLEAR_REGION - clear the rectangular region specified in CS */
 
static void _PG_rst_clear_region(PG_device *dev, int nd, PG_coord_sys cs,
				 double *bx, int pad)
   {

    _PG_clear_raster_region(dev, nd, cs, bx, pad);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_RST_UPDATE_VS - update the view surface for the given device */
 
static void _PG_rst_update_vs(PG_device *dev)
   {

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_RST_CLEAR_PAGE - clear the current page
 *                    - and go to the line i on the screen
 */
 
static void _PG_rst_clear_page(PG_device *dev, int i)
   {

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_EXPOSE_DEVICE - make this device the topmost one */
 
static void _PG_rst_expose_device(PG_device *dev)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_MAKE_DEVICE_CURRENT - make this device the current one for drawing
 *                             - purposes
 */
 
static void _PG_rst_make_device_current(PG_device *dev)
   {

    return;}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_RELEASE_CURRENT_DEVICE - make no device current */
 
static void _PG_rst_release_current_device(PG_device *dev)
   {

    return;}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_RST_IMAGE_TEXT - write out text to the appropriate device */
 
static int _PG_rst_image_text(PG_device *dev, FILE *fp, char *s)
   {int ok;

    ok = _PG_rst_draw_text(dev, s);

    return(ok);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_RST_STROKE_TEXT - write out text to the appropriate device */
 
static int _PG_rst_stroke_text(PG_device *dev, FILE *fp, char *s)
   {int i, ind, savlinclr, ncharin;
    double savlwd, lwd, dx;
    double x[PG_SPACEDM];
    double *r[PG_SPACEDM];
    char *target, *style;

    if (dev != NULL)
       {for (i = 0; i < 2; i++)
	    r[i] = &x[i];

	ncharin   = strlen(s);
	dx        = (dev->char_width_s + dev->char_space_s)*(dev->g.nd_w[1]);
	savlinclr = dev->line_color;
	PG_fset_line_color(dev, dev->text_color, TRUE);

	savlwd = PG_fget_line_width(dev);
	style  = dev->type_style;

/* added capability of limited type styles */
	if (!strcmp(style, "bold") || !strcmp(style, "bold-italic"))
	   lwd = 0.5;
	else
	   lwd = _PG_gattrs.line_width;
	PG_fset_line_width(dev, lwd);

	for (i = 0; i < ncharin; i++)
	    {target = strchr(_PG_rst_char_list, (int)s[i]);
	     if (target != NULL)
	        {ind = target - _PG_rst_char_list;
		 x[0] = dev->tcur[0];
		 x[1] = dev->tcur[1];

		 PG_lin_point(dev, 2, x);
		 PG_draw_markers_n(dev, 2, WORLDC, 1, r,
				   _PG.rst_current_font[ind]);} 

/* need to calculate new x and y positions based upon character size
 * and path.  For now, just for testing, wing it
 */
             dev->tcur[0] += dx;};

	PG_fset_line_color(dev, savlinclr, TRUE);
	PG_fset_line_width(dev, savlwd);};

    return(TRUE);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_RST_WRITE_TEXT - write out text to the appropriate device */
 
static void _PG_rst_write_text(PG_device *dev, FILE *fp, char *s)
   {int ok;

    if (_PG_gattrs.stroke_raster_text == TRUE)
       ok = _PG_rst_stroke_text(dev, fp, s);

    else
       {ok = _PG_rst_image_text(dev, fp, s);
	if (ok == 0)
	   {ok = _PG_rst_stroke_text(dev, fp, s);
	    _PG_gattrs.stroke_raster_text = TRUE;};};
       
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_RST_NEXT_LINE - do a controlled newline */
 
static void _PG_rst_next_line(PG_device *dev, FILE *fp)
   {

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_QUERY_POINTER - query the pointer for location and button status */

static void _PG_rst_query_pointer(PG_device *dev, int *ir, int *pb, int *pq)
   {

    return;}
 
/*--------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_RST_GRAB_POINTER - take over the pointer */

static int _PG_rst_grab_pointer(PG_device *dev)
   {int rv;

    rv = FALSE;

    return(rv);}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_RST_RELEASE_POINTER - release the pointer */

static int _PG_rst_release_pointer(PG_device *dev)
   {int rv;

    rv = FALSE;

    return(rv);}

/*------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_SET_DEV_PROP - set some RASTER device characteristics */

void _PG_rst_set_dev_prop(PG_device *dev, int dx, int dy, int nc)
   {PG_dev_geometry *g;

    g = &dev->g;

    g->hwin[1] = g->hwin[0] + dx;
    g->hwin[3] = g->hwin[2] + dy;

    SET_PC_FROM_HWIN(g);

    dev->absolute_n_color = nc;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_QUERY - query some MPEG device characteristics */

static void _PG_rst_query(PG_device *dev, int *pdx, int *pdy, int *pnc)
   {int id, dx[PG_SPACEDM], nc;
    PG_dev_geometry *g;

    g = &dev->g;

    dx[0] = PG_window_width(dev);
    dx[1] = PG_window_height(dev);

    nc = dev->absolute_n_color;

    for (id = 0; id < 2; id++)
        g->phys_dx[id] = dx[id];

    dev->phys_n_colors = nc;

    *pdx = dx[0];
    *pdy = dx[1];
    *pnc = nc;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_MAP_TO_COLOR_TABLE - map the PGS palette to host
 *                            - color table values
 */

static void _PG_rst_map_to_color_table(PG_device *dev, PG_palette *pal)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_SETUP_RASTER_DEVICE - do the device dependent device initialization
 *                        - for PG_make_device
 */

int PG_setup_raster_device(PG_device *d)
   {

    d->type_index              = RASTER_DEVICE;

    d->events_pending          = NULL;
    d->query_pointer           = _PG_rst_query_pointer;
    d->grab_pointer            = _PG_rst_grab_pointer;
    d->release_pointer         = _PG_rst_release_pointer;
    d->mouse_event_info        = NULL;
    d->key_event_info          = NULL;
    d->clear_page              = _PG_rst_clear_page;
    d->clear_window            = _PG_rst_clear_window;
    d->clear_viewport          = _PG_rst_clear_viewport;
    d->clear_region            = _PG_rst_clear_region;
    d->close_device            = NULL;  /* device specific */
    d->flush_events            = NULL;
    d->draw_dj_polyln_2        = _PG_rst_draw_disjoint_polyline_2;
    d->draw_curve              = _PG_rst_draw_curve;
    d->draw_to_abs             = _PG_rst_draw_to_abs;
    d->draw_to_rel             = _PG_rst_draw_to_rel;
    d->expose_device           = _PG_rst_expose_device;
    d->finish_plot             = NULL;  /* device specific */
    d->get_char                = NULL;
    d->get_image               = _PG_rst_get_image;
    d->get_text_ext            = _PG_rst_get_text_ext;
    d->ggetc                   = io_getc;
    d->ggets                   = io_gets;
    d->gputs                   = NULL;
    d->make_device_current     = _PG_rst_make_device_current;
    d->map_to_color_table      = _PG_rst_map_to_color_table;
    d->match_rgb_colors        = _PG_match_rgb_colors;
    d->move_gr_abs             = _PG_rst_move_gr_abs;
    d->move_tx_abs             = _PG_rst_move_tx_abs;
    d->move_tx_rel             = _PG_rst_move_tx_rel;
    d->next_line               = _PG_rst_next_line;
    d->open_screen             = NULL;   /* device specific */
    d->put_image               = _PG_rst_put_image;
    d->query_screen            = _PG_rst_query;
    d->release_current_device  = _PG_rst_release_current_device;
    d->resolution_scale_factor = 1;
    d->set_clipping            = _PG_rst_set_clipping;
    d->set_char_line           = _PG_rst_set_char_line;
    d->set_char_path           = _PG_rst_set_char_path;
    d->set_char_precision      = _PG_rst_set_char_precision;
    d->set_char_size           = _PG_rst_set_char_size;
    d->set_char_space          = _PG_rst_set_char_space;
    d->set_char_up             = _PG_rst_set_char_up;
    d->set_fill_color          = _PG_rst_set_fill_color;
    d->set_font                = _PG_rst_set_font;
    d->set_line_color          = _PG_rst_set_line_color;
    d->set_line_style          = _PG_rst_set_line_style;
    d->set_line_width          = _PG_rst_set_line_width;
    d->set_text_color          = _PG_rst_set_text_color;
    d->shade_poly              = _PG_rst_shade_poly;
    d->fill_curve              = _PG_rst_fill_curve;
    d->update_vs               = _PG_rst_update_vs;
    d->write_text              = _PG_rst_write_text;

    _PG_rst_init_fonts(d);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
  
