/*
 * TPDOP.C - test optimized hyper stride features of PDB
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "pdb.h"

#define DATA_FILE  "pdopttst.dat"
#define FLOAT_TOLERANCE  1e-10

static int
 debug_mode = FALSE;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nTPDOP - run PDB optimized hyperstride tests\n\n");
    PRINT(STDOUT, "Usage: tpdop [-d] [-h] [-v #]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       d - turn on debug mode to display memory maps\n");
    PRINT(STDOUT, "       h - print this help message and exit\n");
    PRINT(STDOUT, "       v - use specified format version (default 2)\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i;
    float nonew[24];   /* these are all half the size of the */
    float middlew[24]; /* space that will be defent for them */
    float allw[24];    /* which will be of dim [2][6][4] */
    float allwstep[48]; 
    float allrstep[48]; 
    float zerostep[48]; 
    float noner[24];   /* over the course of three cases we */
    float middler[24]; /* write and read back non-contiguously, */
    float allr[24];    /* partially contiguously, and contiguously */
    float zero[24];
    PDBfile *file;

    debug_mode = FALSE;
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'd' :
		      debug_mode  = TRUE;
		      SC_gs.mm_debug = TRUE;
		      break;
                 case 'h' :
		      print_help();
		      return(1);
                 case 'v' :
                      PD_set_fmt_version(SC_stoi(v[++i]));
		      break;};}
         else
            break;};

    file = PD_open(DATA_FILE, "w+");
    if (file == NULL)
       {printf("Error creating %s\n", DATA_FILE);
	exit(1);};

/* initialize the write vars to some semi-arbitrary values */
    for (i = 0; i < 24; i++)
        {nonew[i]   = (float) i;
         middlew[i] = (float) i;
         allw[i]    = (float) i;
         zero[i]    = 0.0;
         noner[i]   = 0.0;
         middler[i] = 0.0;
         allr[i]    = 0.0;}; 


/* step == 1 CASES */

/* first case: NONE of the dimension descriptor describe contiguity */
    if (!PD_defent(file, "none[2,6,4]", "float"))
       {printf("PD_defent failed\n");
        exit(2);};

/* write out one "physics" variable such that there
 * are no contiguous regions to optimize on
 */
    if (!PD_write(file, "none[0:1, 0:5, 0:1]", "float", nonew))
       {printf("PD_write failed\n");
        exit(3);};

/* zero out the rest of it */
    if (!PD_write(file, "none[0:1, 0:5, 2:3]", "float", zero))
       {printf("PD_write failed\n");
        exit(3);};

/* read that variable back again such that there
 * are no contiguous regions to optimize on
 */
    if (!PD_read(file, "none[0:1, 0:5, 0:1]", noner))
       {printf("PD_read failed\n");
        exit(4);};

/* check it to make sure it is correct */
    for (i = 0; i < 24; i++)
        {if (abs(nonew[i] - noner[i]) > FLOAT_TOLERANCE)
            {printf("Error in check %f != %f ", nonew[i], noner[i]);
             printf("at position none[%d]\n", i); 
             PD_close(file);
             exit(-1);};};


/* second case: MIDDLE dimension contains contiguity */
    if (!PD_defent(file, "middle[2,6,4]", "float"))
       {printf("PD_defent failed\n");
        exit(5);};

/* write out on "physics" variable such that some 
 * partial contiguous regions exist to optimize upon
 */
    if (!PD_write(file, "middle[0:1, 3:5, 0:3]", "float", middlew))
       {printf("PD_write failed\n");
        exit(6);};

    if (!PD_write(file, "middle[0:1, 0:2, 0:3]", "float", zero))
       {printf("PD_write failed\n");
        exit(6);};

/* read that variable back again */
    if (!PD_read(file, "middle[0:1, 3:5, 0:3]", middler))
       {printf("PD_read failed\n");
        exit(7);};

/* check it to make sure it is correct */
    for (i = 0; i < 24; i++)
        {if (abs(middlew[i] - middler[i]) > FLOAT_TOLERANCE)
            {printf("Error in check %f != %f ", middlew[i], middler[i]);
             printf("at position middle[%d]\n", i);
             PD_close(file);
             exit(-1);};};


/* third case: ALL of the region to write/read is contiguous */
    if (!PD_defent(file, "all[2,6,4]", "float"))
       {printf("PD_defent failed\n");
        exit(8);};

/* write out on "physics" variable such that the  
 * entire region is contiguous to check full optimization 
 */
    if (!PD_write(file, "all[0:0, 0:5, 0:3]", "float", allw))
       {printf("PD_write failed\n");
        exit(9);};

    if (!PD_write(file, "all[1:1, 0:5, 0:3]", "float", zero))
       {printf("PD_write failed\n");
        exit(9);};

/* read that variable back again */
    if (!PD_read(file, "all[0:0, 0:5, 0:3]", allr))
       {printf("PD_read failed\n");
        exit(10);};

/* check it to make sure it is correct */
    for (i = 0; i < 24; i++)
        {if (abs(allw[i] - allr[i]) > FLOAT_TOLERANCE)
            {printf("Error in check %f != %f ", allw[i], allr[i]);
             printf("at position all[%d]\n", i); 
             PD_close(file);
             exit(-1);};};


/* step != 1 CASES */

/* first case: NONE of the dimension descriptor describe contiguity */
    if (!PD_defent(file, "nonestep[2,6,4]", "float"))
       {printf("PD_defent failed\n");
        exit(2);};

/* write out one "physics" variable such that there
 * are no contiguous regions to optimize on
 */
    if (!PD_write(file, "nonestep[0:1, 0:5, 1:3:2]", "float", nonew))
       {printf("PD_write failed\n");
        exit(3);};

/* zero out the rest of it */
    if (!PD_write(file, "nonestep[0:1, 0:5, 0:2:2]", "float", zero))
       {printf("PD_write failed\n");
        exit(3);};

/* read that variable back again such that there
 * are no contiguous regions to optimize on
 */
    if (!PD_read(file, "nonestep[0:1, 0:5, 1:3:2]", noner))
       {printf("PD_read failed\n");
        exit(4);};

/* check it to make sure it is correct */
    for (i = 0; i < 24; i++)
        {if (abs(nonew[i] - noner[i]) > FLOAT_TOLERANCE)
            {printf("Error in check %f != %f ", nonew[i], noner[i]);
             printf("at position nonestep[%d]\n", i);
             PD_close(file);
             exit(-1);};};


/* second case: MIDDLE dimension contains step; 3rd dim is contiguity */
    if (!PD_defent(file, "middlestep[2,6,4]", "float"))
       {printf("PD_defent failed\n");
        exit(5);};

/* write out on "physics" variable such that some
 * partial contiguous regions exist to optimize upon
 */
    if (!PD_write(file, "middlestep[0:1, 1:5:2, 0:3]", "float", middlew))
       {printf("PD_write failed\n");
        exit(6);};

    if (!PD_write(file, "middlestep[0:1, 0:4:2, 0:3]", "float", zero))
       {printf("PD_write failed\n");
        exit(6);};

/* read that variable back again */
    if (!PD_read(file, "middlestep[0:1, 1:5:2, 0:3]", middler))
       {printf("PD_read failed\n");
        exit(7);};

/* check it to make sure it is correct */
    for (i = 0; i < 24; i++)
        {if (abs(middlew[i] - middler[i]) > FLOAT_TOLERANCE)
            {printf("Error in check %f != %f ", middlew[i], middler[i]);
             printf("at position middlestep[%d]\n", i);
             PD_close(file);
             exit(-1);};};


/* third case: OUTTER dim steps, hence only 2nd dim in is contiguous
 *             This is the only step != 1 case that causes optimization
 */
    for (i = 0; i < 48; i++)
        {allwstep[i] = (float) i;
         allrstep[i] = 0.0; 
         zerostep[i] = 0.0;};

    if (!PD_defent(file, "allstep[4,6,4]", "float"))
       {printf("PD_defent failed\n");
        exit(8);};

/* write out on "physics" variable such that the
 * entire region is contiguous to check full optimization
 */
    if (!PD_write(file, "allstep[0:2:2, 0:5, 0:3]", "float", allwstep))
       {printf("PD_write failed\n");
        exit(9);};

    if (!PD_write(file, "allstep[1:3:2, 0:5, 0:3]", "float", zerostep))
       {printf("PD_write failed\n");
        exit(9);};

/* read that variable back again */
    if (!PD_read(file, "allstep[0:2:2, 0:5, 0:3]", allrstep))
       {printf("PD_read failed\n");
        exit(10);};

/* check it to make sure it is correct */
    for (i = 0; i < 24; i++)
        {if (abs(allwstep[i] - allrstep[i]) > FLOAT_TOLERANCE)
            {printf("Error in check %f != %f ", allwstep[i], allrstep[i]);
             printf("at position allstep[%d]\n", i);
             PD_close(file);
             exit(-1);};};

    PD_close(file);

    SC_remove(DATA_FILE);

    exit(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
