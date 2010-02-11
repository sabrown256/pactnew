/*
 * PDTIME.C - PDBLib performance test
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb.h"

#if 1
#define PDB_TIME
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it all here */

/* To find the peak rate for a given array length, start with a number of
 * iterations such that the elapsed time is around 1 second and then vary it.
 * For shorter elapsed times, timing resolution effects cause noisy results.
 * For longer elapsed times, file length effects(?) cause byte rates to drop.
 */

int main(argc, argv)
   int argc;
   char **argv;
   {int i, len, n, is, ia, size, bytes;
    char filename[20];
    double dt, *f;
#ifdef PDB_TIME
    char s[20], **names;
    PDBfile *pdbf;
#else
    FILE *binf;
#endif

    SC_signal(SIGINT, SIG_DFL);

    setvbuf(stdout, NULL, _IONBF, 0);

    if (argc < 3)
       {printf("\nUsage: pdtime <array-len> <n-iter> [<std> <align>]\n\n");
        printf("<array-len> is the number of elements in the array\n");
        printf("<n-iter> is the number of times the array is written\n");
        printf("<std> is the pdb file data standard\n");
        printf("<align> is the pdb file data alignment\n\n");
        exit(0);};

    len = atoi(argv[1]);
    n   = atoi(argv[2]);

    if (argc == 5)
       {is = atoi(argv[3]);
	ia = atoi(argv[4]);
	REQ_STANDARD  = PD_std_standards[is - 1];
	REQ_ALIGNMENT = PD_std_alignments[ia - 1];
	printf("Data Standard  = %d\nData Alignment = %d\n", is, ia);};
	
#ifdef PDB_TIME
    names = MAKE_N(char *, n);
    for (i = 0; i < n; i++)
        {snprintf(s, 20, "f%d-%d(%d)", len, i, len);
	 names[i] = SC_strsave(s);};
    SC_zero_space(0);
    strcpy(filename, "time.pdb");
    if ((pdbf = PD_open(filename, "w")) == NULL)
       {printf("Time couldn't create PDB file %s\r\n", filename);
        exit(1);};
#else
    strcpy(filename, "time.bin");
    if ((binf = io_open(filename, BINARY_MODE_W)) == NULL)
       {printf("Time couldn't create binary file %s\r\n", filename);
	exit(1);};
#endif

    size  = sizeof(double);
    bytes = len*n*size;

    printf("\nArray Size  Iterations    Time        Rate\n");
    printf("(elements)                (sec)    (bytes/sec)\n");

    if ((f = MAKE_N(double, len)) == NULL)
       {printf("\nCAN'T ALLOCATE %d DOUBLES\n\n", len);
	exit(1);};

    for (i = 0; i < len; i++)
        f[i] = i;

    dt = SC_wall_clock_time();

#ifdef PDB_TIME
    for (i = 0; i < n; i++)
	PD_write(pdbf, names[i], "double", f);
#else
    for (i = 0; i < n; i++)
        io_write(f, (size_t) size, (size_t) len, binf);
#endif

    dt = SC_wall_clock_time() - dt;

    printf("%8d   %8d   %11.3e %11.3e\n\n", len, n,
	   dt, ((double) bytes)/dt);

    SFREE(f);

#ifdef PDB_TIME
    PD_close(pdbf);
#else
    io_close(binf);
#endif

    exit(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
