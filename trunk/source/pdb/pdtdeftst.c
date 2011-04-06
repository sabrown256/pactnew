/*
 * PDTDEFTST.C - test for the PDB file independent stuff
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

int main(int c, char **v)
   {int err;
    PDBfile *file;
    point *ptr;
  
    err = 0;
  
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
  
    if (ptr->x != 1.0 || ptr->y != 2.0)
       err = 1;
   
    CFREE(ptr);
  
    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

