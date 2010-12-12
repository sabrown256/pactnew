/*
 * VARG.C - generate primitive va_arg accessors
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#define NO_VA_ARG_ID

#include "scstd.h"
#include "scope_typeh.h"

#define I_NON         3
#define I_FIX         I_NON+N_PRIMITIVE_FIX    /* last fixed point type */
#define I_FLOAT       I_FIX+N_PRIMITIVE_FP     /* last floating point type */
#define I_COMPLEX     I_FLOAT+N_PRIMITIVE_CPX  /* last complex type */
#define I_QUATERNION  I_COMPLEX+1
#define I_POINTER     I_QUATERNION+1

static char
 *types[] = { NULL, NULL, "bool",
	      "char", "wchar_t",
	      "int8_t", "short", "int", "long", "long long",
	      "float", "double", "long double",
	      "float _Complex", "double _Complex",
	      "long double _Complex",
	      "quaternion", "void *", NULL, "char *"},
 *promo[] = { NULL, NULL, "int",
              "int", "int",
	      "int", "int", "int", "long", "long long",
	      "double", "double", "long double",
	      "float _Complex", "double _Complex",
	      "long double _Complex",
	      "quaternion", "void *", NULL, "char *"};

/*--------------------------------------------------------------------------*/

/*                           VARIABLE ARG HANDLING                          */

/*--------------------------------------------------------------------------*/

/* _SC_WRITE_ARGH - write the variable arg handler for type ID */

static void _SC_write_argh(FILE *fp, int id)
   {

    fprintf(fp, "       case %d:                                    \\\n",
	    id);
    fprintf(fp, "            {%s *_pv = (%s *) (_d);                \\\n",
	    types[id], types[id]);
    fprintf(fp, "             _pv[_n] = va_arg(__a__, %s);};        \\\n",
	    promo[id]);
    fprintf(fp, "            break;                                 \\\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_ARGS - write the va arg routines */

static void write_args(FILE *fp)
   {int i, ng, no_complex;

/* PATHSCALE 2.5 does not support complex types in va_arg */
#if defined(COMPILER_PATHSCALE)
    no_complex = TRUE;
#else
    no_complex = FALSE;
#endif

    fprintf(fp, "/*\n");
    fprintf(fp, " * SCARG.H - variable arg macros for SCORE type manager\n");
    fprintf(fp, " *         - automatically generated - do not edit\n");
    fprintf(fp, " *\n");
    fprintf(fp, " * Source Version: 3.0\n");
    fprintf(fp, " * Software Release #: LLNL-CODE-422942\n");
    fprintf(fp, " *\n");
    fprintf(fp, " */\n");
    fprintf(fp, "\n");

    fprintf(fp, "#ifndef PCK_SCARG\n");
    fprintf(fp, "\n");
    fprintf(fp, "#define PCK_SCARG\n");
    fprintf(fp, "\n");
    fprintf(fp, "#include \"cpyright.h\"\n");
    fprintf(fp, "\n");

    fprintf(fp, "#define SC_VA_ARG_ID(_id, _d, _n)                    \\\n");
    fprintf(fp, "   {int _lid;                                        \\\n");
    fprintf(fp, "    if (_id == SC_STRING_I)                          \\\n");
    fprintf(fp, "       _lid = _id;                                   \\\n");
    fprintf(fp, "    else if (SC_is_type_ptr(_id) == TRUE)            \\\n");
    fprintf(fp, "       _lid = SC_POINTER_I;                          \\\n");
    fprintf(fp, "    else                                             \\\n");
    fprintf(fp, "       _lid = _id;                                   \\\n");
    fprintf(fp, "    switch (_lid) {                                  \\\n");

    for (i = 0; i < N_TYPES; i++)
        {ng = ((types[i] == NULL) ||
	       ((no_complex == TRUE) && (I_FLOAT < i) && (i <= I_COMPLEX)));
	 if (ng == FALSE)
            _SC_write_argh(fp, i);};

    fprintf(fp, "       }                                             \\\n");
    fprintf(fp, "   }                                                 \\\n");
    fprintf(fp, "\n");

    fprintf(fp, "#endif\n");
    fprintf(fp, "\n");

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
       fo = fopen(outf, "w");
    else
       fo = stdout;

/* emit var arg code */
    write_args(fo);

    if (outf != NULL)
       fclose(fo);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
