/*
 * PYPDBD.C - release memory of the give type
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbmodule.h"
#include "pdbrw.h"

#if 0
#define DEBUG_REL(name, type, nitems, vr)  printstr(name, type, nitems, vr)
#else
#define DEBUG_REL(name, type, nitems, vr)
#endif

#if 0
#define DEBUG_MARK(name, type, nitems, vr)  printstr(name, type, nitems, vr)
#else
#define DEBUG_MARK(name, type, nitems, vr)
#endif

static long
    call_ptr = 0L,
    call_stack[1000],
    lval_ptr = 0L,
    str_ptr = 0L;

static SC_address
    lval_stack[1000];

static char
   *str_stack[1000];


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#if 0

static printstr(char *name, char *type, long nitems, void *vr)
{
    static FILE *outto = stderr;
    syment *ep;

    fprintf(outto, "------ %s --------------\n", name);
    fprintf(outto, "type :  %s   len : %ld\n", type, nitems);
    ep = _PD_mk_syment(type, nitems, 0L, NULL, NULL);
    PD_write_entry(outto, PP_vif, "xxx", vr, ep, 0, NULL);
    _PD_rl_syment(ep);
}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_REL_SYMENT - 
 *               - this is written in a continuation passing
 *               - style so that PDB has control over the stack and
 *               - isn't blowing out the execution stack for long linked
 *               - lists
 */

long _PP_rel_syment(hasharr *host_chart, char *vr, long nitems, char *intype)
{
    int dst, size, indir;
    int err;
    long i;
    char *litype, *ttype, *svr;
    defstr *dp;
    memdes *desc, *mem_lst;
    char *first;

    i = 0;
    size = 0;
    err = 0;
    svr = NULL;
    desc = NULL;
    mem_lst = NULL;
    first = vr;

    call_ptr = 0L;
    lval_ptr = 0L;
    str_ptr  = 0L;
    litype   = NULL;

    call_stack[call_ptr++] = DONE;
    dst = _PD_indirection(intype) ? INDIRECT : LEAF;

    if (dst == LEAF) {
        indir = FALSE;
	litype = intype;
    } else {
        indir = TRUE;
	SAVE_S(litype, intype);
    }

/* some AIX compilers will erroneously take the default case if
 * this is terminated with a semicolon
 */
    START
        
/* NOTE: count on this being right and _PD_effective_ep will handle all
 * issues about partial reads across discontiguous blocks by correctly
 * making an effective syment for which this logic works!!!!!!
 */
    case LEAF:

/* the host type must be used to get the correct member offsets for the
 * in memory copy - the file ones might be wrong!!
 */
        dp = PD_inquire_table_type(host_chart, litype);
        if (dp == NULL) {
            PP_error_set_user(NULL, "bad type: %s", litype);
            err = -1;
            GO_CONT;
        }

        mem_lst = dp->members;
        if (!dp->n_indirects || (mem_lst == NULL))
            GO_CONT;

/* if type is a struct with pointered members write them out now 
 * for an array of structs write the indirects for each array element
 */
        size = dp->size;
        svr  = vr;
        i    = 0L;

     case LEAF_ITEM:
         if (i >= nitems) {
             GO_CONT;
         }

         desc = mem_lst;

     case LEAF_INDIR:               /* LEAF_MEMBER */
         if (desc == NULL) {
             i++;
             svr += size;
             GO(LEAF_ITEM);
         }

         PP_CAST_TYPE(ttype, desc, svr + desc->member_offs, svr, Py_None, -1);

         SAVE_S(litype, ttype);

         if (!_PD_indirection(ttype)) {
             RESTORE_S(litype);
             desc = desc->next;
             GO(LEAF_INDIR);
         }

         SAVE_I(nitems);
         nitems = desc->number;

         SAVE_I(i);
         SAVE_I(size);
         SAVE_P(mem_lst);
         SAVE_P(desc);
         SAVE_P(svr);
         SAVE_P(vr);
         vr = svr + desc->member_offs;
         SET_CONT(LEAF_RET);

     case LEAF_RET:
         RESTORE_P(char, vr);
         RESTORE_P(char, svr);
         RESTORE_P(memdes, desc);
         RESTORE_P(memdes, mem_lst);
         RESTORE_I(size);
         RESTORE_I(i);
         RESTORE_I(nitems);
         RESTORE_S(litype);

         desc = desc->next;
         GO(LEAF_INDIR);

     case INDIRECT:
         if (vr == NULL) {
             GO_CONT;
         }

         /* dereference a local copy of the type */
         SAVE_S(litype, litype);
         SC_dereference(litype);

         i = 0L;
         SAVE_P(vr);  /* start of indirect */

     case INDIR_ITEM:
         if (i >= nitems) {
             RESTORE_P(char, vr);  /* start of indirect */
             if (vr != first )
                 CFREE(vr);
             RESTORE_S(litype);
             GO_CONT;
         }

         SAVE_P(vr);
         vr = DEREF(vr);
         if (vr == NULL) {
             RESTORE_P(char, vr);
             i++;
             vr += sizeof(char *);
             GO(INDIR_ITEM);
         }

         SAVE_I(nitems);
         nitems = _PD_number_refd(mem_lst, svr, desc, vr,
				  litype, host_chart);
         if (nitems == -1L) {
             PP_error_set_user(NULL,
                               "error from _PP_number_refd called from _PP_rel_syment, type %s",
                               litype);
             err = -1;
             RESTORE_P(char, vr);
             i++;
             vr += sizeof(char *);
             GO(INDIR_ITEM);
         }

         if (nitems == -2L) {
             PP_error_set_user(NULL,
                               "UNKNOWN TYPE %s - _PD_WR_SYMENT",
                               litype);
             err = -1;
             RESTORE_P(char, vr);
             i++;
             vr += sizeof(char *);
             GO(INDIR_ITEM);
         }

         SAVE_I(i);
         SET_CONT(INDIR_RET);

     case INDIR_RET:
         if (vr != first )
             CFREE(vr);
         RESTORE_I(i);
         RESTORE_I(nitems);
         RESTORE_P(char, vr);

         i++;
         vr += sizeof(char *);

         GO(INDIR_ITEM);

     case DONE:
         if (indir) {
	     RESTORE_S(litype);
         }

         return err;

     FINISH("_PP_REL_SYMENT", -1);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
