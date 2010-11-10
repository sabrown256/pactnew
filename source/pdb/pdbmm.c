/*
 * PDBMM.C - memory management for the PDB library system
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

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
       SC_fix_lmt(sizeof(BIGINT), NULL, &_PD.maxfsize, NULL);

    file = FMAKE(PDBfile, "_PD_MK_PDB:file");
    if (file != NULL)
       {if (sys == NULL)
	   sys = &_PD_sys;

	if (tr == NULL)
	   tr = &_PD_tr;

	fp = (pu != NULL) ? pu->stream : NULL;

	if (name == NULL)
	   name = pu->udl;

	if (md == NULL)
	   md = pu->mode;

	file->udl        = pu;
	file->stream     = fp;
	file->name       = SC_strsavef(name, "char*:_PD_MK_PDB:name");
	file->type       = NULL;

	file->symtab            = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY);
	file->chart             = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY);
	file->host_chart        = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY);
	file->attrtab           = NULL;
	file->mode              = PD_UNINIT;
	file->file_mode         = SC_strsavef(md, "char*:_PD_MK_PDB:file_mode");

	file->maximum_size     = _PD.maxfsize;                   /* family info */
	file->previous_file    = NULL;

	file->virtual_internal = FALSE;                 /* disk file by default */
	file->current_prefix   = NULL;       /* read/write variable name prefix */
	file->ptr_base         = SC_strsavef("/&ptrs/ia_", "_PD_MK_PDB:ptr_base");

	file->default_offset = 0;           /* default offset for array indexes */
	file->major_order    = ROW_MAJOR_ORDER;

	file->use_itags      = TRUE;
	file->track_pointers = TRUE;

	file->std        = NULL;
	file->align      = NULL;
	file->host_std   = _PD_copy_standard(pa->int_std);
	file->host_align = _PD_copy_alignment(pa->int_align);

	file->n_dyn_spaces = 0L;
#if 0
	file->free_list = NULL;
#endif
	file->mpi_mode = 0;              /* 1: serial, 0: parallel */
	file->mpi_file = FALSE;

	file->symtaddr = 0;
	file->chrtaddr = 0;
	file->headaddr = 0;

	file->use_cksum  = PD_MD5_OFF;
	file->file_cksum = PD_MD5_OFF;
	file->fix_denorm = FALSE;

	file->delim      = NULL;
        file->radix      = 10;

	file->sys            = sys;
	file->tr             = tr;
	file->date           = SC_date();
	file->system_version = PDB_SYSTEM_VERSION;

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
	   _PD_ADD_FILE(file, (BIGINT) 0);

	file->host_std->file = file;};

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RL_PDB - THREADSAFE
 *            - release the storage associated with the PDBfile
 */

void _PD_rl_pdb(PDBfile *file)
   {

    SFREE(file->date);
    SFREE(file->ptr_base);

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
       SFREE(file->previous_file);

    if (file->type != NULL)
       SFREE(file->type);

    SFREE(file->current_prefix);
    SFREE(file->file_mode);
    SFREE(file->name);
    SFREE(file);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_ALLOC_ENTRY - allocate memory for the variable type */

void *PD_alloc_entry(PDBfile *file, char *name)
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

void *_PD_alloc_entry(PDBfile *file, char *type, long ni)
   {int ipt;
    long len, bpi;
    void *vr;
    defstr *dp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    if (_PD_indirection(type))
       {vr = (char *) FMAKE_N(char *, ni, "_PD_ALLOC_ENTRY:char *");}
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
        vr  = SC_alloc_nzt(len, 1L, "_PD_ALLOC_ENTRY:vr", NULL);};

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

    std = FMAKE(data_standard, "_PD_MK_STANDARD:std");

    std->bits_byte         = BITS_DEFAULT;
    std->ptr_bytes         = 0;
    std->bool_bytes        = 0;

    for (i = 0; i < PD_N_PRIMITIVE_FIX; i++)
        {std->fx[i].bpi   = 0;
	 std->fx[i].order = NO_ORDER;};

    for (i = 0; i < PD_N_PRIMITIVE_FP; i++)
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

    std = FMAKE(data_standard, "_PD_COPY_STANDARD:std");

    std->bits_byte  = src->bits_byte;
    std->ptr_bytes  = src->ptr_bytes;
    std->bool_bytes = src->bool_bytes;

    for (i = 0; i < PD_N_PRIMITIVE_FIX; i++)
        {std->fx[i].bpi   = src->fx[i].bpi;
	 std->fx[i].order = src->fx[i].order;};

    for (i = 0; i < PD_N_PRIMITIVE_FP; i++)
        {std->fp[i].bpi = src->fp[i].bpi;

	 n    = FORMAT_FIELDS;
         fstd = FMAKE_N(long, n, "_PD_COPY_STANDARD:fp_format");
	 SC_mark(fstd, 1);
	 std->fp[i].format = fstd;
	 fsrc = src->fp[i].format;
	 for (j = 0; j < n; j++, *(fstd++) = *(fsrc++));

	 n = std->fp[i].bpi;
	 n = (n > 0) ? n : -(n / std->bits_byte);
	 ostd = FMAKE_N(int,  n, "_PD_COPY_STANDARD:fp_order");
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
   {if (std != NULL)
       {SFREE(std->fp[0].format);
        SFREE(std->fp[0].order);
        SFREE(std->fp[1].format);
        SFREE(std->fp[1].order);
        SFREE(std->fp[2].format);
        SFREE(std->fp[2].order);

        SFREE(std);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MK_ALIGNMENT - THREADSAFE
 *                  - allocate, initialize and return a pointer to a
 *                  - data_alignment
 */

data_alignment *_PD_mk_alignment(char *vals)
   {int i;
    data_alignment *align;

    align = FMAKE(data_alignment, "_PD_MK_ALIGNMENT:align");

    align->char_alignment = vals[0];
    align->ptr_alignment  = vals[1];
    align->bool_alignment = vals[2];

    for (i = 0; i < 3; i++)
        align->fx[i] = vals[i + 3];

/* default-equal to long alignment*/
    align->fx[3] = vals[5];

    for (i = 0; i < PD_N_PRIMITIVE_FP; i++)
        align->fp[i] = vals[i + 6];

    if (strlen(vals) > 9)
       align->struct_alignment = vals[9];
    else
       align->struct_alignment = 0;

    return(align);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_COPY_ALIGNMENT - THREADSAFE
 *                    - return a copy of a data_alignment
 */

data_alignment *_PD_copy_alignment(data_alignment *src)
   {data_alignment *align;

    align = FMAKE(data_alignment, "_PD_COPY_ALIGNMENT:align");

    *align = *src;

    return(align);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RL_ALIGNMENT - THREADSAFE
 *                  - release a data_alignment
 */

void _PD_rl_alignment(data_alignment *align)
   {if (SC_arrlen(align) > 0)
       {SFREE(align);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_COPY_DIMS - make and return a copy of the given dimension list */

dimdes *PD_copy_dims(dimdes *odims)
   {dimdes *od, *ndims, *prev, *next;

    prev  = NULL;
    ndims = NULL;
    
    for (od = odims; od != NULL; od = od->next)
        {next  = FMAKE(dimdes, "PD_COPY_DIMS:next");
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

/* PD_COPY_SYMENT - make and return a copy of the given syment */

syment *PD_copy_syment(syment *osym)
   {char *ntype;
    syment *nsym;
    dimdes *ndims;

    if (osym == NULL)
       return(NULL);

    nsym = FMAKE(syment, "PD_COPY_SYMENT:nsym");

    ntype = SC_strsavef(PD_entry_type(osym), "char*:PD_COPY_SYMENT:type");
    ndims = PD_copy_dims(PD_entry_dimensions(osym));

    _PD_block_copy(nsym, osym);

    PD_entry_type(nsym)       = ntype;
    PD_entry_dimensions(nsym) = ndims;
    PD_entry_number(nsym)     = PD_entry_number(osym);
    PD_entry_indirects(nsym)  = PD_entry_indirects(osym);

    SC_mark(ntype, 1);
    SC_mark(ndims, 1);

    return(nsym);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MK_SYMENT - make and return a pointer to an entry
 *               - for the symbol table
 */

syment *_PD_mk_syment(char *type, long ni, BIGINT addr, symindir *indr,
		      dimdes *dims)
   {syment *ep;
    char *t;

    ep = FMAKE(syment, "_PD_MK_SYMENT:ep");
    if (ep != NULL)
       {ep->blocks = NULL;

	_PD_block_init(ep, TRUE);
	_PD_entry_set_number(ep, 0, ni);
	_PD_entry_set_address(ep, 0, addr);

	if (type == NULL)
	   t = NULL;
	else
	   {t = SC_strsavef(type, "char*:_PD_MK_SYMENT:type");
	    SC_mark(t, 1);};

	PD_entry_type(ep)       = t;
	PD_entry_number(ep)     = ni;
	PD_entry_dimensions(ep) = dims;

	if (indr == NULL)
	   {symindir iloc;
	    iloc.addr       = 0;
	    iloc.n_ind_type = 0L;
	    iloc.arr_offs = 0L;
	    PD_entry_indirects(ep) = iloc;}

	else
	   PD_entry_indirects(ep)  = *indr;

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

    ok = SC_haelem_data(hp, &name, NULL, (void **) &ep);
    _PD_rl_syment_d(ep);

    SFREE(name);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RL_SYMENT - THREADSAFE
 *               - reclaim the space of the given syment
 */

void _PD_rl_syment(syment *ep)
   {

    if (ep != NULL)
       {SFREE(PD_entry_type(ep));
	_PD_block_rel(ep);
	SFREE(ep);};

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

    dp = FMAKE(defstr, "_PD_MK_DEFSTR:dp");

    dp->type        = SC_strsavef(type, "char*:_PD_MK_DEFSTR:type");
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
	   SFREE(dp->fp.order);

	frm = dp->fp.format;
	if ((frm != NULL) && (SC_arrlen(frm) > -1))
	   SFREE(dp->fp.format);

	SFREE(dp->type);};

    SFREE(dp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_HA_RL_DEFSTR - free up the storage associated with a
 *                  - defstr in a hash array
 */

int _PD_ha_rl_defstr(haelem *hp, void *a)
   {int ok;
    defstr *dp;

    ok = SC_haelem_data(hp, NULL, NULL, (void **) &dp);
    _PD_rl_defstr(dp);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MAKE_TUPLE - instantiate a multides */

multides *_PD_make_tuple(char *type, int ni, int *ord)
   {multides *tuple;

    tuple = FMAKE(multides, "_PD_MAKE_TUPLE:tuple");

    tuple->type  = SC_strsavef(type, "char*:_PD_MAKE_TUPLE:type");
    tuple->ni    = ni;
    tuple->order = ord;

    SC_mark(ord, 1);

    return(tuple);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_COPY_TUPLE - copy a multides */

multides *_PD_copy_tuple(multides *tuple)
   {multides *ntuple;

    ntuple = FMAKE(multides, "_PD_COPY_TUPLE:ntuple");

    *ntuple = *tuple;

    ntuple->type  = SC_strsavef(tuple->type, "char*:_PD_COPY_TUPLE:type");
    ntuple->order = SC_copy_item(tuple->order);

    return(ntuple);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FREE_TUPLE - free a multides */

void _PD_free_tuple(multides *tuple)
   {

    SFREE(tuple->type);
    SFREE(tuple->order);
    SFREE(tuple);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_COPY_MEMBERS - copy a linked list of members */

memdes *PD_copy_members(memdes *desc)
   {memdes *newm, *nnxt, *thism, *prevm;
    char *ms, *ts, *bs, *ns, *cs;
    dimdes *nd;

    newm  = NULL;
    prevm = NULL;
    for (thism = desc; thism != NULL; thism = thism->next)
        {nnxt = FMAKE(memdes, "PD_COPY_MEMBERS:nnxt");

         ms = SC_strsavef(thism->member,
			  "char*:PD_COPY_MEMBERS:member");
         ts = SC_strsavef(thism->type,
			  "char*:PD_COPY_MEMBERS:type");
         bs = SC_strsavef(thism->base_type,
			  "char*:PD_COPY_MEMBERS:base_type");
         ns = SC_strsavef(thism->name,
			  "char*:PD_COPY_MEMBERS:name");
         nd = PD_copy_dims(thism->dimensions);

         nnxt->member      = ms;
         nnxt->type        = ts;
         nnxt->is_indirect = _PD_indirection(nnxt->type);   
         nnxt->base_type   = bs;
         nnxt->name        = ns;
         nnxt->dimensions  = nd;
         nnxt->next        = NULL;

         SC_mark(ms, 1);
         SC_mark(ts, 1);
         SC_mark(bs, 1);
         SC_mark(ns, 1);
         SC_mark(nd, 1);

         nnxt->member_offs = thism->member_offs;
         nnxt->cast_offs   = thism->cast_offs;
         nnxt->number      = thism->number;

         if (thism->cast_memb != NULL)
	    {cs = SC_strsavef(thism->cast_memb,
			      "char*:PD_COPY_MEMBERS:cast_memb");
	     nnxt->cast_memb  = cs;
             SC_mark(cs, 1);}
	 else
	    nnxt->cast_memb = NULL;

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
    char *ms, *ts, *bs, *ns, *p, *fp;
    memdes *desc;
    dimdes *nd;

    desc = FMAKE(memdes, "_PD_MK_DESCRIPTOR:desc");

/* get rid of any leading white space */
    for (p = member; strchr(" \t\n\r\f", *p) != NULL; p++);

    fp = SC_strstr(p, "(*");
    if (fp == NULL)
       {ms = SC_strsavef(p, "char*:_PD_MK_DESCRIPTOR:member");
	ts = _PD_member_type(p);
	bs = _PD_member_base_type(p);
	ns = _PD_member_name(p);
	nd = _PD_ex_dims(p, defoff, &ie);}

    else
       {ts = SC_strsave("function");
	bs = SC_strsave("function");
	ns = _PD_member_name(p);
	nd = NULL;
	snprintf(bf, MAXLINE, "function %s", ns);
	ms = SC_strsavef(bf, "char*:_PD_MK_DESCRIPTOR:member");};
	
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
    desc->next        = NULL;

    return(desc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RL_DESCRIPTOR - THREADSAFE
 *                   - release a MEMber DEScriptor
 */

void _PD_rl_descriptor(memdes *desc)
   {

    if (desc == NULL)
       return;

    if (SC_safe_to_free(desc))
       {SFREE(desc->member);
        SFREE(desc->name);
        SFREE(desc->type);
        SFREE(desc->base_type);
        SFREE(desc->cast_memb);

        _PD_rl_dimensions(desc->dimensions);}

    SFREE(desc);

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

    dims            = FMAKE(dimdes, "_PD_MK_DIMENSIONS:dims");
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
         SFREE(pp);
         if (nc > 1)
	    break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
