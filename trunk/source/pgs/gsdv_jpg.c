/*
 * GSDV_JPG.C - PGS JPEG routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "pgs_int.h"

/*--------------------------------------------------------------------------*/

#ifndef HAVE_JPEGLIB

/*--------------------------------------------------------------------------*/
 
/* PG_SETUP_JPEG_DEVICE - stub for systems that don't have libjpeg.a
 *
 */

int PG_setup_jpeg_device(PG_device *d)
   {return(FALSE);}

/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/

#include "jpeglib.h"

/* to use the JPEG device issue a call
 *
 *   PG_register_device("JPEG", PG_setup_jpeg_device);
 *
 */

/*--------------------------------------------------------------------------*/

/*                            PAGE STATE ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _PG_JP_FINISH_PLOT - do what's necessary to finish up a graphical image
 *                    - and get the device updated with the image
 *                    - after this function nothing more can be added to
 *                    - the image
 */
 
static void _PG_JP_finish_plot(PG_device *dev)
   {int row_stride;
    long imagesize, i, j;
    unsigned char *input, *r, *g, *b;
    char fname[MAXLINE];
    FILE *fh;
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    frame *fr;
    PG_RAST_device *mdv;

    GET_RAST_DEVICE(dev, mdv);

    PG_make_raster_family_name(mdv, fname, MAXLINE);

    fh = SC_fopen_safe(fname, "wb");
    if (fh == NULL)
       return;

/* This function calls the Independent JPEG Group
 * (IJG) JPEG library to write the input image to
 * a file in JPEG format
 */

/* initialize jpeg library default error handler */
    cinfo.err = jpeg_std_error(&jerr);

/* initialize the jpeg library compression structure */
    jpeg_create_compress(&cinfo);

/* tell the jpeg library the destination (output file) */
    jpeg_stdio_dest(&cinfo, fh);

    cinfo.image_width      = mdv->width;
    cinfo.image_height     = mdv->height;
    cinfo.input_components = 3;       /* # color components */
    cinfo.in_color_space   = JCS_RGB; /* color space */

    jpeg_set_defaults(&cinfo);

/* set image quality if requested */
    if ((_PG_gattrs.jpg_quality > 0) && (_PG_gattrs.jpg_quality <= 100))
       jpeg_set_quality(&cinfo, _PG_gattrs.jpg_quality, TRUE);

/* begin a compression cycle */
    jpeg_start_compress(&cinfo, TRUE);

    row_stride = mdv->width * cinfo.input_components;
    fr         = mdv->raster;

    imagesize = mdv->width * mdv->height * cinfo.input_components;
   
    input = CMAKE_N(unsigned char, imagesize);

    GET_RGB(fr, r, g, b);

    for (i = 0, j=0; i < (mdv->width * mdv->height); i++, j+=3)
        {input[j]   = r[i];
         input[j+1] = g[i];
         input[j+2] = b[i];}

    while (cinfo.next_scanline < cinfo.image_height)
       {row_pointer[0] = &input[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);}

/* finish the compression cycle */
    jpeg_finish_compress(&cinfo);

/* close the output file */
    SC_fclose_safe(fh);

    CFREE(input);

/* release the compression object's subsidiary memory */
    jpeg_destroy_compress(&cinfo);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_JP_OPEN - initialize a JPEG device */
 
static PG_device *_PG_JP_open(PG_device *dev,
			      double xf, double yf, double dxf, double dyf)
   {int Lightest, Light, Light_Gray, Dark_Gray, Dark, Darkest;
    int n_colors;
    int dx[PG_SPACEDM];
    char fname[MAXLINE];
    double intensity;
    PG_RAST_device *mdv;
    PG_font_family *ff;
    PG_dev_geometry *g;

    if (dev == NULL)
       return(NULL);

    g = &dev->g;

    PG_setup_markers();

    dev->type_index       = JPEG_DEVICE;
    dev->quadrant         = QUAD_FOUR;
    dev->hard_copy_device = TRUE;
    dev->print_labels     = TRUE;

    _PG_rst_set_dev_prop(dev, (int) dxf, (int) dyf, N_RAST_COLOR);

    PG_device_filename(fname, dev->title, ".jpg");

    mdv = PG_make_raster_device((int) dxf, (int) dyf, fname,
				NULL, TRUE, NULL);
    if (mdv == NULL)
       return(NULL);

    dev->raster = mdv;

/* get the window shape in NDC */
    if ((xf == 0.0) && (yf == 0.0))
       {xf = 0.0;
        yf = 0.0;};

    if ((dxf == 0.0) && (dyf == 0.0))
       {dxf = 1.0;
        dyf = 1.0;};

    PG_query_screen_n(dev, dx, &n_colors);

/* set device pixel coordinate limits */
    g->cpc[0] = SHRT_MIN + dx[0];
    g->cpc[1] = SHRT_MAX - dx[0];
    g->cpc[2] = SHRT_MIN + dx[1];
    g->cpc[3] = SHRT_MAX - dx[1];
    g->cpc[4] = SHRT_MIN;
    g->cpc[5] = SHRT_MAX;

    g->hwin[0] = 0.0;
    g->hwin[1] = dx[0];
    g->hwin[2] = 0.0;
    g->hwin[3] = dx[0];

    SET_PC_FROM_HWIN(g);

    dev->window_x[0] = g->hwin[0];
    dev->window_x[2] = g->hwin[2];

    dev->ncolor = N_RAST_COLOR;
    dev->absolute_n_color = N_RAST_COLOR;
    intensity  = dev->max_intensity*MAXPIX;
    if (dev->background_color_white)
       {Color_Map(dev, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	Lightest   = 0;
	Light      = intensity;
	Light_Gray = 0.8*intensity;
	Dark_Gray  = 0.5*intensity;
	Dark       = 0;
	Darkest    = intensity;}

    else
       {Color_Map(dev, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        Lightest   = intensity;
        Light      = intensity;
	Light_Gray = 0.8*intensity;
	Dark_Gray  = 0.5*intensity;
        Dark       = 0;
        Darkest    = 0;};

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

    PG_fset_font(dev, "helvetica", "medium", 12);

/* put in the default palettes */
    PG_setup_standard_palettes(dev, 64,
			       Light, Dark,
			       Light_Gray, Dark_Gray,
			       Lightest, Darkest);

    SC_set_get_line(io_gets);
    SC_set_put_line(io_printf);
    SC_set_put_string(io_puts);

    return(dev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_JP_CLOSE_DEVICE - close a device */
 
static void _PG_JP_close_device(PG_device *dev)
   {PG_RAST_device *rdv;

    GET_RAST_DEVICE(dev, rdv);

    SC_ASSERT(rdv != NULL);

/* clean up the device */
    PG_rl_device(dev);

    return;}

/*--------------------------------------------------------------------------*/

/*                            PGS INTERFACE ROUTINES                        */

/*--------------------------------------------------------------------------*/
 
/* PG_SETUP_JPEG_DEVICE - do the device dependent device initialization
 *                      - for PG_make_device
 */

int PG_setup_jpeg_device(PG_device *d)
   {

    PG_setup_raster_device(d);

    d->type_index   = JPEG_DEVICE;
    d->close_device = _PG_JP_close_device;
    d->finish_plot  = _PG_JP_finish_plot;
    d->open_screen  = _PG_JP_open;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
