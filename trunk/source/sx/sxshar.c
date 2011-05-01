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
	fun  = tr->write;
	SC_ASSERT(fun != NULL);};

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
	fun  = tr->read;
	SC_ASSERT(fun != NULL);};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TR_FILE_TYPE - return a string object containing the file type name */

object *TR_file_type(SS_psides *si, object *arg)
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

    o = (file == NULL) ? SS_null : SS_mk_string(si, PDBFILE_S);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TR_SPOKEP - return #t iff the named spoke is present */

static object *TR_spokep(SS_psides *si, object *arg)
   {object *rv;

    rv = SS_f;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* SX_INSTALL_FILE_FUNCS - install the generic binary file functions */
 
void SX_install_file_funcs(void)
   {

    SS_install("file-type",
               "Return the type of the given file as a string",
	       SS_sargs,
	       TR_file_type, SS_PR_PROC);

    SS_install("have-spoke?",
               "Return the #t iff the argument names an available spoke",
	       SS_sargs,
	       TR_spokep, SS_PR_PROC);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
