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
	FALSE, FALSE, };

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
	   strncpy(lfile, run(BOTH, "echo \"%s\"", s), MAXLINE);
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

    csetenv("CurrGrp", ge->item);
    csetenv("CurrTool", "");

    note(Log, TRUE, "--- end");

    return;}

/*--------------------------------------------------------------------------*/

/*                                  EMITTERS                                */

/*--------------------------------------------------------------------------*/

/* WRITE_CLASS - write a text representation of a class CLSS of type CTYPE
 *             - look for sub-class SUB of type STYPE
 */

static int write_class(FILE *out, char *clss, char *ctype,
		       char *sub, char *stype, char *ind)
   {int i, n, ic, nc, global;
    char cl[MAXLINE], fmt[MAXLINE];
    char *c, *pc, *t, *var, *val, *dlm;
    char **vars, **vals;

    strncpy(cl, clss, MAXLINE);
    for (c = cl; c != NULL; c = pc)
        {pc = strchr(c, ' ');
         if (pc == NULL)
            {if (c[0] == '\0')
                break;}
         else
            *pc++ = '\0';

	 global = (strcmp(c, "Glb") == 0);

	 if (global == TRUE)
	    {t   = cgetenv(TRUE, "Globals");
	     dlm = " ";}
         else
	    {fprintf(out, "%s%s %s {\n", ind, ctype, c);
	     t   = run(BOTH, "env | egrep '^%s_' | sort", c);
	     dlm = "\n";};

	 n = 0;
	 FOREACH(entry, t, dlm)
	    n++;
	 ENDFOR

	 if ((global == TRUE) && (n > 0))
	    fprintf(out, "# Global variables\n");

	 vars = MAKE_N(char *, n);
	 vals = MAKE_N(char *, n);
	 if ((vars != NULL) && (vals != NULL))
	    {i  = 0;
	     nc = 0;
	     FOREACH(entry, t, dlm)
	        if (global == TRUE)
		   {var = entry;
		    val = cgetenv(TRUE, var);}
		else
		   {var = entry + strlen(c) + 1;
		    val = strchr(var, '=');
		    if (val != NULL)
		       *val++ = '\0';};

	        ic = strlen(var);
   	        nc = max(nc, ic);

	        vars[i] = var;
	        vals[i] = val;
	        i++;
	     ENDFOR

	     if (global == TRUE)
	        snprintf(fmt, MAXLINE, "%%s%%-%ds = %%s\n", nc);
	     else
	        snprintf(fmt, MAXLINE, "%%s   %%-%ds = %%s\n", nc);
	     for (i = 0; i < n; i++)
	         fprintf(out, fmt, ind, vars[i], vals[i]);

	     if (global == TRUE)
	        fprintf(out, "\n");
	     else
	        fprintf(out, "}\n\n");};

	 FREE(vars);
	 FREE(vals);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_DB - write a text representation of the configuration database */

static int write_db(char *name)
   {int rv;
    FILE *out;

    if (name != NULL)
       out = open_file("w", name);
    else
       out = stdout;

    rv = write_class(out, st.def_tools, "Tool", NULL, NULL, "");
    rv = write_class(out, st.def_groups, "Group", st.def_tools, "Tool", "");

    if (name != NULL)
       fclose(out);

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
    vl = expand(s, LRG, NULL);

    note(fsh, TRUE,  "export %s=%s",    var, vl);
    note(fcsh, TRUE, "setenv %s %s",    var, vl);
    note(fdk, TRUE,  "dk_setenv %s %s", var, vl);
    note(fmd, TRUE,  "setenv %s %s;",   var, vl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_ENVF - write out the env file because other scripts need to source it
 *            - write it for CSH or SH families
 */

static void write_envf(int lnotice)
   {int i, n;
    char s[MAXLINE];
    char *t, *var, *val;
    char *site[] = { "SYS_TYPE", "SYS_SITE", "DAS_ROOT", "DAI_ROOT" };
    FILE *fcsh, *fsh, *fdk, *fmd;

    separator(Log);
    if (lnotice == TRUE)
       {noted(Log, "   Environment setup files - env-pact.csh, env-pact.sh, env-pact.dk, and env-pact.mod");
        note(Log, TRUE, "");}
    else
       {note(Log, TRUE, "   Environment setup files - env-pact.csh, env-pact.sh, env-pact.dk, and env-pact.mod");
        note(Log, TRUE, "");};

    fcsh = open_file("w", "%s/env-pact.csh", st.dir.inc);
    fsh  = open_file("w", "%s/env-pact.sh", st.dir.inc);
    fdk  = open_file("w", "%s/env-pact.dk", st.dir.inc);
    fmd  = open_file("w", "%s/env-pact.mod", st.dir.inc);

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
        env_out(fsh, fcsh, fdk, fmd, site[i], cgetenv(TRUE, site[i]));

    fseek(st.aux.SEF, 0, SEEK_SET);
    while (fgets(s, MAXLINE, st.aux.SEF) != NULL)
       {LAST_CHAR(s) = '\0';
	if (strncmp(s, "PATH", 4) != 0)
	   {var = strtok(s, " ");
	    val = strtok(NULL, "\n");
	    env_out(fsh, fcsh, fdk, fmd, var, val);}

/* handle PATH specially - just gather everything that is not $PATH or ${PATH} */
	else if (strlen(s) > 3)
	   push_path(APPEND, epath, s+4);};

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
    note(fmd, TRUE, "prepend-path MANPATH %s/man;", run(FALSE, "echo %s", st.dir.root));

    {char lPython[MAXLINE];

     if (st.have_python == TRUE)
        {snprintf(lPython, MAXLINE, "%s/python%s",
		  st.dir.lib, cgetenv(TRUE, "PyVers"));
	 t = getenv("PYTHONPATH");
	 if (t != NULL)
	    {if (strstr(t, lPython) == NULL)
	        csetenv("PYTHONPATH", "%s:$PYTHONPATH", lPython);}
	 else
	    csetenv("PYTHONPATH", lPython);

	 t = getenv("PYTHONPATH");
	 note(fcsh, TRUE, "setenv PYTHONPATH %s", t);
	 note(fsh, TRUE,  "export PYTHONPATH=%s", t);
	 note(fdk, TRUE,  "dk_setenv PYTHONPATH %s", t);
	 note(fmd, TRUE,  "setenv PYTHONPATH %s;", t);};};

/* emit PATH settings */
    if (epath[0] != '\0')
       {FOREACH(u, epath, ":\n")
	   note(fdk, TRUE, "dk_alter PATH %s", u);
	   note(fmd, TRUE, "prepend-path PATH    %s;", run(FALSE, "echo %s", u));
	ENDFOR
        note(fcsh, TRUE, "setenv PATH    %s/bin:%s:$PATH", st.dir.root, epath);
        note(fsh, TRUE,  "export PATH=%s/bin:%s:$PATH", st.dir.root, epath);}
    else
       {note(fcsh, TRUE, "setenv PATH    %s/bin:$PATH", st.dir.root);
        note(fsh, TRUE, "export PATH=%s/bin:$PATH", st.dir.root);};

    note(fdk, TRUE, "dk_alter PATH    %s/bin", st.dir.root);
    note(fmd, TRUE, "prepend-path PATH    %s/bin;", run(FALSE, "echo %s", st.dir.root));
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
    cat(Log, 0, -1, "%s/env-pact.sh", st.dir.inc);
    cat(Log, 0, -1, "%s/env-pact.dk", st.dir.inc);
    cat(Log, 0, -1, "%s/env-pact.mod", st.dir.inc);

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
			   "scheme", "man", "man/man1"};

    n   = sizeof(dlst)/sizeof(char *);
    sib = cgetenv(TRUE, "InstBase");

    if (st.create_dirs == TRUE)
       {Created[0] = '\0';
        if (strcmp(sib, "none") != 0)
           {for (i = 0; i < n; i++)
                {dir = dlst[i];
                 if (dir_exists("%s/%s", sib, dir) == FALSE)
                    {run(BOTH, "mkdir -p %s/%s", sib, dir);
                     push_tok(Created, LRG, ' ', "%s/%s", sib, dir);};};};

        if (Created[0] != '\0')
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

        if (Missing[0] != '\0')
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
   {int i, ok;
    char *t, *pt, *vr, *avl, *arg, *opt;

    t = STRSAVE(s+1);

    ok  = FALSE;
    avl = "off";
    for (pt = t; TRUE; pt = NULL)
        {vr = trim(strtok(pt, "=\n"), BOTH, " \t");
	 if (vr == NULL)
	    break;

/* parse the _cmd_ key */
	 else if (strcmp(vr, "_cmd_") == 0)
	    {arg = trim(strtok(NULL, ";"), BOTH, " \t");
	     opt = strchr(arg, '@');
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
	    {arg = trim(strtok(NULL, ";"), BOTH, " \t");
	     avl = cgetenv(TRUE, arg);}

/* select the value */
	 else
	    {opt = trim(strtok(NULL, ";]"), BOTH, " \t");

/* treat single '*' specially since match will automatically succeed */
	     if (strcmp(vr, "*") == 0)
	        {if (strcmp(avl, "off") != 0)
		    {strcpy(s, opt);
		     LAST_CHAR(s) = '\0';
		     strcat(s, avl);
		     ok = TRUE;};}

	     else if ((strcmp(avl, vr) == 0) || (match(avl, vr) == TRUE))
	        {strcpy(s, opt);
	         ok = TRUE;};};};

    if (ok == FALSE)
       s[0] = '\0';

    FREE(t);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_LINE - parse the next line from the input */

static void parse_line(char *s, char *key, char *oper, char *value)
   {char t[MAXLINE];
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
	       {read_line(t, MAXLINE);
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

        if ((line[0] == '\0') || (strcmp(line, "++end++") == 0))
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

	if ((strcmp(line, "end") == 0) || (line[0] == '\0'))
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

    csetenv("Host",    st.host);
    csetenv("Arch",    st.arch);
    csetenv("System",  st.system);
    csetenv("Sys",     st.sys);
    csetenv("Base",    base);
    csetenv("RootDir", st.dir.root);
    csetenv("AnaDir",  "%s/analyze", st.dir.mng);
    csetenv("Log",     st.logf);
    csetenv("ALog",    alog);

    csetenv("AbsoluteDeb", st.abs_deb ? "TRUE" : "FALSE");
    csetenv("AbsoluteOpt", st.abs_opt ? "TRUE" : "FALSE");
    csetenv("Profile",     st.profilep ? "TRUE" : "FALSE");
    csetenv("UseTmpDir",   st.tmp_dirp ? "TRUE" : "FALSE");

    if (strncmp(st.os, "CYGWIN", 6) == 0)
       st.os[6] = '\0';

    csetenv("HostOS",      st.os);
    csetenv("HostOSRel",   st.osrel);

    if (strcmp(st.os, "Windows_NT") == 0)
       csetenv("CDecls", "TRUE");
    else
       csetenv("CDecls", "FALSE");

    csetenv("ANSI",   "ANSI");
    csetenv("MeOnly", "TRUE");

/* initialization of non-graphics flags */
    if (strcmp(st.os, "AIX") == 0)
       csetenv("NM", "/usr/bin/nm -g -X %s", cgetenv(TRUE, "Bits"));
    else
       csetenv("NM", "%s -g", cwhich("nm"));

    cinitenv("CC_Version",  "");
    cinitenv("FC_Version",  "");
    cinitenv("LD_Version",  "");

    cinitenv("CC_Inc",    "");
    cinitenv("CC_Flags",  "");
    cinitenv("LD_Lib",    "");
    cinitenv("LD_RPath",  "");
    cinitenv("LD_Flags",  "");

    cinitenv("CPU",       "");
    cinitenv("FPU",       "");
    cinitenv("BE",        "");

/* parallel front end */
    cinitenv("PFE",       "%s/do-run -m", st.dir.bin);

/* cross compile front end */
    if (cmpenv("CROSS_COMPILE", "FALSE") != 0)
       cinitenv("CFE", "%s/do-run -m", st.dir.bin);
    else
       cinitenv("CFE", "");

/* initialization of graphics flags */
    cinitenv("GSYS",       "X");
    cinitenv("Std_UseX",   "TRUE");
    cinitenv("Std_UseOGL", "FALSE");
    cinitenv("Std_UseQD",  "FALSE");

    cinitenv("CC_MDGInc",       "");
    cinitenv("LD_MDGLib",       "");
    cinitenv("GraphicsDevices", "PS CGM MPG PNG JPG");
    cinitenv("GraphicsFlag",    "");

/* initialize Cfg group flags */
    cinitenv("Cfg_CC_Flags",  cgetenv(TRUE, "CC_Flags"));
    cinitenv("Cfg_CC_Inc",    cgetenv(TRUE, "CC_Inc"));
    cinitenv("Cfg_LD_RPath",  cgetenv(TRUE, "LD_RPath"));
    cinitenv("Cfg_LD_Flags",  cgetenv(TRUE, "LD_Flags"));
    cinitenv("Cfg_LD_Lib",    cgetenv(TRUE, "LD_Lib"));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_OUTPUT_ENV - setup the environment for programs which write
 *                  - configured files
 */

static void setup_output_env(char *base)
   {char *rv;

/* close any open intermediate files and export their names */
    csetenv("DPFile", st.aux.dpfn);
    if (st.aux.DPF != NULL)
       fclose(st.aux.DPF);

    csetenv("CEFile", st.aux.cefn);
    if (st.aux.CEF != NULL)
       fclose(st.aux.CEF);

    csetenv("MVFile", st.aux.mvfn);
    if (st.aux.MVF != NULL)
       fclose(st.aux.MVF);

    csetenv("URFile", st.aux.urfn);
    if (st.aux.URF != NULL)
       fclose(st.aux.URF);

/* remove duplicate tokens in selected lists */
    csetenv("CC_MDGInc", unique(cgetenv(TRUE, "CC_MDGInc"), FALSE, ' '));
    csetenv("CC_Inc",    unique(cgetenv(TRUE, "CC_Inc"), FALSE, ' '));
    csetenv("LD_Lib",    unique(cgetenv(TRUE, "LD_Lib"), FALSE, ' '));

/* NOTE: for OSX this would reduce -framework FOO -framework BAR
 * to -framework FOO BAR which is not legal
 */
    if (strcmp(st.os, "Darwin") != 0)
       csetenv("LD_MDGLib", unique(cgetenv(TRUE, "LD_MDGLib"), FALSE, ' '));

    csetenv("BinDir",  st.dir.bin);
    csetenv("IncDir",  st.dir.inc);
    csetenv("ScrDir",  st.dir.scr);
    csetenv("SchDir",  st.dir.sch);
    csetenv("CfgDir",  st.dir.cfg);

    rv = cgetenv(TRUE, "HavePython");
    if (rv == NULL)
       {if (strcmp(rv, "FALSE") == 0)
	   csetenv("HavePython", "FALSE");
	else
	   csetenv("HavePython",  "TRUE");};

    csetenv("Load",        st.loadp ? "TRUE" : "FALSE");
    csetenv("NoExe",       st.exep ? "TRUE" : "FALSE");
    csetenv("ConfigVars",  st.cfgv);
    csetenv("DefGroups",   st.def_groups);
    csetenv("ConfigFile",  st.cfgf);

    csetenv("CCP",     st.rules.ccp);
    csetenv("CCObj",   st.rules.co);
    csetenv("CCArc",   st.rules.ca);
    csetenv("LexObj",  st.rules.lo);
    csetenv("LexArc",  st.rules.la);
    csetenv("LexC",    st.rules.lc);
    csetenv("YaccObj", st.rules.yo);
    csetenv("YaccArc", st.rules.ya);
    csetenv("YaccC",   st.rules.yc);
    csetenv("FCObj",   st.rules.fo);
    csetenv("FCArc",   st.rules.fa);

    csetenv("CCObj_BP",   st.rules.co_bp);
    csetenv("CCArc_BP",   st.rules.ca_bp);
    csetenv("LexObj_BP",  st.rules.lo_bp);
    csetenv("LexArc_BP",  st.rules.la_bp);
    csetenv("YaccObj_BP", st.rules.yo_bp);
    csetenv("YaccArc_BP", st.rules.ya_bp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DEFAULT_VAR - setup default variable values */

static void default_var(char *base)
   {char cmd[MAXLINE];

    if (cdefenv("USER") == FALSE)
       {if (cdefenv("LOGNAME") == FALSE)
	   csetenv("USER", "anonymous");
        else
	   csetenv("USER", cgetenv(TRUE, "LOGNAME"));};

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
    push_tok(st.toolv, MAXLINE, ' ', "MDGInc");
    push_tok(st.toolv, MAXLINE, ' ', "MDGLib");
    push_tok(st.toolv, MAXLINE, ' ', "DPInc");
    push_tok(st.toolv, MAXLINE, ' ', "DPLib");
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
    push_tok(st.cfgv, MAXLINE, ' ', "CC_DPInc");
    push_tok(st.cfgv, MAXLINE, ' ', "CC_MDGInc");
    push_tok(st.cfgv, MAXLINE, ' ', "FC_Exe");
    push_tok(st.cfgv, MAXLINE, ' ', "FC_Linker");
    push_tok(st.cfgv, MAXLINE, ' ', "FC_Flags");
    push_tok(st.cfgv, MAXLINE, ' ', "FC_Debug");
    push_tok(st.cfgv, MAXLINE, ' ', "FC_Optimize");
    push_tok(st.cfgv, MAXLINE, ' ', "LD_Exe");
    push_tok(st.cfgv, MAXLINE, ' ', "LD_RPath");
    push_tok(st.cfgv, MAXLINE, ' ', "LD_Flags");
    push_tok(st.cfgv, MAXLINE, ' ', "LD_Lib");
    push_tok(st.cfgv, MAXLINE, ' ', "LD_MDGLib");
    push_tok(st.cfgv, MAXLINE, ' ', "CC_Shared");
    push_tok(st.cfgv, MAXLINE, ' ', "LD_Shared");

    strcpy(st.sys, path_tail(st.cfgf));

    unamef(st.host,      MAXLINE, "n");
    unamef(st.os,    MAXLINE, "s");
    unamef(st.osrel, MAXLINE, "r");
    unamef(st.hw,    MAXLINE, "m");

    snprintf(st.dir.scr, MAXLINE, "%s/scripts", base);
    snprintf(cmd, MAXLINE, "%s/system-id", st.dir.scr);
    strncpy(st.arch, run(BOTH, cmd), MAXLINE);

/* check variables which may have been initialized from the command line */
    if (st.system[0] == '\0')
       strncpy(st.system, run(BOTH, "%s/cfgman use", st.dir.mng), MAXLINE);

    cinitenv("CfgMan",        "%s/cfgman", run(BOTH, "pwd"));
    cinitenv("Globals",       "");
    cinitenv("MngDir",        st.dir.mng);
    cinitenv("InstBase",      "none");
    cinitenv("PubInc",        "");
    cinitenv("PubLib",        "");
    cinitenv("ScmDir",        "scheme");
    cinitenv("ManDir",        "man/man1");
    cinitenv("CROSS_COMPILE", "FALSE");

/* global variables */
    snprintf(st.dir.root, MAXLINE, "%s/dev/%s",       base, st.system);
    snprintf(st.dir.inc,  MAXLINE, "%s/include",      st.dir.root);
    snprintf(st.env_csh,  MAXLINE, "%s/env-pact.csh", st.dir.inc);
    snprintf(st.env_sh,   MAXLINE, "%s/env-pact.sh",  st.dir.inc);
    snprintf(st.env_dk,   MAXLINE, "%s/env-pact.dk",  st.dir.inc);

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
	     "\t@(%s ; \\\n          %s)\n",
	     "echo \"${FCAnnounce} -c $<\"",
	     "${FC} -c $< -o $@");

    snprintf(st.rules.fa, LRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"${FCAnnounce} -c $<\"",
	     cd, rm, tc,
	     "${FC} -c ${PACTSrcDir}/$< -o $*.o",
	     ar,
	     "${RM} $*.o 2>> errlog");

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

/* locate the tools needed for subshells */
    build_path(NULL,
	       "sed", "grep", "awk", "sort",
	       "ls", "cp", "rm", "mv",
	       "find", "chmod", "cat",
	       "env", "mkdir", "nm",
	       NULL);

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
    char s[LRG];
    char *prfx, *t;
    gt_entry *ge;

    ge   = st.gstck.st + st.gstck.n - 1;
    prfx = ge->item;

/* attach the current group suffix */
    if (strcmp(prfx, "Glb") == 0)
       {snprintf(s, LRG, "%s %s", cgetenv(TRUE, "Globals"), var);
	csetenv("Globals", unique(s, FALSE, ' '));
	strncpy(fvar, var, MAXLINE);}
    else
       snprintf(fvar, MAXLINE, "%s_%s", prfx, var);

    clean_space(val);

    if ((strcmp(oper, "+=") == 0) || (strcmp(oper, "=+") == 0))
       {if (cdefenv(fvar) == FALSE)
           {note(Log, TRUE, "Variable %s does not exist changing %s to =",
                 fvar, oper);
            oper = "=";};};

/* set variable */
    if (strcmp(oper, "=") == 0)
       {t = run(FALSE, "echo %s", val);
        if ((t != NULL) && (*t != '\0'))
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
       {t = run(FALSE, "echo %s", val);
        if ((t != NULL) && (*t != '\0'))
           {strncpy(lval, t, MAXLINE);
            strncpy(nval, run(FALSE, "echo $%s", fvar), MAXLINE);
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
       {t = run(FALSE, "echo %s", val);
        if ((t != NULL) && (*t != '\0'))
           {strncpy(lval, t, MAXLINE);
            strncpy(nval, run(FALSE, "echo $%s", fvar), MAXLINE);
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
       {strncpy(nval, run(FALSE, "echo $%s", fvar), MAXLINE);
        note(Log, TRUE, "Change    |%s|", fvar);
        note(Log, TRUE, "Remove    |%s|", val);
        note(Log, TRUE, "Old value |%s|", nval);

        strncpy(nval, run(FALSE, "echo %s | sed 's|%s||'", nval, val), MAXLINE);
        note(Log, TRUE, "New value |%s|", nval);

        note(Log, TRUE, "Change expression |setenv %s %s|", fvar, nval);
	dbset(NULL, fvar, nval);}

/* set value only if undefined */
    else if (strcmp(oper, "=?") == 0)

/* check value of val
 * we want things such as "foo =? $bar" to do nothing if
 * "$bar" is not defined
 */
       {t = run(FALSE, "echo %s", val);
        if ((t != NULL) && (*t != '\0'))
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
		  sg, cgetenv(FALSE, "CurrGrp"));
             FOREACH(var, st.cfgv, " ")
                snprintf(nvr, MAXLINE, "%s_%s", sg, var);
                if (cdefenv(nvr) == TRUE)
                   {val = cgetenv(TRUE, nvr);
                    if (strcmp(var, "Exe") == 0)
                       set_var(FALSE, var, "=", val);
                    else
                       set_var(FALSE, var, oper, val);};
             ENDFOR
             break;

/* fill out a tool */
        case STACK_TOOL:
             if (cmpenv("CurrTool", "") == 0)
                {note(Log, TRUE, "Use tool %s to fill group %s",
		      sg, cgetenv(FALSE, "CurrGrp"));
                 FOREACH(var, st.toolv, " ")
                    snprintf(nvr, MAXLINE, "%s_%s", sg, var);
                    if (cdefenv(nvr) == TRUE)
                       {val = cgetenv(TRUE, nvr);
                        set_var(FALSE, nvr, oper, val);};
                 ENDFOR}
             else
                {note(Log, TRUE, "Use tool %s to fill tool %s",
		      sg, cgetenv(FALSE, "CurrTool"));
                 FOREACH(var, st.toolv, " ")
                    snprintf(nvr, MAXLINE, "%s_%s", sg, var);
                    if (cdefenv(nvr) == TRUE)
                       {val = cgetenv(TRUE, nvr);
                        set_var(FALSE, var, oper, val);};
                 ENDFOR};
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_CONFIG - read the user configuration file */

static void read_config(char *cfg, int quiet)
   {int il;
    char line[MAXLINE], key[MAXLINE], oper[MAXLINE], value[MAXLINE];
    char *path;
    gt_entry *ge;

    separator(Log);
    if (quiet == TRUE)
       {note(Log, TRUE, "");
	note(Log, TRUE, "Reading configuration file %s", cfg);}
    else
       {noted(Log, "");
	noted(Log, "Reading configuration file %s", cfg);};
    note(Log, TRUE, "");

    note(Log, TRUE, "");
    note(Log, TRUE, "Strict checking level is %s", cgetenv(TRUE, "STRICT"));
    note(Log, TRUE, "");
    separator(Log);

    path = push_file(cfg, STACK_FILE);

/* toplevel loop over the input to define configuration parameters */
    for (il = 0; TRUE; il++)
        {if (cdefenv("STOP") == TRUE)
	    {noted(Log, " ");
	     exit(1);};

	 read_line(line, MAXLINE);

	 if (line[0] == '\0')
            continue;
	 else if (strcmp(line, "++end++") == 0)
            break;

	 parse_line(line, key, oper, value);

	 ge = st.gstck.st + st.gstck.n - 1;

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
	     s = run(FALSE, "echo %s", value);
	     dbset(NULL, oper, s);}

	 else if (strcmp(key, "parent") == 0)
	    {char *s, *var, *val;

	     s    = line + 7;
	     var  = s + strspn(s, " \t");
	     s    = var + strcspn(var, "(");
	     *s++ = '\0';
	     val  = s;
	     s    = val + strcspn(val, ")");
	     *s++ = '\0';
	     if (strcmp(var, "PATH") == 0)
	        {push_path(APPEND, epath, val);
		 s = run(FALSE, "echo %s", val);
		 dbset(NULL, var, s);}
	     else
	        dbset(NULL, var, val);

	     if (st.phase == PHASE_READ)
	        note(st.aux.SEF, TRUE, "%s \"%s\"", var, val);
	     note(Log, TRUE, "Command: setenv %s %s", var, val);}

/* handle Tool specifications */
	 else if (strcmp(key, "Tool") == 0)
	    {note(Log, TRUE, "--- tool %s", oper);
	     csetenv("CurrTool", oper);
	     note(Log, TRUE, "Defining tool %s", oper);
	     push_struct(oper, st.def_tools, STACK_TOOL);}

/* handle Group specifications */
	 else if (strcmp(key, "Group") == 0)
	    {note(Log, TRUE, "--- group %s", oper);
	     csetenv("CurrGrp", oper);
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
   {char name[MAXLINE];

    if (file_executable("analyze/summary") == TRUE)
       printf("%s\n", run(BOTH, "analyze/summary"));

    snprintf(name, MAXLINE, "%s/config.db", st.dir.inc);
    write_db(name);

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
    if (cmpenv("PFE", "mppexec") == 0)
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

/* HELP - give help message */

static void help(void)
   {

    printf("\n");
    printf("Usage: pact-config [-a] [-c] [-f <db>] [-F] [-g] [-i <directory>] [-l] [-o] [-p] [-s <sysid>] [-v] -n | <cfg>\n");
    printf("\n");
    printf("             -a      do NOT perform PACT-ANALYZE step\n");
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
   {int i, append;
    char base[MAXLINE], name[MAXLINE];
    char *s;

    if (c == 0)
       {help();
	return(1);};

/* NOTE: because of OSX's nefarious automounter we have to get the current
 * directory this way (rather that via the getcwd library call) so that
 * we get the same value that shell scripts get in other parts of the
 * config process - and consistency is essential
 */
    strncpy(st.dir.mng, run(BOTH, "pwd"), MAXLINE);

    strcpy(base, path_head(st.dir.mng));
    strcpy(st.cfgf, "DEFAULT");
    csetenv("STRICT", "0");

    append = FALSE;

    st.na   = c - 1;
    st.args = v + 1;

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-strict") == 0)
	    csetenv("STRICT", v[++i]);

	 else if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'a':
                       st.analyzep = TRUE;
                       break;

                 case 'c':
                      st.create_dirs = TRUE;
                      break;
 
                 case 'f':
                      csetenv("PERDB_PATH", v[++i]);
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
                      csetenv("InstBase", s);
                      csetenv("PubInc",   "-I%s/include", s);
                      csetenv("PubLib",   "-L%s/lib", s);
                      st.installp = TRUE;
                      break;
 
                 case 'l':
                      append = TRUE;
                      dbg_fifo = TRUE;
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

	snprintf(name, MAXLINE, "%s/config.gen", st.dir.inc);
	write_db(name);

	noted(Log, "");};

    check_dir();

    if (st.analyzep == FALSE)
       analyze_config(base);

    summarize_config();

    finish_config(base);

    separator(Log);
    run(BOTH, "rm -rf %s", st.dir.cfg);

    LOG_OFF;

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

