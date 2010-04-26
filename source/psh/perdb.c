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

#include <setjmp.h>

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SIGDONE - handle signals meant to end session */

static void sigdone(int sig)
   {

    if (db != NULL)
       {log_activity(db->flog, dbg_db, "SERVER", "signalled %d - done", sig);
	ioc->close_comm(db->root);
	db_srv_save(db);
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

char *srv_save_db(char *fname, char *var)
   {int ok;
    char s[MAXLINE];
    FILE *fp;
    static char t[MAXLINE];

    if (fname == NULL)
       fp = NULL;
    else
       {snprintf(s, MAXLINE, "%s.%s", db->file, fname);
	fp = fopen(s, "w");
        if (fp == NULL)
	   {snprintf(t, MAXLINE, "could not open %s - save %s",
		     fname, strerror(errno));
	    return(t);};
        fname = path_tail(s);};
	   
    ok = save_db(db, var, fp);

    if (fname == NULL)
       {if (var == NULL)
	   snprintf(t, MAXLINE, "saved database");
        else
	   snprintf(t, MAXLINE, "saved %s", var);}

    else
       {fclose(fp);
	if (var == NULL)
	   snprintf(t, MAXLINE, "saved database to %s",
		    fname);
	else
	   snprintf(t, MAXLINE, "saved %s to %s",
		    var, fname);};

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SRV_LOAD_DB - load the database */

char *srv_load_db(char *fname, char *var)
   {char s[MAXLINE];
    FILE *fp;
    static char t[MAXLINE];

    if (fname == NULL)
       fp = NULL;
    else
       {snprintf(s, MAXLINE, "%s.%s", db->file, fname);
	fp = fopen(s, "r");
        if (fp == NULL)
	   {snprintf(t, MAXLINE, "could not open %s - load %s",
		     fname, strerror(errno));
	    return(t);};
        fname = path_tail(s);};
	   
    load_db(db, var, fp);

    if (fname == NULL)
       {if (var == NULL)
	   snprintf(t, MAXLINE, "loaded database");
        else
	   snprintf(t, MAXLINE, "loaded %s", var);}

    else
       {fclose(fp);
	if (var == NULL)
	   snprintf(t, MAXLINE, "loaded database from %s",
		    fname);
	else
	   snprintf(t, MAXLINE, "loaded %s from %s",
		    var, fname);};

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SERVER - run the server side of the database */

int server(char *root, int init, int dmn)
   {int ok, nb, to, st;
    char s[MAXLINE], t[MAXLINE];
    char *var, *val, *nvl, *p;

    if ((dmn == FALSE) || (demonize() == TRUE))
       {signal(SIGTERM, sigdone);
	signal(SIGINT, sigdone);
	signal(SIGHUP, sigrestart);

	log_activity(name_log(root), dbg_db, "SERVER", "begin");

	to = 60;

	db = db_srv_open(root, init);
	if (db != NULL)
	   {for (ok = TRUE; ok == TRUE; )
	        {ok = ioc->exists_comm(root);
		 if (ok == TRUE)
		    {memset(s, 0, MAXLINE);

		     nb = comm_read(root, SERVER, s, MAXLINE, to);
		     if (nb < 0)
		        {ok = FALSE;
			 continue;};

/* decide on the action to take */
		     if (nb > 0)

/* quit session */
		        {if (strncmp(s, "quit:", 5) == 0)
			    {val = "done";
			     ok  = FALSE;}

/* reset database */
		         else if (strncmp(s, "reset:", 6) == 0)
			    {reset_db(db);
			     val = "reset";}

/* save to standard place */
		         else if (strncmp(s, "save:", 5) == 0)
			    {key_val(NULL, &var, s, ": \t\n");
			     val = srv_save_db(NULL, var);}

/* save to specified place */
		         else if (strncmp(s, "save ", 5) == 0)
			    {p = strchr(s+5, ':');
			     *p++ = '\0';
			     key_val(NULL, &var, s+5, " \t\n");
			     val = srv_save_db(s+5, var);}

/* load from standard place */
		         else if (strncmp(s, "load:", 5) == 0)
			    val = srv_load_db(NULL, NULL);

/* load from specified place */
		         else if (strncmp(s, "load ", 5) == 0)
			    {p = strchr(s+5, ':');
			     *p++ = '\0';
			     val = srv_load_db(s+5, NULL);}

/* variable def/init */
			 else if (strchr(s, '?') != NULL)
			    {key_val(&var, &nvl, s, "? \t\n");
			     val = get_db(db, var);
			     st  = (val != cnoval());
			     if (nvl[0] == '\0')
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
			     if ((strchr("'\"(", val[0]) == NULL) && 
				 (strpbrk(val, " \t") != NULL))
			        {snprintf(t, MAXLINE, "\"%s\"", val);
				 val = t;};};}
/* error case */
		     else
		        val = "error";

/* howto use this in a shell: echo "bar" >> ~/foo.= && cat < ~/foo.= */
		     nb = comm_write(root, CLIENT, val, 0, 10);
		     nb = comm_write(root, CLIENT, EOM, 0, 10);};};

	    db_srv_save(db);
	    db_srv_close(db);};

	log_activity(name_log(root), dbg_db, "SERVER", "end");};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXCHANGE - do a transaction from the client side of the database */

int exchange(char *root, char *req)
   {int i, n, rv;
    char **ta;

    ta = _db_clnt_ex(root, req);

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
    printf("Usage: perdb [-c] [-d] [-f <db>] [-h] [-l] [-s] [-x fifo | socket] [<req>]\n");
    printf("   c   create the database, removing any existing\n");
    printf("   d   do not run server as daemon\n");
    printf("   f   root path to database\n");
    printf("   h   this help message\n");
    printf("   l   log transactions with the server\n");
    printf("   s   run as server\n");
    printf("   x   server communication mode\n");
    printf("\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i, rv, srv, dmn, init;
    char root[MAXLINE], r[MAXLINE], req[MAXLINE];

    req[0] = '\0';
    r[0]   = '\0';
    srv    = FALSE;
    init   = FALSE;
    dmn    = TRUE;

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
	    {switch (v[i][1])
                {case 'c' :
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
                      dbg_fifo = TRUE;
                      dbg_sock = TRUE;
                      dbg_db   = TRUE;
                      break;
		 case 's' :
                      srv = TRUE;
                      break;
		 case 'x' :
		      i++;
                      if (strcmp(v[i], "fifo") == 0)
                         ioc = &fifo;
		      else if (strcmp(v[i], "socket") == 0)
			 ioc = &sock;
                      break;};}
	 else
	    {nstrcat(req, MAXLINE, v[i]);
	     nstrcat(req, MAXLINE, " ");};};

    if (r[0] == '\0')
       {if (cdefenv("PERDB_PATH") == TRUE)
           nstrncpy(r, MAXLINE, cgetenv(TRUE, "PERDB_PATH"), -1);};

    if (r[0] == '\0')
       snprintf(r, MAXLINE, "%s/.perdb", cgetenv(TRUE, "HOME"));

    full_path(root, MAXLINE, NULL, r);

    rv = TRUE;
    if (srv == TRUE)
       rv = server(root, init, dmn);

    else if (req[0] != '\0')
      {LAST_CHAR(req) = '\0';
       rv = exchange(root, req);};

    rv = (rv == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
