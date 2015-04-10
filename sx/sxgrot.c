/*
 * SXGROT.C - PGS Grotrian Plot Routines in SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_GROTRIAN_GRAPHP - return TRUE iff the object is a grotrian graph */

int _SX_grotrian_graphp(PG_graph *g)
   {int rv;

    rv = _SX_grotrian_mappingp(g->f);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_GROTRIAN_MAPPINGP - return TRUE iff the object is a grotrian mapping */

int _SX_grotrian_mappingp(PM_mapping *f)
   {int rv;

    rv = strcmp(f->category, "Grotrian-Diagram") == 0 ? TRUE : FALSE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_ANY_GROTRIANP - return TRUE if any of the objects in the arg list
 *                   - are grotrian graphs
 */

int _SX_any_grotrianp(SS_psides *si, object *argl)
   {object *obj;
    PM_mapping *f;

    while (SS_consp(argl))
       {obj  = SS_car(si, argl);
        argl = SS_cdr(si, argl);
        if (SX_MAPPINGP(obj))
           {f = SS_GET(PM_mapping, obj);
            if (_SX_grotrian_mappingp(f))
               return(TRUE);};};

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_DRAW_GROTRIAN - Scheme level Grotrian plot control routine */

object *SX_draw_grotrian(SS_psides *si, object *argl)
   {PG_device *dev;
    PG_graph *g;

    dev = NULL;
    g   = NULL;
    SS_args(si, argl,
            G_PG_DEVICE_I, &dev,
            G_PG_GRAPH_I, &g,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - SX_DRAW_GROTRIAN", argl);

    if (!_SX_grotrian_graphp(g))
       SS_error(si, "GRAPH IS NOT GROTRIAN - SX_DRAW_GROTRIAN", argl);

    PG_grotrian_plot(dev, g);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GROTRIAN_GRAPHP - return #t iff the object is a grotrian graph */

object *_SXI_grotrian_graphp(SS_psides *si, object *obj)
   {PG_graph *g;
    object *o;

    if (SX_GRAPHP(obj))
       {g = SS_GET(PG_graph, obj);
        o = _SX_grotrian_graphp(g) ? SS_t : SS_f;}
    else
       o = SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GROTRIAN_MAPPINGP - return #t iff the object is a grotrian mapping */

object *_SXI_grotrian_mappingp(SS_psides *si, object *obj)
   {PM_mapping *f;
    object *o;

    if (SX_MAPPINGP(obj))
       {f = SS_GET(PM_mapping, obj);
	o = _SX_grotrian_mappingp(f) ? SS_t : SS_f;}
    else
       o = SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
