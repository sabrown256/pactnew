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

#define LIBINFO

#undef MAXLINE
#define MAXLINE 4096

#undef INC
#undef LINK
#undef REGEX
#undef VERSION

enum e_itarget
   {INC, LINK, REGEX, VERSION};

typedef enum e_itarget itarget;

typedef struct s_infodes infodes;

struct s_infodes
   {int complete;
    int literal;
    char root[MAXLINE];};

int system();
char *getenv();

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REPORT_VAR - report on the variable Q in FILE */

static int report_var(infodes *st, char *dir, char *fname,
		      char *q, char *key, int newl, int db)
   {int i, nc, ok, doit, tst;
    int compl, litrl, quote;
    char file[MAXLINE];
    char *dlm, *p, *tok, *txt, *ps, **sa;

    ok = FALSE;

    compl = st->complete;
    litrl = st->literal;
    quote = FALSE;

    snprintf(file, MAXLINE, "%s/%s/%s", st->root, dir, fname);

    dlm = (db == TRUE) ? "=\n" : " \t\n";

    sa = file_text(FALSE, file);
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
	        {tok = strtok(ps, dlm);
		 if (tok == NULL)
		    continue;

		 tst = (litrl) ? (strcmp(tok, q) == 0) :
		                 (strncmp(tok, q, strlen(q)) == 0);

		 if (tst)
		    {txt = strtok(NULL, "\n");

/* with env-pact.sh you WILL get here with txt NULL and tok <var>=<val>
 * or in scconfig.h with txt NULL and tok <var>
 */
		     if (txt == NULL)
		        {p = strchr(tok, '=');
			 if (p != NULL)
			    {*p++ = '\0';
			     txt  = p;}
			 else
			    txt = tok;}

/* with env-pact.csh you WILL get here with txt <val> and tok <var> */
		     else
		        {while (*txt != '\0')
			    {if (strchr("= \t", *txt) == NULL)
			        break;
			     else
			        txt++;};};

		     if (quote == FALSE)
		        {nc = strlen(txt);
			 if (nc >= 2)
			    txt = subst(txt, "\"", "", -1);};

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

static void report(infodes *st, char *q, int newl)
   {int ok;
    char *s;

    ok = FALSE;

    if (strcmp(q, "make") == 0)
       ok = report_var(st, "etc", "make-def", "UMake",
		       NULL, newl, FALSE);

    if (strcmp(q, "config") == 0)
       ok = report_var(st, "etc", "make-def", "System",
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
			   q, "export", newl, FALSE);};

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

int report_info(char *root, int cmpl, int ltrl, itarget tgt, char *ptrn)
   {int ok;
    infodes st;

    st.complete = cmpl;
    st.literal  = ltrl;
    nstrncpy(st.root, MAXLINE, root, -1);

    ok = 0;
    switch (tgt)
       {case LINK :
	     report_cl(&st, "-link");
             break;
	case INC :
	     report_cl(&st, "-incpath");
             break;
	case VERSION :
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
    char cmd[MAXLINE], makef[MAXLINE], etc[MAXLINE];

    err = 0;

    snprintf(etc, MAXLINE, "%s/etc", root);
    snprintf(makef, MAXLINE, "%s", mkfile);

    snprintf(cmd, MAXLINE, "csh -cf \"mkdir -p %s/obj >& /dev/null\"", arch);
    system(cmd);

    if (vrb == TRUE)
       printf("\nMaking %s from pre-Make\n\n", makef);

    snprintf(cmd, MAXLINE, "cp %s/make-def %s", etc, makef);
    err |= system(cmd);

    snprintf(cmd, MAXLINE, "echo PACTTmpDir = %s/obj >> %s", arch, makef);
    err |= system(cmd);

    snprintf(cmd, MAXLINE, "echo PACTSrcDir = ../.. >> %s", makef);
    err |= system(cmd);

    snprintf(cmd, MAXLINE, "cat pre-Make >> %s", makef);
    err |= system(cmd);

    snprintf(cmd, MAXLINE, "cat %s/make-macros >> %s", etc, makef);
    err |= system(cmd);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif