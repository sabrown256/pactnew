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

#define SX_prep_arg(argl)                                                    \
    {argl = SX_arg_prep(argl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_PREP_RET - prepare the return value by delistifying one element lists */

#define SX_prep_ret(ret)                                                     \
    {if (SS_length(ret) == 1)                                               \
        {SS_Assign(ret, SS_car(ret));}                                       \
     else                                                                    \
        {SS_Assign(ret, SS_reverse(ret));};                                  \
     SC_mark(ret, -1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_PREP_RETL - prepare the return value by reversing multi-element lists
 *                this macro does NOT delistify one element lists
 */

#define SX_prep_retl(ret)                                                    \
    {if (SS_length(ret) > 1)                                                \
        {SS_Assign(ret, SS_reverse(ret));};                                  \
     SC_mark(ret, -1);}

/*--------------------------------------------------------------------------*/

/* SX_LAST_ARG - strip off the last element of the arg list
 *             - potential GC problem here
 */

#define SX_last_arg(tok, argl)                                               \
   {object *t;                                                               \
    for (t = argl, tok = SS_null; SS_consp(t); t = SS_cdr(t))                \
        {SS_Assign(tok, SS_cadr(t));                                         \
         if (SS_nullobjp(SS_cddr(t)))                                        \
            {SS_setcdr(t, SS_null);                                          \
	     break;};};}

/*--------------------------------------------------------------------------*/

/*                           STRUCT DEFINITIONS                             */

/*--------------------------------------------------------------------------*/

enum e_SX_file_action
   {DESTROY = 5,
    APPEND,
    FAIL};

typedef enum e_SX_file_action SX_file_action;

typedef struct s_SX_state SX_state;

struct s_SX_state
   {

/* initializes to non-zero values */

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
    double chi_mn;
    double chi_mx;
    double phi_mn;
    double phi_mx;
    double th_mn;
    double th_mx;

    PG_device *gri;

/* SXPAN.C */
    char *rsname;
    char *edname;
    char *ppname;
    char *gfname;

/* SXPDB.C */
    int iwcu;

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

/* SXPDBD.C */
    int err_fpe;
    JMP_BUF diff_err;

    out_device output_devices[N_OUTPUT_DEVICES];
    char command[MAXLINE];
    char line[MAX_BFSZ];

    };

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

extern SX_state
 _SX;

extern SX_file_action
 SX_file_exist_action;

/*--------------------------------------------------------------------------*/

/*                         FUNCTION DECLARATIONS                            */

/*--------------------------------------------------------------------------*/


/* SXCONT.C declarations */

extern object
 *_SX_call_args(int type, void *v),
 *_SXI_menu(SS_psides *si, object *argl);

extern void
 _SX_args(object *obj, void *v, int type),
 _SX_get_menu(g_file *po),
 _SX_push_menu_item(g_file *po, char *name, char *type);


/* SXCRV.C declarations */

extern object
 *_SXI_curveobjp(object *obj);


/* SXEXT.C declarations */

extern void
 SX_install_ext_funcs(void);


/* SXFUNC.C declarations */

extern void
 _SX_mf_inst_g(void);


/* SXGC.C declarations */

extern void
 _SX_gc_data(PDBfile *file, void *vr, long nitems, char *type),
 _SX_gc_indirection(PDBfile *file, char **vr, long nitems, char *type),
 _SX_gc_leaf(PDBfile *file, char *vr, long nitems, char *type);


/* SXGRI.C declarations */

extern void
 _SX_install_pgs_iob(void);


/* SXGROT.C declarations */

extern int
 _SX_grotrian_mappingp(PM_mapping *f),
 _SX_grotrian_graphp(PG_graph *g),
 _SX_any_grotrianp(object *argl);

extern object
 *_SXI_grotrian_mappingp(object *obj),
 *_SXI_grotrian_graphp(object *obj);


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
 *_SXI_extract_mapping(PM_mapping *h, object *argl),
 *_SXI_refine_mapping(PM_mapping *h, object **pargl),
 *_SXI_interp_mapping(PM_mapping *h, object **pargl);


/* SXHOOK.C declarations */

extern memdes
 *_SX_hash_hook(PDBfile *file, char *vr, defstr *dp);

extern void
 _SX_ins_mem(defstr *dp, char *member, int imem, PDBfile *File),
 _SX_init_hash_objects(PDBfile *file);


/* SXMM.C declarations */

extern object
 *_SX_mk_gpdbdata(char *name, void *data, syment *ep, PDBfile *file),
 *_SX_mk_gsyment(syment *ep),
 *_SX_mk_gdefstr(defstr *dp);

extern g_file
 *_SX_mk_file(char *name, char *type, void *file),
 *_SX_mk_open_file(char *name, char *type, char *mode);

extern g_pdbdata
 *_SX_mk_pdbdata(char *name, void *data, syment *ep, PDBfile *file);

extern void
 _SX_free_menu(g_file *po, int re),
 _SX_rel_open_file(g_file *po);


/* SXPANW.C declarations */

extern object
 *_SXI_display_pan_object(object *obj),
 *_SXI_desc_pan(object *obj);


/* SXPDB.C declarations */

extern int
 _SX_file_varp(PDBfile *file, char *name, int flag),
 _SX_read_entry(PDBfile *fp, char *path, char *ty, syment *ep, void *vr);

extern void
 SX_type_container(char *dtype, char *stype);

extern object
 *_SX_open_file(object *arg, char *type, char *mode),
 *_SX_pdbfile_to_list(PDBfile *file),
 *_SX_syment_to_list(syment *ep),
 *_SX_defstr_to_list(defstr *dp),
 *_SX_pdbdata_to_list(char *name, void *vr, syment *ep, PDBfile *file),
 *_SX_make_dims_obj(dimdes *dims);

extern dimdes
 *_SX_make_dims_dimdes(PDBfile *file, object *argl);

extern syment
 *_SX_write_entry(PDBfile *fp, char *path, char *inty, char *outty,
		  void *vr, dimdes *dims);


/* SXPDBC.C declarations */

extern void
 _SX_copy_tree(PDBfile *file, char *vrin, char *vrout,
	       long nitems, char *type);


/* SXPDBD.C declarations */

extern int
 _SX_type_equal(PDBfile *file_a, PDBfile *file_b, char *typea, char *typeb);

extern object
 *_SXI_diff_var(SS_psides *si, object *argl),
 *_SXI_display_diff(SS_psides *si, object *arg);


/* SXPDBF.C declarations */

extern object
 *_SXI_find_types(object *arg);


/* SXPDBL.C declarations */

extern object
 *_SX_make_list_syment(PDBfile *file, void *vr, long nitems, char *type);


/* SXPDBR.C declarations */

extern void
 _SX_rd_tree_list(object *obj, PDBfile *file, char *vr,
		  long nitems, char *type, dimdes *dims);

extern object
 *_SXI_read_numeric_data(SS_psides *si, object *argl);


/* SXPGS.C declarations */

extern SX_menu_item
 *_SX_get_menu_item(g_file *po, int i);


/* SXPML.C declarations */

extern object
 *_SXI_list_array(SS_psides *si, object *argl);


/* SXSET.C declarations */

extern int
 _SX_get_input(object *str);


/* SXSHAR.C declarations */

extern void
 SX_install_file_funcs(void);


/* SXTABLE.C declarations */

extern void
 SX_install_ascii_funcs(void);


/* SXULIO.C declarations */

extern object
 *_SXI_valid_ultra_filep(object *obj);


#ifdef __cplusplus
}
#endif

#endif

