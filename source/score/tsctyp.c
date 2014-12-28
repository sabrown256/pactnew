/*
 * TSCTYP.C - test type system
 *
 */

#include "cpyright.h"

#include "score.h"

#define STR_EQUAL(_a, _b)                                                    \
    (((_a != NULL) && (_b != NULL) && (strcmp(_a, _b) == 0)) ||              \
     ((_a == NULL) && (_b == NULL)))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - check that the kinds of standard types are correct */

static int test_1(void)
   {int i, id, st, cs, nerr;
    char msg[MAXLINE];
    char *sts;
    type_desc *td;
    static int dbg = 0;
    char *tchr[] = { "G_CHAR_I", "G_WCHAR_I" };
    char *tfix[] = { "G_INT8_I", "G_SHORT_I",
		     "G_INT_I", "G_LONG_I", "G_LONG_LONG_I" };
    char *tfp[]  = { "G_FLOAT_I", "G_DOUBLE_I", "G_LONG_DOUBLE_I" };
    char *tcpx[] = { "G_FLOAT_COMPLEX_I", "G_DOUBLE_COMPLEX_I",
		     "G_LONG_DOUBLE_COMPLEX_I" };
    char *tal[]  = { "G_FILE_I", "G_TYPE_GROUP_I", "G_TYPE_KIND_I", 
		     "G_TYPE_DEG_I", "G_ENUM_I", "G_PBOOLEAN_I",
		     "G_SSIZE_I", "G_INTEGER_I", "G_INT16_I",
		     "G_INT32_I", "G_INT64_I",
		     "G_REAL_I", "G_FLOAT32_I", "G_FLOAT64_I",
		     "G_FLOAT128_I", "G_COMPLEX32_I", "G_COMPLEX64_I",
		     "G_COMPLEX128_I"};

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
    for (id = G_FILE_I; id <= G_COMPLEX128_I; id += 2)
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

    st = (nerr == 0);

    io_printf(stdout, "\t\t\ttype indeces .............. %s\n",
	      (st == TRUE) ? "ok" : "ng");

    cs = SC_mem_monitor(cs, dbg, "T1", msg);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - check that the static and dynamic types agree */

static int test_2(void)
   {int i, err, st;
    type_desc *td, *ts;

    st = TRUE;

    for (i = 0; i < N_TYPES; i++)
        {td = _SC_get_type_id(i);
	 ts = SC_gs.stl + i;

	 err = TRUE;
	 err &= (td->id == ts->id);
	 err &= (td->bpi == ts->bpi);
	 err &= STR_EQUAL(td->type, ts->type);
	 err &= STR_EQUAL(td->stype, ts->stype);
	 err &= STR_EQUAL(td->utype, ts->utype);
	 err &= STR_EQUAL(td->ftype, ts->ftype);
	 err &= STR_EQUAL(td->fncp, ts->fncp);
	 err &= STR_EQUAL(td->alias, ts->alias);
	 err &= (td->ptr == ts->ptr);
	 err &= (td->knd == ts->knd);
	 err &= (td->g == ts->g);
	 err &= STR_EQUAL(td->mn, ts->mn);
	 err &= STR_EQUAL(td->mx, ts->mx);
	 err &= STR_EQUAL(td->defv, ts->defv);
	 err &= STR_EQUAL(td->promo, ts->promo);
	 err &= STR_EQUAL(td->comp, ts->comp);
	 err &= STR_EQUAL(td->typ_i, ts->typ_i);
	 err &= STR_EQUAL(td->typ_s, ts->typ_s);

	 st &= err;};

    io_printf(stdout, "\t\t\tstatic/dynamic check ...... %s\n",
	      (st == TRUE) ? "ok" : "ng");

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_3 - check that aliases resolve to the correct type */

static int test_3(void)
   {int i, err, st;
    type_desc *td, *ta;

    err = 0;

    for (i = 0; i < N_TYPES; i++)
        {td = _SC_get_type_id(i);
         if (td->alias != NULL)
	    {ta = SC_find_primitive(i);
             err += (ta == NULL);
             io_printf(stdout, "\t\t%3d %-20s -> %3d %-20s\n",
		       td->id, td->type, ta->id, ta->type);};};

    st = (err == 0);

    io_printf(stdout, "\t\t\talias check ............... %s\n",
	      (st == TRUE) ? "ok" : "ng");

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
