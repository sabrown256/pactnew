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
 *H5FILE_S = "HDF5file";        /* identifier string */

static hid_t
 _H5_enc_compound(PDBfile *file, defstr *dp, intb nb);

static char
 *_H5_dec_compound(PDBfile *file, hid_t htyp);

static long
 _H5_rd_syment(PDBfile *file, syment *ep, char *outtype, void *vr),
 _H5_wr_syment(PDBfile *file, char *vr, long ni,
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
    char *verdict, *mname;
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
                {mname = H5Tget_member_name(dtid, i);
                
		 if ((strcmp(info->member_name, mname) != 0) ||
                     (info->member_offset != H5Tget_member_offset(dtid, i)))
                    {matches = FALSE; 
		     free(mname);
		     break;}; 

		 free(mname);
                
		 info = info->next;};

/* only if everything matches do we assume this is the one */
	    if (matches)
               verdict = iter->compound_name;
	    else
	       iter = iter->next;}
       else
	  iter = iter->next;};

    return(verdict);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_REGISTER - register the compound datatype with us        
 *              - HFP is the HDF5 handle to the datatype
 *              - TYPE null-terminated string naming the type
 */

static void _H5_register(PDBfile *file, hid_t hfp, char *type)
   {int i;
    char *verdict, *mname;
    compound_desc *iter, *prev;
    compound_member_info *info;
    hdf_state *hst;

    prev    = NULL;
    hst     = file->meta;
    iter    = hst->registry; 
    verdict = _H5_is_registered(file, hfp);

/* do not register the compound type if it is already registered */
    if (verdict == NULL)

/* find the last position in the linked list */
       {while (iter != NULL) 
           {prev = iter;
            iter = iter->next;};

/* grow a compound_desc there (or at registry itself if it is empty) */
        if (prev == NULL)
           {hst->registry = CMAKE(compound_desc); 
            iter          = hst->registry;}
        else 
           {prev->next = CMAKE(compound_desc);  
            iter       = prev->next;};

/* insert the data into the new compound_desc */
        iter->compound_name = CSTRSAVE(type);
        iter->num_members   = H5Tget_nmembers(hfp);

/* link in all of the compound member info objects */
        iter->info = CMAKE(compound_member_info);
        info = iter->info;

        for (i = 0 ; i < iter->num_members ; i++)
            {mname = H5Tget_member_name(hfp, i);

             info->member_offset = H5Tget_member_offset(hfp, i); 
             info->member_name   = CSTRSAVE(mname);

             free(mname);

             if ((i+1) < iter->num_members)
                info->next = CMAKE(compound_member_info);
             else
                info->next = NULL;
             
             info = info->next;};

        iter->next = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_GET_ALIGNMENT - return the alignment for type TYPE */

static int _H5_get_alignment(PDBfile *file, char *type)
   {int id, result;
    static int algn[N_PRIMITIVES] = { 0, 0, 0, 0,
                                      2, 4, 4, 4,  /* fixed point types (ok) */
                                      4, 4, 4,     /* floating point types (ok) */
                                      0, 0, 0,     /* complex floating point types (ok) */
                                      0, 0 };

    id     = SC_type_id(type, TRUE);
    result = algn[id];

    return(result);}

/*--------------------------------------------------------------------------*/

/*                             WRITE ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* _H5_ENC_TYPE - encode the PDB type into HDF form
 *              - no matter what type it is
 *              - PTYP name of type
 */

static hid_t _H5_enc_type(PDBfile *file, char *ptyp)
   {int tid;
    intb nb;
    hid_t htyp;
    defstr *dp;

    tid = SC_type_id(ptyp, FALSE);

    if (tid == SC_SHORT_I)
       htyp = H5T_NATIVE_SHORT;

    else if (tid == SC_INT_I)
       htyp = H5T_NATIVE_INT;

    else if (tid == SC_LONG_I)
       htyp = H5T_NATIVE_LONG;

    else if (tid == SC_LONG_LONG_I)
       htyp = H5T_NATIVE_LLONG;

    else if (tid == SC_FLOAT_I)
       htyp = H5T_NATIVE_FLOAT;

    else if (tid == SC_DOUBLE_I)
       htyp = H5T_NATIVE_DOUBLE;

    else if (tid == SC_LONG_DOUBLE_I)
       htyp = H5T_NATIVE_LDOUBLE;

    else if (tid == SC_CHAR_I)
       htyp = H5T_NATIVE_SCHAR;

    else if (tid == SC_BOOL_I)
       htyp = H5T_NATIVE_HBOOL;

    else
       {dp = PD_inquire_host_type(file, ptyp);
	if (dp != NULL)
	   {nb   = _PD_lookup_size(ptyp, file->host_chart);
	    htyp = _H5_enc_compound(file, dp, nb);}
        else
	   {DEBUG1("Unknown type %d\n", tid);};};

#if 0
        case H5T_STRING :   /* NULL-TERMINATED STRINGS */
             typename = _H5_handle_fixed_pt(file, htyp);
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
	     hid_t supertype_id;
             supertype_id = H5Tget_super(htyp);
	     typename     = _H5_enc_type(file, supertype_id); 
	     break;
        case H5T_REFERENCE :  /* REFERENCE ? */
/* IGNORE this case? */
             break;
        case H5T_TIME :  /* DATE AND TIME ? */
/* IGNORE this case? */
             break;
#endif

    return(htyp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_ENC_COMPOUND - encode the PDB representation of a struct into HDF form
 *                  - may recurse if necessary
 *                  - DP is the PDB type
 */

static hid_t _H5_enc_compound(PDBfile *file, defstr *dp, intb nb)
   {memdes *mbr;
    hid_t mtyp, htyp;

    htyp = -1;

    if ((dp != NULL) && (dp->members != NULL) &&
	(PD_type_n_indirects(dp) == 0))
       {htyp = H5Tcreate(H5T_COMPOUND, nb);

	for (mbr = dp->members; mbr != NULL ; mbr = mbr->next)
	    {mtyp = _H5_enc_type(file, mbr->type);
	     H5Tinsert(htyp, mbr->name, mbr->member_offs, mtyp);};

	_H5_register(file, htyp, dp->type);};

    return(htyp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_ENC_DIMS - encode PDB dimensions in HDF form */

static hsize_t *_H5_enc_dims(PDBfile *file, int *pnd,
			     hid_t htyp, dimdes *pdims, inti ni)
   {int nd, status;
    H5T_class_t tcid;
    hsize_t *hdims;

    hdims = NULL;
    tcid  = H5Tget_class(htyp);

    if (tcid == H5T_ARRAY)
       {hid_t nativetype_id, tempType;

        nd = H5Tget_array_ndims(htyp);

        nativetype_id = H5Tcopy(htyp);

/* get the base native type (for array types) */
        while (H5Tget_class(nativetype_id) == H5T_ARRAY) 
           {tempType = H5Tget_super(nativetype_id);
            H5Tclose(nativetype_id);
            nativetype_id = H5Tget_native_type(tempType, H5T_DIR_DESCEND);}

        if (H5Tget_class(nativetype_id) == H5T_STRING) 
           {DEBUG1("_H5_enc_dims: careful -- %s\n", "string array");
            nd++;}

        DEBUG1("      H5T_ARRAY rank(%d)\n", nd);
        hdims = (hsize_t*) calloc(nd, sizeof(hsize_t));

#if (H5_VERS_RELEASE < 4)
        status = H5Tget_array_dims(htyp, hdims); 
#else
	status = H5Tget_array_dims(htyp, hdims, NULL);
#endif
        SC_ASSERT(status != -1);

        if (H5Tget_class(nativetype_id) == H5T_STRING)
           hdims[nd-1] = H5Tget_size(nativetype_id);

        H5Tclose(nativetype_id);}

    else if (tcid == H5T_STRING)
       {DEBUG1("      H5T_STRING nd(%d)\n", nd);

        nd       = 1;
        hdims    = (hsize_t*) calloc(nd, sizeof(hsize_t));
        hdims[0] = H5Tget_size(htyp);}

    else 
       {long id;
	dimdes *lst;

	if (ni > 0)
	   {nd       = 1;
	    hdims    = (hsize_t *) calloc(nd, sizeof(hsize_t)); 
	    hdims[0] = ni;}

        else
	   {for (nd = 0, lst = pdims; lst != NULL; lst = lst->next, nd++);

	    if (nd == 0)
	       {nd       = 1;
		hdims    = (hsize_t *) calloc(nd, sizeof(hsize_t)); 
		hdims[0] = 1;}
	    else
	       {hdims = (hsize_t *) calloc(nd, sizeof(hsize_t)); 

		for (id = 0, lst = pdims; lst != NULL; lst = lst->next, id++)
		    hdims[id] = (long) lst->number;};};};

    if (pnd != NULL)
       *pnd = nd;

    return(hdims);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_WRITE_DATA - after H5LT_make_dataset_numerical
 *                - return TRUE iff successful
 */

int _H5_write_data(hid_t fid, char *fullpath,
		   int nd, hsize_t *dims,
		   hid_t htyp, void *vr)
   {int rv, st;
    hid_t did, sid;

    rv = TRUE;

    sid = H5Screate_simple(nd, dims, NULL);
    if (sid < 0)
       rv = FALSE;

    else
       {did = H5Dcreate2(fid, fullpath, htyp, sid,
			 H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	if (did < 0)
	   rv = FALSE;

	else if (vr != NULL)
	   {st = H5Dwrite(did, htyp, H5S_ALL, H5S_ALL, H5P_DEFAULT, vr);
	    rv = (st >= 0);};

	if (H5Dclose(did) < 0)
	   rv = FALSE;

	else if (H5Sclose(sid) < 0)
	   rv = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_WRITE_ENTRY - write entry method for HDF5 spoke */

syment *_H5_write_entry(PDBfile *fp, char *path, char *inty, char *outty,
			void *vr, dimdes *pdims)
   {int rv, nd, ptr;
    inti ni;
    void *pvr;
    hid_t hfp, htyp;
    hsize_t *hdims;
    syment *ep;
    hdf_state *hst;

    ep = NULL;

    hst = fp->meta;
    hfp = hst->hf;

    ptr = SC_is_type_ptr_a(outty);

/* dereference pointers - only scalar pointers will ultimately succeed */
    if (ptr == TRUE)
       {char pty[MAXLINE];

	SC_strncpy(pty, MAXLINE, outty, -1);
	SC_dereference(pty);

	pvr   = DEREF(vr);
	ni    = SC_arrlen(pvr)/SC_type_size_a(pty);
        htyp  = _H5_enc_type(fp, pty);
	hdims = _H5_enc_dims(fp, &nd, htyp, NULL, ni);}
        
/* handle non pointered data */
    else
       {htyp  = _H5_enc_type(fp, outty);
	hdims = _H5_enc_dims(fp, &nd, htyp, pdims, 0);
        pvr   = vr;};

    ni = _PD_comp_num(pdims);
    ep = _PD_mk_syment(outty, ni, 0, NULL, pdims);

/* complain about pointers that cannot be mapped into arrays */
    if ((ptr == TRUE) && (pdims != NULL))
       printf("Cannot represent '%s[%lld]' of type '%s' to HDF\n",
	      path, (long long) ni, outty);
    else
       {rv = _H5_write_data(hfp, path, nd, hdims, htyp, pvr);
	SC_ASSERT(rv == TRUE);};

    return(ep);}

/*--------------------------------------------------------------------------*/

/*                              READ ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* _H5_DEC_FIXED_PT - decode HDF binary representation of a fixed   
 *                  - point format into PDB form
 */

static char *_H5_dec_fixed_pt(PDBfile *file, hid_t htyp) 
   {int i;
    short precision;
    char *typ;
    PD_byte_order order;
    defstr *dp;
    hdf_state *hst;

    hst = file->meta;
    typ = NULL;
    dp  = CMAKE(defstr);

    precision = (short) H5Tget_precision(htyp);

/* HDF5 assumes all strings are chars with a precision of 8 bits */
    if (H5Tget_class(htyp) == H5T_STRING) 
       precision = 8;

    DEBUG1("      precision %d\n", precision);

/* hard-coded mapping of precision to C type name
 * this is required because HDF5 does not maintain type names
 */
    if (H5Tget_sign(htyp)) 
       {if (precision == 8)
           typ = CSTRSAVE(SC_CHAR_S);
        else if (precision == 16)
           typ = CSTRSAVE(SC_SHORT_S);
        else if (precision == 32)
           typ = CSTRSAVE(SC_INT_S);
        else if (precision == 64)
           typ = CSTRSAVE(SC_LONG_S);
        else if (precision >  64)
           typ = CSTRSAVE(SC_LONG_LONG_S);
        else
           typ = CSTRSAVE("UNKNOWN");}
    else
       {if (precision == 8)
           typ = CSTRSAVE("u_char");
        else if (precision == 16)
           typ = CSTRSAVE("u_short");
        else if (precision == 32)
           typ = CSTRSAVE("u_int");
        else if (precision == 64)
           typ = CSTRSAVE("u_long");
        else if (precision >  64)
           typ = CSTRSAVE("u_long_long");
        else
           typ = CSTRSAVE("UNKNOWN");};

/* decode byte ordering */ 
    if (H5Tget_order(htyp) == H5T_ORDER_BE) 
       {order = NORMAL_ORDER;
        DEBUG1("%s", "      big endian integers\n");}
    else 
       {order = REVERSE_ORDER;
        DEBUG1("%s", "      little endian integers\n");};

/* you never know who might use file->std in the future: fill it in */
    for (i = 0; i < N_PRIMITIVE_FIX; i++)
        hst->pf->std->fx[i].order = order; 

/* create a defstr with this information
 * NOTE: as far as we know -- HDF5 does not support 1's comp
 */
    dp->onescmp   = FALSE;
    dp->type      = CSTRSAVE(typ);
    dp->size_bits = 0;
    dp->size      = precision / 8;

/* GOTCHA: we have no alignment info from the file; so we guess */
    dp->alignment   = _H5_get_alignment(file, typ);

/* HDF5 files do not currently support indirection */
    dp->n_indirects = 0;
    dp->unsgned     = !(H5Tget_sign(htyp)); 
    dp->fix.order   = order;
    dp->fp.order    = NULL;
    dp->fp.format   = NULL;
    dp->members     = NULL; 

    DEBUG1("      type name: %s\n", typ);
    DEBUG1("      size: %d\n", (precision / 8));
    DEBUG1("      alignment: %d\n", _H5_get_alignment(typ));

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
    _PD_d_install(hst->pf, typ, dp, PD_CHART_FILE);

    DEBUG1("  Inserted definition for %s\n", typ);

    return(typ);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_DEC_FLOAT_PT - decode the HDF binary representation of a floating
 *                  - point format into PDB form
 */

static char *_H5_dec_float_pt(PDBfile *file, hid_t htyp) 
   {int i;
    intb bpif, bpid;
    short precision;
    size_t spos, epos, esize, mpos, msize;
    long *format;
    char *typ;
    defstr *dp;
    data_standard *fstd, *hstd;
    herr_t status;
    hdf_state *hst;

    hst  = file->meta;
    hstd = hst->pf->host_std;
    fstd = hst->pf->std;
    typ  = NULL;

    dp = CMAKE(defstr);

    bpif              = sizeof(float);
    fstd->fp[0].bpi   = bpif;
    fstd->fp[0].order = CMAKE_N(int, bpif);

    bpid              = sizeof(double);
    fstd->fp[1].bpi   = bpid;
    fstd->fp[1].order = CMAKE_N(int, bpid);

/* decode bit field
 * you never know who might use file->std in the future: fill it in
 */
    if (H5Tget_order(htyp)) 
       {DEBUG1("%s", "      big endian float\n");

        for (i = 0; i < bpif; i++)
            fstd->fp[0].order[i] = i + 1;

        for (i = 0; i < bpid; i++)
            fstd->fp[1].order[i] = i + 1;}

    else 
       {DEBUG1("%s", "      little endian float\n");

        for (i = 0; i < bpif; i++)
	    fstd->fp[0].order[i] = bpif - i;

        for (i = 0; i < bpid; i++)
            fstd->fp[1].order[i] = bpid - i;};

/* grab the precision of this float pt value */
    precision = (short) H5Tget_precision(htyp);

    if (precision == 0)
       {fprintf(stderr, "_H5_DEC_FLOAT_PT: error retrieving precision\n");
        return(NULL);};

/* hard-coded mapping of precision to C type name
 * this is required because HDF5 does not maintain type names
 */
    if (precision <= 32) 
       {format       = fstd->fp[0].format;
        typ          = CSTRSAVE(SC_FLOAT_S);
        dp->fp.order = CMAKE_N(int, bpif);

        for (i = 0; i < bpif; i++) 
            dp->fp.order[i] = fstd->fp[0].order[i];}

    else 
       {format       = fstd->fp[1].format;
        typ          = CSTRSAVE(SC_DOUBLE_S);
        dp->fp.order = CMAKE_N(int, bpid);

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

    status = H5Tget_fields(htyp, &spos, &epos, &esize, &mpos, &msize);

    if (status < 0)
       {fprintf(stderr, "_H5_DEC_FLOAT_PT: error retrieving exp fields\n");
        return(NULL);};  /* RETURN */

    format[0] = (long) precision;  
    format[4] = format[0] - 1L - (long) epos;
    format[1] = (long) esize;
    format[4] = format[4] - (format[1] - 1L);
    format[5] = format[0] - 1L - (long) mpos;
    format[2] = (long) msize;
    format[5] = format[5] - (format[2] - 1L);
    format[7] = (long) H5Tget_ebias(htyp);

    if (format[7] == 0)
       {fprintf(stderr, "_H5_DEC_FLOAT_PT: error retrieving exp bias\n");
        return(NULL);};  /* RETURN */

    format[3] = 0;   /* Calculate this? */      
    format[6] = 0;   /* Calculate this? */      

    dp->fp.format = CMAKE_N(long, 8);

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
    dp->type      = CSTRSAVE(typ);
    dp->size_bits = 0;  /* format[0]; */
    dp->size      = format[0] / 8;

/* GOTCHA: we have no alignment info from the file; so we guess */
    dp->alignment  = _H5_get_alignment(file, typ);

/* HDF5 files do not currently support indirection */
    dp->n_indirects = 0;
    dp->unsgned     = FALSE;
    dp->members     = NULL;

    DEBUG1("      type name: %s\n", typ);
    DEBUG1("      size: %d\n", (format[0] / 8));
    DEBUG1("      alignment: %d\n", _H5_get_alignment(typ));

/* insert into the file charts */
    _PD_d_install(hst->pf, typ, dp, PD_CHART_FILE);
 
    DEBUG1("  Inserted definition for %s\n", typ);
 
    return(typ);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_DEC_TYPE - decode HDF type into PDB form
 *              - no matter what type it is
 *              - HTYP id of type
 */

static char *_H5_dec_type(PDBfile *file, hid_t htyp) 
   {char tcls;
    char *typ;
    hid_t styp;

    typ  = NULL;
    tcls = H5Tget_class(htyp);
    DEBUG1("      class %d\n", tcls);
  
    switch (tcls) 
       {case H5T_INTEGER :   /* FIXED-PT NUMBERS */
             typ = _H5_dec_fixed_pt(file, htyp);
	     break;
        case H5T_FLOAT :   /* FLOATING-PT NUMBERS */
             typ = _H5_dec_float_pt(file, htyp);
	     break;
        case H5T_COMPOUND :   /* COMPOUND TYPES */
             typ = _H5_dec_compound(file, htyp);
	     break;
        case H5T_STRING :   /* NULL-TERMINATED STRINGS */
             typ = _H5_dec_fixed_pt(file, htyp);
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
             styp = H5Tget_super(htyp);
	     typ  = _H5_dec_type(file, styp); 
	     break;
        case H5T_REFERENCE :  /* REFERENCE ? */
/* IGNORE this case? */
             break;
        case H5T_TIME :  /* DATE AND TIME ? */
/* IGNORE this case? */
             break;
        default :   /* NO SUCH TYPE CLASS */
	     DEBUG1("Unknown type class %d\n", tcls);
	     break;};

    DEBUG1("Determined type to be %s\n", typ);

    return(typ);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_DEC_DIMS - decode HDF dimension into PDB form
 *              - read hdf.ncsa.uiuc.edu/HDF5/doc/H5.format.html 
 *              - for documentation on parsing this layout
 */

static dimdes *_H5_dec_dims(PDBfile *file, hid_t htyp, hid_t hdim) 
   {int nd, status, i;
    dimdes *pdims, *dim;
    H5S_class_t scid;
    H5T_class_t tcid;
    hsize_t *hdims;
    hid_t nativetype_id, tempType;

    pdims = NULL;
    tcid  = H5Tget_class(htyp);

    if (tcid == H5T_ARRAY)
       {nd = H5Tget_array_ndims(htyp);

        nativetype_id = H5Tcopy(htyp);

/* get the base native type (for array types) */
        while (H5Tget_class(nativetype_id) == H5T_ARRAY) 
           {tempType = H5Tget_super(nativetype_id);
            H5Tclose(nativetype_id);
            nativetype_id = H5Tget_native_type(tempType, H5T_DIR_DESCEND);}

        if (H5Tget_class(nativetype_id) == H5T_STRING) 
           {DEBUG1("_H5_dec_dims: careful -- %s\n", "string array");
            nd++;}

        DEBUG1("      H5T_ARRAY rank(%d)\n", nd);
        hdims = (hsize_t*) calloc(nd, sizeof(hsize_t));
#if (H5_VERS_RELEASE < 4)
        status = H5Tget_array_dims(htyp, hdims); 
#else
	status = H5Tget_array_dims(htyp, hdims, NULL);
#endif

        if (H5Tget_class(nativetype_id) == H5T_STRING)
           {hdims[nd-1] = H5Tget_size(nativetype_id);}

        H5Tclose(nativetype_id);}

    else if (tcid == H5T_STRING)
       {nd = 1;

        DEBUG1("      H5T_STRING rank(%d)\n", nd);

        hdims    = (hsize_t*) calloc(nd, sizeof(hsize_t));
        hdims[0] = H5Tget_size(htyp);}

    else 
       {scid = H5Sget_simple_extent_type(hdim);

/* either we have a scalar space or an array (simple) space */
       switch(scid)
          {case H5S_SCALAR :

/* assume dimensionality info is NULL for scalars */
	        DEBUG1("%s\n", "      H5S_SCALAR");

		nd    = -1;
		hdims = NULL;
		break;

	   case H5S_SIMPLE :

/* obtain the rank of this simple data space */
		nd = H5Sget_simple_extent_ndims(hdim);
   
		if (nd < 0) 
		   {fprintf(stderr, "_H5_dec_dims: error obtaining rank\n");
		    return(NULL);};
 
		DEBUG1("      H5S_SIMPLE rank = %d\n", nd);    

/* get the sizes of each dimension */ 
		hdims  = (hsize_t *) calloc(nd, sizeof(hsize_t)); 
		status = H5Sget_simple_extent_dims(hdim, hdims, NULL);

		if (status < 0)
		   {fprintf(stderr, "_H5_dec_dims: error obtaining dims\n");
		    return(NULL);};
		break;
       
	   default :
		return(NULL);};};

/* construct the dimensionality information */
     if (nd > 0)
        {pdims = CMAKE(dimdes);
         dim   = pdims;}
     else 
        pdims = NULL;

     for (i = 0; i < nd; i++) 

/* HDF5 files assume 0 based counting */
         {dim->number    = (long) hdims[i];
          dim->index_min = 0L;
          dim->index_max = dim->number - 1L; 

          DEBUG1("      dim %d:", nd);
          DEBUG1(" 0:%ld\n", dim->index_max);    

/* are we going around again? */
         if (i+1 < nd)
            {dim->next = CMAKE(dimdes);
             dim       = dim->next;}
         else
            dim->next = NULL;};

    return(pdims);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_DEC_COMPOUND - decode the HDF representation of a struct into PDB form
 *                  - may recurse if necessary
 *                  - HTYP is the HDF5 ID for this type
 */

static char *_H5_dec_compound(PDBfile *file, hid_t htyp) 
   {int i, nm, convert;
    off_t moffs;
    char t[BUFFER_SIZE];
    char *mname, *regName;
    char *typ, *type;
    defstr *dpr;
    memdes *members, *mnxt;
    hid_t mtyp;
    dimdes *pdims;
    H5T_class_t tcl;
    hdf_state *hst;

    hst = file->meta;
    tcl = H5Tget_class(htyp);
  
    if (tcl != H5T_COMPOUND)
       return(NULL);

    regName = _H5_is_registered(file, htyp);

    if (regName == NULL)
       {typ     = NULL;
        convert = FALSE;
    
/* begin creating a defstr for the charts */
        members = CMAKE(memdes);
   
/* how many members are there? */ 
        nm = H5Tget_nmembers(htyp);
    
        if (nm <0)
           {fprintf(stderr, "_H5_DEC_COMPOUND: error retrieving num members\n");
            return(NULL);};
        
        DEBUG1("      # members %d\n", nm);
       
/* create a unique type name placeholder */ 
        snprintf(t, BUFFER_SIZE, "%s%d", PD_TYPE_PLACEHOLDER, hst->nstr);
        hst->nstr++;
        typ = CSTRSAVE(t);
        _H5_register(file, htyp, typ);
    
/* begin creating a memdes for the defstr */ 
        mnxt = members;
    
        for (i = 0; i < nm; i++) 

/* grab the member's name */
            {mname = H5Tget_member_name(htyp, i);
    
             if (mname == NULL)
                {fprintf(stderr, "_H5_DEC_COMPOUND: error retrieving member name\n");
                 return(NULL);};
    
             DEBUG1("      memb name %s\n", mname);
    
/* grab the member's offset */
             moffs = (off_t) H5Tget_member_offset(htyp, i); 
             if (moffs < 0)
                {fprintf(stderr, "_H5_DEC_COMPOUND: error retrieving member offset\n");
                 return(NULL);};
    
             DEBUG1("      offset 0x%lx\n", moffs);
    
/* grab the type of the internal member */
             mtyp = H5Tget_member_type(htyp, i);
    
             if (mtyp < 0)
                {fprintf(stderr, "_H5_DEC_COMPOUND: error retrieving member type\n");
                 return(NULL);};
    
             type = _H5_dec_type(file, mtyp); 
    
             DEBUG1("      type is %s\n", type);
    
             snprintf(t, BUFFER_SIZE, "%s %s", type, mname);
            
             pdims = _H5_dec_dims(file, mtyp, 0); 
   
/* fill in the memdes */ 
             mnxt->member      = CSTRSAVE(t);
             mnxt->cast_memb   = CSTRSAVE(type); 
             mnxt->member_offs = moffs;
             mnxt->cast_offs   = -1; 
             mnxt->type        = CSTRSAVE(type);
             mnxt->base_type   = CSTRSAVE(type);
             mnxt->name        = CSTRSAVE(mname);
             mnxt->dimensions  = pdims;

             if (pdims == NULL)
                mnxt->number = 1;
             else
	        mnxt->number = _PD_comp_num(pdims);
    
/* a struct needs conversion if one of its members needs conversion */
             dpr = (defstr*) SC_hasharr_def_lookup(hst->pf->chart, type);

             if (dpr != NULL)
                convert = convert || dpr->convert;
             else
                convert = FALSE;

/* continue to link the memdes members as we loop over the entries */
             if ((i + 1) < nm)
                {mnxt->next = CMAKE(memdes); 
                 mnxt = mnxt->next;}
             else
                mnxt = NULL;

             free(mname);
             H5Tclose(mtyp);
             CFREE(type);};

#if 0
	defstr *dp;

        dp = CMAKE(defstr);
        dp->type        = CSTRSAVE(typ);
        dp->size_bits   = dp->size * 8; 
        dp->size        = H5Tget_size(htyp);
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
        _PD_defstr_inst(hst->pf, typ, STRUCT_KIND, members,
			NO_ORDER, NULL, NULL, PD_CHART_HOST);

#if 0
        dp = _PD_mk_defstr(NULL, typ,
				     members, NULL,
				     H5Tget_size(htyp), 0, -1, FALSE, 
				     NULL, NULL, FALSE, FALSE);

        DEBUG1("      type name %s\n", typ);
        DEBUG1("      size %ld\n", (long) H5Tget_size(htyp));
        DEBUG1("      alignment %d\n", 4);

        _PD_d_install(hst->pf, typ, dp, PD_CHART_HOST);
    
/* make a copy of dp for the file charts */
        host_entry = CMAKE(defstr);
        memcpy(host_entry, dp, sizeof(defstr));
    
        host_entry->type    = CSTRSAVE(dp->type);
        host_entry->members = PD_copy_members(dp->members);
    
/* set convert just before adding to the file charts */
        host_entry->convert = convert; 
        _PD_d_install(hst->pf, typ, host_entry, PD_CHART_FILE);
    
        DEBUG1("Inserted defstr for %s in both charts\n", typ); 
#endif
       }

    else
       {DEBUG1("Compound type for %s already defined\n", regName);
        typ = CSTRSAVE(regName);};

    return(typ);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_READ_DATA - after H5LT_read_dataset_numerical
 *               - return TRUE iff successful
 */

int _H5_read_data(hid_t fid, const char *fullpath, hid_t htyp, void *vr)
   {int rv, st;
    hid_t did;

    rv = TRUE;

    did = H5Dopen2(fid, fullpath, H5P_DEFAULT);
    if (did < 0)
       rv = FALSE;

    else
       {st = H5Dread(did, htyp, H5S_ALL, H5S_ALL, H5P_DEFAULT, vr);
	if (st < 0)
	   {H5Dclose(did);
	    rv = FALSE;}

	else if (H5Dclose(did))
	   rv = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* H5_READ_GROUP_NODE - depth first search through group data objects 
 *                    - parsing data as we go 
 */

static herr_t H5_read_group_node(hid_t hgr, const char *mname, void *a)
   {int nc;
    int64_t addr;
    char *typ, *pgr, *temp_prefix, *fullname;
    herr_t status;
    hid_t hds, hdim, htyp;
    syment *ep;
    dimdes *pdims, *dim;
    H5G_stat_t statbuf;
    PDBfile *file;
    hdf_state *hst;

    file   = (PDBfile *) a;
    hst    = file->meta;
    status = (herr_t) 0;
    typ    = NULL; 
    pgr    = hst->group_prefix;
    pdims  = NULL;

    ep = CMAKE(syment);

    DEBUG1("ENTERING read group node for: %s\n", mname);

/* get the stats on this object */
    status = H5Gget_objinfo(hgr, mname, FALSE, &statbuf);

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
	     temp_prefix = CMAKE_N(char, nc);
	     temp_prefix[0] = '\0';

/* end it in slash '/' */
	     SC_vstrcat(temp_prefix, nc, "%s%s/",
			hst->group_prefix,             /* ends in slash '/' */
			(char *) mname);           /* append new name   */

	     hst->group_prefix = temp_prefix;

	     ep->type = CSTRSAVE("Directory");
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
	     status = H5Giterate(hgr, mname, NULL,
				 &H5_read_group_node, a);

/* return the group prefix to what it was before */
	     hst->group_prefix = pgr;
	     CFREE(temp_prefix);
	     break;
 
        case H5G_DATASET :
             DEBUG1("Parsing H5G_DATASET: %s\n", mname);

#if (H5_VERS_RELEASE < 4)
	     hds = H5Dopen(hgr, mname, H5P_DEFAULT);
#else
	     hds = H5Dopen(hgr, mname);
#endif
	     hdim = H5Dget_space(hds);
	     htyp = H5Dget_type(hds);    
	     addr = H5Dget_offset(hds);
             if (addr < 0)
	        {printf("   Group member %s had no address\n", mname);
		 return(-1);};

	     pdims    = _H5_dec_dims(file, htyp, hdim);
	     typ = _H5_dec_type(file, htyp); 

	     DEBUG1("TYPE is %s\n", typ);
 
/* create another entry in the symbol table */
	     ep->type = CSTRSAVE(typ);

/* setup the dimensionality */
	     if (pdims != NULL)
	        {ep->number = pdims->number;

		 for (dim = pdims->next; dim != NULL; dim = dim->next)
		     ep->number = ep->number * dim->number;

		 ep->dimensions = pdims;} 
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
	     fullname = CMAKE_N(char, nc);
	     fullname[0] = '\0';
	     SC_vstrcat(fullname, nc, "%s%s",
			hst->group_prefix, (char *) mname); 

	     _PD_e_install(hst->pf, fullname, ep, FALSE);

	     DEBUG1("    Inserted syment for %s", fullname);
	     DEBUG1(" with type %s\n", typ);   

/* free up some space */
	     H5Tclose(htyp);
	     H5Sclose(hdim);
	     H5Dclose(hds);
	     CFREE(fullname);
	     CFREE(typ);
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

    CFREE(file->meta);

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
   {int mode, status;
    char lname[MAXLINE];
    char *pname;
    syment *ep;
    PDBfile *file;
    PD_smp_state *pa;
    hdf_state *hst;

    pa = _PD_get_state(-1);

    SC_strncpy(lname, MAXLINE, name, -1);
    pname = strstr(lname, ",fmt=hdf");
    if (pname != NULL)
       *pname = '\0';
    pname = lname;

    DEBUG1("_H5_create called: name(%s)\n", pname);

/* setup any global variables we will not read from the file */
    hst = CMAKE(hdf_state);
    hst->nstr         = 0;
    hst->registry     = NULL;
    hst->group_prefix = NULL;

/* setup any local variables */
    status = 0;
    file   = NULL;

/* turn off the un-intelligent H5E print subsystem */
#if (H5_VERS_RELEASE < 4)
    H5Eset_auto(H5E_DEFAULT, NULL, stderr);
#else
    H5Eset_auto(H5E_DEFAULT, NULL);
#endif

/* make sure it really is an HDF5 file */
    mode = H5F_ACC_TRUNC;
    hst->hf = H5Fcreate(pname, mode, H5P_DEFAULT, H5P_DEFAULT);

/* return if this is not a valid HDF5 file */
    if (hst->hf < 0)
       return(NULL);

    file = _PD_mk_pdb(pu, "temp.pdb", NULL, TRUE, &_H5_sys, tr);
    if (file == NULL)
       return(NULL);

    hst->pf    = file;
    file->meta = hst;

/* fill in the host's std with default values */
    file->host_std   = _PD_copy_standard(pa->int_std);
    file->host_align = _PD_copy_alignment(pa->int_align);

/* insert entries in the host charts (both) since those stds are now known */
    _PD_setup_chart(file->host_chart, file->host_std, NULL, 
                    file->host_align, NULL, PD_CHART_HOST, TRUE);

/* assume the file and host standards are the same until
 * we find out otherwise (and mutate it at some later point)
 */
    file->std              = _PD_copy_standard(file->host_std);
    file->align            = _PD_copy_alignment(file->host_align); 
    file->mode             = PD_APPEND; 
    file->virtual_internal = FALSE; 
    file->type             = CSTRSAVE(H5FILE_S);
    file->current_prefix   = CSTRSAVE("/");

/* init the group/directory prefix to the root dir */
    hst->group_prefix = CSTRSAVE("/");

    ep = CMAKE(syment);
    ep->type = CSTRSAVE("Directory");
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
       {fprintf(stderr, "ERROR CREATING HDF5 FILE %s - _H5_CREATE\n", pname);
        _H5_close(file);
	file = NULL;};

    CFREE(hst->group_prefix);

    return(file);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _H5_OPEN - open an existing HDF5 File and translate it to an PDBfile 
 *          - NAME the absolute pathname of the HDF5 file to open
 */

static PDBfile *_H5_open(tr_layer *tr, SC_udl *pu, char *name, char *mode)
   {int status;
    hid_t hdf_root_group;
    syment *ep;
    PDBfile *file;
    PD_smp_state *pa;
    hdf_state *hst;

    pa = _PD_get_state(-1);

    DEBUG1("_H5_open called: name(%s)\n", name);

/* setup any global variables we will not read from the file */
    hst = CMAKE(hdf_state);
    hst->nstr         = 0;
    hst->registry     = NULL;
    hst->group_prefix = NULL;

/* setup any local variables */
    status = 0;
    file   = NULL;

/* turn off the un-intelligent H5E print subsystem */
#if (H5_VERS_RELEASE < 4)
    H5Eset_auto(H5E_DEFAULT, NULL, stderr);
#else
    H5Eset_auto(H5E_DEFAULT, NULL);
#endif

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
                    file->host_align, NULL, PD_CHART_HOST, TRUE);

/* assume the file and host standards are the same until
 * we find out otherwise (and mutate it at some later point)
 */
    file->std              = _PD_copy_standard( file->host_std);
    file->align            = _PD_copy_alignment(file->host_align); 
    file->mode             = PD_APPEND; 
    file->virtual_internal = FALSE; 
    file->type             = CSTRSAVE(H5FILE_S);
    file->current_prefix   = CSTRSAVE("/");

/* init the group/directory prefix to the root dir */
    hst->group_prefix = CSTRSAVE("/");

    ep = _PD_mk_syment("Directory", 1, 0, NULL, NULL);

#if 0
    ep = CMAKE(syment);
    ep->type                 = CSTRSAVE("Directory");
    ep->number               = 1; 
    ep->dimensions           = NULL; 
    ep->indirects.addr       = 0;
    ep->indirects.n_ind_type = 0;
    ep->indirects.arr_offs   = 0;

    _PD_block_init(ep, TRUE);
    _PD_entry_set_address(ep, 0, 0);
    _PD_entry_set_number(ep, 0, 0);
#endif
         
    _PD_e_install(file, hst->group_prefix, ep, FALSE);

    DEBUG1("    Inserted syment for %s", hst->group_prefix);
    DEBUG1(" with type %s\n", "Directory");   

/* parse the entire HDF5 file
 * insert entries in the charts as we encounter types
 * insert entries in the symbol table as we encounter the objects
 */
#if (H5_VERS_RELEASE < 4)
    hdf_root_group = H5Gopen(hst->hf, "/", H5P_DEFAULT);
#else
    hdf_root_group = H5Gopen(hst->hf, "/");
#endif
    status = H5Giterate(hst->hf, "/", NULL, &H5_read_group_node, file);
    H5Gclose(hdf_root_group);

/* if we were unsuccessful, alert the user */
    if (status < 0) 
       {_H5_close(file);
	file = NULL;};

    CFREE(hst->group_prefix);

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

long _H5_wr_syment(PDBfile *file, char *vr, long ni,
		   char *intype, char *outtype)
   {long rv;

    rv = _PD_wr_syment(file, vr, ni, intype, outtype);

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
 
/* PD_REGISTER_HDF5 - install the HDF5 extensions to PDBLib */
 
int PD_register_hdf5(void)
   {static int n = -1;

    ONCE_SAFE(TRUE, NULL)

/* since this is a spoke the application has to register
 * make sure all the others are registered first
 */
       _PD_register_spokes();

       n = PD_REGISTER(H5FILE_S, "hdf5", _H5_filep,
		       _H5_create, _H5_open, _H5_close,
		       _H5_write_entry, _H5_read_entry);

       SC_ASSERT(n >= 0);

    END_SAFE;

    return(n);}
 
/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/
 
/* PD_REGISTER_HDF5 - install the HDF5 extensions to PDBLib */
 
int PD_register_hdf5(void)
   {

    _PD_register_spokes();

    return(-1);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

