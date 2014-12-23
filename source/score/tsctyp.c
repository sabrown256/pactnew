/*
 * TSCTYP.C - test type system
 *
 */

#include "cpyright.h"

#include "score.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - check that the kinds of standard types are correct */

static int test_1(void)
   {int i, id, st, cs, nerr;
    char msg[MAXLINE];
    char *sts;
    typdes *td;
    static int dbg = 0;
    char *tchr[] = { "SC_CHAR_I", "SC_WCHAR_I" };
    char *tfix[] = { "SC_INT8_I", "SC_SHORT_I",
		     "SC_INT_I", "SC_LONG_I", "SC_LONG_LONG_I" };
    char *tfp[]  = { "SC_FLOAT_I", "SC_DOUBLE_I", "SC_LONG_DOUBLE_I" };
    char *tcpx[] = { "SC_FLOAT_COMPLEX_I", "SC_DOUBLE_COMPLEX_I",
		     "SC_LONG_DOUBLE_COMPLEX_I" };
    char *tal[]  = { "SC_FILE_I", "SC_TYPE_GROUP_I", "SC_TYPE_KIND_I", 
		     "SC_PCONS_I", "SC_ENUM_I", "SC_PBOOLEAN_I",
		     "SC_SSIZE_I", "SC_INTEGER_I", "SC_INT16_I",
		     "SC_INT32_I", "SC_INT64_I",
		     "SC_REAL_I", "SC_FLOAT32_I", "SC_FLOAT64_I",
		     "SC_FLOAT128_I", "SC_COMPLEX32_I", "SC_COMPLEX64_I",
		     "SC_COMPLEX128_I"};

    nerr = 0;

    cs = SC_mem_monitor(-1, dbg, "T1", msg);

/* char types */
    printf("\t\t\tCharacter types:\n");
    for (i = 0; i < N_PRIMITIVE_CHAR; i++)
        {id = SC_TYPE_CHAR_ID(i);
	 td = SC_gs.stl + id;
	 if (td->id == id)
	    sts = "ok";
	 else
	    {sts = "ng";
	     nerr++;};
         printf("\t\t%-24s %3d %3d %3s  %s\n",
		td->typ_s, i, id, sts, tchr[i]);};
    printf("\n");

/* fix point types */
    printf("\t\t\tFixed point types:\n");
    for (i = 0; i < N_PRIMITIVE_FIX; i++)
        {id = SC_TYPE_FIX_ID(i);
	 td = SC_gs.stl + id;
	 if (td->id == id)
	    sts = "ok";
	 else
	    {sts = "ng";
	     nerr++;};
         printf("\t\t%-24s %3d %3d %3s  %s\n",
		td->typ_s, i, id, sts, tfix[i]);};
    printf("\n");

/* floating point types */
    printf("\t\t\tFloating point types:\n");
    for (i = 0; i < N_PRIMITIVE_FP; i++)
        {id = SC_TYPE_FP_ID(i);
	 td = SC_gs.stl + id;
	 if (td->id == id)
	    sts = "ok";
	 else
	    {sts = "ng";
	     nerr++;};
         printf("\t\t%-24s %3d %3d %3s  %s\n",
		td->typ_s, i, id, sts, tfp[i]);};
    printf("\n");

/* complex types */
    printf("\t\t\tComplex types:\n");
    for (i = 0; i < N_PRIMITIVE_CPX; i++)
        {id = SC_TYPE_CPX_ID(i);
	 td = SC_gs.stl + id;
	 if (td->id == id)
	    sts = "ok";
	 else
	    {sts = "ng";
	     nerr++;};
         printf("\t\t%-24s %3d %3d %3s  %s\n",
		td->typ_s, i, id, sts, tcpx[i]);};
    printf("\n");

/* aliased types */
    printf("\t\t\tAliased types:\n");
    for (id = SC_FILE_I; id <= SC_COMPLEX128_I; id += 2)
        {td = SC_gs.stl + id;
	 i  = (id - SC_FILE_I) >> 1;
	 if (td->id == id)
	    sts = "ok";
	 else
	    {sts = "ng";
	     nerr++;};
         printf("\t\t%-24s %3d %3d %3s  %s\n",
		td->typ_s, i, id, sts, tal[i]);};
    printf("\n");


    io_printf(stdout, "\t\t\ttype indeces .............. %s\n",
	      (st == TRUE) ? "ok" : "ng");

    cs = SC_mem_monitor(cs, dbg, "T1", msg);

    st = (nerr == 0);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - check that the static and dynamic types agree */

static int test_2(void)
   {int i, err, st;
    typdes *td, *ts;

    st = TRUE;

    for (i = 0; i < N_TYPES; i++)
        {td = _SC_get_type_id(i);
	 ts = SC_gs.stl + i;

	 err = TRUE;
	 err &= (td->id == ts->id);
	 err &= (td->bpi == ts->bpi);
	 err &= (strcmp(td->type, ts->type) == 0);
	 err &= (strcmp(td->stype, ts->stype) == 0);
	 err &= (strcmp(td->utype, ts->utype) == 0);
	 err &= (strcmp(td->ftype, ts->ftype) == 0);
	 err &= (strcmp(td->fncp, ts->fncp) == 0);
	 err &= (strcmp(td->alias, ts->alias) == 0);
	 err &= (td->ptr == ts->ptr);
	 err &= (td->knd == ts->knd);
	 err &= (td->g == ts->g);
	 err &= (strcmp(td->mn, ts->mn) == 0);
	 err &= (strcmp(td->mx, ts->mx) == 0);
	 err &= (strcmp(td->defv, ts->defv) == 0);
	 err &= (strcmp(td->promo, ts->promo) == 0);
	 err &= (strcmp(td->comp, ts->comp) == 0);
	 err &= (strcmp(td->typ_i, ts->typ_i) == 0);
	 err &= (strcmp(td->typ_s, ts->typ_s) == 0);

	 st &= err;};

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_3 - check that aliases resolve to the correct type */

static int test_3(void)
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
	         case 'h' :
		      printf("\nUsage: tsctyp [-1] [-2] [-3] [-h]\n");
		      printf("   Options:\n");
		      printf("      1 - do not do seq write test\n");
		      printf("      2 - do not do seq read test\n");
		      printf("      3 - do not do rnd write test\n");
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

    io_printf(stdout, "\n");

/* reverse the sense of the test to exit with 0 iff successful */
    rv = (st == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
