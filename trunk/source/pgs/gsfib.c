/*
 * GSFIB.C - FORTRAN interface routines for PGS
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

/* PGGBKC - get the white background flag */

FIXNUM FF_ID(pggbkc, PGGBKC)(FIXNUM *devid, FIXNUM *clr)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    *clr = dev->background_color_white;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGINF - get the graph attribute list */

FIXNUM FF_ID(pgginf, PGGINF)(FIXNUM *gid, FIXNUM *pal)
   {FIXNUM rv;
    PG_graph *g;
    pcons *alist;

    g = SC_GET_POINTER(PG_graph, *gid);

    PG_get_render_info(g, alist);
    *pal = (FIXNUM) SC_ADD_POINTER(alist);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGGID - get the graph identifier */

FIXNUM FF_ID(pgggid, PGGGID)(FIXNUM *gid, FIXNUM *pc)
   {FIXNUM rv;
    PG_graph *g;

    g = SC_GET_POINTER(PG_graph, *gid);
    PG_get_identifier(g, *pc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGUPM - get the use-pixmap flag (X device) */

FIXNUM FF_ID(pggupm, PGGUPM)(FIXNUM *flg)
   {FIXNUM rv;

    PG_get_use_pixmap(*flg);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGMG11 - make a 1D-1D graph */

FIXNUM FF_ID(pgmg11, PGMG11)(FIXNUM *pid,
                                FIXNUM *pnl, char *flabel,
                                FIXNUM *pcp, FIXNUM *pn, double *x, double *y,
                                FIXNUM *pnx, char *fxname,
                                FIXNUM *pny, char *fyname)
   {int id, cp, n;
    FIXNUM rv;
    char label[MAXLINE], xname[MAXLINE], yname[MAXLINE];
    PG_graph *g;

    rv = -1;

    SC_FORTRAN_STR_C(label, flabel, *pnl);
    SC_FORTRAN_STR_C(xname, fxname, *pnx);
    SC_FORTRAN_STR_C(yname, fyname, *pny);

    id = *pid;
    cp = *pcp;
    n  = *pn;

    g = PG_make_graph_1d(id, label, cp, n, x, y, xname, yname);

    if (g != NULL)
       rv = SC_ADD_POINTER(g);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGMG21 - make a 2D-1D graph */

FIXNUM FF_ID(pgmg21, PGMG21)(FIXNUM *pid,
                                FIXNUM *pnl, char *flabel,
                                FIXNUM *pcp, FIXNUM *pk, FIXNUM *pl,
                                FIXNUM *pcen, double *x, double *y, double *r,
                                FIXNUM *pnd, char *fdname,
                                FIXNUM *pnr, char *frname)
   {int id, cp, kmax, lmax;
    FIXNUM rv;
    char label[MAXLINE], dname[MAXLINE], rname[MAXLINE];
    PM_centering centering;
    PG_graph *g;

    rv = -1;

    SC_FORTRAN_STR_C(label, flabel, *pnl);
    SC_FORTRAN_STR_C(dname, fdname, *pnd);
    SC_FORTRAN_STR_C(rname, frname, *pnr);

    id        = *pid;
    cp        = *pcp;
    kmax      = *pk;
    lmax      = *pl;
    centering = (PM_centering) *pcen;

    g = PG_make_graph_r2_r1(id, label, cp, kmax, lmax, centering,
			    x, y, r, dname, rname);

    if (g != NULL)
       rv = SC_ADD_POINTER(g);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGMG22 - make a 2D-2D graph */

FIXNUM FF_ID(pgmg22, PGMG22)(FIXNUM *pid,
                                FIXNUM *pnl, char *flabel,
                                FIXNUM *pcp, FIXNUM *pk, FIXNUM *pl,
                                FIXNUM *pcen,
                                double *x, double *y, double *u, double *v,
                                FIXNUM *pnd, char *fdname,
                                FIXNUM *pnr, char *frname)
   {int id, cp, kmax, lmax;
    FIXNUM rv;
    char label[MAXLINE], dname[MAXLINE], rname[MAXLINE];
    PM_centering centering;
    PM_set *domain, *range;
    PG_graph *g;

    rv = -1;

    SC_FORTRAN_STR_C(label, flabel, *pnl);
    SC_FORTRAN_STR_C(dname, fdname, *pnd);
    SC_FORTRAN_STR_C(rname, frname, *pnr);

    id        = *pid;
    cp        = *pcp;
    kmax      = *pk;
    lmax      = *pl;
    centering = (PM_centering) *pcen;

/* build the domain set */
    domain = PM_make_set(dname, SC_DOUBLE_S, cp, 2, kmax, lmax, 2, x, y);

/* build the range set */
    range = PM_make_set(rname, SC_DOUBLE_S, cp, 2, kmax, lmax, 2, u, v);

    g = PG_make_graph_from_sets(label, domain, range, centering,
                                SC_PCONS_P_S, NULL, id, NULL);

    if (g != NULL)
       rv = SC_ADD_POINTER(g);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGMGFS - make a graph from sets */

FIXNUM FF_ID(pgmgfs, PGMGFS)(FIXNUM *pnl, char *fname,
                                FIXNUM *idom, FIXNUM *iran, FIXNUM *pcen,
                                FIXNUM *pid, FIXNUM *inxt)
   {int id;
    FIXNUM rv;
    char name[MAXLINE];
    PM_centering centering;
    PM_set *domain, *range;
    PM_mapping *f;
    PG_graph *g, *next;

    rv = -1;

    domain = SC_GET_POINTER(PM_set, *idom);
    range  = SC_GET_POINTER(PM_set, *iran);
    next   = SC_GET_POINTER(PG_graph, *inxt);

    SC_FORTRAN_STR_C(name, fname, *pnl);

    id        = *pid;
    centering = (PM_centering) *pcen;

/* build the mapping */
    if (domain->topology == NULL)
       f = PM_make_mapping(name, PM_LR_S, domain, range, centering, NULL);
    else
       f = PM_make_mapping(name, PM_AC_S, domain, range, centering, NULL);

    g = PG_make_graph_from_mapping(f, SC_PCONS_P_S, NULL, id, NULL);
    if (g != NULL)
       {g->next = next;
	rv = SC_ADD_POINTER(g);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGQKBD - get some information about the keyboard */

FIXNUM FF_ID(pgqkbd, PGQKBD)(FIXNUM *devid, FIXNUM *px, FIXNUM *py,
                                FIXNUM *pc, FIXNUM *pmod)
   {FIXNUM rv;

#ifdef HAVE_WINDOW_DEVICE
    PG_device *dev;
    int iev[PG_SPACEDM], mod;
    char bf[2];

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_key_event_info(dev, &_PG_gcont.current_event, iev, bf, 2, &mod);

    *px   = iev[0];
    *py   = iev[1];
    *pc   = bf[0];
    *pmod = mod;
#endif

    rv = TRUE;

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGQPTR - get some information about the pointer (mouse) */

FIXNUM FF_ID(pgqptr, PGQPTR)(FIXNUM *devid, FIXNUM *px, FIXNUM *py,
                                FIXNUM *pbtn, FIXNUM *pmod)
   {int btn, mod;
    int ir[PG_SPACEDM];
    FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);
    PG_query_pointer(dev, ir, &btn, &mod);

    *px   = ir[0];
    *py   = ir[1];
    *pbtn = btn;
    *pmod = mod;

    rv = TRUE;
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGPLOT - render a graph */

FIXNUM FF_ID(pgplot, PGPLOT)(FIXNUM *devid, FIXNUM *grid)
   {FIXNUM rv;
     PG_device *dev;
    PG_graph *g;

    dev = SC_GET_POINTER(PG_device, *devid);
    g   = SC_GET_POINTER(PG_graph, *grid);

    PG_draw_graph(dev, g);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGRLGR - release a graph */

FIXNUM FF_ID(pgrlgr, PGRLGR)(FIXNUM *grid, FIXNUM *prd, FIXNUM *prr)
   {int rld, rlr;
    FIXNUM rv;
    PG_graph *g;

    g   = SC_GET_POINTER(PG_graph, *grid);
    rld = *prd;
    rlr = *prr;

    PG_rl_graph(g, rld, rlr);

    *grid = 0;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSBKC - set the white background flag */

FIXNUM FF_ID(pgsbkc, PGSBKC)(FIXNUM *devid, FIXNUM *clr)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    dev->background_color_white = *clr;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSDLM - set a graph's domain limits */

FIXNUM FF_ID(pgsdlm, PGSDLM)(FIXNUM *grid, FIXNUM *pn, double *v)
   {FIXNUM rv;
    long n;
    double *pt;
    PG_graph *g;

    g = SC_GET_POINTER(PG_graph, *grid);
    n = *pn;

    pt = CMAKE_N(double, n);
    memcpy(pt, v, n*sizeof(double));

    PM_set_limits(g->f->domain, pt);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSEDF - set the default event handler */

FIXNUM FF_ID(pgsedf, PGSEDF)(FIXNUM *devid, PFEventHand fnc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    PG_set_default_event_handler(dev, fnc);
    dev->default_event_handler.lang = _FORTRAN_LANG;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSEEX - set the expose event handler */

FIXNUM FF_ID(pgseex, PGSEEX)(FIXNUM *devid, PFEventHand fnc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    PG_set_expose_event_handler(dev, fnc);
    dev->expose_event_handler.lang = _FORTRAN_LANG;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSEKD - set the key down event handler */

FIXNUM FF_ID(pgsekd, PGSEKD)(FIXNUM *devid, PFEventHand fnc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    PG_set_key_down_event_handler(dev, fnc);
    dev->key_down_event_handler.lang = _FORTRAN_LANG;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSEKU - set the key up event handler */

FIXNUM FF_ID(pgseku, PGSEKU)(FIXNUM *devid, PFEventHand fnc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    PG_set_key_up_event_handler(dev, fnc);
    dev->key_up_event_handler.lang = _FORTRAN_LANG;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSEMD - set the mouse down event handler */

FIXNUM FF_ID(pgsemd, PGSEMD)(FIXNUM *devid, PFEventHand fnc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    PG_set_mouse_down_event_handler(dev, fnc);
    dev->mouse_down_event_handler.lang = _FORTRAN_LANG;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSEMO - set the motion event handler */

FIXNUM FF_ID(pgsemo, PGSEMO)(FIXNUM *devid, PFEventHand fnc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    PG_set_motion_event_handler(dev, fnc);
    dev->motion_event_handler.lang = _FORTRAN_LANG;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSEMU - set the mouse up event handler */

FIXNUM FF_ID(pgsemu, PGSEMU)(FIXNUM *devid, PFEventHand fnc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    PG_set_mouse_up_event_handler(dev, fnc);
    dev->mouse_up_event_handler.lang = _FORTRAN_LANG;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSEUP - set the update event handler */

FIXNUM FF_ID(pgseup, PGSEUP)(FIXNUM *devid, PFEventHand fnc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *devid);

    PG_set_update_event_handler(dev, fnc);
    dev->update_event_handler.lang = _FORTRAN_LANG;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSINF - set the graph attribute list */

FIXNUM FF_ID(pgsinf, PGSINF)(FIXNUM *gid, FIXNUM *pal)
   {FIXNUM rv;
    PG_graph *g;
    pcons *alist;

    g = SC_GET_POINTER(PG_graph, *gid);

    alist = SC_GET_POINTER(pcons, *pal);
    PG_set_render_info(g, alist);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSGID - set the graph identifier */

FIXNUM FF_ID(pgsgid, PGSGID)(FIXNUM *gid, FIXNUM *pc)
   {FIXNUM rv;
    PG_graph *g;

    g = SC_GET_POINTER(PG_graph, *gid);
    PG_set_identifier(g, *pc);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSUPM - set the use-pixmap flag (X device) */

FIXNUM FF_ID(pgsupm, PGSUPM)(FIXNUM *flg)
   {FIXNUM rv;

    PG_set_use_pixmap(*flg);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSRAT - set a graph's rendering attributes */

FIXNUM FF_ID(pgsrat, PGSRAT)(FIXNUM *grid,
                                FIXNUM *pn, char *name,
                                FIXNUM *pt, char *type,
                                char *val)
   {int id;
    FIXNUM rv;
    long n;
    char lname[MAXLINE], ltype[MAXLINE], *pv;
    pcons *info;
    PG_graph *g;

    g = SC_GET_POINTER(PG_graph, *grid);

    SC_FORTRAN_STR_C(lname, name, *pn);
    SC_FORTRAN_STR_C(ltype, type, *pt);

    id = SC_type_id(ltype, FALSE);

    if (id == SC_INT_I)
       {strcpy(ltype, SC_INT_P_S);
        n = sizeof(int);}
    else if (strcmp(ltype, "real") == 0)
       {strcpy(ltype, SC_DOUBLE_P_S);
        n = sizeof(double);}
    else if (strcmp(ltype, "string") == 0)
       {strcpy(ltype, "char **");
        n = sizeof(char *);}
    else
       return((FIXNUM) FALSE);

    pv = CMAKE_N(char, n);
    memcpy(pv, val, n);

    info = (pcons *) g->info;

    info = SC_change_alist(info, lname, ltype, pv);

    g->info = (void *) info;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSRLM - set a graph's range limits */

FIXNUM FF_ID(pgsrlm, PGSRLM)(FIXNUM *grid, FIXNUM *pn, double *v)
   {PG_graph *g;
    long n;
    double *pt;

    g = SC_GET_POINTER(PG_graph, *grid);
    n = *pn;

    pt = CMAKE_N(double, n);
    memcpy(pt, v, n*sizeof(double));

    PM_set_limits(g->f->range, pt);

    return((FIXNUM) TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSVLM - set a graph's viewport limits */

FIXNUM FF_ID(pgsvlm, PGSVLM)(FIXNUM *grid, double *v)
   {FIXNUM rv;
    double *pv;
    PG_graph *g;

    g = SC_GET_POINTER(PG_graph, *grid);

    pv = CMAKE_N(double, 4);
    memcpy(pv, v, 4*sizeof(double));

    PG_set_attrs_graph(g,
		       "VIEW-PORT", SC_DOUBLE_I, TRUE, pv,
		       NULL);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
