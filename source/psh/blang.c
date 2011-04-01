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
   {int fptr;
    char *arg;
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

/* CONCATENATE - concatenate tokens SA to string S */

char *concatenate(char *s, int nc, char **sa, char *dlm)
   {int i;

    s[0] = '\0';
    for (i = 0; sa[i] != NULL; i++)
        vstrcat(s, nc, "%s%s", sa[i], dlm);

    s[strlen(s) - strlen(dlm)] = '\0';

    return(s);}

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
 *            - return TRUE if S is a function pointer
 */

int split_decl(char *type, int nt, char *name, int nn, char *s)
   {int nb, nc, rv;
    char t[MAXLINE];
    char *p, *pn;

    nstrncpy(t, MAXLINE, s, -1);

    p  = trim(strtok(t, "),"), BOTH, " \t");
    nc = strcspn(p, " *\t");
    nb = strspn(p+nc, " *\t");

    pn = p + nc + nb;

/* handle function pointer case which would be like '(*f' here */
    if (*pn == '(')
       {nstrncpy(name, nn, pn+2, -1);

	*pn = '\0';

	strcpy(type, "PFInt");

	rv = TRUE;}

/* handle other args */
    else
       {nstrncpy(name, nn, pn, -1);

	*pn = '\0';

	nstrncpy(type, nt, trim(p, BACK, " \t"), -1);

	rv = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SPLIT_ARGS - split an arg list into individual args */

char **split_args(char *args)
   {char *cc, *cp, *p, *pa, **al;

    al = NULL;
    for (pa = args; IS_NULL(pa) == FALSE; )
        {cc = strchr(pa, ',');
	 cp = strchr(pa, '(');
	 if ((cc == NULL) && (cp == NULL))
	    {al = lst_push(al, pa);
	     pa = NULL;}
	 else if (cp == NULL)
	    {*cc++ = '\0';
	     al = lst_push(al, pa);
	     pa = trim(cc, FRONT, " \t");}
	 else if (cc == NULL)
	    {al = lst_push(al, pa);
	     pa = NULL;}
	 else if ((cc - cp) < 0)
	    {*cc++ = '\0';
	     al = lst_push(al, pa);
	     pa = trim(cc, FRONT, " \t");}
	 else
	    {p = strchr(cp, ')') + 1;
	     p = strchr(p, ')') + 1;
	     *p++ = '\0';
	     al = lst_push(al, pa);
	     pa = trim(p, FRONT, " \t");};};

    return(al);}

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
        {al[i].arg  = args[i];
	 al[i].fptr = split_decl(al[i].type, MAXLINE,
				 al[i].name, MAXLINE,
				 al[i].arg);};

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
   {int ip, na, nt;
    char pf[MAXLINE];
    char *p, *pa, *sp, *pro, *cfn;
    char **ty, **args;
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
	nstrncpy(pf, MAXLINE, pro, -1);
	p = strchr(pf, '(');
	*p++ = '\0';
	pa = p;
	LAST_CHAR(pa) = '\0';

/* get the return type */
        split_decl(dcl->type, MAXLINE, dcl->name, MAXLINE, pf);
	
	ty = tokenize(pf, " \t");
	for (nt = 0; IS_NULL(ty[nt]) == FALSE; nt++);

/* get the function name */
	cfn = ty[--nt];
	ASSERT(cfn != NULL);

/* get the args */
	args = split_args(pa);
	for (na = 0; IS_NULL(args[na]) == FALSE; na++);
	dcl->na   = na;
	dcl->args = args;

	proc_args(dcl);

	free_strings(ty);};

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
	     (strncmp(t, "long long", 9) == 0))
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

    if ((strcmp(t, "char *") == 0) ||
	(strcmp(t, "F77_string") == 0))
       nstrncpy(a, nc, "F77_string", -1);
    else if ((is_ptr(t) == TRUE) || 
	     (strstr(t, "(*") != NULL))
       nstrncpy(a, nc, "void *", -1);
    else if (strncmp(t, "PF", 2) == 0)
       nstrncpy(a, nc, "PFInt", -1);
    else if ((strncmp(t, "int", 3) == 0) ||
	     (strncmp(t, "long", 4) == 0) || 
	     (strncmp(t, "short", 5) == 0) || 
	     (strncmp(t, "long long", 9) == 0) ||
	     (strncmp(t, "FIXNUM", 6) == 0))
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
	     if (al[i].fptr == TRUE)
	        vstrcat(a, MAXLINE, "%s p%s, ", lt, al[i].name);
	     else
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
    farg *al;

    na = dcl->na;
    al = dcl->al;

    voida = no_args(dcl);

    a[0] = '\0';
    if (voida == FALSE)
       {for (i = 0; i < na; i++)
	    vstrcat(a, MAXLINE, "_l%s, ", al[i].name);

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

    memmove(a, trim(subst(a, "* ", "*", -1), BOTH, " "), nc);

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
   {int i, na, nv, voidf, voida, rptr;
    char ufn[MAXLINE], a[MAXLINE], rt[MAXLINE], t[MAXLINE];
    farg *al;

    nstrncpy(ufn, MAXLINE, ffn, -1);
    upcase(ufn);

    na    = dcl->na;
    al    = dcl->al;
    voidf = (strcmp(dcl->type, "void") == 0);
    voida = no_args(dcl);

    fc_type(rt, MAXLINE, dcl->type);
    fc_decl_list(a, MAXLINE, dcl);

    rptr = is_ptr(rt);

    csep(fp);
    fprintf(fp, "\n");

    if (rptr == TRUE)
       snprintf(t, MAXLINE, "FIXNUM FF_ID(%s, %s)(%s)\n", ffn, ufn, a);
    else
       snprintf(t, MAXLINE, "%s FF_ID(%s, %s)(%s)\n", rt, ffn, ufn, a);
    fputs(subst(t, "* ", "*", -1), fp);

/* local variable declarations */
    nv = 0;
    for (i = 0; i <= na; i++)
        {if ((voida == TRUE) && (i == 0))
	    continue;

	 if (nv == 0)
	    fprintf(fp, "   {");
	 else
	    fprintf(fp, "    ");

	 t[0] = '\0';

/* variable for return value */
	 if ((i == na) && (voidf == FALSE))
	    {if (rptr == TRUE)
		snprintf(t, MAXLINE, "FIXNUM _rv;\n");
	      else
		snprintf(t, MAXLINE, "%s _rv;\n", rt);}

/* local vars */
	 else if (al[i].name[0] != '\0')
	    {snprintf(t, MAXLINE, "%s _l%s;\n", al[i].type, al[i].name);
	     nv++;};

	 if (IS_NULL(t) == FALSE)
	    fputs(subst(t, "* ", "*", -1), fp);};

    fprintf(fp, "\n");

/* local variable assignments */
    for (i = 0; i < na; i++)
        {if (al[i].name[0] != '\0')
	    {if (al[i].fptr == TRUE)
		fprintf(fp, "    _l%s = (%s) p%s;\n",
			al[i].name, al[i].type, al[i].name);
	     else
		fprintf(fp, "    _l%s = (%s) *p%s;\n",
			al[i].name, al[i].type, al[i].name);};};

    fprintf(fp, "\n");

/* function call */
    fc_call_list(a, MAXLINE, dcl);
    if (voidf == FALSE)
       {if (rptr == TRUE)
	   fprintf(fp, "    _rv = SC_ADD_POINTER(%s(%s));\n", dcl->name, a);
	else
	   fprintf(fp, "    _rv = %s(%s);\n", dcl->name, a);}
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

/* SC_TYPE - return C type corresponding to SCHEME type T */

void sc_type(char *a, int nc, char *t)
   {

    if ((strcmp(t, "char *") == 0) ||
	(strcmp(t, "F77_string") == 0))
       nstrncpy(a, nc, "F77_string", -1);
    else if ((is_ptr(t) == TRUE) ||
	     (strstr(t, "(*") != NULL))
       nstrncpy(a, nc, "void *", -1);
    else if (strncmp(t, "PF", 2) == 0)
       nstrncpy(a, nc, "PFInt", -1);
    else if ((strncmp(t, "int", 3) == 0) ||
	     (strncmp(t, "long", 4) == 0) || 
	     (strncmp(t, "short", 5) == 0) || 
	     (strncmp(t, "long long", 9) == 0) ||
	     (strncmp(t, "FIXNUM", 6) == 0))
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

/* CS_TYPE - return "Scheme" type corresponding to C type T */

void cs_type(char *a, int nc, char *t)
   {

    if (strcmp(t, "char *") == 0)
       nstrncpy(a, nc, "SC_STRING_I", -1);
    else if (strcmp(t, "char") == 0)
       nstrncpy(a, nc, "SC_CHAR_I", -1);

/* integer type */
    else if (strcmp(t, "int") == 0)
       nstrncpy(a, nc, "SC_INT_I", -1);
    else if (strcmp(t, "int *") == 0)
       nstrncpy(a, nc, "SC_INT_P_I", -1);
    else if (strcmp(t, "long") == 0)
       nstrncpy(a, nc, "SC_LONG_I", -1);
    else if (strcmp(t, "long *") == 0)
       nstrncpy(a, nc, "SC_LONG_P_I", -1);
    else if (strcmp(t, "short") == 0)
       nstrncpy(a, nc, "SC_SHORT_I", -1);
    else if (strcmp(t, "short *") == 0)
       nstrncpy(a, nc, "SC_SHORT_P_I", -1);

    else if (strcmp(t, "long long") == 0)
       nstrncpy(a, nc, "SC_LONG_LONG_I", -1);
    else if (strcmp(t, "long long *") == 0)
       nstrncpy(a, nc, "SC_LONG_LONG_P_I", -1);

/* floating point types */
    else if (strcmp(t, "double") == 0)
       nstrncpy(a, nc, "SC_DOUBLE_I", -1);
    else if (strcmp(t, "double *") == 0)
       nstrncpy(a, nc, "SC_DOUBLE_P_I", -1);
    else if (strcmp(t, "float") == 0)
       nstrncpy(a, nc, "SC_FLOAT_I", -1);
    else if (strcmp(t, "float *") == 0)
       nstrncpy(a, nc, "SC_FLOAT_P_I", -1);

/* structured types */
    else if (strcmp(t, "pcons") == 0)
       nstrncpy(a, nc, "SC_PCONS_I", -1);
    else if (strcmp(t, "pcons *") == 0)
       nstrncpy(a, nc, "SC_PCONS_P_I", -1);
    else if (strcmp(t, "FILE *") == 0)
       nstrncpy(a, nc, "SC_FILE_I", -1);
    else if (strcmp(t, "PROCESS *") == 0)
       nstrncpy(a, nc, "SC_PROCESS_I", -1);

/* others */
    else if (strcmp(t, "void *") == 0)
       nstrncpy(a, nc, "SC_VOID_I", -1);
    else if ((strchr(t, '*') != NULL) ||
	     (strstr(t, "(*") != NULL) ||
	     (strstr(t, "PF") != NULL))
       nstrncpy(a, nc, "SC_POINTER_I", -1);

    else
       nstrncpy(a, nc, "SC_UNKNOWN_I", -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CS_DECL_LIST - render the arg list of DCL into A for the
 *              - Scheme to C value extaction
 */

void cs_decl_list(char *a, int nc, fdecl *dcl)
   {int i, na;
    char ty[MAXLINE];
    farg *al;

    na = dcl->na;
    al = dcl->al;

    a[0] = '\0';
    for (i = 0; i < na; i++)
        {cs_type(ty, MAXLINE, al[i].type);
	 vstrcat(a, MAXLINE, "            %s, &_l%s,\n",
		 ty, al[i].name);};

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

    memmove(a, trim(a, BOTH, " "), nc);

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

char **wrap_scheme(FILE *fp, char **fl, fdecl *dcl, char *ffn, char **com)
   {int i, na, nv, voidf, voida;
    char ufn[MAXLINE], a[MAXLINE], rt[MAXLINE], t[MAXLINE];
    farg *al;

    na    = dcl->na;
    al    = dcl->al;
    voidf = (strcmp(dcl->type, "void") == 0);
    voida = no_args(dcl);

    nstrncpy(ufn, MAXLINE, ffn, -1);
    upcase(ufn);

    sc_type(rt, MAXLINE, dcl->type);

    csep(fp);
    fprintf(fp, "\n");

    fprintf(fp, "static object *");
    fprintf(fp, "_SXI_%s", dcl->name);
    if (na == 0)
       fprintf(fp, "(void)");
    else
       fprintf(fp, "(object *argl)");
    fprintf(fp, "\n");

/* local variable declarations */
    nv = 0;
    for (i = 0; i <= na; i++)
        {if ((voida == TRUE) && (i == 0))
	    continue;

	 if (nv == 0)
	    fprintf(fp, "   {");
	 else
	    fprintf(fp, "    ");

	 t[0] = '\0';

/* variable for return value */
	 if (i == na)
	    {if (voidf == FALSE)
		snprintf(t, MAXLINE, "%s _rv;\n    ", rt);

	     nstrcat(t, MAXLINE, "object *_lo;\n");}

/* local vars */
	 else if (al[i].name[0] != '\0')
	    {snprintf(t, MAXLINE, "%s _l%s;\n", al[i].type, al[i].name);
	     nv++;};

	 if (IS_NULL(t) == FALSE)
	    fputs(subst(t, "* ", "*", -1), fp);};

    fprintf(fp, "\n");

    if (voida == FALSE)
       {cs_decl_list(a, MAXLINE, dcl);
        fprintf(fp, "    SS_args(argl,\n");
	fprintf(fp, "%s", a);
	fprintf(fp, "            0);\n");
	fprintf(fp, "\n");};

/* function call */
    fc_call_list(a, MAXLINE, dcl);
    if (voidf == FALSE)
       fprintf(fp, "    _rv = %s(%s);\n", dcl->name, a);
    else
       fprintf(fp, "    %s(%s);\n", dcl->name, a);

    fprintf(fp, "    _lo = SS_null;\n");
    fprintf(fp, "\n");

    fprintf(fp, "    return(_lo);}\n");

    fprintf(fp, "\n");
    csep(fp);

/* add the installation of the function */
    concatenate(t, MAXLINE, com, " ");
    snprintf(a, MAXLINE, 
	     "    SS_install(\"%s\",\n               \"%s\",\n               SS_nargs,\n               _SXI_%s, SS_PR_PROC);\n\n",
	     ffn, t, dcl->name);
    fl = lst_add(fl, a);

    return(fl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_SCHEME - generate Scheme bindings from SPR and SBI
 *             - return TRUE iff successful
 */

int bind_scheme(FILE *fp, char *pck, char **spr, char **sbi)
   {int i, ib, rv;
    char t[MAXLINE];
    char *sb, **ta, **fl;
    char *cfn, *ffn;
    fdecl *dcl;

    rv = TRUE;
    fl = NULL;

    for (ib = 0; sbi[ib] != NULL; ib++)
        {sb = sbi[ib];
	 if (blank_line(sb) == FALSE)
	    {nstrncpy(t, MAXLINE, sb, -1);
	     ta = tokenize(t, " \t");
	     cfn = ta[0];
	     ffn = ta[2];
             dcl = find_proto(spr, cfn);
	     if (dcl != NULL)
	        {fl = wrap_scheme(fp, fl, dcl, ffn, ta+4);
		 free_decl(dcl);};

	     free_strings(ta);};};

/* write the routine to install the bindings */
    if (fl != NULL)
       {csep(fp);

	fprintf(fp, "\n");
	fprintf(fp, "void SX_install_%s_bindings(void)\n", pck);
	fprintf(fp, "   {\n");
	fprintf(fp, "\n");

	for (i = 0; fl[i] != NULL; i++)
	    fputs(fl[i], fp);

	free_strings(fl);

	fprintf(fp, "   return;}\n");
	fprintf(fp, "\n");
	csep(fp);};

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

    memmove(a, trim(subst(a, "* ", "*", -1), BOTH, " "), nc);

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
	     if (dcl != NULL)
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

void wrap_doc(FILE *fp, fdecl *dcl, char **fn, char **com)
   {int voidf;
    char ufn[MAXLINE], lfn[MAXLINE];
    char af[MAXLINE], as[MAXLINE], ap[MAXLINE];
    char fty[MAXLINE], t[MAXLINE];

    voidf = (strcmp(dcl->type, "void") == 0);

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

    fprintf(fp, "<i>C Binding: </i>       %s\n", dcl->proto);

    if (strcmp(fn[0], "none") == 0)
       fprintf(fp, "<i>Fortran Binding: </i>    none\n");
    else if (voidf == TRUE)
       fprintf(fp, "<i>Fortran Binding: </i> %s(%s)\n", fn[0], af);
    else
       fprintf(fp, "<i>Fortran Binding: </i> %s %s(%s)\n", fty, fn[0], af);

    if (strcmp(fn[1], "none") == 0)
       fprintf(fp, "<i>SX Binding: </i>         none\n");
    else if (IS_NULL(as) == TRUE)
       fprintf(fp, "<i>SX Binding: </i>      (%s)\n", fn[1]);
    else
       fprintf(fp, "<i>SX Binding: </i>      (%s %s)\n", fn[1], as);

    if (strcmp(fn[2], "none") == 0)
       fprintf(fp, "<i>Python Binding: </i>     none\n");
    else
       fprintf(fp, "<i>Python Binding: </i>  pact.%s(%s)\n", fn[2], ap);

    fprintf(fp, "</pre>\n");
    fprintf(fp, "<p>\n");

    concatenate(t, MAXLINE, com, " ");
    fputs(t, fp);
    fprintf(fp, "\n");
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
	     if (dcl != NULL)
	        {wrap_doc(fp, dcl, ta+1, ta+4);
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

	if (spr != NULL)
	   {ff = init_fortran(pck);
	    fs = init_scheme(pck);
	    fp = init_python(pck);
	    fd = init_doc(pck);

	    if (sbi != NULL)
	       {rv &= bind_fortran(ff, spr, sbi);
		rv &= bind_scheme(fs, pck, spr, sbi);
		rv &= bind_python(fp, spr, sbi);
		rv &= bind_doc(fd, spr, sbi);};

	    fin_fortran(ff);
	    fin_scheme(fs);
	    fin_python(fp);
	    fin_doc(fd);

	    free_strings(spr);
	    free_strings(sbi);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it out here */

int main(int c, char **v)
   {int i, rv;
    char pck[MAXLINE], msg[MAXLINE];
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
    snprintf(msg, MAXLINE, "%s bindings", pck);

    printf("      %s ", fill_string(msg, 25));

    rv = blang(pck, fpr, fbi);
    rv = (rv != TRUE);

    printf("done\n");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

