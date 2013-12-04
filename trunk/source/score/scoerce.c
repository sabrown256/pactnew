/*
 * SCOERCE.C - generate primitive type converters
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#define NO_VA_ARG_ID

#undef LIBFIO
#undef SCOPE_SCORE_PREPROC
#include "../psh/common.h"
#include "../psh/libfio.c"

#include "scstd.h"
#include "scope_typeh.h"

#define I_BOOL        2
#define I_CHAR        (I_BOOL+N_PRIMITIVE_CHAR)
#define I_FIX         (I_CHAR+N_PRIMITIVE_FIX)   /* last fixed point type */
#define I_FLOAT       (I_FIX+N_PRIMITIVE_FP)     /* last floating point type */
#define I_COMPLEX     (I_FLOAT+N_PRIMITIVE_CPX)  /* last complex floating point type */
#define I_QUATERNION  (I_COMPLEX+1)
#define I_POINTER     (I_QUATERNION+3)

#undef IS_CHAR
#undef IS_FIX
#undef IS_FLOAT
#undef IS_REAL
#undef IS_COMPLEX
#undef IS_QUATERNION
#undef IS_POINTER
#define IS_CHAR(_i)         ((I_BOOL < _i) && (_i <= I_CHAR))
#define IS_FIX(_i)          ((I_CHAR < _i) && (_i <= I_FIX))
#define IS_FLOAT(_i)        ((I_FIX < _i) && (_i <= I_FLOAT))
#define IS_REAL(_i)         (_i <= I_FLOAT)
#define IS_COMPLEX(_i)      ((I_FLOAT < _i) && (_i <= I_COMPLEX))
#define IS_QUATERNION(_i)   ((I_COMPLEX < _i) && (_i <= I_QUATERNION))
#define IS_POINTER(_i)      ((I_QUATERNION < _i) && (_i <= I_POINTER))

#ifdef SC_FAST_TRUNC
#define CONVERT  _SC_write_n_to_n_fast
#define C_TO_C   _SC_write_n_to_n_fast
#else
#define CONVERT  _SC_write_n_to_n_safe
#define C_TO_C   _SC_write_c_to_c_safe
#endif

#define Separator(_f)  fprintf(_f, "/*--------------------------------------------------------------------------*/\n\n")

static size_t
 sizes[] = { 0, 0, sizeof(bool),
	     sizeof(char), sizeof(wchar_t),
	     sizeof(int8_t), sizeof(short), sizeof(int),
             sizeof(long), sizeof(long long),
	     sizeof(float), sizeof(double), sizeof(long double),
	     sizeof(float _Complex), sizeof(double _Complex),
	     sizeof(long double _Complex),
	     sizeof(quaternion), sizeof(void *), 0, sizeof(char *) };

static char
 *names[] = { NULL, NULL, "bool",
	      "char", "wchar",
	      "int8", "shrt", "int", "lng", "ll",
	      "flt", "dbl", "ldbl",
	      "fcx", "dcx", "ldcx",
	      "qut",
              "ptr", NULL, "str" },
 *types[] = { NULL, NULL, "bool",
	      "char", "wchar_t",
	      "int8_t", "short", "int", "long", "long long",
	      "float", "double", "long double",
	      "float _Complex", "double _Complex",
	      "long double _Complex",
	      "quaternion",
              "void *", NULL, "char *" },
 *mn[]    = { NULL, NULL, "BOOL_MIN",
	      "CHAR_MIN", "WCHAR_MIN",
	      "INT8_MIN", "SHRT_MIN", "INT_MIN", "LONG_MIN", "LLONG_MIN",
	      "-FLT_MAX", "-DBL_MAX", "-LDBL_MAX",
	      "-FLT_MAX", "-DBL_MAX", "-LDBL_MAX",
	      "-DBL_MAX", "-LLONG_MAX" },
 *mx[]    = { NULL, NULL, "BOOL_MAX",
	      "CHAR_MAX", "WCHAR_MAX",
	      "INT8_MAX", "SHRT_MAX", "INT_MAX", "LONG_MAX", "LLONG_MAX",
	      "FLT_MAX", "DBL_MAX", "LDBL_MAX",
	      "FLT_MAX", "DBL_MAX", "LDBL_MAX",
	      "DBL_MAX", "LLONG_MAX" };

#define STRINGIFY(_x)    #_x

static char
 *realp[] = { "crealf", "creal", STRINGIFY(CREALL) },
 *imagp[] = { "cimagf", "cimag", STRINGIFY(CIMAGL) };

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

    fprintf(fp, "#undef CONVERT\n");
    fprintf(fp, " \n");

    fprintf(fp, "#define BOOL_MIN    0\n");
    fprintf(fp, "#define BOOL_MAX    1\n");
    fprintf(fp, " \n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_N_TO_N_FAST - number to number conversions */

static void _SC_write_n_to_n_fast(FILE *fp, int did, int sid)
   {

    fprintf(fp, "long _SC_%s_%s(void *d, long od, long ds, void *s, long os, long ss, long n)\n",
	    names[did], names[sid]);

    fprintf(fp, "   {long i;\n");
    fprintf(fp, "    %s *ps = (%s *) s;\n", types[sid], types[sid]);
    fprintf(fp, "    %s *pd = (%s *) d;\n", types[did], types[did]);

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

    fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
    fprintf(fp, "        *pd = *ps;\n");

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, " \n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_N_TO_N_SAFE - number to number conversions */

static void _SC_write_n_to_n_safe(FILE *fp, int did, int sid)
   {int ok;

    ok = ((sizes[did] < sizes[sid]) && (did <= I_FIX));

    fprintf(fp, "long _SC_%s_%s(void *d, long od, long ds, void *s, long os, long ss, long n)\n",
	    names[did], names[sid]);

    fprintf(fp, "   {long i;\n");
    if (ok == TRUE)
       fprintf(fp, "    %s sm, smn, smx;\n", types[sid]);
    fprintf(fp, "    %s *ps = (%s *) s;\n", types[sid], types[sid]);
    fprintf(fp, "    %s *pd = (%s *) d;\n", types[did], types[did]);

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

    if (ok == TRUE)
       {fprintf(fp, "    smn = (%s) %s;\n", types[sid], mn[did]);
	fprintf(fp, "    smx = (%s) %s;\n", types[sid], mx[did]);
	fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
	fprintf(fp, "        {sm  = *ps;\n");
	fprintf(fp, "         sm  = min(sm, smx);\n");
	fprintf(fp, "         sm  = max(sm, smn);\n");
	fprintf(fp, "         *pd = sm;};\n");}
    else
       {fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
	fprintf(fp, "        *pd = *ps;\n");};

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_C_TO_C_SAFE - complex to complex conversions */

static void _SC_write_c_to_c_safe(FILE *fp, int did, int sid)
   {int eid;

    eid = 2;

    fprintf(fp, "long _SC_%s_%s(void *d, long od, long ds, void *s, long os, long ss, long n)\n",
	    names[did], names[sid]);

    fprintf(fp, "   {long i;\n");
    fprintf(fp, "    %s *ps = (%s *) s;\n", types[sid], types[sid]);
    fprintf(fp, "    %s *pd = (%s *) d;\n", types[did], types[did]);

    if (did < sid)
       {fprintf(fp, "    long double sr, si, smn, smx;\n");
	fprintf(fp, "    long double _Complex zs;\n");
	fprintf(fp, "    smn = %s;\n", mn[did]);
	fprintf(fp, "    smx = %s;\n", mx[did]);};

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

    if (did < sid)
       {fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
	fprintf(fp, "        {zs = *ps;\n");
	fprintf(fp, "         sr = %s(zs);\n", realp[eid]);
	fprintf(fp, "         si = %s(zs);\n", imagp[eid]);
	fprintf(fp, "         sr = min(sr, smx);\n");
	fprintf(fp, "         sr = max(sr, smn);\n");
	fprintf(fp, "         si = min(si, smx);\n");
	fprintf(fp, "         si = max(si, smn);\n");
	fprintf(fp, "         *pd = sr + si*I;};\n");}
    else
       {fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
	fprintf(fp, "        *pd = *ps;\n");};

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_C_TO_R - complex to real conversions */

static void _SC_write_c_to_r(FILE *fp, int did, int sid)
   {int eid;

    eid = 2;

    fprintf(fp, "long _SC_%s_%s(void *d, long od, long ds, void *s, long os, long ss, long n)\n",
	    names[did], names[sid]);

    fprintf(fp, "   {long i;\n");
    fprintf(fp, "    %s *ps = (%s *) s;\n", types[sid], types[sid]);
    fprintf(fp, "    %s *pd = (%s *) d;\n", types[did], types[did]);

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

/* this crashes GCC 4.5.1 - internal compiler error
    fprintf(fp, "    for (i = 0; i < n; i++, *pd += ds = *ps++);\n");
*/
    fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
    fprintf(fp, "        *pd = %s(*ps);\n", realp[eid]);

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_Q_TO_R - quaternion to real conversions */

static void _SC_write_q_to_r(FILE *fp, int did, int sid)
   {

    fprintf(fp, "long _SC_%s_%s(void *d, long od, long ds, void *s, long os, long ss, long n)\n",
	    names[did], names[sid]);

    fprintf(fp, "   {long i;\n");
    fprintf(fp, "    quaternion *ps = (quaternion *) s;\n");
    fprintf(fp, "    %s *pd = (%s *) d;\n", types[did], types[did]);

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

    fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
    fprintf(fp, "        *pd = ps->s;\n");

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_Q_TO_C - quaternion to complex conversions */

static void _SC_write_q_to_c(FILE *fp, int did, int sid)
   {

    fprintf(fp, "long _SC_%s_%s(void *d, long od, long ds, void *s, long os, long ss, long n)\n",
	    names[did], names[sid]);

    fprintf(fp, "   {long i;\n");
    fprintf(fp, "    quaternion *ps = (quaternion *) s;\n");
    fprintf(fp, "    %s *pd = (%s *) d;\n", types[did], types[did]);
    fprintf(fp, "    %s z;\n", types[did]);
    fprintf(fp, "    quaternion q;\n");

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

    fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
    fprintf(fp, "        {q   = *ps;\n");
    fprintf(fp, "         z   = q.s + q.i*I;\n");
    fprintf(fp, "         *pd = z;};\n");

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_R_TO_Q - real to quaternion conversions */

static void _SC_write_r_to_q(FILE *fp, int did, int sid)
   {

    fprintf(fp, "long _SC_%s_%s(void *d, long od, long ds, void *s, long os, long ss, long n)\n",
	    names[did], names[sid]);

    fprintf(fp, "   {long i;\n");
    fprintf(fp, "    %s *ps = (%s *) s;\n", types[sid], types[sid]);
    fprintf(fp, "    quaternion q = {0.0, 0.0, 0.0, 0.0};\n");
    fprintf(fp, "    quaternion *pd = d;\n");

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

    fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
    fprintf(fp, "        {q.s = *ps;\n");
    fprintf(fp, "         *pd = q;};\n");

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_C_TO_Q - complex to quaternion conversions */

static void _SC_write_c_to_q(FILE *fp, int did, int sid)
   {int eid;

    eid = 2;

    fprintf(fp, "long _SC_%s_%s(void *d, long od, long ds, void *s, long os, long ss, long n)\n",
	    names[did], names[sid]);

    fprintf(fp, "   {long i;\n");
    fprintf(fp, "    %s *ps = (%s *) s;\n", types[sid], types[sid]);
    fprintf(fp, "    long double _Complex z;\n");
    fprintf(fp, "    quaternion q = {0.0, 0.0, 0.0, 0.0};\n");
    fprintf(fp, "    quaternion *pd = d;\n");

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

    fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
    fprintf(fp, "        {z   = *ps;\n");
    fprintf(fp, "         q.s = %s(z);\n", realp[eid]);
    fprintf(fp, "         q.i = %s(z);\n", imagp[eid]);
    fprintf(fp, "         *pd = q;};\n");

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_P_TO_N - pointer to number conversions */

static void _SC_write_p_to_n(FILE *fp, int did, int sid)
   {

    fprintf(fp, "long _SC_%s_%s(void *d, long od, long ds, void *s, long os, long ss, long n)\n",
	    names[did], names[sid]);

    fprintf(fp, "   {long i;\n");
    fprintf(fp, "    SC_address ad;\n");
    fprintf(fp, "    void **ps = (void **) s;\n");
    fprintf(fp, "    %s *pd = (%s *) d;\n", types[did], types[did]);

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

    fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
    fprintf(fp, "        {ad.memaddr = *ps;\n");
    fprintf(fp, "         *pd        = ad.diskaddr;};\n");

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_P_TO_Q - pointer to quaternion conversions */

static void _SC_write_p_to_q(FILE *fp, int did, int sid)
   {

    fprintf(fp, "long _SC_%s_%s(void *d, long od, long ds, void *s, long os, long ss, long n)\n",
	    names[did], names[sid]);

    fprintf(fp, "   {long i;\n");
    fprintf(fp, "    SC_address ad;\n");
    fprintf(fp, "    quaternion q = {0.0, 0.0, 0.0, 0.0};\n");
    fprintf(fp, "    quaternion *pd = d;\n");
    fprintf(fp, "    %s *ps = (%s *) s;\n", types[sid], types[sid]);

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

    fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
    fprintf(fp, "        {ad.memaddr = *ps;\n");
    fprintf(fp, "         q.s        = ad.diskaddr;\n");
    fprintf(fp, "         *pd        = q;};\n");

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_N_TO_P - number to pointer conversions */

static void _SC_write_n_to_p(FILE *fp, int did, int sid)
   {

    fprintf(fp, "long _SC_%s_%s(void *d, long od, long ds, void *s, long os, long ss, long n)\n",
	    names[did], names[sid]);

    fprintf(fp, "   {long i;\n");
    fprintf(fp, "    SC_address ad;\n");
    fprintf(fp, "    %s *ps = (%s *) s;\n", types[sid], types[sid]);
    fprintf(fp, "    void **pd = (void **) d;\n");

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

    fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
    fprintf(fp, "        {ad.diskaddr = *ps;\n");
    fprintf(fp, "         *pd         = ad.memaddr;};\n");

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_Q_TO_P - quaternion to pointer conversions */

static void _SC_write_q_to_p(FILE *fp, int did, int sid)
   {

    fprintf(fp, "long _SC_%s_%s(void *d, long od, long ds, void *s, long os, long ss, long n)\n",
	    names[did], names[sid]);

    fprintf(fp, "   {long i;\n");
    fprintf(fp, "    SC_address ad;\n");
    fprintf(fp, "    quaternion q = {0.0, 0.0, 0.0, 0.0};\n");
    fprintf(fp, "    quaternion *ps = (quaternion *) s;\n");
    fprintf(fp, "    void **pd = (void **) d;\n");

    fprintf(fp, "    ps += os;\n");
    fprintf(fp, "    pd += od;\n");

    fprintf(fp, "    for (i = 0; i < n; i++, pd += ds, ps += ss)\n");
    fprintf(fp, "        {q           = *ps;\n");
    fprintf(fp, "         ad.diskaddr = q.s;\n");
    fprintf(fp, "         *pd         = ad.memaddr;};\n");

    fprintf(fp, "    return(i);}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_CONV_DECL - write the declaration of the conversion array */

static void write_conv_decl(FILE *fp)
   {int i, j;

    fprintf(fp, "typedef long (*PFConv)(void *d, long od, long ds, void *s, long os, long ss, long n);\n");
    fprintf(fp, "\n");

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

/* WRITE_CONVERTERS - write the conversion routines */

static void write_converters(FILE *fp)
   {int i, j;
    char *ti, *tj;

    Separator(fp);
    fprintf(fp, "/*                           TYPE CONVERSION                                */\n\n");
    Separator(fp);

    for (i = 0; i < N_PRIMITIVES; i++)
        {ti = types[i];
	 for (j = 0; j < N_PRIMITIVES; j++)
	     {tj = types[j];
	      if ((ti != NULL) && (tj != NULL))

/* direct copy */
		 {if (i == j)
		     _SC_write_n_to_n_fast(fp, i, j);

/* complex to real conversions */
		  else if (IS_REAL(i) && IS_COMPLEX(j))
		     _SC_write_c_to_r(fp, i, j);

/* complex to complex conversions */
		  else if (IS_COMPLEX(i) && IS_COMPLEX(j))
		     C_TO_C(fp, i, j);

/* quaternion to real conversions */
		  else if (IS_REAL(i) && IS_QUATERNION(j))
		     _SC_write_q_to_r(fp, i, j);

/* quaternion to complex conversions */
		  else if (IS_COMPLEX(i) && IS_QUATERNION(j))
		     _SC_write_q_to_c(fp, i, j);

/* quaternion to pointer conversions */
		  else if (IS_POINTER(i) && IS_QUATERNION(j))
		     _SC_write_q_to_p(fp, i, j);

/* real to quaternion conversions */
		  else if (IS_QUATERNION(i) && IS_REAL(j))
		     _SC_write_r_to_q(fp, i, j);

/* complex to quaternion conversions */
		  else if (IS_QUATERNION(i) && IS_COMPLEX(j))
		     _SC_write_c_to_q(fp, i, j);

/* pointer to quaternion conversions */
		  else if (IS_QUATERNION(i) && IS_POINTER(j))
		     _SC_write_p_to_q(fp, i, j);

		  else if (IS_POINTER(j))
		     _SC_write_p_to_n(fp, i, j);

		  else if (IS_POINTER(i))
		     _SC_write_n_to_p(fp, i, j);

		  else
		     CONVERT(fp, i, j);};};};

    fprintf(fp, "\n");

    write_conv_decl(fp);

    return;}

/*--------------------------------------------------------------------------*/

/*                               NUMBER RENDERING                           */

/*--------------------------------------------------------------------------*/

/* _SC_WRITE_BOOL - write the rendering function for type SC_BOOL_I */

static void _SC_write_bool(FILE *fp, int id)
   {

    fprintf(fp, "char *_SC_str_%s(char *t, int nc, void *s, long n, int mode)\n",
	    names[id]);

    fprintf(fp, "   {int nb;\n");
    fprintf(fp, "    char *fmt;\n");
    fprintf(fp, "    %s *pv = (%s *) s;\n", types[id], types[id]);

    fprintf(fp, "    fmt = (mode == 1) ? _SC.types.formats[%d] : _SC.types.formata[%d];\n",
	    id, id);

    fprintf(fp, "    nb = -1;\n");
    fprintf(fp, "    if (strchr(fmt, 's') != NULL)\n");
    fprintf(fp, "       nb = snprintf(t, nc, fmt, pv[n] ? \"T\" : \"F\");\n");
    fprintf(fp, "    else if (strchr(fmt, 'd') != NULL)\n");
    fprintf(fp, "       nb = snprintf(t, nc, fmt, (int) pv[n]);\n");

    fprintf(fp, "    if (nb < 0)\n");
    fprintf(fp, "       t = NULL;\n");
    fprintf(fp, "    return(t);}\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_COMPLEX - write the rendering function for complex type ID */

static void _SC_write_complex(FILE *fp, int id)
   {int eid;
    static char *kind[]  = { "float", "double", "long double" };

    eid = id - I_FLOAT - 1;

    fprintf(fp, "char *_SC_str_%s(char *t, int nc, void *s, long n, int mode)\n",
	    names[id]);
    fprintf(fp, "   {int nb;\n");
    fprintf(fp, "    char *fmt;\n");
    fprintf(fp, "    %s _Complex z;\n", kind[eid]);
    fprintf(fp, "    %s *pv = (%s *) s;\n", types[id], types[id]);

    fprintf(fp, "    fmt = (mode == 1) ? _SC.types.formats[%d] : _SC.types.formata[%d];\n",
	    id, id);

#if (AF_LONG_DOUBLE_IO == 1)
    fprintf(fp, "\n");
    fprintf(fp, "    {char *p;\n");
    fprintf(fp, "     for (p = fmt; *p != \'\\0\'; p++)\n");
    fprintf(fp, "         if (*p == \'L\')\n");
    fprintf(fp, "            *p++ = \'l\';};\n");
    fprintf(fp, "\n");

    eid = min(eid, 1);
#endif

    fprintf(fp, "    {%s pr, pi;\n", kind[eid]);
    fprintf(fp, "     z  = pv[n];\n");
    fprintf(fp, "     pr = %s(z);\n", realp[eid]);
    fprintf(fp, "     pi = %s(z);\n", imagp[eid]);
    fprintf(fp, "     nb = snprintf(t, nc, fmt, pr, pi);};\n");

    fprintf(fp, "    if (nb < 0)\n");
    fprintf(fp, "       t = NULL;\n");

    fprintf(fp, "    return(t);}\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_QUATERNION - write the rendering function
 *                      - for quaternion type ID
 */

static void _SC_write_quaternion(FILE *fp, int id)
   {

    fprintf(fp, "char *_SC_str_%s(char *t, int nc, void *s, long n, int mode)\n",
	    names[id]);
    fprintf(fp, "   {int nb;\n");
    fprintf(fp, "    char *fmt;\n");
    fprintf(fp, "    quaternion q;\n");
    fprintf(fp, "    %s *pv = (%s *) s;\n", types[id], types[id]);

    fprintf(fp, "    fmt = (mode == 1) ? _SC.types.formats[%d] : _SC.types.formata[%d];\n",
	    id, id);

    fprintf(fp, "    q  = pv[n];\n");
    fprintf(fp, "    nb = snprintf(t, nc, fmt, q.s, q.i, q.j, q.k);\n");

    fprintf(fp, "    if (nb < 0)\n");
    fprintf(fp, "       t = NULL;\n");

    fprintf(fp, "    return(t);}\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_NTOS - write the rendering function for type ID */

static void _SC_write_ntos(FILE *fp, int id)
   {

    fprintf(fp, "char *_SC_str_%s(char *t, int nc, void *s, long n, int mode)\n",
	    names[id]);
    fprintf(fp, "   {int nb;\n");
    fprintf(fp, "    char *fmt;\n");
    fprintf(fp, "    static int nld = 0;\n");
    fprintf(fp, "    %s *pv = (%s *) s;\n", types[id], types[id]);

    fprintf(fp, "    fmt = (mode == 1) ? _SC.types.formats[%d] : _SC.types.formata[%d];\n",
	    id, id);

/* non-standard long double I/O */
#if (AF_LONG_DOUBLE_IO == 1)
    fprintf(fp, "\n");
    fprintf(fp, "    {char *p;\n");
    fprintf(fp, "     for (p = fmt; *p != \'\\0\'; p++)\n");
    fprintf(fp, "         if (*p == \'L\')\n");
    fprintf(fp, "            *p++ = \'l\';};\n");
    fprintf(fp, "\n");

/* no long double I/O */
#elif (AF_LONG_DOUBLE_IO == 0)
    fprintf(fp, "\n");
    fprintf(fp, "    {char c, *s, *d;\n");
    fprintf(fp, "     for (s = fmt, d = fmt; *s != \'\\0\'; s++)\n");
    fprintf(fp, "         {c = *s;\n");
    fprintf(fp, "          if (c != \'L\')\n");
    fprintf(fp, "             *d++ = c;\n");
    fprintf(fp, "          else\n");
    fprintf(fp, "             nld = 1;};\n");
    fprintf(fp, "     *d = \'\\0\';};\n");
    fprintf(fp, "\n");
#endif

    fprintf(fp, "    if (nld == 1)\n");
    fprintf(fp, "       nb  = snprintf(t, nc, fmt, (double) pv[n]);\n");
    fprintf(fp, "    else\n");
    fprintf(fp, "       nb  = snprintf(t, nc, fmt, pv[n]);\n");

    fprintf(fp, "    if (nb < 0)\n");
    fprintf(fp, "       t = NULL;\n");

    fprintf(fp, "    return(t);}\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WRITE_PTR - write the rendering function for pointer type ID */

static void _SC_write_ptr(FILE *fp, int id)
   {

    fprintf(fp, "char *_SC_str_%s(char *t, int nc, void *s, long n, int mode)\n",
	    names[id]);
    fprintf(fp, "   {int nld, nb;\n");
    fprintf(fp, "    char *fmt;\n");
    fprintf(fp, "    %s *pv = (%s *) s;\n", types[id], types[id]);

    fprintf(fp, "    nld = 0;\n");
    fprintf(fp, "    fmt = (mode == 1) ? _SC.types.formats[%d] : _SC.types.formata[%d];\n",
	    id, id);

/* non-standard long double I/O */
#if (AF_LONG_DOUBLE_IO == 1)
    fprintf(fp, "\n");
    fprintf(fp, "    {char *p;\n");
    fprintf(fp, "     for (p = fmt; *p != \'\\0\'; p++)\n");
    fprintf(fp, "         if (*p == \'L\')\n");
    fprintf(fp, "            *p++ = \'l\';};\n");
    fprintf(fp, "\n");

/* no long double I/O */
#elif (AF_LONG_DOUBLE_IO == 0)
    fprintf(fp, "\n");
    fprintf(fp, "    {char c, *s, *d;\n");
    fprintf(fp, "     for (s = fmt, d = fmt; *s != \'\\0\'; s++)\n");
    fprintf(fp, "         {c = *s;\n");
    fprintf(fp, "          if (c != \'L\')\n");
    fprintf(fp, "             *d++ = c;\n");
    fprintf(fp, "          else\n");
    fprintf(fp, "             nld = 1;};\n");
    fprintf(fp, "     *d = \'\\0\';};\n");
    fprintf(fp, "\n");
#endif

    fprintf(fp, "    if (nld == 1)\n");
    fprintf(fp, "       nb  = snprintf(t, nc, fmt, *(double *) pv[n]);\n");
    fprintf(fp, "    else\n");
    fprintf(fp, "       nb  = snprintf(t, nc, fmt, pv[n]);\n");

    fprintf(fp, "    if (nb < 0)\n");
    fprintf(fp, "       t = NULL;\n");

    fprintf(fp, "    return(t);}\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_STR_DECL - write the declaration of the rendering array */

static void write_str_decl(FILE *fp)
   {int i;

    fprintf(fp, "typedef char *(*PFStrv)(char *t, int nc, void *s, long n, int mode);\n");
    fprintf(fp, "\n");

    fprintf(fp, "static PFStrv\n");
    fprintf(fp, " _SC_strf[] = {\n");
    for (i = 0; i < N_TYPES; i++)
        {if (types[i] != NULL)
	    {if (i == N_TYPES-1)
	        fprintf(fp, "                _SC_str_%s\n", names[i]);
	     else
	        fprintf(fp, "                _SC_str_%s,\n", names[i]);}
	 else
	    {if (i == N_TYPES-1)
	        fprintf(fp, "                NULL\n");
	     else
	        fprintf(fp, "                NULL,\n");};};

    fprintf(fp, "};\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_STR - write the rendering routines */

static void write_str(FILE *fp)
   {int i;

    Separator(fp);
    fprintf(fp, "/*                          NUMBER RENDERING                                */\n\n");
    Separator(fp);

    for (i = 0; i < N_TYPES; i++)
	{if (types[i] != NULL)
	    {if (i == I_BOOL)
	        _SC_write_bool(fp, i);
	     else if (IS_COMPLEX(i))
	        _SC_write_complex(fp, i);
	     else if (IS_QUATERNION(i))
	        _SC_write_quaternion(fp, i);
	     else if (IS_POINTER(i))
	        _SC_write_ptr(fp, i);
	     else
	        _SC_write_ntos(fp, i);};};

    write_str_decl(fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int i, rv;
    char *outf;
    FILE *fo;

    rv = 0;

    outf = NULL;
    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-o") == 0)
	    outf = v[++i];};

    if (outf != NULL)
       fo = fopen_safe(outf, "w");
    else
       fo = stdout;

    if (fo != NULL)
       {write_header(fo);

/* emit type conversion code */
	write_converters(fo);

/* emit number to string code */
	write_str(fo);

	Separator(fo);

	if (outf != NULL)
	   fclose_safe(fo);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
