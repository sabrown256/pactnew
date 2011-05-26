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

enum e_langmode
   {MODE_C = 1, MODE_F, MODE_S, MODE_P };

typedef enum e_langmode langmode;

enum e_fparam
   {FP_ANY = -1, FP_VARARG, FP_FNC, FP_PTR, FP_STRUCT,
    FP_SCALAR, FP_ARRAY, FP_STRING, FP_VOID};

typedef enum e_fparam fparam;

typedef struct s_fdecl fdecl;
typedef struct s_farg farg;
typedef struct s_mtype mtype;

struct s_farg
   {fparam knd;
    char *arg;
    char type[MAXLINE];
    char name[MAXLINE];};

struct s_fdecl
   {int proc;                         /* TRUE iff processed */
    char *proto;
    char type[MAXLINE];
    char name[MAXLINE];
    int na;
    char **args;
    farg *al;};

struct s_mtype
   {char *cty;
    char *fty;
    char *sty;
    char *pty;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BERR - report error for blang */

static void berr(char *fmt, ...)
   {char s[LRG];
    static int first = TRUE;

    VA_START(fmt);
    VSNPRINTF(s, LRG, fmt);
    VA_END;

    if (first == TRUE)
       {first = FALSE;
	printf("\n");};

    printf("Error: %s\n", s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FEMIT - emit lines broken apropriately for Fortran */

static void femit(FILE *fp, char *t, char *trm)
   {int i, c, n, nc;
    char *pt;

    nc = strlen(t);

    for (pt = t; nc > 0; )
        {n = (pt == t) ? 73 : 51;
	 if (n > nc)
	    {if (pt == t)
		fprintf(fp, "%s %s\n", pt, trm);
	     else
		fprintf(fp, "                      %s %s\n", pt, trm);
	     break;}
	 else
	    {n = min(n, nc);
	     for (i = n-1; i >= 0; i--)
	         {if (strchr(", \t", pt[i]) != NULL)
		     {c = pt[i+1];
		      pt[i+1] = '\0';
		      if (pt == t)
			 fprintf(fp, "%s &\n", pt);
		      else
			 fprintf(fp, "                      %s &\n", pt);
		      pt[i+1] = c;
		      i++;
		      pt += i;
		      nc -= i;
		      break;};};
	     if (i < 0)
	        break;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONCATENATE - concatenate tokens SA to string S */

static char *concatenate(char *s, int nc, char **sa, char *dlm)
   {int i;

    s[0] = '\0';
    for (i = 0; sa[i] != NULL; i++)
        vstrcat(s, nc, "%s%s", sa[i], dlm);

    s[strlen(s) - strlen(dlm)] = '\0';

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IS_PTR - return TRUE if TYPE is a pointer */

static int is_ptr(char *type)
   {int rv;

    rv = (strchr(type, '*') != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NO_ARGS - return TRUE if function takes no args */

static int no_args(fdecl *dcl)
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

static int split_decl(char *type, int nt, char *name, int nn, char *s)
   {int nc, rv;
    char t[MAXLINE];
    char *p, *pn;

    nstrncpy(t, MAXLINE, s, -1);

    p = trim(strtok(t, "),"), BOTH, " \t");

    if (strcmp(p, "void") == 0)
       pn = p + 4;
    else
       {nc = strlen(p);
	for (pn = p + nc - 1; pn > p; pn--)
	    {if (strchr(" *\t", *pn) != NULL)
	        break;};
	if ((pn > p) && (pn[-1] == '('))
	   pn--;
	else
	   pn++;};

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

static char **split_args(char *args)
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

static farg *proc_args(fdecl *dcl)
   {int i, na;
    char **args;
    farg *al;

    na   = dcl->na;
    args = dcl->args;
    
    al = MAKE_N(farg, na);

    for (i = 0; i < na; i++)
        {al[i].arg  = args[i];
	 split_decl(al[i].type, MAXLINE,
		    al[i].name, MAXLINE,
		    al[i].arg);};

    dcl->al = al;

    return(al);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FREE_ARGS - release the arg list AL */

static void free_args(farg *al)
   {

    FREE(al);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIND_PROTO - find the prototype for F in SPR
 *            - return it iff successful
 */

static fdecl *find_proto(char **spr, char *f)
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

    if (strcmp(f, "derived") == 0)
       dcl = NULL;

    else if (pro == NULL)
       {berr("no binding for '%s'", f);
	dcl = NULL;}

    else
       {dcl = MAKE(fdecl);
	dcl->proc  = FALSE;
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

static void free_decl(fdecl *dcl)
   {

    free_args(dcl->al);
    free_strings(dcl->args);
    FREE(dcl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CSEP - print C comment separator */

static void csep(FILE *fp)
   {

    fprintf(fp, "/*--------------------------------------------------------------------------*/\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HSEP - print HTML comment separator */

static void hsep(FILE *fp)
   {

    fprintf(fp, "<!-- -------------------------------------------------------------------- -->\n");

    return;}

/*--------------------------------------------------------------------------*/

/*                           TYPE MAP ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* GET_TYPE_MAP - return the type map
 *              - if NA > 0 add that many elements to the map
 *              - return the number of elements in the map via PN
 *              - return the map via PMAP
 */

static void get_type_map(int *pn, mtype **pmap, int na)
   {static int n = 0;
    static int nx = 0;
    static mtype *map = NULL;

    if (map == NULL)
       {nx = 50;
        map = MAKE_N(mtype, nx);};

    if (n+na+1 >= nx)
       {nx += max(10, na);
        REMAKE(map, mtype, nx);};

    n += na;

    if (pn != NULL)
       *pn = n;

    if (pmap != NULL)
       *pmap = map;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ADD_TYPE - add a type to the map */

static void add_type(char *cty, char *fty, char *sty, char *pty)
   {int n;
    mtype *map;

    get_type_map(&n, &map, 1);

    n--;
    n = max(n, 0);

    map[n].cty = cty;
    map[n].fty = fty;
    map[n].sty = sty;
    map[n].pty = pty;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LOOKUP_TYPE - lookup and return a type from the map */

static char *lookup_type(char *ty, langmode ity, langmode oty)
   {int i, l, n;
    char *rv;
    mtype *map;

    get_type_map(&n, &map, 0);

    l = -1;
    switch (ity)
       {case MODE_C :
             for (i = 0; i < n; i++)
	         {if (strcmp(ty, map[i].cty) == 0)
		     {l = i;
		      break;};};
	     break;
        case MODE_F :
             for (i = 0; i < n; i++)
	         {if (strcmp(ty, map[i].fty) == 0)
		     {l = i;
		      break;};};
	     break;
        case MODE_S :
             for (i = 0; i < n; i++)
	         {if (strcmp(ty, map[i].sty) == 0)
		     {l = i;
		      break;};};
	     break;
        case MODE_P :
             for (i = 0; i < n; i++)
	         {if (strcmp(ty, map[i].pty) == 0)
		     {l = i;
		      break;};};
	     break;};

    rv = NULL;
    if (l != -1)
       {switch (oty)
	   {case MODE_C :
                 rv = map[l].cty;
	         break;
	    case MODE_F :
                 rv = map[l].fty;
	         break;
	    case MODE_S :
                 rv = map[l].sty;
	         break;
	    case MODE_P :
                 rv = map[l].pty;
	         break;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_TYPES - initialize the type map */

static void init_types(void)
   {

    add_type("void",        "",           "",                 NULL);
    add_type("bool",        "logical",    "SC_BOOL_I",        NULL);
    add_type("char",        "character",  "SC_CHAR_I",        NULL);

/* fixed point types */
    add_type("short",       "integer*2",  "SC_SHORT_I",       NULL);
    add_type("int",         "integer",    "SC_INT_I",         NULL);
    add_type("long",        "integer*8",  "SC_LONG_I",        NULL);
    add_type("long long",   "integer*8",  "SC_LONG_LONG_I",   NULL);

/* fixed width fixed point types */
    add_type("int16_t",     "integer*2",  "SC_INT16_I",       NULL);
    add_type("int32_t",     "integer*4",  "SC_INT32_I",       NULL);
    add_type("int64_t",     "integer*8",  "SC_INT64_I",       NULL);

/* floating point types */
    add_type("float",       "real*4",     "SC_FLOAT_I",       NULL);
    add_type("double",      "real*8",     "SC_DOUBLE_I",      NULL);
    add_type("long double", "real*16",    "SC_LONG_DOUBLE_I", NULL);

/* complex types */
    add_type("float _Complex",       "complex*4",
	     "SC_FLOAT_COMPLEX_I", NULL);
    add_type("double _Complex",      "complex*8",
	     "SC_DOUBLE_COMPLEX_I", NULL);
    add_type("long double _Complex", "complex*16",
	     "SC_LONG_DOUBLE_COMPLEX_I", NULL);

    add_type("void *",        "C_PTR-A",     "SC_VOID_P_I",        NULL);
    add_type("bool *",        "logical-A",   "SC_BOOL_P_I",        NULL);
    add_type("char *",        "character-A", "SC_STRING_I",        NULL);
    add_type("int *",         "integer-A",   "SC_INT_P_I",         NULL);
    add_type("short *",       "integer-A",   "SC_SHORT_P_I",       NULL);
    add_type("long *",        "integer-A",   "SC_LONG_P_I",        NULL);
    add_type("long long *",   "integer-A",   "SC_LONG_LONG_P_I",   NULL);
    add_type("float *",       "real*4-A",    "SC_FLOAT_P_I",       NULL);
    add_type("double *",      "real*8-A",    "SC_DOUBLE_P_I",      NULL);
    add_type("long double *", "real*16-A",   "SC_LONG_DOUBLE_P_I", NULL);

    add_type("pcons",         "C_PTR-A",     "SC_PCONS_I",         NULL);
    add_type("pcons *",       "C_PTR-A",     "SC_PCONS_P_I",       NULL);
    add_type("FILE *",        "C_PTR-A",     "SC_FILE_I",          NULL);
    add_type("PROCESS *",     "C_PTR-A",     "SC_PROCESS_I",       NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ADD_DERIVED_TYPES - add derived types */

static void add_derived_types(char **sbi)
   {int ib;
    char s[MAXLINE];
    char *sb, **ta;

    for (ib = 0; sbi[ib] != NULL; ib++)
        {sb = sbi[ib];
	 if (blank_line(sb) == FALSE)
	    {if (strncmp(sb, "derived ", 8) == 0)
		 {nstrncpy(s, MAXLINE, sb, -1);
		  ta = tokenize(s, " \t");
		  add_type(ta[1], ta[2], ta[3], ta[4]);
		  FREE(ta[0]);
		  FREE(ta);};};};

    return;}

/*--------------------------------------------------------------------------*/

/*                            FORTRAN ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* DEREF - dereference a pointer S
 *       - place result in D
 */

static char *deref(char *d, int nc, char *s)
   {

    nstrncpy(d, nc, s, -1);
    LAST_CHAR(d) = '\0';

    memmove(d, trim(d, BOTH, " \t"), nc);

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CF_TYPE - return Fortran type corresponding to C type TY */

static void cf_type(char *a, int nc, char *ty)
   {char *fty;

    if (strcmp(ty, "char *") == 0)
       nstrncpy(a, nc, "string", -1);

    else if (is_ptr(ty) == TRUE)
       nstrncpy(a, nc, "void *", -1);

    else
       {fty = lookup_type(ty, MODE_C, MODE_F);
	if (fty != NULL)
	   nstrncpy(a, nc, fty, -1);
	else
	   nstrncpy(a, nc, "unknown", -1);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FC_TYPE - return C type corresponding to FORTRAN type TY in WTY
 *         - return the kind of FORTRAN argument TY is
 */

static fparam fc_type(char *wty, int nc, char *ty, langmode mode)
   {fparam rv;
    char *pty;

    pty = lookup_type(ty, MODE_C, mode);

/* handle variable arg list */
    if (strcmp(ty, "...") == 0)
       {nstrncpy(wty, nc, "char *", -1);
	rv = FP_VARARG;}

/* handle function pointer */
    else if (strstr(ty, "(*") != NULL)
       {nstrncpy(wty, nc, "void *", -1);
	rv = FP_FNC;}

/* follow the PACT function pointer PF convention */
    else if (strncmp(ty, "PF", 2) == 0)
       {nstrncpy(wty, nc, "PFInt", -1);
	rv = FP_FNC;}

    else if (pty != NULL)
       {if (LAST_CHAR(ty) == '*')
	   {rv = FP_ARRAY;
	    if (mode == MODE_C)
	       deref(wty, MAXLINE, pty);
	    else if (mode == MODE_F)
	       {nstrncpy(wty, nc, pty, -1);
		wty[strlen(wty)-2] = '\0';}
	    else
	       nstrncpy(wty, nc, pty, -1);}
	else
	   {rv = FP_SCALAR;
	    nstrncpy(wty, nc, pty, -1);};}

    else
       {nstrncpy(wty, nc, ty, -1);
	if (LAST_CHAR(ty) == '*')
	   rv = FP_STRUCT;
	else
	   berr("Unknown type '%s'", ty);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FC_DECL_LIST - render the arg list of DCL into A for the
 *              - Fortran callable C wrapper
 */

static void fc_decl_list(char *a, int nc, fdecl *dcl)
   {int i, na;
    fparam knd;
    char lty[MAXLINE], lnm[MAXLINE];
    char *nm;
    farg *al;

    na = dcl->na;
    al = dcl->al;

    a[0] = '\0';
    if ((na == 0) || (no_args(dcl) == TRUE))
       nstrcat(a, MAXLINE, "void");
    else
       {for (i = 0; i < na; i++)
	    {nm = al[i].name;
	     nstrncpy(lnm, MAXLINE, nm, -1);
	     knd = fc_type(lty, MAXLINE, al[i].type, MODE_C);
	     switch (knd)
	        {case FP_PTR :
		      snprintf(nm, MAXLINE, "u%s", lnm);
		      vstrcat(a, MAXLINE, "%s %s, ", lty, nm);
		      break;
		 case FP_FNC :
		      snprintf(nm, MAXLINE, "f%s", lnm);
		      vstrcat(a, MAXLINE, "%s %s, ", lty, nm);
		      break;
		 case FP_STRING :
		      snprintf(nm, MAXLINE, "c%s", lnm);
		      vstrcat(a, MAXLINE, "%s *%s, ", lty, nm);
		      break;
		 case FP_ARRAY :
		      snprintf(nm, MAXLINE, "a%s", lnm);
		      vstrcat(a, MAXLINE, "%s *%s, ", lty, nm);
		      break;
		 case FP_SCALAR :
		      snprintf(nm, MAXLINE, "s%s", lnm);
		      vstrcat(a, MAXLINE, "%s *%s, ", lty, nm);
		      break;
		 case FP_STRUCT :
		      snprintf(nm, MAXLINE, "t%s", lnm);
		      vstrcat(a, MAXLINE, "%s *%s, ", lty, nm);
		      break;
		 default :
		      printf("Unknown type: %s\n", lty);
		      break;};

	   al[i].knd = knd;};

        a[strlen(a) - 2] = '\0';};

    nstrncpy(a, nc, subst(a, "* ", "*", -1), -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FC_CALL_LIST - render the arg list of DCL into A for the
 *              - C function call
 */

static void fc_call_list(char *a, int nc, fdecl *dcl)
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

static void f_proto_list(char *a, int nc, fdecl *dcl)
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

/* INIT_FORTRAN - initialize Fortran wrapper file */

static FILE *init_fortran(char *pck)
   {int nc;
    char fn[MAXLINE], ufn[MAXLINE], fill[MAXLINE];
    FILE *fp;

    snprintf(fn, MAXLINE, "gf-%s.c", pck);
    nstrncpy(ufn, MAXLINE, fn, -1);
    upcase(ufn);

    nc = strlen(fn);
    memset(fill, ' ', nc);
    fill[nc] = '\0';

    fp = open_file("w", fn);

    fprintf(fp, "/*\n");
    fprintf(fp, " * %s - F90 wrappers for %s\n", ufn, pck);
    fprintf(fp, " * %s - NOTE: this file was automatically generated\n", fill);
    fprintf(fp, " * %s - any manual changes will not be effective\n", fill);
    fprintf(fp, " *\n");
    fprintf(fp, " */\n");
    fprintf(fp, "\n");

    fprintf(fp, "\n");
    fprintf(fp, "#include \"cpyright.h\"\n");
    fprintf(fp, "#include \"%s_int.h\"\n", pck);
    fprintf(fp, "\n");

    csep(fp);

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP_FORTRAN - wrap C function PRO in FORTRAN callable function
 *              - using name FFN
 */

static void wrap_fortran(FILE *fp, fdecl *dcl, char *ffn)
   {int i, na, nv, voidf, voida, rptr, done;
    fparam knd;
    char ufn[MAXLINE], a[MAXLINE], rt[MAXLINE], t[MAXLINE];
    farg *al;

    if (strstr(dcl->proto, "...") != NULL)
       berr("%s is not interoperable - variable args", ffn);

    else if (strcmp(ffn, "none") != 0)
       {nstrncpy(ufn, MAXLINE, ffn, -1);
	upcase(ufn);

	na    = dcl->na;
	al    = dcl->al;
	voidf = (strcmp(dcl->type, "void") == 0);
	voida = no_args(dcl);

	knd = fc_type(rt, MAXLINE, dcl->type, MODE_C);
	fc_decl_list(a, MAXLINE, dcl);

	rptr = is_ptr(rt);

	csep(fp);
	fprintf(fp, "\n");

	switch (knd)
	   {case FP_PTR :
	         snprintf(t, MAXLINE, "%s *FF_ID(w%s, W%s)(%s)\n",
			  rt, ffn, ufn, a);
		 break;
	    case FP_STRING :
	         snprintf(t, MAXLINE, "%s *FF_ID(w%s, W%s)(%s)\n",
			  rt, ffn, ufn, a);
		 break;
	    case FP_ARRAY :
	         snprintf(t, MAXLINE, "%s *FF_ID(w%s, W%s)(%s)\n",
			  rt, ffn, ufn, a);
		 break;
	    case FP_STRUCT :
	         snprintf(t, MAXLINE, "FIXNUM FF_ID(w%s, W%s)(%s)\n",
			  ffn, ufn, a);
		 break;
	    case FP_ANY    :
	    case FP_VARARG :
	    case FP_FNC    :
	    case FP_SCALAR :
	    case FP_VOID   :
	    default        :
	         snprintf(t, MAXLINE, "%s FF_ID(w%s, W%s)(%s)\n",
			  rt, ffn, ufn, a);
		 break;};

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

	     done = FALSE;

/* variable for return value */
	     if (i == na)
	        {if (voidf == FALSE)
		    {switch (knd)
		        {case FP_PTR :
			      snprintf(t, MAXLINE, "%s *_rv;\n", rt);
			      break;
			 case FP_STRING :
			      snprintf(t, MAXLINE, "%s *_rv;\n", rt);
			      break;
			 case FP_ARRAY :
			      snprintf(t, MAXLINE, "%s *_rv;\n", rt);
			      break;
			 case FP_STRUCT :
			      snprintf(t, MAXLINE, "FIXNUM _rv;\n");
			      break;
			 case FP_ANY    :
			 case FP_VARARG :
			 case FP_FNC    :
			 case FP_SCALAR :
			 case FP_VOID   :
			 default        :
			      snprintf(t, MAXLINE, "%s _rv;\n", rt);
			      break;};};

		 done = TRUE;}

/* local vars */
	     else if (al[i].name[0] != '\0')
	        {switch (al[i].knd)
		    {case FP_PTR :
		          snprintf(t, MAXLINE, "%s _l%s;\n",
				   al[i].type, al[i].name);
		          break;
		     case FP_STRING :
		          snprintf(t, MAXLINE, "%s_l%s;\n",
				   al[i].type, al[i].name);
		          break;
		     case FP_ARRAY :
		          snprintf(t, MAXLINE, "%s_l%s;\n",
				   al[i].type, al[i].name);
		          break;
		     case FP_ANY    :
		     case FP_VARARG :
		     case FP_FNC    :
		     case FP_SCALAR :
		     case FP_VOID   :
		     default        :
		          snprintf(t, MAXLINE, "%s _l%s;\n",
				   al[i].type, al[i].name);
		          break;};
		 nv++;
		 done = TRUE;};

	     if (IS_NULL(t) == FALSE)
	        fputs(subst(t, "* ", "*", -1), fp);};

	fprintf(fp, "\n");

/* local variable assignments */
	nv = 0;
	for (i = 0; i < na; i++)
	    {if (al[i].name[0] != '\0')
	        {switch (al[i].knd)
		    {case FP_PTR    :
		          snprintf(t, MAXLINE, "    _l%-8s = (%s) %s;\n",
				   al[i].name, al[i].type, al[i].name);
		          break;
		     case FP_STRING :
		          snprintf(t, MAXLINE, "    _l%-8s = %s;\n",
				   al[i].name, al[i].name);
		          break;
		     case FP_ARRAY  :
		          snprintf(t, MAXLINE, "    _l%-8s = (%s) %s;\n",
				   al[i].name, al[i].type, al[i].name);
		          break;
		     case FP_FNC    :
		          snprintf(t, MAXLINE, "    _l%-8s = (%s) %s;\n",
				   al[i].name, al[i].type, al[i].name);
		          break;
		     case FP_ANY    :
		     case FP_VARARG :
		     case FP_SCALAR :
		     case FP_VOID   :
		     default        :
		          snprintf(t, MAXLINE, "    _l%-8s = (%s) *%s;\n",
				   al[i].name, al[i].type, al[i].name);
		          break;};
		 nv++;
		 fputs(t, fp);};};

	if (nv > 0)
	   fprintf(fp, "\n");

/* function call */
	fc_call_list(a, MAXLINE, dcl);
	if (voidf == FALSE)
	   {if ((rptr == TRUE) || (knd == FP_STRUCT))
	       fprintf(fp, "    _rv = SC_ADD_POINTER(%s(%s));\n",
		       dcl->name, a);
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
	csep(fp);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_FORTRAN - generate Fortran bindings from SPR and SBI
 *              - return TRUE iff successful
 */

static int bind_fortran(FILE *fp, char **spr, char **sbi)
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

static void fin_fortran(FILE *fp, char *pck)
   {

    csep(fp);
    fclose(fp);

    return;}

/*--------------------------------------------------------------------------*/

/*                         FORTRAN MODULE ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* MC_TYPE - return FORTRAN type, FTY, C cognate type, CTY,
 *         - corresponding to C type T
 *
 *    Type	Named constant	        C type or types
 *    INTEGER
 *    	C_INT			int, signed int
 *      C_SHORT			short int, signed short int
 *      C_LONG			long int, signed long int
 *      C_LONG_LONG		long long int, signed long long int
 *      C_SIGNED_CHAR		signed char, unsigned char
 *      C_SIZE_T		size_t
 *    	C_INT_LEAST8_T		int_least8_t
 *    	C_INT_LEAST16_T		int_least16_t
 *    	C_INT_LEAST32_T		int_least32_t
 *    	C_INT_LEAST64_T		int_least64_t
 *    	C_INT_FAST8_T		int_fast8_t
 *    	C_INT_FAST16_T		int_fast16_t
 *    	C_INT_FAST32_T		int_fast32_t
 *    	C_INT_FAST64_T		int_fast64_t
 *    	C_INTMAX_T		c intmax_t
 *    
 *    REAL
 *    	C_FLOAT			float, float _Imaginary
 *    	C_DOUBLE		double, double _Imaginary
 *    
 *    COMPLEX
 *    	C_LONG_DOUBLE		long double, long double _Imaginary
 *    	C_COMPLEX		_Complex
 *    	C_DOUBLE_COMPLEX	double _Complex
 *    	C_LONG_DOUBLE_COMPLEX  	long double _Complex
 *    
 *    LOGICAL
 *    	C_BOOL			_Bool
 *    
 *    CHARACTER
 *    	C_CHAR			char
 */

static fparam mc_type(int nc, char *fty, char *cty,
		      char *wty, char *wnm, char *ty)
   {fparam knd;
    char lnm[MAXLINE];

    nstrncpy(lnm, MAXLINE, wnm, -1);
    knd = fc_type(wty, nc, ty, MODE_F);
    memmove(wty, subst(wty, "FIXNUM", "integer", -1), nc);

    if (strcmp(ty, "...") == 0)
       {nstrncpy(fty, nc, "error", -1);
	nstrncpy(cty, nc, "error", -1);}

/* follow the PACT function pointer PF convention */
    else if ((strstr(ty, "(*") != NULL) ||
	     (strncmp(ty, "PF", 2) == 0))
       {nstrncpy(fty, nc, "type", -1);
	nstrncpy(cty, nc, "C_FUNPTR", -1);}

    else if (is_ptr(ty) == TRUE)
       {nstrncpy(fty, nc, "type", -1);
	nstrncpy(cty, nc, "C_PTR", -1);}

    else if (strstr(ty, "long long") != NULL)
       {nstrncpy(fty, nc, "integer", -1);
	nstrncpy(cty, nc, "C_LONG_LONG", -1);}

    else if (strstr(ty, "long") != NULL)
       {nstrncpy(fty, nc, "integer", -1);
	nstrncpy(cty, nc, "C_LONG", -1);}

    else if (strstr(ty, "short") != NULL)
       {nstrncpy(fty, nc, "integer", -1);
	nstrncpy(cty, nc, "C_SHORT", -1);}

    else if (strstr(ty, "int") != NULL)
       {nstrncpy(fty, nc, "integer", -1);
	nstrncpy(cty, nc, "C_INT", -1);}

    else if (strstr(ty, "signed char") != NULL)
       {nstrncpy(fty, nc, "integer", -1);
	nstrncpy(cty, nc, "C_SIGNED_CHAR", -1);}

    else if (strstr(ty, "size_t") != NULL)
       {nstrncpy(fty, nc, "integer", -1);
	nstrncpy(cty, nc, "C_SIZE_T", -1);}

    else if (strstr(ty, "long double _Complex") != NULL)
       {nstrncpy(fty, nc, "complex", -1);
	nstrncpy(cty, nc, "C_LONG_DOUBLE_COMPLEX", -1);}

    else if (strstr(ty, "double _Complex") != NULL)
       {nstrncpy(fty, nc, "complex", -1);
	nstrncpy(cty, nc, "C_DOUBLE_COMPLEX", -1);}

    else if (strstr(ty, "_Complex") != NULL)
       {nstrncpy(fty, nc, "complex", -1);
	nstrncpy(cty, nc, "C_COMPLEX", -1);}

    else if (strstr(ty, "long double") != NULL)
       {nstrncpy(fty, nc, "complex", -1);
	nstrncpy(cty, nc, "C_LONG_DOUBLE", -1);}

    else if (strstr(ty, "double") != NULL)
       {nstrncpy(fty, nc, "real", -1);
	nstrncpy(cty, nc, "C_DOUBLE", -1);}

    else if (strstr(ty, "float") != NULL)
       {nstrncpy(fty, nc, "real", -1);
	nstrncpy(cty, nc, "C_FLOAT", -1);}

    else if (strstr(ty, "_Bool") != NULL)
       {nstrncpy(fty, nc, "logical", -1);
	nstrncpy(cty, nc, "C_BOOL", -1);}

    else if (strcmp(ty, "void") == 0)
       {nstrncpy(fty, nc, "subroutine", -1);
	nstrncpy(cty, nc, "C_VOID", -1);}

/* take unknown types to be integer - covers enums */
    else
       {nstrncpy(fty, nc, "integer", -1);
	nstrncpy(cty, nc, "C_INT", -1);};

    return(knd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MC_NEED_PTR - return TRUE if the package types module will be needed */

static int mc_need_ptr(fdecl *dcl)
   {int i, na, ok;
    char fty[MAXLINE], cty[MAXLINE], wty[MAXLINE], wnm[MAXLINE];
    farg *al;

    na = dcl->na;
    al = dcl->al;

    ok = FALSE;

    if (mc_type(MAXLINE, fty, cty, wty, wnm, dcl->type) != FP_VARARG)
       ok |= ((strcmp(cty, "C_PTR") == 0) ||
	      (strcmp(cty, "C_FUNPTR") == 0));

    if ((na > 0) && (no_args(dcl) == FALSE))
       {for (i = 0; i < na; i++)
	    {if (mc_type(MAXLINE, fty, cty, wty, wnm, al[i].type) != FP_VARARG)
	        ok |= ((strcmp(cty, "C_PTR") == 0) ||
		       (strcmp(cty, "C_FUNPTR") == 0));};};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MC_DECL_LIST - render the arg list of DCL into A for the
 *              - Fortran/C interoperability module interface
 */

static void mc_decl_list(char *a, int nc, fdecl *dcl)
   {int i, na;
    fparam knd;
    char lnm[MAXLINE], lty[MAXLINE];
    char *nm;
    farg *al;

    na = dcl->na;
    al = dcl->al;

    a[0] = '\0';
    if ((na > 0) && (no_args(dcl) == FALSE))
       {for (i = 0; i < na; i++)
	    {nm = al[i].name;

/* if the names have not be processed to the S/A convention */
	     if (dcl->proc == FALSE)
	        {nstrncpy(lnm, MAXLINE, nm, -1);
		 knd = fc_type(lty, MAXLINE, al[i].type, MODE_F);
		 switch (knd)
		    {case FP_PTR :
		          snprintf(nm, MAXLINE, "u%s", lnm);
			  break;
		     case FP_FNC :
			  snprintf(nm, MAXLINE, "f%s", lnm);
			  break;
		     case FP_STRING :
			  snprintf(nm, MAXLINE, "c%s", lnm);
			  break;
		     case FP_ARRAY :
			  snprintf(nm, MAXLINE, "a%s", lnm);
			  break;
		     case FP_SCALAR :
			  snprintf(nm, MAXLINE, "s%s", lnm);
			  break;
		     default :
		          printf("Unknown type: %s\n", lty);
			  break;};

		 al[i].knd = knd;};

	     vstrcat(a, MAXLINE, "%s, ", nm);};

        a[strlen(a) - 2] = '\0';

	dcl->proc = TRUE;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_MODULE - initialize Fortran/C interoperatbility module file */

static FILE *init_module(char *pck)
   {int nc;
    char fn[MAXLINE], ufn[MAXLINE], fill[MAXLINE];
    FILE *fp;

    snprintf(fn, MAXLINE, "gm-%s.f", pck);
    nstrncpy(ufn, MAXLINE, fn, -1);
    upcase(ufn);

    nc = strlen(fn);
    memset(fill, ' ', nc);
    fill[nc] = '\0';

    fp = open_file("w", fn);

    fprintf(fp, "!\n");
    fprintf(fp, "! %s - F90 interfaces for %s\n", ufn, pck);
    fprintf(fp, "! %s - NOTE: this file was automatically generated\n", fill);
    fprintf(fp, "! %s - any manual changes will not be effective\n", fill);
    fprintf(fp, "!\n");
    fprintf(fp, "\n");

/* if there are no interfaces there is no need for the types module */
    fprintf(fp, "module types_%s\n", pck);
    fprintf(fp, "   integer, parameter :: isizea = %d\n",
	    (int) sizeof(char *));
    fprintf(fp, "end module types_%s\n", pck);
    fprintf(fp, "\n");

    fprintf(fp, "module pact_%s\n", pck);
    fprintf(fp, "   use iso_c_binding\n");
    fprintf(fp, "\n");

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ITF_WRAPPABLE - return TRUE iff the function described by DCL
 *               - can be fully wrapped as opposed to being declared
 *               - external
 */

static int itf_wrappable(fdecl *dcl)
   {int rv;
    char *pr;

    pr = dcl->proto;

    rv = ((strstr(pr, "...") == NULL) && (strstr(pr, "void *") == NULL));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ITF_WRAP_EXT - write the interface for a simple extern */

static void itf_wrap_ext(FILE *fp, char *pck, fdecl *dcl,
			 char *cfn, char *ffn)
   {fparam knd;
    char fty[MAXLINE], cty[MAXLINE], wty[MAXLINE], wnm[MAXLINE];
    char *rty;
    static int first = TRUE;

/* declare the incomplete ones as external */
    if ((itf_wrappable(dcl) == FALSE) && (strcmp(ffn, "none") != 0))
       {if (mc_need_ptr(dcl) == TRUE)
	   {if (first == TRUE)
	       {first = FALSE;
		fprintf(fp, "   use types_%s\n", pck);
		fprintf(fp, "\n");};};

	rty = dcl->type;
	knd = mc_type(MAXLINE, fty, cty, wty, wnm, rty);
	if (strcmp(rty, "void") == 0)
	   fprintf(fp, "   external :: w%s\n", ffn);
        else
	  {if ((strcmp(cty, "C_PTR") == 0) || (strcmp(cty, "C_FUNPTR") == 0))
	      fprintf(fp, "   integer(isizea), external :: w%s\n", ffn);
	   else
	     fprintf(fp, "   %s, external :: w%s\n", fty, ffn);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ITF_WRAP_FULL - write the interface for the fully wrapped version */

static void itf_wrap_full(FILE *fp, fdecl *dcl, char *pck,
			  char *cfn, char *ffn)
   {int i, na, voidf, voida;
    fparam knd;
    char a[MAXLINE], t[MAXLINE];
    char fty[MAXLINE], cty[MAXLINE], wty[MAXLINE], wnm[MAXLINE];
    char *nm, *ty, *rty, *oper;
    farg *al;

/* emit complete declarations */
    if ((itf_wrappable(dcl) == TRUE) && (strcmp(ffn, "none") != 0))
       {rty = dcl->type;

	na    = dcl->na;
	al    = dcl->al;
	voidf = (strcmp(rty, "void") == 0);
	voida = no_args(dcl);

	knd = mc_type(MAXLINE, fty, cty, wty, wnm, rty);
	mc_decl_list(a, MAXLINE, dcl);

	if (voidf == TRUE)
           oper = "subroutine";
        else
           oper = "function";

	snprintf(t, MAXLINE, "      %s w%s(%s)", oper, ffn, a);
	femit(fp, t, "");

	if (mc_need_ptr(dcl) == TRUE)
	   fprintf(fp, "         use types_%s\n", pck);

	fprintf(fp, "         implicit none\n");

/* return value declaration */
	if (voidf == FALSE)
	   {if ((strcmp(cty, "C_PTR") == 0) ||
		(strcmp(cty, "C_FUNPTR") == 0))
	       snprintf(t, MAXLINE, "         integer(isizea) :: w%s\n", ffn);
	    else
	       snprintf(t, MAXLINE, "         %-15s :: w%s\n", fty, ffn);

	    fputs(t, fp);};

/* argument declarations */
	for (i = 0; i < na; i++)
	    {if ((voida == TRUE) && (i == 0))
	        continue;

	     ty  = al[i].type;
	     nm  = al[i].name;
	     knd = mc_type(MAXLINE, fty, cty, wty, nm, ty);
	     switch (knd)
	        {case FP_PTR    :
		 case FP_FNC    :
		 case FP_STRUCT :
		      snprintf(t, MAXLINE, "         integer(isizea) :: %s\n",
			       nm);
		      break;
		 case FP_ARRAY :
		      if (strcmp(wty, "character") == 0)
			 snprintf(t, MAXLINE, "         character(*)    :: %s\n", 
				  nm);
		      else if (strcmp(wty, "C_PTR") == 0)
			 snprintf(t, MAXLINE, "         integer(isizea) :: %s\n",
				  nm);
		      else
			 snprintf(t, MAXLINE, "         %-15s :: %s(*)\n",
				  wty, nm);
		      break;
		 case FP_SCALAR :
		 default :
		      snprintf(t, MAXLINE, "         %-15s :: %s\n",
			       wty, nm);
		      break;};

	     fputs(t, fp);};

	fprintf(fp, "      end %s w%s\n", oper, ffn);

	fprintf(fp, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP_MODULE - generate FORTRAN interface specification for
 *             - C function CFN using name FFN
 */

static void wrap_module(FILE *fp, fdecl *dcl, char *cfn)
   {int i, na, nv, voidf, voida, rptr;
    fparam knd;
    char dcn[MAXLINE], a[MAXLINE];
    char fty[MAXLINE], cty[MAXLINE], wty[MAXLINE], wnm[MAXLINE];
    char cd[MAXLINE], cb[MAXLINE];
    farg *al;

    if (strstr(dcl->proto, "...") != NULL)
       berr("%s is not interoperable - variable args", cfn);

    else
       {nstrncpy(dcn, MAXLINE, cfn, -1);
	downcase(dcn);

	na    = dcl->na;
	al    = dcl->al;
	voidf = (strcmp(dcl->type, "void") == 0);
	voida = no_args(dcl);

	knd = mc_type(MAXLINE, fty, cty, wty, wnm, dcl->type);
	mc_decl_list(a, MAXLINE, dcl);

	rptr = is_ptr(fty);

	if (voidf == TRUE)
	   snprintf(cd, MAXLINE, "      %s %s(%s)", fty, dcn, a);
	else
	   snprintf(cd, MAXLINE, "      %s (%s) function %s(%s)",
		    fty, cty, dcn, a);

	snprintf(cb, MAXLINE, "bind(c, name='%s')", cfn);

	femit(fp, cd, "&");
	fprintf(fp, "                %s\n", cb);

	fprintf(fp, "         use iso_c_binding\n");
	fprintf(fp, "         implicit none\n");

/* argument declarations */
	nv = 0;
	for (i = 0; i < na; i++)
	    {if ((voida == TRUE) && (i == 0))
	        continue;

	     knd = mc_type(MAXLINE, fty, cty, wty, wnm, al[i].type);
	     fprintf(fp, "         %s (%s), value :: %s\n",
		     fty, cty, al[i].name);};

	if (voidf == TRUE)
	   fprintf(fp, "      end subroutine %s\n", dcn);
	else
	   fprintf(fp, "      end function %s\n", dcn);
	fprintf(fp, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_MODULE - generate Fortran/C interoperability interface
 *             - from SPR and SBI
 *             - return TRUE iff successful
 */

static int bind_module(FILE *fp, char *pck, char **spr, char **sbi)
   {int ib, rv;
    char t[MAXLINE];
    char *sb, **ta;
    char *cfn, *ffn;
    fdecl *dcl;

    rv = TRUE;

/* make simple external declaration for variable argument functions */
    for (ib = 0; sbi[ib] != NULL; ib++)
        {sb = sbi[ib];
	 if (blank_line(sb) == FALSE)
	    {nstrncpy(t, MAXLINE, sb, -1);
	     ta = tokenize(t, " \t");
	     cfn = ta[0];
	     ffn = ta[1];
             dcl = find_proto(spr, cfn);
	     if (dcl != NULL)
	        {itf_wrap_ext(fp, pck, dcl, cfn, ffn);
		 free_decl(dcl);};

	     free_strings(ta);};};

    fprintf(fp, "\n");

/* start the interface */
    fprintf(fp, "   interface\n");
    fprintf(fp, "\n");

/* make full interface for non-variable argument functions */
    for (ib = 0; sbi[ib] != NULL; ib++)
        {sb = sbi[ib];
	 if (blank_line(sb) == FALSE)
	    {nstrncpy(t, MAXLINE, sb, -1);
	     ta = tokenize(t, " \t");
	     cfn = ta[0];
	     ffn = ta[1];
             dcl = find_proto(spr, cfn);
	     if (dcl != NULL)
	        {wrap_module(fp, dcl, cfn);
		 itf_wrap_full(fp, dcl, pck, cfn, ffn);
		 free_decl(dcl);};

	     free_strings(ta);};};

/* finish the interface */
    fprintf(fp, "   end interface\n");
    fprintf(fp, "\n");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_MODULE - finalize Fortran/C interoperability interface module file */

static void fin_module(FILE *fp, char *pck)
   {

    fprintf(fp, "end module pact_%s\n", pck);
    fprintf(fp, "\n");

    fclose(fp);

    return;}

/*--------------------------------------------------------------------------*/

/*                             SCHEME ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* SC_TYPE - return C type corresponding to SCHEME type T */

static void sc_type(char *a, int nc, char *t)
   {

    if (strcmp(t, "char *") == 0)
       nstrncpy(a, nc, "char *", -1);
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
       nstrncpy(a, nc, t, -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CS_TYPE - return "Scheme" type corresponding to C type TY */

static void cs_type(char *a, int nc, char *ty)
   {char *sty;

    sty = lookup_type(ty, MODE_C, MODE_S);
    if (sty != NULL)
       nstrncpy(a, nc, sty, -1);

    else if (strcmp(ty, "void *") == 0)
       nstrncpy(a, nc, "SC_VOID_I", -1);

    else if ((strchr(ty, '*') != NULL) ||
	     (strstr(ty, "(*") != NULL) ||
	     (strstr(ty, "PF") != NULL))
       nstrncpy(a, nc, "SC_POINTER_I", -1);

    else
       nstrncpy(a, nc, "SC_UNKNOWN_I", -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CS_DECL_LIST - render the arg list of DCL into A for the
 *              - Scheme to C value extaction
 */

static void cs_decl_list(char *a, int nc, fdecl *dcl)
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

static void s_proto_list(char *a, int nc, fdecl *dcl)
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

static FILE *init_scheme(char *pck)
   {FILE *fp;

    fp = open_file("w", "gs-%s.c", pck);

    fprintf(fp, "\n");
    fprintf(fp, "#include \"sx_int.h\"\n");
    fprintf(fp, "#include \"%s_int.h\"\n", pck);
    fprintf(fp, "\n");

    csep(fp);

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP_SCHEME - wrap C function DCL in SCHEME callable function
 *             - using name FFN
 */

static char **wrap_scheme(FILE *fp, char **fl, fdecl *dcl,
			  char *ffn, char **com)
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
       fprintf(fp, "(SS_psides *si)");
    else
       fprintf(fp, "(SS_psides *si, object *argl)");
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
        fprintf(fp, "    SS_args(si, argl,\n");
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
	     "    SS_install(si, \"%s\",\n               \"%s\",\n               SS_nargs,\n               _SXI_%s, SS_PR_PROC);\n\n",
	     ffn, t, dcl->name);
    fl = lst_add(fl, a);

    return(fl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_SCHEME - generate Scheme bindings from SPR and SBI
 *             - return TRUE iff successful
 */

static int bind_scheme(FILE *fp, char *pck, char **spr, char **sbi)
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
	fprintf(fp, "void SX_install_%s_bindings(SS_psides *si)\n", pck);
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

static void fin_scheme(FILE *fp)
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

static void p_proto_list(char *a, int nc, fdecl *dcl)
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

static FILE *init_python(char *pck)
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

static void wrap_python(FILE *fp, fdecl *dcl, char *ffn)
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

static int bind_python(FILE *fp, char **spr, char **sbi)
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

static void fin_python(FILE *fp)
   {

    csep(fp);
    fclose(fp);

    return;}

/*--------------------------------------------------------------------------*/

/*                              DOC ROUTINES                                */

/*--------------------------------------------------------------------------*/

/* INIT_DOC - initialize Doc file */

static FILE *init_doc(char *pck)
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

static void wrap_doc(FILE *fp, fdecl *dcl, char **fn, char **com)
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

static int bind_doc(FILE *fp, char **spr, char **sbi)
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

static void fin_doc(FILE *fp)
   {

    hsep(fp);
    fclose(fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BLANG - control the generation of language binding
 *       - return TRUE iff successful
 */

static int blang(char *pck, int fwr, char *fpr, char *fbi)
   {int rv;
    char **spr, **sbi;
    FILE *ff, *fm, *fs, *fp, *fd;

    if ((IS_NULL(fpr) == TRUE) || (IS_NULL(fbi) == TRUE))
       rv = FALSE;

    else
       {rv = TRUE;

	spr = file_text(fpr);
	sbi = file_text(fbi);

	add_derived_types(sbi);

	if (spr != NULL)
	   {if (fwr & 1)
	       ff = init_fortran(pck);
	    if (fwr & 2)
	       fm = init_module(pck);
	    fs = init_scheme(pck);
	    fp = init_python(pck);
	    fd = init_doc(pck);

	    if (sbi != NULL)
	       {if (fwr & 1)
		   rv &= bind_fortran(ff, spr, sbi);
		if (fwr & 2)
		   rv &= bind_module(fm, pck, spr, sbi);
		rv &= bind_scheme(fs, pck, spr, sbi);
		rv &= bind_python(fp, spr, sbi);
		rv &= bind_doc(fd, spr, sbi);};

	    if (fwr & 1)
	       fin_fortran(ff, pck);
	    if (fwr & 2)
	       fin_module(fm, pck);
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
   {int i, rv, fif;
    char pck[MAXLINE], msg[MAXLINE];
    char *fpr, *fbi;

    fpr = "";
    fbi = "";
    fif = 3;

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-h") == 0)
            {printf("Usage: blang [-h] [-w] <prototypes> <bindings>\n");
             printf("   h    this help message\n");
             printf("   o    no interoprabilty interfaces (Fortran wrappers only)\n");
             printf("   w    no Fortran wrappers (interoperability only)\n");
             printf("   <prototypes>    file containing C function prototypes\n");
             printf("   <binding>       file specifying function bindings\n");
             printf("\n");}
	 else if (strcmp(v[i], "-o") == 0)
            fif &= ~2;
	 else if (strcmp(v[i], "-w") == 0)
            fif &= ~1;
	 else
	    {if (IS_NULL(fpr) == TRUE)
	        fpr = v[i];
	     else if (IS_NULL(fbi) == TRUE)
	        fbi = v[i];};};

    snprintf(pck, MAXLINE, "%s", path_base(path_tail(fbi)));
    snprintf(msg, MAXLINE, "%s bindings", pck);

    printf("      %s ", fill_string(msg, 25));

    init_types();

    rv = blang(pck, fif, fpr, fbi);
    rv = (rv != TRUE);

    printf("done\n");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

