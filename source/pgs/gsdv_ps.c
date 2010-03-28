/*
 * GSDV_PS.C - PGS PS routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
  
#include "pgs_int.h"

/* from the corresponding PR file */

int
 _PG_PS_set_font(PG_device *dev, char *face, char *style, int size);

void
 _PG_PS_draw_disjoint_polyline_2(PG_device *dev, double **r,
				 long n, int clip, int coord),
 _PG_PS_draw_curve(PG_device *dev, PG_curve *crv, int clip),
 _PG_PS_draw_to_abs(PG_device *dev, double x, double y),
 _PG_PS_draw_to_rel(PG_device *dev, double x, double y),
 _PG_PS_get_text_ext(PG_device *dev, int nd, PG_coord_sys cs, char *s, double *p),
 _PG_PS_set_clipping(PG_device *dev, int flag),
 _PG_PS_set_char_line(PG_device *dev, int n),
 _PG_PS_set_char_path(PG_device *dev, double x, double y),
 _PG_PS_set_char_precision(PG_device *dev, int p),
 _PG_PS_set_char_space(PG_device *dev, double s),
 _PG_PS_set_char_size(PG_device *dev, int nd, PG_coord_sys cs, double *x),
 _PG_PS_set_char_up(PG_device *dev, double x, double y),
 _PG_PS_set_fill_color(PG_device *dev, int color, int mapped),
 _PG_PS_set_line_color(PG_device *dev, int color, int mapped),
 _PG_PS_set_line_style(PG_device *dev, int style),
 _PG_PS_set_line_width(PG_device *dev, double width),
 _PG_PS_set_logical_op(PG_device *dev, PG_logical_operation lop),
 _PG_PS_set_text_color(PG_device *dev, int color, int mapped),
 _PG_PS_shade_poly(PG_device *dev, int nd, int n, double **r),
 _PG_PS_fill_curve(PG_device *dev, PG_curve *crv),
 _PG_PS_move_gr_abs(PG_device *dev, double x, double y),
 _PG_PS_move_tx_abs(PG_device *dev, double x, double y),
 _PG_PS_move_tx_rel(PG_device *dev, double x, double y),
 _PG_PS_put_image(PG_device *dev, unsigned char *bf, int ix, int iy,
		  int nx, int ny);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PS_MAKE_DEVICE_CURRENT - make this device the current one for drawing
 *                            - purposes
 */
 
static void _PG_PS_make_device_current(PG_device *dev)
   {

    return;}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PS_RELEASE_CURRENT_DEVICE - make no device current */
 
static void _PG_PS_release_current_device(PG_device *dev)
   {

    return;}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PS_QUERY - query some PostScript device characteristics */

static void _PG_PS_query(PG_device *dev, int *pdx, int *pdy, int *pnc)
   {int dx, dy, nc;

    if (dev->ps_color)
       nc = 256;
    else
       nc = 2;

    dx = (int) (8.5*_PG_gattrs.ps_dots_inch);
    dy = (int) (11.0*_PG_gattrs.ps_dots_inch);

    dev->g.phys_width  = dx;
    dev->g.phys_height = dy;
    dev->phys_n_colors = nc;

    *pdx = dx;
    *pdy = dy;
    *pnc = nc;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_OPEN - initialize a PostScript device */
 
static PG_device *_PG_PS_open(PG_device *dev,
			      double xf, double yf, double dxf, double dyf)
   {int w, h;
    int display_width, display_height, n_colors, mode;
    int Lightest, Light, Light_Gray, Dark_Gray, Dark, Darkest;
    char ltype[MAXLINE], fname[MAXLINE], lname[MAXLINE];
    char *name, *token, *type, *modes, *date, *s;
    double intensity, scale, dpis;
    double pstransp, pslevel, epsflevel;
    PG_font_family *ff;
    PG_dev_geometry *g;
    FILE *ps_fp;

    if (dev == NULL)
       return(NULL);

    g = &dev->g;

    PG_setup_markers();

    dev->type_index       = PS_DEVICE;
    dev->quadrant         = QUAD_ONE;
    dev->hard_copy_device = TRUE;
    dev->print_labels     = TRUE;

    strcpy(ltype, dev->type);
    type = SC_strtok(ltype, " \t\n\r", s);
    if ((type != NULL) && (strcmp(type, "COLOR") == 0))
       dev->ps_color = TRUE;

    modes = SC_strtok(NULL, " \t\n\r", s);
    mode  = PORTRAIT_MODE;
    if (modes != NULL)
       {mode = (strcmp(modes, "LANDSCAPE") == 0) ?
	       LANDSCAPE_MODE : PORTRAIT_MODE;};

    dev->window_orientation = mode;

    strcpy(lname, dev->title);
    name = SC_strtok(lname, " \t\n\r", s);

/* decide on the PostScript level */
    pslevel = -1.0;
    token   = SC_strtok(NULL, " \t\n\r", s);
    if (token != NULL)
       pslevel = SC_stof(token);
    if (pslevel <= 0.0)
       pslevel = 3.0;
    dev->ps_level = pslevel;

/* decide on the EPSF level */
    token     = SC_strtok(NULL, " \t\n\r", s);
    epsflevel = SC_stof(token);

    pstransp = dev->ps_transparent;

    if (name != NULL)
       PG_device_filename(fname, name, ".ps");
    else
       PG_device_filename(fname, "default", ".ps");

    ps_fp = io_open(fname, "w");
    if (ps_fp == NULL)
       return(NULL);

/* set up the PostScript Prolog */
    if (epsflevel > 0.0)
       io_printf(ps_fp, "%%!PS-Adobe-%3.1f EPSF-%3.1f\n", pslevel, epsflevel);
    else
       io_printf(ps_fp, "%%!PS-Adobe-%3.1f\n", pslevel);

/* get the window shape in NDC */
    if ((xf == 0.0) && (yf == 0.0))
       {xf = 0.0;
        yf = 0.0;};

    if (dxf == 0.0)
       dxf = 1.0;

    if (dyf == 0.0)
       dyf = 1.0;

    dpis = _PG_gattrs.ps_dots_inch/600.0;

/* set device pixel coordinate limits */
    switch (mode)
       {default :
	case PORTRAIT_MODE :
	     _PG_PS_query(dev, &display_width, &display_height, &n_colors);

	     w = display_width*dxf;
	     h = display_width*dyf;

	     g->cpc[0] = dpis*SHRT_MIN + display_width;
	     g->cpc[1] = dpis*SHRT_MAX - display_width;
	     g->cpc[2] = dpis*SHRT_MIN + display_height;
       	     g->cpc[3] = dpis*SHRT_MAX - display_height;
	     g->cpc[4] = dpis*SHRT_MIN;
	     g->cpc[5] = dpis*SHRT_MAX;
 
	     g->hwin[0] = display_width*xf;
	     g->hwin[1] = g->hwin[0] + w;
	     g->hwin[2] = display_height*(1.0 - yf);
	     g->hwin[3] = g->hwin[2] + h;

	     SET_PC_FROM_HWIN(g);

	     dev->window_x[0] = g->hwin[0];
	     dev->window_x[2] = g->hwin[2] - h;

	     break;

	case LANDSCAPE_MODE :
	     _PG_PS_query(dev, &display_height, &display_width, &n_colors);

	     w = display_width*dxf;
	     h = display_width*dyf;

	     g->cpc[0] = dpis*SHRT_MIN + display_width;
	     g->cpc[1] = dpis*SHRT_MAX - display_width;
	     g->cpc[2] = dpis*SHRT_MIN + display_height;
       	     g->cpc[3] = dpis*SHRT_MAX - display_height;
 
	     g->hwin[0] = display_width*xf;
	     g->hwin[1] = g->hwin[0] + w;
	     g->hwin[2] = display_height*(1.0 - yf);
	     g->hwin[3] = g->hwin[2] + h;

	     SET_PC_FROM_HWIN(g);

	     dev->window_x[0] = g->hwin[0];
	     dev->window_x[2] = g->hwin[2] - h;
 
	     break;};

    dev->file = ps_fp;

/* decide on the overall color layout
 * for MONOCHROME the 15 maxes the palette as need for annotations
 */
    if (dev->ps_color == FALSE)
       {Color_Map(dev, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);}
    else
       {Color_Map(dev, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);};

    dev->ncolor = max(16, n_colors);
    dev->absolute_n_color = 256;

    io_printf(ps_fp, "%%%%Creator: PACT PGS\n");
    io_printf(ps_fp, "%%%%Title: %s plots\n", lname);
    date = SC_date();
    io_printf(ps_fp, "%%%%CreationDate: %s\n", date);
    SFREE(date);

    io_printf(ps_fp, "%%%%BoundingBox: 0 0 612 792\n");
    io_printf(ps_fp, "%%%%DocumentFonts: Helvetica\n");
    io_printf(ps_fp, "%%%%DocumentNeededResources: font Helvetica\n");
    io_printf(ps_fp, "%%%%EndComments\n");
    io_printf(ps_fp, "%%%%BeginProlog\n");
    io_printf(ps_fp, "%%%%BeginFeatures\n");
    io_printf(ps_fp, "/m {moveto} bind def\n");
    io_printf(ps_fp, "/L {lineto} bind def\n");
    io_printf(ps_fp, "/S {stroke} bind def\n");
    io_printf(ps_fp, "/d {setdash} bind def\n");
    io_printf(ps_fp, "/w {setlinewidth} bind def\n");
    io_printf(ps_fp, "/Tp {pop m pop pop pop pop} bind def\n");
    io_printf(ps_fp, "/Tx {show} bind def\n");
    io_printf(ps_fp, "/RotTxt {gsave translate rotate} bind def\n");
    io_printf(ps_fp, "%%%%EndFeatures\n");

    if (COLOR_POSTSCRIPT_DEVICE(dev))
       {io_printf(ps_fp, "/Srgb {setrgbcolor} bind def\n");

        if (pslevel < 3.0)
           {io_printf(ps_fp, "/ImB {gsave\n");
            io_printf(ps_fp, "      /sf 1 index store pop\n");
            io_printf(ps_fp, "      /oy 1 index store pop\n");
            io_printf(ps_fp, "      /ox 1 index store pop\n");
            io_printf(ps_fp, "      /bc 1 index store pop\n");
            io_printf(ps_fp, "      /ny 1 index store pop\n");
            io_printf(ps_fp, "      /nx 1 index store pop\n");
            io_printf(ps_fp, "      /mx nx 3 mul store\n");
            io_printf(ps_fp, "      /picstr mx string def\n");
            io_printf(ps_fp, "      /sy ny sf mul store\n");
            io_printf(ps_fp, "      /sx nx sf mul store\n");
            io_printf(ps_fp, "      ox oy translate\n");
            io_printf(ps_fp, "      sx sy scale\n");
            io_printf(ps_fp, "      nx ny bc\n");
            io_printf(ps_fp, "      [ nx 0 0 ny 0 0 ]\n");
            io_printf(ps_fp, "      { currentfile picstr readhexstring pop }\n");
            io_printf(ps_fp, "      false 3 colorimage} bind def\n");}

/* use transparency in image plotting (especially for rasters) */
        else if (pstransp == TRUE)
           {io_printf(ps_fp, "/ImB {gsave\n");
            io_printf(ps_fp, "      /sf 1 index store pop\n");
            io_printf(ps_fp, "      /oy 1 index store pop\n");
            io_printf(ps_fp, "      /ox 1 index store pop\n");
            io_printf(ps_fp, "      /bc 1 index store pop\n");
            io_printf(ps_fp, "      /ny 1 index store pop\n");
            io_printf(ps_fp, "      /nx 1 index store pop\n");
            io_printf(ps_fp, "      /sy ny sf mul store\n");
            io_printf(ps_fp, "      /sx nx sf mul store\n");
            io_printf(ps_fp, "      /DeviceRGB setcolorspace\n");
            io_printf(ps_fp, "      ox oy translate\n");
            io_printf(ps_fp, "      sx sy scale\n");
            io_printf(ps_fp, "      /img <<\n");
            io_printf(ps_fp, "             /ImageType 1\n");
            io_printf(ps_fp, "             /Width nx\n");
            io_printf(ps_fp, "             /Height ny\n");
            io_printf(ps_fp, "             /BitsPerComponent bc\n");
            io_printf(ps_fp, "             /Interpolate true\n");
            io_printf(ps_fp, "             /Decode [0 1 0 1 0 1]\n");
            io_printf(ps_fp, "             /ImageMatrix [ nx 0 0 ny 0 0 ]\n");
            io_printf(ps_fp, "             /DataSource currentfile\n");
            io_printf(ps_fp, "             /ASCIIHexDecode filter\n");
            io_printf(ps_fp, "          >> store\n");
            io_printf(ps_fp, "      /msk <<\n");
            io_printf(ps_fp, "             /ImageType 1\n");
            io_printf(ps_fp, "             /Width nx\n");
            io_printf(ps_fp, "             /Height ny\n");
            io_printf(ps_fp, "             /BitsPerComponent bc\n");
            io_printf(ps_fp, "             /Interpolate true\n");
            io_printf(ps_fp, "             /Decode [0 1]\n");
            io_printf(ps_fp, "             /ImageMatrix [ nx 0 0 ny 0 0 ]\n");
            io_printf(ps_fp, "          >> store\n");
            io_printf(ps_fp, "      /t3d <<\n");
            io_printf(ps_fp, "             /ImageType 3\n");
            io_printf(ps_fp, "             /DataDict img\n");
            io_printf(ps_fp, "             /MaskDict msk\n");
            io_printf(ps_fp, "             /InterleaveType 1\n");
            io_printf(ps_fp, "          >> store\n");
            io_printf(ps_fp, "      t3d image} bind def\n");}

        else
           {io_printf(ps_fp, "/ImB {gsave\n");
            io_printf(ps_fp, "      /sf 1 index store pop\n");
            io_printf(ps_fp, "      /oy 1 index store pop\n");
            io_printf(ps_fp, "      /ox 1 index store pop\n");
            io_printf(ps_fp, "      /bc 1 index store pop\n");
            io_printf(ps_fp, "      /ny 1 index store pop\n");
            io_printf(ps_fp, "      /nx 1 index store pop\n");
            io_printf(ps_fp, "      /sy ny sf mul store\n");
            io_printf(ps_fp, "      /sx nx sf mul store\n");
            io_printf(ps_fp, "      /DeviceRGB setcolorspace\n");
            io_printf(ps_fp, "      ox oy translate\n");
            io_printf(ps_fp, "      sx sy scale\n");
            io_printf(ps_fp, "      /imd <<\n");
            io_printf(ps_fp, "             /ImageType 1\n");
            io_printf(ps_fp, "             /Width nx\n");
            io_printf(ps_fp, "             /Height ny\n");
            io_printf(ps_fp, "             /BitsPerComponent bc\n");
            io_printf(ps_fp, "             /Interpolate true\n");
            io_printf(ps_fp, "             /Decode [0 1 0 1 0 1]\n");
            io_printf(ps_fp, "             /ImageMatrix [ nx 0 0 ny 0 0 ]\n");
            io_printf(ps_fp, "             /DataSource currentfile\n");
            io_printf(ps_fp, "             /ASCIIHexDecode filter\n");
            io_printf(ps_fp, "          >> store\n");
            io_printf(ps_fp, "      imd image} bind def\n");};}

    else
       {if (pslevel < 3.0)
           {io_printf(ps_fp, "/ImB {gsave\n");
            io_printf(ps_fp, "      /sf 1 index store pop\n");
            io_printf(ps_fp, "      /oy 1 index store pop\n");
            io_printf(ps_fp, "      /ox 1 index store pop\n");
            io_printf(ps_fp, "      /bc 1 index store pop\n");
            io_printf(ps_fp, "      /ny 1 index store pop\n");
            io_printf(ps_fp, "      /nx 1 index store pop\n");
            io_printf(ps_fp, "      /picstr nx string def\n");
            io_printf(ps_fp, "      /sy ny sf mul store\n");
            io_printf(ps_fp, "      /sx nx sf mul store\n");
            io_printf(ps_fp, "      ox oy translate\n");
            io_printf(ps_fp, "      sx sy scale\n");
            io_printf(ps_fp, "      nx ny bc\n");
            io_printf(ps_fp, "      [ nx 0 0 ny 0 0 ]\n");
            io_printf(ps_fp, "      { currentfile picstr readhexstring pop }\n");
            io_printf(ps_fp, "      image} bind def\n");}

/* use transparency in image plotting (especially for rasters) */
        else if (pstransp == TRUE)
           {io_printf(ps_fp, "/ImB {gsave\n");
            io_printf(ps_fp, "      /sf 1 index store pop\n");
            io_printf(ps_fp, "      /oy 1 index store pop\n");
            io_printf(ps_fp, "      /ox 1 index store pop\n");
            io_printf(ps_fp, "      /bc 1 index store pop\n");
            io_printf(ps_fp, "      /ny 1 index store pop\n");
            io_printf(ps_fp, "      /nx 1 index store pop\n");
            io_printf(ps_fp, "      /sy ny sf mul store\n");
            io_printf(ps_fp, "      /sx nx sf mul store\n");
            io_printf(ps_fp, "      /DeviceGray setcolorspace\n");
            io_printf(ps_fp, "      ox oy translate\n");
            io_printf(ps_fp, "      sx sy scale\n");
            io_printf(ps_fp, "      /img <<\n");
            io_printf(ps_fp, "             /ImageType 1\n");
            io_printf(ps_fp, "             /Width nx\n");
            io_printf(ps_fp, "             /Height ny\n");
            io_printf(ps_fp, "             /BitsPerComponent bc\n");
            io_printf(ps_fp, "             /Interpolate true\n");
            io_printf(ps_fp, "             /Decode [0 1]\n");
            io_printf(ps_fp, "             /ImageMatrix [ nx 0 0 ny 0 0 ]\n");
            io_printf(ps_fp, "             /DataSource currentfile\n");
            io_printf(ps_fp, "             /ASCIIHexDecode filter\n");
            io_printf(ps_fp, "          >> store\n");
            io_printf(ps_fp, "      /msk <<\n");
            io_printf(ps_fp, "             /ImageType 1\n");
            io_printf(ps_fp, "             /Width nx\n");
            io_printf(ps_fp, "             /Height ny\n");
            io_printf(ps_fp, "             /BitsPerComponent bc\n");
            io_printf(ps_fp, "             /Interpolate true\n");
            io_printf(ps_fp, "             /Decode [0 1]\n");
            io_printf(ps_fp, "             /ImageMatrix [ nx 0 0 ny 0 0 ]\n");
            io_printf(ps_fp, "          >> store\n");
            io_printf(ps_fp, "      /t3d <<\n");
            io_printf(ps_fp, "             /ImageType 3\n");
            io_printf(ps_fp, "             /DataDict img\n");
            io_printf(ps_fp, "             /MaskDict msk\n");
            io_printf(ps_fp, "             /InterleaveType 1\n");
            io_printf(ps_fp, "          >> store\n");
            io_printf(ps_fp, "      t3d image} bind def\n");}

        else
           {io_printf(ps_fp, "/ImB {gsave\n");
            io_printf(ps_fp, "      /sf 1 index store pop\n");
            io_printf(ps_fp, "      /oy 1 index store pop\n");
            io_printf(ps_fp, "      /ox 1 index store pop\n");
            io_printf(ps_fp, "      /bc 1 index store pop\n");
            io_printf(ps_fp, "      /ny 1 index store pop\n");
            io_printf(ps_fp, "      /nx 1 index store pop\n");
            io_printf(ps_fp, "      /sy ny sf mul store\n");
            io_printf(ps_fp, "      /sx nx sf mul store\n");
            io_printf(ps_fp, "      /DeviceGray setcolorspace\n");
            io_printf(ps_fp, "      ox oy translate\n");
            io_printf(ps_fp, "      sx sy scale\n");
            io_printf(ps_fp, "      /imd <<\n");
            io_printf(ps_fp, "             /ImageType 1\n");
            io_printf(ps_fp, "             /Width nx\n");
            io_printf(ps_fp, "             /Height ny\n");
            io_printf(ps_fp, "             /BitsPerComponent bc\n");
            io_printf(ps_fp, "             /Interpolate true\n");
            io_printf(ps_fp, "             /Decode [0 1]\n");
            io_printf(ps_fp, "             /ImageMatrix [ nx 0 0 ny 0 0 ]\n");
            io_printf(ps_fp, "             /DataSource currentfile\n");
            io_printf(ps_fp, "             /ASCIIHexDecode filter\n");
            io_printf(ps_fp, "          >> store\n");
            io_printf(ps_fp, "      imd image} bind def\n");};};

    io_printf(ps_fp, "/ImE {grestore} bind def\n");
    io_printf(ps_fp, "/Cf {grestore} bind def\n");
    io_printf(ps_fp, "/Cn {gsave m\n");
    io_printf(ps_fp, "     dup 0 exch rlineto\n");
    io_printf(ps_fp, "     exch 0 rlineto\n");
    io_printf(ps_fp, "     0 exch neg rlineto\n");
    io_printf(ps_fp, "     closepath clip newpath} bind def\n");

    scale = 72.0/_PG_gattrs.ps_dots_inch;
    switch (mode)
       {default :
	case PORTRAIT_MODE :
             io_printf(ps_fp,
		       "/Pset {0 0 translate %f %f scale gsave} bind def\n",
		       scale, scale);
	     break;
	case LANDSCAPE_MODE :
             io_printf(ps_fp,
		       "/Pset {612 0 translate 90 rotate %f %f scale gsave} bind def\n",
		       scale, (8.5/11.0)*scale);
	     break;};
    io_printf(ps_fp, "/Pend {grestore} bind def\n");

    io_printf(ps_fp, "/Fplot  {/Helvetica findfont 12 scalefont setfont} bind def\n");
    io_printf(ps_fp, "/Flabel {/Helvetica findfont  8 scalefont setfont} bind def\n");
    io_printf(ps_fp, "/SF {findfont exch scalefont setfont} bind def\n");
    io_printf(ps_fp, "%%%%EndProlog\n");

    io_printf(ps_fp, "%%%%BeginSetup\n");
    io_printf(ps_fp, "%%%%IncludeFont: Helvetica\n");
    io_printf(ps_fp, "%%%%EndSetup\n");
    io_printf(ps_fp, "\n%% PGS Device Inits\n");

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

    return(dev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PS_MAP_TO_COLOR_TABLE - map the PGS palette to host
 *                           - color table values
 */

static void _PG_PS_map_to_color_table(PG_device *dev, PG_palette *pal)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_CLOSE_DEVICE - close a device */
 
static void _PG_PS_close_device(PG_device *dev)
   {

    if (dev->file != NULL)
       {io_printf(dev->file, "%%%%Trailer\n");
        io_printf(dev->file, "%%%%EOF\n");
	io_close(dev->file);

/* clean up the device */
	PG_rl_device(dev);};

   return;}

/*--------------------------------------------------------------------------*/

/*                            PAGE STATE ROUTINES                           */

/*--------------------------------------------------------------------------*/
 
/* _PG_PS_CLEAR_WINDOW - clear the screen */
 
static void _PG_PS_clear_window(PG_device *dev)
   {

    if (dev == NULL)
       return;

    PG_make_device_current(dev);

    io_printf(dev->file, "%%%%Page: %d\n", ++_PG.pagen);
    io_printf(dev->file, "Pset\n");

    PG_release_current_device(dev);

    PG_free_raster_device(dev->raster);
    dev->raster = NULL;

/* NOTE: this is critical for the correct balancing of
 *       gsave's and grestore's used by PG_set_clipping,
 *       PG_clear_window, and PG_finish_plot
 */
    if (dev->clipping)
       {dev->clipping = FALSE;
        PG_set_clipping(dev, TRUE);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_CLEAR_VIEWPORT - clear the viewport */
 
static void _PG_PS_clear_viewport(PG_device *dev)
   {

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_CLEAR_REGION - clear the rectangular region specified in NDC */
 
static void _PG_PS_clear_region(PG_device *dev, int nd, PG_coord_sys cs,
				double *bx, int pad)
   {

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_UPDATE_VS - update the view surface for the given device */
 
static void _PG_PS_update_vs(PG_device *dev)
   {

    io_flush(dev->file);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_FINISH_PLOT - do what's necessary to finish up a graphical image
 *                    - and get the device updated with the image
 *                    - after this function nothing more can be added to
 *                    - the image
 */
 
static void _PG_PS_finish_plot(PG_device *dev)
   {

    PG_make_device_current(dev);

    if (dev != NULL)

/* make sure there is a grestore to match the gsave that clipping on does */
       {if (dev->clipping)
	   {io_printf(dev->file, "\n%% Clipping OFF\n");
	    io_printf(dev->file, "Cf\n");};    

        _PG_PS_put_raster(dev, 0, 0, -1, -1);

	io_printf(dev->file, "Pend showpage\n\n");

	PG_update_vs(dev);

	PG_release_current_device(dev);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_CLEAR_PAGE - clear the current page
 *                   - and go to the line i on the screen
 */
 
static void _PG_PS_clear_page(PG_device *dev, int i)
   {

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PS_EXPOSE_DEVICE - make this device the topmost one */
 
static void _PG_PS_expose_device(PG_device *dev)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_WRITE_TEXT - write out text to the appropriate device */
 
static void _PG_PS_write_text(PG_device *dev, FILE *fp, char *s)
   {int nlrp, fl, h, w;
    double x, y;
    char *t;
    FILE *file;
    PG_dev_geometry *g;

    if (*s == '\0')
       return;

    g = &dev->g;

    _PG_PS_set_dev_color(dev, PG_TEXT, TRUE);

    nlrp = SC_char_count(s, '(') + SC_char_count(s, ')');
    if (nlrp > 0)
       {t = FMAKE_N(char, strlen(s) + nlrp + 1, "_PG_PS_WRITE_TEXT:t");
        strcpy(t, s);
        SC_str_replace(t, "(", "\\(");
        SC_str_replace(t, ")", "\\)");}

    else
       t = s;
 
    file = dev->file;

    x = dev->char_path[0];
    y = dev->char_path[1];
    fl = !(PM_CLOSETO_ABS(x, 1.0) && PM_CLOSETO_ABS(y, 0.0));

/* if the char path is not oriented along the x axis do the rotation
 * and translation of the coordinate system
 */
    if (fl)
       {int ix, iy;
	double an, r, ca, sa, dx, dy;
	double x0[PG_SPACEDM];

	an = RAD_DEG*PM_atan(x, y);
        r  = sqrt(x*x + y*y) + SMALL;
	ca = x/r;
	sa = y/r;

/* compute the displacement induced by the rotation about the origin */
	x0[0] = dev->tcur[0]*g->w_nd[1];
        x0[1] = dev->tcur[1]*g->w_nd[3];

        dx = (1.0 - ca)*x0[0] + sa*x0[1];
        dy = -sa*x0[0] + (1.0 - ca)*x0[1];

	w = PG_window_width(dev);
        h = PG_window_height(dev);

	ix = floor(dx*w + 0.5);
	iy = floor(dy*h + 0.5);

        io_printf(file, "%7.2f %d %d RotTxt\n", an, ix, iy);};

    io_printf(file, "(%s) Tx\n", t);

/* restore the original matrix */
    if (fl)
       io_printf(file, "grestore\n");

    if (nlrp > 0)
       SFREE(t);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_NEXT_LINE - do a controlled newline */
 
static void _PG_PS_next_line(PG_device *dev, FILE *fp)
   {

    io_printf(dev->file, "newline\n");
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PS_QUERY_POINTER - query the pointer for location and button status */

static void _PG_PS_query_pointer(PG_device *dev, int *ir,
				 int *pb, int *pq)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_SETUP_PS_DEVICE - do the device dependent device initialization
 *                    - for PG_make_device
 */

int PG_setup_ps_device(PG_device *d)
   {

    d->type_index             = PS_DEVICE;
    d->ps_color               = FALSE;

    d->events_pending          = NULL;
    d->query_pointer           = _PG_PS_query_pointer;
    d->mouse_event_info        = NULL;
    d->key_event_info          = NULL;
    d->clear_page              = _PG_PS_clear_page;
    d->clear_window            = _PG_PS_clear_window;
    d->clear_viewport          = _PG_PS_clear_viewport;
    d->clear_region            = _PG_PS_clear_region;
    d->close_device            = _PG_PS_close_device;
    d->flush_events            = NULL;
    d->draw_dj_polyln_2        = _PG_PS_draw_disjoint_polyline_2;
    d->draw_curve              = _PG_PS_draw_curve;
    d->draw_to_abs             = _PG_PS_draw_to_abs;
    d->draw_to_rel             = _PG_PS_draw_to_rel;
    d->expose_device           = _PG_PS_expose_device;
    d->finish_plot             = _PG_PS_finish_plot;
    d->get_char                = NULL;
    d->get_image               = NULL;
    d->get_text_ext            = _PG_PS_get_text_ext;
    d->ggets                   = io_gets;
    d->gputs                   = NULL;
    d->make_device_current     = _PG_PS_make_device_current;
    d->map_to_color_table      = _PG_PS_map_to_color_table;
    d->match_rgb_colors        = _PG_match_rgb_colors;
    d->move_gr_abs             = _PG_PS_move_gr_abs;
    d->move_tx_abs             = _PG_PS_move_tx_abs;
    d->move_tx_rel             = _PG_PS_move_tx_rel;
    d->next_line               = _PG_PS_next_line;
    d->open_screen             = _PG_PS_open;
    d->put_image               = _PG_PS_put_image;
    d->query_screen            = _PG_PS_query;
    d->release_current_device  = _PG_PS_release_current_device;
    d->resolution_scale_factor = 4;
    d->set_clipping            = _PG_PS_set_clipping;
    d->set_char_line           = _PG_PS_set_char_line;
    d->set_char_path           = _PG_PS_set_char_path;
    d->set_char_precision      = _PG_PS_set_char_precision;
    d->set_char_size           = _PG_PS_set_char_size;
    d->set_char_space          = _PG_PS_set_char_space;
    d->set_char_up             = _PG_PS_set_char_up;
    d->set_fill_color          = _PG_PS_set_fill_color;
    d->set_font                = _PG_PS_set_font;
    d->set_line_color          = _PG_PS_set_line_color;
    d->set_line_style          = _PG_PS_set_line_style;
    d->set_line_width          = _PG_PS_set_line_width;
    d->set_logical_op          = _PG_PS_set_logical_op;
    d->set_text_color          = _PG_PS_set_text_color;
    d->shade_poly              = _PG_PS_shade_poly;
    d->fill_curve              = _PG_PS_fill_curve;
    d->update_vs               = _PG_PS_update_vs;
    d->write_text              = _PG_PS_write_text;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
