/*
 * TPDTD.C - test for the PDB file independent stuff
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "pdb.h"

#define DOUBLE_PRECISION 

#ifdef SINGLE_PRECISION
# define Float float
#endif

#ifdef DOUBLE_PRECISION
# define Float double
#endif

typedef struct s_point point;

struct s_point
   {Float x;
    Float y;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nTPDTD - run PDB typedef test\n\n");
    PRINT(STDOUT, "Usage: tpdtd [-d] [-h] [-v #]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       d  - turn on debug mode to display memory maps\n");
    PRINT(STDOUT, "       h  - print this help message and exit\n");
    PRINT(STDOUT, "       v  - use format version # (default is 2)\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i, err;
    PDBfile *file;
    point *ptr;
  
    err = 0;
  
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'd' :
/*		      SC_gs.mm_debug = TRUE; */
		      break;
                 case 'h' :
		      print_help();
		      return(1);
                 case 'v' :
                      PD_set_fmt_version(SC_stoi(v[++i]));
		      break;};}
         else
            break;};

    file = PD_open("out.pdb", "w");
  
#ifdef SINGLE_PRECISION
    printf("single precision\n");
    PD_typedef(file, "float", "Float");
#else
    printf("double precision\n");
    PD_typedef(file, "double", "Float");
#endif

    PD_defstr(file, "point",
	      "Float x", 
	      "Float y",
	      LAST);

    ptr = CMAKE(point);
    ptr->x = 1.0;
    ptr->y = 2.0;

    if (PD_write(file, "data", "point", ptr) == 0)
       printf("PD_write: %s\n", PD_get_error());

    PD_close(file);
  
    CFREE(ptr); 
    ptr = CMAKE(point);
  
    file = PD_open("out.pdb", "r");
  
    if (PD_read(file, "data", ptr) == 0)
       printf("PD_read: %s\n", PD_get_error());
  
    PD_close(file);
  
    if ((ptr->x != 1.0) || (ptr->y != 2.0))
       err = 1;
   
    CFREE(ptr);
  
    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

