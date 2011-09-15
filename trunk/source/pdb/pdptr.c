/*
 * PDPTR.C - pointer read/write support routines for PDBLib
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

/*
 * thread safety
 * 1) since the pointer list is contained in memory with format 3
 *    there is a thread hazard when things like PD_sizeof change out
 *    the list to avoid altering the "main" list.
 *    the solution is to maintain an array of lists.
 *    list 0 is the "main" file list.
 *    operations like PD_sizeof do not remove the main list, they
 *    save it, create a temporary list which is indexed by thread
 *    number (plus 1 to keep the main list untouched), and release
 *    the temporary one when finished.
 *    all the accessors use _PD_ptr_get_ap which checks for a list
 *    in the thread and otherwise return the "main" list.
 *
 * notation convention
 * accessor routines dealing in a single PD_address use _AD
 * accessor routines dealing in an array PD_addresses use _AP
 * accessor routines dealing in an array of arrays of PD_addresses use _APL
 *
 */

#define GET_N_AD(_f)       SC_array_get_n(_PD_ptr_get_ap(_f))

SC_thread_lock
 PD_ptr_lock = SC_LOCK_INIT_STATE,
 PD_dyn_lock = SC_LOCK_INIT_STATE;

static SC_array
 *_PD_ptr_get_ap(PDBfile *file),
 *_PD_ptr_set_ap(PDBfile *file, SC_array *nap, int serial);

/*--------------------------------------------------------------------------*/

/*                                 AD ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* _PD_PTR_INIT_AD - initialize a PD_address */

static void _PD_ptr_init_ad(void *a)
   {PD_address *ad;

    SC_LOCKON(PD_ptr_lock);

    ad = (PD_address *) a;

    ad->indx    = -1;
    ad->addr    = -1;
    ad->reta    = -1;
    ad->entry   = NULL;
    ad->ptr     = NULL;
    ad->written = -1;

    SC_LOCKOFF(PD_ptr_lock);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_GET_AD - return the N address from the current address list */

static PD_address *_PD_ptr_get_ad(PDBfile *file, int n)
   {PD_address *ad;
    SC_array *ap;

/*    SC_LOCKON(PD_ptr_lock); */

    ap = _PD_ptr_get_ap(file);
    ad = SC_array_get(ap, n);
#if 0
if (n != ad->indx)
   printf("-> %d %d\n", n, ad->indx);
#endif

/*    SC_LOCKOFF(PD_ptr_lock); */

    return(ad);}

/*--------------------------------------------------------------------------*/

/*                                 AP ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* _PD_PTR_MAKE_AP - allocate and initialize a file address/pointer list */

SC_array *_PD_ptr_make_ap(void)
   {PD_address ad;
    SC_array *a;

/* if uninitialized allocate them */
    a = CMAKE_ARRAY(PD_address, _PD_ptr_init_ad, 0);

/* grow the array to at least na elements */
    SC_array_resize(a, _PD.ninc, -1.0);

/* NULL is defined to be the first pointer - always */
    ad.indx    = 0;
    ad.addr    = 0;
    ad.reta    = -1;
    ad.entry   = NULL;
    ad.ptr     = NULL;
    ad.written = -1;

    SC_array_push(a, &ad);

    return(a);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_GET_AP - return the current pointer/address list */

static SC_array *_PD_ptr_get_ap(PDBfile *file)
   {int tid;
    SC_array *ap, *apl;

    tid = SC_current_thread() + 1;

    SC_LOCKON(PD_ptr_lock);

    apl = file->ap;

    ap = *(SC_array **) SC_array_get(apl, tid);
    if (ap == NULL)
       ap = *(SC_array **) SC_array_get(apl, 0);

    SC_LOCKOFF(PD_ptr_lock);

    return(ap);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_SET_AP - set the current pointer/address list
 *                - and return the old one
 */

static SC_array *_PD_ptr_set_ap(PDBfile *file, SC_array *nap, int serial)
   {int tid;
    SC_array *oap, *apl;

    if (serial == TRUE)
       tid = 0;
    else
       tid = SC_current_thread() + 1;

    SC_LOCKON(PD_ptr_lock);

    apl = file->ap;
    oap = *(SC_array **) SC_array_get(apl, tid);
    SC_array_set(apl, tid, &nap);

    SC_LOCKOFF(PD_ptr_lock);

    return(oap);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_INIT_AP - initialize a file address/pointer list */

static void _PD_ptr_init_ap(PDBfile *file, int serial)
   {SC_array *a;

    a = _PD_ptr_make_ap();

    _PD_ptr_set_ap(file, a, serial);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_FREE_AP - release the current address/pointer list for FILE */

static void _PD_ptr_free_ap(PDBfile *file, int serial)
   {SC_array *ap;

/* set the current list to NULL and get the old one back */
    ap = _PD_ptr_set_ap(file, NULL, serial);

/* free the old one */
    if (ap != NULL)
       SC_free_array(ap, NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_SAVE_AP - return the current address list in POA and
 *                 - initialize a new one
 */

void _PD_ptr_save_ap(PDBfile *file, SC_array **poa, char **pob, char *base)
   {

    if (poa != NULL)
       *poa = _PD_ptr_get_ap(file);

    SC_LOCKON(PD_ptr_lock);

    if (pob != NULL)
       *pob = file->ptr_base;

    if (base != NULL)
       file->ptr_base = base;

    SC_LOCKOFF(PD_ptr_lock);

    _PD_ptr_init_ap(file, FALSE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_RESTORE_AP - restore OA as the current address list
 *                    - the current one is freed
 */

void _PD_ptr_restore_ap(PDBfile *file, SC_array *oa, char *ob)
   {

    _PD_ptr_free_ap(file, FALSE);

    SC_LOCKON(PD_ptr_lock);

    if (ob != NULL)
       file->ptr_base = ob;

    SC_LOCKOFF(PD_ptr_lock);

    if (oa != NULL)
       _PD_ptr_set_ap(file, oa, FALSE);

    return;}

/*--------------------------------------------------------------------------*/

/*                                APL ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* _PD_PTR_FREE_APL_ITEM - free an SC_array from APL */

static int _PD_ptr_free_apl_item(void *a)
   {SC_array *ap;

    ap = *(SC_array **) a;
    SC_free_array(ap, NULL);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_FREE_APL - free the array of address/pointer lists */

void _PD_ptr_free_apl(PDBfile *file)
   {

    SC_free_array(file->ap, _PD_ptr_free_apl_item);

    file->ap = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_INIT_APL - initialize the array of address/pointer lists */

void _PD_ptr_init_apl(PDBfile *file)
   {

    file->ap = CMAKE_ARRAY(SC_array *, NULL, 0);

    _PD_ptr_init_ap(file, TRUE);

    return;}

/*--------------------------------------------------------------------------*/

/*                          POINTER LIST ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _PD_PTR_ALLOC_SPACE - using PI for info allocate and prep the space
 *                     - into which an indirect will be read
 *                     - and assign VR to it
 *                     - this is independent of the format version
 */

static char *_PD_ptr_alloc_space(PDBfile *file, char **vr,
				 PD_itag *pi, int asgn)
   {long bpi, ni;
    char *pv, *type;

    ni   = pi->nitems;
    type = pi->type;

    bpi = _PD_lookup_size(type, file->host_chart);
    if (bpi == -1)
       PD_error("CAN'T FIND NUMBER OF BYTES - _PD_PTR_ALLOC_SPACE",
                PD_READ);

    pv = CMAKE_N(char, ni*bpi);

    if (asgn == TRUE)
       {DEREF(vr) = pv;
	SC_mark(pv, 1);};

    return(pv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_FIX - constrain N to be a valid pointer index
 *             - any bad values are rerouted to the index for NULL
 */

static long _PD_ptr_fix(PDBfile *file, long n)
   {long ni;

    n  = max(n, 0L);
    ni = GET_N_AD(file);
    if (n >= ni)
       n = 0L;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_INDEX - given a pointer P in which an integer has been stored
 *               - extract and return the integer value
 */

static int _PD_ptr_index(void *p)
    {int i;

     i = *((int *) &p);

     return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INDEX_PTR - convert an integer into a void pointer
 *               - for later retrieval by _PD_ptr_index
 */

static void _PD_index_ptr(char *p, int i)
    {

     *((int *) p) = i;

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_GET_INDEX - verify pointer index N
 *                   - write it into BF if appropriate
 *                   - return the index
 *                   - NOTE: no pointer lookups here
 */

long _PD_ptr_get_index(PDBfile *file, long n, char *bf)
   {

/* constrain N to be a valid pointer index
 * any bad values are rerouted to the index for NULL
 */
    n = _PD_ptr_fix(file, n);

    if ((bf != NULL) && (file->use_itags == FALSE))
       _PD_index_ptr(bf, n);

    return(n);}

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

/*                        LOOKUP/INSTALL ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _PD_PTR_GET_ENTRY - return the syment associated with the Ith pointer */

static syment *_PD_ptr_get_entry(PDBfile *file, long i)
   {PD_address *ad;
    syment *ep;

    SC_LOCKON(PD_ptr_lock);

    i  = _PD_ptr_fix(file, i);
    ad = _PD_ptr_get_ad(file, i);
    ep = ad->entry;

    SC_LOCKOFF(PD_ptr_lock);

    return(ep);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_FIND_ADDR - return the PD_address associated with the address ADDR
 *                   - in the pointer list of FILE
 *                   - return NULL if there is none found
 */

static PD_address *_PD_ptr_find_addr(PDBfile *file, int64_t addr, int lck)
   {long i, ni;
    PD_address *ad, *lad;

    ad = NULL;

    if (file != NULL)
       {if (lck == TRUE)
	   SC_LOCKON(PD_ptr_lock);

	if (addr != 0)
	   {ni = GET_N_AD(file);
	    for (i = 0L; i < ni; i++)
	        {lad = _PD_ptr_get_ad(file, i);
		 if ((lad != NULL) && (addr == lad->addr))
		    {ad = lad;
		     break;};};};

	if (lck == TRUE)
	   SC_LOCKOFF(PD_ptr_lock);};

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_FIND_PTR - return the PD_address associated with the pointer VR
 *                  - in the (write) pointer list of FILE
 *                  - return NULL if there is none found
 */

static PD_address *_PD_ptr_find_ptr(PDBfile *file, void *vr, int lck)
   {long i, ni;
    PD_address *ad, *lad;

    ad = NULL;

    if (file != NULL)
       {if (lck == TRUE)
	   SC_LOCKON(PD_ptr_lock);

	if (vr != NULL)
	   {ni = GET_N_AD(file);
	    for (i = 0L; i < ni; i++)
	        {lad = _PD_ptr_get_ad(file, i);
		 if ((lad != NULL) && (vr == lad->ptr))
		    {ad = lad;
		     break;};};};

	if (lck == TRUE)
	   SC_LOCKOFF(PD_ptr_lock);};

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_FIND_NEXT - find the next PD_address in FILE associated with VR
 *                   - occuring after AD
 */

static PD_address *_PD_ptr_find_next(PDBfile *file, PD_address *ad, void *vr)
   {long i, ni;

    ni = GET_N_AD(file);
    i  = ad->indx;

#if 1
    while ((ad->written == TRUE) && (i < ni))
       {for (i++; i < ni; i++)
	    {ad = _PD_ptr_get_ad(file, i);
	     if (vr == ad->ptr)
	        break;};};
#endif

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_INSTALL_ADDR - install ADDR in the (read) pointer lists of FILE
 *                      - return the associated PD_address
 */

static PD_address *_PD_ptr_install_addr(PDBfile *file, int64_t addr, int lck)
   {long i;
    PD_address *ad;
    SC_array *ap;

    ad = NULL;

    if (file != NULL)
       {if (lck == TRUE)
	   SC_LOCKON(PD_ptr_lock);

	ap = _PD_ptr_get_ap(file);
	i  = SC_array_inc_n(ap, 1, 1);
	ad = SC_array_get(ap, i);

	_PD_ptr_init_ad(ad);
	ad->indx = i;
	ad->addr = addr;

	if (lck == TRUE)
	   SC_LOCKOFF(PD_ptr_lock);};

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_INSTALL_PTR - install VR in the (write) pointer list of FILE
 *                     - and mark it with the WRITE flag
 *                     - return the associated PD_address
 */

static PD_address *_PD_ptr_install_ptr(PDBfile *file, char *vr,
				       int write, int lck)
   {long i;
    PD_address *ad;
    SC_array *ap;

    ad = NULL;

    if (file != NULL)
       {if (lck == TRUE)
	   SC_LOCKON(PD_ptr_lock);

	ap = _PD_ptr_get_ap(file);
	i  = SC_array_inc_n(ap, 1, 1);
	ad = SC_array_get(ap, i);

	_PD_ptr_init_ad(ad);
	ad->indx    = i;
	ad->ptr     = vr;
	ad->written = write;

	if (lck == TRUE)
	   SC_LOCKOFF(PD_ptr_lock);};

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_INSTALL_ENTRY - install EP in the pointer list of FILE
 *                       - return the associated PD_address
 */

static PD_address *_PD_ptr_install_entry(PDBfile *file, long i,
					 syment *ep, int lck)
   {long ni;
    PD_address *ad;
    SC_array *ap;

    ad = NULL;

    if (file != NULL)
       {if (lck == TRUE)
	   SC_LOCKON(PD_ptr_lock);

	ap = _PD_ptr_get_ap(file);
	ad = SC_array_get(ap, i);

	ad->indx  = i;
	ad->entry = ep;
	ad->addr  = PD_entry_address(ep);

	ni = SC_array_get_n(ap);
	ni = max(i+1, ni);

	SC_array_set_n(ap, ni);

	if (lck == TRUE)
	   SC_LOCKOFF(PD_ptr_lock);};

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_REMOVE_ADDR - remove address indexed by I from FILE */

static void _PD_ptr_remove_addr(PDBfile *file, PD_address *ad, int lck)
   {long i, ni;
    PD_address *ada, *adb;
    SC_array *ap;

    if (lck == TRUE)
       SC_LOCKON(PD_ptr_lock);

    i = ad->indx;

    ap = _PD_ptr_get_ap(file);
    ni = SC_array_dec_n(ap, 1, 1);

    ada  = SC_array_get(ap, i);
    adb  = SC_array_get(ap, ni);
    *ada = *adb;
    _PD_ptr_init_ad(adb);

    if (lck == TRUE)
       SC_LOCKOFF(PD_ptr_lock);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_FOREACH - apply FNC to each PD_address in the lists of FILE */

static int _PD_ptr_foreach(PDBfile *file,
			   int (*f)(PD_address *ad, void *a), void *a)
   {int rv;
    long i, ni;
    PD_address *ad;

    SC_LOCKON(PD_ptr_lock);

    ni = GET_N_AD(file);

    rv = TRUE;
    for (i = 0L; i < ni; i++)
        {ad  = _PD_ptr_get_ad(file, i);
	 rv &= f(ad, a);};

    SC_LOCKOFF(PD_ptr_lock);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRAD - diagnostic print of address lists */

int dprad(PD_address *ad, void *a)
   {syment *ep;
    FILE *fp;

    fp = (FILE *) a;

    if (ad != NULL)
       {ep = ad->entry;
	if (ep == NULL)
	   PRINT(fp, "%3d %8ld %8ld  0x%08x      none\n",
		 ad->indx, (long) ad->addr,
		 (long) ad->reta, ad->ptr);
	else
	   PRINT(fp, "%3d %8ld %8ld  0x%08x  %8ld  %-20s\n",
		 ad->indx, (long) ad->addr,
		 (long) ad->reta, ad->ptr,
		 (long) PD_entry_number(ep), PD_entry_type(ep));};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRAP - diagnostic print of address lists */

void dprap(PDBfile *file, int n)
   {int ni;
    SC_array *ap;

    ap = SC_array_get(file->ap, n);
    ni = SC_array_get_n(ap);

    if (ni > 0)
       PRINT(stdout, "  #     Addr     Reta         Ptr     Entry Info\n");
    else
       PRINT(stdout, "No pointers currently tracked\n");

    _PD_ptr_foreach(file, dprad, stdout);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRAPL - diagnostic print of array of address lists */

void dprapl(PDBfile *file)
   {int i, nl;

    nl = SC_array_get_n(file->ap);
    for (i = 0; i < nl; i++)
        dprap(file, i);

    return;}

/*--------------------------------------------------------------------------*/

/*                            READ LIST ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* _PD_PTR_RD_LOOKUP - lookup ADDR to see if it has been previously
 *                   - read from FILE and remember it if not
 *                   - return the index of ADDR in the read pointer list
 *                   - return TRUE in PFRST the first time ADDR is seen
 */

static PD_address *_PD_ptr_rd_lookup(PDBfile *file, int64_t addr, int *pfrst)
   {int frst;
    PD_address *ad;

    frst = FALSE;

/* search the disk address list to see if this pointer is known */
    ad = _PD_ptr_find_addr(file, addr, TRUE);

/* add a new pointer */
    if ((ad == NULL) || (file->track_pointers == FALSE))
       {ad   = _PD_ptr_install_addr(file, addr, TRUE);
	frst = TRUE;};

    if (pfrst != NULL)
       *pfrst = frst;

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_READ - return the syment associated with ADDR
 *              - read from FILE at ADDR if need be 
 *              - in order to make the syment
 *              - return NULL on failure
 */

syment *_PD_ptr_read(PDBfile *file, int64_t addr, int force)
   {int frst;
    long n;
    syment *ep;
    PD_address *ad;

    ad = _PD_ptr_rd_lookup(file, addr, &frst);
    ep = ad->entry;

/* only partial reads can land here */
    if ((frst == TRUE) || (force == TRUE))
       {n  = _PD_rd_pointer(file, addr);
	ep = _PD_ptr_get_entry(file, n);};

    return(ep);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_READ_PUSH - add the itag PI info to the read pointer list of FILE
 *                   - and set VR to the appropriate data
 *                   - this is independent of the format version
 */

static void _PD_ptr_read_push(PDBfile *file, char **vr, PD_itag *pi,
			      int *pfrst)
   {int i, loc;
    int64_t addr, naddr;
    PD_address *ad;

    SC_LOCKON(PD_ptr_lock);

    if (file->use_itags == FALSE)
       {i  = _PD_ptr_index(*vr);
	ad = _PD_ptr_get_ad(file, i);
	if (ad == NULL)
	   {naddr  = -1L;
	    *pfrst = FALSE;}
	else
	   {if (ad->ptr == NULL)
	       {ad->ptr = _PD_ptr_alloc_space(file, vr, pi, FALSE);
		*pfrst = TRUE;}
	    else
	       *pfrst = FALSE;

	    DEREF(vr) = ad->ptr;
	    SC_mark(ad->ptr, 1);
		
/* this can happen when PD_reset_ptr_list is called */
	    if ((ad->addr == -1) && (pi->addr > 0))
	       ad->addr = pi->addr;

	    naddr = ad->addr;};}

    else
       {addr  = pi->addr;
	loc   = pi->flag;
	naddr = -1L;

	ad = _PD_ptr_rd_lookup(file, addr, pfrst);

	if (ad->ptr == NULL)
	   ad->ptr = _PD_ptr_alloc_space(file, vr, pi, FALSE);

	DEREF(vr) = ad->ptr;
	SC_mark(ad->ptr, 1);

/* this pointer has not been seen before */
	if (*pfrst == TRUE)

/* GOTCHA: watch for new case of loc == LOC_BLOCK which means a
 * discontiguous block - deal with this, if and when it arises
 */
	   {if (loc != LOC_HERE)
	       {pi->addr = _PD_get_current_address(file, PD_READ);

/* jump to the place where the actual data starts */
		naddr = addr + pi->length;};}

/* we have seen this pointer before */
	else
	   {if (loc == LOC_HERE)
	       {naddr = ad->reta;

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

    SC_LOCKOFF(PD_ptr_lock);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_RD_ITAGS - read itags and setup the read of an indirection
 *                  - return TRUE iff there is indirect data to be read
 */

int _PD_ptr_rd_itags(PDBfile *file, char **vr, PD_itag *pi)
   {int frst;
    char *p;

    if ((*file->rd_itag)(file, *vr, pi) == FALSE)
       PD_error("BAD ITAG - _PD_PTR_RD_ITAGS", PD_READ);

/* handle NULL pointer case */
    if ((pi->addr == -1) || (pi->nitems == 0L))
       {*vr  = NULL;
	frst = FALSE;}

    else if ((_PD_IS_SEQUENTIAL) || (file->use_itags == FALSE))
       _PD_ptr_read_push(file, vr, pi, &frst);

    else
       {frst = TRUE;
	p    = _PD_ptr_alloc_space(file, vr, pi, TRUE);
	SC_ASSERT(p != NULL);};

    return(frst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_RD_INSTALL_ADDR - install ADDR as the return address in the
 *                         - pointer list for FILE 
 */

void _PD_ptr_rd_install_addr(PDBfile *file, int64_t addr, int loc)
   {int64_t here;
    PD_address *ad;

    if ((_PD_IS_SEQUENTIAL) || (file->use_itags == FALSE))
       {ad   = _PD_ptr_rd_lookup(file, addr, NULL);
	here = _PD_get_current_address(file, PD_READ);

	ad->reta = here;};

/* restore the file pointer to its original location if necessary */
    if (loc != LOC_HERE)
       _PD_set_current_address(file, addr, SEEK_SET, PD_READ);

    return;}

/*--------------------------------------------------------------------------*/

/*                           WRITE LIST ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* _PD_PTR_WR_LOOKUP - lookup VR to see if it has been previously
 *                   - written to FILE and remember it if not
 *                   - return the index of VR in the write pointer list
 *                   - return the location of the data in PLOC
 *                   - location is LOC_HERE, LOC_OTHER, or LOC_BLOCK
 */

PD_address *_PD_ptr_wr_lookup(PDBfile *file, void *vr, int *ploc,
			      int write, int lck)
   {int loc;
    PD_address *ad;

    if (lck == TRUE)
       SC_LOCKON(PD_ptr_lock);

    loc = LOC_OTHER;

/* search the pointer list to see if this pointer is known */
    ad = _PD_ptr_find_ptr(file, vr, FALSE);

    if (file->track_pointers == FALSE)
       {loc = LOC_HERE;

        if ((ad == NULL) || (write == FALSE))
	   ad = _PD_ptr_install_ptr(file, vr, write, FALSE);

        else
            {ad = _PD_ptr_find_next(file, ad, vr);
	     ad->written = write;};}                 

/* add a new pointer */
    else if (ad == NULL)
       {loc = LOC_HERE;
	ad  = _PD_ptr_install_ptr(file, vr, write, FALSE);}

    else if (ad->addr == -1)
       loc = LOC_HERE;

    if (lck == TRUE)
       SC_LOCKOFF(PD_ptr_lock);

    if (ploc != NULL)
       *ploc = loc;

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_WR_SYMENT - make a syment for NI of data with type at the
 *                   - current location in FILE
 *                   - fill in AD with the address info
 */

void _PD_ptr_wr_syment(PDBfile *file, PD_address *ad, char *type,
		       long ni, int64_t addr)
   {char name[MAXLINE];
    syment *ep;

    if (ni > 0)
       {snprintf(name, MAXLINE, "%s%ld", file->ptr_base, (long) ad->indx);
	ep = _PD_mk_syment(type, ni, addr, NULL, NULL);
	_PD_e_install(file, name, ep, TRUE);

	if (file->format_version > 2)
	   {if (ad != NULL)
	       {ad->addr  = addr;
		ad->entry = ep;};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_WR_ITAGS - handle the memory of pointers and write the itags
 *                  - correctly
 */

int _PD_ptr_wr_itags(PDBfile *file, void *vr, long ni, char *type)
   {int rv, loc;
    long n;
    int64_t addr;
    PD_address *ad;

/* save the address of the itag because
 * we don't know the data address yet
 */
    addr = _PD_get_current_address(file, PD_WRITE);
    loc  = LOC_HERE;
    ad   = NULL;

    SC_LOCKON(PD_ptr_lock);

    if ((_PD_IS_SEQUENTIAL) || (file->use_itags == FALSE))

/* remove addr from lists of pointers and disk addresses previously read */
       {if (addr != 0)
	   {ad = _PD_ptr_find_addr(file, addr, FALSE);
	    if (ad != NULL)
	       _PD_ptr_remove_addr(file, ad, FALSE);};

	ad = _PD_ptr_wr_lookup(file, vr, &loc, TRUE, FALSE);

	if (loc == LOC_HERE)
	   ad->addr = addr;
	else
	   addr = ad->addr;}

    else
       {n  = _PD_ptr_get_n_spaces(file, TRUE);
	ad = _PD_ptr_get_ad(file, n);};

/* write the itag to the file */
    (*file->wr_itag)(file, ad, ni, type, addr, loc);

    SC_LOCKOFF(PD_ptr_lock);

    rv = (loc == LOC_HERE);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                         LIST MANAGEMENT ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* _PD_PTR_ENTRY_ITAG - check the ITAG PI and
 *                    - if it indicates ITAGless operation fill it
 *                    - with the entry data from read pointer N
 *                    - return FALSE for a NULL pointer otherwise TRUE
 */

int _PD_ptr_entry_itag(PDBfile *file, PD_itag *pi, char *p)
   {int n, rv;
    syment *ep;

    n  = _PD_ptr_index(p);
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

/* _PD_PTR_RESET_AD - reset the pointer list element poining to VR */

int _PD_ptr_reset_ad(PD_address *ad, void *a)
   {

    if (ad != NULL)
       {ad->addr = -1;
        ad->ptr  = NULL;};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_RESET_PTR_LIST - THREADSAFE
 *                   - reset the pointer lists for the given file
 *                   - so that indirect connectivity bookkeeping
 *                   - is re-initialized this will give the application
 *                   - some control over this process
 *
 * #bind PD_reset_ptr_list fortran() scheme() python()
 */

int PD_reset_ptr_list(PDBfile *file ARG(,,cls))
   {int rv;

    rv = TRUE;
    if ((_PD_IS_SEQUENTIAL) || (file->use_itags == FALSE))
       rv = _PD_ptr_foreach(file, _PD_ptr_reset_ad, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_RESET - reset the pointer list element poining to VR */

int _PD_ptr_reset(PDBfile *file, char *vr)
   {int rv;
    PD_address *ad;

    ad = _PD_ptr_find_ptr(file, vr, TRUE);
    rv = _PD_ptr_reset_ad(ad, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_REGISTER - add a symbol table entry for a pointee to
 *                  - the read pointer list
 */

static int _PD_ptr_register(haelem *hp, void *a)
   {int ok;
    long i, nc;
    char *name;
    syment *ep;
    PDBfile *file;

    file = (PDBfile *) a;

    ok = SC_haelem_data(hp, &name, NULL, (void **) &ep);
    SC_ASSERT(ok == TRUE);

    if ((file != NULL) && (ep != NULL))
       {nc = strlen(file->ptr_base);
	if (strncmp(name, file->ptr_base, nc) == 0) 
	   {i = SC_stoi(name+nc);
            _PD_ptr_install_entry(file, i, ep, TRUE);};};

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

