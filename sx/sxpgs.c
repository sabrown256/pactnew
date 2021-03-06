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

static PG_rendering
 *rendering_mode[4][6] =
    {{&SX_gs.render_def, &SX_gs.render_def,   &SX_gs.render_def,
      &SX_gs.render_def, &SX_gs.render_def,   &SX_gs.render_def},
     {&SX_gs.render_def, &SX_gs.render_1d_1d, &SX_gs.render_def,
      &SX_gs.render_def, &SX_gs.render_def,   &SX_gs.render_def},
     {&SX_gs.render_def, &SX_gs.render_2d_1d, &SX_gs.render_2d_2d,
      &SX_gs.render_def, &SX_gs.render_def,   &SX_gs.render_def},
     {&SX_gs.render_def, &SX_gs.render_def,   &SX_gs.render_def,
      &SX_gs.render_def, &SX_gs.render_def,   &SX_gs.render_def}};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_OPT_PG_DEVICE - handle BLANG binding related operations */

void *_SX_opt_PG_device(PG_device *x, bind_opt wh, void *a)
   {void *rv;
    object *o;
    static char nm[BFSML];

    rv = NULL;
    switch (wh)
       {case BIND_ARG :
	     o = (object *) a;
	     if (SX_DEVICEP(o))
	        rv = SS_GET(PG_device, o);
	     else
	        rv = _SX.unresolved;
	     break;

        case BIND_LABEL :
	     snprintf(nm, BFSML, "%s,%s,%s", x->name, x->type, x->title);
	     rv = CSTRSAVE(nm);
	     break;

        case BIND_PRINT :
	     snprintf(nm, BFSML, "%s,%s,%s", x->name, x->type, x->title);
	     rv = nm;
	     break;

        case BIND_ALLOC :
        case BIND_FREE :
	default:
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_OPT_PG_IMAGE - handle BLANG binding related operations */

void *_SX_opt_PG_image(PG_image *x, bind_opt wh, void *a)
   {void *rv;
    object *o;

    rv = NULL;
    switch (wh)
       {case BIND_ARG :
	     o = (object *) a;
	     if (SX_IMAGEP(o))
	        rv = SS_GET(PG_image, o);
	     else
	        rv = _SX.unresolved;
	     break;

        case BIND_LABEL :
        case BIND_PRINT :
	     rv = x->label;
	     break;

        case BIND_ALLOC :
        case BIND_FREE :
	default:
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_OPT_PG_PALETTE - handle BLANG binding related operations */

void *_SX_opt_PG_palette(PG_palette *x, bind_opt wh, void *a)
   {void *rv;
    object *o;

    rv = NULL;
    switch (wh)
       {case BIND_ARG :
	     o = (object *) a;
	     if (SX_PALETTEP(o))
	        rv = SS_GET(PG_palette, o);
	     else
	        rv = _SX.unresolved;
	     break;

        case BIND_LABEL :
        case BIND_PRINT :
        case BIND_ALLOC :
        case BIND_FREE :
	default:
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_OPT_PG_GRAPH - handle BLANG binding related operations */

void *_SX_opt_PG_graph(PG_graph *x, bind_opt wh, void *a)
   {void *rv;
    object *o;
    static char nm[BFSML];

    rv = NULL;
    switch (wh)
       {case BIND_ARG :
	     o = (object *) a;
	     if (SX_GRAPHP(o))
	        rv = SS_GET(PG_graph, o);
	     else
	        rv = _SX.unresolved;
	     break;

        case BIND_LABEL :
             snprintf(nm, BFSML, "%c", x->identifier);
	     rv = CSTRSAVE(nm);
	     break;

        case BIND_PRINT :
             snprintf(nm, BFSML, "%c", x->identifier);
	     rv = nm;
	     break;

        case BIND_FREE :

/* GOTCHA: don't know if it is safe to GC the mapping or its sets */
	     if (x->info != NULL)
	        CFREE(x->info);
	     CFREE(x);
	     break;

        case BIND_ALLOC :
	default:
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_OPT_PG_DEV_ATTRIBUTES - handle BLANG binding related operations */

void *_SX_opt_PG_dev_attributes(PG_dev_attributes *x, bind_opt wh, void *a)
   {void *rv;

    rv = NULL;
    switch (wh)
       {case BIND_ARG :
	     break;

        case BIND_FREE :
	     CFREE(x);
	     break;

        case BIND_LABEL :
        case BIND_PRINT :
        case BIND_ALLOC :
	default:
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DEF_FILE_GRAPH - define PM_set and PM_mapping to 
 *                     - a PDB file thereby preparing it for mappings
 */

static object *_SXI_def_file_graph(SS_psides *si, object *obj)
   {int ok;
    PDBfile *file;
    SX_file *po;

    po = NULL;
    SS_args(si, obj,
            G_SX_FILE_I, &po,
            0);

    if (po == NULL)
       file = SX_gs.vif;

    else
       {if (strcmp(po->type, G_PDBFILE_S) != 0)
	   SS_error(si, "REQUIRE PDB FILE - _SXI_DEF_FILE_GRAPH", obj);

	file = FILE_FILE(PDBfile, po);};

    if ((file->type == NULL) || (strcmp(file->type, "PDBfile") == 0))
       {if (PD_inquire_type(file, "PM_mapping") == NULL)
	   {ok = PD_def_mapping(file); 
	    if (ok != TRUE)
	       SS_error(si, "CAN`T DEFINE MAPPINGS - _SXI_DEF_FILE_GRAPH",
			obj);};};

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GRAPH_PDBDATA - given a graph object
 *                    - allocate and fill a PDB data object with the
 *                    - PM_mapping and return it
 */

static object *_SXI_graph_pdbdata(SS_psides *si, object *argl)
   {PG_graph *g;
    PM_mapping *f;
    char *name;
    PDBfile *file;
    SX_file *po;
    object *rv;

    rv   = SS_null;
    g    = NULL;
    po   = NULL;
    name = NULL;
    SS_args(si, argl,
            G_PG_GRAPH_I, &g,
            G_SX_FILE_I, &po,
            G_STRING_I, &name,
            0);

    if (strcmp(po->type, G_PDBFILE_S) != 0)
       SS_error(si, "REQUIRE PDB FILE - _SXI_GRAPH_PDBDATA", argl);
    file = FILE_FILE(PDBfile, po);

    if (g == NULL)
       SS_error(si, "BAD GRAPH - _SXI_GRAPH_PDBDATA", argl);

    else

/* disconnect any function pointers or undefined structs/members */
       {for (f = g->f; f != NULL; f = f->next)
	    {f->domain->opers = NULL;
	     f->range->opers  = NULL;};

	rv = SX_pdbdata_handler(si, file, name,
				"PM_mapping *", &(g->f) , TRUE);};

    CFREE(name);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_PDBDATA_GRAPH - worker for mapping side of _SXI_pdbdata_graph */

static object *_SX_pdbdata_graph(SS_psides *si, PDBfile *file,
				 const char *name, syment *ep)
   {int ndd, ndr, nc, clr, ret;
    char dname[MAXLINE];
    char *info_type, *tail;
    void *info;
    SC_address data;
    PG_graph *g;
    PM_mapping *f;
    PM_set *domain, *range;
    object *o;

/* read the mapping */
    if (file == SX_gs.vif)
       {data.diskaddr = PD_entry_address(ep);
        f = *(PM_mapping **) data.memaddr;}
    else
       {if (!PD_read(file, name, &f))
           SS_error(si, PD_get_error(), SS_null);};

/* reconnect any function pointers or undefined structs/members */
    domain = f->domain;
    range  = f->range;
    if (domain == NULL)
       {if (PD_has_directories(file))
           {SC_strncpy(dname, MAXLINE, name, -1);
            tail = strrchr(dname, '/');
	    if (tail == NULL)
	       {tail = dname;
		nc   = MAXLINE;}
            else
	       {tail++;
		nc = MAXLINE - (tail - dname);};

            SC_strncpy(tail, nc, f->name, -1);
            PD_process_set_name(tail);}
        else
           {SC_strncpy(dname, MAXLINE, f->name, -1);
            PD_process_set_name(dname);};

        if (!PD_read(file, dname, &data.memaddr))
           SS_error(si, PD_get_error(), SS_null);

        domain = f->domain = (PM_set *) data.memaddr;};

    ndd = (domain == NULL) ? 0 : domain->dimension_elem;
    ndr = (range == NULL) ? 0 : range->dimension_elem;
    clr = _SX.ig % 14 + 1;

    info_type = G_PCONS_P_S;
    if ((ndd == 1) && (ndr == 1))
       info = PG_set_line_info(NULL, PLOT_CARTESIAN, CARTESIAN_2D,
			       LINE_SOLID, FALSE, 0, clr, 0, 0.0);

    else if ((ndd == 2) && (ndr == 1))
       {int nlev;

	PG_get_attrs_glb(TRUE,
			 "contour-n-levels", &nlev,
			 NULL);
	info = PG_set_tds_info(NULL, PLOT_CONTOUR, CARTESIAN_2D,
			       LINE_SOLID, clr, nlev,
			       1.0, 0.0, 0.0, 0.0, 0.0, HUGE);}

    else if ((ndd == 2) && (ndr == 2))
       info = PG_set_tdv_info(NULL, PLOT_VECTOR, CARTESIAN_2D,
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
                domain->info_type = G_PCONS_P_S;};

         if (range != NULL)
            {ret &= PM_set_opers(range);
	     if (range->info_type == NULL)
                range->info_type = G_PCONS_P_S;};

/* GOTCHA: it is not necessary to die here if you only want to view
 *         the data
 *         doing algebra would be a problem later on however
         if (ret == FALSE)
            SS_error(si, "NO FIELD FOR TYPE - _SXI_PDBDATA_GRAPH", SS_null);
 */
       };

    if (_SX.ig > 'Z')
       _SX.ig = 'A';

    o = SX_make_pg_graph(si, g);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_PDBCURVE_GRAPH - worker for curve side of _SXI_pdbdata_graph */

static object *_SX_pdbcurve_graph(SS_psides *si, PDBfile *file,
				  const char *name, syment *ep)
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
       SS_error(si, PD_get_error(), SS_null);

    domain = PM_make_set("X values", G_DOUBLE_S, FALSE, 1, n, 1, x);
    range  = PM_make_set("Y values", G_DOUBLE_S, FALSE, 1, n, 1, y);

    f    = PM_make_mapping(label, PM_LR_S, domain, range, N_CENT, NULL);
    clr  = _SX.icg % 14 + 1;
    info = PG_set_line_info(NULL, PLOT_CARTESIAN, CARTESIAN_2D,
			    LINE_SOLID, FALSE, 0, clr, 0, 0.0);
    g    = PG_make_graph_from_mapping(f, G_PCONS_P_S, info, _SX.icg++, NULL);

    if (_SX.icg > 1000)
       _SX.icg = 1;

    o = SX_make_pg_graph(si, g);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_GET_MENU_ITEM - return the Ith menu item from PO */

SX_menu_item *_SX_get_menu_item(SS_psides *si, SX_file *po, int i)
   {int n;
    SX_menu_item *mi;

    mi = NULL;

    n = SC_array_get_n(po->menu_lst);
    if ((0 < i) && (i <= n))
       {if (po->menu_lst->array == NULL)
	   _SX_get_menu(si, po);

	mi = SC_array_get(po->menu_lst, i-1);};

    return(mi);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_PDBDATA_GRAPH - read a PG_graph out of a PDB file and
 *                    - return a PGS graph object
 *                    - FORM:
 *                    -    (pdbdata->pg_graph <file> <name>)
 */

static object *_SXI_pdbdata_graph(SS_psides *si, object *argl)
   {object *obj;
    int i;
    char *name;
    PDBfile *file;
    SX_menu_item *mi;
    SX_file *po;
    syment *ep;

    po  = NULL;
    obj = SS_null;
    SS_args(si, argl,
            G_SX_FILE_I, &po,
            G_OBJECT_I, &obj,
            0);

    if (strcmp(po->type, G_PDBFILE_S) != 0)
       SS_error(si, "REQUIRE PDB FILE - _SXI_PDBDATA_GRAPH", argl);

    file = FILE_FILE(PDBfile, po);

    if (SS_integerp(obj))
       {i    = SS_INTEGER_VALUE(obj);
	mi   = _SX_get_menu_item(si, po, i);
	name = (mi == NULL) ? NULL : mi->vname;}
    else
       {argl = SS_cdr(si, argl);
        name = CSTRSAVE(SS_get_string(obj));};

    if (name == NULL)
       return(SS_null);

/* check to see whether or not the variable is in the file */
    ep = PD_inquire_entry(file, name, TRUE, NULL);
    if (ep == NULL)
       return(SS_null);

    if (strcmp(PD_entry_type(ep), "PM_mapping *") == 0)
       return(_SX_pdbdata_graph(si, file, name, ep));

    else if (SC_strstr(name, "curve") != NULL)
       return(_SX_pdbcurve_graph(si, file, name, ep));

    else if (strcmp(PD_entry_type(ep), "PG_image *") == 0)
       /* GOTCHA: images are not currently handled */;

    else
       SS_error(si, "VARIABLE NOT MAPPING - _SXI_PDBDATA_GRAPH",
                SS_mk_string(si, name));

    return(SS_null);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_MENU_ITEM_TYPE - return the type of a menu item */

static object *_SXI_menu_item_type(SS_psides *si, object *argl)
   {int indx;
    char bf[MAXLINE], *s;
    SX_file *po;
    SX_menu_item *mi;
    object *rv;

    po   = NULL;
    indx = -1;
    SS_args(si, argl,
            G_SX_FILE_I, &po,
            G_INT_I, &indx,
            0);

    _SX_get_menu(si, po);

    mi = _SX_get_menu_item(si, po, indx);
    if (mi == NULL)
       rv = SS_null;

    else
       {SC_strncpy(bf, MAXLINE, mi->type + 3, -1);
	rv = SS_mk_string(si, SC_strtok(bf, " *", s));};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GET_TEXT_IMAGE_NAME - given a file and reference to an image by
 *                          - name or menu number,
 *                          - return the image name as an object
 */

static object *_SXI_get_text_image_name(SS_psides *si, object *argl)
   {long i;
    char *name;
    SX_menu_item *mi;
    SX_file *po;
    object *o;

    argl = SX_get_file(si, argl, &po);
    argl = SS_car(si, argl);
    if (SS_integerp(argl))
       {i    = SS_INTEGER_VALUE(argl);
	mi   = _SX_get_menu_item(si, po, i);
	name = (mi == NULL) ? NULL : mi->vname;}
    else
       name = CSTRSAVE(SS_get_string(argl));

    if (name == NULL)
       o = SS_null;
    else
       o = SS_mk_string(si, name);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GET_TEXT_MAPPING_NAME - given a file and reference to a mapping by
 *                            - name or menu number,
 *                            - return the mapping name as an object
 */

static object *_SXI_get_text_mapping_name(SS_psides *si, object *argl)
   {long i;
    char *name;
    SX_menu_item *mi;
    SX_file *po;
    object *o;

    argl = SX_get_file(si, argl, &po);
    argl = SS_car(si, argl);
    if (SS_integerp(argl))
       {i    = SS_INTEGER_VALUE(argl);
	mi   = _SX_get_menu_item(si, po, i);
	name = (mi == NULL) ? NULL : mi->vname;}
    else
       name = CSTRSAVE(SS_get_string(argl));

    if (name == NULL)
       o = SS_null;
    else
       o = SS_mk_string(si, name);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_PDBDATA_IMAGE - read a PG_image out of a PDB file and
 *                    - return a PGS image object
 *                    - FORM:
 *                    -    (pdbdata->pgs_image <file> <name>)
 */

static object *_SXI_pdbdata_image(SS_psides *si, object *argl)
   {object *obj;
    int i;
    char *name;
    SX_menu_item *mi;
    PDBfile *file;
    SX_file *po;
    syment *ep;
    SC_address data;
    PG_image *im;
    object *o;

    if (!SS_consp(argl))
       SS_error(si, "BAD ARGUMENT LIST - _SXI_PDBDATA_IMAGE", argl);

/* if the first object is a pdbfile, use it, otherwise, use default file */
    argl = SX_get_file(si, argl, &po);
    file = (PDBfile *) po->file;

    obj = SS_car(si, argl);
    if (SS_integerp(obj))
       {i    = SS_INTEGER_VALUE(obj);
	mi   = _SX_get_menu_item(si, po, i);
	name = (mi == NULL) ? NULL : mi->vname;}
    else
       {argl = SS_cdr(si, argl);
        name = CSTRSAVE(SS_get_string(obj));};

    if (name == NULL)
       o = SS_null;

    else

/* check to see whether or not the variable is in the file */
       {ep = PD_inquire_entry(file, name, TRUE, NULL);
	if (ep == NULL)
	   SS_error(si, "VARIABLE NOT FOUND - _SXI_PDBDATA_IMAGE", obj);

	else if (strcmp(PD_entry_type(ep), "PG_image *") != 0)
	   SS_error(si, "VARIABLE NOT IMAGE - _SXI_PDBDATA_IMAGE", obj);

/* read the mapping */
	if (file == SX_gs.vif)
	   {data.diskaddr = PD_entry_address(ep);
	    im = *(PG_image **) data.memaddr;}
	else
	   {if (!PD_read(file, name, &im))
	       SS_error(si, PD_get_error(), obj);};

	o = SX_make_pg_image(si, im);};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GRAPH_PDBCURVE - given a PM_mapping or PG_graph object for a curve,
 *                     - write the curve to the specified file
 */

static object *_SXI_graph_pdbcurve(SS_psides *si, object *argl)
   {PM_mapping *f;
    SX_file *po;
    PDBfile *file;
    char *name;
    int i;

    f    = NULL;
    po   = NULL;
    file = NULL;
    SS_args(si, argl,
            G_PM_MAPPING_I, &f,
            G_SX_FILE_I, &po,
            0);

    if ((po == NULL) || (po == SX_gs.gvif))
       {file = SX_gs.vif;
	po   = SX_gs.gvif;}
    else if (strcmp(po->type, G_PDBFILE_S) == 0)
       file = FILE_FILE(PDBfile, po);
    else
       SS_error(si, "BAD FILE - _SXI_GRAPH_PDBCURVE", argl);

    if (f == NULL)
       SS_error(si, "BAD ARGUMENT - _SXI_GRAPH_PDBCURVE", argl);

    else
       {_SX_get_menu(si, po);
	for (i = 0; TRUE; i++)
	    {name = SC_dsnprintf(MAXLINE, "curve%04d", i);
	     if (PD_inquire_entry(file, name, TRUE, NULL) == NULL)
	        break;};

	PD_wrt_pdb_curve(file, f->name, (int) f->domain->n_elements,
			 *(double **) f->domain->elements,
			 *(double **) f->range->elements, i);

/* add to menu */
	_SX_push_menu_item(si, po, name, "PG_curve");};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_IMAGE_PDBDATA - given a PG_image object,
 *                    - allocate and fill a PDB data object with the PG_image.
 *                    - Write the image to the file and return the data object.
 */

static object *_SXI_image_pdbdata(SS_psides *si, object *argl)
   {long i;
    char *name;
    PG_image *f;
    SX_file *file;
    PDBfile *strm;
    object *ret;

    f    = NULL;
    file = NULL;
    strm = NULL;
    SS_args(si, argl,
            G_PG_IMAGE_I, &f,
            G_SX_FILE_I, &file,
            0);

    if ((file == NULL) || (file == SX_gs.gvif))
       {file = SX_gs.gvif;
	strm = SX_gs.vif;}
    else if (strcmp(file->type, G_PDBFILE_S) == 0)
       strm = (PDBfile *) file->file;
    else
       SS_error(si, "BAD FILE - _SXI_IMAGE_PDBDATA", argl);

    if (f == NULL)
       SS_error(si, "BAD ARGUMENT - _SXI_IMAGE_PDBDATA", argl);

    _SX_get_menu(si, file);
    for (i = 0; TRUE; i++)
        {name = SC_dsnprintf(FALSE, "Image%ld", i);
         if (PD_inquire_entry(strm, name, TRUE, NULL) == NULL)
            break;};

    ret = SX_pdbdata_handler(si, strm, name, "PG_image *", &f , TRUE);

/* add to menu */
    _SX_push_menu_item(si, file, name, "PG_image *");

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DEVICEP - function version of SX_DEVICEP macro */

static object *_SXI_devicep(SS_psides *si, object *obj)
   {object *o;

    o = SX_DEVICEP(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GRAPHP - function version of SX_GRAPHP macro */

static object *_SXI_graphp(SS_psides *si, object *obj)
   {object *o;

    o = SX_GRAPHP(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_IMAGEP - function version of SX_IMAGEP macro */

static object *_SXI_imagep(SS_psides *si, object *obj)
   {object *o;

    o = SX_IMAGEP(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DEV_ATTRIBUTESP - function version of SX_DEV_ATTRIBUTESP macro */

static object *_SXI_dev_attributesp(SS_psides *si, object *obj)
   {object *o;

    o = SX_DEV_ATTRIBUTESP(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_MAKE_DEVICE - SX level interface to PG_make_device */

static object *_SXI_make_device(SS_psides *si, object *argl)
   {int i, ok;
    char *name, *type, *title;
    PG_device *dev;
    out_device *out;
    object *o;

    o = SS_null;

    name  = NULL;
    type  = NULL;
    title = NULL;
    SS_args(si, argl,
            G_STRING_I, &name,
            G_STRING_I, &type,
            G_STRING_I, &title,
            0);

/* NOTE: this logic is a little bit strange
 * why should making a device be related to any of the output devices?
 */
    ok = TRUE;
    if (title == NULL)
       {for (i = 0; i < N_OUTPUT_DEVICES; i++)
	    {out = SX_get_device(i);
	     if (out->exist && out->active)
	        {if (strcmp(out->dupp, name) == 0)
		    {title = out->fname;
		     type  = out->type;
		     ok    = FALSE;
		     break;};};};

        if (title == NULL)
           SS_error(si, "NO TITLE SPECIFIED - _SXI_MAKE_DEVICE", SS_null);};
 
    dev = PG_make_device(name, type, title);
    if (dev != NULL)
       {dev->background_color_white = SX_gs.background_color_white;

	o = SX_make_pg_device(si, dev);};

    CFREE(name);
    if (ok == TRUE)
       {CFREE(type);
	CFREE(title);};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DEVICE_PROPS - return a list of device properties: NAME, TYPE, and
 *                   - TITLE
 */

static object *_SXI_device_props(SS_psides *si, object *argl)
   {PG_device *dev;
    object *rv;

    rv  = SS_null;
    dev = NULL;
    SS_args(si, argl,
            G_PG_DEVICE_I, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_DEVICE_PROPS", SS_null);

    else
       rv = SS_make_list(si, G_STRING_I, dev->name,
			 G_STRING_I, dev->type,
			 G_STRING_I, dev->title,
			 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_REF_MAP - get the particular mapping referred to 
 *                - return SS_null if the index is too large
 *                - for this file
 */

object *SX_get_ref_map(SS_psides *si, SX_file *po, int indx)
   {char type;
    object *argl, *ret;
    SX_menu_item *mi;

    ret = SS_null;

    mi = _SX_get_menu_item(si, po, indx-1);
    if (mi != NULL)
       {type = mi->type[3];
	switch (type)
	   {case 'i' :
	         argl = SS_make_list(si, G_SX_FILE_I, po,
				     G_INT_I, &indx,
				     0);
		 ret = _SXI_pdbdata_image(si, argl);
		 SS_gc(si, argl);
		 break;

	    case 'm' :
            default  :
	         argl = SS_make_list(si, G_SX_FILE_I, po,
				     G_INT_I, &indx,
				     0);
		 ret = _SXI_pdbdata_graph(si, argl);
		 SS_gc(si, argl);
		 break;};};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_EXPOSE_EVENT_HANDLER - handle expose events  */

void SX_expose_event_handler(PG_device *dev, PG_event *ev)
   {

    SX_motion_event_handler(dev, ev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MOTION_EVENT_HANDLER - handle motion events  */

void SX_motion_event_handler(PG_device *dev, PG_event *ev)
   {

    if (SX_gs.show_mouse_location)
       PG_print_pointer_location(dev,
                                 SX_gs.show_mouse_x[0],
                                 SX_gs.show_mouse_x[1],
                                 TRUE);
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_UPDATE_EVENT_HANDLER - handle update events  */

void SX_update_event_handler(PG_device *dev, PG_event *ev)
   {int min_dim, ncol;
    int dx[PG_SPACEDM];

    if (dev != NULL)
       {PG_make_device_current(dev);

	SX_gs.window_dx_P[0] = PG_window_width(dev);
	SX_gs.window_dx_P[1] = PG_window_height(dev);
	SX_gs.window_P[0]    = dev->g.hwin[0];
	SX_gs.window_P[1]    = dev->g.hwin[2];

	PG_query_screen_n(dev, dx, &ncol);
	min_dim = min(dx[0], dx[1]);

	SX_gs.window_dx[0] = SX_gs.window_dx_P[0]/min_dim;
	SX_gs.window_dx[1] = SX_gs.window_dx_P[1]/min_dim; 

	SX_gs.window_x[0]  = SX_gs.window_P[0]/min_dim;
	SX_gs.window_x[1]  = SX_gs.window_P[1]/min_dim;};

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
    SS_psides *si;

    si = SC_GET_CONTEXT(SX_mouse_event_handler);

    if (dev != NULL)
       {PG_make_device_current(dev);

        type = PG_EVENT_TYPE(*ev);

	PG_mouse_event_info(dev, ev, evx, &button, &qual);

	for (id = 0; id < 2; id++)
	    wc[id] = evx[id];

	PG_trans_point(dev, 2, PIXELC, wc, WORLDC, wc);

	loc = SS_make_list(si, G_DOUBLE_I, &wc[0],
			   G_DOUBLE_I, &wc[1],
			   0);

	fnc = _SS_lk_var_valc(si, "handle-mouse-event", si->env);
        if (SS_procedurep(fnc))
	   SS_call_scheme(si, "handle-mouse-event",
                          G_INT_I, &type,
			  G_OBJECT_I, loc,
			  G_ENUM_I, &button,
			  G_INT_I, &qual,
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

static object *_SXI_open_device(SS_psides *si, object *argl)
   {PG_device *dev;
    double xf, yf, dxf, dyf;

    dev = NULL;
    xf  = yf  = 0.0;
    dxf = dyf = 0.0;
    SS_args(si, argl,
            G_PG_DEVICE_I, &dev,
            G_DOUBLE_I, &xf,
            G_DOUBLE_I, &yf,
            G_DOUBLE_I, &dxf,
            G_DOUBLE_I, &dyf,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_OPEN_DEVICE", SS_null);

    else

/* PG_open_device set dev->hard_copy_device so we can't query it here */
       {if (!SX_gs.gr_mode &&
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

	SS_set_put_line(si, SX_fprintf);
	SS_set_put_string(si, SX_fputs);
	SC_set_get_line(PG_wind_fgets);

	PG_make_device_current(dev);
	PG_set_viewspace(dev, 2, WORLDC, NULL);
	PG_release_current_device(dev);

	PG_set_default_event_handler(dev, SX_default_event_handler);
	PG_set_mouse_down_event_handler(dev, SX_mouse_event_handler);
	PG_set_mouse_up_event_handler(dev, SX_mouse_event_handler);
	PG_set_motion_event_handler(dev, SX_motion_event_handler);
	PG_set_expose_event_handler(dev, SX_expose_event_handler);
	PG_set_update_event_handler(dev, SX_update_event_handler);

	SC_REGISTER_CONTEXT(SX_mouse_event_handler,   SS_psides, si);
	SC_REGISTER_CONTEXT(SX_motion_event_handler,  SS_psides, si);
	SC_REGISTER_CONTEXT(SX_expose_event_handler,  SS_psides, si);
	SC_REGISTER_CONTEXT(SX_update_event_handler,  SS_psides, si);
	SC_REGISTER_CONTEXT(SX_default_event_handler, SS_psides, si);};

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_NEXT_COLOR - return the next available color */

int SX_next_color(PG_device *dev)
   {int clr, rv;

    PG_get_attrs_glb(TRUE, "line-color", &clr, NULL);

    if (clr != -1)
       {if (dev == NULL)
           rv = _PG_trans_color(PG_gs.console, clr);
        else
           rv = _PG_trans_color(dev, clr);}

    else
       {_SX.color = max(_SX.color + 1, 1);
        if (dev == NULL)
           rv = _PG_trans_color(PG_gs.console, _SX.color);
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

static object *_SXI_make_pgs_graph(SS_psides *si, object *argl)
   {int n, sid, color, style, clr;
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

    color     = BLACK;
    style     = LINE_SOLID;
    centering = U_CENT;
    width     = 0.0;
    arr       = NULL;
    name      = NULL;
    if (++_SX.igg > 'Z')
       _SX.igg = 'A';

    SS_args(si, argl,
            G_PM_SET_I, &domain,
            G_PM_SET_I, &range,
            G_STRING_I, &name,
            G_ENUM_I, &centering,
            G_INT_I, &color,
            G_DOUBLE_I, &width,
            G_INT_I, &style,
	    G_C_ARRAY_I, &arr,
            0);

    if (centering == U_CENT)
       centering = (domain->n_elements == range->n_elements) ? N_CENT : Z_CENT;

    if (name == NULL)
       lbl = SC_dsnprintf(FALSE, "%s->%s", domain->name, range->name);
    else
       lbl = SC_dsnprintf(FALSE, name);

    CFREE(name);

/* build the graph
 * NOTE: since the addition of PG_graph info member there will have to
 * be work here to reconnect the rendering attributes
 */
    clr  = _SX.igg % 14 + 1;
    info = PG_set_line_info(NULL, PLOT_CARTESIAN, CARTESIAN_2D,
			    LINE_SOLID, FALSE, 0, clr, 0, 0.0);
    g = PG_make_graph_from_sets(lbl, domain, range, centering,
                                G_PCONS_P_S, info, _SX.igg, NULL);

    if (arr == NULL)
       {emap = CMAKE_N(char, domain->n_elements);
        memset(emap, 1, domain->n_elements);}
    else
       {PM_ARRAY_CONTENTS(arr, void, n, type, d);
	sid  = SC_type_id(type, FALSE);
        emap = SC_convert_id(G_CHAR_I, NULL, 0, 1, sid, d, 0, 1, n, FALSE);};

    PG_set_attrs_mapping(g->f,
			 "EXISTENCE", G_CHAR_I, TRUE, emap,
			 NULL);

    o = SX_make_pg_graph(si, g);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_SET_LIMITS - add/modify the specified plotting limits for the
 *                - given graph
 */

static void _SX_set_limits(SS_psides *si, PM_set *s, object *argl)
   {int i, nd, nset;
    double *extr, *pe, xmn, xmx;

    if (s != NULL)
       {if (SS_nullobjp(argl))
	   extr = NULL;

        else
	   {nd   = s->dimension_elem;
	    extr = CMAKE_N(double, 2*nd);
	    pe   = extr;
	    for (i = 0; i < nd; i++, argl = SS_cddr(si, argl))
	        {if (SS_nullobjp(argl))
		    {CFREE(extr);
		     break;};

		 nset = SS_args(si, argl,
				G_DOUBLE_I, &xmn,
				G_DOUBLE_I, &xmx,
				0);

		 if (nset < 2)
		    SS_error(si,
			       "INSUFFICIENT LIMITS SPECS - _SX_SET_LIMITS",
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

static object *_SX_get_limits(SS_psides *si, PM_set *s)
   {int i, nd;
    double *limits;
    object *obj, *lst;

    lst    = SS_null;
    nd     = 2*(s->dimension_elem);
    limits = PM_get_limits(s);
    
    if (limits != NULL)
       {limits = PM_array_real(s->es_type, limits, nd, NULL);

	for (i = 0; i < nd; i++)
	    {obj = SS_mk_float(si, *limits++);
	     lst = SS_mk_cons(si, obj, lst);};
    
	CFREE(limits);

	if (lst != SS_null)
	   lst = SS_reverse(si, lst);};

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_GET_EXTREMA - return the extrema of the given set in a form suitable
 *                 - for _SX_SET_LIMITS
 */

static object *_SX_get_extrema(SS_psides *si, PM_set *s)
   {int i, nd;
    double *extr;
    object *obj, *lst;

    nd = 2*(s->dimension_elem);
    
    extr = PM_array_real(s->element_type, s->extrema, nd, NULL);

    lst = SS_null;
    for (i = 0; i < nd; i++)
        {obj = SS_mk_float(si, *extr++);
	 lst = SS_mk_cons(si, obj, lst);};
    
    CFREE(extr);

    if (lst != SS_null)
       lst = SS_reverse(si, lst);

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DRAW_DOMAIN - main Scheme level domain plot control routine */

static object *_SXI_draw_domain(SS_psides *si, object *argl)
   {PG_device *dev;
    PM_set *data, *nxt, *p;
    PG_rendering pty;
    object *obj, *extr;

    dev = NULL;

    if (SS_consp(argl))
       SX_GET_DEVICE_FROM_LIST(si, dev, argl,
			       "BAD DEVICE - _SXI_DRAW_DOMAIN");

    if (dev == NULL)
       return(SS_null);

/* get the list of sets */
    data = nxt = NULL;
    while (SS_consp(argl))
       {obj = SS_car(si, argl);
        if (SX_SETP(obj))
           {if (data == NULL)
               data = nxt = SS_GET(PM_set, obj);
            else
               {nxt->next = SS_GET(PM_set, obj);
                nxt = nxt->next;};}
        else
           break;
        argl = SS_cdr(si, argl);};

    pty  = PLOT_WIRE_MESH;
    extr = SS_null;
    SS_args(si, argl,
            G_ENUM_I, &pty,
	    G_OBJECT_I, &extr,
	    0);

    if (!SS_nullobjp(extr))
       _SX_set_limits(si, data, extr);

    if (data == NULL)
       SS_error(si, "BAD DOMAIN - _SXI_DRAW_DOMAIN", obj);

    else
       {data->info = PG_set_plot_type((pcons *) data->info, pty, CARTESIAN_2D);

	PG_set_attrs_set(data,
			 "THETA", G_DOUBLE_I, FALSE, SX_gs.view_angle[0],
			 "PHI",   G_DOUBLE_I, FALSE, SX_gs.view_angle[1],
			 "CHI",   G_DOUBLE_I, FALSE, SX_gs.view_angle[2],
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
         g->info   = line_info;

         if (pty == PLOT_SCATTER)
	    PG_set_attrs_graph(g,
			       "SCATTER", G_INT_I, FALSE, 1,
			       NULL);

         else if (pty == PLOT_HISTOGRAM)
	    PG_set_attrs_graph(g,
			       "HIST-START", G_INT_I, FALSE, start,
			       NULL);

         g->render = PG_curve_plot;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DRAW_PLOT - main Scheme level plot control routine */

static object *_SXI_draw_plot(SS_psides *si, object *argl)
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
       SX_GET_DEVICE_FROM_LIST(si, dev, argl,
			       "BAD DEVICE - _SXI_DRAW_PLOT");

    if ((dev == NULL) || (!SX_OK_TO_DRAW(dev)))
       return(SS_f);

/* get the list of graphs */
    data = nxt = NULL;
    for ( ; SS_consp(argl); argl = SS_cdr(si, argl))
        {obj = SS_car(si, argl);
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
       SS_error(si, "BAD GRAPH - _SXI_DRAW_PLOT", obj);

    if (_SX_grotrian_graphp(data))
       {PG_grotrian_plot(dev, data);
        return(SS_t);};

    f          = data->f;
    domain     = f->domain;
    range      = f->range;
    domain_dim = (domain == NULL) ? 0 : domain->dimension_elem;
    range_dim  = (range == NULL) ? 0 : range->dimension_elem;
    apty       = PLOT_NONE;
    pty        = data->rendering;
    info       = (pcons *) data->info;

    if (SS_consp(argl))
       {SX_GET_INTEGER_FROM_LIST(si, apty, argl,
				 "BAD PLOT TYPE - _SXI_DRAW_PLOT");

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
			     {SX_GET_INTEGER_FROM_LIST(si, *hsts, argl,
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
			      SX_GET_INTEGER_FROM_LIST(si, *nlev, argl,
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
    data->info      = info;
/*    dev->data_id    = TRUE; */

    if ((pty == PLOT_SURFACE) ||
        (pty == PLOT_SCATTER) ||
        (pty == PLOT_CONTOUR) ||
        (pty == PLOT_WIRE_MESH) ||
        (pty == PLOT_MESH))
       {PG_set_attrs_graph(data,
			   "THETA", G_DOUBLE_I,  FALSE, SX_gs.view_angle[0],
			   "PHI",   G_DOUBLE_I,  FALSE, SX_gs.view_angle[1],
			   "CHI",   G_DOUBLE_I,  FALSE, SX_gs.view_angle[2],
			   NULL);
	PG_set_attrs_mapping(data->f,
			     "THETA", G_DOUBLE_I,  FALSE, SX_gs.view_angle[0],
			     "PHI",   G_DOUBLE_I,  FALSE, SX_gs.view_angle[1],
			     "CHI",   G_DOUBLE_I,  FALSE, SX_gs.view_angle[2],
			     NULL);};

    PG_fset_axis_log_scale(dev, 2, SX_gs.log_scale);
    PG_turn_grid(dev, SX_gs.grid);

    data->rendering = pty;

    SS_set_put_line(si, SX_fprintf);
    SS_set_put_string(si, SX_fputs);

    if ((domain != NULL) && (domain->info_type != NULL))
       {if (strcmp(domain->info_type, G_PCONS_P_S) == 0)
	  dev->autodomain = (SC_assoc_entry((pcons *) domain->info,
					    "LIMITS") == NULL);};

    if ((range != NULL) && (range->info_type != NULL))
       {if (strcmp(range->info_type, G_PCONS_P_S) == 0)
	  dev->autorange  = (SC_assoc_entry((pcons *) range->info,
					    "LIMITS") == NULL);};

    if (data->f->next != NULL)
       {int refm;
	pcons *oinf, *ninf;

	oinf = (pcons *) data->f->map;
	ninf = SC_copy_alist(oinf);
	ninf = SC_append_alist(ninf, SC_copy_alist(info));
	data->f->map = ninf;

	PG_get_attrs_glb(TRUE,
			 "ref-mesh", &refm,
			 NULL);

	if (pty == PLOT_SURFACE)
	   PG_set_attrs_mapping(data->f,
				"DRAW-MESH", G_INT_I, FALSE, refm,
				NULL);

	PG_draw_picture(dev, data->f, pty,
			TRUE, dev->WHITE, LINE_SOLID, 1.2,
			refm, dev->WHITE, LINE_SOLID, 0.0);

	SC_free_alist(ninf, 1);
	data->f->map = oinf;}

    else if (data->render != NULL)
       data->render(dev, data);

    else
       PG_draw_graph(dev, data);

    PG_draw_interface_objects(dev);

    SS_set_put_line(si, SS_printf);
    SS_set_put_string(si, SS_fputs);

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

static object *_SXI_make_image(SS_psides *si, object *argl)
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
    SS_args(si, argl,
            G_C_ARRAY_I, &arr,
            G_INT_I, &w,
            G_INT_I, &h,
            G_STRING_I, &name,
            G_DOUBLE_I, &dbx[0],
            G_DOUBLE_I, &dbx[1],
            G_DOUBLE_I, &dbx[2],
            G_DOUBLE_I, &dbx[3],
            G_DOUBLE_I, &rbx[0],
            G_DOUBLE_I, &rbx[1],
            0);

    if (name == NULL)
       name = "Image";

    if (arr == NULL)
       SS_error(si, "BAD DATA - _SXI_MAKE_IMAGE", argl);

    else
       {im = PG_make_image_n(name, arr->type, arr->data,
			     2, WORLDC, dbx, rbx, w, h, 8, NULL);
	rv = SX_make_pg_image(si, im);};

    CFREE(name);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_BUILD_IMAGE - crack the arg list, build, and return an image object
 *                  - Usage: (pg-build-image <dev> <data> <w> <h>
 *                  -                         [<name> <xmin> <xmax>
 *                  -                          <ymin> <ymax>
 *                  -                          <zmin> <zmax>])
 */

static object *_SXI_build_image(SS_psides *si, object *argl)
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
    SS_args(si, argl,
            G_PG_DEVICE_I, &dev,
            G_C_ARRAY_I, &arr,
            G_INT_I, &w,
            G_INT_I, &h,
            G_STRING_I, &name,
            G_DOUBLE_I, &dbx[0],
            G_DOUBLE_I, &dbx[1],
            G_DOUBLE_I, &dbx[2],
            G_DOUBLE_I, &dbx[3],
            G_DOUBLE_I, &rbx[0],
            G_DOUBLE_I, &rbx[1],
            0);

    if (name == NULL)
       name = "Image";

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_BUILD_IMAGE", argl);

    if (arr == NULL)
       SS_error(si, "BAD DATA - _SXI_BUILD_IMAGE", argl);

    else
       {im = PG_build_image(dev, name, arr->type, arr->data,
			    w, h, 2, WORLDC, dbx, rbx);
	rv = SX_make_pg_image(si, im);};

    CFREE(name);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DRAW_IMAGE - Scheme level image plot control routine */

static object *_SXI_draw_image(SS_psides *si, object *argl)
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

    SS_args(si, argl,
            G_PG_DEVICE_I, &dev,
            G_PG_IMAGE_I, &image,
            G_DOUBLE_I, &xmn,
            G_DOUBLE_I, &xmx,
            G_DOUBLE_I, &ymn,
            G_DOUBLE_I, &ymx,
            0);

    if (!SX_OK_TO_DRAW(dev))
       return(SS_f);

/* if viewport box has been specified, create an assoc list and pass it in */
    if ((xmn < xmx) && (ymn < ymx))
       {vp = TRUE;
        pv = CMAKE_N(double, 4);
        pv[0] = xmn;
        pv[1] = xmx;
        pv[2] = ymn;
        pv[3] = ymx;
	PG_set_attrs_alist(alist,
			   "VIEW-PORT", G_DOUBLE_I, TRUE, pv,
			   NULL);};
            
    PG_draw_image(dev, image, image->label, alist);

    if (vp == TRUE)
       {alist = PG_rem_attrs_alist(alist, "VIEW-PORT", NULL);
        CFREE(pv);}    

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_SET_ATTR_ALIST - set an attribute on an alist */

pcons *SX_set_attr_alist(SS_psides *si, pcons *inf,
			 const char *name, const char *type, object *val)
   {int id, sid;
    void *v;
    object *obj;
    C_array *arr;

    if (strcmp(type, "nil") == 0)
       {if (inf != NULL)
	   inf = SC_rem_alist(inf, name);
	return(inf);};

    id = SC_type_id(type, FALSE);

    v = NULL;

    if (SS_consp(val))
       {char dtype[MAXLINE], stype[MAXLINE];

	obj = SS_null;
	SS_assign(si, obj, SX_list_array(si, val));
	SS_args(si, obj,
		G_C_ARRAY_I, &arr,
		0);

	SC_strncpy(dtype, MAXLINE, type, -1);
	SC_strncpy(stype, MAXLINE, arr->type, -1);
	SC_dereference(dtype);
	SC_dereference(stype);
	sid = SC_type_id(stype, FALSE);

	v = SC_convert_id(sid, NULL, 0, 1,
			  sid, arr->data, 0, 1, arr->length, TRUE);

	PM_rel_array(arr);

	SC_mark(v, 1);
	SS_gc(si, obj);
	SC_mark(v, -1);}

    else if (id == G_INT_P_I)
       {v = CMAKE(int);
        SS_args(si, val,
		G_INT_I, v,
		0);}

    else if (id == G_DOUBLE_P_I)
       {v = CMAKE(double);
        SS_args(si, val,
		G_DOUBLE_I, v,
		0);}

    else if (id == G_STRING_I)
       {SS_args(si, val,
		G_STRING_I, &v,
		0);}

    else
       SS_error(si, "CAN'T HANDLE TYPE - SX_SET_ATTR_ALIST", val);

    inf = SC_change_alist(inf, name, type, v);

    return(inf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SET_ATTR_GRAPH - set an attribute of a PG_graph object
 *                     - usage: (pg-set-graph-attribute! <graph>
 *                     -                         <name> <type> <value>)
 */

static object *_SXI_set_attr_graph(SS_psides *si, object *argl)
   {PG_graph *g;
    char *name, *type;
    object *val;
    pcons *inf;

    g    = NULL;
    name = NULL;
    type = NULL;
    val  = SS_null;
    SS_args(si, argl,
            G_PG_GRAPH_I, &g,
            G_STRING_I, &name,
            G_STRING_I, &type,
            G_OBJECT_I, &val,
            0);

    if ((g == NULL) || (name == NULL) || (type == NULL))
       SS_error(si, "INSUFFICIENT ARGUMENTS - _SXI_SET_ATTR_GRAPH", argl);

    else

/* get the current list */
       {if (g->info_type != NULL)
	   {if (strcmp(g->info_type, G_PCONS_P_S) == 0)
	       inf = (pcons *) g->info;
	    else
	       inf = NULL;}
        else
	   inf = NULL;

	g->info      = SX_set_attr_alist(si, inf, name, type, val);
	g->info_type = G_PCONS_P_S;};

    CFREE(name);
    CFREE(type);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MAP_RAN_EXTREMA - return the overall extrema of the given, possibly linked,
 *                     - mapping in a form suitable for _SX_SET_LIMITS
 */

static object *_SX_map_ran_extrema(SS_psides *si, PM_mapping *m)
   {int i, nd;
    double extr[20], rextr[20];
    PM_set *s;
    PM_mapping *f;
    object *obj, *lst;

    nd = 0;

/* NOTE: allow up to 10 dimensional ranges */

#pragma omp parallel for
    for (i = 0; i < 10; i++)
        {rextr[2*i]   =  HUGE_REAL;
         rextr[2*i+1] = -HUGE_REAL;};

    for (f = m; f != NULL; f = f->next)
        {s = f->range;
	 if (s != NULL)
	    {nd = 2*(s->dimension_elem);
    
	     PM_array_real(s->element_type, s->extrema, nd, extr);

#pragma omp parallel for
	     for (i = 0; i < (nd/2); i++)
	         {rextr[2*i]   = min(rextr[2*i], extr[2*i]);
		  rextr[2*i+1] = max(rextr[2*i+1], extr[2*i+1]);};};};

    lst = SS_null;
    for (i = 0; i < nd; i++)
        {obj = SS_mk_float(si, rextr[i]);
	 lst = SS_mk_cons(si, obj, lst);};
    
    if (lst != SS_null)
       lst = SS_reverse(si, lst);

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MAP_DOM_EXTREMA - return the overall extrema of
 *                     - the given, possibly linked, mapping
 *                     - in a form suitable for _SX_SET_LIMITS
 */

static object *_SX_map_dom_extrema(SS_psides *si, PM_mapping *m)
   {int i, id, n, nd;
    double *extr, *rextr;
    PM_set *s;
    PM_mapping *f;
    object *obj, *lst;

    if (m == NULL)
       return(SS_null);

    nd = m->domain->dimension;
    n  = 2*nd;

    rextr = CMAKE_N(double, n);
    extr  = CMAKE_N(double, n);

#pragma omp parallel for
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

#pragma omp parallel for
	     for (id = 0; id < nd; id++)
	         {rextr[2*id]   = min(rextr[2*id], extr[2*id]);
		  rextr[2*id+1] = max(rextr[2*id+1], extr[2*id+1]);};};};

    lst = SS_null;
    for (i = 0; i < n; i++)
        {obj = SS_mk_float(si, rextr[i]);
	 lst = SS_mk_cons(si, obj, lst);};
    
    if (lst != SS_null)
       lst = SS_reverse(si, lst);

    CFREE(rextr);
    CFREE(extr);

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_IM_DOM_EXTREMA - return the extrema of the given image domain in a form
 *                    - suitable for _SX_SET_LIMITS
 */

static object *_SX_im_dom_extrema(SS_psides *si, PG_image *im)
   {object *obj, *lst;

    lst = SS_null;
    obj = SS_mk_float(si, im->ymax);
    lst = SS_mk_cons(si, obj, lst);
    obj = SS_mk_float(si, im->ymin);
    lst = SS_mk_cons(si, obj, lst);
    obj = SS_mk_float(si, im->xmax);
    lst = SS_mk_cons(si, obj, lst);
    obj = SS_mk_float(si, im->xmin);
    lst = SS_mk_cons(si, obj, lst);

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_IM_RAN_EXTREMA - return the extrema of the given image range in a form
 *                    - suitable for _SX_SET_LIMITS
 */

static object *_SX_im_ran_extrema(SS_psides *si, PG_image *im)
   {object *obj, *lst;

    lst = SS_null;
    obj = SS_mk_float(si, im->zmax);
    lst = SS_mk_cons(si, obj, lst);
    obj = SS_mk_float(si, im->zmin);
    lst = SS_mk_cons(si, obj, lst);

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SET_DOM_LIMITS - set the domain plotting limits for the given graph
 *                     - form:
 *                     -   (pg-set-domain-limits! <graph> (<x1_min> <x1_max> .... ))
 */

static object *_SXI_set_dom_limits(SS_psides *si, object *argl)
   {PM_set *s;
    object *obj;

    s = NULL;
    obj = SS_car(si, argl);
    if (SX_SETP(obj))
       s = SS_GET(PM_set, obj);
    else if (SX_MAPPINGP(obj))
       s = SS_GET(PM_mapping, obj)->domain;
    else if (SX_GRAPHP(obj))
       s = SS_GET(PG_graph, obj)->f->domain;
    else
       SS_error(si, "NO SET IMPLIED - _SXI_SET_DOM_LIMITS", obj);

    _SX_set_limits(si, s, SS_cadr(si, argl));

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SET_RAN_LIMITS - set the range plotting limits for the given graph
 *                     - form:
 *                     -   (pg-set-range-limits! <graph> (<x1_min> <x1_max> .... ))
 */

static object *_SXI_set_ran_limits(SS_psides *si, object *argl)
   {PM_set *s;
    object *obj;

    s = NULL;
    obj = SS_car(si, argl);

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
       SS_error(si, "NO SET IMPLIED - _SXI_SET_RAN_LIMITS", obj);

    _SX_set_limits(si, s, SS_cadr(si, argl));

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DOM_EXTREMA - return the domain extrema for the given drawable form:
 *                  -   (pg-domain-extrema <drawable>)
 */

static object *_SXI_dom_extrema(SS_psides *si, object *obj)
   {PM_set *s;
    PM_mapping *m;
    object *rv;

    s  = NULL;
    rv = SS_null;

    if (SX_SETP(obj))
       s = SS_GET(PM_set, obj);

    else if (SX_MAPPINGP(obj))
       {m  = SS_GET(PM_mapping, obj);
        rv = _SX_map_dom_extrema(si, m);}

    else if (SX_GRAPHP(obj))
       {m  = SS_GET(PG_graph, obj)->f;
        rv = _SX_map_dom_extrema(si, m);}

    else if (SX_IMAGEP(obj))
       rv = _SX_im_dom_extrema(si, SS_GET(PG_image, obj));

    else
       SS_error(si, "NO SET IMPLIED - _SXI_DOM_EXTREMA", obj);

    if (s != NULL)
       rv = _SX_get_extrema(si, s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DOM_LIMITS - return the domain plotting LIMITS if any
 *                 - for the given drawable form:
 *                 -   (pg-domain-limits <drawable>)
 */

static object *_SXI_dom_limits(SS_psides *si, object *argl)
   {PM_set *s;
    object *obj, *rv;

    rv  = SS_null;
    s   = NULL;
    obj = SS_car(si, argl);

    if (SX_SETP(obj))
       s = SS_GET(PM_set, obj);

    else if (SX_MAPPINGP(obj))
       s = SS_GET(PM_mapping, obj)->domain;

    else if (SX_GRAPHP(obj))
       s = SS_GET(PG_graph, obj)->f->domain;

    else if (SX_IMAGEP(obj))
       s = NULL;

    else
       SS_error(si, "NO SET IMPLIED - _SXI_DOM_LIMITS", obj);

    if (s != NULL)
       rv = _SX_get_limits(si, s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_RAN_EXTREMA - return the range extrema for the given drawable form:
 *                  -   (pg-range-extrema <drawable>)
 */

static object *_SXI_ran_extrema(SS_psides *si, object *obj)
   {PM_set *s;
    PM_mapping *m;
    object *rv;

    s  = NULL;
    rv = SS_null;

    if (SX_SETP(obj))
       {s  = SS_GET(PM_set, obj);
	rv = _SX_get_extrema(si, s);}

    else if (SX_MAPPINGP(obj))
       {m  = SS_GET(PM_mapping, obj);
        rv = _SX_map_ran_extrema(si, m);}

    else if (SX_GRAPHP(obj))
       {m  = SS_GET(PG_graph, obj)->f;
        rv = _SX_map_ran_extrema(si, m);}

    else if (SX_IMAGEP(obj))
       rv = _SX_im_ran_extrema(si, SS_GET(PG_image, obj));

    else
       SS_error(si, "NO SET IMPLIED - _SXI_RAN_EXTREMA", obj);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_RAN_LIMITS - return the range plotting LIMITS if any
 *                 - for the given drawable form:
 *                 -   (pg-range-extrema <drawable>)
 */

static object *_SXI_ran_limits(SS_psides *si, object *argl)
   {PM_set *s;
    object *obj, *rv;

    s   = NULL;
    rv  = SS_null;
    obj = SS_car(si, argl);

    if (SX_SETP(obj))
       s = SS_GET(PM_set, obj);
    else if (SX_MAPPINGP(obj))
       s = SS_GET(PM_mapping, obj)->range;
    else if (SX_GRAPHP(obj))
       s = SS_GET(PG_graph, obj)->f->range;
    else if (SX_IMAGEP(obj))
       s = NULL;
    else
       SS_error(si, "NO SET IMPLIED - _SXI_RAN_LIMITS", obj);

    if (s != NULL)
       rv = _SX_get_limits(si, s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SET_LABEL - set the label of the drawable's mapping 
 *                - usage: (pg-set-label! <drawable> <label>)
 */

static object *_SXI_set_label(SS_psides *si, object *argl)
   {char *label;
    object *obj;

    obj   = NULL;
    label = NULL;
    SS_args(si, argl,
            G_OBJECT_I, &obj,
            G_STRING_I, &label,
            0);

    if ((obj == NULL) || (label == NULL))
       SS_error(si, "INSUFFICIENT ARGUMENTS - _SXI_SET_LABEL", argl);

    else if (SX_SETP(obj))
       {PM_set *s;

        s = SS_GET(PM_set, obj);
	CFREE(s->name);
	s->name = label;}

    else if (SX_MAPPINGP(obj))
       {PM_mapping *s;

	s = SS_GET(PM_mapping, obj);
	CFREE(s->name);
	s->name = label;}

    else if (SX_GRAPHP(obj))
       {PM_mapping *s;

	s = SS_GET(PG_graph, obj)->f;
	CFREE(s->name);
	s->name = label;}

    else if (SX_IMAGEP(obj))
       {PG_image *s;

	s = SS_GET(PG_image, obj);
	CFREE(s->label);
	s->label = label;}

    else
       SS_error(si, "BAD DRAWABLE - _SXI_SET_LABEL", obj);

/* NOTE: this is a problematic fix for a bug that only surfaced
 * after more than 10 years
 * the problematic part is the reference count
 * the object print_name never was marked - tough to figure too
 */
    obj->print_name = label;

    obj = SS_car(si, argl);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GET_LABEL - return the label of the drawable's mapping 
 *                - usage: (pg-get-label <drawable>)
 */

static object *_SXI_get_label(SS_psides *si, object *obj)
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
       SS_error(si, "BAD DRAWABLE - _SXI_GET_LABEL", obj);

    rv = SS_mk_string(si, label);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_SET_INFO - get/set information in the PM_set object given */

static object *_SX_set_info(SS_psides *si, object *obj,
			    const char *name, object *val)
   {long v;
    char *t;
    PM_set *s, *u;
    object *ret;

    ret = SS_f;

    s = SS_GET(PM_set, obj);
    if (strcmp(name, "name") == 0)
       {t = s->name;
	if (val != NULL)
	   {SS_args(si, val, G_STRING_I, &t);
	    s->name = t;};

	ret = SS_mk_string(si, t);}

    else if (strcmp(name, "element-type") == 0)
       {t = s->element_type;
	if (val != NULL)
	   {SS_args(si, val, G_STRING_I, &t);
	    s->element_type = t;};

	ret = SS_mk_string(si, t);}

    else if (strcmp(name, "es-type") == 0)
       {t = s->es_type;
	if (val != NULL)
	   {SS_args(si, val, G_STRING_I, &t);
	    s->es_type = t;};

	ret = SS_mk_string(si, t);}

    else if (strcmp(name, "symmetry-type") == 0)
       {t = s->symmetry_type;
	if (val != NULL)
	   {SS_args(si, val, G_STRING_I, &t);
	    s->symmetry_type = t;};

	ret = SS_mk_string(si, t);}

    else if (strcmp(name, "topology-type") == 0)
       {t = s->topology_type;
	if (val != NULL)
	   {SS_args(si, val, G_STRING_I, &t);
	    s->topology_type = t;};

	ret = SS_mk_string(si, t);}

    else if (strcmp(name, "info-type") == 0)
       {t = s->info_type;
	if (val != NULL)
	   {SS_args(si, val, G_STRING_I, &t);
	    s->info_type = t;};

	ret = SS_mk_string(si, t);}

    else if (strcmp(name, "dimension") == 0)
       {v = s->dimension;
	if (val != NULL)
           {SS_args(si, val, G_INT_I, &v);
	    s->dimension = v;};

	ret = SS_mk_integer(si, v);}

    else if (strcmp(name, "dimension-elem") == 0)
       {v = s->dimension_elem;
	if (val != NULL)
           {SS_args(si, val, G_INT_I, &v);
	    s->dimension_elem = v;};

	ret = SS_mk_integer(si, v);}

    else if (strcmp(name, "n-elements") == 0)
       {v = s->n_elements;
	if (val != NULL)
           {SS_args(si, val, G_INT_I, &v);
	    s->n_elements = v;};

	ret = SS_mk_integer(si, v);}

    else if (strcmp(name, "next") == 0)
       {u = s->next;
	if (val != NULL)
	   {SS_args(si, val, G_PM_SET_I, &u);
	    s->next = u;};

	ret = SX_make_pm_set(si, u);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MAP_INFO - get/set information in the PM_mapping object given */

static object *_SX_map_info(SS_psides *si, object *obj,
			    const char *name, object *val)
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
	   {SS_args(si, val, G_STRING_I, &t);
	    f->name = t;};

	ret = SS_mk_string(si, t);}

    else if (strcmp(name, "category") == 0)
       {t = f->category;
	if (val != NULL)
	   {SS_args(si, val, G_STRING_I, &t);
	    f->category = t;};

	ret = SS_mk_string(si, t);}

    else if (strcmp(name, "domain") == 0)
       {s = f->domain;
	if (val != NULL)
	   {SS_args(si, val, G_PM_SET_I, &s);
	    f->domain = s;};

	ret = SX_make_pm_set(si, s);}

    else if (strcmp(name, "range") == 0)
       {s = f->range;
	if (val != NULL)
	   {SS_args(si, val, G_PM_SET_I, &s);
	    f->range = s;};

	ret = SX_make_pm_set(si, s);}

    else if (strcmp(name, "next") == 0)
       {g = f->next;
	if (val != NULL)
	   {SS_args(si, val, G_PM_MAPPING_I, &g);
	    f->next = g;};

	ret = SX_make_pm_mapping(si, f);}

    else if (strcmp(name, "file-type") == 0)
       {v = f->file_type;
	if (val != NULL)
           {SS_args(si, val, G_INT_I, &v);
	    f->file_type = v;};

	ret = SS_mk_integer(si, v);}

    else if (strcmp(name, "map-type") == 0)
       {t = f->map_type;
	if (val != NULL)
	   {SS_args(si, val, G_STRING_I, &t);
	    f->map_type = t;};

	ret = SS_mk_string(si, t);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _G_PG_GRAPH_INFO - get/set information in the PG_graph object given */

static object *_SX_graph_info(SS_psides *si, object *obj,
			      const char *name, object *val)
   {long v;
    const char *t;
    PG_graph *g, *h;
    PM_mapping *f;
    object *ret;

    ret = SS_f;

    g = SS_GET(PG_graph, obj);
    if (strcmp(name, "rendering") == 0)
       {v = g->rendering;
	if (val != NULL)
           {PG_rendering pty;

	    SS_args(si, val, G_ENUM_I, &pty);
	    g->rendering = pty;};

	ret = SS_mk_integer(si, v);}

    else if (strcmp(name, "identifier") == 0)
       {v = g->identifier;
	if (val != NULL)
           {SS_args(si, val, G_LONG_I, &v);
	    g->identifier = v;};

	ret = SS_mk_integer(si, v);}

    else if (strcmp(name, "mesh") == 0)
       {v = g->mesh;
	if (val != NULL)
           {SS_args(si, val, G_LONG_I, &v);
	    g->mesh = v;};

	ret = SS_mk_integer(si, v);}

    else if (strcmp(name, "next") == 0)
       {h = g->next;
	if (val != NULL)
           {SS_args(si, val, G_PG_GRAPH_I, &h);
	    g->next = h;};

	ret = SX_make_pg_graph(si, h);}

    else if (strcmp(name, "f") == 0)
       {f = g->f;
	if (val != NULL)
	   {SS_args(si, val, G_PM_MAPPING_I, &f);
	    g->f = f;};

	ret = SX_make_pm_mapping(si, f);}

    else if (strcmp(name, "info-type") == 0)
       {t = g->info_type;
	if (val != NULL)
	   {SS_args(si, val, G_STRING_I, &t);
	    g->info_type = t;};

	ret = SS_mk_string(si, t);}

    else if (strcmp(name, "use") == 0)
       {t = g->use;
	if (val != NULL)
	   {SS_args(si, val, G_STRING_I, &t);
	    g->use = t;};

	ret = SS_mk_string(si, t);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _G_PG_IMAGE_INFO - get/set information in the PG_image object given */

static object *_SX_image_info(SS_psides *si, object *obj,
			      const char *name, object *val)
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
           {SS_args(si, val, G_LONG_I, &v);
	    im->version_id = v;};

	ret = SS_mk_integer(si, v);}

    else if (strcmp(name, "kmax") == 0)
       {v = im->kmax;
	if (val != NULL)
           {SS_args(si, val, G_LONG_I, &v);
	    im->kmax = v;};

	ret = SS_mk_integer(si, v);}

    else if (strcmp(name, "lmax") == 0)
       {v = im->lmax;
	if (val != NULL)
           {SS_args(si, val, G_LONG_I, &v);
	    im->lmax = v;};

	ret = SS_mk_integer(si, v);}

    else if (strcmp(name, "size") == 0)
       {v = im->size;
	if (val != NULL)
           {SS_args(si, val, G_LONG_I, &v);
	    im->size = v;};

	ret = SS_mk_integer(si, v);}

    else if (strcmp(name, "bits-pixel") == 0)
       {v = im->bits_pixel;
	if (val != NULL)
           {SS_args(si, val, G_LONG_I, &v);
	    im->bits_pixel = v;};

	ret = SS_mk_integer(si, v);}

    else if (strcmp(name, "label") == 0)
       {t = im->label;
	if (val != NULL)
	   {SS_args(si, val, G_STRING_I, &t);
	    im->label = t;};

	ret = SS_mk_string(si, t);}

    else if (strcmp(name, "element-type") == 0)
       {t = im->element_type;
	if (val != NULL)
	   {SS_args(si, val, G_STRING_I, &t);
	    im->element_type = t;};

	ret = SS_mk_string(si, t);}

    else if (strcmp(name, "xmin") == 0)
       {d = im->xmin;
	if (val != NULL)
	   {SS_args(si, val, G_DOUBLE_I, &d);
	    im->xmin = d;};

	ret = SS_mk_float(si, d);}

    else if (strcmp(name, "xmax") == 0)
       {d = im->xmax;
	if (val != NULL)
	   {SS_args(si, val, G_DOUBLE_I, &d);
	    im->xmax = d;};

	ret = SS_mk_float(si, d);}

    else if (strcmp(name, "ymin") == 0)
       {d = im->ymin;
	if (val != NULL)
	   {SS_args(si, val, G_DOUBLE_I, &d);
	    im->ymin = d;};

	ret = SS_mk_float(si, d);}

    else if (strcmp(name, "ymax") == 0)
       {d = im->ymax;
	if (val != NULL)
	   {SS_args(si, val, G_DOUBLE_I, &d);
	    im->ymax = d;};

	ret = SS_mk_float(si, d);}

    else if (strcmp(name, "zmin") == 0)
       {d = im->zmin;
	if (val != NULL)
	   {SS_args(si, val, G_DOUBLE_I, &d);
	    im->zmin = d;};

	ret = SS_mk_float(si, d);}

    else if (strcmp(name, "zmax") == 0)
       {d = im->zmax;
	if (val != NULL)
	   {SS_args(si, val, G_DOUBLE_I, &d);
	    im->zmax = d;};

	ret = SS_mk_float(si, d);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DRAWABLE_INFO - get/set the drawable's information
 *                    - usage: (pg-drawable-info <obj> <name> [<value>])
 */

static object *_SXI_drawable_info(SS_psides *si, object *argl)
   {char *name;
    object *obj, *val, *ret;

    ret  = SS_null;
    obj  = NULL;
    name = NULL;
    val  = NULL;
    SS_args(si, argl,
            G_OBJECT_I, &obj,
            G_STRING_I, &name,
            G_OBJECT_I, &val,
            0);

    if (SX_SETP(obj))
       ret = _SX_set_info(si, obj, name, val);

    else if (SX_MAPPINGP(obj))
       ret = _SX_map_info(si, obj, name, val);

    else if (SX_GRAPHP(obj))
       ret = _SX_graph_info(si, obj, name, val);

    else if (SX_IMAGEP(obj))
       ret = _SX_image_info(si, obj, name, val);

    else
       SS_error(si, "BAD DRAWABLE - _SXI_DRAWABLE_INFO", obj);

    CFREE(name);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* SX_INSTALL_PGS_FUNCS - install the PGS extensions to Scheme */
 
void SX_install_pgs_funcs(SS_psides *si)
   {double *labpy;
    extern void _SX_install_pgs_primitives(SS_psides *si);

    labpy = PG_ptr_attr_glb("label-position-y");

    SS_install(si, "pdbdata->pg-graph",
               "Read a PGS graph object from a PDB file\nFORM (pdbdata->pg-graph <file> <name>)",
               SS_nargs,
               _SXI_pdbdata_graph, SS_PR_PROC);

    SS_install(si, "pdbdata->pg-image",
               "Read a PGS image object from a PDB file\nFORM (pdbdata->pg-image <file> <name>)",
               SS_nargs,
               _SXI_pdbdata_image, SS_PR_PROC);

    SS_install(si, "pg-build-image",
               "Build a PGS image object\nFORM (pg-build-image <data> <kmax> <lmax> [<name> <xmin> <xmax> <ymin> <ymax> <zmin> zmax>])",
               SS_nargs,
               _SXI_build_image, SS_PR_PROC);

    SS_install(si, "pg-def-graph-file",
               "Set up a PDB file to recieve PGS graph objects",
               SS_sargs,
               _SXI_def_file_graph, SS_PR_PROC);

    SS_install(si, "pg-device?",
               "Returns #t if the object is a PGS device, and #f otherwise",
               SS_sargs,
               _SXI_devicep, SS_PR_PROC);

    SS_install(si, "pg-device-attributes?",
               "Returns #t if the object is a set of PGS device attributes, and #f otherwise",
               SS_sargs,
               _SXI_dev_attributesp, SS_PR_PROC);

    SS_install(si, "pg-device-properties",
               "Return a list with NAME, TYPE, and TITLE of the given device",
               SS_nargs,
               _SXI_device_props, SS_PR_PROC);

    SS_install(si, "pg-domain-extrema",
               "Return the domain extrema\nFORM (pg-domain-extrema <graph>)",
               SS_sargs,
               _SXI_dom_extrema, SS_PR_PROC);

    SS_install(si, "pg-domain-limits",
               "Return the domain plotting limits\nFORM (pg-domain-limits <graph>)",
               SS_sargs,
               _SXI_dom_limits, SS_PR_PROC);

    SS_install(si, "pg-draw-domain",
               "Draws the mesh specified by a PML set object\nFORM (pg-draw-domain <device> <set>)",
               SS_nargs,
               _SXI_draw_domain, SS_PR_PROC);

    SS_install(si, "pg-draw-graph",
               "Draws a PGS graph object\nFORM (pg-draw-graph <device> <graph> <rendering> [...])",
               SS_nargs,
               _SXI_draw_plot, SS_PR_PROC);

    SS_install(si, "pg-draw-image",
               "Draws a PGS image object\nFORM (pg-draw-image <device> <image>)",
               SS_nargs,
               _SXI_draw_image, SS_PR_PROC);

    SS_install(si, "pg-get-label",
               "Return the label of the specified mapping/graph/image/set",
               SS_sargs,
               _SXI_get_label, SS_PR_PROC);

    SS_install(si, "pg-graph?",
               "Returns #t if the object is a PGS graph, and #f otherwise",
               SS_sargs,
               _SXI_graphp, SS_PR_PROC);

    SS_install(si, "pg-graph->pdbcurve",
               "Write an ULTRA curve object to a PDB file\nFORM (pg-graph->pdbcurve <curve> <file>)",
               SS_nargs,
               _SXI_graph_pdbcurve, SS_PR_PROC);

    SS_install(si, "pg-graph->pdbdata",
               "Write a PGS graph object to a PDB file\nFORM (pg-graph->pdbdata <graph> <file>)",
               SS_nargs,
               _SXI_graph_pdbdata, SS_PR_PROC);

    SS_install(si, "pg-drawable-info",
               "Get/Set drawable object information\nFORM (pg-drawable-info <name> [<value>])",
               SS_nargs,
               _SXI_drawable_info, SS_PR_PROC);

    SS_install(si, "pg-grotrian-graph?",
               "Returns #t if the object is a PGS grotrian graph, and #f otherwise",
               SS_sargs,
               _SXI_grotrian_graphp, SS_PR_PROC);

    SS_install(si, "pg-image?",
               "Returns #t if the object is a PGS image, and #f otherwise",
               SS_sargs,
               _SXI_imagep, SS_PR_PROC);

    SS_install(si, "pg-image-name",
               "Return the image referenced by name or menu number",
               SS_nargs,
               _SXI_get_text_image_name, SS_PR_PROC);

    SS_install(si, "pg-image->pdbdata",
               "Write a PGS image object to a PDB file\nFORM (pg-image->pdbdata <image> <file>)",
               SS_nargs,
               _SXI_image_pdbdata, SS_PR_PROC);

    SS_install(si, "pg-make-device",
               "Returns a PGS device with NAME, TYPE, and TITLE",
               SS_nargs,
               _SXI_make_device, SS_PR_PROC);

    SS_install(si, "pg-make-graph",
               "Return a PGS graph object\nFORM (pg-make-graph <domain> <range> [<color> <width> <style> <id>])",
               SS_nargs,
               _SXI_make_pgs_graph, SS_PR_PROC);

    SS_install(si, "pg-make-image",
               "Make a PGS image object\nFORM (pg-make-image <data> <kmax> <lmax> [<name> <xmin> <xmax> <ymin> <ymax> <zmin> zmax>])",
               SS_nargs,
               _SXI_make_image, SS_PR_PROC);

    SS_install(si, "pg-mapping-name",
               "Return the mapping referenced by name or menu number",
               SS_nargs,
               _SXI_get_text_mapping_name, SS_PR_PROC);

    SS_install(si, "pg-menu-item-type",
               "Return the name of the type of the referenced menu item",
               SS_nargs,
               _SXI_menu_item_type, SS_PR_PROC);

    SS_install(si, "pg-open-device",
               "Opens a PGS device DEV at (X, Y) with (DX, DY)",
               SS_nargs,
               _SXI_open_device, SS_PR_PROC);

    SS_install(si, "pg-range-extrema",
               "Return the range extrema\nFORM (pg-range-extrema <graph>)",
               SS_sargs,
               _SXI_ran_extrema, SS_PR_PROC);

    SS_install(si, "pg-range-limits",
               "Return the range plotting limits\nFORM (pg-range-limits <graph>)",
               SS_sargs,
               _SXI_ran_limits, SS_PR_PROC);

    SS_install(si, "pg-set-domain-limits!",
               "Set the domain plotting limits for the given graph",
               SS_nargs,
               _SXI_set_dom_limits, SS_PR_PROC);

    SS_install(si, "pg-set-graph-attribute!",
               "Set an attribute of the given graph",
               SS_nargs,
               _SXI_set_attr_graph, SS_PR_PROC);

    SS_install(si, "pg-set-label!",
               "Set the graph/mapping/image/set label to the given string",
               SS_nargs,
               _SXI_set_label, SS_PR_PROC);

    SS_install(si, "pg-set-range-limits!",
               "Set the range plotting limits for the given graph",
               SS_nargs,
               _SXI_set_ran_limits, SS_PR_PROC);

    SS_install_cf(si, "label-y-position",
                  "Internal variable for displaying mapping labels - do NOT set",
                  SS_acc_double,
		  labpy);

/* low level PGS functions */
    _SX_install_pgs_primitives(si);

/* interface object functions */
    _SX_install_pgs_iob(si);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
