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
#include "libtyp.c"

#define NMAX 100

#define I_BOOL        2
#define I_CHAR        (I_BOOL+N_PRIMITIVE_CHAR)
#define I_FIX         (I_CHAR+N_PRIMITIVE_FIX)   /* last fixed point type */
#define I_FLOAT       (I_FIX+N_PRIMITIVE_FP)     /* last floating point type */
#define I_COMPLEX     (I_FLOAT+N_PRIMITIVE_CPX)  /* last complex floating point type */
#define I_QUATERNION  (I_COMPLEX+1)
#define I_POINTER     (I_QUATERNION+1)

#define Separator(_f)  fprintf(_f, "/*--------------------------------------------------------------------------*/\n\n")

typedef struct s_template template;
typedef struct s_typinf typinf;

struct s_template
   {int tmn;
    int tmx;
    int nl;                   /* number of lines of body */
    int lo;                   /* starting line number in file */
    char *rtype;
    char *fname;
    char *args;
    char **body;};

struct s_typinf
   {int id;
    char *name;
    char *type;
    char *stype;
    char *utype;
    char *comp;
    char *promo;
    char *mn;
    char *mx;};

static typinf
 sti[] = {{0,  NULL,   NULL, NULL, NULL, NULL, NULL, NULL, NULL},
	  {1,  NULL,   NULL, NULL, NULL, NULL, NULL, NULL, NULL},
	  {2,  "bool", "bool", "bool", "bool", "bool", "int", "BOOL_MIN", "BOOL_MAX"},
	  {3,  "char", "char", "signed char", "unsigned char", "char", "int", "SCHAR_MIN", "SCHAR_MAX"},
	  {4,  "wchr", "wchar_t", "wchar_t", "wchar_t", "wchar_t", "int", "WCHAR_MIN", "WCHAR_MAX"},
	  {5,  "int8", "int8_t", "int8_t", "u_int8_t", "int8_t", "int", "INT8_MIN", "INT8_MAX"},
	  {6,  "shrt", "short", "signed short", "unsigned short", "short", "int", "SHRT_MIN", "SHRT_MAX"},
	  {7,  "int",  "int", "signed int", "unsigned int", "int", "int", "INT_MIN", "INT_MAX"},
	  {8,  "lng",  "long", "signed long", "unsigned long", "long", "long", "LONG_MIN", "LONG_MAX"},
	  {9,  "ll",   "long long", "signed long long", "unsigned long long", "long long", "long long", "LLONG_MIN", "LLONG_MAX"},
	  {10, "flt",  "float", "float", "float", "float", "double", "-FLT_MAX", "FLT_MAX"},
	  {11, "dbl",  "double", "double", "double", "double", "double", "-DBL_MAX", "DBL_MAX"},
	  {12, "ldbl", "long double", "long double", "long double", "long double", "long double", "-LDBL_MAX", "LDBL_MAX"},
	  {13, "fcx",  "float _Complex", "float _Complex", "float _Complex", "float", "float _Complex", "-FLT_MAX", "FLT_MAX"},
	  {14, "dcx",  "double _Complex", "double _Complex", "double _Complex", "double", "double _Complex", "-DBL_MAX", "DBL_MAX"},
	  {15, "ldcx", "long double _Complex", "long double _Complex", "long double _Complex", "long double", "long double _Complex", "-LDBL_MAX", "LDBL_MAX"},
	  {16, "qut",  "quaternion", "quaternion", "quaternion", "double", "quaternion", "-DBL_MAX", "DBL_MAX"},
	  {17, "ptr",  "void *", "void *", "void *", "void *", "void *", "-LLONG_MAX", "LLONG_MAX"},
	  {18, NULL,   NULL, NULL, NULL, NULL, NULL, NULL, NULL},
	  {19, "str",  "char *", "char *", "char *", "char *", "char *", "-LLONG_MAX", "LLONG_MAX"} },
  *ti;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_TYPE_TABLE - construct the internal representation
 *                 - of the type table
 */

static void make_type_table(char *tytab)
   {

    parse_type_table(tytab);

    ti = sti;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_TEMPLATE - instantiate a template */

template *make_template(char *proto, int nl, char **body,
			char *tmn, char *tmx, int ln)
   {int i, id, n;
    char rtype[BFLRG];
    char *p, *args, *fname, **sa;
    template *t;

    t = MAKE(template);

    t->tmn = 0;
    t->tmx = 0;

/* reduce the type range names to indeces */
    for (i = 0; i < N_TYPES; i++)
        {id = ti[i].id;
	 if (ti[i].type != NULL)
	    {if (strcmp(tmn, ti[i].type) == 0)
	        t->tmn = id;
	     else if (strcmp(tmx, ti[i].type) == 0)
	        t->tmx = id;};};

/* parse out the prototype */
    p = strtok(proto, "(\n\r");

/* get the arg list */
    args = strtok(NULL, "");
    LAST_CHAR(args) = '\0';
    LAST_CHAR(args) = '\0';

/* get the function name */
    sa = tokenize(p, " \t\n\r", 0);
    for (n = 0; sa[n] != NULL; n++);
    fname = sa[--n];

/* get the qualifiers */
    memset(rtype, 0, BFLRG);
    for (i = 0; i < n; i++)
        {nstrcat(rtype, BFLRG, sa[i]);
	 nstrcat(rtype, BFLRG, " ");};
    LAST_CHAR(rtype) = '\0';
        
    t->nl    = nl;
    t->lo    = ln;
    t->rtype = STRSAVE(rtype);
    t->fname = STRSAVE(fname);
    t->args  = STRSAVE(args);
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
   {char s[BFLRG];
    char *p;

    nstrncpy(s, BFLRG, inf, -1);
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
    char s[BFLRG];
    char **body, *rtype, *fname, *args, *p;

    Separator(fp);

    nl    = t->nl;
    body  = t->body;
    rtype = t->rtype;
    fname = t->fname;
    args  = t->args;

    fprintf(fp, "static %s %s_%s(%s)\n", rtype, fname,
	    ti[id].name, args);
    for (i = 0; i < nl; i++)
        {nstrncpy(s, BFLRG, body[i], -1);
	 p = subst(s, "<TYPE>",     ti[id].type,  -1);
	 p = subst(p, "<SIGNED>",   ti[id].stype, -1);
	 p = subst(p, "<UNSIGNED>", ti[id].utype, -1);
	 p = subst(p, "<CTYPE>",    ti[id].comp,  -1);
	 p = subst(p, "<MIN>",      ti[id].mn,    -1);
	 p = subst(p, "<MAX>",      ti[id].mx,    -1);
	 fputs(p, fp);};

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_FNC_DECL - write the declaration of the template array */

static void write_fnc_decl(FILE *fp, template **tl, int it, int nt)
   {int i, id, lt, ok, tmn, tmx;
    char *rtype, *fname, *args;
    template *t;

    t = tl[it];
    if (t == NULL)
       return;

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
	 id = i;

/* scan templates for this type */
	 for (lt = it; lt < nt; lt++)
	     {t = tl[lt];
	      if ((t == NULL) || (strcmp(t->fname, fname) != 0))
		 continue;

	      tmn = t->tmn;
	      tmx = t->tmx;
	      if ((ti[i].type != NULL) && (tmn <= id) && (id <= tmx))
		 {if (i == N_TYPES-1)
		     fprintf(fp, "                %s_%s\n",
			     fname, ti[i].name);
		  else
		     fprintf(fp, "                %s_%s,\n",
			     fname, ti[i].name);
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
	{if (ti[i].type != NULL)
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
    char s[BFLRG], u[BFLRG];
    char **body, *p;

    nl   = t->nl;
    body = t->body;

    fprintf(fp, "        case %d :\n", id);

    for (i = 0; i < nl; i++)
	{nstrncpy(s, BFLRG, body[i], -1);
	 p = subst(s, "<TYPE>",  ti[i].type, -1);
	 p = subst(p, "<CTYPE>", ti[i].comp, -1);
	 p = subst(p, "<MIN>",   ti[i].mn,   -1);
	 p = subst(p, "<MAX>",   ti[i].mx,   -1);

	 nstrncpy(u, BFLRG, "          ", -1);
	 nstrcat(u, BFLRG, p);

	 fputs(u, fp);};

    fprintf(fp, "             break;\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_SWITCH - write switch based template */

static void write_switch(FILE *fp, template *t)
   {int i;
    char *rtype, *fname, *args;

    Separator(fp);

    rtype = t->rtype;
    fname = t->fname;
    args  = t->args;

    fprintf(fp, "static %s %s_switch(%s)\n", rtype, fname, args);
    fprintf(fp, "   {\n");
    fprintf(fp, "    \n");
    fprintf(fp, "    switch (id)\n");
    fprintf(fp, "       {\n");

    for (i = 0; i < N_TYPES; i++)
        {if (ti[i].type != NULL)
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

static void write_va_arg_clause(FILE *fp, int i)
   {int id;

    id = ti[i].id;

    fprintf(fp, "       case %d:                                    \\\n",
	    id);
    fprintf(fp, "            {%s *_pv = (%s *) (_d);                \\\n",
	    ti[i].type, ti[i].type);
    fprintf(fp, "             _pv[_n] = va_arg(__a__, %s);};        \\\n",
	    ti[i].promo);
    fprintf(fp, "            break;                                 \\\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_VA_ARG - write the va arg routines */

static void write_va_arg(FILE *fp)
   {int i, id;

/* if va_arg supports complex types */
    fprintf(fp, "#ifdef HAVE_COMPLEX_VA_ARG\n");

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
        {if (ti[i].type != NULL)
	    {id = i;
             write_va_arg_clause(fp, id);};};

    fprintf(fp, "       }                                             \\\n");
    fprintf(fp, "   }\n");

/* if va_arg does NOT support complex type */
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

    for (i = 0; i <= I_FLOAT; i++)
        {if (ti[i].type != NULL)
	    {id = i;
	     write_va_arg_clause(fp, id);};};

    for (i = I_COMPLEX+1; i < N_TYPES; i++)
        {if (ti[i].type != NULL)
	    {id = i;
	     write_va_arg_clause(fp, id);};};

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
    char s[BFLRG], proto[BFLRG];
    char *tmn, *tmx, *p, **sa;
    template *t;

    memset(proto, 0, BFLRG);

    fpr = FALSE;
    nl  = 0;
    npo = 0;
    npc = 0;
    nt  = 0;
    sa  = NULL;
    lo  = *flo;
    for (i = 0; TRUE; i++)
        {ad = ftell(fp);

	 p = fgets(s, BFLRG, fp);
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
                 ASSERT(st == 0);
		 break;};
	     fpr = FALSE;}
	 else if (nt == 1)
	    {if (fpr == FALSE)
	        {npo += strcntc(s, '(', TRUE);
	         npc += strcntc(s, ')', TRUE);
		 nstrcat(proto, BFLRG, s);
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
    char tmpf[BFLRG], tytab[BFLRG];
    char *inf, *outf, *s;
    FILE *fi, *fo;
    template *t, *tl[NMAX];

    rv = 0;

    s = getenv("DB_TYPES");
    if (s != NULL)
       nstrncpy(tytab, BFLRG, s, -1);

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
	 else if (strcmp(v[i], "-t") == 0)
	    nstrncpy(tytab, BFLRG, v[++i], -1);
	 else if (strcmp(v[i], "-va") == 0)
	    tgt = 2;
	 else
	    inf = v[i];};

    if (IS_NULL(tytab) == TRUE)
       {printf("No type table file specified - exiting\n");
	return(1);}
    else
       make_type_table(tytab);

    if (inf != NULL)
       fi = fopen_safe(inf, "r+");

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
       {snprintf(tmpf, BFLRG, "%s.%d", outf, getpid());

/* write a temporary file then rename it at the end
 * so that if the file currently exists it can be used
 * and there will be no gap when it does not exist or is empty
 */
	fo = fopen_safe(tmpf, "w");}
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
       {fclose_safe(fo);
	rename(tmpf, outf);};

    log_safe("dump", 0, NULL, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
