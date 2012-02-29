/*
 * PDPATH.C - grammar driven parser for variable specifications
 *          - THREADSAFE
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

/* The fundamental operations are:
 *        GOTO    - goto the place in memory or on disk implied by the
 *                  locator on the top of the stack
 *        INDEX   - compute the hyper-space shape implied by the
 *                  dimensions on the top of the stack this implies
 *                  an offset from the current location and a
 *                  number of items (max) from the offset
 *                  the current location is changed by offset from
 *                  the previous location
 *        MEMBER  - item on the top of the stack is a member name
 *                  and implies an offset from the current location
 *                  the current location is changed by offset from
 *                  the previous location
 *        DEREF   - assuming the current location is a pointer in
 *                  memory or an itag on disk dereference so that
 *                  the current location is at the pointee
 *        DIGRESS - begin a subroutine which will result with a
 *                - new integer value on the stack upon completion
 *        CAST    - specify an output type that overrides the
 *                - file type
 */

#define MAXPARSEDEPTH 150
#define LASTTOK        42
#define STATEFLAG   -1000

enum e_PD_expr_action
   {GOTO_C   = 1,
    MEMBER_C = 2,
    INDEX_C  = 3,
    CAST_C   = 4,
    DEREF_C  = 5,
    RESULT_C = 6};

typedef enum e_PD_expr_action PD_expr_action;

enum e_PD_expr_syntax
   {ERRCODE     = 256,
    OPEN_PAREN  = 257,
    CLOSE_PAREN = 258,
    STAR        = 259,
    DOT         = 260,
    ARROW       = 261,
    IDENTIFIER  = 262,
    COMMA       = 263,
    COLON       = 264,
    INTEGER     = 265};

typedef enum e_PD_expr_syntax PD_expr_syntax;

#define input()                                                              \
   FRAME(lex_bf)[FRAME(index)++]

#define unput(c)                                                             \
   (FRAME(index) = (--FRAME(index) < 0) ? 0 : FRAME(index),                  \
    FRAME(lex_bf)[FRAME(index)] = c)

#define GOT_TOKEN(tok)                                                       \
    {if (FRAME(index) == start+1)                                            \
        return(tok);                                                         \
     else                                                                    \
        {unput(c);                                                           \
	 return(_PD_next_token(pa, start));};}

#define CURRENT(x) FRAME(stack)[FRAME(n)].x

#define FRAME_FRAME     pa->frames
#define FRAME_N         pa->frame_n
#define FRAME_NX        pa->frame_nx
#define FILE_S          pa->file_s
#define OUTTYPE         pa->outtype
#define HAVE_COLON      pa->have_colon
#define MSG             pa->msg
#define TEXT            pa->text
#define NUM_VAL         pa->num_val
#define FRAME_OBJ       ((parse_frame *) FRAME_FRAME)
#define FRAME(x)        FRAME_OBJ[FRAME_N].x

typedef struct s_locator locator;
typedef struct s_parse_frame parse_frame;

struct s_locator
   {char intype[MAXLINE];
    int cmmnd;
    int indirect;
    SC_address ad;
    long number;
    dimdes *dims;
    SC_array *blocks;
    int disposable;
    long n_struct_ptr;
    long n_array_items;
    symindir indir_info;};

struct s_parse_frame
   {locator *stack;
    long n;
    long nx;
    int64_t diskaddr;
    char path[MAXLINE];
    int flag;
    char *lex_bf;
    char *lval;
    char *val;
    char *v[MAXPARSEDEPTH];           	/* parser value stack */
    char **pv;		                /* top of parser value stack */
    int current_token;			/* current input token number */
    int error;		                /* error recovery flag */
    int n_error;			/* number of errors */
    int state;		         	/* current state */
    int tmp;	            		/* extra var (lasts between blocks) */
    int s[MAXPARSEDEPTH];		/* parser state stack */
    int *ps;		                /* top of parser state stack */
    int index;};


/*---------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPSTACK - print the locator stack for debug purposes */

void dpstack(int tid, int n)
   {int i;
    char cmmnds[MAXLINE];
    PD_smp_state *pa;

    pa = _PD_get_state(tid);

    PRINT(stdout, "\n%16s %8s %4s %8s %3s %3s  %s  %s\n",
          "Type", "Dims", "N", "Address", "#A", "#S", "ILoc", "Command");
    for (i = 1; i <= n; i++)
        {switch (FRAME(stack)[i].cmmnd)
            {case GOTO_C :
                  strcpy(cmmnds, "GOTO");
                  break;
             case MEMBER_C :
                  strcpy(cmmnds, "MEMBER");
                  break;
             case INDEX_C :
                  strcpy(cmmnds, "INDEX");
                  break;
             case DEREF_C :
                  strcpy(cmmnds, "DEREF");
                  break;
             case CAST_C :
                  strcpy(cmmnds, "CAST");
                  break;
             case RESULT_C :
                  strcpy(cmmnds, "RESULT");
                  break;
             default :
                  strcpy(cmmnds, "UNKNOWN");
                  break;};

        PRINT(stdout, "%16s %8lx %4ld ",
              FRAME(stack)[i].intype,
              FRAME(stack)[i].dims,
              FRAME(stack)[i].number);

	if (FILE_S->virtual_internal)
	   PRINT(stdout, "%8lx ",
		 FRAME(stack)[i].ad.memaddr);
        else
	   PRINT(stdout, "%8ld ",
		 FRAME(stack)[i].ad.diskaddr);

        PRINT(stdout, "%3ld %3ld %5ld   %s\n",
              FRAME(stack)[i].n_array_items,
              FRAME(stack)[i].n_struct_ptr,
              FRAME(stack)[i].indir_info.addr,
              cmmnds);};

    PRINT(stdout, "\n");

    return;}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* DSTCKTR - debugging aid */

static void dstcktr(int cmnd, int i, int64_t addr, long numb, int end)
   {char *cmd;
    parse_frame *fr;
    locator *stck;
    PD_smp_state *pa;
    static int dbg = FALSE;

    if (dbg == TRUE)
       {pa = _PD_get_state(-1);

        fr   = FRAME_OBJ + FRAME_N;
	stck = fr->stack;
	if (end == 0)
	   {if (cmnd == DEREF_C)
	       cmd = "deref";
	    else if (cmnd == INDEX_C)
	       cmd = "index";
	    else if (cmnd == MEMBER_C)
	       cmd = "member";
	    else if (cmnd == CAST_C)
	       cmd = "cast";
	    else
	       cmd = " ";

	    PRINT(stdout, "%3d %8s %14s (%3ld/%3ld) %6ld -> ",
		  i, cmd,
		  stck[i-1].intype, stck[i].n_array_items, numb,
		  (long) addr);}
	else
	    PRINT(stdout, "%14s %3ld %6ld\n",
		  stck[i].intype, numb, (long) addr);};

    return;}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_RL_FRAME_BLOCKS - release blocks on the locator stack between
 *                     - NMN and NMX except for NE
 */

static void _PD_rl_frame_blocks(PD_smp_state *pa, int nmn, int nmx, long ne)
   {int i;
    SC_array *bl;

    for (i = nmn; i <= nmx; i++)
        {if (i == ne)
            continue;

	 bl = FRAME(stack)[i].blocks;
	 if ((bl != NULL) && FRAME(stack)[i].disposable)
	    _PD_block_free(bl);};

    return;}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_RL_FRAMES - free the set parse frames */

static void _PD_rl_frames(PD_smp_state *pa)
   {

    CFREE(FRAME(stack));
    CFREE(FRAME(lex_bf));
    CFREE(FRAME_FRAME);

    return;}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_SAVE_STACK - save the state of the current parse */

static void _PD_save_stack(PD_smp_state *pa)
   {

    FRAME_N++;
    if (FRAME_N >= FRAME_NX)
       {FRAME_NX += 2;
        CREMAKE(FRAME_FRAME, parse_frame, FRAME_NX);};

    SC_MEM_INIT(parse_frame, &FRAME_OBJ[FRAME_N]);

    return;}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_RESTORE_STACK - restore the state of the previous parse */

static void _PD_restore_stack(PD_smp_state *pa)
   {

    CFREE(FRAME(stack));
    CFREE(FRAME(lex_bf));
    FRAME_N--;

    return;}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_NUM_INDIRECTS - count up the number of members of the given
 *                   - structure with indirect references
 *                   - includes the indirections of children too
 */

intb _PD_num_indirects(char *type, hasharr *tab)
   {intb rv;
    char *mtype;
    defstr *dp;

    rv = 0;

    mtype = _PD_member_base_type(type);
    dp    = PD_inquire_table_type(tab, mtype);
    CFREE(mtype);

    if (dp == NULL)
       PD_error("CAN'T FIND TYPE - _PD_NUM_INDIRECTS", PD_TRACE);

    else
       rv = dp->n_indirects;

    return(rv);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_BLOCK_DEREF_ADDR - handle block manipulations
 *                      - for deref_addr variants
 */

static void _PD_block_deref_addr(PD_smp_state *pa, PDBfile *file,
				 locator *stck, long n,
				 int64_t addr, long numb)
   {dimdes *dims;
    SC_array *bl;

    if (!_PD_indirection(stck[n].intype))
       {bl = _PD_block_make(1);
	_PD_block_set_desc(addr, numb, bl, 0);

	if ((n + 1) == FRAME(n))
	   dims = _PD_mk_dimensions(file->default_offset, numb);
	else
	   dims = NULL;

	stck[n].blocks = bl;
	stck[n].dims   = dims;

	if (n < FRAME(n))
	   {if (stck[n+1].cmmnd == INDEX_C)
	       {stck[n+1].blocks = bl;
		stck[n+1].dims   = dims;};};};

    return;}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_BLOCK_INDEX_DEREF - handle block manipulations
 *                       - for index_deref variants
 *                       - change ILOC, ADDR, NI
 *                       - return new block list, NBL
 */

static SC_array *_PD_block_index_deref(locator *stck, long n,
				       hasharr *tab, char *type,
				       symindir *iloc,
				       int64_t *paddr, long *pnumb)
   {inti i, nbk, nib, nbb, nit, ni;
    intb bpi;
    int64_t addr;
    defstr *dp;
    SC_array *nbl, *bl;

    addr = *paddr;

    stck[n].dims = stck[n-1].dims;
    addr = stck[n-1].ad.diskaddr;
    nbk  = stck[n-1].number;

    bl = stck[n].blocks;
    ni = stck[n].ad.diskaddr;

    bpi = _PD_lookup_size(type, tab);

    iloc->addr = addr + nbk*bpi;

/* deal with multiblock entries */
    nbl = NULL;

/* NOTE: it is not the most general thing to assume that bitstreams
 *       (indicated by negative addresses) must be contiguous although
 *       all current examples are
 */
    if ((bl != NULL) && (addr >= 0))
       {nbk = SC_array_get_n(bl);
	nbb = 0;

/* find out which block we got into */
	for (i = 0; i < nbk; i++)
	    {_PD_block_get_desc(&addr, &nib, bl, i);
	     nbb = nib*bpi;
	     if (ni < nbb)
	        break;

	     iloc->arr_offs -= nib;
	     ni -= nbb;};

	iloc->addr = addr + nbb;

/* make a copy of the remaining blocks for the effective entry */
	if (i < nbk)
	   {stck[n].disposable = TRUE;
	    nbl = _PD_block_copy_seg(bl, i, nbk);

/* adjust the first block to be consistent with the rest of the locator */
	    nit  = _PD_block_get_number(nbl, 0);
	    nit -= ni/bpi;
	    _PD_block_set_desc(addr + ni, nit, nbl, 0);};};

    if (addr < 0)
       {dp = PD_inquire_table_type(tab, type);
	if (dp != NULL)
	   addr -= (ni/bpi)*dp->size_bits;}

    else
       {*pnumb = stck[n].number;
	addr  += ni;};

    *paddr = addr;

    return(nbl);}

/*---------------------------------------------------------------------------*/

/*                           ITAG REDUCE ROUTINES                            */

/*---------------------------------------------------------------------------*/

/* _PD_SKIP_OVER - given a number of units, skip over that many units
 *               - including subunits referenced by the top level units
 *               - if noind is TRUE don't pick up the additional indirects
 */

int64_t _PD_skip_over(PDBfile *file, long skip, int noind)
   {int indir;
    intb bpi;
    int64_t addr;
    hasharr *tab;
    PD_itag itag;

    tab = file->chart;

    while (skip-- > 0L)
       {(*file->rd_itag)(file, NULL, &itag);

/* note whether this is an indirection */
        indir = _PD_indirection(itag.type);

/* if noind is TRUE don't pick up the indirects */
        if (noind == FALSE)

/* if it is an indirection we have more to skip over */
           {if (indir)
               skip += itag.nitems;

/* NOTE: this clause was added to handle the case of correctly
 * picking up the indirections with a struct ** where the
 * struct also contains indirections.
 * if it is a structure with indirections we have more to skip over
 */
            else if (itag.flag == 1)
               skip += itag.nitems*_PD_num_indirects(itag.type, tab);};

/* if it was not a NULL pointer find it */
        if ((itag.addr != -1) && (itag.nitems != 0L))
           {if ((itag.flag == LOC_OTHER) && (skip == -1))
	       {_PD_set_current_address(file, itag.addr, SEEK_SET, PD_TRACE);
		(*file->rd_itag)(file, NULL, &itag);};

/* layered indirects have no "data" bytes written out to be skipped over */
            if (indir == 0)
               {bpi = _PD_lookup_size(itag.type, tab);
                if (bpi == -1)
                   PD_error("CAN'T FIND NUMBER OF BYTES - _PD_SKIP_OVER",
                            PD_TRACE);}
            else
               bpi = 0;

/* if its here, step over the data */
            if ((itag.flag == LOC_HERE) && (skip > -1))
               {addr = bpi*itag.nitems;
                if (!indir)
		   _PD_set_current_address(file, addr, SEEK_CUR, PD_TRACE);};};};

    addr = _PD_get_current_address(file, PD_TRACE);

    return(addr);}

/*--------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_ITAG_DEREF_ADDR - dereference a pointer and return the correct address
 *                     - of the pointee
 *                     - the entire parse tree is available to provide all
 *                     - necessary context
 */

static int64_t _PD_itag_deref_addr(PD_smp_state *pa, int n)
   {inti numb;
    intb bpi;
    int64_t addr;
    char *type;
    hasharr *tab;
    parse_frame *fr;
    locator *stck;
    PDBfile *file;

    file = FILE_S;
    fr   = FRAME_OBJ + FRAME_N;
    stck = fr->stack;

    tab  = file->chart;
    type = stck[n-1].intype;
    bpi  = _PD_lookup_size(type, tab);

/* handle the case of in memory pointers */
    if (file->virtual_internal)
       {addr = stck[n].ad.diskaddr;
        numb = stck[n].number;}

/* handle the case of file pointers */
    else
       {PD_itag itag;

        addr = stck[n-1].ad.diskaddr;
	numb = stck[n-1].number;

/* get past the level that contains the dereference
 * NOTE: when using ITAGS, PDB declines to write top level pointers
 * which are useless numbers, it starts in with the pointees and
 * hence the start of such objects are the itags of the pointees
 */
	if ((file->use_itags == FALSE) || (_PD_indirection(type) == FALSE))
	   addr += numb*bpi;

	_PD_set_current_address(file, addr, SEEK_SET, PD_TRACE);

	(*file->rd_itag)(file, NULL, &itag);

	addr = _PD_get_current_address(file, PD_TRACE);
        numb = itag.nitems;

	_PD_block_deref_addr(pa, file, stck, n, addr, numb);};

    stck[n].number      = numb;
    stck[n].ad.diskaddr = addr;

    return(addr);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_ITAG_INDEX_DEREF - handle indexing where a pointered type was just
 *                      - dereferenced this will mean skipping over itags
 *                      - and other pointees
 */

static int64_t _PD_itag_index_deref(PD_smp_state *pa, int n,
				  dimdes **pdims, long *pnumb)
   {inti indx, numb, naitems, fn;
    intb bpi;
    int64_t addr;
    char *type, *typc, *typp;
    SC_array *nbl;
    symindir iloc;
    PDBfile *file;
    parse_frame *fr;
    locator *stck;
    hasharr *tab;

    file = FILE_S;
    fr   = FRAME_OBJ + FRAME_N;
    stck = fr->stack;
    fn   = fr->n;

    nbl = NULL;

    iloc.addr       = 0;
    iloc.n_ind_type = 0L;
    iloc.arr_offs   = 0L;

/* handle in memory indexing */
    if (file->virtual_internal)
       addr = stck[n].ad.diskaddr;

/* handle file indexing */
    else

/* start at the address before the latest DEREF */
       {typp = stck[n-1].intype;
	type = stck[n].intype;
	typc = stck[n+1].intype;
	indx = stck[n].n_array_items;

        tab = file->chart;

	iloc.n_ind_type = _PD_num_indirects(type, tab);
	iloc.arr_offs   = indx;

/* in order to know where to go you have to know whether the
 * next thing on the locator stack dereferences a pointer
 */
	if (((n < fn) && _PD_indirection(typc)) || _PD_indirection(typp))
	   {numb = stck[n-1].number;
	    if ((indx < 0) || (numb < indx))
	       PD_error("INDEX OUT OF BOUNDS - _PD_ITAG_INDEX_DEREF", PD_TRACE);

/* handle GOTO, DEREF, INDEX */
	    if (stck[n-1].cmmnd == DEREF_C)
	       {addr = stck[n-2].ad.diskaddr;
		_PD_set_current_address(file, addr, SEEK_SET, PD_TRACE);

/* skip over the thing that was DEREF'd to where its pointees begin */
		addr = _PD_skip_over(file, 1L, TRUE);

/* skip over all items before the indexed one */
		numb    = _PD_num_indirects(type, tab);
		naitems = indx*max(1, numb);
		addr    = _PD_skip_over(file, naitems, FALSE);}

/* handle GOTO, INDEX */
	    else
	      {addr = stck[n-1].ad.diskaddr;

	       if (!_PD_indirection(typp))
		  {bpi   = _PD_lookup_size(typp, tab);
		   addr += numb*bpi;
		   _PD_set_current_address(file, addr, SEEK_SET, PD_TRACE);

/* skip over all items before the indexed one */
		   numb    = _PD_num_indirects(typp, tab);
		   naitems = indx*max(1, numb);
		   addr    = _PD_skip_over(file, naitems, FALSE);}

/* NOTE: if we get here, then we have an array of pointers (the
 *       data for which is not written by PDB - the pointers are
 *       meaningless numbers) consequently we are staring at the
 *       ITAG of the first pointee
 */
	       else
		  {PD_itag itag;

/* be sure that we are at the first ITAG */
		   _PD_set_current_address(file, addr, SEEK_SET, PD_TRACE);

		   *pdims = NULL;

/* skip over to the indexed element */
		   numb    = _PD_num_indirects(typp, tab);
		   naitems = indx*max(1, numb);
		   addr    = _PD_skip_over(file, naitems, FALSE);

		   (*file->rd_itag)(file, NULL, &itag);
		   if (!itag.flag)
		      {_PD_set_current_address(file, addr, SEEK_SET, PD_TRACE);
		       (*file->rd_itag)(file, NULL, &itag);};

		   numb   = itag.nitems;
		   *pnumb = numb;
		   stck[n].number   = numb;

/* after doing one index the next thing has to be contiguous */
		   stck[n+1].blocks = NULL;

		   addr = _PD_get_current_address(file, PD_TRACE);};};}

/* handle direct types simply */
	else
	   {

/* GOTCHA: it is a temporary measure to pass the old dimensions up the stack
 *         the correct thing to do is to distinguish between the dimensions
 *         of the source and the effective dimension of the target.  This
 *         will never be right until then
 */
	    if (*pdims == NULL)
	       *pdims = stck[n].dims;

	    nbl = _PD_block_index_deref(stck, n, tab, type,
					&iloc, &addr, pnumb);};};

    if (stck[n-1].cmmnd == DEREF_C)
       _PD_block_free(stck[n].blocks);

    stck[n].blocks      = nbl;
    stck[n].ad.diskaddr = addr;
    stck[n].indir_info  = iloc;

    return(addr);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_ITAG_MEMBER_DEREF - find the member where a pointered type was just
 *                       - dereferenced this will mean skipping over itags
 *                       - and other pointees
 */

static int64_t _PD_itag_member_deref(PD_smp_state *pa, int n)
   {long nsitems;
    int64_t addr;
    char *type;
    parse_frame *fr;
    locator *stck;
    PDBfile *file;

    file = FILE_S;
    fr   = FRAME_OBJ + FRAME_N;
    stck = fr->stack;

/* handle in memory members */
    if (file->virtual_internal)
       addr = stck[n].ad.diskaddr;

/* handle file members */
    else
       {int indir, cmmnd;
        inti numb;
        intb bpi;

	cmmnd = stck[n-1].cmmnd;
	indir = _PD_indirection(stck[n].intype);
	if ((cmmnd == GOTO_C) && indir)
	   {addr = stck[n-1].ad.diskaddr;
	    type = stck[n-1].intype;
	    numb = stck[n-1].number;
            bpi  = _PD_lookup_size(type, file->chart);

	    addr += bpi*numb;

	    _PD_set_current_address(file, addr, SEEK_SET, PD_TRACE);}

        else if ((cmmnd != INDEX_C) && indir)
	   {addr = stck[n-2].ad.diskaddr;

	    _PD_set_current_address(file, addr, SEEK_SET, PD_TRACE);

/* skip over the thing that was DEREF'd to where its pointees begin */
	    addr = _PD_skip_over(file, 1L, TRUE);}

/* start at the address in the previous locator */
        else
	   addr = stck[n-1].ad.diskaddr;

/* handle indirect types differently from direct ones */
        type = stck[n].intype;
	if (_PD_indirection(type))
	   {nsitems = stck[n].n_struct_ptr;

	    _PD_set_current_address(file, addr, SEEK_SET, PD_TRACE);

/* skip over all items before the specified member */
	    addr = _PD_skip_over(file, nsitems, FALSE);}

/* handle direct types simply */
	else
	   addr += stck[n].ad.diskaddr;};

    stck[n].ad.diskaddr = addr;

    return(addr);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_ITAG_REDUCE - reduce the parse tree
 *                 - this means looping over the locator stack thru the
 *                 - latest GOTO command and determining a new locator
 *                 - whose intype, dimensions, number, and address can
 *                 - be used to create an valid effective symbol table entry
 *                 - or an actual one
 *                 - if there is an intermediate expression on the stack
 *                 - it will be read and the value (which can only be an
 *                 - index) returned
 */

static long _PD_itag_reduce(PD_smp_state *pa)
   {int i, nmn, nmx, cmnd;
    long numb;
    int64_t addr; 
    long val;
    char *type;
    dimdes *dims;
    SC_array *bl;
    symindir iloc;
    parse_frame *fr;
    locator *stck;
    PDBfile *file;

    file = FILE_S;
    fr   = FRAME_OBJ + FRAME_N;
    stck = fr->stack;

    val = 0L;
    nmx = FRAME(n);

    type = CURRENT(intype);
    numb = CURRENT(number);
    dims = CURRENT(dims);

/* find the most recent GOTO commmand */
    for (i = nmx; i > 0; i--)
        {cmnd = stck[i].cmmnd;
         if (cmnd == GOTO_C)
            break;};

    nmn  = max(i, 1);
    addr = 0;

    iloc.addr       = 0;
    iloc.n_ind_type = 0L;
    iloc.arr_offs   = 0L;

/* find the actual address of the specified object */
    if (file->virtual_internal)
       addr = stck[nmx].ad.diskaddr;

    else
       {for (i = nmn; i <= nmx; i++)
	    {cmnd = stck[i].cmmnd;
	     stck[i].disposable = FALSE;
	     if (cmnd == DEREF_C)
	        addr = _PD_itag_deref_addr(pa, i);
	     else if (cmnd == INDEX_C)
	        {addr = _PD_itag_index_deref(pa, i, &dims, &numb);
		 iloc = stck[i].indir_info;}
	     else if (cmnd == MEMBER_C)
	        {addr = _PD_itag_member_deref(pa, i);
		 numb = stck[i].number;}
	     else if (cmnd != CAST_C)
	        {addr += stck[i].ad.diskaddr;
		 stck[i].ad.diskaddr = addr;};};};

/* this must be taken now because the address reduction may have
 * changed the original
 */
    bl = CURRENT(blocks);

    _PD_rl_frame_blocks(pa, nmn, nmx, FRAME(n));

    FRAME(n) = nmn;

/* if we are not at the bottom of the locator stack we have
 * and intermediate expression which must by read in via _PD_sys_read
 */
    if (nmn != 1)
       {syment *ep;

        if (numb != 1L)
           PD_error("INTERMEDIATE MUST BE SCALAR INTEGER - _PD_ITAG_REDUCE",
                    PD_TRACE);

        ep = _PD_mk_syment(CURRENT(intype), 1L, addr, NULL, NULL);
	_PD_sys_read(file, ep, SC_LONG_S, &val);
        _PD_rl_syment(ep);

        FRAME(n)--;}

/* otherwise we are at the end of the locator stack and the necessary
 * information to build an effective syment must be filled in the
 * bottom most locator
 */
    else
       {/* dpstack(pa, nmx); */

        strcpy(CURRENT(intype), type);

        CURRENT(number)      = numb;
        CURRENT(ad.diskaddr) = addr;
        CURRENT(blocks)      = bl;
	CURRENT(dims)        = dims;
	CURRENT(indir_info)  = iloc;
        CURRENT(cmmnd)       = RESULT_C;};

    return(val);}

/*---------------------------------------------------------------------------*/

/*                         ITAGLESS REDUCE ROUTINES                          */

/*---------------------------------------------------------------------------*/

/* _PD_PTR_DEREF_ADDR - dereference a pointer and return the correct address
 *                    - of the pointee
 *                    - the entire parse tree is available to provide all
 *                    - necessary context
 */

static int64_t _PD_ptr_deref_addr(PD_smp_state *pa, int n)
   {long numb;
    int64_t addr;
    syment *ep;
    parse_frame *fr;
    locator *stck;
    PDBfile *file;

    file = FILE_S;
    fr   = FRAME_OBJ + FRAME_N;
    stck = fr->stack;

/* handle the case of in memory pointers */
    if (file->virtual_internal)
       {addr = stck[n].ad.diskaddr;
        numb = stck[n].number;}

/* handle the case of file pointers */
    else
       {addr = stck[n-1].ad.diskaddr;
	numb = stck[n-1].number;

	_PD_set_current_address(file, addr, SEEK_SET, PD_TRACE);

	ep = _PD_ptr_read(file, addr, TRUE);
	if (ep == NULL)
	   {addr = 0;
	    numb = 0;}
	else
	   {addr = PD_entry_address(ep);
	    numb = PD_entry_number(ep);};

	_PD_block_deref_addr(pa, file, stck, n, addr, numb);};

    stck[n].number      = numb;
    stck[n].ad.diskaddr = addr;

    return(addr);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_PTR_INDEX_DEREF - handle indexing where a pointered type was just
 *                     - dereferenced this will mean skipping over itags
 *                     - and other pointees
 */

static int64_t _PD_ptr_index_deref(PD_smp_state *pa, int n,
                                 dimdes **pdims, long *pnumb)
   {int64_t addr;
    char *type;
    SC_array *nbl;
    symindir iloc;
    PDBfile *file;
    parse_frame *fr;
    locator *stck;
    hasharr *tab;

    file = FILE_S;
    fr   = FRAME_OBJ + FRAME_N;
    stck = fr->stack;

    nbl = NULL;

    iloc.addr       = 0;
    iloc.n_ind_type = 0L;
    iloc.arr_offs   = 0L;

/* handle in memory indexing */
    if (file->virtual_internal)
       addr = stck[n].ad.diskaddr;

/* handle file indexing */
    else
       {tab  = file->chart;
	type = stck[n].intype;

/* GOTCHA: it is a temporary measure to pass the old dimensions up the stack
 *         the correct thing to do is to distinguish between the dimensions
 *         of the source and the effective dimension of the target.  This
 *         will never be right until then
 */
	if (*pdims == NULL)
	   *pdims = stck[n].dims;

	nbl = _PD_block_index_deref(stck, n, tab, type,
				    &iloc, &addr, pnumb);};

    if (stck[n-1].cmmnd == DEREF_C)
       _PD_block_free(stck[n].blocks);

    stck[n].blocks      = nbl;
    stck[n].ad.diskaddr = addr;
    stck[n].indir_info  = iloc;

    return(addr);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_PTR_MEMBER_DEREF - find the member where a pointered type was just
 *                      - dereferenced this will mean skipping over itags
 *                      - and other pointees
 */

static int64_t _PD_ptr_member_deref(PD_smp_state *pa, int n)
   {int64_t addr;
    parse_frame *fr;
    locator *stck;
    PDBfile *file;

    file = FILE_S;
    fr   = FRAME_OBJ + FRAME_N;
    stck = fr->stack;

/* handle in memory members */
    if (file->virtual_internal)
       addr = stck[n].ad.diskaddr;

/* handle file members */
    else
       {addr  = stck[n-1].ad.diskaddr;
	addr += stck[n].ad.diskaddr;};

    stck[n].ad.diskaddr = addr;

    return(addr);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_PTR_REDUCE - reduce the parse tree
 *                - this means looping over the locator stack thru the
 *                - latest GOTO command and determining a new locator
 *                - whose intype, dimensions, number, and address can
 *                - be used to create an valid effective symbol table entry
 *                - or an actual one
 *                - if there is an intermediate expression on the stack
 *                - it will be read and the value (which can only be an
 *                - index) returned
 */

static long _PD_ptr_reduce(PD_smp_state *pa)
   {int i, nmn, nmx, cmnd;
    long numb;
    int64_t addr; 
    long val;
    char *type;
    dimdes *dims;
    SC_array *bl;
    symindir iloc;
    parse_frame *fr;
    locator *stck;
    PDBfile *file;

    file = FILE_S;
    fr   = FRAME_OBJ + FRAME_N;
    stck = fr->stack;

    val = 0L;
    nmx = FRAME(n);

    type = CURRENT(intype);
    numb = CURRENT(number);
    dims = CURRENT(dims);

/* find the most recent GOTO commmand */
    for (i = nmx; i > 0; i--)
        {cmnd = stck[i].cmmnd;
         if (cmnd == GOTO_C)
            break;};

    nmn  = max(i, 1);
    addr = 0;

    iloc.addr       = 0;
    iloc.n_ind_type = 0L;
    iloc.arr_offs   = 0L;

/* find the actual address of the specified object */
    if (file->virtual_internal)
       addr = stck[nmx].ad.diskaddr;

    else
       {for (i = nmn; i <= nmx; i++)
	    {cmnd = stck[i].cmmnd;
	     stck[i].disposable = FALSE;
	     dstcktr(cmnd, i, addr, numb, 0);

	     if (cmnd == DEREF_C)
	        {addr = _PD_ptr_deref_addr(pa, i);
		 numb = stck[i].number;}
	     else if (cmnd == INDEX_C)
	        {addr = _PD_ptr_index_deref(pa, i, &dims, &numb);
		 iloc = stck[i].indir_info;}
	     else if (cmnd == MEMBER_C)
	        {addr = _PD_ptr_member_deref(pa, i);
		 numb = stck[i].number;}
	     else if (cmnd != CAST_C)
	        {addr += stck[i].ad.diskaddr;
		 stck[i].ad.diskaddr = addr;};

	     dstcktr(cmnd, i, addr, numb, 1);};};

/* this must be taken now because the address reduction may have
 * changed the original
 */
    bl = CURRENT(blocks);

    _PD_rl_frame_blocks(pa, nmn, nmx, FRAME(n));

    FRAME(n) = nmn;

/* if we are not at the bottom of the locator stack we have
 * and intermediate expression which must by read in via _PD_sys_read
 */
    if (nmn != 1)
       {syment *ep;

        if (numb != 1L)
           PD_error("INTERMEDIATE MUST BE SCALAR INTEGER - _PD_PTR_REDUCE",
                    PD_TRACE);

        ep = _PD_mk_syment(CURRENT(intype), 1L, addr, NULL, NULL);
	_PD_sys_read(file, ep, SC_LONG_S, &val);
        _PD_rl_syment(ep);

        FRAME(n)--;}

/* otherwise we are at the end of the locator stack and the necessary
 * information to build an effective syment must be filled in the
 * bottom most locator
 */
    else
       {/* dpstack(pa, nmx); */

        strcpy(CURRENT(intype), type);

        CURRENT(number)      = numb;
        CURRENT(ad.diskaddr) = addr;
        CURRENT(blocks)      = bl;
	CURRENT(dims)        = dims;
	CURRENT(indir_info)  = iloc;
        CURRENT(cmmnd)       = RESULT_C;};

    return(val);}

/*---------------------------------------------------------------------------*/

/*                               PARSE ROUTINES                              */

/*---------------------------------------------------------------------------*/

/* _PD_SHIFT - perform a SHIFT operation by
 *           - pushing the input info onto the stack
 */

static void _PD_shift(PD_smp_state *pa, char *name, char *type,
		      dimdes *dims, SC_array *bl, long numb,
		      int64_t addr, int indr, int cmmnd)
   {

    if (type[0] == '\0')
       PD_error("NO TYPE SPECIFIED - _PD_SHIFT", PD_TRACE);

    if (FRAME_FRAME == NULL)
       {FRAME_N     = 0;
        FRAME_NX    = 2;
        FRAME_FRAME = CMAKE_N(parse_frame, FRAME_NX);};

    FRAME(n)++;
    if (FRAME(n) >= FRAME(nx))
       {FRAME(nx) += 10;
        CREMAKE(FRAME(stack), locator, FRAME(nx));};

    SC_MEM_INIT(locator, FRAME(stack)+FRAME(n));

    strcpy(CURRENT(intype), type);

    CURRENT(number)      = numb;
    CURRENT(ad.diskaddr) = addr;
    CURRENT(indirect)    = indr;
    CURRENT(dims)        = dims;
    CURRENT(blocks)      = bl;
    CURRENT(cmmnd)       = cmmnd;

    return;}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_DO_GOTO - carry out a GOTO command
 *             - this should be starting out with something which is
 *             - in the symbol table (it is an error if not)
 */

static void _PD_do_goto(PD_smp_state *pa, char *name)
   {char *type;
    int indr;
    long numb;
    int64_t addr;
    dimdes *dims;
    SC_array *bl;
    syment *ep;
    defstr *dp;
    PDBfile *file;

    file = FILE_S;

    ep = PD_inquire_entry(file, name, FRAME(flag), FRAME(path));
    if (ep == NULL)
       PD_error("NON-EXISTENT ENTRY - _PD_DO_GOTO", PD_TRACE);

    else

/* shift the starting point information onto the locator stack */
       {numb = PD_entry_number(ep);
	addr = PD_entry_address(ep);
	type = PD_entry_type(ep);
	dims = PD_entry_dimensions(ep);
	bl   = ep->blocks;

	dp = _PD_type_lookup(file, PD_CHART_FILE, type);
	if (dp == NULL)
	   PD_error("UNDEFINED TYPE - _PD_DO_GOTO", PD_TRACE);

	else if ((dp->size_bits != 0) && (addr > 0))
	   addr *= -SC_BITS_BYTE;

/* indirect does NOT mean that the type is indirect but that the
 * entry in the symbol table refers to a dynamically allocated
 * quantity, hence indirect means no dimensions
 * 
 */
	indr = (dims == NULL);

	_PD_shift(pa, name, type, dims, bl, numb, addr, indr, GOTO_C);};

    return;}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_IS_MEMBER - determine whether or not the given member is the
 *               - named member and return TRUE iff it is
 *               - also return the updated number of
 *               - struct indirections to track via the arg list
 */

static int _PD_is_member(char *name, memdes *desc, hasharr *tab, long *pns)
   {int rv;

/* if this is the member say so */
    if (strcmp(desc->name, name) == 0)
       rv = TRUE;

/* count up the number of indirects in the structure which will be skipped */
    else
       {if (_PD_indirection(desc->type))
           *pns += _PD_member_items(desc->member);

        rv = FALSE;};

    return(rv);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_GET_TYPE_MEMBER - get the true type of the member
 *                     - handle any casts
 */

static char *_PD_get_type_member(PDBfile *file, PD_smp_state *pa,
				 char *path_name, char *name,
				 memdes *desc, defstr **pdp)
   {char *mtype;
    hasharr *tab;

    if (file->virtual_internal)
       tab = file->host_chart;
    else
       tab = file->chart;

    *pdp = PD_inquire_table_type(tab, desc->base_type);
    if (*pdp == NULL)
       PD_error("UNDEFINED TYPE - _PD_GET_TYPE_MEMBER", PD_TRACE);

    if (desc->cast_offs < 0L)
       mtype = desc->type;
    else
       {if (file->virtual_internal)
           {SC_address ad;

            ad    = FRAME(stack)[FRAME(n)].ad;
	    mtype = DEREF(ad.memaddr + desc->cast_offs);
	    if (mtype == NULL)
               {if (DEREF(ad.memaddr + desc->member_offs) == NULL)
		   mtype = desc->type;
		else
		   PD_error("NULL CAST TO NON-NULL MEMBER - _PD_GET_TYPE_MEMBER",
			    PD_TRACE);};}

        else
           {char s[MAXLINE], c;
            int i;

/* build the path of the member which points to the real type */
	    strcpy(s, path_name);
            for (i = strlen(s) - 1; i >= 0; i--)
	        {c = s[i];
		 if ((c == '>') || (c == '.'))
		    break;}
	    s[i+1] = '\0';
	    strcat(s, desc->cast_memb);

            _PD_save_stack(pa);

/* read the real type in */
            PD_read(file, s, &mtype);
	    if (mtype == NULL)
	       mtype = desc->type;

            _PD_restore_stack(pa);};};

    return(mtype);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_DO_DEREF - carry out a DEREF command */

static void _PD_do_deref(PD_smp_state *pa)
   {int64_t addr;
    char t[MAXLINE];
    PDBfile *file;

    file = FILE_S;

    strcpy(t, CURRENT(intype));

    if (file->virtual_internal)
       {SC_address ad;

        ad         = FRAME(stack)[FRAME(n)].ad;
	ad.memaddr = *(char **) ad.memaddr;
	addr       = ad.diskaddr;}

    else
       addr = 0;

    _PD_shift(pa, "", t, NULL, NULL, -1L, addr, 0, DEREF_C);

/* since the shift added a new one this will dereference the current locator */
    PD_dereference(CURRENT(intype));

    return;}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_DO_MEMBER - carry out a MEMBER command */

static void _PD_do_member(PD_smp_state *pa, char *name, int deref_flag)
   {char *type, t[MAXLINE];
    int indr;
    long numb, nsitems;
    int64_t addr;
    dimdes *dims;
    defstr *dp;
    memdes *desc, *nxt;
    hasharr *tab;
    PDBfile *file;

    file = FILE_S;

    if (file->virtual_internal)
       tab = file->host_chart;
    else
       tab = file->chart;

/* if we came here with the "->" syntax we will need to shift
 * a derefence onto the locator stack ahead of the member shift
 * also update the path while we're at it
 */
    if (deref_flag)
       {_PD_do_deref(pa);
        snprintf(t, MAXLINE, "%s->%s", FRAME(path), name);}
    else
       snprintf(t, MAXLINE, "%s.%s", FRAME(path), name);

    strcpy(FRAME(path), t);

/* NOTE: we had better be properly dereferenced at this point!!!!!!!
 * DO NOT IMAGINE THAT ANYTHING DIFFERENT CAN BE DONE!!!!!!
 */
    type = CURRENT(intype);
    if (_PD_indirection(type))
       PD_error("IMPROPERLY DEREFERENCED EXPRESSION - _PD_DO_MEMBER",
                PD_TRACE);

/* find the defstr whose members are to be searched */
    dp = PD_inquire_table_type(tab, type);
    if (dp == NULL)
       PD_error("UNKNOWN TYPE - _PD_DO_MEMBER", PD_TRACE);

    else
       {addr    = 0;
	nsitems = 0L;

/* loop over the members accumulating offset to the new address
 * and the number of indirect members which will have to
 * be skipped over
 */
	for (desc = dp->members; desc != NULL; desc = nxt)
	    {nxt = desc->next;
	     if (_PD_is_member(name, desc, tab, &nsitems))
	        {type = _PD_get_type_member(file, pa, FRAME(path),
					    name, desc, &dp);

		 addr = desc->member_offs;
		 dims = desc->dimensions;
		 numb = _PD_comp_num(dims);
		 indr = _PD_indirection(type);

		 if (file->virtual_internal)
		    {SC_address ad;

		     ad   = FRAME(stack)[FRAME(n)].ad;
		     addr = ad.diskaddr + desc->member_offs;};

/* shift the member onto the locator stack */
		 _PD_shift(pa, name, type, dims, NULL,
			   numb, addr, indr, MEMBER_C);
		 CURRENT(n_struct_ptr) = nsitems;

		 return;};};

	PD_error("UNKNOWN MEMBER - _PD_DO_MEMBER", PD_TRACE);};

    return;}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_DO_INDEX - carry out a INDEX command
 *              - this must always set the current location
 *              - to point to the first element indexed
 *              - if more than one element is referenced then that
 *              - information must be put into the locator for future
 *              - action
 */

static void _PD_do_index(PD_smp_state *pa, char *expr)
   {int indr;
    intb bpi;
    inti start, stop, step, numb, doff; 
    int64_t addr;
    char t[MAXLINE], s[MAXLINE];
    char *type, *tok, *pt;
    dimdes *dims;
    SC_array *bl;
    PDBfile *file;

    file  = FILE_S;
    start = 0;
    numb  = 0;
    indr  = 0;

/* update the path */
    snprintf(t, MAXLINE, "%s[%s]", FRAME(path), expr);
    strcpy(FRAME(path), t);

    dims = CURRENT(dims);
    type = CURRENT(intype);
    doff = PD_get_offset(file);

    if (dims != NULL)
       {SC_strncpy(t, MAXLINE, type, -1);
        PD_dereference(t);
        numb = _PD_hyper_number(file, expr, dims, &start);
        indr = FALSE;}

    else if (_PD_indirection(type))
       {_PD_do_deref(pa);

/* find the offset which will be the first part of the index expression
 * find the number of items requested
 */
	SC_strncpy(t, MAXLINE, expr, -1);
	tok = SC_firsttok(t, ",");

#if 0

/* GOTCHA: this should be made to work (it crashed the test suite)
 *         and will have to work if PDBLib is to handle the
 *         extended index expression syntax when the parse routine
 *         is regenerated
 */
        numb = _PD_parse_index_expr(tok, dims, &start, &stop, &step);
	if (numb < 1)
	   PD_error("BAD INDEX EXPRESSION - _PD_DO_INDEX", PD_TRACE);
	start -= doff;

#else

	SC_strncpy(s, MAXLINE, tok, -1);
	tok = SC_strtok(s, ":", pt);
	if (tok == NULL)
	   PD_error("BAD INDEX EXPRESSION - _PD_DO_INDEX", PD_TRACE);

	start = SC_stol(tok) - doff;

        tok = SC_strtok(NULL, ":", pt);
        if (tok == NULL)
           stop = start;
        else
           stop = SC_stol(tok) - doff;

        step = SC_stol(SC_strtok(NULL, ":", pt));
        if (step == 0L)
           step = 1L;

        numb = (stop - start)/step + 1;

#endif

        strcpy(t, CURRENT(intype));
        indr = TRUE;}

    else
       PD_error("CAN'T INDEX OBJECT - _PD_DO_INDEX", PD_TRACE);

    bpi = _PD_lookup_size(t, file->chart);

    if (file->virtual_internal)
       {SC_address ad;

        ad   = FRAME(stack)[FRAME(n)].ad;
	addr = ad.diskaddr;}

    else
       addr = 0;

    addr += start*bpi;

    bl = CURRENT(blocks);

    _PD_shift(pa, expr, t, dims, bl, numb, addr, indr, INDEX_C);

    CURRENT(n_array_items) = start;

    return;}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_DO_CAST - carry out a CAST command */

static void _PD_do_cast(PD_smp_state *pa, char *type)
   {int in;
    long n;
    int64_t da;
    char t[MAXLINE], s[MAXLINE];
    SC_array *bl;
    dimdes *dm;

/* update the path */
    snprintf(t, MAXLINE, "(%s) %s", type, FRAME(path));
    strcpy(FRAME(path), t);

    da = CURRENT(ad.diskaddr);
    in = CURRENT(indirect);
    n  = CURRENT(number);
    bl = CURRENT(blocks);
    dm = CURRENT(dims);

    strcpy(s, CURRENT(intype));

    _PD_shift(pa, "", s, dm, bl, n, da, in, CAST_C);

    strcpy(OUTTYPE, type);

    return;}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_DO_DIGRESS - carry out a DIGRESS command */

static long _PD_do_digress(PD_smp_state *pa, char *expr)
   {long val;
    char t[MAXLINE];
    PDBfile *file;

    file = FILE_S;

/* save the path
 * NOTE: this doesn't support more than one level of recursion!!
 */
    strcpy(t, FRAME(path));

    if (file->use_itags == TRUE)
       val = _PD_itag_reduce(pa);
    else
       val = _PD_ptr_reduce(pa);

/* restore the path */
    strcpy(FRAME(path), t);

    return(val);}

/*---------------------------------------------------------------------------*/

/*                           LEXICAL SCANNER ROUTINES                        */

/*---------------------------------------------------------------------------*/

/* _PD_NEXT_TOKEN - figure out whether the specified token is an
 *                - IDENTIFIER or an INTEGER
 *                - and take the apropriate action
 */

static int _PD_next_token(PD_smp_state *pa, int start)
   {int nc, rv;
    char *end, s[MAXLINE], *tok, *t;

    nc = FRAME(index) - start;
    SC_strncpy(s, MAXLINE, FRAME(lex_bf)+start, nc);

/* eliminate whitespace from either end of the token
 * NOTE: things like "a b" are illegal anyway
 */
    tok = SC_strtok(s, " \t\f\n\r", t);
    if (tok != NULL)
       {strcpy(TEXT, tok);

	NUM_VAL = SC_strtol(TEXT, &end, 0);
	tok     = TEXT + strlen(TEXT);
	rv      = (tok == end) ? INTEGER : IDENTIFIER;}
    else
       rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_LEX - lexical scanner called by the generated parser
 *         - return the value of the lexical token if successful
 *         - return 0 if at the end of the input string
 *         - text of identifiers is put in the global variable TEXT
 *         - the numerical value of an INTEGER token is put in the global
 *         - variable NUM_VAL
 *         - legal token values are:
 *         -     
 *         -     OPEN_PAREN       ( or [
 *         -     CLOSE_PAREN      ) or ]
 *         -     DOT              .
 *         -     COMMA            ,
 *         -     COLON            :
 *         -     STAR             *
 *         -     ARROW            ->
 *         -     INTEGER          octal, decimal, or hexidecimal integer
 *         -     IDENTIFIER       just about anything else (no white space)
 */

static int _PD_lex(PD_smp_state *pa)
   {int c, d, start, rv;

    start = FRAME(index);
    while (TRUE)
       {c = input();
	switch (c)
	   {case '\0' :
	         if (FRAME(index) == start+1)
		    {unput(c);
		     rv = 0;}
		 else
		    {unput(c);
		     rv = _PD_next_token(pa, start);};

		 return(rv);

	    case '(' :
	    case '[' :
	         GOT_TOKEN(OPEN_PAREN);

	    case ')' :
	    case ']' :
	         GOT_TOKEN(CLOSE_PAREN);

	    case '.' :
	         GOT_TOKEN(DOT);

	    case ',' :
	         GOT_TOKEN(COMMA);

	    case ':' :
	         GOT_TOKEN(COLON);

	    case '*' :
	        GOT_TOKEN(STAR);

	    case '-' :
	         d = input();
                 if (d == '>')
		    {if (FRAME(index) == start+2)
                        rv = ARROW;
		     else
		        {unput(d);
			 unput(c);
			 rv = _PD_next_token(pa, start);};

		     return(rv);};

	    default :
	         break;};};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PARSE_INDEX_EXPR - decipher things of the form
 *                      -   [n][:[n][:[n]]]
 *                      - and return a start, stop, step triple
 */

inti _PD_parse_index_expr(char *expr, dimdes *dim, inti *pstart,
			  inti *pstop, inti *pstep)
   {int i, j, tv, nc;
    int rv;
    long tr[3];
    PD_smp_state *pa;
    SC_THREAD_ID(tid);

    if (strlen(expr) == 0)
       return(0L);

    pa = _PD_get_state(tid);

    FRAME_N  = 0;
    FRAME_NX = 2;
    FRAME_FRAME   = CMAKE_N(parse_frame, FRAME_NX);

    FRAME(lex_bf) = CSTRSAVE(expr);
    FRAME(index)  = 0;
   
/* count the number of colons in the expression */
    nc = 0;

    tr[2] = 1;

    for (j = 0, i = 0; i < 5; i++)
        {tv = _PD_lex(pa);
	 if (tv == COLON)
            {nc += 1;
	     if (j == 0)
	        {if (dim != NULL)
		    tr[j++] = dim->index_min;
		 else
		    return(0L);}
	     else if (j == 1)
	        {if (nc == 2)
		    {if (dim != NULL)
		        tr[j++] = dim->index_max;
		     else
		        return(0L);};};}
	 else if (tv == INTEGER) 
	    tr[j++] = NUM_VAL;
         else
	    break;};

    if (j == 1)
       {if (nc == 0)
	   tr[1] = tr[0];
        else if (nc == 1)
	   {if (dim != NULL)
	       tr[1] = dim->index_max;
	    else
	       return(0L);}
	else
	   return(0L);};

    *pstart = tr[0];
    *pstop  = tr[1];
    *pstep  = tr[2];

    _PD_rl_frames(pa);

    rv = (tr[1] - tr[0])/tr[2] + 1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DISP_RULES - dispatch on the specified rule */

static void _PD_disp_rules(PD_smp_state *pa, int rule, char **pvt)
   {

    switch (rule)

/* variable_expression :
 *      unary_expression
 *    | OPEN_PAREN type CLOSE_PAREN variable_expression
 */
       {case 2:
	     _PD_do_cast(pa, pvt[-2]);
	     break;

/*    | */
	case 3:
/*	     PD_error("EMPTY UNARY EXPRESSION - _PD_DISP_RULES", PD_TRACE); */
	     break;

/* type :
 *      IDENTIFIER
 */
	case 4:
	     FRAME(val) = TEXT;
	     break;

/*    | type STAR */
	case 5:
	     snprintf(MSG, MAXLINE, "%s *", pvt[-1]);
	     FRAME(val) = MSG;
	     break;

/* unary_expression :
 *      postfix_expression
 *    | STAR variable_expression
 */
	case 7:
	     _PD_do_deref(pa);
	     break;

/* postfix_expression :
 *      primary_expression
 */
	case 8:
	     _PD_do_goto(pa, pvt[-0]);
	     break;

/*    | postfix_expression OPEN_PAREN index_expression CLOSE_PAREN */
	case 9:
	     _PD_do_index(pa, pvt[-1]);
	     CFREE(pvt[-1]);
	     break;

/*    | postfix_expression DOT primary_expression */
	case 10:
	     _PD_do_member(pa, pvt[-0], FALSE);
	     break;

/*    | postfix_expression ARROW primary_expression */
	case 11:
	     _PD_do_member(pa, pvt[-0], TRUE);
	     break;

/* index_expression :
 *         range
 *       | index_expression COMMA range
 */
	case 13:
	     snprintf(MSG, MAXLINE, "%s,%s", pvt[-2], pvt[-0]);
	     CFREE(pvt[-2]);
	     CFREE(pvt[-0]);
	     FRAME(val) = CSTRSAVE(MSG);
	     break;

/* range : index
 *       | index COLON index
 */
	case 15:
	     if (strcmp(pvt[-2], pvt[-0]) != 0)
	        HAVE_COLON = TRUE;
	     snprintf(MSG, MAXLINE, "%s:%s", pvt[-2], pvt[-0]);
	     CFREE(pvt[-2]);
	     CFREE(pvt[-0]);
	     FRAME(val) = CSTRSAVE(MSG);
	     break;

/*       | index COLON index COLON index */
	case 16:
	     if (strcmp(pvt[-4], pvt[-2]) != 0)
	        HAVE_COLON = TRUE;
	     snprintf(MSG, MAXLINE, "%s:%s:%s", pvt[-4], pvt[-2], pvt[-0]);
	     CFREE(pvt[-4]);
	     CFREE(pvt[-2]);
	     CFREE(pvt[-0]);
	     FRAME(val) = CSTRSAVE(MSG);
	     break;

/* index : INTEGER */
	case 17:
	     snprintf(MSG, MAXLINE, "%ld", NUM_VAL);
	     FRAME(val) = CSTRSAVE(MSG);
	     break;

/*       | variable_expression */
	case 18:
	     snprintf(MSG, MAXLINE, "%ld", _PD_do_digress(pa, pvt[-0]));
	     FRAME(val) = CSTRSAVE(MSG);
	     break;

/* primary_expression : IDENTIFIER */
	case 19:
	     if (HAVE_COLON)
	        PD_error("HYPERINDEX ON NON-TERMINAL NODE - _PD_DISP_RULES",
			 PD_TRACE);
	     FRAME(val) = TEXT;
	     break;};

    return;}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _PD_PARSE - parse an expression which is in the lexical buffer of
 *           - the current parse frame
 *           - return TRUE if there are no syntax errors
 *           - return FALSE otherwise
 */

static void _PD_parse(PD_smp_state *pa)
   {char **pvt;
    register char **lpv;   	/* top of value stack */
    register int *lps;	        /* top of state stack */
    register int lstate;	/* current state */
    register int  n;	        /* internal state number info */
    register int len;

    static int
      exca[] = {-1, 1, 0, -1, -2, 0,},
      act[]  = { 3, 29,  5, 26, 24,  7,  7,  9, 19, 25,
                 3, 18,  5, 10, 17,  7, 11, 12, 14, 15,
                20,  1, 16,  4,  6,  8, 13,  2,  0,  0,
                 0,  0,  0,  0,  0, 23, 21, 22, 28,  0,
                27, 30},
      pact[] = { -247, -1000, -1000,  -255,  -244,  -247,
                -1000, -1000,  -240, -1000,
                 -257,  -256,  -256, -1000,  -247, -1000,
                 -254, -1000,  -261, -1000,
                -1000, -1000, -1000, -1000, -1000,  -257,
                 -257, -1000,  -263,  -257, -1000},
      pgo[] = {0, 20, 27, 25, 23, 24, 22, 14, 11},
      r1[]  = {0, 1, 1, 1, 3, 3, 2, 2, 4, 4,
               4, 4, 6, 6, 7, 7, 7, 8, 8, 5},
      r2[]  = {0, 2, 9, 1, 3, 5,  2, 5, 3, 9,
               7, 7, 2, 7, 2, 7, 11, 3, 3, 3},
      chk[] = {-1000,  -1,  -2, 257,  -4, 259,  -5, 262,  -3, 262,
                 257, 260, 261,  -1, 258, 259,  -6,  -7,  -8, 265,
                  -1,  -5,  -5,  -1, 258, 263, 264,  -7,  -8, 264, -8},
      def[] = { 3, -2,  1, 0, 6, 3, 8, 19,  0,  4,
                3,  0,  0, 7, 3, 5, 0, 12, 14, 17,
               18, 10, 11, 2, 9, 3, 3, 13, 15,  3, 16};

/* initialize externals - _PD_parse may be called more than once */
    FRAME(pv) = &FRAME(v)[0] - 1;
    FRAME(ps) = &FRAME(s)[0] - 1;

    FRAME(state)         = 0;
    FRAME(tmp)           = 0;
    FRAME(n_error)       = 0;
    FRAME(error)         = 0;
    FRAME(current_token) = -1;

    lpv    = FRAME(pv);
    lps    = FRAME(ps);
    lstate = FRAME(state);

    HAVE_COLON = FALSE;

/* loop as expressions are pushed onto the stack */
    for (;;)

/* put a state and value onto the stacks */
        {if (++lps >= &FRAME(s)[MAXPARSEDEPTH])
	    PD_error("STACK OVERFLOW - _PD_PARSE", PD_TRACE);

	 *lps   = lstate;
	 *++lpv = FRAME(val);

/* we have a new state - find out what to do */
	 n = pact[lstate];
	 if (n > STATEFLAG)
	    {if ((FRAME(current_token) < 0) &&
		 ((FRAME(current_token) = _PD_lex(pa)) < 0))
	        FRAME(current_token) = 0;

/* valid shift */
	     n += FRAME(current_token);
	     if ((n >= 0) && (n < LASTTOK))
	        {n = act[n];
		 if (chk[n] == FRAME(current_token))
		    {FRAME(current_token) = -1;
		     FRAME(val) = FRAME(lval);

		     lstate = n;
		     if (FRAME(error) > 0)
		        FRAME(error)--;
		     continue;};};};

	 n = def[lstate];
	 if (n == -2)
	    {int *xi;

	     if ((FRAME(current_token) < 0) &&
		 ((FRAME(current_token) = _PD_lex(pa)) < 0))
	        FRAME(current_token) = 0;

/* look through exception table */
	     xi = exca;

	     while ((*xi != -1) || (xi[1] != lstate))
	        {xi += 2;};

	     while ((*(xi += 2) >= 0) && (*xi != FRAME(current_token)));

	     n = xi[1];
	     if (n < 0)
	        return;};

/* check for syntax error */
	 if (n == 0)
	    {if (FRAME(error) > 0)
	       PD_error("SYNTAX ERROR - _PD_PARSE", PD_TRACE);};

/* reduction by production n */
	 FRAME(tmp) = n;		/* value to switch over */
	 pvt = lpv;			/* top of value stack */

/* look in goto table for next state
 * if r2[n] doesn't have the low order bit set
 * then there is no action to be done for this reduction
 * and no saving/unsaving of registers done
 */
	 len = r2[n];
	 if (!(len & 01))
	    {len >>= 1;
	     lpv -= len;
	     FRAME(val) = lpv[1];

	     n = r1[n];
	     lps -= len;
	     lstate = pgo[n] + *lps + 1;
	     if ((lstate >= LASTTOK) ||
		 (chk[lstate = act[lstate]] != -n))
	        {lstate = act[pgo[n]];};

	     continue;};

	 len >>= 1;
	 lpv -= len;
	 FRAME(val) = lpv[1];

	 n   = r1[n];
	 lps -= len;
	 lstate = pgo[n] + *lps + 1;
	 
	 if ((lstate >= LASTTOK) ||
	     (chk[lstate = act[lstate]] != -n))
	    {lstate = act[pgo[n]];};

/* save until reenter driver code */
	 FRAME(state) = lstate;
	 FRAME(ps)    = lps;
	 FRAME(pv)    = lpv;

	 _PD_disp_rules(pa, FRAME(tmp), pvt);

	 lpv    = FRAME(pv);
	 lps    = FRAME(ps);
	 lstate = FRAME(state);};}

/*---------------------------------------------------------------------------*/

/*                            EXTERNAL ROUTINES                              */

/*---------------------------------------------------------------------------*/

/* _PD_EFFECTIVE_EP - look up the symbol table entry for the named quantity
 *                  - return an effective symbol table entry which contains
 *                  - the type and dimensions of the entire variable(!) and
 *                  - the disk address and number of items referred to by the
 *                  - hyper-index expression, if any
 *                  - if name contains such a specification
 *                  - the returned syment will be newly allocated
 *                  - return NULL iff there is an error
 *
 */

syment *_PD_effective_ep(PDBfile *file, char *name, int flag, char *fullname)
   {int alloc_frames;
    dimdes *dims;
    char *s, *type, *lname, bf[MAXLINE];
    long ni;
    int64_t addr;
    symindir indr;
    SC_array *bl;
    syment *ep;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

/* to improve performance and to accomodate certain unusual variable names
 * such as domain names, see if the variable name is literally in the file
 */
    ep = PD_inquire_entry(file, name, flag, fullname);
    if (ep != NULL)
       return(PD_copy_syment(ep));

    lname = _PD_var_namef(NULL, name, bf, MAXLINE);

/* get the top level struct to start */
    s = SC_strstr(lname, "->");
    if (s != NULL)
       *s = '\0';
	
#ifdef USE_REQUESTS
/*
    CFREE(file->req.base_name);
    file->req.base_name = CSTRSAVE(lname);
*/
#endif

    ep = PD_inquire_entry(file, lname, flag, fullname);
    if ((ep == NULL) ||
	((PD_entry_dimensions(ep) == NULL) && (ep->blocks == NULL)))
       return(ep);

    alloc_frames = FALSE;
    if (FRAME_FRAME == NULL)
       {alloc_frames = TRUE;

	FRAME_N      = 0;
        FRAME_NX     = 4;
        FRAME_FRAME  = CMAKE_N(parse_frame, FRAME_NX);
        FRAME(stack) = NULL;
        FRAME(nx)    = 0;};

    FRAME(lex_bf) = CSTRSAVE(name);
    FRAME(index)  = 0;

    FRAME(n) = 0L;
    if (FRAME(stack) == NULL)
       {FRAME(nx)   += 10;
	FRAME(stack) = CMAKE_N(locator, 10);};

    switch (SETJMP(pa->trace_err))
       {case ABORT :
	     if ((fullname != NULL) && flag)
	        strcpy(fullname, name);
	     if (alloc_frames)
	        _PD_rl_frames(pa);
             return(NULL);

        case ERR_FREE :
	     if (alloc_frames)
	        _PD_rl_frames(pa);
             return(NULL);

        default :
	     memset(pa->err, 0, MAXLINE);
	     break;};

/* copy these arguments into global (file static) variables */
    FILE_S      = file;
    FRAME(flag) = flag;

    _PD_parse(pa);

    if (file->use_itags == TRUE)
       _PD_itag_reduce(pa);
    else
       _PD_ptr_reduce(pa);

    dims = CURRENT(dims);
    type = CURRENT(intype);
    ni   = CURRENT(number);
    indr = CURRENT(indir_info);
    addr = CURRENT(ad).diskaddr;
    bl   = CURRENT(blocks);

/*    dpstack(FRAME(pa, n)); */

    ep = _PD_mk_syment(type, ni, addr, &indr, dims);

    if ((file->virtual_internal == FALSE) && (bl != NULL))
       _PD_block_switch(ep, bl);

    if (fullname != NULL)
       strcpy(fullname, FRAME(path));

    if (alloc_frames)
       _PD_rl_frames(pa);

    return(ep);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
