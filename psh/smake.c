/*
 * SMAKE.C - wrapper to control make for PACT's needs
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * include "cpyright.h"
 *
 */

#include "common.h"
#include "libpsh.c"
#include "libinfo.c"

typedef struct s_statedes statedes;

struct s_statedes
   {int show;
    int complete;
    int literal;
    int db_only;
    int quote;
    char sys[BFLRG];
    char arch[BFLRG];
    char root[BFLRG];
    char cwd[BFLRG];
    char srcdir[BFLRG];
    char tmpdir[BFLRG];};

int system();
int atoi();
char *getcwd();
char *getenv();
void exit(int status);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MANAGE_TMP_DIR - manage a temporary directory to guarantee multi-platform
 *                - builds do not interfere with each other
 *                - the DIR_WAS_THERE variable is to manage the situation
 *                - of recursive pact invocations
 */

void manage_tmp_dir(statedes *st, int start)
   {int ss;
    char cmd[BFLRG];
    static int dir_was_there = FALSE;

/* do this at the beginning */
    if (start == TRUE)
       {nstrncpy(st->srcdir, BFLRG, st->cwd, -1);

#ifdef USE_TMP_DIR
        if (strncmp(st->cwd, "/tmp/pact_make_", 15) == 0)
	   nstrncpy(st->tmpdir, BFLRG, st->cwd, -1);
        else
	    snprintf(st->tmpdir, BFLRG, "/tmp/pact_make_%d",
		     (int) getpid());
#else
	snprintf(st->tmpdir, BFLRG, "%s/%s%s/obj",
		 st->cwd, PSY_Prefix, st->sys);
#endif

	snprintf(cmd, BFLRG, "test -d %s/", st->tmpdir);
	ss = system(cmd);
	if (ss != 0)
	   dir_was_there = FALSE;
	else
	   dir_was_there = TRUE;

        if (dir_was_there == FALSE)
	   {snprintf(cmd, BFLRG, "mkdir -p %s/", st->tmpdir);
	    ss = system(cmd);};}

/* do this at the end */
    else
       {if (dir_was_there == FALSE)
	   {snprintf(cmd, BFLRG, "rm -rf %s/", st->tmpdir);
	    ss = system(cmd);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HANDLER - handle signals gracefully */

static void handler(int sig)
   {

    fprintf(stdout, "PACT exiting with signal %d\n", sig);

    exit(sig);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INVOKE_MAKE - complete the command line CMD and exec it */

static int invoke_make(statedes *st, char *cmd, int nc, char *mkf, int c, char **v)
   {int i, rv, na, ns;
    char s[BFLRG];
    char *log, *p;
    
#if defined(USE_GNU_MAKE)
    p = cwhich("gmake");
    if ((IS_NULL(p) == TRUE) || (strcmp(p, "none") == 0))
       snprintf(s, BFLRG, "make --no-print-directory -f -");
    else
       snprintf(s, BFLRG, "gmake --no-print-directory -f -");
#else
    snprintf(s, BFLRG, "make -f -");
#endif

    nstrcat(cmd, nc, s);

    p = getenv("SC_EXEC_N_ATTEMPTS");
    if (p != NULL)
       {na = atoi(p);
        na = max(na, 1);}
    else
       na = 3;

    log = NULL;
    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-f") == 0)
            i++;

         else if (strcmp(v[i], "-log") == 0)
            log = v[++i];

         else if (strcmp(v[i], "-cmd") == 0)
            break;

	 else if ((strcmp(v[i], "-na") == 0)   ||
		  (strcmp(v[i], "+na") == 0))
            na = atoi(v[++i]);

	 else if (strcmp(v[i], "-async") == 0)
	    {

#ifdef USE_GNU_MAKE
	     snprintf(s, BFLRG, " -j %d", atoi(v[++i]));
	     nstrcat(cmd, nc, s);
#endif
	    }

	 else
	    {nstrcat(cmd, nc, " ");
	     nstrcat(cmd, nc, v[i]);};};

    if (log != NULL)
       {snprintf(s, BFLRG, " | tee -a %s", log);
	nstrcat(cmd, nc, s);};

/* do the make */
    if (st->show)
       printf("Command: %s\n", cmd);

/* retry until successful */
    for (i = 0; i < na; i++)
        {rv = system(cmd);   
	 if (rv == 0)
	    break;
	 else if (i+1 < na)
            {ns = (i < 1) ? 1 : 30;
	     printf("***> failed (%d) - attempt %d in %d seconds\n",
		    rv, i+2, ns);
	     printf("***>      %s\n", cmd);
	     printf("***> retry '%s' - task 1\n", cmd);
	     sleep(ns);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* METHOD_1 - try to set make-def, pre-Make, and make-macros to make
 *          - via stdin
 *          - PROBLEM: on some systems the input to stdin is routed
 *          - to a temporary file and file system errors under load
 *          - can kill you
 */

static int method_1(statedes *st, int c, char **v, char *pmname)
   {int i, status;
    char cmd[BFLRG], s[BFLRG];

/* find the current directory */
    if (getcwd(st->cwd, BFLRG) == NULL)
       {fprintf(stderr, "ERROR: CAN'T GET CURRENT DIRECTORY\n");
        fprintf(stderr, "   %d - %s\n", errno, strerror(errno));
        return(1);};

/* manage the hidden directory for the temporary files */
    manage_tmp_dir(st, TRUE);

/* write the command line */
    nstrncpy(cmd, BFLRG, "(", -1);
	
    i = strlen(st->cwd) - 7;
    i = max(i, 0);
    if (strcmp(st->cwd+i, "manager") == 0)
       {snprintf(s, BFLRG, "echo \"PSY_ID = %s\" ; ", st->sys);

        snprintf(s, BFLRG, "cat %s", pmname);
	nstrcat(cmd, BFLRG, s);}

    else if (strcmp(pmname, "Makefile") == 0)
       {snprintf(s, BFLRG, "cat %s ; ", pmname);
	nstrcat(cmd, BFLRG, s);}

    else
       {snprintf(s, BFLRG, "echo \"include %s/etc/make-def\" ; ", st->root);
	nstrcat(cmd, BFLRG, s);

        snprintf(s, BFLRG, "echo \"PACTSrcDir = %s\" ; ", st->srcdir);
	nstrcat(cmd, BFLRG, s);

	for (i = 1; i < c; i++)
	    {if ((strcmp(v[i], "-f") == 0) &&
		 (strstr(v[i+1], "pre-Make") != NULL))
	        {pmname = v[i+1];
		 break;};};

        snprintf(s, BFLRG, "cat %s ; ", pmname);
	nstrcat(cmd, BFLRG, s);

	snprintf(s, BFLRG, "echo \"include %s/etc/make-macros\"", st->root);
	nstrcat(cmd, BFLRG, s);};

    nstrcat(cmd, BFLRG, ") | ");

/* make the command to invoke make */
    status = invoke_make(st, cmd, BFLRG, "-", c, v);

/* remove the hidden directory for the temporary files */
    manage_tmp_dir(st, FALSE);

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* METHOD_2 - try to use a pre-built Makefile in MKFILE */

static int method_2(statedes *st, int c, char **v, char *mkfile, long tmm)
   {int status;
    long tmp;
    char cmd[BFLRG];
    struct stat sbf;

/* check to see if the pre-Make is newer than the Makefile */
    if (stat("pre-Make", &sbf) == 0)
       {if (S_ISREG(sbf.st_mode))
	   {tmp = sbf.st_mtime;
	    if (tmp > tmm)
               build_makefile(st->root, st->arch, mkfile, FALSE);};};

/* make the command to invoke make */
    cmd[0] = '\0';
    status = invoke_make(st, cmd, BFLRG, mkfile, c, v);

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMMAND_MAKEFILE - construct a database to run
 *                  - a command specified in A[1-N]
 *                  - in each directory specified in the file A[0]
 *                  - return 0 iff successful
 */

static int command_makefile(statedes *st, char *fname, int c, char **v, char **a)
   {int i, err;
    char s[BFLRG], u[BFLRG], cmd[BFLRG];
    char *p, *tok, *dir, **sa;

    err = 1;

/* make up the command string */
    s[0] = '\0';
    for (i = 0; TRUE; i++)
        {tok = a[i];
	 if (tok == NULL)
	    break;
	 nstrcat(s, BFLRG, tok);
	 nstrcat(s, BFLRG, " ");};

    sa = file_text(FALSE, fname);
    if (sa != NULL)
       {snprintf(cmd, BFLRG, "(echo \"go :\"");

	for (i = 0; sa[i] != NULL; i++)
	    {p   = sa[i];
	     dir = strtok(p, "\n");
	     if (dir == NULL)
	        break;
	     snprintf(u, BFLRG,
		      " ; echo \"\t@(cd %s ; echo \"\" ; echo \"In %s doing %s\" ; %s)\"",
		      dir, dir, s, s);
	     nstrcat(cmd, BFLRG, u);};

	free_strings(sa);

	nstrcat(cmd, BFLRG, ") | ");
	err = invoke_make(st, cmd, BFLRG, "-", c, v);};

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_ENV - initialize the ARCH and ROOT strings
 *           - return TRUE iff successful with both
 */

static int setup_env(statedes *st, char *src)
   {char s[BFLRG], npath[BFLRG];
    char *p, *ps;

    st->show     = 0;
    st->complete = FALSE;
    st->literal  = FALSE;
    st->db_only  = FALSE;

    memset(st->cwd, 0, BFLRG);

    file_path(src, st->root, BFLRG);

    st->sys[0] = '\0';

/* remove exe name */
    p = pop_path(st->root);

/* put the directory of the executable at the head of the path
 * important for multi-platform builds
 */
    nstrncpy(npath, BFLRG, getenv("PATH"), -1);
    push_path(P_PREPEND, npath, st->root);
    setenv("PATH", npath, 1);

/* remove bin dir */
    p = pop_path(st->root);

/* setup the architecture string */
    p = getenv("SESSION_CONFIG");
    if (p != NULL)
       {strncpy(st->sys, p, BFLRG);
	st->sys[BFLRG-1] = '\0';}

    else
       {nstrncpy(s, BFLRG, st->root, -1);

	ps = pop_path(s);
	p  = pop_path(s);

/* if root is of the form /.../dev/<PSY_id> use <PSY_id> */
	if (strcmp(p, "dev") == 0)
	   nstrncpy(st->sys, BFLRG, ps, -1);

/* otherwise use the configured PSY_ID */
	else
	   nstrncpy(st->sys, BFLRG, PSY_ID, -1);};

    snprintf(st->arch, BFLRG, "%s%s", PSY_Prefix, st->sys);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* USAGE - print help */

void usage(void)
   {

    printf("\n");
    printf("Usage: pact [-async <n>] [-B] [-h] [-incpath] [-info <pttrn> | <var>] [+info <var>]\n");
    printf("            [-log <file>] [-link] [+l] [-na <n>] [-sys <dir>] [-v] <make-args> <target>\n");
    printf("\n");

    printf("Make Options:\n");
    printf("    -B       build Makefile from pre-Make\n");
    printf("    -async   number of processes to use\n");
    printf("    -h       this help message\n");
    printf("    -na      number of attempts to make\n");
    printf("    -log     log output to designated file\n");
    printf("    -sys     directory in which to put/find Makefile\n");
    printf("\n");

    printf("Info Options:\n");
    printf("    -db      use the database only\n");
    printf("    -incpath report elements needed to compile files\n");
    printf("    -info    report configuration elements matching the argument\n");
    printf("    +info    complete report of configuration variable matching the argument\n");
    printf("    -link    report elements needed to link applications\n");
    printf("    +l       report only exact matches of the argument\n");
    printf("    -v       report the PACT version\n");
    printf("\n");
    printf("    All other arguments are passed into make\n");
    printf("\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int i, status, ok, mc;
    long tmm;
    char mkfile[BFLRG];
    char *pmname, *mv[100];
    statedes st;
    static char *default_pmname = "pre-Make";
    struct stat sbf;

    nsigaction(NULL, SIGINT, handler, SA_RESTART, -1);

    pmname = default_pmname;

/* find/get SESSION_CONFIG before setup_env */
    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-a") == 0)
	    {setenv("SESSION_CONFIG", v[++i], 1);
	     break;};};

    setup_env(&st, v[0]);

    ok = TRUE;
    mc = 1;
    for (i = 1; (i < c) && ok; i++)

/* handle dmake -cmd option */
	{if (strcmp(v[i], "-cmd") == 0)
            {i++;
	     ok = command_makefile(&st, v[i], c, v, v+i+1);
	     return(ok);}

         else if (strcmp(v[i], "-db") == 0)
	    st.db_only = 1;

	 else if (strcmp(v[i], "-incpath") == 0)
	    {report_info(st.root, st.complete, st.literal, st.db_only,
			 INCL, NULL);
	     return(0);}

	 else if (strcmp(v[i], "-link") == 0)
	    {report_info(st.root, st.complete, st.literal, st.db_only,
			 LINK, NULL);
	     return(0);}

         else if (strcmp(v[i], "-show") == 0)
	    st.show = 1;

	 else if (strcmp(v[i], "-sys") == 0)
	    snprintf(st.arch, BFLRG, "%s", v[++i]);

/* get dmake options taking the next argument later */
	 else if ((strcmp(v[i], "-async") == 0) ||
		  (strcmp(v[i], "-na") == 0)   ||
		  (strcmp(v[i], "+na") == 0)   ||
		  (strcmp(v[i], "-log") == 0))
	    {mv[mc++] = v[i++];
	     mv[mc++] = v[i];}

/* ignore dmake options taking the next argument */
	 else if ((strcmp(v[i], "-dst") == 0)   ||
		  (strcmp(v[i], "-flt") == 0)   ||
		  (strcmp(v[i], "-cmd") == 0))
            {printf("smake: option %s ignored\n", v[i]);
	     i++;}

/* ignore dmake options taking no argument */
	 else if ((strcmp(v[i], "-dbg") == 0) ||
		  (strcmp(v[i], "-dmp") == 0) ||
		  (strcmp(v[i], "-rcr") == 0) ||
		  (strcmp(v[i], "-srv") == 0) ||
		  (strcmp(v[i], "-vrb") == 0))
            {printf("smake: option %s ignored\n", v[i]);
	     continue;}

         else if (v[i][0] == '-')
            {switch (v[i][1])
	        {case 'B' :
                      snprintf(mkfile, BFLRG, "%s/Makefile", st.arch);
                      status = build_makefile(st.root, st.arch, mkfile, TRUE);
		      return(status);
		      break;
		 case 'h' :
		      usage();
		      return(1);
		      break;
		 case 'i' :
		      if (strcmp(v[i], "-info") == 0)
			 {if (++i < c)
			     report_info(st.root, st.complete, st.literal,
					 st.db_only, REGEX, v[i]);
			  return(0);};
		      i--;
		      ok = FALSE;
		      break;
		 case 'f' :
		      pmname = v[++i];
		      break;
	         case 'n' :
                      mv[mc++] = v[i];
		      break;
	         case 'v' :
		      report_info(st.root, st.complete, st.literal, st.db_only,
				  VERS, NULL);
		      return(0);};}

	 else if (v[i][0] == '+')
            {switch (v[i][1])
                {case 'i' :
		      if (strcmp(v[i], "+info") == 0)
			 {if (++i < c)
			     {st.complete = TRUE;
			      report_info(st.root, st.complete, st.literal,
					  st.db_only, REGEX, v[i]);};
			  return(0);};
		      break;
		 case 'l' :
		      st.literal = TRUE;
		      break;};}
	 else
	    {mv[mc++] = v[i];
	     ok = FALSE;};};

    for (; i < c; i++)
        mv[mc++] = v[i];

    mv[mc] = NULL;
    mv[0]  = v[0];

    snprintf(mkfile, BFLRG, "%s/Makefile", st.arch);
    if ((stat(mkfile, &sbf) == 0) && (S_ISREG(sbf.st_mode)))
       {tmm    = (long) sbf.st_mtime;
	status = method_2(&st, mc, mv, mkfile, tmm);}
    else
       status = method_1(&st, mc, mv, pmname);

    log_safe("dump", 0, NULL, NULL);

/* reverse the status */
    status = (status == 0) ? 0 : 1;

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
