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

#define NULL_ADDR         ((void *) -1)

typedef int (*PFPtrMth)(PD_address *ad, void *a);

SC_thread_lock
 PD_ptr_lock = SC_LOCK_INIT_STATE,
 PD_dyn_lock = SC_LOCK_INIT_STATE;

static long
 _PD_ptr_install(adloc *al, long i, void *vr);

static adloc
 *_PD_ptr_get_al(PDBfile *file, int lck),
 *_PD_ptr_set_al(PDBfile *file, adloc *nal, int serial);

/*--------------------------------------------------------------------------*/

/*                                 AD ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* _PD_PTR_INIT_NULL - initialize a PD_address */

static void _PD_ptr_init_null(void *a)
   {PD_address **pad;

    pad = (PD_address **) a;

    *pad = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MAKE_ADDR - make and return a PD_address */

static PD_address *_PD_make_addr(long i, int64_t addr, char *ptr, syment *ep)
   {PD_address *ad;

    ad = CMAKE(PD_address);

    ad->indx    = i;
    ad->addr    = addr;
    ad->reta    = -1;
    ad->entry   = ep;
    ad->ptr     = ptr;
    ad->written = -1;

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FREE_ADDR - release a PD_address from an SC_array */

static int _PD_free_addr(void *a)
   {PD_address *ad;

    ad = *(PD_address **) a;

    CFREE(ad);

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                                 AL ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* _PD_MAKE_ADLOC - allocate and initialize a file address/pointer list */

adloc *_PD_make_adloc(void)
   {PD_address *ad;
    SC_array *ap;
    hasharr *ah;
    adloc *al;

/* NULL is defined to be the first pointer - always */
    ad = _PD_make_addr(0, 0, NULL, NULL);

    ah = SC_make_hasharr(_PD.ninc, FALSE, SC_HA_ADDR_KEY, 4);
    ap = CMAKE_ARRAY(PD_address *, _PD_ptr_init_null, 0);

    al = CMAKE(adloc);
    al->ap = ap;
    al->ah = ah;

    _PD_ptr_install(al, 0, NULL_ADDR);

    SC_array_resize(ap, _PD.ninc, -1.0);
    SC_array_push(ap, &ad);

    return(al);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FREE_ADLOC - release adloc instance AL */

void _PD_free_adloc(adloc *al)
   {

    if (al != NULL)
       {if (SC_safe_to_free(al) == TRUE)
	   {SC_free_array(al->ap, _PD_free_addr);
	    SC_free_hasharr(al->ah, NULL, NULL);};

	CFREE(al);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_GET_AL - return the current pointer/address list
 *                - LCK is TRUE iff a lock is not already on
 */

static adloc *_PD_ptr_get_al(PDBfile *file, int lck)
   {int tid;
    SC_array *apl;
    adloc *al;

    tid = SC_current_thread() + 1;

    if (lck == TRUE)
       SC_LOCKON(PD_ptr_lock);

    apl = file->ap;

    al = *(adloc **) SC_array_get(apl, tid);
    if (al == NULL)
       al = *(adloc **) SC_array_get(apl, 0);

    if (lck == TRUE)
       SC_LOCKOFF(PD_ptr_lock);

    return(al);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_SET_AL - set the current pointer/address list
 *                - and return the old one
 */

static adloc *_PD_ptr_set_al(PDBfile *file, adloc *nal, int serial)
   {int tid;
    SC_array *apl;
    adloc *oal;

    if (serial == TRUE)
       tid = 0;
    else
       tid = SC_current_thread() + 1;

    SC_LOCKON(PD_ptr_lock);

    apl = file->ap;
    oal = *(adloc **) SC_array_get(apl, tid);
    SC_array_set(apl, tid, &nal);

    SC_mark(oal, -1);
    SC_mark(nal, 1);

    SC_LOCKOFF(PD_ptr_lock);

    return(oal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_INIT_AL - initialize a file address/pointer list */

static void _PD_ptr_init_al(PDBfile *file, int serial)
   {adloc *al;

    al = _PD_make_adloc();

    _PD_ptr_set_al(file, al, serial);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_FREE_AL - release the current address/pointer list for FILE */

static void _PD_ptr_free_al(PDBfile *file, int serial)
   {adloc *al;

/* set the current list to NULL and get the old one back */
    al = _PD_ptr_set_al(file, NULL, serial);

/* free the old one */
    if (al != NULL)
       _PD_free_adloc(al);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_SAVE_AL - return the current address list in POA and
 *                 - initialize a new one
 */

void _PD_ptr_save_al(PDBfile *file, adloc **poa, char **pob, char *base)
   {adloc *al;

    SC_LOCKON(PD_ptr_lock);

    if (poa != NULL)
       {al = _PD_ptr_get_al(file, FALSE);
	SC_mark(al, 1);
	*poa = al;};

    if (pob != NULL)
       *pob = file->ptr_base;

    if (base != NULL)
       file->ptr_base = base;

    SC_LOCKOFF(PD_ptr_lock);

    _PD_ptr_init_al(file, FALSE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_RESTORE_AL - restore OA as the current address list
 *                    - the current one is freed
 */

void _PD_ptr_restore_al(PDBfile *file, adloc *oa, char *ob)
   {

    _PD_ptr_free_al(file, FALSE);

    SC_mark(oa, -1);

    SC_LOCKON(PD_ptr_lock);

    if (ob != NULL)
       file->ptr_base = ob;

    SC_LOCKOFF(PD_ptr_lock);

    if (oa != NULL)
       _PD_ptr_set_al(file, oa, FALSE);

    return;}

/*--------------------------------------------------------------------------*/

/*                                APL ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* _PD_PTR_FREE_APL_ITEM - free an SC_array from APL */

static int _PD_ptr_free_apl_item(void *a)
   {adloc *al, **pal;

    pal  = (adloc **) a;
    al   = *pal;
    *pal = NULL;

    _PD_free_adloc(al);

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

    file->ap = CMAKE_ARRAY(adloc *, NULL, 0);

    _PD_ptr_init_al(file, TRUE);

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
   {inti ni;
    intb bpi;
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

static long _PD_ptr_fix(adloc *al, long n)
   {inti ni;
    SC_array *ap;

    ap = al->ap;
    ni = SC_array_get_n(ap);

    n = max(n, 0L);
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

     i = *((long *) &p);

     return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INDEX_PTR - convert an integer into a void pointer
 *               - for later retrieval by _PD_ptr_index
 */

static void _PD_index_ptr(char *p, int i)
    {

     *((long *) p) = i;

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_GET_INDEX - verify pointer index N
 *                   - write it into BF if appropriate
 *                   - return the index
 *                   - NOTE: no pointer lookups here
 */

long _PD_ptr_get_index(PDBfile *file, long n, char *bf)
   {adloc *al;

    al = _PD_ptr_get_al(file, TRUE);

/* constrain N to be a valid pointer index
 * any bad values are rerouted to the index for NULL
 */
    n = _PD_ptr_fix(al, n);

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

/* _PD_PTR_GET_AD - return the N address from the current address list */

static PD_address *_PD_ptr_get_ad(adloc *al, int n)
   {PD_address *ad, **pad;
    SC_array *ap;

    ap  = al->ap;
    pad = (PD_address **) SC_array_get(ap, n);
    ad  = (pad != NULL) ? *pad : NULL;

    if (ad == NULL)
       {ad = _PD_make_addr(n, -1, NULL, NULL);
	SC_array_set(ap, n, &ad);};

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_LOOKUP - return the N address from the current address list */

static long _PD_ptr_lookup(adloc *al, void *vr)
   {long i;
    long *pi;
    hasharr *ah;

    ah = al->ah;

    pi = (long *) SC_hasharr_def_lookup(ah, vr);
    i  = (pi != NULL) ? *pi : -1L;

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_INSTALL - return the N address from the current address list */

static long _PD_ptr_install(adloc *al, long i, void *vr)
   {long *pi;
    hasharr *ah;

    ah = al->ah;

    pi  = CMAKE(long);
    *pi = i;

/* used to do this but saved memory trying to reuse any existing entry
 * no test found a problem - changed 11/14/2011
 * remove if no problems arise by 06/2012
    SC_hasharr_install(ah, vr, pi, SC_LONG_S, 1, -1);
 */
    SC_hasharr_install(ah, vr, pi, SC_LONG_S, 7, -1);

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_SET_SPACE - allocate space for VR and set AD to point to it */

static void _PD_ptr_set_space(PDBfile *file, PD_address *ad,
			      char **vr, PD_itag *pi)
   {adloc *al;

    al = _PD_ptr_get_al(file, TRUE);

    ad->ptr = _PD_ptr_alloc_space(file, vr, pi, FALSE);
    _PD_ptr_install(al, ad->indx, ad->ptr);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_GET_ENTRY - return the syment associated with the Ith pointer */

static syment *_PD_ptr_get_entry(adloc *al, long i)
   {PD_address *ad;
    syment *ep;

    SC_LOCKON(PD_ptr_lock);

    i  = _PD_ptr_fix(al, i);
    ad = _PD_ptr_get_ad(al, i);
    ep = ad->entry;

    SC_LOCKOFF(PD_ptr_lock);

    return(ep);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_FIND_ADDR - return the PD_address associated with the address ADDR
 *                   - in the pointer list of FILE
 *                   - LCK is TRUE iff a lock is not already on
 *                   - return NULL if there is none found
 */

static PD_address *_PD_ptr_find_addr(adloc *al, int64_t addr, int lck)
   {long l;
    void *vr;
    PD_address *ad, *lad;

    ad = NULL;

    if (al != NULL)
       {if (lck == TRUE)
	   SC_LOCKON(PD_ptr_lock);

	if (addr != 0)
	   {vr = _SC_to_address(addr);

	    l = _PD_ptr_lookup(al, vr);
	    if (l != -1)
	       {lad = _PD_ptr_get_ad(al, l);
		if ((lad != NULL) && (addr == lad->addr))
		   ad = lad;};};

	if (lck == TRUE)
	   SC_LOCKOFF(PD_ptr_lock);};

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_FIND_PTR - return the PD_address associated with the pointer VR
 *                  - in the (write) pointer list of FILE
 *                  - LCK is TRUE iff a lock is not already on
 *                  - return NULL if there is none found
 */

static PD_address *_PD_ptr_find_ptr(adloc *al, void *vr, int lck)
   {long l;
    PD_address *ad, *lad;

    ad = NULL;

    if (al != NULL)
       {if (lck == TRUE)
	   SC_LOCKON(PD_ptr_lock);

	if (vr != NULL)
	   {l = _PD_ptr_lookup(al, vr);
	    if (l != -1)
	       {lad = _PD_ptr_get_ad(al, l);
		if ((lad != NULL) && (vr == lad->ptr))
		   ad = lad;};};

	if (lck == TRUE)
	   SC_LOCKOFF(PD_ptr_lock);};

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_FIND_NEXT - find the next PD_address in FILE associated with VR
 *                   - occuring after AD that has not be written
 */

static PD_address *_PD_ptr_find_next(adloc *al, PD_address *ad, void *vr)
   {inti i, ni;
    SC_array *ap;

    ap = al->ap;
    ni = SC_array_get_n(ap);

    i = ad->indx;

    while ((ad->written == TRUE) && (i < ni))
       {for (i++; i < ni; i++)
	    {ad = _PD_ptr_get_ad(al, i);
	     if (vr == ad->ptr)
	        break;};};

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_INSTALL_ADDR - install ADDR in the (read) pointer lists of FILE
 *                      - return the associated PD_address
 *                      - LCK is TRUE iff a lock is not already on
 */

static PD_address *_PD_ptr_install_addr(adloc *al, int64_t addr, int lck)
   {long i;
    void *vr;
    PD_address *ad;
    SC_array *ap;

    ad = NULL;

    if (al != NULL)
       {if (lck == TRUE)
	   SC_LOCKON(PD_ptr_lock);

	ap = al->ap;

	i = SC_array_get_n(ap);

	vr = _SC_to_address(addr);
	ad = _PD_make_addr(i, addr, NULL, NULL);

/* hasharr */
	_PD_ptr_install(al, i, vr);

/* array */
	SC_array_inc_n(ap, 1, 1);

	SC_array_set(ap, i, &ad);

	ad = _PD_ptr_get_ad(al, i);

	if (lck == TRUE)
	   SC_LOCKOFF(PD_ptr_lock);};

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_INSTALL_PTR - install VR in the (write) pointer list of FILE
 *                     - and mark it with the WRITE flag
 *                     - LCK is TRUE iff a lock is not already on
 *                     - return the associated PD_address
 */

static PD_address *_PD_ptr_install_ptr(adloc *al, char *vr,
				       int write, int lck)
   {long i;
    void *key;
    PD_address *ad;
    SC_array *ap;

    ad = NULL;

    if (al != NULL)
       {if (lck == TRUE)
	   SC_LOCKON(PD_ptr_lock);

	ap = al->ap;

	i = SC_array_get_n(ap);

	ad = _PD_make_addr(i, -1, vr, NULL);
	ad->written = write;

/* hasharr */
	key = (vr == NULL) ? NULL_ADDR : vr;
	_PD_ptr_install(al, i, key);

/* array */
	SC_array_inc_n(ap, 1, 1);

	SC_array_set(ap, i, &ad);

	ad = _PD_ptr_get_ad(al, i);

	if (lck == TRUE)
	   SC_LOCKOFF(PD_ptr_lock);};

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_INSTALL_ENTRY - install EP in the pointer list of FILE
 *                       - LCK is TRUE iff a lock is not already on
 *                       - return the associated PD_address
 */

static PD_address *_PD_ptr_install_entry(adloc *al, long i,
					 syment *ep, int lck)
   {int64_t addr;
    void *vr;
    PD_address *ad;
    SC_array *ap;

    ad = NULL;

    if (al != NULL)
       {if (lck == TRUE)
	   SC_LOCKON(PD_ptr_lock);

	ap = al->ap;

	addr = PD_entry_address(ep);
	vr   = _SC_to_address(addr);
	ad   = _PD_make_addr(i, addr, NULL, ep);

/* hasharr */
	_PD_ptr_install(al, i, vr);

/* array */
	SC_array_set(ap, i, &ad);

	if (lck == TRUE)
	   SC_LOCKOFF(PD_ptr_lock);};

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_REMOVE_ADDR - remove address indexed by I from FILE
 *                     - LCK is TRUE iff a lock is not already on
 */

static void _PD_ptr_remove_addr(adloc *al, PD_address *ad, int lck)
   {

#if 0
    void *vr;
    hasharr *ah;

    if (file != NULL)
       {if (lck == TRUE)
	   SC_LOCKON(PD_ptr_lock);

	vr = _SC_to_address(ad->addr);

	ah = al->ah;

	SC_hasharr_remove(ah, vr);

	if (lck == TRUE)
	   SC_LOCKOFF(PD_ptr_lock);};
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_FOREACH - apply FNC to each PD_address in the lists of FILE */

static int _PD_ptr_foreach(adloc *al, PFPtrMth f, void *a)
   {int rv;
    inti i, ni;
    PD_address *ad;
    SC_array *ap;

    SC_LOCKON(PD_ptr_lock);

    rv = TRUE;

    ap = al->ap;
    ni = SC_array_get_n(ap);

    for (i = 0L; i < ni; i++)
        {ad  = _PD_ptr_get_ad(al, i);
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

void dprap(adloc *al, int n)
   {inti ni;

    ni = SC_array_get_n(al->ap);

    if (ni > 0)
       PRINT(stdout, "  #     Addr     Reta         Ptr     Entry Info\n");
    else
       PRINT(stdout, "No pointers currently tracked\n");

    _PD_ptr_foreach(al, dprad, stdout);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRAPL - diagnostic print of array of address lists */

void dprapl(PDBfile *file)
   {int i, nl;
    adloc *al;

    al = _PD_ptr_get_al(file, TRUE);

    nl = SC_array_get_n(file->ap);
    for (i = 0; i < nl; i++)
        dprap(al, i);

    return;}

/*--------------------------------------------------------------------------*/

/*                            READ LIST ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* _PD_PTR_RD_LOOKUP - lookup ADDR to see if it has been previously
 *                   - read from FILE and remember it if not
 *                   - LCK is TRUE iff a lock is not already on
 *                   - return the index of ADDR in the read pointer list
 *                   - return TRUE in PFRST the first time ADDR is seen
 */

static PD_address *_PD_ptr_rd_lookup(PDBfile *file, int64_t addr,
				     int lck, int *pfrst)
   {int frst;
    PD_address *ad;
    adloc *al;

    if (lck == TRUE)
       SC_LOCKON(PD_ptr_lock);

    al = _PD_ptr_get_al(file, FALSE);

    frst = FALSE;

/* search the disk address list to see if this pointer is known */
    ad = _PD_ptr_find_addr(al, addr, FALSE);

/* add a new pointer */
    if ((ad == NULL) || (file->track_pointers == FALSE))
       {ad   = _PD_ptr_install_addr(al, addr, FALSE);
	frst = TRUE;};

    if (lck == TRUE)
       SC_LOCKOFF(PD_ptr_lock);

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
    adloc *al;

    ep = NULL;
    al = _PD_ptr_get_al(file, TRUE);

    ad = _PD_ptr_rd_lookup(file, addr, TRUE, &frst);
    if (ad != NULL)
       ep = ad->entry;

/* only partial reads can land here */
    if ((frst == TRUE) || (force == TRUE))
       {n  = _PD_rd_pointer(file, addr);
	ep = _PD_ptr_get_entry(al, n);};

    return(ep);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_READ_PUSH - add the itag PI info to the read pointer list of FILE
 *                   - and set VR to the appropriate data
 *                   - this is independent of the format version
 */

static void _PD_ptr_read_push(PDBfile *file, char **vr, PD_itag *pi,
			      int *pfrst)
   {int i;
    int64_t addr, naddr;
    PD_data_location loc;
    PD_address *ad;
    adloc *al;

    SC_LOCKON(PD_ptr_lock);

    al = _PD_ptr_get_al(file, FALSE);

    if (file->use_itags == FALSE)
       {i  = _PD_ptr_index(*vr);
	ad = _PD_ptr_get_ad(al, i);
	if (ad == NULL)
	   {naddr  = -1L;
	    *pfrst = FALSE;}
	else
	   {if (ad->ptr == NULL)
	       {_PD_ptr_set_space(file, ad, vr, pi);
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

	ad = _PD_ptr_rd_lookup(file, addr, FALSE, pfrst);
	if (ad != NULL)
	   {if (ad->ptr == NULL)
	       _PD_ptr_set_space(file, ad, vr, pi);

	    DEREF(vr) = ad->ptr;
	    SC_mark(ad->ptr, 1);};

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
	       {if (ad != NULL)
		   naddr = ad->reta;

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

    frst = FALSE;
    if (vr != NULL)
       {if ((*file->rd_itag)(file, *vr, pi) == FALSE)
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
	    SC_ASSERT(p != NULL);};};

    return(frst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_RD_INSTALL_ADDR - install ADDR as the return address in the
 *                         - pointer list for FILE 
 */

void _PD_ptr_rd_install_addr(PDBfile *file, int64_t addr,
			     PD_data_location loc)
   {int64_t here;
    PD_address *ad;

    if ((_PD_IS_SEQUENTIAL) || (file->use_itags == FALSE))
       {ad = _PD_ptr_rd_lookup(file, addr, TRUE, NULL);
	if (ad != NULL)
	   {here = _PD_get_current_address(file, PD_READ);
	    ad->reta = here;};};

/* restore the file pointer to its original location if necessary */
    if (loc != LOC_HERE)
       _PD_set_current_address(file, addr, SEEK_SET, PD_READ);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_REMOVE_ENTRY - remove address referenced by syment EP from FILE
 *                      - LCK is TRUE iff a lock is not already on
 */

void _PD_ptr_remove_entry(PDBfile *file, syment *ep, int lck)
   {

#if 0
    if (file != NULL)
       {if (lck == TRUE)
	   SC_LOCKON(PD_ptr_lock);

	int64_t addr;
	PD_address *ad;
	adloc *al;

	al   = _PD_ptr_get_al(file, FALSE);
	addr = PD_entry_address(ep);
	ad   = _PD_ptr_rd_lookup(file, addr, FALSE, NULL);
	_PD_ptr_remove_addr(al, ad, FALSE);

/* this will be freed when the hash table is releases
 * we had to free the contents here
	CFREE(ad);
 */

	if (lck == TRUE)
	   SC_LOCKOFF(PD_ptr_lock);};
#endif

    return;}

/*--------------------------------------------------------------------------*/

/*                           WRITE LIST ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* _PD_PTR_WR_LOOKUP - lookup VR to see if it has been previously
 *                   - written to FILE and remember it if not
 *                   - LCK is TRUE iff a lock is not already on
 *                   - return the index of VR in the write pointer list
 *                   - return the location of the data in PLOC
 *                   - location is LOC_HERE, LOC_OTHER, or LOC_BLOCK
 */

PD_address *_PD_ptr_wr_lookup(PDBfile *file, void *vr,
			      PD_data_location *ploc, int write, int lck)
   {PD_data_location loc;
    PD_address *ad;
    adloc *al;

    if (lck == TRUE)
       SC_LOCKON(PD_ptr_lock);

    loc = LOC_OTHER;

    al = _PD_ptr_get_al(file, FALSE);

/* search the pointer list to see if this pointer is known */
    ad = _PD_ptr_find_ptr(al, vr, FALSE);

    if (file->track_pointers == FALSE)
       {loc = LOC_HERE;

        if ((ad == NULL) || (write == FALSE))
	   ad = _PD_ptr_install_ptr(al, vr, write, FALSE);

        else
            {ad = _PD_ptr_find_next(al, ad, vr);
	     ad->written = write;};}                 

/* add a new pointer */
    else if (ad == NULL)
       {loc = LOC_HERE;
	ad  = _PD_ptr_install_ptr(al, vr, write, FALSE);}

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

void _PD_ptr_wr_syment(PDBfile *file, char *name,
		       PD_address *ad, char *type,
		       inti ni, int64_t addr)
   {char pname[MAXLINE];
    syment *ep;

    if ((ni > 0) && (ad != NULL))
       {snprintf(pname, MAXLINE, "%s%ld#%s",
		 file->ptr_base, (long) ad->indx, name);

	ep = _PD_mk_syment(type, ni, addr, NULL, NULL);
	_PD_e_install(file, pname, ep, TRUE);

	if (file->format_version > 2)
	   {ad->addr  = addr;
	    ad->entry = ep;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_WR_ITAGS - handle the memory of pointers and write the itags
 *                  - correctly
 */

int _PD_ptr_wr_itags(PDBfile *file, char *name, void *vr, inti ni, char *type)
   {int rv;
    long n;
    int64_t addr;
    PD_data_location loc;
    PD_address *ad;
    adloc *al;

/* save the address of the itag because
 * we don't know the data address yet
 */
    addr = _PD_get_current_address(file, PD_WRITE);
    loc  = LOC_HERE;
    ad   = NULL;

    SC_LOCKON(PD_ptr_lock);

    al = _PD_ptr_get_al(file, FALSE);

    if ((_PD_IS_SEQUENTIAL) || (file->use_itags == FALSE))

/* remove addr from lists of pointers and disk addresses previously read */
       {if (addr != 0)
	   {ad = _PD_ptr_find_addr(al, addr, FALSE);
	    if (ad != NULL)
	       _PD_ptr_remove_addr(al, ad, FALSE);};

	ad = _PD_ptr_wr_lookup(file, vr, &loc, TRUE, FALSE);
	if (ad != NULL)
	   {if (loc == LOC_HERE)
	       ad->addr = addr;
	    else
	       addr = ad->addr;};}

    else
       {n  = _PD_ptr_get_n_spaces(file, TRUE);
	ad = _PD_ptr_get_ad(al, n);};

/* write the itag to the file */
    (*file->wr_itag)(file, name, ad, ni, type, addr, loc);

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
    adloc *al;

    al = _PD_ptr_get_al(file, TRUE);
    n  = _PD_ptr_index(p);
    ep = _PD_ptr_get_entry(al, n);

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

static int _PD_ptr_reset_ad(PD_address *ad, void *a)
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
    adloc *al;

    al = _PD_ptr_get_al(file, TRUE);

    rv = TRUE;
    if ((_PD_IS_SEQUENTIAL) || (file->use_itags == FALSE))
       rv = _PD_ptr_foreach(al, _PD_ptr_reset_ad, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_RESET - reset the pointer list element poining to VR */

int _PD_ptr_reset(PDBfile *file, char *vr)
   {int rv;
    PD_address *ad;
    adloc *al;

    al = _PD_ptr_get_al(file, TRUE);

    ad = _PD_ptr_find_ptr(al, vr, TRUE);
    rv = _PD_ptr_reset_ad(ad, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PTR_REGISTER_ENTRY - add a symbol table entry for a pointee to
 *                        - the read pointer list
 */

int _PD_ptr_register_entry(PDBfile *file, char *name, syment *ep)
   {int ok;
    long i, nc;
    adloc *al;

    ok = 0;

    if ((file != NULL) && (ep != NULL))
       {al = _PD_ptr_get_al(file, TRUE);
        nc = strlen(file->ptr_base);
	if (strncmp(name, file->ptr_base, nc) == 0) 
	   {i = SC_stol(name+nc);
            _PD_ptr_install_entry(al, i, ep, TRUE);};};

    return(ok);}

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
    adloc *al;

    file = (PDBfile *) a;
    if (file != NULL)
       {al = _PD_ptr_get_al(file, TRUE);

	ok = SC_haelem_data(hp, &name, NULL, (void **) &ep, FALSE);
	SC_ASSERT(ok == TRUE);

	if ((file != NULL) && (ep != NULL))
	   {nc = strlen(file->ptr_base);
	    if (strncmp(name, file->ptr_base, nc) == 0) 
	       {i = SC_stol(name+nc);
		_PD_ptr_install_entry(al, i, ep, TRUE);};};};

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

