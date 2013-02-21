/*
 * PERDB.C - implement persistent database access
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#include "libdb.c"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SIGDONE - handle signals meant to end session */

static void sigdone(int sig)
   {client *cl;
    database *db;

    cl = svs.cl;
    db = (database *) cl->a;
    if (db != NULL)
       {CLOG(cl, 4, "signalled %d - done", sig);
	close_sock(cl);
	db_srv_save(-1, db);
	unlink_safe(cl->fcon);};

    exit(sig);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SIGRESTART - handle signals meant to restart session */

static void sigrestart(int sig)
   {database *db;

    db = (database *) svs.cl->a;

    db_srv_restart(db);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* VERIFYX - verify the transaction request
 *         - return TRUE if ANS is correct
 */

int verifyx(client *cl, int nc, char *ans, char *res)
   {int nk, no, nt, rv;
    char lres[BFLRG];
    char *key, *t;

    rv = TRUE;

    nk  = cl->nkey;
    key = cl->key;

/* without authentication */
    if ((nk == 0) && (key == NULL))
       rv = TRUE;

/* client wants the answer */
    else if (res != NULL)
       {snprintf(res, nk+1, "%s", key);
	rv = TRUE;}

/* server matches the answer */
    else
       {snprintf(lres, nk+1, "%s", key);

	no = 0;
	nt = nc + nk + 2;
	t  = MAKE_N(char, nt);

	nstrncpy(t, nt, ans, -1);

	rv = (svs.auth == FALSE);
	if (strncmp(t, "auth:", 5) == 0)
	   {rv = FALSE;
	    if (svs.auth == TRUE)
	       {no = nk + 6;
		rv = (strncmp(lres, ans+5, nk) == 0);
		memmove(t, t+no, nc-no);};};

	if ((t != NULL) && (strcmp(ans, t) != 0))
	   nstrncpy(ans, nc, t, -1);

	FREE(t);};

    if (rv == FALSE)
       CLOG(cl, 1, "access denied - bad key");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SRV_SAVE_DB - save the database
 *             - destination is controlled by FNAME
 *             - examples:
 *             -     FNAME = NULL   -> <stdout>
 *             -     FNAME = /a/b/c -> /a/b/c
 *             -     FNAME = foo    -> <root>.foo.db
 *             -     FNAME = @      -> <root>.db
 */

static char *srv_save_db(database *db, char *fname,
			 char **var, const char *fmt)
   {int ok;
    char s[BFLRG];
    FILE *fp;
    static char t[BFLRG];

/* figure out where results go */
    if ((fname == NULL) || (strcmp(fname, "stdout") == 0))
       fp = NULL;
    else
       {if (fname[0] == '/')
	   nstrncpy(s, BFLRG, fname, -1);
	else if (strcmp(fname, "@") == 0)
	   snprintf(s, BFLRG, "%s.db", db->cl->root);
        else
	   snprintf(s, BFLRG, "%s.%s.db", db->cl->root, fname);

	fp = fopen(s, "w");
	if (fp == NULL)
	   {snprintf(t, BFLRG, "could not open %s - save %s",
		     s, strerror(errno));
	    return(t);};
        fname = path_tail(s);};
	   
/* do the work */
    ok = save_db(db, var, fp, fmt);
    ASSERT(ok == 0);

    if (fp != NULL)
       fclose(fp);
	
/* make up the ending message */
    if ((IS_NULL(fmt) == FALSE) &&
	((strncmp(fmt, "setenv ", 7) == 0) ||
	 (strncmp(fmt, "export ", 7) == 0)))
       snprintf(t, BFLRG, "%s", cwhich("true"));

     else if (fname == NULL)
       {if (var == NULL)
	   snprintf(t, BFLRG, "saved database");
        else
	   snprintf(t, BFLRG, "saved variables");}

    else
       {if (var == NULL)
	   snprintf(t, BFLRG, "saved database to %s",
		    fname);
	else
	   snprintf(t, BFLRG, "saved variables to %s",
		    fname);};

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SRV_LOAD_DB - load the database */

static char *srv_load_db(client *cl, char *fname, char *var)
   {char s[BFLRG];
    char *root;
    FILE *fp;
    database *db;
    static char t[BFLRG];

    db   = (database *) cl->a;
    root = cl->root;

    if (fname == NULL)
       fp = NULL;

    else
       {if (file_exists(fname) == TRUE)
	   nstrncpy(s, BFLRG, fname, -1);
        else
	   snprintf(s, BFLRG, "%s.%s.db", root, fname);

	fp = fopen(s, "r");
        if (fp == NULL)
	   {snprintf(t, BFLRG, "could not open %s - load %s",
		     s, strerror(errno));
	    return(t);};
        fname = path_tail(s);};
	   
    load_db(db, var, fp);

    if (fname == NULL)
       {if (var == NULL)
	   snprintf(t, BFLRG, "loaded database");
        else
	   snprintf(t, BFLRG, "loaded %s", var);}

    else
       {if (var == NULL)
	   snprintf(t, BFLRG, "loaded database from %s",
		    fname);
	else
	   snprintf(t, BFLRG, "loaded %s from %s",
		    var, fname);};

    if (fp != NULL)
       fclose(fp);
	
    return(t);}

/*--------------------------------------------------------------------------*/

/*                               SERVER METHODS                             */

/*--------------------------------------------------------------------------*/

/* SRV_LOGGER - log messages in the server SV */

static void srv_logger(srvdes *sv, int lvl, char *fmt, ...)
   {char s[BFLRG];
    char *root, *flog;
    client *cl;

    VA_START(fmt);
    VSNPRINTF(s, BFLRG, fmt);
    VA_END;

    cl   = (client *) sv->a;
    root = cl->root;
    flog = name_log(root);

    log_activity(flog, svs.debug, lvl, "SERVER", s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SRV_SETUP - setup the server SV */

static void srv_setup(srvdes *sv, int *ptmax, int *pdt)
   {int dt, tmax;
    char *s;

/* set idle timeout and idle interval
 * if no input comes in within the idle timeout window stop and return
 * have the select timeout at the idle interval
 */
    s = cgetenv(FALSE, "PERDB_IDLE_TIMEOUT");
    if (IS_NULL(s) == TRUE)
       tmax = 60;
    else
       {tmax = abs(atoi(s));
	SLOG(sv, 4, "PERDB_IDLE_TIMEOUT = %d", tmax);};

    s = cgetenv(FALSE, "PERDB_IDLE_INTERVAL");
    if (IS_NULL(s) == TRUE)
       dt = tmax >> 1;
    else
       {dt = atoi(s);
	SLOG(sv, 4, "PERDB_IDLE_INTERVAL = %d", dt);};

    if (ptmax != NULL)
       *ptmax = tmax;

    if (pdt != NULL)
       *pdt = dt;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PARSE_DB_SPEC - parse a database specifier
 *                - syntax of S is
 *                -    [<opt>] <var>*
 *                -    <opt>    := [<syntax>:]<inf>:
 *                -    <syntax> := db | csh | sh
 *                -    <inf>    := stdin | stdout | stderr | <db> | <file>
 *                -    <db>     := <identifier>
 *                -    <file>   := <full-path>
 *                -    <var>    := variable name to be saved
 */

static void _parse_db_spec(char *s, char **pp, char **pfname, char **pfmt,
			   char ***popt, char ***pvar)
   {int lo, no;
    char *p, *fname, *fmt, **opt, **var;
    static char *defmt[] = { "%s=%s", "setenv %s %s ;", "export %s=%s ;" };
    static char *def_db = "@";

    p     = s;
    fname = NULL;
    fmt   = defmt[0];
    opt   = NULL;
    var   = NULL;

/* variable list */
    p = strpbrk(s, " \t\f");
    if (p != NULL)
       *p++ = '\0';

/* options */
    if (strchr(s, ':') != NULL)
       {opt = tokenize(s, ":", 0);
	no  = lst_length(opt);
	lo  = 0;

/* determine format */
	if ((no - lo > 0) && (opt[lo] != NULL))
	   {if (strcmp(opt[lo], "csh") == 0)
	       {fmt = defmt[1];
		lo++;}
	    else if (strcmp(opt[lo], "sh") == 0)
	       {fmt = defmt[2];
		lo++;}
	    else if (strcmp(opt[lo], "db") == 0)
	       {fmt   = defmt[0];
		fname = def_db;
		lo++;};};

/* determine file */
	if ((no - lo > 0) && (opt[lo] != NULL))
	   fname = trim(opt[lo++], FRONT, " \t");};

    if (pp != NULL)
       *pp = trim(p, FRONT, " \t");
    if (pfname != NULL)
       *pfname = fname;
    if (pfmt != NULL)
       *pfmt = fmt;
    if (popt != NULL)
       *popt = opt;
    if (pvar != NULL)
       *pvar = var;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO_SAVE - save the database variables specified by S
 *         - syntax of S is
 *         -    [<opt>] <var>*
 *         -    <opt>    := [<syntax>:]<inf>:
 *         -    <syntax> := db | csh | sh
 *         -    <inf>    := stdin | stdout | stderr | <db> | <file>
 *         -    <db>     := <identifier>
 *         -    <file>   := <full-path>
 *         -    <var>    := variable name to be saved
 *         - if <inf> is stdout or stderr dumps to that device
 *         - else if <inf> is full path saves to specified file
 *         - otherwise database will be named <root>.<db>.db
 *         - the default is <root>.db
 *         - if no variables are specified all are saved
 */

static char *do_save(database *db, char *s)
   {char *fmt, *val, *fname, *p;
    char **var, **opt;

    _parse_db_spec(s, &p, &fname, &fmt, &opt, &var);

    var = tokenize(p, " \t\n\f", 0);
    if ((var != NULL) && (var[0] == NULL))
       {free_strings(var);
        var = NULL;};

    val = srv_save_db(db, fname, var, fmt);

    free_strings(opt);
    free_strings(var);

    return(val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO_LOAD - load the database variables
 *         - syntax of S is
 *         -    [<opt>] <var>*
 *         -    <opt>    := [<syntax>:]<inf>:
 *         -    <syntax> := db | csh | sh
 *         -    <inf>    := stdin | stdout | stderr | <db> | <file>
 *         -    <db>     := <identifier>
 *         -    <file>   := <full-path>
 *         -     <var>   := variable name to be saved
 *         - if <inf> is stdin load from that device
 *         - else if <inf> is full path load from specified file
 *         - otherwise database will be named <root>.<db>.db
 *         - the default is <root>.db
 *         - if no variables are specified all are loaded
 */

static char *do_load(client *cl, char *s)
   {char *var, *val, *fname, *p;
    char **opt;

    _parse_db_spec(s, &p, &fname, NULL, &opt, NULL);

    key_val(&var, NULL, p, " \t\n");

    val = srv_load_db(cl, fname, var);

    free_strings(opt);

    return(val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RENDER_VAL - render database value, VAL, according to FMT */

static char *render_val(char *var, char *val, char *fmt)
   {char *s;
    static char t[BFLRG];

    s = val;
    if (fmt != NULL)
       {if (strcmp(fmt, "csh") == 0)
	   {snprintf(t, BFLRG, "setenv %s \"%s\" ; ",
		     var, strip_quote(val));
	    s = t;}
	else if (strcmp(fmt, "sh") == 0)
	   {snprintf(t, BFLRG, "%s=\"%s\" ; export %s ; ",
		     var, strip_quote(val), var);
	    s = t;}
	else if (strcmp(fmt, "pl") == 0)
	   {snprintf(t, BFLRG, "$%s = \"%s\"; ",
		     var, strip_quote(val));
	    s = t;}
	else if (strcmp(fmt, "db") == 0)
	   {snprintf(t, BFLRG, "%s=%s", var, val);
	    s = t;};};

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO_COND_INIT - handle conditional initialization */

static char *do_cond_init(database *db, char *s, char *fmt)
   {int st;
    char *var, *val, *nvl;

    key_val(&var, &nvl, s, "=? \t\n");

    val = get_db(db, var);

    st = (val != cnoval());
    if (st == FALSE)
       val = put_db(db, var, nvl);

    val = render_val(var, val, fmt);

    return(val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO_DEFD - handle defined query */

static char *do_defd(database *db, char *s, char *fmt)
   {int st;
    char *var, *val, *nvl;

    key_val(&var, &nvl, s, "? \t\n");

    val = get_db(db, var);

    st = (val != cnoval());
    if (IS_NULL(nvl) == TRUE)
       {if (st == TRUE)
	   val = "defined{TRUE}";
        else
	   val = "defined{FALSE}";}

    else if (st == FALSE)
       val = put_db(db, var, nvl);

    val = render_val(var, val, fmt);

    return(val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO_SET_GET - handle variable set or get operations */

static char *do_set_get(database *db, char *s, char *fmt)
   {char *var, *val;
    static char t[BFLRG];

    key_val(&var, &val, s, "= \t\n");

    if (val == NULL)
       val = get_db(db, var);

    else
       {val = trim(val, BACK, " \t");
	val = put_db(db, var, val);};

    if ((val != NULL) &&
	(strchr("'\"(", val[0]) == NULL) && 
	(strpbrk(val, " \t") != NULL))
       {snprintf(t, BFLRG, "\"%s\"", val);
	val = t;};

    val = render_val(var, val, fmt);

    return(val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO_VAR_ACC - handle variable access: set, get, condi, defd */

static char **do_var_acc(database *db, char *s)
   {int is, ns;
    char *t, *fmt, **val, **sa;

/* multi-character delimiter */
    if (strncmp(s, "dlm:", 4) == 0)
       {char *dlm, *u;

	t  = s + 4;
	ns = strcspn(t, " \t\f\n");
	u  = t + ns;
	*u++ = '\0';
	dlm = STRSAVE(t);
	sa = tokenized(u, dlm, 0);
	FREE(dlm);}

/* single-character delimiter */
    else
       {char c, dl[2];

        c = s[0];
	if (strchr(",.:;-_+~^@/|<>", c) != NULL)
	   dl[0] = c;
	else
	   dl[0] = '^';
	dl[1] = '\0';
	sa = tokenize(s, dl, 0);};

/* process list of accesses */
    val = NULL;
    fmt = NULL;

    ns = lst_length(sa);
    for (is = 0; is < ns; is++)
        {t = sa[is];

	 if (strncmp(t, "fmt:", 4) == 0)
	    {fmt = STRSAVE(t+4);
	     continue;}

/* variable conditional init */
	 if (strstr(t, "=\?") != NULL)
	    val = lst_push(val, do_cond_init(db, t, fmt));

/* variable defined? */
	 else if (strchr(t, '?') != NULL)
	    val = lst_push(val, do_defd(db, t, fmt));

/* variable set/get */
	 else
	    val = lst_push(val, do_set_get(db, t, fmt));};

    FREE(fmt);
    free_strings(sa);

    return(val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SRV_PROCCESS - process transaction request S in the server SV
 *              - on client connection CL
 *              - return the response strings in VAL
 */

static char **srv_process(srvdes *sv, char *s)
   {char **val;
    database *db;
    client *cl;

    val = NULL;

    if ((sv != NULL) && (s != NULL))
       {cl = (client *) sv->a;
	db = (database *) cl->a;

	if (strncmp(s, "load:", 5) == 0)
	   val = lst_push(val, do_load(cl, s+5));

/* reset database */
	else if (strncmp(s, "reset:", 6) == 0)
	   {reset_db(db);
	    val = lst_push(val, "reset");}

/* save database */
	else if (strncmp(s, "save:", 5) == 0)
	   val = lst_push(val, do_save(db, s+5));

	else
	   val = do_var_acc(db, s);};

    return(val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SERVER - run the server side of the database
 *        - return TRUE iff successful
 */

static int server(char *root, int init, int dmn)
   {int rv;
    client *cl;
    srvdes sv;

    rv = FALSE;

    if ((dmn == FALSE) || (demonize() == TRUE))
       {signal(SIGTERM, sigdone);
	signal(SIGINT, sigdone);
	signal(SIGHUP, sigrestart);

	cl     = make_client(SERVER, DB_PORT, svs.auth,
			     root, cl_logger, verifyx);
	svs.cl = cl;

	CLOG(cl, 1, "start server");

	if (db_srv_open(svs.cl, init, svs.debug, svs.auth) == TRUE)
	   {database *db;

	    db = (database *) cl->a;

	    sv.a       = cl;
	    sv.slog    = srv_logger;
            sv.setup   = srv_setup;
            sv.process = srv_process;

	    async_server(&sv);

	    db_srv_save(-1, db);
	    db_srv_close(db);

	    cl = NULL;
	    rv = TRUE;};

	svs.cl = NULL;

	CLOG(cl, 1, "end server");};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXCHANGE - do a transaction from the client side of the database
 *          - if LTR is TRUE strip off the quotes the server side added
 */

static int exchange(char *root, int ltr, char *req)
   {int i, n, rv;
    char **ta;
    client *cl;

    cl = make_client(CLIENT, DB_PORT, svs.auth,
		     root, cl_logger, verifyx);
    ta = _db_clnt_ex(cl, TRUE, req);
    free_client(cl);

    rv = (ta != NULL);

/* print the reply */
    n = lst_length(ta);
    for (i = 0; i < n; i++)
        {if (strcmp(ta[i], "defined{TRUE}") == 0)
	    rv = TRUE;
	 else if (strcmp(ta[i], "defined{FALSE}") == 0)
	    rv = FALSE;
	 else if (ltr == TRUE)
	    printf("%s\n", strip_quote(ta[i]));
	 else
	    printf("%s\n", ta[i]);};

    lst_free(ta);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - print usage info */

static void help(void)
   {
    printf("\n");
    printf("Usage: perdb [-a] [-c] [-d] [-e] [-f <db>] [-h] [-l] [-s] [<req>]\n");
    printf("   a   authenticate transactions\n");
    printf("   c   create the database, removing any existing\n");
    printf("   d   do not run server as daemon\n");
    printf("   e   exact - do not strip off quotes\n");
    printf("   f   root path to database\n");
    printf("   h   this help message\n");
    printf("   l   log transactions with the server\n");
    printf("   s   run as server\n");
    printf("\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i, rv, srv, dmn, init, ltr;
    char root[BFLRG], r[BFLRG], req[BFLRG];
    char *except[] = {"PATH", "PERDB_IDLE_TIMEOUT",
		      "PERDB_IDLE_INTERVAL", "PERDB_PATH",
		      NULL};

    r[0] = '\0';
    srv  = FALSE;
    init = FALSE;
    dmn  = TRUE;
    ltr  = FALSE;

/* everything up to the first token not beginning with '-' is a flag */
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
	    {switch (v[i][1])
                {case 'a' :
                      svs.auth = TRUE;
		      break;
                 case 'c' :
                      init = TRUE;
		      break;
                 case 'd' :
                      dmn = FALSE;
		      break;
                 case 'e' :
                      ltr = TRUE;
		      break;
                 case 'f' :
                      nstrncpy(r, BFLRG, v[++i], -1);
		      break;
		 case 'h' :
                      help();
                      return(1);
		 case 'l' :
                      svs.debug = TRUE;
                      break;
		 case 's' :
                      srv = TRUE;
                      break;};}
	 else
	    break;};

/* everything from here is the request */
    req[0] = '\0';
    for (; i < c; i++)
        {nstrcat(req, BFLRG, v[i]);
	 nstrcat(req, BFLRG, " ");};

    if (IS_NULL(r) == TRUE)
       {if (cdefenv("PERDB_PATH") == TRUE)
           nstrncpy(r, BFLRG, cgetenv(TRUE, "PERDB_PATH"), -1);};

    if (IS_NULL(r) == TRUE)
       snprintf(r, BFLRG, "%s/.perdb", cgetenv(TRUE, "HOME"));

    full_path(root, BFLRG, NULL, r);

    rv = cclearenv(except);

    rv = TRUE;
    if (srv == TRUE)
       rv = server(root, init, dmn);

    else if (IS_NULL(req) == FALSE)
      {LAST_CHAR(req) = '\0';
       rv = exchange(root, ltr, req);};

/* reverse sense for exit status */
    rv = (rv == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
