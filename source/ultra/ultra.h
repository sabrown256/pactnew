/*
 * ULTRA.H - the main header supporting the Ultra system
 *
 * Source Version: 4.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_ULTRA

#include "cpyright.h"
 
#define PCK_ULTRA

#include "sx_int.h"

#undef CODE
#define CODE "ULTRA 2.5"

#define UL_CURVE_INDEX_I 500
#define UL_CURVE_INDEX_J 502
#define UL_DATA_ID_I     501   

/*--------------------------------------------------------------------------*/

/*                                 TYPEDEFS                                 */

/*--------------------------------------------------------------------------*/

/* use the same values at PG_text_alignment so the SCHEME left, right, center
 * constants can be used for both
 */
enum e_UL_hist_mode
   {HIST_LEFT,
    HIST_RIGHT,
    HIST_CENTER};

typedef enum e_UL_hist_mode UL_hist_mode;


#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                           VARIABLE DECLARATIONS                          */

/*--------------------------------------------------------------------------*/

extern int
 UL_simple_append,
 UL_save_intermediate;

extern double
 UL_derivative_tolerance;

extern double
 UL_window_height_factor,
 *UL_buf1x,
 *UL_buf1y,
 *UL_buf2x,
 *UL_buf2y;

extern object
 *UL_ann_lst;

/*--------------------------------------------------------------------------*/

/*                          PROCEDURE DECLARATIONS                          */

/*--------------------------------------------------------------------------*/


/* ULAUXF.C declarations */

extern object
 *UL_fft(int j),
 *UL_get_value(double *sp, double *vp, double val, int n, int id),
 *UL_curve_eval(object *arg),
 *UL_getx(object *obj, object *tok),
 *UL_gety(object *obj, object *tok),
 *UL_fit(object *obj, object *tok),
 *UL_dupx(int j),
 *UL_stat(int j),
 *UL_disp(int j, double d1, double d2);

extern void
 UL_install_aux_funcs(void);

extern PM_matrix
 *UL_lsq(PM_matrix *a, PM_matrix *ay);


/* ULCNTL.C declarations */

extern void
 UL_init_hash(void),
 UL_install_global_vars(void),
 UL_plot_limits(PG_device *dev, int pflg, double *wc),
 UL_pause(int pf);

extern object
 *_ULI_describe(SS_psides *si, object *obj),
 *_ULI_apropos(object *obj);


/* ULFUNC.C declarations */

extern void
 _UL_quit(int i),
 UL_install_scheme_funcs(void),
 UL_install_funcs(void);

extern object
 *UL_delete(object *s),
 *UL_sort(int k),
 *UL_get_crv_list(void),
 *_UL_make_ln(double slope, double interc, 
	      double first, double last, int n);


/* ULHAND.C declarations */

extern void
 UL_lmt(double *pt, int n, double *mymin, double *mymax),
 UL_check_order(double *p, int n, int i);

extern int
 UL_curve_strp(object *obj);

extern object
 *UL_us(SS_psides *si, C_procedure *cp, object *argl),
 *UL_uc(SS_psides *si, C_procedure *cp, object *argl),
 *UL_opxc(SS_psides *si, C_procedure *cp, object *argl),
 *UL_opyc(SS_psides *si, C_procedure *cp, object *argl),
 *UL_ul2toc(SS_psides *si, C_procedure *cp, object *argl),
 *UL_ul2tocnp(SS_psides *si, C_procedure *cp, object *argl),
 *_UL_ul2toc(SS_psides *si, C_procedure *cp, object *argl, int flag),
 *UL_ulntoc(SS_psides *si, C_procedure *cp, object *argl),
 *UL_uopxc(SS_psides *si, C_procedure *cp, object *argl),
 *UL_uopyc(SS_psides *si, C_procedure *cp, object *argl),
 *UL_bftoc(SS_psides *si, C_procedure *cp, object *argl),
 *UL_bltoc(SS_psides *si, C_procedure *cp, object *argl),
 *UL_bltocnp(SS_psides *si, C_procedure *cp, object *argl),
 *UL_bc(SS_psides *si, C_procedure *cp, object *argl),
 *UL_bcxl(SS_psides *si, C_procedure *cp, object *argl);


/* ULTRA.C declarations */

#ifdef DEBUG

extern int
 heapch;

#endif

extern object
 *UL_mode_text(SS_psides *si),
 *UL_mode_graphics(SS_psides *si),
 *UL_plot(void),
 *_ULI_printscr(SS_psides *si),
 *_ULI_set_id(SS_psides *si, object *argl),
 *_ULI_thru(SS_psides *si, object *s),
 *UL_copy_curve(int j),
 *_ULI_extract_curve(SS_psides *si, object *argl),
 *UL_xindex_curve(int j);

extern void
 UL_init_view(void),
 UL_set_graphics_state(PG_device *d),
 UL_print_banner(void);


#ifdef __cplusplus
}
#endif

#endif

