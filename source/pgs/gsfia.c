/*
 * GSFIA.C - FORTRAN interface routines for PGS
 *         - NOTE: let's keep these in alphabetical order
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGAXD - get axis decades */

double FF_ID(pggaxd, PGGAXD)(void)
   {double d;

    d = PG_fget_axis_decades();

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSAXD - set axis decades */

double FF_ID(pgsaxd, PGSAXD)(double *sd)
   {double d;

    d = *sd;

    PG_fset_axis_decades(d);

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGCLM - get the clear mode */

FIXNUM FF_ID(pggclm, PGGCLM)(void)
   {FIXNUM rv;

    rv = PG_fget_clear_mode();

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSCLM - set the clear mode */

FIXNUM FF_ID(pgsclm, PGSCLM)(FIXNUM *sc)
   {FIXNUM rv;

    rv = *sc;

    PG_fset_clear_mode(rv);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGCLP - get the clipping */

FIXNUM FF_ID(pggclp, PGGCLP)(FIXNUM *sdid)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    rv  = PG_fget_clipping(dev);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSCLP - set the clipping */

FIXNUM FF_ID(pgsclp, PGSCLP)(FIXNUM *sdid, FIXNUM *sc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    rv  = PG_fset_clipping(dev, *sc);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGCPW - get the char path direction in WC */

void FF_ID(pggcpw, PGGCPW)(FIXNUM *sdid, double *ax)
   {PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_fget_char_path(dev, ax);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSCPW - set the char path direction in WC */

void FF_ID(pgscpw, PGSCPW)(FIXNUM *sdid, double *ax)
   {PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_fset_char_path(dev, ax);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGCUW - get the char up direction in WC */

void FF_ID(pggcuw, PGGCUW)(FIXNUM *sdid, double *ax)
   {PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_fget_char_up(dev, ax);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSCUW - set the char up direction in WC */

void FF_ID(pgscuw, PGSCUW)(FIXNUM *sdid, double *ax)
   {PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_fset_char_up(dev, ax);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGCSS - get the char size in NDC */

void FF_ID(pggcss, PGGCSS)(FIXNUM *sdid, double *as)
   {PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_fget_char_size_n(dev, 2, NORMC, as);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGCSW - get the char size in WC */

void FF_ID(pggcsw, PGGCSW)(FIXNUM *sdid, double *as)
   {PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_fget_char_size_n(dev, 2, WORLDC, as);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGLOP - get the logical operation */

FIXNUM FF_ID(pgglop, PGGLOP)(FIXNUM *sdid)
   {FIXNUM rv;
    PG_logical_operation lop;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    lop = PG_fget_logical_op(dev);
    rv  = lop;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSLOP - set the logical operation */

FIXNUM FF_ID(pgslop, PGSLOP)(FIXNUM *sdid, FIXNUM *slop)
   {FIXNUM rv;
    PG_logical_operation lop;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    lop = (PG_logical_operation) *slop;
    lop = PG_fset_logical_op(dev, lop);
    rv  = lop;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGLNS - get the line style */

FIXNUM FF_ID(pgglns, PGGLNS)(FIXNUM *sdid, FIXNUM *ss)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    rv  = PG_fget_line_style(dev);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSLNS - set the line style */

FIXNUM FF_ID(pgslns, PGSLNS)(FIXNUM *sdid, FIXNUM *ss)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    rv  = PG_fset_line_style(dev, *ss);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGLNW - get the line width */

double FF_ID(pgglnw, PGGLNW)(FIXNUM *sdid)
   {double rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    rv  = PG_fget_line_width(dev);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSLNW - set the line width */

double FF_ID(pgslnw, PGSLNW)(FIXNUM *sdid, double *sw)
   {double rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    rv  = PG_fset_line_width(dev, *sw);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGMKO - get the marker orientation */

double FF_ID(pggmko, PGGMKO)(FIXNUM *sdid)
   {double rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    rv  = PG_fget_marker_orientation(dev);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSMKO - set the marker orientation */

double FF_ID(pgsmko, PGSMKO)(FIXNUM *sdid, double *sw)
   {double rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    rv  = PG_fset_marker_orientation(dev, *sw);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGMKS - get the marker scale */

double FF_ID(pggmks, PGGMKS)(FIXNUM *sdid)
   {double rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    rv  = PG_fget_marker_scale(dev);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSMKS - set the marker scale */

double FF_ID(pgsmks, PGSMKS)(FIXNUM *sdid, double *ss)
   {double rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    rv  = PG_fset_marker_scale(dev, *ss);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGPMF - get the pixmap flag */

FIXNUM FF_ID(pggpmf, PGGPMF)(FIXNUM *sdid)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    rv  = PG_fget_pixmap_flag(dev);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSPMF - set the pixmap flag */

FIXNUM FF_ID(pgspmf, PGSPMF)(FIXNUM *sdid, FIXNUM *sc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    rv  = PG_fset_pixmap_flag(dev, *sc);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGDPI - get the PostScript dots per inch for 8.5 x 11 page */

double FF_ID(pggdpi, PGGDPI)(void)
   {double rv;

    rv = PG_fget_ps_dots_inch();

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSDPI - set the PostScript dots per inch for 8.5 x 11 page */

double FF_ID(pgsdpi, PGSDPI)(double *sdpi)
   {double rv;

    rv = PG_fset_ps_dots_inch(*sdpi);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSBWD - set the border width */

FIXNUM FF_ID(pgsbwd, PGSBWD)(FIXNUM *sdid, FIXNUM *sw)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    rv  = PG_fset_border_width(dev, *sw);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSVPS - set viewport position */

FIXNUM FF_ID(pgsvps, PGSVPS)(FIXNUM *sdid, double *ax)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    PG_fset_viewport_pos(dev, ax);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSVSH - set viewport shape */

FIXNUM FF_ID(pgsvsh, PGSVSH)(FIXNUM *sdid, double *ax, double *sa)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    PG_fset_viewport_shape(dev, ax, *sa);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/








/*--------------------------------------------------------------------------*/

/* PG_VIEW_ATTRIBUTES_POINTER - return the PG_view_attributes pointer
 *                            - associated with the index if the index
 *                            - is valid and NULL otherwise
 */

PG_view_attributes *PG_view_attributes_pointer(int vwatid)
   {PG_view_attributes *rv;

    if ((vwatid < 0) || (vwatid >= _PG.view_attr_indx))
       rv = NULL;
    else
       rv = _PG.view_attr_list[vwatid];

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGAXIS - draw an axis set */

FIXNUM FF_ID(pgaxis, PGAXIS)(FIXNUM *sdid, FIXNUM *saxt)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_axis(dev, (int) *saxt);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSAXA - set the parameters which control the look
 *        - of the axes being drawn
 *        - see PG_set_axis_attributes description
 *        - for character valued attributes the number of characters
 *        - in the value is in the ATTR (PAT) value position and
 *        - the characters are stored consecutively in PATC
 */

FIXNUM FF_ID(pgsaxa, PGSAXA)(FIXNUM *sdid, FIXNUM *sn,
			     double *aat, char *patc)
   {int i, nc, nn, type, lnclr, txclr, prec;
    double *attr;
    double chsp;
    double chup[PG_SPACEDM], chpth[PG_SPACEDM];
    char *pc, bf[MAXLINE];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    if (dev == NULL)
       return(FALSE);

/* load default values for external variables */
    lnclr = dev->WHITE;
    txclr = dev->WHITE;
    prec  = TEXT_CHARACTER_PRECISION;

    if (_PG_gattrs.axis_label_x_format == NULL)
       _PG_gattrs.axis_label_x_format = CSTRSAVE("%10.2g");

    if (_PG_gattrs.axis_label_y_format == NULL)
       _PG_gattrs.axis_label_y_format = CSTRSAVE("%10.2g");

    if (_PG_gattrs.axis_type_face == NULL)
       _PG_gattrs.axis_type_face = CSTRSAVE("helvetica");

    chsp   = 0.0;
    chpth[0] = 1.0;
    chpth[1] = 0.0;
    chup[0]  = 0.0;
    chup[1]  = 1.0;

    _PG_gattrs.axis_grid_on = FALSE;

/* the type of the second part of the pair is dependent on the
 * value of type 
 */
    nn   = *sn;
    pc   = patc;
    attr = aat;
    for (i = 0; i < nn; i++)
        {type = *attr++;
         switch (type)
            {case AXIS_LINESTYLE :
                  _PG_gattrs.axis_grid_style = *attr++;
                  break;

             case AXIS_LINETHICK :
                  _PG_gattrs.axis_line_width = *attr++;
                  break;

             case AXIS_LINECOLOR :
                  lnclr = *attr++;
                  break;

             case AXIS_LABELCOLOR :
                  txclr = *attr++;
                  break;

             case AXIS_LABELSIZE :
                  _PG_gattrs.axis_char_size = *attr++;
                  break;

             case AXIS_CHAR_ANGLE :
                  _PG_gattrs.axis_char_angle = *attr++;
                  break;

             case AXIS_LABELPREC :
                  prec = *attr++;
                  break;

             case AXIS_TICKSIZE :
                  _PG_gattrs.axis_major_tick_size = *attr++;
                  break;

             case AXIS_GRID_ON :
                  _PG_gattrs.axis_grid_on = *attr++;
                  break;

             case AXIS_LABELFONT :
                  CFREE(_PG_gattrs.axis_type_face);
                  nc = *attr++;
                  SC_strncpy(bf, MAXLINE, pc, nc);

                  pc += nc;
                  _PG_gattrs.axis_type_face = CSTRSAVE(bf);
                  break;

             case AXIS_X_FORMAT :
                  nc = *attr++;
                  SC_strncpy(bf, MAXLINE, pc, nc);

                  pc += nc;
                  strcpy(_PG_gattrs.axis_label_x_format, bf);
                  break;

             case AXIS_Y_FORMAT :
                  nc = *attr++;
                  SC_strncpy(bf, MAXLINE, pc, nc);

                  pc += nc;
                  strcpy(_PG_gattrs.axis_label_y_format, bf);
                  break;

             default :
                  return(FALSE);};};

    chpth[0] = sin(_PG_gattrs.axis_char_angle);
    chpth[1] = cos(_PG_gattrs.axis_char_angle);

/* set attribute values */
    PG_fset_clipping(dev, FALSE);
    PG_set_color_text(dev, txclr, TRUE);
    PG_fset_font(dev, _PG_gattrs.axis_type_face,
		 dev->type_style, dev->type_size);
    PG_fset_char_path(dev, chpth);
    PG_fset_char_precision(dev, prec);
    PG_fset_char_up(dev, chup);
    PG_fset_char_space(dev, chsp);
    PG_set_color_line(dev, lnclr, TRUE);
    PG_fset_line_style(dev, _PG_gattrs.axis_line_style);
    PG_fset_line_width(dev, _PG_gattrs.axis_line_width);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGAXL - get log axis flags */

FIXNUM FF_ID(pggaxl, PGGAXL)(FIXNUM *sdid, FIXNUM *snd, FIXNUM *aflg)
   {int id, nd;
    int ifl[PG_SPACEDM];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    nd = *snd;

    PG_get_axis_log_scale(dev, nd, ifl);

    for (id = 0; id < nd; id++)
        aflg[id] = ifl[id];

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGBSZ - return the I/O buffer size */

FIXNUM FF_ID(pggbsz, PGGBSZ)(void)
   {FIXNUM rv;
    int64_t sz;

    sz = PG_get_buffer_size();
    rv = sz;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGCLOS - close the PG_device associated with the integer index */

FIXNUM FF_ID(pgclos, PGCLOS)(FIXNUM *sdid)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_DEL_POINTER(PG_device, *sdid);
    PG_close_device(dev);

    *sdid = -1;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGCLPG - clear the page */

FIXNUM FF_ID(pgclpg, PGCLPG)(FIXNUM *sdid, FIXNUM *si)
   {int i;
    FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    i   = *si;

    PG_clear_page(dev, i);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGCLRG - clear the specified region */

FIXNUM FF_ID(pgclrg, PGCLRG)(FIXNUM *sdid, double *sxn, double *sxx,
			     double *syn, double *syx, FIXNUM *sad)
   {int pd;
    FIXNUM rv;
    double ndc[PG_BOXSZ];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    pd  = *sad;

    ndc[0] = *sxn;
    ndc[1] = *sxx;
    ndc[2] = *syn;
    ndc[3] = *syx;

    PG_clear_region(dev, 2, NORMC, ndc, pd);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGCLSC - clear the screen (necessary for CGM devices) */

FIXNUM FF_ID(pgclsc, PGCLSC)(FIXNUM *sdid)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_clear_window(dev);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGCLVP - clear the viewport */

FIXNUM FF_ID(pgclvp, PGCLVP)(FIXNUM *sdid)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_clear_viewport(dev);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDDP2 - draw 2d disjoint polyline */

FIXNUM FF_ID(pgddp2, PGDDP2)(FIXNUM *sdid, double *ax, double *ay,
			     FIXNUM *sn, FIXNUM *sf, FIXNUM *sc)
   {int f;
    long n;
    FIXNUM rv;
    double *t[2];
    PG_coord_sys cs;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    t[0] = ax;
    t[1] = ay;

    f  = (int) *sf;
    n  = (long) *sn;
    cs = (PG_coord_sys) *sc;

    PG_draw_disjoint_polyline_n(dev, 2, cs, n, t, f);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDPL2 - draw 2d polyline */

FIXNUM FF_ID(pgdpl2, PGDPL2)(FIXNUM *sdid, double *ax, double *ay,
			     FIXNUM *sn, FIXNUM *sc)
   {FIXNUM rv;
    double *t[2];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    t[0] = ax;
    t[1] = ay;

    PG_draw_polyline_n(dev, 2, WORLDC, (long) *sn, t, (int) *sc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDDP3 - draw 3d disjoint polyline */

FIXNUM FF_ID(pgddp3, PGDDP3)(FIXNUM *sdid, double *ax, double *ay, double *az,
			     FIXNUM *sn, FIXNUM *sf, FIXNUM *sc)
   {int f;
    long n;
    FIXNUM rv;
    double *t[PG_SPACEDM];
    PG_coord_sys cs;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    t[0] = ax;
    t[1] = ay;
    t[2] = az;

    f  = (int) *sf;
    n  = (long) *sn;
    cs = (PG_coord_sys) *sc;

    PG_draw_disjoint_polyline_n(dev, 3, cs, n, t, f);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDRAX - draw a single axis */

FIXNUM FF_ID(pgdrax, PGDRAX)(FIXNUM *sdid, double *sxl, double *syl,
			     double *sxr, double *syr,
			     double *st1, double *st2,
			     double *sv1, double *sv2, double *ssc,
			     FIXNUM *sncf, char *format,
			     FIXNUM *std, FIXNUM *stt, FIXNUM *slt)
   {int td;
    FIXNUM rv;
    double tn[2], vw[2];
    double xl[PG_SPACEDM], xr[PG_SPACEDM];
    char lformat[MAXLINE];
    PG_device *dev;

    SC_FORTRAN_STR_C(lformat, format, *sncf);

    dev = SC_GET_POINTER(PG_device, *sdid);

    td = *std;

    xl[0] = *sxl;
    xl[1] = *syl;
    xr[0] = *sxr;
    xr[1] = *syr;
    tn[0] = *st1;
    tn[1] = *st2;
    vw[0] = *sv1;
    vw[1] = *sv2;

    PG_draw_axis_n(dev, xl, xr, tn, vw, *ssc, lformat,
		   (int) *stt, (int) *slt, FALSE,
		   td, 0);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDAX3 - draw a 3D axis set */

FIXNUM FF_ID(pgdax3, PGDAX3)(FIXNUM *sdid, double *axl, double *ayl,
			     double *azl, FIXNUM *snp)
   {FIXNUM rv;
    double *p[PG_SPACEDM];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    p[0] = axl;
    p[1] = ayl;
    p[2] = azl;

    PG_axis_3(dev, p, (int) *snp, NULL);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDRMK - draw marker characters */

FIXNUM FF_ID(pgdrmk, PGDRMK)(FIXNUM *sdid, FIXNUM *sn,
			     double *ax, double *ay, FIXNUM *smrk)
   {int n, mrk;
    FIXNUM rv;
    double *r[PG_SPACEDM];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    n   = *sn;
    mrk = *smrk;

    r[0] = ax;
    r[1] = ay;

    PG_draw_markers_n(dev, 2, WORLDC, n, r, mrk);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDMRK - define a marker character */

FIXNUM FF_ID(pgdmrk, PGDMRK)(FIXNUM *sn, double *ax1, double *ay1,
			     double *ax2, double *ay2)
   {int n;
    FIXNUM rv;

    n = *sn;

    rv = PG_def_marker(n, ax1, ay1, ax2, ay2);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDPLT - draw a mesh - plot a domain */

FIXNUM FF_ID(pgdplt, PGDPLT)(FIXNUM *sdid, FIXNUM *sdom)
   {FIXNUM rv;
    PG_device *dev;
    PM_set *set;

    set = *(PM_set **) sdom;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_domain_plot(dev, set, NULL);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDRBX - draw a box */

FIXNUM FF_ID(pgdrbx, PGDRBX)(FIXNUM *sdid, double *sx1, double *sx2,
			     double *sy1, double *sy2)
   {FIXNUM rv;
    double bx[PG_BOXSZ];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    bx[0] = *sx1;
    bx[1] = *sx2;
    bx[2] = *sy1;
    bx[3] = *sy2;
    PG_draw_box_n(dev, 2, WORLDC, bx);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDRIF - draw the set of interface objects for the device */

FIXNUM FF_ID(pgdrif, PGDRIF)(FIXNUM *sdid)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_draw_interface_objects(dev);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDRLN - draw a line between the specified points */

FIXNUM FF_ID(pgdrln, PGDRLN)(FIXNUM *sdid, double *sx1, double *sy1,
			     double *sx2, double *sy2)
   {FIXNUM rv;
    double x1[PG_SPACEDM], x2[PG_SPACEDM];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    x1[0] = *sx1;
    x1[1] = *sy1;
    x2[0] = *sx2;
    x2[1] = *sy2;
    PG_draw_line_n(dev, 2, WORLDC, x1, x2, dev->clipping);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDRPA - draw the current palette */

FIXNUM FF_ID(pgdrpa, PGDRPA)(FIXNUM *sdid, double *sx1, double *sy1,
			     double *sx2, double *sy2,
			     double *sz1, double *sz2,
			     double *sw)
   {FIXNUM rv;
    double dbx[PG_BOXSZ], rbx[PG_BOXSZ];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    dbx[0] = *sx1;
    dbx[1] = *sx2;
    dbx[2] = *sy1;
    dbx[3] = *sy2;
    rbx[0] = *sz1;
    rbx[1] = *sz2;

    PG_draw_palette_n(dev, dbx, rbx, *sw, FALSE);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDRPP - draw the current palette */

FIXNUM FF_ID(pgdrpp, PGDRPP)(FIXNUM *sdid, double *sx1, double *sy1,
			     double *sx2, double *sy2,
			     double *sz1, double *sz2,
			     double *sw, FIXNUM *se)
   {FIXNUM rv;
    double dbx[PG_BOXSZ], rbx[PG_BOXSZ];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    dbx[0] = *sx1;
    dbx[1] = *sx2;
    dbx[2] = *sy1;
    dbx[3] = *sy2;
    rbx[0] = *sz1;
    rbx[1] = *sz2;

    PG_draw_palette_n(dev, dbx, rbx, *sw, *se);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGFNPL - finish the plot (necessary for CGM devices) */

FIXNUM FF_ID(pgfnpl, PGFNPL)(FIXNUM *sdid)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_finish_plot(dev);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGFPLY - draw and fill the specified polygon with the specified color */

FIXNUM FF_ID(pgfply, PGFPLY)(FIXNUM *sdid, double *ax, double *ay,
			     FIXNUM *sn, FIXNUM *sc)
   {FIXNUM rv;
    double *r[2];
    PG_device *dev;

    r[0] = ax;
    r[1] = ay;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_fill_polygon_n(dev, (int) *sc, TRUE, 2, WORLDC, *sn, r);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGFIN - get the finished status */

FIXNUM FF_ID(pggfin, PGGFIN)(FIXNUM *sdid, FIXNUM *sc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    *sc = dev->finished;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGLNC - get the line color */

FIXNUM FF_ID(pgglnc, PGGLNC)(FIXNUM *sdid, FIXNUM *sc)
   {int lc;
    FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_get_line_color(dev, &lc);

    *sc = lc;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGTLN - get a line of text from the input descriptor */

FIXNUM FF_ID(pggtln, PGGTLN)(FIXNUM *sncs, char *s, FIXNUM *sfd)
   {FIXNUM rv;
    long fd;
    char *t;
    SC_address f;

    fd = (long) *sfd;

    if (fd == 0L)
       t = GETLN(s, (int) *sncs, stdin);

    else
       {f.diskaddr = fd;
        t = GETLN(s, (int) *sncs, (FILE *) f.memaddr);};

    rv = (t != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGTXC - get the text color */

FIXNUM FF_ID(pggtxc, PGGTXC)(FIXNUM *sdid, FIXNUM *sc)
   {int lc;
    FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_get_text_color(dev, &lc);

    *sc = lc;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGTXF - get the font */

FIXNUM FF_ID(pggtxf, PGGTXF)(FIXNUM *sdid,
			     FIXNUM *sncf, char *pf,
			     FIXNUM *sncs, char *pst,
			     FIXNUM *ssz)
   {int ls, nf, ns, lnf, lns;
    FIXNUM rv;
    char *f, *st;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_fget_font(dev, &f, &st, &ls);

    nf = strlen(f);
    ns = strlen(st);

/* get the buffer sizes */
    lnf = *sncf;
    lns = *sncs;

/* return the actual string lengths */
    *sncf = nf;
    *sncs = ns;

    if ((nf > lnf) || (ns > lns))
       rv = FALSE;

    else
       {SC_strncpy(pf, *sncf, f, lnf);
        SC_strncpy(pst, *sncs, st, lns);
        *ssz = ls;

        CFREE(f);
        CFREE(st);

        rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGTEX - get the text extent in CS */

FIXNUM FF_ID(pggtex, PGGTEX)(FIXNUM *sdid, FIXNUM *snd, FIXNUM *sc,
			     FIXNUM *sncs, char *s,
			     double *sdx, double *sdy)
   {FIXNUM rv;
    double p[PG_SPACEDM];
    char ls[MAXLINE];
    PG_coord_sys cs;
    PG_device *dev;

    SC_FORTRAN_STR_C(ls, s, *sncs);

    dev = SC_GET_POINTER(PG_device, *sdid);

    cs = (PG_coord_sys) *sc;

    PG_get_text_ext_n(dev, *snd, cs, ls, p);

    *sdx = p[0];
    *sdy = p[1];

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGVWP - get the viewport */

FIXNUM FF_ID(pggvwp, PGGVWP)(FIXNUM *sdid, double *sx1, double *sx2,
			     double *sy1, double *sy2)
   {FIXNUM rv;
    double ndc[PG_BOXSZ];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    PG_get_viewspace(dev, WORLDC, ndc);
    *sx1 = ndc[0];
    *sx2 = ndc[1];
    *sy1 = ndc[2];
    *sy2 = ndc[3];

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGWCS - get the world coordinate system */

FIXNUM FF_ID(pggwcs, PGGWCS)(FIXNUM *sdid, double *sx1, double *sx2,
			     double *sy1, double *sy2)
   {FIXNUM rv;
    double wc[PG_BOXSZ];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    PG_get_viewspace(dev, WORLDC, wc);
    *sx1 = wc[0];
    *sx2 = wc[1];
    *sy1 = wc[2];
    *sy2 = wc[3];

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGMDVC - make the device current */

FIXNUM FF_ID(pgmdvc, PGMDVC)(FIXNUM *sdid)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_make_device_current(dev);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGMKDV - make a PG_device
 *        - save the PG_device pointer in an internal array
 *        - and return an integer index to the pointer if successful
 *        - return -1 otherwise
 */

FIXNUM FF_ID(pgmkdv, PGMKDV)(FIXNUM *sncn, char *name,
			     FIXNUM *snct, char *type,
			     FIXNUM *sncl, char *title)
   {FIXNUM rv;
    char lname[MAXLINE], ltype[MAXLINE], ltitle[MAXLINE];
    PG_device *dev;

    SC_FORTRAN_STR_C(lname, name, *sncn);
    SC_FORTRAN_STR_C(ltype, type, *snct);
    SC_FORTRAN_STR_C(ltitle, title, *sncl);

    dev = PG_make_device(lname, ltype, ltitle);
    if (dev == NULL)
       rv = -1;

    else
       rv = SC_ADD_POINTER(dev);

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGOPEN - open the specified PG_device */

FIXNUM FF_ID(pgopen, PGOPEN)(FIXNUM *sdid, double *sxf, double *syf,
			     double *sdxf, double *sdyf)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    if (PG_open_device(dev, *sxf, *syf, *sdxf, *sdyf) == NULL)
       rv = FALSE;
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGPTIN - given a pointer, return its index
 *        - return -1 otherwise
 */

FIXNUM FF_ID(pgptin, PGPTIN)(FIXNUM *sptr, FIXNUM *sidx)
   {FIXNUM rv;

    rv = (*sidx = SC_GET_INDEX((void *) sptr));

    return(rv);}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGQDEV - get some physical parameters from the specified PG_device */

FIXNUM FF_ID(pgqdev, PGQDEV)(FIXNUM *sdid, FIXNUM *sdx, FIXNUM *sdy,
			     FIXNUM *snc)
   {int dx, dy, nc;
    FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_query_device(dev, &dx, &dy, &nc);

    *sdx = dx;
    *sdy = dy;
    *snc = nc;

    rv = TRUE;

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGQWIN - get the window shape of the specified device */

FIXNUM FF_ID(pgqwin, PGQWIN)(FIXNUM *sdid, FIXNUM *sdx, FIXNUM *sdy)
   {int dx, dy;
    FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_query_window(dev, &dx, &dy);

    *sdx = dx;
    *sdy = dy;

    rv = TRUE;

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGPLLN - low level line plot routine */

FIXNUM FF_ID(pgplln, PGPLLN)(FIXNUM *sdid, double *ax, double *ay,
			     FIXNUM *sn, FIXNUM *spty, FIXNUM *saxt,
			     FIXNUM *scol, double *swid,
			     FIXNUM *ssty, FIXNUM *ssca, FIXNUM *smrk,
			     FIXNUM *ssta, FIXNUM *sl)
   {int axt, sty, sca, mrk, clr, sta;
    long n, l;
    FIXNUM rv;
    PG_rendering pty;
    PG_device *dev;
    pcons *info;

    dev = SC_GET_POINTER(PG_device, *sdid);

    n   = *sn;
    l   = *sl;
    axt = *saxt;
    sty = *ssty;
    sca = *ssca;
    mrk = *smrk;
    clr = *scol;
    sta = *ssta;
    pty = (PG_rendering) *spty;

    info = PG_set_line_info(NULL, pty, axt, sty, sca, mrk, clr, sta, *swid);
    PG_plot_curve(dev, ax, ay, n, info, l);

    SC_free_alist(info, 2);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGPTOS - convert from PC to NDC */

FIXNUM FF_ID(pgptos, PGPTOS)(FIXNUM *sdid, FIXNUM *six, FIXNUM *siy,
			     double *sx, double *sy)
   {FIXNUM rv;
    double p[PG_SPACEDM];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    p[0] = *six;
    p[1] = *siy;
    PG_trans_point(dev, 2, PIXELC, p, NORMC, p);
    *sx = p[0];
    *sy = p[1];

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGRDIF - read an interface description file */

FIXNUM FF_ID(pgrdif, PGRDIF)(FIXNUM *sdid, FIXNUM *sncn, char *name)
   {FIXNUM rv;
    char lname[MAXLINE];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    SC_FORTRAN_STR_C(lname, name, *sncn);

    rv = PG_read_interface(dev, lname);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGRDVC - release device as the device current */

FIXNUM FF_ID(pgrdvc, PGRDVC)(FIXNUM *sdid)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_release_current_device(dev);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGRGDV - register a device */

FIXNUM FF_ID(pgrgdv, PGRGDV)(FIXNUM *sncn, char *name)
   {FIXNUM rv;
    char lname[MAXLINE];
    
    SC_FORTRAN_STR_C(lname, name, *sncn);

    rv = FALSE;

    if (!strcmp(lname, "IMAGE"))
       {PG_register_device("IMAGE", PG_setup_image_device);
        rv = TRUE;}

    else if (!strcmp(lname, "PNG"))
       {PG_register_device("PNG", PG_setup_png_device);
        rv = TRUE;}

    else if (!strcmp(lname, "JPEG"))
       {PG_register_device("JPEG", PG_setup_jpeg_device);
        rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGRGFN - register a call back function with the device */

FIXNUM FF_ID(pgrgfn, PGRGFN)(FIXNUM *sncn, char *name, PFEvCallback fnc)
   {FIXNUM rv;
    char lname[MAXLINE];

    SC_FORTRAN_STR_C(lname, name, *sncn);
    PG_register_callback(lname, fnc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGRGVR - register a variable with the device */

FIXNUM FF_ID(pgrgvr, PGRGVR)(FIXNUM *sncn, char *name,
			     FIXNUM *snct, char *type,
			     void *vr, void *vn, void *vx)
   {FIXNUM rv;
    char lname[MAXLINE], ltype[MAXLINE];

    SC_FORTRAN_STR_C(lname, name, *sncn);
    SC_FORTRAN_STR_C(ltype, type, *snct);

    PG_register_variable(lname,
			 CSTRSAVE(ltype),
			 vr, vn, vx);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGRVPA - restore the PG_view_attributes for the specified device
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(pgrvpa, PGRVPA)(FIXNUM *sdid, FIXNUM *svwatid)
   {FIXNUM rv;
    PG_device *dev;
    PG_view_attributes *d;

    rv = FALSE;

    dev = SC_GET_POINTER(PG_device, *sdid);
    if (dev != NULL)
       {d = PG_view_attributes_pointer((int) *svwatid);

	PG_restore_view_attributes(dev, d);

	rv = TRUE;};

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSADR - set the auto domain and range */

FIXNUM FF_ID(pgsadr, PGSADR)(FIXNUM *sdid, FIXNUM *sd, FIXNUM *sr)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    dev->autodomain = *sd;
    dev->autorange  = *sr;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSADM - set the auto domain */

FIXNUM FF_ID(pgsadm, PGSADM)(FIXNUM *sdid, FIXNUM *sd)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    dev->autodomain = *sd;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSARN - set the auto range */

FIXNUM FF_ID(pgsarn, PGSARN)(FIXNUM *sdid, FIXNUM *sr)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    dev->autorange  = *sr;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSAXL - set log axis flags */

FIXNUM FF_ID(pgsaxl, PGSAXL)(FIXNUM *sdid, FIXNUM *snd, FIXNUM *aflg)
   {int id, nd;
    int ifl[PG_SPACEDM];
    FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    nd = *snd;

    for (id = 0; id < nd; id++)
        ifl[id] = aflg[id];

    PG_set_axis_log_scale(dev, nd, ifl);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSBSZ - set the I/O buffer size */

FIXNUM FF_ID(pgsbsz, PGSBSZ)(FIXNUM *ssz)
   {FIXNUM rv;
    int64_t sz;

    sz = *ssz;
    PG_set_buffer_size(sz);
    rv = sz;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSELC - select a color from the n-dimensional palette */

FIXNUM FF_ID(pgselc, PGSELC)(FIXNUM *sdid, FIXNUM *sn, double *av,
			     double *an, double *ax)
   {FIXNUM rv;
    double extr[2];
    PG_device *dev;

    extr[0] = *an;
    extr[1] = *ax;
    
    dev = SC_GET_POINTER(PG_device, *sdid);

    rv = PG_select_color(dev, *sn, av, extr);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSFCL - set the fill color */

FIXNUM FF_ID(pgsfcl, PGSFCL)(FIXNUM *sdid, FIXNUM *sc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_set_fill_color(dev, *sc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSFIN - set the finished status */

FIXNUM FF_ID(pgsfin, PGSFIN)(FIXNUM *sdid, FIXNUM *sc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    dev->finished = *sc;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSLNC - set the line color */

FIXNUM FF_ID(pgslnc, PGSLNC)(FIXNUM *sdid, FIXNUM *sc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_set_color_line(dev, (int) *sc, TRUE);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSPAL - set the current palette */

FIXNUM FF_ID(pgspal, PGSPAL)(FIXNUM *sdid, FIXNUM *sncn, char *pname)
   {FIXNUM rv;
    PG_device *dev;
    char lname[MAXLINE];

    dev = SC_GET_POINTER(PG_device, *sdid);
    SC_FORTRAN_STR_C(lname, pname, *sncn);

    rv = (PG_set_palette(dev, lname) != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSTOP - convert from NDC to PC */

FIXNUM FF_ID(pgstop, PGSTOP)(FIXNUM *sdid, double *sx, double *sy,
			     FIXNUM *six, FIXNUM *siy)
   {FIXNUM rv;
    double p[PG_SPACEDM];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    p[0] = *sx;
    p[1] = *sy;
    PG_trans_point(dev, 2, NORMC, p, PIXELC, p);
    *six = p[0];
    *siy = p[1];

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSTOW - convert from NDC to WC */

FIXNUM FF_ID(pgstow, PGSTOW)(FIXNUM *sdid, double *sx, double *sy)
   {FIXNUM rv;
    double p[PG_SPACEDM];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    p[0] = *sx;
    p[1] = *sy;
    PG_trans_point(dev, 2, NORMC, p, WORLDC, p);
    *sx = p[0];
    *sy = p[1];

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSTXC - set the text color */

FIXNUM FF_ID(pgstxc, PGSTXC)(FIXNUM *sdid, FIXNUM *sc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_set_color_text(dev, (int) *sc, TRUE);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSTXF - set the font */

FIXNUM FF_ID(pgstxf, PGSTXF)(FIXNUM *sdid, FIXNUM *sncf, char *face,
			     FIXNUM *sncs, char *style, FIXNUM *ssz)
   {FIXNUM rv;
    PG_device *dev;
    char lface[MAXLINE], lstyle[MAXLINE];

    dev = SC_GET_POINTER(PG_device, *sdid);

    SC_FORTRAN_STR_C(lface, face, *sncf);
    SC_FORTRAN_STR_C(lstyle, style, *sncs);

    PG_fset_font(dev, lface, lstyle, (int) *ssz);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSVPA - save the PG_view_attributes for the specified device
 *        - save the PG_view_attributes pointer in an internal array
 *        - return an integer index to the pointer if successful (via PN)
 *        - return TRUE if successful
 *        - if N < 0 allocate a new PG_view_attributes
 *        - otherwise N is taken to be the index of an existing structure
 *        - to be reused
 */

FIXNUM FF_ID(pgsvpa, PGSVPA)(FIXNUM *sdid, FIXNUM *sn)
   {int n;
    FIXNUM rv;
    PG_device *dev;
    PG_view_attributes *d;

    dev = SC_GET_POINTER(PG_device, *sdid);
    n   = *sn;
    if ((n < 0) || (n >= _PG.view_attr_indx))

/* the first time out allocate the lists of pointers and disk addresses */
       {if (_PG.view_attr_list == NULL)
           {_PG.view_attr_indx     = 0;
            _PG.view_attr_max_indx = 10;
            _PG.view_attr_list     = CMAKE_N(PG_view_attributes *,
					     _PG.view_attr_max_indx);}

        if (_PG.view_attr_max_indx <= _PG.view_attr_indx)
           {_PG.view_attr_max_indx += 10;
            CREMAKE(_PG.view_attr_list,
                     PG_view_attributes *,
                     _PG.view_attr_max_indx);};

        d = PG_make_view_attributes(dev);
        if (d == NULL)
           return((FIXNUM) -1);
        else
           {_PG.view_attr_list[_PG.view_attr_indx] = d;
            n = _PG.view_attr_indx++;};}

    else
       {d = _PG.view_attr_list[n];
        PG_save_view_attributes(d, dev);};

    *sn = n;

    rv = TRUE;

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSVWP - set the viewport */

FIXNUM FF_ID(pgsvwp, PGSVWP)(FIXNUM *sdid, double *sx1, double *sx2,
			     double *sy1, double *sy2)
   {FIXNUM rv;
    double ndc[PG_BOXSZ];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    ndc[0] = *sx1;
    ndc[1] = *sx2;
    ndc[2] = *sy1;
    ndc[3] = *sy2;

    PG_set_viewspace(dev, 2, NORMC, ndc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSWCS - set the world coordinate system */

FIXNUM FF_ID(pgswcs, PGSWCS)(FIXNUM *sdid, double *sx1, double *sx2,
			     double *sy1, double *sy2)
   {FIXNUM rv;
    double wc[PG_BOXSZ];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    wc[0] = *sx1;
    wc[1] = *sx2;
    wc[2] = *sy1;
    wc[3] = *sy2;
    PG_set_viewspace(dev, 2, WORLDC, wc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGTDID - turn device data id on or off */

FIXNUM FF_ID(pgtdid, PGTDID)(FIXNUM *sdid, FIXNUM *sval)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    PG_turn_data_id(dev, *sval);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGUPVS - update the view surface of the PG_device */

FIXNUM FF_ID(pgupvs, PGUPVS)(FIXNUM *sdid)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_update_vs(dev);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGWRCL - write label centered wrt x */

FIXNUM FF_ID(pgwrcl, PGWRCL)(FIXNUM *sdid, double *ssy,
			     FIXNUM *sncl, char *label)
   {FIXNUM rv;
    char llabel[MAXLINE];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    SC_FORTRAN_STR_C(llabel, label, *sncl);

    PG_center_label(dev, *ssy, llabel);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGWRIF - write an interface description file */

FIXNUM FF_ID(pgwrif, PGWRIF)(FIXNUM *sdid, FIXNUM *sncn, char *name)
   {FIXNUM rv;
    char lname[MAXLINE];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    SC_FORTRAN_STR_C(lname, name, *sncn);

    rv = PG_write_interface(dev, lname);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGWRTA - print the given message
 *        - at the specified screen coordinate in WC
 */

FIXNUM FF_ID(pgwrta, PGWRTA)(FIXNUM *sdid, double *sx, double *sy,
			     FIXNUM *sncm, char *msg)
   {FIXNUM rv;
    double x, y;
    char lmsg[MAXLINE];
    PG_device *dev;

    x = *sx;
    y = *sy;
    SC_FORTRAN_STR_C(lmsg, msg, *sncm);

    dev = SC_GET_POINTER(PG_device, *sdid);
    if ((dev != NULL) && (dev->gprint_flag))
       {PG_move_tx_abs(dev, x, y);
        PG_write_text(dev, stdscr, lmsg);};

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGWTOS - convert from WC to NDC */

FIXNUM FF_ID(pgwtos, PGWTOS)(FIXNUM *sdid, double *sx, double *sy)
   {FIXNUM rv;
    double p[PG_SPACEDM];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    p[0] = *sx;
    p[1] = *sy;
    PG_trans_point(dev, 2, WORLDC, p, NORMC, p);
    *sx = p[0];
    *sy = p[1];

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSVA - set the view angle */

FIXNUM FF_ID(pgsva, PGSVA)(FIXNUM *sdid, double *st, double *sp, double *sc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    PG_set_view_angle(dev, *st, *sp, *sc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSLA - set the lighting angle */

FIXNUM FF_ID(pgsla, PGSLA)(FIXNUM *sdid, double *st, double *sp)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    PG_set_light_angle(dev, *st, *sp);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGVA - get the view angle */

FIXNUM FF_ID(pggva, PGGVA)(FIXNUM *sdid, FIXNUM *scnv,
			   double *st, double *sp, double *sc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    PG_get_view_angle(dev, *scnv, st, sp, sc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGLA - get the lighting angle */

FIXNUM FF_ID(pggla, PGGLA)(FIXNUM *sdid, FIXNUM *scnv, double *st, double *sp)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    PG_get_light_angle(dev, *scnv, st, sp);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
