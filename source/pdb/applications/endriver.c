/*
 * ENDRIVER.C - test driver to read EnSight files
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb.h"
#include "global_extern.h"          /* Any files containing these routines */
                                    /* should include this header file     */

#define MAXTIMES 100
#define MAXPARTS 100

int 
 debug = 1;   /* debug flag */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help()
   {PRINT(STDOUT, "\nENDRIVER - driver to exercise the EnSight PDB reader\n\n");
    PRINT(STDOUT, "Usage: endriver file\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       where file is either a pdbfile containing mappings\n");
    PRINT(STDOUT, "       containing data to be plotted in EnSight or an ASCII\n");
    PRINT(STDOUT, "       problem definition file.\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*
   NEW_SUFFIX - Replace a suffix (insuffix) if it exists with another
                suffix (outsuffix).
                If insuffix is not there append outsuffix.
*/

static char *new_suffix(in, insuffix, outsuffix)
   char *in, *insuffix, *outsuffix;
   {int found = TRUE;
    char in_rev[MAXLINE], insuffix_rev[MAXLINE];
    char *out;
    int len_in, len_insuffix, len_out;

    if (in == NULL)
       return (char *)NULL;

/* look for insuffix at the end of in */
    SC_strrev(strcpy(in_rev, in));
    SC_strrev(strcpy(insuffix_rev, insuffix));

    if (strstr(in_rev, insuffix_rev) != in_rev)
       found = FALSE;

    len_in       = strlen(in);
    len_insuffix = strlen(insuffix);
    len_out      = (found) ? len_in - len_insuffix + strlen(outsuffix) + 1:
                             len_in + strlen(outsuffix) + 1;

    out =  FMAKE_N(char, len_out, "NEW_SUFFIX:out");

    if (found)
       strncpy(out, in, len_in - len_insuffix);
    else
       strcpy(out, in);

    strcat(out, outsuffix);

    return(out);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* Exercise the EnSight PDB reader functions for debugging purposes */

int main(argc, argv)
   int argc;
   char **argv;
   {PDBfile *pdb_file;
    char infile[MAXLINE];
    char dline1[MAXLINE], dline2[MAXLINE];
    char **part_description, **var_description, **var_filename;
    int i, iret;
    int nvariables = -1, ntimesteps = -1, nfiles, nparts;
    int time_step = 0; 
    int gstatus, nstatus, estatus;
    int part_numbers[MAXPARTS], part_types[MAXPARTS];
    int *ijk_dimensions[3], *var_type, *var_classify;
    int ncoords;
    float solution_times[MAXTIMES];
    float *xcoord_array, *ycoord_array, *zcoord_array;
    float *scalar_array;

    if (argc == 2)
       {strcpy(infile, argv[1]);}  /* the name of the geometry or
                                      ASCII problem definition file */
    else
       {print_help();
        exit(1);}

    SC_zero_space(1);

    if ((iret = USERD_set_filenames(infile, NULL, NULL, FALSE)) 
                != Z_OK)
       {printf("USERD_set_filenames returned error--MAIN\n");
        exit(1);}

    if ((ntimesteps = USERD_get_number_of_time_steps()) < 0)
       {printf("USERD_get_number_of_time_steps returned %d\n",
               ntimesteps);
        exit(1);}

    USERD_get_solution_times(solution_times);

    USERD_set_time_step(time_step);

    if ((gstatus = USERD_get_changing_geometry_status()) != Z_STATIC)
       {printf("USERD_get_changing_geometry_status returned unexpected\
 value: %d\n", gstatus);
        exit(1);}

    nstatus = USERD_get_node_label_status();

    estatus = USERD_get_element_label_status();

    nfiles = USERD_get_number_of_files_in_dataset();
    
    USERD_get_description_lines(Z_GEOM, 0, dline1, dline2);

    nparts = USERD_get_number_of_model_parts();

    for (i = 0; i < 3; i++)
        {ijk_dimensions[i] = FMAKE_N(int, nparts, "ENDRIVER:ijk_dimensions[i]");}

    part_description = FMAKE_N(char *, nparts, "ENDRIVER:PARTS_DESCRIPTION");

    for (i = 0; i < nparts; i++)
       {part_description[i] = FMAKE_N(char, Z_BUFL,"ENDRIVER:PARTS_DESCRIPTION[i]");}

    if ((iret = USERD_get_part_build_info(part_numbers, part_types,
                      part_description, NULL, 
                      ijk_dimensions, NULL)) != Z_OK)
       {printf("USERD_get_part_build_info returned error\n");
        exit(1);}

/* allocate space for the coords */
    ncoords = ijk_dimensions[0][0] * ijk_dimensions[0][1] *
              ijk_dimensions[0][2];

    xcoord_array = FMAKE_N(float, ncoords, "ENDRIVER:XCOORD_ARRAY");
    ycoord_array = FMAKE_N(float, ncoords, "ENDRIVER:YCOORD_ARRAY");
    zcoord_array = FMAKE_N(float, ncoords, "ENDRIVER:ZCOORD_ARRAY");

    if ((iret = USERD_get_block_coords_by_component(1, Z_COMPX,
                        xcoord_array)) != Z_OK)
       {printf("USERD_get_block_coords_by_component returned error\
for Z_COMPX\n");
        exit(1);}
    if ((iret = USERD_get_block_coords_by_component(1, Z_COMPY,
                        ycoord_array)) != Z_OK)
       {printf("USERD_get_block_coords_by_component returned error\
for Z_COMPY\n");
        exit(1);}

    if ((iret = USERD_get_block_coords_by_component(1, Z_COMPZ,
                        zcoord_array)) != Z_OK)
       {printf("USERD_get_block_coords_by_component returned error\
for Z_COMPZ\n");
        exit(1);}

    if ((nvariables = USERD_get_number_of_variables()) < 0)
       {printf("USERD_get_number_of_variables returned %d\n",
               nvariables);
        exit(1);}

    var_description = FMAKE_N(char *, nvariables, "ENDRIVER:VAR_DESCRIPTION");
    var_filename    = FMAKE_N(char *, nvariables, "ENDRIVER:VAR_FILENAME");
    var_type        = FMAKE_N(int, nvariables,    "ENDRIVER:VAR_TYPE"); 
    var_classify        = FMAKE_N(int, nvariables,    "ENDRIVER:VAR_CLASSIFY"); 

    for (i = 0; i < nvariables; i++)
        {var_description[i] = FMAKE_N(char, Z_BUFL, "ENDRIVER:VAR_DESCRIPTION[i]");
         var_filename[i]    = FMAKE_N(char, Z_BUFL, "ENDRIVER:VAR_FILENAME[i]");}

    if ((iret = USERD_get_variable_info(var_description,
                                        var_filename,
                                        var_type,
                                        var_classify)) != Z_OK)
       {printf("USERD_get_variable_info returned error--MAIN\n");
        exit(1);}

/* should loop over nvariables and read each */
    scalar_array = FMAKE_N(float, ncoords, "ENDRIVER:SCALAR_ARRAY");
    if ((iret = USERD_get_block_scalar_values(1, 1,
                        scalar_array)) != Z_OK)
       {printf("USERD_get_block_calar_values returned error\n");
        exit(1);}

    exit(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
