/*
 * GSDV_CGM.C - PGS CGM routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
  
#include "pgs_int.h"

/* make CGMs which conform to DOD CALS spec (MIL-D-28003A) */
#define DOD_CALS

/* maximum length of a CELL ARRAY partition */
#define MAX_PARTITION 0x7FFC

SC_array
 *_PG_CGM_x_point_list,
 *_PG_CGM_y_point_list;

/* from the corresponding PR file */

int
 _PG_CGM_set_font(PG_device *dev, char *face, char *style, int size);

void
 _PG_CGM_draw_disjoint_polyline_2(PG_device *dev, double **r,
				  long n, int clip, int coord),
 _PG_CGM_draw_curve(PG_device *dev, PG_curve *crv, int clip),
 _PG_CGM_draw_to_abs(PG_device *dev, double x, double y),
 _PG_CGM_draw_to_rel(PG_device *dev, double x, double y),
 _PG_CGM_get_text_ext(PG_device *dev, int nd, PG_coord_sys cs, char *s, double *p),
 _PG_CGM_set_clipping(PG_device *dev, bool flag),
 _PG_CGM_set_char_line(PG_device *dev, int n),
 _PG_CGM_set_char_path(PG_device *dev, double x, double y),
 _PG_CGM_set_char_precision(PG_device *dev, int p),
 _PG_CGM_set_char_space(PG_device *dev, double s),
 _PG_CGM_set_char_size(PG_device *dev, int nd, PG_coord_sys cs, double *x),
 _PG_CGM_set_char_up(PG_device *dev, double x, double y),
 _PG_CGM_set_fill_color(PG_device *dev, int color, int mapped),
 _PG_CGM_set_line_color(PG_device *dev, int color, int mapped),
 _PG_CGM_set_line_style(PG_device *dev, int style),
 _PG_CGM_set_line_width(PG_device *dev, double width),
 _PG_CGM_set_logical_op(PG_device *dev, PG_logical_operation lop),
 _PG_CGM_set_text_color(PG_device *dev, int color, int mapped),
 _PG_CGM_shade_poly(PG_device *dev, int nd, int n, double **r),
 _PG_CGM_fill_curve(PG_device *dev, PG_curve *crv),
 _PG_CGM_move_gr_abs(PG_device *dev, double x, double y),
 _PG_CGM_move_tx_abs(PG_device *dev, double x, double y),
 _PG_CGM_move_tx_rel(PG_device *dev, double x, double y),
 _PG_CGM_put_image(PG_device *dev, unsigned char *bf, int ix, int iy,
		   int nx, int ny);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CGM_DEFAULTS - set the page defaults for the CGM */

static void PG_CGM_defaults(PG_device *dev)
   {int nbytes;

    nbytes = 4;
    PG_CGM_command(dev, METAFILE_DEFAULTS_REPLACEMENT(nbytes));

/* use stroked text */
    PG_CGM_command(dev, TEXT_PRECISION, 2);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CGM_QUERY - query some CGM device characteristics */

static void _PG_CGM_query(PG_device *dev,
			  int *pdx, int *pdy, int *pnc)
   {int dx, dy, nc;

    dx = 32767;
    dy = 32767;

    if (strcmp(dev->type, "MONOCHROME") == 0)
       nc = 16;
    else
       nc = 256;
        
    dev->g.phys_width  = dx;
    dev->g.phys_height = dy;
    dev->phys_n_colors = nc;

    *pdx = dx;
    *pdy = dy;
    *pnc = nc;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_OPEN - initialize a Computer Graphics Metafile device */
 
static PG_device *_PG_CGM_open(PG_device *dev,
			       double xf, double yf, double dxf, double dyf)
   {int i, h, w, nt, nfonts, mono;
    int Lightest, Light, Light_Gray, Dark_Gray, Dark, Darkest;
    int display_width, display_height, n_colors, lst[20];
    double intensity;
    char **font_name, **fs, *token, *name, *date, *s;
    char lname[MAXLINE], fname[MAXLINE], description[MAXLINE];
    FILE *cgm_fp;
    PG_font_family *ff;
    PG_dev_geometry *g;

    if (dev == NULL)
       return(NULL);

    g = &dev->g;

    PG_setup_markers();

    dev->type_index       = CGMF_DEVICE;
    dev->hard_copy_device = TRUE;
    dev->print_labels     = TRUE;

/* the default CGM uses integer coordinates in the 1rst quadrant */
    dev->quadrant = QUAD_ONE;

/* get the window shape in NDC */
    if ((xf == 0.0) && (yf == 0.0))
       {xf = 0.1;
        yf = 0.0;};

    if ((dxf == 0.0) && (dyf == 0.0))
       {dxf = 0.8;
        dyf = 0.8;};

    _PG_CGM_query(dev, &display_width, &display_height, &n_colors);

    _PG_CGM_x_point_list = CMAKE_ARRAY(double, NULL, 0);
    _PG_CGM_y_point_list = CMAKE_ARRAY(double, NULL, 0);

    w = display_width*dxf;
    h = display_width*dyf;

/* GOTCHA: the following pixel coordinate limits may be incorrect
 * set device pixel coordinate limits
 */
    g->cpc[0] = INT_MIN + display_width;
    g->cpc[1] = INT_MAX - display_width;
    g->cpc[2] = INT_MIN + display_height;
    g->cpc[3] = INT_MAX - display_height;
    g->cpc[4] = INT_MIN;
    g->cpc[5] = INT_MAX;

    g->hwin[0] = display_width*xf;
    g->hwin[1] = g->hwin[0] + w;
    g->hwin[2] = display_height*(1.0 - yf);
    g->hwin[3] = g->hwin[2] + h;

    SET_PC_FROM_HWIN(g);

    dev->window_x[0] = g->hwin[0];
    dev->window_x[2] = g->hwin[2] - h;

    SC_strncpy(lname, MAXLINE, dev->title, MAXLINE);
    name = SC_strtok(lname, " \t\n\r", s);
    if (name != NULL)
       SC_strncpy(fname, MAXLINE, name, MAXLINE);
    else
       SC_strncpy(fname, MAXLINE, "default", MAXLINE);

    token = SC_strtok(NULL, " \t\n\r", s);
    if (token != NULL)
       _PG_gattrs.cgm_text_mag = SC_stof(token);

    PG_device_filename(fname, MAXLINE, dev->title, ".cgm");

    cgm_fp = _PG_fopen(fname, "wb");
    if (cgm_fp == NULL)
       return(NULL);

    dev->file = cgm_fp;

/* write out standard CGM prolog */
    PG_CGM_command(dev, BEGIN_METAFILE, fname);
    PG_CGM_command(dev, METAFILE_VERSION, 1);

    date = SC_date();

#ifdef DOD_CALS

    snprintf(description, MAXLINE,
	     "Creator: PACT PGS ; Title: %s plots ; CreationDate: %s ; MIL-D-28003A/BASIC-1",
	     dev->title,
	     date);
#else

    snprintf(description, MAXLINE,
	     "Creator: PACT PGS ; Title: %s plots ; CreationDate: %s",
	     dev->title,
	     date);
#endif

    CFREE(date);

    PG_CGM_command(dev, METAFILE_DESCRIPTION, description);

    PG_CGM_command(dev, MAXIMUM_COLOUR_INDEX, 255);
    lst[0] =  1;
    lst[1] = -1;
    lst[2] =  1;
    PG_CGM_command(dev, METAFILE_ELEMENT_LIST, lst);
    PG_CGM_defaults(dev);

/* decide on the overall color layout */
    intensity = dev->max_intensity*MAXPIX;
    mono      = (strcmp(dev->type, "MONOCHROME") == 0);
    PG_color_map(dev, mono, TRUE, BLACK, WHITE);
    if (dev->background_color_white == TRUE)
       {Lightest   = 0;
        Light      = intensity;
        Light_Gray = 0.8*intensity;
        Dark_Gray  = 0.5*intensity;
        Dark       = 0;
        Darkest    = intensity;}
    else
       {Lightest   = intensity;
        Light      = intensity;
        Light_Gray = 0.8*intensity;
        Dark_Gray  = 0.5*intensity;
        Dark       = 0;
        Darkest    = 0;};

    dev->ncolor = n_colors;
    dev->absolute_n_color = 256;

/* compute the view port */
    _PG_default_viewspace(dev, FALSE);

    PG_init_viewspace(dev, TRUE);
 
/* set up fonts */

#ifdef DOD_CALS

    ff = PG_make_font_family(dev, "helvetica", NULL, 4,
                             "HELVETICA",
                             "HELVETICA_OBLIQUE",
                             "HELVETICA_BOLD",
                             "HELVETICA_BOLD_OBLIQUE");

    ff = PG_make_font_family(dev, "times", ff, 4,
                             "TIMES_ROMAN",
                             "TIMES_ITALIC",
                             "TIMES_BOLD",
                             "TIMES_BOLD_ITALIC");

    ff = PG_make_font_family(dev, "courier", ff, 4,
                             "COURIER",
                             "COURIER_OBLIQUE",
                             "COURIER_BOLD",
                             "COURIER_BOLD_OBLIQUE");

#else

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
#endif

    dev->font_family = ff;

/* initialize font list */
    nfonts = 0;
    for (ff = dev->font_family; ff != NULL; ff = ff->next)
        nfonts += ff->n_styles;

    font_name = CMAKE_N(char *, nfonts);
    for (nfonts = 0, ff = dev->font_family; ff != NULL; ff = ff->next)
        {nt = ff->n_styles;
         fs = ff->type_styles;
         for (i = 0; i < nt; i++)
             font_name[nfonts++] = fs[i];};

    PG_CGM_command(dev, FONT_LIST(nfonts), font_name);

    CFREE(font_name);

    PG_fset_font(dev, "helvetica", "medium", 12);

/* put in the default palettes */
    PG_setup_standard_palettes(dev, 64,
			       Light, Dark,
			       Light_Gray, Dark_Gray,
			       Lightest, Darkest);

    return(dev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CGM_MAP_TO_COLOR_TABLE - map the PGS palette to host
 *                            - color table values
 */

static void _PG_CGM_map_to_color_table(PG_device *dev, PG_palette *pal)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_CLOSE_DEVICE - close a device */
 
static void _PG_CGM_close_device(PG_device *dev)
   {

    if (dev->file != NULL)
       {if (dev->cgm_page_set)
	   PG_CGM_command(dev, END_PICTURE);

        PG_CGM_command(dev, END_METAFILE);
	io_close(dev->file);};

/* clean up the device */
    PG_rl_device(dev);

    SC_free_array(_PG_CGM_x_point_list, NULL);
    SC_free_array(_PG_CGM_y_point_list, NULL);

    return;}

/*--------------------------------------------------------------------------*/

/*                            PAGE STATE ROUTINES                           */

/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_CLEAR_WINDOW - clear the screen */
 
static void _PG_CGM_clear_window(PG_device *dev)
   {int ih, h, fc, al[2], pts[10];
    double ca[2];
    char s[MAXLINE];
    PG_palette *cp;

    if (dev == NULL)
       return;

    PG_make_device_current(dev);

    snprintf(s, MAXLINE, "Page %d", ++_PG.npage);
    PG_CGM_command(dev, BEGIN_PICTURE, s);

/* set line width specification mode */
    PG_CGM_command(dev, LINE_WIDTH_SPECIFICATION_MODE, 0);

    PG_CGM_command(dev, BEGIN_PICTURE_BODY);
    
    cp = dev->current_palette;

/* set the palette
 * due to a GPLOT bug this is the one exceptional case in the CGM
 * command handling scheme
 * the offset will be specially trapped when COLOUR TABLE is parsed
 */
    PG_CGM_command(dev, COLOUR_TABLE(cp->n_pal_colors), cp, 0);

/* start with clipping off */
    PG_CGM_command(dev, CLIP_INDICATOR, FALSE);

    h  = PG_window_height(dev);
    ih = _PG_gattrs.cgm_text_mag*dev->char_height_s*h;
    PG_CGM_command(dev, CHARACTER_HEIGHT, ih);

    al[0] = 1;             /* left */
    al[1] = 4;             /* base */
    ca[0] = 0.0;           /* continuous horizontal */
    ca[1] = 0.0;           /* continuous vertical */
    PG_CGM_command(dev, TEXT_ALIGNMENT(al, ca));

    dev->cgm_page_set = TRUE;

/* solid fill */
    PG_CGM_command(dev, INTERIOR_STYLE, 1);

    fc = dev->fill_color;
    if (dev->current_palette != dev->palettes)
       {PG_fset_fill_color(dev, 9, FALSE);}
    else
       {PG_fset_fill_color(dev, dev->BLACK, FALSE);};

    pts[0] = 0;
    pts[1] = 0;
    pts[2] = 32767;
    pts[3] = 0;
    pts[4] = 32767;
    pts[5] = 32767;
    pts[6] = 0;
    pts[7] = 32767;
    pts[8] = 0;
    pts[9] = 0;
    PG_CGM_command(dev, POLYGON(10), pts);
    PG_fset_fill_color(dev, fc, FALSE);

    PG_fset_text_color(dev, dev->WHITE, TRUE);
    PG_fset_line_color(dev, dev->WHITE, TRUE);

    PG_release_current_device(dev);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_CLEAR_VIEWPORT - clear the viewport */
 
static void _PG_CGM_clear_viewport(PG_device *dev)
   {

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_CLEAR_REGION - clear the rectangular region specified in NDC */
 
static void _PG_CGM_clear_region(PG_device *dev, int nd, PG_coord_sys cs,
				 double *bx, int pad)
   {

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_UPDATE_VS - update the view surface for the given device */
 
static void _PG_CGM_update_vs(PG_device *dev)
   {

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_FINISH_PLOT - do what's necessary to finish up a graphical image
 *                     - and get the device updated with the image
 *                     - after this function nothing more can be added to
 *                     - the image
 */
 
static void _PG_CGM_finish_plot(PG_device *dev)
   {int64_t addr;

    if (dev == NULL)
       return;

    PG_make_device_current(dev);

    PG_update_vs(dev);

    PG_CGM_command(dev, END_PICTURE);
    addr = io_tell(dev->file);

    dev->cgm_page_set = FALSE;
    
    PG_CGM_command(dev, END_METAFILE);
    io_seek(dev->file, addr, SEEK_SET);

    PG_release_current_device(dev);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_CLEAR_PAGE - clear the current page
 *                    - and go to the line i on the screen
 */
 
static void _PG_CGM_clear_page(PG_device *dev, int i)
   {

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CGM_EXPOSE_DEVICE - make this device the topmost one */
 
static void _PG_CGM_expose_device(PG_device *dev)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CGM_MAKE_DEVICE_CURRENT - make this device the current one for drawing
 *                             - purposes
 */
 
static void _PG_CGM_make_device_current(PG_device *dev)
   {

    return;}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CGM_RELEASE_CURRENT_DEVICE - make no device current */
 
static void _PG_CGM_release_current_device(PG_device *dev)
   {

    return;}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_WRITE_TEXT - write out text to the appropriate device */
 
static void _PG_CGM_write_text(PG_device *dev, FILE *fp, char *s)
   {int fl, w, h;
    int info[3], rot[4];
    double x[PG_SPACEDM], ix[PG_SPACEDM];
    PG_dev_geometry *g;

    memset(x, 0, sizeof(x));
    memset(ix, 0, sizeof(ix));

    g = &dev->g;

    x[0] = dev->tcur[0];
    x[1] = dev->tcur[1];

    PG_trans_point(dev, 2, WORLDC, x, PIXELC, ix);
 
    x[0] = dev->char_path[0];
    x[1] = dev->char_path[1];
    fl = !(PM_CLOSETO_ABS(x[0], 1.0) && PM_CLOSETO_ABS(x[1], 0.0));

/* if the char path is not oriented along the x axis do the rotation */
    if (fl)
       {int lx[PG_SPACEDM];

/* convert slope to NDC */
        x[0] *= g->w_nd[1];
        x[1] *= g->w_nd[3];

/* limit the NDC slope */
	x[0] = max(x[0], -1.0);
	x[0] = min(x[0],  1.0);
	x[1] = max(x[1], -1.0);
	x[1] = min(x[1],  1.0);

/* convert slope to PC */
	w = PG_window_width(dev);
        h = PG_window_height(dev);

	lx[0] = floor(x[0]*w + 0.5);
	lx[1] = floor(x[1]*h + 0.5);

        rot[0] = -lx[1];
        rot[1] =  lx[0];
        rot[2] =  lx[0];
        rot[3] =  lx[1];

        PG_CGM_command(dev, CHARACTER_ORIENTATION, rot);};

    info[0] = ix[0];
    info[1] = ix[1];
    info[2] = 1;
    PG_CGM_command(dev, TEXT, info, 1, SC_STRING_S, s);
 
/* restore the character orientation to horizontal */
    if (fl)
       {rot[0] = 0;
        rot[1] = 1;
        rot[2] = 1;
        rot[3] = 0;

        PG_CGM_command(dev, CHARACTER_ORIENTATION, rot);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_NEXT_LINE - do a controlled newline */
 
static void _PG_CGM_next_line(PG_device *dev, FILE *fp)
   {

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CGM_QUERY_POINTER - query the pointer for location and button status */

static void _PG_CGM_query_pointer(PG_device *dev,
				  int *ir, int *pb, int *pq)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_CGM_GRAB_POINTER - take over the pointer */

static int _PG_CGM_grab_pointer(PG_device *dev)
   {int rv;

    rv = FALSE;

    return(rv);}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_CGM_RELEASE_POINTER - release the pointer */

static int _PG_CGM_release_pointer(PG_device *dev)
   {int rv;

    rv = FALSE;

    return(rv);}

/*------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_SETUP_CGM_DEVICE - do the device dependent device initialization
 *                     - for PG_make_device
 */

int PG_setup_cgm_device(PG_device *d)
   {

    d->cgm_page_set            = FALSE;
    d->type_index              = CGMF_DEVICE;

    d->events_pending          = NULL;
    d->query_pointer           = _PG_CGM_query_pointer;
    d->grab_pointer            = _PG_CGM_grab_pointer;
    d->release_pointer         = _PG_CGM_release_pointer;
    d->mouse_event_info        = NULL;
    d->key_event_info          = NULL;
    d->clear_page              = _PG_CGM_clear_page;
    d->clear_window            = _PG_CGM_clear_window;
    d->clear_viewport          = _PG_CGM_clear_viewport;
    d->clear_region            = _PG_CGM_clear_region;
    d->close_device            = _PG_CGM_close_device;
    d->flush_events            = NULL;
    d->draw_dj_polyln_2        = _PG_CGM_draw_disjoint_polyline_2;
    d->draw_curve              = _PG_CGM_draw_curve;
    d->draw_to_abs             = _PG_CGM_draw_to_abs;
    d->draw_to_rel             = _PG_CGM_draw_to_rel;
    d->expose_device           = _PG_CGM_expose_device;
    d->finish_plot             = _PG_CGM_finish_plot;
    d->get_char                = NULL;
    d->get_image               = NULL;
    d->get_text_ext            = _PG_CGM_get_text_ext;
    d->ggetc                   = io_getc;
    d->ggets                   = io_gets;
    d->gputs                   = NULL;
    d->make_device_current     = _PG_CGM_make_device_current;
    d->map_to_color_table      = _PG_CGM_map_to_color_table;
    d->match_rgb_colors        = _PG_match_rgb_colors;
    d->move_gr_abs             = _PG_CGM_move_gr_abs;
    d->move_tx_abs             = _PG_CGM_move_tx_abs;
    d->move_tx_rel             = _PG_CGM_move_tx_rel;
    d->next_line               = _PG_CGM_next_line;
    d->open_screen             = _PG_CGM_open;
    d->put_image               = _PG_CGM_put_image;
    d->query_screen            = _PG_CGM_query;
    d->release_current_device  = _PG_CGM_release_current_device;
    d->resolution_scale_factor = 64;
    d->set_clipping            = _PG_CGM_set_clipping;
    d->set_char_line           = _PG_CGM_set_char_line;
    d->set_char_path           = _PG_CGM_set_char_path;
    d->set_char_precision      = _PG_CGM_set_char_precision;
    d->set_char_size           = _PG_CGM_set_char_size;
    d->set_char_space          = _PG_CGM_set_char_space;
    d->set_char_up             = _PG_CGM_set_char_up;
    d->set_fill_color          = _PG_CGM_set_fill_color;
    d->set_font                = _PG_CGM_set_font;
    d->set_line_color          = _PG_CGM_set_line_color;
    d->set_line_style          = _PG_CGM_set_line_style;
    d->set_line_width          = _PG_CGM_set_line_width;
    d->set_logical_op          = _PG_CGM_set_logical_op;
    d->set_text_color          = _PG_CGM_set_text_color;
    d->shade_poly              = _PG_CGM_shade_poly;
    d->fill_curve              = _PG_CGM_fill_curve;
    d->update_vs               = _PG_CGM_update_vs;
    d->write_text              = _PG_CGM_write_text;
    
    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
