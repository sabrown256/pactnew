/*
 * TPDCORE0.C - core PDB test #0
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#if 0
# include "pdbtfr.h"
# define DATFILE "nat"
#endif

#define N_CHAR  10

struct s_fr0
   {float x_min;
    float x_max;
    float y_min;
    float y_max;};

typedef struct s_fr0 fr0;

struct s_plot0
   {float x_axis[N_CHAR];
    float y_axis[N_CHAR];
    int npts;
    char *label;
    fr0 view;};

typedef struct s_plot0 plot0;

extern long _PD_lookup_size(char *s, hasharr *tab);

/*--------------------------------------------------------------------------*/

/*                            TEST #0 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* PRINT_TEST_0_DATA - print it out to STDOUT */

static void print_test_0_data(PDBfile *strm, FILE *fp)
   {int nbp;

/* print scalars */
    nbp = _PD_lookup_size("char", strm->chart);
    PRINT(fp, "size(char) = %d\n", nbp);

    nbp = _PD_lookup_size("short", strm->chart);
    PRINT(fp, "size(short) = %d\n", nbp);

    nbp = _PD_lookup_size("int", strm->chart);
    PRINT(fp, "size(int) = %d\n", nbp);

    nbp = _PD_lookup_size("long", strm->chart);
    PRINT(fp, "size(long) = %d\n", nbp);

    nbp = _PD_lookup_size("long_long", strm->chart);
    PRINT(fp, "size(long_long) = %d\n", nbp);

    nbp = _PD_lookup_size("float", strm->chart);
    PRINT(fp, "size(float) = %d\n", nbp);

    nbp = _PD_lookup_size("double", strm->chart);
    PRINT(fp, "size(double) = %d\n", nbp);

#ifdef HAVE_ANSI_FLOAT16
    nbp = _PD_lookup_size("long_double", strm->chart);
    PRINT(fp, "size(long double) = %d\n", nbp);
#endif

    nbp = _PD_lookup_size("*", strm->chart);
    PRINT(fp, "size(char *) = %d\n", nbp);

    PRINT(fp, "\n");

/* whole struct test */
    nbp = _PD_lookup_size("fr0", strm->chart);
    PRINT(fp, "size(fr0) = %d\n", nbp);

    nbp = _PD_lookup_size("plot0", strm->chart);
    PRINT(fp, "size(plot0) = %d\n", nbp);

    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_0_DATA - compare the test data */

static int compare_test_0_data(PDBfile *strm, FILE *fp)
   {int nbp, nbc, err, err_tot;

    err     = TRUE;
    err_tot = TRUE;

/* compare primitive types */
    nbp  = _PD_lookup_size("char", strm->host_chart);
    nbc  = sizeof(char);
    err &= (nbp == nbc);

    nbp  = _PD_lookup_size("short", strm->host_chart);
    nbc  = sizeof(short);
    err &= (nbp == nbc);

    nbp  = _PD_lookup_size("int", strm->host_chart);
    nbc  = sizeof(int);
    err &= (nbp == nbc);

    nbp  = _PD_lookup_size("long", strm->host_chart);
    nbc  = sizeof(long);
    err &= (nbp == nbc);

    nbp  = _PD_lookup_size("long_long", strm->host_chart);
    nbc  = sizeof(int64_t);
    err &= (nbp == nbc);

    nbp  = _PD_lookup_size("float", strm->host_chart);
    nbc  = sizeof(float);
    err &= (nbp == nbc);

    nbp  = _PD_lookup_size("double", strm->host_chart);
    nbc  = sizeof(double);
    err &= (nbp == nbc);

#ifdef HAVE_ANSI_FLOAT16
    nbp  = _PD_lookup_size("long_double", strm->host_chart);
    nbc  = sizeof(long double);
    err &= (nbp == nbc);
#endif

    nbp  = _PD_lookup_size("*", strm->host_chart);
    nbc  = sizeof(char *);
    err &= (nbp == nbc);

    if (err)
       PRINT(fp, "Primitive types compare\n");
    else
       PRINT(fp, "Primitive types differ\n");
    err_tot &= err;

/* compare structures */
    nbp  = _PD_lookup_size("fr0", strm->host_chart);
    nbc  = sizeof(fr0);
    err &= (nbp == nbc);

    nbp  = _PD_lookup_size("plot0", strm->host_chart);
    nbc  = sizeof(plot0);
    err &= (nbp == nbc);

    if (err)
       PRINT(fp, "Structs compare\n");
    else
       PRINT(fp, "Structs differ\n");
    err_tot &= err;

    return(err_tot);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
