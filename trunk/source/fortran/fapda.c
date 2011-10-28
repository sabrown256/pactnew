/*
 * FAPDA.C - FORTRAN interface routines for PDBLib (in PDBX)
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "fpact.h"

#ifndef FF_INT_SIZE_PTR_DIFFER

# define _PD_append_aux(file, name, type, space, nd, dims)                   \
    ((FIXNUM) PD_append_as_alt(file, name, type, space, nd, (long *) dims))

# define _PD_defent_aux(file, name, type, nd, dims)                          \
    ((FIXNUM) PD_defent_alt(file, name, type, nd, (long *) dims))

#endif

/*--------------------------------------------------------------------------*/

#ifdef FF_INT_SIZE_PTR_DIFFER

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

    pi = CMAKE_N(long, ne);
    for (i = 0; i < ne; i++)
        pi[i] = ind[i];

    ret = _PD_indexed_read_as(file, fullpath, type, vr, nd, pi, ep);

    CFREE(pi);

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
    pi = CMAKE_N(long, ne);
    for (i = 0; i < ne; i++)
        pi[i] = dims[i];

    ret = PD_write_as_alt(file, name, typi, typo, space, nd, pi);

    CFREE(pi);

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
    pi = CMAKE_N(long, ne);
    for (i = 0; i < ne; i++)
        pi[i] = dims[i];

    ret = PD_append_as_alt(file, name, type, space, nd, pi);

    CFREE(pi);

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
    pi = CMAKE_N(long, ne);
    for (i = 0; i < ne; i++)
        pi[i] = dims[i];

    ep = PD_defent_alt(file, name, type, nd, pi);

    CFREE(pi);

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

/* _PD_GET_LOCAL_SET_PARAMS - return a long version of ADP */

int *_PD_get_local_set_params(FIXNUM *adp)
   {int i, ne;
    int *ldp;

    ne = 5 + adp[2];
    ldp = CMAKE_N(int, ne);
    for (i = 0; i < ne; i++)
        ldp[i] = adp[i];

    return(ldp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGERR - FORTRAN interface routine to fetch PD_ERR */

FIXNUM FF_ID(pfgerr, PFGERR)(FIXNUM *snce, char *err)
   {FIXNUM rv;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    SC_strncpy(err, *snce, pa->err, MAXLINE);

    *snce = strlen(pa->err);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGFNM - FORTRAN interface routine to fetch the name of the file */

FIXNUM FF_ID(pfgfnm, PFGFNM)(FIXNUM *sfid, FIXNUM *sncn, char *name)
   {PDBfile *file;
    int nc, lc;
    FIXNUM rv;

    nc = *sncn;

    file = SC_GET_POINTER(PDBfile, *sfid);

    lc = strlen(file->name);
    if (lc != nc)
       *sncn = lc;

    if (nc >= lc)
       {strncpy(name, file->name, nc);
        rv = TRUE;}
    else
       {PD_error("BUFFER TOO SMALL TO HOLD FILE NAME - PFGFNM", PD_GENERIC);
	rv = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGFDT - FORTRAN interface routine to fetch the file creation date */

FIXNUM FF_ID(pfgfdt, PFGFDT)(FIXNUM *sfid, FIXNUM *sncd, char *date)
   {PDBfile *file;
    int nc, lc;
    FIXNUM rv;

    nc = *sncd;

    file = SC_GET_POINTER(PDBfile, *sfid);

    lc = strlen(file->date);
    if (lc != nc)
       *sncd = lc;

    if (nc >= lc)
       {SC_strncpy(date, *sncd, file->date, nc);
        rv = TRUE;}
    else
       {PD_error("BUFFER TOO SMALL TO HOLD FILE DATE - PFGFDT", PD_GENERIC);
	rv = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGCSM - FORTRAN interface routine to fetch file->use_cksum */

FIXNUM FF_ID(pfgcsm, PFGCSM)(FIXNUM *sfid, FIXNUM *sv)
   {FIXNUM rv;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);

    rv = file->use_cksum;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFSCSM - FORTRAN interface routine to set file->use_cksum */

FIXNUM FF_ID(pfscsm, PFSCSM)(FIXNUM *sfid, FIXNUM *sv)
   {FIXNUM rv;
    PD_checksum_mode v;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);

    v = (PD_checksum_mode) *sv;

    rv = PD_activate_cksum(file, v);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGTPT - FORTRAN interface routine to fetch file->track_pointers */

FIXNUM FF_ID(pfgtpt, PFGTPT)(FIXNUM *sfid, FIXNUM *sv)
   {FIXNUM rv;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);

    rv = file->track_pointers;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFSTPT - FORTRAN interface routine to set file->track_pointers */

FIXNUM FF_ID(pfstpt, PFSTPT)(FIXNUM *sfid, FIXNUM *sv)
   {FIXNUM rv;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);

    rv = *sv;
    file->track_pointers = rv;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGBFS - FORTRAN interface routine to fetch PD_buffer_size */

FIXNUM FF_ID(pfgbfs, PFGBFS)(void)
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

FIXNUM FF_ID(pfsbfs, PFSBFS)(FIXNUM *ssz)
   {FIXNUM rv;
    int64_t sz;
    PD_smp_state *pa;

    _PD_init_state(FALSE);

    pa = _PD_get_state(-1);

    sz              = *ssz;
    pa->buffer_size = sz;
    rv              = sz;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGFMV - FORTRAN interface routine to fetch PD_default_format_version */

FIXNUM FF_ID(pfgfmv, PFGFMV)(FIXNUM *sv)
   {FIXNUM rv;

    *sv = PD_get_fmt_version();

    rv = *sv;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFSFMV - FORTRAN interface routine to set PD_default_format_version */

FIXNUM FF_ID(pfsfmv, PFSFMV)(FIXNUM *sv)
   {FIXNUM rv;

    PD_set_fmt_version(*sv);

    rv = *sv;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGOFF - FORTRAN interface routine to fetch
 *        - file's default_offset
 */

FIXNUM FF_ID(pfgoff, PFGOFF)(FIXNUM *sfid)
   {FIXNUM rv;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);

    rv = file->default_offset;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFSOFF - FORTRAN interface routine to set
 *        - file's default_offset
 */

FIXNUM FF_ID(pfsoff, PFSOFF)(FIXNUM *sfid, FIXNUM *sv)
   {FIXNUM rv;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);

    file->default_offset = *sv;
    rv = file->default_offset;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGMXS - FORTRAN interface routine to fetch
 *        - file's maximum size
 */

FIXNUM FF_ID(pfgmxs, PFGMXS)(FIXNUM *sfid)
   {FIXNUM rv;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);

    rv = file->maximum_size;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFSMXS - FORTRAN interface routine to set
 *        - file's maximum size
 */

FIXNUM FF_ID(pfsmxs, PFSMXS)(FIXNUM *sfid, FIXNUM *sv)
   {FIXNUM rv;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);

    file->maximum_size = *sv;
    rv = file->maximum_size;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGMOD - FORTRAN interface routine to fetch
 *        - file's mode
 */

FIXNUM FF_ID(pfgmod, PFGMOD)(FIXNUM *sfid)
   {FIXNUM rv;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);

    rv = file->mode;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFINTH - FORTRAN interface routine to initialize pdblib for threads
 * 
 */

FIXNUM FF_ID(pfinth, PFINTH)(FIXNUM *snt, PFTid tid)
   {int nt;
    PFTid ltid;
    
    nt = (int)(*snt);
    
    if (tid == 0)
       ltid = NULL;
    else
       ltid = tid;  

    PD_init_threads(nt, ltid);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFVART - FORTRAN interface routine to set up a list of file variables
 *        - sorted in the specified order
 *        - returns 1 if successful and 0 otherwise
 */

FIXNUM FF_ID(pfvart, PFVART)(FIXNUM *sfid, FIXNUM *sord, FIXNUM *sn)
   {int entries;
    int (*fun)(haelem **v, int n);
    FIXNUM rv;
    PD_symbol_order ord;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);
    ord  = (PD_symbol_order) *sord;

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
       {*sn = entries;
        rv  = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFDVAR - FORTRAN interface routine to delete internal table set up by a
 *        - call to PFVART.
 */

FIXNUM FF_ID(pfdvar, PFDVAR)(void)
   {

    SC_free_hash_dump();

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGVAR - get name of the nth variable in the internal sorted table set up
 *        - by a call to PFVART. return 1 if successful and 0 otherwise.
 */

FIXNUM FF_ID(pfgvar, PFGVAR)(FIXNUM *sn, FIXNUM *sncn, char *name)
   {int n;
    FIXNUM rv;
    char *s;

    n = *sn - 1;
    s = SC_get_entry(n);
    if (s == NULL)
       {PD_error("VARIABLE ORDINAL OUT OF RANGE - PFGVAR", PD_GENERIC);
	rv = FALSE;}

    else
       {strcpy(name, s);
        *sncn = strlen(s);
        rv    = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFIVAR - inquire about a variable in a PDBfile
 *        - this probably won't do everything but it will do some of the
 *        - useful stuff
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(pfivar, PFIVAR)(FIXNUM *sfid, FIXNUM *sncn, char *name,
			     FIXNUM *snct, char *type,
			     FIXNUM *ssz, FIXNUM *snd, FIXNUM *dims)
   {FIXNUM nd, rv;
    char s[MAXLINE], *ltype;
    syment *ep;
    dimdes *pd;
    PDBfile *file;
    PD_smp_state *pa;

    SC_FORTRAN_STR_C(s, name, *sncn);

    ltype = type;

    file = SC_GET_POINTER(PDBfile, *sfid);

    pa = _PD_get_state(-1);

    ep = _PD_effective_ep(file, s, TRUE, NULL);
    if (ep == NULL)
       {snprintf(pa->err, MAXLINE,
		 "ERROR: VARIABLE %s NOT FOUND - PFIVAR\n", s);
        rv = FALSE;}

    else
       {strcpy(ltype, PD_entry_type(ep));
	*snct = strlen(ltype);
	*ssz = PD_entry_number(ep);

	for (nd = 0, pd = PD_entry_dimensions(ep); pd != NULL; pd = pd->next)
	    {dims[nd++] = pd->index_min;
	     dims[nd++] = pd->index_max;};
	*snd = nd/2;

	_PD_rl_syment_d(ep);

	rv = TRUE;};

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFCTYP - copy a type definition from source file to destination file
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(pfctyp, PFCTYP)(FIXNUM *ssfid, FIXNUM *sdfid,
			     FIXNUM *snct, char *type)
   {FIXNUM rv;
    char ty[MAXLINE];
    PDBfile *sf, *df;

    SC_FORTRAN_STR_C(ty, type, *snct);

    sf = SC_GET_POINTER(PDBfile, *ssfid);
    df = SC_GET_POINTER(PDBfile, *sdfid);

    rv = PD_copy_type(sf, df, ty);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFITYP - inquire about a type in a PDBfile
 *        - this probably won't do everything but it will do some of the
 *        - useful stuff
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(pfityp, PFITYP)(FIXNUM *sfid, FIXNUM *snct, char *type,
			     FIXNUM *ssz, FIXNUM *salgn, FIXNUM *sind)
   {FIXNUM rv;
    char s[MAXLINE];
    defstr *dp;
    PDBfile *file;
    PD_smp_state *pa;

    rv = TRUE;

    SC_FORTRAN_STR_C(s, type, *snct);

    file = SC_GET_POINTER(PDBfile, *sfid);

    pa = _PD_get_state(-1);

    dp = PD_inquire_type(file, s);
    if (dp == NULL)
       {snprintf(pa->err, MAXLINE,
		 "ERROR: TYPE %s NOT FOUND - PFITYP\n", s);
        rv = FALSE;}

    else
       {*ssz = dp->size;
	*salgn = dp->alignment;
	*sind  = dp->n_indirects;};

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFIMBR - inquire about the nth member of a type in a PDBfile
 *        - return the member in the space provided
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(pfimbr, PFIMBR)(FIXNUM *sfid, FIXNUM *snct, char *type,
			     FIXNUM *sn, FIXNUM *ssz, char *space)
   {int i, n, sz, nc;
    FIXNUM rv;
    char s[MAXLINE], *ps;
    defstr *dp;
    memdes *desc;
    PDBfile *file;
    PD_smp_state *pa;

    SC_FORTRAN_STR_C(s, type, *snct);

    file = SC_GET_POINTER(PDBfile, *sfid);
    rv   = TRUE;

    pa = _PD_get_state(-1);

    dp = PD_inquire_type(file, s);
    if (dp == NULL)
       {snprintf(pa->err, MAXLINE,
		 "ERROR: TYPE %s NOT FOUND - PFIMBR\n", s);
	rv = FALSE;}

    else
       {n = *sn;
	for (i = 1, desc = dp->members;
	     (i < n) && (desc != NULL);
	     i++, desc = desc->next);

	if (desc != NULL)
	   {ps = desc->member;
	    nc = strlen(ps);

	    sz = *ssz;
	    if (sz != nc)
	       *ssz = nc;

	    if (sz >= nc)
	       strncpy(space, ps, sz);
	    else
	       {PD_error("BUFFER TOO SMALL TO HOLD DESCRIPTION - PFIMBR",
			 PD_GENERIC);
		rv = FALSE;};}
	else
	   {*ssz = -1;
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

FIXNUM FF_ID(pfqmbr, PFQMBR)(FIXNUM *sfid, FIXNUM *snct, char *type,
			     FIXNUM *sn, FIXNUM *sncn, char *name,
			     FIXNUM *snco, char *tout,
			     FIXNUM *snd, FIXNUM *dims)
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

    SC_FORTRAN_STR_C(s, type, *snct);

    file = SC_GET_POINTER(PDBfile, *sfid);

    pa = _PD_get_state(-1);

    dp = PD_inquire_type(file, s);
    if (dp == NULL)
       {snprintf(pa->err, MAXLINE,
		 "ERROR: TYPE %s NOT FOUND - PFQMBR\n", s);
        return(FALSE);};

    n = *sn;
    for (i = 1, desc = dp->members;
         (i < n) && (desc != NULL);
         i++, desc = desc->next);

    if (desc != NULL)
       {ps = desc->name;
        nc = strlen(ps);

        sz = *sncn;
        if (sz != nc)
	   *sncn = nc;

        if (sz >= nc)
           SC_strncpy(name, *sn, ps, sz);
        else
           {err = (FIXNUM) TRUE;
            SC_strcat(errmsg, MAXLINE, "NAME BUFFER TOO SMALL TO HOLD NAME\n");};

        ps = desc->type;
        nc = strlen(ps);

        sz = *snco;
        if (sz != nc)
	   *snco = nc;

        if (sz >= nc)
           SC_strncpy(tout, *snco, ps, sz);
        else
           {err = (FIXNUM) TRUE;
            SC_strcat(errmsg, MAXLINE, "TYPE BUFFER TOO SMALL TO HOLD TYPE\n");};

        ndims = desc->number;
 
        if ((int) *snd > (ndims * 2))
           {for (n = 0, dimens = desc->dimensions; 
                 dimens != NULL; 
                 dimens = dimens->next)
                {dims[n++] = dimens->index_min;
                 dims[n++] = dimens->index_max;};
            *snd = n/2;}
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

FIXNUM FF_ID(pfread, PFREAD)(FIXNUM *sfid, FIXNUM *sncn, char *name,
			     void *space)
   {FIXNUM rv;
    char s[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *sncn);

    file = SC_GET_POINTER(PDBfile, *sfid);

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

FIXNUM FF_ID(pfrdas, PFRDAS)(FIXNUM *sfid, FIXNUM *sncn, char *name,
			     FIXNUM *snct, char *type, void *space)
   {FIXNUM rv;
    char s[MAXLINE], t[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *sncn);
    SC_FORTRAN_STR_C(t, type, *snct);

    file = SC_GET_POINTER(PDBfile, *sfid);

    rv = PD_read_as(file, s, t,  space);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFRDAI - FORTRAN interface to PD_read_as_dwim */

FIXNUM FF_ID(pfrdai, PFRDAI)(FIXNUM *sfid, FIXNUM *sncn, char *name,
			     FIXNUM *snct, char *type,
			     FIXNUM *sni, void *space)
   {inti ni;
    FIXNUM rv;
    char s[MAXLINE], t[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *sncn);
    SC_FORTRAN_STR_C(t, type, *snct);

    ni   = (inti) *sni;
    file = SC_GET_POINTER(PDBfile, *sfid);

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

FIXNUM FF_ID(pfrptr, PFRPTR)(FIXNUM *sfid, FIXNUM *sncn, char *name,
			     FIXNUM *sni, void *space)
   {FIXNUM rv;
    char s[MAXLINE], fullpath[MAXLINE], *type;
    syment *ep;
    PDBfile *file;
    PD_smp_state *pa;

    SC_FORTRAN_STR_C(s, name, *sncn);

    file = SC_GET_POINTER(PDBfile, *sfid);

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
	    inti nir, nis, nb;
	    intb bpi;

	    rv = _PD_hyper_read(file, fullpath, type, ep, &vr);

	    strcpy(s, type);
	    PD_dereference(s);
	    bpi = _PD_lookup_size(s, file->host_chart);
	    nis = (*sni)*bpi;
	    nir = SC_arrlen(vr);
	    if (nir > nis)
	       {rv = FALSE;
	        nb = nis;}
	    else
	       nb = nir;

	    memcpy(space, vr, nb);

	    CFREE(vr);

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

FIXNUM FF_ID(pfptrd, PFPTRD)(FIXNUM *sfid, FIXNUM *sncn, char *name,
			     void *space, FIXNUM *ind)
   {FIXNUM rv;
    char s[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *sncn);

    file = SC_GET_POINTER(PDBfile, *sfid);

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

FIXNUM FF_ID(pfrdad, PFRDAD)(FIXNUM *sfid, FIXNUM *sncn, char *name,
			     FIXNUM *snct, char *type,
			     void *space, FIXNUM *ind)
   {FIXNUM rv;
    char s[MAXLINE], t[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *sncn);
    SC_FORTRAN_STR_C(t, type, *snct);

    file = SC_GET_POINTER(PDBfile, *sfid);

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

FIXNUM FF_ID(pfappa, PFAPPA)(FIXNUM *sfid, FIXNUM *sncn, char *name,
			     void *space)
   {FIXNUM rv;
    char s[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *sncn);

    file = SC_GET_POINTER(PDBfile, *sfid);

    rv = PD_append(file, s, space);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFAPAS - append to a variable in a PDBfile
 *        - convert from TYPE to the type of the existing data
 *        - ASCII encode the dimensions in the name
 *        - this probably won't do everything but it will do some of the
 *        - useful stuff
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(pfapas, PFAPAS)(FIXNUM *sfid, FIXNUM *sncn, char *name,
			     FIXNUM *snct, char *type, void *space)
   {FIXNUM rv;
    char s[MAXLINE], t[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *sncn);
    SC_FORTRAN_STR_C(t, type, *snct);

    file = SC_GET_POINTER(PDBfile, *sfid);

    rv = PD_append_as(file, s, t, space);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFAPPD - append to a variable in a PDBfile
 *        - the dimensions are specified in an array of (min,max) pairs
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(pfappd, PFAPPD)(FIXNUM *sfid, FIXNUM *sncn, char *name,
			     void *space, FIXNUM *snd, FIXNUM *dims)
   {FIXNUM rv;
    char s[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *sncn);

    file = SC_GET_POINTER(PDBfile, *sfid);

    rv = _PD_append_aux(file, s, (char *) NULL, space, *snd, dims);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFAPAD - append to a variable in a PDBfile
 *        - convert from TYPE to the type of the existing data
 *        - the dimensions are specified in an array of (min,max) pairs
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(pfapad, PFAPAD)(FIXNUM *sfid, FIXNUM *sncn, char *name,
			     FIXNUM *snct, char *type, void *space,
			     FIXNUM *snd, FIXNUM *dims)
   {FIXNUM rv;
    char s[MAXLINE], t[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *sncn);
    SC_FORTRAN_STR_C(t, type, *snct);

    file = SC_GET_POINTER(PDBfile, *sfid);

    rv = _PD_append_aux(file, s, t, space, *snd, dims);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFWRTA - write a variable to a PDBfile
 *        - ASCII encode the dimensions in the name
 *        - this probably won't do everything but it will do some of the
 *        - useful stuff
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(pfwrta, PFWRTA)(FIXNUM *sfid, FIXNUM *sncn, char *name,
			     FIXNUM *snct, char *type, void *space)
   {FIXNUM rv;
    char s[MAXLINE], t[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *sncn);
    SC_FORTRAN_STR_C(t, type, *snct);

    file = SC_GET_POINTER(PDBfile, *sfid);

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

FIXNUM FF_ID(pfwras, PFWRAS)(FIXNUM *sfid, FIXNUM *sncn, char *name,
			     FIXNUM *snci, char *intype,
			     FIXNUM *snco, char *outtype, void *space)
   {FIXNUM rv;
    char s[MAXLINE], t1[MAXLINE], t2[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *sncn);
    SC_FORTRAN_STR_C(t1, intype, *snci);
    SC_FORTRAN_STR_C(t2, outtype, *snco);

    file = SC_GET_POINTER(PDBfile, *sfid);

    rv = PD_write_as(file, s, t1, t2, space);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFWRTD - write a variable to a PDBfile
 *        - the dimensions are specified in an array of (min,max) pairs
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(pfwrtd, PFWRTD)(FIXNUM *sfid, FIXNUM *sncn, char *name,
			     FIXNUM *snct, char *type, void *space,
			     FIXNUM *snd, FIXNUM *dims)
   {FIXNUM rv;
    char s[MAXLINE], t[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *sncn);
    SC_FORTRAN_STR_C(t, type, *snct);

    file = SC_GET_POINTER(PDBfile, *sfid);

    rv = _PD_write_aux(file, s, t, t, space, *snd, dims);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFWRAD - write a variable INTYPE to a PDBfile as type OUTTYPE
 *        - the dimensions are specified in an array of (min,max) pairs
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(pfwrad, PFWRAD)(FIXNUM *sfid, FIXNUM *sncn, char *name,
			     FIXNUM *snci, char *intype,
			     FIXNUM *snco, char *outtype, void *space,
			     FIXNUM *snd,  FIXNUM *dims)
   {FIXNUM rv;
    char s[MAXLINE], ti[MAXLINE], to[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *sncn);
    SC_FORTRAN_STR_C(ti, intype, *snci);
    SC_FORTRAN_STR_C(to, outtype, *snco);

    file = SC_GET_POINTER(PDBfile, *sfid);

    rv = _PD_write_aux(file, s, ti, to, space, *snd, dims);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFDEFA - define a symbol table entry and reserve disk space
 *        - dimension information is encoded in name
 *        - return TRUE if successful, FALSE otherwise
 */

FIXNUM FF_ID(pfdefa, PFDEFA)(FIXNUM *sfid, FIXNUM *sncn, char *name,
			     FIXNUM *snct, char *type)
   {FIXNUM rv;
    char s[MAXLINE], t[MAXLINE];
    syment *ep;
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *sncn);
    SC_FORTRAN_STR_C(t, type, *snct);

    file = SC_GET_POINTER(PDBfile, *sfid);

    ep = PD_defent(file, s, t);
    rv = (ep != NULL);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFDEFD - define a symbol table entry and reserve disk space
 *        - dimension information is provided in index range array
 *        - return TRUE if successful, FALSE otherwise
 */

FIXNUM FF_ID(pfdefd, PFDEFD)(FIXNUM *sfid, FIXNUM *sncn, char *name,
			     FIXNUM *snct, char *type,
			     FIXNUM *snd, FIXNUM *dims)
   {FIXNUM rv;
    char s[MAXLINE], t[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *sncn);
    SC_FORTRAN_STR_C(t, type, *snct);

    file = SC_GET_POINTER(PDBfile, *sfid);

    rv = _PD_defent_aux(file, s, t, *snd, dims);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFDEFS - define a structure for the FORTRAN interface
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(pfdefs, PFDEFS)(FIXNUM *sfid,
			     FIXNUM *sncn, char *name, ...)
   {FIXNUM n, rv;
    FIXNUM *pn;
    char *ps;
    char *ptype, *type;
    char s[MAXLINE], lname[MAXLINE];
    hasharr *fchrt;
    defstr *dp;
    memdes *desc, *lst, *prev;
    PDBfile *file;
    PD_smp_state *pa;

    SC_VA_START(name);

    file = SC_GET_POINTER(PDBfile, *sfid);

    SC_FORTRAN_STR_C(lname, name, *sncn);

    pa = _PD_get_state(-1);

    prev  = NULL;
    lst   = NULL;
    fchrt = file->chart;
    for (pn = SC_VA_ARG(FIXNUM *); (n = *pn) != 0;
         pn = SC_VA_ARG(FIXNUM *))
        {ps = SC_VA_ARG(char *);
         SC_FORTRAN_STR_C(s, ps, n);

         desc  = _PD_mk_descriptor(s, file->default_offset);
         type  = CSTRSAVE(s);
         ptype = SC_firsttok(type, " \n");
         if (SC_hasharr_lookup(fchrt, ptype) == NULL)
            if ((strcmp(ptype, lname) != 0) || !_PD_indirection(s))
               {snprintf(pa->err, MAXLINE,
			 "ERROR: %s BAD MEMBER TYPE - PFDEFS\n",
			 s);
                return(FALSE);};
         CFREE(type);
         if (lst == NULL)
            lst = desc;
         else
            prev->next = desc;
         prev = desc;};
    SC_VA_END;

/* install the type in both charts */
    rv = TRUE;
    dp = _PD_defstr_inst(file, lname, STRUCT_KIND, lst,
			 NO_ORDER, NULL, NULL, PD_CHART_HOST);
    if (dp == NULL)
       {PD_error("CAN'T HANDLE PRIMITIVE TYPE - PFDEFS", PD_GENERIC);
        rv = FALSE;};

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFDEFT - define a structure for the FORTRAN interface
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(pfdeft, PFDEFT)(FIXNUM *sfid, FIXNUM *sncn, char *name,
			     FIXNUM *snm, FIXNUM *nc, char *nm)
   {int i, n, indx, mc;
    FIXNUM rv;
    char lname[MAXLINE], bf[MAXLINE], *lnm, **members;
    defstr *dp;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);
    lnm  = nm;

    SC_FORTRAN_STR_C(lname, name, *sncn);

    n = *snm;
    members = CMAKE_N(char *, n);
    for (i = 0; i < n; i++)
        {indx = nc[2*i];
         mc   = nc[2*i + 1];

         SC_strncpy(bf, MAXLINE, lnm+indx, mc);

         members[i] = CSTRSAVE(bf);};

/* install the type in both charts */
    dp = PD_defstr_alt(file, lname, n, members);

    for (i = 0; i < n; i++)
        CFREE(members[i]);
    CFREE(members);

    rv = (dp != NULL);
    
    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFFREE - wrapper over PD_free
 *        - NOTE: SPACE must be a pointer to an object with the type
 *        - given by TYPE (PDBLib will allocated space if necessary)!
 */

FIXNUM FF_ID(pffree, PFFREE)(FIXNUM *sfid, FIXNUM *snct, char *type,
			     void *space)
   {FIXNUM rv;
    PDBfile *file;
    char s[MAXLINE];

    SC_FORTRAN_STR_C(s, type, *snct);

    file = SC_GET_POINTER(PDBfile, *sfid);

    rv = PD_free(file, s, space);

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFOPEN - open a PDBfile
 *        - save the PDBfile pointer in an internal array
 *        - and return an integer index to the pointer if successful
 *        - return 0 otherwise
 */

FIXNUM FF_ID(pfopen, PFOPEN)(FIXNUM *sncn, char *name, char *mode)
   {FIXNUM rv;
    char s[MAXLINE], t[2];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *sncn);
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

FIXNUM FF_ID(pfclos, PFCLOS)(FIXNUM *sfid)
   {FIXNUM rv;
    PDBfile *file;

    file = SC_DEL_POINTER(PDBfile, *sfid);

    *sfid = 0;

    rv = PD_close(file);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFWULC - write an ULTRA curve into a PDB file */

FIXNUM FF_ID(pfwulc, PFWULC)(FIXNUM *sfid, FIXNUM *sncl, char *labl,
			     FIXNUM *snp, double *px, double *py,
			     FIXNUM *sic)
   {int i, n, rv;
    char s[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, labl, *sncl);

    i    = *sic;
    n    = *snp;
    file = SC_GET_POINTER(PDBfile, *sfid);

    rv = PD_wrt_curve_alt(file, s, n, px, py, &i);

    *sic = i;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFWULY - write the y values for an ULTRA curve into a PDB file */

FIXNUM FF_ID(pfwuly, PFWULY)(FIXNUM *sfid, FIXNUM *sncl, char *labl,
			     FIXNUM *snp, FIXNUM *six, double *py,
			     FIXNUM *sic)
   {int i, n, rv;
    char s[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(s, labl, *sncl);

    i    = *sic;
    n    = *snp;
    file = SC_GET_POINTER(PDBfile, *sfid);

    rv = PD_wrt_curve_y_alt(file, s, n, *six, py, &i);

    *sic = i;

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

FIXNUM FF_ID(pfwset, PFWSET)(FIXNUM *sfid, char *dname,
			     FIXNUM *adp, double *adm)
   {FIXNUM rv;
    int *ldp;
    char s[MAXLINE];
    PDBfile *file;

    rv = TRUE;

    file = SC_GET_POINTER(PDBfile, *sfid);

    SC_FORTRAN_STR_C(s, dname, adp[0]);

    ldp = _PD_get_local_set_params(adp);

    rv = PD_wrt_set(file, s, ldp, adm);

    CFREE(ldp);

    return(rv);}

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

FIXNUM FF_ID(pfwmap, PFWMAP)(FIXNUM *sfid, char *dname,
			     FIXNUM *adp, double *adm, char *rname,
			     FIXNUM *arp, double *arm, FIXNUM *sim)
   {int lsm;
    FIXNUM rv;
    int *ldp, *lrp;
    char d[MAXLINE], r[MAXLINE];
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);

    SC_FORTRAN_STR_C(d, dname, adp[0]);
    SC_FORTRAN_STR_C(r, rname, arp[0]);

    ldp = _PD_get_local_set_params(adp);
    lrp = _PD_get_local_set_params(arp);
    lsm = *sim;

    rv = PD_wrt_map(file, d, ldp, adm, r, lrp, arm, &lsm);

    *sim = lsm;
    CFREE(ldp);
    CFREE(lrp);

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

FIXNUM FF_ID(pfwrae, PFWRAE)(FIXNUM *sfid, char *dname, FIXNUM *sncd,
			     char *rname, FIXNUM *arp, double *arm,
			     FIXNUM *sinf, FIXNUM *sim)
   {int lsm;
    FIXNUM rv;
    int *lrp;
    char d[MAXLINE], r[MAXLINE];
    pcons *info;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);
    if (sinf == NULL)
       info = NULL;
    else
       info = SC_GET_POINTER(pcons, *sinf);

    SC_FORTRAN_STR_C(d, dname, *sncd);
    SC_FORTRAN_STR_C(r, rname, arp[0]);

    lrp = _PD_get_local_set_params(arp);
    lsm = *sim;

    rv = PD_wrt_map_ran(file, d, r, lrp, arm, info, &lsm);

    *sim = lsm;
    CFREE(lrp);

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

FIXNUM FF_ID(pfwran, PFWRAN)(FIXNUM *sfid, char *dname, FIXNUM *sncd,
			     char *rname, FIXNUM *arp, double *arm,
			     FIXNUM *sim)
   {int lsm;
    FIXNUM rv;
    int *lrp;
    char d[MAXLINE], r[MAXLINE];
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);

    SC_FORTRAN_STR_C(d, dname, *sncd);
    SC_FORTRAN_STR_C(r, rname, arp[0]);

    lrp = _PD_get_local_set_params(arp);
    lsm = *sim;

    rv = PD_wrt_map_ran(file, d, r, lrp, arm, NULL, &lsm);

    *sim = lsm;
    CFREE(lrp);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFWIMA - write a PG_image into a PDB file */

FIXNUM FF_ID(pfwima, PFWIMA)(FIXNUM *sfid, FIXNUM *sncn, char *name,
			     FIXNUM *skn, FIXNUM *skx,
			     FIXNUM *sln, FIXNUM *slx,
			     double *data, double *sxn, double *sxx,
			     double *syn, double *syx, FIXNUM *sim)
   {int n, kx, lx, kmin, kmax, lmin, k1, k2, l1, l2, k, l, i;
    FIXNUM rv;
    double *pd, *d, xmin, xmax, ymin, ymax, zmin, zmax, z;
    char s[MAXLINE];
    PD_image *im;
    PDBfile *file;

    rv = TRUE;

    file = SC_GET_POINTER(PDBfile, *sfid);

    if ((SC_hasharr_lookup(file->chart, "PG_image") == NULL) &&
	(!PD_def_mapping(file)))
       rv = FALSE;

    else
       {SC_FORTRAN_STR_C(s, name, *sncn);

	xmin = *sxn;
	xmax = *sxx;
	ymin = *syn;
	ymax = *syx;
	kmin = *skn;
	kmax = *skx;
	lmin = *sln;
	k1   = xmin;
	k2   = xmax;
	l1   = ymin;
	l2   = ymax;
	kx   = k2 - k1 + 1;
	lx   = l2 - l1 + 1;
	n    = kx*lx;
	d    = pd = CMAKE_N(double, n);
	zmax = -HUGE;
	zmin =  HUGE;
	for (l = l1; l <= l2; l++)
	    for (k = k1; k <= k2; k++)
	        {i     = (l - lmin)*(kmax - kmin + 1) + k - kmin;
		 z     = data[i];
		 zmax  = max(zmax, z);
		 zmin  = min(zmin, z);
		 *pd++ = z;}

	im = PD_make_image(s, SC_DOUBLE_P_S, d, kx, lx, 8, xmin, xmax,
			   ymin, ymax, zmin, zmax);

	if (!PD_put_image(file, im, *sim))
	   rv = FALSE;
	else
	   PD_rel_image(im);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFDATT - FORTRAN interface routine to define an attribute */

FIXNUM FF_ID(pfdatt, PFDATT)(FIXNUM *sfid, FIXNUM *snca, char *fattr,
			     FIXNUM *snct, char *ftype)
   {FIXNUM rv;
    char lattr[MAXLINE], ltype[MAXLINE];
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);

    SC_FORTRAN_STR_C(lattr, fattr, *snca);

    SC_FORTRAN_STR_C(ltype, ftype, *snct);

    rv = PD_def_attribute(file, lattr, ltype);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFRATT - FORTRAN interface routine to remove an attribute */

FIXNUM FF_ID(pfratt, PFRATT)(FIXNUM *sfid, FIXNUM *snca, char *fattr)
   {FIXNUM rv;
    char lattr[MAXLINE];
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);

    SC_FORTRAN_STR_C(lattr, fattr, *snca);

    rv = PD_rem_attribute(file, lattr);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFSVAT - FORTRAN interface routine to assign an attribute value to
 *        - a variable
 */

FIXNUM FF_ID(pfsvat, PFSVAT)(FIXNUM *sfid, FIXNUM *sncv, char *fvar,
			     FIXNUM *snca, char *fattr, void *vl)
   {FIXNUM rv;
    char lattr[MAXLINE], lvar[MAXLINE], **lvl;
    PDBfile *file;
    attribute *attr;

    file = SC_GET_POINTER(PDBfile, *sfid);

    SC_FORTRAN_STR_C(lvar, fvar, *sncv);

    SC_FORTRAN_STR_C(lattr, fattr, *snca);

    attr = PD_inquire_attribute(file, lattr, NULL);
    lvl  = CMAKE(char *);
    if (strcmp(attr->type, "char ***") == 0)
       *lvl = CSTRSAVE(vl);
    else
       *lvl = vl;

    rv = PD_set_attribute(file, lvar, lattr, (void *) lvl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGVAT - FORTRAN interface routine to get an attribute value of
 *        - a variable
 */

FIXNUM FF_ID(pfgvat, PFGVAT)(FIXNUM *sfid, FIXNUM *sncv, char *fvar,
			     FIXNUM *snca, char *fattr, void *vl)
   {int nc;
    FIXNUM rv;
    char lvar[MAXLINE], lattr[MAXLINE];
    void *lvl;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);

    SC_FORTRAN_STR_C(lvar, fvar, *sncv);

    SC_FORTRAN_STR_C(lattr, fattr, *snca);

    lvl = PD_get_attribute(file, lvar, lattr);
    if (lvl != NULL)
       {nc = SC_arrlen(*(char **)lvl);
        memcpy(vl, *(char **)lvl, nc);};

    rv = (lvl != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFFLSH - FORTRAN interface routine to PD_flush */

FIXNUM FF_ID(pfflsh, PFFLSH)(FIXNUM *sfid)
   {FIXNUM rv;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);

    rv = PD_flush(file);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFFAMI - FORTRAN interface routine to PD_family */

FIXNUM FF_ID(pffami, PFFAMI)(FIXNUM *sfid, FIXNUM *sf)
   {FIXNUM rv;
    PDBfile *file, *nfile;

    file = SC_DEL_POINTER(PDBfile, *sfid);

    nfile = PD_family(file, (int) *sf);
    if ((nfile != file) && (*sf != 0))
       *sfid = 0;

    rv = SC_ADD_POINTER(nfile);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFCD - FORTRAN interface routine to change currrent working directory */

FIXNUM FF_ID(pfcd, PFCD)(FIXNUM *sfid, FIXNUM *sncd, char *dirname)
   {FIXNUM rv;
    char dir[MAXLINE+1];
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);

    if (*sncd > MAXLINE)
       {PD_error("DIRECTORY NAME TOO LONG - PFCD", PD_GENERIC);
        rv = FALSE;}

    else
       {SC_FORTRAN_STR_C(dir, dirname, *sncd);

	rv = PD_cd(file, dir);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFLN - FORTRAN interface routine to create a link to a variable */

FIXNUM FF_ID(pfln, PFLN)(FIXNUM *sfid, FIXNUM *snco, char *oname,
			 FIXNUM *sncn, char *nname)
   {FIXNUM rv;
    char oldname[MAXLINE+1], newname[MAXLINE+1];
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);

    if ((*snco > MAXLINE) || (*sncn > MAXLINE))
       {PD_error("VARIABLE NAME TOO LONG - PFLN", PD_GENERIC);
        return(FALSE);};

    SC_FORTRAN_STR_C(oldname, oname, *snco);
    SC_FORTRAN_STR_C(newname, nname, *sncn);

    rv = PD_ln(file, oldname, newname);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFLST - FORTRAN interface routine to get an alphabetical list of file
 *       - variables and directories in the current working directory that
 *       - match the specified pattern and type. Previous lists are deleted.
 */

FIXNUM FF_ID(pflst, PFLST)(FIXNUM *sfid, FIXNUM *sncp, char *path,
			   FIXNUM *snct, char *type, FIXNUM *sn)
   {FIXNUM rv;
    char *ppath, *ptype;
    char lpath[MAXLINE+1], ltype[MAXLINE+1];
    PDBfile *file;
    PD_smp_state *pa;

    rv = TRUE;

    pa = _PD_get_state(-1);

    pa->n_entries = 0;
    if (pa->outlist != NULL)
       CFREE(pa->outlist);

    file = SC_GET_POINTER(PDBfile, *sfid);

    if (*sncp > MAXLINE)
       {PD_error("VARIABLE NAME TOO LONG - PFLST", PD_GENERIC);
        rv = FALSE;}

    else if (*snct > MAXLINE)
       {PD_error("VARIABLE TYPE TOO LONG - PFLST", PD_GENERIC);
        rv = FALSE;}

    else
       {if (*sncp == 0)
	   ppath = NULL;
        else
	   {ppath = lpath;
	    SC_FORTRAN_STR_C(lpath, path, *sncp);};

	if (*snct == 0)
	   ptype = NULL;
	else
	   {ptype = ltype;
	    SC_FORTRAN_STR_C(ltype, type, *snct);};

	pa->outlist = PD_ls(file, ppath, ptype, &pa->n_entries);

	if ((pa->outlist == NULL) && (pa->err[0] != '\0'))
	   rv = FALSE;
	else
	   *sn = pa->n_entries;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGLS - FORTRAN interface routine to get the name of the nth variable or
 *       - directory in the internal sorted table set up by a call to PFLST.
 */

FIXNUM FF_ID(pfgls, PFGLS)(FIXNUM *sord, FIXNUM *sncn, char *name)
   {int n;
    FIXNUM rv;
    char *s;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    n = *sord - 1;

    if ((n < 0) || (n >= pa->n_entries))
       {PD_error("VARIABLE ORDINAL OUT OF RANGE - PFGLS", PD_GENERIC);
        rv = FALSE;}
    else
       {s = pa->outlist[n];

	strcpy(name, s);
	*sncn = strlen(s);

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFDLS - FORTRAN interface routine to delete internal table set up by a call
 *       - to PFLST.
 */

FIXNUM FF_ID(pfdls, PFDLS)(void)
   {FIXNUM rv;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    rv = TRUE;

    if (pa->outlist != NULL)
       CFREE(pa->outlist);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFMKDR - FORTRAN interface routine to create a directory */

FIXNUM FF_ID(pfmkdr, PFMKDR)(FIXNUM *sfid, FIXNUM *sncd, char *dirname)
   {FIXNUM rv;
    char dir[MAXLINE+1];
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);

    if (*sncd > MAXLINE)
       {PD_error("DIRECTORY NAME TOO LONG - PFMKDR", PD_GENERIC);
        return(FALSE);};

    SC_FORTRAN_STR_C(dir, dirname, *sncd);

    rv = PD_mkdir(file, dir);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFISDR - FORTRAN interface routine to test for a directory */

FIXNUM FF_ID(pfisdr, PFISDR)(FIXNUM *sfid, FIXNUM *sncd, char *dirname)
   {FIXNUM rv;
    char dir[MAXLINE+1];
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);

    if (*sncd > MAXLINE)
       {PD_error("DIRECTORY NAME TOO LONG - PFISDR", PD_GENERIC);
        return(FALSE);};

    SC_FORTRAN_STR_C(dir, dirname, *sncd);

    rv = PD_isdir(file, dir);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFPWD - FORTRAN interface routine to return currrent working directory */

FIXNUM FF_ID(pfpwd, PFPWD)(FIXNUM *sfid, FIXNUM *sncc, char *cwd)
   {FIXNUM rv;
    char *pwd, *lcwd;
    PDBfile *file;

    rv = TRUE;

    file = SC_GET_POINTER(PDBfile, *sfid);
    lcwd = cwd;

    pwd = PD_pwd(file);
    if (pwd == NULL)
       rv = FALSE;

    else
       {strcpy(lcwd, pwd);
	*sncc = strlen(lcwd);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFNCIN - FORTRAN interface routine to convert a buffer full of data 
 *          in the format of a remote machine of a different architecture
 *          to the format of the host machine.                           
 */

FIXNUM FF_ID(pfncin, PFNCIN)(void *out, void *in,
			     FIXNUM *sni, FIXNUM *schrt,
			     FIXNUM *snct, char *type)
   {FIXNUM rv;
    char s[MAXLINE];
    hasharr *chrt;

    rv = TRUE;

    SC_FORTRAN_STR_C(s, type, *snct);

    chrt = SC_GET_POINTER(hasharr, *schrt);

    PN_conv_in(out, in, s, *sni, chrt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFNCOT - FORTRAN interface routine to convert a buffer full of
 *          data in the host machine format to the format of a machine
 *          with a different architecture.
 */

FIXNUM FF_ID(pfncot, PFNCOT)(void *out, void *in,
			     FIXNUM *sni, FIXNUM *schrt,
			     FIXNUM *snct, char *type)
   {FIXNUM rv;
    char s[MAXLINE];
    hasharr *chrt;

    rv = TRUE;

    SC_FORTRAN_STR_C(s, type, *snct);

    chrt = SC_GET_POINTER(hasharr, *schrt);

    PN_conv_out(out, in, s, *sni, chrt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFRDBT - FORTRAN interface to a routine to read a bitstream
 *
 */

FIXNUM FF_ID(pfrdbt, PFRDBT)(FIXNUM *sfid, FIXNUM *sncn, char *name,
			     FIXNUM *snct, char *type, FIXNUM *sni,
			     FIXNUM *ssgn, FIXNUM *snb, FIXNUM *sadsz,
			     FIXNUM *sfpp, FIXNUM *soffs, FIXNUM *pan,
			     void *pdata)
   {int id;
    inti ni, nb;
    intb bpi;
    FIXNUM rv;
    char s[MAXLINE], t[MAXLINE];
    char *dataout;
    PDBfile *file;

    SC_FORTRAN_STR_C(s, name, *sncn);
    SC_FORTRAN_STR_C(t, type, *snct);

    file = SC_GET_POINTER(PDBfile, *sfid);

    id  = SC_type_id(t, FALSE);
    bpi = SC_type_size_i(id);
    ni  = *sni;
    nb  = bpi*ni;

    rv = PD_read_bits(file, s, t, *sni, *ssgn, *snb, *sadsz,
		       *sfpp, *soffs, (long *) pan, &dataout);
    if (rv != 0)
       {if ((SC_is_type_char(id) == TRUE) || (SC_is_type_fix(id) == TRUE))
           memcpy(pdata, dataout, nb);
        else
           rv = FALSE;};

    if (rv)
       CFREE(dataout);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFSMJO - set the major order for the given file
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(pfsmjo, PFSMJO)(FIXNUM *sfid, FIXNUM *sv)
   {FIXNUM rv;
    PDBfile *file;

    rv   = FALSE;
    file = SC_GET_POINTER(PDBfile, *sfid);

    if (file != NULL)
       {file->major_order = (PD_major_order) *sv;
	rv                = TRUE;};

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGMJO - get the major order for the given file
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(pfgmjo, PFGMJO)(FIXNUM *sfid, FIXNUM *sv)
   {FIXNUM rv;
    PDBfile *file;

    rv   = FALSE;
    file = SC_GET_POINTER(PDBfile, *sfid);

    if (file != NULL)
       {*sv = file->major_order;
	rv  = TRUE;};

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFRSPL - reset the pointer list
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(pfrspl, PFRSPL)(FIXNUM *sfid)
   {FIXNUM rv;
    PDBfile *file;

    rv   = FALSE;
    file = SC_GET_POINTER(PDBfile, *sfid);

    if (file != NULL)
       rv = PD_reset_ptr_list(file);

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFGFLN - return the byte length of the given file
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(pfgfln, PFGFLN)(FIXNUM *sfid, FIXNUM *sln)
   {FIXNUM rv;
    PDBfile *file;

    rv   = FALSE;
    file = SC_GET_POINTER(PDBfile, *sfid);

    if (file != NULL)
       *sln = PD_get_file_length(file);

    return(rv);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFDNRM - wrapper over PD_fix_denorm
 *        - return TRUE iff successful
 *        - return FALSE if not a floating point type
 */

FIXNUM FF_ID(pfdnrm, PFDNRM)(FIXNUM *istd, FIXNUM *snct, char *type,
			     FIXNUM *sln, void *vr) 
   {int is;
    long n;
    FIXNUM rv;
    data_standard* std;
    char t[MAXLINE];

    is = *istd;
    n  = *sln;
    SC_FORTRAN_STR_C(t, type, *snct);

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

FIXNUM FF_ID(pfrent, PFRENT)(FIXNUM *sfid, FIXNUM *sncn, char *name)
   {FIXNUM rv;
    char lnam[MAXLINE];
    PDBfile *file;

    SC_FORTRAN_STR_C(lnam, name, *sncn);

    file = SC_GET_POINTER(PDBfile, *sfid);

    rv = PD_remove_entry(file, lnam);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFTRGT - target the next file to be opened
 *        - given an index from the following list:
 *        -
 *        -   (1,  5)  - GCC 4.0 and later X86_64
 *        -   (2,  5)  - GCC 4.0 and later Ix86
 *        -   (1,  5)  - Mac OSX 10.6 and later
 *        -   (3, 11)  - Mac OSX 10.5
 *        -   (2,  7)  - Cygwin i686
 *        -   (5,  9)  - IBM PPC64 XLC 64 bit
 *        -   (6, 10)  - IBM PPC64 XLC 32 bit
 *        -   (6, 12)  - SPARC
 *        -   (4,  2)  - DOS
 *        -
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(pftrgt, PFTRGT)(FIXNUM *sis, FIXNUM *sia)
   {int al, st;
    FIXNUM rv;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    al = *sia;
    st = *sis;
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

/* PFNTGT - FORTRAN interface to a routine to allocate, initialize, and
 *          return a structure chart
 */

FIXNUM FF_ID(pfntgt, PFNTGT)(FIXNUM *sis, FIXNUM *sia)
   {int al, st, ret;
    FIXNUM rv;
    hasharr *chart;

    al = *sia;
    st = *sis;
    ret = (al != 6);
    if (ret)
       {chart = PN_target(PD_std_standards[st - 1], PD_std_alignments[al - 1]);
        rv    = SC_ADD_POINTER(chart);}

    else
       {PD_error("REQUESTED ALIGNMENT NO LONGER EXISTS - PFNTGT", PD_GENERIC);
        rv = -1;};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                                DP ROUTINES                               */

/*--------------------------------------------------------------------------*/

#ifdef HAVE_MPI

#include "scope_mpi.h"

/*--------------------------------------------------------------------------*/

/* PFINMP - FORTRAN interface routine to initialize pdblib for dpi
 * 
 */

FIXNUM FF_ID(pfinmp, PFINMP)(FIXNUM *smp, FIXNUM *snt, PFTid tid)
   {FIXNUM rv;

    rv = PD_init_mpi(*smp, *snt, tid);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFMPSS - FORTRAN interface routine to set file->mpi_file */

FIXNUM FF_ID(pfmpss, PFMPSS)(FIXNUM *sfid, FIXNUM *sv)
   {FIXNUM rv;
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);
    rv   = PD_mp_set_serial(file, *sv);
  
    return(rv);}
  
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFMPOP - open a PDBfile for dpi
 *        - save the PDBfile pointer in an internal array
 *        - and return an integer index to the pointer if successful
 *        - return 0 otherwise
 */

FIXNUM FF_ID(pfmpop, PFMPOP)(FIXNUM *sncn, char *name,
			     char *mode, FIXNUM *scomm)
   {FIXNUM rv;
    char s[MAXLINE], t[2];
    PDBfile *file;
    SC_communicator comm;

    SC_FORTRAN_STR_C(s, name, *sncn);
    SC_FORTRAN_STR_C(t, mode, 1);
    comm = (SC_communicator) MPI_Comm_f2c(*scomm);

    rv   = 0;
    file = PD_mp_open(s, t, comm);
    if (file != NULL)
       {file->major_order    = COLUMN_MAJOR_ORDER;
        file->default_offset = 1;

        rv = SC_ADD_POINTER(file);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PFTMMP - FORTRAN interface routine to terminate pdblib mpi */

FIXNUM FF_ID(pftmmp, PFTMMP)(void)
   {

    PD_term_mpi();

    return(TRUE);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

