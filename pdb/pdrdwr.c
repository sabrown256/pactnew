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
    {pa->rw_str_stack[pa->rw_str_ptr++] = s;                                 \
     s = CSTRSAVE(t);}

#define RESTORE_S(s)                                                         \
    {CFREE(s);                                                               \
     s = pa->rw_str_stack[--pa->rw_str_ptr];}

#define SAVE_I(val)                                                          \
    (pa->rw_lval_stack[pa->rw_lval_ptr++].diskaddr = (int64_t) val)

#define RESTORE_I(val)                                                       \
    (val = pa->rw_lval_stack[--pa->rw_lval_ptr].diskaddr)

#define SAVE_P(val)                                                          \
    (pa->rw_lval_stack[pa->rw_lval_ptr++].memaddr = (char *) val)

#define RESTORE_P(type, val)                                                 \
    (val = (type *) pa->rw_lval_stack[--pa->rw_lval_ptr].memaddr)

#define SET_CONT(ret, frce)                                                  \
   {pa->rw_call_stack[pa->rw_call_ptr++] = ret;                              \
    dst = (_PD_indirection(litype) && (frce || itags)) ? INDIRECT : LEAF;    \
    continue;}

#define SET_CONT_RD(ret, branch)                                             \
   {pa->rw_call_stack[pa->rw_call_ptr++] = ret;                              \
    dst = branch;                                                            \
    continue;}

#define GO_CONT                                                              \
   {dst = pa->rw_call_stack[--pa->rw_call_ptr];                              \
    continue;}

#define GO(lbl)                                                              \
    {dst = lbl;                                                              \
     continue;}

#define DYN_STK(_t, _s, _p, _px)                                             \
    {if (_p >= _px)                                                          \
        {_px += pa->rw_list_d;                                               \
	 if (_s == NULL)                                                     \
	    {_s = CMAKE_N(_t, _px);}                                         \
	 else                                                                \
	    CREMAKE(_s, _t, _px);};}

#define DYN_MAKE_STACKS                                                      \
    DYN_STK(long, pa->rw_call_stack,                                         \
	    pa->rw_call_ptr, pa->rw_call_ptr_x);                             \
    DYN_STK(char *, pa->rw_str_stack,                                        \
	    pa->rw_str_ptr, pa->rw_str_ptr_x);                               \
    DYN_STK(SC_address, pa->rw_lval_stack,                                   \
	    pa->rw_lval_ptr, pa->rw_lval_ptr_x)

#define DYN_FREE_STACKS                                                      \
    CFREE(pa->rw_call_stack);                                                \
    CFREE(pa->rw_str_stack);                                                 \
    CFREE(pa->rw_lval_stack)

#define START                                                                \
    count = 0;                                                               \
    while (TRUE)                                                             \
       {DYN_MAKE_STACKS;                                                     \
	count++;                                                             \
	switch (dst) {

#define FINISH(f, tag)                                                       \
    default  :                                                               \
         snprintf(bf, MAXLINE, "UNDECIDABLE CASE - %s", f);                  \
         PD_error(bf, tag);};}

/*--------------------------------------------------------------------------*/

/*                          AUXILLIARY ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* _PD_NUMBER_REFD - THREADSAFE
 *                 - compute the number of items pointed to by VR
 *                 - with addition of PD_size_from may have to
 *                 - refer to struct containing VR with is SVR
 *                 - described by MEML
 *                 - return the number of items if successful
 *                 - return -1 if SCORE did NOT allocate the block
 *                 - return -2 if the type is unknown
 */

long _PD_number_refd(memdes *meml, void *svr, memdes *desc, const void *vr,
		     const char *type, hasharr *tab)
   {intb bpi;
    inti ni;

    ni = 0;

    if (vr != NULL)
       {if ((desc != NULL) && (desc->size_memb != NULL))
           ni = _PD_cast_size(meml, svr, desc);

	if (ni <= 0)
	   {ni  = SC_arrlen(vr);
	    bpi = _PD_lookup_size(type, tab);
	    if ((ni <= 0) && (bpi != -1))
	       ni = bpi;
	    else if (bpi == -1)
	       ni = -2;
	    else
	       ni /= bpi;};};

    return(ni);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INDIRECTION - THREADSAFE
 *                 - return TRUE if the TYPE is an indirect type
 *                 - i.e. has a "*" as the last non-blank character
 */

int _PD_indirection(const char *s)
   {int rv;
    const char *t;

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
/*--------------------------------------------------------------------------*/

/* _PD_INDEX_STR - conver the triple IND to a string and return it in S */

static void _PD_index_str(char *s, int nc, inti *ind)
   {int i;
    char t[3][MAXLINE];

    for (i = 0; i < 3; i++)
        SC_itos(t[i], MAXLINE, ind[i], NULL);

    if (ind[0] == ind[1])
       SC_vstrcat(s, nc, "%s,", t[0]);

    else if (ind[2] <= 1L)
       SC_vstrcat(s, nc, "%s:%s,", t[0], t[1]);
            
    else
       SC_vstrcat(s, nc, "%s:%s:%s,", t[0], t[1], t[2]);

    return;}

/*--------------------------------------------------------------------------*/

/*                           HYPER INDEX SUPPORT                            */

/*--------------------------------------------------------------------------*/

/* _PD_INIT_DIMIND - THREADSAFE
 *                 - fill a dimind struct given the stride and an
 *                 - ASCII index expression
 */

static void _PD_init_dimind(dimind *pi, long offset, long stride, char *expr)
   {char s[MAXLINE], *token, *t;
    inti start, stop, step;

    if (expr != NULL)
       SC_strncpy(s, MAXLINE, expr, -1);
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
    inti maxs, old_start, old_stop;
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
    while (((nd-1) > 0) &&
	   (pi[nd-1].start == (curr_dim->index_min - off)) &&
	   (pi[nd-1].stop  == (curr_dim->index_max - off)) &&
	   (pi[nd-1].step  == 1))

/* step back one dimension */
       {nd--;

/* find the proper next dimension descriptor */
        if (file->major_order == COLUMN_MAJOR_ORDER) 
           curr_dim = curr_dim->next;

        else if (file->major_order == ROW_MAJOR_ORDER)
           {curr_dim = dims;

            for (i = 0 ; i < (nd-1) ; i++)
                curr_dim = curr_dim->next;};};

/* if stepsize is not 1 at this dimension, no contiguity is 
 * possible here and we should then optimize just the next dimension in
 * if the next dimension in exists 
 */
    if ((pi[nd-1].step != 1) && ((nd+1) <= max_nd))
       nd++;
 
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
    if (((nd-1) >= 0) && (nd < max_nd))
       {old_start = pi[nd-1].start;
        old_stop  = pi[nd-1].stop;

/* scale up start */
        pi[nd-1].start = (old_start * pi[nd-1].stride);

/* scale up stop pt and set it w.r.t. start pt
 * length is expanded by contiguous stride
 * and reset by offset
 */
        pi[nd-1].stop  = (((old_stop - old_start) + 1)*pi[nd-1].stride) +
	                 pi[nd-1].start - 1;

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

int64_t _PD_hyper_number(PDBfile *file, char *indxpr,
			 dimdes *dims, inti *poff)
   {int i, nd;
    inti maxs, sum, offs;
    char s[MAXLINE];
    dimind *pi;

    SC_strncpy(s, MAXLINE, indxpr, -1);
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

long PD_hyper_number(PDBfile *file ARG(,,cls), const char *name, syment *ep)
   {int c;
    long rv;
    char s[MAXLINE];

/* if name is of the form a[...] strip off the name part
 * by design _PD_hyper_number can't handle anything but the index part
 */
    SC_strncpy(s, MAXLINE, name, -1);
    c = s[0];
    if (strchr("0123456789-.", c) == NULL)
       SC_firsttok(s, "([");

    rv = _PD_hyper_number(file, s, PD_entry_dimensions(ep), NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_VALID_DIMS - Make sure the dimension information specified in a variable
 *                - name is valid
 */

int _PD_valid_dims(dimdes *dimscheck, dimdes *vardims)
   {int nndims, nodims;
    dimdes *od, *nd;

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
   {int nc;
    inti n, start, stop, step;
    char s[MAXLINE];
    char *t;
    dimdes *ndims, *prev, *next, *dp;

    ndims = NULL;

    SC_strncpy(s, MAXLINE, name, -1);
    t = SC_lasttok(s, "[]()");
    if (t != NULL)
       {nc = strlen(t);
	memmove(s, t, nc);}
    else
       nc = 0;
    s[nc] = '\0';

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
   {int err, nc;
    inti ind[3];
    syment *ep;
    dimdes *dims, *pd;
    char s[MAXLINE], lname[MAXLINE], lndx[MAXLINE];
    char *t, *rv;

    SC_strncpy(s, MAXLINE, name, -1);
    if (SC_LAST_CHAR(s) != ']')
       return(CSTRSAVE(s));
    if (SC_NTH_LAST_CHAR(s, 1) == '[')
       return(NULL);

    SC_strncpy(lname, MAXLINE, s, -1);
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
    if (t != NULL)
       {nc = strlen(t);
	memmove(s, t, nc);}
    else
       nc = 0;
    s[nc] = '\0';

    err = FALSE;
    SC_strncpy(lndx, MAXLINE, "[", -1);
    for (pd = dims; pd != NULL; pd = pd->next)
        {t = SC_firsttok(s, " ,()[]\n\r");
	 if (t == NULL)
	    {err = TRUE;
	     break;};

	 if (_PD_parse_index_expr(t, pd, &ind[0], &ind[1], &ind[2]) == 0)
	    {err = TRUE;
	     break;};

	 _PD_index_str(lndx, MAXLINE, ind);};
    
    _PD_rl_syment_d(ep);

    rv = NULL;
    if (err == 0)
       {SC_LAST_CHAR(lndx) = ']';
        snprintf(s, MAXLINE, "%s%s", lname, lndx);
        rv = CSTRSAVE(s);};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                            WRITE ROUTINES                                */

/*--------------------------------------------------------------------------*/

/* _PD_WR_LEAF_MEMBERS - write the direct leaf data */

static void _PD_wr_leaf_members(PDBfile *file, const char *intype,
				const char *outtype, inti ni, void *vr)
   {int cnv, ipt, ok;
    intb bpi;
    inti nb;
    char *out, *in, *bf;
    defstr *dpf;

    if ((intype == NULL) || (outtype == NULL) || (vr == NULL))
       return;

    bpi = -1;

    dpf = _PD_type_lookup(file, PD_CHART_FILE, outtype);
    if (dpf == NULL)
       PD_error("CANNOT FIND TYPE IN THE FILE CHART - _PD_WR_LEAF_MEMBERS",
		PD_WRITE);
    else
       bpi = dpf->size;

    ipt = _PD_items_per_tuple(dpf);
    ni *= ipt;
       
    cnv = _PD_requires_conversion(file, dpf, intype, outtype);
    cnv |= (_PD_TEXT_OUT(file) == TRUE);

    ok = FALSE;
    if (bpi == -1)
       PD_error("CAN'T GET NUMBER OF BYTES - _PD_WR_LEAF_MEMBERS", PD_WRITE);

/* dispatch all other writes */
    else if (cnv == TRUE)
       {nb = ni*bpi;

/* add 100 extra bytes in case we are in TEXT mode and need quotes
 * terminating nulls and maybe a newline
 */
	bf = CMAKE_N(char, nb+100);
        if (bf == NULL)
           PD_error("CAN'T ALLOCATE MEMORY - _PD_WR_LEAF_MEMBERS", PD_WRITE);

	if (SC_zero_on_alloc_n(-1) == FALSE)
	   memset(bf, 0, nb);

        out = bf;
        in  = (char *) vr;
        PD_convert(&out, &in, intype, outtype, ni,
                   file->host_std, file->std, file->host_std,
                   file->host_chart, file->chart, 0, PD_WRITE);

	ok = _PD_filt_block_out(file, (unsigned char *) bf, outtype, bpi, ni);

        CFREE(bf);}
    else
       ok = _PD_filt_block_out(file, (unsigned char *) vr, outtype, bpi, ni);

    if (ok == FALSE)
       PD_error("BYTE WRITE FAILED - _PD_WR_LEAF_MEMBERS", PD_WRITE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WRITE_HYPER_VIF - write a part of a hyper-surface to memory
 *                     - ADDR is the starting address
 *                     - STOP is the upper bound on the address
 *                     - STEP is the increment of the address for each entry
 *                     - FBPI is the bytes per item in the file
 *                     - HBPI is the bytes per item in memory
 */

static char *_PD_write_hyper_vif(PDBfile *file, const char *name,
				 char *in, const char *intype, 
				 syment *ep, intb hbpi, intb fbpi,
				 int64_t addr, inti stop, inti step)
   {inti ni;
    char *outtype;

    outtype = PD_entry_type(ep);

/* items logically contiguous */
    if (step == fbpi)
       {ni = (stop - addr)/step + 1L;

	_PD_set_current_address(file, addr, SEEK_SET, PD_WRITE);

	_PD_sys_write(file, name, in, ni, intype, outtype);}

/* items logically discontiguous */
    else
       {for (; addr <= stop; addr += step, in += hbpi)
	    {_PD_set_current_address(file, addr, SEEK_SET, PD_WRITE);
             _PD_sys_write(file, name, in, 1L, intype, outtype);};};

    return(in);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WRITE_HYPER_SPACE - write a part of a hyper-surface to disk
 *                       - ADDR is the starting address
 *                       - STOP is the upper bound on the address
 *                       - STEP is the increment of the address for each entry
 *                       - FBPI is the bytes per item in the file
 *                       - HBPI is the bytes per item in memory
 */

static char *_PD_write_hyper_space(PDBfile *file, const char *name,
				   char *in, const char *intype, 
				   syment *ep, intb hbpi, intb fbpi,
				   int64_t addr, inti stop, inti step)
   {inti n, nb, niw, ni;
    int64_t eaddr;
    char *outtype;
    SC_array *bl;

    if ((file == NULL) || (ep == NULL))
       return(NULL);

    outtype = PD_entry_type(ep);
    bl      = ep->blocks;

/* items logically contiguous */
    if (step == fbpi)
       {ni = (stop - addr)/step + 1L;

/* get writes across blocks correct */
        while (ni > 0)
           {eaddr = addr;

/* adjust the address for the correct block */
            n = _PD_effective_addr(&eaddr, &nb, fbpi, bl);

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

            _PD_sys_write(file, name, in, niw, intype, outtype);
	    _PD_block_set_valid(ep->blocks, n, PD_BLOCK_INVALID);

            ni   -= niw;
            addr += fbpi*niw;
            in   += hbpi*niw;};}

/* items logically discontiguous */
    else
       {for (; addr <= stop; addr += step, in += hbpi)
            {eaddr = addr;
             n = _PD_effective_addr(&eaddr, &nb, fbpi, bl);
	     _PD_set_current_address(file, eaddr, SEEK_SET, PD_WRITE);

             _PD_sys_write(file, name, in, 1L, intype, outtype);
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
 *                    - FBPI is the bytes per item in the file
 *                    - HBPI is the bytes per item in memory
 */

static char *_PD_wr_hyper_index(PDBfile *file, const char *name,
				char *out, dimind *pi, 
				const char *intype, int64_t addr,
				syment *ep, intb hbpi, intb fbpi)
   {inti stride, step;
    inti offset, start, stop;

/* for each index specification compute the range and recurse */
    stride = fbpi*pi->stride;
    start  = stride*pi->start;
    stop   = stride*pi->stop;
    step   = stride*pi->step;

    stop  = addr + stop - start;
    start = addr;

/* at the bottom of the recursion do the actual operations */
    if (stride <= (long) fbpi)
       {if (file->virtual_internal == TRUE)
	   out = _PD_write_hyper_vif(file, name, out, intype, ep,
				     hbpi, fbpi, start, stop, step);
	else
	   out = _PD_write_hyper_space(file, name, out, intype, ep,
				       hbpi, fbpi, start, stop, step);}
    else
       {for (offset = start; offset <= stop; offset += step)
	    out = _PD_wr_hyper_index(file, name,
				     out, pi + 1, intype, offset,
				     ep, hbpi, fbpi);};

    return(out);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INIT_STACKS - setup dynamic stacks for continuation passing
 *                 - data tree walkers
 */

INLINE static void _PD_init_stacks(long t, long d)
   {int64_t a, f;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    if (pa->rw_list_t == 0)
       {pa->rw_list_t = t;
	pa->rw_list_d = d;

	SC_mem_stats(&a, &f, NULL, NULL);

	DYN_MAKE_STACKS;

	SC_mem_stats_set(a, f);};

    pa->rw_call_stack[0L] = DONE;

    pa->rw_call_ptr = 1L;
    pa->rw_lval_ptr = 0L;
    pa->rw_str_ptr  = 0L;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FIN_STACKS - free dynamic stacks for continuation passing
 *                - data tree walkers
 */

void _PD_fin_stacks(void)
   {PD_smp_state *pa;

    pa = _PD_get_state(-1);

    DYN_FREE_STACKS;

    pa->rw_call_ptr = 0L;
    pa->rw_lval_ptr = 0L;
    pa->rw_str_ptr  = 0L;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_ANNOTATE_TEXT - for text files annotate the output */

int64_t _PD_annotate_text(PDBfile *file, syment *ep, const char *name,
			  int64_t addr)
   {long nc;
    inti ni;
    int64_t na, pa;
    char s[MAXLINE];
    char *typ;

    na = addr;

    if (_PD_TEXT_OUT(file) == TRUE)
       {ni  = PD_entry_number(ep);
	typ = PD_entry_type(ep);
	if (ni > 1)
	   snprintf(s, MAXLINE, "\n# %s %s[%s]\n",
		    typ, name, SC_itos(NULL, 0, ni, NULL));
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

int64_t _PD_wr_syment(PDBfile *file, const char *name,
		      char *vr, int64_t ni,
		      const char *intype, const char *outtype)
   {int dst, indir, count, itags;
    inti i;
    intb size;
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
	litype = (char *) intype;
	lotype = (char *) outtype;}
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
         svr  = (char *) vr;
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

         if (svr != NULL)
	    {PD_CAST_TYPE(ttype, desc, svr+desc->member_offs, svr,
			  PD_error, "BAD CAST - _PD_WR_SYMENT", PD_WRITE);}
         else
	    ttype = NULL;

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
            {_PD_wr_itag(file, name, NULL, 0L, litype, -1L, LOC_HERE);
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
         vr = (vr != NULL) ? DEREF(vr) : NULL;
         if (vr == NULL)
            {_PD_wr_itag(file, name, NULL, 0L, litype, -1L, LOC_OTHER);
             RESTORE_P(char, vr);
             i++;
             vr += sizeof(char *);
             GO(INDIR_ITEM);};

         SAVE_I(ni);
         ni = _PD_number_refd(mem_lst, svr, desc, vr,
			      litype, file->host_chart);
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

         if (_PD_ptr_wr_itags(file, name, vr, ni, litype) == FALSE)
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

int _PD_hyper_write(PDBfile *file, const char *name, syment *ep,
		    void *vr, const char *intype)
   {int nc, nd, c, rv;
    intb hbpi, fbpi;
    char s[MAXLINE];
    char *expr;
    dimdes *dims;
    dimind *pi;

    if ((file == NULL) || (ep == NULL))
       return(0);

    rv = TRUE;

    dims = PD_entry_dimensions(ep);
    SC_strncpy(s, MAXLINE, name, -1);
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
	   {rv  = _PD_sys_write(file, name, vr, PD_entry_number(ep),
				intype, PD_entry_type(ep));
	    rv *= _PD_cksum_block_write(file, ep, 0);};}

    else
       {if (_PD_indirection(PD_entry_type(ep)))
	   PD_error("CAN'T HYPER INDEX INDIRECT TYPE - _PD_HYPER_WRITE",
		    PD_WRITE);

	expr = SC_lasttok(s, "[]()");
	if (expr != NULL)
	   {nc   = strlen(expr) + 1;
	     nc   = min(nc, MAXLINE);
	     memmove(s, expr, nc);};

	pi = _PD_compute_hyper_strides(file, s, dims, &nd);
	if (pi == NULL)
	   PD_error("CAN'T FIND HYPER INDICES - _PD_HYPER_WRITE", PD_WRITE);

	fbpi = _PD_lookup_size(PD_entry_type(ep), file->chart);
	if (fbpi == -1)
	   PD_error("CAN'T FIND NUMBER OF FILE BYTES - _PD_HYPER_WRITE",
		    PD_WRITE);

	hbpi = _PD_lookup_size(intype, file->host_chart);
	if (hbpi == -1)
	   PD_error("CAN'T FIND NUMBER OF HOST BYTES - _PD_HYPER_WRITE",
		    PD_WRITE);

	_PD_wr_hyper_index(file, name, vr, pi, intype, PD_entry_address(ep),
			   ep, hbpi, fbpi);

	CFREE(pi);};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                            READ ROUTINES                                 */

/*--------------------------------------------------------------------------*/

/* _PD_READ_HYPER_SPACE - read a part of a hyper-surface from a data array
 *                      - ADDR is the starting address
 *                      - STOP is the upper bound on the address
 *                      - STEP is the increment of the address for each entry
 *                      - FBPI is the number of bytes in the file for each
 *                      - item to be read
 *                      - HBPI is the number of bytes in memory for each
 *                      - item to be read
 *                      - EP is a scratch syment for temporary use
 *                      - EPO is the originating syment
 *                      - return the number of item successfully read
 */

static int _PD_read_hyper_space(PDBfile *file, const char *name, syment *ep,
				char *out, syment *epo, const char *outtype,
				intb hbpi, intb fbpi, 
                                int64_t addr, inti stop, inti step)
   {int nrd, nr;
    inti n, nb, ni;
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
       {if (step == fbpi)
	   {inti niw;

/* read across blocks */
	    ni = (stop - addr)/step + 1L;
	    while (ni > 0)
	       {eaddr = addr;
		n = _PD_effective_addr(&eaddr, &nb, fbpi, blo);

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

		if (_PD_cksum_block_read(file, name, epo, n) == FALSE)
		   nr = 0;
		else
		   nr = _PD_sys_read(file, ep, outtype, out);
		nrd += nr;

		ni   -= niw;
		addr += fbpi*niw;
		out  += hbpi*niw;};}

/* items not logically contiguous */
        else
	   {ep->number = 1L;
	    for (; addr <= stop; addr += step, out += hbpi)
	        {eaddr = addr;
		 n = _PD_effective_addr(&eaddr, &nb, fbpi, blo);
		 _PD_block_set_address(bl, 0, eaddr);
		 if (_PD_cksum_block_read(file, name, epo, n) == FALSE)
		    nr = 0;
		 else
		    nr = _PD_sys_read(file, ep, outtype, out);
		 nrd += nr;};};}

/* we have a bitstream */
    else
       {defstr* dpf;

	dpf = _PD_type_lookup(file, PD_CHART_FILE, intype);
	if (dpf == NULL)
	   PD_error("CANNOT FIND TYPE IN THE FILE CHART - _PD_READ_HYPER_SPACE",
		    PD_READ);

/* items logically contiguous */
	else if (step == -dpf->size_bits)
	   {ni = (stop - addr)/step + 1L;

/* NOTE: multi-block bitstreams are not supported */
	    ep->number = ni;
	    PD_entry_set_address(ep, addr);
	    nrd += _PD_sys_read(file, ep, outtype, out);
	    out += hbpi*ni;}

/* items not logically contiguous */
	else
	   {ep->number = 1L;
	    for (; addr >= stop; addr += step, out += hbpi)
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
 *                    - FBPI is the number of bytes in the file for each item
 *                    - to be read
 *                    - HBPI is the number of bytes in memory for each item
 *                    - to be read
 *                    - return the number of item successfully read
 */

static int _PD_rd_hyper_index(PDBfile *file, const char *name,
			      syment *ep, char *out,
			      dimind *pi, syment *epo, const char *outtype,
			      int64_t addr, intb hbpi, intb fbpi)
   {inti nrd, nir;
    inti stride, step;
    inti offset, start, stop;
    char *intype;

    intype = PD_entry_type(epo);

/* for each index specification compute the range and recurse */
    stride = fbpi*pi->stride;
    start  = stride*pi->start;
    stop   = stride*pi->stop;
    step   = stride*pi->step;

    if (addr < 0)
       {defstr* dpf;

	dpf = _PD_type_lookup(file, PD_CHART_FILE, intype);
        if (dpf == NULL)
           PD_error("CANNOT FIND TYPE IN THE FILE CHART - _PD_RD_HYPER_INDEX",
		    PD_READ);
	else
	   {stop = addr - dpf->size_bits*((stop - start)/fbpi);
	    step = -dpf->size_bits*(step/fbpi);};}

    else
       stop = addr + (stop - start);

/* at the bottom of the recursion do the actual reads */
    nrd = 0;
    if (stride <= (long) fbpi)
       nrd += _PD_read_hyper_space(file, name, ep, out, epo, outtype, 
				   hbpi, fbpi, addr, stop, step);
    else if (addr < 0)
       {for (offset = -addr; offset <= -stop; offset -= step)
            {nir = _PD_rd_hyper_index(file, name, ep, out, pi + 1, epo,
				      outtype, -offset, hbpi, fbpi);
	     nrd += nir;
             out += nir*hbpi; };}
    else
       {for (offset = addr; offset <= stop; offset += step)
	    {nir = _PD_rd_hyper_index(file, name, ep, out, pi + 1, epo,
				      outtype, offset, hbpi, fbpi);
	     nrd += nir;
             out += nir*hbpi; };};

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

static void _PD_rd_leaf_members(PDBfile *file, char *vr, inti ni, 
                                const char *intype, const char *outtype,
				int boffs)
   {int ipt, cnv, ok;
    inti nia, nb;
    intb bpi, nbt;
    char *bf, *in, *out;
    defstr *dpf;

    if ((intype == NULL) || (outtype == NULL) || (vr == NULL))
       return;

    bpi = -1;
    dpf = _PD_type_lookup(file, PD_CHART_FILE, intype);
    ipt = _PD_items_per_tuple(dpf);

    if (dpf == NULL)
       PD_error("CANNOT FIND TYPE IN THE FILE CHART - _PD_RD_LEAF_MEMBERS",
		PD_READ);
    else
       bpi = dpf->size;

    ni *= ipt;

    cnv = _PD_requires_conversion(file, dpf, outtype, intype);

    ok = FALSE;
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

        ok = _PD_filt_block_in(file, (unsigned char *) bf, intype, bpi, nia);
        if (ok == TRUE)
	   {in  = bf;
            out = vr;
            PD_convert(&out, &in, intype, outtype, ni,
                       file->std, file->host_std, file->host_std,
                       file->chart, file->host_chart,
		       boffs, PD_READ);
            CFREE(bf);}
        else
           CFREE(bf);}

    else
       ok = _PD_filt_block_in(file, (unsigned char *) vr, intype, bpi, ni);

    if (ok == FALSE)
       PD_error("DATA READ FAILED - _PD_RD_LEAF_MEMBERS", PD_READ);

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

int _PD_indexed_read_as(PDBfile *file, char *fullpath,
			const char *type, void *vr, 
                        int nd, long *ind, syment *ep)
   {int i, j, err;
    inti indl[3];
    char lndx[MAXLINE], hname[MAXLINE];
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

    SC_strncpy(lndx, MAXLINE, "(", -1);
    for (i = 0; i < nd; i++)
        {for (j = 0; j < 3; j++)
	     indl[j] = *ind++;

	 _PD_index_str(lndx, MAXLINE, indl);};

    if (strlen(lndx) > 1)
       {SC_LAST_CHAR(lndx) = ')';
        snprintf(hname, MAXLINE, "%s%s", fullpath, lndx);}

    else
       SC_strncpy(hname, MAXLINE, fullpath, -1);

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

int _PD_hyper_read(PDBfile *file, const char *name, const char *outtype,
		   syment *ep, void *vr)
   {int nc, nd, c, nrd;
    intb hbpi, fbpi;
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
       {if (_PD_cksum_block_read(file, name, ep, -1) == FALSE)
	   nrd = 0;
	else
	   nrd = _PD_sys_read(file, ep, outtype, vr);}

    else
       {dims = PD_entry_dimensions(ep);
	SC_strncpy(s, MAXLINE, name, -1);
	c = SC_LAST_CHAR(s);
	if (((c != ')') && (c != ']')) || (dims == NULL))
	   {if (_PD_cksum_block_read(file, name, ep, 0) == FALSE)
	       nrd = 0;
	    else
	       nrd = _PD_sys_read(file, ep, outtype, vr);}

	else
	   {if (_PD_indirection(outtype))
	       PD_error("CAN'T HYPER INDEX INDIRECT TYPE - _PD_HYPER_READ",
			PD_READ);

	    expr = SC_lasttok(s, "[]()");
	    if (expr != NULL)
	       {nc   = strlen(expr) + 1;
		nc   = min(nc, MAXLINE);
		memmove(s, expr, nc);};

	    pi = _PD_compute_hyper_strides(file, s, dims, &nd);
	    if (pi == NULL)
	       PD_error("CAN'T FIND HYPER INDICES - _PD_HYPER_READ", PD_READ);

	    fbpi = _PD_lookup_size(PD_entry_type(ep), file->chart);
	    if (fbpi == -1)
	       PD_error("CAN'T FIND NUMBER OF FILE BYTES - _PD_HYPER_READ",
			PD_READ);

	    hbpi = _PD_lookup_size(outtype, file->host_chart);
	    if (hbpi == -1)
	       PD_error("CAN'T FIND NUMBER OF HOST BYTES - _PD_HYPER_READ",
			PD_READ);

/* make a dummy for the hyper read to use as scratch space */
	    tep = _PD_mk_syment(NULL, 0L, 0L, &(ep->indirects), NULL);

	    nrd = _PD_rd_hyper_index(file, name, tep, vr, pi, ep,
				     outtype, PD_entry_address(ep),
				     hbpi, fbpi);

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
		 const char *name, const char *type, int64_t ni,
		 int sgned, int nbits, int padsz, int fpp,
		 int64_t offs, long *pan, char **pdata)
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
 *             -   NI      the number of items requested
 *             -   SGNED   TRUE if the data type is signed
 *             -   NBITS   the number of bits per item
 *             -   PADSZ   the number of bits of pad preceding the fields
 *             -   FPP     the number of fields per pad
 *             -   OFFS    offset from the beginning of the input data
 *             -   PAN     the number of items found
 *             -   PDATA   the data array returned
 */

int _PD_rd_bits(PDBfile *file, const char *name, const char *type, inti ni,
		int sgned, intb nbits, int padsz, int fpp,
		inti offs, long *pan, char **pdata)
   {int i, ityp, out_flag, onescmp;
    int *ord;
    inti nii, nie, nir;
    intb bpie, bpio;
    char *etype, *in, *out;
    syment *ep;
    defstr *dp1, *dp2;

    ep = _PD_effective_ep(file, name, TRUE, NULL);
    if (ep == NULL)
       return(FALSE);

    nii   = PD_entry_number(ep);
    etype = PD_entry_type(ep);

    dp1 = PD_inquire_type(file, etype);
    if (dp1 == NULL)
       return(FALSE);

    onescmp = dp1->onescmp;

    dp2 = PD_inquire_host_type(file, type);
    if (dp2 == NULL)
       return(FALSE);

    out_flag = dp2->fix.order;

    bpie = _PD_lookup_size(etype, file->chart);
    nie  = nii*bpie;

    in = CMAKE_N(char, nie);

    CFREE(etype);

    ep->type   = CSTRSAVE(G_CHAR_S);
    ep->number = nie;
    nir = _PD_sys_read(file, ep, G_CHAR_S, in);
    if (nir != nie)
       {CFREE(in);
	_PD_rl_syment_d(ep);
	return(FALSE);};

    bpio = _PD_lookup_size(type, file->host_chart);
    out  = _PD_alloc_entry(file, type, ni);
    ityp = SC_type_id(type, FALSE);

    if ((SC_is_type_char(ityp) == FALSE) && (SC_is_type_fix(ityp) == FALSE))
       return(FALSE);

    SC_unpack_bits(out, in, ityp, nbits, padsz, fpp, ni, offs);

/* convert integers */
    if (SC_is_type_char(ityp) == FALSE)
       {ord = CMAKE_N(int, bpio);

        if (out_flag == NORMAL_ORDER)
           for (i = 0; i < bpio; ord[i] = i + 1, i++);
        else                         
           for (i = 0; i < bpio; ord[i] = bpio - i, i++);

        if (sgned)
           _PD_sign_extend(out, ni, bpio, nbits, ord);

        if (onescmp)
           _PD_ones_complement(out, ni, bpio, ord);

        CFREE(ord);};

    *pan   = ni;
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

int64_t _PD_rd_syment(PDBfile *file, syment *ep,
		      const char *outtype, void *vr)
   {int dst, vif, count, itags;
    inti i, n, ni, nrd, size;
    intb bpi, boffs;
    int64_t addr, eaddr;
    PD_data_location loc;
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
    addr    = 0;
    boffs   = 0;
    size    = 0;
    ni      = 0;

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
	     if (pv != NULL)
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
	     if (pv != NULL)
	        DEREF(pv) = DEREF(ad.memaddr);
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
         if (lvr != NULL)
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
   {inti n, nr;
    intb bpi;
    char bf[MAXLINE];
    FILE *fp;

    fp  = file->stream;
    bpi = file->std->ptr_bytes;

/* set the file location */
    _PD_set_current_address(file, addr, SEEK_SET, PD_READ);

/* read the pointer value from the file location */
    nr = lio_read(bf, bpi, 1, fp);
    if (nr == 1)
       {intb fbpi, hbpi;
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

