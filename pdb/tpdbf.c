/*
 * TPDBF.C - test for the PDB I/O buffering
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb.h"

typedef struct s_statedes statedes;
typedef struct s_anades anades;

struct s_statedes
   {int vers;
    int debug_mode;
    int cksum;
    int nc;
    int64_t n;
    int64_t bfsz;};

struct s_anades
   {int nc;
    int ni;
    int vfy;
    double dt;
    char *tag;};

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

static void analyze(anades *ad)
   {int vfy;
    long nb, nc, ni;
    long *stats;
    char nis[MAXLINE], nbs[MAXLINE], rat[MAXLINE];
    char *tag;
    double dt;

    nc  = ad->nc;
    ni  = ad->ni;
    dt  = ad->dt;
    vfy = ad->vfy;
    tag = ad->tag;

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

    printf("            ");
    printf("%2s  %c  %ld    %6s %6s %8s    %5ld    %5ld   %.1e\n",
	   tag,
	   (vfy == TRUE) ? '+' : '-',
	   nc, nis, nbs, rat, stats[3], stats[5], dt);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRT - write a buffered file */

static void wrt(statedes *st, anades *ad)
   {long i, n, ni, nc;
    long ind[3];
    char *d, *pd;
    char name[MAXLINE];
    double t0, dt;
    PDBfile *strm;

    ni = st->n;
    nc = st->nc;

    d = CMAKE_N(char, ni);
    memset(d, 0, ni);

    strm = PD_create("wrn.pdb");

    if (st->cksum == TRUE)
       PD_activate_cksum(strm, PD_MD5_FILE);

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

    ad->ni  = ni;
    ad->nc  = nc;
    ad->dt  = dt;
    ad->vfy = strm->cksum.verified;

/* close the file */
    PD_close(strm);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RD - read a buffered file */

static void rd(statedes *st, anades *ad)
   {long i, n, ni, nc;
    long ind[3];
    char *d, *pd;
    char name[MAXLINE];
    double t0, dt;
    PDBfile *strm;

    ni = st->n;
    nc = st->nc;

    d = CMAKE_N(char, ni);
    memset(d, 0, ni);

    strm = PD_open("wrn.pdb", "r");

    n = (ni + nc - 1)/nc;
    ind[0] = 0L;
    ind[1] = n - 1;
    ind[2] = 1L;

    t0 = SC_wall_clock_time();

    for (i = 0; i < nc; i++)
        {pd = d + i*n;
	 snprintf(name, MAXLINE, "v%08ld", i);
	 PD_read_alt(strm, name, pd, ind);};

    dt = SC_wall_clock_time() - t0;

    ad->ni  = ni;
    ad->nc  = nc;
    ad->dt  = dt;
    ad->vfy = strm->cksum.verified;

/* close the file */
    PD_close(strm);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - test the buffering in PDBLib writes */

static void test_1(statedes *st)
   {anades ad;

    wrt(st, &ad);

    ad.tag = "wr";

    analyze(&ad);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - test the buffering in PDBLib reads */

static void test_2(statedes *st)
   {anades ad;

    rd(st, &ad);

    ad.tag = "rd";

    analyze(&ad);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nTPDBF - run PDB buffer size tests\n\n");
    PRINT(STDOUT, "Usage: tpdbf [-b #] [-c #] [-h] [-n #] [-s] [-v #]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       b  - set buffer size (default no buffering)\n");
    PRINT(STDOUT, "       c  - number of iterations\n");
    PRINT(STDOUT, "       h  - print this help message and exit\n");
    PRINT(STDOUT, "       n  - number of items in array\n");
    PRINT(STDOUT, "       s  - turn off whole file checksums\n");
    PRINT(STDOUT, "       v  - use format version # (default is 2)\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test the PDB Library system */

int main(int c, char **v)
   {int i;
    statedes st = { 3, FALSE, TRUE, 4, 1000000, SC_OPT_BFSZ };

    PD_init_threads(0, NULL);

    st.n    = 1000000;
    st.nc   = 4;
    st.bfsz = SC_OPT_BFSZ;
    st.vers = 3;

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'b' :
		      st.bfsz = SC_stol(v[++i]);
		      break;
                 case 'c' :
		      st.nc = SC_stoi(v[++i]);
		      break;
                 case 'h' :
		      print_help();
		      return(1);
                 case 'n' :
		      st.n = SC_stol(v[++i]);
		      break;
                 case 's' :
		      st.cksum = FALSE;
		      break;
                 case 'v' :
                      st.vers = SC_stoi(v[++i]);
		      break;};}
         else
            break;};

    PD_set_fmt_version(st.vers);
    PD_set_buffer_size(st.bfsz);

    test_1(&st);
    test_2(&st);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
