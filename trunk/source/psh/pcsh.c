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

/* HAS_PAR_CONST - return the line number in SA of the first parallel
 *               - construct indicated by TOK
 *               - return -1 if there are none or there is not a matching PEND
 */

static int has_par_const(char **sa, char *tok)
   {int i, ie, is, nc, ne, ns;
    char *t;

    ns = 0;
    ne = 0;
    is = -1;
    ie = -1;

    if ((sa != NULL) && (tok != NULL))
       {nc = strlen(tok);

	for (i = 0; sa[i] != NULL; i++)
	    {t = trim(sa[i], FRONT, " \t");
	     if (strncmp(t, tok, nc) == 0)
	        {ns++;
		 is = i;};
	     if (strncmp(t, "pend", 4) == 0)
	        {ne++;
		 ie = i;};};};
            
    ASSERT(ie != -1);

    is = ((ns == 1) && (ns == ne)) ? is : -1;

    return(is);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_SHELL_SCRIPT - prepare a shell script for invocation */

static int make_shell_script(char **sa, char *fname, char *shell, char *pact,
			     char *args, int henv,
			     char **vo, char **v, int k)
   {int i, co;
    FILE *fo;

/* initialize the repackaged version */
    fo = fopen_safe(fname, "w");
    if (fo != NULL)
       {char lpth[BFLRG];

/* use full path for PCSH_TMP_ variable */
	full_path(lpth, BFLRG, 0, NULL, fname);

	fprintf(fo, "#!%s %s\n", shell, args);
	fprintf(fo, "setenv PCSH_TMP_ %s\n", lpth);

	script_env(fo, pact);

	fprintf(fo, "source %s/etc/csh-subroutines\n", pact);
	fprintf(fo, "HAVE_SUBROUTINES\n");
	fprintf(fo, "\n");

/* add the remainder of the shell script */
        strings_out(fo, sa, 0, -1, TRUE);

/* finish up the file */
	fclose_safe(fo);

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

/* MAKE_CTL_SCRIPT - write the parallel controller script */

void make_ctl_script(char *fname, char *dname, char *lname,
		     char *cmd, char *al, int *lm)
   {FILE *fp;

    fp = fopen_safe(fname, "w");

    fprintf(fp, "#!/bin/csh -f\n");
    fprintf(fp, "unalias *\n");
    fprintf(fp, "@ err = 0\n");
    fprintf(fp, "setenv SEMEX %s/sem.%s\n", dname, lname);
    fprintf(fp, "set log = %s/log.%s\n", dname, lname);
    fprintf(fp, "set cmd = ( %s )\n", cmd);
    fprintf(fp, "set arg = ( %s )\n", al);
    fprintf(fp, "\n");

    fprintf(fp, "rm -f $log\n");
    fprintf(fp, "touch $log\n");
    fprintf(fp, "echo \"Host:  `uname -n`\" >>& $log\n");
    fprintf(fp, "echo \"PID:   $$\" >>& $log\n");
    fprintf(fp, "echo \"CWD:   $cwd\" >>& $log\n");

    fprintf(fp, "if ($?PCSH_NPROC == 1) then\n");
    fprintf(fp, "   @ nn = $PCSH_NPROC\n");
    fprintf(fp, "   @ dt = 10\n");
    fprintf(fp, "else\n");
    fprintf(fp, "   @ nn = 1\n");
    fprintf(fp, "   @ dt = 0\n");
    fprintf(fp, "endif\n");
    fprintf(fp, "echo \"NPROC: $nn\" >>& $log\n");
    fprintf(fp, "\n");

    fprintf(fp, "@ ip = %d\n", lm[0]);
    fprintf(fp, "@ mx = %d\n", lm[1]);
    fprintf(fp, "@ dm = %d\n", lm[2]);
    fprintf(fp, "while ($ip < $mx)\n");
    fprintf(fp, "   while (1)\n");
    fprintf(fp, "      sleep $dt\n");
    fprintf(fp, "      @ ns = `ls -1 %s | grep $SEMEX:t | wc -l`\n", dname);
    fprintf(fp, "      if ($ns < $nn) then\n");
    fprintf(fp, "         $cmd %s.$ip $arg >>& $log\n", fname);
    fprintf(fp, "         @ err = $err + $status\n");
    fprintf(fp, "         @ ip  = $ip + $dm\n");
    fprintf(fp, "         break\n");
    fprintf(fp, "      endif\n");
    fprintf(fp, "   end\n");
    fprintf(fp, "end\n");
    fprintf(fp, "\n");

    fprintf(fp, "while (1)\n");
    fprintf(fp, "   sleep $dt\n");
    fprintf(fp, "   @ ns = `ls -1 %s | grep $SEMEX:t | wc -l`\n", dname);
    fprintf(fp, "   if ($ns == 0) then\n");
    fprintf(fp, "      echo ' done' >>& $log\n");
    fprintf(fp, "      break\n");
    fprintf(fp, "   endif\n");
    fprintf(fp, "end\n");
    fprintf(fp, "\n");

    fprintf(fp, "if ($err == 0) then\n");
    fprintf(fp, "   unlink %s\n", fname);
    fprintf(fp, "endif\n");
    fprintf(fp, "\n");

/* close the master file */
    fprintf(fp, "exit($err)\n");
    fclose_safe(fp);

    chmod(fname, S_IRUSR | S_IWUSR | S_IXUSR);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPUTE_PDO_LIMITS - from SA[IS] compute the skeleton for the
 *                    - shell line in a PDO construct
 */

static char **compute_pdo_limits(char **sa, int is, char *al, int nc)
   {int n;
    char fn[BFLRG], t[BFLRG];
    char *p, *r, **rv, **ra, **ta;
    FILE *fp;

    rv = NULL;

    snprintf(fn, BFLRG, "/tmp/pcsh-pdo.%d", getpid());    
    fp = fopen_safe(fn, "w");

/* write the prelimary part of the script */
    strings_out(fp, sa, 0, is, TRUE);

/* parse SA[IS] and write the conclusion of the temporary script */
    nstrncpy(t, BFLRG, sa[is], -1);
    ta = tokenize(t, "[]", 0);

    nstrncpy(t, BFLRG, ta[0], -1);

/* expected tokens are: pdo, <var>, <mn>, <mx>, and <dm> */
    ra = tokenize(t, "=, \t", 0);
    n  = lst_length(ra);
    fprintf(fp, "    echo \"var %s ; \"\n", ra[1]);
    fprintf(fp, "    echo \"mn  %s ; \"\n", ra[2]);
    fprintf(fp, "    echo \"mx  %s ; \"\n", ra[3]);
    if (n < 5)
       fprintf(fp, "    echo \"dm  1 ; \"\n");
    else
       fprintf(fp, "    echo \"dm  %s ; \"\n", ra[4]);

    fprintf(fp, "    echo \"_cmd_ %s\"\n", ta[1]);

    free_strings(ta);
    free_strings(ra);

/* close the file */
    fprintf(fp, "    exit(131)\n");
    fclose_safe(fp);

/* run the script */
    r = run(FALSE, "pcsh %s %s | tail -n 5", fn, al);
    p = strstr(r, "_cmd_");
    rv = lst_add(rv, p + 6);

    unlink(fn);

    ta = tokenize(r, " ;\n", 0);
    rv = lst_add(rv, ta[1]);
    rv = lst_add(rv, ta[3]);
    rv = lst_add(rv, ta[5]);
    rv = lst_add(rv, ta[7]);

    free_strings(ta);

    rv = lst_add(rv, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_PDO_SCRIPT - prepare a shell script for parallel DO invocation */

static int make_pdo_script(char **sa, int is, char *fname, int nc,
			   char *shell, char *pact, char *args, int henv,
			   char **vo, char **v, int k)
   {int i, ip, co, n;
    int lm[3];
    char al[BFLRG], r[BFLRG], fn[BFLRG];
    char dname[BFLRG], lname[BFLRG];
    char *ro, **ta;

    memset(dname, 0, BFLRG);
    memset(lname, 0, BFLRG);

    snprintf(dname, BFLRG, "tmp-%d", (int) getpid());
    nstrncpy(lname, BFLRG, path_base(path_tail(fname)), -1);
    snprintf(fname, nc, "%s/%s", dname, lname);

    mkdir(dname, 0770);

    concatenate(al, BFLRG, v, k+1, -1, " ");

    ta = compute_pdo_limits(sa, is, al, BFLRG);
    n  = lst_length(ta);
    lm[0] = (n > 1) ? atoi(ta[2]) : 0;
    lm[1] = (n > 2) ? atoi(ta[3]) : -1;
    lm[2] = (n > 3) ? atoi(ta[4]) : 1;

    ro = sa[is];
    sa[is] = r;

/* blank out the pend line */
    for (i = is; sa[is] != NULL; i++)
        {if (strncmp(trim(sa[i], FRONT, " \t"), "pend", 4) == 0)
	    {sa[i][0] = '\0';
	     break;};};

/* write one script for each loop iteration */
    memset(fn, 0, BFLRG);
    memset(r, 0, BFLRG);
    for (ip = lm[0]; ip < lm[1]; ip += lm[2])
        {snprintf(fn, BFLRG, "%s.%d", fname, ip);
	 snprintf(r, BFLRG, "   set %s = %d\n", ta[1], ip);
	 co = make_shell_script(sa, fn, shell, pact,
				args, henv, vo, v, k);};

    sa[is] = ro;

/* write a controller script to run them all */
    make_ctl_script(fname, dname, lname, ta[0], al, lm);

/* clean up */
    free_strings(ta);

    return(co);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPUTE_PFOR_LIMITS - from SA[IS] compute the skeleton for the
 *                     - shell line in a PFOR construct
 */

static char **compute_pfor_limits(char **sa, int is, char *al, int nc)
   {int i, n;
    char fn[BFLRG], t[BFLRG];
    char *p, *r, **rv, **ra, **ta;
    FILE *fp;

    rv = NULL;

    snprintf(fn, BFLRG, "/tmp/pcsh-pfor.%d", getpid());    
    fp = fopen_safe(fn, "w");

/* write the prelimary part of the script */
    strings_out(fp, sa, 0, is, TRUE);

/* parse SA[IS] and write the conclusion of the temporary script */
    nstrncpy(t, BFLRG, sa[is], -1);
    ta = tokenize(t, "[]", 0);

    nstrncpy(t, BFLRG, ta[0], -1);

/* expected tokens are: pfor, <var>, <lst> */
    ra = tokenize(t, "()", 0);
    n  = lst_length(ra);
    fprintf(fp, "    echo \"var %s ; \"\n", trim(ra[0]+4, BOTH, " \t"));
    fprintf(fp, "    echo \"lst %s ; \"\n", ra[1]);
    fprintf(fp, "    echo \"_cmd_ %s\"\n", ta[1]);

    free_strings(ta);
    free_strings(ra);

/* close the file */
    fprintf(fp, "    exit(131)\n");
    fclose_safe(fp);

/* run the script */
    r = run(FALSE, "pcsh %s %s | tail -n 5", fn, al);
    p = strstr(r, "_cmd_");
    *p = '\0';
    rv = lst_add(rv, p + 6);

    unlink(fn);

    ta = tokenize(r, " ;\n", 0);
    n  = lst_length(ta);
    rv = lst_add(rv, ta[1]);

    for (i = 3; i < n; i++)
        rv = lst_add(rv, ta[i]);

    free_strings(ta);

    rv = lst_add(rv, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_PFOR_SCRIPT - prepare a shell script for parallel FOR invocation */

static int make_pfor_script(char **sa, int is, char *fname, int nc,
			    char *shell, char *pact, char *args, int henv,
			    char **vo, char **v, int k)
   {int i, ip, co, n;
    int lm[3];
    char al[BFLRG], r[BFLRG], fn[BFLRG];
    char dname[BFLRG], lname[BFLRG];
    char *ro, **ta;

    memset(dname, 0, BFLRG);
    memset(lname, 0, BFLRG);

    snprintf(dname, BFLRG, "tmp-%d", (int) getpid());
    nstrncpy(lname, BFLRG, path_base(path_tail(fname)), -1);
    snprintf(fname, nc, "%s/%s", dname, lname);

    mkdir(dname, 0770);

    concatenate(al, BFLRG, v, k+1, -1, " ");

    ta = compute_pfor_limits(sa, is, al, BFLRG);
    n  = lst_length(ta);

    ro = sa[is];
    sa[is] = r;

/* blank out the pend line */
    for (i = is; sa[is] != NULL; i++)
        {if (strncmp(trim(sa[i], FRONT, " \t"), "pend", 4) == 0)
	    {sa[i][0] = '\0';
	     break;};};

/* write one script for each list element */
    memset(fn, 0, BFLRG);
    memset(r, 0, BFLRG);
    for (ip = 2; ip < n; ip++)
        {snprintf(fn, BFLRG, "%s.%d", fname, ip);
	 snprintf(r, BFLRG, "   set %s = %s\n", ta[1], ta[ip]);
	 co = make_shell_script(sa, fn, shell, pact,
				args, henv, vo, v, k);};

    sa[is] = ro;

    lm[0] = 2;
    lm[1] = n;
    lm[2] = 1;

/* write a controller script to run them all */
    make_ctl_script(fname, dname, lname, ta[0], al, lm);

/* clean up */
    free_strings(ta);

    return(co);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_C_SCRIPT - write and compile a C program for invocation */

static int make_c_script(char **sa, char *fname, char **v)
   {int co;
    char s[BFLRG], cname[BFLRG], exe[BFLRG];
    char bindir[BFLRG], incdir[BFLRG];
    FILE *fo;

    nstrncpy(exe, BFLRG, cwhich(v[0]), -1);
    nstrncpy(bindir, BFLRG, path_head(exe), -1);
    nstrncpy(s, BFLRG, bindir, -1);
    snprintf(incdir, BFLRG, "%s/include", path_head(s));
    snprintf(cname, BFLRG, "%s.c", fname);

/* initialize the repackaged version */
    fo = fopen_safe(cname, "w");

/* copy the remainder as the C program */
    if (fo != NULL)
       {strings_out(fo, sa, 0, -1, TRUE);

/* finish up the file */
	fclose_safe(fo);};

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
   {int i, is, co, henv;
    char fname[BFLRG], s[BFLRG], args[BFLRG];
    char *sname, *p, **sa;

    sname = v[k];

    snprintf(fname, BFLRG, "/tmp/g.%s.%d", path_tail(sname), (int) getpid());

    co = 0;

/* read the first line of the shell script */
    sa = file_text(FALSE, sname);
    if (sa != NULL)
       {nstrncpy(s, BFLRG, sa[0], -1);

	p = strtok(s, " ");

/* look for #!/usr/bin/env as a special case */
	if ((p != NULL) && (strstr(p, "bin/env") != NULL))
	   {henv = TRUE;
	    nstrncpy(s, BFLRG, sa[1], -1);
	    if (strncmp(s, "#OPT", 4) == 0)
               {nstrncpy(sa[1], 3, "#\n", -1);
		p = strtok(s + 4, "\n");}
	    else
	       p = "";}
	else
	   {henv = FALSE;
	    p    = strtok(NULL, "\n");};

	nstrncpy(args, BFLRG, p, -1);

	p = strstr(args, "-lang");
	if (p == NULL)
           {is = -1;

/* adjust PF to make the generated script in the current directory
 * for batch cases because the remote host may not see /tmp on
 * the current host
 */
	    if ((is = has_par_const(sa, "pdo")) >= 0)
	       co = make_pdo_script(sa, is, fname, BFLRG, shell, pact,
				     args, henv, vo, v, k);
		
	    else if ((is = has_par_const(sa, "pfor")) >= 0)
	       co = make_pfor_script(sa, is, fname, BFLRG, shell, pact,
				     args, henv, vo, v, k);
	    else
	       co = make_shell_script(sa, fname, shell, pact,
				      args, henv, vo, v, k);}

	else if (strcmp(p+6, "c") == 0)
	   co = make_c_script(sa+2, fname, v);

	free_strings(sa);};

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
    char s[BFLRG], t[BFLRG], u[BFLRG];
    char *p;

    haverc = cdefenv("PCSHRC");

    co = 0;
    vo[co++] = shell;

/* get in everything before the command */
    for (i = 1; i < k; i++)
        vo[co++] = v[i];

    s[0] = '\0';

    nstrcat(s, BFLRG, "( ");

    if (cdefenv("PCSHRC") == TRUE)
       {snprintf(t, BFLRG, "source %s ", cgetenv(TRUE, "PCSHRC"));
	nstrcat(s, BFLRG, t);
	nstrcat(s, BFLRG, "; ");};

/* check for a pcshrc file to include before PACT environment */
    if (haverc == TRUE)
       {p = cgetenv(TRUE, "PCSHRC");
	if (file_exists(p) == TRUE)
	   {snprintf(t, BFLRG, "source %s ; ", p);
	    nstrcat(s, BFLRG, t);};};

    snprintf(u, BFLRG, "source %s/etc/env-pact.csh ; ", pact);
    nstrcat(s, BFLRG, u);
    snprintf(u, BFLRG, "source %s/etc/functions-pact.csh ; ", pact);
    nstrcat(s, BFLRG, u);
    snprintf(u, BFLRG, "unsetenv CROSS ; ");
    nstrcat(s, BFLRG, u);

/* check for a pcshrc file to include before PACT environment */
    if (haverc == TRUE)
       {if (file_exists("%s.post", p) == TRUE)
	   {snprintf(t, BFLRG, "source %s.post ; ", p);
	    nstrcat(s, BFLRG, t);};};

    nstrcat(s, BFLRG, v[k]);
    nstrcat(s, BFLRG, " )");

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
    char home[BFLRG], user[BFLRG], cshrc[BFLRG];
    char dname[BFLRG], fname[BFLRG];
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

    nstrncpy(home,  BFLRG, cgetenv(TRUE, "HOME"), -1);
    nstrncpy(user,  BFLRG, cgetenv(FALSE, "USER"), -1);
    snprintf(cshrc, BFLRG, "%s/.cshrc", home);

/* make a temporary home directory */
    snprintf(dname, BFLRG, "/tmp/%s.%d", user, (int) getpid());
    run(FALSE, "mkdir -p %s", dname);

/* write a temporary .cshrc */
    snprintf(fname, BFLRG, "%s/.cshrc", dname);
    fo = fopen_safe(fname, "w");
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

	fclose_safe(fo);};

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
   {char exe[BFLRG], s[BFLRG];

/* add the path to here iff you are able to verify it */
    full_path(exe, BFLRG, 0, NULL, path_head(prog));
    csetenv("PATH", "%s:%s", exe, cgetenv(TRUE, "PATH"));

    full_path(exe, BFLRG, 0, NULL, cwhich("pact"));
    if (file_executable(exe) == TRUE)
       {nstrncpy(s, BFLRG, path_head(exe), -1);
        nstrncpy(s, BFLRG, path_head(s), -1);

	csetenv("PACT", s);
        csetenv("PATH", "%s/bin:%s", s, cgetenv(TRUE, "PATH"));};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it out here */

int main(int c, char **v)
   {int i, j, k, ok, done;
    char shell[BFLRG], pact[BFLRG], s[BFLRG];
    char *scr, *vo[1024];
    static int verbose = FALSE;

    setbuf(stdout, NULL);

    nstrncpy(shell, BFLRG, "/bin/csh", -1);

/* if v[1] indicates a different PACT version than
 * v[0] it could end up with an incompatible environment
 * for example: /usr/local/bin/pdbview and ~/pact/dev/bin/pcsh
 * so for consistency iff v[1] indicates a different PACT
 * than v[0] derive the base from v[1] instead of V[0]
 *
 * first cut see if there is a pcsh in the same directory as
 * the one V[1] is in
 * might need to check also for ../etc directory as well to
 * guard against a local utility named pcsh without the supporting
 * files
 */
    nstrncpy(pact, BFLRG, path_head(path_head(cwhich(v[1]))), -1);
    if ((file_exists("%s/bin/pcsh", pact) == FALSE) ||
	(dir_exists("%s/etc", pact) == FALSE))
       nstrncpy(pact, BFLRG, path_head(path_head(cwhich(v[0]))), -1);

/* locate the tools needed for subshells */
    build_path(NULL,
	       "mv", "sed", "rm", "cc", "pcsh",
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
       {full_path(s, BFLRG, 0, NULL, v[k]);
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

    log_safe("dump", 0, NULL, NULL);

    execvp(vo[0], vo);

    return(1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
