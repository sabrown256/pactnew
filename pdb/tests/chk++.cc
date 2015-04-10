// 
// CHK++.CC - test PDB complex handling against C++
// 
// include "cpyright.h"
// 

#include "pdb.h"

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

int main (int c, char **v)
   {int rv, rd;
    defstr *dp;
    PDBfile *fp;

// test compile these:
//    PD_open, PD_write*, PD_read*, PD_append*, PD_defstr*,
//    PD_inquire*, PD_cd, PD_mkdir*, PD_ls, PD_ln, PD_isdir

    fp = PD_open("c++.dat", "w");
    rv = (fp != NULL);

    PD_write(fp, "v1", "int", &rv);
    PD_read(fp, "v1", &rd);
    PD_append(fp, "v1", &rd);

    dp = PD_defstr(fp, "foo", "int a", NULL);
    dp = PD_inquire_type(fp, "foo");
    rv = (dp != NULL);

    PD_mkdir(fp, "/d1");
    PD_cd(fp, "/d1");
    rd = PD_isdir(fp, "/d1");
    PD_ln(fp, "/d1", "/d2");
    PD_ls(fp, "/d1", NULL, NULL);

    PD_close(fp);

    return(rv);}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
