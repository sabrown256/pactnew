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

#include "scstd.h"
#include <complex.h>

#ifdef NO_LONG_LONG

#define MAXLONG     LONG_MAX
#define MINLONG     LONG_MIN
#define LONGFORMAT  "%ld"
#define VLONGFORMAT "%*ld"

#else

#define MAXLONG     ((BIGUINT) (1LL << (8LL*sizeof(BIGINT) - 1LL)) - 1LL)
#define MINLONG     (-1LL << (8LL*sizeof(BIGINT) - 1LL))
#define LONGFORMAT  "%lld"
#define VLONGFORMAT "%*lld"

#endif

#define NTYPES 14

#define BITS_DEFAULT 8

/* these structs will help determine alignment of the primitive types */
struct cchar
   {char c;
    char x;} cc;
struct cptr
   {char c;
    char *x;} cp;
struct cfix2
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
    BIGINT x;} cll;
struct cfloat4
   {char c;
    float x;} cf;
struct cfloat8
   {char c;
    double x;} cd;
struct cfloat16
   {char c;
    long double x;} cld;
struct cbool
   {char c;
    bool b;} cb;
struct ccomplex4
   {char c;
    float complex x;} cfc;
struct ccomplex8
   {char c;
    double complex x;} cdc;
struct ccomplex16
   {char c;
    long double complex x;} clc;

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
 size[NTYPES],
 align[NTYPES];

static char
 int_order[80];
 
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
    long exp;

    sbyte = ff[4]/8;
    ebyte = ff[5]/8;
    exp   = 0L;
    for (i = sbyte; i <= ebyte; i++)
        {l = fb[i] - 1;
         exp <<= 8;
         exp |= a[l];};

    nbits = (ebyte + 1)*8 - ff[5];
    exp >>= nbits;

    ff[7] = exp;

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
    union ucf {unsigned char c[32]; float f[2];} bofa;

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

void print_html(void)
    {int mfields;
     char temp[MAXLINE];

/* determine the size of the min and max fields */     
     snprintf(temp, MAXLINE, LONGFORMAT, MAXLONG);
     mfields = strlen(temp);

     snprintf(temp, MAXLINE, "%3.8g", DBL_MAX);
     mfields = max(mfields, strlen(temp));
     mfields += 2;

/* dump the output page */
     printf("Content-type: text/html\n\n");

     printf("<HTML>\n");

     printf("<HEAD>\n");
     printf("<TITLE>Native Type Information</TITLE>\n");
     printf("</HEAD>\n");

     printf("<BODY>\n");
     printf("<CENTER><H2>NATIVE TYPE INFORMATION</H2></CENTER>\n");

     printf("<CENTER>\n");
     printf("<TABLE BORDER=0>\n");
     printf("<TR><TD>");

/* byte order */
     if (int_order[0] == 'R')     
        printf("Machine byte order is reverse order (little endian),\n");
     else
        printf("Machine byte order is normal order (big endian),\n");

     printf("</TD></TR>\n");
     printf("<TR><TD>A pointer is %d bytes long.</TD></TR></TABLE></CENTER>\n\n", size[1]);
     printf("<P>");

/* table of type info */
     printf("<CENTER>\n");
     printf("<TABLE BORDER=1>\n");

/* table headings */
     printf("<TR><TH>Type</TH> <TH>Size (bytes)</TH> <TH>Alignment</TH>");
     printf("<TH>Minimum Value</TH> <TH>Maximum Value</TH></TR>\n");

/* table rows */
/* char */
     printf("<TR ALIGN=RIGHT><TD>Char</TD><TD>1</TD><TD>%d</TD><TD>%d</TD><TD>%d</TD></TR>\n",
            align[0], CHAR_MIN, CHAR_MAX);

/* short */
     printf("<TR ALIGN=RIGHT><TD>Short</TD><TD>%d</TD><TD>%d</TD><TD>%d</TD><TD>%d</TD></TR>\n",
            size[2], align[2], SHRT_MIN, SHRT_MAX);

/* int */
     printf("<TR ALIGN=RIGHT><TD>Int</TD><TD>%d</TD><TD>%d</TD><TD>%d</TD><TD>%d</TD></TR>\n",
            size[3], align[3], INT_MIN, INT_MAX);

/* long */
     printf("<TR ALIGN=RIGHT><TD>Long</TD><TD>%d</TD><TD>%d</TD><TD>%ld</TD><TD>%ld</TD></TR>\n",
            size[4], align[4], LONG_MIN, LONG_MAX);

/* long long */
     printf("<TR ALIGN=RIGHT><TD>Long long</TD><TD>%d</TD><TD>%d</TD><TD>%lld</TD><TD>%lld</TD></TR>\n",
            size[5], align[5], MINLONG, MAXLONG);

/* float */
     printf("<TR ALIGN=RIGHT><TD>Float</TD><TD>%d</TD><TD>%d</TD><TD>%3.8g</TD><TD>%3.8g</TD></TR>\n",
            size[6], align[6], FLT_MIN, FLT_MAX);

/* double */
     printf("<TR ALIGN=RIGHT><TD>Double</TD><TD>%d</TD><TD>%d</TD><TD>%3.8g</TD><TD>%3.8g</TD></TR>\n",
            size[7], align[7], DBL_MIN, DBL_MAX);

/* long double */
     printf("<TR ALIGN=RIGHT><TD>Long double</TD><TD>%d</TD><TD>%d</TD><TD>%.8Le</TD><TD>%.8Le</TD></TR>\n",
            size[8], align[8], LDBL_MIN, LDBL_MAX);

     printf("</TABLE></CENTER>\n");

     printf("</BODY>\n");

     printf("</HTML>\n");
          
     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_FIX_TYPE - print info about a fixed point type */

static void print_fix_type(char *type, int sz, int aln,
			   int mfields, BIGINT mn, BIGINT mx)
   {int tfield, sfield, afield;
    char bf[MAXLINE], t[MAXLINE];
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

    snprintf(t, MAXLINE, VLONGFORMAT, mfields, mn);
    strncpy(mnptr, t, strlen(t));

    snprintf(t, MAXLINE, VLONGFORMAT, mfields, mx);
    strcpy(mxptr, t);

    puts(bf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_FLT_TYPE - print info about a floating point type */

static void print_flt_type(char *type, int sz, int aln,
			   int mfields, long double mn, long double mx)
   {int tfield, sfield, afield, offset;
    char bf[MAXLINE], s[MAXLINE], t[MAXLINE];
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

    if (sz < 5)
       {snprintf(s, MAXLINE, "%.7Le", mn);
	snprintf(t, MAXLINE, "%.7Le", mx);}
    else if (sz < 9)
       {snprintf(s, MAXLINE, "%.9Le", mn);
	snprintf(t, MAXLINE, "%.9Le", mx);}
    else
       {snprintf(s, MAXLINE, "%.11Le", mn);
	snprintf(t, MAXLINE, "%.11Le", mx);};

    offset = mfields - strlen(t);
    strncpy(mnptr+offset, t, strlen(t));

    offset = mfields - strlen(t);
    strcpy(mxptr+offset, t);

    puts(bf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HUMAN - organize the detect output for human consumption */

void print_human(int sflag, int *fc, int *dc, int *lc)
    {char bf[MAXLINE], t[MAXLINE];
     char *tptr, *sptr, *aptr, *mnptr, *mxptr;

/* sizes of the fields in the output table */
     int mfields, offset;
     int tfield = 12;
     int sfield = 14;
     int afield = 11;

/* determine the size of the min and max fields */     
     snprintf(t, MAXLINE, LONGFORMAT, MAXLONG);
     mfields = strlen(t);

     snprintf(t, MAXLINE, "%3.8g", DBL_MAX);
     mfields = max(mfields, strlen(t));
     mfields += 2;

/* pointers to the beginning of each of the fields */     
     tptr  = bf;
     sptr  = bf + tfield;
     aptr  = bf + tfield + sfield;
     mnptr = bf + tfield + sfield + afield;
     mxptr = bf + tfield + sfield + afield + mfields;

     memset(bf, ' ', MAXLINE);

/* title */
     printf("\n              NATIVE TYPE INFORMATION\n\n");

/* byte order */
     if (int_order[0] == 'R')
        printf("Machine byte order is reverse order (little endian).\n");
     else
        printf("Machine byte order is normal order (big endian)\n");

     printf("A pointer is %d bytes long.\n\n", size[1]);

/* print a table header */
     strncpy(tptr, "Type", 4);
     strncpy(sptr, "Size (bytes)", 12);
     strncpy(aptr, "Alignment", 9);
     offset = mfields - 13;
     strncpy(mnptr+offset, "Minimum Value", 13);
     strcpy(mxptr+ offset, "Maximum Value\n");
     puts(bf);

/* print char info */
     print_fix_type("Char", 1, align[0], mfields, CHAR_MIN, CHAR_MAX);

/* print char info */
     print_fix_type("Bool", size[10], align[10], mfields, false, true);

/* print short info */
     print_fix_type("Short", size[2], align[2], mfields, SHRT_MIN, SHRT_MAX);

/* print int info */
     print_fix_type("Int", size[3], align[3], mfields, INT_MIN, INT_MAX);

/* print long info */
     print_fix_type("Long", size[4], align[4], mfields, LONG_MIN, LONG_MAX);

/* print long long info */
     print_fix_type("Long long", size[5], align[5],
		    mfields, MINLONG, MAXLONG);

/* print float info */
     print_flt_type("Float", size[6], align[6], mfields, FLT_MIN, FLT_MAX);

/* print double info */
     print_flt_type("Double", size[7], align[7], mfields, DBL_MIN, DBL_MAX);

/* print long double info */
     print_flt_type("Long double", size[8], align[8], mfields, LDBL_MIN, LDBL_MAX);

/* print float complex info */
     print_flt_type("Float complex", size[11], align[11], mfields, FLT_MIN, FLT_MAX);

/* print double complex info */
     print_flt_type("Double complex", size[12], align[12], mfields, DBL_MIN, DBL_MAX);

/* print long double complex info */
     print_flt_type("Long double complex", size[13], align[13], mfields, LDBL_MIN, LDBL_MAX);

     printf("\n");
     printf("Complex part order:   real  imaginary\n");
     printf("Float complex           %d       %d\n", fc[2]+1, fc[3]+1);
     printf("Double complex          %d       %d\n", dc[2]+1, dc[3]+1);
     printf("Long double complex     %d       %d\n", lc[2]+1, lc[3]+1);
     printf("\n");

/* print optional non-native types */
     if (sflag)
        {memset(bf, ' ', MAXLINE);
         strncpy(tptr, "BIGINT", 6);
    
         snprintf(t, MAXLINE, "%12d", (int) sizeof(BIGINT));
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

void print_header(int *fb, int *db, int *ldb, long *ff, long *df, long *ldf,
		  int *fc, int *dc, int *lc)
    {int i, j;

/* print the floating point ordering info */
    printf("\nint\n");

    printf(" int_ord_f[] = {%d", fb[0]);
    for (i = 1; i < size[6]; i++)
        printf(", %d", fb[i]);
    printf("}, \n");

    printf(" int_ord_d[] = {%d", db[0]);
    for (i = 1; i < size[7]; i++)
        printf(", %d", db[i]);
    printf("},\n");

    printf(" int_ord_ld[] = {%d", ldb[0]);
    for (i = 1; i < size[8]; i++)
        printf(", %d", ldb[i]);
    printf("};\n");

/* print the floating point format info */
    printf("\nlong\n");

    printf(" int_frm_f[] = {");
    for (i = 0; i < 7; i++)
        printf("%2ldL, ", ff[i]);
    printf("0x%lXL}, \n", ff[7]);

    printf(" int_frm_d[] = {");
    for (i = 0; i < 7; i++)
        printf("%2ldL, ", df[i]);
    printf("0x%lXL},\n", df[7]);

    printf(" int_frm_ld[] = {");
    for (i = 0; i < 7; i++)
        printf("%2ldL, ", ldf[i]);
    printf("0x%lXL};\n", ldf[7]);

    printf("\n/* Internal DATA_STANDARD */\n\n");
    printf("data_standard\n");
    printf(" INT_STD = {%d,                                         /* bits per byte */\n",
           (int)BITS_DEFAULT);
    printf("            %d,                                       /* size of pointer */\n", 
           size[1]);
    printf("            %d,                                          /* size of bool */\n", 
           size[10]);
    printf("            {{%d, %s},             /* size and order of short */\n", 
           size[2], int_order);
    printf("             {%d, %s},               /* size and order of int */\n", 
           size[3], int_order);
    printf("             {%d, %s},              /* size and order of long */\n", 
           size[4], int_order);
    printf("             {%d, %s}},        /* size and order of long long */\n", 
           size[5], int_order);
    printf("            {{%d, int_frm_f, int_ord_f},             /* float definition */\n", 
           size[6]);
    printf("             {%d, int_frm_d, int_ord_d},            /* double definition */\n", 
           size[7]);
    printf("             {%d, int_frm_ld, int_ord_ld}}},  /* long double definition */\n", 
           size[8]);
    printf(" *INT_STANDARD = &INT_STD;\n");

    printf("\n/* Internal DATA_ALIGNMENT */\n\n");
    printf("data_alignment\n");
    printf(" INT_ALG = {%d, %d, %d, {%d, %d, %d, %d}, {%d, %d, %d}, %d},\n", 
           align[0], align[1], align[10], align[2], align[3], 
           align[4], align[5], align[6], align[7], align[8],
	   align[9]);
    printf(" *INT_ALIGNMENT = &INT_ALG;\n");

    printf("\n/* complex tuple info */\n\n");
    printf("int\n");
    printf(" floatc_ord[] = {");
    for (j = 0; j < 2; j++)
        {if (j == 0)
	    printf("%d", fc[j+2]);
	 else
	    printf(", %d", fc[j+2]);};
    printf("},\n");
    printf(" doublec_ord[] = {");
    for (j = 0; j < 2; j++)
        {if (j == 0)
	    printf("%d", dc[j+2]);
	 else
	    printf(", %d", dc[j+2]);};
    printf("},\n");
    printf(" long_doublec_ord[] = {");
    for (j = 0; j < 2; j++)
        {if (j == 0)
	    printf("%d", lc[j+2]);
	 else
	    printf(", %d", lc[j+2]);};
    printf("};\n\n");
    
    printf("multides\n");
    printf(" floatc_tuple = { \"float_complex\", 2, floatc_ord },\n");
    printf(" doublec_tuple = { \"double_complex\", 2, doublec_ord },\n");
    printf(" long_doublec_tuple = { \"long_double_complex\", 2, long_doublec_ord };\n");

    printf("\n\n");}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - print help message */

static void help(void)
   {

    printf("Usage: detect [-c] [-h] [-s] [-w]\n");
    printf("   c  produce C header style output\n");
    printf("   h  this help message\n");
    printf("   s  print optional size related types\n");
    printf("   w  produce HTML output\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int argc, char **argv)
   {int i, ssize[NTYPES];
    int cflag, wflag, sflag;
    int fb[40], db[40], ldb[40];
    int fc[4], dc[4], lc[4];
    long ff[8], df[8], ldf[8];

/* produce c header style output */
    cflag = 0;

/* produce HTML output suitable for cgi
 * the default is to produce human readable output
 */
    wflag = 0;

/* print optional size related types like size_t and BIGINT */
    sflag = 0;

    for (i = 1; i < argc; i++)
        {if (argv[i][0] == '-')
            {switch (argv[i][1])
                {case 'c' : 
                      cflag = 1;
                      break;
                 case 'h' :
                      help();
		      return(1);
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

/* data type sizes are straightforward */
    size[0]  = sizeof(char);
    size[1]  = sizeof(char *);
    size[2]  = sizeof(short);
    size[3]  = sizeof(int);
    size[4]  = sizeof(long);
    size[5]  = sizeof(BIGINT);
    size[6]  = sizeof(float);
    size[7]  = sizeof(double);
    size[8]  = sizeof(long double);
    size[9]  = 2*sizeof(char);
    size[10] = sizeof(bool);
    size[11] = sizeof(float complex);
    size[12] = sizeof(double complex);
    size[13] = sizeof(long double complex);

    ssize[0]  = sizeof(cc);
    ssize[1]  = sizeof(cp);
    ssize[2]  = sizeof(cs);
    ssize[3]  = sizeof(ci);
    ssize[4]  = sizeof(cl);
    ssize[5]  = sizeof(cll);
    ssize[6]  = sizeof(cf);
    ssize[7]  = sizeof(cd);
    ssize[8]  = sizeof(cld);
    ssize[9]  = sizeof(ct);
    ssize[10] = sizeof(cb);
    ssize[11] = sizeof(cfc);
    ssize[12] = sizeof(cdc);
    ssize[13] = sizeof(clc);

/* first possibility: align <= size (usual case)
 * alignment is difference between struct length and member size:
 */
    for (i = 0; i < NTYPES; i++)
        align[i] = ssize[i] - size[i];

/* second possibility: align > size (e.g. Cray char)
 * alignment is half of structure size:
 */
    for (i = 0; i < NTYPES; i++)
        if (align[i] > (ssize[i] >> 1))
           align[i] = ssize[i] >> 1;

    bo.i[0] = 1;
    if (bo.c[0] == 1)
       strcpy(int_order, "REVERSE_ORDER");
    else
       strcpy(int_order, "NORMAL_ORDER");

    derive_fp_format(fb, db, ldb, ff, df, ldf);
    derive_complex_format(fc, dc, lc);

    if (cflag)
        print_header(fb, db, ldb, ff, df, ldf, fc, dc, lc);
    else if (wflag)
        print_html();
    else
        print_human(sflag, fc, dc, lc);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

