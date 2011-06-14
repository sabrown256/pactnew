/*
 * GSPR_QD.C - PGS QuickDraw primitive routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "scope_quickdraw.h"

void
 _PG_set_text_font_mac(PG_device *dev, int index),
 _PG_set_text_size_mac(PG_device *dev, int size_index, int flag);
 
 static void _PG_qd_draw_polyline(PG_device *dev, double *x, double *y, int n);

/*--------------------------------------------------------------------------*/

/*                         STATE QUERY ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* _PG_QD_GET_TEXT_EXT - return the text extent in CS
 *                     - of the given string
 */

void _PG_qd_get_text_ext(PG_device *dev, int nd, PG_coord_sys cs, char *s, double *p)
   {double x, y;
    int ix, iy;
    FontInfo info;

    SetPort(dev->window);

    STRING_WIDTH(ix, s);

    GetFontInfo(&info);
    iy = info.ascent + info.descent;

    p[0] = ix;
    p[1] = iy;
    p[2] = 0;

    p[1] *= 0.7;

    PG_trans_interval(dev, nd, PIXELC, p, cs, p);

    return;}

/*--------------------------------------------------------------------------*/

/*                          STATE CHANGE ROUTINES                           */

/*--------------------------------------------------------------------------*/
 
/* _PG_QD_SET_LOGICAL_OP - set the logical operation */
 
void _PG_qd_set_logical_op(PG_device *dev, PG_logical_operation lop)
   {

    dev->logical_op = lop;
    SetPort(dev->window);

    switch (lop)
       {case GS_XOR :
/* GOTCHA: Put something appropriate here */
             break; 

        default      :
        case GS_COPY :
/* GOTCHA: Put something appropriate here */
             break;};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_SET_LINE_STYLE - set the line style */
 
void _PG_qd_set_line_style(PG_device *dev, int style)
   {Pattern patt;

    dev->line_style = style;
    SetPort(dev->window);

    switch (style)
       {case LINE_DASHED    :
             StuffHex(&patt, "\p0607333303CCFC78");
             break; 
        case LINE_DOTTED    :
             StuffHex(&patt, "\p44BA455545BA4439");
             break;
        case LINE_DOTDASHED :
             StuffHex(&patt, "\p02397D7D7D39827C");
             break;
        case LINE_SOLID     :
        default             :
             StuffHex(&patt, "\pFFFFFFFFFFFFFFFF");
             break;};

    PenPat(&patt);
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_SET_LINE_WIDTH - set the line width */
 
void _PG_qd_set_line_width(PG_device *dev, double width)
   {int lwd;

    dev->line_width = width;
    SetPort(dev->window);

    lwd = max(1, width);
    PenSize(lwd, lwd);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_SET_LINE_COLOR - set the line color */
 
void _PG_qd_set_line_color(PG_device *dev, int color, int mapped)
   {dev->line_color = color;
    SetPort(dev->window);

    if (mapped)
       color = _PG_trans_color(dev, color);

#ifdef MAC_COLOR
    if (dev->ncolor > 2)
       RGBForeColor(&dev->current_palette->true_colormap[color]);
#endif
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_SET_TEXT_COLOR - set the color of the text */
 
void _PG_qd_set_text_color(PG_device *dev, int color, int mapped)
   {dev->text_color = color;
    SetPort(dev->window);

    if (mapped)
       color = _PG_trans_color(dev, color);

#ifdef MAC_COLOR
    if (dev->ncolor > 2)
       RGBForeColor(&dev->current_palette->true_colormap[color]);
#endif
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_SET_FILL_COLOR - sets current fill color */

void _PG_qd_set_fill_color(PG_device *dev, int color, int mapped)
   {dev->fill_color = color;
    SetPort(dev->window);

#ifdef MAC_COLOR

    if (dev->ncolor > 2)
       {if (mapped)
           RGBForeColor(&dev->current_palette->true_colormap[color]);
        else
           {RGBColor pixcolor;

            Index2Color((long) color, &pixcolor);
            RGBForeColor(&pixcolor);};};
#endif

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_SET_FONT - set the character font */

int _PG_qd_set_font(PG_device *dev, char *face, char *style, int size)
   {int nfont, nstyle;
    char *font_name;
    short f;
    FontInfo font_data;

    if (dev == NULL)
       return(FALSE);

    if (!PG_setup_font(dev, face, style, size, &font_name, &nfont, &nstyle))
       return(FALSE);

    SetPort(dev->window);

/* set the type face */
    GET_FNUM(font_name, &f);
    _PG_set_text_font_mac(dev, (int) f);

/* set the type style */
    switch (nstyle)
       {case 0  :
             f = normal;             /* medium -> normal */
             break;
        case 1  :
             f = italic;             /* italic -> italic */
             break;
        case 2  :
             f = bold;               /* bold -> bold */
             break;
        case 3  :
             f = bold & italic;      /* bold-italic -> bold&italic */
             break;
        default :
             f = normal;
             break;};

    TextFace(f);
    TextSize((short) size);
    GetFontInfo(&font_data);
    
    dev->char_width_s  = font_data.widMax/PG_window_width(dev);
    dev->char_height_s = dev->txt_ratio*(font_data.ascent + font_data.descent)/
                         PG_window_height(dev);
    
    PG_release_current_device(dev);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_SET_CHAR_SIZE - set the character size in CS */

void _PG_qd_set_char_size(PG_device *dev, int nd, PG_coord_sys cs, double *x)
   {int size;
    double p[PG_SPACEDM];

    PG_trans_point(dev, nd, cs, x, NORMC, p);

    dev->char_height_s = p[0];
    dev->char_width_s  = p[1];

    size = dev->char_width_s*PG_window_width(dev);
    if (size > 4)
       _PG_set_text_size_mac(dev, size, FALSE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_SET_CHAR_PATH - set the direction along which text will be written
 *                      - defaults to (1, 0)
 */

void _PG_qd_set_char_path(PG_device *dev, double x, double y)
   {

    dev->char_path[0] = x;
    dev->char_path[1] = y;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_SET_CHAR_PRECISION - set the character precision
 *                           - fast and fixed size or
 *                           - slow and flexible
 */

void _PG_qd_set_char_precision(PG_device *dev, int p)
   {

    dev->char_precision = p;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_SET_CHAR_SPACE - set the space between characters */

void _PG_qd_set_char_space(PG_device *dev, double s)
   {

    dev->char_space = (double) s;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_SET_CHAR_UP - set the direction which is up for individual
 *                    - characters
 *                    - defaults to (0, 1)
 */

void _PG_qd_set_char_up(PG_device *dev, double x, double y)
   {

    dev->char_up[0] = x;
    dev->char_up[1] = y;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_SET_CHAR_LINE - set the number characters per line */
 
void _PG_qd_set_char_line(PG_device *dev, int n)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_SET_CLIPPING - set clipping
 *                     - flag = FALSE  -->  clipping off
 *                     - flag = TRUE   -->  clipping on
 */

void _PG_qd_set_clipping(PG_device *dev, int flag)
   {int pc[PG_BOXSZ];
    Rect shape;

    _PG_find_clip_region(dev, pc, flag, FALSE);

/* add a thick line worth of pixels on the bottom and right */
    pc[1] += 4;
    pc[3] += 4;

    PG_QUAD_FOUR_BOX(dev, pc);

    SetPort(dev->window);
    SETRECT(&shape, pc[0], pc[2], pc[1], pc[3]);
    ClipRect(&shape);
 
    dev->clipping = flag;

    return;}
 
/*--------------------------------------------------------------------------*/

/*                          MOVE AND DRAW ROUTINES                          */

/*--------------------------------------------------------------------------*/
 
/* _PG_QD_MOVE_GR_ABS - move the current graphics cursor position to the
 *                    - given absolute coordinates in WC
 */
 
void _PG_qd_move_gr_abs(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_gr_abs(dev, p);

	PG_trans_point(dev, 2, WORLDC, p, PIXELC, p);
 
	SetPort(dev->window);
	MoveTo(p[0], p[1]);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_MOVE_TX_ABS - move the current text cursor position to the
 *                    - given coordinates in WC
 */
 
void _PG_qd_move_tx_abs(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_tx_abs(dev, p);

	PG_trans_point(dev, 2, WORLDC, p, PIXELC, p);
 
	SetPort(dev->window);
	MoveTo(p[0], p[1]);};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_MOVE_TX_REL - move the current text cursor position to the
 *                    - given relative coordinates in WC
 */
 
void _PG_qd_move_tx_rel(PG_device *dev, double x, double y)
   {int p[0], p[1];
    double p[PG_SPACEDM];

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_tx_rel(dev, p);

	PG_trans_point(dev, 2, WORLDC, p, PIXELC, p);
 
	SetPort(dev->window);
	Move(p[0], p[1]);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_DRAW_TO_ABS - draw a line from current position to
 *                    - absolute position (x, y)
 *                    - in WC
 */
 
void _PG_qd_draw_to_abs(PG_device *dev, double x, double y)
   {int p[0], p[1];
    double p[PG_SPACEDM];

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_gr_abs(dev, p);

	PG_trans_point(dev, 2, WORLDC, p, PIXELC, p);
 
	PG_QUAD_FOUR_POINT(dev, p);

	SetPort(dev->window);
	LineTo(p[0], p[1]);};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_DRAW_TO_REL - draw a line from current position to
 *                    - relative position (x, y)
 *                    - in WC
 */
 
void _PG_qd_draw_to_rel(PG_device *dev, double x, double y)
   {int p[0], p[1];
    double p[PG_SPACEDM];

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_gr_rel(dev, p);

	PG_trans_point(dev, 2, WORLDC, p, PIXELC, p);

	PG_QUAD_FOUR_POINT(dev, p);

	SetPort(dev->window);
	LineTo(p[0], p[1]);};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_DRAW_POLYLINE - plot line segments between points specified
 *                      - by values from an array of x and an array of
 *                      - y values make our own calls to lineabs so that
 *                      - log plots will always work
 */
 
void _PG_qd_draw_polyline(PG_device *dev, double *x, double *y, int n)
   {int i;

    PG_move_gr_abs_n(dev, x);

    for (i = 1; i < n; i++)
        PG_draw_to_abs_n(dev, x);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_DRAW_CURVE - draw a PG_curve */
 
void _PG_qd_draw_curve(PG_device *dev, PG_curve *crv, int clip)
   {int i, n, xo, yo, ix, iy;
    int p[PG_SPACEDM];
    int *x, *y;

    n  = crv->n;
    x  = crv->x;
    y  = crv->y;
    xo = crv->x_origin;
    yo = crv->y_origin;

    SetPort(dev->window);

    ix = x[0] + xo;
    iy = y[0] + yo;
    MoveTo(ix, iy);

    for (i = 1; i < n; i++)
        {p[0] = x[i] + xo;
         p[1] = y[i] + yo;

	 PG_QUAD_FOUR_POINT(dev, p);

         LineTo(p[0], p[1]);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_DRAW_DISJOINT_POLYLINE_2 - draws disjoint two dimensional
 *                                 - line segments specified in CS
 */

void _PG_qd_draw_disjoint_polyline_2(PG_device *dev, double **r,
				     long n, int flag, PG_coord_sys cs)
   {int i;
    double x1[PG_SPACEDM], x2[PG_SPACEDM];
    double *px, *py;

    px = r[0];
    py = r[1];

    for (i = 0; i < n; i++)
        {x1[0] = *px++;
	 x1[1] = *py++;
	 x2[0] = *px++;
	 x2[1] = *py++;
	 PG_draw_line_n(dev, 2, cs, x1, x2, dev->clipping);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_SHADE_POLY - polygon shading routine */

void _PG_qd_shade_poly(PG_device *dev, int nd, int n, double **r)
   {int i;
    PolyHandle poly;
    Pattern patt;

    if (nd == 3)
       _PG_rst_shade_poly(dev, nd, n, r);

    else if (dev != NULL)
       {SetPort(dev->window);

	poly = OpenPoly();

	pc[0] = r[0][0];
	pc[1] = r[1][0];
	PG_move_gr_abs_n(dev, p);
	for (i = 1; i < n; i++)
	    {pc[0] = r[0][i];
	     pc[1] = r[1][i];
	     PG_draw_to_abs_n(dev, p);};

	ClosePoly();

	PaintPoly(poly);
	KillPoly(poly);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_FILL_CURVE - fill a closed PG_curve */

void _PG_qd_fill_curve(PG_device *dev, PG_curve *crv)
   {int i, n, xo, yo;
    int p[PG_SPACEDM];
    int *x, *y;
    PolyHandle poly;
    Pattern patt;

    n  = crv->n;
    x  = crv->x;
    y  = crv->y;
    xo = crv->x_origin;
    yo = crv->y_origin;

    SetPort(dev->window);
    poly = OpenPoly();

    p[0] = x[0] + xo;
    p[1] = y[0] + yo;

    PG_QUAD_FOUR_POINT(dev, p);
    MoveTo(p[0], p[1]);

    for (i = 1; i < n; i++)
        {p[0] = x[i] + xo;
         p[1] = y[i] + yo;

	 PG_QUAD_FOUR_POINT(dev, p);
         LineTo(p[0], p[1]);};

    ClosePoly();

    PaintPoly(poly);
    KillPoly(poly);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_PUT_IMAGE - put the image on the screen
 *                  - the image buffer may be overwritten by the pseudo
 *                  - color mapping if it is needed!!
 */

void _PG_qd_put_image(PG_device *dev, unsigned char *bf,
		      int ix, int iy, int nx, int ny)
   {int i, k, l, n_pal_colors, n_dev_colors;
    int pbx[PG_BOXSZ];
    double rv, scale;
    Rect srect, drect;
    WindowPtr win;
    PG_palette *pal;
    RGB_color_map *pseudo_cm, *true_cm;

    pbx[0] = ix;
    pbx[1] = ix + nx;
    pbx[2] = iy;
    pbx[3] = iy + ny;

    SetPort(dev->window);

    pal          = dev->current_palette;
    n_dev_colors = dev->absolute_n_color;
    n_pal_colors = pal->n_pal_colors;
    pseudo_cm    = pal->pseudo_colormap;
    true_cm      = pal->true_colormap;

    PG_invert_image_data(bf, nx, ny, 1);

    if ((n_dev_colors < n_pal_colors) && (pseudo_cm != NULL))
       {unsigned char *pbf;

        pbf = bf;
        PM_random(-1);
        for (l = 0; l < ny; l++)
            for (k = 0; k < nx; k++)
                {i      = *pbf;
                 rv     = 3.5*PM_random(1) - 0.83;
                 *pbf++ = (rv < pseudo_cm[i].red) ?
                          pseudo_cm[i].green : pseudo_cm[i].blue;};};

    win = dev->window;
    SelectWindow(win);

    k = nx*ny;

    PG_QUAD_FOUR_BOX(dev, pbx);

    SETRECT(&srect, 0, 0, nx, ny);
    SETRECT(&drect, pbx[0], pbx[2], pbx[1], pbx[3]);

    if (n_pal_colors > 2)
       {PixMap **shbits, *spm, *dpm;
        CTabHandle sct, dct;
        ColorSpec *dcs;
        PG_palette *stdpal;
        RGBColor pixcolor, *bfRGB;
        long val;
        static RGBColor white = {0xFFFF, 0xFFFF, 0xFFFF};
        static RGBColor black = {0, 0, 0};

        for (i = 0; i < k; i++)
            bf[i] += 16;

        dpm = *(((CWindowPtr) win)->portPixMap);
        dct = dpm->pmTable;
        
        if (dpm->pixelSize <= 8)
           {shbits        = NewPixMap();
            spm           = *shbits;
            sct           = spm->pmTable;
            spm->rowBytes = nx | 0x8000;
            spm->bounds   = srect;
            spm->pmTable  = dct;
            dcs = (*dct)->ctTable;
            spm->baseAddr = (char *) bf;}
        else
           {shbits = NULL;
	    spm    = NULL;
	    dcs    = CMAKE_N(ColorSpec, n_pal_colors + 18);};

/* NOTE: the following loop maxes out at n_pal_colors + 2 because
 * n_pal_colors does not include black and white!
 */

        for (i = 0, l = 16; i < n_pal_colors + 2; i++, l++)
            {dcs[l].value = l;
             dcs[l].rgb   = true_cm[i];};

        stdpal  = PG_fset_palette(dev, "standard");
        true_cm = stdpal->true_colormap;
        for (i = 0; i < 16; i++)
            {dcs[i].value = i;
             dcs[i].rgb   = true_cm[i];};
        dev->current_palette = pal;

/* NOTE: cruft abounds on the wonderful Macintosh!!!
 * the problem seems to be that the mapping from index->RGB through
 * a PixMap is unknown
 * hence PGS is forced to filter the indices through
 * an index->PGS-RGB->MAC-RGB->index process
 * the MAC-RGB->index process is limited by the inverse color table
 * which is essentially undocumented
 *
 * GOTCHA: the THINK C debugger does something funny with the
 * color table(s) or palettes or something so that one has to 
 * reboot between debugging runs or the correlation between this
 * coding and the on screen behavior is a hard 0!!!!!!!
 *
 * Don't know if this is an issue anymore.
 */
        if (dpm->pixelSize <= 8)
           {for (i = 0; i < k; i++)
                {Index2Color((long) bf[i], &pixcolor);
                 val      = Color2Index(&pixcolor);
                 bf[i]    = val;};
            RGBForeColor(&black);
            RGBBackColor(&white);

            CopyBits((BitMap *) spm, (BitMap *) dpm,
                     &srect, &drect, srcCopy, NULL);}
        else
           {short j, k;
            for (i = 0, j = iy; j < iy + ny; j++)
                {for (k = ix; k < ix + nx; k++, i++)
		     SetCPixel(k, j, &dcs[bf[i]].rgb);};}        

        if (dpm->pixelSize > 8)
           {CFREE(dcs);}
        else
           {spm->pmTable = sct;
            DisposePixMap(shbits);};}

    else
       {int bx;
        BitMap *spm, *dpm;
        unsigned char *bm;

        bx = _PG_byte_bit_map(bf, nx, ny, FALSE);

        spm = CMAKE(BitMap);
        spm->baseAddr = (char *) bf;
        spm->rowBytes = bx;
        spm->bounds   = srect;

        dpm = &(win->portBits);

        CopyBits(spm, dpm, &srect, &drect, srcCopy, NULL);

        CFREE(spm);};

    return;}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#if 0
/* _PG_QD_PUT_IMAGE - put the image on the screen
 *                  - the image buffer may be overwritten by the pseudo
 *                  - color mapping if it is needed!!
 */

void _PG_qd_put_image(PG_device *dev, unsigned char *bf,
		      int ix, int iy, int nx, int ny)
   {Rect srect, drect;
    WindowPtr win;
    double rv, scale;
    int i, k, l, n_pal_colors, n_dev_colors;
    PG_palette *pal;
    RGB_color_map *pseudo_cm, *true_cm;

    SetPort(dev->window);

    pal          = dev->current_palette;
    n_dev_colors = dev->absolute_n_color;
    n_pal_colors = pal->n_pal_colors;
    pseudo_cm    = pal->pseudo_colormap;
    true_cm      = pal->true_colormap;

    PG_invert_image_data(bf, nx, ny, 1);

    if ((n_dev_colors < n_pal_colors) && (pseudo_cm != NULL))
       {unsigned char *pbf;

        pbf = bf;
        PM_random(-1);
        for (l = 0; l < ny; l++)
            for (k = 0; k < nx; k++)
                {i      = *pbf;
                 rv     = 3.5*PM_random(1) - 0.83;
                 *pbf++ = (rv < pseudo_cm[i].red) ?
                          pseudo_cm[i].green : pseudo_cm[i].blue;};};

    win = dev->window;
    SelectWindow(win);

    k = nx*ny;

    SETRECT(&srect, 0, 0, nx, ny);
    SETRECT(&drect, ix, iy, ix+nx, iy+ny);

    if (n_pal_colors > 2)
       {PixMap **shbits, *spm, *dpm;
        CTabHandle sct, dct;
        ColorSpec *dcs;
        PG_palette *stdpal;
        RGBColor pixcolor, *bfRGB;
        long val;
        static RGBColor white = {0xFFFF, 0xFFFF, 0xFFFF};
        static RGBColor black = {0, 0, 0};

        for (i = 0; i < k; i++)
            bf[i] += 16;

        dpm = *(((CWindowPtr) win)->portPixMap);
        dct = dpm->pmTable;
        
        shbits = NewPixMap();
        spm           = *shbits;
        sct           = spm->pmTable;
        spm->rowBytes = nx | 0x8000;
        spm->bounds   = srect;
        spm->pmTable  = dct;
        if (dpm->pixelSize <= 8)
           {dcs = (*dct)->ctTable;
            spm->baseAddr = (char *) bf;}
        else
           {dcs   = CMAKE_N(ColorSpec, n_pal_colors + 18);
            bfRGB = CMAKE_N(RGBColor, k);
            spm->baseAddr = (char *) bfRGB;};

/* NOTE: the following loop maxes out at n_pal_colors + 2 because
 * n_pal_colors does not include black and white!
 */

        for (i = 0, l = 16; i < n_pal_colors + 2; i++, l++)
            {dcs[l].value = l;
             dcs[l].rgb   = true_cm[i];};

        stdpal  = PG_fset_palette(dev, "standard");
        true_cm = stdpal->true_colormap;
        for (i = 0; i < 16; i++)
            {dcs[i].value = i;
             dcs[i].rgb   = true_cm[i];};
        dev->current_palette = pal;

/* NOTE: cruft abounds on the wonderful Macintosh!!!
 * the problem seems to be that the mapping from index->RGB through
 * a PixMap is unknown
 * hence PGS is forced to filter the indices through
 * an index->PGS-RGB->MAC-RGB->index process
 * the MAC-RGB->index process is limited by the inverse color table
 * which is essentially undocumented
 *
 * GOTCHA: the THINK C debugger does something funny with the
 * color table(s) or palettes or something so that one has to 
 * reboot between debugging runs or the correlation between this
 * coding and the on screen behavior is a hard 0!!!!!!!
 *
 * Don't know if this is an issue anymore.
 */
        if (dpm->pixelSize <= 8)
           {for (i = 0; i < k; i++)
                {Index2Color((long) bf[i], &pixcolor);
                 val      = Color2Index(&pixcolor);
                 bf[i]    = val;};}
        else
           {(*sct)->ctFlags = 0x4000;
            for (i = 0; i < k; i++)
                bfRGB[i] = dcs[bf[i]].rgb;};

        RGBForeColor(&black);
        RGBBackColor(&white);

        CopyBits((BitMap *) spm, (BitMap *) dpm,
                 &srect, &drect, srcCopy, NULL);

        spm->pmTable = sct;

        if (dpm->pixelSize > 8)
           {CFREE(dcs);
            CFREE(bfRGB);};

        DisposePixMap(shbits);}

    else
       {int bx;
        BitMap *spm, *dpm;
        unsigned char *bm;

        bx = _PG_byte_bit_map(bf, nx, ny, FALSE);

        spm = CMAKE(BitMap);
        spm->baseAddr = (char *) bf;
        spm->rowBytes = bx;
        spm->bounds   = srect;

        dpm = &(win->portBits);

        CopyBits(spm, dpm, &srect, &drect, srcCopy, NULL);

        CFREE(spm);};

    return;}

#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_GET_IMAGE - get the image on the screen into
 *                  - the image buffer
 */

void _PG_qd_get_image(PG_device *dev, unsigned char *bf,
		      int ix, int iy, int nx, int ny)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

