/*
 * DETECT.C - decipher the basic C binary data formats
 *          - adapted and completed from the Dave Munro original
 *          - this is constructed to be absolutely vanilla C and
 *          - should compile correctly with ANSI compilers
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#undef LONGLONG_MAX

/* GOTCHA: clean up this mess */

#include "../psh/posix.h"

#include "scstd.h"
#include "scope_typeh.h"

#undef LIBFIO
#undef SCOPE_SCORE_PREPROC
#define VA_START  SC_VA_START
#define VSNPRINTF(s, n, f) vsnprintf(s, n, f, __a__)
#define VA_END    SC_VA_END
#include "../psh/libfio.c"

#define DBFSZ                  512
#define BITS_DEFAULT             8

#define I_BOOL                   0
#define I_CHAR                   1
#define I_WCHAR                  2
#define I_INT8                   3
#define I_SHORT                  4
#define I_INT                    5
#define I_LONG                   6
#define I_LONG_LONG              7
#define I_FLOAT                  8
#define I_DOUBLE                 9
#define I_LONG_DOUBLE           10
#define I_FLOAT_COMPLEX         11
#define I_DOUBLE_COMPLEX        12
#define I_LONG_DOUBLE_COMPLEX   13
#define I_POINTER               14
#define I_STRUCT                15


#define TYPE_SET(_i, _t, _v)     type_set(_i, sizeof(_t), sizeof(_v))

/* these structs will help determine alignment of the primitive types */
struct cbool
   {char c;
    bool b;} cb;

struct cchar
   {char c;
    char x;} cc;
struct cwchar
   {char c;
    wchar_t x;} cw;

struct cint8
   {char c;
    int8_t x;} ci8;
struct cshort
   {char c;
    short x;} cs;
struct cint
   {char c;
    int x;} ci;
struct clong
   {char c;
    long x;} cl;
struct clonglong
   {char c;
    int64_t x;} cll;

struct cfloat4
   {char c;
    float x;} cf;
struct cfloat8
   {char c;
    double x;} cd;
struct cfloat16
   {char c;
    long double x;} cld;

struct ccomplex4
   {char c;
    float complex x;} cfc;
struct ccomplex8
   {char c;
    double complex x;} cdc;
struct ccomplex16
   {char c;
    long double complex x;} clc;

struct cptr
   {char c;
    void *x;} cp;

struct cint16
   {char c;
    int16_t x;} ci16;
struct cint32
   {char c;
    int32_t x;} ci32;
struct cint64
   {char c;
    int64_t x;} ci64;

/* some machines have an additional alignment for structs
 * this struct will test for such a "feature"
 */
struct cstruct
   {struct LevOne
       {char one;} a;
    struct LevTwo
       {char two;} b;} ct;

/* this union will be used to determine the integer types
 * parameters especially the byte order
 */

union ucsil
   {unsigned char c[32];
    short s[2];
    int i[2];
    long l[2];} bo;

static int 
 size[N_TYPES],
 ssize[N_TYPES],
 align[N_TYPES];

static char
 int_order[80];
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TYPE_SET - set some type characteristics */

static void type_set(int i, int nbt, int nbv)
   {

    size[i]  = nbt;
    ssize[i] = nbv;

/* first possibility: align <= size (usual case)
 * alignment is difference between struct length and member size:
 */
    align[i] = ssize[i] - size[i];

/* second possibility: align > size (e.g. Cray char)
 * alignment is half of structure size:
 */
    if (align[i] > (ssize[i] >> 1))
       align[i] = ssize[i] >> 1;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DIFFER_BYTE - find the location of the differing byte */

int differ_byte(int n, int k, int *fb,
                unsigned char *a, unsigned char *b)
   {int i;

    fb[k] = 0;

    for (i = 0; i < n; i++)
        {if (a[i] != b[i])
            {fb[k] = i + 1;
             break;};};

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIX_ORDER - determine the byte order assuming that the mantissa byte
 *           - pattern implies the entire ordering
 */

int fix_order(int n, int *fb, int first, int last)
   {int i;
    static int ordering = -1;

    if ((ordering == -1) && (last > first + 1))

/* left to right or normal ordering */
       {if ((fb[last] > fb[last-1]) && (fb[last-1] > fb[last-2]))
           ordering = 0;

/* right to left or reverse ordering (INTEL) */
        else if ((fb[last] < fb[last-1]) && (fb[last-1] < fb[last-2]))
           ordering = 1;

/* middle endian machine (VAX) */
        else
           ordering = 2;};

    switch (ordering)

/* left to right or normal ordering */
       {case 0 :
             for (i = 0; i < n; i++)
                 fb[i] = i + 1;
             break;

/* right to left or reverse ordering (INTEL) */
        case 1 :
             for (i = 0; i < n; i++)
	         fb[i] = n - i;
             break;

/* middle endian machine (VAX)
 * NOTE: this assumes that only the VAX does this!
 */
        case 2 :
             for (i = 0; i < n; i += 2)
                 {fb[i]   = n + 2;
                  fb[i+1] = n + 1;};
             break;};

    return(1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GUARD_BIT - check for the existence of a mantissa guard bit */

int guard_bit(int n, int k, int *fb, long *ff,
              unsigned char *a, unsigned char *b)
   {int i, j, l;
    unsigned char byt, nxt;

    ff[k] = 0L;

/* xor the two arrays - they should differ by one bit */
    for (i = 0; i < n; i++)
        {l   = fb[i] - 1;
         byt = a[l] ^ b[l];
         if (byt == 1)
            {if (b[l+1])
                {ff[k] = 1L;
                 return(1);};}

         else if (byt != 0)

/* find the location of the bit */
             for (j = 0; j < 8; j++)
                 {if (byt == 0x80)

/* if the next bit of the second one is on there is a guard bit */
                     {nxt = b[l] << (j+1);
                      if (nxt)
                         {ff[k] = 1L;
                          return(1);};};

                  byt <<= 1;};};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DIFFER_BIT - find the location of the differing bit */

int differ_bit(int n, int k, int *fb, long *ff,
               unsigned char *a, unsigned char *b)
   {unsigned char byt;
    int i, j, l;

    for (i = 0; i < n; i++)
        {l = fb[i] - 1;
         if (a[l] != b[l])
            {byt = a[l] ^ b[l];
             for (j = 0; j < 8; j++)
                 {if (byt == 0x80)
                     {ff[k] = j + i*8;
                      break;};
                  byt <<= 1;};};};

    return(1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIND_EXP - find the exponent bias */

int find_exp(int *fb, long *ff, unsigned char *a)
   {int i, l, nbits, sbyte, ebyte;
    long lexp;

    sbyte = ff[4]/8;
    ebyte = ff[5]/8;
    lexp  = 0L;
    for (i = sbyte; i <= ebyte; i++)
        {l = fb[i] - 1;
         lexp <<= 8;
         lexp |= a[l];};

    nbits  = (ebyte + 1)*8 - ff[5];
    lexp >>= nbits;

    ff[7] = lexp;

    return(1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DERIVE_FLOAT - figure out the float format */

int derive_float(int *fb, long *ff)
   {int j, szf, first, last;
    union ucf {unsigned char c[32]; float f[2];} bofa, bofb, bofc;
    float fval, dfv;

    szf = sizeof(float);

    ff[0]  = szf*8;

/* get the byte ordering from the float mantissa bytes */
    fval  = 0.0;
    dfv   = 1.0;
    first = -1;
    last  = -1;
    for (j = 1; j < szf; j++)
        {bofb.f[0] = fval;

         fval += dfv;
         dfv  /= 256.0;
         bofa.f[0] = fval;

         if (j > 1)
            {if (differ_byte(szf, j, fb, bofb.c, bofa.c) < szf)
                last = j;
             if (first == -1)
                first = last;};};

    fix_order(szf, fb, first, last);

/* find the mantissa guard bit for existence */
    bofa.f[0] = 0.5;
    bofb.f[0] = 1.0;
    guard_bit(szf, 6, fb, ff, bofa.c, bofb.c);

/* find the sign bit */
    bofa.f[0] = -1.0;
    bofb.f[0] = 1.0;
    differ_bit(szf, 3, fb, ff, bofa.c, bofb.c);

/* find loc(exponent) */
    ff[4]  = ff[3] + 1;

/* find loc(mantissa) */
    bofc.f[0] = 1.5;
    differ_bit(szf, 5, fb, ff, bofc.c, bofb.c);

/* adjust loc(mantissa) */
    ff[5]  -= ff[6];

/* #exponent_bits = loc(mantissa) - loc(exponent) */
    ff[1]  = ff[5]  - ff[4];

/* #mantissa_bits = #bits - #exponent_bits - loc(sign) - 1 */
    ff[2]  = ff[0]  - ff[1]  - ff[3] - 1;

/* find exponent bias */
    find_exp(fb,  ff,  bofb.c);

/* adjust exponent bias */
    ff[7]  -= ff[6];

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DERIVE_DOUBLE - figure out the double format */

int derive_double(int *db, long *df)
   {int j, szd, first, last;
    union ucd {unsigned char c[32]; double d[2];} boda, bodb, bodc;
    double dval, ddv;

    szd = sizeof(double);

    df[0]  = szd*8;

/* get the byte ordering from the double mantissa bytes */
    dval  = 0.0;
    ddv   = 1.0;
    first = -1;
    last  = -1;
    for (j = 1; j < szd; j++)
        {bodb.d[0] = dval;

         dval += ddv;
         ddv  /= 256.0;
         boda.d[0] = dval;

         if (j > 1)
            {if (differ_byte(szd, j, db, bodb.c, boda.c) < szd)
                last = j;
             if (first == -1)
                first = last;};};

    fix_order(szd, db, first, last);

/* find the mantissa guard bit for existence */
    boda.d[0] = 0.5;
    bodb.d[0] = 1.0;
    guard_bit(szd, 6, db, df, boda.c, bodb.c);

/* find the sign bit */
    boda.d[0] = -1.0;
    bodb.d[0] = 1.0;
    differ_bit(szd, 3, db, df, boda.c, bodb.c);

/* find loc(exponent) */
    df[4]  = df[3] + 1;

/* find loc(mantissa) */
    bodc.d[0] = 1.5;
    differ_bit(szd, 5, db, df, bodc.c, bodb.c);

/* adjust loc(mantissa) */
    df[5]  -= df[6];

/* #exponent_bits = loc(mantissa) - loc(exponent) */
    df[1]  = df[5]  - df[4];

/* #mantissa_bits = #bits - #exponent_bits - loc(sign) - 1 */
    df[2]  = df[0]  - df[1]  - df[3] - 1;

/* find exponent bias */
    find_exp(db,  df,  bodb.c);

/* adjust exponent bias */
    df[7]  -= df[6];

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DERIVE_QUAD - figure out the long double format */

int derive_quad(int *ldb, long *ldf)
   {int j, szl, first, last;
    union ucl {unsigned char c[32]; long double l[2];} bola, bolb, bolc;
    long double lval, ldv;

    szl = sizeof(long double);

    ldf[0] = szl*8;

/* get the byte ordering from the long double mantissa bytes */
    lval  = 0.0;
    ldv   = 1.0;
    first = -1;
    last  = -1;
    for (j = 1; j < szl; j++)
        {bolb.l[0] = lval;

         lval += ldv;
         ldv  /= 256.0;
         bola.l[0] = lval;

         if (j > 1)
            {if (differ_byte(szl, j, ldb, bolb.c, bola.c) < szl)
                last = j;
             if (first == -1)
                first = last;};};

    fix_order(szl, ldb, first, last);

/* find the mantissa guard bit for existence */
    bola.l[0] = 0.5;
    bolb.l[0] = 1.0;
    guard_bit(szl, 6, ldb, ldf, bola.c, bolb.c);

/* find the sign bit */
    bola.l[0] = -1.0;
    bolb.l[0] = 1.0;
    differ_bit(szl, 3, ldb, ldf, bola.c, bolb.c);

/* find loc(exponent) */
    ldf[4] = ldf[3] + 1;

/* find loc(mantissa) */
    bolc.l[0] = 1.5;
    differ_bit(szl, 5, ldb, ldf, bolc.c, bolb.c);

/* adjust loc(mantissa) */
    ldf[5] -= ldf[6];

/* #exponent_bits = loc(mantissa) - loc(exponent) */
    ldf[1] = ldf[5] - ldf[4];

/* #mantissa_bits = #bits - #exponent_bits - loc(sign) - 1 */
    ldf[2] = ldf[0] - ldf[1] - ldf[3] - 1;

/* find exponent bias */
    find_exp(ldb, ldf, bolb.c);

/* adjust exponent bias */
    ldf[7] -= ldf[6];

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DERIVE_FP_FORMAT - figure out the floating point format */

int derive_fp_format(int *fb, int *db, int *ldb, long *ff, long *df, long *ldf)
   {

    derive_double(db, df);
    derive_quad(ldb, ldf);
    derive_float(fb, ff);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DERIVE_COMPLEX_FLOAT - figure out the float format */

int derive_complex_float(int *fc)
   {int i, szc, szf, iprt, rprt;
    union ucf {unsigned char c[32]; float _Complex f[2];} bofa;

    szc = sizeof(float _Complex);
    szf = sizeof(float);

    fc[0] = szc;
    fc[1] = szf;

/* get the part ordering from the float complex */
    bofa.f[0] = 1.0;
    bofa.f[1] = I;

/* treated as an index into an array of 2 floats find
 * the real part
 */
    for (i = 0; i < szf; i++)
        {if (bofa.c[i] != 0)
	    break;};
    rprt = (i >= szf);

/* the imaginary part */
    for (i = 0; i < szf; i++)
        {if (bofa.c[i+szc] != 0)
	    break;};
    iprt = (i >= szf);

    fc[2] = rprt;
    fc[3] = iprt;

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DERIVE_COMPLEX_DOUBLE - figure out the double format */

int derive_complex_double(int *dc)
   {int i, szd, szc, iprt, rprt;
    union ucd {unsigned char c[32]; double _Complex d[2];} boda;

    szc = sizeof(double _Complex);
    szd = sizeof(double);

    dc[0] = szc;
    dc[1] = szd;

/* get the part ordering from the double complex */
    boda.d[0] = 1.0;
    boda.d[1] = I;

/* treated as an index into an array of 2 doubles find
 * the real part
 */
    for (i = 0; i < szd; i++)
        {if (boda.c[i] != 0)
	    break;};
    rprt = (i >= szd);

/* the imaginary part */
    for (i = 0; i < szd; i++)
        {if (boda.c[i+szc] != 0)
	    break;};
    iprt = (i >= szd);

    dc[2] = rprt;
    dc[3] = iprt;

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DERIVE_COMPLEX_QUAD - figure out the long double format */

int derive_complex_quad(int *lc)
   {int i, szl, szc, iprt, rprt;
    union ucd {unsigned char c[32]; long double _Complex l[2];} bola;

    szc = sizeof(long double _Complex);
    szl = sizeof(long double);

    lc[0] = szc;
    lc[1] = szl;

/* get the part ordering from the long double complex */
    bola.l[0] = 1.0;
    bola.l[1] = I;

/* treated as an index into an array of 2 doubles find
 * the real part
 */
    for (i = 0; i < szl; i++)
        {if (bola.c[i] != 0)
	    break;};
    rprt = (i >= szl);

/* the imaginary part */
    for (i = 0; i < szl; i++)
        {if (bola.c[i+szc] != 0)
	    break;};
    iprt = (i >= szl);

    lc[2] = rprt;
    lc[3] = iprt;

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DERIVE_COMPLEX_FORMAT - figure out the complex floating point format */

int derive_complex_format(int *fc, int *dc, int *lc)
   {

    derive_complex_double(dc);
    derive_complex_quad(lc);
    derive_complex_float(fc);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HTML - organize the detect output in the form of a web page */

void print_html(FILE *fp)
   {int mfields, nt;
    char lmx[MAXLINE], lmn[MAXLINE], t[MAXLINE];
    char dmx[MAXLINE], dmn[MAXLINE];

/* determine the size of the min and max fields */     
    snprintf(lmn, MAXLINE, LL_FMT, (long long) LLONG_MIN);
    snprintf(lmx, MAXLINE, LL_FMT, (long long) LLONG_MAX);
    mfields = strlen(lmx);

#if defined(MSW)
    snprintf(dmn, MAXLINE, "%.8e", (double) LDBL_MIN);
    snprintf(dmx, MAXLINE, "%.8e", (double) LDBL_MAX);
#else
    snprintf(dmn, MAXLINE, "%.8Le", (long double) LDBL_MIN);
    snprintf(dmx, MAXLINE, "%.8Le", (long double) LDBL_MAX);
#endif

     snprintf(t, MAXLINE, "%3.8g", DBL_MAX);
     nt = strlen(t);
     mfields = max(mfields, nt);
     mfields += 2;

/* dump the output page */
     fprintf(fp, "Content-type: text/html\n\n");

     fprintf(fp, "<HTML>\n");

     fprintf(fp, "<HEAD>\n");
     fprintf(fp, "<TITLE>Native Type Information</TITLE>\n");
     fprintf(fp, "</HEAD>\n");

     fprintf(fp, "<BODY>\n");
     fprintf(fp, "<CENTER><H2>NATIVE TYPE INFORMATION</H2></CENTER>\n");

     fprintf(fp, "<CENTER>\n");
     fprintf(fp, "<TABLE BORDER=0>\n");
     fprintf(fp, "<TR><TD>");

/* byte order */
     if (int_order[0] == 'R')     
        fprintf(fp, "Machine byte order is reverse order (little endian),\n");
     else
        fprintf(fp, "Machine byte order is normal order (big endian),\n");

     fprintf(fp, "</TD></TR>\n");
     fprintf(fp, "<TR><TD>A pointer is %d bytes long.</TD></TR></TABLE></CENTER>\n\n", size[I_POINTER]);
     fprintf(fp, "<P>");

/* table of type info */
     fprintf(fp, "<CENTER>\n");
     fprintf(fp, "<TABLE BORDER=1>\n");

/* table headings */
     fprintf(fp, "<TR><TH>Type</TH> <TH>Size (bytes)</TH> <TH>Alignment</TH>");
     fprintf(fp, "<TH>Minimum Value</TH> <TH>Maximum Value</TH></TR>\n");

/* table rows */
/* char */
     fprintf(fp, "<TR ALIGN=RIGHT><TD>Char</TD><TD>1</TD><TD>%d</TD><TD>%d</TD><TD>%d</TD></TR>\n",
            align[I_CHAR], CHAR_MIN, CHAR_MAX);

/* wchar */
     fprintf(fp, "<TR ALIGN=RIGHT><TD>Wchar</TD><TD>1</TD><TD>%d</TD><TD>%ld</TD><TD>%ld</TD></TR>\n",
            align[I_WCHAR], (long) WCHAR_MIN, (long) WCHAR_MAX);

/* int8_t */
     fprintf(fp, "<TR ALIGN=RIGHT><TD>Int8_t</TD><TD>%d</TD><TD>%d</TD><TD>%d</TD><TD>%d</TD></TR>\n",
            size[I_INT8], align[I_INT8], INT8_MIN, INT8_MAX);

/* short */
     fprintf(fp, "<TR ALIGN=RIGHT><TD>Short</TD><TD>%d</TD><TD>%d</TD><TD>%d</TD><TD>%d</TD></TR>\n",
            size[I_SHORT], align[I_SHORT], SHRT_MIN, SHRT_MAX);

/* int */
     fprintf(fp, "<TR ALIGN=RIGHT><TD>Int</TD><TD>%d</TD><TD>%d</TD><TD>%d</TD><TD>%d</TD></TR>\n",
            size[I_INT], align[I_INT], INT_MIN, INT_MAX);

/* long */
     fprintf(fp, "<TR ALIGN=RIGHT><TD>Long</TD><TD>%d</TD><TD>%d</TD><TD>%ld</TD><TD>%ld</TD></TR>\n",
            size[I_LONG], align[I_LONG], LONG_MIN, LONG_MAX);

/* long long */
     fprintf(fp, "<TR ALIGN=RIGHT><TD>Long long</TD><TD>%d</TD><TD>%d</TD><TD>%s</TD><TD>%s</TD></TR>\n",
	     size[I_LONG_LONG], align[I_LONG_LONG],
	     lmn, lmx);

/* float */
     fprintf(fp, "<TR ALIGN=RIGHT><TD>Float</TD><TD>%d</TD><TD>%d</TD><TD>%3.8g</TD><TD>%3.8g</TD></TR>\n",
            size[I_FLOAT], align[I_FLOAT], FLT_MIN, FLT_MAX);

/* double */
     fprintf(fp, "<TR ALIGN=RIGHT><TD>Double</TD><TD>%d</TD><TD>%d</TD><TD>%3.8g</TD><TD>%3.8g</TD></TR>\n",
            size[I_DOUBLE], align[I_DOUBLE], DBL_MIN, DBL_MAX);

/* long double */
     fprintf(fp, "<TR ALIGN=RIGHT><TD>Long double</TD><TD>%d</TD><TD>%d</TD><TD>%s</TD><TD>%s</TD></TR>\n",
            size[I_LONG_DOUBLE], align[I_LONG_DOUBLE], dmn, dmx);

     fprintf(fp, "</TABLE></CENTER>\n");

     fprintf(fp, "</BODY>\n");

     fprintf(fp, "</HTML>\n");
          
     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_FIX_TYPE - print info about a fixed point type */

static void print_fix_type(char *type, int sz, int aln,
			   int mfields, int64_t mn, int64_t mx)
   {int tfield, sfield, afield;
    char bf[MAXLINE], t[MAXLINE];
    char lmn[MAXLINE], lmx[MAXLINE];
    char *tptr, *sptr, *aptr, *mnptr, *mxptr;

/* sizes of the fields in the output table */
    tfield = 12;
    sfield = 14;
    afield = 11;

    tptr  = bf;
    sptr  = bf + tfield;
    aptr  = bf + tfield + sfield;
    mnptr = bf + tfield + sfield + afield;
    mxptr = bf + tfield + sfield + afield + mfields;

    memset(bf, ' ', MAXLINE);

    strncpy(tptr, type, strlen(type));

    snprintf(t, MAXLINE, "%12d", sz);
    strncpy(sptr, t, strlen(t));

    snprintf(t, MAXLINE, "%9d", aln);
    strncpy(aptr, t, strlen(t));

    snprintf(lmn, MAXLINE, LL_FMT, (long long) mn);
    snprintf(lmx, MAXLINE, LL_FMT, (long long) mx);

    snprintf(t, MAXLINE, "%*s", mfields, lmn);
    strncpy(mnptr, t, strlen(t));

    snprintf(t, MAXLINE, "%*s", mfields, lmx);
    strncpy(mxptr, t, MAXLINE);

    puts(bf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_FLT_TYPE - print info about a floating point type */

static void print_flt_type(char *type, int sz, int aln,
			   int mfields, long double mn, long double mx)
   {int tfield, sfield, afield, offset, np;
    char bf[MAXLINE], s[MAXLINE], t[MAXLINE], fmt[MAXLINE];
    char *tptr, *sptr, *aptr, *mnptr, *mxptr;

/* sizes of the fields in the output table */
    tfield = 20;
    sfield = 6;
    afield = 11;

    tptr  = bf;
    sptr  = bf + tfield;
    aptr  = bf + tfield + sfield;
    mnptr = bf + tfield + sfield + afield;
    mxptr = bf + tfield + sfield + afield + mfields;

    memset(bf, ' ', MAXLINE);

    strncpy(tptr, type, strlen(type));

    snprintf(t, MAXLINE, "%4d", sz);
    strncpy(sptr, t, strlen(t));

    snprintf(t, MAXLINE, "%9d", aln);
    strncpy(aptr, t, strlen(t));

#if defined(MSW)
    if (sz < 5)
       np = 7;
    else if (sz < 9)
       np = 9;
    else
       np = 11;
    snprintf(fmt, MAXLINE, "%%.%dle", np);
    snprintf(s, MAXLINE, fmt, (double) mn);
    snprintf(t, MAXLINE, fmt, (double) mx);
#else
    if (sz < 5)
       np = 7;
    else if (sz < 9)
       np = 9;
    else
       np = 11;
    snprintf(fmt, MAXLINE, "%%.%dLe", np);
    snprintf(s, MAXLINE, fmt, mn);
    snprintf(t, MAXLINE, fmt, mx);
#endif

/*
    if (sz < 5)
       {snprintf(s, MAXLINE, "%.7Le", mn);
	snprintf(t, MAXLINE, "%.7Le", mx);}
    else if (sz < 9)
       {snprintf(s, MAXLINE, "%.9Le", mn);
	snprintf(t, MAXLINE, "%.9Le", mx);}
    else
       {snprintf(s, MAXLINE, "%.11Le", mn);
	snprintf(t, MAXLINE, "%.11Le", mx);};
*/
    offset = mfields - strlen(t);
    strncpy(mnptr+offset, t, strlen(t));

    offset = mfields - strlen(t);
    strcpy(mxptr+offset, t);

    puts(bf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HUMAN - organize the detect output for human consumption */

void print_human(FILE *fp, int sflag, int *fc, int *dc, int *lc)
    {int nt, mfields, offset;
     int tfield, sfield, afield;
     char bf[DBFSZ], t[MAXLINE];
     char *tptr, *sptr, *aptr, *mnptr, *mxptr;

/* sizes of the fields in the output table */
     tfield = 12;
     sfield = 14;
     afield = 11;

/* determine the size of the min and max fields */     
     snprintf(t, MAXLINE, LL_FMT, (long long) LLONG_MAX);
     mfields = strlen(t);

     snprintf(t, MAXLINE, "%3.8g", DBL_MAX);
     nt = strlen(t);
     mfields = max(mfields, nt);
     mfields += 2;

/* pointers to the beginning of each of the fields */     
     tptr  = bf;
     sptr  = bf + tfield;
     aptr  = bf + tfield + sfield;
     mnptr = bf + tfield + sfield + afield;
     mxptr = bf + tfield + sfield + afield + mfields;

     memset(bf, ' ', MAXLINE);

/* title */
     fprintf(fp, "\n              NATIVE TYPE INFORMATION\n\n");

/* byte order */
     if (int_order[0] == 'R')
        fprintf(fp, "Machine byte order is reverse order (little endian).\n");
     else
        fprintf(fp, "Machine byte order is normal order (big endian)\n");

     fprintf(fp, "A pointer is %d bytes long.\n\n", size[I_POINTER]);

/* print a table header */
     strncpy(tptr, "Type", 4);
     strncpy(sptr, "Size (bytes)", 12);
     strncpy(aptr, "Alignment", 9);
     offset = mfields - 13;
     strncpy(mnptr+offset, "Minimum Value", 13);
     strcpy(mxptr+ offset, "Maximum Value\n");
     puts(bf);

/* print bool info */
     print_fix_type("Bool", size[I_BOOL], align[I_BOOL], mfields, false, true);

/* print char info */
     print_fix_type("Char", 1, align[I_CHAR], mfields, CHAR_MIN, CHAR_MAX);

/* print wchar info */
     print_fix_type("Wchar", 1, align[I_WCHAR], mfields, WCHAR_MIN, WCHAR_MAX);

/* print int8 info */
     print_fix_type("Int8_t", size[I_INT8], align[I_INT8], mfields, INT8_MIN, INT8_MAX);

/* print short info */
     print_fix_type("Short", size[I_SHORT], align[I_SHORT], mfields, SHRT_MIN, SHRT_MAX);

/* print int info */
     print_fix_type("Int", size[I_INT], align[I_INT], mfields, INT_MIN, INT_MAX);

/* print long info */
     print_fix_type("Long", size[I_LONG], align[I_LONG], mfields, LONG_MIN, LONG_MAX);

/* print long long info */
     print_fix_type("Long long", size[I_LONG_LONG], align[I_LONG_LONG],
		    mfields, LLONG_MIN, LLONG_MAX);

/* print C99 fixed width integer types info */
     print_fix_type("int8_t", size[14], align[14],
		    mfields, INT8_MIN, INT8_MAX);
     print_fix_type("int16_t", size[15], align[15],
		    mfields, INT16_MIN, INT16_MAX);
     print_fix_type("int32_t", size[16], align[16],
		    mfields, INT32_MIN, INT32_MAX);
     print_fix_type("int64_t", size[17], align[17],
		    mfields, INT64_MIN, INT64_MAX);

/* print float info */
     print_flt_type("Float", size[I_FLOAT], align[I_FLOAT], mfields, FLT_MIN, FLT_MAX);

/* print double info */
     print_flt_type("Double", size[I_DOUBLE], align[I_DOUBLE], mfields, DBL_MIN, DBL_MAX);

/* print long double info */
     print_flt_type("Long double", size[I_LONG_DOUBLE], align[I_LONG_DOUBLE], mfields, LDBL_MIN, LDBL_MAX);

/* print float complex info */
     print_flt_type("Float complex", size[I_FLOAT_COMPLEX], align[I_FLOAT_COMPLEX], mfields, FLT_MIN, FLT_MAX);

/* print double complex info */
     print_flt_type("Double complex", size[I_DOUBLE_COMPLEX], align[I_DOUBLE_COMPLEX], mfields, DBL_MIN, DBL_MAX);

/* print long double complex info */
     print_flt_type("Long double complex", size[I_LONG_DOUBLE_COMPLEX], align[I_LONG_DOUBLE_COMPLEX], mfields, LDBL_MIN, LDBL_MAX);

     fprintf(fp, "\n");
     fprintf(fp, "Complex part order:   real  imaginary\n");
     fprintf(fp, "Float complex           %d       %d\n", fc[2]+1, fc[3]+1);
     fprintf(fp, "Double complex          %d       %d\n", dc[2]+1, dc[3]+1);
     fprintf(fp, "Long double complex     %d       %d\n", lc[2]+1, lc[3]+1);
     fprintf(fp, "\n");

/* print optional non-native types */
     if (sflag)
        {memset(bf, ' ', MAXLINE);
         strncpy(tptr, "int64_t", 6);
    
         snprintf(t, MAXLINE, "%12d", (int) sizeof(int64_t));
         strcpy(sptr, t);
         puts(bf);

         memset(bf, ' ', MAXLINE);
         strncpy(tptr, "size_t", 6);

         snprintf(t, MAXLINE, "%12d", (int) sizeof(size_t));
         strcpy(sptr, t);
         puts(bf);}

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HEADER - organize the detect output in the form of a c header file */

void print_header(FILE *fp,
		  int *fb, int *db, int *ldb, long *ff, long *df, long *ldf,
		  int *fc, int *dc, int *lc)
    {int i, j;

/* print the floating point ordering info */
    fprintf(fp, "#define HOST_ORDER %s\n", int_order);
    fprintf(fp, "\n");

    fprintf(fp, "\nint\n");
    fprintf(fp, " int_ord_f[] = {%d", fb[0]);
    for (i = 1; i < size[I_FLOAT]; i++)
        fprintf(fp, ", %d", fb[i]);
    fprintf(fp, "}, \n");

    fprintf(fp, " int_ord_d[] = {%d", db[0]);
    for (i = 1; i < size[I_DOUBLE]; i++)
        fprintf(fp, ", %d", db[i]);
    fprintf(fp, "},\n");

    fprintf(fp, " int_ord_ld[] = {%d", ldb[0]);
    for (i = 1; i < size[I_LONG_DOUBLE]; i++)
        fprintf(fp, ", %d", ldb[i]);
    fprintf(fp, "};\n");

/* print the floating point format info */
    fprintf(fp, "\nlong\n");

    fprintf(fp, " int_frm_f[] = {");
    for (i = 0; i < 7; i++)
        fprintf(fp, "%2ldL, ", ff[i]);
    fprintf(fp, "0x%lXL}, \n", ff[7]);

    fprintf(fp, " int_frm_d[] = {");
    for (i = 0; i < 7; i++)
        fprintf(fp, "%2ldL, ", df[i]);
    fprintf(fp, "0x%lXL},\n", df[7]);

    fprintf(fp, " int_frm_ld[] = {");
    for (i = 0; i < 7; i++)
        fprintf(fp, "%2ldL, ", ldf[i]);
    fprintf(fp, "0x%lXL};\n", ldf[7]);

/* emit data_standard definition */
    fprintf(fp, "\n/* Internal DATA_STANDARD */\n\n");
    fprintf(fp, "data_standard\n");
    fprintf(fp, " INT_STD = {%d,                                         /* bits per byte */\n",
           (int) BITS_DEFAULT);
    fprintf(fp, "            %d,                                       /* size of pointer */\n", 
           size[I_POINTER]);
    fprintf(fp, "            %d,                                          /* size of bool */\n", 
           size[I_BOOL]);

/* character types */
    fprintf(fp, "            {{%d, UTF_%d},                      /* size and order of char */\n", 
           size[I_CHAR], 8*size[I_CHAR]);
    fprintf(fp, "             {%d, UTF_%d}},                 /* size and order of wchar_t */\n", 
           size[I_WCHAR], 8*size[I_WCHAR]);

/* fixed point types */
    fprintf(fp, "            {{%d, %s},            /* size and order of int8_t */\n", 
           size[I_INT8], int_order);
    fprintf(fp, "             {%d, %s},             /* size and order of short */\n", 
           size[I_SHORT], int_order);
    fprintf(fp, "             {%d, %s},               /* size and order of int */\n", 
           size[I_INT], int_order);
    fprintf(fp, "             {%d, %s},              /* size and order of long */\n", 
           size[I_LONG], int_order);
    fprintf(fp, "             {%d, %s}},        /* size and order of long long */\n", 
           size[I_LONG_LONG], int_order);

/* floating point types */
    fprintf(fp, "            {{%d, int_frm_f, int_ord_f},             /* float definition */\n", 
           size[I_FLOAT]);
    fprintf(fp, "             {%d, int_frm_d, int_ord_d},            /* double definition */\n", 
           size[I_DOUBLE]);
    fprintf(fp, "             {%d, int_frm_ld, int_ord_ld}}};  /* long double definition */\n", 
           size[I_LONG_DOUBLE]);

/* emit data_alignment definition */
    fprintf(fp, "\n/* Internal DATA_ALIGNMENT */\n\n");
    fprintf(fp, "data_alignment\n");
    fprintf(fp, " INT_ALG = {%d, %d, {%d, %d}, {%d, %d, %d, %d, %d}, {%d, %d, %d}, %d};\n", 
	   align[I_POINTER], align[I_BOOL],
           align[I_CHAR], align[I_WCHAR],
           align[I_INT8], align[I_SHORT], align[I_INT], 
           align[I_LONG], align[I_LONG_LONG],
	   align[I_FLOAT], align[I_DOUBLE], align[I_LONG_DOUBLE],
	   align[I_STRUCT]);

    fprintf(fp, "\n/* complex tuple info */\n\n");
    fprintf(fp, "int\n");
    fprintf(fp, " floatc_ord[] = {");
    for (j = 0; j < 2; j++)
        {if (j == 0)
	    fprintf(fp, "%d", fc[j+2]);
	 else
	    fprintf(fp, ", %d", fc[j+2]);};
    fprintf(fp, "},\n");
    fprintf(fp, " doublec_ord[] = {");
    for (j = 0; j < 2; j++)
        {if (j == 0)
	    fprintf(fp, "%d", dc[j+2]);
	 else
	    fprintf(fp, ", %d", dc[j+2]);};
    fprintf(fp, "},\n");
    fprintf(fp, " long_doublec_ord[] = {");
    for (j = 0; j < 2; j++)
        {if (j == 0)
	    fprintf(fp, "%d", lc[j+2]);
	 else
	    fprintf(fp, ", %d", lc[j+2]);};
    fprintf(fp, "};\n\n");
    
    fprintf(fp, "multides\n");
    fprintf(fp, " floatc_tuple = { \"float_complex\", 2, floatc_ord },\n");
    fprintf(fp, " doublec_tuple = { \"double_complex\", 2, doublec_ord },\n");
    fprintf(fp, " long_doublec_tuple = { \"long_double_complex\", 2, long_doublec_ord };\n");

    fprintf(fp, "\n\n");}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - print help message */

static void help(void)
   {

    printf("Usage: detect [-c] [-h] [-o <file>] [-s] [-w]\n");
    printf("   c  produce C header style output\n");
    printf("   h  this help message\n");
    printf("   o  output file name (default stdout)\n");
    printf("   s  print optional size related types\n");
    printf("   w  produce HTML output\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int i;
    int cflag, wflag, sflag;
    int fb[40], db[40], ldb[40];
    int fc[4], dc[4], lc[4];
    long ff[8], df[8], ldf[8];
    char *outf;
    FILE *fo;

/* produce c header style output */
    cflag = 0;

/* produce HTML output suitable for cgi
 * the default is to produce human readable output
 */
    wflag = 0;

/* print optional size related types like size_t and int64_t */
    sflag = 0;

    outf = NULL;
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'c' : 
                      cflag = 1;
                      break;
                 case 'h' :
                      help();
		      return(1);
                      break;
                 case 'o' :
                      outf = v[++i];
                      break;
                 case 's' :
                      sflag = 1;
                      break;
                 case 'w' :
                      wflag = 1;
                      break;
                 default :
                      break;};}
         else
            break;};

    if (outf != NULL)
       fo = fopen_safe(outf, "w");
    else
       fo = stdout;

    if (fo != NULL)

/* data type sizes are straightforward */
       {TYPE_SET(I_BOOL,                bool,                 cb);
	TYPE_SET(I_CHAR,                char,                 cc);
	TYPE_SET(I_WCHAR,               wchar_t,              cw);
	TYPE_SET(I_INT8,                int8_t,               ci8);
	TYPE_SET(I_SHORT,               short,                cs);
	TYPE_SET(I_INT,                 int,                  ci);
	TYPE_SET(I_LONG,                long,                 cl);
	TYPE_SET(I_LONG_LONG,           long long,            cll);
	TYPE_SET(I_FLOAT,               float,                cf);
	TYPE_SET(I_DOUBLE,              double,               cd);
	TYPE_SET(I_LONG_DOUBLE,         long double,          cld);
	TYPE_SET(I_FLOAT_COMPLEX,       float _Complex,       cfc);
	TYPE_SET(I_DOUBLE_COMPLEX,      double _Complex,      cdc);
	TYPE_SET(I_LONG_DOUBLE_COMPLEX, long double _Complex, clc);

	TYPE_SET(I_POINTER,             void *,               cp);
	type_set(I_STRUCT,              2*sizeof(char),       sizeof(ct));

	TYPE_SET(17, int16_t,              ci16);
	TYPE_SET(18, int32_t,              ci32);
	TYPE_SET(19, int64_t,              ci64);

	bo.i[0] = 1;
	if (bo.c[0] == 1)
	   strncpy(int_order, "REVERSE_ORDER", 80);
	else
	   strncpy(int_order, "NORMAL_ORDER", 80);

	derive_fp_format(fb, db, ldb, ff, df, ldf);
	derive_complex_format(fc, dc, lc);

	if (cflag)
	   print_header(fo, fb, db, ldb, ff, df, ldf, fc, dc, lc);
	else if (wflag)
	   print_html(fo);
	else
	   print_human(fo, sflag, fc, dc, lc);

	if (outf != NULL)
	   fclose_safe(fo);};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

