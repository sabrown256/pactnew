/*
 * TRHDF5.C - command processors for HDF5 spoke in TRANSL
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

/* For more information on the HDF5 API see:
 *
 * http://hdf.ncsa.uiuc.edu/HDF5/doc/RM_H5Front.html
 *
 * In order to provide a translation from an HDF5 file to a PDBfile struct
 * some internal C typenames have been imposed on HDF5 datatypes. For
 * the sake of standardization, the following names and sizes have been
 * used for the missing C typenames in HDF5 files:
 *
 * PDB      HDF5
 *
 * char     fixed-pt   8 bits
 * short    fixed-pt  16 bits
 * int      fixed-pt  32 bits
 * long     fixed-pt  64 bits
 * float    float-pt  32 bits
 * double   float-pt  64 bits
 *
 */

#include "../../pdb_int.h"

#include "trhdf5.h"

#ifdef HAVE_HDF5

#include "hdf5.h"

/* If DEBUG is 1 then debug info is printed to stderr
 *
 * To debug this code as:  pdbview filename.h5
 *
 * totalview `which sx` -a -p filename.h5 
 *
 */
#define DEBUG 0

#if DEBUG
#  define DEBUG1(_a_, _b_)  fprintf(stderr, _a_, _b_)
#else
#  define DEBUG1(_a_, _b_)
#endif

typedef struct s_hdf_state hdf_state;

struct s_hdf_state
   {int nstr;                   /* number of structs created so far */
    compound_desc *registry;    /* registry of info on structs created */
    PDBfile *pf;                /* global PDB file pointer */
    hid_t hf;                   /* global hdf_file pointer */
    char *group_prefix;};       /* current dir prefix */

static char
 *H5FILE_S = "HDF5file";  /* Identifier string */

static char
 *_H5_handle_compound(PDBfile *file, hid_t datatype_id);

static long
 _H5_rd_syment(PDBfile *file, syment *ep, char *outtype, void *vr),
 _H5_wr_syment(PDBfile *file, char *vr, long nitems,
	       char *intype, char *outtype);

static sys_layer
 _H5_sys = {_H5_rd_syment, _H5_wr_syment};

/*--------------------------------------------------------------------------*/

/*               PRIVATE HDF5 SPOKE FUNCTION DEFINITIONS                    */

/*--------------------------------------------------------------------------*/

/* _H5_IS_REGISTERED - determine if a compound datatype is already  
 *                   - registered with us, and if so, return its name
 *                   - DTID is the compound datatype to check
 */

static char *_H5_is_registered(PDBfile *file, hid_t dtid)
   {int i, matches;
    char *verdict, *tempname;
    compound_desc *iter;
    compound_member_info *info;
    H5T_class_t type_class;
    hdf_state *hst;

    type_class = H5Tget_class(dtid);
 
/* sanity check: we only register compound types */ 
    if (type_class != H5T_COMPOUND)
       return NULL;

    hst     = file->meta;
    iter    = hst->registry; 
    verdict = NULL;

/* find any matching datatype registered with us already */
    while (iter != NULL && !verdict) 

/* do they have the same number of members? */
       {if (iter->num_members == H5Tget_nmembers(dtid))

/* initially assume it matches */
	   {matches = TRUE;
       
	    info = iter->info;

/* if anything does NOT match, remember this */
	    for (i = 0 ; i < iter->num_members ; i++)
                {tempname = H5Tget_member_name(dtid, i);
                
		 if ((strcmp(info->member_name, tempname) != 0) ||
                     (info->member_offset != H5Tget_member_offset(dtid, i)))
                    {matches = FALSE; 
		     free(tempname);
		     break;}; 

		 free(tempname);
                
		 info = info->next;};

/* only if everything matches do we assume this is the one */
	    if (matches)
               verdict = iter->compound_name;}
       else
	  iter = iter->next;};

    return(verdict);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_REGISTER - register the compound datatype with us        
 *              - ID is the HDF5 handle to the datatype
 *              - TYPE null-terminated string naming the type
 */

static void _H5_register(PDBfile *file, hid_t id, char *type)
   {int i;
    char *verdict, *tempname;
    compound_desc *iter, *prev;
    compound_member_info *info;
    hdf_state *hst;

    prev    = NULL;
    hst     = file->meta;
    iter    = hst->registry; 
    verdict = _H5_is_registered(file, id);

/* do not register the compound type if it is already registered */
    if (verdict == NULL)

/* find the last position in the linked list */
       {while (iter != NULL) 
           {prev = iter;
            iter = iter->next;};

/* grow a compound_desc there (or at registry itself if it is empty) */
        if (prev == NULL)
          {hst->registry = FMAKE(compound_desc, "_H5_REGISTER:iter"); 
           iter         = hst->registry;}
        else 
          {prev->next = FMAKE(compound_desc, "_H5_REGISTER:iter");  
           iter       = prev->next;};

/* insert the data into the new compound_desc */
        iter->compound_name = SC_strsavef(type, "_H5_REGISTER:type");
        iter->num_members   = H5Tget_nmembers(id);

/* link in all of the compound member info objects */
        iter->info = FMAKE(compound_member_info, "_H5_REGISTER:info");
        info = iter->info;

        for (i = 0 ; i < iter->num_members ; i++)
            {tempname = H5Tget_member_name(id, i);

             info->member_offset = H5Tget_member_offset(id, i); 
             info->member_name   = SC_strsavef(tempname, "_H5_REGISTER:name");

             free(tempname);

             if ((i+1) < iter->num_members)
                {info->next = FMAKE(compound_member_info, "_H5_REGISTER:info");}
             else
                {info->next = NULL;}; 
             
             info = info->next;};

        iter->next = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_GET_ALIGNMENT - return the alignment for type TYPE */

static int _H5_get_alignment(PDBfile *file, char *type)
   {int id, resut;
    static algn[PN_N_PRIMITIVES] = { 0, 0, 0, 0,
                                     2, 4, 4, 4,  /* fixed point types */
                                     4, 4, 4,     /* floating point types */
                                     0, 0, 0 };   /* complex floating point types */

    id = SC_type_id(type, TRUE);
    result = algn[id];

    return(result);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_HANDLE_FIXED_PT - parse the binary representation of a fixed   
 *                     - point format from the HDF5 file
 */

static char *_H5_handle_fixed_pt(PDBfile *file, hid_t dtid) 
   {int i;
    short precision;
    char *typename;
    PD_byte_order order;
    defstr *dp;
    hdf_state *hst;

    hst      = file->meta;
    typename = NULL;
    dp       = FMAKE(defstr, "_H5_HANDLE_FIXED_PT:dp");

    precision = (short) H5Tget_precision(dtid);

/* HDF5 assumes all strings are chars with a precision of 8 bits */
    if (H5Tget_class(dtid) == H5T_STRING) 
       precision = 8;

    DEBUG1("      precision %d\n", precision);

/* hard-coded mapping of precision to C type name
 * this is required because HDF5 does not maintain type names
 */
    if (H5Tget_sign(dtid)) 
       {if (precision == 8)
           {typename = SC_strsavef(SC_CHAR_S,      "_H5_HANDLE_FIXED_PT:typename");}
        else if (precision == 16)
           {typename = SC_strsavef(SC_SHORT_S,     "_H5_HANDLE_FIXED_PT:typename");}
        else if (precision == 32)
           {typename = SC_strsavef(SC_INT_S,       "_H5_HANDLE_FIXED_PT:typename");}
        else if (precision == 64)
           {typename = SC_strsavef(SC_LONG_S,      "_H5_HANDLE_FIXED_PT:typename");}
        else if (precision >  64)
           {typename = SC_strsavef(SC_LONG_LONG_S, "_H5_HANDLE_FIXED_PT:typename");}
        else
           {typename = SC_strsavef("UNKNOWN",      "_H5_HANDLE_FIXED_PT:typename");};}
    else
       {if (precision == 8)
           {typename = SC_strsavef("u_char",      "_H5_HANDLE_FIXED_PT:typename");}
        else if (precision == 16)
           {typename = SC_strsavef("u_short",     "_H5_HANDLE_FIXED_PT:typename");}
        else if (precision == 32)
           {typename = SC_strsavef("u_int",       "_H5_HANDLE_FIXED_PT:typename");}
        else if (precision == 64)
           {typename = SC_strsavef("u_long",      "_H5_HANDLE_FIXED_PT:typename");}
        else if (precision >  64)
           {typename = SC_strsavef("u_long_long", "_H5_HANDLE_FIXED_PT:typename");}
        else
           {typename = SC_strsavef("UNKNOWN",     "_H5_HANDLE_FIXED_PT:typename");};};

/* handle byte ordering */ 
    if (H5Tget_order(dtid) == H5T_ORDER_BE) 
       {order = NORMAL_ORDER;
        DEBUG1("%s", "      big endian integers\n");}
    else 
       {order = REVERSE_ORDER;
        DEBUG1("%s", "      little endian integers\n");};

/* you never know who might use file->std in the future: fill it in */
    for (i = 0; i < PD_N_PRIMITIVE_FIX; i++)
        hst->pf->std->fx[i].order = order; 

/* create a defstr with this information
 * NOTE: as far as we know -- HDF5 does not support 1's comp
 */
    dp->onescmp   = FALSE;
    dp->type      = SC_strsavef(typename, "_H5_HANDLE_FIXED_PT:dp->type");
    dp->size_bits = 0;
    dp->size      = precision / 8;

/* GOTCHA: we have no alignment info from the file; so we guess */
    dp->alignment   = _H5_get_alignment(file, typename);

/* HDF5 files do not currently support indirection */
    dp->n_indirects = 0;
    dp->unsgned     = !(H5Tget_sign(dtid)); 
    dp->fix.order   = order;
    dp->fp.order    = NULL;
    dp->fp.format   = NULL;
    dp->members     = NULL; 

    DEBUG1("      type name: %s\n", typename);
    DEBUG1("      size: %d\n", (precision / 8));
    DEBUG1("      alignment: %d\n", _H5_get_alignment(typename));

/* does not matter which one */
    if (order != hst->pf->host_std->fx[1].order)
/*      || precision != XYZ   Not feasible since C type is unspecified */
/*      || alignment != XYZ   Not feasible since C type is unspecified */  
       {dp->convert = TRUE;
        DEBUG1("%s", "      conversions WILL be done\n");}
    else
       {dp->convert = FALSE;
        DEBUG1("%s", "      conversions will NOT be done\n");};

/* insert into the file charts */
    _PD_d_install(hst->pf, typename, dp, FALSE);

    DEBUG1("  Inserted definition for %s\n", typename);

    return(typename);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_HANDLE_FLOAT_PT - parse the binary representation of a floating
 *                     - point format from the HDF5 file
 */

static char *_H5_handle_float_pt(PDBfile *file, hid_t dtid) 
   {int i, bpif, bpid;
    short precision;
    size_t spos, epos, esize, mpos, msize;
    long *format;
    char *typename;
    defstr *dp;
    data_standard *fstd, *hstd;
    herr_t status;
    hdf_state *hst;

    hst      = file->meta;
    hstd     = hst->pf->host_std;
    fstd     = hst->pf->std;
    typename = NULL;

    dp = FMAKE(defstr, "_H5_HANDLE_FLOAT_PT:dp");

    bpif              = sizeof(float);
    fstd->fp[0].bpi   = bpif;
    fstd->fp[0].order = MAKE_N(int, bpif);

    bpid              = sizeof(double);
    fstd->fp[1].bpi   = bpid;
    fstd->fp[1].order = MAKE_N(int, bpid);

/* handle bit field
 * you never know who might use file->std in the future: fill it in
 */
    if (H5Tget_order(dtid)) 
       {DEBUG1("%s", "      big endian float\n");

        for (i = 0 ; i < bpif; i++)
            fstd->fp[0].order[i] = i + 1;

        for (i = 0 ; i < bpid; i++)
            fstd->fp[1].order[i] = i + 1;}

    else 
       {DEBUG1("%s", "      little endian float\n");

        for (i = 0 ; i < bpif; i++)
	    fstd->fp[0].order[i] = bpif - i;

        for (i = 0 ; i < bpid; i++)
            fstd->fp[1].order[i] = bpid - i;};

/* grab the precision of this float pt value */
    precision = (short) H5Tget_precision(dtid);

    if (precision == 0)
       {fprintf(stderr, "_H5_HANDLE_FLOAT_PT: error retrieving precision\n");
        return(NULL);};

/* hard-coded mapping of precision to C type name
 * this is required because HDF5 does not maintain type names
 */
    if (precision <= 32) 
       {format   = fstd->fp[0].format;
        typename = SC_strsavef(SC_FLOAT_S, "_H5_HANDLE_FLOAT_PT:typename");
        dp->fp.order = FMAKE_N(int, bpif, "_H5_HANDLE_FLOAT_PT:order");

        for (i = 0; i < bpif; i++) 
            dp->fp.order[i] = fstd->fp[0].order[i];}

    else 
       {format   = fstd->fp[1].format;
        typename = SC_strsavef(SC_DOUBLE_S, "_H5_HANDLE_FLOAT_PT:typename");
        dp->fp.order = FMAKE_N(int, bpid, "_H5_HANDLE_FLOAT_PT:order");

        for (i = 0; i < bpid; i++) 
            dp->fp.order[i] = fstd->fp[1].order[i];};

/* pdbconv.c contains information on how this is laid out
 *
 *        format[0] = # of bits per number    32
 *        format[1] = # of bits in exponent   8
 *        format[2] = # of bits in mantissa   23
 *        format[3] = start bit of sign       0    PDBlib uses
 *        format[4] = start bit of exponent   1    non-standard 
 *        format[5] = start bit of mantissa   9    numbering system
 *        format[6] = high order mantissa bit 0    for these
 *        format[7] = bias of exponent        127  
 */

    status = H5Tget_fields(dtid, &spos, &epos, &esize, &mpos, &msize);

    if (status < 0)
       {fprintf(stderr, "_H5_HANDLE_FLOAT_PT: error retrieving exp fields\n");
        return(NULL);};  /* RETURN */

    format[0] = (long) precision;  
    format[4] = format[0] - 1L - (long) epos;
    format[1] = (long) esize;
    format[4] = format[4] - (format[1] - 1L);
    format[5] = format[0] - 1L - (long) mpos;
    format[2] = (long) msize;
    format[5] = format[5] - (format[2] - 1L);
    format[7] = (long) H5Tget_ebias(dtid);

    if (format[7] == 0)
       {fprintf(stderr, "_H5_HANDLE_FLOAT_PT: error retrieving exp bias\n");
        return(NULL);};  /* RETURN */

    format[3] = 0;   /* Calculate this? */      
    format[6] = 0;   /* Calculate this? */      

    dp->fp.format = FMAKE_N(long, 8, "_H5_HANDLE_FLOAT_PT:format");

    for (i = 0; i < 8; i++)
        dp->fp.format[i] = format[i];

#if DEBUG
    DEBUG1("%s", "      format is {");

    for (i = 0; i < 8; i++)
        {DEBUG1("%ld, ", format[i]);};

    DEBUG1("%s", "}\n      float_order {");

    for (i = 0; i < bpif; i++)
        {DEBUG1("%d, ", fstd->fp[0].order[i]);};

    DEBUG1("%s", "}\n      double_order {");

    for (i = 0; i < bpid; i++)
        {DEBUG1("%d, ", fstd->fp[1].order[i]);};

    DEBUG1("%s", "}\n");
#endif

/* determine if conversion is necessary due to format or ordering
 * hard-coded mapping of precision to C type name
 * this is required because HDF5 does not maintain type names
 */
    bpif = hstd->fp[0].bpi;
    bpid = hstd->fp[1].bpi;
    if (precision <= 32) 
       {for (i = 0; i < 8; i++)
            {if (dp->fp.format[i] != hstd->fp[0].format[i])
                dp->convert = TRUE;};

        for (i = 0; i < bpif; i++)
            {if (dp->fp.order[i] != hstd->fp[0].order[i])
                dp->convert = TRUE;};}
    else 
       {for (i = 0; i < 8; i++)
            {if (dp->fp.format[i] != hstd->fp[1].format[i])
                dp->convert = TRUE;};

        for (i = 0; i < bpid; i++)
            {if (dp->fp.order[i] != hstd->fp[1].order[i])
                dp->convert = TRUE;};};

/* NOTE: as far as we know -- HDF5 does not support 1's comp */
    dp->onescmp   = FALSE;
    dp->fix.order = NO_ORDER;
    dp->type      = SC_strsavef(typename, "_H5_HANDLE_FLOAT_PT:dp->type");
    dp->size_bits = 0;  /* format[0]; */
    dp->size      = format[0] / 8;

/* GOTCHA: we have no alignment info from the file; so we guess */
    dp->alignment  = _H5_get_alignment(file, typename);

/* HDF5 files do not currently support indirection */
    dp->n_indirects = 0;
    dp->unsgned     = FALSE;
    dp->members     = NULL;

    DEBUG1("      type name: %s\n", typename);
    DEBUG1("      size: %d\n", (format[0] / 8));
    DEBUG1("      alignment: %d\n", _H5_get_alignment(typename));

/* insert into the file charts */
    _PD_d_install(hst->pf, typename, dp, FALSE);
 
    DEBUG1("  Inserted definition for %s\n", typename);
 
    return(typename);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_PARSE_DATATYPE - parse the datatype, no matter what type it is
 *                    - DTID id of datatype
 */

static char *_H5_parse_datatype(PDBfile *file, hid_t dtid) 
   {char type_class;
    char *typename;
    hid_t supertype_id;

    typename   = NULL;
    type_class = H5Tget_class(dtid);
    DEBUG1("      class %d\n", type_class);
  
    switch (type_class) 
       {case H5T_INTEGER :   /* FIXED-PT NUMBERS */
             typename = _H5_handle_fixed_pt(file, dtid);
	     break;
        case H5T_FLOAT :   /* FLOATING-PT NUMBERS */
             typename = _H5_handle_float_pt(file, dtid);
	     break;
        case H5T_COMPOUND :   /* COMPOUND TYPES */
             typename = _H5_handle_compound(file, dtid);
	     break;
        case H5T_STRING :   /* NULL-TERMINATED STRINGS */
             typename = _H5_handle_fixed_pt(file, dtid);
	     break;  
        case H5T_BITFIELD :   /* BIT FIELDS */
/* IGNORE this case? */
             break;
        case H5T_OPAQUE :   /* OPAQUE TYPES */
/* IGNORE this case? */
             break;
        case H5T_ENUM :   /* ENUMERATION TYPES */
/* IGNORE this case? */
             break;
        case H5T_VLEN :   /* VARIABLE LENGTH TYPES (Grow-able?) */
/* IGNORE this case? */
             break;
        case H5T_ARRAY  :  /* ARRAYS */
             supertype_id = H5Tget_super(dtid);
	     typename     = _H5_parse_datatype(file, supertype_id); 
	     break;
        case H5T_REFERENCE :  /* REFERENCE ? */
/* IGNORE this case? */
             break;
        case H5T_TIME :  /* DATE AND TIME ? */
/* IGNORE this case? */
             break;
        default :   /* NO SUCH TYPE CLASS */
	     DEBUG1("Unknown type class %d\n", type_class);
	     break;};

    DEBUG1("Determined datatype to be %s\n", typename);

    return (typename);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_PARSE_DIMENSIONS - read hdf.ncsa.uiuc.edu/HDF5/doc/H5.format.html 
 *                      - for documentation on parsing this layout
 */

static dimdes *_H5_parse_dimensions(PDBfile *file, hid_t dtid, hid_t dataspace_id) 
   {int rank, status, i;
    dimdes *dimensions, *dim;
    H5S_class_t scid;
    H5T_class_t tcid;
    hsize_t *dim_size;
    hid_t nativetype_id, tempType;

    dimensions = NULL;
    tcid       = H5Tget_class(dtid);

    if (tcid == H5T_ARRAY)
       {rank = H5Tget_array_ndims(dtid);

        nativetype_id = H5Tcopy(dtid);

/* get the base native type (for array types) */
        while (H5Tget_class(nativetype_id) == H5T_ARRAY) 
           {tempType = H5Tget_super(nativetype_id);
            H5Tclose(nativetype_id);
            nativetype_id = H5Tget_native_type(tempType, H5T_DIR_DESCEND);}

        if (H5Tget_class(nativetype_id) == H5T_STRING) 
           {DEBUG1("_H5_parse_dimensions: careful -- %s\n", "string array");
            rank++;}

        DEBUG1("      H5T_ARRAY rank(%d)\n", rank);
        dim_size = (hsize_t*) calloc(rank, sizeof(hsize_t));
        status   = H5Tget_array_dims(dtid, dim_size); 

        if (H5Tget_class(nativetype_id) == H5T_STRING)
           {dim_size[rank-1] = H5Tget_size(nativetype_id);}

        H5Tclose(nativetype_id);}

    else if (tcid == H5T_STRING)
       {rank = 1;

        DEBUG1("      H5T_STRING rank(%d)\n", rank);

        dim_size = (hsize_t*) calloc(rank, sizeof(hsize_t));
        dim_size[0] = H5Tget_size(dtid);}

    else 
       {scid = H5Sget_simple_extent_type(dataspace_id);

/* either we have a scalar space or an array (simple) space */
       switch(scid)
          {case H5S_SCALAR :

/* assume dimensionality info is NULL for scalars */
	        DEBUG1("%s\n", "      H5S_SCALAR");

		rank = -1;
		dim_size = NULL;
		break;

	   case H5S_SIMPLE :

/* obtain the rank of this simple data space */
		rank = H5Sget_simple_extent_ndims(dataspace_id);
   
		if (rank < 0) 
		   {fprintf(stderr, "_H5_parse_dimensions: error obtaining rank\n");
		    return(NULL);};
 
		DEBUG1("      H5S_SIMPLE rank = %d\n", rank);    

/* get the sizes of each dimension */ 
		dim_size = (hsize_t *) calloc(rank, sizeof(hsize_t)); 
		status   = H5Sget_simple_extent_dims(dataspace_id, dim_size, NULL);

		if (status < 0)
		   {fprintf(stderr, "_H5_parse_dimensions: error obtaining dims\n");
		    return(NULL);};
		break;
       
	   default :
		return(NULL);};};

/* construct the dimensionality information */
     if (rank > 0)
        {dimensions = FMAKE(dimdes, "_H5_READ_DATASPACE:dimensions");
         dim        = dimensions;}
     else 
        dimensions = NULL;

     for (i = 0; i < rank; i++) 

/* HDF5 files assume 0 based counting */
         {dim->number    = (long) dim_size[i];
          dim->index_min = 0L;
          dim->index_max = dim->number - 1L; 

          DEBUG1("      dim %d:", rank);
          DEBUG1(" 0:%ld\n", dim->index_max);    

/* are we going around again? */
         if (i+1 < rank)
            {dim->next = FMAKE(dimdes, "_H5_READ_DATASPACE:dim");
             dim = dim->next;}
         else
            dim->next = NULL;};

    return(dimensions);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_HANDLE_COMPOUND - parse the binary representation of a compound
 *                     - datatype (may recurse if necessary)
 *                     - DTID is the HDF5 ID for this datatype
 */

static char *_H5_handle_compound(PDBfile *file, hid_t dtid) 
   {int i, nm, convert;
    off_t moffs;
    char t[BUFFER_SIZE];
    char *mname, *regName;
    char *typename, *type;
    defstr *dp, *dpr;
    memdes *members, *mnxt;
    hid_t idtid;
    dimdes *dimensions;
    H5T_class_t tcl;
    hdf_state *hst;

    hst = file->meta;
    tcl = H5Tget_class(dtid);
  
    if (tcl != H5T_COMPOUND)
       return(NULL);

    regName = _H5_is_registered(file, dtid);

    if (regName == NULL)
       {typename = NULL;
        convert  = FALSE;
    
/* begin creating a defstr for the charts */
        dp = FMAKE(defstr, "_H5_HANDLE_COMPOUND:dp");
        members = FMAKE(memdes, "_H5_HANDLE_COMPOUND:members");
   
/* how many members are there? */ 
        nm = H5Tget_nmembers(dtid);
    
        if (nm <0)
           {fprintf(stderr, "_H5_HANDLE_COMPOUND: error retrieving num members\n");
            return(NULL);};
        
        DEBUG1("      # members %d\n", nm);
       
/* create a unique type name placeholder */ 
        snprintf(t, BUFFER_SIZE, "%s%d", PD_TYPE_PLACEHOLDER, hst->nstr);
        hst->nstr++;
        typename = SC_strsavef(t, "_H5_HANDLE_COMPOUND:typename");
        _H5_register(file, dtid, typename);
    
/* begin creating a memdes for the defstr */ 
        mnxt = members;
    
        for (i = 0; i < nm; i++) 

/* grab the member's name */
            {mname = H5Tget_member_name(dtid, i);
    
             if (mname == NULL)
                {fprintf(stderr, "_H5_HANDLE_COMPOUND: error retrieving member name\n");
                 return(NULL);};
    
             DEBUG1("      memb name %s\n", mname);
    
/* grab the member's offset */
             memset(&moffs, 0, sizeof(off_t)); 
             moffs = (off_t) H5Tget_member_offset(dtid, i); 
    
             if (moffs < 0)
                {fprintf(stderr, "_H5_HANDLE_COMPOUND: error retrieving member offset\n");
                 return(NULL);};
    
             DEBUG1("      offset 0x%lx\n", moffs);
    
/* grab the datatype of the internal member */
             idtid = H5Tget_member_type(dtid, i);
    
             if (idtid < 0)
                {fprintf(stderr, "_H5_HANDLE_COMPOUND: error retrieving member type\n");
                 return(NULL);};
    
             type = _H5_parse_datatype(file, idtid); 
    
             DEBUG1("      type is %s\n", type);
    
             snprintf(t, BUFFER_SIZE, "%s %s", type, mname);
            
             dimensions = _H5_parse_dimensions(file, idtid, 0); 
   
/* fill in the memdes */ 
             mnxt->member      = SC_strsavef(t, "_H5_HANDLE_COMPOUND:member");
             mnxt->cast_memb   = SC_strsavef(type, "_H5_HANDLE_COMPOUND:type"); 
             mnxt->member_offs = moffs;
             mnxt->cast_offs   = -1; 
             mnxt->type        = SC_strsavef(type, "_H5_HANDLE_COMPOUND:type");
             mnxt->base_type   = SC_strsavef(type, "_H5_HANDLE_COMPOUND:type");
             mnxt->name        = SC_strsavef(mname, "_H5_HANDLE_COMPOUND:type");
             mnxt->dimensions  = dimensions;

             if (dimensions == NULL)
                {mnxt->number = 1;}
             else
                {mnxt->number = _PD_comp_num(dimensions);};
    
/* a struct needs conversion if one of its members needs conversion */
             dpr = (defstr*) SC_hasharr_def_lookup(hst->pf->chart, type);

             if (dpr != NULL)
                {convert = convert || dpr->convert;}
             else
                {convert = FALSE;};

/* continue to link the memdes members as we loop over the entries */
             if ((i + 1) < nm)
                {mnxt->next = FMAKE(memdes, "_H5_HANDLE_COMPOUND:next"); 
                 mnxt = mnxt->next;}
             else
                {mnxt = NULL;}; 

             free(mname);
             H5Tclose(idtid);
             SFREE(type);};

#if 0
        dp->type        = SC_strsavef(typename, "_H5_HANDLE_COMPOUND:type");
        dp->size_bits   = dp->size * 8; 
        dp->size        = H5Tget_size(dtid);
/* GOTCHA: we have no alignment info from the file; so we guess */
        dp->alignment   = 4;
        dp->n_indirects = 0;
        dp->onescmp     = FALSE;
        dp->unsgned     = FALSE;
        dp->order_flag  = NO_ORDER;
        dp->order       = NULL;
        dp->format      = NULL;
        dp->members     = members;
#endif
   
/* after fully parsing the members, construct the defstr */
        _PD_defstr_inst(hst->pf, typename, STRUCT_KIND, members,
			NO_ORDER, NULL, NULL, FALSE);

#if 0
        dp = _PD_mk_defstr(NULL, typename,
				     members, NULL,
				     H5Tget_size(dtid), 0, -1, FALSE, 
				     NULL, NULL, FALSE, FALSE);

        DEBUG1("      type name %s\n", typename);
        DEBUG1("      size %ld\n", (long) H5Tget_size(dtid));
        DEBUG1("      alignment %d\n", 4);

        _PD_d_install(hst->pf, typename, dp, TRUE);
    
/* make a copy of dp for the file charts */
        host_entry = FMAKE(defstr, "_H5_HANDLE_COMPOUND:host_entry");
        memcpy(host_entry, dp, sizeof(defstr));
    
        host_entry->type    = SC_strsavef(dp->type, "_H5_HANDLE_COMPOUND:type");
        host_entry->members = PD_copy_members(dp->members);
    
/* set convert just before adding to the file charts */
        host_entry->convert = convert; 
        _PD_d_install(hst->pf, typename, host_entry, FALSE);
    
        DEBUG1("Inserted defstr for %s in both charts\n", typename); 
#endif
       }

    else
       {DEBUG1("Compound type for %s already defined\n", regName);
        typename = SC_strsavef(regName, "_H5_HANDLE_COMPOUND:typename");};

    return(typename);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* H5_READ_GROUP_NODE - depth first search through group data objects 
 *                    - parsing data as we go 
 */

static herr_t H5_read_group_node(hid_t group_id, const char *mname,
				 void *a)
   {int nc;
    BIGINT addr;
    char *typename, *prev_group, *temp_prefix, *fullname;
    herr_t status;
    hid_t  dt_id;
    hid_t  dp_id; 
    hid_t  dtid;
    syment *ep;
    dimdes *dimensions, *dim;
    H5G_stat_t statbuf;
    PDBfile *file;
    hdf_state *hst;

    file       = (PDBfile *) a;
    hst        = file->meta;
    status     = (herr_t) 0;
    typename   = NULL; 
    prev_group = hst->group_prefix;
    dimensions = NULL;

    ep = FMAKE(syment, "_H5_READ_GROUP_NODE:ep");

    DEBUG1("ENTERING read group node for: %s\n", mname);

/* get the stats on this object */
    status = H5Gget_objinfo(group_id, mname, FALSE, &statbuf);

/* ignore directory entries for "." and ".." */
    if ((strcmp(mname, "..") == 0) || (strcmp(mname, ".") == 0)) 
        return(0);

    if (status < 0)
       {fprintf(stderr, "H5_READ_GROUP_NODE: error getting obj stats\n");
        return(-1);};

    switch (statbuf.type)
       {case H5G_GROUP :
             DEBUG1("Parsing H5G_GROUP: %s\n", mname);

/* append to group prefix */
	     nc = strlen(hst->group_prefix) + strlen(mname) + 2,
	     temp_prefix = FMAKE_N(char, nc, "H5_READ_GROUP_NODE:temp_prefix");
	     temp_prefix[0] = '\0';

/* end it in slash '/' */
	     SC_vstrcat(temp_prefix, nc, "%s%s/",
			hst->group_prefix,             /* ends in slash '/' */
			(char *) mname);           /* append new name   */

	     hst->group_prefix = temp_prefix;

	     ep->type = SC_strsavef("Directory", "H5_READ_GROUP_NODE:type");
	     ep->number               = 1; 
	     ep->dimensions           = NULL; 
	     ep->indirects.addr       = 0;
	     ep->indirects.n_ind_type = 0;
	     ep->indirects.arr_offs   = 0;

	     _PD_block_init(ep, TRUE);
	     _PD_entry_set_address(ep, 0, 0);
	     _PD_entry_set_number(ep, 0, 0);
               
	     _PD_e_install(hst->pf, hst->group_prefix, ep, FALSE);

	     DEBUG1("    Inserted syment for %s", hst->group_prefix);
	     DEBUG1(" with type %s\n", "Directory");   

/* recurse */
	     status = H5Giterate(group_id, mname, NULL,
				 &H5_read_group_node, a);

/* return the group prefix to what it was before */
	     hst->group_prefix = prev_group;
	     SFREE(temp_prefix);
	     break;
 
        case H5G_DATASET :
             DEBUG1("Parsing H5G_DATASET: %s\n", mname);

	     dt_id   = H5Dopen(group_id, mname, H5P_DEFAULT);
	     dp_id = H5Dget_space(dt_id);
	     dtid  = H5Dget_type(dt_id);    
	     addr         = H5Dget_offset(dt_id);
             if (addr < 0)
	        {printf("   Group member %s had no address\n", mname);
		 return(-1);};

	     dimensions = _H5_parse_dimensions(file, dtid, dp_id);
	     typename   = _H5_parse_datatype(file, dtid); 

	     DEBUG1("TYPE is %s\n", typename);
 
/* create another entry in the symbol table */
	     ep->type = SC_strsavef(typename, "H5_READ_GROUP_NODE:type");

/* setup the dimensionality */
	     if (dimensions != NULL)
	        {ep->number = dimensions->number;

		 for (dim = dimensions->next; dim != NULL; dim = dim->next)
		     ep->number = ep->number * dim->number;

		 ep->dimensions = dimensions;} 
	     else 
	        {ep->number     = 1;
		 ep->dimensions = NULL;};

/* setup the symbols mapping to a disk addr */
	     _PD_block_init(ep, TRUE);
	     _PD_entry_set_address(ep, 0, addr);
	     _PD_entry_set_number(ep, 0, ep->number);

	     DEBUG1("        data addr: %lx\n", ep->blocks->diskaddr);

	     ep->indirects.addr       = 0;
	     ep->indirects.n_ind_type = 0;
	     ep->indirects.arr_offs   = 0;
	     
	     nc = strlen(hst->group_prefix) + strlen(mname) + 1;
	     fullname = FMAKE_N(char, nc, "H5_READ_GROUP_NODE:fullname");
	     fullname[0] = '\0';
	     SC_vstrcat(fullname, nc, "%s%s",
			hst->group_prefix, (char *) mname); 

	     _PD_e_install(hst->pf, fullname, ep, FALSE);

	     DEBUG1("    Inserted syment for %s", fullname);
	     DEBUG1(" with type %s\n", typename);   

/* free up some space */
	     H5Tclose(dtid);
	     H5Sclose(dp_id);
	     H5Dclose(dt_id);
	     SFREE(fullname);
	     SFREE(typename);
	     break;

        case H5G_LINK :
             DEBUG1("Ignoring H5G_LINK: %s\n", mname);
/* GOTCHA: do nothing: DO NOT FOLLOW LINKS */
	     break;

        case H5G_TYPE :
             DEBUG1("Ignoring H5G_TYPE: %s\n", mname);
	     break;

        default :
             fprintf(stderr, "UNKNOWN NODE TYPE %d - H5_READ_GROUP_NODE\n",
		     statbuf.type);
	     return(-1);};
 
    DEBUG1("EXITING read group node for %s\n", mname);

    return(status);}

/*--------------------------------------------------------------------------*/

/*               PUBLIC HDF5 SPOKE FUNCTION DEFINITIONS                     */

/*--------------------------------------------------------------------------*/

/* _H5_FILEP - return TRUE iff TYPE is an HDF5 file */

static int _H5_filep(char *type)
   {int rv;

    rv = (strcmp(type, H5FILE_S) == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_CLOSE - close the PDB file 
 *           - FILE a valid PDB file to close
 */

static int _H5_close(PDBfile *file)
   {FILE *fp;

    SFREE(file->meta);

/* this closes the FILE* stream shared by both hdf_file and file */
    fp = file->stream; 
    if (lio_close(fp) != 0)
       PD_error("CANNOT CLOSE FILE - _H5_CLOSE", PD_CLOSE);

/* free the space */
    _PD_rl_pdb(file);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_CREATE - create an HDF5 file
 *            - NAME the absolute pathname of the HDF5 file to create
 */

static PDBfile *_H5_create(tr_layer *tr, SC_udl *pu, char *name, void *a)
   {int status, depth;
    syment *ep;
    PDBfile *file;
    PD_smp_state *pa;
    hdf_state *hst;

    pa = _PD_get_state(-1);

    DEBUG1("_H5_create called: name(%s)\n", name);

/* setup any global variables we will not read from the file */
    hst = FMAKE(hdf_state, "_H5_OPEN:hst");
    hst->nstr         = 0;
    hst->registry     = NULL;
    hst->group_prefix = NULL;

/* setup any local variables */
    status = 0;
    depth  = 0;
    file   = NULL;

/* turn off the un-intelligent H5E print subsystem */
    H5Eset_auto(H5E_DEFAULT, NULL, stderr);

/* make sure it really is an HDF5 file */
    hst->hf = H5Fcreate(name, H5F_ACC_RDWR, H5P_DEFAULT, H5P_DEFAULT);

/* return if this is not a valid HDF5 file */
    if (hst->hf < 0)
       return(NULL);

    file = _PD_mk_pdb(pu, "temp.pdb", NULL, TRUE, &_H5_sys, tr);
    if (file == NULL)
       return(NULL);

    hst->pf = file;

/* fill in the host's std with default values */
    file->host_std   = _PD_copy_standard(pa->int_std);
    file->host_align = _PD_copy_alignment(pa->int_align);

/* insert entries in the host charts (both) since those stds are now known */
    _PD_setup_chart(file->host_chart, file->host_std, NULL, 
                    file->host_align, NULL, FALSE, TRUE);

/* assume the file and host standards are the same until
 * we find out otherwise (and mutate it at some later point)
 */
    file->std              = _PD_copy_standard(file->host_std);
    file->align            = _PD_copy_alignment(file->host_align); 
    file->mode             = PD_APPEND; 
    file->virtual_internal = FALSE; 
    file->type             = SC_strsavef(H5FILE_S, "char*:_H5_CREATE:type");
    file->current_prefix   = SC_strsavef("/", "char*:_H5_CREATE:current_prefix");

/* init the group/directory prefix to the root dir */
    hst->group_prefix = SC_strsavef("/", "char*:_H5_CREATE:group_prefix");

    ep = FMAKE(syment, "_H5_CREATE:ep");
    ep->type = SC_strsavef("Directory", "_H5_CREATE:type");
    ep->number               = 1; 
    ep->dimensions           = NULL; 
    ep->indirects.addr       = 0;
    ep->indirects.n_ind_type = 0;
    ep->indirects.arr_offs   = 0;

    _PD_block_init(ep, TRUE);
    _PD_entry_set_address(ep, 0, 0);
    _PD_entry_set_number(ep, 0, 0);
         
    _PD_e_install(file, hst->group_prefix, ep, FALSE);

    DEBUG1("    Inserted syment for %s", hst->group_prefix);
    DEBUG1(" with type %s\n", "Directory");   

/*
 * stuff goes here
    hdf_root_group = H5Gopen(hst->hf, "/", H5P_DEFAULT);
    status = H5Giterate(hst->hf, "/", NULL, &H5_read_group_node, file);
    H5Gclose(hdf_root_group);
*/

/* if we were unsuccessful, alert the user */
    if (status < 0) 
       {fprintf(stderr, "ERROR CREATING HDF5 FILE %s - _H5_CREATE\n", name);
        _H5_close(file);
	file = NULL;};

    SFREE(hst->group_prefix);

    return(file);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_OPEN - open an existing HDF5 File and translate it to an PDBfile 
 *          - NAME the absolute pathname of the HDF5 file to open
 */

static PDBfile *_H5_open(tr_layer *tr, SC_udl *pu, char *name, char *mode)
   {int status, depth;
    hid_t hdf_root_group;
    syment *ep;
    PDBfile *file;
    PD_smp_state *pa;
    hdf_state *hst;

    pa = _PD_get_state(-1);

    DEBUG1("_H5_open called: name(%s)\n", name);

/* setup any global variables we will not read from the file */
    hst = FMAKE(hdf_state, "_H5_OPEN:hst");
    hst->nstr         = 0;
    hst->registry     = NULL;
    hst->group_prefix = NULL;

/* setup any local variables */
    status = 0;
    depth  = 0;
    file   = NULL;

/* turn off the un-intelligent H5E print subsystem */
    H5Eset_auto(H5E_DEFAULT, NULL, stderr);

/* make sure it really is an HDF5 file */
    hst->hf = H5Fopen(name, H5F_ACC_RDONLY, H5P_DEFAULT);

/* return if this is not a valid HDF5 file */
    if (hst->hf < 0)
       return(NULL);

    file = _PD_mk_pdb(pu, "temp.pdb", mode, TRUE, &_H5_sys, tr);
    if (file == NULL)
       return(NULL);

    hst->pf    = file;
    file->meta = hst;

/* fill in the host's std with default values */
    file->host_std   = _PD_copy_standard(pa->int_std);
    file->host_align = _PD_copy_alignment(pa->int_align);

/* insert entries in the host charts (both) since those stds are now known */
    _PD_setup_chart(file->host_chart, file->host_std, NULL, 
                    file->host_align, NULL, FALSE, TRUE);

/* assume the file and host standards are the same until
 * we find out otherwise (and mutate it at some later point)
 */
    file->std              = _PD_copy_standard( file->host_std);
    file->align            = _PD_copy_alignment(file->host_align); 
    file->mode             = PD_APPEND; 
    file->virtual_internal = FALSE; 
    file->type             = SC_strsavef(H5FILE_S, "char*:_H5_OPEN:type");
    file->current_prefix   = SC_strsavef("/", "char*:_H5_OPEN:current_prefix");

/* init the group/directory prefix to the root dir */
    hst->group_prefix = SC_strsavef("/", "char*:_H5_OPEN:group_prefix");

    ep = FMAKE(syment, "_H5_OPEN:ep");
    ep->type                 = SC_strsavef("Directory", "_H5_OPEN:type");
    ep->number               = 1; 
    ep->dimensions           = NULL; 
    ep->indirects.addr       = 0;
    ep->indirects.n_ind_type = 0;
    ep->indirects.arr_offs   = 0;

    _PD_block_init(ep, TRUE);
    _PD_entry_set_address(ep, 0, 0);
    _PD_entry_set_number(ep, 0, 0);
         
    _PD_e_install(file, hst->group_prefix, ep, FALSE);

    DEBUG1("    Inserted syment for %s", hst->group_prefix);
    DEBUG1(" with type %s\n", "Directory");   

/* parse the entire HDF5 file
 * insert entries in the charts as we encounter types
 * insert entries in the symbol table as we encounter the objects
 */
    hdf_root_group = H5Gopen(hst->hf, "/", H5P_DEFAULT);
    status = H5Giterate(hst->hf, "/", NULL, &H5_read_group_node, file);
    H5Gclose(hdf_root_group);

/* if we were unsuccessful, alert the user */
    if (status < 0) 
       {_H5_close(file);
	file = NULL;};

    SFREE(hst->group_prefix);

    return(file);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_RD_SYMENT - read syment method for HDF5 spoke */

long _H5_rd_syment(PDBfile *file, syment *ep, char *outtype, void *vr)
   {long rv;

    rv = _PD_rd_syment(file, ep, outtype, vr);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_WR_SYMENT - read syment method for HDF5 spoke */

long _H5_wr_syment(PDBfile *file, char *vr, long nitems,
		   char *intype, char *outtype)
   {long rv;

    rv = _PD_wr_syment(file, vr, nitems, intype, outtype);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_READ_ENTRY - read entry method for HDF5 spoke */

int _H5_read_entry(PDBfile *fp, char *path, char *ty, syment *ep, void *vr)
   {int rv;

    rv = _PD_hyper_read(fp, path, ty, ep, vr);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_WRITE_ENTRY - write entry method for HDF5 spoke */

syment *_H5_write_entry(PDBfile *fp, char *path, char *inty, char *outty,
			void *vr, dimdes *dims)
   {int new;
    syment *ep;

    ep = fp->tr->write(fp, path, inty, outty, vr, dims, FALSE, &new);

    if ((ep != NULL) && (new == TRUE))
       ep = PD_copy_syment(ep);

    return(ep);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PD_REGISTER_HDF5 - install the HDF5 extensions to PDBLib */
 
void PD_register_hdf5(void)
   {int n;

    n = PD_REGISTER(H5FILE_S, "hdf5", _H5_filep,
		    _H5_create, _H5_open, _H5_close,
		    _H5_write_entry, _H5_read_entry);

    return;}
 
/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/
 
/* PD_REGISTER_HDF5 - install the HDF5 extensions to PDBLib */
 
void PD_register_hdf5(void)
   {

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

