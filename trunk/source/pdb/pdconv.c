/*
 * PDCONV.C - routines to do conversions between data formats
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"
#include <pdform.h>

enum e_PD_conv_type
   {SHORT,
    INTEGER,
    LONG,
    FLOAT,
    DOUBLE,
    CHAR,
    POINTER,
    NTYPES};

typedef enum e_PD_conv_type PD_conv_type;

#define MBLOCKS 256
#define MBYTES   16

#define PD_NEXT_LOC(_a, _inc)                                                \
   {*(_a)       += (_inc);                                                   \
    _a ## _offs += (_inc);}

#define ONES_COMP_NEG(n, nb, incr)                                           \
    {if (nb == 8*sizeof(long))                                               \
        n = ~n + incr;                                                       \
     else                                                                    \
        {long msk;                                                           \
         msk = (1L << nb) - 1L;                                              \
         n   = (~n + incr) & msk;};}

int
 REVERSE_IN,
 REVERSE_OUT;

/*--------------------------------------------------------------------------*/

/*                             DATA_STANDARDS                               */

/*--------------------------------------------------------------------------*/

/*
 * Floating Point Format Descriptor
 *
 * -   format[0] = # of bits per number
 * -   format[1] = # of bits in exponent
 * -   format[2] = # of bits in mantissa
 * -   format[3] = start bit of sign
 * -   format[4] = start bit of exponent
 * -   format[5] = start bit of mantissa
 * -   format[6] = high order mantissa bit
 * -   format[7] = bias of exponent
 *
 */

/* NOTE: consider ASCII to be binary character data along with
 *       all the LOCALE variants, wide chars, etc.
 *       TEXT is used to refer to non-binary character data
 */

int
 FORMAT_FIELDS = 8,
 text_order[]         = {-1},

 float2_nrm_order[]   = {1, 2},
 float4_nrm_order[]   = {1, 2, 3, 4},
 float8_nrm_order[]   = {1, 2, 3, 4, 5, 6, 7, 8},
 float10_nrm_order[]  = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
 float12_nrm_order[]  = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
 float16_nrm_order[]  = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16},

 float2_rev_order[]   = {2, 1},
 float4_rev_order[]   = {4, 3, 2, 1},
 float8_rev_order[]   = {8, 7, 6, 5, 4, 3, 2, 1},
 float10_rev_order[]  = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
 float12_rev_order[]  = {12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
 float16_rev_order[]  = {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},

 float4_vax_order[]   = {2, 1, 4, 3},
 float8_vax_order[]   = {2, 1, 4, 3, 6, 5, 8, 7};

long
 float4_text[]    = {15L,   2L,   8L, 0L, 0L,  0L, 0L,    0x0L},
 float8_text[]    = {26L,   3L,  16L, 0L, 0L,  0L, 0L,    0x0L},
 float16_text[]   = {44L,   3L,  16L, 0L, 0L,  0L, 0L,    0x0L},

 float2_ieee[]    = {16L,   5L,  10L, 0L, 1L,  6L, 0L,    0xFL},
 float4_ieee[]    = {32L,   8L,  23L, 0L, 1L,  9L, 0L,   0x7FL},
 float8_ieee[]    = {64L,  11L,  52L, 0L, 1L, 12L, 0L,  0x3FFL},
 float10_ieee[]   = {80L,  15L,  64L, 0L, 1L, 32L, 1L, 0x3FFFL},
 float12_ieee[]   = {96L,  15L,  64L, 0L, 1L, 32L, 1L, 0x3FFEL},
 float16_ieee[]   = {128L, 11L, 116L, 0L, 1L, 12L, 0L, 0x3FFFL},

/* historical formats */

#ifdef GFLOAT
 float4_vax[]     = {32L,   8L,  23L, 0L, 1L,  9L, 0L,   0x81L},
 float8_vax[]     = {64L,  11L,  52L, 0L, 1L, 12L, 0L,  0x401L},
#else
 float4_vax[]     = {32L,   8L,  23L, 0L, 1L,  9L, 0L,   0x81L},
 float8_vax[]     = {64L,   8L,  55L, 0L, 1L,  9L, 0L,   0x81L},
#endif

 float8_cray[]    = {64L,  15L,  48L, 0L, 1L, 16L, 1L, 0x4000L},

/* GOTCHA: unsure of this one */
 float16_cray[]   = {128L, 15L, 112L, 0L, 1L, 16L, 1L, 0x4000L};

/* Data standard for the different architectures
 *
 *   struct s_data_standard
 *      {int bits_byte;
 *       int ptr_bytes;
 *       int short_bytes;
 *       int short_order;
 *       int int_bytes;
 *       int int_order;
 *       int long_bytes;
 *       int long_order;
 *       int longlong_bytes;
 *       int longlong_order;
 *       int float_bytes;
 *       long *float_format;
 *       int *float_order;
 *       int double_bytes;
 *       long *double_format;
 *       int *double_order;
 *       int quad_bytes;
 *       long *quad_format;
 *       int *quad_order;};
 *   
 *   typedef struct s_data_standard data_standard;
 *
 *   NOTE: NORMAL_ORDER is big-endian and REVERSE_ORDER is little-endian
 */

data_standard
 TEXT_STD   = {BITS_DEFAULT,                               /* bits per byte */
               12,                                       /* size of pointer */
               7,  TEXT_ORDER,                   /* size and order of short */
               12, TEXT_ORDER,                     /* size and order of int */
               22, TEXT_ORDER,                    /* size and order of long */
               22, TEXT_ORDER,               /* size and order of LONG_LONG */
               15, float4_text, text_order,             /* float definition */
               26, float8_text, text_order,            /* double definition */
               42, float16_text, text_order},            /* quad definition */
 I386_STD   = {BITS_DEFAULT,                               /* bits per byte */
               4,                                        /* size of pointer */
               2, REVERSE_ORDER,                 /* size and order of short */
               2, REVERSE_ORDER,                   /* size and order of int */
               4, REVERSE_ORDER,                  /* size and order of long */
               4, REVERSE_ORDER,             /* size and order of LONG_LONG */
               4, float4_ieee, float4_rev_order,        /* float definition */
               8, float8_ieee, float8_rev_order,       /* double definition */
               16, float16_ieee, float16_nrm_order},     /* quad definition */
 I586_STD   = {BITS_DEFAULT,                               /* bits per byte */
               4,                                        /* size of pointer */
               2, REVERSE_ORDER,                 /* size and order of short */
               4, REVERSE_ORDER,                   /* size and order of int */
               4, REVERSE_ORDER,                  /* size and order of long */
               8, REVERSE_ORDER,             /* size and order of LONG_LONG */
               4, float4_ieee, float4_rev_order,        /* float definition */
               8, float8_ieee, float8_rev_order,       /* double definition */
               16, float16_ieee, float16_nrm_order},     /* quad definition */
 X86_64_STD = {BITS_DEFAULT,                               /* bits per byte */
               8,                                        /* size of pointer */
               2, REVERSE_ORDER,                 /* size and order of short */
               4, REVERSE_ORDER,                   /* size and order of int */
               8, REVERSE_ORDER,                  /* size and order of long */
               8, REVERSE_ORDER,             /* size and order of LONG_LONG */
               4, float4_ieee, float4_rev_order,        /* float definition */
               8, float8_ieee, float8_rev_order,       /* double definition */
               16, float16_ieee, float16_nrm_order},     /* quad definition */
 PPC32_STD  = {BITS_DEFAULT,                               /* bits per byte */
               4,                                        /* size of pointer */
               2, NORMAL_ORDER,                  /* size and order of short */
               4, NORMAL_ORDER,                    /* size and order of int */
               4, NORMAL_ORDER,                   /* size and order of long */
               8, NORMAL_ORDER,              /* size and order of LONG_LONG */
               4, float4_ieee, float4_nrm_order,        /* float definition */
               8, float8_ieee, float8_nrm_order,       /* double definition */
               16, float16_ieee, float16_nrm_order},     /* quad definition */
 PPC64_STD  = {BITS_DEFAULT,                               /* bits per byte */
               8,                                        /* size of pointer */
               2, NORMAL_ORDER,                  /* size and order of short */
               4, NORMAL_ORDER,                    /* size and order of int */
               8, NORMAL_ORDER,                   /* size and order of long */
               8, NORMAL_ORDER,              /* size and order of LONG_LONG */
               4, float4_ieee, float4_nrm_order,        /* float definition */
               8, float8_ieee, float8_nrm_order,       /* double definition */
               16, float16_ieee, float16_nrm_order},     /* quad definition */

/* obsolete standards */
 M68X_STD   = {BITS_DEFAULT,                               /* bits per byte */
               4,                                        /* size of pointer */
               2, NORMAL_ORDER,                  /* size and order of short */
               2, NORMAL_ORDER,                    /* size and order of int */
               4, NORMAL_ORDER,                   /* size and order of long */
               4, NORMAL_ORDER,              /* size and order of LONG_LONG */
               4, float4_ieee, float4_nrm_order,        /* float definition */
               12, float12_ieee, float12_nrm_order,    /* double definition */
               16, float16_ieee, float16_nrm_order},     /* quad definition */
 VAX_STD    = {BITS_DEFAULT,                               /* bits per byte */
               4,                                        /* size of pointer */
               2, REVERSE_ORDER,                 /* size and order of short */
               4, REVERSE_ORDER,                   /* size and order of int */
               4, REVERSE_ORDER,                  /* size and order of long */
               4, REVERSE_ORDER,             /* size and order of LONG_LONG */
               4, float4_vax, float4_vax_order,         /* float definition */
               8, float8_vax, float8_vax_order,        /* double definition */
               8, float8_vax, float8_vax_order},         /* quad definition */
 CRAY_STD   = {BITS_DEFAULT,                               /* bits per byte */
               8,                                        /* size of pointer */
               8, NORMAL_ORDER,                  /* size and order of short */
               8, NORMAL_ORDER,                    /* size and order of int */
               8, NORMAL_ORDER,                   /* size and order of long */
               8, NORMAL_ORDER,              /* size and order of LONG_LONG */
               8, float8_cray, float8_nrm_order,        /* float definition */
               8, float8_cray, float8_nrm_order,       /* double definition */
               16, float16_cray, float16_nrm_order};     /* quad definition */

/*--------------------------------------------------------------------------*/

/*                            DATA_ALIGNMENTS                               */

/*--------------------------------------------------------------------------*/

/* Word Alignment for the different architectures
 *
 *   struct s_data_alignment
 *      {int char_alignment;
 *       int ptr_alignment;
 *       int short_alignment;
 *       int int_alignment;
 *       int long_alignment;
 *       int longlong_alignment;
 *       int float_alignment;
 *       int double_alignment;
 *       int quad_alignment;
 *       int struct_alignment;};
 *   
 *   typedef struct s_data_alignment data_alignment;
 */

data_alignment
 TEXT_ALIGNMENT        = {0, 0, 0, 0, 0, 0, 0, 0,  0, 0},
 BYTE_ALIGNMENT        = {1, 1, 1, 1, 1, 1, 1, 1,  1, 0},
 WORD2_ALIGNMENT       = {1, 2, 2, 2, 2, 2, 2, 2,  2, 0},
 WORD4_ALIGNMENT       = {1, 4, 4, 4, 4, 4, 4, 4,  4, 0},
 WORD8_ALIGNMENT       = {4, 8, 8, 8, 8, 8, 8, 8,  8, 8},
 GNU4_I686_ALIGNMENT   = {1, 4, 2, 4, 4, 4, 4, 4,  4, 0},
 OSX_10_5_ALIGNMENT    = {1, 4, 2, 4, 4, 4, 4, 4, 16, 0},
 SPARC_ALIGNMENT       = {1, 4, 2, 4, 4, 4, 4, 8,  8, 0},
 XLC32_PPC64_ALIGNMENT = {1, 4, 2, 4, 4, 8, 4, 4,  4, 0},
 CYGWIN_I686_ALIGNMENT = {1, 4, 2, 4, 4, 8, 4, 8,  4, 0},
 GNU3_PPC64_ALIGNMENT  = {1, 4, 2, 4, 4, 8, 4, 8,  8, 0},
 GNU4_PPC64_ALIGNMENT  = {1, 4, 2, 4, 4, 8, 4, 8, 16, 0},
 XLC64_PPC64_ALIGNMENT = {1, 8, 2, 4, 8, 8, 4, 4,  4, 0},
 GNU4_X86_64_ALIGNMENT = {1, 8, 2, 4, 8, 8, 4, 8, 16, 0},
 PGI_X86_64_ALIGNMENT  = {1, 8, 2, 4, 8, 8, 4, 8,  8, 0};

/*--------------------------------------------------------------------------*/

/*                             HELPER ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* _PD_NCOPY - copy the NITEMS of size BPI from IN to OUT */

static void _PD_ncopy(char **out, char **in, long nitems, long bpi)
   {long nbytes;

    nbytes = nitems*bpi;
    memcpy(*out, *in, nbytes);

    *in  += nbytes;
    *out += nbytes;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SET_BIT - set the bit specified as on offset from the given pointer */

static void _PD_set_bit(char *base, int offs)
   {int nbytes, mask;

    nbytes  = offs >> 3;
    base   += nbytes;
    offs   -= 8*nbytes;  

    mask    = (1 << (7 - offs));
    *base  |= mask;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_GET_BIT - return the bit specified as on offset
 *             - from the given pointer
 */

static int _PD_get_bit(char *base, int offs, int nby, int *ord)
   {int nbytes, mask, n;

    n      = offs >> 3;
    nbytes = n % nby;
    n     -= nbytes;
    offs   = offs % 8;

    if (ord == NULL)
       base += (n + nbytes);
    else
       base += (n + (ord[nbytes] - 1));

    mask = (1 << (7 - offs));

    return((*base & mask) != 0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SIGN_EXTEND - sign extend integers which may come from odd bit
 *                 - sized sources
 */

void _PD_sign_extend(char *out, long nitems, int nbo, int nbti, int *ord)
   {int i, j, sba, tsba, sign, indx;
    unsigned char *lout, mask;

    sba = 8*nbo - nbti;

/* if sba is less than zero we have truncated the integers and really
 * don't know about the sign
 */
    if (sba < 0)
       return;

    lout = (unsigned char *) out;
    for (i = 0L; i < nitems; i++)
        {sign = _PD_get_bit((char *) lout, sba, nbo, ord);
         tsba = sba;
         if (sign)
            {for (j = 0; (tsba > 8); j++, tsba -= 8)
	         {indx = (ord != NULL) ? ord[j] - 1 : j;
		  lout[indx] = 0xFF;};

	     mask = ((1 << tsba) - 1) << (8 - tsba);
	     indx = (ord != NULL) ? ord[j] - 1 : j;
	     lout[indx] |= mask;};

	 lout += nbo;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_ONES_COMPLEMENT - convert ones complement integers to twos
 *                     - complement
 *                     - NOTE: modern machines use twos complement arithmetic
 *                     -       and therefore this is a one way conversion
 */

void _PD_ones_complement(char *out, long nitems, int nbo, int *order)
   {int i, j, indx;
    unsigned int carry;
    unsigned char *lout;

    lout = (unsigned char *) out;
    for (i = 0L; i < nitems; i++)
        {indx = (order != NULL) ? order[0] - 1 : 0;
         if (lout[indx] & 0x80 )
	    {carry = 1;
	     for (j = nbo-1; (j >= 0) && (carry > 0); j--)
	         {indx = (order != NULL) ? order[j] - 1 : j;
                  carry  += lout[indx];
		  lout[indx] = carry & 0xFF;
		  carry   = (carry > 0xFF);};};

	 lout += nbo;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_BYTE_REVERSE - byte reverse nitems words
 *                 - each word is nb bytes long where nb is even
 */

void PD_byte_reverse(char *out, long nb, long nitems)
   {long i, jl, jh, nbo2;
    char tmp;
    char *p1, *p2;

    nbo2 = nb >> 1;
    for (jl = 0; jl < nbo2; jl++)
        {jh = nb - jl - 1;
         p1 = out + jh;
         p2 = out + jl;
	 for (i = 0L; i < nitems; i++)
             {tmp = *p1;
              *p1 = *p2;
              *p2 = tmp;
              p1 += nb;
              p2 += nb;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_INSERT_FIELD - copy the least significant NB bits from INL
 *                  - into the byte array pointed to by OUT
 *                  - all indexing is 0 based
 *                  - OFFS is the offset from the beginning of OUT in bits
 *                  - this assumes that the output bit array is initialized
 *                  - to all zeros after offs
 */

void _PD_insert_field(long inl, int nb, char *out,
		      int offs, int lord, int lby)
   {int mi, n, dm;
    long longmask;
    static int MaxBits = 8*sizeof(long);
    unsigned char fb;
    char *in;

    in = (char *) &inl;

/* if the output start bit is not in the first byte move past the apropriate
 * number of bytes so that the start bit is in the first byte
 */
    if (offs > 7)
       {out  += (offs >> 3);
        offs %= 8;};
        
/* if mi is less than offs, copy the first dm bits over, reset offs to 0,
 * advance mi by dm, and handle the rest as if mi >= offs
 */
    mi = MaxBits - nb;
    if (mi < offs)
       {dm = MaxBits - (8 - offs);
        if (nb == MaxBits)
           longmask = ~((1L << dm) - 1L);
        else
           longmask = ((1L << nb) - 1L) ^ ((1L << dm) - 1L);

        fb = ((inl & longmask) >> dm) & ((1L << (nb - dm)) - 1L);
        *(out++) |= fb;

        mi  += 8 - offs;
        offs = 0;};

/* assuming mi >= offs, left shift the input so that it is bit aligned
 * with the output
 */
    dm       = mi - offs;
    longmask = ~((1L << dm) - 1L);
    inl      = (inl << dm) & longmask;

/* reorder the bytes apropriately */
    if (lord == REVERSE_ORDER)
       PD_byte_reverse(in, lby, 1L);

/* copy the remaining aligned bytes over */
    for (n = (offs+nb+7)/8; n > 0; n--, *(out++) |= *(in++));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_REORDER - given a pointer to an array ARR with NITEMS of NBYTES each
 *             - put them in the order defined by ORD
 */

static void _PD_reorder(char *arr, long nitems, int nbytes, int *ord)
   {char local[MAXLINE];
    int j, lreorder;

    lreorder = FALSE;

    for (j = 0; j < nbytes; j++)
        if (ord[j] != (j+1))
           {lreorder = TRUE;
             break;}

    if (lreorder)
       {for (; nitems > 0; nitems--)
           {arr--;
            for (j = 0; j < nbytes; local[j] = arr[ord[j]], j++);
            arr++;
            for (j = 0; j < nbytes; *(arr++) = local[j++]);};}

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BYTE_ALIGN - given an input bitstream of packed items,
 *                - return an array of the items aligned on byte
 *                - boundaries
 */

static void _PD_byte_align(char *out, char *in, long nitems,
			   long *infor, int *inord, int boffs)
   {long nbitsin, inrem;
    int chunk1, chunk2, outbytes, remainder, i;
    unsigned char *inptr, *outptr;
    unsigned char mask1, mask2;
 
    nbitsin = infor[0];

    outbytes = (nbitsin >> 3) + 1;
 
    memset(out, 0, nitems*outbytes);

/* develop logic for doing the unpack without regard to inord. */
    inptr  = (unsigned char *)in;
    outptr = (unsigned char *)out;
    inrem  = nbitsin * nitems;
    remainder = nbitsin % 8;

    chunk1 = min(8, nbitsin);
    chunk1 -= boffs;
    chunk2 =  (boffs == 0) ? 0 : 8 - chunk1;
    mask1  = (1 << chunk1) - 1;
    mask2  = (1 << chunk2) - 1;

    while (inrem)
          {for (i=0; i<outbytes-1; i++, inptr++, outptr++, inrem -= 8)
               {*outptr = ((*inptr & mask1) << chunk2) |    
                          ((*(inptr + 1)  >> chunk1) & mask2);}
           
           if (remainder <= chunk1)
              {*outptr++ = (*inptr << chunk2) & 
                           (((1 << remainder) - 1)  << (8 - remainder));
               if (remainder == chunk1) inptr++;
               chunk1 -= remainder;
               if (chunk1 == 0) chunk1 = 8;
               chunk2  = 8 - chunk1;}
            else
              {*outptr =    ((*inptr << chunk2) &
                            (((1 << chunk1) - 1) << chunk2));
               *outptr++ |= ((*(++inptr) >> chunk1) &
                            (((1 << (remainder - chunk1)) - 1) << (8 - (remainder - chunk1) - chunk1)));
               chunk1 = 8 - (remainder - chunk1);
               chunk2 = 8 - chunk1;}
           mask1 = (1 << chunk1) - 1;
           mask2 = (1 << chunk2) - 1;
           inrem -= remainder;}
    
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FIELD_REORDER - given an input byte array containing floating
 *                   - pt. numbers with fields as described by the input
 *                   - format, return an array containing the equivalent
 *                   - numbers with the sign bit, exponent and mantissa
 *                   - as specified by the output format.
 */

static void _PD_field_reorder(char *in, char *out, long *infor,
			      long *outfor, int *inord, int lord,
                              int lby, int nitems)
   {int nbi, nbo, nbi_exp, nbo_exp, bi_sign, bo_sign,
        bi_exp, bo_exp, bi_mant, bo_mant, inbytes,
        outbytes;
    int inrem, outrem, indxin, indxout, nbits, sign;
    long i, expn, mant;
    char *lout, *lin;
    static int BitsMax = 8*sizeof(long);


    nbi     = infor[0];
    nbo     = outfor[0];
    nbi_exp = infor[1];
    nbo_exp = outfor[1];

    bi_sign = infor[3];
    bo_sign = outfor[3];
    bi_exp  = infor[4];
    bo_exp  = outfor[4];
    bi_mant = infor[5];
    bo_mant = outfor[5];

    inbytes   = (nbi + 7) >> 3;
    outbytes  = (nbo + 7) >> 3;

/* zero out the output buffer */
    memset(out, 0, nitems*outbytes);

    lout = out;
    lin  = in;
    for (i = 0L; i < nitems; i++)

/* move the exponent over */
        {expn = SC_extract_field(lin, bi_exp, nbi_exp, inbytes, inord);
         sign = _PD_get_bit(lin, bi_sign, inbytes, inord);


         _PD_insert_field(expn, nbo_exp, lout, bo_exp,
                          lord, lby);

         if (sign)
             _PD_set_bit(lout, bo_sign);

         indxin  = bi_mant;
         inrem   = infor[2];
         indxout = bo_mant;
         outrem  = outfor[2];

/* move the mantissa over in sizeof(long) packets */
         while ((inrem > 0) && (outrem > 0))
               {nbits = min(BitsMax, inrem);
                nbits = min(nbits, outrem);
                mant  = SC_extract_field(lin, indxin, nbits,
					  inbytes, inord);

                _PD_insert_field(mant, nbits, lout, indxout,
                                 lord, lby);

                indxin  += nbits;
                indxout += nbits;
                inrem   -= nbits;
                outrem  -= nbits;}

	 bi_sign += nbi;
	 bi_exp  += nbi;
	 bi_mant += nbi;
	 bo_sign += nbo;
	 bo_exp  += nbo;
	 bo_mant += nbo;};

    return;}

/*--------------------------------------------------------------------------*/

/*                         HIGH LEVEL CONVERSION ROUTINES                   */

/*--------------------------------------------------------------------------*/

/* _PD_NULL_POINTER - if the input contains only null bytes, return TRUE
 *                  - otherwise, return FALSE
 */

int _PD_null_pointer(char *in, int nb)
   {int i;

    for (i = 0; i < nb; i++)
        if (*in++)
           return(FALSE);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PRIM_TYPEP - returns TRUE if the given struct member is a primitive
 *                - data type and FALSE otherwise
 */

int _PD_prim_typep(char *memb, hasharr *chrt, PD_major_op error)
   {int rv;
    defstr *pd;

    rv = FALSE;
    pd = PD_inquire_table_type(chrt, memb);
    if (pd == NULL)
       PD_error("BAD TYPE FROM STRUCTURE CHART - _PD_PRIM_TYPEP", error);
    else
       rv = (pd->members == NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                         TEXT CONVERSION ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* _PD_BIN_TEXT - convert binary numbers to text */

static void _PD_bin_text(char **out, char **in, char *typ, long nitems,
			 int boffs,
			 PD_type_kind kndi, long *ifmt, long nbi,
			 PD_byte_order ordi, int *iord,
			 PD_type_kind kndo, long *ofmt, long nbo,
			 PD_byte_order ordo, int *oord,
			 data_standard *hstd,
			 int onescmp, int usg, int rdx, char *delim)
   {int fl, nc, nd;
    int *hord;
    long i, nb, ne, nbl;
    long *hfmt;
    char fmt[MAXLINE];
    char *lout, *lin, *tout, *p;
    PD_byte_order ordl;

    lin  = *in;
    lout = *out;

/* GOTCHA: should this now be quad_format? */
    hfmt = hstd->double_format;
    hord = hstd->double_order;
    ordl = hstd->long_order;
    nbl  = hstd->long_bytes;

/* convert char types */
    if (kndi == CHAR_KIND)
       {nb = nitems*nbi;
	if (delim == NULL)
	   {snprintf(fmt, MAXLINE, "%%-%lds", nb);
	    nc = nb;}
	else
	   {ne = strlen(lin);
	    ne = min(ne, nb);
	    nc = ne + strlen(delim) + 2;
	    if (strchr(lin, '\'') == NULL)
	       snprintf(fmt, MAXLINE, "'%%-%lds'%s", ne, delim);
	    else
	       snprintf(fmt, MAXLINE, "\"%%-%lds\"%s", ne, delim);};
	snprintf(lout, nc+1, fmt, lin);

	*in  += nb;
	*out += nc;}

/* convert integer types */
    else if (kndi == INT_KIND)
       {long *lv;

	lv   = FMAKE_N(long, nitems, "_PD_BIN_TEXT:lv");
	tout = (char *) lv;

	_PD_iconvert(&tout, in, nitems, nbi, ordi, nbl, ordl, onescmp, usg);

	if (delim == NULL)
	   snprintf(fmt, MAXLINE, "%%%ldld", nbo);
	else
	   {snprintf(fmt, MAXLINE, "%%%ldld%s", nbo, delim);
	    nbo += strlen(delim);};

        for (p = lout, i = 0; i < nitems; p += nbo, i++)
	    snprintf(p, nbo+1, fmt, lv[i]);

	SFREE(lv);

	*out += nbo*nitems;}

/* convert floating point types */
    else if (kndi == FLOAT_KIND)
       {double *dv;

	dv   = FMAKE_N(double, nitems, "_PD_BIN_TEXT:dv");
	tout = (char *) dv;

	_PD_fconvert(&tout, in, nitems, boffs,
		     ifmt, iord, hfmt, hord,
		     ordl, nbl, onescmp);

        fl = ofmt[0];
        nd = ofmt[2];
	if (delim == NULL)
	   snprintf(fmt, MAXLINE, "%%%d.%de", fl, nd);
	else
	   {snprintf(fmt, MAXLINE, "%%%d.%de%s", fl, nd, delim);
	    nbo += strlen(delim);};

        for (p = lout, i = 0; i < nitems; p += nbo, i++)
	    snprintf(p, nbo+1, fmt, dv[i]);

	SFREE(dv);

	*out += nbo*nitems;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_TEXT_BIN - convert text to binary numbers */

static void _PD_text_bin(char **out, char **in, char *typ, long nitems,
			 int boffs,
			 PD_type_kind kndi, long *ifmt, long nbi,
			 PD_byte_order ordi, int *iord,
			 PD_type_kind kndo, long *ofmt, long nbo,
			 PD_byte_order ordo, int *oord,
			 data_standard *hstd,
			 int onescmp, int usg, int rdx, char *delim)
   {int *hord;
    long i, nb, nc, ne, nbl;
    long *hfmt;
    char *lout, *lin, *tin, *p;
    PD_byte_order ordl;
    char *(*strdelim)(const char *a, const char *b);

/* GOTCHA: two possible delimiter strategies
 *  delimeter is string such "abc" - e.g. "1abc2abc3abc"
 *  in which case use strstr
 *  delimeter is a single character from list such "abc" - e.g. "1a2b3c"
 *  in which case use strpbrk
 */
    strdelim = strpbrk;

    lin = *in;
    lout = *out;

/* GOTCHA: should this now be quad_format? */
    hfmt = hstd->double_format;
    hord = hstd->double_order;
    ordl = hstd->long_order;
    nbl  = hstd->long_bytes;

    if (delim == NULL)
       delim = "\n";

/* convert char types */
    if (kndi == CHAR_KIND)
       {nb = nitems*nbo;
	p  = strdelim(lin, delim);
	if (p != NULL)
	   {*p = '\0';
	    nc = strlen(lin) + 1;
	    ne = min(nb + 1, nc);}
	else
	   {nc = nitems*nbi;
	    ne = min(nb + 1, nc + 1);};

	SC_strncpy(*out, ne, lin, -1);

	*out += nb;
	lin  += nc;}

/* convert integer types */
    else if (kndi == INT_KIND)
       {long *lv;

	lv = FMAKE_N(long, nitems, "_PD_TEXT_BIN:lv");

        for (i = 0; i < nitems; i++)
	    {p = strdelim(lin, delim);
	     if (p != NULL)
	        *p++ = '\0';

	     lv[i] = SC_strtol(lin, &lin, rdx);
	     if (*delim != '\n')
	        lin++;};

	tin = (char *) lv;
	_PD_iconvert(out, &tin, nitems, nbl, ordl, nbo, ordo, onescmp, usg);

	SFREE(lv);}

/* convert floating point types */
    else if (kndi == FLOAT_KIND)
       {double *dv;

	dv = FMAKE_N(double, nitems, "_PD_TEXT_BIN:dv");

	for (i = 0; i < nitems; i++)
	    {p = strdelim(lin, delim);
	     if (p != NULL)
	        *p++ = '\0';

	     dv[i] = SC_strtod(lin, &lin);
	     if (*delim != '\n')
	        lin++;};

	tin = (char *) dv;
	_PD_fconvert(out, &tin, nitems, boffs,
		     hfmt, hord, ofmt, oord,
		     ordl, nbl, onescmp);

	SFREE(dv);};

    *in = lin;

    return;}

/*--------------------------------------------------------------------------*/

/*                         POINTER CONVERSION ROUTINES                      */

/*--------------------------------------------------------------------------*/

/* _PD_CONVERT_PTR - convert pointer in the following sense
 *                 - writing:
 *                 -    map memory address into an index <n>
 *                 -    which, under the name /&ptrs/ia_<n>, the
 *                 -    pointee is written
 *                 - reading:
 *                 -    read the index <n>
 *                 -    which, under the name /&ptrs/ia_<n>, the
 *                 -    pointee may be read
 *                 -    set the memory address to point to the
 *                 -    space into which is allocated by PDBLib
 *                 -    to recieve the pointee
 *                 - FO is TRUE iff the input is a memory address (write)
 *                 - HO is TRUE iff the output is a memory address (read)
 *                 - NBO is the output size (possibly plus alignment)
 *                 - NBi is the input size (possibly plus alignment)
 */

static int _PD_convert_ptr(char **pout, char **pin, long *poo, long *pio,
			   int nitems, hasharr *chi, hasharr *cho,
			   data_standard *stdo, data_standard *stdi,
			   data_standard *hstd, long iao, long iai)
   {int loc, fbpi, hbpi;
    long i, n, nbo, nbi;
    char *in, *out, *p;
    PDBfile *file;
    data_standard *hs, *fs;
    PD_byte_order ford, hord;

    nbo = stdo->ptr_bytes + iao;
    nbi = stdi->ptr_bytes + iai;

    for (i = 0; i < nitems; i++)
        {n = 1L;
	 if (hstd->file != NULL)
	    {file = hstd->file;
	     hs   = file->host_std;
	     fs   = file->std;
	     ford = fs->long_order;
	     fbpi = fs->ptr_bytes;
	     hord = hs->long_order;
	     hbpi = hs->long_bytes;
       
/* convert pointer when writing to file */
	     if (stdi == hstd)
	        {p = *pin;
		 p = DEREF(p);
		 n = _PD_ptr_wr_lookup(file, p, &loc, FALSE);

		 in  = (char *) &n;
		 out = *pout;
		 if (_PD_TEXT_OUT(file) == TRUE)
		    {memset(out, ' ', fbpi);
		     _PD_bin_text(&out, &in, "integer", 1L, 0,
				  INT_KIND, NULL, hbpi, hord, NULL,
				  INT_KIND, NULL, fbpi, ford, NULL,
				  hs, FALSE, FALSE, 10, NULL);}
		 else
		    {memset(out, 0, fbpi);
		     _PD_iconvert(&out, &in, 1L, hbpi, hord, fbpi, ford,
				  FALSE, FALSE);};}

/* convert pointer when reading from file */
	     else if (stdo == hstd)
	        {if (ford == TEXT_ORDER)
		    {in  = *pin;
		     out = (char *) &n;
		     _PD_text_bin(&out, &in, "integer", 1L, 0,
				  INT_KIND, NULL, fbpi, ford, NULL,
				  INT_KIND, NULL, hbpi, hord, NULL,
				  hs, FALSE, FALSE, 10, NULL);
		     n = _PD_ptr_fix(file, n);}
		 else
		    n = _PD_ptr_get_index(file, *pin);

		 if (file->use_itags == FALSE)
		    _PD_index_ptr(*pout, n);};};

	 *pin  += nbi;
	 *pio  += nbi;
	 *pout += nbo;
	 *poo  += nbo;};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CONVERT_FNC - convert function pointer
 *                 - this is a no-op which skips over the space
 */

static int _PD_convert_fnc(char **pout, char **pin, long *poo, long *pio,
			   long nbo, long nbi)
   {

    if (_PD_null_pointer(*pin, nbi) == FALSE)
       **pout = 1;

    *pin  += nbi;
    *pio  += nbi;
    *pout += nbo;
    *poo  += nbo;

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                           ASCII CONVERSION ROUTINES                      */

/*--------------------------------------------------------------------------*/

/* _PD_CONVERT_ASCII - translate characters from BCPI bit ASCII to
 *                   - 8-bit ASCII
 *                   - NOTE: this is only here to allow PDB to read old
 *                   -       machine data which may not be 8 bit ASCII
 *                   -
 *                   - Inputs : IN     - input buffer
 *                   -          NITEMS - number of fields to translate
 *                   -          BCPI   - bits per character on input
 *                   -          OFFS   - zero-origin bit offset from IN
 *                   -                   to start of data
 *                   - Output:  OUT    - output buffer
 */

void _PD_convert_ascii(char *out, char *in, long nitems,
		       int bpci, int offs)
   {long i;

/* translate chars by adding a blank (0x20) character
 * to give upper-case letters then or-ing the sum with a blank
 * to give lower-case letters
 */
    for (i = 0; i < nitems; i++)
        out[i] = (out[i] + ' ') | ' ';

    return;}

/*--------------------------------------------------------------------------*/

/*                      INTEGER CONVERSION ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* _PD_ICONVERT - convert integers of nbi bytes to integers of nbo bytes
 *              - the number of bytes for each integer are given
 */

void _PD_iconvert(char **out, char **in, long nitems,
		  long nbi, PD_byte_order ordi,
		  long nbo, PD_byte_order ordo,
		  int onescmp, int usg)
   {long i;
    int j;
    char *lout, *lin, *po, *pi;
    char s_extend;

    lin = *in;
    lout = *out;

    s_extend = usg ? 0 : 0xff;

/* convert nitems integers
 * test sign bit to properly convert negative integers
 */
    if (nbi < nbo)
       {if (ordi == REVERSE_ORDER)
	   {for (j = nbi; j < nbo; j++)
                {po = lout + j - nbi;
                 pi = lin + nbi - 1;
                 for (i = 0L; i < nitems; i++)
                     {*po = (*pi & 0x80) ? s_extend : 0;
                      po += nbo;
		      pi += nbi;};};
	    for (j = nbi; j > 0; j--)
                {po = lout + nbo - j;
                 pi = lin + j - 1;
                 for (i = 0L; i < nitems; i++)
                     {*po = *pi;
                      po += nbo;
		      pi += nbi;};};}
        else
	   {for (j = nbi; j < nbo; j++)
                {po = lout + j - nbi;
                 pi = lin;
                 for (i = 0L; i < nitems; i++)
                     {*po = (*pi & 0x80) ? s_extend : 0;
                      po += nbo;
		      pi += nbi;};};
	    for (j = 0; j < nbi; j++)
                {po = lout + j + nbo - nbi;
                 pi = lin + j;
                 for (i = 0L; i < nitems; i++)
                     {*po = *pi;
                      po += nbo;
		      pi += nbi;};};};}

    else if (nbi >= nbo)
       {if (ordi == REVERSE_ORDER)
           {for (j = nbo; j > 0; j--)
                {po = lout + nbo - j;
                 pi = lin + j - 1;
                 for (i = 0L; i < nitems; i++)
                     {*po = *pi;
                      po += nbo;
		      pi += nbi;};};}
        else
           {for (j = nbi - nbo; j < nbi; j++)
                {po = lout + j - nbi + nbo;
                 pi = lin + j;
                 for (i = 0L; i < nitems; i++)
                     {*po = *pi;
                      po += nbo;
		      pi += nbi;};};};};

/* if the input used ones complement arithmetic convert to twos complement */
    if (onescmp)
       _PD_ones_complement(*out, nitems, nbo, NULL);
                          
    if (ordo == REVERSE_ORDER)
       PD_byte_reverse(*out, nbo, nitems);

    *in  += nitems*nbi;
    *out += nitems*nbo;

    return;}

/*--------------------------------------------------------------------------*/

/*                       FLOAT CONVERSION ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* Parametrized Data Conversion Method
 *
 * Floating point formats are characterized by a set of parameters which
 * describe the fundamental elements of a floating point number. These are
 *
 *  Sign     - always assumed to be a single bit
 *           - requires bit offset
 *  Exponent - assumed to be a biased integer smaller than 32 bits
 *           - (this allows the conversion to use a long on all known
 *           - platforms - an exponent greater than 32 bits long would
 *           - allow much larger numbers than should be needed for
 *           - scientific computations)
 *           - requires a bit offset, a bit length, and a bias
 * Mantissa  - assumed to be a bitstream of arbitrary length
 *           - requires a bit offset and a bit length
 * HMB       - in all floating point representations the mantissa is
 *           - normalized so that the most significant bit is one.
 *           - in some formats the one is explicitly included in the
 *           - representation and in others it is only implicit
 *           - this gives some formats an extra bit of precision.
 *           - requires a flag which is TRUE if the HMB is explicit
 * 
 * Two other factors involved are: the byte order which could be
 * mixed with the bit layout of the numbers but isn't in actual practice
 * on current machines; and whether one's complement or two's complement
 * arithmetic is used. Modern machines all use two's complement arithmetic
 * and the model used here and now is that data from one's complement
 * machines is to be read only.  This restriction is relatively easy
 * to relax, but there is no evidence that it should be.
 *
 * An issue which is not a problem in the current implementation is that
 * old machines with byte sizes other than 8 bits can be accomodated
 * because the conversions treat the input and output as bitstreams
 * instead of bytestreams.
 *
 * The conversion process is summarized as follows:
 *   1) Extract the sign bit and exponent field from the input number
 *   2) Subtract the bias of the source format and add the bias
 *      of the target format
 *   3) Check for overflow in the exponent
 *   4) Insert the new exponent and the sign bit in the target stream
 *   5) Copy the mantissa bits from the source to the target
 *      compensating for differences in the HMB between the two
 *      formats
 *   6) Take care of any known anomalies - e.g. CRAY format is
 *      inconsistent in that the HMB is explicitly on for all numbers
 *      with the exception of 0.0
 *   7) Reorder the bytes of the target stream appropriately
 *
 * The floating point formats for a variety of platforms are supplied by
 * PDBLib and are defined at the top of this file
 *
 */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* XFER2 - transfer _NI items at _BPI bytes per item from _IBF to _OBF
 *       - rearranging the byte order according to _IBO and _OBO
 */

#define XFER2(_obf, _ni, _ibf, _ibo, _obo, _bpi)                             \
   {int _i;                                                                  \
    int _ibo0, _ibo1;                                                        \
    int _obo0, _obo1;                                                        \
    _ibo0   = _ibo[0] - 1;                                                   \
    _ibo1   = _ibo[1] - 1;                                                   \
    _obo0  = _obo[0] - 1;                                                    \
    _obo1  = _obo[1] - 1;                                                    \
    for (_i = 0; _i < _ni; _i++, _obf += _bpi, _ibf += _bpi)                 \
        {_obf[_obo0]  = _ibf[_ibo0];                                         \
	 _obf[_obo1]  = _ibf[_ibo1];};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* XFER4 - transfer _NI items at _BPI bytes per item from _IBF to _OBF
 *       - rearranging the byte order according to _IBO and _OBO
 */

#define XFER4(_obf, _ni, _ibf, _ibo, _obo, _bpi)                             \
   {int _i;                                                                  \
    int _ibo0, _ibo1, _ibo2, _ibo3;                                          \
    int _obo0, _obo1, _obo2, _obo3;                                          \
    _ibo0   = _ibo[0] - 1;                                                   \
    _ibo1   = _ibo[1] - 1;                                                   \
    _ibo2   = _ibo[2] - 1;                                                   \
    _ibo3   = _ibo[3] - 1;                                                   \
    _obo0  = _obo[0] - 1;                                                    \
    _obo1  = _obo[1] - 1;                                                    \
    _obo2  = _obo[2] - 1;                                                    \
    _obo3  = _obo[3] - 1;                                                    \
    for (_i = 0; _i < _ni; _i++, _obf += _bpi, _ibf += _bpi)                 \
        {_obf[_obo0]  = _ibf[_ibo0];                                         \
	 _obf[_obo1]  = _ibf[_ibo1];                                         \
	 _obf[_obo2]  = _ibf[_ibo2];                                         \
	 _obf[_obo3]  = _ibf[_ibo3];};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* XFER8 - transfer _NI items at _BPI bytes per item from _IBF to _OBF
 *       - rearranging the byte order according to _IBO and _OBO
 */

#define XFER8(_obf, _ni, _ibf, _ibo, _obo, _bpi)                             \
   {int _i;                                                                  \
    int _ibo0, _ibo1, _ibo2, _ibo3, _ibo4, _ibo5, _ibo6, _ibo7;              \
    int _obo0, _obo1, _obo2, _obo3, _obo4, _obo5, _obo6, _obo7;              \
    _ibo0   = _ibo[0] - 1;                                                   \
    _ibo1   = _ibo[1] - 1;                                                   \
    _ibo2   = _ibo[2] - 1;                                                   \
    _ibo3   = _ibo[3] - 1;                                                   \
    _ibo4   = _ibo[4] - 1;                                                   \
    _ibo5   = _ibo[5] - 1;                                                   \
    _ibo6   = _ibo[6] - 1;                                                   \
    _ibo7   = _ibo[7] - 1;                                                   \
    _obo0  = _obo[0] - 1;                                                    \
    _obo1  = _obo[1] - 1;                                                    \
    _obo2  = _obo[2] - 1;                                                    \
    _obo3  = _obo[3] - 1;                                                    \
    _obo4  = _obo[4] - 1;                                                    \
    _obo5  = _obo[5] - 1;                                                    \
    _obo6  = _obo[6] - 1;                                                    \
    _obo7  = _obo[7] - 1;                                                    \
    for (_i = 0; _i < _ni; _i++, _obf += _bpi, _ibf += _bpi)                 \
        {_obf[_obo0]  = _ibf[_ibo0];                                         \
	 _obf[_obo1]  = _ibf[_ibo1];                                         \
	 _obf[_obo2]  = _ibf[_ibo2];                                         \
	 _obf[_obo3]  = _ibf[_ibo3];                                         \
	 _obf[_obo4]  = _ibf[_ibo4];                                         \
	 _obf[_obo5]  = _ibf[_ibo5];                                         \
	 _obf[_obo6]  = _ibf[_ibo6];                                         \
	 _obf[_obo7]  = _ibf[_ibo7];};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* XFER16 - transfer _NI items at _BPI bytes per item from _IBF to _OBF
 *        - rearranging the byte order according to _IBO and _OBO
 */

#define XFER16(_obf, _ni, _ibf, _ibo, _obo, _bpi)                            \
   {int _i;                                                                  \
    int _ibo0, _ibo1, _ibo2, _ibo3, _ibo4, _ibo5, _ibo6, _ibo7;              \
    int _ibo8, _ibo9, _ibo10, _ibo11, _ibo12, _ibo13, _ibo14, _ibo15;        \
    int _obo0, _obo1, _obo2, _obo3, _obo4, _obo5, _obo6, _obo7;              \
    int _obo8, _obo9, _obo10, _obo11, _obo12, _obo13, _obo14, _obo15;        \
    _ibo0   = _ibo[0] - 1;                                                   \
    _ibo1   = _ibo[1] - 1;                                                   \
    _ibo2   = _ibo[2] - 1;                                                   \
    _ibo3   = _ibo[3] - 1;                                                   \
    _ibo4   = _ibo[4] - 1;                                                   \
    _ibo5   = _ibo[5] - 1;                                                   \
    _ibo6   = _ibo[6] - 1;                                                   \
    _ibo7   = _ibo[7] - 1;                                                   \
    _ibo8   = _ibo[8] - 1;                                                   \
    _ibo9   = _ibo[9] - 1;                                                   \
    _ibo10  = _ibo[10] - 1;                                                  \
    _ibo11  = _ibo[11] - 1;                                                  \
    _ibo12  = _ibo[12] - 1;                                                  \
    _ibo13  = _ibo[13] - 1;                                                  \
    _ibo14  = _ibo[14] - 1;                                                  \
    _ibo15  = _ibo[15] - 1;                                                  \
    _obo0  = _obo[0] - 1;                                                    \
    _obo1  = _obo[1] - 1;                                                    \
    _obo2  = _obo[2] - 1;                                                    \
    _obo3  = _obo[3] - 1;                                                    \
    _obo4  = _obo[4] - 1;                                                    \
    _obo5  = _obo[5] - 1;                                                    \
    _obo6  = _obo[6] - 1;                                                    \
    _obo7  = _obo[7] - 1;                                                    \
    _obo8  = _obo[8] - 1;                                                    \
    _obo9  = _obo[9] - 1;                                                    \
    _obo10 = _obo[10] - 1;                                                   \
    _obo11 = _obo[11] - 1;                                                   \
    _obo12 = _obo[12] - 1;                                                   \
    _obo13 = _obo[13] - 1;                                                   \
    _obo14 = _obo[14] - 1;                                                   \
    _obo15 = _obo[15] - 1;                                                   \
    for (_i = 0; _i < _ni; _i++, _obf += _bpi, _ibf += _bpi)                 \
        {_obf[_obo0]  = _ibf[_ibo0];                                         \
	 _obf[_obo1]  = _ibf[_ibo1];                                         \
	 _obf[_obo2]  = _ibf[_ibo2];                                         \
	 _obf[_obo3]  = _ibf[_ibo3];                                         \
	 _obf[_obo4]  = _ibf[_ibo4];                                         \
	 _obf[_obo5]  = _ibf[_ibo5];                                         \
	 _obf[_obo6]  = _ibf[_ibo6];                                         \
	 _obf[_obo7]  = _ibf[_ibo7];                                         \
	 _obf[_obo8]  = _ibf[_ibo8];                                         \
	 _obf[_obo9]  = _ibf[_ibo9];                                         \
	 _obf[_obo10] = _ibf[_ibo10];                                        \
	 _obf[_obo11] = _ibf[_ibo11];                                        \
	 _obf[_obo12] = _ibf[_ibo12];                                        \
	 _obf[_obo13] = _ibf[_ibo13];                                        \
	 _obf[_obo14] = _ibf[_ibo14];                                        \
	 _obf[_obo15] = _ibf[_ibo15];};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FCONVERT - general floating point conversion routine
 *              - convert from floating point format specified by infor
 *              - to format specified by outfor
 *              -
 *              - floating point format specification:
 *              -
 *              -   format[0] = # of bits per number
 *              -   format[1] = # of bits in exponent
 *              -   format[2] = # of bits in mantissa
 *              -   format[3] = start bit of sign
 *              -   format[4] = start bit of exponent
 *              -   format[5] = start bit of mantissa
 *              -   format[6] = high order mantissa bit (CRAY needs this)
 *              -   format[7] = bias of exponent
 */

void _PD_fconvert(char **out, char **in, long nitems, int boffs,
		  long *infor, int *inord, long *outfor, int *outord,
		  PD_byte_order l_order, int l_bytes, int onescmp)
   {long i, j, expn_max, hexpn, DeltaBias, hmbo, hmbi;
    int nbits, inbytes, outbytes;
    int indxout, inrem, outrem, dindx;
    int bi_sign, bo_sign, bi_exp, bo_exp, bi_mant, bo_mant;
    int bo_sign_save, bo_exp_save, bo_mant_save;
    int nbi_exp, nbo_exp, nbi_man, nbo_man, nbi, nbo;
    char *lout, *lin;
    char *aligned, *insave, *inrsave;
   
    unsigned char *rout;

    long start, end, count, sign_byte, exp_byte;
    long hman_byte, in_man_byte, out_man_byte;
    int in_man_bit, out_man_bit, nbitsout;
    int sign_bit, exp_bit, hman_bit, rshift;
    int numleft, nb_mant_rem, remainder, k, nleft;
    unsigned char *out_mbptr, *in_mbptr, *outptr, *inptr;
    char mask, mask1, mask2, cmask;
    unsigned char *sbptr, *ebptr, *ebptrsave, *hman_bptr;
    long loverflow, lunderflow, lreorder, lreformat;
    long out_freorder;
    char *bufin, *btemp, *ctemp;
    char *freorder;

    long expn_save[MBLOCKS];
    int sign_save[MBLOCKS];
    char inreorder[MBLOCKS*MBYTES];
    long tformat[8];

    aligned      = NULL;
    freorder     = NULL;
    out_freorder = FALSE;
    insave       = NULL;
    inrsave      = NULL;

    nbi      = infor[0];
    nbo      = outfor[0];
    inbytes  = (nbi + 7) >> 3;
    outbytes = (nbo + 7) >> 3;

    if ((nbi % 8 != 0) || boffs)
       {aligned = SC_alloc_nzt(inbytes*nitems, 1L,
			       "_PD_FCONVERT:aligned", NULL);
        _PD_byte_align(aligned, *in, nitems, infor, inord, boffs);
        insave = *in;
        *in = aligned;};

/* check for cases where only difference between formats is in byte order */
    lreformat = FALSE;
    for (i = 0; i < 8; i++)
        {if (infor[i] != outfor[i])
            {lreformat = TRUE;
             break;};}

    if (lreformat == FALSE)

/* check to see if input byte order is different than output byte order */
       {lreorder = FALSE;
        for (i = 0; i < inbytes; i++)
            {if (inord[i] != outord[i])
                {lreorder = TRUE;
                 break;};}

/* reorder bytes and return if only difference is in byte order */
        if (lreorder == TRUE)
           {lin  = *in;
            lout = *out;

/* allow for compiler to do loop unrolling */
            switch (inbytes)
	       {case 1:
		     break;
		case 2:
		     XFER2(lout, nitems, lin, inord, outord, inbytes);
		     break;
		case 4:
		     XFER4(lout, nitems, lin, inord, outord, inbytes);
		     break;
		case 8:
		     XFER8(lout, nitems, lin, inord, outord, inbytes);
		     break;
		case 16:
		     XFER16(lout, nitems, lin, inord, outord, inbytes);
		     break;
		default:
		     for (j = 0; j < nitems; j++, lin += inbytes, lout += inbytes)
		         {for (i = 0; i < inbytes; i++)
			      {lout[outord[i] - 1] = lin[inord[i] - 1];};};};

            *in  += nitems*inbytes;
            *out += nitems*outbytes;

            return;};}   /* RETURN */

    nbi_exp = infor[1];
    nbo_exp = outfor[1];
    nbi_man = infor[2];
    nbo_man = outfor[2];
    bi_sign = infor[3];
    bo_sign = outfor[3];
    bi_exp  = infor[4];
    bo_exp  = outfor[4];
    bi_mant = infor[5];
    bo_mant = outfor[5];

    hman_byte = 0;

    hmbo    = (outfor[6] & 1L);
    hmbi    = (infor[6] & 1L);

    DeltaBias = outfor[7] + hmbo - infor[7] - hmbi;
    hexpn     = 1L << (outfor[1] - 1L);
    expn_max  = (1L << outfor[1]) - 1L;

/* the floating point conversion algorithm assumes that the sign bit 
 * comes before the exponent field, which comes before the mantissa.
 * if the input format deviates from this order the input is first
 * reordered into another buffer
 * if the output format deviates then the conversion is done as if
 * the fields are in the assumed order, then the output fields are
 * reordered into the desired order after going through the conversion
 * logic
 */

    if (!((infor[3] < infor[4]) && (infor[4] < infor[5])))
       {freorder   = SC_alloc_nzt(inbytes*nitems, 1L,
				  "_PD_FCONVERT:freorder", NULL);
        tformat[0]    = infor[0];
        tformat[1]    = infor[1];
        tformat[2]    = infor[2];
        tformat[3]    = bi_sign = 0L;
        tformat[4]    = bi_exp  = 1L;
        tformat[5]    = bi_mant = tformat[4] + tformat[1];
        _PD_field_reorder(*in, freorder, infor, tformat, inord,
                          l_order, l_bytes, nitems);
        inrsave       = *in;
        *in           = freorder;}

    if (!((outfor[3] < outfor[4]) && (outfor[4] < outfor[5])))
       {out_freorder = TRUE;
        bo_sign = bo_sign_save = 0L;
        bo_exp  = bo_exp_save  = 1L;
        bo_mant = bo_mant_save = bo_exp + outfor[1];}

/* check to see if input needs reordering */
    lreorder = FALSE;
    for (i = 0; i < inbytes; i++)
        {if (inord[i] != (i+1))
            {lreorder = 1;
             break;};}

/* if input needs reordering, reorder into a buffer */
    lout = *out;
    if (lreorder == TRUE)
       lin = inreorder;
    else
       lin = *in;

/* zero out the output buffer */
    memset(*out, 0, nitems*outbytes);

    start = 0;
    end   = min(nitems, MBLOCKS);
    count = 0;  /* count of items processed */

/* set up sign bit byte index and bit index */
    sign_byte = bi_sign >> 3;
    sign_bit  = bi_sign % 8;

/* set up exponent location variables */
    exp_byte = bi_exp >> 3;
    exp_bit  = bi_exp % 8;

/* set up mantissa location variables */
    in_man_byte = bi_mant >> 3;
    in_man_bit  = bi_mant % 8;

    out_man_byte = bo_mant >> 3;
    out_man_bit  = bo_mant % 8;

    dindx = hmbo - hmbi;

/* if input high mantissa bit (HMB) is assumed 1 and not written (e.g. IEEE)
 * but output HMB is assumed 0 (e.g. CRAY) write the input starting at
 * the output HMB+1 and set the HMB
 */
    if (dindx > 0)
       {hman_bit    = out_man_bit;
        hman_byte   = out_man_byte;
        out_man_bit = (out_man_bit + dindx) % 8;
        if (out_man_bit == 0)
	   out_man_byte++;} 

/* if input HMB is assumed 0 (e.g. CRAY) 
 * but output HMB is assumed 1 and not written (e.g. IEEE) take the input
 * from HMB+1 and write it to output HMB
 */
    else if (dindx < 0)
       {in_man_bit = (in_man_bit - dindx) % 8;
        if (in_man_bit == 0)
	   in_man_byte++;}

   while (count < nitems)

/* reorder the input into a buffer */
      {if (lreorder)
	  {bufin = *in + (count * inbytes) - 1;
	   for (i = 0; i < inbytes; i++)
	       {btemp = &bufin[inord[i]];
		ctemp = lin + i;
		for (j = start; j < end; j++, btemp += inbytes, ctemp += inbytes)
		    *ctemp = *btemp;};};
     
       ebptr     = (unsigned char *) lin + exp_byte + (!lreorder)*(count*inbytes);
       sbptr     = (unsigned char *) lin + sign_byte + (!lreorder)*(count*inbytes);
       in_mbptr  = (unsigned char *) lin + in_man_byte + (!lreorder)*(count*inbytes);
       out_mbptr = (unsigned char *) lout + out_man_byte + (count*outbytes);
       hman_bptr = (unsigned char *) lout + hman_byte + (count*outbytes);

/* get the sign bit, saving it in sign_save */       
       mask = 1 << (7 - sign_bit);
       for (i = start; i < end; i++, sbptr += inbytes)
	   sign_save[i] = *sbptr & mask;

       inrem  = nbi_exp;
       nbits  = 8 - exp_bit;
       mask   = (1 << nbits) - 1;
       rshift = 0;
       if (inrem < 7)
	  {mask = ((unsigned char)mask >> (7 - inrem));
	   rshift = 7 - inrem;}

       memset(expn_save, 0, (end-start)*sizeof(long));

       ebptrsave = ebptr;

       while (inrem > 0)

/* get the exponent */ 
	  {for (i = start; i < end; i++, ebptr += inbytes )
	       {expn_save[i] = (expn_save[i] << nbits) |
		               ((*ebptr >> rshift) & mask);};

	   inrem    -= nbits;
	   nbits     = min(8, inrem);
	   rshift    = 8 - nbits;
	   mask      = (1 << nbits) - 1;
	   ebptr     = ebptrsave + 1;
	   ebptrsave = ebptr;};

       if (onescmp)
	  {for (i = start; i < end; i++)
	       {if (sign_save[i])
		   {ONES_COMP_NEG(expn_save[i], nbi_exp, 1L);}
	        else
		   expn_save[i] += (expn_save[i] < hexpn);};};

/* add the bias and store the exponent */
       loverflow  = FALSE;
       lunderflow = FALSE;
       for (i = start; i < end; i++, bo_exp  += nbo, bo_sign += nbo)
	   {if (expn_save[i] != 0)
	       expn_save[i] += DeltaBias;

	    if ((0 < expn_save[i]) && (expn_save[i] < expn_max))
	       {_PD_insert_field(expn_save[i], nbo_exp, lout, bo_exp,
                                    l_order, l_bytes);
		if (sign_save[i]) 
		   _PD_set_bit(lout, bo_sign);}

	    else if (expn_max <= expn_save[i])
	       {loverflow = TRUE;
		_PD_insert_field(expn_max, nbo_exp, lout, bo_exp,
                                    l_order, l_bytes);
		if (sign_save[i])
		   _PD_set_bit(lout, bo_sign);}

	    else
	       lunderflow = TRUE;};

/* if input high mantissa bit (HMB) is assumed 1 and not written (e.g. IEEE)
 * but output HMB is assumed 0 (e.g. CRAY) write the input starting at
 * the output HMB+1 and set the HMB
 */
       if (dindx > 0)
	  {mask   = 1 << (7 - hman_bit);
	   outptr = hman_bptr;

	   if (loverflow || lunderflow)
	      {for (i = start; i < end; i++, outptr += outbytes)
		   {if ((0 < expn_save[i]) && (expn_save[i] < expn_max))
		       *outptr |= mask;};}
	   else
	      {for (i = start; i < end; i++, outptr += outbytes)
		   *outptr |= mask;};};

       nbitsout = 8 - out_man_bit;
       mask     = (1 << nbitsout) - 1;
       outptr   = out_mbptr;
       inptr    = in_mbptr;
       inrem    = nbi_man;
       outrem   = nbo_man;
       if (dindx > 0)
	  outrem -= dindx;
       else if (dindx < 0)
	  inrem += dindx;

/* will the chunk of in mantissa in first byte fit in the first byte
 * of out mantissa?
 */ 
       if (in_man_bit >= out_man_bit)

/* it will fit - store it */
	  {if (loverflow || lunderflow)
	      {if (onescmp)
		  {cmask = (1 << (8 - in_man_bit)) - 1;
		   for (i = start; i < end; i++, outptr += outbytes, inptr += inbytes)
		       {if ((0 < expn_save[i]) && (expn_save[i] < expn_max))
			   {if (sign_save[i])
			       *outptr |=  (~(*inptr & cmask) << (in_man_bit - out_man_bit))  & mask;
			    else
			       *outptr |=  (*inptr << (in_man_bit - out_man_bit))  & mask;};};}
	       else
		  {for (i = start; i < end; i++, outptr += outbytes, inptr += inbytes)
		       {if ((0 < expn_save[i]) && (expn_save[i] < expn_max))
			   *outptr |= (*inptr << (in_man_bit - out_man_bit))  & mask;};};}

/* no overflow */
	  else
	     {if (onescmp)
		 {cmask = (1 << (8 - in_man_bit)) - 1;
		  for (i = start; i < end; i++, outptr += outbytes, inptr  += inbytes)
		      {if (sign_save[i])
			  *outptr |=  (~(*inptr & cmask) << (in_man_bit - out_man_bit))  & mask;
		       else
			  *outptr |=  (*inptr << (in_man_bit - out_man_bit))  & mask;};}
	      else
		 {for (i = start; i < end; i++, outptr += outbytes, inptr  += inbytes)
		      *outptr |= (*inptr << (in_man_bit - out_man_bit))  & mask;};};
 
/* fill in the rest of the first output byte with the first
 * part of the second input byte
 */
	   numleft   = in_man_bit - out_man_bit;
	   remainder = 0;
	   if (numleft > 0)
	      {mask      = (1 << numleft) - 1;
	       inptr     = ++in_mbptr;
	       outptr    = out_mbptr;
	       remainder = 8 - numleft;
	       if (loverflow || lunderflow)
		  {if (onescmp)
		      {for (i = start; i < end; i++, outptr += outbytes, inptr  += inbytes)
			   {if ((0 < expn_save[i]) && (expn_save[i] < expn_max))
			       {if (sign_save[i])
				   *outptr |= ~(*inptr >> remainder) & mask;
			        else
				   *outptr |=  (*inptr >> remainder) & mask;};};}
		   else
		      {for (i = start; i < end; i++, outptr += outbytes, inptr  += inbytes)
			   {if ((0 < expn_save[i]) && (expn_save[i] < expn_max))
			       *outptr |= (*inptr >> remainder) & mask;};};}
	       else 
		  {if (onescmp)
		      {for (i = start; i < end; i++, outptr += outbytes, inptr  += inbytes)
			   {if (sign_save[i])
			       *outptr |= ~(*inptr >> remainder) & mask;
			    else
			       *outptr |=  (*inptr >> remainder) & mask;};}
		   else
		      {for (i = start; i < end; i++, outptr += outbytes, inptr  += inbytes)
			   *outptr |= (*inptr >> remainder) & mask;};};};}

/* in_man_bit < out_man_bit */
       else

/* no it won't fit */
	  {remainder = out_man_bit - in_man_bit; 
	   if (loverflow || lunderflow)
	      {if (onescmp)
		  {for (i = start; i < end; i++, outptr += outbytes, inptr  += inbytes)
		       {if ((0 < expn_save[i]) && (expn_save[i] < expn_max))
			   {if (sign_save[i])
			       *outptr |= ~(*inptr >> remainder)  & mask;
			    else
			       *outptr |=  (*inptr >> remainder)  & mask;};};}
	       else
		  {for (i = start; i < end; i++, outptr += outbytes, inptr  += inbytes)
		       {if ((0 < expn_save[i]) && (expn_save[i] < expn_max))
			   *outptr |= (*inptr >> remainder)  & mask;};};}       
	   else
	      {if (onescmp)
		  {for (i = start; i < end; i++, outptr += outbytes, inptr  += inbytes)
		       {if (sign_save[i])
			   *outptr |= ~(*inptr >> remainder)  & mask;
		        else
			   *outptr |=  (*inptr >> remainder)  & mask;};}
	       else
		  {for (i = start; i < end; i++, outptr += outbytes, inptr  += inbytes)
		       *outptr |= (*inptr >> remainder)  & mask;};};};

       inrem  -= nbitsout;
       outrem -= nbitsout;          
       outptr  = ++out_mbptr; /* begin storing to the beginning of the next */
                              /* sink byte.                                 */
       inptr   = in_mbptr;    /* from the byte with the remainder. */

/* how many full bytes of mantissa left? */
       nb_mant_rem = inrem >> 3;
       nb_mant_rem = min(nb_mant_rem, outrem >> 3);
       mask1 = (1 << remainder) -1;
       mask2 = (1 << (8 - remainder)) -1;
       
       for (k = 0; k < nb_mant_rem; k++)
	   {outptr = out_mbptr + k;
	    inptr  = in_mbptr  + k;
	    if (loverflow || lunderflow)
	       {if (onescmp)

/* move the mantissa over bytewise */
		   {for (i = start; i < end; i++, outptr += outbytes, inptr  += inbytes)
		        {if ((0 < expn_save[i]) && (expn_save[i] < expn_max))
			    {if (sign_save[i])
			        *outptr = ((~(*inptr) & mask1) << (8 - remainder)) |
				          (~(*(inptr + 1) >> remainder) & mask2);
			     else
			        *outptr = ((*inptr & mask1) << (8 - remainder)) |
				          ((*(inptr + 1) >> remainder) & mask2);};};}

/* move the mantissa over bytewise */
	        else
		   {for (i = start; i < end; i++, outptr += outbytes, inptr  += inbytes)
		        {if ((0 < expn_save[i]) && (expn_save[i] < expn_max))
			    *outptr = ((*inptr & mask1) << (8 - remainder)) |
			              ((*(inptr + 1) >> remainder) & mask2);};};}
	    else
	       {if (onescmp)

/* move the mantissa over bytewise */
		   {for (i = start; i < end; i++, outptr += outbytes, inptr  += inbytes)
		        {if (sign_save[i])
			    *outptr = ((~(*inptr) & mask1) << (8 - remainder)) |
			              (~(*(inptr + 1) >> remainder) & mask2);
			 else
			    *outptr = (((*inptr) & mask1) << (8 - remainder)) |
			              ((*(inptr + 1) >> remainder) & mask2);};}
	        else

/* move the mantissa over bytewise */
		   {for (i = start; i < end; i++, outptr += outbytes, inptr  += inbytes)
		        {*outptr = ((*inptr & mask1) << (8 - remainder)) |
			           ((*(inptr + 1) >> remainder) & mask2);};};};}

       out_mbptr += nb_mant_rem;
       in_mbptr  += nb_mant_rem;
       nbitsout   = (nb_mant_rem << 3);
       inrem     -= nbitsout;
       outrem    -= nbitsout;

/* store the last bits */
       nleft = min(inrem, outrem);
       if (nleft)
	  {mask   = ((1 << nleft) - 1) << (8 - nleft);
	   outptr = out_mbptr;
	   inptr  = in_mbptr;
	   if (loverflow || lunderflow)
	      {if (onescmp)
		  {for (i = start; i < end; i++, outptr += outbytes, inptr  += inbytes)
		       {if ((0 < expn_save[i]) && (expn_save[i] < expn_max))
			   {if (sign_save[i])
			       *outptr = ~(*inptr << (8 - remainder)) & mask;
			    else
			       *outptr =  (*inptr << (8 - remainder)) & mask;};};}
	       else
		  {for (i = start; i < end; i++, outptr += outbytes, inptr  += inbytes)
		       {if ((0 < expn_save[i]) && (expn_save[i] < expn_max))
			   *outptr = (*inptr << (8 - remainder)) & mask;};};}
	   else
	      {if (onescmp)
		  {for (i = start; i < end; i++, outptr += outbytes, inptr  += inbytes)
		       {if (sign_save[i])
			   *outptr = ~(*inptr << (8 - remainder)) & mask;
		        else
			   *outptr = (*inptr << (8 - remainder)) & mask;};}
	       else
		  {for (i = start; i < end; i++, outptr += outbytes, inptr  += inbytes)
		       *outptr = (*inptr << (8 - remainder)) & mask;};};};

       count += end - start;
       end    = min(nitems-count, MBLOCKS);}; /* end while (count < nitems) */

/* handle CRAY inconsistency which has zero as the only floating point
 * number with a 0 in the HMB
 * also problem for IEEE 96 bit float - fixed by Dave Munro
 */
    if (hmbo)
       {int j, mask;

	mask = (1 << (7 - bo_mant % 8));

        indxout = outfor[5]/8;
        rout    = (unsigned char *) *out;
        for (i = 0L; i < nitems; i++, rout += outbytes)
            {for (j = 0; j < outbytes; j++)
                 if ((j == indxout) ? (rout[j] != mask) : rout[j])
                    break;
             if (j == outbytes)
                rout[indxout] = 0;};};

    if (*in == freorder)
       {SFREE(freorder);
        *in = inrsave;}

    if (*in == aligned)
       {SFREE(aligned);
        *in = insave;}

    if (out_freorder)
       {freorder = SC_alloc_nzt(outbytes*nitems, 1L,
				"_PD_FCONVERT:freorder", NULL);
        tformat[0] = outfor[0];
        tformat[1] = outfor[1];
        tformat[2] = outfor[2];
        tformat[3] = bo_sign_save;
        tformat[4] = bo_exp_save;
        tformat[5] = bo_mant_save;
        _PD_field_reorder(*out, freorder, tformat, outfor, outord,
                          l_order, l_bytes, nitems);
        memcpy(*out, freorder, nitems*outbytes);
        SFREE(freorder);};


/* put the output bytes into the specified order */
    _PD_reorder(*out, nitems, outbytes, outord);

    *in  += nitems*inbytes;
    *out += nitems*outbytes;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CONVERT - convert primitive types from one machine format to
 *             - another.  Guaranteed that there will be no indirects here.
 *             - Convert NITEMS of type, TYPE, from IN and
 *             - put them in OUT
 *             - ISTD defines the data format of the data from IN
 *             - PIN_OFFS and POUT_OFFS are pointers to external offset
 *             - counters and they are incremented to account for
 *             - data alignment
 *             - returns TRUE iff successful
 */

static int _PD_convert(char **out, char **in, long nitems, int boffs,
		       defstr *idp, defstr *odp,
		       data_standard *istd, data_standard *ostd,
		       data_standard *hstd,
		       long *pin_offs, long *pout_offs)
   {int i, lnby, inbts, onescmp, usg;
    int iusg, ousg, ret, rdx;
    int *intord, *iaord, *oaord;
    long inb, onb, nbi, nbo;
    long *ifmt, *ofmt;
    char *inty, *outty, *delim;
    PD_type_kind iknd, oknd;
    PD_byte_order isord, osord, lsord, inord, outord;

    inty    = idp->type;
    outty   = odp->type;
    inbts   = idp->size_bits;
    iknd    = idp->kind;
    oknd    = odp->kind;
    inb     = idp->size;
    onb     = odp->size;
    iaord   = idp->order;
    oaord   = odp->order;
    isord   = idp->order_flag;
    osord   = odp->order_flag;
    ifmt    = idp->format;
    ofmt    = odp->format;
    onescmp = idp->onescmp;
    iusg    = idp->unsgned;
    ousg    = odp->unsgned;

    if ((strchr(inty, '*') != NULL) || (strchr(outty, '*') != NULL))
       return(FALSE);

    usg = (!iusg && !ousg) ? FALSE : TRUE;

    inord  = istd->long_order;
    outord = ostd->long_order;
    lsord  = hstd->long_order;
    lnby   = hstd->long_bytes;

    ret = TRUE;

/* sometimes this should be clear from the text (e.g. 0xfff)
 * sometimes it should be specified by the caller (e.g. 101 - binary, octal, ...?)
 */
    nbi = inb*nitems;
    nbo = onb*nitems;

    if (hstd->file != NULL)
       {rdx   = hstd->file->radix;
	delim = hstd->file->delim;}
    else
       {rdx   = 10;
	delim = NULL;};

/* text to binary conversions */
    if (inord == TEXT_ORDER)
       {_PD_text_bin(out, in, idp->type, nitems, boffs,
		     iknd, ifmt, inb, isord, iaord,
		     oknd, ofmt, onb, osord, oaord,
		     hstd, onescmp, usg,
		     rdx, delim);

        *pin_offs  += nbi;
        *pout_offs += nbo;}

/* binary to text conversions */
    else if (outord == TEXT_ORDER)
       {_PD_bin_text(out, in, idp->type, nitems, boffs,
		     iknd, ifmt, inb, isord, iaord,
		     oknd, ofmt, onb, osord, oaord,
		     hstd, onescmp, usg,
		     rdx, delim);

        *pin_offs  += nbi;
        *pout_offs += nbo;}

/* if direct types are the same and non-converting just copy them over */
    else if ((strcmp(odp->type, idp->type) == 0) &&
	(odp->convert == 0) && (idp->convert == 0) &&
	(nbi == nbo))
       {_PD_ncopy(out, in, nitems, inb);

        *pin_offs  += nbi;
        *pout_offs += nbo;}

/* handle floating point conversions */
    else if ((ifmt != NULL) && (ofmt != NULL))
       {_PD_fconvert(out, in, nitems, boffs, ifmt, iaord,
                     ofmt, oaord, lsord, lnby, onescmp);

        *pin_offs  += nbi;
        *pout_offs += nbo;}

/* handle integral bitstreams */
    else if (inbts > 0)
       {int ityp;

	if (onb == sizeof(char))
	   ityp = SC_CHAR_I;

	else if (onb == sizeof(long))
	   ityp = SC_LONG_I;

	else if (onb == sizeof(short))
	   ityp = SC_SHORT_I;

	else if (onb == sizeof(int))
	   ityp = SC_INTEGER_I;

	else
	   return(FALSE);

/* unpack the bitstream into a bytestream */
	ret = SC_unpack_bits(*out, *in, ityp, inbts,
			     0, nitems, nitems, boffs);

/* convert binary characters */
        if (strcmp(inty, SC_CHAR_S) == 0)
	   _PD_convert_ascii(*out, *in, nitems, inbts, 0);

/* convert integers */
        else
	   {intord = (int *) SC_alloc_nzt(onb, sizeof(int),
					  "_PD_CONVERT:intord",
					  NULL);
            if (osord == NORMAL_ORDER)
               for (i = 0; i < onb; intord[i] = i + 1, i++);
            else                         
               for (i = 0; i < onb; intord[i] = onb - i, i++);

            if (usg == FALSE)
      	        _PD_sign_extend(*out, nitems, onb, inbts, intord);

            if (onescmp)
               _PD_ones_complement(*out, nitems, onb, intord);

            SFREE(intord);

	    *in        += nbi;
	    *out       += nbo;
	    *pin_offs  += nbi;
	    *pout_offs += nbo;};}

/* handle integer conversions */
    else if ((isord != NO_ORDER) && (osord != NO_ORDER))
       {_PD_iconvert(out, in, nitems,
                     (int) inb, isord,
                     (int) onb, osord, onescmp, usg);

        *pin_offs  += nbi;
        *pout_offs += nbo;}

/* handle character or unconverted types */
    else
       {_PD_ncopy(out, in, nitems, inb);

        *pin_offs  += nbi;
        *pout_offs += nbo;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_CONVERT - convert from one machine format to another NITEMS of
 *            - type, TYPE, from IN and put them in OUT
 *            - ISTD and OSTD are the data format standards of IN and 
 *            - OUT respectively
 */

int PD_convert(char **out, char **in, char *typi, char *typo,
	       long nitems, data_standard *stdi, data_standard *stdo,
	       data_standard *hstd, hasharr *chi, hasharr *cho,
	       int boffs, PD_major_op error)
   {int ret, tmp;
    long i, mitems, inci, inco;
    long in_offs, out_offs;
    char *mtype;
    defstr *dpi, *dpo, *mdpi, *mdpo;
    memdes *desc;

    ret      = 0;
    in_offs  = 0L;
    out_offs = 0L;

    if (_PD_indirection(typi) == TRUE)
       {dpi = PD_inquire_table_type(chi, "*");
	dpo = PD_inquire_table_type(cho, "*");}
    else
       {dpi = PD_inquire_table_type(chi, typi);
	dpo = PD_inquire_table_type(cho, typo);};

    if (dpi == NULL)
       PD_error("BAD TYPE IN_CHART - PD_CONVERT", error);

    if (dpo == NULL)
       PD_error("BAD TYPE OUT_CHART - PD_CONVERT", error);

/* convert pointers to data via /&ptrs mechanism */
    else if (dpo->is_indirect) 
       {inci = _PD_align(in_offs, typi, dpi->is_indirect, chi, &tmp);
        inco = _PD_align(out_offs, typo, dpo->is_indirect, cho, &tmp);

	ret = _PD_convert_ptr(out, in, &out_offs, &in_offs, nitems,
			      cho, chi, stdo, stdi, hstd, inco, inci);}

/* if members is non-NULL then it is a derived type */
    else if (dpo->members != NULL)
       {for (i = 0L; i < nitems; i++)
            {inci = _PD_align(in_offs, typi, dpi->is_indirect, chi, &tmp);
             inco = _PD_align(out_offs, typo, dpo->is_indirect, cho, &tmp);

	     PD_NEXT_LOC(in, inci);
	     PD_NEXT_LOC(out, inco);

             for (desc = dpo->members; desc != NULL; desc = desc->next)
                 {mitems = desc->number;
                  mtype  = desc->type;

                  inci = _PD_align(in_offs, mtype, desc->is_indirect, chi, &tmp);
                  inco = _PD_align(out_offs, mtype, desc->is_indirect, cho, &tmp);

/* increment the offsets to the alignments */
		  PD_NEXT_LOC(in, inci);
		  PD_NEXT_LOC(out, inco);

/* convert pointers to data via /&ptrs mechanism and
 * don't throw away information about whether or not pointer is null
 */
                  if (desc->is_indirect == TRUE)
		     ret = _PD_convert_ptr(out, in, &out_offs, &in_offs, 1L,
					   cho, chi, stdo, stdi, hstd, 0L, 0L);

/* convert function pointers by skipping over them and
 * don't throw away information about whether or not pointer is null
 */
                  else if (strcmp(mtype, "function") == 0)
		     ret = _PD_convert_fnc(out, in, &out_offs, &in_offs,
					   stdo->ptr_bytes, stdi->ptr_bytes);

/* check for direct primitives */
                  else if (_PD_prim_typep(desc->base_type, chi, error))
                     {mdpo = PD_inquire_table_type(cho, mtype);
                      if (mdpo == NULL)
                         PD_error("BAD OUT TYPE IN STRUCT - PD_CONVERT",
                                  error);

                      mdpi = PD_inquire_table_type(chi, mtype);
                      if (mdpi == NULL)
                         PD_error("BAD IN TYPE IN STRUCT - PD_CONVERT",
                                  error);

		      ret = _PD_convert((char **) out, in, mitems, boffs,
					mdpi, mdpo, stdi, stdo, hstd,
					&in_offs, &out_offs);}

/* recurse for direct derived types */
                  else
                     ret = PD_convert(out, in, mtype, mtype, mitems,
                                      stdi, stdo, hstd,
                                      chi, cho, boffs, error);

                  if (ret == FALSE)
                     PD_error("STRUCT CONVERSION FAILED - PD_CONVERT",
                              error);};};}

/* if members is NULL then it is a primitive type */
    else
       {ret = _PD_convert((char **) out, in, nitems, boffs, dpi, dpo,
                          stdi, stdo, hstd, &in_offs, &out_offs);
        if (ret == FALSE)
           PD_error("PRIMITIVE CONVERSION FAILED - PD_CONVERT",
                    error);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

