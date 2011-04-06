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

#define SX_GET_FLOAT_FROM_LIST(x, argl, s)                                   \
   {obj  = SS_car(argl);                                                     \
    argl = SS_cdr(argl);                                                     \
    if (SS_integerp(obj))                                                    \
       x = (double) SS_INTEGER_VALUE(obj);                                   \
    else if (SS_floatp(obj))                                                 \
       x = SS_FLOAT_VALUE(obj);                                              \
    else                                                                     \
       SS_error(s, obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_INTEGER_FROM_LIST - extract an int from the list and cdr the list */

#define SX_GET_INTEGER_FROM_LIST(x, argl, s)                                 \
   {obj  = SS_car(argl);                                                     \
    argl = SS_cdr(argl);                                                     \
    if (SS_integerp(obj))                                                    \
       x = SS_INTEGER_VALUE(obj);                                            \
    else                                                                     \
       SS_error(s, obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_STRING_FROM_LIST - extract a string from the list
 *                         - and cdr the list
 */

#define SX_GET_STRING_FROM_LIST(x, argl, s)                                  \
   {obj  = SS_car(argl);                                                     \
    argl = SS_cdr(argl);                                                     \
    x    = CSTRSAVE(SS_get_string(obj));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_OBJECT_FROM_LIST - extract any object from the list
 *                         - and cdr the list
 */

#define SX_GET_OBJECT_FROM_LIST(predicate, x, value, argl, s)                \
   {obj  = SS_car(argl);                                                     \
    argl = SS_cdr(argl);                                                     \
    if (predicate)                                                           \
       x = (value);                                                          \
    else                                                                     \
       SS_error(s, obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_ARRAY_FROM_LIST - extract a numeric array from the list
 *                        - and cdr the list
 */

#define SX_GET_ARRAY_FROM_LIST(x, argl, s)                                   \
   {obj  = SS_car(argl);                                                     \
    argl = SS_cdr(argl);                                                     \
    if (SX_NUMERIC_ARRAYP(obj))                                              \
       x = NUMERIC_ARRAY_DATA(obj);                                          \
    else                                                                     \
       SS_error(s, obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_GRAPH_FROM_LIST - extract a PG_graph from the list
 *                        - and cdr the list
 */

#define SX_GET_GRAPH_FROM_LIST(x, argl, s)                                   \
   {obj  = SS_car(argl);                                                     \
    argl = SS_cdr(argl);                                                     \
    if (SX_GRAPHP(obj))                                                      \
       x = SS_GET(PG_graph, obj);                                            \
    else                                                                     \
       SS_error(s, obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_SET_FROM_LIST - extract a PM_set from the list
 *                      - and cdr the list
 */

#define SX_GET_SET_FROM_LIST(x, argl, s)                                     \
   {obj  = SS_car(argl);                                                     \
    argl = SS_cdr(argl);                                                     \
    if (SX_SETP(obj))                                                        \
       x = SS_GET(PM_set, obj);                                              \
    else                                                                     \
       SS_error(s, obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_MAPPING_FROM_LIST - extract a PM_mapping from the list
 *                          - and cdr the list
 *                          - if item is a graph extract the mapping from it
 */

#define SX_GET_MAPPING_FROM_LIST(pn, argl)                                   \
    {PG_graph *g;                                                            \
     object *obj;                                                            \
     obj  = SS_car(argl);                                                    \
     argl = SS_cdr(argl);                                                    \
     if (SX_GRAPHP(obj))                                                     \
        {g  = SS_GET(PG_graph, obj);                                         \
         pn = g->f;}                                                         \
     else if (SX_MAPPINGP(obj))                                              \
        pn = SS_GET(PM_mapping, obj);                                        \
     else                                                                    \
        SS_error("BAD MAPPING OR GRAPH - SX_GET_MAPPING_FROM_LIST",          \
                 obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_OK_TO_DRAW - TRUE iff in graphics mode or is a hardcopy device */

#define SX_OK_TO_DRAW(dev) (SX_gr_mode || HARDCOPY_DEVICE(dev))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define FUNCTION_NAME(x)                                                     \
    (SS_GET(SX_object, x)->name)
#define FUNCTION_PROC(x)                                                     \
    ((PFPObject) (SS_GET(SX_object, x)->p1.fp))
#define FUNCTION_HANDLER(x)                                                  \
    ((PFPObject) (SS_GET(SX_object, x)->p2.fp))
#define FUNCTION_DOCUMENTATION(x)                                            \
    ((char *) (SS_GET(SX_object, x)->p3.dp))

#define SX_SET_FUNCTION(x, v)       (SS_GET(SX_object, x)->p1.dp = (void *) v)
#define SX_SET_HANDLER(x, v)        (SS_GET(SX_object, x)->p2.fp = (PFVoid) v)
#define SX_SET_DOCUMENTATION(x, v)  (SS_GET(SX_object, x)->p3.dp = (void *) v)

/* PDBLib Types */

#define FILE_FILE(type, x)    (type *) ((x)->file)

#define FILE_TYPE(x)                   (SS_GET(g_file, x)->type)
#define FILE_STREAM(type, x)  (type *) (SS_GET(g_file, x)->file)
#define FILE_NAME(x)                   (SS_GET(g_file, x)->name)

#define PDBDATA_NAME(x)     (SS_GET(g_pdbdata, x)->name)
#define PDBDATA_DATA(x)     (SS_GET(g_pdbdata, x)->data)
#define PDBDATA_EP(x)       (SS_GET(g_pdbdata, x)->ep)
#define PDBDATA_FILE(x)     (SS_GET(g_pdbdata, x)->file)

#define SYMENT_TYPE(x)      PD_entry_type(SS_GET(syment, x))
#define SYMENT_DIMS(x)      PD_entry_dimensions(SS_GET(syment, x))
#define SYMENT_NUM(x)       PD_entry_number(SS_GET(syment, x))
#define SYMENT_ADDR(x)      PD_entry_address(SS_GET(syment, x))

#define DEFSTR_TYPE(x)      (SS_GET(defstr, x)->type)
#define DEFSTR_SIZE(x)      (SS_GET(defstr, x)->size)
#define DEFSTR_ALIGN(x)     (SS_GET(defstr, x)->alignment)
#define DEFSTR_MEMBERS(x)   (SS_GET(defstr, x)->members)

/* PANACEA Types */

#define PANVAR_NAME(x)      (SS_GET(PA_variable, x)->name)
#define PANVAR_DATA(x)      (SS_GET(PA_variable, x)->data)
#define PANVAR_SIZE(x)      (SS_GET(PA_variable, x)->size)
#define PANVAR_RESTART(x)   (SS_GET(PA_variable, x)->restart)
#define PANVAR_CLASS(x)     (SS_GET(PA_variable, x)->option)
#define PANVAR_CONV(x)      (SS_GET(PA_variable, x)->conv)
#define PANVAR_UNIT(x)      (SS_GET(PA_variable, x)->unit)
#define PANVAR_DESC(x)      (SS_GET(PA_variable, x)->desc)

#define PACKAGE_NAME(x)     (SS_GET(PA_package, x)->name)
#define PACKAGE_TIME(x)     (SS_GET(PA_package, x)->time)
#define PACKAGE_SPACE(x)    (SS_GET(PA_package, x)->space)
#define PACKAGE_DT(x)       (SS_GET(PA_package, x)->dt)
#define PACKAGE_GENCMD(x)   (SS_GET(PA_package, x)->gencmd)
#define PACKAGE_DFSTRC(x)   (SS_GET(PA_package, x)->dfstrc)
#define PACKAGE_INTRN(x)    (SS_GET(PA_package, x)->intrn)
#define PACKAGE_DEFVAR(x)   (SS_GET(PA_package, x)->defvar)
#define PACKAGE_DEFCNT(x)   (SS_GET(PA_package, x)->defcnt)
#define PACKAGE_INIZER(x)   (SS_GET(PA_package, x)->inizer)
#define PACKAGE_MAIN(x)     (SS_GET(PA_package, x)->main)
#define PACKAGE_PPSOR(x)    (SS_GET(PA_package, x)->ppsor)
#define PACKAGE_FINZER(x)   (SS_GET(PA_package, x)->finzer)
#define PACKAGE_PPCMD(x)    (SS_GET(PA_package, x)->ppcmd)
#define PACKAGE_N_SWTCH(x)  (SS_GET(PA_package, x)->n_swtch)
#define PACKAGE_SWTCH(x)    (SS_GET(PA_package, x)->iswtch)
#define PACKAGE_N_PARAM(x)  (SS_GET(PA_package, x)->n_param)
#define PACKAGE_PARAM(x)    (SS_GET(PA_package, x)->rparam)
#define PACKAGE_N_ASCII(x)  (SS_GET(PA_package, x)->n_ascii)
#define PACKAGE_ASCII(x)    (SS_GET(PA_package, x)->ascii)
#define PACKAGE_S(x)        (SS_GET(PA_package, x)->s)

#define SOURCE_VARIABLE_NAME(x)        (SS_GET(PA_src_variable, x)->name)
#define SOURCE_VARIABLE_VAR_INDEX(x)   (SS_GET(PA_src_variable, x)->var_index)
#define SOURCE_VARIABLE_N_TIMEs(x)     (SS_GET(PA_src_variable, x)->n_times)
#define SOURCE_VARIABLE_CONV(x)        (SS_GET(PA_src_variable, x)->conv)
#define SOURCE_VARIABLE_TIMES(x)       (SS_GET(PA_src_variable, x)->times)
#define SOURCE_VARIABLE_QUEUE(x)       (SS_GET(PA_src_variable, x)->queue)
#define SOURCE_VARIABLE_QUEUE_TIMES(x) (SS_GET(PA_src_variable, x)->queue_times)
#define SOURCE_VARIABLE_INDEX(x)       (SS_GET(PA_src_variable, x)->index)
#define SOURCE_VARIABLE_SIZE(x)        (SS_GET(PA_src_variable, x)->size)
#define SOURCE_VARIABLE_FILE(x)        (SS_GET(PA_src_variable, x)->file)

/* IV_SPECIFICATION is PD_DEFSTR'd in DEF_STR in PANACEA.C
 * so any changes here must be reflected there
 */

#define IV_SPECIFICATION_TYPE(x)      (SS_GET(PA_iv_specification, x)->type)
#define IV_SPECIFICATION_NAME(x)      (SS_GET(PA_iv_specification, x)->name)
#define IV_SPECIFICATION_FILE(x)      (SS_GET(PA_iv_specification, x)->file)
#define IV_SPECIFICATION_NUM(x)       (SS_GET(PA_iv_specification, x)->num)
#define IV_SPECIFICATION_INDEX(x)     (SS_GET(PA_iv_specification, x)->index)
#define IV_SPECIFICATION_SPEC(x)      (SS_GET(PA_iv_specification, x)->spec)
#define IV_SPECIFICATION_DATA(x)      (SS_GET(PA_iv_specification, x)->data)
#define IV_SPECIFICATION_NEXT(x)      (SS_GET(PA_iv_specification, x)->next)

/* PLOT_REQUEST is PD_DEFSTR'd in DEF_STR in PANACEA.C
 * so any changes here must be reflected there
 */

#define PLOT_REQUEST_Y_AXIS(x)      (SS_GET(plot_request, x)->y_axis)
#define PLOT_REQUEST_Y_ZONE(x)      (SS_GET(plot_request, x)->y_zone)
#define PLOT_REQUEST_Y_AXIS_INT(x)  (SS_GET(plot_request, x)->y_axis_int)
#define PLOT_REQUEST_X_AXIS(x)      (SS_GET(plot_request, x)->x_axis)
#define PLOT_REQUEST_X_ZONE(x)      (SS_GET(plot_request, x)->x_zone)
#define PLOT_REQUEST_X_AXIS_INT(x)  (SS_GET(plot_request, x)->x_axis_int)
#define PLOT_REQUEST_SNAP(x)        (SS_GET(plot_request, x)->snap)
#define PLOT_REQUEST_SNAP_TIMES(x)  (SS_GET(plot_request, x)->snap_times)
#define PLOT_REQUEST_NEXT(x)        (SS_GET(plot_request, x)->next)

#define PLOT_MAP_NAME(x)       (SS_GET(plot_map, x)->name)
#define PLOT_MAP_FALSE(x)      (SS_GET(plot_map, x)->false)
#define PLOT_MAP_INDEX(x)      (SS_GET(plot_map, x)->index)
#define PLOT_MAP_CONV(x)       (SS_GET(plot_map, x)->conv)

#define PLT_CRV_LABEL(x)      (SS_GET(plt_crv, x)->label)
#define PLT_CRV_Y_INDEX(x)    (SS_GET(plt_crv, x)->y_index)
#define PLT_CRV_Y_CONV(x)     (SS_GET(plt_crv, x)->y_conv)
#define PLT_CRV_X_INDEX(x)    (SS_GET(plt_crv, x)->x_index)
#define PLT_CRV_X_CONV(x)     (SS_GET(plt_crv, x)->x_conv)
#define PLT_CRV_PLT_R(x)      (SS_GET(plt_crv, x)->plt_r)
#define PLT_CRV_NPTS(x)       (SS_GET(plt_crv, x)->npts)

/* PGS extensions */

/* GRAPH - contains the information necessary to display a list of functions
 *       - in some fashion
 */

#define GRAPH_F(x)             (SS_GET(PG_graph, x)->f)
#define GRAPH_IDENTIFIER(x)    (SS_GET(PG_graph, x)->identifier)
#define GRAPH_INFO(x)          (SS_GET(PG_graph, x)->info)
#define GRAPH_NEXT(x)          (SS_GET(PG_graph, x)->next)

/* INTERFACE_OBJECT - access PG_interface_object structure members */

#define INTERFACE_OBJECT(x)           (SS_GET(PG_interface_object, x))
#define INTERFACE_OBJECT_NAME(x)      (SS_GET(PG_interface_object, x)->name)
#define INTERFACE_OBJECT_TYPE(x)      (SS_GET(PG_interface_object, x)->type)
#define INTERFACE_OBJECT_DEVICE(x)    (SS_GET(PG_interface_object, x)->device)

/* IMAGE - access image structure members */

#define IMAGE_NAME(x)          (SS_GET(PG_image, x)->label)
#define IMAGE_DATA(x)          (SS_GET(PG_image, x)->bf)
#define IMAGE_TYPE(x)          (SS_GET(PG_image, x)->element_type)
#define IMAGE_KMAX(x)          (SS_GET(PG_image, x)->kmax)
#define IMAGE_LMAX(x)          (SS_GET(PG_image, x)->lmax)
#define IMAGE_XMAX(x)          (SS_GET(PG_image, x)->xmax)
#define IMAGE_XMIN(x)          (SS_GET(PG_image, x)->xmin)
#define IMAGE_YMAX(x)          (SS_GET(PG_image, x)->ymax)
#define IMAGE_YMIN(x)          (SS_GET(PG_image, x)->ymin)
#define IMAGE_ZMAX(x)          (SS_GET(PG_image, x)->zmax)
#define IMAGE_ZMIN(x)          (SS_GET(PG_image, x)->ymin)

#define DEV_ATTRIBUTES_CLIPPING(x)        (SS_GET(PG_dev_attributes, x)->clipping)
#define DEV_ATTRIBUTES_CHAR_FONT(x)       (SS_GET(PG_dev_attributes, x)->char_font)
#define DEV_ATTRIBUTES_CHAR_FRAC(x)       (SS_GET(PG_dev_attributes, x)->char_frac)
#define DEV_ATTRIBUTES_CHAR_HEIGHT(x)     (SS_GET(PG_dev_attributes, x)->char_height)
#define DEV_ATTRIBUTES_CHAR_PRECISION(x)  (SS_GET(PG_dev_attributes, x)->char_precision)
#define DEV_ATTRIBUTES_CHAR_SPACE(x)      (SS_GET(PG_dev_attributes, x)->char_space)
#define DEV_ATTRIBUTES_CHAR_UP_X(x)       (SS_GET(PG_dev_attributes, x)->char_up_x)
#define DEV_ATTRIBUTES_CHAR_UP_Y(x)       (SS_GET(PG_dev_attributes, x)->char_up_y)
#define DEV_ATTRIBUTES_CHAR_WIDTH(x)      (SS_GET(PG_dev_attributes, x)->char_width)
#define DEV_ATTRIBUTES_FILL_COLOR(x)      (SS_GET(PG_dev_attributes, x)->fill_color)
#define DEV_ATTRIBUTES_LINE_COLOR(x)      (SS_GET(PG_dev_attributes, x)->line_color)
#define DEV_ATTRIBUTES_LINE_STYLE(x)      (SS_GET(PG_dev_attributes, x)->line_style)
#define DEV_ATTRIBUTES_LINE_WIDTH(x)      (SS_GET(PG_dev_attributes, x)->line_width)
#define DEV_ATTRIBUTES_LOGICAL_OP(x)      (SS_GET(PG_dev_attributes, x)->logical_op)
#define DEV_ATTRIBUTES_TEXT_COLOR(x)      (SS_GET(PG_dev_attributes, x)->text_color)

#ifdef DEVICE_TYPE
# undef DEVICE_TYPE
#endif

#define DEVICE_AUTODOMAIN(x)             (SS_GET(PG_device, x)->autodomain)
#define DEVICE_AUTOPLOT(x)               (SS_GET(PG_device, x)->autoplot)
#define DEVICE_AUTORANGE(x)              (SS_GET(PG_device, x)->autorange)
#define DEVICE_BACKGROUND_COLOR_WHITE(x) (SS_GET(PG_device, x)->background_color_white)
#define DEVICE_BORDER_WIDTH(x)           (SS_GET(PG_device, x)->border_width)
#define DEVICE_CHAR_FONT(x)              (SS_GET(PG_device, x)->char_font)
#define DEVICE_CHAR_FRAC(x)              (SS_GET(PG_device, x)->char_frac)
#define DEVICE_CHAR_HEIGHT(x)            (SS_GET(PG_device, x)->char_height)
#define DEVICE_CHAR_HEIGHT_S(x)          (SS_GET(PG_device, x)->char_height_s)
#define DEVICE_CHAR_PRECISION(x)         (SS_GET(PG_device, x)->char_precision)
#define DEVICE_CHAR_SPACE(x)             (SS_GET(PG_device, x)->char_space)
#define DEVICE_CHAR_SPACE_S(x)           (SS_GET(PG_device, x)->char_space_s)
#define DEVICE_CHAR_UP_X(x)              (SS_GET(PG_device, x)->char_up_x)
#define DEVICE_CHAR_UP_Y(x)              (SS_GET(PG_device, x)->char_up_y)
#define DEVICE_CHAR_WIDTH(x)             (SS_GET(PG_device, x)->char_width)
#define DEVICE_CHAR_WIDTH_S(x)           (SS_GET(PG_device, x)->char_width_s)
#define DEVICE_CLIPPING(x)               (SS_GET(PG_device, x)->clipping)
#define DEVICE_FILE(x)                   (SS_GET(PG_device, x)->file)
#define DEVICE_FILL_COLOR(x)             (SS_GET(PG_device, x)->fill_color)
#define DEVICE_GPRINT_FLAG(x)            (SS_GET(PG_device, x)->gprint_flag)
#define DEVICE_GRID(x)                   (SS_GET(PG_device, x)->grid)
#define DEVICE_HARD_COPY_DEVICE(x)       (SS_GET(PG_device, x)->hard_copy_device)
#define DEVICE_LINE_STYLE(x)             (SS_GET(PG_device, x)->line_style)
#define DEVICE_LINE_COLOR(x)             (SS_GET(PG_device, x)->line_color)
#define DEVICE_TEXT_COLOR(x)             (SS_GET(PG_device, x)->text_color)
#define DEVICE_LINE_WIDTH(x)             (SS_GET(PG_device, x)->line_width)
#define DEVICE_LOGICAL_OP(x)             (SS_GET(PG_device, x)->logical_op)
#define DEVICE_MARK(x)                   (SS_GET(PG_device, x)->mark)
#define DEVICE_MARKER(x)                 (SS_GET(PG_device, x)->marker)
#define DEVICE_MODE(x)                   (SS_GET(PG_device, x)->mode)
#define DEVICE_NAME(x)                   (SS_GET(PG_device, x)->name)
#define DEVICE_N_CHAR_LINE(x)            (SS_GET(PG_device, x)->n_char_line)
#define DEVICE_NCOLOR(x)                 (SS_GET(PG_device, x)->ncolor)
#define DEVICE_N_LINES_PAGE(x)           (SS_GET(PG_device, x)->n_lines_page)
#define DEVICE_NLINES(x)                 (SS_GET(PG_device, x)->nlines)
#define DEVICE_PS_COLOR(x)               (SS_GET(PG_device, x)->ps_color)
#define DEVICE_QUADRANT(x)               (SS_GET(PG_device, x)->quadrant)
#define DEVICE_SCATTER(x)                (SS_GET(PG_device, x)->scatter)
#define DEVICE_TITLE(x)                  (SS_GET(PG_device, x)->title)
#define DEVICE_TYPE_INDEX(x)             (SS_GET(PG_device, x)->type_index)
#define DEVICE_TXT_RATIO(x)              (SS_GET(PG_device, x)->txt_ratio)
#define DEVICE_TYPE(x)                   (SS_GET(PG_device, x)->type)
#define DEVICE_VIEW_HEIGHT(x)            (SS_GET(PG_device, x)->view_height)
#define DEVICE_BLACK(x)                  (SS_GET(PG_device, x)->BLACK)
#define DEVICE_WHITE(x)                  (SS_GET(PG_device, x)->WHITE)
#define DEVICE_GRAY(x)                   (SS_GET(PG_device, x)->GRAY)
#define DEVICE_DARK_GRAY(x)              (SS_GET(PG_device, x)->DARK_GRAY)
#define DEVICE_BLUE(x)                   (SS_GET(PG_device, x)->BLUE)
#define DEVICE_GREEN(x)                  (SS_GET(PG_device, x)->GREEN)
#define DEVICE_CYAN(x)                   (SS_GET(PG_device, x)->CYAN)
#define DEVICE_RED(x)                    (SS_GET(PG_device, x)->RED)
#define DEVICE_MAGENTA(x)                (SS_GET(PG_device, x)->MAGENTA)
#define DEVICE_BROWN(x)                  (SS_GET(PG_device, x)->BROWN)
#define DEVICE_DARK_BLUE(x)              (SS_GET(PG_device, x)->DARK_BLUE)
#define DEVICE_DARK_GREEN(x)             (SS_GET(PG_device, x)->DARK_GREEN)
#define DEVICE_DARK_CYAN(x)              (SS_GET(PG_device, x)->DARK_CYAN)
#define DEVICE_DARK_RED(x)               (SS_GET(PG_device, x)->DARK_RED)
#define DEVICE_YELLOW(x)                 (SS_GET(PG_device, x)->YELLOW)
#define DEVICE_DARK_MAGENTA(x)           (SS_GET(PG_device, x)->DARK_MAGENTA)

/* PML Extensions */

#define NUMERIC_ARRAY(x)            (SS_GET(C_array, x))
#define NUMERIC_ARRAY_TYPE(x)       (SS_GET(C_array, x)->type)
#define NUMERIC_ARRAY_LENGTH(x)     (SS_GET(C_array, x)->length)
#define NUMERIC_ARRAY_DATA(x)       (SS_GET(C_array, x)->data)

#define SET_NAME(x)                 (SS_GET(PM_set, x)->name)
#define SET_ELEMENT_TYPE(x)         (SS_GET(PM_set, x)->element_type)
#define SET_N_ELEMENTS(x)           (SS_GET(PM_set, x)->n_elements)
#define SET_DIMENSION(x)            (SS_GET(PM_set, x)->dimension)
#define SET_MAX_INDEX(x)            (SS_GET(PM_set, x)->max_index)
#define SET_ELEMENTS(x)             (SS_GET(PM_set, x)->elements)
#define SET_ES_TYPE(x)              (SS_GET(PM_set, x)->es_type)
#define SET_EXTREMA(x)              (SS_GET(PM_set, x)->extrema)
#define SET_SCALES(x)               (SS_GET(PM_set, x)->scales)
#define SET_OPERS(x)                (SS_GET(PM_set, x)->opers)
#define SET_METRIC(x)               (SS_GET(PM_set, x)->metric)
#define SET_SYMMETRY_TYPE(x)        (SS_GET(PM_set, x)->symmetry_type)
#define SET_SYMMETRY(x)             (SS_GET(PM_set, x)->symmetry)
#define SET_TOPOLOGY_TYPE(x)        (SS_GET(PM_set, x)->topology_type)
#define SET_TOPOLOGY(x)             (SS_GET(PM_set, x)->topology)
#define SET_INFO_TYPE(x)            (SS_GET(PM_set, x)->info_type)
#define SET_INFO(x)                 (SS_GET(PM_set, x)->info)

#define MAPPING_NAME(x)             (SS_GET(PM_mapping, x)->name)
#define MAPPING_DOMAIN(x)           (SS_GET(PM_mapping, x)->domain)
#define MAPPING_RANGE(x)            (SS_GET(PM_mapping, x)->range)
#define MAPPING_MAP_TYPE(x)         (SS_GET(PM_mapping, x)->map_type)
#define MAPPING_MAP(x)              (SS_GET(PM_mapping, x)->map)
#define MAPPING_FILE_TYPE(x)        (SS_GET(PM_mapping, x)->file_type)
#define MAPPING_FILE_INFO(x)        (SS_GET(PM_mapping, x)->file_info)
#define MAPPING_FILE(x)             (SS_GET(PM_mapping, x)->file)
#define MAPPING_NEXT(x)             (SS_GET(PM_mapping, x)->next)

/*--------------------------------------------------------------------------*/

/*                           PREDICATE MACROS                               */

/*--------------------------------------------------------------------------*/
 
/* SX Types */

#define SX_FUNCTIONP(obj)         (SS_OBJECT_TYPE(obj) == G_FUNCTION)
#define SX_FILEP(obj)             (SS_OBJECT_TYPE(obj) == G_FILE)

/* PDBLib Types */

#define SX_DEFSTRP(obj)           (SS_OBJECT_TYPE(obj) == G_DEFSTR)
#define SX_SYMENTP(obj)           (SS_OBJECT_TYPE(obj) == G_SYMENT)
#define SX_PDBDATAP(obj)          (SS_OBJECT_TYPE(obj) == G_PDBDATA)

/* PANACEA Types */

#define SX_PANVARP(obj)           (SS_OBJECT_TYPE(obj) == G_PANVAR)
#define SX_PACKAGEP(obj)          (SS_OBJECT_TYPE(obj) == G_PACKAGE)
#define SX_SOURCE_VARIABLEP(obj)  (SS_OBJECT_TYPE(obj) == G_SOURCE_VARIABLE)
#define SX_IV_SPECIFICATIONP(obj) (SS_OBJECT_TYPE(obj) == G_IV_SPECIFICATION)
#define SX_PLOT_REQUESTP(obj)     (SS_OBJECT_TYPE(obj) == G_PLOT_REQUEST)
#define SX_PLOT_MAPP(obj)         (SS_OBJECT_TYPE(obj) == G_PLOT_MAP)
#define SX_PLT_CRVP(obj)          (SS_OBJECT_TYPE(obj) == G_PLT_CRV)

/* PGS Types */

#define SX_GRAPHP(obj)            (SS_OBJECT_TYPE(obj) == G_GRAPH)
#define SX_IMAGEP(obj)            (SS_OBJECT_TYPE(obj) == G_IMAGE)
#define SX_INTERFACE_OBJECTP(obj) (SS_OBJECT_TYPE(obj) == G_INTERFACE_OBJECT)
#define SX_DEVICEP(obj)           (SS_OBJECT_TYPE(obj) == G_DEVICE)
#define SX_DEV_ATTRIBUTESP(obj)   (SS_OBJECT_TYPE(obj) == G_DEV_ATTRIBUTES)

/* PML Types */

#define SX_NUMERIC_ARRAYP(obj)    (SS_OBJECT_TYPE(obj) == G_NUM_ARRAY)
#define SX_POLYGONP(obj)          (SS_OBJECT_TYPE(obj) == G_POLYGON)
#define SX_MAPPINGP(obj)          (SS_OBJECT_TYPE(obj) == G_MAPPING)
#define SX_SETP(obj)              (SS_OBJECT_TYPE(obj) == G_SET)

/*--------------------------------------------------------------------------*/

/*                           STRUCT DEFINITIONS                             */

/*--------------------------------------------------------------------------*/

typedef struct s_g_pdbdata g_pdbdata;
typedef struct s_SX_object SX_object;
typedef struct s_SX_menu_item SX_menu_item;
typedef struct s_g_file g_file;
typedef struct s_out_device out_device;

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

struct s_g_file
   {char *name;
    char *type;                                       /* file type SX knows */
    void *file;
    object *file_object;
    SC_array *menu_lst;
    struct s_g_file *next;};
    
struct s_out_device
   {int exist;
    int active;
    int background_color;
    char *dname;               /* device name lower case */
    char *dupp;                /* device name upper case */
    double x0;
    double y0;
    double width;
    double height;
    char *fname;               /* file name */
    char *type;
    PG_device *dev;};

/* SCHEME object type designations */

enum e_SX_object_type
   {G_FILE = 240,
    G_DEFSTR,
    G_SYMENT,
    G_DIMDES,
    G_MEMDES,
    G_PDBDATA,
    G_PANVAR,
    G_PACKAGE,
    G_SOURCE_VARIABLE,
    G_IV_SPECIFICATION,
    G_PLOT_REQUEST,
    G_PLOT_MAP,
    G_PLT_CRV,
    G_GRAPH,
    G_DEVICE,
    G_DEV_ATTRIBUTES,
    G_NUM_ARRAY,
    G_POLYGON,
    G_MAPPING,
    G_SET,
    G_FUNCTION,
    G_IMAGE,
    G_INTERFACE_OBJECT};

typedef enum e_SX_object_type SX_object_type;

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                        VARIABLE DECLARATIONS                             */

/*--------------------------------------------------------------------------*/

extern data_standard
 *PDB_STANDARD;

extern FILE
 *SX_command_log;

extern PDBfile
 *SX_vif;

extern curve
 *SX_dataset;

extern g_file
 *SX_file_list,
 *SX_gvif;

extern object
 *_SX_var_tab,
 *SX_curfile,
 *SX_ovif;

extern PFVoid
 SX_plot_hook;

extern PFInt
 SX_pan_data_hook;

extern int
 *SX_data_index,
 SX_disp_individ_diff,
 SX_default_npts,
 SX_prefix_list[],
 SX_autorange,
 SX_autodomain,
 SX_autoplot,
 SX_background_color_white,
 SX_border_width,
 SX_data_id,
 SX_default_color,
 SX_GRI_type_size,
 SX_gr_mode,
 SX_grid,
 *SX_number,
 SX_N_Curves,
 SX_n_curves_read,
 _SX_next_available_number,
 SX_plot_flag,
 SX_plot_type_size,
 SX_qflag,
 SX_show_mouse_location,
 SX_log_scale[];

extern double
 SX_chi,
 SX_console_x,
 SX_console_y,
 SX_console_width,
 SX_console_height,
 SX_gwc[],
 SX_gpad[],
 SX_interp_power,
 SX_interp_strength,
 SX_interp_scale,
 SX_window_x[],
 SX_window_P[],
 SX_window_width,
 SX_window_width_P,
 SX_window_height,
 SX_window_height_P,
 SX_GRI_x,
 SX_GRI_y,
 SX_GRI_dx,
 SX_GRI_dy,
 SX_marker_orientation,
 SX_phi,
 SX_show_mouse_location_x,
 SX_show_mouse_location_y,
 SX_theta,
 SX_view_x[],
 SX_view_width,
 SX_view_height,
 SX_view_aspect;

extern char
 *SX_text_output_format,
 *SX_command_log_name,
 *SX_console_type,
 *SX_current_palette,
 SX_data_directory[],
 *SX_display_name,
 *SX_display_type,
 *SX_display_title,
 SX_err[],                      /* global buffer for certain error messages */
 *SX_GRI_title,
 *SX_GRI_type_face,
 *SX_GRI_type_style,
 *SX_plot_type_style,
 *SX_promotion_type,
 *SX_pui_file,
 *SX_smooth_method;

extern PG_rendering
 SX_render_def,
 SX_render_1d_1d,
 SX_render_2d_1d,
 SX_render_2d_2d,
 SX_render_3d_1d;

extern FILE
 *SX_out_text,
 *SX_out_bin;

extern PDBfile
 *SX_out_pdb;
    
extern PG_device
 *SX_graphics_device;

/*--------------------------------------------------------------------------*/

/*                        FUNCTION DECLARATIONS                             */

/*--------------------------------------------------------------------------*/

/* SXCMD.C declarations */

extern void
 SX_end(int val),
 SX_init_view(void),
 SX_init_mappings(void),
 SX_init_env(void);

extern int
 SX_rd_scm(char *name),
 SX_command(char *file, char *cmd);


/* SXCONT.C declarations */

extern int
 SX_split_command(char *cmd, char *lst),
 SX_expand_expr(char *s);

extern char
 *SX_wrap_paren(char *open, char *form, char *close, size_t ln);

extern void
 SX_load_rc(char *ffn, int ldrc, char *ifna, char *ifnb),
 SX_parse(void (*replot)(void), char *(*reproc)(char *s), object *strm),
 SX_init_device_vars(int idev, double xf, double yf, double wd, double hg),
 SX_install_global_vars(void);

extern out_device
 *SX_get_device(int idev),
 *SX_match_device(PG_device *dev);


/* SXCRV.C declarations */

extern object
 *SX_get_curve_obj(int j),
 *SX_get_curve_proc(int j),
 *SX_get_curve_var(int j),
 *SX_mk_curve_proc(int i),
 *SX_mk_curve(int na, double *xa, double *ya, 
	      char *label, char *filename, PFVoid fnc),
 *SX_set_crv_id(int i, char *id),
 *SX_re_id(void),
 *SX_get_data_domain(object *argl),
 *SX_get_data_range(object *argl),
 *SX_rl_curve(int j);

extern void
 SX_zero_curve(int i),
 SX_assign_next_id(int i, PFVoid fnc),
 SX_enlarge_dataset(PFVoid eval);

extern int
 SX_get_data_index(char *s),
 SX_get_crv_index_i(object *obj),
 SX_get_curve_id(char *s),
 SX_get_crv_index_j(object *obj),
 SX_curvep(char *s),
 SX_next_space(void),
 SX_curve_id(object *c),
 SX_curvep_b(object *obj),
 SX_curvep_a(object *obj);


/* SXFUNC.C declarations */

extern void
 SX_quit(int i),
 SX_filter_coeff(double *yp, int n, C_array *arr, int ntimes),
 SX_install_global_funcs(void);

extern object
 *SX_plane(object *argl);

extern int
 SX_map_count(void);


/* SXGRI.C declarations */

extern object
 *SX_mk_iob(PG_interface_object *iob);

extern void
 SX_end_prog(void *d, PG_event *ev),
 SX_rem_iob(PG_interface_object *iob, int flag);


/* SXGROT.C declarations */

extern object
 *SX_draw_grotrian(object *argl);


/* SXHAND.C declarations */

extern object
 *SX_display_map(object *mo);

extern void
 SX_determine_drw_obj(PM_mapping **pf, object **po, object **pargl),
 SX_determine_mapping(PM_mapping **pf, object **pargl),
 SX_mf_install(void);

extern int
 SX_have_display_list(void);


/* SXHBO.C declarations */

extern PM_mapping
 *SX_build_return_mapping(PM_mapping *h, char *label,
			  PM_set *domain, int init, int wgt);


/* SXHOOK.C declarations */

extern int
 SX_fprintf(FILE *fp, char *fmt, ...),
 SX_fputs(char *s, FILE *fp);


/* SXIO.C declarations */

extern object
*SX_print(object *argl);


/* SXMM.C declarations */

extern object
 *SX_mk_gfile(g_file *po);


/* SXMODE.C declarations */

extern void
 SX_setup_viewspace(PG_device *dev, double mh);

extern object
 *SX_mode_text(void),
 *SX_mode_graphics(void);


/* SXPAN.C declarations */

extern void
 SX_install_panacea_funcs(void);

extern object
 *SX_mk_package(PA_package *pck),
 *SX_mk_variable(PA_variable *pp);


/* SXPDB.C declarations */

extern void
 SX_install_pdb_funcs(void);

extern int
 SX_pdbfilep(object *arg),
 SX_ipdbfilep(object *arg),
 SX_convert(char *dtype, void **pd, char *stype, void *s, int n, int flag);

extern object
 *SX_get_pdbfile(object *argl, PDBfile **pfile, g_file **gfile),
 *SX_get_file(object *argl, g_file **pfile),
 *SX_pdbdata_handler(PDBfile *file, char *name,
		     char *type, void *vr, int flag);


/* SXPDBA.C declarations */

extern void
 SX_install_pdb_attr_funcs(void);


/* SXPGS.C declarations */

extern object
 *SX_mk_graph(PG_graph *g),
 *SX_mk_image(PG_image *im),
 *SX_mk_graphics_device(PG_device *dev),
 *SX_mk_dev_attributes(PG_dev_attributes *da),
 *SX_get_ref_map(g_file *po, int indx, char *dtype);

extern pcons
 *SX_set_attr_alist(pcons *inf, char *name, char *type, object *val);

extern int
 SX_next_color(PG_device *dev);

extern void
 SX_install_pgs_funcs(void),
 SX_expose_event_handler(PG_device *dev, PG_event *ev),
 SX_update_event_handler(PG_device *dev, PG_event *ev),
 SX_motion_event_handler(PG_device *dev, PG_event *ev),
 SX_mouse_event_handler(PG_device *dev, PG_event *ev),
 SX_default_event_handler(PG_device *dev, PG_event *ev);


/* SXPML.C declarations */

extern void
 SX_install_pml_funcs(void);

extern object
 *SX_setp(object *obj),
 *SX_list_array(object *argl),
 *SX_mk_set(PM_set *set),
 *SX_mk_mapping(PM_mapping *f),
 *SX_mk_C_array(C_array *arr);


/* SXSET.C declarations */

extern object
 *SX_arg_prep(object *argl);

extern void
 SX_init(char *code, char *vers),
 SX_reset_prefix(void),
 SX_install_funcs(void);

extern int
 SX_next_prefix(void);


/* SXULIO.C declarations */

extern object
 *SX_read_data(object *obj),
 *SX_read_ver1(object *obj),
 *SX_read_text_table(object *argl),
 *SX_table_curve(object *argl),
 *SX_table_attr(void),
 *SX_crv_file_info(object *obj),
 *SX_write_data(object *argl);

extern void
 SX_cache_addpid(),
 SX_uncache_curve(curve *crv),
 SX_push_open_file(FILE *fp),
 SX_close_open_files(void);


#ifdef __cplusplus
}
#endif

#endif

