/*
 * SXPDB.C - command processors for PDBLib extensions in SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"
#include "scope_proc.h"
#include "scope_mpi.h"

typedef syment *(*PDBFileWrite)(PDBfile *fp, char *path,
				char *inty, char *outty,
				void *vr, dimdes *dims);
typedef int (*PDBFileRead)(PDBfile *fp, char *path, char *ty,
			   syment *ep, void *vr);

/*--------------------------------------------------------------------------*/

#if 0

/*--------------------------------------------------------------------------*/

/* _SX_TARGET_OLD - set the data standard and alignment for the next file to be
 *                - created. If an argument is 0 or missing, the corresponding
 *                - value is not changed.
 */

static void _SX_target_old(SS_psides *si, int data, int align)
   {int data_max, align_max;

    data_max  = 0;
    align_max = 0;

    if (data == 0)
       {if (PD_gs.req_standard != NULL)
           while (PD_gs.req_standard != PD_gs.std_standards[data++]);}
    else
       {while (PD_gs.std_standards[data_max++]);
        if ((data < 1) || (data >= data_max))
           SS_error(si, "UNKNOWN DATA STANDARD - _SX_TARGET",
                    SS_mk_integer(si, data));};
        
    if (align == 0)
       {if (PD_gs.req_alignment != NULL)
           while (PD_gs.req_alignment != PD_gs.std_alignments[align++]);}
    else
       {while (PD_gs.std_alignments[align_max++]);
       if ((align < 1) || (align >= align_max))
          SS_error(si, "UNKNOWN DATA ALIGNMENT - _SX_TARGET",
                   SS_mk_integer(si, align));};

    if (data != 0)
       PD_gs.req_standard  = PD_gs.std_standards[data - 1];
    if (align != 0)
       PD_gs.req_alignment = PD_gs.std_alignments[align - 1];

    PD_target(PD_gs.req_standard, PD_gs.req_alignment);

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* _SX_TARGET - set the data standard and alignment for the next file to be
 *            - created. If an argument is 0 or missing, the corresponding
 *            - value is not changed.
 */

static void _SX_target(SS_psides *si,
		       PD_data_std_i data, PD_data_algn_i align)
   {int a, d;

    if (data == PD_NO_STD)
       {if (PD_gs.req_standard != NULL)
           while (PD_gs.req_standard != &PD_gs.standards[data++]);}
    else
       {d = data;
        if ((d < 0) || (d >= PD_N_STANDARDS))
           SS_error(si, "UNKNOWN DATA STANDARD - _SX_TARGET",
                    SS_mk_integer(si, d));};
        
    if (align == PD_NO_ALGN)
       {if (PD_gs.req_alignment != NULL)
           while (PD_gs.req_alignment != &PD_gs.alignments[align++]);}
    else
       {a = align;
	if ((a < 0) || (a >= PD_N_ALIGNMENTS))
           SS_error(si, "UNKNOWN DATA ALIGNMENT - _SX_TARGET",
		    SS_mk_integer(si, a));};

    if (data != PD_NO_STD)
       PD_gs.req_standard  = &PD_gs.standards[data];
    if (align != PD_NO_ALGN)
       PD_gs.req_alignment = &PD_gs.alignments[align];

    PD_target(PD_gs.req_standard, PD_gs.req_alignment);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_TARGET - set the data standard and alignment for subsequently created
 *             - files. If an argument is 0 or missing, the corresponding
 *             - value is not changed. The data standard and alignment are
 *             - returned as a pair. If the data standard or alignment was
 *             - never set, 0 is returned.
 */

static object *_SXI_target(SS_psides *si, object *arg)
   {PD_data_std_i data;
    PD_data_algn_i align;
    object *o;

    data  = PD_NO_STD;
    align = PD_NO_ALGN;

    SS_args(si, arg,
            SC_ENUM_I, &data,
            SC_ENUM_I, &align,
            0);

    _SX_target(si, data, align);

    o = SS_mk_cons(si, SS_mk_integer(si, data),
		   SS_mk_integer(si, align));

    return(o);}

/*--------------------------------------------------------------------------*/

/*                          RAW BINARY FILE I/O                             */

/*--------------------------------------------------------------------------*/

/* _SXI_OPEN_RAW_FILE - open a file as a binary input port */

static object *_SXI_open_raw_file(SS_psides *si, object *argl)
   {PD_data_std_i data;
    PD_data_algn_i align;
    char *name, *mode, *type, *md;
    SC_udl *pu;
    PDBfile *file;
    PD_smp_state *pa;
    FILE *fp;
    object *o;

    if (!_PD_IS_SMP_INIT)
       PD_init_threads(0, NULL);

    data  = PD_NO_STD;
    align = PD_NO_ALGN;
    type  = NULL;
    SS_args(si, argl,
            SC_STRING_I, &name,
            SC_STRING_I, &mode,
            SC_STRING_I, &type,
            SC_ENUM_I, &data,
            SC_ENUM_I, &align,
            0);

/* if it is a UDL */
    if (_SC_udlp(name) == TRUE)
       return(SS_null);

    _SX_target(si, data, align);

    pa = _PD_get_state(-1);

    file = NULL;
    fp   = NULL;
    switch (SETJMP(pa->open_err))
       {case ABORT :
	     lio_close(fp);
	     return(SS_f);
        default :
	     memset(pa->err, 0, MAXLINE);
	     break;};

    _PD_set_null_comm(-1);

    if (*mode == 'w')
       {md = BINARY_MODE_WPLUS;
	pu = _PD_pio_open(name, md);}

    else
       {md = BINARY_MODE_RPLUS;
        pu = _PD_pio_open(name, md);};

    if (pu == NULL)
       {*mode = 'r';
	md = BINARY_MODE_R;
        pu = _PD_pio_open(name, md);
	if (pu == NULL)
           return(SS_null);};

    fp = pu->stream;

    file = _PD_mk_pdb(pu, NULL, NULL, TRUE, NULL, NULL);
    if (file == NULL)
       SS_error(si, "CAN'T ALLOCATE PDBFILE - _SXI_OPEN_RAW_FILE", argl);

/* initialize the PDB file */
    file->chrtaddr = 0;

    if (*mode == 'a')
       file->mode = PD_APPEND;
    else
       file->mode = PD_OPEN;

    _PD_set_standard(file, NULL, NULL);

    _PD_init_chrt(file, TRUE);

    o  = SX_mk_gfile(si, _SX_mk_file(name, PDBFILE_S, file));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SEEK_RAW_FILE - seek in a file */

static object *_SXI_seek_raw_file(SS_psides *si, object *argl)
   {int whence, ret;
    int64_t addr;
    PDBfile *file;
    FILE *fp;
    g_file *po;

    addr   = 0;
    whence = 0;
    SS_args(si, argl,
            G_FILE, &po,
            SC_LONG_LONG_I, &addr,
            SC_INT_I, &whence,
            0);

    if (po == NULL)
       SS_error(si, "BAD FILE POINTER - _SXI_SEEK_RAW_FILE", argl);
       
    else
       {file = FILE_FILE(PDBfile, po);
	fp   = file->stream;

	switch (whence)
	   {default :
	    case -1 :
	         ret = lio_seek(fp, addr, SEEK_SET);
	         break;
	    case -2 :
	         ret = lio_seek(fp, addr, SEEK_CUR);
	         break;
	    case -3 :
                 ret = lio_seek(fp, addr, SEEK_END);
	         break;};

	if (ret != 0)
	   SS_error(si, "CAN'T SEEK IN FILE - _SXI_SEEK_RAW_FILE", argl);};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_CLOSE_RAW_FILE - close a file as a binary input port */

static object *_SXI_close_raw_file(SS_psides *si, object *argl)
   {PDBfile *file;
    g_file *po;
    FILE *fp;

    SS_args(si, argl,
            G_FILE, &po,
            0);

    if (po == NULL)
       SS_error(si, "BAD FILE POINTER - _SXI_CLOSE_RAW_FILE", argl);
       
    else
       {file = FILE_FILE(PDBfile, po);
	fp   = file->stream;

	if (lio_close(fp) != 0)
	   SS_error(si, "CAN'T CLOSE FILE - _SXI_CLOSE_RAW_FILE", argl);

/* free the space */
	_PD_rl_pdb(file);};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_RD_RAW - read from a raw binary file
 *             - (read-binary <file> <address> <nitems>
 *             -              <file-type> [<mem-type>])
 */

static object *_SXI_rd_raw(SS_psides *si, object *argl)
   {int ret;
    char *intype, *outtype;
    long ni, nir;
    int64_t addr;
    void *vr;
    PDBfile *file;
    PD_smp_state *pa;
    FILE *fp;
    syment *ep;
    g_file *po;
    object *rv;

    pa = _PD_get_state(-1);

    rv = SS_null;

    if (SS_length(si, argl) > 4)
       SS_args(si, argl,
               G_FILE, &po,
               SC_LONG_LONG_I, &addr,
               SC_LONG_I, &ni,
               SC_STRING_I, &intype,
               SC_STRING_I, &outtype,
               0);
    else
       {SS_args(si, argl,
                G_FILE, &po,
                SC_LONG_LONG_I, &addr,
                SC_LONG_I, &ni,
                SC_STRING_I, &intype,
                0);

        outtype = intype;};

    if (po == NULL)
       SS_error(si, "BAD FILE POINTER - _SXI_RD_RAW", argl);
       
    else
       {file = FILE_FILE(PDBfile, po);
	fp   = file->stream;

	switch (addr)
	   {case -1 :
	         ret = lio_seek(fp, 0, SEEK_SET);
		 break;
	    case -2 :
	         ret = FALSE;
		 break;
	    case -3 :
	         ret = lio_seek(fp, 0, SEEK_END);
		 break;
	    default :
	         ret = lio_seek(fp, addr, SEEK_SET);
		 break;};
	
	addr = lio_tell(fp);
	ep   = _PD_mk_syment(intype, ni, addr, NULL, NULL);

	if (ret)
	   SS_error(si, "FSEEK FAILED TO FIND REQUESTED ADDRESS - _SXI_RD_RAW",
		    argl);

/* pad char arrays with 2 null characters - for printing and SC_firsttok */
	if (strcmp(outtype, SC_CHAR_S) == 0)
	   vr = _PD_alloc_entry(file, outtype, ni + 2L);
	else
	   vr = _PD_alloc_entry(file, outtype, ni);

	if (vr == NULL)
	   SS_error(si, "CAN'T ALLOCATE MEMORY - _SXI_RD_RAW", SS_null);

	switch (SETJMP(pa->read_err))
	   {case ABORT :
	         return(SS_f);
	    default :
	         memset(PD_gs.err, 0, MAXLINE);
		 break;};

	nir = _PD_sys_read(file, ep, outtype, vr);
	SC_ASSERT(nir >= 0);

	rv = _SX_mk_gpdbdata(si, "data", vr, ep, file);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_WR_RAW - write to a raw binary file
 *             - (write-binary <file> <data> <address> <nitems>
 *             -               <file-type> [<mem-type>])
 */

static object *_SXI_wr_raw(SS_psides *si, object *argl)
   {int ret;
    long ni;
    long long addr;
    char *name, *intype, *outtype;
    void *vr;
    PDBfile *file;
    PD_smp_state *pa;
    object *obj;
    FILE *fp;
    g_file *po;
    
    pa = _PD_get_state(-1);

    name = "/sx-var";

    vr = NULL;

    if (SS_length(si, argl) > 5)
       SS_args(si, argl,
               G_FILE, &po,
               SS_OBJECT_I, &obj,
               SC_LONG_LONG_I, &addr,
               SC_LONG_I, &ni,
               SC_STRING_I, &intype,
               SC_STRING_I, &outtype,
               0);
    else
       {SS_args(si, argl,
                G_FILE, &po,
                SS_OBJECT_I, &obj,
                SC_LONG_LONG_I, &addr,
                SC_LONG_I, &ni,
                SC_STRING_I, &intype,
                0);

        outtype = intype;};

    if (SX_PDBDATAP(obj))
       vr = (void *) PDBDATA_DATA(obj);
    else
       SS_error(si, "CAN'T HANDLE THIS DATA - _SXI_WR_RAW", argl);

    if (po == NULL)
       SS_error(si, "BAD FILE POINTER - _SXI_WR_RAW", argl);
       
    else
       {file = FILE_FILE(PDBfile, po);
	fp   = file->stream;

	if (lio_flush(fp))
	   SS_error(si, "FFLUSH FAILED BEFORE READ - _SXI_WR_RAW", argl);

	switch (addr)
	   {case -1 :
	         ret = lio_seek(fp, 0, SEEK_SET);
		 break;
	    case -2 :
		 ret = FALSE;
		 break;
	    case -3 :
		 ret = lio_seek(fp, 0, SEEK_END);
		 break;
	    default :
	         ret = lio_seek(fp, addr, SEEK_SET);
		 break;};

	if (ret)
	   SS_error(si, "FSEEK FAILED TO FIND REQUESTED ADDRESS - _SXI_WR_RAW",
		    argl);

	switch (SETJMP(pa->write_err))
	   {case ABORT :
	         return(SS_f);
	    default :
	         memset(PD_gs.err, 0, MAXLINE);
		 break;};

	_PD_sys_write(file, name, vr, ni, intype, outtype);};

    return(SS_t);}

/*--------------------------------------------------------------------------*/

/*                              PDB FILE I/O                                */

/*--------------------------------------------------------------------------*/

/* SX_MAKE_DIMS_DIMDES - take a list of integers and make a dimension
 *                     - descriptor
 *                     - (type dimdes)
 */

static dimdes *_SX_make_dims_dimdes(SS_psides *si, PDBfile *file,
				    object *argl)
   {long mini, leng;
    object *dim_obj;
    dimdes *dims, *next, *prev;

    mini = 0;
    leng = 0;
    dims = NULL;
    for ( ; !SS_nullobjp(argl); argl = SS_cdr(si, argl))
        {dim_obj = SS_car(si, argl);
         if (SS_integerp(dim_obj))
            {mini = (long) file->default_offset;
             leng = SS_INTEGER_VALUE(dim_obj);}
         else if (SS_floatp(dim_obj))
            {mini = (long) file->default_offset;
             leng = SS_FLOAT_VALUE(dim_obj);}
         else if (SS_consp(dim_obj))
            {mini = SS_INTEGER_VALUE(SS_car(si, dim_obj));
             leng = SS_INTEGER_VALUE(SS_cdr(si, dim_obj)) - mini + 1;}
         else
            SS_error(si,
		       "DIMENSIONS MUST BE INTEGERS - _SX_MAKE_DIMS_DIMDES",
		       dim_obj);

         next = _PD_mk_dimensions(mini, leng);
         if (dims == NULL)
            dims = next;
         else
            {prev->next = next;
	     SC_mark(next, 1);};

         prev = next;};

    return(dims);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_SPEC_INSTANCE - convert the data specification in the arg list to
 *                   - an instance in memory and return it
 *                   -
 *                   - Syntax:
 *                   -
 *                   -    ((type <type-name> [<dim1> ... <dimn>])
 *                   -     <item1> ... <itemn>)
 *                   -
 *                   - each pointered type has the data in a list
 *                   -
 *                   - Examples:
 *                   -
 *                   - (make-defstr* nil "bird"
 *                   -               (def-member integer i)
 *                   -               (def-member integer *i1)
 *                   -               (def-member char c 10) 
 *                   -               (def-member char *a)
 *                   -               (def-member char **s)
 *                   -               (def-member float f))
 *                   -
 *                   - ((type "bird" 3)
 *                   -  '(10 ((1 2))
 *                   -    "test" ("doggie") (("big" "ugly")) 20.0)
 *                   -  '(20 ((3 4 5 6))
 *                   -    "baz"  ("kitty")  (("nice" "soft" "warm")) 22.0)
 *                   -  '(30 (7)
 *                   -    "foo"  ("birdy")  (("small")) 24.0)))
 */

static syment *_SX_spec_instance(SS_psides *si, PDBfile *file,
				 int defent, object *argl)
   {object *data;
    SC_address val;
    long number;
    char *type, *label;
    dimdes *dims;
    syment *ep;

    ep = _PD_mk_syment(NULL, 0L, 0L, NULL, NULL);
    if (ep != NULL)
       {val.memaddr = NULL;

	data = SS_car(si, argl);
	argl = SS_cdr(si, argl);

	if (!SS_consp(data))
	   SS_error(si, "SHOULD BE LIST - _SX_SPEC_INSTANCE", data);

	label = CSTRSAVE(SS_get_string(SS_car(si, data)));
	data  = SS_cdr(si, data);
	if (strcmp(label, "type") == 0)
	   {type        = CSTRSAVE(SS_get_string(SS_car(si, data)));
	    dims        = _SX_make_dims_dimdes(si, file, SS_cdr(si, data));
	    number      = _PD_comp_num(dims);
	    if (defent)
	       val.memaddr = NULL;
	    else
	       {val.memaddr = _PD_alloc_entry(file, type, number);
		if (val.memaddr == NULL)
		   SS_error(si, "CAN'T ALLOCATE TYPE - _SX_SPEC_INSTANCE", data);

		_SX_rd_tree_list(si, argl, file,
				 val.memaddr, number, type, dims);
		SC_mark(val.memaddr, 1);};

	    ep->type       = CSTRSAVE(type);
            ep->dimensions = dims;
            ep->number     = number;
            PD_entry_set_address(ep, val.diskaddr);};};

    return(ep);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_PDBFILEP - return TRUE iff an object is a G_FILE of external type PDB */

int SX_pdbfilep(object *obj)
   {int rv;
    PDBfile *file;
    g_file *po;

    rv = FALSE;
    if (SX_FILEP(obj))
       {po = SS_GET(g_file, obj);

	file = FILE_FILE(PDBfile, po);

/* the virtual internal file may have a NULL translation layer
 * but it is a PDBfile
 */
	if ((file == NULL) || (strcmp(file->tr->type, PDBFILE_S) == 0))
           rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_IPDBFILEP - return TRUE iff an object is a G_FILE of internal type PDB */

int SX_ipdbfilep(object *obj)
   {int rv;

    rv = FALSE;
    if (SX_FILEP(obj))
       {if (strcmp(FILE_TYPE(obj), PDBFILE_S) == 0)
           rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_PDBFILE - examine the argument list and return either the
 *                - PDBfile which is first on the list or 
 *                - the default internal file if there is no file on the
 *                - argument list
 */

object *SX_get_pdbfile(SS_psides *si, object *argl,
		       PDBfile **pfile, g_file **gfile)
   {PDBfile *pf;
    g_file *po;
    object *fo;

    pf = SX_gs.vif;
    po = SX_gs.gvif;

    fo = SS_car(si, argl);
    if (SX_FILEP(fo))
       {po = SS_GET(g_file, fo);
	pf = FILE_FILE(PDBfile, po);};

    argl   = SS_cdr(si, argl);
    *pfile = pf;
    *gfile = po;

    return(argl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_FILE - examine the argument list and return either the
 *             - g_file which is first on the list or 
 *             - the default internal file if there is no file on the
 *             - argument list
 */

object *SX_get_file(SS_psides *si, object *argl, g_file **pfile)
   {object *fo;
    g_file *po;

    fo = SS_car(si, argl);
    if (SX_FILEP(fo))
       {po     = SS_GET(g_file, fo);
        argl   = SS_cdr(si, argl);
        *pfile = po;}

    else if (SS_nullobjp(fo))
       {argl   = SS_cdr(si, argl);
        *pfile = SX_gs.gvif;};
       
    return(argl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_OPEN_FILE - open up the named G_FILE and
 *               - cons its object to SX_gs.file_list.
 */

object *_SX_open_file(SS_psides *si, object *arg, char *type, char *mode)
   {char *name;
    g_file *po, *pn;
    object *o;

    name = NULL;
    SS_args(si, arg,
            SC_STRING_I, &name,
            0);

    if (name == NULL)
       SS_error(si, "BAD FILE NAME - _SX_OPEN_FILE", arg);

/* search for an existing file by this name */
    for (po = SX_gs.file_list; po != NULL; po = po->next)
        {if (strcmp(name, po->name) == 0)
	    {o = po->file_object;
             return(o);};};

/* add to file_list */
    pn = _SX_mk_open_file(si, name, type, mode);

/* if file could not be opened return #f so Scheme spoke has a chance */
    if (pn == NULL)
       o = SS_f;

    else
       {SX_gs.file_list = pn;

	o = SX_gs.file_list->file_object;};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_COLLECT_IO_INFO - return the file_io_desc stats */

static object *_SXI_collect_io_info(SS_psides *si, object *arg)
   {int wh;
    object *o;

    wh = FALSE;
    SS_args(si, arg,
            SC_INT_I, &wh,
            0);

    wh = SC_collect_io_info(wh);

    o = SS_mk_integer(si, wh);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GET_IO_INFO - return the file_io_desc stats */

static object *_SXI_get_io_info(SS_psides *si, object *arg)
   {int i;
    int *nh;
    double *ns;
    g_file *po;
    PDBfile *file;
    object *o, *c;

    po = NULL;
    SS_args(si, arg,
            G_FILE, &po,
            0);

    o = SS_null;
    if (po != NULL)
       {file = FILE_FILE(PDBfile, po);
	SC_get_io_info(file->stream, &nh, &ns);

	if ((ns != NULL) && (nh != NULL))
	   {for (i = 0; i < SC_N_IO_OPER; i++)
	        {c = SS_mk_cons(si, SS_mk_integer(si, nh[i]),
				SS_mk_float(si, ns[i]));
		 o = SS_mk_cons(si, c, o);};

	    o = SS_reverse(si, o);};};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_CREATE_PDBFILE - create up the named PDBFile */

static object *_SXI_create_pdbfile(SS_psides *si, object *arg)
   {object *o;

    o = _SX_open_file(si, arg, PDBFILE_S, "w");

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_OPEN_PDBFILE - open up the named PDBFile */

static object *_SXI_open_pdbfile(SS_psides *si, object *argl)
   {char mode[10], *md;
    object *obj, *o;

    strcpy(mode, "r");
    if (SS_consp(argl))
       {obj  = SS_car(si, argl);
        argl = SS_cdr(si, argl);
        if (SS_consp(argl))
           {md      = SS_get_string(SS_car(si, argl));
	    SC_strncpy(mode, 10, md, -1);};}
    else
       obj = argl;

    o = _SX_open_file(si, obj, PDBFILE_S, mode);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_FLUSH_PDBFILE - flush a PDBFile */

static object *_SXI_flush_pdbfile(SS_psides *si, object *arg)
   {g_file *po;
    PDBfile *file;
    object *o;

    po = NULL;
    SS_args(si, arg,
            G_FILE, &po,
            0);

    o = SS_f;
    if (po != NULL)
       {file = FILE_FILE(PDBfile, po);
	if (PD_flush(file))
	   o = SS_t;};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DEF_COMMON_TYPES - define common internal SX types to the file */

static object *_SXI_def_common_types(SS_psides *si, object *arg)
   {g_file *po;
    PDBfile *file;

    po = NULL;
    SS_args(si, arg,
            G_FILE, &po,
            0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    _SX_init_hash_objects(si, file);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_ENTRY_NUMBER - return the number of items for the named variable */

static object *_SXI_entry_number(SS_psides *si, object *argl)
   {char *name;
    g_file *po;
    PDBfile *file;
    object *obj;
    syment *ep;

    po   = NULL;
    name = NULL;
    SS_args(si, argl,
            G_FILE, &po,
            SC_STRING_I, &name,
            0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    obj = SS_null;
    if (name != NULL)
       {ep = _PD_effective_ep(file, name, TRUE, NULL);
        if (ep != NULL)
	   obj = SS_mk_integer(si, PD_entry_number(ep));};

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_RESET_PTRS - call PD_reset_ptr_list */

static object *_SXI_reset_ptrs(SS_psides *si, object *argl)
   {PDBfile *file;
    g_file *po;

    po = NULL;
    SS_args(si, argl,
            G_FILE, &po,
            0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    PD_reset_ptr_list(file);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_FAMILY_FILE - family the file */

static object *_SXI_family_file(SS_psides *si, object *argl)
   {int ifl;
    PDBfile *file;
    g_file *po;
    object *fl, *o;

    o  = SS_null;
    po = NULL;
    fl = SS_t;
    SS_args(si, argl,
            G_FILE, &po,
            SS_OBJECT_I, &fl,
            0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    ifl  = SS_true(fl);
    file = PD_family(file, ifl);
    if (file != NULL)
       o = SX_mk_gfile(si, _SX_mk_file(file->name, PDBFILE_S, file));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_FILE_CONTENT - print the metadata of the given file
 *                   - Usage: (file-content <file> [<outfile>] [<version>] [<chart>])
 */

static object *_SXI_file_content(SS_psides *si, object *argl)
   {int vers, chrt;
    g_file *po;
    PDBfile *file;
    FILE *out;
    object *fout;

    vers = PD_get_fmt_version();

    po   = NULL;
    fout = SS_null;
    chrt = 0;
    SS_args(si, argl,
            G_FILE, &po,
            SS_OBJECT_I, &fout,
	    SC_INT_I, &vers,
	    SC_INT_I, &chrt,
            0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    if (SS_nullobjp(fout))
       out = stdout;
    else
       out = SS_OUTSTREAM(fout);

    PD_contents(file, out, chrt, vers);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_FILE_INFO - return the named info about the file
 *                - available info is:
 *                -    name (char *)                  - file name
 *                -    mode (int)                     - file mode
 *                -    default-offset (int)           - index default offset
 *                -    conversions (int)              - whether conversions will be done
 *                -    flushed (int)                  - whether file has been flushed
 *                -    virtual-internal (int)         - whether file is "internal"
 *                -    system-version (int)           - PDB system version
 *                -    major-order (int)              - major order for indexes
 *                -    header-address (int64_t)       - address of header info
 *                -    chart-address (int64_t)        - address of structure chart
 *                -    symbol-table-address (int64_t) - address of symbol table
 *                -    date (char *)                  - file creation date
 *                -    maximum_size (int64_t)         - threshold for new file
 *                -    previous_file (char *)         - previous file in family
 */

static object *_SXI_file_info(SS_psides *si, object *argl)
   {char *name;
    g_file *po;
    PDBfile *file;
    object *obj;

    po   = NULL;
    name = NULL;
    SS_args(si, argl,
            G_FILE, &po,
            SC_STRING_I, &name,
            0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

/* put some help here - return a list of available info or something */
    if (name == NULL)
       obj = SS_null;

    else if (strcmp(name, "position") == 0)
       {FILE *fp;

	fp = file->stream;

        if (fp != NULL)
           obj = SS_mk_integer(si, lio_tell(fp));
        else
           obj = SS_mk_integer(si, -1);}

    else if (strcmp(name, "name") == 0)
       obj = SS_mk_string(si, file->name);

    else if (strcmp(name, "mode") == 0)
       obj = SS_mk_integer(si, file->mode);    

    else if (strcmp(name, "default-offset") == 0)
       obj = SS_mk_integer(si, file->default_offset);    

    else if (strcmp(name, "flushed") == 0)
       obj = SS_mk_integer(si, file->flushed);    

    else if (strcmp(name, "virtual-internal") == 0)
       obj = SS_mk_integer(si, file->virtual_internal);    

    else if (strcmp(name, "system-version") == 0)
       obj = SS_mk_integer(si, file->system_version);    

    else if (strcmp(name, "major-order") == 0)
       obj = SS_mk_integer(si, file->major_order);    

    else if (strcmp(name, "maximum-size") == 0)
       obj = SS_mk_integer(si, file->maximum_size);    

    else if (strcmp(name, "header-address") == 0)
       obj = SS_mk_integer(si, file->headaddr);

    else if (strcmp(name, "chart-address") == 0)
       obj = SS_mk_integer(si, file->chrtaddr);    

    else if (strcmp(name, "symbol-table-address") == 0)
       obj = SS_mk_integer(si, file->symtaddr);    

    else if (strcmp(name, "date") == 0)
       obj = SS_mk_string(si, file->date);    

    else if (strcmp(name, "previous-file") == 0)
       obj = SS_mk_string(si, file->previous_file);    

    else if (strcmp(name, "track-pointers") == 0)
       obj = SS_mk_integer(si, file->track_pointers);    

    else
       obj = SS_null;

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DEFAULT_OFFSET - set the default offset for the given file */

static object *_SXI_default_offset(SS_psides *si, object *arg)
   {int offset, nargs;
    g_file *po;
    PDBfile *file;
    object *o;

    po = NULL;
    nargs = SS_args(si, arg,
                    G_FILE, &po,
                    SC_INT_I, &offset,
                    0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    if (nargs >= 2)
       file->default_offset = offset;

    o = SS_mk_integer(si, file->default_offset);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_MAJOR_ORDER - set the file to row or column major order */

static object *_SXI_major_order(SS_psides *si, object *arg)
   {char *s;
    PD_major_order ord;
    g_file *po;
    PDBfile *file;
    object *o, *order;

    po    = NULL;
    order = SS_null;
    SS_args(si, arg,
            G_FILE, &po,
            SS_OBJECT_I, &order,
            0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    ord = file->major_order;

/* if setting major_order */
    if (SS_nullobjp(order) == FALSE)
       {if (SS_stringp(order) == TRUE)
	   {s = SS_STRING_TEXT(order);
	    if ((s[0] == 'c') || (s[0] == 'C'))
	       ord = COLUMN_MAJOR_ORDER;}
	else if (SS_floatp(order) == TRUE)
	   ord = SS_FLOAT_VALUE(order);
	else if (SS_integerp(order) == TRUE)
	   ord = SS_INTEGER_VALUE(order);

	file->major_order = ord;};

    o = SS_mk_integer(si, ord);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_FILE_MODE - set the file mode */

static object *_SXI_file_mode(SS_psides *si, object *arg)
   {char *mode;
    g_file *po;
    PDBfile *file;
    object *o;

    po = NULL;
    mode = NULL;
    SS_args(si, arg,
            G_FILE, &po,
            SC_STRING_I, &mode,
            0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    if (mode != NULL)
       {if ((mode[0] == 'r') || (mode[0] == 'R'))
           file->mode = PD_READ;
        else if ((mode[0] == 'w') || (mode[0] == 'W'))
           file->mode = PD_WRITE;
        else
           file->mode = PD_APPEND;}

    o = SS_mk_integer(si, file->mode);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_PDBFILE_TO_LIST - return a PDBfile as a list
 *                     - (symtab chart headaddr symtaddr chrtaddr)
 *
 *  Do not garbage collect the hashtables because they are
 *  still needed by the pdbfile.
 */

object *_SX_pdbfile_to_list(SS_psides *si, PDBfile *file)
   {object *obj, *obj1;

    obj = SS_null;
    obj = SS_mk_cons(si, SS_mk_integer(si, file->chrtaddr), obj);
    obj = SS_mk_cons(si, SS_mk_integer(si, file->symtaddr), obj);
    obj = SS_mk_cons(si, SS_mk_integer(si, file->headaddr), obj);

    obj1 = SS_mk_hasharr(si, file->chart);
    SS_UNCOLLECT(obj1);
    obj = SS_mk_cons(si, obj1, obj);

    obj1 = SS_mk_hasharr(si, file->host_chart);
    SS_UNCOLLECT(obj1);
    obj = SS_mk_cons(si, obj1, obj);

    obj1 = SS_mk_hasharr(si, file->symtab);
    SS_UNCOLLECT(obj1);
    obj = SS_mk_cons(si, obj1, obj);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_SYMENT_TO_LIST - returns the syment as a list of objects
 *                    - (type ni diskaddr dimensions)
 */

object *_SX_syment_to_list(SS_psides *si, syment *ep)
   {object *obj;

    obj = _SX_make_dims_obj(si, PD_entry_dimensions(ep));
    obj = SS_mk_cons(si, SS_mk_integer(si, PD_entry_address(ep)), obj);
    obj = SS_mk_cons(si, SS_mk_integer(si, PD_entry_number(ep)), obj);
    obj = SS_mk_cons(si, SS_mk_string(si, PD_entry_type(ep)), obj);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_DEFSTR_TO_LIST - returns the defstr as a list of objects
 *                    - (name size align kind (members))
 *                    - members (type name dimensions)
 */

object *_SX_defstr_to_list(SS_psides *si, defstr *dp)
   {object *obj, *obj1, *o;
    memdes *lst;

    obj = SS_null;
    obj = SS_mk_cons(si, SS_mk_string(si, dp->type), obj);
    obj = SS_mk_cons(si, SS_mk_integer(si, dp->size), obj);
    obj = SS_mk_cons(si, SS_mk_integer(si, dp->kind), obj);
    obj = SS_mk_cons(si, SS_mk_integer(si, dp->alignment), obj);

/* convert to dimension from a dimdes to a cons list */
    for (lst = dp->members; lst != NULL; lst = lst->next)
        {obj1 = SS_make_list(si, SC_STRING_I, lst->type,
                             SC_STRING_I, lst->name,
                             0);

         obj1 = SS_mk_cons(si, obj1,
                           _SX_make_dims_obj(si, lst->dimensions));

         obj = SS_mk_cons(si, obj1, obj);};

    o = SS_reverse(si, obj);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MEMDES_TO_LIST - returns the memdes as a list of objects
 *                    - member := (type name dimensions)
 */

static object *_SX_memdes_to_list(SS_psides *si, memdes *mp)
   {object *obj;

    obj = SS_make_list(si, SC_STRING_I, mp->type,
                       SC_STRING_I, mp->name,
                       SC_INT_I, &mp->number,
                       SS_OBJECT_I, _SX_make_dims_obj(si, mp->dimensions),
                       0);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_PDBDATA_TO_LIST - convert a pdbdata type to a list.
 *                     - (name (type dimension) data)
 */

object *_SX_pdbdata_to_list(SS_psides *si, char *name, void *vr,
			    syment *ep, PDBfile *file)
   {object *obj, *obj1;

    obj1 = _SX_make_dims_obj(si, PD_entry_dimensions(ep));
    obj1 = SS_mk_cons(si, SS_mk_string(si, PD_entry_type(ep)), obj1);
    obj1 = SS_mk_cons(si, SS_mk_string(si, "type"), obj1);

    obj = _SX_make_list_syment(si, file, vr,
			       PD_entry_number(ep), PD_entry_type(ep));
    obj = SS_mk_cons(si, obj1, obj);
    obj = SS_mk_cons(si, SS_mk_string(si, name), obj);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_PDB_TO_LIST - convert a pdb type to a list
 *                  -   syment to (type dimensions number diskaddr)
 *                  -   defstr to (type size members)
 *                  -   pdbdata to (file name defstr data)
 *                  -   pdbfile to (symtab chart headaddr symtaddr chrtaddr)
 */

static object *_SXI_pdb_to_list(SS_psides *si, object *arg)
   {object *obj;
    g_pdbdata *pp;

    switch (SS_OBJECT_TYPE(arg))
       {default     :
        case G_FILE :
	     if (SX_ipdbfilep(arg))
	        obj = _SX_pdbfile_to_list(si, FILE_STREAM(PDBfile, arg));
	     else
	        obj = _SX_pdbfile_to_list(si, SX_gs.vif);
	     break;
        case G_DEFSTR :
	     obj = _SX_defstr_to_list(si, SS_GET(defstr, arg));
	     break;
        case G_SYMENT :
	     obj = _SX_syment_to_list(si, SS_GET(syment, arg));
             break;
        case G_MEMDES :
	     obj = _SX_memdes_to_list(si, SS_GET(memdes, arg));
             break;
        case G_PDBDATA :
	     pp = SS_GET(g_pdbdata, arg);
	     obj = _SX_pdbdata_to_list(si, pp->name, pp->data,
				       pp->ep, pp->file);
             break;};

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_PARSE_TYPE - return a list of items describing a C/PDB type variable
 *                 - definition
 */

static object *_SXI_parse_type(SS_psides *si, object *argl)
   {char *def;
    object *ret;

    def = NULL;
    SS_args(si, argl,
            SC_STRING_I, &def,
            0);

    ret = SS_null;

    if (def != NULL)
       {memdes *desc;

        desc = _PD_mk_descriptor(def, 0);
        ret  = _SX_memdes_to_list(si, desc);
        _PD_rl_descriptor(desc);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_CLOSE_FILE - close a G_FILE and
 *                - remove the object from SX_gs.file_list
 */

static object *_SX_close_file(SS_psides *si, object *arg)
   {g_file *po, *prev;

    po = NULL;
    SS_args(si, arg,
            G_FILE, &po,
            0);

    if (po == NULL)
       return(SS_f);

/* remove from list */
    if (po == SX_gs.file_list)
       SX_gs.file_list = SX_gs.file_list->next;

/* find the link preceding this one */
    else
       {for (prev = SX_gs.file_list; prev != NULL; prev = prev->next)
            if (prev->next == po)
               break;

        if (prev != NULL)
           prev->next = po->next;};

    _SX_rel_open_file(si, po);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_CLOSE_PDBFILE - close a PDBFile and
 *                    - remove the object from SX_gs.file_list
 */

static object *_SXI_close_pdbfile(SS_psides *si, object *arg)
   {object *o;

    o = _SX_close_file(si, arg);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_LIST_FILE - return a list of open files */

static object *_SXI_list_file(SS_psides *si)
   {g_file *po;
    object *obj;

    obj = SS_null;
    for (po = SX_gs.file_list; po != NULL; po = po->next)
        obj = SS_mk_cons(si, po->file_object, obj);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_PDBFP - Scheme level version of SX_pdbfilep function */

static object *_SXI_pdbfp(SS_psides *si, object *obj)
   {object *o;

    o = SX_pdbfilep(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DEFP - function version of SX_DEFSTRP macro */

static object *_SXI_defp(SS_psides *si, object *obj)
   {object *o;

    o = SX_DEFSTRP(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_PDBDATAP - function version of SX_PDBDATAP macro */

static object *_SXI_pdbdatap(SS_psides *si, object *obj)
   {object *o;

    o = SX_PDBDATAP(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SYMP - function version of SX_SYMENTP macro */

static object *_SXI_symp(SS_psides *si, object *obj)
   {object *o;

    o = SX_SYMENTP(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_LIST_SYMTAB - return a list of the symbol table entries */

static object *_SXI_list_symtab(SS_psides *si, object *argl)
   {object *args, *obj;
    char *patt;
    g_file *po;
    PDBfile *file;

    po   = NULL;
    patt = NULL;
    SS_args(si, argl,
            G_FILE, &po,
            SC_STRING_I, &patt,
            0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    if (patt == NULL)
       args = SS_mk_cons(si,
			 SS_mk_hasharr(si, file->symtab),
                         SS_null);
    else
       args = SS_mk_cons(si,
			 SS_mk_hasharr(si, file->symtab),
                         SS_mk_cons(si,
				    SS_mk_string(si, patt),
                                    SS_null));

    obj = SS_hash_dump(si, args);
    
    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_LIST_VARIABLES - return a list of the variables in a file directory */

static object *_SXI_list_variables(SS_psides *si, object *argl)
   {object *obj, *oall;
    char *patt;
    char *type;
    char *flags;
    char **names;
    int i, num, all;
    long size;
    g_file *po;
    PDBfile *file;

    po    = NULL;
    patt  = NULL;
    type  = NULL;
    flags = NULL;
    size  = -1L;
    oall  = SS_f;
    SS_args(si, argl,
            G_FILE, &po,
            SC_STRING_I, &flags,
            SC_STRING_I, &patt,
            SC_STRING_I, &type,
            SC_LONG_I, &size,
            SS_OBJECT_I, &oall,
            0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    all = SS_true(oall);

    if ((flags != NULL) && (strcmp(flags, "nil") == 0))
       {CFREE(flags);
        flags = NULL;}

    obj = SS_null;

    names = _PD_ls_extr(file, patt, type, size, &num, all, flags);
    if (names != NULL)
       {for (i = num - 1; i >= 0; i--)
	    {SS_assign(si, obj,
		       SS_mk_cons(si, SS_mk_string(si, names[i]),
				  obj));};};

    CFREE(names);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_CHANGE_DIRECTORY - change the current working directory in a file */

static object *_SXI_change_directory(SS_psides *si, object *argl)
   {char *dir;
    g_file *po;
    PDBfile *file;
    object *o;

    po  = NULL;
    dir = NULL;
    SS_args(si, argl,
            G_FILE, &po,
            SC_STRING_I, &dir,
            0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    if (dir == NULL)
       dir = "/";

    if (!PD_cd(file, dir))
       SS_error(si, "BAD DIRECTORY NAME - _SXI_CHANGE_DIRECTORY", argl);

    o = SS_mk_string(si, dir);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_MAKE_DIRECTORY - make a directory in a file */

static object *_SXI_make_directory(SS_psides *si, object *argl)
   {char *dir;
    g_file *po;
    PDBfile *file;
    object *errf, *o;

    po   = NULL;
    dir  = NULL;
    errf = SS_t;
    SS_args(si, argl,
            G_FILE, &po,
            SC_STRING_I, &dir,
            SS_OBJECT_I, &errf,
            0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    if (!PD_mkdir(file, dir))
       {PD_gs.err[0] = '\0';
	if (SS_true(errf))
           SS_error(si, "UNABLE TO CREATE DIRECTORY - _SXI_MAKE_DIRECTORY",
		    argl);};

    o = SS_mk_string(si, dir);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_FILE_DIRP - return #t iff the given variable name/path refers or 
 *                - could possibly refer to a directory in the given file
 */

static object *_SXI_file_dirp(SS_psides *si, object *argl)
   {char *dir;
    g_file *po;
    PDBfile *file;
    object *o;

    po  = NULL;
    dir = NULL;
    SS_args(si, argl,
            G_FILE, &po,
            SC_STRING_I, &dir,
            0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    o = (PD_isdir(file, dir)) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_CURRENT_DIRECTORY - return the current working directory in a file */

static object *_SXI_current_directory(SS_psides *si, object *arg)
   {g_file *po;
    PDBfile *file;
    object *o;

    po      = NULL;
    SS_args(si, arg,
            G_FILE, &po,
            0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    o = SS_mk_string(si, PD_pwd(file));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_CREATE_LINK - create a link to a variable in a file */

static object *_SXI_create_link(SS_psides *si, object *argl)
   {char *oldname, *newname;
    g_file *po;
    PDBfile *file;
    object *o;

    po      = NULL;
    oldname = NULL;
    newname = NULL;
    SS_args(si, argl,
            G_FILE, &po,
            SC_STRING_I, &oldname,
            SC_STRING_I, &newname,
            0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    if (!PD_ln(file, oldname, newname))
       SS_error(si, "UNABLE TO CREATE LINK - _SXI_CREATE_LINK", argl);

    o = SS_mk_string(si, newname);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_LIST_DEFSTRS - return a list of the derived data types in a file */

static object *_SXI_list_defstrs(SS_psides *si, object *argl)
   {object *obj, *sort;
    g_file *po;
    PDBfile *strm;

    po   = NULL;
    sort = SS_t;
    SS_args(si, argl,
            G_FILE, &po,
	    SS_OBJECT_I, &sort,
            0);

    if ((po == NULL) || (po == SX_gs.gvif))
       strm = SX_gs.vif;
    else
       strm = FILE_FILE(PDBfile, po);

    obj = SS_make_list(si, SS_OBJECT_I, SS_mk_hasharr(si, strm->chart),
                       SS_OBJECT_I, SS_null,
                       SS_OBJECT_I, sort,
                       0);

    obj = SS_hash_dump(si, obj);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DEF_PRIM - Scheme version of PD_DEFIX, PD_DEFLOAT, PD_DEFNCV
 *               -
 *               - (def-primitive file name "ncv" bpi align)
 *               - (def-primitive file name "fix" bpi align flg)
 *               - (def-primitive file name "fp"  bpi align
 *               -                order-list expb mantb sbs sbe sbm hmb bias)
 */
 
static object *_SXI_def_prim(SS_psides *si, object *argl)
   {int n;
    char *name, *type;
    intb bpi;
    int align;
    PDBfile *file;
    defstr *dp;
    g_file *po;
    object *rv;

    dp    = NULL;
    po    = NULL;
    name  = NULL;
    type  = NULL;
    bpi   = 0L;
    align = 0;

    n = SS_args(si, argl,
                G_FILE, &po,
                SC_STRING_I, &name,
                SC_STRING_I, &type,
                SC_LONG_I, &bpi,
                SC_INT_I, &align,
                0);

    if ((po == NULL) || (po == SX_gs.gvif))
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    if (strcmp(type, "ncv") == 0)
       dp = PD_defncv(file, name, bpi, align);

    else if (strcmp(type, "fix") == 0)
       {PD_byte_order ord;
        object *rest;

        rest = SS_list_tail(si, argl, n);
        SS_args(si, rest,
                SC_ENUM_I, &ord,
                0);

        dp = PD_defix(file, name, bpi, align, ord);}

    else if (strcmp(type, "fp") == 0)
       {int *ordr;
        long i, no, expb, mantb, sbs, sbe, sbm, hmb, bias;
        object *rest, *ord;

        rest = SS_list_tail(si, argl, n);
        SS_args(si, rest,
                SS_OBJECT_I, &ord,
                SC_LONG_I, &expb,
                SC_LONG_I, &mantb,
                SC_LONG_I, &sbs,
                SC_LONG_I, &sbe,
                SC_LONG_I, &sbm,
                SC_LONG_I, &hmb,
                SC_LONG_I, &bias,
                0);

        if (SS_nullobjp(ord))
           SS_error(si, "BAD BYTE ORDERING - _SXI_DEF_PRIM", ord);

        no = SS_length(si, ord);
        if (no != bpi)
           SS_error(si, "INCONSISTENT SIZE - _SXI_DEF_PRIM", ord);

        ordr = CMAKE_N(int, no);
        for (i = 0L; i < no; i++, ord = SS_cdr(si, ord))
            ordr[i] = SS_INTEGER_VALUE(SS_car(si, ord));

        dp = PD_defloat(file, name, bpi, align, ordr,
                        expb, mantb, sbs, sbe, sbm, hmb, bias);};

    rv = _SX_mk_gdefstr(si, dp);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_CHG_PRIM - Scheme version of PD_CHANGE_PRIMITIVE
 *               -
 *               - (change-primitive file ityp nb align 
 *               -                        order-list
 *               -                        expb mantb sbs sbe sbm hmb bias)
 */
 
static object *_SXI_chg_prim(SS_psides *si, object *argl)
   {int i, id, ityp, nb, no, align;
    int *ordr;
    long fmt[7];
    PDBfile *file;
    g_file *po;
    object *ord, *rv;

    po    = NULL;
    ord   = NULL;
    align = 0;

    SS_args(si, argl,
	    G_FILE, &po,
	    SC_INT_I, &ityp,
	    SC_INT_I, &nb,
	    SC_INT_I, &align,
	    SS_OBJECT_I, &ord,
	    SC_LONG_I, fmt + 0,
	    SC_LONG_I, fmt + 1,
	    SC_LONG_I, fmt + 2,
	    SC_LONG_I, fmt + 3,
	    SC_LONG_I, fmt + 4,
	    SC_LONG_I, fmt + 5,
	    SC_LONG_I, fmt + 6,
	    0);

    if ((po == NULL) || (po == SX_gs.gvif))
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    if (SS_nullobjp(ord))
       SS_error(si, "BAD BYTE ORDERING - _SXI_CHG_PRIM", ord);

    no = SS_length(si, ord);
    if (no != nb)
       SS_error(si, "INCONSISTENT SIZE - _SXI_CHG_PRIM", ord);

    ordr = CMAKE_N(int, no);
    for (i = 0L; i < no; i++, ord = SS_cdr(si, ord))
        ordr[i] = SS_INTEGER_VALUE(SS_car(si, ord));

    id = PD_change_primitive(file, ityp, nb, align, fmt, ordr);

    rv = SS_mk_integer(si, id);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_READ_DEFSTR - read a defstr from a file
 *                  - Usage: (read-defstr file name)
 */
 
static object *_SXI_read_defstr(SS_psides *si, object *argl)
   {char *name;
    g_file *po;
    PDBfile *file;
    defstr *dp;
    object *o;

    po   = NULL;
    name = NULL;
    SS_args(si, argl,
            G_FILE, &po,
            SC_STRING_I, &name,
            0);

    if ((po == NULL) || (po == SX_gs.gvif))
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    dp = PD_inquire_type(file, name);
    if (dp == NULL)
       SS_error(si, "BAD TYPE - _SXI_READ_DEFSTR", argl);

    o = _SX_mk_gdefstr(si, dp);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_WRITE_DEFSTR - write a defstr to a file
 *                   - Usage: (write-defstr file defstr)
 */
 
static object *_SXI_write_defstr(SS_psides *si, object *argl)
   {g_file *po;
    PDBfile *file;
    memdes *desc;
    defstr *dp;

    po = NULL;
    dp = NULL;
    SS_args(si, argl,
            G_FILE, &po,
            G_DEFSTR, &dp,
            0);

    if ((po == NULL) || (po == SX_gs.gvif))
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    desc = PD_copy_members(dp->members);

    if (desc == NULL)
       {if (PD_inquire_table_type(file->chart, dp->type) == NULL)
           _PD_defstr(file, PD_CHART_FILE, dp->type, dp->kind,
		      desc, NULL,
		      dp->size, dp->alignment,
		      dp->fix.order, dp->convert, dp->fp.order, dp->fp.format, 
		      dp->unsgned, dp->onescmp);

        if (PD_inquire_table_type(file->host_chart, dp->type) == NULL)
           _PD_defstr(file, PD_CHART_HOST, dp->type, dp->kind,
		      desc, NULL,
		      dp->size, dp->alignment,
		      dp->fix.order, dp->convert, dp->fp.order, dp->fp.format, 
		      dp->unsgned, dp->onescmp);}

    else
       _PD_defstr_inst(file, dp->type, dp->kind, desc,
		       NO_ORDER, NULL, NULL, PD_CHART_FILE);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_MAKE_DEFSTR - Create a new derived type
 *                  - Usage: (make-defstr file name [member(s)]+)
 *                  -
 *                  - member = (type name dimension(s))
 */
 
static object *_SXI_make_defstr(SS_psides *si, object *argl)
   {int nc, nr;
    char member[MAXLINE];
    char *name, *mname, *type, *t;
    memdes *desc, *lst, *prev;
    dimdes *dims, *dim0;
    object *member_obj, *dim_obj, *o;
    g_file *po;
    PDBfile *file;
    defstr *dp;

    po   = NULL;
    name = NULL;
    SS_args(si, argl,
            G_FILE, &po,
            SC_STRING_I, &name,
            0);

    if ((po == NULL) || (po == SX_gs.gvif))
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    argl = SS_cddr(si, argl);
    lst  = NULL;
    for (; !SS_nullobjp(argl); argl = SS_cdr(si, argl))
        {member_obj = SS_car(si, argl);
	 if (!SS_consp(member_obj))
	    SS_error(si, "MEMBER MUST BE A LIST - _SXI_MAKE_DEFSTR",
		     member_obj);

         mname = NULL;
         type  = NULL;
         SS_args(si, member_obj,
                 SC_STRING_I, &type,
                 SC_STRING_I, &mname,
                 0);

	 snprintf(member, MAXLINE, "%s %s", type, mname);
	 dim_obj = SS_cddr(si, member_obj);
	 if (!SS_nullobjp(dim_obj))
	    {dims = _SX_make_dims_dimdes(si, file, dim_obj);
	     nc = strlen(member);
	     t  = member + nc;
	     nr = MAXLINE - nc;
	     *t++ = '(';
	     for (; dims != NULL; dims = dim0)
	         {if (dims->index_min == file->default_offset)
		     {nc = snprintf(t, nr, "%ld", dims->number);
		      t  += nc;
		      nr -= nc;}
		  else
		     {nc = snprintf(t, nr, "%ld:%ld",
				    dims->index_min, dims->index_max);
		      t  += nc;
		      nr -= nc;};

		  dim0 = dims->next;
		  if (dim0 != NULL)
		     *t++ = ',';
		  CFREE(dims);};
	     *t++ = ')';
	     *t = '\0';};
 
	 desc = _PD_mk_descriptor(member, file->default_offset);

	 if (lst == NULL)
	    lst = desc;
	 else
	    prev->next = desc;
	 prev = desc;};

    dp = _PD_defstr_inst(file, name, STRUCT_KIND, lst,
			 NO_ORDER, NULL, NULL, PD_CHART_HOST);
    o  = _SX_mk_gdefstr(si, dp);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_MAKE_TYPEDEF - Scheme version of PD_TYPEDEF
 *                   -
 *                   - (make-typedef file old new)
 */
 
static object *_SXI_make_typedef(SS_psides *si, object *argl)
   {char *ntype, *otype;
    PDBfile *file;
    g_file *po;
    object *o;

    po    = NULL;
    ntype = NULL;
    otype = NULL;
    SS_args(si, argl,
            G_FILE, &po,
            SC_STRING_I, &otype,
            SC_STRING_I, &ntype,
            0);

    if ((po == NULL) || (po == SX_gs.gvif))
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    o = _SX_mk_gdefstr(si, PD_typedef(file, otype, ntype));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_MAKE_CAST - Scheme version of PD_CAST
 *                -
 *                - (make-cast file type member controller)
 */
 
static object *_SXI_make_cast(SS_psides *si, object *argl)
   {char *type, *memb, *contr;
    PDBfile *file;
    g_file *po;
    object *o;

    po   = NULL;
    type  = NULL;
    memb  = NULL;
    contr = NULL;
    SS_args(si, argl,
            G_FILE, &po,
            SC_STRING_I, &type,
            SC_STRING_I, &memb,
            SC_STRING_I, &contr,
            0);

    if ((po == NULL) || (po == SX_gs.gvif))
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    o = PD_cast(file, type, memb, contr) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_FILE_VARP - return #t iff the given variable name/path refers or 
 *                - could possibly refer to a variable in the given file
 */

static object *_SXI_file_varp(SS_psides *si, object *argl)
   {int flag, ret;
    char *name;
    object *fobj, *o;
    PDBfile *file;
    g_file *po;

    po   = NULL;
    name = NULL;
    fobj = SS_f;
    SS_args(si, argl,
            G_FILE, &po,
            SC_STRING_I, &name,
            SS_OBJECT_I, &fobj,
            0);

    if ((po == NULL) || (po == SX_gs.gvif))
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    if (file == NULL)
       ret = FALSE;

    else

/* check for the flag */
       {flag = (fobj != SS_f);

	 ret = _SX_file_varp(file, name, flag);
	 CFREE(name);};

    o = ret ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_FILE_VARP - return TRUE iff the named variable is in the file */

int _SX_file_varp(PDBfile *file, char *name, int flag)
   {int rv;
    char *token;
    syment *ep;

    ep = NULL;

    if (name != NULL)
       {if (flag)
	   {ep = _PD_effective_ep(file, name, TRUE, NULL);
	    if (ep != NULL)
	       _PD_rl_syment_d(ep);}

        else

/* check the literal name first - e.g. {t=1!000e-01,x(3:7)} */
	   {ep = PD_inquire_entry(file, name, TRUE, NULL);
	    if (ep == NULL)
	       {token = SC_firsttok(name, ".([ ");

		_SC_udl_container(token, TRUE);

		ep = PD_inquire_entry(file, token, TRUE, NULL);};};};

    rv = (ep != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_RD_SYMENT - read a syment from the given file
 *                -
 *                - (read-syment <file> <name> [<flag>])
 *                -
 *                - This is a low level function not intended for the
 *                - general user. A syment is also created and written
 *                - when a variable is written using write-pdbdata.
 *                -
 *                - if the file is not specified, the internal virtual
 *                - file is used
 *                - the syment is looked up
 *                -
 *                - if flag is false (default) and syment for name is
 *                - not found, an error call results. if flag is true
 *                - a null object pointer is returned.
 */

static object *_SXI_rd_syment(SS_psides *si, object *argl)
   {char *name, *s;
    g_file *po;
    PDBfile *file;
    syment *ep;
    dimdes *dp;
    object *err, *o;

    po   = NULL;
    name = NULL;
    err  = SS_f;
    SS_args(si, argl,
            G_FILE, &po,
            SC_STRING_I, &name,
            SS_OBJECT_I, &err,
            0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    s = name;
    name = _PD_expand_hyper_name(file, s);
    CFREE(s);
    if (name == NULL)
       return(SS_null);

    ep = _PD_effective_ep(file, name, TRUE, NULL);
    if (ep == NULL)
       {CFREE(name);
	if (SS_true(err))
	   return(SS_null);
	else
	   SS_error(si, PD_get_error(), SS_cadr(si, argl));}

    else if (SC_LAST_CHAR(name) == ']')
       {dp = PD_entry_dimensions(ep);
        ep->dimensions = _PD_hyper_dims(file, name, dp);

/* GOTCHA: this set of dimensions may still be valid for the original ep
 *         they weren't copied - its certainly TRUE in cases with VIF's
	_PD_rl_dimensions(dp);
 */
        };

/* GOTCHA: the syment EP will never be GC'd as the coding stands
 *         the error case above does not count!
 */

    o = _SX_mk_gsyment(si, ep);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MAKE_BLOCKS - make a block list from the given disk address
 *                 - information
 */

static SC_array *_SX_make_blocks(SS_psides *si, object *alst, long numb)
   {int i, n;
    long long addr, ni, tot;
    SC_array *bl;

    bl = NULL;

    if (SS_consp(alst))
       {n   = SS_length(si, alst) >> 1;
	bl  = _PD_block_make(n);
	ni  = 0;
        tot = 0L;
        for (i = 0; i < n; i++)
            {SS_args(si, alst,
                     SC_LONG_LONG_I, &addr,
                     SC_LONG_LONG_I, &ni,
                     0);
	     _PD_block_set_desc(addr, ni, bl, i);

             tot += ni;

             alst = SS_cddr(si, alst);};

        if (tot != numb)
           SS_error(si,
		      "INCONSISTENT DISCONTIGUOUS ENTRY - _SX_MAKE_BLOCKS",
		      alst);}

    else
       {bl = _PD_block_make(1);

	SS_args(si, alst,
		SC_LONG_LONG_I, &addr,
		0);

	_PD_block_set_desc(addr, 1, bl, 0);};

    return(bl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_WR_SYMENT - install a syment
 *                -
 *                - (write-syment <file> <name> <type> <diskaddr> [<dimensions>])
 *                -
 *                - This is a low level function not intended for the
 *                - general user. A syment is also created and written
 *                - when a variable is written using write-pdbdata.
 *                -
 *                - If the file is not specified, the internal virtual
 *                - file is used. A syment is created.
 *                - The number of items is computed from the dimension.
 */

static object *_SXI_wr_syment(SS_psides *si, object *argl)
   {char *name, *type;
    char s[MAXLINE];
    long n;
    g_file *po;
    PDBfile *file;
    syment *ep;
    dimdes *dims;
    object *alst, *o;
    SC_array *bl;

    po   = NULL;
    name = NULL;
    type = NULL;
    alst = SS_null;
    SS_args(si, argl,
            G_FILE, &po,
            SC_STRING_I, &name,
            SC_STRING_I, &type,
            SS_OBJECT_I, &alst,
            0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

/* get optional dimensions */
    argl = SS_cddr(si, SS_cddr(si, argl));

    dims = _SX_make_dims_dimdes(si, file, argl);
    n    = _PD_comp_num(dims);
    bl   = _SX_make_blocks(si, alst, n);
    ep   = _PD_mk_syment(type, n, 0L, NULL, dims);
    strcpy(s, _PD_fixname(file, name));

    _PD_e_install(file, s, ep, TRUE);

    _PD_block_switch(ep, bl);

    o = _SX_mk_gsyment(si, ep);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_PSEUDO_SUITABLE - is the specified entry suitable for writing to
 *                     - a "psuedo" file (i.e. a buffer) or only with
 *                     - the more restricted VIF
 *                     - return TRUE iff suitable for "pseudo" file
 */

static int _SX_pseudo_suitable(PDBfile *file, syment *et)
   {int ok;
    char *type;
    defstr *dp;
    memdes *dm;

    type = PD_entry_type(et);

    ok = !_PD_indirection(type);
    dp = _PD_type_lookup(file, PD_CHART_HOST, type);
    if (dp != NULL)
       {for (dm = dp->members; (dm != NULL) && (ok == TRUE); dm = dm->next)
	    ok &= !_PD_indirection(dm->type);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_SETUP_FILEDATA_VIF - do VIF appropriate preparations prior
 *                        - to the actual data write
 */

static void _SX_setup_filedata_vif(PDBfile **pfp, int *pwrfl,
				   char *fullpath, syment *ep)
   {char bf[MAXLINE];
    char *t;
    PDBfile *file, *fp;
    syment *et, *eo;
    SC_address b;

    file = *pfp;

    et = _PD_effective_ep(file, fullpath, FALSE, NULL);
    if (et == NULL)
       {_PD_e_install(file, fullpath, ep, TRUE);
	*pwrfl = FALSE;}

    else if (!_SX_pseudo_suitable(file, et))
       {_PD_rl_syment(et);
	*pwrfl = FALSE;}

    else
       {_PD_rl_syment(et);

	strcpy(bf, fullpath);
	t  = SC_firsttok(bf, "[]() ");
	eo = PD_inquire_entry(file, t, TRUE, NULL);

	b.diskaddr = PD_entry_address(eo);

	fp = PN_open(file, b.memaddr);
	if (fp != NULL)
	   {et = _PD_mk_syment(PD_entry_type(eo),
			       PD_entry_number(eo),
			       b.mdiskaddr,
			       NULL,
			       PD_entry_dimensions(eo));

	    _PD_e_install(fp, t, et, TRUE);};

        *pfp = fp;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_READ_ENTRY - read entry method for PDB spoke */

int _SX_read_entry(PDBfile *fp, char *path, char *ty, syment *ep, void *vr)
   {int rv;

    rv = _PD_hyper_read(fp, path, ty, ep, vr);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_WRITE_ENTRY - write entry method for PDB spoke */

syment *_SX_write_entry(PDBfile *fp, char *path, char *inty, char *outty,
			void *vr, dimdes *dims)
   {int new;
    syment *ep;

    ep = fp->tr->write(fp, path, inty, outty, vr, dims, FALSE, &new);

    if ((ep != NULL) && (new == TRUE))
       ep = PD_copy_syment(ep);

    return(ep);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_WRITE_FILEDATA - write a pdbdata object from a PDB file obtained by
 *                    - the function passed in
 *                    -
 *                    - Syntax:
 *                    -    (write-filedata file name (type dimension) data)
 *                    - or
 *                    -    (write-filedata file name pdbdata)
 *                    -
 *                    - this routine will write a pdbdata object
 *                    - if file is missing, the internal virtual file will
 *                    - be used
 */

static object *_SX_write_filedata(SS_psides *si, object *argl)
   {int wrfl, new;
    long number;
    char *type, *ntype, *vrin;
    char fullpath[MAXLINE];
    PDBfile *file, *fp;
    syment *ep;
    dimdes *dims;
    defstr *dp, *ndp;
    SC_address addr;
    g_file *po;
    PFBinWrite wr;
    object *namo, *symo, *rv;
    
    if (!SS_consp(argl))
       SS_error(si, "BAD ARGUMENT LIST - SX_WRITE_FILEDATA", argl);

    argl = SX_get_pdbfile(si, argl, &file, &po);

    wr = file->tr->write;

/* get the name of the variable */
    namo = SS_car(si, argl);
    strcpy(fullpath, _PD_fixname(file, SS_get_string(namo)));

    argl = SS_cdr(si, argl);

/* if no other arguments its an error */
    if (SS_nullobjp(argl))
       SS_error(si, "INSUFFICIENT ARGUMENTS - SX_WRITE_FILEDATA", argl);

/* if the next item is a pdbdata, use its info */
    symo = SS_car(si, argl);
    if (SX_PDBDATAP(symo))
       {SS_mark(symo);
        ep           = PDBDATA_EP(symo);
        addr.memaddr = (char *) PDBDATA_DATA(symo);
        type         = CSTRSAVE(PD_entry_type(ep));
        number       = PD_entry_number(ep);
        dims         = PD_copy_dims(PD_entry_dimensions(ep));
        if (file->virtual_internal == TRUE)
           {vrin         = addr.memaddr;
            addr.memaddr = _PD_alloc_entry(file, type, number);
            _SX_copy_tree(si, file, vrin, addr.memaddr, number, type);
            ep = _PD_mk_syment(type, number, addr.diskaddr, NULL, dims);
            SC_mark(addr.memaddr, 1);
	    _PD_e_install(file, fullpath, ep, TRUE);}
        else
           {dp    = PD_inquire_host_type(file, type);
	    ndp   = _PD_type_container(file, dp);
	    ntype = (ndp == NULL) ? type : ndp->type;
	    ep    = wr(file, fullpath, ntype, ntype, addr.memaddr, dims,
		       FALSE, &new);
            if (ep == NULL)
	       {CFREE(type);
		CFREE(dims);
		SS_error(si, PD_get_error(), namo);};};}
        
/* otherwise the next thing should be a cons */
    else
       {ep = _SX_spec_instance(si, file, FALSE, argl);

	addr.diskaddr = PD_entry_address(ep);
        if (addr.memaddr == NULL)
           SS_error(si, "UNKNOWN DATA - SX_WRITE_FILEDATA", argl);

/* "write" to correct file */
	wrfl = TRUE;
	fp   = file;
        if (file->virtual_internal == TRUE)
	   _SX_setup_filedata_vif(&fp, &wrfl, fullpath, ep);

	if ((fp != NULL) && (wrfl == TRUE))
           {if (fp->mode == PD_OPEN)
	       SS_error(si, "FILE OPENED READ-ONLY - SX_WRITE_FILEDATA",
			SS_null);

            ep = wr(fp, fullpath, PD_entry_type(ep), PD_entry_type(ep),
		    addr.memaddr, PD_entry_dimensions(ep),
		    FALSE, &new);
            if (ep == NULL)
	       SS_error(si, PD_get_error(), namo);};

	if ((fp != NULL) && (fp != file))
	   PN_close(fp);};

    rv = _SX_mk_gpdbdata(si, fullpath, addr.memaddr, ep, file);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_WRITE_PDBDATA - write a pdbdata object
 *                    -
 *                    - Syntax:
 *                    -    (write-pdbdata file name (type dimension) data)
 *                    - or
 *                    -    (write-pdbdata file name pdbdata)
 *                    -
 *                    - This routine will write a pdbdata object
 *                    - If file is missing, the internal virtual file will
 *                    - be used.
 */

static object *_SXI_write_pdbdata(SS_psides *si, object *argl)
   {object *rv;

    rv = _SX_write_filedata(si, argl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_SCATTER_FILEDATA - scatter data to a pdbdata object from a PDB file
 *                     - obtained by the function passed in
 *                     - Syntax:
 *                     -    (scatter-filedata file name (type dimension)
 *                     -                      indl data)
 *                     - this routine will write a pdbdata object
 *                     - if file is missing, the internal virtual file will
 *                     - be used
 */

static object *SX_scatter_filedata(object *argl)
   {object *rv;

/* NOTE: write this function
 * it should be similar to SX_write_filedata but without the pdbdata
 * case and using PD_scatter_as instead of file->tr->write
 */
    rv = SS_f;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SCATTER_PDBDATA - scatter data to a pdbdata object
 *                      -
 *                      - Syntax:
 *                      -    (scatter-pdbdata file name (type dimension)
 *                      -                     indl data)
 *                      - This routine will write a pdbdata object
 *                      - If file is missing, the internal virtual file will
 *                      - be used.
 */

static object *_SXI_scatter_pdbdata(SS_psides *si, object *argl)
   {object *rv;

    rv = SX_scatter_filedata(argl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_REMOVE_ENTRY - remove an entry from a file
 *                   -
 *                   - Syntax:
 *                   -    (remove-entry file name)
 */

static object *_SXI_remove_entry(SS_psides *si, object *argl)
   {char *name;
    PDBfile *file;
    g_file *po;
    object *rv;

    rv   = SS_f;
    po   = NULL;
    name = NULL;
    file = NULL;
    SS_args(si, argl,
            G_FILE, &po,
	    SC_STRING_I, &name,
	    0);

    if ((po == NULL) || (po == SX_gs.gvif))
       file = SX_gs.vif;

    else if (strcmp(po->type, PDBFILE_S) == 0)
       file = FILE_FILE(PDBfile, po);

    else
       SS_error(si, "BAD FILE - _SXI_REMOVE_ENTRY", argl);

    if (file != NULL)
       rv = (PD_remove_entry(file, name) == TRUE) ? SS_t : SS_f;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_RESERVE_PDBDATA - defent a pdbdata object to a PDB file obtained by
 *                      - the function passed in.
 *                      -
 *                      - Syntax:
 *                      -    (reserve-pdbdata file name (type dimension))
 *                      - or
 *                      -    (reserve-pdbdata file name pdbdata)
 *                      -
 *                      - This routine will defent a pdbdata object
 *                      - If file is missing, the internal virtual file will
 *                      - be used.
 */

static object *_SXI_reserve_pdbdata(SS_psides *si, object *argl)
   {int wrfl;
    long number;
    char *type, *ntype;
    char fullpath[MAXLINE];
    PDBfile *file, *fp;
    syment *ep, *et;
    dimdes *dims;
    defstr *dp, *ndp;
    SC_address addr;
    g_file *po;
    object *namo, *symo, *o;
    
    if (!SS_consp(argl))
       SS_error(si, "BAD ARGUMENT LIST - _SXI_RESERVE_PDBDATA", argl);

    argl = SX_get_pdbfile(si, argl, &file, &po);

/* get the name of the variable */
    namo = SS_car(si, argl);
    strcpy(fullpath, _PD_fixname(file, SS_get_string(namo)));
    argl     = SS_cdr(si, argl);

/* if no other arguments its an error */
    if (SS_nullobjp(argl))
       SS_error(si, "INSUFFICIENT ARGUMENTS - _SXI_RESERVE_PDBDATA", argl);

/* if the next item is a pdbdata, use its info */
    symo = SS_car(si, argl);
    if (SX_PDBDATAP(symo))
       {SS_mark(symo);
        ep           = PDBDATA_EP(symo);
        addr.memaddr = (char *) PDBDATA_DATA(symo);
        type         = CSTRSAVE(PD_entry_type(ep));
        number       = PD_entry_number(ep);
        dims         = PD_copy_dims(PD_entry_dimensions(ep));

	dp    = PD_inquire_host_type(file, type);
	ndp   = _PD_type_container(file, dp);
	ntype = (ndp == NULL) ? type : ndp->type;
	ep    = _PD_defent(file, fullpath, ntype, number, dims);
	if (ep == NULL)
	   {CFREE(type);
	    CFREE(dims);
	    SS_error(si, PD_get_error(), namo);};

	ep = PD_copy_syment(ep);}
        
/* otherwise the next thing should be a cons */
    else
       {ep            = _SX_spec_instance(si, file, TRUE, argl);
	number        = PD_entry_number(ep);
	addr.diskaddr = 0L;

/* "defent" to correct file */
	wrfl = TRUE;
	fp   = file;
        if (file->virtual_internal == TRUE)
	   {et = _PD_effective_ep(file, fullpath, FALSE, NULL);
	    if (et == NULL)
	       {_PD_e_install(file, fullpath, ep, TRUE);
		wrfl = FALSE;}

	    else if (!_SX_pseudo_suitable(file, et))
	       {_PD_rl_syment(et);
		wrfl = FALSE;}

	    else
	       SS_error(si, "NOT APPROPRIATE - _SXI_RESERVE_PDBDATA", SS_null);};

	if (wrfl)
           {if (fp->mode == PD_OPEN)
	       SS_error(si, "FILE OPENED READ-ONLY - _SXI_RESERVE_PDBDATA", SS_null);

            ep = _PD_defent(fp, fullpath, PD_entry_type(ep),
                            number, PD_entry_dimensions(ep));
            if (ep == NULL)
	       SS_error(si, PD_get_error(), namo);

	    ep = PD_copy_syment(ep);};

	if (fp != file)
	   PN_close(fp);};

    o = _SX_mk_gpdbdata(si, fullpath, addr.memaddr, ep, file);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_READ_FILEDATA - read a pdbdata object from a PDB file obtained by
 *                   - the function passed in
 *                   - Syntax: (read-filedata file name)
 *                   -
 *                   - this routine will make a pdbdata object
 *                   - if FILE is nil, the internal virtual file will be used.
 *                   - if there is no entry NAME return SS_null
 */

static object *_SX_read_filedata(SS_psides *si, object *argl)
   {int n, err, ie, valid;
    long number;
    char *name, *s, *type;
    PDBfile *file;
    PD_smp_state *pa;
    syment *ep, *tep;
    SC_address addr;
    dimdes *dims, *dimscheck;
    PFBinRead rd;
    g_file *po;
    object *namo, *o;

    pa = _PD_get_state(-1);

    if (!SS_consp(argl))
       SS_error(si, "BAD ARGUMENT LIST - SX_READ_FILEDATA", argl);

    argl = SX_get_pdbfile(si, argl, &file, &po);

    rd = file->tr->read;

/* get the name of the variable */
    namo = SS_car(si, argl);
    name = CSTRSAVE(_PD_fixname(file, SS_get_string(namo)));
    s    = name;
    name = _PD_expand_hyper_name(file, s);
    CFREE(s);
    if (name == NULL)
       SS_error(si, "BAD SUBSCRIPT EXPRESSION - SX_READ_FILEDATA", namo);

    ep = _PD_effective_ep(file, name, FALSE, NULL);

    if (ep == NULL)
       {CFREE(name);
	return(SS_null);};
  
/* make sure the dimension specification is valid */
    dimscheck = _PD_ex_dims(name, file->default_offset, &ie);  
    if (dimscheck != NULL)
       {valid = _PD_valid_dims(dimscheck, PD_entry_dimensions(ep));
	_PD_rl_dimensions(dimscheck);

	if (!valid)
	   SS_error(si, "Invalid dimension specifications - SX_READ_FILEDATA",
		    namo);};

    type   = PD_entry_type(ep);
    number = PD_entry_number(ep);

    if (file == SX_gs.vif)

/* GOTCHA: this protects against problems with pointers into the middle
 *         of SCORE allocated arrays - which are fierce
 *         at the cost of being unable to edit memory with PDBView
 */
       {SC_address ad;

	if (!_PD_indirection(type))
           {addr.memaddr = _PD_alloc_entry(file, type, number);
	    ad.diskaddr  = PD_entry_address(ep);
	    memcpy(addr.memaddr,
		   ad.memaddr,
		   SC_arrlen(addr.memaddr));}
        else
	   addr.diskaddr = PD_entry_address(ep);

/* GOTCHA: until PDB is made to do the reference counting in detail
 *         the best we can do is to assume that things with NO
 *         reference counts are incorrect and have at least 1 reference
 */
        if (SC_ref_count(addr.memaddr) < 1)
	   SC_mark(addr.memaddr, 1);}

    else
       {switch (SETJMP(pa->read_err))
           {case ABORT :
                 SS_error(si, "_PD_HYPER_READ FAILED - SX_READ_FILEDATA",
                          namo);
            default :
                 memset(PD_gs.err, 0, MAXLINE);
                 break;};

        addr.memaddr = _PD_alloc_entry(file, type, number);

	err = rd(file, name, type, ep, addr.memaddr, -1, NULL);
        if (!err)
           SS_error(si, "PROBLEMS READING DATA - SX_READ_FILEDATA", namo);};

    if (SC_LAST_CHAR(name) == ']')
       {type = _PD_hyper_type(name, type);
	dims = _PD_hyper_dims(file, name, PD_entry_dimensions(ep));
	tep  = _PD_mk_syment(type, number, addr.diskaddr, NULL, dims);
	_PD_rl_syment_d(ep);}
    else
       tep = ep;

    if ((strcmp(type, SC_CHAR_S) == 0) && (_SX.string_mode != LITERAL))
       {s = (char *) addr.memaddr;
	if (s != NULL)
	   {for (n = number - 1; n >= 0; n--)
	        {if (SC_is_print_char(s[n], 4))
		    {s[n+1] = '\0';
		     break;};};
	    if (n < 0)
	       s[0] = '\0';};};

    o = _SX_mk_gpdbdata(si, name, addr.memaddr, tep, file);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_READ_PDBDATA - read a pdbdata object
 *                   -
 *                   - Syntax: (read-pdbdata file name)
 *                   -
 *                   - This routine will make a pdbdata object
 *                   - If file is nil, the internal virtual file will be used.
 */

static object *_SXI_read_pdbdata(SS_psides *si, object *argl)
   {object *o;

    o = _SX_read_filedata(si, argl);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_WRT_ULTRA_CURVE - write an ULTRA curve specified by some numeric
 *                      - arrays
 */

static object *_SXI_wrt_ultra_curve(SS_psides *si, object *argl)
   {int npts;
    PDBfile *file;
    g_file *po;
    C_array *xarr, *yarr;
    double *xval, *yval;
    char *labl;

    po   = NULL;
    file = NULL;
    labl = NULL;
    npts = 0;
    xarr = NULL;
    yarr = NULL;
    SS_args(si, argl,
            G_FILE, &po,
            G_NUM_ARRAY, &xarr,
            G_NUM_ARRAY, &yarr,
            SC_STRING_I, &labl,
            SC_INT_I, &npts,
            0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    xval = (double *) xarr->data;
    yval = (double *) yarr->data;
    PD_wrt_pdb_curve(file, labl, npts, xval, yval, _SX.iwcu++);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SIZEOF - return the size in bytes of the given type name */

static object *_SXI_sizeof(SS_psides *si, object *argl)
   {int flg;
    intb bpi;
    char *type;
    g_file *po;
    PDBfile *file;
    object *o;

    po   = NULL;
    file = NULL;
    type = NULL;
    flg  = FALSE;
    SS_args(si, argl,
            SC_STRING_I, &type,
            G_FILE, &po,
            SC_INT_I, &flg,
            0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    if (flg)
       bpi = _PD_lookup_size(type, file->host_chart);
    else
       bpi = _PD_lookup_size(type, file->chart);

    o = SS_mk_integer(si, bpi);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_WRITE_PDB - display/write content of a pdb object */

static object *_SX_write_pdb(SS_psides *si, FILE *f0, object *argl)
   {int n, sid;
    char typ[MAXLINE];
    long *ind;
    void *d;
    g_pdbdata *pp;
    C_array *iarr;
    object *obj;

    if (SS_consp(argl))
       {obj  = SS_car(si, argl);
	argl = SS_cdr(si, argl);}
    else
       {obj  = argl;
	argl = SS_null;};

    switch (SS_OBJECT_TYPE(obj))
       {default     :
	case G_FILE :
             if (SX_ipdbfilep(obj))
                PD_write_extras(f0, FILE_STREAM(PDBfile, obj));
	     else
	        PD_write_extras(f0, SX_gs.vif);
             break;

        case G_DEFSTR :
             PD_write_defstr(f0, SS_GET(defstr, obj));
             break;

        case G_SYMENT :
             PD_write_syment(f0, SS_GET(syment, obj));
             break;

        case G_PDBDATA :
             pp   = SS_GET(g_pdbdata, obj);
	     iarr = NULL;
	     SS_args(si, argl,
		     SC_INT_I, &_SC.types.max_digits,
		     G_NUM_ARRAY, &iarr,
		     0);

	     n   = 0;
	     ind = NULL;
	     if (iarr != NULL)
	        {PM_ARRAY_CONTENTS(iarr, void, n, typ, d);
		 sid = SC_type_id(typ, FALSE);
		 ind = SC_convert_id(SC_LONG_I, NULL, 0, 1,
				     sid, d, 0, 1, n, FALSE);};

             PD_write_entry(f0,
                            pp->file, pp->name,
                            pp->data, pp->ep,
			    n, ind);
             break;};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_PRINT_PDB - print contents of a pdb object */

static object *_SX_print_pdb(SS_psides *si, FILE *fp, object *argl)
   {object *x, *rv;

    rv = SS_null;

    for (; SS_consp(argl); argl = SS_cdr(si, argl))
        {x  = SS_car(si, argl);
	 rv = _SX_write_pdb(si, fp, x);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_PRINT_PDB - print contents of a pdb object */

static object *_SXI_print_pdb(SS_psides *si, object *argl)
   {object *rv;
    FILE *fp;

    fp = NULL;
    SS_args(si, argl,
	    SS_OUTPUT_PORT_I, &fp,
	    0);
    argl = SS_cdr(si, argl);

    if (fp == NULL)
       fp = stdout;

    rv = _SX_print_pdb(si, fp, argl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SHOW_PDB - display content of a pdb object */

static object *_SXI_show_pdb(SS_psides *si, object *argl)
   {object *rv;

    rv = _SX_print_pdb(si, stdout, argl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_TO_PDBDATA - return a pdbdata object from the arguments */

static object *_SXI_to_pdbdata(SS_psides *si, object *argl)
   {SC_address val;
    syment *ep;
    object *rv;

    ep = _SX_spec_instance(si, SX_gs.vif, FALSE, argl);

    val.diskaddr = PD_entry_address(ep);

    rv = _SX_mk_gpdbdata(si, "none", val.memaddr, ep, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DESC_VARIABLE - print out the syment in a object in human
 *                    - readable form.
 */

static object *_SXI_desc_variable(SS_psides *si, object *obj)
   {object *rv;

    if (!SX_PDBDATAP(obj))
       rv = SS_null;

    else
       {PD_write_syment(stdout, PDBDATA_EP(obj));
	rv = obj;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MAKE_DIMS_OBJ - take an object and make a list of integers */

object *_SX_make_dims_obj(SS_psides *si, dimdes *dims)
   {object *obj, *o;

/* if not dimensions, return nothing */
    if (dims == NULL)
       o = SS_null;

    else
       {for (obj = SS_null; dims != NULL; dims = dims->next)
	    obj = SS_mk_cons(si,
			     SS_mk_cons(si,
					SS_mk_integer(si, dims->index_min), 
					SS_mk_integer(si, dims->index_max)),
			     obj);

	o = SS_reverse(si, obj);};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_PDBDATA_TO_HASH - convert a pdbdata to a hash table.
 *                      - (pdbdata->hash data)
 */

static object *_SXI_pdbdata_to_hash(SS_psides *si, object *arg)
   {object *o;

    if (!SX_PDBDATAP(arg))
       o = SS_null;
    else
       o = SS_mk_hasharr(si, PDBDATA_DATA(arg));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_HASH_TO_PDBDATA - convert a hash table to a pdbdata object. 
 *                      - (hash->pdbdata hash-table file name)
 */

static object *_SXI_hash_to_pdbdata(SS_psides *si, object *argl)
   {char *name;
    hasharr *tab;
    g_file *po;
    PDBfile *file;
    object *rv;

    rv   = SS_null;
    tab  = NULL;
    file = NULL;
    name = NULL;
    SS_args(si, argl,
            G_FILE, &po,
            SS_HASHARR_I, &tab,
            SC_STRING_I, &name,
            0);

    if (po == NULL)
       SS_error(si, "BAD FILE - _SXI_HASH_TO_PDBDATA", argl);
    else
       file = FILE_FILE(PDBfile, po);

    if (tab == NULL)
       SS_error(si, "BAD HASH TABLE - _SXI_HASH_TO_PDBDATA", argl);

    else if (name == NULL)
       SS_error(si, "BAD HASH TABLE NAME - _SXI_HASH_TO_PDBDATA", argl);

    else
       rv = SX_pdbdata_handler(si, file, name, "hasharr", tab, FALSE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_PDBDATA_HANDLER - given a list with a PDB file object and a name,
 *                    - a type, and a pointer to some data
 *                    - encapsulate the pointer in a PDBDATA object and
 *                    - return a pointer to it
 */

object *SX_pdbdata_handler(SS_psides *si, PDBfile *file,
			   char *name, char *type, void *vr, int flag)
   {syment *ep;
    SC_address data;
    object *ret;
    char fullpath[MAXLINE];
    int new;

    data.memaddr = (char *) vr;
    strcpy(fullpath, _PD_fixname(file, name));

    if (file == NULL)
       ep = NULL;

/* if the next object is a pdbfile, use it, otherwise, use default file */
    else if (file->virtual_internal == TRUE)

/* if its an indirection, chances are high that vr is on the stack as something
 * like &f where f is a local variable
 * this had better be guarded against
 */
       {if (_PD_indirection(type))
           {char **p;
            p  = CMAKE(char *);
            *p = DEREF(vr);
            data.memaddr = (char *) p;
	    SC_mark(*p, 1);
            SC_mark(p, 1);};

        ep = _PD_mk_syment(type, 1L, data.diskaddr, NULL, NULL);

        _PD_e_install(file, fullpath, ep, TRUE);}

    else
       {ep = file->tr->write(file, fullpath, type, type,
			     data.memaddr, NULL, FALSE, &new);
        if (ep == NULL)
           SS_error(si, PD_get_error(), SS_null);

	if (_PD_IS_DP_INIT)
	   lio_flush(file->stream);

	if (new)
	   ep = PD_copy_syment(ep);

        if (flag)
           PD_reset_ptr_list(file);};
    
    ret = _SX_mk_gpdbdata(si, fullpath, data.memaddr, ep, file);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SET_SWITCH - set a switch */

static object *_SXI_set_switch(SS_psides *si, object *argl)
   {int indx, val;
    object *rv;

    indx = -1;
    val  = -1;
    SS_args(si, argl,
            SC_INT_I, &indx,
            SC_INT_I, &val,
            0);

    if (indx < 0)
       SS_error(si, "BAD INDEX - _SXI_SET_SWITCH", argl);

    if (val != -1)
       PD_gs.print_ctrl[indx] = val;

    rv = SS_mk_integer(si, PD_gs.print_ctrl[indx]);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SET_MAX_SIZE - set the file->maximum_size */

static object *_SXI_set_max_size(SS_psides *si, object *argl)
   {int v;
    g_file *po;
    PDBfile *file;

    po = NULL;
    v  = -1;
    SS_args(si, argl,
            G_FILE, &po,
            SC_INT_I, &v,
            0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    PD_set_max_file_size(file, v);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SET_TRACK_POINTERS - set the file->track_pointers */

static object *_SXI_set_track_pointers(SS_psides *si, object *argl)
   {int v;
    g_file *po;
    PDBfile *file;

    po = NULL;
    v  = -1;
    SS_args(si, argl,
            G_FILE, &po,
            SC_INT_I, &v,
            0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    PD_set_track_pointers(file, v);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_SET_USER_FORMAT - set the SCORE user formats entries I to FORMAT
 *                     - return a string object naming the format
 *                     - for that type
 */

static object *_SX_set_user_format(SS_psides *si, int i, char *format, int whch)
   {int h1, h2;
    object *rv;
    char **fmts, **fmta;
    char **ufmts, **ufmta;

    fmts  = _SC.types.formats;
    fmta  = _SC.types.formata;
    ufmts = _SC.types.user_formats;
    ufmta = _SC.types.user_formata;

    h1 = ((whch & 1) != 0);
    h2 = ((whch & 2) != 0);

    if (format != NULL)
       {if (h1 == TRUE)
	   {if (ufmts[i] != NULL)
	       {CFREE(ufmts[i]);};
	    ufmts[i] = CSTRSAVE(format);}
	if (h2 == TRUE)
	   {if (ufmta[i] != NULL)
	       {CFREE(ufmta[i]);};
	    ufmta[i] = CSTRSAVE(format);};};

    if (h1 == TRUE)
       {if (ufmts[i] != NULL)
	   rv = SS_mk_string(si, ufmts[i]);
        else
	   rv = SS_mk_string(si, fmts[i]);}

    else if (h2 == TRUE)
       {if (ufmta[i] != NULL)
	   rv = SS_mk_string(si, ufmta[i]);
        else
	   rv = SS_mk_string(si, fmta[i]);}

    else
       rv = SS_null;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SET_FORMAT - set a format */

static object *_SXI_set_format(SS_psides *si, object *argl)
   {int i, id, ok;
    char s1[MAXLINE], s2[MAXLINE];
    char *field, *format, *typ;
    char **chtypes, **fxtypes, **fptypes, **cmtypes;
    object *rv;

    chtypes = _SC.types.chrtyp;
    fxtypes = _SC.types.fixtyp;
    fptypes = _SC.types.fptyp;
    cmtypes = _SC.types.cpxtyp;

    field  = NULL;
    format = NULL;
    SS_args(si, argl,
            SC_STRING_I, &field,
            SC_STRING_I, &format,
            0);

    rv = SS_t;

    ok = FALSE;

/* character types (proper) */
    for (i = 0; (i < N_PRIMITIVE_CHAR) && (ok == FALSE); i++)
        {id  = SC_TYPE_CHAR(i);
	 typ = chtypes[i];
	 snprintf(s1, MAXLINE, "%s1", typ);
	 snprintf(s2, MAXLINE, "%s2", typ);
	 if (strcmp(field, typ) == 0)
	    {rv = _SX_set_user_format(si, id, format, 3);
	     ok = TRUE;}

	 else if (strcmp(field, s1) == 0)
	    {rv = _SX_set_user_format(si, id, format, 1);
	     ok = TRUE;}

	 else if (strcmp(field, s2) == 0)
	    {rv = _SX_set_user_format(si, id, format, 2);
	     ok = TRUE;};};

/* fixed point types (proper) */
    for (i = 0; (i < N_PRIMITIVE_FIX) && (ok == FALSE); i++)
        {id  = SC_TYPE_FIX_ID(i);
	 typ = fxtypes[i];
	 snprintf(s1, MAXLINE, "%s1", typ);
	 snprintf(s2, MAXLINE, "%s2", typ);
	 if (strcmp(field, typ) == 0)
	    {rv = _SX_set_user_format(si, id, format, 3);
	     ok = TRUE;}

	 else if (strcmp(field, s1) == 0)
	    {rv = _SX_set_user_format(si, id, format, 1);
	     ok = TRUE;}

	 else if (strcmp(field, s2) == 0)
	    {rv = _SX_set_user_format(si, id, format, 2);
	     ok = TRUE;};};

/* real floating point types (proper) */
    for (i = 0; (i < N_PRIMITIVE_FP) && (ok == FALSE); i++)
        {id  = SC_TYPE_FP_ID(i);
	 typ = fptypes[i];
	 snprintf(s1, MAXLINE, "%s1", typ);
	 snprintf(s2, MAXLINE, "%s2", typ);
	 if (strcmp(field, typ) == 0)
	    {rv = _SX_set_user_format(si, id, format, 3);
	     ok = TRUE;}

	 else if (strcmp(field, s1) == 0)
	    {rv = _SX_set_user_format(si, id, format, 1);
	     ok = TRUE;}

	 else if (strcmp(field, s2) == 0)
	    {rv = _SX_set_user_format(si, id, format, 2);
	     ok = TRUE;};};

/* complex floating point types (proper) */
    for (i = 0; (i < N_PRIMITIVE_FP) && (ok == FALSE); i++)
        {id  = SC_TYPE_CPX_ID(i);
	 typ = cmtypes[i];
	 snprintf(s1, MAXLINE, "%s1", typ);
	 snprintf(s2, MAXLINE, "%s2", typ);
	 if (strcmp(field, typ) == 0)
	    {rv = _SX_set_user_format(si, id, format, 3);
	     ok = TRUE;}

	 else if (strcmp(field, s1) == 0)
	    {rv = _SX_set_user_format(si, id, format, 1);
	     ok = TRUE;}

	 else if (strcmp(field, s2) == 0)
	    {rv = _SX_set_user_format(si, id, format, 2);
	     ok = TRUE;};};

    if (ok == FALSE)
       {if (strcmp(field, SC_BOOL_S) == 0)
	   rv = _SX_set_user_format(si, SC_BOOL_I, format, 3);

        else if (strcmp(field, "bool1") == 0)
	   rv = _SX_set_user_format(si, SC_BOOL_I, format, 1);

	else if (strcmp(field, "bool2") == 0)
	   rv = _SX_set_user_format(si, SC_BOOL_I, format, 2);

	else if (strcmp(field, "user-int") == 0)
	   rv = _SX_set_user_format(si, SC_BIT_I, format, 3);

	else if (strcmp(field, "suppress-member") == 0)
	   {if (_SC.types.suppress_member != NULL)
	       {CFREE(_SC.types.suppress_member);};
	    _SC.types.suppress_member = CSTRSAVE(format);}

	else if (strcmp(field, "default") == 0)
	   _SC_set_user_defaults();

	else
	   SS_error(si, "UNKNOWN TYPE - _SXI_SET_FORMAT", argl);};

    CFREE(field);
    CFREE(format);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_PDB_TYPE - make a type cons for make-pdbdata */

static object *_SXI_pdb_type(SS_psides *si, object *argl)
   {object *o;

    o = SS_mk_cons(si, SS_mk_string(si, "type"), argl);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_UNP_BITSTRM - read a variable as a bitstream and unpack it into
 *                  - the specified type in a numeric array
 *                  -
 *                  - Form: (pd-unpack-bitstream <file> <name> <type> <nbits>
 *                  -                            <padsz> <fpp>)
 *                  - 
 *                  -  <type>  = data type of destination
 *                  -  <nbits> = # bits per field
 *                  -  <padsz> = # pad bits
 *                  -  <fpp>   = # fields per pad
 *                  -            <pad> <field> <field> <field> ... <pad> ...
 */

static object *_SXI_unp_bitstrm(SS_psides *si, object *argl)
   {int nbits, padsz, fpp;
    long anumb, offs, ni;
    char *name, *type, *data;
    PDBfile *file;
    C_array *arr;
    g_file *po;
    object *rv;

    rv    = SS_null;
    po    = NULL;
    name  = NULL;
    type  = NULL;
    file  = NULL;
    nbits = 8;
    padsz = 0;
    fpp   = 1;
    offs  = 0L;
    SS_args(si, argl,
            G_FILE, &po,
            SC_STRING_I, &name,
            SC_STRING_I, &type,
            SC_LONG_I, &ni,
            SC_INT_I, &nbits,
            SC_INT_I, &padsz,
            SC_INT_I, &fpp,
            SC_LONG_I, &offs,
            0);

    if ((po == NULL) || (po == SX_gs.gvif))
       file = SX_gs.vif;

    else if (strcmp(po->type, PDBFILE_S) == 0)
       file = FILE_FILE(PDBfile, po);

    else
       SS_error(si, "BAD FILE - _SXI_UNP_BITSTRM", argl);

    if (file != NULL)
       {arr = NULL;
	if (!_PD_rd_bits(file, name, type, ni, FALSE,
			 nbits, padsz, fpp, offs,
			 &anumb, &data))
	   SS_error(si, "_PD_RD_BITS FAILED - _SXI_UNP_BITSTRM", argl);

	arr = PM_make_array(type, anumb, data);

	rv = SX_mk_C_array(si, arr);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_TYPE_CONTAINER - function to hang on SC_contain_hook */

void _SX_type_container(char *dtype, char *stype)
   {defstr *sp, *dp;
    PDBfile *file;
    g_file *po;
    SS_psides *si;

    si = SC_GET_CONTEXT(_SX_type_container);

    dtype[0] = '\0';

    po = NULL;
    SS_args(si, SS_lk_var_val(si, SX_gs.curfile),
	    G_FILE, &po,
	    0);

    if (po == NULL)
       file = SX_gs.vif;
    else
       file = FILE_FILE(PDBfile, po);

    sp = PD_inquire_type(file, stype);
    if (sp != NULL)
       {dp = _PD_type_container(file, sp);
	if (dp != NULL)
	   strcpy(dtype, dp->type);
	else
	   strcpy(dtype, stype);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_INDEX_TO_EXPR - wrapper for PD_index_to_expr
 *                    - usage: (index->tuple-string <file> <name> <n>)
 */

static object *_SXI_index_to_expr(SS_psides *si, object *argl)
   {long indx;
    char path[MAXLINE];
    char *name, *s;
    syment *ep;
    dimdes *dp;
    PDBfile *file;
    g_file *po;
    object *rv;

    rv   = SS_null;
    po   = NULL;
    name = NULL;
    indx = 0L;
    file = NULL;
    SS_args(si, argl,
            G_FILE, &po,
	    SC_STRING_I, &name,
	    SC_LONG_I, &indx,
	    0);

    if ((po == NULL) || (po == SX_gs.gvif))
       file = SX_gs.vif;

    else if (strcmp(po->type, PDBFILE_S) == 0)
       file = FILE_FILE(PDBfile, po);

    else
       SS_error(si, "BAD FILE - _SXI_INDEX_TO_EXPR", argl);

    if (file != NULL)
       {s = CMAKE_N(char, MAXLINE);

	ep = _PD_effective_ep(file, name, TRUE, path);
	if (ep != NULL)
	   {dp = PD_entry_dimensions(ep);

	    PD_index_to_expr(s, indx, dp,
			     PD_get_major_order(file),
			     PD_get_offset(file));

	    _PD_rl_syment_d(ep);};

	rv = SS_mk_string(si, s);};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                         PDB FUNCTION INSTALLATION                        */

/*--------------------------------------------------------------------------*/
 
/* SX_INSTALL_PDB_FUNCS - install the PDB extensions to Scheme */
 
void SX_install_pdb_funcs(SS_psides *si)
   {

    SS_install(si, "change-directory",
               "Change current working directory in a file",
               SS_nargs,
               _SXI_change_directory, SS_PR_PROC);

    SS_install(si, "close-bin-file",
               "Close a binary file",
	       SS_sargs,
	       _SXI_close_pdbfile, SS_PR_PROC);

    SS_install(si, "close-pdbfile",
               "Close a PDB file",
               SS_sargs,
               _SXI_close_pdbfile, SS_PR_PROC);

    SS_install(si, "close-raw-binary-file",
               "Close a raw binary file",
               SS_nargs,
               _SXI_close_raw_file, SS_PR_PROC);

    SS_install(si, "collect-io-info",
               "Globally control collection of stats on I/O operations on files",
               SS_sargs,
               _SXI_collect_io_info, SS_PR_PROC);

    SS_install(si, "create-link",
               "Create a link to a variable in a file",
               SS_nargs,
               _SXI_create_link, SS_PR_PROC);

    SS_install(si, "create-pdbfile",
               "Create a PDB file, return PDBfile",
               SS_sargs,
               _SXI_create_pdbfile, SS_PR_PROC);

    SS_install(si, "current-directory",
               "Return the current working directory in a file",
               SS_sargs,
               _SXI_current_directory, SS_PR_PROC);

    SS_install(si, "def-common-types",
               "Define some common internal SX data types to the given file",
               SS_sargs,
               _SXI_def_common_types, SS_PR_PROC);

    SS_install(si, "def-member",
               "Create a member list",
               SS_nargs, 
               SS_list, SS_UR_MACRO);

    SS_install(si, "change-primitive",
               "Change properties of a primitive type which",
               SS_nargs,
               _SXI_chg_prim, SS_PR_PROC);

    SS_install(si, "def-primitive",
               "Define a new primitive type which is either: ncv, fix, fp",
               SS_nargs,
               _SXI_def_prim, SS_PR_PROC);

    SS_install(si, "default-offset",
               "Set the default offset for the given file",
               SS_nargs,
               _SXI_default_offset, SS_PR_PROC);

    SS_install(si, "defstr?",
               "Returns #t if the object is a defstr, and #f otherwise",
               SS_sargs, 
               _SXI_defp, SS_PR_PROC);

    SS_install(si, "desc-variable",
               "Describe a pdb varible",
               SS_sargs, 
               _SXI_desc_variable, SS_PR_PROC);

    SS_install(si, "diff-variable",
               "Compare an entry in two files\nFORM: (diff-variable <file1> <file2> <var1> <var2> [<n-digits>] [<individ-diff-flag>] [<promote-flag>])",
               SS_nargs, 
               _SXI_diff_var, SS_PR_PROC);

    SS_install(si, "display-differences",
               "Display information returned by diff-variable",
               SS_nargs, 
               _SXI_display_diff, SS_PR_PROC);

    SS_install(si, "display-menu",
               "Display a menu of mappings, images, and curves",
               SS_nargs, 
               _SXI_menu, SS_PR_PROC);

    SS_install(si, "family-file",
               "Check the file size and if necessary open the next family member",
               SS_nargs,
               _SXI_family_file, SS_PR_PROC);

    SS_install(si, "file-content",
               "Print the metadata of the given file\nFORM: (file-content <file> [<outfile>] [<version>] [<chart>])",
               SS_nargs, 
               _SXI_file_content, SS_PR_PROC);

    SS_install(si, "file-info",
               "Return the named information about the given file",
               SS_nargs, 
               _SXI_file_info, SS_PR_PROC);

    SS_install(si, "file-mode",
               "Set the mode of the given file",
               SS_nargs, 
               _SXI_file_mode, SS_PR_PROC);

    SS_install(si, "file-directory?",
               "Return #t iff the named object is a directory in the given file",
               SS_nargs,
               _SXI_file_dirp, SS_PR_PROC);

    SS_install(si, "file-variable?",
               "Return #t iff the named object is a variable in the given file",
               SS_nargs,
               _SXI_file_varp, SS_PR_PROC);

    SS_install(si, "find-types",
               "Return a list of the derived data types in a variable",
               SS_sargs,
               _SXI_find_types, SS_PR_PROC);

    SS_install(si, "flush-pdbfile",
               "Flush a PDB file",
               SS_sargs,
               _SXI_flush_pdbfile, SS_PR_PROC);

    SS_install(si, "get-io-info",
               "Return a list of stats on I/O operations on a PDB file",
               SS_sargs,
               _SXI_get_io_info, SS_PR_PROC);

    SS_install(si, "hash->pdbdata",
               "Convert a hash table to a pdbdata object",
               SS_nargs, 
               _SXI_hash_to_pdbdata, SS_PR_PROC);

    SS_install(si, "index->tuple-string",
               "Return an index tuple string generated from a linear index",
               SS_nargs, 
               _SXI_index_to_expr, SS_PR_PROC);

    SS_install(si, "indirection",
               "Create a type list",
               SS_nargs, 
               SS_list, SS_PR_PROC);

    SS_install(si, "list-defstrs",
               "Return a list of the data types in a file",
               SS_nargs,
               _SXI_list_defstrs, SS_PR_PROC);

    SS_install(si, "list-file",
               "Return a list of open pdbfiles",
               SS_zargs,
               _SXI_list_file, SS_PR_PROC);

    SS_install(si, "list-symtab",
               "Return a list of the symbol table entries",
               SS_nargs,
               _SXI_list_symtab, SS_PR_PROC);

    SS_install(si, "list-variables",
               "Return a list of the variables in a file directory",
               SS_nargs,
               _SXI_list_variables, SS_PR_PROC);

    SS_install(si, "make-cast",
               "Specify that the real type of a member comes from another member",
               SS_nargs,
               _SXI_make_cast, SS_UR_MACRO);

    SS_install(si, "make-cast*",
               "Specify that the real type of a member comes from another member",
               SS_nargs,
               _SXI_make_cast, SS_PR_PROC);

    SS_install(si, "make-defstr",
               "Create a DEFSTR from the list (macro version)",
               SS_nargs,
               _SXI_make_defstr, SS_UR_MACRO);

    SS_install(si, "make-defstr*",
               "Create a DEFSTR from the list (procedure version)",
               SS_nargs,
               _SXI_make_defstr, SS_PR_PROC);

    SS_install(si, "make-directory",
               "Create a directory in a file",
               SS_nargs,
               _SXI_make_directory, SS_PR_PROC);

    SS_install(si, "make-typedef",
               "Create a type alias for the given type (macro version)",
               SS_nargs,
               _SXI_make_typedef, SS_UR_MACRO);

    SS_install(si, "make-typedef*",
               "Create a type alias for the given type (procedure version)",
               SS_nargs,
               _SXI_make_typedef, SS_PR_PROC);

    SS_install(si, "major-order",
               "Set the given file to row or column major order",
               SS_nargs,
               _SXI_major_order, SS_PR_PROC);

    SS_install(si, "open-bin-file",
               "Open a binary file",
	       SS_nargs,
	       _SXI_open_pdbfile, SS_PR_PROC);

    SS_install(si, "open-pdbfile",
               "Open a PDB file, return PDBfile",
               SS_nargs,
               _SXI_open_pdbfile, SS_PR_PROC);

    SS_install(si, "open-raw-binary-file",
               "Open a binary file as an input port",
               SS_nargs,
               _SXI_open_raw_file, SS_PR_PROC);

    SS_install(si, "parse-declaration",
               "Parse a C style definition/declaration and return a list of elements",
               SS_nargs, 
               _SXI_parse_type, SS_PR_PROC);

    SS_install(si, "pd-entry-number",
               "Return the number of items for the named variabled",
               SS_nargs, 
               _SXI_entry_number, SS_PR_PROC);

    SS_install(si, "pd-unpack-bitstream",
               "Read a variable from a file as a bit stream and unpack it",
               SS_nargs, 
               _SXI_unp_bitstrm, SS_PR_PROC);

    SS_install(si, "pdb->list",
               "Convert some pdb type to a list",
               SS_sargs, 
               _SXI_pdb_to_list, SS_PR_PROC);

    SS_install(si, "pdb-read-numeric-data",
               "Read a numeric array from a PDB file",
               SS_nargs,
               _SXI_read_numeric_data, SS_PR_PROC);

    SS_install(si, "pdbdata",
               "Convert arguments to a pdb variable",
               SS_nargs, 
               _SXI_to_pdbdata, SS_PR_PROC);

    SS_install(si, "pdbdata?",
               "Returns #t if the object is a pdbdata, and #f otherwise",
               SS_sargs, 
               _SXI_pdbdatap, SS_PR_PROC);

    SS_install(si, "pdbdata->hash",
               "Convert a pdbdata object to a hash object",
               SS_sargs, 
               _SXI_pdbdata_to_hash, SS_PR_PROC);

    SS_install(si, "pdbfile?",
               "Returns #t if the object is a pdb file, and #f otherwise",
               SS_sargs, 
               _SXI_pdbfp, SS_PR_PROC);

    SS_install(si, "print-pdb",
               "Print the contents of a PDB variable",
               SS_nargs, 
               _SXI_print_pdb, SS_PR_PROC);

    SS_install(si, "read-binary",
               "Read binary data from a raw binary file",
               SS_nargs,
               _SXI_rd_raw, SS_PR_PROC);

    SS_install(si, "read-defstr",
               "Read a DEFSTR from the given file (macro version)",
               SS_nargs,
               _SXI_read_defstr, SS_UR_MACRO);

    SS_install(si, "read-defstr*",
               "Read a DEFSTR from the given file (procedure version)",
               SS_nargs,
               _SXI_read_defstr, SS_PR_PROC);

    SS_install(si, "read-pdbdata",
               "Read PDB data from a file and encapsulate is as a PDBDATA object",
               SS_nargs,
               _SXI_read_pdbdata, SS_PR_PROC);

    SS_install(si, "read-syment",
               "Get a syment from the specified file",
               SS_nargs,
               _SXI_rd_syment, SS_PR_PROC);

    SS_install(si, "remove-entry",
               "Remove an entry from a PDB file",
               SS_nargs,
               _SXI_remove_entry, SS_PR_PROC);

    SS_install(si, "reserve-pdbdata",
               "Reserve space for PDB data in a file",
               SS_nargs,
               _SXI_reserve_pdbdata, SS_PR_PROC);

    SS_install(si, "reset-pointer-list!",
               "Reset the pointer lists for the given file",
               SS_nargs,
               _SXI_reset_ptrs, SS_PR_PROC);

    SS_install(si, "scatter-pdbdata",
               "Scatter data to a file and encapsulate is as a PDBDATA object",
               SS_nargs,
               _SXI_scatter_pdbdata, SS_PR_PROC);

    SS_install(si, "seek-file",
               "Seek in a binary file",
               SS_nargs,
               _SXI_seek_raw_file, SS_PR_PROC);

    SS_install(si, "set-format",
               "Set an edit format",
               SS_nargs, 
               _SXI_set_format, SS_PR_PROC);

    SS_install(si, "set-switch",
               "Set a code switch",
               SS_nargs, 
               _SXI_set_switch, SS_PR_PROC);

    SS_install(si, "set-maximum-file-size!",
               "Set the maximum file size for a PDB file",
               SS_nargs, 
               _SXI_set_max_size, SS_PR_PROC);

    SS_install(si, "set-track-pointers!",
               "Set the track_pointers flag for a PDB file",
               SS_nargs, 
               _SXI_set_track_pointers, SS_PR_PROC);

    SS_install(si, "sizeof",
               "Return the size in bytes of the specified type\nUsage: (sizeof <type> [<file> [1]])",
               SS_nargs, 
               _SXI_sizeof, SS_PR_PROC);

    SS_install(si, "show-pdb",
               "Display the contents of a PDB variable",
               SS_nargs, 
               _SXI_show_pdb, SS_PR_PROC);

    SS_install(si, "syment?",
               "Returns #t if the object is a syment, and #f otherwise",
               SS_sargs, 
               _SXI_symp, SS_PR_PROC);

    SS_install(si, "target",
               "Set the data standard and alignment for the next file opened",
               SS_nargs,
               _SXI_target, SS_PR_PROC);

    SS_install(si, "type",
               "Create a type list for make-pdbdata",
               SS_nargs, 
               _SXI_pdb_type, SS_PR_PROC);

    SS_install(si, "write-binary",
               "Write binary data to a raw binary file",
               SS_nargs,
               _SXI_wr_raw, SS_PR_PROC);

    SS_install(si, "write-defstr",
               "Write a DEFSTR to the given file (macro version)",
               SS_nargs,
               _SXI_write_defstr, SS_UR_MACRO);

    SS_install(si, "write-defstr*",
               "Write a DEFSTR to the given file (procedure version)",
               SS_nargs,
               _SXI_write_defstr, SS_PR_PROC);

    SS_install(si, "write-pdbdata",
               "Write PDB data to a file and encapsulate is as a PDBDATA object",
               SS_nargs,
               _SXI_write_pdbdata, SS_PR_PROC);

    SS_install(si, "write-syment",
               "Install a syment in the specified file",
               SS_nargs,
               _SXI_wr_syment, SS_PR_PROC);

    SS_install(si, "write-ultra-curve",
               "Create an ULTRA curve",
               SS_nargs,
               _SXI_wrt_ultra_curve, SS_PR_PROC);

#if 0

    SS_install(si, "close-text-file",
               "Close a text file",
               SS_nargs,
               _SXI_close_textfile, SS_PR_PROC);

    SS_install(si, "open-text-file",
               "Open a text file",
               SS_nargs,
               _SXI_open_textfile, SS_PR_PROC);

    SS_install(si, "wr-to-textfile",
               "Write the contents of a PDB variable to a text file",
               SS_nargs,
               _SXI_wr_to_textfile, SS_PR_PROC);

#endif

    SS_install_cf(si, "string-mode",
                  "Variable: Mode for string reads - literal or nopadding",
                  SS_acc_int,
                  &_SX.string_mode);

    SS_install_cf(si, "comparison-precision",
		  "Variable: Comparison precision for floats",
		  SS_acc_int,
		  &_SC.types.max_digits);

    SS_install_cf(si, "display-individual-differences",
		  "Variable: Difference display mode flag",
		  SS_acc_int,
                  &SX_gs.disp_individ_diff);

    SS_install_cf(si, "format-version",
		  "Variable: PDB metadata format version",
		  SS_acc_int,
                  &PD_gs.default_format_version);

    SX_install_pdb_attr_funcs(si);

    return;}
 
/*--------------------------------------------------------------------------*/

#if 0

/*--------------------------------------------------------------------------*/

/* _SXI_SET_BUFFER_SIZE - set the buffer_size */

static object *_SXI_set_buffer_size(SS_psides *si, object *argl)
   {int v;

    v = -1;
    SS_args(si, argl,
            SC_INT_I, &v,
            0);

    PD_set_buffer_size(v);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GET_BUFFER_SIZE - get the buffer_size */

static object *_SXI_get_buffer_size(SS_psides *si)
   {long long v;
    object *o;

    v = PD_get_buffer_size();
    o = SS_mk_integer(si, v);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GET_ERROR - get the PDB error message */

static object *_SXI_get_error(SS_psides *si)
   {char *v;
    object *o;

    v = PD_get_error();
    o = SS_mk_string(si, v);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* SX_INSTALL_PDB_FUNCS - install the PDB extensions to Scheme */
 
void SX_install_pdb_funcs(SS_psides *si)
   {

    SS_install(si, "pd-get-buffer-size",
               "Get the default buffer size for I/O files",
               SS_zargs,
               _SXI_get_buffer_size, SS_PR_PROC);

    SS_install(si, "pd-set-buffer-size!",
               "Set the default file buffer size",
               SS_nargs, 
               _SXI_set_buffer_size, SS_PR_PROC);

    SS_install(si, "pd-error-message",
               "Get the last PDB error message",
               SS_zargs,
               _SXI_get_error, SS_PR_PROC);

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
