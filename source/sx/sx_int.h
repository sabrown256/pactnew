/*
 * SX_INTERNAL.H - internal header file for SX
 *               - Scheme with PACT Extensions
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#ifndef PCK_SX_INTERNAL

#define PCK_SX_INTERNAL

#include "panacea_int.h"
#include "scheme_int.h"
#include "sx.h"

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                                CONSTANTS                                 */

/*--------------------------------------------------------------------------*/

#define LITERAL   1
#define NOPADDING 2

/*--------------------------------------------------------------------------*/

/*                            PROCEDURAL MACROS                             */

/*--------------------------------------------------------------------------*/

/* SX_PREP_ARG - prepare the arg list by merging lists and
 *             - delistifying one element lists
 *             - NOTE: SX_arg_prep is doing the ref count right
 *             -       so that a simple assignment must be done here!!!
 */

#define SX_prep_arg(_si, _a)                                                 \
    {_a = SX_arg_prep(_si, _a);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_PREP_RET - prepare the return value by delistifying one element lists */

#define SX_prep_ret(_si, _rv)                                                \
    {if (SS_length(_si, _rv) == 1)                                           \
        {SS_assign(_si, _rv, SS_car(_si, _rv));}                             \
     else                                                                    \
        {SS_assign(_si, _rv, SS_reverse(_si, _rv));};                        \
     SC_mark(_rv, -1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_PREP_RETL - prepare the return value by reversing multi-element lists
 *                this macro does NOT delistify one element lists
 */

#define SX_prep_retl(_si, _rv)                                               \
    {if (SS_length(_si, _rv) > 1)                                            \
        {SS_assign(_si, _rv, SS_reverse(_si, _rv));};                        \
     SC_mark(_rv, -1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_LAST_ARG - strip off the last element of the arg list
 *             - potential GC problem here
 */

#define SX_last_arg(_si, _s, _a)                                             \
   {object *_t;                                                              \
    for (_t = _a, _s = SS_null; SS_consp(_t); _t = SS_cdr(_si, _t))          \
        {SS_assign(_si, _s, SS_cadr(_si, _t));                               \
         if (SS_nullobjp(SS_cddr(_si, _t)))                                  \
            {SS_setcdr(_si, _t, SS_null);                                    \
	     break;};};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define SX_CAST_TYPE(_si, t, d, vg, vl, _errm, _erra)                        \
    {if (d->cast_offs < 0L)                                                  \
        t = d->type;                                                         \
     else                                                                    \
        {t = DEREF(vl + d->cast_offs);                                       \
         if (t == NULL)                                                      \
            {if (DEREF(vg) != NULL)                                          \
                SS_error(_si, _errm, _erra);                                 \
             else                                                            \
                t = d->type;};};}

/*--------------------------------------------------------------------------*/

/*                           STRUCT DEFINITIONS                             */

/*--------------------------------------------------------------------------*/

enum e_SX_file_action
   {DESTROY = 5,
    APPEND,
    FAIL};

typedef enum e_SX_file_action SX_file_action;

typedef struct s_SX_scope_private SX_scope_private;

struct s_SX_scope_private
   {

/* initializes to non-zero values */

/* SXCONT.C */
    SX_file_action file_exist_action;

/* SXPGS.C */
    int color;

/* SXPAN.C */
    double dtf_min;
    double dtf_max;
    double dtf_inc;

/* SXFUNC.C */
    int mid;

/* SXPDB.C */
    int string_mode;

/* SXPGS.C */
    int ig;
    int icg;
    int igg;

    char cache_filename[32];

/* ULAUXF.C */
    int fft_order;

/* initializes to 0 bytes */

/* ULFUNC.C */
    int old_autoplot;

/* ULCNTL.C */
    double last_state;

/* SXCRV.C */
    int next_avail;
    object **crv_obj;
    object **crv_proc;
    object **crv_varbl;
    C_procedure *cproc;

/* SXFUNC.C */
    char *fname;

/* SXGRI.C */
    double ea_mn[PG_SPACEDM];
    double ea_mx[PG_SPACEDM];
    PG_device *gri;

/* SXPAN.C */
    char *rsname;
    char *edname;
    char *ppname;
    char *gfname;

/* SXPDB.C */
    int iwcu;

/* SXPDBD.C */
    int promote_flag;
    int promote_fixed;
    int promote_float;

/* SXPML.C */
    int ap;
    int api;

/* SXSET.C */
    PFfgets gets;

/* SXULIO.C */
    int icw;
    int icc;

    hasharr *files;
    PDBfile *cache_file;
    PM_matrix *current_table;
    pcons *file_list;
    unsigned int dataptr;

    long table_ln;
    int table_n;
    char *table_name;

    out_device output_devices[N_OUTPUT_DEVICES];
    char command[MAXLINE];
    char line[MAX_BFSZ];

    };

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

extern SX_scope_private
 _SX;

/*--------------------------------------------------------------------------*/

/*                         FUNCTION DECLARATIONS                            */

/*--------------------------------------------------------------------------*/


/* SXCONT.C declarations */

extern object
 *_SX_call_args(SS_psides *si, int type, void *v),
 *_SXI_menu(SS_psides *si, object *argl);

extern void
 _SX_args(SS_psides *si, object *obj, void *v, int type),
 _SX_get_menu(SS_psides *si, g_file *po),
 _SX_push_menu_item(SS_psides *si, g_file *po, char *name, char *type);


/* SXCRV.C declarations */

extern object
 *_SXI_curveobjp(SS_psides *si, object *obj);


/* SXEXT.C declarations */

extern void
 SX_install_ext_funcs(SS_psides *si);


/* SXFUNC.C declarations */

extern void
 _SX_mf_inst_g(SS_psides *si);


/* SXGC.C declarations */


/* SXGRI.C declarations */

extern void
 _SX_install_pgs_iob(SS_psides *si);


/* SXGROT.C declarations */

extern int
 _SX_grotrian_mappingp(PM_mapping *f),
 _SX_grotrian_graphp(PG_graph *g),
 _SX_any_grotrianp(SS_psides *si, object *argl);

extern object
 *_SXI_grotrian_mappingp(SS_psides *si, object *obj),
 *_SXI_grotrian_graphp(SS_psides *si, object *obj);


/* SXHAND.C declarations */

extern object
 *_SX_mh_u_s(SS_psides *si, C_procedure *cp, object *argl),
 *_SX_mh_u_s_nm(SS_psides *si, C_procedure *cp, object *argl),
 *_SX_m11_x(SS_psides *si, C_procedure *cp, object *argl),
 *_SX_m11_b_mds(SS_psides *si, C_procedure *cp, object *argl),
 *_SX_m11_b_mro(SS_psides *si, C_procedure *cp, object *argl),
 *_SX_m11_b_mrs(SS_psides *si, C_procedure *cp, object *argl),
 *_SX_mh_u_m(SS_psides *si, C_procedure *cp, object *argl),
 *_SX_mh_u_o(SS_psides *si, C_procedure *cp, object *argl),
 *_SX_mh_u_v(SS_psides *si, C_procedure *cp, object *argl),
 *_SX_ah_m(SS_psides *si, C_procedure *cp, object *argl);


/* SXHBO.C declarations */

extern object
 *_SX_mh_b_s(SS_psides *si, C_procedure *cp, object *argl);

extern PM_mapping
 *_SXI_extract_mapping(SS_psides *si, PM_mapping *h, object *argl),
 *_SXI_refine_mapping(SS_psides *si, PM_mapping *h, object **pargl),
 *_SXI_interp_mapping(SS_psides *si, PM_mapping *h, object **pargl);


/* SXHOOK.C declarations */

extern memdes
 *_SX_hash_hook(PDBfile *file, char *vr, defstr *dp);

extern void
 _SX_init_hash_objects(SS_psides *si, PDBfile *file);


/* SXMM.C declarations */

extern object
 *_SX_mk_gpdbdata(SS_psides *si, char *name,
		  void *data, syment *ep, PDBfile *file),
 *_SX_mk_gsyment(SS_psides *si, syment *ep),
 *_SX_mk_gdefstr(SS_psides *si, defstr *dp);

extern g_file
 *_SX_mk_file(char *name, char *type, void *file),
 *_SX_mk_open_file(SS_psides *si, char *name, char *type, char *mode);

extern g_pdbdata
 *_SX_mk_pdbdata(char *name, void *data, syment *ep, PDBfile *file);

extern void
 _SX_free_menu(g_file *po, int re),
 _SX_rel_open_file(SS_psides *si, g_file *po);


/* SXPANW.C declarations */

extern object
 *_SXI_display_pan_object(SS_psides *si, object *obj),
 *_SXI_desc_pan(SS_psides *si, object *obj);


/* SXPDB.C declarations */

extern int
 _SX_file_varp(PDBfile *file, char *name, int flag),
 _SX_read_entry(PDBfile *fp, char *path, char *ty, syment *ep, void *vr);

extern void
 _SX_type_container(char *dtype, char *stype);

extern object
 *_SX_open_file(SS_psides *si, object *arg, char *type, char *mode),
 *_SX_pdbfile_to_list(SS_psides *si, PDBfile *file),
 *_SX_syment_to_list(SS_psides *si, syment *ep),
 *_SX_defstr_to_list(SS_psides *si, defstr *dp),
 *_SX_pdbdata_to_list(SS_psides *si, char *name, void *vr,
		      syment *ep, PDBfile *file),
 *_SX_make_dims_obj(SS_psides *si, dimdes *dims);

extern syment
 *_SX_write_entry(PDBfile *fp, char *path, char *inty, char *outty,
		  void *vr, dimdes *dims);


/* SXPDBC.C declarations */

extern void
 _SX_copy_tree(SS_psides *si, PDBfile *file, char *vrin, char *vrout,
	       inti ni, char *type);


/* SXPDBD.C declarations */

extern int
 _SX_type_equal(PDBfile *file_a, PDBfile *file_b, char *typea, char *typeb);

extern object
 *_SXI_diff_var(SS_psides *si, object *argl),
 *_SXI_display_diff(SS_psides *si, object *arg);


/* SXPDBF.C declarations */

extern object
 *_SXI_find_types(SS_psides *si, object *arg);


/* SXPDBL.C declarations */

extern object
 *_SX_make_list_syment(SS_psides *si, PDBfile *file,
		       void *vr, inti ni, char *type);


/* SXPDBR.C declarations */

extern void
 _SX_rd_tree_list(SS_psides *si, object *obj, PDBfile *file, char *vr,
		  inti ni, char *type, dimdes *dims);

extern object
 *_SXI_read_numeric_data(SS_psides *si, object *argl);


/* SXPGS.C declarations */

extern SX_menu_item
 *_SX_get_menu_item(SS_psides *si, g_file *po, int i);


/* SXPML.C declarations */

extern PM_set
 *SX_rep_to_ac(char *name, double *rx, double *ry,
	       int n_nodes, int n_zones, int *zones);

extern object
 *_SXI_list_array(SS_psides *si, object *argl);


/* SXSET.C declarations */

extern int
 _SX_get_input(SS_psides *si, object *str);

extern void
 _SX_pdbview_mode(SS_psides *si, char *code, int load_init, int load_rc);


/* SXSHAR.C declarations */

extern void
 SX_install_file_funcs(SS_psides *si);


/* SXTABLE.C declarations */

extern void
 SX_install_ascii_funcs(SS_psides *si);


/* SXULIO.C declarations */

extern object
 *_SXI_valid_ultra_filep(SS_psides *si, object *obj);


#ifdef __cplusplus
}
#endif

#endif

