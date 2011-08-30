/*
 * PNTEST.C - test for the PDB file independent stuff
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb.h"

#define NBYTES 100000
#define DATA_SRC "cray"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test the PDB Library system */

int main(int c, char **v)
   {hasharr *chrt;
    FILE *fp;
    size_t bpi, ni;
    char in[NBYTES];
    float a[1000];
    double b[1000];

                         .
                         .
                         .
                         .

/* target the file as asked */
    chrt = PN_target(&CRAY_STD, &WORD8_ALIGNMENT);

/* make a few defstructs */
/*
    PN_defstr(chrt, "l_frame", &WORD8_ALIGNMENT, 0,
              "float x_min",
              "float x_max",
              "float y_min",
              "float y_max",
              LAST);
    PN_defstr(strm, "plot", &WORD8_ALIGNMENT, 0,
              "float x_axis(10)",
              "float y_axis(10)",
              "int npts", 
              "char *label",
              "l_frame view",
              LAST);
*/

    fp = io_open(fname, BINARY_MODE_RPLUS);

/* read in and convert 100 floats */
    bpi = PN_sizeof("float", chrt);
    ni  = 100;
    SC_fread_sigsafe(in, bpi, ni, fp);
    PN_conv_in(a, in, "float", ni, chrt);

/* read in and convert 500 doubles */
    bpi = PN_sizeof("double", chrt);
    ni  = 500;
    SC_fread_sigsafe(in, bpi, ni, fp);
    PN_conv_in(b, in, "double", ni, chrt);

    io_close(fp);

    exit(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
