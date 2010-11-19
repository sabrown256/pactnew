/*
 * CONVERT.C - generate primitive type converters
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "scstd.h"
#include "scope_typeh.h"

#define I_NON          3
#define I_FIX          7
#define I_FLOAT       10
#define I_COMPLEX     13
#define I_QUATERNION  14
#define I_POINTER     15
#define N_PRIMITIVES  16

#define Separator  fprintf(fp, "/*--------------------------------------------------------------------------*/\n\n")

static char
 *names[] = { NULL, NULL, "bool", "char",
	      "shrt", "int", "lng", "ll",
	      "flt", "dbl", "ldbl",
	      "fcx", "dcx", "ldcx",
	      "qut", "ptr" },
 *types[] = { NULL, NULL, "bool", "char",
	      "short", "int", "long", "long long",
	      "float", "double", "long double",
	      "float complex", "double complex",
	      "long double complex",
	      "quaternion", "void *" },
 *promo[] = { NULL, NULL, "int", "int",
	      "int", "int", "long", "long long",
	      "double", "double", "long double",
	      "float complex", "double complex",
	      "long double complex",
	      "quaternion", "void *" },
 *mn[]    = { NULL, NULL, "BOOL_MIN", "CHAR_MIN",
	      "SHRT_MIN", "INT_MIN", "LONG_MIN", "LLONG_MIN",
	      "FLT_MIN", "DBL_MIN", "LDBL_MIN",
	      "FLT_MIN", "DBL_MIN", "LDBL_MIN",
	      "DBL_MIN", "LLONG_MIN" },
 *mx[]    = { NULL, NULL, "BOOL_MAX", "CHAR_MAX",
	      "SHRT_MAX", "INT_MAX", "LONG_MAX", "LLONG_MAX",
	      "FLT_MAX", "DBL_MAX", "LDBL_MAX",
	      "FLT_MAX", "DBL_MAX", "LDBL_MAX",
	      "DBL_MAX", "LLONG_MAX" };

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_HEADER - write the beginning of the file */

static void write_header(FILE *fp)
   {

    fprintf(fp, "/*\n");
    fprintf(fp, " * SCTYPEG.H - generated type handling routines\n");
    fprintf(fp, " *\n");
    fprintf(fp, " * Source Version: 3.0\n");
    fprintf(fp, " * Software Release #: LLNL-CODE-422942\n");
    fprintf(fp, " *\n");
    fprintf(fp, " */\n");
    fprintf(fp, " \n");

    fprintf(fp, "#include <cpyright.h>\n");
    fprintf(fp, "#include <score.h>\n");
    fprintf(fp, " \n");

    fprintf(fp, "#define N_PRIMITIVES  %d\n", N_PRIMITIVES);
    fprintf(fp, " \n");

    Separator;
    fprintf(fp, "/*                           TYPE CONVERSION                                */\n\n");
    Separator;

    fprintf(fp, "#undef CONVERT\n");
    fprintf(fp, " \n");

    fprintf(fp, "#define BOOL_MIN    0\n");
    fprintf(fp, "#define BOOL_MAX    1\n");
    fprintf(fp, " \n");

    fprintf(fp, "#define DIRECT(_dtyp, _d, _styp, _s, _dmn, _dmx)           \\\n");
    fprintf(fp, "long _SC_ ## _d ## _ ## _s(void *d, void *s, long n)       \\\n");
    fprintf(fp, "   {long i;                                                \\\n");
    fprintf(fp, "    _styp *ps = (_styp *) s;                               \\\n");
    fprintf(fp, "    _dtyp *pd = (_dtyp *) d;                               \\\n");
    fprintf(fp, "    for (i = 0; i < n; i++, *pd++ = *ps++);                \\\n");
    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "#ifdef SC_FAST_TRUNC\n");
    fprintf(fp, "\n");
    fprintf(fp, "#define CONVERT  DIRECT\n");
    fprintf(fp, "#define C_TO_C   DIRECT\n");
    fprintf(fp, "\n");
    fprintf(fp, "#else\n");
    fprintf(fp, "\n");
    fprintf(fp, "#define CONVERT(_dtyp, _d, _styp, _s, _dmn, _dmx)          \\\n");
    fprintf(fp, "long _SC_ ## _d ## _ ## _s(void *d, void *s, long n)       \\\n");
    fprintf(fp, "   {long i;                                                \\\n");
    fprintf(fp, "    _styp sm, smn, smx;                                    \\\n");
    fprintf(fp, "    _styp *ps = (_styp *) s;                               \\\n");
    fprintf(fp, "    _dtyp *pd = (_dtyp *) d;                               \\\n");
    fprintf(fp, "    if (sizeof(sm) < sizeof(_dmx))                         \\\n");
    fprintf(fp, "       {for (i = 0; i < n; i++, *pd++ = *ps++);}           \\\n");
    fprintf(fp, "    else                                                   \\\n");
    fprintf(fp, "       {smn = (_styp) _dmn;                                \\\n");
    fprintf(fp, "        smx = (_styp) _dmx;                                \\\n");
    fprintf(fp, "        for (i = 0; i < n; i++)                            \\\n");
    fprintf(fp, "            {sm = *ps++;                                   \\\n");
    fprintf(fp, "             sm = min(sm, smx);                            \\\n");
    fprintf(fp, "             sm = max(sm, smn);                            \\\n");
    fprintf(fp, "             *pd++ = sm;};};                               \\\n");
    fprintf(fp, "    return(i);}\n");
    fprintf(fp, "\n");

    fprintf(fp, "#define C_TO_C(_dtyp, _d, _styp, _s, _dmn, _dmx)           \\\n");
    fprintf(fp, "long _SC_ ## _d ## _ ## _s(void *d, void *s, long n)       \\\n");
    fprintf(fp, "   {long i;                                                \\\n");
    fprintf(fp, "    long double sr, si, smn, smx;                          \\\n");
    fprintf(fp, "    long double _Complex zs;                               \\\n");
    fprintf(fp, "    _styp *ps = (_styp *) s;                               \\\n");
    fprintf(fp, "    _dtyp *pd = (_dtyp *) d;                               \\\n");
    fprintf(fp, "    smn = (_styp) _dmn;                                    \\\n");
    fprintf(fp, "    smx = (_styp) _dmx;                                    \\\n");
    fprintf(fp, "    for (i = 0; i < n; i++)                                \\\n");
    fprintf(fp, "        {zs = *ps++;                                       \\\n");
    fprintf(fp, "         sr = creall(zs);                                  \\\n");
    fprintf(fp, "         si = cimagl(zs);                                  \\\n");
    fprintf(fp, "         sr = min(sr, smx);                                \\\n");
    fprintf(fp, "         sr = max(sr, smn);                                \\\n");
    fprintf(fp, "         si = min(si, smx);                                \\\n");
    fprintf(fp, "         si = max(si, smn);                                \\\n");
    fprintf(fp, "         *pd++ = sr + si*I;};                              \\\n");
    fprintf(fp, "    return(i);}\n");
    fprintf(fp, "\n");
    fprintf(fp, "#endif\n");
    fprintf(fp, "\n");

/* complex to real conversions */
    fprintf(fp, "#define C_TO_R(_dtyp, _d, _styp, _s, _dmn, _dmx)           \\\n");
    fprintf(fp, "long _SC_ ## _d ## _ ## _s(void *d, void *s, long n)       \\\n");
    fprintf(fp, "   {long i;                                                \\\n");
    fprintf(fp, "    _styp *ps = (_styp *) s;                               \\\n");
    fprintf(fp, "    _dtyp *pd = (_dtyp *) d;                               \\\n");
/* this crashes GCC 4.5.1 - internal compiler error
    fprintf(fp, "    for (i = 0; i < n; i++, *pd++ = *ps++);                \\\n");
*/
    fprintf(fp, "    for (i = 0; i < n; i++)                                \\\n");
    fprintf(fp, "        pd[i] = creall(ps[i]);                             \\\n");
    fprintf(fp, "    return(i);}\n");
    fprintf(fp, "\n");

/* quaternion to real conversions */
    fprintf(fp, "#define Q_TO_R(_dtyp, _d, _styp, _s, _dmn, _dmx)           \\\n");
    fprintf(fp, "long _SC_ ## _d ## _ ## _s(void *d, void *s, long n)       \\\n");
    fprintf(fp, "   {long i;                                                \\\n");
    fprintf(fp, "    _styp *ps = (_styp *) s;                               \\\n");
    fprintf(fp, "    _dtyp *pd = (_dtyp *) d;                               \\\n");
    fprintf(fp, "    for (i = 0; i < n; i++, *pd++ = (*ps++).s);            \\\n");
    fprintf(fp, "    return(i);}\n");
    fprintf(fp, "\n");

/* quaternion to complex conversions */
    fprintf(fp, "#define Q_TO_C(_dtyp, _d, _styp, _s, _dmn, _dmx)           \\\n");
    fprintf(fp, "long _SC_ ## _d ## _ ## _s(void *d, void *s, long n)       \\\n");
    fprintf(fp, "   {long i;                                                \\\n");
    fprintf(fp, "    _styp *ps = (_styp *) s;                               \\\n");
    fprintf(fp, "    _dtyp *pd = (_dtyp *) d;                               \\\n");
    fprintf(fp, "    _dtyp z;                                               \\\n");
    fprintf(fp, "    quaternion q;                                          \\\n");
    fprintf(fp, "    for (i = 0; i < n; i++);                               \\\n");
    fprintf(fp, "        {q     = *ps++;                                    \\\n");
    fprintf(fp, "         z     = q.s + q.i*I;                              \\\n");
    fprintf(fp, "         *pd++ = z;};                                      \\\n");
    fprintf(fp, "    return(i);}\n");
    fprintf(fp, "\n");

/* real to quaternion conversions */
    fprintf(fp, "#define R_TO_Q(_dtyp, _d, _styp, _s, _dmn, _dmx)           \\\n");
    fprintf(fp, "long _SC_ ## _d ## _ ## _s(void *d, void *s, long n)       \\\n");
    fprintf(fp, "   {long i;                                                \\\n");
    fprintf(fp, "    _styp *ps = (_styp *) s;                               \\\n");
    fprintf(fp, "    quaternion q = {0.0, 0.0, 0.0, 0.0};                   \\\n");
    fprintf(fp, "    quaternion *pd = d;                                    \\\n");
    fprintf(fp, "    for (i = 0; i < n; i++);                               \\\n");
    fprintf(fp, "        {q.s   = *ps++;                                    \\\n");
    fprintf(fp, "         *pd++ = q;};                                      \\\n");
    fprintf(fp, "    return(i);}\n");
    fprintf(fp, "\n");

/* complex to quaternion conversions */
    fprintf(fp, "#define C_TO_Q(_dtyp, _d, _styp, _s, _dmn, _dmx)           \\\n");
    fprintf(fp, "long _SC_ ## _d ## _ ## _s(void *d, void *s, long n)       \\\n");
    fprintf(fp, "   {long i;                                                \\\n");
    fprintf(fp, "    _styp *ps = (_styp *) s;                               \\\n");
    fprintf(fp, "    long double _Complex z;                                \\\n");
    fprintf(fp, "    quaternion q = {0.0, 0.0, 0.0, 0.0};                   \\\n");
    fprintf(fp, "    quaternion *pd = d;                                    \\\n");
    fprintf(fp, "    for (i = 0; i < n; i++);                               \\\n");
    fprintf(fp, "        {z     = *ps++;                                    \\\n");
    fprintf(fp, "         q.s   = creall(z);                                \\\n");
    fprintf(fp, "         q.i   = cimagl(z);                                \\\n");
    fprintf(fp, "         *pd++ = q;};                                      \\\n");
    fprintf(fp, "    return(i);}\n");
    fprintf(fp, "\n");

/* pointer to number conversions */
    fprintf(fp, "#define PTR_TO_N(_dtyp, _d, _styp, _s, _dmn, _dmx)         \\\n");
    fprintf(fp, "long _SC_ ## _d ## _ ## _s(void *d, void *s, long n)       \\\n");
    fprintf(fp, "   {long i;                                                \\\n");
    fprintf(fp, "    SC_address ad;                                         \\\n");
    fprintf(fp, "    _styp *ps = (_styp *) s;                               \\\n");
    fprintf(fp, "    _dtyp *pd = (_dtyp *) d;                               \\\n");
    fprintf(fp, "    for (i = 0; i < n; i++);                               \\\n");
    fprintf(fp, "        {ad.memaddr = *ps++;                               \\\n");
    fprintf(fp, "         *pd++      = ad.diskaddr;};                       \\\n");
    fprintf(fp, "    return(i);}\n");
    fprintf(fp, "\n");

/* pointer to quaternion conversions */
    fprintf(fp, "#define PTR_TO_Q(_dtyp, _d, _styp, _s, _dmn, _dmx)         \\\n");
    fprintf(fp, "long _SC_ ## _d ## _ ## _s(void *d, void *s, long n)       \\\n");
    fprintf(fp, "   {long i;                                                \\\n");
    fprintf(fp, "    SC_address ad;                                         \\\n");
    fprintf(fp, "    quaternion q = {0.0, 0.0, 0.0, 0.0};                   \\\n");
    fprintf(fp, "    quaternion *pd = d;                                    \\\n");
    fprintf(fp, "    _styp *ps = (_styp *) s;                               \\\n");
    fprintf(fp, "    for (i = 0; i < n; i++);                               \\\n");
    fprintf(fp, "        {ad.memaddr = *ps++;                               \\\n");
    fprintf(fp, "         q.s        = ad.diskaddr;                         \\\n");
    fprintf(fp, "         *pd++      = q;};                                 \\\n");
    fprintf(fp, "    return(i);}\n");
    fprintf(fp, "\n");

/* number to pointer conversions */
    fprintf(fp, "#define N_TO_PTR(_dtyp, _d, _styp, _s, _dmn, _dmx)         \\\n");
    fprintf(fp, "long _SC_ ## _d ## _ ## _s(void *d, void *s, long n)       \\\n");
    fprintf(fp, "   {long i;                                                \\\n");
    fprintf(fp, "    SC_address ad;                                         \\\n");
    fprintf(fp, "    _styp *ps = (_styp *) s;                               \\\n");
    fprintf(fp, "    _dtyp *pd = (_dtyp *) d;                               \\\n");
    fprintf(fp, "    for (i = 0; i < n; i++);                               \\\n");
    fprintf(fp, "        {ad.diskaddr = *ps++;                              \\\n");
    fprintf(fp, "         *pd++       = ad.memaddr;};                       \\\n");
    fprintf(fp, "    return(i);}\n");
    fprintf(fp, "\n");

/* quaternion to pointer conversions */
    fprintf(fp, "#define Q_TO_PTR(_dtyp, _d, _styp, _s, _dmn, _dmx)         \\\n");
    fprintf(fp, "long _SC_ ## _d ## _ ## _s(void *d, void *s, long n)       \\\n");
    fprintf(fp, "   {long i;                                                \\\n");
    fprintf(fp, "    quaternion q = {0.0, 0.0, 0.0, 0.0};                   \\\n");
    fprintf(fp, "    SC_address ad;                                         \\\n");
    fprintf(fp, "    _styp *ps = (_styp *) s;                               \\\n");
    fprintf(fp, "    _dtyp *pd = (_dtyp *) d;                               \\\n");
    fprintf(fp, "    for (i = 0; i < n; i++);                               \\\n");
    fprintf(fp, "        {q           = ps[i];                              \\\n");
    fprintf(fp, "         ad.diskaddr = q.s;                                \\\n");
    fprintf(fp, "         *pd++       = ad.memaddr;};                       \\\n");
    fprintf(fp, "    return(i);}\n");
    fprintf(fp, "\n");

    Separator;
    fprintf(fp, "/*                         VA ARG EXTRACTION                                */\n\n");
    Separator;

    fprintf(fp, "#define GETARG(_ntyp, _dtyp, _ptyp)                        \\\n");
    fprintf(fp, "int _SC_arg_ ## _ntyp(va_list __a__, void *d, long n)      \\\n");
    fprintf(fp, "   {_dtyp *pv = (_dtyp *) d;                               \\\n");
    fprintf(fp, "    pv[n] = SC_VA_ARG(_ptyp);                              \\\n");
    fprintf(fp, "    return(TRUE);}\n");
    fprintf(fp, "\n");

    Separator;
    fprintf(fp, "/*                          NUMBER RENDERING                                */\n\n");
    Separator;

    fprintf(fp, "#define NTOS(_ntyp, _styp, _i)                             \\\n");
    fprintf(fp, "int _SC_str_ ## _ntyp(char *t, int nc, void *s, long n)    \\\n");
    fprintf(fp, "   {int nb;                                                \\\n");
    fprintf(fp, "    _styp *pv = (_styp *) s;                               \\\n");
    fprintf(fp, "    nb = snprintf(t, nc, SC_print_format[_i], pv[n]);      \\\n");
    fprintf(fp, "    return(nb);}\n");
    fprintf(fp, "\n");

    Separator;
    fprintf(fp, "typedef long (*PFConv)(void *d, void *s, long n);\n");
    fprintf(fp, "typedef int (*PFArgv)(va_list a, void *d, long n);\n");
    fprintf(fp, "typedef int (*PFStrv)(char *t, int nc, void *s, long n);\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_CONVERTERS - write the conversion routines */

static void write_converters(FILE *fp)
   {int i, j;
    char *ti, *tj;

    for (i = 0; i < N_PRIMITIVES; i++)
        {ti = types[i];
	 for (j = 0; j < N_PRIMITIVES; j++)
	     {tj = types[j];
	      if ((ti != NULL) && (tj != NULL))

/* direct copy */
		 {if (i == j)
		     fprintf(fp, "DIRECT(%s, %s, %s, %s, %s, %s)\n\n",
			     ti, names[i], types[j], names[j],
			     mn[i], mx[i]);

/* complex to real conversions */
		  else if ((i <= I_FLOAT) && (I_FLOAT < j) && (j <= I_COMPLEX))
		     fprintf(fp, "C_TO_R(%s, %s, %s, %s, %s, %s)\n\n",
			     ti, names[i], types[j], names[j],
			     mn[i], mx[i]);

/* complex to complex conversions */
		  else if ((I_FLOAT < i) && (i <= I_COMPLEX) &&
			   (I_FLOAT < j) && (j <= I_COMPLEX))
		     fprintf(fp, "C_TO_C(%s, %s, %s, %s, %s, %s)\n\n",
			     ti, names[i], types[j], names[j],
			     mn[i], mx[i]);

/* quaternion to real conversions */
		  else if ((i <= I_FLOAT) && (j == I_QUATERNION))
		     fprintf(fp, "Q_TO_R(%s, %s, %s, %s, %s, %s)\n\n",
			     ti, names[i], types[j], names[j],
			     mn[i], mx[i]);

/* quaternion to complex conversions */
		  else if ((I_FLOAT < i) && (i <= I_COMPLEX) && (j == I_QUATERNION))
		     fprintf(fp, "Q_TO_C(%s, %s, %s, %s, %s, %s)\n\n",
			     ti, names[i], types[j], names[j],
			     mn[i], mx[i]);

/* quaternion to pointer conversions */
		  else if ((i == I_POINTER) && (j == I_QUATERNION))
		     fprintf(fp, "Q_TO_PTR(%s, %s, %s, %s, %s, %s)\n\n",
			     ti, names[i], types[j], names[j],
			     mn[i], mx[i]);

/* real to quaternion conversions */
		  else if ((i == I_QUATERNION) && (j <= I_FLOAT))
		     fprintf(fp, "R_TO_Q(%s, %s, %s, %s, %s, %s)\n\n",
			     ti, names[i], types[j], names[j],
			     mn[i], mx[i]);

/* complex to quaternion conversions */
		  else if ((i == I_QUATERNION) && (I_FLOAT < j) && (j <= I_COMPLEX))
		     fprintf(fp, "C_TO_Q(%s, %s, %s, %s, %s, %s)\n\n",
			     ti, names[i], types[j], names[j],
			     mn[i], mx[i]);

/* pointer to quaternion conversions */
		  else if ((i == I_QUATERNION) && (j == I_POINTER))
		     fprintf(fp, "PTR_TO_Q(%s, %s, %s, %s, %s, %s)\n\n",
			     ti, names[i], types[j], names[j],
			     mn[i], mx[i]);

		  else if (j == I_POINTER)
		     fprintf(fp, "PTR_TO_N(%s, %s, %s, %s, %s, %s)\n\n",
			     ti, names[i], types[j], names[j],
			     mn[i], mx[i]);

		  else if (i == I_POINTER)
		     fprintf(fp, "N_TO_PTR(%s, %s, %s, %s, %s, %s)\n\n",
			     ti, names[i], types[j], names[j],
			     mn[i], mx[i]);

		  else
		     fprintf(fp, "CONVERT(%s, %s, %s, %s, %s, %s)\n\n",
			     ti, names[i], types[j], names[j],
			     mn[i], mx[i]);};};};

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_CONV_DECL - write the declaration of the conversion array */

static void write_conv_decl(FILE *fp)
   {int i, j;

    fprintf(fp, "static PFConv\n");
    fprintf(fp, " _SC_convf[][%d] = {\n", N_PRIMITIVES);
    for (i = 0; i < N_PRIMITIVES; i++)
        {if (types[i] != NULL)
	    {for (j = 0; j < N_PRIMITIVES; j++)
	         {if (j == 0)
		     fprintf(fp, "      { ");
		  else
		     fprintf(fp, "        ");
	          if (j == N_PRIMITIVES-1)
		     {if ((names[i] != NULL) && (names[j] != NULL))
			 fprintf(fp, "_SC_%s_%s ", names[i], names[j]);
		      else
			 fprintf(fp, "NULL ");}
		  else
		     {if ((names[i] != NULL) && (names[j] != NULL))
			 fprintf(fp, "_SC_%s_%s,\n", names[i], names[j]);
		      else
			 fprintf(fp, "NULL,\n");};};}
	 else
	    {for (j = 0; j < N_PRIMITIVES; j++)
	         {if (j == 0)
		     fprintf(fp, "      { ");
		  else
		     fprintf(fp, "        ");
		  if (j == N_PRIMITIVES-1)
		     fprintf(fp, "NULL ");
		  else
		     fprintf(fp, "NULL,\n");};};

	 if (i == N_PRIMITIVES-1)
	    fprintf(fp, "}\n");
	 else
	    fprintf(fp, "},\n");};

    fprintf(fp, "};\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_ARGS - write the va arg routines */

static void write_args(FILE *fp)
   {int i, n;

/* PATHSCALE 2.5 does not support complex types in va_arg
 * GOTCHA: this is the wrong way to do this because we MUST
 * have all the types that are supported in the proper order
 */
#if defined(COMPILER_PATHSCALE)
    n = 10;
#else
    n = N_PRIMITIVES;
#endif

    for (i = 0; i < n; i++)
        {if (types[i] != NULL)
	    fprintf(fp, "GETARG(%s, %s, %s)\n\n",
		    names[i], types[i], promo[i]);};

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_ARG_DECL - write the declaration of the conversion array */

static void write_arg_decl(FILE *fp)
   {int i, n;

/* PATHSCALE 2.5 does not support complex types in va_arg */
#if defined(COMPILER_PATHSCALE)
    n = 10;
#else
    n = N_PRIMITIVES;
#endif

    fprintf(fp, "static PFArgv\n");
    fprintf(fp, " _SC_argf[] = {\n");
    for (i = 0; i < n; i++)
        {if (types[i] != NULL)
	    {if (i == n - 1)
	        fprintf(fp, "                _SC_arg_%s\n", names[i]);
	     else
	        fprintf(fp, "                _SC_arg_%s,\n", names[i]);}
	 else
	    {if (i == n - 1)
		fprintf(fp, "                NULL\n");
	     else
		fprintf(fp, "                NULL,\n");};};

    fprintf(fp, "};\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_STR - write the va arg routines */

static void write_str(FILE *fp)
   {int i;

    for (i = 0; i < N_PRIMITIVES; i++)
        {if (types[i] != NULL)
	    fprintf(fp, "NTOS(%s, %s, %d)\n\n",
		    names[i], types[i], i);};

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_STR_DECL - write the declaration of the conversion array */

static void write_str_decl(FILE *fp)
   {int i;

    fprintf(fp, "static PFStrv\n");
    fprintf(fp, " _SC_strf[] = {\n");
    for (i = 0; i < N_PRIMITIVES; i++)
        {if (types[i] != NULL)
	    {if (i == N_PRIMITIVES-1)
	        fprintf(fp, "                _SC_str_%s\n", names[i]);
	     else
	        fprintf(fp, "                _SC_str_%s,\n", names[i]);}
	 else
	    {if (i == N_PRIMITIVES-1)
	        fprintf(fp, "                NULL\n");
	     else
	        fprintf(fp, "                NULL,\n");};};

    fprintf(fp, "};\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int rv;
    char *name;
    FILE *fp;

    rv = 0;

/*    name = "scconv.c"; */
    name = NULL;

    if (name != NULL)
       fp = fopen(name, "w");
    else
       fp = stdout;

    write_header(fp);

/* emit type conversion code */
    Separator;

    write_converters(fp);

    write_conv_decl(fp);

/* emit var arg code */
    Separator;

    write_args(fp);

    write_arg_decl(fp);

/* emit number to string code */
    Separator;

    write_str(fp);

    write_str_decl(fp);

    if (name != NULL)
       fclose(fp);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
