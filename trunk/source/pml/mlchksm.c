/*
 * MLCHKSM.C - checksum and related routines
 *           - after a Fortran implementation by Clifford E. Rhoades, Jr.
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

/* This notice pertains to each of the PM_md5_* and PM_MD5_* items below */

/* Copyright @ 1991-2, RSA Data Security, Inc. Created 1991. All
 * rights reserved.
 * 
 * License to copy and use this software is granted provided that it
 * is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 * Algorithm" in all material mentioning or referencing this software
 * or this function.
 * 
 * License is also granted to make and use derivative works provided
 * that such works are identified as "derived from the RSA Data
 * Security, Inc. MD5 Message-Digest Algorithm" in all material
 * mentioning or referencing the derived work.
 * 
 * RSA Data Security, Inc. makes no representations concerning either
 * the merchantability of this software or the suitability of this
 * software for any particular purpose. It is provided "as is"
 * without express or implied warranty of any kind.
 * 
 * These notices must be retained in any copies of any part of this
 * documentation and/or software.
 * 
 */

/* constants for MD5 Transform routine */
#define PM_MD5_S11 7
#define PM_MD5_S12 12
#define PM_MD5_S13 17
#define PM_MD5_S14 22
#define PM_MD5_S21 5
#define PM_MD5_S22 9
#define PM_MD5_S23 14
#define PM_MD5_S24 20
#define PM_MD5_S31 4
#define PM_MD5_S32 11
#define PM_MD5_S33 16
#define PM_MD5_S34 23
#define PM_MD5_S41 6
#define PM_MD5_S42 10
#define PM_MD5_S43 15
#define PM_MD5_S44 21

/* F, G, H and I are basic MD5 functions */

#define PM_MD5_F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define PM_MD5_G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define PM_MD5_H(x, y, z) ((x) ^ (y) ^ (z))
#define PM_MD5_I(x, y, z) ((y) ^ ((x) | (~z)))

/* PM_MD5_ROTATE_LEFT rotates x to the left by n bits */

#define PM_MD5_ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
 * Rotation is separate from addition to prevent recomputation.
 */

#define PM_MD5_FF(a, b, c, d, x, s, ac)                                        \
   {(a) += PM_MD5_F((b), (c), (d)) + (x) + (u_int32_t) (ac);                   \
    (a)  = PM_MD5_ROTATE_LEFT((a), (s));                                       \
    (a) += (b);}

#define PM_MD5_GG(a, b, c, d, x, s, ac)                                        \
   {(a) += PM_MD5_G((b), (c), (d)) + (x) + (u_int32_t) (ac);                   \
    (a)  = PM_MD5_ROTATE_LEFT((a), (s));                                       \
    (a) += (b);}

#define PM_MD5_HH(a, b, c, d, x, s, ac)                                        \
   {(a) += PM_MD5_H((b), (c), (d)) + (x) + (u_int32_t) (ac);                   \
    (a)  = PM_MD5_ROTATE_LEFT((a), (s));                                       \
    (a) += (b);}

#define PM_MD5_II(a, b, c, d, x, s, ac)                                        \
   {(a) += PM_MD5_I((b), (c), (d)) + (x) + (u_int32_t) (ac);                   \
    (a)  = PM_MD5_ROTATE_LEFT((a), (s));                                       \
    (a) += (b);}

#undef OVERFLOW
#define OVERFLOW(_i, _m, _s)  ((_i & _m) + (_i >> _s))


/* MD5 context */
typedef struct s_PM_MD5_CTX PM_MD5_CTX;

struct s_PM_MD5_CTX
   {u_int32_t state[4];                                       /* state (ABCD) */
    u_int32_t count[2];            /* number of bits, modulo 2^64 (lsb first) */
    unsigned char buffer[64];};                               /* input buffer */


/* MD5 padding */
static unsigned char
 PM_MD5_PADDING[64] = {0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0};

/* this stuff derives from detect.c which unfortunately comes later */
union ci
   {unsigned char c[32];
    int i[2];} bo;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_MD5_ENCODE - encode INPUT into OUTPUT
 *                - assume LEN is a multiple of 4
 */

static void _PM_md5_encode(unsigned char *out, u_int32_t *in,
			   unsigned int len) 
   {unsigned int i, j;

    for (i = 0, j = 0; j < len; i++, j += 4) 
        {out[j]   = (unsigned char) (in[i]         & 0xff);
	 out[j+1] = (unsigned char) ((in[i] >> 8)  & 0xff);
	 out[j+2] = (unsigned char) ((in[i] >> 16) & 0xff);
	 out[j+3] = (unsigned char) ((in[i] >> 24) & 0xff);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_MD5_DECODE - decode IN into OUT
 *                - assume LEN is a multiple of 4
 */

static void _PM_md5_decode(u_int32_t *out, unsigned char *in,
			   unsigned int len) 
   {unsigned int i, j;

    for (i = 0, j = 0; j < len; i++, j += 4)
        out[i] = ((u_int32_t) in[j])           |
	         (((u_int32_t) in[j+1]) << 8)  |
	         (((u_int32_t) in[j+2]) << 16) |
		 (((u_int32_t) in[j+3]) << 24);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_MD5_TRANSFORM - MD5 basic transformation
 *                   - transform state based on block
 */

static void _PM_md5_transform(u_int32_t state[4], unsigned char block[64]) 
   {u_int32_t a, b, c, d, x[16];

    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];

    _PM_md5_decode(x, block, 64);

/* round 1 */
    PM_MD5_FF(a, b, c, d, x[ 0], PM_MD5_S11, 0xd76aa478); /* 1 */
    PM_MD5_FF(d, a, b, c, x[ 1], PM_MD5_S12, 0xe8c7b756); /* 2 */
    PM_MD5_FF(c, d, a, b, x[ 2], PM_MD5_S13, 0x242070db); /* 3 */
    PM_MD5_FF(b, c, d, a, x[ 3], PM_MD5_S14, 0xc1bdceee); /* 4 */
    PM_MD5_FF(a, b, c, d, x[ 4], PM_MD5_S11, 0xf57c0faf); /* 5 */
    PM_MD5_FF(d, a, b, c, x[ 5], PM_MD5_S12, 0x4787c62a); /* 6 */
    PM_MD5_FF(c, d, a, b, x[ 6], PM_MD5_S13, 0xa8304613); /* 7 */
    PM_MD5_FF(b, c, d, a, x[ 7], PM_MD5_S14, 0xfd469501); /* 8 */
    PM_MD5_FF(a, b, c, d, x[ 8], PM_MD5_S11, 0x698098d8); /* 9 */
    PM_MD5_FF(d, a, b, c, x[ 9], PM_MD5_S12, 0x8b44f7af); /* 10 */
    PM_MD5_FF(c, d, a, b, x[10], PM_MD5_S13, 0xffff5bb1); /* 11 */
    PM_MD5_FF(b, c, d, a, x[11], PM_MD5_S14, 0x895cd7be); /* 12 */
    PM_MD5_FF(a, b, c, d, x[12], PM_MD5_S11, 0x6b901122); /* 13 */
    PM_MD5_FF(d, a, b, c, x[13], PM_MD5_S12, 0xfd987193); /* 14 */
    PM_MD5_FF(c, d, a, b, x[14], PM_MD5_S13, 0xa679438e); /* 15 */
    PM_MD5_FF(b, c, d, a, x[15], PM_MD5_S14, 0x49b40821); /* 16 */
  
/* round 2 */
    PM_MD5_GG(a, b, c, d, x[ 1], PM_MD5_S21, 0xf61e2562); /* 17 */
    PM_MD5_GG(d, a, b, c, x[ 6], PM_MD5_S22, 0xc040b340); /* 18 */
    PM_MD5_GG(c, d, a, b, x[11], PM_MD5_S23, 0x265e5a51); /* 19 */
    PM_MD5_GG(b, c, d, a, x[ 0], PM_MD5_S24, 0xe9b6c7aa); /* 20 */
    PM_MD5_GG(a, b, c, d, x[ 5], PM_MD5_S21, 0xd62f105d); /* 21 */
    PM_MD5_GG(d, a, b, c, x[10], PM_MD5_S22,  0x2441453); /* 22 */
    PM_MD5_GG(c, d, a, b, x[15], PM_MD5_S23, 0xd8a1e681); /* 23 */
    PM_MD5_GG(b, c, d, a, x[ 4], PM_MD5_S24, 0xe7d3fbc8); /* 24 */
    PM_MD5_GG(a, b, c, d, x[ 9], PM_MD5_S21, 0x21e1cde6); /* 25 */
    PM_MD5_GG(d, a, b, c, x[14], PM_MD5_S22, 0xc33707d6); /* 26 */
    PM_MD5_GG(c, d, a, b, x[ 3], PM_MD5_S23, 0xf4d50d87); /* 27 */
    PM_MD5_GG(b, c, d, a, x[ 8], PM_MD5_S24, 0x455a14ed); /* 28 */
    PM_MD5_GG(a, b, c, d, x[13], PM_MD5_S21, 0xa9e3e905); /* 29 */
    PM_MD5_GG(d, a, b, c, x[ 2], PM_MD5_S22, 0xfcefa3f8); /* 30 */
    PM_MD5_GG(c, d, a, b, x[ 7], PM_MD5_S23, 0x676f02d9); /* 31 */
    PM_MD5_GG(b, c, d, a, x[12], PM_MD5_S24, 0x8d2a4c8a); /* 32 */
  
/* round 3 */
    PM_MD5_HH(a, b, c, d, x[ 5], PM_MD5_S31, 0xfffa3942); /* 33 */
    PM_MD5_HH(d, a, b, c, x[ 8], PM_MD5_S32, 0x8771f681); /* 34 */
    PM_MD5_HH(c, d, a, b, x[11], PM_MD5_S33, 0x6d9d6122); /* 35 */
    PM_MD5_HH(b, c, d, a, x[14], PM_MD5_S34, 0xfde5380c); /* 36 */
    PM_MD5_HH(a, b, c, d, x[ 1], PM_MD5_S31, 0xa4beea44); /* 37 */
    PM_MD5_HH(d, a, b, c, x[ 4], PM_MD5_S32, 0x4bdecfa9); /* 38 */
    PM_MD5_HH(c, d, a, b, x[ 7], PM_MD5_S33, 0xf6bb4b60); /* 39 */
    PM_MD5_HH(b, c, d, a, x[10], PM_MD5_S34, 0xbebfbc70); /* 40 */
    PM_MD5_HH(a, b, c, d, x[13], PM_MD5_S31, 0x289b7ec6); /* 41 */
    PM_MD5_HH(d, a, b, c, x[ 0], PM_MD5_S32, 0xeaa127fa); /* 42 */
    PM_MD5_HH(c, d, a, b, x[ 3], PM_MD5_S33, 0xd4ef3085); /* 43 */
    PM_MD5_HH(b, c, d, a, x[ 6], PM_MD5_S34,  0x4881d05); /* 44 */
    PM_MD5_HH(a, b, c, d, x[ 9], PM_MD5_S31, 0xd9d4d039); /* 45 */
    PM_MD5_HH(d, a, b, c, x[12], PM_MD5_S32, 0xe6db99e5); /* 46 */
    PM_MD5_HH(c, d, a, b, x[15], PM_MD5_S33, 0x1fa27cf8); /* 47 */
    PM_MD5_HH(b, c, d, a, x[ 2], PM_MD5_S34, 0xc4ac5665); /* 48 */
  
/* round 4 */
    PM_MD5_II(a, b, c, d, x[ 0], PM_MD5_S41, 0xf4292244); /* 49 */
    PM_MD5_II(d, a, b, c, x[ 7], PM_MD5_S42, 0x432aff97); /* 50 */
    PM_MD5_II(c, d, a, b, x[14], PM_MD5_S43, 0xab9423a7); /* 51 */
    PM_MD5_II(b, c, d, a, x[ 5], PM_MD5_S44, 0xfc93a039); /* 52 */
    PM_MD5_II(a, b, c, d, x[12], PM_MD5_S41, 0x655b59c3); /* 53 */
    PM_MD5_II(d, a, b, c, x[ 3], PM_MD5_S42, 0x8f0ccc92); /* 54 */
    PM_MD5_II(c, d, a, b, x[10], PM_MD5_S43, 0xffeff47d); /* 55 */
    PM_MD5_II(b, c, d, a, x[ 1], PM_MD5_S44, 0x85845dd1); /* 56 */
    PM_MD5_II(a, b, c, d, x[ 8], PM_MD5_S41, 0x6fa87e4f); /* 57 */
    PM_MD5_II(d, a, b, c, x[15], PM_MD5_S42, 0xfe2ce6e0); /* 58 */
    PM_MD5_II(c, d, a, b, x[ 6], PM_MD5_S43, 0xa3014314); /* 59 */
    PM_MD5_II(b, c, d, a, x[13], PM_MD5_S44, 0x4e0811a1); /* 60 */
    PM_MD5_II(a, b, c, d, x[ 4], PM_MD5_S41, 0xf7537e82); /* 61 */
    PM_MD5_II(d, a, b, c, x[11], PM_MD5_S42, 0xbd3af235); /* 62 */
    PM_MD5_II(c, d, a, b, x[ 2], PM_MD5_S43, 0x2ad7d2bb); /* 63 */
    PM_MD5_II(b, c, d, a, x[ 9], PM_MD5_S44, 0xeb86d391); /* 64 */
  
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
  
/* zero out sensitive information */
    SC_MEM_INIT_V(x);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_MD5_INIT - MD5 initialization
 *              - begin an MD5 operation, writing a new context
 */

static void _PM_md5_init(PM_MD5_CTX *mc) 
   {

    mc->count[0] = mc->count[1] = 0;

/* load magic initialization constants */
    mc->state[0] = 0x67452301;
    mc->state[1] = 0xefcdab89;
    mc->state[2] = 0x98badcfe;
    mc->state[3] = 0x10325476;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_MD5_UPDATE - MD5 block update operation
 *                - continue an MD5 message-digest operation
 *                - processing another message block
 *                - and updating the context
 */

static void _PM_md5_update(PM_MD5_CTX *mc, unsigned char *in,
			   unsigned int inl) 
   {unsigned int i, index, pln;
    unsigned char *pin;

/* compute number of bytes mod 64 */
    index = (unsigned int) ((mc->count[0] >> 3) & 0x3F);

/* update number of bits */
    if ((mc->count[0] += ((u_int32_t) inl << 3)) < ((u_int32_t) inl << 3))
       mc->count[1]++;

    mc->count[1] += ((u_int32_t) inl >> 29);

    pln = 64 - index;

/* transform as many times as possible */
    if (inl >= pln) 
       {memcpy(&mc->buffer[index], in, (size_t) pln);
        _PM_md5_transform(mc->state, mc->buffer);

        for (i = pln; i + 63 < inl; i += 64)
	    {pin = in + i;
	     _PM_md5_transform(mc->state, pin);};

        index = 0;}

    else 
       i = 0;

/* buffer remaining input */
    memcpy(&mc->buffer[index], &in[i], (size_t) inl-i);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_MD5_FINAL - MD5 finalization
 *               - end an MD5 message-digest operation
 *               - writing the the message digest
 *               - and zeroing out the context
 */

static void _PM_md5_final(unsigned char *dig, PM_MD5_CTX *mc) 
   {int i;
    char s[MAXLINE];
    char *p;
    unsigned char bits[8];
    unsigned int index, pln;

/* save number of bits */
    _PM_md5_encode(bits, mc->count, 8);

/* pad out to 56 mod 64 */
    index = (unsigned int)((mc->count[0] >> 3) & 0x3f);
    pln   = (index < 56) ? (56 - index) : (120 - index);
    _PM_md5_update(mc, PM_MD5_PADDING, pln);

/* append length (before padding) */
    _PM_md5_update(mc, bits, 8);

/* store state in dig */
    _PM_md5_encode(dig, mc->state, 16);

    p = s;
    for (i = 0; i < 16; i++, p += 2)
        snprintf(p, MAXLINE - 2*i, "%02x", dig[i]);
    strcpy((char *) dig, s);

/* zero out sensitive information */
    memset(mc, 0, sizeof(*mc));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MD5_CHECKSUM_ARRAY - calculate an MD5 checksum on the contents of ARR
 *                       - for NI items that are BPI bytes/item
 *                       - places output in DIG in hex form
 *                       - DIG must be at least 33 bytes long (1 for the
 *                       - trailing termination byte
 */
 
void PM_md5_checksum_array(void *arr, uint64_t ni, uint64_t bpi,
			   unsigned char *dig)
   {int64_t sz, ln;
    uint64_t ib, nb;
    unsigned char *p;
    PM_MD5_CTX mc;
    
    nb = ni*bpi;

/* read the arr in 1024 byte (1 kB) chunks */
    sz = (int64_t) 1024;
    
#if 0
    {double t0, rate;
    t0 = SC_wall_clock_time();
#endif

    _PM_md5_init(&mc);

/* walk any multiples of SZ bytes */
    p = (unsigned char *) arr;
    for (ib = nb; ib > 0; )
        {ln = min(ib, sz);
	 _PM_md5_update(&mc, p, ln);
	 ib -= ln;
	 p  += ln;};

    _PM_md5_final(dig, &mc);
    
#if 0
    t0   = SC_wall_clock_time() - t0;
    rate = ((double) nb)/t0;};
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MD5_CHECKSUM_FILE - calculate an MD5 checksum on the contents of FILE
 *                      - from diskaddress START to STOP
 *                      - places output in DIG in hex form
 *                      - DIG must be at least 33 bytes long (1 for the
 *                      - trailing termination byte
 */

void PM_md5_checksum_file(FILE *file, int64_t start, int64_t stop,
			  unsigned char *dig) 
   {int len, seg, sz;
    int64_t here, nb, ib;
    unsigned char bf[1024];
    PM_MD5_CTX mc;

/* remember where we were in the file */
    here = io_tell(file);

    if (start < 0)
       start = 0;

    if (stop < 0)
       {io_seek(file, 0, SEEK_END);
	stop = io_tell(file);};
        
/* read the file in 1024 byte (1 kB) chunks */        
    sz = 1024;   
    nb = (stop + 1) - start;

/* jump to the starting point */
    io_seek(file, start, SEEK_SET);

#if 0
    {double t0, rate;
    t0 = SC_wall_clock_time();
#endif

    _PM_md5_init(&mc);

    for (ib = nb; ib > 0; )
        {seg = min(ib, sz);
	 len = io_read(bf, 1, seg, file);  
	 if (len == seg)
	    {_PM_md5_update(&mc, bf, len);
	     ib -= len;}
	 else
	    break;};

    _PM_md5_final(dig, &mc); 

#if 0
    t0   = SC_wall_clock_time() - t0;
    rate = ((double) nb)/t0;};
#endif

/* return to where we were in the file */
    io_seek(file, here, SEEK_SET);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CHECKSUM - compute a checksum on an array PARR which is NI items long
 *             - and BPI bytes per item
 *             - BOFF is a bit offset for the first item
 *             - NBTS is the number of bits for the checksum units
 *             - 29 is a good choice for NBTS
 *             - for applications such as computing a checksum for
 *             - a file more than one call can be made:
 *             -   
 *             -   sum  = 0;
 *             -   rmdr = 0;
 *             -   while (TRUE)
 *             -      {PM_checksum(&sum, &rmdr, in, boff, ni, bpi, 29);
 *             -        ... get next in, boff, ni, and bpi ...
 *             -      };
 */

void PM_checksum(long *psum, int *prmdr, void *parr, int boff,
		 int ni, int bpi, int nbts)
   {int i, l, nf, rmdr, sbi, ln;
    int ord[64];
    long is, sum, nby, nbi, msk;
    char *in;

/* this stuff is derived from detect.c which unfortunately
 * comes later
 */
    bo.i[0] = 1;
    if (bo.c[0] == 1)
       {for (i = 0 ; i < bpi; i++)
            ord[i] = bpi - i;}
    else
       {for (i = 0 ; i < bpi; i++)
            ord[i] = i + 1;};

    sum  = *psum;
    rmdr = *prmdr;

    nby = ni*bpi;
    nbi = 8*nby;
    msk = (1L << nbts) - 1L;
    in  = (char *) parr;

    ln = min(rmdr, nbi);
    is = SC_extract_field(in, boff, ln, bpi, ord);

    ln = nbi - rmdr;
    if (ln > 0)
       {nf  = ln/nbts + 1;
	sbi = boff + rmdr;

	for (l = 0; l < nf; l++, sbi += nbts, ln -= nbts)
	    {is += SC_extract_field(in, sbi, min(ln, nbts), bpi, ord);
	     is  = OVERFLOW(is, msk, nbts);};};

    sum   += is;
    *psum  = OVERFLOW(sum, msk, nbts);
    *prmdr = abs(ln) % nbts;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CHECKSUM_ARRAY - compute a checksum on an array ARR
 *                   - which is NI items long
 *                   - and BPI bytes per item
 *                   - NBTS is the number of bits for the checksum units
 *                   - 29 is a good choice for NBTS
 */

long PM_checksum_array(void *arr, int ni, int bpi, int nbts)
   {int rmdr;
    long sum;

    sum  = 0L;
    rmdr = 0;
    PM_checksum(&sum, &rmdr, arr, 0, ni, bpi, nbts);

    return(sum);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CHECKSUM_FILE - compute a checksum on an array ARR
 *                  - which is NI items long
 *                  - and BPI bytes per item
 *                  - NBTS is the number of bits for the checksum units
 *                  - 29 is a good choice for NBTS
 */

long PM_checksum_file(FILE* file, int64_t start, int64_t stop, int nbts)
   {int len, seg, sz, rmdr;
    long sum;
    int64_t here, nb;
    unsigned char bf[1024];

/* Sanity checks */
    if (file == NULL)
      return(-1);

/* remember where we were in the file */
    here = io_tell(file);

    if (start < 0)
       start = 0;

    if (stop < 0)
       {io_seek(file, 0, SEEK_END);
	stop = io_tell(file);};
       
    nb = stop - start + 1;

/* jump to the starting point */
    io_seek(file, start, SEEK_SET);

/* read the file in SZ chunks */
    sum  = 0L;
    rmdr = 0;
    sz   = 1024;
    while (nb > 0)
       {seg = min(nb, sz);
	len = io_read(bf, 1, seg, file);  
	if (len == seg)
	   {PM_checksum(&sum, &rmdr, bf, 0, len, 1, nbts);
	    nb -= len;}
	else
	   break;};

/* return to where we were in the file */
    io_seek(file, here, SEEK_SET);

    return(sum);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PMCSMA - Fortran callable wrapper for PM_checksum_array */

FIXNUM F77_FUNC(pmcsma, PMCSMA)(void *arr, FIXNUM *pni, FIXNUM *pbpi,
				FIXNUM *pnbts)
   {int ni, bpi, nbts;
    FIXNUM cs;

    ni   = *pni;
    bpi  = *pbpi;
    nbts = *pnbts;

    cs = PM_checksum_array(arr, ni, bpi, nbts);

    return(cs);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
