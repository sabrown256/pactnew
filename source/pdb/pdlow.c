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

char
 *PD_ALIGNMENT_S = NULL,
 *PD_DEFSTR_S    = NULL,
 *PD_STANDARD_S  = NULL,
 *SYMENT_P_S     = NULL,
 *SYMENT_S       = NULL;

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
		for (j = 0; j < FORMAT_FIELDS; j++)
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

    for (i = 0; i < PD_N_PRIMITIVE_FIX; i++)
        eq &= ((a->fx[i].bpi   == b->fx[i].bpi) &&
	       (a->fx[i].order == b->fx[i].order));

    for (i = 0; i < PD_N_PRIMITIVE_FP; i++)
        eq &= (a->fp[i].bpi == b->fp[i].bpi);

    if (!eq)
       return(FALSE);

/* check the floating point byte orders */
    for (i = 0; i < PD_N_PRIMITIVE_FP; i++)
        {n  = a->fp[i].bpi;
	 oa = a->fp[i].order;
	 ob = b->fp[i].order;
	 for (j = 0; j < n; j++, eq &= (*(oa++) == *(ob++)));};

/* check the floating point format data */
    n  = FORMAT_FIELDS;
    for (i = 0; i < PD_N_PRIMITIVE_FP; i++)
        {fa = a->fp[i].format;
	 fb = b->fp[i].format;
	 for (j = 0; j < n; j++, eq &= (*(fa++) == *(fb++)));};

/* check alignments */
    eq &= ((c->char_alignment == d->char_alignment) &&
           (c->ptr_alignment  == d->ptr_alignment) &&
           (c->bool_alignment == d->bool_alignment));

    for (i = 0; i < PD_N_PRIMITIVE_FIX; i++)
        eq &= (c->fx[i] == d->fx[i]);

    for (i = 0; i < PD_N_PRIMITIVE_FP; i++)
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
    if (pa->buffer_size != -1)
       {fp = pu->stream;
	bf = FMAKE_N(char, sz, "_PD_SET_IO_BUFFER:bf");

	if (lio_setvbuf(fp, bf, _IOFBF, sz) != 0)
	   {SFREE(bf);
	    PD_error("CANNOT SET FILE BUFFER - _PD_SET_IO_BUFFER", PD_OPEN);}
	else
	   {if (pu->buffer != NULL)
	       SFREE(pu->buffer);
	    pu->buffer = bf;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PIO_OPEN - do an open in a parallel context */

SC_udl *_PD_pio_open(char *name, char *mode)
   {char *s, *md;
    FILE *fp;
    PFfopen opn;
    SC_udl *pu;

    opn = PD_par_fnc.open_hook;
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
	    pu->mode   = SC_strsavef(md, "char*:_PD_PIO_OPEN:md");
	    _PD_set_io_buffer(pu);};};

    return(pu);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SAFE_FLUSH - do a flush if it is safe to do so
 *                - ANSI standard says fflush on output stream only
 *                - so do not try it on a read only stream
 *                - return TRUE iff successful
 */

int _PD_safe_flush(PDBfile *file)
   {int ok, rv;
    FILE *fp;
    SC_THREAD_ID(_t_index);

    rv = TRUE;
    if ((file->virtual_internal == FALSE) &&
        (strcmp(file->file_mode, BINARY_MODE_R) != 0))
       {fp = file->stream;
        ok = _PD_SERIAL_FLUSH(fp, _t_index);
        rv = (ok == 0);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_INQUIRE_TYPE - lookup and return the file chart entry for NAME */

defstr *PD_inquire_type(PDBfile *file, char *name)
   {defstr *dp;

    if (_PD_indirection(name) == TRUE)
       dp = (defstr *) SC_hasharr_def_lookup(file->chart, "*");
    else
       dp = (defstr *) SC_hasharr_def_lookup(file->chart, name);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_INQUIRE_HOST_TYPE - lookup and return the host chart entry for NAME */

defstr *PD_inquire_host_type(PDBfile *file, char *name)
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

void _PD_e_install(PDBfile *file, char *name, syment *ep, int lookup)
   {hasharr *tab;

    tab = file->symtab;

/* we can leak a lot of memory if we don't check this!! */
    if (lookup)
       PD_remove_entry(file, name);

    SC_hasharr_install(tab, name, ep, SYMENT_P_S, TRUE, FALSE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_D_INSTALL_IN - install a defstr pointer in the given chart 
 *                  - (should be used in this file only)
 */

static void _PD_d_install_in(char *name, defstr *def, hasharr *tab)
   {defstr *dp;
    haelem *hp;

/* we can leak a lot of memory if we don't check this
 * PURIFY complains of free memory read if hp->def not nulled before SC_hasharr_remove
 */
    hp = SC_hasharr_lookup(tab, name);
    dp = (hp != NULL) ? (defstr *) hp->def : NULL;
    if (dp != NULL)
       {_PD_rl_defstr(dp);
	hp->def = NULL;
	SC_hasharr_remove(tab, name);};

    SC_hasharr_install(tab, name, def, PD_DEFSTR_S, TRUE, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_D_INSTALL - install a defstr pointer in the given chart of the file */

void _PD_d_install(PDBfile *file, char *name, defstr *def, int host)
   {

    if (host)
       {_PD_d_install_in(name, def, file->host_chart);
	SC_register_type(name, def->size, NULL);}
    else
       _PD_d_install_in(name, def, file->chart);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DEFSTR_IN - primitive defstruct used to install the primitive types
 *               - in the specified chart (should be used in this file only)
 */

static defstr *_PD_defstr_in(hasharr *chart, char *name, PD_type_kind kind,
			     memdes *desc, multides *tuple,
                             long sz, int align,
			     PD_byte_order ord, int conv,
			     int *ordr, long *formt, int unsgned, int onescmp)
   {defstr *dp;

    dp = _PD_mk_defstr(chart, name, kind,
		       desc, tuple,
		       sz, align, ord, conv,
		       ordr, formt, unsgned, onescmp);
    if (dp == NULL)
       PD_error("DEFINITION FAILED - _PD_DEFSTR_IN", PD_GENERIC);
    else
       _PD_d_install_in(name, dp, chart);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DEFSTR - primitive define struct used to install the primitive types
 *            - in the specified chart set (both hashed and non-hashed versions) 
 */

defstr *_PD_defstr(PDBfile *file, int host, char *name, PD_type_kind kind,
		   memdes *desc, multides *tuple,
		   long sz, int align, PD_byte_order ord,
		   int conv, int *ordr, long *formt, int unsgned, int onescmp)
   {defstr *dp;

    if (host)
       dp = _PD_defstr_in(file->host_chart, name, kind,
			  desc, tuple, sz, align, ord, conv,
			  ordr, formt, unsgned, onescmp);
    else
       dp = _PD_defstr_in(file->chart, name, kind,
			  desc, tuple, sz, align, ord, conv,
			  ordr, formt, unsgned, onescmp);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DEFSTR_INST - install the defstr in all charts
 *                 - if FLAG is TRUE return the file defstr
 *                 - if FLAG is FALSE return the file host_chart defstr
 */

defstr *_PD_defstr_inst(PDBfile *file, char *name, PD_type_kind kind,
			memdes *desc, PD_byte_order ord,
			int *ordr, long *formt, int flag)
   {int algn, conv;
    long sz;
    defstr *rv, *dp, *sdp;
    memdes *pd, *memb;
    hasharr *chart, *host_chart;
    data_alignment *align, *host_align;

    chart      = file->chart;
    host_chart = file->host_chart;
    align      = file->align;
    host_align = file->host_align;

    if (desc == NULL)
       return(NULL);

    dp = flag ? PD_inquire_table_type(chart, name) :
                PD_inquire_table_type(host_chart, name);
    if (dp != NULL)
       return(dp);

/* install the type in the file->chart */
    sz   = _PD_str_size(desc, chart);
    conv = FALSE;
    algn = align->struct_alignment;
    for (pd = desc; pd != NULL; pd = pd->next)
        {dp = PD_inquire_table_type(chart, pd->base_type);
         if (_PD_indirection(pd->type) || (dp == NULL))
            {algn = max(algn, align->ptr_alignment);
	     conv = TRUE;}
         else
            {algn  = max(algn, dp->alignment);
             conv |= (dp->convert > 0);};

/* in case we are installing this defstr having read it from
 * another file (as in a copy type operation) redo the cast offsets
 */
         if (pd->cast_memb != NULL)
            pd->cast_offs = _PD_member_location(pd->cast_memb,
						chart,
						dp,
						&memb);};

    dp = _PD_defstr(file, FALSE, name, kind,
		    desc, NULL,
		    sz, algn, ord,
	            conv, ordr, formt, FALSE, FALSE);

/* install the type in the host_chart */
    desc = PD_copy_members(desc);
    sz   = _PD_str_size(desc, host_chart);

    algn = host_align->struct_alignment;
    for (pd = desc; pd != NULL; pd = pd->next)
        {dp = PD_inquire_table_type(host_chart, pd->base_type);
         if (_PD_indirection(pd->type) || (dp == NULL))
            algn = max(algn, host_align->ptr_alignment);
         else
            algn = max(algn, dp->alignment);

/* in case we are installing this defstr having read it from
 * another file (as in a copy type operation) redo the cast offsets
 */
         if (pd->cast_memb != NULL)
            pd->cast_offs = _PD_member_location(pd->cast_memb,
						host_chart,
						dp,
						&memb);};

/* NOTE: ordr, formt, and conv apply only to the file chart
 *       never to the host chart!!!
 *       these are for non-default primitive types which
 *       have no host representation
 */
    sdp = _PD_defstr(file, TRUE, name, kind,
		     desc, NULL,
		     sz, algn, NO_ORDER,
		     FALSE, NULL, NULL, FALSE, FALSE);

    rv = flag ? dp : sdp;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_TYPEDEF_PRIMITIVE_TYPES - make typedefs for user-defined primitives
 *                            - in the file chart
 */

void PD_typedef_primitive_types(PDBfile *file)
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

defstr *_PD_type_container(PDBfile *file, defstr *dp)
   {int i, n, id, bpi;
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

/* floating point types (proper) */
    if (dp->fp.format != NULL)
       {for (i = 0; i < N_PRIMITIVE_FP; i++)
	    {id   = i + SC_FLOAT_I;
	     bpi  = SC_type_size_i(id);
	     type = SC_type_name(id);

	     if (size <= bpi)
	        {ndp = PD_inquire_host_type(file, type);
		 break;};};}

    else if ((dp->fp.format == NULL) && (dp->fix.order != NO_ORDER))
       {if (size <= sizeof(char))
	   ndp = PD_inquire_host_type(file, SC_CHAR_S);
	else if (size <= sizeof(bool))
	   ndp = PD_inquire_host_type(file, SC_BOOL_S);

/* fixed point types (proper) */
	else
	   {for (i = 0; i < N_PRIMITIVE_FIX; i++)
	        {id   = i + SC_SHORT_I;
		 bpi  = SC_type_size_i(id);
		 type = SC_type_name(id);

		 if (size <= bpi)
		    {ndp = PD_inquire_host_type(file, type);
		     break;};};};};

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

int64_t _PD_get_current_address(PDBfile *file, PD_major_op tag)
   {int64_t addr;
    FILE *fp;

    fp = file->stream;

    addr = lio_tell(fp);
    if ((addr == -1) && (tag != -1))
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
    if ((st != 0) && (tag != -1))
       PD_error("FAILED TO FIND ADDRESS - _PD_SET_CURRENT_ADDRESS", tag);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_GET_NEXT_ADDRESS - reserve the appropriate amount of space in FILE
 *                      - defined by TYPE, NUMBER, and VR
 *                      - and return the starting address of it
 *                      - in threaded mode seek to the appropriate
 *                      - place in the file iff SEEKF is TRUE
 *                      - in non-threaded mode use tell to specify
 *                      - the available address iff TELLF is TRUE
 *                      - in MPI mode get collective space iff COLF is TRUE
 */

int64_t _PD_get_next_address(PDBfile *file, char *type, long number,
			   void *vr, int seekf, int tellf, int colf)
   {int64_t addr;

    if (number == 0)
       addr = -1;

    else
       addr = _PD_NEXT_ADDRESS(file, type, number, vr, seekf, tellf, colf);

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

/* PD_ENTRY_SET_ADDRESS - return the address of the first block of EP */

int64_t PD_entry_set_address(syment *ep, int64_t addr)
   {

    _PD_entry_set_address(ep, 0, addr);

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_ENTRY_ADDRESS - return the address of the first block of EP */

int64_t PD_entry_address(syment *ep)
   {int64_t addr;

    addr = _PD_entry_get_address(ep, 0);

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INIT_CONSTS - first time initializations of some PDBLib stuff */

void _PD_init_consts(void)
   {

    if (PD_DEFSTR_S == NULL)
       {LAST        = NMAKE(int, "PERM|_PD_INIT_CHART:LAST");
        *LAST       = 0;

        PD_DEFSTR_S = SC_strsaven("defstr *",
				  "PERM|char*:_PD_INIT_CHRT:defstr");
        SYMENT_P_S  = SC_strsaven("syment *",
				  "PERM|char*:_PD_INIT_CHRT:syment*");
        SYMENT_S    = SC_strsaven("syment",
				  "PERM|char*:_PD_INIT_CHRT:syment");};

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

    _PD_setup_chart(fchrt, fstd, hstd, falign, halign, TRUE, ftk);
    _PD_setup_chart(hchrt, hstd, NULL, halign, NULL, FALSE, ftk);

    if (sizeof(REAL) == sizeof(double))
       PD_typedef(file, SC_DOUBLE_S, "REAL");
    else
       PD_typedef(file, SC_FLOAT_S, "REAL");

/* NOTE: function MUST be handled this way - PD_DEFNCV does NOT
 *       sequence the two charts properly and death can result
 *       when reading on a different machine, (e.g. 32 bit pointer
 *       in file and 64 bit pointer in host
 */
    ret = PD_inquire_type(file, "function");
    if (ret == NULL)
       {ret = PD_inquire_type(file, "*");
	dp  = _PD_mk_defstr(fchrt, "function", NON_CONVERT_KIND,
			    NULL, NULL,
			    ret->size, ret->alignment, ret->fix.order,
			    ret->convert, NULL, NULL, FALSE, FALSE);
	if (dp == NULL)
	   PD_error("FILE FUNCTION DEFINITION FAILED - _PD_INIT_CHART",
		    PD_OPEN);

	_PD_d_install(file, "function", dp, FALSE);};

    ret = PD_inquire_host_type(file, "*");
    dp  = _PD_mk_defstr(hchrt, "function", NON_CONVERT_KIND,
			NULL, NULL,
			ret->size, ret->alignment, ret->fix.order,
			ret->convert, NULL, NULL, FALSE, FALSE);
    if (dp == NULL)
       PD_error("HOST FUNCTION DEFINITION FAILED - _PD_INIT_CHART",
		PD_OPEN);

    _PD_d_install(file, "function", dp, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SETUP_CHART - setup a structure chart with the conversions selected
 *                 - and the types installed
 */

void _PD_setup_chart(hasharr *chart, data_standard *fstd, data_standard *hstd,
		     data_alignment *falign, data_alignment *halign,
		     int flag, int ftk)
   {int ifx, ifp, conv;
    int fcnv[PD_N_PRIMITIVE_FP];
    char utyp[MAXLINE];
    char *styp, *btyp;
    multides *tup;

    flag = (hstd != NULL);

    _PD_defstr_in(chart, "*", INT_KIND, NULL, NULL, fstd->ptr_bytes, 
                  falign->ptr_alignment, fstd->fx[1].order,
		  TRUE, NULL, NULL, 0, 0);

    if (flag == TRUE)
       {if (ftk == FALSE)
	   conv = -1;
        else
	   PD_COMPARE_CHAR_STD(conv, fstd, hstd, falign, halign);}
    else
       conv = FALSE;
    _PD_defstr_in(chart, SC_CHAR_S, CHAR_KIND,
		  NULL, NULL, 1L, falign->char_alignment, 
                  NO_ORDER, conv, NULL, NULL, FALSE, FALSE);
    _PD_defstr_in(chart, "u_char", CHAR_KIND,
		  NULL, NULL, 1L, falign->char_alignment, 
                  NO_ORDER, conv, NULL, NULL, TRUE, FALSE);

    if (flag == TRUE)
       {if (ftk == FALSE)
	   conv = -1;
        else
	   PD_COMPARE_BOOL_STD(conv, fstd, hstd, falign, halign);}
    else
       conv = FALSE;
    _PD_defstr_in(chart, SC_BOOL_S, INT_KIND,
		  NULL, NULL, fstd->bool_bytes,
		  falign->bool_alignment, NO_ORDER,
		  conv, NULL, NULL, FALSE, FALSE);

/* fixed point types (proper) */
    for (ifx = 0; ifx < PD_N_PRIMITIVE_FIX; ifx++)
        {styp = SC_type_name(ifx + SC_SHORT_I);
	 snprintf(utyp, MAXLINE, "u_%s", styp);

	 conv = _PD_compare_fix_std(ifx, fstd, hstd, falign, halign, flag, ftk);
	 _PD_defstr_in(chart, styp, INT_KIND,
		       NULL, NULL, fstd->fx[ifx].bpi, 
		       falign->fx[ifx], fstd->fx[ifx].order,
		       conv, NULL, NULL, FALSE, FALSE);

	 _PD_defstr_in(chart, utyp, INT_KIND,
		       NULL, NULL, fstd->fx[ifx].bpi, 
		       falign->fx[ifx], fstd->fx[ifx].order,
		       conv, NULL, NULL, TRUE, FALSE);};

    ifx  = 1;
    styp = SC_INTEGER_S;
    snprintf(utyp, MAXLINE, "u_%s", styp);

    conv = _PD_compare_fix_std(ifx, fstd, hstd, falign, halign, flag, ftk);
    _PD_defstr_in(chart, styp, INT_KIND,
		  NULL, NULL, fstd->fx[ifx].bpi, 
		  falign->fx[ifx], fstd->fx[ifx].order,
		  conv, NULL, NULL, FALSE, FALSE);

    _PD_defstr_in(chart, utyp, INT_KIND,
		  NULL, NULL, fstd->fx[ifx].bpi, 
		  falign->fx[ifx], fstd->fx[ifx].order,
		  conv, NULL, NULL, TRUE, FALSE);

/* floating point types (proper) */
    for (ifp = 0; ifp < PD_N_PRIMITIVE_FP; ifp++)
        {styp = SC_type_name(ifp + SC_FLOAT_I);

	 fcnv[ifp] = _PD_compare_fp_std(ifp, fstd, hstd, falign, halign, flag, ftk);
	 _PD_defstr_in(chart, styp, FLOAT_KIND,
		       NULL, NULL, fstd->fp[ifp].bpi, 
		       falign->fp[ifp], NO_ORDER,
		       fcnv[ifp], fstd->fp[ifp].order,
		       fstd->fp[ifp].format, FALSE, FALSE);};

/* complex floating point types (proper) */
    for (ifp = 0; ifp < PD_N_PRIMITIVE_FP; ifp++)
        {btyp = SC_type_name(ifp + SC_FLOAT_I);
         styp = SC_type_name(ifp + SC_FLOAT_COMPLEX_I);

	 tup = _PD_make_tuple(btyp, 2, NULL);
	 _PD_defstr_in(chart, styp, FLOAT_KIND,
		       NULL, tup, fstd->fp[ifp].bpi, 
		       falign->fp[ifp], NO_ORDER,
		       fcnv[ifp], fstd->fp[ifp].order,
		       fstd->fp[ifp].format, FALSE, FALSE);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DEF_REAL - get the miserable, pesky type REAL in once and for all!! */

void _PD_def_real(char *type, PDBfile *file)
   {

    if (strcmp(type, PDBFILE_S) == 0)
       {PD_typedef_primitive_types(file);
	if (sizeof(REAL) == sizeof(double))
	   PD_typedef(file, SC_DOUBLE_S, "REAL");
        else
	   PD_typedef(file, SC_FLOAT_S, "REAL");};

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

/* PD_GET_ERROR - fetch PD_err */

char *PD_get_error(void)
   {PD_smp_state *pa;

    pa = _PD_get_state(-1);

    return(pa->err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_GET_BUFFER_SIZE - fetch PD_buffer_size */

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

/* PD_SET_BUFFER_SIZE - set PD_buffer_size */

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

/* PD_GET_FILE_LENGTH - return the current file length */

int64_t PD_get_file_length(PDBfile *file)
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
       {SFREE(file->delim);

	file->delim = SC_strsavef(d, "char*PD_SET_TEXT_DELIMITER:d");};

    rv = file->delim;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_GET_ENTRY_INFO - return the type, number of items, number of dimensions
 *                   - and dimensions
 */

int PD_get_entry_info(syment *ep, char **ptyp, long *pni,
		      int *pnd, long **pdim)
   {int rv;
    long nd;
    long *dims;
    dimdes *pd;

    if (ep != NULL)
       {*ptyp = SC_strsavef(PD_entry_type(ep), "PD_GET_ENTRY_INFO:ptyp");
	*pni  = PD_entry_number(ep);

/* count dimensions */
	for (nd = 0, pd = PD_entry_dimensions(ep);
	     pd != NULL;
	     nd++, pd = pd->next);

	*pnd = nd;
	dims = FMAKE_N(long, 2*nd, "PD_GET_ENTRY_INFO:dims");

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

/* PD_FREE_ENTRY_INFO - free the info allocated by PD_get_entry */

void PD_free_entry_info(char *typ, long *pdim)
   {

    SFREE(typ);
    SFREE(pdim);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_REL_ENTRY_INFO - release the info allocated by PD_get_entry */

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
	PD_get_format_version(nv);
	nv = max(nv, 3);
	PD_set_format_version(nv);

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

PDBfile *_PD_create(tr_layer *tr, SC_udl *pu, char *name, void *a)
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
    
	if (a != NULL)
	   _PD_SETUP_MP_FILE(file, *(SC_communicator *) a);

	_PD_set_standard(file, NULL, NULL);

	pa->cfp = fp;

	ok = (*file->create)(file, _PD_IS_MASTER(file));
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
 *           - return TRUE if successful and FALSE otherwise
 */

int _PD_close(PDBfile *file)
   {int ret;
    FILE *fp;

    ret = FALSE;

    if (file != NULL)
       {ret = _PD_csum_close(file);

/* position the file pointer at the greater of the current position and
 * the location of the chart
 */
	if ((file->mode == PD_CREATE) || (file->mode == PD_APPEND))
	   ret = PD_flush(file);

	fp = file->stream;
	if (fp != NULL)
	   {if (lio_close(fp) != 0)
	       PD_error("CANNOT CLOSE FILE - PD_CLOSE", PD_CLOSE);};

/* free the space */
	_PD_rl_pdb(file);};

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

    _PD_MARK_AS_FLUSHED(file, TRUE);

    if (a != NULL)
       _PD_SETUP_MP_FILE(file, *(SC_communicator *) a);

    vers = _PD_identify_file(file);
    if (vers > 0)
       {ok = (*file->open)(file);
	if (ok != TRUE)
	   PD_error("CANNOT OPEN FILE - _PD_OPEN", PD_OPEN);}
    else
       {file->udl = NULL;
	_PD_rl_pdb(file);
	file = NULL;};

    return(file);}

/*--------------------------------------------------------------------------*/

/*                      FREE_LIST MANAGEMENT ROUTINES                       */

/*--------------------------------------------------------------------------*/

#if 0
/* _PD_ADD_FREE_SPACE - add block to free list in descending size order
 */

void _PD_add_free_space(PDBfile *file, int64_t address, int64_t size)
   {PD_disk_block *block, *free, *prev;

    if (size > 0)
       {block = FMAKE(PD_disk_block, "_PD_add_free_space:block");
        block->addr.diskaddr = address;
        block->length        = size;
        block->next          = NULL;

        free = file->free_list;
        prev = file->free_list;
        if (free == NULL)
           file->free_list = block;
        else if (free->length <= size)
           {block->next = free;
            file->free_list = block;}
        else
           {while((free = free->next) != NULL)
               {if (free->length <= size)
                   {block->next = free;
                    prev->next  = block;
                    break;}
                prev = free;}

            if (free == NULL) 
               {prev->next = block;};};}
    return;}  

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_GET_FREE_SPACE - compute size of free space in FILE */

int64_t _PD_get_free_space(PDBfile *file, int64_t size)
   {int64_t ret;
    PD_disk_block *free, *prev, *save;

    ret = 0;

    if ((file->free_list != NULL) && (size > 0))
       if (size <= file->free_list->length)
          {free = file->free_list;
           prev = file->free_list;
           save = NULL;
 
           while ((free = free->next) != NULL)
              {if (free->length < size)
                  {ret = size;
                   if (prev->length != size)
                      prev->length -= size;
                   else 
                      {if (save == NULL)
                          file->free_list = free;
                       else
                          save->next = free;
                       SFREE(prev);};}
               save = prev;
               prev = free;}

           if (free == NULL)
              {ret = size;
               if (prev->length != size)
                  prev->length -= size;
               else
                  {save->next = NULL;
                   SFREE(prev);};};} 

    return(ret);} 

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
