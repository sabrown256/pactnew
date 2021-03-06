/*
 * LIBINFO.C - report info about configured PACT
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * include "cpyright.h"
 *
 */

#ifndef LIBINFO

# define LIBINFO

/*--------------------------------------------------------------------------*/

# ifndef SCOPE_SCORE_COMPILE

#undef INCL
#undef LINK
#undef REGEX
#undef VERS

enum e_itarget
   {INCL, LINK, REGEX, VERS};

typedef enum e_itarget itarget;

typedef struct s_infodes infodes;

struct s_infodes
   {int complete;
    int literal;
    int db_only;
    char root[BFLRG];};

# endif

/*--------------------------------------------------------------------------*/

# ifndef SCOPE_SCORE_PREPROC

int system();
char *getenv();

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REPORT_VAR - report on the variable Q in FILE */

static int report_var(infodes *st, char *dir, char *fname,
		      char *q, char *key, int newl, int db)
   {int i, nc, ok, doit, tst;
    int compl, litrl, quote;
    char file[BFLRG];
    char *dlm, *p, *var, *val, *ps, **sa;

    ok = FALSE;

    compl = st->complete;
    litrl = st->literal;
    quote = FALSE;

    snprintf(file, BFLRG, "%s/%s/%s", st->root, dir, fname);

    dlm = (db == TRUE) ? "=\n" : " \t\n";

    sa = file_text(FALSE, file);
    if (sa != NULL)
       {for (i = 0; sa[i] != NULL; i++)
	    {p = sa[i];
	     if (key != NULL)
	        {var  = strtok(p, " \t\r");
		 doit = ((var != NULL) && (strcmp(var, key) == 0));
		 ps   = NULL;}
	     else
	        {doit = TRUE;
		 ps   = p;};

	     if (doit)
	        {var = strtok(ps, dlm);
		 if (var == NULL)
		    continue;

		 tst = (litrl) ? (strcmp(var, q) == 0) :
		                 (strncmp(var, q, strlen(q)) == 0);

		 if (tst)
		    {val = strtok(NULL, "\n");

/* with env-pact.sh you WILL get here with val NULL and var <var>=<val>
 * or in scconfig.h with val NULL and var <var>
 */
		     if (val == NULL)
		        {p = strchr(var, '=');
			 if (p != NULL)
			    {*p++ = '\0';
			     val  = p;}
			 else
			    {if (!compl)
			        continue;
			     else
			        val = "";};}

/* with env-pact.csh you WILL get here with val <val> and var <var> */
		     else
		        {while (*val != '\0')
			    {if (strchr("= \t", *val) == NULL)
			        break;
			     else
			        val++;};};

		     if (quote == FALSE)
		        {nc = strlen(val);
			 if (nc >= 2)
			    val = subst(val, "\"", "", -1);};

		     if (compl)
		        printf("%s = %s", var, val);
		     else
		        printf("%s", val);

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

static void report(infodes *st, char *q, int newl)
   {int ok, dbo;
    char *s;

    ok  = FALSE;
    dbo = st->db_only;

    if (dbo == FALSE)
       {if (strcmp(q, "make") == 0)
	   ok = report_var(st, "etc", "make-def", "UMake",
			   NULL, newl, FALSE);

	if (strcmp(q, "config") == 0)
	   ok = report_var(st, "etc", "make-def", "PSY_ID",
			   NULL, newl, FALSE);

	if (!ok)
	   ok = report_var(st, "include", "scconfig.h", q,
			   "#define", newl, FALSE);

	if (!ok)
	   ok = report_var(st, "etc", "make-def", q,
			   NULL, newl, FALSE);

	if (!ok)
	   ok = report_var(st, "etc", "configured", q,
			   NULL, newl, FALSE);

	if (!ok)
	   {s = getenv("SHELL");
	    if ((s != NULL) && (strstr(s, "csh") != NULL))
	       ok = report_var(st, "etc", "env-pact.csh",
			       q, "setenv", newl, FALSE);
	    else
	       ok = report_var(st, "etc", "env-pact.sh",
			       q, "export", newl, FALSE);};};

    if (!ok)
       ok = report_var(st, "etc", "cfg.db", q,
		       NULL, newl, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REPORT_CL - report the information relevant to compiling and linking */

static void report_cl(infodes *st, char *q)
   {

    st->literal = TRUE;

    if (strcmp(q, "-incpath") == 0)
       {report(st, "MDGInc", FALSE);
        report(st, "MDInc", TRUE);}

    else if (strcmp(q, "-link") == 0)
       {report(st, "LDRPath", FALSE);
        report(st, "LDPath", FALSE);
	report(st, "DP_Lib", FALSE);
	report(st, "MDGLib", FALSE);
	report(st, "MDLib",  TRUE);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REPORT_INFO - function call API for pact-info */

int report_info(char *root, int cmpl, int ltrl, int dbo,
		itarget tgt, char *ptrn)
   {int ok;
    infodes st;

    st.complete = cmpl;
    st.literal  = ltrl;
    st.db_only  = dbo;
    nstrncpy(st.root, BFLRG, root, -1);

    ok = 0;
    switch (tgt)
       {case LINK :
	     report_cl(&st, "-link");
             break;
	case INCL :
	     report_cl(&st, "-incpath");
             break;
	case VERS :
	     printf("%s\n", PACT_VERSION);
             break;
	case REGEX :
	     report(&st, ptrn, TRUE);
             break;};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BUILD_MAKEFILE - make the specified Makefile from the pre-Make
 *                - in the current directory
 *                - return 0 iff successful
 */

int build_makefile(char *root, char *arch, char *mkfile, int vrb)
   {int err;
    char cmd[BFLRG], makef[BFLRG], etc[BFLRG], d[BFLRG];

    err = 0;

    snprintf(etc, BFLRG, "%s/etc", root);
    snprintf(makef, BFLRG, "%s", mkfile);
    getcwd(d, BFLRG);

    snprintf(cmd, BFLRG, "csh -cf \"mkdir -p %s/obj >& /dev/null\"", arch);
    system(cmd);

    if (vrb == TRUE)
       printf("\nMaking %s from pre-Make\n\n", makef);

    snprintf(cmd, BFLRG, "cp %s/make-def %s", etc, makef);
    err |= system(cmd);

    snprintf(cmd, BFLRG, "echo PACTTmpDir = %s/obj >> %s", arch, makef);
    err |= system(cmd);

    snprintf(cmd, BFLRG, "echo PACTSrcDir = %s >> %s", d, makef);
    err |= system(cmd);

    snprintf(cmd, BFLRG, "cat pre-Make >> %s", makef);
    err |= system(cmd);

    snprintf(cmd, BFLRG, "cat %s/make-macros >> %s", etc, makef);
    err |= system(cmd);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

# endif
#endif
