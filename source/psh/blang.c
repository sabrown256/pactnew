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

/* #define RETURN_INTEGER */

#ifdef RETURN_INTEGER
#define C_PTR_RETURN "integer(isizea)"
#else
#define C_PTR_RETURN "type(C_PTR)"
#endif

enum e_langmode
   {MODE_C = 1, MODE_F, MODE_S, MODE_P };

typedef enum e_langmode langmode;

enum e_fparam
   {FP_ANY = -1, FP_VARARG, FP_FNC, FP_STRUCT_P, FP_SCALAR, FP_ARRAY};

typedef enum e_fparam fparam;

typedef struct s_bindes bindes;
typedef struct s_fdecl fdecl;
typedef struct s_farg farg;
typedef struct s_mtype mtype;

struct s_bindes
   {int cfl;
    char *pck;
    char **sbi;
    char **cpr;
    char **fpr;
    char **fwr;
    FILE *fp;
    void (*init)(bindes *bd, char *pck, int cfl,
		 char **sbi, char **cpr, char **fpr, char **fwr);
    int (*bind)(bindes *bd);
    void (*fin)(bindes *bd);};

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
    int ntf;
    char **tfproto;                   /* type, var fortran prototype pairs */
    farg *al;};

struct s_mtype
   {char *cty;
    char *fty;
    char *sty;
    char *pty;};

static char
 **mc_proto_list(fdecl *dcl);

static fparam
 fc_type(char *wty, int nc, char *ty, langmode mode);

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
	nstrncpy(type, nt, s, -1);

	rv = TRUE;}

    else if (strncmp(p, "PF", 2) == 0)
       {nstrncpy(name, nn, pn, -1);
	nstrncpy(type, nt, s, -1);

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
    char lty[MAXLINE];
    char **args;
    farg *al;

    na   = dcl->na;
    args = dcl->args;
    
    al = MAKE_N(farg, na);

    for (i = 0; i < na; i++)
        {al[i].arg  = args[i];
	 split_decl(al[i].type, MAXLINE,
		    al[i].name, MAXLINE,
		    al[i].arg);

	 al[i].knd = fc_type(lty, MAXLINE, al[i].type, MODE_C);};

    dcl->al = al;

/* fill in the Fortran prototype token pairs */
    mc_proto_list(dcl);

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

/* FIND_PROTO - find the prototype for F in CPR
 *            - return it iff successful
 */

static fdecl *find_proto(char **cpr, char *f)
   {int ip, na, nt;
    char pf[MAXLINE];
    char *p, *pa, *sp, *pro, *cfn;
    char **ty, **args;
    fdecl *dcl;

    pro = NULL;

    for (ip = 0; cpr[ip] != NULL; ip++)
        {sp = cpr[ip];
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
    free_strings(dcl->cargs);
    free_strings(dcl->tfproto);
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

    add_type("void *",        "C_PTR-A",      "SC_POINTER_I",       NULL);
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

/* MAP_NAME - map the C function name CF to form target language
 *          - unless the specified language binging name LF is non-NULL
 *          - or not "yes"
 *          - in which case return LF in D
 */

static char *map_name(char *d, int nc, char *cf, char *lf,
		      char *sfx, int cs, int us)
   {

    if ((lf != NULL) && (strcmp(lf, "none") != 0) && (strcmp(lf, "yes") != 0))
       {if ((sfx != NULL) && (sfx[0] == 'i'))
	   snprintf(d, nc, "%s_%s", lf, sfx);
	else
	   nstrncpy(d, nc, lf, -1);}
    else
       {if (sfx != NULL)
	   snprintf(d, nc, "%s_%s", cf, sfx);
	else
	   snprintf(d, nc, "%s", cf);};

    switch (cs)
       {case -1 :
	     downcase(d);
	     break;
	case 1 :
	     upcase(d);
	     break;};

/* map underscore to dash */
    if (us == TRUE)
       memmove(d, subst(d, "_", "-", -1), nc);

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
/*--------------------------------------------------------------------------*/

/* SETUP_BINDER - initialize the data members of BD */

static void setup_binder(bindes *bd, FILE *fp, char *pck, int cfl,
			 char **sbi, char **cpr, char **fpr, char **fwr)
   {

    if (bd != NULL)
       {bd->pck = pck;
	bd->fp  = fp;
	bd->cfl = cfl;
	bd->sbi = sbi;
	bd->cpr = cpr;
	bd->fpr = fpr;
	bd->fwr = fwr;};

    return;}

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
       {nstrncpy(wty, nc, ty, -1);
	rv = FP_FNC;}

/* follow the PACT function pointer PF convention */
    else if (strncmp(ty, "PF", 2) == 0)
       {nstrncpy(wty, nc, ty, -1);
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
	   rv = FP_STRUCT_P;
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
    char *ty, *nm, **cargs;
    farg *al;

    na = dcl->na;
    al = dcl->al;

    a[0] = '\0';
    if ((na == 0) || (no_args(dcl) == TRUE))
       nstrcat(a, MAXLINE, "void");
    else
       {for (i = 0; i < na; i++)
	    {nm  = al[i].name;
	     ty  = al[i].type;
	     knd = al[i].knd;

	     switch (knd)
	        {case FP_FNC :
		      vstrcat(a, MAXLINE, "%s, ", ty);
 		      break;
		 case FP_ARRAY :
		      if ((dcl->nc == 0) && (strcmp(ty, "char *") == 0))
			 dcl->cargs = lst_push(dcl->cargs, nm);
		      vstrcat(a, MAXLINE, "%s%s, ", ty, nm);
		      break;
		 case FP_SCALAR :
		      vstrcat(a, MAXLINE, "%s *%s, ", ty, nm);
		      break;
		 case FP_STRUCT_P :
		      vstrcat(a, MAXLINE, "%s *%s, ", ty, nm);
		      break;
		 default :
		      printf("Unknown type: %s\n", ty);
		      break;};

	   al[i].knd = knd;};

	cargs = dcl->cargs;
	nca   = lst_length(cargs);
        for (i = 0; i < nca; i++)
	    vstrcat(a, MAXLINE, "int snc%s, ", cargs[i]);
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

static void fc_call_list(char *a, int nc, fdecl *dcl, int local)
   {int i, na, voida;
    farg *al;

    na = dcl->na;
    al = dcl->al;

    voida = no_args(dcl);

    a[0] = '\0';
    if (voida == FALSE)
       {for (i = 0; i < na; i++)
	    {if ((al[i].knd == FP_FNC) && (local == FALSE))
	        vstrcat(a, MAXLINE, "%s, ", al[i].name);
	     else
	        vstrcat(a, MAXLINE, "_l%s, ", al[i].name);};

	a[strlen(a) - 2] = '\0';};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_FORTRAN - initialize Fortran wrapper file */

static void init_fortran(bindes *bd, char *pck, int cfl,
			 char **sbi, char **cpr, char **fpr, char **fwr)
   {int nc;
    char fn[MAXLINE], ufn[MAXLINE], fill[MAXLINE];
    FILE *fp;

    fp = NULL;

    if (cfl & 1)
       {snprintf(fn, MAXLINE, "gf-%s.c", pck);
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

	csep(fp);};

    setup_binder(bd, fp, pck, cfl, sbi, cpr, fpr, fwr);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FORTRAN_WRAP_DECL - function declaration */

static void fortran_wrap_decl(FILE *fp, fdecl *dcl,
			      fparam knd, char *rt, char *cfn, char *ffn)
   {char ucn[MAXLINE], dcn[MAXLINE], a[MAXLINE], t[MAXLINE];

    map_name(dcn, MAXLINE, cfn, ffn, "f", -1, FALSE);

    nstrncpy(ucn, MAXLINE, dcn, -1);
    upcase(ucn);

    fc_decl_list(a, MAXLINE, dcl);

    fprintf(fp, "\n");

    switch (knd)
       {case FP_STRUCT_P :

#ifdef RETURN_INTEGER
	     snprintf(t, MAXLINE, "FIXNUM FF_ID(%s, %s)(%s)\n",
		      dcn, ucn, a);
#else
	     snprintf(t, MAXLINE, "void *FF_ID(%s, %s)(%s)\n",
		      dcn, ucn, a);
#endif
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
   {int i, na, nv, voida;
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
	    snprintf(t, MAXLINE, "   {");
	 else
	    snprintf(t, MAXLINE, "    ");

/* variable for return value */
	 if (i == na)
	    {if (voidf == FALSE)
		{switch (knd)
		    {case FP_STRUCT_P :
#ifdef RETURN_INTEGER
		          nstrcat(t, MAXLINE, "FIXNUM _rv;\n");
			  vstrcat(t, MAXLINE, "    SC_address _ad%s;\n",
				  dcl->name);
#else
		          nstrcat(t, MAXLINE, "void *_rv;\n");
#endif
			  break;
		     case FP_ARRAY :
			  vstrcat(t, MAXLINE, "%s *_rv;\n", rt);
			  break;
		     case FP_SCALAR :
		          vstrcat(t, MAXLINE, "%s _rv;\n", rt);
			  break;
		     default :
		          vstrcat(t, MAXLINE, "%s _rv;\n", rt);
			  break;};};}

/* local vars */
	 else if (nm[0] != '\0')
	    {ty = al[i].type;
	     switch (al[i].knd)
		{case FP_FNC :
                      if (nv > 0)
			 t[0] = '\0';
		      else
			 nstrcat(t, MAXLINE, "\n");
		      break;
		 case FP_STRUCT_P :
		      vstrcat(t, MAXLINE, "%s _l%s;\n", ty, nm);
		      break;
		 case FP_ARRAY :
		      if (strcmp(ty, "char *") == 0)
			 vstrcat(t, MAXLINE, "char _l%s[MAXLINE];\n", nm);
		      else
			 vstrcat(t, MAXLINE, "%s_l%s;\n", ty, nm);
		      break;
		 case FP_SCALAR :
		      vstrcat(t, MAXLINE, "%s _l%s;\n", ty, nm);
		      break;
		 default :
		      vstrcat(t, MAXLINE, "%s _l%s;\n", ty, nm);
		      break;};
	     nv++;};

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
		      t[0] = '\0';
		      break;
		 case FP_STRUCT_P :
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

	     if (IS_NULL(t) == FALSE)
	        {nv++;
		 fputs(t, fp);};};};

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

    fc_call_list(a, MAXLINE, dcl, FALSE);

    nm = dcl->name;
    if (voidf == FALSE)
       {switch (knd)
	   {case FP_STRUCT_P :
#ifdef RETURN_INTEGER
	         fprintf(fp, "    _ad%s.memaddr = (void *) %s(%s);\n",
			 nm, nm, a);
#else
	         fprintf(fp, "    _rv = (void *) %s(%s);\n",
			 nm, a);
#endif
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
   {

    if (voidf == FALSE)
       {switch (knd)
	   {case FP_STRUCT_P :
#ifdef RETURN_INTEGER
	         fprintf(fp, "    _rv = _ad%s.diskaddr;\n\n", dcl->name);
#endif
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
	fortran_wrap_decl(fp, dcl, knd, rt, cfn, ffn);

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

/* BIND_FORTRAN - generate Fortran bindings from CPR and SBI
 *              - return TRUE iff successful
 */

static int bind_fortran(bindes *bd)
   {int ib, rv;
    char t[MAXLINE];
    char *sb, **ta;
    char *cfn, *ffn;
    fdecl *dcl;
    FILE *fp;
    char **cpr, **sbi;

    rv = TRUE;

    if (bd->cfl & 1)
       {fp  = bd->fp;
	sbi = bd->sbi;
	cpr = bd->cpr;

	for (ib = 0; sbi[ib] != NULL; ib++)
	    {sb = sbi[ib];
	     if (blank_line(sb) == FALSE)
	        {nstrncpy(t, MAXLINE, sb, -1);
		 ta = tokenize(t, " \t");
		 cfn = ta[0];
		 ffn = ta[1];
		 dcl = find_proto(cpr, cfn);
		 if (dcl != NULL)
		    {fortran_wrap(fp, dcl, cfn, ffn);
		     free_decl(dcl);};

		 free_strings(ta);};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_FORTRAN - finalize Fortran file */

static void fin_fortran(bindes *bd)
   {FILE *fp;

    fp = bd->fp;

    csep(fp);
    fclose(fp);

    bd->fp = NULL;

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
		      char *wty, char *ty)
   {fparam knd;
    char lfty[MAXLINE], lcty[MAXLINE], lwty[MAXLINE];

    knd = fc_type(lwty, nc, ty, MODE_F);
    memmove(lwty, subst(lwty, "FIXNUM", "integer", -1), nc);

    if (strcmp(ty, "...") == 0)
       {nstrncpy(lfty, MAXLINE, "error", -1);
	nstrncpy(lcty, MAXLINE, "error", -1);}

/* follow the PACT function pointer PF convention */
    else if ((strstr(ty, "(*") != NULL) ||
	     (strncmp(ty, "PF", 2) == 0))
       {nstrncpy(lfty, MAXLINE, "type", -1);
	nstrncpy(lcty, MAXLINE, "C_FUNPTR", -1);}

    else if (is_ptr(ty) == TRUE)
       {nstrncpy(lfty, MAXLINE, "type", -1);
	nstrncpy(lcty, MAXLINE, "C_PTR", -1);}

    else if (strstr(ty, "long long") != NULL)
       {nstrncpy(lfty, MAXLINE, "integer", -1);
	nstrncpy(lcty, MAXLINE, "C_LONG_LONG", -1);}

    else if (strstr(ty, "long") != NULL)
       {nstrncpy(lfty, MAXLINE, "integer", -1);
	nstrncpy(lcty, MAXLINE, "C_LONG", -1);}

    else if (strstr(ty, "short") != NULL)
       {nstrncpy(lfty, MAXLINE, "integer", -1);
	nstrncpy(lcty, MAXLINE, "C_SHORT", -1);}

    else if (strstr(ty, "int") != NULL)
       {nstrncpy(lfty, MAXLINE, "integer", -1);
	nstrncpy(lcty, MAXLINE, "C_INT", -1);}

    else if (strstr(ty, "signed char") != NULL)
       {nstrncpy(lfty, MAXLINE, "integer", -1);
	nstrncpy(lcty, MAXLINE, "C_SIGNED_CHAR", -1);}

    else if (strstr(ty, "size_t") != NULL)
       {nstrncpy(lfty, MAXLINE, "integer", -1);
	nstrncpy(lcty, MAXLINE, "C_SIZE_T", -1);}

    else if (strstr(ty, "long double _Complex") != NULL)
       {nstrncpy(lfty, MAXLINE, "complex", -1);
	nstrncpy(lcty, MAXLINE, "C_LONG_DOUBLE_COMPLEX", -1);}

    else if (strstr(ty, "double _Complex") != NULL)
       {nstrncpy(lfty, MAXLINE, "complex", -1);
	nstrncpy(lcty, MAXLINE, "C_DOUBLE_COMPLEX", -1);}

    else if (strstr(ty, "float _Complex") != NULL)
       {nstrncpy(lfty, MAXLINE, "complex", -1);
	nstrncpy(lcty, MAXLINE, "C_FLOAT_COMPLEX", -1);}

    else if (strstr(ty, "long double") != NULL)
       {nstrncpy(lfty, MAXLINE, "real", -1);
	nstrncpy(lcty, MAXLINE, "C_LONG_DOUBLE", -1);}

    else if (strstr(ty, "double") != NULL)
       {nstrncpy(lfty, MAXLINE, "real", -1);
	nstrncpy(lcty, MAXLINE, "C_DOUBLE", -1);}

    else if (strstr(ty, "float") != NULL)
       {nstrncpy(lfty, MAXLINE, "real", -1);
	nstrncpy(lcty, MAXLINE, "C_FLOAT", -1);}

    else if (strstr(ty, "_Bool") != NULL)
       {nstrncpy(lfty, MAXLINE, "logical", -1);
	nstrncpy(lcty, MAXLINE, "C_BOOL", -1);}

    else if (strcmp(ty, "void") == 0)
       {nstrncpy(lfty, MAXLINE, "subroutine", -1);
	nstrncpy(lcty, MAXLINE, "C_VOID", -1);}

/* take unknown types to be integer - covers enums */
    else
       {nstrncpy(lfty, MAXLINE, "integer", -1);
	nstrncpy(lcty, MAXLINE, "C_INT", -1);};

    if (fty != NULL)
       nstrncpy(fty, nc, lfty, -1);

    if (cty != NULL)
       nstrncpy(cty, nc, lcty, -1);

    if (wty != NULL)
       nstrncpy(wty, nc, lwty, -1);

    return(knd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MC_PROTO_LIST - return list of tokens for type and name of argument
 *               - caller formats and frees list
 */

static char **mc_proto_list(fdecl *dcl)
   {int i, na, voida;
    fparam knd;
    char wty[MAXLINE];
    char **lst, *ty, *nm;
    farg *al;

    lst = NULL;

    na    = dcl->na;
    al    = dcl->al;
    voida = no_args(dcl);

/* function/return type */
    ty  = dcl->type;
    nm  = dcl->name;
    knd = mc_type(MAXLINE, NULL, NULL, wty, ty);
    switch (knd)
       {case FP_FNC      :
        case FP_STRUCT_P :
	     lst = lst_push(lst, C_PTR_RETURN);
	     lst = lst_push(lst, nm);
	     break;
        case FP_ARRAY :
	     if (strcmp(wty, "character") == 0)
	        {lst = lst_push(lst, "character(*)");
		 lst = lst_push(lst, nm);}
	     else if (strcmp(wty, "C_PTR") == 0)
	        {lst = lst_push(lst, C_PTR_RETURN);
		 lst = lst_push(lst, nm);}
	     else
	        {lst = lst_push(lst, wty);
		 lst = lst_push(lst, "%s(*)", nm);};
	     break;
        case FP_SCALAR :
        default        :
	     lst = lst_push(lst, wty);
	     lst = lst_push(lst, nm);
	     break;};

/* arguments */
    for (i = 0; i < na; i++)
        {if ((voida == TRUE) && (i == 0))
	    continue;

	 ty  = al[i].type;
	 nm  = al[i].name;
	 knd = mc_type(MAXLINE, NULL, NULL, wty, ty);
	 switch (knd)
	    {case FP_FNC      :
	     case FP_STRUCT_P :
	          lst = lst_push(lst, C_PTR_RETURN);
	          lst = lst_push(lst, nm);
		  break;
	     case FP_ARRAY :
	          if (strcmp(wty, "character") == 0)
		     {lst = lst_push(lst, "character(*)");
		      lst = lst_push(lst, nm);}
		  else if (strcmp(wty, "C_PTR") == 0)
		     {lst = lst_push(lst, C_PTR_RETURN);
		      lst = lst_push(lst, nm);}
		  else
		     {lst = lst_push(lst, wty);
		      lst = lst_push(lst, "%s(*)", nm);};
		  break;
	     case FP_SCALAR :
	     default        :
	          lst = lst_push(lst, wty);
	          lst = lst_push(lst, nm);
		  break;};};

    dcl->tfproto = lst;
    dcl->ntf     = lst_length(lst);

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MC_NEED_PTR - return TRUE if the package types module will be needed */

static int mc_need_ptr(fdecl *dcl)
   {int i, na, ok;
    char cty[MAXLINE];
    farg *al;

    na = dcl->na;
    al = dcl->al;

    ok = FALSE;

    if (mc_type(MAXLINE, NULL, cty, NULL, dcl->type) != FP_VARARG)
       ok |= ((strcmp(cty, "C_PTR") == 0) ||
	      (strcmp(cty, "C_FUNPTR") == 0));

    if ((na > 0) && (no_args(dcl) == FALSE))
       {for (i = 0; i < na; i++)
	    {if (mc_type(MAXLINE, NULL, cty, NULL, al[i].type) != FP_VARARG)
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
    char *nm;
    farg *al;

    na = dcl->na;
    al = dcl->al;

    a[0] = '\0';
    if ((na > 0) && (no_args(dcl) == FALSE))
       {for (i = 0; i < na; i++)
	    {nm = al[i].name;

	     vstrcat(a, MAXLINE, "%s, ", nm);};

        a[strlen(a) - 2] = '\0';};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_MODULE - initialize Fortran/C interoperatbility module file */

static void init_module(bindes *bd, char *pck, int cfl,
			char **sbi, char **cpr, char **fpr, char **fwr)
   {int nc;
    char fn[MAXLINE], ufn[MAXLINE], fill[MAXLINE];
    FILE *fp;

    fp = NULL;

    if (cfl & 2)
       {snprintf(fn, MAXLINE, "gm-%s.f", pck);
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
	fprintf(fp, "\n");};

    setup_binder(bd, fp, pck, cfl, sbi, cpr, fpr, fwr);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MODULE_NATIVE_F - write interface for native Fortran function */

static char **module_native_f(FILE *fp, char **sa, char *pck)
   {int i, voidf;
    char *oper, *p;

/* skip past any leading blank lines */
    for ( ; sa != NULL; sa++)
        {p = *sa;
	 if (p == NULL)
	    return(NULL);
	 else if (IS_NULL(p) == FALSE)
	    break;};

    oper = NULL;

    for (i = 0 ; sa != NULL; sa++, i++)
        {p = *sa;

	 if (IS_NULL(p) == TRUE)
	    {sa++;
	     break;};

	 if (i == 0)
	    {fprintf(fp, "%s\n", p);
	     voidf = (strstr(p, "subroutine") != NULL);
	     oper  = (voidf == TRUE) ? "subroutine" : "function";

	     fprintf(fp, "         use iso_c_binding\n");
	     fprintf(fp, "         implicit none\n");}
	 else
	    fprintf(fp, "   %s\n", p);};

    if (oper != NULL)
       fprintf(fp, "      end %s\n", oper);

    fprintf(fp, "\n");

    return(sa);}

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
	  (strstr(pr, "type(C_PTR)") == NULL) &&
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
   {int i, nt, voidf;
    char a[MAXLINE], t[MAXLINE];
    char *rty, *oper, *dcn;

    if ((ta != NULL) && (module_pre_wrappable(pr) == TRUE))
       {nt = lst_length(ta);

	rty   = ta[0];
	dcn   = ta[1];
	voidf = (strcmp(rty, "void") == 0);
	oper  = (voidf == TRUE) ? "subroutine" : "function";

	a[0] = '\0';
        for (i = 3; i < nt; i += 2)
	    vstrcat(a, MAXLINE, "%s, ", ta[i]);
	if (nt > 2)
	   a[strlen(a) - 2] = '\0';

	snprintf(t, MAXLINE, "      %s %s(%s)", oper, dcn, a);
	femit(fp, t, "");

	fprintf(fp, "         use iso_c_binding\n");
	fprintf(fp, "         use types_%s\n", pck);
	fprintf(fp, "         implicit none\n");
	if (voidf == FALSE)
	   fprintf(fp, "         %-12s :: %s\n", rty, dcn);

	for (i = 2; i < nt; i += 2)
	    {if (strcmp(ta[i], "character") == 0)
		fprintf(fp, "         %s(*) :: %s\n", ta[i], ta[i+1]);
             else if (strcmp(ta[i], "integer-A") == 0)
                fprintf(fp, "         %-12s :: %s(*)\n", "integer", ta[i+1]);
             else if (strcmp(ta[i], "real8-A") == 0)
                fprintf(fp, "         %-12s :: %s(*)\n", "real*8", ta[i+1]);
             else
                fprintf(fp, "         %-12s :: %s\n", ta[i], ta[i+1]);};

	fprintf(fp, "      end %s %s\n", oper, dcn);
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
   {char dcn[MAXLINE], fty[MAXLINE], cty[MAXLINE];
    char *rty;
    static int first = TRUE;

/* declare the incomplete ones as external */
    if ((module_itf_wrappable(dcl) == FALSE) && (strcmp(ffn, "none") != 0))
       {map_name(dcn, MAXLINE, cfn, ffn, "f", -1, FALSE);

	if (mc_need_ptr(dcl) == TRUE)
	   {if (first == TRUE)
	       {first = FALSE;
		fprintf(fp, "   use types_%s\n", pck);
		fprintf(fp, "\n");};};

	rty = dcl->type;
	mc_type(MAXLINE, fty, cty, NULL, rty);
	if (strcmp(rty, "void") == 0)
	   fprintf(fp, "   external :: %s\n", dcn);
        else
	  {if ((strcmp(cty, "C_PTR") == 0) || (strcmp(cty, "C_FUNPTR") == 0))
	      fprintf(fp, "   %s, external :: %s\n",
		      C_PTR_RETURN, dcn);
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
   {int i, ns, voidf;
    char dcn[MAXLINE], a[MAXLINE], t[MAXLINE];
    char fty[MAXLINE], cty[MAXLINE];
    char *rty, *oper, **args;

/* emit complete declarations */
    if ((module_itf_wrappable(dcl) == TRUE) && (strcmp(ffn, "none") != 0))
       {map_name(dcn, MAXLINE, cfn, ffn, "f", -1, FALSE);

	rty   = dcl->type;
	voidf = (strcmp(rty, "void") == 0);
	oper  = (voidf == TRUE) ? "subroutine" : "function";

	mc_type(MAXLINE, fty, cty, NULL, rty);
	mc_decl_list(a, MAXLINE, dcl);

	snprintf(t, MAXLINE, "      %s %s(%s)", oper, dcn, a);
	femit(fp, t, "");

	fprintf(fp, "         use iso_c_binding\n");
	if (mc_need_ptr(dcl) == TRUE)
	   fprintf(fp, "         use types_%s\n", pck);

	fprintf(fp, "         implicit none\n");

/* return value declaration */
	if (voidf == FALSE)
	   {if ((strcmp(cty, "C_PTR") == 0) ||
		(strcmp(cty, "C_FUNPTR") == 0))
	       snprintf(t, MAXLINE, "         %-15s :: %s\n",
			C_PTR_RETURN, dcn);
	    else
	       snprintf(t, MAXLINE, "         %-15s :: %s\n", fty, dcn);

	    fputs(t, fp);};

/* argument declarations */
	args = dcl->tfproto;
	ns   = dcl->ntf;
	for (i = 2; i < ns; i += 2)
	    {snprintf(t, MAXLINE, "         %-15s :: %s\n",
		      args[i], args[i+1]);
	     fputs(t, fp);};

	fprintf(fp, "      end %s %s\n", oper, dcn);

	fprintf(fp, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MODULE_INTEROP_WRAP - generate FORTRAN/C interoperability interface for
 *                     - C function CFN
 */

static void module_interop_wrap(FILE *fp, fdecl *dcl, char *cfn, char *ffn)
   {int i, na, voidf, voida;
    char dcn[MAXLINE], a[MAXLINE];
    char fty[MAXLINE], cty[MAXLINE];
    char cd[MAXLINE], cb[MAXLINE];
    char *nm, *ty, *rty, *oper;
    farg *al;

    if (strstr(dcl->proto, "...") != NULL)
       berr("%s is not interoperable - variable args", cfn);

    else
       {map_name(dcn, MAXLINE, cfn, ffn, "i", -1, FALSE);

	rty   = dcl->type;
	na    = dcl->na;
	al    = dcl->al;
	voida = no_args(dcl);
	voidf = (strcmp(rty, "void") == 0);
	oper  = (voidf == TRUE) ? "subroutine" : "function";

	mc_type(MAXLINE, fty, cty, NULL, rty);
	mc_decl_list(a, MAXLINE, dcl);

	snprintf(cd, MAXLINE, "      %s %s(%s)", oper, dcn, a);
	femit(fp, cd, "&");

	snprintf(cb, MAXLINE, "bind(c, name='%s')", cfn);
	fprintf(fp, "                %s\n", cb);

	fprintf(fp, "         use iso_c_binding\n");
	fprintf(fp, "         implicit none\n");
	if (voidf == FALSE)
	   fprintf(fp, "         %s(%s) :: %s\n", fty, cty, dcn);

/* argument declarations */
	for (i = 0; i < na; i++)
	    {if ((voida == TRUE) && (i == 0))
	        continue;

	     ty = al[i].type;
	     nm = al[i].name;
	     mc_type(MAXLINE, fty, cty, NULL, ty);
	     fprintf(fp, "         %s(%s), value :: %s\n",
		     fty, cty, nm);};

	fprintf(fp, "      end %s %s\n", oper, dcn);
	fprintf(fp, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_MODULE - generate Fortran/C interoperability interface
 *             - from CPR and SBI
 *             - return TRUE iff successful
 */

static int bind_module(bindes *bd)
   {int ib, iw, rv;
    char t[MAXLINE];
    char *sb, **ta;
    char *cfn, *ffn;
    fdecl *dcl;
    FILE *fp;
    char *pck, **cpr, **fpr, **sbi, **fwr, **sa;

    rv = TRUE;

    if (bd->cfl & 2)
       {fp  = bd->fp;
	pck = bd->pck;
	sbi = bd->sbi;
	cpr = bd->cpr;
	fpr = bd->fpr;
	fwr = bd->fwr;

/* make simple external declaration for variable argument functions */
	fprintf(fp, "! ... external declarations for generated wrappers\n");
	for (ib = 0; sbi[ib] != NULL; ib++)
	    {sb = sbi[ib];
	     if (blank_line(sb) == FALSE)
	        {nstrncpy(t, MAXLINE, sb, -1);
		 ta = tokenize(t, " \t");
		 cfn = ta[0];
		 ffn = ta[1];
		 dcl = find_proto(cpr, cfn);
		 if (dcl != NULL)
		    {module_itf_wrap_ext(fp, pck, dcl, cfn, ffn);
		     free_decl(dcl);};

		 free_strings(ta);};};

	fprintf(fp, "\n");

/* make external declarations for variable argument pre-wrapped functions */
	if (fwr != NULL)
	   {fprintf(fp, "! ... external declarations for old wrappers\n");
	    for (iw = 0; fwr[iw] != NULL; iw++)
	        {sb = fwr[iw];
		 if (blank_line(sb) == FALSE)
		    {nstrncpy(t, MAXLINE, sb, -1);
		     ta = tokenize(t, " \t");
		     module_pre_wrap_ext(fp, sb, ta, pck);
		     free_strings(ta);};};};

	fprintf(fp, "\n");

/* start the interface */
	fprintf(fp, "   interface\n");
	fprintf(fp, "\n");

/* make interface for native Fortran functions */
	if (fpr != NULL)
	   {fprintf(fp, "! ... declarations for native Fortran functions\n");
	    for (iw = 0, sa = fpr; TRUE; iw++)
	        {sa = module_native_f(fp, sa, pck);
		 if (sa == NULL)
		    break;};};

/* make full interface for non-variable argument functions */
	fprintf(fp, "! ... declarations for generated wrappers\n");
	for (ib = 0; sbi[ib] != NULL; ib++)
	    {sb = sbi[ib];
	     if (blank_line(sb) == FALSE)
	        {nstrncpy(t, MAXLINE, sb, -1);
		 ta = tokenize(t, " \t");
		 cfn = ta[0];
		 ffn = ta[1];
		 dcl = find_proto(cpr, cfn);
		 if (dcl != NULL)
		    {module_itf_wrap_full(fp, dcl, pck, cfn, ffn);
		     free_decl(dcl);};

		 free_strings(ta);};};

/* make full interface for non-variable argument pre-wrapped functions */
	if (fwr != NULL)
	   {fprintf(fp, "! ... declarations for old wrappers\n");
	    for (iw = 0; fwr[iw] != NULL; iw++)
	        {sb = fwr[iw];
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
		 dcl = find_proto(cpr, cfn);
		 if (dcl != NULL)
		    {module_interop_wrap(fp, dcl, cfn, ffn);
		     free_decl(dcl);};

		 free_strings(ta);};};

/* finish the interface */
	fprintf(fp, "   end interface\n");
	fprintf(fp, "\n");};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_MODULE - finalize Fortran/C interoperability interface module file */

static void fin_module(bindes *bd)
   {char *pck;
    FILE *fp;

    fp  = bd->fp;
    pck = bd->pck;

    fprintf(fp, "end module pact_%s\n", pck);
    fprintf(fp, "\n");

    fclose(fp);

    bd->fp = NULL;

    return;}

/*--------------------------------------------------------------------------*/

/*                             SCHEME ROUTINES                              */

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

/* SO_TYPE - return the Scheme object constructor for C type TY */

static fparam so_type(char *a, int nc, char *ty)
   {fparam rv;

    rv = FP_SCALAR;

    if (strcmp(ty, "char") == 0)
       nstrncpy(a, nc, "SS_mk_char", -1);
       
    else if (strcmp(ty, "char *") == 0)
       nstrncpy(a, nc, "SS_mk_string", -1);
       
    else if (strcmp(ty, "bool") == 0)
       nstrncpy(a, nc, "SS_mk_boolean", -1);
       
    else if ((strcmp(ty, "short") == 0) ||
	     (strcmp(ty, "int") == 0) ||
	     (strcmp(ty, "long") == 0) ||
	     (strcmp(ty, "long long") == 0) ||
	     (strcmp(ty, "int16_t") == 0) ||
	     (strcmp(ty, "int32_t") == 0) ||
	     (strcmp(ty, "int64_t") == 0))
       nstrncpy(a, nc, "SS_mk_integer", -1);
       
    else if ((strcmp(ty, "float") == 0) ||
	     (strcmp(ty, "double") == 0) ||
	     (strcmp(ty, "long double") == 0))
       nstrncpy(a, nc, "SS_mk_float", -1);
       
    else if ((strcmp(ty, "float _Complex") == 0) ||
	     (strcmp(ty, "double _Complex") == 0) ||
	     (strcmp(ty, "long double _Complex") == 0))
       nstrncpy(a, nc, "SS_mk_complex", -1);
       
    else if (is_ptr(ty) == TRUE)
       {rv = FP_ARRAY;
	nstrncpy(a, nc, "SX_mk_C_array", -1);}

/* handle unknown types with a mostly graceful failure */
    else
       {rv = FP_ANY;
	nstrncpy(a, nc, "none", -1);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_SCHEME - initialize Scheme file */

static void init_scheme(bindes *bd, char *pck, int cfl,
			char **sbi, char **cpr, char **fpr, char **fwr)
   {FILE *fp;

    fp = open_file("w", "gs-%s.c", pck);

    fprintf(fp, "\n");
    fprintf(fp, "#include \"sx_int.h\"\n");
    fprintf(fp, "#include \"%s_int.h\"\n", pck);
    fprintf(fp, "\n");

    csep(fp);

    setup_binder(bd, fp, pck, cfl, sbi, cpr, fpr, fwr);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CALL_LIST - render the arg list of DCL into A for the
 *              - SCHEME function call
 */

static void sc_call_list(char *a, int nc, fdecl *dcl)
   {int i, na, voida;
    farg *al;

    na = dcl->na;
    al = dcl->al;

    voida = no_args(dcl);

    a[0] = '\0';
    if (voida == FALSE)
       {for (i = 0; i < na; i++)
	    vstrcat(a, MAXLINE, "%s ", al[i].name);

	a[strlen(a) - 1] = '\0';};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_DECL - function declaration */

static void scheme_wrap_decl(FILE *fp, fdecl *dcl)
   {int na;
    char dcn[MAXLINE];

    na = dcl->na;

    nstrncpy(dcn, MAXLINE, dcl->name, -1);
    downcase(dcn);

    fprintf(fp, "\n");

    fprintf(fp, "static object *");
    fprintf(fp, "_SXI_%s", dcn);
    if (na == 0)
       fprintf(fp, "(SS_psides *si)");
    else
       fprintf(fp, "(SS_psides *si, object *argl)");

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_LOCAL_DECL - local variable declarations */

static void scheme_wrap_local_decl(FILE *fp, fdecl *dcl,
				   fparam knd, char *so, int voida)
   {int i, na, nv, voidf;
    char t[MAXLINE];
    char *ty, *nm, *rty;
    farg *al;

    na = dcl->na;
    al = dcl->al;

    rty = dcl->type;

    voidf = (strcmp(dcl->type, "void") == 0);
	
    nv = 0;
    for (i = 0; i <= na; i++)
        {if ((voida == TRUE) && (i == 0))
	    continue;

	 ty  = al[i].type;
	 nm  = al[i].name;

	 if (nv == 0)
	    snprintf(t, MAXLINE, "   {");
	 else
	    snprintf(t, MAXLINE, "    ");

/* variable for return value */
	 if (i == na)
	    {if (voidf == FALSE)
	        {vstrcat(t, MAXLINE, "%s _rv;\n    ", rty);
		 if (knd == FP_ARRAY)
		    {nstrcat(t, MAXLINE, "long _sz;\n");
		     nstrcat(t, MAXLINE, "    C_array *_arr;\n    ");};};

	     nstrcat(t, MAXLINE, "object *_lo;\n");}

/* local vars */
	 else if (nm != '\0')
	    {if (al[i].knd == FP_FNC)
	        vstrcat(t, MAXLINE, "PFInt _l%s;\n", nm);
	     else
	        vstrcat(t, MAXLINE, "%s _l%s;\n", ty, nm);
	     nv++;};

	 if (IS_NULL(t) == FALSE)
	    fputs(subst(t, "* ", "*", -1), fp);};

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_LOCAL_ASSN - local variable assignments */

static void scheme_wrap_local_assn(FILE *fp, fdecl *dcl, int voida)
   {int i, na;
    char a[MAXLINE], ty[MAXLINE];
    farg *al;

    if (voida == FALSE)
       {na = dcl->na;
	al = dcl->al;

	a[0] = '\0';
	for (i = 0; i < na; i++)
	    {cs_type(ty, MAXLINE, al[i].type);
	     vstrcat(a, MAXLINE, "            %s, &_l%s,\n",
		     ty, al[i].name);};

	fprintf(fp, "    SS_args(si, argl,\n");
	fprintf(fp, "%s", a);
	fprintf(fp, "            0);\n");
	fprintf(fp, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_LOCAL_CALL - function call */

static void scheme_wrap_local_call(FILE *fp, fdecl *dcl)
   {char a[MAXLINE], t[MAXLINE];
    char *ty, *nm;

    ty = dcl->type;
    nm = dcl->name;

    fc_call_list(a, MAXLINE, dcl, TRUE);

    if (strcmp(ty, "void") == 0)
       snprintf(t, MAXLINE, "    %s(%s);\n", nm, a);
    else
       snprintf(t, MAXLINE, "    _rv = %s(%s);\n", nm, a);

    fputs(t, fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_LOCAL_RETURN - function return */

static void scheme_wrap_local_return(FILE *fp, fdecl *dcl,
				     fparam knd, char *so)
   {char t[MAXLINE], dty[MAXLINE];
    char *ty;

    ty = dcl->type;

    if (strcmp(ty, "void") == 0)
       snprintf(t, MAXLINE, "    _lo = SS_null;\n");
    else
       {if (IS_NULL(so) == FALSE)
	   {switch (knd)
	       {case FP_ANY :
		     snprintf(t, MAXLINE,
			      "\n/* no way to return '%s' */\n", ty);
		     nstrcat(t, MAXLINE, "    _lo = SS_null;\n");
		     break;

	        case FP_ARRAY :
		     deref(dty, MAXLINE, ty);
		     if (strcmp(dty, "void") == 0)
		        {snprintf(t, MAXLINE,
				  "    _sz = SC_arrlen(_rv);\n");
			 vstrcat(t, MAXLINE,
				 "    _arr = PM_make_array(\"char\", _sz, _rv);\n");}
		     else
		        {snprintf(t, MAXLINE,
				  "    _sz = SC_arrlen(_rv)/sizeof(%s);\n",
				  dty);
			 vstrcat(t, MAXLINE,
				 "    _arr = PM_make_array(\"%s\", _sz, _rv);\n",
				 dty);};
		     vstrcat(t, MAXLINE, "    _lo  = %s(si, _arr);\n", so);
		     break;
	        default :
                     if (strcmp(ty, "bool") == 0)
		        snprintf(t, MAXLINE, "    _lo = %s(si, \"g\", (int) _rv);\n",
				 so);
		     else
		        snprintf(t, MAXLINE, "    _lo = %s(si, _rv);\n", so);
		     break;};}
	else
	   snprintf(t, MAXLINE, "    _lo = SS_null;\n");};
    fputs(t, fp);

    fprintf(fp, "\n");

    fprintf(fp, "    return(_lo);}\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_INSTALL - add the installation of the function */

static char **scheme_wrap_install(char **fl, fdecl *dcl, char *sfn,
				  int voida, char **com)
   {char a[MAXLINE], t[MAXLINE], dcn[MAXLINE], ifn[MAXLINE];
    char *pi;

    nstrncpy(dcn, MAXLINE, dcl->name, -1);
    downcase(dcn);

/* make the scheme interpreter name */
    pi = (strcmp(sfn, "yes") == 0) ? dcl->name : sfn;
    map_name(ifn, MAXLINE, pi, sfn, NULL, -1, TRUE);

/* prepare the function inline documenation */
    concatenate(t, MAXLINE, com, " ");
    if (IS_NULL(t) == TRUE)
       {sc_call_list(a, MAXLINE, dcl);
	if (voida == FALSE)
	   snprintf(t, MAXLINE, "Procedure: %s\\n     Usage: (%s %s)",
		    ifn, ifn, a);
	else
	   snprintf(t, MAXLINE, "Procedure: %s\\n     Usage: (%s)",
		    ifn, ifn);};

/* add the installation of the function */
    if (voida == FALSE)
       snprintf(a, MAXLINE, 
		"    SS_install(si, \"%s\",\n               \"%s\",\n               SS_nargs,\n               _SXI_%s, SS_PR_PROC);\n\n",
		ifn, t, dcn);
    else
       snprintf(a, MAXLINE, 
		"    SS_install(si, \"%s\",\n               \"%s\",\n               SS_zargs,\n               _SXI_%s, SS_PR_PROC);\n\n",
		ifn, t, dcn);

    fl = lst_add(fl, a);

    return(fl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP - wrap C function DCL in SCHEME callable function
 *             - using name FFN
 */

static char **scheme_wrap(FILE *fp, char **fl, fdecl *dcl,
			  char *sfn, char **com)
   {int voida;
    fparam knd;
    char so[MAXLINE];

    if (strcmp(sfn, "none") != 0)
       {voida = no_args(dcl);

	csep(fp);

	knd = so_type(so, MAXLINE, dcl->type);
	
/* function declaration */
	scheme_wrap_decl(fp, dcl);

/* local variable declarations */
	scheme_wrap_local_decl(fp, dcl, knd, so, voida);

/* local variable assignments */
	scheme_wrap_local_assn(fp, dcl, voida);

/* function call */
	scheme_wrap_local_call(fp, dcl);

/* function return */
	scheme_wrap_local_return(fp, dcl, knd, so);

	csep(fp);

/* add the installation of the function */
	fl = scheme_wrap_install(fl, dcl, sfn, voida, com);};

    return(fl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_INSTALL - write the routine to install the bindings */

static void scheme_install(FILE *fp, char *pck, char **fl)
    {int i;

     csep(fp);

     fprintf(fp, "\n");
     fprintf(fp, "void SX_install_%s_bindings(SS_psides *si)\n", pck);
     fprintf(fp, "   {\n");
     fprintf(fp, "\n");

     if (fl != NULL)
        {for (i = 0; fl[i] != NULL; i++)
	     fputs(fl[i], fp);

	 free_strings(fl);};

     fprintf(fp, "   return;}\n");
     fprintf(fp, "\n");
     csep(fp);

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_SCHEME - generate Scheme bindings from CPR and SBI
 *             - return TRUE iff successful
 */

static int bind_scheme(bindes *bd)
   {int ib, rv;
    char t[MAXLINE];
    char *sb, **ta, **fl;
    char *cfn, *sfn;
    fdecl *dcl;
    FILE *fp;
    char *pck, **cpr, **sbi;

    fp  = bd->fp;
    pck = bd->pck;
    sbi = bd->sbi;
    cpr = bd->cpr;

    rv = TRUE;
    fl = NULL;

    for (ib = 0; sbi[ib] != NULL; ib++)
        {sb = sbi[ib];
	 if (blank_line(sb) == FALSE)
	    {nstrncpy(t, MAXLINE, sb, -1);
	     ta = tokenize(t, " \t");
	     cfn = ta[0];
	     sfn = ta[2];
             dcl = find_proto(cpr, cfn);
	     if (dcl != NULL)
	        {fl = scheme_wrap(fp, fl, dcl, sfn, ta+4);
		 free_decl(dcl);};

	     free_strings(ta);};};

/* write the routine to install the bindings */
    scheme_install(fp, pck, fl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_SCHEME - finalize Scheme file */

static void fin_scheme(bindes *bd)
   {FILE *fp;

    fp = bd->fp;

    csep(fp);
    fclose(fp);

    bd->fp = NULL;

    return;}

/*--------------------------------------------------------------------------*/

/*                             PYTHON ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* INIT_PYTHON - initialize Python file */

static void init_python(bindes *bd, char *pck, int cfl,
			char **sbi, char **cpr, char **fpr, char **fwr)
   {FILE *fp;

    fp = open_file("w", "gp-%s.c", pck);

    fprintf(fp, "\n");
    fprintf(fp, "#include \"sx_int.h\"\n");
    fprintf(fp, "\n");

    csep(fp);

    setup_binder(bd, fp, pck, cfl, sbi, cpr, fpr, fwr);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_WRAP - wrap C function DCL in PYTHON callable function
 *             - using name PFN
 */

static void python_wrap(FILE *fp, fdecl *dcl, char *pfn)
   {char upn[MAXLINE];

    if (strcmp(pfn, "none") != 0)
       {nstrncpy(upn, MAXLINE, pfn, -1);
	upcase(upn);

	csep(fp);
	fprintf(fp, "\n");

	fprintf(fp, "\n");
	csep(fp);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_PYTHON - generate Python bindings from CPR and SBI
 *             - return TRUE iff successful
 */

static int bind_python(bindes *bd)
   {int ib, rv;
    char t[MAXLINE];
    char *sb, **ta;
    char *cfn, *pfn;
    fdecl *dcl;
    FILE *fp;
    char **cpr, **sbi;

    fp  = bd->fp;
    sbi = bd->sbi;
    cpr = bd->cpr;

    rv = TRUE;

    for (ib = 0; sbi[ib] != NULL; ib++)
        {sb = sbi[ib];
	 if (blank_line(sb) == FALSE)
	    {nstrncpy(t, MAXLINE, sb, -1);
	     ta = tokenize(t, " \t");
	     cfn = ta[0];
	     pfn = ta[3];
             dcl = find_proto(cpr, cfn);
	     if (dcl != NULL)
	        {python_wrap(fp, dcl, pfn);
		 free_decl(dcl);};

	     free_strings(ta);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_PYTHON - finalize Python file */

static void fin_python(bindes *bd)
   {FILE *fp;

    fp = bd->fp;

    csep(fp);
    fclose(fp);

    bd->fp = NULL;

    return;}

/*--------------------------------------------------------------------------*/

/*                              DOC ROUTINES                                */

/*--------------------------------------------------------------------------*/

/* INIT_DOC - initialize Doc file */

static void init_doc(bindes *bd, char *pck, int cfl,
		     char **sbi, char **cpr, char **fpr, char **fwr)
   {FILE *fp;

    fp = open_file("w", "gh-%s.html", pck);

    fprintf(fp, "\n");

    hsep(fp);

    setup_binder(bd, fp, pck, cfl, sbi, cpr, fpr, fwr);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DOC_PROTO_FORTRAN - render the arg list of DCL into A for the
 *                   - Fortran callable C wrapper
 */

static void doc_proto_fortran(char *a, int nc, fdecl *dcl)
   {int i, na, ns;
    char **args;

    na = dcl->na;

    a[0] = '\0';
    if (na != 0)
       {args = dcl->tfproto;
	ns   = dcl->ntf;
        for (i = 2; i < ns; i += 2)
	    vstrcat(a, MAXLINE, "%s %s, ", args[i], args[i+1]);
        a[strlen(a) - 2] = '\0';};

    memmove(a, trim(subst(a, "* ", "*", -1), BOTH, " "), nc);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DOC_PROTO_NAME_ONLY - render the arg list of DCL into A using
 *                     - variable names only
 */

static void doc_proto_name_only(char *a, int nc, fdecl *dcl, char *dlm)
   {int i, na;
    farg *al;

    na = dcl->na;
    al = dcl->al;

    a[0] = '\0';
    if (na != 0)
       {if (dlm == NULL)
	   {for (i = 0; i < na; i++)
	        vstrcat(a, MAXLINE, " %s", al[i].name);}
	else
	   {for (i = 0; i < na; i++)
	        vstrcat(a, MAXLINE, " %s%s", al[i].name, dlm);
	    a[strlen(a)-strlen(dlm)] = '\0';};};

    memmove(a, trim(a, BOTH, " "), nc);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DOC_WRAP - wrap C function DCL in DOC callable function
 *          - using names in FN
 */

static void doc_wrap(FILE *fp, fdecl *dcl, char *cfn, char **fn, char **com)
   {int voidf;
    char upn[MAXLINE], lfn[MAXLINE], dcn[MAXLINE];
    char af[MAXLINE], as[MAXLINE], ap[MAXLINE];
    char fty[MAXLINE], t[MAXLINE];
    char **args;

    voidf = (strcmp(dcl->type, "void") == 0);

    nstrncpy(upn, MAXLINE, cfn, -1);
    upcase(upn);

    nstrncpy(lfn, MAXLINE, cfn, -1);
    downcase(lfn);

    cf_type(fty, MAXLINE, dcl->type);

    hsep(fp);
    fprintf(fp, "\n");

    fprintf(fp, "<a name=\"%s\"><h2>%s</h2></a>\n", lfn, upn);
    fprintf(fp, "\n");
    fprintf(fp, "<p>\n");
    fprintf(fp, "<pre>\n");

/* C */
    fprintf(fp, "<i>C Binding: </i>       %s\n", dcl->proto);

/* Fortran */
    if (strcmp(fn[0], "none") == 0)
       fprintf(fp, "<i>Fortran Binding: </i> none\n");
    else
       {args = dcl->tfproto;

	doc_proto_fortran(af, MAXLINE, dcl);
	map_name(dcn, MAXLINE, args[1], NULL, "f", -1, FALSE);
	if (voidf == TRUE)
	   fprintf(fp, "<i>Fortran Binding: </i> %s(%s)\n",
		   dcn, af);
	else
	   fprintf(fp, "<i>Fortran Binding: </i> %s %s(%s)\n",
		   args[0], dcn, af);};

/* Scheme */
    if (strcmp(fn[1], "none") == 0)
       fprintf(fp, "<i>SX Binding: </i>      none\n");
    else
       {map_name(dcn, MAXLINE, cfn, NULL, NULL, -1, TRUE);
	doc_proto_name_only(as, MAXLINE, dcl, NULL);
	if (IS_NULL(as) == TRUE)
	   fprintf(fp, "<i>SX Binding: </i>      (%s)\n", dcn);
	else
	   fprintf(fp, "<i>SX Binding: </i>      (%s %s)\n", dcn, as);};

/* Python */
    if (strcmp(fn[2], "none") == 0)
       fprintf(fp, "<i>Python Binding: </i>  none\n");
    else
       {map_name(dcn, MAXLINE, cfn, NULL, NULL, -1, FALSE);
	doc_proto_name_only(ap, MAXLINE, dcl, ",");
	fprintf(fp, "<i>Python Binding: </i>  pact.%s(%s)\n", dcn, ap);};

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

/* BIND_DOC - generate Doc bindings from CPR and SBI
 *          - return TRUE iff successful
 */

static int bind_doc(bindes *bd)
   {int ib, rv;
    char t[MAXLINE];
    char *sb, **ta;
    char *cfn;
    fdecl *dcl;
    FILE *fp;
    char **cpr, **sbi;

    fp  = bd->fp;
    cpr = bd->cpr;
    sbi = bd->sbi;

    rv = TRUE;

    for (ib = 0; sbi[ib] != NULL; ib++)
        {sb = sbi[ib];
	 if (blank_line(sb) == FALSE)
	    {nstrncpy(t, MAXLINE, sb, -1);
	     ta = tokenize(t, " \t");
	     cfn = ta[0];
             dcl = find_proto(cpr, cfn);
	     if (dcl != NULL)
	        {doc_wrap(fp, dcl, cfn, ta+1, ta+4);
		 free_decl(dcl);};

	     free_strings(ta);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_DOC - finalize Doc file */

static void fin_doc(bindes *bd)
   {FILE *fp;

    fp = bd->fp;

    hsep(fp);
    fclose(fp);

    bd->fp = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BLANG - control the generation of language binding
 *       - return TRUE iff successful
 */

static int blang(char *pck, int cfl, char *fbi,
		 char *cpr, char *fpr, char *fwr)
   {int ib, nb, rv;
    char **sbi, **scp, **sfp, **swr;
    bindes *pb;
    bindes bd[] = { {0, NULL, NULL, NULL, NULL, NULL, NULL,
		     init_fortran, bind_fortran, fin_fortran},
		    {0, NULL, NULL, NULL, NULL, NULL, NULL,
		     init_module, bind_module, fin_module},
		    {0, NULL, NULL, NULL, NULL, NULL, NULL,
		     init_scheme, bind_scheme, fin_scheme},
		    {0, NULL, NULL, NULL, NULL, NULL, NULL,
		     init_python, bind_python, fin_python},
		    {0, NULL, NULL, NULL, NULL, NULL, NULL,
		     init_doc, bind_doc, fin_doc} };

    rv = FALSE;

    if ((IS_NULL(cpr) == FALSE) && (IS_NULL(fbi) == FALSE))
       {sbi = file_text(fbi);
	scp = file_text(cpr);
	sfp = file_text(fpr);
	swr = file_text(fwr);

	if (scp == NULL)
	   printf("No prototypes found for '%s'\n", pck);

        else if (sbi == NULL)
	   printf("No bindings found for '%s'\n", pck);
            
	else
	   {add_derived_types(sbi);

	    if (scp != NULL)
	       {nb = sizeof(bd)/sizeof(bindes);

		for (pb = bd, ib = 0; ib < nb; ib++, pb++)
		    pb->init(pb, pck, cfl, sbi, scp, sfp, swr);

		if (sbi != NULL)
		   {for (pb = bd, ib = 0; ib < nb; ib++, pb++)
		        pb->bind(pb);};

		for (pb = bd, ib = 0; ib < nb; ib++, pb++)
		    pb->fin(pb);

		rv = TRUE;};};

	free_strings(sbi);
	free_strings(scp);
	free_strings(sfp);
	free_strings(swr);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it out here */

int main(int c, char **v)
   {int i, rv, cfl;
    char pck[MAXLINE], msg[MAXLINE];
    char *fbi, *cpr, *fpr, *fwr;

    fbi = "";
    cpr = "";
    fpr = "";
    fwr = "";
    cfl = 3;

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
            cfl &= ~2;
	 else if (strcmp(v[i], "-w") == 0)
            cfl &= ~1;
	 else
	    {if (IS_NULL(fbi) == TRUE)
	        fbi = v[i];
	     else if (IS_NULL(cpr) == TRUE)
	        cpr = v[i];
	     else if (IS_NULL(fpr) == TRUE)
	        fpr = v[i];
	     else if (IS_NULL(fwr) == TRUE)
	        fwr = v[i];};};

    snprintf(pck, MAXLINE, "%s", path_base(path_tail(fbi)));
    snprintf(msg, MAXLINE, "%s bindings", pck);

    printf("      %s ", fill_string(msg, 25));

    init_types();

    rv = blang(pck, cfl, fbi, cpr, fpr, fwr);
    rv = (rv != TRUE);

    printf("done\n");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

