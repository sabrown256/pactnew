/*
 * TEMPLATE.C - generate C functions for primitive types from templates
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "common.h"
#include "libpsh.c"
#include <scope_typeh.h>

#define I_BOOL        2
#define I_CHAR        (I_BOOL+N_PRIMITIVE_CHAR)
#define I_FIX         (I_CHAR+N_PRIMITIVE_FIX)   /* last fixed point type */
#define I_FLOAT       (I_FIX+N_PRIMITIVE_FP)     /* last floating point type */
#define I_COMPLEX     (I_FLOAT+N_PRIMITIVE_CPX)  /* last complex floating point type */
#define I_QUATERNION  (I_COMPLEX+1)
#define I_POINTER     (I_QUATERNION+1)

#define Separator(_f)  fprintf(_f, "/*--------------------------------------------------------------------------*/\n\n")

typedef struct s_template template;

struct s_template
   {int tmn;
    int tmx;
    char **body;};

static char
 *names[] = { NULL, NULL, "bool",
	      "char", "wchar",
	      "int8", "shrt", "int", "lng", "ll",
	      "flt", "dbl", "ldbl",
	      "fcx", "dcx", "ldcx",
	      "qut", "ptr", NULL, "str" },
 *types[] = { NULL, NULL, "bool",
	      "char", "wchar_t",
	      "int8_t", "short", "int", "long", "long long",
	      "float", "double", "long double",
	      "float _Complex", "double _Complex",
	      "long double _Complex",
	      "quaternion", "void *", NULL, "char *" },
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

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_TEMPLATE - instantiate a template */

template *make_template(char **body, char *tmn, char *tmx)
   {int i;
    template *t;

    t = MAKE(template);

    for (i = 0; i < N_TYPES; i++)
        {if (strcmp(tmn, types[i]) == 0)
	    t->tmn = i;
	 else if (strcmp(tmx, types[i]) == 0)
	    t->tmx = i;};

    t->body = body;

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FREE_TEMPLATE - release a template */

void free_template(template *t)
   {

    if (t != NULL)
       {free_strings(t->body);
	FREE(t);};

    return;}

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

/* WRITE_FNC - write the function for type ID */

static void write_fnc(FILE *fp, int id, template *t)
   {

    fprintf(fp, "char *tmpl_%s(char *t, int nc, void *s, long n, int mode)\n",
	    names[id]);
    fprintf(fp, "   {int nb;\n");
    fprintf(fp, "    char *fmt;\n");
    fprintf(fp, "    %s *pv = (%s *) s;\n", types[id], types[id]);

    fprintf(fp, "    fmt = (mode == 1) ? _SC.types.formats[%d] : _SC.types.formata[%d];\n",
	    id, id);

    fprintf(fp, "    nb  = snprintf(t, nc, fmt, pv[n]);\n");

    fprintf(fp, "    if (nb < 0)\n");
    fprintf(fp, "       t = NULL;\n");

    fprintf(fp, "    return(t);}\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TMPL_DECL - write the declaration of the template array */

static void write_tmpl_decl(FILE *fp)
   {int i;

    fprintf(fp, "typedef char *(*PFStrv)(char *t, int nc, void *s, long n, int mode);\n");
    fprintf(fp, "\n");

    fprintf(fp, "static PFStrv\n");
    fprintf(fp, " tmplf[] = {\n");
    for (i = 0; i < N_TYPES; i++)
        {if (types[i] != NULL)
	    {if (i == N_TYPES-1)
	        fprintf(fp, "                tmpl_%s\n", names[i]);
	     else
	        fprintf(fp, "                tmpl_%s,\n", names[i]);}
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

/* WRITE_TMPL - write the routines from the template */

static void write_tmpl(FILE *fp, template *t)
   {int i;

    Separator(fp);

    for (i = 0; i < N_TYPES; i++)
	{if (types[i] != NULL)
	    write_fnc(fp, i, t);};

    write_tmpl_decl(fp);

    Separator(fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_TMPL - read and parse the next template from FP */

static template *parse_tmpl(FILE *fp)
   {int i, nt, nl;
    off_t ad;
    char s[MAXLINE];
    char *tmn, *tmx, *p, **sa;
    template *t;

    nt = 0;
    sa = NULL;
    for (i = 0; TRUE; i++)
        {ad = ftell(fp);

	 p = fgets(s, MAXLINE, fp);
	 if (p == NULL)
	    break;
	 if (blank_line(p) == TRUE)
	    continue;
	 else if (strncmp(p, "template<", 9) == 0)
	    {nl = 0;
	     nt++;
	     if (nt == 1)
	        {strtok(p, "<|>");
		 tmn = strtok(NULL, "<|>");
		 tmx = strtok(NULL, "<|>");}
	     else
	        {fseek(fp, SEEK_SET, ad);
		 break;};}
	 else if (nt == 1)
 	    {nl++;
	     sa = lst_push(sa, "%s", p);};};

    t = make_template(sa, tmn, tmx);

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int i, rv;
    char *inf, *outf;
    FILE *fi, *fo;
    template *t;

    rv = 0;

    inf  = NULL;
    outf = NULL;
    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-o") == 0)
	    outf = v[++i];
	 else
	    inf = v[i];};

    if (inf != NULL)
       fi = fopen(inf, "r");

    if ((inf == NULL) || (fi == NULL))
       {printf("No input file specified - exiting\n");
	return(1);};

    if (outf != NULL)
       fo = fopen(outf, "w");
    else
       fo = stdout;

    write_header(fo);

    while (TRUE)
       {t = parse_tmpl(fi);

	write_tmpl(fo, t);

	free_template(t);};

    if (outf != NULL)
       fclose(fo);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
