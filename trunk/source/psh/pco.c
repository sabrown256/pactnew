/*
 * PCO.C - processor for auto-configuration information
 *       - this program is like the PACT-CONFIG script but adds the RUN command
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * include "cpyright.h"
 *
 */

#include "common.h"
#include "libpsh.c"
#include "libeval.c"
#include "libdb.c"

#define STACK_FILE      1
#define STACK_PROCESS   2
#define STACK_TOOL      3
#define STACK_GROUP     4

#define PHASE_READ      10
#define PHASE_ANALYZE   11
#define PHASE_WRITE     12

#define LOG_ON      {Log = open_file("a", st.logf); setbuf(Log, NULL);}
#define LOG_OFF     {fclose(Log); Log = NULL;}

enum e_exoper
   { PNONE, PEQ, PLT, PLE, PGT, PGE, PNE };

typedef enum e_exoper exoper;

typedef struct s_file_stack file_stack;
typedef struct s_file_entry file_entry;
typedef struct s_gt_stack gt_stack;
typedef struct s_gt_entry gt_entry;
typedef struct s_state state;
typedef struct s_dirdes dirdes;
typedef struct s_ruledes ruledes;
typedef struct s_auxfdes auxfdes;

struct s_file_entry
   {int itype;
    int iline;
    char *name;
    FILE *fp;};

struct s_file_stack
   {int n;
    file_entry file[N_STACK];};

struct s_gt_entry
   {int itype;
    char *opt;
    char *item;};

struct s_gt_stack
   {int n;
    gt_entry st[N_STACK];};

struct s_dirdes
   {char root[MAXLINE];
    char bin[MAXLINE];
    char cfg[MAXLINE];
    char inc[MAXLINE];
    char lib[MAXLINE];
    char mng[MAXLINE];
    char sch[MAXLINE];
    char scr[MAXLINE];};

struct s_ruledes
   {char ccp[LRG];            /* .c -> .i */
    char co[LRG];             /* .c -> .o */
    char ca[LRG];             /* .c -> .a */
    char fo[LRG];             /* .f -> .o */
    char fa[LRG];             /* .f -> .a */
    char lo[LRG];             /* .l -> .o */
    char la[LRG];             /* .l -> .a */
    char lc[LRG];             /* .l -> .c */
    char yo[LRG];             /* .y -> .o */
    char ya[LRG];             /* .y -> .a */
    char yc[LRG];             /* .y -> .c */
    char th[LRG];             /* .t -> .h */
    char co_bp[LRG];          /* bad pragma versions */
    char ca_bp[LRG];
    char lo_bp[LRG];
    char la_bp[LRG];
    char yo_bp[LRG];
    char ya_bp[LRG];};

struct s_auxfdes
   {char cefn[MAXLINE];       /* config defines for C */
    FILE *CEF;
    char dpfn[MAXLINE];       /* config variable for distributed parallel */
    FILE *DPF;
    char mvfn[MAXLINE];       /* config variables for make */
    FILE *MVF;
    char urfn[MAXLINE];       /* config rules for make */
    FILE *URF;
    FILE *SEF;};

struct s_state
   {int abs_deb;
    int abs_opt;
    int create_dirs;
    int have_python;
    int installp;
    int loadp;
    int analyzep;
    int configp;
    int exep;
    int phase;
    int profilep;
    int tmp_dirp;
    int verbose;
    int launched;               /* TRUE iff PCO launched PERDB
                                 * FALSE if PERDB was already running */

    int na;
    char **args;

    file_stack fstck;
    gt_stack gstck;
    dirdes dir;
    ruledes rules;
    auxfdes aux;

    char cfgf[MAXLINE];          /* config file name */
    char logf[MAXLINE];          /* log file name */

    char cfgv[MAXLINE];          /* config variables */
    char def_tools[MAXLINE];     /* default tools */
    char def_groups[MAXLINE];    /* defaults groups */
    char toolv[MAXLINE];         /* tool variables */

    char env_csh[MAXLINE];
    char env_sh[MAXLINE];
    char env_dk[MAXLINE];
    char env_mdl[MAXLINE];

    char arch[MAXLINE];
    char host[MAXLINE];
    char os[MAXLINE];
    char osrel[MAXLINE];
    char hw[MAXLINE];
    char sys[MAXLINE];
    char system[MAXLINE];};

static state
 st = { FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,
	FALSE, FALSE, FALSE, PHASE_READ, FALSE,
	FALSE, FALSE, FALSE, };

static void
 parse_line(char *s, char *key, char *oper, char *value);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ECHO - run echo command
 *      - only exec shell if necessary
 */

static char *echo(int log, char *fmt, ...)
   {char s[LRG];
    char *rv;

    VA_START(fmt);
    VSNPRINTF(s, LRG, fmt);
    VA_END;

/*
    run(BOTH, "echo \"%s\"", s)
    run(FALSE, "echo %s", val)
    run(FALSE, "echo $%s", fvar)
    run(FALSE, "echo %s | sed 's|%s||'", nval, val)
*/

    if (strpbrk(s, "$*|{}[]\"'") == NULL)
       rv = STRSAVE(trim(s, BOTH, " \t\n"));
    else
       rv = run(BOTH, "echo %s", s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PUSH_FILE - push S onto the file stack */

static char *push_file(char *s, int itype)
   {int id, n, nd, ok;
    file_entry *se;
    static char lfile[MAXLINE], t[MAXLINE];
    static char *places[] = { NULL, "local", "std", "features",
			      "compilers", "analyze", "." };

    n  = st.fstck.n++;
    se = st.fstck.file + n;

    ok = TRUE;
    if (itype == STACK_FILE)
       {nd = sizeof(places)/sizeof(char *);
	ok = FALSE;
	for (id = 0; (id < nd) && (ok == FALSE); id++)
	    {if (places[id] == NULL)
	        {strcpy(lfile, s);
		 ok = file_exists(lfile);}
	     else
	        {snprintf(lfile, MAXLINE, "%s/%s", places[id], s);
		 ok = file_exists("%s/%s", st.dir.mng, lfile);};};

	if (ok == FALSE)
	   {noted(Log, "***> Cannot find file %s\n", s);
	    ok = FALSE;};}

    else if (itype == STACK_PROCESS)
       {if (strchr(s, '$') != NULL)
	   strncpy(lfile, echo(BOTH, "\"%s\"", s), MAXLINE);
	else
	   strncpy(lfile, s, MAXLINE);};

    if ((ok == TRUE) && (se != NULL))
       {se->itype = itype;
	se->iline = 0;
	se->name  = STRSAVE(lfile);

	if (itype == STACK_FILE)
	   {if (lfile[0] != '/')
               snprintf(t, MAXLINE, "%s/%s", st.dir.mng, lfile);
	    else
	       strncpy(t, lfile, MAXLINE);
	    se->fp = fopen(t, "r");}

	else if (itype == STACK_PROCESS)
	   se->fp = popen(lfile, "r");};

    return(lfile);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POP_FILE - pop the file stack */

static void pop_file(void)
   {int n;
    file_entry *se;

    st.fstck.n--;

    n  = st.fstck.n;
    se = st.fstck.file + n;

    if (se->itype == STACK_FILE)
       fclose(se->fp);
    else if (se->itype == STACK_PROCESS)
       pclose(se->fp);

    se->iline = 0;
    se->fp    = NULL;
    FREE(se->name);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PUSH_STRUCT - push S onto the struct stack */

static void push_struct(char *item, char *collection, int itype)
   {int n;
    char t[MAXLINE], lst[MAXLINE];
    char *p, *ps;
    gt_entry *ge;

    strcpy(t, item);
    strcpy(lst, collection);

    ps = lst;
    while (TRUE)
       {p = strtok(ps, " \t");
	if (p == NULL)
	   break;
	else if (strcmp(p, item) != 0)
	   {nstrcat(t, MAXLINE, " ");
	    nstrcat(t, MAXLINE, p);};
	ps = NULL;};

    strcpy(collection, t);

    n  = st.gstck.n++;
    ge = st.gstck.st + n;
    ge->item  = STRSAVE(item);
    ge->itype = itype;
    ge->opt   = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POP_STRUCT - pop the struct stack */

static void pop_struct(void)
   {int n;
    gt_entry *ge;

/* release the current stack entry */
    n  = --st.gstck.n;
    ge = st.gstck.st + n;

    FREE(ge->item);

/* reset the current group or tool */
    n  = st.gstck.n - 1;
    ge = st.gstck.st + n;

    dbset(NULL, "CurrGrp", ge->item);
    dbset(NULL, "CurrTool", "");

    note(Log, TRUE, "--- end");

    return;}

/*--------------------------------------------------------------------------*/

/*                                  EMITTERS                                */

/*--------------------------------------------------------------------------*/

/* WRITE_CLASS_PCO - write a PCO form text representation of
 *                 - a class CLSS of type CTYPE
 *                 - look for sub-class SUB of type STYPE
 */

static int write_class_pco(FILE *out, char *clss, char *ctype,
			   char *sub, char *stype, char *ind)
   {int i, n, ic, nc, global;
    char cl[MAXLINE], fmt[MAXLINE];
    char *c, *pc, *t, *var, *val, *entry;
    char **vars, **vals, **sa;

    strncpy(cl, clss, MAXLINE);
    for (c = cl; c != NULL; c = pc)
        {pc = strchr(c, ' ');
         if (pc == NULL)
            {if (IS_NULL(c) == TRUE)
                break;}
         else
            *pc++ = '\0';

	 global = (strcmp(c, "Glb") == 0);

	 if (global == TRUE)
	    {t  = dbget(NULL, FALSE, "Globals");
	     sa = tokenize(t, " \t\n\r");}
         else
	    {fprintf(out, "%s%s %s {\n", ind, ctype, c);
	     t  = run(BOTH, "env | egrep '^%s_' | sort", c);
	     sa = tokenize(t, "\n\r");};

         for (n = 0; sa[n] != NULL; n++);

	 if ((global == TRUE) && (n > 0))
	    fprintf(out, "# Global variables\n");

	 vars = MAKE_N(char *, n+1);
	 vals = MAKE_N(char *, n+1);
	 if ((vars != NULL) && (vals != NULL))
	    {nc = 0;
	     for (i = 0; i < n; i++)
	         {entry = sa[i];
		  if (IS_NULL(entry) == TRUE)
		     continue;
		  if (global == TRUE)
		     {var = entry;
		      val = dbget(NULL, TRUE, var);}
		  else
		     {var = entry + strlen(c) + 1;
		      val = strchr(var, '=');
		      if (val != NULL)
			 *val++ = '\0';};

		  ic = strlen(var);
		  nc = max(nc, ic);

		  vars[i] = var;
		  vals[i] = STRSAVE(val);};

	     vars[n] = NULL;
	     vals[n] = NULL;

	     if (global == TRUE)
	        snprintf(fmt, MAXLINE, "%%s%%-%ds = %%s\n", nc);
	     else
	        snprintf(fmt, MAXLINE, "%%s   %%-%ds = %%s\n", nc);

	     for (i = 0; i < n; i++)
	         {if ((vars[i] != NULL) && (vals[i] != NULL))
		     fprintf(out, fmt, ind, vars[i], vals[i]);};

	     if (global == TRUE)
	        fprintf(out, "\n");
	     else
	        fprintf(out, "}\n\n");

	     free_strings(vals);
	     FREE(vars);};

	 free_strings(sa);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_PCO - write the input form of the database
 *           - to be read back in by pco
 */

static void write_pco(state *st, char *dbname)
   {int rv;
    char t[MAXLINE];
    FILE *out;

    if (dbname != NULL)
       snprintf(t, MAXLINE, "%s.%s.pco", cgetenv(FALSE, "PERDB_PATH"), dbname);
    else
       snprintf(t, MAXLINE, "%s.pco", cgetenv(FALSE, "PERDB_PATH"));

    out = open_file("w", t);

    rv = write_class_pco(out, st->def_tools, "Tool", NULL, NULL, "");
    ASSERT(rv == 0);

    rv = write_class_pco(out, st->def_groups, "Group", st->def_tools, "Tool", "");
    ASSERT(rv == 0);

    fclose(out);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_CLASS_PERL - write a PERL form text representation of
 *                  - a class CLSS of type CTYPE
 *                  - look for sub-class SUB of type STYPE
 */

static int write_class_perl(FILE *out, char *clss, char *ctype,
			    char *sub, char *stype, char *ind)
   {int i, n, ic, nc, global;
    char cl[MAXLINE], fmt[MAXLINE];
    char *c, *pc, *t, *var, *val, *entry;
    char **vars, **vals, **sa;

    strncpy(cl, clss, MAXLINE);
    for (c = cl; c != NULL; c = pc)
        {pc = strchr(c, ' ');
         if (pc == NULL)
            {if (IS_NULL(c) == TRUE)
                break;}
         else
            *pc++ = '\0';

	 global = (strcmp(c, "Glb") == 0);

	 if (global == TRUE)
	    {t  = dbget(NULL, FALSE, "Globals");
	     sa = tokenize(t, " \t\n\r");}
         else
	    {fprintf(out, "%s'%s:%s_pact' => {\n", ind, ctype, c);
	     t  = run(BOTH, "env | egrep '^%s_' | sort", c);
	     sa = tokenize(t, "\n\r");};

         for (n = 0; sa[n] != NULL; n++);

	 if ((global == TRUE) && (n > 0))
	    fprintf(out, "# Global variables\n");

	 vars = MAKE_N(char *, n+1);
	 vals = MAKE_N(char *, n+1);
	 if ((vars != NULL) && (vals != NULL))
	    {nc = 0;
	     for (i = 0; i < n; i++)
	         {entry = sa[i];
		  if (IS_NULL(entry) == TRUE)
		     continue;
		  if (global == TRUE)
		     {var = entry;
		      val = dbget(NULL, FALSE, var);}
		  else
		     {var = entry + strlen(c) + 1;
		      val = strchr(var, '=');
		      if (val != NULL)
			 *val++ = '\0';};

		  ic = strlen(var);
		  nc = max(nc, ic);

		  vars[i] = var;
		  vals[i] = STRSAVE(val);};

	     vars[n] = NULL;
	     vals[n] = NULL;

	     if (global == TRUE)
	        snprintf(fmt, MAXLINE, "%%s%%-%ds => '%%s',\n", nc);
	     else
	        snprintf(fmt, MAXLINE, "%%s   %%-%ds => '%%s',\n", nc);

	     for (i = 0; i < n; i++)
	         {if ((vars[i] != NULL) && (vals[i] != NULL))
		     fprintf(out, fmt, ind, vars[i], vals[i]);};

	     if (global == TRUE)
	        fprintf(out, "\n");
	     else
	        fprintf(out, "%s},\n\n", ind);

	     free_strings(vals);
	     FREE(vars);};

	 free_strings(sa);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_PERL - write the input form of the database
 *            - to be read by mio
 */

static void write_perl(state *st, char *dbname)
   {int rv;
    char t[MAXLINE];
    FILE *out;

    if (dbname != NULL)
       snprintf(t, MAXLINE, "%s.%s.pl", cgetenv(FALSE, "PERDB_PATH"), dbname);
    else
       snprintf(t, MAXLINE, "%s.pl", cgetenv(FALSE, "PERDB_PATH"));
    out = open_file("w", t);

    fprintf(out, "{\n");

    rv = write_class_perl(out, st->def_tools, "Tool",
			  NULL, NULL, "   ");
    ASSERT(rv == 0);

    rv = write_class_perl(out, st->def_groups, "Group",
			  st->def_tools, "Tool", "   ");
    ASSERT(rv == 0);

    fprintf(out, "}\n");

    fclose(out);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PCO_SAVE_DB - write a text representation of the configuration database */

static int pco_save_db(char *dbname)
   {int rv;
    char t[MAXLINE];

    rv = TRUE;

    dbset(NULL, "Tools",  st.def_tools);
    dbset(NULL, "Groups", st.def_groups);

/* save the persistent database */
    if (dbname == NULL)
       nstrncpy(t, MAXLINE, "save:", -1);
    else
       snprintf(t, MAXLINE, "save %s:", dbname);
    dbcmd(NULL, t);

/* write the input form of the database - to be read back in by pco */
    write_pco(&st, dbname);

/* write the perl form of the database - to be read back in by mio */
    write_perl(&st, dbname);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PCO_LOAD_DB - load the specified database */

static int pco_load_db(char *dbname)
   {int rv;

    rv = db_restore(NULL, dbname);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ENV_OUT - write out an environment variable to various files */

static void env_out(FILE *fsh, FILE *fcsh, FILE *fdk, FILE *fmd,
		    char *var, char *val)
   {char s[LRG];
    char *vl;

    if (val == NULL)
       val = "\"\"";

    nstrncpy(s, LRG, val, -1);
    vl = trim(expand(s, LRG, NULL, FALSE), BOTH, " \t");

    note(fsh,  TRUE, "export %s=%s",    var, vl);
    note(fcsh, TRUE, "setenv %s %s",    var, vl);
    note(fdk,  TRUE, "dk_setenv %s %s", var, vl);
    note(fmd,  TRUE, "setenv %s %s;",   var, vl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_ENVF - write out the env file because other scripts need to source it
 *            - write it for CSH or SH families
 */

static void write_envf(int lnotice)
   {int i, j, n, nc, ok;
    char *p, *t, *var, *val, **sa;
    char *site[] = { "SYS_TYPE", "SYS_SITE", "DAS_ROOT", "DAI_ROOT" };
    FILE *fcsh, *fsh, *fdk, *fmd;

    separator(Log);
    if (lnotice == TRUE)
       noted(Log, "   Environment setup files - env-pact.csh, env-pact.sh, env-pact.dk, and env-pact.mdl");
    else
       note(Log, TRUE, "   Environment setup files - env-pact.csh, env-pact.sh, env-pact.dk, and env-pact.mdl");
    note(Log, TRUE, "");

    fcsh = open_file("w", "%s/env-pact.csh", st.dir.inc);
    fsh  = open_file("w", "%s/env-pact.sh",  st.dir.inc);
    fdk  = open_file("w", "%s/env-pact.dk",  st.dir.inc);
    fmd  = open_file("w", "%s/env-pact.mdl", st.dir.inc);

/* initialize module with boilerplate */
    note(fmd, TRUE,  "#%%Module1.0");
    note(fmd, TRUE,  "#");
    note(fmd, TRUE,  "# pact modulefile");
    note(fmd, TRUE,  "# vi:set filetype=tcl:");
    note(fmd, TRUE,  "#");
    note(fmd, TRUE,  "");
    note(fmd, TRUE,  "# module whatis");
    note(fmd, TRUE,  "module-whatis \"PACT Environment\";");
    note(fmd, TRUE,  "");
    note(fmd, TRUE,  "# module help");
    note(fmd, TRUE,  "proc ModulesHelp { } {");
    note(fmd, TRUE,  "   global version;");
    note(fmd, TRUE,  "");
    note(fmd, TRUE,  "   puts stderr \"\\tThis module sets your environment to use PACT\\n\";");
    note(fmd, TRUE,  "}");
    note(fmd, TRUE, "");

/* make a temporary approximation to st.env_csh
 * NOTE: used the C Shell to expand and print unique environment variable settings
 * in Bourne Shell syntax in the past
 */
    n = sizeof(site)/sizeof(char *);
    for (i = 0; i < n; i++)
        env_out(fsh, fcsh, fdk, fmd, site[i], dbget(NULL, TRUE, site[i]));

    fflush(st.aux.SEF);
    sa = file_text("%s/log/file.se", st.dir.root);
    for (n = 0; sa[n] != NULL; n++);

    for (i = 0; i < n; i++)
	{p = sa[i];

	 var = strtok(p, " ");
	 val = strtok(NULL, "\n");
	 nc  = strlen(var);

/* handle PATH specially - just gather everything that is not $PATH or ${PATH} */
	 if (strcmp(var, "PATH") == 0)
	    push_path(APPEND, epath, val);

/* weed out duplicates - taking only the last setting */
	 else
	    {ok = FALSE;
	     for (j = i+1; (j < n) && (ok == FALSE); j++)
	         ok = ((strncmp(var, sa[j], nc) == 0) && (sa[j][nc] == ' '));
		     
	     if (ok == FALSE)
	        env_out(fsh, fcsh, fdk, fmd, var, val);};};

    free_strings(sa);

    note(fcsh, TRUE, "");
    note(fsh, TRUE, "");
    note(fdk, TRUE, "");
    note(fmd, TRUE, "");

/* emit MANPATH settings */
    note(fcsh, TRUE, "if ($?MANPATH == 1) then");
    note(fcsh, TRUE, "   setenv MANPATH %s/man:$MANPATH", st.dir.root);
    note(fcsh, TRUE, "else");
    note(fcsh, TRUE, "   setenv MANPATH %s/man:", st.dir.root);
    note(fcsh, TRUE, "endif");
    note(fcsh, TRUE, "");

    note(fsh, TRUE, "if [ \"$MANPATH\" != \"\" ]; then");
    note(fsh, TRUE, "   export MANPATH=%s/man:$MANPATH", st.dir.root);
    note(fsh, TRUE, "else");
    note(fsh, TRUE, "   export MANPATH=%s/man", st.dir.root);
    note(fsh, TRUE, "fi");
    note(fsh, TRUE, "");

    note(fdk, TRUE, "dk_alter MANPATH %s/man", st.dir.root);
    note(fmd, TRUE, "prepend-path MANPATH %s/man;", echo(FALSE, st.dir.root));

    {char lPython[MAXLINE];

     if (st.have_python == TRUE)
        {snprintf(lPython, MAXLINE, "%s/python%s",
		  st.dir.lib, dbget(NULL, TRUE, "PyVers"));
	 t = dbget(NULL, TRUE, "PYTHONPATH");
	 if (t != NULL)
	    {if (strstr(t, lPython) == NULL)
	        dbset(NULL, "PYTHONPATH", "%s:$PYTHONPATH", lPython);}
	 else
	    dbset(NULL, "PYTHONPATH", lPython);

	 t = dbget(NULL, TRUE, "PYTHONPATH");
	 note(fcsh, TRUE, "setenv PYTHONPATH %s", t);
	 note(fsh, TRUE,  "export PYTHONPATH=%s", t);
	 note(fdk, TRUE,  "dk_setenv PYTHONPATH %s", t);
	 note(fmd, TRUE,  "setenv PYTHONPATH %s;", t);};};

/* emit PATH settings */
    if (IS_NULL(epath) == FALSE)
       {FOREACH(u, epath, ":\n")
	   note(fdk, TRUE, "dk_alter PATH %s", u);
	   note(fmd, TRUE, "prepend-path PATH    %s;", echo(FALSE, u));
	ENDFOR
        note(fcsh, TRUE, "setenv PATH    %s/bin:%s:$PATH", st.dir.root, epath);
        note(fsh, TRUE,  "export PATH=%s/bin:%s:$PATH", st.dir.root, epath);}
    else
       {note(fcsh, TRUE, "setenv PATH    %s/bin:$PATH", st.dir.root);
        note(fsh, TRUE, "export PATH=%s/bin:$PATH", st.dir.root);};

    note(fdk, TRUE, "dk_alter PATH    %s/bin", st.dir.root);
    note(fmd, TRUE, "prepend-path PATH    %s/bin;", echo(FALSE, st.dir.root));
    note(fdk, TRUE, "");
    note(fmd, TRUE, "");

/* write the CSH version like this because PCSH scripts like
 * PDBView need to get the users SCHEME variable if defined
 */
    note(fcsh, TRUE, "if ($?SCHEME == 1) then");
    note(fcsh, TRUE, "   setenv SCHEME  ${SCHEME}:%s/scheme", st.dir.root);
    note(fcsh, TRUE, "else");
    note(fcsh, TRUE, "   setenv SCHEME  %s/scheme", st.dir.root);
    note(fcsh, TRUE, "endif");
    note(fcsh, TRUE, "setenv ULTRA   %s/scheme", st.dir.root);

    note(fsh, TRUE, "export SCHEME=%s/scheme", st.dir.root);
    note(fsh, TRUE, "export ULTRA=%s/scheme", st.dir.root);
    note(fdk, TRUE, "dk_setenv SCHEME  %s/scheme", st.dir.root);
    note(fdk, TRUE, "dk_setenv ULTRA   %s/scheme", st.dir.root);
    note(fmd, TRUE, "setenv SCHEME  %s/scheme;", st.dir.root);
    note(fmd, TRUE, "setenv ULTRA   %s/scheme;", st.dir.root);

    fclose(fcsh);
    fclose(fsh);
    fclose(fdk);
    fclose(fmd);

    cat(Log, 0, -1, "%s/env-pact.csh", st.dir.inc);
    cat(Log, 0, -1, "%s/env-pact.sh",  st.dir.inc);
    cat(Log, 0, -1, "%s/env-pact.dk",  st.dir.inc);
    cat(Log, 0, -1, "%s/env-pact.mdl", st.dir.inc);

/* source this now before trying to compile smake in write/smake */
    note(Log, TRUE, "Sourcing %s", st.env_csh);
#if 0
    source $st.env_csh
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHECK_DIR - check for missing directories */

static void check_dir(void)
   {int i, n;
    char Created[LRG], Missing[LRG];
    char *sib, *dir;
    static char *dlst[] = {"bin", "lib", "include", "include/shell",
			   "scheme", "man", "man/man1", "man/man3"};

    n   = sizeof(dlst)/sizeof(char *);
    sib = dbget(NULL, TRUE, "InstBase");

    if (st.create_dirs == TRUE)
       {Created[0] = '\0';
        if (strcmp(sib, "none") != 0)
           {for (i = 0; i < n; i++)
                {dir = dlst[i];
                 if (dir_exists("%s/%s", sib, dir) == FALSE)
                    {run(BOTH, "mkdir -p %s/%s", sib, dir);
                     push_tok(Created, LRG, ' ', "%s/%s", sib, dir);};};};

        if (IS_NULL(Created) == FALSE)
           {noted(Log, "");
            noted(Log, "Directories:");
            FOREACH(dir, Created, " ")
               noted(Log, "   %s", dir);
            ENDFOR
            noted(Log, "have been created as requested");
            noted(Log, "");};}

    else
       {Missing[0] = '\0';
        if (strcmp(sib, "none") != 0)
           {for (i = 0; i < n; i++)
                {dir = dlst[i];
                 if (dir_exists("%s/%s", sib, dir) == FALSE)
                    {push_tok(Missing, LRG, ' ', "%s/%s", sib, dir);};};};

        if (IS_NULL(Missing) == FALSE)
           {noted(Log, "");
            noted(Log, "You have asked that PACT be installed in the following");
            noted(Log, "missing directories:");
            FOREACH (dir, Missing, " ")
               noted(Log, "   %s", dir);
            ENDFOR
            noted(Log, "You must either use the -c option to create these");
            noted(Log, "directories, choose another place to install PACT,");
            noted(Log, "or specify -i none for no installation");
            noted(Log, "");

            exit(2);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_LINE - read the next line from the input */

static void read_line(char *s, int nc)
   {char *p, *pc;
    file_entry *se;

    se = &st.fstck.file[st.fstck.n-1];

    p = fgets(s, nc-1, se->fp);
    if (p != NULL)
       {se->iline++;

	pc = strchr(p, '#');
	if (pc != NULL)
	   *pc = '\0';
	else
	   LAST_CHAR(s) = '\0';

/* toss blank and comment lines */
	if (blank_line(s) == TRUE)
           {if (st.verbose == TRUE)
	       noted(Log, "%s line %d ignored: %s",
		     se->name, se->iline, s);
	    s[0] = '\0';};}
    else
       {pop_file();
	if (st.fstck.n > 0)
	   s[0] = '\0';
	else
	   strcpy(s, "++end++");
	note(Log, TRUE, "");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_OPT - work with option specifications */

static void parse_opt(char *s)
   {int i, l, n, ok, mt;
    exoper oper;
    char vr[MAXLINE], op[MAXLINE], vl[MAXLINE];
    char *t, *avl, *arg, *opt, **sa;

    sa = tokenize(s, "[;]\n\r");
    for (n = 0; sa[n] != NULL; n++);

    ok  = FALSE;
    avl = "off";
    for (l = 0; l < n; l++)
        {vr[0] = '\0';

	 t = sa[l];
	 parse_line(t, vr, op, vl);

	 if (IS_NULL(vr) == TRUE)
	    continue;

	 arg = trim(vl, BOTH, " \t");

	 if (strcmp(op, "=") == 0)
	    oper = PEQ;
	 else if (strcmp(op, "<") == 0)
	    oper = PLT;
	 else if (strcmp(op, "<=") == 0)
	    oper = PLE;
	 else if (strcmp(op, ">") == 0)
	    oper = PGT;
	 else if (strcmp(op, ">=") == 0)
	    oper = PGE;
	 else if (strcmp(op, "!=") == 0)
	    oper = PNE;
	 else
	    oper = PNONE;

/* parse the _cmd_ key */
	 if (strcmp(vr, "_cmd_") == 0)
	    {opt = strchr(arg, '@');
	     if (opt != NULL)
	        opt[-1] = '\0';
	     for (i = 0; i < st.na; i++)
	         {if (strcmp(st.args[i], arg) == 0)
		     {if ((opt == NULL) || (strchr(opt, '@') == NULL))
			 avl = "on";
		      else
			 avl = st.args[++i];
		      break;};};}

/* parse the _env_ key */
	 else if (strcmp(vr, "_env_") == 0)
	    avl = dbget(NULL, TRUE, arg);

/* select the value */
	 else

/* treat single '*' specially since match will automatically succeed */
	    {if (strcmp(vr, "*") == 0)
	        {if (strcmp(avl, "off") != 0)
		    {strcpy(s, arg);
		     LAST_CHAR(s) = '\0';
		     strcat(s, avl);
		     ok = TRUE;};}

	     else if (strcmp(avl, vr) == 0)
	        {strcpy(s, arg);
	         ok = TRUE;}

	     else
	        {mt = match(avl, vr);
		 switch (oper)
		    {case PEQ :
		          if (mt == 0)
			     {strcpy(s, arg);
			      ok = TRUE;};
			  break;
		     case PLT :
		          if (mt < 0)
			     {strcpy(s, arg);
			      ok = TRUE;};
			  break;
		     case PLE :
		          if (mt < 1)
			     {strcpy(s, arg);
			      ok = TRUE;};
			  break;
		     case PGT :
		          if (mt > 0)
			     {strcpy(s, arg);
			      ok = TRUE;};
			  break;
		     case PGE :
		          if (mt > -1)
			     {strcpy(s, arg);
			      ok = TRUE;};
			  break;
		     case PNE :
		          if (mt != 0)
			     {strcpy(s, arg);
			      ok = TRUE;};
			  break;
		     default :
			  break;};};};};

    if (ok == FALSE)
       s[0] = '\0';

    free_strings(sa);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_LINE - parse the next line from the input */

static void parse_line(char *s, char *key, char *oper, char *value)
   {char t[LRG];
    char *p;

    strcpy(t, s);

    p = strtok(t, " \t");
    if (p != NULL)
       strcpy(key, p);

    p = strtok(NULL, " \t");
    if (p != NULL)
       strcpy(oper, p);
    else
       oper[0] = '\0';

    p = strtok(NULL, "\n");
    if (p != NULL)
       {strcpy(value, p);
	if ((strchr(oper, '=') != NULL) && (p[0] == '['))
	   {while (strchr(value, ']') == NULL)
	       {read_line(t, LRG);
		strcat(value, " ");
		strcat(value, trim(t, FRONT, " \t"));};
	    parse_opt(value);};}
    else
       value[0] = '\0';

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_RULE - read a rule into the VAR */

static void parse_rule(char *var, int nc)
   {int n;
    char line[MAXLINE], val[MAXLINE];

    val[0] = '\0';

    while (TRUE)
       {read_line(line, MAXLINE);

        if ((IS_NULL(line) == TRUE) || (strcmp(line, "++end++") == 0))
           break;

	nstrcat(val, MAXLINE, line);
        nstrcat(val, MAXLINE, "\n");};

    n = strlen(val);
    n = min(n, nc-1);
    strncpy(var, val, nc);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DP_DEFINE - gather DP defines */

static void dp_define(void)
   {char line[MAXLINE];

    if (st.aux.DPF == NULL)
       st.aux.DPF = open_file("w", st.aux.dpfn);

    while (TRUE)
       {read_line(line, MAXLINE);

	if ((strcmp(line, "end") == 0) || (IS_NULL(line) == TRUE))
	   {note(st.aux.DPF, TRUE, "");
	    break;};
    
	print_text(st.aux.DPF, "setenv %s \"\"", line);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_ANALYZE_ENV - setup the environment for programs which analyze
 *                   - features
 */

static void setup_analyze_env(char *base)
   {char alog[MAXLINE];
    FILE *out;

/* setup the analyze log file */
    snprintf(alog, MAXLINE, "%s/log/analyze",  st.dir.root);
    out = open_file("w", alog);
    note(out, TRUE, "%s", get_date());
    fclose(out);

    dbset(NULL, "Host",    st.host);
    dbset(NULL, "Arch",    st.arch);
    dbset(NULL, "System",  st.system);
    dbset(NULL, "Sys",     st.sys);
    dbset(NULL, "Base",    base);
    dbset(NULL, "RootDir", st.dir.root);
    dbset(NULL, "AnaDir",  "%s/analyze", st.dir.mng);
    dbset(NULL, "Log",     st.logf);
    dbset(NULL, "ALog",    alog);

    dbset(NULL, "AbsoluteDeb", st.abs_deb ? "TRUE" : "FALSE");
    dbset(NULL, "AbsoluteOpt", st.abs_opt ? "TRUE" : "FALSE");
    dbset(NULL, "Profile",     st.profilep ? "TRUE" : "FALSE");
    dbset(NULL, "UseTmpDir",   st.tmp_dirp ? "TRUE" : "FALSE");

    if (strncmp(st.os, "CYGWIN", 6) == 0)
       st.os[6] = '\0';

    dbset(NULL, "HostOS",      st.os);
    dbset(NULL, "HostOSRel",   st.osrel);

    if (strcmp(st.os, "Windows_NT") == 0)
       dbset(NULL, "CDecls", "TRUE");
    else
       dbset(NULL, "CDecls", "FALSE");

    dbset(NULL, "ANSI",   "ANSI");
    dbset(NULL, "MeOnly", "TRUE");

/* initialization of non-graphics flags */
    if (strcmp(st.os, "AIX") == 0)
       dbset(NULL, "NM", "/usr/bin/nm -g -X %s", dbget(NULL, TRUE, "Bits"));
    else
       dbset(NULL, "NM", "%s -g", cwhich("nm"));

    dbinitv(NULL, "CC_Version",  "");
    dbinitv(NULL, "FC_Version",  "");
    dbinitv(NULL, "LD_Version",  "");

    dbinitv(NULL, "CC_Inc",    "");
    dbinitv(NULL, "CC_Flags",  "");
    dbinitv(NULL, "LD_Lib",    "");
    dbinitv(NULL, "LD_RPath",  "");
    dbinitv(NULL, "LD_Flags",  "");

    dbinitv(NULL, "CPU",       "");
    dbinitv(NULL, "FPU",       "");
    dbinitv(NULL, "BE",        "");

/* parallel front end */
    dbinitv(NULL, "PFE",       "%s/do-run -m", st.dir.bin);

/* cross compile front end */
    if (dbcmp(NULL, "CROSS_COMPILE", "FALSE") != 0)
       dbinitv(NULL, "CFE", "%s/do-run -m", st.dir.bin);
    else
       dbinitv(NULL, "CFE", "");

/* initialization of graphics flags */
    dbinitv(NULL, "GSYS",       "X");
    dbinitv(NULL, "Std_UseX",   "TRUE");
    dbinitv(NULL, "Std_UseOGL", "FALSE");
    dbinitv(NULL, "Std_UseQD",  "FALSE");

    dbinitv(NULL, "DP_Inc",          "");
    dbinitv(NULL, "DP_Lib",          "");
    dbinitv(NULL, "MDG_Inc",         "");
    dbinitv(NULL, "MDG_Lib",         "");
    dbinitv(NULL, "MD_Inc",          "");
    dbinitv(NULL, "MD_Lib",          "");
    dbinitv(NULL, "GraphicsDevices", "PS CGM MPG PNG JPG");
    dbinitv(NULL, "GraphicsFlag",    "");

/* initialize Cfg group flags */
    dbinitv(NULL, "Cfg_CC_Flags",  dbget(NULL, FALSE, "CC_Flags"));
    dbinitv(NULL, "Cfg_CC_Inc",    dbget(NULL, FALSE, "CC_Inc"));
    dbinitv(NULL, "Cfg_LD_RPath",  dbget(NULL, FALSE, "LD_RPath"));
    dbinitv(NULL, "Cfg_LD_Flags",  dbget(NULL, FALSE, "LD_Flags"));
    dbinitv(NULL, "Cfg_LD_Lib",    dbget(NULL, FALSE, "LD_Lib"));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_OUTPUT_ENV - setup the environment for programs which write
 *                  - configured files
 */

static void setup_output_env(char *base)
   {char *rv;

/* close any open intermediate files and export their names */
    dbset(NULL, "DPFile", st.aux.dpfn);
    if (st.aux.DPF != NULL)
       fclose(st.aux.DPF);

    dbset(NULL, "CEFile", st.aux.cefn);
    if (st.aux.CEF != NULL)
       fclose(st.aux.CEF);

    dbset(NULL, "MVFile", st.aux.mvfn);
    if (st.aux.MVF != NULL)
       fclose(st.aux.MVF);

    dbset(NULL, "URFile", st.aux.urfn);
    if (st.aux.URF != NULL)
       fclose(st.aux.URF);

/* remove duplicate tokens in selected lists */
    dbset(NULL, "DP_Inc",  unique(dbget(NULL, FALSE, "DP_Inc"),  FALSE, ' '));
    dbset(NULL, "MDG_Inc", unique(dbget(NULL, FALSE, "MDG_Inc"), FALSE, ' '));
    dbset(NULL, "MD_Inc",  unique(dbget(NULL, FALSE, "MD_Inc"),  FALSE, ' '));
    dbset(NULL, "CC_Inc",  unique(dbget(NULL, FALSE, "CC_Inc"),  FALSE, ' '));

    dbset(NULL, "DP_Lib",  unique(dbget(NULL, FALSE, "DP_Lib"),  FALSE, ' '));
    dbset(NULL, "MD_Lib",  unique(dbget(NULL, FALSE, "MD_Lib"),  FALSE, ' '));
    dbset(NULL, "LD_Lib",  unique(dbget(NULL, FALSE, "LD_Lib"),  FALSE, ' '));

/* NOTE: for OSX this would reduce -framework FOO -framework BAR
 * to -framework FOO BAR which is not legal
 */
    if (strcmp(st.os, "Darwin") != 0)
       dbset(NULL, "MDG_Lib", unique(dbget(NULL, FALSE, "MDG_Lib"), FALSE, ' '));

    dbset(NULL, "BinDir",  st.dir.bin);
    dbset(NULL, "IncDir",  st.dir.inc);
    dbset(NULL, "ScrDir",  st.dir.scr);
    dbset(NULL, "SchDir",  st.dir.sch);
    dbset(NULL, "CfgDir",  st.dir.cfg);

    rv = dbget(NULL, TRUE, "HavePython");
    if (rv == NULL)
       {if (strcmp(rv, "FALSE") == 0)
	   dbset(NULL, "HavePython", "FALSE");
	else
	   dbset(NULL, "HavePython",  "TRUE");};

    dbset(NULL, "Load",        st.loadp ? "TRUE" : "FALSE");
    dbset(NULL, "NoExe",       st.exep ? "TRUE" : "FALSE");
    dbset(NULL, "ConfigVars",  st.cfgv);
    dbset(NULL, "DefGroups",   st.def_groups);
    dbset(NULL, "ConfigFile",  st.cfgf);

    dbset(NULL, "CCP",     st.rules.ccp);
    dbset(NULL, "CCObj",   st.rules.co);
    dbset(NULL, "CCArc",   st.rules.ca);
    dbset(NULL, "LexObj",  st.rules.lo);
    dbset(NULL, "LexArc",  st.rules.la);
    dbset(NULL, "LexC",    st.rules.lc);
    dbset(NULL, "YaccObj", st.rules.yo);
    dbset(NULL, "YaccArc", st.rules.ya);
    dbset(NULL, "YaccC",   st.rules.yc);
    dbset(NULL, "FCObj",   st.rules.fo);
    dbset(NULL, "FCArc",   st.rules.fa);
    dbset(NULL, "TemplH",  st.rules.th);

    dbset(NULL, "CCObj_BP",   st.rules.co_bp);
    dbset(NULL, "CCArc_BP",   st.rules.ca_bp);
    dbset(NULL, "LexObj_BP",  st.rules.lo_bp);
    dbset(NULL, "LexArc_BP",  st.rules.la_bp);
    dbset(NULL, "YaccObj_BP", st.rules.yo_bp);
    dbset(NULL, "YaccArc_BP", st.rules.ya_bp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DEFAULT_VAR - setup default variable values */

static void default_var(char *base)
   {char cmd[MAXLINE];

    if (cdefenv("USER") == FALSE)
       {if (cdefenv("LOGNAME") == FALSE)
	   dbset(NULL, "USER", "anonymous");
        else
	   dbset(NULL, "USER", cgetenv(TRUE, "LOGNAME"));};

    csetenv("PATH", "%s:%s", st.dir.mng, cgetenv(TRUE, "PATH"));

    run(BOTH, "env | sort");

/* define the set of specifications which define a tool */
    st.toolv[0] = '\0';
    push_tok(st.toolv, MAXLINE, ' ', "Exe");
    push_tok(st.toolv, MAXLINE, ' ', "Flags");
    push_tok(st.toolv, MAXLINE, ' ', "Version");
    push_tok(st.toolv, MAXLINE, ' ', "Debug");
    push_tok(st.toolv, MAXLINE, ' ', "Optimize");
    push_tok(st.toolv, MAXLINE, ' ', "Shared");
    push_tok(st.toolv, MAXLINE, ' ', "Inc");
    push_tok(st.toolv, MAXLINE, ' ', "Lib");
    push_tok(st.toolv, MAXLINE, ' ', "RPath");
    push_tok(st.toolv, MAXLINE, ' ', "IFlag");
    push_tok(st.toolv, MAXLINE, ' ', "XFlag");

/* define and initialize the (special) config variables */
    st.cfgv[0] = '\0';
    push_tok(st.cfgv, MAXLINE, ' ', "CC_Exe");
    push_tok(st.cfgv, MAXLINE, ' ', "CC_Linker");
    push_tok(st.cfgv, MAXLINE, ' ', "CC_Flags");
    push_tok(st.cfgv, MAXLINE, ' ', "CC_Debug");
    push_tok(st.cfgv, MAXLINE, ' ', "CC_Optimize");
    push_tok(st.cfgv, MAXLINE, ' ', "CC_Inc");
    push_tok(st.cfgv, MAXLINE, ' ', "FC_Exe");
    push_tok(st.cfgv, MAXLINE, ' ', "FC_Linker");
    push_tok(st.cfgv, MAXLINE, ' ', "FC_Flags");
    push_tok(st.cfgv, MAXLINE, ' ', "FC_Debug");
    push_tok(st.cfgv, MAXLINE, ' ', "FC_Optimize");
    push_tok(st.cfgv, MAXLINE, ' ', "LD_Exe");
    push_tok(st.cfgv, MAXLINE, ' ', "LD_RPath");
    push_tok(st.cfgv, MAXLINE, ' ', "LD_Flags");
    push_tok(st.cfgv, MAXLINE, ' ', "LD_Lib");
    push_tok(st.cfgv, MAXLINE, ' ', "CC_Shared");
    push_tok(st.cfgv, MAXLINE, ' ', "LD_Shared");

    strcpy(st.sys, path_tail(st.cfgf));

    unamef(st.host,  MAXLINE, "n");
    unamef(st.os,    MAXLINE, "s");
    unamef(st.osrel, MAXLINE, "r");
    unamef(st.hw,    MAXLINE, "m");

    snprintf(st.dir.scr, MAXLINE, "%s/scripts", base);
    snprintf(cmd, MAXLINE, "%s/system-id", st.dir.scr);
    strncpy(st.arch, run(BOTH, cmd), MAXLINE);

/* check variables which may have been initialized from the command line */
    if (IS_NULL(st.system) == TRUE)
       strncpy(st.system, run(BOTH, "%s/cfgman use", st.dir.scr), MAXLINE);

    dbinitv(NULL, "CfgMan",        "%s/cfgman", st.dir.scr);
    dbinitv(NULL, "Globals",       "");
    dbinitv(NULL, "MngDir",        st.dir.mng);
    dbinitv(NULL, "InstBase",      "none");
    dbinitv(NULL, "PubInc",        "");
    dbinitv(NULL, "PubLib",        "");
    dbinitv(NULL, "ScmDir",        "scheme");
    dbinitv(NULL, "Man1Dir",       "man/man1");
    dbinitv(NULL, "Man3Dir",       "man/man3");
    dbinitv(NULL, "CROSS_COMPILE", "FALSE");

/* global variables */
    snprintf(st.dir.root, MAXLINE, "%s/dev/%s",       base, st.system);
    snprintf(st.dir.inc,  MAXLINE, "%s/include",      st.dir.root);
    snprintf(st.env_csh,  MAXLINE, "%s/env-pact.csh", st.dir.inc);
    snprintf(st.env_sh,   MAXLINE, "%s/env-pact.sh",  st.dir.inc);
    snprintf(st.env_dk,   MAXLINE, "%s/env-pact.dk",  st.dir.inc);
    snprintf(st.env_mdl,  MAXLINE, "%s/env-pact.mdl", st.dir.inc);

    snprintf(st.dir.inc, MAXLINE, "%s/include", st.dir.root);
    snprintf(st.dir.lib, MAXLINE, "%s/lib",     st.dir.root);
    snprintf(st.dir.bin, MAXLINE, "%s/bin",     st.dir.root);
    snprintf(st.dir.sch, MAXLINE, "%s/scheme",  st.dir.root);

/* variable defaults */
    strcpy(st.def_tools,    "");

    push_struct("Glb", st.def_groups, STACK_GROUP);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DEFAULT_FILES - setup the various work files */

static void default_files(int append)
   {

    snprintf(st.logf, MAXLINE, "%s/log/config", st.dir.root);
    if (append == FALSE)
       unlink(st.logf);
    LOG_ON;

    st.aux.SEF = open_file("w+", "%s/log/file.se", st.dir.root);

    snprintf(st.aux.mvfn, MAXLINE, "%s/log/file.mv", st.dir.root);
    st.aux.MVF = NULL;

    snprintf(st.aux.cefn, MAXLINE, "%s/log/file.ce", st.dir.root);
    st.aux.CEF = NULL;

    snprintf(st.aux.urfn, MAXLINE, "%s/log/file.ur", st.dir.root);
    st.aux.URF = NULL;

    snprintf(st.aux.dpfn, MAXLINE, "%s/log/file.dpe", st.dir.root);
    st.aux.DPF = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DEFAULT_RULES - setup the default rules for CC, Lex, Yacc, and FC */

static void default_rules(void)
   {char *ar, *cd, *le, *ye, *rm, *tc;

    le  = "sed \"s|lex.yy.c|$*.c|\" lex.yy.c | sed \"s|yy|$*_|g\" > $*.c";
    ye  = "sed \"s|y.tab.c|$*.c|\" y.tab.c | sed \"s|yy|$*_|g\" > $*.c";
    ar  = "${AR} ${AROpt} ${TGTLib} $*.o 2>> errlog";
    rm  = "${RM} errlog";
    cd  = "cd ${PACTTmpDir}";
    tc  = "touch errlog";

/* C rules */
    snprintf(st.rules.ccp,   LRG,
             "\t@(%s ; \\\n          %s)\n",
	     "echo \"${CCAnnounce} -P $<\"",
	     "${CC} -P $<");

    snprintf(st.rules.co, LRG,
             "\t@(%s ; \\\n          %s)\n",
	     "echo \"${CCAnnounce} -c $<\"",
	     "${CC} -c $< -o $@");

    snprintf(st.rules.ca, LRG,
             "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"${CCAnnounce} -c $<\"",
	     cd, rm, tc,
	     "${CC} -c ${PACTSrcDir}/$< -o $*.o",
	     ar,
	     "${RM} $*.o 2>> errlog");

/* lex rules */
    snprintf(st.rules.lo, LRG,
             "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
             "echo \"lex $<\"",
	     rm, tc,
	     "${LEX} $< 2>> errlog",
	     le,
	     "${LX} -c $*.c",
	     "${RM} lex.yy.c $*.c");

    snprintf(st.rules.la, LRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"lex $<\"",
	     cd, rm, tc,
	     "${LEX} -t ${PACTSrcDir}/$< 1> lex.yy.c 2>> errlog",
	     le,
	     "echo \"${LX} -c $*.c\"",
	     "${LX} -c $*.c -o $*.o",
	     ar,
	     "${RM} lex.yy.c $*.c");

    snprintf(st.rules.lc, LRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"lex $<\"",
	     cd, rm, tc,
	     "${LEX} ${PACTSrcDir}/$< 2>> errlog",
	     le,
	     "${RM} lex.yy.c");

/* yacc rules */
    snprintf(st.rules.yo, LRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"yacc $<\"",
	     cd, rm, tc,
	     "${YACC} ${PACTSrcDir}/$< 2>> errlog",
	     ye,
	     "${YC} -c $*.c -o $*.o",
	     "${RM} $*.c");

    snprintf(st.rules.ya, LRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"yacc $<\"",
	     cd, rm, tc,
	     "${YACC} ${PACTSrcDir}/$< 2>> errlog",
	     ye,
	     "echo \"${YC} -c $*.c\"",
	     "${YC} -c $*.c -o $*.o",
	     ar,
	     "${RM} $*.c $*.o");

    snprintf(st.rules.yc, LRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"yacc $<\"",
	     cd, rm, tc,
	     "${YACC} ${PACTSrcDir}/$< 2>> errlog",
	     ye,
	     "mv $*.c ${PACTSrcDir}");
      
/* Fortran rules */
    snprintf(st.rules.fo, LRG,
	     "\t%s \\\n\t%s \\\n\t%s \\\n\t%s \\\n\t%s \\\n\t%s\n",
	     "@if [ ${FC_Exe} == none ]; then",
	     "   echo \"No Fortran compiler for $<\" ;",
	     "else",
	     "   echo \"${FCAnnounce} -c $<\" ;",
	     "   ${FC} -c $< -o $@ ;",
	     "fi");

    snprintf(st.rules.fa, LRG,
	     "\t%s \\\n\t%s \\\n\t%s \\\n\t%s \\\n\t     %s ; \\\n\t     %s ; \\\n\t     %s ; \\\n\t%s \\\n\t     %s ; \\\n\t%s \\\n\t%s\n",
	     "@if [ ${FC_Exe} == none ]; then",
	     "   echo \"No Fortran compiler for $<\" ;",
	     "else",
	     "   echo \"${FCAnnounce} -c $<\" ;",
	     cd, rm, tc,
	     "     ${FC} -c ${PACTSrcDir}/$< -o $*.o ;",
	     ar,
	     "     ${RM} $*.o 2>> errlog ;",
	     "fi");

/* template rules */
    snprintf(st.rules.th, LRG,
	     "\t@(%s ; \\\n          %s)\n",
	     "echo \"${Template} $< -o ${IncDir}/$*.h\"",
	     "${BinDir}/${Template} $< -o ${IncDir}/$*.h");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BAD_PRAGMA_RULES - setup the rules for CC, Lex, Yacc, and FC
 *                  - allowing for bad handling of _Pragma
 */

static void bad_pragma_rules(void)
   {char *ar, *cd, *le, *ye, *rm, *tc;

    le  = "sed \"s|lex.yy.c|$*.c|\" lex.yy.c | sed \"s|yy|$*_|g\" > $*.c";
    ye  = "sed \"s|y.tab.c|$*.c|\" y.tab.c | sed \"s|yy|$*_|g\" > $*.c";
    ar  = "${AR} ${AROpt} ${TGTLib} $*.o 2>> errlog";
    rm  = "${RM} errlog";
    cd  = "cd ${PACTTmpDir}";
    tc  = "touch errlog";

/* C rules */
    snprintf(st.rules.co_bp, LRG,
             "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"${CCAnnounce} -c $<\"",
	     "${RM} $*.int.c",
	     "${CC} -E $< -o $@ > $*.int.c",
	     "${CC} -c $*.int.c -o $@",
	     "${RM} $*.int.c");

    snprintf(st.rules.ca_bp, LRG,
             "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"${CCAnnounce} -c $<\"",
	     cd, rm, tc,
	     "${RM} $*.int.c",
	     "${CC} -E ${PACTSrcDir}/$< > $*.int.c",
	     "${CC} -c $*.int.c -o $*.o",
	     ar,
	     "${RM} $*.int.c $*.o 2>> errlog");

/* lex rules */
    snprintf(st.rules.lo_bp, LRG,
             "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
             "echo \"lex $<\"",
	     rm, tc,
	     "${LEX} $< 2>> errlog",
	     le,
	     "${LX} -E $*.c > $*.int.c",
	     "${LX} -c $*.int.c",
	     "${RM} lex.yy.c $*.int.c $*.c");

    snprintf(st.rules.la_bp, LRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"lex $<\"",
	     cd, rm, tc,
	     "${LEX} -t ${PACTSrcDir}/$< 1> lex.yy.c 2>> errlog",
	     le,
	     "echo \"${LX} -c $*.c\"",
	     "${LX} -E $*.c > $*.int.c",
	     "${LX} -c $*.int.c -o $*.o",
	     ar,
	     "${RM} lex.yy.c $*.int.c $*.c");

/* yacc rules */
    snprintf(st.rules.yo_bp, LRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"yacc $<\"",
	     cd, rm, tc,
	     "${YACC} ${PACTSrcDir}/$< 2>> errlog",
	     ye,
	     "${YC} -E $*.c > $*.int.c",
	     "${YC} -c $*.int.c -o $*.o",
	     "${RM} $*.int.c $*.c");

    snprintf(st.rules.ya_bp, LRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"yacc $<\"",
	     cd, rm, tc,
	     "${YACC} ${PACTSrcDir}/$< 2>> errlog",
	     ye,
	     "echo \"${YC} -c $*.c\"",
	     "${YC} -E $*.c > $*.int.c",
	     "${YC} -c $*.int.c -o $*.o",
	     ar,
	     "${RM} $*.c $*.int.c $*.o");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_SESSION - initialize the state of the config session */

static void init_session(char *base, int append)
   {

/* setup default variable values */
    default_var(base);

/* setup the default rules for CC, Lex, Yacc, and FC */
    default_rules();
    bad_pragma_rules();

/* setup work files */
    default_files(append);

    printf("\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SET_VAR - set a variable as directed */

static void set_var(int rep, char *var, char *oper, char *val)
   {char fvar[MAXLINE], nval[MAXLINE], mval[MAXLINE], lval[MAXLINE];
    char s[LRG+1];
    char *prfx, *t;
    gt_entry *ge;

    ge   = st.gstck.st + st.gstck.n - 1;
    prfx = ge->item;

/* attach the current group suffix */
    if (strcmp(prfx, "Glb") == 0)
       {t = dbget(NULL, FALSE, "Globals");
	snprintf(s, LRG, "%s %s", t, var);
	dbset(NULL, "Globals", unique(s, FALSE, ' '));
	strncpy(fvar, var, MAXLINE);}
    else
       snprintf(fvar, MAXLINE, "%s_%s", prfx, var);

    clean_space(val);

    if ((strcmp(oper, "+=") == 0) || (strcmp(oper, "=+") == 0))
       {if (dbdef(NULL, fvar) == FALSE)
           {note(Log, TRUE, "Variable %s does not exist changing %s to =",
                 fvar, oper);
            oper = "=";};};

/* set variable */
    if (strcmp(oper, "=") == 0)
       {t = echo(FALSE, val);
        if (IS_NULL(t) == FALSE)
           {strncpy(nval, t, MAXLINE);
            if (rep == TRUE)
	       {if (st.aux.MVF == NULL)
		   st.aux.MVF = open_file("w", st.aux.mvfn);

		note(st.aux.MVF, TRUE, "%s = %s", fvar, nval);};

            dbset(NULL, fvar, nval);}

        else
           {nval[0] = '\0';
            dbset(NULL, fvar, nval);};}

/* add item to beginning of the existing variable */
    else if (strcmp(oper, "+=") == 0)

/* check value of val
 * we want things such as "foo += $bar" to do nothing if
 * "$bar" is not defined
 */
       {t = echo(FALSE, val);
        if (IS_NULL(t) == FALSE)
           {strncpy(lval, t, MAXLINE);
            strncpy(nval, echo(FALSE, "$%s", fvar), MAXLINE);
            note(Log, TRUE, "Change    |%s|", fvar);
            note(Log, TRUE, "Prepend   |%s|", lval);
            note(Log, TRUE, "Old value |%s|", nval);

            snprintf(mval, MAXLINE, "%s %s", lval, nval);
            note(Log, TRUE, "New value |%s|", mval);

            note(Log, TRUE, "Change expression |setenv %s %s|", fvar, mval);
            dbset(NULL, fvar, mval);}
        else
           note(Log, TRUE, "   += not changing %s - no value for |%s|", fvar, val);}

/* add item to end of the existing variable */
    else if (strcmp(oper, "=+") == 0)

/* check value of val
 * we want things such as "foo =+ $bar" to do nothing if
 * "$bar" is not defined
 */
       {t = echo(FALSE, val);
        if (IS_NULL(t) == FALSE)
           {strncpy(lval, t, MAXLINE);
            strncpy(nval, echo(FALSE, "$%s", fvar), MAXLINE);
            note(Log, TRUE, "Change    |%s|", fvar);
            note(Log, TRUE, "Append    |%s|", lval);
            note(Log, TRUE, "Old value |%s|", nval);

            snprintf(mval, MAXLINE, "%s %s", nval, lval);
            note(Log, TRUE, "New value |%s|", mval);

            note(Log, TRUE, "Change expression |setenv %s %s|", fvar, mval);
            dbset(NULL, fvar, mval);}
         else
            note(Log, TRUE, "   =+ not changing %s - no value for |%s|", fvar, val);}

/* remove literal item from variable */
    else if (strcmp(oper, "-=") == 0)
       {strncpy(nval, echo(FALSE, "$%s", fvar), MAXLINE);
        note(Log, TRUE, "Change    |%s|", fvar);
        note(Log, TRUE, "Remove    |%s|", val);
        note(Log, TRUE, "Old value |%s|", nval);

/*        strncpy(nval, run(FALSE, "echo %s | sed 's|%s||'", nval, val), MAXLINE); */
        strncpy(nval, echo(FALSE, "%s | sed 's|%s||'", nval, val), MAXLINE);
        note(Log, TRUE, "New value |%s|", nval);

        note(Log, TRUE, "Change expression |setenv %s %s|", fvar, nval);
	dbset(NULL, fvar, nval);}

/* set value only if undefined */
    else if (strcmp(oper, "=?") == 0)

/* check value of val
 * we want things such as "foo =? $bar" to do nothing if
 * "$bar" is not defined
 */
       {t = echo(FALSE, val);
        if (IS_NULL(t) == FALSE)
	   dbset(NULL, fvar, t);
        else
           note(Log, TRUE, "   =? not changing %s - no value for |%s|", fvar, val);}

    else
       noted(Log, "Bad operator '%s' in SET_VAR", oper);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROCESS_USE - do all the variable settings implied by a Use specification */

static void process_use(char *sg, char *oper)
   {int whch;
    char nvr[MAXLINE];
    char *val;

    whch = -1;

/* determine whether or not it is a tool we are filling */
    FOREACH (t, st.def_tools, " ")
       if (strcmp(t, sg) == 0)
          {whch = STACK_TOOL;
           break;};
    ENDFOR

/* determine whether or not it is a group we are filling */
    FOREACH (t, st.def_groups, " ")
       if (strcmp(t, sg) == 0)
          {whch = STACK_GROUP;
           break;};
    ENDFOR

    switch (whch)

/* fill out a group */
       {case STACK_GROUP:
             note(Log, TRUE, "Use group %s to fill group %s",
		  sg, dbget(NULL, FALSE, "CurrGrp"));
             FOREACH(var, st.cfgv, " ")
                snprintf(nvr, MAXLINE, "%s_%s", sg, var);
                if (dbdef(NULL, nvr) == TRUE)
                   {val = dbget(NULL, TRUE, nvr);
                    if (strcmp(var, "Exe") == 0)
                       set_var(FALSE, var, "=", val);
                    else
                       set_var(FALSE, var, oper, val);};
             ENDFOR
             break;

/* fill out a tool */
        case STACK_TOOL:
             if (dbcmp(NULL, "CurrTool", "") == 0)
                {note(Log, TRUE, "Use tool %s to fill group %s",
		      sg, dbget(NULL, FALSE, "CurrGrp"));
                 FOREACH(var, st.toolv, " ")
                    snprintf(nvr, MAXLINE, "%s_%s", sg, var);
                    if (dbdef(NULL, nvr) == TRUE)
                       {val = dbget(NULL, TRUE, nvr);
                        set_var(FALSE, nvr, oper, val);};
                 ENDFOR}
             else
                {note(Log, TRUE, "Use tool %s to fill tool %s",
		      sg, dbget(NULL, FALSE, "CurrTool"));
                 FOREACH(var, st.toolv, " ")
                    snprintf(nvr, MAXLINE, "%s_%s", sg, var);
                    if (dbdef(NULL, nvr) == TRUE)
                       {val = dbget(NULL, TRUE, nvr);
                        set_var(FALSE, var, oper, val);};
                 ENDFOR};
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_CONFIG - read the user configuration file */

static void read_config(char *cfg, int quiet)
   {int il;
    char line[LRG], key[MAXLINE], oper[LRG], value[LRG];
    char *path;

    separator(Log);
    if (quiet == TRUE)
       {note(Log, TRUE, "");
	note(Log, TRUE, "Reading configuration file %s", cfg);}
    else
       {noted(Log, "");
	noted(Log, "Reading configuration file %s", cfg);};
    note(Log, TRUE, "");

    note(Log, TRUE, "");
    note(Log, TRUE, "Strict checking level is %s", dbget(NULL, TRUE, "STRICT"));
    note(Log, TRUE, "");
    separator(Log);

    path = push_file(cfg, STACK_FILE);

/* toplevel loop over the input to define configuration parameters */
    for (il = 0; TRUE; il++)
        {if (dbdef(NULL, "STOP") == TRUE)
	    {noted(Log, " ");
	     exit(1);};

	 read_line(line, LRG);

	 if (IS_NULL(line) == TRUE)
            continue;
	 else if (strcmp(line, "++end++") == 0)
            break;

	 parse_line(line, key, oper, value);

/* handle include directives */
	 if (strcmp(key, "include") == 0)
	    {int n;
	     char ldepth[MAXLINE];

	     n = st.fstck.n;
	     memset(ldepth, ' ', 3*n);
	     ldepth[3*n] = '\0';

	     path = push_file(oper, STACK_FILE);
	     note(Log, TRUE, "");
	     noted(Log, "%sincluding %s", ldepth, path);}

/* handle run directives */
	 else if (strcmp(key, "run") == 0)
	    {int n;
	     char ldepth[MAXLINE];

	     n = st.fstck.n;
	     memset(ldepth, ' ', 3*n);
	     ldepth[3*n] = '\0';

	     path = push_file(line+4, STACK_PROCESS);
	     note(Log, TRUE, "");
	     if (st.phase == PHASE_READ)
	        noted(Log, "%srunning %s", ldepth, path);}

/* handle definition of the distributed parallel environment */
	 else if (strcmp(key, "DPEnvironment") == 0)
	    dp_define();

	 else if (strcmp(key, "InstBase") == 0)
	    {if (st.installp == FALSE)
	        {dbset(NULL, "InstBase", value);
		 dbset(NULL, "PubInc",   "-I%s/include", value);
		 dbset(NULL, "PubLib",   "-L%s/lib", value);};}

	 else if (strcmp(key, "exep") == 0)
	    {st.exep = TRUE;
	     st.loadp  = FALSE;}

	 else if (strcmp(key, "define") == 0)
	    {if (st.aux.CEF == NULL)
	        st.aux.CEF = open_file("w", st.aux.cefn);
	     note(st.aux.CEF, TRUE, "#define %s %s", oper, value);}

	 else if (strcmp(key, "setenv") == 0)
	    {char *s;
	     note(st.aux.SEF, TRUE, "%s %s", oper, value);
	     if (strcmp(oper, "PATH") == 0)
                push_path(APPEND, epath, value);
	     s = echo(FALSE, value);
	     dbset(NULL, oper, s);}

	 else if (strcmp(key, "parent") == 0)
	    {char *s, *var, *val;

	     s   = line + 7;
	     var = s + strspn(s, " \t");
	     val = delimited(var, "(", ")");

	     if (strcmp(var, "PATH") == 0)
	        {push_path(APPEND, epath, val);
		 s = echo(FALSE, val);
		 dbset(NULL, var, s);}
	     else
	        dbset(NULL, var, val);

/*	     if (st.phase == PHASE_READ) */
	        note(st.aux.SEF, TRUE, "%s \"%s\"", var, val);
	     note(Log, TRUE, "Command: setenv %s %s", var, val);}

/* handle Tool specifications */
	 else if (strcmp(key, "Tool") == 0)
	    {note(Log, TRUE, "--- tool %s", oper);
	     dbset(NULL, "CurrTool", oper);
	     note(Log, TRUE, "Defining tool %s", oper);
	     push_struct(oper, st.def_tools, STACK_TOOL);}

/* handle Group specifications */
	 else if (strcmp(key, "Group") == 0)
	    {note(Log, TRUE, "--- group %s", oper);
	     dbset(NULL, "CurrGrp", oper);
	     push_struct(oper, st.def_groups, STACK_GROUP);}

/* handle Use specifications */
	 else if (strcmp(key, "Use") == 0)
	    process_use(value, oper);

/* handle struct close specifications */
	 else if (strcmp(key, "}") == 0)
            pop_struct();

	 else if ((strcmp(oper, "=") == 0)  ||
		  (strcmp(oper, "+=") == 0) ||
		  (strcmp(oper, "=+") == 0) ||
		  (strcmp(oper, "-=") == 0) ||
		  (strcmp(oper, "=?") == 0))
	    set_var(TRUE, key, oper, value);

/* .c.i rule handler */
	 else if (strcmp(key, ".c.i:") == 0)
	    {parse_rule(st.rules.ccp, MAXLINE);
	     if (st.verbose == TRUE)
	        noted(Log, "Redefining .c.i rule:\n%s\n", st.rules.ccp);}

/* .c.o rule handler */
	 else if (strcmp(key, ".c.o:") == 0)
	    {parse_rule(st.rules.co, MAXLINE);
	     if (st.verbose == TRUE)
	        noted(Log, "Redefining .c.o rule:\n%s\n", st.rules.co);}

/* .c.a rule handler */
	 else if (strcmp(key, ".c.a:") == 0)
	    {parse_rule(st.rules.ca, MAXLINE);
	     if (st.verbose == TRUE)
	        noted(Log, "Redefining .c.a rule:\n%s\n", st.rules.ca);}

/* .f.o rule handler */
	 else if (strcmp(key, ".f.o:") == 0)
	    {parse_rule(st.rules.fo, MAXLINE);
	     if (st.verbose == TRUE)
	        noted(Log, "Redefining .f.o rule:\n%s\n", st.rules.fo);}

/* .f.a rule handler */
	 else if (strcmp(key, ".f.a:") == 0)
	    {parse_rule(st.rules.fa, MAXLINE);
	     if (st.verbose == TRUE)
	        noted(Log, "Redefining .f.a rule:\n%s\n", st.rules.fa);}

/* .l.o rule handler */
	 else if (strcmp(key, ".l.o:") == 0)
	    {parse_rule(st.rules.lo, MAXLINE);
	     if (st.verbose == TRUE)
	        noted(Log, "Redefining .l.o rule:\n%s\n", st.rules.lo);}

/* .l.a rule handler */
	 else if (strcmp(key, ".l.a:") == 0)
	    {parse_rule(st.rules.la, MAXLINE);
	     if (st.verbose == TRUE)
	        noted(Log, "Redefining .l.a rule:\n%s\n", st.rules.la);}

/* .l.c rule handler */
	 else if (strcmp(key, ".l.c:") == 0)
	    {parse_rule(st.rules.lc, MAXLINE);
	     if (st.verbose == TRUE)
	        noted(Log, "Redefining .l.c rule:\n%s\n", st.rules.lc);}

/* .y.o rule handler */
	 else if (strcmp(key, ".y.c:") == 0)
	    {parse_rule(st.rules.yo, MAXLINE);
	     if (st.verbose == TRUE)
	        noted(Log, "Redefining .y.o rule:\n%s\n", st.rules.yo);}

/* .y.a rule handler */
	 else if (strcmp(key, ".y.a:") == 0)
	    {parse_rule(st.rules.ya, MAXLINE);
	     if (st.verbose == TRUE)
	        noted(Log, "Redefining .y.a rule:\n%s\n", st.rules.ya);}

/* .y.c rule handler */
	 else if (strcmp(key, ".y.c:") == 0)
	    {parse_rule(st.rules.yc, MAXLINE);
	     if (st.verbose == TRUE)
	        noted(Log, "Redefining .y.c rule:\n%s\n", st.rules.yc);}

/* gather unknown specifications during read phase */
	 else if (st.phase == PHASE_READ)
	    {if (st.aux.URF == NULL)
	        st.aux.URF = open_file("w", st.aux.urfn);

	     fputs(line, st.aux.URF);}

/* print unknown specifications during any non-read phase */
	 else
            printf("%s\n", line);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ANALYZE_CONFIG - analyze the system configuration */

static void analyze_config(char *base)
   {

    st.phase = PHASE_ANALYZE;

    separator(Log);
    noted(Log, "Analyzing system on %s", st.host);
    note(Log, TRUE, "");

    write_envf(FALSE);
    
/* setup the environment for programs which analyze features */
    setup_analyze_env(base);

    push_dir(st.dir.cfg);

/* read the file which does the analysis */
    if (file_exists("../analyze/program-analyze") == TRUE)
       read_config("program-analyze", TRUE);

    run(BOTH, "rm * > /dev/null 2>&1");
    pop_dir();

    noted(Log, "");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SUMMARIZE_CONFIG - make a summary report of the configuration */

static void summarize_config(void)
   {

    if (file_executable("analyze/summary") == TRUE)
       printf("%s\n", run(BOTH, "analyze/summary"));

    pco_save_db(NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FINISH_CONFIG - complete the configuration files */

static void finish_config(char *base)
   {

    st.phase = PHASE_WRITE;

/* these are finally determined by now and it is safe to define them */
    snprintf(st.dir.inc, MAXLINE, "%s/include", st.dir.root);
    snprintf(st.dir.lib, MAXLINE, "%s/lib",     st.dir.root);
    snprintf(st.dir.bin, MAXLINE, "%s/bin",     st.dir.root);
    snprintf(st.dir.sch, MAXLINE, "%s/scheme",  st.dir.root);

    separator(Log);
    noted(Log, "Writing system dependent files");
    note(Log, TRUE, "");

/* if T3D, fiddle pdb fortran interface regression test source */
    if (dbcmp(NULL, "PFE", "mppexec") == 0)
       {char tmpf[MAXLINE];

	snprintf(tmpf, MAXLINE, "tmp-%s", st.system);

	push_dir("%s/pdb", base);

        run(BOTH, "mv pdftst.f %s.f", tmpf);
        run(BOTH, "sed -e s/'iarg, iargc'/'iarg, ipxfargc, ilen, ierror'/"
            "-e s/'iargc()'/'ipxfargc()'/"
            "-e s/'getarg(iarg, arg)'/'pxfgetarg(iarg, arg, ilen, ierror)'/g"
            "%s.f > pdftst.f",
            tmpf);

       pop_dir();};

    setup_output_env(base);

/* write the configured files for PACT */
    write_envf(TRUE);

    LOG_OFF;

    if (file_exists("analyze/program-fin") == TRUE)
       read_config("program-fin", TRUE);

    LOG_ON;

    note(Log, TRUE, "");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LAUNCH_PERDB - look for -f arg and if found launch perdb first of all */

int launch_perdb(int c, char **v)
   {int i, l, n, ok, rv;
    char t[MAXLINE];
    char *db;
    static char *sfx[] = { "db", "log", "pid" };

    st.launched = FALSE;

    if (cdefenv("PERDB_PATH") == FALSE)
       {ok = FALSE;
	n  = sizeof(sfx)/sizeof(char *);

	for (i = 1; i < c; i++)
	    {if (strcmp(v[i], "-f") == 0)
	        {db = v[++i];
		 csetenv("PERDB_PATH", db);

		 for (l = 0; l < n; l++)
		     {snprintf(t, MAXLINE, "%s.%s", db, sfx[l]);
		      unlink(t);};

		 rv = execute(FALSE, "perdb -f %s -l -c -s", db);
		 ok = ((rv & 0xff) == 0);
		 break;};};

/* give the server some time to initialize */
	nsleep(100);

	st.launched = ok;};

    return(ok);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* KILL_PERDB - shutdown perdb */

int kill_perdb(void)
   {int ok;
    char *db, *bf;

    ok = TRUE;
    if (st.launched == TRUE)
       {db = cgetenv(FALSE, "PERDB_PATH");
	bf = run(TRUE, "perdb -f %s -l quit:", db);
	ok = (bf != NULL);};

    return(ok);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - give help message */

static void help(void)
   {

    printf("\n");
    printf("Usage: pact-config [-a] [-as] [-c] [-f <db>] [-F] [-g] [-i <directory>] [-l] [-o] [-p] [-s <sysid>] [-v] -n | <cfg>\n");
    printf("\n");
    printf("             -a      do NOT perform PACT-ANALYZE step\n");
    printf("             -as     run the database asynchronously\n");
    printf("             -c      create missing directories for -i option\n");
    printf("             -f      path to database\n");
    printf("             -F      do builds in /tmp for speed\n");
    printf("             -g      build debuggable version\n");
    printf("             -i      base installation directory (default /usr/local)\n");
    printf("             -l      append to the log file\n");
    printf("             -n      do an auto-configuration\n");
    printf("             -o      build optimized version\n");
    printf("             -p      build profiling version\n");
    printf("             -s      explicit system id\n");
    printf("             -v      verbose on (diagnostic)\n");
    printf("             <cfg>   the config file for the installation\n");
    printf("\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it out here */

int main(int c, char **v)
   {int i, append, havedb;
    char base[MAXLINE];
    char *s, *strct;

    if (c == 0)
       {help();
	return(1);};

/* locate the tools needed for subshells */
    build_path(NULL,
	       "sed", "grep", "awk", "sort",
	       "ls", "cp", "rm", "mv",
	       "find", "chmod", "cat",
	       "env", "mkdir", "nm", "perdb",
	       NULL);

    havedb = launch_perdb(c, v);

/* technically these are set by 'dsys config' */
    cinitenv("DbgOpt", "-g");
    cinitenv("PACTVer", "debug");

/* NOTE: because of OSX's nefarious automounter we have to get the current
 * directory this way (rather that via the getcwd library call) so that
 * we get the same value that shell scripts get in other parts of the
 * config process - and consistency is essential
 */
    strncpy(st.dir.mng, run(BOTH, "pwd"), MAXLINE);

    strcpy(base, path_head(st.dir.mng));
    strcpy(st.cfgf, "DEFAULT");

    strct  = "0";
    append = FALSE;

    st.na   = c - 1;
    st.args = v + 1;

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-strict") == 0)
	    strct = v[++i];

	 else if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'a':
		       if (v[i][2] == 's')
			  async_srv = TRUE;
		       else
			  st.analyzep = TRUE;
                       break;

                 case 'c':
                      st.create_dirs = TRUE;
                      break;
 
                 case 'F':
                      st.tmp_dirp = FALSE;
                      break;
 
                 case 'g':
                      st.abs_deb = TRUE;
                      break;
 
                 case 'i':
		      s = v[++i];
                      if (LAST_CHAR(s) == '/')
			 LAST_CHAR(s) = '\0';
                      dbset(NULL, "InstBase", s);
                      dbset(NULL, "PubInc",   "-I%s/include", s);
                      dbset(NULL, "PubLib",   "-L%s/lib", s);
                      st.installp = TRUE;
                      break;
 
                 case 'l':
                      append   = TRUE;
                      dbg_sock = TRUE;
                      dbg_db   = TRUE;
                      break;
 
                 case 'n':
                      st.configp = TRUE;
                      break;
 
                 case 'o':
                      st.abs_opt = TRUE;
                      break;
 
                 case 'p':
                      st.profilep = TRUE;
                      break;
 
                 case 's':
                      strncpy(st.system, v[++i], MAXLINE);
                      break;
 
                 case 'v':
                      st.verbose = TRUE;
                      break;};}
         else if (v[i][0] == '+')
            {switch (v[i][1])
                {case 'F':
                      st.tmp_dirp = TRUE;
                      break;};}

         else
            strcpy(st.cfgf, v[i]);};

    dbset(NULL, "STRICT", strct);

    push_file(st.cfgf, STACK_FILE);
    strcpy(st.cfgf, st.fstck.file[st.fstck.n-1].name);
    pop_file();

    init_session(base, append);

/* make config directory */
    snprintf(st.dir.cfg, MAXLINE, "cfg-%s", st.system);
    run(BOTH, "rm -rf %s", st.dir.cfg);
    run(BOTH, "mkdir %s", st.dir.cfg);

    if (st.configp == FALSE)
       {if (file_exists("analyze/program-init") == TRUE)
	   read_config("program-init", TRUE);

	read_config(st.cfgf, FALSE);

	pco_save_db("inp");

	noted(Log, "");}
    else
       pco_load_db("inp");

    check_dir();

    if (st.analyzep == FALSE)
       analyze_config(base);

    summarize_config();

    finish_config(base);

    separator(Log);
    run(BOTH, "rm -rf %s", st.dir.cfg);

    LOG_OFF;

    if (havedb == TRUE)
       kill_perdb();

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

