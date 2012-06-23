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
    int quote;
    char sys[MAXLINE];
    char arch[MAXLINE];
    char root[MAXLINE];
    char cwd[MAXLINE];
    char srcdir[MAXLINE];
    char tmpdir[MAXLINE];};

int system();
int atoi();
char *getcwd();
char *getenv();
void (*signal())();
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
    char cmd[MAXLINE];
    static int dir_was_there = FALSE;

/* do this at the beginning */
    if (start == TRUE)
       {strcpy(st->srcdir, st->cwd);

#ifdef USE_TMP_DIR
        if (strncmp(st->cwd, "/tmp/pact_make_", 15) == 0)
	   strcpy(st->tmpdir, st->cwd);
        else
	    sprintf(st->tmpdir, "/tmp/pact_make_%d", (int) getpid());
#else
	sprintf(st->tmpdir, "%s/z-%s/obj", st->cwd, st->sys);
#endif

	snprintf(cmd, MAXLINE, "test -d %s/", st->tmpdir);
	ss = system(cmd);
	if (ss != 0)
	   dir_was_there = FALSE;
	else
	   dir_was_there = TRUE;

        if (!dir_was_there)
	   {snprintf(cmd, MAXLINE, "mkdir -p %s/", st->tmpdir);
	    ss = system(cmd);};}

/* do this at the end */
    else
       {if (!dir_was_there)
	   {snprintf(cmd, MAXLINE, "rm -rf %s/", st->tmpdir);
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
    char s[MAXLINE];
    char *log, *p;
    
#if defined(USE_GNU_MAKE)
    snprintf(s, MAXLINE, "gmake --no-print-directory -f -");
#else
    snprintf(s, MAXLINE, "make -f -");
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
	     snprintf(s, MAXLINE, " -j %d", atoi(v[++i]));
	     nstrcat(cmd, nc, s);
#endif
	    }

	 else
	    {nstrcat(cmd, nc, " ");
	     nstrcat(cmd, nc, v[i]);};};

    if (log != NULL)
       {snprintf(s, MAXLINE, " | tee -a %s", log);
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
    char cmd[MAXLINE], s[MAXLINE];

/* find the current directory */
    if (getcwd(st->cwd, MAXLINE) == NULL)
       {fprintf(stderr, "ERROR: CAN'T GET CURRENT DIRECTORY\n");
        fprintf(stderr, "   %d - %s\n", errno, strerror(errno));
        return(1);};

/* manage the hidden directory for the temporary files */
    manage_tmp_dir(st, TRUE);

/* write the command line */
    strcpy(cmd, "(");
	
    i = strlen(st->cwd) - 7;
    i = max(i, 0);
    if (strcmp(st->cwd+i, "manager") == 0)
       {snprintf(s, MAXLINE, "echo \"System = %s\" ; ", st->sys);

        snprintf(s, MAXLINE, "cat %s", pmname);
	nstrcat(cmd, MAXLINE, s);}

    else if (strcmp(pmname, "Makefile") == 0)
       {snprintf(s, MAXLINE, "cat %s ; ", pmname);
	nstrcat(cmd, MAXLINE, s);}

    else
       {snprintf(s, MAXLINE, "echo \"include %s/etc/make-def\" ; ", st->root);
	nstrcat(cmd, MAXLINE, s);

        snprintf(s, MAXLINE, "echo \"PACTTmpDir = %s\" ; ", st->tmpdir);
	nstrcat(cmd, MAXLINE, s);

        snprintf(s, MAXLINE, "echo \"PACTSrcDir = %s\" ; ", st->srcdir);
	nstrcat(cmd, MAXLINE, s);

	for (i = 1; i < c; i++)
	    {if ((strcmp(v[i], "-f") == 0) &&
		 (strstr(v[i+1], "pre-Make") != NULL))
	        {pmname = v[i+1];
		 break;};};

        snprintf(s, MAXLINE, "cat %s ; ", pmname);
	nstrcat(cmd, MAXLINE, s);

	snprintf(s, MAXLINE, "echo \"include %s/etc/make-macros\"", st->root);
	nstrcat(cmd, MAXLINE, s);};

    nstrcat(cmd, MAXLINE, ") | ");

/* make the command to invoke make */
    status = invoke_make(st, cmd, MAXLINE, "-", c, v);

/* remove the hidden directory for the temporary files */
    manage_tmp_dir(st, FALSE);

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* METHOD_2 - try to use a pre-built Makefile in MKFILE */

static int method_2(statedes *st, int c, char **v, char *mkfile, long tmm)
   {int status;
    long tmp;
    char cmd[MAXLINE];
    struct stat sbf;

/* check to see if the pre-Make is newer than the Makefile */
    if (stat("pre-Make", &sbf) == 0)
       {if (S_ISREG(sbf.st_mode))
	   {tmp = sbf.st_mtime;
	    if (tmp > tmm)
               build_makefile(st->root, st->arch, mkfile, FALSE);};};

/* make the command to invoke make */
    cmd[0] = '\0';
    status = invoke_make(st, cmd, MAXLINE, mkfile, c, v);

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
    char s[MAXLINE], u[MAXLINE], cmd[MAXLINE];
    char *p, *tok, *dir, **sa;

    err = 1;

/* make up the command string */
    s[0] = '\0';
    for (i = 0; TRUE; i++)
        {tok = a[i];
	 if (tok == NULL)
	    break;
	 nstrcat(s, MAXLINE, tok);
	 nstrcat(s, MAXLINE, " ");};

    sa = file_text(FALSE, fname);
    if (sa != NULL)
       {snprintf(cmd, MAXLINE, "(echo \"go :\"");

	for (i = 0; sa[i] != NULL; i++)
	    {p   = sa[i];
	     dir = strtok(p, "\n");
	     if (dir == NULL)
	        break;
	     snprintf(u, MAXLINE,
		      " ; echo \"\t@(cd %s ; echo \"\" ; echo \"In %s doing %s\" ; %s)\"",
		      dir, dir, s, s);
	     nstrcat(cmd, MAXLINE, u);};

	free_strings(sa);

	nstrcat(cmd, MAXLINE, ") | ");
	err = invoke_make(st, cmd, MAXLINE, "-", c, v);};

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_ENV - initialize the ARCH and ROOT strings
 *           - return TRUE iff successful with both
 */

static int setup_env(statedes *st, char *src)
   {char s[MAXLINE];
    char *p, *ps;

    st->show     = 0;
    st->complete = FALSE;
    st->literal  = FALSE;

    memset(st->cwd, 0, MAXLINE);

    file_path(src, st->root, MAXLINE);

    st->sys[0] = '\0';

/* remove exe name */
    p = pop_path(st->root);

/* remove bin dir */
    p = pop_path(st->root);

/* setup the architecture string */
    p = getenv("SESSION_CONFIG");
    if (p != NULL)
       {strncpy(st->sys, p, MAXLINE);
	st->sys[MAXLINE-1] = '\0';}

    else
       {strcpy(s, st->root);

	ps = pop_path(s);
	p  = pop_path(s);

/* if root is of the form /.../dev/<arch> use <arch> */
	if (strcmp(p, "dev") == 0)
	   strcpy(st->sys, ps);

/* otherwise use the configured system_id */
	else
	   strcpy(st->sys, SYSTEM_ID);};

    sprintf(st->arch, "z-%s", st->sys);

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

    printf("Info Options:\n");
    printf("    -async   number of processes to use\n");
    printf("    -na      number of attempts to make\n");
    printf("    -log     log output to designated file\n");
    printf("\n");

    printf("Info Options:\n");
    printf("    -B       build Makefile from pre-Make\n");
    printf("    -h       this help message\n");
    printf("    -incpath report elements needed to compile files\n");
    printf("    -info    report configuration elements matching the argument\n");
    printf("    +info    complete report of configuration variable matching the argument\n");
    printf("    -link    report elements needed to link applications\n");
    printf("    +l       report only exact matches of the argument\n");
    printf("    -sys     directory in which to put/find Makefile\n");
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
    char mkfile[MAXLINE];
    char *pmname, *mv[100];
    statedes st;
    static char *default_pmname = "pre-Make";
    struct stat sbf;

    signal(2, handler);

    pmname = default_pmname;

    setup_env(&st, v[0]);

    ok = TRUE;
    mc = 1;
    for (i = 1; (i < c) && ok; i++)
        {if (strcmp(v[i], "-sys") == 0)
	    sprintf(st.arch, "%s", v[++i]);

         else if (strcmp(v[i], "-show") == 0)
	    st.show = 1;

/* get dmake options taking the next argument later */
	 else if ((strcmp(v[i], "-async") == 0) ||
		  (strcmp(v[i], "-na") == 0)   ||
		  (strcmp(v[i], "+na") == 0)   ||
		  (strcmp(v[i], "-log") == 0))
	    {mv[mc++] = v[i++];
	     mv[mc++] = v[i];}

/* handle dmake -cmd option */
	 else if (strcmp(v[i], "-cmd") == 0)
            {i++;
	     ok = command_makefile(&st, v[i], c, v, v+i+1);
	     return(ok);}

/* ignore dmake options taking the next argument */
	 else if ((strcmp(v[i], "-dst") == 0)   ||
		  (strcmp(v[i], "-flt") == 0)   ||
		  (strcmp(v[i], "-cmd") == 0))
            {printf("smake: option %s ignored\n", v[i]);
	     i++;}

	 else if (strcmp(v[i], "-incpath") == 0)
	    {report_info(st.root, st.complete, st.literal, INC, NULL);
	     return(0);}

	 else if (strcmp(v[i], "-link") == 0)
	    {report_info(st.root, st.complete, st.literal, LINK, NULL);
	     return(0);}

/* ignore dmake options taking no argument */
	 else if ((strcmp(v[i], "-dbg") == 0) ||
		  (strcmp(v[i], "-dmp") == 0) ||
		  (strcmp(v[i], "-srv") == 0) ||
		  (strcmp(v[i], "-vrb") == 0))
            {printf("smake: option %s ignored\n", v[i]);
	     continue;}

         else if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'B' :
                      snprintf(mkfile, MAXLINE, "%s/Makefile", st.arch);
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
					 REGEX, v[i]);
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
		      report_info(st.root, st.complete, st.literal,
				  VERSION, NULL);
		      return(0);};}

	 else if (v[i][0] == '+')
            {switch (v[i][1])
                {case 'i' :
		      if (strcmp(v[i], "+info") == 0)
			 {if (++i < c)
			     {st.complete = TRUE;
			      report_info(st.root, st.complete, st.literal,
					  REGEX, v[i]);};
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

    snprintf(mkfile, MAXLINE, "%s/Makefile", st.arch);
    if (stat(mkfile, &sbf) == 0)
       {if (S_ISREG(sbf.st_mode))
	   {tmm    = (long) sbf.st_mtime;
	    status = method_2(&st, mc, mv, mkfile, tmm);
	    status = (status == 0) ? 0 : 1;
	    return(status);};};

    status = method_1(&st, mc, mv, pmname);

/* reverse the status */
    status = (status == 0) ? 0 : 1;

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
