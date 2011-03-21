/*
 * TEMPLATE.C - generate C functions for primitive types from templates
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#define NO_VA_ARG_ID

#include "common.h"
#include "libpsh.c"
#include <scope_typeh.h>

#define NMAX 100

#define I_BOOL        2
#define I_NON         3
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
    int nl;                   /* number of lines of body */
    int lo;                   /* starting line number in file */
    char *rtype;
    char *fname;
    char *args;
    char **body;};

static char
 *names[]  = { NULL, NULL, "bool",
	       "char", "wchar",
	       "int8", "shrt", "int", "lng", "ll",
	       "flt", "dbl", "ldbl",
	       "fcx", "dcx", "ldcx",
	       "qut", "ptr", NULL, "str" },
 *types[]  = { NULL, NULL, "bool",
	       "char", "wchar_t",
	       "int8_t", "short", "int", "long", "long long",
	       "float", "double", "long double",
	       "float _Complex", "double _Complex",
	       "long double _Complex",
	       "quaternion", "void *", NULL, "char *" },
 *stypes[] = { NULL, NULL, "bool",
	       "signed char", "wchar_t",
	       "int8_t", "signed short", "signed int",
               "signed long", "signed long long",
	       "float", "double", "long double",
	       "float _Complex", "double _Complex",
	       "long double _Complex",
	       "quaternion", "void *", NULL, "char *" },
 *utypes[] = { NULL, NULL, "bool",
	       "unsigned char", "wchar_t",
	       "uint8_t", "unsigned short", "unsigned int",
               "unsigned long", "unsigned long long",
	       "float", "double", "long double",
	       "float _Complex", "double _Complex",
	       "long double _Complex",
	       "quaternion", "void *", NULL, "char *" },
 *comp[]   = { NULL, NULL, "bool",
	       "char", "wchar_t",
	       "int8_t", "short", "int", "long", "long long",
	       "float", "double", "long double",
	       "float", "double",
	       "long double",
	       "double", "void *", NULL, "char *" },
 *promo[] =  { NULL, NULL, "int",
               "int", "int",
	       "int", "int", "int", "long", "long long",
	       "double", "double", "long double",
	       "float _Complex", "double _Complex",
	       "long double _Complex",
	       "quaternion", "void *", NULL, "char *" },
 *mn[]    =  { NULL, NULL, "BOOL_MIN",
	       "CHAR_MIN", "WCHAR_MIN",
	       "INT8_MIN", "SHRT_MIN", "INT_MIN", "LONG_MIN", "LLONG_MIN",
	       "-FLT_MAX", "-DBL_MAX", "-LDBL_MAX",
	       "-FLT_MAX", "-DBL_MAX", "-LDBL_MAX",
	       "-DBL_MAX", "-LLONG_MAX" },
 *mx[]    =  { NULL, NULL, "BOOL_MAX",
	       "CHAR_MAX", "WCHAR_MAX",
	       "INT8_MAX", "SHRT_MAX", "INT_MAX", "LONG_MAX", "LLONG_MAX",
	       "FLT_MAX", "DBL_MAX", "LDBL_MAX",
	       "FLT_MAX", "DBL_MAX", "LDBL_MAX",
	       "DBL_MAX", "LLONG_MAX" };

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_TEMPLATE - instantiate a template */

template *make_template(char *proto, int nl, char **body,
			char *tmn, char *tmx, int ln)
   {int i, n;
    char rtype[MAXLINE];
    char *p, *args, *fname, **sa;
    template *t;

    t = MAKE(template);

/* reduce the type range names to indeces */
    for (i = 0; i < N_TYPES; i++)
        {if (types[i] != NULL)
	    {if (strcmp(tmn, types[i]) == 0)
	        t->tmn = i;
	     else if (strcmp(tmx, types[i]) == 0)
	        t->tmx = i;};};

/* parse out the prototype */
    p = strtok(proto, "(\n\r");

/* get the arg list */
    args = strtok(NULL, "");
    LAST_CHAR(args) = '\0';
    LAST_CHAR(args) = '\0';

/* get the function name */
    sa = tokenize(p, " \t\n\r");
    for (n = 0; sa[n] != NULL; n++);
    fname = sa[--n];

/* get the qualifiers */
    memset(rtype, 0, MAXLINE);
    for (i = 0; i < n; i++)
        {nstrcat(rtype, MAXLINE, sa[i]);
	 nstrcat(rtype, MAXLINE, " ");};
    LAST_CHAR(rtype) = '\0';
        
    t->rtype = STRSAVE(rtype);
    t->fname = STRSAVE(fname);
    t->args  = STRSAVE(args);
    t->nl    = nl;
    t->lo    = ln;
    t->body  = body;

    free_strings(sa);
    FREE(tmn);
    FREE(tmx);

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

static void write_header(FILE *fp, char *inf)
   {char s[MAXLINE];
    char *p;

    nstrncpy(s, MAXLINE, inf, -1);
    p = path_tail(upcase(strtok(s, ".\n")));

    fprintf(fp, "/*\n");
    fprintf(fp, " * %s.H - generated type handling routines - do not edit\n",
	    p);
    fprintf(fp, " *\n");
    fprintf(fp, " * Source Version: 3.0\n");
    fprintf(fp, " * Software Release #: LLNL-CODE-422942\n");
    fprintf(fp, " *\n");
    fprintf(fp, " */\n");
    fprintf(fp, "\n");

    fprintf(fp, "#ifndef PCK_%s\n", p);
    fprintf(fp, "\n");
    fprintf(fp, "#define PCK_%s\n", p);
    fprintf(fp, "\n");

    fprintf(fp, "#include \"cpyright.h\"\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TRAILER - write the end of the file */

static void write_trailer(FILE *fp, char *inf)
   {

    fprintf(fp, "\n");
    fprintf(fp, "#endif\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/

/*                          FUNCTION TEMPLATES                              */

/*--------------------------------------------------------------------------*/

/* WRITE_FNC_INSTANCE - write the function for type ID */

static void write_fnc_instance(FILE *fp, int id, template *t)
   {int i, nl;
    char s[MAXLINE];
    char **body, *rtype, *fname, *args, *p;

    Separator(fp);

    nl    = t->nl;
    body  = t->body;
    rtype = t->rtype;
    fname = t->fname;
    args  = t->args;

    fprintf(fp, "static %s %s_%s(%s)\n", rtype, fname, names[id], args);
    for (i = 0; i < nl; i++)
        {nstrncpy(s, MAXLINE, body[i], -1);
	 p = subst(s, "<TYPE>",     types[id],  -1);
	 p = subst(p, "<SIGNED>",   stypes[id], -1);
	 p = subst(p, "<UNSIGNED>", utypes[id], -1);
	 p = subst(p, "<CTYPE>",    comp[id],   -1);
	 p = subst(p, "<MIN>",      mn[id],     -1);
	 p = subst(p, "<MAX>",      mx[id],     -1);
	 fputs(p, fp);};

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_FNC_DECL - write the declaration of the template array */

static void write_fnc_decl(FILE *fp, template **tl, int it, int nt)
   {int i, lt, nl, ok, tmn, tmx;
    char **body, *rtype, *fname, *args;
    template *t;

    t = tl[it];
    if (t == NULL)
       return;

    nl    = t->nl;
    body  = t->body;
    rtype = t->rtype;
    fname = t->fname;
    args  = t->args;
    tmn   = t->tmn;
    tmx   = t->tmx;

    fprintf(fp, "typedef %s (*PF%s)(%s);\n", rtype, fname, args);
    fprintf(fp, "\n");

    fprintf(fp, "static PF%s\n", fname);
    fprintf(fp, " %s_fnc[] = {\n", fname);
    for (i = 0; i < N_TYPES; i++)
        {ok = FALSE;

/* scan templates for this type */
	 for (lt = it; lt < nt; lt++)
	     {t = tl[lt];
	      if ((t == NULL) || (strcmp(t->fname, fname) != 0))
		 continue;

	      body = t->body;
	      tmn  = t->tmn;
	      tmx  = t->tmx;
	      if ((types[i] != NULL) && (tmn <= i) && (i <= tmx))
		 {if (i == N_TYPES-1)
		     fprintf(fp, "                %s_%s\n", fname, names[i]);
		  else
		     fprintf(fp, "                %s_%s,\n", fname, names[i]);
		  ok = TRUE;
		  break;};};

	 if (ok == FALSE)
	    {if (i == N_TYPES-1)
	        fprintf(fp, "                NULL\n");
	     else
	        fprintf(fp, "                NULL,\n");};};

    fprintf(fp, "};\n");
    fprintf(fp, "\n");

/* free all templates with the current function name */
    for (lt = it; lt < nt; lt++)
        {t = tl[lt];
	 if (strcmp(t->fname, fname) == 0)
	    {free_template(tl[i]);
	     tl[lt] = NULL;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_FNC - write the routines from the template */

static void write_fnc(FILE *fp, template **tl, int it, int nt)
   {int i, tmn, tmx;
    template *t;

    t   = tl[it];
    tmn = t->tmn;
    tmx = t->tmx;

    for (i = tmn; i <= tmx; i++)
	{if (types[i] != NULL)
	    write_fnc_instance(fp, i, t);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_FNC_DECLS - write the function array declaration */

static void write_fnc_decls(FILE *fp, template **tl, int it, int nt)
   {

    Separator(fp);

    write_fnc_decl(fp, tl, it, nt);

    return;}

/*--------------------------------------------------------------------------*/

/*                            SWITCH TEMPLATES                              */

/*--------------------------------------------------------------------------*/

/* WRITE_SWITCH_CLAUSE - write the switch clause for type ID */

static void write_switch_clause(FILE *fp, int id, template *t)
   {int i, nl;
    char s[MAXLINE], u[MAXLINE];
    char **body, *rtype, *fname, *args, *p;

    nl    = t->nl;
    body  = t->body;
    rtype = t->rtype;
    fname = t->fname;
    args  = t->args;

    fprintf(fp, "        case %d :\n", id);

    for (i = 0; i < nl; i++)
        {nstrncpy(s, MAXLINE, body[i], -1);
	 p = subst(s, "<TYPE>", types[id], -1);
	 p = subst(p, "<CTYPE>", comp[id], -1);
	 p = subst(p, "<MIN>", mn[id], -1);
	 p = subst(p, "<MAX>", mx[id], -1);

	 nstrncpy(u, MAXLINE, "          ", -1);
	 nstrcat(u, MAXLINE, p);

	 fputs(u, fp);};

    fprintf(fp, "             break;\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_SWITCH - write switch based template */

static void write_switch(FILE *fp, template *t)
   {int i, nl;
    char **body, *rtype, *fname, *args;

    Separator(fp);

    nl    = t->nl;
    body  = t->body;
    rtype = t->rtype;
    fname = t->fname;
    args  = t->args;

    fprintf(fp, "static %s %s_switch(%s)\n", rtype, fname, args);
    fprintf(fp, "   {\n");
    fprintf(fp, "    \n");
    fprintf(fp, "    switch (id)\n");
    fprintf(fp, "       {\n");

    for (i = 0; i < N_TYPES; i++)
        {if (types[i] != NULL)
            write_switch_clause(fp, i, t);};

/* add empty default clause */
    fprintf(fp, "         default :\n");
    fprintf(fp, "             break;\n");
    fprintf(fp, "       };\n");
    fprintf(fp, "\n");

    fprintf(fp, "    return;}\n");
    fprintf(fp, "\n");

    Separator(fp);

    return;}

/*--------------------------------------------------------------------------*/

/*                            VA ARG TEMPLATES                              */

/*--------------------------------------------------------------------------*/

/* WRITE_VA_ARG_CLAUSE - write the variable arg clause for type ID */

static void write_va_arg_clause(FILE *fp, int id)
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

/* WRITE_VA_ARG - write the va arg routines */

static void write_va_arg(FILE *fp)
   {int i;

/* PATHSCALE 3.2.99 does not support complex types in va_arg */
    fprintf(fp, "#ifdef COMPILER_PATHSCALE\n");

    fprintf(fp, "#define SC_VA_ARG_ID(_id, _d, _n)                    \\\n");
    fprintf(fp, "   {int _lid;                                        \\\n");
    fprintf(fp, "    if (_id == SC_STRING_I)                          \\\n");
    fprintf(fp, "       _lid = _id;                                   \\\n");
    fprintf(fp, "    else if (SC_is_type_ptr(_id) == TRUE)            \\\n");
    fprintf(fp, "       _lid = SC_POINTER_I;                          \\\n");
    fprintf(fp, "    else                                             \\\n");
    fprintf(fp, "       _lid = _id;                                   \\\n");
    fprintf(fp, "    switch (_lid) {                                  \\\n");

    for (i = 0; i <= I_FLOAT; i++)
        {if (types[i] != NULL)
            write_va_arg_clause(fp, i);};

    for (i = I_COMPLEX+1; i < N_TYPES; i++)
        {if (types[i] != NULL)
            write_va_arg_clause(fp, i);};

    fprintf(fp, "       }                                             \\\n");
    fprintf(fp, "   }\n");

    fprintf(fp, "#else\n");

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
        {if (types[i] != NULL)
            write_va_arg_clause(fp, i);};

    fprintf(fp, "       }                                             \\\n");
    fprintf(fp, "   }\n");

    fprintf(fp, "#endif\n");

    return;}

/*--------------------------------------------------------------------------*/

/*                             PARSING ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* PARSE_TMPL - read and parse the next template from FP */

static template *parse_tmpl(FILE *fp, int *flo)
   {int i, lo, nt, nl, npo, npc, fpr, st;
    off_t ad;
    char s[MAXLINE], proto[MAXLINE];
    char *tmn, *tmx, *p, **sa;
    template *t;

    memset(proto, 0, MAXLINE);

    fpr = FALSE;
    nl  = 0;
    npo = 0;
    npc = 0;
    nt  = 0;
    sa  = NULL;
    lo  = *flo;
    for (i = 0; TRUE; i++)
        {ad = ftell(fp);

	 p = fgets(s, MAXLINE, fp);
	 if (p == NULL)
	    break;

	 (*flo)++;
	 if (blank_line(p) == TRUE)
	    continue;
	 else if (strncmp(p, "template<", 9) == 0)
	    {nt++;
	     if (nt == 1)
	        {strtok(p, "<|>");
		 tmn = STRSAVE(strtok(NULL, "<|>"));
		 tmx = STRSAVE(strtok(NULL, "<|>"));}
	     else
	        {st = fseek(fp, ad, SEEK_SET);
		 break;};
	     fpr = FALSE;}
	 else if (nt == 1)
	    {if (fpr == FALSE)
	        {npo += nchar(s, '(');
	         npc += nchar(s, ')');
		 nstrcat(proto, MAXLINE, s);
		 if ((npo == npc) && (npo > 0))
		    fpr = TRUE;}
	     else
	        {nl++;
		 sa = lst_push(sa, "%s", p);};};};

    if ((nt > 0) && (sa != NULL))
       t = make_template(proto, nl, sa, tmn, tmx, lo);
    else
       t = NULL;

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - print usage info */

static void help(void)
   {

    printf("\n");
    printf("Usage: template [-h] [-f] [-o <file>] [-s] [-va] [<tf>]\n");
    printf("\n");
    printf("   h    this help message\n");
    printf("   f    generate templates as array of functions (default)\n");
    printf("   s    generate templates as switch statement\n");
    printf("   o    write output to file (default stdout)\n");
    printf("   va   write special variable argument handler\n");
    printf("   <tf> is an template file containing one or more\n");
    printf("        templates\n");
    printf("\n");
    printf("\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int i, ln, nt, rv, tgt;
    char *inf, *outf;
    FILE *fi, *fo;
    template *t, *tl[NMAX];

    rv = 0;

    tgt  = 0;
    inf  = NULL;
    outf = NULL;
    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-h") == 0)
	    {help();
	     return(1);}
	 else if (strcmp(v[i], "-f") == 0)
	    tgt = 0;
	 else if (strcmp(v[i], "-o") == 0)
	    outf = v[++i];
	 else if (strcmp(v[i], "-s") == 0)
	    tgt = 1;
	 else if (strcmp(v[i], "-va") == 0)
	    tgt = 2;
	 else
	    inf = v[i];};

    if (inf != NULL)
       fi = fopen(inf, "r+");

    if (tgt == 2)
       {nt  = 0;
	inf = "scarg.h";}

    else if ((inf == NULL) || (fi == NULL))
       {printf("No input file specified - exiting\n");
	return(1);}

    else
       {memset(tl, 0, sizeof(tl));

	ln = 1;

	for (nt = 0; nt < NMAX; nt++)
	    {t = parse_tmpl(fi, &ln);
	     if (t == NULL)
	        break;
	     tl[nt] = t;};};

    if (outf != NULL)
       fo = fopen(outf, "w");
    else
       fo = stdout;

    write_header(fo, inf);

    switch (tgt)

/* write template as an array of functions */
       {case 0 :
	     for (i = 0; i < nt; i++)
	         write_fnc(fo, tl, i, nt);

	     for (i = 0; i < nt; i++)
	         write_fnc_decls(fo, tl, i, nt);

	     break;

/* write template as a switch statement */
	case 1 :
	     for (i = 0; i < nt; i++)
	         write_switch(fo, tl[i]);

	     break;

/* write special template for variable arg handler */
	case 2 :
	     write_va_arg(fo);
	     break;};

    for (i = 0; i < nt; i++)
	free_template(tl[i]);

    write_trailer(fo, inf);

    if (outf != NULL)
       fclose(fo);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
