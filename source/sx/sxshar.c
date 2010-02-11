/*
 * SXSHAR.C - routines for all of C coded spoke handlers
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TR_OPEN_BIN - open up the specified UDL */

object *TR_open_bin(object *arg)
   {char *name, *mode, *path, *scope;
    char *lst[2];
    PDBfile *file;
    SC_udl *pu;
    object *obj;
    g_file *po;

    if (!_PD_IS_SMP_INIT)
       PD_init_threads(0, NULL);

    name  = NULL;
    mode  = NULL;
    scope = NULL;
    SS_args(arg,
            SC_STRING_I, &name,
            SC_STRING_I, &mode,
            SC_STRING_I, &scope,
            0);

    if (name == NULL)
       SS_error("BAD FILE NAME - TR_OPEN_BIN", arg);

    pu = _SC_parse_udl(name);

/* check for UDL */
    if (pu->server != NULL)
       path = name;

/* handle local file */
    else
       {if ((scope == NULL) || (strcmp(scope, "local") == 0))
	   {lst[0] = ".";
	    lst[1] = NULL;
	    path   = SC_search_file(lst, pu->path);}
        else
	   path = SC_search_file(NULL, pu->path);

	if (path == NULL)
	   {if (strcmp(mode, "r") == 0)
	       SS_error("CAN'T FIND FILE - TR_OPEN_BIN", arg);
	    path = name;};

	if (mode == NULL)
	   mode = "r";

/* search for an existing file by this name */
	for (po = SX_file_list; po != NULL; po = po->next)
	    {if (strcmp(path, po->name) == 0)
	        {obj = po->file_object;
		 return(obj);};};};

    path = SC_strsavef(name, "char*:TR_OPEN_BIN:path");

    obj = SS_null;

/* open the file */
    file = PD_open(path, mode);
    if ((file == NULL) && (strcmp(mode, "r") != 0))
       file = PD_open(path, "r");
    if (file != NULL)
       {obj = SX_mk_gfile(_SX_mk_file(path, PDBFILE_S, file));
	SS_UNCOLLECT(obj);

	_PD_def_real(PDBFILE_S, file);

	po              = SS_GET(g_file, obj);
	po->file_object = obj;

/* add to file_list */
	SX_file_list = po;};

    SFREE(path);

    if (obj == SS_null)
       SS_error("FILE OF UNKNOWN TYPE OR ACCESS DENIED - TR_OPEN_BIN", arg);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TR_CLOSE_BIN - close the named binary file */

object *TR_close_bin(object *arg)
   {PFBinClose fun;
    PDBfile *file;
    tr_layer *tr;
    g_file *po;
    object *o;

    po = NULL;
    SS_args(arg,
            G_FILE, &po,
            0);
    
    o = SS_f;
    if (po != NULL)
       {file = FILE_FILE(PDBfile, po);
	tr   = file->tr;
	fun  = tr->close;
	if (fun(file) == TRUE)
	   o = SS_t;};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TR_WRITE_BIN - write an entry to the specified binary file */

object *TR_write_bin(object *arg)
   {PFBinWrite fun;
    PDBfile *file;
    tr_layer *tr;
    g_file *po;
    object *o;

    po = NULL;
    SS_args(arg,
            G_FILE, &po,
            0);
    
    o = SS_f;
    if (po != NULL)
       {file = FILE_FILE(PDBfile, po);
	tr   = file->tr;
	fun  = tr->write;};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TR_READ_BIN - read an entry from the specified binary file */

object *TR_read_bin(object *arg)
   {PFBinRead fun;
    PDBfile *file;
    tr_layer *tr;
    g_file *po;
    object *o;

    po = NULL;
    SS_args(arg,
            G_FILE, &po,
            0);
    
    o = SS_f;
    if (po != NULL)
       {file = FILE_FILE(PDBfile, po);
	tr   = file->tr;
	fun  = tr->read;};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TR_FILE_TYPE - return a string object containing the file type name */

object *TR_file_type(object *arg)
   {g_file *po;
    PDBfile *file;
    object *o;

    po   = NULL;
    file = NULL;
    SS_args(arg,
            G_FILE, &po,
            0);

    if (po != NULL)
       file = FILE_FILE(PDBfile, po);

    o = (file == NULL) ? SS_null : SS_mk_string(PDBFILE_S);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TR_SPOKEP - return #t iff the named spoke is present */

static object *TR_spokep(object *arg)
   {object *rv;

    rv = SS_f;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* SX_INSTALL_FILE_FUNCS - install the generic binary file functions */
 
void SX_install_file_funcs(void)
   {

    SS_install("open-bin-file",
               "Open a binary file",
	       SS_nargs,
	       TR_open_bin, SS_PR_PROC);

    SS_install("close-bin-file",
               "Close a binary file",
	       SS_nargs,
	       TR_close_bin, SS_PR_PROC);

    SS_install("file-type",
               "Return the type of the given file as a string",
	       SS_sargs,
	       TR_file_type, SS_PR_PROC);

    SS_install("have-spoke?",
               "Return the #t iff the argument names a present spoke",
	       SS_sargs,
	       TR_spokep, SS_PR_PROC);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
