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
# define PDB_TIME
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nPDTIME - performance timing test\n\n");
    PRINT(STDOUT, "Usage: pdtime [-h] [-v #] <array-len> <n-iter> [<std> <align>]\n\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       h  - print this help message and exit\n");
    PRINT(STDOUT, "       v  - use format version # (default is 2)\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "<array-len> is the number of elements in the array\n");
    PRINT(STDOUT, "<n-iter> is the number of times the array is written\n");
    PRINT(STDOUT, "<std> is the pdb file data standard\n");
    PRINT(STDOUT, "<align> is the pdb file data alignment\n\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it all here */

/* to find the peak rate for a given array length, start with a number of
 * iterations such that the elapsed time is around 1 second and then vary it.
 * For shorter elapsed times, timing resolution effects cause noisy results.
 * For longer elapsed times, file length effects(?) cause byte rates to drop.
 */

int main(int c, char **v)
   {int i, it, len, n, size, bytes;
    char filename[20];
    double dt, *f;
    PD_data_std_i is;
    PD_data_algn_i ia;
#ifdef PDB_TIME
    char s[20], **names;
    PDBfile *pdbf;
#else
    FILE *binf;
#endif

    len = -1;
    n   = -1;
    is  = PD_NO_STD;
    ia  = PD_NO_ALGN;

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'h' :
		      print_help();
		      return(1);
                 case 'v' :
                      PD_set_fmt_version(SC_stoi(v[++i]));
		      break;};}
         else if (len == -1)
	    len = SC_stoi(v[i]);
         else if (n == -1)
	    n = SC_stoi(v[i]);
         else if (is == PD_NO_STD)
	    {it = SC_stoi(v[i]);
	     it = max(it, 0);
	     it = min(it, PD_N_STANDARDS);
	     is = it;
	     PD_gs.req_standard = &PD_gs.standards[is];
	     printf("Data Standard  = %d\n", is);}
         else if (ia == PD_NO_ALGN)
	    {it = SC_stoi(v[i]);
	     it = max(it, 0);
	     it = min(it, PD_N_ALIGNMENTS);
	     ia = it;
	     PD_gs.req_alignment = &PD_gs.alignments[ia];
	     printf("Data Alignment = %d\n", ia);};};

    SC_signal(SIGINT, SIG_DFL);

    setvbuf(stdout, NULL, _IONBF, 0);

    if (c < 3)
       {print_help();
        exit(0);};

#ifdef PDB_TIME
    names = CMAKE_N(char *, n);
    for (i = 0; i < n; i++)
        {snprintf(s, 20, "f%d-%d(%d)", len, i, len);
	 names[i] = CSTRSAVE(s);};
    SC_zero_space_n(1, -2);
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

    if ((f = CMAKE_N(double, len)) == NULL)
       {printf("\nCAN'T ALLOCATE %d DOUBLES\n\n", len);
	exit(1);};

    for (i = 0; i < len; i++)
        f[i] = i;

    dt = SC_wall_clock_time();

#ifdef PDB_TIME
    for (i = 0; i < n; i++)
	PD_write(pdbf, names[i], SC_DOUBLE_S, f);
#else
    for (i = 0; i < n; i++)
        io_write(f, (size_t) size, (size_t) len, binf);
#endif

    dt = SC_wall_clock_time() - dt;

    printf("%8d   %8d   %11.3e %11.3e\n\n", len, n,
	   dt, ((double) bytes)/dt);

    CFREE(f);

#ifdef PDB_TIME
    PD_close(pdbf);
#else
    io_close(binf);
#endif

    exit(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
