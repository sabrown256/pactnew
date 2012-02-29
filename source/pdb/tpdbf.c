/*
 * TPDBF.C - test for the PDB I/O buffering
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BYTE_SIZE_HR - print out human readable byte size */

static void byte_size_hr(char *ns, int nc, int64_t n)
   {

    if (n >= 1000000000)
       snprintf(ns, nc, "%.0fG", n/1000000000.0);
    else if (n >= 1000000)
       snprintf(ns, nc, "%.0fM", n/1000000.0);
    else if (n >= 1000)
       snprintf(ns, nc, "%.0fK", n/1000.0);
    else if (n > 0)
       snprintf(ns, nc, "%d", (int) n);
    else
       snprintf(ns, nc, "sys");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ANALYZE - report the I/O stats */

static void analyze(long nc, long ni, double dt)
   {long nb;
    long *stats;
    char nis[MAXLINE], nbs[MAXLINE], rat[MAXLINE];

    stats = SC_binfo();

    nb = PD_get_buffer_size();

    byte_size_hr(nis, MAXLINE, ni);
    byte_size_hr(nbs, MAXLINE, nb);

    if (nb < 0)
       snprintf(rat, MAXLINE, "sys");
    else if (nb < ni)
       snprintf(rat, MAXLINE, "1/%ld", ni/nb);
    else
       snprintf(rat, MAXLINE, "%ld", nb/ni);

    printf("                         ");
    printf("%ld    %6s %6s %8s    %5ld   %.1e\n",
	   nc, nis, nbs, rat, stats[3], dt);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - test the buffering in PDBLib writes */

static void test_1(long ni, long nc)
   {long i, n;
    long ind[3];
    char *d, *pd;
    char name[MAXLINE];
    double t0, dt;
    PDBfile *strm;

    d = CMAKE_N(char, ni);
    memset(d, 0, ni);

    strm = PD_create("wrn.pdb");

    n = (ni + nc - 1)/nc;
    ind[0] = 0L;
    ind[1] = n - 1;
    ind[2] = 1L;

    t0 = SC_wall_clock_time();

    for (i = 0; i < nc; i++)
        {pd = d + i*n;
	 snprintf(name, MAXLINE, "v%08ld", i);
	 PD_write_alt(strm, name, "char", pd, 1, ind);};

    dt = SC_wall_clock_time() - t0;

/* close the file */
    PD_close(strm);

    analyze(nc, ni, dt);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test the PDB Library system */

int main(int c, char **v)
   {int i, vers;
    long n, nc, bfsz;

    PD_init_threads(0, NULL);

    n    = 1000000;
    nc   = 4;
    bfsz = 1000000;
    vers = 3;

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'b' :
		      bfsz = SC_stol(v[++i]);
		      break;
                 case 'c' :
		      nc = SC_stoi(v[++i]);
		      break;
                 case 'h' :
		      return(1);
                 case 'n' :
		      n = SC_stol(v[++i]);
		      break;
                 case 'v' :
                      vers = SC_stoi(v[++i]);
		      break;};}
         else
            break;};

    PD_set_fmt_version(vers);
    PD_set_buffer_size(bfsz);

    test_1(n, nc);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
