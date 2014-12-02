/*
 * PDB_FORMATS.H - spell out the data formats
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_FORMATS

#include "cpyright.h"

#define PCK_FORMATS

#include <pdform.h>

/*--------------------------------------------------------------------------*/
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


/*--------------------------------------------------------------------------*/

/*                             DATA_STANDARDS                               */

/*--------------------------------------------------------------------------*/

#define DEF_TEXT_STD                                                        \
     {BITS_DEFAULT,                               /* bits per byte */       \
      12,                                       /* size of pointer */       \
      1,                                           /* size of bool */       \
      {{1, UTF_8},                                    /* char info */       \
       {4, UTF_32}},                                 /* wchar info */       \
      {{4,  TEXT_ORDER},               /* size and order of int8_t */       \
       {7,  TEXT_ORDER},                /* size and order of short */       \
       {12, TEXT_ORDER},                  /* size and order of int */       \
       {22, TEXT_ORDER},                 /* size and order of long */       \
       {22, TEXT_ORDER}},           /* size and order of long long */       \
      {{15, float4_text, text_order},          /* float definition */       \
       {26, float8_text, text_order},         /* double definition */       \
       {42, float16_text, text_order}}}         /* quad definition */

#define DEF_I386_STD                                                        \
     {BITS_DEFAULT,                               /* bits per byte */       \
      4,                                        /* size of pointer */       \
      1,                                           /* size of bool */       \
      {{1, UTF_8},                                    /* char info */       \
       {4, UTF_32}},                                 /* wchar info */       \
      {{1, REVERSE_ORDER},             /* size and order of int8_t */       \
       {2, REVERSE_ORDER},              /* size and order of short */       \
       {2, REVERSE_ORDER},                /* size and order of int */       \
       {4, REVERSE_ORDER},               /* size and order of long */       \
       {4, REVERSE_ORDER}},         /* size and order of long long */       \
      {{4, float4_ieee, float4_rev_order},     /* float definition */       \
       {8, float8_ieee, float8_rev_order},    /* double definition */       \
       {16, float16_ieee, float16_rev_order}}}  /* quad definition */

#define DEF_I586L_STD                                                       \
     {BITS_DEFAULT,                               /* bits per byte */       \
      4,                                        /* size of pointer */       \
      1,                                           /* size of bool */       \
      {{1, UTF_8},                                    /* char info */       \
       {4, UTF_32}},                                 /* wchar info */       \
      {{1, REVERSE_ORDER},             /* size and order of int8_t */       \
       {2, REVERSE_ORDER},              /* size and order of short */       \
       {4, REVERSE_ORDER},                /* size and order of int */       \
       {4, REVERSE_ORDER},               /* size and order of long */       \
       {8, REVERSE_ORDER}},         /* size and order of long long */       \
      {{4, float4_ieee, float4_rev_order},     /* float definition */       \
       {8, float8_ieee, float8_rev_order},    /* double definition */       \
       {12, float12_ieee, float12_rev_order}}}  /* quad definition */

#define DEF_I586O_STD                                                       \
     {BITS_DEFAULT,                               /* bits per byte */       \
      4,                                        /* size of pointer */       \
      1,                                           /* size of bool */       \
      {{1, UTF_8},                                    /* char info */       \
       {4, UTF_32}},                                 /* wchar info */       \
      {{1, REVERSE_ORDER},             /* size and order of int8_t */       \
       {2, REVERSE_ORDER},              /* size and order of short */       \
       {4, REVERSE_ORDER},                /* size and order of int */       \
       {4, REVERSE_ORDER},               /* size and order of long */       \
       {8, REVERSE_ORDER}},         /* size and order of long long */       \
      {{4, float4_ieee, float4_rev_order},     /* float definition */       \
       {8, float8_ieee, float8_rev_order},    /* double definition */       \
       {16, float16_ieee, float16_rev_order}}}  /* quad definition */

#define DEF_X86_64_STD                                                      \
     {BITS_DEFAULT,                               /* bits per byte */       \
      8,                                        /* size of pointer */       \
      1,                                           /* size of bool */       \
      {{1, UTF_8},                                    /* char info */       \
       {4, UTF_32}},                                 /* wchar info */       \
      {{1, REVERSE_ORDER},             /* size and order of int8_t */       \
       {2, REVERSE_ORDER},              /* size and order of short */       \
       {4, REVERSE_ORDER},                /* size and order of int */       \
       {8, REVERSE_ORDER},               /* size and order of long */       \
       {8, REVERSE_ORDER}},         /* size and order of long long */       \
      {{4, float4_ieee, float4_rev_order},     /* float definition */       \
       {8, float8_ieee, float8_rev_order},    /* double definition */       \
       {16, float16_ieee, float16_rev_order}}}  /* quad definition */

#define DEF_X86_64A_STD                                                     \
     {BITS_DEFAULT,                               /* bits per byte */       \
      8,                                        /* size of pointer */       \
      1,                                           /* size of bool */       \
      {{1, UTF_8},                                    /* char info */       \
       {4, UTF_32}},                                 /* wchar info */       \
      {{1, REVERSE_ORDER},             /* size and order of int8_t */       \
       {2, REVERSE_ORDER},              /* size and order of short */       \
       {4, REVERSE_ORDER},                /* size and order of int */       \
       {8, REVERSE_ORDER},               /* size and order of long */       \
       {8, REVERSE_ORDER}},         /* size and order of long long */       \
      {{4, float4_ieee, float4_rev_order},     /* float definition */       \
       {8, float8_ieee, float8_rev_order},    /* double definition */       \
       {16, float10_ix87, float16_rev_order}}}  /* quad definition */

#define DEF_PPC32_STD                                                       \
     {BITS_DEFAULT,                               /* bits per byte */       \
      4,                                        /* size of pointer */       \
      1,                                           /* size of bool */       \
      {{1, UTF_8},                                    /* char info */       \
       {4, UTF_32}},                                 /* wchar info */       \
      {{1, NORMAL_ORDER},              /* size and order of int8_t */       \
       {2, NORMAL_ORDER},               /* size and order of short */       \
       {4, NORMAL_ORDER},                 /* size and order of int */       \
       {4, NORMAL_ORDER},                /* size and order of long */       \
       {8, NORMAL_ORDER}},          /* size and order of long long */       \
      {{4, float4_ieee, float4_nrm_order},     /* float definition */       \
       {8, float8_ieee, float8_nrm_order},    /* double definition */       \
       {16, float16_ieee, float16_nrm_order}}}  /* quad definition */

#define DEF_PPC64_STD                                                       \
     {BITS_DEFAULT,                               /* bits per byte */       \
      8,                                        /* size of pointer */       \
      1,                                           /* size of bool */       \
      {{1, UTF_8},                                    /* char info */       \
       {4, UTF_32}},                                 /* wchar info */       \
      {{1, NORMAL_ORDER},              /* size and order of int8_t */       \
       {2, NORMAL_ORDER},               /* size and order of short */       \
       {4, NORMAL_ORDER},                 /* size and order of int */       \
       {8, NORMAL_ORDER},                /* size and order of long */       \
       {8, NORMAL_ORDER}},          /* size and order of long long */       \
      {{4, float4_ieee, float4_nrm_order},     /* float definition */       \
       {8, float8_ieee, float8_nrm_order},    /* double definition */       \
       {16, float16_ieee, float16_nrm_order}}}  /* quad definition */

/* historical formats - kept for testing */

#define DEF_M68X_STD                                                        \
     {BITS_DEFAULT,                               /* bits per byte */       \
      4,                                        /* size of pointer */       \
      1,                                           /* size of bool */       \
      {{1, UTF_8},                                    /* char info */       \
       {2, UTF_16}},                                 /* wchar info */       \
      {{1, NORMAL_ORDER},              /* size and order of int8_t */       \
       {2, NORMAL_ORDER},               /* size and order of short */       \
       {2, NORMAL_ORDER},                 /* size and order of int */       \
       {4, NORMAL_ORDER},                /* size and order of long */       \
       {4, NORMAL_ORDER}},          /* size and order of long long */       \
      {{4, float4_ieee, float4_nrm_order},     /* float definition */       \
       {12, float12_ieee, float12_nrm_order}, /* double definition */       \
       {16, float16_ieee, float16_nrm_order}}}  /* quad definition */

#define DEF_VAX_STD                                                         \
     {BITS_DEFAULT,                               /* bits per byte */       \
      4,                                        /* size of pointer */       \
      1,                                           /* size of bool */       \
      {{1, UTF_8},                                    /* char info */       \
       {2, UTF_16}},                                 /* wchar info */       \
      {{1, REVERSE_ORDER},             /* size and order of int8_t */       \
       {2, REVERSE_ORDER},              /* size and order of short */       \
       {4, REVERSE_ORDER},                /* size and order of int */       \
       {4, REVERSE_ORDER},               /* size and order of long */       \
       {4, REVERSE_ORDER}},         /* size and order of long long */       \
      {{4, float4_vax, float4_vax_order},      /* float definition */       \
       {8, float8_vax, float8_vax_order},     /* double definition */       \
       {8, float8_vax, float8_vax_order}}}      /* quad definition */

#define DEF_CRAY_STD                                                        \
     {BITS_DEFAULT,                               /* bits per byte */       \
      8,                                        /* size of pointer */       \
      1,                                           /* size of bool */       \
      {{1, UTF_8},                                    /* char info */       \
       {2, UTF_16}},                                 /* wchar info */       \
      {{8, NORMAL_ORDER},              /* size and order of int8_t */       \
       {8, NORMAL_ORDER},               /* size and order of short */       \
       {8, NORMAL_ORDER},                 /* size and order of int */       \
       {8, NORMAL_ORDER},                /* size and order of long */       \
       {8, NORMAL_ORDER}},          /* size and order of long long */       \
      {{8, float8_cray, float8_nrm_order},     /* float definition */       \
       {8, float8_cray, float8_nrm_order},    /* double definition */       \
       {16, float16_cray, float16_nrm_order}}}  /* quad definition */

/*--------------------------------------------------------------------------*/

/*                            DATA_ALIGNMENTS                               */

/*--------------------------------------------------------------------------*/

#define DEF_TEXT_ALIGN         {0, 0, {0, 0}, {0, 0, 0, 0, 0}, {0, 0,  0}, 0}
#define DEF_BYTE_ALIGN         {1, 1, {1, 1}, {1, 1, 1, 1, 1}, {1, 1,  1}, 0}
#define DEF_WORD2_ALIGN        {2, 1, {1, 2}, {1, 2, 2, 2, 2}, {2, 2,  2}, 0}
#define DEF_WORD4_ALIGN        {4, 1, {1, 4}, {1, 4, 4, 4, 4}, {4, 4,  4}, 0}
#define DEF_WORD8_ALIGN        {8, 1, {4, 8}, {4, 8, 8, 8, 8}, {8, 8,  8}, 8}
#define DEF_GNU4_I686_ALIGN    {4, 1, {1, 4}, {1, 2, 4, 4, 4}, {4, 4,  4}, 0}
#define DEF_OSX_10_5_ALIGN     {4, 1, {1, 4}, {1, 2, 4, 4, 4}, {4, 4, 16}, 0}
#define DEF_SPARC_ALIGN        {4, 1, {1, 4}, {1, 2, 4, 4, 4}, {4, 8,  8}, 0}
#define DEF_XLC32_PPC64_ALIGN  {4, 1, {1, 4}, {1, 2, 4, 4, 8}, {4, 4,  4}, 0}
#define DEF_CYGWIN_I686_ALIGN  {4, 1, {1, 4}, {1, 2, 4, 4, 8}, {4, 8,  4}, 0}
#define DEF_GNU3_PPC64_ALIGN   {4, 1, {1, 4}, {1, 2, 4, 4, 8}, {4, 8,  8}, 0}
#define DEF_GNU4_PPC64_ALIGN   {4, 1, {1, 4}, {1, 2, 4, 4, 8}, {4, 8, 16}, 0}
#define DEF_XLC64_PPC64_ALIGN  {8, 1, {1, 4}, {1, 2, 4, 8, 8}, {4, 4,  4}, 0}
#define DEF_PGI_X86_64_ALIGN   {8, 1, {1, 4}, {1, 2, 4, 8, 8}, {4, 8,  8}, 0}
#define DEF_GNU4_X86_64_ALIGN  {8, 1, {1, 4}, {1, 2, 4, 8, 8}, {4, 8, 16}, 0}


#endif

