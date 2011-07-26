/*
 * PDSZOF.C - byte size routines for PDB
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

enum e_PD_instr_rdwr
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

typedef enum e_PD_instr_rdwr PD_instr_rdwr;
 
#define SAVE_S(s, t)                                                         \
    {SZ_STR_STACK[SZ_STR_PTR++] = s;                                         \
     s = CSTRSAVE(t);}

#define RESTORE_S(s)                                                         \
    {CFREE(s);                                                               \
     s = SZ_STR_STACK[--SZ_STR_PTR];}

#define SAVE_I(val)                                                          \
    (SZ_LVAL_STACK[SZ_LVAL_PTR++].diskaddr = (long) val)

#define RESTORE_I(val)                                                       \
    (val = SZ_LVAL_STACK[--SZ_LVAL_PTR].diskaddr)

#define SAVE_P(val)                                                          \
    (SZ_LVAL_STACK[SZ_LVAL_PTR++].memaddr = (char *) val)

#define RESTORE_P(type, val)                                                 \
    (val = (type *) SZ_LVAL_STACK[--SZ_LVAL_PTR].memaddr)

#define SET_CONT(ret, frce)                                                  \
   {SZ_CALL_STACK[SZ_CALL_PTR++] = ret;                                      \
    dst = (_PD_indirection(ltype) && (frce || itags)) ? INDIRECT : LEAF;     \
    continue;}

#define SET_CONT_RD(ret, branch)                                             \
   {SZ_CALL_STACK[SZ_CALL_PTR++] = ret;                                      \
    dst = branch;                                                            \
    continue;}

#define GO_CONT                                                              \
   {dst = SZ_CALL_STACK[--SZ_CALL_PTR];                                      \
    continue;}

#define GO(lbl)                                                              \
    {dst = lbl;                                                              \
     continue;}

#define INIT_STACKS(_t, _d)                                                  \
    {long _a, _f;                                                            \
     if (SZ_LIST_T == 0)                                                     \
        {SZ_LIST_T = _t;                                                     \
	 SZ_LIST_D = _d;                                                     \
         SC_mem_stats(&_a, &_f, NULL, NULL);                                 \
	 DYN_STK(long, SZ_CALL_STACK, SZ_CALL_PTR, SZ_CALL_PTR_X);           \
	 DYN_STK(char *, SZ_STR_STACK, SZ_STR_PTR, SZ_STR_PTR_X);            \
	 DYN_STK(SC_address, SZ_LVAL_STACK, SZ_LVAL_PTR, SZ_LVAL_PTR_X);     \
         SC_mem_stats_set(_a, _f);};                                         \
     SZ_CALL_STACK[0L] = DONE;                                               \
     SZ_CALL_PTR = 1L;                                                       \
     SZ_LVAL_PTR = 0L;                                                       \
     SZ_STR_PTR  = 0L;}

#define DYN_STK(_t, _s, _p, _px)                                             \
    {if (_p >= _px)                                                          \
        {_px += SZ_LIST_D;                                                   \
	 if (_s == NULL)                                                     \
	    {_s = CPMAKE_N(_t, _px, 3);}                                     \
	 else                                                                \
	    CPREMAKE(_s, _t, _px, 3);};}

#define START                                                                \
    while (TRUE)                                                             \
       {DYN_STK(long, SZ_CALL_STACK, SZ_CALL_PTR, SZ_CALL_PTR_X);            \
	DYN_STK(char *, SZ_STR_STACK, SZ_STR_PTR, SZ_STR_PTR_X);             \
        DYN_STK(SC_address, SZ_LVAL_STACK, SZ_LVAL_PTR, SZ_LVAL_PTR_X);      \
	switch (dst) {

#define FINISH(f, tag)                                                       \
    default  :                                                               \
         snprintf(bf, MAXLINE, "UNDECIDABLE CASE - %s", f);                  \
         PD_error(bf, tag);};}


#define SZ_CALL_STACK    pa->sz_call_stack
#define SZ_CALL_PTR      pa->sz_call_ptr
#define SZ_CALL_PTR_X    pa->sz_call_ptr_x
#define SZ_LVAL_STACK    pa->sz_lval_stack
#define SZ_LVAL_PTR      pa->sz_lval_ptr
#define SZ_LVAL_PTR_X    pa->sz_lval_ptr_x
#define SZ_STR_STACK     pa->sz_str_stack
#define SZ_STR_PTR       pa->sz_str_ptr
#define SZ_STR_PTR_X     pa->sz_str_ptr_x
#define SZ_LIST_T        pa->sz_list_t
#define SZ_LIST_D        pa->sz_list_d

SC_THREAD_LOCK(PD_szof_lock);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SZ_ITAG - compute the byte size that the ITAG would incur */

static long _PD_sz_itag(char *type, int itags)
   {long nb;

    if (itags == FALSE)
       nb = 0;

    else
       {IF_SAFE(TRUE, &PD_szof_lock, _PD.szl == 0L)
	   _PD.szl = (long) ceil(log10(pow(256.0, ((double) sizeof(long)))));
	END_SAFE;

/* the itag format is "%ld\001%s\001%32ld\001%d\001\n"
 * allow for maximum space for 2 longs, a 32 digit address, and a string
 */
	nb = 2*_PD.szl + 32 + strlen(type) + 5;};

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SZ_LEAF_MEMBERS - write the direct leaf data */

static long _PD_sz_leaf_members(PDBfile *file, char *type, long nitems,
				void *vr)
   {long bpi, nb;
    defstr *dpf;

    dpf = _PD_lookup_type(type, file->chart);
    bpi = dpf->size;
    if (bpi == -1)
       PD_error("CAN'T GET NUMBER OF BYTES - _PD_SZ_LEAF_MEMBERS", PD_TRACE);

    nb = nitems*bpi;

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_SZ_ITAGS - handle the memory of pointers and write the itags
 *                  - correctly
 */

static int _PD_ptr_sz_itags(long *pnb, PDBfile *file, void *vr,
			    long nitems, char *type)
   {int ret, loc, itags;
    long i;

    itags = file->use_itags;

    *pnb += _PD_sz_itag(type, itags);

    loc = LOC_HERE;
    if ((_PD_IS_SEQUENTIAL) || (itags == FALSE))
       {i = _PD_ptr_wr_lookup(file, vr, &loc, TRUE);
	SC_ASSERT(i >= 0);};

    ret = (loc == LOC_HERE);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_SIZEOF - return the size in bytes of the space needed to contain
 *           - a PDB representation of the given object
 *           - NOTE: this always computes with an empty pointer list
 *           - which means that it can overestimate the actual space
 *           - a subsequent PD_write will use by the amount of space
 *           - for pointees already written
 *
 * #bind PD_sizeof fortran() scheme() python()
 */

long PD_sizeof(PDBfile *file ARG(,,cls),
	       char *type, long nitems, void *vri)
   {int dst, size, indir, itags;
    long i, nb;
    char bf[MAXLINE], *ltype, *vr, *svr, *ttype;
    defstr *dp;
    memdes *desc, *mem_lst;
    PD_smp_state *pa;
    SC_array *ap;

    nb = 0L;
    ap = NULL;

    i       = -1;
    size    = 0;
    svr     = NULL;
    mem_lst = NULL;
    desc    = NULL;
    ltype   = NULL;

    if (vri == NULL)
       return(nb);

    pa = _PD_get_state(-1);

    switch (SETJMP(pa->trace_err))
       {case ABORT :
	     return(0L);
        case ERR_FREE :
	     return(nb);
        default :
	     memset(pa->err, 0, MAXLINE);
	     break;};

    itags = file->use_itags;

    _PD_MARK_AS_FLUSHED(file, FALSE);

/* save the current ptr lists */
    if ((_PD_IS_SEQUENTIAL) || (itags == FALSE))
       _PD_ptr_save_ap(file, &ap, NULL, NULL);

    INIT_STACKS(100L, 1000L);

    dst = (_PD_indirection(type) && itags) ? INDIRECT : LEAF;

    if (dst == LEAF)
       {indir = FALSE;
	ltype = type;}
    else
       {indir = TRUE;
	SAVE_S(ltype, type);};

    vr = (char *) vri;

/* some AIX compilers will erroneously take the default case if
 * this is terminated with a semicolon
 */
    START

    case LEAF :
         nb += _PD_sz_leaf_members(file, ltype, nitems, vr);

         dp = PD_inquire_host_type(file, ltype);
         if (dp == NULL)
            PD_error("BAD TYPE - PD_SIZEOF", PD_TRACE);
         else
	    {mem_lst = dp->members;
	     if (dp->type[0] == '*')
	        {GO(INDIRECT);}
	     else if (!dp->n_indirects || (mem_lst == NULL))
	        GO_CONT;

/* if type is a struct with pointered members write them out now 
 * for an array of structs write the indirects for each array element
 */
	     size = dp->size;
	     svr  = vr;
	     i    = 0L;};

    case LEAF_ITEM :
         if (i >= nitems)
            GO_CONT;

         desc = mem_lst;

    case LEAF_INDIR :
         if (desc == NULL)
            {i++;
             svr += size;
             GO(LEAF_ITEM);};

         PD_CAST_TYPE(ttype, desc, svr+desc->member_offs, svr,
		      PD_error, "BAD CAST - PD_SIZEOF", PD_TRACE);

         SAVE_S(ltype, ttype);

         if (!_PD_indirection(ltype))
            {RESTORE_S(ltype);
             desc = desc->next;
             GO(LEAF_INDIR);};

         SAVE_I(nitems);
         nitems = desc->number;

         SAVE_I(i);
         SAVE_I(size);
         SAVE_P(mem_lst);
         SAVE_P(desc);
         SAVE_P(svr);
         SAVE_P(vr);
         vr = svr + desc->member_offs;
         SET_CONT(LEAF_RET, TRUE);

    case LEAF_RET :
         RESTORE_P(char, vr);
         RESTORE_P(char, svr);
         RESTORE_P(memdes, desc);
         RESTORE_P(memdes, mem_lst);
         RESTORE_I(size);
         RESTORE_I(i);
         RESTORE_I(nitems);
         RESTORE_S(ltype);

         desc = desc->next;
         GO(LEAF_INDIR);

    case INDIRECT :
    
         if (vr == NULL)
            {nb += _PD_sz_itag(ltype, itags);
             GO_CONT;};

/* dereference a local copy of the type */
         SAVE_S(ltype, ltype);
         PD_dereference(ltype);

/* write the data */
         i = 0L;

    case INDIR_ITEM :
         if (i >= nitems)
            {RESTORE_S(ltype);
             GO_CONT;};

         SAVE_P(vr);
         vr = DEREF(vr);
         if (vr == NULL)
            {nb += _PD_sz_itag(ltype, itags);
             RESTORE_P(char, vr);
             i++;
             vr += sizeof(char *);
             GO(INDIR_ITEM);};

         SAVE_I(nitems);
         nitems = _PD_number_refd(vr, ltype, file->host_chart);
         if (nitems == -1L)
            {snprintf(bf, MAXLINE,
                      "CAN'T GET POINTER LENGTH ON %s - PD_SIZEOF",
                     ltype);
             PD_error(bf, PD_TRACE);};

         if (nitems == -2L)
            {snprintf(bf, MAXLINE,
                      "UNKNOWN TYPE %s - PD_SIZEOF",
                     ltype);
             PD_error(bf, PD_TRACE);};

         if (!_PD_ptr_sz_itags(&nb, file, vr, nitems, ltype))
            {RESTORE_I(nitems);
             RESTORE_P(char, vr);
             i++;
             vr += sizeof(char *);
             GO(INDIR_ITEM);};

         SAVE_I(i);
         SAVE_S(ltype, ltype);
         SET_CONT(INDIR_RET, FALSE);

    case INDIR_RET :
         RESTORE_S(ltype);
         RESTORE_I(i);
         RESTORE_I(nitems);
         RESTORE_P(char, vr);

         i++;
         vr += sizeof(char *);

         GO(INDIR_ITEM);

    case DONE :
         if (indir)
	    {RESTORE_S(ltype);};

/* restore the initial ptr lists */
         if ((_PD_IS_SEQUENTIAL) || (itags == FALSE))
	    _PD_ptr_restore_ap(file, ap, NULL);

         return(nb);

    FINISH("PD_SIZEOF", PD_TRACE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PN_RELOCATE - change all the ITAGs to be relative to the current
 *             - buffer space
 *             - this is to cover the case of multiply referenced spaces
 *             - buffers which have been passed around
 *
 * #bind PN_relocate fortran() scheme() python()
 */

int PN_relocate(PDBfile *file, char *type, long n)
   {int ret;

    ret = FALSE;

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

