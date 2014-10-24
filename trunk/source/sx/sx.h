/*
 * SX.H - header file for SX
 *      - Scheme with PACT Extensions
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#ifndef PCK_SX

#define PCK_SX

#include "panace.h"
#include "scheme.h"
#include "sx_gen.h"

/*--------------------------------------------------------------------------*/

/*                              DEFINED CONSTANTS                           */

/*--------------------------------------------------------------------------*/

#define MAXPTS  2000                   /* number of points in buffer arrays */

#define NPREFIX            26
#define N_OUTPUT_DEVICES    5

/*--------------------------------------------------------------------------*/

/*                            PROCEDURAL MACROS                             */

/*--------------------------------------------------------------------------*/

/* SX_GET_FLOAT_FROM_LIST - extract a double from the list and cdr the list */

#define SX_GET_FLOAT_FROM_LIST(_si, _v, _a, _s)                              \
   {obj = SS_car(_si, _a);                                                   \
    _a = SS_cdr(_si, _a);                                                    \
    if (SS_integerp(obj))                                                    \
       _v = (double) SS_INTEGER_VALUE(obj);                                  \
    else if (SS_floatp(obj))                                                 \
       _v = SS_FLOAT_VALUE(obj);                                             \
    else                                                                     \
       SS_error(_si, _s, obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_INTEGER_FROM_LIST - extract an int from the list and cdr the list */

#define SX_GET_INTEGER_FROM_LIST(_si, _v, _a, _s)                            \
   {obj = SS_car(_si, _a);                                                   \
    _a = SS_cdr(_si, _a);                                                    \
    if (SS_integerp(obj))                                                    \
       _v = SS_INTEGER_VALUE(obj);                                           \
    else                                                                     \
       SS_error(_si, _s, obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_STRING_FROM_LIST - extract a string from the list
 *                         - and cdr the list
 */

#define SX_GET_STRING_FROM_LIST(_si, _v, _a, _s)                             \
   {obj = SS_car(_si, _a);                                                   \
    _a = SS_cdr(_si, _a);                                                    \
    _v = CSTRSAVE(SS_get_string(obj));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_OBJECT_FROM_LIST - extract any object from the list
 *                         - and cdr the list
 */

#define SX_GET_OBJECT_FROM_LIST(_si, _pred, _v, _x, _a, _s)                  \
   {obj = SS_car(_si, _a);                                                   \
    _a = SS_cdr(_si, _a);                                                    \
    if (_pred)                                                               \
       _v = (_x);                                                            \
    else                                                                     \
       SS_error(_si, _s, obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_GRAPH_FROM_LIST - extract a PG_graph from the list
 *                        - and cdr the list
 */

#define SX_GET_GRAPH_FROM_LIST(_si, _v, _a, _s)                              \
   {obj  = SS_car(_si, _a);                                                  \
    _a = SS_cdr(_si, _a);                                                    \
    if (SX_GRAPHP(obj))                                                      \
       _v = SS_GET(PG_graph, obj);                                           \
    else                                                                     \
       SS_error(_si, _s, obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_SET_FROM_LIST - extract a PM_set from the list
 *                      - and cdr the list
 */

#define SX_GET_SET_FROM_LIST(_si, _v, _a, _s)                                \
   {obj = SS_car(_si, _a);                                                   \
    _a = SS_cdr(_si, _a);                                                    \
    if (SX_SETP(obj))                                                        \
       _v = SS_GET(PM_set, obj);                                             \
    else                                                                     \
       SS_error(_si, _s, obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_MAPPING_FROM_LIST - extract a PM_mapping from the list
 *                          - and cdr the list
 *                          - if item is a graph extract the mapping from it
 *                          - NOTE: generalization of generated macro
 */

#undef SX_GET_MAPPING_FROM_LIST
#define SX_GET_MAPPING_FROM_LIST(_si, _p, _a)                                \
    {PG_graph *_g;                                                           \
     object *_o;                                                             \
     _o = SS_car(_si, _a);                                                   \
     _a = SS_cdr(_si, _a);                                                   \
     if (SX_GRAPHP(_o))                                                      \
        {_g = SS_GET(PG_graph, _o);                                          \
         _p = g->f;}                                                         \
     else if (SX_MAPPINGP(_o))                                               \
        _p = SS_GET(PM_mapping, _o);                                         \
     else                                                                    \
        SS_error(_si, "BAD MAPPING OR GRAPH - SX_GET_MAPPING_FROM_LIST",     \
		 _o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_OK_TO_DRAW - TRUE iff in graphics mode or is a hardcopy device */

#define SX_OK_TO_DRAW(dev) (SX_gs.gr_mode || HARDCOPY_DEVICE(dev))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define FUNCTION_NAME(_o)                                                    \
    (SS_GET(SX_object, _o)->name)
#define FUNCTION_PROC(_o)                                                    \
    ((PFPObject) (SS_GET(SX_object, _o)->p1.fp))
#define FUNCTION_HANDLER(_o)                                                 \
    ((PFPObject) (SS_GET(SX_object, _o)->p2.fp))
#define FUNCTION_DOCUMENTATION(_o)                                           \
    ((char *) (SS_GET(SX_object, _o)->p3.dp))

#define SX_SET_FUNCTION(_o, _v)       (SS_GET(SX_object, _o)->p1.dp = (void *) _v)
#define SX_SET_HANDLER(_o, _v)        (SS_GET(SX_object, _o)->p2.fp = (PFVoid) _v)
#define SX_SET_DOCUMENTATION(_o, _v)  (SS_GET(SX_object, _o)->p3.dp = (void *) _v)

/* PDBLib Types */

#define FILE_FILE(_t, _o)    (_t *) ((_o)->file)

#undef FILE_TYPE    /* Solaris has this */
#define FILE_TYPE(_o)        (SS_GET(g_file, _o)->type)
#define FILE_STREAM(_t, _o)  (_t *) (SS_GET(g_file, _o)->file)
#define FILE_NAME(_o)        (SS_GET(g_file, _o)->name)

#define PDBDATA_NAME(_o)     (SS_GET(g_pdbdata, _o)->name)
#define PDBDATA_DATA(_o)     (SS_GET(g_pdbdata, _o)->data)
#define PDBDATA_EP(_o)       (SS_GET(g_pdbdata, _o)->ep)
#define PDBDATA_FILE(_o)     (SS_GET(g_pdbdata, _o)->file)

/* PANACEA Types */

/* IV_SPECIFICATION is PD_DEFSTR'd in DEF_STR in PANACEA.C
 * so any changes here must be reflected there
 */

#define IV_SPECIFICATION_TYPE(_o)      (SS_GET(PA_iv_specification, _o)->type)
#define IV_SPECIFICATION_NAME(_o)      (SS_GET(PA_iv_specification, _o)->name)
#define IV_SPECIFICATION_FILE(_o)      (SS_GET(PA_iv_specification, _o)->file)
#define IV_SPECIFICATION_NUM(_o)       (SS_GET(PA_iv_specification, _o)->num)
#define IV_SPECIFICATION_INDEX(_o)     (SS_GET(PA_iv_specification, _o)->index)
#define IV_SPECIFICATION_SPEC(_o)      (SS_GET(PA_iv_specification, _o)->spec)
#define IV_SPECIFICATION_DATA(_o)      (SS_GET(PA_iv_specification, _o)->data)
#define IV_SPECIFICATION_NEXT(_o)      (SS_GET(PA_iv_specification, _o)->next)

/* PLOT_REQUEST is PD_DEFSTR'd in DEF_STR in PANACEA.C
 * so any changes here must be reflected there
 */

#define PLOT_REQUEST_Y_AXIS(_o)      (SS_GET(plot_request, _o)->y_axis)
#define PLOT_REQUEST_Y_ZONE(_o)      (SS_GET(plot_request, _o)->y_zone)
#define PLOT_REQUEST_Y_AXIS_INT(_o)  (SS_GET(plot_request, _o)->y_axis_int)
#define PLOT_REQUEST_X_AXIS(_o)      (SS_GET(plot_request, _o)->x_axis)
#define PLOT_REQUEST_X_ZONE(_o)      (SS_GET(plot_request, _o)->x_zone)
#define PLOT_REQUEST_X_AXIS_INT(_o)  (SS_GET(plot_request, _o)->x_axis_int)
#define PLOT_REQUEST_SNAP(_o)        (SS_GET(plot_request, _o)->snap)
#define PLOT_REQUEST_SNAP_TIMES(_o)  (SS_GET(plot_request, _o)->snap_times)
#define PLOT_REQUEST_NEXT(_o)        (SS_GET(plot_request, _o)->next)

#define PLOT_MAP_NAME(_o)       (SS_GET(plot_map, _o)->name)
#define PLOT_MAP_FALSE(_o)      (SS_GET(plot_map, _o)->false)
#define PLOT_MAP_INDEX(_o)      (SS_GET(plot_map, _o)->index)
#define PLOT_MAP_CONV(_o)       (SS_GET(plot_map, _o)->conv)

#define PLT_CRV_LABEL(_o)      (SS_GET(plt_crv, _o)->label)
#define PLT_CRV_Y_INDEX(_o)    (SS_GET(plt_crv, _o)->y_index)
#define PLT_CRV_Y_CONV(_o)     (SS_GET(plt_crv, _o)->y_conv)
#define PLT_CRV_X_INDEX(_o)    (SS_GET(plt_crv, _o)->x_index)
#define PLT_CRV_X_CONV(_o)     (SS_GET(plt_crv, _o)->x_conv)
#define PLT_CRV_PLT_R(_o)      (SS_GET(plt_crv, _o)->plt_r)
#define PLT_CRV_NPTS(_o)       (SS_GET(plt_crv, _o)->npts)

/* PGS extensions */

/* GRAPH - contains the information necessary to display a list of functions
 *       - in some fashion
 */

#define GRAPH_F(_o)             (SS_GET(PG_graph, _o)->f)
#define GRAPH_IDENTIFIER(_o)    (SS_GET(PG_graph, _o)->identifier)
#define GRAPH_INFO(_o)          (SS_GET(PG_graph, _o)->info)
#define GRAPH_NEXT(_o)          (SS_GET(PG_graph, _o)->next)

/* INTERFACE_OBJECT - access PG_interface_object structure members */

#define INTERFACE_OBJECT(_o)         (SS_GET(PG_interface_object, _o))
#define INTERFACE_OBJECT_NAME(_o)    (SS_GET(PG_interface_object, _o)->name)
#define INTERFACE_OBJECT_TYPE(_o)    (SS_GET(PG_interface_object, _o)->type)
#define INTERFACE_OBJECT_DEVICE(_o)  (SS_GET(PG_interface_object, _o)->device)

/* IMAGE - access image structure members */

#define IMAGE_NAME(_o)          (SS_GET(PG_image, _o)->label)
#define IMAGE_DATA(_o)          (SS_GET(PG_image, _o)->bf)
#define IMAGE_TYPE(_o)          (SS_GET(PG_image, _o)->element_type)

#define DEV_ATTRIBUTES_CLIPPING(_o)        (SS_GET(PG_dev_attributes, _o)->clipping)
#define DEV_ATTRIBUTES_CHAR_FONT(_o)       (SS_GET(PG_dev_attributes, _o)->char_font)
#define DEV_ATTRIBUTES_CHAR_FRAC(_o)       (SS_GET(PG_dev_attributes, _o)->char_frac)
#define DEV_ATTRIBUTES_CHAR_HEIGHT(_o)     (SS_GET(PG_dev_attributes, _o)->char_height)
#define DEV_ATTRIBUTES_CHAR_PRECISION(_o)  (SS_GET(PG_dev_attributes, _o)->char_precision)
#define DEV_ATTRIBUTES_CHAR_SPACE(_o)      (SS_GET(PG_dev_attributes, _o)->char_space)
#define DEV_ATTRIBUTES_CHAR_UP_X(_o)       (SS_GET(PG_dev_attributes, _o)->char_up_x)
#define DEV_ATTRIBUTES_CHAR_UP_Y(_o)       (SS_GET(PG_dev_attributes, _o)->char_up_y)
#define DEV_ATTRIBUTES_CHAR_WIDTH(_o)      (SS_GET(PG_dev_attributes, _o)->char_width)
#define DEV_ATTRIBUTES_FILL_COLOR(_o)      (SS_GET(PG_dev_attributes, _o)->fill_color)
#define DEV_ATTRIBUTES_LINE_COLOR(_o)      (SS_GET(PG_dev_attributes, _o)->line_color)
#define DEV_ATTRIBUTES_LINE_STYLE(_o)      (SS_GET(PG_dev_attributes, _o)->line_style)
#define DEV_ATTRIBUTES_LINE_WIDTH(_o)      (SS_GET(PG_dev_attributes, _o)->line_width)
#define DEV_ATTRIBUTES_LOGICAL_OP(_o)      (SS_GET(PG_dev_attributes, _o)->logical_op)
#define DEV_ATTRIBUTES_TEXT_COLOR(_o)      (SS_GET(PG_dev_attributes, _o)->text_color)


/* PML Extensions */

#define C_ARRAY(_o)                  (SS_GET(C_array, _o))

#define SET_NAME(_o)                 (SS_GET(PM_set, _o)->name)
#define SET_ELEMENT_TYPE(_o)         (SS_GET(PM_set, _o)->element_type)
#define SET_N_ELEMENTS(_o)           (SS_GET(PM_set, _o)->n_elements)
#define SET_DIMENSION(_o)            (SS_GET(PM_set, _o)->dimension)
#define SET_MAX_INDEX(_o)            (SS_GET(PM_set, _o)->max_index)
#define SET_ELEMENTS(_o)             (SS_GET(PM_set, _o)->elements)
#define SET_ES_TYPE(_o)              (SS_GET(PM_set, _o)->es_type)
#define SET_EXTREMA(_o)              (SS_GET(PM_set, _o)->extrema)
#define SET_SCALES(_o)               (SS_GET(PM_set, _o)->scales)
#define SET_OPERS(_o)                (SS_GET(PM_set, _o)->opers)
#define SET_METRIC(_o)               (SS_GET(PM_set, _o)->metric)
#define SET_SYMMETRY_TYPE(_o)        (SS_GET(PM_set, _o)->symmetry_type)
#define SET_SYMMETRY(_o)             (SS_GET(PM_set, _o)->symmetry)
#define SET_TOPOLOGY_TYPE(_o)        (SS_GET(PM_set, _o)->topology_type)
#define SET_TOPOLOGY(_o)             (SS_GET(PM_set, _o)->topology)
#define SET_INFO_TYPE(_o)            (SS_GET(PM_set, _o)->info_type)
#define SET_INFO(_o)                 (SS_GET(PM_set, _o)->info)

#define MAPPING_NAME(_o)             (SS_GET(PM_mapping, _o)->name)
#define MAPPING_DOMAIN(_o)           (SS_GET(PM_mapping, _o)->domain)
#define MAPPING_RANGE(_o)            (SS_GET(PM_mapping, _o)->range)
#define MAPPING_MAP_TYPE(_o)         (SS_GET(PM_mapping, _o)->map_type)
#define MAPPING_MAP(_o)              (SS_GET(PM_mapping, _o)->map)
#define MAPPING_FILE_TYPE(_o)        (SS_GET(PM_mapping, _o)->file_type)
#define MAPPING_FILE_INFO(_o)        (SS_GET(PM_mapping, _o)->file_info)
#define MAPPING_FILE(_o)             (SS_GET(PM_mapping, _o)->file)
#define MAPPING_NEXT(_o)             (SS_GET(PM_mapping, _o)->next)

/*--------------------------------------------------------------------------*/

/*                           PREDICATE MACROS                               */

/*--------------------------------------------------------------------------*/
 
/* SX Types */

#define SX_FUNCTIONP(_o)         (SS_OBJECT_TYPE(_o) == G_FUNCTION)
#define SX_FILEP(_o)             (SS_OBJECT_TYPE(_o) == G_FILE)

/* PDBLib Types */

#define SX_PDBDATAP(_o)          (SS_OBJECT_TYPE(_o) == G_PDBDATA)

/* PANACEA Types */

#define SX_PLOT_REQUESTP(_o)     (SS_OBJECT_TYPE(_o) == G_PLOT_REQUEST)
#define SX_PLOT_MAPP(_o)         (SS_OBJECT_TYPE(_o) == G_PLOT_MAP)
#define SX_PLT_CRVP(_o)          (SS_OBJECT_TYPE(_o) == G_PLT_CRV)

/*--------------------------------------------------------------------------*/

/*                           STRUCT DEFINITIONS                             */

/*--------------------------------------------------------------------------*/

/* SCHEME object type designations */

enum e_SX_object_type
   {G_FILE = 240,
    G_MEMDES,
    G_PDBDATA,
    G_PLOT_REQUEST,
    G_PLOT_MAP,
    G_PLT_CRV,
    G_FUNCTION};

typedef enum e_SX_object_type SX_object_type;

enum e_SX_session_mode
   {SX_MODE_SCHEME = 0, SX_MODE_SX, SX_MODE_PDBVIEW, SX_MODE_ULTRA};

typedef enum e_SX_session_mode SX_session_mode;


typedef struct s_g_pdbdata g_pdbdata;
typedef struct s_SX_object SX_object;
typedef struct s_SX_menu_item SX_menu_item;
typedef struct s_SX_reparsed SX_reparsed;
typedef struct s_g_file g_file;
typedef struct s_out_device out_device;
typedef struct s_SX_scope_public SX_scope_public;

typedef union u_SX_pointer SX_pointer;

struct s_g_pdbdata
   {char *name;                                     /* name of the variable */
    void *data;                                      /* pointer to the data */
    syment *ep;                                       /* symbol table entry */
    PDBfile *file;};  /* the file this data is from (needs structure chart) */

union u_SX_pointer
   {PFVoid fp;
    void *dp;};

struct s_SX_object
   {char *name;
    char *type;
    SX_pointer p1;
    SX_pointer p2;
    SX_pointer p3;};

struct s_SX_menu_item
   {char *vname;
    char *label;
    char *type;};

struct s_SX_reparsed
   {char bf[BFLRG];
    SS_psides *si;
    char *(*reproc)(SX_reparsed *pd, char *s);
    object *(*replot)(SS_psides *si);};

struct s_g_file
   {char *name;
    char *type;                                       /* file type SX knows */
    void *file;
    object *file_object;
    SC_array *menu_lst;
    g_file *next;};
    
struct s_out_device
   {int exist;
    int active;
    int background_color;
    char *dname;               /* device name lower case */
    char *dupp;                /* device name upper case */
    double x0[PG_SPACEDM];
    double dx0[PG_SPACEDM];
    char *fname;               /* file name */
    char *type;
    PG_device *dev;};


struct s_SX_scope_public

/* non-zero default value */
   {int default_npts;

    int n_curves;               /* curve controls */
    int n_curves_read;
    int next_available_number;

    int interp_method;          /* interpolation controls */
    double interp_power;
    double interp_scale;
    double interp_strength;
    double interp_mq_scale;

    int gri_type_size;       /* GUI controls */

    double gri_x[PG_SPACEDM];
    double gri_dx[PG_SPACEDM];

/* zero default value */
    int autorange;
    int autodomain;
    int autoplot;
    int *data_index;
    int disp_individ_diff;
    int prefix_list[NPREFIX];
    int background_color_white;
    int border_width;
    int data_id;
    int gr_mode;
    int grid;
    int *number;
    int plot_flag;
    int plot_type_size;
    int qflag;
    int show_mouse_location;
    int log_scale[PG_SPACEDM];

    double console_x[PG_SPACEDM];
    double console_dx[PG_SPACEDM];
    double gwc[PG_BOXSZ];
    double gpad[PG_BOXSZ];
    double window_x[PG_SPACEDM];
    double window_P[PG_SPACEDM];
    double window_dx[PG_SPACEDM];
    double window_dx_P[PG_SPACEDM];

    double marker_orientation;
    double show_mouse_x[PG_SPACEDM];
    double view_aspect;
    double view_x[PG_BOXSZ];
    double view_dx[PG_SPACEDM];
    double view_angle[PG_SPACEDM];

    SX_session_mode sm;

    char *text_output_format;
    char *command_log_name;
    char *console_type;
    char *current_palette;
    char data_directory[MAXLINE];
    char *display_name;
    char *display_type;
    char *display_title;
    char err[MAXLINE];         /* global buffer for certain error messages */
    char *gri_title;
    char *gri_type_face;
    char *gri_type_style;
    char *plot_type_style;
    char *promotion_type;
    char *pui_file;
    char *smooth_method;

    PG_rendering render_def;
    PG_rendering render_1d_1d;
    PG_rendering render_2d_1d;
    PG_rendering render_2d_2d;
    PG_rendering render_3d_1d;

    data_standard *PDB_STANDARD;

    PFInt pan_data_hook;

    curve *dataset;

    object *var_tab;
    object *curfile;
    object *ovif;
    object *(*plot_hook)(SS_psides *si);

    FILE *command_log;
    FILE *out_text;
    FILE *out_bin;

    PDBfile *out_pdb;
    PDBfile *vif;

    g_file *gvif;
    g_file *file_list;

    PG_device *graphics_device;};


#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                        VARIABLE DECLARATIONS                             */

/*--------------------------------------------------------------------------*/

extern SX_scope_public
 SX_gs;

/*--------------------------------------------------------------------------*/

/*                        FUNCTION DECLARATIONS                             */

/*--------------------------------------------------------------------------*/

/* SXCMD.C declarations */

extern void
 SX_end(SS_psides *si, int val),
 SX_init_view(SS_psides *si),
 SX_init_mappings(SS_psides *si),
 SX_init_env(SS_psides *si);

extern int
 SX_rd_scm(SS_psides *si, char *name),
 SX_command(SS_psides *si, char *file, char *cmd);


/* SXCONT.C declarations */

extern int
 SX_split_command(char *cmd, int nc, char *lst),
 SX_expand_expr(char *s, int nb);

extern char
 *SX_wrap_paren(char *open, char *form, char *close, size_t ln);

extern void
 SX_register_devices(void),
 SX_load_rc(SS_psides *si, char *ffn, int ldrc, char *ifna, char *ifnb),
 SX_parse(SX_reparsed *pd, object *strm),
 SX_init_device_vars(int idev, double *xf, double *dxf),
 SX_install_global_vars(SS_psides *si);

extern out_device
 *SX_get_device(int idev),
 *SX_match_device(PG_device *dev);


/* SXCRV.C declarations */

extern object
 *SX_get_curve_obj(int j),
 *SX_get_curve_proc(int j),
 *SX_get_curve_var(int j),
 *SX_mk_curve_proc(int i),
 *SX_mk_curve(SS_psides *si, int na, double **xa,
	      char *label, char *filename,
	      object *(*plt)(SS_psides *si)),
 *SX_set_crv_id(int i, char *id),
 *SX_re_id(SS_psides *si),
 *SX_get_data_domain(SS_psides *si, object *argl),
 *SX_get_data_range(SS_psides *si, object *argl),
 *SX_rl_curve(int j);

extern void
 SX_zero_curve(int i),
 SX_assign_next_id(SS_psides *si, int i, object *(*plt)(SS_psides *si)),
 SX_enlarge_dataset(SS_psides *si, PFVoid eval);

extern int
 SX_get_data_index(char *s),
 SX_get_crv_index_i(object *obj),
 SX_get_curve_id(char *s),
 SX_get_crv_index_j(object *obj),
 SX_curvep(char *s),
 SX_next_space(SS_psides *si),
 SX_curve_id(object *c),
 SX_curvep_b(object *obj),
 SX_curvep_a(object *obj);


/* SXFUNC.C declarations */

extern void
 SX_quit(int i),
 SX_filter_coeff(SS_psides *si, double *yp, int n,
		 C_array *arr, int ntimes),
 SX_install_global_funcs(SS_psides *si);

extern object
 *SX_plane(SS_psides *si, object *argl);

extern int
 SX_map_count(void);


/* SXGRI.C declarations */

extern void
 SX_end_prog(void *d, PG_event *ev),
 SX_rem_iob(PG_interface_object *iob, int flag);


/* SXGROT.C declarations */

extern object
 *SX_draw_grotrian(SS_psides *si, object *argl);


/* SXHAND.C declarations */

extern object
 *SX_display_map(SS_psides *si, object *mo);

extern void
 SX_determine_drw_obj(SS_psides *si, PM_mapping **pf,
		      object **po, object **pargl),
 SX_determine_mapping(SS_psides *si, PM_mapping **pf, object **pargl),
 SX_mf_install(SS_psides *si);

extern int
 SX_have_display_list(SS_psides *si);


/* SXHBO.C declarations */

extern PM_mapping
 *SX_build_return_mapping(SS_psides *si, PM_mapping *h, char *label,
			  PM_set *domain, int init, int wgt);


/* SXHOOK.C declarations */

extern int
 SX_fprintf(FILE *fp, char *fmt, ...),
 SX_fputs(const char *s, FILE *fp);


/* SXIO.C declarations */

extern object
*SX_print(SS_psides *si, object *argl);


/* SXMM.C declarations */

extern object
 *SX_mk_gfile(SS_psides *si, g_file *po);


/* SXMODE.C declarations */

extern void
 SX_setup_viewspace(PG_device *dev, double mh);

extern object
 *SX_mode_text(SS_psides *si),
 *SX_mode_graphics(SS_psides *si);

extern void
 SX_mode_pdbview(SS_psides *si, int load_init, int load_rc),
 SX_install_modes(SS_psides *si);


/* SXPAN.C declarations */

extern void
 SX_install_panacea_funcs(SS_psides *si);

extern object
 *SX_mk_package(SS_psides *si, PA_package *pck),
 *SX_mk_variable(SS_psides *si, PA_variable *pp);


/* SXPDB.C declarations */

extern void
 SX_install_pdb_funcs(SS_psides *si);

extern int
 SX_pdbfilep(object *arg),
 SX_ipdbfilep(object *arg),
 SX_convert(char *dtype, void **pd, char *stype, void *s, int n, int flag);

extern object
 *SX_get_pdbfile(SS_psides *si, object *argl,
		 PDBfile **pfile, g_file **gfile),
 *SX_get_file(SS_psides *si, object *argl, g_file **pfile),
 *SX_pdbdata_handler(SS_psides *si, PDBfile *file,
		     char *name, char *type, void *vr, int flag);


/* SXPDBA.C declarations */

extern void
 SX_install_pdb_attr_funcs(SS_psides *si);


/* SXPGS.C declarations */

extern object
 *SX_mk_graph(SS_psides *si, PG_graph *g),
 *SX_mk_dev_attributes(SS_psides *si, PG_dev_attributes *da),
 *SX_get_ref_map(SS_psides *si, g_file *po, int indx, char *dtype);

extern pcons
 *SX_set_attr_alist(SS_psides *si, pcons *inf,
		    char *name, char *type, object *val);

extern int
 SX_next_color(PG_device *dev);

extern void
 SX_install_pgs_funcs(SS_psides *si),
 SX_expose_event_handler(PG_device *dev, PG_event *ev),
 SX_update_event_handler(PG_device *dev, PG_event *ev),
 SX_motion_event_handler(PG_device *dev, PG_event *ev),
 SX_mouse_event_handler(PG_device *dev, PG_event *ev),
 SX_default_event_handler(PG_device *dev, PG_event *ev);


/* SXPML.C declarations */

extern void
 SX_install_pml_funcs(SS_psides *si);

extern object
 *SX_setp(object *obj),
 *SX_list_array(SS_psides *si, object *argl);


/* SXSET.C declarations */

extern object
 *SX_arg_prep(SS_psides *si, object *argl);

extern SS_psides
 *SX_init(char *code, char *vers, int c, char **v, char **env);

extern void
 SX_reset_prefix(void),
 SX_install_funcs(SS_psides *si);

extern int
 SX_next_prefix(void),
 SX_import_so(SS_psides *si, char *hdr, char *so, char *flags);


/* SXULIO.C declarations */

extern object
 *SX_read_data(SS_psides *si, object *obj),
 *SX_read_ver1(SS_psides *si, object *obj),
 *SX_read_text_table(SS_psides *si, object *argl),
 *SX_table_curve(SS_psides *si, object *argl),
 *SX_table_attr(SS_psides *si),
 *SX_crv_file_info(SS_psides *si, object *obj),
 *SX_write_data(SS_psides *si, object *argl);

extern void
 SX_cache_addpid(void),
 SX_uncache_curve(SS_psides *si, curve *crv),
 SX_push_open_file(FILE *fp),
 SX_close_open_files(void);


#ifdef __cplusplus
}
#endif

#endif

