/*
 * PDRW.H - lifted from pdrdwr.c
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

enum e_PD_instr_kind
   {DONE        = 1,
    LEAF        = 2,
    LEAF_ITEM   = 3,
    LEAF_RET    = 4,
    LEAF_INDIR  = 5,
    INDIRECT    = 6,
    INDIR_ITEM  = 7,
    INDIR_RET   = 8,
    BLOCK       = 9,
    BLOCK_ITEM  = 10,
    BLOCK_RET   = 11,
    SKIP_TO     = 12,
    SKIP_RET    = 13};

typedef enum e_PD_instr_kind PD_instr_kind;
 
#define SAVE_S(s, t)                                                         \
    {str_stack[str_ptr++] = s;                                               \
     s = CSTRSAVE(t);}

#define RESTORE_S(s)                                                         \
    {CFREE(s);                                                               \
     s = str_stack[--str_ptr];}

#define SAVE_I(val)     lval_stack[lval_ptr++].diskaddr = (long) val;
#define RESTORE_I(val)  val = lval_stack[--lval_ptr].diskaddr;

#define SAVE_P(val)           lval_stack[lval_ptr++].memaddr = (char *) val;
#define RESTORE_P(type, val)  val = (type *) lval_stack[--lval_ptr].memaddr;

#define SET_CONT(ret)                                                        \
   {call_stack[call_ptr++] = ret;                                            \
    dst = _PD_indirection(litype) ? INDIRECT : LEAF;                         \
    continue;}

#define SET_CONT_RD(ret, branch)                                             \
   {call_stack[call_ptr++] = ret;                                            \
    dst = branch;                                                            \
    continue;}

#define GO_CONT                                                              \
   {dst = call_stack[--call_ptr];                                            \
    continue;}

#define GO(lbl)                                                              \
    {dst = lbl;                                                              \
     continue;}

#define START                                                                \
    while (TRUE) {switch (dst) {

#define FINISH(f, ret)                                                       \
    default  :                                                               \
         PP_error_set_user(NULL, "UNDECIDABLE CASE");                        \
         return ret;};}


#if 0
/* also in pputil.h */
#define PP_CAST_TYPE(t, d, vg, vl, err_obj, ret_val)                         \
    {if (d->cast_offs < 0L)                                                  \
        t = d->type;                                                         \
     else                                                                    \
        {t = DEREF(vl + d->cast_offs);                                       \
         if (t == NULL)                                                      \
            {if (DEREF(vg) != NULL)                                          \
                {PP_error_set_user(err_obj, "bad cast");return ret_val;}     \
             else                                                            \
                t = d->type;};};}
#endif
