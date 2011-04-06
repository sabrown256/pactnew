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

FIXNUM F77_FUNC(pgaxis, PGAXIS)(FIXNUM *devid, FIXNUM *paxt)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_axis(dev, (int) *paxt);

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

FIXNUM F77_FUNC(pgsaxa, PGSAXA)(FIXNUM *devid, FIXNUM *pn,
                                double *pat, F77_string patc)
   {int i, nc, nn, type, linecolor, txtcolor, prec;
    double *attr;
    double charspace, chupx, chupy, chpthx, chpthy;
    char *pc, bf[MAXLINE];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    if (dev == NULL)
       return(FALSE);

/* load default values for external variables */
    linecolor     = dev->WHITE;
    txtcolor      = dev->WHITE;
    prec          = TEXT_CHARACTER_PRECISION;

    if (_PG_gattrs.axis_label_x_format == NULL)
       _PG_gattrs.axis_label_x_format = CSTRSAVE("%10.2g");

    if (_PG_gattrs.axis_label_y_format == NULL)
       _PG_gattrs.axis_label_y_format = CSTRSAVE("%10.2g");

    if (_PG_gattrs.axis_type_face == NULL)
       _PG_gattrs.axis_type_face = CSTRSAVE("helvetica");

    charspace    = 0.0;
    chpthx       = 1.0;
    chpthy       = 0.0;
    chupx        = 0.0;
    chupy        = 1.0;

    _PG_gattrs.axis_grid_on = FALSE;

/* the type of the second part of the pair is dependent on the
 * value of type 
 */
    nn   = *pn;
    pc   = SC_F77_C_STRING(patc);
    attr = pat;
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
                  linecolor = *attr++;
                  break;

             case AXIS_LABELCOLOR :
                  txtcolor = *attr++;
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

    chpthx = sin(_PG_gattrs.axis_char_angle);
    chpthy = cos(_PG_gattrs.axis_char_angle);

/* set attribute values */
    PG_set_clipping(dev, FALSE);
    PG_set_color_text(dev, txtcolor, TRUE);
    PG_set_font(dev, _PG_gattrs.axis_type_face, dev->type_style, dev->type_size);
    PG_set_char_precision(dev, prec);
    PG_set_char_path(dev, chpthx, chpthy);
    PG_set_char_up(dev, chupx, chupy);
    PG_set_char_space(dev, charspace);
    PG_set_color_line(dev, linecolor, TRUE);
    PG_set_line_style(dev, _PG_gattrs.axis_line_style);
    PG_set_line_width(dev, _PG_gattrs.axis_line_width);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGAXD - get axis decades */

FIXNUM F77_FUNC(pggaxd, PGGAXD)(double *pd)
   {

    PG_get_axis_decades(*pd);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGAXL - get log axis flags */

FIXNUM F77_FUNC(pggaxl, PGGAXL)(FIXNUM *devid, FIXNUM *pnd, FIXNUM *iflg)
   {int id, nd;
    int ifl[PG_SPACEDM];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    nd = *pnd;

    PG_get_axis_log_scale(dev, nd, ifl);

    for (id = 0; id < nd; id++)
        iflg[id] = ifl[id];

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGBSZ - return the I/O buffer size */

FIXNUM F77_FUNC(pggbsz, PGGBSZ)(void)
   {FIXNUM rv;
    int64_t sz;

    sz = PG_get_buffer_size();
    rv = sz;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGCLOS - close the PG_device associated with the integer index */

FIXNUM F77_FUNC(pgclos, PGCLOS)(FIXNUM *devid)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_DEL_POINTER(PG_device, *devid);
    PG_close_device(dev);

    *devid = -1;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGCLPG - clear the page */

FIXNUM F77_FUNC(pgclpg, PGCLPG)(FIXNUM *devid, FIXNUM *pi)
   {int i;
    FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    i   = *pi;

    PG_clear_page(dev, i);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGCLRG - clear the specified region */

FIXNUM F77_FUNC(pgclrg, PGCLRG)(FIXNUM *devid, double *pxn, double *pxx,
                                double *pyn, double *pyx, FIXNUM *pad)
   {int pd;
    FIXNUM rv;
    double ndc[PG_BOXSZ];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    pd  = *pad;

    ndc[0] = *pxn;
    ndc[1] = *pxx;
    ndc[2] = *pyn;
    ndc[3] = *pyx;

    PG_clear_region(dev, 2, NORMC, ndc, pd);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGCLSC - clear the screen (necessary for CGM devices) */

FIXNUM F77_FUNC(pgclsc, PGCLSC)(FIXNUM *devid)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_clear_window(dev);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGCLVP - clear the viewport */

FIXNUM F77_FUNC(pgclvp, PGCLVP)(FIXNUM *devid)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_clear_viewport(dev);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDDP2 - draw 2d disjoint polyline */

FIXNUM F77_FUNC(pgddp2, PGDDP2)(FIXNUM *devid, double *px, double *py,
                                FIXNUM *pn, FIXNUM *pf, FIXNUM *pc)
   {int f;
    long n;
    FIXNUM rv;
    double *t[2];
    PG_coord_sys cs;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    t[0] = px;
    t[1] = py;

    f  = (int) *pf;
    n  = (long) *pn;
    cs = (PG_coord_sys) *pc;

    PG_draw_disjoint_polyline_n(dev, 2, cs, n, t, f);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDPL2 - draw 2d polyline */

FIXNUM F77_FUNC(pgdpl2, PGDPL2)(FIXNUM *devid, double *px, double *py,
                                FIXNUM *pn, FIXNUM *pc)
   {FIXNUM rv;
    double *t[2];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    t[0] = px;
    t[1] = py;

    PG_draw_polyline_n(dev, 2, WORLDC, (long) *pn, t, (int) *pc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDDP3 - draw 3d disjoint polyline */

FIXNUM F77_FUNC(pgddp3, PGDDP3)(FIXNUM *devid, double *px, double *py, double *pz,
                                FIXNUM *pn, FIXNUM *pf, FIXNUM *pc)
   {int f;
    long n;
    FIXNUM rv;
    double *t[PG_SPACEDM];
    PG_coord_sys cs;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    t[0] = px;
    t[1] = py;
    t[2] = pz;

    f  = (int) *pf;
    n  = (long) *pn;
    cs = (PG_coord_sys) *pc;

    PG_draw_disjoint_polyline_n(dev, 3, cs, n, t, f);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDRAX - draw a single axis */

FIXNUM F77_FUNC(pgdrax, PGDRAX)(FIXNUM *devid, double *pxl, double *pyl,
                                double *pxr, double *pyr, double *pt1, double *pt2,
                                double *pv1, double *pv2, double *psc,
                                FIXNUM *pnc, F77_string format,
                                FIXNUM *ptd, FIXNUM *ptt, FIXNUM *plt)
   {int td;
    FIXNUM rv;
    double tn[2], vw[2];
    double xl[PG_SPACEDM], xr[PG_SPACEDM];
    char lformat[MAXLINE];
    PG_device *dev;

    SC_FORTRAN_STR_C(lformat, format, *pnc);

    dev = SC_GET_POINTER(PG_device, *devid);

    td = *ptd;

    xl[0] = *pxl;
    xl[1] = *pyl;
    xr[0] = *pxr;
    xr[1] = *pyr;
    tn[0] = *pt1;
    tn[1] = *pt2;
    vw[0] = *pv1;
    vw[1] = *pv2;

    PG_draw_axis_n(dev, xl, xr, tn, vw, *psc, lformat,
		   (int) *ptt, (int) *plt, FALSE,
		   td, 0);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDAX3 - draw a 3D axis set */

FIXNUM F77_FUNC(pgdax3, PGDAX3)(FIXNUM *devid, double *pxl, double *pyl,
                                double *pzl, FIXNUM *pnp)
   {FIXNUM rv;
    double *p[PG_SPACEDM];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    p[0] = pxl;
    p[1] = pyl;
    p[2] = pzl;

    PG_axis_3(dev, p, (int) *pnp, NULL);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDRMK - draw marker characters */

FIXNUM F77_FUNC(pgdrmk, PGDRMK)(FIXNUM *devid, FIXNUM *pn,
			        double *px, double *py, FIXNUM *pmrk)
   {int n, mrk;
    FIXNUM rv;
    double *r[PG_SPACEDM];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    n   = *pn;
    mrk = *pmrk;

    r[0] = px;
    r[1] = py;

    PG_draw_markers_n(dev, 2, WORLDC, n, r, mrk);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDMRK - define a marker character */

FIXNUM F77_FUNC(pgdmrk, PGDMRK)(FIXNUM *pn, double *px1, double *py1,
                                double *px2, double *py2)
   {int n;
    FIXNUM rv;

    n = *pn;

    rv = PG_def_marker(n, px1, py1, px2, py2);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDPLT - draw a mesh - plot a domain */

FIXNUM F77_FUNC(pgdplt, PGDPLT)(FIXNUM *devid, FIXNUM *dom)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_domain_plot(dev, *(PM_set **) dom, NULL);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDRBX - draw a box */

FIXNUM F77_FUNC(pgdrbx, PGDRBX)(FIXNUM *devid, double *px1, double *px2,
			        double *py1, double *py2)
   {FIXNUM rv;
    double bx[PG_BOXSZ];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    bx[0] = *px1;
    bx[1] = *px2;
    bx[2] = *py1;
    bx[3] = *py2;
    PG_draw_box_n(dev, 2, WORLDC, bx);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDRIF - draw the set of interface objects for the device */

FIXNUM F77_FUNC(pgdrif, PGDRIF)(FIXNUM *devid)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_draw_interface_objects(dev);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDRLN - draw a line between the specified points */

FIXNUM F77_FUNC(pgdrln, PGDRLN)(FIXNUM *devid, double *px1, double *py1,
				double *px2, double *py2)
   {FIXNUM rv;
    double x1[PG_SPACEDM], x2[PG_SPACEDM];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    x1[0] = *px1;
    x1[1] = *py1;
    x2[0] = *px2;
    x2[1] = *py2;
    PG_draw_line_n(dev, 2, WORLDC, x1, x2, dev->clipping);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDRPA - draw the current palette */

FIXNUM F77_FUNC(pgdrpa, PGDRPA)(FIXNUM *devid, double *px1, double *py1,
				double *px2, double *py2, double *pz1, double *pz2,
				double *pw)
   {FIXNUM rv;
    double dbx[PG_BOXSZ], rbx[PG_BOXSZ];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    dbx[0] = *px1;
    dbx[1] = *px2;
    dbx[2] = *py1;
    dbx[3] = *py2;
    rbx[0] = *pz1;
    rbx[1] = *pz2;

    PG_draw_palette_n(dev, dbx, rbx, *pw, FALSE);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGDRPP - draw the current palette */

FIXNUM F77_FUNC(pgdrpp, PGDRPP)(FIXNUM *devid, double *px1, double *py1,
				double *px2, double *py2, double *pz1, double *pz2,
				double *pw, FIXNUM *pe)
   {FIXNUM rv;
    double dbx[PG_BOXSZ], rbx[PG_BOXSZ];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    dbx[0] = *px1;
    dbx[1] = *px2;
    dbx[2] = *py1;
    dbx[3] = *py2;
    rbx[0] = *pz1;
    rbx[1] = *pz2;

    PG_draw_palette_n(dev, dbx, rbx, *pw, *pe);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGFNPL - finish the plot (necessary for CGM devices) */

FIXNUM F77_FUNC(pgfnpl, PGFNPL)(FIXNUM *devid)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_finish_plot(dev);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGFPLY - draw and fill the specified polygon with the specified color */

FIXNUM F77_FUNC(pgfply, PGFPLY)(FIXNUM *devid, double *px, double *py,
			        FIXNUM *pn, FIXNUM *pc)
   {FIXNUM rv;
    double *r[2];
    PG_device *dev;

    r[0] = px;
    r[1] = py;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_fill_polygon_n(dev, (int) *pc, TRUE, 2, WORLDC, *pn, r);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGCLM - get the clear mode */

FIXNUM F77_FUNC(pggclm, PGGCLM)(FIXNUM *devid, FIXNUM *pc)
   {FIXNUM rv;

    PG_get_clear_mode(*pc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGCLP - get the clipping */

FIXNUM F77_FUNC(pggclp, PGGCLP)(FIXNUM *devid, FIXNUM *pc)
   {int flg;
    FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_get_clipping(dev, &flg);

    *pc = flg;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGCPW - get the char path direction in WC */

FIXNUM F77_FUNC(pggcpw, PGGCPW)(FIXNUM *devid, double *px, double *py)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_get_char_path(dev, px, py);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGCSS - get the char size in NDC */

FIXNUM F77_FUNC(pggcss, PGGCSS)(FIXNUM *devid, double *pw, double *ph)
   {FIXNUM rv;
    double p[PG_SPACEDM];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_get_char_size_n(dev, 2, NORMC, p);

    *pw = p[0];
    *ph = p[1];

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGCSW - get the char size in WC */

FIXNUM F77_FUNC(pggcsw, PGGCSW)(FIXNUM *devid, double *pw, double *ph)
   {FIXNUM rv;
    double p[PG_SPACEDM];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_get_char_size_n(dev, 2, WORLDC, p);

    *pw = p[0];
    *ph = p[1];

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGCUW - get the char up direction in WC */

FIXNUM F77_FUNC(pggcuw, PGGCUW)(FIXNUM *devid, double *px, double *py)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_get_char_up(dev, px, py);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGDPI - get the PostScript dots per inch for 8.5 x 11 page */

FIXNUM F77_FUNC(pggdpi, PGGDPI)(double *dpi)
   {FIXNUM rv;

    PG_get_ps_dots_inch(*dpi);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGFIN - get the finished status */

FIXNUM F77_FUNC(pggfin, PGGFIN)(FIXNUM *devid, FIXNUM *pc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    *pc = dev->finished;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGLNC - get the line color */

FIXNUM F77_FUNC(pgglnc, PGGLNC)(FIXNUM *devid, FIXNUM *pc)
   {int lc;
    FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_get_line_color(dev, &lc);

    *pc = lc;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGLOP - get the logical operation */

FIXNUM F77_FUNC(pgglop, PGGLOP)(FIXNUM *devid, FIXNUM *plop)
   {FIXNUM rv;
    PG_logical_operation lop;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_get_logical_op(dev, &lop);

    *plop = lop;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGLNS - get the line style */

FIXNUM F77_FUNC(pgglns, PGGLNS)(FIXNUM *devid, FIXNUM *ps)
   {int ls;
    FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_get_line_style(dev, &ls);

    *ps = ls;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGLNW - get the line width */

FIXNUM F77_FUNC(pgglnw, PGGLNW)(FIXNUM *devid, double *pw)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_get_line_width(dev, pw);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGMKO - get the marker orientation */

FIXNUM F77_FUNC(pggmko, PGGMKO)(FIXNUM *devid, double *pw)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_get_marker_orientation(dev, *pw);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGMKS - get the marker scale */

FIXNUM F77_FUNC(pggmks, PGGMKS)(FIXNUM *devid, double *ps)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_get_marker_scale(dev, *ps);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGPMF - get the pixmap flag */

FIXNUM F77_FUNC(pggpmf, PGGPMF)(FIXNUM *devid, FIXNUM *pc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_get_pixmap_flag(dev, *pc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGTLN - get a line of text from the input descriptor */

FIXNUM F77_FUNC(pggtln, PGGTLN)(FIXNUM *pnc, F77_string s, FIXNUM *pfd)
   {FIXNUM rv;
    long fd;
    char *t;
    SC_address f;

    fd = (long) *pfd;

    if (fd == 0L)
       t = GETLN(SC_F77_C_STRING(s), (int) *pnc, stdin);

    else
       {f.diskaddr = fd;
        t = GETLN(SC_F77_C_STRING(s), (int) *pnc, (FILE *) f.memaddr);};

    rv = (t != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGTXC - get the text color */

FIXNUM F77_FUNC(pggtxc, PGGTXC)(FIXNUM *devid, FIXNUM *pc)
   {int lc;
    FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_get_text_color(dev, &lc);

    *pc = lc;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGTXF - get the font */

FIXNUM F77_FUNC(pggtxf, PGGTXF)(FIXNUM *devid, FIXNUM *pncf,
				F77_string pf, FIXNUM *pncs, F77_string pst,
				FIXNUM *psz)
   {int ls, nf, ns, lnf, lns;
    FIXNUM rv;
    char *f, *st;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_get_font(dev, &f, &st, &ls);

    nf = strlen(f);
    ns = strlen(st);

/* get the buffer sizes */
    lnf = *pncf;
    lns = *pncs;

/* return the actual string lengths */
    *pncf = nf;
    *pncs = ns;

    if ((nf > lnf) || (ns > lns))
       rv = FALSE;

    else
       {SC_strncpy(SC_F77_C_STRING(pf), *pncf, f, lnf);
        SC_strncpy(SC_F77_C_STRING(pst), *pncs, st, lns);
        *psz = ls;

        CFREE(f);
        CFREE(st);

        rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGTEX - get the text extent in CS */

FIXNUM F77_FUNC(pggtex, PGGTEX)(FIXNUM *devid, FIXNUM *pnd, FIXNUM *pc,
				FIXNUM *pnc, F77_string s,
			        double *pdx, double *pdy)
   {FIXNUM rv;
    double p[PG_SPACEDM];
    char ls[MAXLINE];
    PG_coord_sys cs;
    PG_device *dev;

    SC_FORTRAN_STR_C(ls, s, *pnc);

    dev = SC_GET_POINTER(PG_device, *devid);

    cs = (PG_coord_sys) *pc;

    PG_get_text_ext_n(dev, *pnd, cs, ls, p);

    *pdx = p[0];
    *pdy = p[1];

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGVWP - get the viewport */

FIXNUM F77_FUNC(pggvwp, PGGVWP)(FIXNUM *devid, double *px1, double *px2,
				double *py1, double *py2)
   {FIXNUM rv;
    double ndc[PG_BOXSZ];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    PG_get_viewspace(dev, WORLDC, ndc);
    *px1 = ndc[0];
    *px2 = ndc[1];
    *py1 = ndc[2];
    *py2 = ndc[3];

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGWCS - get the world coordinate system */

FIXNUM F77_FUNC(pggwcs, PGGWCS)(FIXNUM *devid, double *px1, double *px2,
				double *py1, double *py2)
   {FIXNUM rv;
    double wc[PG_BOXSZ];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    PG_get_viewspace(dev, WORLDC, wc);
    *px1 = wc[0];
    *px2 = wc[1];
    *py1 = wc[2];
    *py2 = wc[3];

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGMDVC - make the device current */

FIXNUM F77_FUNC(pgmdvc, PGMDVC)(FIXNUM *devid)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
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

FIXNUM F77_FUNC(pgmkdv, PGMKDV)(FIXNUM *pncn, F77_string name,
				FIXNUM *pnct, F77_string type,
				FIXNUM *pncl, F77_string title)
   {FIXNUM rv;
    char lname[MAXLINE], ltype[MAXLINE], ltitle[MAXLINE];
    PG_device *dev;

    SC_FORTRAN_STR_C(lname, name, *pncn);
    SC_FORTRAN_STR_C(ltype, type, *pnct);
    SC_FORTRAN_STR_C(ltitle, title, *pncl);

    dev = PG_make_device(lname, ltype, ltitle);
    if (dev == NULL)
       rv = -1;

    else
       rv = SC_ADD_POINTER(dev);

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGOPEN - open the specified PG_device */

FIXNUM F77_FUNC(pgopen, PGOPEN)(FIXNUM *devid, double *pxf, double *pyf,
				double *pdxf, double *pdyf)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    if (PG_open_device(dev, *pxf, *pyf, *pdxf, *pdyf) == NULL)
       rv = FALSE;
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGPTIN - given a pointer, return its index
 *        - return -1 otherwise
 */

FIXNUM F77_FUNC(pgptin, PGPTIN)(FIXNUM *pntr, FIXNUM *indx)
   {FIXNUM rv;

    rv = (*indx = SC_GET_INDEX((void *) pntr));

    return(rv);}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGQDEV - get some physical parameters from the specified PG_device */

FIXNUM F77_FUNC(pgqdev, PGQDEV)(FIXNUM *devid, FIXNUM *pdx, FIXNUM *pdy,
				FIXNUM *pnc)
   {int dx, dy, nc;
    FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_query_device(dev, &dx, &dy, &nc);

    *pdx = dx;
    *pdy = dy;
    *pnc = nc;

    rv = TRUE;

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGQWIN - get the window shape of the specified device */

FIXNUM F77_FUNC(pgqwin, PGQWIN)(FIXNUM *devid, FIXNUM *pdx, FIXNUM *pdy)
   {int dx, dy;
    FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_query_window(dev, &dx, &dy);

    *pdx = dx;
    *pdy = dy;

    rv = TRUE;

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGPLLN - low level line plot routine */

FIXNUM F77_FUNC(pgplln, PGPLLN)(FIXNUM *devid, double *px, double *py,
			        FIXNUM *pn, FIXNUM *ppty, FIXNUM *paxt,
				FIXNUM *pcol, double *pwid,
			        FIXNUM *psty, FIXNUM *psca, FIXNUM *pmrk,
				FIXNUM *psta, FIXNUM *pl)
   {int axt, sty, sca, mrk, clr, sta;
    long n, l;
    FIXNUM rv;
    PG_rendering pty;
    PG_device *dev;
    pcons *info;

    dev = SC_GET_POINTER(PG_device, *devid);

    n   = *pn;
    l   = *pl;
    axt = *paxt;
    sty = *psty;
    sca = *psca;
    mrk = *pmrk;
    clr = *pcol;
    sta = *psta;
    pty = (PG_rendering) *ppty;

    info = PG_set_line_info(NULL, pty, axt, sty, sca, mrk, clr, sta, *pwid);
    PG_plot_curve(dev, px, py, n, info, l);

    SC_free_alist(info, 2);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGPTOS - convert from PC to NDC */

FIXNUM F77_FUNC(pgptos, PGPTOS)(FIXNUM *devid, FIXNUM *pix, FIXNUM *piy,
			        double *px, double *py)
   {FIXNUM rv;
    double p[PG_SPACEDM];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    p[0] = *pix;
    p[1] = *piy;
    PG_trans_point(dev, 2, PIXELC, p, NORMC, p);
    *px = p[0];
    *py = p[1];

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGRDIF - read an interface description file */

FIXNUM F77_FUNC(pgrdif, PGRDIF)(FIXNUM *devid, FIXNUM *pnc, F77_string name)
   {FIXNUM rv;
    char lname[MAXLINE];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    SC_FORTRAN_STR_C(lname, name, *pnc);

    rv = PG_read_interface(dev, lname);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGRDVC - release device as the device current */

FIXNUM F77_FUNC(pgrdvc, PGRDVC)(FIXNUM *devid)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_release_current_device(dev);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGRGDV - register a device */

FIXNUM F77_FUNC(pgrgdv, PGRGDV)(FIXNUM *pnc, F77_string name)
   {FIXNUM rv;
    char lname[MAXLINE];
    
    SC_FORTRAN_STR_C(lname, name, *pnc);

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

FIXNUM F77_FUNC(pgrgfn, PGRGFN)(FIXNUM *pnc, F77_string name,
				PFEvCallback fnc)
   {FIXNUM rv;
    char lname[MAXLINE];

    SC_FORTRAN_STR_C(lname, name, *pnc);
    PG_register_callback(lname, fnc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGRGVR - register a variable with the device */

FIXNUM F77_FUNC(pgrgvr, PGRGVR)(FIXNUM *pnc, F77_string name,
				FIXNUM *pnt, F77_string type,
				void *vr, void *vn, void *vx)
   {FIXNUM rv;
    char lname[MAXLINE], ltype[MAXLINE];

    SC_FORTRAN_STR_C(lname, name, *pnc);
    SC_FORTRAN_STR_C(ltype, type, *pnt);

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

FIXNUM F77_FUNC(pgrvpa, PGRVPA)(FIXNUM *devid, FIXNUM *vwatid)
   {FIXNUM rv;
    PG_device *dev;
    PG_view_attributes *d;

    rv = FALSE;

    dev = SC_GET_POINTER(PG_device, *devid);
    if (dev != NULL)
       {d = PG_view_attributes_pointer((int) *vwatid);

	PG_restore_view_attributes(dev, d);

	rv = TRUE;};

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSADR - set the auto domain and range */

FIXNUM F77_FUNC(pgsadr, PGSADR)(FIXNUM *devid, FIXNUM *pd, FIXNUM *pr)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    dev->autodomain = *pd;
    dev->autorange  = *pr;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSADM - set the auto domain */

FIXNUM F77_FUNC(pgsadm, PGSADM)(FIXNUM *devid, FIXNUM *pd)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    dev->autodomain = *pd;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSARN - set the auto range */

FIXNUM F77_FUNC(pgsarn, PGSARN)(FIXNUM *devid, FIXNUM *pr)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    dev->autorange  = *pr;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSAXD - set axis decades */

FIXNUM F77_FUNC(pgsaxd, PGSAXD)(double *pd)
   {FIXNUM rv;
    double d;

    d = *pd;

    PG_set_axis_decades(d);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSAXL - set log axis flags */

FIXNUM F77_FUNC(pgsaxl, PGSAXL)(FIXNUM *devid, FIXNUM *pnd, FIXNUM *iflg)
   {int id, nd;
    int ifl[PG_SPACEDM];
    FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    nd = *pnd;

    for (id = 0; id < nd; id++)
        ifl[id] = iflg[id];

    PG_set_axis_log_scale(dev, nd, ifl);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSBWD - set the border width */

FIXNUM F77_FUNC(pgsbwd, PGSBWD)(FIXNUM *devid, FIXNUM *pw)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    PG_set_border_width(dev, *pw);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSBSZ - set the I/O buffer size */

FIXNUM F77_FUNC(pgsbsz, PGSBSZ)(FIXNUM *psz)
   {FIXNUM rv;
    int64_t sz;

    sz = *psz;
    PG_set_buffer_size(sz);
    rv = sz;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSCLM - set the clear mode */

FIXNUM F77_FUNC(pgsclm, PGSCLM)(FIXNUM *devid, FIXNUM *pc)
   {FIXNUM rv;

    PG_set_clear_mode(*pc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSCLP - set the clipping */

FIXNUM F77_FUNC(pgsclp, PGSCLP)(FIXNUM *devid, FIXNUM *pc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_set_clipping(dev, (int) *pc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSCPW - set the char path direction in WC */

FIXNUM F77_FUNC(pgscpw, PGSCPW)(FIXNUM *devid, double *px, double *py)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_set_char_path(dev, (double) *px, (double) *py);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSCUW - set the char up direction in WC */

FIXNUM F77_FUNC(pgscuw, PGSCUW)(FIXNUM *devid, double *px, double *py)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_set_char_up(dev, (double) *px, (double) *py);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSDPI - set the PostScript dots per inch for 8.5 x 11 page */

FIXNUM F77_FUNC(pgsdpi, PGSDPI)(double *dpi)
   {FIXNUM rv;

    PG_set_ps_dots_inch(*dpi);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSELC - select a color from the n-dimensional palette */

FIXNUM F77_FUNC(pgselc, PGSELC)(FIXNUM *devid, FIXNUM *n, double *av,
				double *an, double *ax)
   {FIXNUM rv;
    double extr[2];
    PG_device *dev;

    extr[0] = *an;
    extr[1] = *ax;
    
    dev = SC_GET_POINTER(PG_device, *devid);

    rv = PG_select_color(dev, *n, av, extr);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSFCL - set the fill color */

FIXNUM F77_FUNC(pgsfcl, PGSFCL)(FIXNUM *devid, FIXNUM *pc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_set_fill_color(dev, *pc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSFIN - set the finished status */

FIXNUM F77_FUNC(pgsfin, PGSFIN)(FIXNUM *devid, FIXNUM *pc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    dev->finished = *pc;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSLNC - set the line color */

FIXNUM F77_FUNC(pgslnc, PGSLNC)(FIXNUM *devid, FIXNUM *pc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_set_color_line(dev, (int) *pc, TRUE);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSLOP - set the logical operation */

FIXNUM F77_FUNC(pgslop, PGSLOP)(FIXNUM *devid, FIXNUM *plop)
   {FIXNUM rv;
    PG_logical_operation lop;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    lop = (PG_logical_operation) *plop;
    PG_set_logical_op(dev, lop);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSLNS - set the line style */

FIXNUM F77_FUNC(pgslns, PGSLNS)(FIXNUM *devid, FIXNUM *ps)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_set_line_style(dev, (int) *ps);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSLNW - set the line width */

FIXNUM F77_FUNC(pgslnw, PGSLNW)(FIXNUM *devid, double *pw)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_set_line_width(dev, (double) *pw);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSMKO - set the marker orientation */

FIXNUM F77_FUNC(pgsmko, PGSMKO)(FIXNUM *devid, double *pw)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_set_marker_orientation(dev, (double) *pw);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSMKS - set the marker scale */

FIXNUM F77_FUNC(pgsmks, PGSMKS)(FIXNUM *devid, double *ps)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_set_marker_scale(dev, (double) *ps);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSPAL - set the current palette */

FIXNUM F77_FUNC(pgspal, PGSPAL)(FIXNUM *devid, FIXNUM *pnc, F77_string pname)
   {FIXNUM rv;
    PG_device *dev;
    char lname[MAXLINE];

    dev = SC_GET_POINTER(PG_device, *devid);
    SC_FORTRAN_STR_C(lname, pname, *pnc);

    rv = (PG_set_palette(dev, lname) != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSPMF - set the pixmap flag */

FIXNUM F77_FUNC(pgspmf, PGSPMF)(FIXNUM *devid, FIXNUM *pc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_set_pixmap_flag(dev, *pc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSTOP - convert from NDC to PC */

FIXNUM F77_FUNC(pgstop, PGSTOP)(FIXNUM *devid, double *px, double *py,
			        FIXNUM *pix, FIXNUM *piy)
   {FIXNUM rv;
    double p[PG_SPACEDM];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    p[0] = *px;
    p[1] = *py;
    PG_trans_point(dev, 2, NORMC, p, PIXELC, p);
    *pix = p[0];
    *piy = p[1];

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSTOW - convert from NDC to WC */

FIXNUM F77_FUNC(pgstow, PGSTOW)(FIXNUM *devid, double *px, double *py)
   {FIXNUM rv;
    double p[PG_SPACEDM];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    p[0] = *px;
    p[1] = *py;
    PG_trans_point(dev, 2, NORMC, p, WORLDC, p);
    *px = p[0];
    *py = p[1];

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSTXC - set the text color */

FIXNUM F77_FUNC(pgstxc, PGSTXC)(FIXNUM *devid, FIXNUM *pc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_set_color_text(dev, (int) *pc, TRUE);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSTXF - set the font */

FIXNUM F77_FUNC(pgstxf, PGSTXF)(FIXNUM *devid, FIXNUM *pncf,
				F77_string face, FIXNUM *pncs,
				F77_string style, FIXNUM *psize)
   {FIXNUM rv;
    PG_device *dev;
    char lface[MAXLINE], lstyle[MAXLINE];

    dev = SC_GET_POINTER(PG_device, *devid);

    SC_FORTRAN_STR_C(lface, face, *pncf);
    SC_FORTRAN_STR_C(lstyle, style, *pncs);

    PG_set_font(dev, lface, lstyle, (int) *psize);

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

FIXNUM F77_FUNC(pgsvpa, PGSVPA)(FIXNUM *devid, FIXNUM *pn)
   {int n;
    FIXNUM rv;
    PG_device *dev;
    PG_view_attributes *d;

    dev = SC_GET_POINTER(PG_device, *devid);
    n   = *pn;
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

    *pn = n;

    rv = TRUE;

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSVWP - set the viewport */

FIXNUM F77_FUNC(pgsvwp, PGSVWP)(FIXNUM *devid, double *px1, double *px2,
				double *py1, double *py2)
   {FIXNUM rv;
    double ndc[PG_BOXSZ];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    ndc[0] = *px1;
    ndc[1] = *px2;
    ndc[2] = *py1;
    ndc[3] = *py2;

    PG_set_viewspace(dev, 2, NORMC, ndc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSVPS - set viewport position */

FIXNUM F77_FUNC(pgsvps, PGSVPS)(FIXNUM *devid, double *px, double *py)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    PG_set_viewport_pos(dev, *px, *py);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSVSH - set viewport shape */

FIXNUM F77_FUNC(pgsvsh, PGSVSH)(FIXNUM *devid, double *pw, double *ph, double *pa)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    PG_set_viewport_shape(dev, *pw, *ph, *pa);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSWCS - set the world coordinate system */

FIXNUM F77_FUNC(pgswcs, PGSWCS)(FIXNUM *devid, double *px1, double *px2,
				double *py1, double *py2)
   {FIXNUM rv;
    double wc[PG_BOXSZ];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    wc[0] = *px1;
    wc[1] = *px2;
    wc[2] = *py1;
    wc[3] = *py2;
    PG_set_viewspace(dev, 2, WORLDC, wc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGTDID - turn device data id on or off */

FIXNUM F77_FUNC(pgtdid, PGTDID)(FIXNUM *devid, FIXNUM *val)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    PG_turn_data_id(dev, *val);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGUPVS - update the view surface of the PG_device */

FIXNUM F77_FUNC(pgupvs, PGUPVS)(FIXNUM *devid)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_update_vs(dev);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGWRCL - write label centered wrt x */

FIXNUM F77_FUNC(pgwrcl, PGWRCL)(FIXNUM *devid, double *psy,
				FIXNUM *pnc, F77_string label)
   {FIXNUM rv;
    char llabel[MAXLINE];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    SC_FORTRAN_STR_C(llabel, label, *pnc);

    PG_center_label(dev, *psy, llabel);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGWRIF - write an interface description file */

FIXNUM F77_FUNC(pgwrif, PGWRIF)(FIXNUM *devid, FIXNUM *pnc, F77_string name)
   {FIXNUM rv;
    char lname[MAXLINE];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    SC_FORTRAN_STR_C(lname, name, *pnc);

    rv = PG_write_interface(dev, lname);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGWRTA - print the given message
 *        - at the specified screen coordinate in WC
 */

FIXNUM F77_FUNC(pgwrta, PGWRTA)(FIXNUM *devid, double *px, double *py,
				FIXNUM *pnc, F77_string msg)
   {FIXNUM rv;
    double x, y;
    char lmsg[MAXLINE];
    PG_device *dev;

    x = *px;
    y = *py;
    SC_FORTRAN_STR_C(lmsg, msg, *pnc);

    dev = SC_GET_POINTER(PG_device, *devid);
    if ((dev != NULL) && (dev->gprint_flag))
       {PG_move_tx_abs(dev, x, y);
        PG_write_text(dev, stdscr, lmsg);};

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGWTOS - convert from WC to NDC */

FIXNUM F77_FUNC(pgwtos, PGWTOS)(FIXNUM *devid, double *px, double *py)
   {FIXNUM rv;
    double p[PG_SPACEDM];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    p[0] = *px;
    p[1] = *py;
    PG_trans_point(dev, 2, WORLDC, p, NORMC, p);
    *px = p[0];
    *py = p[1];

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSVA - set the view angle */

FIXNUM F77_FUNC(pgsva, PGSVA)(FIXNUM *devid, double *pt, double *pp, double *pc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    PG_set_view_angle(dev, *pt, *pp, *pc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSLA - set the lighting angle */

FIXNUM F77_FUNC(pgsla, PGSLA)(FIXNUM *devid, double *pt, double *pp)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    PG_set_light_angle(dev, *pt, *pp);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGVA - get the view angle */

FIXNUM F77_FUNC(pggva, PGGVA)(FIXNUM *devid, FIXNUM *pcnv,
			      double *pt, double *pp, double *pc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    PG_get_view_angle(dev, *pcnv, pt, pp, pc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGLA - get the lighting angle */

FIXNUM F77_FUNC(pggla, PGGLA)(FIXNUM *devid, FIXNUM *pcnv, double *pt, double *pp)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    PG_get_light_angle(dev, *pcnv, pt, pp);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
