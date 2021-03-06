/*
 * TPDNRM.C - test floating point denormalized value fix
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

#define DENORM_FILE "tpdnrm.pdb"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nTPDNRM - test denormalized value handling\n\n");
    PRINT(STDOUT, "Usage: tpdnrm [-h] [-v #]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       h  - print this help message and exit\n");
    PRINT(STDOUT, "       v  - use format version # (default is 2)\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i, status;
    float* fvalue;
    double* dvalue;
    PDBfile *file;

    status = 0;

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'h' :
		      print_help();
		      return(1);
                 case 'v' :
                      PD_set_fmt_version(SC_stoi(v[++i]));
		      break;};}
         else
            break;};

/* open a file so we have a standard */
    file = PD_open(DENORM_FILE, "w");

    if (file == NULL)
       {fprintf(stderr, "Error creating tester.pdb\n");
        return(-1);};

    fvalue = CMAKE_N(float, 3);
    dvalue = CMAKE_N(double, 2);

    if (file->std->fx[PD_LONG_I].order == REVERSE_ORDER)

/* create some denormalized float values */
       {*((char *) fvalue)     = 0x01;  /* fvalue[0] */
	*((char *) fvalue + 1) = 0x00;
	*((char *) fvalue + 2) = 0x00;
	*((char *) fvalue + 3) = 0x00;
	*((char *) fvalue + 4) = 0x40;  /* fvalue[1] */
	*((char *) fvalue + 5) = 0x01;
	*((char *) fvalue + 6) = 0x00;
	*((char *) fvalue + 7) = 0x00;
	*((char *) fvalue + 8) = 0x40;  /* fvalue[2] */
	*((char *) fvalue + 9) = 0x01;
	*((char *) fvalue + 10) = 0x1F;
	*((char *) fvalue + 11) = 0x02;
  
/* create some denormalized double values */
	*((char *) dvalue)     = 0x01;  /* dvalue[0] */
	*((char *) dvalue + 1) = 0x00;
	*((char *) dvalue + 2) = 0x40;
	*((char *) dvalue + 3) = 0x00;
	*((char *) dvalue + 4) = 0x00;
	*((char *) dvalue + 5) = 0x00;
	*((char *) dvalue + 6) = 0x00;
	*((char *) dvalue + 7) = 0x00;
	*((char *) dvalue + 8) = 0x00;  /* dvalue[1] */
	*((char *) dvalue + 9) = 0x01;
	*((char *) dvalue + 10) = 0x00;
	*((char *) dvalue + 11) = 0xB0;
	*((char *) dvalue + 12) = 0x00;
	*((char *) dvalue + 13) = 0x0F;
	*((char *) dvalue + 14) = 0x10;
	*((char *) dvalue + 15) = 0x20;}

    else
       {*((char *) fvalue)     = 0x00;  /* fvalue[0] */
	*((char *) fvalue + 1) = 0x00;
	*((char *) fvalue + 2) = 0x00;
	*((char *) fvalue + 3) = 0x01;
	*((char *) fvalue + 4) = 0x00;  /* fvalue[1] */
	*((char *) fvalue + 5) = 0x00;
	*((char *) fvalue + 6) = 0x01;
	*((char *) fvalue + 7) = 0x40;
	*((char *) fvalue + 8) = 0x02;  /* fvalue[2] */
	*((char *) fvalue + 9) = 0x1F;
	*((char *) fvalue + 10) = 0x01;
	*((char *) fvalue + 11) = 0x40;
  
/* create some denormalized double values */
	*((char *) dvalue)     = 0x00;  /* dvalue[0] */
	*((char *) dvalue + 1) = 0x00;
	*((char *) dvalue + 2) = 0x00;
	*((char *) dvalue + 3) = 0x00;
	*((char *) dvalue + 4) = 0x00;
	*((char *) dvalue + 5) = 0x40;
	*((char *) dvalue + 6) = 0x00;
	*((char *) dvalue + 7) = 0x01;
	*((char *) dvalue + 8) = 0x20;  /* dvalue[1] */
	*((char *) dvalue + 9) = 0x10;
	*((char *) dvalue + 10) = 0x0F;
	*((char *) dvalue + 11) = 0x00;
	*((char *) dvalue + 12) = 0xB0;
	*((char *) dvalue + 13) = 0x00;
	*((char *) dvalue + 14) = 0x01;
	*((char *) dvalue + 15) = 0x00;};

    PD_fix_denorm(file->std, "float", 3, fvalue);
    PD_fix_denorm(NULL, "double", 2, dvalue);

    if (fvalue[0] != 0.0 || fvalue[1] != 0.0 || fvalue[2] == 0.0)
       {printf("Error in denormalization test (float)\n");
        printf("fvalue[0] = %f fvalue[1] = %f fvalue[2] = %f\n",
	       fvalue[0], fvalue[1], fvalue[2]);
        status = -1;}

    if (dvalue[0] != 0.0 || dvalue[1] == 0.0)
       {printf("Error in denormalization test (double)\n");
        printf("dvalue[0] = %f dvalue[1] = %f\n", dvalue[0], dvalue[1]);
        status = -1;}

    PD_close(file);

    REMOVE(DENORM_FILE);
    CFREE(fvalue);
    CFREE(dvalue);
 
    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

