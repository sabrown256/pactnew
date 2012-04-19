/*
 * TPATH.C - test the time history routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "fpact.h"

typedef int (*PFTest)(char *base, char *seq, char *prev, char *mode,
		      long size, double time, int indx, int iter,
		      int steps, int flush);

typedef struct s_dt_2 dt_2;

struct s_dt_2
   {double t;
    short a;
    long b;
    float c;
    char d;};

int
 debug_mode;

FIXNUM
 FF_ID(pathop, PATHOP)(FIXNUM *pnf, char *fname, char *fmode,
		       FIXNUM *psz, FIXNUM *pnp, char *fprev),
 FF_ID(pabrec, PABREC)(FIXNUM *fileid, FIXNUM *pnf,
		       char *fname, FIXNUM *pnt,
		       char *ftype, FIXNUM *pnd, char *ftime),
 FF_ID(paarec, PAAREC)(FIXNUM *fileid, FIXNUM *recid,
		       FIXNUM *pnm, char *fmemb,
		       FIXNUM *pnl, char *flabl),
 FF_ID(paerec, PAEREC)(FIXNUM *fileid, FIXNUM *recid),
 FF_ID(pafrec, PAFREC)(FIXNUM *recid),
 FF_ID(pagrid, PAGRID)(FIXNUM *fileid, FIXNUM *pind,
		       FIXNUM *pnn, char *name,
		       FIXNUM *pnt, char *type, FIXNUM *prid),
 FF_ID(pathfm, PATHFM)(FIXNUM *fileid),
 FF_ID(pawrec, PAWREC)(FIXNUM *fileid, FIXNUM *recid,
		       FIXNUM *pinst, FIXNUM *pnr,
		       void *vr),
 FF_ID(patrnl, PATRNL)(FIXNUM *pnchrs, char *chrs,
		       FIXNUM *pord, FIXNUM *pncpf),
 FF_ID(pfclos, PFCLOS)(FIXNUM *fileid);

static void
 print_help(void);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RUN_TEST - execute a test sequence */

static int run_test(PFTest test, char *base, char *restart,
		    int level, int iter, int opt, int sz)
   {int steps, titer;
    int64_t bytab, bytfb, bytaa, bytfa;
    double time;
    char s[MAXLINE], t[MAXLINE];

    SC_mem_stats(&bytab, &bytfb, NULL, NULL);

    time = SC_wall_clock_time();

    PRINT(STDOUT, "\t\t      %s ", base);

/* test arguments:
 *  char *base   - base family name
 *  char *seq    - suffix for designated file
 *  char *prev   - previous file in family link
 *  char *mode   - file open mode
 *  long size    - maximum file size
 *  double time  - first time value for file
 *  int indx     - first record index for file (instance index)
 *  int iter     - number of iterations to do
 *  int steps    - number of step per iteration
 *  int flush    - cleanup the mess flag
 */

    steps = 11;

/* basic test */
    (*test)(base, "00", NULL, "w", sz,
            0.0, 0, iter, steps, FALSE);

    PRINT(STDOUT, ".");

/* restart test */
    if (opt > 1)
       {titer = 1.1*((double) iter);
        (*test)(base, restart, NULL, "a", sz,
                12.5, 120, titer, steps, FALSE);
        PRINT(STDOUT, ".");}
    else
        PRINT(STDOUT, " ");

/* backup test */
    if (opt > 2)
       {titer = 0.8*((double) iter);
        snprintf(s, MAXLINE, "%sa", base);
        snprintf(t, MAXLINE, "%s.t02", base);
        (*test)(s, "00", t, "w", sz,
                6.0, 55, titer, steps, FALSE);
        PRINT(STDOUT, ".");}
    else
        PRINT(STDOUT, " ");

/* large file size test */
    if (opt > 3)
       {snprintf(s, MAXLINE, "%sb", base);
        (*test)(s, "00", NULL, "w", SC_OPT_BFSZ,
                0.0, 0, iter, steps, level);
        PRINT(STDOUT, ".");}
    else
        PRINT(STDOUT, " ");

    SC_mem_stats(&bytaa, &bytfa, NULL, NULL);

    bytaa -= bytab;
    bytfa -= bytfb;
    time   = SC_wall_clock_time() - time;

    PRINT(STDOUT,
          "   %9d  %9d  %9d      %.2g\n",
          bytaa, bytfa, bytaa - bytfa, time);

    switch (level)
       {case 2  :
             snprintf(s, MAXLINE, "rm %s*.u*", base);
             SYSTEM(s);
        case 1  :
             snprintf(s, MAXLINE, "rm %s*.t*", base);
             SYSTEM(s);
        default :
             break;};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - basic capabilities test */

static int test_1(char *base, char *seq, char *prev, char *mode,
		  long size, double time, int indx, int iter,
		  int steps, int flush)
   {PDBfile *file, *nf;
    int i, j, n, n0, nr, ret, offs, n_members, n_labels, count;
    double *outa;
    char *thname, *thtype, **members, **labels;
    char family[MAXLINE];

    snprintf(family, MAXLINE, "%s.t%s", base, seq);

    file = PA_th_open(family, mode, size, prev);
    if (file == NULL)
       {PRINT(STDOUT, "OPEN FAILED - TEST_1\n");
        exit(1);};

    thname = CSTRSAVE("data");
    thtype = CSTRSAVE("strt");

    n_members = 3;
    n_labels  = n_members - 1;

    members = CMAKE_N(char *, n_members);
    labels  = CMAKE_N(char *, n_labels);

    members[0] = CSTRSAVE("t");
    members[1] = CSTRSAVE("v1");
    members[2] = CSTRSAVE("v2");

    labels[0] = NULL;
    labels[1] = CSTRSAVE("v2 label");

    if (mode[0] != 'a')
       if (PA_th_def_rec(file, thname, thtype, n_members, members, labels) == NULL)
          {PRINT(STDOUT, "DEF_REC FAILED - TEST_1\n");
           exit(1);};

    outa  = CMAKE_N(double, n_members*steps);

    n0    = indx/steps - 1;
    count = 0;
    ret   = TRUE;
    for (n = n0; n < iter; n++)
        {nf = PA_th_family(file);
         if (file != nf)
            {file   = nf;
             count += 100;};

         for (i = 0, j = 0; j < steps; j++, i += n_members)
             {outa[i + 0] = time;
              time       += 0.1;
              outa[i + 1] = count + j;
              outa[i + 2] = count - j;};

         offs  = 0;
         nr    = 1;
         ret  &= PA_th_write(file, thname, thtype, indx, nr, outa);
         if (ret == FALSE)
            {PRINT(STDOUT, "WRITE 1 AT indx = %d FAILED - TEST_1\n", indx);
             exit(1);};
         indx += nr;
         offs += n_members*nr;

         nr    = 3;
         ret  &= PA_th_write(file, thname, thtype, indx, nr, outa+offs);
         if (ret == FALSE)
            {PRINT(STDOUT, "WRITE 2 AT indx = %d FAILED - TEST_1\n", indx);
             exit(1);};
         indx += nr;
         offs += n_members*nr;

         nr    = 2;
         ret  &= PA_th_write(file, thname, thtype, indx, nr, outa+offs);
         if (ret == FALSE)
            {PRINT(STDOUT, "WRITE 3 AT indx = %d FAILED - TEST_1\n", indx);
             exit(1);};
         indx += nr;
         offs += n_members*nr;

         nr    = 5;
         ret  &= PA_th_write(file, thname, thtype, indx, nr, outa+offs);
         if (ret == FALSE)
            {PRINT(STDOUT, "WRITE 4 AT indx = %d FAILED - TEST_1\n", indx);
             exit(1);};
         indx += nr;
         offs += n_members*nr;};

/* test the writing of individual members */
    outa[0] = time;
    outa[1] = 0.5*count + steps;
    outa[2] = 0.5*count - steps;
    if (!PA_th_wr_member(file, thname, members[0], thtype, indx, outa))
       {PRINT(STDOUT, "MEMBER WRITE 0 AT indx = %d FAILED - TEST_1\n", indx);
	exit(1);};

    if (!PA_th_wr_member(file, thname, members[1], thtype, indx, outa+1))
       {PRINT(STDOUT, "MEMBER WRITE 1 AT indx = %d FAILED - TEST_1\n", indx);
	exit(1);};

    if (!PA_th_wr_member(file, thname, members[2], thtype, indx, outa+2))
       {PRINT(STDOUT, "MEMBER WRITE 2 AT indx = %d FAILED - TEST_1\n", indx);
	exit(1);};

    CFREE(outa);

    CFREE(members[0]);
    members[0] = CSTRSAVE(file->name);

    PD_close(file);
    PA_th_trans_link(1, members, -1, 20);

    for (i = 0; i < n_labels; i++)
        CFREE(labels[i]);
    CFREE(labels);

    for (i = 0; i < n_members; i++)
        CFREE(members[i]);
    CFREE(members);

    CFREE(thname);
    CFREE(thtype);

    if (ret)
       {switch (flush)
           {case 2 :
                 snprintf(family, MAXLINE, "rm %s.u*\n", base);
                 SYSTEM(family);

            case 1 :
                 snprintf(family, MAXLINE, "rm %s.t*\n", base);
                 SYSTEM(family);

            default :
                 break;};};

    if (debug_mode)
       SC_mem_map(STDOUT, FALSE);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - test the use of various data types */

static int test_2(char *base, char *seq, char *prev, char *mode,
		  long size, double time, int indx, int iter,
		  int steps, int flush)
   {PDBfile *file, *nf;
    int i, n, n0, nr, ret, offs, n_members, n_labels, count;
    dt_2 *outa;
    char *thname, *thtype, **members, **labels;
    char family[MAXLINE];

    snprintf(family, MAXLINE, "%s.t%s", base, seq);

    file = PA_th_open(family, mode, size, prev);
    if (file == NULL)
       {PRINT(STDOUT, "OPEN FAILED - TEST_2\n");
        exit(1);};

    thname = CSTRSAVE("data");
    thtype = CSTRSAVE("strt");

    n_members = 5;
    n_labels  = n_members - 1;

    members = CMAKE_N(char *, n_members);
    labels  = CMAKE_N(char *, n_labels);

    members[0] = CSTRSAVE("t");
    members[1] = CSTRSAVE("short a");
    members[2] = CSTRSAVE("long b");
    members[3] = CSTRSAVE("float c");
    members[4] = CSTRSAVE("char d");

    labels[0] = NULL;
/*    labels[0] = CSTRSAVE("a vs. t"); */
    labels[1] = CSTRSAVE("b vs. t");
    labels[2] = CSTRSAVE("c vs. t");
    labels[3] = CSTRSAVE("d vs. t");

    if (mode[0] != 'a')
       if (PA_th_def_rec(file, thname, thtype, 5, members, labels) == NULL)
          {PRINT(STDOUT, "DEF_REC FAILED - TEST_2\n");
           exit(1);};

    outa  = CMAKE_N(dt_2, steps);

    n0    = indx/steps - 1;
    count = 0;
    ret   = TRUE;
    for (n = n0; n < iter; n++)
        {nf = PA_th_family(file);
         if (file != nf)
            {file   = nf;
             count++;};

         for (i = 0; i < steps; i++)
             {outa[i].t = time;
              time     += 0.1;
              outa[i].a = (short) (count + 0.5*i);
              outa[i].b = (long)  (count + 0.75*i);
              outa[i].c = (float) (count + i);
              outa[i].d = (char)  (((long) (count + 0.25*i)) % CHAR_MAX);};

         offs = 0;
         nr   = 1;
         ret &= PA_th_write(file, thname, thtype, indx, nr, outa);
         if (ret == FALSE)
            {PRINT(STDOUT, "WRITE 1 AT indx = %d FAILED - TEST_2\n", indx);
             exit(1);};
         offs += nr;
         indx += nr;

         nr   = 3;
         ret &= PA_th_write(file, thname, thtype, indx, nr, outa+offs);
         if (ret == FALSE)
            {PRINT(STDOUT, "WRITE 2 AT indx = %d FAILED - TEST_2\n", indx);
             exit(1);};
         offs += nr;
         indx += nr;

         nr   = 2;
         ret &= PA_th_write(file, thname, thtype, indx, nr, outa+offs);
         if (ret == FALSE)
            {PRINT(STDOUT, "WRITE 3 AT indx = %d FAILED - TEST_2\n", indx);
             exit(1);};
         offs += nr;
         indx += nr;

         nr   = 5;
         ret &= PA_th_write(file, thname, thtype, indx, nr, outa+offs);
         if (ret == FALSE)
            {PRINT(STDOUT, "WRITE 4 AT indx = %d FAILED - TEST_2\n", indx);
             exit(1);};
         offs += nr;
         indx += nr;};

    CFREE(outa);

    CFREE(members[0]);
    members[0] = CSTRSAVE(file->name);

    PD_close(file);
    PA_th_trans_link(1, members, -1, 20);

    for (i = 0; i < n_labels; i++)
        CFREE(labels[i]);
    CFREE(labels);

    for (i = 0; i < n_members; i++)
        CFREE(members[i]);
    CFREE(members);

    CFREE(thname);
    CFREE(thtype);

    if (ret)
       {switch (flush)
           {case 2 :
                 snprintf(family, MAXLINE, "rm %s.u*\n", base);
                 SYSTEM(family);

            case 1 :
                 snprintf(family, MAXLINE, "rm %s.t*\n", base);
                 SYSTEM(family);

            default :
                 break;};};

    if (debug_mode)
       SC_mem_map(STDOUT, FALSE);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_3 - test the use of various data types */

static int test_3(char *base, char *seq, char *prev, char *mode,
		  long size, double time, int indx, int iter,
		  int steps, int flush)
   {PDBfile *file, *nf;
    int i, j, n, n0, ret, n_memba, n_labla, n_membb, n_lablb;
    int inda, indb, offa, offb, nra, nrb;
    double couna, counb, tima, timb;
    double *outb;
    dt_2 *outa;
    char *thnama, *thtypa, *thnamb, *thtypb;
    char **memba, **labla, **membb, **lablb;
    char family[MAXLINE];

    snprintf(family, MAXLINE, "%s.t%s", base, seq);

    file = PA_th_open(family, mode, size, prev);
    if (file == NULL)
       {PRINT(STDOUT, "OPEN FAILED - TEST_3\n");
        exit(1);};

/* define the first record */
    thnama = CSTRSAVE("data-a");
    thtypa = CSTRSAVE("strt-a");

    n_memba = 5;
    n_labla = n_memba - 1;

    memba = CMAKE_N(char *, n_memba);
    labla = CMAKE_N(char *, n_labla);

    memba[0] = CSTRSAVE("t");
    memba[1] = CSTRSAVE("short a");
    memba[2] = CSTRSAVE("long b");
    memba[3] = CSTRSAVE("float c");
    memba[4] = CSTRSAVE("char d");

    labla[0] = NULL;
/*    labla[0] = CSTRSAVE("a vs. t"); */
    labla[1] = CSTRSAVE("b vs. t");
    labla[2] = CSTRSAVE("c vs. t");
    labla[3] = CSTRSAVE("d vs. t");

    if (mode[0] != 'a')
       if (PA_th_def_rec(file, thnama, thtypa, 5, memba, labla) == NULL)
          {PRINT(STDOUT, "DEF_REC A FAILED - TEST_3\n");
           exit(1);};

/* define the second record */
    thnamb = CSTRSAVE("data-b");
    thtypb = CSTRSAVE("strt-b");

    n_membb = 3;
    n_lablb = n_membb - 1;

    membb = CMAKE_N(char *, n_membb);
    lablb = CMAKE_N(char *, n_lablb);

    membb[0] = CSTRSAVE("t");
    membb[1] = CSTRSAVE("v1");
    membb[2] = CSTRSAVE("v2");

    lablb[0] = NULL;
    lablb[1] = CSTRSAVE("v2 label");

    if (mode[0] != 'a')
       if (PA_th_def_rec(file, thnamb, thtypb, n_membb, membb, lablb) == NULL)
          {PRINT(STDOUT, "DEF_REC B FAILED - TEST_3\n");
           exit(1);};

/* generate the data */
    outa  = CMAKE_N(dt_2, steps);
    outb  = CMAKE_N(double, n_membb*steps);

    inda  = indx;
    indb  = indx;

    tima  = time;
    timb  = time;

    n0    = inda/steps - 1;
    n0    = max(n0, 0);
    iter /= 2;
    if (iter <= n0)
       iter = n0+2;

    couna = 0;
    counb = 0;
    ret   = TRUE;
    for (n = n0; n < iter; n++)
        {nf = PA_th_family(file);
         if (file != nf)
            {file   = nf;
             couna += 10;
             counb++;};

         for (i = 0; i < steps; i++)
             {outa[i].t = tima;
              tima     += 0.1;
              outa[i].a = (short) (couna + 0.5*i);
              outa[i].b = (long)  (couna + 0.75*i);
              outa[i].c = (float) (couna + i);
              outa[i].d = (char)  (((long) (couna + 0.25*i)) % CHAR_MAX);};

         for (i = 0, j = 0; j < steps; j++, i += n_membb)
             {outb[i + 0] = timb;
              timb       += 0.1;
              outb[i + 1] = counb + j;
              outb[i + 2] = counb - j;};

         offa = 0;
         offb = 0;

         nra = 1;
         ret &= PA_th_write(file, thnama, thtypa, inda, nra, outa);
         if (ret == FALSE)
            {PRINT(STDOUT, "WRITE 1 AT inda = %d FAILED - TEST_3\n", inda);
             exit(1);};
         offa += nra;
         inda += nra;

         nra  = 3;
         ret &= PA_th_write(file, thnama, thtypa, inda, nra, outa+offa);
         if (ret == FALSE)
            {PRINT(STDOUT, "WRITE 2 AT inda = %d FAILED - TEST_3\n", inda);
             exit(1);};
         offa += nra;
         inda += nra;

         nrb = 1;
         ret &= PA_th_write(file, thnamb, thtypb, indb, nrb, outb);
         if (ret == FALSE)
            {PRINT(STDOUT, "WRITE 1 AT indb = %d FAILED - TEST_3\n", indb);
             exit(1);};
         indb += nrb;
         offb += n_membb*nrb;

         nrb  = 3;
         ret &= PA_th_write(file, thnamb, thtypb, indb, nrb, outb+offb);
         if (ret == FALSE)
            {PRINT(STDOUT, "WRITE 2 AT indb = %d FAILED - TEST_3\n", indb);
             exit(1);};
         indb += nrb;
         offb += n_membb*nrb;

         nrb  = 2;
         ret &= PA_th_write(file, thnamb, thtypb, indb, nrb, outb+offb);
         if (ret == FALSE)
            {PRINT(STDOUT, "WRITE 3 AT indb = %d FAILED - TEST_3\n", indb);
             exit(1);};
         indb += nrb;
         offb += n_membb*nrb;

         nra  = 2;
         ret &= PA_th_write(file, thnama, thtypa, inda, nra, outa+offa);
         if (ret == FALSE)
            {PRINT(STDOUT, "WRITE 3 AT inda = %d FAILED - TEST_3\n", inda);
             exit(1);};
         offa += nra;
         inda += nra;

         nra  = 5;
         ret &= PA_th_write(file, thnama, thtypa, inda, nra, outa+offa);
         if (ret == FALSE)
            {PRINT(STDOUT, "WRITE 4 AT inda = %d FAILED - TEST_3\n", inda);
             exit(1);};
         offa += nra;
         inda += nra;

         nrb  = 5;
         ret &= PA_th_write(file, thnamb, thtypb, indb, nrb, outb+offb);
         if (ret == FALSE)
            {PRINT(STDOUT, "WRITE 4 AT indb = %d FAILED - TEST_3\n", indb);
             exit(1);};
         indb += nrb;
         offb += n_membb*nrb;};

/* clean up the a record */
    CFREE(outa);

    CFREE(memba[0]);
    memba[0] = CSTRSAVE(file->name);

    for (i = 0; i < n_labla; i++)
        CFREE(labla[i]);
    CFREE(labla);

    for (i = 0; i < n_memba; i++)
        CFREE(memba[i]);
    CFREE(memba);

    CFREE(thnama);
    CFREE(thtypa);

/* clean up the b record */
    CFREE(outb);

    CFREE(membb[0]);
    membb[0] = CSTRSAVE(file->name);

    PD_close(file);
    PA_th_trans_link(1, membb, -1, 20);

    for (i = 0; i < n_lablb; i++)
        CFREE(lablb[i]);
    CFREE(lablb);

    for (i = 0; i < n_membb; i++)
        CFREE(membb[i]);
    CFREE(membb);

    CFREE(thnamb);
    CFREE(thtypb);

/* remove requested files */
    if (ret)
       {switch (flush)
           {case 2 :
                 snprintf(family, MAXLINE, "rm %s.u*\n", base);
                 SYSTEM(family);

            case 1 :
                 snprintf(family, MAXLINE, "rm %s.t*\n", base);
                 SYSTEM(family);

            default :
                 break;};};

    if (debug_mode)
       SC_mem_map(STDOUT, FALSE);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_4 - basic capabilities test FORTRAN API */

static int test_4(char *base, char *seq, char *prev, char *mode,
		  long size, double time, int index, int iter,
		  int steps, int flush)
   {PDBfile *file, *nf;
    int i, j, n, n0, ret, offs, n_members, n_labels, count;
    double *outa;
    char *thname, *thtype, **members, **labels;
    char family[MAXLINE];
    FIXNUM nc, nm, np, nr, nt, ord, sz, recid, fileid, nfileid, indx;

/* increment to obtain FORTRAN style indexing */
    indx = index + 1;

    snprintf(family, MAXLINE, "%s.t%s", base, seq);
    nc = strlen(family);
    nm = strlen(mode);
    if (prev != NULL)
       np = strlen(prev);
    else
       np = 0;
    sz     = size;
    fileid = FF_ID(pathop, PATHOP)(&nc, family, mode,
				   &sz, &np, prev);
    file = SC_GET_POINTER(PDBfile, fileid);
    if (file == NULL)
       {PRINT(STDOUT, "OPEN FAILED - TEST_4\n");
        exit(1);};

    thname = CSTRSAVE("data");
    thtype = CSTRSAVE("strt");

    n_members = 3;
    n_labels  = n_members - 1;

    members = CMAKE_N(char *, n_members);
    labels  = CMAKE_N(char *, n_labels);

    members[0] = CSTRSAVE("t");
    members[1] = CSTRSAVE("v1");
    members[2] = CSTRSAVE("v2");

    labels[0] = NULL;
    labels[1] = CSTRSAVE("v2 label");

    nc = strlen(thname);
    nt = strlen(thtype);
    if (mode[0] != 'a')
       {FIXNUM nd, nl;

        nd = strlen(members[0]);
        recid = FF_ID(pabrec, PABREC)(&fileid,
				      &nc, thname,
				      &nt, thtype,
				      &nd, members[0]);
        if (recid == 0L)
           {PRINT(STDOUT, "PABREC FAILED - TEST_4\n");
            exit(1);};
           
        for (i = 1; i < n_members; i++)
            {nm = strlen(members[i]);
             if (labels[i-1] != NULL)
                nl = strlen(labels[i-1]);
	     else
	        nl = 0;
	     if (!FF_ID(paarec, PAAREC)(&fileid, &recid,
					&nm, members[i],
					&nl, labels[i-1]))
	        {PRINT(STDOUT, "PAAREC FAILED - TEST_4\n");
		 exit(1);};};

	if (!FF_ID(paerec, PAEREC)(&fileid, &recid))
	   {PRINT(STDOUT, "PAEREC FAILED - TEST_4\n");
	    exit(1);};}
    else
       {nm = 0;
        FF_ID(pagrid, PAGRID)(&fileid, &nm,
			      &nc, thname,
			      &nt, thtype,
			      &recid);
	if (recid == 0L)
	   {PRINT(STDOUT, "PAGRID FAILED - TEST_4\n");
	    exit(1);};};

    outa  = CMAKE_N(double, n_members*steps);

    n0    = indx/steps - 1;
    count = 0;
    ret   = TRUE;
    for (n = n0; n < iter; n++)
        {nfileid = FF_ID(pathfm, PATHFM)(&fileid);
	 nf      = SC_GET_POINTER(PDBfile, nfileid);
         if (file != nf)
            {file   = nf;
             fileid = nfileid;
             count += 100;};

         for (i = 0, j = 0; j < steps; j++, i += n_members)
             {outa[i + 0] = time;
              time       += 0.1;
              outa[i + 1] = count + j;
              outa[i + 2] = count - j;};

         offs = 0;
         nr   = 1;

         ret = FF_ID(pawrec, PAWREC)(&fileid, &recid, &indx, &nr, outa);
         if (ret == FALSE)
            {PRINT(STDOUT, "WRITE 1 AT indx = %d FAILED - TEST_4\n", indx);
             exit(1);};
         indx += nr;
         offs += n_members*nr;

         nr  = 3;
         ret = FF_ID(pawrec, PAWREC)(&fileid, &recid, &indx, &nr, outa+offs);
         if (ret == FALSE)
            {PRINT(STDOUT, "WRITE 2 AT indx = %d FAILED - TEST_4\n", indx);
             exit(1);};
         indx += nr;
         offs += n_members*nr;

         nr  = 2;
         ret = FF_ID(pawrec, PAWREC)(&fileid, &recid, &indx, &nr, outa+offs);
         if (ret == FALSE)
            {PRINT(STDOUT, "WRITE 3 AT indx = %d FAILED - TEST_4\n", indx);
             exit(1);};
         indx += nr;
         offs += n_members*nr;

         nr  = 5;
         ret = FF_ID(pawrec, PAWREC)(&fileid, &recid, &indx, &nr, outa+offs);
         if (ret == FALSE)
            {PRINT(STDOUT, "WRITE 4 AT indx = %d FAILED - TEST_4\n", indx);
             exit(1);};
         indx += nr;
         offs += n_members*nr;};

    CFREE(outa);

    CFREE(members[0]);
    members[0] = CSTRSAVE(file->name);

    FF_ID(pfclos, PFCLOS)(&fileid);

    nm  = strlen(members[0]);
    ord = -1;
    nc  = 20;
    FF_ID(patrnl, PATRNL)(&nm, members[0], &ord, &nc);

    for (i = 0; i < n_labels; i++)
        CFREE(labels[i]);
    CFREE(labels);

    for (i = 0; i < n_members; i++)
        CFREE(members[i]);
    CFREE(members);

    CFREE(thname);
    CFREE(thtype);

    FF_ID(pafrec, PAFREC)(&recid);

    SC_free_stash();

    if (ret)
       {switch (flush)
           {case 2 :
                 snprintf(family, MAXLINE, "rm %s.u*\n", base);
                 SYSTEM(family);

            case 1 :
                 snprintf(family, MAXLINE, "rm %s.t*\n", base);
                 SYSTEM(family);

            default :
                 break;};};

    if (debug_mode)
       SC_mem_map(STDOUT, FALSE);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nTPATH - run time history test suite\n");
    PRINT(STDOUT, "           tests are:\n");
    PRINT(STDOUT, "             1 - simple test of basic functionality\n");
    PRINT(STDOUT, "             2 - test the use of non-double types\n");
    PRINT(STDOUT, "             3 - test the use of multiple records\n");
    PRINT(STDOUT, "             4 - simple test of FORTRAN API\n");
    PRINT(STDOUT, "\n");

    PRINT(STDOUT, "Usage: tpath [-c #] [-d] [-h] [-i] [-l #] [-v #] [-1] [-2] [-3] [-4]\n");
    PRINT(STDOUT, "\n");

    PRINT(STDOUT, "       c - cleanup level\n");
    PRINT(STDOUT, "           0 remove nothing\n");
    PRINT(STDOUT, "           1 remove time history files only\n");
    PRINT(STDOUT, "           2 remove ULTRA files and time history files\n");

    PRINT(STDOUT, "       d - turn on debug mode to display memory maps\n");
    PRINT(STDOUT, "       h - print this help message and exit\n");
    PRINT(STDOUT, "       i - number of iterations per test (default 10)\n");

    PRINT(STDOUT, "       l - testing level\n");
    PRINT(STDOUT, "           1 basic test only\n");
    PRINT(STDOUT, "           2 restart and basic test\n");
    PRINT(STDOUT, "           3 backup, restart, and basic\n");
    PRINT(STDOUT, "           4 large file, backup, restart, and basic\n");

    PRINT(STDOUT, "       v  - use format version # (default is 2)\n");

    PRINT(STDOUT, "       1 - do NOT run test #1\n");
    PRINT(STDOUT, "       2 - do NOT run test #2\n");
    PRINT(STDOUT, "       3 - do NOT run test #3\n");
    PRINT(STDOUT, "       4 - do NOT run test #4\n");

    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - read the restart dump and run the physics loop */

int main(int c, char **v)
   {int i;
    int test_one, test_two, test_three, test_four;
    int iter, cleanup_level, opt;

    SC_zero_space_n(0, -2);

    SC_init("TPATH: Exit with error", NULL,
            TRUE, NULL, NULL,
            TRUE, NULL, 0);
 
    debug_mode    = FALSE;
    cleanup_level = 1;
    iter          = 10;
    opt           = 4;
    test_one      = TRUE;
    test_two      = TRUE;
    test_three    = TRUE;
    test_four     = TRUE;
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'c' :
		      cleanup_level = atoi(v[++i]);
		      break;
                 case 'd' :
		      debug_mode  = TRUE;
		      SC_gs.mm_debug = TRUE;
		      break;
                 case 'i' :
		      iter = atoi(v[++i]);
		      break;
                 case 'h' :
		      print_help();
		      return(1);
                 case 'l' :
		      opt = atoi(v[++i]);
		      break;
                 case 'v' :
                      PD_set_fmt_version(SC_stoi(v[++i]));
		      break;
                 case '1' :
		      test_one = FALSE;
		      break;
                 case '2' :
		      test_two = FALSE;
		      break;
                 case '3' :
		      test_three = FALSE;
		      break;
                 case '4' :
		      test_four = FALSE;
		      break;};}
         else
            break;};

    SC_signal(SIGINT, SIG_DFL);

    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "\t\t\t\t\t         Memory                Time\n");
    PRINT(STDOUT, "\t\t\t\t\t         (bytes)              (secs)\n");
    PRINT(STDOUT, "\t\t\tTest\t   Allocated     Freed        Diff\n");

    if (test_one)
       run_test(test_1, "thts1", "03", cleanup_level, iter, opt, 1100);

    if (test_two)
       run_test(test_2, "thts2", "03", cleanup_level, iter, opt, 1500);

    if (test_three)
       run_test(test_3, "thts3", "02", cleanup_level, iter, opt, 2200);

    if (test_four)
       run_test(test_4, "thts4", "03", cleanup_level, iter, opt, 1100);

    PRINT(STDOUT, "\n");

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

