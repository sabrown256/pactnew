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

#define UL_CURVE_INDEX_I   _UL.tnames[0]
#define UL_CURVE_INDEX_J   _UL.tnames[1]
#define UL_DATA_ID_I       _UL.tnames[2]
#define N_UL_TYPES         3

#define PRINT_DOUBLE(_x)                                                     \
    PRINT(stdout, "%s", SC_ftos(NULL, -1, FALSE, SS_gs.fmts[1], _x))

/*--------------------------------------------------------------------------*/

/*                                 TYPEDEFS                                 */

/*--------------------------------------------------------------------------*/

typedef struct s_UL_scope_public UL_scope_public;

struct s_UL_scope_public
   {
    int simple_append;
    int save_intermediate;

    double derivative_tolerance;
    double window_height_factor;
    double *bfa[2];
    double *bfb[2];};

typedef struct s_UL_scope_private UL_scope_private;

struct s_UL_scope_private
   {

/* ULCMD.C */
    int tnames[N_UL_TYPES];

/* ULTRA.C */
    object *crva;
    object *crvb;
    char bf[MAXLINE];};

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                           VARIABLE DECLARATIONS                          */

/*--------------------------------------------------------------------------*/

extern UL_scope_public
 UL_gs;

extern UL_scope_private
 _UL;

/*--------------------------------------------------------------------------*/

/*                          PROCEDURE DECLARATIONS                          */

/*--------------------------------------------------------------------------*/


/* ULCMD.C declarations */

extern SS_psides
 *UL_init(const char *code, const char *vers,
	  int c, char **v, char **env);

extern void
 UL_init_curves(SS_psides *si),
 UL_init_env(SS_psides *si),
 SX_mode_ultra(SS_psides *si, int load_init, int load_rc, int track);


/* ULAUXF.C declarations */

extern object
 *UL_fft(SS_psides *si, int j),
 *UL_curve_eval(SS_psides *si, object *arg),
 *UL_dupx(SS_psides *si, int j),
 *UL_getx(SS_psides *si, object *obj, object *tok),
 *UL_gety(SS_psides *si, object *obj, object *tok);

extern void
 UL_install_aux_funcs(SS_psides *si);

extern PM_matrix
 *UL_lsq(PM_matrix *a, PM_matrix *ay);


/* ULCNTL.C declarations */

extern void
 UL_init_hash(void),
 UL_install_global_vars(SS_psides *si),
 UL_plot_limits(PG_device *dev, int pflg, double *wc),
 UL_pause(SS_psides *si, int pf);

extern char
 *_UL_id_str(int i, int j);

extern object
 *_ULI_describe(SS_psides *si, object *obj),
 *_ULI_apropos(SS_psides *si, object *obj);


/* ULFUNC.C declarations */

extern void
 _UL_quit(SS_psides *si, int i),
 UL_install_scheme_funcs(SS_psides *si),
 UL_install_funcs(SS_psides *si);

extern object
 *UL_delete(SS_psides *si, object *s),
 *UL_sort(SS_psides *si, int k),
 *UL_get_crv_list(SS_psides *si),
 *_UL_make_ln(SS_psides *si, double slope, double interc,
	      double first, double last, int n);


/* ULHAND.C declarations */

extern void
 UL_lmt(double *pt, int n, double *mymin, double *mymax),
 UL_check_order(SS_psides *si, double *p, int n, int i);

extern int
 UL_curve_strp(object *obj);

extern object
 *UL_us(SS_psides *si, SS_C_procedure *cp, object *argl),
 *UL_uc(SS_psides *si, SS_C_procedure *cp, object *argl),
 *UL_opxc(SS_psides *si, SS_C_procedure *cp, object *argl),
 *UL_opyc(SS_psides *si, SS_C_procedure *cp, object *argl),
 *UL_ul2toc(SS_psides *si, SS_C_procedure *cp, object *argl),
 *UL_ul2tocnp(SS_psides *si, SS_C_procedure *cp, object *argl),
 *UL_ulntoc(SS_psides *si, SS_C_procedure *cp, object *argl),
 *UL_uopxc(SS_psides *si, SS_C_procedure *cp, object *argl),
 *UL_uopyc(SS_psides *si, SS_C_procedure *cp, object *argl),
 *UL_bftoc(SS_psides *si, SS_C_procedure *cp, object *argl),
 *UL_bltoc(SS_psides *si, SS_C_procedure *cp, object *argl),
 *UL_bltocnp(SS_psides *si, SS_C_procedure *cp, object *argl),
 *UL_bc(SS_psides *si, SS_C_procedure *cp, object *argl),
 *UL_bcxl(SS_psides *si, SS_C_procedure *cp, object *argl);


/* ULTRA.C declarations */

#ifdef DEBUG

extern int
 heapch;

#endif

extern object
 *UL_mode_text(SS_psides *si),
 *UL_mode_graphics(SS_psides *si),
 *UL_plot(SS_psides *si),
 *_ULI_printscr(SS_psides *si),
 *_ULI_set_id(SS_psides *si, object *argl),
 *_ULI_thru(SS_psides *si, object *s),
 *UL_copy_curve(SS_psides *si, int j),
 *_ULI_extract_curve(SS_psides *si, object *argl),
 *UL_xindex_curve(SS_psides *si, int j);

extern void
 UL_init_view(SS_psides *si),
 UL_set_graphics_state(PG_device *d);


#ifdef __cplusplus
}
#endif

#endif

