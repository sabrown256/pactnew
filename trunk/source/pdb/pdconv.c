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

#define ITA2_FIGS  0x1B       /* FIGS character (11011) */
#define ITA2_LTRS  0x1F       /* LTRS character (11111) */

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

/* Character Standards
 *
 * The most widely used standard character representation since 1963
 * is 7-bit ASCII.
 * This has been superceded by UTF-8 of which 7-bit ASCII is a subset.
 * Text conversions are done to 7-bit ASCII currently.
 * Values in the following tables are the ASCII equivalents.
 *
 * Other character standard are:
 *    5-bit ITA2 used from ~1930 to 1963 in telegraphy
 *               used today in TDD and RTTY applications
 *    6-bit ASCII Iso standard, used in CDC computers
 *    EBCDIC IBM 1963 - ?  quasi-non-standard
 *
 * We convert from the above to 7-bit ASCII.  There has been no
 * request or requirement to go the other way
 *
 */

static unsigned char

/* ITA2 letter to 7-bit ASCII
 * NOTE: 0x80 corresponds to shift to figures and 0x81 shift to letters
 */
 ita2l_a7[32] = { 0x20, 0x54, 0x0D, 0x4F, 0x20, 0x48, 0x4E, 0x4D,
		  0x0A, 0x4C, 0x52, 0x47, 0x49, 0x50, 0x43, 0x56,

		  0x45, 0x5A, 0x44, 0x42, 0x53, 0x59, 0x46, 0x58,
		  0x41, 0x57, 0x4A, 0x80, 0x55, 0x51, 0x4B, 0x81 },

/* 7-bit ASCII to ITA2 letter */
 a7_ita2l[128],

/* ITA2 figure to 7-bit ASCII */
 ita2f_a7[32] = { 0x20, 0x35, 0x0D, 0x39, 0x20, 0x23, 0x2C, 0x2E,
		  0x0A, 0x29, 0x34, 0x26, 0x38, 0x30, 0x3A, 0x3B,

		  0x33, 0x22, 0x24, 0x3F, 0x7, 0x36, 0x21, 0x2F,
		  0x2D, 0x32, 0x27, 0x80, 0x37, 0x31, 0x28, 0x81 },

/* 7-bit ASCII to ITA2 figure */
 a7_ita2f[128],

/* 6-bit ASCII to 7-bit ASCII */
 a6_a7[64] = { 0x20, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	       0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,

	       0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	       0x38, 0x39, 0x3A, 0x3B, 0x24, 0x25, 0x26, 0x27,

	       0x00, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
	       0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,

	       0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
	       0x58, 0x59, 0x5A, 0x5B, 0xA3, 0x5D, 0x1B, 0x7F },

/* 7-bit ASCII to 6-bit ASCII */
 a7_a6[128],

/* US EBCDIC to 7-bit ASCII
 * NOTE: characters with no ASCII counterpart have value 0xFF
 */
 e8_a7[256] =  { 0x00, 0x01, 0x02, 0x03, 0xFF, 0x09, 0xFF, 0x7F,
		 0xFF, 0xFF, 0xFF, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,

		 0x10, 0x11, 0x12, 0x13, 0xFF, 0x85, 0x08, 0xFF,
		 0x18, 0x19, 0xFF, 0xFF, 0x1C, 0x1D, 0x1E, 0x1F,
      
		 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0A, 0x17, 0x1B,
		 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x05, 0x06, 0x07,
     
		 0xFF, 0xFF, 0x16, 0xFF, 0xFF, 0xFF, 0xFF, 0x04,
		 0xFF, 0xFF, 0xFF, 0xFF, 0x14, 0x15, 0xFF, 0x1A,
     
		 0x20, 0xA0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		 0xFF, 0xFF, 0xFF, 0x2E, 0x3C, 0x28, 0x2B, 0x7C,
     
		 0x26, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		 0xFF, 0xFF, 0x21, 0x24, 0x2A, 0x29, 0x3B, 0xAC,
     
		 0x2D, 0x2F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		 0xFF, 0xFF, 0xA6, 0x2C, 0x25, 0x5F, 0x3E, 0x3F,
     
		 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		 0xFF, 0x60, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22,
     
		 0xFF, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
		 0x68, 0x69, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xB1,
     
		 0xFF, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
		 0x71, 0x72, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     
		 0xFF, 0x7E, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
		 0x79, 0x7A, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     
		 0x5E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		 0xFF, 0xFF, 0x5B, 0x5D, 0xFF, 0xFF, 0xFF, 0xFF,
     
		 0x7B, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
		 0x48, 0x49, 0xAD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     
		 0x7D, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
		 0x51, 0x52, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     
		 0x5C, 0xFF, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
		 0x59, 0x5A, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     
		 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
		 0x38, 0x39, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },

/* 7-bit ASCII to US EBCDIC */
 a7_e8[128];

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
 float4_text[]    = {15L,   2L,   8L,  0L,  0L,  0L, 0L,    0x0L},
 float8_text[]    = {26L,   3L,  16L,  0L,  0L,  0L, 0L,    0x0L},
 float16_text[]   = {44L,   3L,  16L,  0L,  0L,  0L, 0L,    0x0L},
 float2_ieee[]    = {16L,   5L,  10L,  0L,  1L,  6L, 0L,    0xFL},
 float4_ieee[]    = {32L,   8L,  23L,  0L,  1L,  9L, 0L,   0x7FL},
 float8_ieee[]    = {64L,  11L,  52L,  0L,  1L, 12L, 0L,  0x3FFL},
 float10_ieee[]   = {80L,  15L,  64L,  0L,  1L, 16L, 0L, 0x3FFFL},
 float10_ix87[]   = {128L, 15L,  64L, 48L, 49L, 64L, 1L, 0x3FFEL},
 float12_ieee[]   = {96L,  15L,  64L,  0L,  1L, 16L, 1L, 0x3FFEL},
 float16_ieee[]   = {128L, 15L, 112L,  0L,  1L, 16L, 0L, 0x3FFFL},
 float16_aix[]    = {128L, 11L, 116L,  0L,  1L, 12L, 0L,  0x3FFL},

/* historical formats - kept for testing */

#ifdef GFLOAT
 float4_vax[]     = {32L,   8L,  23L,  0L,  1L,  9L, 0L,   0x81L},
 float8_vax[]     = {64L,  11L,  52L,  0L,  1L, 12L, 0L,  0x401L},
#else
 float4_vax[]     = {32L,   8L,  23L,  0L,  1L,  9L, 0L,   0x81L},
 float8_vax[]     = {64L,   8L,  55L,  0L,  1L,  9L, 0L,   0x81L},
#endif

 float8_cray[]    = {64L,  15L,  48L,  0L,  1L, 16L, 1L, 0x4000L},
 float16_cray[]   = {128L, 15L, 112L,  0L,  1L, 16L, 1L, 0x4000L};

data_standard
 TEXT_STD    = {BITS_DEFAULT,                               /* bits per byte */
                12,                                       /* size of pointer */
                1,                                           /* size of bool */
                {{1, UTF_8},                                    /* char info */
		 {4, UTF_32}},                                 /* wchar info */
                {{4,  TEXT_ORDER},               /* size and order of int8_t */
                 {7,  TEXT_ORDER},                /* size and order of short */
                 {12, TEXT_ORDER},                  /* size and order of int */
                 {22, TEXT_ORDER},                 /* size and order of long */
                 {22, TEXT_ORDER}},           /* size and order of long long */
                {{15, float4_text, text_order},          /* float definition */
                 {26, float8_text, text_order},         /* double definition */
                 {42, float16_text, text_order}}},        /* quad definition */
 I386_STD    = {BITS_DEFAULT,                               /* bits per byte */
                4,                                        /* size of pointer */
                1,                                           /* size of bool */
                {{1, UTF_8},                                    /* char info */
		 {4, UTF_32}},                                 /* wchar info */
                {{1, REVERSE_ORDER},             /* size and order of int8_t */
                 {2, REVERSE_ORDER},              /* size and order of short */
                 {2, REVERSE_ORDER},                /* size and order of int */
                 {4, REVERSE_ORDER},               /* size and order of long */
                 {4, REVERSE_ORDER}},         /* size and order of long long */
                {{4, float4_ieee, float4_rev_order},     /* float definition */
                 {8, float8_ieee, float8_rev_order},    /* double definition */
                 {16, float16_ieee, float16_rev_order}}}, /* quad definition */
 I586L_STD   = {BITS_DEFAULT,                               /* bits per byte */
                4,                                        /* size of pointer */
                1,                                           /* size of bool */
                {{1, UTF_8},                                    /* char info */
		 {4, UTF_32}},                                 /* wchar info */
                {{1, REVERSE_ORDER},             /* size and order of int8_t */
                 {2, REVERSE_ORDER},              /* size and order of short */
                 {4, REVERSE_ORDER},                /* size and order of int */
                 {4, REVERSE_ORDER},               /* size and order of long */
                 {8, REVERSE_ORDER}},         /* size and order of long long */
		{{4, float4_ieee, float4_rev_order},     /* float definition */
                 {8, float8_ieee, float8_rev_order},    /* double definition */
                 {12, float12_ieee, float12_rev_order}}}, /* quad definition */
 I586O_STD   = {BITS_DEFAULT,                               /* bits per byte */
                4,                                        /* size of pointer */
                1,                                           /* size of bool */
                {{1, UTF_8},                                    /* char info */
		 {4, UTF_32}},                                 /* wchar info */
                {{1, REVERSE_ORDER},             /* size and order of int8_t */
                 {2, REVERSE_ORDER},              /* size and order of short */
                 {4, REVERSE_ORDER},                /* size and order of int */
                 {4, REVERSE_ORDER},               /* size and order of long */
                 {8, REVERSE_ORDER}},         /* size and order of long long */
                {{4, float4_ieee, float4_rev_order},     /* float definition */
                 {8, float8_ieee, float8_rev_order},    /* double definition */
                 {16, float16_ieee, float16_rev_order}}}, /* quad definition */
 PPC32_STD   = {BITS_DEFAULT,                               /* bits per byte */
                4,                                        /* size of pointer */
                1,                                           /* size of bool */
                {{1, UTF_8},                                    /* char info */
		 {4, UTF_32}},                                 /* wchar info */
                {{1, NORMAL_ORDER},              /* size and order of int8_t */
                 {2, NORMAL_ORDER},               /* size and order of short */
                 {4, NORMAL_ORDER},                 /* size and order of int */
                 {4, NORMAL_ORDER},                /* size and order of long */
                 {8, NORMAL_ORDER}},          /* size and order of long long */
                {{4, float4_ieee, float4_nrm_order},     /* float definition */
                 {8, float8_ieee, float8_nrm_order},    /* double definition */
                 {16, float16_ieee, float16_nrm_order}}}, /* quad definition */
 X86_64_STD  = {BITS_DEFAULT,                               /* bits per byte */
                8,                                        /* size of pointer */
                1,                                           /* size of bool */
                {{1, UTF_8},                                    /* char info */
		 {4, UTF_32}},                                 /* wchar info */
                {{1, REVERSE_ORDER},             /* size and order of int8_t */
                 {2, REVERSE_ORDER},              /* size and order of short */
                 {4, REVERSE_ORDER},                /* size and order of int */
                 {8, REVERSE_ORDER},               /* size and order of long */
                 {8, REVERSE_ORDER}},         /* size and order of long long */
                {{4, float4_ieee, float4_rev_order},     /* float definition */
                 {8, float8_ieee, float8_rev_order},    /* double definition */
                 {16, float16_ieee, float16_rev_order}}}, /* quad definition */
 X86_64A_STD = {BITS_DEFAULT,                               /* bits per byte */
                8,                                        /* size of pointer */
                1,                                           /* size of bool */
                {{1, UTF_8},                                    /* char info */
		 {4, UTF_32}},                                 /* wchar info */
                {{1, REVERSE_ORDER},             /* size and order of int8_t */
                 {2, REVERSE_ORDER},              /* size and order of short */
                 {4, REVERSE_ORDER},                /* size and order of int */
                 {8, REVERSE_ORDER},               /* size and order of long */
                 {8, REVERSE_ORDER}},         /* size and order of long long */
                {{4, float4_ieee, float4_rev_order},     /* float definition */
                 {8, float8_ieee, float8_rev_order},    /* double definition */
                 {16, float10_ix87, float16_rev_order}}}, /* quad definition */
 PPC64_STD   = {BITS_DEFAULT,                               /* bits per byte */
                8,                                        /* size of pointer */
                1,                                           /* size of bool */
                {{1, UTF_8},                                    /* char info */
		 {4, UTF_32}},                                 /* wchar info */
                {{1, NORMAL_ORDER},              /* size and order of int8_t */
                 {2, NORMAL_ORDER},               /* size and order of short */
                 {4, NORMAL_ORDER},                 /* size and order of int */
                 {8, NORMAL_ORDER},                /* size and order of long */
                 {8, NORMAL_ORDER}},          /* size and order of long long */
                {{4, float4_ieee, float4_nrm_order},     /* float definition */
                 {8, float8_ieee, float8_nrm_order},    /* double definition */
                 {16, float16_ieee, float16_nrm_order}}}, /* quad definition */

/* historical formats - kept for testing */

 M68X_STD    = {BITS_DEFAULT,                               /* bits per byte */
                4,                                        /* size of pointer */
                1,                                           /* size of bool */
                {{1, UTF_8},                                    /* char info */
		 {2, UTF_16}},                                 /* wchar info */
                {{1, NORMAL_ORDER},              /* size and order of int8_t */
                 {2, NORMAL_ORDER},               /* size and order of short */
                 {2, NORMAL_ORDER},                 /* size and order of int */
                 {4, NORMAL_ORDER},                /* size and order of long */
                 {4, NORMAL_ORDER}},          /* size and order of long long */
                {{4, float4_ieee, float4_nrm_order},     /* float definition */
                 {12, float12_ieee, float12_nrm_order}, /* double definition */
                 {16, float16_ieee, float16_nrm_order}}}, /* quad definition */
 VAX_STD     = {BITS_DEFAULT,                               /* bits per byte */
                4,                                        /* size of pointer */
                1,                                           /* size of bool */
                {{1, UTF_8},                                    /* char info */
		 {2, UTF_16}},                                 /* wchar info */
                {{1, REVERSE_ORDER},             /* size and order of int8_t */
                 {2, REVERSE_ORDER},              /* size and order of short */
                 {4, REVERSE_ORDER},                /* size and order of int */
                 {4, REVERSE_ORDER},               /* size and order of long */
                 {4, REVERSE_ORDER}},         /* size and order of long long */
                {{4, float4_vax, float4_vax_order},      /* float definition */
                 {8, float8_vax, float8_vax_order},     /* double definition */
                 {8, float8_vax, float8_vax_order}}},     /* quad definition */
 CRAY_STD    = {BITS_DEFAULT,                               /* bits per byte */
                8,                                        /* size of pointer */
                1,                                           /* size of bool */
                {{1, UTF_8},                                    /* char info */
		 {2, UTF_16}},                                 /* wchar info */
                {{8, NORMAL_ORDER},              /* size and order of int8_t */
		 {8, NORMAL_ORDER},               /* size and order of short */
                 {8, NORMAL_ORDER},                 /* size and order of int */
                 {8, NORMAL_ORDER},                /* size and order of long */
                 {8, NORMAL_ORDER}},          /* size and order of long long */
                {{8, float8_cray, float8_nrm_order},     /* float definition */
                 {8, float8_cray, float8_nrm_order},    /* double definition */
                 {16, float16_cray, float16_nrm_order}}}; /* quad definition */

/*--------------------------------------------------------------------------*/

/*                            DATA_ALIGNMENTS                               */

/*--------------------------------------------------------------------------*/

/* Word Alignment for the different architectures */

data_alignment
 TEXT_ALIGNMENT        = {0, 0, {0, 0}, {0, 0, 0, 0, 0}, {0, 0,  0}, 0},
 BYTE_ALIGNMENT        = {1, 1, {1, 1}, {1, 1, 1, 1, 1}, {1, 1,  1}, 0},
 WORD2_ALIGNMENT       = {2, 1, {1, 2}, {1, 2, 2, 2, 2}, {2, 2,  2}, 0},
 WORD4_ALIGNMENT       = {4, 1, {1, 4}, {1, 4, 4, 4, 4}, {4, 4,  4}, 0},
 WORD8_ALIGNMENT       = {8, 1, {4, 8}, {4, 8, 8, 8, 8}, {8, 8,  8}, 8},
 GNU4_I686_ALIGNMENT   = {4, 1, {1, 4}, {1, 2, 4, 4, 4}, {4, 4,  4}, 0},
 OSX_10_5_ALIGNMENT    = {4, 1, {1, 4}, {1, 2, 4, 4, 4}, {4, 4, 16}, 0},
 SPARC_ALIGNMENT       = {4, 1, {1, 4}, {1, 2, 4, 4, 4}, {4, 8,  8}, 0},
 XLC32_PPC64_ALIGNMENT = {4, 1, {1, 4}, {1, 2, 4, 4, 8}, {4, 4,  4}, 0},
 CYGWIN_I686_ALIGNMENT = {4, 1, {1, 4}, {1, 2, 4, 4, 8}, {4, 8,  4}, 0},
 GNU3_PPC64_ALIGNMENT  = {4, 1, {1, 4}, {1, 2, 4, 4, 8}, {4, 8,  8}, 0},
 GNU4_PPC64_ALIGNMENT  = {4, 1, {1, 4}, {1, 2, 4, 4, 8}, {4, 8, 16}, 0},
 XLC64_PPC64_ALIGNMENT = {8, 1, {1, 4}, {1, 2, 4, 8, 8}, {4, 4,  4}, 0},
 PGI_X86_64_ALIGNMENT  = {8, 1, {1, 4}, {1, 2, 4, 8, 8}, {4, 8,  8}, 0},
 GNU4_X86_64_ALIGNMENT = {8, 1, {1, 4}, {1, 2, 4, 8, 8}, {4, 8, 16}, 0};

/*--------------------------------------------------------------------------*/

/*                             HELPER ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* _PD_NCOPY - copy the NI of size BPI from IN to OUT */

static void _PD_ncopy(char **out, char **in, inti ni, intb bpi)
   {inti nbytes;

    nbytes = ni*bpi;
    memcpy(*out, *in, nbytes);

    *in  += nbytes;
    *out += nbytes;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SET_BIT - set the bit specified as on offset from the given pointer */

static void _PD_set_bit(char *base, inti offs)
   {inti nbytes, mask;

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

static int _PD_get_bit(char *base, inti offs, intb nby, int *ord)
   {inti n;
    intb nbytes, mask;

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

void _PD_sign_extend(char *out, inti ni, intb nbo, intb nbti, int *ord)
   {int sign, indx;
    inti i;
    intb j, sba, tsba;
    unsigned char *lout, mask;

    sba = 8*nbo - nbti;

/* if sba is less than zero we have truncated the integers and really
 * don't know about the sign
 */
    if (sba < 0)
       return;

    lout = (unsigned char *) out;
    for (i = 0L; i < ni; i++)
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

void _PD_ones_complement(char *out, inti ni, intb nbo, int *order)
   {int indx;
    inti i;
    intb j;
    unsigned int carry;
    unsigned char *lout;

    lout = (unsigned char *) out;
    for (i = 0L; i < ni; i++)
        {indx = (order != NULL) ? order[0] - 1 : 0;
         if (lout[indx] & 0x80 )
	    {carry = 1;
	     for (j = nbo-1; (j >= 0) && (carry > 0); j--)
	         {indx   = (order != NULL) ? order[j] - 1 : j;
                  carry += lout[indx];
		  lout[indx] = carry & 0xFF;
		  carry  = (carry > 0xFF);};};

	 lout += nbo;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_BYTE_REVERSE - byte reverse NI words
 *                 - each word is NB bytes long where NB is even
 *
 * #bind PD_byte_reverse fortran() scheme() python()
 */

void PD_byte_reverse(char *out, int32_t nb, int64_t ni)
   {inti i;
    intb jl, jh, nbo2;
    char tmp;
    char *p1, *p2;

    nbo2 = nb >> 1;
    for (jl = 0; jl < nbo2; jl++)
        {jh = nb - jl - 1;
         p1 = out + jh;
         p2 = out + jl;
	 for (i = 0L; i < ni; i++)
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

void _PD_insert_field(long inl, intb nb, char *out,
		      intb offs, int lord, intb lby)
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

/* _PD_PACK_BITS - pack an array that contains a bitstream
 *               - arguments are:
 *               -   ITYP   the target type of the data when unpacked
 *               -   NBITS  the number of bits per item
 *               -   PADSZ  the number of bits of pad preceding the fields
 *               -   FPP    the number of fields per pad
 *               -   NI     the number of items expected
 *               -   OFFS   the bit offset of the first pad
 */

int _PD_pack_bits(char *out, char *in, int ityp, intb nbits,
		  intb padsz, int fpp, inti ni, intb offs)
   {inti i;
    long vl;

    for (i = 0L; i < ni; i++)
        {SC_convert_id(SC_LONG_I, &vl, 0, 1, ityp, in, i, 1, 1, FALSE);
	 _PD_insert_field(vl, nbits, out, offs, host_order, sizeof(long));
	 offs += nbits;};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_REQUIRE_CONV - return TRUE iff conversion between file type DPF and
 *                  - host type DPH is required
 */

int _PD_require_conv(defstr *dpf, defstr *dph)
   {int i, ni, lreorder, lreformat, ltuple, cnv;
    intb bpif, bpih;
    int *ordf, *ordh;
    long *fmtf, *fmth;
    multides *tupf, *tuph;

    cnv = dpf->convert;

    if (cnv == -1)
       {ordf = dpf->fp.order;
	fmtf = dpf->fp.format;
	bpif = dpf->size;
	tupf = dpf->tuple;

	ordh = dph->fp.order;
	fmth = dph->fp.format;
	bpih = dph->size;
	tuph = dph->tuple;

	lreorder = (bpif != bpih);
	if ((ordf != NULL) && (ordh != NULL))
	   {for (i = 0; (i < bpif) && (lreorder == FALSE); i++)
	        lreorder = (ordf[i] != ordh[i]);};

	lreformat = FALSE;
	if ((fmtf != NULL) && (fmth != NULL))
	   {for (i = 0; (i < 8) && (lreformat == FALSE); i++)
	        lreformat = (fmtf[i] != fmth[i]);};

	ltuple = FALSE;
	if ((tupf != NULL) && (tuph != NULL))
	   {ltuple = (tupf->ni != tuph->ni);
	    ni     = tupf->ni;
	    if ((tupf->order != NULL) && (tuph->order != NULL))
	       {for (i = 0; (i < ni) && (ltuple == FALSE); i++)
		    ltuple = (tupf->order[i] != tuph->order[i]);};};

	cnv = FALSE;

	cnv |= (dpf->kind        != dph->kind);
	cnv |= (dpf->size_bits   != dph->size_bits);
	cnv |= (dpf->size        != dph->size);
	cnv |= (dpf->alignment   != dph->alignment);
	cnv |= (dpf->n_indirects != dph->n_indirects);
	cnv |= (dpf->is_indirect != dph->is_indirect);
	cnv |= (dpf->onescmp     != dph->onescmp);
	cnv |= (dpf->unsgned     != dph->unsgned);

        if (dpf->kind == INT_KIND)
	   cnv |= (dpf->fix.order   != dph->fix.order);

	cnv |= lreorder;
	cnv |= lreformat;
	cnv |= ltuple;

	dpf->convert = cnv;};

    return(cnv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_REQUIRES_CONVERSION - return TRUE iff defstr DPF of type
 *                         - OUTTYPE requires conversion from INTYPE
 */

int _PD_requires_conversion(PDBfile *file, defstr *dpf,
			    char *outtype, char *intype)
   {int cnv;
    defstr *dph;

    if (dpf == NULL)
       dpf = _PD_lookup_type(intype, file->chart);

    if (dpf->convert == -1)
       {dph = _PD_lookup_type(outtype, file->host_chart);
	cnv = _PD_require_conv(dpf, dph);}
    else
       cnv = ((dpf->convert > 0) || (strcmp(intype, outtype) != 0));

    return(cnv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_REORDER - given a pointer to an array ARR with NI items of BPI each
 *             - put them in the order defined by ORD
 */

static void _PD_reorder(char *arr, inti ni, intb bpi, int *ord)
   {int lreorder;
    inti i;
    char local[MAXLINE];

    lreorder = FALSE;

    for (i = 0; i < bpi; i++)
        if (ord[i] != (i+1))
           {lreorder = TRUE;
	    break;}

    if (lreorder == TRUE)
       {for (; ni > 0; ni--)
	    {arr--;
	     for (i = 0; i < bpi; local[i] = arr[ord[i]], i++);
	     arr++;
	     for (i = 0; i < bpi; *(arr++) = local[i++]);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BYTE_ALIGN - given an input bitstream of packed items,
 *                - return an array of the items aligned on byte
 *                - boundaries
 */

static void _PD_byte_align(char *out, char *in, inti ni,
			   long *infor, int *inord, intb boffs)
   {intb chunk1, chunk2, outbytes, remainder, nbitsin;
    inti i, inrem;
    unsigned char *inptr, *outptr;
    unsigned char mask1, mask2;
 
    nbitsin = infor[0];

    outbytes = (nbitsin >> 3) + 1;
 
    memset(out, 0, ni*outbytes);

/* develop logic for doing the unpack without regard to inord. */
    inptr  = (unsigned char *) in;
    outptr = (unsigned char *) out;
    inrem  = nbitsin * ni;
    remainder = nbitsin % 8;

    chunk1  = min(8, nbitsin);
    chunk1 -= boffs;
    chunk2  = (boffs == 0) ? 0 : 8 - chunk1;
    mask1   = (1 << chunk1) - 1;
    mask2   = (1 << chunk2) - 1;

    while (inrem)
       {for (i = 0; i < outbytes-1; i++, inptr++, outptr++, inrem -= 8)
	    {*outptr = ((*inptr & mask1) << chunk2) |    
		       ((*(inptr + 1)  >> chunk1) & mask2);};
           
	if (remainder <= chunk1)
	   {*outptr++ = (*inptr << chunk2) & 
	                (((1 << remainder) - 1)  << (8 - remainder));
	    if (remainder == chunk1)
	       inptr++;
	    chunk1 -= remainder;
	    if (chunk1 == 0)
	       chunk1 = 8;
	    chunk2  = 8 - chunk1;}

	else
	   {*outptr =    ((*inptr << chunk2) &
			  (((1 << chunk1) - 1) << chunk2));
	    *outptr++ |= ((*(++inptr) >> chunk1) &
			  (((1 << (remainder - chunk1)) - 1) << (8 - (remainder - chunk1) - chunk1)));
	    chunk1 = 8 - (remainder - chunk1);
	    chunk2 = 8 - chunk1;};

	mask1 = (1 << chunk1) - 1;
	mask2 = (1 << chunk2) - 1;
	inrem -= remainder;};
    
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
                              intb lby, inti ni)
   {int sign;
    intb nbi, nbo, nbi_exp, nbo_exp;
    intb inrem, outrem, indxin, indxout, nbits;
    intb byi, byo;
    inti i;
    inti bi_exp, bi_mant, bi_sign;
    inti bo_exp, bo_mant, bo_sign;
    long expn, mant;
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

    byi = (nbi + 7) >> 3;
    byo = (nbo + 7) >> 3;

/* zero out the output buffer */
    memset(out, 0, ni*byo);

    lout = out;
    lin  = in;
    for (i = 0L; i < ni; i++)

/* move the exponent over */
        {expn = SC_extract_field(lin, bi_exp, nbi_exp, byi, inord);
         sign = _PD_get_bit(lin, bi_sign, byi, inord);

         _PD_insert_field(expn, nbo_exp, lout, bo_exp, lord, lby);

         if (sign)
	    _PD_set_bit(lout, bo_sign);

         indxin  = bi_mant;
         indxout = bo_mant;
         inrem   = infor[2];
         outrem  = outfor[2];

/* move the mantissa over in sizeof(long) packets */
         while ((inrem > 0) && (outrem > 0))
	    {nbits = min(BitsMax, inrem);
	     nbits = min(nbits, outrem);
	     mant  = SC_extract_field(lin, indxin, nbits, byi, inord);

	     _PD_insert_field(mant, nbits, lout, indxout, lord, lby);

	     indxin  += nbits;
	     indxout += nbits;
	     inrem   -= nbits;
	     outrem  -= nbits;};

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

int _PD_null_pointer(char *in, intb nb)
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

static void _PD_bin_text(char **out, char **in, char *typ, inti ni,
			 intb boffs,
			 PD_type_kind kndi, long *ifmt, intb nbi,
			 PD_byte_order ordi, int *iord,
			 PD_type_kind kndo, long *ofmt, intb nbo,
			 PD_byte_order ordo, int *oord,
			 data_standard *hstd,
			 int onescmp, int usg, int rdx, char *delim)
   {int fl, nd;
    int *hord;
    inti i, nb, ne;
    intb nc, nbl;
    long *hfmt;
    char fmt[MAXLINE];
    char *lout, *lin, *tout, *p;
    PD_byte_order ordl;

    lin  = *in;
    lout = *out;

/* GOTCHA: should this now be quad_format? */
    hfmt = hstd->fp[PD_DOUBLE_I].format;
    hord = hstd->fp[PD_DOUBLE_I].order;
    ordl = hstd->fx[PD_LONG_I].order;
    nbl  = hstd->fx[PD_LONG_I].bpi;

/* convert char types */
    if (kndi == CHAR_KIND)
       {nb = ni*nbi;
	if (delim == NULL)
	   {snprintf(fmt, MAXLINE, "%%-%llds", (long long) nb);
	    nc = nb;}
	else
	   {ne = strlen(lin);
	    ne = min(ne, nb);
	    nc = ne + strlen(delim) + 2;
	    if (strchr(lin, '\'') == NULL)
	       snprintf(fmt, MAXLINE, "'%%-%llds'%s",
			(long long) ne, delim);
	    else
	       snprintf(fmt, MAXLINE, "\"%%-%llds\"%s",
			(long long) ne, delim);};
	snprintf(lout, nc+1, fmt, lin);

	*in  += nb;
	*out += nc;}

/* convert integer types */
    else if (kndi == INT_KIND)
       {long *lv;

	lv   = CMAKE_N(long, ni);
	tout = (char *) lv;

	_PD_iconvert(&tout, in, ni, nbi, ordi, nbl, ordl, onescmp, usg);

	if (delim == NULL)
	   snprintf(fmt, MAXLINE, "%%%ldld", (long) nbo);
	else
	   {snprintf(fmt, MAXLINE, "%%%ldld%s", (long) nbo, delim);
	    nbo += strlen(delim);};

        for (p = lout, i = 0; i < ni; p += nbo, i++)
	    snprintf(p, nbo+1, fmt, lv[i]);

	CFREE(lv);

	*out += nbo*ni;}

/* convert floating point types */
    else if (kndi == FLOAT_KIND)
       {double *dv;

	dv   = CMAKE_N(double, ni);
	tout = (char *) dv;

	_PD_fconvert(&tout, in, ni, boffs,
		     ifmt, iord, hfmt, hord,
		     ordl, nbl, onescmp);

        fl = ofmt[0];
        nd = ofmt[2];
	if (delim == NULL)
	   snprintf(fmt, MAXLINE, "%%%d.%de", fl, nd);
	else
	   {snprintf(fmt, MAXLINE, "%%%d.%de%s", fl, nd, delim);
	    nbo += strlen(delim);};

        for (p = lout, i = 0; i < ni; p += nbo, i++)
	    snprintf(p, nbo+1, fmt, dv[i]);

	CFREE(dv);

	*out += nbo*ni;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_TEXT_BIN - convert text to binary numbers */

void _PD_text_bin(char **out, char **in, char *typ, inti ni,
		  intb boffs,
		  PD_type_kind kndi, long *ifmt, intb nbi,
		  PD_byte_order ordi, int *iord,
		  PD_type_kind kndo, long *ofmt, intb nbo,
		  PD_byte_order ordo, int *oord,
		  data_standard *hstd,
		  int onescmp, int usg, int rdx, char *delim)
   {int *hord;
    inti i, nb, ne;
    intb nc, nbl;
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

    lin  = *in;
    lout = *out;
    SC_ASSERT(lout != NULL);

/* GOTCHA: should this now be quad_format? */
    hfmt = hstd->fp[PD_DOUBLE_I].format;
    hord = hstd->fp[PD_DOUBLE_I].order;
    ordl = hstd->fx[PD_LONG_I].order;
    nbl  = hstd->fx[PD_LONG_I].bpi;

    if (delim == NULL)
       delim = "\n";

/* convert char types */
    if (kndi == CHAR_KIND)
       {nb = ni*nbo;
	p  = strdelim(lin, delim);
	if (p != NULL)
	   {*p = '\0';
	    nc = strlen(lin) + 1;
	    ne = min(nb + 1, nc);}
	else
	   {nc = ni*nbi;
	    ne = min(nb + 1, nc + 1);};

	SC_strncpy(*out, ne, lin, -1);

	*out += nb;
	lin  += nc;}

/* convert integer types */
    else if (kndi == INT_KIND)
       {long *lv;

	lv = CMAKE_N(long, ni);

        for (i = 0; i < ni; i++)
	    {p = strdelim(lin, delim);
	     if (p != NULL)
	        *p++ = '\0';

	     lv[i] = SC_strtol(lin, &lin, rdx);
	     if (*delim != '\n')
	        lin++;};

	tin = (char *) lv;
	_PD_iconvert(out, &tin, ni, nbl, ordl, nbo, ordo, onescmp, usg);

	CFREE(lv);}

/* convert floating point types */
    else if (kndi == FLOAT_KIND)
       {double *dv;

	dv = CMAKE_N(double, ni);

	for (i = 0; i < ni; i++)
	    {p = strdelim(lin, delim);
	     if (p != NULL)
	        *p++ = '\0';

	     dv[i] = SC_strtod(lin, &lin);
	     if (*delim != '\n')
	        lin++;};

	tin = (char *) dv;
	_PD_fconvert(out, &tin, ni, boffs,
		     hfmt, hord, ofmt, oord,
		     ordl, nbl, onescmp);

	CFREE(dv);};

    *in = lin;

    return;}

/*--------------------------------------------------------------------------*/

/*                         POINTER CONVERSION ROUTINES                      */

/*--------------------------------------------------------------------------*/

/* _PD_CONVERT_PTR_RD - convert the pointer from BF to a fixed point number
 *                    - and return it
 */

long _PD_convert_ptr_rd(char *bfi, intb fbpi, PD_byte_order ford,
			intb hbpi, PD_byte_order hord, data_standard *hs)
   {long n;
    char *in, *out;

    in  = bfi;
    out = (char *) &n;
    if (ford == TEXT_ORDER)
       _PD_text_bin(&out, &in, SC_INT_S, 1L, 0,
		    INT_KIND, NULL, fbpi, ford, NULL,
		    INT_KIND, NULL, hbpi, hord, NULL,
		    hs, FALSE, FALSE, 10, NULL);
    else
       _PD_iconvert(&out, &in, 1L, fbpi, ford, hbpi, hord, FALSE, FALSE);

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CONVERT_PTR_WR - convert the pointer N to text form */

static void _PD_convert_ptr_wr(char *bfo, long n, PDBfile *file,
			       intb fbpi, PD_byte_order ford,
			       intb hbpi, PD_byte_order hord,
			       data_standard *hs)
   {char *in, *out;

    in  = (char *) &n;
    out = bfo;
    if (_PD_TEXT_OUT(file) == TRUE)
       {memset(out, ' ', fbpi);
	_PD_bin_text(&out, &in, SC_INT_S, 1L, 0,
		     INT_KIND, NULL, hbpi, hord, NULL,
		     INT_KIND, NULL, fbpi, ford, NULL,
		     hs, FALSE, FALSE, 10, NULL);}
    else
       {memset(out, 0, fbpi);
	_PD_iconvert(&out, &in, 1L, hbpi, hord, fbpi, ford,
		     FALSE, FALSE);};

    return;}

/*--------------------------------------------------------------------------*/
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

static int _PD_convert_ptr(char **pout, char **pin, inti *poo, inti *pio,
			   inti ni, hasharr *chi, hasharr *cho,
			   data_standard *stdo, data_standard *stdi,
			   data_standard *hstd, inti iao, inti iai)
   {intb fbpi, hbpi;
    inti i, n, nbo, nbi;
    char *p;
    PD_data_location loc;
    PDBfile *file;
    data_standard *hs, *fs;
    PD_address *ad;
    PD_byte_order ford, hord;

    nbo = stdo->ptr_bytes + iao;
    nbi = stdi->ptr_bytes + iai;

    file = hstd->file;
    if (file != NULL)
       {hs   = file->host_std;
	fs   = file->std;
	fbpi = fs->ptr_bytes;
	ford = fs->fx[PD_LONG_I].order;
	hbpi = hs->fx[PD_LONG_I].bpi;
	hord = hs->fx[PD_LONG_I].order;

	for (i = 0; i < ni; i++)
	    {n = 1L;

/* convert pointer when writing to file */
	     if (stdi == hstd)
	        {p  = *pin;
		 p  = DEREF(p);
		 ad = _PD_ptr_wr_lookup(file, p, &loc, FALSE, TRUE);
		 n  = (ad == NULL) ? -1 : ad->indx;

		 _PD_convert_ptr_wr(*pout, n, file, fbpi, ford, hbpi, hord, hs);}

/* convert pointer when reading from file */
	     else if (stdo == hstd)
	        {n = _PD_convert_ptr_rd(*pin, fbpi, ford, hbpi, hord, hs);
		 _PD_ptr_get_index(file, n, *pout);};

	     *pin  += nbi;
	     *pio  += nbi;
	     *pout += nbo;
	     *poo  += nbo;};}

    else
       {*pin  += ni*nbi;
	*pio  += ni*nbi;
	*pout += ni*nbo;
	*poo  += ni*nbo;};
       
    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CONVERT_FNC - convert function pointer
 *                 - this is a no-op which skips over the space
 */

static int _PD_convert_fnc(char **pout, char **pin, inti *poo, inti *pio,
			   inti nbo, inti nbi)
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

/* _PD_CONV_TO_ASCII_7 - translate characters from CSTD to 7-bit ASCII
 *                     - do translation in place
 *                     - Inputs : BF   - character buffer
 *                     -          NI   - number of fields to translate
 *                     -          CSTD - character standard ID
 */

static void _PD_conv_to_ascii_7(char *bf, inti ni, int cstd)
   {int c, cn, lwc;
    inti i, l;
    PD_character_standard ucstd;
    unsigned char *pcs, *ubf;

    ubf   = (unsigned char *) bf;
    ucstd = (PD_character_standard) abs(cstd);
    lwc   = (cstd > 0);

    if (ucstd == PD_ITA2_LOWER)
       {pcs = ita2l_a7;
	for (i = 0, l = 0; i < ni; i++)
	    {c = bf[i];
	     if (c == ITA2_FIGS)
	        pcs = ita2f_a7;
	     else if (c == ITA2_LTRS)
	        pcs = ita2l_a7;
	     else
	        {cn = pcs[c];
		 if (lwc == TRUE)
		    cn = tolower(cn);

		 bf[l++] = cn;};};
       bf[l++] = '\0';
       bf[l++] = '\0';}

    else if (ucstd == PD_ASCII_6_LOWER)
       {pcs = a6_a7;
	for (i = 0; i < ni; i++)
	    {c = bf[i];

	     c = pcs[c];
	     if (lwc == TRUE)
	        c = tolower(c);

	     bf[i] = c;};}

    else if (ucstd == PD_EBCDIC)
       {pcs = e8_a7;
	for (i = 0; i < ni; i++)
	    {c = ubf[i];
	     ubf[i] = pcs[c];};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CONV_TO_ASCII_6 - translate characters from CSTD to ASCII_6
 *                     - do translation in place
 *                     - Inputs : BF   - character buffer
 *                     -          NI   - number of fields to translate
 *                     -          CSTD - character standard ID
 */

static void _PD_conv_to_ascii_6(char *bf, inti ni, int cstd)
   {int c, nc;
    inti i;

    ONCE_SAFE(TRUE, NULL)
       memset(a7_a6, 0, 128);
       nc = sizeof(a6_a7);
       for (i = 0; i < nc; i++)
	   {c = a6_a7[i];
	    a7_a6[c] = i;};
    END_SAFE;

    _PD_conv_to_ascii_7(bf, ni, cstd);

    for (i = 0; i < ni; i++)
        {c = bf[i];
	 c = toupper(c);
	 bf[i] = a7_a6[c];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CONV_TO_EBCDIC - translate characters from CSTD to EBCDIC
 *                    - do translation in place
 *                    - Inputs : BF   - character buffer
 *                    -          NI   - number of fields to translate
 *                    -          CSTD - character standard ID
 */

static void _PD_conv_to_ebcdic(char *bf, inti ni, int cstd)
   {int c, nc;
    inti i;

    ONCE_SAFE(TRUE, NULL)
       memset(a7_e8, 0, 128);
       nc = sizeof(e8_a7);
       for (i = 0; i < nc; i++)
           {c = e8_a7[i];
	    a7_e8[c] = i;};
    END_SAFE;

    _PD_conv_to_ascii_7(bf, ni, cstd);

    for (i = 0; i < ni; i++)
        {c = bf[i];
	 bf[i] = a7_e8[c];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CONV_TO_ITA2 - translate characters from CSTD to ITA2
 *                  - do translation in place
 *                  - Inputs : BF   - character buffer
 *                  -          NB   - size of BF
 *                  -          NI   - number of fields to translate
 *                  -          CSTD - character standard ID
 */

static int _PD_conv_to_ita2(char *bf, inti nb, inti ni, int cstd)
   {int c, cn, nc, let, nlet;
    inti i, l;
    char *nbf;

    ONCE_SAFE(TRUE, NULL)
       memset(a7_ita2l, '\004', 128);
       nc = sizeof(ita2l_a7);
       for (i = 0; i < nc; i++)
           {c = ita2l_a7[i];
	    a7_ita2l[c] = i;};

       memset(a7_ita2f, '\004', 128);
       nc = sizeof(ita2f_a7);
       for (i = 0; i < nc; i++)
           {c = ita2f_a7[i];
	    a7_ita2f[c] = i;};
    END_SAFE;

    _PD_conv_to_ascii_7(bf, ni, cstd);

    nbf = CMAKE_N(char, 2*ni);

/* start with two letter shift chars */
    l        = 0;
    let      = ITA2_LTRS;
    nbf[l++] = let;
    nbf[l++] = let;

    for (i = 0; i < ni; i++)
        {c = bf[i];
	 c = toupper(c);
	 if (('A' <= c) && (c <= 'Z'))
	    {cn   = a7_ita2l[c];
	     nlet = ITA2_LTRS;}
	 else if ((c == '\n') || (c == '\r'))
	    {cn   = a7_ita2l[c];
	     nlet = let;}
	 else
	    {cn   = a7_ita2f[c];
	     nlet = ITA2_FIGS;};

	 if (let != nlet)
	    nbf[l++] = nlet;

	 nbf[l++] = cn;
	 let      = nlet;};

    SC_strncpy(bf, nb, nbf, l);

    CFREE(nbf);

    return(l);}

/*--------------------------------------------------------------------------*/

/*                      INTEGER CONVERSION ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* _PD_ICONVERT - convert NI integers of NBI bytes to integers of NBO bytes
 *              - the number of bytes for each integer are given
 */

void _PD_iconvert(char **out, char **in, inti ni,
		  intb nbi, PD_byte_order ordi,
		  intb nbo, PD_byte_order ordo,
		  int onescmp, int usg)
   {inti i;
    intb j;
    char *lout, *lin, *po, *pi;
    char s_extend;

    lin  = *in;
    lout = *out;

    s_extend = usg ? 0 : 0xff;

/* convert ni integers
 * test sign bit to properly convert negative integers
 */
    if (nbi < nbo)
       {if (ordi == REVERSE_ORDER)
	   {for (j = nbi; j < nbo; j++)
                {po = lout + j - nbi;
                 pi = lin + nbi - 1;
                 for (i = 0L; i < ni; i++)
                     {*po = (*pi & 0x80) ? s_extend : 0;
                      po += nbo;
		      pi += nbi;};};
	    for (j = nbi; j > 0; j--)
                {po = lout + nbo - j;
                 pi = lin + j - 1;
                 for (i = 0L; i < ni; i++)
                     {*po = *pi;
                      po += nbo;
		      pi += nbi;};};}
        else
	   {for (j = nbi; j < nbo; j++)
                {po = lout + j - nbi;
                 pi = lin;
                 for (i = 0L; i < ni; i++)
                     {*po = (*pi & 0x80) ? s_extend : 0;
                      po += nbo;
		      pi += nbi;};};
	    for (j = 0; j < nbi; j++)
                {po = lout + j + nbo - nbi;
                 pi = lin + j;
                 for (i = 0L; i < ni; i++)
                     {*po = *pi;
                      po += nbo;
		      pi += nbi;};};};}

    else if (nbi >= nbo)
       {if (ordi == REVERSE_ORDER)
           {for (j = nbo; j > 0; j--)
                {po = lout + nbo - j;
                 pi = lin + j - 1;
                 for (i = 0L; i < ni; i++)
                     {*po = *pi;
                      po += nbo;
		      pi += nbi;};};}
        else
           {for (j = nbi - nbo; j < nbi; j++)
                {po = lout + j - nbi + nbo;
                 pi = lin + j;
                 for (i = 0L; i < ni; i++)
                     {*po = *pi;
                      po += nbo;
		      pi += nbi;};};};};

/* if the input used ones complement arithmetic convert to twos complement */
    if (onescmp)
       _PD_ones_complement(*out, ni, nbo, NULL);
                          
    if (ordo == REVERSE_ORDER)
       PD_byte_reverse(*out, nbo, ni);

    *in  += ni*nbi;
    *out += ni*nbo;

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
   {inti _i;                                                                 \
    intb _ibo0, _ibo1;                                                       \
    intb _obo0, _obo1;                                                       \
    _ibo0 = _ibo[0] - 1;                                                     \
    _ibo1 = _ibo[1] - 1;                                                     \
    _obo0 = _obo[0] - 1;                                                     \
    _obo1 = _obo[1] - 1;                                                     \
    for (_i = 0; _i < _ni; _i++, _obf += _bpi, _ibf += _bpi)                 \
        {_obf[_obo0] = _ibf[_ibo0];                                          \
	 _obf[_obo1] = _ibf[_ibo1];};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* XFER4 - transfer _NI items at _BPI bytes per item from _IBF to _OBF
 *       - rearranging the byte order according to _IBO and _OBO
 */

#define XFER4(_obf, _ni, _ibf, _ibo, _obo, _bpi)                             \
   {inti _i;                                                                 \
    intb _ibo0, _ibo1, _ibo2, _ibo3;                                         \
    intb _obo0, _obo1, _obo2, _obo3;                                         \
    _ibo0 = _ibo[0] - 1;                                                     \
    _ibo1 = _ibo[1] - 1;                                                     \
    _ibo2 = _ibo[2] - 1;                                                     \
    _ibo3 = _ibo[3] - 1;                                                     \
    _obo0 = _obo[0] - 1;                                                     \
    _obo1 = _obo[1] - 1;                                                     \
    _obo2 = _obo[2] - 1;                                                     \
    _obo3 = _obo[3] - 1;                                                     \
    for (_i = 0; _i < _ni; _i++, _obf += _bpi, _ibf += _bpi)                 \
        {_obf[_obo0] = _ibf[_ibo0];                                          \
	 _obf[_obo1] = _ibf[_ibo1];                                          \
	 _obf[_obo2] = _ibf[_ibo2];                                          \
	 _obf[_obo3] = _ibf[_ibo3];};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* XFER8 - transfer _NI items at _BPI bytes per item from _IBF to _OBF
 *       - rearranging the byte order according to _IBO and _OBO
 */

#define XFER8(_obf, _ni, _ibf, _ibo, _obo, _bpi)                             \
   {inti _i;                                                                 \
    intb _ibo0, _ibo1, _ibo2, _ibo3, _ibo4, _ibo5, _ibo6, _ibo7;             \
    intb _obo0, _obo1, _obo2, _obo3, _obo4, _obo5, _obo6, _obo7;             \
    _ibo0 = _ibo[0] - 1;                                                     \
    _ibo1 = _ibo[1] - 1;                                                     \
    _ibo2 = _ibo[2] - 1;                                                     \
    _ibo3 = _ibo[3] - 1;                                                     \
    _ibo4 = _ibo[4] - 1;                                                     \
    _ibo5 = _ibo[5] - 1;                                                     \
    _ibo6 = _ibo[6] - 1;                                                     \
    _ibo7 = _ibo[7] - 1;                                                     \
    _obo0 = _obo[0] - 1;                                                     \
    _obo1 = _obo[1] - 1;                                                     \
    _obo2 = _obo[2] - 1;                                                     \
    _obo3 = _obo[3] - 1;                                                     \
    _obo4 = _obo[4] - 1;                                                     \
    _obo5 = _obo[5] - 1;                                                     \
    _obo6 = _obo[6] - 1;                                                     \
    _obo7 = _obo[7] - 1;                                                     \
    for (_i = 0; _i < _ni; _i++, _obf += _bpi, _ibf += _bpi)                 \
        {_obf[_obo0] = _ibf[_ibo0];                                          \
	 _obf[_obo1] = _ibf[_ibo1];                                          \
	 _obf[_obo2] = _ibf[_ibo2];                                          \
	 _obf[_obo3] = _ibf[_ibo3];                                          \
	 _obf[_obo4] = _ibf[_ibo4];                                          \
	 _obf[_obo5] = _ibf[_ibo5];                                          \
	 _obf[_obo6] = _ibf[_ibo6];                                          \
	 _obf[_obo7] = _ibf[_ibo7];};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* XFER16 - transfer _NI items at _BPI bytes per item from _IBF to _OBF
 *        - rearranging the byte order according to _IBO and _OBO
 */

#define XFER16(_obf, _ni, _ibf, _ibo, _obo, _bpi)                            \
   {inti _i;                                                                 \
    intb _ibo0, _ibo1, _ibo2, _ibo3, _ibo4, _ibo5, _ibo6, _ibo7;             \
    intb _ibo8, _ibo9, _ibo10, _ibo11, _ibo12, _ibo13, _ibo14, _ibo15;       \
    intb _obo0, _obo1, _obo2, _obo3, _obo4, _obo5, _obo6, _obo7;             \
    intb _obo8, _obo9, _obo10, _obo11, _obo12, _obo13, _obo14, _obo15;       \
    _ibo0  = _ibo[0] - 1;                                                    \
    _ibo1  = _ibo[1] - 1;                                                    \
    _ibo2  = _ibo[2] - 1;                                                    \
    _ibo3  = _ibo[3] - 1;                                                    \
    _ibo4  = _ibo[4] - 1;                                                    \
    _ibo5  = _ibo[5] - 1;                                                    \
    _ibo6  = _ibo[6] - 1;                                                    \
    _ibo7  = _ibo[7] - 1;                                                    \
    _ibo8  = _ibo[8] - 1;                                                    \
    _ibo9  = _ibo[9] - 1;                                                    \
    _ibo10 = _ibo[10] - 1;                                                   \
    _ibo11 = _ibo[11] - 1;                                                   \
    _ibo12 = _ibo[12] - 1;                                                   \
    _ibo13 = _ibo[13] - 1;                                                   \
    _ibo14 = _ibo[14] - 1;                                                   \
    _ibo15 = _ibo[15] - 1;                                                   \
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

void _PD_fconvert(char **out, char **in, inti ni, intb boffs,
		  long *infor, int *inord, long *outfor, int *outord,
		  PD_byte_order ordl, intb bpl, int onescmp)
   {int loverflow, lunderflow, lreorder, lreformat;
    int out_freorder;
    int ssave[MBLOCKS];
    inti i, j, l, ls, le;
    inti bi_sign, bo_sign, bi_exp, bo_exp, bi_mant, bo_mant;
    inti bo_sign_save, bo_exp_save, bo_mant_save;
    inti lbpi, lbpo, lbpir;
    intb hexpn, dbias, hmbo, hmbi;
    intb nbits, bpii, bpio;
    intb indxout, inrem, outrem, nleft, numleft;
    intb mbiti, mbito, nbitsout;
    intb nbi_exp, nbo_exp, nbi_man, nbo_man, nbi, nbo;
    intb hmbyt, mbyti, mbyto;
    intb sbit, ebit, hmbit, dhmb, dmbit, rshift;
    intb sbyt, ebyt;
    intb nb_mant_rem, remainder, k;
    long emax;
    long esave[MBLOCKS], tformat[8];
    char mask, mask1, mask2, cmask;
    char inreorder[MBLOCKS*MBYTES];
    char *lout, *lin;
    char *aligned, *insave, *inrsave;
    char *bufin, *btemp, *ctemp;
    char *freorder;
    unsigned char *rout;
    unsigned char *pmbo, *pmbi, *pout, *pin;
    unsigned char *psb, *peb, *pebsave, *phmb;

    aligned      = NULL;
    freorder     = NULL;
    out_freorder = FALSE;
    insave       = NULL;
    inrsave      = NULL;

    nbi  = infor[0];
    nbo  = outfor[0];
    bpii = (nbi + 7) >> 3;
    bpio = (nbo + 7) >> 3;

    if ((nbi % 8 != 0) || boffs)
       {aligned = CMAKE_N(char, bpii*ni);
        _PD_byte_align(aligned, *in, ni, infor, inord, boffs);
        insave = *in;
        *in    = aligned;};

/* check for cases where only difference between formats is in byte order */
    lreformat = FALSE;
    for (i = 0; i < 8; i++)
        {if (infor[i] != outfor[i])
            {lreformat = TRUE;
             break;};};

    if (lreformat == FALSE)

/* check to see if input byte order is different than output byte order */
       {lreorder = FALSE;
        for (i = 0; i < bpii; i++)
            {if (inord[i] != outord[i])
                {lreorder = TRUE;
                 break;};};

/* reorder bytes and return if only difference is in byte order */
        if (lreorder == TRUE)
           {lin  = *in;
            lout = *out;

/* allow for compiler to do loop unrolling */
            switch (bpii)
	       {case 1:
		     break;
		case 2:
		     XFER2(lout, ni, lin, inord, outord, bpii);
		     break;
		case 4:
		     XFER4(lout, ni, lin, inord, outord, bpii);
		     break;
		case 8:
		     XFER8(lout, ni, lin, inord, outord, bpii);
		     break;
		case 16:
		     XFER16(lout, ni, lin, inord, outord, bpii);
		     break;
		default:
		     for (j = 0; j < ni; j++, lin += bpii, lout += bpii)
		         {for (i = 0; i < bpii; i++)
			      {lout[outord[i] - 1] = lin[inord[i] - 1];};};};

            *in  += ni*bpii;
            *out += ni*bpio;

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

    hmbyt = 0;
    hmbo  = (outfor[6] & 1L);
    hmbi  = (infor[6] & 1L);

    dbias = outfor[7] + hmbo - infor[7] - hmbi;
    hexpn = 1L << (outfor[1] - 1L);
    emax  = (1L << outfor[1]) - 1L;

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
       {freorder = CMAKE_N(char, bpii*ni);
        tformat[0] = infor[0];
        tformat[1] = infor[1];
        tformat[2] = infor[2];
        tformat[3] = bi_sign = 0L;
        tformat[4] = bi_exp  = 1L;
        tformat[5] = bi_mant = tformat[4] + tformat[1];
        _PD_field_reorder(*in, freorder, infor, tformat, inord,
                          ordl, bpl, ni);
        inrsave = *in;
        *in     = freorder;}

    if (!((outfor[3] < outfor[4]) && (outfor[4] < outfor[5])))
       {out_freorder = TRUE;
        bo_sign = bo_sign_save = 0L;
        bo_exp  = bo_exp_save  = 1L;
        bo_mant = bo_mant_save = bo_exp + outfor[1];}

/* check to see if input needs reordering */
    lreorder = FALSE;
    for (i = 0; i < bpii; i++)
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
    memset(*out, 0, ni*bpio);

/* set up sign bit byte index and bit index */
    sbyt = bi_sign >> 3;
    sbit = bi_sign % 8;

/* set up exponent location variables */
    ebyt = bi_exp >> 3;
    ebit = bi_exp % 8;

/* set up mantissa location variables */
    mbyti = bi_mant >> 3;
    mbiti = bi_mant % 8;

    mbyto = bo_mant >> 3;
    mbito = bo_mant % 8;

    dhmb  = hmbo - hmbi;

/* if input high mantissa bit (HMB) is assumed 1 and not written (e.g. IEEE)
 * but output HMB is assumed 0 (e.g. CRAY) write the input starting at
 * the output HMB+1 and set the HMB
 */
    if (dhmb > 0)
       {hmbit = mbito;
        hmbyt = mbyto;
        mbito = (mbito + dhmb) % 8;
        if (mbito == 0)
	   mbyto++;} 

/* if input HMB is assumed 0 (e.g. CRAY) 
 * but output HMB is assumed 1 and not written (e.g. IEEE) take the input
 * from HMB+1 and write it to output HMB
 */
    else if (dhmb < 0)
       {mbiti = (mbiti - dhmb) % 8;
        if (mbiti == 0)
	   mbyti++;}

    dmbit = mbiti - mbito;

    ls = 0;
    le = min(ni, MBLOCKS);
    for (l = 0; l < ni; )
        {lbpi  = l*bpii;
	 lbpo  = l*bpio;
	 lbpir = (lreorder == FALSE)*lbpi;

/* reorder the input into a buffer */
	 if (lreorder == TRUE)
	    {bufin = *in + lbpi - 1;
	     for (i = 0; i < bpii; i++)
	         {btemp = &bufin[inord[i]];
		  ctemp = lin + i;
		  for (j = ls; j < le; j++, btemp += bpii, ctemp += bpii)
		      *ctemp = *btemp;};};
     
	 peb  = (unsigned char *) lin  + ebyt  + lbpir;
	 psb  = (unsigned char *) lin  + sbyt  + lbpir;
	 pmbi = (unsigned char *) lin  + mbyti + lbpir;
	 pmbo = (unsigned char *) lout + mbyto + lbpo;
	 phmb = (unsigned char *) lout + hmbyt + lbpo;

/* get the sign bit, saving it in ssave */       
	 mask = 1 << (7 - sbit);
	 for (i = ls; i < le; i++, psb += bpii)
	     ssave[i] = *psb & mask;

	 inrem  = nbi_exp;
	 nbits  = 8 - ebit;
	 rshift = 0;
	 mask   = (1 << nbits) - 1;
	 if (inrem < 7)
	    {mask   = ((unsigned char) mask >> (7 - inrem));
	     rshift = 7 - inrem;}

	 SC_MEM_INIT_N(long, esave, le-ls);

	 pebsave = peb;

	 while (inrem > 0)

/* get the exponent */ 
	    {for (i = ls; i < le; i++, peb += bpii )
	         {esave[i] = (esave[i] << nbits) |
		             ((*peb >> rshift) & mask);};

	     inrem  -= nbits;
	     nbits   = min(8, inrem);
	     rshift  = 8 - nbits;
	     mask    = (1 << nbits) - 1;
	     peb     = pebsave + 1;
	     pebsave = peb;};

	 if (onescmp)
	    {for (i = ls; i < le; i++)
	         {if (ssave[i])
		     {ONES_COMP_NEG(esave[i], nbi_exp, 1L);}
		  else
		     esave[i] += (esave[i] < hexpn);};};

/* add the bias and store the exponent */
	 loverflow  = FALSE;
	 lunderflow = FALSE;
	 for (i = ls; i < le; i++, bo_exp += nbo, bo_sign += nbo)
	     {if (esave[i] != 0)
		 esave[i] += dbias;

	      if ((0 < esave[i]) && (esave[i] < emax))
		 {_PD_insert_field(esave[i], nbo_exp, lout, bo_exp,
				   ordl, bpl);
		  if (ssave[i]) 
		     _PD_set_bit(lout, bo_sign);}

	      else if (emax <= esave[i])
		 {loverflow = TRUE;
		  _PD_insert_field(emax, nbo_exp, lout, bo_exp,
				   ordl, bpl);
		  if (ssave[i])
		     _PD_set_bit(lout, bo_sign);}

	      else
		 lunderflow = TRUE;};

/* if input high mantissa bit (HMB) is assumed 1 and not written (e.g. IEEE)
 * but output HMB is assumed 0 (e.g. CRAY) write the input starting at
 * the output HMB+1 and set the HMB
 */
	 if (dhmb > 0)
	    {mask = 1 << (7 - hmbit);
	     pout = phmb;

	     if (loverflow || lunderflow)
	        {for (i = ls; i < le; i++, pout += bpio)
		     {if ((0 < esave[i]) && (esave[i] < emax))
			 *pout |= mask;};}
	     else
	        {for (i = ls; i < le; i++, pout += bpio)
		     *pout |= mask;};};

	 nbitsout = 8 - mbito;
	 mask     = (1 << nbitsout) - 1;
	 pout     = pmbo;
	 pin      = pmbi;
	 inrem    = nbi_man;
	 outrem   = nbo_man;
	 if (dhmb > 0)
	    outrem -= dhmb;
	 else if (dhmb < 0)
	    inrem += dhmb;

/* will the chunk of IN mantissa in first byte fit in the first byte
 * of OUT mantissa?
 */ 
	 if (mbiti >= mbito)

/* it will fit - store it */
	    {if (loverflow || lunderflow)
	        {if (onescmp)
		    {cmask = (1 << (8 - mbiti)) - 1;
		     for (i = ls; i < le; i++, pout += bpio, pin += bpii)
		         {if ((0 < esave[i]) && (esave[i] < emax))
			     {if (ssave[i])
				 *pout |= (~(pin[0] & cmask) << dmbit) & mask;
			      else
				 *pout |= (pin[0] << dmbit) & mask;};};}
		 else
		    {for (i = ls; i < le; i++, pout += bpio, pin += bpii)
		         {if ((0 < esave[i]) && (esave[i] < emax))
			     *pout |= (pin[0] << dmbit)  & mask;};};}

/* no overflow */
	     else
	        {if (onescmp)
		    {cmask = (1 << (8 - mbiti)) - 1;
		     for (i = ls; i < le; i++, pout += bpio, pin += bpii)
		         {if (ssave[i])
			     *pout |= (~(pin[0] & cmask) << dmbit) & mask;
			  else
			     *pout |= (pin[0] << dmbit) & mask;};}
		 else
		    {for (i = ls; i < le; i++, pout += bpio, pin += bpii)
		         *pout |= (pin[0] << dmbit)  & mask;};};
 
/* fill in the rest of the first output byte with the first
 * part of the second input byte
 */
	     numleft   = dmbit;
	     remainder = 0;
	     if (numleft > 0)
	        {mask      = (1 << numleft) - 1;
		 pin       = ++pmbi;
		 pout      = pmbo;
		 remainder = 8 - numleft;
		 if (loverflow || lunderflow)
		    {if (onescmp)
		        {for (i = ls; i < le; i++, pout += bpio, pin += bpii)
			     {if ((0 < esave[i]) && (esave[i] < emax))
				 {if (ssave[i])
				     *pout |= ~(pin[0] >> remainder) & mask;
				  else
				     *pout |=  (pin[0] >> remainder) & mask;};};}
		     else
		        {for (i = ls; i < le; i++, pout += bpio, pin += bpii)
			     {if ((0 < esave[i]) && (esave[i] < emax))
				 *pout |= (pin[0] >> remainder) & mask;};};}
		 else 
		    {if (onescmp)
		        {for (i = ls; i < le; i++, pout += bpio, pin += bpii)
			     {if (ssave[i])
				 *pout |= ~(pin[0] >> remainder) & mask;
			      else
				 *pout |=  (pin[0] >> remainder) & mask;};}
		     else
		        {for (i = ls; i < le; i++, pout += bpio, pin += bpii)
			     *pout |= (pin[0] >> remainder) & mask;};};};}

/* mbiti < mbito */
	 else

/* no it won't fit */
	    {remainder = mbito - mbiti; 
	     if (loverflow || lunderflow)
	        {if (onescmp)
		    {for (i = ls; i < le; i++, pout += bpio, pin += bpii)
		         {if ((0 < esave[i]) && (esave[i] < emax))
			     {if (ssave[i])
				 *pout |= ~(pin[0] >> remainder) & mask;
			      else
				 *pout |=  (pin[0] >> remainder) & mask;};};}
		 else
		    {for (i = ls; i < le; i++, pout += bpio, pin += bpii)
		         {if ((0 < esave[i]) && (esave[i] < emax))
			     *pout |= (pin[0] >> remainder) & mask;};};}
	     else
	        {if (onescmp)
		    {for (i = ls; i < le; i++, pout += bpio, pin += bpii)
		         {if (ssave[i])
			     *pout |= ~(pin[0] >> remainder) & mask;
			  else
			     *pout |=  (pin[0] >> remainder) & mask;};}
		 else
		    {for (i = ls; i < le; i++, pout += bpio, pin += bpii)
		         *pout |= (pin[0] >> remainder) & mask;};};};

	 inrem  -= nbitsout;
	 outrem -= nbitsout;          

/* begin storing to the beginning of the next sink byte
 * from the byte with the remainder
 */
	 pout = ++pmbo;
	 pin  = pmbi;

/* how many full bytes of mantissa left? */
	 nb_mant_rem = inrem >> 3;
	 nb_mant_rem = min(nb_mant_rem, outrem >> 3);
	 mask1 = (1 << remainder) -1;
	 mask2 = (1 << (8 - remainder)) -1;
       
	 for (k = 0; k < nb_mant_rem; k++)
	     {pout = pmbo + k;
	      pin  = pmbi  + k;
	      if (loverflow || lunderflow)
		 {if (onescmp)

/* move the mantissa over bytewise */
		     {for (i = ls; i < le; i++, pout += bpio, pin += bpii)
		          {if ((0 < esave[i]) && (esave[i] < emax))
			      {if (ssave[i])
				  *pout = ((~pin[0] & mask1) << (8 - remainder)) |
				            (~(pin[1] >> remainder) & mask2);
			       else
				  *pout = ((pin[0] & mask1) << (8 - remainder)) |
				            ((pin[1] >> remainder) & mask2);};};}

/* move the mantissa over bytewise */
		  else
		     {for (i = ls; i < le; i++, pout += bpio, pin += bpii)
		          {if ((0 < esave[i]) && (esave[i] < emax))
			      *pout = ((pin[0] & mask1) << (8 - remainder)) |
				        ((pin[1] >> remainder) & mask2);};};}
	      else
		 {if (onescmp)

/* move the mantissa over bytewise */
		     {for (i = ls; i < le; i++, pout += bpio, pin += bpii)
		          {if (ssave[i])
			      *pout = ((~pin[0] & mask1) << (8 - remainder)) |
			                (~(pin[1] >> remainder) & mask2);
			   else
			      *pout = ((pin[0] & mask1) << (8 - remainder)) |
			                ((pin[1] >> remainder) & mask2);};}
		  else

/* move the mantissa over bytewise */
		     {for (i = ls; i < le; i++, pout += bpio, pin += bpii)
		          {*pout = ((pin[0] & mask1) << (8 - remainder)) |
			             ((pin[1] >> remainder) & mask2);};};};}

	 pmbo    += nb_mant_rem;
	 pmbi    += nb_mant_rem;
	 nbitsout = (nb_mant_rem << 3);
	 inrem   -= nbitsout;
	 outrem  -= nbitsout;

/* store the last bits */
	 nleft = min(inrem, outrem);
	 if (nleft)
	    {mask = ((1 << nleft) - 1) << (8 - nleft);
	     pout = pmbo;
	     pin  = pmbi;
	     if (loverflow || lunderflow)
	        {if (onescmp)
		    {for (i = ls; i < le; i++, pout += bpio, pin += bpii)
		         {if ((0 < esave[i]) && (esave[i] < emax))
			     {if (ssave[i])
				 *pout = ~(pin[0] << (8 - remainder)) & mask;
			      else
				 *pout =  (pin[0] << (8 - remainder)) & mask;};};}
		 else
		    {for (i = ls; i < le; i++, pout += bpio, pin += bpii)
		         {if ((0 < esave[i]) && (esave[i] < emax))
			     *pout = (pin[0] << (8 - remainder)) & mask;};};}
	     else
	        {if (onescmp)
		    {for (i = ls; i < le; i++, pout += bpio, pin += bpii)
		         {if (ssave[i])
			     *pout = ~(pin[0] << (8 - remainder)) & mask;
			  else
			     *pout = (pin[0] << (8 - remainder)) & mask;};}
		 else
		    {for (i = ls; i < le; i++, pout += bpio, pin += bpii)
		         *pout = (pin[0] << (8 - remainder)) & mask;};};};

	 l += le - ls;
	 le = min(ni-l, MBLOCKS);};

/* handle CRAY inconsistency which has zero as the only floating point
 * number with a 0 in the HMB
 * also problem for IEEE 96 bit float - fixed by Dave Munro
 */
    if (hmbo != 0)
       {int j, mask;

	mask = (1 << (7 - bo_mant % 8));

        indxout = outfor[5]/8;
        rout    = (unsigned char *) *out;
        for (i = 0L; i < ni; i++, rout += bpio)
            {for (j = 0; j < bpio; j++)
                 if ((j == indxout) ? (rout[j] != mask) : rout[j])
                    break;
             if (j == bpio)
                rout[indxout] = 0;};};

    if (*in == freorder)
       {CFREE(freorder);
        *in = inrsave;};

    if (*in == aligned)
       {CFREE(aligned);
        *in = insave;};

    if (out_freorder == TRUE)
       {freorder = CMAKE_N(char, bpio*ni);
        tformat[0] = outfor[0];
        tformat[1] = outfor[1];
        tformat[2] = outfor[2];
        tformat[3] = bo_sign_save;
        tformat[4] = bo_exp_save;
        tformat[5] = bo_mant_save;
        _PD_field_reorder(*out, freorder, tformat, outfor, outord,
                          ordl, bpl, ni);
        memcpy(*out, freorder, ni*bpio);
        CFREE(freorder);};


/* put the output bytes into the specified order */
    _PD_reorder(*out, ni, bpio, outord);

    *in  += ni*bpii;
    *out += ni*bpio;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CONVERT - convert primitive types from one machine format to another
 *             - guaranteed that there will be no indirects here
 *             - convert NI items of type, TYPE, from IN and put them in OUT
 *             - ISTD defines the data format of the data from IN
 *             - PIN_OFFS and POUT_OFFS are pointers to external offset
 *             - counters and they are incremented to account for
 *             - data alignment
 *             - returns TRUE iff successful
 */

static int _PD_convert(char **out, char **in, inti ni, int boffs,
		       defstr *idp, defstr *odp,
		       data_standard *istd, data_standard *ostd,
		       data_standard *hstd,
		       inti *pin_offs, inti *pout_offs)
   {int onescmp, usg;
    int iusg, ousg, ret, rdx;
    int *intord, *iaord, *oaord;
    inti i, inb, onb, nbi, nbo;
    intb inbts, lnby;
    long *ifmt, *ofmt;
    char *inty, *outty, *delim;
    PD_type_kind iknd, oknd;
    PD_byte_order isord, osord, lsord, inord, outord;

    inty    = idp->type;
    iknd    = idp->kind;
    inb     = idp->size;
    iaord   = idp->fp.order;
    isord   = idp->fix.order;
    ifmt    = idp->fp.format;
    iusg    = idp->unsgned;
    inbts   = idp->size_bits;
    onescmp = idp->onescmp;

    outty   = odp->type;
    oknd    = odp->kind;
    onb     = odp->size;
    oaord   = odp->fp.order;
    osord   = odp->fix.order;
    ofmt    = odp->fp.format;
    ousg    = odp->unsgned;

    if ((strchr(inty, '*') != NULL) || (strchr(outty, '*') != NULL))
       return(FALSE);

    usg = (!iusg && !ousg) ? FALSE : TRUE;

    inord  = istd->fx[PD_LONG_I].order;
    outord = ostd->fx[PD_LONG_I].order;
    lsord  = hstd->fx[PD_LONG_I].order;
    lnby   = hstd->fx[PD_LONG_I].bpi;

    ret = TRUE;

/* sometimes this should be clear from the text (e.g. 0xfff)
 * sometimes it should be specified by the caller (e.g. 101 - binary, octal, ...?)
 */
    nbi = inb*ni;
    nbo = onb*ni;

    if (hstd->file != NULL)
       {rdx   = hstd->file->radix;
	delim = hstd->file->delim;}
    else
       {rdx   = 10;
	delim = NULL;};

/* text to binary conversions */
    if (inord == TEXT_ORDER)
       {_PD_text_bin(out, in, idp->type, ni, boffs,
		     iknd, ifmt, inb, isord, iaord,
		     oknd, ofmt, onb, osord, oaord,
		     hstd, onescmp, usg,
		     rdx, delim);

        *pin_offs  += nbi;
        *pout_offs += nbo;}

/* binary to text conversions */
    else if (outord == TEXT_ORDER)
       {_PD_bin_text(out, in, idp->type, ni, boffs,
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
       {_PD_ncopy(out, in, ni, inb);

        *pin_offs  += nbi;
        *pout_offs += nbo;}

/* handle floating point conversions */
    else if ((ifmt != NULL) && (ofmt != NULL))
       {_PD_fconvert(out, in, ni, boffs, ifmt, iaord,
                     ofmt, oaord, lsord, lnby, onescmp);

        *pin_offs  += nbi;
        *pout_offs += nbo;}

/* handle integral bitstreams */
    else if (inbts > 0)
       {int ityp;

	ityp = SC_type_container_size(KIND_INT, onb);
	if (ityp == SC_UNKNOWN_I)
	   return(FALSE);

/* unpack the bitstream into a bytestream */
	ret = SC_unpack_bits(*out, *in, ityp, inbts,
			     0, ni, ni, boffs);

/* convert binary characters */
        if (strcmp(inty, SC_CHAR_S) == 0)
	   _PD_conv_to_ascii_7(*out, ni, inbts);

/* convert integers */
        else
	   {intord = CMAKE_N(int, onb);
            if (osord == NORMAL_ORDER)
               for (i = 0; i < onb; intord[i] = i + 1, i++);
            else                         
               for (i = 0; i < onb; intord[i] = onb - i, i++);

            if (usg == FALSE)
      	        _PD_sign_extend(*out, ni, onb, inbts, intord);

            if (onescmp)
               _PD_ones_complement(*out, ni, onb, intord);

            CFREE(intord);

	    *in        += nbi;
	    *out       += nbo;
	    *pin_offs  += nbi;
	    *pout_offs += nbo;};}

/* handle integer conversions */
    else if ((isord != NO_ORDER) && (osord != NO_ORDER))
       {_PD_iconvert(out, in, ni,
                     (int) inb, isord,
                     (int) onb, osord, onescmp, usg);

        *pin_offs  += nbi;
        *pout_offs += nbo;}

/* handle character or unconverted types */
    else
       {_PD_ncopy(out, in, ni, inb);

        *pin_offs  += nbi;
        *pout_offs += nbo;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_CONVERT - convert from one machine format to another NI of
 *            - type, TYPE, from IN and put them in OUT
 *            - ISTD and OSTD are the data format standards of IN and 
 *            - OUT respectively
 *            - return TRUE iff successful
 *
 * #bind PD_convert fortran() scheme() python()
 */

int PD_convert(char **out, char **in, char *typi, char *typo,
	       int64_t ni, data_standard *stdi, data_standard *stdo,
	       data_standard *hstd, hasharr *chi, hasharr *cho,
	       int boffs, PD_major_op error)
   {int ret, tmp;
    inti i, mitems, inci, inco;
    inti in_offs, out_offs;
    char msg[MAXLINE];
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
       {snprintf(msg, MAXLINE, "BAD IN TYPE, '%s' - PD_CONVERT", typi);
	PD_error(msg, error);};

    if (dpo == NULL)
       {snprintf(msg, MAXLINE, "BAD OUT TYPE, '%s' - PD_CONVERT", typo);
	PD_error(msg, error);}

/* convert pointers to data via /&ptrs mechanism */
    else if (dpo->is_indirect) 
       {inci = _PD_align(in_offs, typi, dpi->is_indirect, chi, &tmp);
        inco = _PD_align(out_offs, typo, dpo->is_indirect, cho, &tmp);

	ret = _PD_convert_ptr(out, in, &out_offs, &in_offs, ni,
			      cho, chi, stdo, stdi, hstd, inco, inci);}

/* if members is non-NULL then it is a derived type */
    else if (dpo->members != NULL)
       {for (i = 0L; i < ni; i++)
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
			 {snprintf(msg, MAXLINE,
				   "BAD OUT TYPE, '%s' IN STRUCT - PD_CONVERT",
				   mtype);
			  PD_error(msg, error);};

                      mdpi = PD_inquire_table_type(chi, mtype);
                      if (mdpi == NULL)
			 {snprintf(msg, MAXLINE,
				   "BAD IN TYPE, '%s' IN STRUCT - PD_CONVERT",
				   mtype);
			  PD_error(msg, error);};

		      ret = _PD_convert((char **) out, in, mitems, boffs,
					mdpi, mdpo, stdi, stdo, hstd,
					&in_offs, &out_offs);}

/* recurse for direct derived types */
                  else
                     ret = PD_convert(out, in, mtype, mtype, mitems,
                                      stdi, stdo, hstd,
                                      chi, cho, boffs, error);

                  if (ret == FALSE)
		     {snprintf(msg, MAXLINE,
			       "STRUCT CONVERSION FAILED FOR '%s' - PD_CONVERT",
			       typi);
		      PD_error(msg, error);};};};}

/* if members is NULL then it is a primitive type */
    else
       {ret = _PD_convert((char **) out, in, ni, boffs, dpi, dpo,
                          stdi, stdo, hstd, &in_offs, &out_offs);
        if (ret == FALSE)
	   {snprintf(msg, MAXLINE,
		     "PRIMITIVE CONVERSION FAILED FOR '%s' TO '%s' - PD_CONVERT",
		     typi, typo);
	    PD_error(msg, error);};};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_N_BIT_CHAR_STD - return the bit size correspondingn to CSTD */

int PD_n_bit_char_std(PD_character_standard cstd)
   {int nb;

    switch (cstd)
        {case PD_ITA2_UPPER :
	 case PD_ITA2_LOWER :
	      nb = 5;
	      break;
	 case PD_ASCII_6_UPPER :
	 case PD_ASCII_6_LOWER :
	      nb = 6;
	      break;
	 case PD_EBCDIC :
	 default :
	      nb = 8;
	      break;};

     return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_CONVERT_ASCII - convert character bit-stream in IN
 *                  - to standard 7-bit ASCII bytestream OUT
 *                  - IN is NB bytes long (NOTE: not characters long)
 *                  - CSTD is one of:
 *                  -   PD_ITA2_UPPER     upper case ITA2
 *                  -   PD_ITA2_LOWER     lower case ITA2
 *                  -   PD_ASCII_6_UPPER  upper case 6-bit ASCII
 *                  -   PD_ASCII_6_LOWER  lower case 6-bit ASCII
 *                  -   PD_ASCII_7        7-bit ASCII (no-op)
 *                  -   PD_UTF_8          8-bit UTF (no-op)
 *                  -   PD_EBCDIC         IBM EBCDIC 8-bit
 *
 * #bind PD_convert_ascii fortran() scheme() python()
 */

char *PD_convert_ascii(char *out, int nc, PD_character_standard cstd,
		       char *in, int64_t nb)
   {int n, ret;
    inti ni;

    switch (cstd)
       {case PD_ITA2_UPPER :
        case PD_ITA2_LOWER :
             n = 5;
	     break;
        case PD_ASCII_6_UPPER :
        case PD_ASCII_6_LOWER :
             n = 6;
	     break;
        default :
             n = 8;
	     break;};

    ni = (8*nb)/n;

/* unpack the bitstream into a bytestream */
    ret = SC_unpack_bits(out, in, SC_INT8_I, n, 0, ni, ni, 0);

    SC_ASSERT(ret == TRUE);

    _PD_conv_to_ascii_7(out, ni, cstd);

    return(out);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_CONV_FROM_ASCII - convert character bit-stream in IN
 *                    - to standard 7-bit ASCII bytestream OUT
 *                    - IN is NB bytes long (NOTE: not characters long)
 *                    - CSTD is one of:
 *                    -   PD_ITA2_UPPER     upper case ITA2
 *                    -   PD_ITA2_LOWER     lower case ITA2
 *                    -   PD_ASCII_6_UPPER  upper case 6-bit ASCII
 *                    -   PD_ASCII_6_LOWER  lower case 6-bit ASCII
 *                    -   PD_ASCII_7        7-bit ASCII (no-op)
 *                    -   PD_UTF_8          8-bit UTF (no-op)
 *                    -   PD_EBCDIC         IBM EBCDIC 8-bit
 *
 * #bind PD_conv_from_ascii fortran() scheme() python()
 */

char *PD_conv_from_ascii(char *out, int nc, PD_character_standard cstd,
			 char *in, int64_t nb)
   {inti n, nn, nx;
    char *t;

    n  = min(nc, nb);
    nx = max(2*nb, nc);

    switch (cstd)
       {case PD_ITA2_UPPER :
        case PD_ITA2_LOWER :
             t = CMAKE_N(char, nx);
	     memcpy(t, in, n);
	     memset(out, 0, nc);
             nn = _PD_conv_to_ita2(t, nx, n, PD_ASCII_7);
	     _PD_pack_bits(out, t, SC_CHAR_I, 5, 0, 1, nn, 0);
             CFREE(t);
	     break;
        case PD_ASCII_6_UPPER :
        case PD_ASCII_6_LOWER :
             t = CMAKE_N(char, n);
	     memcpy(t, in, n);
	     memset(out, 0, nc);
             _PD_conv_to_ascii_6(t, n, PD_ASCII_7);
	     _PD_pack_bits(out, t, SC_CHAR_I, 6, 0, 1, n, 0);
             CFREE(t);
	     break;
        case PD_EBCDIC :
	     memcpy(out, in, n);
             _PD_conv_to_ebcdic(out, n, PD_ASCII_7);
	     break;
        default :
	     memcpy(out, in, n);
	     break;};

    return(out);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CONV_ASCII_CHK - check conversions in and out of CTSD */

int _PD_conv_ascii_chk(PD_character_standard cstd)
   {int i, n, rv;
    char *in, *im, *out;

    n   = 128;
    in  = CMAKE_N(char, n);
    im  = CMAKE_N(char, n);
    out = CMAKE_N(char, n);
    for (i = 0; i < n; i++)
        in[i] = i;

    PD_conv_from_ascii(im, n, cstd, in, n);
    PD_convert_ascii(out, n, cstd, im, n);

    rv = 0;
    for (i = 0; i < n; i++)
        rv += (out[i] != in[i]);

    CFREE(in);
    CFREE(im);
    CFREE(out);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

