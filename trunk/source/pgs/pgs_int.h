/*
 * PGS_INT.H - internal header in support of Portable Graphics System
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_PGS_INT

#include "cpyright.h"

#define PCK_PGS_INT

#include "pdb_int.h"
#include "pgs.h"
#include "scope_raster.h"
#include "scope_cgm.h"

#define SET_PC_FROM_HWIN(g)                                                \
    g->pc[0] = 0.0;                                                        \
    g->pc[1] = g->hwin[1] - g->hwin[0];                                    \
    g->pc[2] = 0.0;                                                        \
    g->pc[3] = g->hwin[3] - g->hwin[2];                                    \
    g->pc[4] = 0.0;                                                        \
    g->pc[5] = min(g->pc[1], g->pc[3])

#define TRI_NODE(a, n) ((a)->node[(n)])

#define PG_QUAD_FOUR_POINT(d, x)                                           \
    {if ((d)->quadrant == QUAD_FOUR)                                       \
        (x)[1] = (d)->g.pc[3] - (x)[1];}

#define PG_QUAD_FOUR_BOX(d, b)                                             \
    {if ((d)->quadrant == QUAD_FOUR)                                       \
        (b)[2] = (d)->g.pc[3] - (b)[3];}

#define IOB(a_, i_)  (*(PG_interface_object **) SC_array_get(a_, i_))

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                          TYPEDEFS AND STRUCTS                            */

/*--------------------------------------------------------------------------*/

typedef struct s_gattrdes gattrdes;
typedef struct s_gcontdes gcontdes;

typedef struct s_PG_state PG_state;

struct s_PG_state
   {

/* initializes to non-zero values */

/* GSHLS.C */
    int no_order;

/* GSIMAG.C */
    int ok_change_view;

/* GSVECT.C */
    double scale;
    double hdangle;
    double headsize;

/* GSSCALE.C */
    int axis_label_standoff_pc;

/* GSPR_PS */
    int last_color;
    int last_prim;

/* initializes to 0 bytes */

/* GSVECT.C */
    int ifix;
    int ifixhead;
    int maxvsz;
    double fixsize;
    double sizemax;

/* GSTXT.C */
    PFKeymap *std_keymap;

/* GSTX_X.C */
    PG_device *txtd;

/* GSREND.C */
    void (*picture)(PG_device *dev, PG_graph *data,
		    PG_picture_desc *pd);

/* GSPR_PS.C */
    PG_device *last_dev;

/* GSPR.C */
    int marker_max;
    PG_marker *marker_list;

/* GSNCNT.C */
    PM_set **bnd_surf;

/* GSMM.C */
    int dp;
    hasharr *device_table;

/* GSIOB.C */
    hasharr *callback_tab;

/* GSIMAG.C */
    double *pol_extr;
    PG_graph *im_temp;

/* GSHLS.C */
    int depth_sort;

/* GSFIA.C */
    int view_attr_indx;
    int view_attr_max_indx;
    PG_view_attributes **view_attr_list;

/* GSDV.C */
    off_t buffer_size;

/* GSDV_RST.C */
    int **rst_marker_fonts;
    int *rst_current_font;

/* GSDV_IM.C */
    PROCESS *pp_me;

/* GSDV_PS.C */
    int pagen;

/* GSDV_CGM.C */
    int npage;

/* GSDPLT.C */
    PM_centering cntrg;
    PG_rendering ptype;
    int scatter;
    char *palette;

/* GSAUX.C */
    double fac;

/* GSATTR.C */
    hasharr *attr_table;

    int dsp_intr;
    int tty_intr;
    int next_char;
    PG_text_box *b0;

    SC_array *devlst;

    char iso_err[MAXLINE];};


/* global PGS attributes */

struct s_gattrdes
   {int axis_char_size;
    int axis_grid_on;
    int axis_grid_style;
    int axis_labels;
    int axis_line_style;
    int axis_max_major_ticks;
    int axis_n_minor_ticks;
    int axis_n_minor_x_ticks;
    int axis_n_minor_y_ticks;
    int axis_on;
    int axis_tick_type;
    int axis_type;
    int color_map_to_extrema;
    int contour_n_levels;
    int edit_mode;
    int hide_rescale;
    int hist_start;
    int hl_clear_mode;
    int interp_flag;
    int jpg_quality;
    int label_color_flag;
    int label_length;
    int label_type_size;
    int line_color;
    int line_style;
    int logical_plot;
    int marker_index;
    int max_color_factor;
    int numeric_data_id;
    int overplot;
    int palette_orientation;
    int parallel_graphics;
    int parallel_simulate;
    int plot_date;
    int plot_labels;
    int plot_title;
    int plot_type;
    int ref_mesh_color;
    int ref_mesh;
    int restore_viewport;
    int scatter_plot;
    int squeeze_labels;
    int stroke_raster_text;
    int text_color;
    int use_pixmap;                      /* works for X only */

    double axis_char_angle;
    double axis_label_x_standoff;
    double axis_label_y_standoff;
    double axis_line_width;
    double axis_major_tick_size;
    double axis_n_decades;
    double cgm_text_mag;
    double contour_ratio;
    double error_cap_size;
    double label_position_y;
    double label_space;
    double label_yoffset;
    double line_width;
    double marker_scale;
    double palette_width;
    double ps_dots_inch;

    char *axis_label_x_format;
    char *axis_label_y_format;
    char *axis_type_face;
    char *text_format;};


/* global PGS controls */

struct s_gcontdes
   {int input_nc;
    int input_ncx;
    char default_window_device[MAXLINE];
    char *input_bf;
    PG_event current_event;
    PFGetEvent get_event;
    PFOpenConsole open_console;
    PFEventDevice event_device;
    int (*setup_window)(PG_device *dev);};


/* PICTURE - contains the information necessary to describe a picture
 *         - in some fashion
 */

struct s_PG_picture_desc
   {PG_graph *data;
    pcons *alist;
    double *viewport;
    PG_rendering rendering;

    int pl_type;
    int mesh_fl;

    int axis_fl;                    /* axis description */
    int ax_type;

    int legend_curve_fl;            /* print curve labels */
    int legend_contour_fl;          /* contour plot legend */
    int legend_palette_fl;          /* color palette display */
    int legend_label_fl;
    char *legend_label;
    PG_palette *legend_palette;
    int *legend_place;              /* info for placing legend in legend box */

    int lev_method;                 /* 1 - black, 2 - color, 3 - fill */
    int n_levels;
    double *levels;

    double va[PG_NANGLES];          /* view angle */
    double view[4];                 /* original viewport */
    double wc[4];};                 /* original WC */


/* used by PG_hls_remove */

typedef struct s_PG_node PG_node;
typedef struct s_PG_triangle PG_triangle;

struct s_PG_node
   {int indx;      /* -1 if the node is artificial */
    double x[PG_SPACEDM];
    double val;};

struct s_PG_triangle
   {double norm[PG_SPACEDM];
    PG_node node[PG_SPACEDM];};

/*--------------------------------------------------------------------------*/

/*                          VARIABLE DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

extern PG_state
 _PG;

extern JMP_BUF
 io_avail;

extern gattrdes
 _PG_gattrs;

extern gcontdes
 _PG_gcont;

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/


/* GSATTR.C declarations */

extern void
 _PG_rl_attrs_glb(void),
 _PG_ptr_attr_set(char *typ, void *pvo, void *pvi);

extern haelem
 *PG_ptr_attr_entry(char *name);


/* GSCLR.C declarations */

extern void
 _PG_match_rgb_colors(PG_device *dev, PG_palette *pal);

extern int
 _PG_trans_color(PG_device *dev, int color);


/* GSCNTR.C declarations */

extern int
 _PG_get_fill_contour_color(PG_device *dev, int l, int n);


/* GSDV.C declarations */

extern FILE
 *_PG_fopen(char *name, char *mode);

extern void
 _PG_default_viewspace(PG_device *dev, int asp),
 _PG_push_device(PG_device *dev),
 _PG_remove_device(PG_device *dev),
 PG_setup_ctrls_glb(char *s),
 PG_X_setup_ctrls_glb(void),
 PG_GL_setup_ctrls_glb(void),
 PG_qd_setup_ctrls_glb(void),
 PG_win32_setup_ctrls_glb(void);

extern int
 PG_setup_x11_device(PG_device *dev),
 PG_setup_gl_device(PG_device *dev),
 PG_setup_qd_device(PG_device *dev),
 PG_setup_win32_device(PG_device *dev);


/* GSDV_IM.C declarations */

extern PG_par_rend_info
 *_PG_init_dev_parallel(PG_device *dd, int dp);


/* GSDV_RST.C declarations */

extern void
 _PG_rl_rst_fonts(void);


/* GSDV_RST.C declarations */

extern void
 PG_make_raster_family_name(PG_RAST_device *mdv, char *fname, int nc),
 _PG_X_fill_pixmap(PG_device *dev, unsigned int *ipc, int color);


/* GSHLS.C declarations */

extern PG_triangle
 *_PG_hls_remove(PG_device *dev, char *type, double **r, double *f,
		 void *cnnct, pcons *alst, int *pn);


/* GSIMAG.C declarations */

extern void
 _PG_intp_byte(unsigned char *op, unsigned char *np, int ox, int nx,
	       int os, int ns);

extern int
 _PG_allocate_image_buffer(PG_device *dev, unsigned char **pbf,
			   int *pnx, int *pny),
 _PG_byte_bit_map(unsigned char *bf, int nx, int ny, int complmnt),
 _PG_map_image_color(PG_device *dev, int pc, int fc, int bc);


/* GSIOB.C declarations */

extern void
 PG_clear_interface_objects(PG_device *dev);


/* GSHIGH.C declarations */

extern void
 _PG_print_labels(PG_device *dev, PG_graph *data);


/* GSNCNT.C declarations */

void _PG_iso_nc_lr_3d(PG_device *dev, double *a,
		      int ndd, double **x, double *lev,
		      int nlev, int id, void *cnnct, pcons *alist,
		      int method);


/* GSMM.C declarations */

extern void
 _PG_rl_interface_object(PG_interface_object *iob, int flag);


/* GSPR.C declarations */

extern void
 _PG_set_raster_text_scale(PG_device *dev, double sc),
 _PG_move_gr_abs(PG_device *dev, double *p),
 _PG_move_gr_rel(PG_device *dev, double *p),
 _PG_move_tx_abs(PG_device *dev, double *p),
 _PG_move_tx_rel(PG_device *dev, double *p);


/* GSPR_PS.C declarations */

extern void
 _PG_PS_set_dev_color(PG_device *dev, int prim, int check);

extern int
 _PG_PS_put_raster(PG_device *dev,
		   int xo, int yo, int nx, int ny);


/* GSPR_X.C declarations */

extern int
 _PG_X_put_raster(PG_device *dev, int bc,
		  int xo, int yo, int dx, int dy,
		  double ca, double sa),
 _PG_X_put_ximage(PG_device *dev, void *psi, int bc, int swc, int sbc,
		  int xo, int yo, int dx, int dy,
		  double ca, double sa);


/* GSRWI.C declarations */

extern void
 _PG_parent_limits(PG_device *dev, PG_interface_object *parent,
		   double *ndc);


/* GSSCALE.C declarations */

extern PG_axis_def
 *_PG_mk_axis_def(int atype, int ttype, int ltype, double *xl, double *xr,
		  double t1, double t2, double vn, double vx,
		  double as, int *iflg);

extern double
 _PG_axis_place(PG_device *dev, double *dx,
		PG_axis_def *ad, double sz, int tick);

extern void
 _PG_tick(PG_axis_def *ad, int tick),
 _PG_rl_axis_def(PG_axis_def *ad);


/* GSSFNT.C declarations */

extern void
 PG_def_stroke_font(int *marker_indices);


/* GSTXT.C declarations */

extern void
 _PG_draw_cursor(PG_text_box *b, int inv);


/* GSVSP.C declarations */

extern void
 _PG_find_clip_region(PG_device *dev, int *pc, int clip, int swflag),
 _PG_fix_wtos(PG_device *dev, int wh);


#ifdef __cplusplus
}
#endif

#endif

