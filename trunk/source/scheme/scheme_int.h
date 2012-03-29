/*
 * SCHEME_INT.H - internal header file supporting Scheme system
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SCHEME_INT

#include "cpyright.h"

#define PCK_SCHEME_INT

#define LARGE

#include "ppc_int.h"
#include "scheme.h"
#include "scope_quaternion.h"

/*--------------------------------------------------------------------------*/

/*                           DEFINED CONSTANTS                              */

/*--------------------------------------------------------------------------*/

#define _SS_GET_C_PROCEDURE_N(_t, _cp, _n)  ((_t) (_cp)->proc[_n])

/*--------------------------------------------------------------------------*/

/*                           STRUCTs and TYPEDEFs                           */

/*--------------------------------------------------------------------------*/

typedef struct s_SS_scope_private SS_scope_private;
typedef struct s_SS_smp_state SS_smp_state;

struct s_SS_scope_private
   {

/* initializes to non-zero values */

/* SHENVR.C */
    int ita;

/* SHPRNT.C */
    int override;

/* SHINT.C */
    int ncx;

/* SHPRNT.C */
    int disp_flag_ext;           /* user level control of quote display */
    int disp_flag;               /* control display of quotes internally */
    char *bf;
    char *vbf;

/* initializes to 0 bytes */

/* SHFSUP.C */
    int f_mode_init;

/* SHPRM1.C */
    int nsym;
    int nest_level;
    object *watch_val;
    object *watch_var;
    PFPrintErrMsg oph;
     
/* SHMM.C */
    object *trace_object;

/* SHINT.C */
    char *ibf;

/* SHPROC.C */
    int n_tries;

/* SHREAD.C */
    hasharr *parser_tab;

/* SHSYNT.C */
    int have_eof;

/* SHTLEV.C */
    int active_objects;
    int exit_val;
    int ret;
    PFPrintErrMsg pr_err;

/* SHTTY.C */
    char *pr_prompt;
    PFfprintf pr_print;
    PFfputs pr_puts;
    PFfgets pr_gets;
    char str[MAXLINE];};

struct s_SS_smp_state
   {SS_psides si;
    object *(*parser)(SS_psides *si);};

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                              LIST MACROS                                 */

/*--------------------------------------------------------------------------*/

/* SS_END_CONS - macro version of the endcons functionality */

#define SS_end_cons(_si, _frst, _ths, _nxt)                                  \
    {object *_x;                                                             \
     _x = _nxt;                                                              \
     if (SS_nullobjp(_frst))                                                 \
        {_frst = SS_mk_cons(_si, _x, SS_null);                               \
         _ths = _frst;}                                                      \
     else                                                                    \
        {SS_setcdr(_si, _ths, SS_mk_cons(_si, _x, SS_null));                 \
         _ths = SS_cdr(_si, _ths);};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_END_CONS_MACRO - macro version of the endcons functionality
 *                   - mark the first element on the list
 *                   - treat both first and this as registers
 *                   - this distinguishes this macro from SS_end_cons
 */

#define SS_end_cons_macro(_si, _frst, _ths, _nxt)                            \
    {object *_x;                                                             \
     _x = _nxt;                                                              \
     if (SS_nullobjp(_frst))                                                 \
        {_frst = SS_mk_cons(_si, _x, SS_null);                               \
         SS_mark(_frst);                                                     \
         SS_assign(_si, _ths, _frst);}                                       \
     else                                                                    \
        {SS_setcdr(_si, _ths, SS_mk_cons(_si, _x, SS_null));                 \
         SS_assign(_si, _ths, SS_cdr(_si, _ths));};}

/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

extern SS_scope_private
 _SS;

/*--------------------------------------------------------------------------*/

/*                         FUNCTION DECLARATIONS                            */

/*--------------------------------------------------------------------------*/


/* SHARR.C declarations */

extern void
 _SS_inst_hash(SS_psides *si);


/* SHCHR.C declarations */

extern void
 _SS_inst_chr(SS_psides *si);


/* SHENVR.C declarations */

extern object
 *_SSI_defp(SS_psides *si, object *vr),
 *_SS_bind_envc(SS_psides *si, char *name, object *penv),
 *_SS_lk_var_valc(SS_psides *si, char *name, object *penv);

extern void
 _SS_rem_varc(SS_psides *si, char *name, object *penv);

extern char
 *_SS_get_print_name(SS_psides *si, object *o);

extern SS_smp_state
 *_SS_get_state(int id);


/* SHEVAL.C declarations */

extern void
 _SS_eval(SS_psides *si),
 _SS_inst_eval(SS_psides *si);


/* SHINT.C declarations */

extern object
 *_SS_make_list(SS_psides *si, int n, int *type, void **ptr);

extern int
 _SS_text_data_filep(FILE *fp, int cmnt);


#ifdef LARGE

/* SHLARG.C declarations */

extern void
 _SS_inst_lrg(SS_psides *si);


/* SHPROC.C declarations */

extern void
 _SS_inst_proc(SS_psides *si);

#endif


/* SHMM.C declarations */

extern C_procedure
 *_SS_mk_C_proc(PFPHand phand, int n, PFVoid *pr),
 *_SS_mk_C_proc_va(PFPHand phand, int n, ...);

extern procedure
 *_SS_mk_scheme_proc(char *pname, char *pdoc, SS_form ptype,
                     C_procedure *cp);

extern int
 _SS_object_map(SS_psides *si, FILE *fp, int flag),
 _SS_get_object_length(SS_psides *si, object *obj),
 _SS_object_to_numtype_id(int vid, void *p, long n, object *val),
 _SS_object_to_numtype(char *type, void *p, long n, object *val),
 _SS_list_to_numtype_id(SS_psides *si, int vid, void *p, long n, object *o),
 _SS_list_to_numtype(SS_psides *si, char *type, void *p, long n, object *o),
 _SS_max_numeric_type(SS_psides *si, object *argl, long *pn);

extern void
 _SS_rl_C_proc(C_procedure *cp),
 _SS_install(SS_psides *si, char* pname, char *pdoc, PFPHand phand,
             int n, PFVoid *pr, SS_form ptype);

extern object
 *_SS_numtype_to_object(SS_psides *si, char *type, void *p, long n),
 *_SS_numtype_to_list(SS_psides *si, char *type, void *p, long n);


/* SHPRM1.C declarations */

extern void
 _SS_bgn_trace(SS_psides *si, object *pfun, object *pargl),
 _SS_end_trace(SS_psides *si),
 _SS_inst_prm1(SS_psides *si);


/* SHPRM2.C declarations */

extern void
 _SS_install_math(SS_psides *si);


/* SHPRM3.C declarations */

extern object
 *_SS_endcons(SS_psides *si, object *list, object *obj);

extern void
 _SS_inst_prm3(SS_psides *si);


/* SHPRM4.C declarations */

extern void
 _SS_install_complex(SS_psides *si);


/* SHPRM5.C declarations */

extern void
 _SS_install_quaternion(SS_psides *si);


/* SHPRNT.C declarations */

extern void
 _SS_set_ans_prompt(SS_psides *si),
 _SS_inst_print(SS_psides *si);

extern char
 *_SS_vdsnprintf(int cp, char *fmt, va_list __a__),
 *_SS_sprintf(SS_psides *si, char *fmt, object *obj);


#ifdef LARGE

/* SHPROC.C declarations */

extern void
 _SS_rl_process(SS_psides *si, object *obj);

#endif


/* SHREAD.C declarations */

extern void
 _SS_inst_read(SS_psides *si);

extern object
 *_SSI_scheme_mode(SS_psides *si);


/* SHSTRG.C declarations */

extern void
 _SS_inst_str(SS_psides *si);


/* SHSYNT.C declarations */

extern int
 _SS_diagnose_return_synt(SS_psides *si, int x, char *y, PFPInt fnc);

extern void
 _SS_push_token(SS_psides *si, char *s),
 _SS_unsupported_syntax(SS_psides *si, char *msg),
 _SS_diagnostic_synt(SS_psides *si, object *expr, char *msg, int diag);


/* SHTLEV.C declarations */

extern void
 _SS_inst_prm(SS_psides *si),
 _SS_restore_state(SS_psides *si, object *esc_proc);


#ifdef __cplusplus
}
#endif

#endif

