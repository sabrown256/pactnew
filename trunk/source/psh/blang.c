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
   {FP_ANY = -1, FP_VARARG, FP_FNC, FP_STRUCT, FP_SCALAR, FP_ARRAY};

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
    char **args;                      /* all args */
    int nc;
    char **cargs;                     /* char * args */
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
    else if (strcmp(p, "...") == 0)
       pn = p + 3;
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

/* FIND_FUNC - return true if function named in prototype PR matches F */

static int find_func(char *pr, char *f)
   {int rv;
    char t[MAXLINE];

    snprintf(t, MAXLINE, "%s(", f);
    rv = (strstr(pr, t) != NULL);

    return(rv);}

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
	    {if (find_func(sp, f) == TRUE)
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
	dcl->na    = 0;
	dcl->args  = NULL;
	dcl->nc    = 0;
	dcl->cargs = NULL;

/* break up the prototype into the type/function name and args */
	nstrncpy(pf, MAXLINE, pro, -1);
	p = strchr(pf, '(');
	*p++ = '\0';
	pa = p;
	LAST_CHAR(pa) = '\0';

/* get the return type */
        split_decl(dcl->type, MAXLINE, dcl->name, MAXLINE, pf);
	
	ty = tokenize(pf, " \t");
	nt = lst_length(ty);

/* get the function name */
	cfn = ty[nt-1];
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

    add_type("void",        "",            "",                 NULL);
    add_type("bool",        "logical",     "SC_BOOL_I",        NULL);
    add_type("char",        "character",   "SC_CHAR_I",        NULL);

/* fixed point types */
    add_type("short",       "integer(2)",  "SC_SHORT_I",       NULL);
    add_type("int",         "integer",     "SC_INT_I",         NULL);
    add_type("long",        "integer(8)",  "SC_LONG_I",        NULL);
    add_type("long long",   "integer(8)",  "SC_LONG_LONG_I",   NULL);

/* fixed width fixed point types */
    add_type("int16_t",     "integer(2)",  "SC_INT16_I",       NULL);
    add_type("int32_t",     "integer(4)",  "SC_INT32_I",       NULL);
    add_type("int64_t",     "integer(8)",  "SC_INT64_I",       NULL);

/* floating point types */
    add_type("float",       "real(4)",     "SC_FLOAT_I",       NULL);
    add_type("double",      "real(8)",     "SC_DOUBLE_I",      NULL);
    add_type("long double", "real(16)",    "SC_LONG_DOUBLE_I", NULL);

/* complex types */
    add_type("float _Complex",       "complex(4)",
	     "SC_FLOAT_COMPLEX_I", NULL);
    add_type("double _Complex",      "complex(8)",
	     "SC_DOUBLE_COMPLEX_I", NULL);
    add_type("long double _Complex", "complex(16)",
	     "SC_LONG_DOUBLE_COMPLEX_I", NULL);

    add_type("void *",        "C_PTR-A",      "SC_VOID_P_I",        NULL);
    add_type("bool *",        "logical-A",    "SC_BOOL_P_I",        NULL);
    add_type("char *",        "character-A",  "SC_STRING_I",        NULL);

    add_type("short *",       "integer(2)-A", "SC_SHORT_P_I",       NULL);
    add_type("int *",         "integer-A",    "SC_INT_P_I",         NULL);
    add_type("long *",        "integer(8)-A", "SC_LONG_P_I",        NULL);
    add_type("long long *",   "integer(8)-A", "SC_LONG_LONG_P_I",   NULL);

    add_type("float *",       "real(4)-A",    "SC_FLOAT_P_I",       NULL);
    add_type("double *",      "real(8)-A",    "SC_DOUBLE_P_I",      NULL);
    add_type("long double *", "real(16)-A",   "SC_LONG_DOUBLE_P_I", NULL);

    add_type("pcons",         "C_PTR-A",      "SC_PCONS_I",         NULL);
    add_type("pcons *",       "C_PTR-A",      "SC_PCONS_P_I",       NULL);
    add_type("FILE *",        "C_PTR-A",      "SC_FILE_I",          NULL);
    add_type("PROCESS *",     "C_PTR-A",      "SC_PROCESS_I",       NULL);

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
/*--------------------------------------------------------------------------*/

/* RENAME_VAR - rename the variable ONM to NNM according to type KND */

static void rename_var(char *nnm, int nc, char *onm, fparam knd)
   {

    switch (knd)
       {case FP_FNC :
	     snprintf(nnm, nc, "f%s", onm);
	     break;
        case FP_ARRAY :
	     snprintf(nnm, nc, "a%s", onm);
	     break;
        case FP_SCALAR :
	     snprintf(nnm, nc, "s%s", onm);
	     break;
        case FP_STRUCT :
	     snprintf(nnm, nc, "t%s", onm);
	     break;
        default :
	     printf("Unknown type: %s\n", onm);
	     break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAP_NAME - map the C function name S to form target language */

static char *map_name(char *d, int nc, char *s, char *sfx, int cs)
   {

    if (sfx != NULL)
       snprintf(d, nc, "%s_%s", s, sfx);
    else
       snprintf(d, nc, "%s", s);

    switch (cs)
       {case -1 :
	     downcase(d);
	     break;
	case 1 :
	     upcase(d);
	     break;};

    return(d);}

/*--------------------------------------------------------------------------*/
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

/*                            FORTRAN ROUTINES                              */

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

/* for C dereference the type so that we have an array of type */
	    if (mode == MODE_C)
	       deref(wty, MAXLINE, pty);

/* for Fortran account for the -A convention used in the type registry */
	    else if (mode == MODE_F)
	       {nstrncpy(wty, nc, pty, -1);
		wty[strlen(wty)-2] = '\0';}

/* do not yet know to do anything but take the type for other bindings */
	    else
	       nstrncpy(wty, nc, pty, -1);}
	else
	   {rv = FP_SCALAR;
	    nstrncpy(wty, nc, pty, -1);};}

/* unknown type */
    else
       {nstrncpy(wty, nc, ty, -1);

/* assume a pointer to an unknown type is a struct pointer */
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
   {int i, na, nca;
    fparam knd;
    char lty[MAXLINE], lnm[MAXLINE];
    char *nm, **cargs;
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
             rename_var(nm, MAXLINE, lnm, knd);
	     switch (knd)
	        {case FP_FNC :
		      vstrcat(a, MAXLINE, "%s %s, ", lty, nm);
		      break;
		 case FP_ARRAY :
		      if ((dcl->nc == 0) && (strcmp(lty, "char") == 0))
			 dcl->cargs = lst_push(dcl->cargs, lnm);
		      vstrcat(a, MAXLINE, "%s *%s, ", lty, nm);
		      break;
		 case FP_SCALAR :
		      vstrcat(a, MAXLINE, "%s *%s, ", lty, nm);
		      break;
		 case FP_STRUCT :
		      vstrcat(a, MAXLINE, "%s *%s, ", lty, nm);
		      break;
		 default :
		      printf("Unknown type: %s\n", lty);
		      break;};

	   al[i].knd = knd;};

	cargs = dcl->cargs;
	nca   = lst_length(cargs);
        for (i = 0; i < nca; i++)
	    vstrcat(a, MAXLINE, "int snca%s, ", cargs[i]);
	if (dcl->nc == 0)
	   dcl->nc = nca;

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

/* FORTRAN_PROTO_LIST - render the arg list of DCL into A for the
 *                    - Fortran callable C wrapper
 */

static void fortran_proto_list(char *a, int nc, fdecl *dcl)
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

    fprintf(fp, "#include \"cpyright.h\"\n");
    fprintf(fp, "#include \"%s_int.h\"\n", pck);
    fprintf(fp, "\n");

    csep(fp);

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FORTRAN_WRAP_DECL - function declaration */

static void fortran_wrap_decl(FILE *fp, fdecl *dcl,
			      fparam knd, char *rt, char *cfn)
   {char ucn[MAXLINE], dcn[MAXLINE], a[MAXLINE], t[MAXLINE];

    map_name(dcn, MAXLINE, cfn, "f", -1);

    nstrncpy(ucn, MAXLINE, dcn, -1);
    upcase(ucn);

    fc_decl_list(a, MAXLINE, dcl);

    fprintf(fp, "\n");

    switch (knd)
       {case FP_STRUCT :
	     snprintf(t, MAXLINE, "FIXNUM FF_ID(%s, %s)(%s)\n",
		      dcn, ucn, a);
	     break;
        case FP_ARRAY :
	     snprintf(t, MAXLINE, "%s *FF_ID(%s, %s)(%s)\n",
		      rt, dcn, ucn, a);
	     break;
        case FP_SCALAR :
	     snprintf(t, MAXLINE, "%s FF_ID(%s, %s)(%s)\n",
		      rt, dcn, ucn, a);
	     break;
        default :
	     snprintf(t, MAXLINE, "%s FF_ID(%s, %s)(%s)\n",
		      rt, dcn, ucn, a);
	     break;};

    fputs(subst(t, "* ", "*", -1), fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FORTRAN_WRAP_LOCAL_DECL - local variable declarations */

static void fortran_wrap_local_decl(FILE *fp, fdecl *dcl,
				    fparam knd, char *rt, int voidf)
   {int i, na, nv, done, voida;
    char t[MAXLINE];
    char *nm, *ty;
    farg *al;

    voida = no_args(dcl);

    na = dcl->na;
    al = dcl->al;

    nv = 0;
    for (i = 0; i <= na; i++)
        {nm = al[i].name;
	 if ((voida == TRUE) && (i == 0))
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
		    {case FP_STRUCT :
		          snprintf(t, MAXLINE, "FIXNUM _rv;\n");
			  vstrcat(t, MAXLINE, "    SC_address _ad%s;\n",
				  dcl->name);
			  break;
		     case FP_ARRAY :
			  snprintf(t, MAXLINE, "%s *_rv;\n", rt);
			  break;
		     case FP_SCALAR :
		          snprintf(t, MAXLINE, "%s _rv;\n", rt);
			  break;
		     default :
		          snprintf(t, MAXLINE, "%s _rv;\n", rt);
			  break;};};

	     done = TRUE;}

/* local vars */
	 else if (nm[0] != '\0')
	    {ty = al[i].type;
	     switch (al[i].knd)
		{case FP_STRUCT :
		      snprintf(t, MAXLINE, "%s _l%s;\n", ty, nm);
		      break;
		 case FP_ARRAY :
		      if (strcmp(ty, "char *") == 0)
			 snprintf(t, MAXLINE, "char _l%s[MAXLINE];\n", nm);
		      else
			 snprintf(t, MAXLINE, "%s_l%s;\n", ty, nm);
		      break;
		 case FP_SCALAR :
		      snprintf(t, MAXLINE, "%s _l%s;\n", ty, nm);
		      break;
		 default :
		      snprintf(t, MAXLINE, "%s _l%s;\n", ty, nm);
		      break;};
	     nv++;
	     done = TRUE;};

	 if (IS_NULL(t) == FALSE)
	    fputs(subst(t, "* ", "*", -1), fp);};

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FORTRAN_WRAP_LOCAL_ASSN - local variable assignments */

static void fortran_wrap_local_assn(FILE *fp, fdecl *dcl)
   {int i, na, nv;
    fparam knd;
    char t[MAXLINE];
    char *nm, *ty;
    farg *al;

    na = dcl->na;
    al = dcl->al;

    nv = 0;
    for (i = 0; i < na; i++)
        {nm = al[i].name;
	 if (nm[0] != '\0')
	    {ty  = al[i].type;
	     knd = al[i].knd;
	     switch (knd)
	        {case FP_FNC :
		      snprintf(t, MAXLINE, "    _l%-8s = (%s) %s;\n",
			       nm, ty, nm);
		      break;
		 case FP_STRUCT :
		      snprintf(t, MAXLINE, "    _l%-8s = *(%s*) %s;\n",
			       nm, ty, nm);
		      break;
		 case FP_ARRAY :
		      if (strcmp(ty, "char *") == 0)
			 snprintf(t, MAXLINE,
				  "    SC_FORTRAN_STR_C(_l%s, %s, snc%s);\n",
				  nm, nm, nm);
		      else
			 snprintf(t, MAXLINE, "    _l%-8s = (%s) %s;\n",
				  nm, ty, nm);
		      break;
		 case FP_SCALAR :
		      snprintf(t, MAXLINE, "    _l%-8s = (%s) *%s;\n",
			       nm, ty, nm);
		      break;
		 default :
		      snprintf(t, MAXLINE, "    _l%-8s = (%s) *%s;\n",
			       nm, ty, nm);
		      break;};
	     nv++;
	     fputs(t, fp);};};

    if (nv > 0)
       fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FORTRAN_WRAP_LOCAL_CALL - function call */

static void fortran_wrap_local_call(FILE *fp, fdecl *dcl,
				    fparam knd, char *rt, int voidf)
   {char a[MAXLINE];
    char *nm;

    fc_call_list(a, MAXLINE, dcl);

    nm = dcl->name;
    if (voidf == FALSE)
       {switch (knd)
	   {case FP_STRUCT :
	         fprintf(fp, "    _ad%s.memaddr = (void *) %s(%s);\n",
			 nm, nm, a);
		 break;
	    default :
	         fprintf(fp, "    _rv = %s(%s);\n", nm, a);
		 break;};}
    else
       fprintf(fp, "    %s(%s);\n", nm, a);

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FORTRAN_WRAP_LOCAL_RETURN - emit the return */

static void fortran_wrap_local_return(FILE *fp, fdecl *dcl,
				      fparam knd, int voidf)
   {char *nm;

    nm = dcl->name;
    if (voidf == FALSE)
       {switch (knd)
	   {case FP_STRUCT :
	         fprintf(fp, "    _rv = _ad%s.diskaddr;\n\n", nm);
	         fprintf(fp, "    return(_rv);}\n");
		 break;
	    default :
	         fprintf(fp, "    return(_rv);}\n");
		 break;};}
    else
       fprintf(fp, "    return;}\n");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FORTRAN_WRAP - wrap C function PRO in FORTRAN callable function
 *              - using name FFN
 */

static void fortran_wrap(FILE *fp, fdecl *dcl, char *cfn, char *ffn)
   {int voidf;
    fparam knd;
    char rt[MAXLINE];

    if (strstr(dcl->proto, "...") != NULL)
       berr("%s is not interoperable - variable args", ffn);

    else if (strcmp(ffn, "none") != 0)
       {voidf = (strcmp(dcl->type, "void") == 0);

	knd = fc_type(rt, MAXLINE, dcl->type, MODE_C);

	csep(fp);

/* function declaration */
	fortran_wrap_decl(fp, dcl, knd, rt, cfn);

/* local variable declarations */
        fortran_wrap_local_decl(fp, dcl, knd, rt, voidf);

/* local variable assignments */
	fortran_wrap_local_assn(fp, dcl);

/* function call */
	fortran_wrap_local_call(fp, dcl, knd, rt, voidf);

/* return */
        fortran_wrap_local_return(fp, dcl, knd, voidf);

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
	        {fortran_wrap(fp, dcl, cfn, ffn);
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
 *    	C_FLOAT_COMPLEX		float _Complex
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

    else if (strstr(ty, "float _Complex") != NULL)
       {nstrncpy(fty, nc, "complex", -1);
	nstrncpy(cty, nc, "C_FLOAT_COMPLEX", -1);}

    else if (strstr(ty, "long double") != NULL)
       {nstrncpy(fty, nc, "real", -1);
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
		 rename_var(nm, MAXLINE, lnm, knd);
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

/* MODULE_PRE_WRAPPABLE - return TRUE iff the function described by PR
 *                      - can be fully wrapped as opposed to being declared
 *                      - external
 */

static int module_pre_wrappable(char *pr)
   {int rv;

    rv = ((strstr(pr, "...") == NULL) &&
	  (strstr(pr, "integer(isizea)") == NULL) &&
	  (strstr(pr, "external") == NULL));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MODULE_PRE_WRAP_EXT - write the Fortran interface for hand written
 *                     - prototype PR
 *                     - for functions that can only be declared external
 */

static void module_pre_wrap_ext(FILE *fp, char *pr, char **ta, char *pck)
   {

    if ((ta != NULL) && (module_pre_wrappable(pr) == FALSE))
       {if (strcmp(ta[0], "void") == 0)
	   fprintf(fp, "   external :: %s\n", ta[1]);
	else
	   fprintf(fp, "   %s, external :: %s\n", ta[0], ta[1]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MODULE_PRE_WRAP_FULL - write the Fortran interface for hand written
 *                      - prototype PR
 *                      - for functions that can only be fully declared
 */

static void module_pre_wrap_full(FILE *fp, char *pr, char **ta, char *pck)
   {int i, nt;
    char a[MAXLINE];
    char *oper;

    if ((ta != NULL) && (module_pre_wrappable(pr) == TRUE))
       {if (strcmp(ta[0], "void") == 0)
           oper = "subroutine";
        else
	   oper = "function";

	nt = lst_length(ta);

	a[0] = '\0';
	vstrcat(a, MAXLINE, "      %s %s(", oper, ta[1]);
        for (i = 3; i < nt; i += 2)
	    vstrcat(a, MAXLINE, "%s, ", ta[i]);
	if (nt > 2)
	   a[strlen(a) - 2] = '\0';
	vstrcat(a, MAXLINE, ")");

	femit(fp, a, "");

	if (strstr(pr, "(isizea)") != NULL)
	   fprintf(fp, "         use types_%s\n", pck);

	fprintf(fp, "         implicit none\n");
	if (strcmp(oper, "function") == 0)
	   fprintf(fp, "         %-12s :: %s\n", ta[0], ta[1]);

	for (i = 2; i < nt; i += 2)
	    {if (strcmp(ta[i], "character") == 0)
		fprintf(fp, "         %s(*) :: %s\n", ta[i], ta[i+1]);
             else if (strcmp(ta[i], "integer-A") == 0)
                fprintf(fp, "         %-12s :: %s(*)\n", "integer", ta[i+1]);
             else if (strcmp(ta[i], "real8-A") == 0)
                fprintf(fp, "         %-12s :: %s(*)\n", "real*8", ta[i+1]);
             else
                fprintf(fp, "         %-12s :: %s\n", ta[i], ta[i+1]);};

	fprintf(fp, "      end %s %s\n", oper, ta[1]);
	fprintf(fp, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MODULE_ITF_WRAPPABLE - return TRUE iff the function described by DCL
 *                      - can be fully wrapped as opposed to being declared
 *                      - external
 */

static int module_itf_wrappable(fdecl *dcl)
   {int rv;
    char *pr;

    pr = dcl->proto;

    rv = ((strstr(pr, "...") == NULL) && (strstr(pr, "void *") == NULL));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MODULE_ITF_WRAP_EXT - write the interface for a simple extern */

static void module_itf_wrap_ext(FILE *fp, char *pck, fdecl *dcl,
				char *cfn, char *ffn)
   {fparam knd;
    char dcn[MAXLINE], wnm[MAXLINE];
    char fty[MAXLINE], cty[MAXLINE], wty[MAXLINE];
    char *rty;
    static int first = TRUE;

/* declare the incomplete ones as external */
    if ((module_itf_wrappable(dcl) == FALSE) && (strcmp(ffn, "none") != 0))
       {map_name(dcn, MAXLINE, cfn, "f", -1);

	if (mc_need_ptr(dcl) == TRUE)
	   {if (first == TRUE)
	       {first = FALSE;
		fprintf(fp, "   use types_%s\n", pck);
		fprintf(fp, "\n");};};

	rty = dcl->type;
	knd = mc_type(MAXLINE, fty, cty, wty, wnm, rty);
	if (strcmp(rty, "void") == 0)
	   fprintf(fp, "   external :: %s\n", dcn);
        else
	  {if ((strcmp(cty, "C_PTR") == 0) || (strcmp(cty, "C_FUNPTR") == 0))
	      fprintf(fp, "   integer(isizea), external :: %s\n", dcn);
	   else
	     fprintf(fp, "   %s, external :: %s\n", fty, dcn);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MODULE_ITF_WRAP_FULL - write the Fortran interface for
 *                      - C function CFN
 */

static void module_itf_wrap_full(FILE *fp, fdecl *dcl, char *pck,
				 char *cfn, char *ffn)
   {int i, na, voidf, voida;
    fparam knd;
    char dcn[MAXLINE], a[MAXLINE], t[MAXLINE];
    char fty[MAXLINE], cty[MAXLINE], wty[MAXLINE], wnm[MAXLINE];
    char *nm, *ty, *rty, *oper;
    farg *al;

/* emit complete declarations */
    if ((module_itf_wrappable(dcl) == TRUE) && (strcmp(ffn, "none") != 0))
       {map_name(dcn, MAXLINE, cfn, "f", -1);

	rty   = dcl->type;
	na    = dcl->na;
	al    = dcl->al;
	voida = no_args(dcl);
	voidf = (strcmp(rty, "void") == 0);
	oper  = (voidf == TRUE) ? "subroutine" : "function";

	knd = mc_type(MAXLINE, fty, cty, wty, wnm, rty);
	mc_decl_list(a, MAXLINE, dcl);

	snprintf(t, MAXLINE, "      %s %s(%s)", oper, dcn, a);
	femit(fp, t, "");

	if (mc_need_ptr(dcl) == TRUE)
	   fprintf(fp, "         use types_%s\n", pck);

	fprintf(fp, "         implicit none\n");

/* return value declaration */
	if (voidf == FALSE)
	   {if ((strcmp(cty, "C_PTR") == 0) ||
		(strcmp(cty, "C_FUNPTR") == 0))
	       snprintf(t, MAXLINE, "         integer(isizea) :: %s\n", dcn);
	    else
	       snprintf(t, MAXLINE, "         %-15s :: %s\n", fty, dcn);

	    fputs(t, fp);};

/* argument declarations */
	for (i = 0; i < na; i++)
	    {if ((voida == TRUE) && (i == 0))
	        continue;

	     ty  = al[i].type;
	     nm  = al[i].name;
	     knd = mc_type(MAXLINE, fty, cty, wty, nm, ty);
	     switch (knd)
	        {case FP_FNC    :
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
		      snprintf(t, MAXLINE, "         %-15s :: %s\n",
			       wty, nm);
		      break;
		 default :
		      snprintf(t, MAXLINE, "         %-15s :: %s\n",
			       wty, nm);
		      break;};

	     fputs(t, fp);};

	fprintf(fp, "      end %s %s\n", oper, dcn);

	fprintf(fp, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MODULE_INTEROP_WRAP - generate FORTRAN/C interoperability interface for
 *                     - C function CFN
 */

static void module_interop_wrap(FILE *fp, fdecl *dcl, char *cfn)
   {int i, na, nv, voidf, voida;
    fparam knd;
    char dcn[MAXLINE], a[MAXLINE];
    char fty[MAXLINE], cty[MAXLINE], wty[MAXLINE], wnm[MAXLINE];
    char cd[MAXLINE], cb[MAXLINE];
    char *nm, *ty, *rty, *oper;
    farg *al;

    if (strstr(dcl->proto, "...") != NULL)
       berr("%s is not interoperable - variable args", cfn);

    else
       {map_name(dcn, MAXLINE, cfn, "i", -1);

	rty   = dcl->type;
	na    = dcl->na;
	al    = dcl->al;
	voida = no_args(dcl);
	voidf = (strcmp(rty, "void") == 0);
	oper  = (voidf == TRUE) ? "subroutine" : "function";


	knd = mc_type(MAXLINE, fty, cty, wty, wnm, rty);
	mc_decl_list(a, MAXLINE, dcl);

	if (voidf == TRUE)
	   snprintf(cd, MAXLINE, "      %s %s(%s)", fty, dcn, a);
	else
	   snprintf(cd, MAXLINE, "      %s (%s) function %s(%s)",
		    fty, cty, dcn, a);
	femit(fp, cd, "&");

	snprintf(cb, MAXLINE, "bind(c, name='%s')", cfn);
	fprintf(fp, "                %s\n", cb);

	fprintf(fp, "         use iso_c_binding\n");
	fprintf(fp, "         implicit none\n");

/* argument declarations */
	nv = 0;
	for (i = 0; i < na; i++)
	    {if ((voida == TRUE) && (i == 0))
	        continue;

	     ty  = al[i].type;
	     nm  = al[i].name;
	     knd = mc_type(MAXLINE, fty, cty, wty, wnm, ty);
	     fprintf(fp, "         %s (%s), value :: %s\n",
		     fty, cty, nm);};

	fprintf(fp, "      end %s %s\n", oper, dcn);
	fprintf(fp, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_MODULE - generate Fortran/C interoperability interface
 *             - from SPR and SBI
 *             - return TRUE iff successful
 */

static int bind_module(FILE *fp, char *pck,
		       char **spr, char **sbi, char **swr)
   {int ib, iw, rv;
    char t[MAXLINE];
    char *sb, **ta;
    char *cfn, *ffn;
    fdecl *dcl;

    rv = TRUE;

/* make simple external declaration for variable argument functions */
    fprintf(fp, "! ... external declarations for generated wrappers\n");
    for (ib = 0; sbi[ib] != NULL; ib++)
        {sb = sbi[ib];
	 if (blank_line(sb) == FALSE)
	    {nstrncpy(t, MAXLINE, sb, -1);
	     ta = tokenize(t, " \t");
	     cfn = ta[0];
	     ffn = ta[1];
             dcl = find_proto(spr, cfn);
	     if (dcl != NULL)
	        {module_itf_wrap_ext(fp, pck, dcl, cfn, ffn);
		 free_decl(dcl);};

	     free_strings(ta);};};

    fprintf(fp, "\n");

/* make external declarations for variable argument pre-wrapped functions */
    if (swr != NULL)
       {fprintf(fp, "! ... external declarations for old wrappers\n");
        for (iw = 0; swr[iw] != NULL; iw++)
	    {sb = swr[iw];
	     if (blank_line(sb) == FALSE)
	        {nstrncpy(t, MAXLINE, sb, -1);
		 ta = tokenize(t, " \t");
		 module_pre_wrap_ext(fp, sb, ta, pck);
		 free_strings(ta);};};};

    fprintf(fp, "\n");

/* start the interface */
    fprintf(fp, "   interface\n");
    fprintf(fp, "\n");

/* make full interface for non-variable argument functions */
    fprintf(fp, "! ... declarations for generated wrappers\n");
    for (ib = 0; sbi[ib] != NULL; ib++)
        {sb = sbi[ib];
	 if (blank_line(sb) == FALSE)
	    {nstrncpy(t, MAXLINE, sb, -1);
	     ta = tokenize(t, " \t");
	     cfn = ta[0];
	     ffn = ta[1];
             dcl = find_proto(spr, cfn);
	     if (dcl != NULL)
	        {module_itf_wrap_full(fp, dcl, pck, cfn, ffn);
		 free_decl(dcl);};

	     free_strings(ta);};};

/* make full interface for non-variable argument pre-wrapped functions */
    if (swr != NULL)
       {fprintf(fp, "! ... declarations for old wrappers\n");
	for (iw = 0; swr[iw] != NULL; iw++)
	    {sb = swr[iw];
	     if (blank_line(sb) == FALSE)
	        {nstrncpy(t, MAXLINE, sb, -1);
		 ta = tokenize(t, " \t");
		 module_pre_wrap_full(fp, sb, ta, pck);
		 free_strings(ta);};};};

/* make full interface for non-variable argument functions */
    fprintf(fp, "! ... declarations for interoperability\n");
    for (ib = 0; sbi[ib] != NULL; ib++)
        {sb = sbi[ib];
	 if (blank_line(sb) == FALSE)
	    {nstrncpy(t, MAXLINE, sb, -1);
	     ta = tokenize(t, " \t");
	     cfn = ta[0];
	     ffn = ta[1];
             dcl = find_proto(spr, cfn);
	     if (dcl != NULL)
	        {module_interop_wrap(fp, dcl, cfn);
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

/* SCHEME_PROTO_LIST - render the arg list of DCL into A for the
 *                   - Scheme prototype
 */

static void scheme_proto_list(char *a, int nc, fdecl *dcl)
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

/* PYTHON_PROTO_LIST - render the arg list of DCL into A for the
 *                   - Python callable C wrapper
 */

static void python_proto_list(char *a, int nc, fdecl *dcl)
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

    fortran_proto_list(af, MAXLINE, dcl);
    scheme_proto_list(as, MAXLINE, dcl);
    python_proto_list(ap, MAXLINE, dcl);

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

static int blang(char *pck, int fif, char *fpr, char *fbi, char *fwr)
   {int rv;
    char **spr, **sbi, **swr;
    FILE *ff, *fm, *fs, *fp, *fd;

    if ((IS_NULL(fpr) == TRUE) || (IS_NULL(fbi) == TRUE))
       rv = FALSE;

    else
       {spr = file_text(fpr);
	sbi = file_text(fbi);
	swr = file_text(fwr);

	if (spr == NULL)
	   {printf("No prototypes found for '%s'\n", pck);
            rv = FALSE;}
        else if (sbi == NULL)
	   {printf("No bindings found for '%s'\n", pck);
            rv = FALSE;}
	else
	   {rv = TRUE;

	    add_derived_types(sbi);

	    if (spr != NULL)
	       {if (fif & 1)
		   ff = init_fortran(pck);
		if (fif & 2)
		   fm = init_module(pck);
		fs = init_scheme(pck);
		fp = init_python(pck);
		fd = init_doc(pck);

		if (sbi != NULL)
		   {if (fif & 1)
		       rv &= bind_fortran(ff, spr, sbi);
		    if (fif & 2)
		       rv &= bind_module(fm, pck, spr, sbi, swr);
		    rv &= bind_scheme(fs, pck, spr, sbi);
		    rv &= bind_python(fp, spr, sbi);
		    rv &= bind_doc(fd, spr, sbi);};

		if (fif & 1)
		   fin_fortran(ff, pck);
		if (fif & 2)
		   fin_module(fm, pck);
		fin_scheme(fs);
		fin_python(fp);
		fin_doc(fd);

		free_strings(spr);
		free_strings(sbi);
		free_strings(swr);};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it out here */

int main(int c, char **v)
   {int i, rv, fif;
    char pck[MAXLINE], msg[MAXLINE];
    char *fpr, *fwr, *fbi;

    fpr = "";
    fwr = "";
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
	        fbi = v[i];
	     else if (IS_NULL(fwr) == TRUE)
	        fwr = v[i];};};

    snprintf(pck, MAXLINE, "%s", path_base(path_tail(fbi)));
    snprintf(msg, MAXLINE, "%s bindings", pck);

    printf("      %s ", fill_string(msg, 25));

    init_types();

    rv = blang(pck, fif, fpr, fbi, fwr);
    rv = (rv != TRUE);

    printf("done\n");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

