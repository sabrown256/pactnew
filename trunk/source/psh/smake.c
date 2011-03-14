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

#undef MAXLINE
#define MAXLINE 4096

static int
 show = 0;

static char
 sys[MAXLINE],
 arch[MAXLINE],
 root[MAXLINE],
 cwd[MAXLINE],
 srcdir[MAXLINE],
 tmpdir[MAXLINE];

int system();
int atoi();
char *getcwd();
char *getenv();
void (*signal())();
void exit(int status);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REPORT_VAR - report on the variable Q in FILE */

static int report_var(char *file, char *q, char *key,
		      int compl, int litrl, int newl)
   {int i, ok, doit, tst;
    char *p, *tok, *txt, *ps, **sa;

    ok = FALSE;

    sa = file_text(file);
    if (sa != NULL)
       {for (i = 0; sa[i] != NULL; i++)
	    {p = sa[i];
	     if (key != NULL)
	        {tok  = strtok(p, " \t\r");
		 doit = ((tok != NULL) && (strcmp(tok, key) == 0));
		 ps   = NULL;}
	     else
	        {doit = TRUE;
		 ps   = p;};

	     if (doit)
	        {tok = strtok(ps, " \t\n");
		 if (tok == NULL)
		    continue;

		 tst = (litrl) ? (strcmp(tok, q) == 0) :
		                 (strncmp(tok, q, strlen(q)) == 0);

		 if (tst)
		    {txt = strtok(NULL, "\n");

/* with env-pact.sh you WILL get here with txt NULL and tok <var>=<val> */
		     if (txt == NULL)
		        {p = strchr(tok, '=');
			 if (p != NULL)
			    {*p++ = '\0';
			     txt  = p;};}

/* with env-pact.csh you WILL get here with txt <val> and tok <var> */
		     else
		        {while (*txt != '\0')
			    {if (strchr("= \t", *txt) == NULL)
			        break;
			     else
			        txt++;};};

		     if (compl)
		        printf("%s = %s", tok, txt);
		     else
		        printf("%s", txt);

		     ok = TRUE;

		     if (newl == TRUE)
		        printf("\n");
                     else
		        printf(" ");};};};

	free_strings(sa);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REPORT - report the value of one of the configuration quantities */

static void report(char *q, int compl, int litrl, int newl)
   {int ok;
    char devdir[MAXLINE], file[MAXLINE];
    char *s;

    strcpy(devdir, root);

    ok = FALSE;

    if (strcmp(q, "make") == 0)
       {snprintf(file, MAXLINE, "%s/include/make-def", devdir);
        ok = report_var(file, "UMake", NULL, compl, litrl, newl);};

    if (strcmp(q, "config") == 0)
       {snprintf(file, MAXLINE, "%s/include/make-def", devdir);
        ok = report_var(file, "System", NULL, compl, litrl, newl);};

    if (!ok)
       {snprintf(file, MAXLINE, "%s/include/scconfig.h", devdir);
	ok = report_var(file, q, "#define", compl, litrl, newl);};

    if (!ok)
       {snprintf(file, MAXLINE, "%s/include/make-def", devdir);
	ok = report_var(file, q, NULL, compl, litrl, newl);};

    if (!ok)
       {snprintf(file, MAXLINE, "%s/include/configured", devdir);
	ok = report_var(file, q, NULL, compl, litrl, newl);};

    if (!ok)
       {s = getenv("SHELL");
	if (s != NULL)
	   {if (strstr(s, "csh") != NULL)
	       {snprintf(file, MAXLINE, "%s/include/env-pact.csh", devdir);
		ok = report_var(file, q, "setenv", compl, litrl, newl);}
	    else
	       {snprintf(file, MAXLINE, "%s/include/env-pact.sh", devdir);
		ok = report_var(file, q, "export", compl, litrl, newl);};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REPORT_CL - report the information relevant to compiling and linking */

static void report_cl(char *q)
   {

    if (strcmp(q, "-incpath") == 0)
       {report("MDGInc", FALSE, TRUE, FALSE);
        report("MDInc", FALSE, TRUE, TRUE);}

    else if (strcmp(q, "-link") == 0)
       {report("LDRPath", FALSE, TRUE, FALSE);
        report("LDPath", FALSE, TRUE, FALSE);
	report("DP_Lib", FALSE, TRUE, FALSE);
	report("MDGLib", FALSE, TRUE, FALSE);
	report("MDLib",  FALSE, TRUE, TRUE);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MANAGE_TMP_DIR - manage a temporary directory to guarantee multi-platform
 *                - builds do not interfere with each other
 *                - the DIR_WAS_THERE variable is to manage the situation
 *                - of recursive pact invocations
 */

void manage_tmp_dir(char *cwd, char *sname, int start)
   {int ss;
    char cmd[MAXLINE];
    static int dir_was_there = FALSE;

/* do this at the beginning */
    if (start == TRUE)
       {strcpy(srcdir, cwd);

#ifdef USE_TMP_DIR
        if (strncmp(cwd, "/tmp/pact_make_", 15) == 0)
	   strcpy(tmpdir, cwd);
        else
	    sprintf(tmpdir, "/tmp/pact_make_%d", (int) getpid());
#else
	sprintf(tmpdir, "%s/z-%s/obj", cwd, sname);
#endif

	snprintf(cmd, MAXLINE, "test -d %s/", tmpdir);
	ss = system(cmd);
	if (ss != 0)
	   dir_was_there = FALSE;
	else
	   dir_was_there = TRUE;

        if (!dir_was_there)
	   {snprintf(cmd, MAXLINE, "mkdir -p %s/", tmpdir);
	    ss = system(cmd);};}

/* do this at the end */
    else
       {if (!dir_was_there)
	   {snprintf(cmd, MAXLINE, "rm -rf %s/", tmpdir);
	    ss = system(cmd);};};


    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BUILD_MAKEFILE - make the specified Makefile from the pre-Make
 *                - in the current directory
 *                - return 0 iff successful
 */

int build_makefile(char *mkfile)
   {int err;
    char cmd[MAXLINE], makef[MAXLINE], inc[MAXLINE];

    err = 0;

    snprintf(inc, MAXLINE, "%s/include", root);
    snprintf(makef, MAXLINE, "%s", mkfile);

    snprintf(cmd, MAXLINE, "csh -c \"mkdir -p %s/obj >& /dev/null\"", arch);
    system(cmd);

/*    printf("\nMaking %s from pre-Make\n\n", makef); */

    snprintf(cmd, MAXLINE, "cp %s/make-def %s", inc, makef);
    err |= system(cmd);

    snprintf(cmd, MAXLINE, "echo PACTTmpDir = %s/obj >> %s", arch, makef);
    err |= system(cmd);

    snprintf(cmd, MAXLINE, "echo PACTSrcDir = ../.. >> %s", makef);
    err |= system(cmd);

    snprintf(cmd, MAXLINE, "cat pre-Make >> %s", makef);
    err |= system(cmd);

    snprintf(cmd, MAXLINE, "cat %s/make-macros >> %s", inc, makef);
    err |= system(cmd);

    return(err);}

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

static int invoke_make(char *cmd, int nc, char *mkf, int c, char **v)
   {int i, st, na, ns;
    char s[MAXLINE];
    char *log, *p;
    
#ifdef USE_GNU_MAKE
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
    if (show)
       printf("Command: %s\n", cmd);

/* retry until successful */
    for (i = 0; i < na; i++)
        {st = system(cmd);   
	 if (st == 0)
	    break;
	 else if (i+1 < na)
            {ns = (i < 1) ? 1 : 30;
	     printf("***> failed (%d) - attempt %d in %d seconds\n",
		    st, i+2, ns);
	     printf("***>      %s\n", cmd);
	     printf("***> retry '%s' - task 1\n", cmd);
	     sleep(ns);};};

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* METHOD_1 - try to set make-def, pre-Make, and make-macros to make
 *          - via stdin
 *          - PROBLEM: on some systems the input to stdin is routed
 *          - to a temporary file and file system errors under load
 *          - can kill you
 */

static int method_1(int c, char **v, char *pmname)
   {int i, status;
    char cmd[MAXLINE], s[MAXLINE];

/* find the current directory */
    if (getcwd(cwd, MAXLINE) == NULL)
       {fprintf(stderr, "ERROR: CAN'T GET CURRENT DIRECTORY\n");
        fprintf(stderr, "   %d - %s\n", errno, strerror(errno));
        return(1);};

/* manage the hidden directory for the temporary files */
    manage_tmp_dir(cwd, sys, TRUE);

/* write the command line */
    strcpy(cmd, "(");
	
    i = strlen(cwd) - 7;
    i = max(i, 0);
    if (strcmp(cwd+i, "manager") == 0)
       {snprintf(s, MAXLINE, "echo \"System = %s\" ; ", sys);

        snprintf(s, MAXLINE, "cat %s", pmname);
	nstrcat(cmd, MAXLINE, s);}

    else if (strcmp(pmname, "Makefile") == 0)
       {snprintf(s, MAXLINE, "cat %s ; ", pmname);
	nstrcat(cmd, MAXLINE, s);}

    else
       {snprintf(s, MAXLINE, "echo \"include %s/include/make-def\" ; ", root);
	nstrcat(cmd, MAXLINE, s);

        snprintf(s, MAXLINE, "echo \"PACTTmpDir = %s\" ; ", tmpdir);
	nstrcat(cmd, MAXLINE, s);

        snprintf(s, MAXLINE, "echo \"PACTSrcDir = %s\" ; ", srcdir);
	nstrcat(cmd, MAXLINE, s);

        snprintf(s, MAXLINE, "cat %s ; ", pmname);
	nstrcat(cmd, MAXLINE, s);

	snprintf(s, MAXLINE, "echo \"include %s/include/make-macros\"", root);
	nstrcat(cmd, MAXLINE, s);};

    nstrcat(cmd, MAXLINE, ") | ");

/* make the command to invoke make */
    status = invoke_make(cmd, MAXLINE, "-", c, v);

/* remove the hidden directory for the temporary files */
    manage_tmp_dir(cwd, sys, FALSE);

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* METHOD_2 - try to use a pre-built Makefile in MKFILE */

static int method_2(int c, char **v, char *mkfile, long tmm)
   {int status;
    long tmp;
    char cmd[MAXLINE];
    struct stat st;

/* check to see if the pre-Make is newer than the Makefile */
    if (stat("pre-Make", &st) == 0)
       {if (S_ISREG(st.st_mode))
	   {tmp = st.st_mtime;
	    if (tmp > tmm)
               build_makefile(mkfile);};};

/* make the command to invoke make */
    cmd[0] = '\0';
    status = invoke_make(cmd, MAXLINE, mkfile, c, v);

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMMAND_MAKEFILE - construct a database to run
 *                  - a command specified in A[1-N]
 *                  - in each directory specified in the file A[0]
 *                  - return 0 iff successful
 */

static int command_makefile(char *fname, int c, char **v, char **a)
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

    sa = file_text(fname);
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
	err = invoke_make(cmd, MAXLINE, "-", c, v);};

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POP_PATH - pop the tail of the path off
 *          - return a pointer to the head component
 */

char *pop_path(char *path)
   {char *p;

    p = strrchr(path, '/');
    if (p != NULL)
       *p++ = '\0';
    else
       p = path;

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IS_EXECUTABLE_FILE - if PATH is the name of an executable file
 *                    - and if the length of path is <= NCX
 *                    - copy it into FP and return the length
 */

static int is_executable_file(char *fp, char *path, int ncx)
   {int n, muid, mgid, fuid, fgid;
    int usrx, grpx, othx, file;
    struct stat bf;

    n = -1;

    muid = getuid();
    mgid = getgid();

    n = -1;
    if (stat(path, &bf) == 0)
       {fuid = bf.st_uid;
	fgid = bf.st_gid;
	file = bf.st_mode & S_IFREG;
	usrx = ((muid == fuid) && (bf.st_mode & S_IXUSR));
	grpx = ((mgid == fgid) && (bf.st_mode & S_IXGRP));
	othx = (bf.st_mode & S_IXOTH);
	if (file && (usrx || grpx || othx))
	   {n = strlen(path);
	    if (n <= ncx)
	       {strcpy(fp, path);
		n = 0;};};}

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HANDLE_PATH_DOT - given a directory DIR and path NAME
 *                 - resolve out ./ and ../ elements of NAME
 *                 - return result in PATH
 *                 - DIR may be destroyed in this process
 */

static void handle_path_dot(char *path, char *dir, char *name)
   {char *s;

    if (dir != NULL)
       {s = name;

/* loop over any number of ./ or ../ elements of the path */
	while (s[0] == '.')
	   {if (s[1] == '.')
	       {s += 3;
		pop_path(dir);}
	    else
	       s += 2;};

/* construct the candidate path */
	sprintf(path, "%s/%s", dir, s);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FILE_PATH - given the name of a file return the full path
 *           - if FULL is TRUE
 *           - otherwise expand out '.' and '..' constructions
 *           - input is the NAME, output is the PATH which is
 *           - at most NC chars long
 *           - return:
 *           -    0 on success
 *           -    n NAME is there but NC is too few characters
 *           -      n is the number of characters needed to contain
 *           -      the full path
 */

int file_path(char *name, char *path, int nc)
   {int n;
    size_t nb;
    char pathvar[MAXLINE], fp[MAXLINE];
    char *t, *p;
    extern char *getcwd(char *buf, size_t size);

    n  = -1;
    nb = MAXLINE - 1;
    switch (name[0])
       {case '/' :
	     n = is_executable_file(path, name, nc);
             break;

        case '.' :
             t = getcwd(pathvar, nb);
             handle_path_dot(fp, t, name);
	     n = is_executable_file(path, fp, nc);
             break;

        default:
	     p = getenv("PATH");
	     if (p != NULL)
	        {strncpy(pathvar, p, MAXLINE);
		 pathvar[MAXLINE-1] = '\0';

		 for (t = strtok(pathvar, ":");
		      t != NULL;
		      t = strtok(NULL, ":"))
		     {handle_path_dot(fp, t, name);
		      n = is_executable_file(path, fp, nc);
		      if (n == 0)
			 break;};};
             break;};

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_ENV - initialize the ARCH and ROOT strings
 *           - return TRUE iff successful with both
 */

static int setup_env(char *src, char *arch, char *root)
   {char s[MAXLINE];
    char *p, *ps;

    file_path(src, root, MAXLINE);

    sys[0] = '\0';

/* remove exe name */
    p = pop_path(root);

/* remove bin dir */
    p = pop_path(root);

/* setup the architecture string */
    p = getenv("PACT_CONFIG");
    if (p != NULL)
       {strncpy(sys, p, MAXLINE);
	sys[MAXLINE-1] = '\0';}

    else
       {strcpy(s, root);

	ps = pop_path(s);
	p  = pop_path(s);

/* if root is of the form /.../dev/<arch> use <arch> */
	if (strcmp(p, "dev") == 0)
	   strcpy(sys, ps);

/* otherwise use the configured system_id */
	else
	   strcpy(sys, SYSTEM_ID);};

    sprintf(arch, "z-%s", sys);

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
   {int i, status, ok, litrl, mc;
    long tmm;
    char mkfile[MAXLINE];
    char *pmname, *mv[100];
    static char *default_pmname = "pre-Make";
    struct stat st;

    signal(2, handler);
    memset(cwd, 0, MAXLINE);

    pmname = default_pmname;
    show   = 0;

    setup_env(v[0], arch, root);

    ok    = TRUE;
    litrl = FALSE;
    mc    = 1;
    for (i = 1; (i < c) && ok; i++)
        {if (strcmp(v[i], "-sys") == 0)
	    sprintf(arch, "%s", v[++i]);

         else if (strcmp(v[i], "-show") == 0)
	    show = 1;

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
	     ok = command_makefile(v[i], c, v, v+i+1);
	     return(ok);}

/* ignore dmake options taking the next argument */
	 else if ((strcmp(v[i], "-dst") == 0)   ||
		  (strcmp(v[i], "-flt") == 0)   ||
		  (strcmp(v[i], "-cmd") == 0))
            {printf("smake: option %s ignored\n", v[i]);
	     i++;}

	 else if (strcmp(v[i], "-incpath") == 0)
	    {report_cl(v[i]);
	     return(0);}

	 else if (strcmp(v[i], "-link") == 0)
	    {report_cl(v[i]);
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
                      snprintf(mkfile, MAXLINE, "%s/Makefile", arch);
                      status = build_makefile(mkfile);
		      return(status);
		      break;
		 case 'h' :
		      usage();
		      return(1);
		      break;
		 case 'i' :
		      if (strcmp(v[i], "-info") == 0)
			 {if (++i < c)
                             report(v[i], FALSE, litrl, TRUE);
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
		      printf("%s\n", PACT_VERSION);
		      exit(0);
		      break;};}
	 else if (v[i][0] == '+')
            {switch (v[i][1])
                {case 'i' :
		      if (strcmp(v[i], "+info") == 0)
			 {if (++i < c)
                             report(v[i], TRUE, litrl, TRUE);
			  return(0);};
		      break;
		 case 'l' :
		      litrl = TRUE;
		      break;};}
	 else
	    {mv[mc++] = v[i];
	     ok = FALSE;};};

    for (; i < c; i++)
        mv[mc++] = v[i];

    mv[mc] = NULL;
    mv[0]  = v[0];

    snprintf(mkfile, MAXLINE, "%s/Makefile", arch);
    if (stat(mkfile, &st) == 0)
       {if (S_ISREG(st.st_mode))
	   {tmm    = (long) st.st_mtime;
	    status = method_2(mc, mv, mkfile, tmm);
	    return((status == 0) ? 0 : 1);};};

    status = method_1(mc, mv, pmname);

    return((status == 0) ? 0 : 1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
