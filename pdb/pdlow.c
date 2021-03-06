/*
 * PDLOW.C - low level routines for PDBlib
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

#define PD_COMPARE_PTR_STD(eq, a, b, c, d)                                   \
    eq = (a->ptr_bytes != b->ptr_bytes) ||                                   \
         (c->ptr_alignment != d->ptr_alignment)

#define PD_COMPARE_CHAR_STD(eq, a, b, c, d)                                  \
    eq = (c->char_alignment != d->char_alignment)

#define PD_COMPARE_BOOL_STD(eq, a, b, c, d)                                  \
    eq = ((a->bool_bytes != b->bool_bytes) ||                                \
	  (c->bool_alignment != d->bool_alignment))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_COMPARE_CHAR_STD - compare character type N from
 *                      - data_standard SA and SB and
 *                      - data_alignments AA and AB
 *                      - return TRUE iff conversion is needed
 *                      - if FTK is TRUE defer decision and return -1
 */

static int _PD_compare_char_std(int n, data_standard *sa, data_standard *sb,
				data_alignment *aa, data_alignment *ab,
				int flag, int ftk)
   {int eq;

    eq = FALSE;

    if (flag == TRUE)
       {if (ftk == FALSE)
	   eq = -1;
        else
	   {eq |= (sa->chr[n].bpi != sb->chr[n].bpi);
	    eq |= (sa->chr[n].utf != sb->chr[n].utf);
	    eq |= (aa->chr[n] != ab->chr[n]);};};

    return(eq);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_COMPARE_FIX_STD - compare fixed point type N from
 *                     - data_standard SA and SB and
 *                     - data_alignments AA and AB
 *                     - return TRUE iff conversion is needed
 *                     - if FTK is TRUE defer decision and return -1
 */

static int _PD_compare_fix_std(int n, data_standard *sa, data_standard *sb,
			       data_alignment *aa, data_alignment *ab,
			       int flag, int ftk)
   {int eq, na, nb, la, lb;
    PD_byte_order oa, ob;

    eq = FALSE;

    if (flag == TRUE)
       {if (ftk == FALSE)
	   eq = -1;
        else
	   {na = sa->fx[n].bpi;
	    oa = sa->fx[n].order;
	    la = aa->fx[n];

	    nb = sb->fx[n].bpi;
	    ob = sb->fx[n].order;
	    lb = ab->fx[n];

	    eq = (na != nb) || (oa != ob) || (la != lb);};};

    return(eq);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_COMPARE_FP_STD - compare floating point type N from
 *                    - data_standard SA and SB and
 *                    - data_alignments AA and AB
 *                    - return TRUE iff conversion is needed
 *                    - if FTK is TRUE defer decision and return -1
 */

static int _PD_compare_fp_std(int n, data_standard *sa, data_standard *sb,
			      data_alignment *aa, data_alignment *ab,
			      int flag, int ftk)
   {int j, la, lb, na, nb, eq;
    int *poa, *pob;
    long *pfa, *pfb;

    eq = FALSE;
    if (flag == TRUE)
       {if (ftk == FALSE)
	   eq = -1;

        else
	   {na  = sa->fp[n].bpi;
	    poa = sa->fp[n].order;
	    pfa = sa->fp[n].format;
	    la  = aa->fp[n];

	    nb  = sb->fp[n].bpi;
	    pob = sb->fp[n].order;
	    pfb = sb->fp[n].format;
	    lb  = ab->fp[n];

	    eq = (na != nb) || (la != lb);
	    if (eq == FALSE)
	       {for (j = 0; j < na; j++)
		    eq |= (*(poa++) != *(pob++));
		for (j = 0; j < PD_gs.format_fields; j++)
		    eq |= (*(pfa++) != *(pfb++));};};};

    return(eq);}

/*--------------------------------------------------------------------------*/

/*                           FORMAT ROUTINES                                */

/*--------------------------------------------------------------------------*/

/* _PD_COMPARE_STD - compare two data_standards
 *                 - and the associated alignments
 *                 - return TRUE if all elements are equal in value
 */

int _PD_compare_std(data_standard *a, data_standard *b,
		    data_alignment *c, data_alignment *d)
   {int i, j, n, *oa, *ob;
    long *fa, *fb;
    int eq;

    eq = (a->ptr_bytes == b->ptr_bytes);

    for (i = 0; i < N_PRIMITIVE_CHAR; i++)
        eq &= ((a->chr[i].bpi == b->chr[i].bpi) &&
	       (a->chr[i].utf == b->chr[i].utf));

    for (i = 0; i < N_PRIMITIVE_FIX; i++)
        eq &= ((a->fx[i].bpi   == b->fx[i].bpi) &&
	       (a->fx[i].order == b->fx[i].order));

    for (i = 0; i < N_PRIMITIVE_FP; i++)
        eq &= (a->fp[i].bpi == b->fp[i].bpi);

    if (!eq)
       return(FALSE);

/* check the floating point byte orders */
    for (i = 0; i < N_PRIMITIVE_FP; i++)
        {n  = a->fp[i].bpi;
	 oa = a->fp[i].order;
	 ob = b->fp[i].order;
	 for (j = 0; j < n; j++, eq &= (*(oa++) == *(ob++)));};

/* check the floating point format data */
    n  = PD_gs.format_fields;
    for (i = 0; i < N_PRIMITIVE_FP; i++)
        {fa = a->fp[i].format;
	 fb = b->fp[i].format;
	 for (j = 0; j < n; j++, eq &= (*(fa++) == *(fb++)));};

/* check alignments */
    eq &= ((c->ptr_alignment  == d->ptr_alignment) &&
           (c->bool_alignment == d->bool_alignment));

    for (i = 0; i < N_PRIMITIVE_CHAR; i++)
        eq &= (c->chr[i] == d->chr[i]);

    for (i = 0; i < N_PRIMITIVE_FIX; i++)
        eq &= (c->fx[i] == d->fx[i]);

    for (i = 0; i < N_PRIMITIVE_FP; i++)
        eq &= (c->fp[i] == d->fp[i]);

    return(eq);}

/*--------------------------------------------------------------------------*/

/*                            SUPPORT ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* _PD_SET_IO_BUFFER - set the I/O buffer */

void _PD_set_io_buffer(SC_udl *pu)
   {int64_t sz;
    char *bf;
    FILE *fp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);
    sz = pa->buffer_size;
    if (sz > 0)
       {fp = pu->stream;
	bf = CMAKE_N(char, sz);

	if (lio_setvbuf(fp, bf, _IOFBF, sz) != 0)
	   {CFREE(bf);
	    PD_error("CANNOT SET FILE BUFFER - _PD_SET_IO_BUFFER", PD_OPEN);}
	else
	   {if (pu->buffer != NULL)
	       CFREE(pu->buffer);
	    pu->buffer = bf;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PIO_OPEN - do an open in a parallel context */

SC_udl *_PD_pio_open(const char *name, const char *mode)
   {const char *s, *md;
    FILE *fp;
    PFfopen opn;
    SC_udl *pu;

    opn = PD_gs.par.open_hook;
    fp  = NULL;

    pu = _SC_parse_udl(name);
    if ((pu != NULL) && (pu->protocol == NULL))
       {s  = name;
	md = mode;
	if ((s != NULL) && (pu->entry != NULL))
	   fp = _PD_open_container_file(s, md);

	else if (opn != NULL)
	   {if ((md != NULL) && (strcmp(md, "net") == 0))
	       md = BINARY_MODE_WPLUS;
	    else
	       s = pu->path;

	    fp = (*opn)(s, md);};

	if (fp == NULL)
	   {_SC_rel_udl(pu);
	    pu = NULL;}
	else
	   {pu->stream = fp;
	    pu->mode   = CSTRSAVE(md);
	    _PD_set_io_buffer(pu);};};

    return(pu);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SAFE_FLUSH - do a flush if it is safe to do so
 *                - ANSI standard says fflush on output stream only
 *                - so do not try it on a read only stream
 *                - return file length if successful
 *                - and -1 otherwise
 */

int _PD_safe_flush(PDBfile *file)
   {int ok, rv;
    FILE *fp;
    SC_THREAD_ID(_t_index);

    rv = -1;
    if ((file->virtual_internal == FALSE) &&
        (strcmp(file->file_mode, BINARY_MODE_R) != 0))
       {fp = file->stream;
        ok = _PD_SERIAL_FLUSH(fp, _t_index);
        rv = (ok == 0);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_INQUIRE_TYPE - Lookup and return the file chart entry for NAME
 *                 - in the PDBfile FILE.
 *                 - Return NULL if not found.
 *
 * #bind PD_inquire_type fortran() scheme() python()
 */

defstr *PD_inquire_type(const PDBfile *file ARG(,,cls), const char *name)
   {defstr *dp;

    dp = NULL;
    if (name != NULL)
       {if (_PD_indirection(name) == TRUE)
	   dp = (defstr *) SC_hasharr_def_lookup(file->chart, "*");
        else
	   dp = (defstr *) SC_hasharr_def_lookup(file->chart, name);};

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_INQUIRE_HOST_TYPE - Lookup and return the host chart entry for NAME
 *                      - in the PDBfile FILE.
 *                      - Return NULL if not found.
 *
 * #bind PD_inquire_host_type fortran() scheme() python()
 */

defstr *PD_inquire_host_type(const PDBfile *file ARG(,,cls),
			     const char *name)
   {defstr *dp;

    if (_PD_indirection(name) == TRUE)
       dp = (defstr *) SC_hasharr_def_lookup(file->host_chart, "*");
    else
       dp = (defstr *) SC_hasharr_def_lookup(file->host_chart, name);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_REMOVE_TYPE - remove a defstr from the given file
 *                 - return FALSE if the NAME'd type does not exist
 *                 - in both charts
 *                 - return TRUE if successful
 */

int _PD_remove_type(PDBfile *file, char *name)
   {int rv;
    defstr *dp;

    rv = 0;

/* remove from the host chart */
    dp = (defstr *) SC_hasharr_def_lookup(file->host_chart, name);
    if (dp != NULL)
       {rv++;
	SC_hasharr_remove(file->host_chart, name);};

/* remove from the file chart */
    dp = (defstr *) SC_hasharr_def_lookup(file->chart, name);
    if (dp != NULL)
       {rv++;
	SC_hasharr_remove(file->chart, name);};

    rv = (rv == 2);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_E_INSTALL - install a syment in the given hash table */

int _PD_e_install(PDBfile *file, const char *name, syment *ep, int lookup)
   {int rv;
    hasharr *tab;
    dimdes *dms;

    rv = FALSE;

/* do not add zero length variables to the symbol table */
    dms = PD_entry_dimensions(ep);
    if ((dms != NULL) && (dms->number == 0))
       _PD_rl_dimensions(dms);

    else
       {tab = file->symtab;

/* we can leak a lot of memory if we don't check this!! */
	if (lookup)
	   PD_remove_entry(file, name);

	SC_hasharr_install(tab, name, ep, G_SYMENT_P_S, 1, -1);

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_D_INSTALL_IN - install a defstr pointer in the given chart 
 *                  - (should be used in this file only)
 */

static void _PD_d_install_in(const char *name, defstr *def, hasharr *tab,
			     PD_chart_kind chk, const char *alias)
   {char *typ;
    defstr *dp;
    haelem *hp;
    SC_kind kind;

/* we can leak a lot of memory if we don't check this
 * PURIFY complains of free memory read if hp->def not
 * nulled before SC_hasharr_remove
 */
    hp = SC_hasharr_lookup(tab, name);
    dp = (hp != NULL) ? (defstr *) hp->def : NULL;
    if (dp != NULL)
       {_PD_rl_defstr(dp);
	hp->def = NULL;
	SC_hasharr_remove(tab, name);};

    SC_hasharr_install(tab, name, def, G_DEFSTR_P_S, 3, -1);

/* if this is the host chart register the type */
    typ = def->type;
    if (alias != NULL)
       SC_type_alias(name, SC_type_id(alias, FALSE));
    else if ((chk == PD_CHART_HOST) && (strncmp(typ, "u_", 2) != 0))
       {if (typ[0] == '*')
           kind = KIND_POINTER;
	else if (strcmp(typ, "bool") == 0)
	   kind = KIND_BOOL;
	else
	   {kind = KIND_OTHER;
	    switch (def->kind)
	       {case KIND_CHAR :
		     kind = KIND_CHAR;
		     break;
		case KIND_INT :
		     kind = KIND_INT;
		     break;
		case KIND_FLOAT :
		     if (def->tuple == NULL)
		        kind = KIND_FLOAT;
		     else if (def->tuple->ni == 2)
		        kind = KIND_COMPLEX;
		     else if (def->tuple->ni == 4)
		        kind = KIND_QUATERNION;
		     break;
		case KIND_STRUCT :
		     kind = KIND_STRUCT;
		     break;
		default :
		     break;};};

	    SC_type_register(name, kind, B_T, def->size, 0);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_D_INSTALL - install a defstr pointer in the given chart of the file */

void _PD_d_install(PDBfile *file, const char *name, defstr *def,
		   PD_chart_kind chk, const char *alias)
   {hasharr *ch;

    ch = (chk == PD_CHART_HOST) ? file->host_chart : file->chart;

    _PD_d_install_in(name, def, ch, chk, alias);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DEFSTR_IN - primitive defstruct used to install the primitive types
 *               - in the specified chart (should be used in this file only)
 */

static defstr *_PD_defstr_in(hasharr *chart, const char *name, SC_kind kind,
			     memdes *desc, multides *tuple,
                             long sz, int align,
			     PD_byte_order ord, int conv,
			     int *ordr, long *formt, int unsgned, int onescmp,
			     PD_chart_kind chk, char *alias)
   {defstr *dp;

    dp = _PD_mk_defstr(chart, name, kind,
		       desc, tuple,
		       sz, align, ord, conv,
		       ordr, formt, unsgned, onescmp);
    if (dp == NULL)
       PD_error("DEFINITION FAILED - _PD_DEFSTR_IN", PD_GENERIC);
    else
       _PD_d_install_in(name, dp, chart, chk, alias);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DEFSTR - primitive define struct used to install the primitive types
 *            - in the specified chart set
 *            - (both hashed and non-hashed versions) 
 */

defstr *_PD_defstr(PDBfile *file, PD_chart_kind chk,
		   const char *name, SC_kind kind,
		   memdes *desc, multides *tuple,
		   long sz, int align, PD_byte_order ord,
		   int conv, int *ordr, long *formt, int unsgned, int onescmp)
   {defstr *dp;
    hasharr *ch;

    dp = NULL;

/* do not permit '*' in type names except for "*" to define
 * the pointer
 */
    if ((strchr(name, '*') == NULL) || (strlen(name) < 2))
       {ch = (chk == PD_CHART_HOST) ? file->host_chart : file->chart;

	dp = _PD_defstr_in(ch, name, kind,
			   desc, tuple, sz, align, ord, conv,
			   ordr, formt, unsgned, onescmp, chk, NULL);}
    else
       PD_error("ILLEGAL '*' IN TYPE NAME - _PD_DEFSTR", PD_GENERIC);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DEFSTR_INST - install the defstr in all charts
 *                 - if CHK is PD_CHART_FILE return the file defstr
 *                 - if CHK is PD_CHART_HOST return the file host_chart defstr
 */

defstr *_PD_defstr_inst(PDBfile *file, const char *name, SC_kind kind,
			memdes *desc, PD_byte_order ord,
			int *ordr, long *formt, PD_chart_kind chk)
   {int algn, conv;
    long sz;
    defstr *rv, *dpf, *dph;
    memdes *pd, *memb;
    hasharr *chart, *host_chart;
    data_alignment *align, *host_align;

    chart      = file->chart;
    host_chart = file->host_chart;
    align      = file->align;
    host_align = file->host_align;

    if (desc == NULL)
       return(NULL);

    dpf = (chk == PD_CHART_FILE) ?
          PD_inquire_table_type(chart, name) :
	  PD_inquire_table_type(host_chart, name);
    if (dpf != NULL)
       return(dpf);

/* install the type in the file->chart */
    sz   = _PD_str_size(desc, chart);
    conv = FALSE;
    algn = align->struct_alignment;
    for (pd = desc; pd != NULL; pd = pd->next)
        {dpf = PD_inquire_table_type(chart, pd->base_type);
         if (_PD_indirection(pd->type) || (dpf == NULL))
            {algn = max(algn, align->ptr_alignment);
	     conv = TRUE;}
         else
            {algn  = max(algn, dpf->alignment);
             conv |= (dpf->convert != 0);};

/* in case we are installing this defstr having read it from
 * another file (as in a copy type operation) redo the cast offsets
 */
         if (pd->cast_memb != NULL)
            pd->cast_offs = _PD_member_location(pd->cast_memb,
						chart,
						dpf,
						&memb);};

    dpf = _PD_defstr(file, PD_CHART_FILE, name, kind,
		    desc, NULL,
		    sz, algn, ord,
	            conv, ordr, formt, FALSE, FALSE);

/* install the type in the host_chart */
    desc = PD_copy_members(desc);
    sz   = _PD_str_size(desc, host_chart);

    algn = host_align->struct_alignment;
    for (pd = desc; pd != NULL; pd = pd->next)
        {dph = PD_inquire_table_type(host_chart, pd->base_type);
	 if (dph != NULL)
	    {if (_PD_indirection(pd->type) || (dph == NULL))
		algn = max(algn, host_align->ptr_alignment);
	     else
	        algn = max(algn, dph->alignment);

/* in case we are installing this defstr having read it from
 * another file (as in a copy type operation) redo the cast offsets
 */
	     if (pd->cast_memb != NULL)
	        pd->cast_offs = _PD_member_location(pd->cast_memb,
						    host_chart,
						    dph,
						    &memb);};};

/* NOTE: ordr, formt, and conv apply only to the file chart
 *       never to the host chart!!!
 *       these are for non-default primitive types which
 *       have no host representation
 */
    dph = _PD_defstr(file, PD_CHART_HOST, name, kind,
		     desc, NULL,
		     sz, algn, NO_ORDER,
		     FALSE, NULL, NULL, FALSE, FALSE);

    rv = (chk == PD_CHART_FILE) ? dpf : dph;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DEFSTR_PRIM_RD - define TYPE from info read from file chart */

void _PD_defstr_prim_rd(PDBfile *file, char *type, char *origtype,
			SC_kind kind, multides *tuple,
			intb bpi, int align, PD_byte_order ord,
			int *ordr, long *formt,
			int unsgned, int onescmp, int conv)
   {int cnv, host_empty;
    defstr *dp;

/* it is either a typedef or a normal type */
     if (origtype != NULL) 
        {dp = PD_inquire_host_type(file, origtype);
	 if (dp != NULL)
	    {_PD_d_install(file,  type, _PD_defstr_copy(dp),
			   PD_CHART_HOST, origtype);
	     host_empty = FALSE;}
	 else
	    host_empty = TRUE;

	 dp = PD_inquire_type(file, origtype);
	 if (dp != NULL)
	    {_PD_d_install(file, type, _PD_defstr_copy(dp),
			   PD_CHART_FILE, origtype);

/* only the file chart has it - better install in the host chart too */
	     if (host_empty == TRUE)
	        _PD_d_install(file, type, _PD_defstr_copy(dp),
			      PD_CHART_HOST, origtype);};

	 _PD_free_tuple(tuple);}

     else 
        {dp = PD_inquire_host_type(file, type);
	 if ((conv == 0) && (dp == NULL))
	    _PD_defstr(file, PD_CHART_HOST, type, kind,
		       NULL, tuple, bpi, align, ord, 0,
		       ordr, formt, unsgned, onescmp);

	 cnv = (_PD.force == TRUE) ? 1 : -1;

	 _PD_defstr(file, PD_CHART_FILE, type, kind,
		    NULL, tuple, bpi, align, ord, cnv,
		    ordr, formt, unsgned, onescmp);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_TYPEDEF_PRIMITIVE_TYPES - Make typedefs for user-defined primitive
 *                            - types in the file chart of PDBfile FILE.
 *
 * #bind PD_typedef_primitive_types fortran() scheme() python()
 */

void PD_typedef_primitive_types(PDBfile *file ARG(,,cls))
   {long i;
    char *type;
    defstr *dp, *ndp;

    for (i = 0; SC_hasharr_next(file->chart, &i, NULL, NULL, (void **) &dp); i++)
        {type = dp->type;
	 if (PD_inquire_host_type(file, type) == NULL)
	    {ndp = _PD_type_container(file, dp);
	     if (ndp != NULL)
	        PD_typedef(file, ndp->type, type);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_TYPE_CONTAINER - return the smallest container type in which the
 *                    - specified user-defined primitive type will fit
 *                    - if the specified type is a struct or one of the
 *                    - standard primitive types, return NULL
 */

defstr *_PD_type_container(const PDBfile *file, defstr *dp)
   {int i, n, id, ipt;
    long size;
    char *type;
    defstr *ndp;
    static char *pdb_types[] = { "*", "function", "Directory" };

    ndp = NULL;

    if ((dp == NULL) || (dp->members != NULL))
       return(ndp);

    type = dp->type;
    id   = SC_type_id(type, FALSE);
    if (id != -1)
       return(ndp);

    n = sizeof(pdb_types)/sizeof(char *);
    for (i = 0; i < n; i++)
        if (strncmp(type, pdb_types[i], strlen(pdb_types[i])) == 0)
	   return(ndp);

    size = dp->size;

    if (dp->fp.format != NULL)
       {ipt = _PD_items_per_tuple(dp);
	if (ipt == 1)
	   id = SC_type_container_size(KIND_FLOAT, size);
	else if (ipt == 2)
	   id = SC_type_container_size(KIND_COMPLEX, size);
	else
	   return(NULL);}

    else if ((dp->fp.format == NULL) && (dp->fix.order != NO_ORDER))
       id = SC_type_container_size(KIND_INT, size);

    type = SC_type_name(id);
    ndp  = (type != NULL) ? PD_inquire_host_type(file, type) : NULL;

    return(ndp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_ITEMS_PER_TUPLE - return the number of items in a tuple */

int _PD_items_per_tuple(defstr *dp)
   {int ipt;
    multides *tuple;

    ipt = 1;

    if (dp != NULL)
       {tuple = dp->tuple;
	if (tuple != NULL)
	   ipt = tuple->ni;};

    return(ipt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_GET_CURRENT_ADDRESS - return the current file address */

int64_t _PD_get_current_address(const PDBfile *file, PD_major_op tag)
   {int64_t addr;
    FILE *fp;

    fp = file->stream;

    addr = lio_tell(fp);
    if ((addr == -1) && (tag != PD_UNINIT))
       PD_error("FAILED TO FIND ADDRESS - _PD_GET_CURRENT_ADDRESS", tag);

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SET_CURRENT_ADDRESS - set the current file address */

int _PD_set_current_address(PDBfile *file, int64_t addr, int wh,
			    PD_major_op tag)
   {int st;
    FILE *fp;

    fp = file->stream;

    st = lio_seek(fp, addr, wh);
    if ((st != 0) && (tag != PD_UNINIT))
       PD_error("FAILED TO FIND ADDRESS - _PD_SET_CURRENT_ADDRESS", tag);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_GET_NEXT_ADDRESS - reserve the appropriate amount of space in FILE
 *                      - defined by TYPE, NI, and VR
 *                      - and return the starting address of it
 *                      - in threaded mode seek to the appropriate
 *                      - place in the file iff SEEKF is TRUE
 *                      - in non-threaded mode use tell to specify
 *                      - the available address iff TELLF is TRUE
 *                      - in MPI mode get collective space iff COLF is TRUE
 */

int64_t _PD_get_next_address(PDBfile *file, const char *type, inti ni,
			     const void *vr, int seekf, int tellf, int colf)
   {int64_t addr;

    if (ni == 0)
       addr = -1;

    else
       addr = _PD_NEXT_ADDRESS(file, type, ni, vr, seekf, tellf, colf);

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_EOD - mark the end of data in the file
 *         - that is set the chart address to the current file position
 */

int64_t _PD_eod(PDBfile *file)
   {int64_t addr;

    addr = _PD_GETSPACE(file, 0, FALSE, 0);

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_ENTRY_SET_ADDRESS - Set the disk address of the first block of
 *                      - the symbol table entry EP to ADDR.
 *                      - Return the disk address ADDR.
 *
 * #bind PD_entry_set_address fortran() scheme(pd-entry-set-address!) python()
 */

int64_t PD_entry_set_address(syment *ep, int64_t addr)
   {

    _PD_entry_set_address(ep, 0, addr);

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_ENTRY_ADDRESS - Return the disk address of the first block of
 *                  - the symbol table entry EP.
 *
 * #bind PD_entry_address fortran() scheme() python()
 */

int64_t PD_entry_address(const syment *ep)
   {int64_t addr;

    addr = _PD_entry_get_address(ep, 0);

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INIT_CONSTS - first time initializations of some PDBLib stuff */

void _PD_init_consts(void)
   {

    ONCE_SAFE(TRUE, NULL)
       LAST  = CPMAKE(int, 3);
       *LAST = 0;

        G_register_score_types();
        G_register_pml_types();
        G_register_pdb_types();
    END_SAFE;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INIT_TYPEDEFS - initialize the standard typedefs for FILE */

static void _PD_init_typedefs(PDBfile *file)
   {int id, nt;
    type_desc *td, *ta;

/* add PDB_SYSTEM_VERSION <= 30 PDB types for backward compatibility */
    PD_typedef(file, "unsigned char",        "u_char");
    PD_typedef(file, "wchar_t",              "wchar");
    PD_typedef(file, "unsigned wchar_t",     "u_wchar");

    PD_typedef(file, "unsigned int8_t",      "u_int8_t");
    PD_typedef(file, "unsigned short",       "u_short");
    PD_typedef(file, "unsigned int",         "u_int");
    PD_typedef(file, "unsigned long",        "u_long");
    PD_typedef(file, "long long",            "long_long");
    PD_typedef(file, "unsigned long long",   "u_long_long");
    PD_typedef(file, "unsigned int",         "u_integer");

    PD_typedef(file, "long double",          "long_double");

    PD_typedef(file, "float _Complex",       "float_complex");
    PD_typedef(file, "double _Complex",      "double_complex");
    PD_typedef(file, "long double _Complex", "long_double_complex");

/* add the standard C type aliases */
    nt = _SC.types.nstandard;
    for (id = 0; id < nt; id++)
        {td = SC_gs.stl + id;
	 if ((td->alias != NULL) && (td->g != KIND_POINTER))
	    {ta = _SC_get_type_name(td->alias);
	     PD_typedef(file, ta->type, td->type);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INIT_CHRT - initialize the charts with the primitives
 *               - NOTE: define both int and integer!!!!!!!!!!!!!!!!!!!!
 *               - if file types are unknown (FTK) then we cannot determine
 *               - whether or not file types need conversion
 */

void _PD_init_chrt(PDBfile *file, int ftk)
   {hasharr *fchrt, *hchrt;
    defstr *ret, *dp;
    data_standard *fstd, *hstd;
    data_alignment *falign, *halign;

/* one time initializations */
    _PD_init_consts();

    fchrt  = file->chart;
    fstd   = file->std;
    falign = file->align;

    hchrt  = file->host_chart;
    hstd   = file->host_std;
    halign = file->host_align;

    _PD_setup_chart(fchrt, fstd, hstd, falign, halign, PD_CHART_FILE, ftk);
    _PD_setup_chart(hchrt, hstd, NULL, halign, NULL, PD_CHART_HOST, ftk);

    _PD_init_typedefs(file);

/* NOTE: function MUST be handled this way - PD_DEFNCV does NOT
 *       sequence the two charts properly and death can result
 *       when reading on a different machine, (e.g. 32 bit pointer
 *       in file and 64 bit pointer in host
 */
    ret = PD_inquire_type(file, "function");
    if (ret == NULL)
       {ret = PD_inquire_type(file, "*");
	if (ret != NULL)
	   {dp  = _PD_mk_defstr(fchrt, "function", KIND_OTHER,
				NULL, NULL,
				ret->size, ret->alignment, ret->fix.order,
				ret->convert, NULL, NULL, FALSE, FALSE);
	    if (dp == NULL)
	       PD_error("FILE FUNCTION DEFINITION FAILED - _PD_INIT_CHART",
			PD_OPEN);

	    _PD_d_install(file, "function", dp,
			  PD_CHART_FILE, NULL);};};

    ret = PD_inquire_host_type(file, "*");
    if (ret != NULL)
       {dp  = _PD_mk_defstr(hchrt, "function", KIND_OTHER,
			    NULL, NULL,
			    ret->size, ret->alignment, ret->fix.order,
			    ret->convert, NULL, NULL, FALSE, FALSE);
	if (dp == NULL)
	   PD_error("HOST FUNCTION DEFINITION FAILED - _PD_INIT_CHART",
		    PD_OPEN);

	_PD_d_install(file, "function", dp, PD_CHART_HOST, NULL);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SETUP_CHART - setup a structure chart with the conversions selected
 *                 - and the types installed
 */

void _PD_setup_chart(hasharr *chart, data_standard *fstd, data_standard *hstd,
		     data_alignment *falign, data_alignment *halign,
		     PD_chart_kind chk, int ftk)
   {int ic, id, ifx, ifp, conv, flag;
    int fcnv[N_PRIMITIVE_FP];
    multides *tup;
    type_desc *td;

    flag = (hstd != NULL);

    _PD_defstr_in(chart, "*", KIND_POINTER, NULL, NULL, fstd->ptr_bytes, 
                  falign->ptr_alignment, fstd->fx[1].order,
		  TRUE, NULL, NULL, 0, 0, chk, "void *");

    if (flag == TRUE)
       {if (ftk == FALSE)
	   conv = -1;
        else
	   PD_COMPARE_BOOL_STD(conv, fstd, hstd, falign, halign);}
    else
       conv = FALSE;
    _PD_defstr_in(chart, G_BOOL_S, KIND_INT,
		  NULL, NULL, fstd->bool_bytes,
		  falign->bool_alignment, NO_ORDER,
		  conv, NULL, NULL, FALSE, FALSE, chk, NULL);

/* character types (proper) */
    for (ic = 0; ic < N_PRIMITIVE_CHAR; ic++)
        {id = SC_TYPE_CHAR_ID(ic);
	 td = SC_gs.stl + id;

	 conv = _PD_compare_char_std(ic, fstd, hstd, falign, halign,
				     flag, ftk);

	 _PD_defstr_in(chart, td->type, KIND_CHAR,
		       NULL, NULL, fstd->chr[ic].bpi, 
		       falign->chr[ic], NO_ORDER,
		       conv, NULL, NULL, FALSE, FALSE, chk, NULL);

	 _PD_defstr_in(chart, td->utype, KIND_CHAR,
		       NULL, NULL, fstd->chr[ic].bpi, 
		       falign->chr[ic], NO_ORDER,
		       conv, NULL, NULL, TRUE, FALSE, chk, NULL);};

/* fixed point types (proper) */
    for (ifx = 0; ifx < N_PRIMITIVE_FIX; ifx++)
        {id = SC_TYPE_FIX_ID(ifx);
	 td = SC_gs.stl + id;

	 conv = _PD_compare_fix_std(ifx, fstd, hstd, falign, halign,
				    flag, ftk);
	 _PD_defstr_in(chart, td->type, KIND_INT,
		       NULL, NULL, fstd->fx[ifx].bpi, 
		       falign->fx[ifx], fstd->fx[ifx].order,
		       conv, NULL, NULL, FALSE, FALSE, chk, NULL);

	 _PD_defstr_in(chart, td->utype, KIND_INT,
		       NULL, NULL, fstd->fx[ifx].bpi, 
		       falign->fx[ifx], fstd->fx[ifx].order,
		       conv, NULL, NULL, TRUE, FALSE, chk, NULL);};

/* floating point types (proper) */
    for (ifp = 0; ifp < N_PRIMITIVE_FP; ifp++)
        {id = SC_TYPE_FP_ID(ifp);
	 td = SC_gs.stl + id;

	 fcnv[ifp] = _PD_compare_fp_std(ifp, fstd, hstd, falign, halign,
					flag, ftk);
	 _PD_defstr_in(chart, td->type, KIND_FLOAT,
		       NULL, NULL, fstd->fp[ifp].bpi, 
		       falign->fp[ifp], NO_ORDER,
		       fcnv[ifp], fstd->fp[ifp].order,
		       fstd->fp[ifp].format, FALSE, FALSE, chk, NULL);};

/* complex floating point types (proper) */
    for (ifp = 0; ifp < N_PRIMITIVE_FP; ifp++)
        {id = SC_TYPE_CPX_ID(ifp);
	 td = SC_gs.stl + id;

	 tup = _PD_make_tuple(td->comp, 2, NULL);
	 _PD_defstr_in(chart, td->type, KIND_FLOAT,
		       NULL, tup, fstd->fp[ifp].bpi, 
		       falign->fp[ifp], NO_ORDER,
		       fcnv[ifp], fstd->fp[ifp].order,
		       fstd->fp[ifp].format, FALSE, FALSE, chk, NULL);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DEF_REAL - get the miserable, pesky type REAL in once and for all!! */

void _PD_def_real(PDBfile *file, const char *fty)
   {

    if (strcmp(fty, G_PDBFILE_S) == 0)
       {PD_typedef_primitive_types(file);
	_PD_init_typedefs(file);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_REV_CHRT - reverse the list of structures in the single element hash
 *              - table that makes up the chart
 */

int _PD_rev_chrt(hasharr *ch)
   {

    SC_REVERSE_LIST(haelem, *(ch->table), next);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_GET_ERROR - Return the current PDBLib error message for
 *              - the current thread.
 *
 * #bind PD_get_error fortran() scheme() python()
 */

char *PD_get_error(void)
   {PD_smp_state *pa;

    pa = _PD_get_state(-1);

    return(pa->err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_GET_BUFFER_SIZE - Return the current global PDBLib buffer size.
 *
 * #bind PD_get_buffer_size fortran() scheme() python()
 */

int64_t PD_get_buffer_size(void)
   {int64_t rv;
    PD_smp_state *pa;

    _PD_init_state(FALSE);

    pa = _PD_get_state(-1);

    if (pa != NULL)
       rv = pa->buffer_size;
    else
       rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_SET_BUFFER_SIZE - Set the global PDBLib buffer size to V.
 *                    - Return the buffer size.
 *
 * #bind PD_set_buffer_size fortran() scheme(pd-set-buffer-size!) python()
 */

int64_t PD_set_buffer_size(int64_t v)
   {PD_smp_state *pa;
    
    _PD_init_state(FALSE);

    pa = _PD_get_state(-1);

    if (pa != NULL)
       pa->buffer_size = v;

    SC_set_buffer_size(v);

    return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_GET_SYSTEM_VERSION - Return the PDB system version of PDBfile FILE.
 *
 * #bind PD_get_system_version fortran() scheme() python()
 */

int PD_get_system_version(const PDBfile *file ARG(,,cls))
   {int rv;

    if (file != NULL)
       rv = file->system_version;
    else
       rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_GET_FILE_LENGTH - Return the current length of PDBfile FILE.
 *
 * #bind PD_get_file_length fortran() scheme() python()
 */

int64_t PD_get_file_length(const PDBfile *file ARG(,,cls))
   {int64_t rv;

    rv = _PD_GET_FILE_SIZE(file);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_SET_TEXT_DELIMITER - set file->delim
 *                       - if D is NULL return the current value
 */

char *PD_set_text_delimiter(PDBfile *file, char *d)
   {char *rv;
    
    if (d != NULL)
       {CFREE(file->delim);

	file->delim = CSTRSAVE(d);};

    rv = file->delim;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_GET_ENTRY_INFO - Return the type, number of items, number of dimensions
 *                   - and dimensions of the symbol table entry EP.
 *                   - Return the type in PTYP, the number of items in PNI,
 *                   - the number of dimensions in PND, and the dimensions
 *                   - in PDIM.
 *                   - Return TRUE if successful and FALSE otherwise.
 *
 * #bind PD_get_entry_info fortran() scheme() python()
 */

int PD_get_entry_info(syment *ep, char **ptyp, long *pni,
		      int *pnd, long **pdim)
   {int rv;
    long nd;
    long *dims;
    dimdes *pd;

    if (ep != NULL)
       {*ptyp = CSTRSAVE(PD_entry_type(ep));
	*pni  = PD_entry_number(ep);

/* count dimensions */
	for (nd = 0, pd = PD_entry_dimensions(ep);
	     pd != NULL;
	     nd++, pd = pd->next);

	*pnd = nd;
	dims = CMAKE_N(long, 2*nd);

/* copy dimensions */
	for (nd = 0, pd = PD_entry_dimensions(ep);
	     pd != NULL;
	     pd = pd->next)
            {dims[nd++] = pd->index_min;
	     dims[nd++] = pd->index_max;};

	*pdim = dims;

        rv = TRUE;}

    else
       {*ptyp = NULL;
	*pni  = -1;
	*pnd  = -1;
	*pdim = NULL;

	rv = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_FREE_ENTRY_INFO - Free TYP and PDIM, values allocated by PD_get_entry.
 *
 * #bind PD_free_entry_info fortran() scheme() python()
 */

void PD_free_entry_info(char *typ, long *pdim)
   {

    CFREE(typ);
    CFREE(pdim);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_REL_ENTRY_INFO - Free EP, TYP, and PDIM, values
 *                   - allocated by PD_get_entry.
 *
 * #bind PD_rel_entry_info fortran() scheme() python()
 */

void PD_rel_entry_info(syment *ep, char *typ, long *pdim)
   {

    PD_free_entry_info(typ, pdim);

    _PD_rl_syment(ep);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SET_STANDARD - set the data_standard and data_alignment of FILE */

void _PD_set_standard(PDBfile *file, data_standard *std, data_alignment *algn)
   {char *sval;
    SC_udl *pu;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    pu = file->udl;

    if (std == NULL)
       std = file->host_std;

    if (algn == NULL)
       algn = file->host_align;

    if (pu != NULL)
       sval = SC_assoc(pu->info, "tgt");
    else
       sval = NULL;

/* TEXT standard */
    if ((sval != NULL) && (strcmp(sval, "txt") == 0))
       {int nv;

/* must be format version 3 or higher */
	nv = PD_get_fmt_version();
	nv = max(nv, 3);
	PD_set_fmt_version(nv);

/* set the TEXT standard */
	file->std   = _PD_copy_standard(&TEXT_STD);
	file->align = _PD_copy_alignment(&TEXT_ALIGNMENT);}

/* default to host standard */
    else
       {file->std   = _PD_copy_standard(std);
	file->align = _PD_copy_alignment(algn);};

    if (pa->req_std != NULL)
       {if (!_PD_compare_std(pa->req_std, file->std,
                             pa->req_align, file->align))
           {_PD_rl_standard(file->std);
	    file->std   = _PD_copy_standard(pa->req_std);
	    _PD_rl_alignment(file->align);
            file->align = _PD_copy_alignment(pa->req_align);};

        pa->req_std = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CREATE - worker to create a PDB file
 *            - and return an initialized PDBfile
 *            - if successful else NULL
 */

PDBfile *_PD_create(tr_layer *tr, SC_udl *pu, char *name, char *mode, void *a)
   {int ok;
    FILE *fp;
    PDBfile *file;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);
    fp = pu->stream;

    _PD_set_io_buffer(pu);

/* make the PDBfile */
    file = _PD_mk_pdb(pu, NULL, NULL, TRUE, NULL, tr);
    if (file != NULL)
       {file->mode = PD_CREATE;
    
	_PD_symt_set_delay_mode(file, mode);

	if (a != NULL)
	   _PD_SETUP_MP_FILE(file, *(SC_communicator *) a);

	_PD_set_standard(file, NULL, NULL);

	pa->cfp = fp;

	ok = (*file->create)(file, mode, _PD_IS_MASTER(file));
	if (ok != TRUE)
	   PD_error("FAILED TO CREATE FILE - _PD_CREATE", PD_CREATE);};

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DATA_SOURCE - return a FILE from PU if it exists */

FILE *_PD_data_source(SC_udl *pu)
   {FILE *fp;

    fp = NULL;
    if (pu != NULL)
       {fp = pu->stream;
	if (_PD_IS_NULL_FP(fp) == TRUE)
	   fp = NULL;};

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CLOSE - close a PDB file after writing out the symbol table and
 *           - structure chart and completing the header
 *           - return 1 if successful, -1 on bad checksum,
 *           - and 0 otherwise
 */

int64_t _PD_close(PDBfile *file)
   {int ok, st;
    int64_t ret;
    FILE *fp;

    ret = FALSE;

    if (file != NULL)
       {ok = TRUE;

	ok &= _PD_cksum_close(file);

/* position the file pointer at the greater of the current position and
 * the location of the chart
 */
	if ((file->mode == PD_CREATE) || (file->mode == PD_APPEND))
	   ret = PD_flush(file);
	else
	   ret = PD_get_file_length(file);

	if (file->virtual_internal == FALSE)
	   {if ((ret > 0) && (ok == TRUE))
	       {st  = _PD_filt_close(file);
		ok &= (st != 0);};};

	fp = file->stream;
	if (fp != NULL)
	   {if (lio_close(fp) != 0)
	       PD_error("CANNOT CLOSE FILE - PD_CLOSE", PD_CLOSE);

/* GOTCHA: double free - avoid */
	    file->stream      = NULL;
	    file->udl->stream = NULL;};

/* free the space */
	_PD_rl_pdb(file);

	ret = (ok == TRUE) ? ret : -1;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_OPEN - worker to setup PDB files
 *          - NOTE: MODE is the PDBLib mode - "r", "w", or "a"
 *          - these are different than the C fopen modes  - "r+b", etc
 *          - the C modes are contained in pu->mode
 */

PDBfile *_PD_open(tr_layer *tr, SC_udl *pu, char *name, char *mode, void *a)
   {int vers, ok;
    PDBfile *file;
    PD_smp_state *pa;
    FILE *fp;

    pa = _PD_get_state(-1);
    fp = _PD_data_source(pu);

    _PD_set_io_buffer(pu);

    file = _PD_mk_pdb(pu, NULL, NULL, TRUE, NULL, tr);
    if (file == NULL)
       PD_error("CANNOT ALLOCATE PDBFILE - _PD_OPEN", PD_OPEN);

    pa->ofp = fp;

    if (*mode == 'a')
       file->mode = PD_APPEND;
    else
       file->mode = PD_OPEN;

    _PD_symt_set_delay_mode(file, mode);

    _PD_MARK_AS_FLUSHED(file, TRUE);

    if (a != NULL)
       _PD_SETUP_MP_FILE(file, *(SC_communicator *) a);

    vers = -1;

    ok = _PD_filt_file_in(file);
    if (ok == TRUE)
       {vers = _PD_identify_file(file);
	if (vers > 0)
	   {ok = (*file->open)(file, mode);
	    if (ok != TRUE)
	       PD_error("CANNOT OPEN FILE - _PD_OPEN", PD_OPEN);};};

    if (vers <= 0)
       {file->udl = NULL;
	_PD_rl_pdb(file);
	file = NULL;};

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_REPLACE_FILE - replace the file associated with FILE with NAME
 *                  - and position the new file at ADDR
 *                  - used primarily with filter chains
 */

void _PD_replace_file(PDBfile *file, const char *name, int64_t addr)
   {SC_udl *pu;
    PD_smp_state *pa;
    FILE *fp;

    pa = _PD_get_state(-1);

    _SC_rel_udl(file->udl);

    rename(name, file->name);

/* switch all the file references to the new file */
    pu = _PD_pio_open(file->name, "r+b");
    fp = _PD_data_source(pu);

    file->udl    = pu;
    file->stream = fp;
    pa->ofp      = fp;

    _PD_set_io_buffer(pu);

    lio_seek(fp, addr, SEEK_SET);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_REQUEST_UNSET - reset I/O request in FILE */

void _PD_request_unset(PDBfile *file)
   {io_request *req;

    if (file != NULL)
       {req = &file->req;

	req->oper = PD_UNINIT;

#ifdef USE_REQUESTS
/*
	SC_mark(req->ftype, -1);
	req->ftype = NULL;

	SC_mark(req->htype, -1);
	req->htype = NULL;
*/
#endif
	
	CFREE(req->base_name);
	CFREE(req->base_type);};

    return;}

/*--------------------------------------------------------------------------*/

/*                            FILE INFO ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* PD_GET_MODE - Return the file mode of PDBfile FILE.
 *
 * #bind PD_get_mode fortran() scheme() python()
 */

PD_major_op PD_get_mode(const PDBfile *file ARG(,,cls))
   {PD_major_op rv;

    if (file != NULL)
       rv = file->mode;
    else
       rv = PD_GENERIC;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_SET_MODE - Set the file mode of PDBfile FILE to V
 *             - and return the old file mode.
 *
 * #bind PD_set_mode fortran() scheme(pd-set-mode!) python()
 */

PD_major_op PD_set_mode(PDBfile *file ARG(,,cls), PD_major_op v)
   {PD_major_op rv;

    if (file != NULL)
       {rv = file->mode;
	file->mode = v;}
    else
       rv = PD_GENERIC;

    return(rv);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_GET_OFFSET - Return the file default offset of PDBfile FILE.
 *
 * #bind PD_get_offset fortran() scheme() python()
 */

int PD_get_offset(const PDBfile *file ARG(,,cls))
   {int rv;

    if (file != NULL)
       rv = file->default_offset;
    else
       rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_SET_OFFSET - Set the file default offset of PDBfile FILE to V
 *               - and return the old offset.
 *
 * #bind PD_set_offset fortran() scheme(pd-set-offset!) python()
 */

int PD_set_offset(PDBfile *file ARG(,,cls), int v)
   {int rv;

    if (file != NULL)
       {rv = file->default_offset;
	file->default_offset = v;}
    else
       rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_GET_MAJOR_ORDER - Return the file major_order of PDBfile FILE.
 *
 * #bind PD_get_major_order fortran() scheme() python()
 */

PD_major_order PD_get_major_order(const PDBfile *file ARG(,,cls))
   {PD_major_order rv;

    if (file != NULL)
       rv = file->major_order;
    else
       rv = NO_MAJOR_ORDER;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_SET_MAJOR_ORDER - Set the file major_order of PDBfile FILE to V
 *                    - and return the old value.
 *
 * #bind PD_set_major_order fortran() scheme(pd-set-major_order!) python()
 */

PD_major_order PD_set_major_order(PDBfile *file ARG(,,cls), PD_major_order v)
   {PD_major_order rv;

    if (file != NULL)
       {rv = file->major_order;
	file->major_order = v;}
    else
       rv = NO_MAJOR_ORDER;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_GET_TRACK_POINTERS - Return the pointer tracking flag of PDBfile FILE.
 *
 * #bind PD_get_track_pointers fortran() scheme() python()
 */

int PD_get_track_pointers(const PDBfile *file ARG(,,cls))
   {int rv;

    if (file != NULL)
       rv = file->track_pointers;
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_SET_TRACK_POINTERS - Set the pointer tracking flag of PDBfile FILE to V
 *                       - and return the old value.
 *
 * #bind PD_set_track_pointers fortran() scheme(pd-set-track_pointers!) python()
 */

int PD_set_track_pointers(PDBfile *file ARG(,,cls), int v)
   {int rv;

    if (file != NULL)
       {rv = file->track_pointers;
	file->track_pointers = v;}
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_GET_MAX_FILE_SIZE - Return the maximum file size of PDBfile FILE.
 *
 * #bind PD_get_max_file_size fortran() scheme() python()
 */

int64_t PD_get_max_file_size(const PDBfile *file ARG(,,cls))
   {int64_t rv;

    if (file != NULL)
       rv = file->maximum_size;
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_SET_MAX_FILE_SIZE - Set the maximum file size of PDBfile FILE to V
 *                      - and return the old value.
 *
 * #bind PD_set_max_file_size fortran() scheme(pd-set-max_file_size!) python()
 */

int64_t PD_set_max_file_size(PDBfile *file ARG(,,cls), int64_t v)
   {int64_t rv;

    if (file != NULL)
       {rv = file->maximum_size;
	file->maximum_size = v;}
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_GET_FMT_VERSION - Return the format version to be used when
 *                    - creating subsequent PDBfiles.
 *
 * #bind PD_get_fmt_version fortran() scheme() python()
 */

int PD_get_fmt_version(void)
   {int rv;

    rv = PD_gs.default_format_version;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_SET_FMT_VERSION - Set the format version to be used when
 *                    - creating subsequent PDBfiles.
 *                    - Return the old value.
 *
 * #bind PD_set_fmt_version fortran() scheme(pd-set-fmt_version!) python()
 */

int PD_set_fmt_version(int v)
   {int rv;

    rv = PD_gs.default_format_version;
    PD_gs.default_format_version = v;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_GET_POINTER_SIZE - Return the global value of the initial size of the
 *                     - pointer table in PDBfiles.
 *
 * #bind PD_get_pointer_size fortran() scheme(pd-get-pointer-size) python()
 */

long PD_get_pointer_size(void)
   {long no;

    no = _PD.ninc;

    return(no);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_SET_POINTER_SIZE - Set the global value of the initial size of the
 *                     - pointer table in PDBfiles to N.
 *
 * #bind PD_set_pointer_size fortran() scheme(pd-set-pointer-size!) python()
 */

long PD_set_pointer_size(long n)
   {long no;

    no = _PD.ninc;
    _PD.ninc = n;

    return(no);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_GET_FILE_NAME - Return the file name of PDBfile FILE.
 *
 * #bind PD_get_file_name fortran() scheme() python()
 */

char *PD_get_file_name(const PDBfile *file ARG(,,cls))
   {char *rv;

    if (file != NULL)
       rv = file->name;
    else
       rv = NULL;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_ENTRY_TYPE - Return the type of the symbol table entry EP.
 *
 * #bind PD_entry_type fortran() scheme() python()
 */

char *PD_entry_type(const syment *ep)
   {char *rv;

    if (ep != NULL)
       rv = ep->type;
    else
       rv = NULL;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_ENTRY_NUMBER - Return the number of items of the symbol table entry EP.
 *
 * #bind PD_entry_number fortran() scheme() python()
 */

int PD_entry_number(const syment *ep)
   {int rv;

    if (ep != NULL)
       rv = ep->number;
    else
       rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_ENTRY_N_DIMENSIONS - Return the number of dimensions of the
 *                       - symbol table entry EP.
 *
 * #bind PD_entry_n_dimensions fortran() scheme() python()
 */

int PD_entry_n_dimensions(const syment *ep)
   {int nd;
    dimdes *pd;

    if (ep != NULL)
       {for (nd = 0, pd = PD_entry_dimensions(ep);
	     pd != NULL;
	     pd = pd->next, nd++);}
    else
       nd = 0;

    return(nd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_ENTRY_DIMENSIONS - Return the dimensions of the symbol table entry EP.
 *
 * #bind PD_entry_dimensions fortran() scheme() python()
 */

dimdes *PD_entry_dimensions(const syment *ep)
   {dimdes *rv;

    if (ep != NULL)
       rv = ep->dimensions;
    else
       rv = NULL;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_TYPE_SIZE - Return the byte size of the type specified by DP.
 *
 * #bind PD_type_size fortran() scheme() python()
 */

int PD_type_size(defstr *dp)
   {int rv;

    if (dp != NULL)
       rv = dp->size;
    else
       rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_TYPE_ALIGNMENT - Return the byte alignment of the type specified by DP.
 *
 * #bind PD_type_alignment fortran() scheme() python()
 */

int PD_type_alignment(defstr *dp)
   {int rv;

    if (dp != NULL)
       rv = dp->alignment;
    else
       rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_TYPE_N_INDIRECTS - Return number of indirects of the
 *                     - type specified by DP.
 *
 * #bind PD_type_n_indirects fortran() scheme() python()
 */

int PD_type_n_indirects(defstr *dp)
   {int rv;

    if (dp != NULL)
       rv = dp->n_indirects;
    else
       rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
