/*
 * SCPSD.C - PostScript diff
 *         - compares two PostScript files upto roundoff
 *         - NOTE: intended for testing PGS generated PostScript
 *         -  files.  Any others MIGHT work but no guarantees.
 *         - NOTE: two defects for future consideration
 *         -  1) No way to really compare images (very hard)
 *         -  2) Assumes only 1 line at a time differs
 *         -     This could be fixed to save and check a section
 *         -     at a time (moderate difficulty)
 *         -     For example
 *         -       3357c3357
 *         -       < 1 0 0 1 213 1615 0 Tp
 *         -       ---
 *         -       > 1 0 0 1 212 1615 0 Tp
 *         -     is handled correctly and
 *         -       3357,8c3357,8
 *         -       < 1 0 0 1 213 1615 0 Tp
 *         -       < 1 0 0 1 213 2183 0 Tp
 *         -       ---
 *         -       > 1 0 0 1 212 1615 0 Tp
 *         -       < 1 0 0 1 213 2183 0 Tp
 *         -     will only check the second line for difference
 *         -     missing the difference in the first line
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_proc.h"

#define LINE_LEN   120
#define BFSZ       8192

typedef struct s_diff_stat diff_stat;
typedef struct s_pixdes pixdes;

struct s_diff_stat
   {int ndiff;
    double fdmn;
    double fdmx;
    double fdsm;
    double dsm;
    double asum;};

struct s_pixdes
   {int nb;         /* # bytes per pixel */
    int nc;         /* # characters per pixel */
    int clr;};      /* pixel color model */

static PROCESS
 *pa = NULL,
 *pb = NULL;

static int
 damx = -1,
 drmx = -1,
 dgmx = -1,
 dbmx = -1,
 dpmx = -1,
 n_pixel = 1,
 imav    = FALSE;

static double
 frac_tol = 1.0e-3;

/*--------------------------------------------------------------------------*/

/*                           INTERACTIVE COMPARISONS                        */

/*--------------------------------------------------------------------------*/

/* SESSION_OVER - send quit message to the processes
 *              - and close the processes
 */

static void session_over(void)
   {

    if (pa != NULL)
       {SC_printf(pa, "quit");
	SC_close(pa);
 	pa = NULL;};

    if (pb != NULL)
       {SC_printf(pb, "quit");
	SC_close(pb);
 	pb = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INTERRUPT_HANDLER - handle interrupt signals */

static void interrupt_handler(int sig)
   {

    session_over();

    printf("\n");
    exit(1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PS_DIFF_OPEN - open a GS session */

static PROCESS *ps_diff_open(char *f, int off, int side)
   {PROCESS *pp;

#if 1
    int pid, rv, ok;
    char xrdb[PATH_MAX];
    char *rn, *s;
    FILE *fp;
    SC_contextdes hnd;
    static char *a[] = {"gs", "-sDEVICE=x11", NULL, "-", NULL};

/* ignore SIGCHLD for the system call */
    hnd = SC_signal_n(SIGCHLD, SIG_IGN, NULL);

    pid = getpid();
    rn = SC_dsnprintf(TRUE, ".Xres-%d", pid);
    fp = io_open(rn, "w");

    if (side == -1)
       io_printf(fp, "Ghostscript*geometry:  512x512+%d+2\n", off);
    else
       io_printf(fp, "Ghostscript*geometry:  512x512-%d+2\n", off);

    io_printf(fp, "Ghostscript*xResolution: 72\n");
    io_printf(fp, "Ghostscript*yResolution: 72\n");

    io_close(fp);

    ok = SC_full_path("xrdb", xrdb, PATH_MAX);
    if (ok < 0)
       printf("There is no xrdb on your path\n");

    else
       {s  = SC_dsnprintf(FALSE, "%s -merge %s", xrdb, rn);
        rv = system(s);
	SC_ASSERT(rv == 0);};

    unlink(rn);

    CFREE(rn);

/* restore the handler on SIGCHLD */
    SC_signal_n(SIGCHLD, hnd.f, hnd.a);

    a[2] = f;

#else
    static char *a[] = {"gs", "-sDEVICE=x11", "-r72x72", "-g512x512",
			NULL, "-", NULL};

    a[4] = f;

#endif

    pp = SC_open(a, NULL, "apo", NULL);

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROCESS_OUTPUT - handle output from the child process PP */

static int process_output(PROCESS *pp)
   {int rv;
    char s[MAXLINE], t[MAXLINE];

    t[0] = '\0';

    while (SC_gets(s, MAXLINE, pp) != NULL)
       strcpy(t, s);

    rv = (t[0] != '\0');

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PS_DIFF_INSPECT - compare the specified PostScript files visually */

static int ps_diff_inspect(char *f1, char *f2, int o1, int o2,
			   pixdes *pd, int verbose)
   {int err, na;
    char s[MAXLINE];

    err = TRUE;

    pa = ps_diff_open(f1, o1, -1);
    SC_sleep(1000);
    pb = ps_diff_open(f2, o2,  1);

    SC_signal(SIGINT, interrupt_handler);

    SC_setbuf(stdout, NULL);

    while (TRUE)
       {printf("\r-> ");
	if (io_gets(s, MAXLINE, stdin) != NULL)
	   {if (strcmp(s, "quit\n") == 0)
	       break;
	    else
	       {SC_printf(pa, "%s", s);
		SC_printf(pb, "%s", s);};};

	na = 0;
	na += process_output(pa);
	na += process_output(pb);

        if (na == 0)
	   break;};

    session_over();

    printf("\n");

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            PIXEL COMPARISONS                             */

/*--------------------------------------------------------------------------*/

/* MISS_PIXEL - check two pixels equality
 *            - each pixel has (may have) RGB components
 *            - which must be separately checked
 *            - return TRUE if they do not match within tolerance
 */

static int miss_pixel(int a, int b, pixdes *pd)
   {int rv;
    int aa, ar, ag, ab, ba, br, bg, bb, da, dr, dg, db, dp;

/* RGB + transparency */
    if (pd->clr == 2)
       {aa = (a >> 24) & 0xFF;
        ar = (a >> 16) & 0xFF;
	ag = (a >> 8) & 0xFF;
	ab = a & 0xFF;

	ba = (b >> 24) & 0xFF;
	br = (b >> 16) & 0xFF;
	bg = (b >> 8) & 0xFF;
	bb = b & 0xFF;

	da = abs(aa - ba);
	dr = abs(ar - br);
	dg = abs(ag - bg);
	db = abs(ab - bb);

	damx = max(damx, da);
	drmx = max(drmx, dr);
	dgmx = max(dgmx, dg);
	dbmx = max(dbmx, db);

	rv = ((dr > n_pixel) || (dg > n_pixel) || (db > n_pixel));}

/* RGB */
    else if (pd->clr == 1)
       {ar = (a >> 16) & 0xFF;
	ag = (a >> 8) & 0xFF;
	ab = a & 0xFF;

	br = (b >> 16) & 0xFF;
	bg = (b >> 8) & 0xFF;
	bb = b & 0xFF;

	dr = abs(ar - br);
	dg = abs(ag - bg);
	db = abs(ab - bb);

	drmx = max(drmx, dr);
	dgmx = max(dgmx, dg);
	dbmx = max(dbmx, db);

	rv = ((dr > n_pixel) || (dg > n_pixel) || (db > n_pixel));}

    else
       {dp   = abs(a - b);
	dpmx = max(dpmx, dp);
	rv   = (dp > n_pixel);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHECK_IMAGE_LOC - check for equality of pixels in an image
 *                 - this is a local method - checking pixel and neighbors
 *                 - return TRUE iff any bytes differ by more than N_PIXEL
 *                 - NOTE: changed so that if a pixel is off by more than
 *                 - N_PIXEL look to see if the difference is that there
 *                 - was a shift (due to round off in the domain) in which
 *                 - case the central pixel and a neighbor of one array
 *                 - must both match the values shifted by one of the
 *                 - other array
 */

static int check_image_loc(char *ta, char *tb, pixdes *pd)
   {int i, j, nb, err, nca, ncb, npe, dnc;
    int ca[3], cb[3];
    char sa[9], sb[9];
    char *pta, *ptb;
    static int row = 0;

    err = FALSE;

    memset(ca, 0, sizeof(ca));
    memset(cb, 0, sizeof(cb));

    nb = pd->nb;

    sa[nb] = '\0';
    sb[nb] = '\0';

    pta = ta;
    ptb = tb;

    nca = strlen(ta)/nb;
    ncb = strlen(tb)/nb;

/* permit image widths to differ by a pixel */
    dnc = nca - ncb;
    if (abs(dnc) > 1)
       return(TRUE);

    nca = min(nca, ncb);

    npe = 0;
    for (i = 0; i < nca; i++)
        {for (j = 0; j < nb; j++)
	     {sa[j] = *pta++;
	      sb[j] = *ptb++;};

/* check right edge when we are done with the text */
	 if (i == nca-1)
	    {if (miss_pixel(ca[2], cb[2], pd))
	        {if (miss_pixel(ca[1], cb[2], pd) &&
		     miss_pixel(ca[2], cb[1], pd))
		    npe++;};

	     break;};

/* update the template */
	 ca[0] = ca[1];
	 ca[1] = ca[2];
	 ca[2] = SC_strtol(sa, NULL, 16);

	 cb[0] = cb[1];
	 cb[1] = cb[2];
	 cb[2] = SC_strtol(sb, NULL, 16);

/* check left edge */
	 if (i == 1)
	    {if (miss_pixel(ca[1], cb[1], pd))
	        {if (miss_pixel(ca[1], cb[2], pd) &&
		     miss_pixel(ca[2], cb[1], pd))
		    npe++;};}

/* check center template */
	 else if (i > 2)

/* if no match then
 *         A0  A1  A2
 *          |   |
 *     B0  B1  B2
 * or
 *         A0  A1  A2
 *              |   |
 *             B0  B1  B2
 */
	    {if (miss_pixel(ca[1], cb[1], pd))
	        {if ((miss_pixel(ca[1], cb[0], pd) ||
		      miss_pixel(ca[2], cb[1], pd)) &&
		     (miss_pixel(ca[1], cb[2], pd) ||
		      miss_pixel(ca[0], cb[1], pd)))
		    npe++;};};};

    row++;

/* error for more than 2 pixels to be off in a line */
    err = (npe > 2);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHECK_IMAGE_AVE - check for equality of pixels in an image
 *                 - this is an averaging method - checking a row pixel
 *                 - the fractional component differences are summed
 *                 - over a row
 *                 - return TRUE iff the component differences exceed 3
 */

static int check_image_ave(char *ta, char *tb, pixdes *pd)
   {int i, j, k, nb, nc, err, nca, ncb;
    int ca, cb;
    double tol, den, ex, f;
    double d[4];
    char ua[9], ub[9];
    char *pta, *ptb;
    static int row = 0;

    err = FALSE;

    nb = pd->nb;
    nc = pd->nc;

    ua[nb] = '\0';
    ub[nb] = '\0';

    pta = ta;
    ptb = tb;

    nca = strlen(ta)/(nb*nc);
    ncb = strlen(tb)/(nb*nc);
    if (nca != ncb)
       return(TRUE);

    for (j = 0; j < nc; j++)
	d[j] = 0.0;

    ex  = 2.0;
    den = 1.0/pow(255.0, ex);

    for (i = 0; i < nca; i++)
        {for (j = 0; j < nc; j++)
	     {for (k = 0; k < nb; k++)
		  {ua[k] = *pta++;
		   ub[k] = *ptb++;};

	      ca = SC_strtol(ua, NULL, 16);
	      cb = SC_strtol(ub, NULL, 16);

	      ca = pow(ca, ex);
	      cb = pow(cb, ex);
	      f  = ((ca - cb)*den);
	      f  = ABS(f);

	      d[j] += f;};};

    ex = 1.0/ex;
    for (j = 0; j < nc; j++)
	d[j] = pow(d[j], ex);

    tol = 3.0;

    for (j = 0; j < nc; j++)
        err |= (d[j] > tol);

    row++;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DIFF_LINE - compare a line of difference */

static int diff_line(char *bfa, char *bfb, pixdes *pd, int verbose,
		     char *ecnt, char *ea, char *eb)
   {int n, errl, more, pl;
    int ia, ib, di, na, bpp;
    double da, db, dd, toler;
    char tba[MAXLINE], tbb[MAXLINE];
    char *ta, *tb, *pa, *pb;

    errl = 0;

/* some SGI libraries have a problem printing floating point numbers
 * so for example where another platform prints
 *      -6.02e+22
 * the SGI prints
 *      -0.60e+23
 * this limits the comparison tolerance severely
 * we'll assume at least three digits are printed - two after the decimal
 * and limit the tolerance therefore to 1.0e-2
 */
#ifdef SGI
    toler = 1.0e-2;
#else
    toler = 1.0e-10;
#endif

/* ignore PostScript comments (they begin with %) */
    if ((bfa[0] == '%') && (bfb[0] == '%'))
       return(errl);

    else
       {if (verbose)
	   {printf("< (%d) %s", (int) strlen(bfa), bfa);
	    printf("> (%d) %s", (int) strlen(bfb), bfb);};

	bpp  = pd->nb;
	pl   = bpp*30;   /* 30 RGB pixels at a time */
	errl = 0;
	more = TRUE;
	while (more)
	   {n = strlen(bfa);
	    if (n > MAXLINE)
	       {SC_strncpy(tba, MAXLINE, bfa, pl);

		strcpy(bfa, bfa+pl);
		pa = tba;

		SC_strncpy(tbb, MAXLINE, bfb, pl);

		strcpy(bfb, bfb+pl);
		pb = tbb;}
	    else
	       {pa   = bfa;
		pb   = bfb;
		more = FALSE;};

	    while (errl == 0)
	       {ta = SC_firsttok(pa, " \t\n\f\r()");
		tb = SC_firsttok(pb, " \t\n\f\r()");
		if ((ta == NULL) && (tb == NULL))
		   break;

		if (((ta != NULL) && (tb == NULL)) ||
		    ((ta == NULL) && (tb != NULL)))
		   {errl = TRUE;
		    break;};

/* if the tokens are the same OK */
		if (strcmp(ta, tb) == 0)
		   continue;

/* NOTE: if we have images to compare the entire line will be one
 *       very long word
 *       check that no byte differs by more than one
 */
		na = strlen(ta);
		if (na > 60)
		   {if (imav == TRUE)
		       errl = check_image_ave(ta, tb, pd);
		    else
		       errl = check_image_loc(ta, tb, pd);}

/* if they are integer check equality to tolerance */
		else if (SC_intstrp(ta, 10) && SC_intstrp(tb, 10))
		   {ia   = SC_stoi(ta);
		    ib   = SC_stoi(tb);
		    di   = ia - ib;
		    di   = abs(di);
		    errl = (di > n_pixel);}

/* if they are double check equality to tolerance */
		else if (SC_fltstrp(ta) && SC_fltstrp(tb))
		   {da   = SC_stof(ta);
		    db   = SC_stof(tb);
		    dd   = (da - db)/(ABS(da) + ABS(db) + SMALL);
		    errl = (ABS(dd) > toler);}

/* otherwise it is an error */
		else
		   errl = TRUE;};};

	if (errl)
	   {printf("%s", ecnt);
	    printf("%s", ea);
	    printf("---\n");
	    printf("%s", eb);};};

    return(errl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PS_DIFF_A - compare the specified PostScript files upto roundoff
 *           - this algorithm uses the diff utility to find all
 *           - differences in the text and then we analyze what
 *           - diff reports
 *           - we really want to check items that look like
 *           -   3357c3357
 *           -   < 1 0 0 1 213 1615 0 Tp
 *           -   ---
 *           -   > 1 0 0 1 212 1615 0 Tp
 *           - or
 *           -   3366c3366
 *           -   < ( 100    0.00e+00   1.00e+01   -1.00e-00   1.00e+00) Tx
 *           -   ---
 *           -   > ( 100    0.00e+00   1.00e+01   -1.00e+00   1.00e+00) Tx
 */

static int ps_diff_a(char *f1, char *f2, pixdes *pd, int verbose)
   {int err, la, lb, st;
    char *ea, *eb, *ecnt, *bfa, *bfb, *cmnd;
    FILE *fd;
    static int n = BFSZ;
    static char *s = NULL;

    cmnd = SC_dsnprintf(FALSE, "diff %s %s > __tmp__", f1, f2);
    st   = system(cmnd);
    SC_ASSERT(st == 0);

    fd = io_open("__tmp__", "r");

    la = 0;
    lb = 0;

    err = 0;

    ea   = NULL;
    eb   = NULL;
    ecnt = NULL;
    bfa  = NULL;
    bfb  = NULL;

    while (TRUE)
       {s = SC_dgets(s, &n, fd);
	if (s == NULL)
           {io_close(fd);
	    unlink("__tmp__");
	    break;};

/* copy the A file difference */
	if (s[0] == '<')
	   {ea  = SC_dstrcpy(ea, s);
	    bfa = SC_dstrcpy(bfa, s+2);
	    la  = strlen(bfa);}

/* ignore the difference separator: --- */
	else if (strncmp(s, "---", 3) == 0)
           continue;

/* copy the B file difference */
	else if (s[0] == '>')
	   {eb  = SC_dstrcpy(eb, s);
	    bfb = SC_dstrcpy(bfb, s+2);
	    lb  = strlen(bfb);}

/* now process the actual differences in this line */
	else if ((la != 0) && (lb != 0))
	   {err |= diff_line(bfa, bfb, pd, verbose, ecnt, ea, eb);

/* reset for the next difference */
	    la = 0;
	    lb = 0;
	    ecnt = SC_dstrcpy(ecnt, s);}

/* make a copy of the diff line info */
	else
	   ecnt = SC_dstrcpy(ecnt, s);

	if (strlen(s) > (n >> 1))
           {n <<= 1;
	    CREMAKE(s, char, n);};};

    CFREE(ea);
    CFREE(eb);
    CFREE(bfa);
    CFREE(bfb);
    CFREE(ecnt);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PS_DIFF_B - compare the specified PostScript files upto roundoff
 *           - this algorithm assumes that differences are small
 *           - enough that a line by line comparison will not get lost
 *           - so we can sensibly simply process pairs of lines
 *           - from each file
 */

static int ps_diff_b(char *f1, char *f2, pixdes *pd, int verbose)
   {int i, err, la, lb;
    char *ea, *eb, *ecnt;
    FILE *fda, *fdb;
    static int na = BFSZ, nb = BFSZ;
    static char *sa = NULL, *sb = NULL;

    fda = io_open(f1, "r");
    fdb = io_open(f2, "r");

    la = 0;
    lb = 0;

    err = 0;

    for (i = 0; TRUE; i++)
        {sa = SC_dgets(sa, &na, fda);
	 sb = SC_dgets(sb, &nb, fdb);

	 if ((sa == NULL) || (sb == NULL))
            {io_close(fda);
	     io_close(fdb);
	     break;};

	 ea = SC_dsnprintf(TRUE, "< %s", sa);
	 la = strlen(sa);

	 eb = SC_dsnprintf(TRUE, "> %s", sb);
	 lb = strlen(sb);

	 ecnt = SC_dsnprintf(TRUE, "%d\n", i);

/* now process the actual differences in this line */
	 if ((la != 0) && (lb != 0))
	    err |= diff_line(sa, sb, pd, verbose, ecnt, ea, eb);

	 CFREE(ea);
	 CFREE(eb);
	 CFREE(ecnt);

	 if (la > (na >> 1))
            {na <<= 1;
	     CREMAKE(sa, char, na);};

	 if (lb > (nb >> 1))
            {nb <<= 1;
	     CREMAKE(sb, char, nb);};};

    return(err);}

/*--------------------------------------------------------------------------*/

/*                          FRACTIONAL COMpaRISONS                          */

/*--------------------------------------------------------------------------*/

/* DIFF_FRAC - compare a line of difference fractionally */

static int diff_frac(char *bfa, char *bfb, diff_stat *ds,
		     pixdes *pd, int verbose, char *ecnt, char *ea, char *eb)
   {int n, errl, more, dntok;
    int na, pl;
    double da, db, dd;
    double ddmn, ddmx, dav, ddf;
    char tba[MAXLINE], tbb[MAXLINE];
    char *ta, *tb, *pa, *pb;

    ddmn = ds->fdmn;
    ddmx = ds->fdmx;
    errl = 0;

/* ignore PostScript comments (they begin with %) */
    if ((bfa[0] == '%') && (bfb[0] == '%'))
       return(errl);

    else
       {if (verbose)
	   {printf("< (%d) %s", (int) strlen(bfa), bfa);
	    printf("> (%d) %s", (int) strlen(bfb), bfb);};

	pl   = 6*30;   /* 30 RGB pixels at a time - 2 chars per component */
	errl = 0;
	more = TRUE;
	while (more)
	   {n = strlen(bfa);
	    if (n > MAXLINE)
	       {SC_strncpy(tba, MAXLINE, bfa, pl);

		strcpy(bfa, bfa+pl);
		pa = tba;

		SC_strncpy(tbb, MAXLINE, bfb, pl);

		strcpy(bfb, bfb+pl);
		pb = tbb;}
	    else
	       {pa   = bfa;
		pb   = bfb;
		more = FALSE;};

	    
	    dntok = FALSE;
	    while (dntok == 0)
	       {ta = SC_firsttok(pa, " \t\n\f\r()");
		tb = SC_firsttok(pb, " \t\n\f\r()");
		if (((ta != NULL) && (tb == NULL)) ||
		    ((ta == NULL) && (tb != NULL)))
		   {dntok = TRUE;
		    errl  = TRUE;
		    break;};

		if ((ta == NULL) && (tb == NULL))
		   break;

/* if the tokens are the same OK */
		if (strcmp(ta, tb) == 0)
		   continue;

/* NOTE: if we have images to compare the entire line will be one
 *       very long word
 *       check that no byte differs by more than one
 */
		na = strlen(ta);
		if ((na > 60) && (na == ((na >> 1) << 1)))
		   {if (imav == TRUE)
		       errl = check_image_ave(ta, tb, pd);
		    else
		       errl = check_image_loc(ta, tb, pd);}

		else

/* if they are integer check equality to tolerance */
		   {if (SC_intstrp(ta, 10) && SC_intstrp(tb, 10))
		       {da = SC_stoi(ta);
			db = SC_stoi(tb);}

/* if they are double check equality to tolerance */
		    else if (SC_fltstrp(ta) && SC_fltstrp(tb))
		       {da = SC_stof(ta);
			db = SC_stof(tb);}

/* otherwise it is an error */
		    else
		       {errl  = TRUE;
			dntok = TRUE;
		        continue;};

/* NOTE: this is a bit of defensive coding because NCAR PS
 * files can be very ratty wrt stroking text in that
 * the number of points in a polyline can be different
 * so this test is trying to bail if one of the numbers is
 * likely to be the number of points in a polygon and not
 * a coordinate value
 * Sorry!
 */
		    if ((da < 64) || (db < 64))
		       continue;

		    else
		       {dav  = 0.5*(ABS(da) + ABS(db) + SMALL);
			ddf  = (da - db);
			ddf  = ABS(ddf);
			dd   = ddf/dav;
			ddmn = min(ddmn, dd);
			ddmx = max(ddmx, dd);
		       
			if (dd > frac_tol)
			   {errl = TRUE;
			    ds->dsm  += ddf;
			    ds->fdsm += dd;
			    ds->asum += dav;
			    ds->ndiff++;};};};};};

	if (!dntok && ((ddmn < ds->fdmn) || (ds->fdmx < ddmx)))
	   {ds->fdmn = ddmn;
	    ds->fdmx = ddmx;};

	if (errl)
	   {printf("%s", ecnt);
	    printf("%s", ea);
	    printf("---\n");
	    printf("%s", eb);};};

    return(errl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PS_DIFF_FRAC - compare the specified PostScript files to fractional error
 *              - this algorithm assumes that differences are small
 *              - enough that a line by line comparison will not get lost
 *              - so we can sensibly simply process pairs of lines
 *              - from each file
 */

static int ps_diff_frac(char *f1, char *f2, pixdes *pd, int verbose)
   {int i, err, la, lb;
    char *ea, *eb, *ecnt;
    double ddmn, ddmx;
    double nd, dfave;
    FILE *fda, *fdb;
    diff_stat ds;
    static int na = BFSZ, nb = BFSZ;
    static char *sa = NULL, *sb = NULL;

    SC_MEM_INIT_V(&ds);

    fda = io_open(f1, "r");
    fdb = io_open(f2, "r");

    la = 0;
    lb = 0;

    ddmn = HUGE;
    ddmx = -HUGE;
    err  = 0;

    for (i = 0; TRUE; i++)
        {sa = SC_dgets(sa, &na, fda);
	 sb = SC_dgets(sb, &nb, fdb);
	 if ((sa == NULL) || (sb == NULL))
            {io_close(fda);
	     io_close(fdb);
	     break;};

	 ea = SC_dsnprintf(TRUE, "< %s", sa);
	 la = strlen(sa);

	 eb = SC_dsnprintf(TRUE, "> %s", sb);
	 lb = strlen(sb);

	 ecnt = SC_dsnprintf(TRUE, "%d\n", i);

/* now process the actual differences in this line */
	 if ((la != 0) && (lb != 0))
	    err |= diff_frac(sa, sb, &ds, pd, verbose, ecnt, ea, eb);

	 CFREE(ea);
	 CFREE(eb);
	 CFREE(ecnt);

	 if (la > (na >> 1))
            {na <<= 1;
	     CREMAKE(sa, char, na);};

	 if (lb > (nb >> 1))
            {nb <<= 1;
	     CREMAKE(sb, char, nb);};};

    nd    = ds.ndiff + 1;
    dfave = ds.fdsm/nd;

    printf("\n");
    printf("Fractional differences range: %10.3e <= fr <= %10.3e\n",
	   ddmn, ddmx);
    if (dfave < 10.0*frac_tol)
       {err = 0;
	printf("Average fractional difference (pass): %10.3e < %10.3e\n",
	       dfave, 10.0*frac_tol);}
    else
       {err = 1;
	printf("Average fractional difference (fail): %10.3e > %10.3e\n",
	       dfave, 10.0*frac_tol);};

    printf("Specified tolerance:           %10.3e\n", frac_tol);
    printf("\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - given usage info */

static void help(void)
   {

    printf("\n");
    printf("PS-DIFF - compare two PostScript file allowing a\n");
    printf("          pixel difference in positions of lines and text\n");
    printf("\n");

    printf("Usage: ps-diff [-a] [-b] [-f] [-h] [-i] [-ia] [-o n,m] [-v] [-tf r]\n");
    printf("               [-tp n] <file1> <file2>\n");
    printf("\n");

    printf("Options:\n");
    printf("\n");
    printf("   a  - method a\n");
    printf("   b  - method b (default)\n");
    printf("   f  - fractional difference method\n");
    printf("   h  - this help message\n");
    printf("   i  - visually inspect the images interactively\n");
    printf("   ia - row averaged image differences\n");
    printf("   o  - horizontal offsets of the two windows\n");
    printf("        n pixels from the left for <file1> window and\n");
    printf("        m pixels from the right for <file2> window\n");
    printf("   tf - max fractional difference allowed for -f option (default 1.03e-3)\n");
    printf("   tp - max pixel difference allowed for -a or -b option (default 1)\n");
    printf("   v  - verbose reporting of differences\n");
    printf("\n");

    exit(1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int err, verbose, method;
    int i, nf, o1, o2;
    char *file[10];
    pixdes pd;

    verbose = FALSE;
    method  = 2;
    nf      = 0;
    err     = FALSE;

    o1 = 2;
    o2 = 120; 

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
	    {if (strcmp(v[i], "-v") == 0)
	        verbose = TRUE;
	     else if (strcmp(v[i], "-a") == 0)
	        method = 1;
	     else if (strcmp(v[i], "-b") == 0)
	        method = 2;
	     else if (strcmp(v[i], "-f") == 0)
	        method = 4;
	     else if (strcmp(v[i], "-h") == 0)
	        help();
	     else if (strcmp(v[i], "-i") == 0)
	        method = 3;
	     else if (strcmp(v[i], "-ia") == 0)
	        imav = TRUE;
	     else if (strcmp(v[i], "-o") == 0)
	        {o1 = SC_stoi(strtok(v[++i], ",\t\n"));
		 o2 = SC_stoi(strtok(NULL, ", \t\n"));}
	     else if (strcmp(v[i], "-tp") == 0)
	        n_pixel = SC_stoi(v[++i]);
	     else if (strcmp(v[i], "-tf") == 0)
	        frac_tol = SC_stof(v[++i]);
	     else
	        io_printf(stdout, "Unknown ps-diff option: %s\n", v[i]);}
	 else
            file[nf++] = v[i];};

/* 0, for Monochrome, 1 for RGB, and 2 for RGB + transparency */
    pd.clr = 2;

    switch (pd.clr)
       {case 0:
             pd.nb = 2;
             break;

/* 3 bytes (6 chars) per pixel */
        case 1:
	     pd.nb = 6;
             break;

/* 4 bytes (8 chars) per pixel with transparency */
        case 2:
	     pd.nb = 8;
             break;};
    pd.nc = pd.nb >> 1;

    if (nf == 2)
       {switch (method)
	   {case 1 :
	         err = ps_diff_a(file[0], file[1], &pd, verbose);
		 break;
	    case 2 :
	         err = ps_diff_b(file[0], file[1], &pd, verbose);
		 break;
	    case 3 :
	         err = ps_diff_inspect(file[0], file[1], o1, o2, &pd, verbose);
		 break;
	    case 4 :
	         err = ps_diff_frac(file[0], file[1], &pd, verbose);
		 break;};}

    else
       {printf("Too many files specified (%d)\n", nf);
	err = TRUE;};

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
