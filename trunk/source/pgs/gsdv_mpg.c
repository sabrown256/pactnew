/*
 * GSDV_MPG.C - PGS MPEG device routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "pgs_int.h"

typedef int boolean;

extern void
 save_image2file(PG_device *dev),
 close_MPEGjob(PG_device *dev);

extern boolean
 init_MPEGjob(PG_device *dev);

#if !defined(WIN_32)
# define HAVE_MPEG
#endif

/* to use the MPEG device issue a call
 *
 *   PG_register_device("MPEG", PG_setup_mpeg_device);
 *
 */

/*--------------------------------------------------------------------------*/

#ifndef HAVE_MPEG

/*--------------------------------------------------------------------------*/
 
/* PG_SETUP_MPEG_DEVICE - stub for systems that don't have MPEG
 *
 */

int PG_setup_mpeg_device(PG_device *d)
   {return(FALSE);}

/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/

/*                            PAGE STATE ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _PG_MP_FINISH_PLOT - do what's necessary to finish up a graphical image
 *                    - and get the device updated with the image
 *                    - after this function nothing more can be added to
 *                    - the image
 */
 
static void _PG_MP_finish_plot(PG_device *dev)
   {     
    PG_make_device_current(dev);

    save_image2file(dev);

    PG_update_vs(dev);

    PG_release_current_device(dev);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_MP_OPEN - initialize a MPEG device */
 
static PG_device *_PG_MP_open(PG_device *dev,
			      double xf, double yf, double dxf, double dyf)
   {int display_width, display_height, n_colors;
    int Lightest, Light, Light_Gray, Dark_Gray, Dark, Darkest;
    char fname[MAXLINE];
    double intensity;
    PG_RAST_device *mdv;
    PG_font_family *ff;
    PG_dev_geometry *g;
    FILE *fh;

    if (dev == NULL)
       return(NULL);

    g = &dev->g;

    PG_setup_markers();

    dev->type_index       = MPEG_DEVICE;
    dev->quadrant         = QUAD_FOUR;
    dev->hard_copy_device = TRUE;
    dev->print_labels     = TRUE;

    _PG_rst_set_dev_prop(dev, (int) dxf, (int) dyf, N_RAST_COLOR);

    PG_device_filename(fname, dev->title, ".mpg");

    mdv = PG_make_raster_device((int) dxf, (int) dyf, fname,
				NULL, TRUE, NULL);
    if (mdv == NULL)
       return(NULL);

/* create an MPEG file */
    fh = SC_fopen(fname, "wb");
    if (fh == NULL)
       return(NULL);

    mdv->fp     = fh;
    dev->raster = mdv;

/* get the window shape in NDC */
    if ((xf == 0.0) && (yf == 0.0))
       {xf = 0.0;
        yf = 0.0;};

    if ((dxf == 0.0) && (dyf == 0.0))
       {dxf = 1.0;
        dyf = 1.0;};

    PG_query_screen(dev, &display_width, &display_height, &n_colors);

/* set device pixel coordinate limits */
    g->cpc[0] = SHRT_MIN + display_width;
    g->cpc[1] = SHRT_MAX - display_width;
    g->cpc[2] = SHRT_MIN + display_height;
    g->cpc[3] = SHRT_MAX - display_height;
    g->cpc[4] = SHRT_MIN;
    g->cpc[5] = SHRT_MAX;
 
    g->hwin[0] = 0.0;
    g->hwin[1] = display_width;
    g->hwin[2] = 0.0;
    g->hwin[3] = display_width;

    SET_PC_FROM_HWIN(g);

    dev->window_x[0] = g->hwin[0];
    dev->window_x[2] = g->hwin[2];

    Color_Map(dev, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

    dev->ncolor = N_RAST_COLOR;
    dev->absolute_n_color = N_RAST_COLOR;

/* compute the view port */
    _PG_default_viewspace(dev, FALSE);

    PG_init_viewspace(dev, TRUE);
 
/* initialize fonts */
    ff = PG_make_font_family(dev, "helvetica", NULL, 4,
                             "Helvetica",
                             "Helvetica-Oblique",
                             "Helvetica-Bold",
                             "Helvetica-BoldOblique");

    ff = PG_make_font_family(dev, "times", ff, 4,
                             "Times-Roman",
                             "Times-Italic",
                             "Times-Bold",
                             "Times-BoldItalic");

    ff = PG_make_font_family(dev, "courier", ff, 4,
                             "Courier",
                             "Courier-Oblique",
                             "Courier-Bold",
                             "Courier-BoldOblique");

    dev->font_family = ff;

    PG_set_font(dev, "helvetica", "medium", 12);

/* put in the default palettes */
    intensity  = dev->max_intensity*MAXPIX;
    Lightest   = 0;
    Light      = intensity;
    Light_Gray = 0.8*intensity;
    Dark_Gray  = 0.5*intensity;
    Dark       = 0;
    Darkest    = intensity;

    PG_setup_standard_palettes(dev, 64,
			       Light, Dark,
			       Light_Gray, Dark_Gray,
			       Lightest, Darkest);

    SC_set_get_line(io_gets);
    SC_set_put_line(io_printf);
    SC_set_put_string(io_puts);

/* initialization of starting necessary MPEG job */
    if (!init_MPEGjob(dev))
       return(NULL);

    return(dev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_MP_CLOSE_DEVICE - close a device */
 
static void _PG_MP_close_device(PG_device *dev)
   {PG_RAST_device *mdv;

    GET_RAST_DEVICE(dev, mdv);

/* close the MPEG stuff */
    if (mdv != NULL)
       {close_MPEGjob(dev);
	io_close(mdv->fp);};

/* clean up the device */
    PG_rl_device(dev);

    return;}

/*--------------------------------------------------------------------------*/

/*                            PGS INTERFACE ROUTINES                        */

/*--------------------------------------------------------------------------*/
 
/* PG_SETUP_MPEG_DEVICE - do the device dependent device initialization
 *                      - for PG_make_device
 */

int PG_setup_mpeg_device(PG_device *d)
   {

    PG_setup_raster_device(d);

    d->type_index   = MPEG_DEVICE;
    d->close_device = _PG_MP_close_device;
    d->finish_plot  = _PG_MP_finish_plot;
    d->open_screen  = _PG_MP_open;

    return(TRUE);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

