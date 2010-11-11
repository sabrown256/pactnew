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

/*--------------------------------------------------------------------------*/

/*                           DEFINED CONSTANTS                              */

/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/

/*                           STRUCTs and TYPEDEFs                           */

/*--------------------------------------------------------------------------*/

typedef struct s_SS_state SS_state;
typedef struct s_SS_smp_state SS_smp_state;

struct s_SS_state
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
    PFfprintf pr_print;
    PFfputs pr_puts;
    PFfgets pr_gets;
    char str[MAXLINE];};

struct s_SS_smp_state
   {PFPObject parser;};

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                              LIST MACROS                                 */

/*--------------------------------------------------------------------------*/

/* SS_END_CONS - macro version of the endcons functionality */

#define SS_end_cons(first, ths, nxt)                                        \
    {object *x;                                                             \
     x = nxt;                                                               \
     if (SS_nullobjp(first))                                                \
        {first = SS_mk_cons(x, SS_null);                                    \
         ths = first;}                                                      \
     else                                                                   \
        {SS_setcdr(ths, SS_mk_cons(x, SS_null));                            \
         ths = SS_cdr(ths);};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_END_CONS_MACRO - macro version of the endcons functionality
 *                   - mark the first element on the list
 *                   - treat both first and this as registers
 *                   - this distinguishes this macro from SS_end_cons
 */

#define SS_end_cons_macro(first, ths, nxt)                                  \
    {object *x;                                                             \
     x = nxt;                                                               \
     if (SS_nullobjp(first))                                                \
        {first = SS_mk_cons(x, SS_null);                                    \
         SS_MARK(first);                                                    \
         SS_Assign(ths, first);}                                            \
     else                                                                   \
        {SS_setcdr(ths, SS_mk_cons(x, SS_null));                            \
         SS_Assign(ths, SS_cdr(ths));};}

/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

extern SS_state
 _SS;

/*--------------------------------------------------------------------------*/

/*                         FUNCTION DECLARATIONS                            */

/*--------------------------------------------------------------------------*/


/* SHARR.C declarations */

extern void
 _SS_inst_hash(void);

extern object
 *SS_mk_hasharr(hasharr *tb);


/* SHCHR.C declarations */

extern void
 _SS_inst_chr(void);


/* SHENVR.C declarations */

extern object
 *_SS_bind_envc(char *name, object *penv),
 *_SS_lk_var_valc(char *name, object *penv);

extern void
 _SS_rem_varc(char *name, object *penv),
 _SS_def_varc(char *name, object *vl, object *penv);

extern char
 *_SS_get_print_name(object *o);

extern SS_smp_state
 *_SS_get_state(int id);


/* SHEVAL.C declarations */

extern void
 _SS_eval(void),
 _SS_inst_eval(void);


/* SHINT.C declarations */

extern int
 _SS_text_data_filep(FILE *fp, int cmnt);


#ifdef LARGE

/* SHLARG.C declarations */

extern void
 _SS_inst_lrg(void);


/* SHPROC.C declarations */

extern void
 _SS_inst_proc(void);

#endif


/* SHMM.C declarations */

extern int
 _SS_object_map(FILE *fp, int flag);


/* SHPRM1.C declarations */

extern void
 _SS_inst_prm1(void);


/* SHPRM2.C declarations */

extern void
 _SS_install_math(void);


/* SHPRM3.C declarations */

extern object
 *_SS_endcons(object *list, object *obj);

extern void
 _SS_inst_prm3(void);


/* SHPRM4.C declarations */

extern void
 _SS_install_complex(void);


/* SHPRNT.C declarations */

extern void
 _SS_set_ans_prompt(void),
 _SS_inst_print(void);

extern char
 *_SS_vdsnprintf(int cp, char *fmt, va_list __a__),
 *_SS_sprintf(char *fmt, object *obj);


#ifdef LARGE

/* SHPROC.C declarations */

extern void
 _SS_rl_process(object *obj);

#endif


/* SHREAD.C declarations */

extern void
 _SS_inst_read(void);

extern object
 *_SSI_scheme_mode(void);


/* SHSTRG.C declarations */

extern void
 _SS_inst_str(void);


/* SHSYNT.C declarations */

extern int
 _SS_diagnose_return_synt(int x, char *y, PFPInt fnc);

extern void
 _SS_push_token(char *s),
 _SS_diagnostic_synt(object *expr, char *msg, int diag);


/* SHTLEV.C declarations */

extern void
 _SS_restore_state(object *esc_proc);


#ifdef __cplusplus
}
#endif

#endif

