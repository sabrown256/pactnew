/*
 * TSCDL.C - test dynamic linking support
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"
#include "scope_dl.h"

typedef struct s_statedes statedes;

struct s_statedes
   {int nbi;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WORK - worker for dynamic linking test */

static int work(statedes *st)
   {int rv;
    double d;
    double (*fcos)(double);

    rv = SC_so_register_func("libm.so", NULL, "double", "cos", "(double x)");

    fcos = SC_so_get_func("cos");
    if (fcos == NULL)
       {printf("Failed to load 'cos'\n");
	rv = FALSE;}

    else
       {d = fcos(2.0);
	printf("cos(2.0) = %.4f\n", d);};

    rv = SC_so_close("cos");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test SC_so_get_func */

int main(int c, char **v, char **envp)
   {int i, rv;
    statedes st;

/* process the command line arguments */
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'h' :
		      printf("\nUsage: tscdl [-h]\n");
		      printf("   Options:\n");
		      printf("      h - print this help message\n");
		      printf("\n");
		      exit(1);

		 default :
		      break;};}
         else
            break;};

    rv = work(&st);

/* reverse the sense */
    rv = (rv == FALSE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
