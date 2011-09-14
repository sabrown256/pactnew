/*
 * PDRDWR.C - new read/write routines for PDBLib
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
    {RW_STR_STACK[RW_STR_PTR++] = s;                                         \
     s = CSTRSAVE(t);}

#define RESTORE_S(s)                                                         \
    {CFREE(s);                                                               \
     s = RW_STR_STACK[--RW_STR_PTR];}

#define SAVE_I(val)                                                          \
    (RW_LVAL_STACK[RW_LVAL_PTR++].diskaddr = (int64_t) val)

#define RESTORE_I(val)                                                       \
    (val = RW_LVAL_STACK[--RW_LVAL_PTR].diskaddr)

#define SAVE_P(val)                                                          \
    (RW_LVAL_STACK[RW_LVAL_PTR++].memaddr = (char *) val)

#define RESTORE_P(type, val)                                                 \
    (val = (type *) RW_LVAL_STACK[--RW_LVAL_PTR].memaddr)

#define SET_CONT(ret, frce)                                                  \
   {RW_CALL_STACK[RW_CALL_PTR++] = ret;                                      \
    dst = (_PD_indirection(litype) && (frce || itags)) ? INDIRECT : LEAF;    \
    continue;}

#define SET_CONT_RD(ret, branch)                                             \
   {RW_CALL_STACK[RW_CALL_PTR++] = ret;                                      \
    dst = branch;                                                            \
    continue;}

#define GO_CONT                                                              \
   {dst = RW_CALL_STACK[--RW_CALL_PTR];                                      \
    continue;}

#define GO(lbl)                                                              \
    {dst = lbl;                                                              \
     continue;}

#define DYN_STK(_t, _s, _p, _px)                                             \
    {if (_p >= _px)                                                          \
        {_px += RW_LIST_D;                                                   \
	 if (_s == NULL)                                                     \
	    {_s = CPMAKE_N(_t, _px, 3);}                                     \
	 else                                                                \
	    CPREMAKE(_s, _t, _px, 3);};}

#define START                                                                \
    count = 0;                                                               \
    while (TRUE)                                                             \
       {DYN_STK(long, RW_CALL_STACK, RW_CALL_PTR, RW_CALL_PTR_X);            \
	DYN_STK(char *, RW_STR_STACK, RW_STR_PTR, RW_STR_PTR_X);             \
        DYN_STK(SC_address, RW_LVAL_STACK, RW_LVAL_PTR, RW_LVAL_PTR_X);      \
	count++;                                                             \
	switch (dst) {

#define FINISH(f, tag)                                                       \
    default  :                                                               \
         snprintf(bf, MAXLINE, "UNDECIDABLE CASE - %s", f);                  \
         PD_error(bf, tag);};}


#define RW_CALL_STACK      pa->rw_call_stack
#define RW_CALL_PTR        pa->rw_call_ptr
#define RW_CALL_PTR_X      pa->rw_call_ptr_x
#define RW_LVAL_STACK      pa->rw_lval_stack
#define RW_LVAL_PTR        pa->rw_lval_ptr
#define RW_LVAL_PTR_X      pa->rw_lval_ptr_x
#define RW_STR_STACK       pa->rw_str_stack
#define RW_STR_PTR         pa->rw_str_ptr
#define RW_STR_PTR_X       pa->rw_str_ptr_x
#define RW_LIST_T          pa->rw_list_t
#define RW_LIST_D          pa->rw_list_d

/*--------------------------------------------------------------------------*/

/*                          AUXILLIARY ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* _PD_NUMBER_REFD - THREADSAFE
 *                 - compute the number of items pointed to by VR
 *                 - return the number of items if successful
 *                 - return -1 if SCORE did NOT allocate the block
 *                 - return -2 if the type is unknown
 */

long _PD_number_refd(void *vr, char *type, hasharr *tab)
   {long bpi, ni;

    if (vr == NULL)
       return(0L);

    ni = SC_arrlen(vr);
    if (ni <= 0)
       return(-1L);

    bpi = _PD_lookup_size(type, tab);
    if (bpi == -1)
       return(-2L);

    ni /= bpi;

    return(ni);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INDIRECTION - THREADSAFE
 *                 - return TRUE if the TYPE is an indirect type
 *                 - i.e. has a "*" as the last non-blank character
 */

int _PD_indirection(char *s)
   {int rv;
    char *t;

    rv = FALSE;
    for (t = s + strlen(s); t >= s; t--)
        if (*t == '*')
           {rv = TRUE;
	    break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEREFERENCE - THREADSAFE
 *                - starting at the end of the string work backwards to
 *                - the first non-blank character and if it is a '*'
 *                - insert '\0' in its place
 *                - return a pointer to the beginning of the string
 *
 * #bind PD_dereference fortran() scheme() python()
 */

char *PD_dereference(char *s)
   {char *rv;

    rv = SC_dereference(s);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                           HYPER INDEX SUPPORT                            */

/*--------------------------------------------------------------------------*/

/* _PD_INIT_DIMIND - THREADSAFE
 *                 - fill a dimind struct given the stride and an
 *                 - ASCII index expression
 */

static void _PD_init_dimind(dimind *pi, long offset, long stride, char *expr)
   {char s[MAXLINE], *token, *t;
    long start, stop, step;

    if (expr != NULL)
       strcpy(s, expr);
    else
       s[0] = '\0';

    token = SC_strtok(s, " \t:", t);
    if (token == NULL)
       start = 0L;
    else
       start = atol(token);

    token = SC_strtok(NULL, " \t:", t);
    if (token == NULL)
       stop = start;
    else
       stop = atol(token);

    token = SC_strtok(NULL, " \t:", t);
    if (token == NULL)
       step = 1L;
    else
       step = atol(token);

    pi->stride = stride;
    pi->start  = start - offset;
    pi->stop   = stop - offset;
    pi->step   = step;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_COMPUTE_HYPER_STRIDES - THREADSAFE
 *                           - initialize and return an array of dimension
 *                           - indexes representing the hyper strides
 *                           - from the given hyper index expression
 */

static dimind  *_PD_compute_hyper_strides(PDBfile *file, char *ind,
					  dimdes *dims, int *pnd)
   {int i, nd, max_nd, off;
    long maxs, old_start, old_stop;
    dimdes *pd, *curr_dim;
    dimind *pi;

    if (dims == NULL)
       {pi = CMAKE(dimind);

        _PD_init_dimind(pi, (long) file->default_offset, 0L,
                        SC_firsttok(ind, ",()[]\n\r"));

        *pnd = 1;
        return(pi);};

/* count the number of dimensions and allocate some temporaries */
    for (nd = 0, pd = dims; pd != NULL; pd = pd->next, nd++);
    pi = CMAKE_N(dimind, nd);

    off = PD_get_offset(file);

/* pre-compute the strides, offsets, and so on for the hyper-space walk */
    if (file->major_order == COLUMN_MAJOR_ORDER)
       {maxs = 1L;
        for (i = nd - 1, pd = dims; i >= 0; i--)
            {if (pd != NULL)
                {_PD_init_dimind(&pi[i], pd->index_min, maxs,
				 SC_firsttok(ind, ",()[]\n\r"));
		 maxs *= pd->number;
                 pd    = pd->next;};};}

    else if (file->major_order == ROW_MAJOR_ORDER)
       {for (maxs = 1L, pd = dims->next; pd != NULL; pd = pd->next)
            maxs *= pd->number;

        for (i = 0, pd = dims; i < nd; i++)
            {_PD_init_dimind(&pi[i], pd->index_min, maxs,
                             SC_firsttok(ind, ",()[]\n\r"));
             if (pd->next != NULL)
                {pd    = pd->next;
                 maxs /= pd->number;};};};

/* Now: optimize any contiguous regions within the destination's 
 *      dimension index descriptor "dimind *pi" so that stride is 1
 *      there and start...stop are extended to include the whole region 
 */

/* remember the maximum number of dimensions */ 
    max_nd = nd;
 
/* find the inner most dimension descriptor */
    curr_dim = dims;

    if (file->major_order == ROW_MAJOR_ORDER)
       {for (i = 0 ; i < (nd-1) ; i++ )
            {curr_dim = curr_dim->next; }; }; 

/* while the inner dimensions are contiguous, back out one dimension */
    while ( (nd-1) > 0 && pi[nd-1].start == (curr_dim->index_min - off) &&
                          pi[nd-1].stop  == (curr_dim->index_max - off) &&
                          pi[nd-1].step  == 1) 
       {nd--;  /* Step back one dimension */

/* find the proper next dimension descriptor */
        if (file->major_order == COLUMN_MAJOR_ORDER) 
           {curr_dim = curr_dim->next; }
        else if (file->major_order == ROW_MAJOR_ORDER)
           {curr_dim = dims;

            for (i = 0 ; i < (nd-1) ; i++ )
                {curr_dim = curr_dim->next; }; }; };

/* if stepsize is not 1 at this dimension, no contiguity is 
 * possible here and we should then optimize just the next dimension in
 * if the next dimension in exists 
 */
    if (pi[nd-1].step != 1 && (nd+1) <= max_nd)
       {nd++; };
 
 /* optimize the dimind by extending the current start..stop to
  * encompass all inner contiguous regions
  *
  * This optimization works for the case where the entire index  
  * expression is contiguous as well as just partial contiguity
  *
  * Note that if nd == max_nd, then no optimizations are done 
  * because there are no inner dimensions that are contiguous
  *
  * Note that we are optimizing the destination dim descriptor
  * and _not_ the source dim descriptor because the source
  * is assumed to always be contiguous
  */ 
    if ( (nd-1) >= 0 && nd < max_nd )
       {old_start = pi[nd-1].start;
        old_stop  = pi[nd-1].stop;

/* scale up start */
        pi[nd-1].start  = (old_start * pi[nd-1].stride);

/* scale up stop pt and set it w.r.t. start pt */
        pi[nd-1].stop   = (((old_stop - old_start) + 1) /* length is */
                                  * pi[nd-1].stride)    /* expanded by contiguous stride */
                                  + pi[nd-1].start - 1; /* and reset by offset */

/* make _PD_wr/rd_hyper_index stop recursing here */
        pi[nd-1].stride = 1;};

    *pnd = nd;

    return(pi);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_HYPER_NUMBER - THREADSAFE
 *                  - return the number of elements implied by a hyper
 *                  - index expression
 *                  -
 *                  -    <expr>  := <spec> | <expr>, <spec>
 *                  -    <spec>  := <start> |
 *                  -               <start>:<stop> |
 *                  -               <start>:<stop>:<step>
 *                  -    <start> := starting integer index value
 *                  -    <stop>  := ending integer index value
 *                  -    <step>  := integer index step value
 *                  -
 */

long _PD_hyper_number(PDBfile *file, char *indxpr, long numb, dimdes *dims, long *poff)
   {int nd;
    long i, maxs, sum, offs;
    char s[MAXLINE];
    dimind *pi;

    strcpy(s, indxpr);
    pi = _PD_compute_hyper_strides(file, s, dims, &nd);

    offs = 0L;
    sum  = 1L;
    for (i = 0; i < nd; i++)
        {maxs  = (pi[i].stop - pi[i].start + pi[i].step)/pi[i].step;
         offs += pi[i].start*pi[i].stride;
         sum  *= maxs;};

    CFREE(pi);

    if (poff != NULL)
       *poff = offs;

    return(sum);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_HYPER_NUMBER - THREADSAFE
 *                 - return the number of elements implied by a hyper
 *                 - index expression
 *
 * #bind PD_hyper_number fortran() scheme() python()
 */

long PD_hyper_number(PDBfile *file ARG(,,cls), char *name, syment *ep)
   {int c;
    long rv;
    char s[MAXLINE];

/* if name is of the form a[...] strip off the name part
 * by design _PD_hyper_number can't handle anything but the index part
 */
    strcpy(s, name);
    c = s[0];
    if (strchr("0123456789-.", c) == NULL)
       SC_firsttok(s, "([");

    rv = _PD_hyper_number(file, s,
			  PD_entry_number(ep),
			  PD_entry_dimensions(ep), NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_VALID_DIMS - Make sure the dimension information specified in a variable
 *                - name is valid
 */

int _PD_valid_dims(dimdes *dimscheck, dimdes *vardims)
   {dimdes *od, *nd;
    int nndims, nodims;

/* count the dimensions */
    for (nodims = 0, od = vardims;
         od != NULL;
         nodims++, od = od->next);

    for (nndims = 0, nd = dimscheck;
         nd != NULL;
         nndims++, nd = nd->next);

    if (nodims != nndims)
       return(FALSE);

    if (nodims == 0)
       return(TRUE);

    for (od = vardims, nd = dimscheck;
	     (od != NULL) && (nd != NULL);
	     od = od->next, nd = nd->next)
            {if ((od->index_min > nd->index_min) ||
                 (od->index_max < nd->index_max))
                return(FALSE);}

    return(TRUE);}
   

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_HYPER_DIMS - return a reduced dimension descriptor for a hyper slab
 *                - preserve size and shape, eliminate degenerate dimensions
 */

dimdes *_PD_hyper_dims(PDBfile *file, char *name, dimdes *dims)
   {dimdes *ndims, *prev, *next, *dp;
    char s[MAXLINE];
    char *t;
    long n, start, stop, step;

    ndims = NULL;

    strcpy(s, name);
    t = SC_lasttok(s, "[]()");
    strcpy(s, t);

    for (dp = dims; dp != NULL; dp = dims->next)
       {t = SC_firsttok(s, " ,()[]\n\r");
	if (t == NULL)
	   break;

	n = _PD_parse_index_expr(t, dp, &start, &stop, &step);
	if (n <= 1)
	   continue;

	next = _PD_mk_dimensions(file->default_offset, n);

        if (ndims == NULL)
           ndims = next;
        else
	   {prev->next = next;
	    SC_mark(next, 1);};

	prev = next;};

    return(ndims);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_EXPAND_HYPER_NAME - Return hyper-indexed name with any partial index
 *                       - expressions fully expanded
 */

char *_PD_expand_hyper_name(PDBfile *file, char *name)
   {int err;
    syment *ep;
    dimdes *dims, *pd;
    long start, stop, step;
    char s[MAXLINE], lname[MAXLINE], index[MAXLINE];
    char *t, *rv;

    strcpy(s, name);
    if (SC_LAST_CHAR(s) != ']')
       return(CSTRSAVE(s));
    if (SC_NTH_LAST_CHAR(s, 1) == '[')
       return(NULL);

    strcpy(lname, s);
    t = strrchr(lname, '[');
    if (t != NULL)
       t[0] = '\0';

    ep = _PD_effective_ep(file, lname, TRUE, NULL);
    if (ep == NULL)
      return(NULL);

    dims = PD_entry_dimensions(ep);
    if (dims == NULL)
       {_PD_rl_syment_d(ep);
	return(CSTRSAVE(s));};

    t = SC_lasttok(s, "[]()");
    strcpy(s, t);

    err = FALSE;
    strcpy(index, "[");
    for (pd = dims; pd != NULL; pd = pd->next)
        {t = SC_firsttok(s, " ,()[]\n\r");
	 if (t == NULL)
	    {err = TRUE;
	     break;};

	 if (_PD_parse_index_expr(t, pd, &start, &stop, &step) == 0)
	    {err = TRUE;
	     break;};

         if (start == stop)
            SC_vstrcat(index, MAXLINE, "%ld,", start);

         else if (step <= 1L)
            SC_vstrcat(index, MAXLINE, "%ld:%ld,", start, stop);

         else
            SC_vstrcat(index, MAXLINE, "%ld:%ld:%ld,", start, stop, step);};
    
    _PD_rl_syment_d(ep);

    rv = NULL;
    if (err == 0)
       {SC_LAST_CHAR(index) = ']';
        snprintf(s, MAXLINE, "%s%s", lname, index);
        rv = CSTRSAVE(s);};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                            WRITE ROUTINES                                */

/*--------------------------------------------------------------------------*/

/* _PD_WR_LEAF_MEMBERS - write the direct leaf data */

static void _PD_wr_leaf_members(PDBfile *file, char *intype, char *outtype, 
                                long nitems, void *vr)
   {int cnv, ipt;
    long bpi;
    size_t ni, nb;
    char *in, *out, *bf;
    FILE *fp;
    defstr *dpf, *dph;

    fp  = file->stream;
    bpi = -1;
    ni  = 0;

    dpf = _PD_lookup_type(outtype, file->chart);
    if (dpf == NULL)
       PD_error("CANNOT FIND TYPE IN THE FILE CHART - _PD_WR_LEAF_MEMBERS", PD_WRITE);
    else
       bpi = dpf->size;

    ipt = _PD_items_per_tuple(dpf);

    nitems *= ipt;
       
    if (dpf->convert == -1)
       {dph = _PD_lookup_type(intype, file->host_chart);
	cnv = _PD_require_conv(dpf, dph);}
    else
       cnv = ((dpf->convert > 0) ||
	      (_PD_TEXT_OUT(file) == TRUE) ||
	      (strcmp(intype, outtype) != 0));

    if (bpi == -1)
       PD_error("CAN'T GET NUMBER OF BYTES - _PD_WR_LEAF_MEMBERS", PD_WRITE);

/* dispatch all other writes */
    else if (cnv == TRUE)
       {nb = nitems*bpi;

/* add 100 extra bytes in case we are in TEXT mode and need quotes
 * terminating nulls and maybe a newline
 */
	bf = CMAKE_N(char, nb+100);
        if (bf == NULL)
           PD_error("CAN'T ALLOCATE MEMORY - _PD_WR_LEAF_MEMBERS", PD_WRITE);

	if (SC_zero_on_alloc_n(-1) == FALSE)
	   memset(bf, 0, nb);

        out = bf;
        in  = vr;
        PD_convert(&out, &in, intype, outtype, nitems,
                   file->host_std, file->std, file->host_std,
                   file->host_chart, file->chart, 0, PD_WRITE);
        ni = lio_write(bf, (size_t) bpi, (size_t) nitems, fp);
        CFREE(bf);}
    else
       ni = lio_write(vr, (size_t) bpi, (size_t) nitems, fp);

    if (ni != nitems)
       PD_error("BYTE WRITE FAILED - _PD_WR_LEAF_MEMBERS", PD_WRITE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WRITE_HYPER_VIF - write a part of a hyper-surface to memory
 *                     - ADDR is the starting address
 *                     - STOP is the upper bound on the address
 *                     - STEP is the increment of the address for each entry
 *                     - FBYT is the number of bytes in the file for each
 *                     - item to be written
 *                     - HBYT is the number of bytes in memory for each
 *                     - item to be written
 */

static char *_PD_write_hyper_vif(PDBfile *file, char *in, char *intype, 
				 syment *ep, int hbyt, int fbyt,
				 int64_t addr, int64_t stop, long step)
   {long ni;
    char *outtype;

    outtype = PD_entry_type(ep);

/* items logically contiguous */
    if (step == fbyt)
       {ni = (stop - addr)/step + 1L;

	_PD_set_current_address(file, addr, SEEK_SET, PD_WRITE);

	_PD_sys_write(file, in, ni, intype, outtype);}

/* items logically discontiguous */
    else
       {for (; addr <= stop; addr += step, in += hbyt)
	    {_PD_set_current_address(file, addr, SEEK_SET, PD_WRITE);
             _PD_sys_write(file, in, 1L, intype, outtype);};};

    return(in);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WRITE_HYPER_SPACE - write a part of a hyper-surface to disk
 *                       - ADDR is the starting address
 *                       - STOP is the upper bound on the address
 *                       - STEP is the increment of the address for each entry
 *                       - FBYT is the number of bytes in the file for each
 *                       - item to be written
 *                       - HBYT is the number of bytes in memory for each
 *                       - item to be written
 */

static char *_PD_write_hyper_space(PDBfile *file, char *in, char *intype, 
				   syment *ep, int hbyt, int fbyt,
				   int64_t addr, int64_t stop, long step)
   {long n, nb, niw, ni;
    int64_t eaddr;
    char *outtype;
    SC_array *bl;

    if ((file == NULL) || (ep == NULL))
       return(NULL);

    outtype = PD_entry_type(ep);
    bl      = ep->blocks;

/* items logically contiguous */
    if (step == fbyt)
       {ni = (stop - addr)/step + 1L;

/* get writes across blocks correct */
        while (ni > 0)
           {eaddr = addr;

/* adjust the address for the correct block */
            n = _PD_effective_addr(&eaddr, &nb, fbyt, bl);

	    _PD_set_current_address(file, eaddr, SEEK_SET, PD_WRITE);

/* NOTE: this subverts _PD_effective_addr in part, but because _PD_effective_ep
 * cannot be modified to build an effective syment for indirectly referenced data
 * which cannot be distinguished from an explicitly dimensioned array, this
 * is the best that can be done
 */
            if ((eaddr == 0) || (nb == 0))
               {eaddr = addr;
                nb    = ni;};

            niw = min(nb, ni);

            _PD_sys_write(file, in, niw, intype, outtype);
	    _PD_block_set_valid(ep->blocks, n, PD_BLOCK_INVALID);

            ni   -= niw;
            addr += fbyt*niw;
            in   += hbyt*niw;};}

/* items logically discontiguous */
    else
       {for (; addr <= stop; addr += step, in += hbyt)
            {eaddr = addr;
             n = _PD_effective_addr(&eaddr, &nb, fbyt, bl);
	     _PD_set_current_address(file, eaddr, SEEK_SET, PD_WRITE);

             _PD_sys_write(file, in, 1L, intype, outtype);
	     _PD_block_set_valid(ep->blocks, n, PD_BLOCK_INVALID);};};

    return(in);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WR_HYPER_INDEX - do the real work of parsing an index expression into
 *                    - components and recursively determine the disk
 *                    - locations to read into the buffer OUT
 *                    - OUT is to be filled in order from smallest index to
 *                    - largest
 *                    - the offset is specified by the starting address
 *                    - which is ADDR
 *                    - FBYT is the number of bytes in the file for each item
 *                    - to be read
 *                    - HBYT is the number of bytes in memory for each item
 *                    - to be read
 */

static char *_PD_wr_hyper_index(PDBfile *file, char *out, dimind *pi, 
                                char *intype, int64_t addr,
				syment *ep, int hbyt, int fbyt)
   {long stride, step;
    int64_t offset, start, stop;

/* for each index specification compute the range and recurse */
    stride = fbyt*pi->stride;
    start  = stride*pi->start;
    stop   = stride*pi->stop;
    step   = stride*pi->step;

    stop  = addr + stop - start;
    start = addr;

/* at the bottom of the recursion do the actual operations */
    if (stride <= (long) fbyt)
       {if (file->virtual_internal == TRUE)
	   out = _PD_write_hyper_vif(file, out, intype, ep,
				     hbyt, fbyt, start, stop, step);
	else
	   out = _PD_write_hyper_space(file, out, intype, ep,
				       hbyt, fbyt, start, stop, step);}
    else
       {for (offset = start; offset <= stop; offset += step)
	    out = _PD_wr_hyper_index(file, out, pi + 1, intype, offset,
				     ep, hbyt, fbyt);};

    return(out);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INIT_STACKS - setup dynamic stacks for continuation passing
 *                 - data tree walkers
 */

static void INLINE _PD_init_stacks(long t, long d)
   {int64_t a, f;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    if (pa->rw_list_t == 0)
       {pa->rw_list_t = t;
	pa->rw_list_d = d;

	SC_mem_stats(&a, &f, NULL, NULL);

	DYN_STK(long, pa->rw_call_stack,
		pa->rw_call_ptr, pa->rw_call_ptr_x);

	DYN_STK(char *, pa->rw_str_stack,
		pa->rw_str_ptr, pa->rw_str_ptr_x);

	DYN_STK(SC_address, pa->rw_lval_stack,
		pa->rw_lval_ptr, pa->rw_lval_ptr_x);

	SC_mem_stats_set(a, f);};

    pa->rw_call_stack[0L] = DONE;

    pa->rw_call_ptr = 1L;
    pa->rw_lval_ptr = 0L;
    pa->rw_str_ptr  = 0L;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_ANNOTATE_TEXT - for text files annotate the output */

int64_t _PD_annotate_text(PDBfile *file, syment *ep, char *name,
			int64_t addr, void *vr)
   {long ni, nc;
    int64_t na, pa;
    char s[MAXLINE];
    char *typ;

    na = addr;

    if (_PD_TEXT_OUT(file) == TRUE)
       {ni  = PD_entry_number(ep);
	typ = PD_entry_type(ep);
	if (ni > 1)
	   snprintf(s, MAXLINE, "\n# %s %s[%ld]\n", typ, name, ni);
	else
	   snprintf(s, MAXLINE, "\n# %s %s\n", typ, name);

	nc = strlen(s);
	io_puts(s, file->stream);

	na = _PD_get_current_address(file, PD_WRITE);
	pa = _PD_GETSPACE(file, nc, FALSE, 0);
	_PD_SET_ADDRESS(file, pa + nc);
/*	file->chrtaddr = _PD_eod(file); */

	_PD_entry_set_address(ep, 0, na);};

    return(na);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WR_SYMENT - write the NUMBER of elements of type, TYPE,
 *               - from memory pointed to by VAR
 *               - returns the number of items successfully written
 *               - this new version is written in a continuation passing
 *               - style so that PDB has control over the stack and
 *               - isn't blowing out the execution stack for long linked
 *               - lists
 */

long _PD_wr_syment(PDBfile *file, char *vr, long ni,
		   char *intype, char *outtype)
   {int dst, size, indir, count, itags;
    long i;
    char bf[MAXLINE];
    char *litype, *lotype, *svr, *ttype;
    defstr *dp;
    memdes *desc, *mem_lst;
    PD_smp_state *pa;

    if ((file == NULL) || (vr == NULL) || (outtype == NULL))
       return(0L);

    pa = _PD_get_state(-1);

    i       = 0;
    size    = 0;
    svr     = NULL;
    desc    = NULL;
    mem_lst = NULL;
    litype  = NULL;
    lotype  = NULL;

    _PD_MARK_AS_FLUSHED(file, FALSE);

    _PD_init_stacks(100L, 1000L);

    itags = file->use_itags;
    dst   = (_PD_indirection(intype) && itags) ? INDIRECT : LEAF;

    if (dst == LEAF)
       {indir  = FALSE;
	litype = intype;
	lotype = outtype;}
    else
       {indir = TRUE;
	SAVE_S(litype, intype);
	SAVE_S(lotype, outtype);};

/* some AIX compilers will erroneously take the default case if
 * this is terminated with a semicolon
 */
    START

    case LEAF :
         _PD_wr_leaf_members(file, litype, lotype, ni, vr);

         dp = PD_inquire_host_type(file, litype);
         if (dp == NULL)
            PD_error("BAD TYPE - _PD_WR_SYMENT", PD_WRITE);

         mem_lst = dp->members;
         if (dp->type[0] == '*')
	    {GO(INDIRECT);}
	 else if (!dp->n_indirects || (mem_lst == NULL))
            GO_CONT;

         if (pa->wr_hook != NULL)
            mem_lst = (*pa->wr_hook)(file, vr, dp);

/* if type is a struct with pointered members write them out now 
 * for an array of structs write the indirects for each array element
 */
         size = dp->size;
         svr  = vr;
         i    = 0L;

    case LEAF_ITEM :
         if (i >= ni)
            GO_CONT;

         desc = mem_lst;

    case LEAF_INDIR :
         if (desc == NULL)
            {i++;
             svr += size;
             GO(LEAF_ITEM);};

         PD_CAST_TYPE(ttype, desc, svr+desc->member_offs, svr,
		      PD_error, "BAD CAST - _PD_WR_SYMENT", PD_WRITE);

         SAVE_S(litype, ttype);

         if (!_PD_indirection(litype))
            {RESTORE_S(litype);
             desc = desc->next;
             GO(LEAF_INDIR);};

         SAVE_I(ni);
         ni = desc->number;

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
         RESTORE_I(ni);
         RESTORE_S(litype);

         desc = desc->next;
         GO(LEAF_INDIR);

    case INDIRECT :
    
         if (vr == NULL)
            {(*file->wr_itag)(file, NULL, 0L, litype, -1L, TRUE);
             GO_CONT;};

/* dereference a local copy of the type */
         SAVE_S(litype, litype);
         PD_dereference(litype);

/* check the data */
         i = 0L;

    case INDIR_ITEM :
         if (i >= ni)
            {RESTORE_S(litype);
             GO_CONT;};

         SAVE_P(vr);
         vr = DEREF(vr);
         if (vr == NULL)
            {(*file->wr_itag)(file, NULL, 0L, litype, -1L, FALSE);
             RESTORE_P(char, vr);
             i++;
             vr += sizeof(char *);
             GO(INDIR_ITEM);};

         SAVE_I(ni);
         ni = _PD_number_refd(vr, litype, file->host_chart);
         if (ni == -1L)
            {snprintf(bf, MAXLINE,
                      "CAN'T GET POINTER LENGTH ON %s - _PD_WR_SYMENT",
                     litype);
             PD_error(bf, PD_WRITE);};

         if (ni == -2L)
            {snprintf(bf, MAXLINE,
                      "UNKNOWN TYPE %s - _PD_WR_SYMENT",
                     litype);
             PD_error(bf, PD_WRITE);};

         if (_PD_ptr_wr_itags(file, vr, ni, litype) == FALSE)
            {RESTORE_I(ni);
             RESTORE_P(char, vr);
             i++;
             vr += sizeof(char *);
             GO(INDIR_ITEM);};

         SAVE_I(i);
         SAVE_S(lotype, litype);
         SET_CONT(INDIR_RET, FALSE);

    case INDIR_RET :
         RESTORE_S(lotype);
         RESTORE_I(i);
         RESTORE_I(ni);
         RESTORE_P(char, vr);

         i++;
         vr += sizeof(char *);

         GO(INDIR_ITEM);

    case DONE :
         if (indir)
	    {RESTORE_S(lotype);
	     RESTORE_S(litype);};

/* update the end of data mark */
         if (_PD_IS_MASTER(file))
            file->chrtaddr = _PD_eod(file);

         return(ni);

    FINISH("_PD_WR_SYMENT", PD_WRITE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_HYPER_WRITE - parse an index expression of the form
 *                 -
 *                 -    <expr>  := <spec> | <expr>, <spec>
 *                 -    <spec>  := <start> |
 *                 -               <start>:<stop> |
 *                 -               <start>:<stop>:<step>
 *                 -    <start> := starting integer index value
 *                 -    <stop>  := ending integer index value
 *                 -    <step>  := integer index step value
 *                 -
 *                 - and write the specified elements to the PDBfile
 *                 - from the array provided
 */

int _PD_hyper_write(PDBfile *file, char *name, syment *ep,
		    void *vr, char *intype)
   {int nc, nd, c, hbyt, fbyt, rv;
    char s[MAXLINE], *expr;
    dimdes *dims;
    dimind *pi;

    if ((file == NULL) || (ep == NULL))
       return(0);

    rv = TRUE;

    dims = PD_entry_dimensions(ep);
    strcpy(s, name);
    c = SC_LAST_CHAR(s);

    if (((c != ')') && (c != ']')) || (dims == NULL))
       {if (file->virtual_internal == TRUE)

/* GOTCHA: assuming that if we get here then we are rewriting an
 *         entire entry in a VIF (as opposed to something like an
 *         array element) and that it doesn't really need to be done
 *         We aren't fully set to handle this yet
 */
	   rv = TRUE;

	else
	   {rv  = _PD_sys_write(file, vr, PD_entry_number(ep),
				intype, PD_entry_type(ep));
	    rv *= _PD_csum_block_write(file, ep, 0);};}

    else
       {if (_PD_indirection(PD_entry_type(ep)))
	   PD_error("CAN'T HYPER INDEX INDIRECT TYPE - _PD_HYPER_WRITE",
		    PD_WRITE);

	expr = SC_lasttok(s, "[]()");
	nc   = strlen(expr) + 1;
	nc   = min(nc, MAXLINE);
	memmove(s, expr, nc);

	pi = _PD_compute_hyper_strides(file, s, dims, &nd);
	if (pi == NULL)
	   PD_error("CAN'T FIND HYPER INDICES - _PD_HYPER_WRITE", PD_WRITE);

	fbyt = _PD_lookup_size(PD_entry_type(ep), file->chart);
	if (fbyt == -1)
	   PD_error("CAN'T FIND NUMBER OF FILE BYTES - _PD_HYPER_WRITE",
		    PD_WRITE);

	hbyt = _PD_lookup_size(intype, file->host_chart);
	if (hbyt == -1)
	   PD_error("CAN'T FIND NUMBER OF HOST BYTES - _PD_HYPER_WRITE",
		    PD_WRITE);

	_PD_wr_hyper_index(file, vr, pi, intype, PD_entry_address(ep),
			   ep, hbyt, fbyt);

	CFREE(pi);};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                            READ ROUTINES                                 */

/*--------------------------------------------------------------------------*/

/* _PD_READ_HYPER_SPACE - read a part of a hyper-surface from a data array
 *                      - ADDR is the starting address
 *                      - STOP is the upper bound on the address
 *                      - STEP is the increment of the address for each entry
 *                      - FBYT is the number of bytes in the file for each
 *                      - item to be read
 *                      - HBYT is the number of bytes in memory for each
 *                      - item to be read
 *                      - EP is a scratch syment for temporary use
 *                      - EPO is the originating syment
 *                      - return the number of item successfully read
 */

static int _PD_read_hyper_space(PDBfile *file, char *name, syment *ep,
				char *out, syment *epo, char *outtype,
				int hbyt, int fbyt, 
                                int64_t addr, int64_t stop, long step)
   {int nrd, nr;
    long n, nb, ni;
    int64_t eaddr; 
    char *intype;
    SC_array *bl, *blo;

    if ((file == NULL) || (ep == NULL) || (epo == NULL))
       return(0);

    intype = PD_entry_type(epo);
    blo    = epo->blocks;

    bl = ep->blocks;
    ep->type       = intype;
    ep->dimensions = NULL;

    nrd = 0;
    if (addr >= 0)

/* items logically contiguous */
       {if (step == fbyt)
	   {long niw;

/* read across blocks */
	    ni = (stop - addr)/step + 1L;
	    while (ni > 0)
	       {eaddr = addr;
		n = _PD_effective_addr(&eaddr, &nb, fbyt, blo);

/* NOTE: this subverts _PD_effective_addr in part, but because _PD_effective_ep
 * cannot be modified to build an effective syment for indirectly referenced data
 * which cannot be distinguished from an explicitly dimensioned array, this
 * is the best that can be done
 */
		if ((eaddr == 0) || (nb == 0))
		   {eaddr = addr;
		    nb    = ni;};

		niw = min(ni, nb);

		_PD_block_set_desc(eaddr, niw, bl, 0);
		ep->number = niw;

		if (_PD_csum_block_read(file, name, epo, n) == FALSE)
		   nr = 0;
		else
		   nr = _PD_sys_read(file, ep, outtype, out);
		nrd += nr;

		ni   -= niw;
		addr += fbyt*niw;
		out  += hbyt*niw;};}

/* items not logically contiguous */
        else
	   {ep->number = 1L;
	    for (; addr <= stop; addr += step, out += hbyt)
	        {eaddr = addr;
		 n = _PD_effective_addr(&eaddr, &nb, fbyt, blo);
		 _PD_block_set_address(bl, 0, eaddr);
		 if (_PD_csum_block_read(file, name, epo, n) == FALSE)
		    nr = 0;
		 else
		    nr = _PD_sys_read(file, ep, outtype, out);
		 nrd += nr;};};}

/* we have a bitstream */
    else
       {defstr* dpf;

	dpf = _PD_lookup_type(intype, file->chart);
	if (dpf == NULL)
	   PD_error("CANNOT FIND TYPE IN THE FILE CHART - _PD_READ_HYPER_SPACE", PD_READ);

/* items logically contiguous */
	else if (step == -dpf->size_bits)
	   {ni = (stop - addr)/step + 1L;

/* NOTE: multi-block bitstreams are not supported */
	    ep->number = ni;
	    PD_entry_set_address(ep, addr);
	    nrd += _PD_sys_read(file, ep, outtype, out);
	    out += hbyt*ni;}

/* items not logically contiguous */
	else
	   {ep->number = 1L;
	    for (; addr >= stop; addr += step, out += hbyt)
	        {PD_entry_set_address(ep, addr);
		 nrd += _PD_sys_read(file, ep, outtype, out);};};};

    ep->type = NULL;

    return(nrd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_HYPER_INDEX - do the real work of parsing an index expression into
 *                    - compontents and recursively determine the disk
 *                    - locations to read into the buffer OUT
 *                    - OUT is to be filled in order from smallest index to
 *                    - largest
 *                    - the offset is specified by the starting address
 *                    - which is ADDR
 *                    - FBYT is the number of bytes in the file for each item
 *                    - to be read
 *                    - HBYT is the number of bytes in memory for each item
 *                    - to be read
 *                    - return the number of item successfully read
 */

static int _PD_rd_hyper_index(PDBfile *file, char *name,
			      syment *ep, char *out,
			      dimind *pi, syment *epo, char *outtype,
			      int64_t addr, int hbyt, int fbyt)
   {int nrd, nir;
    long stride, step;
    int64_t offset, start, stop;
    char *intype;

    intype = PD_entry_type(epo);

/* for each index specification compute the range and recurse */
    stride = fbyt*pi->stride;
    start  = stride*pi->start;
    stop   = stride*pi->stop;
    step   = stride*pi->step;

    if (addr < 0)
       {defstr* dpf;

        dpf = _PD_lookup_type(intype, file->chart);
        if (dpf == NULL)
           PD_error("CANNOT FIND TYPE IN THE FILE CHART - _PD_RD_HYPER_INDEX", PD_READ);
	else
	   {stop = addr - dpf->size_bits*((stop - start)/fbyt);
	    step = -dpf->size_bits*(step/fbyt);};}

    else
       stop = addr + (stop - start);

/* at the bottom of the recursion do the actual reads */
    nrd = 0;
    if (stride <= (long) fbyt)
       nrd += _PD_read_hyper_space(file, name, ep, out, epo, outtype, 
				   hbyt, fbyt, addr, stop, step);
    else if (addr < 0)
       {for (offset = -addr; offset <= -stop; offset -= step)
            {nir = _PD_rd_hyper_index(file, name, ep, out, pi + 1, epo,
				      outtype, -offset, hbyt, fbyt);
	     nrd += nir;
             out += nir*hbyt; };}
    else
       {for (offset = addr; offset <= stop; offset += step)
	    {nir = _PD_rd_hyper_index(file, name, ep, out, pi + 1, epo,
				      outtype, offset, hbyt, fbyt);
	     nrd += nir;
             out += nir*hbyt; };};

    return(nrd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_LEAF_MEMBERS - read the leaves only for NI of INTYPE
 *                     - from the PDBfile FILE
 *                     - into the location pointed to by VR as type OUTTYPE
 *                     - at this level it guaranteed that the type
 *                     - will not be a pointer
 *                     - return the number of items successfully read
 */

static void _PD_rd_leaf_members(PDBfile *file, char *vr, long ni, 
                                char *intype, char *outtype, int boffs)
   {int ipt, nbt, cnv;
    long bpi, nir, nia, nb;
    char *bf, *in, *out;
    defstr *dpf, *dph;
    FILE *fp;

    bpi = -1;
    fp  = file->stream;

    dpf = _PD_lookup_type(intype, file->chart);
    ipt = _PD_items_per_tuple(dpf);

    if (dpf == NULL)
       PD_error("CANNOT FIND TYPE IN THE FILE CHART - _PD_RD_LEAF_MEMBERS", PD_READ);
    else
       bpi = dpf->size;

    if (dpf->convert == -1)
       {dph = _PD_lookup_type(outtype, file->host_chart);
	cnv = _PD_require_conv(dpf, dph);}
    else
       cnv = ((dpf->convert > 0) || (strcmp(intype, outtype) != 0));

    ni *= ipt;

    if (bpi == -1)
       PD_error("CAN'T FIND NUMBER OF BYTES - _PD_RD_LEAF_MEMBERS", PD_READ);

    else if (cnv == TRUE)
       {nbt = dpf->size_bits;
	if (nbt != 0)
           nia = (((ni*nbt + boffs + SC_BITS_BYTE - 1) / 
		   SC_BITS_BYTE) + bpi - 1)/bpi;
	else
	   nia = ni;

	nb = nia*bpi;

/* add 4 extra bytes in case we are in TEXT mode and need quotes
 * terminating nulls and maybe a newline
 */
	bf = CMAKE_N(char, nb+4);
	if (bf == NULL)
	   PD_error("CAN'T ALLOCATE MEMORY - _PD_RD_LEAF_MEMBERS", PD_READ);

	if (SC_zero_on_alloc_n(-1) == FALSE)
	   memset(bf, 0, nb);

        nir = lio_read(bf, (size_t) bpi, (size_t) nia, fp);
        if (nir == nia)
           {in  = bf;
            out = vr;
            PD_convert(&out, &in, intype, outtype, ni,
                       file->std, file->host_std, file->host_std,
                       file->chart, file->host_chart,
		       boffs, PD_READ);
            CFREE(bf);}
        else
           {CFREE(bf);
            PD_error("FILE READ FAILED - _PD_RD_LEAF_MEMBERS", PD_READ);};}

    else
       {nir = lio_read(vr, (size_t) bpi, (size_t) ni, fp);
        if (nir != ni)
           PD_error("DATA READ FAILED - _PD_RD_LEAF_MEMBERS", PD_READ);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INDEXED_READ_AS - read part of an entry from the PDB file
 *                     - into the location pointed to by VR
 *                     - IND contains one triplet of long ints per variable
 *                     - dimension specifying start, stop, and step for the
 *                     - index
 *                     - return the number of item successfully read
 */

int _PD_indexed_read_as(PDBfile *file, char *fullpath, char *type, void *vr, 
                        int nd, long *ind, syment *ep)
   {int i, err;
    long start, stop, step;
    char index[MAXLINE], hname[MAXLINE];
    PD_smp_state *pa;

    err = 0;

    pa = _PD_get_state(-1);

    switch (SETJMP(pa->read_err))
       {case ABORT :
	     return(FALSE);
        case ERR_FREE :
	     return(TRUE);
        default :
	     memset(pa->err, 0, MAXLINE);
	     break;};

    strcpy(index, "(");
    for (i = 0; i < nd; i++)
        {start = ind[0];
         stop  = ind[1];
         step  = ind[2];
         ind  += 3;
         if (start == stop)
            SC_vstrcat(index, MAXLINE, "%ld,", start);

         else if (step <= 1L)
            SC_vstrcat(index, MAXLINE, "%ld:%ld,", start, stop);
            
         else
            SC_vstrcat(index, MAXLINE, "%ld:%ld:%ld,", start, stop, step);};

    if (strlen(index) > 1)
       {SC_LAST_CHAR(index) = ')';
        snprintf(hname, MAXLINE, "%s%s", fullpath, index);}

    else
       strcpy(hname, fullpath);

    _PD_rl_syment_d(ep);
    ep = _PD_effective_ep(file, hname, TRUE, fullpath);
    if (ep == NULL)
       PD_error("CAN'T FIND ENTRY - _PD_INDEXED_READ_AS", PD_READ);

    else
       {ep->number = PD_hyper_number(file, hname, ep);
	if (type == NULL)
	   type = PD_entry_type(ep);

	err = _PD_hyper_read(file, hname, type, ep, vr);
	_PD_rl_syment_d(ep);};

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_HYPER_READ - parse an index expression of the form
 *                -
 *                -    <expr>  := <spec> | <expr>, <spec>
 *                -    <spec>  := <start> |
 *                -               <start>:<stop> |
 *                -               <start>:<stop>:<step>
 *                -    <start> := starting integer index value
 *                -    <stop>  := ending integer index value
 *                -    <step>  := integer index step value
 *                -
 *                - and read the specified elements from the PDBfile
 *                - into the array provided
 *                - return the number of items successfully read
 */

int _PD_hyper_read(PDBfile *file, char *name, char *outtype,
		   syment *ep, void *vr)
   {int nc, nd, c, nrd, hbyt, fbyt;
    char s[MAXLINE], *expr;
    dimdes *dims;
    dimind *pi;
    syment *dep, *tep;

    if ((file == NULL) || (name == NULL) || (ep == NULL))
       return(0);

/* to accomodate certain bad users do one quick test
 * see if the variable name is literally in the file
 * this lets things such as foo(2,10) be variable names
 */
    dep = PD_inquire_entry(file, name, FALSE, NULL);
    if (dep != NULL)
       {if (_PD_csum_block_read(file, name, ep, -1) == FALSE)
	   nrd = 0;
	else
	   nrd = _PD_sys_read(file, ep, outtype, vr);}

    else
       {dims = PD_entry_dimensions(ep);
	strcpy(s, name);
	c = SC_LAST_CHAR(s);
	if (((c != ')') && (c != ']')) || (dims == NULL))
	   {if (_PD_csum_block_read(file, name, ep, 0) == FALSE)
	       nrd = 0;
	    else
	       nrd = _PD_sys_read(file, ep, outtype, vr);}

	else
	   {if (_PD_indirection(outtype))
	       PD_error("CAN'T HYPER INDEX INDIRECT TYPE - _PD_HYPER_READ",
			PD_READ);

	    expr = SC_lasttok(s, "[]()");
	    nc   = strlen(expr) + 1;
	    nc   = min(nc, MAXLINE);
	    memmove(s, expr, nc);

	    pi = _PD_compute_hyper_strides(file, s, dims, &nd);
	    if (pi == NULL)
	       PD_error("CAN'T FIND HYPER INDICES - _PD_HYPER_READ", PD_READ);

	    fbyt = _PD_lookup_size(PD_entry_type(ep), file->chart);
	    if (fbyt == -1)
	       PD_error("CAN'T FIND NUMBER OF FILE BYTES - _PD_HYPER_READ",
			PD_READ);

	    hbyt = _PD_lookup_size(outtype, file->host_chart);
	    if (hbyt == -1)
	       PD_error("CAN'T FIND NUMBER OF HOST BYTES - _PD_HYPER_READ",
			PD_READ);

/* make a dummy for the hyper read to use as scratch space */
	    tep = _PD_mk_syment(NULL, 0L, 0L, &(ep->indirects), NULL);

	    nrd = _PD_rd_hyper_index(file, name, tep, vr, pi, ep,
				     outtype, PD_entry_address(ep),
				     hbyt, fbyt);

	    _PD_rl_syment(tep);

	    CFREE(pi);};};

    return(nrd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_READ_BITS - read a chunk of data which is a bitstream and unpack it
 *              - arguments are:
 *              -   FILE    the PDBfile to use
 *              -   NAME    the name of the variable in the file
 *              -   TYPE    the target type of the data when unpacked
 *              -   NI      the number of items requested
 *              -   SGNED   TRUE if the data type is signed
 *              -   NBITS   the number of bits per item
 *              -   PADSZ   the number of bits of pad preceding the fields
 *              -   FPP     the number of fields per pad
 *              -   OFFS    offset from the beginning of the input data
 *              -   PAN     the number of items found 
 *              -   PDATA   the data array returned
 *
 * #bind PD_read_bits fortran() scheme() python()
 */

int PD_read_bits(PDBfile *file ARG(,,cls),
		 char *name, char *type, long ni,
		 int sgned, int nbits, int padsz, int fpp,
		 long offs, long *pan, char **pdata)
   {int ret;

    ret = _PD_rd_bits(file, name, type, ni, sgned, nbits,
		      padsz, fpp, offs, pan, pdata);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_BITS - read a chunk of data which is a bitstream and unpack it
 *             - arguments are:
 *             -   FILE    the PDBfile to use
 *             -   NAME    the name of the variable in the file
 *             -   TYPE    the target type of the data when unpacked
 *             -   NITEMS  the number of items requested
 *             -   SGNED   TRUE if the data type is signed
 *             -   NBITS   the number of bits per item
 *             -   PADSZ   the number of bits of pad preceding the fields
 *             -   FPP     the number of fields per pad
 *             -   OFFS    offset from the beginning of the input data
 *             -   PAN     the number of items found
 *             -   PDATA   the data array returned
 */

int _PD_rd_bits(PDBfile *file, char *name, char *type, long nitems,
		int sgned, int nbits, int padsz, int fpp,
		long offs, long *pan, char **pdata)
   {int i, ityp, out_flag, onescmp;
    int *ord;
    long nitemsin, enumb, ebyte, obyte, ni;
    char *etype, *in, *out;
    syment *ep;
    defstr *dp1, *dp2;

    ep = _PD_effective_ep(file, name, TRUE, NULL);
    if (ep == NULL)
       return(FALSE);

    nitemsin = PD_entry_number(ep);
    etype  = PD_entry_type(ep);

    dp1 = PD_inquire_type(file, etype);
    if (dp1 == NULL)
       return(FALSE);

    onescmp = dp1->onescmp;

    dp2 = PD_inquire_host_type(file, type);
    if (dp2 == NULL)
       return(FALSE);

    out_flag = dp2->fix.order;

    ebyte = _PD_lookup_size(etype, file->chart);
    enumb = nitemsin * ebyte;

    in = CMAKE_N(char, enumb);

    CFREE(etype);

    ep->type   = CSTRSAVE(SC_CHAR_S);
    ep->number = enumb;
    ni = _PD_sys_read(file, ep, SC_CHAR_S, in);
    if (ni != enumb)
       {CFREE(in);
	_PD_rl_syment_d(ep);
	return(FALSE);};

    obyte = _PD_lookup_size(type, file->host_chart);
    out   = _PD_alloc_entry(file, type, nitems);
    ityp  = SC_type_id(type, FALSE);

    if ((SC_is_type_char(ityp) == FALSE) && (SC_is_type_fix(ityp) == FALSE))
       return(FALSE);

    SC_unpack_bits(out, in, ityp, nbits, padsz, fpp, nitems, offs);

/* convert integers */
    if (SC_is_type_char(ityp) == FALSE)
       {ord = CMAKE_N(int, obyte);

        if (out_flag == NORMAL_ORDER)
           for (i = 0; i < obyte; ord[i] = i + 1, i++);
        else                         
           for (i = 0; i < obyte; ord[i] = obyte - i, i++);

        if (sgned)
           _PD_sign_extend(out, nitems, obyte, nbits, ord);

        if (onescmp)
           _PD_ones_complement(out, nitems, obyte, ord);

        CFREE(ord);};

    *pan   = nitems;
    *pdata = out;

    CFREE(in);
    _PD_rl_syment_d(ep);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_SYMENT - from the information in the symbol table entry EP
 *               - read the entry from the PDBfile FILE into the location
 *               - pointed to by VR
 *               - at this point the things have been completely dereferenced
 *               - return the number of items successfully read
 *               - this new version is written in a continuation passing
 *               - style so that PDB has control over the stack and
 *               - isn't blowing out the execution stack for long linked
 *               - lists
 */

long _PD_rd_syment(PDBfile *file, syment *ep, char *outtype, void *vr)
   {int dst, vif, size, boffs, count, itags;
    long i, n, ni, bpi, nrd;
    long loc;
    int64_t addr, eaddr;
    char bf[MAXLINE];
    char *pv, *litype, *lotype, *svr, **lvr;
    symindir iloc;
    defstr *dp;
    memdes *desc, *mem_lst;
    SC_array *bl;
    PD_itag pi;
    PD_smp_state *pa;

    if ((file == NULL) || (ep == NULL))
       return(0);

    pa = _PD_get_state(-1);

    lvr     = NULL;
    svr     = NULL;
    desc    = NULL;
    mem_lst = NULL;
    litype  = NULL;
    lotype  = NULL;
    size    = 0;
    ni  = 0;

    vif   = file->virtual_internal;
    itags = file->use_itags;

    iloc = ep->indirects;

    _PD_init_stacks(100L, 1000L);

    SAVE_S(lotype, outtype);

    dst = BLOCK;

/* some AIX compilers will erroneously take the default case if
 * this is terminated with a semicolon
 */
    START

/* NOTE: count on this being right and _PD_effective_ep will handle all
 * issues about partial reads across discontiguous blocks by correctly
 * making an effective syment for which this logic works!!!!!!
 */
    case BLOCK :
         bpi = _PD_lookup_size(outtype, file->host_chart);
         if (bpi == -1)
            PD_error("CAN'T FIND NUMBER OF BYTES - _PD_RD_SYMENT", PD_READ);

         bl = ep->blocks;
         n  = _PD_n_blocks(ep);
         if (n == 1)
            _PD_block_set_number(bl, 0, PD_entry_number(ep));

         pv  = (char *) vr;
         nrd = 0L;
         i   = 0L;

    case BLOCK_ITEM :
         if (i >= n)
            {GO_CONT;};

         _PD_block_get_desc(&addr, &ni, bl, i);

/* all directories have an address of -1 which unsigned is INT_MAX */
         addr = (addr == INT_MAX) ? 0 : addr;

/* if negative we are staring at a bit address */
         if (addr < 0)
            {eaddr = (-addr) >> 3;
	     boffs = -addr - (eaddr << 3);}
         else
            {eaddr = addr;
	     boffs = 0;};

         if (!vif)
	    _PD_set_current_address(file, eaddr, SEEK_SET, PD_READ);

         SAVE_I(i);
         SAVE_I(n);
         SAVE_S(litype, PD_entry_type(ep));
         SAVE_P(pv);
         SET_CONT(BLOCK_RET, FALSE);

    case BLOCK_RET :
         RESTORE_P(char, pv);
         RESTORE_S(litype);
         RESTORE_I(n);
         RESTORE_I(i);

         pv += ni*bpi;
         i++;

         GO(BLOCK_ITEM);

    case LEAF:
         if (vif)
            {SC_address ad;

             ad.diskaddr = addr;
             memcpy(pv, ad.memaddr, ni*bpi);
/*
	     nrd += ni;
	     GO_CONT;
*/
	     }

         else
            _PD_rd_leaf_members(file, pv, ni, litype, lotype, boffs);

         nrd += ni;

/* the host type must be used to get the correct member offsets for the
 * in memory copy - the file ones might be wrong!!
 */
         dp = PD_inquire_host_type(file, lotype);
         if (dp == NULL)
            PD_error("BAD TYPE - _PD_RD_SYMENT", PD_READ);

         mem_lst = dp->members;
         if (dp->type[0] == '*')
	    {GO(INDIRECT);}
	 else if ((dp->n_indirects == 0) || (mem_lst == NULL))
            GO_CONT;

         if (pa->rd_hook != NULL)
            mem_lst = (*pa->rd_hook)(dp->members);

/* for an array of structs read the indirects for each array element */
         size = dp->size;
         svr  = pv;
         i    = 0L;

    case LEAF_ITEM :
         if (i >= ni)
            GO_CONT;

         desc = mem_lst;

    case LEAF_INDIR :
         if (desc == NULL)
            {i++;
             svr += size;
             GO(LEAF_ITEM);};

         if (desc->is_indirect == FALSE)
            {desc = desc->next;
             GO(LEAF_INDIR);};

         SAVE_I(i);
         SAVE_I(size);
         SAVE_P(mem_lst);
         SAVE_P(desc);
         SAVE_P(svr);
         SAVE_P(pv);
         pv  = svr + desc->member_offs;
         lvr = (char **) pv;
         SET_CONT_RD(LEAF_RET, SKIP_TO);

    case LEAF_RET :
         RESTORE_P(char, pv);
         RESTORE_P(char, svr);
         RESTORE_P(memdes, desc);
         RESTORE_P(memdes, mem_lst);
         RESTORE_I(size);
         RESTORE_I(i);

         desc = desc->next;
         GO(LEAF_INDIR);

    case INDIRECT :
         SAVE_P(pv);
         lvr = (char **) pv;
         i   = 0L;

    case INDIR_ITEM :
         if (i >= ni)
            {RESTORE_P(char, pv);
             nrd += ni;
             GO_CONT;};

         SAVE_I(i);
         SAVE_I(nrd);
         SAVE_P(lvr);
         lvr = &lvr[i];
         SET_CONT_RD(INDIR_RET, SKIP_TO);

    case INDIR_RET :
         RESTORE_P(char *, lvr);
         RESTORE_I(nrd);
         RESTORE_I(i);
         i++;

         GO(INDIR_ITEM);

    case SKIP_TO :
         if ((iloc.addr > 0) && (itags == TRUE))
	    {long naitems;

	     _PD_set_current_address(file, iloc.addr, SEEK_SET, PD_READ);
	     iloc.addr = -1;

	     naitems = iloc.n_ind_type*iloc.arr_offs;
	     _PD_skip_over(file, naitems, FALSE);};

         if (vif)
            {SC_address ad;

             ad.diskaddr = addr;
             DEREF(pv)   = DEREF(ad.memaddr);
             GO_CONT;}

         else if (_PD_ptr_rd_itags(file, lvr, &pi) == FALSE)
            {GO_CONT;};

/* now read the data */
         SAVE_I(nrd);
         SAVE_I(ni);

         ni     = pi.nitems;
         addr   = pi.addr;
         loc    = pi.flag;

         SAVE_S(litype, pi.type);
         SAVE_S(lotype, litype);
         SAVE_I(addr);
         SAVE_I(loc);
         SAVE_P(pv);
         pv = lvr[0];
         SET_CONT(SKIP_RET, FALSE);

    case SKIP_RET :
         RESTORE_P(char, pv);
         RESTORE_I(loc);
         RESTORE_I(addr);
         RESTORE_S(lotype);
         RESTORE_S(litype);
         RESTORE_I(ni);
         RESTORE_I(nrd);

         _PD_ptr_rd_install_addr(file, addr, loc);
         GO_CONT;

    case DONE :
         RESTORE_S(lotype);

         return(nrd);

    FINISH("_PD_RD_SYMENT", PD_READ);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_POINTER - read and return a pointer index from FILE at ADDR */

long _PD_rd_pointer(PDBfile *file, int64_t addr)
   {long n;
    size_t bpi, nr;
    char bf[MAXLINE];
    FILE *fp;

    fp  = file->stream;
    bpi = file->std->ptr_bytes;

/* set the file location */
    _PD_set_current_address(file, addr, SEEK_SET, PD_READ);

/* read the pointer value from the file location */
    nr = lio_read(bf, bpi, 1, fp);
    if (nr == 1)
       {int fbpi, hbpi;
	data_standard *hs, *fs;
	PD_byte_order ford, hord;

	hs = file->host_std;
	fs = file->std;

	fbpi = fs->ptr_bytes;
	ford = fs->fx[PD_LONG_I].order;
	hbpi = hs->fx[PD_LONG_I].bpi;
	hord = hs->fx[PD_LONG_I].order;
       
	n = _PD_convert_ptr_rd(bf, fbpi, ford, hbpi, hord, hs);}
    else
       n = -1;

/* restore the file location */
    _PD_set_current_address(file, -bpi, SEEK_CUR, PD_READ);

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

