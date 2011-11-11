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

int
 PD_DIM = INT_MIN,
 PD_buffer_size = -1;

data_standard
 *REQ_STANDARD = NULL;

data_alignment
 *REQ_ALIGNMENT = NULL;

JMP_BUF
 _PD_read_err,
 _PD_write_err,
 _PD_print_err,
 _PD_open_err,
 _PD_trace_err,
 _PD_close_err,
 _PD_create_err;

PFPDBwrite
 pdb_wr_hook = NULL;

PFPDBread
 pdb_rd_hook = NULL;

PDBfile
 *PD_vif = NULL;

char
 PD_err[MAXLINE];

PD_state
 _PD = { -1, 50L, -1, -1, FALSE, -1, FALSE,
	 (SC_communicator) 1, (SC_communicator) 2, (SC_communicator) 3, };

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_CREATE - create a PDB file and return an initialized PDBfile
 *           - if successful else NULL
 *
 * #bind PD_create fortran() scheme() python()
 */

PDBfile *PD_create(char *name)
   {PDBfile *file;

    _PD_init_state(FALSE);

    _PD_set_null_comm(-1);

    if (!_PD_IS_SMP_INIT)
       PD_init_threads(0, NULL);

    file = _PD_open_bin(name, "w", NULL);

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_OPEN - open an existing PDB file, extract the symbol table and
 *         - structure chart, and return a pointer to the PDB file
 *         - if successful else NULL
 *
 * #bind PD_open fortran() scheme() python()
 */

PDBfile *PD_open(char *name, char *mode)
   {PDBfile *file;

    _PD_init_state(FALSE);

    _PD_set_null_comm(-1);

    if (!_PD_IS_SMP_INIT)
       PD_init_threads(0, NULL);

    file = _PD_open_bin(name, mode, NULL);

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_FAMILY - return the next member of the file family if the given
 *           - file exceeds its maximum size
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
       {strcpy(name, of->name);
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

/* PD_OPEN_VIF - open a virtual internal PDB file
 *             - return the VIF iff successful
 *
 * #bind PD_open_vif fortran() scheme() python()
 */

PDBfile *PD_open_vif(char *name)
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

/* PD_COPY_TYPE - copy a TYPE definition from source file SF to
 *              - destination file DF
 *              - return TRUE iff successful
 *
 * #bind PD_copy_type fortran() scheme() python()
 */

int PD_copy_type(PDBfile *sf, PDBfile *df, char *type)
   {int rv;
    defstr *dp;
    memdes *lst;
    PD_smp_state *pa;

    rv = TRUE;

    pa = _PD_get_state(-1);

    dp = PD_inquire_type(sf, type);
    if (dp == NULL)
       {snprintf(pa->err, MAXLINE,
		 "ERROR: TYPE %s NOT FOUND - PD_COPY_TYPE\n", type);
        rv = FALSE;}

    else
       {lst = PD_copy_members(dp->members);
	dp  = _PD_defstr_inst(df, type, STRUCT_KIND, lst,
			      NO_ORDER, NULL, NULL, PD_CHART_HOST);
	if (dp == NULL)
	   {snprintf(pa->err, MAXLINE,
		     "ERROR: CANNOT CREATE TYPE %s - PD_COPY_TYPE\n", type);
	    rv = FALSE;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_CLOSE - Close a PDB file after writing out the symbol table and
 *          - structure chart.
 *          - Return TRUE if successful and FALSE otherwise.
 *
 * #bind PD_close fortran() scheme() python()
 */

int PD_close(PDBfile *file ARG(,,cls))
   {int ret;
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

/*
    _SC_prune_major_blocks();
*/

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_FLUSH - dump the data description tables containing the current
 *          - state of the PDB file
 *          - the tables are:
 *          -    structure chart
 *          -    symbol table
 *          -    extras table
 *          - the table addresses are also updated
 *          - NOTE: do not invoke PD_error in here because we do not
 *          - know whether we came in on a write or a close and therefore
 *          - do not know which jmpbuf to use
 *
 * #bind PD_flush fortran() scheme() python()
 */

int PD_flush(PDBfile *file ARG(,,cls))
   {int ret;

    ret = _PD_FLUSH_FILE(file);

    ret = _PD_safe_flush(file);

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
 
int _PD_read(PDBfile *file, char *fullpath, char *type, syment *ep, void *vr, 
             int nd, long *ind)
   {int rv, nf;
    char *typ;
    defstr *dp;

    file->req.oper = PD_READ;

#if 0
    CFREE(file->req.base_type);
    file->req.base_type = CSTRSAVE(type);
#endif

/* figure out information needed to call PD_fix_denorm later if needed */
    nf  = 0;
    typ = (type == NULL) ? PD_entry_type(ep) : type;
    if (file->fix_denorm &&  (typ != NULL))
       {dp = PD_inquire_type(file, typ);
	if ((dp != NULL) && (dp->kind == FLOAT_KIND))
	   {if (ind == NULL)
	       nf = _PD_comp_num(ep->dimensions);
	    else
	       nf = _PD_comp_nind(nd, ind, 3);};};

    if ((nd == -1) && (ind == NULL))
       rv = _PD_hyper_read(file, fullpath, type, ep, vr);
    else
       rv = _PD_indexed_read_as(file, fullpath, type, vr, nd, ind, ep);

/* if requested, set denorm float pt vars to zero */
    if (nf > 0)
       PD_fix_denorm(NULL, typ, nf, vr);

    _PD_request_unset(file);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_READ - read an entry from the PDB file pointed to by the
 *         - symbol table into the location pointed to by vr
 *         - return the number of item successfully read
 *         -
 *         - NOTE: VR must be a pointer to an object with the type
 *         - given by TYPE (PDBLib will allocated space if necessary)!
 *
 * #bind PD_read fortran() scheme() python()
 */

int PD_read(PDBfile *file ARG(,,cls), char *name, void *vr)
   {int rv;

    rv = PD_read_as(file, name, NULL, vr);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_READ_AS - read an entry from the PDB file pointed to by the
 *            - symbol table into the location pointed to by vr
 *            - convert to type TYPE regardless of symbol entry type
 *            - return the number of item successfully read
 *            -
 *            - NOTE: VR must be a pointer to an object with the type
 *            - given by TYPE (PDBLib will allocate space if necessary)!
 *
 * #bind PD_read_as fortran() scheme() python()
 */

int PD_read_as(PDBfile *file ARG(,,cls), char *name, char *type, void *vr)
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

/* PD_READ_AS_DWIM - do a special PD_read_as on the variable NAME
 *                 - if the variable is a direct type read it normally
 *                 - if it is indirect do a PD_read_as but copy what
 *                 - PDB returns into the space provided by the caller
 *                 - this frees the caller from having to worry about indirections
 *                 - provided the number of items to be read and hence the size
 *                 - of SPACE is large enough to hold the data
 *                 - return the number of items successfully read
 *                 - if less than NIX otherwise read NIX items and
 *                 - return -(number of items)
 *                 - NOTE: SPACE must be an object with the type
 *                 - given by TYPE (potenitally differs from the
 *                 - conventional PDB rule)
 *
 * #bind PD_read_as_dwim fortran() scheme() python()
 */

int PD_read_as_dwim(PDBfile *file ARG(,,cls), char *name,
		    char *outtype, long nix, void *space)
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
       {void *vr;
	char *ps, *pv;
	intb bpi;
	data_standard *std;
	hasharr *chrt;

	strcpy(s, intype);
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

/* PD_READ_ALT - read part of an entry from the PDB file pointed to by
 *             - the symbol table into the location pointed to by VR
 *             - IND contains one triplet of long ints per variable
 *             - dimension specifying start, stop, and step for the index
 *             - return the number of item successfully read
 *             -
 *             - NOTE: VR must be a pointer to an object with the type
 *             - given by TYPE (PDBLib will allocated space if necessary)!
 *
 * #bind PD_read_alt fortran() scheme() python()
 */

int PD_read_alt(PDBfile *file ARG(,,cls), char *name, void *vr, long *ind)
   {int rv;

    rv = PD_read_as_alt(file, name, NULL, vr, ind);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_READ_AS_ALT - read part of an entry from the PDB file pointed to by
 *                - the symbol table into the location pointed to by VR
 *                - IND contains one triplet of long ints per variable
 *                - dimension specifying start, stop, and step for the index
 *                - return the number of item successfully read
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

int PD_read_as_alt(PDBfile *file ARG(,,cls), char *name, char *type,
		   void *vr, long *ind)
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

syment *_PD_defent(PDBfile *file, char *name, char *outtype,
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

    lname = _PD_var_namef(file, name, bf);

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

/* PD_DEFENT - define an entry in the PDB file symbol table and
 *           - stake out the disk space but write nothing
 *           - any dimensional information is the NAME string
 *           - return the new symbol table entry if successful, and
 *           - return NULL otherwise
 *
 * #bind PD_defent fortran() scheme() python()
 */

syment *PD_defent(PDBfile *file ARG(,,cls), char *name, char *outtype)
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

/* PD_DEFENT_ALT - define an entry in the PDB file symbol table and
 *               - stake out the disk space but write nothing
 *               - dimensional information is specified by the number
 *               - of dimensions, ND, and the array of (min, max)
 *               - pairs of long ints in IND
 *               - return the new symbol table entry if successful, and
 *               - return NULL otherwise
 *
 * #bind PD_defent_alt fortran() scheme() python()
 */

syment *PD_defent_alt(PDBfile *file ARG(,,cls), char *name, char *outtype,
		      int nd, long *ind)
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

syment *_PD_write(PDBfile *file, char *name, char *intype, char *outtype,
		  void *vr, dimdes *dims, int appnd, int *pnew)
   {int c, new, ok;
    long number; 
    int64_t addr;
    char bf[MAXLINE];
    char *lname, *fullpath;
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

/* append a new block to an existing entry if TRUE */
    if (appnd)
       {strcpy(bf, name);

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

	    number = _PD_comp_num(dims);
	    addr   = _PD_get_next_address(file, PD_entry_type(ep), number, vr,
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

/* if the variable already exists use the existing file info */
    if (ep != NULL)

/* handle the zero space defent case */
       {if (ep->blocks == NULL)
	   {number = _PD_comp_num(dims);
	    addr   = _PD_get_next_address(file, PD_entry_type(ep), number, vr,
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

	lname = fullpath;}

/* if the variable doesn't exist define it to the file */
    else
       {if (appnd)
	   {if (lname != NULL)
	       *lname = '\0';
	    lname = bf;}

	else
	   lname = _PD_var_namef(NULL, fullpath, bf);

	number = _PD_comp_num(dims);
	addr   = _PD_get_next_address(file, outtype, number, vr,
				      FALSE, FALSE, FALSE);

	ep = _PD_mk_syment(outtype, number, addr, NULL, dims);
	_PD_e_install(file, lname, ep, TRUE);

	addr = _PD_annotate_text(file, ep, lname, addr, vr);

	new = TRUE;};

/* now write the data to the file */
    if (file->virtual_internal)
       {if (new)
	   {ad.memaddr = vr;
	    _PD_entry_set_address(ep, 0, ad.diskaddr);
	    SC_mark(vr, 1);}

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
	   file->chrtaddr = _PD_get_current_address(file, PD_WRITE);};

    _PD_request_unset(file);

    *pnew = new;

    ok = _PD_csum_var_write(file, lname, ep);
    SC_ASSERT(ok == TRUE);
  
    return(ep);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRITE - write NUMBER VAR's of type TYPE to the PDB file, FILE
 *          - make an entry in the file's symbol table
 *          - return TRUE iff successful
 *          -
 *          - NOTE: VR must be a pointer to an object with the type
 *          - given by TYPE!!!!
 *
 * #bind PD_write fortran() scheme() python()
 */

int PD_write(PDBfile *file ARG(,,cls), char *name, char *type, void *vr)
   {int rv;

    rv = PD_write_as(file, name, type, type, vr);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRITE_AS - write NUMBER VAR's of type INTYPE to the PDB file, FILE
 *             - as type OUTTYPE
 *             - make an entry in the file's symbol table
 *             - return TRUE iff successful
 *             -
 *             - NOTE: VR must be a pointer to an object with the type
 *             - given by TYPE!!!!
 *
 * #bind PD_write_as fortran() scheme() python()
 */

int PD_write_as(PDBfile *file ARG(,,cls), char *name,
		char *intype, char *outtype, void *vr)
   {int appnd, new, ie, rv;
    char *s, *t, *lname, fullpath[MAXLINE];
    syment *ep;
    dimdes *dims;
    PD_smp_state *pa;

    rv = FALSE;

    if (file != NULL)
       {pa = _PD_get_state(-1);

        if (PD_has_directories(file))
	   {lname = _PD_fixname(file, name);
	    strcpy(fullpath, lname);
	    t = strchr(lname, '.');
	    if (t != NULL)
	       *t = '\0';
	    s = fullpath;}
        else
	   {strcpy(fullpath, name);
	    lname = fullpath;
	    t = strchr(fullpath, '.');
	    if (t != NULL)
	       *t = '\0';
	    s = name;};

	appnd = pa->append_flag;
	dims  = _PD_ex_dims(lname, file->default_offset, &ie);
	ep    = file->tr->write(file, s, intype, outtype, vr, dims, appnd, &new);
	if (ep != NULL)
	   {if (new == FALSE)
	       {if (appnd == FALSE)
		   _PD_rl_dimensions(dims);
		_PD_rl_syment_d(ep);};
	    rv = TRUE;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRITE_ALT - write an entry of type TYPE to the PDB file, FILE
 *              - make an entry in the file's symbol table
 *              - return TRUE iff successful
 *              - the entry is named by NAME has ND dimensions and IND
 *              - contains the min and max (pairwise) of each dimensions
 *              - range
 *              - return the new syment if successful and NULL otherwise
 *              -
 *              - NOTE: VR must be a pointer to an object with the type
 *              - given by TYPE!!!!
 *
 * #bind PD_write_alt fortran() scheme() python()
 */

int PD_write_alt(PDBfile *file ARG(,,cls), char *name, char *type, void *vr,
		 int nd, long *ind)
   {int rv;

    rv = PD_write_as_alt(file, name, type, type, vr, nd, ind);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRITE_AS_ALT - write an entry of type INTYPE to the PDB file, FILE
 *                 - as type OUTTYPE
 *                 - make an entry in the file's symbol table
 *                 - return TRUE iff successful
 *                 - the entry has name, NAME, ND dimensions, and the ranges
 *                 - of the dimensions are given (min, max) pairwise in IND
 *                 - if successful and otherwise return NULL
 *                 -
 *                 - NOTE: VR must be a pointer to an object with the type
 *                 - given by TYPE!!!!
 *
 * #bind PD_write_as_alt fortran() scheme() python()
 */

int PD_write_as_alt(PDBfile *file ARG(,,cls), char *name,
		    char *intype, char *outtype,
		    void *vr, int nd, long *ind)
   {int i, new, appnd, ret, nc;
    long start, stop, step, leng;
    char index[MAXLINE], hname[MAXLINE], fullpath[MAXLINE];
    dimdes *dims, *next, *prev;
    syment *ep;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    prev = NULL;
    dims = NULL;

    strcpy(index, "(");

    for (i = 0; i < nd; i++)
        {start = ind[0];
         stop  = ind[1];
         step  = ind[2];
         ind += 3;

         SC_vstrcat(index, MAXLINE, "%ld:%ld:%ld,", start, stop, step);

         leng = stop - start + 1L;
         next = _PD_mk_dimensions(start, leng);
         if (dims == NULL)
            dims = next;
         else
            {prev->next = next;
	     SC_mark(next, 1);};

         prev = next;};

    nc = strlen(index);
    if (nc > 1)
       {index[nc-1] = ')';
        snprintf(hname, MAXLINE, "%s%s", name, index);}
    else
       strcpy(hname, name);

    if (intype != NULL)
       SC_trim_right(intype, " \t");
    if (outtype != NULL)
       SC_trim_right(outtype, " \t");

    appnd = pa->append_flag;
    strcpy(fullpath, _PD_fixname(file, hname));
    ep = file->tr->write(file, fullpath, intype, outtype,
			 vr, dims, pa->append_flag, &new);

    if (ep != NULL)
       {if (!new)
	   {if (!appnd)
	       _PD_rl_dimensions(dims);
	    _PD_rl_syment_d(ep);};
	ret = TRUE;}
    else
       ret = FALSE;

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_APPEND - append a new block of data to an existing entry
 *           - NOTE: VR must be a pointer to an object with the type
 *           - of the existing entry
 *
 * #bind PD_append fortran() scheme() python()
 */

int PD_append(PDBfile *file ARG(,,cls), char *name, void *vr)
   {int rv;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    pa->append_flag = TRUE;

    rv = PD_write_as(file, name, NULL, NULL, vr);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_APPEND_AS - append a new block of data to an existing entry
 *              - convert from INTYPE to the type of the existing data
 *              - NOTE: VR must be a pointer to an object with the type
 *              - of the existing entry
 *
 * #bind PD_append_as fortran() scheme() python()
 */

int PD_append_as(PDBfile *file ARG(,,cls), char *name,
		 char *intype, void *vr)
   {int rv;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);
    
    pa->append_flag = TRUE;

    rv = PD_write_as(file, name, intype, NULL, vr);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_APPEND_ALT - append a new block of data to an existing entry
 *               - NOTE: VR must be a pointer to an object with the type
 *               - of the existing entry
 *
 * #bind PD_append_alt fortran() scheme() python()
 */

int PD_append_alt(PDBfile *file ARG(,,cls), char *name,
		  void *vr, int nd, long *ind)
   {int rv;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);
    
    pa->append_flag = TRUE;

    rv = PD_write_as_alt(file, name, NULL, NULL, vr, nd, ind);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_APPEND_AS_ALT - append a new block of data to an existing entry
 *                  - convert from INTYPE to the type of the existing data
 *                  - NOTE: VR must be a pointer to an object with the type
 *                  - of the existing entry
 *
 * #bind PD_append_as_alt fortran() scheme() python()
 */

int PD_append_as_alt(PDBfile *file ARG(,,cls), char *name, char *intype,
		     void *vr, int nd, long *ind)
   {int rv;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);
    
    pa->append_flag = TRUE;

    rv = PD_write_as_alt(file, name, intype, NULL, vr, nd, ind);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_CAST - tell PDBLib that the type of a particular member (which must
 *         - be a pointer) is specified by another member (which must
 *         - be a character pointer). Put this information into the FILE chart.
 *         - return TRUE iff successful
 *
 * #bind PD_cast fortran() scheme() python()
 */

int PD_cast(PDBfile *file ARG(,,cls), char *type, char *memb, char *contr)
   {long i;
    hasharr *tab;
    defstr *dp;
    memdes *desc, *lst;

/* first make sure that "type" has a member "contr" which is of type char
 * this assumes that file->chart is a linked list
 */
    tab = file->chart;
    for (i = 0; SC_hasharr_next(tab, &i, NULL, NULL, (void **) &dp); i++)
        {if (strcmp(type, dp->type) != 0)
	    continue;

/* check that the contr is right */
	 for (desc = dp->members; desc != NULL; desc = desc->next)
	     {if (strcmp(contr, desc->name) != 0)
		 continue;

/* do this once, don't repeat in other chart */
	      if ((strcmp(desc->base_type, SC_CHAR_S) != 0) ||
		  !_PD_indirection(desc->type))
		 {PD_error("BAD CAST CONTROLLER - PD_CAST", PD_GENERIC);
		  return(FALSE);};
	      break;};};

/* now that we know such a cast is reasonable, add the cast to the host charts */

/* add the cast to the file->host_chart */
    tab = file->host_chart;
    for (i = 0; SC_hasharr_next(tab, &i, NULL, NULL, (void **) &dp); i++)
        {if (strcmp(type, dp->type) != 0)
	    continue;

	 for (desc = dp->members; desc != NULL; desc = desc->next)
	     {if (strcmp(memb, desc->name) != 0)
		 continue;

/* make an independent copy in case the one in the file chart is released */
	      CFREE(desc->cast_memb);
	      desc->cast_memb = CSTRSAVE(contr);
	      desc->cast_offs = _PD_member_location(contr, tab, dp, &lst);

	      SC_mark(contr, 1);};};

/* too slow? */
#if 0
    _PD_cast_insert(file->host_chart, type, memb, contr);
#endif

/* we don't need to add it to the charts to pass the tests - odd */
#if 0
    _PD_cast_insert(file->chart, type, memb, contr);
#endif

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CAST_INSERT - tell PDBLib that the type of a particular member (which
 *                 - must be a pointer) is specified by another member
 *                 - (which must be a character pointer)
 */

void _PD_cast_insert(hasharr* chart, char* type, char* memb, char* cast)
   {memdes *desc, *lst;
    defstr *dp;

/* the calculation of the hash may be a bit slow? */
    dp = (defstr *) SC_hasharr_def_lookup(chart, type);
    
    for (desc = dp->members; desc != NULL; desc = desc->next)
        {if (strcmp(memb, desc->name) != 0)
             continue;

/* make an independent copy in case the one passed in is released */
         desc->cast_memb = CSTRSAVE(cast);
         desc->cast_offs = _PD_member_location(cast, chart, dp, &lst);

	 SC_mark(cast, 1);};

   return;}

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

/* PD_ERROR - signal an error
 *
 * #bind PD_error fortran() scheme() python()
 */

void PD_error(char *s, PD_major_op n)
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

/* PD_FREE - free allocated memory recursively
 *
 * #bind PD_free fortran() scheme() python()
 */

int PD_free(PDBfile *file ARG(,,cls), char *type, void *var) 
   {int rv;
    inti i, nb, nr, ni;
    intb bpi;
    char *pc, *p, *dtyp, *ityp;
    defstr *dp;
    memdes *member;
    void *pd;

    rv = TRUE;

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
	   return(rv);

	if (nb < 0)
	   {fprintf(stderr, "NOT SCORE ALLOCATED MEMORY %p - PD_FREE\n", pc);
	    return(-1);};

	dp = _PD_type_lookup(file, PD_CHART_HOST, ityp);
 
	if (dp == NULL) 
	   {fprintf(stderr, "CANNOT LOOKUP %s IN FILE - PD_FREE\n", ityp);
	    return(-1);};

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

/* PD_REMOVE_ENTRY - remove a syment from the given hash table
 *                 - return FALSE if the NAME'd entry does not exist
 *                 - return TRUE if successful
 *
 * #bind PD_remove_entry fortran() scheme() python()
 */

int PD_remove_entry(PDBfile *file ARG(,,cls), char *name) 
   {int rv;
    hasharr *tab;
    syment *ep;

    rv  = FALSE;
    tab = file->symtab;

    ep = (syment *) SC_hasharr_def_lookup(tab, name);
    if (ep != NULL)
       {rv = TRUE;

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

/* PD_AUTOFIX_DENORM - if flag is TRUE, turn it on
 *                   - otherwise turn it off.
 *                   - return the old flag value
 *                   - PD_read calls then call PD_fix_denorm automagically
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

/* PD_FIX_DENORM - mutate given floating point array VR so that all
 *               - instances of denormalized floating point values are
 *               - set to zero 
 *               - return TRUE iff successful
 *               - return FALSE if not a floating point type
 *
 * #bind PD_fix_denorm fortran() scheme() python()
 */

int PD_fix_denorm(data_standard* std, char *type, int64_t ni, void *vr)
   {int id, ifp, st, reord, mask, rshift, nbits, nrem;
    int n_exp, n_mant, exp_sum, mant_sum, mant_bit, exp_bit;
    int *ord;
    int64_t i, j, in, nb;
    long *fmt;
    char *buf, *vtemp, *btemp, *var;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    nb  = 0;
    fmt = NULL;
    ord = NULL;
    st  = TRUE;

    if (std == NULL)
       std = INT_STANDARD;

    id = SC_type_id(type, FALSE);

    if (_PD_indirection(type) == TRUE)
       {snprintf(pa->err, MAXLINE,
		 "ERROR: TYPE %s IS INDIRECT - PD_FIX_DENORM\n",
		 type);
	st = FALSE;}

/* floating point type fmt */
    else if (SC_is_type_fp(id) == TRUE)
       {ifp = SC_TYPE_FP(id);
	if (ifp < N_PRIMITIVE_FP)
	   {nb  = std->fp[ifp].bpi;
	    fmt = std->fp[ifp].format;
	    ord = std->fp[ifp].order;};}

/* complex floating point type fmt */
    else if (SC_is_type_cx(id) == TRUE)
       {ifp = SC_TYPE_CPX(id);
	if (ifp < N_PRIMITIVE_FP)
	   {nb  = std->fp[ifp].bpi;
	    fmt = std->fp[ifp].format;
	    ord = std->fp[ifp].order;};}

    else
       {snprintf(pa->err, MAXLINE,
		 "ERROR: TYPE %s IS NOT FLOATING POINT - PD_FIX_DENORM\n",
		 type);
	st = FALSE;};

    if (st == TRUE)

/* make sure buf points to bytes that are in big endian byte order */
       {reord = FALSE;

	for (i = 0L; i < nb; i++)
	    {if (ord[i] != (i + 1L))
	        {reord = TRUE;
		 break;};}

	var = (char *) vr;
	if (reord == TRUE)
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
	if (st == TRUE)
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
	if (reord == TRUE)
	   {CFREE(buf);};};

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#if 0

Infinities          32 bit                        64 bit
Positive Inf:     7f80 0000                7ff0 0000 0000 0000
Negative Inf:     ff80 0000                fff0 0000 0000 0000

NaNs                      32 bit                             64 bit
Signalling Nan:   7f80 0001 - 7fbf ffff    7ff0 0000 0000 0001 - 7ff7 ffff ffff ffff
    or            ff80 0001 - ffbf ffff    fff0 0000 0000 0001 - fff7 ffff ffff ffff
Quiet Nan:        7fc0 0000 - 7fff ffff    7ff8 0000 0000 0000 - 7fff ffff ffff ffff
    or            ffc0 0000 - ffff ffff    fff8 0000 0000 0000 - ffff ffff ffff ffff

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

