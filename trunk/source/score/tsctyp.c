/*
 * TSCTYP.C - test type system
 *
 */

#include "cpyright.h"

#include "score.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - simple sequential write test
 *        - return TRUE iff successful
 */

static int test_1(void)
   {int i, id, st, cs;
    char msg[MAXLINE];
    typdes *td;
    static int dbg = 0;
    char *tchr[] = { "SC_CHAR_I", "SC_WCHAR_I" };
    char *tfix[] = { "SC_INT8_I", "SC_SHORT_I",
		     "SC_INT_I", "SC_LONG_I", "SC_LONG_LONG_I" };
    char *tfp[]  = { "SC_FLOAT_I", "SC_DOUBLE_I", "SC_LONG_DOUBLE_I" };
    char *tcpx[] = { "SC_FLOAT_COMPLEX_I", "SC_DOUBLE_COMPLEX_I",
		     "SC_LONG_DOUBLE_COMPLEX_I" };
    char *tal[]  = { "SC_FILE_I", "SC_ENUM_I", "SC_PBOOLEAN_I",
		     "SC_SSIZE_I", "SC_INTEGER_I", "SC_INT16_I",
		     "SC_INT32_I", "SC_INT64_I",
		     "SC_REAL_I", "SC_FLOAT32_I", "SC_FLOAT64_I",
		     "SC_FLOAT128_I", "SC_COMPLEX32_I", "SC_COMPLEX64_I",
		     "SC_COMPLEX128_I"};

    st = TRUE;

    cs = SC_mem_monitor(-1, dbg, "T1", msg);

/* char types */
    printf("\t\t\tCharacter types:\n");
    for (i = 0; i < N_PRIMITIVE_CHAR; i++)
        {id = SC_TYPE_CHAR_ID(i);
	 td = SC_gs.stl + id;
         printf("   %-20s %3d %3d %3d %s\n",
		td->typ_s, i, id, td->typ_i, tchr[i]);};
    printf("\n");

/* fix point types */
    printf("\t\t\tFixed point types:\n");
    for (i = 0; i < N_PRIMITIVE_FIX; i++)
        {id = SC_TYPE_FIX_ID(i);
	 td = SC_gs.stl + id;
         printf("   %-20s %3d %3d %3d %s\n",
		td->typ_s, i, id, td->typ_i, tfix[i]);};
    printf("\n");

/* floating point types */
    printf("\t\t\tFloating point types:\n");
    for (i = 0; i < N_PRIMITIVE_FP; i++)
        {id = SC_TYPE_FP_ID(i);
	 td = SC_gs.stl + id;
         printf("   %-20s %3d %3d %3d %s\n",
		td->typ_s, i, id, td->typ_i, tfp[i]);};
    printf("\n");

/* complex types */
    printf("\t\t\tComplex types:\n");
    for (i = 0; i < N_PRIMITIVE_CPX; i++)
        {id = SC_TYPE_CPX_ID(i);
	 td = SC_gs.stl + id;
         printf("   %-20s %3d %3d %3d %s\n",
		td->typ_s, i, id, td->typ_i, tcpx[i]);};
    printf("\n");

/* aliased types */
    printf("\t\t\tAliased types:\n");
    for (id = SC_FILE_I; id <= SC_COMPLEX128_I; id++)
        {td = SC_gs.stl + id;
	 i  = id - SC_FILE_I;
         printf("   %-20s %3d %3d %3d %s\n",
		td->typ_s, i, id, td->typ_i, tal[i]);};
    printf("\n");


    io_printf(stdout, "\t\t\ttype indeces .............. %s\n",
	      (st == TRUE) ? "ok" : "ng");

    cs = SC_mem_monitor(cs, dbg, "T1", msg);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - simple sequential read test (using file from test_1)
 *        - return TRUE iff successful
 */

static int test_2(void)
   {int st;

    st  = TRUE;

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_3 - test "random" writes
 *        - return TRUE iff successful
 */

static int test_3(void)
   {int st;

    st  = TRUE;

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_4 - test random reads from file of test_3
 *        - return TRUE iff successful
 */

static int test_4(void)
   {int st;

    st  = TRUE;

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test mapped files */

int main(int c, char **v)
   {int i, rv, st;
    int ts[4];

    SC_setbuf(stdout, NULL);

    for (i = 0; i < 5; i++)
        ts[i] = TRUE;

/* process the command line arguments */
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case '1' :
		      ts[0] = FALSE;
		      break;
                 case '2' :
		      ts[1] = FALSE;
		      break;
                 case '3' :
		      ts[2] = FALSE;
		      break;
                 case '4' :
		      ts[3] = FALSE;
		      break;

	         case 'h' :
		      printf("\nUsage: tsctyp [-1] [-2] [-3] [-4] [-h]\n");
		      printf("   Options:\n");
		      printf("      1 - do not do seq write test\n");
		      printf("      2 - do not do seq read test\n");
		      printf("      3 - do not do rnd write test\n");
		      printf("      4 - do not do rnd read test\n");
		      printf("      h - print this help message\n");
		      printf("\n");
		      exit(1);};}
         else
            break;};

    st = TRUE;

    if (ts[0] == TRUE)
       st &= test_1();

    if (ts[1] == TRUE)
       st &= test_2();

    if (ts[2] == TRUE)
       st &= test_3();

    if (ts[3] == TRUE)
       st &= test_4();

    io_printf(stdout, "\n");

/* reverse the sense of the test to exit with 0 iff successful */
    rv = (st == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
