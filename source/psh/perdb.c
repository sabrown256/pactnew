/*
 * PERDB.C - implement persistent database access
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

/* #define NEWWAY */

#include "common.h"
#include "libpsh.c"
#include "libdb.c"

#include <setjmp.h>

static database
 *db = NULL;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SIGDONE - handle signals meant to end session */

static void sigdone(int sig)
   {

    if (db != NULL)
       {log_activity(db->flog, dbg_db, "SERVER", "signalled %d - done", sig);
	close_sock(db->root);
	db_srv_save(-1, db);
	unlink(db->fpid);};

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

char *srv_save_db(client *cl, char *fname, char *var)
   {int fd, ok;
    char s[MAXLINE];
    FILE *fp;
    database *db;
    static char t[MAXLINE];

    fd = cl->fd;
    db = cl->db;

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
	   
    ok = save_db(fd, db, var, fp);
    ASSERT(ok == 0);

    if (fp != NULL)
       fclose(fp);
	
    if (fname == NULL)
       {if (var == NULL)
	   snprintf(t, MAXLINE, "saved database");
        else
	   snprintf(t, MAXLINE, "saved %s", var);}

    else
       {if (var == NULL)
	   snprintf(t, MAXLINE, "saved database to %s",
		    fname);
	else
	   snprintf(t, MAXLINE, "saved %s to %s",
		    var, fname);};

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SRV_LOAD_DB - load the database */

char *srv_load_db(client *cl, char *fname, char *var)
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

void check_fd(connection *srv, char *flog)
   {int fd;
    char s[MAXLINE];
    fd_set rfs;

    rfs = srv->afs;

    s[0] = '\0';
    for (fd = 0; fd < FD_SETSIZE; ++fd)
        {if (FD_ISSET(fd, &rfs))
            vstrcat(s, MAXLINE, " %d", fd);};

    log_activity(flog, dbg_db, "SERVER", "monitoring: %s", s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ADD_FD - add FD to the set in SRV */

void add_fd(connection *srv, char *flog, int fd)
   {

    FD_SET(fd, &srv->afs);
    log_activity(flog, dbg_db, "SERVER", "add %d", fd);

    check_fd(srv, flog);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REMOVE_FD - add FD to the set in SRV */

void remove_fd(connection *srv, char *flog, int fd)
   {

    FD_CLR(fd, &srv->afs);
    log_activity(flog, dbg_db, "SERVER", "remove %d", fd);

    check_fd(srv, flog);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NEW_CONNECTION - make a new connection on SFD and add it to AFS */

void new_connection(char *root, connection *srv)
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
	log_activity(flog, dbg_db, "SERVER",
		     "accept error - %s (%d)",
		     strerror(errno), errno);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TERM_CONNECTION - finish off client connection CL */

void term_connection(client *cl)
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

/* PROC_CONNECTION - process input on client connection CL
 *                 - return -1 on error
 *                 -         0 on end of connection
 *                 -         1 if otherwise successful
 */

int proc_connection(client *cl)
   {int rv, nb, st, to;
    char s[MAXLINE], t[MAXLINE];
    char *var, *val, *nvl, *p;
    database *db;

    db = cl->db;

    to = 60;
    rv = 1;

    memset(s, 0, MAXLINE);

    nb = comm_read(cl, s, MAXLINE, to);

/* decide on the action to take */
    if (nb > 0)

/* quit session */
       {if (strncmp(s, "quit:", 5) == 0)
	   {rv  = 0;
	    val = "done";}

/* client is exiting */
	else if (strncmp(s, "fin:", 4) == 0)
	   {term_connection(cl);
	    val = NULL;}

/* reset database */
        else if (strncmp(s, "reset:", 6) == 0)
	   {reset_db(db);
	    val = "reset";}

/* save to standard place */
	else if (strncmp(s, "save:", 5) == 0)
	   {key_val(NULL, &var, s, ": \t\n");
	    val = srv_save_db(cl, NULL, var);}

/* save to specified place */
	else if (strncmp(s, "save ", 5) == 0)
	   {p = strchr(s+5, ':');
	    if (p != NULL)
	       *p++ = '\0';
	    key_val(NULL, &var, s+5, " \t\n");
	    val = srv_save_db(cl, s+5, var);}

/* load from standard place */
	else if (strncmp(s, "load:", 5) == 0)
	   val = srv_load_db(cl, NULL, NULL);

/* load from specified place */
	else if (strncmp(s, "load ", 5) == 0)
	   {p = strchr(s+5, ':');
	    if (p != NULL)
	       *p++ = '\0';
	    val = srv_load_db(cl, s+5, NULL);}

/* variable conditional init */
	else if (strstr(s, "=\?") != NULL)
	   {key_val(&var, &nvl, s, "=? \t\n");
	    val = get_db(db, var);
	    st  = (val != cnoval());
	    if (st == FALSE)
	       val = put_db(db, var, nvl);}

/* variable defined? */
	else if (strchr(s, '?') != NULL)
	   {key_val(&var, &nvl, s, "? \t\n");
	    val = get_db(db, var);
	    st  = (val != cnoval());
	    if (IS_NULL(nvl) == TRUE)
	       {if (st == TRUE)
		   val = "defined{TRUE}";
	        else
		   val = "defined{FALSE}";}
	    else if (st == FALSE)
	       val = put_db(db, var, nvl);}

/* variable set/get */
	else
	   {key_val(&var, &val, s, "= \t\n");
	    if (val == NULL)
	       {val = get_db(db, var);}
	    else
	       {val = trim(val, BACK, " \t");
		val = put_db(db, var, val);};

	    if ((val != NULL) &&
		(strchr("'\"(", val[0]) == NULL) && 
		(strpbrk(val, " \t") != NULL))
	       {snprintf(t, MAXLINE, "\"%s\"", val);
		val = t;};};

	if (val != NULL)
	   {nb = comm_write(cl, val, 0, 10);
	    nb = comm_write(cl, EOM, 0, 10);};

#ifndef NEWWAY
	if (cl->async == TRUE)
	   cl->fd = connect_close(cl->fd, cl, NULL);
#endif
        };

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
       {for (ok = TRUE; ok == TRUE; )
	    {check_fd(srv, flog);

	     rfs = srv->afs;
	     nr  = select(FD_SETSIZE, &rfs, NULL, NULL, NULL);
	     if (nr > 0)
	        {for (fd = 0; (fd < FD_SETSIZE) && (ok == TRUE); ++fd)
		     {if (FD_ISSET(fd, &rfs))
			 {log_activity(flog, dbg_db, "SERVER",
				       "data available on %d (server %d)",
				       fd, srv->server);

/* new connection request */
			  if (fd == srv->server)
			     new_connection(root, srv);

/* data arriving on an existing connection */
			  else
			     {cl->fd = fd;
			      ok     = proc_connection(cl);
			      sched_yield();};};};};};}
    else
       log_activity(flog, dbg_db, "SERVER",
		    "async_server error (%s - %d)",
		    strerror(errno), errno);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SYNC_SERVER - run a synchronous server in which
 *             - transactions come in one at a time sequentially
 */

static void sync_server(client *cl)
   {int ok;
    char *root;

    root = cl->root;

    for (ok = TRUE; ok == TRUE; )
        {ok = sock_exists(root);
	 if (ok == TRUE)
	    {cl->fd = -1;
	     ok     = proc_connection(cl);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SERVER - run the server side of the database */

int server(char *root, int init, int dmn)
   {char *flog;
    client *cl;

    if ((dmn == FALSE) || (demonize() == TRUE))
       {signal(SIGTERM, sigdone);
	signal(SIGINT, sigdone);
	signal(SIGHUP, sigrestart);

	flog = name_log(root);

	log_activity(flog, dbg_db, "SERVER", "begin");

	cl = make_client(root, async_srv, SERVER);

	cl->db = db_srv_open(root, init);
	if (cl->db != NULL)
	   {db = cl->db;

	    if (async_srv == TRUE)
	       async_server(cl);

	    else
	       sync_server(cl);

	    db_srv_save(-1, cl->db);
	    db_srv_close(cl->db);

	    log_activity(flog, dbg_db, "SERVER", "end");};

	free_client(cl);};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXCHANGE - do a transaction from the client side of the database */

int exchange(char *root, int async, char *req)
   {int i, n, rv;
    char **ta;
    client *cl;

    cl = make_client(root, async, CLIENT);
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
	 else
	    printf("%s\n", ta[i]);};

    lst_free(ta);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - print usage info */

void help(void)
   {
    printf("\n");
    printf("Usage: perdb [-as] [-c] [-d] [-f <db>] [-h] [-l] [-s] [<req>]\n");
    printf("   as  run the database asynchronously\n");
    printf("   c   create the database, removing any existing\n");
    printf("   d   do not run server as daemon\n");
    printf("   f   root path to database\n");
    printf("   h   this help message\n");
    printf("   l   log transactions with the server\n");
    printf("   s   run as server\n");
    printf("\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i, rv, srv, dmn, init;
    char root[MAXLINE], r[MAXLINE], req[MAXLINE];
    char *except[] = {"PATH", NULL};

    req[0] = '\0';
    r[0]   = '\0';
    srv    = FALSE;
    init   = FALSE;
    dmn    = TRUE;

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
	    {switch (v[i][1])
                {case 'a' :
		      if (v[i][2] == 's')
			 async_srv = TRUE;
		      break;
                 case 'c' :
                      init = TRUE;
		      break;
                 case 'd' :
                      dmn = FALSE;
		      break;
                 case 'f' :
                      nstrncpy(r, MAXLINE, v[++i], -1);
		      break;
		 case 'h' :
                      help();
                      return(1);
		 case 'l' :
                      dbg_sock = TRUE;
                      dbg_db   = TRUE;
                      break;
		 case 's' :
                      srv = TRUE;
                      break;};}
	 else
	    {nstrcat(req, MAXLINE, v[i]);
	     nstrcat(req, MAXLINE, " ");};};

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
       rv = exchange(root, async_srv, req);};

    rv = (rv == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
