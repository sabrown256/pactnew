/*
 * PDNORMTST.C - Test floating pt denormalized value fix
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb.h"

#define DENORM_FILE "pdnormtst.pdb"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int argc, char **argv)
   {PDBfile *file;
    float* fvalue;
    double* dvalue;
    int status;

    status = 0;

    /* Open a file so we have a standard */
    file = PD_open(DENORM_FILE, "w");

    if (file == NULL)
       {fprintf(stderr, "Error creating tester.pdb\n");
        return(-1);};

    fvalue = MAKE_N(float, 3);
    dvalue = MAKE_N(double, 2);

    if (file->std->fx[0].order == REVERSE_ORDER)

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
    SFREE(fvalue);
    SFREE(dvalue);
 
    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

