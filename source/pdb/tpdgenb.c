/*
 * TPDGENB.C - generate small file with some test doubles
 *
 */

#include "stdlib.h"
#include "stdio.h"
#include "math.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - write file with 4 native floating point numbers */

int main(int c, char **v)
   {int i, nw, err;
    double a[4];
    char *fname;
    FILE *fp;

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {default:
		      break;};}
         else
            {fname = v[i];
             break;};};

    fp = fopen(fname, "wb");

    a[0] = 0.0;
    a[1] = 1.0;
    a[2] = 131.0;
    a[3] = exp(1.0);

    nw = fwrite(a, sizeof(double), 4, fp);
    if (nw != 4)
       err = 1;
    else
       err = 0;

    fclose(fp);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
