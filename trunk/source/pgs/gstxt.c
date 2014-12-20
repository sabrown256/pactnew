/*
 * GSTXT.C - PGS text sub-window routines
 *         - THREADSAFE
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
  
#include "pgs_int.h"

#define CTL_A '\001'
#define CTL_B '\002'
#define CTL_D '\004'
#define CTL_E '\005'
#define CTL_F '\006'
#define CTL_H '\010'
#define CTL_J '\012'
#define CTL_K '\013'
#define CTL_M '\015'
#define CTL_N '\016'
#define CTL_P '\020'
#define DEL   '\177'

SC_thread_lock
 PG_text_lock = SC_LOCK_INIT_STATE;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static INLINE void _PG_place_text(PG_device *dev, char *p)
   {double dx[PG_SPACEDM];

    if (dev != NULL)
       {dx[0] = dev->tcur[0];
        dx[1] = dev->tcur[1];

        PG_move_tx_abs_n(dev, dx);

        PG_get_text_ext_n(dev, 2, WORLDC, p, dx);
        dev->tcur[0] += dx[0];

        PG_write_text(dev, PG_gs.stdscr, p);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_OPEN_TEXT_BOX - open up a special region in which to enter and
 *                  - display text
 */
 
PG_text_box *PG_open_text_box(PG_device *dev, char *name, int type,
			      PFKeymap *keymap,
			      double xf, double yf, double dxf, double dyf)
   {double cbx[PG_BOXSZ], co[PG_SPACEDM];
    PG_curve *crv;
    PG_text_box *tb;

    co[0] = 0.0;
    co[1] = 0.0;

    cbx[0] = xf;
    cbx[1] = xf + dxf;
    cbx[2] = yf;
    cbx[3] = yf + dyf;

    crv = PG_make_box_curve(dev, NORMC, co, cbx);

    tb = PG_open_text_rect(dev, name, type, keymap, crv, 0.1, TRUE);

    return(tb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_CLOSE_TEXT_BOX - close a text box */
 
void PG_close_text_box(PG_text_box *b)
   {int i, ln;
    char **bf;

    ln = b->n_lines;
    bf = b->text_buffer;
    for (i = 0; i < ln; i++)
        {CFREE(bf[i]);};

    CFREE(bf);

    PG_release_curve(b->bnd);

    CFREE(b);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_RESIZE_TEXT_BOX - resize the text box to the specified new
 *                    - NDC limits
 */

void PG_resize_text_box(PG_text_box *b, double *cbx, int n_lines)
   {int i, j, off, onl, oln, nln;
    int n_linep, n_chars;
    char **of, **nf;
    double line_frac;
    double co[PG_SPACEDM];
    PG_device *dev;
    PG_curve *crv;

    if (b == NULL)
       return;

    dev = b->dev;
    if (dev == NULL)
       return;

    crv = b->bnd;

    PG_get_text_ext_n(dev, 2, NORMC, "s", co);

    n_linep = ABS(cbx[2] - cbx[3])/co[1];
    n_chars = ABS(cbx[0] - cbx[1])/co[0];

    n_linep = max(1, n_linep);
    n_chars = max(1, n_chars);

/* keep bottom line well away from the margin */
    line_frac = n_linep*(dev->char_height_s);
    if (line_frac > 0.99)
       n_linep--;
    
/* change the number of lines */
    onl = b->n_lines;
    if (n_lines != onl)
       {off = (n_lines < onl) ? onl - n_lines : 0;
	of  = b->text_buffer;
        nf  = CMAKE_N(char *, n_lines);
	for (i = 0; i < n_lines; i++)
	    {j = i + off;
	     nf[i] = CMAKE_N(char, n_chars);
	     if (j < onl)
	        strcpy(nf[i], of[j]);};}

    else
       nf = b->text_buffer;

    oln = b->line;
    nln = min(oln, onl);

/* resize the bounding curve */
    if (crv != NULL)
       PG_release_curve(crv);

    co[0] = cbx[0];
    co[1] = cbx[2];

    crv = PG_make_box_curve(dev, NORMC, co, cbx);

/* set the new state */
    b->bnd          = crv;
    b->line         = nln;
    b->n_lines      = n_lines;
    b->n_chars_line = n_chars;
    b->n_lines_page = n_linep;
    b->text_buffer  = nf;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CLEAR_TEXT_BOX - clear the current page
 *                    - and go to the line i on the screen
 */
 
static void _PG_clear_text_box(PG_text_box *b, int i)
   {int j, ln, cn;
    int lclr, fclr, bgc;
    char **bf;
    double bwid;
    PG_device *dev;
    PG_curve *crv;

    if (b == NULL)
       return;

    dev = b->dev;
    crv = b->bnd;

    bwid = b->border;
    bgc  = b->background;
    bf   = b->text_buffer;
    ln   = b->n_lines;
    cn   = b->n_chars_line;

/* clear the text buffers */
    for (j = 0; j < ln; j++)
        memset(bf[j], 0, cn);

    b->col  = 0;
    b->line = i;

/* redisplay iff text box is active */
    if ((dev != NULL) && (b->active))
       {lclr = PG_fget_line_color(dev);
	fclr = PG_fget_fill_color(dev);

	if (HARDCOPY_DEVICE(dev))
	   PG_fset_fill_color(dev, bgc, FALSE);
	else
	   PG_fset_fill_color(dev, bgc, TRUE);
	PG_fill_curve(dev, crv);

	if (bwid > 0.0)
	   {double wid;

	    wid = PG_fget_line_width(dev);
	    PG_fset_line_width(dev, bwid);
	    PG_fset_line_color(dev, bgc, FALSE);

	    PG_draw_curve(dev, crv, FALSE);

	    PG_fset_line_width(dev, wid);};

	PG_update_vs(dev);

	PG_fset_fill_color(dev, fclr, FALSE);
	PG_fset_line_color(dev, lclr, FALSE);

	PG_release_current_device(dev);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_MOVE_TO - move to the specified line and column of a text box */

static void _PG_move_to(PG_text_box *b, int c, int l)
   {int so, xr, yr, xl, yl;
    int ix[PG_SPACEDM], xo[PG_SPACEDM];
    int *x[PG_SPACEDM];
    double dx, sign, ang;
    double dxe[PG_SPACEDM], dxp[PG_SPACEDM], f[PG_SPACEDM];
    char *bf;
    PG_device *dev;
    PG_curve *bnd;
    PG_textdes *td;

    if ((b != NULL) && (b->active))
       {dev = b->dev;
	if (dev == NULL)
	   return;

	td = &b->desc;

	sign = 1.0;
	bf   = b->text_buffer[l];
	ang  = td->angle;

	PG_get_text_ext_n(dev, 2, NORMC, bf, dxe);
	PG_get_text_ext_n(dev, 2, NORMC, bf+c, dxp);

	dx = dxe[0]*PG_window_width(dev);

	bnd = b->bnd;
	so  = b->standoff;

	x[0] = bnd->x;
	x[1] = bnd->y;

	xl = x[0][0];
	xr = x[0][1];
	yl = x[1][0];
	yr = x[1][3];

	xo[0] = bnd->x_origin + xl;
	if (ABS(sin(ang)) > sin(PI/4.0))
	   xo[1] = bnd->y_origin + yl;
	else
	   xo[1] = bnd->y_origin + yr - 0.8*sign*dxe[1]*PG_window_height(dev);

	switch (td->align)
	   {default   :
	    case DIR_LEFT :
	         ix[0] = xo[0] + cos(ang)*so;
	         ix[1] = xo[1] + sign*sin(ang)*so;
	         break;

	    case DIR_CENTER :
                 ix[0] = xo[0] + cos(ang)*0.5*(xr - xl - dx);
	         ix[1] = xo[1] + sign*sin(ang)*0.5*(ABS(yr - yl) - dx);
	         break;

	    case DIR_RIGHT :
	         ix[0] = xo[0] + xr - xl - cos(ang)*(so + dx);
	         ix[1] = xo[1] + sign*sin(ang)*(ABS(yr - yl) - dx - so);
	         break;};

	f[0] = ix[0];
	f[1] = ix[1];
	PG_trans_point(dev, 2, PIXELC, f, NORMC, f);

	f[0] += cos(ang)*(dxe[0] - dxp[0]) + sin(ang)*(l*dxe[1]);
	f[1] += sin(ang)*(dxe[0] - dxp[0]) - cos(ang)*(l*dxe[1]);
	PG_trans_point(dev, 2, NORMC, f, WORLDC, f);

	PG_move_tx_abs_n(dev, f);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_REFRESH_TEXT_BOX - redraw all lines of text in the box
 *                     - do a clear first
 */

void PG_refresh_text_box(PG_text_box *b)
   {int i, ln, sz;
    double ang, bwid;
    double x[PG_SPACEDM], o[PG_SPACEDM], dc[PG_SPACEDM];
    char *face, *sty, *tfc, **bf;
    PG_device *dev;
    PG_curve *crv;
    PG_palette *pl;
    PG_textdes *td;

    if ((b != NULL) && (b->active))
       {dev = b->dev;
	if (dev == NULL)
	   return;

	td = &b->desc;

	crv = b->bnd;
	ang = td->angle;
	if (ang != 0)
	   b->n_lines = 1;

	ln = b->n_lines;
	bf = b->text_buffer;

	dc[0] = cos(ang);
	dc[1] = sin(ang);

	pl = dev->current_palette;
	PG_fset_palette(dev, "standard");

	if (HARDCOPY_DEVICE(dev))
	   PG_fset_fill_color(dev, b->background, FALSE);
	else
	   PG_fset_fill_color(dev, b->background, TRUE);

/* update the pixel location of curve (window may have changed) */
	o[0] = crv->rx_origin;
	o[1] = crv->ry_origin;
	PG_trans_point(dev, 2, NORMC, o, PIXELC, o);
	crv->x_origin = o[0];
	crv->y_origin = o[1];

	PG_fill_curve(dev, crv);

	bwid = b->border;
	if (bwid > 0.0)
	   {double wid;

	    wid = PG_fget_line_width(dev);
	    PG_fset_line_width(dev, bwid);
	    (*dev->set_line_color)(dev, td->color, FALSE);

	    PG_draw_curve(dev, crv, FALSE);

	    PG_fset_line_width(dev, wid);};

	PG_fget_char_path(dev, x);
	PG_fget_font(dev, &face, &sty, &sz);

        tfc = td->face;
	if ((tfc == NULL) || (strcmp(tfc, "nil") == 0))
	   tfc = face;

	PG_fset_char_path(dev, dc);
	PG_fset_font(dev, tfc, td->style, td->size);
	PG_fset_text_color(dev, td->color, FALSE);

	for (i = 0; i < ln; i++)
	    {_PG_move_to(b, 0, i);
	     PG_write_text(dev, PG_gs.stdscr, bf[i]);};

	PG_fset_char_path(dev, x);
	PG_fset_font(dev, face, sty, sz);

	dev->current_palette = pl;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_INSERT_TEXT - insert text into the given string
 *                 - return the overflow
 */

static char *_PG_insert_text(char *bf, int ncpl, int c, char *s)
   {int i, n, nb, ns, cl;
    char *tmp, *p, *q, *overflow;

    nb = strlen(bf);
    ns = strlen(s);

    tmp = CMAKE_N(char, ncpl+ns);
    strcpy(tmp, bf);

    p = tmp + nb;
    q = p - ns;
    n = nb - c;
    for (i = 0; i < n; i++, *p-- = *q--);

    memcpy(tmp+c, s, ns);
    tmp[nb+ns] = '\0';

    cl = ns + nb - ncpl;
    if (cl > 0)
       {strcpy(s, tmp+ncpl);
	memcpy(bf, tmp, ncpl);
        bf[ncpl] = '\0';
        overflow = s;}

    else
       {strcpy(bf, tmp);
        overflow  = NULL;};

    CFREE(tmp);

    return(overflow);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CLEAR_LINE - clear the line to the end */

static void _PG_clear_line(PG_text_box *b, int l)
   {char *bf;
    PG_device *dev;

    if ((b != NULL) && (b->active))
       {dev = b->dev;
	if (dev != NULL)
	   {bf = b->text_buffer[l];
	    if (bf != NULL)
	       {_PG_move_to(b, 0, l);
		PG_fset_text_color(dev, b->background, FALSE);
		PG_write_text(dev, PG_gs.stdscr, bf);};};};

    return;}    

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DRAW_CURSOR - draw the cursor in the inverse video or normal */

void _PG_draw_cursor(PG_text_box *b, int inv)
   {int c, l, fc, bc, eol;
    double dx[PG_SPACEDM], xo[PG_SPACEDM], n[PG_SPACEDM];
    double ndc[PG_BOXSZ];
    char *bf, crsr[2];
    PG_curve *crv;
    PG_device *dev;
    PG_textdes *td;

    if ((b != NULL) && (b->active))

/* it is possible for the PG_text_box to be GC'd so test the device
 * and just leave if its gone
 */
       {dev = b->dev;
	if (dev == NULL)
	   return;

	c = b->col;
	l = b->line;

	td = &b->desc;
	bf = b->text_buffer[l];

	crsr[0] = bf[c];
	crsr[1] = '\0';
	if (crsr[0] == '\0')
	   {eol = TRUE;
	    crsr[0] = 's';}
	else
	   eol = FALSE;

	_PG_move_to(b, c, l);
	PG_get_text_ext_n(dev, 2, NORMC, crsr, dx);

	xo[0] = 0.0;
	xo[1] = 0.0;

	ndc[0] = dev->tcur[0];
	ndc[2] = dev->tcur[1];

	n[0] = ndc[0];
	n[1] = ndc[2];
	PG_trans_point(dev, 2, WORLDC, n, NORMC, n);	

	ndc[2] -= 0.3*dx[1];

	ndc[1] = ndc[0] + dx[0];
	ndc[3] = ndc[2] + dx[1];
	crv = PG_make_box_curve(dev, NORMC, xo, ndc);

	if (inv)
	   {fc = td->color;
	    bc = b->background;}
	else
	   {fc = b->background;
	    bc = td->color;};

	PG_fset_fill_color(dev, fc, TRUE);
	PG_fill_curve(dev, crv);
	PG_fset_text_color(dev, bc, FALSE);

	if (!eol)
	   PG_write_text(dev, PG_gs.stdscr, crsr);

	PG_release_curve(crv);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_REDRAW_TEXT_LINE - draw a proper image of the text line */

static void _PG_redraw_text_line(PG_text_box *b, int c, int l, int flag)
   {char *bf;
    PG_device *dev;
    PG_textdes *td;

    if ((b != NULL) && (b->active))
       {dev = b->dev;
	if (dev != NULL)
	   {td = &b->desc;
	    bf = b->text_buffer[l];

	    _PG_clear_line(b, l);
	    _PG_move_to(b, 0, l);
	    PG_fset_text_color(dev, td->color, FALSE);
	    PG_write_text(dev, PG_gs.stdscr, bf);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WRITE_TEXT - write out text to the appropriate device */
 
static void _PG_write_text(PG_text_box *b, char *s)
   {int col, ln, lmx, ncpl, cr, cl, ns, fl;
    char *bf, *overflow;

    if ((b == NULL) || (s == NULL))
       return;

    col  = b->col;
    ln   = b->line;
    lmx  = b->n_lines_page;
    ncpl = b->n_chars_line;
    ns   = strlen(s);

    switch (b->mode)
       {default          :
        case TEXT_INSERT :
	     for (overflow = s;
		  (overflow != NULL) && (ln < lmx);
		  col = 0, ln++)
	         {_PG_clear_line(b, ln);
		  bf       = b->text_buffer[ln];
		  overflow = _PG_insert_text(bf, ncpl, col, overflow);};

             col = b->col + ns;
             ln  = b->line;
             for (fl = FALSE;
		  (col >= ncpl) && (ln < lmx);
		  col -= ncpl, ln++, fl = TRUE)
                 _PG_redraw_text_line(b, 0, ln, fl);

	     b->col = col;
             if (ln < lmx)
	        {_PG_redraw_text_line(b, col, ln, TRUE);
		 b->line = ln;};
	     break;

        case TEXT_OVERWRITE :
             bf = b->text_buffer[ln];
             cl = ncpl - col;
	     if (ns <= cl)
	        {strcpy(bf+col, s);
		 _PG_redraw_text_line(b, col, ln, FALSE);
		 b->col = col + ns;}
	     else
	        {strncpy(bf+col, s, cl);
		 _PG_redraw_text_line(b, col, ln, FALSE);

		 bf = b->text_buffer[++(b->line)];
		 cr = ns - cl;
		 strncpy(bf, s+cl, cr);
		 _PG_redraw_text_line(b, cr, ++ln, TRUE);

		 b->col = cr;};

	     break;};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_NEWLINE - do a controlled newline */
 
static void _PG_newline(PG_text_box *b)
   {int i, line, nl, nc;
    char *p0, *p, **bf;
    PG_device *dev;
    PG_textdes *td;

    dev  = b->dev;
    td   = &b->desc;
    line = b->line++;
    bf   = b->text_buffer;
    nl   = b->n_lines_page - 2;
    nc   = b->n_chars_line;

/* this is for a MORE window */
    if (b->type == MORE_WINDOW)
       {if (line < nl)
	   line++;
	else
	   {nl++;
            _PG_move_to(b, 0, nl);
	    PG_write_text(dev, PG_gs.stdscr, "More ...");
            PG_update_vs(dev);
	    PG_get_char(dev);

	    PG_fset_fill_color(dev, b->background, TRUE);
	    PG_fill_curve(dev, b->bnd);

	    for (i = 0; i < nl; i++)
	        memset(bf[i], 0, nc);
	    line = 0;};}

/* this is for a SCROLLING window */
    else if (b->type == SCROLLING_WINDOW)
       {if (line < b->n_lines_page-1)
	   line++;
        else
	   {p0 = bf[0];
	    memset(p0, 0, nc);

	    if (b->active)
	       {PG_fset_fill_color(dev, b->background, TRUE);
		PG_fill_curve(dev, b->bnd);

		PG_fset_fill_color(dev, td->color, TRUE);};

	    for (i = 0; i < line; i++)
	        {_PG_move_to(b, 0, i);
		 p = bf[i] = bf[i+1];
		 if ((p != NULL) && b->active)
		    PG_write_text(dev, PG_gs.stdscr, p);};

	    bf[line] = p0;
	    line = nl+1;};}

    else
       {nl += 2;
        line = min(line, nl);};
    
    b->line = line;
    b->col  = 0;

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_BACKUP_CHAR - move the text cursor back the
 *                 - specified number of characters in the buffer
 *                 - pointed to by p
 */

static char *_PG_backup_char(PG_text_box *b, char *p, int n)
   {double dx[PG_SPACEDM], x[PG_SPACEDM];
    PG_device *dev;

    if (b != NULL)
       {dev = b->dev;
	if (dev != NULL)
	   {p -= n;

	    if (b->active)
	       {PG_get_text_ext_n(dev, 2, WORLDC, p, dx);

		x[0] = -dx[0];
		x[1] = 0.0;
		PG_move_tx_rel_n(dev, x);

		PG_fset_text_color(dev, b->background, FALSE);
		PG_write_text(dev, PG_gs.stdscr, p);

		x[0] = 0.0;
		x[1] = 0.0;
		PG_move_tx_rel_n(dev, x);};};};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_TEXT_GETS - do fgets for a text box */
 
static char *_PG_text_gets(PG_text_box *b, char *bfr, int nc, FILE *stream)
   {int col, line, sc;
    char c, eof, tmp[MAXLINE];
    char *p, *ps, **bf;
    static char prompt = '_', cr = 13, bs = 8;

    if (b == NULL)
       return(NULL);

    p   = (bfr != NULL) ? bfr : tmp;
    ps  = p;
    eof = (char) EOF;
 
    if (stream == stdin)
       {PG_device *dev;

        dev  = b->dev;
	col  = b->col;
        line = b->line;
        bf   = b->text_buffer;

        p[0] = prompt;
        p[1] = '\0';

	sc = col;

	col--;
	col = max(col, 0);
	_PG_move_to(b, col, line);
        _PG_place_text(dev, p);
        p++;

        while (((c = PG_get_char(dev)) != eof) && (c != cr))
           {if (c != bs)
               {p    = _PG_backup_char(b, p, 1);
                p[0] = (c == '\n') ? ';' : c;
                p[1] = prompt;
                p[2] = '\0';

		_PG_place_text(dev, p);

                p += 2;}
            else if (p > ps+1)
               {p    = _PG_backup_char(b, p, 2);
                p[0] = prompt;
                p[1] = '\0';

		_PG_place_text(dev, p);

                p++;};};

        p = _PG_backup_char(b, p, 1);

        if (c == cr)
           {*p++ = '\n';
            *p = '\0';

	    if (bfr != NULL)
	       {p = b->text_buffer[b->line] + sc;
		strcpy(bfr, p);}
	    else
	       p = bf[line];

            _PG_newline(b);};
                     
        if (c == eof)
           p = NULL;}

    else
       p = io_gets(p, nc, stream);
 
    return(p);}

/*--------------------------------------------------------------------------*/

/*                            EDITING COMMANDS                              */

/*--------------------------------------------------------------------------*/

/* _PG_BACKWARD_CHARS - move backward n characters */

static int _PG_backward_chars(PG_text_box *b, int n)
   {int oln, ocol, nln, ncol;

    oln  = b->line;
    ocol = b->col;
    nln  = oln;
    ncol = ocol - n;
    if (ncol < 0)
       {if (oln == 0)
           return(FALSE);

        nln  = oln - 1;
	ncol = strlen(b->text_buffer[nln]) - n + 1;};

    b->line = nln;
    b->col  = ncol;

    _PG_redraw_text_line(b, ncol, nln, TRUE);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DELETE_BACK - delete the character n back from current position */

static void _PG_delete_back(PG_text_box *b, int n)
   {int col, ln;
    char *bf, *p, *s;

    if (_PG_backward_chars(b, n))
       {ln  = b->line;
	col = b->col;
	_PG_clear_line(b, ln);

	bf = b->text_buffer[ln] + col;
	for (p = bf, s = bf + 1; (*p++ = *s++););

	_PG_redraw_text_line(b, col, ln, TRUE);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FIXUP_EOL - move the position back to the end of text if needed */

static void _PG_fixup_eol(PG_text_box *b)
   {int colx, col, ln;

    ln = b->line;
    if (ln < b->n_lines)
       {col  = b->col;
	colx = strlen(b->text_buffer[ln]);
	if (colx < col)
	   b->col = colx;

	_PG_redraw_text_line(b, col, ln, TRUE);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GOTO_BOL - goto the beginning of the line */

static void _PG_goto_bol(PG_text_box *b)
   {int ln;

    ln = b->line;

    b->col = 0;

    _PG_redraw_text_line(b, 0, ln, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GOTO_EOL - goto the end of the line */

static void _PG_goto_eol(PG_text_box *b)
   {int ln, ncol;

    ln = b->line;

    b->col = ncol = strlen(b->text_buffer[ln]);

    _PG_redraw_text_line(b, ncol, ln, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_KILL_EOL - kill to the end of the line */

static void _PG_kill_eol(PG_text_box *b)
   {int col, ln;
    char *bf;

    col = b->col;
    ln = b->line;
    _PG_clear_line(b, ln);

    bf = b->text_buffer[ln];
    bf[col] = '\0';

    _PG_redraw_text_line(b, col, ln, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FORWARD_CHAR - move forward one character */

static void _PG_forward_char(PG_text_box *b)
   {int oln, ocol, nln, ncol, colx;

    ocol = b->col;
    oln  = b->line;
    colx = strlen(b->text_buffer[oln]);
    ncol = ocol + 1;
    nln  = oln;
    if (ncol > colx)
       {nln  = oln + 1;
        ncol = 0;
        if (nln >= b->n_lines)
           return;};

    b->line = nln;
    b->col  = ncol;

    _PG_redraw_text_line(b, ncol, nln, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_BACKWARD_CHAR - move backward one character */

static void _PG_backward_char(PG_text_box *b)
   {

    _PG_backward_chars(b, 1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_NEXT_LINE - move to the next line */

static void _PG_next_line(PG_text_box *b)
   {int ln, lnx;

    ln  = ++(b->line);
    lnx = b->n_lines;
    if (ln >= lnx)
       b->line = lnx - 1;

    _PG_fixup_eol(b);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PREVIOUS_LINE - move to the previous line */

static void _PG_previous_line(PG_text_box *b)
   {int ln;

    ln = --(b->line);
    if (ln < 0)
       b->line = 0;

    _PG_fixup_eol(b);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DELETE_CURRENT - delete the current character */

static void _PG_delete_current(PG_text_box *b)
   {

    _PG_delete_back(b, 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DELETE_PREVIOUS - delete the previous character */

static void _PG_delete_previous(PG_text_box *b)
   {

    _PG_delete_back(b, 1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_OPEN_TEXT_RECT - open up a special region in which to enter and
 *                   - display text
 */
 
PG_text_box *PG_open_text_rect(PG_device *dev, char *name, int type,
			       PFKeymap *keymap, PG_curve *crv,
			       double brd, int ndrw)
   {int i, ni, n_linep, n_lines, n_chars;
    char **bf;
    double line_frac;
    double dx[PG_SPACEDM], bx[PG_BOXSZ];
    PG_textdes *td;
    PG_text_box *b;

    if (dev == NULL)
       return(NULL);

    if (_PG.std_keymap == NULL)
       {SC_LOCKON(PG_text_lock);
	if (_PG.std_keymap == NULL)
	   {ni = 256;
	    _PG.std_keymap = CMAKE_N(PFKeymap, ni);

	    if (SC_zero_on_alloc_n(-1) == FALSE)
	       SC_MEM_INIT_N(PFVoid, _PG.std_keymap, ni);

	    _PG.std_keymap[CTL_A] = _PG_goto_bol;
	    _PG.std_keymap[CTL_B] = _PG_backward_char;
	    _PG.std_keymap[CTL_D] = _PG_delete_current;
	    _PG.std_keymap[CTL_E] = _PG_goto_eol;
	    _PG.std_keymap[CTL_F] = _PG_forward_char;
	    _PG.std_keymap[CTL_H] = _PG_delete_previous;
	    _PG.std_keymap[CTL_J] = _PG_newline;
	    _PG.std_keymap[CTL_K] = _PG_kill_eol;
	    _PG.std_keymap[CTL_M] = _PG_newline;
	    _PG.std_keymap[CTL_N] = _PG_next_line;
	    _PG.std_keymap[CTL_P] = _PG_previous_line;
	    _PG.std_keymap[DEL]   = _PG_delete_previous;};
	SC_LOCKOFF(PG_text_lock);};

    b = CMAKE(PG_text_box);

    PG_get_text_ext_n(dev, 2, NORMC, "s", dx);
    PG_get_curve_extent(dev, crv, NORMC, bx);
    n_linep = ABS(bx[2] - bx[3])/dx[1];
    n_chars = ABS(bx[0] - bx[1])/dx[0];

    n_linep = max(1, n_linep);
    n_chars = max(1, n_chars);

/* keep bottom line well away from the margin */
    line_frac = n_linep*(dev->char_height_s);
    if (line_frac > 0.99)
       n_linep--;
    
/* this might want to be different at some point */
    n_lines = n_linep;

    b->name         = CSTRSAVE(name);
    b->type         = type;
    b->active       = TRUE;
    b->dev          = dev;
    b->bnd          = crv;
    b->line         = 0;
    b->col          = 0;
    b->mode         = TEXT_INSERT;
    b->background   = dev->BLACK;
    b->border       = brd;
    b->standoff     = 5;
    b->n_lines      = n_lines;
    b->n_chars_line = n_chars;
    b->n_lines_page = n_linep;
    b->text_buffer  = bf = CMAKE_N(char *, n_lines);

    td = &b->desc;
    td->color = dev->WHITE;
    td->align = DIR_LEFT;
    td->angle = 0.0;

    for (i = 0; i < n_lines; i++)
        bf[i] = CMAKE_N(char, n_chars);

    if (keymap == NULL)
       b->keymap = _PG.std_keymap;
    else
       b->keymap = keymap;

    b->backup  = _PG_backup_char;
    b->gets    = _PG_text_gets;
    b->clear   = _PG_clear_text_box;
    b->write   = _PG_write_text;
    b->newline = _PG_newline;

    if (ndrw)
       _PG_clear_text_box(b, 0);

    return(b);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_COPY_TEXT_BOX - copy a text box object B */

PG_text_box *PG_copy_text_object(PG_text_box *ib,
				 PG_device *dvd, PG_device *dvs)
   {int i, nl, nc;
    char **bfo, **bfi;
    PG_curve *icrv, *ocrv;
    PG_text_box *ob;

    if (ib == NULL)
       return(NULL);

    icrv = ib->bnd;
    bfi  = ib->text_buffer;
    nl   = ib->n_lines;
    nc   = ib->n_chars_line;

    ocrv = PG_copy_curve(icrv, dvd, dvs);

    ob = PG_open_text_rect(dvd, ib->name, ib->type, ib->keymap,
			   ocrv, ib->border, FALSE);

    if (ob != NULL)
       {ob->line         = ib->line;
	ob->col          = ib->col;
	ob->mode         = ib->mode;
	ob->background   = ib->background;
	ob->standoff     = ib->standoff;
	ob->n_chars_line = nc;

	ob->desc = ib->desc;

/* copy the strings from the input buffer */
	bfo = ob->text_buffer;
	for (i = 0; i < nl; i++)
	    {CFREE(bfo[i]);
	   
	     bfo[i] = CMAKE_N(char, nc);

	     strcpy(bfo[i], bfi[i]);};};

    return(ob);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_IS_INACTIVE_TEXT_BOX - return TRUE iff interface object
 *                         - is a PG_text_box which is not active
 */

int PG_is_inactive_text_box(PG_interface_object *iob)
   {int flag;
    PG_text_box *b;

    flag = FALSE;

    if (strcmp(iob->type, PG_TEXT_OBJECT_S) == 0)
       {b    = (PG_text_box *) iob->obj;
	flag = (b->active == FALSE);};

    return(flag);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_IS_TEXT_BOX - return TRUE iff interface object
 *                - is a PG_text_box
 */

int PG_is_text_box(PG_interface_object *iob)
   {int flag;

    flag = (strcmp(iob->type, PG_TEXT_OBJECT_S) == 0);

    return(flag);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
