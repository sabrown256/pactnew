/*
 * PDBFTS.C - test for the PDB I/O buffering
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - test the buffering in PDBLib writes */

static void test_1(long ni, long nc)
   {long i, n;
    long ind[3];
    char *d, *pd;
    char name[MAXLINE];
    PDBfile *strm;

    d = FMAKE_N(char, ni, "TEST_1:d");
    memset(d, 0, ni);

    strm = PD_create("wrn.pdb");

    n = (ni + nc - 1)/nc;
    ind[0] = 0L;
    ind[1] = n - 1;
    ind[2] = 1L;

    for (i = 0; i < nc; i++)
        {pd = d + i*n;
	 snprintf(name, MAXLINE, "v%08ld", i);
	 PD_write_alt(strm, name, "char", pd, 1, ind);};

/* close the file */
    PD_close(strm);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test the PDB Library system */

int main(int c, char **v)
   {int i, vers;
    long n, nc, bfsz;

    PD_init_threads(0, NULL);

    n    = 1000000;
    nc   = 4;
    bfsz = 1000000;
    vers = 3;

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'b' :
		      bfsz = SC_stoi(v[++i]);
		      break;
                 case 'c' :
		      nc = SC_stoi(v[++i]);
		      break;
                 case 'h' :
		      return(1);
                 case 'n' :
		      n = SC_stoi(v[++i]);
		      break;
                 case 'v' :
                      vers = SC_stoi(v[++i]);
		      break;};}
         else
            break;};

    PD_set_format_version(vers);
    PD_set_buffer_size(bfsz);

    test_1(n, nc);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
