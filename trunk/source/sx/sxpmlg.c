/*
 * SXPMLG.C - PML extensions in SX
 *          - to be replace by generated versions
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

/*--------------------------------------------------------------------------*/

/*                             PM_SET BINDING                               */

/*--------------------------------------------------------------------------*/

/* _SX_WR_GSET - print a g_set */

static void _SX_wr_gset(SS_psides *si, object *obj, object *strm)
   {

    PRINT(SS_OUTSTREAM(strm), "<SET|%s>",
                              SET_NAME(obj));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RL_GSET - gc a set */

static void _SX_rl_gset(SS_psides *si, object *obj)
   {

/* you don't know whether a mapping is pointing to this
    PM_set *set;
    set = SS_GET(PM_set, obj);

    PM_rel_set(set, FALSE);
*/

    SS_rl_object(si, obj);;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MK_SET - encapsulate a PM_set as an object */

object *SX_mk_set(SS_psides *si, PM_set *set)
   {object *op;

    if (set == NULL)
       op = SS_null;
    else
       op = SS_mk_object(si, set, G_SET, SELF_EV, set->name,
			 _SX_wr_gset, _SX_rl_gset);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_ARG_G_SET - extract and return a PM_set from O */

void *_SX_arg_g_set(SS_psides *si, object *o)
   {void *rv;

    rv = NULL;

    if (SX_SETP(o))
       rv = (void *) SS_GET(PM_set, o);
    else
       SS_error(si, "OBJECT NOT PM_SET - _SX_ARGS", o);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SETP - function version of SX_SETP macro */

object *_SXI_setp(SS_psides *si, object *obj)
   {object *o;

    o = SX_SETP(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/

/*                           PM_MAPPING BINDING                             */

/*--------------------------------------------------------------------------*/

/* _SX_WR_GMAPPING - print a g_mapping */

static void _SX_wr_gmapping(SS_psides *si, object *obj, object *strm)
   {

    PRINT(SS_OUTSTREAM(strm), "<MAPPING|%s>", MAPPING_NAME(obj));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RL_GMAPPING - gc a mapping */

static void _SX_rl_gmapping(SS_psides *si, object *obj)
   {PM_mapping *f;

    f = SS_GET(PM_mapping, obj);
    PM_rel_mapping(f, TRUE, TRUE);

    SS_rl_object(si, obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MK_MAPPING - encapsulate a PM_mapping as an object */

object *SX_mk_mapping(SS_psides *si, PM_mapping *f)
   {object *op;

    op = SS_mk_object(si, f, G_MAPPING, SELF_EV, f->name,
		      _SX_wr_gmapping, _SX_rl_gmapping);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_ARG_G_MAPPING - extract and return a PM_mapping from O */

void *_SX_arg_g_mapping(SS_psides *si, object *o)
   {void *rv;

    rv = NULL;

    if (SX_GRAPHP(o))
       rv = (void *) (SS_GET(PG_graph, o)->f);
    else if (SX_MAPPINGP(o))
       rv = (void *) SS_GET(PM_mapping, o);
    else
       SS_error(si, "OBJECT NOT PM_MAPPING - _SX_ARGS", o);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_MAPPINGP - function version of SX_MAPPINGP macro */

object *_SXI_mappingp(SS_psides *si, object *obj)
   {object *o;

    o = SX_MAPPINGP(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/

/*                             C_ARRAY BINDING                              */

/*--------------------------------------------------------------------------*/

/* _SX_WR_GNUM_ARRAY - print a g_num_array */

static void _SX_wr_gnum_array(SS_psides *si, object *obj, object *strm)
   {

    PRINT(SS_OUTSTREAM(strm), "<ARRAY|%s>", NUMERIC_ARRAY_TYPE(obj));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RL_GNUM_ARRAY - release g_num_array */

static void _SX_rl_gnum_array(SS_psides *si, object *obj)
   {C_array *arr;

    arr = SS_GET(C_array, obj);

/*  GOTCHA - it's currently possible that some PM_set may still be pointing
 *  at type and/or array even though the reference count doesn't reflect it
 *  CFREE(arr->type);
 *  CFREE(arr->data);
 */
    CFREE(arr);
    SS_rl_object(si, obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MK_C_ARRAY - encapsulate a C_array as an object */

object *SX_mk_C_array(SS_psides *si, C_array *arr)
   {object *op;

    op = SS_mk_object(si, arr, G_NUM_ARRAY, SELF_EV, arr->type,
		      _SX_wr_gnum_array, _SX_rl_gnum_array);

    SC_mark(arr, 1);
    SC_mark(arr->type, 1);
    SC_mark(arr->data, 1);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_ARG_NUM_ARRAY - extract and return a c_array from O */

void *_SX_arg_num_array(SS_psides *si, object *o)
   {void *rv;

    rv = NULL;

    if (SX_NUMERIC_ARRAYP(o))
       rv = (void *) SS_GET(C_array, o);
    else
       SS_error(si, "OBJECT NOT C_ARRAY - _SX_ARGS", o);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_NUMERIC_ARRAYP - function version of SX_NUMERIC_ARRAYP macro */

object *_SXI_numeric_arrayp(SS_psides *si, object *obj)
   {object *o;

    o = SX_NUMERIC_ARRAYP(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/

/*                            PM_POLYGON BINDING                            */

/*--------------------------------------------------------------------------*/

/* _SX_WR_GPOLYGON - print a gpolygon */

static void _SX_wr_gpolygon(SS_psides *si, object *obj, object *strm)
   {PM_polygon *py;

    py = SS_GET(PM_polygon, obj);

    PRINT(SS_OUTSTREAM(strm), "<POLYGON|%ld>", py->nn);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RL_GPOLYGON - release g_polygon */

static void _SX_rl_gpolygon(SS_psides *si, object *obj)
   {PM_polygon *py;

    py = SS_GET(PM_polygon, obj);

    PM_free_polygon(py);
    SS_rl_object(si, obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MK_POLYGON - encapsulate a PM_polygon as an object */

object *SX_mk_polygon(SS_psides *si, PM_polygon *py)
   {object *op;

    op = SS_mk_object(si, py, G_POLYGON, SELF_EV, "pm-polygon",
		      _SX_wr_gpolygon, _SX_rl_gpolygon);

    SC_mark(py, 1);
    SC_mark(py->x, 1);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_ARG_PM_POLYGON - extract and return a PM_polygon from O */

void *_SX_arg_pm_polygon(SS_psides *si, object *o)
   {void *rv;

    rv = NULL;

    if (SX_POLYGONP(o))
       rv = (void *) SS_GET(PM_polygon, o);
    else
       SS_error(si, "OBJECT NOT PM_POLYGON - _SX_ARGS", o);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_POLYGONP - function version of SX_POLYGONP macro */

object *_SXI_polygonp(SS_psides *si, object *obj)
   {object *o;

    o = SX_POLYGONP(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
