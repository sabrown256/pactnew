/*
 * PDFIA.C - FORTRAN interface routines for PDBLib (in PDBX)
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"


#ifndef F77_INT_SIZE_PTR_DIFFER

# define _PD_append_aux(file, name, type, space, nd, dims)                   \
    ((FIXNUM) PD_append_as_alt(file, name, type, space, nd, (long *) dims))

# define _PD_defent_aux(file, name, type, nd, dims)                          \
    ((FIXNUM) PD_defent_alt(file, name, type, nd, (long *) dims))

#endif

data_standard
 *PD_std_standards[] = {&PPC32_STD,
                        &M68X_STD,
                        &X86_64_STD,
                        &I386_STD,
                        &I586_STD,
                        &VAX_STD,
                        &CRAY_STD,
                        &PPC64_STD,
                        &X86_64_STD,
                        NULL};

data_alignment
 *PD_std_alignments[] = {&WORD2_ALIGNMENT,         /* 1 */
                         &SPARC_ALIGNMENT,
                         &GNU3_PPC64_ALIGNMENT,
                         &WORD2_ALIGNMENT,
                         &WORD4_ALIGNMENT,         /* 5 */
                         &WORD8_ALIGNMENT,
                         &XLC64_PPC64_ALIGNMENT,
                         &XLC32_PPC64_ALIGNMENT,
                         &GNU4_X86_64_ALIGNMENT,
                         &OSX_10_5_ALIGNMENT,      /* 10 */
                         &CYGWIN_I686_ALIGNMENT,
                         &GNU4_I686_ALIGNMENT,
                         &BYTE_ALIGNMENT,
                         NULL};

/*--------------------------------------------------------------------------*/

#ifdef F77_INT_SIZE_PTR_DIFFER

/*--------------------------------------------------------------------------*/

/* _PD_READ_AUX - read a variable in via _PD_indexed_read_as but map the
 *              - indexing information to the correct type first
 *              - NOTE: this would be symmetric with _PD_WRITE_AUX if
 *              - the read calls required the number of dimensions as the
 *              - write calls do!!!!
 *              - This can be useful to applications dealing with Fortran
 */

FIXNUM _PD_read_aux(PDBfile *file, char *name, char *type, void *vr,
		    FIXNUM *ind)
   {long *pi;
    FIXNUM i, ne, nd, ret;
    char fullpath[MAXLINE];
    dimdes *pd, *dims;
    syment *ep;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    switch (SETJMP(pa->read_err))
       {case ABORT    : return(FALSE);
        case ERR_FREE : return(TRUE);
        default       : memset(pa->err, 0, MAXLINE);
                        break;};

/* look up the variable name and return FALSE if it is not there */
    ep = _PD_effective_ep(file, name, TRUE, fullpath);
    if (ep == NULL)
       PD_error("ENTRY NOT IN SYMBOL TABLE - _PD_READ_AUX", PD_READ);

    dims = PD_entry_dimensions(ep);
    for (nd = 0, pd = dims; pd != NULL; pd = pd->next, nd++);

    ne = 3*nd;

    pi = FMAKE_N(long, ne, "_PD_READ_AUX:pi");
    for (i = 0; i < ne; i++)
        pi[i] = ind[i];

    ret = _PD_indexed_read_as(file, fullpath, type, vr, nd, pi, ep);

    SFREE(pi);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WRITE_AUX - write a variable out via PD_write_as_alt but map the
 *               - indexing information to the correct type first
 *               - This can be useful to applications dealing with Fortran
 */

FIXNUM _PD_write_aux(PDBfile *file, char *name, char *typi, char *typo,
		     void *space, FIXNUM nd, FIXNUM *dims)
   {long i, ne, *pi;
    FIXNUM ret;

    ne = 3*nd;
    pi = FMAKE_N(long, ne, "_PD_WRITE_AUX:pi");
    for (i = 0; i < ne; i++)
        pi[i] = dims[i];

    ret = PD_write_as_alt(file, name, typi, typo, space, nd, pi);

    SFREE(pi);

    return(ret);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_APPEND_AUX - append to a variable via PD_append_as_alt but map the
 *                - indexing information to the correct type first
 */

static FIXNUM _PD_append_aux(PDBfile *file, char *name, char *type,
			     void *space, FIXNUM nd, FIXNUM *dims)
   {long i, ne, *pi;
    FIXNUM ret;

    ne = 3*nd;
    pi = FMAKE_N(long, ne, "_PD_APPEND_AUX:pi");
    for (i = 0; i < ne; i++)
        pi[i] = dims[i];

    ret = PD_append_as_alt(file, name, type, space, nd, pi);

    SFREE(pi);

    return(ret);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DEFENT_AUX - define a variable via PD_defent_alt but map the
 *                - indexing information to the correct type first
 */

static FIXNUM _PD_defent_aux(PDBfile *file, char *name, char *type,
			     FIXNUM nd, FIXNUM *dims)
   {long i, ne, *pi;
    syment *ep;

    ne = 3*nd;
    pi = FMAKE_N(long, ne, "_PD_DEFENT_AUX:pi");
    for (i = 0; i < ne; i++)
        pi[i] = dims[i];

    ep = PD_defent_alt(file, name, type, nd, pi);

    SFREE(pi);

    return(ep != NULL);}
    
/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/

/* _PD_READ_AUX - read a variable in via _PD_indexed_read_as but map the
 *              - indexing information to the correct type first
 *              - NOTE: this would be symmetric with _PD_WRITE_AUX if
 *              - the read calls required the number of dimensions as the
 *              - write calls do!!!!
 *              - This can be useful to applications dealing with Fortran
 */

FIXNUM _PD_read_aux(PDBfile *file, char *name, char *type, void *vr,
		    FIXNUM *ind)
   {FIXNUM ret;

    ret = PD_read_as_alt(file, name, type, vr, (long *) ind);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WRITE_AUX - write a variable out via PD_write_as_alt but map the
 *               - indexing information to the correct type first
 *               - This can be useful to applications dealing with Fortran
 */

FIXNUM _PD_write_aux(PDBfile *file, char *name, char *typi, char *typo,
		     void *space, FIXNUM nd, FIXNUM *dims)
   {FIXNUM ret;

    ret = PD_write_as_alt(file, name, typi, typo, space, nd, (long *) dims);

    return(ret);}
    
/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* PFGERR - FORTRAN interface routine to fetch PD_ERR */

FIXNUM F77_FUNC(pfgerr, PFGERR)(FIXNUM *nchr, F77_string err)
   {FIXNUM rv;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    SC_strncpy(SC_F77_C_STRING(err), *nchr, pa->err, MAXLINE);

    *nchr = strlen(pa->err);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGFNM - FORTRAN interface routine to fetch the name of the file */

FIXNUM F77_FUNC(pfgfnm, PFGFNM)(FIXNUM *fileid, FIXNUM *pnc, F77_string name)
   {PDBfile *file;
    int nc, lc;
    FIXNUM rv;

    nc = *pnc;

    file = SC_GET_POINTER(PDBfile, *fileid);

    lc = strlen(file->name);
    if (lc != nc)
       *pnc = lc;

    if (nc >= lc)
       {strncpy(SC_F77_C_STRING(name), file->name, nc);
        rv = TRUE;}
    else
       {PD_error("BUFFER TOO SMALL TO HOLD FILE NAME - PFGFNM", PD_GENERIC);
	rv = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGFDT - FORTRAN interface routine to fetch the file creation date */

FIXNUM F77_FUNC(pfgfdt, PFGFDT)(FIXNUM *fileid, FIXNUM *pnc, F77_string date)
   {PDBfile *file;
    int nc, lc;
    FIXNUM rv;

    nc = *pnc;

    file = SC_GET_POINTER(PDBfile, *fileid);

    lc = strlen(file->date);
    if (lc != nc)
       *pnc = lc;

    if (nc >= lc)
       {SC_strncpy(SC_F77_C_STRING(date), *pnc, file->date, nc);
        rv = TRUE;}
    else
       {PD_error("BUFFER TOO SMALL TO HOLD FILE DATE - PFGFDT", PD_GENERIC);
	rv = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGCSM - FORTRAN interface routine to fetch file->use_cksum */

FIXNUM F77_FUNC(pfgcsm, PFGCSM)(FIXNUM *fileid, FIXNUM *v)
   {FIXNUM rv;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = file->use_cksum;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFSCSM - FORTRAN interface routine to set file->use_cksum */

FIXNUM F77_FUNC(pfscsm, PFSCSM)(FIXNUM *fileid, FIXNUM *pv)
   {FIXNUM rv;
    PD_checksum_mode v;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *fileid);

    v = (PD_checksum_mode) *pv;

    rv = PD_activate_cksum(file, v);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGTPT - FORTRAN interface routine to fetch file->track_pointers */

FIXNUM F77_FUNC(pfgtpt, PFGTPT)(FIXNUM *fileid, FIXNUM *v)
   {FIXNUM rv;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = file->track_pointers;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFSTPT - FORTRAN interface routine to set file->track_pointers */

FIXNUM F77_FUNC(pfstpt, PFSTPT)(FIXNUM *fileid, FIXNUM *v)
   {FIXNUM rv;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = *v;
    file->track_pointers = rv;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGBFS - FORTRAN interface routine to fetch PD_buffer_size */

FIXNUM F77_FUNC(pfgbfs, PFGBFS)(void)
   {FIXNUM rv;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    rv = FALSE;

    _PD_init_state(FALSE);

    rv = pa->buffer_size;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFSBFS - FORTRAN interface routine to set PD_buffer_size */

FIXNUM F77_FUNC(pfsbfs, PFSBFS)(FIXNUM *psz)
   {FIXNUM rv;
    BIGINT sz;
    PD_smp_state *pa;

    _PD_init_state(FALSE);

    pa = _PD_get_state(-1);

    sz              = *psz;
    pa->buffer_size = sz;
    rv              = sz;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGFMV - FORTRAN interface routine to fetch PD_default_format_version */

FIXNUM F77_FUNC(pfgfmv, PFGFMV)(FIXNUM *v)
   {FIXNUM rv;

    PD_get_format_version(*v);

    rv = *v;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFSFMV - FORTRAN interface routine to set PD_default_format_version */

FIXNUM F77_FUNC(pfsfmv, PFSFMV)(FIXNUM *v)
   {FIXNUM rv;

    PD_set_format_version(*v);

    rv = *v;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGOFF - FORTRAN interface routine to fetch
 *        - file's default_offset
 */

FIXNUM F77_FUNC(pfgoff, PFGOFF)(FIXNUM *fileid)
   {FIXNUM rv;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = file->default_offset;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFSOFF - FORTRAN interface routine to set
 *        - file's default_offset
 */

FIXNUM F77_FUNC(pfsoff, PFSOFF)(FIXNUM *fileid, FIXNUM *v)
   {FIXNUM rv;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *fileid);

    file->default_offset = *v;
    rv = file->default_offset;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGMXS - FORTRAN interface routine to fetch
 *        - file's maximum size
 */

FIXNUM F77_FUNC(pfgmxs, PFGMXS)(FIXNUM *fileid)
   {FIXNUM rv;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = file->maximum_size;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFSMXS - FORTRAN interface routine to set
 *        - file's maximum size
 */

FIXNUM F77_FUNC(pfsmxs, PFSMXS)(FIXNUM *fileid, FIXNUM *v)
   {FIXNUM rv;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *fileid);

    file->maximum_size = *v;
    rv = file->maximum_size;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGMOD - FORTRAN interface routine to fetch
 *        - file's mode
 */

FIXNUM F77_FUNC(pfgmod, PFGMOD)(FIXNUM *fileid)
   {FIXNUM rv;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = file->mode;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFINTH - FORTRAN interface routine to initialize pdblib for threads
 * 
 */

FIXNUM F77_FUNC(pfinth, PFINTH)(FIXNUM *pnthreads, PFTid tid)
   {int numthreads;
    PFTid threadid;
    
    numthreads = (int)(*pnthreads);
    
    if (tid == 0)
      threadid = NULL;
    else
      threadid = tid;  

    PD_init_threads(numthreads, threadid);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFVART - FORTRAN interface routine to set up a list of file variables
 *        - sorted in the specified order
 *        - returns 1 if successful and 0 otherwise
 */

FIXNUM F77_FUNC(pfvart, PFVART)(FIXNUM *fileid, FIXNUM *pord, FIXNUM *n)
   {int entries;
    int (*fun)(haelem **v, int n);
    FIXNUM rv;
    PD_symbol_order ord;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *fileid);
    ord  = (PD_symbol_order) *pord;

    switch (ord)
       {case PF_ALPHABETIC :
             fun = NULL;
	     break;
        case PF_DISK_ORDER :
	     fun = _PD_disk_addr_sort;
	     break;
        default :
	     PD_error("BAD ORDER ARGUMENT - PFVART", PD_GENERIC);
	     return(FALSE);};

    entries = SC_set_hash_dump(file->symtab, fun);

    if (entries == -1)
       {PD_error("HASH DUMP FAILED - PFVART", PD_GENERIC);
        rv = FALSE;}
    else
       {*n = entries;
        rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFDVAR - FORTRAN interface routine to delete internal table set up by a
 *        - call to PFVART.
 */

FIXNUM F77_FUNC(pfdvar, PFDVAR)(void)
   {

    SC_free_hash_dump();

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGVAR - get name of the nth variable in the internal sorted table set up
 *        - by a call to PFVART. return 1 if successful and 0 otherwise.
 */

FIXNUM F77_FUNC(pfgvar, PFGVAR)(FIXNUM *pn, FIXNUM *pnchr, F77_string pname)
   {int n;
    FIXNUM rv;
    char *s;

    n = *pn - 1;
    s = SC_get_entry(n);
    if (s == NULL)
       {PD_error("VARIABLE ORDINAL OUT OF RANGE - PFGVAR", PD_GENERIC);
	rv = FALSE;}

    else
       {strcpy(SC_F77_C_STRING(pname), s);
        *pnchr = strlen(s);
        rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFIVAR - inquire about a variable in a PDBfile
 *        - this probably won't do everything but it will do some of the
 *        - useful stuff
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pfivar, PFIVAR)(FIXNUM *fileid, FIXNUM *pnchr,
				F77_string name, FIXNUM *pntyp,
				F77_string type,
				FIXNUM *psize, FIXNUM *pndims, FIXNUM *dims)
   {FIXNUM nd, rv;
    char s[MAXLINE], *ltype;
    syment *ep;
    dimdes *pd;
    PDBfile *file;
    PD_smp_state *pa;

    SC_FORTRAN_STR_C(s, name, *pnchr);

    ltype = SC_F77_C_STRING(type);

    file = SC_GET_POINTER(PDBfile, *fileid);

    pa = _PD_get_state(-1);

    ep = _PD_effective_ep(file, s, TRUE, NULL);
    if (ep == NULL)
       {snprintf(pa->err, MAXLINE,
		 "ERROR: VARIABLE %s NOT FOUND - PFIVAR\n", s);
        rv = FALSE;}

    else
       {strcpy(ltype, PD_entry_type(ep));
	*pntyp = strlen(ltype);
	*psize = PD_entry_number(ep);

	for (nd = (FIXNUM) 0, pd = PD_entry_dimensions(ep);
	     pd != NULL;
	     pd = pd->next)
	   {dims[nd++] = pd->index_min;
	    dims[nd++] = pd->index_max;};
	*pndims = nd/2;

	_PD_rl_syment_d(ep);

	rv = TRUE;};

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFCTYP - copy a type definition from source file to destination file
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pfctyp, PFCTYP)(FIXNUM *sfid, FIXNUM *dfid, FIXNUM *pntyp,
			     F77_string type)
   {FIXNUM rv;
    char s[MAXLINE];
    defstr *dp;
    memdes *lst;
    PDBfile *sf, *df;
    PD_smp_state *pa;

    rv = TRUE;

    SC_FORTRAN_STR_C(s, type, *pntyp);

    sf = SC_GET_POINTER(PDBfile, *sfid);
    df = SC_GET_POINTER(PDBfile, *dfid);

    pa = _PD_get_state(-1);

    dp = PD_inquire_type(sf, s);
    if (dp == NULL)
       {snprintf(pa->err, MAXLINE,
		 "ERROR: TYPE %s NOT FOUND - PFCTYP\n", s);
        rv = FALSE;}

    else
       {lst = PD_copy_members(dp->members);
	dp  = _PD_defstr_inst(df, s, STRUCT_KIND, lst,
			      NO_ORDER, NULL, NULL, FALSE);
	if (dp == NULL)
	   {snprintf(pa->err, MAXLINE,
		     "ERROR: CANNOT CREATE TYPE %s - PFCTYP\n", s);
	    rv = FALSE;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFITYP - inquire about a type in a PDBfile
 *        - this probably won't do everything but it will do some of the
 *        - useful stuff
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pfityp, PFITYP)(FIXNUM *fileid, FIXNUM *pntyp,
				F77_string type,
				FIXNUM *psize, FIXNUM *palgn, FIXNUM *pind)
   {FIXNUM rv;
    char s[MAXLINE];
    defstr *dp;
    PDBfile *file;
    PD_smp_state *pa;

    rv = TRUE;

    SC_FORTRAN_STR_C(s, type, *pntyp);

    file = SC_GET_POINTER(PDBfile, *fileid);

    pa = _PD_get_state(-1);

    dp = PD_inquire_type(file, s);
    if (dp == NULL)
       {snprintf(pa->err, MAXLINE,
		 "ERROR: TYPE %s NOT FOUND - PFITYP\n", s);
        rv = FALSE;}

    else
       {*psize = dp->size;
	*palgn = dp->alignment;
	*pind  = dp->n_indirects;};

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFIMBR - inquire about the nth member of a type in a PDBfile
 *        - return the member in the space provided
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pfimbr, PFIMBR)(FIXNUM *fileid, FIXNUM *pntyp,
				F77_string type,
				FIXNUM *pn, FIXNUM *psize,
				F77_string space)
   {int i, n, sz, nc;
    FIXNUM rv;
    char s[MAXLINE], *ps;
    defstr *dp;
    memdes *desc;
    PDBfile *file;
    PD_smp_state *pa;

    SC_FORTRAN_STR_C(s, type, *pntyp);

    file = SC_GET_POINTER(PDBfile, *fileid);
    rv   = TRUE;

    pa = _PD_get_state(-1);

    dp = PD_inquire_type(file, s);
    if (dp == NULL)
       {snprintf(pa->err, MAXLINE,
		 "ERROR: TYPE %s NOT FOUND - PFIMBR\n", s);
	rv = FALSE;}

    else
       {n = *pn;
	for (i = 1, desc = dp->members;
	     (i < n) && (desc != NULL);
	     i++, desc = desc->next);

	if (desc != NULL)
	   {ps = desc->member;
	    nc = strlen(ps);

	    sz = *psize;
	    if (sz != nc)
	       *psize = nc;

	    if (sz >= nc)
	       strncpy(SC_F77_C_STRING(space), ps, sz);
	    else
	       {PD_error("BUFFER TOO SMALL TO HOLD DESCRIPTION - PFIMBR",
			 PD_GENERIC);
		rv = FALSE;};}
	else
	   {*psize = -1;
	    snprintf(pa->err, MAXLINE,
		     "ERROR: MEMBER %d NOT FOUND - PFIMBR\n", n);
	    rv = FALSE;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFQMBR - inquire about the nth member of a type in a PDBfile
 *        - return the name, type and dimensions
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pfqmbr, PFQMBR)(FIXNUM *fileid, FIXNUM *pntyp,
			     F77_string type,
			     FIXNUM *pn, FIXNUM *pname,
			     F77_string name,
			     FIXNUM *pntout,
			     F77_string tout,
			     FIXNUM *pndims, FIXNUM *dims)
   {int i, n, ndims, nc, sz;
    char s[MAXLINE], *ps, errmsg[MAXLINE];
    defstr *dp;
    memdes *desc;
    dimdes *dimens;
    FIXNUM err, rv;
    PDBfile *file;
    PD_smp_state *pa;

    err = (FIXNUM) FALSE;
    strcpy(errmsg, "ERROR: ");

    SC_FORTRAN_STR_C(s, type, *pntyp);

    file = SC_GET_POINTER(PDBfile, *fileid);

    pa = _PD_get_state(-1);

    dp = PD_inquire_type(file, s);
    if (dp == NULL)
       {snprintf(pa->err, MAXLINE,
		 "ERROR: TYPE %s NOT FOUND - PFQMBR\n", s);
        return(FALSE);};

    n = *pn;
    for (i = 1, desc = dp->members;
         (i < n) && (desc != NULL);
         i++, desc = desc->next);

    if (desc != NULL)
       {ps = desc->name;
        nc = strlen(ps);

        sz = *pname;
        if (sz != nc)
	   *pname = nc;

        if (sz >= nc)
           SC_strncpy(SC_F77_C_STRING(name), *pn, ps, sz);
        else
           {err = (FIXNUM) TRUE;
            SC_strcat(errmsg, MAXLINE, "NAME BUFFER TOO SMALL TO HOLD NAME\n");};

        ps = desc->type;
        nc = strlen(ps);

        sz = *pntout;
        if (sz != nc)
	   *pntout = nc;

        if (sz >= nc)
           SC_strncpy(SC_F77_C_STRING(tout), *pntout, ps, sz);
        else
           {err = (FIXNUM) TRUE;
            SC_strcat(errmsg, MAXLINE, "TYPE BUFFER TOO SMALL TO HOLD TYPE\n");};

        ndims = desc->number;
 
        if ((int) *pndims > (ndims * 2))
           {for (n = 0, dimens = desc->dimensions; 
                 dimens != NULL; 
                 dimens = dimens->next)
                {dims[n++] = dimens->index_min;
                 dims[n++] = dimens->index_max;};
            *pndims = n/2;}
        else 
           {err = (FIXNUM) TRUE;
            SC_strcat(errmsg, MAXLINE, 
		      "DIMENSIONS ARRAY TOO SMALL TO HOLD DIMENSIONS\n");};}
    else
        {snprintf(pa->err, MAXLINE,
		  "ERROR: MEMBER %d NOT FOUND - PFQMBR\n", n); 
         return((FIXNUM) FALSE);};

    if (err)
       {PD_error(errmsg, PD_GENERIC);
        rv = FALSE;}
    else
       rv = TRUE;
    
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFREAD - read a variable from a PDBfile
 *        - this probably won't do everything but it will do some of the
 *        - useful stuff
 *        - return the number of item successfully read
 *        -
 *        - NOTE: SPACE must be a pointer to an object with the type
 *        - given by TYPE (PDBLib will allocated space if necessary)!
 */

FIXNUM F77_FUNC(pfread, PFREAD)(FIXNUM *fileid, FIXNUM *pnchr,
				F77_string name, void *space)
   {FIXNUM rv;
    char s[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *pnchr);

    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = PD_read(file, s, space);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFRDAS - read a variable from a PDBfile
 *        - convert to type TYPE regardless of symbol entry type
 *        - this probably won't do everything but it will do some of the
 *        - useful stuff
 *        - return the number of item successfully read
 *        -
 *        - NOTE: SPACE must be a pointer to an object with the type
 *        - given by TYPE (PDBLib will allocated space if necessary)!
 */

FIXNUM F77_FUNC(pfrdas, PFRDAS)(FIXNUM *fileid, FIXNUM *pnchr,
				F77_string name,
				FIXNUM *pntyp, F77_string type,
				void *space)
   {FIXNUM rv;
    char s[MAXLINE], t[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *pnchr);
    SC_FORTRAN_STR_C(t, type, *pntyp);

    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = PD_read_as(file, s, t,  space);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFRDAI - FORTRAN interface to PD_read_as_dwim */

FIXNUM F77_FUNC(pfrdai, PFRDAI)(FIXNUM *fileid, FIXNUM *pnchr,
			     F77_string name,
			     FIXNUM *pntyp, F77_string type,
			     FIXNUM *pni, void *space)
   {long ni;
    FIXNUM rv;
    char s[MAXLINE], t[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *pnchr);
    SC_FORTRAN_STR_C(t, type, *pntyp);

    ni   = (long) *pni;
    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = PD_read_as_dwim(file, s, t, ni, space);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFRPTR - read an indirect variable from a PDBfile
 *        - this probably won't do everything but it will do some of the
 *        - useful stuff
 *        - return the number of item successfully read
 *        -
 *        - NOTE: SPACE must be an object with the type
 *        - given by TYPE (differs from the conventional PDB rule)
 */

FIXNUM F77_FUNC(pfrptr, PFRPTR)(FIXNUM *fileid, FIXNUM *pnchr,
				F77_string name,
				FIXNUM *pni, void *space)
   {FIXNUM rv;
    char s[MAXLINE], fullpath[MAXLINE], *type;
    syment *ep;
    PDBfile *file;
    PD_smp_state *pa;

    SC_FORTRAN_STR_C(s, name, *pnchr);

    file = SC_GET_POINTER(PDBfile, *fileid);

    pa = _PD_get_state(-1);

    switch (SETJMP(pa->read_err))
       {case ABORT :
	     return((FIXNUM) FALSE);
        case ERR_FREE :
	     return((FIXNUM) TRUE);
        default :
	     memset(pa->err, 0, MAXLINE);
	     break;};

    rv = FALSE;

/* find the effective symbol table entry for the named item */
    ep = _PD_effective_ep(file, s, TRUE, fullpath);
    if (ep == NULL)
       {snprintf(s, MAXLINE, "UNREADABLE OR MISSING ENTRY \"%s\" - PFRPTR",
                   fullpath);
        PD_error(s, PD_READ);}

    else
       {type = PD_entry_type(ep);
	if ((type != NULL) && _PD_indirection(type))
	   {void *vr;
	    size_t nb;
	    long nir, nis, bpi;

	    rv = _PD_hyper_read(file, fullpath, type, ep, &vr);

	    strcpy(s, type);
	    PD_dereference(s);
	    bpi = _PD_lookup_size(s, file->host_chart);
	    nis = (*pni)*bpi;
	    nir = SC_arrlen(vr);
	    if (nir > nis)
	       {rv = FALSE;
	        nb = nis;}
	    else
	       nb = nir;

	    memcpy(space, vr, nb);

	    SFREE(vr);

	    _PD_rl_syment_d(ep);};};

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFPTRD - read a part of a variable from a PDBfile
 *        - this probably won't do everything but it will do some of the
 *        - useful stuff
 *        - return the number of item successfully read
 *
 *        - NOTE: the entry MUST be an array (either a static array or
 *        - a pointer)
 *        -
 *        - NOTE: SPACE must be a pointer to an object with the type
 *        - of the object associated with NAME (PDBLib will allocate
 *        - space if necessary)!
 */

FIXNUM F77_FUNC(pfptrd, PFPTRD)(FIXNUM *fileid, FIXNUM *pnchr,
				F77_string name, void *space,
				FIXNUM *ind)
   {FIXNUM rv;
    char s[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *pnchr);

    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = _PD_read_aux(file, s, (char *) NULL, space, ind);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFRDAD - read a part of a variable from a PDBfile
 *        - convert to type TYPE regardless of symbol entry type
 *        - this probably won't do everything but it will do some of the
 *        - useful stuff
 *        - return the number of item successfully read
 *
 *        - NOTE: the entry MUST be an array (either a static array or
 *        - a pointer)
 *        -
 *        - NOTE: SPACE must be a pointer to an object with the type
 *        - of the object associated with NAME (PDBLib will allocate
 *        - space if necessary)!
 */

FIXNUM F77_FUNC(pfrdad, PFRDAD)(FIXNUM *fileid, FIXNUM *pnchr,
				F77_string name,
				FIXNUM *pntyp, F77_string type,
				void *space, FIXNUM *ind)
   {FIXNUM rv;
    char s[MAXLINE], t[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *pnchr);
    SC_FORTRAN_STR_C(t, type, *pntyp);

    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = _PD_read_aux(file, s, t, space, ind);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFAPPA - append to a variable in a PDBfile
 *        - ASCII encode the dimensions in the name
 *        - this probably won't do everything but it will do some of the
 *        - useful stuff
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pfappa, PFAPPA)(FIXNUM *fileid, FIXNUM *pnchr,
				F77_string name, void *space)
   {FIXNUM rv;
    char s[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *pnchr);

    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = PD_append(file, s, space);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFAPAS - append to a variable in a PDBfile
 *        - convert from INTYPE to the type of the existing data
 *        - ASCII encode the dimensions in the name
 *        - this probably won't do everything but it will do some of the
 *        - useful stuff
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pfapas, PFAPAS)(FIXNUM *fileid, FIXNUM *pnchr,
				F77_string name, FIXNUM *pntyp,
				F77_string intype, void *space)
   {FIXNUM rv;
    char s[MAXLINE], t[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *pnchr);
    SC_FORTRAN_STR_C(t, intype, *pntyp);

    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = PD_append_as(file, s, t, space);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFAPPD - append to a variable in a PDBfile
 *        - the dimensions are specified in an array of (min,max) pairs
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pfappd, PFAPPD)(FIXNUM *fileid, FIXNUM *pnchr,
				F77_string name, void *space,
				FIXNUM *pndims, FIXNUM *dims)
   {FIXNUM rv;
    char s[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *pnchr);

    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = _PD_append_aux(file, s, (char *) NULL, space, *pndims, dims);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFAPAD - append to a variable in a PDBfile
 *        - convert from INTYPE to the type of the existing data
 *        - the dimensions are specified in an array of (min,max) pairs
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pfapad, PFAPAD)(FIXNUM *fileid, FIXNUM *pnchr,
				F77_string name, FIXNUM *pntyp,
				F77_string intype, void *space,
				FIXNUM *pndims, FIXNUM *dims)
   {FIXNUM rv;
    char s[MAXLINE], t[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *pnchr);
    SC_FORTRAN_STR_C(t, intype, *pntyp);

    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = _PD_append_aux(file, s, t, space, *pndims, dims);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFWRTA - write a variable to a PDBfile
 *        - ASCII encode the dimensions in the name
 *        - this probably won't do everything but it will do some of the
 *        - useful stuff
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pfwrta, PFWRTA)(FIXNUM *fileid, FIXNUM *pnchr,
				F77_string name, FIXNUM *pntyp,
				F77_string type, void *space)
   {FIXNUM rv;
    char s[MAXLINE], t[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *pnchr);
    SC_FORTRAN_STR_C(t, type, *pntyp);

    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = PD_write(file, s, t, space);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFWRAS - write a variable of type INTYPE to a PDBfile as type OUTTYPE
 *        - ASCII encode the dimensions in the name
 *        - this probably won't do everything but it will do some of the
 *        - useful stuff
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pfwras, PFWRAS)(FIXNUM *fileid, FIXNUM *pnchr,
				F77_string name, FIXNUM *pintyp,
				F77_string intype, FIXNUM *poutyp, 
				F77_string outtype, void *space)
   {FIXNUM rv;
    char s[MAXLINE], t1[MAXLINE], t2[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *pnchr);
    SC_FORTRAN_STR_C(t1, intype, *pintyp);
    SC_FORTRAN_STR_C(t2, outtype, *poutyp);

    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = PD_write_as(file, s, t1, t2, space);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFWRTD - write a variable to a PDBfile
 *        - the dimensions are specified in an array of (min,max) pairs
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pfwrtd, PFWRTD)(FIXNUM *fileid, FIXNUM *pnchr,
				F77_string name, FIXNUM *pntyp,
				F77_string type, void *space,
				FIXNUM *pndims, FIXNUM *dims)
   {FIXNUM rv;
    char s[MAXLINE], t[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *pnchr);
    SC_FORTRAN_STR_C(t, type, *pntyp);

    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = _PD_write_aux(file, s, t, t, space, *pndims, dims);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFWRAD - write a variable INTYPE to a PDBfile as type OUTTYPE
 *        - the dimensions are specified in an array of (min,max) pairs
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pfwrad, PFWRAD)(FIXNUM *fileid, FIXNUM *pnchr,
				F77_string name, FIXNUM *pintyp,
				F77_string intype, FIXNUM *poutyp, 
				F77_string outtype, void *space,
				FIXNUM *pndims,  FIXNUM *dims)
   {FIXNUM rv;
    char s[MAXLINE], ti[MAXLINE], to[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *pnchr);
    SC_FORTRAN_STR_C(ti, intype, *pintyp);
    SC_FORTRAN_STR_C(to, outtype, *poutyp);

    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = _PD_write_aux(file, s, ti, to, space, *pndims, dims);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFDEFA - define a symbol table entry and reserve disk space
 *        - dimension information is encoded in name
 *        - return TRUE if successful, FALSE otherwise
 */

FIXNUM F77_FUNC(pfdefa, PFDEFA)(FIXNUM *fileid, FIXNUM *pnchr,
				F77_string name, FIXNUM *pntyp,
				F77_string type)
   {FIXNUM rv;
    char s[MAXLINE], t[MAXLINE];
    syment *ep;
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *pnchr);
    SC_FORTRAN_STR_C(t, type, *pntyp);

    file = SC_GET_POINTER(PDBfile, *fileid);

    ep = PD_defent(file, s, t);
    rv = (ep != NULL);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFDEFD - define a symbol table entry and reserve disk space
 *        - dimension information is provided in index range array
 *        - return TRUE if successful, FALSE otherwise
 */

FIXNUM F77_FUNC(pfdefd, PFDEFD)(FIXNUM *fileid, FIXNUM *pnchr,
				F77_string name, FIXNUM *pntyp,
				F77_string type, FIXNUM *pndims, FIXNUM *dims)
   {FIXNUM rv;
    char s[MAXLINE], t[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *pnchr);
    SC_FORTRAN_STR_C(t, type, *pntyp);

    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = _PD_defent_aux(file, s, t, *pndims, dims);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFDEFS - define a structure for the FORTRAN interface
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pfdefs, PFDEFS)(FIXNUM *fileid,
				FIXNUM *pnchr, F77_string name, ...)
   {FIXNUM n, rv;
    FIXNUM *pn;
    F77_string ps;
    char *ptype, *type;
    char s[MAXLINE], lname[MAXLINE];
    hasharr *fchrt;
    defstr *dp;
    memdes *desc, *lst, *prev;
    PDBfile *file;
    PD_smp_state *pa;

    SC_VA_START(name);

    file = SC_GET_POINTER(PDBfile, *fileid);

    SC_FORTRAN_STR_C(lname, name, *pnchr);

    pa = _PD_get_state(-1);

    prev  = NULL;
    lst   = NULL;
    fchrt = file->chart;
    for (pn = SC_VA_ARG(FIXNUM *); (n = *pn) != 0;
         pn = SC_VA_ARG(FIXNUM *))
        {ps = SC_VA_ARG(F77_string);
         SC_FORTRAN_STR_C(s, ps, n);

         desc  = _PD_mk_descriptor(s, file->default_offset);
         type  = SC_strsavef(s, "char*:PFDEFS:type");
         ptype = SC_firsttok(type, " \n");
         if (SC_hasharr_lookup(fchrt, ptype) == NULL)
            if ((strcmp(ptype, lname) != 0) || !_PD_indirection(s))
               {snprintf(pa->err, MAXLINE,
			 "ERROR: %s BAD MEMBER TYPE - PFDEFS\n",
			 s);
                return(FALSE);};
         SFREE(type);
         if (lst == NULL)
            lst = desc;
         else
            prev->next = desc;
         prev = desc;};
    SC_VA_END;

/* install the type in both charts */
    rv = TRUE;
    dp = _PD_defstr_inst(file, lname, STRUCT_KIND, lst,
			 NO_ORDER, NULL, NULL, FALSE);
    if (dp == NULL)
       {PD_error("CAN'T HANDLE PRIMITIVE TYPE - PFDEFS", PD_GENERIC);
        rv = FALSE;};

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFDEFT - define a structure for the FORTRAN interface
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pfdeft, PFDEFT)(FIXNUM *fileid, FIXNUM *pnchr, F77_string name,
				FIXNUM *pnm, FIXNUM *nc, F77_string nm)
   {int i, n, indx, mc;
    FIXNUM rv;
    char lname[MAXLINE], bf[MAXLINE], *lnm, **members;
    defstr *dp;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *fileid);
    lnm  = SC_F77_C_STRING(nm);

    SC_FORTRAN_STR_C(lname, name, *pnchr);

    n = *pnm;
    members = FMAKE_N(char *, n, "PFDEFT:members");
    for (i = 0; i < n; i++)
        {indx = nc[2*i];
         mc   = nc[2*i + 1];

         SC_strncpy(bf, MAXLINE, lnm+indx, mc);

         members[i] = SC_strsavef(bf, "char*:PFDEFT:bf");};

/* install the type in both charts */
    dp = PD_defstr_alt(file, lname, n, members);

    for (i = 0; i < n; i++)
        SFREE(members[i]);
    SFREE(members);

    rv = (dp != NULL);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFFREE - wrapper over PD_free
 *        - NOTE: SPACE must be a pointer to an object with the type
 *        - given by TYPE (PDBLib will allocated space if necessary)!
 */

FIXNUM F77_FUNC(pffree, PFFREE)(FIXNUM *fileid, FIXNUM *pnchr,
				F77_string type, void *space)
   {FIXNUM rv;
    PDBfile *file;
    char s[MAXLINE];

    SC_FORTRAN_STR_C(s, type, *pnchr);

    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = PD_free(file, s, space);

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFTRGT - target the next file to be opened
 *        - given an index from the following list:
 *        -
 *        -   (5,  9)  - GCC 4.0 and later X86_64
 *        -   (5, 10)  - Mac OSX 10.5
 *        -   (5,  9)  - Mac OSX 10.6 and later
 *        -   (5,  4)  - Cygwin i686
 *        -   (1,  7)  - IBM PPC64 XLC 64 bit
 *        -   (1,  8)  - IBM PPC64 XLC 32 bit
 *        -   (1,  2)  - SPARC
 *        -   (4,  1)  - DOS
 *        -
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pftrgt, PFTRGT)(FIXNUM *pis, FIXNUM *pia)
   {int al, st;
    FIXNUM rv;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    al = *pia;
    st = *pis;
    rv = (al != 6);
    if (rv)
       {pa->req_std   = PD_std_standards[st - 1];
        pa->req_align = PD_std_alignments[al - 1];}

    else
       {pa->req_std   = NULL;
        pa->req_align = NULL;

        PD_error("REQUESTED ALIGNMENT NO LONGER EXISTS - PFTRGT",
		 PD_GENERIC);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFOPEN - open a PDBfile
 *        - save the PDBfile pointer in an internal array
 *        - and return an integer index to the pointer if successful
 *        - return 0 otherwise
 */

FIXNUM F77_FUNC(pfopen, PFOPEN)(FIXNUM *pnchr, F77_string name,
				F77_string mode)
   {FIXNUM rv;
    char s[MAXLINE], t[2];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *pnchr);
    SC_FORTRAN_STR_C(t, mode, 1);

    file = PD_open(s, t);
    if (file == NULL)
       rv = 0;

    else
       {file->major_order    = COLUMN_MAJOR_ORDER;
        file->default_offset = 1;

        rv = SC_ADD_POINTER(file);};

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFCLOS - close the PDBfile associated with the integer index
 *        - return TRUE if successful and FALSE otherwise
 */

FIXNUM F77_FUNC(pfclos, PFCLOS)(FIXNUM *fileid)
   {FIXNUM rv;
    PDBfile *file;

    file = SC_DEL_POINTER(PDBfile, *fileid);

    *fileid = 0;

    rv = PD_close(file);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFWULC - write an ULTRA curve into a PDB file */

FIXNUM F77_FUNC(pfwulc, PFWULC)(FIXNUM *fileid, FIXNUM *pnchr,
				F77_string labl, FIXNUM *pnpts,
				double *px, double *py, FIXNUM *pic)
   {int i, n, rv;
    char s[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, labl, *pnchr);

    i    = *pic;
    n    = *pnpts;
    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = PD_wrt_pdb_curve(file, s, n, px, py, i);

/* increment the curve count if no error */
    if (rv)
       (*pic)++;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFWULY - write the y values for an ULTRA curve into a PDB file */

FIXNUM F77_FUNC(pfwuly, PFWULY)(FIXNUM *fileid, FIXNUM *pnchr,
				F77_string labl, FIXNUM *pnpts, FIXNUM *pix,
				double *py, FIXNUM *pic)
   {int i, n, rv;
    char s[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, labl, *pnchr);

    i    = *pic;
    n    = *pnpts;
    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = PD_wrt_pdb_curve_y(file, s, n, (int) *pix, py, i);

/* increment the curve count if no error */
    if (rv)
       (*pic)++;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_BUILD_SET - build and return a set from FORTRAN type information */

static PM_set *_PD_build_set(FIXNUM *si, double *sd, char *sname)
   {int i, j, nd, nde, ne;
    int *maxes;
    void **elem;
    double *data;
    double tmp;
    PM_set *set;

    nd  = si[0];
    nde = si[1];
    ne  = si[2];

    maxes = FMAKE_N(int, nd, "_PD_BUILD_SET:maxes");
    for (i = 0; i < nd; i++)
        maxes[i] = (int) si[i+3];

    elem = FMAKE_N(void *, nde, "_PD_BUILD_SET:elem");
    for (i = 0; i < nde; i++)
        {data    = FMAKE_N(double, ne, "_PD_BUILD_SET:data");
         elem[i] = (void *) data;
         for (j = 0; j < ne; j++)
             {tmp = *sd++;
              *data++ = tmp;};};

    set = PM_mk_set(sname, SC_DOUBLE_S, FALSE,
		    ne, nd, nde, maxes, elem,
		    PM_REAL_Opers, NULL,
		    NULL, NULL, NULL, NULL, NULL, NULL,
		    NULL);

    return(set);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFWMAP - write an PM_mapping into a PDB file
 *        - the domain information (dname, dp, dm) and range 
 *        - information (rname, rp, rm) are structured the same
 *        - and are as follows:
 *        -
 *        - dname : the FORTRAN version of the set name left justified
 *        -
 *        - dp[1]              : the number of characters in dname
 *        - dp[2]              : the dimensionality of the set - nd
 *        - dp[3]              : the dimensionality of the elements - nde
 *        - dp[4]              : the number of elements in the set - ne
 *        - dp[5] ... dp[5+nd] : the sizes in each dimension
 *        -
 *        - dm[1]      - dm[ne]          : values of first component of
 *        -                                elements
 *        -            .
 *        -            .
 *        -            .
 *        -
 *        - dm[nde*ne] - dm[nde*ne + ne] : values of nde'th component of
 *        -                                elements
 */

FIXNUM F77_FUNC(pfwmap, PFWMAP)(FIXNUM *fileid, F77_string dname,
				FIXNUM *dp, double *dm, F77_string rname,
				FIXNUM *rp, double *rm, FIXNUM *pim)
   {FIXNUM rv;
    char s[MAXLINE];
    PM_mapping *f;
    PM_set *domain, *range;
    PDBfile *file;

    rv = TRUE;

    file = SC_GET_POINTER(PDBfile, *fileid);

    if ((SC_hasharr_lookup(file->chart, "PM_mapping") == NULL) &&
	(!PD_def_mapping(file)))
       rv = FALSE;

    else
       {SC_FORTRAN_STR_C(s, dname, dp[0]);
	domain = _PD_build_set(dp+1, dm, s);

	SC_FORTRAN_STR_C(s, rname, rp[0]);
	range  = _PD_build_set(rp+1, rm, s);

	snprintf(s, MAXLINE, "%s->%s", domain->name, range->name);
	f = PM_make_mapping(s, PM_LR_S, domain, range, N_CENT, NULL);

/* disconnect the function pointers or undefined structs/members */
	f->domain->opers = NULL;
	f->range->opers  = NULL;

	if (!PD_put_mapping(file, f, (*pim)++))
	   rv = FALSE;
	else
	   PM_rel_mapping(f, TRUE, TRUE);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFWIMA - write a PG_image into a PDB file */

FIXNUM F77_FUNC(pfwima, PFWIMA)(FIXNUM *fileid, FIXNUM *nchr, F77_string name,
				FIXNUM *pkn, FIXNUM *pkx,
				FIXNUM *pln, FIXNUM *plx,
				double *data, double *pxn, double *pxx,
				double *pyn, double *pyx, FIXNUM *image)
   {int n, kx, lx, kmin, kmax, lmin, k1, k2, l1, l2, k, l, i;
    FIXNUM rv;
    double *pd, *d, xmin, xmax, ymin, ymax, zmin, zmax, z;
    char s[MAXLINE];
    PD_image *im;
    PDBfile *file;

    rv = TRUE;

    file = SC_GET_POINTER(PDBfile, *fileid);

    if ((SC_hasharr_lookup(file->chart, "PG_image") == NULL) &&
	(!PD_def_mapping(file)))
       rv = FALSE;

    else
       {SC_FORTRAN_STR_C(s, name, *nchr);

	xmin = *pxn;
	xmax = *pxx;
	ymin = *pyn;
	ymax = *pyx;
	kmin = *pkn;
	kmax = *pkx;
	lmin = *pln;
	k1   = xmin;
	k2   = xmax;
	l1   = ymin;
	l2   = ymax;
	kx   = k2 - k1 + 1;
	lx   = l2 - l1 + 1;
	n    = kx*lx;
	d    = pd = FMAKE_N(double, n, "PFWIMA:d");
	zmax = -HUGE;
	zmin =  HUGE;
	for (l = l1; l <= l2; l++)
	    for (k = k1; k <= k2; k++)
	        {i     = (l - lmin)*(kmax - kmin + 1) + k - kmin;
		 z     = data[i];
		 zmax  = max(zmax, z);
		 zmin  = min(zmin, z);
		 *pd++ = z;}

	im = PD_make_image(s, "double *", d, kx, lx, 8, xmin, xmax,
			   ymin, ymax, zmin, zmax);

	if (!PD_put_image(file, im, *image))
	   rv = FALSE;
	else
	   PD_rel_image(im);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFWSET - write a PM_set into a PDB file
 *        - the set information (dname, dp, dm) is structured
 *        - as follows:
 *        -
 *        - dname : the FORTRAN version of the set name left justified
 *        -
 *        - dp[1]              : the number of characters in dname
 *        - dp[2]              : the dimensionality of the set - nd
 *        - dp[3]              : the dimensionality of the elements - nde
 *        - dp[4]              : the number of elements in the set - ne
 *        - dp[5] ... dp[5+nd] : the sizes in each dimension
 *        -
 *        - dm[1]      - dm[ne]          : values of first component of
 *        -                                elements
 *        -            .
 *        -            .
 *        -            .
 *        -
 *        - dm[nde*ne] - dm[nde*ne + ne] : values of nde'th component of
 *        -                                elements
 */

FIXNUM F77_FUNC(pfwset, PFWSET)(FIXNUM *fileid, F77_string dname,
				FIXNUM *dp, double *dm)
   {FIXNUM rv;
    char s[MAXLINE];
    PDBfile *file;
    PM_set *set;

    rv = TRUE;

    file = SC_GET_POINTER(PDBfile, *fileid);

    if ((SC_hasharr_lookup(file->chart, "PM_set") == NULL) &&
	(!PD_def_mapping(file)))
       rv = FALSE;

    else
       {SC_FORTRAN_STR_C(s, dname, dp[0]);
	set = _PD_build_set(dp+1, dm, s);

/* disconnect the function pointers or undefined structs/members */
	set->opers = NULL;

	if (!PD_put_set(file, set))
	   rv = FALSE;
	else
	   PM_rel_set(set, FALSE);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFWRAE - write a PM_mapping into a PDB file
 *        - only the range part of the mapping is given
 *        - but additional information is given in INFO such as
 *        - an existence map and centering are included
 *        - the domain (common to many mappings) is written separately
 *        -
 *        - the domain name is the only part of the domain specified
 *        -
 *        - the range information (rname, rp, rm) is structured
 *        - as follows:
 *        -
 *        - rname : the FORTRAN version of the set name left justified
 *        -
 *        - rp[1]              : the number of characters in rname
 *        - rp[2]              : the dimensionality of the set - nd
 *        - rp[3]              : the dimensionality of the elements - nde
 *        - rp[4]              : the number of elements in the set - ne
 *        - rp[5] ... rp[5+nd] : the sizes in each dimension
 *        -
 *        - rm[1]      - rm[ne]          : values of first component of
 *        -                                elements
 *        -            .
 *        -            .
 *        -            .
 *        -
 *        - rm[nde*ne] - rm[nde*ne + ne] : values of nde'th component of
 *        -                                elements
 */

FIXNUM F77_FUNC(pfwrae, PFWRAE)(FIXNUM *fileid, F77_string dname,
				FIXNUM *nchr, F77_string rname,
				FIXNUM *rp, double *rm,
				FIXNUM *inf, FIXNUM *pim)
   {int i;
    FIXNUM rv;
    char d[MAXLINE], r[MAXLINE], s[MAXLINE], name[MAXLINE];
    PM_centering cent, *pcent;
    PM_mapping *f, *link, *pm;
    PM_set *range, *bs;
    pcons *info, *alst;
    PDBfile *file;

    rv = TRUE;

    file = SC_GET_POINTER(PDBfile, *fileid);
    if (inf == NULL)
       info = NULL;
    else
       info = SC_GET_POINTER(pcons, *inf);

    if (SC_hasharr_lookup(file->chart, "PM_mapping") == NULL)
       {if (!PD_def_mapping(file))
           return(FALSE);};

    SC_FORTRAN_STR_C(d, dname, *nchr);
    SC_FORTRAN_STR_C(r, rname, rp[0]);

    pcent = (PM_centering *) SC_assoc(info, "CENTERING");
    cent  = (pcent == NULL) ? N_CENT : *pcent;
    range = _PD_build_set(rp+1, rm, r);

    snprintf(s, MAXLINE, "%s->%s", d, range->name);
    f = PM_make_mapping(s, PM_LR_S, NULL, range, cent, NULL);

/* disconnect the function pointers or undefined structs/members */
    f->range->opers = NULL;

    link = f;
    if (info != NULL)

/* check for mappings to overlay */
       {for (i = 0; TRUE; i++)
	    {snprintf(name, MAXLINE, "OVERLAY[%d]", i);
	     pm = (PM_mapping *) SC_assoc(info, name);
	     if (pm == NULL)
	        break;

	     else
	        {link->next = pm;
		 link       = pm;
		 SC_mark(pm, 1);

		 info = SC_rem_alist(info, name);};};

/* check for boundary sets to overlay */
	for (i = 0; TRUE; i++)
	    {snprintf(name, MAXLINE, "BND[%d]", i);
	     bs = (PM_set *) SC_assoc(info, name);
	     if (bs == NULL)
	        break;

	     else
	        {pm = PM_make_mapping(name, PM_AC_S, bs, NULL,
				      N_CENT, NULL);

		 link->next = pm;
		 link       = pm;
		 SC_mark(pm, 1);

		 info = SC_rem_alist(info, name);};};

/* append all other info to the mapping's info list */
	alst   = (pcons *) f->map;
	alst   = SC_append_alist(alst, info);
	f->map = (void *) alst;};

    if (!PD_put_mapping(file, f, (*pim)++))
       rv = FALSE;
    else
       PM_rel_mapping(f, TRUE, TRUE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFWRAN - write a PM_mapping into a PDB file
 *        - only the range part of the mapping is given
 *        - the domain (common to many mappings) is written separately
 *        -
 *        - the domain name is the only part of the domain specified
 *        -
 *        - the range information (rname, rp, rm) is structured
 *        - as follows:
 *        -
 *        - rname : the FORTRAN version of the set name left justified
 *        -
 *        - rp[1]              : the number of characters in rname
 *        - rp[2]              : the dimensionality of the set - nd
 *        - rp[3]              : the dimensionality of the elements - nde
 *        - rp[4]              : the number of elements in the set - ne
 *        - rp[5] ... rp[5+nd] : the sizes in each dimension
 *        -
 *        - rm[1]      - rm[ne]          : values of first component of
 *        -                                elements
 *        -            .
 *        -            .
 *        -            .
 *        -
 *        - rm[nde*ne] - rm[nde*ne + ne] : values of nde'th component of
 *        -                                elements
 */

FIXNUM F77_FUNC(pfwran, PFWRAN)(FIXNUM *fileid, F77_string dname,
				FIXNUM *nchr, F77_string rname,
				FIXNUM *rp, double *rm, FIXNUM *pim)
   {FIXNUM rv;

    rv = F77_FUNC(pfwrae, PFWRAE)(fileid, dname, nchr,
				  rname, rp, rm,
				  NULL, pim);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFDATT - FORTRAN interface routine to define an attribute */

FIXNUM F77_FUNC(pfdatt, PFDATT)(FIXNUM *fileid, FIXNUM *pna,
				F77_string fattr, FIXNUM *pnt,
				F77_string ftype)
   {int nc;
    FIXNUM rv;
    char lattr[MAXLINE], ltype[MAXLINE];
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *fileid);

    nc = *pna;
    SC_FORTRAN_STR_C(lattr, fattr, nc);

    nc = *pnt;
    SC_FORTRAN_STR_C(ltype, ftype, nc);

    rv = PD_def_attribute(file, lattr, ltype);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFRATT - FORTRAN interface routine to remove an attribute */

FIXNUM F77_FUNC(pfratt, PFRATT)(FIXNUM *fileid, FIXNUM *pna,
				F77_string fattr)
   {int nc;
    FIXNUM rv;
    char lattr[MAXLINE];
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *fileid);

    nc = *pna;
    SC_FORTRAN_STR_C(lattr, fattr, nc);

    rv = PD_rem_attribute(file, lattr);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFSVAT - FORTRAN interface routine to assign an attribute value to
 *        - a variable
 */

FIXNUM F77_FUNC(pfsvat, PFSVAT)(FIXNUM *fileid, FIXNUM *pnv,
				F77_string fvar, FIXNUM *pna,
				F77_string fattr, void *vl)
   {int nc;
    FIXNUM rv;
    char lattr[MAXLINE], lvar[MAXLINE], **lvl;
    PDBfile *file;
    attribute *attr;

    file = SC_GET_POINTER(PDBfile, *fileid);

    nc = *pnv;
    SC_FORTRAN_STR_C(lvar, fvar, nc);

    nc = *pna;
    SC_FORTRAN_STR_C(lattr, fattr, nc);

    attr = PD_inquire_attribute(file, lattr, NULL);
    lvl  = FMAKE(char *, "PFSVAT:lvl");
    if (strcmp(attr->type, "char ***") == 0)
       *lvl = SC_strsavef(vl, "char*:PFSVAT:vl");
    else
       *lvl = vl;

    rv = PD_set_attribute(file, lvar, lattr, (void *) lvl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGVAT - FORTRAN interface routine to get an attribute value of
 *        - a variable
 */

FIXNUM F77_FUNC(pfgvat, PFGVAT)(FIXNUM *fileid, FIXNUM *pnv, F77_string fvar,
				FIXNUM *pna, F77_string fattr, void *vl)
   {int nc;
    FIXNUM rv;
    char lvar[MAXLINE], lattr[MAXLINE];
    void *lvl;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *fileid);

    nc = *pnv;
    SC_FORTRAN_STR_C(lvar, fvar, nc);

    nc = *pna;
    SC_FORTRAN_STR_C(lattr, fattr, nc);

    lvl = PD_get_attribute(file, lvar, lattr);
    if (lvl != NULL)
       {nc  = SC_arrlen(*(char **)lvl);
        memcpy(vl, *(char **)lvl, nc);};

    rv = (lvl != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFFLSH - FORTRAN interface routine to PD_flush */

FIXNUM F77_FUNC(pfflsh, PFFLSH)(FIXNUM *fileid)
   {FIXNUM rv;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = PD_flush(file);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFFAMI - FORTRAN interface routine to PD_family */

FIXNUM F77_FUNC(pffami, PFFAMI)(FIXNUM *fileid, FIXNUM *pf)
   {FIXNUM rv;
    PDBfile *file, *nfile;

    file = SC_DEL_POINTER(PDBfile, *fileid);

    nfile = PD_family(file, (int) *pf);
    if ((nfile != file) && (*pf != 0))
       *fileid = 0;

    rv = SC_ADD_POINTER(nfile);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFCD - FORTRAN interface routine to change currrent working directory */

FIXNUM F77_FUNC(pfcd, PFCD)(FIXNUM *fileid, FIXNUM *nchr, F77_string dirname)
   {FIXNUM rv;
    char dir[MAXLINE+1];
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *fileid);

    if (*nchr > MAXLINE)
       {PD_error("DIRECTORY NAME TOO LONG - PFCD", PD_GENERIC);
        rv = FALSE;}

    else
       {SC_FORTRAN_STR_C(dir, dirname, *nchr);

	rv = PD_cd(file, dir);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFLN - FORTRAN interface routine to create a link to a variable */

FIXNUM F77_FUNC(pfln, PFLN)(FIXNUM *fileid, FIXNUM *ochr, F77_string oname,
			    FIXNUM *nchr, F77_string nname)
   {FIXNUM rv;
    char oldname[MAXLINE+1], newname[MAXLINE+1];
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *fileid);

    if ((*ochr > MAXLINE) || (*nchr > MAXLINE))
       {PD_error("VARIABLE NAME TOO LONG - PFLN", PD_GENERIC);
        return(FALSE);};

    SC_FORTRAN_STR_C(oldname, oname, *ochr);
    SC_FORTRAN_STR_C(newname, nname, *nchr);

    rv = PD_ln(file, oldname, newname);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFLST - FORTRAN interface routine to get an alphabetical list of file
 *       - variables and directories in the current working directory that
 *       - match the specified pattern and type. Previous lists are deleted.
 */

FIXNUM F77_FUNC(pflst, PFLST)(FIXNUM *fileid, FIXNUM *npath, F77_string path,
			      FIXNUM *ntype, F77_string type,
			      FIXNUM *num)
   {FIXNUM rv;
    char *ppath, *ptype;
    char lpath[MAXLINE+1], ltype[MAXLINE+1];
    PDBfile *file;
    PD_smp_state *pa;

    rv = TRUE;

    pa = _PD_get_state(-1);

    pa->n_entries = 0;
    if (pa->outlist != NULL)
       SFREE(pa->outlist);

    file = SC_GET_POINTER(PDBfile, *fileid);

    if (*npath > MAXLINE)
       {PD_error("VARIABLE NAME TOO LONG - PFLST", PD_GENERIC);
        rv = FALSE;}

    else if (*ntype > MAXLINE)
       {PD_error("VARIABLE TYPE TOO LONG - PFLST", PD_GENERIC);
        rv = FALSE;}

    else
       {if (*npath == 0)
	   ppath = NULL;
        else
	   {ppath = lpath;
	    SC_FORTRAN_STR_C(lpath, path, *npath);};

	if (*ntype == 0)
	   ptype = NULL;
	else
	   {ptype = ltype;
	    SC_FORTRAN_STR_C(ltype, type, *ntype);};

	pa->outlist = PD_ls(file, ppath, ptype, &pa->n_entries);

	if ((pa->outlist == NULL) && (pa->err[0] != '\0'))
	   rv = FALSE;
	else
	   *num = pa->n_entries;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGLS - FORTRAN interface routine to get the name of the nth variable or
 *       - directory in the internal sorted table set up by a call to PFLST.
 */

FIXNUM F77_FUNC(pfgls, PFGLS)(FIXNUM *ord, FIXNUM *len, F77_string name)
   {int n;
    FIXNUM rv;
    char *s;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    n = *ord - 1;

    if ((n < 0) || (n >= pa->n_entries))
       {PD_error("VARIABLE ORDINAL OUT OF RANGE - PFGLS", PD_GENERIC);
        rv = FALSE;}
    else
       {s = pa->outlist[n];

	strcpy(SC_F77_C_STRING(name), s);
	*len = strlen(s);

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFDLS - FORTRAN interface routine to delete internal table set up by a call
 *       - to PFLST.
 */

FIXNUM F77_FUNC(pfdls, PFDLS)(void)
   {FIXNUM rv;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    rv = TRUE;

    if (pa->outlist != NULL)
       SFREE(pa->outlist);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFMKDR - FORTRAN interface routine to create a directory */

FIXNUM F77_FUNC(pfmkdr, PFMKDR)(FIXNUM *fileid, FIXNUM *nchr,
				F77_string dirname)
   {FIXNUM rv;
    char dir[MAXLINE+1];
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *fileid);

    if (*nchr > MAXLINE)
       {PD_error("DIRECTORY NAME TOO LONG - PFMKDR", PD_GENERIC);
        return(FALSE);};

    SC_FORTRAN_STR_C(dir, dirname, *nchr);

    rv = PD_mkdir(file, dir);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFISDR - FORTRAN interface routine to test for a directory */

FIXNUM F77_FUNC(pfisdr, PFISDR)(FIXNUM *fileid, FIXNUM *nchr,
				F77_string dirname)
   {FIXNUM rv;
    char dir[MAXLINE+1];
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *fileid);

    if (*nchr > MAXLINE)
       {PD_error("DIRECTORY NAME TOO LONG - PFISDR", PD_GENERIC);
        return(FALSE);};

    SC_FORTRAN_STR_C(dir, dirname, *nchr);

    rv = PD_isdir(file, dir);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFPWD - FORTRAN interface routine to return currrent working directory */

FIXNUM F77_FUNC(pfpwd, PFPWD)(FIXNUM *fileid, FIXNUM *nchr, F77_string cwd)
   {FIXNUM rv;
    char *pwd, *lcwd;
    PDBfile *file;

    rv = TRUE;

    file = SC_GET_POINTER(PDBfile, *fileid);
    lcwd = SC_F77_C_STRING(cwd);

    pwd = PD_pwd(file);
    if (pwd == NULL)
       rv = FALSE;

    else
       {strcpy(lcwd, pwd);
	*nchr = strlen(lcwd);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFNCIN - FORTRAN interface routine to convert a buffer full of data 
 *          in the format of a remote machine of a different architecture
 *          to the format of the host machine.                           
 */

FIXNUM F77_FUNC(pfncin, PFNCIN)(void *out, void *in,
				FIXNUM *nitems, FIXNUM *chart, FIXNUM *pntyp,
				F77_string type)
   {FIXNUM rv;
    char s[MAXLINE];
    hasharr *chrt;

    rv = TRUE;

    SC_FORTRAN_STR_C(s, type, *pntyp);

    chrt = SC_GET_POINTER(hasharr, *chart);

    PN_conv_in(out, in, s, *nitems, chrt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFNCOT - FORTRAN interface routine to convert a buffer full of
 *          data in the host machine format to the format of a machine
 *          with a different architecture.
 */

FIXNUM F77_FUNC(pfncot, PFNCOT)(void *out, void *in,
				FIXNUM *nitems, FIXNUM *chart, FIXNUM *pntyp,
				F77_string type)
   {FIXNUM rv;
    char s[MAXLINE];
    hasharr *chrt;

    rv = TRUE;

    SC_FORTRAN_STR_C(s, type, *pntyp);

    chrt = SC_GET_POINTER(hasharr, *chart);

    PN_conv_out(out, in, s, *nitems, chrt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFNTGT - FORTRAN interface to a routine to allocate, initialize, and
 *          return a structure chart
 */

FIXNUM F77_FUNC(pfntgt, PFNTGT)(FIXNUM *pis, FIXNUM *pia)
   {int al, st, ret;
    FIXNUM rv;
    hasharr *chart;

    al = *pia;
    st = *pis;
    ret = (al != 6);
    if (ret)
       {chart = PN_target(PD_std_standards[st - 1], PD_std_alignments[al - 1]);
        rv    = SC_ADD_POINTER(chart);}

    else
       {PD_error("REQUESTED ALIGNMENT NO LONGER EXISTS - PFNTGT", PD_GENERIC);
        rv = -1;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFRDBT - FORTRAN interface to a routine to read a bitstream
 *
 */

FIXNUM F77_FUNC(pfrdbt, PFRDBT)(FIXNUM *fileid, FIXNUM *nchrnm,
				F77_string name, FIXNUM *nchrtp,
				F77_string type, FIXNUM *nitems,
				FIXNUM *sgned, FIXNUM *nbits, FIXNUM *padsz,
				FIXNUM *fpp, FIXNUM *offs, FIXNUM *pan,
				void *pdata)
   {long numitems;
    FIXNUM rv;
    char s[MAXLINE], t[MAXLINE];
    char *dataout;
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *nchrnm);
    SC_FORTRAN_STR_C(t, type, *nchrtp);

    file = SC_GET_POINTER(PDBfile, *fileid);

    numitems = *nitems;

    rv = PD_read_bits(file, s, t, *nitems, *sgned, *nbits, *padsz,
		       *fpp, *offs, (long *) pan, &dataout);
    if (rv != 0)
       {if (strcmp(t, "long") == 0)
           memcpy(pdata, dataout, numitems * sizeof(long));
        else if ((strcmp(t, "int") == 0) || (strcmp(t, "integer") == 0))
           memcpy(pdata, dataout, numitems * sizeof(int));
        else if (strcmp(t, "short") == 0)
           memcpy(pdata, dataout, numitems * sizeof(short));
        else if (strcmp(t, "char")  == 0)
           memcpy(pdata, dataout, numitems);
        else
           rv = FALSE;}

    if (rv)
       SFREE(dataout);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFSMJO - set the major order for the given file
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pfsmjo, PFSMJO)(FIXNUM *fileid, FIXNUM *pval)
   {FIXNUM rv;
    PDBfile *file;

    rv   = FALSE;
    file = SC_GET_POINTER(PDBfile, *fileid);

    if (file != NULL)
       {file->major_order = (PD_major_order) *pval;
	rv                = TRUE;};

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGMJO - get the major order for the given file
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pfgmjo, PFGMJO)(FIXNUM *fileid, FIXNUM *pval)
   {FIXNUM rv;
    PDBfile *file;

    rv   = FALSE;
    file = SC_GET_POINTER(PDBfile, *fileid);

    if (file != NULL)
       {*pval = file->major_order;
	rv    = TRUE;};

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFRSPL - reset the pointer list
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pfrspl, PFRSPL)(FIXNUM *fileid)
   {FIXNUM rv;
    PDBfile *file;

    rv   = FALSE;
    file = SC_GET_POINTER(PDBfile, *fileid);

    if (file != NULL)
       rv = PD_reset_ptr_list(file);

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGFLN - return the byte length of the given file
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pfgfln, PFGFLN)(FIXNUM *fileid, FIXNUM *pln)
   {FIXNUM rv;
    PDBfile *file;

    rv   = FALSE;
    file = SC_GET_POINTER(PDBfile, *fileid);

    if (file != NULL)
       *pln = PD_get_file_length(file);

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFDNRM - wrapper over PD_fix_denorm
 *        - return TRUE iff successful
 *        - return FALSE if not a floating point type
 */

FIXNUM F77_FUNC(pfdnrm, PFDNRM)(FIXNUM *istd, FIXNUM *pnc, F77_string type,
				FIXNUM *plen, void *vr) 
   {int is;
    long n;
    FIXNUM rv;
    data_standard* std;
    char t[MAXLINE];

    is = *istd;
    n  = *plen;
    SC_FORTRAN_STR_C(t, type, *pnc);

    if (is == -1)
       std = NULL;
    else
       std = PD_std_standards[is - 1];

    rv = PD_fix_denorm(std, t, n, vr);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFRENT - wrapper over PD_remove_entry
 *        - return TRUE iff successful
 */

FIXNUM F77_FUNC(pfrent, PFRENT)(FIXNUM *fileid, FIXNUM *nchrnm,
				F77_string name)
   {FIXNUM rv;
    char lnam[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(lnam, name, *nchrnm);

    file = SC_GET_POINTER(PDBfile, *fileid);

    rv = PD_remove_entry(file, lnam);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

