/*
 * FAPANA.C - FORTRAN interface routines for PANACEA
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "fpact.h"

typedef struct s_ff_th_record ff_th_record;

struct s_ff_th_record
   {SC_array *labels;
    SC_array *members;
    char *type;
    char *entry_name;};

/*--------------------------------------------------------------------------*/

/*                          DATABASE ACCESSORS                              */

/*--------------------------------------------------------------------------*/

/* PAMCON - give FORTRAN routines access to the database
 *        - via pointers
 *        -   NAME is the package name
 *        -   AVN is the array of variable names
 *        -   AP is the array of pointers to connect
 *        -   SN is the number of variables to connect
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(pamcon, PAMCON)(FIXNUM *anc, char **avn, void **ap, FIXNUM *sn)
   {int i, n;
    char t[MAXLINE];
    void **ptr;
    PA_package *pck;

    pck = PA_current_package();
    SC_ASSERT(pck != NULL);

    n = *sn;
    for (i = 0; i < n; i++)
        {SC_strncpy(t, MAXLINE, avn[i], anc[i]);

         ptr = ap + i;
         PA_get_access(ptr, t, -1L, 0L, TRUE);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PANTRN - intern to a single variable
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(pantrn, PANTRN)(void *av, FIXNUM *sncn, char *vname)
   {char t[MAXLINE];

    SC_FORTRAN_STR_C(t, vname, *sncn);

    PA_intern(av, t);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PACONN - connect to a single variable
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(paconn, PACONN)(void **av, FIXNUM *sncn, char *vname,
			     FIXNUM *sf)
   {char t[MAXLINE];

    SC_FORTRAN_STR_C(t, vname, *sncn);

    PA_get_access(av, t, -1L, 0L, *sf);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PADCON - disconnect a single variable
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(padcon, PADCON)(void **av, FIXNUM *sncn, char *vname)
   {char t[MAXLINE], s[MAXLINE];

    SC_FORTRAN_STR_C(t, vname, *sncn);

    PA_rel_access(av, s, -1L, 0L);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PALOCA - allocate some space and hand it back
 *        - this is a FORTRAN version of MAKE_N
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(paloca, PALOCA)(void **av, FIXNUM *sbpi, FIXNUM *sn, ...)
   {long i, n, size;
    FIXNUM *dim;

    SC_VA_START(sn);

    n    = *sn;
    size = 1L;
    for (i = 0L; i < n; i++)
        {dim   = SC_VA_ARG(FIXNUM *);
         size *= *dim;};

    SC_VA_END;

    *av = CMAKE_N(char, size*(*sbpi));

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PALLOC - complete the definition and allocate the named variable
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(palloc, PALLOC)(void **av, FIXNUM *sncn,
			     char *vname, FIXNUM *sf, ...)
   {char s[MAXLINE];
    int *pdima;
    PA_variable *pp;
    dimdes *dims, *pd;
    PA_dimens *vdims, *pvd;

    SC_FORTRAN_STR_C(s, vname, *sncn);

    pp = PA_inquire_variable(s);
    PA_ERR(pp == NULL,
           "VARIABLE %s NOT IN DATABASE - PALLOC", vname);

    SC_VA_START(sf);

/* GOTCHA: there is real trouble if a sizeof(FIXNUM) != sizeof(int)
 *         FORTRAN naturally uses FIXNUM as ints
 *         C dimension variable are ints
 *
 *         Document this as a requirement and remove
 */

    vdims = PA_VARIABLE_DIMS(pp);
    dims  = PA_VARIABLE_DESC(pp)->dimensions;
    pd    = dims;
    pvd   = vdims;
    while (pvd != NULL)
       {pdima = (int *) SC_VA_ARG(FIXNUM *);
        if (pdima == PA_gs.dul)
           {pvd->index_min = (int *) SC_VA_ARG(FIXNUM *);
            pvd->index_max = (int *) SC_VA_ARG(FIXNUM *);
            pd->index_min  = *pvd->index_min;
            pd->index_max  = *pvd->index_max;
            pd->number     = pd->index_max - pd->index_min + 1L;}

        else if (pdima == PA_gs.don)
           {pvd->index_min = (int *) SC_VA_ARG(FIXNUM *);
            pvd->index_max = (int *) SC_VA_ARG(FIXNUM *);
            pd->index_min  = *pvd->index_min;
            pd->index_max  = *pvd->index_min + *pvd->index_max - 1L;
            pd->number     = *pvd->index_max;}

        else
           {switch (pvd->method)
               {case PA_UPPER_LOWER :
                     pvd->index_max = pdima;
                     pd->index_max  = *pdima;
                     pd->number     = pd->index_max - pd->index_min + 1L;
                     break;
                case PA_OFFSET_NUMBER :
                     pvd->index_max = pdima;
                     pd->number     = *pdima;
                     pd->index_max  = *pdima + pd->index_min - 1L;};};

        pvd = pvd->next;
        pd  = pd->next;};

    SC_VA_END;

    PA_get_access(av, s, -1L, 0L, *sf);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PASPCK - set the current package to be the named one
 *        - return TRUE iff successful
 */

FIXNUM FF_ID(paspck, PASPCK)(FIXNUM *sncn, char *pname)
   {char t[MAXLINE];

    SC_FORTRAN_STR_C(t, pname, *sncn);

    PA_control_set(t);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAERR - handle errors */

void FF_ID(paerr, PAERR)(FIXNUM *sncn, char *pname)
   {char t[MAXLINE];

    SC_FORTRAN_STR_C(t, pname, *sncn);

    PA_error(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAGDOF - get the file default offset */

void FF_ID(pagdof, PAGDOF)(FIXNUM *sd)
   {

    *sd = PA_get_default_offset();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PASDOF - set the file default offset */

void FF_ID(pasdof, PASDOF)(FIXNUM *sd)
   {

    PA_set_default_offset(*sd);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PASEQV - set an equivalenced value */

void FF_ID(paseqv, PASEQV)(FIXNUM *sncn, char *name, void *avl)
   {char t[MAXLINE];

    SC_FORTRAN_STR_C(t, name, *sncn);

    PA_set_equiv(t, avl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PADEQV - define an equivalence */

void FF_ID(padeqv, PADEQV)(void *avr, FIXNUM *sncn, char *name)
   {char t[MAXLINE];

    SC_FORTRAN_STR_C(t, name, *sncn);

    PA_equivalence(avr, t);

    return;}

/*--------------------------------------------------------------------------*/

/*                          TIME HISTORY ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* PATHOP - open a time history file */

FIXNUM FF_ID(pathop, PATHOP)(FIXNUM *sncn, char *fname, char *fmode,
			     FIXNUM *ssz, FIXNUM *sncp, char *fprev)
   {int np;
    FIXNUM rv;
    char s[MAXLINE], t[2], u[MAXLINE], *lmode;
    PDBfile *file;

    SC_FORTRAN_STR_C(s, fname, *sncn);

    lmode = fmode;

    t[0] = *lmode;
    t[1] = '\0';

    np = *sncp;
    if (np == 0)
       file = PA_th_open(s, t, (long) *ssz, NULL);
    else
       {SC_FORTRAN_STR_C(u, fprev, np);
        file = PA_th_open(s, t, (long) *ssz, u);};

    if (file == NULL)
       rv = -1;
    else
       {file->major_order    = COLUMN_MAJOR_ORDER;
        file->default_offset = 1;

        rv = SC_ADD_POINTER(file);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATHFM - check the file and return the next member of the family 
 *        - if appropriate
 */

FIXNUM FF_ID(pathfm, PATHFM)(FIXNUM *sfid)
   {FIXNUM ret;
    PDBfile *file, *newfile;

    file = SC_GET_POINTER(PDBfile, *sfid);
    ret = *sfid;    

    newfile = PA_th_family(file);

    if (newfile != file)
       {ret = SC_ADD_POINTER(newfile);
        file = SC_DEL_POINTER(PDBfile, *sfid);}
        
    if (newfile == NULL)
       ret = -1;
    else
       {newfile->major_order    = COLUMN_MAJOR_ORDER;
        newfile->default_offset = 1;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PABREC - begin a time domain struct definition */

FIXNUM FF_ID(pabrec, PABREC)(FIXNUM *sfid, FIXNUM *sncn, char *fname,
			     FIXNUM *snct, char *ftype,
			     FIXNUM *sncm, char *ftime)
   {FIXNUM rv;
    char ltype[MAXLINE], lname[MAXLINE], ltime[MAXLINE];
    ff_th_record *fth;

    SC_FORTRAN_STR_C(ltype, ftype, *snct);

    SC_FORTRAN_STR_C(lname, fname, *sncn);

    fth = CMAKE(ff_th_record);
    
    fth->labels     = SC_STRING_ARRAY();
    fth->members    = SC_STRING_ARRAY();
    fth->type       = CSTRSAVE(ltype);
    fth->entry_name = CSTRSAVE(lname);

    SC_FORTRAN_STR_C(ltime, ftime, *sncm);

    SC_array_string_add_copy(fth->members, ltime);

    rv = SC_ADD_POINTER(fth);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAGRID - return the record id for the named record */

FIXNUM FF_ID(pagrid, PAGRID)(FIXNUM *sfid, FIXNUM *sind,
			     FIXNUM *sncn, char *name,
			     FIXNUM *snct, char *type, FIXNUM *srid)
   {int n;
    FIXNUM rv;
    char dname[MAXLINE], *s;
    ff_th_record *fth;
    th_record thd;
    PDBfile *file;

    rv = FALSE;

    file = SC_GET_POINTER(PDBfile, *sfid);
    
    snprintf(dname, MAXLINE, "th%d", (int) *sind);
    if (PD_read(file, dname, &thd) != 0)
       {fth = CMAKE(ff_th_record);
    
	n = thd.n_members;

	fth->members    = SC_strings_array(n, thd.members);
        fth->labels     = SC_strings_array(n-1, thd.labels);
	fth->type       = thd.type;
	fth->entry_name = thd.entry_name;

	s = thd.entry_name;
	n = strlen(s);
	SC_strncpy(name, *sncn, s, n);
	*sncn = n;

	s = thd.type;
	n = strlen(s);
	SC_strncpy(type, *snct, s, n);
	*snct  = n;

	*srid = (FIXNUM) SC_ADD_POINTER(fth);

        rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAAREC - add a member to a time domain struct definition */

FIXNUM FF_ID(paarec, PAAREC)(FIXNUM *sfid, FIXNUM *srid,
			     FIXNUM *sncm, char *fmemb,
			     FIXNUM *sncl, char *flabl)
   {int nc;
    FIXNUM rv;
    char lmemb[MAXLINE], llabl[MAXLINE], *s;
    ff_th_record *fth;

    fth  = SC_GET_POINTER(ff_th_record, *srid);

    SC_FORTRAN_STR_C(lmemb, fmemb, *sncm);

    nc = *sncl;
    SC_FORTRAN_STR_C(llabl, flabl, nc);

    SC_array_string_add_copy(fth->members, lmemb);

    if (nc > 0)
       s = CSTRSAVE(llabl);
    else
       s = NULL;

    SC_array_string_add(fth->labels, s);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAEREC - finish a time domain struct definition */

FIXNUM FF_ID(paerec, PAEREC)(FIXNUM *sfid, FIXNUM *srid)
   {int nm;
    FIXNUM rv;
    char **sm, **sl;
    PDBfile *file;
    ff_th_record *fth;
    defstr *dp;
    SC_array *fm, *fl;

    file = SC_GET_POINTER(PDBfile, *sfid);
    fth  = SC_GET_POINTER(ff_th_record, *srid);
    fm   = fth->members;
    fl   = fth->labels;

    nm = SC_array_get_n(fm);
    sm = SC_array_array(fm);

    sl = SC_array_array(fl);

    dp = PA_th_def_rec(file, fth->entry_name, fth->type, nm, sm, sl);

    rv = (dp != NULL);

    CFREE(sm);
    CFREE(sl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAWREC - write a time domain record */

FIXNUM FF_ID(pawrec, PAWREC)(FIXNUM *sfid, FIXNUM *srid,
			     FIXNUM *sinst, FIXNUM *snr, void *avr)
   {FIXNUM ret;
    PDBfile *file;
    ff_th_record *fth;

    file = SC_GET_POINTER(PDBfile, *sfid);
    fth  = SC_GET_POINTER(ff_th_record, *srid);

    ret = PA_th_write(file, fth->entry_name, fth->type,
                      (int) *sinst, (int) *snr, avr);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAWMEM - write a time domain record member */

FIXNUM FF_ID(pawmem, PAWMEM)(FIXNUM *sfid, FIXNUM *srid,
			     FIXNUM *sncm, char *mbr,
			     FIXNUM *sinst, FIXNUM *snr, void *avr)
   {FIXNUM ret;
    char memb[MAXLINE];
    PDBfile *file;
    ff_th_record *fth;

    file = SC_GET_POINTER(PDBfile, *sfid);
    fth  = SC_GET_POINTER(ff_th_record, *srid);

    SC_FORTRAN_STR_C(memb, mbr, *sncm);

    ret = PA_th_wr_member(file, fth->entry_name, memb, fth->type,
			  (int) *sinst, avr);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAWRIA - write a time domain record instance attribute */

FIXNUM FF_ID(pawria, PAWRIA)(FIXNUM *sfid, FIXNUM *sncv, char *fvar,
			     FIXNUM *sinst, FIXNUM *snca, char *fattr,
			     void *avl)
   {FIXNUM ret;
    char lvar[MAXLINE], lattr[MAXLINE];
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *sfid);

    SC_FORTRAN_STR_C(lvar, fvar, *sncv);
    SC_FORTRAN_STR_C(lattr, fattr, *snca);

    ret = PA_th_wr_iattr(file, lvar, (int) *sinst, lattr, avl);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATRNF - transpose a time history file family
 *        - return 1 if successful and 0 otherwise.
 */

FIXNUM FF_ID(patrnf, PATRNF)(FIXNUM *sncn, char *fname, FIXNUM *sord,
			     FIXNUM *sncpf)
   {FIXNUM ret;
    char s[MAXLINE];

    SC_FORTRAN_STR_C(s, fname, *sncn);

    ret = PA_th_trans_family(s, (int) *sord, (int) *sncpf);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATRNN - transpose a time history file family by explicit names
 *        - return 1 if successful and 0 otherwise.
 */

FIXNUM FF_ID(patrnn, PATRNN)(FIXNUM *sncc, char *chrs,
			     FIXNUM *sord, FIXNUM *sncpf)
   {int n;
    FIXNUM ret;
    char **names, *pc;

    pc = CMAKE_N(char, *sncc + 2);
    SC_FORTRAN_STR_C(pc, chrs, *sncc);

    names = SC_tokenize(pc, " \t\f\n\r");
    for (n = 0; names[n] != NULL; n++);

    ret = PA_th_trans_name(n, names, (int) *sord, (int) *sncpf);

    SC_free_strings(names);

    CFREE(pc);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATRNL - transpose a time history file family by links
 *        - Return 1 if successful and 0 otherwise.
 */

FIXNUM FF_ID(patrnl, PATRNL)(FIXNUM *sncc, char *chrs,
			     FIXNUM *sord, FIXNUM *sncpf)
   {int n;
    FIXNUM ret;
    char **names, *pc;

    pc = CMAKE_N(char, *sncc + 2);
    SC_FORTRAN_STR_C(pc, chrs, *sncc);

    names = SC_tokenize(pc, " \t\f\n\r");
    for (n = 0; names[n] != NULL; n++);

    ret = PA_th_trans_link(n, names, (int) *sord, (int) *sncpf);
    
    SC_free_strings(names);

    CFREE(pc);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAMRGF - FORTRAN interface routine to merge a family of ULTRA files
 *        - Return 1 if successful and 0 otherwise.
 *
 *        - SNCB    number of characters in BASE
 *        - BASE    base name of target (merged) file family
 *        - SNCF    number of characters in FAMILY
 *        - FAMILY  base name of source file family
 *        - SNCPF   number of curves per target file
 */

FIXNUM FF_ID(pamrgf, PAMRGF)(FIXNUM *sncb, char *base,
			     FIXNUM *sncf, char *family, FIXNUM *sncpf)
   {FIXNUM ret;
    char s[MAXLINE];
    char t[MAXLINE];

    SC_FORTRAN_STR_C(s, base, *sncb);
    SC_FORTRAN_STR_C(t, family, *sncf);

    ret = PA_merge_family(s, t, (int) *sncpf);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAMRGN - FORTRAN interface routine to merge a list of ULTRA files.
 *        - return 1 if successful and 0 otherwise.
 *
 *        - SNCB    number of characters in BASE
 *        - BASE    base name of target (merged) file family
 *        - SNCC    number of characters in CHRS
 *        - CHRS    list of file names
 *        - SNCPF   number of curves per target file
 */

FIXNUM FF_ID(pamrgn, PAMRGN)(FIXNUM *sncb, char *base,
			     FIXNUM *sncc, char *chrs, FIXNUM *sncpf)
   {int n;
    FIXNUM ret;
    char **files, *pc, s[MAXLINE];

    pc = CMAKE_N(char, *sncc + 2);
    SC_FORTRAN_STR_C(pc, chrs, *sncc);
    SC_FORTRAN_STR_C(s, base, *sncb);

    files = SC_tokenize(pc, " \t\f\n\r");
    for (n = 0; files[n] != NULL; n++);

    ret = PA_merge_files(s, n, files, (int) *sncpf);

    SC_free_strings(files);

    CFREE(pc);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAFREC - Free a time domain record */

FIXNUM FF_ID(pafrec, PAFREC)(FIXNUM *srid)
   {FIXNUM rv;
    ff_th_record *fth;

    fth = SC_GET_POINTER(ff_th_record, *srid);

    SC_free_array(fth->labels, SC_array_free_n);
    SC_free_array(fth->members, SC_array_free_n);

    CFREE(fth->type);
    CFREE(fth->entry_name);

    CFREE(fth);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATHTR - transpose a time history file family
 *        -
 *        - USE PATRNF INSTEAD
 */

FIXNUM FF_ID(pathtr, PATHTR)(FIXNUM *sncn, char *fname, FIXNUM *sncpf)
   {FIXNUM ret;
    char s[MAXLINE];

    SC_FORTRAN_STR_C(s, fname, *sncn);

    ret = PA_th_trans_family(s, 1, (int) *sncpf);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATHTN - transpose a time history file family by explicit names
 *        -
 *        - USE PATRNN INSTEAD
 */

FIXNUM FF_ID(pathtn, PATHTN)(char *chrs, FIXNUM *sord, FIXNUM *sncpf)
   {int n;
    FIXNUM ret;
    char *pc, **names;

    pc    = chrs;
    names = SC_tokenize(pc, " \t\f\n\r");
    for (n = 0; names[n] != NULL; n++);

    ret = PA_th_trans_name(n, names, (int) *sord, (int) *sncpf);

    SC_free_strings(names);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATHTL - transpose a time history file family by links
 *        -
 *        - USE PATRNL INSTEAD
 */

FIXNUM FF_ID(pathtl, PATHTL)(char *chrs, FIXNUM *sord, FIXNUM *sncpf)
   {int n;
    FIXNUM ret;
    char **names, *pc;

    pc    = chrs;
    names = SC_tokenize(pc, " \t\f\n\r");
    for (n = 0; names[n] != NULL; n++);

    ret = PA_th_trans_link(n, names, (int) *sord, (int) *sncpf);
    
    SC_free_strings(names);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
