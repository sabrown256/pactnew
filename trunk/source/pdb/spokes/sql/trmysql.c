/*
 * TRMYSQL.C - Mysql support for SQL spoke in SX
 *
 * mySQL commands
 *
 *   show tables;
 *   describe $table;
 *   select * from $table;
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "trsql.h"

#ifdef HAVE_MYSQL

#include <mysql.h>

#define GET_CONNECTION(_f) ((MYSQL *) GET_SQL_FILE(_f)->conn)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_ERROR - handle MYSQL errors */

static void _MYSQL_error(MYSQL *conn, char *msg)
   {

    printf("MYSQL %s: %s failed\n", msg, mysql_error(conn));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_DB_CLOSE - close the database conection */

static int _MYSQL_db_close(FILE *fp)
   {MYSQL *conn;
    sql_file *sys;

    sys  = (sql_file *) fp;
    conn = (MYSQL *) sys->conn;

    mysql_close(conn);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_SPLIT_ROW - split line S at newlines and add them to OUT */

static void _MYSQL_split_row(SC_array *out, char *s)
   {char *p, *u, *l;

    for (p = s; TRUE; p = NULL)
        {l = SC_strtok(p, "\n", u);
	 if (l != NULL)
	    SC_array_string_add_copy(out, l);
	 else
	    break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_GATHER_ROW - make lines out of the tokens of row IN */

static void _MYSQL_gather_row(SC_array *out, MYSQL_ROW in,
			      int nf, char *delim)
   {int i;
    char s[MAX_BFSZ];

    s[0] = '\0';
    for (i = 0; i < nf; i++)
        {if (in[i][0] == '\0')
	    break;
         strcat(s, in[i]);
	 strcat(s, delim);};

    SC_LAST_CHAR(s) = '\0';

    _MYSQL_split_row(out, s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_OPER - do the specified SQL operation */

int _MYSQL_oper(FILE *fp, char *sql)
   {int rv;
    MYSQL *conn;

    conn = GET_CONNECTION(fp);

    if (mysql_query(conn, sql) != 0)
       {_MYSQL_error(conn, "operation");
        printf("-> %s\n", sql);
        rv = FALSE;}

    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_QUERY - do the specified query */

sql_table *_MYSQL_query(FILE *fp, char *sql, char *delim, int add)
   {int nf, nr;
    SC_array *arr;
    MYSQL_RES *res;
    MYSQL_ROW row;
    sql_table *tab;
    MYSQL *conn;

    tab  = NULL;
    conn = GET_CONNECTION(fp);

    if (mysql_query(conn, sql) != 0)
       {_MYSQL_error(conn, "query");
        printf("-> %s\n", sql);}

    else
       {res = mysql_store_result(conn);
	if (res == NULL)
	   _MYSQL_error(conn, "store");

	else
	   {nr = mysql_num_rows(res);
	    nf = mysql_num_fields(res);

	    tab = _SQL_mk_table(nr, nf);

	    arr = SC_MAKE_ARRAY("_MYSQL_QUERY", char *, NULL);

	    if (add == TRUE)
	       SC_array_string_add_copy(arr, sql);

/* gather results */
	    while ((row = mysql_fetch_row(res)) != NULL)
	       {nf = mysql_num_fields(res);
		_MYSQL_gather_row(arr, row, nf, delim);};

	    mysql_free_result(res);

	    SC_array_string_add(arr, NULL);

	    SFREE(tab->table);
	    tab->table = SC_array_done(arr);};};

    return(tab);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_TABLES - list the tables for MYSQL */

static sql_table *_MYSQL_tables(FILE *fp)
   {sql_table *tab;

    tab = _MYSQL_query(fp, "show tables;", " ", FALSE);

    return(tab);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_DESC - describe a type for MYSQL */

static sql_table *_MYSQL_desc(FILE *fp, char *s)
   {char cmd[MAXLINE];
    sql_table *tab;

    snprintf(cmd, MAXLINE, "describe %s;", s);

    tab = _MYSQL_query(fp, cmd, " ", TRUE);

    return(tab);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_NITEMS - return the number of rows in table S */

static int _MYSQL_nitems(FILE *fp, char *s)
   {int nr;
    char cmd[MAXLINE];
    sql_table *tab;

    snprintf(cmd, MAXLINE, "select count(*) from %s", s);

    tab = _MYSQL_query(fp, cmd, " ", FALSE);

    nr = SC_stoi(tab->table[0]);

    _SQL_rl_table(tab);

    return(nr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_SELECT - do a select for MYSQL */

static sql_table *_MYSQL_select(FILE *fp, char *s)
   {char cmd[MAXLINE];
    sql_table *tab;

    snprintf(cmd, MAXLINE, "select * from %s;", s);

    tab = _MYSQL_query(fp, cmd, "|", FALSE);

    return(tab);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

sql_cmd
 _MYSQL_cmd = { _MYSQL_tables, _MYSQL_desc,
		_MYSQL_nitems, _MYSQL_select,
                _MYSQL_oper };

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_FILE - return a file_io_desc cast as a FILE for
 *             - use by the IO_ functions in the case of an
 *             - MYSQL connection file
 */

FILE *_MYSQL_file(sql_file *fs)
   {file_io_desc *fid;
    FILE *fp;

    fid          = FMAKE(file_io_desc, "_MYSQL_file:fid");
    fid->info    = fs;
    fid->fopen   = NULL;

    fid->ftell   = NULL;
    fid->lftell  = NULL;
    fid->fseek   = NULL;
    fid->lfseek  = NULL;
    fid->fread   = NULL;
    fid->lfread  = NULL;
    fid->fwrite  = NULL;
    fid->lfwrite = NULL;

    fid->setvbuf = NULL;
    fid->fclose  = _MYSQL_db_close;
    fid->fprintf = NULL;
    fid->fputs   = NULL;
    fid->fgetc   = NULL;
    fid->ungetc  = NULL;
    fid->fflush  = NULL;
    fid->feof    = NULL;
    fid->fgets   = NULL;
    fid->pointer = NULL;
    fid->segsize = NULL;

    fp = (FILE *) fid;

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _MYSQL_OPEN - open an Mysql database */

FILE *_MYSQL_open(PDBfile *file, char *name, char *mode)
   {int st;
    char s[MAXLINE];
    char *hst, *usr, *pwd, *db, *p;
    FILE *fp;
    pcons *pc;
    SC_udl *pu;
    MYSQL *conn;
    sql_file *fs;

    fp = NULL;
    pu = file->udl;

    if ((pu->protocol != NULL) && (strcmp(pu->protocol, "mysql") == 0))
       {hst = pu->server;
	db  = pu->path;
        p   = strchr(db, '/');
	if (p == NULL)
	   usr = pu->path;
	else
	   {*p++ = '\0';
	    usr = p;};

	if ((hst == NULL) || (hst[0] == '\0'))
	   hst = "localhost";

	pc = SC_assoc_entry(pu->info, "pwd");

/* if no pwd attribute given assume none required */
	if (pc == NULL)
	   pwd = NULL;

/* if a pwd attribute has a value use it */
	else if (pc->cdr != NULL)
	   pwd = pc->cdr;

/* otherwise prompt for pwd */
	else
           {printf("password: ");
	    fflush(stdout);
	    SC_echo_off_file(stdin);
	    pwd = fgets(s, MAXLINE, stdin);
	    SC_LAST_CHAR(s) = '\0';
	    SC_echo_on_file(stdin);};

	conn = mysql_init(NULL);
	if (mysql_real_connect(conn, hst, usr, pwd, db, 0, NULL, 0) == NULL)
	   _MYSQL_error(conn, "connect");

        else
	   {st = mysql_query(conn, "SET NAMES 'utf8'");
	    if (st != 0)
	       _MYSQL_error(conn, "set names");

	    fs       = FMAKE(sql_file, "_MYSQL_DB_OPEN:fs");
	    fs->conn = (FILE *) conn;
	    fs->sys  = &_MYSQL_cmd;

	    fp = _MYSQL_file(fs);

	    if (fp != NULL)
	       {file->stream = fp;
		 _SQL_sym_chart(file);};};};

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

