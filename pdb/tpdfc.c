/*
 * TPDFC.C - test floating point conversions in PDB
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

static void fcnv(char *out, char *in, inti ni,
		 int dir, PDBfile *vif, char *type)
   {int onescmp;
    intb lnby;
    int *iaord, *oaord;
    long *ifmt, *ofmt;
    defstr *idp, *odp;
    data_standard *hstd;
    PD_byte_order lsord;

    odp  = PD_inquire_table_type(vif->chart, type);
    idp  = PD_inquire_table_type(vif->host_chart, type);
    hstd = vif->host_std;

    if (dir == 0)
       {ifmt    = idp->fp.format;
	iaord   = idp->fp.order;
	onescmp = idp->onescmp;

	ofmt    = odp->fp.format;
	oaord   = odp->fp.order;}
    else
       {ifmt    = odp->fp.format;
	iaord   = odp->fp.order;
	onescmp = odp->onescmp;

	ofmt    = idp->fp.format;
	oaord   = idp->fp.order;};

    lsord = hstd->fx[PD_LONG_I].order;
    lnby  = hstd->fx[PD_LONG_I].bpi;

    _PD_fconvert(&out, &in, ni, 0, ifmt, iaord, ofmt, oaord,
		 lsord, lnby, onescmp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FCMP - do buffer comparisons */

static int fcmp(char *bfa, char *bfb, inti nb)
   {int rv;
    inti i;

    rv = TRUE;
    for (i = 0; (i < nb) && (rv == TRUE); i++)
        rv &= (bfa[i] == bfb[i]);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* T8 - test float8 conversions */

static int t8(PDBfile *vif)
   {int rv, err;
    long ni;
    double s, a[4];
    char out[MAXLINE], res[MAXLINE];
    char *in;

    s = exp(1.0);
    a[0] = s;
    a[1] = 1.0;
    a[2] = sqrt(2.0);
    a[3] = 0.999999;

    rv = TRUE;

    io_printf(stdout, "\t\t%12s %3d ...", vif->name, sizeof(double));

    in = (char *) &s;
    ni = 1L;
    fcnv(out, in, ni, 0, vif, "double");
    fcnv(res, out, ni, 1, vif, "double");
    err = fcmp(in, res, ni*sizeof(double));
    rv &= err;
    if (err == TRUE)
       io_printf(stdout, ".");
    else
       io_printf(stdout, "x");

    in = (char *) a;
    ni = 4L;
    fcnv(out, in, ni, 0, vif, "double");
    fcnv(res, out, ni, 1, vif, "double");
    err = fcmp(in, res, ni*sizeof(double));
    rv &= err;
    if (err == TRUE)
       io_printf(stdout, ".");
    else
       io_printf(stdout, "x");

    if (rv == TRUE)
       io_printf(stdout, " ok\n");
    else
       io_printf(stdout, " ng\n");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* T16 - test float16 conversions */

static int t16(PDBfile *vif)
   {int rv, err;
    inti ni;
    long double s, a[4];
    char out[MAXLINE], res[MAXLINE];
    char *in;

    memset(a, 0, sizeof(a));
    memset(out, 0, sizeof(out));
    memset(res, 0, sizeof(res));

    s = expl(1.0L);
    a[0] = s;
    a[1] = 1.0L;
    a[2] = sqrtl(2.0L);
    a[3] = 0.999999L;

    rv = TRUE;

    io_printf(stdout, "\t\t%12s %3d ...", vif->name, sizeof(long double));

    in = (char *) &s;
    ni = 1L;
    fcnv(out, in, ni, 0, vif, "long_double");
    fcnv(res, out, ni, 1, vif, "long_double");
    err = fcmp(in, res, ni*sizeof(long double));
    rv &= err;
    if (err == TRUE)
       io_printf(stdout, ".");
    else
       io_printf(stdout, "x");

    in = (char *) a;
    ni = 4L;
    fcnv(out, in, ni, 0, vif, "long_double");
    fcnv(res, out, ni, 1, vif, "long_double");
    err = fcmp(in, res, ni*sizeof(long double));
    rv &= err;
    if (err == TRUE)
       io_printf(stdout, ".");
    else
       io_printf(stdout, "x");

    if (rv == TRUE)
       io_printf(stdout, " ok\n");
    else
       io_printf(stdout, " ng\n");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_TARGET - set up the target for the data file */

static PDBfile *test_target(int n)
   {int rv;
    char *nm;
    PDBfile *vif;

    rv = PD_target_platform_n(n);
    SC_ASSERT(rv == TRUE);

    nm = PD_target_platform_name(n);

    vif = PD_open_vif(nm);

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

    PRINT(STDOUT, "\nTPDFC - test floating point conversions\n\n");
    PRINT(STDOUT, "Usage: tpdfc [-h]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       h  - print this help message and exit\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test the PDB Library system */

int main(int c, char **v)
   {int i, err, rv;

    PD_init_threads(0, NULL);

    SC_zero_space_n(1, -2);

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

    rv = (err != 2);

    PRINT(STDOUT, "\n");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
