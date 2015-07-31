/*
 * GSACC.C - struct accessors for PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "pgs_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_BUFFER_SIZE - Return the default I/O buffer size.
 *
 * #bind PG_fget_buffer_size fortran() scheme(pg-get-buffer-size) python()
 */

int64_t PG_fget_buffer_size(void)
   {int64_t rv;
    
    rv = _PG.buffer_size;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_BUFFER_SIZE - Set the default I/O buffer size to SZ.
 *                     - If SZ is -1 turn off the default buffering
 *                     - optimization which happens on file opens.
 *                     - Return the old value.
 *
 * #bind PG_fset_buffer_size fortran() scheme(pg-set-buffer-size!) python()
 */

int64_t PG_fset_buffer_size(int64_t sz ARG(-1,in))
   {int64_t rv;
    
    rv = _PG.buffer_size;
    
    _PG.buffer_size = sz;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_CLEAR_MODE - Return the high level PGS window clear mode.
 *
 * #bind PG_fget_clear_mode fortran() scheme(pg-get-clear-mode) python()
 */

int PG_fget_clear_mode(void)
   {int i;

    PG_get_attrs_glb(TRUE, "hl-clear-mode", &i, NULL);

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_CLEAR_MODE - Set the high level PGS window clear mode to I.
 *
 * #bind PG_fset_clear_mode fortran() scheme(pg-set-clear-mode!) python()
 */

int PG_fset_clear_mode(int i ARG(CLEAR_SCREEN))
   {

    PG_set_attrs_glb(TRUE, "hl-clear-mode", i, NULL);

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_CLIPPING - Return the value of the clipping flag of device DEV.
 *
 * #bind PG_fget_clipping fortran() scheme(pg-get-clipping) python()
 */

bool PG_fget_clipping(const PG_device *dev ARG(,in,cls))
   {bool rv;
 
    rv = (dev == NULL) ? FALSE : dev->clipping;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_CLIPPING - Set the value of the clipping flag of device DEV
 *                  - to FLAG.
 *                  - Return the old value.
 *
 * #bind PG_fset_clipping fortran() scheme(pg-set-clipping!) python()
 */

bool PG_fset_clipping(PG_device *dev ARG(,in,cls),
		      bool flag ARG(TRUE))
   {bool rv;

    if ((dev != NULL) && (dev->set_clipping != NULL))
       {rv = dev->clipping;
	dev->set_clipping(dev, flag);}
    else
       rv = flag;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_CHAR_PATH - Return the char_path direction of device DEV in X.
 *                   - The direction char_path defines the line along which
 *                   - sequences of characters are rendered on DEV.
 *                   - The default is (1, 0) and if DEV is NULL the default
 *                   - is returned.
 *
 * #bind PG_fget_char_path fortran() scheme(pg-get-char-path) python()
 */

void PG_fget_char_path(const PG_device *dev ARG(,in,cls),
		       double *x ARG([*,*],out))
   {

    if (dev != NULL)
       {x[0] = dev->char_path[0];
	x[1] = dev->char_path[1];}
    else
       {x[0] = 1.0;
	x[1] = 0.0;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_CHAR_PATH - Set the char_path direction for device DEV to the
 *                   - values in vector X.
 *                   - The direction char_path defines the line along which
 *                   - sequences of characters are rendered on DEV.
 *                   - The old value is returned in X.
 *
 * #bind PG_fset_char_path fortran() scheme(pg-set-char-path!) python()
 */

void PG_fset_char_path(PG_device *dev ARG(,in,cls),
		       double *x ARG([1.0,0.0],io))
   {double xo[PG_SPACEDM];

    if ((dev != NULL) && (dev->set_char_path != NULL))
       {xo[0] = dev->char_path[0];
	xo[1] = dev->char_path[1];

	dev->set_char_path(dev, x[0], x[1]);

	x[0] = xo[0];
	x[1] = xo[1];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_CHAR_UP - Return the char_up direction of device DEV in X.
 *                 - The direction char_up defines up for a character
 *                 - rendered on DEV.
 *                 - The default is (0, 1) and if DEV is NULL the default
 *                 - is returned.
 *
 * #bind PG_fget_char_up fortran() scheme(pg-get-char-up) python()
 */

void PG_fget_char_up(const PG_device *dev ARG(,in,cls),
		     double *x ARG([*,*],out))
   {

    if (dev != NULL)
       {x[0] = dev->char_up[0];
	x[1] = dev->char_up[1];}
    else
       {x[0] = 0.0;
	x[1] = 1.0;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_CHAR_UP - Set the char_up direction for device DEV to the
 *                 - values in vector X.
 *                 - The direction char_up defines up for a character
 *                 - rendered on DEV.
 *                 - The old value is returned in X.
 *
 * #bind PG_fset_char_up fortran() scheme(pg-set-char-up!) python()
 */

void PG_fset_char_up(PG_device *dev ARG(,in,cls),
		     double *x ARG([0.0,1.0],io))
   {double xo[PG_SPACEDM];

    if ((dev != NULL) && (dev->set_char_up != NULL))
       {xo[0] = dev->char_up[0];
	xo[1] = dev->char_up[1];

	dev->set_char_up(dev, x[0], x[1]);

	x[0] = xo[0];
	x[1] = xo[1];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_CHAR_PRECISION - Return the character precision for device DEV.
 *                        - The character precision defaults to 1.
 *
 * #bind PG_fget_char_precision fortran() scheme(pg-get-char-precision) python()
 */

int PG_fget_char_precision(const PG_device *dev ARG(,in,cls))
   {int rv;

    if (dev != NULL)
       rv = dev->char_precision;
    else
       rv = 1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_CHAR_PRECISION - Set the character precision for device DEV to P.
 *                        - The old value is returned.
 *
 * #bind PG_fset_char_precision fortran() scheme(pg-set-char-precision!) python()
 */

int PG_fset_char_precision(PG_device *dev ARG(,in,cls),
			   int p ARG(1,in))
   {int rv;

    if ((dev != NULL) && (dev->set_char_precision != NULL))
       {rv = dev->char_precision;
	dev->set_char_precision(dev, p);}
    else
       rv = 1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_CHAR_SIZE_N - Return the character size of device DEV in P.
 *                     - P is given in coordinate system CS and
 *                     - P[0] is the character height while P[1] is
 *                     - the character width. ND should be 2.
 *
 * #bind PG_fget_char_size_n fortran() scheme() python()
 */

void PG_fget_char_size_n(const PG_device *dev ARG(,in,cls),
			 int nd, PG_coord_sys cs,
			 double *p ARG([0.0,0.0],out))
   {

    if (dev != NULL)
       {p[0] = dev->char_height_s;
	p[1] = dev->char_width_s;}
    else
       {p[0] = 0.0;
	p[1] = 0.0;};

    PG_trans_point(dev, nd, NORMC, p, cs, p);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_CHAR_SIZE_N - Set the character size of device DEV from P.
 *                     - P is given in coordinate system CS and
 *                     - P[0] is the character height while P[1] is
 *                     - the character width. ND should be 2.
 *
 * #bind PG_fset_char_size_n fortran() scheme() python()
 */

void PG_fset_char_size_n(PG_device *dev ARG(,in,cls),
			 int nd, PG_coord_sys cs,
			 double *p ARG([0.0,0.0],io))
   {double po[PG_SPACEDM];

    if ((dev != NULL) && (dev->set_char_size != NULL))
       {po[0] = dev->char_width_s;
	po[1] = dev->char_height_s;

	dev->set_char_size(dev, nd, cs, p);

	p[0] = po[0];
	p[0] = po[1];}
    else
       {p[0] = 0.0;
	p[0] = 0.0;};

    PG_trans_point(dev, nd, NORMC, p, cs, p);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_CHAR_SPACE - Return the space between characters for
 *                    - the device DEV.  The default value, 0.0, is returned
 *                    - if DEV is NULL.
 *
 * #bind PG_fget_char_space fortran() scheme(pg-get-char-space) python()
 */

double PG_fget_char_space(const PG_device *dev ARG(,in,cls))
   {double d;

    if (dev != NULL)
       d = dev->char_space;
    else
       d = 0.0;

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_CHAR_SPACE - Set the space between characters for device DEV
 *                    - to D.  Return the old value.
 *
 * #bind PG_fset_char_space fortran() scheme(pg-set-char-space!) python()
 */

double PG_fset_char_space(PG_device *dev ARG(,in,cls),
			  double d ARG(0.0,in))
   {double rv;

    if ((dev != NULL) && (dev->set_char_space != NULL))
       {rv = dev->char_space;
	dev->set_char_space(dev, d);}
    else
       rv = 0.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_FILL_BOUND - Return draw_fill_bound flag for the device DEV.
 *                    - The default value, FALSE, is returned if DEV is NULL.
 *
 * #bind PG_fget_fill_bound fortran() scheme(pg-get-fill-bound) python()
 */

int PG_fget_fill_bound(const PG_device *dev ARG(,in,cls))
   {int rv;

    if (dev != NULL)
       rv = dev->draw_fill_bound;
    else
       rv = FALSE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_FILL_BOUND - Set the draw_fill_bound flag for the device DEV.
 *                    - The old value is returned if DEV is not NULL and
 *                    - the default value, FALSE, otherwise.
 *
 * #bind PG_fset_fill_bound fortran() scheme(pg-set-fill-bound!) python()
 */

int PG_fset_fill_bound(PG_device *dev ARG(,in,cls),
		       int v ARG(FALSE,in))
   {int rv;

    if (dev != NULL)
       {rv = dev->draw_fill_bound;
	dev->draw_fill_bound = v;}
    else
       rv = FALSE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_FINISH_STATE - Return the finished flag for the device DEV.
 *                      - The finished flag says that no further elements
 *                      - will be rendered to the current window of DEV
 *                      - and devices which need to put standard page
 *                      - terminating elements (for example PostScript)
 *                      - may do so.
 *                      - The default value, TRUE, is returned if DEV is NULL.
 *
 * #bind PG_fget_finish_state fortran() scheme(pg-get-finish-state) python()
 */

int PG_fget_finish_state(const PG_device *dev ARG(,in,cls))
   {int rv;

    if (dev != NULL)
       rv = dev->finished;
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_FINISH_STATE - Set the finished flag for the device DEV.
 *                      - The finished flag says that no further elements
 *                      - will be rendered to the current window of DEV
 *                      - and devices which need to put standard page
 *                      - terminating elements (for example PostScript)
 *                      - may do so.
 *                      - The old value is returned if DEV is non-NULL.
 *                      - The default value, TRUE, is returned if DEV is NULL.
 *
 * #bind PG_fset_finish_state fortran() scheme(pg-set-finish-state!) python()
 */

int PG_fset_finish_state(PG_device *dev ARG(,in,cls),
			 int v ARG(TRUE,in))
   {int rv;

    if (dev != NULL)
       {rv = dev->finished;
	dev->finished = v;}
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_FONT - Return the current font info for the device DEV.
 *              - For each non-NULL argument the following are returned:
 *              -   OF    the type face ("helvetica", "times", or "courier")
 *              -   OST   the type style ("medium", "bold", "italic" or
 *              -                         "bold-italic")
 *              -   OSZ   the type size in points
 *
 * #bind PG_fget_font fortran() scheme() python()
 */

void PG_fget_font(const PG_device *dev ARG(,in,cls),
		  char **of, char **ost, int *osz)
   {

    if (of != NULL)
       *of  = CSTRSAVE(dev->type_face);
    if (ost != NULL)
       *ost = CSTRSAVE(dev->type_style);
    if (osz != NULL)
       *osz = dev->type_size;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_FONT - Set the current font info for the device DEV.
 *              - For each non-NULL argument the following are returned:
 *              -   FACE  the type face ("helvetica", "times", or "courier")
 *              -   STYLE the type style ("medium", "bold", "italic" or
 *              -                         "bold-italic")
 *              -   SZ    the type size in points
 *
 * #bind PG_fset_font fortran() scheme() python()
 */

void PG_fset_font(PG_device *dev ARG(,in,cls),
		  const char *face ARG("helvetica",in),
		  const char *style ARG("medium",in),
		  int sz ARG(12,in))
   {

    if ((dev != NULL) && (dev->set_font != NULL))
       {if (face == NULL)
	   face = dev->type_face;
	if (style == NULL)
	   style = dev->type_style;
	if (sz == -1)
	   sz = dev->type_size;

	dev->set_font(dev, face, style, sz);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_IDENTIFIER - Return the identifier of graph G if G is non-NULL
 *                    - and return 'A' otherwise.
 *
 * #bind PG_fget_identifier fortran() scheme(pg-get-identifier) python()
 */

int PG_fget_identifier(const PG_graph *g)
   {int rv;

    if (g != NULL)
       rv = g->identifier;
    else
       rv = 'A';

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_IDENTIFIER - Set the identifier of graph G to ID.
 *                    - Return the old value if G is non-NULL
 *                    - and return 'A' otherwise.
 *
 * #bind PG_fset_identifier fortran() scheme(pg-set-identifier!) python()
 */

int PG_fset_identifier(PG_graph *g, int id ARG('A',in))
   {int rv;

    if (g != NULL)
       {rv = g->identifier;
	g->identifier = id;}
    else
       rv = 'A';
       
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_LOGICAL_OP - Return the device logical operation flag for
 *                    - device DEV.  The default is GS_COPY.
 *                    - If DEV is NULL return the default.
 *
 * #bind PG_fget_logical_op fortran() scheme(pg-get-logical-op) python()
 */

PG_logical_operation PG_fget_logical_op(const PG_device *dev ARG(,in,cls))
   {PG_logical_operation lop;

    if (dev != NULL)
       lop = dev->logical_op;
    else
       lop = GS_COPY;

    return(lop);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_LOGICAL_OP - Set the device logical operation flag for device DEV.
 *                    - Return the old value if DEV is non-NULL and
 *                    - return GS_COPY otherwise.
 *
 * #bind PG_fset_logical_op fortran() scheme(pg-set-logical-op!) python()
 */

PG_logical_operation PG_fset_logical_op(PG_device *dev ARG(,in,cls),
					PG_logical_operation lop ARG(GS_COPY,in))
   {PG_logical_operation rv;

    if ((dev != NULL) && (dev->set_logical_op != NULL))
       {rv = dev->logical_op;
	dev->set_logical_op(dev, lop);}
    else
       rv = GS_COPY;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_LINE_STYLE - Return the line style of device DEV.
 *                    - If DEV is NULL return the global 'line-style' value.
 *                    - The global 'line-style' value defaults to LINE_SOLID.
 *
 * #bind PG_fget_line_style fortran() scheme(pg-get-line-style) python()
 */

int PG_fget_line_style(const PG_device *dev ARG(,in,cls))
   {int st;

    if (dev != NULL)
       st = dev->line_style;
    else
       PG_get_attrs_glb(TRUE, "line-style", &st, NULL);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_LINE_STYLE - Set the line style of device DEV to ST.
 *                    - Return the old value if DEV is non-NULL.
 *                    - If DEV is NULL return the global 'line-style' value.
 *                    - The global 'line-style' value defaults to LINE_SOLID.
 *
 * #bind PG_fset_line_style fortran() scheme(pg-set-line-style!) python()
 */

int PG_fset_line_style(PG_device *dev ARG(,in,cls),
		       int st ARG(LINE_SOLID,in))
   {int rv;

    if ((dev != NULL) && (dev->set_line_style != NULL))
       {rv = dev->line_style;
	dev->set_line_style(dev, st);}
    else
       PG_get_attrs_glb(TRUE, "line-style", &rv, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_LINE_WIDTH - Return the line width of device DEV.
 *                    - If DEV is NULL return the global 'line-width' value.
 *                    - The global 'line-width' value defaults to 0.1.
 *
 * #bind PG_fget_line_width fortran() scheme(pg-get-line-width) python()
 */

double PG_fget_line_width(const PG_device *dev ARG(,in,cls))
   {double wd;

    if (dev != NULL)
       wd = dev->line_width;
    else
       PG_get_attrs_glb(TRUE, "line-width", &wd, NULL);

    return(wd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_LINE_WIDTH - Set the line width of device DEV to WD.
 *                    - If DEV is non-NULL return the old value.
 *                    - If DEV is NULL return the global 'line-width' value.
 *                    - The global 'line-width' value defaults to 0.1.
 *
 * #bind PG_fset_line_width fortran() scheme(pg-set-line-width!) python()
 */

double PG_fset_line_width(PG_device *dev ARG(,in,cls),
			  double wd ARG(0.1,in))
   {double owd;

    PG_get_attrs_glb(TRUE, "line-width", &owd, NULL);
    if (wd <= 0.0)
       wd = owd;

    if ((dev != NULL) && (dev->set_line_width != NULL))
       {owd = dev->line_width;
	dev->set_line_width(dev, wd);};

    return(owd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_MARKER_ORIENTATION - Return the marker orientation for device DEV.
 *                            - If DEV is NULL return the default value, 0.0.
 *
 * #bind PG_fget_marker_orientation fortran() scheme(pg-get-marker-orientation) python()
 */

double PG_fget_marker_orientation(const PG_device *dev ARG(,in,cls))
   {double rv;

    if (dev != NULL)
       rv = dev->marker_orientation;
    else
       rv = 0.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_MARKER_ORIENTATION - Set the marker orientation for device DEV to
 *                            - the value A. 
 *                            - Return the old value if DEV is non-NULL.
 *                            - If DEV is NULL return the default value, 0.0.
 *
 * #bind PG_fset_marker_orientation fortran() scheme(pg-set-marker-orientation!) python()
 */

double PG_fset_marker_orientation(PG_device *dev ARG(,in,cls),
				  double a ARG(0.0,in))
   {double rv;

    if (dev != NULL)
       {rv = dev->marker_orientation;
	dev->marker_orientation = a;}
    else
       rv = 0.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_MARKER_SCALE - Return the marker scale for device DEV.
 *                      - If DEV is NULL return the default value, 0.0.
 *
 * #bind PG_fget_marker_scale fortran() scheme(pg-get-marker-scale) python()
 */

double PG_fget_marker_scale(const PG_device *dev ARG(,in,cls))
   {double rv;

    if (dev != NULL)
       rv = dev->marker_scale;
    else
       rv = 0.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_MARKER_SCALE - Set the marker scale for device DEV to S.
 *                      - Return the old value if DEV is non-NULL.
 *                      - If DEV is NULL return the default value, 0.0.
 *
 * #bind PG_fset_marker_scale fortran() scheme(pg-set-marker-scale!) python()
 */

double PG_fset_marker_scale(PG_device *dev ARG(,in,cls),
			    double s ARG(0.0,in))
   {double rv;

    if (dev != NULL)
       {rv = dev->marker_scale;
	dev->marker_scale = max(s, 0.0);}
    else
       rv = 0.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_MAX_INTENSITY - Return the maximum intensity of device DEV.
 *                       - If DEV is NULL return the default value, 0.0.
 *
 * #bind PG_fget_max_intensity fortran() scheme() python()
 */

double PG_fget_max_intensity(const PG_device *dev ARG(,in,cls))
   {double rv;

    if (dev != NULL)
       rv = dev->max_intensity;
    else
       rv = 0.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_MAX_INTENSITY - Set the maximum intensity for device DEV to I.
 *                       - Return the old value if DEV is non-NULL.
 *                       - If DEV is NULL return the default value, 1.0.
 *
 * #bind PG_fset_max_intensity fortran() scheme() python()
 */

double PG_fset_max_intensity(PG_device *dev ARG(,in,cls),
			     double i ARG(1.0,in))
   {double rv;

    if (dev != NULL)
       {rv = dev->max_intensity;
	dev->max_intensity = min(i, 1.0);}
    else
       rv = 1.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_MAX_RED_INTENSITY - Return the maximum red intensity for device DEV.
 *                           - If DEV is NULL return the default value, 0.0.
 *
 * #bind PG_fget_max_red_intensity fortran() scheme(pg-get-max-red-intensity) python()
 */

double PG_fget_max_red_intensity(const PG_device *dev ARG(,in,cls))
   {double rv;

    if (dev != NULL)
       rv = dev->max_red_intensity;
    else
       rv = 0.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_MAX_RED_INTENSITY - Set the maximum red intensity for
 *                           - device DEV to I.
 *                           - Return the old value if DEV is non-NULL.
 *                           - If DEV is NULL return the default value, 1.0.
 *
 * #bind PG_fset_max_red_intensity fortran() scheme(pg-set-max-red-intensity!) python()
 */

double PG_fset_max_red_intensity(PG_device *dev ARG(,in,cls),
				 double i ARG(1.0,in))
   {double rv;

    if (dev != NULL)
       {rv = dev->max_red_intensity;
	dev->max_red_intensity = min(i, 1.0);}
    else
       rv = 1.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_MAX_GREEN_INTENSITY - Return the maximum green intensity for
 *                             - device DEV.
 *                             - If DEV is NULL return the default value, 0.0.
 *
 * #bind PG_fget_max_green_intensity fortran() scheme(pg-get-max-green-intensity) python()
 */

double PG_fget_max_green_intensity(const PG_device *dev ARG(,in,cls))
   {double rv;

    if (dev != NULL)
       rv = dev->max_green_intensity;
    else
       rv = 0.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_MAX_GREEN_INTENSITY - Set the maximum green intensity for
 *                             - device DEV to I.
 *                             - Return the old value if DEV is non-NULL.
 *                             - If DEV is NULL return the default value, 1.0.
 *
 * #bind PG_fset_max_green_intensity fortran() scheme(pg-set-max-green-intensity!) python()
 */

double PG_fset_max_green_intensity(PG_device *dev ARG(,in,cls),
				   double i ARG(1.0,in))
   {double rv;

    if (dev != NULL)
       {rv = dev->max_green_intensity;
	dev->max_green_intensity = min(i, 1.0);}
    else
       rv = 1.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_MAX_BLUE_INTENSITY - Return the maximum blue intensity
 *                            - for device DEV.
 *                            - If DEV is NULL return the default value, 0.0.
 *
 * #bind PG_fget_max_blue_intensity fortran() scheme(pg-get-max-blue-intensity) python()
 */

double PG_fget_max_blue_intensity(const PG_device *dev ARG(,in,cls))
   {double rv;

    if (dev != NULL)
       rv = dev->max_blue_intensity;
    else
       rv = 0.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_MAX_BLUE_INTENSITY - Set the maximum blue intensity for
 *                            - device DEV to I.
 *                            - Return the old value if DEV is non-NULL.
 *                            - If DEV is NULL return the default value, 1.0.
 *
 * #bind PG_fset_max_blue_intensity fortran() scheme(pg-set-max-blue-intensity!) python()
 */

double PG_fset_max_blue_intensity(PG_device *dev ARG(,in,cls),
				  double i ARG(1.0,in))
   {double rv;

    if (dev != NULL)
       {rv = dev->max_blue_intensity;
	dev->max_blue_intensity = min(i, 1.0);}
    else
       rv = 1.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_PIXMAP_FLAG - Return the pixmap flag for device DEV.
 *                     - If DEV is NULL return the default value, 0.
 *
 * #bind PG_fget_pixmap_flag fortran() scheme(ps-get-pixmap-flag) python()
 */

int PG_fget_pixmap_flag(const PG_device *dev ARG(,in,cls))
   {int fl;

    if (dev != NULL)
       fl = dev->use_pixmap;
    else
       fl = 0;

    return(fl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_PIXMAP_FLAG - Set the pixmap flag for device DEV to FL.
 *                     - Return the old value if DEV is non-NULL.
 *                     - If DEV is NULL return the default value, 0.
 *
 * #bind PG_fset_pixmap_flag fortran() scheme(pg-set-pixmap-flag!) python()
 */

int PG_fset_pixmap_flag(PG_device *dev ARG(,in,cls),
			int fl ARG(0,in))
   {int rv;

    if (dev != NULL)
       {rv = dev->use_pixmap;
	dev->use_pixmap = fl;}
    else
       rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_PS_DOTS_INCH - Return the global dots per inch value.
 *                      - The default value is 0.
 *
 * #bind PG_fget_ps_dots_inch fortran() scheme(pg-get-ps-dots-inch) python()
 */

double PG_fget_ps_dots_inch(void)
   {double dpi;

    PG_get_attrs_glb(TRUE, "ps-dots-inch", &dpi, NULL);

    return(dpi);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_PS_DOTS_INCH - Set the global dots per inch value to DPI.
 *                      - Return the value DPI.
 *
 * #bind PG_fset_ps_dots_inch fortran() scheme(pg-set-ps-dots-inch!) python()
 */

double PG_fset_ps_dots_inch(double dpi)
   {

    PG_set_attrs_glb(TRUE, "ps-dots-inch", dpi, NULL);

    return(dpi);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_RENDER_INFO - Get the render_info for graph G.
 *                     - If unsuccessful return NULL.
 *
 * #bind PG_fget_render_info fortran() scheme() python()
 */

pcons *PG_fget_render_info(const PG_graph *g)
   {pcons *rv;

    rv = NULL;
    if ((g != NULL) && (g->info_type != NULL))
       {if (strcmp(g->info_type, G_PCONS_P_S) == 0)
	   rv = (pcons *) g->info;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_RENDER_INFO - Set the render_info for graph G to A.
 *                     - Return the old value if G is non-NULL and
 *                     - return NULL otherwise.
 *
 * #bind PG_fset_render_info fortran() scheme() python()
 */

pcons *PG_fset_render_info(PG_graph *g, pcons *a)
   {pcons *rv;

    if (g != NULL)
       {rv = g->info;
	g->info = a;}
    else
       rv = NULL;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_USE_PIXMAP - Return the global 'use-pixmap' flag value.
 *                    - If 'use-pixmap' is TRUE renderings are done
 *                    - off screen in a pixmap.  This is much faster
 *                    - but causes some functions to behave differently.
 *
 * #bind PG_fget_use_pixmap fortran() scheme(pg-get-use-pixmap) python()
 */

int PG_fget_use_pixmap(void)
   {int i;

    PG_get_attrs_glb(TRUE, "use-pixmap", &i, NULL);

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_USE_PIXMAP - Set the global 'use-pixmap' flag value to I.
 *                    - If 'use-pixmap' is TRUE renderings are done
 *                    - off screen in a pixmap.  This is much faster
 *                    - but causes some functions to behave differently.
 *                    - Return I.
 *
 * #bind PG_fset_use_pixmap fortran() scheme(pg-set-use-pixmap!) python()
 */

int PG_fset_use_pixmap(int i)
   {

    PG_set_attrs_glb(TRUE, "use-pixmap", i, NULL);

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_RES_SCALE_FACTOR - Return the resolution scale factor
 *                          - for device DEV.
 *                          - If DEV is NULL return the default value, 1.
 *
 * #bind PG_fget_res_scale_factor fortran() scheme(pg-get-resolution-scale-factor) python()
 */

int PG_fget_res_scale_factor(const PG_device *dev ARG(,in,cls))
   {int rv;

    if (dev != NULL)
       rv = dev->resolution_scale_factor;
    else
       rv = 1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_RES_SCALE_FACTOR - Set the resolution scale factor for
 *                          - device DEV to S.
 *                          - Return the old value if DEV is non-NULL.
 *                          - If DEV is NULL return the default value, 1.
 *
 * #bind PG_fset_res_scale_factor fortran() scheme(pg-set-resolution-scale-factor!) python()
 */

int PG_fset_res_scale_factor(PG_device *dev ARG(,in,cls),
			     int s ARG(1,in))
   {int rv;

    if (dev != NULL)
       {rv = dev->resolution_scale_factor;
	dev->resolution_scale_factor = s;}
    else
       rv = 1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_BORDER_WIDTH - Return the border width for device DEV.
 *                      - If DEV is NULL return the default value, 1.
 *
 * #bind PG_fget_border_width fortran() scheme(pg-get-border-width) python()
 */

int PG_fget_border_width(const PG_device *dev ARG(,in,cls))
   {int rv;

    if (dev != NULL)
       rv = dev->border_width;
    else
       rv = 1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_BORDER_WIDTH - Set the border width for device DEV to W.
 *                      - Return the old value if DEV is non-NULL.
 *                      - If DEV is NULL return the default value, 1.
 *
 * #bind PG_fset_border_width fortran() scheme(pg-set-border-width!) python()
 */

int PG_fset_border_width(PG_device *dev ARG(,in,cls),
			 int w ARG(1,in))
   {int rv;

    if (dev != NULL)
       {rv = dev->border_width;
	dev->border_width = w;}
    else
       rv = 1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_VIEWPORT_POS - Return the current viewport position for
 *                      - device DEV in X.
 *                      - The viewport position is the lower, left, front
 *                      - point of the viewport.
 *                      - If DEV is NULL return the default value, (0.0, 0.0).
 *
 * #bind PG_fget_viewport_pos fortran() scheme() python()
 */

void PG_fget_viewport_pos(const PG_device *dev ARG(,in,cls),
			  double *x)
   {

    if (dev != NULL)
       {x[0] = dev->view_x[0];
	x[1] = dev->view_x[2];}
    else
       {x[0] = 0.0;
	x[1] = 0.0;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_VIEWPORT_POS - Set the current viewport position for
 *                      - device DEV to the values in X.
 *                      - The viewport position is the lower, left, front
 *                      - point of the viewport.
 *                      - Return the old values in X if DEV is non-NULL.
 *                      - If DEV is NULL return the default values, (0.0, 0.0).
 *
 * #bind PG_fset_viewport_pos fortran() scheme() python()
 */

void PG_fset_viewport_pos(PG_device *dev ARG(,in,cls),
			  double *x ARG([0.0,0.0],in))
   {double xo[PG_SPACEDM];

    if (dev != NULL)
       {xo[0] = dev->view_x[0];
	xo[1] = dev->view_x[2];

	dev->view_x[0] = x[0];
	dev->view_x[2] = x[1];

	x[0] = xo[0];
	x[1] = xo[1];}
    else
       {x[0] = 0.0;
	x[1] = 0.0;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_VIEWPORT_SHAPE - Return the current viewport shape for
 *                        - device DEV in DX and the aspect ratio in PA.
 *                        - The viewport shape is the width and height
 *                        - of the viewport. If DEV is NULL return the default
 *                        - values, (0.0, 0.0) and 1.0 respectively.
 *
 * #bind PG_fget_viewport_shape fortran() scheme() python()
 */

void PG_fget_viewport_shape(const PG_device *dev ARG(,in,cls),
			    double *dx, double *pa)
   {

    if (dev != NULL)
       {dx[0] = dev->view_x[1] - dev->view_x[0];
	dx[1] = dev->view_x[3] - dev->view_x[2];
        *pa   = dev->view_aspect;}
    else
       {dx[0] = 0.0;
	dx[1] = 0.0;
        *pa   = 1.0;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_VIEWPORT_SHAPE - Set the current viewport shape for
 *                        - device DEV to the values in DX and ASP.
 *                        - The viewport shape is the width and height
 *                        - of the viewport.
 *                        - Return the old values in DX if DEV
 *                        - is non-NULL. If DEV is NULL return the default
 *                        - values, (0.0, 0.0).
 *
 * #bind PG_fset_viewport_shape fortran() scheme() python()
 */

void PG_fset_viewport_shape(PG_device *dev ARG(,in,cls),
			    double *dx ARG([0.0,0.0],in),
			    double asp)
   {double dxo[PG_SPACEDM];

    if (dev != NULL)
       {dxo[0] = dev->view_x[1] - dev->view_x[0];
	dxo[1] = dev->view_x[3] - dev->view_x[2];

	dev->view_x[1]   = dev->view_x[0] + dx[0];
        dev->view_x[3]   = dev->view_x[2] + dx[1];
        dev->view_aspect = asp;

	dx[0] = dxo[0];
	dx[1] = dxo[1];}
    else
       {dx[0] = 0.0;
	dx[1] = 0.0;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_LINE_COLOR - Return the line color of device DEV.
 *                    - If DEV is NULL return the global 'line-color' value.
 *                    - The global 'line-color' value defaults to  BLACK
 *
 * #bind PG_fget_line_color fortran() scheme(pg-get-line-color) python()
 */

int PG_fget_line_color(const PG_device *dev ARG(,in,cls))
   {int rv;

    if (dev != NULL)
       rv = dev->line_color;
    else
       PG_get_attrs_glb(TRUE, "line-color", &rv, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_LINE_COLOR - Set the line color of device DEV to CLR.
 *                    - If MAPPED is TRUE colors will be mapped to values
 *                    - that are guaranteed to be visible.
 *                    - If DEV is NULL return the global 'line-color' value.
 *                    - The global 'line-color' value defaults to BLACK.
 *
 * #bind PG_fset_line_color fortran() scheme(pg-set-line-color!) python()
 */

int PG_fset_line_color(PG_device *dev ARG(,in,cls),
		       int clr ARG(1,in),
		       int mapped ARG(TRUE,in))
   {int rv;

    if ((dev != NULL) && (dev->set_line_color != NULL))
       {rv = dev->line_color;
	dev->set_line_color(dev, clr, mapped);}
    else
       PG_get_attrs_glb(TRUE, "line-color", &rv, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_TEXT_COLOR - Return the text color of device DEV.
 *                    - If DEV is NULL return the global 'text-color' value.
 *                    - The global 'text-color' value defaults to  BLACK
 *
 * #bind PG_fget_text_color fortran() scheme(pg-get-text-color) python()
 */

int PG_fget_text_color(const PG_device *dev ARG(,in,cls))
   {int rv;

    if (dev != NULL)
       rv = dev->text_color;
    else
       PG_get_attrs_glb(TRUE, "text-color", &rv, NULL);
/*       rv = BLACK; */

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_TEXT_COLOR - Set the text color of device DEV to CLR.
 *                    - If MAPPED is TRUE colors will be mapped to values
 *                    - that are guaranteed to be visible.
 *                    - If DEV is NULL return the global 'text-color' value.
 *                    - The global 'text-color' value defaults to BLACK.
 *
 * #bind PG_fset_text_color fortran() scheme(pg-set-text-color!) python()
 */

int PG_fset_text_color(PG_device *dev ARG(,in,cls),
		       int clr ARG(1,in),
		       int mapped ARG(TRUE,in))
   {int rv;

    if ((dev != NULL) && (dev->set_text_color != NULL))
       {rv = dev->text_color;
	dev->set_text_color(dev, clr, mapped);}
    else
       PG_get_attrs_glb(TRUE, "text-color", &rv, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_FILL_COLOR - Return the fill color of device DEV.
 *                    - If DEV is NULL return the global 'fill-color' value.
 *                    - The global 'fill-color' value defaults to  BLACK
 *
 * #bind PG_fget_fill_color fortran() scheme(pg-get-fill-color) python()
 */

int PG_fget_fill_color(const PG_device *dev ARG(,in,cls))
   {int rv;

    rv = dev->fill_color;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_FILL_COLOR - Set the fill color of device DEV to CLR.
 *                    - If MAPPED is TRUE colors will be mapped to values
 *                    - that are guaranteed to be visible.
 *                    - If DEV is NULL return the global 'fill-color' value.
 *                    - The global 'fill-color' value defaults to BLACK.
 *
 * #bind PG_fset_fill_color fortran() scheme(pg-set-fill-color!) python()
 */

int PG_fset_fill_color(PG_device *dev ARG(,in,cls),
		       int clr ARG(1,in),
		       int mapped ARG(TRUE,in))
   {int rv;

    if ((dev != NULL) && (dev->set_fill_color != NULL))
       {rv = dev->fill_color;
	dev->set_fill_color(dev, clr, mapped);}
    else
       PG_get_attrs_glb(TRUE, "fill-color", &rv, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_WHITE_BACKGROUND - Set the background color white flag for device DEV
 *                     - to the value T.
 *                     - Return the old value if DEV is non-NULL.
 *                     - If DEV is NULL return the default value, TRUE.
 *
 * #bind PG_white_background fortran() scheme(pg-set-white-background!) python()
 */

int PG_white_background(PG_device *dev ARG(,in,cls),
			int t ARG(TRUE,in))
   {int rv;

    if (dev != NULL)
       {rv = dev->background_color_white;
	dev->background_color_white = t;}
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TURN_GRID - Set the grid flag for device DEV to the value T.
 *              - Return the old value if DEV is non-NULL.
 *              - If DEV is NULL return the default value, TRUE.
 *
 * #bind PG_turn_grid fortran() scheme(pg-set-grid-flag!) python()
 */

int PG_turn_grid(PG_device *dev ARG(,in,cls),
		 int t ARG(TRUE,in))
   {int rv;

    if (dev != NULL)
       {rv = dev->grid;
	dev->grid = t;}
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TURN_DATA_ID - Set the data_id flag for device DEV to the value T.
 *                 - Return the old value if DEV is non-NULL.
 *                 - If DEV is NULL return the default value, TRUE.
 *
 * #bind PG_turn_data_id fortran() scheme(pg-set-data-id-flag!) python()
 */

int PG_turn_data_id(PG_device *dev ARG(,in,cls),
		    int t ARG(TRUE,in))
   {int rv;

    if (dev != NULL)
       {rv = dev->data_id;
	dev->data_id = t;}
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TURN_SCATTER - Set the scatter flag for device DEV to the value T.
 *                 - Return the old value if DEV is non-NULL.
 *                 - If DEV is NULL return the default value, TRUE.
 *
 * #bind PG_turn_scatter fortran() scheme(pg-set-scatter-flag!) python()
 */

int PG_turn_scatter(PG_device *dev ARG(,in,cls),
		    int t ARG(TRUE,in))
   {int rv;

    if (dev != NULL)
       {rv = dev->scatter;
	dev->scatter = t;}
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TURN_AUTODOMAIN - Set the autodomain flag for device DEV to the value T.
 *                    - Return the old value if DEV is non-NULL.
 *                    - If DEV is NULL return the default value, TRUE.
 *
 * #bind PG_turn_autodomain fortran() scheme(pg-set-autodomain!) python()
 */

int PG_turn_autodomain(PG_device *dev ARG(,in,cls),
		       int t ARG(TRUE,in))
   {int rv;

    if (dev != NULL)
       {rv = dev->autodomain;
	dev->autodomain = t;}
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TURN_AUTORANGE - Set the autorange flag for device DEV to the value T.
 *                   - Return the old value if DEV is non-NULL.
 *                   - If DEV is NULL return the default value, TRUE.
 *
 * #bind PG_turn_autorange fortran() scheme(pg-set-autorange!) python()
 */

int PG_turn_autorange(PG_device *dev ARG(,in,cls),
		      int t ARG(TRUE,in))
   {int rv;

    if (dev != NULL)
       {rv = dev->autorange;
	dev->autorange = t;}
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TURN_AUTOPLOT - Set the autoplot flag for device DEV to the value T.
 *                  - Return the old value if DEV is non-NULL.
 *                  - If DEV is NULL return the default value, TRUE.
 *
 * #bind PG_turn_autoplot fortran() scheme(pg-set-autoplot!) python()
 */

int PG_turn_autoplot(PG_device *dev ARG(,in,cls),
		     int t ARG(TRUE,in))
   {int rv;

    if (dev != NULL)
       {rv = dev->autoplot;
	dev->autoplot = t;}
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_N_EVENTS_PENDING - Return the number of events pending on device DEV.
 *                     - If DEV is NULL return 0.
 *
 * #bind PG_n_events_pending fortran() scheme(pg-get-n-events-pending) python()
 */

int PG_n_events_pending(PG_device *dev ARG(,in,cls))
   {int rv;

    if (dev != NULL)
       rv = dev->events_pending(dev);
    else
       rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_HANDLE_EXPOSE_EVENT - Handle an expose event EV for device DEV.
 *
 * #bind PG_handle_expose_event fortran() scheme() python()
 */

void PG_handle_expose_event(PG_device *dev ARG(,in,cls), PG_event *ev)
   {

    if ((dev != NULL) && (dev->expose_event_handler.fnc != NULL))
       dev->expose_event_handler.fnc(dev, ev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_HANDLE_UPDATE_EVENT - Handle an update event EV for device DEV.
 *
 * #bind PG_handle_update_event fortran() scheme() python()
 */

void PG_handle_update_event(PG_device *dev ARG(,in,cls), PG_event *ev)
   {

    if ((dev != NULL) && (dev->update_event_handler.fnc != NULL))
       dev->update_event_handler.fnc(dev, ev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_HANDLE_MOUSE_DOWN_EVENT - Handle an mouse_down event EV for device DEV.
 *
 * #bind PG_handle_mouse_down_event fortran() scheme() python()
 */

void PG_handle_mouse_down_event(PG_device *dev ARG(,in,cls), PG_event *ev)
   {

    if ((dev != NULL) && (dev->mouse_down_event_handler.fnc != NULL))
       dev->mouse_down_event_handler.fnc(dev, ev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_HANDLE_MOUSE_UP_EVENT - Handle an mouse_up event EV for device DEV.
 *
 * #bind PG_handle_mouse_up_event fortran() scheme() python()
 */

void PG_handle_mouse_up_event(PG_device *dev ARG(,in,cls), PG_event *ev)
   {

    if ((dev != NULL) && (dev->mouse_up_event_handler.fnc != NULL))
       dev->mouse_up_event_handler.fnc(dev, ev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_HANDLE_KEY_DOWN_EVENT - Handle an key_down event EV for device DEV.
 *
 * #bind PG_handle_key_down_event fortran() scheme() python()
 */

void PG_handle_key_down_event(PG_device *dev ARG(,in,cls), PG_event *ev)
   {

    if ((dev != NULL) && (dev->key_down_event_handler.fnc != NULL))
       dev->key_down_event_handler.fnc(dev, ev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_HANDLE_KEY_UP_EVENT - Handle an key_up event EV for device DEV.
 *
 * #bind PG_handle_key_up_event fortran() scheme() python()
 */

void PG_handle_key_up_event(PG_device *dev ARG(,in,cls), PG_event *ev)
   {

    if ((dev != NULL) && (dev->key_up_event_handler.fnc != NULL))
       dev->key_up_event_handler.fnc(dev, ev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_HANDLE_DEFAULT_EVENT - Handle an default event EV for device DEV.
 *
 * #bind PG_handle_default_event fortran() scheme() python()
 */

void PG_handle_default_event(PG_device *dev ARG(,in,cls), PG_event *ev)
   {

    if ((dev != NULL) && (dev->default_event_handler.fnc != NULL))
       dev->default_event_handler.fnc(dev, ev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_QUERY_POINTER - Return the pointer state for device DEV.
 *                  - For each non-NULL argument return:
 *                  -   IR   the pointer location (X,Y)
 *                  -   PB   state of pointer buttons
 *                  -   PQ   state of qualifier keys (CTRL, ALT, SHIFT, ...)
 *
 * #bind PG_query_pointer fortran() scheme() python()
 */

void PG_query_pointer(PG_device *dev ARG(,in,cls),
		      int *ir, int *pb, int *pq)
   {

    if ((dev != NULL) && (dev->query_pointer != NULL))
       dev->query_pointer(dev, ir, pb, pq);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GRAB_POINTER - Grab the pointer for device DEV.
 *                 - This means to shift focus to the device in
 *                 - which the pointer lies.
 *                 - Return TRUE if successful and otherwise return FALSE.
 *
 * #bind PG_grab_pointer fortran() scheme() python()
 */

int PG_grab_pointer(PG_device *dev ARG(,in,cls))
   {int rv;

    rv = FALSE;

    if ((dev != NULL) && (dev->grab_pointer != NULL))
       rv = dev->grab_pointer(dev);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_RELEASE_POINTER - Ungrab the pointer for device DEV.
 *                    - This means to release focus from the device in
 *                    - which the pointer lies.
 *                    - Return TRUE if successful and otherwise return FALSE.
 *
 * #bind PG_release_pointer fortran() scheme() python()
 */

int PG_release_pointer(PG_device *dev ARG(,in,cls))
   {int rv;

    rv = FALSE;

    if ((dev != NULL) && (dev->release_pointer != NULL))
       rv = dev->release_pointer(dev);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MOUSE_EVENT_INFO - Return info on mouse event EV in device DEV.
 *                     - For each non-NULL argument return:
 *                     -   IEV  the pointer location (X,Y)
 *                     -   PEB  state of pointer buttons
 *                     -   PEQ  state of qualifier keys (CTRL, ALT, SHIFT, ...)
 *
 * #bind PG_mouse_event_info fortran() scheme() python()
 */

void PG_mouse_event_info(PG_device *dev ARG(,in,cls),
			 PG_event *ev,
			 int *iev, PG_mouse_button *peb, int *peq)
   {

    if ((dev != NULL) && (dev->mouse_event_info != NULL))
       dev->mouse_event_info(dev, ev, iev, peb, peq);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_KEY_EVENT_INFO - Return info on key event EV in device DEV.
 *                   - For each non-NULL argument return:
 *                   -   IEV  the pointer location (X,Y)
 *                   -   BF   text buffer to receive upto N characters
 *                   -   PEQ  state of qualifier keys (CTRL, ALT, SHIFT, ...)
 *
 * #bind PG_key_event_info fortran() scheme() python()
 */

void PG_key_event_info(PG_device *dev ARG(,in,cls),
		       PG_event *ev, int *iev,
		       char *bf, int n, int *peq)
   {

    if ((dev != NULL) && (dev->key_event_info != NULL))
       dev->key_event_info(dev, ev, iev, bf, n, peq);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_OPEN_SCREEN - Open the device DEV at the NDC box
 *                - of the physical device.
 *
 * #bind PG_open_screen fortran() scheme() python()
 */

void PG_open_screen(PG_device *dev ARG(,in,cls), double *ndc)
   {double xf, yf, dxf, dyf;

    if ((dev != NULL) && (dev->open_screen != NULL))
       {xf = ndc[0];
	yf = ndc[1];
	dxf = ndc[2] - xf;
	dyf = ndc[3] - yf;
	dev->open_screen(dev, xf, yf, dxf, dyf);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_QUERY_SCREEN_N - Query the device DEV.
 *                   - Return size in DX and number of colors in PNC.
 *
 * #bind PG_query_screen_n fortran() scheme() python()
 */

void PG_query_screen_n(PG_device *dev ARG(,in,cls),
		       int *dx, int *pnc)
   {

    if ((dev != NULL) && (dev->query_screen != NULL))
       dev->query_screen(dev, dx, pnc);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MAKE_DEVICE_CURRENT - Make the device DEV the current one for drawing.
 *
 * #bind PG_make_device_current fortran() scheme() python()
 */

void PG_make_device_current(PG_device *dev ARG(,in,cls))
   {

    if ((dev != NULL) && (dev->make_device_current != NULL))
       dev->make_device_current(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_RELEASE_CURRENT_DEVICE - Make the device DEV NOT
 *                           - the current one for drawing.
 *
 * #bind PG_release_current_device fortran() scheme() python()
 */

void PG_release_current_device(PG_device *dev ARG(,in,cls))
   {

    if ((dev != NULL) && (dev->release_current_device != NULL))
       dev->release_current_device(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_UPDATE_VS - Update the view surface of the device DEV.
 *
 * #bind PG_update_vs fortran() scheme(pg-update-view-surface) python()
 */

void PG_update_vs(PG_device *dev ARG(,in,cls))
   {

    if ((dev != NULL) && (dev->update_vs != NULL))
       dev->update_vs(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FINISH_PLOT - Declare the picture in the view surface of the device DEV
 *                - finished.  Finished means that no more should be drawn
 *                - to DEV until the next PG_clear_screen.
 *
 * #bind PG_finish_plot fortran() scheme() python()
 */

void PG_finish_plot(PG_device *dev ARG(,in,cls))
   {

    if ((dev != NULL) && (dev->finish_plot != NULL))
       dev->finish_plot(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_EXPOSE_DEVICE - Bring the device DEV to the top and give it focus.
 *
 * #bind PG_expose_device fortran() scheme() python()
 */

void PG_expose_device(PG_device *dev ARG(,in,cls))
   {

    if ((dev != NULL) && (dev->expose_device != NULL))
       dev->expose_device(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CLOSE_DEVICE - Close the device DEV.
 *
 * #bind PG_close_device fortran() scheme() python()
 */

void PG_close_device(PG_device *dev ARG(,in,cls))
   {

    if ((dev != NULL) && (dev->close_device != NULL))
       dev->close_device(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CLOSE_CONSOLE - Close the console device.
 *
 * #bind PG_close_console fortran() scheme() python()
 */

void PG_close_console(void)
   {

    if ((PG_gs.console != NULL) &&
	(PG_gs.console->close_console != NULL))
       PG_gs.console->close_console();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CLEAR_FRAME - Clear the current frame of device DEV.
 *
 * #bind PG_clear_frame fortran() scheme() python()
 */

void PG_clear_frame(PG_device *dev ARG(,in,cls))
   {

    if (dev != NULL)
       PG_clear_region(dev, 2, NORMC, dev->g.fr, 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CLEAR_WINDOW - Clear the current window of device DEV.
 *
 * #bind PG_clear_window fortran() scheme() python()
 */

void PG_clear_window(PG_device *dev ARG(,in,cls))
   {

    if ((dev != NULL) && (dev->clear_window != NULL))
       {PG_set_attrs_glb(TRUE, "label-position-y", HUGE, NULL);
	dev->clear_window(dev);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CLEAR_VIEWPORT - Clear the current viewport of device DEV.
 *
 * #bind PG_clear_viewport fortran() scheme() python()
 */

void PG_clear_viewport(PG_device *dev ARG(,in,cls))
   {

    if ((dev != NULL) && (dev->clear_viewport != NULL))
       dev->clear_viewport(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CLEAR_REGION - Clear the region of device DEV specified by NDC.
 *
 * #bind PG_clear_region fortran() scheme() python()
 */

void PG_clear_region(PG_device *dev ARG(,in,cls),
		     int nd, PG_coord_sys cs,
		     double *ndc, int pad)
   {

    if ((dev != NULL) && (dev->clear_region != NULL))
       dev->clear_region(dev, nd, cs, ndc, pad);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CLEAR_PAGE - Clear the current page of device DEV and go to
 *               - line I on the screen.
 *
 * #bind PG_clear_page fortran() scheme() python()
 */

void PG_clear_page(PG_device *dev ARG(,in,cls), int i)
   {

    if ((dev != NULL) && (dev->clear_page != NULL))
       dev->clear_page(dev, i);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_WRITE_TEXT - Write text S to file FP and/or device DEV.
 *
 * #bind PG_write_text fortran() scheme() python()
 */

void PG_write_text(PG_device *dev ARG(,in,cls), FILE *fp, const char *s)
   {

    if ((dev != NULL) && (dev->write_text != NULL))
       dev->write_text(dev, fp, s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_NEXT_LINE - Move to next line of file FP and/or device DEV.
 *
 * #bind PG_next_line fortran() scheme() python()
 */

void PG_next_line(PG_device *dev ARG(,in,cls), FILE *fp)
   {

    if ((dev != NULL) && (dev->next_line != NULL))
       dev->next_line(dev, fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SHADE_POLY_N - Draw and fill polygon R in device DEV.
 *                 - The polygon R has ND dimensions and N points.
 *
 * #bind PG_shade_poly_n fortran() scheme() python()
 */

void PG_shade_poly_n(PG_device *dev ARG(,in,cls),
		     int nd, int n, double **r)
   {

    if ((dev != NULL) && (dev->shade_poly != NULL))
       dev->shade_poly(dev, nd, n, r);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FILL_CURVE - Draw curve CRV on device DEV and fill underneath it.
 *
 * #bind PG_fill_curve fortran() scheme() python()
 */

void PG_fill_curve(PG_device *dev ARG(,in,cls), PG_curve *crv)
   {

    if ((dev != NULL) && (dev->fill_curve != NULL))
       dev->fill_curve(dev, crv);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_CURVE - Draw curve CRV on device DEV.
 *               - If CLIP is TRUE the curve is clipped to the viewport.
 *
 * #bind PG_draw_curve fortran() scheme() python()
 */

void PG_draw_curve(PG_device *dev ARG(,in,cls), PG_curve *crv, int clip)
   {

    if ((dev != NULL) && (dev->draw_curve != NULL))
       dev->draw_curve(dev, crv, clip);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MAKE_PALETTE_CURRENT - Make P the current palette of device DEV.
 *
 * #bind PG_make_palette_current fortran() scheme() python()
 */

void PG_make_palette_current(PG_device *dev ARG(,in,cls), PG_palette *p)
   {

    if (dev != NULL)
       dev->current_palette = p;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_IMAGE_N - Copy the region of the window defined by IRG
 *                - of device DEV into buffer, BF.
 *                - The region is specifed in coordinate system CS.
 *
 * #bind PG_get_image_n fortran() scheme() python()
 */

void PG_get_image_n(const PG_device *dev ARG(,in,cls),
		    unsigned char *bf,
		    PG_coord_sys cs, double *irg)
   {double ix[PG_SPACEDM], nx[PG_SPACEDM], pc[PG_BOXSZ];

    if ((dev != NULL) && (dev->get_image != NULL))
       {PG_trans_box(dev, 2, cs, irg, PIXELC, pc);
	ix[0] = pc[0];
	ix[1] = pc[2];
	nx[0] = pc[1] - pc[0];
	nx[1] = pc[3] - pc[2];
	dev->get_image(dev, bf, ix[0], ix[1], nx[0], nx[1]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_PUT_IMAGE_N - Copy the buffer, BF, into region IRG of device DEV.
 *                - The coordinate system of IRG is CS.
 *
 * #bind PG_put_image_n fortran() scheme() python()
 */

void PG_put_image_n(PG_device *dev ARG(,in,cls),
		    unsigned char *bf,
		    PG_coord_sys cs, double *irg)
   {double ix[PG_SPACEDM], nx[PG_SPACEDM], pc[PG_BOXSZ];

    if ((dev != NULL) && (dev->put_image != NULL))
       {PG_trans_box(dev, 2, cs, irg, PIXELC, pc);
	ix[0] = pc[0];
	ix[1] = pc[2];
	nx[0] = pc[1] - pc[0];
	nx[1] = pc[3] - pc[2];
	dev->put_image(dev, bf, ix[0], ix[1], nx[0], nx[1]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MOVE_GR_ABS_N - Move the current point of device DEV to X.
 *                  - X is in world coordinates, WORLDC.
 *
 * #bind PG_move_gr_abs_n fortran() scheme() python()
 */

void PG_move_gr_abs_n(PG_device *dev ARG(,in,cls), double *x)
   {

    if ((dev != NULL) && (dev->move_gr_abs != NULL))
       dev->move_gr_abs(dev, x[0], x[1]);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MOVE_TX_ABS_N - Move the current text point of device DEV to X.
 *                  - X is in world coordinates, WORLDC.
 *
 * #bind PG_move_tx_abs_n fortran() scheme() python()
 */

void PG_move_tx_abs_n(PG_device *dev ARG(,in,cls), double *x)
   {

    if ((dev != NULL) && (dev->move_tx_abs != NULL))
       dev->move_tx_abs(dev, x[0], x[1]);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MOVE_TX_REL_N - Move the current text point of device DEV
 *                  - by displacement vector DX.
 *                  - DX is in world coordinates, WORLDC.
 *
 * #bind PG_move_tx_rel_n fortran() scheme() python()
 */

void PG_move_tx_rel_n(PG_device *dev ARG(,in,cls), double *dx)
   {

    if ((dev != NULL) && (dev->move_tx_rel != NULL))
       dev->move_tx_rel(dev, dx[0], dx[1]);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_TO_ABS_N - Draw from the current point of device DEV
 *                  - to the point X.
 *                  - X is in world coordinates, WORLDC.
 *
 * #bind PG_draw_to_abs_n fortran() scheme() python()
 */

void PG_draw_to_abs_n(PG_device *dev ARG(,in,cls), double *x)
   {

    if ((dev != NULL) && (dev->draw_to_abs != NULL))
       dev->draw_to_abs(dev, x[0], x[1]);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_TO_REL_N - Draw from the current point of device DEV
 *                  - by displacement vector DX.
 *                  - DX is in world coordinates, WORLDC.
 *
 * #bind PG_draw_to_rel_n fortran() scheme() python()
 */

void PG_draw_to_rel_n(PG_device *dev ARG(,in,cls), double *dx)
   {

    if ((dev != NULL) && (dev->draw_to_rel != NULL))
       dev->draw_to_rel(dev, dx[0], dx[1]);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

