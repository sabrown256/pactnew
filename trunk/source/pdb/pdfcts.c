/*
 * PDFCTS.C - test floating point conversions in PDB
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

typedef int (*PFtfc)(PDBfile *vif);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FCNV - do floating point conversions */

static void fcnv(char *out, char *in, long ni, PDBfile *vif, char *type)
   {int rv;
    int lnby, onescmp;
    int *iaord, *oaord;
    long *ifmt, *ofmt;
    defstr *idp, *odp;
    data_standard *hstd;
    PD_byte_order lsord;

    rv = TRUE;

    odp  = PD_inquire_table_type(vif->chart, type);
    idp  = PD_inquire_table_type(vif->host_chart, type);
    hstd = vif->host_std;

    ifmt    = idp->format;
    iaord   = idp->order;
    onescmp = idp->onescmp;

    ofmt    = odp->format;
    oaord   = odp->order;

    lsord   = hstd->long_order;
    lnby    = hstd->long_bytes;

    _PD_fconvert(&out, &in, ni, 0, ifmt, iaord, ofmt, oaord,
		 lsord, lnby, onescmp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FCMP - do buffer comparisons */

static int fcmp(char *bfa, char *bfb, int nb)
   {int i, rv;

    rv = TRUE;
    for (i = 0; i < nb; i++)
        rv &= (bfa[i] == bfb[i]);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* T8 - test float8 conversions */

static int t8(PDBfile *vif)
   {int rv;
    double s, a[4];
    char out[MAXLINE], res[MAXLINE];
    char *in;

    s = exp(1.0);
    a[0] = s;
    a[1] = 1.0;
    a[2] = sqrt(2.0);
    a[3] = 0.999999;

    rv = TRUE;

    in = (char *) &s;

    fcnv(out, in, 1L, vif, "double");
    fcnv(res, out, 1L, vif, "double");
    rv &= fcmp(in, res, sizeof(double));

    in = (char *) a;

    fcnv(out, in, 4L, vif, "double");
    fcnv(res, out, 4L, vif, "double");
    rv &= fcmp(in, res, 4*sizeof(double));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* T16 - test float16 conversions */

static int t16(PDBfile *vif)
   {int rv;
    long double s, a[4];
    char out[MAXLINE], res[MAXLINE];
    char *in;

    s = expl(1.0L);
    a[0] = s;
    a[1] = 1.0L;
    a[2] = sqrtl(2.0L);
    a[3] = 0.999999L;

    rv = TRUE;

    in = (char *) &s;

    fcnv(out, in, 1L, vif, "long_double");
    fcnv(res, out, 1L, vif, "long_double");
    rv &= fcmp(in, res, sizeof(long double));

    in = (char *) a;

    fcnv(out, in, 4L, vif, "long_double");
    fcnv(res, out, 4L, vif, "long_double");
    rv &= fcmp(in, res, 4*sizeof(long double));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_TARGET - set up the target for the data file */

static PDBfile *test_target(int n)
   {int rv;
    PDBfile *vif;

    rv = PD_target_platform_n(n);

    vif = PD_open_vif("internal");

    return(vif);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RUN_TEST - run a test doing all the boiler plate work */

static int run_test(PFtfc tst, int fl)
   {int i, n, rv;
    PDBfile *vif;

    n = 10;
    rv = TRUE;

    for (i = 0; i < n; i++)
        {vif = test_target(i);
	 rv &= tst(vif);

	 PD_close(vif);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nPDFCTS - test floating point conversions\n\n");
    PRINT(STDOUT, "Usage: pdfcts [-h]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       h  - print this help message and exit\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test the PDB Library system */

int main(int c, char **v)
   {int i, err;

    PD_init_threads(0, NULL);

    SC_zero_space(0);

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'h' :
		      print_help();
		      return(1);
		      break;};}
         else
            break;};

    SC_signal(SIGINT, SIG_DFL);

    err = 0;
    err += run_test(t8, TRUE);
    err += run_test(t16, TRUE);

    PRINT(STDOUT, "\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
