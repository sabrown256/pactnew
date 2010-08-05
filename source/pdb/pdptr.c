/*
 * PDPTR.C - pointer read/write support routines for PDBLib
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

#define GET_N_AP(_f)       SC_array_get_n(_PD_ptr_get_curr(_f))

SC_thread_lock
 PD_ptr_lock = SC_LOCK_INIT_STATE,
 PD_dyn_lock = SC_LOCK_INIT_STATE;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_GET_CURR - return the current pointer/address list */

SC_array *_PD_ptr_get_curr(PDBfile *file)
   {SC_array *ap;

    ap = file->ap;

    return(ap);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_SET_CURR - set the current pointer/address list
 *                  - and return the old one
 */

SC_array *_PD_ptr_set_curr(PDBfile *file, SC_array *nap)
   {SC_array *oap;

    oap = file->ap;
    file->ap = nap;

    return(oap);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_INIT_ADDR - initialize a PD_address */

static void _PD_ptr_init_addr(void *a)
   {PD_address *ap;

    SC_LOCKON(PD_ptr_lock);

    ap = (PD_address *) a;

    ap->indx    = -1;
    ap->addr    = -1;
    ap->reta    = -1;
    ap->entry   = NULL;
    ap->ptr     = NULL;
    ap->written = -1;

    SC_LOCKOFF(PD_ptr_lock);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_INIT_AP - initialize the file address pointer list */

void _PD_ptr_init_ap(PDBfile *file, long nl)
   {long na;
    PD_address al;
    SC_array *a;

    SC_LOCKON(PD_ptr_lock);

/* if uninitialized allocate them */
    a = SC_MAKE_ARRAY("_PD_PTR_INIT_AP", PD_address, _PD_ptr_init_addr);

    na = max(_PD.ninc, nl);

/* grow the array to at least na elements */
    SC_array_resize(a, na, -1.0);

/* NULL is defined to be the first pointer - always */
    al.indx    = 0;
    al.addr    = 0;
    al.reta    = -1;
    al.entry   = NULL;
    al.ptr     = NULL;
    al.written = -1;
    SC_array_push(a, &al);

    _PD_ptr_set_curr(file, a);

    SC_LOCKOFF(PD_ptr_lock);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_SAVE_AP - save the address list
 *                 - terribly thread hazardous
 */

void _PD_ptr_save_ap(PDBfile *file, SC_array **poa, char **pob, char *base)
   {

    SC_LOCKON(PD_ptr_lock);

    if (poa != NULL)
       *poa = _PD_ptr_get_curr(file);

    if (pob != NULL)
       *pob = file->ptr_base;

    if (base != NULL)
       file->ptr_base = base;

    _PD_ptr_init_ap(file, 0);

    SC_LOCKOFF(PD_ptr_lock);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_RESTORE_AP - restore the address list
 *                    - terribly thread hazardous
 */

void _PD_ptr_restore_ap(PDBfile *file, SC_array *oa, char *ob)
   {

    SC_LOCKON(PD_ptr_lock);

    _PD_ptr_free_list(file);

    if (ob != NULL)
       file->ptr_base = ob;

    if (oa != NULL)
       _PD_ptr_set_curr(file, oa);

    SC_LOCKOFF(PD_ptr_lock);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_GET_AD - return the N address from the current address list */

static PD_address *_PD_ptr_get_ad(PDBfile *file, int n)
   {PD_address *ap;

    ap = SC_array_get(_PD_ptr_get_curr(file), n);

    return(ap);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_GET_ENTRY - return the syment associated with the Ith pointer */

syment *_PD_ptr_get_entry(PDBfile *file, long i)
   {PD_address *ap;
    syment *ep;

    ap = _PD_ptr_get_ad(file, i);
    ep = ap->entry;

    return(ep);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRAP - diagnostic print of address lists */

void dprap(PDBfile *file)
   {int i, ni;
    PD_address *ap;
    syment *ep;

    ni = GET_N_AP(file);

    if (ni > 0)
       PRINT(stdout, "  #     Addr     Reta         Ptr     Entry Info\n");
    else
       PRINT(stdout, "No pointers currently tracked\n");

    for (i = 0; i < ni; i++)
        {ap = _PD_ptr_get_ad(file, i);
	 ep = ap->entry;
	 if (ep == NULL)
	    PRINT(stdout, "%3d %8ld %8ld  0x%08x      none\n",
		  ap->indx, (long) ap->addr,
		  (long) ap->reta, ap->ptr);
	 else
	    PRINT(stdout, "%3d %8ld %8ld  0x%08x  %8ld  %-20s\n",
		  ap->indx, (long) ap->addr,
		  (long) ap->reta, ap->ptr,
		  (long) PD_entry_number(ep), PD_entry_type(ep));};

    SFREE(ap);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_INDEX - given a pointer P in which an integer has been stored
 *               - extract and return the integer value
 */

int _PD_ptr_index(void *p)
    {int i;

     i = *((int *) &p);

     return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INDEX_PTR - convert an integer into a void pointer
 *               - for later retrieval by _PD_ptr_index
 */

void _PD_index_ptr(char *p, int i)
    {

     *((int *) p) = i;

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_FIND_PTR - return the index of the pointer VR in the
 *                  - pointer list of FILE
 *                  - return the index of the next pointer if not found
 *                  - return -1 if there is no list or if ADDR is invalid
 */

static long _PD_ptr_find_ptr(PDBfile *file, void *vr)
   {long i, ni;
    PD_address *ap;

    ni = GET_N_AP(file);

    if ((ni > 0L) && (vr != NULL))
       {for (i = 0L; i < ni; i++)
	    {ap = _PD_ptr_get_ad(file, i);
	     if ((ap != NULL) && (vr == ap->ptr))
	        break;};}
    else
       i = -1L;

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_FIND_ADDR - return the index of the address ADDR in the
 *                   - pointer list of FILE
 *                   - return the index of the next pointer if not found
 *                   - return -1 if there is no list or if ADDR is invalid
 */

static long _PD_ptr_find_addr(PDBfile *file, off_t addr)
   {long i, ni;
    PD_address *ap;

    ni = GET_N_AP(file);

    if ((ni > 0L) && (addr != 0))
       {for (i = 0L; i < ni; i++)
	    {ap = _PD_ptr_get_ad(file, i);
	     if ((ap != NULL) && (addr == ap->addr))
	        break;};}
    else
       i = -1L;

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_FREE_LIST - release the pointer lists for FILE */

void _PD_ptr_free_list(PDBfile *file)
   {SC_array *ap;

    ap = _PD_ptr_set_curr(file, NULL);
    if (ap != NULL)
       {SC_LOCKON(PD_ptr_lock);

	SC_free_array(ap, NULL);

	SC_LOCKOFF(PD_ptr_lock);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_RESET_PTR_LIST - THREADSAFE
 *                   - reset the pointer lists for the given file
 *                   - so that indirect connectivity bookkeeping
 *                   - is re-initialized this will give the application
 *                   - some control over this process
 */

int PD_reset_ptr_list(PDBfile *file)
   {long i, ni;
    PD_address *ap;

    if ((_PD_IS_SEQUENTIAL) || (file->use_itags == FALSE))
       {ni = GET_N_AP(file);

	for (i = 0L; i < ni; i++)
	    {ap       = _PD_ptr_get_ad(file, i);
	     ap->addr = -1;
	     ap->ptr  = NULL;};};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_RESET - reset the pointer list element poining to VR */

int _PD_ptr_reset(PDBfile *file, char *vr)
   {long i, ni;
    PD_address *ap;

    ni = GET_N_AP(file);

    i = _PD_ptr_find_ptr(file, vr);
    if ((0 <= i) && (i < ni))
       {ap       = _PD_ptr_get_ad(file, i);
	ap->addr = -1;
        ap->ptr  = NULL;};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_REGISTER - add a symbol table entry for a pointee to
 *                  - the read pointer list
 */

static int _PD_ptr_register(haelem *hp, void *a)
   {int ok;
    long i, nc, ni;
    char *name;
    syment *ep;
    PD_address *ap;
    PDBfile *file;

    file = (PDBfile *) a;
    ok   = SC_haelem_data(hp, &name, NULL, (void **) &ep);
    if ((file != NULL) && (ep != NULL))
       {nc = strlen(file->ptr_base);
	if (strncmp(name, file->ptr_base, nc) == 0) 
	   {i = SC_stoi(name+nc);

	    ap = _PD_ptr_get_ad(file, i);
	    ap->indx  = i;
	    ap->entry = ep;
	    ap->addr  = PD_entry_address(ep);

	    ni = GET_N_AP(file);
	    ni = max(i+1, ni);

	    SC_array_set_n(_PD_ptr_get_curr(file), ni);};};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_OPEN_SETUP - scan the symbol table for pointers and initialize
 *                    - the read pointer list
 */

void _PD_ptr_open_setup(PDBfile *file)
   {

    SC_hasharr_foreach(file->symtab, _PD_ptr_register, file);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_GET_N_SPACES - return n_dyn_spaces in a thread safe way */

static int _PD_ptr_get_n_spaces(PDBfile *file, int inc)
   {int np;

    SC_LOCKON(PD_dyn_lock);

    if (inc == TRUE)
       np = file->n_dyn_spaces++;
    else
       np = file->n_dyn_spaces;

    SC_LOCKOFF(PD_dyn_lock);

    np++;

    return(np);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_ENTRY_ITAG - check the ITAG PI and
 *                    - if it indicates ITAGless operation fill it
 *                    - with the entry data from read pointer N
 *                    - return FALSE for a NULL pointer otherwise TRUE
 */

int _PD_ptr_entry_itag(PDBfile *file, int n, PD_itag *pi)
   {int rv;
    syment *ep;

    ep = _PD_ptr_get_entry(file, n);

    if (ep == NULL)
       {pi->type   = NULL;
	pi->nitems = 0;
	pi->addr   = 0;
	pi->flag   = LOC_HERE;

	rv = FALSE;}

    else
       {pi->type   = PD_entry_type(ep);
	pi->nitems = PD_entry_number(ep);
	pi->addr   = PD_entry_address(ep);
	pi->flag   = LOC_HERE;
	pi->length = 0;

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_REMOVE_RD - remove ADDR from the read pointer lists of FILE */

static void _PD_ptr_remove_rd(PDBfile *file, off_t addr)
   {long i, ni;
    PD_address *apa, *apb;

    ni = GET_N_AP(file);

/* remove addr from lists of pointers and disk addresses previously read */
    if ((ni > 0) && (addr != 0))
       {i = _PD_ptr_find_addr(file, addr);
	if (i < ni)
	   {ni = SC_array_dec_n(_PD_ptr_get_curr(file), 1, 1);

	    apa = _PD_ptr_get_ad(file, i);
	    apb = _PD_ptr_get_ad(file, ni);
	    *apa = *apb;
	    _PD_ptr_init_addr(apb);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_ADD_ENTRY - install the pointer VR in the pointer list
 *                   - of FILE and marked with the WRITE flag
 */

static long _PD_ptr_add_entry(PDBfile *file, char *vr, int write)
   {long i;
    PD_address *ap;

    i = -1;

    if (file != NULL)
       {SC_LOCKON(PD_ptr_lock);

	i = SC_array_inc_n(_PD_ptr_get_curr(file), 1, 1);

	ap = _PD_ptr_get_ad(file, i);

	_PD_ptr_init_addr(ap);
	ap->indx    = i;
	ap->ptr     = vr;
	ap->written = write;

	SC_LOCKOFF(PD_ptr_lock);};

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_WR_LOOKUP - lookup VR to see if it has been previously
 *                   - written to FILE and remember it if not
 *                   - return the index of VR in the write pointer list
 *                   - return the location of the data in PLOC
 *                   - location is LOC_HERE, LOC_OTHER, or LOC_BLOCK
 */

long _PD_ptr_wr_lookup(PDBfile *file, void *vr, int *ploc, int write)
   {int loc;
    long i, ni;
    PD_address *ap;

    SC_LOCKON(PD_ptr_lock);

    ni = GET_N_AP(file);

    loc = LOC_OTHER;

/* search the pointer list to see if this pointer is known */
    i = _PD_ptr_find_ptr(file, vr);

    ap = _PD_ptr_get_ad(file, i);

    if (file->track_pointers == FALSE)
       {loc = LOC_HERE;

        if ((i >= ni) || (write == FALSE))
	   i = _PD_ptr_add_entry(file, vr, write);

        else
            {while ((ap->written == TRUE) && (i < ni))
                {for (i++; i < ni; i++)
		     {ap = _PD_ptr_get_ad(file, i);
		      if (vr == ap->ptr)
		         break;};};

             if (i >= ni)
	        i = _PD_ptr_add_entry(file, vr, write);
             else
	        ap->written = write;};}                 

/* add a new pointer */
    else if (i >= ni)
       {loc = LOC_HERE;
	i   = _PD_ptr_add_entry(file, vr, write);}

    else if (ap->addr == -1)
       loc = LOC_HERE;

    SC_LOCKOFF(PD_ptr_lock);

    *ploc = loc;

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_WR_ITAGS - handle the memory of pointers and write the itags
 *                  - correctly
 */

int _PD_ptr_wr_itags(PDBfile *file, void *vr, long nitems, char *type)
   {int rv, loc;
    long n;
    off_t addr;
    PD_address *ap;

/* save the address of the itag because
 * we don't know the data address yet
 */
    addr = _PD_get_current_address(file, PD_WRITE);
    loc  = LOC_HERE;
    n    = -1L;

    if ((_PD_IS_SEQUENTIAL) || (file->use_itags == FALSE))
       {_PD_ptr_remove_rd(file, addr);

	n  = _PD_ptr_wr_lookup(file, vr, &loc, TRUE);
	ap = _PD_ptr_get_ad(file, n);

	if (loc == LOC_HERE)
	   {ap->addr = addr;
	    ap->indx = n;}
	else
	   addr = ap->addr;}

    else
       n = _PD_ptr_get_n_spaces(file, TRUE);

/* write the itag for the read */
    (*file->wr_itag)(file, n, nitems, type, addr, loc);

    rv = (loc == LOC_HERE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_ALLOC_SPACE - using PI for info allocate and prep the space
 *                     - into which an indirect will be read
 *                     - and assign VR to it
 *                     - this is independent of the format version
 */

static char *_PD_ptr_alloc_space(PDBfile *file, char **vr,
				 PD_itag *pi, int asgn)
   {long bpi, nitems;
    char *pv, *type;

    nitems = pi->nitems;
    type   = pi->type;

    bpi = _PD_lookup_size(type, file->host_chart);
    if (bpi == -1)
       PD_error("CAN'T FIND NUMBER OF BYTES - _PD_PTR_ALLOC_SPACE",
                PD_READ);

    pv = (char *) SC_alloc_nzt(nitems, bpi, "_PD_PTR_ALLOC_SPACE:pv", NULL);

    if (asgn == TRUE)
       {DEREF(vr) = pv;
	SC_mark(pv, 1);};

    return(pv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_RD_LOOKUP - lookup ADDR to see if it has been previously
 *                   - read from FILE and remember it if not
 *                   - return the index of ADDR in the read pointer list
 *                   - return TRUE in PFRST the first time ADDR is seen
 */

static long _PD_ptr_rd_lookup(PDBfile *file, off_t addr, int *pfrst)
   {int frst;
    long i, ni;
    PD_address *ap;

    frst = FALSE;

/* search the disk address list to see if this pointer is known */
    i = _PD_ptr_find_addr(file, addr);

/* add a new pointer */
    ni = GET_N_AP(file);
    if ((i >= ni) || (file->track_pointers == FALSE))
       {SC_array_inc_n(_PD_ptr_get_curr(file), 1, -1);
	i = ni;

	ap = _PD_ptr_get_ad(file, i);

	_PD_ptr_init_addr(ap);
	ap->indx = i;
	ap->addr = addr;

	frst = TRUE;};

    *pfrst = frst;

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_RD_INSTALL_ADDR - install ADDR as the return address in the
 *                         - pointer list for FILE 
 */

void _PD_ptr_rd_install_addr(PDBfile *file, off_t addr, int loc)
   {int frst;
    long i;
    off_t here;
    PD_address *ap;

    if ((_PD_IS_SEQUENTIAL) || (file->use_itags == FALSE))
       {i    = _PD_ptr_rd_lookup(file, addr, &frst);
	here = _PD_get_current_address(file, PD_READ);

	ap = _PD_ptr_get_ad(file, i);
	ap->reta = here;
	SFREE(ap);};

/* restore the file pointer to its original location if necessary */
    if (loc != LOC_HERE)
       _PD_set_current_address(file, addr, SEEK_SET, PD_READ);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_READ_PUSH - add the itag PI info to the read pointer list of FILE
 *                   - and set VR to the appropriate data
 *                   - this is independent of the format version
 */

static long _PD_ptr_read_push(PDBfile *file, char **vr, PD_itag *pi,
			      int *pfrst)
   {int i, loc;
    long rv;
    off_t addr, naddr;
    PD_address *ap;

    if (file->use_itags == FALSE)
       {i = _PD_ptr_index(*vr);
	if (i == 0)
	   {rv     = i;
	    naddr  = -1L;
	    *pfrst = FALSE;}
	else
	   {ap = _PD_ptr_get_ad(file, i);

	    if (ap->ptr == NULL)
	       {ap->ptr = _PD_ptr_alloc_space(file, vr, pi, FALSE);
		rv     = i;
		*pfrst = TRUE;}
	    else
	       {rv     = -1L;
		*pfrst = FALSE;};

	    DEREF(vr) = ap->ptr;
	    SC_mark(ap->ptr, 1);
		
/* this can happen when PD_reset_ptr_list is called */
	    if ((ap->addr == -1) && (pi->addr > 0))
	       ap->addr = pi->addr;

	    naddr = ap->addr;};}

    else
       {addr  = pi->addr;
	loc   = pi->flag;
	naddr = -1L;

	i = _PD_ptr_rd_lookup(file, addr, pfrst);

	ap = _PD_ptr_get_ad(file, i);

	if (ap->ptr == NULL)
	   ap->ptr = _PD_ptr_alloc_space(file, vr, pi, FALSE);

	DEREF(vr) = ap->ptr;
	SC_mark(ap->ptr, 1);

/* this pointer has not been seen before */
	if (*pfrst == TRUE)
	   {rv = i;

/* GOTCHA: watch for new case of loc == LOC_BLOCK which means a
 * discontiguous block - deal with this, if and when it arises
 */
	    if (loc != LOC_HERE)
	       {pi->addr = _PD_get_current_address(file, PD_READ);

/* jump to the place where the actual data starts */
		naddr = addr + pi->length;};}

/* we have seen this pointer before */
	else
	   {rv = -1L;

	    if (loc == LOC_HERE)
	       {naddr = ap->reta;

/* NOTE: this is terrible - we have the data but we have to read it
 * anyway because we don't have an address to skip over to
 * this happens when you write then read without an intervening close
 * and open
 */
		if (naddr == -1L)
		   *pfrst = TRUE;};};};

/* position the file location correctly */
    if (naddr != -1L)
       _PD_set_current_address(file, naddr, SEEK_SET, PD_READ);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_RD_ITAGS - read itags and setup the read of an indirection
 *                  - return TRUE iff there is indirect data to be read
 */

int _PD_ptr_rd_itags(PDBfile *file, char **vr, PD_itag *pi)
   {int frst;
    long i;
    char *p;

    if ((*file->rd_itag)(file, *vr, pi) == FALSE)
       PD_error("BAD ITAG - _PD_PTR_RD_ITAGS", PD_READ);

/* handle NULL pointer case */
    if ((pi->addr == -1) || (pi->nitems == 0L))
       {*vr  = NULL;
	frst = FALSE;}

    else if ((_PD_IS_SEQUENTIAL) || (file->use_itags == FALSE))
       i = _PD_ptr_read_push(file, vr, pi, &frst);

    else
       {frst = TRUE;
	p    = _PD_ptr_alloc_space(file, vr, pi, TRUE);};

    return(frst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_WR_SYMENT - make a syment for NITEMS of data with type at the
 *                   - current location in FILE
 */

void _PD_ptr_wr_syment(PDBfile *file, long n, char *type,
		       long nitems, off_t addr)
   {char name[MAXLINE];
    syment *ep;
    PD_address *ap;

    if (nitems > 0)
       {SC_LOCKON(PD_ptr_lock);

	snprintf(name, MAXLINE, "%s%ld", file->ptr_base, n);
	ep = _PD_mk_syment(type, nitems, addr, NULL, NULL);
	_PD_e_install(file, name, ep, TRUE);

        ap = _PD_ptr_get_ad(file, n);
	if (ap != NULL)
	   {ap->addr  = addr;
	    ap->entry = ep;};

	SC_LOCKOFF(PD_ptr_lock);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_FIX - constrain N to be a valid pointer index
 *             - any bad values are rerouted to the index for NULL
 */

long _PD_ptr_fix(PDBfile *file, long n)
   {long ni;

    n  = max(n, 0L);
    ni = GET_N_AP(file);
    if (n >= ni)
       n = 0L;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_GET_INDEX - extract a pointer index from BF
 *                   - according to the standards in FILE
 *                   - return the index
 */

long _PD_ptr_get_index(PDBfile *file, char *bf)
   {int fbpi, hbpi;
    long n;
    char *in, *out;
    data_standard *hs, *fs;
    PD_byte_order ford, hord;

    hs = file->host_std;
    fs = file->std;

    ford = fs->long_order;
    fbpi = fs->ptr_bytes;
    hord = hs->long_order;
    hbpi = hs->long_bytes;
       
    in  = bf;
    out = (char *) &n;
    _PD_iconvert(&out, &in, 1L, fbpi, ford, hbpi, hord, FALSE, FALSE);

/* constrain N to be a valid pointer index
 * any bad values are rerouted to the index for NULL
 */
    n = _PD_ptr_fix(file, n);

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_READ - read and return the index into the read pointer list
 *              - value at the current location in FILE
 *              - return -1 on failure
 */

long _PD_ptr_read(PDBfile *file, off_t addr, int force)
   {int frst;
    long n;
    size_t bpi, nr;
    char bf[MAXLINE];
    FILE *fp;
    PD_address *ap;

    n = _PD_ptr_rd_lookup(file, addr, &frst);

/* only partial reads can land here */
    if ((frst == TRUE) || (force == TRUE))
       {ap = _PD_ptr_get_ad(file, n);

/* undo the addition of ADDR to the read pointer list */
	SC_array_dec_n(_PD_ptr_get_curr(file), frst, -1);

	ap->addr = 0;

	fp  = file->stream;
	bpi = file->std->ptr_bytes;

/* set the file location */
	_PD_set_current_address(file, addr, SEEK_SET, PD_READ);

/* read the pointer value from the file location */
	nr = lio_read(bf, bpi, 1, fp);
	if (nr == 1)
	   n = _PD_ptr_get_index(file, bf);
	else
	   n = -1;

/* restore the file location */
	_PD_set_current_address(file, -bpi, SEEK_CUR, PD_READ);};

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

