/*
 * SCMMOV.C - measure overhead of system memory manager
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

typedef struct s_statedes statedes;

struct s_statedes
   {int mm;
    long ni;
    int nbi;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MEASURE - measure memory manager overhead */

static int measure(statedes *st, long n)
   {int nbi, ov;
    long i, sz;
    double mem[2];
    double **x;
    SC_rusedes ru;

    nbi = st->nbi;

    x = CMAKE_N(double *, n);

    SC_resource_usage(&ru, -1);

    if (st->mm == 0)
       {for (i = 0; i < n; i++)
            {x[i]  = malloc(nbi);
	     *x[i] = 0.0;};}
    else
       {for (i = 0; i < n; i++)
	    {x[i]  = CMAKE(double);
	     *x[i] = 0.0;};};

    SC_free_mem(mem);

    SC_resource_usage(&ru, -1);
    sz = ru.maxrss;
    ov = (1000.0*sz - n*nbi + nbi - 1)/((double) n);

    if (mem[1] > 0.01*mem[0])
       printf("%12ld %8d %10.2f %10.2f %10d\n",
	      n, nbi, 1.0e-6*n*nbi, 1.0e-3*sz, ov);
    else
       {printf("memory maxed - results meaningless\n");
	ov = -1;};

    return(ov);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test SC_exec_server */

int main(int c, char **v, char **envp)
   {int i, rv;
    statedes st;

    st.mm  = 0;
    st.ni  = 1000000;
    st.nbi = sizeof(double);

/* process the command line arguments */
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'b' :
		      st.nbi = SC_stoi(v[++i]);
		      break;

	         case 'h' :
		      printf("\nUsage: scmmov [-b #] [-h] [-m #] [-n #]\n");
		      printf("   Options:\n");
		      printf("      b - number of bytes per block\n");
		      printf("      h - print this help message\n");
		      printf("      m - memory manager: 0 system, 1 SCORE\n");
		      printf("      n - number of items to allocate\n");
		      printf("\n");
		      exit(1);

                 case 'm' :
                      st.mm = SC_stoi(v[++i]);
		      break;

                 case 'n' :
                      st.ni = SC_stoi(v[++i]);
		      break;};}
         else
            break;};

    rv = measure(&st, st.ni);
    rv = (rv == -1);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
