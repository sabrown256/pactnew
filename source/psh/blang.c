/*
 * BLANG.C - generate language bindings and documentation
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#include "common.h"
#include "libpsh.c"

typedef struct s_fdecl fdecl;
typedef struct s_farg farg;

struct s_farg
   {char *arg;
    char type[MAXLINE];
    char name[MAXLINE];};

struct s_fdecl
   {char *proto;
    char type[MAXLINE];
    char name[MAXLINE];
    int na;
    char **args;
    farg *al;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IS_PTR - return TRUE if TYPE is a pointer */

int is_ptr(char *type)
   {int rv;

    rv = (strchr(type, '*') != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NO_ARGS - return TRUE if function takes no args */

int no_args(fdecl *dcl)
   {int rv;

    rv = ((dcl->na == 1) && (dcl->al[0].name[0] == '\0'));
/* (strcmp(dcl->al[0].type, "void") == 0) */

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SPLIT_DECL - split a declaration S of the form
 *            -    <type> ['*']* <name>
 *            - into proper type and name
 */

void split_decl(char *type, int nt, char *name, int nn, char *s)
   {int nb, nc;
    char *p;

    p  = trim(s, BOTH, " \t");
    nc = strcspn(p, " *\t");
    nb = strspn(p+nc, " *\t");

    nstrncpy(name, nn, p+nc+nb, -1);
    p[nc+nb] = '\0';

    nstrncpy(type, nt, trim(p, BACK, " \t"), -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROC_ARGS - process the arg list in the declaration DCL */

farg *proc_args(fdecl *dcl)
   {int i, na;
    char **args;
    farg *al;

    na   = dcl->na;
    args = dcl->args;
    
    al = MAKE_N(farg, na);

    for (i = 0; i < na; i++)
        {al[i].arg = args[i];
	 split_decl(al[i].type, MAXLINE, al[i].name, MAXLINE, al[i].arg);};

    dcl->al = al;

    return(al);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FREE_ARGS - release the arg list AL */

void free_args(farg *al)
   {

    FREE(al);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIND_PROTO - find the prototype for F in SPR
 *            - return it iff successful
 */

fdecl *find_proto(char **spr, char *f)
   {int ip, n, na, nt;
    char *sp, *pro, *cfn;
    char **sa, **ty, **args;
    fdecl *dcl;

    pro = NULL;

    for (ip = 0; spr[ip] != NULL; ip++)
        {sp = spr[ip];
	 if (blank_line(sp) == FALSE)
	    {if (strstr(sp, f) != NULL)
	        {pro = sp;
		 break;};};};

    if (pro == NULL)
       {printf("Error: no binding for '%s'\n", f);
	dcl = NULL;}

    else
       {dcl = MAKE(fdecl);
	dcl->proto = pro;

/* break up the prototype into the type/function name and args */
	sa = tokenize(pro, "()");
	for (n = 0; IS_NULL(sa[n]) == FALSE; n++);

/* get the return type */
        split_decl(dcl->type, MAXLINE, dcl->name, MAXLINE, sa[0]);
	
	ty = tokenize(sa[0], " \t");
	for (nt = 0; IS_NULL(ty[nt]) == FALSE; nt++);

/* get the function name */
	cfn = ty[--nt];

/* get the args */
	args = tokenize(sa[n-1], ",");
	for (na = 0; IS_NULL(args[na]) == FALSE; na++);
	dcl->na   = na;
	dcl->args = args;

	proc_args(dcl);

	free_strings(ty);
	free_strings(sa);};

    return(dcl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FREE_DECL - release the declaration DCL */

void free_decl(fdecl *dcl)
   {

    free_args(dcl->al);
    free_strings(dcl->args);
    FREE(dcl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CSEP - print C comment separator */

void csep(FILE *fp)
   {

    fprintf(fp, "/*--------------------------------------------------------------------------*/\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HSEP - print HTML comment separator */

void hsep(FILE *fp)
   {

    fprintf(fp, "<!-- -------------------------------------------------------------------- -->\n");

    return;}

/*--------------------------------------------------------------------------*/

/*                            FORTRAN ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* CF_TYPE - return Fortran type corresponding to C type T */

void cf_type(char *a, int nc, char *t)
   {

    if (strcmp(t, "char *") == 0)
       nstrncpy(a, nc, "string", -1);
    else if (is_ptr(t) == TRUE)
       nstrncpy(a, nc, "void *", -1);
    else if ((strncmp(t, "int", 3) == 0) ||
	     (strncmp(t, "long", 4) == 0) || 
	     (strncmp(t, "short", 5) == 0) || 
	     (strncmp(t, "BIGINT", 6) == 0))
       nstrncpy(a, nc, "integer", -1);
    else if (strncmp(t, "double", 6) == 0)
       nstrncpy(a, nc, "real*8", -1);
    else if (strncmp(t, "float", 6) == 0)
       nstrncpy(a, nc, "real*4", -1);
    else if (strncmp(t, "void", 4) == 0)
       nstrncpy(a, nc, "", -1);
    else
       nstrncpy(a, nc, "unknown", -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FC_TYPE - return C type corresponding to FORTRAN type T */

void fc_type(char *a, int nc, char *t)
   {

    if (strcmp(t, "char *") == 0)
       nstrncpy(a, nc, "F77_string", -1);
    else if (is_ptr(t) == TRUE)
       nstrncpy(a, nc, "void *", -1);
    else if ((strncmp(t, "int", 3) == 0) ||
	     (strncmp(t, "long", 4) == 0) || 
	     (strncmp(t, "short", 5) == 0) || 
	     (strncmp(t, "BIGINT", 6) == 0))
       nstrncpy(a, nc, "FIXNUM", -1);
    else if (strncmp(t, "double", 6) == 0)
       nstrncpy(a, nc, "double", -1);
    else if (strncmp(t, "float", 6) == 0)
       nstrncpy(a, nc, "float", -1);
    else if (strncmp(t, "void", 4) == 0)
       nstrncpy(a, nc, "void", -1);
    else
       nstrncpy(a, nc, "unknown", -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FC_DECL_LIST - render the arg list of DCL into A for the
 *              - Fortran callable C wrapper
 */

void fc_decl_list(char *a, int nc, fdecl *dcl)
   {int i, na;
    char lt[MAXLINE];
    farg *al;

    na = dcl->na;
    al = dcl->al;

    a[0] = '\0';
    if ((na == 0) || (no_args(dcl) == TRUE))
       nstrcat(a, MAXLINE, "void");
    else
       {for (i = 0; i < na; i++)
	    {fc_type(lt, MAXLINE, al[i].type);
	     vstrcat(a, MAXLINE, "%s *p%s, ", lt, al[i].name);};
        a[strlen(a) - 2] = '\0';};

    nstrncpy(a, nc, subst(a, "* ", "*", -1), -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FC_CALL_LIST - render the arg list of DCL into A for the
 *              - C function call
 */

void fc_call_list(char *a, int nc, fdecl *dcl)
   {int i, na, voida;
    char lt[MAXLINE];
    farg *al;

    na = dcl->na;
    al = dcl->al;

    voida = no_args(dcl);

    a[0] = '\0';
    if (voida == FALSE)
       {for (i = 0; i < na; i++)
	    {fc_type(lt, MAXLINE, al[i].type);
	     vstrcat(a, MAXLINE, "_l%s, ", al[i].name);};

	a[strlen(a) - 2] = '\0';};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* F_PROTO_LIST - render the arg list of DCL into A for the
 *              - Fortran callable C wrapper
 */

void f_proto_list(char *a, int nc, fdecl *dcl)
   {int i, na;
    char t[MAXLINE];
    farg *al;

    na = dcl->na;
    al = dcl->al;

    a[0] = '\0';
    if (na != 0)
       {for (i = 0; i < na; i++)
	    {cf_type(t, MAXLINE, al[i].type);
	     vstrcat(a, MAXLINE, "%s %s, ", t, al[i].name);};
        a[strlen(a) - 2] = '\0';};

    nstrncpy(a, nc, subst(a, "* ", "*", -1), -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_FORTRAN - initialize Fortran file */

FILE *init_fortran(char *pck)
   {FILE *fp;

    fp = open_file("w", "gf-%s.c", pck);

    fprintf(fp, "\n");
    fprintf(fp, "#include \"%s_int.h\"\n", pck);
    fprintf(fp, "\n");

    csep(fp);

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP_FORTRAN - wrap C function PRO in FORTRAN callable function
 *              - using name FFN
 */

void wrap_fortran(FILE *fp, fdecl *dcl, char *ffn)
   {int i, na, nv, voidf, voida;
    char ufn[MAXLINE], a[MAXLINE], rt[MAXLINE];
    farg *al;

    nstrncpy(ufn, MAXLINE, ffn, -1);
    upcase(ufn);

    na = dcl->na;
    al = dcl->al;
    voidf = (strcmp(dcl->type, "void") == 0);
    voida = no_args(dcl);

    fc_type(rt, MAXLINE, dcl->type);
    fc_decl_list(a, MAXLINE, dcl);

    csep(fp);
    fprintf(fp, "\n");

    fprintf(fp, "%s FF_ID(%s, %s)(%s)\n", rt, ffn, ufn, a);

/* local variable declarations */
    nv = 0;
    for (i = 0; i <= na; i++)
        {if ((voida == TRUE) && (i == 0))
	    continue;

	 if (nv == 0)
	    fprintf(fp, "   {");
	 else
	    fprintf(fp, "    ");

/* variable for return value */
	 if ((i == na) && (voidf == FALSE))
	    fprintf(fp, "%s _rv;\n", rt);

/* local vars */
	 else if (al[i].name[0] != '\0')
	    {fprintf(fp, "%s _l%s;\n", al[i].type, al[i].name);
	     nv++;};};

    fprintf(fp, "\n");

/* local variable assignments */
    for (i = 0; i < na; i++)
        {if (al[i].name[0] != '\0')
	    fprintf(fp, "    _l%s = (%s) *p%s;\n",
		     al[i].name, al[i].type, al[i].name);};

    fprintf(fp, "\n");

/* function call */
    fc_call_list(a, MAXLINE, dcl);
    if (voidf == FALSE)
       fprintf(fp, "    _rv = %s(%s);\n", dcl->name, a);
    else
       fprintf(fp, "    %s(%s);\n", dcl->name, a);

    fprintf(fp, "\n");

    if (voidf == FALSE)
       fprintf(fp, "    return(_rv);}\n");
    else
       fprintf(fp, "    return;}\n");

    fprintf(fp, "\n");
    csep(fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_FORTRAN - generate Fortran bindings from SPR and SBI
 *              - return TRUE iff successful
 */

int bind_fortran(FILE *fp, char **spr, char **sbi)
   {int ib, rv;
    char t[MAXLINE];
    char *sb, **ta;
    char *cfn, *ffn;
    fdecl *dcl;

    rv = TRUE;

    for (ib = 0; sbi[ib] != NULL; ib++)
        {sb = sbi[ib];
	 if (blank_line(sb) == FALSE)
	    {nstrncpy(t, MAXLINE, sb, -1);
	     ta = tokenize(t, " \t");
	     cfn = ta[0];
	     ffn = ta[1];
             dcl = find_proto(spr, cfn);
	     if (dcl != NULL)
	        {wrap_fortran(fp, dcl, ffn);
		 free_decl(dcl);};

	     free_strings(ta);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_FORTRAN - finalize Fortran file */

void fin_fortran(FILE *fp)
   {

    csep(fp);
    fclose(fp);

    return;}

/*--------------------------------------------------------------------------*/

/*                             SCHEME ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* CS_TYPE - return "Scheme" type corresponding to C type T */

void cs_type(char *a, int nc, char *t)
   {

    if (strcmp(t, "char *") == 0)
       nstrncpy(a, nc, "SC_STRING_I", -1);
    else if (strncmp(t, "int", 3) == 0)
       nstrncpy(a, nc, "SC_INTEGER_I", -1);
    else if (strncmp(t, "long", 4) == 0)
       nstrncpy(a, nc, "SC_LONG_I", -1);
    else if (strncmp(t, "short", 5) == 0)
       nstrncpy(a, nc, "SC_SHORT_I", -1);
    else if (strncmp(t, "SC_BIGINT_I", 6) == 0)
       nstrncpy(a, nc, "SC_BIGINT_I", -1);
    else if (strncmp(t, "double", 6) == 0)
       nstrncpy(a, nc, "SC_DOUBLE_I", -1);
    else if (strncmp(t, "float", 6) == 0)
       nstrncpy(a, nc, "SC_FLOAT_I", -1);
    else
       nstrncpy(a, nc, "unknown", -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CS_DECL_LIST - render the arg list of DCL into A for the
 *              - Scheme to C value extaction
 */

void cs_decl_list(char *a, int nc, fdecl *dcl)
   {int i, na;
    farg *al;

    na = dcl->na;
    al = dcl->al;

    a[0] = '\0';
    for (i = 0; i < na; i++)
        vstrcat(a, MAXLINE, "            %s, &%s,\n",
		al[i].type, al[i].name);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* S_PROTO_LIST - render the arg list of DCL into A for the
 *              - Scheme prototype
 */

void s_proto_list(char *a, int nc, fdecl *dcl)
   {int i, na;
    farg *al;

    na = dcl->na;
    al = dcl->al;

    a[0] = '\0';
    if (na != 0)
       {for (i = 0; i < na; i++)
	    vstrcat(a, MAXLINE, " %s", al[i].name);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_SCHEME - initialize Scheme file */

FILE *init_scheme(char *pck)
   {FILE *fp;

    fp = open_file("w", "gs-%s.c", pck);

    fprintf(fp, "\n");
    fprintf(fp, "#include \"sx_int.h\"\n");
    fprintf(fp, "\n");

    csep(fp);

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP_SCHEME - wrap C function DCL in SCHEME callable function
 *             - using name FFN
 */

void wrap_scheme(FILE *fp, fdecl *dcl, char *ffn)
   {int na;
    char ufn[MAXLINE], a[MAXLINE];

    na = dcl->na;

    nstrncpy(ufn, MAXLINE, ffn, -1);
    upcase(ufn);

    cs_decl_list(a, MAXLINE, dcl);

    csep(fp);
    fprintf(fp, "\n");

    fprintf(fp, "static object *");
    fprintf(fp, "_SXI_%s", dcl->name);
    if (na == 0)
       fprintf(fp, "(void)");
    else
       fprintf(fp, "(object *argl)");
    fprintf(fp, "\n");

    fprintf(fp, "   {object *o;\n");
    fprintf(fp, "\n");

    if (na != 0)
       {fprintf(fp, "    SS_args(argl,\n");
	fprintf(fp, "%s", a);
	fprintf(fp, "            0);\n");
	fprintf(fp, "\n");};

    fprintf(fp, "    %s();\n", dcl->name);
    fprintf(fp, "\n");

    fprintf(fp, "    o = SS_null;\n");
    fprintf(fp, "\n");

    fprintf(fp, "    return(o);}\n");

    fprintf(fp, "\n");
    csep(fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_SCHEME - generate Scheme bindings from SPR and SBI
 *             - return TRUE iff successful
 */

int bind_scheme(FILE *fp, char **spr, char **sbi)
   {int ib, rv;
    char t[MAXLINE];
    char *sb, **ta;
    char *cfn, *ffn;
    fdecl *dcl;

    rv = TRUE;

    for (ib = 0; sbi[ib] != NULL; ib++)
        {sb = sbi[ib];
	 if (blank_line(sb) == FALSE)
	    {nstrncpy(t, MAXLINE, sb, -1);
	     ta = tokenize(t, " \t");
	     cfn = ta[0];
	     ffn = ta[2];
             dcl = find_proto(spr, cfn);
	     if (dcl == NULL)
	        printf("Error: no binding for '%s'\n", cfn);
	     else
	        {wrap_scheme(fp, dcl, ffn);
		 free_decl(dcl);};

	     free_strings(ta);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_SCHEME - finalize Scheme file */

void fin_scheme(FILE *fp)
   {

    csep(fp);
    fclose(fp);

    return;}

/*--------------------------------------------------------------------------*/

/*                             PYTHON ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* P_PROTO_LIST - render the arg list of DCL into A for the
 *              - Python callable C wrapper
 */

void p_proto_list(char *a, int nc, fdecl *dcl)
   {int i, na;
    farg *al;

    na = dcl->na;
    al = dcl->al;

    a[0] = '\0';
    if (na != 0)
       {for (i = 0; i < na; i++)
	    vstrcat(a, MAXLINE, "%s %s, ", al[i].type, al[i].name);
        a[strlen(a) - 2] = '\0';};

    nstrncpy(a, nc, subst(a, "* ", "*", -1), -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_PYTHON - initialize Python file */

FILE *init_python(char *pck)
   {FILE *fp;

    fp = open_file("w", "gp-%s.c", pck);

    fprintf(fp, "\n");
    fprintf(fp, "#include \"sx_int.h\"\n");
    fprintf(fp, "\n");

    csep(fp);

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP_PYTHON - wrap C function DCL in PYTHON callable function
 *             - using name FFN
 */

void wrap_python(FILE *fp, fdecl *dcl, char *ffn)
   {char ufn[MAXLINE];

    nstrncpy(ufn, MAXLINE, ffn, -1);
    upcase(ufn);

    csep(fp);
    fprintf(fp, "\n");

    fprintf(fp, "\n");
    csep(fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_PYTHON - generate Python bindings from SPR and SBI
 *             - return TRUE iff successful
 */

int bind_python(FILE *fp, char **spr, char **sbi)
   {int ib, rv;
    char t[MAXLINE];
    char *sb, **ta;
    char *cfn, *ffn;
    fdecl *dcl;

    rv = TRUE;

    for (ib = 0; sbi[ib] != NULL; ib++)
        {sb = sbi[ib];
	 if (blank_line(sb) == FALSE)
	    {nstrncpy(t, MAXLINE, sb, -1);
	     ta = tokenize(t, " \t");
	     cfn = ta[0];
	     ffn = ta[3];
             dcl = find_proto(spr, cfn);
	     if (dcl == NULL)
	        printf("Error: no binding for '%s'\n", cfn);
	     else
	        {wrap_python(fp, dcl, ffn);
		 free_decl(dcl);};

	     free_strings(ta);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_PYTHON - finalize Python file */

void fin_python(FILE *fp)
   {

    csep(fp);
    fclose(fp);

    return;}

/*--------------------------------------------------------------------------*/

/*                              DOC ROUTINES                                */

/*--------------------------------------------------------------------------*/

/* INIT_DOC - initialize Doc file */

FILE *init_doc(char *pck)
   {FILE *fp;

    fp = open_file("w", "gh-%s.html", pck);

    fprintf(fp, "\n");

    hsep(fp);

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP_DOC - wrap C function DCL in DOC callable function
 *          - using names in FN
 */

void wrap_doc(FILE *fp, fdecl *dcl, char **fn)
   {char ufn[MAXLINE], lfn[MAXLINE];
    char af[MAXLINE], as[MAXLINE], ap[MAXLINE];
    char fty[MAXLINE];

    nstrncpy(ufn, MAXLINE, fn[0], -1);
    upcase(ufn);

    nstrncpy(lfn, MAXLINE, fn[0], -1);
    upcase(lfn);

    cf_type(fty, MAXLINE, dcl->type);

    f_proto_list(af, MAXLINE, dcl);
    s_proto_list(as, MAXLINE, dcl);
    p_proto_list(ap, MAXLINE, dcl);

    hsep(fp);
    fprintf(fp, "\n");

    fprintf(fp, "<a name=\"%s\"><h2>%s</h2></a>\n", lfn, ufn);
    fprintf(fp, "\n");
    fprintf(fp, "<p>\n");
    fprintf(fp, "<pre>\n");

    fprintf(fp, "<i>C Binding: </i> %s\n", dcl->proto);

    if (strcmp(fn[0], "none") == 0)
       fprintf(fp, "<i>Fortran Binding: </i> none\n");
    else
       fprintf(fp, "<i>Fortran Binding: </i> %s %s(%s)\n", fty, fn[0], af);

    if (strcmp(fn[1], "none") == 0)
       fprintf(fp, "<i>SX Binding: </i> none\n");
    else
       fprintf(fp, "<i>SX Binding: </i> (%s%s)\n", fn[1], as);

    if (strcmp(fn[2], "none") == 0)
       fprintf(fp, "<i>Python Binding: </i> none\n");
    else
       fprintf(fp, "<i>Python Binding: </i> pact.%s(%s)\n", fn[2], ap);

    fprintf(fp, "</pre>\n");
    fprintf(fp, "<p>\n");

    fprintf(fp, "\n");
    hsep(fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_DOC - generate Doc bindings from SPR and SBI
 *          - return TRUE iff successful
 */

int bind_doc(FILE *fp, char **spr, char **sbi)
   {int ib, rv;
    char t[MAXLINE];
    char *sb, **ta;
    char *cfn;
    fdecl *dcl;

    rv = TRUE;

    for (ib = 0; sbi[ib] != NULL; ib++)
        {sb = sbi[ib];
	 if (blank_line(sb) == FALSE)
	    {nstrncpy(t, MAXLINE, sb, -1);
	     ta = tokenize(t, " \t");
	     cfn = ta[0];
             dcl = find_proto(spr, cfn);
	     if (dcl == NULL)
	        printf("Error: no binding for '%s'\n", cfn);
	     else
	        {wrap_doc(fp, dcl, ta+1);
		 free_decl(dcl);};

	     free_strings(ta);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_DOC - finalize Doc file */

void fin_doc(FILE *fp)
   {

    hsep(fp);
    fclose(fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BLANG - control the generation of language binding
 *       - return TRUE iff successful
 */

int blang(char *pck, char *fpr, char *fbi)
   {int rv;
    char **spr, **sbi;
    FILE *ff, *fs, *fp, *fd;

    if ((IS_NULL(fpr) == TRUE) || (IS_NULL(fbi) == TRUE))
       rv = FALSE;

    else
       {rv = TRUE;

	spr = file_text(fpr);
	sbi = file_text(fbi);

	ff = init_fortran(pck);
	rv &= bind_fortran(ff, spr, sbi);
	fin_fortran(ff);

	fs = init_scheme(pck);
	rv &= bind_scheme(fs, spr, sbi);
	fin_scheme(fs);

	fp = init_python(pck);
	rv &= bind_python(fp, spr, sbi);
	fin_python(fp);

	fd = init_doc(pck);
	rv &= bind_doc(fd, spr, sbi);
	fin_doc(fd);

	free_strings(spr);
	free_strings(sbi);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it out here */

int main(int c, char **v)
   {int i, rv;
    char pck[MAXLINE];
    char *fpr, *fbi;

    fpr = "";
    fbi = "";

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-h") == 0)
            {printf("Usage: blang [-h] <prototypes> <bindings>\n");
             printf("   h          this help message\n");
             printf("   <prototypes>    file containing C function prototypes\n");
             printf("   <binding>       file specifying function bindings\n");
             printf("\n");}
	 else
	    {if (IS_NULL(fpr) == TRUE)
	        fpr = v[i];
	     else if (IS_NULL(fbi) == TRUE)
	        fbi = v[i];};};

    snprintf(pck, MAXLINE, "%s", path_base(path_tail(fbi)));

    rv = blang(pck, fpr, fbi);
    rv = (rv != TRUE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

