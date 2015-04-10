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
   {int rv, cs;
    char msg[MAXLINE];
    char *so;
    double d;
    double (*fcos)(double x), (*fsin)(double x);
    static int dbg = 2;

#ifdef MACOSX
    so = "libSystem.B.dylib";
#else
    so = "libm.so";
#endif

    cs = SC_mem_monitor(-1, dbg, "SO", msg);

/* register a specific function */
    rv = SC_so_register_func(OBJ_FUNC, so, "cos", NULL, NULL,
			     "double", "(double x)");

/* register a shared library */
    rv = SC_so_register_func(OBJ_SO, so, "math", NULL, NULL, NULL, NULL);

/* load specifically registered function */
    fcos = SC_so_get(OBJ_FUNC, "cos");
    if (fcos == NULL)
       {printf("Failed to load 'cos'\n");
	rv = FALSE;}

    else
       {d = fcos(2.0);
	printf("cos(2.0) = %.4f\n", d);};

/* load unregistered function from shared object */
    fsin = SC_so_get(OBJ_SO, "math", "sin");
    if (fsin == NULL)
       {printf("Failed to load 'sin'\n");
	rv = FALSE;}

    else
       {d = fsin(2.0);
	printf("sin(2.0) = %.4f\n", d);};

    rv = SC_so_release();

    cs = SC_mem_monitor(cs, dbg, "SO", msg);

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
