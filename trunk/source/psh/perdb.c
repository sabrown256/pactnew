/*
 * PERDB.C - implement persistent database access
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#include "common.h"
#include "libpsh.c"
#include "libdb.c"

static database
 *db = NULL;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SIGDONE - handle signals meant to end session */

static void sigdone(int sig)
   {

    if (db != NULL)
       {log_activity(db->flog, dbs.debug, 4,
		     "SERVER", "signalled %d - done", sig);
	close_sock(db->root);
	db_srv_save(-1, db);
	unlink_safe(db->fcon);};

    exit(sig);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SIGRESTART - handle signals meant to restart session */

static void sigrestart(int sig)
   {

    db_srv_restart(db);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SRV_SAVE_DB - save the database */

static char *srv_save_db(client *cl, char *fname, char **var, const char *fmt)
   {int fd, ok;
    char s[MAXLINE];
    FILE *fp;
    database *db;
    static char t[MAXLINE];

    fd = cl->fd;
    db = cl->db;

/* figure out where results go */
    if ((fname == NULL) || (strcmp(fname, "stdout") == 0))
       fp = NULL;
    else
       {if (fname[0] == '/')
	   nstrncpy(s, MAXLINE, fname, -1);
        else
	   snprintf(s, MAXLINE, "%s.%s.db", db->root, fname);

	fp = fopen(s, "w");
	if (fp == NULL)
	   {snprintf(t, MAXLINE, "could not open %s - save %s",
		     s, strerror(errno));
	    return(t);};
        fname = path_tail(s);};
	   
/* do the work */
    ok = save_db(fd, db, var, fp, fmt);
    ASSERT(ok == 0);

    if (fp != NULL)
       fclose(fp);
	
/* make up the ending message */
    if ((IS_NULL(fmt) == FALSE) &&
	((strncmp(fmt, "setenv ", 7) == 0) ||
	 (strncmp(fmt, "export ", 7) == 0)))
       snprintf(t, MAXLINE, "%s", cwhich("true"));

     else if (fname == NULL)
       {if (var == NULL)
	   snprintf(t, MAXLINE, "saved database");
        else
	   snprintf(t, MAXLINE, "saved variables");}

    else
       {if (var == NULL)
	   snprintf(t, MAXLINE, "saved database to %s",
		    fname);
	else
	   snprintf(t, MAXLINE, "saved variables to %s",
		    fname);};

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SRV_LOAD_DB - load the database */

static char *srv_load_db(client *cl, char *fname, char *var)
   {char s[MAXLINE];
    char *root;
    FILE *fp;
    database *db;
    static char t[MAXLINE];

    db   = cl->db;
    root = cl->root;

    if (fname == NULL)
       fp = NULL;

    else
       {if (file_exists(fname) == TRUE)
	   nstrncpy(s, MAXLINE, fname, -1);
        else
	   snprintf(s, MAXLINE, "%s.%s.db", root, fname);

	fp = fopen(s, "r");
        if (fp == NULL)
	   {snprintf(t, MAXLINE, "could not open %s - load %s",
		     s, strerror(errno));
	    return(t);};
        fname = path_tail(s);};
	   
    load_db(db, var, fp);

    if (fname == NULL)
       {if (var == NULL)
	   snprintf(t, MAXLINE, "loaded database");
        else
	   snprintf(t, MAXLINE, "loaded %s", var);}

    else
       {if (var == NULL)
	   snprintf(t, MAXLINE, "loaded database from %s",
		    fname);
	else
	   snprintf(t, MAXLINE, "loaded %s from %s",
		    var, fname);};

    if (fp != NULL)
       fclose(fp);
	
    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHECK_FD - verify/log fd set in SRV */

static void check_fd(connection *srv, char *flog)
   {int fd;
    char s[MAXLINE];
    fd_set rfs;

    rfs = srv->afs;

    s[0] = '\0';
    for (fd = 0; fd < FD_SETSIZE; ++fd)
        {if (FD_ISSET(fd, &rfs))
            vstrcat(s, MAXLINE, " %d", fd);};

    log_activity(flog, dbs.debug, 4, "SERVER", "monitoring: %s", s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ADD_FD - add FD to the set in SRV */

static void add_fd(connection *srv, char *flog, int fd)
   {

    FD_SET(fd, &srv->afs);
    log_activity(flog, dbs.debug, 1, "SERVER", "add fd %d", fd);

    check_fd(srv, flog);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REMOVE_FD - add FD to the set in SRV */

static void remove_fd(connection *srv, char *flog, int fd)
   {

    FD_CLR(fd, &srv->afs);
    log_activity(flog, dbs.debug, 1, "SERVER", "remove fd %d", fd);

    check_fd(srv, flog);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NEW_CONNECTION - make a new connection on SFD and add it to AFS */

static void new_connection(char *root, connection *srv)
   {int fd;
    char *flog;
    socklen_t sz;

    flog = name_log(root);

    sz = sizeof(srv->sck);

    fd = accept(srv->server, (struct sockaddr *) &srv->sck, &sz);
    if (fd > 0)
       add_fd(srv, flog, fd);
    else
       {close(fd);
	log_activity(flog, dbs.debug, 1, "SERVER",
		     "accept error - %s (%d)",
		     strerror(errno), errno);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TERM_CONNECTION - finish off client connection CL */

static void term_connection(client *cl)
   {int fd;
    char *root, *flog;
    connection *srv;

    fd   = cl->fd;
    root = cl->root;
    srv  = cl->server;
    flog = name_log(root);

    remove_fd(srv, flog, fd);

    cl->fd = connect_close(fd, cl, NULL);

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
 *                -     <var>   := variable name to be saved
 */

static void _parse_db_spec(char *s, char **pp, char **pfname, char **pfmt,
			   char ***popt, char ***pvar)
   {int lo, no;
    char *p, *fname, *fmt, **opt, **var;

    p     = s;
    fname = NULL;
    fmt   = "%s=%s";
    opt   = NULL;
    var   = NULL;

    if (strchr(p, ':') != NULL)
       {opt = tokenize(p, ":");
	no  = lst_length(opt);
	lo  = 0;

/* determine format */
	if ((no - lo > 0) && (opt[lo] != NULL))
	   {if (strcmp(opt[lo], "csh") == 0)
	       {fmt = "setenv %s %s ;";
		lo++;}
	    else if (strcmp(opt[lo], "sh") == 0)
	       {fmt = "export %s=%s ;";
		lo++;}
	    else if (strcmp(opt[lo], "db") == 0)
	       lo++;};

/* determine file */
	if ((no - lo > 0) && (opt[lo] != NULL))
	   fname = opt[lo++];

/* variable list */
        p = opt[lo];};

    if (pp != NULL)
       *pp = p;
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
 *         -     <var>   := variable name to be saved
 *         - if <inf> is stdout or stderr dumps to that device
 *         - else if <inf> is full path saves to specified file
 *         - otherwise database will be named <root>.<db>.db
 *         - the default is <root>.db
 *         - if no variables are specified all are saved
 */

static char *do_save(client *cl, char *s)
   {char *fmt, *val, *fname, *p;
    char **var, **opt;

    _parse_db_spec(s, &p, &fname, &fmt, &opt, &var);

    var = tokenize(p, " \t\n\f");
    if ((var != NULL) && (var[0] == NULL))
       {free_strings(var);
        var = NULL;};

    val = srv_save_db(cl, fname, var, fmt);

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
    static char t[LRG];

    s = val;
    if (fmt != NULL)
       {if (strcmp(fmt, "csh") == 0)
	   {snprintf(t, LRG, "setenv %s \"%s\" ; ",
		     var, strip_quote(val));
	    s = t;}
	else if (strcmp(fmt, "sh") == 0)
	   {snprintf(t, LRG, "%s=\"%s\" ; export %s ; ",
		     var, strip_quote(val), var);
	    s = t;}
	else if (strcmp(fmt, "pl") == 0)
	   {snprintf(t, LRG, "$%s = \"%s\"; ",
		     var, strip_quote(val));
	    s = t;}
	else if (strcmp(fmt, "db") == 0)
	   {snprintf(t, LRG, "%s=%s", var, val);
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
    static char t[MAXLINE];

    key_val(&var, &val, s, "= \t\n");

    if (val == NULL)
       val = get_db(db, var);

    else
       {val = trim(val, BACK, " \t");
	val = put_db(db, var, val);};

    if ((val != NULL) &&
	(strchr("'\"(", val[0]) == NULL) && 
	(strpbrk(val, " \t") != NULL))
       {snprintf(t, MAXLINE, "\"%s\"", val);
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
	sa = tokenized(u, dlm);
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
	sa = tokenize(s, dl);};

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

/* PROC_CONNECTION - process input on client connection CL
 *                 - return -1 on error
 *                 -         0 on end of connection
 *                 -         1 if otherwise successful
 */

static int proc_connection(client *cl)
   {int rv, nb, to;
    char s[MAXLINE];
    char **val;
    database *db;

    db = cl->db;

    to = 60;
    rv = 1;

    memset(s, 0, MAXLINE);

    nb = comm_read(cl, s, MAXLINE, to);

/* decide on the action to take */
    if (nb <= 0)
       rv = -1;

    else
       {val = NULL;

/* client is exiting */
	if (strncmp(s, "fin:", 4) == 0)
	   term_connection(cl);

/* load database */
	else if (strncmp(s, "load:", 5) == 0)
           val = lst_push(val, do_load(cl, s+5));

/* quit session */
	else if (strncmp(s, "quit:", 5) == 0)
	   {rv  = 0;
	    val = lst_push(val, "done");}

/* reset database */
        else if (strncmp(s, "reset:", 6) == 0)
	   {reset_db(db);
	    val = lst_push(val, "reset");}

/* save database */
	else if (strncmp(s, "save:", 5) == 0)
	   val = lst_push(val, do_save(cl, s+5));

	else
	   val = do_var_acc(db, s);

	if (val != NULL)
	   {int i, nv;

	    nv = lst_length(val);
	    for (i = 0; i < nv; i++)
	        nb = comm_write(cl, val[i], 0, 10);
	    nb = comm_write(cl, EOM, 0, 10);

	    free_strings(val);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ASYNC_SERVER - run a fully asynchronous server */

static void async_server(client *cl)
   {int fd, nr, ok;
    char *root, *flog;
    fd_set rfs;
    connection *srv;

    root = cl->root;
    srv  = cl->server;

    flog = name_log(root);

    if (listen(srv->server, 5) >= 0)
       {int ng, nb, nbmax, dt, tmax;
	char *s;
        struct timeval t;

/* set idle timeout and idle interval
 * if no input comes in within the idle timeout window stop and return
 * have the select timeout at the idle interval
 */
        s = cgetenv(FALSE, "PERDB_IDLE_TIMEOUT");
	if (IS_NULL(s) == TRUE)
	   tmax = 60;
	else
	   {tmax = atoi(s);
	    log_activity(flog, dbs.debug, 4, "SERVER",
			 "PERDB_IDLE_TIMEOUT = %d", tmax);};

        s = cgetenv(FALSE, "PERDB_IDLE_INTERVAL");
	if (IS_NULL(s) == TRUE)
	   dt = tmax >> 1;
	else
	   {dt = atoi(s);
	    log_activity(flog, dbs.debug, 4, "SERVER",
			 "PERDB_IDLE_INTERVAL = %d", dt);};

/* maximum number of consecutive 0 length reads
 * indicating dropped connection
 */
	nb    = 0;
	nbmax = 10;

	ng = 0;
	for (ok = TRUE; (ok == TRUE) && (ng < tmax) && (nb < nbmax); )
	    {check_fd(srv, flog);

/* timeout the select every DT seconds */
	     t.tv_sec  = dt;
	     t.tv_usec = 0;

	     rfs = srv->afs;
	     nr  = select(FD_SETSIZE, &rfs, NULL, NULL, &t);
	     if (nr > 0)
	        {for (fd = 0; (fd < FD_SETSIZE) && (ok == TRUE); ++fd)
		     {if (FD_ISSET(fd, &rfs))
			 {log_activity(flog, dbs.debug, 4, "SERVER",
				       "data available on %d (server %d)",
				       fd, srv->server);

/* new connection request */
			  if (fd == srv->server)
			     new_connection(root, srv);

/* data arriving on an existing connection */
			  else
			     {cl->fd = fd;

			      ok = proc_connection(cl);
                              if (ok == -1)
				 {ok = TRUE;
				  remove_fd(srv, flog, fd);
				  nb++;}
			      else
				 nb = 0;

			      sched_yield();};};};
		  ng = 0;}
	     else
	        ng += dt;};

	if (ok != TRUE)
	   log_activity(flog, dbs.debug, 4, "SERVER", "done by command");
	else if (ng >= tmax)
	   log_activity(flog, dbs.debug, 4, "SERVER",
			"done by time: %d >= %d", ng, tmax);
	else if (nb >= nbmax)
	   log_activity(flog, dbs.debug, 4, "SERVER",
			"done by failed reads: %d >= %d", nb, nbmax);}

    else
       log_activity(flog, dbs.debug, 1, "SERVER",
		    "async_server error (%s - %d)",
		    strerror(errno), errno);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SERVER - run the server side of the database */

static int server(char *root, int init, int dmn)
   {char *flog;
    client *cl;

    if ((dmn == FALSE) || (demonize() == TRUE))
       {signal(SIGTERM, sigdone);
	signal(SIGINT, sigdone);
	signal(SIGHUP, sigrestart);

	flog = name_log(root);

	log_activity(flog, dbs.debug, 1, "SERVER", "start server");

	cl = make_client(root, SERVER);

	if (db_srv_open(cl, root, init) == TRUE)
	   {async_server(cl);

	    db_srv_save(-1, cl->db);
	    db_srv_close(cl->db);};

	free_client(cl);

	log_activity(flog, dbs.debug, 1, "SERVER", "end server");};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXCHANGE - do a transaction from the client side of the database
 *          - if LTR is TRUE strip off the quotes the server side added
 */

static int exchange(char *root, int ltr, char *req)
   {int i, n, rv;
    char **ta;
    client *cl;

    cl = make_client(root, CLIENT);
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
    char root[MAXLINE], r[MAXLINE], req[MAXLINE];
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
                      dbs.auth = TRUE;
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
                      nstrncpy(r, MAXLINE, v[++i], -1);
		      break;
		 case 'h' :
                      help();
                      return(1);
		 case 'l' :
                      dbg_sock = TRUE;
                      dbs.debug   = TRUE;
                      break;
		 case 's' :
                      srv = TRUE;
                      break;};}
	 else
	    break;};

/* everything from here is the request */
    req[0] = '\0';
    for (; i < c; i++)
        {nstrcat(req, MAXLINE, v[i]);
	 nstrcat(req, MAXLINE, " ");};

    if (IS_NULL(r) == TRUE)
       {if (cdefenv("PERDB_PATH") == TRUE)
           nstrncpy(r, MAXLINE, cgetenv(TRUE, "PERDB_PATH"), -1);};

    if (IS_NULL(r) == TRUE)
       snprintf(r, MAXLINE, "%s/.perdb", cgetenv(TRUE, "HOME"));

    full_path(root, MAXLINE, NULL, r);

    rv = cclearenv(except);

    rv = TRUE;
    if (srv == TRUE)
       rv = server(root, init, dmn);

    else if (IS_NULL(req) == FALSE)
      {LAST_CHAR(req) = '\0';
       rv = exchange(root, ltr, req);};

    rv = (rv == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
