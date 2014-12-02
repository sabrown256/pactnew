/*
 * PDBMM.C - memory management for the PDB library system
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"
#include "data_formats.h"

PD_scope_public
 PD_gs = { 0, 8, 2, -1, HOST_ORDER,

/* print controls
 *  0  -  print prefix: 0 = full path, 1 = space, 2 = tree
 *  1  -  0 = print name, 1 = print type and name
 *  2  -  recursion: 0 = yes, 1 = count
 *  3  -  number of items before going to array mode of display
 *  4  -  number of items per line
 *  5  -  print variable label: 0 = full, 1 = partial, 2 = nothing
 */
           {0L, 0L, 0L, 20L, 2L, 0L, 0L, 0L, 0L, 0L},
           "",
           { "PDBfile", NULL, NULL, NULL },
	   NULL, NULL, NULL,

/* data_standards */
           &INT_STD, NULL,

/* the array of data_standard definitions */
           { DEF_TEXT_STD,
             DEF_I386_STD,
	     DEF_I586L_STD,
	     DEF_I586O_STD,
	     DEF_PPC32_STD,
             DEF_X86_64_STD,
	     DEF_X86_64A_STD,
	     DEF_PPC64_STD,
             DEF_M68X_STD,
	     DEF_VAX_STD,
	     DEF_CRAY_STD },

/* data_alignments */
	   &INT_ALG, NULL, 

/* the array of data_alignment definitions */
           { DEF_TEXT_ALIGN,
	     DEF_BYTE_ALIGN,
	     DEF_WORD2_ALIGN,
	     DEF_WORD4_ALIGN,
	     DEF_WORD8_ALIGN,
	     DEF_GNU4_I686_ALIGN,
	     DEF_OSX_10_5_ALIGN,
	     DEF_SPARC_ALIGN,
	     DEF_XLC32_PPC64_ALIGN,
	     DEF_CYGWIN_I686_ALIGN,
	     DEF_GNU3_PPC64_ALIGN,
	     DEF_GNU4_PPC64_ALIGN,
	     DEF_XLC64_PPC64_ALIGN,
	     DEF_GNU4_X86_64_ALIGN,
	     DEF_PGI_X86_64_ALIGN },

/* NOTE: these next two members are deprecated - do not use */

/* the array of data_standard accesses */
           { &X86_64_STD,                 /* 1 */
             &I586L_STD,
             &I586O_STD,
             &I386_STD,
             &PPC64_STD,                  /* 5 */
             &PPC32_STD,
             &M68X_STD,
             &VAX_STD,
             &CRAY_STD,
             NULL },

/* the array of data_alignment accesses */
           { &BYTE_ALIGNMENT,            /* 1 */
             &WORD2_ALIGNMENT,
             &WORD4_ALIGNMENT,
             &WORD8_ALIGNMENT,
             &GNU4_X86_64_ALIGNMENT,     /* 5 */
             &GNU4_I686_ALIGNMENT,
             &CYGWIN_I686_ALIGNMENT,
             &GNU3_PPC64_ALIGNMENT,
             &XLC64_PPC64_ALIGNMENT,
             &XLC32_PPC64_ALIGNMENT,     /* 10 */
             &OSX_10_5_ALIGNMENT,
             &SPARC_ALIGNMENT,
             NULL },

         };

PD_scope_private
 _PD = { -1, 50L,
         -1, -1, FALSE, -1, FALSE,
         FALSE,
	 { (SC_communicator) 1, (SC_communicator) 2, (SC_communicator) 3},
	 { (SC_communicator) 0, (SC_communicator) 0, (SC_communicator) 0},
       };

static sys_layer
 _PD_sys = {_PD_rd_syment, _PD_wr_syment};

static tr_layer
 _PD_tr = {"PDBfile", "pdb", _PD_pdbfilep,
	   NULL, NULL, _PD_close,
	   _PD_read, _PD_write};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MK_PDB - THREADSAFE 
 *            - construct and return a pointer to a PDBFile
 */

PDBfile *_PD_mk_pdb(SC_udl *pu, char *name, char *md, int reg,
		    sys_layer *sys, tr_layer *tr)
   {FILE *fp;
    PDBfile *file;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    if (_PD.maxfsize == 0)
       SC_fix_lmt(sizeof(int64_t), NULL, &_PD.maxfsize, NULL);

    file = CMAKE(PDBfile);
    if (file != NULL)
       {if (sys == NULL)
	   sys = &_PD_sys;

	if (tr == NULL)
	   tr = &_PD_tr;

	fp = NULL;
	if (pu != NULL)
	   {fp = pu->stream;

	    if (name == NULL)
	       name = pu->udl;

	    if (md == NULL)
	       md = pu->mode;};

	file->udl        = pu;
	file->stream     = fp;
	file->name       = CSTRSAVE(name);
	file->type       = NULL;

	file->symtab     = SC_make_hasharr(_PD.ninc, NODOC, SC_HA_NAME_KEY, 0);
	file->chart      = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY, 0);
	file->host_chart = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY, 0);
	file->attrtab    = NULL;
	file->mode       = PD_UNINIT;
	file->file_mode  = CSTRSAVE(md);

	file->maximum_size     = _PD.maxfsize;                /* family info */
	file->previous_file    = NULL;

	file->virtual_internal = FALSE;              /* disk file by default */
	file->current_prefix   = NULL;    /* read/write variable name prefix */
	file->ptr_base         = CSTRSAVE("/&ptrs/ia_");

	file->default_offset = 0;        /* default offset for array indexes */
	file->major_order    = ROW_MAJOR_ORDER;

	file->track_pointers = TRUE;
	file->delay_sym      = PD_DELAY_NONE;

	file->std        = NULL;
	file->align      = NULL;
	file->host_std   = _PD_copy_standard(pa->int_std);
	file->host_align = _PD_copy_alignment(pa->int_align);

	file->n_dyn_spaces = 0L;

	file->mpi_mode = 0;                        /* 1: serial, 0: parallel */
	file->mpi_file = FALSE;

	file->req.oper      = PD_OPEN;
	file->req.base_name = NULL;
	file->req.base_type = NULL;
	file->req.ftype     = NULL;
	file->req.htype     = NULL;

	file->block_chain  = NULL;
	file->file_chain   = _PD.file_chain;
	_PD.file_chain     = NULL;

	file->symtaddr = 0;
	file->chrtaddr = 0;
	file->headaddr = 0;
	file->bfsz     = 0;

	file->cksum.verified = FALSE;
	file->cksum.use      = PD_MD5_OFF;
	file->cksum.file     = PD_MD5_OFF;

	file->fix_denorm = FALSE;
	file->delim      = NULL;
        file->radix      = 10;

	file->sys            = sys;
	file->tr             = tr;
	file->date           = SC_date();
	file->system_version = PDB_SYSTEM_VERSION;

	file->symatch        = NULL;

	_PD_MARK_AS_FLUSHED(file, FALSE);

/* set the default format version for the file */
	_PD_format_version(file, -1);

	SC_mark(file->name, 1);
	SC_mark(file->symtab, 1);
	SC_mark(file->chart, 1);
	SC_mark(file->host_chart, 1);

/* initialize the pointer list */
	_PD_ptr_init_apl(file);

/* register the file with the parallel file manager */
	if (reg == TRUE)
	   _PD_ADD_FILE(file, (int64_t) 0);

	file->host_std->file = file;};

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RL_PDB - THREADSAFE
 *            - release the storage associated with the PDBfile
 */

void _PD_rl_pdb(PDBfile *file)
   {

    CFREE(file->date);
    CFREE(file->ptr_base);

    _SC_rel_udl(file->udl);

    _PD_ptr_free_apl(file);

    PD_rel_attr_table(file);

    _PD_clr_table(file->host_chart, _PD_ha_rl_defstr);
    _PD_clr_table(file->chart, _PD_ha_rl_defstr);
    _PD_clr_table(file->symtab, _PD_ha_rl_syment);

    _PD_rl_standard(file->std);
    _PD_rl_standard(file->host_std);
    _PD_rl_alignment(file->align);
    _PD_rl_alignment(file->host_align);

    if (file->previous_file != NULL)
       CFREE(file->previous_file);

    if (file->type != NULL)
       CFREE(file->type);

    CFREE(file->current_prefix);
    CFREE(file->file_mode);
    CFREE(file->name);
    CFREE(file);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_ALLOC_ENTRY - allocate memory for the variable type
 *
 * #bind PD_alloc_entry fortran() scheme() python()
 */

void *PD_alloc_entry(PDBfile *file ARG(,,cls), char *name)
   {syment *ep;
    void *ret;

    ep = _PD_effective_ep(file, name, TRUE, NULL);
    if (ep == NULL)
       ret = NULL;
    else
       {ret = _PD_alloc_entry(file, PD_entry_type(ep), PD_entry_number(ep));
	_PD_rl_syment_d(ep);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_ALLOC_ENTRY - allocate memory for the variable type */

void *_PD_alloc_entry(PDBfile *file, char *type, inti ni)
   {int ipt;
    inti len;
    intb bpi;
    void *vr;
    defstr *dp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    if (_PD_indirection(type))
       vr = (char *) CMAKE_N(char *, ni);
    else
       {dp = PD_inquire_host_type(file, type);
        if (dp == NULL)
           {snprintf(pa->err, MAXLINE,
		     "BAD TYPE %s - _PD_ALLOC_ENTRY", type);
            return(NULL);};

/* add extra space to chars - one for '\0' and one in case SC_firsttok
 * is applied to this string
 */
        if (strcmp(type, SC_CHAR_S) == 0)
           ni += 2;

	ipt = _PD_items_per_tuple(dp);
        bpi = dp->size;
        len = ni*ipt*bpi;
        vr  = CMAKE_N(char, len);};

    return(vr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CLR_TABLE - THREADSAFE
 *               - release the storage associated with a homogeneous
 *               - hash table
 */

void _PD_clr_table(hasharr *tab, int (*rel)(haelem *p, void *a))
   {

/*    SC_hasharr_clear(tab, rel, NULL); */
    SC_free_hasharr(tab, rel, NULL);

    return;}    

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MK_STANDARD - THREADSAFE
 *                 - allocate, initialize and return a pointer to a
 *                 - data_standard
 */

data_standard *_PD_mk_standard(PDBfile *file)
   {int i;
    data_standard *std;

    std = CMAKE(data_standard);

    std->bits_byte         = BITS_DEFAULT;
    std->ptr_bytes         = 0;
    std->bool_bytes        = 0;

    for (i = 0; i < N_PRIMITIVE_CHAR; i++)
        {std->chr[i].bpi = 0;
	 std->chr[i].utf = UTF_NONE;};

    for (i = 0; i < N_PRIMITIVE_FIX; i++)
        {std->fx[i].bpi   = 0;
	 std->fx[i].order = NO_ORDER;};

    for (i = 0; i < N_PRIMITIVE_FP; i++)
        {std->fp[i].bpi    = 0;
	 std->fp[i].order  = NULL;
	 std->fp[i].format = NULL;};

    std->file = file;

    return(std);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_COPY_STANDARD - THREADSAFE
 *                   - return a copy of the given data_standard
 */

data_standard *_PD_copy_standard(data_standard *src)
   {int i, j, n;
    int *ostd, *osrc;
    long *fstd, *fsrc;
    data_standard *std;

    std = CMAKE(data_standard);

    std->bits_byte  = src->bits_byte;
    std->ptr_bytes  = src->ptr_bytes;
    std->bool_bytes = src->bool_bytes;

    for (i = 0; i < N_PRIMITIVE_CHAR; i++)
        {std->chr[i].bpi = src->chr[i].bpi;
	 std->chr[i].utf = src->chr[i].utf;};

    for (i = 0; i < N_PRIMITIVE_FIX; i++)
        {std->fx[i].bpi   = src->fx[i].bpi;
	 std->fx[i].order = src->fx[i].order;};

    for (i = 0; i < N_PRIMITIVE_FP; i++)
        {std->fp[i].bpi = src->fp[i].bpi;

	 n    = PD_gs.format_fields;
         fstd = CMAKE_N(long, n);
	 SC_mark(fstd, 1);
	 std->fp[i].format = fstd;
	 fsrc = src->fp[i].format;
	 for (j = 0; j < n; j++, *(fstd++) = *(fsrc++));

	 n = std->fp[i].bpi;
	 n = (n > 0) ? n : -(n / std->bits_byte);
	 ostd = CMAKE_N(int,  n);
	 SC_mark(ostd, 1);
	 std->fp[i].order = ostd;
	 osrc = src->fp[i].order;
	 for (j = 0; j < n; j++, *(ostd++) = *(osrc++));};

    return(std);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RL_STANDARD - THREADSAFE
 *                 - release a data_standard
 */

void _PD_rl_standard(data_standard *std)
   {

    if (std != NULL)
       {CFREE(std->fp[0].format);
        CFREE(std->fp[0].order);
        CFREE(std->fp[1].format);
        CFREE(std->fp[1].order);
        CFREE(std->fp[2].format);
        CFREE(std->fp[2].order);

        CFREE(std);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_COPY_ALIGNMENT - THREADSAFE
 *                    - return a copy of a data_alignment
 */

data_alignment *_PD_copy_alignment(data_alignment *src)
   {data_alignment *align;

    align = CMAKE(data_alignment);

    *align = *src;

    return(align);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RL_ALIGNMENT - THREADSAFE
 *                  - release a data_alignment
 */

void _PD_rl_alignment(data_alignment *align)
   {if (SC_arrlen(align) > 0)
       {CFREE(align);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_COPY_DIMS - make and return a copy of the given dimension list
 *
 * #bind PD_copy_dims fortran() scheme() python()
 */

dimdes *PD_copy_dims(dimdes *odims)
   {dimdes *od, *ndims, *prev, *next;

    prev  = NULL;
    ndims = NULL;
    
    for (od = odims; od != NULL; od = od->next)
        {next  = CMAKE(dimdes);
         *next = *od;
	 next->next = NULL;

         if (ndims == NULL)
            ndims = next;
         else
	    {prev->next = next;
	     SC_mark(next, 1);};

         prev = next;};

    return(ndims);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_COPY_SYMENT - make and return a copy of the given syment
 *
 * #bind PD_copy_syment fortran() scheme() python()
 */

syment *PD_copy_syment(syment *osym)
   {char *ntype;
    syment *nsym;
    dimdes *ndims;

    if (osym == NULL)
       return(NULL);

    nsym = CMAKE(syment);

    ntype = CSTRSAVE(PD_entry_type(osym));
    ndims = PD_copy_dims(PD_entry_dimensions(osym));

    _PD_block_copy(nsym, osym);

    nsym->type       = ntype;
    nsym->dimensions = ndims;
    nsym->number     = PD_entry_number(osym);
    nsym->indirects  = osym->indirects;

    SC_mark(ntype, 1);
    SC_mark(ndims, 1);

    return(nsym);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MK_SYMENT - make and return a pointer to an entry
 *               - for the symbol table
 */

syment *_PD_mk_syment(char *type, inti ni, int64_t addr, symindir *indr,
		      dimdes *dims)
   {syment *ep;
    char *t;

    ep = CMAKE(syment);
    if (ep != NULL)
       {ep->blocks = NULL;

	_PD_block_init(ep, TRUE);
	_PD_entry_set_number(ep, 0, ni);
	_PD_entry_set_address(ep, 0, addr);

	if (type == NULL)
	   t = NULL;
	else
	   {t = CSTRSAVE(type);
	    SC_mark(t, 1);};

	ep->type       = t;
	ep->number     = ni;
	ep->dimensions = dims;

	if (indr == NULL)
	   {symindir iloc;
	    iloc.addr       = 0;
	    iloc.n_ind_type = 0L;
	    iloc.arr_offs   = 0L;
	    ep->indirects   = iloc;}

	else
	   ep->indirects = *indr;

	SC_mark(dims, 1);};

    return(ep);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RL_SYMENT_D - THREADSAFE
 *                 - reclaim the space of the given syment 
 *                 - including its dimensions
 */

void _PD_rl_syment_d(syment *ep)
   {

    if (ep != NULL)
       {_PD_rl_dimensions(PD_entry_dimensions(ep));
	ep->dimensions = NULL;
	_PD_rl_syment(ep);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_HA_RL_SYMENT - reclaim the space of the given syment in a hash array
 *                  - including its dimensions
 */

int _PD_ha_rl_syment(haelem *hp, void *a)
   {int ok;
    char *name;
    syment *ep;

    ok = SC_haelem_data(hp, &name, NULL, (void **) &ep, TRUE);
    SC_ASSERT(ok == TRUE);

    _PD_rl_syment_d(ep);

    CFREE(hp->name);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RL_SYMENT - THREADSAFE
 *               - reclaim the space of the given syment
 */

void _PD_rl_syment(syment *ep)
   {

    if (ep != NULL)
       {CFREE(ep->type);
	_PD_block_rel(ep);
	CFREE(ep);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MK_DEFSTR - make a defstr entry for the structure chart */

defstr *_PD_mk_defstr(hasharr *chrt, char *type, PD_type_kind kind,
		      memdes *lst, multides *tuple, long sz, int align,
		      PD_byte_order ord, int conv,
		      int *ordr, long *formt, int unsgned, int onescmp)
   {defstr *dp;
    memdes *desc;
    int n;

    dp = CMAKE(defstr);

    dp->type        = CSTRSAVE(type);
    dp->is_indirect = _PD_indirection(dp->type);   
    dp->alignment   = align;
    dp->kind        = kind;
    dp->convert     = conv;
    dp->onescmp     = onescmp;
    dp->unsgned     = unsgned;
    dp->fix.order   = ord;
    dp->fp.order    = ordr;
    dp->fp.format   = formt;
    dp->members     = lst;
    dp->tuple       = tuple;

    if (sz >= 0)
       {dp->size_bits = 0L;
        dp->size      = sz;}
    else
       {dp->size_bits = -sz;
        dp->size      = (-sz + 7) >> 3L;};

/* find the number of indirects */
    for (n = 0, desc = lst; desc != NULL; desc = desc->next)
        {if (_PD_indirection(desc->type))
            n++;

/* add the number of indirects of the member type
 * without this _PD_sys_write won't properly write out
 * trees with nested structures in which there are layers
 * with no indirects!!!
 */
/* GOTCHA: make the reads/writes work when the counting goes this way
	 n += _PD_num_indirects(desc->type, chrt);
 */
         };

    n += (strcmp(type, "*") == 0);

    dp->n_indirects = n;

    SC_mark(dp->type, 1);
    SC_mark(ordr, 1);
    SC_mark(formt, 1);
    SC_mark(lst, 1);
    SC_mark(tuple, 1);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DEFSTR_COPY - make a deep copy of a defstr */

defstr* _PD_defstr_copy(defstr *dp)
   {int *order;
    long *format;
    defstr *copy;
    memdes *members;
    multides *tuple;

    if (dp->tuple != NULL)
       tuple = _PD_copy_tuple(dp->tuple);
    else
       tuple = NULL;

    if (dp->members != NULL)
       members = PD_copy_members(dp->members);
    else
       members = NULL;

    if (dp->fp.order != NULL)
       order = dp->fp.order;       /* GOTCHA: not a deep copy */
    else 
       order = NULL;

    if (dp->fp.format != NULL)
       format = dp->fp.format;      /* GOTCHA: not a deep copy */
    else
       format = NULL;

    copy = _PD_mk_defstr(NULL, dp->type, dp->kind,
			 members, tuple, dp->size, 
			 dp->alignment, dp->fix.order, dp->convert, 
                         order, format, dp->unsgned, dp->onescmp);

    return(copy);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RL_DEFSTR - THREADSAFE
 *               - free up the storage associated with a defstr
 */

void _PD_rl_defstr(defstr *dp)
   {int *ord;
    long *frm;
    memdes *desc, *next;
    multides *tuple;

    if (dp == NULL)
       return;

    if (SC_safe_to_free(dp))
       {for (desc = dp->members; desc != NULL; desc = next)
	    {next = desc->next;
	     _PD_rl_descriptor(desc);};

	tuple = dp->tuple;
	if (tuple != NULL)
	   _PD_free_tuple(tuple);

	ord = dp->fp.order;
	if ((ord != NULL) && (SC_arrlen(ord) > -1))
	   CFREE(dp->fp.order);

	frm = dp->fp.format;
	if ((frm != NULL) && (SC_arrlen(frm) > -1))
	   CFREE(dp->fp.format);

	CFREE(dp->type);};

    CFREE(dp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_HA_RL_DEFSTR - free up the storage associated with a
 *                  - defstr in a hash array
 */

int _PD_ha_rl_defstr(haelem *hp, void *a)
   {int ok;
    defstr *dp;

    ok = SC_haelem_data(hp, NULL, NULL, (void **) &dp, TRUE);
    SC_ASSERT(ok == TRUE);

    _PD_rl_defstr(dp);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MAKE_TUPLE - instantiate a multides */

multides *_PD_make_tuple(char *type, int ni, int *ord)
   {multides *tuple;

    tuple = CMAKE(multides);

    tuple->type  = CSTRSAVE(type);
    tuple->ni    = ni;
    tuple->order = ord;

    SC_mark(ord, 1);
    SC_mark(type, 1);

    return(tuple);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_COPY_TUPLE - copy a multides */

multides *_PD_copy_tuple(multides *tuple)
   {multides *ntuple;

    ntuple = CMAKE(multides);

    *ntuple = *tuple;

    ntuple->type  = CSTRSAVE(tuple->type);
    ntuple->order = SC_copy_item(tuple->order);

    SC_mark(ntuple->order, 1);
    SC_mark(ntuple->type, 1);

    return(ntuple);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FREE_TUPLE - free a multides */

void _PD_free_tuple(multides *tuple)
   {

    if (tuple != NULL)
       {CFREE(tuple->type);
	CFREE(tuple->order);
	CFREE(tuple);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_COPY_MEMBERS - copy a linked list of members
 *
 * #bind PD_copy_members fortran() scheme() python()
 */

memdes *PD_copy_members(memdes *desc)
   {inti is, ns;
    memdes *newm, *nnxt, *thism, *prevm;
    char *ms, *ts, *bs, *ss, *cs;
    dimdes *nd;

    newm  = NULL;
    prevm = NULL;
    for (thism = desc; thism != NULL; thism = thism->next)
        {nnxt = CMAKE(memdes);

         ms = CSTRSAVE(thism->member);
         ts = CSTRSAVE(thism->type);
         bs = CSTRSAVE(thism->base_type);
         ss = CSTRSAVE(thism->name);
         nd = PD_copy_dims(thism->dimensions);

         nnxt->member      = ms;
         nnxt->type        = ts;
         nnxt->is_indirect = _PD_indirection(nnxt->type);   
         nnxt->base_type   = bs;
         nnxt->name        = ss;
         nnxt->dimensions  = nd;
         nnxt->next        = NULL;

         SC_mark(ms, 1);
         SC_mark(ts, 1);
         SC_mark(bs, 1);
         SC_mark(ss, 1);
         SC_mark(nd, 1);

         nnxt->member_offs = thism->member_offs;
         nnxt->number      = thism->number;

/* copy cast data */
         nnxt->cast_offs = thism->cast_offs;
         if (thism->cast_memb != NULL)
	    {cs = CSTRSAVE(thism->cast_memb);
	     nnxt->cast_memb  = cs;
             SC_mark(cs, 1);}
	 else
	    nnxt->cast_memb = NULL;

/* copy size_from data */
	 ns = SC_arrlen(thism->size_offs);
	 if (ns > 0)
	    {int64_t *nso;
	     char **nsm;

	     ns /= sizeof(int64_t);

	     nso = CMAKE_N(int64_t, ns);
	     nsm = CMAKE_N(char *, ns);

	     nnxt->size_offs = nso;
	     nnxt->size_memb = nsm;

	     for (is = 0; is < ns; is++)
	         {nso[is] = thism->size_offs[is];
		  cs      = thism->size_memb[is];
		  if (cs != NULL)
		     {nsm[is] = CSTRSAVE(cs);
		      SC_mark(cs, 1);};};}
	 else
	    {nnxt->size_offs = NULL;
	     nnxt->size_memb = NULL;};

         if (newm == NULL)
            newm = nnxt;
         else
            {prevm->next = nnxt;
	     SC_mark(nnxt, 1);};

         prevm = nnxt;};

    return(newm);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MK_DESCRIPTOR - build a MEMber DEScriptor out of the given string */

memdes *_PD_mk_descriptor(char *member, int defoff)
   {int ie;
    char bf[MAXLINE];
    char *ms, *ts, *bs, *ns, *p;
    memdes *desc;
    dimdes *nd;

    desc = CMAKE(memdes);

/* get rid of any leading white space */
    p = SC_trim_left(member, " \t\n\r\f");

/* convert 'void *' to 'char *'
 * also 'unsigned <type>' to 'u_<type>'
 * GOTCHA: this should be eliminated by accepting and properly
 * treating 'void *' because if you convert char types you
 * cannot confuse them with blobs
 */
    if ((strncmp(p, "void ", 5) == 0) && (p[5] == '*'))
       {SC_strncpy(bf, MAXLINE, p, -1);
	p = PS_subst(bf, "void", "char", 1);};

    if (strncmp(p, "unsigned ", 9) == 0)
       {SC_strncpy(bf, MAXLINE, p, -1);
	p = PS_subst(bf, "unsigned ", "u_", 1);};

/* look for function pointer */
    if (SC_strstr(p, "(*") != NULL)
       {ts = CSTRSAVE("function");
	bs = CSTRSAVE("function");
	ns = _PD_member_name(p);
	nd = NULL;
	snprintf(bf, MAXLINE, "function %s", ns);
	ms = CSTRSAVE(bf);}

    else
       {ms = CSTRSAVE(p);
	ts = _PD_member_type(p);
	bs = _PD_member_base_type(p);
	ns = _PD_member_name(p);
	nd = _PD_ex_dims(p, defoff, &ie);};
	
    desc->member      = ms;
    desc->type        = ts;
    desc->is_indirect = _PD_indirection(desc->type);  
    desc->base_type   = bs;
    desc->name        = ns;
    desc->dimensions  = nd;

    SC_mark(ms, 1);
    SC_mark(ts, 1);
    SC_mark(bs, 1);
    SC_mark(ns, 1);
    SC_mark(nd, 1);

    desc->number      = _PD_comp_num(desc->dimensions);
    desc->member_offs = -1L;
    desc->cast_offs   = -1L;
    desc->cast_memb   = NULL;
    desc->size_offs   = NULL;
    desc->size_memb   = NULL;
    desc->next        = NULL;

    return(desc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RL_DESCRIPTOR - THREADSAFE
 *                   - release a MEMber DEScriptor
 */

void _PD_rl_descriptor(memdes *desc)
   {

    if (desc != NULL)
       {if (SC_safe_to_free(desc))
	   {CFREE(desc->member);
	    CFREE(desc->name);
	    CFREE(desc->type);
	    CFREE(desc->base_type);
	    CFREE(desc->cast_memb);
	    CFREE(desc->size_offs);
	    SC_free_strings(desc->size_memb);

	    _PD_rl_dimensions(desc->dimensions);
	    desc->dimensions = NULL;};

	CFREE(desc);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MK_DIMENSIONS - build a DIMension DEScriptor out of the given numbers
 *                   -
 *                   - struct s_dimdes
 *                   -    {long index_min;
 *                   -     long index_max;
 *                   -     long number;
 *                   -     struct s_dimdes *next;};
 *                   -
 */

dimdes *_PD_mk_dimensions(long mini, long leng)
   {dimdes *dims;

    dims            = CMAKE(dimdes);
    dims->index_min = mini;
    dims->index_max = mini + leng - 1L;
    dims->number    = leng;
    dims->next      = NULL;

    return(dims);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RL_DIMENSIONS - THREADSAFE
 *                   - release a DIMension DEScriptor
 */

void _PD_rl_dimensions(dimdes *dims)
   {dimdes *pp, *nxt;
    int nc;

    for (pp = dims; pp != NULL; pp = nxt)
        {nxt = pp->next;
         nc  = SC_ref_count(pp);
         CFREE(pp);
         if (nc > 1)
	    break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_FINISH - THREADSAFE
 *           - cleanup special PDB memory
 */

void PD_finish(void)
   {

    _PD_fin_stacks();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
