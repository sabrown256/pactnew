/*
 * LIMITS.C - decipher the basic C binary data type limits
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#include <stdio.h>

#undef TRUE
#undef FALSE
#define TRUE  1
#define FALSE 0

#ifdef USE_LONG_LONG
typedef long long BIGINT;
#else
typedef long BIGINT;
#endif

typedef union u_ucsil ucsil;
typedef union u_ucl ucl;

union u_ucsil
   {unsigned char b[32];
    unsigned char c;
    short s;
    int i;
    unsigned int ui;
    long l;
    BIGINT ll;
    float f;
    double d;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FIX_LMT - return the signed minimum and maximum values as well as
 *            - the unsigned maximum value for an integer type
 *            - of NB bytes
 *            - return TRUE iff NB is smaller than sizeof(BIGINT)
 */

int PM_fix_lmt(char *name, char *sfx, int nb,
	       BIGINT *pmn, BIGINT *pmx, BIGINT *pumx)
   {int i, rv;
    BIGINT imn, imx, uimx;

    rv = FALSE;

    if (nb <= sizeof(BIGINT))

/* do the min limits */
       {for (imn = 0x80, i = 1; i < nb; i++)
	    imn <<= 8;

/* do the max limits */
	for (imx = 0x7f, i = 1; i < nb; i++)
	    imx = (imx << 8) + 0xff;

/* do the unsigned max limits */
	for (uimx = 0xff, i = 1; i < nb; i++)
	    uimx = (uimx << 8) + 0xff;

	if (nb < sizeof(BIGINT))
	   imn = -imn;

	printf("#define %s_MIN \t%lld%s\n", name, imn, sfx);
	printf("#define %s_MAX \t %lld%s\n", name, imx, sfx);
	printf("#define U%s_MAX\t %llu%s\n",  name, uimx, sfx);
	printf("\n");

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

/* MAIN - start here */

main()
   {int i, nfp, fw;
    char format[40];
    BIGINT imn, imx, uimx;
    float fmn, fmx;
    double dmn, dmx, conf;
    ucsil bo, bp;

    printf("\n");

/* do the char limits */
    PM_fix_lmt("CHAR", "", sizeof(char), &imn, &imx, &uimx);

/* do the short limits */
    PM_fix_lmt("SHRT", "", sizeof(short), &imn, &imx, &uimx);

/* do the int limits */
    PM_fix_lmt("INT", "", sizeof(int), &imn, &imx, &uimx);

/* do the long limits */
    PM_fix_lmt("LONG", "L", sizeof(long), &imn, &imx, &uimx);

/* do the long long limits */
    PM_fix_lmt("LLONG", "LL", sizeof(long long), &imn, &imx, &uimx);

    conf = 24.0/10.0;

/* the floating point algorithm will not work as is for CRAY arithmetic
 * no big deal since they have an ANSI compiler which will have the
 * necessary constants
 * GOTCHA: this is also not guaranteed as is to work with formats using
 * the mantissa guard bit (IEEE 96 bit format for example)
 */

/* do the float limits */
    bo.f = 1.0;
    bp.f = 0.5;
    for (i = 0; i < sizeof(float); i++)
        {bo.b[i] = bo.b[i] & (~bp.b[i]);
         bp.b[i] = 0xFF ^ bo.b[i];};
    fmn =  bo.f;
    fmx = -bp.f;

    nfp  = conf*(sizeof(float) - 1.0) + 1;
    fw   = nfp + 8;
    snprintf(format, 40, "%%s%%%d.%de", fw, nfp);

    printf(format, "#define FLT_MIN \t", fmn);
    printf("\n");

    printf(format, "#define FLT_MAX \t", fmx);
    printf("\n");

/* do the double limits */
    bo.d = 1.0;
    bp.d = 0.5;
    for (i = 0; i < sizeof(double); i++)
        {bo.b[i] = bo.b[i] & (~bp.b[i]);
         bp.b[i] = 0xFF ^ bo.b[i];};
    dmn =  bo.d;
    dmx = -bp.d;

    nfp  = conf*(sizeof(double) - 1.0) + 1;
    fw   = nfp + 8;
    snprintf(format, 40, "%%s%%%d.%de", fw, nfp);

    printf(format, "#define DBL_MIN \t", dmn);
    printf("\n");

    printf(format, "#define DBL_MAX \t", dmx);
    printf("\n\n");

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

