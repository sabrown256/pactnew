/*
 * CONVERT.C - generate primitive type converters
 *
 */

#include "scstd.h"

#define N_PRIMITIVES 14

static char
 *names[N_PRIMITIVES] = { NULL, NULL, "bool", "char",
			  "shrt", "int", "lng", "ll",
			  "flt", "dbl", "ldbl",
			  "fcx", "dcx", "ldcx" },
 *types[N_PRIMITIVES] = { NULL, NULL, "bool", "char",
			  "short", "int", "long", "long long",
			  "float", "double", "long double",
			  "float complex", "double complex",
			  "long double complex" };

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_HEADER - write the beginning of the file */

static void write_header(FILE *fp)
   {

    fprintf(fp, "/*\n");
    fprintf(fp, " * SCCONV.C - generated conversion routine matrix\n");
    fprintf(fp, " *\n");
    fprintf(fp, " */\n");
    fprintf(fp, " \n");
    fprintf(fp, "#include <cpyright.h>\n");
    fprintf(fp, "#include <score.h>\n");
    fprintf(fp, " \n");
    fprintf(fp, "#undef CONVERT\n");
    fprintf(fp, " \n");

    fprintf(fp, "#define CONVERT(_otyp, _out, _ityp, _in)                   \\\n");
    fprintf(fp, "void _SC_ ## _out ## _ ## _in(void *out, void *in, long n) \\\n");
    fprintf(fp, "   {long i;                                                \\\n");
    fprintf(fp, "    _otyp *po = (_otyp *) out;                             \\\n");
    fprintf(fp, "    _ityp *pi = (_ityp *) in;                              \\\n");
    fprintf(fp, "    for (i = 0; i < n; i++, *po++ = *pi++);                \\\n");
    fprintf(fp, "    return;}\n");
    fprintf(fp, "\n");

    fprintf(fp, "typedef void (*PFConv)(void *d, void *s, long n);\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_ROUTINES - write the conversion routines */

static void write_routines(FILE *fp)
   {int i, j;

    for (i = 0; i < N_PRIMITIVES; i++)
        for (j = 0; j < N_PRIMITIVES; j++)
	    {if ((types[i] != NULL) && (types[j] != NULL))
	        fprintf(fp, "CONVERT(%s, %s, %s, %s)\n\n",
			types[i], names[i], types[j], names[j]);};

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_DECL - write the declaration of the conversion array */

static void write_decl(FILE *fp)
   {int i, j;

    fprintf(fp, "static PFConv\n");
    fprintf(fp, " _SC_convf[][] = {\n");
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

/* WRITE_CONV - write the master conversion routine */

static void write_conv(FILE *fp)
   {

    fprintf(fp, "void SC_convert_primitive(int did, void *d, int sid, void *s, long n)\n");
    fprintf(fp, "   {\n");
    fprintf(fp, "    \n");
    fprintf(fp, "    if (_SC_convf[did][sid] != NULL)\n");
    fprintf(fp, "       _SC_convf[did][sid](d, s, n);\n");
    fprintf(fp, "    \n");
    fprintf(fp, "    return;}\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TRAILER - write the end of the file */

static void write_trailer(FILE *fp)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int rv;
    FILE *fp;

    rv = 0;

    fp = fopen("scconv.c", "w");

    write_header(fp);

    write_routines(fp);

    write_decl(fp);

    write_conv(fp);

    write_trailer(fp);

    fclose(fp);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
