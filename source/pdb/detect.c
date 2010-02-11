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

#define BITS_DEFAULT 8

/* these structs will help determine alignment of the primitive types */
struct cchar
   {char c;
    char x;} cc;
struct cptr
   {char c;
    char *x;} cp;
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
    BIGINT x;} cll;
struct cfloat
   {char c;
    float x;} cf;
struct cdouble
   {char c;
    double x;} cd;

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
 fb[40],
 size[9],
 db[40],
 align[9];

static long 
 ff[8],
 df[8];

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
            {byt   = a[l] ^ b[l];
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

/* DERIVE_FP_FORMAT - figure out the floating point format */

int derive_fp_format(int *fb, int *db, long *ff, long *df)
   {int j, szf, szd, first, last;
    union ucf {unsigned char c[32]; float f[2];} bofa, bofb, bofc;
    union ucd {unsigned char c[32]; double d[2];} boda, bodb, bodc;
    float fval, dfv;
    double dval, ddv;

    szd = sizeof(double);
    szf = sizeof(float);

    df[0] = szd*8;
    ff[0] = szf*8;

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
    boda.d[0] = 0.5;
    bodb.d[0] = 1.0;
    guard_bit(szd, 6, db, df, boda.c, bodb.c);

    bofa.f[0] = 0.5;
    bofb.f[0] = 1.0;
    guard_bit(szf, 6, fb, ff, bofa.c, bofb.c);

/* find the sign bit */
    boda.d[0] = -1.0;
    bodb.d[0] = 1.0;
    differ_bit(szd, 3, db, df, boda.c, bodb.c);

    bofa.f[0] = -1.0;
    bofb.f[0] = 1.0;
    differ_bit(szf, 3, fb, ff, bofa.c, bofb.c);

    df[4] = df[3] + 1;
    ff[4] = ff[3] + 1;

    bodc.d[0] = 1.5;
    differ_bit(szd, 5, db, df, bodc.c, bodb.c);

    bofc.f[0] = 1.5;
    differ_bit(szf, 5, fb, ff, bofc.c, bofb.c);

/* adjust loc(mantissa) */
    df[5] -= df[6];
    ff[5] -= ff[6];

/* #exponent_bits = loc(mantissa) - loc(exponent) */
    df[1] = df[5] - df[4];
    ff[1] = ff[5] - ff[4];

/* #mantissa_bits = #bits - #exponent_bits - 1 */
    df[2] = df[0] - df[1] - 1;
    ff[2] = ff[0] - ff[1] - 1;

    find_exp(db, df, bodb.c);
    find_exp(fb, ff, bofb.c);

/* adjust exponent */
    df[7] -= df[6];
    ff[7] -= ff[6];

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
    tfield = 10;
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
			   int mfields, double mn, double mx)
   {int tfield, sfield, afield, offset;
    char bf[MAXLINE], t[MAXLINE];
    char *tptr, *sptr, *aptr, *mnptr, *mxptr;

/* sizes of the fields in the output table */
    tfield = 10;
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

    snprintf(t, MAXLINE, "%3.8g", mn);
    offset = mfields - strlen(t);
    strncpy(mnptr+offset, t, strlen(t));

    snprintf(t, MAXLINE, "%3.8g", mx);
    offset = mfields - strlen(t);
    strcpy(mxptr+offset, t);

    puts(bf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HUMAN - organize the detect output for human consumption */

void print_human(int sflag)
    {char bf[MAXLINE], t[MAXLINE];
     char *tptr, *sptr, *aptr, *mnptr, *mxptr;

/* sizes of the fields in the output table */
     int mfields, offset;
     int tfield = 10;
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

/* print optional non-native types */
     if (sflag)
        {memset(bf, ' ', MAXLINE);
         strncpy(tptr, "off_t", 5);
    
         snprintf(t, MAXLINE, "%12d", (int) sizeof(off_t));
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

void print_header(void)
    {int i;

/* print the floating point ordering info */
    printf("\nint\n");
    printf(" int_ord_f[] = {%d", fb[0]);
    for (i = 1; i < size[6]; i++)
        printf(", %d", fb[i]);
    printf("}, \n");
    printf(" int_ord_d[] = {%d", db[0]);
    for (i = 1; i < size[7]; i++)
        printf(", %d", db[i]);
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
    printf("0x%lXL};\n", df[7]);

    printf("\n/* Internal DATA_STANDARD */\n");
    printf("data_standard\n");
    printf(" INT_STD = {%d,                               /* bits per byte */\n",
           (int)BITS_DEFAULT);
    printf("            %d,                             /* size of pointer */\n", 
           size[1]);
    printf("            %d, %s,       /* size and order of short */\n", 
           size[2], int_order);
    printf("            %d, %s,         /* size and order of int */\n", 
           size[3], int_order);
    printf("            %d, %s,        /* size and order of long */\n", 
           size[4], int_order);
    printf("            %d, %s,   /* size and order of long long */\n", 
           size[5], int_order);
    printf("            %d, int_frm_f, int_ord_f,      /* float definition */\n", 
           size[6]);
    printf("            %d, int_frm_d, int_ord_d},    /* double definition */\n", 
           size[7]);
    printf(" *INT_STANDARD = &INT_STD;\n");

    printf("\n/* Internal DATA_ALIGNMENT */\n");
    printf("data_alignment\n");
    printf(" INT_ALG = {%d, %d, %d, %d, %d, %d, %d, %d, %d},\n", 
           align[0], align[1], align[2], align[3], 
           align[4], align[5], align[6], align[7], align[8]);
    printf(" *INT_ALIGNMENT = &INT_ALG;\n");

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
   {int ssize[9], i;
    int cflag = 0;  /* produce c header style output            */
    int wflag = 0;  /* produce HTML output suitable for cgi     */
                    /* the default is to produce human readable */
                    /* output.                                  */
    int sflag = 0;  /* print optional size related types        */
                    /* like size_t and off_t                    */

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
    size[0] = sizeof(char);
    size[1] = sizeof(char *);
    size[2] = sizeof(short);
    size[3] = sizeof(int);
    size[4] = sizeof(long);
    size[5] = sizeof(BIGINT);
    size[6] = sizeof(float);
    size[7] = sizeof(double);
    size[8] = 2*sizeof(char);

    ssize[0] = sizeof(cc);
    ssize[1] = sizeof(cp);
    ssize[2] = sizeof(cs);
    ssize[3] = sizeof(ci);
    ssize[4] = sizeof(cl);
    ssize[5] = sizeof(cll);
    ssize[6] = sizeof(cf);
    ssize[7] = sizeof(cd);
    ssize[8] = sizeof(ct);

/* first possibility: align <= size (usual case)
 * alignment is difference between struct length and member size:
 */
    for (i = 0; i < 9; i++)
        align[i] = ssize[i] - size[i];

/* second possibility: align > size (e.g. Cray char)
 * alignment is half of structure size:
 */
    for (i = 0; i < 9; i++)
        if (align[i] > (ssize[i] >> 1))
           align[i] = ssize[i] >> 1;

    bo.i[0] = 1;
    if (bo.c[0] == 1)
       strcpy(int_order, "REVERSE_ORDER");
    else
       strcpy(int_order, "NORMAL_ORDER");

    derive_fp_format(fb, db, ff, df);

    if (cflag)
        print_header();
    else if (wflag)
        print_html();
    else
        print_human(sflag);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

