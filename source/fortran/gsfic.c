/*
 * GSFIC.C - FORTRAN interface routines for PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "fpact.h"

/*--------------------------------------------------------------------------*/

/*                              CONTOUR ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* PGCLEV - compute contour levels */

FIXNUM FF_ID(pgclev, PGCLEV)(double *lev, FIXNUM *sn,
			     double *sfn, double *sfx, double *sr)
   {FIXNUM rv;
    
    rv = PG_contour_levels(lev, (int) *sn, *sfn, *sfx, *sr);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGPLCN - low level contour plot routine */

FIXNUM FF_ID(pgplcn, PGPLCN)(FIXNUM *sdid, double *ax, double *ay,
			     double *aa, double *al,
			     FIXNUM *skx, FIXNUM *slx,
			     FIXNUM *snl, FIXNUM *sli, FIXNUM *said)
   {int maxes[2];
    FIXNUM rv;
    double *x[2];
    pcons *alst;
    PG_device *dev;

    maxes[0] = *skx;
    maxes[1] = *slx;

    dev  = SC_GET_POINTER(PG_device, *sdid);
    alst = SC_GET_POINTER(pcons, *said);

    x[0] = ax;
    x[1] = ay;

    PG_draw_iso_nc_lr(dev, aa, 2, x, al,
                      *snl, *sli, (void *) maxes, alst);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                               IMAGE ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* PGPLIM - low level image plot routine */

FIXNUM FF_ID(pgplim, PGPLIM)(FIXNUM *sdid, FIXNUM *sncn, char *name,
			     FIXNUM *snct, char *type,
			     double *az, FIXNUM *sk, FIXNUM *sl,
			     double *sxn, double *sxx,
			     double *syn, double *syx,
			     double *szn, double *szx, FIXNUM *said)
   {int maxes[2];
    FIXNUM rv;
    double frm[PG_BOXSZ];
    char lname[MAXLINE], ltype[MAXLINE];
    pcons *alst;
    PG_device *dev;

    SC_FORTRAN_STR_C(lname, name, *sncn);
    SC_FORTRAN_STR_C(ltype, type, *snct);

    maxes[0] = *sk;
    maxes[1] = *sl;

    dev  = SC_GET_POINTER(PG_device, *sdid);
    alst = SC_GET_POINTER(pcons, *said);

    _PG_draw_image_nc_lr(dev, lname, ltype, az, frm, maxes, alst);

    *sxn = frm[0];
    *sxx = frm[1];
    *syn = frm[2];
    *syx = frm[3];
    *szn = frm[4];
    *szx = frm[5];

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                              SURFACE ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* PGPLSF - low level surface plot routine */

FIXNUM FF_ID(pgplsf, PGPLSF)(FIXNUM *sdid, double *ax, double *ay, double *az,
			     FIXNUM *sn, double *sxn, double *sxx,
			     double *syn, double *syx, double *szn, double *szx,
			     FIXNUM *skx, FIXNUM *slx,
			     double *sth, double *sph, double *sch,
			     FIXNUM *styp, FIXNUM *scol, double *swid,
			     FIXNUM *ssty, FIXNUM *sncl, char *label)
   {int sty, clr;
    int maxes[2];
    FIXNUM rv;
    double ext[PG_BOXSZ], va[PG_SPACEDM];
    double *r[PG_SPACEDM];
    char llabel[MAXLINE];
    PG_rendering pty;
    PG_device *dev;

    maxes[0] = (int) *skx;
    maxes[1] = (int) *slx;

    clr = *scol;
    sty = *ssty;
    pty = (PG_rendering) *styp;

    ext[0] = *szn;
    ext[1] = *szx;
    ext[2] = *szn;
    ext[3] = *szx; 

    SC_FORTRAN_STR_C(llabel, label, *sncl);

    r[0] = ax;
    r[1] = ay;

    va[0] = *sth;
    va[1] = *sph;
    va[2] = *sch;

    dev = SC_GET_POINTER(PG_device, *sdid);

    PG_draw_surface_n(dev, az, az, ext, r, *sn,
		      va, *swid, clr, sty,
		      pty, llabel, PM_LR_S, maxes, NULL);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                               VECTOR ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* PGSVAT - set the properties of the vectors for the
 *        - next vector plot
 *        - (see the documentation for PD_SET_VEC_ATTR)
 */

void FF_ID(pgsvat, PGSVAT)(FIXNUM *sdid, ...)
   {int type, itemp;
    double diffvp, diffwd, temp;
    double wc[PG_BOXSZ];
    PG_device *dev;

    dev = *(PG_device **) sdid;

/* get the current viewport and window -- this is used to go from NDC to
 * WC for specifying _PG.fixsize and _PG.maxvsz 
 */
    PG_get_viewspace(dev, NORMC, wc);
    diffvp = wc[1] - wc[0];
    PG_get_viewspace(dev, WORLDC, wc);
    diffwd = wc[1] - wc[0];

/* get the attributes */
    SC_VA_START(sdid);
    while (TRUE)
       {type = *SC_VA_ARG(FIXNUM *);
        if (type == 0)
           break;

        switch (type)
           {case VEC_SCALE :
	         _PG.scale = (double) *SC_VA_ARG(double *);
		 break;

            case VEC_ANGLE :
	         _PG.hdangle = (double) *SC_VA_ARG(double *);
		 break;

            case VEC_FIXHEAD :
	         _PG.ifixhead = *SC_VA_ARG(FIXNUM *);
		 break;

            case VEC_HEADSIZE :
	         _PG.headsize = (double) *SC_VA_ARG(double *);
		 break;

            case VEC_FIXSIZE :
	         temp = *SC_VA_ARG(double *);
		 if (temp > 0.0)
		    {_PG.ifix = TRUE;
		     _PG.fixsize = temp*diffwd/diffvp;};
		 break;

            case VEC_MAXSIZE :
	         temp = *SC_VA_ARG(double *);
		 if (temp > 0.0)
		    {_PG.maxvsz = TRUE;
		     _PG.sizemax = temp*diffwd/diffvp;}
		 else
		    {_PG.maxvsz = FALSE;
		     _PG.sizemax = 0.0;};
		 break;

            case VEC_LINESTYLE :
	         itemp = *SC_VA_ARG(FIXNUM *);
		 PG_fset_line_style(dev, itemp);
		 break;

            case VEC_LINETHICK :
	         temp = *SC_VA_ARG(double *);
		 PG_fset_line_width(dev, temp);
		 break;

            case VEC_COLOR :
	         itemp = *SC_VA_ARG(FIXNUM *);
		 PG_fset_line_color(dev, itemp, TRUE);
		 break;

            default :
	         break;};};

    SC_VA_END;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGPLVC - low level vector plot routine */

FIXNUM FF_ID(pgplvc, PGPLVC)(FIXNUM *sdid,
			     double *ax, double *ay, double *au, double *av,
			     FIXNUM *sn, FIXNUM *said)
   {FIXNUM rv;
    double *x[PG_SPACEDM], *u[PG_SPACEDM];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    x[0] = ax;
    x[1] = ay;
    u[0] = au;
    u[1] = av;

    PG_draw_vector_n(dev, 2, WORLDC, *sn, x, u);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
