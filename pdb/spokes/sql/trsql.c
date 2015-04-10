/*
 * TRSQL.C - command processors for SQL spoke in SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "trsql.h"

#ifdef HAVE_SQL

char
 *SQL_DATABASE_S = "SQLdatabase";

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQL_DATABASEP - return TRUE iff TYPE is SQL database */

static int _SQL_databasep(char *type)
   {int rv;

    rv = (strcmp(type, SQL_DATABASE_S) == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQL_CLOSE - close an SQL file */

static int _SQL_close(PDBfile *file)
   {FILE *fp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    switch (SETJMP(pa->close_err))
       {case ABORT :
	     return(FALSE);
        case ERR_FREE :
	     return(TRUE);
        default :
	     memset(pa->err, 0, MAXLINE);
	     break;};

    fp = file->stream;

#ifdef HAVE_MYSQL
    if ((fp != NULL) && (strstr(file->name, "mysql://") != NULL))
       {lio_close(fp);
	fp = NULL;};
#endif

#ifdef HAVE_SQLITE
    if (fp != NULL)
       {lio_close(fp);
	fp = NULL;};
#endif

    if (fp == NULL)
       {file->stream      = NULL;
	file->udl->stream = NULL;};

/* free the space */
    _PD_rl_pdb(file);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQL_CREATE - create an existing SQL File */

static PDBfile *_SQL_create(tr_layer *tr, SC_udl *pu, char *name, void *a)
   {char *lname;
    PDBfile *file;
    FILE *fp;

    lname = pu->udl;

    if (pu->server == NULL)
       {fp = pu->stream;
	_PD_set_io_buffer(pu);};

    file = _PD_mk_pdb(pu, NULL, NULL, TRUE, NULL, tr);
    if (file == NULL)
       PD_error("CANNOT ALLOCATE SQL - SQL_CREATE", PD_CREATE);

    file->mode = PD_CREATE;
    file->default_offset = 0;
    file->major_order    = ROW_MAJOR_ORDER;
    file->type           = CSTRSAVE(SQL_DATABASE_S);

    _PD_set_standard(file, &TEXT_STD, &TEXT_ALIGNMENT);

    _PD_init_chrt(file, TRUE);

    fp = NULL;

#ifdef HAVE_MYSQL
    if (fp == NULL)
       fp = _MYSQL_open(file, lname, "w");
#endif

#ifdef HAVE_SQLITE
    if (fp == NULL)
       fp = _SQLITE_open(file, lname, "w");
#endif

    if (fp == NULL)
       {file->udl = NULL;
	_PD_rl_pdb(file);
	file = NULL;}
    else
       pu->stream = fp;

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQL_OPEN - open an existing SQL File */

static PDBfile *_SQL_open(tr_layer *tr, SC_udl *pu, char *name, char *mode)
   {unsigned char str[MAXLINE];
    char id[MAXLINE];
    char *lname;
    PDBfile *file;
    FILE *fp;

    lname = pu->udl;

    if (pu->server == NULL)
       {fp = pu->stream;
	_PD_set_io_buffer(pu);

/* attempt to read an ASCII header */
	if (lio_seek(fp, 0L, SEEK_SET))
	   PD_error("FSEEK FAILED TO FIND ORIGIN - _SQL_OPEN", PD_OPEN);

	if (lio_read(str, 1, 16, fp) != 16)
	   return(NULL);

/* grab the ASCII id */
	strncpy(id, (char *) str, 16);
	id[16] = '\0';
	if (strcmp(id, "SQLite format 3") != 0)
	   return(NULL);};

    file = _PD_mk_pdb(pu, NULL, mode, TRUE, NULL, tr);
    if (file == NULL)
       PD_error("CAN'T ALLOCATE SQL - SQL_OPEN", PD_OPEN);

    file->default_offset = 0;
    file->major_order    = ROW_MAJOR_ORDER;
    file->type           = CSTRSAVE(SQL_DATABASE_S);
    if (*mode == 'a')
       file->mode = PD_APPEND;
    else
       file->mode = PD_OPEN;

    _PD_set_standard(file, &TEXT_STD, &TEXT_ALIGNMENT);

    _PD_init_chrt(file, TRUE);

    fp = NULL;

#ifdef HAVE_MYSQL
    if (fp == NULL)
       fp = _MYSQL_open(file, lname, mode);
#endif

#ifdef HAVE_SQLITE
    if (fp == NULL)
       fp = _SQLITE_open(file, lname, mode);
#endif

    if (fp == NULL)
       {file->udl = NULL;
	_PD_rl_pdb(file);
	file = NULL;}
    else
       pu->stream = fp;

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQL_READ_ENTRY - read entry method for SQL spoke */

int _SQL_read_entry(PDBfile *fp, char *path, char *ty, syment *ep, void *vr)
   {int rv;
    long bs, be, bd, doff, ni;
    char s[MAXLINE];
    char *name, *tok, *p;
    dimind dim;

    rv = 0;

    doff = PD_get_offset(fp);

    SC_strncpy(s, MAXLINE, path, -1);
    name = SC_strtok(s, "()[]", p);
    if (name != NULL)
       {bs = 1L - doff;
	be = PD_entry_number(ep) - doff;
	bd = 1L;

	tok = SC_strtok(NULL, ":]", p);
	if (tok != NULL)
	   {bs  = SC_stoi(tok) - doff;
	    tok = SC_strtok(NULL, ":]", p);
	    if (tok == NULL)
	       be = bs;
	    else
	       be = SC_stoi(tok) - doff;

	    bd = SC_stoi(SC_strtok(NULL, ":]", p));
	    if (bd == 0L)
	       bd = 1L;};

	dim.stride = 0;
	dim.start  = bs;
	dim.stop   = be;
	dim.step   = bd;

	ni = (dim.stop - dim.start)/dim.step + 1;

	rv = _SQL_read(fp, name, ni, &dim, vr);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SQL_WRITE_ENTRY - write entry method for SQL spoke */

syment *_SQL_write_entry(PDBfile *fp, char *path, char *inty, char *outty,
			 void *vr, dimdes *dims)
   {long ni;
    syment *ep;

    ni = _PD_comp_num(dims);

    ep = _SQL_write(fp, inty, ni, vr);

    return(ep);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PD_REGISTER_SQL - install the SQL extensions to PDBLib */
 
int PD_register_sql(void)
   {static int nmysql = -1, nsqlite = -1;

    ONCE_SAFE(TRUE, NULL)

/* since this is a spoke the application has to register
 * make sure all the others are registered first
 */
       _PD_register_spokes();

       nmysql = PD_REGISTER(SQL_DATABASE_S, "mysql", _SQL_databasep,
			    _SQL_create, _SQL_open, _SQL_close,
			    _SQL_write_entry, _SQL_read_entry);
       SC_ASSERT(nmysql >= 0);

       nsqlite = PD_REGISTER(SQL_DATABASE_S, "sqlite", _SQL_databasep,
			     _SQL_create, _SQL_open, _SQL_close,
			     _SQL_write_entry, _SQL_read_entry);

       SC_ASSERT(nsqlite >= 0);

    END_SAFE;

    return(nsqlite);}
 
/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/
 
/* PD_REGISTER_SQL - install the SQL extensions to PDBLib */
 
int PD_register_sql(void)
   {

    _PD_register_spokes();

    return(-1);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

