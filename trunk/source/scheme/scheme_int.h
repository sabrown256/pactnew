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

/* convert a numerical primitive type to an object */

#define _SS_NUMTYPE_TO_OBJ(_oid, _o, _vtyp, _vr, _n)                         \
    {_vtyp *_p;                                                              \
     _p = (_vtyp *) _vr;                                                     \
      if (_oid == SC_CHAR_I)                                                 \
        _o = SS_mk_integer(_p[_n]);                                          \
     else if (_oid == SC_BOOL_I)                                             \
        _o = SS_mk_boolean("#boolean", _p[_n]);                              \
     else if (SC_is_type_fix(_oid) == TRUE)                                  \
        _o = SS_mk_integer(_p[_n]);                                          \
     else if (SC_is_type_fp(_oid) == TRUE)                                   \
        _o = SS_mk_float(_p[_n]);                                            \
     else if (SC_is_type_cx(_oid) == TRUE)                                   \
        _o = SS_mk_complex(_p[_n]);                                          \
     else if (_oid == SC_QUATERNION_I)                                       \
        _o = SS_mk_quaternion(((quaternion *) _p)[_n]);}

#define _SS_QUATERNION_TO_OBJ(_oid, _o, _vr, _n)                             \
    {quaternion *_p;                                                         \
     _p = (quaternion *) _vr;                                                \
      if (_oid == SC_CHAR_I)                                                 \
        _o = SS_mk_integer(_p[_n].s);                                        \
     else if (_oid == SC_BOOL_I)                                             \
        _o = SS_mk_boolean("#boolean", _p[_n].s);                            \
     else if (SC_is_type_fix(_oid) == TRUE)                                  \
        _o = SS_mk_integer(_p[_n].s);                                        \
     else if (SC_is_type_fp(_oid) == TRUE)                                   \
        _o = SS_mk_float(_p[_n].s);                                          \
     else if (SC_is_type_cx(_oid) == TRUE)                                   \
        _o = SS_mk_complex(_p[_n].s);                                        \
     else if (_oid == SC_QUATERNION_I)                                       \
        _o = SS_mk_quaternion(_p[_n]);}

/* convert a numerical primitive array to a list */

#define _SS_NUMTYPE_TO_LIST(_oid, _lst, _vtyp, _vr, _n)                      \
    {long _i;                                                                \
     object *_o;                                                             \
     _lst = SS_null;                                                         \
     for (_i = 0L; _i < _n; _i++)                                            \
         {_SS_NUMTYPE_TO_OBJ(_oid, _o, _vtyp, _vr, _i);                      \
          _lst = SS_mk_cons(_o, _lst);};}

#define _SS_QUATERNION_TO_LIST(_oid, _lst, _vr, _n)                          \
    {long _i;                                                                \
     object *_o;                                                             \
     _lst = SS_null;                                                         \
     for (_i = 0L; _i < _n; _i++)                                            \
         {_SS_QUATERNION_TO_OBJ(_oid, _o, _vr, _i);                          \
          _lst = SS_mk_cons(_o, _lst);};}

/*--------------------------------------------------------------------------*/

/* convert an object to a numerical primitive type */

#define _SS_OBJ_TO_NUMTYPE(_vtyp, _vid, _vr, _i, _oid, _o)                   \
    {_vtyp *_pv;                                                             \
     _pv = (_vtyp *) _vr;                                                    \
     if (_vid == SC_QUATERNION_I)                                            \
        {quaternion _q;                                                      \
         _q.i = 0.0;                                                         \
         _q.j = 0.0;                                                         \
         _q.k = 0.0;                                                         \
         if ((_oid == SC_CHAR_I) || (_oid == SS_CHARACTER_I))                \
            _q.s = SS_CHARACTER_VALUE(_o);                                   \
         else if (_oid == SC_BOOL_I)                                         \
            _q.s = SS_BOOLEAN_VALUE(_o);                                     \
         else if (_oid == SC_INT_I)                                          \
            _q.s = SS_INTEGER_VALUE(_o);                                     \
         else if (_oid == SC_FLOAT_I)                                        \
            _q.s = SS_FLOAT_VALUE(_o);                                       \
         else if (_oid == SC_DOUBLE_COMPLEX_I)                               \
            {double _Complex _z;                                             \
             _z   = SS_COMPLEX_VALUE(_o);                                    \
             _q.s = creal(_z);                                               \
             _q.i = cimag(_z);}                                              \
         else if (_oid == SC_QUATERNION_I)                                   \
            _q = SS_QUATERNION_VALUE(_o);                                    \
         else                                                                \
            rv = FALSE;                                                      \
         if (rv == TRUE)                                                     \
            _pv[_i] = *(_vtyp *) &_q;}                                       \
     else if (SC_is_type_cx(_vid) == TRUE)                                   \
        {double _re, _im;                                                    \
         double _Complex _z;                                                 \
         _re = 0.0;                                                          \
         _im = 0.0;                                                          \
         if ((_oid == SC_CHAR_I) || (_oid == SS_CHARACTER_I))                \
            _re = SS_CHARACTER_VALUE(_o);                                    \
         else if (_oid == SC_BOOL_I)                                         \
            _re = SS_BOOLEAN_VALUE(_o);                                      \
         else if (_oid == SC_INT_I)                                          \
            _re = SS_INTEGER_VALUE(_o);                                      \
         else if (_oid == SC_FLOAT_I)                                        \
            _re = SS_FLOAT_VALUE(_o);                                        \
         else if (_oid == SC_DOUBLE_COMPLEX_I)                               \
            {_z = SS_COMPLEX_VALUE(_o);                                      \
             _re = creal(_z);                                                \
             _im = cimag(_z);}                                               \
         else if (_oid == SC_QUATERNION_I)                                   \
            {quaternion _q;                                                  \
             _q = SS_QUATERNION_VALUE(_o);                                   \
             _re = _q.s;                                                     \
             _im = _q.i;}                                                    \
         else                                                                \
            rv = FALSE;                                                      \
         if (rv == TRUE)                                                     \
            _pv[_i] = _re + _im*I;}                                          \
     else if ((SC_BIT_I <= _vid) && (_vid <= SC_LONG_DOUBLE_I))              \
        {if ((_oid == SC_CHAR_I) || (_oid == SS_CHARACTER_I))                \
            _pv[_i] = (_vtyp) SS_CHARACTER_VALUE(_o);                        \
         else if (_oid == SC_BOOL_I)                                         \
            _pv[_i] = (_vtyp) SS_BOOLEAN_VALUE(_o);                          \
         else if (_oid == SC_INT_I)                                          \
            _pv[_i] = (_vtyp) SS_INTEGER_VALUE(_o);                          \
         else if (_oid == SC_FLOAT_I)                                        \
            _pv[_i] = (_vtyp) SS_FLOAT_VALUE(_o);                            \
         else if (_oid == SC_DOUBLE_COMPLEX_I)                               \
            {double _Complex _z;                                             \
             _z = SS_COMPLEX_VALUE(_o);                                      \
             _pv[_i] = (_vtyp) creal(_z);}                                   \
         else if (_oid == SC_QUATERNION_I)                                   \
            {quaternion _q;                                                  \
             _q = SS_QUATERNION_VALUE(_o);                                   \
            _pv[_i] = (_vtyp) _q.s;}                                         \
         else                                                                \
            rv = FALSE;};}


#define _SS_OBJ_TO_QUATERNION(_vr, _i, _oid, _o)                             \
    {quaternion _q, *_pv;                                                    \
     _q.i = 0.0;                                                             \
     _q.j = 0.0;                                                             \
     _q.k = 0.0;                                                             \
     _pv = (quaternion *) _vr;                                               \
     if ((_oid == SC_CHAR_I) || (_oid == SS_CHARACTER_I))                    \
        _q.s = SS_CHARACTER_VALUE(_o);                                       \
     else if (_oid == SC_BOOL_I)                                             \
        _q.s = SS_BOOLEAN_VALUE(_o);                                         \
     else if (_oid == SC_INT_I)                                              \
        _q.s = SS_INTEGER_VALUE(_o);                                         \
     else if (_oid == SC_FLOAT_I)                                            \
        _q.s = SS_FLOAT_VALUE(_o);                                           \
     else if (_oid == SC_DOUBLE_COMPLEX_I)                                   \
        {double _Complex _z;                                                 \
         _z   = SS_COMPLEX_VALUE(_o);                                        \
         _q.s = creal(_z);                                                   \
         _q.i = cimag(_z);}                                                  \
     else if (_oid == SC_QUATERNION_I)                                       \
        _q = SS_QUATERNION_VALUE(_o);                                        \
     else                                                                    \
        rv = FALSE;                                                          \
     if (rv == TRUE)                                                         \
        _pv[_i] = _q;}


/* convert an object, list or array to a numerical primitive array */

#define _SS_LIST_TO_NUMTYPE(_vtyp, _vid, _vr, _n, _o)                        \
   {int _oid, _jtyp;                                                         \
    long _i;                                                                 \
    _oid = SC_arrtype(_o, -1);                                               \
    if (_oid == SS_CONS_I)                                                   \
       {object *_to;                                                         \
        if (SS_consp(SS_car(_o)))                                            \
           _o = SS_car(_o);                                                  \
        for (_i = 0; _i < _n; _i++)                                          \
            {_to   = SS_car(_o);                                             \
             _jtyp = SC_arrtype(_to, -1);                                    \
             _SS_OBJ_TO_NUMTYPE(_vtyp, _vid, _vr, _i, _jtyp, _to);           \
             _to = SS_cdr(_o);                                               \
             if (_to != SS_null)                                             \
                _o = _to;};}                                                 \
    else if (_oid == SS_VECTOR_I)                                            \
       {object **_ao;                                                        \
        _ao = SS_VECTOR_ARRAY(_o);                                           \
        for (_i = 0; _i < _n; _i++)                                          \
            {_jtyp = SC_arrtype(_ao[_i], -1);                                \
             _SS_OBJ_TO_NUMTYPE(_vtyp, _vid, _vr, _i, _jtyp, _ao[_i]);};}    \
    else                                                                     \
       _SS_OBJ_TO_NUMTYPE(_vtyp, _vid, _vr, 0, _oid, _o);}

#define _SS_LIST_TO_QUATERNION(_vr, _n, _o)                                  \
   {int _oid, _jtyp;                                                         \
    long _i;                                                                 \
    _oid = SC_arrtype(_o, -1);                                               \
    if (_oid == SS_CONS_I)                                                   \
       {object *_to;                                                         \
        if (SS_consp(SS_car(_o)))                                            \
           _o = SS_car(_o);                                                  \
        for (_i = 0; _i < _n; _i++)                                          \
            {_to   = SS_car(_o);                                             \
             _jtyp = SC_arrtype(_to, -1);                                    \
             _SS_OBJ_TO_QUATERNION(_vr, _i, _jtyp, _to);                     \
             _to = SS_cdr(_o);                                               \
             if (_to != SS_null)                                             \
                _o = _to;};}                                                 \
    else if (_oid == SS_VECTOR_I)                                            \
       {object **_ao;                                                        \
        _ao = SS_VECTOR_ARRAY(_o);                                           \
        for (_i = 0; _i < _n; _i++)                                          \
            {_jtyp = SC_arrtype(_ao[_i], -1);                                \
             _SS_OBJ_TO_QUATERNION(_vr, _i, _jtyp, _ao[_i]);};}              \
    else                                                                     \
       _SS_OBJ_TO_QUATERNION(_vr, 0, _oid, _o);}

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

extern C_procedure
 *_SS_mk_C_proc(PFPHand phand, int n, PFVoid *pr),
 *_SS_mk_C_proc_va(PFPHand phand, int n, ...);

extern procedure
 *_SS_mk_scheme_proc(char *pname, char *pdoc, SS_form ptype,
                     C_procedure *cp);

extern int
 _SS_object_map(FILE *fp, int flag),
 _SS_get_object_length(object *obj),
 _SS_object_to_numtype_id(int vid, void *p, long n, object *val),
 _SS_object_to_numtype(char *type, void *p, long n, object *val),
 _SS_list_to_numtype_id(int vid, void *p, long n, object *o),
 _SS_list_to_numtype(char *type, void *p, long n, object *o),
 _SS_max_numeric_type(object *argl, long *pn);

extern void
 _SS_rl_C_proc(C_procedure *cp),
 _SS_install(char* pname, char *pdoc, PFPHand phand,
             int n, PFVoid *pr, SS_form ptype);

extern object
 *_SS_numtype_to_object(char *type, void *p, long n),
 *_SS_numtype_to_list(char *type, void *p, long n);


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


/* SHPRM5.C declarations */

extern void
 _SS_install_quaternion(void);


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

