/*
 * PDSMP.C - Test SMP I/O in PDBLib
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

#define NPTS 10

typedef struct s_l_frame l_frame;
typedef struct s_plot myplot;

struct s_l_frame
   {float x_min;
    float x_max;
    float y_min;
    float y_max;};

struct s_plot
   {float x_axis[NPTS];
    float y_axis[NPTS];
    int npts;
    char *label;
    l_frame *view;};

float
 d[100],
 d2[100];

int
 j[100],
 j2[100],
 ok_count,
 opt = 2,
 n_iter = 10,
 n_err = 0;

myplot
 mypl;

PDBfile
 *file,
 *file2;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RELEASE - release a chunk of memory after checking its
 *         - status wrt the file pointer list
 *         - this is mainly diagnostics for sorting out reference counts
 *         - it really doesn't have much to do with parallel at all
 */

static void release(void *p, char *lbl, int op)
   {extern void dprap(PDBfile *file);

    if (p != NULL)
       SFREE(p);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_PLOT - initialize the plot A */

static void setup_plot(myplot *xp, int whch)
   {int i;

    xp->label = SC_strsavef("Myplot Xplot label", "char*:SETUP_PLOT:label");
    xp->view  = FMAKE(l_frame, "SETUP_PLOT:view");

    if (whch == 0)
       {mypl.view->x_min = 0.1;
	mypl.view->x_max = 0.8;
	mypl.view->y_min = 0.2;
	mypl.view->y_max = 0.7;}

    else
       {xp->view->x_min = 0.15;
	xp->view->x_max = 0.85;
	xp->view->y_min = 0.25;
	xp->view->y_max = 0.75;};

    xp->npts = 10;
    for (i = 0; i < 10; i++)
        xp->x_axis[i] = xp->y_axis[i] = i;

    SC_mark(xp->view, 1);
    SC_mark(xp->label, 1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEAN_PLOT - release the contenst of XP */

static void clean_plot(myplot *xp, int op)
   {

    release(xp->view, "view", op);
    release(xp->label, "label", op);

    xp->label = NULL;
    xp->view  = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_PLOT - allocate and initialize a myplot instance */

myplot *make_plot(void)
   {int i;
    myplot *xp;

    xp = FMAKE(myplot, "MAKE_PLOT:xp");

    xp->label = SC_strsavef("Dynamic XP label", "char*:MAKE_PLOT:xp");
    xp->view  = FMAKE(l_frame, "MAKE_PLOT:view");

    xp->view->x_min = 0.2;
    xp->view->x_max = 0.9;
    xp->view->y_min = 0.33;
    xp->view->y_max = 0.99;
    xp->npts        = 10;

    for (i = 0; i < 10; i++)
        xp->x_axis[i] = xp->y_axis[i] = 4.0*i;

    SC_mark(xp->view, 1);
    SC_mark(xp->label, 1);

/* NOTE: this is unusual usage but is correct reference counting
 * the local variable in the caller is this reference
 */
    SC_mark(xp, 1);

    return(xp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FREE_PLOT - release the myplot instance XP */

static void free_plot(myplot *xp, int op)
   {

    clean_plot(xp, op);

    release(xp, "plot", op);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHECK_MYPLOT - compare plots A and B for content */

static int check_myplot(int tid, myplot *a, myplot *b, char *s)
   {int i, ok;

    ok = TRUE;

    if ((ok == TRUE) && (a->npts != b->npts))
       {PRINT(stdout, "%d> a->npts differs from b->npts in %s\n", tid, s);
        ok = FALSE;};

    if (ok == TRUE)
       {for (i = 0; i < 10; i++)
	    {if (a->x_axis[i] != b->x_axis[i])
	        {PRINT(stdout, "%d> a->x_axis[%d] differs from b->x_axis[%d] in %s\n",
		       tid, i, i, s);
		 ok = FALSE;};};};

    if ((ok == TRUE) && (a->label == NULL))
       {PRINT(stdout, "%d> a->label is NULL %s\n",
	      tid, s);
        ok = FALSE;};

    if ((ok == TRUE) && (b->label == NULL))
       {PRINT(stdout, "%d> b->label is NULL %s\n",
	      tid, s);
        ok = FALSE;};

    if ((ok == TRUE) && (strcmp(a->label, b->label) != 0))
       {PRINT(stdout, "%d> a->label differs from b->label in %s\n",
	      tid, s);
        ok = FALSE;};

    if ((ok == TRUE) && (a->view->x_min != b->view->x_min))
       {PRINT(stdout, "%d> a->view->xmin differs from b->view->xmin in %s\n",
	      tid, s);
        ok = FALSE;};

    if ((ok == TRUE) && (a->view->x_max != b->view->x_max))
       {PRINT(stdout, "%d> a->view->x_max differs from b->view->x_max in %s\n",
	      tid, s);
        ok = FALSE;};

    if ((ok == TRUE) && (a->view->y_min != b->view->y_min))
       {PRINT(stdout, "%d> a->view->y_min differs from b->view->y_min in %s\n",
	      tid, s);
        ok = FALSE;};

    if ((ok == TRUE) && (a->view->y_max != b->view->y_max))
       {PRINT(stdout, "%d> a->view->y_max differs from b->view->y_max in %s\n",
	      tid, s);
        ok = FALSE;};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITEITD - write fully dynamic variables to the already opened file
 *          - the number of variables written
 *          - is a function of the number of iterations specified
 *          - thread 0 does this
 */

static void *writeitd(void *arg)
   {int n;
    myplot *xp;
    char sname1[100], sname2[100], sname3[100], suffix[10];

    xp = make_plot();
         
    strcpy(sname1, "mypl_wr1a"); 
    strcpy(sname2, "xpl_wr1a"); 
    strcpy(sname3, "mypl_wr1b"); 

    for (n = 0; n < n_iter; n++)    
        {snprintf(suffix, 10, "%d", n);

         strcpy(sname1+9, suffix);
         strcpy(sname2+8, suffix);
         strcpy(sname3+9, suffix);

        if (!PD_write(file, sname1, "myplot", &mypl))
           {PRINT(stdout, "Error writing %s - exiting\n", sname1);
            exit(1);}

        if (!PD_write(file, sname2, "myplot *", &xp))
           {PRINT(stdout, "Error writing %s - exiting\n", sname2);
            exit(1);}

        if (!PD_write(file, sname3, "myplot", &mypl))
           {PRINT(stdout, "Error writing %s - exiting\n", sname3);
            exit(1);};}

    ok_count++;

    if (opt == 1)
       free_plot(xp, 1);

    return(NULL);}
     
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITEITS - write partially static variables to the already opened file
 *          - the number of variables written
 *          - is a function of the number of iterations specified
 *          - thread 1 does this
 */

static void *writeits(void *arg)
   {int n;
    char sname1[100], sname2[100], suffix[MAXLINE];
    myplot xplot;

    setup_plot(&xplot, 1);

    strcpy(sname1, "mypl_wr2a"); 
    strcpy(sname2, "xpl_wr2a"); 

    for (n = 0; n < n_iter; n++)
        {snprintf(suffix, MAXLINE, "%d", n);

         strcpy(sname1+9, suffix);
         strcpy(sname2+8, suffix);

         if (!PD_write(file, sname1, "myplot", &xplot))
            {PRINT(stdout, "Error writing %s - exiting\n", sname1);
             exit(1);}       

         if (!PD_write(file, sname2, "myplot", &xplot))
            {PRINT(stdout, "Error writing %s - exiting\n", sname2);
             exit(1);};}

    ok_count++;

    if (opt == 1)
       clean_plot(&xplot, 1);
 
    return NULL;}
     
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READITD - read fully dynamic variables from the already opened file
 *         - the number of variables read
 *         - is a function of the number of iterations specified
 *         - thread 0 does this
 */

void *readitd(void *arg)
   {int n, err;
    char sname1[100], sname2[100], sname3[100], suffix[10];
    myplot *xp, *rxp, rmypl;
    SC_THREAD_ID(tid);

    xp = make_plot();

    strcpy(sname1, "mypl_wr1a"); 
    strcpy(sname2, "xpl_wr1a"); 
    strcpy(sname3, "mypl_wr1b"); 

    err = TRUE;

    for (n = 0; n < n_iter; n++)
        {snprintf(suffix, 10, "%d", n);

         strcpy(sname1+9, suffix);
         strcpy(sname2+8, suffix);
         strcpy(sname3+9, suffix);

         if (!PD_read(file, sname1, &rmypl))
            {PRINT(stdout, "Error reading %s - exiting\n", sname1);
             exit(1);}

/* compare with original */
         err &= check_myplot(tid, &mypl, &rmypl, sname1);

/* free the memory */    
	 if (opt == 1)
	    clean_plot(&rmypl, 0);

         if (!PD_read(file, sname2, &rxp))
            {PRINT(stdout, "Error reading %s - exiting\n", sname2);
             exit(1);}

/* compare with original */
         err &= check_myplot(tid, xp, rxp, sname2);

/* free the memory */ 
	 if (opt == 1)
	    free_plot(rxp, 0);

         if (!PD_read(file, sname3, &rmypl))
            {PRINT(stdout, "Error reading %s - exiting\n", sname3);
             exit(1);};

/* compare with original */
         err &= check_myplot(tid, &mypl, &rmypl, sname3);

/* free the memory */
	 if (opt == 1)
	    clean_plot(&rmypl, 0);};

    ok_count++;

    if (opt == 1)
       free_plot(xp, 0);

    return(NULL);}
     
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READITS - read partially static variables from the already opened file
 *         - the number of variables read
 *         - is a function of the number of iterations specified
 *         - thread 1 does this
 */

void *readits(void *arg)
   {int n, err;
    char sname1[100], sname2[100], suffix[MAXLINE];
    myplot xplot, rxplot;
    SC_THREAD_ID(tid);

    setup_plot(&xplot, 1);

    strcpy(sname1, "mypl_wr2a"); 
    strcpy(sname2, "xpl_wr2a"); 

    err = TRUE;

    for (n = 0; n < n_iter; n++)
        {snprintf(suffix, MAXLINE, "%d", n);

         strcpy(sname1+9, suffix);
         strcpy(sname2+8, suffix);

         if (!PD_read(file, sname1, &rxplot))
            {PRINT(stdout, "Error reading %s - exiting\n", sname1);
             exit(1);}       

/* compare with original */
         err &= check_myplot(tid, &xplot, &rxplot, sname1);

/* free the memory */
	 if (opt == 1)
	    clean_plot(&rxplot, 0);

         if (!PD_read(file, sname2, &rxplot))
            {PRINT(stdout, "Error reading %s - exiting\n", sname2);
             exit(1);};

/* compare with original */
         err &= check_myplot(tid, &xplot, &rxplot, sname2);

/* free the memory */
	 clean_plot(&rxplot, 0);};

    ok_count++;

    if (opt == 1)
       clean_plot(&xplot, 0);
 
    return(NULL);}
     
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

void print_help(void)
   {

    PRINT(STDOUT, "\nPDSMP - run basic PDB smp test\n\n");
    PRINT(STDOUT, "Usage: pdsmp [-h] [-i n] [-m #] [-v #]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       h - print this help message and exit\n");
    PRINT(STDOUT, "       i - set number of iterations in reading and writing loops to n\n");
    PRINT(STDOUT, "       m - memory strategy: 1, do not track pointers;\n");
    PRINT(STDOUT, "           2, track pointers and reset between read and write\n");
    PRINT(STDOUT, "       v - use specified format version (default 2)\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here
 *      - as of 4/26/06 the following table shows the SMP parallel status
 *      - see comments on OPT below
 *      -
 *      -   Format\OPT       0       1        2
 *      -      II            OK      OK       NG
 *      -      III           OK      NG       OK
 *      -
 *      - these may leak memory but they do not crash
 */

int main(int c, char **v)
   {int i, nthreads, rv;
    int nt[2];
    void *(*fnc[2])(void *);

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'h' :
                      print_help();
                      return(1);
                 case 'i' :
                      n_iter = atol(v[++i]);
                      break;
                 case 'm' :
                      opt = atol(v[++i]);
                      break;
                 case 'v' :
                      PD_set_format_version(SC_stoi(v[++i]));
                      break;};}
         else
            break;};

    ok_count = 0;

    PRINT(stdout, "%d iterations per thread\n", n_iter);

/* load up the output arrays */
    for (i = 0; i < 100; i++)
        {d[i] = d2[i] = (float) i;
         j[i] = j2[i] = i;}

    nthreads = 5;

/* initialize PDBlib for 5 threads (includes main thread) */
    PD_init_threads(nthreads, NULL);

/* create the output file */
    file = PD_open("ptest.pdb", "w");
    if (file == NULL)
       {PRINT(stdout, "Error creating ptest.pdb\n");
        return(1);};

/* define the structures to the library */
    PD_defstr(file, "l_frame",
              " float x_min",
              "float x_max",
              "float y_min",
              "float y_max",
              LAST);

    PD_defstr(file, "myplot",
              "float x_axis(10)",
              "float y_axis(10)",
              "integer npts", 
              "char * label",
              "l_frame * view",
               LAST);

/* if opt is equal to one then turn off pointer tracking
 * in this case we expect to have to free lots of memory
 * following the reads because each read will have its own
 * fresh set of pointees
 */
    if (opt == 1)
       PD_set_track_pointers(file, FALSE);

/* put some values in the structures */
    setup_plot(&mypl, 0);

    nt[0] = 1;
    nt[1] = 1;

/* two threads write the file */
    fnc[0] = writeitd;
    fnc[1] = writeits;
    SC_do_threads(2, nt, fnc, NULL, NULL);

/* if opt is equal to two then reset the pointer lists
 * between the writes and reads
 * in this case we expect to have to free very little memory
 * following the reads because the writes kept writing the
 * same pointers over and over
 */
    if (opt == 2)
       PD_reset_ptr_list(file);

/* flush the tables to the file */
    if (!PD_flush(file))
       {PRINT(stdout, "Error flushing file before reads\n");
        return(1);};

    PD_close(file);

    if ((file = PD_open("ptest.pdb", "r")) == NULL)
       {PRINT(stdout, "Error re-opening ptest.pdb\n");
        return(1);};

/* two threads read the file */
    fnc[0] = readitd;
    fnc[1] = readits;
    SC_do_threads(2, nt, fnc, NULL, NULL);

/* close file */
    PD_close(file);

    rv = n_err/n_iter;
    if ((rv == 0) && (ok_count != 4))
       rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

