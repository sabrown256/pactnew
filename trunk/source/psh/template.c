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
    int nl;
    char *rtype;
    char *fname;
    char *args;
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

template *make_template(char *proto, int nl, char **body,
			char *tmn, char *tmx)
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
    args = strtok(NULL, "\n\r");
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
    fprintf(fp, " * %s.H - generated type handling routines\n", p);
    fprintf(fp, " *\n");
    fprintf(fp, " * Source Version: 3.0\n");
    fprintf(fp, " * Software Release #: LLNL-CODE-422942\n");
    fprintf(fp, " *\n");
    fprintf(fp, " */\n");
    fprintf(fp, " \n");

    fprintf(fp, "#include <cpyright.h>\n");
    fprintf(fp, " \n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_FNC - write the function for type ID */

static void write_fnc(FILE *fp, int id, template *t)
   {int i, nl;
    char s[MAXLINE];
    char **body, *rtype, *fname, *args, *p;

    nl    = t->nl;
    body  = t->body;
    rtype = t->rtype;
    fname = t->fname;
    args  = t->args;

    fprintf(fp, "static %s %s_%s(%s)\n", rtype, fname, names[id], args);
    for (i = 1; i < nl; i++)
        {nstrncpy(s, MAXLINE, body[i], -1);
	 p = subst(s, "<TYPE>", types[id], -1);
	 p = subst(p, "<MIN>", mn[id], -1);
	 p = subst(p, "<MAX>", mx[id], -1);
	 fputs(p, fp);};

    fprintf(fp, "\n");

    Separator(fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TMPL_DECL - write the declaration of the template array */

static void write_tmpl_decl(FILE *fp, template *t)
   {int i, nl, tmn, tmx;
    char **body, *rtype, *fname, *args;

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
        {if ((types[i] != NULL) && (tmn <= i) && (i <= tmx))
	    {if (i == N_TYPES-1)
	        fprintf(fp, "                %s_%s\n", fname, names[i]);
	     else
	        fprintf(fp, "                %s_%s,\n", fname, names[i]);}
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
   {int i, tmn, tmx;

    Separator(fp);

    tmn = t->tmn;
    tmx = t->tmx;

    for (i = tmn; i <= tmx; i++)
	{if (types[i] != NULL)
	    write_fnc(fp, i, t);};

    write_tmpl_decl(fp, t);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_TMPL - read and parse the next template from FP */

static template *parse_tmpl(FILE *fp)
   {int i, nt, nl, npo, npc, fpr;
    off_t ad;
    char s[MAXLINE], proto[MAXLINE];
    char *tmn, *tmx, *p, **sa;
    template *t;

    fpr = FALSE;
    npo = 0;
    npc = 0;
    nt  = 0;
    sa  = NULL;
    for (i = 0; TRUE; i++)
        {ad = ftell(fp);

	 p = fgets(s, MAXLINE, fp);
	 if (p == NULL)
	    break;
	 if (blank_line(p) == TRUE)
	    continue;
	 else if (strncmp(p, "template<", 9) == 0)
	    {nt++;
	     if (nt == 1)
	        {strtok(p, "<|>");
		 tmn = STRSAVE(strtok(NULL, "<|>"));
		 tmx = STRSAVE(strtok(NULL, "<|>"));
	         memset(proto, 0, MAXLINE);}
	     else
	        {fseek(fp, SEEK_SET, ad);
		 break;};
	     nl  = 0;
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
       t = make_template(proto, nl, sa, tmn, tmx);
    else
       t = NULL;

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
       fi = fopen(inf, "r+");

    if ((inf == NULL) || (fi == NULL))
       {printf("No input file specified - exiting\n");
	return(1);};

    if (outf != NULL)
       fo = fopen(outf, "w");
    else
       fo = stdout;

    write_header(fo, inf);

    while (TRUE)
       {t = parse_tmpl(fi);
	if (t == NULL)
	   break;

	write_tmpl(fo, t);

	free_template(t);};

    if (outf != NULL)
       fclose(fo);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
