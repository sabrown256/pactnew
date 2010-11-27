/*
 * MLNANTS.C - test PM_fix_nan
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml.h"

#define N 6

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DBL_TEST - work over double NaNs */

int dbl_test(void)
   {int i, nn, rv, le;
    uint64_t *ba;
    char *ca, *msg;
    double d[N], dhuge;

    PM_enable_fpe(FALSE, SIG_IGN);

    for (i = 0; i < N; i++)
        d[i] = 0.0;

    rv = 1;
    ca = (char *) &rv;
    le = (ca[0] == 1);

    ca = (char *) d;
    ba = (uint64_t *) d;

/* plus and minus infinities
 *   positive double 0x7ff0000000000000
 *   negative double 0xfff0000000000000
 */
    if (le == TRUE)
       {ca[6] = 0xf0;
	ca[7] = 0x7f;
	ca += 8;
	ca[6] = 0xf0;
	ca[7] = 0xff;
	ca += 8;}
    else
       {ca[0] = 0x7f;
	ca[1] = 0xf0;
	ca += 8;
	ca[0] = 0xff;
	ca[1] = 0xf0;
	ca += 8;};

/* plus and minus signalling NaNs
 *   positive double 0x7ff0000000000001 to 0x7ff7ffffffffffff
 *   negative double 0xfff0000000000001 to 0xfff7ffffffffffff
 */
    if (le == TRUE)
       {ca[5] = 0x80;
	ca[6] = 0xf0;
	ca[7] = 0x7f;
	ca += 8;
	ca[5] = 0x80;
	ca[6] = 0xf0;
	ca[7] = 0xff;
	ca += 8;}
    else
       {ca[0] = 0x7f;
	ca[1] = 0xf0;
	ca[2] = 0x80;
	ca += 8;
	ca[0] = 0xff;
	ca[1] = 0xf0;
	ca[2] = 0x80;
	ca += 8;};

/* plus and minus quiet NaNs
 *   positive double 0x7ff8000000000000 to 0x7fffffffffffffff
 *   negative double 0xfff8000000000000 to 0xffffffffffffffff
 */
    if (le == TRUE)
       {ca[5] = 0x08;
	ca[6] = 0xf8;
	ca[7] = 0x7f;
	ca += 8;
	ca[5] = 0x08;
	ca[6] = 0xf8;
	ca[7] = 0xff;
	ca += 8;}
    else
       {ca[0] = 0x7f;
	ca[1] = 0xf8;
	ca[2] = 0x08;
	ca += 8;
	ca[0] = 0xff;
	ca[1] = 0xf8;
	ca[2] = 0x08;
	ca += 8;};

/* classify NaNs */
    PRINT(stdout, "\t\tDouble NaN detection:\n");
    for (i = 0; i < N; i++)
        {rv = PM_fp_typed(d[i]);
	 switch (rv)
	    {default :
	     case 0  :
	          msg = "legal";
		  break;
	     case 1 :
	          msg = "+inf";
		  break;
	     case 2 :
	          msg = "-inf";
		  break;
	     case 4 :
	          msg = "+nans";
		  break;
	     case 8 :
	          msg = "-nans";
		  break;
	     case 16 :
	          msg = "+nanq";
		  break;
	     case 32 :
	          msg = "-nanq";
		  break;};

	 PRINT(stdout, "\t\t%10g ... %s\n", d[i], msg);};
    PRINT(stdout, "\n");

    PM_clear_fpu();

    dhuge = 1.01e100;

/* NaN repare */
    PRINT(stdout, "\t\tDouble NaN repare:");
    nn  = PM_fix_nand(N, d,  1, dhuge);
    nn += PM_fix_nand(N, d,  2, -dhuge);
    nn += PM_fix_nand(N, d,  4, 10.0*dhuge);
    nn += PM_fix_nand(N, d,  8, -10.0*dhuge);
    nn += PM_fix_nand(N, d, 16, 100.0*dhuge);
    nn += PM_fix_nand(N, d, 32, -100.0*dhuge);
    PRINT(stdout, " %d nans fixed\n", nn);
    PRINT(stdout, "\n");

    rv = (nn == 6);
    PRINT(stdout, "\t\tDouble test ..................... %s\n",
	  rv ? "ok" : "ng");
    PRINT(stdout, "\n");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FLT_TEST - work over float NaNs */

int flt_test(void)
   {int i, nn, rv, le;
    int *ba;
    char *ca, *msg;
    float f[N], fhuge;

    PM_enable_fpe(FALSE, SIG_IGN);

    for (i = 0; i < N; i++)
        f[i] = 0.0;

    rv = 1;
    ca = (char *) &rv;
    le = (ca[0] == 1);

    ca = (char *) f;
    ba = (int *) f;

/* plus and minus infinities
 *   positive float  0x7f800000
 *   negative float  0xff800000
 */
    if (le == TRUE)
       {ca[2] = 0x80;
	ca[3] = 0x7f;
	ca += 4;
	ca[2] = 0x80;
	ca[3] = 0xff;
	ca += 4;}
    else
       {ca[0] = 0x7f;
	ca[1] = 0x80;
	ca += 4;
	ca[0] = 0xff;
	ca[1] = 0x80;
	ca += 4;};

/* plus and minus signalling NaNs
 *   positive float  0x7f800001 to 0x7fbfffff
 *   negative float  0xff800001 to 0xffbfffff
 */
    if (le == TRUE)
       {ca[2] = 0x81;
	ca[3] = 0x7f;
	ca += 4;
	ca[2] = 0x81;
	ca[3] = 0xff;
	ca += 4;}
    else
       {ca[0] = 0x7f;
	ca[1] = 0x81;
	ca += 4;
	ca[0] = 0xff;
	ca[1] = 0x81;
	ca += 4;};

/* plus and minus quiet NaNs
 *   positive float  0x7fc00000 to 0x7fffffff
 *   negative float  0xffc00000 to 0xffffffff
 */
    if (le == TRUE)
       {ca[2] = 0xc1;
	ca[3] = 0x7f;
	ca += 4;
	ca[2] = 0xc1;
	ca[3] = 0xff;
	ca += 4;}
    else
       {ca[0] = 0x7f;
	ca[1] = 0xc1;
	ca += 4;
	ca[0] = 0xff;
	ca[1] = 0xc1;
	ca += 4;};

/* classify NaNs */
    PRINT(stdout, "\t\tFloat NaN detection:\n");
    for (i = 0; i < N; i++)
        {rv = PM_fp_typef(f[i]);
	 switch (rv)
	    {default :
	     case 0  :
	          msg = "legal";
		  break;
	     case 1 :
	          msg = "+inf";
		  break;
	     case 2 :
	          msg = "-inf";
		  break;
	     case 4 :
	          msg = "+nans";
		  break;
	     case 8 :
	          msg = "-nans";
		  break;
	     case 16 :
	          msg = "+nanq";
		  break;
	     case 32 :
	          msg = "-nanq";
		  break;};

	 PRINT(stdout, "\t\t%10g ... %s\n", f[i], msg);};
    PRINT(stdout, "\n");

    PM_clear_fpu();

    fhuge = 1.001e28;

/* NaN repare */
    PRINT(stdout, "\t\tFloat NaN repare:");
    nn  = PM_fix_nanf(N, f,  1, fhuge);
    nn += PM_fix_nanf(N, f,  2, -fhuge);
    nn += PM_fix_nanf(N, f,  4, 10.0*fhuge);
    nn += PM_fix_nanf(N, f,  8, -10.0*fhuge);
    nn += PM_fix_nanf(N, f, 16, 100.0*fhuge);
    nn += PM_fix_nanf(N, f, 32, -100.0*fhuge);
    PRINT(stdout, " %d nans fixed\n", nn);
    PRINT(stdout, "\n");

    rv = (nn == 6);
    PRINT(stdout, "\t\tFloat test ...................... %s\n",
	  rv ? "ok" : "ng");
    PRINT(stdout, "\n");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int rv;

    rv = TRUE;

    rv &= flt_test();
    rv &= dbl_test();

    rv = (rv == FALSE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

