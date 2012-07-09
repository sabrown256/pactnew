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

/* #define FORTRAN_BIND_C_ALL */

/* #define NO_DEFAULT_VALUE "NULL" */
#define NO_DEFAULT_VALUE "----"

#define N_MODES  5

/* #define RETURN_INTEGER */

#ifdef RETURN_INTEGER
#define C_PTR_RETURN "integer(isizea)"
#else
#define C_PTR_RETURN "type(C_PTR)"
#endif

enum e_langmode
   {MODE_C = 1, MODE_F, MODE_S, MODE_P, MODE_B };

typedef enum e_langmode langmode;

enum e_fparam
   {FP_ANY = 0,
    FP_VARARG,
    FP_FNC,            /* pointer to function */
    FP_INDIRECT,       /* pointer to undefined type */
    FP_ARRAY,          /* pointer to defined type */
    FP_SCALAR};

typedef enum e_fparam fparam;

enum e_fdir
   {FD_NONE = -1, FD_IN, FD_OUT, FD_IN_OUT};

typedef enum e_fdir fdir;

typedef struct s_statedes statedes;
typedef struct s_bindes bindes;
typedef struct s_idecl idecl;
typedef struct s_fdecl fdecl;
typedef struct s_farg farg;
typedef struct s_mtype mtype;

struct s_idecl
   {char decl[MAXLINE];    /* local variable declaration */
    char defa[MAXLINE];    /* default value assignment */
    char argc[MAXLINE];    /* argument to C function call */ 
    char argi[MAXLINE];    /* argument to call to get value from arg list */ 
    char argn[MAXLINE];};  /* arg names - for Python keyword list */

struct s_farg
   {int nv;                          /* number of default values specified */
    int cls;                         /* TRUE for class */
    fparam knd;
    fdir dir;                        /* data flow direction */
    char *arg;                       /* original C argument specification */
    char *qualifier;                 /* ARG qualifiers on argument */
    char type[MAXLINE];              /* argument type */
    char name[MAXLINE];              /* argument name */
    char **val;                      /* default value specifications */
    idecl interp;};

struct s_fdecl
   {int proc;                        /* TRUE iff processed */
    int na;                          /* number of arguments */
    int nr;                          /* number of return quantities */
    int nc;                          /* number of char * args */
    int ntf;                         /* number of fortran prototype pairs */
    int voidf;                       /* TRUE for functions returning nothing */
    int voida;                       /* TRUE for functions with no args */
    int error;                       /* TRUE if declaration is incorrect */
    char **args;                     /* all args */
    char **cargs;                    /* char * args */
    char **tfproto;                  /* type, var fortran prototype pairs */
    char **bindings;                 /* list of language bindings specified */
    farg proto;                      /* farg representation of declaration */
    farg *al;};

struct s_mtype
   {char *cty;                       /* C types */
    char *fty;                       /* Fortran types */
    char *sty;                       /* Scheme types */
    char *pty;                       /* Python types */
    char *bty;                       /* Basis types */
    char *defv;};

struct s_statedes
   {int cfl;                         /* fortran wrappers/modules flag */
    int nbi;                         /* number of bindings */
    int ncp;                         /* number of C prototype lines */
    int ndc;                         /* number of doc lines */
    int nfp;                         /* number of Fortran prototype lines */
    int nfw;                         /* number of Fortran wrapper lines */
    int ndcl;                        /* number of declarations */
    int no[N_MODES];                 /* no generate flags */
    char *pck;                       /* name of package */
    char **sbi;
    char **cdc;
    char **cpr;
    char **fpr;
    char **fwr;
    fdecl *dcl;
    char path[MAXLINE];};            /* path for output files */

struct s_bindes
   {statedes *st;
    FILE *fp;
    void (*init)(statedes *st, bindes *bd);
    int (*bind)(bindes *bd);
    void (*fin)(bindes *bd);};

static char
 *ind = "";
/* *ind = "      "; */

static char
 *lookup_type(char ***val, char *ty, langmode ity, langmode oty),
 **mc_proto_list(fdecl *dcl);

static void
 fc_type(char *wty, int nc, farg *al, int afl, langmode mode),
 cs_type(char *a, int nc, farg *arg, int drf);

static char
 *istrl;

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

/* IS_PTR - return TRUE if TYPE is a pointer */

static int is_ptr(char *type)
   {int rv;

    rv = (strchr(type, '*') != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IS_FUNC_PTR - return TRUE if TYPE is a function pointer */

static int is_func_ptr(char *type, int wh)
   {int rv, syn, cnv;

    syn = (strstr(type, "(*") != NULL);
    cnv = (strncmp(type, "PF", 2) == 0);

    if ((wh & 1) && (syn == TRUE))
       rv = TRUE;

    else if ((wh & 2) && (cnv == TRUE))
       rv = TRUE;

    else
       rv = FALSE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DEREF - dereference a pointer S
 *       - place result in D
 *       - return TRUE iff a pointer was dereferenced
 */

static int deref(char *d, int nc, char *s)
   {int rv;

    rv = FALSE;

    nstrncpy(d, nc, s, -1);
    if (LAST_CHAR(d) == '*')
       {LAST_CHAR(d) = '\0';
	memmove(d, trim(d, BOTH, " \t"), nc);
        rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IDEREF - dereference S with SC_FOO_P_I to SC_FOO_I
 *        - in place
 *        - return TRUE iff a pointer was dereferenced
 */

static int ideref(char *s)
   {int rv;
    char *p;

    rv = FALSE;

    p = strstr(s, "_P_I");
    if (p != NULL)
       {memmove(p, p+2, 3);
	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GET_DEF_VALUE - get a proper default value VL appropriate to type TY
 *               - from the value specification SP
 *               - proper means legal in C syntax
 *               - the space VL is NC long
 */

static void get_def_value(char *lvl, int nc, char *sp, char *ty)
   {char lty[MAXLINE];
    char **dv;

    if (sp == NULL)
       lvl[0] = '\0';
    else
       nstrncpy(lvl, nc, sp, -1);

/* if no value specified */
    dv = NULL;
    if ((IS_NULL(lvl) == TRUE) || (strcmp(lvl, NO_DEFAULT_VALUE) == 0))
       {if ((is_ptr(ty) == TRUE) || (is_func_ptr(ty, 3) == TRUE))
	   nstrncpy(lvl, nc, "NULL", -1);
	else
	   lookup_type(&dv, ty, MODE_C, MODE_C);}

/* dereference a pointer type and get its default value */
    else if (lvl[0] == '*')
       {if (is_ptr(ty) == TRUE)
	   {deref(lty, MAXLINE, ty);
	    lookup_type(&dv, lty, MODE_C, MODE_C);}
	else
	   lookup_type(&dv, ty, MODE_C, MODE_C);};

    if (dv != NULL)
       {nstrncpy(lvl, nc, dv[0], -1);
	lst_free(dv);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GET_CLASS_ARG - return the argument of DCL that specifies a class
 *               - return NULL if there is no class arg
 */

static farg *get_class_arg(fdecl *dcl)
   {int i, na;
    farg *al, *rv;

    na = dcl->na;
    al = dcl->al;
    rv = NULL;

    for (i = 0; (i < na) && (rv == NULL); i++)
        {if (al[i].cls == TRUE)
	    rv = al + i;};

    return(rv);}

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

/* IS_VAR_ARG - return TRUE iff the function described by PR
 *            - has a variable arg list
 */

static int is_var_arg(char *pr)
   {int rv;

    rv = (strstr(pr, "...") != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GET_ARG - return the next argument from the list ARGS */

static char *get_arg(char *args)
   {int c, np, ok;
    char *pa;

    ok = FALSE;
    np = 0;
    pa = args;

    for (c = *pa++; (c != '\0') && (ok == FALSE); c = *pa++)
        {switch (c)
	    {case ',' :
	          ok = (np == 0);
	          break;
	     case '(' :
	     case '[' :
	          np++;
	          break;
	     case ')' :
	     case ']' :
	          ok = (np == 0);
	          np--;
	          break;
	     default :
	          break;};};

    if ((c != '\0') && (pa > args))
       pa[-2] = '\0';

    return(args);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SPLIT_ARGS - split an arg list into individual args */

static char **split_args(char *args)
   {char *arg, *pa, **al;

    al = NULL;
    for (pa = args; IS_NULL(pa) == FALSE; )
        {arg = get_arg(pa);
	 pa += strlen(arg) + 1;
	 al  = lst_push(al, trim(arg, FRONT, " \t"));};

    return(al);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ID_NO_DEFAULT - handle intepreter declarations in case
 *               - no defaults specified
 */

static void id_no_default(idecl *ip, char *ty, char *lty, char *nm, char *val)
   {char lvl[MAXLINE];

    get_def_value(lvl, MAXLINE, val, ty);

    snprintf(ip->decl, MAXLINE, "%s _l%s;\n", ty, nm);
    snprintf(ip->defa, MAXLINE, "    _l%-8s = %s;\n", nm, lvl);
    snprintf(ip->argi, MAXLINE, "            %s, &_l%s,\n", lty, nm);
    snprintf(ip->argn, MAXLINE, "\"%s\", ", nm);
    snprintf(ip->argc, MAXLINE, "_l%s", nm);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ID_FD_OUT - handle intepreter declarations in case
 *           - of output arguments
 */

static void id_fd_out(idecl *ip, char *ty, char *nm, int nvl, char **vls)
   {int drf;
    char dty[MAXLINE], vl[MAXLINE];

    drf = deref(dty, MAXLINE, ty);

    if (drf == TRUE)
       snprintf(ip->argc, MAXLINE, "_l%s", nm);
    else
       snprintf(ip->argc, MAXLINE, "&_l%s", nm);

    if (nvl == 1)
       {if (drf == TRUE)
	   {if (strcmp(vls[0], "NULL") == 0)
	       {snprintf(ip->decl, MAXLINE, "%s _l%s;\n", ty, nm);
		snprintf(ip->defa, MAXLINE, "    _l%-8s = NULL;\n", nm);}
	    else if (vls[0][0] == '*')
	       {get_def_value(vl, MAXLINE, vls[0], dty);
		snprintf(ip->decl, MAXLINE, "%s _l%s;\n", dty, nm);
		snprintf(ip->defa, MAXLINE, "    _l%-8s = %s;\n", nm, vl);
		snprintf(ip->argc, MAXLINE, "&_l%s", nm);}
	    else
	       {snprintf(ip->decl, MAXLINE, "%s _l%s;\n", dty, nm);
		snprintf(ip->defa, MAXLINE, "    _l%-8s = %s;\n", nm, vls[0]);
		snprintf(ip->argc, MAXLINE, "&_l%s", nm);};}
	else
	   snprintf(ip->decl, MAXLINE, "%s _l%s;\n", dty, nm);}
    else
       snprintf(ip->decl, MAXLINE, "%s _l%s[%d];\n", dty, nm, nvl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ID_FD_IN_OUT - handle intepreter declarations in case
 *              - of input/output arguments
 */

static void id_fd_in_out(idecl *ip, char *ty, char *lty,
			 char *nm, int nvl, char **vls)
   {int l;
    char dty[MAXLINE], lvl[MAXLINE];

    deref(dty, MAXLINE, ty);
    ideref(lty);

    if (nvl == 1)
       {get_def_value(lvl, MAXLINE, NULL, dty);
	snprintf(ip->decl, MAXLINE, "%s _l%s;\n", dty, nm);
	snprintf(ip->defa, MAXLINE, "    _l%-8s = %s;\n", nm, lvl);
	snprintf(ip->argi, MAXLINE, "            %s, &_l%s,\n", lty, nm);
	snprintf(ip->argn, MAXLINE, "\"%s\", ", nm);
	snprintf(ip->argc, MAXLINE, "&_l%s", nm);}

    else
       {snprintf(ip->decl, MAXLINE, "%s _l%s[%d];\n", dty, nm, nvl);
	for (l = 0; l < nvl; l++)
	    {get_def_value(lvl, MAXLINE, vls[l], dty);
	     vstrcat(ip->defa, MAXLINE, "    _l%s[%d] = %s;\n", nm, l, lvl);
	     vstrcat(ip->argi, MAXLINE, "            %s, &_l%s[%d],\n",
		     lty, nm, l);
	     vstrcat(ip->argn, MAXLINE, "\"%s%d\", ", nm, l);};

	snprintf(ip->argc, MAXLINE, "_l%s", nm);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ID_FD_IN - handle intepreter declarations in case
 *          - of input arguments
 */

static void id_fd_in(idecl *ip, char *ty, char *lty,
		     char *nm, int nvl, char **vls)
   {int l, drf;
    char dty[MAXLINE], lvl[MAXLINE];

    drf = deref(dty, MAXLINE, ty);
    ideref(lty);

    if (nvl == 1)
       {get_def_value(lvl, MAXLINE, vls[0], dty);

	snprintf(ip->defa, MAXLINE, "    _l%-8s = %s;\n", nm, lvl);
	snprintf(ip->argi, MAXLINE, "            %s, &_l%s,\n", lty, nm);
	snprintf(ip->argn, MAXLINE, "\"%s\", ", nm);

	if ((drf == TRUE) &&
	    ((is_ptr(ty) == FALSE) || (strcmp(vls[0], "NULL") != 0)))
	   {snprintf(ip->decl, MAXLINE, "%s _l%s;\n", dty, nm);
	    snprintf(ip->argc, MAXLINE, "&_l%s", nm);}
	else
	   {snprintf(ip->decl, MAXLINE, "%s _l%s;\n", ty, nm);
	    snprintf(ip->argc, MAXLINE, "_l%s", nm);};}

/* multiple default values means a local array */
    else
       {snprintf(ip->decl, MAXLINE, "%s _l%s[%d];\n", dty, nm, nvl);

	for (l = 0; l < nvl; l++)
	    {get_def_value(lvl, MAXLINE, vls[l], ty);
	     vstrcat(ip->defa, MAXLINE, "    _l%s[%d] = %s;\n", nm, l, lvl);
	     vstrcat(ip->argi, MAXLINE, "            %s, &_l%s[%d],\n",
		     lty, nm, l);
	     vstrcat(ip->argn, MAXLINE, "\"%s%d\", ", nm, l);};

	snprintf(ip->argc, MAXLINE, "_l%s", nm);};


    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROCESS_INTERP_DEF - setup correct expressions for interpreter
 *                    - handling of defaults
 */

static void process_interp_def(farg *al)
   {int nvl;
    fdir dir;
    char s[MAXLINE], lty[MAXLINE], lvl[MAXLINE];
    char oexp[MAXLINE], nexp[MAXLINE];
    char *ty, *nm, *arg, *val, **vls;
    idecl *ip;
    static int count = 0;
	
    ip  = &al->interp;
    ty  = al->type;
    nm  = al->name;
    dir = al->dir;
    arg = al->arg;
    vls = al->val;
    nvl = al->nv;

    ip->decl[0] = '\0';
    ip->defa[0] = '\0';
    ip->argi[0] = '\0';
    ip->argn[0] = '\0';
    ip->argc[0] = '\0';

    val = (nvl > 0) ? vls[0] : NO_DEFAULT_VALUE;

    cs_type(lty, MAXLINE, al, FALSE);

/* function pointer */
    if (is_func_ptr(arg, 1) == TRUE)
       {get_def_value(lvl, MAXLINE, val, ty);
	snprintf(oexp, MAXLINE, "(*%s)", nm);
	snprintf(nexp, MAXLINE, "(*_l%s)", nm);
	nstrncpy(s, MAXLINE, arg, -1);
	snprintf(ip->decl, MAXLINE, "%s;\n", subst(s, oexp, nexp, 1));
	snprintf(ip->defa, MAXLINE, "    _l%-8s = %s;\n", nm, lvl);
	snprintf(ip->argi, MAXLINE, "            %s, &_l%s,\n", lty, nm);
	snprintf(ip->argn, MAXLINE, "\"%s\", ", nm);
	snprintf(ip->argc, MAXLINE, "_l%s", nm);}

    else if (nvl == 0)
       id_no_default(ip, ty, lty, nm, val);

/* single default values means a local scalar for a C pointer type */
    else
       {switch (dir)
	   {case FD_OUT :
	         id_fd_out(ip, ty, nm, nvl, vls);
		 break;

	    case FD_IN_OUT :
	         id_fd_in_out(ip, ty, lty, nm, nvl, vls);
		 break;

	    case FD_IN :
	         id_fd_in(ip, ty, lty, nm, nvl, vls);
		 break;

	    default :
	         id_no_default(ip, ty, lty, nm, val);
		 break;};};

    count++;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROCESS_QUALIFIERS - process argument qualifiers
 *                    - return TRUE if successful
 */

static int process_qualifiers(farg *al, char *qual)
   {int rv, ptr;
    char t[MAXLINE];
    char *val, *dir, **lst, **ta;

    rv = TRUE;

    if (IS_NULL(qual) == FALSE)
       {nstrncpy(t, MAXLINE, qual+4, -1);
	LAST_CHAR(t) = '\0';

	ta = split_args(t);
	if (ta != NULL)
	   {ptr = is_ptr(al->type);

/* get default value(s)
 *    <sspec> := "*" | <val>
 *    <spec>  := "" | <sspec>
 *    <list>  := <sspec> | <list> ',' <sspec>
 *    <specs> := <spec> | '[' <list> ']'
 *    ""       - no value specification
 *    "*"      - any single value
 *    <val>    - single value <val> 
 * NOTE: to make parsing easier things like [,] are not allowed
 * hence the <spec> <sspec> distinction
 */
	    val = ta[0];
	    lst = NULL;
	    if ((IS_NULL(val) == TRUE) || (val[0] == '*'))
	       lookup_type(&lst, al->type, MODE_C, MODE_C);
	    else
	       lst = tokenize(val, "[,]");
	    al->val = lst;
	    al->nv  = lst_length(lst);

	    if ((al->nv > 1) && (ptr == FALSE))
	       {berr("multiple values illegal with scalar '%s %s'",
		     al->arg, al->qualifier);
		al->nv = 0;
		rv = FALSE;};

/* get argument IN/OUT direction */
	    al->dir = FD_IN;
	    dir = ta[1];
	    if (dir != NULL)
	       {if (strcmp(dir, "in") == 0)
		   al->dir = FD_IN;
		else if (strcmp(dir, "out") == 0)
		   al->dir = FD_OUT;
		else if (strcmp(dir, "io") == 0)
		   al->dir = FD_IN_OUT;};

	    if (((al->dir == FD_OUT) || (al->dir == FD_IN_OUT)) &&
		 (ptr == FALSE))
	       {berr("scalar arguments cannot be IO or OUT - '%s %s'",
		     al->arg, al->qualifier);
		al->nv = 0;
		rv = FALSE;};

/* get kind adjustment */
	    al->cls = ((IS_NULL(ta[2]) == FALSE) && (strcmp(ta[2], "cls") == 0));

	    free_strings(ta);}
	else
	   {lookup_type(&al->val, al->type, MODE_C, MODE_C);
	    al->dir = FD_NONE;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SPLIT_DECL - split a declaration S of the form
 *            -    <type> ['*']* <name> [ARG([spec,]* | [spec])]
 *            - into proper type and name
 *            - return TRUE iff successful
 */

static int split_decl(farg *al, char *s, int isarg)
   {int nc, rv;
    char t[MAXLINE];
    char *p, *pn, *qual;

    nstrncpy(t, MAXLINE, s, -1);

/* look for ARG qualifiers */
    qual = strstr(t, "ARG(");
    if (qual != NULL)
       qual[-1] = '\0';

    al->arg       = STRSAVE(t);
    al->qualifier = (qual == NULL) ? qual : STRSAVE(qual);
    al->nv        = 0;
    al->knd       = FP_ANY;
    al->dir       = FD_NONE;
    al->val       = NULL;

    p = trim(strtok(t, "),"), BOTH, " \t");

    if (strcmp(p, "void") == 0)
       pn = p + 4;
    else if (is_var_arg(p) == TRUE)
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
       {nstrncpy(al->name, MAXLINE, pn+2, -1);
	*pn = '\0';
	nstrncpy(al->type, MAXLINE, "PFInt", -1);

	rv = TRUE;}

    else if (is_func_ptr(p, 2) == TRUE)
       {nstrncpy(al->name, MAXLINE, pn, -1);
	pn[-1] = '\0';
	nstrncpy(al->type, MAXLINE, p, -1);

	rv = TRUE;}

/* handle other args */
    else
       {nstrncpy(al->name, MAXLINE, pn, -1);
	*pn = '\0';
	nstrncpy(al->type, MAXLINE, trim(p, BACK, " \t"), -1);

	rv = FALSE;};

    rv = process_qualifiers(al, qual);

/* setup correct default values for interpreters */
    if ((rv == TRUE) && (isarg == TRUE))
       process_interp_def(al);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROC_ARGS - process the arg list in the declaration DCL */

static farg *proc_args(fdecl *dcl)
   {int i, na, nr, ok, err;
    char **args;
    farg *al;

    na   = dcl->na;
    args = dcl->args;
    
    al = MAKE_N(farg, na);
    if (al != NULL)
       {memset(al, 0, na*sizeof(farg));

	err = FALSE;
	nr  = 0;
	for (i = 0; i < na; i++)
	    {ok = split_decl(al+i, args[i], TRUE);
	     if (ok == FALSE)
	        {FREE(al);
		 free_strings(dcl->bindings);
		 dcl->bindings = NULL;
		 dcl->na       = 0;
		 err           = TRUE;
		 break;}
	     else
	        {fc_type(NULL, 0, al+i, TRUE, MODE_C);
		 nr += al[i].nv;};};

	dcl->error = err;
	dcl->al    = al;
	dcl->nr    = nr;
	dcl->voida = ((dcl->na == 1) && (IS_NULL(dcl->al[0].name) == TRUE));

/* fill in the Fortran prototype token pairs */
	mc_proto_list(dcl);};

    return(al);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIND_FUNC - return true if function named in prototype PR matches F */

static int find_func(char *pr, char *f)
   {int rv;
    char t[MAXLINE];
    char *p;

    snprintf(t, MAXLINE, "%s(", f);
    p  = strstr(pr, t);
    rv = ((p != NULL) && (strchr(" *\t", p[-1]) != NULL));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIND_PROTO - find the prototype for F in CPR
 *            - return it iff successful
 */

static int find_proto(fdecl *dcl, char **cpr, char *f)
   {int ip, na, nt, ok;
    char pf[MAXLINE];
    char *p, *pa, *sp, *pro, *cfn;
    char **ty, **args;

    ok  = FALSE;
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
       {if (dcl == NULL)
	   {dcl = MAKE(fdecl);
	    memset(dcl, 0, sizeof(fdecl));};

	if (dcl != NULL)
	   {dcl->proc  = FALSE;
	    dcl->na    = 0;
	    dcl->args  = NULL;
	    dcl->nc    = 0;
	    dcl->cargs = NULL;

/* break up the prototype into the type/function name and args */
	    nstrncpy(pf, MAXLINE, pro, -1);
	    p = strchr(pf, '(');
            if (p != NULL)
	       *p++ = '\0';
	    pa = p;
	    LAST_CHAR(pa) = '\0';

/* get the return type */
	    ok = split_decl(&dcl->proto, pf, FALSE);
	
	    ty = tokenize(pf, " \t");
	    if (ty != NULL)
	       {nt = lst_length(ty);

		FREE(dcl->proto.arg);
		dcl->proto.arg = STRSAVE(pro);

/* get the function name */
		cfn = ty[nt-1];
		ASSERT(cfn != NULL);

/* get the args */
		args = split_args(pa);
		for (na = 0; IS_NULL(args[na]) == FALSE; na++);
		dcl->na    = na;
		dcl->args  = args;
		dcl->voida = TRUE;
		dcl->voidf = (strcmp(dcl->proto.type, "void") == 0);

		proc_args(dcl);

		free_strings(ty);};};};

    return(ok);}

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

static void add_type(char *cty, char *fty, char *sty, char *pty, char *defv)
   {int n;
    mtype *map;

    get_type_map(&n, &map, 1);
    if (map != NULL)
       {n--;
	n = max(n, 0);

	map[n].cty  = cty;
	map[n].fty  = fty;
	map[n].sty  = sty;
	map[n].pty  = pty;
	map[n].defv = defv;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LOOKUP_TYPE - lookup and return a type from the map */

static char *lookup_type(char ***val, char *ty, langmode ity, langmode oty)
   {int i, l, n;
    char *rv, *dv, **lst;
    mtype *map;

    rv = NULL;

    get_type_map(&n, &map, 0);

    l = -1;
    if (map != NULL)
       {switch (ity)
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
		 break;
 	    case MODE_B :
	         for (i = 0; i < n; i++)
		     {if (strcmp(ty, map[i].bty) == 0)
			 {l = i;
			  break;};};
		 break;};

	dv = NO_DEFAULT_VALUE;
	if (l != -1)
	   {dv = map[l].defv;
	    switch (oty)
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
		     break;
	        case MODE_B :
		     rv = map[l].bty;
		     break;};}
	else if ((is_func_ptr(ty, 3) == TRUE) || (is_ptr(ty) == TRUE))
	   dv = "NULL";

	if (val != NULL)
	   {lst = NULL;
	    if (dv != NULL)
	       lst = lst_push(lst, dv);
	    *val = lst;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_TYPES - initialize the type map */

static void init_types(void)
   {

    add_type("void",        "",            "",                 NULL, "NULL");
    add_type("bool",        "logical",     "SC_BOOL_I",        NULL, "FALSE");
    add_type("char",        "character",   "SC_CHAR_I",        NULL, "'\\0'");

/* fixed point types */
    add_type("short",       "integer(2)",  "SC_SHORT_I",       NULL, "0");
    add_type("int",         "integer",     "SC_INT_I",         NULL, "0");
    add_type("long",        "integer(8)",  "SC_LONG_I",        NULL, "0L");
    add_type("long long",   "integer(8)",  "SC_LONG_LONG_I",   NULL, "0LL");

    add_type("size_t",      "integer(8)",  "SC_LONG_I",        NULL, "0L");
    add_type("ssize_t",     "integer(8)",  "SC_LONG_I",        NULL, "0L");

/* fixed width fixed point types */
    add_type("int16_t",     "integer(2)",  "SC_INT16_I",       NULL, "0");
    add_type("int32_t",     "integer(4)",  "SC_INT32_I",       NULL, "0");
    add_type("int64_t",     "integer(8)",  "SC_INT64_I",       NULL, "0L");

/* floating point types */
    add_type("float",       "real(4)",     "SC_FLOAT_I",       NULL, "0.0");
    add_type("double",      "real(8)",     "SC_DOUBLE_I",      NULL, "0.0");
    add_type("long double", "real(16)",    "SC_LONG_DOUBLE_I", NULL, "0.0");

/* complex types */
    add_type("float _Complex",       "complex(4)",
	     "SC_FLOAT_COMPLEX_I", NULL, "0.0");
    add_type("double _Complex",      "complex(8)",
	     "SC_DOUBLE_COMPLEX_I", NULL, "0.0");
    add_type("long double _Complex", "complex(16)",
	     "SC_LONG_DOUBLE_COMPLEX_I", NULL, "0.0");

/* GOTCHA: there is a general issue with pointers and Fortran here
 * doing add_type on "void *" causes Fortran wrapper declarations
 * to be generated with "void *" in the arg list
 * if on the other hand we do not to an add_type on "void *" then
 * blang will generate Fortran wrapper declarations with "void **"
 * in the arg list
 * in some contexts we would rather have "void **" to accord with
 * the extra reference added by Fortran which is call by reference
 * by default
 * the same applies to all of these pointers and we have been
 * bitten by FILE and void in the tests
 * with "void *" defined pd_write_f works for real*8 a(10)
 * but fails for type(C_PTR) b
 */
    add_type("void *",        "C_PTR-A",      "SC_POINTER_I",       NULL, "NULL");
    add_type("bool *",        "logical-A",    "SC_BOOL_P_I",        NULL, "NULL");
    add_type("char *",        "character-A",  "SC_STRING_I",        NULL, "NULL");

    add_type("short *",       "integer(2)-A", "SC_SHORT_P_I",       NULL, "NULL");
    add_type("int *",         "integer-A",    "SC_INT_P_I",         NULL, "NULL");
    add_type("long *",        "integer(8)-A", "SC_LONG_P_I",        NULL, "NULL");
    add_type("long long *",   "integer(8)-A", "SC_LONG_LONG_P_I",   NULL, "NULL");

    add_type("float *",       "real(4)-A",    "SC_FLOAT_P_I",       NULL, "NULL");
    add_type("double *",      "real(8)-A",    "SC_DOUBLE_P_I",      NULL, "NULL");
    add_type("long double *", "real(16)-A",   "SC_LONG_DOUBLE_P_I", NULL, "NULL");

/* complex types */
    add_type("float _Complex *",       "complex(4)-A",
	     "SC_FLOAT_COMPLEX_P_I", NULL, "NULL");
    add_type("double _Complex *",      "complex(8)-A",
	     "SC_DOUBLE_COMPLEX_P_I", NULL, "NULL");
    add_type("long double _Complex *", "complex(16)-A",
	     "SC_LONG_DOUBLE_COMPLEX_P_I", NULL, "NULL");

    add_type("pcons",         "C_PTR-A",      "SC_PCONS_I",         NULL, "NULL");
    add_type("pcons *",       "C_PTR-A",      "SC_PCONS_P_I",       NULL, "NULL");
/*    add_type("FILE *",        "C_PTR-A",      "SC_FILE_I",          NULL, "NULL"); */
    add_type("PROCESS *",     "C_PTR-A",      "SC_PROCESS_I",       NULL, "NULL");

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
		  if (ta != NULL)
		     {add_type(ta[1], ta[2], ta[3], ta[4], ta[5]);
		      FREE(ta[0]);
		      FREE(ta);};};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAP_NAME - map the C function name CF to form target language name D
 *          - unless the specified language binging name LF is non-NULL
 *          - or not "yes"
 *          - in which case return LF in D
 */

static char *map_name(char *d, int nc, char *cf, char *lf,
		      char *sfx, int cs, int us, int cls)
   {int i, n, ok;
    static char *pre[] = { "SC_", "PM_", "PD_", "PC_", "PG_", "PA_" };

    n = sizeof(pre)/sizeof(char *);
    ok = FALSE;
    for (i = 0; (i < n) && (ok == FALSE); i++)
        {if (strncmp(cf, pre[i], 3) == 0)
	    ok = TRUE;};

    if ((lf != NULL) && (strcmp(lf, "none") != 0) && (strcmp(lf, "yes") != 0))
       {if ((sfx != NULL) && (sfx[0] == 'i'))
	   snprintf(d, nc, "%s_%s", lf, sfx);
	else
	   nstrncpy(d, nc, lf, -1);}

    else if ((cls == TRUE) && (ok == TRUE))
       snprintf(d, nc, "%s", cf+3);

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

/* FIND_BIND - generate declarations for all bindings from CPR and SBI
 *           - return TRUE iff successful
 */

static void find_bind(statedes *st)
   {int i, ib, nb, nbi;
    char t[MAXLINE];
    char *sb, *lng, *p;
    char **cpr, **sbi, **sa, **ta;
    fdecl *dcl;

    nbi = st->nbi;
    sbi = st->sbi;
    cpr = st->cpr;

    nb  = 0;
    dcl = MAKE_N(fdecl, nbi);
    if (dcl != NULL)
       {for (ib = 0; ib < nbi; ib++)
	    {sb = sbi[ib];
	     if (blank_line(sb) == FALSE)
	        {nstrncpy(t, MAXLINE, sb, -1);
		 sa = tokenize(t, " \t");

		 if ((sa != NULL) && (strcmp(sa[0], "derived") != 0))
		    {ta = NULL;
		     for (i = 1; sa[i] != NULL; i++)
		         {lng = sa[i];
			  p   = strchr(lng, '(');
			  if (p != NULL)
			     {*p++ = '\0';
			      ta   = lst_push(ta, lng);
			      p    = trim(p, BACK, ")");
			      if (IS_NULL(p) == TRUE)
				 ta = lst_push(ta, "yes");
			      else
				 ta = lst_push(ta, p);}
			  else
			     ta = lst_push(ta, lng);};

		     dcl[nb].bindings = ta;

		     nb += find_proto(dcl+nb, cpr, sa[0]);};

		 free_strings(sa);};};};

    st->dcl  = dcl;
    st->ndcl = nb;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HAS_BINDING - return the LANG binding specification */

static char *has_binding(fdecl *dcl, char *lang)
   {int ib;
    char *b, *spec;
    char **bindings;

    spec = NULL;

    bindings = dcl->bindings;
    if (bindings != NULL)
       {for (ib = 0; bindings[ib] != NULL; ib += 2)
	    {b = bindings[ib];
	     if (strcmp(b, lang) == 0)
	        {spec = bindings[ib+1];
		 break;};};};

    return(spec);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_BINDER - initialize the data members of BD */

static void setup_binder(statedes *st, char *pck, int cfl,
			 char **sbi, char **cdc, char **cpr,
			 char **fpr, char **fwr)
   {

    if (st != NULL)
       {st->pck = pck;
	st->cfl = cfl;

	st->sbi = sbi;
	st->nbi = lst_length(sbi);

	st->cdc = cdc;
	st->ndc = lst_length(cdc);

	st->cpr = cpr;
	st->ncp = lst_length(cpr);

	st->fpr = fpr;
	st->nfp = lst_length(fpr);

	st->fwr = fwr;
	st->nfw = lst_length(fwr);

/* parse out and construct all the declarations */
	find_bind(st);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CF_CALL_LIST - render the arg list of DCL into A for a
 *              - compiled function call (usually C)
 */

static void cf_call_list(char *a, int nc, fdecl *dcl, int local)
   {int i, na, voida;
    char *nm;
    farg *al;
    idecl *ip;

    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;

    a[0] = '\0';
    if (voida == FALSE)
       {for (i = 0; i < na; i++)
	    {nm = al[i].name;
	     ip = &al[i].interp;

/* pass function pointers straight thru - no local variable */
	     if ((al[i].knd == FP_FNC) && (local == FALSE))
	        vstrcat(a, MAXLINE, "%s, ", nm);

/* scalar return values need references */
	     else if (local == TRUE)
	        vstrcat(a, MAXLINE, "%s, ", ip->argc);

/* all other argument go thru a local variable as declared */
	     else
	        vstrcat(a, MAXLINE, "_l%s, ", nm);};

	a[strlen(a) - 2] = '\0';};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IF_CALL_LIST - render the arg list of DCL into A for an
 *              - interpreter function call
 */

static void if_call_list(char *a, int nc, fdecl *dcl, char *dlm)
   {int i, na, voida;
    farg *al;

    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;

    a[0] = '\0';
    if (voida == FALSE)
       {if (dlm == NULL)
	   {for (i = 0; i < na; i++)
	        {if (al[i].cls == FALSE)
		    vstrcat(a, MAXLINE, "%s ", al[i].name);};

	    a[strlen(a) - 1] = '\0';}

	else
	   {for (i = 0; i < na; i++)
	        {if (al[i].cls == FALSE)
		    vstrcat(a, MAXLINE, "%s%s ", al[i].name, dlm);};

	    a[strlen(a) - 2] = '\0';};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* C_PROTO - make a C prototype without any ARG qualifiers */

static void c_proto(char *pr, int nc, fdecl *dcl)
   {int i, na, voida;
    char *arg;
    farg *al;

    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;

    snprintf(pr, nc, "%s %s(", dcl->proto.type, dcl->proto.name);
    if (voida == FALSE)
       {for (i = 0; i < na; i++)
	    {arg = al[i].arg;
	     vstrcat(pr, nc, "%s, ", arg);};
	pr[strlen(pr)-2] = '\0';};

    nstrcat(pr, nc, ")");
    nstrncpy(pr, nc, subst(pr, "* ", "*", -1), -1);

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
       {fty = lookup_type(NULL, ty, MODE_C, MODE_F);
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

static void fc_type(char *wty, int nc, farg *al, int afl, langmode mode)
   {fparam knd;
    char lty[MAXLINE];
    char *pty, *arg, *ty;

    if (al != NULL)
       {knd = FP_ANY;

	if (wty == NULL)
	   {wty = lty;
	    nc  = MAXLINE;};

	arg = al->arg;
	ty  = al->type;

	pty = lookup_type(NULL, ty, MODE_C, mode);

/* handle variable arg list */
	if (is_var_arg(ty) == TRUE)
	   {nstrncpy(wty, nc, "char *", -1);
	    knd = FP_VARARG;}

/* handle function pointer */
	else if ((afl == TRUE) && (strstr(arg, "(*") != NULL))
	   {nstrncpy(wty, nc, arg, -1);
	    knd = FP_FNC;}

/* follow the PACT function pointer PF convention */
	else if (is_func_ptr(ty, 2) == TRUE)
	   {nstrncpy(wty, nc, ty, -1);
	    knd = FP_FNC;}

	else if (pty != NULL)
	   {if (LAST_CHAR(ty) == '*')
	       {knd = FP_ARRAY;

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
	       {knd = FP_SCALAR;
		nstrncpy(wty, nc, pty, -1);};}

/* unknown type */
	else
	   {nstrncpy(wty, nc, ty, -1);

	    if (LAST_CHAR(ty) == '*')
	       knd = FP_INDIRECT;
	    else
	       berr("Unknown type '%s'", ty);};

	al->knd = knd;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FC_DECL_LIST - render the arg list of DCL into A for the
 *              - Fortran callable C wrapper
 */

static void fc_decl_list(char *a, int nc, fdecl *dcl)
   {int i, na, nca, voida;
    fparam knd;
    char *ty, *nm, *arg, **cargs;
    farg *al;

    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;

    a[0] = '\0';
    if ((na == 0) || (voida == TRUE))
       nstrcat(a, MAXLINE, "void");
    else
       {for (i = 0; i < na; i++)
	    {nm  = al[i].name;
	     ty  = al[i].type;
             arg = al[i].arg;
	     knd = al[i].knd;

	     switch (knd)
	        {case FP_FNC :
		      vstrcat(a, MAXLINE, "%s, ", arg);
 		      break;
		 case FP_ARRAY :
		      if ((dcl->nc == 0) && (strcmp(ty, "char *") == 0))
			 dcl->cargs = lst_push(dcl->cargs, nm);
		      vstrcat(a, MAXLINE, "%s%s, ", ty, nm);
		      break;
		 case FP_SCALAR :
		      vstrcat(a, MAXLINE, "%s *%s, ", ty, nm);
		      break;
		 case FP_INDIRECT :
		      vstrcat(a, MAXLINE, "%s *%s, ", ty, nm);
		      break;
		 default :
		      printf("Unknown type: %s\n", ty);
		      break;};

	   al[i].knd = knd;};

	cargs = dcl->cargs;
	nca   = lst_length(cargs);
        for (i = 0; i < nca; i++)
	    vstrcat(a, MAXLINE, "%s snc%s, ", istrl, cargs[i]);
	if (dcl->nc == 0)
	   dcl->nc = nca;

        a[strlen(a) - 2] = '\0';};

    nstrncpy(a, nc, subst(a, "* ", "*", -1), -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_FORTRAN - initialize Fortran wrapper file */

static void init_fortran(statedes *st, bindes *bd)
   {int nc, cfl;
    char fn[MAXLINE], hf[MAXLINE], ufn[MAXLINE], fill[MAXLINE];
    char *pck;
    FILE *fp;

    fp  = NULL;
    cfl = st->cfl;
    pck = st->pck;

    if (cfl & 1)
       {if ((st->path == NULL) || (strcmp(st->path, ".") == 0))
	   snprintf(fn, MAXLINE, "gf-%s.c", pck);
	 else
	   snprintf(fn, MAXLINE, "%s/gf-%s.c", st->path, pck);
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

	snprintf(hf, MAXLINE, "%s_int.h", pck);
	if (file_exists(hf) == TRUE)
	   fprintf(fp, "#include \"%s\"\n", hf);
	else
	   fprintf(fp, "#include \"sx_int.h\"\n");

	fprintf(fp, "\n");
	csep(fp);};

    bd->fp = fp;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FORTRAN_WRAP_DECL - function declaration */

static void fortran_wrap_decl(FILE *fp, fdecl *dcl,
			      fparam knd, char *rt, char *cfn, char *ffn)
   {char ucn[MAXLINE], dcn[MAXLINE], a[MAXLINE], t[MAXLINE];

    map_name(dcn, MAXLINE, cfn, ffn, "f", -1, FALSE, FALSE);

    nstrncpy(ucn, MAXLINE, dcn, -1);
    upcase(ucn);

    fc_decl_list(a, MAXLINE, dcl);

    fprintf(fp, "\n");
    fprintf(fp, "/* WRAP |%s| */\n", dcl->proto.arg);
    fprintf(fp, "\n");

#if defined(FORTRAN_BIND_C_ALL)
    switch (knd)
       {case FP_INDIRECT :

#ifdef RETURN_INTEGER
	     snprintf(t, MAXLINE, "FIXNUM %s(%s)\n",
		      dcn, a);
#else
	     snprintf(t, MAXLINE, "void *%s(%s)\n",
		      dcn, a);
#endif
	     break;
        case FP_ARRAY :
	     snprintf(t, MAXLINE, "%s *%s(%s)\n",
		      rt, dcn, a);
	     break;
        case FP_SCALAR :
	     snprintf(t, MAXLINE, "%s %s(%s)\n",
		      rt, dcn, a);
	     break;
        default :
	     snprintf(t, MAXLINE, "%s %s(%s)\n",
		      rt, dcn, a);
	     break;};

#else

    switch (knd)
       {case FP_INDIRECT :

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
#endif

    fputs(subst(t, "* ", "*", -1), fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FORTRAN_WRAP_LOCAL_DECL - local variable declarations */

static void fortran_wrap_local_decl(FILE *fp, fdecl *dcl,
				    fparam knd, char *rt)
   {int i, na, nv, voida, voidf;
    char t[MAXLINE];
    char *nm, *ty;
    farg *al;

    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;
    voidf = dcl->voidf;

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
		    {case FP_INDIRECT :
#ifdef RETURN_INTEGER
		          nstrcat(t, MAXLINE, "FIXNUM _rv;\n");
			  vstrcat(t, MAXLINE, "    SC_address _ad%s;\n",
				  dcl->proto.name);
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
		 case FP_INDIRECT :
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
		 case FP_INDIRECT :
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
				    fparam knd, char *rt)
   {int voidf;
    char a[MAXLINE];
    char *nm;

    cf_call_list(a, MAXLINE, dcl, FALSE);

    nm    = dcl->proto.name;
    voidf = dcl->voidf;

    if (voidf == FALSE)
       {switch (knd)
	   {case FP_INDIRECT :
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

static void fortran_wrap_local_return(FILE *fp, fdecl *dcl, fparam knd)
   {int voidf;

    voidf = dcl->voidf;

    if (voidf == FALSE)
       {switch (knd)
	   {case FP_INDIRECT :
#ifdef RETURN_INTEGER
	         fprintf(fp, "    _rv = _ad%s.diskaddr;\n\n", dcl->proto.name);
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

static void fortran_wrap(FILE *fp, fdecl *dcl, char *ffn)
   {fparam knd;
    char rt[MAXLINE];
    char *cfn, *fn;

    cfn = dcl->proto.name;

    if (is_var_arg(dcl->proto.arg) == TRUE)
       {if ((strcmp(ffn, "yes") == 0) || (strcmp(ffn, "none") == 0))
	   fn = cfn;
        else
	   fn = ffn;
	berr("%s is not interoperable - variable args", fn);}

    else if (strcmp(ffn, "none") != 0)
       {fc_type(rt, MAXLINE, &dcl->proto, FALSE, MODE_C);

	knd = dcl->proto.knd;

	csep(fp);

/* function declaration */
	fortran_wrap_decl(fp, dcl, knd, rt, cfn, ffn);

/* local variable declarations */
        fortran_wrap_local_decl(fp, dcl, knd, rt);

/* local variable assignments */
	fortran_wrap_local_assn(fp, dcl);

/* function call */
	fortran_wrap_local_call(fp, dcl, knd, rt);

/* return */
        fortran_wrap_local_return(fp, dcl, knd);

	csep(fp);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_FORTRAN - generate Fortran bindings from CPR and SBI
 *              - return TRUE iff successful
 */

static int bind_fortran(bindes *bd)
   {int ib, rv, ndcl;
    char *ffn;
    fdecl *dcl, *dcls;
    statedes *st;
    FILE *fp;

    rv = TRUE;
    st = bd->st;

    if (st->cfl & 1)
       {fp = bd->fp;

	dcls = st->dcl;
	ndcl = st->ndcl;
	for (ib = 0; ib < ndcl; ib++)
	    {dcl = dcls + ib;
	     ffn = has_binding(dcl, "fortran");
	     if (ffn != NULL)
	        fortran_wrap(fp, dcl, ffn);};};

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

static void mc_type(int nc, char *fty, char *cty,
		    char *wty, farg *arg)
   {char lfty[MAXLINE], lcty[MAXLINE], lwty[MAXLINE];
    char *ty;

    if (arg != NULL)
       {fc_type(lwty, nc, arg, FALSE, MODE_F);
	memmove(lwty, subst(lwty, "FIXNUM", "integer", -1), nc);

	ty = arg->type;
	if (is_var_arg(ty) == TRUE)
	   {nstrncpy(lfty, MAXLINE, "error", -1);
	    nstrncpy(lcty, MAXLINE, "error", -1);}

/* follow the PACT function pointer PF convention */
	else if (is_func_ptr(ty, 3) == TRUE)
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
	   nstrncpy(wty, nc, lwty, -1);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MC_PROTO_LIST - return list of tokens for type and name of argument
 *               - caller formats and frees list
 */

static char **mc_proto_list(fdecl *dcl)
   {int i, na, voida;
    fparam knd;
    char wty[MAXLINE];
    char **lst, *nm;
    farg *al;

    lst = NULL;

    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;

    if (al != NULL)

/* function/return type */
       {mc_type(MAXLINE, NULL, NULL, wty, &dcl->proto);
	nm  = dcl->proto.name;
	knd = dcl->proto.knd;
	switch (knd)
	   {case FP_FNC      :
	    case FP_INDIRECT :
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

	      mc_type(MAXLINE, NULL, NULL, wty, al+i);
	      nm  = al[i].name;
	      knd = al[i].knd;
	      switch (knd)
		 {case FP_FNC      :
		  case FP_INDIRECT :
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
	dcl->ntf     = lst_length(lst);};

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MC_NEED_PTR - return TRUE if the package types module will be needed */

static int mc_need_ptr(fdecl *dcl)
   {int i, na, ok, voida;
    char cty[MAXLINE];
    farg *al;

    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;

    ok = FALSE;

    mc_type(MAXLINE, NULL, cty, NULL, &dcl->proto);
    if (dcl->proto.knd != FP_VARARG)
       ok |= ((strcmp(cty, "C_PTR") == 0) ||
	      (strcmp(cty, "C_FUNPTR") == 0));

    if ((na > 0) && (voida == FALSE))
       {for (i = 0; i < na; i++)
	    {mc_type(MAXLINE, NULL, cty, NULL, al+i);
	     if (al[i].knd != FP_VARARG)
	        ok |= ((strcmp(cty, "C_PTR") == 0) ||
		       (strcmp(cty, "C_FUNPTR") == 0));};};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MC_DECL_LIST - render the arg list of DCL into A for the
 *              - Fortran/C interoperability module interface
 */

static void mc_decl_list(char *a, int nc, fdecl *dcl)
   {int i, na, voida;
    char *nm;
    farg *al;

    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;

    a[0] = '\0';
    if ((na > 0) && (voida == FALSE))
       {for (i = 0; i < na; i++)
	    {nm = al[i].name;

	     vstrcat(a, MAXLINE, "%s, ", nm);};

        a[strlen(a) - 2] = '\0';};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_MODULE - initialize Fortran/C interoperatbility module file */

static void init_module(statedes *st, bindes *bd)
   {int nc, cfl;
    char fn[MAXLINE], ufn[MAXLINE], fill[MAXLINE];
    char *pck;
    FILE *fp;

    fp = NULL;

    cfl = st->cfl;
    pck = st->pck;

    if (cfl & 2)
       {if ((st->path == NULL) || (strcmp(st->path, ".") == 0))
	   snprintf(fn, MAXLINE, "gm-%s.f", pck);
	 else
	   snprintf(fn, MAXLINE, "%s/gm-%s.f", st->path, pck);
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
	fprintf(fp, "%smodule types_%s\n", ind, pck);
	fprintf(fp, "%s   integer, parameter :: isizea = %d\n", ind,
		(int) sizeof(char *));
	fprintf(fp, "%send module types_%s\n", ind, pck);
	fprintf(fp, "\n");

	fprintf(fp, "%smodule pact_%s\n", ind, pck);
	fprintf(fp, "%s   use iso_c_binding\n", ind);
	fprintf(fp, "\n");};

    bd->fp = fp;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MODULE_NATIVE_F - write interface for native Fortran function */

static char **module_native_f(FILE *fp, fdecl *dcl, char **sa, char *pck)
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
	    {fprintf(fp, "%s%s\n", ind, p);
	     voidf = (strstr(p, "subroutine") != NULL);
	     oper  = (voidf == TRUE) ? "subroutine" : "function";

	     fprintf(fp, "%s         use iso_c_binding\n", ind);
	     fprintf(fp, "%s         implicit none\n", ind);}
	 else
	    fprintf(fp, "%s   %s\n", ind, p);};

    if (oper != NULL)
       fprintf(fp, "%s      end %s\n", ind, oper);

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

    rv = ((is_var_arg(pr) == FALSE) &&
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
	   fprintf(fp, "%s   external :: %s\n", ind, ta[1]);
	else
	   fprintf(fp, "%s   %s, external :: %s\n", ind, ta[0], ta[1]);};

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

	snprintf(t, MAXLINE, "%s      %s %s(%s)", ind, oper, dcn, a);
	femit(fp, t, "");

	fprintf(fp, "%s         use iso_c_binding\n", ind);
	fprintf(fp, "%s         use types_%s\n", ind, pck);
	fprintf(fp, "%s         implicit none\n", ind);
	if (voidf == FALSE)
	   fprintf(fp, "%s         %-12s :: %s\n", ind, rty, dcn);

	for (i = 2; i < nt; i += 2)
	    {if (strcmp(ta[i], "character") == 0)
		fprintf(fp, "%s         %s(*) :: %s\n", ind, ta[i], ta[i+1]);
             else if (strcmp(ta[i], "integer-A") == 0)
                fprintf(fp, "%s         %-12s :: %s(*)\n", "integer", ind, ta[i+1]);
             else if (strcmp(ta[i], "real8-A") == 0)
                fprintf(fp, "%s         %-12s :: %s(*)\n", "real*8", ind, ta[i+1]);
             else
                fprintf(fp, "%s         %-12s :: %s\n", ind, ta[i], ta[i+1]);};

	fprintf(fp, "%s      end %s %s\n", ind, oper, dcn);
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

    pr = dcl->proto.arg;
    rv = ((is_var_arg(pr) == FALSE) && (strstr(pr, "void *") == NULL));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MODULE_ITF_WRAP_EXT - write the interface for a simple extern */

static void module_itf_wrap_ext(FILE *fp, fdecl *dcl, char *pck, char *ffn)
   {char dcn[MAXLINE], fty[MAXLINE], cty[MAXLINE];
    char *cfn, *rty;
    static int first = TRUE;

    cfn = dcl->proto.name;

/* declare the incomplete ones as external */
    if ((module_itf_wrappable(dcl) == FALSE) && (strcmp(ffn, "none") != 0))
       {map_name(dcn, MAXLINE, cfn, ffn, "f", -1, FALSE, FALSE);

	if (mc_need_ptr(dcl) == TRUE)
	   {if (first == TRUE)
	       {first = FALSE;
		fprintf(fp, "%s   use types_%s\n", ind, pck);
		fprintf(fp, "\n");};};

	rty = dcl->proto.type;
	mc_type(MAXLINE, fty, cty, NULL, &dcl->proto);
	if (strcmp(rty, "void") == 0)
	   fprintf(fp, "%s   external :: %s\n", ind, dcn);
        else
	  {if ((strcmp(cty, "C_PTR") == 0) || (strcmp(cty, "C_FUNPTR") == 0))
	      fprintf(fp, "%s   %s, external :: %s\n",
		      ind, C_PTR_RETURN, dcn);
	   else
	     fprintf(fp, "%s   %s, external :: %s\n", ind, fty, dcn);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MODULE_ITF_WRAP_FULL - write the Fortran interface for
 *                      - C function CFN
 */

static void module_itf_wrap_full(FILE *fp, fdecl *dcl, char *pck, char *ffn)
   {int i, ns, voidf;
    char dcn[MAXLINE], a[MAXLINE], t[MAXLINE];
    char fty[MAXLINE], cty[MAXLINE];
    char *cfn, *oper, **args;

    cfn = dcl->proto.name;

/* emit complete declarations */
    if ((module_itf_wrappable(dcl) == TRUE) && (strcmp(ffn, "none") != 0))
       {map_name(dcn, MAXLINE, cfn, ffn, "f", -1, FALSE, FALSE);

	voidf = dcl->voidf;
	oper  = (voidf == TRUE) ? "subroutine" : "function";

	mc_type(MAXLINE, fty, cty, NULL, &dcl->proto);
	mc_decl_list(a, MAXLINE, dcl);

#if defined(FORTRAN_BIND_C_ALL)
	snprintf(t, MAXLINE, "%s      %s %s(%s)", ind, oper, dcn, a);
	femit(fp, t, "&");

	snprintf(t, MAXLINE, "bind(c)");
	fprintf(fp, "%s                %s\n", ind, t);
#else
	snprintf(t, MAXLINE, "%s      %s %s(%s)", ind, oper, dcn, a);
	femit(fp, t, "");
#endif
	fprintf(fp, "%s         use iso_c_binding\n", ind);
	if (mc_need_ptr(dcl) == TRUE)
	   fprintf(fp, "%s         use types_%s\n", ind, pck);

	fprintf(fp, "%s         implicit none\n", ind);

/* return value declaration */
	if (voidf == FALSE)
	   {if ((strcmp(cty, "C_PTR") == 0) ||
		(strcmp(cty, "C_FUNPTR") == 0))
	       snprintf(t, MAXLINE, "%s         %-15s :: %s\n",
			ind, C_PTR_RETURN, dcn);
	    else
	       snprintf(t, MAXLINE, "%s         %-15s :: %s\n", ind, fty, dcn);

	    fputs(t, fp);};

/* argument declarations */
	args = dcl->tfproto;
	ns   = dcl->ntf;
	for (i = 2; i < ns; i += 2)
	    {snprintf(t, MAXLINE, "%s         %-15s :: %s\n",
		      ind, args[i], args[i+1]);
	     fputs(t, fp);};

	fprintf(fp, "%s      end %s %s\n", ind, oper, dcn);

	fprintf(fp, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MODULE_INTEROP_WRAP - generate FORTRAN/C interoperability interface for
 *                     - C function CFN
 */

static void module_interop_wrap(FILE *fp, fdecl *dcl, char *ffn)
   {int i, na, voidf, voida;
    char dcn[MAXLINE], a[MAXLINE];
    char fty[MAXLINE], cty[MAXLINE];
    char cd[MAXLINE], cb[MAXLINE];
    char *cfn, *nm, *oper;
    farg *al;

    cfn = dcl->proto.name;

    if (is_var_arg(dcl->proto.arg) == TRUE)
       berr("%s is not interoperable - variable args", cfn);

    else
       {map_name(dcn, MAXLINE, cfn, ffn, "i", -1, FALSE, FALSE);

	na    = dcl->na;
	al    = dcl->al;
	voida = dcl->voida;
	voidf = dcl->voidf;
	oper  = (voidf == TRUE) ? "subroutine" : "function";

	mc_type(MAXLINE, fty, cty, NULL, &dcl->proto);
	mc_decl_list(a, MAXLINE, dcl);

	snprintf(cd, MAXLINE, "%s      %s %s(%s)", ind, oper, dcn, a);
	femit(fp, cd, "&");

	snprintf(cb, MAXLINE, "bind(c, name='%s')", cfn);
	fprintf(fp, "%s                %s\n", ind, cb);

	fprintf(fp, "%s         use iso_c_binding\n", ind);
	fprintf(fp, "%s         implicit none\n", ind);
	if (voidf == FALSE)
	   fprintf(fp, "%s         %s(%s) :: %s\n", ind, fty, cty, dcn);

/* argument declarations */
	for (i = 0; i < na; i++)
	    {if ((voida == TRUE) && (i == 0))
	        continue;

	     mc_type(MAXLINE, fty, cty, NULL, al+i);
	     nm = al[i].name;
	     fprintf(fp, "%s         %s(%s), value :: %s\n",
		     ind, fty, cty, nm);};

	fprintf(fp, "%s      end %s %s\n", ind, oper, dcn);
	fprintf(fp, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_MODULE - generate Fortran/C interoperability interface
 *             - from CPR and SBI
 *             - return TRUE iff successful
 */

static int bind_module(bindes *bd)
   {int ib, iw, rv, ndcl;
    char t[MAXLINE];
    char *ffn, *sb, *pck;
    char **fpr, **fwr, **sa, **ta;;
    fdecl *dcl, *dcls;
    statedes *st;
    FILE *fp;

    rv = TRUE;
    st = bd->st;

    if (st->cfl & 2)
       {fp   = bd->fp;
	pck  = st->pck;
	fpr  = st->fpr;
	fwr  = st->fwr;
	dcls = st->dcl;
	ndcl = st->ndcl;

/* make simple external declaration for variable argument functions */
	fprintf(fp, "! ... external declarations for generated wrappers\n");
	for (ib = 0; ib < ndcl; ib++)
	    {dcl = dcls + ib;
	     ffn = has_binding(dcl, "fortran");
	     if (ffn != NULL)
	        module_itf_wrap_ext(fp, dcl, pck, ffn);};

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
	fprintf(fp, "%s   interface\n", ind);
	fprintf(fp, "\n");

/* make interface for native Fortran functions */
	if (fpr != NULL)
	   {fprintf(fp, "! ... declarations for native Fortran functions\n");
	    for (iw = 0, sa = fpr; TRUE; iw++)
	        {sa = module_native_f(fp, dcl, sa, pck);
		 if (sa == NULL)
		    break;};};

/* make full interface for non-variable argument functions */
	fprintf(fp, "! ... declarations for generated wrappers\n");
	for (ib = 0; ib < ndcl; ib++)
	    {dcl = dcls + ib;
	     ffn = has_binding(dcl, "fortran");
	     if (ffn != NULL)
	        module_itf_wrap_full(fp, dcl, pck, ffn);};

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
	for (ib = 0; ib < ndcl; ib++)
	    {dcl = dcls + ib;
	     ffn = has_binding(dcl, "fortran");
	     if (ffn != NULL)
	        module_interop_wrap(fp, dcl, ffn);};

/* finish the interface */
	fprintf(fp, "%s   end interface\n", ind);
	fprintf(fp, "\n");};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_MODULE - finalize Fortran/C interoperability interface module file */

static void fin_module(bindes *bd)
   {char *pck;
    statedes *st;
    FILE *fp;

    fp  = bd->fp;
    st  = bd->st;
    pck = st->pck;

    fprintf(fp, "%send module pact_%s\n", ind, pck);
    fprintf(fp, "\n");

    fclose(fp);

    bd->fp = NULL;

    return;}

/*--------------------------------------------------------------------------*/

/*                             SCHEME ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* CS_TYPE - return "Scheme" type corresponding to C type TY
 *         - to make SS_args call
 */

static void cs_type(char *a, int nc, farg *al, int drf)
   {char t[MAXLINE], ty[MAXLINE];
    char *sty, *dty;

    if (drf == TRUE)
       deref(ty, MAXLINE, al->type);
    else
       nstrncpy(ty, MAXLINE, al->type, -1);

    sty = lookup_type(NULL, ty, MODE_C, MODE_S);
    if (IS_NULL(sty) == FALSE)
       nstrncpy(a, nc, sty, -1);

    else if (strcmp(ty, "void *") == 0)
       nstrncpy(a, nc, "SC_VOID_I", -1);

    else if (is_func_ptr(ty, 3) == TRUE)
       nstrncpy(a, nc, "SC_POINTER_I", -1);

    else if (strchr(ty, '*') != NULL)
       {deref(t, MAXLINE, ty);
	dty = lookup_type(NULL, t, MODE_C, MODE_S);
	if (dty != NULL)
	   nstrncpy(a, nc, dty, -1);
	else
	   nstrncpy(a, nc, "SC_POINTER_I", -1);}

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

static void init_scheme(statedes *st, bindes *bd)
   {char fn[MAXLINE];
    char *pck;
    FILE *fp;

    pck = st->pck;

    if ((st->path == NULL) || (strcmp(st->path, ".") == 0))
       snprintf(fn, MAXLINE, "gs-%s.c", pck);
    else
       snprintf(fn, MAXLINE, "%s/gs-%s.c", st->path, pck);

    fp = open_file("w", fn);

    fprintf(fp, "\n");
    fprintf(fp, "#include \"sx_int.h\"\n");
    fprintf(fp, "#include \"%s_int.h\"\n", pck);
    fprintf(fp, "\n");

    csep(fp);

    bd->fp = fp;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_DECL - function declaration */

static void scheme_wrap_decl(FILE *fp, fdecl *dcl)
   {int na;
    char dcn[MAXLINE];

    na = dcl->na;

    nstrncpy(dcn, MAXLINE, dcl->proto.name, -1);
    downcase(dcn);

    fprintf(fp, "\n");
    fprintf(fp, "/* WRAP |%s| */\n", dcl->proto.arg);
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
				   fparam knd, char *so)
   {int i, na, nv, voida, voidf;
    char t[MAXLINE];
    char *rty;
    farg *al;
    idecl *ip;

    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;
    voidf = dcl->voidf;
    rty   = dcl->proto.type;
	
    nv = 0;
    for (i = 0; i <= na; i++)
        {if ((voida == TRUE) && (i == 0))
	    continue;

	 ip = &al[i].interp;

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
	 else if (IS_NULL(ip->decl) == FALSE)
           {nstrcat(t, MAXLINE, ip->decl);
            nv++;};

	 if (IS_NULL(t) == FALSE)
	    fputs(subst(t, "* ", "*", -1), fp);};

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_LOCAL_ASSN_DEF - assign default values to local variable AL */

static void scheme_wrap_local_assn_def(FILE *fp, farg *al)
   {char *defa;
    idecl *ip;

    ip   = &al->interp;
    defa = ip->defa;

    fputs(defa, fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_LOCAL_ASSN_ARG - add AL to SS_args call argument list */

static void scheme_wrap_local_assn_arg(char *a, int nc, farg *al)
   {char *arg;
    idecl *ip;

    ip  = &al->interp;
    arg = ip->argi;

    nstrcat(a, nc, arg);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_LOCAL_ASSN - local variable assignments */

static void scheme_wrap_local_assn(FILE *fp, fdecl *dcl)
   {int i, na, voida;
    char a[MAXLINE];
    farg *al;

    voida = dcl->voida;

    if (voida == FALSE)
       {na = dcl->na;
	al = dcl->al;

/* set the default values */
	for (i = 0; i < na; i++)
	    scheme_wrap_local_assn_def(fp, al+i);

/* make the SS_args call */
	a[0] = '\0';
	for (i = 0; i < na; i++)
	    scheme_wrap_local_assn_arg(a, MAXLINE, al+i);

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

    ty = dcl->proto.type;
    nm = dcl->proto.name;

    cf_call_list(a, MAXLINE, dcl, TRUE);

    if (strcmp(ty, "void") == 0)
       snprintf(t, MAXLINE, "    %s(%s);\n", nm, a);
    else
       snprintf(t, MAXLINE, "    _rv = %s(%s);\n", nm, a);

    fputs(t, fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_ARRAY_RETURN - compute possible array return from DCL */

static void scheme_array_return(char *t, int nc, fdecl *dcl)
   {int i, iv, na, nr, nvl;
    fdir dir;
    char a[MAXLINE], dty[MAXLINE];
    char *nm;
    farg *al;

    t[0] = '\0';
    a[0] = '\0';

    nr = dcl->nr;

/* make up the list arguments */
    if (nr > 0)
       {na = dcl->na;
	al = dcl->al;
        for (i = 0; i < na; i++)
	    {nvl = al[i].nv;
	     dir = al[i].dir;
	     if ((nvl > 0) && ((dir == FD_OUT) || (dir == FD_IN_OUT)))
	        {nm = al[i].name;
		 cs_type(dty, MAXLINE, al+i, TRUE);
	     
		 if (nvl == 1)
		    vstrcat(a, MAXLINE,
			    "                       %s, &_l%s,\n",
			    dty, nm);
		 else
		    {for (iv = 0; iv < nvl; iv++)
			 vstrcat(a, MAXLINE,
				 "                       %s, &_l%s[%d],\n",
				 dty, nm, iv);};};};};

/* if the list argument are non empty make up the call */
    if (IS_NULL(a) == FALSE)
       {snprintf(t, nc, "    _lo = SS_make_list(si,\n");
	nstrcat(t, nc, a);
	vstrcat(t, nc, "                       0);\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_SCALAR_RETURN - compute scalar return from DCL */

static void scheme_scalar_return(char *t, int nc,
				 fdecl *dcl, fparam knd, char *so)
   {char dty[MAXLINE];
    char *ty, *sty;

    t[0] = '\0';

    ty = dcl->proto.type;

    if (strcmp(ty, "void") == 0)
       snprintf(t, nc, "    _lo = SS_f;\n");
    else
       {if (IS_NULL(so) == FALSE)
	   {switch (knd)
	       {case FP_ANY :
		     sty = lookup_type(NULL, ty, MODE_C, MODE_S);
		     if ((sty != NULL) && (strcmp(sty, "SC_ENUM_I") == 0))
		        snprintf(t, nc, "    _lo = SS_mk_integer(si, _rv);\n");
		     else
		        {snprintf(t, nc,
				  "\n/* no way to return '%s' */\n", ty);
			 nstrcat(t, nc, "    _lo = SS_null;\n");};
		     break;

	        case FP_ARRAY :
		     deref(dty, MAXLINE, ty);
		     if (strcmp(dty, "void") == 0)
		        {snprintf(t, nc,
				  "    _sz = SC_arrlen(_rv);\n");
			 vstrcat(t, nc,
				 "    _arr = PM_make_array(\"char\", _sz, _rv);\n");}
		     else
		        {snprintf(t, nc,
				  "    _sz = SC_arrlen(_rv)/sizeof(%s);\n",
				  dty);
			 vstrcat(t, nc,
				 "    _arr = PM_make_array(\"%s\", _sz, _rv);\n",
				 dty);};
		     vstrcat(t, nc, "    _lo  = %s(si, _arr);\n", so);
		     break;
	        default :
                     if (strcmp(ty, "bool") == 0)
		        snprintf(t, nc, "    _lo = %s(si, \"g\", (int) _rv);\n",
				 so);
		     else
		        snprintf(t, nc, "    _lo = %s(si, _rv);\n", so);
		     break;};}
	else
	   snprintf(t, nc, "    _lo = SS_null;\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_LOCAL_RETURN - function return */

static void scheme_wrap_local_return(FILE *fp, fdecl *dcl,
				     fparam knd, char *so)
   {char t[MAXLINE];

    scheme_array_return(t, MAXLINE, dcl);
    if (IS_NULL(t) == TRUE)
       scheme_scalar_return(t, MAXLINE, dcl, knd, so);

    fputs(t, fp);

    fprintf(fp, "\n");

    fprintf(fp, "    return(_lo);}\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHEME_WRAP_INSTALL - add the installation of the function */

static char **scheme_wrap_install(char **fl, fdecl *dcl, char *sfn,
				  char **com)
   {int voida;
    char a[MAXLINE], t[MAXLINE], dcn[MAXLINE], ifn[MAXLINE];
    char *pi;

    voida = dcl->voida;

    nstrncpy(dcn, MAXLINE, dcl->proto.name, -1);
    downcase(dcn);

/* make the scheme interpreter name */
    pi = (strcmp(sfn, "yes") == 0) ? dcl->proto.name : sfn;
    map_name(ifn, MAXLINE, pi, sfn, NULL, -1, TRUE, FALSE);

/* prepare the function inline documenation */
    t[0] = '\0';
    if (com != NULL)
       concatenate(t, MAXLINE, com, " ");

    if (IS_NULL(t) == TRUE)
       {if_call_list(a, MAXLINE, dcl, NULL);
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
   {fparam knd;
    char so[MAXLINE];

    if ((is_var_arg(dcl->proto.arg) == FALSE) && (strcmp(sfn, "none") != 0))
       {csep(fp);

	knd = so_type(so, MAXLINE, dcl->proto.type);
	
/* function declaration */
	scheme_wrap_decl(fp, dcl);

/* local variable declarations */
	scheme_wrap_local_decl(fp, dcl, knd, so);

/* local variable assignments */
	scheme_wrap_local_assn(fp, dcl);

/* function call */
	scheme_wrap_local_call(fp, dcl);

/* function return */
	scheme_wrap_local_return(fp, dcl, knd, so);

	csep(fp);

/* add the installation of the function */
	fl = scheme_wrap_install(fl, dcl, sfn, com);};

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
   {int ib, ndcl, rv;
    char *sfn, *pck, **fl;
    fdecl *dcl, *dcls;
    statedes *st;
    FILE *fp;

    fp   = bd->fp;
    st   = bd->st;
    pck  = st->pck;
    dcls = st->dcl;
    ndcl = st->ndcl;

    rv = TRUE;
    fl = NULL;

    for (ib = 0; ib < ndcl; ib++)
        {dcl = dcls + ib;
	 sfn = has_binding(dcl, "scheme");
	 if (sfn != NULL)
	    fl = scheme_wrap(fp, fl, dcl, sfn, NULL);};

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

/* PY_FORMAT - convert <itype>, <var> pairs to Python parse format string */

static void py_format(char *fmt, int nc, char *spec, char *name)
   {int i;
    char t[MAXLINE];
    char *ty, *pf, **ta;

    memset(fmt, 0, nc);

    nstrncpy(t, MAXLINE, spec, -1);
    ta = tokenize(t, ", \t\n");
    if (ta != NULL)
       {pf = fmt;
        for (i = 0; ta[i] != NULL; i += 2)
	    {ty = ta[i];
	     if ((strcmp(ty, "SC_CHAR_I") == 0) ||
		 (strcmp(ty, "char") == 0))
	        *pf++ = 'b';
	     else if ((strcmp(ty, "SC_SHORT_I") == 0) ||
		      (strcmp(ty, "short") == 0))
	        *pf++ = 'h';
	     else if ((strcmp(ty, "SC_INT_I") == 0) ||
		      (strcmp(ty, "int") == 0))
	        *pf++ = 'i';
	     else if ((strcmp(ty, "SC_LONG_I") == 0) ||
		      (strcmp(ty, "long") == 0))
	        *pf++ = 'l';
	     else if ((strcmp(ty, "SC_LONG_LONG_I") == 0) ||
		      (strcmp(ty, "long long") == 0))
	        *pf++ = 'L';
	     else if ((strcmp(ty, "SC_FLOAT_I") == 0) ||
		      (strcmp(ty, "float") == 0))
	        *pf++ = 'f';
	     else if ((strcmp(ty, "SC_DOUBLE_I") == 0) ||
		      (strcmp(ty, "double") == 0))
	        *pf++ = 'd';
	     else if ((strcmp(ty, "SC_STRING_I") == 0) ||
		      (strcmp(ty, "char *") == 0))
	        *pf++ = 's';
	     else if (strcmp(ty, "SC_ENUM_I") == 0)
	        *pf++ = 'i';
	     else
	        *pf++ = 'O';};

	if (name != NULL)
	   vstrcat(fmt, nc, ":%s", name);

	free_strings(ta);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PY_ARG - convert <itype>, <var> pairs to Python call arg list string */

static void py_arg(char *arg, int nc, char *spec)
   {int i, n;
    char t[MAXLINE];
    char **ta;

    if (arg != NULL)
       {arg[0] = '\0';

	nstrncpy(t, MAXLINE, spec, -1);
	ta = tokenize(t, ", \t\n");
	if (ta != NULL)
	   {for (i = 1; ta[i] != NULL; i += 2)
	        vstrcat(arg, nc, "%s, ", ta[i]);

	    n = strlen(arg);
	    arg[n-2] = '\0';

	    free_strings(ta);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_DEF_STRUCTS - define local version of struct definitions */

static void python_def_structs(FILE *fp, statedes *st)
   {int ib, nbi;
    char t[MAXLINE];
    char *sb, *cty, *pty, **sbi, **sa;

    sbi = st->sbi;
    nbi = st->nbi;

/* make local struct definitions */
    for (ib = 0; ib < nbi; ib++)
        {sb = sbi[ib];
	 if (blank_line(sb) == FALSE)
	    {nstrncpy(t, MAXLINE, sb, -1);
	     sa = tokenize(t, " \t");
	     if ((sa != NULL) && (strcmp(sa[0], "derived") == 0))
	        {cty = sa[1];
		 pty = sa[4];
		 if ((IS_NULL(pty) == FALSE) &&
		     (strcmp(pty, "SC_ENUM_I") != 0) &&
		     (strcmp(pty, "PyObject") != 0))
		    {fprintf(fp, "typedef struct s_%s *%sp;\n", pty, pty);
		     fprintf(fp, "typedef struct s_%s %s;\n", pty, pty);
		     fprintf(fp, "\n");
		     fprintf(fp, "struct s_%s\n", pty);
		     fprintf(fp, "   {PyObject_HEAD\n");
		     fprintf(fp, "    %s *pobj;};\n", cty);
		     fprintf(fp, "\n");};};

	     free_strings(sa);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_PYTHON - initialize Python file */

static void init_python(statedes *st, bindes *bd)
   {char fn[MAXLINE];
    char *pck;
    FILE *fp;

    pck = st->pck;

    if ((st->path == NULL) || (strcmp(st->path, ".") == 0))
       snprintf(fn, MAXLINE, "gp-%s.c", pck);
    else
       snprintf(fn, MAXLINE, "%s/gp-%s.c", st->path, pck);

    fp = open_file("w", fn);

    fprintf(fp, "\n");
    fprintf(fp, "#include <Python.h>\n");
    fprintf(fp, "#include \"%s_int.h\"\n", pck);
    fprintf(fp, "#include \"sx_int.h\"\n");
    fprintf(fp, "\n");

    python_def_structs(fp, st);

    csep(fp);

    bd->fp = fp;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_MAKE_DECL - make wrapper function declaration */

static void python_make_decl(char *t, int nc, fdecl *dcl)
   {int i, na, n;
    char dty[MAXLINE], s[MAXLINE];
    char *cfn, *ty, *lty, *pty;
    farg *al;

    na  = dcl->na;
    cfn = dcl->proto.name;

    snprintf(t, nc, " *_PY_%s", cfn);
    n = strlen(t);
    memset(s, ' ', n);
    s[n] = '\0';

    pty = "PyObject";
    for (i = 0; i < na; i++)
        {al = dcl->al + i;
	 ty = al->type;
	 deref(dty, MAXLINE, ty);
	 lty = lookup_type(NULL, dty, MODE_C, MODE_P);
	 if ((lty != NULL) && (strcmp(lty, "SC_ENUM_I") != 0))
	    {pty = lty;
	     break;};};

    vstrcat(t, nc, "(%s *self, PyObject *args, PyObject *kwds)", pty);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_WRAP_DECL - function declaration */

static void python_wrap_decl(FILE *fp, fdecl *dcl)
   {char t[MAXLINE];

    fprintf(fp, "\n");
    fprintf(fp, "/* WRAP |%s| */\n", dcl->proto.arg);
    fprintf(fp, "\n");

    python_make_decl(t, MAXLINE, dcl);

    fprintf(fp, "PyObject%s\n", t);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_KW_LIST - compute the keyword list */

static void python_kw_list(char *kw, int nc, fdecl *dcl)
   {int i, na;
    farg *al;
    idecl *ip;

    na = dcl->na;
    al = dcl->al;
	
    kw[0] = '\0';

    for (i = 0; i < na; i++)
        {if (al[i].cls == FALSE)
	    {ip = &al[i].interp;
	     nstrcat(kw, MAXLINE, ip->argn);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_CLASS_SELF - emit the self object assignment */

static void python_class_self(FILE *fp, fdecl *dcl)
   {int i, na;
    char *nm;
    farg *al;

    na = dcl->na;
    al = dcl->al;
	
    for (i = 0; i < na; i++)
        {if (al[i].cls == TRUE)
	    {nm = al[i].name;
	     fprintf(fp, "    _l%s = self->pobj;\n\n", nm);
	     break;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_WRAP_LOCAL_DECL - local variable declarations */

static void python_wrap_local_decl(FILE *fp, fdecl *dcl, char *kw)
   {int i, na, voida, voidf;
    char t[MAXLINE];
    char *rty;
    farg *al;
    idecl *ip;

    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;
    voidf = dcl->voidf;
    rty   = dcl->proto.type;
	
    if ((voida == FALSE) && (IS_NULL(kw) == FALSE))
       {fprintf(fp, "   {int ok;\n");
	fprintf(fp, "    PyObject *_lo;\n");}
    else
       fprintf(fp, "   {PyObject *_lo;\n");

    for (i = 0; i <= na; i++)
        {if ((voida == TRUE) && (i == 0))
	    continue;

	 ip = &al[i].interp;

	 snprintf(t, MAXLINE, "    ");

/* variable for return value */
	 if (i == na)
	    {if (voidf == FALSE)
	        vstrcat(t, MAXLINE, "%s _rv;\n    ", rty);

	     if ((voida == FALSE) && (IS_NULL(kw) == FALSE))
	        vstrcat(t, MAXLINE, "char *kw_list[] = {%sNULL};\n", kw);}

/* local vars */
	 else if (IS_NULL(ip->decl) == FALSE)
	    nstrcat(t, MAXLINE, ip->decl);

	 if (IS_NULL(t) == FALSE)
	    fputs(subst(t, "* ", "*", -1), fp);};

    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_WRAP_LOCAL_ASSN_ARG - add AL to PyArg_ParseTupleAndKeywords call
 *                            - argument list
 */

static void python_wrap_local_assn_arg(char *a, int nc, farg *al)
   {char *arg;
    idecl *ip;

    if (al->cls == FALSE)
       {ip  = &al->interp;
        arg = ip->argi;
	nstrcat(a, nc, arg);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_WRAP_LOCAL_ASSN - local variable assignments */

static void python_wrap_local_assn(FILE *fp, fdecl *dcl, char *pfn, char *kw)
   {int i, na, voida;
    char a[MAXLINE], dcn[MAXLINE], fmt[MAXLINE], arg[MAXLINE];
    char *cfn;
    farg *al;

    cfn   = dcl->proto.name;
    na    = dcl->na;
    al    = dcl->al;
    voida = dcl->voida;

    python_class_self(fp, dcl);

    if ((voida == FALSE) && (IS_NULL(kw) == FALSE))
       {map_name(dcn, MAXLINE, cfn, pfn, "p", -1, FALSE, TRUE);

/* make the PyArg_ParseTupleAndKeywords call */
	a[0] = '\0';
	for (i = 0; i < na; i++)
	    python_wrap_local_assn_arg(a, MAXLINE, al+i);

	py_format(fmt, MAXLINE, a, dcn);
	fprintf(fp, "    ok = PyArg_ParseTupleAndKeywords(args, kwds,\n");
	fprintf(fp, "                                     \"%s\",\n", fmt);
	fprintf(fp, "                                     kw_list,\n");

	py_arg(arg, MAXLINE, a);
	fprintf(fp, "                                     %s);\n", arg);

	fprintf(fp, "    if (ok == FALSE)\n");
	fprintf(fp, "       return(NULL);\n");
	fprintf(fp, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_WRAP_LOCAL_CALL - function call */

static void python_wrap_local_call(FILE *fp, fdecl *dcl)
   {int voidf;
    char a[MAXLINE], t[MAXLINE];
    char *nm;

    voidf = dcl->voidf;
    nm    = dcl->proto.name;

    cf_call_list(a, MAXLINE, dcl, TRUE);

    if (voidf == TRUE)
       snprintf(t, MAXLINE, "    %s(%s);\n", nm, a);
    else
       snprintf(t, MAXLINE, "    _rv = %s(%s);\n", nm, a);

    fputs(t, fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_VALUE_RETURN - compute return value from DCL */

static void python_value_return(char *t, int nc, fdecl *dcl)
   {int i, iv, na, nr, nvl, voidf;
    fdir dir;
    char a[MAXLINE], dty[MAXLINE], fmt[MAXLINE], arg[MAXLINE];
    char *nm;
    farg *al;

    t[0] = '\0';
    a[0] = '\0';

    nr    = dcl->nr;
    voidf = dcl->voidf;

    if (voidf == FALSE)
       {cs_type(dty, MAXLINE, &dcl->proto, TRUE);
	vstrcat(a, MAXLINE, "\t\t\t%s, &_rv,\n", dty);};

/* make up the list arguments */
    if (nr > 0)
       {na = dcl->na;
	al = dcl->al;
        for (i = 0; i < na; i++)
	    {nvl = al[i].nv;
	     dir = al[i].dir;
	     if ((nvl > 0) && ((dir == FD_OUT) || (dir == FD_IN_OUT)))
	        {nm = al[i].name;
		 cs_type(dty, MAXLINE, al+i, TRUE);
	     
		 if (nvl == 1)
		    vstrcat(a, MAXLINE, "\t\t\t%s, &_l%s,\n", dty, nm);
		 else
		    {for (iv = 0; iv < nvl; iv++)
			 vstrcat(a, MAXLINE, "\t\t\t%s, &_l%s[%d],\n",
				 dty, nm, iv);};};};};

/* if the list argument are non empty make up the call */
    if (IS_NULL(a) == FALSE)
       {py_format(fmt, MAXLINE, a, NULL);
	py_arg(arg, MAXLINE, a);
	snprintf(t, nc, "    _lo = Py_BuildValue(\"%s\",\n", fmt);
	vstrcat(t, nc,  "                        %s);\n", arg);}
    else
       {snprintf(t, nc, "\n");
	nstrcat(t, nc, "    Py_INCREF(Py_None);\n");
	nstrcat(t, nc, "    _lo = Py_None;\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_WRAP_LOCAL_RETURN - function return */

static void python_wrap_local_return(FILE *fp, fdecl *dcl)
   {char t[MAXLINE];

    python_value_return(t, MAXLINE, dcl);
    if (IS_NULL(t) == TRUE)
       fprintf(fp, "    _lo = Py_None;\n");
    else
       fputs(t, fp);
    fprintf(fp, "\n");

    fprintf(fp, "    return(_lo);}\n");
    fprintf(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_METHOD_DEF - method definition wrapper */

static void python_method_def(char *dfn, int nc, fdecl *dcl,
			      char *pfn, int def)
   {int n, voida;
    char a[MAXLINE], s[MAXLINE], t[MAXLINE];
    char fn[MAXLINE];
    char *cfn, *nm;
    farg *alc;

    cfn   = dcl->proto.name;
    voida = dcl->voida;

    if (def == TRUE)
       {map_name(fn, MAXLINE, cfn, pfn, NULL, -1, FALSE, TRUE);

	alc = get_class_arg(dcl);

/* prepare the function inline documenation */
	if_call_list(a, MAXLINE, dcl, ",");
	if (alc != NULL)
	   {nm = alc->name;
	    if (voida == FALSE)
	       snprintf(t, MAXLINE, "Method: %s\\n     Usage: %s.%s(%s)",
			fn, nm, fn, a);
	    else
	       snprintf(t, MAXLINE, "Method: %s\\n     Usage: %s.%s()",
			fn, nm, fn);}
	else
	   {if (voida == FALSE)
	       snprintf(t, MAXLINE, "Procedure: %s\\n     Usage: %s(%s)",
			fn, fn, a);
	    else
	       snprintf(t, MAXLINE, "Procedure: %s\\n     Usage: %s()",
			fn, fn);};

/* emit the method definition */
	snprintf(s, MAXLINE, " _PYD_%s = ", cfn);
	n = strlen(s) + 1;

	snprintf(dfn, nc, "{\"%s\", (PyCFunction) _PY_%s, METH_KEYWORDS,\n",
		 fn, cfn);

	memset(s, ' ', n);
	s[n] = '\0';
	vstrcat(dfn, nc, "%s\"%s\"}", s, t);}

/* emit the method declaration */
    else
       snprintf(dfn, nc, " _PYD_%s", cfn);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_WRAP - wrap C function DCL in PYTHON callable function
 *             - using name PFN
 */

static void python_wrap(FILE *fp, fdecl *dcl, char *pfn)
   {char upn[MAXLINE], kw[MAXLINE];

    if ((is_var_arg(dcl->proto.arg) == FALSE) && (strcmp(pfn, "none") != 0))
       {nstrncpy(upn, MAXLINE, pfn, -1);
	upcase(upn);

	csep(fp);

	python_kw_list(kw, MAXLINE, dcl);

/* function declaration */
	python_wrap_decl(fp, dcl);

/* local variable declarations */
	python_wrap_local_decl(fp, dcl, kw);

/* local variable assignments */
	python_wrap_local_assn(fp, dcl, pfn, kw);

/* function call */
	python_wrap_local_call(fp, dcl);

/* function return */
	python_wrap_local_return(fp, dcl);

	csep(fp);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_HEADER - write the header with declarations for Python wrappers */

static void python_header(bindes *bd)
   {int ib, ndcl;
    char name[MAXLINE], upk[MAXLINE], dfn[MAXLINE], t[MAXLINE];
    char *pck, *cfn, *pfn;
    fdecl *dcl, *dcls;
    statedes *st;
    FILE *fh;

    st   = bd->st;
    pck  = st->pck;
    dcls = st->dcl;
    ndcl = st->ndcl;

    nstrncpy(upk, MAXLINE, pck, -1);
    upcase(upk);

    if ((st->path == NULL) || (strcmp(st->path, ".") == 0))
       snprintf(name, MAXLINE, "py-%s.h", pck);
    else
       snprintf(name, MAXLINE, "%s/py-%s.h", st->path, pck);

    fh = fopen(name, "w");
    if (fh == NULL)
       return;

    fprintf(fh, "/*\n");
    fprintf(fh, " * PY-%s.H - generated header for %s bindings\n", upk, pck);
    fprintf(fh, " *\n");
    fprintf(fh, " */\n");
    fprintf(fh, "\n");

/* extern declarations for wrappers */
    fprintf(fh, "extern PyObject\n");

    for (ib = 0; ib < ndcl; ib++)
        {dcl = dcls + ib;
	 pfn = has_binding(dcl, "python");
	 if ((pfn != NULL) && (strcmp(pfn, "none") != 0) &&
	     (is_var_arg(dcl->proto.arg) == FALSE))
	    {python_make_decl(t, MAXLINE, dcl);
	     fprintf(fh, "%s,\n", t);};};

    fprintf(fh, " _PY_%s_null;\n", pck);
    fprintf(fh, "\n");

/* method definition macros */
    for (ib = 0; ib < ndcl; ib++)
        {dcl = dcls + ib;
	 cfn = dcl->proto.name;
	 pfn = has_binding(dcl, "python");
	 if ((pfn != NULL) && (strcmp(pfn, "none") != 0) &&
	     (is_var_arg(dcl->proto.arg) == FALSE))
	    {python_method_def(dfn, MAXLINE, dcl, pfn, TRUE);
	     fprintf(fh, "#define _PYD_%s %s\n",
		     cfn, subst(dfn, "\n", "\\\n", -1));};};

    fprintf(fh, "\n");

    fclose(fh);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PYTHON_INSTALL - write the routine to install the bindings */

static void python_install(bindes *bd)
   {int ib, ndcl;
    char dfn[MAXLINE];
    char *pck, *cfn, *pfn;
    fdecl *dcl, *dcls;
    statedes *st;
    FILE *fp;

    fp   = bd->fp;
    st   = bd->st;
    pck  = st->pck;
    dcls = st->dcl;
    ndcl = st->ndcl;

    csep(fp);

    fprintf(fp, "\n");
    fprintf(fp, "PyMethodDef\n");

/* method definition wrapper */
    for (ib = 0; ib < ndcl; ib++)
        {dcl = dcls + ib;
	 cfn = dcl->proto.name;
	 pfn = has_binding(dcl, "python");
	 if ((pfn != NULL) && (strcmp(pfn, "none") != 0) &&
	     (is_var_arg(dcl->proto.arg) == FALSE))
	    {python_method_def(dfn, MAXLINE, dcl, pfn, TRUE);
	     fprintf(fp, " _PYD_%s = %s,\n", cfn, dfn);};};

    fprintf(fp, " _PYD_%s_null;\n", pck);
    fprintf(fp, "\n");
    csep(fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_PYTHON - generate Python bindings from CPR and SBI
 *             - return TRUE iff successful
 */

static int bind_python(bindes *bd)
   {int ib, ndcl, rv;
    char *pfn;
    fdecl *dcl, *dcls;
    statedes *st;
    FILE *fp;

    fp   = bd->fp;
    st   = bd->st;
    dcls = st->dcl;
    ndcl = st->ndcl;

    rv = TRUE;

    for (ib = 0; ib < ndcl; ib++)
        {dcl = dcls + ib;
	 pfn = has_binding(dcl, "python");
	 if (pfn != NULL)
	    python_wrap(fp, dcl, pfn);};

/* write the method definitions */
    python_install(bd);

/* write the method declarations */
    python_header(bd);

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

static void init_doc(statedes *st, bindes *bd)
   {char fn[MAXLINE];
    char *pck;
    FILE *fp;

    pck = st->pck;

    if ((st->path == NULL) || (strcmp(st->path, ".") == 0))
       snprintf(fn, MAXLINE, "gh-%s.html", pck);
    else
       snprintf(fn, MAXLINE, "%s/gh-%s.html", st->path, pck);

    fp = open_file("w", fn);

    fprintf(fp, "\n");

    hsep(fp);

    bd->fp = fp;

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

/* DOC_PROTO_PYTHON - render the arg list of DCL into A for the
 *                  - Python callable C wrapper
 */

static void doc_proto_python(char *a, int nc, char *dcn, fdecl *dcl)
   {char t[MAXLINE];
    farg *alc;

    alc = get_class_arg(dcl);

    a[0] = '\0';

    if (alc != NULL)
       vstrcat(a, nc, "%s.%s", alc->name, dcn);
    else
       vstrcat(a, nc, "%s", dcn);

    if_call_list(t, MAXLINE, dcl, ",");
    vstrcat(a, nc, "(%s)", t);

#if 0
    int i, na;
    farg *al;

    na = dcl->na;
    al = dcl->al;

    vstrcat(a, nc, "%s(", dcn);
    if (na != 0)
       {for (i = 0; i < na; i++)
	    {if (al[i].cls == FALSE)
		vstrcat(a, MAXLINE, "%s, ", al[i].name);};
	a[strlen(a)-2] = '\0';};
    nstrcat(a, nc, ")");
#endif

    memmove(a, trim(a, BOTH, " "), nc);

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

/* FIND_COMMENT - return a pointer to the part of CDC which
 *              - documents CFN
 */

static char **find_comment(char *cfn, int ndc, char **cdc)
   {int i;
    char *s, **com;

    com = NULL;
    for (i = 0; (i < ndc) && (com == NULL); i++)
        {s = cdc[i];
	 if ((strstr(s, "#bind") != NULL) &&
	     (strstr(s, cfn) != NULL))
	    {for ( ; i >= 0; i--)
	         {s = cdc[i];
                  if (strncmp(s, "/*", 2) == 0)
		     {com = cdc + i;
		      break;};};};};

    return(com);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROCESS_DOC - process the comments into a string */

static void process_doc(char *t, int nc, char **com)
   {int i;
    char *s, *p;

    t[0] = '\0';
    if (com != NULL)
       {for (i = 0; IS_NULL(com[i]) == FALSE; i++)
	    {s = com[i] + 3;
	     if (strstr(s, "#bind ") == NULL)
	        {p = strchr(s, '-');
		 if (p != NULL)
		    vstrcat(t, nc, "%s\n", p+2);
		 else
		    nstrcat(t, nc, "\n");};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HTML_WRAP - wrap C function DCL in HTML form */

static void html_wrap(FILE *fp, fdecl *dcl, char *sb, int ndc, char **cdc)
   {int voidf;
    char pr[MAXLINE];
    char upn[MAXLINE], lfn[MAXLINE], dcn[MAXLINE];
    char af[MAXLINE], as[MAXLINE], ap[MAXLINE];
    char fty[MAXLINE], t[MAXLINE];
    char *cfn, *bfn, **args, **com;

    cfn = dcl->proto.name;

    com = find_comment(cfn, ndc, cdc);

    voidf = dcl->voidf;

    nstrncpy(upn, MAXLINE, cfn, -1);
    upcase(upn);

    nstrncpy(lfn, MAXLINE, cfn, -1);
    downcase(lfn);

    cf_type(fty, MAXLINE, dcl->proto.type);

    hsep(fp);
    fprintf(fp, "\n");

    fprintf(fp, "<a name=\"%s\"><h2>%s</h2></a>\n", lfn, upn);
    fprintf(fp, "\n");
    fprintf(fp, "<p>\n");
    fprintf(fp, "<pre>\n");

/* C */
    c_proto(pr, MAXLINE, dcl);
    fprintf(fp, "<i>C Binding: </i>       %s\n", pr);

/* Fortran */
    bfn = has_binding(dcl, "fortran");
    if (bfn == NULL)
       fprintf(fp, "<i>Fortran Binding: </i> none\n");
    else if (dcl->bindings != NULL)
       {args = dcl->tfproto;

	doc_proto_fortran(af, MAXLINE, dcl);
	map_name(dcn, MAXLINE, args[1], bfn, "f", -1, FALSE, FALSE);
	if (voidf == TRUE)
	   fprintf(fp, "<i>Fortran Binding: </i> %s(%s)\n",
		   dcn, af);
	else
	   fprintf(fp, "<i>Fortran Binding: </i> %s %s(%s)\n",
		   args[0], dcn, af);};

/* Scheme */
    bfn = has_binding(dcl, "scheme");
    if (bfn == NULL)
       fprintf(fp, "<i>SX Binding: </i>      none\n");
    else if (dcl->bindings != NULL)
       {map_name(dcn, MAXLINE, cfn, bfn, NULL, -1, TRUE, FALSE);
	doc_proto_name_only(as, MAXLINE, dcl, NULL);
	if (IS_NULL(as) == TRUE)
	   fprintf(fp, "<i>SX Binding: </i>      (%s)\n", dcn);
	else
	   fprintf(fp, "<i>SX Binding: </i>      (%s %s)\n", dcn, as);};

/* Python */
    bfn = has_binding(dcl, "python");
    if (bfn == NULL)
       fprintf(fp, "<i>Python Binding: </i>  none\n");
    else if (dcl->bindings != NULL)
       {map_name(dcn, MAXLINE, cfn, bfn, NULL, -1, FALSE, TRUE);
	doc_proto_python(ap, MAXLINE, dcn, dcl);
	fprintf(fp, "<i>Python Binding: </i>  %s\n", ap);};

    fprintf(fp, "</pre>\n");
    fprintf(fp, "<p>\n");

    process_doc(t, MAXLINE, com);
    if (IS_NULL(t) == FALSE)
       {fprintf(fp, "<pre>\n");
	fputs(t, fp);
	fprintf(fp, "</pre>\n");};

    fprintf(fp, "<p>\n");
    fprintf(fp, "\n");
    hsep(fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAN_WRAP - wrap C function DCL in MAN form */

static void man_wrap(statedes *st, fdecl *dcl,
		     char *sb, char *pck, int ndc, char **cdc)
   {int voidf;
    char fname[MAXLINE], upk[MAXLINE], pr[MAXLINE];
    char upn[MAXLINE], lfn[MAXLINE], dcn[MAXLINE];
    char af[MAXLINE], as[MAXLINE], ap[MAXLINE];
    char fty[MAXLINE], t[MAXLINE];
    char *cfn, *bfn, **args, **com;
    FILE *fp;

    cfn   = dcl->proto.name;
    voidf = dcl->voidf;

    com = find_comment(cfn, ndc, cdc);

    nstrncpy(upk, MAXLINE, pck, -1);
    upcase(upk);

    nstrncpy(upn, MAXLINE, cfn, -1);
    upcase(upn);

    nstrncpy(lfn, MAXLINE, cfn, -1);
    downcase(lfn);

    cf_type(fty, MAXLINE, dcl->proto.type);

    if ((st->path == NULL) || (strcmp(st->path, ".") == 0))
       snprintf(fname, MAXLINE, "%s.3", cfn);
    else
       snprintf(fname, MAXLINE, "%s/%s.3", st->path, cfn);

    fp = fopen(fname, "w");
    if (fp == NULL)
       return;

    fprintf(fp, ".\\\"\n");
    fprintf(fp, ".\\\" See the terms of include/cpyright.h\n");
    fprintf(fp, ".\\\"\n");
    fprintf(fp, ".TH %s 3  2011-06-21 \"%s\" \"%s Documentation\"\n",
	    upn, upk, upk);
    fprintf(fp, ".SH NAME\n");
    fprintf(fp, "%s \\- \n", cfn);
    fprintf(fp, ".SH SYNOPSIS\n");
    fprintf(fp, ".nf\n");

    fprintf(fp, ".B #include <%s.h>\n", pck);
    fprintf(fp, ".sp\n");

/* C */
    c_proto(pr, MAXLINE, dcl);
    fprintf(fp, ".B C Binding:       %s\n", pr);
    fprintf(fp, ".sp\n");

/* Fortran */
    bfn = has_binding(dcl, "fortran");
    if (bfn == NULL)
       {fprintf(fp, ".B Fortran Binding: none\n");
	fprintf(fp, ".sp\n");}
    else if (dcl->bindings != NULL)
       {args = dcl->tfproto;

	doc_proto_fortran(af, MAXLINE, dcl);
	map_name(dcn, MAXLINE, args[1], bfn, "f", -1, FALSE, FALSE);
	if (voidf == TRUE)
	   fprintf(fp, ".B Fortran Binding: %s(%s)\n",
		   dcn, af);
	else
	   fprintf(fp, ".B Fortran Binding: %s %s(%s)\n",
		   args[0], dcn, af);

	fprintf(fp, ".sp\n");};

/* Scheme */
    bfn = has_binding(dcl, "scheme");
    if (bfn == NULL)
       {fprintf(fp, ".B SX Binding:      none\n");
	fprintf(fp, ".sp\n");}
    else if (dcl->bindings != NULL)
       {map_name(dcn, MAXLINE, cfn, bfn, NULL, -1, TRUE, FALSE);
	doc_proto_name_only(as, MAXLINE, dcl, NULL);
	if (IS_NULL(as) == TRUE)
	   fprintf(fp, ".B SX Binding:      (%s)\n", dcn);
	else
	   fprintf(fp, ".B SX Binding:      (%s %s)\n", dcn, as);

	fprintf(fp, ".sp\n");};

/* Python */
    bfn = has_binding(dcl, "python");
    if (bfn == NULL)
       {fprintf(fp, ".B Python Binding:  none\n");
	fprintf(fp, ".sp\n");}
    else if (dcl->bindings != NULL)
       {map_name(dcn, MAXLINE, cfn, bfn, NULL, -1, FALSE, TRUE);
	doc_proto_python(ap, MAXLINE, dcn, dcl);
	fprintf(fp, ".B Python Binding:  %s\n", ap);
	fprintf(fp, ".sp\n");};

    process_doc(t, MAXLINE, com);
    if (IS_NULL(t) == FALSE)
       {fprintf(fp, ".fi\n");
	fprintf(fp, ".SH DESCRIPTION\n");
	fprintf(fp, "%s\n", t);};

    if (voidf == TRUE)
       {fprintf(fp, ".SH RETURN VALUE\n");
	fprintf(fp, "none\n");
	fprintf(fp, ".sp\n");};

    fprintf(fp, ".SH COPYRIGHT\n");
    fprintf(fp, "include/cpyright.h\n");
    fprintf(fp, ".sp\n");

    fprintf(fp, "\n");

    fclose(fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_DOC - generate Doc bindings from CPR and SBI
 *          - return TRUE iff successful
 */

static int bind_doc(bindes *bd)
   {int ib, ndc, ndcl, rv;
    char *pck, **cdc;
    fdecl *dcl, *dcls;
    statedes *st;
    FILE *fp;

    fp   = bd->fp;
    st   = bd->st;
    ndc  = st->ndc;
    cdc  = st->cdc;
    pck  = st->pck;
    dcls = st->dcl;
    ndcl = st->ndcl;

    rv = TRUE;

    for (ib = 0; ib < ndcl; ib++)
        {dcl = dcls + ib;
	 if (dcl->error == FALSE)
	    {html_wrap(fp, dcl, NULL, ndc, cdc);
	      man_wrap(st, dcl, NULL, pck, ndc, cdc);};};

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

/*                              BASIS ROUTINES                                */

/*--------------------------------------------------------------------------*/

/* INIT_BASIS - initialize Basis file */

static void init_basis(statedes *st, bindes *bd)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_BASIS - generate Basis bindings from CPR and SBI
 *            - return TRUE iff successful
 */

static int bind_basis(bindes *bd)
   {int rv;

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_BASIS - finalize Basis file */

static void fin_basis(bindes *bd)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BLANG - control the generation of language binding
 *       - return TRUE iff successful
 */

static int blang(char *pck, char *pth, int cfl, char *fbi,
		 char *cdc, char *cpr, char *fpr, char *fwr,
		 int *no)
   {int i, ib, nb, rv;
    char **sbi, **sdc, **scp, **sfp, **swr;
    bindes *pb;
    statedes st = {0, 0, 0, 0, 0, 0, 0,
                   {FALSE, FALSE, FALSE, FALSE},
		   NULL, NULL, NULL, NULL, NULL, NULL, NULL, };
    bindes bd[] = { {&st, NULL, init_fortran, bind_fortran, fin_fortran},
		    {&st, NULL, init_module, bind_module, fin_module},
		    {&st, NULL, init_scheme, bind_scheme, fin_scheme},
		    {&st, NULL, init_python, bind_python, fin_python},
		    {&st, NULL, init_doc, bind_doc, fin_doc},
		    {&st, NULL, init_basis, bind_basis, fin_basis} };

    rv = FALSE;

    for (i = 0; i < N_MODES; i++)
        st.no[i] = no[i];

    nstrncpy(st.path, MAXLINE, pth, -1);

/* no documents */
    if (no[0] == TRUE)
       {bd[4].init = NULL;
	bd[4].bind = NULL;
	bd[4].fin  = NULL;};

/* no fortran */
    if (no[1] == TRUE)
       {bd[0].init = NULL;
	bd[0].bind = NULL;
	bd[0].fin  = NULL;
	bd[1].init = NULL;
	bd[1].bind = NULL;
	bd[1].fin  = NULL;};

/* no python */
    if (no[2] == TRUE)
       {bd[3].init = NULL;
	bd[3].bind = NULL;
	bd[3].fin  = NULL;};

/* no scheme */
    if (no[3] == TRUE)
       {bd[2].init = NULL;
	bd[2].bind = NULL;
	bd[2].fin  = NULL;};

    if ((IS_NULL(cpr) == FALSE) && (IS_NULL(fbi) == FALSE))
       {sbi = file_text(FALSE, fbi);
	sdc = file_text(FALSE, cdc);
	scp = file_text(FALSE, cpr);
	sfp = file_text(FALSE, fpr);
	swr = file_text(FALSE, fwr);

	if (scp == NULL)
	   printf("No prototypes found for '%s'\n", pck);

        else if (sbi == NULL)
	   printf("No bindings found for '%s'\n", pck);
            
	else
	   {add_derived_types(sbi);

	    if (scp != NULL)
	       {nb = sizeof(bd)/sizeof(bindes);

		setup_binder(&st, pck, cfl, sbi, sdc, scp, sfp, swr);

/* initialize the binding constructors */
		for (pb = bd, ib = 0; ib < nb; ib++, pb++)
		    {if (pb->init != NULL)
		        pb->init(&st, pb);};

/* make all the language bindings */
		if (sbi != NULL)
		   {for (pb = bd, ib = 0; ib < nb; ib++, pb++)
		        {if (pb->bind != NULL)
			    pb->bind(pb);};};

/* cleanup */
		for (pb = bd, ib = 0; ib < nb; ib++, pb++)
		    {if (pb->fin != NULL)
		        pb->fin(pb);};

		rv = TRUE;};};

	free_strings(sbi);
	free_strings(sdc);
	free_strings(scp);
	free_strings(sfp);
	free_strings(swr);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it out here */

int main(int c, char **v)
   {int i, rv, cfl;
    int no[N_MODES];
    char pck[MAXLINE], msg[MAXLINE];
    char *fbi, *cdc, *cpr, *fpr, *fwr, *pth;

    istrl = "int";
    pth   = ".";
    fbi   = "";
    cpr   = "";
    cdc   = "";
    fpr   = "";
    fwr   = "";
    cfl   = 3;

    memset(no, 0, sizeof(no));

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-b") == 0)
	    fbi = v[++i];
	 else if (strcmp(v[i], "-c") == 0)
	    cpr = v[++i];
	 else if (strcmp(v[i], "-d") == 0)
	    cdc = v[++i];
	 else if (strcmp(v[i], "-f") == 0)
	    fpr = v[++i];
	 else if (strcmp(v[i], "-h") == 0)
            {printf("Usage: blang -b <bindings> -c <c-proto> [-d <doc>] [-f <f-proto>] [-h] [-l] [-nob] [-nod] [-nof] [-nop] [-nos] [-p <dir>] [-w <f-wrapper>] [-wr]\n");
             printf("   b    file containing binding specifications\n");
             printf("   c    file containing C prototypes\n");
             printf("   d    file containing documentation comments\n");
             printf("   f    file containing Fortran prototypes\n");
             printf("   h    this help message\n");
             printf("   l    use long for Fortran implicit arguments\n");
             printf("   nod  do not generate documentation\n");
             printf("   nof  do not generate fortran interfaces\n");
             printf("   nop  do not generate python interfaces\n");
             printf("   nos  do not generate scheme interfaces\n");
             printf("   o    no interoprabilty interfaces (Fortran wrappers only)\n");
             printf("   p    directory for generated files\n");
             printf("   w    file containing Fortran wrapper specifications\n");
             printf("   wr   no Fortran wrappers (interoperability only)\n");
             printf("\n");}
	 else if (strcmp(v[i], "-l") == 0)
            istrl = "long";
	 else if (strcmp(v[i], "-nob") == 0)
	    no[4] = TRUE;
	 else if (strcmp(v[i], "-nod") == 0)
	    no[0] = TRUE;
	 else if (strcmp(v[i], "-nof") == 0)
	    no[1] = TRUE;
	 else if (strcmp(v[i], "-nop") == 0)
	    no[2] = TRUE;
	 else if (strcmp(v[i], "-nos") == 0)
	    no[3] = TRUE;
	 else if (strcmp(v[i], "-o") == 0)
            cfl &= ~2;
	 else if (strcmp(v[i], "-p") == 0)
            pth = v[++i];
	 else if (strcmp(v[i], "-w") == 0)
	    fwr = v[++i];
	 else if (strcmp(v[i], "-wr") == 0)
            cfl &= ~1;};

    snprintf(pck, MAXLINE, "%s", path_base(path_tail(fbi)));
    snprintf(msg, MAXLINE, "%s bindings", pck);

    printf("      %s ", fill_string(msg, 25));

    init_types();

    rv = blang(pck, pth, cfl, fbi, cdc, cpr, fpr, fwr, no);
    rv = (rv != TRUE);

    printf("done\n");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

