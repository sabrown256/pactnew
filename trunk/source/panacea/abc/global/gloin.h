/*
 * GLOIN.H - header supporting the internals for mesh generation
 *         - for the LR global package
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#ifndef PCK_GLOIN

#define PCK_GLOIN

#include <sx.h>
#include <scheme_int.h>
#include "global.h"
#include "mesh.h"

enum e_line_case
   {PARLLL    =  1,
    OPPST     =  0,
    INTERSECT = -1,
    CONTAINED = -2};

typedef enum e_line_case line_case;

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

/* INTRS_P - return TRUE if two segments with the same direction have
 *         - a non-vanishing intersection
 */

#define INTRS_P(x1, x2, x3, x4)                                              \
    !(((x1 < x2) && ((x2-x3 < TOLERANCE) || (x4-x1 < TOLERANCE))) ||         \
      ((x1 > x2) && ((x3-x2 < TOLERANCE) || (x1-x4 < TOLERANCE))))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INTRS_A - return TRUE if two segments with opposite dircetion have
 *         - a non-vanishing intersection
 */

#define INTRS_A(x1, x2, x3, x4)                                              \
    !(((x1 < x2) && ((x2-x4 < TOLERANCE) || (x3-x1 < TOLERANCE))) ||         \
      ((x1 > x2) && ((x4-x2 < TOLERANCE) || (x1-x3 < TOLERANCE))))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SAME - return TRUE is the points are too close */

#define SAME(x1, y1, x2, y2)                                                 \
    ((ABS(x1-x2) < TOLERANCE) && (ABS(y1-y2) < TOLERANCE))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLOSETO - return TRUE is the numbers are close enough */

#define CLOSETO(x1, x2)                                                      \
    ((ABS(x1-x2) < TOLERANCE))

/*--------------------------------------------------------------------------*/

/*                           TYPEDEFS AND STRUCTS                           */

/*--------------------------------------------------------------------------*/

struct s_mesh_quality
   {double skew;
    double jac;
    double orth;};

typedef struct s_mesh_quality mesh_quality;

/*--------------------------------------------------------------------------*/

/*                           PACKAGE VARIABLES                              */

/*--------------------------------------------------------------------------*/

extern int
 graphics_init,
 mesh_acolor,
 mesh_bcolor,
 mesh_bndry,
 mesh_bndry_r,
 mesh_grid,
 mesh_kmax,
 mesh_kmin,
 mesh_lmax,
 mesh_lmin,
 mesh_mcolor,
 mesh_mesh,
 mesh_mesh_r,
 mesh_plots,
 mesh_plot_log,
 mesh_plot_phys,
 mesh_plots_r,
 mesh_scatter,
 mesh_tcolor,
 mesh_vcolor,
 mesh_vectr,
 mesh_vectr_r,
 window_init;

extern hasharr
 *curves,
 *swtab;

extern double
 *apk,
 *apl,
 *kra,
 *lra;

extern int
 N_parts,
 N_plots,
 N_regs;

extern char
 *DOMAIN_MAP_S,
 *MESH_CURVE;

extern double
 mesh_botspace,
 mesh_gxmax,
 mesh_gxmin,
 mesh_gymax,
 mesh_gymin,
 mesh_leftspace,
 mesh_rightspace,
 mesh_topspace,
 mesh_xmax,
 mesh_xmin,
 mesh_ymax,
 mesh_ymin,
 pc,
 xmn,
 xmx,
 ymn,
 ymx;

extern PG_device
 *mesh_display,
 *mesh_PS_display;

/*--------------------------------------------------------------------------*/

/*                           PACKAGE FUNCTIONS                              */

/*--------------------------------------------------------------------------*/

/* GPLOT.C declarations */

extern void
 global_end_graphics(int err),
 turn(void),
 viewh(void),
 plot(int new, char *name),
 printh(void),
 printscr(int new, char *name);

extern int
 make_frame(void),
 mesh_init_graphics_state(void),
 mesh_set_graphics_state(PG_device *d),
 meshlmt(void),
 plotw(PG_device *dev, int new, char *name),
 newframew(PG_device *dev, int new),
 matsw(PG_device *dev, int refl),
 plot_leg(PG_device *dev, PM_side *base, int refl),
 meshw(PG_device *dev, int refl),
 velw(PG_device *dev, int refl);


/* GINIT.C declarations */

extern int
 LR_get_data(double *pnz, double *pnc, double *pt, double *pdt,
		       char **prs, char **ped, char **ppp, char **pgf),
 part_reg(char *s);

extern void
 comp_volume(void),
 assign_globals(void);


/* GMESH.C declarations */

extern double
 comp_ratio(double b0, int nz);

extern void
 make_mesh(void),
 clearh(void),
 parth(void),
 recth(void),
 sideh(void),
 endpth(void),
 curveh(void),
 LR_clean_variable(double *v);

extern int
 what_axis(char *s, char **sub),
 rl_mesh(void),
 make_lrm(void),
 fill_mesh(PM_part *parts),
 resize(int npt, int nz),
 chck_part(PM_part *parts),
 map_tree(PM_side *base),
 map_base(PM_side *base),
 norm_cline(PM_conic_curve *cp),
 containsw(double xr, double yr),
 inside	 (double xr, double yr, double x1, double y1, double x2, double y2,
	  double x3, double y3, double x4, double y4),
 gen_dumpw(hasharr *curves);

extern PM_conic_curve
 *rd_cline(char *s),
 *atocur(char *s);

extern mesh_quality
 *compute_mesh_quality(void);


/* GPP.C declarations */

extern PM_set
 *_LR_get_n_set(PA_plot_request *pr, char *name),
 *_LR_get_pseudo_set(PA_plot_request *pr, char *name),
 *LR_build_domain(char *base_name, C_array *arr, double t),
 *LR_get_set(char *name, PM_centering *pcent, C_array *arr, int space);

extern PM_mapping
 *LR_build_mapping(PA_plot_request *pr, double t),
 *LR_build_pseudo_mapping(PA_plot_request *pr, double t);

extern int
 LR_flatten_space(PA_plot_request *pr),
 LR_flatten_plots(void),
 LR_get_domain_zone(PA_plot_request *pr),
 LR_plot_accept(PA_plot_request *pr, ...);

extern void
 LR_init_io(PA_package *pck, PFPreMap func),
 LR_set_set_limits(int *maxes, int centering),
 LR_expand_plot_data(PA_plot_request *pr, int ni);

extern double
 *LR_map_centers(double *data, int centering);

extern object
 *LR_int_plot(PG_device *dev, char *rname, PM_centering centering,
	      int nde, double **elem),
 *LR_var_plot(SS_psides *si, object *argl),
 *LR_get_time_data(void),
 *LR_get_mesh_data(void),
 *LR_def_domain(SS_psides *si, object *argl);

#endif

