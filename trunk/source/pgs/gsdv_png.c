/*
 * GSDV_PNG.C - PGS Portable Network Graphics (PNG) routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "scconfig.h"

int
 PG_png_quality = 100;

/*--------------------------------------------------------------------------*/

#ifndef HAVE_PNGLIB

#include "pgs_int.h"

/*--------------------------------------------------------------------------*/
 
/* PG_SETUP_PNG_DEVICE - stub for systems that don't have libpng.a
 *
 */

int PG_setup_png_device(PG_device *d)
   {return(FALSE);}

/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/

/* GOTCHA: png.h has some faulty logic with respect to setjmp.h that requires
 * it to be included ahead of pgs_int.h
 * this then necessitates having the '#include pgs_int.h' appear twice in this
 * file
 */

#ifdef LINUX
# include <png.h>
# include "pgs_int.h"
#else
# include "pgs_int.h"
# include <png.h>
#endif

/* to use the PNG device issue a call
 *
 *   PG_register_device("PNG", PG_setup_png_device);
 *
 */

/*--------------------------------------------------------------------------*/

/*                            PAGE STATE ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _PG_PNG_FINISH_PLOT - do what's necessary to finish up a graphical image
 *                     - and get the device updated with the image
 *                     - after this function nothing more can be added to
 *                     - the image
 *                     - this function calls the PNG library to write the
 *                     - input image to a file in PNG format
 */
 
static void _PG_PNG_finish_plot(PG_device *dev)
   {int i, j, width, height, bpp, bitpp;
    unsigned char *row, *pr;
    unsigned char *r, *g, *b;
    char fname[MAXLINE];
    FILE *fh;
    png_structp pw;
    png_infop pi;
    png_textp tp;
    png_color_8 pnc;
    PG_RAST_device *mdv;

    GET_RAST_DEVICE(dev, mdv);

    PG_make_raster_family_name(mdv, fname, MAXLINE);

    fh = fopen(fname, "wb");
    if (fh == NULL)
       return;

    width  = mdv->width;
    height = mdv->height;

    r = mdv->raster->r;
    g = mdv->raster->g;
    b = mdv->raster->b;

    bitpp = 24;
    bpp   = bitpp/3;

    pw = png_create_write_struct(PNG_LIBPNG_VER_STRING,
				 (png_voidp) NULL, NULL, NULL);
    if (pw == NULL)
       return;

    pi = png_create_info_struct(pw);
    if (pi == NULL)
       {png_destroy_write_struct(&pw, (png_infopp) NULL);
        return;};

#ifdef PNG_SETJMP_SUPPORTED
    if (setjmp(png_jmpbuf(pw)))
       {png_destroy_write_struct(&pw, &pi);
        fclose(fh);
	return;};
#endif

    png_init_io(pw, fh);

    png_set_IHDR(pw, pi, width, height, bpp,
		 PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		 PNG_COMPRESSION_TYPE_DEFAULT,
		 PNG_FILTER_TYPE_DEFAULT);

    png_set_oFFs(pw, pi, 0, 0, PNG_OFFSET_PIXEL);
    png_set_sRGB(pw, pi, PNG_sRGB_INTENT_ABSOLUTE);

    pnc.red   = bpp;
    pnc.green = bpp;
    pnc.blue  = bpp;
    png_set_sBIT(pw, pi, &pnc);

/* setup text */
    tp = CMAKE_N(png_text, 1);

    i = 0;
    tp[i].key         = "author";
    tp[i].text        = "PACT PGS";
    tp[i].compression = PNG_TEXT_COMPRESSION_NONE;
    tp[i].text_length = strlen("PACT PGS");
    i++;

    png_set_text(pw, pi, tp, i);

    png_write_info(pw, pi);

    row = CMAKE_N(unsigned char, 3*width);

    for (i = 0; i < height; i++)
        {pr = row;
	 for (j = 0; j < width; j++)
	     {*pr++ = *r++;
	      *pr++ = *g++;
	      *pr++ = *b++;};
	 png_write_row(pw, row);};

    CFREE(row);
    CFREE(tp);

    png_write_end(pw, pi);

    png_destroy_write_struct(&pw, &pi);

    fclose(fh);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PNG_OPEN - initialize a PNG device */
 
static PG_device *_PG_PNG_open(PG_device *dev,
			       double xf, double yf, double dxf, double dyf)
   {int display_width, display_height, n_colors;
    int Lightest, Light, Light_Gray, Dark_Gray, Dark, Darkest;
    char fname[MAXLINE];
    double intensity;
    PG_RAST_device *mdv;
    PG_font_family *ff;
    PG_dev_geometry *g;

    if (dev == NULL)
       return(NULL);

    g = &dev->g;

    PG_setup_markers();

    dev->type_index       = PNG_DEVICE;
    dev->quadrant         = QUAD_FOUR;
    dev->hard_copy_device = TRUE;
    dev->print_labels     = TRUE;

    _PG_rst_set_dev_prop(dev, (int) dxf, (int) dyf, N_RAST_COLOR);

    PG_device_filename(fname, dev->title, ".png");

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

    dev->ncolor           = N_RAST_COLOR;
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

    PG_set_font(dev, "helvetica", "medium", 12);

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

/* _PG_PNG_CLOSE_DEVICE - close a device */
 
static void _PG_PNG_close_device(PG_device *dev)
   {PG_RAST_device *rdv;

    GET_RAST_DEVICE(dev, rdv);

    SC_ASSERT(rdv != NULL);

/* clean up the device */
    PG_rl_device(dev);

    return;}

/*--------------------------------------------------------------------------*/

/*                            PGS INTERFACE ROUTINES                        */

/*--------------------------------------------------------------------------*/
 
/* PG_SETUP_PNG_DEVICE - do the device dependent device initialization
 *                     - for PG_make_device
 */

int PG_setup_png_device(PG_device *d)
   {

    PG_setup_raster_device(d);

    d->type_index   = PNG_DEVICE;
    d->close_device = _PG_PNG_close_device;
    d->finish_plot  = _PG_PNG_finish_plot;
    d->open_screen  = _PG_PNG_open;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
