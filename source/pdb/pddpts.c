/*
 * PDDPTS.C - Test DP I/O in PDBLib
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb.h"
#include "scope_mpi.h"

/* these are 2**N so they can be decomposed over N processes */
#define N_FLOAT 256
#define N_INT   256
#define N_ELEM  256

#define N_PTS   256 

typedef struct s_l_frame l_frame;
typedef struct s_plot myplot;

struct s_l_frame
   {float x_min;
    float x_max;
    float y_min;
    float y_max;};


struct s_plot
   {float x_axis[N_PTS];
    float y_axis[N_PTS];
    int npts;
    char *label;
    l_frame *view;};

    
static int
 debug,
 *p_int_w,
 *p_int_r;

static float
 x[N_ELEM],
 xr[N_ELEM],
 *p_float_w,
 *p_float_r,
 *p_fserial_w,
 *p_fserial_r;

static myplot
 mypl_w,
 mypl_r;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_LOG - print log message */

void print_log(FILE *log, char *s, int st)
   {

    if (st == TRUE)
       {PRINT(STDOUT,
	      "                       %-12s ................ ok\n",
	      s);
        PRINT(log, "%s ... ok\n", s);}

    else
      {PRINT(STDOUT,
	     "                        %-12s ................ ng\n",
	     s);
       PRINT(log, "%s ... ng\n", s);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_COMM_TYPE - print communicator type to stdout */

void print_comm_type(SC_communicator comm)   
   {

    if (comm == SC_COMM_SELF)           
       printf("MPI_COMM_SELF used\n");  
    else                       
       printf("MPI_COMM_WORLD used\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHECK_MYPLOT - check plot data for errors */

void check_myplot(myplot xplot, myplot rxplot, SC_communicator comm)
   {int i;

    if (strcmp(xplot.label, rxplot.label) != 0)
       {printf("xplot.label differs from rxplot.label\n");
	printf("xplot.label: %s, rxplot.label: %s\n", xplot.label, rxplot.label);
	print_comm_type(comm);};

    if (xplot.view->x_min != rxplot.view->x_min)
       {printf("xplot.view->xmin differs from rxplot.view->xmin\n");
	print_comm_type(comm);};

    if (xplot.view->x_max != rxplot.view->x_max)
       {printf("xplot.view->x_max differs from rxplot.view->x_max\n");
	print_comm_type(comm);};

    if (xplot.view->y_min != rxplot.view->y_min)
       {printf("xplot.view->y_min differs from rxplot.view->y_min\n");
	print_comm_type(comm);};

    if (xplot.view->y_max != rxplot.view->y_max)
       {printf("xplot.view->y_max differs from rxplot.view->y_max\n");
	print_comm_type(comm);};

    if (xplot.npts != rxplot.npts)
       {printf("xplot.npts differs from rxplot.npts\n");
	print_comm_type(comm);};

    for (i = 0; i < N_PTS; i++)
        if (xplot.x_axis[i] != rxplot.x_axis[i])
	   {printf("xplot.x_axis[%d] differs from rxplot.x_axis[%d]\n", i, i);
	    print_comm_type(comm);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_DATA - setup data for tests */

static int setup_data(int rank)
   {int i;

/* put some values in the structures */
    for (i = 0; i < N_PTS; i++)
        mypl_w.x_axis[i] = mypl_w.y_axis[i] = (float)i;

    mypl_w.npts  = N_PTS;
    mypl_w.label = SC_strsavef("Myplot MYPL label", "PDDPTS:main mypl_w");
    mypl_w.view  = FMAKE(l_frame, "Myplot MYPL view");

    mypl_w.view->x_min = 0.1;
    mypl_w.view->x_max = 0.8;
    mypl_w.view->y_min = 0.2;
    mypl_w.view->y_max = 0.7;
   
/* set up the int array */
    p_int_w = FMAKE_N(int, N_INT, "SETUP_DATA:p_int_w");
    if (p_int_w == NULL)
       {printf("Error allocating p_int_w, process %d--quitting\n", rank);
        exit(1);}

    for (i = 0; i < N_INT; i++)
        p_int_w[i] = rank * i;

/* set up the float array */
    p_float_w = FMAKE_N(float, N_FLOAT, "SETUP_DATA:p_float_w");
    if (p_float_w == NULL)
       {printf("Error allocating p_float_w, process %d--quitting\n", rank);
        return(FALSE);};

    for (i = 0; i < N_FLOAT; i++)
        p_float_w[i] = (float)(rank * i);

/* setup the serial array */
    p_fserial_w = FMAKE_N(float, N_FLOAT, "SETUP_DATA:p_fserial_w");
    if (p_fserial_w == NULL)
       {printf("Error allocating p_fserial_w, process %d--quitting\n", rank);
        return(FALSE);};

    for (i = 0; i < N_FLOAT; i++)
        p_fserial_w[i] = (float)(i);

/* setup the distributed array */
    for (i = 0; i < N_ELEM; i++)
        x[i] = (float) rank;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FREE_MEMORY - release the memory from the tests */

static int free_memory()
   {

    SFREE(mypl_w.label);
    SFREE(mypl_w.view);
    SFREE(p_int_w);
    SFREE(p_fserial_w);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_DATA_SERIAL - write data serially to the parallel PDB file */

static int write_data_serial(PDBfile *file, int rank, int numprocs,
		             SC_communicator comm)
   {float float_serial;
    int int_serial, oldflag, rv;

    rv = TRUE;

/* write some stuff serially - must be done on master process only */
    if (rank == 0)
       {oldflag = PD_mp_set_serial(file, TRUE);

        float_serial = FLT_MAX;
        int_serial   = INT_MAX;

        if (!PD_write(file, "float_serial", "float", &float_serial))
           {printf("Error writing fserial: process %d\n", rank);
            rv = FALSE;};

        if (!PD_write(file, "p_fserial", "float *", &p_fserial_w))
           {printf("Error writing p_fserial: process %d\n", rank);
            rv = FALSE;};

        if (!PD_write(file, "int_serial", "int", &int_serial))
           {printf("Error writing iserial: process %d\n", rank);
            rv = FALSE;};

        PD_mp_set_serial(file, oldflag);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_DATA_SCALAR - write the test data to the parallel PDB file */

static int write_data_scalar(PDBfile *file, int rank, int numprocs,
			     SC_communicator comm)
   {float frank;
    int rv;
    char var[MAXLINE];

    rv = TRUE;

/* write an integer */
    snprintf(var, MAXLINE, "rank_%d", rank);
    if (!PD_write(file, var, "integer", &rank))
       {printf("Error writing rank: process %d\n", rank);
        rv = FALSE;};

/* write a float */
    frank = rank;
    snprintf(var, MAXLINE, "frank_%d", rank);
    if (!PD_write(file, var, "float", &frank))
       {printf("Error writing frank: process %d\n", rank);
        rv = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_DATA_ARRAY - write the test data to the parallel PDB file */

static int write_data_array(PDBfile *file, int rank, int numprocs,
			    SC_communicator comm)
   {char var[MAXLINE];
    int rv;

    rv = TRUE;

/* write an int array */
    snprintf(var, MAXLINE, "p_int_%d", rank);
    if (!PD_write(file, var, "int *", &p_int_w))
       {printf("Error writing p_int: process %d\n", rank);
        rv = FALSE;};

/* write an float array */
    snprintf(var, MAXLINE, "p_float_%d", rank);
    if (!PD_write(file, var, "float *", &p_float_w))
       {printf("Error writing p_float: process %d\n", rank);
        rv = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_DATA_STRUCT - write the test data to the parallel PDB file */

static int write_data_struct(PDBfile *file, int rank, int numprocs,
			     SC_communicator comm)
   {char sx_axis[MAXLINE], sy_axis[MAXLINE], var[MAXLINE];
    int i, nwrite, rv;

    rv = TRUE;

/* define the structures to the library */
    PD_defstr(file, "l_frame",
              "float x_min",
              "float x_max",
              "float y_min",
              "float y_max",
              LAST);

    snprintf(sx_axis, MAXLINE, "float x_axis(%d)", N_PTS);
    snprintf(sy_axis, MAXLINE, "float y_axis(%d)", N_PTS);

    PD_defstr(file, "myplot",
              sx_axis,
              sy_axis,
              "integer npts", 
              "char * label",
              "l_frame * view",
               LAST);

/* write a struct */
    snprintf(var, MAXLINE, "myplot_%d", rank);
    if (!PD_write(file, var, "myplot", &mypl_w))
       {printf("Error writing myplot: process %d\n", rank);
        rv = FALSE;};

/* try a PD_defent */
    snprintf(var, MAXLINE, "x[%d]", N_ELEM);
    if (!PD_defent(file, var, "float"))
       {printf("PD_defent failed for x[%d], process %d\n", N_ELEM, rank);
        rv = FALSE;};

    nwrite = (comm == SC_COMM_SELF) ? N_ELEM : N_ELEM / numprocs;
    for (i = 0; i < N_ELEM; i++)
        x[i] = (float) rank;

    if (comm != SC_COMM_SELF)
       snprintf(var, MAXLINE, "x[%d:%d]", rank*nwrite, (rank+1)*nwrite - 1);
    else
       snprintf(var, MAXLINE, "x[0:%d]", N_ELEM-1);

    if (!PD_write(file, var, "float", x))
       {printf("Error writing x, process %d\n", rank);
        rv = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_DATA_SERIAL - make sure what we read is what we wrote */

static int read_data_serial(PDBfile *file, int rank, int numprocs,
			    SC_communicator comm)
   {float float_serial;
    int i, int_serial, rv;

    rv = TRUE;

/* read the serial data */
    if (((comm == SC_COMM_SELF) && (rank == 0)) || (comm == SC_COMM_WORLD))
       {if (!PD_read(file, "p_fserial", &p_fserial_r))
           {printf("Error reading p_fserial: process %d\n", rank);
            rv = FALSE;};

        for (i = 0; i < N_FLOAT; i++)
            if (p_fserial_r[i] != p_fserial_w[i])
               {printf("Error: p_fserial_r[%d] = %g != p_fserial_w[%d] = %g",
                        i, p_fserial_r[i], i, p_fserial_w[i]);
                printf(" Process %d\n", rank);
                print_comm_type(comm);
                rv = FALSE;
                break;};

        if (!PD_read(file, "float_serial", &float_serial))
           {printf("Error reading float_serial: process %d\n", rank);
            rv = FALSE;};

        if (float_serial != FLT_MAX)
           {printf("Error: float_serial_w = %g != float_serial_r = %g\n",
		   float_serial, FLT_MAX);
            rv = FALSE;};

        if (!PD_read(file, "int_serial", &int_serial))
           {printf("Error reading int_serial: process %d\n", rank);
            rv = FALSE;};

        if (int_serial != INT_MAX)
           {printf("Error: int_serial_w = %d != int_serial_r = %d\n",
		   int_serial, INT_MAX);
            rv = FALSE;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_DATA_SCALAR - make sure what we read is what we wrote */

static int read_data_scalar(PDBfile *file, int rank, int numprocs,
			    SC_communicator comm)
   {char var[MAXLINE];
    int rankr, rv;
    float frank;

    rv = TRUE;

/* read an integer */
    snprintf(var, MAXLINE, "rank_%d", rank);
    if (!PD_read(file, var, &rankr))
       {printf("Error reading rank: process %d\n", rank);
        print_comm_type(comm);
        rv = FALSE;};

    if (rankr != rank)
       {printf("Error: rankr != rank: process %d\n", rank);
        print_comm_type(comm);
        rv = FALSE;};

/* read a float */
    snprintf(var, MAXLINE, "frank_%d", rank);
    if (!PD_read(file, var, &frank))
       {printf("Error reading frank: process %d\n", rank);
        print_comm_type(comm);
        rv = FALSE;};

    if (frank != (float)rank)
       {printf("Error: frank != rank: process %d\n", rank);
        print_comm_type(comm);
        rv = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_DATA_ARRAY - make sure what we read is what we wrote */

static int read_data_array(PDBfile *file, int rank, int numprocs,
			   SC_communicator comm)
   {char var[MAXLINE];
    int i, rv;

    rv = TRUE;

/* read an int *array */
    snprintf(var, MAXLINE, "p_int_%d", rank);
    if (!PD_read(file, var, &p_int_r))
       {printf("Error reading p_int: process %d\n", rank);
        print_comm_type(comm);
        rv = FALSE;}

    for (i = 0; i < N_INT; i++)
        if (p_int_r[i] != p_int_w[i])
           {printf("Error: p_int_r[%d] != p_int_w[%d], p_int_r[%d] = %d, p_int_w[%d] = %d",
                    i, i, i, p_int_r[i], i, p_int_w[i]);
            printf(" Process %d\n", rank);
            print_comm_type(comm);
            rv = FALSE;
            break;};

/* read a float array */
    snprintf(var, MAXLINE, "p_float_%d", rank);
    if (!PD_read(file, var, &p_float_r))
       {printf("Error reading p_float: process %d\n", rank);
        rv = FALSE;};

    for (i = 0; i < N_FLOAT; i++)
        if (p_float_r[i] != p_float_w[i])
           {printf("Error: p_float_r[%d] != p_float_w[%d], p_float_r[%d] = %g, p_float_w[%d] = %g",
                    i, i, i, p_float_r[i], i, p_float_w[i]);
            printf(" Process %d\n", rank);
            print_comm_type(comm);
            rv = FALSE;
            break;};

/* free mem allocated in read
 * GOTCHA: this is necessary because the pointer tracking is slightly
 * wrong - by definition, once it is fixed the tests should go
 */
    if (p_int_r != p_int_w)
       {SFREE(p_int_r);};
    if (p_float_r != p_float_w)
       {SFREE(p_float_r);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_DATA_STRUCT - make sure what we read is what we wrote */

static int read_data_struct(PDBfile *file, int rank, int numprocs,
			    SC_communicator comm)
   {char var[MAXLINE];
    int i, rv;

    rv = TRUE;

/* read a struct */
    snprintf(var, MAXLINE, "myplot_%d", rank);
    if (!PD_read(file, var, &mypl_r))
       {printf("Error reading myplot: process %d\n", rank);
        print_comm_type(comm);
        rv = FALSE;};

    check_myplot(mypl_w, mypl_r, comm);

/* read the PD_defented array */
    snprintf(var, MAXLINE, "x[0:%d]", N_ELEM-1);

    if (!PD_read(file, var, xr))
       {printf("Error reading x, process %d\n", rank);
        print_comm_type(comm);
        rv = FALSE;};

    if (comm == SC_COMM_SELF)
       {for (i = 0; i < N_ELEM; i++)
            if (xr[i] != x[i])
               {printf("Error: xr[%d] != x[%d], xr[%d] = %g, x[%d] = %g\n",
                       i, i, i, xr[i], i, x[i]);
                printf(" Process %d\n", rank);
                print_comm_type(comm); 
                rv = FALSE; 
                break;};}
    else
       {int nchunk = N_ELEM / numprocs;
        for (i = rank*nchunk; i < (rank+1)*nchunk; i++)
            if (xr[i] != (float)rank)
               {printf("Error: xr[%d] != rank, xr[%d] = %f, rank = %d\n",
                       i, i, xr[i], rank);
                printf(" Process %d\n", rank);
                print_comm_type(comm);
                rv = FALSE;
                break;};};

/* GOTCHA: this is necessary because the pointer tracking is slightly
 * wrong - by definition, once it is fixed the tests should go
 */
    if (mypl_r.label != mypl_w.label)
       {SFREE(mypl_r.label)};

    if (mypl_r.view != mypl_w.view)
       {SFREE(mypl_r.view);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SERIAL_TEST - test serial I/O in DP file */

static int serial_test(char *path, int rank, int numprocs, FILE *log)
   {int rv;
    char wname[MAXLINE];
    PDBfile *file;

    snprintf(wname, MAXLINE, "%s/dp_serial.pdb", path);

    REMOVE(wname);

/* create the output file */
    file = PD_mp_open(wname, "a", SC_COMM_WORLD);

    if (file == NULL)
       {PRINT(STDOUT, "serial_test: error creating %s\n", wname);
	rv = FALSE;}

    else
       {rv = TRUE;

/* do the SERIAL write */
	rv &= write_data_serial(file, rank, numprocs, SC_COMM_WORLD);

	print_log(log, "serial write", rv);

	PD_flush(file);

	if (!PD_close(file))
	   {PRINT(STDOUT, "serial_test: error closing file %s\r\n", wname);
	    rv = FALSE;}; 

	file = PD_mp_open(wname, "a", SC_COMM_WORLD);

/* do the SERIAL read */
	rv &= read_data_serial(file, rank, numprocs, SC_COMM_SELF);
	rv &= read_data_serial(file, rank, numprocs, SC_COMM_WORLD);

	print_log(log, "serial read", rv);

/* close file */
	PD_flush(file);

	if (!PD_close(file))
	   {PRINT(STDOUT, "serial_test: error closing file %s\r\n", wname);
	    rv = FALSE;};};

    MPI_Barrier(SC_COMM_WORLD);

    if ((rank == 0) && (rv == TRUE))
       {REMOVE(wname);}

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SELF_TEST - DP self test */

static int self_test(char *path, int rank, int numprocs, FILE *log)
   {int rv;
    char sname[MAXLINE];
    PDBfile *file;

    snprintf(sname, MAXLINE, "%s/dp_self%d.pdb", path, rank);

    REMOVE(sname);

/* create the output file */
    file = PD_mp_open(sname, "a", SC_COMM_SELF);

    if (file == NULL)
       {PRINT(STDOUT, "self_test: error creating %s\n", sname);
	rv = FALSE;}

    else
       {rv = TRUE;

/* do the SELF write */
	rv &= write_data_scalar(file, rank, numprocs, SC_COMM_SELF);
	rv &= write_data_array( file, rank, numprocs, SC_COMM_SELF);
	rv &= write_data_struct(file, rank, numprocs, SC_COMM_SELF);

	print_log(log, "self write", rv);

/* do the SELF read */
	rv &= read_data_scalar(file, rank, numprocs, SC_COMM_SELF);
	rv &= read_data_array(file, rank, numprocs, SC_COMM_SELF);
	rv &= read_data_struct(file, rank, numprocs, SC_COMM_SELF);

	print_log(log, "self read", rv);

/* close file */
	PD_flush(file);

	if (!PD_close(file))
	   {PRINT(STDOUT, "self_test: error closing file %s\r\n", sname);
	    rv = FALSE;};};

    MPI_Barrier(SC_COMM_WORLD);

    if (rv == TRUE)
       {REMOVE(sname);}

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WORLD_TEST - test I/O across all DP processes */

static int world_test(char *path, int rank, int numprocs, FILE *log)
   {int rv;
    char wname[MAXLINE];
    PDBfile *file;

    snprintf(wname, MAXLINE, "%s/dp_world.pdb", path);

    REMOVE(wname);

/* create the output file */
    file = PD_mp_open(wname, "a", SC_COMM_WORLD);

    if (file == NULL)
       {PRINT(STDOUT, "world_test: error creating %s\n", wname);
	rv = FALSE;}

    else
       {rv = TRUE;

/* do the WORLD writes */
	rv &= write_data_scalar(file, rank, numprocs, SC_COMM_WORLD);
	rv &= write_data_array( file, rank, numprocs, SC_COMM_WORLD);
	rv &= write_data_struct(file, rank, numprocs, SC_COMM_WORLD);

	print_log(log, "world write", rv);

/* do the WORLD read */
	rv &= read_data_scalar(file, rank, numprocs, SC_COMM_WORLD);
	rv &= read_data_array( file, rank, numprocs, SC_COMM_WORLD);
	rv &= read_data_struct(file, rank, numprocs, SC_COMM_WORLD);

	print_log(log, "world read", rv);

/* close file */
	PD_flush(file);

	if (!PD_close(file))
	   {PRINT(STDOUT, "world_test: error closing file %s\r\n", wname);
	    rv = FALSE;};};

    MPI_Barrier(SC_COMM_WORLD);

    if ((rank == 0) && (rv == TRUE))
       {REMOVE(wname);
        io_close(log);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RUN_TESTS - run the distributed parallel tests */

static int run_tests(char *path, int rank, int numprocs)
   {int rv;
    char fname[MAXLINE];
    FILE *log;

    snprintf(fname, MAXLINE, "pddpts.log");

/* remove any old files */
    REMOVE(fname);

    log = io_open(fname, "w");

    rv  = TRUE;
    rv &= serial_test(path, rank, numprocs, log);
    rv &= self_test(path, rank, numprocs, log);
    rv &= world_test(path, rank, numprocs, log);

    if ((rank == 0) && (rv == TRUE) && (debug == FALSE))
       {REMOVE(fname);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start right here */

int main(int c, char **v)
   {int i, n, pow2, rank, numprocs, success;
    long bc;
    int64_t bsz;
    char path[MAXLINE], msg[MAXLINE];

    strcpy(path, ".");
    debug = FALSE;

    bsz = 1024*1024;

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'b' :
		      bsz = SC_stoi(v[++i]);
		      break;
                 case 'd' :
		      debug = TRUE;
		      break;
                 case 'p' :
                      strcpy(path, v[++i]);
                      break;
                 case 'v' :
                      PD_set_format_version(SC_stoi(v[++i]));
                      break;
                 default:
                      printf("Unknown flag: %c\n", v[i][1]);};}
         else
            break;};

    PD_set_buffer_size(bsz);

    if (debug)
       bc = SC_mem_monitor(-1, 2, "dmp", msg);

    MPI_Init(&c, &v); 
    MPI_Comm_rank(SC_COMM_WORLD, &rank);
    MPI_Comm_size(SC_COMM_WORLD, &numprocs);

/* Make sure we have a power of 2 worth of processes */
   n = numprocs;
   pow2 = 0;
   while (n > 0)
      {if (n & 0x01)
          {if (pow2)
              {pow2 = 0;
               break;}
            else
              {pow2 = 1;}}

       n = n >> 1;}

   if (!pow2) 
      {fprintf(stderr, "The number of processes must be a power of 2\n");
       MPI_Abort(SC_COMM_WORLD, 0);
       exit(0);}

/* Initialize PDBlib for MPI 
 * master process:    0
 * number of threads: 1    (per MPI process?)
 * tid function:      NULL 
 */
    PD_init_mpi(0, 1, NULL);

    success = setup_data(rank);

    if (success)
       {success = run_tests(path, rank, numprocs); 
        free_memory();};

    PD_term_mpi();

    MPI_Finalize();

    if (debug)
       bc = SC_mem_monitor(bc, 2, "dmp", msg);

    return(success != TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

