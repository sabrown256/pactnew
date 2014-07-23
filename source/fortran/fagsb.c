/*
 * FAGSB.C - FORTRAN interface routines for PGS
 *         - NOTE: let's keep these in alphabetical order
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "fpact.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGGID - get the graph identifier */

FIXNUM FF_ID(pgggid, PGGGID)(FIXNUM *sgid)
   {FIXNUM rv;
    PG_graph *g;

    g = SC_GET_POINTER(PG_graph, *sgid);
    rv = PG_fget_identifier(g);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSGID - set the graph identifier */

FIXNUM FF_ID(pgsgid, PGSGID)(FIXNUM *sgid, FIXNUM *sc)
   {FIXNUM rv;
    PG_graph *g;

    g  = SC_GET_POINTER(PG_graph, *sgid);
    rv = PG_fset_identifier(g, *sc);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGINF - get the graph attribute list */

FIXNUM FF_ID(pgginf, PGGINF)(FIXNUM *sgid, FIXNUM *said)
   {FIXNUM rv;
    PG_graph *g;
    pcons *alist;

    g = SC_GET_POINTER(PG_graph, *sgid);

    alist = PG_fget_render_info(g);
    *said = SC_ADD_POINTER(alist);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSINF - set the graph attribute list */

FIXNUM FF_ID(pgsinf, PGSINF)(FIXNUM *sgid, FIXNUM *said)
   {FIXNUM rv;
    PG_graph *g;
    pcons *alist;

    g = SC_GET_POINTER(PG_graph, *sgid);

    alist = SC_GET_POINTER(pcons, *said);
    PG_fset_render_info(g, alist);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGGUPM - get the use-pixmap flag (X device) */

FIXNUM FF_ID(pggupm, PGGUPM)(void)
   {FIXNUM rv;

    rv = PG_fget_use_pixmap();

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSUPM - set the use-pixmap flag (X device) */

FIXNUM FF_ID(pgsupm, PGSUPM)(FIXNUM *sflg)
   {FIXNUM rv;

    rv = PG_fset_use_pixmap(*sflg);

    return(rv);}

/*--------------------------------------------------------------------------*/







/*--------------------------------------------------------------------------*/

/* PGGBKC - get the white background flag */

FIXNUM FF_ID(pggbkc, PGGBKC)(FIXNUM *sdid, FIXNUM *sclr)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    *sclr = dev->background_color_white;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGMG11 - make a 1D-1D graph */

FIXNUM FF_ID(pgmg11, PGMG11)(FIXNUM *sid,
			     FIXNUM *sncl, char *flabel,
			     FIXNUM *scp, FIXNUM *sn, double *ax, double *ay,
			     FIXNUM *sncx, char *fxname,
			     FIXNUM *sncy, char *fyname)
   {int id, cp, n;
    FIXNUM rv;
    char label[MAXLINE], xname[MAXLINE], yname[MAXLINE];
    PG_graph *g;

    rv = -1;

    SC_FORTRAN_STR_C(label, flabel, *sncl);
    SC_FORTRAN_STR_C(xname, fxname, *sncx);
    SC_FORTRAN_STR_C(yname, fyname, *sncy);

    id = *sid;
    cp = *scp;
    n  = *sn;

    g = PG_make_graph_1d(id, label, cp, n, ax, ay, xname, yname);

    if (g != NULL)
       rv = SC_ADD_POINTER(g);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGMG21 - make a 2D-1D graph */

FIXNUM FF_ID(pgmg21, PGMG21)(FIXNUM *sid,
			     FIXNUM *sncl, char *flabel,
			     FIXNUM *scp, FIXNUM *sk, FIXNUM *sl,
			     FIXNUM *scen,
			     double *ax, double *ay, double *ar,
			     FIXNUM *sncd, char *fdname,
			     FIXNUM *sncr, char *frname)
   {int id, cp, kmax, lmax;
    FIXNUM rv;
    char label[MAXLINE], dname[MAXLINE], rname[MAXLINE];
    PM_centering centering;
    PG_graph *g;

    rv = -1;

    SC_FORTRAN_STR_C(label, flabel, *sncl);
    SC_FORTRAN_STR_C(dname, fdname, *sncd);
    SC_FORTRAN_STR_C(rname, frname, *sncr);

    id        = *sid;
    cp        = *scp;
    kmax      = *sk;
    lmax      = *sl;
    centering = (PM_centering) *scen;

    g = PG_make_graph_r2_r1(id, label, cp, kmax, lmax, centering,
			    ax, ay, ar, dname, rname);

    if (g != NULL)
       rv = SC_ADD_POINTER(g);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGMG22 - make a 2D-2D graph */

FIXNUM FF_ID(pgmg22, PGMG22)(FIXNUM *sid,
			     FIXNUM *sncl, char *flabel,
			     FIXNUM *scp, FIXNUM *sk, FIXNUM *sl,
			     FIXNUM *scen,
			     double *ax, double *ay, double *au, double *av,
			     FIXNUM *sncd, char *fdname,
			     FIXNUM *sncr, char *frname)
   {int id, cp, kmax, lmax;
    FIXNUM rv;
    char label[MAXLINE], dname[MAXLINE], rname[MAXLINE];
    PM_centering centering;
    PM_set *domain, *range;
    PG_graph *g;

    rv = -1;

    SC_FORTRAN_STR_C(label, flabel, *sncl);
    SC_FORTRAN_STR_C(dname, fdname, *sncd);
    SC_FORTRAN_STR_C(rname, frname, *sncr);

    id        = *sid;
    cp        = *scp;
    kmax      = *sk;
    lmax      = *sl;
    centering = (PM_centering) *scen;

/* build the domain set */
    domain = PM_make_set(dname, SC_DOUBLE_S, cp, 2, kmax, lmax, 2, ax, ay);

/* build the range set */
    range = PM_make_set(rname, SC_DOUBLE_S, cp, 2, kmax, lmax, 2, au, av);

    g = PG_make_graph_from_sets(label, domain, range, centering,
                                SC_PCONS_P_S, NULL, id, NULL);

    if (g != NULL)
       rv = SC_ADD_POINTER(g);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGMGFS - make a graph from sets */

FIXNUM FF_ID(pgmgfs, PGMGFS)(FIXNUM *sncn, char *fname,
			     FIXNUM *sidm, FIXNUM *sirn, FIXNUM *scen,
			     FIXNUM *sid, FIXNUM *sinxt)
   {int id;
    FIXNUM rv;
    char name[MAXLINE];
    PM_centering centering;
    PM_set *domain, *range;
    PM_mapping *f;
    PG_graph *g, *next;

    rv = -1;

    domain = SC_GET_POINTER(PM_set, *sidm);
    range  = SC_GET_POINTER(PM_set, *sirn);
    next   = SC_GET_POINTER(PG_graph, *sinxt);

    SC_FORTRAN_STR_C(name, fname, *sncn);

    id        = *sid;
    centering = (PM_centering) *scen;

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

FIXNUM FF_ID(pgqkbd, PGQKBD)(FIXNUM *sdid, FIXNUM *sx, FIXNUM *sy,
			     FIXNUM *sc, FIXNUM *smod)
   {FIXNUM rv;

#ifdef HAVE_WINDOW_DEVICE
    PG_device *dev;
    int iev[PG_SPACEDM], mod;
    char bf[2];

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_key_event_info(dev, &_PG_gcont.current_event, iev, bf, 2, &mod);

    *sx   = iev[0];
    *sy   = iev[1];
    *sc   = bf[0];
    *smod = mod;
#endif

    rv = TRUE;

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGQPTR - get some information about the pointer (mouse) */

FIXNUM FF_ID(pgqptr, PGQPTR)(FIXNUM *sdid, FIXNUM *sx, FIXNUM *sy,
			     FIXNUM *sbtn, FIXNUM *smod)
   {int btn, mod;
    int ir[PG_SPACEDM];
    FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);
    PG_query_pointer(dev, ir, &btn, &mod);

    *sx   = ir[0];
    *sy   = ir[1];
    *sbtn = btn;
    *smod = mod;

    rv = TRUE;
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGPLOT - render a graph */

FIXNUM FF_ID(pgplot, PGPLOT)(FIXNUM *sdid, FIXNUM *sgid)
   {FIXNUM rv;
     PG_device *dev;
    PG_graph *g;

    dev = SC_GET_POINTER(PG_device, *sdid);
    g   = SC_GET_POINTER(PG_graph, *sgid);

    PG_draw_graph(dev, g);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGRLGR - release a graph */

FIXNUM FF_ID(pgrlgr, PGRLGR)(FIXNUM *sgid, FIXNUM *srd, FIXNUM *srr)
   {int rld, rlr;
    FIXNUM rv;
    PG_graph *g;

    g   = SC_GET_POINTER(PG_graph, *sgid);
    rld = *srd;
    rlr = *srr;

    PG_rl_graph(g, rld, rlr);

    *sgid = 0;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSBKC - set the white background flag */

FIXNUM FF_ID(pgsbkc, PGSBKC)(FIXNUM *sdid, FIXNUM *sclr)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    dev->background_color_white = *sclr;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSDLM - set a graph's domain limits */

FIXNUM FF_ID(pgsdlm, PGSDLM)(FIXNUM *sgid, FIXNUM *sn, double *av)
   {FIXNUM rv;
    long n;
    double *pt;
    PG_graph *g;

    g = SC_GET_POINTER(PG_graph, *sgid);
    n = *sn;

    pt = CMAKE_N(double, n);
    memcpy(pt, av, n*sizeof(double));

    PM_set_limits(g->f->domain, pt);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSEDF - set the default event handler */

FIXNUM FF_ID(pgsedf, PGSEDF)(FIXNUM *sdid, PFEventHand fnc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    PG_set_default_event_handler(dev, fnc);
    dev->default_event_handler.lang = _FORTRAN_LANG;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSEEX - set the expose event handler */

FIXNUM FF_ID(pgseex, PGSEEX)(FIXNUM *sdid, PFEventHand fnc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    PG_set_expose_event_handler(dev, fnc);
    dev->expose_event_handler.lang = _FORTRAN_LANG;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSEKD - set the key down event handler */

FIXNUM FF_ID(pgsekd, PGSEKD)(FIXNUM *sdid, PFEventHand fnc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    PG_set_key_down_event_handler(dev, fnc);
    dev->key_down_event_handler.lang = _FORTRAN_LANG;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSEKU - set the key up event handler */

FIXNUM FF_ID(pgseku, PGSEKU)(FIXNUM *sdid, PFEventHand fnc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    PG_set_key_up_event_handler(dev, fnc);
    dev->key_up_event_handler.lang = _FORTRAN_LANG;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSEMD - set the mouse down event handler */

FIXNUM FF_ID(pgsemd, PGSEMD)(FIXNUM *sdid, PFEventHand fnc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    PG_set_mouse_down_event_handler(dev, fnc);
    dev->mouse_down_event_handler.lang = _FORTRAN_LANG;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSEMO - set the motion event handler */

FIXNUM FF_ID(pgsemo, PGSEMO)(FIXNUM *sdid, PFEventHand fnc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    PG_set_motion_event_handler(dev, fnc);
    dev->motion_event_handler.lang = _FORTRAN_LANG;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSEMU - set the mouse up event handler */

FIXNUM FF_ID(pgsemu, PGSEMU)(FIXNUM *sdid, PFEventHand fnc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    PG_set_mouse_up_event_handler(dev, fnc);
    dev->mouse_up_event_handler.lang = _FORTRAN_LANG;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSEUP - set the update event handler */

FIXNUM FF_ID(pgseup, PGSEUP)(FIXNUM *sdid, PFEventHand fnc)
   {FIXNUM rv;
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    PG_set_update_event_handler(dev, fnc);
    dev->update_event_handler.lang = _FORTRAN_LANG;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSRAT - set a graph's rendering attributes */

FIXNUM FF_ID(pgsrat, PGSRAT)(FIXNUM *sgid,
			     FIXNUM *sncn, char *name,
			     FIXNUM *snct, char *type,
			     char *val)
   {int id;
    FIXNUM rv;
    long n;
    char lname[MAXLINE], ltype[MAXLINE], *pv;
    pcons *info;
    PG_graph *g;

    g = SC_GET_POINTER(PG_graph, *sgid);

    SC_FORTRAN_STR_C(lname, name, *sncn);
    SC_FORTRAN_STR_C(ltype, type, *snct);

    id = SC_type_id(ltype, FALSE);

    if (id == SC_INT_I)
       {SC_strncpy(ltype, MAXLINE, SC_INT_P_S, -1);
        n = sizeof(int);}
    else if (strcmp(ltype, "real") == 0)
       {SC_strncpy(ltype, MAXLINE, SC_DOUBLE_P_S, -1);
        n = sizeof(double);}
    else if (strcmp(ltype, "string") == 0)
       {SC_strncpy(ltype, MAXLINE, "char **", -1);
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

FIXNUM FF_ID(pgsrlm, PGSRLM)(FIXNUM *sgid, FIXNUM *sn, double *av)
   {PG_graph *g;
    long n;
    double *pt;

    g = SC_GET_POINTER(PG_graph, *sgid);
    n = *sn;

    pt = CMAKE_N(double, n);
    memcpy(pt, av, n*sizeof(double));

    PM_set_limits(g->f->range, pt);

    return((FIXNUM) TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSVLM - set a graph's viewport limits */

FIXNUM FF_ID(pgsvlm, PGSVLM)(FIXNUM *sgid, double *av)
   {FIXNUM rv;
    double *pv;
    PG_graph *g;

    g = SC_GET_POINTER(PG_graph, *sgid);

    pv = CMAKE_N(double, 4);
    memcpy(pv, av, 4*sizeof(double));

    PG_set_attrs_graph(g,
		       "VIEW-PORT", SC_DOUBLE_I, TRUE, pv,
		       NULL);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
