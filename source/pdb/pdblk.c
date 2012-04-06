/*
 * PDBLK.C - block management routines for PDBlib
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

typedef struct s_symblock symblock;

struct s_symblock
   {long number;
    int64_t diskaddr;
    PD_block_type valid;
    unsigned char checksum[PD_CKSUM_LEN];};

#define PD_DEFINE_SYMBLOCK(_f)            \
   PD_defstr(_f, "symblock",              \
             "long number",               \
             "long_long diskaddr",        \
	     "int valid",                 \
             "u_char checksum[16]",       \
             LAST)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRBL - diagnostic print of block list */

void dprbl(SC_array *bl)
   {long i, nb;
    symblock *sp;

    nb = SC_array_get_n(bl);
    if (nb > 0)
       {PRINT(stdout, "      #   Nitems  Address  Valid  Checksum\n");
	for (i = 0; i < nb; i++)
	    {sp = SC_array_get(bl, i);

	     PRINT(stdout, "  %5d %8ld %8ld     %2d  %s\n",
		   i+1, sp->number, (long) sp->diskaddr, sp->valid,
		   sp->checksum);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SYMBLOCK_INIT - initialize symblock SP */

static void _PD_symblock_init(void *a)
   {symblock *sp;

    sp = (symblock *) a;

    sp->diskaddr = -1;
    sp->number   = 0;
    sp->valid    = PD_BLOCK_UNINIT;

    memset(sp->checksum, 0, PD_CKSUM_LEN);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_MAKE - allocate and initialize a block list */

SC_array *_PD_block_make(long n)
   {SC_array *bl;

    bl = CMAKE_ARRAY(symblock, _PD_symblock_init, 0);

    SC_array_resize(bl, n, -1.0);

    return(bl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_FREE - release the block list */

void _PD_block_free(SC_array *bl)
   {

    if (bl != NULL)
       {SC_free_array(bl, NULL);
        bl = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_INIT - initialize a block list for EP */

SC_array *_PD_block_init(syment *ep, int fl)
   {SC_array *bl;

    bl = ep->blocks;
    if ((bl == NULL) || (fl == TRUE))
       {bl = CMAKE_ARRAY(symblock, _PD_symblock_init, 0);
	ep->blocks = bl;
        SC_mark(bl, 1);}
    else
       CINIT_ARRAY(bl, symblock, _PD_symblock_init, 0);

    return(bl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_REL - release the block list of EP */

void _PD_block_rel(syment *ep)
   {

    if (ep->blocks != NULL)
       {SC_free_array(ep->blocks, NULL);
	ep->blocks = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_CHECK - make sure the block list is at least N long */

static void _PD_block_check(SC_array *bl, long n)
   {long nb;

    nb = SC_array_get_n(bl);
    if (nb <= n)
       SC_array_set_n(bl, n);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_GET_ADDRESS - return the address of the Nth block of EP */

int64_t _PD_block_get_address(SC_array *bl, inti n)
   {inti nb;
    int64_t addr;
    symblock *sp;

    nb = SC_array_get_n(bl);
    if (n < nb)
       {sp   = SC_array_get(bl, n);
	addr = sp->diskaddr;}
    else
       addr = -1;

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_SET_ADDRESS - set the address of the Nth block of EP */

int64_t _PD_block_set_address(SC_array *bl, inti n, int64_t addr)
   {symblock *sp;

    _PD_block_check(bl, n+1);

    sp           = SC_array_get(bl, n);
    sp->diskaddr = addr;

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_GET_NUMBER - return the number of items in the Nth block of EP */

inti _PD_block_get_number(SC_array *bl, inti n)
   {inti ni, nb;
    symblock *sp;

    nb = SC_array_get_n(bl);
    if (n < nb)
       {sp = SC_array_get(bl, n);
	ni = sp->number;}
    else
       ni = -1;

    return(ni);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_SET_NUMBER - set the number of items in the Nth block of EP */

inti _PD_block_set_number(SC_array *bl, inti n, inti ni)
   {symblock *sp;

    _PD_block_check(bl, n+1);

    sp         = SC_array_get(bl, n);
    sp->number = ni;

    return(ni);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_GET_DESC - return the address and number of items
 *                    - in the Nth block of BL
 */

void _PD_block_get_desc(int64_t *paddr, inti *pni, SC_array *bl, inti n)
   {inti ni, nb;
    int64_t addr;
    symblock *sp;

    nb = SC_array_get_n(bl);

    if (n < nb)
       {sp   = SC_array_get(bl, n);
	addr = sp->diskaddr;
	ni   = sp->number;}
    else
       {addr = -1;
        ni   = -1;};

    if (paddr != NULL)
       *paddr = addr;

    if (pni != NULL)
       *pni = ni;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_SET_DESC - set the address and number of items
 *                    - in the Nth block of BL
 */

void _PD_block_set_desc(int64_t addr, inti ni, SC_array *bl, inti n)
   {symblock *sp;

    _PD_block_check(bl, n+1);

    sp = SC_array_get(bl, n);

    sp->diskaddr = addr;
    sp->number   = ni;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_GET_VALID - return the valid of items in the Nth block of EP */

PD_block_type _PD_block_get_valid(SC_array *bl, inti n)
   {inti nb;
    PD_block_type vl;
    symblock *sp;

    nb = SC_array_get_n(bl);
    if (n < nb)
       {sp = SC_array_get(bl, n);
	vl = sp->valid;}
    else
       vl = PD_BLOCK_UNINIT;

    return(vl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_SET_VALID - set the valid of items in the Nth block of EP */

int _PD_block_set_valid(SC_array *bl, inti n, PD_block_type vl)
   {symblock *sp;

    _PD_block_check(bl, n+1);

    sp = SC_array_get(bl, n);
    sp->valid = vl;

    return(vl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_N_BLOCKS - return the number of blocks for the entry EP */

inti _PD_n_blocks(syment *ep)
   {inti n;

    n = SC_array_get_n(ep->blocks);

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_GET_CKSUM - return the checksum of the Nth block of BL in DIG
 *                    - return TRUE iff successful
 */

int _PD_block_get_cksum(SC_array *bl, inti n, unsigned char *dig)
   {int rv;
    symblock *sp;

    sp = SC_array_get(bl, n);

    if ((dig != NULL) &&
	((sp->valid == PD_BLOCK_VALID) ||
	 (sp->valid == PD_BLOCK_UNVERIFIED)))
       {memcpy(dig, sp->checksum, PD_CKSUM_LEN);
	rv = TRUE;}
    else
       rv = FALSE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_SET_CKSUM - set the checksum of the Nth block of BL in DIG
 *                     - return TRUE iff successful
 */

int _PD_block_set_cksum(SC_array *bl, inti n, unsigned char *dig)
   {int rv;
    symblock *sp;

    sp = SC_array_get(bl, n);

    if (dig != NULL)
       {memcpy(sp->checksum, dig, PD_CKSUM_LEN);
	sp->valid = PD_BLOCK_VALID;
	rv        = TRUE;}
    else
       {sp->valid = PD_BLOCK_INVALID;
	rv        = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CONSISTENT_DIMS - check two sets of dimensions for consistency of
 *                     - the non-changing sector and proper updating of
 *                     - the changing dimension
 */
   
static int _PD_consistent_dims(PDBfile *file, syment *ep, dimdes *ndims)
   {int rv;
    dimdes *odims, *od, *nd;

    if ((file == NULL) || (ep == NULL) || (ndims == NULL))
       rv = FALSE;

    else
       {rv = TRUE;

	od    = NULL;
	nd    = NULL;
	odims = PD_entry_dimensions(ep);

/* check the dimensions for consistency */
	if (file->major_order == COLUMN_MAJOR_ORDER)
	   {for (od = odims, nd = ndims;
		 (od != NULL) && (nd != NULL) && (nd->next != NULL);
		 od = od->next, nd = nd->next)
	        {if ((od->index_min != nd->index_min) ||
		     (od->index_max != nd->index_max) ||
		     (od->number != nd->number))
		    {rv = FALSE;
		     break;};};}

	else if (file->major_order == ROW_MAJOR_ORDER)
	   {for (od = odims->next, nd = ndims->next;
		 (od != NULL) && (nd != NULL);
		 od = od->next, nd = nd->next)
	        {if ((od->index_min != nd->index_min) ||
		     (od->index_max != nd->index_max) ||
		     (od->number != nd->number))
		    {rv = FALSE;
		     break;};};

	    nd = ndims;
	    od = odims;};

	if (rv == TRUE)
	   {if ((nd == NULL) || (od == NULL))
	       rv = FALSE;

	    else if (nd->index_min == file->default_offset)
	       od->index_max += nd->index_max - nd->index_min + 1;

	    else if (nd->index_min == od->index_max + 1L)
	       od->index_max = nd->index_max;

	    else
	       rv = FALSE;

	    if (rv == TRUE)
	       {od->number = od->index_max - od->index_min + 1L;
		ep->number = _PD_comp_num(odims);};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_ADD - add a new block to an entry
 *               - this does nothing to the file, only the syment
 *               - the file extension operations are left to file->tr->write
 *               - or to _PD_defent
 */

int _PD_block_add(PDBfile *file, syment *ep, dimdes *dims, int64_t addr)
   {int n, rv;
    inti ni, nb;
    intb bpi;
    SC_array *bl;

    if (file == NULL)
       PD_error("NULL FILE - _PD_BLOCK_ADD", PD_WRITE);

    if (!_PD_consistent_dims(file, ep, dims))
       PD_error("INCONSISTENT DIMENSION CHANGE - _PD_BLOCK_ADD", PD_WRITE);

    rv = FALSE;
    if ((file != NULL) && (ep != NULL))
       {bpi = _PD_lookup_size(PD_entry_type(ep), file->chart);
	ni  = _PD_comp_num(dims);
	nb  = ni*bpi;
	bl  = ep->blocks;
	n   = SC_array_get_n(bl);

	_PD_block_set_desc(addr, ni, bl, n);
	_PD_block_set_valid(bl, n, PD_BLOCK_INVALID);

/* we are through with the dimensions
 * their information has been moved into the entry dimensions
 */
	_PD_rl_dimensions(dims);

	_PD_SET_EOD(file, addr, nb);

	rv = _PD_EXTEND_FILE(file, nb);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_FIND - return the index of the block of EP which
 *                - contains the address ADDR
 */

inti _PD_block_find(PDBfile *file, syment *ep, int64_t addr)
   {inti i, n, start, stop;
    intb bpi;
    SC_array *bl;
    symblock *sp;

    i = -1;
    if ((file != NULL) && (ep != NULL))
       {bpi = _PD_lookup_size(PD_entry_type(ep), file->chart);

	bl  = ep->blocks;
	n   = SC_array_get_n(bl);

	for (i = 0; i < n; i++)
	    {sp    = SC_array_get(bl, i);
	     start = sp->diskaddr;
	     stop  = start + bpi*sp->number;
	     if ((start <= addr) && (addr < stop))
	        break;};

	if (i >= n)
	   i = -1;};

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_COPY_SEG - make a copy of part of the block list BL */

SC_array *_PD_block_copy_seg(SC_array *bl, long imn, long imx)
   {inti i, n;
    SC_array *nbl;
    symblock *sp;

    n   = imx - imn;
    nbl = _PD_block_make(n);

    for (i = 0; i < n; i++)
        {sp = SC_array_get(bl, i + imn);
         SC_array_set(nbl, i, sp);};

    return(nbl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_COPY - copy the block list of EPO into EPN */

void _PD_block_copy(syment *epn, syment *epo)
   {SC_array *spn, *spo;

    spo = epo->blocks;
    spn = SC_array_copy(spo);

    epn->blocks = spn;
    SC_mark(spn, 1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_SWITCH - replace the block list of EP with SP */

void _PD_block_switch(syment *ep, SC_array *bln)
   {inti nb;
    symblock *spn;
    SC_array *bl;
    
    nb  = SC_array_get_n(bln);
    spn = SC_array_array(bln);

    bln->array = NULL;
    _PD_block_free(bln);

    bl = ep->blocks;
    CFREE(bl->array);
    bl->array = spn;
    bl->n     = nb;
    bl->nx    = nb;

/* GOTCHA: the ref count may be correct because of its origin in bln */
/*    SC_mark(spn, 1); */

    CFREE(spn);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_TRUNCATE - truncate the blocks of EP to the specified size NI
 *                    - in the slowest varying dimension
 */

void _PD_block_truncate(syment *ep, inti ni)
   {inti i, n, nib;
    symblock *sp;

    if (ep != NULL)
       {ep->number = ni;

/* find and adjust the last block contained in the new size */
	n = _PD_n_blocks(ep);
	for (i = 0L; i < n; i++, ni -= nib)
	    {sp  = SC_array_get(ep->blocks, i);
	     nib = sp->number;
	     if (ni <= nib)
	        {sp->number    = ni;
		 ep->blocks->n = i + 1;
		 break;};};

/* zero out any remaining blocks */
	for (; i < n; i++)
	    {sp = SC_array_get(ep->blocks, i);
	     sp->number   = 0;
	     sp->diskaddr = 0;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_EFFECTIVE_ADDR - THREADSAFE
 *                    - given a disk address computed on the assumption
 *                    - of one contiguous block and a list of symblocks
 *                    - compute and return the actual disk address
 *                    - also return the number of items remaining in the
 *                    - block after the effective address
 */

inti _PD_effective_addr(int64_t *paddr, inti *pni,
			intb bpi, SC_array *bl)
   {inti i, nb, nt, addr, eaddr;
    symblock *sp;

    eaddr = *paddr;
    i     = 0;
    sp    = SC_array_get(bl, i);
    addr  = sp->diskaddr;
    nt    = eaddr - addr;
    while (TRUE)
        {nb  = sp->number*bpi;
         nt -= nb;

         if ((nb <= 0L) || (nt < 0L))
            break;

         i++;
	 sp    = SC_array_get(bl, i);
         addr  = sp->diskaddr;
         eaddr = addr + nt;};

    *paddr = eaddr;
    *pni   = (addr + nb - eaddr)/bpi;

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_DEFINE - define block relates types for FILE */

int _PD_block_define(PDBfile *file)
   {int rv;
    defstr *dp;

    dp = PD_DEFINE_SYMBLOCK(file);
    if (dp == NULL)
       {PD_error("COULDN'T DEFINE SYMBLOCK - _PD_BLOCK_DEFINE", PD_GENERIC);
        rv = FALSE;}
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_CKSUM_WRITE - write the per block checksums
 *                       - for the Checksums extra when closing the file
 */

int _PD_block_cksum_write(PDBfile *file, syment *ep, char *name)
   {int ok, st, ne;
    inti j, n;
    unsigned char dig[PD_CKSUM_LEN];
    symblock *sp;
    SC_array *bl;

    ok = TRUE;

    if (file->use_cksum & PD_MD5_RW)
       {bl = ep->blocks;
	n  = SC_array_get_n(bl);

/* count the initialized checksums */
	ne = 0;
	for (j = 0L; j < n; j++)
	    {sp  = SC_array_get(bl, j);
	     ne += (sp->valid != PD_BLOCK_UNINIT);};

	if (ne > 0)
	   {ok &= _PD_put_string(1, "%s %lld %ld\n   ",
				 name, (long long) n, ne);

/* write the initialized checksums */
	    for (j = 0L; j < n; j++)
	        {sp = SC_array_get(bl, j);
	         if (sp->valid != PD_BLOCK_UNINIT)
		    {if ((j != 0L) && ((j % 2) == 0))
		        _PD_put_string(1, "\n   ");

		     if (sp->valid == PD_BLOCK_INVALID)
		        _PD_cksum_block_write(file, ep, j);

		     st = _PD_block_get_cksum(bl, j, dig);
		     SC_ASSERT(st == TRUE);

		     ok &= _PD_put_string(1, " %lld %s",
					  (long long) j, dig);};};

	    ok &= _PD_put_string(1, "\n");};};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BLOCK_CKSUM_READ - read the per block checksums
 *                      - for the Checksums extra when opening the file
 */

int _PD_block_cksum_read(PDBfile *file)
   {int ok;
    long ie, ne, n, j, nb, bsz;
    unsigned char *dig;
    char *local, *name, *token, *s;
    syment *ep;
    SC_array *bl;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    local = pa->local;
    bsz   = sizeof(pa->local);

    ok = TRUE;

    while (_PD_get_token(NULL, local, bsz, '\n'))
       {if (*local == '\0')
	   break;

	name = SC_strtok(local, " \n", s);
	n    = SC_stol(SC_strtok(NULL, " \n", s));
	ne   = SC_stol(SC_strtok(NULL, " \n", s));

	SC_ASSERT(n > 0);

	ep = PD_inquire_entry(file, name, FALSE, NULL);
	if (ep != NULL)
	   {bl = ep->blocks;
	    nb = SC_array_get_n(bl);

	    for (ie = 0L; ie < ne; ie++)
	        {token = SC_strtok(NULL, " \n", s);
		 if (token == NULL)
		    {_PD_get_token(NULL, local, bsz, '\n');
		     token = SC_strtok(local, " \n", s);};
		     
		 j   = SC_stol(token);
		 dig = (unsigned char *) SC_strtok(NULL, " \n", s);

		 if (j < nb)
		    {_PD_block_set_cksum(bl, j, dig);
		     _PD_block_set_valid(bl, j, PD_BLOCK_UNVERIFIED);};};};};

    file->file_cksum |= PD_MD5_RW;

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

