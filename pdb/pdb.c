/*
 * PDB.C - Portable Data Base Library
 *       - a collection of file manipulation routines with the following
 *       - aims:
 *       -      (1) Portable, therefore written in C
 *       -      (2) Machine Independent, carries information about objects
 *       -          in the file so that the implementation can extract
 *       -          the data even if on a wholely different machine type
 *       -      (3) Simplicity, for ease of implementation and so that
 *       -          linkable modules in another language can use these
 *       -          routines
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_CREATE - Create a PDB file named NAME.
 *           - Return an initialized PDBfile instance if successful
 *           - otherwise return NULL.
 *
 * #bind PD_create fortran() scheme() python()
 */

PDBfile *PD_create(const char *name)
   {PDBfile *file;

    _PD_init_state(FALSE);

    _PD_set_null_comm(-1);

    if (!_PD_IS_SMP_INIT)
       PD_init_threads(0, NULL);

    file = _PD_open_bin(name, "w", NULL);

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_OPEN - Open an existing PDB file named NAME, extract the symbol table
 *         - and structure chart.
 *         - The file is opened in MODE which can be one of "r", "w", or "a"
 *         - corresponding to read-only, write-only, or append respectively.
 *         - Return a pointer to a PDBfile instance if successful
 *         - otherwise return NULL.
 *
 * #bind PD_open scheme() python()
 */

PDBfile *PD_open(const char *name, const char *mode)
   {PDBfile *file;

    _PD_init_state(FALSE);

    _PD_set_null_comm(-1);

    if (!_PD_IS_SMP_INIT)
       PD_init_threads(0, NULL);

    file = _PD_open_bin(name, mode, NULL);

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_OPEN_F - Open an existing PDB file named NAME, extract the symbol table
 *           - and structure chart, and set the major_order and default_offset
 *           - appropriately for Fortran usage.
 *           - Return a pointer to a PDBfile instance if successful
 *           - otherwise return NULL.
 *
 * #bind PD_open_f fortran(pd_open_f)
 */

PDBfile *PD_open_f(const char *name, const char *mode)
   {PDBfile *file;

    file = PD_open(name, mode);
    if (file != NULL)
       {file->major_order    = COLUMN_MAJOR_ORDER;
        file->default_offset = 1;};

    return(file);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_FAMILY - If the PDBfile OF exceeds its maximum size open the
 *           - the next member of the file family.
 *           - If FLAG is TRUE and a new file is opened, then the old
 *           - file will be closed.
 *           - Return a pointer to a PDBfile instance, either OF or the
 *           - next file in the family sequence, if successful
 *           - otherwise return NULL.
 *
 * #bind PD_family fortran() scheme() python()
 */

PDBfile *PD_family(PDBfile *of ARG(,,cls), int flag)
   {long i;
    int64_t msz, csz;
    char name[MAXLINE];
    PDBfile *nf;
    defstr *dp;
    memdes *lst;
    PD_smp_state *pa;

    if (of == NULL)
       return(NULL);

    if (_PD.maxfsize == 0)
       SC_fix_lmt(sizeof(int64_t), NULL, &_PD.maxfsize, NULL);

/* if the size is effectively infinite stop ASAP */
    msz = PD_get_max_file_size(of);
    if (msz == _PD.maxfsize)
       return(of);

    pa = _PD_get_state(-1);

    csz = _PD_GET_FILE_SIZE(of);

    nf = of;
    if (msz < csz)
       {SC_strncpy(name, MAXLINE, of->name, -1);
        SC_advance_name(name);

/* create the new file - being sure to get the target right */
        pa->req_std   = of->std;
        pa->req_align = of->align;
        nf = PD_open(name, "w");
	if (nf != NULL)
	   {PD_set_max_file_size(nf, msz);
	    PD_set_offset(nf, PD_get_offset(of));
	    PD_set_track_pointers(nf, PD_get_track_pointers(of));
	    PD_set_major_order(nf, PD_get_major_order(of));

	    nf->previous_file = CSTRSAVE(of->name);

/* copy the types over */
	    for (i = 0; SC_hasharr_next(of->chart, &i, NULL, NULL, (void **) &dp); i++)
	        {lst = PD_copy_members(dp->members);
		 _PD_defstr_inst(nf, dp->type, dp->kind, lst,
				 NO_ORDER, NULL, NULL, PD_CHART_HOST);};

/* copy the attributes over? */

/* close the old file */
	    if (flag)
	       PD_close(of);};};

    return(nf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_OPEN_VIF - Open a virtual internal PDB file named NAME.
 *             - Return an initialized PDBfile instance if successful
 *             - otherwise return NULL.
 *
 * #bind PD_open_vif fortran() scheme() python()
 */

PDBfile *PD_open_vif(const char *name)
   {int reg;
    PDBfile *file;
    SC_udl *pu;

    PD_init_threads(SC_n_threads, NULL);

    pu = _PD_pio_open(NULL, NULL);

    reg  = TRUE;
    file = _PD_mk_pdb(pu, name, BINARY_MODE_RPLUS, reg, NULL, NULL);

    file->virtual_internal = TRUE;

    _PD_set_standard(file, NULL, NULL);

    _PD_init_chrt(file, TRUE);

/* initialize the starting address for writing */
    _PD_SET_ADDRESS(file, file->chrtaddr);

/* initialize directories */
    if (!PD_def_dir(file))
       {PD_error("FAILED TO INITIALIZE DIRECTORIES - PD_OPEN_VIF",
		 PD_GENERIC);
	return(NULL);};

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_COPY_TYPE - Copy a data type, TYPE, definition from source file SF to
 *              - destination file DF.
 *              - Return TRUE iff successful.
 *
 * #bind PD_copy_type fortran() scheme() python()
 */

pboolean PD_copy_type(const PDBfile *sf, PDBfile *df, const char *type)
   {int rv;
    defstr *dp;
    memdes *lst;
    PD_smp_state *pa;

    rv = B_T;

    pa = _PD_get_state(-1);

    dp = PD_inquire_type(sf, type);
    if (dp == NULL)
       {snprintf(pa->err, MAXLINE,
		 "ERROR: TYPE %s NOT FOUND - PD_COPY_TYPE\n", type);
        rv = B_F;}

    else
       {lst = PD_copy_members(dp->members);
	dp  = _PD_defstr_inst(df, type, KIND_STRUCT, lst,
			      NO_ORDER, NULL, NULL, PD_CHART_HOST);
	if (dp == NULL)
	   {snprintf(pa->err, MAXLINE,
		     "ERROR: CANNOT CREATE TYPE %s - PD_COPY_TYPE\n", type);
	    rv = B_F;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CLOSE_WRK - close a PDB file after writing out the symbol table and
 *               - structure chart
 *               - return the length of the file if successful
 *               - and -1 otherwise
 */

static int64_t _PD_close_wrk(PDBfile *file)
   {int64_t ret;
    PD_smp_state *pa;
    PFBinClose fun;
    tr_layer *tr;

    ret = FALSE;

    if (file != NULL)
       {pa = _PD_get_state(-1);

	switch (SETJMP(pa->close_err))
	   {case ABORT :
	         return(FALSE);
	    case ERR_FREE :
	         return(TRUE);
	    default :
	         memset(pa->err, 0, MAXLINE);
		 break;};

	file->req.oper = PD_CLOSE;

	tr  = file->tr;
	fun = tr->close;
	ret = fun(file);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_CLOSE - Close PDBfile FILE after writing out the symbol table and
 *          - structure chart.
 *          - Return TRUE if successful and FALSE otherwise.
 *
 * #bind PD_close fortran() scheme() python()
 */

pboolean PD_close(PDBfile *file ARG(,,cls))
   {pboolean ret;
    int64_t ln;

    ln  = _PD_close_wrk(file);
    ret = (ln > 0);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_CLOSE_N - Close PDBfile FILE after writing out the symbol table and
 *            - structure chart.
 *            - Return the file length if successful
 *            - and -1 otherwise.
 *
 * #bind PD_close_n fortran() scheme() python()
 */

int64_t PD_close_n(PDBfile *file ARG(,,cls))
   {int64_t ret;

    ret = _PD_close_wrk(file);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_FLUSH - Dump the metadata describing the contents of the PDBfile FILE.
 *          - The metadata tables are:
 *          -    structure chart
 *          -    symbol table
 *          -    extras table
 *          - The table addresses are also updated.
 *          - Return TRUE iff successful.
 *          - NOTE: do not invoke PD_error in here because we do not
 *          - know whether we came in on a write or a close and therefore
 *          - do not know which jmpbuf to use
 *
 * #bind PD_flush fortran() scheme() python()
 */

int64_t PD_flush(PDBfile *file ARG(,,cls))
   {int64_t ret;

/* do the PDB level work */
    ret = _PD_FLUSH_FILE(file);

/* NOTE: this way uses seek and tell to get the length
 * another, stronger way would be for PDBLib to actually count
 * the bytes thus returning a length independently of the
 * C library
 */
    if (ret == TRUE)
       ret = PD_get_file_length(file);

    _PD_MARK_AS_FLUSHED(file, TRUE);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_READ - read an entry from the PDB file described by
 *          - FULLPATH, TYPE, EP, and the ND dimension specifications IND
 *          - into the location pointed to by VR
 *          - return the number of items successfully read
 *          -
 *          - NOTE: VR must be a pointer to an object with the type
 *          - given by TYPE (PDBLib will allocated space if necessary)!
 */ 
 
int _PD_read(PDBfile *file, char *fullpath, const char *type,
	     syment *ep, void *vr, int nd, long *ind)
   {int rv, nf;
    const char *typ;
    defstr *dp;

    rv = 0;

    file->req.oper = PD_READ;

#ifdef USE_REQUESTS
    CFREE(file->req.base_type);
    file->req.base_type = CSTRSAVE(type);
#endif

/* figure out information needed to call PD_fix_denorm later if needed */
    nf  = 0;
    typ = (type == NULL) ? PD_entry_type(ep) : type;
    if (file->fix_denorm &&  (typ != NULL))
       {dp = PD_inquire_type(file, typ);
	if ((dp != NULL) && (dp->kind == KIND_FLOAT))
	   {if (ind == NULL)
	       nf = _PD_comp_num(ep->dimensions);
	    else
	       nf = _PD_comp_nind(nd, ind, 3);};};

    if ((nd == -1) && (ind == NULL))
       {if (type != NULL)
	   rv = _PD_hyper_read(file, fullpath, type, ep, vr);}
    else if (ind != NULL)
       rv = _PD_indexed_read_as(file, fullpath, type, vr, nd, ind, ep);

/* if requested, set denorm float pt vars to zero */
    if (nf > 0)
       PD_fix_denorm(NULL, typ, nf, vr);

    _PD_request_unset(file);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_READ - Read entry NAME from the PDBfile FILE
 *         - into the location pointed to by VR.
 *         - Return the number of items successfully read.
 *         -
 *         - NOTE: VR must be a pointer to an object with the type
 *         - given by TYPE (PDBLib will allocated space if necessary)!
 *
 * #bind PD_read fortran() scheme() python()
 */

int PD_read(PDBfile *file ARG(,,cls), const char *name, void *vr)
   {int rv;

    rv = PD_read_as(file, name, NULL, vr);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_READ_AS - Read entry NAME from the PDBfile FILE
 *            - into the location pointed to by VR.
 *            - Convert to type TYPE regardless of entry type.
 *            - Return the number of items successfully read.
 *            -
 *            - NOTE: VR must be a pointer to an object with the type
 *            - given by TYPE (PDBLib will allocate space if necessary)!
 *
 * #bind PD_read_as fortran() scheme() python()
 */

int PD_read_as(PDBfile *file ARG(,,cls), const char *name,
	       const char *type, void *vr)
   {int err;
    syment *ep;
    PD_smp_state *pa;
    char msg[MAXLINE], fullpath[MAXLINE];

    pa = _PD_get_state(-1);

    switch (SETJMP(pa->read_err))
       {case ABORT :
	     return(FALSE);
        case ERR_FREE :
	     return(TRUE);
        default :
	     memset(pa->err, 0, MAXLINE);
	     break;};

/* find the effective symbol table entry for the named item */
    ep = _PD_effective_ep(file, name, TRUE, fullpath);
    if (ep == NULL)
       {snprintf(msg, MAXLINE, "UNREADABLE OR MISSING ENTRY \"%s\" - PD_READ_AS",
                     fullpath);
        PD_error(msg, PD_READ);};

    if (type == NULL)
       type = PD_entry_type(ep);

    err = file->tr->read(file, fullpath, type, ep, vr, -1, NULL);

    _PD_rl_syment_d(ep);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_READ_AS_DWIM - A special variant of PD_read_as on the variable NAME.
 *                 - If the variable is a direct type read it normally.
 *                 - If it is indirect do a PD_read_as but copy what
 *                 - the returned values into the space, SPACE.
 *                 - This frees the caller from having to worry about
 *                 - indirections provided the number of items to be read and
 *                 - hence the size of SPACE is large enough to hold the data.
 *                 - Return the number of items successfully read
 *                 - if less than NIX otherwise read NIX items and
 *                 - return minus the actual number of items.
 *                 -
 *                 - NOTE: SPACE must be an object with the type
 *                 - given by TYPE (potenitally differs from the
 *                 - conventional PDB rule)
 *
 * #bind PD_read_as_dwim fortran() scheme() python()
 */

int PD_read_as_dwim(PDBfile *file ARG(,,cls), const char *name,
		    const char *outtype, long nix, void *space)
   {int nir, nl;
    long ntr;
    char s[MAXLINE], fullpath[MAXLINE];
    char *intype;
    syment *ep;
    PD_smp_state *pa;

    nl  = 0;
    nir = 0;

    pa = _PD_get_state(-1);

    switch (SETJMP(pa->read_err))
       {case ABORT :
	     return(0);
        case ERR_FREE :
	     return(nl);
        default :
	     memset(pa->err, 0, MAXLINE);
	     break;};

/* find the effective symbol table entry for the named item */
    ep = _PD_effective_ep(file, name, TRUE, fullpath);
    if (ep == NULL)
       {snprintf(s, MAXLINE, "UNREADABLE OR MISSING ENTRY \"%s\" - PD_READ_AS_DWIM",
                   fullpath);
        PD_error(s, PD_READ);};

    if (outtype != NULL)
       {if (_PD_indirection(outtype))
	   {snprintf(s, MAXLINE, "TYPE MUST NOT BE POINTER \"%s\" - PD_READ_AS_DWIM",
		    outtype);
	    PD_error(s, PD_READ);};};

    intype = PD_entry_type(ep);
    if (_PD_indirection(intype))
       {intb bpi;
	char *ps, *pv;
	void *vr;
	data_standard *std;
	hasharr *chrt;

	SC_strncpy(s, MAXLINE, intype, -1);
	PD_dereference(s);
	bpi = _PD_lookup_size(s, file->host_chart);

        ntr = file->tr->read(file, fullpath, intype, ep, &vr, -1, NULL);
	if (ntr <= 0L)
	   PD_error("INDIRECT READ FAILED - PD_READ_AS_DWIM", PD_READ);

	nir = SC_arrlen(vr)/bpi;
	if (nir > nix)
	   {nl  = -nir;
	    nir = nix;}
	else
	   nl = nir;

/* convert/copy the dereferenced data */
	ps   = (char *) space;
	pv   = (char *) vr;
	std  = file->host_std;
	chrt = file->host_chart;
	PD_convert(&ps, &pv, s, outtype, nir,
		   std, std, std, chrt, chrt, 0, PD_READ);

/* since we are freeing the space we had better reset the pointer lists */
	_PD_ptr_reset(file, vr);

	CFREE(vr);}

/* read same as for direct case - should be equivalent to PD_read_as */
    else
       {if (outtype == NULL)
	   outtype = intype;

	ntr = PD_entry_number(ep);
	if (nir > nix)
	   {nl  = -ntr;
	    ep->number = nix;}
	else
	   nl = ntr;
	
	nir = file->tr->read(file, fullpath, outtype, ep, space, -1, NULL);};

    _PD_rl_syment_d(ep);

    return(nl);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_READ_ALT - Read part of entry NAME from PDBfile FILE
 *             - into the location pointed to by VR.
 *             - IND contains one triplet of long ints per variable
 *             - dimension specifying start, stop, and step for the index.
 *             - Return the number of item successfully read.
 *             -
 *             - NOTE: VR must be a pointer to an object with the type
 *             - given by TYPE (PDBLib will allocated space if necessary)!
 *
 * #bind PD_read_alt fortran() scheme() python()
 */

int PD_read_alt(PDBfile *file ARG(,,cls), const char *name,
		void *vr, long *ind)
   {int rv;

    rv = PD_read_as_alt(file, name, NULL, vr, ind);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_READ_AS_ALT - Read part of entry NAME from the PDBfile FILE
 *                - into the location pointed to by VR.
 *                - IND contains one triplet of long ints per variable
 *                - dimension specifying start, stop, and step for the index.
 *                - Convert to type TYPE regardless of entry type.
 *                - Return the number of items successfully read.
 *                -
 *                - NOTE: the entry MUST be an array (either a static
 *                - array or a pointer)
 *                -
 *                - NOTE: VR must be a pointer to an object with the type
 *                - of the object associated with NAME (PDBLib will
 *                - allocated space if necessary)!
 *
 * #bind PD_read_as_alt fortran() scheme() python()
 */

int PD_read_as_alt(PDBfile *file ARG(,,cls), const char *name,
		   const char *type, void *vr, long *ind)
   {int nd, rv;
    char fullpath[MAXLINE];
    dimdes *pd, *dims;
    syment *ep;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    switch (SETJMP(pa->read_err))
       {case ABORT :
	     return(FALSE);
        case ERR_FREE :
	     return(TRUE);
        default :
	     memset(pa->err, 0, MAXLINE);
	     break;};

/* look up the variable name and return FALSE if it is not there */
    ep = _PD_effective_ep(file, name, TRUE, fullpath);
    if (ep == NULL)
       PD_error("ENTRY NOT IN SYMBOL TABLE - PD_READ_AS_ALT", PD_READ);

    dims = PD_entry_dimensions(ep);
    for (nd = 0, pd = dims; pd != NULL; pd = pd->next, nd++);

    rv = file->tr->read(file, fullpath, type, ep, vr, nd, ind);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DEFENT - do the work of defining an entry in the PDB file
 *            - symbol table and stake out the disk space
 *            - but write nothing
 *            - return the new symbol table entry if successful, and
 *            - return NULL otherwise
 */

syment *_PD_defent(PDBfile *file, const char *name, const char *outtype,
		   inti number, dimdes *dims)
   {intb bpi;
    int64_t addr;
    char bf[MAXLINE];
    char *lname;
    defstr *dp;
    syment *ep;
    PD_smp_state *pa;

    ep = NULL;

    pa = _PD_get_state(-1);

    switch (SETJMP(pa->write_err))
       {case ABORT :
	     return(NULL);
        case ERR_FREE :
	     return(ep);
        default :
	     memset(pa->err, 0, MAXLINE);
	     break;};

/* if there are pointers involved it is an error */
    dp = PD_inquire_type(file, outtype);
    if (dp == NULL)
       PD_error("UNKNOWN FILE TYPE - _PD_DEFENT", PD_WRITE);

    if (dp->n_indirects)
       PD_error("CANNOT DEFINE ENTRY WITH INDIRECTS - _PD_DEFENT",
                PD_WRITE);

    lname = _PD_var_namef(file, name, bf, MAXLINE);

    ep   = PD_inquire_entry(file, lname, TRUE, NULL);
    addr = _PD_get_next_address(file, outtype, number, NULL,
				TRUE, FALSE, TRUE);

/* if this is a new entry */
    if (ep == NULL)
       {ep = _PD_mk_syment(outtype, number, addr, NULL, dims);

        _PD_e_install(file, lname, ep, FALSE);

	bpi = _PD_lookup_size(outtype, file->chart);
	ep  = _PD_EXTEND_FILE(file, number*bpi) ? ep : NULL;}

/* if this is only a new block */
    else
       ep = _PD_block_add(file, ep, dims, addr) ? ep : NULL;

    return(ep);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEFENT - Define an entry in the PDBfile, FILE, symbol table and
 *           - reserve the disk space but write nothing out.
 *           - All dimensional information is in the NAME string.
 *           - Return the new symbol table entry if successful, and
 *           - return NULL otherwise.
 *
 * #bind PD_defent fortran() scheme() python()
 */

syment *PD_defent(PDBfile *file ARG(,,cls), const char *name,
		  const char *outtype)
   {int ie;
    long number;
    dimdes *dims;
    syment *ep;

    dims = _PD_ex_dims(name, file->default_offset, &ie);
    if ((dims == NULL) && (ie == TRUE))
       number = 0;
    else
       number = _PD_comp_num(dims);

    ep = _PD_defent(file, name, outtype, number, dims);

    return(ep);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEFENT_ALT - Define an entry in the PDBfile, FILE, symbol table and
 *               - reserve the disk space but write nothing out.
 *               - All dimensional information is specified by the number
 *               - of dimensions, ND, and the array of (min, max)
 *               - pairs of long ints in IND.
 *               - Return the new symbol table entry if successful, and
 *               - return NULL otherwise.
 *
 * #bind PD_defent_alt fortran() scheme() python()
 */

syment *PD_defent_alt(PDBfile *file ARG(,,cls), const char *name,
		      const char *outtype, int nd, long *ind)
   {int i;
    long number, maxi, mini, leng;
    dimdes *dims, *next, *prev;
    syment *ep;

    prev = NULL;
    
/* compute the disk address, the number of items, and the dimensions */
    number = 1L;
    dims   = NULL;
    for (i = 0; i < nd; i++)
        {mini = ind[0];
         maxi = ind[1];
         ind += 2;

         leng    = maxi - mini + 1L;
         number *= leng;

/* handle case of zero space to be reserved */
	 if (number == 0L)
	    {dims = NULL;
	     break;};

         next = _PD_mk_dimensions(mini, leng);
         if (dims == NULL)
            dims = next;
         else
            {prev->next = next;
	     SC_mark(next, 1);};

         prev = next;};

    ep = _PD_defent(file, name, outtype, number, dims);

    return(ep);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WRITE - primitive write to PDBfile which is used by 
 *           - PD_WRITE and PD_WRITE_AS
 *           -
 *           - NOTE: VR must be a pointer to an object with the type
 *           - given by TYPE!!!!
 */

syment *_PD_write(PDBfile *file, const char *name,
		  const char *intype, const char *outtype,
		  void *vr, dimdes *dims, int appnd, int *pnew)
   {int c, new, ok, st;
    long number; 
    int64_t addr;
    char bf[MAXLINE];
    char *lname;
    const char *fullpath;
    syment *ep;
    PD_smp_state *pa;
    SC_address ad;

    pa = _PD_get_state(-1);

    pa->append_flag = FALSE;

    ep    = NULL;
    new   = TRUE;    
    lname = NULL;
    
    switch (SETJMP(pa->write_err))
       {case ABORT :
             if (pnew != NULL)
	        *pnew = new;
	     return(NULL);
        case ERR_FREE :
	     *pnew = new;
	     return(ep);
        default :
	     memset(pa->err, 0, MAXLINE);
	     break;};

    if (file->mode == PD_OPEN)
       PD_error("FILE OPENED IN READ-ONLY MODE - _PD_WRITE", PD_WRITE);
    
    file->req.oper = PD_WRITE;

    number = _PD_comp_num(dims);

/* append a new block to an existing entry if TRUE */
    if (appnd)
       {SC_strncpy(bf, MAXLINE, name, -1);

/* do this so that things such as a[20:20].b work properly
 * NOTE: this also implies that a[20:20].b.c works while
 *       a.b[20:20].c doesn't
 *       for now this defines the semantics of append (10/6/93)
 *       In fact we handle this case in PDBView at the high level
 */
	lname = strpbrk(bf, ".()[]");
	if (lname != NULL)
           {c      = *lname;
	    *lname = '\0';
	    ep     = PD_inquire_entry(file, bf, FALSE, NULL);
	    *lname = c;}
	else
	    ep = PD_inquire_entry(file, bf, FALSE, NULL);

        if (ep == NULL) 
           PD_error("CANNOT APPEND TO NON-EXISTING ENTRY - _PD_WRITE",
                    PD_WRITE);

	if ((PD_entry_dimensions(ep) != NULL) && (PD_entry_number(ep) > 0))
	   {_PD_adj_dimensions(file, bf, ep);

	    addr = _PD_get_next_address(file, PD_entry_type(ep), number, vr,
					FALSE, FALSE, FALSE);
        
/* extend the syment */
	    _PD_block_add(file, ep, dims, addr);}

/* handle the case of a defent for no space */
	else
	   _PD_block_rel(ep);

	fullpath = bf;}

    else
       fullpath = name;

    ep = _PD_effective_ep(file, fullpath, FALSE, NULL);

    file->cksum.verified = FALSE;

#ifdef USE_REQUESTS
    CFREE(file->req.base_type);
    file->req.base_type = CSTRSAVE(intype);
#endif

/* if the variable already exists use the existing file info */
    if (ep != NULL)

/* handle the zero space defent case */
       {if (ep->blocks == NULL)
	   {addr = _PD_get_next_address(file, PD_entry_type(ep), number, vr,
					FALSE, FALSE, TRUE);

	    _PD_block_init(ep, TRUE);
	    ep->number     = number;
	    ep->dimensions = dims;
	    _PD_entry_set_address(ep, 0, addr);
	    _PD_entry_set_number(ep, 0, number);

	    SC_mark(dims, 1);

            new = TRUE;}

/* the canonical case */
        else
	   {addr = PD_entry_address(ep);
	    new  = FALSE;};

	lname = (char *) fullpath;
	st    = TRUE;}

/* if the variable doesn't exist define it to the file */
    else
       {if (appnd)
	   {if (lname != NULL)
	       *lname = '\0';
	    lname = bf;}

	else
	   lname = _PD_var_namef(NULL, fullpath, bf, MAXLINE);

	addr = _PD_get_next_address(file, outtype, number, vr,
				    FALSE, FALSE, FALSE);

	ep  = _PD_mk_syment(outtype, number, addr, NULL, dims);
	st  = _PD_e_install(file, lname, ep, TRUE);

	addr = _PD_annotate_text(file, ep, lname, addr);};

/* if the number of items is zero or the pointer NULL
 * do not attempt to do the actual write
 */
    if ((number > 0) && (vr != NULL))

/* now write the data to the file */
       {if (file->virtual_internal)
	   {if (new)
	       {ad.memaddr = (void *) vr;
		_PD_entry_set_address(ep, 0, ad.diskaddr);
		SC_mark(ad.memaddr, 1);}

	    else
	       {if (!_PD_hyper_write(file, lname, ep, vr, intype))
		   PD_error("CANNOT WRITE VIRTUAL VARIABLE - _PD_WRITE",
			    PD_WRITE);};}

        else
	   {if (outtype == NULL)
	       outtype = PD_entry_type(ep);

	    if (intype == NULL)
	       intype = outtype;

/* go to the correct address */
	    _PD_set_current_address(file, addr, SEEK_SET, PD_WRITE);

/* do the low level write */
	    if (!_PD_hyper_write(file, lname, ep, vr, intype))
	       PD_error("CANNOT WRITE VARIABLE - _PD_WRITE", PD_WRITE);

/* if the variable didn't previously exist we're at the end of the file */
	    if (new)
	       file->chrtaddr = _PD_get_current_address(file, PD_WRITE);};};

    _PD_request_unset(file);

    ok = _PD_cksum_var_write(file, lname, ep);
    SC_ASSERT(ok == TRUE);
  
    if (st == FALSE)
       _PD_rl_syment(ep);

    *pnew = new;

    return(ep);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRITE - Make an entry in the symbol table of PDBfile FILE
 *          - under NAME and write the data of type, TYPE, pointed to by VR
 *          - to the file.
 *          - Return TRUE if successful and FALSE otherwise.
 *          -
 *          - NOTE: VR must be a pointer to an object with the type
 *          - given by TYPE!!!!
 *
 * #bind PD_write fortran() scheme() python()
 */

pboolean PD_write(PDBfile *file ARG(,,cls), const char *name,
		  const char *type, void *vr)
   {pboolean rv;

    rv = PD_write_as(file, name, type, type, vr);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRITE_AS - Make an entry in the symbol table of PDBfile FILE
 *             - under NAME, convert the data pointed to by VR from
 *             - type INTYPE to type OUTTYPE and write the resulting data
 *             - to the file.
 *             - Return TRUE iff successful.
 *             -
 *             - NOTE: VR must be a pointer to an object with the type
 *             - given by TYPE!!!!
 *
 * #bind PD_write_as fortran() scheme() python()
 */

pboolean PD_write_as(PDBfile *file ARG(,,cls), const char *name,
		     const char *intype, const char *outtype, void *vr)
   {int appnd, new, ie;
    pboolean rv;
    char *t, *lname, fullpath[MAXLINE];
    const char *s;
    syment *ep;
    dimdes *dims;
    PD_smp_state *pa;

    rv = B_F;

    if (file != NULL)
       {pa = _PD_get_state(-1);

        if (PD_has_directories(file))
	   {lname = _PD_fixname(file, name);
	    SC_strncpy(fullpath, MAXLINE, lname, -1);
	    t = strchr(lname, '.');
	    if (t != NULL)
	       *t = '\0';
	    s = fullpath;}
        else
	   {SC_strncpy(fullpath, MAXLINE, name, -1);
	    lname = fullpath;
	    t = strchr(fullpath, '.');
	    if (t != NULL)
	       *t = '\0';
	    s = name;};

	appnd = pa->append_flag;
	dims  = _PD_ex_dims(lname, file->default_offset, &ie);
	ep    = file->tr->write(file, s, intype, outtype,
                                vr, dims, appnd, &new);
	if (ep != NULL)
	   {if (new == B_F)
	       {if (appnd == B_F)
		   _PD_rl_dimensions(dims);
		_PD_rl_syment_d(ep);};
	    rv = B_T;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRITE_ALT - Make an entry in the symbol table of PDBfile FILE
 *              - under NAME and write the data of type, TYPE, pointed
 *              - to by VR to the file.
 *              - The entry is named by NAME has ND dimensions and IND
 *              - contains the min and max (pairwise) of each dimension's
 *              - range.
 *              - Return TRUE iff successful.
 *              -
 *              - NOTE: VR must be a pointer to an object with the type
 *              - given by TYPE!!!!
 *
 * #bind PD_write_alt fortran() scheme() python()
 */

pboolean PD_write_alt(PDBfile *file ARG(,,cls), const char *name,
		      const char *type, void *vr, int nd, long *ind)
   {pboolean rv;

    rv = PD_write_as_alt(file, name, type, type, vr, nd, ind);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRITE_AS_ALT - Make an entry in the symbol table of PDBfile FILE
 *                 - under NAME, convert the data pointed to by VR from
 *                 - type INTYPE to type OUTTYPE and write the resulting data
 *                 - to the file.
 *                 - The entry has name, NAME, ND dimensions, and the ranges
 *                 - of the dimensions are given (min, max) pairwise in IND.
 *                 - Return TRUE iff successful.
 *                 -
 *                 - NOTE: VR must be a pointer to an object with the type
 *                 - given by TYPE!!!!
 *
 * #bind PD_write_as_alt fortran() scheme() python()
 */

pboolean PD_write_as_alt(PDBfile *file ARG(,,cls), const char *name,
			 const char *intype, const char *outtype,
			 void *vr, int nd, long *ind)
   {int i, new, appnd, nc;
    pboolean rv;
    long start, stop, step, leng;
    char lndx[MAXLINE], hname[MAXLINE], fullpath[MAXLINE];
    char inty[MAXLINE], outy[MAXLINE];
    dimdes *dims, *next, *prev;
    syment *ep;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    prev = NULL;
    dims = NULL;

    SC_strncpy(lndx, MAXLINE, "(", -1);

    for (i = 0; i < nd; i++)
        {start = ind[0];
         stop  = ind[1];
         step  = ind[2];
         ind += 3;

         SC_vstrcat(lndx, MAXLINE, "%ld:%ld:%ld,", start, stop, step);

         leng = stop - start + 1L;
         next = _PD_mk_dimensions(start, leng);
         if (dims == NULL)
            dims = next;
         else
            {prev->next = next;
	     SC_mark(next, 1);};

         prev = next;};

    nc = strlen(lndx);
    if (nc > 1)
       {lndx[nc-1] = ')';
        snprintf(hname, MAXLINE, "%s%s", name, lndx);}
    else
       SC_strncpy(hname, MAXLINE, name, -1);

    if (intype != NULL)
       {SC_strncpy(inty, MAXLINE, intype, -1);
	SC_trim_right(inty, " \t");
	intype = inty;};
    if (outtype != NULL)
       {SC_strncpy(outy, MAXLINE, outtype, -1);
	SC_trim_right(outy, " \t");
	outtype = outy;};

    appnd = pa->append_flag;
    SC_strncpy(fullpath, MAXLINE, _PD_fixname(file, hname), -1);
    ep = file->tr->write(file, fullpath, intype, outtype,
			 vr, dims, pa->append_flag, &new);

    if (ep != NULL)
       {if (!new)
	   {if (!appnd)
	       _PD_rl_dimensions(dims);
	    _PD_rl_syment_d(ep);};
	rv = B_T;}
    else
       rv = B_F;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_APPEND - Append a new block of data pointed to by VR
 *           - to an existing entry named by NAME in PDBfile FILE .
 *           - The dimensions of the new data are specified in NAME.
 *           -
 *           - NOTE: VR must be a pointer to an object with the type
 *           - of the existing entry
 *
 * #bind PD_append fortran() scheme() python()
 */

pboolean PD_append(PDBfile *file ARG(,,cls), const char *name, void *vr)
   {pboolean rv;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    pa->append_flag = TRUE;

    rv = PD_write_as(file, name, NULL, NULL, vr);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_APPEND_AS - Append a new block of data pointed to by VR
 *              - to an existing entry named by NAME in PDBfile FILE .
 *              - The dimensions of the new data are specified in NAME.
 *              - Convert the data from INTYPE to the type of the
 *              - existing data in FILE.
 *              -
 *              - NOTE: VR must be a pointer to an object with the type
 *              - of the existing entry
 *
 * #bind PD_append_as fortran() scheme() python()
 */

pboolean PD_append_as(PDBfile *file ARG(,,cls), const char *name,
		      const char *intype, void *vr)
   {pboolean rv;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);
    
    pa->append_flag = B_T;

    rv = PD_write_as(file, name, intype, NULL, vr);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_APPEND_ALT - Append a new block of data pointed to by VR
 *               - to an existing entry named by NAME in PDBfile FILE .
 *               - The new data has ND dimensions, and the ranges
 *               - of the dimensions are given (min, max) pairwise in IND.
 *               -
 *               - NOTE: VR must be a pointer to an object with the type
 *               - of the existing entry
 *
 * #bind PD_append_alt fortran() scheme() python()
 */

pboolean PD_append_alt(PDBfile *file ARG(,,cls), const char *name,
		       void *vr, int nd, long *ind)
   {pboolean rv;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);
    
    pa->append_flag = B_T;

    rv = PD_write_as_alt(file, name, NULL, NULL, vr, nd, ind);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_APPEND_AS_ALT - Append a new block of data pointed to by VR
 *                  - to an existing entry named by NAME in PDBfile FILE.
 *                  - Convert the data from INTYPE to the type of the
 *                  - existing data before writing to FILE.
 *                  - The new data has ND dimensions, and the ranges
 *                  - of the dimensions are given (min, max) pairwise in IND.
 *                  -
 *                  - NOTE: VR must be a pointer to an object with the type
 *                  - of the existing entry
 *
 * #bind PD_append_as_alt fortran() scheme() python()
 */

pboolean PD_append_as_alt(PDBfile *file ARG(,,cls), const char *name,
			  const char *intype, void *vr, int nd, long *ind)
   {pboolean rv;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);
    
    pa->append_flag = B_T;

    rv = PD_write_as_alt(file, name, intype, NULL, vr, nd, ind);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_SET_IO_HOOKS - set the I/O hooks for PDB
 *                 - should be set at beginning in serial region
 */

void PD_set_io_hooks(int which)
   {

    switch (which)
       {case 0 :
	     SC_io_connect(SC_BUFFERED);
	     break;

	case 1 :
	     SC_mf_set_hooks();
	     break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_ERROR - On error print the message S and longjmp to
 *          - the recovery point specified by N.
 *
 * #bind PD_error fortran() scheme() python()
 */

void PD_error(const char *s, PD_major_op n)
   {PD_smp_state *pa;

    pa = _PD_get_state(-1);

    if (pa->err[0] == '\0')
       snprintf(pa->err, MAXLINE, "ERROR: %s\n", s);

    switch (n)
       {case PD_OPEN :
	     LONGJMP(pa->open_err, ABORT);

        case PD_CREATE :
	     LONGJMP(pa->create_err, ABORT);

        case PD_TRACE :
	     LONGJMP(pa->trace_err, ABORT);

        case PD_CLOSE :
	     LONGJMP(pa->close_err, ABORT);   

        case PD_READ :
	     LONGJMP(pa->read_err, ABORT);   

        case PD_PRINT :
	     LONGJMP(pa->print_err, ABORT);   

        case PD_WRITE :
	     LONGJMP(pa->write_err, ABORT);

	default :
	     break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_FREE - Recursively free memory allocated by PDBLib.
 *         - Such memory typically results from PD_read variant calls
 *         - involving indirectly referenced data).
 *
 * #bind PD_free fortran() scheme() python()
 */

pboolean PD_free(PDBfile *file ARG(,,cls), const char *type, void *var) 
   {pboolean rv;
    inti i, nb, nr, ni;
    intb bpi;
    char *pc, *p, *dtyp, *ityp;
    defstr *dp;
    memdes *member;
    void *pd;

    rv = B_T;

    if (var != NULL)
       {ityp = SC_dstrcpy(NULL, type);

	pc = (char *) var;
	nr = SC_ref_count(pc);
	nb = SC_arrlen(pc);
	if ((nb < 0) && (_PD_indirection(ityp)))
	   {PD_dereference(ityp);
	    pc = DEREF(pc);
	    nr = SC_ref_count(pc);
	    nb = SC_arrlen(pc);};
 
	if (pc == NULL)
	   {CFREE(ityp);
	    return(rv);};

	if (nb < 0)
	   {CFREE(ityp);
	    fprintf(stderr, "NOT SCORE ALLOCATED MEMORY %p - PD_FREE\n", pc);
	    return(B_F);};

	dp = _PD_type_lookup(file, PD_CHART_HOST, ityp);
 
	if (dp == NULL) 
	   {CFREE(ityp);
	    fprintf(stderr, "CANNOT LOOKUP %s IN FILE - PD_FREE\n", ityp);
	    return(B_F);};

	bpi = dp->size;
	ni  = nb / bpi;
	
/* only free the pointees if this is the only reference to the pointer */
	if (nr <= 1)
	   {dtyp = NULL;
	    for (i = 0; i < ni; i++)
	        {p = pc + i*bpi;

/* walk through each member of element p */
		 for (member = dp->members; member != NULL; member = member->next) 
		     {if (_PD_indirection(member->type)) 
			 {dtyp = SC_dstrcpy(dtyp, member->type);
			  PD_dereference(dtyp);
			  pd = *((void **) (p + member->member_offs));
			  PD_free(file, dtyp, pd);};};

		 if (_PD_indirection(ityp))
		    {p = DEREF(p);
		     if (SC_is_active_space(p, NULL, NULL)) 
		        CFREE(p);};

		 CFREE(dtyp);};};

/* free array or scalar compound itself */
	if (SC_is_active_space(pc, NULL, NULL)) 
	   CFREE(pc);

	CFREE(ityp);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_REMOVE_ENTRY - Remove the entry specified by NAME from symbol table
 *                 - of the PDBfile FILE.
 *                 - Return FALSE if the entry does not exist and
 *                 - return TRUE if successful.
 *
 * #bind PD_remove_entry fortran() scheme() python()
 */

pboolean PD_remove_entry(PDBfile *file ARG(,,cls), const char *name) 
   {pboolean rv;
    hasharr *tab;
    syment *ep;

    rv  = B_F;
    tab = file->symtab;

    ep = (syment *) SC_hasharr_def_lookup(tab, name);
    if (ep != NULL)
       {rv = B_T;

/* NOTE: SC_hasharr_remove will do raw free on ep and miss the contents
 * _PD_rl_syment_d will do it all properly
 * since we are doing 2 frees we have to add one to the count
 * simply switching the order would not respect the real count
 */
	SC_mark(ep, 1);
	SC_hasharr_remove(tab, name);
	_PD_rl_syment_d(ep);};

    return(rv);}
     
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_AUTOFIX_DENORM - Set the fix denorm flag in PDBfile FILE.
 *                   - If flag is TRUE, turn it on
 *                   - otherwise turn it off.
 *                   - Return the old flag value.
 *                   - If the fix denorm flag is on PD_read variants
 *                   - call PD_fix_denorm automatically.
 *
 * #bind PD_autofix_denorm fortran() scheme() python()
 */

int PD_autofix_denorm(PDBfile *file ARG(,,cls), int flag) 
    {int ov;

     ov = file->fix_denorm;

     file->fix_denorm = flag;
   
     return(ov);}
     
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_FIX_DENORM - Change the given floating point array VR so that all
 *               - instances of denormalized floating point values are
 *               - set to zero.
 *               - Return TRUE iff successful and
 *               - return FALSE if not a floating point type.
 *               -
 *               - Infinities          32 bit                 64 bit
 *               - Positive Inf:     7f80 0000         7ff0 0000 0000 0000
 *               - Negative Inf:     ff80 0000         fff0 0000 0000 0000
 *
 *               - NaNs                      32 bit
 *               - Signalling Nan:   7f80 0001 - 7fbf ffff
 *               -     or            ff80 0001 - ffbf ffff
 *               - Quiet Nan:        7fc0 0000 - 7fff ffff
 *               -     or            ffc0 0000 - ffff ffff
 *               -
 *               - NaNs                                64 bit
 *               - Signalling Nan:   7ff0 0000 0000 0001 - 7ff7 ffff ffff ffff
 *               -     or            fff0 0000 0000 0001 - fff7 ffff ffff ffff
 *               - Quiet Nan:        7ff8 0000 0000 0000 - 7fff ffff ffff ffff
 *               -     or            fff8 0000 0000 0000 - ffff ffff ffff ffff
 *
 * #bind PD_fix_denorm fortran() scheme() python()
 */

pboolean PD_fix_denorm(data_standard* std, const char *type,
		       int64_t ni, void *vr)
   {int id, ifp, reord, mask, rshift, nbits, nrem;
    int n_exp, n_mant, exp_sum, mant_sum, mant_bit, exp_bit;
    pboolean st;
    int *ord;
    int64_t i, j, in, nb;
    long *fmt;
    char *buf, *vtemp, *btemp, *var;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    nb  = 0;
    fmt = NULL;
    ord = NULL;
    st  = B_T;

    if (std == NULL)
       std = PD_gs.int_standard;

    id = SC_type_id(type, B_F);

    if (_PD_indirection(type) == B_T)
       {snprintf(pa->err, MAXLINE,
		 "ERROR: TYPE %s IS INDIRECT - PD_FIX_DENORM\n",
		 type);
	st = B_F;}

/* floating point type fmt */
    else if (SC_is_type_fp(id) == B_T)
       {ifp = SC_TYPE_FP(id);
	if (ifp < N_PRIMITIVE_FP)
	   {nb  = std->fp[ifp].bpi;
	    fmt = std->fp[ifp].format;
	    ord = std->fp[ifp].order;};}

/* complex floating point type fmt */
    else if (SC_is_type_cx(id) == B_T)
       {ifp = SC_TYPE_CPX(id);
	if (ifp < N_PRIMITIVE_FP)
	   {nb  = std->fp[ifp].bpi;
	    fmt = std->fp[ifp].format;
	    ord = std->fp[ifp].order;};}

    else
       {snprintf(pa->err, MAXLINE,
		 "ERROR: TYPE %s IS NOT FLOATING POINT - PD_FIX_DENORM\n",
		 type);
	st = B_F;};

    if ((st == B_T) && (ord != NULL))

/* make sure buf points to bytes that are in big endian byte order */
       {reord = B_F;

	for (i = 0L; i < nb; i++)
	    {if (ord[i] != (i + 1L))
	        {reord = B_T;
		 break;};}

	var = (char *) vr;
	if (reord == B_T)
	   {buf = CMAKE_N(char, nb * ni);
       
	    if (buf == NULL)
	       {PD_error("MEM ALLOC FAILED - PD_FIX_DENORM", PD_GENERIC);
		return(-1);};

	    for (i = 0L; i < nb; i++)
	        {btemp = &buf[ord[i] - 1];
		 vtemp = var + i;

		 for (j = 0L; j < ni; j++, vtemp += nb, btemp += nb)
		     *btemp = *vtemp;};}
	else
	   buf = var;

/* handle the general case: walk float values, zero-ing if necessary */
	if ((st == B_T) && (fmt != NULL))
	   {n_exp    = fmt[1];     /* # of bits in exponent */
	    n_mant   = fmt[2];     /* # of bits in mantissa */
	    exp_bit  = fmt[4] % 8; /* start bit of exponent mod byte size */
	    mant_bit = fmt[5] % 8; /* start bit of mantissa mod byte size */
       
/* start looking for denormalized floating point values */ 
	    for (in = 0L; in < ni; in++)
	        {vtemp = var + in*nb;
		 btemp = buf + in*nb;

/* sum exponent bits */
		 nrem   = n_exp;
		 nbits  = 8 - exp_bit;
		 mask   = (1 << nbits) - 1;
		 rshift = 0;
  
		 exp_sum = 0;
		 while (nrem > 0)
		    {exp_sum += (*btemp >> rshift) & mask;
		     nrem    -= nbits;
		     nbits    = min(8, nrem);
		     rshift   = 8 - nbits;
		     mask     = (1 << nbits) - 1;
              
/* shift ptr if we finished with this byte */ 
		     if (nrem > 0)
		        btemp++;};
  
/* if zero exponent - check for non-zero mantissa */
		 if (exp_sum == 0)

/* sum mantissa bits */
		    {nrem   = n_mant;
		     nbits  = 8 - mant_bit;
		     mask   = (1 << nbits) - 1;
		     rshift = 0;
      
		     mant_sum = 0;
		     while (nrem > 0)
		        {mant_sum += (*btemp >> rshift) & mask;
			 nrem    -= nbits;
			 nbits    = min(8, nrem);
			 rshift   = 8 - nbits;
			 mask     = (1 << nbits) - 1;
                   
			 if (nrem > 0)
			    btemp++;};
  
/* zero out nbytes of var if denormalized */
		     if (mant_sum != 0) 
		        {for (i = 0L; i < nb; i++)
			     *vtemp++ = '\0';};};};};
  
/* if we reordered into a buffer, then free the buffer */
	if (reord == B_T)
	   {CFREE(buf);};};

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

