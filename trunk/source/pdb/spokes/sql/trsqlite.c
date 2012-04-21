/*
 * TRSQLITE.C - SQLite support for SQL spoke in SX
 *
 * SQLite commands
 *
 *   .tables               list tables
 *   .schema $table        describe table
 *   select * from $table  print entire table
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "trsql.h"

#ifdef HAVE_SQLITE

#include <sqlite3.h>

#define GET_CONNECTION(_f)     ((sqlite3 *) GET_SQL_FILE(_f)->conn)

typedef struct s_sql_hdr sql_hdr;

struct s_sql_hdr
   {short page_size;    /* database page size in bytes */
    char writev;        /* write version */
    char readv;         /* read version */
    char unused;        /* number of bytes unused at end of page */
    char maxfri;        /* max fraction of an index tree page */
    char minfri;        /* min fraction of an index tree page */
    char minfrt;        /* min fraction of an table tree page */
    int cntr;           /* file change counter */
    int ftp;            /* number of first freelist trunk page */
    int nfp;            /* number of free pages */
    int schemav;        /* schema version */
    int schemaf;        /* schema layer file-format */
    int cachesz;        /* pager cache size */
    int avcd;           /* max page number for auto-vacuum capable databases */
    int txte;           /* text encoding
                         * 1  UTF-8 encoding
                         * 2 little-endian UTF-16
                         * 3 big-endian UTF-16
                         */
    int ucv;            /* user cookie value */
    int ivf;};          /* incremental-vacuum flag */


typedef struct s_indbt indbt;

struct s_indbt
   {char flag;           /* page flags */
    short ofree;         /* byte offset of first block of free space */
    short ncells;        /* numer of cells */
    short ocell;         /* byte offset of first byte of cell content area */
    char nfr;            /* number of fragmented free bytes */
    int ipg;};           /* number of rightmost child page */

#include "sqlite3.api"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQLITE_ERROR - handle SQLITE errors */

static void _SQLITE_error(int errn, char *msg)
   {

    printf("SQLITE %s failed - %d\n", msg, errn);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQLITE_DB_CLOSE - close the database conection */

static int _SQLITE_db_close(FILE *fp)
   {sqlite3 *conn;
    sql_file *sys;

    sys  = (sql_file *) fp;
    conn = (sqlite3 *) sys->conn;

    sqlite3_close(conn);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQLITE_SPLIT_ROW - split line S at newlines and add them to OUT */

static void _SQLITE_split_row(SC_array *out, char *s)
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

/* _SQLITE_GATHER_ROW - make lines out of the NF tokens of row IR */

static void _SQLITE_gather_row(SC_array *out, char **in,
			       int nf, int ir)
   {int i, l;
    char s[MAX_BFSZ];

    s[0] = '\0';
    for (i = 0; i < nf; i++)
        {l = (ir + 1)*nf + i;
	 if (in[l][0] == '\0')
	    strcat(s, "NULL");
         else
	    strcat(s, in[l]);
	 strcat(s, "|");};

    SC_LAST_CHAR(s) = '\0';

    _SQLITE_split_row(out, s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQLITE_OPER - do the specified SQL command */

int _SQLITE_oper(FILE *fp, char *sql)
   {int ok, rv;
    char *err;
    sqlite3 *conn;

    conn = GET_CONNECTION(fp);

    ok = sqlite3_exec(conn, sql, NULL, NULL, &err);
    if (ok != SQLITE_OK)
       {rv = FALSE;
	_SQLITE_error(ok, err);}
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQLITE_QUERY - do the specified SQL command */

sql_table *_SQLITE_query(FILE *fp, char *sql, char *delim, int add)
   {int ir, nf, nr, ok;
    char **res, *err;
    SC_array *arr;
    sql_table *tab;
    sqlite3 *conn;

    tab  = NULL;
    conn = GET_CONNECTION(fp);

    ok = sqlite3_get_table(conn, sql, &res, &nr, &nf, &err);
    if (ok != SQLITE_OK)
       _SQLITE_error(ok, err);

    else
       {tab = _SQL_mk_table(nr, nf);

	arr = CMAKE_ARRAY(char *, NULL, 0);

/* gather results */
	for (ir = 0; ir < nr; ir++)
	    _SQLITE_gather_row(arr, res, nf, ir);

	sqlite3_free_table(res);

	SC_array_string_add(arr, NULL);

	CFREE(tab->table);
	tab->table = SC_array_done(arr);};

    return(tab);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQLITE_TABLES - list the tables for SQLITE */

static sql_table *_SQLITE_tables(FILE *fp)
   {sql_table *tab;

    tab = _SQLITE_query(fp, "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;",
		       NULL, 0);

    return(tab);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQLITE_DESC - describe a type for SQLITE */

static sql_table *_SQLITE_desc(FILE *fp, char *s)
   {char cmd[MAXLINE];
    sql_table *tab;

    snprintf(cmd, MAXLINE, "SELECT sql FROM sqlite_master where name='%s'", s);

    tab = _SQLITE_query(fp, cmd, NULL, 0);

    return(tab);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQLITE_NI - return the number of rows in table S */

static int _SQLITE_ni(FILE *fp, char *s)
   {int nr;
    char cmd[MAXLINE];
    sql_table *tab;

    snprintf(cmd, MAXLINE, "select count(*) from %s", s);

    tab = _SQLITE_query(fp, cmd, NULL, 0);
    nr  = (tab != NULL) ? SC_stoi(tab->table[0]) : 0;

    _SQL_rl_table(tab);

    return(nr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQLITE_SELECT - do a select for SQLITE */

static sql_table *_SQLITE_select(FILE *fp, char *s)
   {char cmd[MAXLINE];
    sql_table *tab;

    snprintf(cmd, MAXLINE, "select * from %s", s);

    tab = _SQLITE_query(fp, cmd, NULL, 0);

    return(tab);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static sql_cmd
 _SQLITE_cmd = { _SQLITE_tables, _SQLITE_desc,
		 _SQLITE_ni, _SQLITE_select,
                 _SQLITE_oper };

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQLITE_FILE - return a file_io_desc cast as a FILE for
 *              - use by the IO_ functions in the case of an
 *              - SQLite connection file
 */

FILE *_SQLITE_file(sql_file *fs)
   {file_io_desc *fid;
    FILE *fp;

    fid         = SC_make_io_desc(fs);
    fid->fclose = _SQLITE_db_close;

    fp = (FILE *) fid;

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQLITE_OPEN - open an SQLITE database */

FILE *_SQLITE_open(PDBfile *file, char *name, char *mode)
   {int ok;
    char *path;
    FILE *fp;
    SC_udl *pu;
    sqlite3 *conn;
    sql_file *fs;

    _SQLITE3_set_api();

    fp = NULL;
    pu = file->udl;

    path = SC_search_file(NULL, pu->path);
    SC_ASSERT(path != NULL);

/*#ifdef HAVE_SQLITE_V2*/
#if 0
    {int sm;

     sm = 0;
     switch (*mode)
        {case 'a' :
              if (path == NULL)
	         sm = SQLITE_OPEN_READWRITE;
	      else
	         sm = SQLITE_OPEN_CREATE;
              break;
         case 'w' :
              sm = SQLITE_OPEN_CREATE;
              break;
         case 'r' :
              if (strncmp(mode, "r+", 2) == 0)
	         sm = SQLITE_OPEN_READWRITE;
	      else
	         sm = SQLITE_OPEN_READONLY;
              break;};

     sm |= SQLITE_OPEN_NOMUTEX;

     ok = sqlite3_open_v2(pu->path, &conn, sm, NULL);};
#else
    ok = sqlite3_open(pu->path, &conn);
#endif

    if ((ok != SQLITE_OK) || (conn == NULL))
       _SQLITE_error(ok, "connection");

    else
       {fs       = CMAKE(sql_file);
	fs->conn = (FILE *) conn;
	fs->sys  = &_SQLITE_cmd;

	fp = _SQLITE_file(fs);

	if (fp != NULL)
	   {file->stream = fp;
	    _SQL_sym_chart(file);};};

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

