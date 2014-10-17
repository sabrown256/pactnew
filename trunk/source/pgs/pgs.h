/*
 * PGS.H - header in support of Portable Graphics System
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_PGS

#include "cpyright.h"

#define PCK_PGS                               /* graphics package resident */

#define PGS_TOL 1.0e-10

#include "pdb.h"

#ifndef PACT_INCLUDES

# include <pdb.h>

#endif

#undef PACT_INCLUDES

#ifdef HAVE_OGL
# ifndef HAVE_X11
#  define HAVE_X11
# endif
# undef SIZEOF
# include <GL/glx.h>
# include <GL/gl.h>
# undef SIZEOF
# define SIZEOF (*SC_gs.size)
#endif

#include "scope_x11.h"

/*--------------------------------------------------------------------------*/

/*                            DEFINED CONSTANTS                             */

/*--------------------------------------------------------------------------*/

#define PG_SPACEDM            PM_SPACEDM /* maximum dimension of view space */
#define PG_BOXSZ              6          /* 2*PG_SPACEDM */
#define PG_NANGLES            3          /* number of Euler angles */

#define N_COLORS              16
#define N_IOB_FLAGS           3
#define N_ANGLES              180

#define TXSPAN                80.0

#define PG_IMAGE_VERSION      0

#ifndef MAXPIX
# define MAXPIX 65535
#endif

#define PG_TEXT_OBJECT_S         PG_gs.tnames[0]
#define PG_VARIABLE_OBJECT_S     PG_gs.tnames[1]
#define PG_BUTTON_OBJECT_S       PG_gs.tnames[2]
#define PG_CONTAINER_OBJECT_S    PG_gs.tnames[3]
#define PG_N_TYPES               4

/*--------------------------------------------------------------------------*/

/*                            PROCEDURAL MACROS                             */

/*--------------------------------------------------------------------------*/

/* COLOR_POSTSCRIPT_DEVICE - TRUE iff dev is a color PostScript device */

#define COLOR_POSTSCRIPT_DEVICE(dev)                                         \
    (dev->ps_color == TRUE)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MONOCHROME_POSTSCRIPT_DEVICE - TRUE iff dev is a monochrome PostScript device */

#define MONOCHROME_POSTSCRIPT_DEVICE(dev)                                    \
    (dev->ps_color == FALSE)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POSTSCRIPT_DEVICE - TRUE iff dev is a PostScript device */

#define POSTSCRIPT_DEVICE(dev) (dev->type_index == PS_DEVICE)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CGM_DEVICE - TRUE iff dev is a CGM device */

#define CGM_DEVICE(dev) (dev->type_index == CGMF_DEVICE)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HARDCOPY_DEVICE - TRUE iff dev is a hard copy device */

#define HARDCOPY_DEVICE(dev) (dev->hard_copy_device)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CALL_HANDLER - call the registered event handler with language
 *                   appropriate arguments
 */

#define PG_CALL_HANDLER(hndler, dvice, evnt)                                 \
    {if ((hndler).lang == _C_LANG)                                           \
       {(*(hndler).fnc)(dvice, &evnt);}                                      \
    else if ((hndler).lang == _FORTRAN_LANG)                                 \
       {PFFMouseHand ffcn = (PFFMouseHand) (hndler).fnc;                     \
        FIXNUM fdvice, fevnt;                                                \
        fdvice = SC_GET_INDEX(dvice);                                        \
        fevnt  = SC_ADD_POINTER(&evnt);                                      \
        (*ffcn)(&fdvice, &fevnt);}}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_ADJUST_LIMITS_3D - adjust limits for 3d plots and axes */

#define PG_ADJUST_LIMITS_3D(_bi, _bo)                                        \
    {int _i;                                                                 \
     double _d;                                                              \
     for (_i = 0; _i < 6; _i += 2)                                           \
         {_d = (_bi[_i+1] - _bi[_i])*TOLERANCE;                              \
          _bo[_i]   = _bi[_i] - _d;                                          \
	  _bo[_i+1] = _bi[_i+1] + _d;};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_WITH_CLIP - gather simple clipping info while copying points */

#define PG_SET_WITH_CLIP_INIT(_ok, _os)                                      \
   {_ok = FALSE;                                                             \
    _os = -2;}

#define PG_SET_WITH_CLIP(_xo, _xi, _ok, _os, _mn, _mx)                       \
   {int _s;                                                                  \
    double _v;                                                               \
    _v = _xi;                                                                \
    if (!_ok)                                                                \
       {if (_v < _mn)                                                        \
	   _s = -1;                                                          \
        else if (_mx < _v)                                                   \
	   _s = 1;                                                           \
	else                                                                 \
	   _s = 0;                                                           \
	_ok |= ((_s == 0) || ((_os != -2) && (_s != _os)));                  \
	_os  = _s;};                                                         \
    _xo = _v;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_PUSH_CHILD_IOB - add a child interface object to a parent's list */

#define PG_PUSH_CHILD_IOB(par, chld)                                         \
   {PG_interface_object *_piob;                                              \
    _piob = (par);                                                           \
    SC_array_push(_piob->children, &chld);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef EVENT_DEFINED
# define PG_EVENT_TYPE(ev)   (ev).type
#endif

#define KEY_ACTION(b, bf, nc) PG_key_action(b, bf, nc)

#define PG_clear_screen       PG_clear_window

#define PG_window_width(_d)     abs((_d)->g.hwin[1] - (_d)->g.hwin[0])
#define PG_window_height(_d)    abs((_d)->g.hwin[3] - (_d)->g.hwin[2])

#define PG_luminance(_r, _g, _b) (0.2988*(_r) + 0.5869*(_g) + 0.1143*(_b) + 0.5)

#define PG_true_color(_r, _g, _b)                                            \
    ((0x7f) << 24 +                                                          \
     ((_r) & 0xff) << 16 +                                                   \
     ((_g) & 0xff) << 8 +                                                    \
     ((_b) & 0xff))

#define PG_is_true_color(_c)   (((0x7f << 24) & (_c)) != 0)

/*--------------------------------------------------------------------------*/
 
/*                        TYPEDEF'S AND STRUCT'S                            */
 
/*--------------------------------------------------------------------------*/

typedef struct s_PG_axis_tick_def PG_axis_tick_def;
typedef struct s_PG_axis_def PG_axis_def;
typedef struct s_PG_graph PG_graph;
typedef PG_graph *(*PFPPG_graph)(void);
typedef struct s_PG_marker PG_marker;
typedef struct s_PG_textdes PG_textdes;
typedef struct s_PG_font_family PG_font_family;
typedef struct s_PG_curve PG_curve;
typedef struct s_PG_interface_object PG_interface_object;
typedef PG_interface_object *(*PFPPG_interface_object)(void);
typedef struct s_PG_device PG_device;
typedef PG_device *(*PFPPG_device)(void);
typedef struct s_PG_palette PG_palette;
typedef struct s_PG_picture_desc PG_picture_desc;
typedef struct s_PG_image PG_image;
typedef struct s_PG_par_rend_info PG_par_rend_info;
typedef struct s_PG_view_attributes PG_view_attributes;
typedef struct s_PG_dev_attributes PG_dev_attributes;
typedef struct s_PG_dev_geometry PG_dev_geometry;
typedef struct s_curve curve;
typedef struct s_PG_text_box PG_text_box;
typedef struct s_PG_event_handler PG_event_handler;
typedef struct s_PG_scope_public PG_scope_public;

typedef int (*PFRDev)(PG_device *dev);
typedef void (*PFKeymap)(PG_text_box *b);

/*
 * #bind derived PG_coord_sys integer SC_ENUM_I SC_ENUM_I WORLDC
 */

enum e_PG_coord_sys
   {WORLDC = 1,
    NORMC,
    PIXELC,
    BOUNDC,
    FRAMEC};

typedef enum e_PG_coord_sys PG_coord_sys;

enum e_PG_dev_type
   {TEXT_WINDOW_DEVICE = 128,             /* Quickdraw requires distinction */
    GRAPHIC_WINDOW_DEVICE,                                 /* Screen Window */
    PS_DEVICE,                                                /* PostScript */
    CGMF_DEVICE,                                                     /* CGM */
    PNG_DEVICE,                                                      /* PNG */
    JPEG_DEVICE,                                                    /* JPEG */
    MPEG_DEVICE,                                                    /* MPEG */
    HARD_COPY_DEVICE,                                /* undifferentiated HC */
    IMAGE_DEVICE,                                           /* IMAGE device */
    RASTER_DEVICE};                                        /* RASTER device */

typedef enum e_PG_dev_type PG_dev_type;

/*
 * #bind derived PG_color integer SC_ENUM_I SC_ENUM_I ROTATING
 */

enum e_PG_color
   {CLEAR = -2, ROTATING = -1,
    WHITE, BLACK, GRAY, DARK_GRAY,
    BLUE, GREEN, CYAN, RED, MAGENTA, BROWN,
    DARK_BLUE, DARK_GREEN, DARK_CYAN, DARK_RED, YELLOW, DARK_MAGENTA};

typedef enum e_PG_color PG_color;

/*
 * #bind derived PG_rendering integer SC_ENUM_I SC_ENUM_I PLOT_NONE
 */

enum e_PG_rendering
   {PLOT_NONE = 0,
    PLOT_CURVE = 10,
    PLOT_CONTOUR,
    PLOT_IMAGE,
    PLOT_WIRE_MESH,
    PLOT_SURFACE,
    PLOT_VECTOR,
    PLOT_FILL_POLY,
    PLOT_MESH,
    PLOT_ERROR_BAR,
    PLOT_SCATTER,
    PLOT_DV_BND,
    PLOT_DEFAULT,
    PLOT_CARTESIAN,
    PLOT_POLAR,
    PLOT_INSEL,
    PLOT_HISTOGRAM,
    PLOT_LOGICAL};

typedef enum e_PG_rendering PG_rendering;

/*
 * #bind derived PG_axis_attr SC_ENUM_I SC_ENUM_I AXIS_LINESTYLE
 */

enum e_PG_axis_attr
   {AXIS_LINESTYLE = 100,
    AXIS_LINETHICK,
    AXIS_LINECOLOR,
    AXIS_LABELCOLOR,
    AXIS_LABELSIZE,
    AXIS_LABELFONT,
    AXIS_LABELPREC,
    AXIS_X_FORMAT,
    AXIS_Y_FORMAT,
    AXIS_TICKSIZE,
    AXIS_GRID_ON,
    AXIS_SIGNIF_DIGIT,
    AXIS_CHAR_ANGLE,
    AXIS_TICK_RIGHT,
    AXIS_TICK_LEFT,
    AXIS_TICK_STRADDLE,
    AXIS_TICK_ENDS,
    AXIS_TICK_MAJOR = 1,   /* these are used a flags which can be OR'd together */
    AXIS_TICK_MINOR = 2,
    AXIS_TICK_LABEL = 4,
    AXIS_TICK_NONE  = 8};

typedef enum e_PG_axis_attr PG_axis_attr;

/*
 * #bind derived PG_axis_type SC_ENUM_I SC_ENUM_I CARTESIAN_2D
 */

enum e_PG_axis_type
   {CARTESIAN_2D = -1,
    CARTESIAN_3D = -2,
    POLAR        = -3,
    INSEL        = -4};

typedef enum e_PG_axis_type PG_axis_type;

enum e_PG_grid_attr
   {GRID_LINESTYLE = 1,
    GRID_LINETHICK,
    GRID_LINECOLOR};

typedef enum e_PG_grid_attr PG_grid_attr;

/*
 * #bind derived PG_line_attr SC_ENUM_I SC_ENUM_I LINE_SOLID
 */

enum e_PG_line_attr
   {LINE_SOLID = 1,
    LINE_DASHED,
    LINE_DOTTED,
    LINE_DOTDASHED,
    LINE_NONE};

typedef enum e_PG_line_attr PG_line_attr;

/*
 * #bind derived PG_vector_attr SC_ENUM_I SC_ENUM_I VEC_SCALE
 */

enum e_PG_vector_attr
   {VEC_SCALE = 1,
    VEC_ANGLE,
    VEC_HEADSIZE,
    VEC_FIXSIZE,
    VEC_MAXSIZE,
    VEC_LINESTYLE,
    VEC_LINETHICK,
    VEC_COLOR,
    VEC_FIXHEAD};

typedef enum e_PG_vector_attr PG_vector_attr;

/*
 * #bind derived PG_mouse_button integer SC_ENUM_I SC_ENUM_I MOUSE_NONE
 */

enum e_PG_mouse_button
   {MOUSE_NONE       = 0,
    MOUSE_LEFT       = 1,
    MOUSE_MIDDLE     = 2,
    MOUSE_RIGHT      = 4,
    MOUSE_WHEEL_UP   = 8,
    MOUSE_WHEEL_DOWN = 16,
    MOUSE_SOFTWARE   = 10,
    MOUSE_HARDWARE   = 11};

typedef enum e_PG_mouse_button PG_mouse_button;

/*
 * #bind derived PG_key_modifier integer SC_ENUM_I SC_ENUM_I KEY_SHIFT
 */

enum e_PG_key_modifier
    {KEY_SHIFT = 0x10,
     KEY_CNTL  = 0x20,
     KEY_ALT   = 0x40,
     KEY_LOCK  = 0x80};

typedef enum e_PG_key_modifier PG_key_modifier;

enum e_PG_iob_tag
   {PG_IOB_NAME = 1,
    PG_IOB_CLR  = 2,
    PG_IOB_FLG  = 3,
    PG_IOB_DRW  = 4,
    PG_IOB_ACT  = 5,
    PG_IOB_SEL  = 6,
    PG_IOB_OBJ  = 7,
    PG_IOB_END  = 8};

typedef enum e_PG_iob_tag PG_iob_tag;

enum e_PG_iob_state
   {PG_IsVis = 0,
    PG_IsSel = 1,
    PG_IsAct = 2};

typedef enum e_PG_iob_state PG_iob_state;

enum e_PG_clear_mode
  {CLEAR_SCREEN   = -5,
   CLEAR_VIEWPORT = -6,
   CLEAR_FRAME    = -7};

typedef enum e_PG_clear_mode PG_clear_mode;

enum e_PG_orientation
   {HORIZONTAL     = -10,
    VERTICAL       = -11,
    PORTRAIT_MODE  =  10,
    LANDSCAPE_MODE =  11};

typedef enum e_PG_orientation PG_orientation;

enum e_PG_PS_primitive
   {PG_TEXT   = 1,
    PG_FILL   = 2,
    PG_LINE   = 3,
    PG_NOPRIM = -1};

typedef enum e_PG_PS_primitive PG_PS_primitive;

enum e_PG_text_window_attr
   {MORE_WINDOW              = -1,
    SCROLLING_WINDOW         = -2,
    TEXT_INSERT              = -3,
    TEXT_OVERWRITE           = -4,
    TEXT_CHARACTER_PRECISION = 1};

typedef enum e_PG_text_window_attr PG_text_window_attr;

enum e_PG_quadrant
   {QUAD_ONE  = 1,
    QUAD_FOUR = 4};

typedef enum e_PG_quadrant PG_quadrant;

enum e_PG_angle_unit
   {DEGREE = 1,
    RADIAN = 2};

typedef enum e_PG_angle_unit PG_angle_unit;

/*
 * #bind derived PG_logical_operation integer SC_ENUM_I SC_ENUM_I GS_COPY
 */

enum e_PG_logical_operation
   {GS_COPY = 3,
    GS_XOR  = 6};

typedef enum e_PG_logical_operation PG_logical_operation;

enum e_PG_lang_binding
   {_C_LANG       = 1,
    _FORTRAN_LANG = 2};

typedef enum e_PG_lang_binding PG_lang_binding;

/*--------------------------------------------------------------------------*/

struct s_curve
   {char *text;                            /* text associated with the data */
    int modified;                          /* flag indicating changed curve */
    void *obj;                       /* pointer to the encapsulating object */
    double wc[PG_BOXSZ];
    int n;                                /* number of data points in array */
    double *x[PG_SPACEDM];                       /* point coordinate arrays */
    pcons *info;
    void *file_info;           /* file info - cast to some struct with info */
    SC_file_type file_type;                 /* file type ASCII, BINARY, PDB */
    char *file;                                      /* file name for curve */
    unsigned int id;};                                 /* letter identifier */
     
/*--------------------------------------------------------------------------*/


/* PG_AXIS_TICK_DEF - axis tick definition information */

struct s_PG_axis_tick_def
   {int n;
    int ndiv;        /* number of divisions - size of dx */
    int log_scale;
    double space;
    double en[2];
    double vo[2];    /* the extremal label value */
    double *dx;};    /* DX is ordered from min to max - normalized (0, 1) */


/* PG_AXIS_DEF - axis definition information */

struct s_PG_axis_def
   {int axis_type;
    int tick_type;
    int label_type;
    int log_scale[PG_SPACEDM];

    double x0[PG_SPACEDM];  /* axis vector origin */
    double dr;              /* axis vector length */
    double cosa;            /* axis vector direction cosines */
    double sina;

    double tn[2];  /* normalized interval along axis vector
                    * tn[0] < tn[1] means outward and
                    * tn[0] > tn[1] means inward */
    double vo[2];  /* extremal label value */

    double scale;
    double scale_x[PG_SPACEDM];
    double eps_rel;

    PG_axis_tick_def tick[3];};

/*--------------------------------------------------------------------------*/

/* GRAPH - contains the information necessary to display a list of functions
 *       - in some fashion
 */

struct s_PG_graph
   {PM_mapping *f;
    PG_rendering rendering;
    int mesh;
    char *info_type;                       /* data type for the info member */
    void *info;        /* attribute information for the rendering algorithm */
    int identifier;
    char *use;                          /* volatile communicator internally */
    void (*render)(PG_device *dev, PG_graph *data, ...);
    struct s_PG_graph *next;};

/*--------------------------------------------------------------------------*/

/* MARKER - line segment description of a marker character */

struct s_PG_marker
   {int n_seg;
    double *x1;
    double *y1;
    double *x2;
    double *y2;};

/*--------------------------------------------------------------------------*/

/* TEXTDES - describe text with font, color, alignment, and so on */

struct s_PG_textdes
   {int color;              /* text color */
    int size;               /* point size */
    char *style;            /* style - bold, italic, ... */
    char *face;             /* font name */
    double angle;           /* angle from horizontal */
    PM_direction align;     /* alignment within text_box */
    char *other;};          /* any system dependent info */

/*--------------------------------------------------------------------------*/

/* FONT_FAMILY - info for a family of fonts
 *             - the canonical set of styles is (in order):
 *             -
 *             - Normal
 *             - Italic
 *             - Bold
 *             - Bold Italic
 *             -
 *             - others may be defined, but these should be here always
 */

struct s_PG_font_family
   {int n_styles;
    char *type_face;
    char **type_styles;
    struct s_PG_font_family *next;};

/*--------------------------------------------------------------------------*/

/* declare RGB color map */

struct s_RGB_color_map
   {double red;
    double green;
    double blue;};

typedef struct s_RGB_color_map RGB_color_map;

/*--------------------------------------------------------------------------*/

/* palette structure
 * #bind derived PG_palette character-A SC_STRING_I PyObject NULL
 */

struct s_PG_palette
   {char *name;
    int max_pal_dims;                 /* number of different palette shapes */
    int **pal_dims;            /* shapes for 1, 2, ... dimensional palettes */
    int n_pal_colors;
    int n_dev_colors;
    double max_red_intensity;
    double max_green_intensity;
    double max_blue_intensity;
    RGB_color_map *pseudo_colormap;
    RGB_color_map *true_colormap;
    unsigned long *pixel_value;

#ifdef MAC_COLOR
    PaletteHandle hpalette;
#endif

    struct s_PG_palette *next;};

/*--------------------------------------------------------------------------*/

#ifndef EVENT_DEFINED

typedef struct s_PG_event PG_event;

struct s_PG_event
   {int type;};

#endif

/*--------------------------------------------------------------------------*/

typedef int (*PFGetEvent)(PG_event *ev);
typedef PG_device *(*PFEventDevice)(PG_event *ev);
typedef int (*PFOpenConsole)(char *title, char *type, int bckgr,
                             double xf, double yf, double dxf, double dyf);
typedef void (*PFEventHand)(PG_device *dev, PG_event *ev);
typedef void (*PFMouseHand)(PG_device *dev, PG_event *ev);
typedef void (*PFFMouseHand)(FIXNUM *fdvice, FIXNUM *fevnt);

typedef void (*PFEvCallback)(void *d, PG_event *ev);
typedef void (*PFIobDraw)(PG_interface_object *iob);
typedef void (*PFIobAction)(PG_interface_object *iob, PG_event *ev);
typedef PG_interface_object *(*PFIobSelect)(PG_interface_object *iob, PG_event *ev);

/*--------------------------------------------------------------------------*/

/* PG_CURVE - this is a structure containing a curve (line)
 *          - the points are in pixel coordinates which makes
 *          - plotting operations faster
 *          - it is useful for defining boundaries
 */

struct s_PG_curve
   {int n;
    int closed;
    int x_origin;
    int y_origin;
    int *x;
    int *y;
    double rx_origin;
    double ry_origin;};

/*--------------------------------------------------------------------------*/

struct s_PG_interface_object
   {PG_device *device;
    PG_curve *curve;
    char *name;
    char *type;
    void *obj;
    int foreground;
    int background;
    int is_visible;
    int is_active;
    int is_selectable;

    char *draw_name;
    char *action_name;
    char *select_name;

    PFIobDraw draw;
    PFIobAction action;
    PFIobSelect select;

    PG_interface_object *parent;
    SC_array *children;};

/*--------------------------------------------------------------------------*/

struct s_PG_text_box
   {char *name;
    int type;
    int active;
    PG_device *dev;
    PG_curve *bnd;
    int n_lines;
    int n_chars_line;
    int n_lines_page;
    int line;
    int col;
    int mode;
    int background;
    double border;
    int standoff;
    char **text_buffer;
    PG_textdes desc;
    PFKeymap *keymap;
    PG_text_box *next;

    char *(*backup)(PG_text_box *b, char *p, int n);
    char *(*gets)(PG_text_box *b, char *buffer, int maxlen, FILE *stream);

    void (*clear)(PG_text_box *b, int i);
    void (*write)(PG_text_box *b, char *s);
    void (*newline)(PG_text_box *b);};

/*--------------------------------------------------------------------------*/

struct s_PG_event_handler
   {PFEventHand fnc;
    int lang;};

/*--------------------------------------------------------------------------*/

struct s_PG_dev_geometry
   {int phys_dx[PG_SPACEDM];  /* physical device size in PIXELS */
    int hwin[4];              /* 2D window shape on device in PIXELS */
    int iflog[PG_SPACEDM];    /* log space for dimensions */
    int cpc[PG_BOXSZ];        /* conversion limits in PC */
    int tx[PG_BOXSZ];         /* text window in PC */
    double pad[PG_BOXSZ];     /* fractional offset from WC to BND */
    double nd_w[PG_BOXSZ];    /* NDC -> WC transform coefficients */
    double w_nd[PG_BOXSZ];    /* WC -> NDC transform coefficients */
    double fr[PG_BOXSZ];      /* frame limits in NDC */
    double bnd[PG_BOXSZ];     /* boundary limits in WC */
    double pc[PG_BOXSZ];      /* 3D window limits in PC */
    double ndc[PG_BOXSZ];     /* viewspace limits in NDC */
    double wc[PG_BOXSZ];      /* viewspace limits in WC */

    double view_angle[PG_NANGLES];};  /* Euler angles in radians */

/*--------------------------------------------------------------------------*/

/* PG_DEVICE - graphics device structure
 * #bind derived PG_device character-A G_DEVICE PP_deviceObject NULL
 */

struct s_PG_device
   {

#ifdef HAVE_QUICKDRAW

    WindowPtr window;
    Rect window_shape;
    Rect scroll_bar_shape;
    Rect drag_bound_shape;
    Rect display_shape;  /* display rect which should be cleared */
    Rect text_shape;     /* display_shape inset by (4,0) */
    Point mouse;
    TEHandle text;
    ControlHandle scroll_bar;
    MenuHandle menu[menuCount];
    int next_palette_index;

#endif

#ifdef WIN32
    HWND window;
#endif

/* X11 variables */

#ifdef HAVE_X11

    int X_cap_style;
    int X_join_style;
    int X_line_style;
    unsigned int X_line_width;
    Display *display;
    Window window;
    Pixmap pixmap;
    GC gc;
# ifdef HAVE_OGL
    GLXContext glxgc;
# endif
    XGCValues values;
    XFontStruct *font_info;

#endif

/* PostScript device info */
    double ps_level;                 /* PostScript standard level */
    int ps_color;                    /* TRUE if a color PostScript printer */
    int ps_transparent;              /* use masked image drawing (for rasters) */
    int window_orientation;          /* PORTRAIT_MODE or LANDSCAPE_MODE */

/* CGM device info */
    int cgm_page_set;

/* generic device info */

    int absolute_n_color;
    int autodomain;             /* Sets the auto domain */
    int autoplot;
    int autorange;              /* Sets autoranging */

    int background_color_white;
    int border_width;

/* character controls */
    double char_frac;
    double char_height_s;
    int char_precision;
    double char_path[PG_SPACEDM];
    double char_space;
    double char_space_s;
    double char_up[PG_SPACEDM];
    double char_width_s;
    
    bool clipping;

    int data_id;
    int draw_fill_bound;

    FILE *file;
    int fill_color;
    int finished;

    PG_font_family *font_family;

/* graphics cursor location in WC */
    double gcur[PG_SPACEDM];

    int gprint_flag;     /* print suppression flag (TRUE = ON, FALSE = OFF) */
    int grid;

    PG_dev_geometry g;

    int hard_copy_device;                     /* TRUE for hard copy devices */

    SC_array *iobjs;

    int is_visible;

    double light_angle[2];                    /* lighting angles in radians */

    int line_style;
    int line_color;
    double line_width;
    PG_logical_operation logical_op;

/* marker controls */
    double marker_orientation;
    double marker_scale;

    double max_intensity;
    double max_red_intensity;
    double max_green_intensity;
    double max_blue_intensity;

/* color mapping flags */
    int map_text_color;
    int map_line_color;
    int map_fill_color;

    int mode;                           /* _mode = -1 when graphics is off */
    char *name;
    int ncolor;
    int phys_n_colors;
    PG_par_rend_info *pri;              /* parallel rendering info */
    int print_labels;
    int quadrant;
    int range_n_extrema;
    double *range_extrema;

    void *raster;

    int re_open;
    int resolution_scale_factor;
    int scatter;
    int supress_setup;

/* text cursor location in WC */
    double tcur[PG_SPACEDM];

    int text_color;
    char *title; 

    PG_dev_type type_index;
    double txt_ratio;
    char *type;

    char *type_face;
    char *type_style;
    int  type_size;

    int use_pixmap;

/* viewport location/size in window in NDC */
    double view_aspect;
    double view_x[PG_BOXSZ];

/* window location/size in PIXELS */
    double window_x[PG_BOXSZ];

    int xor_parity;

    PG_palette *palettes;
    PG_palette *current_palette;
    PG_palette *color_table;

/* 16 standard colors */
    int BLACK;
    int WHITE;
    int GRAY;
    int DARK_GRAY;
    int BLUE;
    int GREEN;
    int CYAN;
    int RED;
    int MAGENTA;
    int BROWN;
    int DARK_BLUE;
    int DARK_GREEN;
    int DARK_CYAN;
    int DARK_RED;
    int YELLOW;
    int DARK_MAGENTA;

    void (*query_pointer)(PG_device *dev, int *ir, int *pb, int *pq);
    void (*clear_page)(PG_device *dev, int i);
    void (*clear_window)(PG_device *dev);
    void (*clear_viewport)(PG_device *dev);
    void (*clear_region)(PG_device *dev, int nd, PG_coord_sys cs, double *ndc, int pad);
    void (*close_console)(void);
    void (*close_device)(PG_device *dev);
    void (*flush_events)(PG_device *dev);
    void (*draw_dj_polyln_2)(PG_device *dev, double **r, long n,
			     int clip, int coord);
    void (*draw_curve)(PG_device *dev, PG_curve *crv, int clip);
    void (*draw_to_abs)(PG_device *dev, double x, double y);
    void (*draw_to_rel)(PG_device *dev, double x, double y);
    void (*expose_device)(PG_device *dev);
    void (*finish_plot)(PG_device *dev);
    int (*grab_pointer)(PG_device *dev);
    int (*release_pointer)(PG_device *dev);
    int (*get_char)(PG_device *dev);
    void (*get_image)(PG_device *dev, unsigned char *bf, int ix, int iy,
		      int nx, int ny);
    void (*get_text_ext)(PG_device *dev, int nd, PG_coord_sys cs, char *s, double *bx);
    PFread gread;
    PFfgetc ggetc;
    PFfgets ggets;
    void (*gputs)(char *bf);
    void (*set_clipping)(PG_device *dev, bool flag);
    void (*set_char_line)(PG_device *dev, int n);
    void (*set_char_path)(PG_device *dev, double x, double y);
    void (*set_char_precision)(PG_device *dev, int p);
    void (*set_char_space)(PG_device *dev, double s);
    void (*set_char_size)(PG_device *dev, int nd, PG_coord_sys cs, double *x);
    void (*set_char_up)(PG_device *dev, double x, double y);
    void (*set_fill_color)(PG_device *dev, int color, int mapped);
    int (*set_font)(PG_device *dev, char *face, char *style, int size);
    void (*set_line_color)(PG_device *dev, int color, int mapped);
    void (*set_line_style)(PG_device *dev, int style);
    void (*set_line_width)(PG_device *dev, double width);
    void (*set_logical_op)(PG_device *dev, PG_logical_operation lop);
    void (*set_text_color)(PG_device *dev, int color, int mapped);
    void (*shade_poly)(PG_device *dev, int nd, int n, double **r);
    void (*fill_curve)(PG_device *dev, PG_curve *crv);
    void (*make_device_current)(PG_device *dev);
    void (*map_to_color_table)(PG_device *dev, PG_palette *pal);
    void (*match_rgb_colors)(PG_device *dev, PG_palette *pal);
    void (*move_gr_abs)(PG_device *dev, double x, double y);
    void (*move_tx_abs)(PG_device *dev, double x, double y);
    void (*move_tx_rel)(PG_device *dev, double x, double y);
    void (*next_line)(PG_device *dev, FILE *fp);
    PG_device *(*open_screen)(PG_device *dev, double xf, double yf,
			      double dxf, double dyf);
    void (*put_image)(PG_device *dev, unsigned char *bf, int ix, int iy,
		      int nx, int ny);
    void (*query_screen)(PG_device *dev, int *pdx, int *pnc);
    void (*release_current_device)(PG_device *dev);
    void (*update_vs)(PG_device *dev);
    void (*write_text)(PG_device *dev, FILE *fp, char *s);

    int (*events_pending)(PG_device *dev);
    void (*mouse_event_info)(PG_device *dev, PG_event *ev,
			     int *iev, PG_mouse_button *peb, int *peq);
    void (*key_event_info)(PG_device *dev, PG_event *ev, int *iev,
			   char *bf, int n, int *peq);

    PG_event_handler expose_event_handler;
    PG_event_handler update_event_handler;
    PG_event_handler mouse_down_event_handler;
    PG_event_handler mouse_up_event_handler;
    PG_event_handler motion_event_handler;
    PG_event_handler key_down_event_handler;
    PG_event_handler key_up_event_handler;
    PG_event_handler default_event_handler;};
   
/*--------------------------------------------------------------------------*/

/* PG_IMAGE is PD_defstr'd in PDBX.C any changes here must be reflected
 * there!!
 * #bind derived PG_image character-A G_IMAGE PyObject NULL
 */

struct s_PG_image
   {int version_id;    /* version id in case of change see PG_IMAGE_VERSION */
    char *label;
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double zmin;
    double zmax;
    char *element_type;
    unsigned char *buffer;
    int kmax;
    int lmax;
    long size;
    int bits_pixel;
    PG_palette *palette;};


/*--------------------------------------------------------------------------*/

struct s_PG_par_rend_info
   {PG_device *dd;                   /* drawing device for parallel graphics */
    PROCESS *pp;                     /* process for parallel graphics */
    int ip;                          /* id of processor doing final output */
    int have_data;                   /* TRUE if current node has data */
    int *map;                        /* map of processors with data to plot */
    pcons *alist;                    /* picture attributes for parallel graphics */
    char *label;                     /* data label */
    PG_rendering render;             /* rendering mode for parallel graphics */
    int polar;};                     /* polar domain flag */

/*--------------------------------------------------------------------------*/

struct s_PG_view_attributes
   {int finished;

    PG_dev_geometry g;

/* graphics cursor location in WC */
    double gcur[PG_SPACEDM];

/* text cursor location in WC */
    double tcur[PG_SPACEDM];

/* viewport location/size in window in NDC */
    double view_aspect;
    double view_x[PG_BOXSZ];};

/*--------------------------------------------------------------------------*/

struct s_PG_dev_attributes
   {bool clipping;
    double char_frac;
    int char_precision;
    double char_space;
    double char_up[PG_SPACEDM];
    int fill_color;
    int line_color;
    int line_style;
    double line_width;
    PG_logical_operation logical_op;
    int text_color;
    PG_palette *palette;};

/*--------------------------------------------------------------------------*/

struct s_PG_scope_public
   {char *tnames[PG_N_TYPES];
    FILE *stdscr;      /* this is the effective file pointer for the screen */
    PG_device *console;};


#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                          VARIABLE DECLARATIONS                           */

/*--------------------------------------------------------------------------*/
 
extern PG_scope_public
 PG_gs;

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

/* GSACC.C declarations */

extern double
 PG_fget_char_space(PG_device *dev),
 PG_fset_char_space(PG_device *dev, double d),
 PG_fget_line_width(PG_device *dev),
 PG_fset_line_width(PG_device *dev, double wd),
 PG_fget_marker_orientation(PG_device *dev),
 PG_fset_marker_orientation(PG_device *dev, double a),
 PG_fget_marker_scale(PG_device *dev),
 PG_fset_marker_scale(PG_device *dev, double s),
 PG_fget_max_intensity(PG_device *dev),
 PG_fset_max_intensity(PG_device *dev, double i),
 PG_fget_max_red_intensity(PG_device *dev),
 PG_fset_max_red_intensity(PG_device *dev, double i),
 PG_fget_max_green_intensity(PG_device *dev),
 PG_fset_max_green_intensity(PG_device *dev, double i),
 PG_fget_max_blue_intensity(PG_device *dev),
 PG_fset_max_blue_intensity(PG_device *dev, double i),
 PG_fget_ps_dots_inch(void),
 PG_fset_ps_dots_inch(double dpi);

extern int64_t
 PG_fget_buffer_size(void),
 PG_fset_buffer_size(int64_t sz);

extern bool
 PG_fget_clipping(PG_device *dev),
 PG_fset_clipping(PG_device *dev, bool flag);

extern int
 PG_fget_clear_mode(void),
 PG_fset_clear_mode(int i),
 PG_fget_char_precision(PG_device *dev),
 PG_fset_char_precision(PG_device *dev, int p),
 PG_fget_fill_bound(PG_device *dev),
 PG_fset_fill_bound(PG_device *dev, int v),
 PG_fget_finish_state(PG_device *dev),
 PG_fset_finish_state(PG_device *dev, int v),
 PG_fget_identifier(PG_graph *g),
 PG_fset_identifier(PG_graph *g, int id),
 PG_fget_line_style(PG_device *dev),
 PG_fset_line_style(PG_device *dev, int st),
 PG_fget_pixmap_flag(PG_device *dev),
 PG_fset_pixmap_flag(PG_device *dev, int fl),
 PG_fget_use_pixmap(void),
 PG_fset_use_pixmap(int i),
 PG_fget_res_scale_factor(PG_device *dev),
 PG_fset_res_scale_factor(PG_device *dev, int s),
 PG_fget_border_width(PG_device *dev),
 PG_fset_border_width(PG_device *dev, int w),
 PG_fget_line_color(PG_device *dev),
 PG_fset_line_color(PG_device *dev, int clr, int mapped),
 PG_fget_text_color(PG_device *dev),
 PG_fset_text_color(PG_device *dev, int clr, int mapped),
 PG_fget_fill_color(PG_device *dev),
 PG_fset_fill_color(PG_device *dev, int clr, int mapped),
 PG_grab_pointer(PG_device *dev),
 PG_release_pointer(PG_device *dev),
 PG_white_background(PG_device *dev, int t),
 PG_turn_grid(PG_device *dev, int t),
 PG_turn_data_id(PG_device *dev, int t),
 PG_turn_scatter(PG_device *dev, int t),
 PG_turn_autodomain(PG_device *dev, int t),
 PG_turn_autorange(PG_device *dev, int t),
 PG_turn_autoplot(PG_device *dev, int t),
 PG_n_events_pending(PG_device *dev);

extern void
 PG_fget_char_path(PG_device *dev, double *x),
 PG_fset_char_path(PG_device *dev, double *x),
 PG_fget_char_up(PG_device *dev, double *x),
 PG_fset_char_up(PG_device *dev, double *x),
 PG_fget_char_size_n(PG_device *dev, int nd, PG_coord_sys cs, double *p),
 PG_fset_char_size_n(PG_device *dev, int nd, PG_coord_sys cs, double *p),
 PG_fget_font(PG_device *dev, char **of, char **ost, int *osz),
 PG_fset_font(PG_device *dev, char *face, char *style, int sz),
 PG_fget_viewport_pos(PG_device *dev, double *x),
 PG_fset_viewport_pos(PG_device *dev, double *x),
 PG_fget_viewport_shape(PG_device *dev, double *dx, double *pa),
 PG_fset_viewport_shape(PG_device *dev, double *dx, double asp),
 PG_handle_expose_event(PG_device *dev, PG_event *ev),
 PG_handle_update_event(PG_device *dev, PG_event *ev),
 PG_handle_mouse_down_event(PG_device *dev, PG_event *ev),
 PG_handle_mouse_up_event(PG_device *dev, PG_event *ev),
 PG_handle_key_down_event(PG_device *dev, PG_event *ev),
 PG_handle_key_up_event(PG_device *dev, PG_event *ev),
 PG_handle_default_event(PG_device *dev, PG_event *ev),
 PG_query_pointer(PG_device *dev, int *ir, int *pb, int *pq),
 PG_mouse_event_info(PG_device *dev, PG_event *ev,
			 int *iev, PG_mouse_button *peb, int *peq),
 PG_key_event_info(PG_device *dev, PG_event *ev, int *iev,
		       char *bf, int n, int *peq),
 PG_open_screen(PG_device *dev, double *ndc),
 PG_query_screen_n(PG_device *dev, int *dx, int *pnc),
 PG_make_device_current(PG_device *dev),
 PG_release_current_device(PG_device *dev),
 PG_update_vs(PG_device *dev),
 PG_finish_plot(PG_device *dev),
 PG_expose_device(PG_device *dev),
 PG_close_device(PG_device *dev),
 PG_close_console(void),
 PG_clear_frame(PG_device *dev),
 PG_clear_window(PG_device *dev),
 PG_clear_viewport(PG_device *dev),
 PG_clear_region(PG_device *dev, int nd, PG_coord_sys cs,
		 double *ndc, int pad),
 PG_clear_page(PG_device *dev, int i),
 PG_write_text(PG_device *dev, FILE *fp, char *s),
 PG_next_line(PG_device *dev, FILE *fp),
 PG_shade_poly_n(PG_device *dev, int nd, int n, double **r),
 PG_fill_curve(PG_device *dev, PG_curve *crv),
 PG_draw_curve(PG_device *dev, PG_curve *crv, int clip),
 PG_make_palette_current(PG_device *dev, PG_palette *p),
 PG_get_image_n(PG_device *dev, unsigned char *bf,
		PG_coord_sys cs, double *irg),
 PG_put_image_n(PG_device *dev, unsigned char *bf,
		PG_coord_sys cs, double *irg),
 PG_move_gr_abs_n(PG_device *dev, double *p),
 PG_move_tx_abs_n(PG_device *dev, double *p),
 PG_move_tx_rel_n(PG_device *dev, double *p),
 PG_draw_to_abs_n(PG_device *dev, double *p),
 PG_draw_to_rel_n(PG_device *dev, double *p);

extern PG_logical_operation
 PG_fget_logical_op(PG_device *dev),
 PG_fset_logical_op(PG_device *dev, PG_logical_operation lop);

extern pcons
 *PG_fget_render_info(PG_graph *g),
 *PG_fset_render_info(PG_graph *g, pcons *a);


/* GSATTR.C declarations */

extern int
 PG_get_attrs_graph(PG_graph *g, int all, ...),
 PG_get_attrs_mapping(PM_mapping *f, ...),
 PG_get_attrs_set(PM_set *s, ...),
 PG_get_attrs_alist(pcons *alst, ...),
 PG_get_attrs_glb(int dflt, ...),
 PG_set_attrs_graph(PG_graph *g, ...),
 PG_set_attrs_mapping(PM_mapping *f, ...),
 PG_set_attrs_set(PM_set *s, ...),
 PG_set_attrs_glb(int dflt, ...),
 PG_rem_attrs_graph(PG_graph *g, ...),
 PG_rem_attrs_mapping(PM_mapping *f, ...),
 PG_rem_attrs_set(PM_set *s, ...);

extern pcons
 *PG_set_attrs_alist(pcons *alst, ...),
 *PG_rem_attrs_alist(pcons *alst, ...);

extern void
 PG_setup_attrs_glb(void),
 *PG_ptr_attr_glb(char *name);


/* GSAUX.C declarations */

extern unsigned char
 *PG_c_str_pascal(unsigned char *bf, char *s, long n, int pad);

extern int
 PG_CGM_command(PG_device *dev, int cat, int id, int nparams, ...);


/* GSAXIS.C declarations */

extern int
 PG_set_axis_attributes(PG_device *dev, ...);

extern double
 PG_fget_axis_decades(void),
 PG_fset_axis_decades(double d);

extern void
 PG_axis(PG_device *dev, int axis_type),
 PG_axis_3(PG_device *dev, double **x, int n_pts, double *extr);

extern PG_axis_def
 *PG_draw_axis_n(PG_device *dev, double *xl, double *xr,
		 double *tn, double *vw, double sc,
		 char *format, int tick_type, int label_type,
		 int flag, ...);


/* GSCLR.C declarations */

extern int
 PG_rgb_index(PG_device *dev, RGB_color_map *cm),
 PG_select_color(PG_device *dev, int n, double *a, double *extr),
 PG_wr_palette(PG_device *dev, PG_palette *pal, char *fname);

extern void
 PG_register_palette(PG_device *dev, PG_palette *pal, int map),
 PG_setup_standard_palettes(PG_device *dev, int nc, int *gs),
 PG_rl_palette(PG_palette *pal),
 PG_show_palettes(PG_device *sdev, char *type, int wbck),
 PG_show_colormap(char *type, int wbck),
 PG_dump_colormap(char *type, char *file),
 PG_rgb_color(RGB_color_map *clr, int lc, int rgb, PG_palette *pal),
 PG_color_map(PG_device *dev, int mono, int fix),
 PG_gray_map(PG_device *dev, int ng, int *gs, double imx);

extern PG_palette
 *PG_mk_palette(PG_device *dev, char *name, int nclr),
 *PG_fget_palette(PG_device *dev, char *name),
 *PG_fset_palette(PG_device *dev, char *name),
 *PG_make_palette(PG_device *tdev, char *name,
		  int nclr, int wbck),
 *PG_make_ndim_palette(PG_device *tdev, char *name,
		       int ndims, int *dims,  int wbck),
 *PG_rd_palette(PG_device *dev, char *fname);


/* GSCNTR.C declarations */

extern PG_picture_desc
 *PG_setup_picture_contour(PG_device *dev, PG_graph *data,
			   int save, int clear);

extern void
 PG_iso_limit(double *a, int npts, double *pmin, double *pmax),
 PG_contour_plot(PG_device *dev, PG_graph *data, ...);

extern int
 PG_contour_levels(double *lev, int nlev, double fmin, double fmax,
		   double ratio);


/* GSDLIN.C declarations */

extern void
 PG_draw_polygon(PG_device *dev, PM_polygon *py, int clip),
 PG_draw_multiple_line(PG_device *dev, int nlines,
		       double *x1, double *x2, double *x3, double *x4,
		       double *dx),
 PG_draw_line_n(PG_device *dev, int nd, PG_coord_sys cs,
		double *x1, double *x2, int clip),
 PG_draw_polyline_n(PG_device *dev, int nd, PG_coord_sys cs,
		    long n, double **x, int clip),
 PG_draw_projected_polyline_n(PG_device *dev, int nd, PG_coord_sys cs,
			      long n, double **x, int clip),
 PG_draw_disjoint_polyline_n(PG_device *dev, int nd, PG_coord_sys cs,
			     long n, double **x, int clip),
 PG_draw_rad(PG_device *dev, double rmin, double rmax,
	     double a, double x, double y, int unit),
 PG_draw_arc(PG_device *dev, double r, double a1, double a2,
	     double x, double y, int unit);


/* GSDPLT.C declarations */

extern PG_picture_desc
 *PG_setup_picture_mesh(PG_device *dev, PG_graph *data, int save, int clear);

extern void
 PG_mesh_plot(PG_device *dev, PG_graph *data, ...),
 PG_domain_plot(PG_device *dev, PM_set *dom, PM_set *ran),
 PG_draw_domain_boundary(PG_device *dev, PM_mapping *f),
 PG_ref_mesh(PG_device *dev, PG_graph *data, int ndims,
	     double *bx, double *vwprt);


/* GSDV.C declarations */

extern void
 PG_setup_input(char *s, int nc, size_t nr),
 PG_reset_input(void),
 PG_device_filename(char *fname, long nc, char *raw, char *ext),
 PG_query_device(PG_device *dev, int *pdx, int *pdy, int *pnc),
 PG_query_window(PG_device *dev, int *pdx, int *pdy),
 PG_register_range_extrema(PG_device *dev, int nd, double *extr),
 PG_flush_events(PG_device *dev);

extern PG_device
 *PG_open_device(PG_device *dev, double xf, double yf, double dxf, double dyf);

extern ssize_t
 PG_wind_read(int fd, void *bf, size_t nc);

extern char
 *PG_wind_fgets(char *str, int maxlen, FILE *stream);

extern int
 PG_setup_window_device(PG_device *dev),
 PG_fprintf(FILE *fp, char *fmt, ...),
 PG_fputs(char *s, FILE *fp),
 PG_write_n(PG_device *dev, int nd, PG_coord_sys cs, double *x, char *fmt, ...),
 PG_wind_fgetc(FILE *stream),
 PG_wind_fprintf(FILE *fp, char *fmt, ...),
 PG_wind_fputs(char *s, FILE *fp),
 PG_open_console(char *title, char *type, int bckgr,
		 double xf, double yf, double dxf, double dyf);


/* GSEVMN.C declarations */

extern void
 PG_remove_callback(int *pfd),
 PG_poll(int ind, int to),
 PG_catch_interrupts(int flg),
 PG_flush_events(PG_device *dev);

extern int
 PG_init_event_handling(void),
 PG_loop_callback(int type, void *p,
		  PFFileCallback acc, PFFileCallback rej, int pid),
 PG_check_events(int flag, int to),
 PG_get_next_event(PG_event *ev);

extern SC_evlpdes
 *PG_get_event_loop(void);

extern PG_device
 *PG_get_event_device(PG_event *ev);


/* GSFIA.C declarations */

extern PG_view_attributes
 *PG_view_attributes_pointer(int vwatid);


/* GSGROT.C declarations */

extern void
 PG_grotrian_plot(PG_device *dev, PG_graph *data, ...);


/* GSDV_CGM.C declarations */

extern int
 PG_setup_cgm_device(PG_device *d);


/* GSDV_GL.C declarations */


/* GSDV_IM.C declarations */

extern int
 PG_get_processor_number(void),
 PG_get_number_processors(void),
 PG_setup_image_device(PG_device *d);

extern void
 PG_parallel_setup(PG_device *dev, PG_picture_desc *pd);


/* GSDV_JPG.C declarations */

extern int
 PG_setup_jpeg_device(PG_device *d);


/* GSDV_MPG.C declarations */

extern int
 PG_setup_mpeg_device(PG_device *d);


/* GSDV_PNG.C declarations */

extern int
 PG_setup_png_device(PG_device *d);


/* GSDV_PS.C declarations */

extern int
 PG_setup_ps_device(PG_device *d);


/* GSDV_RST.C declarations */

extern int
 PG_setup_raster_device(PG_device *d);


/* GSIMAG.C declarations */

extern PG_picture_desc
 *PG_setup_picture_image(PG_device *dev, PG_graph *data, int save, int clear);

extern void
 PG_image_picture_info(PG_graph *data, int *pnd, double *dbx, double *rbx),
 PG_image_plot(PG_device *dev, PG_graph *data, ...),
 PG_draw_image_pixels(PG_device *dev, char *name, char *type,
		      void *f, double *dbx, double *ext,
		      void *cnnct, pcons *alist),
 PG_draw_palette_n(PG_device *dev, double *dbx, double *rbx,
		   double wid, int exact),
 PG_draw_2dpalette(PG_device *dev, double *frm, double *rex, double wid),
 PG_invert_image_data(unsigned char *bf,
		      int kx, int lx, int bpi),
 PG_invert_image(PG_image *im),
 PG_get_viewport_size(PG_device *dev, int *pix, int *piy, int *pnx, int *pny),
 PG_draw_image(PG_device *dev, PG_image *im, char *label,
	       pcons *alst),
 PG_rl_image(PG_image *im);

extern PG_image
 *PG_build_image(PG_device *dev, char *name, char *type, void *z,
		 int w, int h, int nd, PG_coord_sys cs, double *dbx, double *rbx),
 *PG_make_image_n(char *label, char *type, void *z,
		  int nd, PG_coord_sys cs, double *dbx, double *rbx,
		  int w, int h, int bpp, PG_palette *palette),
 *PG_extract_image(PG_device *dev, double *dbx, double *rbx);

extern int
 PG_render_data_type(PG_graph *data),
 PG_copy_image(PG_image *dim, PG_image *sim, int bck),
 PG_place_image(PG_image *dim, PG_image *sim, int scale),
 PG_shift_image_range(PG_image *a, PG_image *b, int off);


/* GSCRV.C declarations */

extern PG_picture_desc
 *PG_setup_picture_curve(PG_device *dev, PG_graph *data, int save, int clear);

extern void
 PG_curve_plot(PG_device *dev, PG_graph *data, ...),
 PG_plot_curve(PG_device *dev, double *x, double *y,
	       int n, pcons *info, int l),
 PG_draw_data_ids(PG_device *dev, double *x, double *y, int n,
		  int label, pcons *info),
 PG_rect_plot(PG_device *dev, double *x, double *y, int n, int lncol,
	      double lnwid, int lnsty, int scatter, int marker, int l),
 PG_histogram_plot(PG_device *dev, double *x, double *y, int n,
		   int lncol, double lnwid, int lnsty,
		   int scatter, int marker, PM_direction start, int l),
 PG_insel_plot(PG_device *dev, double *x, double *y, int n,
	       int lncol, double lnwid, int lnsty, int l),
 PG_polar_plot(PG_device *dev, double *x, double *y, int n, int lncol,
	       double lnwid, int lnsty, int scatter, int marker, int l);


/* GSHIGH.C declarations */

extern pcons
 *PG_set_plot_type(pcons *info, PG_rendering pty, int axs);

extern SC_array
 *PG_clip_polygon(PG_device *dev, PM_polygon *py);

extern void
 PG_clip_data(PG_device *dev, double *tx, double *ty, int *ptn,
	      double *x, double *y, int n),
 PG_clip_line(int *pfl, double *x1, double *x2, double *wc, int *lgf),
 PG_center_label(PG_device *dev, double sy, char *label),
 PG_polar_limits(double *bx, double *x, double *y, int n),
 PG_set_limits_n(PG_device *dev, int nd, PG_coord_sys cs, long n, double **x,
		 PG_rendering pty),
 PG_print_label_set(double *pyo, PG_device *dev, int nlabs,
		    char **labels, char **files,
		    int *dataid, int *modified, int *clr,
		    double *extr, char *ctg,
		    unsigned int llf, int lcf, int slf, int lts);

extern int
 PG_clip_line_seg(PG_device *dev, double *x1, double *x2),
 PG_clip_box(double *bx, double *wc, int *lgf);


/* GSREND.C declarations */

extern PG_rendering
 PG_est_rendering(PG_graph *data);

extern void
 PG_find_extrema(PG_graph *data, double stdoff,
		 double **pdpex, double **prpex,
		 int *pnde, double **pddex,
		 int *pnre, double **prdex),
 PG_adorn_before(PG_device *dev, PG_picture_desc *pd, PG_graph *data),
 PG_draw_picture(PG_device *dev, PM_mapping *f, PG_rendering ptyp,
		 int bndp, int cbnd, int sbnd, double wbnd,
		 int mshp, int cmsh, int smsh, double wmsh),
 PG_finish_picture(PG_device *dev, PG_graph *data, PG_picture_desc *pd),
 PG_draw_graph(PG_device *dev, PG_graph *data),
 PG_fset_picture_hook(void (*f)(PG_device *dev, PG_graph *data,
				PG_picture_desc *pd));

extern PG_picture_desc
 *PG_setup_picture(PG_device *dev, PG_graph *data, int save,
		   int clear, int par),
 *PG_get_rendering_properties(PG_device *dev, PG_graph *data);


/* GSIOB.C declarations */

extern PG_device
 *PG_handle_button_press(void *d, PG_event *ev),
 *PG_handle_key_press(void *d, PG_event *ev);

extern PG_interface_object
 *PG_get_object_event(PG_device *dev, PG_event *ev),
 *PG_copy_interface_object(PG_device *dev, PG_interface_object *iob),
 *PG_find_object(PG_device *dev, char *s, PG_interface_object *parent);

extern haelem
 *PG_lookup_variable(char *name);

extern PFVoid
 PG_lookup_callback(char *name);

extern void
 PG_define_region(PG_device *dev, PG_coord_sys cs, double *bx),
 PG_print_pointer_location(PG_device *dev, double cx, double cy, int coord),
 PG_register_callback(char *name, ...),
 PG_register_variable(char *name, char *type, void *var, void *vn, void *vx),
 PG_move_object(PG_interface_object *iob, double *bx, int redraw),
 PG_draw_interface_object(PG_interface_object *iob),
 PG_draw_interface_objects(PG_device *dev),
 PG_key_action(PG_text_box *b, char *bf, int nc),
 PG_map_interface_object(PG_interface_object *iob, PFEvCallback fnc, void *a),
 PG_map_interface_objects(PG_device *dev, PFEvCallback fnc, void *a),
 PG_register_interface_object(PG_device *dev, PG_interface_object *iob),
 PG_add_text_obj(PG_device *dev, double *obx, char *s),
 PG_add_button(PG_device *dev, double *obx, char *s, PFEvCallback fnc),
 PG_free_interface_objects(PG_device *dev),
 PG_copy_interface_objects(PG_device *dvd, PG_device *dvs, int rm),
 PG_handle_keyboard_event(PG_interface_object *iob, PG_event *ev);


/* GSIOD.C declarations */

extern PG_interface_object
 *PG_add_iob(PG_device *dev, PG_interface_object *cnt,
	     double *bx, char *type, ...),
 *PG_iob_make_container(PG_device *dev, char *name, double *bx);

extern void
 PG_iob_register_toggle(PG_device *dev, PG_interface_object *cnt,
			char *name, char *abbrv, int *pv,
			double *bx, int on_clr, int off_clr),
 PG_iob_register_slider(PG_device *dev, PG_interface_object *cnt,
			char *name, char *abbrv, int *pv,
			double *bx, double dx, double dy);


/* GSMM.C declarations */

extern PG_graph
 *PG_make_graph_1d(int id, char *label, int cp, int n, double *x, double *y,
		   char *xname, char *yname),
 *PG_make_graph_r2_r1(int id, char *label, int cp, int imx, int jmx,
		      PM_centering centering, double *x, double *y,
		      double *r, char *dname, char *rname),
 *PG_make_graph_r3_r1(int id, char *label, int cp,
		      int imx, int jmx, int kmx, PM_centering centering,
		      double *x, double *y, double *z, double *r,
		      char *dname, char *rname),
 *PG_make_graph_from_sets(char *name, PM_set *domain, PM_set *range,
			  PM_centering centering, char *info_type, void *info,
			  int id, PG_graph *next),
 *PG_make_graph_from_mapping(PM_mapping *f, char *info_type, void *info,
			     int id, PG_graph *next);

extern PG_device
 *PG_make_device(char *name, char *type, char *title);

extern PG_view_attributes
 *PG_make_view_attributes(PG_device *dev);

extern PG_font_family
 *PG_make_font_family(PG_device *dev, char *name,
		      PG_font_family *next, int n, ...);

extern PG_interface_object
 *PG_make_interface_object_n(PG_device *dev, char *name, char *type,
			     void *obj, PG_curve *crv,
			     int *flags, int fc, int bc,
			     PG_textdes *ptd,
			     char *slct, char *draw, char *action,
			     PG_interface_object *parent);

extern PG_curve
 *PG_make_curve(PG_device *dev, PG_coord_sys cs, int closed, int n,
		double *xo, double **r),
 *PG_make_box_curve(PG_device *dev, PG_coord_sys cs, double *co, double *cbx),
 *PG_copy_curve(PG_curve *icv, PG_device *odv, PG_device *idv);

extern pcons
 *PG_set_line_info(pcons *info, PG_rendering pty, int axis_type,
		   int style, int scatter, int marker,
		   int color, PM_direction start, double width),
 *PG_set_tds_info(pcons *info, PG_rendering type, int axis_type,
		  int style, int color, int nlev, double ratio,
		  double width, double theta, double phi, double chi,
		  double d),
 *PG_set_tdv_info(pcons *info, PG_rendering type, int axis_type, 
		  int style, int color, double width);

extern void
 PG_rl_all(void),
 PG_register_device(char *name, PFRDev fnc),
 PG_rl_device(PG_device *dev),
 PG_rl_graph(PG_graph *g, int rld, int rlr),
 PG_save_view_attributes(PG_view_attributes *d, PG_device *dev),
 PG_restore_view_attributes(PG_device *dev, PG_view_attributes *d),
 PG_release_curve(PG_curve *reg);

extern int
 PG_init_parallel(char **argv, PROCESS *pp),
 PG_fin_parallel(int trm);


/* GSPR.C declarations */

extern PFEventHand
 PG_set_expose_event_handler(PG_device *dev, PFEventHand fnc),
 PG_set_update_event_handler(PG_device *dev, PFEventHand fnc),
 PG_set_mouse_down_event_handler(PG_device *dev, PFEventHand fnc),
 PG_set_mouse_up_event_handler(PG_device *dev, PFEventHand fnc),
 PG_set_motion_event_handler(PG_device *dev, PFEventHand fnc),
 PG_set_key_down_event_handler(PG_device *dev, PFEventHand fnc),
 PG_set_key_up_event_handler(PG_device *dev, PFEventHand fnc),
 PG_set_default_event_handler(PG_device *dev, PFEventHand fnc);

extern PG_dev_attributes
 *PG_get_attributes(PG_device *dev);

extern void
 PG_get_text_ext_n(PG_device *dev, int nd, PG_coord_sys cs, char *s, double *p),
 PG_set_attributes(PG_device *dev, PG_dev_attributes *attr),
 PG_setup_markers(void),
 PG_draw_markers_n(PG_device *dev, int nd, PG_coord_sys cs,
		   int n, double **r, int marker),
 PG_draw_box_n(PG_device *dev, int nd, PG_coord_sys cs, double *box),
 PG_fill_polygon_n(PG_device *dev, int color, int mapped,
		   int nd, PG_coord_sys cs, long n, double **r);

extern int
 PG_def_marker(int n, double *x1, double *y1, double *x2, double *y2),
 PG_setup_font(PG_device *dev, char *face, char *style, int size,
	       char **pfn, int *pnf, int *pns);


/* GSRWI.C declarations */

extern int
 PG_write_interface(PG_device *dev, char *name),
 PG_read_interface(PG_device *dev, char *name);


/* GSSCAT.C declarations */

extern PG_picture_desc
 *PG_setup_picture_scatter(PG_device *dev, PG_graph *data, int save, int clear);

extern void
 PG_scatter_plot(PG_device *dev, PG_graph *data, ...),
 PG_draw_scatter(PG_device *dev, double *a1, double *a2,
		 int nn, int ne, double **x,
		 double theta, double phi, double chi, double width,
		 int color, int style, int type, char *name,
		 pcons *alist);


/* GSSURF.C declarations */

extern PG_picture_desc
 *PG_setup_picture_surface(PG_device *dev, PG_graph *data, int save, int clear);

extern void
 PG_surface_plot(PG_device *dev, PG_graph *data, ...),
 PG_draw_surface_n(PG_device *dev, double *a1, double *a2, double *aext,
		   double **r, int nn, double *va, double width,
		   int color, int style, PG_rendering pty, char *name,
		   char *mesh_type, void *cnnct, pcons *alist);


/* GSPOLF.C declarations */

extern PG_picture_desc
 *PG_setup_picture_fill_poly(PG_device *dev, PG_graph *data, int save, int clear);

extern void
 PG_poly_fill_plot(PG_device *dev, PG_graph *data, ...),
 PG_fill_poly_zc_lr(PG_device *dev, int nd, double **a,
		    double *x, double *y, double *aext, void *cnnct,
		    pcons *alist),
 PG_fill_poly_nc_lr(PG_device *dev, int nd, double **a,
		    double *x, double *y, double *aext, void *cnnct,
		    pcons *alist),
 PG_fill_poly_zc_ac(PG_device *dev, int nd, double **a,
		    double *x, double *y, double *aext, void *cnnct,
		    pcons *alist),
 PG_fill_poly_nc_ac(PG_device *dev, int nd, double **a,
		    double *x, double *y, double *aext, void *cnnct,
		    pcons *alist);


/* GSDVB.C declarations */

extern PG_picture_desc
 *PG_setup_picture_dv_bnd(PG_device *dev, PG_graph *data, int save, int clear);

extern void
 PG_dv_bnd_plot(PG_device *dev, PG_graph *data, ...),
 PG_dv_bnd_zc_lr(PG_device *dev, int nd, int *a,
		 double *x, double *y, int npts, int *aext,
		 void *cnnct, pcons *alist),
 PG_dv_bnd_zc_ac(PG_device *dev, int nd, int *a,
		 double *x, double *y, int npts, int *aext,
		 void *cnnct, pcons *alist);


/* GSTXT.C declarations */

extern int
 PG_is_text_box(PG_interface_object *iob),
 PG_is_inactive_text_box(PG_interface_object *iob);

extern PG_text_box
 *PG_open_text_rect(PG_device *dev, char *name, int type,
		    PFKeymap *keymap, PG_curve *crv,
		    double brd, int ndrw),
 *PG_open_text_box(PG_device *dev, char *name, int type,
		   PFKeymap *keymap,
		   double xf, double yf,
		   double dxf, double dyf),
 *PG_copy_text_object(PG_text_box *ib,
		      PG_device *dvd, PG_device *dvs);

extern void
 PG_resize_text_box(PG_text_box *b, double *bx, int n_lines),
 PG_refresh_text_box(PG_text_box *b),
 PG_close_text_box(PG_text_box *b);


/* GSVECT.C declarations */

extern PG_picture_desc
 *PG_setup_picture_vector(PG_device *dev, PG_graph *data, int save, int clear);

extern void
 PG_draw_vector_n(PG_device *dev, int nd, PG_coord_sys cs, long n,
		  double **x, double **u),
 PG_vector_plot(PG_device *dev, PG_graph *data, ...),
 PG_draw_vector(PG_device *dev, PG_graph *g),
 PG_set_vec_attr(PG_device *dev, ...);


/* GSVSP.C declarations */

extern void
 PG_log_space(PG_device *dev, int nd, int dec, double *box),
 PG_log_point(PG_device *dev, int nd, double *p),
 PG_lin_space(PG_device *dev, int nd, double *box),
 PG_lin_point(PG_device *dev, int nd, double *p),
 PG_get_curve_extent(PG_device *dev, PG_curve *crv, PG_coord_sys cs, double *bx),
 PG_get_limit(PG_device *dev, PG_coord_sys cs, double *lim),
 PG_frame_points(PG_device *dev, int nd, PG_coord_sys cs, long n, double **xi,
		 double **xo),
 PG_frame_point(PG_device *dev, int nd, PG_coord_sys cs, double *xi, double *xo),
 PG_frame_interval(PG_device *dev, int nd, double *dxi, double *dxo),
 PG_frame_box(PG_device *dev, int nd, PG_coord_sys cs, double *box),
 PG_frame_viewport(PG_device *dev, int nd, double *ndc),
 PG_viewport_frame(PG_device *dev, int nd, double *ndc),
 PG_fget_axis_log_scale(PG_device *dev, int nd, int *iflg),
 PG_fset_axis_log_scale(PG_device *dev, int nd, int *iflg),
 PG_init_viewspace(PG_device *dev, int setw),
 PG_set_viewspace(PG_device *dev, int nd, PG_coord_sys cs, double *extr),
 PG_get_viewspace(PG_device *dev, PG_coord_sys cs, double *box),
 PG_fget_view_angle(PG_device *dev, int cnv,
		    double *pt, double *pp, double *pc),
 PG_fset_view_angle(PG_device *dev, int cnv,
		    double *pt, double *pp, double *pc),
 PG_fget_light_angle(PG_device *dev, int cnv, double *pt, double *pp),
 PG_fset_light_angle(PG_device *dev, int cnv, double *pt, double *pp),
 PG_scale_points(PG_device *dev, int n, int nd, PG_coord_sys ics, double **xi,
		 PG_coord_sys ocs, double **xo),
 PG_trans_points(PG_device *dev, int n, int nd, PG_coord_sys ics, double **xi,
		 PG_coord_sys ocs, double **xo),
 PG_trans_point(PG_device *dev, int nd, PG_coord_sys ics, double *xi,
		PG_coord_sys ocs, double *xo),
 PG_trans_box(PG_device *dev, int nd, PG_coord_sys ics, double *bi,
	      PG_coord_sys ocs, double *bo),
 PG_trans_interval(PG_device *dev, int nd, PG_coord_sys ics, double *dxi,
		   PG_coord_sys ocs, double *dxo),
 PG_box_init(int nd, double *bx, double mn, double mx),
 PG_box_copy(int nd, double *d, double *s),
 PG_conform_view(PG_device *dev, int nd, PG_coord_sys cs, double *box,
		 int n, double **x),
 PG_rotate_vectors(PG_device *dev, int nd, int n, double **x);

extern int
 PG_box_contains(int nd, double *box, double *p);

extern double
 **PG_get_space_box(PG_device *dev, double *extr, int offs);


#ifdef __cplusplus
}
#endif

#include "pgs_old.h"

#endif


