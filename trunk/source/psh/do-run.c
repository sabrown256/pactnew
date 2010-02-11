/*
 * DO-RUN - portable driver for parallel programs
 *        - this includes debugging and batch support
 *        - NOTE: the design goal it to provide a common front end
 *        -       to distributed parallel (DP), thread parallel (SMP),
 *        -       and debuggers.  In such a way that if you run a code that
 *        -       has MPI linked in but never called (usually an option)
 *        -       in a session, making it essentially a non-parallel run,
 *        -       then the MPI frontends and other restrictive features
 *        -       of distributed parallel code runs may be avoided.
 *        -       a "parallel" run with one process is treated as a
 *        -       special case (see the -f option)
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#include "common.h"
#include "libpsh.c"
#include "libtime.c"
#include "libasync.c"
#include "libeval.c"

#undef MAXLINE
#define MAXLINE 512

typedef struct s_rundes rundes;

struct s_rundes
   {int conditional;     /* (un)conditionally call MPI_Init iff (FALSE)TRUE */
    int debug;
    int dryrun;
    int usempi;
    int ncpu;
    int errio;           /* (un)suppress stderr iff (TRUE)FALSE */
    int trace;
    int verbose;
    char bargs[MAXLINE];
    char dargs[MAXLINE];
    char bindir[MAXLINE];
    char os[MAXLINE];
    char host[MAXLINE];
    char dbgtgt[MAXLINE];
    char crosstgt[MAXLINE];
    char mpitgt[MAXLINE];
    char sgn[MAXLINE];
    char mpife[MAXLINE];};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TERMINATE - cleanup properly at session end */

static void terminate(int st)
   {

    if (cdefenv("TVDirs") == TRUE)
       unlink(cgetenv(TRUE, "TVDirs"));

    exit(st);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LOAD_TARGET - load the target string TGT from the list LST */

static void load_target(char *tgt, int nc, char *lst)
   {int i, n;
    char *exe, **tok;

    tok = NULL;
    FOREACH(s, lst, " :")
       tok = lst_push(tok, s);
    ENDFOR;

    n = lst_length(tok);

    tgt[0] = '\0';
    for (i = 0; i < n; i++)
        {exe = cwhich(tok[i]);
	 if (file_executable(exe) == TRUE)
	    {nstrncpy(tgt, nc, tok[i], -1);
	     break;};};

    lst_free(tok);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_DBG_TARGET - initialize DBG default target */

static void init_dbg_target(rundes *st)
   {char clst[MAXLINE];

    if (st->dbgtgt[0] == '\0')
       {if (cdefenv("DO_RUN_DBG") == TRUE)
	   nstrncpy(clst, MAXLINE, cgetenv(TRUE, "DO_RUN_DBG"), -1);
        else if (cdefenv("SC_DEBUGGER") == TRUE)
	   nstrncpy(clst, MAXLINE, cgetenv(TRUE, "SC_DEBUGGER"), -1);
	else
	   nstrncpy(clst, MAXLINE, "totalview gdb dbx", -1);

	load_target(st->dbgtgt, MAXLINE, clst);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_DBG - setup the debugger */

static void init_dbg(rundes *st)
   {

    if (cdefenv("DBG") == FALSE)
       csetenv("DBG", st->dbgtgt);

    if ((strcmp(st->os, "AIX") == 0) && (cmpenv("DBG", "zf") == 0))
       csetenv("DBG", "");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_MPI_TARGET - initialize MPI default target */

static void init_mpi_target(rundes *st)
   {char clst[MAXLINE];

    if (st->mpitgt[0] == '\0')
       {if (cdefenv("DO_RUN_MPI") == TRUE)
	   nstrncpy(clst, MAXLINE, cgetenv(TRUE, "DO_RUN_MPI"), -1);
        else
	   nstrncpy(clst, MAXLINE, "poe srun mpirun dmpirun", -1);

	load_target(st->mpitgt, MAXLINE, clst);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_MPI - setup for MPI */

static void init_mpi(rundes *st)
   {int ok;
    char t[MAXLINE];
    char *s, *c, *p;
    FILE *fp;

    c = cgetenv(TRUE, "Code");

    ok = TRUE;

/* figure out whether we have an MPI code */

/* a script cannot be an MPI code */
    if (file_script(c) == TRUE)
       {ok = FALSE;
        csetenv("MPI", "");};

/* a code without MPI_Init cannot be an MPI code */
    if (cdefenv("MPI") == 0)
       {s = run(FALSE, "mpi-info -x %s", c);
	if (strcmp(s, "no") == 0)
	   {ok = FALSE;
	    csetenv("MPI", "");}
	else
	   csetenv("MPI", st->mpitgt);};

/* unless forbidden set MPI to some non-empty frontend
 * the dryrun exception is for testing purposes and must be supported
 */
    if ((cmpenv("MPI", "") == 0) && ((ok == TRUE) || (st->dryrun == TRUE)))
       csetenv("MPI", st->mpitgt);

/* figure out the I/O wrapper */
    if (cmpenv("MPI", "") == 0)
       csetenv("Wrap", "");

    else if (strcmp(st->os, "AIX") == 0)
       {fp = open_file("w", ".dp-aix.%s.%d", st->host, getpid());
	fprintf(fp, "%s\n", st->host);
	fclose(fp);};

/* if we are MPI then we are not CROSS */
    if (cmpenv("MPI", "") != 0)
       {csetenv("CROSS", "");
	csetenv("CFE", "");};

/* find the corresponding frontend */
    if (cdefenv("MPIFE") == 0)
       {s = run(FALSE, "mpi-info -b %s", c);
	p = strtok(s, " \t\n");
	csetenv("MPIFE", s);
	nstrncpy(t, MAXLINE, s, -1);
	for (p = t + strlen(t) - 1; (p >= t) && (*p != '/'); p--);
        if (p >= t)
	   *p = '\0';

	push_path(PREPEND, lpath, t);};

/* find the corresponding lib */
    if (cdefenv("MPILib") == 0)
       {s = run(FALSE, "mpi-info -l %s", c);
	csetenv("MPILib", s);};

    if (st->usempi == FALSE)
       csetenv("MPI", "-none-");

    if (st->trace > 3)
       {printf("MPICnd = %s\n", cgetenv(TRUE, "MPICnd"));
	printf("MPIFE  = %s\n", cgetenv(TRUE, "MPIFE"));
	printf("MPILib = %s\n", cgetenv(TRUE, "MPILib"));
	printf("Wrap   = %s\n", cgetenv(TRUE, "Wrap"));};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_CROSS_TARGET - initialize CROSS default target */

static void init_cross_target(rundes *st)
   {char clst[MAXLINE];

    if (st->crosstgt[0] == '\0')
       {if (cdefenv("DO_RUN_CROSS") == TRUE)
	   nstrncpy(clst, MAXLINE, cgetenv(TRUE, "DO_RUN_CROSS"), -1);
        else
	   clst[0] = '\0';

	load_target(st->crosstgt, MAXLINE, clst);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_CROSS - setup for CROSS */

static void init_cross(rundes *st)
   {char t[MAXLINE];
    char *s, *c, *p;

    c = cgetenv(TRUE, "Code");

/* figure out whether we have an CROSS code */
    if (file_script(c) == TRUE)
       csetenv("CROSS", "");

    if (cdefenv("CROSS") == 0)
       {s = run(FALSE, "cross-info -x %s", c);
	nstrncpy(st->crosstgt, MAXLINE, s, -1);
	if (strcmp(st->crosstgt, "local") != 0)
	   csetenv("CROSS", st->crosstgt);};

/* find the corresponding frontend */
    if (cdefenv("CFE") == 0)
       {s = run(FALSE, "cross-info -b %s", c);
	p = strtok(s, " \t\n");
	csetenv("CFE", s);
	nstrncpy(t, MAXLINE, s, -1);
	for (p = t + strlen(t) - 1; (p >= t) && (*p != '/'); p--);
        if (p >= t)
	   *p = '\0';

	push_path(PREPEND, lpath, t);};

    if (st->trace > 3)
       {printf("CROSS = %s\n", cgetenv(TRUE, "CROSS"));
        printf("CFE   = %s\n", cgetenv(TRUE, "CFE"));};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT - setup the state */

static int init(rundes *st, char *os, char *host)
   {int rv;
	
    rv = TRUE;

    cunsetenv("MPI");
    cunsetenv("DBG");

    nstrncpy(st->os,   MAXLINE, os, -1);
    nstrncpy(st->host, MAXLINE, host, -1);

    csetenv("Host", st->host);

/* determine use of mpi-io-wrap */
    if (cdefenv("Wrap") == FALSE)
       {if (file_executable("%s/mpi-io-wrap", st->bindir) == TRUE)
	   csetenv("Wrap", "%s/mpi-io-wrap", st->bindir);
        else
	   csetenv("Wrap", cwhich("mpi-io-wrap"));

        if (file_executable(cgetenv(TRUE, "Wrap")) == FALSE)
	   csetenv("Wrap", "");};

    st->debug       = FALSE;
    st->dryrun      = FALSE;
    st->usempi      = TRUE;
    st->errio       = TRUE;
    st->trace       = 0;
    st->verbose     = 0;
    st->dargs[0]    = '\0';
    st->bargs[0]    = '\0';
    st->sgn[0]      = '\0';
    st->dbgtgt[0]   = '\0';
    st->mpitgt[0]   = '\0';
    st->crosstgt[0] = '\0';

/* variables for use in run signature database */
    csetenv("Batch",   "FALSE");
    csetenv("Code",    "");
    csetenv("CArgs",   "");
    csetenv("Force",   "FALSE");
    csetenv("MPICnd",  "FALSE");
    csetenv("MPINot",  "FALSE");
    csetenv("NNode",   "1");
    csetenv("NProc",   "1");
    csetenv("NThread", "1");
    csetenv("STDERR",  "TRUE");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SHELL_CLEAN - handle crappy vendor shell syntax problems
 *             - escape '(', ')', and '"' so that things like
 *             -    do-run scheme \(load \"foo\" \#t\)
 *             -    do-run basis "read foo ; read bar"
 *             - will work as needed
 *             - needed on AIX only
 */

static char *shell_clean(char *s)
   {char *p;

    p = subst(s, "(", "\\(", -1);
    p = subst(p, ")", "\\)", -1);
    p = subst(p, "\"", "\\\"", -1);
    p = subst(p, "#", "\\#", -1);
    p = subst(p, ";", "\\;", -1);
    p = subst(p, "\\\\", "\\", -1);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHECK - verify and correct the state after reading the command line args */

static int check(rundes *st)
   {int rv;
    char *c, *exe;

/* NOTE: I expected ncpu to be an environment variable
 * but change it and the behavior is all wrong - see test suite
 */
    st->ncpu = atoi(cgetenv(FALSE, "NProc"));

    c   = cgetenv(TRUE, "Code");
    exe = cwhich(c);

/* default command - most likely to be changed while parsing the signature file */
    csetenv("Cmd", "%s %s", exe, cgetenv(TRUE, "CArgs"));

    csetenv("Code", exe);
    c = cgetenv(TRUE, "Code");
    if ((file_executable(c) == FALSE) && (st->dryrun == FALSE))
       {printf("No '%s' on your path\n", c);
	rv = FALSE;};

    if (st->sgn[0] == '\0')
       snprintf(st->sgn, MAXLINE, "%s/include/do-run-db",
                path_head(st->bindir));

    if (file_exists(st->sgn) == FALSE)
       {if (cdefenv("RUN_SIGNATURE_DB") == TRUE)
           nstrncpy(st->sgn, MAXLINE, cgetenv(TRUE, "RUN_SIGNATURE_DB"), -1);
        else
	   nstrncpy(st->sgn, MAXLINE, "run.sgn", -1);};

    if (file_exists(st->sgn) == TRUE)
       {if (st->verbose > 0)
	   printf("Using signature file '%s'\n", st->sgn);}
    else
       {if (st->verbose > 0)
	   printf("File '%s' not found - serial execution only\n", st->sgn);
	st->sgn[0] = '\0';};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_COLON - parse the ':' syntax element */

static void parse_colon(rundes *st, char *sect, char *var, char *val)
   {char vl[MAXLINE];
    char *p;

    nstrncpy(vl, MAXLINE, val, -1);
    p = strpbrk(vl, " \t");
    if (p != NULL)
       *p = '\0';

    if ((strcmp(var, "CROSS") == 0) && (cmpenv("CROSS", vl) == 0))
       {nstrncpy(sect, MAXLINE, var, -1);
	if (st->trace > 1)
	   printf("Entering %s section %s\n", var, vl);}

    else if ((strcmp(var, "MPI") == 0) && (cmpenv("MPI", vl) == 0))
       {nstrncpy(sect, MAXLINE, var, -1);
	if (st->trace > 1)
	   printf("Entering %s section %s\n", var, vl);}

    else if ((strcmp(var, "DBG") == 0) && (cmpenv("DBG", vl) == 0) &&
	     (st->debug == TRUE))
       {nstrncpy(sect, MAXLINE, var, -1);
	if (st->trace > 1)
	   printf("Entering %s section %s\n", var, vl);}

    else
       {nstrncpy(sect, MAXLINE, "skip", -1);
	if (st->trace > 1)
	   printf("Skipping %s section %s\n", var, vl);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EVAL_EXPR - evaluate EXPR using the test utility
 *           - return TRUE if test returns 0
 *           - return FALSE otherwise
 */

static int eval_expr(rundes *st, char *expr)
   {int rv;
    char s[MAXLINE];

    if (expr == NULL)
       rv = -1;

    else
       {if ((expr[0] == '(') && (LAST_CHAR(expr) == ')'))
	   {nstrncpy(s, MAXLINE, expr+1, -1);
	    LAST_CHAR(s) = '\0';}
	else
	   nstrncpy(s, MAXLINE, expr, -1);

	nstrncpy(s, MAXLINE, subst(expr, "(", "\\(", -1), -1);
	nstrncpy(s, MAXLINE, subst(s, ")", "\\)", -1), -1);

	rv = execute(st->errio, "test %s > /dev/null 2>&1", s);

	rv = (rv == 0);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EVAL_COND - return either CONS or ALT depending on OK
 *           - if OK == -1 then evaluate EXPR and set OK
 *           - otherwise use the value of OK specified
 */

static char *eval_cond(rundes *st, int ok, char *expr, char *cons, char *alt)
   {static char val[MAXLINE];

/* with a conditional expression */
    if (expr[0] != '\0')
       {if (ok == -1)
	   {ok = eval_expr(st, expr);
	    if (st->trace > 2)
	       printf("> if %s {%s} else {%s}\n", expr, cons, alt);}

/* true result */
	if (ok == TRUE)
	   {nstrncpy(val, MAXLINE, cons, -1);
	    if (st->trace > 2)
	       printf(">    Consequent: %s\n", val);}

/* false result */
	else if (ok == FALSE)
	   {nstrncpy(val, MAXLINE, alt, -1);
	    if (st->trace > 2)
	       printf(">    Alternate: %s\n", val);}

/* undefined expresssion */
	else
	   val[0] = '\0';}

/* without a conditional expression */
    else
       nstrncpy(val, MAXLINE, cons, -1);

    trim(val, FRONT, " \n");
    trim(val, BACK, " \n");

    return(val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ASSGN_ACTION - carry out the specified action
 *              - (see tests/run.s3 for possibilities)
 */

static void assgn_action(rundes *st, int asgn, char *var,
			 char *effvar, char *lval)
   {char *v, *vl;

    if (strcmp(lval, "-die-") == 0)
       {printf("Error: unexpected test result for %s\n", effvar);
	if (st->trace > 1)
	   printf("Exiting if trace level < 2\n");
	else
	   terminate(1);}

    else if (strcmp(lval, "-noop-") == 0)
       return;

    else if (strcmp(lval, "-unsetenv-") == 0)
       {if (st->trace > 0)
	   printf("> unsetenv %s\n", effvar);

	cunsetenv(effvar);}

    else if (strcmp(lval, "-warn-") == 0)
       printf("Warning: unexpected test result for %s\n", effvar);

    else

/* do lazy evaluation for Cmd only */
       {if (strcmp(var, "Cmd") == 0)
	   {if (asgn == TRUE)
	       csetenv(effvar, lval);}

/* sane C shells do this (tcsh defines sane) */
	else
	   {if (asgn == TRUE)
	       {vl = eval(lval, MAXLINE, effvar);
		csetenv(effvar, vl);

/* add to path so that xxx_Exe will be on the ultimate execution path */
                if (strcmp(effvar + strlen(effvar) - 4, "_Exe") == 0)
                   push_path(PREPEND, lpath, path_head(cwhich(vl)));};};

	if (st->trace > 0)
	   {if (asgn == TRUE)
	       {v = cgetenv(FALSE, effvar);
	        printf("> setenv %s %s\n", effvar, v);}
	    else
	       {v = cgetenv(TRUE, effvar);
		printf("> %s already has value '%s'\n",
		       effvar, v);};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_ASSGN - parse an assignment */

static void parse_assgn(rundes *st, char *sect,
			char *var, char *oper, char *val,
			char *effvar)
   {int enverr, synerr, asgn, ok, kind;
    char expr[MAXLINE], cons[MAXLINE], alt[MAXLINE];
    char clause[MAXLINE], unev[MAXLINE], vex[MAXLINE];
    char *lval, *ldef, *valv, *rv;

    expr[0] = '\0';
    cons[0] = '\0';
    alt[0]  = '\0';

    if (val == NULL)
       val = "";

/* parse 'expr ? cons : alt' statements
 * require ' ? ' and ' : ' so that other shell syntax, e.g. $var:h will still work
 * this also permit calling '<expr> : <val>' a syntax error
 * gather tokens for a boolean expression, expr, a consequent clause, cons,
 * and an alternate clause, alt
 * convert C syntax to 'test' syntax
 * NOTE: we require a fully expanded version of clause and an unevaluated version, unev
 */
    clause[0] = '\0';
    unev[0]   = '\0';
    enverr = FALSE;
    synerr = FALSE;
    ok     = -1;
    kind   = 0;

    nstrncpy(vex, MAXLINE, subst(val, "(", " ( ", -1), -1);
    nstrncpy(vex, MAXLINE, subst(vex, ")", " ) ", -1), -1);
    FOREACH(tok, vex, " \t")

/* handle '?' token */
       if (tok[0] == '?')
	  {nstrncpy(expr, MAXLINE, subst(clause, ">=", "-ge", -1), -1);
	   nstrncpy(expr, MAXLINE, subst(expr, ">", "-gt", -1), -1);
	   nstrncpy(expr, MAXLINE, subst(expr, "<=", "-le", -1), -1);
	   nstrncpy(expr, MAXLINE, subst(expr, "<", "-lt", -1), -1);
	   nstrncpy(expr, MAXLINE, subst(expr, "!=", "-ne", -1), -1);
	   nstrncpy(expr, MAXLINE, subst(expr, "==", "-eq", -1), -1);

	   ok = eval_expr(st, eval(expr, MAXLINE, NULL));

           clause[0] = '\0';
           unev[0]   = '\0';

	   if (ok == -1)
	      enverr = TRUE;

/* if expr is TRUE we must check the consequent clause in detail */
           kind = (ok != FALSE);}

/* handle ':' token */
       else if (tok[0] == ':')
	  {if (expr[0] == '\0')
	      {synerr = TRUE;
	       break;}
	   else if (clause[0] == '\0')
	      {synerr = TRUE;
               break;}
	   else
	      {nstrncpy(cons, MAXLINE, clause, -1);
	       clause[0] = '\0';
               unev[0]   = '\0';};

/* if expr is FALSE we must check the alternate clause in detail */
           kind = (ok != TRUE);}

/* evaluate expressions with environment variable references - $foo */
       else if ((strchr(tok, '$') != NULL) && (kind == TRUE))
	  {if (strstr(tok, "$@") != NULL)
	      {valv = cgetenv(TRUE, effvar);
               push_tok(clause, MAXLINE, ' ', valv);
               push_tok(unev, MAXLINE, ' ', valv);}
	   else
	      {rv = eval(tok, MAXLINE, NULL);
               if (rv != NULL)
		  {push_tok(clause, MAXLINE, ' ', rv);
                   push_tok(unev, MAXLINE, ' ', tok);}
	       else
		  {enverr = TRUE;
                   break;};};}

/* convert C logical operators into a form suitable for /bin/test */
       else if (strcmp(tok, "||") == 0)
	  {push_tok(clause, MAXLINE, ' ', "-o");
           push_tok(unev, MAXLINE, ' ', "-o");}
       else if (strcmp(tok, "&&") == 0)
	  {push_tok(clause, MAXLINE, ' ', "-a");
           push_tok(unev, MAXLINE, ' ', "-a");}
       else if (strcmp(tok, "==") == 0)
	  {push_tok(clause, MAXLINE, ' ', "=");
           push_tok(unev, MAXLINE, ' ', "=");}

/* accumlate everything else into clause and unev as is */
       else
	  {if (clause[0] == '\0')
	      {nstrncpy(clause, MAXLINE, tok, -1);
	       nstrncpy(unev, MAXLINE, tok, -1);}
	   else
	      {push_tok(clause, MAXLINE, ' ', tok);
               push_tok(unev, MAXLINE, ' ', tok);};};
    ENDFOR;

    if (enverr == TRUE)
       {printf("*** error: undefined variable for %s: \"%s\"\n", var, val);
	return;};

    if (synerr == TRUE)
       {printf("*** error: bad syntax for %s: \"%s\"\n", var, val);
	return;};

/* use any left over tokens to fill an empty consequent */
    if ((unev[0] != '\0') && (cons[0] == '\0'))
       {nstrncpy(cons, MAXLINE, unev, -1);
	clause[0] = '\0';
	unev[0]   = '\0';};

/* use any left over tokens to fill an empty alternate */
    if ((unev[0] != '\0') && (alt[0] == '\0'))
       {nstrncpy(alt, MAXLINE, unev, -1);
	clause[0] = '\0';
	unev[0]   = '\0';};

/* evaluate any conditional to determine the action desired */
    if ((ok != -1) && (st->trace > 2))
       printf("> if %s {%s} else {%s}\n", expr, cons, alt);

    lval = eval_cond(st, ok, expr, cons, alt);

    asgn = TRUE;
    if (strcmp(oper, "?=") == 0)
       {ldef = cgetenv(TRUE, effvar);
	if (ldef[0] != '\0')
	   asgn = FALSE;};

/* carry out the specified action (see tests/run.s3 for possibilities) */
    assgn_action(st, asgn, var, effvar, lval);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SET_TARGET - set the DBG, MPI, and CROSS targets */

static int set_target(rundes *st)
   {int i, rv;
    char s[MAXLINE];
    char *p, *var;
    char *cat, *lst;
    FILE *fp;

    rv = TRUE;

/* check the config db for default target specs
 * NOTE: the only way the targets are non-empty is if
 *       they were specified on the command line
 *       and that must win
 */
    if (st->sgn[0] != '\0')
       {fp = fopen(st->sgn, "r");

	for (i = 0; TRUE; i++)
	    {p = fgets(s, MAXLINE, fp);
	     if (p == NULL)
	        break;

	     LAST_CHAR(s) = '\0';

	     if ((strlen(s) == 0) || (blank_line(s) == TRUE))
	        continue;

	     var = strtok(s, " \t");
	     if (strcmp(var, "default") == 0)
	        {cat = strtok(NULL, " \t");
		 lst = strtok(NULL, "\n");

		 if ((strcmp(cat, "DBG") == 0) && (st->dbgtgt[0] == '\0'))
		    load_target(st->dbgtgt, MAXLINE, lst);

		 else if ((strcmp(cat, "MPI") == 0) && (st->mpitgt[0] == '\0'))
		    load_target(st->mpitgt, MAXLINE, lst);

		 else if ((strcmp(cat, "CROSS") == 0) && (st->crosstgt[0] == '\0'))
		    load_target(st->crosstgt, MAXLINE, lst);};};

	fclose(fp);};

/* check the environment default target specs */
    init_mpi_target(st);
    init_dbg_target(st);
    init_cross_target(st);

/* set the corresponding environment variables */
    init_dbg(st);
    init_cross(st);
    init_mpi(st);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_DB - parse the RUN_SIGNATURE database */

static void parse_db(rundes *st)
   {int i;
    char s[MAXLINE], effvar[MAXLINE], sect[MAXLINE];
    char *p, *var, *oper, *val, *exe;
    FILE *fp;

    if (st->sgn[0] != '\0')
       {csetenv("DBG_Exe",     "");
	csetenv("DBG_Flags",   "");
	csetenv("DBG_Delim",   "");
	csetenv("MPI_Exe",     "");
	csetenv("MPI_Flags",   "");
	csetenv("CROSS_Exe",   "");
	csetenv("CROSS_Flags", "");

	if (st->trace > 2)
	   {printf("CROSS = %s\n", cgetenv(TRUE, "CROSS"));
	    printf("MPI   = %s\n", cgetenv(TRUE, "MPI"));
	    printf("DBG   = %s\n", cgetenv(TRUE, "DBG"));};

	fp = fopen(st->sgn, "r");
	sect[0] = '\0';

	for (i = 0; TRUE; i++)
	    {p = fgets(s, MAXLINE, fp);
	     if (p == NULL)
	        break;

	     LAST_CHAR(s) = '\0';

	     if ((strlen(s) == 0) || (blank_line(s) == TRUE))
	        continue;

	     if (st->trace > 4)
	        printf("%d> %s\n", i, s);

	     var = strtok(s, " \t");

/* check for end of section */
	     if (var[0] == '}')
	        {if ((st->trace > 1) && (strcmp(sect, "skip") != 0))
		    printf("Leaving section %s\n", sect);

		 sect[0] = '\0';
		 continue;};

/* skip 'default' specifications */
	     if (strcmp(var, "default") == 0)
	        continue;

/* skip lines in non-matching sections */
	     if (strcmp(sect, "skip") == 0)
	        continue;

/* set the effective variable name */
	     if (strcmp(var, "Env") == 0)
	        {var    = strtok(NULL, " \t");
		 nstrncpy(effvar, MAXLINE, var, -1);}
	     else if ((strcmp(sect, "") == 0) || (strcmp(var, "Cmd") == 0))
	        nstrncpy(effvar, MAXLINE, var, -1);
	     else
	        snprintf(effvar, MAXLINE, "%s_%s", sect, var);

	     oper = strtok(NULL, " \t");
	     val  = strtok(NULL, "\n");

	     if (*oper == ':')
	        parse_colon(st, sect, var, val);
	     else if (oper[strlen(oper)-1] == '=')
	        parse_assgn(st, sect, var, oper, val, effvar);
	     else
	        printf("Unknown operator %s - ignored\n", oper);};};

/* use full path to MPI front end */
    exe = cwhich(cgetenv(TRUE, "MPI_Exe"));
    if (file_executable(exe) == TRUE)
       csetenv("MPI_Exe", exe);

/* use full path to debugger */
    exe = cwhich(cgetenv(TRUE, "DBG_Exe"));
    if (file_executable(exe) == TRUE)
       csetenv("DBG_Exe", exe);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_DEBUG_PATH - work out command options for the debugger
 *                  - especially the source path
 */

static int setup_dbg_path(rundes *st)
   {int rv;
    char s[MAXLINE], ent[MAXLINE];
    char *c, *fn, *lst;
    FILE *fp;

    c = cgetenv(TRUE, "DBG_Exe");
    if ((file_executable(cwhich(c)) == FALSE) && (st->dryrun == FALSE))
       {printf("No %s on your path\n", c);
	rv = FALSE;}

    else
       {rv = TRUE;

	if (cdefenv("TVDirs") == TRUE)
	   {fn = cgetenv(TRUE, "TVDirs");
	    fp = open_file("a", fn);}
	else
	   {getcwd(s, MAXLINE);
	    csetenv("TVDirs", "%s/.tv.%d", s, getpid());
	    fn = cgetenv(TRUE, "TVDirs");
	    fp = open_file("w", fn);};

/* add the PACT directories to TVDirs */
        if (fp != NULL)
	   {nstrncpy(s, MAXLINE, st->bindir, -1);
	    nstrncpy(s, MAXLINE, path_head(s), -1);
	    nstrncpy(s, MAXLINE, path_head(s), -1);
	    if (dir_exists("%s/sources", s) == TRUE)
	       lst = run(FALSE, "ls %s/sources/ 2> /dev/null", s);

	    else
	       {nstrncpy(s, MAXLINE, path_head(s), -1);
		if (dir_exists(s) == TRUE)
		   lst = run(FALSE, "ls %s 2> /dev/null", s);
		else
		   lst = NULL;};

	    FOREACH(i, lst, " :\n");
	       nstrncpy(ent, MAXLINE, path_tail(i), -1);
	       if ((dir_exists("%s/%s", s, ent) == TRUE) &&
	           (strcmp(ent, "CVS") != 0) &&
	           (strncmp(ent, "z-", 2) != 0) &&
		   (strcmp(ent, "dev") != 0))
		  fprintf(fp, "%s/%s\n", s, ent);
	    ENDFOR;

	    fclose(fp);

	    if (st->verbose > 0)
	       printf("Running under %s\n", cgetenv(TRUE, "DBG_Exe"));

	    push_tok(st->dargs, MAXLINE, ' ', "-l");};};

   return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_DBG - work out command options to run the program under
 *           - a type debugger
 *           - return TRUE iff successful
 */

static int setup_dbg(rundes *st)
   {int rv;
    char *mpife;

/* setup to run the debugger */
    rv = setup_dbg_path(st);

/* fix up the command line if there is no MPI front end */
    if (rv == TRUE) 
       {mpife = cgetenv(FALSE, "MPI_Exe");
	if ((mpife == NULL) || (mpife[0] == '\0'))
	   csetenv("Cmd", "$Wrap $DBG_Exe $DBG_Flags $Code $DBG_Delim $CArgs");};

   return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_ENV - remove do-run environment variables and
 *             - return the final, ultimate command line
 */

static char *cleanup_env(rundes *st)
   {char *cmd;

    cmd = subst(cgetenv(TRUE, "Cmd"), "\"\"", "", -1);
    cmd = shell_clean(cmd);

    snprintf(st->mpife, MAXLINE, "%s", cgetenv(TRUE, "MPI"));

    st->errio = (cmpenv("STDERR", "TRUE") == 0);

    cunsetenv("Batch");
    cunsetenv("CArgs");
    cunsetenv("Cmd");
    cunsetenv("Code");
    cunsetenv("DBG");
    cunsetenv("DBG_Delim");
    cunsetenv("DBG_Exe");
    cunsetenv("DBG_Flags");
    cunsetenv("Force");
    cunsetenv("Host");
    cunsetenv("MPI");
    cunsetenv("MPICnd");
    cunsetenv("MPIFE");
    cunsetenv("MPILib");
    cunsetenv("MPINot");
    cunsetenv("MPI_Exe");
    cunsetenv("MPI_Flags");
    cunsetenv("CROSS_Exe");
    cunsetenv("CROSS_Flags");
    cunsetenv("NCPU");
    cunsetenv("NNode");
    cunsetenv("NProc");
    cunsetenv("NThread");
    cunsetenv("Pool");
    cunsetenv("STDERR");

    return(cmd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RUN_BATCH - run whatever it was we decided to do in batch mode */

static int run_batch(rundes *st)
   {int rv;
    char *cmd;

    cmd = cleanup_env(st);

    if (st->verbose > 0)
       {printf("HaveMPI = |%d|\n", st->usempi);
        printf("Wrap    = |%s|\n", cgetenv(TRUE, "Wrap"));
        printf("do-batch %s -p %d %s\n",
               st->bargs, st->ncpu, cmd);};

    if (st->dryrun == TRUE)
       rv = 1;
    else
       rv = execute(st->errio, "do-batch %s -p %d %s",
                    st->bargs, st->ncpu, cmd);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RUN_INTERACTIVE - run whatever it was we decided to do
 *                 - in interactive mode
 */

static int run_interactive(rundes *st)
   {int rv;
    char *cmd;

    cmd = cleanup_env(st);

    if (st->verbose > 0)
       {if (strcmp(st->mpitgt, "poe") == 0)
           {printf("setenv MP_LABELIO  %s\n", cgetenv(TRUE, "MP_LABELIO"));
            printf("setenv MP_NODES    %s\n", cgetenv(TRUE, "MP_NODES"));
            printf("setenv MP_PROCS    %s\n", cgetenv(TRUE, "MP_PROCS"));
            if (cmpenv("MPINot", "TRUE") == 0)
               {printf("setenv MP_EUILIB   %s\n", cgetenv(TRUE, "MP_EUILIB"));
                printf("setenv MP_RESD     %s\n", cgetenv(TRUE, "MP_RESD"));
                printf("setenv MP_CPU_USE  %s\n", cgetenv(TRUE, "MP_CPU_USE"));}
            else
               printf("setenv MP_RMPOOL   %s\n", cgetenv(TRUE, "MP_RMPOOL"));

            printf("setenv MP_HOSTFILE %s\n", cgetenv(TRUE, "MP_HOSTFILE"));};

       printf("%s\n", cmd);};

    if (st->dryrun == TRUE)
       rv = 1;
    else
       rv = execute(st->errio, cmd);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FINISH - semi-arbitrarily declare that failure to run the
 *        - job by reason of lack of resources (usually CPUs)
 *        - causes an exit status value of -1
 *        - this is AIXs and OSFs value
 */

static int finish(rundes *st, int rv)
   {int rsn, sts;
    char s[MAXLINE];

/* this is the right place to remove the AIX MPI hostfile */
    if (strcmp(st->os, "AIX") == 0)
       {snprintf(s, MAXLINE, ".dp-aix.%s.%d", st->host, getpid());
	unlink(s);};

    rsn = rv >> 8;
    sts = rv & 0xff;

/* this corresponds to a -1 exit status in a shell script */
    if ((rsn == 0xff) && (rv == 0x100))
       rsn = -1;

    else if (((strcmp(st->mpife, "prun") == 0) && (rsn == 126)) ||
	     ((strcmp(st->mpife, "srun") == 0) && (rsn == 1)) ||
	     ((strcmp(st->mpife, "salloc") == 0) && (rsn == 1)))
       rsn = -1;
/*
    else if (rsn == 1)
       rsn = -1;
*/
   return(rsn);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INTERRUPT - cleanup properly after an interrupt */

static void interrupt(int sig)
   {

    terminate(1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - tell how to run do-run */

static void help(void)
   {

    printf("\n");
    printf("Usage: do-run [-b] [-bf <file>] [-c] [-cross <cross-fe>] [-d] [-dbg <dbg>] [-dr]\n");
    printf("              [-e] [-f] [-h] [-level #] [-m] [-mpi <mpi-fe>] [-m] [-n #] [-o <file>]\n");
    printf("              [-p #] [-prt <name>] [-q] [-s <file>] [-t #] [-v] [-vg] [-vgd] [-x]\n");
    printf("              [-z] args\n");
    printf("\n");
    printf("       b     - run in batch\n");
    printf("       bf    - file containing info for local batch system\n");
    printf("       c     - code conditionally invokes MPI\n");
    printf("       cross - override default cross compile front end\n");
    printf("       d     - run the code under the debugger\n");
    printf("       dbg   - override default debugger\n");
    printf("       dr    - show command but do not execute it\n");
    printf("       e     - output to stderr is suppressed\n");
    printf("       f     - force a distributed run even with 1 MPI process\n");
    printf("       h     - this help message\n");
    printf("       level - diagnostic trace of session\n");
    printf("       m     - suppress MPI I/O bug fix\n");
    printf("       mpi   - override default MPI front end\n");
    printf("       n     - number of nodes to use - default is 1 (MPI)\n");
    printf("       o     - output file for DP runs\n");
    printf("       p     - number of processes to use (MPI)\n");
    printf("       prt   - partition to be used (MPI)\n");
    printf("       q     - suppress do-batch messages in logs\n");
    printf("       s     - use signature file\n");
    printf("       t     - number of threads to use (SMP)\n");
    printf("       v     - verbose mode\n");
    printf("       vg    - run under Valgrind\n");
    printf("       vgd   - run with interactive GDB Valgrind\n");
    printf("       x     - suppress MPI frontend (sometimes useful in batch)\n");
    printf("       z     - run under ZeroFault on AIX only\n");
    printf("\n");

    terminate(1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROCESS_ARGS - process the command line arguments */

static int process_args(rundes *st, int c, char **v)
   {int i, nt;
    char s[MAXLINE];
    char *ky, *vl;

    if (c == 0)
       help();

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-b") == 0)
            csetenv("Batch", "TRUE");

	 else if (strcmp(v[i], "-bf") == 0)
            {push_tok(st->bargs, MAXLINE, ' ', "-f");
	     push_tok(st->bargs, MAXLINE, ' ', v[++i]);
	     csetenv("Batch", "TRUE");}

	 else if (strcmp(v[i], "-c") == 0)
	    {st->conditional = TRUE;
	     csetenv("MPICnd", "TRUE");}

	 else if (strcmp(v[i], "-cross") == 0)
	    nstrncpy(st->crosstgt, MAXLINE, v[++i], -1);

	 else if (strcmp(v[i], "-d") == 0)
            {st->debug  = TRUE;
	     nstrncpy(st->dbgtgt, MAXLINE, "tv", -1);}

	 else if (strcmp(v[i], "-dbg") == 0)
            {st->debug  = TRUE;
	     nstrncpy(st->dbgtgt, MAXLINE, v[++i], -1);}

	 else if (strcmp(v[i], "-dr") == 0)
            {st->verbose++;
	     st->dryrun = TRUE;}

	 else if (strcmp(v[i], "-e") == 0)
            {st->errio = FALSE;
	     csetenv("STDERR", "FALSE");}

	 else if (strcmp(v[i], "-f") == 0)
            csetenv("Force", "TRUE");

	 else if ((strcmp(v[i], "-h") == 0) ||
		  (strcmp(v[i], "help") == 0))
	    help();

	 else if (strcmp(v[i], "-level") == 0)
	    st->trace = atoi(v[++i]);

	 else if (strcmp(v[i], "-m") == 0)
	    csetenv("Wrap", "");

	 else if (strcmp(v[i], "-mpi") == 0)
	    nstrncpy(st->mpitgt, MAXLINE, v[++i], -1);

	 else if (strcmp(v[i], "-o") == 0)
	    {push_tok(st->dargs, MAXLINE, ' ', "-o");
	     push_tok(st->dargs, MAXLINE, ' ', v[++i]);}

	 else if (strcmp(v[i], "-n") == 0)
	    {csetenv("NNode", v[++i]);
	     push_tok(st->dargs, MAXLINE, ' ', "-n");
	     push_tok(st->dargs, MAXLINE, ' ', v[i]);}

	 else if (strcmp(v[i], "-p") == 0)
	    {nstrncpy(s, MAXLINE, v[++i], -1);
	     key_val(&ky, &vl, s, ",\n");
	     csetenv("NProc", ky);
	     if (vl != NULL)
		csetenv("NNode", vl);
	     push_tok(st->dargs, MAXLINE, ' ', "-p");
	     push_tok(st->dargs, MAXLINE, ' ', v[i]);}

	 else if (strcmp(v[i], "-prt") == 0)
	    csetenv("Pool", v[++i]);

	 else if (strcmp(v[i], "-q") == 0)
	    push_tok(st->dargs, MAXLINE, ' ', v[++i]);

	 else if (strcmp(v[i], "-s") == 0)
	    nstrncpy(st->sgn, MAXLINE, v[++i], -1);

	 else if (strcmp(v[i], "-t") == 0)
	    {nt = atoi(v[++i]);
	     csetenv("NThread", v[i]);
             if (nt > 0)
	        csetenv("OMP_NUM_THREADS", v[i]);}

	 else if (strcmp(v[i], "-v") == 0)
	    st->verbose++;

	 else if (strcmp(v[i], "-vg") == 0)
            {st->debug  = TRUE;
	     nstrncpy(st->dbgtgt, MAXLINE, "vg", -1);}

	 else if (strcmp(v[i], "-vgd") == 0)
            {st->debug  = TRUE;
	     nstrncpy(st->dbgtgt, MAXLINE, "vgd", -1);}

	 else if (strcmp(v[i], "-x") == 0)
            st->usempi = FALSE;

	 else if (strcmp(v[i], "-z") == 0)
            {st->debug  = TRUE;
	     nstrncpy(st->dbgtgt, MAXLINE, "zf", -1);}

	 else
            {char *p;

	     csetenv("Code", v[i++]);
             s[0] = '\0';
             for (; i < c; i++)
	         {p = v[i];

/* if P contains spaces it must have been quoted on the command line
 * so restore the quotes which the shell will have stripped off
 * this lets thing like:  "a ; b" pass through do-run and be
 * passed properly to the ultimate execute line
 */
		  if (strpbrk(p, " \t") != NULL)
		     push_tok(s, MAXLINE, ' ', "\"%s\"", v[i]);
		  else
		     push_tok(s, MAXLINE, ' ', v[i]);};

	     p = shell_clean(s);

	     csetenv("CArgs", p);};};

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it out here */

int main(int c, char **v)
   {int ok, rv;
    char exe[MAXLINE], os[MAXLINE], host[MAXLINE];
    rundes state;

    setbuf(stdout, NULL);

    signal(SIGINT, interrupt);

    memset(&state, 0, sizeof(rundes));

/* find the directory with do-run */
    nstrncpy(exe, MAXLINE, cwhich(v[0]), -1);
    nstrncpy(state.bindir, MAXLINE, path_head(exe), -1);

    unamef(os,   MAXLINE, "s");
    unamef(host, MAXLINE, "n");

/* locate the tools needed for subshells */
    if (strcmp(os, "AIX") == 0)
       build_path(NULL,
		  "swhich", "mpi-info", "dump", "nm", "file",
		  NULL);
    else
       build_path(NULL,
		  "swhich", "mpi-info", "nm", "ls", "file",
		  NULL);

    push_path(PREPEND, lpath, state.bindir);

    rv = init(&state, os, host);
    if (rv == TRUE)
       {rv = process_args(&state, c, v);
	rv = check(&state);
	set_target(&state);

	parse_db(&state);

	ok = TRUE;
	if (state.debug == TRUE)
	   {ok = setup_dbg(&state);
	    rv = 2;};

	if (ok == TRUE)
	   {csetenv("Cmd", eval(cgetenv(TRUE, "Cmd"), LRG, "Cmd"));

	    if (state.verbose > 1)
	       {printf("NCPU        = |%d|\n", state.ncpu);
		printf("Wrap        = |%s|\n", cgetenv(TRUE, "Wrap"));
		printf("DBG_Exe     = |%s|\n", cgetenv(TRUE, "DBG_Exe"));
		printf("DBG_Flags   = |%s|\n", cgetenv(TRUE, "DBG_Flags"));
		printf("CROSS_Exe   = |%s|\n", cgetenv(TRUE, "CROSS_Exe"));
		printf("MPI_Exe     = |%s|\n", cgetenv(TRUE, "MPI_Exe"));
		printf("DBG_Delim   = |%s|\n", cgetenv(TRUE, "DBG_Delim"));
		printf("CROSS_Flags = |%s|\n", cgetenv(TRUE, "CROSS_Flags"));
		printf("MPI_Flags   = |%s|\n", cgetenv(TRUE, "MPI_Flags"));
		printf("Code        = |%s|\n", cgetenv(TRUE, "Code"));
		printf("CArgs       = |%s|\n", cgetenv(TRUE, "CArgs"));
		printf("Command: %s\n", cgetenv(TRUE, "Cmd"));};

	    if (cmpenv("Batch", "TRUE") == 0)
	       rv = run_batch(&state);
	    else
	       rv = run_interactive(&state);

	    rv = finish(&state, rv);};};

    terminate(rv);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

