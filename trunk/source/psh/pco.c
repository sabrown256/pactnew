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
   {char root[BFLRG];
    char bin[BFLRG];
    char cfg[BFLRG];
    char inc[BFLRG];
    char etc[BFLRG];
    char lib[BFLRG];
    char mng[BFLRG];
    char sch[BFLRG];
    char scr[BFLRG];};

struct s_ruledes
   {char ccp[BFLRG];            /* .c -> .i */
    char co[BFLRG];             /* .c -> .o */
    char ca[BFLRG];             /* .c -> .a */
    char fo[BFLRG];             /* .f -> .o */
    char fa[BFLRG];             /* .f -> .a */
    char lo[BFLRG];             /* .l -> .o */
    char la[BFLRG];             /* .l -> .a */
    char lc[BFLRG];             /* .l -> .c */
    char yo[BFLRG];             /* .y -> .o */
    char ya[BFLRG];             /* .y -> .a */
    char yc[BFLRG];             /* .y -> .c */
    char th[BFLRG];             /* .t -> .h */
    char co_bp[BFLRG];          /* bad pragma versions */
    char ca_bp[BFLRG];
    char lo_bp[BFLRG];
    char la_bp[BFLRG];
    char yo_bp[BFLRG];
    char ya_bp[BFLRG];};

struct s_auxfdes
   {char cefn[BFLRG];       /* config defines for C */
    FILE *CEF;
    char dpfn[BFLRG];       /* config variable for distributed parallel */
    FILE *DPF;
    char mvfn[BFLRG];       /* config variables for make */
    FILE *MVF;
    char urfn[BFLRG];       /* config rules for make */
    FILE *URF;
    FILE *SEF;};

struct s_state
   {int abs_deb;
    int abs_opt;
    int create_dirs;
    int have_python;
    int have_db;
    int installp;

    int loadp;
    int analyzep;
    int exep;
    int phase;
    int profilep;

    int tmp_dirp;
    int verbose;
    int launched;               /* TRUE iff PCO launched PERDB
                                 * FALSE if PERDB was already running */

    int na;
    char **args;
    char *db;

    file_stack fstck;
    gt_stack gstck;
    dirdes dir;
    ruledes rules;
    auxfdes aux;

    char cfgf[BFLRG];          /* config file name */
    char logf[BFLRG];          /* log file name */

    char cfgv[BFLRG];          /* config variables */
    char def_tools[BFLRG];     /* default tools */
    char def_groups[BFLRG];    /* defaults groups */
    char toolv[BFLRG];         /* tool variables */

    char env_csh[BFLRG];
    char env_sh[BFLRG];
    char env_dk[BFLRG];
    char env_mdl[BFLRG];

    char arch[BFLRG];
    char host[BFLRG];
    char os[BFLRG];
    char osrel[BFLRG];
    char hw[BFLRG];
    char sys[BFLRG];
    char system[BFLRG];};

static state
 st = { FALSE, FALSE, FALSE, FALSE, FALSE,
        TRUE, FALSE, FALSE, PHASE_READ, FALSE,
	FALSE, FALSE, FALSE, 0, };

static void
 parse_line(client *cl, char *s, char *key, char *oper, char *value, int nc);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ECHO - run echo command
 *      - only exec shell if necessary
 */

static char *echo(int log, char *fmt, ...)
   {char s[BFLRG];
    char *rv;

    VA_START(fmt);
    VSNPRINTF(s, BFLRG, fmt);
    VA_END;

    if (strpbrk(s, "$*|{}[]\"'") == NULL)
       rv = STRSAVE(trim(s, BOTH, " \t\n"));
    else
       rv = STRSAVE(run(BOTH, "echo %s", s));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PUSH_FILE - push S onto the file stack */

static char *push_file(char *s, int itype)
   {int id, n, nd, ok;
    file_entry *se;
    static char lfile[BFLRG], t[BFLRG];
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
	        {snprintf(lfile, BFLRG, "%s/%s", places[id], s);
		 ok = file_exists("%s/%s", st.dir.mng, lfile);};};

	if (ok == FALSE)
	   {noted(Log, "***> Cannot find file %s\n", s);
	    ok = FALSE;};}

    else if (itype == STACK_PROCESS)
       {if (strchr(s, '$') != NULL)
	   {char *p;

	    p = echo(BOTH, "\"%s\"", s);
	    if (p != NULL)
	       {nstrncpy(lfile, BFLRG, p, -1);
		FREE(p);};}
	else
	   nstrncpy(lfile, BFLRG, s, -1);};

    if ((ok == TRUE) && (se != NULL))
       {se->itype = itype;
	se->iline = 0;
	se->name  = STRSAVE(lfile);

	if (itype == STACK_FILE)
	   {if (lfile[0] != '/')
               snprintf(t, BFLRG, "%s/%s", st.dir.mng, lfile);
	    else
	       nstrncpy(t, BFLRG, lfile, -1);
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
    char t[BFLRG], lst[BFLRG];
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
	   {nstrcat(t, BFLRG, " ");
	    nstrcat(t, BFLRG, p);};
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

static void pop_struct(client *cl)
   {int n;
    gt_entry *ge;

/* release the current stack entry */
    n  = --st.gstck.n;
    ge = st.gstck.st + n;

    FREE(ge->item);

/* reset the current group or tool */
    n  = st.gstck.n - 1;
    ge = st.gstck.st + n;

    dbset(cl, "CurrGrp", ge->item);
    dbset(cl, "CurrTool", "");

    note(Log, TRUE, "--- end");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RESET_ENV - check for and act on specifications to modify the environment
 *           - clear out all environment variables except for PATH
 *           - and those specified in the optional file
 */

static int reset_env(int c, char **v)
   {int i, rv;
    char *p, **sa, **except;

    sa = NULL;
    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-env") == 0)
	    {sa = file_text(FALSE, v[++i]);
	     break;};};

    if (sa != NULL)
       {except = NULL;

	for (i = 0; sa[i] != NULL; i++)
	    {p = sa[i];
	     if ((IS_NULL(p) == FALSE) && (p[0] != '#'))
	        except = lst_push(except, trim(p, BOTH, " \t\n"));};
	free_strings(sa);

	if (except != NULL)
	   {except = lst_push(except, NULL);

	    rv = cclearenv(except);

	    free_strings(except);};};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                                  EMITTERS                                */

/*--------------------------------------------------------------------------*/

/* WRITE_CLASS_PCO - write a PCO form text representation of
 *                 - a class CLSS of type CTYPE
 *                 - look for sub-class SUB of type STYPE
 */

static int write_class_pco(client *cl, FILE *out, char *clss, char *ctype,
			   char *sub, char *stype, char *ind)
   {int i, n, ic, nc, global;
    char cln[BFLRG], fmt[BFLRG];
    char *c, *pc, *t, *var, *val, *entry;
    char **vars, **vals, **sa;

    nstrncpy(cln, BFLRG, clss, -1);
    for (c = cln; c != NULL; c = pc)
        {pc = strchr(c, ' ');
         if (pc == NULL)
            {if (IS_NULL(c) == TRUE)
                break;}
         else
            *pc++ = '\0';

	 global = (strcmp(c, "Glb") == 0);

	 if (global == TRUE)
	    {t  = dbget(cl, FALSE, "Globals");
	     sa = tokenize(t, " \t\n\r");}
         else
	    {fprintf(out, "%s%s %s {\n", ind, ctype, c);
	     t  = run(BOTH, "env | egrep '^%s_' | sort", c);
	     sa = tokenize(t, "\n\r");};

	 if (sa != NULL)
	    {for (n = 0; sa[n] != NULL; n++);

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
			  val = dbget(cl, TRUE, var);}
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
		    snprintf(fmt, BFLRG, "%%s%%-%ds = %%s\n", nc);
		 else
		    snprintf(fmt, BFLRG, "%%s   %%-%ds = %%s\n", nc);

		 for (i = 0; i < n; i++)
		     {if ((vars[i] != NULL) && (vals[i] != NULL))
			 fprintf(out, fmt, ind, vars[i], vals[i]);};

		 if (global == TRUE)
		    fprintf(out, "\n");
		 else
		    fprintf(out, "}\n\n");};

	     FREE(vars);
	     free_strings(vals);
	     free_strings(sa);};};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_PCO - write the input form of the database
 *           - to be read back in by pco
 */

static void write_pco(client *cl, state *st, char *dbname)
   {int rv;
    char t[BFLRG];
    FILE *out;

    if (dbname != NULL)
       snprintf(t, BFLRG, "%s.%s.pco", cgetenv(FALSE, "PERDB_PATH"), dbname);
    else
       snprintf(t, BFLRG, "%s.pco", cgetenv(FALSE, "PERDB_PATH"));

    out = open_file("w", t);

    rv = write_class_pco(cl, out, st->def_tools,
			 "Tool", NULL, NULL, "");
    ASSERT(rv == 0);

    rv = write_class_pco(cl, out, st->def_groups,
			 "Group", st->def_tools, "Tool", "");
    ASSERT(rv == 0);

    fclose(out);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_CLASS_PERL - write a PERL form text representation of
 *                  - a class CLSS of type CTYPE
 *                  - look for sub-class SUB of type STYPE
 */

static int write_class_perl(client *cl, FILE *out, char *clss, char *ctype,
			    char *sub, char *stype, char *ind)
   {int i, n, ic, nc, global;
    char cln[BFLRG], fmt[BFLRG];
    char *c, *pc, *t, *var, *val, *entry;
    char **vars, **vals, **sa;

    nstrncpy(cln, BFLRG, clss, -1);
    for (c = cln; c != NULL; c = pc)
        {pc = strchr(c, ' ');
         if (pc == NULL)
            {if (IS_NULL(c) == TRUE)
                break;}
         else
            *pc++ = '\0';

	 global = (strcmp(c, "Glb") == 0);

	 if (global == TRUE)
	    {t  = dbget(cl, FALSE, "Globals");
	     sa = tokenize(t, " \t\n\r");}
         else
	    {fprintf(out, "%s'%s:%s_pact' => {\n", ind, ctype, c);
	     t  = run(BOTH, "env | egrep '^%s_' | sort", c);
	     sa = tokenize(t, "\n\r");};

	 if (sa != NULL)
	    {for (n = 0; sa[n] != NULL; n++);

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
			  val = dbget(cl, FALSE, var);}
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
		    snprintf(fmt, BFLRG, "%%s%%-%ds => '%%s',\n", nc);
		 else
		    snprintf(fmt, BFLRG, "%%s   %%-%ds => '%%s',\n", nc);

		 for (i = 0; i < n; i++)
		     {if ((vars[i] != NULL) && (vals[i] != NULL))
			 fprintf(out, fmt, ind, vars[i], vals[i]);};

		 if (global == TRUE)
		    fprintf(out, "\n");
		 else
		    fprintf(out, "%s},\n\n", ind);};

	     FREE(vars);
	     free_strings(vals);
	     free_strings(sa);};};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_PERL - write the input form of the database
 *            - to be read by mio
 */

static void write_perl(client *cl, state *st, char *dbname)
   {int rv;
    char t[BFLRG];
    FILE *out;

    if (dbname != NULL)
       snprintf(t, BFLRG, "%s.%s.pl", cgetenv(FALSE, "PERDB_PATH"), dbname);
    else
       snprintf(t, BFLRG, "%s.pl", cgetenv(FALSE, "PERDB_PATH"));
    out = open_file("w", t);

    fprintf(out, "{\n");

    rv = write_class_perl(cl, out, st->def_tools, "Tool",
			  NULL, NULL, "   ");
    ASSERT(rv == 0);

    rv = write_class_perl(cl, out, st->def_groups, "Group",
			  st->def_tools, "Tool", "   ");
    ASSERT(rv == 0);

    fprintf(out, "}\n");

    fclose(out);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PCO_SAVE_DB - write a text representation of the configuration database */

static int pco_save_db(client *cl, char *dbname)
   {int rv;
    char t[BFLRG];

    rv = TRUE;

    dbset(cl, "Tools",  st.def_tools);
    dbset(cl, "Groups", st.def_groups);

/* save the persistent database */
    if (dbname == NULL)
       nstrncpy(t, BFLRG, "save:", -1);
    else
       snprintf(t, BFLRG, "save:%s:", dbname);
    dbcmd(cl, t);

/* write the input form of the database - to be read back in by pco */
    write_pco(cl, &st, dbname);

/* write the perl form of the database - to be read back in by mio */
    write_perl(cl, &st, dbname);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PCO_LOAD_DB - load the specified database */

static int pco_load_db(client *cl, char *dbname)
   {int rv, i, nv;
    char **ta;

    rv = dbrestore(cl, dbname);

    separator(Log);

    note(Log, FALSE, "Restored database variables:\n");

    ta = cenv(TRUE, NULL);
    if (ta != NULL)
       {nv = lst_length(ta);
	for (i = 0; i < nv; i++)
	    note(Log, FALSE, "   %s\n", ta[i]);

        note(Log, FALSE, "%d variables total\n", i);
	free_strings(ta);};

    separator(Log);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ENV_OUT - write out an environment variable to various files */

static void env_out(FILE *fsh, FILE *fcsh, FILE *fdk, FILE *fmd,
		    char *var, char *val)
   {char s[BFLRG];
    char *vl;

    if (val == NULL)
       val = "\"\"";

    nstrncpy(s, BFLRG, val, -1);
    vl = trim(expand(s, BFLRG, NULL, FALSE), BOTH, " \t");

    note(fsh,  TRUE, "export %s=%s",    var, vl);
    note(fcsh, TRUE, "setenv %s %s",    var, vl);
    note(fdk,  TRUE, "dk_setenv %s %s", var, vl);
    note(fmd,  TRUE, "setenv %s %s;",   var, vl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ADD_SPEC_ENV_VARS - add variables specified in the environment
 *                   - variable ENV_VARS to the environment files
 */

static void add_spec_env_vars(client *cl,
			      FILE *fcsh, FILE *fsh, FILE *fdk, FILE *fmd,
			      int n, char **sa, int blank)
   {int i, iv, nv, ok;
    char *p, *v, *var, **va;

    var = cgetenv(TRUE, "ENV_VARS");
    va  = tokenize(var, " :");
    nv  = (va != NULL) ? lst_length(va) : 0;
    for (iv = 0; iv < nv; iv++)

/* see if S is also in the list SA */
        {ok = TRUE;
	 for (i = 0; (i < n) && (ok == TRUE); i++)
	     {v = va[iv];
	      ok &= (strncmp(v, sa[i], strlen(v)) != 0);};

	 if (ok == TRUE)
	    {p = dbget(cl, TRUE, v);
	     if ((IS_NULL(p) == FALSE) || (blank == TRUE))
	        env_out(fsh, fcsh, fdk, fmd, v, p);};};

    free_strings(va);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ADD_SET_CFG - add variables set in config files */

static void add_set_cfg(client *cl,
			FILE *fcsh, FILE *fsh, FILE *fdk, FILE *fmd)
   {int i, j, n, nc, ok;
    char *var, *val, *p, **sa;

    fflush(st.aux.SEF);
    sa = file_text(TRUE, "%s/log/file.se", st.dir.root);
    n  = (sa != NULL) ? lst_length(sa) : 0;

    add_spec_env_vars(cl, fcsh, fsh, fdk, fmd, n, sa, TRUE);

/* add all the setenv'd variables */
    if (sa != NULL)
       {for (i = 0; i < n; i++)
	    {p = sa[i];

	     var = strtok(p, " ");
	     if (var != NULL)
	        {nc  = strlen(var);
		 val = strtok(NULL, "\n");
		    
		 if (strcmp(var, "ENV_VARS") == 0)
		    continue;

/* handle PATH specially - just gather everything that is
 * not $PATH or ${PATH}
 */
	         else if (strcmp(var, "PATH") == 0)
		    push_path(P_APPEND, epath, val);

/* weed out duplicates - taking only the last setting */
		else
		   {ok = FALSE;
		    for (j = i+1; (j < n) && (ok == FALSE); j++)
		        ok = ((strncmp(var, sa[j], nc) == 0) &&
			      (sa[j][nc] == ' '));
		     
		    if (ok == FALSE)
		       env_out(fsh, fcsh, fdk, fmd, var, val);};};};

	free_strings(sa);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ADD_SET_DB - add variables set from database file */

static void add_set_db(FILE *fcsh, FILE *fsh, FILE *fdk, FILE *fmd)
   {int i, n, nc;
    char s[BFLRG];
    char *var, *val, **sa;
    static char *rej[] = { "Log", "ALog",
			   "CCP", "CCObj", "CCArc",
			   "CCObj_BP", "CCArc_BP", 
			   "LexObj", "LexArc", "LexC",
			   "LexObj_BP", "LexArc_BP", 
			   "YaccObj", "YaccArc", "YaccC",
			   "YaccObj_BP", "YaccArc_BP", 
			   "FCObj", "FCArc", "TemplH", NULL };

    sa = cenv(TRUE, rej);
    if (sa != NULL)
       {n = lst_length(sa);
	for (i = 0; i < n; i++)
	    {var = sa[i];
	     val = strchr(var, '=');
	     if (val != NULL)
	        {*val++ = '\0';
		    
		 nc = last_char(val);
		 if (val[nc] == '\n')
		    val[nc] = '\0';

/* handle PATH specially - just gather everything that is not $PATH or ${PATH} */
	         if (strcmp(var, "PATH") == 0)
		    push_path(P_APPEND, epath, val);

		else
		   {snprintf(s, BFLRG, "\"%s\"", val);
		    env_out(fsh, fcsh, fdk, fmd, var, s);};};};

	free_strings(sa);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_ENVF - write out the env file because other scripts need to source it
 *            - write it for CSH or SH families
 */

static void write_envf(client *cl, int lnotice)
   {int i, n;
    char *t;
    char *site[] = { "CONFIG_METHOD" };
    char *sfx[]  = { "csh", "sh", "dk", "mdl" };
    FILE *fcsh, *fsh, *fdk, *fmd;

    separator(Log);
    if (lnotice == TRUE)
       noted(Log, "   Environment setup files - env-pact.csh, env-pact.sh, env-pact.dk, and env-pact.mdl");
    else
       note(Log, TRUE, "   Environment setup files - env-pact.csh, env-pact.sh, env-pact.dk, and env-pact.mdl");
    note(Log, TRUE, "");

    fcsh = open_file("w", "%s/env-pact.csh", st.dir.etc);
    fsh  = open_file("w", "%s/env-pact.sh",  st.dir.etc);
    fdk  = open_file("w", "%s/env-pact.dk",  st.dir.etc);
    fmd  = open_file("w", "%s/env-pact.mdl", st.dir.etc);

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
    if (st.db == NULL)
       {n = sizeof(site)/sizeof(char *);
	for (i = 0; i < n; i++)
	    env_out(fsh, fcsh, fdk, fmd, site[i], dbget(cl, TRUE, site[i]));

	add_set_cfg(cl, fcsh, fsh, fdk, fmd);}
    else
       add_set_db(fcsh, fsh, fdk, fmd);

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

#if 0
    {char lPython[BFLRG];

     if (st.have_python == TRUE)
        {snprintf(lPython, BFLRG, "%s/python%s",
		  st.dir.lib, dbget(cl, TRUE, "PY_Vers"));
	 t = dbget(cl, TRUE, "PYTHONPATH");
	 if (t != NULL)
	    {if (strstr(t, lPython) == NULL)
	        dbset(cl, "PYTHONPATH", "%s:$PYTHONPATH", lPython);}
	 else
	    dbset(cl, "PYTHONPATH", lPython);

	 t = dbget(cl, TRUE, "PYTHONPATH");
	 note(fcsh, TRUE, "setenv PYTHONPATH %s", t);
	 note(fsh, TRUE,  "export PYTHONPATH=%s", t);
	 note(fdk, TRUE,  "dk_setenv PYTHONPATH %s", t);
	 note(fmd, TRUE,  "setenv PYTHONPATH %s;", t);};};
#endif

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

/* log the results */
    n = sizeof(sfx)/sizeof(char *);
    for (i = 0; i < n; i++)
        {note(Log, TRUE, "----- env-pact.%s -----", sfx[i]);
	 cat(Log, 0, -1, "%s/env-pact.%s", st.dir.etc, sfx[i]);
         note(Log, TRUE, " ");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHECK_CROSS - verify that we have a RUN_SIGNATURE_DB if
 *             - we are cross compiling
 */

static int check_cross(client *cl)
   {int rv;
    char *s;

    rv = TRUE;

    s = cgetenv(TRUE, "CROSS_COMPILE");
    if (strcmp(s, "FALSE") != 0)
       {s = cgetenv(TRUE, "RUN_SIGNATURE_DB");
	if (file_exists(s) == FALSE)
	   {noted(Log, "");
	    noted(Log, "Cross compiling without a run_signature database - exiting");
	    noted(Log, "");
	    rv = FALSE;}
	else
	   {dbset(cl, "RUN_SIGNATURE_DB", s);
	    note(Log, TRUE, "");
	    note(Log, TRUE, "Cross compiling with run_signature database %s",
		 s);
	    note(Log, TRUE, "");};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHECK_DIR - check for missing directories */

static void check_dir(client *cl)
   {int i, n;
    char Created[BFLRG], Missing[BFLRG];
    char *sib, *dir;
    static char *dlst[] = {"bin", "lib", "include", "include/shell",
			   "etc", "scheme", "man", "man/man1", "man/man3"};

    n   = sizeof(dlst)/sizeof(char *);
    sib = dbget(cl, TRUE, "InstBase");

    if (st.create_dirs == TRUE)
       {Created[0] = '\0';
        if ((IS_NULL(sib) == FALSE) && (strcmp(sib, "none") != 0))
           {for (i = 0; i < n; i++)
                {dir = dlst[i];
                 if (dir_exists("%s/%s", sib, dir) == FALSE)
                    {run(BOTH, "mkdir -p %s/%s", sib, dir);
                     push_tok(Created, BFLRG, ' ', "%s/%s", sib, dir);};};};

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
        if ((IS_NULL(sib) == FALSE) && (strcmp(sib, "none") != 0))
           {for (i = 0; i < n; i++)
                {dir = dlst[i];
                 if (dir_exists("%s/%s", sib, dir) == FALSE)
                    {push_tok(Missing, BFLRG, ' ', "%s/%s", sib, dir);};};};

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
   {int n;
    char t[BFLRG];
    char *p, *pc;
    file_entry *se;

    se = &st.fstck.file[st.fstck.n-1];

    p = fgets(s, nc-1, se->fp);
    if (p != NULL)
       {se->iline++;

	n = strlen(s);

/* handle continued lines */
        while ((n > 1) && (s[n-2] == '\\') && (s[n-1] == '\n'))
           {p = fgets(t, BFLRG, se->fp);
	    if (p != NULL)
	       {nstrncpy(s+n-2, nc-n-2, trim(t, BOTH, " \t"), -1);
		n = strlen(s);};
	    se->iline++;};

/* make sure the line ends in '\n'
 * if last line in file does not end in '\n' this
 * can happen with bad consequences
 */
	if (s[n-1] != '\n')
	   s[n++] = '\n';

/* now end the line - stripping off trailing comment or '\n' */
	pc = strchr(s, '#');
	if (pc != NULL)
	   *pc = '\0';
	else
	   s[n-1] = '\0';

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

static void parse_opt(client *cl, char *s, int nc)
   {int i, l, n, ok, mt;
    exoper oper;
    char vr[BFLRG], op[BFLRG], vl[BFLRG];
    char *t, *avl, *arg, *opt, **sa;

    sa = tokenize(s, "[;]\n\r");
    if (sa != NULL)
       {for (n = 0; sa[n] != NULL; n++);

	ok  = FALSE;
	avl = "off";
	for (l = 0; (l < n) && (ok == FALSE); l++)
	    {vr[0] = '\0';

	     t = sa[l];
	     parse_line(cl, t, vr, op, vl, nc);

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
	        avl = dbget(cl, TRUE, arg);

/* select the value */
	     else

/* treat single '*' specially since match will automatically succeed */
	       {if (strcmp(vr, "*") == 0)
		   {if (strcmp(avl, "off") != 0)
		       {nstrncpy(s, nc, arg, -1);
			trim(s, BACK, " *\t\n");
/*
                        if (LAST_CHAR(s) == '\n')
		           LAST_CHAR(s) = '\0';
*/
			nstrcat(s, nc, avl);
			ok = TRUE;};}

	        else if (strcmp(avl, vr) == 0)
		   {nstrncpy(s, nc, arg, -1);
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

	free_strings(sa);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_LINE - parse the next line from the input */

static void parse_line(client *cl, char *s,
		       char *key, char *oper, char *value, int nc)
   {char t[BFLRG];
    char *p;

    strcpy(t, s);

    p = strtok(t, " \t");
    if (p != NULL)
       nstrncpy(key, nc, p, -1);

    p = strtok(NULL, " \t");
    if (p != NULL)
       nstrncpy(oper, nc, p, -1);
    else
       oper[0] = '\0';

    p = strtok(NULL, "\n");
    if (p != NULL)
       {nstrncpy(value, nc, p, -1);
	if ((strchr(oper, '=') != NULL) && (p[0] == '['))
	   {while (strchr(value, ']') == NULL)
	       {read_line(t, BFLRG);
		nstrcat(value, nc, " ");
		nstrcat(value, nc, trim(t, FRONT, " \t"));};
	    parse_opt(cl, value, nc);};}
    else
       value[0] = '\0';

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARSE_RULE - read a rule into the VAR */

static void parse_rule(char *var, int nc)
   {int n;
    char line[BFLRG], val[BFLRG];

    val[0] = '\0';

    while (TRUE)
       {read_line(line, BFLRG);

        if ((IS_NULL(line) == TRUE) || (strcmp(line, "++end++") == 0))
           break;

	nstrcat(val, BFLRG, line);
        nstrcat(val, BFLRG, "\n");};

    n = strlen(val);
    n = min(n, nc-1);
    nstrncpy(var, nc, val, -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DP_DEFINE - gather DP defines */

static void dp_define(void)
   {char line[BFLRG];

    if (st.aux.DPF == NULL)
       st.aux.DPF = open_file("w", st.aux.dpfn);

    while (TRUE)
       {read_line(line, BFLRG);

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

static void setup_analyze_env(client *cl, char *base)
   {char alog[BFLRG];
    FILE *out;

/* setup the analyze log file */
    snprintf(alog, BFLRG, "%s/log/analyze",  st.dir.root);
    out = open_file("w", alog);
    note(out, TRUE, "%s", get_date());
    fclose(out);

    dbset(cl, "Host",    st.host);
    dbset(cl, "Arch",    st.arch);
    dbset(cl, "System",  st.system);
    dbset(cl, "Sys",     st.sys);
    dbset(cl, "BaseDir", base);
    dbset(cl, "SysDir",  st.dir.root);
    dbset(cl, "ScrDir",  st.dir.scr);
    dbset(cl, "AnaDir",  "%s/analyze", st.dir.mng);
    dbset(cl, "Log",     st.logf);
    dbset(cl, "ALog",    alog);

    dbset(cl, "AbsoluteDeb", st.abs_deb ? "TRUE" : "FALSE");
    dbset(cl, "AbsoluteOpt", st.abs_opt ? "TRUE" : "FALSE");
    dbset(cl, "Profile",     st.profilep ? "TRUE" : "FALSE");
    dbset(cl, "UseTmpDir",   st.tmp_dirp ? "TRUE" : "FALSE");

    if (strncmp(st.os, "CYGWIN", 6) == 0)
       st.os[6] = '\0';

    dbset(cl, "OS_Name",      st.os);
    dbset(cl, "OS_Release",   st.osrel);

    if (strcmp(st.os, "Windows_NT") == 0)
       dbset(cl, "CDecls", "TRUE");
    else
       dbset(cl, "CDecls", "FALSE");

    dbset(cl, "ANSI",   "ANSI");
    dbset(cl, "MeOnly", "TRUE");

/* initialization of non-graphics flags */
    if (dbdef(cl, "NM") == FALSE)
       {if (strcmp(st.os, "AIX") == 0)
	   dbset(cl, "NM", "/usr/bin/nm -g -X %s", dbget(cl, TRUE, "Bits"));
        else
	   dbset(cl, "NM", "%s -g", cwhich("nm"));};

    dbinitv(cl, "CC_Version",  "");
    dbinitv(cl, "FC_Version",  "");
    dbinitv(cl, "LD_Version",  "");

    dbinitv(cl, "CC_Inc",    "");
    dbinitv(cl, "CC_Flags",  "");
    dbinitv(cl, "LD_Lib",    "");
    dbinitv(cl, "LD_RPath",  "");
    dbinitv(cl, "LD_Flags",  "");

    dbinitv(cl, "CPU",       "unknown");
    dbinitv(cl, "FPU",       "unknown");
    dbinitv(cl, "BE",        "");

/* parallel front end */
    dbinitv(cl, "PFE",       "%s/do-run -m", st.dir.bin);

/* cross compile front end */
    if (dbcmp(cl, "CROSS_COMPILE", "FALSE") != 0)
       dbinitv(cl, "CFE", "%s/do-run -m", st.dir.bin);
    else
       dbinitv(cl, "CFE", "");

/* initialization of graphics flags */
    dbinitv(cl, "GRAPHICS_Windows",       "X");
    dbinitv(cl, "Std_UseX",   "TRUE");
    dbinitv(cl, "Std_UseOGL", "FALSE");
    dbinitv(cl, "Std_UseQD",  "FALSE");

    dbinitv(cl, "DP_Inc",          "");
    dbinitv(cl, "DP_Lib",          "");
    dbinitv(cl, "DP_Path",         "");
    dbinitv(cl, "MDG_Inc",         "");
    dbinitv(cl, "MDG_Lib",         "");
    dbinitv(cl, "MD_Inc",          "");
    dbinitv(cl, "MD_Lib",          "");
    dbinitv(cl, "GRAPHICS_Devices", "PS CGM MPG PNG JPG");
    dbinitv(cl, "GRAPHICS_Flags",    "");

/* initialize Cfg group flags */
    dbinitv(cl, "Cfg_CC_Flags",  dbget(cl, FALSE, "CC_Flags"));
    dbinitv(cl, "Cfg_CC_Inc",    dbget(cl, FALSE, "CC_Inc"));
    dbinitv(cl, "Cfg_LD_RPath",  dbget(cl, FALSE, "LD_RPath"));
    dbinitv(cl, "Cfg_LD_Flags",  dbget(cl, FALSE, "LD_Flags"));
    dbinitv(cl, "Cfg_LD_Lib",    dbget(cl, FALSE, "LD_Lib"));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_OUTPUT_ENV - setup the environment for programs which write
 *                  - configured files
 */

static void setup_output_env(client *cl, char *base)
   {

/* close any open intermediate files and export their names */
    dbset(cl, "DPFile", st.aux.dpfn);
    if (st.aux.DPF != NULL)
       fclose(st.aux.DPF);

    dbset(cl, "CEFile", st.aux.cefn);
    if (st.aux.CEF != NULL)
       fclose(st.aux.CEF);

    dbset(cl, "MVFile", st.aux.mvfn);
    if (st.aux.MVF != NULL)
       fclose(st.aux.MVF);

    dbset(cl, "URFile", st.aux.urfn);
    if (st.aux.URF != NULL)
       fclose(st.aux.URF);

/* remove duplicate tokens in selected lists */
    dbset(cl, "DP_Inc",  unique(dbget(cl, FALSE, "DP_Inc"),  FALSE, ' '));
    dbset(cl, "MDG_Inc", unique(dbget(cl, FALSE, "MDG_Inc"), FALSE, ' '));
    dbset(cl, "MD_Inc",  unique(dbget(cl, FALSE, "MD_Inc"),  FALSE, ' '));
    dbset(cl, "CC_Inc",  unique(dbget(cl, FALSE, "CC_Inc"),  FALSE, ' '));

    dbset(cl, "DP_Lib",  unique(dbget(cl, FALSE, "DP_Lib"),  FALSE, ' '));
    dbset(cl, "MD_Lib",  unique(dbget(cl, FALSE, "MD_Lib"),  FALSE, ' '));
    dbset(cl, "LD_Lib",  unique(dbget(cl, FALSE, "LD_Lib"),  FALSE, ' '));

    dbset(cl, "DP_Path", unique(dbget(cl, FALSE, "DP_Path"), FALSE, ' '));

/* NOTE: for OSX this would reduce -framework FOO -framework BAR
 * to -framework FOO BAR which is not legal
 */
    if (strcmp(st.os, "Darwin") != 0)
       dbset(cl, "MDG_Lib", unique(dbget(cl, FALSE, "MDG_Lib"), FALSE, ' '));

    dbset(cl, "BinDir",  st.dir.bin);
    dbset(cl, "IncDir",  st.dir.inc);
    dbset(cl, "EtcDir",  st.dir.etc);
    dbset(cl, "ScrDir",  st.dir.scr);
    dbset(cl, "SchDir",  st.dir.sch);
    dbset(cl, "CfgDir",  st.dir.cfg);

    dbset(cl, "Load",        st.loadp ? "TRUE" : "FALSE");
    dbset(cl, "NoExe",       st.exep ? "TRUE" : "FALSE");
    dbset(cl, "ConfigVars",  st.cfgv);
    dbset(cl, "DefGroups",   st.def_groups);
    dbset(cl, "CONFIG_FILE", st.cfgf);

    dbset(cl, "CCP",     st.rules.ccp);
    dbset(cl, "CCObj",   st.rules.co);
    dbset(cl, "CCArc",   st.rules.ca);
    dbset(cl, "LexObj",  st.rules.lo);
    dbset(cl, "LexArc",  st.rules.la);
    dbset(cl, "LexC",    st.rules.lc);
    dbset(cl, "YaccObj", st.rules.yo);
    dbset(cl, "YaccArc", st.rules.ya);
    dbset(cl, "YaccC",   st.rules.yc);
    dbset(cl, "FCObj",   st.rules.fo);
    dbset(cl, "FCArc",   st.rules.fa);
    dbset(cl, "TemplH",  st.rules.th);

    dbset(cl, "CCObj_BP",   st.rules.co_bp);
    dbset(cl, "CCArc_BP",   st.rules.ca_bp);
    dbset(cl, "LexObj_BP",  st.rules.lo_bp);
    dbset(cl, "LexArc_BP",  st.rules.la_bp);
    dbset(cl, "YaccObj_BP", st.rules.yo_bp);
    dbset(cl, "YaccArc_BP", st.rules.ya_bp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DEFAULT_VAR - setup default variable values */

static void default_var(client *cl, char *base)
   {int i, n;
    char cmd[BFLRG];
    char **sa;

    if (cdefenv("USER") == FALSE)
       {if (cdefenv("LOGNAME") == FALSE)
	   dbset(cl, "USER", "anonymous");
        else
	   dbset(cl, "USER", cgetenv(TRUE, "LOGNAME"));};

    csetenv("PATH", "%s:%s", st.dir.mng, cgetenv(TRUE, "PATH"));

/* log the current environment */
    sa = cenv(TRUE, NULL);
    n  = lst_length(sa);
    for (i = 0; i < n; i++)
        note(Log, TRUE, "%s", sa[i]);
    free_strings(sa);

/* define the set of specifications which define a tool */
    st.toolv[0] = '\0';
    push_tok(st.toolv, BFLRG, ' ', "Exe");
    push_tok(st.toolv, BFLRG, ' ', "Flags");
    push_tok(st.toolv, BFLRG, ' ', "Version");
    push_tok(st.toolv, BFLRG, ' ', "Debug");
    push_tok(st.toolv, BFLRG, ' ', "Optimize");
    push_tok(st.toolv, BFLRG, ' ', "Inc");
    push_tok(st.toolv, BFLRG, ' ', "Lib");
    push_tok(st.toolv, BFLRG, ' ', "RPath");
    push_tok(st.toolv, BFLRG, ' ', "IFlag");
    push_tok(st.toolv, BFLRG, ' ', "XFlag");

/* define and initialize the (special) config variables */
    st.cfgv[0] = '\0';
    push_tok(st.cfgv, BFLRG, ' ', "CC_Exe");
    push_tok(st.cfgv, BFLRG, ' ', "CC_Linker");
    push_tok(st.cfgv, BFLRG, ' ', "CC_Flags");
    push_tok(st.cfgv, BFLRG, ' ', "CC_Debug");
    push_tok(st.cfgv, BFLRG, ' ', "CC_Optimize");
    push_tok(st.cfgv, BFLRG, ' ', "CC_Inc");
    push_tok(st.cfgv, BFLRG, ' ', "FC_Exe");
    push_tok(st.cfgv, BFLRG, ' ', "FC_Linker");
    push_tok(st.cfgv, BFLRG, ' ', "FC_Flags");
    push_tok(st.cfgv, BFLRG, ' ', "FC_Debug");
    push_tok(st.cfgv, BFLRG, ' ', "FC_Optimize");
    push_tok(st.cfgv, BFLRG, ' ', "LD_Exe");
    push_tok(st.cfgv, BFLRG, ' ', "LD_RPath");
    push_tok(st.cfgv, BFLRG, ' ', "LD_Flags");
    push_tok(st.cfgv, BFLRG, ' ', "LD_Lib");

    strcpy(st.sys, path_tail(st.cfgf));

    unamef(st.host,  BFLRG, "n");
    unamef(st.os,    BFLRG, "s");
    unamef(st.osrel, BFLRG, "r");
    unamef(st.hw,    BFLRG, "m");

/* remove parens from osrel - it is bad for the shells later on */
    sa = tokenize(st.osrel, "()");
    concatenate(st.osrel, BFLRG, sa, ",");
    free_strings(sa);

    snprintf(st.dir.scr, BFLRG, "%s/scripts", base);
    snprintf(cmd, BFLRG, "%s/system-id", st.dir.scr);
    nstrncpy(st.arch, BFLRG, run(BOTH, cmd), -1);

/* check variables which may have been initialized from the command line */
    if (IS_NULL(st.system) == TRUE)
       nstrncpy(st.system, BFLRG, run(BOTH, "%s/cfgman use", st.dir.scr), -1);
    cinitenv("System", st.system);

    dbinitv(cl, "CfgMan",        "%s/cfgman", st.dir.scr);
    dbinitv(cl, "Globals",       "");
    dbinitv(cl, "MngDir",        st.dir.mng);
    dbinitv(cl, "InstBase",      "none");
    dbinitv(cl, "PubInc",        "");
    dbinitv(cl, "PubLib",        "");
    dbinitv(cl, "ScmDir",        "scheme");
    dbinitv(cl, "Man1Dir",       "man/man1");
    dbinitv(cl, "Man3Dir",       "man/man3");
    dbinitv(cl, "CROSS_COMPILE", "FALSE");

/* global variables */
    snprintf(st.dir.root, BFLRG, "%s/dev/%s",  base, st.system);
    snprintf(st.dir.inc,  BFLRG, "%s/include", st.dir.root);
    snprintf(st.dir.etc,  BFLRG, "%s/etc",     st.dir.root);
    snprintf(st.dir.lib,  BFLRG, "%s/lib",     st.dir.root);
    snprintf(st.dir.bin,  BFLRG, "%s/bin",     st.dir.root);
    snprintf(st.dir.sch,  BFLRG, "%s/scheme",  st.dir.root);

    snprintf(st.env_csh,  BFLRG, "%s/env-pact.csh", st.dir.etc);
    snprintf(st.env_sh,   BFLRG, "%s/env-pact.sh",  st.dir.etc);
    snprintf(st.env_dk,   BFLRG, "%s/env-pact.dk",  st.dir.etc);
    snprintf(st.env_mdl,  BFLRG, "%s/env-pact.mdl", st.dir.etc);

/* variable defaults */
    strcpy(st.def_tools, "");

    push_struct("Glb", st.def_groups, STACK_GROUP);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RESET_MAKE_VARS - save the Group and Tool variables
 *                 - needed in the make-def file
 *                 - GOTCHA: if we do not need $MVFile then
 *                 - we do not need this routine (see write/make-def)
 */

static void reset_make_vars(void)
   {int i, nc;
    char vr[BFLRG];
    char *vl, **ta;

    if (st.aux.MVF == NULL)
       st.aux.MVF = open_file("w", st.aux.mvfn);

    nstrncpy(st.def_tools, BFLRG, cgetenv(FALSE, "Tools"), -1);
    nstrncpy(st.def_groups, BFLRG, cgetenv(FALSE, "Groups"), -1);

    ta = cenv(FALSE, NULL);
    if (ta != NULL)
       {for (i = 0; ta[i] != NULL; i++)
	    {nstrncpy(vr, BFLRG, ta[i], -1);
	     vl = strchr(vr, '=');
	     if (vl != NULL)
	        {*vl++ = '\0';

		 if (IS_NULL(vl) == TRUE)
		    continue;

/* write Tool vars */
		 FOREACH (t, st.def_tools, " ")
		    nc = strlen(t);
		    if ((strncmp(t, vr, nc) == 0) && (vr[nc] == '_'))
		       note(st.aux.MVF, TRUE, "%s = %s", vr, vl);
		 ENDFOR

/* write Group vars */
		 FOREACH (t, st.def_groups, " ")
		    nc = strlen(t);
		    if ((strncmp(t, vr, nc) == 0) && (vr[nc] == '_'))
		       note(st.aux.MVF, TRUE, "%s = %s", vr, vl);
		 ENDFOR;};};

	free_strings(ta);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DEFAULT_FILES - setup the various work files */

static void default_files(int append)
   {

    snprintf(st.logf, BFLRG, "%s/log/config", st.dir.root);
    if (append == FALSE)
       unlink_safe(st.logf);
    LOG_ON;

    st.aux.SEF = open_file("w+", "%s/log/file.se", st.dir.root);

    snprintf(st.aux.mvfn, BFLRG, "%s/log/file.mv", st.dir.root);
    st.aux.MVF = NULL;

    snprintf(st.aux.cefn, BFLRG, "%s/log/file.ce", st.dir.root);
    st.aux.CEF = NULL;

    snprintf(st.aux.urfn, BFLRG, "%s/log/file.ur", st.dir.root);
    st.aux.URF = NULL;

    snprintf(st.aux.dpfn, BFLRG, "%s/log/file.dpe", st.dir.root);
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
    snprintf(st.rules.ccp,   BFLRG,
             "\t%s\n",
	     "${CC} -E $<");

    snprintf(st.rules.co, BFLRG,
             "\t@(%s ; \\\n          %s)\n",
	     "echo \"${CCAnnounce} -c $<\"",
	     "${CC} -c $< -o $@");

    snprintf(st.rules.ca, BFLRG,
             "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"${CCAnnounce} -c $<\"",
	     cd, rm, tc,
	     "${CC} -c ${PACTSrcDir}/$< -o $*.o",
	     ar,
	     "${RM} $*.o 2>> errlog");

/* lex rules */
    snprintf(st.rules.lo, BFLRG,
             "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
             "echo \"lex $<\"",
	     rm, tc,
	     "${LEX} $< 2>> errlog",
	     le,
	     "${LX} -c $*.c",
	     "${RM} lex.yy.c $*.c");

    snprintf(st.rules.la, BFLRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"lex $<\"",
	     cd, rm, tc,
	     "${LEX} -t ${PACTSrcDir}/$< 1> lex.yy.c 2>> errlog",
	     le,
	     "echo \"${LX} -c $*.c\"",
	     "${LX} -c $*.c -o $*.o",
	     ar,
	     "${RM} lex.yy.c $*.c");

    snprintf(st.rules.lc, BFLRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"lex $<\"",
	     cd, rm, tc,
	     "${LEX} ${PACTSrcDir}/$< 2>> errlog",
	     le,
	     "${RM} lex.yy.c");

/* yacc rules */
    snprintf(st.rules.yo, BFLRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"yacc $<\"",
	     cd, rm, tc,
	     "${YACC} ${PACTSrcDir}/$< 2>> errlog",
	     ye,
	     "${YC} -c $*.c -o $*.o",
	     "${RM} $*.c");

    snprintf(st.rules.ya, BFLRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"yacc $<\"",
	     cd, rm, tc,
	     "${YACC} ${PACTSrcDir}/$< 2>> errlog",
	     ye,
	     "echo \"${YC} -c $*.c\"",
	     "${YC} -c $*.c -o $*.o",
	     ar,
	     "${RM} $*.c $*.o");

    snprintf(st.rules.yc, BFLRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"yacc $<\"",
	     cd, rm, tc,
	     "${YACC} ${PACTSrcDir}/$< 2>> errlog",
	     ye,
	     "mv $*.c ${PACTSrcDir}");
      
/* Fortran rules */
    snprintf(st.rules.fo, BFLRG,
	     "\t%s \\\n\t%s \\\n\t%s \\\n\t%s \\\n\t%s \\\n\t%s\n",
	     "@if [ ${FC_Exe} == none ]; then",
	     "   echo \"No Fortran compiler for $<\" ;",
	     "else",
	     "   echo \"${FCAnnounce} -c $<\" ;",
	     "   ${FC} -c $< -o $@ ;",
	     "fi");

    snprintf(st.rules.fa, BFLRG,
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
    snprintf(st.rules.th, BFLRG,
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
    snprintf(st.rules.co_bp, BFLRG,
             "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"${CCAnnounce} -c $<\"",
	     "${RM} $*.int.c",
	     "${CC} -E $< -o $@ > $*.int.c",
	     "${CC} -c $*.int.c -o $@",
	     "${RM} $*.int.c");

    snprintf(st.rules.ca_bp, BFLRG,
             "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"${CCAnnounce} -c $<\"",
	     cd, rm, tc,
	     "${RM} $*.int.c",
	     "${CC} -E ${PACTSrcDir}/$< > $*.int.c",
	     "${CC} -c $*.int.c -o $*.o",
	     ar,
	     "${RM} $*.int.c $*.o 2>> errlog");

/* lex rules */
    snprintf(st.rules.lo_bp, BFLRG,
             "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
             "echo \"lex $<\"",
	     rm, tc,
	     "${LEX} $< 2>> errlog",
	     le,
	     "${LX} -E $*.c > $*.int.c",
	     "${LX} -c $*.int.c",
	     "${RM} lex.yy.c $*.int.c $*.c");

    snprintf(st.rules.la_bp, BFLRG,
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
    snprintf(st.rules.yo_bp, BFLRG,
	     "\t@(%s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s ; \\\n          %s)\n",
	     "echo \"yacc $<\"",
	     cd, rm, tc,
	     "${YACC} ${PACTSrcDir}/$< 2>> errlog",
	     ye,
	     "${YC} -E $*.c > $*.int.c",
	     "${YC} -c $*.int.c -o $*.o",
	     "${RM} $*.int.c $*.c");

    snprintf(st.rules.ya_bp, BFLRG,
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

static void init_session(client *cl, char *base, int append)
   {

/* setup default variable values */
    default_var(cl, base);

/* setup the default rules for CC, Lex, Yacc, and FC */
    default_rules();
    bad_pragma_rules();

/* setup work files */
    default_files(append);

    printf("\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SET_INST_BASE - setup the InstBase related state */

static void set_inst_base(client *cl, char *ib)
   {

    if (strcmp(ib, "none") != 0)
       {if (LAST_CHAR(ib) == '/')
	   LAST_CHAR(ib) = '\0';

	st.installp = TRUE;};

    dbset(cl, "InstBase", ib);
    dbset(cl, "PubInc",   "-I%s/include", ib);
    dbset(cl, "PubLib",   "-L%s/lib", ib);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ENV_SUBST - substitute the text NT for the occurrence of
 *           - the text of environment variable REFVAR in
 *           - any environment variable
 */

static void env_subst(client *cl, char *refvar, char *nt)
   {int i, nc, nv;
    char *ot, *vr, *vl, *p, **ta;

    ot = cgetenv(FALSE, refvar);
    if (IS_NULL(ot) == FALSE)
       {ta = cenv(FALSE, NULL);
	if (ta != NULL)
	   {nv = lst_length(ta);
	    for (i = 0; i < nv; i++)
	        {vr = ta[i];
		 if (vr != NULL)
		    {vl = strchr(vr, '=');
		     if (vl != NULL)
		        {*vl++ = '\0';

			 nc = strlen(vl);
			 if (vl[nc-1] == '\n')
			    vl[nc-1] = '\0';

			 p = strstr(vl, ot);
			 if (p != NULL)
			    dbset(cl, vr, subst(vl, ot, nt, -1));};};};

	    free_strings(ta);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SET_VAR - set a variable as directed */

static void set_var(client *cl, int rep, char *var, char *oper, char *val)
   {char fvar[BFLRG], nval[BFLRG], mval[BFLRG], lval[BFLRG];
    char s[BFLRG+1];
    char *prfx, *t, *p;
    gt_entry *ge;

    ge   = st.gstck.st + st.gstck.n - 1;
    prfx = ge->item;

/* attach the current group suffix */
    if (strcmp(prfx, "Glb") == 0)
       {t = dbget(cl, FALSE, "Globals");
	snprintf(s, BFLRG, "%s %s", t, var);
	dbset(cl, "Globals", unique(s, FALSE, ' '));
	nstrncpy(fvar, BFLRG, var, -1);}
    else
       snprintf(fvar, BFLRG, "%s_%s", prfx, var);

    clean_space(val);

    if ((strcmp(oper, "+=") == 0) || (strcmp(oper, "=+") == 0))
       {if (dbdef(cl, fvar) == FALSE)
           {note(Log, TRUE, "Variable %s does not exist changing %s to =",
                 fvar, oper);
            oper = "=";};};

/* set variable */
    if (strcmp(oper, "=") == 0)
       {t = echo(FALSE, val);
        if (IS_NULL(t) == FALSE)
           {nstrncpy(nval, BFLRG, t, -1);
            if (rep == TRUE)
	       {if (st.aux.MVF == NULL)
		   st.aux.MVF = open_file("w", st.aux.mvfn);

		note(st.aux.MVF, TRUE, "%s = %s", fvar, nval);};

            dbset(cl, fvar, nval);}

        else
           {nval[0] = '\0';
            dbset(cl, fvar, nval);};

	FREE(t);}

/* add item to beginning of the existing variable */
    else if (strcmp(oper, "+=") == 0)

/* check value of val
 * we want things such as "foo += $bar" to do nothing if
 * "$bar" is not defined
 */
       {t = echo(FALSE, val);
        if (IS_NULL(t) == FALSE)
           {nstrncpy(lval, BFLRG, t, -1);
	    p = echo(FALSE, "$%s", fvar);
	    if (p != NULL)
	       {nstrncpy(nval, BFLRG, p, -1);
		FREE(p);};
            note(Log, TRUE, "Change    |%s|", fvar);
            note(Log, TRUE, "Prepend   |%s|", lval);
            note(Log, TRUE, "Old value |%s|", nval);

            snprintf(mval, BFLRG, "%s %s", lval, nval);
            note(Log, TRUE, "New value |%s|", mval);

            note(Log, TRUE, "Change expression |setenv %s %s|", fvar, mval);
            dbset(cl, fvar, mval);}
        else
           note(Log, TRUE, "   += not changing %s - no value for |%s|",
		fvar, val);

	FREE(t);}

/* add item to end of the existing variable */
    else if (strcmp(oper, "=+") == 0)

/* check value of val
 * we want things such as "foo =+ $bar" to do nothing if
 * "$bar" is not defined
 */
       {t = echo(FALSE, val);
        if (IS_NULL(t) == FALSE)
           {nstrncpy(lval, BFLRG, t, -1);
	    p = echo(FALSE, "$%s", fvar);
	    if (p != NULL)
	       {nstrncpy(nval, BFLRG, p, -1);
		FREE(p);};
            note(Log, TRUE, "Change    |%s|", fvar);
            note(Log, TRUE, "Append    |%s|", lval);
            note(Log, TRUE, "Old value |%s|", nval);

            snprintf(mval, BFLRG, "%s %s", nval, lval);
            note(Log, TRUE, "New value |%s|", mval);

            note(Log, TRUE, "Change expression |setenv %s %s|", fvar, mval);
            dbset(cl, fvar, mval);}
         else
            note(Log, TRUE, "   =+ not changing %s - no value for |%s|",
		 fvar, val);

	FREE(t);}

/* remove literal item from variable */
    else if (strcmp(oper, "-=") == 0)
       {t = echo(FALSE, "$%s", fvar);
	if (IS_NULL(t) == FALSE)
	   {nstrncpy(nval, BFLRG, t, -1);
	    note(Log, TRUE, "Change    |%s|", fvar);
	    note(Log, TRUE, "Remove    |%s|", val);
	    note(Log, TRUE, "Old value |%s|", nval);

	    p = echo(FALSE, "%s | sed 's|%s||'", nval, val);
	    if (p != NULL)
	       {nstrncpy(nval, BFLRG, p, -1);
		FREE(p);};

	    note(Log, TRUE, "New value |%s|", nval);

	    note(Log, TRUE, "Change expression |setenv %s %s|", fvar, nval);
	    dbset(cl, fvar, nval);}
         else
            note(Log, TRUE, "   -= not changing %s - no value for |%s|",
		 fvar, val);

	FREE(t);}

/* set value only if undefined */
    else if (strcmp(oper, "=?") == 0)

/* check value of val
 * we want things such as "foo =? $bar" to do nothing if
 * "$bar" is not defined
 */
       {t = echo(FALSE, val);
        if (IS_NULL(t) == FALSE)
	   dbset(cl, fvar, t);
        else
           note(Log, TRUE, "   =? not changing %s - no value for |%s|",
		fvar, val);

	FREE(t);}

    else
       noted(Log, "Bad operator '%s' in SET_VAR", oper);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROCESS_USE - do all the variable settings implied by a Use specification */

static void process_use(client *cl, char *sg, char *oper)
   {int whch;
    char nvr[BFLRG];
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
		  sg, dbget(cl, FALSE, "CurrGrp"));
             FOREACH(var, st.cfgv, " ")
                snprintf(nvr, BFLRG, "%s_%s", sg, var);
                if (dbdef(cl, nvr) == TRUE)
                   {val = dbget(cl, TRUE, nvr);
                    if (strcmp(var, "Exe") == 0)
                       set_var(cl, FALSE, var, "=", val);
                    else
                       set_var(cl, FALSE, var, oper, val);};
             ENDFOR
             break;

/* fill out a tool */
        case STACK_TOOL:
             if (dbcmp(cl, "CurrTool", "") == 0)
                {note(Log, TRUE, "Use tool %s to fill group %s",
		      sg, dbget(cl, FALSE, "CurrGrp"));
                 FOREACH(var, st.toolv, " ")
                    snprintf(nvr, BFLRG, "%s_%s", sg, var);
                    if (dbdef(cl, nvr) == TRUE)
                       {val = dbget(cl, TRUE, nvr);
                        set_var(cl, FALSE, nvr, oper, val);};
                 ENDFOR}
             else
                {note(Log, TRUE, "Use tool %s to fill tool %s",
		      sg, dbget(cl, FALSE, "CurrTool"));
                 FOREACH(var, st.toolv, " ")
                    snprintf(nvr, BFLRG, "%s_%s", sg, var);
                    if (dbdef(cl, nvr) == TRUE)
                       {val = dbget(cl, TRUE, nvr);
                        set_var(cl, FALSE, var, oper, val);};
                 ENDFOR};
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_CONFIG - read the user configuration file */

static void read_config(client *cl, char *cfg, int quiet)
   {int il;
    char line[BFLRG], key[BFLRG], oper[BFLRG], value[BFLRG];
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
    note(Log, TRUE, "Strict checking level is %s", dbget(cl, TRUE, "STRICT"));
    note(Log, TRUE, "");
    separator(Log);

    path = push_file(cfg, STACK_FILE);

/* toplevel loop over the input to define configuration parameters */
    for (il = 0; TRUE; il++)
        {if (dbdef(cl, "STOP") == TRUE)
	    {noted(Log, " ");
	     exit(1);};

	 read_line(line, BFLRG);

	 if (IS_NULL(line) == TRUE)
            continue;
	 else if (strcmp(line, "++end++") == 0)
            break;

	 parse_line(cl, line, key, oper, value, BFLRG);

/* handle include directives */
	 if (strcmp(key, "include") == 0)
	    {int n;
	     char ldepth[BFLRG];

	     n = st.fstck.n;
	     memset(ldepth, ' ', 3*n);
	     ldepth[3*n] = '\0';

	     path = push_file(oper, STACK_FILE);
	     note(Log, TRUE, "");
	     noted(Log, "%sincluding %s", ldepth, path);}

/* handle run directives */
	 else if (strcmp(key, "run") == 0)
	    {int n;
	     char ldepth[BFLRG];

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
	        {dbset(cl, "InstBase", value);
		 dbset(cl, "PubInc",   "-I%s/include", value);
		 dbset(cl, "PubLib",   "-L%s/lib", value);};}

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
                push_path(P_APPEND, epath, value);
	     s = echo(FALSE, value);
	     dbset(cl, oper, s);}

	 else if (strcmp(key, "parent") == 0)
	    {char *s, *var, *val;

	     s   = line + 7;
	     var = s + strspn(s, " \t");
	     val = delimited(var, "(", ")");

	     if (strcmp(var, "PATH") == 0)
	        {push_path(P_APPEND, epath, val);
		 s = echo(FALSE, val);
		 dbset(cl, var, s);}
	     else
	        dbset(cl, var, val);

/*	     if (st.phase == PHASE_READ) */
	        note(st.aux.SEF, TRUE, "%s \"%s\"", var, val);
	     note(Log, TRUE, "Command: setenv %s %s", var, val);}

/* handle Tool specifications */
	 else if (strcmp(key, "Tool") == 0)
	    {note(Log, TRUE, "--- tool %s", oper);
	     dbset(cl, "CurrTool", oper);
	     note(Log, TRUE, "Defining tool %s", oper);
	     push_struct(oper, st.def_tools, STACK_TOOL);}

/* handle Group specifications */
	 else if (strcmp(key, "Group") == 0)
	    {note(Log, TRUE, "--- group %s", oper);
	     dbset(cl, "CurrGrp", oper);
	     push_struct(oper, st.def_groups, STACK_GROUP);}

/* handle Use specifications */
	 else if (strcmp(key, "Use") == 0)
	    process_use(cl, value, oper);

/* handle struct close specifications */
	 else if (strcmp(key, "}") == 0)
            pop_struct(cl);

	 else if ((strcmp(oper, "=") == 0)  ||
		  (strcmp(oper, "+=") == 0) ||
		  (strcmp(oper, "=+") == 0) ||
		  (strcmp(oper, "-=") == 0) ||
		  (strcmp(oper, "=?") == 0))
	    set_var(cl, TRUE, key, oper, value);

/* .c.i rule handler */
	 else if (strcmp(key, ".c.i:") == 0)
	    {parse_rule(st.rules.ccp, BFLRG);
	     if (st.verbose == TRUE)
	        noted(Log, "Redefining .c.i rule:\n%s\n", st.rules.ccp);}

/* .c.o rule handler */
	 else if (strcmp(key, ".c.o:") == 0)
	    {parse_rule(st.rules.co, BFLRG);
	     if (st.verbose == TRUE)
	        noted(Log, "Redefining .c.o rule:\n%s\n", st.rules.co);}

/* .c.a rule handler */
	 else if (strcmp(key, ".c.a:") == 0)
	    {parse_rule(st.rules.ca, BFLRG);
	     if (st.verbose == TRUE)
	        noted(Log, "Redefining .c.a rule:\n%s\n", st.rules.ca);}

/* .f.o rule handler */
	 else if (strcmp(key, ".f.o:") == 0)
	    {parse_rule(st.rules.fo, BFLRG);
	     if (st.verbose == TRUE)
	        noted(Log, "Redefining .f.o rule:\n%s\n", st.rules.fo);}

/* .f.a rule handler */
	 else if (strcmp(key, ".f.a:") == 0)
	    {parse_rule(st.rules.fa, BFLRG);
	     if (st.verbose == TRUE)
	        noted(Log, "Redefining .f.a rule:\n%s\n", st.rules.fa);}

/* .l.o rule handler */
	 else if (strcmp(key, ".l.o:") == 0)
	    {parse_rule(st.rules.lo, BFLRG);
	     if (st.verbose == TRUE)
	        noted(Log, "Redefining .l.o rule:\n%s\n", st.rules.lo);}

/* .l.a rule handler */
	 else if (strcmp(key, ".l.a:") == 0)
	    {parse_rule(st.rules.la, BFLRG);
	     if (st.verbose == TRUE)
	        noted(Log, "Redefining .l.a rule:\n%s\n", st.rules.la);}

/* .l.c rule handler */
	 else if (strcmp(key, ".l.c:") == 0)
	    {parse_rule(st.rules.lc, BFLRG);
	     if (st.verbose == TRUE)
	        noted(Log, "Redefining .l.c rule:\n%s\n", st.rules.lc);}

/* .y.o rule handler */
	 else if (strcmp(key, ".y.c:") == 0)
	    {parse_rule(st.rules.yo, BFLRG);
	     if (st.verbose == TRUE)
	        noted(Log, "Redefining .y.o rule:\n%s\n", st.rules.yo);}

/* .y.a rule handler */
	 else if (strcmp(key, ".y.a:") == 0)
	    {parse_rule(st.rules.ya, BFLRG);
	     if (st.verbose == TRUE)
	        noted(Log, "Redefining .y.a rule:\n%s\n", st.rules.ya);}

/* .y.c rule handler */
	 else if (strcmp(key, ".y.c:") == 0)
	    {parse_rule(st.rules.yc, BFLRG);
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

/* ADD_DO_RUN - add an entry to the database */

static void add_do_run(FILE *fp, char *lf)
   {int i;
    char **sa;

    sa = file_text(FALSE, lf);

    for (i = 0; sa[i] != NULL; i++)
        {if (strchr(sa[i], '#') == NULL)
 	    fprintf(fp, "%s\n", sa[i]);};

    free_strings(sa);

    fprintf(fp, "\n");
    fprintf(fp, "#-------------------------------------------------------------------------\n");

    note(Log, TRUE, "Using %s", lf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO_RUN_SPEC - return the path to S in LST */

static char *do_run_spec(char *lst, char *s)
   {char *file;
    static char t[BFLRG];

    file = NULL;
    if (file_exists(s) == TRUE)
       file = s;

    else
       {FOREACH(d, lst, " \t\n")
	   snprintf(t, BFLRG, "%s/do-run-%s", d, s);
	   if (file_exists(t) == TRUE)
	      {file = t;
	       break;};
	ENDFOR;};

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_DO_RUN_DB - write the do-run database */

static void write_do_run_db(client *cl, state *st)
   {int i, l, nm, ns, ok;
    char ldbg[BFLRG], lmpi[BFLRG], lcrs[BFLRG];
    char cfg[BFLRG], db[BFLRG];
    char s[BFLRG];
    char **cnd, **sa, *file, *exe, *p;
    FILE *fp;

    p = cgetenv(TRUE, "ConfigDir");
    if (IS_NULL(p) == TRUE)
       p = "local std";
    nstrncpy(cfg,  BFLRG, p, -1);

    nstrncpy(ldbg, BFLRG, dbget(cl, TRUE, "DO_RUN_DBG"), -1);
    nstrncpy(lmpi, BFLRG, dbget(cl, TRUE, "DO_RUN_MPI"), -1);
    nstrncpy(lcrs, BFLRG, dbget(cl, TRUE, "DO_RUN_CROSS"), -1);

    separator(Log);
    note(Log, TRUE, "   ConfigDir     = %s", cfg);
    note(Log, TRUE, "   DO_RUN_DBG    = %s", ldbg);
    note(Log, TRUE, "   DO_RUN_MPI    = %s", lmpi);
    note(Log, TRUE, "   DO_RUN_CROSS  = %s", lcrs);

/* initialize the database file */
    note(Log, TRUE, "   Do-run signature database - do-run-db");

    snprintf(db, BFLRG, "%s/do-run-db", st->dir.etc);

/* see if file is properly specified via environment variable */
    p   = cgetenv(TRUE, "RUN_SIGNATURE_DB");
    ok  = TRUE;
    ok &= (file_exists(p) == TRUE);
    ok &= (strcmp(p, db) != 0);

/* copy the do-run database */
    if (ok == TRUE)
       {note(Log, TRUE, "      copying %s", p);
        copy(db, p);}

/* write the do-run database */
    else
       {note(Log, TRUE, "      generating %s", db);

	fp = fopen(db, "w");

	fprintf(fp, "#\n");
	fprintf(fp, "# DO-RUN-DB - execution signatures for do-run\n");
	fprintf(fp, "#\n");

	cnd = NULL;

/* check for debug specs */
	if ((IS_NULL(ldbg) == FALSE) && (strcmp(ldbg, "none") != 0))
	   {cnd = lst_push(cnd, ldbg);
	    fprintf(fp, "default DBG %s\n", path_base(ldbg));}
	else
	   {cnd = lst_push(cnd, "gdb");
	    cnd = lst_push(cnd, "tv");
	    cnd = lst_push(cnd, "vg");
	    cnd = lst_push(cnd, "vgd");
	    cnd = lst_push(cnd, "prf");
	    cnd = lst_push(cnd, "zf");};

/* check for MPI specs */
	if ((IS_NULL(lmpi) == FALSE) && (strcmp(lmpi, "none") != 0))
	   {cnd = lst_push(cnd, lmpi);
	    fprintf(fp, "default MPI %s\n", path_base(lmpi));}
	else
	   {cnd = lst_push(cnd, "poe");
	    cnd = lst_push(cnd, "srun");
	    cnd = lst_push(cnd, "dmpirun");
	    cnd = lst_push(cnd, "mpirun");};

/* check for cross compile specs */
	if ((IS_NULL(lcrs) == FALSE) && (strcmp(lcrs, "none") != 0))
	   {cnd = lst_push(cnd, lcrs);
	    fprintf(fp, "default CROSS %s\n", path_base(lcrs));}
	else
	   cnd = lst_push(cnd, "submit");

	ns = 0;

/* find the prolog */
	FOREACH(d, cfg, " \t\n")
	   snprintf(s, BFLRG, "%s/do-run-prolog", d);
	   if (file_exists(s) == TRUE)
              {add_do_run(fp, s);
	       ns++;
	       break;};
        ENDFOR;

/* process the specs in CND */
	nm = 0;
	for (i = 0; cnd[i] != NULL; i++)
	    {note(Log, TRUE, "Checking %s", cnd[i]);

	     file = do_run_spec(cfg, cnd[i]);
	     if (IS_NULL(file) == TRUE)
	        {nm++;
		 note(Log, TRUE, "   missing do-run config = |%s|", file);}

	     else
	        {note(Log, TRUE, "   using do-run config = |%s|", file);

		 exe = cwhich(path_base(cnd[i]));
		 note(Log, TRUE, "   candidate for %s name = |%s|",
		      cnd[i], exe);
		 if (file_executable(exe) == TRUE)
		    {add_do_run(fp, file);
		     ns++;}
		 else
		    {sa = file_text(FALSE, file);

		     for (l = 0; sa[l] != NULL; l++)
		         {if (strstr(sa[l], "Exe") != NULL)
			     {p   = strchr(sa[l], '=');
			      exe = trim(p+1, BOTH, " \t\n\r");
			      if (exe[0] != '/')
				 exe = cwhich(exe);
			      
			      note(Log, TRUE, "   candidate for %s file = |%s|",
				   cnd[i], exe);
			      if (file_executable(exe) == TRUE)
				 {add_do_run(fp, file);
				  ns++;};
			      break;};};

		     free_strings(sa);};};};

	lst_free(cnd);

	fprintf(fp, "\n");

	fclose(fp);};

/* add the signature variable to the databases iff
 * there were specification - ns > 0 
 * and there were no missing specifications - nm == 0
 */
    if ((nm == 0) && (ns > 0))
       {dbset(cl, "RUN_SIGNATURE_DB", db);
	csetenv("RUN_SIGNATURE_DB", db);
	note(st->aux.SEF, TRUE, "RUN_SIGNATURE_DB %s", db);};

    separator(Log);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ANALYZE_CONFIG - analyze the system configuration */

static void analyze_config(client *cl, char *base)
   {

    st.phase = PHASE_ANALYZE;

    separator(Log);
    noted(Log, "Analyzing system on %s", st.host);
    note(Log, TRUE, "");

    write_envf(cl, FALSE);
    
/* setup the environment for programs which analyze features */
    setup_analyze_env(cl, base);

    push_dir(st.dir.cfg);

/* read the file which does the analysis */
    if (file_exists("../analyze/program-analyze") == TRUE)
       read_config(cl, "program-analyze", TRUE);

    run(BOTH, "rm * > /dev/null 2>&1");
    pop_dir();

    noted(Log, "");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SUMMARIZE_CONFIG - make a summary report of the configuration */

static void summarize_config(client *cl)
   {

    if (file_executable("analyze/summary") == TRUE)
       printf("%s\n", run(BOTH, "analyze/summary"));

    pco_save_db(cl, NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FINISH_CONFIG - complete the configuration files */

static void finish_config(client *cl, char *base)
   {

    st.phase = PHASE_WRITE;

/* these are finally determined by now and it is safe to define them */
    snprintf(st.dir.inc, BFLRG, "%s/include", st.dir.root);
    snprintf(st.dir.etc, BFLRG, "%s/etc",     st.dir.root);
    snprintf(st.dir.lib, BFLRG, "%s/lib",     st.dir.root);
    snprintf(st.dir.bin, BFLRG, "%s/bin",     st.dir.root);
    snprintf(st.dir.sch, BFLRG, "%s/scheme",  st.dir.root);

    separator(Log);
    noted(Log, "Writing system dependent files");
    note(Log, TRUE, "");

/* if T3D, fiddle pdb fortran interface regression test source */
    if (dbcmp(cl, "PFE", "mppexec") == 0)
       {char tmpf[BFLRG];

	snprintf(tmpf, BFLRG, "tmp-%s", st.system);

	push_dir("%s/pdb", base);

        run(BOTH, "mv pdftst.f %s.f", tmpf);
        run(BOTH, "sed -e s/'iarg, iargc'/'iarg, ipxfargc, ilen, ierror'/"
            "-e s/'iargc()'/'ipxfargc()'/"
            "-e s/'getarg(iarg, arg)'/'pxfgetarg(iarg, arg, ilen, ierror)'/g"
            "%s.f > pdftst.f",
            tmpf);

       pop_dir();};

    setup_output_env(cl, base);

/* write the configured files for PACT */
    write_envf(cl, TRUE);

    LOG_OFF;

    if (file_exists("analyze/program-fin") == TRUE)
       read_config(cl, "program-fin", TRUE);

    LOG_ON;

    note(Log, TRUE, "");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LAUNCH_PERDB - look for -f arg and if found launch perdb first of all */

int launch_perdb(int c, char **v)
   {int i, l, n, ok, rv;
    char t[BFLRG];
    char *db;
    static char *sfx[] = { "db", "log", "pid" };

    ok = FALSE;

    st.launched = FALSE;

    if (cdefenv("PERDB_PATH") == FALSE)
       {n = sizeof(sfx)/sizeof(char *);

	for (i = 1; i < c; i++)
	    {if (strcmp(v[i], "-f") == 0)
	        {db = v[++i];
		 csetenv("PERDB_PATH", db);

		 for (l = 0; l < n; l++)
		     {snprintf(t, BFLRG, "%s.%s", db, sfx[l]);
		      unlink_safe(t);};

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
	if (IS_NULL(db) == FALSE)
	   {bf = run(TRUE, "perdb -f %s -l quit:", db);
	    ok = (bf != NULL);};};

    return(ok);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SIGDONE - handle signals meant to end session */

static void sigdone(int sig)
   {

    if (st.have_db == TRUE)
       kill_perdb();

    exit(sig);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DENV - print the current environment */

void denv(void)
   {int i, n;
    char **ta;

    ta = cenv(TRUE, NULL);
    if (ta != NULL)
       {n = lst_length(ta);
	for (i = 0; i < n; i++)
	    printf("> %s\n", ta[i]);

	free_strings(ta);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - give help message */

static void help(void)
   {

    printf("\n");
    printf("Usage: pact-config [-a] [-c] [-db <db>] [-f <path>] [-F] [-g] [-i <directory>] [-l] [-o] [-p] [-s <sysid>] [-v] | <cfg>\n");
    printf("\n");
    printf("             -a      do NOT perform PACT-ANALYZE step\n");
    printf("             -c      create missing directories for -i option\n");
    printf("             -db     configure using database <db>\n");
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

int main(int c, char **v, char **env)
   {int i, append, ok;
    char base[BFLRG], ib[BFLRG], d[BFLRG];
    char *root, *strct;
    client *cl;

    if (c <= 1)
       {help();
	return(1);};

    signal(SIGSEGV, sigdone);
    signal(SIGBUS,  sigdone);
    signal(SIGFPE,  sigdone);
    signal(SIGTERM, sigdone);
    signal(SIGINT,  sigdone);

    root = cgetenv(TRUE, "PERDB_PATH");
    cl   = make_client(CLIENT, DB_PORT, FALSE, root, cl_logger);

    ok = reset_env(c, v);
    if (ok == -1)
       {printf("Could not clear environment - exiting\n");
	return(1);};

/* locate the tools needed for subshells */
    build_path(NULL,
	       "sed", "grep", "awk", "sort",
	       "ls", "cp", "rm", "mv",
	       "find", "chmod", "cat",
	       "env", "mkdir", "nm", "perdb",
	       NULL);

    st.have_db = launch_perdb(c, v);

/* technically this is set by 'dsys config' */
    cinitenv("DbgOpt", "-g");

/* NOTE: because of OSX's nefarious automounter we have to get the current
 * directory this way (rather that via the getcwd library call) so that
 * we get the same value that shell scripts get in other parts of the
 * config process - and consistency is essential
 */
    nstrncpy(st.dir.mng, BFLRG, run(BOTH, "pwd"), -1);

    nstrncpy(base, BFLRG, path_head(st.dir.mng), -1);
    strcpy(st.cfgf, "DEFAULT");

    nstrncpy(ib, BFLRG, "none", -1);
    strct  = "0";
    append = FALSE;

    st.na   = c - 1;
    st.args = v + 1;

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-strict") == 0)
	    strct = v[++i];

         else if (strcmp(v[i], "-db") == 0)
	    {full_path(d, BFLRG, NULL, v[++i]);
             if (file_exists(d) == FALSE)
	        {noted(Log, "No such database '%s' - exiting\n", d);
		 if (st.have_db == TRUE)
		    kill_perdb();
		 return(1);};
	     st.db = d;
	     dbset(cl, "CONFIG_METHOD", "database");}
 
/* this was handled in reset_env */
         else if (strcmp(v[i], "-env") == 0)
            i++;
 
	 else if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'a':
		       st.analyzep = TRUE;
                       break;

                 case 'c':
                      st.create_dirs = TRUE;
                      break;
 
/* this was handled in launch_perdb */
                 case 'f':
		      i++;
                      break;
 
                 case 'F':
                      st.tmp_dirp = FALSE;
                      break;
 
                 case 'g':
                      st.abs_deb = TRUE;
                      break;
 
                 case 'i':
		      nstrncpy(ib, BFLRG, v[++i], -1);
                      break;
 
                 case 'l':
                      append    = TRUE;
                      svs.debug = TRUE;
                      break;
 
                 case 'o':
                      st.abs_opt = TRUE;
                      break;
 
                 case 'p':
                      st.profilep = TRUE;
                      break;
 
                 case 's':
                      nstrncpy(st.system, BFLRG, v[++i], -1);
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
	    {nstrncpy(st.cfgf, BFLRG, v[i], -1);
	     dbset(cl, "CONFIG_METHOD", "file");};};

    set_inst_base(cl, ib);

    init_session(cl, base, append);

/* make config directory */
    snprintf(st.dir.cfg, BFLRG, "cfg-%s", st.system);
    run(BOTH, "rm -rf %s", st.dir.cfg);
    run(BOTH, "mkdir %s", st.dir.cfg);

    if (st.db == NULL)
       {dbset(cl, "STRICT", strct);

	push_file(st.cfgf, STACK_FILE);
	strcpy(st.cfgf, st.fstck.file[st.fstck.n-1].name);
	pop_file();

	if (file_exists("analyze/program-init") == TRUE)
	   read_config(cl, "program-init", TRUE);

	read_config(cl, st.cfgf, FALSE);

        write_do_run_db(cl, &st);

	ok = check_cross(cl);
	if (ok == FALSE)
	   {free_client(cl);

	    if (st.have_db == TRUE)
	       kill_perdb();

	    return(2);};

	pco_save_db(cl, "inp");

	noted(Log, "");

	check_dir(cl);

	if (st.analyzep == FALSE)
	   analyze_config(cl, base);}

    else
       {pco_load_db(cl, st.db);

        write_do_run_db(cl, &st);

/* order matters crucially here */
        env_subst(cl, "InstBase",      ib);
        env_subst(cl, "SysDir",        st.dir.root);
        env_subst(cl, "BaseDir",       base);
        env_subst(cl, "System",        st.system);
        env_subst(cl, "CONFIG_METHOD", "database");

	snprintf(st.dir.cfg, BFLRG, "cfg-%s", st.system);
	nstrncpy(st.cfgf,    BFLRG, cgetenv(FALSE, "CONFIG_FILE"), -1);

/* reset the rules */
	snprintf(st.rules.ccp, BFLRG, "\t%s\n", cgetenv(FALSE, "CCP"));
	snprintf(st.rules.co,  BFLRG, "\t%s\n", cgetenv(FALSE, "CCObj"));
	snprintf(st.rules.ca,  BFLRG, "\t%s\n", cgetenv(FALSE, "CCArc"));
	snprintf(st.rules.lo,  BFLRG, "\t%s\n", cgetenv(FALSE, "LexObj"));
	snprintf(st.rules.la,  BFLRG, "\t%s\n", cgetenv(FALSE, "LexArc"));
	snprintf(st.rules.lc,  BFLRG, "\t%s\n", cgetenv(FALSE, "LexC"));
	snprintf(st.rules.yo,  BFLRG, "\t%s\n", cgetenv(FALSE, "YaccObj"));
	snprintf(st.rules.ya,  BFLRG, "\t%s\n", cgetenv(FALSE, "YaccArc"));
	snprintf(st.rules.yc,  BFLRG, "\t%s\n", cgetenv(FALSE, "YaccC"));
	snprintf(st.rules.fo,  BFLRG, "\t%s\n", cgetenv(FALSE, "FCObj"));
	snprintf(st.rules.fa,  BFLRG, "\t%s\n", cgetenv(FALSE, "FCArc"));
	snprintf(st.rules.th,  BFLRG, "\t%s\n", cgetenv(FALSE, "TemplH"));

	snprintf(st.rules.co_bp, BFLRG, "\t%s\n", cgetenv(FALSE, "CCObj_BP"));
	snprintf(st.rules.ca_bp, BFLRG, "\t%s\n", cgetenv(FALSE, "CCArc_BP"));
	snprintf(st.rules.lo_bp, BFLRG, "\t%s\n", cgetenv(FALSE, "LexObj_BP"));
	snprintf(st.rules.la_bp, BFLRG, "\t%s\n", cgetenv(FALSE, "LexArc_BP"));
	snprintf(st.rules.yo_bp, BFLRG, "\t%s\n", cgetenv(FALSE, "YaccObj_BP"));
	snprintf(st.rules.ya_bp, BFLRG, "\t%s\n", cgetenv(FALSE, "YaccArc_BP"));

/* place the make variables */
        reset_make_vars();

	check_dir(cl);};

    summarize_config(cl);

    finish_config(cl, base);

    separator(Log);
    run(BOTH, "rm -rf %s", st.dir.cfg);

    LOG_OFF;

    free_client(cl);

    if (st.have_db == TRUE)
       kill_perdb();

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

