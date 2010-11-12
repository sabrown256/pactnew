/*
 * SXPGS.C - PGS extensions in SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"
#include "scope_raster.h"

#define LABEL_TEXT_SIZE 30

double
 SX_gwc[PG_BOXSZ],
 SX_gpad[PG_BOXSZ],
 SX_window_x[PG_SPACEDM],
 SX_window_P[PG_SPACEDM],
 SX_window_width,
 SX_window_width_P,
 SX_window_height,
 SX_window_height_P,
 SX_view_x[PG_BOXSZ],
 SX_view_width,
 SX_view_height,
 SX_view_aspect;

PG_device
 *SX_graphics_device;

static PG_rendering
 *rendering_mode[4][6] =
    {{&SX_render_def, &SX_render_def,   &SX_render_def,
      &SX_render_def, &SX_render_def,   &SX_render_def},
     {&SX_render_def, &SX_render_1d_1d, &SX_render_def,
      &SX_render_def, &SX_render_def,   &SX_render_def},
     {&SX_render_def, &SX_render_2d_1d, &SX_render_2d_2d,
      &SX_render_def, &SX_render_def,   &SX_render_def},
     {&SX_render_def, &SX_render_def,   &SX_render_def,
      &SX_render_def, &SX_render_def,   &SX_render_def}};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DEF_FILE_GRAPH - define PM_set and PM_mapping to 
 *                     - a PDB file thereby preparing it for mappings
 */

static object *_SXI_def_file_graph(object *obj)
   {PDBfile *file;
    g_file *po;

    po = NULL;
    SS_args(obj,
            G_FILE, &po,
            0);

    if (po == NULL)
       file = SX_vif;

    else
       {if (strcmp(po->type, PDBFILE_S) != 0)
	   SS_error("REQUIRE PDB FILE - _SXI_DEF_FILE_GRAPH", obj);

	file = FILE_FILE(PDBfile, po);};

    if (!PD_inquire_type(file, "PM_mapping"))
       if (!PD_def_mapping(file))
	  SS_error("CAN`T DEFINE MAPPINGS - _SXI_DEF_FILE_GRAPH", obj);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GRAPH_PDBDATA - given a graph object
 *                    - allocate and fill a PDB data object with the
 *                    - PM_mapping and return it
 */

static object *_SXI_graph_pdbdata(object *argl)
   {PG_graph *g;
    PM_mapping *f;
    char *name;
    PDBfile *file;
    g_file *po;
    object *rv;

    rv   = SS_null;
    g    = NULL;
    po   = NULL;
    name = NULL;
    SS_args(argl,
            G_GRAPH, &g,
            G_FILE, &po,
            SC_STRING_I, &name,
            0);

    if (strcmp(po->type, PDBFILE_S) != 0)
       SS_error("REQUIRE PDB FILE - _SXI_GRAPH_PDBDATA", argl);
    file = FILE_FILE(PDBfile, po);

    if (g == NULL)
       SS_error("BAD GRAPH - _SXI_GRAPH_PDBDATA", argl);

    else

/* disconnect any function pointers or undefined structs/members */
       {for (f = g->f; f != NULL; f = f->next)
	    {f->domain->opers = NULL;
	     f->range->opers  = NULL;};

	rv = SX_pdbdata_handler(file, name, "PM_mapping *", &(g->f) , TRUE);};

    SFREE(name);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_PDBDATA_GRAPH - worker for mapping side of _SXI_pdbdata_graph */

static object *_SX_pdbdata_graph(PDBfile *file, char *name, syment *ep)
   {int ndd, ndr, clr, ret;
    char dname[MAXLINE];
    char *info_type, *tail;
    void *info;
    SC_address data;
    PG_graph *g;
    PM_mapping *f;
    PM_set *domain, *range;
    object *o;

/* read the mapping */
    if (file == SX_vif)
       {data.diskaddr = PD_entry_address(ep);
        f = *(PM_mapping **) data.memaddr;}
    else
       {if (!PD_read(file, name, &f))
           SS_error(PD_err, SS_null);};

/* reconnect any function pointers or undefined structs/members */
    domain = f->domain;
    range  = f->range;
    if (domain == NULL)
       {if (PD_has_directories(file))
           {strcpy(dname, name);
            tail = strrchr(dname, '/');
            tail = (tail == NULL) ? dname : tail + 1;
            strcpy(tail, f->name);
            PD_process_set_name(tail);}
        else
           {strcpy(dname, f->name);
            PD_process_set_name(dname);};

        if (!PD_read(file, dname, &data.memaddr))
           SS_error(PD_err, SS_null);

        domain = f->domain = (PM_set *) data.memaddr;};

    ndd = (domain == NULL) ? 0 : domain->dimension_elem;
    ndr = (range == NULL) ? 0 : range->dimension_elem;
    clr = _SX.ig % 14 + 1;

    info_type = SC_PCONS_P_S;
    if ((ndd == 1) && (ndr == 1))
       info = (void *) PG_set_line_info(NULL, PLOT_CARTESIAN, CARTESIAN_2D,
					LINE_SOLID, FALSE, 0, clr, 0, 0.0);

    else if ((ndd == 2) && (ndr == 1))
       {int nlev;

	PG_get_attrs_glb(TRUE,
			 "contour-n-levels", &nlev,
			 NULL);
	info = (void *) PG_set_tds_info(NULL, PLOT_CONTOUR, CARTESIAN_2D,
					LINE_SOLID, clr, nlev,
					1.0, 0.0, 0.0, 0.0, 0.0, HUGE);}

    else if ((ndd == 2) && (ndr == 2))
       info = (void *) PG_set_tdv_info(NULL, PLOT_VECTOR, CARTESIAN_2D,
				       LINE_SOLID, clr, 0.0);
    else
       info = NULL;

    g = PG_make_graph_from_mapping(f, info_type, info, _SX.ig++, NULL);

    ret = TRUE;
    for (f = g->f; f != NULL; f = f->next)
        {domain = f->domain;
         range  = f->range;
         if (domain != NULL)
            {ret &= PM_set_opers(domain);
	     if (domain->info_type == NULL)
                domain->info_type = SC_PCONS_P_S;};

         if (range != NULL)
            {ret &= PM_set_opers(range);
	     if (range->info_type == NULL)
                range->info_type = SC_PCONS_P_S;};

/* GOTCHA: it is not necessary to die here if you only want to view
 *         the data
 *         doing algebra would be a problem later on however
         if (ret == FALSE)
            SS_error("NO FIELD FOR TYPE - _SXI_PDBDATA_GRAPH", SS_null);
 */
       };

    if (_SX.ig > 'Z')
       _SX.ig = 'A';

    o = SX_mk_graph(g);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_PDBCURVE_GRAPH - worker for curve side of _SXI_pdbdata_graph */

static object *_SX_pdbcurve_graph(PDBfile *file, char *name, syment *ep)
   {int n, clr;
    char label[MAXLINE];
    double *x, *y;
    double xmn, xmx, ymn, ymx;
    pcons *info;
    PG_graph *g;
    PM_mapping *f;
    PM_set *domain, *range;
    object *o;

/* read the curve */
    if (!PD_read_pdb_curve(file, name, &x, &y, &n, label,
                           &xmn, &xmx, &ymn, &ymx, X_AND_Y))
       SS_error(PD_err, SS_null);

    domain = PM_make_set("X values", SC_DOUBLE_S, FALSE, 1, n, 1, x);
    range  = PM_make_set("Y values", SC_DOUBLE_S, FALSE, 1, n, 1, y);

    f    = PM_make_mapping(label, PM_LR_S, domain, range, N_CENT, NULL);
    clr  = _SX.icg % 14 + 1;
    info = PG_set_line_info(NULL, PLOT_CARTESIAN, CARTESIAN_2D,
			    LINE_SOLID, FALSE, 0, clr, 0, 0.0);
    g    = PG_make_graph_from_mapping(f, SC_PCONS_P_S, info, _SX.icg++, NULL);

    if (_SX.icg > 1000)
       _SX.icg = 1;

    o = SX_mk_graph(g);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_GET_MENU_ITEM - return the Ith menu item from PO */

SX_menu_item *_SX_get_menu_item(g_file *po, int i)
   {int n;
    SX_menu_item *mi;

    mi = NULL;

    n = SC_array_get_n(po->menu_lst);
    if ((0 < i) && (i <= n))
       {if (po->menu_lst->array == NULL)
	   _SX_get_menu(po);

	mi = SC_array_get(po->menu_lst, i-1);};

    return(mi);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_PDBDATA_GRAPH - read a PG_graph out of a PDB file and
 *                    - return a PGS graph object
 *                    - FORM:
 *                    -    (pdbdata->pg_graph <file> <name>)
 */

static object *_SXI_pdbdata_graph(object *argl)
   {object *obj;
    int i;
    char *name;
    PDBfile *file;
    SX_menu_item *mi;
    g_file *po;
    syment *ep;

    po  = NULL;
    obj = SS_null;
    SS_args(argl,
            G_FILE, &po,
            SS_OBJECT_I, &obj,
            0);

    if (strcmp(po->type, PDBFILE_S) != 0)
       SS_error("REQUIRE PDB FILE - _SXI_PDBDATA_GRAPH", argl);

    file = FILE_FILE(PDBfile, po);

    if (SS_integerp(obj))
       {i    = SS_INTEGER_VALUE(obj);
	mi   = _SX_get_menu_item(po, i);
	name = (mi == NULL) ? NULL : mi->vname;}
    else
       {argl = SS_cdr(argl);
        name = SC_strsavef(SS_get_string(obj),
			   "char*:_SXI_PDBDATA_GRAPH:name");};

    if (name == NULL)
       return(SS_null);

/* check to see whether or not the variable is in the file */
    ep = PD_inquire_entry(file, name, TRUE, NULL);
    if (ep == NULL)
       return(SS_null);

    if (strcmp(PD_entry_type(ep), "PM_mapping *") == 0)
       return(_SX_pdbdata_graph(file, name, ep));

    else if (SC_strstr(name, "curve") != NULL)
       return(_SX_pdbcurve_graph(file, name, ep));

    else if (strcmp(PD_entry_type(ep), "PG_image *") == 0)
       /* GOTCHA: images are not currently handled */;

    else
       SS_error("VARIABLE NOT MAPPING - _SXI_PDBDATA_GRAPH",
                SS_mk_string(name));

    return(SS_null);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_WR_GGRAPH - print a g_graph */

static void _SX_wr_ggraph(object *obj, object *strm)
   {

    PRINT(SS_OUTSTREAM(strm), "<GRAPH|%c>", GRAPH_IDENTIFIER(obj));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RL_GGRAPH - gc a graph */

static void _SX_rl_ggraph(object *obj)
   {PG_graph *g;

    g = SS_GET(PG_graph, obj);
    if (g->info != NULL)
       SFREE(g->info);
    SFREE(g);

/* GOTCHA: don't know if it is safe to GC the mapping or its sets */

    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MK_GRAPH - encapsulate a PG_graph as an object */

object *SX_mk_graph(PG_graph *g)
   {object *op;

    op = SS_mk_object(g, G_GRAPH, SELF_EV, g->f->name,
		      _SX_wr_ggraph, _SX_rl_ggraph);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_WR_GDEV_ATTR - print a g_dev_attributes */

static void _SX_wr_gdev_attr(object *obj, object *strm)
   {

    PRINT(SS_OUTSTREAM(strm), "<DEV_ATTRIBUTES>");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RL_GDEV_ATTR - gc a dev_attributes */

static void _SX_rl_gdev_attr(object *obj)
   {PG_dev_attributes *da;

    da = SS_GET(PG_dev_attributes, obj);
    SFREE(da);

    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MK_DEV_ATTRIBUTES - encapsulate a PG_dev_attributes as an object */

object *SX_mk_dev_attributes(PG_dev_attributes *da)
   {object *op;

    op = SS_mk_object(da, G_DEV_ATTRIBUTES, SELF_EV, NULL,
		      _SX_wr_gdev_attr, _SX_rl_gdev_attr);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_MENU_ITEM_TYPE - return the type of a menu item */

static object *_SXI_menu_item_type(object *argl)
   {int indx;
    char bf[MAXLINE], *s;
    g_file *po;
    SX_menu_item *mi;
    object *rv;

    po   = NULL;
    indx = -1;
    SS_args(argl,
            G_FILE, &po,
            SC_INT_I, &indx,
            0);

    _SX_get_menu(po);

    mi = _SX_get_menu_item(po, indx);
    if (mi == NULL)
       rv = SS_null;

    else
       {strcpy(bf, mi->type + 3);
	rv = SS_mk_string(SC_strtok(bf, " *", s));};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GET_TEXT_IMAGE_NAME - given a file and reference to an image by
 *                          - name or menu number,
 *                          - return the image name as an object
 */

static object *_SXI_get_text_image_name(object *argl)
   {long i;
    char *name;
    SX_menu_item *mi;
    g_file *po;
    object *o;

    argl = SX_get_file(argl, &po);
    argl = SS_car(argl);
    if (SS_integerp(argl))
       {i    = SS_INTEGER_VALUE(argl);
	mi   = _SX_get_menu_item(po, i);
	name = (mi == NULL) ? NULL : mi->vname;}
    else
       name = SC_strsavef(SS_get_string(argl),
			  "char*:_SXI_GET_TEXT_IMAGE_NAME:name");

    if (name == NULL)
       o = SS_null;
    else
       o = SS_mk_string(name);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GET_TEXT_MAPPING_NAME - given a file and reference to a mapping by
 *                            - name or menu number,
 *                            - return the mapping name as an object
 */

static object *_SXI_get_text_mapping_name(object *argl)
   {long i;
    char *name;
    SX_menu_item *mi;
    g_file *po;
    object *o;

    argl = SX_get_file(argl, &po);
    argl = SS_car(argl);
    if (SS_integerp(argl))
       {i    = SS_INTEGER_VALUE(argl);
	mi   = _SX_get_menu_item(po, i);
	name = (mi == NULL) ? NULL : mi->vname;}
    else
       name = SC_strsavef(SS_get_string(argl),
			  "char*:_SXI_GET_TEXT_MAPPING_NAME:name");

    if (name == NULL)
       o = SS_null;
    else
       o = SS_mk_string(name);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_PDBDATA_IMAGE - read a PG_image out of a PDB file and
 *                    - return a PGS image object
 *                    - FORM:
 *                    -    (pdbdata->pgs_image <file> <name>)
 */

static object *_SXI_pdbdata_image(object *argl)
   {object *obj;
    int i;
    char *name;
    SX_menu_item *mi;
    PDBfile *file;
    g_file *po;
    syment *ep;
    SC_address data;
    PG_image *im;
    object *o;

    if (!SS_consp(argl))
       SS_error("BAD ARGUMENT LIST - _SXI_PDBDATA_IMAGE", argl);

/* if the first object is a pdbfile, use it, otherwise, use default file */
    argl = SX_get_file(argl, &po);
    file = (PDBfile *) po->file;

    obj = SS_car(argl);
    if (SS_integerp(obj))
       {i    = SS_INTEGER_VALUE(obj);
	mi   = _SX_get_menu_item(po, i);
	name = (mi == NULL) ? NULL : mi->vname;}
    else
       {argl = SS_cdr(argl);
        name = SC_strsavef(SS_get_string(obj),
			   "char*:_SXI_PDBDATA_IMAGE:name");};

    if (name == NULL)
       o = SS_null;

    else

/* check to see whether or not the variable is in the file */
       {ep = PD_inquire_entry(file, name, TRUE, NULL);
	if (ep == NULL)
	   SS_error("VARIABLE NOT FOUND - _SXI_PDBDATA_IMAGE", obj);

	else if (strcmp(PD_entry_type(ep), "PG_image *") != 0)
	   SS_error("VARIABLE NOT IMAGE - _SXI_PDBDATA_IMAGE", obj);

/* read the mapping */
	if (file == SX_vif)
	   {data.diskaddr = PD_entry_address(ep);
	    im = *(PG_image **) data.memaddr;}
	else
	   {if (!PD_read(file, name, &im))
	       SS_error(PD_err, obj);};

	o = SX_mk_image(im);};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_WR_GIMAGE - print a g_image */

static void _SX_wr_gimage(object *obj, object *strm)
   {

    PRINT(SS_OUTSTREAM(strm), "<IMAGE|%s>",
                              IMAGE_NAME(obj));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RL_GIMAGE - gc a image */

static void _SX_rl_gimage(object *obj)
   {

/* GOTCHA - don't know right thing to do here. See _SX_rl_ggraph.
   If image released, ci 1; ci 1 crashes pdbview. */
/*
    PG_image *im;

    im = SS_GET(PG_image, obj);
    SFREE(im->label);
    SFREE(im->element_type);
    SFREE(im->buffer);
    SFREE(im);
*/

    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MK_IMAGE - encapsulate a PG_image as an object */

object *SX_mk_image(PG_image *im)
   {object *op;

    op = SS_mk_object(im, G_IMAGE, SELF_EV, im->label,
		      _SX_wr_gimage, _SX_rl_gimage);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GRAPH_PDBCURVE - given a PM_mapping or PG_graph object for a curve,
 *                     - write the curve to the specified file
 */

static object *_SXI_graph_pdbcurve(object *argl)
   {PM_mapping *f;
    g_file *po;
    PDBfile *file;
    char *name;
    int i;

    f    = NULL;
    po   = NULL;
    file = NULL;
    SS_args(argl,
            G_MAPPING, &f,
            G_FILE, &po,
            0);

    if ((po == NULL) || (po == SX_gvif))
       file = SX_vif;
    else if (strcmp(po->type, PDBFILE_S) == 0)
       file = FILE_FILE(PDBfile, po);
    else
       SS_error("BAD FILE - _SXI_GRAPH_PDBCURVE", argl);

    if (f == NULL)
       SS_error("BAD ARGUMENT - _SXI_GRAPH_PDBCURVE", argl);

    else
       {_SX_get_menu(po);
	for (i = 0; TRUE; i++)
	    {name = SC_dsnprintf(MAXLINE, "curve%04d", i);
	     if (PD_inquire_entry(file, name, TRUE, NULL) == NULL)
	        break;};

	PD_wrt_pdb_curve(file, f->name, (int) f->domain->n_elements,
			 *(double **) f->domain->elements,
			 *(double **) f->range->elements, i);

/* add to menu */
	_SX_push_menu_item(po, name, "PG_curve");};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_IMAGE_PDBDATA - given a PG_image object,
 *                    - allocate and fill a PDB data object with the PG_image.
 *                    - Write the image to the file and return the data object.
 */

static object *_SXI_image_pdbdata(object *argl)
   {long i;
    char *name;
    PG_image *f;
    g_file *file;
    PDBfile *strm;
    object *ret;

    f    = NULL;
    file = NULL;
    strm = NULL;
    SS_args(argl,
            G_IMAGE, &f,
            G_FILE, &file,
            0);

    if ((file == NULL) || (file == SX_gvif))
       {file = SX_gvif;
	strm = SX_vif;}
    else if (strcmp(file->type, PDBFILE_S) == 0)
       strm = (PDBfile *) file->file;
    else
       SS_error("BAD FILE - _SXI_IMAGE_PDBDATA", argl);

    if (f == NULL)
       SS_error("BAD ARGUMENT - _SXI_IMAGE_PDBDATA", argl);

    _SX_get_menu(file);
    for (i = 0; TRUE; i++)
        {name = SC_dsnprintf(FALSE, "Image%ld", i);
         if (PD_inquire_entry(strm, name, TRUE, NULL) == NULL)
            break;};

    ret = SX_pdbdata_handler(strm, name, "PG_image *", &f , TRUE);

/* add to menu */
    _SX_push_menu_item(file, name, "PG_image *");

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DEVICEP - function version of SX_DEVICEP macro */

static object *_SXI_devicep(object *obj)
   {object *o;

    o = SX_DEVICEP(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GRAPHP - function version of SX_GRAPHP macro */

static object *_SXI_graphp(object *obj)
   {object *o;

    o = SX_GRAPHP(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_IMAGEP - function version of SX_IMAGEP macro */

static object *_SXI_imagep(object *obj)
   {object *o;

    o = SX_IMAGEP(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DEV_ATTRIBUTESP - function version of SX_DEV_ATTRIBUTESP macro */

static object *_SXI_dev_attributesp(object *obj)
   {object *o;

    o = SX_DEV_ATTRIBUTESP(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_MAKE_DEVICE - SX level interface to PG_make_device */

static object *_SXI_make_device(object *argl)
   {int i;
    char *name, *type, *title;
    PG_device *dev;
    out_device *out;
    object *o;

    name  = NULL;
    type  = NULL;
    title = NULL;
    SS_args(argl,
            SC_STRING_I, &name,
            SC_STRING_I, &type,
            SC_STRING_I, &title,
            0);

/* NOTE: this logic is a little bit strange
 * why should making a device be related to any of the output devices?
 */
    if (title == NULL)
       {for (i = 0; i < N_OUTPUT_DEVICES; i++)
	    {out = SX_get_device(i);
	     if (out->exist && out->active)
	        {if (strcmp(out->dupp, name) == 0)
		    {title = out->fname;
		     type  = out->type;
		     break;};};};

        if (title == NULL)
           SS_error("NO TITLE SPECIFIED - _SXI_MAKE_DEVICE", SS_null);};
 
    dev = PG_make_device(name, type, title);

    dev->background_color_white = SX_background_color_white;

    o = SX_mk_graphics_device(dev);

    SFREE(name);
    SFREE(type);
    SFREE(title);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DEVICE_PROPS - return a list of device properties: NAME, TYPE, and
 *                   - TITLE
 */

static object *_SXI_device_props(object *argl)
   {PG_device *dev;
    object *rv;

    rv  = SS_null;
    dev = NULL;
    SS_args(argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error("BAD DEVICE - _SXI_DEVICE_PROPS", SS_null);

    else
       rv = SS_make_list(SC_STRING_I, dev->name,
			 SC_STRING_I, dev->type,
			 SC_STRING_I, dev->title,
			 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_WR_GDEVICE - print a g_device */

static void _SX_wr_gdevice(object *obj, object *strm)
   {

    PRINT(SS_OUTSTREAM(strm), "<GRAPHICS-DEVICE|%s,%s,%s>",
                              DEVICE_NAME(obj),
                              DEVICE_TYPE(obj),
                              DEVICE_TITLE(obj));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RL_GDEVICE - gc a device */

static void _SX_rl_gdevice(object *obj)
   {PG_device *dev;

    dev = SS_GET(PG_device, obj);
    PG_rl_device(dev);

    SS_rl_object(obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MK_GRAPHICS_DEVICE - encapsulate a PG_device as an object */

object *SX_mk_graphics_device(PG_device *dev)
   {object *op;

    if (dev == NULL)
       op = SS_null;
    else
       op = SS_mk_object(dev, G_DEVICE, SELF_EV, dev->title,
			 _SX_wr_gdevice, _SX_rl_gdevice);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_REF_MAP - get the particular mapping referred to 
 *                - return SS_null if the index is too large
 *                - for this file
 */

object *SX_get_ref_map(g_file *po, int indx, char *dtype)
   {char type;
    object *argl, *ret;
    SX_menu_item *mi;

    ret = SS_null;

    mi = _SX_get_menu_item(po, indx-1);
    if (mi != NULL)
       {type = mi->type[3];
	switch (type)
	   {case 'i' :
	         argl = SS_make_list(G_FILE, po,
				     SC_INT_I, &indx,
				     0);
		 ret = _SXI_pdbdata_image(argl);
		 SS_GC(argl);
		 break;

	    case 'm' :
            default  :
	         argl = SS_make_list(G_FILE, po,
				     SC_INT_I, &indx,
				     0);
		 ret = _SXI_pdbdata_graph(argl);
		 SS_GC(argl);
		 break;};};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_EXPOSE_EVENT_HANDLER - handle expose events  */

void SX_expose_event_handler(PG_device *dev, PG_event *ev)
   {

    SX_motion_event_handler(dev, ev);

/*
    PRINT(stdout, ".");
    SX_plot();
*/
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MOTION_EVENT_HANDLER - handle motion events  */

void SX_motion_event_handler(PG_device *dev, PG_event *ev)
   {

    if (SX_show_mouse_location)
       PG_print_pointer_location(dev,
                                 SX_show_mouse_location_x,
                                 SX_show_mouse_location_y,
                                 TRUE);
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_UPDATE_EVENT_HANDLER - handle update events  */

void SX_update_event_handler(PG_device *dev, PG_event *ev)
   {int width, height, min_dim, ncol;

    if (dev != NULL)
       {PG_make_device_current(dev);

	SX_window_height_P = PG_window_height(dev);
	SX_window_width_P  = PG_window_width(dev);
	SX_window_P[0]     = dev->g.hwin[0];
	SX_window_P[1]     = dev->g.hwin[2];

	PG_query_screen(dev, &width, &height, &ncol);
	min_dim = min(width, height);

	SX_window_width  = SX_window_width_P/min_dim;
	SX_window_height = SX_window_height_P/min_dim; 

	SX_window_x[0] = SX_window_P[0]/min_dim;
	SX_window_x[1] = SX_window_P[1]/min_dim;           

/*  SX_plot(); */

	 };

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MOUSE_EVENT_HANDLER - handle mouse button events  */

void SX_mouse_event_handler(PG_device *dev, PG_event *ev)
   {int id, qual, type;
    int evx[PG_SPACEDM];
    double wc[PG_SPACEDM];
    PG_mouse_button button;
    object *loc, *fnc;

    if (dev != NULL)
       {PG_make_device_current(dev);

        type = PG_EVENT_TYPE(*ev);

	PG_mouse_event_info(dev, ev, evx, &button, &qual);

	for (id = 0; id < 2; id++)
	    wc[id] = evx[id];

	PG_trans_point(dev, 2, PIXELC, wc, WORLDC, wc);

	loc = SS_make_list(SC_DOUBLE_I, &wc[0],
			   SC_DOUBLE_I, &wc[1],
			   0);

	fnc = _SS_lk_var_valc("handle-mouse-event", SS_Env);
        if (SS_procedurep(fnc))
	   SS_call_scheme("handle-mouse-event",
                          SC_INT_I, &type,
			  SS_OBJECT_I, loc,
			  SC_ENUM_I, &button,
			  SC_INT_I, &qual,
			  0);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_DEFAULT_EVENT_HANDLER - handle events that get through to here */

void SX_default_event_handler(PG_device *dev, PG_event *ev)
   {

    SX_motion_event_handler(dev, ev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_OPEN_DEVICE - SX level interface to PG_open_device */

static object *_SXI_open_device(object *argl)
   {PG_device *dev;
    double xf, yf, dxf, dyf;

    dev = NULL;
    xf  = yf  = 0.0;
    dxf = dyf = 0.0;
    SS_args(argl,
            G_DEVICE, &dev,
            SC_DOUBLE_I, &xf,
            SC_DOUBLE_I, &yf,
            SC_DOUBLE_I, &dxf,
            SC_DOUBLE_I, &dyf,
            0);

    if (dev == NULL)
       SS_error("BAD DEVICE - _SXI_OPEN_DEVICE", SS_null);

    else

/* PG_open_device set dev->hard_copy_device so we can't query it here */
       {if (!SX_gr_mode &&
	    (dev->type_index != PS_DEVICE) &&
	    (dev->type_index != MPEG_DEVICE) &&
	    (dev->type_index != CGMF_DEVICE))
	   return(SS_f);
	
	dev->autodomain = TRUE;
	dev->autorange  = TRUE;
	dev->data_id    = TRUE;

	dev = PG_open_device(dev, xf, yf, dxf, dyf);
	if (dev == NULL)
	   return(SS_f);

	SC_set_put_line(SX_fprintf);
	SC_set_put_string(SX_fputs);
	SC_set_get_line(PG_wind_fgets);

	PG_make_device_current(dev);
	PG_set_viewspace(dev, 2, WORLDC, NULL);
	PG_release_current_device(dev);

	PG_set_default_event_handler(dev, SX_default_event_handler);
	PG_set_mouse_down_event_handler(dev, SX_mouse_event_handler);
	PG_set_mouse_up_event_handler(dev, SX_mouse_event_handler);
	PG_set_motion_event_handler(dev, SX_motion_event_handler);
	PG_set_expose_event_handler(dev, SX_expose_event_handler);
	PG_set_update_event_handler(dev, SX_update_event_handler);};

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_CLOSE_DEVICE - SX level interface to PG_close_device */

static object *_SXI_close_device(object *argl)
   {PG_device *dev;

    dev = NULL;
    SS_args(argl,
            G_DEVICE, &dev,
            0);

    PG_close_device(dev);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_NEXT_COLOR - return the next available color */

int SX_next_color(PG_device *dev)
   {int rv;

    if (SX_default_color != -1)
       {if (dev == NULL)
           rv = _PG_trans_color(PG_console_device, SX_default_color);
        else
           rv = _PG_trans_color(dev, SX_default_color);}

    else
       {_SX.color = max(_SX.color + 1, 1);
        if (dev == NULL)
           rv = _PG_trans_color(PG_console_device, _SX.color);
        else
           rv = _PG_trans_color(dev, _SX.color);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_MAKE_PGS_GRAPH - build a PGS graph object out of a domain, range
 *                     - and attribute set
 *                     -
 *                     - FORM:
 *                     - (pg-make-graph <domain> <range>
 *                     -                 [<centering> <color> <width> <style>
 *                     -                  <emap> <name>])
 *                     -
 *                     - Centering defaults to zone
 *                     - Color defaults to BLUE
 *                     - Width defaults to thinnest possible
 *                     - Style defaults to LINE_SOLID
 *                     - Existence map thru which to plot defaults to all 1's
 *                     - mapping name
 */

static object *_SXI_make_pgs_graph(object *argl)
   {int n, color, style, clr;
    double width;
    char type[MAXLINE];
    char *lbl, *name, *emap;
    pcons *info;
    void *d;
    C_array *arr;
    PM_centering centering;
    PM_set *domain, *range;
    PG_graph *g;
    object *o;

    color     = 1;
    style     = LINE_SOLID;
    centering = U_CENT;
    width     = 0.0;
    arr       = NULL;
    name      = NULL;
    if (++_SX.igg > 'Z')
       _SX.igg = 'A';

    SS_args(argl,
            G_SET, &domain,
            G_SET, &range,
            SC_STRING_I, &name,
            SC_ENUM_I, &centering,
            SC_INT_I, &color,
            SC_DOUBLE_I, &width,
            SC_INT_I, &style,
	    G_NUM_ARRAY, &arr,
            0);

    if (centering == U_CENT)
       centering = (domain->n_elements == range->n_elements) ? N_CENT : Z_CENT;

    if (name == NULL)
       lbl = SC_dsnprintf(FALSE, "%s->%s", domain->name, range->name);
    else
       lbl = SC_dsnprintf(FALSE, name);

    SFREE(name);

/* build the graph
 * NOTE: since the addition of PG_graph info member there will have to
 * be work here to reconnect the rendering attributes
 */
    clr  = _SX.igg % 14 + 1;
    info = PG_set_line_info(NULL, PLOT_CARTESIAN, CARTESIAN_2D,
			    LINE_SOLID, FALSE, 0, clr, 0, 0.0);
    g = PG_make_graph_from_sets(lbl, domain, range, centering,
                                SC_PCONS_P_S, info, _SX.igg, NULL);

    if (arr == NULL)
       {emap = FMAKE_N(char, domain->n_elements,
                       "_SXI_MAKE_PGS_GRAPH:emap");
        memset(emap, 1, domain->n_elements);}
    else
       {emap = NULL;

	PM_ARRAY_CONTENTS(arr, void, n, type, d);

        CONVERT(SC_CHAR_S, (void **) &emap, type, d, n, FALSE);};

    PG_set_attrs_mapping(g->f,
			 "EXISTENCE", SC_CHAR_I, TRUE, emap,
			 NULL);

    o = SX_mk_graph(g);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_SET_LIMITS - add/modify the specified plotting limits for the
 *                - given graph
 */

static void _SX_set_limits(char *t, PM_set *s, object *argl)
   {int i, nd, nset;
    double *extr, *pe, xmn, xmx;

    if (s != NULL)
       {if (SS_nullobjp(argl))
	   extr = NULL;

        else
	   {nd   = s->dimension_elem;
	    extr = FMAKE_N(double, 2*nd, "_SX_SET_LIMITS:extr");
	    pe   = extr;
	    for (i = 0; i < nd; i++, argl = SS_cddr(argl))
	        {if (SS_nullobjp(argl))
		    {SFREE(extr);
		     break;};

		 nset = SS_args(argl,
				SC_DOUBLE_I, &xmn,
				SC_DOUBLE_I, &xmx,
				0);

		 if (nset < 2)
		    SS_error("INSUFFICIENT LIMITS SPECS - _SX_SET_LIMITS",
			     argl);

		 *pe++ = xmn;
		 *pe++ = xmx;};};

	PM_set_limits(s, extr);};
    
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_GET_LIMITS - return the plotting LIMITS of the given set if any
 *                - in a form suitable for _SX_SET_LIMITS
 */

static object *_SX_get_limits(PM_set *s)
   {int i, nd;
    double *limits;
    object *obj, *lst;

    lst    = SS_null;
    nd     = 2*(s->dimension_elem);
    limits = PM_get_limits(s);
    
    if (limits != NULL)
       {limits = PM_array_real(s->es_type, limits, nd, NULL);

	for (i = 0; i < nd; i++)
	    {obj = SS_mk_float(*limits++);
	     lst = SS_mk_cons(obj, lst);};
    
	SFREE(limits);

	if (lst != SS_null)
	   lst = SS_reverse(lst);};

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_GET_EXTREMA - return the extrema of the given set in a form suitable
 *                 - for _SX_SET_LIMITS
 */

static object *_SX_get_extrema(PM_set *s)
   {int i, nd;
    double *extr;
    object *obj, *lst;

    nd = 2*(s->dimension_elem);
    
    extr = PM_array_real(s->element_type, s->extrema, nd, NULL);

    lst = SS_null;
    for (i = 0; i < nd; i++)
        {obj = SS_mk_float(*extr++);
	 lst = SS_mk_cons(obj, lst);};
    
    SFREE(extr);

    if (lst != SS_null)
       lst = SS_reverse(lst);

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DRAW_DOMAIN - main Scheme level domain plot control routine */

static object *_SXI_draw_domain(object *argl)
   {PG_device *dev;
    PM_set *data, *nxt, *p;
    PG_rendering pty;
    object *obj, *extr;

    dev = NULL;

    if (SS_consp(argl))
       SX_GET_OBJECT_FROM_LIST(SX_DEVICEP(obj), dev,
                               SS_GET(PG_device, obj),
                               argl, "BAD DEVICE - _SXI_DRAW_DOMAIN");

    if (dev == NULL)
       return(SS_null);

/* get the list of sets */
    data = nxt = NULL;
    while (SS_consp(argl))
       {obj = SS_car(argl);
        if (SX_SETP(obj))
           {if (data == NULL)
               data = nxt = SS_GET(PM_set, obj);
            else
               {nxt->next = SS_GET(PM_set, obj);
                nxt = nxt->next;};}
        else
           break;
        argl = SS_cdr(argl);};

    pty  = PLOT_WIRE_MESH;
    extr = SS_null;
    SS_args(argl,
            SC_ENUM_I, &pty,
	    SS_OBJECT_I, &extr,
	    0);

    if (!SS_nullobjp(extr))
       _SX_set_limits("LIMITS", data, extr);

    if (data == NULL)
       SS_error("BAD DOMAIN - _SXI_DRAW_DOMAIN", obj);

    else
       {data->info = PG_set_plot_type((pcons *) data->info, pty, CARTESIAN_2D);

	PG_set_attrs_set(data,
			 "THETA", SC_DOUBLE_I, FALSE, SX_theta,
			 "PHI",   SC_DOUBLE_I, FALSE, SX_phi,
			 "CHI",   SC_DOUBLE_I, FALSE, SX_chi,
			 NULL);

	PG_domain_plot(dev, data, NULL);

/* disconnect the meshes */
	for (p = data; p != NULL; p = nxt)
	    {nxt = p->next;
	     p->next = NULL;};};

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_ATTACH_RENDERING_1D - set up the rendering and axis type for all
 *                         - of the graphs in the list
 */

static void _SX_attach_rendering_1d(PG_graph *data, PG_rendering pty,
				    int axis_type, int start)
   {PG_graph *g;
    pcons *line_info;

    for (g = data; g != NULL; g = g->next)
        {line_info = (pcons *) g->info;
	 line_info = PG_set_plot_type(line_info, pty, axis_type);
         g->info   = (void *) line_info;

         if (pty == PLOT_SCATTER)
	    PG_set_attrs_graph(g,
			       "SCATTER", SC_INT_I, FALSE, 1,
			       NULL);

         else if (pty == PLOT_HISTOGRAM)
	    PG_set_attrs_graph(g,
			       "HIST-START", SC_INT_I, FALSE, start,
			       NULL);

         g->render = PG_curve_plot;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DRAW_PLOT - main Scheme level plot control routine */

static object *_SXI_draw_plot(object *argl)
   {int domain_dim, range_dim;
    int *hsts;
    PG_rendering pty, apty;
    PG_device *dev;
    PG_graph *data, *nxt, *g;
    PM_mapping *f;
    PM_set *domain, *range;
    pcons *info;
    object *obj;
	
    dev = NULL;

    if (SS_consp(argl))
       SX_GET_OBJECT_FROM_LIST(SX_DEVICEP(obj), dev,
                               SS_GET(PG_device, obj),
                               argl, "BAD DEVICE - _SXI_DRAW_PLOT");

    if ((dev == NULL) || (!SX_OK_TO_DRAW(dev)))
       return(SS_f);

/* get the list of graphs */
    data = nxt = NULL;
    for ( ; SS_consp(argl); argl = SS_cdr(argl))
        {obj = SS_car(argl);
	 if (SX_GRAPHP(obj))
            {if (data == NULL)
                data = nxt = SS_GET(PG_graph, obj);
             else
                {nxt->next = SS_GET(PG_graph, obj);
		 nxt = nxt->next;};}

	 else if (SX_MAPPINGP(obj))
            {f = SS_GET(PM_mapping, obj);

/* here's a GC nightmare! */
	     g = PG_make_graph_from_mapping(f, NULL, NULL,
					    SX_map_count(), NULL);
	     if (data == NULL)
                data = nxt = g;
	     else
                {nxt->next = g;
		 nxt = nxt->next;};}
	 else
            break;};

    if (data == NULL)
       SS_error("BAD GRAPH - _SXI_DRAW_PLOT", obj);

    if (_SX_grotrian_graphp(data))
       {PG_grotrian_plot(dev, data);
        return(SS_t);};

    f          = data->f;
    domain     = f->domain;
    range      = f->range;
    domain_dim = (domain == NULL) ? 0 : domain->dimension_elem;
    range_dim  = (range == NULL) ? 0 : range->dimension_elem;
    pty        = data->rendering;
    info       = (pcons *) data->info;

    if (SS_consp(argl))
       {SX_GET_OBJECT_FROM_LIST(SS_integerp(obj), apty,
                                SS_INTEGER_VALUE(obj),
                                argl, "BAD PLOT TYPE - _SXI_DRAW_PLOT");

/*	if ((pty == PLOT_NONE) && (apty != PLOT_NONE)) */
	   pty = apty;

	info = PG_set_plot_type(info, pty, CARTESIAN_2D);

	switch (domain_dim)
	   {case 1 :
	         switch (pty)
		    {case PLOT_MESH :
		          data->render = PG_mesh_plot;
			  break;

		     default        :
		     case PLOT_CARTESIAN :
		     case PLOT_LOGICAL   :
		          data->render = PG_curve_plot;
			  break;

		     case PLOT_HISTOGRAM :
		          hsts = PG_ptr_attr_glb("hist-start");
		          if (SS_consp(argl))
			     {SX_GET_INTEGER_FROM_LIST(*hsts, argl,
						       "BAD HISTOGRAM OPTION - _SXI_DRAW_PLOT");};
			  _SX_attach_rendering_1d(data, PLOT_HISTOGRAM, CARTESIAN_2D, *hsts);
			  break;

		     case PLOT_POLAR :
		          _SX_attach_rendering_1d(data, PLOT_POLAR, POLAR, 0);
			  break;

		     case PLOT_INSEL :
		          _SX_attach_rendering_1d(data, PLOT_INSEL, INSEL, 0);
			  break;

		     case PLOT_SCATTER :
		          _SX_attach_rendering_1d(data, PLOT_SCATTER, CARTESIAN_2D, 0);
		          break;};
                 break;

	    default :
	         switch (pty)
		    {case PLOT_IMAGE :
		          data->render = PG_image_plot;
			  break;

		     case PLOT_DV_BND :
		          data->render = PG_dv_bnd_plot;
			  break;

		     case PLOT_FILL_POLY :
		          data->render = PG_poly_fill_plot;
			  break;

		     case PLOT_SURFACE   :
		     case PLOT_WIRE_MESH :
		          data->render = PG_surface_plot;
			  break;

		     case PLOT_MESH :
		          data->render = PG_mesh_plot;
			  break;

		     case PLOT_SCATTER :
		          data->render = PG_scatter_plot;
			  break;

		     default :
		     case PLOT_CONTOUR :
		          if (SS_consp(argl))
			     {int *nlev;

			      nlev = PG_ptr_attr_glb("contour-n-levels");
			      SX_GET_OBJECT_FROM_LIST(SS_integerp(obj),
						      *nlev,
						      SS_INTEGER_VALUE(obj),
						      argl,
						      "BAD NUMBER OF LEVELS - _SXI_DRAW_PLOT");};

			  data->render = PG_contour_plot;
			  break;

		     case PLOT_VECTOR :
		          data->render = PG_vector_plot;
		          break;};
                 break;};

	*rendering_mode[domain_dim][range_dim] = pty;};

    pty = *rendering_mode[domain_dim][range_dim];

    data->rendering = pty;
    data->info      = (void *) info;
/*    dev->data_id    = TRUE; */

    if ((pty == PLOT_SURFACE) ||
        (pty == PLOT_SCATTER) ||
        (pty == PLOT_CONTOUR) ||
        (pty == PLOT_WIRE_MESH) ||
        (pty == PLOT_MESH))
       {PG_set_attrs_graph(data,
			   "THETA", SC_DOUBLE_I,  FALSE, SX_theta,
			   "PHI",   SC_DOUBLE_I,  FALSE, SX_phi,
			   "CHI",   SC_DOUBLE_I,  FALSE, SX_chi,
			   NULL);
	PG_set_attrs_mapping(data->f,
			     "THETA", SC_DOUBLE_I,  FALSE, SX_theta,
			     "PHI",   SC_DOUBLE_I,  FALSE, SX_phi,
			     "CHI",   SC_DOUBLE_I,  FALSE, SX_chi,
			     NULL);};

    PG_set_axis_log_scale(dev, 2, SX_log_scale);
    PG_turn_grid(dev, SX_grid);

    data->rendering = pty;

    SC_set_put_line(SX_fprintf);
    SC_set_put_string(SX_fputs);

    if ((domain != NULL) && (domain->info_type != NULL))
       {if (strcmp(domain->info_type, SC_PCONS_P_S) == 0)
	  dev->autodomain = (SC_assoc_entry((pcons *) domain->info,
					    "LIMITS") == NULL);};

    if ((range != NULL) && (range->info_type != NULL))
       {if (strcmp(range->info_type, SC_PCONS_P_S) == 0)
	  dev->autorange  = (SC_assoc_entry((pcons *) range->info,
					    "LIMITS") == NULL);};

    if (data->f->next != NULL)
       {int refm;
	pcons *oinf, *ninf;

	oinf = (pcons *) data->f->map;
	ninf = SC_copy_alist(oinf);
	ninf = SC_append_alist(ninf, SC_copy_alist(info));
	data->f->map = (void *) ninf;

	PG_get_attrs_glb(TRUE,
			 "ref-mesh", &refm,
			 NULL);

	if (pty == PLOT_SURFACE)
	   PG_set_attrs_mapping(data->f,
				"DRAW-MESH", SC_INT_I, FALSE, refm,
				NULL);

	PG_draw_picture(dev, data->f, pty,
			TRUE, dev->WHITE, LINE_SOLID, 1.2,
			refm, dev->WHITE, LINE_SOLID, 0.0);

	SC_free_alist(ninf, 1);
	data->f->map = (void *) oinf;}

    else if (data->render != NULL)
       data->render(dev, data);

    else
       PG_draw_graph(dev, data);

    PG_draw_interface_objects(dev);

    SC_set_put_line(SS_printf);
    SC_set_put_string(SS_fputs);

/* unchain the list of graphs */
    while (data->next != NULL)
       {nxt = data->next;
        data->next = NULL;
        data = nxt;};

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_MAKE_IMAGE - crack the arg list, make, and return an image object
 *                  - Usage: (pg-make-image <data> <w> <h>
 *                  -                       [<name> <xmn> <xmx> <ymn> <ymx>
 *                  -                        <zmn> <zmx>])
 */

static object *_SXI_make_image(object *argl)
   {int w, h;
    char *name;
    double dbx[PG_BOXSZ], rbx[PG_BOXSZ];
    C_array *arr;
    PG_image *im;
    object *rv;

    rv   = SS_null;
    name = "Image";
    w    = 0;
    h    = 0;

    PG_box_init(2, dbx, 0.0, 0.0);
    PG_box_init(1, rbx, 0.0, 0.0);

    arr = NULL;
    SS_args(argl,
            G_NUM_ARRAY, &arr,
            SC_INT_I, &w,
            SC_INT_I, &h,
            SC_STRING_I, &name,
            SC_DOUBLE_I, &dbx[0],
            SC_DOUBLE_I, &dbx[1],
            SC_DOUBLE_I, &dbx[2],
            SC_DOUBLE_I, &dbx[3],
            SC_DOUBLE_I, &rbx[0],
            SC_DOUBLE_I, &rbx[1],
            0);

    if (name == NULL)
       name = "Image";

    if (arr == NULL)
       SS_error("BAD DATA - _SXI_MAKE_IMAGE", argl);

    else
       {im = PG_make_image_n(name, arr->type, arr->data,
			     2, WORLDC, dbx, rbx, w, h, 8, NULL);
	rv = SX_mk_image(im);};

    SFREE(name);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_BUILD_IMAGE - crack the arg list, build, and return an image object
 *                  - Usage: (pg-build-image <dev> <data> <w> <h>
 *                  -                         [<name> <xmin> <xmax>
 *                  -                          <ymin> <ymax>
 *                  -                          <zmin> <zmax>])
 */

static object *_SXI_build_image(object *argl)
   {int w, h;
    double dbx[PG_BOXSZ], rbx[PG_BOXSZ];
    char *name;
    C_array *arr;
    PG_device *dev;
    PG_image *im;
    object *rv;

    rv   = SS_null;
    name = "Image";
    w  = 0;
    h  = 0;

    PG_box_init(2, dbx, 0.0, 0.0);
    PG_box_init(1, rbx, 0.0, 0.0);

    dev = NULL;
    arr = NULL;
    SS_args(argl,
            G_DEVICE, &dev,
            G_NUM_ARRAY, &arr,
            SC_INT_I, &w,
            SC_INT_I, &h,
            SC_STRING_I, &name,
            SC_DOUBLE_I, &dbx[0],
            SC_DOUBLE_I, &dbx[1],
            SC_DOUBLE_I, &dbx[2],
            SC_DOUBLE_I, &dbx[3],
            SC_DOUBLE_I, &rbx[0],
            SC_DOUBLE_I, &rbx[1],
            0);

    if (name == NULL)
       name = "Image";

    if (dev == NULL)
       SS_error("BAD DEVICE - _SXI_BUILD_IMAGE", argl);

    if (arr == NULL)
       SS_error("BAD DATA - _SXI_BUILD_IMAGE", argl);

    else
       {im = PG_build_image(dev, name, arr->type, arr->data,
			    w, h, 2, WORLDC, dbx, rbx);
	rv = SX_mk_image(im);};

    SFREE(name);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DRAW_IMAGE - Scheme level image plot control routine */

static object *_SXI_draw_image(object *argl)
   {int vp;
    double xmn, xmx, ymn, ymx, *pv;
    PG_device *dev;
    PG_image *image;
    pcons *alist;

    dev   = NULL;
    image = NULL;
    xmn   = 0.0;
    xmx   = 0.0;
    ymn   = 0.0;
    ymx   = 0.0;
    alist = NULL;
    vp    = FALSE;

    SS_args(argl,
            G_DEVICE, &dev,
            G_IMAGE, &image,
            SC_DOUBLE_I, &xmn,
            SC_DOUBLE_I, &xmx,
            SC_DOUBLE_I, &ymn,
            SC_DOUBLE_I, &ymx,
            0);

    if (!SX_OK_TO_DRAW(dev))
       return(SS_f);

/* if viewport box has been specified, create an assoc list and pass it in */
    if ((xmn < xmx) && (ymn < ymx))
       {vp = TRUE;
        pv = FMAKE_N(double, 4, "_SXI_DRAW_IMAGE:pv");
        pv[0] = xmn;
        pv[1] = xmx;
        pv[2] = ymn;
        pv[3] = ymx;
	PG_set_attrs_alist(alist,
			   "VIEW-PORT", SC_DOUBLE_I, TRUE, pv,
			   NULL);};
            
    PG_draw_image(dev, image, image->label, alist);

    if (vp == TRUE)
       {alist = PG_rem_attrs_alist(alist, "VIEW-PORT", NULL);
        SFREE_N(pv, 4);}    

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_SET_ATTR_ALIST - set an attribute on an alist */

pcons *SX_set_attr_alist(pcons *inf, char *name, char *type, object *val)
   {void *v;
    object *obj;
    C_array *arr;

    if (strcmp(type, "nil") == 0)
       {if (inf != NULL)
	   inf = SC_rem_alist(inf, name);
	return(inf);};

    v = NULL;

    if (SS_consp(val))
       {char dtype[MAXLINE], stype[MAXLINE];

	obj = SS_null;
	SS_Assign(obj, SX_list_array(val));
	SS_args(obj,
		G_NUM_ARRAY, &arr,
		0);

	strcpy(dtype, type);
	strcpy(stype, arr->type);
	SC_dereference(dtype);
	SC_dereference(stype);
	v = NULL;
	SC_convert(dtype, &v, stype, arr->data, arr->length, TRUE);

	PM_rel_array(arr);

	SC_mark(v, 1);
	SS_GC(obj);
	SC_mark(v, -1);}

    else if ((strcmp(type, SC_INT_P_S) == 0) ||
	     (strcmp(type, "integer *") == 0))
       {v = SC_alloc_nzt(1L, sizeof(int), NULL, NULL);
        SS_args(val,
		SC_INT_I, v,
		0);}

    else if (strcmp(type, SC_DOUBLE_P_S) == 0)
       {v = SC_alloc_nzt(1L, sizeof(double), NULL, NULL);
        SS_args(val,
		SC_DOUBLE_I, v,
		0);}

    else if (strcmp(type, SC_STRING_S) == 0)
       {SS_args(val,
		SC_STRING_I, &v,
		0);}

    else
       SS_error("CAN'T HANDLE TYPE - SX_SET_ATTR_ALIST", val);

    inf = SC_change_alist(inf, name, type, v);

    return(inf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SET_ATTR_GRAPH - set an attribute of a PG_graph object
 *                     - usage: (pg-set-graph-attribute! <graph>
 *                     -                         <name> <type> <value>)
 */

static object *_SXI_set_attr_graph(object *argl)
   {PG_graph *g;
    char *name, *type;
    object *val;
    pcons *inf;

    g    = NULL;
    name = NULL;
    type = NULL;
    val  = SS_null;
    SS_args(argl,
            G_GRAPH, &g,
            SC_STRING_I, &name,
            SC_STRING_I, &type,
            SS_OBJECT_I, &val,
            0);

    if ((g == NULL) || (name == NULL) || (type == NULL))
       SS_error("INSUFFICIENT ARGUMENTS - _SXI_SET_ATTR_GRAPH", argl);

    else

/* get the current list */
       {if (g->info_type != NULL)
	   {if (strcmp(g->info_type, SC_PCONS_P_S) == 0)
	       inf = (pcons *) g->info;
	    else
	       inf = NULL;}
        else
	   inf = NULL;

	g->info      = SX_set_attr_alist(inf, name, type, val);
	g->info_type = SC_PCONS_P_S;};

    SFREE(name);
    SFREE(type);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MAP_RAN_EXTREMA - return the overall extrema of the given, possibly linked,
 *                     - mapping in a form suitable for _SX_SET_LIMITS
 */

static object *_SX_map_ran_extrema(PM_mapping *m)
   {int i, nd;
    double extr[20], rextr[20];
    PM_set *s;
    PM_mapping *f;
    object *obj, *lst;

    nd = 0;

/* NOTE: allow up to 10 dimensional ranges */
    for (i = 0; i < 10; i++)
        {rextr[2*i]   =  HUGE_REAL;
         rextr[2*i+1] = -HUGE_REAL;};

    for (f = m; f != NULL; f = f->next)
        {s = f->range;
	 if (s != NULL)
	    {nd = 2*(s->dimension_elem);
    
	     PM_array_real(s->element_type, s->extrema, nd, extr);

	     for (i = 0; i < (nd/2); i++)
	         {rextr[2*i]   = min(rextr[2*i], extr[2*i]);
		  rextr[2*i+1] = max(rextr[2*i+1], extr[2*i+1]);};};};

    lst = SS_null;
    for (i = 0; i < nd; i++)
        {obj = SS_mk_float(rextr[i]);
	 lst = SS_mk_cons(obj, lst);};
    
    if (lst != SS_null)
       lst = SS_reverse(lst);

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MAP_DOM_EXTREMA - return the overall extrema of
 *                     - the given, possibly linked, mapping
 *                     - in a form suitable for _SX_SET_LIMITS
 */

static object *_SX_map_dom_extrema(PM_mapping *m)
   {int i, id, n, nd;
    double *extr, *rextr;
    PM_set *s;
    PM_mapping *f;
    object *obj, *lst;

    if (m == NULL)
       return(SS_null);

    nd = m->domain->dimension;
    n  = 2*nd;

    rextr = FMAKE_N(double, n, "_SX_MAP_DOM_EXTREMA:rextr");
    extr  = FMAKE_N(double, n, "_SX_MAP_DOM_EXTREMA:extr");
    for (id = 0; id < nd; id++)
        {rextr[2*id]   = HUGE_REAL;
	 rextr[2*id+1] = -HUGE_REAL;};

    for (f = m; f != NULL; f = f->next)
        {s = f->domain;

/* only include existing domains of the same dimension as the first
 * NOTE: this case is important if the lower dimensional boundary mappings
 *       are present
 */
	 if ((s != NULL) && (s->dimension == nd))
	    {PM_array_real(s->element_type, s->extrema, n, extr);

	     for (id = 0; id < nd; id++)
	         {rextr[2*id]   = min(rextr[2*id], extr[2*id]);
		  rextr[2*id+1] = max(rextr[2*id+1], extr[2*id+1]);};};};

    lst = SS_null;
    for (i = 0; i < n; i++)
        {obj = SS_mk_float(rextr[i]);
	 lst = SS_mk_cons(obj, lst);};
    
    if (lst != SS_null)
       lst = SS_reverse(lst);

    SFREE(rextr);
    SFREE(extr);

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_IM_DOM_EXTREMA - return the extrema of the given image domain in a form
 *                    - suitable for _SX_SET_LIMITS
 */

static object *_SX_im_dom_extrema(PG_image *im)
   {object *obj, *lst;

    lst = SS_null;
    obj = SS_mk_float(im->ymax);
    lst = SS_mk_cons(obj, lst);
    obj = SS_mk_float(im->ymin);
    lst = SS_mk_cons(obj, lst);
    obj = SS_mk_float(im->xmax);
    lst = SS_mk_cons(obj, lst);
    obj = SS_mk_float(im->xmin);
    lst = SS_mk_cons(obj, lst);

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_IM_RAN_EXTREMA - return the extrema of the given image range in a form
 *                    - suitable for _SX_SET_LIMITS
 */

static object *_SX_im_ran_extrema(PG_image *im)
   {object *obj, *lst;

    lst = SS_null;
    obj = SS_mk_float(im->zmax);
    lst = SS_mk_cons(obj, lst);
    obj = SS_mk_float(im->zmin);
    lst = SS_mk_cons(obj, lst);

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SET_DOM_LIMITS - set the domain plotting limits for the given graph
 *                     - form:
 *                     -   (pg-set-domain-limits! <graph> (<x1_min> <x1_max> .... ))
 */

static object *_SXI_set_dom_limits(object *argl)
   {PM_set *s;
    object *obj;

    s = NULL;
    obj = SS_car(argl);
    if (SX_SETP(obj))
       s = SS_GET(PM_set, obj);
    else if (SX_MAPPINGP(obj))
       s = SS_GET(PM_mapping, obj)->domain;
    else if (SX_GRAPHP(obj))
       s = SS_GET(PG_graph, obj)->f->domain;
    else
       SS_error("NO SET IMPLIED - _SXI_SET_DOM_LIMITS", obj);

    _SX_set_limits("LIMITS", s, SS_cadr(argl));

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SET_RAN_LIMITS - set the range plotting limits for the given graph
 *                     - form:
 *                     -   (pg-set-range-limits! <graph> (<x1_min> <x1_max> .... ))
 */

static object *_SXI_set_ran_limits(object *argl)
   {PM_set *s;
    object *obj;

    s = NULL;
    obj = SS_car(argl);

/* use pg-set-domain-limits! for bare set
 * there is logic in vp-update-drawable that demands this
 */
    if (SX_SETP(obj))
       return(obj);

    else if (SX_MAPPINGP(obj))
       s = SS_GET(PM_mapping, obj)->range;
    else if (SX_GRAPHP(obj))
       s = SS_GET(PG_graph, obj)->f->range;
    else
       SS_error("NO SET IMPLIED - _SXI_SET_RAN_LIMITS", obj);

    _SX_set_limits("LIMITS", s, SS_cadr(argl));

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DOM_EXTREMA - return the domain extrema for the given drawable form:
 *                  -   (pg-domain-extrema <drawable>)
 */

static object *_SXI_dom_extrema(object *obj)
   {PM_set *s;
    PM_mapping *m;
    object *rv;

    s  = NULL;
    rv = SS_null;

    if (SX_SETP(obj))
       s = SS_GET(PM_set, obj);

    else if (SX_MAPPINGP(obj))
       {m  = SS_GET(PM_mapping, obj);
        rv = _SX_map_dom_extrema(m);}

    else if (SX_GRAPHP(obj))
       {m  = SS_GET(PG_graph, obj)->f;
        rv = _SX_map_dom_extrema(m);}

    else if (SX_IMAGEP(obj))
       rv = _SX_im_dom_extrema(SS_GET(PG_image, obj));

    else
       SS_error("NO SET IMPLIED - _SXI_DOM_EXTREMA", obj);

    if (s != NULL)
       rv = _SX_get_extrema(s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DOM_LIMITS - return the domain plotting LIMITS if any
 *                 - for the given drawable form:
 *                 -   (pg-domain-limits <drawable>)
 */

static object *_SXI_dom_limits(object *argl)
   {PM_set *s;
    object *obj, *rv;

    rv  = SS_null;
    s   = NULL;
    obj = SS_car(argl);

    if (SX_SETP(obj))
       s = SS_GET(PM_set, obj);

    else if (SX_MAPPINGP(obj))
       s = SS_GET(PM_mapping, obj)->domain;

    else if (SX_GRAPHP(obj))
       s = SS_GET(PG_graph, obj)->f->domain;

    else if (SX_IMAGEP(obj))
       s = NULL;

    else
       SS_error("NO SET IMPLIED - _SXI_DOM_LIMITS", obj);

    if (s != NULL)
       rv = _SX_get_limits(s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_RAN_EXTREMA - return the range extrema for the given drawable form:
 *                  -   (pg-range-extrema <drawable>)
 */

static object *_SXI_ran_extrema(object *obj)
   {PM_set *s;
    PM_mapping *m;
    object *rv;

    s  = NULL;
    rv = SS_null;

    if (SX_SETP(obj))
       {s  = SS_GET(PM_set, obj);
	rv = _SX_get_extrema(s);}

    else if (SX_MAPPINGP(obj))
       {m  = SS_GET(PM_mapping, obj);
        rv = _SX_map_ran_extrema(m);}

    else if (SX_GRAPHP(obj))
       {m  = SS_GET(PG_graph, obj)->f;
        rv = _SX_map_ran_extrema(m);}

    else if (SX_IMAGEP(obj))
       rv = _SX_im_ran_extrema(SS_GET(PG_image, obj));

    else
       SS_error("NO SET IMPLIED - _SXI_RAN_EXTREMA", obj);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_RAN_LIMITS - return the range plotting LIMITS if any
 *                 - for the given drawable form:
 *                 -   (pg-range-extrema <drawable>)
 */

static object *_SXI_ran_limits(object *argl)
   {PM_set *s;
    object *obj, *rv;

    s   = NULL;
    rv  = SS_null;
    obj = SS_car(argl);

    if (SX_SETP(obj))
       s = SS_GET(PM_set, obj);
    else if (SX_MAPPINGP(obj))
       s = SS_GET(PM_mapping, obj)->range;
    else if (SX_GRAPHP(obj))
       s = SS_GET(PG_graph, obj)->f->range;
    else if (SX_IMAGEP(obj))
       s = NULL;
    else
       SS_error("NO SET IMPLIED - _SXI_RAN_LIMITS", obj);

    if (s != NULL)
       rv = _SX_get_limits(s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SET_LABEL - set the label of the drawable's mapping 
 *                - usage: (pg-set-label! <drawable> <label>)
 */

static object *_SXI_set_label(object *argl)
   {char *label;
    object *obj;

    obj   = NULL;
    label = NULL;
    SS_args(argl,
            SS_OBJECT_I, &obj,
            SC_STRING_I, &label,
            0);

    if ((obj == NULL) || (label == NULL))
       SS_error("INSUFFICIENT ARGUMENTS - _SXI_SET_LABEL", argl);

    if (SX_SETP(obj))
       {PM_set *s;

        s = SS_GET(PM_set, obj);
	SFREE(s->name);
	s->name = label;}

    else if (SX_MAPPINGP(obj))
       {PM_mapping *s;

	s = SS_GET(PM_mapping, obj);
	SFREE(s->name);
	s->name = label;}

    else if (SX_GRAPHP(obj))
       {PM_mapping *s;

	s = SS_GET(PG_graph, obj)->f;
	SFREE(s->name);
	s->name = label;}

    else if (SX_IMAGEP(obj))
       {PG_image *s;

	s = SS_GET(PG_image, obj);
	SFREE(s->label);
	s->label = label;}

    else
       SS_error("BAD DRAWABLE - _SXI_SET_LABEL", obj);

    obj = SS_car(argl);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GET_LABEL - return the label of the drawable's mapping 
 *                - usage: (pg-get-label <drawable>)
 */

static object *_SXI_get_label(object *obj)
   {char *label;
    object *rv;

    label = NULL;

    if (SX_SETP(obj))
       {PM_set *s;

        s = SS_GET(PM_set, obj);
	label = s->name;}

    else if (SX_MAPPINGP(obj))
       {PM_mapping *s;

	s = SS_GET(PM_mapping, obj);
	label = s->name;}

    else if (SX_GRAPHP(obj))
       {PM_mapping *s;

	s = SS_GET(PG_graph, obj)->f;
	label = s->name;}

    else if (SX_IMAGEP(obj))
       {PG_image *s;

	s = SS_GET(PG_image, obj);
	label = s->label;}

    else
       SS_error("BAD DRAWABLE - _SXI_GET_LABEL", obj);

    rv = SS_mk_string(label);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_SET_INFO - get/set information in the PM_set object given */

static object *_SX_set_info(object *obj, char *name, object *val)
   {long v;
    char *t;
    PM_set *s, *u;
    object *ret;

    ret = SS_f;

    s = SS_GET(PM_set, obj);
    if (strcmp(name, "name") == 0)
       {t = s->name;
	if (val != NULL)
	   {SS_args(val, SC_STRING_I, &t);
	    s->name = t;};

	ret = SS_mk_string(t);}

    else if (strcmp(name, "element-type") == 0)
       {t = s->element_type;
	if (val != NULL)
	   {SS_args(val, SC_STRING_I, &t);
	    s->element_type = t;};

	ret = SS_mk_string(t);}

    else if (strcmp(name, "es-type") == 0)
       {t = s->es_type;
	if (val != NULL)
	   {SS_args(val, SC_STRING_I, &t);
	    s->es_type = t;};

	ret = SS_mk_string(t);}

    else if (strcmp(name, "symmetry-type") == 0)
       {t = s->symmetry_type;
	if (val != NULL)
	   {SS_args(val, SC_STRING_I, &t);
	    s->symmetry_type = t;};

	ret = SS_mk_string(t);}

    else if (strcmp(name, "topology-type") == 0)
       {t = s->topology_type;
	if (val != NULL)
	   {SS_args(val, SC_STRING_I, &t);
	    s->topology_type = t;};

	ret = SS_mk_string(t);}

    else if (strcmp(name, "info-type") == 0)
       {t = s->info_type;
	if (val != NULL)
	   {SS_args(val, SC_STRING_I, &t);
	    s->info_type = t;};

	ret = SS_mk_string(t);}

    else if (strcmp(name, "dimension") == 0)
       {v = s->dimension;
	if (val != NULL)
           {SS_args(val, SC_INT_I, &v);
	    s->dimension = v;};

	ret = SS_mk_integer(v);}

    else if (strcmp(name, "dimension-elem") == 0)
       {v = s->dimension_elem;
	if (val != NULL)
           {SS_args(val, SC_INT_I, &v);
	    s->dimension_elem = v;};

	ret = SS_mk_integer(v);}

    else if (strcmp(name, "n-elements") == 0)
       {v = s->n_elements;
	if (val != NULL)
           {SS_args(val, SC_INT_I, &v);
	    s->n_elements = v;};

	ret = SS_mk_integer(v);}

    else if (strcmp(name, "next") == 0)
       {u = s->next;
	if (val != NULL)
	   {SS_args(val, G_SET, &u);
	    s->next = u;};

	ret = SX_mk_set(u);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MAP_INFO - get/set information in the PM_mapping object given */

static object *_SX_map_info(object *obj, char *name, object *val)
   {long v;
    char *t;
    PM_mapping *f, *g;
    PM_set *s;
    object *ret;

    ret = SS_f;

    f = SS_GET(PM_mapping, obj);
    if (strcmp(name, "name") == 0)
       {t = f->name;
	if (val != NULL)
	   {SS_args(val, SC_STRING_I, &t);
	    f->name = t;};

	ret = SS_mk_string(t);}

    else if (strcmp(name, "category") == 0)
       {t = f->category;
	if (val != NULL)
	   {SS_args(val, SC_STRING_I, &t);
	    f->category = t;};

	ret = SS_mk_string(t);}

    else if (strcmp(name, "domain") == 0)
       {s = f->domain;
	if (val != NULL)
	   {SS_args(val, G_SET, &s);
	    f->domain = s;};

	ret = SX_mk_set(s);}

    else if (strcmp(name, "range") == 0)
       {s = f->range;
	if (val != NULL)
	   {SS_args(val, G_SET, &s);
	    f->range = s;};

	ret = SX_mk_set(s);}

    else if (strcmp(name, "next") == 0)
       {g = f->next;
	if (val != NULL)
	   {SS_args(val, G_MAPPING, &g);
	    f->next = g;};

	ret = SX_mk_mapping(f);}

    else if (strcmp(name, "file-type") == 0)
       {v = f->file_type;
	if (val != NULL)
           {SS_args(val, SC_INT_I, &v);
	    f->file_type = v;};

	ret = SS_mk_integer(v);}

    else if (strcmp(name, "map-type") == 0)
       {t = f->map_type;
	if (val != NULL)
	   {SS_args(val, SC_STRING_I, &t);
	    f->map_type = t;};

	ret = SS_mk_string(t);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_GRAPH_INFO - get/set information in the PG_graph object given */

static object *_SX_graph_info(object *obj, char *name, object *val)
   {long v;
    char *t;
    PG_graph *g, *h;
    PM_mapping *f;
    object *ret;

    ret = SS_f;

    g = SS_GET(PG_graph, obj);
    if (strcmp(name, "rendering") == 0)
       {v = g->rendering;
	if (val != NULL)
           {PG_rendering pty;

	    SS_args(val, SC_ENUM_I, &pty);
	    g->rendering = pty;};

	ret = SS_mk_integer(v);}

    else if (strcmp(name, "identifier") == 0)
       {v = g->identifier;
	if (val != NULL)
           {SS_args(val, SC_LONG_I, &v);
	    g->identifier = v;};

	ret = SS_mk_integer(v);}

    else if (strcmp(name, "mesh") == 0)
       {v = g->mesh;
	if (val != NULL)
           {SS_args(val, SC_LONG_I, &v);
	    g->mesh = v;};

	ret = SS_mk_integer(v);}

    else if (strcmp(name, "next") == 0)
       {h = g->next;
	if (val != NULL)
           {SS_args(val, G_GRAPH, &h);
	    g->next = h;};

	ret = SX_mk_graph(h);}

    else if (strcmp(name, "f") == 0)
       {f = g->f;
	if (val != NULL)
	   {SS_args(val, G_MAPPING, &f);
	    g->f = f;};

	ret = SX_mk_mapping(f);}

    else if (strcmp(name, "info-type") == 0)
       {t = g->info_type;
	if (val != NULL)
	   {SS_args(val, SC_STRING_I, &t);
	    g->info_type = t;};

	ret = SS_mk_string(t);}

    else if (strcmp(name, "use") == 0)
       {t = g->use;
	if (val != NULL)
	   {SS_args(val, SC_STRING_I, &t);
	    g->use = t;};

	ret = SS_mk_string(t);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_IMAGE_INFO - get/set information in the PG_image object given */

static object *_SX_image_info(object *obj, char *name, object *val)
   {long v;
    double d;
    char *t;
    PG_image *im;
    object *ret;

    ret = SS_f;

    im = SS_GET(PG_image, obj);
    if (strcmp(name, "version-id") == 0)
       {v = im->version_id;
	if (val != NULL)
           {SS_args(val, SC_LONG_I, &v);
	    im->version_id = v;};

	ret = SS_mk_integer(v);}

    else if (strcmp(name, "kmax") == 0)
       {v = im->kmax;
	if (val != NULL)
           {SS_args(val, SC_LONG_I, &v);
	    im->kmax = v;};

	ret = SS_mk_integer(v);}

    else if (strcmp(name, "lmax") == 0)
       {v = im->lmax;
	if (val != NULL)
           {SS_args(val, SC_LONG_I, &v);
	    im->lmax = v;};

	ret = SS_mk_integer(v);}

    else if (strcmp(name, "size") == 0)
       {v = im->size;
	if (val != NULL)
           {SS_args(val, SC_LONG_I, &v);
	    im->size = v;};

	ret = SS_mk_integer(v);}

    else if (strcmp(name, "bits-pixel") == 0)
       {v = im->bits_pixel;
	if (val != NULL)
           {SS_args(val, SC_LONG_I, &v);
	    im->bits_pixel = v;};

	ret = SS_mk_integer(v);}

    else if (strcmp(name, "label") == 0)
       {t = im->label;
	if (val != NULL)
	   {SS_args(val, SC_STRING_I, &t);
	    im->label = t;};

	ret = SS_mk_string(t);}

    else if (strcmp(name, "element-type") == 0)
       {t = im->element_type;
	if (val != NULL)
	   {SS_args(val, SC_STRING_I, &t);
	    im->element_type = t;};

	ret = SS_mk_string(t);}

    else if (strcmp(name, "xmin") == 0)
       {d = im->xmin;
	if (val != NULL)
	   {SS_args(val, SC_DOUBLE_I, &d);
	    im->xmin = d;};

	ret = SS_mk_float(d);}

    else if (strcmp(name, "xmax") == 0)
       {d = im->xmax;
	if (val != NULL)
	   {SS_args(val, SC_DOUBLE_I, &d);
	    im->xmax = d;};

	ret = SS_mk_float(d);}

    else if (strcmp(name, "ymin") == 0)
       {d = im->ymin;
	if (val != NULL)
	   {SS_args(val, SC_DOUBLE_I, &d);
	    im->ymin = d;};

	ret = SS_mk_float(d);}

    else if (strcmp(name, "ymax") == 0)
       {d = im->ymax;
	if (val != NULL)
	   {SS_args(val, SC_DOUBLE_I, &d);
	    im->ymax = d;};

	ret = SS_mk_float(d);}

    else if (strcmp(name, "zmin") == 0)
       {d = im->zmin;
	if (val != NULL)
	   {SS_args(val, SC_DOUBLE_I, &d);
	    im->zmin = d;};

	ret = SS_mk_float(d);}

    else if (strcmp(name, "zmax") == 0)
       {d = im->zmax;
	if (val != NULL)
	   {SS_args(val, SC_DOUBLE_I, &d);
	    im->zmax = d;};

	ret = SS_mk_float(d);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DRAWABLE_INFO - get/set the drawable's information
 *                    - usage: (pg-drawable-info <obj> <name> [<value>])
 */

static object *_SXI_drawable_info(object *argl)
   {char *name;
    object *obj, *val, *ret;

    ret  = SS_null;
    obj  = NULL;
    name = NULL;
    val  = NULL;
    SS_args(argl,
            SS_OBJECT_I, &obj,
            SC_STRING_I, &name,
            SS_OBJECT_I, &val,
            0);

    if (SX_SETP(obj))
       ret = _SX_set_info(obj, name, val);

    else if (SX_MAPPINGP(obj))
       ret = _SX_map_info(obj, name, val);

    else if (SX_GRAPHP(obj))
       ret = _SX_graph_info(obj, name, val);

    else if (SX_IMAGEP(obj))
       ret = _SX_image_info(obj, name, val);

    else
       SS_error("BAD DRAWABLE - _SXI_DRAWABLE_INFO", obj);

    SFREE(name);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* SX_INSTALL_PGS_FUNCS - install the PGS extensions to Scheme */
 
void SX_install_pgs_funcs(void)
   {double *labpy;
    extern void _SX_install_pgs_primitives(void);

    labpy = PG_ptr_attr_glb("label-position-y");

    SS_install("pdbdata->pg-graph",
               "Read a PGS graph object from a PDB file\nFORM (pdbdata->pg-graph <file> <name>)",
               SS_nargs,
               _SXI_pdbdata_graph, SS_PR_PROC);

    SS_install("pdbdata->pg-image",
               "Read a PGS image object from a PDB file\nFORM (pdbdata->pg-image <file> <name>)",
               SS_nargs,
               _SXI_pdbdata_image, SS_PR_PROC);

    SS_install("pg-build-image",
               "Build a PGS image object\nFORM (pg-build-image <data> <kmax> <lmax> [<name> <xmin> <xmax> <ymin> <ymax> <zmin> zmax>])",
               SS_nargs,
               _SXI_build_image, SS_PR_PROC);

    SS_install("pg-close-device",
               "Closes a PGS device",
               SS_nargs,
               _SXI_close_device, SS_PR_PROC);

    SS_install("pg-def-graph-file",
               "Set up a PDB file to recieve PGS graph objects",
               SS_sargs,
               _SXI_def_file_graph, SS_PR_PROC);

    SS_install("pg-device?",
               "Returns #t if the object is a PGS device, and #f otherwise",
               SS_sargs,
               _SXI_devicep, SS_PR_PROC);

    SS_install("pg-device-attributes?",
               "Returns #t if the object is a set of PGS device attributes, and #f otherwise",
               SS_sargs,
               _SXI_dev_attributesp, SS_PR_PROC);

    SS_install("pg-device-properties",
               "Return a list with NAME, TYPE, and TITLE of the given device",
               SS_nargs,
               _SXI_device_props, SS_PR_PROC);

    SS_install("pg-domain-extrema",
               "Return the domain extrema\nFORM (pg-domain-extrema <graph>)",
               SS_sargs,
               _SXI_dom_extrema, SS_PR_PROC);

    SS_install("pg-domain-limits",
               "Return the domain plotting limits\nFORM (pg-domain-limits <graph>)",
               SS_sargs,
               _SXI_dom_limits, SS_PR_PROC);

    SS_install("pg-draw-domain",
               "Draws the mesh specified by a PML set object\nFORM (pg-draw-domain <device> <set>)",
               SS_nargs,
               _SXI_draw_domain, SS_PR_PROC);

    SS_install("pg-draw-graph",
               "Draws a PGS graph object\nFORM (pg-draw-graph <device> <graph> <rendering> [...])",
               SS_nargs,
               _SXI_draw_plot, SS_PR_PROC);

    SS_install("pg-draw-image",
               "Draws a PGS image object\nFORM (pg-draw-image <device> <image>)",
               SS_nargs,
               _SXI_draw_image, SS_PR_PROC);

    SS_install("pg-get-label",
               "Return the label of the specified mapping/graph/image/set",
               SS_sargs,
               _SXI_get_label, SS_PR_PROC);

    SS_install("pg-graph?",
               "Returns #t if the object is a PGS graph, and #f otherwise",
               SS_sargs,
               _SXI_graphp, SS_PR_PROC);

    SS_install("pg-graph->pdbcurve",
               "Write an ULTRA curve object to a PDB file\nFORM (pg-graph->pdbcurve <curve> <file>)",
               SS_nargs,
               _SXI_graph_pdbcurve, SS_PR_PROC);

    SS_install("pg-graph->pdbdata",
               "Write a PGS graph object to a PDB file\nFORM (pg-graph->pdbdata <graph> <file>)",
               SS_nargs,
               _SXI_graph_pdbdata, SS_PR_PROC);

    SS_install("pg-drawable-info",
               "Get/Set drawable object information\nFORM (pg-drawable-info <name> [<value>])",
               SS_nargs,
               _SXI_drawable_info, SS_PR_PROC);

    SS_install("pg-grotrian-graph?",
               "Returns #t if the object is a PGS grotrian graph, and #f otherwise",
               SS_sargs,
               _SXI_grotrian_graphp, SS_PR_PROC);

    SS_install("pg-image?",
               "Returns #t if the object is a PGS image, and #f otherwise",
               SS_sargs,
               _SXI_imagep, SS_PR_PROC);

    SS_install("pg-image-name",
               "Return the image referenced by name or menu number",
               SS_nargs,
               _SXI_get_text_image_name, SS_PR_PROC);

    SS_install("pg-image->pdbdata",
               "Write a PGS image object to a PDB file\nFORM (pg-image->pdbdata <image> <file>)",
               SS_nargs,
               _SXI_image_pdbdata, SS_PR_PROC);

    SS_install("pg-make-device",
               "Returns a PGS device with NAME, TYPE, and TITLE",
               SS_nargs,
               _SXI_make_device, SS_PR_PROC);

    SS_install("pg-make-graph",
               "Return a PGS graph object\nFORM (pg-make-graph <domain> <range> [<color> <width> <style> <id>])",
               SS_nargs,
               _SXI_make_pgs_graph, SS_PR_PROC);

    SS_install("pg-make-image",
               "Make a PGS image object\nFORM (pg-make-image <data> <kmax> <lmax> [<name> <xmin> <xmax> <ymin> <ymax> <zmin> zmax>])",
               SS_nargs,
               _SXI_make_image, SS_PR_PROC);

    SS_install("pg-mapping-name",
               "Return the mapping referenced by name or menu number",
               SS_nargs,
               _SXI_get_text_mapping_name, SS_PR_PROC);

    SS_install("pg-menu-item-type",
               "Return the name of the type of the referenced menu item",
               SS_nargs,
               _SXI_menu_item_type, SS_PR_PROC);

    SS_install("pg-open-device",
               "Opens a PGS device DEV at (X, Y) with (DX, DY)",
               SS_nargs,
               _SXI_open_device, SS_PR_PROC);

    SS_install("pg-range-extrema",
               "Return the range extrema\nFORM (pg-range-extrema <graph>)",
               SS_sargs,
               _SXI_ran_extrema, SS_PR_PROC);

    SS_install("pg-range-limits",
               "Return the range plotting limits\nFORM (pg-range-limits <graph>)",
               SS_sargs,
               _SXI_ran_limits, SS_PR_PROC);

    SS_install("pg-set-domain-limits!",
               "Set the domain plotting limits for the given graph",
               SS_nargs,
               _SXI_set_dom_limits, SS_PR_PROC);

    SS_install("pg-set-graph-attribute!",
               "Set an attribute of the given graph",
               SS_nargs,
               _SXI_set_attr_graph, SS_PR_PROC);

    SS_install("pg-set-label!",
               "Set the graph/mapping/image/set label to the given string",
               SS_nargs,
               _SXI_set_label, SS_PR_PROC);

    SS_install("pg-set-range-limits!",
               "Set the range plotting limits for the given graph",
               SS_nargs,
               _SXI_set_ran_limits, SS_PR_PROC);

    SS_install_cf("label-y-position",
                  "Internal variable for displaying mapping labels - do NOT set",
                  SS_acc_double,
		  labpy);

/* low level PGS functions */
    _SX_install_pgs_primitives();

/* interface object functions */
    _SX_install_pgs_iob();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
