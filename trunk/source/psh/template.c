/*
 * TEMPLATE.C - generate C functions for primitive types from templates
 *
 * #include "cpyright.h"
 *
 */

#define NO_VA_ARG_ID
#define HAVE_ANSI_C9X_COMPLEX

#include "common.h"
#include "libpsh.c"
#include "libtyp.c"

#define NMAX 100

#define N_PRIMITIVES     type_counts[11]
#define N_TYPES          type_counts[12]

#define Separator(_f)  fprintf(_f, "/*--------------------------------------------------------------------------*/\n\n")

#define STRINGIFY(_x)    #_x

#define TYPE_FIELD(_f, _t)                                                   \
    ((_t == NULL) ? fprintf(_f, "NULL, ") : fprintf(_f, "\"%s\", ", _t))

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
 *realp[] = { "crealf", "creal", STRINGIFY(CREALL) },
 *imagp[] = { "cimagf", "cimag", STRINGIFY(CIMAGL) };

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_TYPE_TABLE - construct the internal representation
 *                 - of the type table
 */

static int make_type_table(char *tytab)
   {int rv;
    char t[BFLRG];
    char *s;

    if (IS_NULL(tytab) == TRUE)
       {s = getenv("DB_TYPES");
	if (s != NULL)
	   {nstrncpy(t, BFLRG, s, -1);
	    tytab = t;};};

    if (IS_NULL(tytab) == TRUE)
       {printf("No type table file specified - exiting\n");
	rv = FALSE;}
    else
       {parse_type_table(tytab);
	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_TEMPLATE - instantiate a template */

template *make_template(char *proto, int ne, typdes *tl,
			int nl, char **body,
			char *tmn, char *tmx, int ln)
   {int i, id, n;
    char rtype[BFLRG];
    char *p, *args, *fname, **sa;
    typdes *td;
    template *t;

    t = MAKE(template);

    t->tmn = 0;
    t->tmx = 0;

/* reduce the type range names to indeces */
    for (id = 0; id < ne; id++)
        {td = tl + id;
	 if (td->type != NULL)
	    {if (strcmp(tmn, td->type) == 0)
	        t->tmn = id;
	     else if (strcmp(tmx, td->type) == 0)
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

static void write_header(FILE *fp, char *inf, int pfl)
   {char s[BFLRG];
    char *p;

    nstrncpy(s, BFLRG, inf, -1);
#if 0
    p = path_tail(upcase(strtok(s, ".\n")));
#else
    p = path_tail(upcase(subst(s, ".", "_", -1)));
#endif

    fprintf(fp, "/*\n");
    fprintf(fp, " * %s.H - generated type handling routines - do not edit\n",
	    p);
    fprintf(fp, " *\n");
    fprintf(fp, " */\n");
    fprintf(fp, "\n");

    if (pfl == TRUE)
       {fprintf(fp, "#ifndef PCK_%s\n", p);
	fprintf(fp, "\n");
	fprintf(fp, "#define PCK_%s\n", p);
	fprintf(fp, "\n");};

    fprintf(fp, "#include \"cpyright.h\"\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TRAILER - write the end of the file */

static void write_trailer(FILE *fp, char *inf, int pfl)
   {

    fprintf(fp, "\n");

    if (pfl == TRUE)
       {fprintf(fp, "#endif\n");
	fprintf(fp, "\n");};

    return;}

/*--------------------------------------------------------------------------*/

/*                          FUNCTION TEMPLATES                              */

/*--------------------------------------------------------------------------*/

/* WRITE_FNC_INSTANCE - write the function for type TD */

static void write_fnc_instance(FILE *fp, typdes *td, template *t)
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
	    td->fncp, args);
    for (i = 0; i < nl; i++)
        {nstrncpy(s, BFLRG, body[i], -1);
	 p = subst(s, "<TYPE>",     td->type,  -1);
	 p = subst(p, "<SIGNED>",   td->stype, -1);
	 p = subst(p, "<UNSIGNED>", td->utype, -1);
	 p = subst(p, "<CTYPE>",    td->comp,  -1);
	 p = subst(p, "<MIN>",      td->mn,    -1);
	 p = subst(p, "<MAX>",      td->mx,    -1);
	 fputs(p, fp);};

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_FNC_DECL - write the declaration of the template array */

static void write_fnc_decl(FILE *fp, int ne, typdes *tl,
			   template **tm, int it, int nt)
   {int i, id, lt, ok, tmn, tmx;
    char *rtype, *fname, *args;
    template *t;
    typdes *td;

    t = tm[it];
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
    for (i = 0; i < ne; i++)
        {id = i;
	 td = tl + i;

/* scan templates for this type */
	 ok = FALSE;
	 if (td->g != KIND_POINTER)
	    {for (lt = it; lt < nt; lt++)
		 {t = tm[lt];
		  if ((t == NULL) || (strcmp(t->fname, fname) != 0))
		     continue;

		  tmn = t->tmn;
		  tmx = t->tmx;
		  if ((td->type != NULL) && (tmn <= id) && (id <= tmx))
		     {fprintf(fp, "                %s_%s",
			      fname, td->fncp);
		      ok = TRUE;
		      break;};};};

	 if (ok == FALSE)
	    fprintf(fp, "                NULL");

	 fprintf(fp, ",\n");};

    fprintf(fp, "   };\n");
    fprintf(fp, "\n");

/* free all templates with the current function name */
    for (lt = it; lt < nt; lt++)
        {t = tm[lt];
	 if (strcmp(t->fname, fname) == 0)
	    {free_template(tm[i]);
	     tm[lt] = NULL;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_FNC - write the routines from the template */

static void write_fnc(FILE *fp, int ne, typdes *tl,
		      template **tm, int it, int nt)
   {int id, tmn, tmx;
    typdes *td;
    template *t;

    t   = tm[it];
    tmn = t->tmn;
    tmx = t->tmx;

    for (id = tmn; id <= tmx; id++)
	{td = tl + id;
	 if (IS_PRIMITIVE_TYPE(td) == B_F)
	    continue;
         else
	    write_fnc_instance(fp, td, t);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_FNC_DECLS - write the function array declaration */

static void write_fnc_decls(FILE *fp, int ne, typdes *tl,
			    template **tm, int it, int nt)
   {

    Separator(fp);

    write_fnc_decl(fp, ne, tl, tm, it, nt);

    return;}

/*--------------------------------------------------------------------------*/

/*                            SWITCH TEMPLATES                              */

/*--------------------------------------------------------------------------*/

/* WRITE_SWITCH_CLAUSE - write the switch clause for type TD */

static void write_switch_clause(FILE *fp, typdes *td, template *t)
   {int i, nl;
    char s[BFLRG], u[BFLRG];
    char **body, *p;

    nl   = t->nl;
    body = t->body;

    fprintf(fp, "        case %d :\n", td->id);

    for (i = 0; i < nl; i++)
	{nstrncpy(s, BFLRG, body[i], -1);
	 p = subst(s, "<TYPE>",  td->type, -1);
	 p = subst(p, "<CTYPE>", td->comp, -1);
	 p = subst(p, "<MIN>",   td->mn,   -1);
	 p = subst(p, "<MAX>",   td->mx,   -1);

	 nstrncpy(u, BFLRG, "          ", -1);
	 nstrcat(u, BFLRG, p);

	 fputs(u, fp);};

    fprintf(fp, "             break;\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_SWITCH - write switch based template */

static void write_switch(FILE *fp, int ne, typdes *tl, template *t)
   {int id;
    char *rtype, *fname, *args;
    typdes *td;

    Separator(fp);

    rtype = t->rtype;
    fname = t->fname;
    args  = t->args;

    fprintf(fp, "static %s %s_switch(%s)\n", rtype, fname, args);
    fprintf(fp, "   {\n");
    fprintf(fp, "    \n");
    fprintf(fp, "    switch (id)\n");
    fprintf(fp, "       {\n");

    for (id = 0; id < ne; id++)
        {td = tl + id;
	 if (td->type != NULL)
	    write_switch_clause(fp, td, t);};

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

/*                             PARSING ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* PARSE_TMPL - read and parse the next template from FP */

static template *parse_tmpl(FILE *fp, int ne, typdes *tl, int *flo)
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
       t = make_template(proto, ne, tl, nl, sa, tmn, tmx, lo);
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

#include "template-type.c"
#include "template-vararg.c"
#include "template-coerce.c"
#include "template-render.c"
#include "template-desc.c"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int i, ln, ne, nt, rv, tgt, fl, pfl;
    int dfl[3];
    char tmpf[BFLRG], tytab[BFLRG];
    char *inf, *outf;
    FILE *fi, *fo;
    template *t, *tm[NMAX];
    typdes *tl;

    rv = 0;

    tytab[0] = '\0';

    dfl[0] = 0;    /* C header for desc mode */
    dfl[1] = 0;    /* HTML for desc mode */
    dfl[2] = 0;    /* show alias types */

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
	 else if (strcmp(v[i], "-gh") == 0)
	    tgt = 2;
	 else if (strcmp(v[i], "-gc") == 0)
	    tgt = 3;
	 else if (strcmp(v[i], "-desc") == 0)
	    {tgt = 4;
             fl  = atol(v[++i]);
	     if (fl & 1)
	        dfl[0] = TRUE;
	     else if (fl & 2)
	        dfl[1] = TRUE;
	     else if (fl & 4)
	        dfl[2] = TRUE;}
	 else
	    inf = v[i];};

    if (make_type_table(tytab) == FALSE)
       {printf("No type table file specified - exiting\n");
	return(1);};

    if (inf != NULL)
       fi = fopen_safe(inf, "r+");

    tl = type_table(NULL);
    for (ne = 0; tl[ne].type != NULL; ne++);

    pfl = TRUE;
    if (tgt == 2)
       {nt  = 0;
	inf = "gc_type.h";}

    else if (tgt == 3)
       {nt  = 0;
	inf = "gc_type.c";}

    else if (tgt == 4)
       {nt  = 0;
	inf = "pdform.h";}

    else if ((inf == NULL) || (fi == NULL))
       {printf("No input file specified - exiting\n");
	return(1);}

    else
       {ln = 1;

        memset(tm, 0, sizeof(tm));

	for (nt = 0; nt < NMAX; nt++)
	    {t = parse_tmpl(fi, ne, tl, &ln);
	     if (t == NULL)
	        break;
	     tm[nt] = t;};};

    if (outf != NULL)
       {snprintf(tmpf, BFLRG, "%s.%ld", outf, (long) getpid());

/* write a temporary file then rename it at the end
 * so that if the file currently exists it can be used
 * and there will be no gap when it does not exist or is empty
 */
	fo = fopen_safe(tmpf, "w");}
    else
       fo = stdout;

    write_header(fo, inf, pfl);

    switch (tgt)

/* write template as an array of functions */
       {case 0 :
	     for (i = 0; i < nt; i++)
	         write_fnc(fo, ne, tl, tm, i, nt);

	     for (i = 0; i < nt; i++)
	         write_fnc_decls(fo, ne, tl, tm, i, nt);

	     break;

/* write template as a switch statement */
	case 1 :
	     for (i = 0; i < nt; i++)
	         write_switch(fo, ne, tl, tm[i]);

	     break;

/* write special type handler declarations in gc-type.h */
	case 2 :
	     def_type_manager(fo, ne, tl);
	     write_va_arg(fo, ne, tl);
	     write_coerce(fo, ne, tl, FALSE);
	     write_str(fo, ne, tl, FALSE);
	     break;

/* write special type handler definitions in gc-type.c */
	case 3 :
	     write_coerce(fo, ne, tl, TRUE);
	     write_str(fo, ne, tl, TRUE);
	     break;

/* write special template for binary type format specifications in pdform.h */
	case 4 :
	     write_desc(fo, ne, tl, dfl);
	     break;};

    for (i = 0; i < nt; i++)
	free_template(tm[i]);

    write_trailer(fo, inf, pfl);

    if (outf != NULL)
       {fclose_safe(fo);
	rename(tmpf, outf);};

    log_safe("dump", 0, NULL, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
