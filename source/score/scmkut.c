/*
 * SCMKUT.C - a full make utility with
 *          - persistence at the individual command level
 *          - full up version of pact utility
 *          - smake in psh is there to bootstrap up to this one
 *          - and be a backup in case this one will not build
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_make.h"

/* get report_info from psh routines which supply
 * smake, dmake, and pact-info
 */
#define NO_STD_INCLUDES
#include <../psh/common.h>
#include <../psh/libpsh.c>
#include <../psh/libinfo.c>

static int
 debug = FALSE,
 phase = 0;

static char
 **argv = NULL;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMMAND_MAKEFILE - construct a database to run
 *                  - a command specified in A[1-N]
 *                  - in each directory specified in the file A[0]
 *                  - return 0 iff successful
 */

static int command_makefile(anadep *state, char **a)
   {int i, err;
    char s[MAXLINE];
    char *fname, *tok, *dir, *p, *t, *rule, *cmnd;
    FILE *fp;

    err = 0;

    cmnd = NULL;

/* make up the command string */
    for (i = 1; TRUE; i++)
        {tok = a[i];
	 if (tok == NULL)
	    break;
	 cmnd = SC_dstrcat(cmnd, tok);
	 cmnd = SC_dstrcat(cmnd, " ");};

    if (a[0][0] == '-')
       {snprintf(s, MAXLINE, "%s/etc/package", state->root);
	fname = s;}
    else
       fname = a[0];

    fp = io_open(fname, "r");
    if (fp != NULL)
       {rule = SC_dsnprintf(TRUE, "go :\n");

	while (io_gets(s, MAXLINE, fp) != NULL)
	   {dir = SC_strtok(s, "\n", p);
	    t   = SC_dsnprintf(FALSE,
			       "@(cd %s ; echo \"\" ; echo \"In %s doing %s\" ; %s)\n",
			       dir, dir, cmnd, cmnd);

	    rule = SC_dstrcat(rule, t);};

	io_close(fp);

	SC_make_def_rule(state, rule);

	CFREE(rule);

	err = TRUE;};

    CFREE(cmnd);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_ENV - initialize the ARCH and ROOT strings
 *           - return TRUE iff successful with both
 */

static int setup_env(char *src, anadep *state)
   {int st, nc, ok;
    char *p, *t, *s;

/* setup the exe path string */
    st = SC_full_path(src, state->exe, PATH_MAX);
    if (st != 0)
       {io_printf(stdout, "Cannot locate %s - exiting\n", src);
	return(FALSE);};

/* setup the root directory string */
    SC_strncpy(state->root, PATH_MAX, state->exe, -1);

/* remove the executable name */
    p = SC_pop_path(state->root);
    if (p == state->root)
       {io_printf(stdout, "Cannot understand path %s - exiting\n", src);
	return(FALSE);};

/* remove the bin directory */
    p = SC_pop_path(state->root);
    if (strcmp(p, "bin") != 0)
       io_printf(stdout, "Cannot find configuration info from %s\n", state->root);

/* check for environment override */
    p = getenv("PACT_ROOT");
    if (p != NULL)
       SC_strncpy(state->root, PATH_MAX, p, PATH_MAX);

/* setup the architecture string */
    p = getenv("SESSION_CONFIG");
    if (p != NULL)
       snprintf(state->arch, MAXLINE, "z-%s", p);
    else
       {s  = SC_dstrcpy(NULL, state->root);
	ok = FALSE;
	p  = strrchr(s, '/');
	if (p != NULL)
	   {t  = p+1;
	    *p = '\0';
	    nc = strlen(s);
	    nc = max(nc, 3);
	    p  = s + nc - 3;
	    if (strcmp(p, "dev") == 0)
	       {snprintf(state->arch, MAXLINE, "z-%s", t);
		ok = TRUE;};};
	       
	CFREE(s);

	if (!ok)
	   snprintf(state->arch, MAXLINE, "z-%s", SYSTEM_ID);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_BY_HOST - do the command specified by V
 *              - on each host in LST
 *              - return TRUE iff successful
 */

static int make_by_host(char *lst, char **v, char *shell, char **env,
			char *flt, int na, int show, int ignore)
   {int i, ok;
    char cm[MAXLINE];
    char **sys, *cmnds[2];

    ok = TRUE;

    SC_strncpy(cm, MAXLINE, "pact", -1);
    for (i = 0; v[i] != NULL; i++)
        SC_vstrcat(cm, MAXLINE, " %s", v[i]);
    cmnds[0] = cm;
    cmnds[1] = NULL;

    if (lst[0] == '-')
       sys = SC_get_host_types(0, NULL);

    else
       sys = SC_tokenize(lst, ",");

    ok = SC_exec_async_s(shell, env, sys, cmnds, NULL,
			 -1, flt, na, show, ignore);

    SC_free_strings(sys);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_DISTRIBUTED - do specified target TGT
 *                  - in directories specified by FILE
 *                  - return TRUE iff successful
 */

static int make_distributed(char *tgt, char *file, char *server,
			    char *shell, char **env,
			    char *flt, int na, int show, int ignore)
   {int ok;
    char s[MAXLINE], cm[MAXLINE];
    char *p, **dirs, *cmnds[2];
    FILE *fp;
    SC_array *arr;

    ok = TRUE;

    snprintf(cm, MAXLINE, "pact %s", tgt);
    cmnds[0] = cm;
    cmnds[1] = NULL;

    fp = io_open(file, "r");
    if (fp != NULL)
       {arr = SC_STRING_ARRAY();

	while (TRUE)
	   {p = io_gets(s, MAXLINE, fp);
	    if (p == NULL)
	       {io_close(fp);
		break;}

	    else if (!SC_blankp(s, "#"))
	       {p = strtok(s, " \n");
		SC_array_string_add_copy(arr, p);};};

	SC_array_string_add(arr, NULL);

	dirs = SC_array_done(arr);

	ok   = SC_exec_async(shell, cmnds, dirs, server, env, 8,
			     NULL, flt, na, show, ignore, FALSE);

	SC_free_strings(dirs);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DMAKE_SIG_HANDLER - handle signals gracefully */

static void dmake_sig_handler(int sig)
   {int i, rv;
    char s[MAXLINE];

    SC_setup_sig_handlers(dmake_sig_handler, NULL, FALSE);

    SC_signal(SIGCHLD, SIG_IGN);
    SC_signal(SC_SIGIO, SIG_IGN);

    switch (phase)
       {case 0 :
             strcpy(s, "initializing");
	     break;
        case 1 :
             strcpy(s, "parsing input");
	     break;
        case 2 :
             strcpy(s, "analyzing dependencies");
	     break;
        case 3 :
             strcpy(s, "executing commands");
	     break;
        case 4 :
             strcpy(s, "cleaning up");
	     break;};

    io_printf(stdout, "PACT: signal %s (%d) to PID %d while %s for\n",
	      SC_signal_name(sig), sig, getpid(), s);
    io_printf(stdout, "      ");
    for (i = 0; TRUE; i++)
        {if (argv[i] == NULL)
	    break;
	 io_printf(stdout, "%s ", argv[i]);};
    io_printf(stdout, "\n");

/* attach the debugger to this rascal if requested */
    if ((debug == TRUE) &&
	((sig != SIGINT) && (sig != SIGTERM)))
       SC_attach_dbg(-1);

/* do a call stack trace if it has died */
    else if ((sig != SIGINT) && (sig != SIGTERM))
       SC_retrace_exe(NULL, -1, 120000);

    SC_block_file(stdin);

    rv = SC_signal_async(sig);
    if (rv == TRUE)
       exit(sig);

    SC_setup_sig_handlers(dmake_sig_handler, NULL, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* USAGE - print help */

void usage(void)
   {

    printf("\n");
    printf("Usage: pact [-async <n>] [-B] [-cmd <file> <cmd>] [-d] [-dbg] [-dmp] [-f <file>]\n");
    printf("            [-flt <file>] [-h] [-i] [-incpath] [-info <pttrn> | <var>] [+info <var>]\n");
    printf("            [-link] [-log <file>] [+l] [-n] [-na <n>] [+na <n>] [-pna <n>] [-rcr]\n");
    printf("            [-s] [-sys <dir>] [-v] <make-args> <target>\n");
    printf("\n");

    printf("Make Options:\n");
    printf("    -async   issue commands asynchronously to <n> nodes\n");
    printf("    -B       build Makefile from pre-Make\n");
    printf("    -cmd     run command <cmd> in directories specified in <file>\n");
    printf("             this must come last in the argument list\n");
    printf("    -d       show all rules and variables\n");
    printf("    -dbg     attach a debugger when a trapped signal is receive\n");
    printf("    -dmp     dump the logs at the end of the session\n");
    printf("    -f       use <file> for all targets, rules, and variables\n");
    printf("    -flt     use <file> for specifications to filter out unwanted output\n");
    printf("    -h       this help message\n");
    printf("    -i       continue on after errors\n");
    printf("    -log     write output to <file> in addition to stdout\n");
    printf("    -n       only print commands, do not execute them\n");
    printf("    -na      attempt each command at most <n> times before failing\n");
    printf("    +na      like -na but applies to all child pact sessions too\n");
    printf("    -pna     like -na but applies only to parent pact session\n");
    printf("    -rcr     expand simple recursive invocations in place\n");
    printf("    -s       do not print commands before executing them\n");
    printf("    -sys     directory in which to put/find Makefile\n");
    printf("    -vrb     print all commands before executing them (overrides '@')\n");

    printf("\n");

    printf("Info Options:\n");
    printf("    -incpath report elements needed to compile files\n");
    printf("    -info    report configuration elements matching the argument\n");
    printf("    +info    complete report of configuration variable matching the argument\n");
    printf("    -link    report elements needed to link applications\n");
    printf("    +l       report only exact matches of the argument\n");
    printf("    -v       report the PACT version\n");

    printf("\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v, char **env)
   {int i, dryrun, ignore, show, st, na, no, nt, rv;
    int async, nconn, recur, dmp, mem, cs;
    int rnfd, rnprc;
    int64_t rmem, rcpu, rfsz;
    char mkfile[MAXLINE], s[MAXLINE], server[MAXLINE], srvna[MAXLINE];
    char msg[MAXLINE];
    char **cmnds, *mkname, *log, *flt, *tgt, *shell, *dpdir, *dpsys;
    char *err, *p;
    anadep *state;

    SC_single_thread();

    SC_init("PACT: Exit with error", NULL,
            TRUE, NULL, NULL,
            TRUE, NULL, 0);

    argv = v;

    SC_get_resource_limits(&rmem, &rcpu, &rfsz, &rnfd, &rnprc);
    SC_set_resource_limits(-1, -1, -1, -1, -1);

    SC_setup_sig_handlers(dmake_sig_handler, NULL, TRUE);

    state = SC_make_state();

    st = setup_env(v[0], state);
    if (st == 0)
       return(1);

    SC_setbuf(stdout, NULL);

#ifdef MAKE_FILTER_FILE
    flt = MAKE_FILTER_FILE;
#else
    flt = NULL;
#endif

    shell = SC_get_shell(NULL);

    p = getenv("SC_EXEC_N_ATTEMPTS");
    if (p != NULL)
       {na = SC_stoi(p);
        na = max(na, 1);}
    else
       na = 3;

    cs     = 0;
    async  = FALSE;
    debug  = FALSE;
    dryrun = FALSE;
    ignore = FALSE;
    mem    = FALSE;
    nconn  = -1;
    recur  = FALSE;
    dmp    = FALSE;
    show   = 1;

    dpdir  = NULL;
    dpsys  = NULL;
    log    = NULL;
    mkname = NULL;
    tgt    = NULL;

    srvna[0] = '\0';

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-async") == 0)
	    {async = TRUE;
	     nconn = SC_stoi(v[++i]);
             snprintf(srvna, MAXLINE, "-na %d", na);
             na = 1;}
	 else if (strcmp(v[i], "-B") == 0)
	    {snprintf(mkfile, MAXLINE, "%s/Makefile", state->arch);
	     st = build_makefile(state->root, state->arch, mkfile, TRUE);
	     return(st);}
	 else if (strcmp(v[i], "-cmd") == 0)
	    {mkname = ".command";
	     tgt    = "go";
	     break;}
	 else if (strcmp(v[i], "-d") == 0)
	    {state->show_rules = TRUE;
	     state->show_vars  = TRUE;}
	 else if (strcmp(v[i], "-dbg") == 0)
	    debug = TRUE;
	 else if (strcmp(v[i], "-dmp") == 0)
	    dmp = TRUE;
	 else if (strcmp(v[i], "-dst") == 0)
	    dpdir = v[++i];
         else if (strcmp(v[i], "-f") == 0)
	    mkname = v[++i];
         else if (strcmp(v[i], "-flt") == 0)
	    flt = v[++i];
         else if (strcmp(v[i], "-h") == 0)
	    {usage();
	     return(1);}
         else if (strcmp(v[i], "-i") == 0)
	    ignore = TRUE;
	 else if (strcmp(v[i], "-incpath") == 0)
	    {report_info(state->root, state->complete, state->literal,
			 INC, NULL);
	     return(0);}
	 else if (strcmp(v[i], "-info") == 0)
	    {if (++i < c)
	        report_info(state->root, state->complete, state->literal,
			    REGEX, v[i]);
	     return(0);}
	 else if (strcmp(v[i], "+info") == 0)
	    {if (++i < c)
	        {state->complete = TRUE;
		 report_info(state->root, state->complete, state->literal,
			     REGEX, v[i]);};
	     return(0);}
         else if (strcmp(v[i], "+l") == 0)
	    state->literal = TRUE;
	 else if (strcmp(v[i], "-link") == 0)
	    {report_info(state->root, state->complete, state->literal,
			 LINK, NULL);
	     return(0);}
         else if (strcmp(v[i], "-log") == 0)
	    log = v[++i];
         else if (strcmp(v[i], "-mem") == 0)
	    mem = TRUE;
         else if (strcmp(v[i], "-n") == 0)
	    dryrun = TRUE;
         else if (strcmp(v[i], "-na") == 0)
	    {na = SC_stoi(v[++i]);
	     na = max(na, 1);
	     snprintf(srvna, MAXLINE, "-na %d", na);
             if (async == TRUE)
                na = 1;}
         else if (strcmp(v[i], "+na") == 0)
	    {na = SC_stoi(v[++i]);
	     na = max(na, 1);
	     snprintf(srvna, MAXLINE, "+na %d", na);
             if (async == TRUE)
                na = 1;
	     snprintf(s, MAXLINE, "SC_EXEC_N_ATTEMPTS=%d", na);
	     SC_putenv(s);}
         else if (strcmp(v[i], "-pna") == 0)
	    {na = SC_stoi(v[++i]);
	     na = max(na, 1);
	     snprintf(s, MAXLINE, "SC_EXEC_N_ATTEMPTS=%d", na);
	     SC_putenv(s);}
         else if (strcmp(v[i], "-rcr") == 0)
	    recur = TRUE;
         else if (strcmp(v[i], "-s") == 0)
	    show = 0;

	 else if (strcmp(v[i], "-srv") == 0)
	    {phase = 3;
	     if (mem == TRUE)
	        cs = SC_mem_monitor(-1, 2, "dmake", msg);
	     st = SC_exec_server(shell, flt, na, show, ignore, debug);
	     if (mem == TRUE)
	        SC_mem_monitor(cs, 2, "dmake", msg);
	     return(st);}

	 else if (strcmp(v[i], "-st") == 0)
	    {dpsys = v[++i];
	     break;}
	 else if (strcmp(v[i], "-sys") == 0)
	    snprintf(state->arch, MAXLINE, "%s", v[++i]);
	 else if (strcmp(v[i], "-v") == 0)
	    {report_info(state->root, state->complete, state->literal,
			 VERSION, NULL);
	     return(0);}
         else if (strcmp(v[i], "-vrb") == 0)
	    {show = 2;
             state->verbose = TRUE;}
	 else
            {if (strchr(v[i], '=') != NULL)
	        SC_make_def_var(state, v[i], -2);
	     else
	        tgt = v[i];};};

    if (mem == TRUE)
       cs = SC_mem_monitor(-1, 2, "dmake", msg);

    if (debug == TRUE)
       snprintf(server, MAXLINE, "%s -dbg %s -srv", state->exe, srvna);
    else
       snprintf(server, MAXLINE, "%s %s -srv", state->exe, srvna);

    if (dpsys != NULL)
       {st = make_by_host(dpsys, v+i+1, shell, env, flt, na, show, ignore);
	SC_free_state(state);
	return(st);};

    if (tgt == NULL)
       {io_printf(stdout, "Must specify a target - exiting\n");
	return(1);};

    if (dpdir != NULL)
       {st = make_distributed(tgt, dpdir, server, shell, env,
			      flt, na, show, ignore);
	SC_free_state(state);
	return(st);};

    if (mkname == NULL)
       {mkname = "pre-Make";
        if (SC_isfile(mkname) == 0)
	   {mkname = "Makefile";
	    if (SC_isfile(mkname) == 0)
	       {io_printf(stdout, "No make file specified or found - exiting\n");
		return(1);};};};

/* parse makefile(s) */
    phase = 1;

/* compute offset to end of mkname - length of "pre-Make"
 * to allow things like -f <path>/pre-Make to work
 */
    no = strlen(mkname) - 8;
    no = max(no, 0);

    if (strcmp(mkname, ".command") == 0)
       rv = command_makefile(state, v+i+1);

/* read a pre-Make if that is what is requested */
    else if (strcmp(mkname+no, "pre-Make") == 0)
       rv = SC_parse_premake(state, mkname);

/* read anything else as if a complete Makefile */
    else
       rv = SC_parse_makefile(state, mkname);

    if (rv != TRUE)
       {err = SC_error_msg();
	if ((err != NULL) && (err[0] != '\0'))
	   io_printf(stdout, "PACT: %s\n", err);
	else
	   {char *cwd;

	    cwd = SC_getcwd();
	    io_printf(stdout, "PACT: error %d parsing %s/%s\n", rv, cwd, mkname);
	    CFREE(cwd);};

	st = 1;}

    else
       {SC_show_make_rules(state);

	shell = _SC_var_lookup(state, "SHELL");
	shell = CSTRSAVE(shell);
	snprintf(s, MAXLINE, "SHELL=%s", shell);

/* GOTCHA: POSIX says that we cannot set the SHELL environment variable
	SC_putenv(s);
*/

/* analyze dependencies */
	phase = 2;

	nt = SC_analyze_dependencies(state, tgt);
	SC_ASSERT(nt >= 0);

	cmnds = SC_action_commands(state, recur);

	SC_free_state(state);

/* execute commands */
	phase = 3;

	st = 0;
	if ((cmnds != NULL) && (cmnds[0] != NULL))
	   {if (dryrun == TRUE)
	       {for (i = 0; TRUE; i++)
		    {if (cmnds[i] == NULL)
		        break;
		     io_printf(stdout, "%s\n", cmnds[i]);};}

	    else if (async == TRUE)
	       st = SC_exec_async(shell, cmnds, NULL,
				  server, env, nconn,
				  log, flt, na, show, ignore, dmp);

	    else
	       st = SC_exec_commands(shell, cmnds, env, -1,
				     log, flt, na, show, ignore, dmp);}

	else
	   io_printf(stdout, "Nothing to be done for '%s'\n", tgt);

/* cleanup */
	phase = 4;

	if (SC_gs.assert_fail > 0)
	   {io_printf(stdout, "***> %d internal assertions violated\n",
		      SC_gs.assert_fail);
/*	    st = 1; */};

	CFREE(shell);
	if (cmnds != NULL)
	   SC_free_strings(cmnds);};

    if (mem == TRUE)
       SC_mem_monitor(cs, 2, "dmake", msg);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
