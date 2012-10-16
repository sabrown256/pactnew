/*
 * PCSH - relaunch CSH session with knowledge of PACT
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#include "common.h"
#include "libpsh.c"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCRIPT_ENV - write the environment setup part of a script to FO */

static void script_env(FILE *fo, char *pact)
   {int haverc;
    char *p;

    haverc = cdefenv("PCSHRC");

/* check for a pcshrc file to include before PACT environment */
    if (haverc == TRUE)
       {p = cgetenv(TRUE, "PCSHRC");
	if (file_exists(p) == TRUE)
	   fprintf(fo, "source %s\n", p);};

    fprintf(fo, "source %s/etc/env-pact.csh\n", pact);
    fprintf(fo, "source %s/etc/functions-pact.csh\n", pact);
    fprintf(fo, "unsetenv CROSS\n");

/* check for a pcshrc file to include after PACT environment */
    if (haverc == TRUE)
       {if (file_exists("%s.post", p) == TRUE)
	   fprintf(fo, "source %s.post\n", p);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_SHELL_SCRIPT - prepare a shell script for invocation */

static int make_shell_script(FILE *fi, char *fname, char *shell, char *pact,
			     char *args, int henv,
			     char **vo, char **v, int k)
   {int i, co;
    char s[LRG];
    FILE *fo;

/* initialize the repackaged version */
    fo = fopen(fname, "w");
    if (fo != NULL)
       {fprintf(fo, "#!%s %s\n", shell, args);
	fprintf(fo, "setenv PCSH_TMP_ %s\n", fname);

	script_env(fo, pact);

	fprintf(fo, "source %s/etc/csh-subroutines\n", pact);
	fprintf(fo, "HAVE_SUBROUTINES\n");
	fprintf(fo, "\n");

/* add the remainder of the shell script */
	for (i = 0; fgets(s, MAXLINE, fi) != NULL; i++)
	    fputs(s, fo);

/* finish up the file */
	fclose(fo);

	run(FALSE, "mv %s %s.tmp ; sed 's/[ \t]*exit(/ quit(/g' %s.tmp | sed 's/[ \t]*exit[ \t]/ quit /g' > %s ; rm %s.tmp",
	    fname, fname, fname, fname, fname);

	chmod(fname, S_IRUSR | S_IWUSR | S_IXUSR);};

/* make the new command line to exec */
    co = 0;
    vo[co++] = shell;

/* get any command line args destined for the shell */
    for (i = 1; i < k; i++)
        vo[co++] = v[i];

/* get any script args destined for the shell */
    if (henv == TRUE)
       {FOREACH(arg, args, " ")
	   vo[co++] = STRSAVE(arg);
	ENDFOR;};

    return(co);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_C_SCRIPT - write and compile a C program for invocation */

static int make_c_script(FILE *fi, char *fname, char **v)
   {int i, co;
    char s[LRG], cname[MAXLINE], exe[MAXLINE];
    char bindir[MAXLINE], incdir[MAXLINE];
    FILE *fo;

    nstrncpy(exe, MAXLINE, cwhich(v[0]), -1);
    nstrncpy(bindir, MAXLINE, path_head(exe), -1);
    nstrncpy(s, LRG, bindir, -1);
    snprintf(incdir, MAXLINE, "%s/include", path_head(s));
    snprintf(cname, MAXLINE, "%s.c", fname);

/* initialize the repackaged version */
    fo = fopen(cname, "w");

/* copy the remainder as the C program */
    if (fo != NULL)
       {for (i = 0; fgets(s, MAXLINE, fi) != NULL; i++)
	    fputs(s, fo);

/* finish up the file */
	fclose(fo);};

    execute(TRUE, "cc -g -I%s %s -o %s -lc -lm", incdir, cname, fname);
    unlink(cname);

/* make the new command line to exec */
    co = 0;

    return(co);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INVOKE_SCRIPT - prepare a shell script invocation
 *               - return the arglist in VO
 */

static void invoke_script(char **vo, char *shell, char *pact,
			  char **v, int k, int c)
   {int i, co, henv;
    off_t ad;
    char fname[MAXLINE], s[LRG], args[MAXLINE];
    char *sname, *p;
    FILE *fi;

    sname = v[k];

    snprintf(fname, MAXLINE, "/tmp/%s.%d",
	     path_tail(sname), (int) getpid());

    co = 0;

/* read the first line of the shell script */
    fi = fopen(sname, "r");
    if (fi != NULL)
       {fgets(s, MAXLINE, fi);

	p = strtok(s, " ");

/* look for #!/usr/bin/env as a special case */
	if ((p != NULL) && (strstr(p, "bin/env") != NULL))
	   {henv = TRUE;
	    ad   = ftell(fi);
	    fgets(s, MAXLINE, fi);
	    if (strncmp(s, "#OPT", 4) == 0)
	       p = strtok(s + 4, "\n");
	    else
	       {fseek(fi, ad, SEEK_SET);
		p = "";};}
	else
	   {henv = FALSE;
	    p    = strtok(NULL, "\n");};

	nstrncpy(args, MAXLINE, p, -1);

	p = strstr(args, "-lang");
	if (p == NULL)
	   co = make_shell_script(fi, fname, shell, pact, args, henv, vo, v, k);
	else if (strcmp(p+6, "c") == 0)
	   co = make_c_script(fi, fname, v);

	fclose(fi);};

    vo[co++] = STRSAVE(fname);

/* get any command line args destined for the script */
    for (i = k+1; i < c; i++)
        vo[co++] = v[i];

    vo[co] = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INVOKE_COMMAND - prepare to invoke a shell to do a single command
 *                - return the arglist in VO
 */

static void invoke_command(char **vo, char *shell, char *pact,
			   char **v, int k, int c)
   {int i, co, haverc;
    char s[LRG], t[MAXLINE], u[MAXLINE];
    char *p;

    haverc = cdefenv("PCSHRC");

    co = 0;
    vo[co++] = shell;

/* get in everything before the command */
    for (i = 1; i < k; i++)
        vo[co++] = v[i];

    s[0] = '\0';

    nstrcat(s, LRG, "( ");

    if (cdefenv("PCSHRC") == TRUE)
       {snprintf(t, MAXLINE, "source %s ", cgetenv(TRUE, "PCSHRC"));
	nstrcat(s, LRG, t);
	nstrcat(s, LRG, "; ");};

/* check for a pcshrc file to include before PACT environment */
    if (haverc == TRUE)
       {p = cgetenv(TRUE, "PCSHRC");
	if (file_exists(p) == TRUE)
	   {snprintf(t, MAXLINE, "source %s ; ", p);
	    nstrcat(s, LRG, t);};};

    snprintf(u, MAXLINE, "source %s/etc/env-pact.csh ; ", pact);
    nstrcat(s, LRG, u);
    snprintf(u, MAXLINE, "source %s/etc/functions-pact.csh ; ", pact);
    nstrcat(s, LRG, u);
    snprintf(u, MAXLINE, "unsetenv CROSS ; ");
    nstrcat(s, LRG, u);

/* check for a pcshrc file to include before PACT environment */
    if (haverc == TRUE)
       {if (file_exists("%s.post", p) == TRUE)
	   {snprintf(t, MAXLINE, "source %s.post ; ", p);
	    nstrcat(s, LRG, t);};};

    nstrcat(s, LRG, v[k]);
    nstrcat(s, LRG, " )");

    vo[co++] = STRSAVE(s);
    vo[co]   = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INVOKE_SESSION - prepare to invoke an interactive shell session
 *                - return the arglist in VO
 */

static void invoke_session(char **vo, char *shell, char *pact,
			   char **v, int c)
   {int i, co, nodot;
    char home[MAXLINE], user[MAXLINE], cshrc[MAXLINE];
    char dname[MAXLINE], fname[MAXLINE];
    char *p;
    FILE *fo;

/* make the proper command line */
    co = 0;

    nodot    = FALSE;
    vo[co++] = shell;
    for (i = 1; i <= c; i++)
        {p = v[i];
	 if ((p != NULL) && (strcmp(p, "-f") == 0))
	    nodot = TRUE;
	 else
	    vo[co++] = p;};

    nstrncpy(home,  MAXLINE, cgetenv(TRUE, "HOME"), -1);
    nstrncpy(user,  MAXLINE, cgetenv(FALSE, "USER"), -1);
    snprintf(cshrc, MAXLINE, "%s/.cshrc", home);

/* make a temporary home directory */
    snprintf(dname, MAXLINE, "/tmp/%s.%d", user, (int) getpid());
    run(FALSE, "mkdir -p %s", dname);

/* write a temporary .cshrc */
    snprintf(fname, MAXLINE, "%s/.cshrc", dname);
    fo = fopen(fname, "w");
    if (fo != NULL)
       {fprintf(fo, "setenv HOME %s\n", home);
	fprintf(fo, "cd %s\n", home);
	fprintf(fo, "\n");

	if (nodot == FALSE)
	   {fprintf(fo, "if (-f %s) then\n", cshrc);
	    fprintf(fo, "   source %s\n", cshrc);
	    fprintf(fo, "endif\n");
	    fprintf(fo, "\n");};

	script_env(fo, pact);
	fprintf(fo, "\n");

	fprintf(fo, "rm -rf %s\n", dname);

	fclose(fo);};

/* set the HOME variable to reference the temporary .cshrc */
    if (file_exists(fname) == TRUE)
       csetenv("HOME", dname);

    return;}
	
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIX_ENV - augment the PATH to find PACT utilities
 *         - and add PACT variable
 */

static void fix_env(char *prog)
   {char exe[LRG], s[LRG];

/* add the path to here iff you are able to verify it */
    full_path(exe, LRG, NULL, path_head(prog));
    csetenv("PATH", "%s:%s", exe, cgetenv(TRUE, "PATH"));

    full_path(exe, LRG, NULL, cwhich("pact"));
    if (file_executable(exe) == TRUE)
       {nstrncpy(s, LRG, path_head(exe), -1);
        nstrncpy(s, LRG, path_head(s), -1);

	csetenv("PACT", s);
        csetenv("PATH", "%s/bin:%s", s, cgetenv(TRUE, "PATH"));};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it out here */

int main(int c, char **v)
   {int i, j, k, ok, done;
    char shell[MAXLINE], pact[MAXLINE], s[MAXLINE];
    char *scr, *vo[1024];
    static int verbose = FALSE;

    setbuf(stdout, NULL);

    nstrncpy(shell, MAXLINE, "/bin/csh", -1);
    nstrncpy(pact, MAXLINE, path_head(path_head(cwhich(v[0]))), -1);

/* locate the tools needed for subshells */
    build_path(NULL,
	       "mv", "sed", "rm", "cc",
	       NULL);

/* print the entire command line */
    if (verbose == TRUE)
       {printf("Command line:");
	for (i = 0; i < c; i++)
	    printf(" %s", v[i]);
	printf("\n");};

/* parse the command line arguments */
    k   = 0;
    scr = v[0];
    for (done = FALSE, i = 1; (i < c) && (done == FALSE); i++)
        {if (v[i][0] == '-')
	    {for (ok = TRUE, j = 1; ok == TRUE; j++)
	         {switch (v[i][j])
		     {case '\0':
		           ok = FALSE;
			   break;

                      case 'e':
                      case 'f':
                      case 'i':
                      case 'n':
                      case 'q':
                      case 's':
                      case 't':
                      case 'v':
                      case 'V':
                      case 'x':
                      case 'X':
                      case 'l':
		      default :
			   break;

		      case 'b':
			   k = i + 1;
		           done = TRUE;
			   break;
                      case 'c':
			   k = -i - 1;
		           done = TRUE;
			   break;};};}
         else
	    {k    = i;
	     done = TRUE;};};

/* rework shell script invocation */
    if (k > 0)
       {full_path(s, MAXLINE, NULL, v[k]);
	scr = s;
	invoke_script(vo, shell, pact, v, k, c);}

    else if (k < 0)
       invoke_command(vo, shell, pact, v, -k, c);

/* rework interactive shell invocation */
    else
       invoke_session(vo, shell, pact, v, c);

    fix_env(scr);

/* print the command we are about to exec */
    if (verbose == TRUE)
       {printf("Exec (%s):", shell);
	for (i = 0; vo[i] != NULL; i++)
	    printf(" %s", vo[i]);
	printf("\n");};

    execvp(vo[0], vo);

    return(1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
