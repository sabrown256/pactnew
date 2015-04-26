/*
 * PDSHAR.C - routines for all of C coded spoke handlers
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "pdb_int.h"

SC_array
 *_PD_file_types;

extern int
 _PD_register_spokes(void);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_REGISTER - register objects defining a translation layer
 *              - return the index of the new tr_layer
 */

int _PD_register(char *type, const char *fmt, PFBinType hook,
		 PFBinCreate crt, PFBinOpen opn, PFBinClose cls,
		 PFBinWrite wrt, PFBinRead rd)
   {int n;
    tr_layer tr;
		 
    if (_PD_file_types == NULL)
       _PD_file_types = CMAKE_ARRAY(tr_layer, NULL, 3);

    if (wrt == NULL)
       wrt = _PD_write;

    if (rd == NULL)
       rd = (PFBinRead) _PD_read;

    tr.type    = type;
    tr.fmt     = fmt;
    tr.gettype = hook;
    tr.create  = crt;
    tr.open    = opn;
    tr.close   = cls;
    tr.write   = wrt;
    tr.read    = rd;

    SC_array_push(_PD_file_types, &tr);
    n = SC_array_get_n(_PD_file_types) - 1;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_LOOKUP - lookup and return a translation layer named by TYPE */

tr_layer *_PD_lookup(char *type)
   {int i, n;
    tr_layer *tr, *ptr;
		 
    _PD_register_spokes();

    n  = SC_array_get_n(_PD_file_types);

    ptr = NULL;
    for (i = 0; i < n; i++)
        {tr = SC_array_get(_PD_file_types, i);
         if (strcmp(tr->type, type) == 0)
	    {ptr = tr;
	     break;};};

    return(ptr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_LOOKUP_FMT - lookup and return a translation layer named by FMT */

tr_layer *_PD_lookup_fmt(const char *fmt)
   {int i, n;
    tr_layer *tr, *ptr;
		 
    _PD_register_spokes();

    n  = SC_array_get_n(_PD_file_types);

    ptr = NULL;
    for (i = 0; i < n; i++)
        {tr = SC_array_get(_PD_file_types, i);
         if (strcmp(tr->fmt, fmt) == 0)
	    {ptr = tr;
	     break;};};

    return(ptr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_LOOKUP_FN - lookup and return a translation layer named by
 *               - the filename, FN, suffux
 */

tr_layer *_PD_lookup_fn(char *fn)
   {int ns;
    char **sa;
    tr_layer *ptr;
		 
    ptr = NULL;

    sa = SC_tokenize(fn, ".");
    if (sa != NULL)
       {ns  = PS_lst_length(sa);
	ptr = _PD_lookup_fmt(sa[ns-1]);
	SC_free_strings(sa);};

    return(ptr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CONV_IN - convert some data to internal format */

void _PD_conv_in(PDBfile *file, void *out, void *in, char *type, inti ni)
   {PD_smp_state *pa;

    pa = _PD_get_state(-1);

    switch (SETJMP(pa->trace_err))
       {case ABORT :
	     return;
        case ERR_FREE :
	     return;
        default :
	     memset(PD_gs.err, 0, MAXLINE);
	     break;};

    PD_convert((char **) &out, (char **) &in, type, type, ni,
               file->std, file->host_std, file->host_std,
               file->chart, file->host_chart, 0, PD_TRACE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CLOSE_BIN - close the named binary file */

int _PD_close_bin(PDBfile *file)
   {int rv;
    PFBinClose fun;
    tr_layer *tr;

    rv = TRUE;

    if (file != NULL)
       {tr  = file->tr;
	fun = tr->close;
	rv  = (*fun)(file);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WRITE_BIN - write an entry to the specified binary file */

inti _PD_write_bin(PDBfile *file)
   {inti ni;
    PFBinWrite fun;
    tr_layer *tr;
    
    ni = 0;

    if (file != NULL)
       {tr  = file->tr;
	fun = tr->write;
	SC_ASSERT(fun != NULL);};

    return(ni);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_READ_BIN - read an entry from the specified binary file */

inti PD_read_bin(PDBfile *file)
   {inti ni;
    PFBinRead fun;
    tr_layer *tr;

    ni = 0;

    if (file != NULL)
       {tr  = file->tr;
	fun = tr->read;
	SC_ASSERT(fun != NULL);};

    return(ni);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FILE_TYPE - return a string containing the file type name */

char *_PD_file_type(PDBfile *file)
   {char *ty;

    ty = (file == NULL) ? NULL : file->tr->type;

    return(ty);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SPOKEP - return TRUE iff the named spoke is present */

int _PD_spokep(char *type)
   {int rv;

    rv = (_PD_lookup(type) != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PDBFILEP - return TRUE iff TYPE is PDBfile */

int _PD_pdbfilep(char *type)
   {int rv;

    rv = (strcmp(type, G_PDBFILE_S) == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PD_REGISTER_PDB - install the basic methods for PDBLib */
 
int PD_register_pdb(void)
   {static int n = -1;

    ONCE_SAFE(TRUE, NULL)

       n = PD_REGISTER(G_PDBFILE_S, "pdb", _PD_pdbfilep,
		       _PD_create, _PD_open, _PD_close, _PD_write, _PD_read);

       SC_ASSERT(n >= 0);

    END_SAFE;

    return(n);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_OPEN_BIN_AUX - open a PDB file */

static PDBfile *_PD_open_bin_aux(SC_udl *pu, const char *name,
				 const char *mode, tr_layer *tr, void *a)
   {PDBfile *file;
    PFBinOpen opn;
    PFBinCreate crt;

    opn  = tr->open;
    crt  = tr->create;
    file = NULL;

/* create the file */
    if (*mode == 'w')
       {if (crt != NULL)
	   {if (pu != NULL)
	       file = crt(tr, pu, name, mode, a);};}

/* open an existing file */
    else if (opn != NULL)

/* if we have an open C level file try to open a PDB level file */
       {if (_PD_data_source(pu) != NULL)
	   file = opn(tr, pu, name, mode, a);

/* if we cannot open existing file and it does not exist
 * try creating file
 */
	if ((file == NULL) && (SC_isfile(pu->path) == FALSE))
	   {if ((*mode == 'a') && (crt != NULL) && (tr->type == G_PDBFILE_S))
	       {_SC_rel_udl(pu);
	        file = _PD_open_bin_aux(pu, name, "w", tr, a);};};};

    return(file);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_OPEN_BIN - open up the specified UDL */

PDBfile *_PD_open_bin(const char *name, const char *mode, void *a)
   {int i, n, ok;
    char *md, *type, *rfmt;
    const char *tfmt;
    SC_udl *pu;
    tr_layer *tr;
    PDBfile *file;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    if (!_PD_IS_SMP_INIT)
       PD_init_threads(0, NULL);

    pu   = NULL;
    file = NULL;

/* set error traps and open the files at the C/system level */
    if (*mode == 'w')
       {switch (SETJMP(pa->create_err))
	   {case ABORT :
	         if (pu != NULL)
		    _SC_rel_udl(pu);
		 return(NULL);
	    case ERR_FREE :
	         return(file);
	    default :
	         memset(pa->err, 0, MAXLINE);
		 break;};

	md = BINARY_MODE_WPLUS;
	pu = _PD_pio_open(name, md);}

    else
       {switch (SETJMP(pa->open_err))
	    {case ABORT :
	          if (pu != NULL)
		     _SC_rel_udl(pu);
		  return(NULL);
	     case ERR_FREE :
	          return(file);
	     default :
	          memset(pa->err, 0, MAXLINE);
		  break;};

	pu = NULL;

/* try to open the file read/write */
	if (*mode == 'a')
	   {md = BINARY_MODE_RPLUS;
	    pu = _PD_pio_open(name, md);};

/* if that fails try to read only */
	if (_PD_data_source(pu) == NULL)
	   {md = BINARY_MODE_R;
	    pu = _PD_pio_open(name, md);};

/* if that fails try create it */
	if ((_PD_data_source(pu) == NULL) && (*mode == 'a'))
	   return(_PD_open_bin(name, "w", a));};

    file = NULL;

    if (_PD_data_source(pu) != NULL)

/* get the specified format */
       {if (pu->protocol != NULL)
	   rfmt = pu->protocol;
        else
	   rfmt = SC_assoc(pu->info, "fmt");

	n = _PD_register_spokes();
	for (i = 0; i < n; i++)
	    {tr = SC_array_get(_PD_file_types, i);
	     type = tr->type;
	     tfmt = tr->fmt;

/* work out whether this translation layer matches the request
 * when creating a file
 * empty request matches PDB format only
 */
	     if (*mode == 'w')
	        {if (rfmt == NULL)
		    ok = (strcmp(tfmt, "pdb") == 0);
		 else
		    ok = (strcmp(tfmt, rfmt) == 0);}
	     else
	        ok = TRUE;

/* open the file */
	     if (ok == TRUE)
	        {file = _PD_open_bin_aux(pu, name, mode, tr, a);
		 if (file != NULL)
		    {_PD_def_real(type, file);
		     break;};};};};

    if ((file == NULL) && (pu != NULL))
       _SC_rel_udl(pu);

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
