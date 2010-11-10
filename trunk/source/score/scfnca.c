/*
 * SCFNCA.C - some more core routines used by many packages
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"

/*--------------------------------------------------------------------------*/

#define SC_TRANS_SPACE(_d, _dtyp, _pd, _n)                                   \
    _dtyp *_d;                                                               \
    _d = (_dtyp *) *_pd;                                                     \
    if (_d == NULL)                                                          \
       {_d   = FMAKE_N(_dtyp, _n, "SC_TRANS_SPACE:_d");                      \
        *_pd = (void *) _d;};                                                \

/*--------------------------------------------------------------------------*/

#define SC_TRANS_DATA(_dtyp, _pd, _styp, _s, _n)                             \
    {SC_TRANS_SPACE(_d, _dtyp, _pd, _n);                                     \
     if (_d != NULL)                                                         \
        {if (strcmp(_styp, SC_DOUBLE_S) == 0)                                \
            {SC_TRANS_TYPE(_d, double, _s, _n);}                             \
         else if (strcmp(_styp, SC_FLOAT_S) == 0)                            \
            {SC_TRANS_TYPE(_d, float, _s, _n);}                              \
	 else if ((strcmp(_styp, SC_INT_S) == 0) ||                          \
		  (strcmp(_styp, SC_INTEGER_S) == 0))                        \
            {SC_TRANS_TYPE(_d, int, _s, _n);}                                \
         else if (strcmp(_styp, SC_LONG_LONG_S) == 0)                        \
            {SC_TRANS_TYPE(_d, long long, _s, _n);}                          \
         else if (strcmp(_styp, SC_LONG_S) == 0)                             \
            {SC_TRANS_TYPE(_d, long, _s, _n);}                               \
         else if (strcmp(_styp, SC_SHORT_S) == 0)                            \
            {SC_TRANS_TYPE(_d, short, _s, _n);}                              \
         else if (strcmp(_styp, SC_CHAR_S) == 0)                             \
            {SC_TRANS_TYPE(_d, char, _s, _n);};                              \
     ret = TRUE;};}

/*--------------------------------------------------------------------------*/

#define SC_TRANS_TYPE(_d, _styp, _s, _n)                                     \
    {_styp *_ps;                                                             \
     _ps = (_styp *) _s;                                                     \
     for (i = 0; i < _n; i++, *_d++ = *_ps++);}

/*--------------------------------------------------------------------------*/

#define SC_TRANS_TYPE_L(_d, _styp, _s, _n, _mn, _mx)                         \
    {_styp *_ps;                                                             \
     _styp _sm, _smn, _smx;                                                  \
     _smn = (_styp) _mn;                                                     \
     _smx = (_styp) _mx;                                                     \
     _ps  = (_styp *) _s;                                                    \
     for (i = 0; i < _n; i++)                                                \
         {_sm = *_ps++;                                                      \
          _sm = min(_sm, _smx);                                              \
          _sm = max(_sm, _smn);                                              \
          *_d++ = _sm;};}

/*--------------------------------------------------------------------------*/

#ifdef IBMMP

# undef STDOUT
# include <pm_util.h>

int
 SC_ibm_non_ANSI_compliant = FALSE;

#endif

int
 (*SC_sizeof_hook)(char *s) = SC_sizeof;

int
 (*SC_convert_hook)(char *dtype, void **pd,
                    char *stype, void *s,
                    int n, int flag) = SC_convert;

void
 (*SC_type_container_hook)(char *dtype, char *stype) = SC_type_container;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TO_NUMBER - in a thread safe way convert an address to
 *               - a number via a union
 */

BIGINT _SC_to_number(void *a)
   {long long rv;
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

void *_SC_to_address(BIGINT a)
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

SC_address _SC_set_disk_addr(BIGINT x)
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

/* SC_DEREFERENCE - THREADSAFE
 *                - starting at the end of the string work backwards to
 *                - the first non-blank character and if it is a '*'
 *                - insert '\0' in its place
 *                - return a pointer to the beginning of the string
 */

char *SC_dereference(char *s)
   {char *t;

/* starting at the end of the string, working backwards */
    t = s + (strlen(s) - 1);

/* skip over any whitespace */
    while ((t >= s) && (*t == ' '))
       {t--;};

/* remove any terminating '*' char */
    if ((t >= s) && (*t == '*'))
       {*t = '\0';
        t--;};

/* remove any trailing whitespace -- e.g. "char *" -> "char " -> "char" */
    while ((t >= s) && (strchr(" \t", *t) != NULL))
       {*t = '\0';
        t--;};

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FIX_LMT - return the signed minimum and maximum values as well as
 *            - the unsigned maximum value for an integer type
 *            - of NB bytes
 *            - return TRUE iff NB is smaller than sizeof(long long)
 */

int SC_fix_lmt(int nb, BIGINT *pmn, BIGINT *pmx, BIGINT *pumx)
   {int i, rv;
    long long imn, imx, uimx;

    rv = FALSE;

    if (nb <= sizeof(long long))

/* do the min limits */
       {for (imn = 0x80, i = 1; i < nb; i++)
	    imn <<= 8;

/* do the max limits */
	for (imx = 0x7f, i = 1; i < nb; i++)
	    imx = (imx << 8) + 0xff;

/* do the unsigned max limits */
	for (uimx = 0xff, i = 1; i < nb; i++)
	    uimx = (uimx << 8) + 0xff;

	if (nb < sizeof(long long))
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

/* SC_CONVERT - convert data from one binary format to another
 *            - if destination pointer is NULL, space is allocated
 *            - if types are the same do nothing but return -1
 *            - arguments:
 *            -     dtype - type of destination data
 *            -     d     - pointer to converted data destination
 *            -     stype - type of source data
 *            -     s     - pointer to data to be converted
 *            -     n     - number of data items
 *            -     flag  - free source data flag
 */

int SC_convert(char *dtype, void **pd, char *stype, void *s,
	       int n, int flag)
   {int i, ret;

    if (_SC.bmx == 0)
       SC_fix_lmt(sizeof(long long), &_SC.bmn, &_SC.bmx, NULL);

    ret = FALSE;

#ifdef SC_FAST_TRUNC

/* fast but with simple truncation - FPE's are possible */

/* floating point types */
    if (strcmp(dtype, SC_FLOAT_S) == 0)
       {SC_TRANS_DATA(float, pd, stype, s, n);}

    else if (strcmp(dtype, SC_DOUBLE_S) == 0)
       {SC_TRANS_DATA(double, pd, stype, s, n);}

    else if (strcmp(dtype, SC_LONG_DOUBLE_S) == 0)
       {SC_TRANS_DATA(long double, pd, stype, s, n);}


/* fixed point types */
    else if (strcmp(dtype, SC_SHORT_S) == 0)
       {SC_TRANS_DATA(short, pd, stype, s, n);}

    else if ((strcmp(dtype, SC_INT_S) == 0) ||
	     (strcmp(dtype, SC_INTEGER_S) == 0))
       {SC_TRANS_DATA(int, pd, stype, s, n);}

    else if (strcmp(dtype, SC_LONG_S) == 0)
       {SC_TRANS_DATA(long, pd, stype, s, n);}

    else if (strcmp(dtype, SC_LONG_LONG_S) == 0)
       {SC_TRANS_DATA(long long, pd, stype, s, n);}

    else if (strcmp(dtype, SC_CHAR_S) == 0)
       {SC_TRANS_DATA(char, pd, stype, s, n);};

#else

/* slower with clipping - FPE's are NOT possible */

    if (strcmp(dtype, SC_FLOAT_S) == 0)
       {SC_TRANS_SPACE(_d, float, pd, n);
        if (_d != NULL)

/* floating point types */
	   {if (strcmp(stype, SC_FLOAT_S) == 0)
               {SC_TRANS_TYPE(_d, float, s, n);}
	    else if (strcmp(stype, SC_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, double, s, n, FLT_MIN, FLT_MAX);}
	    else if (strcmp(stype, SC_LONG_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, long double, s, n, FLT_MIN, FLT_MAX);}

/* fixed point types */
            else if (strcmp(stype, SC_SHORT_S) == 0)
               {SC_TRANS_TYPE(_d, short, s, n);}
	    else if ((strcmp(dtype, SC_INT_S) == 0) ||
		     (strcmp(dtype, SC_INTEGER_S) == 0))
               {SC_TRANS_TYPE(_d, int, s, n);}
            else if (strcmp(stype, SC_LONG_S) == 0)
               {SC_TRANS_TYPE(_d, long, s, n);}
            else if (strcmp(stype, SC_LONG_LONG_S) == 0)
               {SC_TRANS_TYPE(_d, long long, s, n);}

            else if (strcmp(stype, SC_CHAR_S) == 0)
               {SC_TRANS_TYPE(_d, char, s, n);};
            ret = TRUE;};}

    else if (strcmp(dtype, SC_DOUBLE_S) == 0)
       {SC_TRANS_DATA(double, pd, stype, s, n);}

    else if (strcmp(dtype, SC_LONG_DOUBLE_S) == 0)
       {SC_TRANS_SPACE(_d, long double, pd, n);
        if (_d != NULL)

/* floating point types */
	   {if (strcmp(stype, SC_FLOAT_S) == 0)
               {SC_TRANS_TYPE(_d, float, s, n);}
	    else if (strcmp(stype, SC_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, double, s, n, FLT_MIN, FLT_MAX);}
	    else if (strcmp(stype, SC_LONG_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, long double, s, n, FLT_MIN, FLT_MAX);}

/* fixed point types */
            else if (strcmp(stype, SC_SHORT_S) == 0)
               {SC_TRANS_TYPE(_d, short, s, n);}
	    else if ((strcmp(dtype, SC_INT_S) == 0) ||
		     (strcmp(dtype, SC_INTEGER_S) == 0))
               {SC_TRANS_TYPE(_d, int, s, n);}
            else if (strcmp(stype, SC_LONG_S) == 0)
               {SC_TRANS_TYPE(_d, long, s, n);}
            else if (strcmp(stype, SC_LONG_LONG_S) == 0)
               {SC_TRANS_TYPE(_d, long long, s, n);}

            else if (strcmp(stype, SC_CHAR_S) == 0)
               {SC_TRANS_TYPE(_d, char, s, n);};
            ret = TRUE;};}

    else if (strcmp(dtype, SC_SHORT_S) == 0)
       {SC_TRANS_SPACE(_d, short, pd, n);
        if (_d != NULL)

/* floating point types */
	   {if (strcmp(stype, SC_FLOAT_S) == 0)
               {SC_TRANS_TYPE_L(_d, float, s, n, SHRT_MIN, SHRT_MAX);}
            else if (strcmp(stype, SC_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, double, s, n, SHRT_MIN, SHRT_MAX);}
            else if (strcmp(stype, SC_LONG_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, long double, s, n, SHRT_MIN, SHRT_MAX);}

/* fixed point types */
            else if (strcmp(stype, SC_SHORT_S) == 0)
               {SC_TRANS_TYPE(_d, short, s, n);}
	    else if ((strcmp(dtype, SC_INT_S) == 0) ||
		     (strcmp(dtype, SC_INTEGER_S) == 0))
               {SC_TRANS_TYPE_L(_d, int, s, n, SHRT_MIN, SHRT_MAX);}
            else if (strcmp(stype, SC_LONG_S) == 0)
               {SC_TRANS_TYPE_L(_d, long, s, n, SHRT_MIN, SHRT_MAX);}
            else if (strcmp(stype, SC_LONG_LONG_S) == 0)
               {SC_TRANS_TYPE_L(_d, long long, s, n, SHRT_MIN, SHRT_MAX);}

            else if (strcmp(stype, SC_CHAR_S) == 0)
               {SC_TRANS_TYPE(_d, char, s, n);};
        ret = TRUE;};}

    else if ((strcmp(dtype, SC_INT_S) == 0) ||
	     (strcmp(dtype, SC_INTEGER_S) == 0))
       {SC_TRANS_SPACE(_d, int, pd, n);
        if (_d != NULL)

/* floating point types */
	   {if (strcmp(stype, SC_FLOAT_S) == 0)
               {SC_TRANS_TYPE_L(_d, float, s, n, INT_MIN, INT_MAX);}
	    else if (strcmp(stype, SC_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, double, s, n, INT_MIN, INT_MAX);}
	    else if (strcmp(stype, SC_LONG_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, long double, s, n, INT_MIN, INT_MAX);}

/* fixed point types */
            else if (strcmp(stype, SC_SHORT_S) == 0)
               {SC_TRANS_TYPE(_d, short, s, n);}
	    else if ((strcmp(dtype, SC_INT_S) == 0) ||
		     (strcmp(dtype, SC_INTEGER_S) == 0))
               {SC_TRANS_TYPE(_d, int, s, n);}
            else if (strcmp(stype, SC_LONG_S) == 0)
               {SC_TRANS_TYPE_L(_d, long, s, n, INT_MIN, INT_MAX);}
            else if (strcmp(stype, SC_LONG_LONG_S) == 0)
               {SC_TRANS_TYPE_L(_d, long long, s, n, INT_MIN, INT_MAX);}

            else if (strcmp(stype, SC_CHAR_S) == 0)
               {SC_TRANS_TYPE(_d, char, s, n);};
        ret = TRUE;};}

    else if (strcmp(dtype, SC_LONG_S) == 0)
       {SC_TRANS_SPACE(_d, long, pd, n);
        if (_d != NULL)

/* floating point types */
	   {if (strcmp(stype, SC_FLOAT_S) == 0)
               {SC_TRANS_TYPE_L(_d, float, s, n, LONG_MIN, LONG_MAX);}
            else if (strcmp(stype, SC_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, double, s, n, LONG_MIN, LONG_MAX);}
            else if (strcmp(stype, SC_LONG_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, long double, s, n, LONG_MIN, LONG_MAX);}

/* fixed point types */
            else if (strcmp(stype, SC_SHORT_S) == 0)
               {SC_TRANS_TYPE(_d, short, s, n);}
	    else if ((strcmp(dtype, SC_INT_S) == 0) ||
		     (strcmp(dtype, SC_INTEGER_S) == 0))
               {SC_TRANS_TYPE(_d, int, s, n);}
            else if (strcmp(stype, SC_LONG_S) == 0)
               {SC_TRANS_TYPE(_d, long, s, n);}
            else if (strcmp(stype, SC_LONG_LONG_S) == 0)
               {SC_TRANS_TYPE_L(_d, long long, s, n, LONG_MIN, LONG_MAX);}

            else if (strcmp(stype, SC_CHAR_S) == 0)
               {SC_TRANS_TYPE(_d, char, s, n);};
        ret = TRUE;};}

    else if (strcmp(dtype, SC_LONG_LONG_S) == 0)
       {SC_TRANS_SPACE(_d, long long, pd, n);
        if (_d != NULL)

/* floating point types */
	   {if (strcmp(stype, SC_FLOAT_S) == 0)
               {SC_TRANS_TYPE(_d, float, s, n);}
	    else if (strcmp(stype, SC_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, double, s, n, _SC.bmn, _SC.bmx);}
	    else if (strcmp(stype, SC_LONG_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, long double, s, n, _SC.bmn, _SC.bmx);}

/* fixed point types */
            else if (strcmp(stype, SC_SHORT_S) == 0)
               {SC_TRANS_TYPE(_d, short, s, n);}
	    else if ((strcmp(dtype, SC_INT_S) == 0) ||
		     (strcmp(dtype, SC_INTEGER_S) == 0))
               {SC_TRANS_TYPE(_d, int, s, n);}
            else if (strcmp(stype, SC_LONG_S) == 0)
               {SC_TRANS_TYPE(_d, long, s, n);}
            else if (strcmp(stype, SC_LONG_LONG_S) == 0)
               {SC_TRANS_TYPE(_d, long long, s, n);}

            else if (strcmp(stype, SC_CHAR_S) == 0)
               {SC_TRANS_TYPE(_d, char, s, n);};
        ret = TRUE;};}

    else if (strcmp(dtype, SC_CHAR_S) == 0)
       {SC_TRANS_SPACE(_d, char, pd, n);
        if (_d != NULL)

/* floating point types */
	   {if (strcmp(stype, SC_FLOAT_S) == 0)
               {SC_TRANS_TYPE_L(_d, float, s, n, CHAR_MIN, CHAR_MAX);}
	    else if (strcmp(stype, SC_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, double, s, n, CHAR_MIN, CHAR_MAX);}
	    else if (strcmp(stype, SC_LONG_DOUBLE_S) == 0)
               {SC_TRANS_TYPE_L(_d, long double, s, n, CHAR_MIN, CHAR_MAX);}

/* fixed point types */
            else if (strcmp(stype, SC_SHORT_S) == 0)
               {SC_TRANS_TYPE_L(_d, short, s, n, CHAR_MIN, CHAR_MAX);}
	    else if ((strcmp(dtype, SC_INT_S) == 0) ||
		     (strcmp(dtype, SC_INTEGER_S) == 0))
               {SC_TRANS_TYPE_L(_d, int, s, n, CHAR_MIN, CHAR_MAX);}
            else if (strcmp(stype, SC_LONG_S) == 0)
               {SC_TRANS_TYPE_L(_d, long, s, n, CHAR_MIN, CHAR_MAX);}
            else if (strcmp(stype, SC_LONG_LONG_S) == 0)
               {SC_TRANS_TYPE_L(_d, long long, s, n, CHAR_MIN, CHAR_MAX);}

            else if (strcmp(stype, SC_CHAR_S) == 0)
               {SC_TRANS_TYPE(_d, char, s, n);};

        ret = TRUE;};}

#endif

    if (flag && (ret == TRUE))
       SFREE(s);

    return(ret);}

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
   {long n, bit_field;
    int offy, tgt, ind;
    unsigned char mask, bpb;

    bit_field = 0L;

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

    bpb  = 8 - offs;
    if (ord == NULL)
       ind = offy++;
    else
       {if (offy >= nby)
           {offy -= nby;
	    in += nby;};
	ind = (ord[offy++] - 1);};

    tgt  = in[ind];
    mask = (1 << bpb) - 1;
    bit_field = ((bit_field << bpb) | (tgt & mask));
    nbi -= bpb;
    if (nbi < 0)
       bit_field = bit_field >> (-nbi);
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
             bit_field = ((bit_field << bpb) |
                          ((tgt >> (8 - bpb)) & mask));};};

    return(bit_field);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_UNPACK_BITS - unpack an array that contains a bitstream
 *                - arguments are:
 *                -   ITYP    the target type of the data when unpacked
 *                -   NBITS   the number of bits per item
 *                -   PADSZ   the number of bits of pad preceding the fields
 *                -   FPP     the number of fields per pad
 *                -   NITEMS  the number of items expected
 *                -   OFFS    the bit offset of the first pad
 */

int SC_unpack_bits(char *out, char *in, int ityp, int nbits,
		   int padsz, int fpp, long nitems, long offs)
   {long i, bita, fld, np, *pl;
    char *pc;
    int *pi;
    short *ps;

    for (i = 0L; i < nitems; i++)
        {np   = 1 + i/fpp;
	 bita = np*padsz + i*nbits + offs;
	 fld  = SC_extract_field(in, bita, nbits, INT_MAX, NULL);

	 if (ityp == SC_CHAR_I)
	    {pc = (char *) out;
	     pc[i] = (char) fld;}

	 else if (ityp == SC_SHORT_I)
	    {ps    = (short *) out;
	     ps[i] = (short) fld;}

	 else if (ityp == SC_INT_I)
	    {pi    = (int *) out;
	     pi[i] = (int) fld;}

	 else if (ityp == SC_LONG_I)
	    {pl    = (long *) out;
	     pl[i] = (long) fld;};};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SIZEOF - a string driven sizeof operator if you can't do any better */

int SC_sizeof(char *s)
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
    n = umask(o);

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
   {

    LONGJMP(_SC.ok_ptr, 1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_POINTER_OK - return TRUE iff
 *               - accessing the given pointer
 *               - will not cause a segmentation violation
 */

int SC_pointer_ok(void *p)
   {int ok;
    char *s;
    volatile char c;
    PFSignal_handler osegv, obus, oill;

    if (p != NULL)
       {osegv = SC_signal(SIGSEGV, _SC_segv_handler);
#ifdef SIGBUS
        obus  = SC_signal(SIGBUS, _SC_segv_handler);
#endif
        oill  = SC_signal(SIGILL, _SC_segv_handler);

        if (SETJMP(_SC.ok_ptr) == 0)
           {s  = (char *) p;
            c  = *s++;
            ok = TRUE;}
        else
            ok = FALSE;

        SC_signal(SIGSEGV, osegv);
#ifdef SIGBUS
        SC_signal(SIGBUS, obus);
#endif
        SC_signal(SIGILL, oill);}
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

    t   = SC_strsavef(s, "PERM|char*:SC_PUTENV:s");
    err = putenv(t);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

