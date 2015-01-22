// 
// CHK++.CC - test PDB complex handling against C++
// 
// include "cpyright.h"
// 

#include "pdb.h"

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

int main (int c, char **v)
   {int rv;
    PDBfile *fp;

// test compile these:
//    PD_open, PD_write*, PD_read*, PD_append*, PD_defstr*,
//    PD_inquire*, PD_cd, PD_mkdir*, PD_ls, PD_ln, PD_isdir

    fp = PD_open("c++.dat", "w");
    rv = (fp != NULL);
    PD_write(fp, "v1", "int", &rv);
    PD_close(fp);

    return(rv);}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
