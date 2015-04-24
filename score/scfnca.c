/*
 * SCFNCA.C - some more core routines used by many packages
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"

#include "scfnca.h"

#ifdef IBMMP

# undef STDOUT
# include <pm_util.h>

int
 SC_ibm_non_ANSI_compliant = FALSE;

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TO_NUMBER - in a thread safe way convert an address to
 *               - a number via a union
 */

int64_t _SC_to_number(void *a)
   {int64_t rv;
    SC_address ad;

/* in case diskaddr is larger than memaddr */
    ad.diskaddr = 0;
    ad.memaddr  = (char *) a;

    rv = ad.mdiskaddr;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TO_ADDRESS - in a thread safe way convert a number to
 *                - an address via a union
 */

void *_SC_to_address(int64_t a)
   {void *rv;
    SC_address ad;

/* in case diskaddr is larger than mdiskaddr */
    ad.diskaddr  = 0;
    ad.mdiskaddr = a;

    rv = ad.memaddr;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SET_FUNC_ADDR - convert a function pointer to a number via a union */

SC_address _SC_set_func_addr(PFInt x)
   {SC_address ad;

/* in case diskaddr is larger than funcaddr */
    ad.diskaddr = 0;
    ad.funcaddr = x;

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SET_MEM_ADDR - convert a pointer to a number via a union */

SC_address _SC_set_mem_addr(void *x)
   {SC_address ad;

/* in case diskaddr is larger than memaddr */
    ad.diskaddr = 0;
    ad.memaddr  = (char *) x;

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SET_DISK_ADDR - convert a disk address to a number via a union */

SC_address _SC_set_disk_addr(int64_t x)
   {SC_address ad;

/* in case memaddr is larger than diskaddr */
    ad.memaddr  = 0;
    ad.diskaddr = x;

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_HASHARR_LOOKUP_FUNCTION - in a thread safe way lookup a function pointer
 *                             - in a hash table via a union
 */

PFInt _SC_hasharr_lookup_function(hasharr *tab, char *nm)
   {SC_address ad;

    ad.memaddr = SC_hasharr_def_lookup(tab, nm);

    return(ad.funcaddr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ASSOC_FUNCTION - in a thread safe way lookup a function pointer
 *                    - in an association list via a union
 */

PFInt _SC_assoc_function(pcons *alist, char *nm)
   {SC_address ad;

    ad.memaddr = SC_assoc(alist, nm);

    return(ad.funcaddr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TYPE_CONTAINER - simple place holder for a potentially complicated 
 *                   - function
 */

void SC_type_container(char *dtype, char *stype)
   {

    strcpy(dtype, stype);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FIX_LMT - return the signed minimum and maximum values as well as
 *            - the unsigned maximum value for an integer type
 *            - of NB bytes
 *            - return TRUE iff NB is smaller than sizeof(int64_t)
 */

int SC_fix_lmt(int nb, int64_t *pmn, int64_t *pmx, int64_t *pumx)
   {int i, rv;
    int64_t imn, imx, uimx;
    static int nbi = sizeof(int64_t);

    rv = FALSE;

    if (nb <= nbi)

/* do the min limits */
       {for (imn = 0x80, i = 1; i < nb; i++)
	    imn <<= 8;

/* do the max limits */
	for (imx = 0x7f, i = 1; i < nb; i++)
	    imx = (imx << 8) + 0xff;

/* do the unsigned max limits */
	for (uimx = 0xff, i = 1; i < nb; i++)
	    uimx = (uimx << 8) + 0xff;

	if (nb < nbi)
	   imn = -imn;

	if (pmn != NULL)
	   *pmn  = imn;
	if (pmx != NULL)
	   *pmx  = imx;
	if (pumx != NULL)
	   *pumx = uimx;

	rv = TRUE;}

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_EXTRACT_FIELD - make a copy of the bit field specified by the starting
 *                  - bit, OFFS, and the number of bits, NBI, from the byte
 *                  - array pointed to by IN
 *                  - indexing off IN is 0 based
 *                  - the copy is to be put in a long and returned
 *                  - this imposes a 32 bit limit (minimum) so repeated calls
 *                  - must be made for longer fields
 *                  - ORD is an array NBY long which specifies the byte order
 *                  - of the original units in the array IN
 *                  - if NULL the order is taken as NORMAL order in the
 *                  - sense defined in PDB and NBY is ignored
 */

long SC_extract_field(char *in, int offs, int nbi, int nby, int *ord)
   {long n, bitf;
    int offy, tgt, ind;
    unsigned char mask, bpb;

    bitf = 0L;

/* move past the apropriate number of bytes so that the start bit is
 * in the first byte
 * OFFY is the offset of the byte containing the bit OFFS
 */
    n    = offs >> 3;
    offy = n % nby;
    n   -= offy;
    offs = offs % 8;

/* advance the pointer past the unneeded items */
    in += n;

    bpb = 8 - offs;
    if (ord == NULL)
       ind = offy++;
    else
       {if (offy >= nby)
           {offy -= nby;
	    in += nby;};
	ind = (ord[offy++] - 1);};

    tgt  = in[ind];
    mask = (1 << bpb) - 1;
    bitf = ((bitf << bpb) | (tgt & mask));
    nbi -= bpb;
    if (nbi < 0)
       bitf = bitf >> (-nbi);
    else
       {for (; nbi > 0; nbi -= bpb)
            {if (ord == NULL)
	        ind = offy++;
	     else
	        {if (offy >= nby)
		    {offy -= nby;
		     in += nby;};
		 ind = (ord[offy++] - 1);};
		 
             tgt  = in[ind];
             bpb  = min(nbi, 8);
             mask = (1 << bpb) - 1;
             bitf = ((bitf << bpb) |
		     ((tgt >> (8 - bpb)) & mask));};};

    return(bitf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_UNPACK_BITS - unpack an array that contains a bitstream
 *                - arguments are:
 *                -   ITYP    the target type of the data when unpacked
 *                -   NBITS   the number of bits per item
 *                -   PADSZ   the number of bits of pad preceding the fields
 *                -   FPP     the number of fields per pad
 *                -   NI      the number of items expected
 *                -   OFFS    the bit offset of the first pad
 */

int SC_unpack_bits(char *out, char *in, int ityp, int nbits,
		   int padsz, int fpp, long ni, long offs)
   {long i, bita, fld, np;

    for (i = 0L; i < ni; i++)
        {np   = 1 + i/fpp;
	 bita = np*padsz + i*nbits + offs;
	 fld  = SC_extract_field(in, bita, nbits, INT_MAX, NULL);

	 if (SC_is_type_fix(ityp) == TRUE)
	    {if (SC_unpack_bits_fnc[ityp] != NULL)
	        SC_unpack_bits_fnc[ityp](out, i, fld);};};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SIZEOF - a string driven sizeof operator if you can't do any better */

int SC_sizeof(const char *s)
   {int bpi;

    bpi = SC_type_size_a(s);

    if (bpi == -1)
       bpi = 0;

    return(bpi);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_UMASK - return the current umask */

mode_t SC_get_umask(void)
   {mode_t o, n;

/* setting umask returns the old value */
    n = 0;
    o = umask(n);

/* restore the original value */
    umask(o);

    return(o);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_PERM - return the complement of the current umask */

mode_t SC_get_perm(int exe)
   {mode_t n, p;

/* setting umask returns the old value */
    n = SC_get_umask();

    if (exe == TRUE)
       p = n ^ 0777;
    else
       p = n ^ 0666;

    return(p);}
 
/*--------------------------------------------------------------------------*/

/*                          POINTER CHECK ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* _SC_SEGV_HANDLER - helper for SC_POINTER_OK */

static void _SC_segv_handler(int signo)
   {JMP_BUF *cpu;

    cpu = SC_GET_CONTEXT(_SC_segv_handler);

    LONGJMP(*cpu, 1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_POINTER_OK - return TRUE iff
 *               - accessing the given pointer
 *               - will not cause a segmentation violation
 */

int SC_pointer_ok(void *p)
   {int ok, sz;
    char *s;
    volatile char c;
    JMP_BUF cpu;
    SC_contextdes osegv, obus, oill;

    sz = sizeof(JMP_BUF);

    if (p != NULL)
       {osegv = SC_signal_n(SIGSEGV, _SC_segv_handler, &cpu, sz);
#ifdef SIGBUS
        obus  = SC_signal_n(SIGBUS, _SC_segv_handler, &cpu, sz);
#endif
        oill  = SC_signal_n(SIGILL, _SC_segv_handler, &cpu, sz);

        if (SETJMP(cpu) == 0)
           {s = (char *) p;
            c = *s++;
	    SC_ASSERT(c == s[-1]);

            ok = TRUE;}
        else
            ok = FALSE;

	SC_restore_signal_n(SIGSEGV, osegv);
#ifdef SIGBUS
	SC_restore_signal_n(SIGBUS, obus);
#endif
	SC_restore_signal_n(SIGILL, oill);}
 
    else
       ok = TRUE; 

    return(ok);}
 
/*--------------------------------------------------------------------------*/

/*                                MISCELLANEOUS                             */

/*--------------------------------------------------------------------------*/

/* SC_PUTENV - wrapper for putenv because systems such as AIX
 *           - do not do putenv rationally
 *           - so we give putenv non-volatile memory because
 *           - it might not be smart enought to do its own copy
 */

int SC_putenv(char *s)
   {int err;
    char *t;

    t   = CSTRDUP(s, 3);
    err = putenv(t);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

