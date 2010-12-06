/*
 * SXPDBD.C - PDB diff functionality in SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

#define LINE_SIZE      MAXLINE
#define LINE_SIZ2      LINE_SIZE + 30

#define NOT_PRESENT      -10
#define CANT_SEEK        -20
#define CANT_ALLOC       -30
#define CANT_READ        -40
#define NOT_SAME_TYPE    -50
#define NOT_SAME_SIZE    -60
#define NOT_PRIMITIVE    -70
#define BAD_UNIT_SIZE    -80
#define BAD_NUMBERS      -90
#define CONTENTS_DIFFER -100

/*--------------------------------------------------------------------------*/

#define SX_SKIP_ITAG(fpa)                                                    \
    {FILE *fp;                                                               \
     fp = fpa->stream;                                                       \
     _PD_rfgets(bf, MAXLINE, fp);}

/*--------------------------------------------------------------------------*/

#define SX_READ_ITAG(fpa, bfa, na, ta, ada, fla)                             \
    _PD_rfgets(bfa, MAXLINE, fpa);                                           \
    ta    = SC_strtok(bfa, "\001", s);                                       \
    na    = (ta != NULL) ? atoi(ta) : 0;                                     \
    ta    = SC_strtok(NULL, "\001\n", s);                                    \
    token = SC_strtok(NULL, "\001\n", s);                                    \
    if (token == NULL)                                                       \
       {ada = -1;                                                            \
        fla = TRUE;}                                                         \
    else                                                                     \
       {ada   = SC_stol(token);                                              \
        token = SC_strtok(NULL, "\001\n", s);                                \
        if (token == NULL)                                                   \
           fla = TRUE;                                                       \
        else                                                                 \
           fla = atoi(token);}

/*--------------------------------------------------------------------------*/

#define SKIP_UNIT(pfa, ada, skip)                                            \
    {FILE *fp;                                                               \
     fp = pfa->stream;                                                       \
     lio_seek(fp, ada, SEEK_SET);                                            \
     _PD_skip_over(pfa, skip, FALSE);}

/*--------------------------------------------------------------------------*/

#define DIFF_FIX_ARRAY(_ret, _indx, _a, _b, _n)                              \
    {int _ok;                                                                \
     long _i;                                                                \
     if (SX_disp_individ_diff == TRUE)                                       \
        {for (_i = 0L; _i < _n; _i++)                                        \
             {PM_CLOSETO_FIX(_ok, _a[_i], _b[_i]);                           \
              if (_ok == TRUE)                                               \
                 {_ret     &= FALSE;                                         \
                  _indx[_i] = TRUE;};};}                                     \
     else                                                                    \
        {for (_i = 0L; _i < _n; _i++)                                        \
             {PM_CLOSETO_FIX(_ok, _a[_i], _b[_i]);                           \
              _ret &= _ok;};};}

/*--------------------------------------------------------------------------*/

#define DIFF_FP_ARRAY(_ret, _indx, _a, _b, _n, _tol)                         \
    {int _ok;                                                                \
     long _i;                                                                \
     if (SX_disp_individ_diff == TRUE)                                       \
        {for (_i = 0L; _i < _n; _i++)                                        \
             {PM_CLOSETO_FLOAT(_ok, _a[_i], _b[_i], _tol);                   \
              if (_ok == TRUE)                                               \
                 {_ret     &= FALSE;                                         \
                  _indx[_i] = TRUE;};};}                                     \
     else                                                                    \
        {for (_i = 0L; _i < _n; _i++)                                        \
             {PM_CLOSETO_FLOAT(_ok, _a[_i], _b[_i], _tol);                   \
              ret &= _ok;};};}

/*--------------------------------------------------------------------------*/

#define DIFF_TUPLE_ARRAY(_ret, _typ, _indx, _n, _tol)                        \
    {int _ok;                                                                \
     long _i, _ipt, _ne;                                                     \
     defstr *_dp;                                                            \
     _typ *_a, *_b;                                                          \
     _a = (_typ *) bfa;                                                      \
     _b = (_typ *) bfb;                                                      \
     _dp  = PD_inquire_type(pf, type);                                       \
     _ipt = _PD_items_per_tuple(_dp);                                        \
     _ne  = _ipt*_n;                                                         \
     if (SX_disp_individ_diff == TRUE)                                       \
        {for (_i = 0L; _i < _ne; _i++)                                       \
             {PM_CLOSETO_FLOAT(_ok, _a[_i], _b[_i], _tol);                   \
              if (_ok == TRUE)                                               \
                 {_ret     &= FALSE;                                         \
                  _indx[_i] = TRUE;};};}                                     \
     else                                                                    \
        {for (_i = 0L; _i < _ne; _i++)                                       \
             {PM_CLOSETO_FLOAT(_ok, _a[_i], _b[_i], _tol);                   \
              ret &= _ok;};};}

/*--------------------------------------------------------------------------*/

int
 SX_disp_individ_diff = FALSE, 
 SX_promote_flag      = 0, 
 SX_promote_fixed     = FALSE, 
 SX_promote_float     = FALSE;

static int
 _SX_diff_tree(char *nma, char *nmb, 
	       PDBfile *pfa, PDBfile *pfb, 
	       syment *epa, syment *epb, int flag);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_DIFF_SIGNAL - handle signals during comparisons */

static void _SX_diff_signal(int sig)
   {

    _SX.err_fpe = TRUE;

    LONGJMP(_SX.diff_err, ABORT);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_CHECK_POINTERS - check that two pointers are both commensurate
 *                    - either both are NULL or both are non-NULL
 *                    - skip over apropriate data in files to preserve
 *                    - data flow through diff process
 *                    - return TRUE iff they are commensurate
 */

static int _SX_check_pointers(PDBfile *pfa, PDBfile *pfb, 
			      char *nma, char *nmb, char *ta, 
			      long na, int64_t ada, char *tb, 
			      long nb, int64_t adb)
   {int rv;

    rv = FALSE;

    if (((ta == NULL) && (tb == NULL)) ||
        ((ta != NULL) && (tb != NULL)))
       rv = TRUE;

    else if (ta == NULL)
       {PRINT(stdout, "\nPointer, %s, in first file is NULL\n", nma);
        SKIP_UNIT(pfb, adb, nb);}
    else
       {PRINT(stdout, "\nPointer, %s, in second file is NULL\n", nmb);
        SKIP_UNIT(pfa, ada, na);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_TYPE_EQUAL - return TRUE if the two types in the two files are the same
 *                - or in the same class and that class' promote flag is set.
 */

int _SX_type_equal(PDBfile *pfa, PDBfile *pfb, char *typa, char *typb)
   {int ret, ln;
    char *token;
    defstr *dpa, *dpb;
    memdes *dea, *deb;
    char suffixa[MAXLINE], suffixb[MAXLINE], bf[MAXLINE];

/* short-circuit directory type (directories are always the same type) */
    if ((strcmp(typa, "Directory") == 0) && (strcmp(typa, typb) == 0))
       return(TRUE);
 
    strcpy(bf, typa);
    token = SC_firsttok(bf, " *");
    dpa   = PD_inquire_type(pfa, token);

    strcpy(bf, typb);
    token = SC_firsttok(bf, " *");
    dpb   = PD_inquire_type(pfb, token);

/* sanity check: we must be able to lookup both base types */
    if ((dpa == NULL) || (dpb == NULL))
       return(FALSE);

/* assume they are the same base type, until we find out otherwise */
    ret = TRUE;

/* only do typename checking if both types are not input spoke
 * placeholder typenames
 */
    ln = strlen(PD_TYPE_PLACEHOLDER);
    if ((strncmp(typa, PD_TYPE_PLACEHOLDER, ln) != 0) && 
        (strncmp(typb, PD_TYPE_PLACEHOLDER, ln) != 0))

/* this assumes typenames are of the form "T *" and not "T*" */
       {strcpy(bf, typa);
        strcpy(suffixa, SC_lasttok(bf, " "));
        strcpy(bf, typb);
        strcpy(suffixb, SC_lasttok(bf, " "));
    
/* they must have the same number of indirections */
        if ((strcmp(typa, suffixa) != 0) && (strcmp(suffixa, suffixb) != 0))
           ret &= FALSE;
     
/* they must have the same typename */
        if (strcmp(typa, typb) != 0)
           {ret &= FALSE;};};

/* they must have the same base type */

/* PRIMITIVES */
    if (dpa->members == NULL && dpb->members == NULL)

/* type promotion SHORT-CIRCUITS */
       {if (SX_promote_flag)
           {if (SX_promote_float && (dpa->fp.format != NULL) &&
		(dpb->fp.format != NULL))
               return(TRUE);

            if (SX_promote_fixed &&
		(dpa->fp.format == NULL) && (dpb->fp.format == NULL) &&
                (dpa->fix.order != NO_ORDER) && (dpb->fix.order != NO_ORDER))
               return(TRUE);};}

/* STRUCTS */
    else 

/* they must have the same members in order to be the same */
       {for (dea = dpa->members, deb = dpb->members;
             (dea != NULL) && (deb != NULL);
             dea = dea->next, deb = deb->next)

/* number of items must be the same */
            {ret &= (dea->number == deb->number);
  
/* member type must be the same
 * do not recursive if member type is the same
 * as struct type (infinite recursion)
 */
             if ((strcmp(dea->base_type, typa) != 0) &&
		 (strcmp(dea->type, typa) != 0))
                ret &= _SX_type_equal(pfa, pfb, dea->type, deb->type);
   
/* short-circuit the check if return value becomes FALSE */
             if (!ret)
                return(ret);};
    
/* they must have the same number of members */
        if (dea != deb)
           ret &= FALSE;};

/* they must have the same number of indirections */
    ret &= (dpa->n_indirects == dpb->n_indirects);
 
    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DISPLAY_DIFF - display a diff list object */

object *_SXI_display_diff(object *argl)
   {

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_PRINT_INDIVID_DIFF - print individual differences side by side */

static void _SX_print_individ_diff(PDBfile *pf, char *nma,  char *nmb, 
				   char *pva, char *pvb, char *indx, 
				   long ni, char *type, dimdes *dims, 
				   int mjr)
   {int id, nn, def_off, samen, lna, lnb;
    long i, isz, msz;
    char sa[MAXLINE], sb[MAXLINE];
    char bf[LINE_SIZE+1], bfa[LINE_SIZE], bfb[LINE_SIZ2+1];
    char tmp[LINE_SIZE];

    samen = (strcmp(nma, nmb) == 0);

    nn = strlen(nma);
    memset(bf, ' ', LINE_SIZE);
    bf[LINE_SIZE] = '\0';

    def_off = pf->default_offset;
    msz = 0L;
    for (i = 0L; i < ni; i++)
        {if (indx[i])
            {isz = strlen(PD_index_to_expr(bfa, i, dims, mjr, def_off));
             msz = max(msz, isz);};};
    nn += (msz + 3);

    bf[nn] = '\0';
    PRINT(stdout, "\n%s%s", bf, "          File 1                    File 2\n\n");
    if (samen == FALSE)
       {memset(bfb, ' ', LINE_SIZ2+1);
        bfb[LINE_SIZ2] = '\0';
        lna = strlen(nma);
        lnb = strlen(nmb);
/* NOTE: the hard-wired numbers in the strncpy calls below are based on
 *       the "File 1..." line above. If that line is changed, these
 *       numbers will need to be changed as well.
 */                                                 
        strncpy(&bfb[nn+10], nma, lna);
        strncpy(&bfb[nn+36], nmb, lnb);
        bfb[nn+36+lnb] = '\0';
        PRINT(stdout, "%s%s", bfb, "\n\n");};
    memset(bf, ' ', LINE_SIZE);

    id = SC_type_id(type, FALSE);

    if ((SC_is_type_num(id) == TRUE) || (id == SC_POINTER_I))
       {for (i = 0L; i < ni; i++)
	    {if (indx[i])
	        {if ((dims == NULL) && (ni == 1))
		    snprintf(tmp, LINE_SIZE, "%s", nma);
		 else
		    snprintf(tmp, LINE_SIZE, "%s(%s)", nma,
			     PD_index_to_expr(bfa, i, dims, mjr, def_off));
		 memcpy(bf, tmp, min(LINE_SIZE, strlen(tmp)));

		 SC_ntos(sa, MAXLINE, id, pva, i, 1);
		 SC_ntos(sb, MAXLINE, id, pvb, i, 1);
		 snprintf(tmp, LINE_SIZE, ":   %s   %s", sa, sb);

		 memcpy(&bf[nn], tmp, min(LINE_SIZE - nn, strlen(tmp) + 1));
		 PRINT(stdout, "%s\n", bf);
		 memset(bf, ' ', LINE_SIZE);};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_DIFF_INDIRECTION - read the information about an indirection, 
 *                      - allocate the space, connect the pointer, and
 *                      - read in the data and compare it
 *                      - return TRUE iff they are the same
 */

static int _SX_diff_indirection(char *nma, char *nmb, 
			        PDBfile *pfa, PDBfile *pfb)
   {int fla, flb, ret;
    long na, nb;
    int64_t ada, adb, oada, oadb;
    char bfa[MAXLINE], bfb[MAXLINE], bfc[MAXLINE];
    char *ta, *tb, *token, *s;
    FILE *fpa, *fpb;
    syment *epa, *epb;

    ret = FALSE;

    if ((pfa == NULL) || (pfb == NULL))
       return(ret);

    fpa = pfa->stream;
    fpb = pfb->stream;

    SX_READ_ITAG(fpa, bfa, na, ta, ada, fla);
    SX_READ_ITAG(fpb, bfb, nb, tb, adb, flb);

/* if the pointers differ in type - fail now */
    if (!_SX_check_pointers(pfa, pfb, nma, nmb, 
			    ta, 1L, ada, tb, 1L, adb))
       ret = FALSE;

/* if both NULL - pass now */
    else if ((ada == -1L) && (adb == -1L))
       ret = TRUE;

/* if no type call it a pass */
    else if (ta == NULL)
       ret = TRUE;

    else if ((na != nb) || (strcmp(ta, tb) != 0) ||
	     (((ada == -1L) || (adb == -1L)) && (ada != adb)))
       {if (strcmp (nma, nmb) == 0)
           PRINT(stdout, "\nPointer, %s, differs in type or number of elements\n", nma);
        else
           PRINT(stdout, "\nPointer %s differs in type or number of elements from pointer %s\n", nma, nmb);
        if (ada != -1L)
           SKIP_UNIT(pfa, ada, 1L);
        if (adb != -1L)
           SKIP_UNIT(pfb, adb, 1L);

        ret = FALSE;}

    else if (!_SX_type_equal(pfa, pfb, ta, tb))
       {PRINT(stdout, "\nType, %s, differs between the two files\n", ta);
        LONGJMP(_SX.diff_err, ABORT);}

    else if ((ada != -1L) && (na != 0L))
       {oada = lio_tell(fpa);
        if (oada == -1L)
           SS_error("CAN'T FIND ADDRESS A - _SX_DIFF_INDIRECTION", SS_null);

        oadb = lio_tell(fpb);
        if (oadb == -1L)
           SS_error("CAN'T FIND ADDRESS B - _SX_DIFF_INDIRECTION", SS_null);

        if (!fla)
           {lio_seek(fpa, ada, SEEK_SET);
            _PD_rfgets(bfc, MAXLINE, fpa);};

        if (!flb)
           {lio_seek(fpb, adb, SEEK_SET);
            _PD_rfgets(bfc, MAXLINE, fpb);};

        if ((ada = lio_tell(fpa)) == -1L)
           SS_error("CAN'T FIND NEW ADDRESS A - _SX_DIFF_INDIRECTION", 
                    SS_null);
        if ((adb = lio_tell(fpb)) == -1L)
           SS_error("CAN'T FIND NEW ADDRESS B - _SX_DIFF_INDIRECTION", 
                    SS_null);

        epa = _PD_mk_syment(ta, na, ada, NULL, NULL);
        epb = _PD_mk_syment(tb, nb, adb, NULL, NULL);

        ret = _SX_diff_tree(nma, nmb, pfa, pfb, epa, epb, TRUE);

        _PD_rl_syment_d(epa);
        _PD_rl_syment_d(epb);

        if (!fla)
           if (lio_seek(fpa, oada, SEEK_SET))
              SS_error("FAILED TO FIND OLD ADDRESS A - _SX_DIFF_INDIRECTION", 
                       SS_null);

        if (!flb)
           if (lio_seek(fpb, oadb, SEEK_SET))
              SS_error("FAILED TO FIND OLD ADDRESS B - _SX_DIFF_INDIRECTION", 
                       SS_null);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_DIFF_LEAF_INDIRECTS - compare the indirects in structs
 *                         - a cast is also an indirection
 *                         - compare data based on type in tag itag, not cast
 */

static int _SX_diff_leaf_indirects(PDBfile *pfa, PDBfile *pfb, 
				   PDBfile *pfc, char *bfa, char *bfb, 
				   long na, char *nma, char *nmb, 
				   defstr *dp)
   {int ret;
    long i, sz;
    char *sva, *svb;
    char mnma[MAXLINE], mnmb[MAXLINE];
    memdes *desc, *mem_lst;

    sz      = dp->size;
    mem_lst = dp->members;

    sva = bfa;
    svb = bfb;
    ret = TRUE;
    for (i = 0L; i < na; i++)
        {for (desc = mem_lst; desc != NULL; desc = desc->next)
             {if (desc->cast_offs >= 0L || _PD_indirection(desc->type))
                 {snprintf(mnma, MAXLINE, "%s[%ld].%s", nma, 
                           i + pfc->default_offset, desc->name);
                  snprintf(mnmb, MAXLINE, "%s[%ld].%s", nmb, 
                           i + pfc->default_offset, desc->name);
                  ret &= _SX_diff_indirection(mnma, mnmb, 
                                              pfa, pfb);};};

	 sva += sz;
	 svb += sz;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_DISPLAY_DIFF - print out the given differences */

static int _SX_display_diff(PDBfile *pf, char *nma, char *nmb, 
			    char *pva, char *pvb, char *indx, 
			    long ni, char *type, dimdes *dims)
   {int mjr, def_off;
    PD_printdes prnt;

    mjr     = pf->major_order;
    def_off = pf->default_offset;

    SC_MEM_INIT(PD_printdes, &prnt);

    prnt.prefix   = "";
    prnt.before   = "";
    prnt.after    = "";
    prnt.mjr      = mjr;
    prnt.def_off  = def_off;
    prnt.dims     = dims;
    prnt.fp       = stdout;

    if (!SX_disp_individ_diff)
       {PRINT(stdout, "\nValues from the first file:\n");

        if (pva != NULL)
	   {prnt.nodename = nma;
	     _PD_print_leaf(&prnt, pf, pva, ni, 
			    type, FALSE, 0, NULL);};

        PRINT(stdout, "\nValues from the second file:\n");

        if (pvb != NULL)
	   {prnt.nodename = nmb;
	    _PD_print_leaf(&prnt, pf, pvb, ni, 
                          type, FALSE, 0, NULL);};}

    else if ((pva != NULL) && (pvb != NULL) && (indx != NULL))
       _SX_print_individ_diff(pf, nma, nmb, pva, pvb, indx, ni, type, 
                              dims, mjr);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_DIFF_PRIMITIVES - compare two arrays of primitive data types */

static int _SX_diff_primitives(PDBfile *pf, char *nma, char *nmb, 
			       char *bfa, char *bfb, char *type, 
			       long ni, dimdes *dims)
   {int id, ret;
    char *indx;
    precisionfp *fp_pre;

    fp_pre = _SC.types.fp_precision;

    if (SX_disp_individ_diff == TRUE)
       indx = FMAKE_N(char, ni, "_SX_DIFF_PRIMITIVES:indx");
    else
       indx = NULL;

    id = SC_type_id(type, TRUE);

/* find the type and compare */
    ret = TRUE;

/* character types (ok) */
    if (id == SC_CHAR_I)
       {char *va, *vb;
	va = (char *) bfa;
        vb = (char *) bfb;
        DIFF_FIX_ARRAY(ret, indx, va, vb, ni);}

    else if (id == SC_WCHAR_I)
       {wchar_t *va, *vb;
	va = (wchar_t *) bfa;
        vb = (wchar_t *) bfb;
        DIFF_FIX_ARRAY(ret, indx, va, vb, ni);}

/* fixed point types (ok) */
    else if (id == SC_INT8_I)
       {int8_t *va, *vb;
	va = (int8_t *) bfa;
        vb = (int8_t *) bfb;
        DIFF_FIX_ARRAY(ret, indx, va, vb, ni);}

    else if (id == SC_SHORT_I)
       {short *va, *vb;
	va = (short *) bfa;
        vb = (short *) bfb;
        DIFF_FIX_ARRAY(ret, indx, va, vb, ni);}

    else if (id == SC_INT_I)
       {int *va, *vb;
	va = (int *) bfa;
        vb = (int *) bfb;
        DIFF_FIX_ARRAY(ret, indx, va, vb, ni);}

    else if (id == SC_LONG_I)
       {long *va, *vb;
	va = (long *) bfa;
        vb = (long *) bfb;
        DIFF_FIX_ARRAY(ret, indx, va, vb, ni);}

    else if (id == SC_LONG_LONG_I)
       {long long *va, *vb;
	va = (long long *) bfa;
        vb = (long long *) bfb;
        DIFF_FIX_ARRAY(ret, indx, va, vb, ni);}

/* floating point types (ok) */
    else if (id == SC_FLOAT_I)
       {float *va, *vb;
	va = (float *) bfa;
        vb = (float *) bfb;
        DIFF_FP_ARRAY(ret, indx, va, vb, ni, fp_pre[0].tolerance);}

    else if (id == SC_DOUBLE_I)
       {double *va, *vb;
	va = (double *) bfa;
        vb = (double *) bfb;
        DIFF_FP_ARRAY(ret, indx, va, vb, ni, fp_pre[1].tolerance);}

    else if (id == SC_LONG_DOUBLE_I)
       {long double *va, *vb;
	va = (long double *) bfa;
        vb = (long double *) bfb;
        DIFF_FP_ARRAY(ret, indx, va, vb, ni, fp_pre[2].tolerance);}

/* complex floating point types (ok) */
    else if (id == SC_FLOAT_COMPLEX_I)
       {DIFF_TUPLE_ARRAY(ret, float, indx, ni, fp_pre[0].tolerance);}

    else if (id == SC_DOUBLE_COMPLEX_I)
       {DIFF_TUPLE_ARRAY(ret, double, indx, ni, fp_pre[1].tolerance);}

    else if (id == SC_LONG_DOUBLE_COMPLEX_I)
       {DIFF_TUPLE_ARRAY(ret, long double, indx, ni, fp_pre[2].tolerance);}

/* quaternion floating point types (ok) */
    else if (id == SC_QUATERNION_I)
       {DIFF_TUPLE_ARRAY(ret, double, indx, ni, fp_pre[1].tolerance);};

    if (ret == FALSE)
       _SX_display_diff(pf, nma, nmb, bfa, bfb, indx, ni, type, dims);

    if (SX_disp_individ_diff == TRUE)
       SFREE(indx);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_DIFF_STRUCTS - compare the non-pointered members of
 *                  - two structures in memory
 *                  - return TRUE if they are the same and
 *                  - FALSE otherwise
 */

static int _SX_diff_structs(PDBfile *pfa, PDBfile *pfb, 
			    char *nma, char *nmb, 
                            char *bfa, char *bfb, 
			    char *typa, char *typb, long ni)
   {int ret;
    long i, mitems, sza, szb;
    memdes *desca, *descb;
    char *sva, *svb, *tva, *tvb, mnma[MAXLINE], mnmb[MAXLINE];
    defstr *dpa, *dpb;
    dimdes *mdims;

    ret = TRUE;
    dpa = PD_inquire_table_type(pfa->host_chart, typa);
    dpb = PD_inquire_table_type(pfb->host_chart, typb);
    if ((dpa == NULL) || (dpb == NULL))
       SS_error("BAD TYPE - _SX_DIFF_STRUCTS", SS_null);

    else
       {sza = dpa->size;
	szb = dpa->size;
	sva = bfa;
	svb = bfb;
	for (i = 0L; i < ni; i++)
	    {for (desca = dpa->members, descb = dpb->members;
		  desca != NULL && descb != NULL; 
		  desca = desca->next, descb = descb->next)
	         {tva = sva + desca->member_offs;
		  tvb = svb + descb->member_offs;

		  if (_PD_indirection(desca->type))
		     continue;

		  mitems = desca->number;

		  snprintf(mnma, MAXLINE, "%s[%ld].%s", 
			   nma, i + pfa->default_offset, desca->name);
		  snprintf(mnmb, MAXLINE, "%s[%ld].%s", 
			   nmb, i + pfb->default_offset, descb->name);

		  mdims = desca->dimensions;

		  if (_PD_prim_typep(desca->type, pfa->host_chart, PD_READ))
		     ret &= _SX_diff_primitives(pfa, mnma, mnmb, 
						tva, tvb, 
						desca->type, mitems, mdims);
		  else
		     ret &= _SX_diff_structs(pfa, pfb, mnma, mnmb, 
					     tva, tvb, 
					     desca->type, descb->type, 
					     mitems);};

	     sva += sza;
	     svb += szb;};};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RD_LEAF_T - read a leaf into a temporary space passed in */

static int _SX_rd_leaf_t(PDBfile *pf, syment *ep, char *vr, char *in_type, 
			 long ni, long bpi, char *out_type)
   {int convert;
    long i, n, nrd, nib;
    int64_t addr;
    char *buf, *vbuf, *svr;
    defstr *dpf;
    FILE *fp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    switch (SETJMP(pa->trace_err))
       {case ABORT :
	     return(FALSE);
        case ERR_FREE :
	     return(TRUE);
	default :
	     memset(PD_err, 0, MAXLINE);
	     break;};

    dpf = _PD_lookup_type(out_type, pf->chart);
    fp  = pf->stream;

    n = _PD_n_blocks(ep);
    if (n == 1)
       _PD_entry_set_number(ep, 0, PD_entry_number(ep));

    convert = FALSE;
    if ((dpf->convert > 0) || (strcmp(in_type, out_type) != 0))
       convert = TRUE;

    if (convert)
       {buf = (char *) SC_alloc_nzt(ni, bpi, "_SX_RD_LEAF_T:buf", NULL);
        if (buf == NULL)
           return(FALSE);}
    else
       buf = vr;       

    nrd = 0L;
    for (i = 0; i < n; i++)
        {nib = _PD_entry_get_number(ep, i);
	 if (lio_read(buf, (size_t) bpi, (size_t) nib, fp) != nib)
            break;
         nrd += nib;
         if ((i+1) < n) 
	    {addr = _PD_entry_get_address(ep, i+1);
	     if (lio_seek(fp, addr, SEEK_SET))
                break;};};

    if (nrd != ni)
       {if (convert)
          SFREE(buf);
        return(FALSE);}

    if (convert)
       {vbuf = buf;
        svr  = vr;
        PD_convert(&svr, &vbuf, in_type, out_type, ni, 
                   pf->std, pf->host_std, pf->host_std, 
                   pf->chart, pf->host_chart, 0, PD_TRACE);
        SFREE(buf);}

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_DIFF_LEAF - compare two arrays of numbers */

static int _SX_diff_leaf(char *nma, char *nmb, 
			 PDBfile *pfa, PDBfile *pfb, 
			 syment *epa, syment *epb)
   {int ret;
    long na, nb, bpi, bpa, bpb;
    char *ta, *tb, *tc, *bfa, *bfb;
    memdes *mem_lst;
    defstr *dp;
    dimdes *dims;
    hasharr *hc, *fa, *fb;
    PDBfile *pfc;

    ret = FALSE;
    na  = PD_entry_number(epa);
    ta  = PD_entry_type(epa);
    nb  = PD_entry_number(epb);
    tb  = PD_entry_type(epb);
    fa  = pfa->chart;
    fb  = pfb->chart;

    if (!_SX_type_equal(pfa, pfb, ta, tb))
       LONGJMP(_SX.diff_err, ABORT);
 
/* directories cannot be compared except via their name (SHORT-CIRCUIT) */
    if ((strcmp(ta, "Directory") == 0) &&
	(strcmp(ta, tb) == 0) &&
	(strcmp(nma, nmb) == 0))
       {return(TRUE);}

/* find the bytes per item */
    bpa = _PD_lookup_size(ta, fa);
    if (bpa == -1L)
       return(BAD_UNIT_SIZE);

    bpb = _PD_lookup_size(tb, fb);
    if (bpb == -1L)
       return(BAD_UNIT_SIZE);

    if (bpa >= bpb)
       {tc  = ta;
        hc  = pfa->host_chart;
        pfc = pfa;}
    else
       {tc  = tb;
        hc  = pfb->host_chart;
        pfc = pfb;}

    if ((bpi = _PD_lookup_size(tc, hc)) == -1L)
       return(BAD_UNIT_SIZE);

    bfa = (char *) SC_alloc_nzt(na, bpi, "_SX_DIFF_LEAF:bfa", NULL);
    bfb = (char *) SC_alloc_nzt(nb, bpi, "_SX_DIFF_LEAF:bfb", NULL);

    if ((bfa == NULL) || (bfb == NULL))
       {if ((bfa == NULL) && (bfb == NULL) &&
	    (na == 0) && (nb == 0))
	   return(TRUE);
	else
	   return(CANT_ALLOC);};

/* bring in the leaf from file a */
    if (!_SX_rd_leaf_t(pfa, epa, bfa, ta, na, bpa, ta))
       return(CANT_READ);

/* bring in the leaf from file b */
    if (!_SX_rd_leaf_t(pfb, epb, bfb, tb, nb, bpb, tb))
       return(CANT_READ);

    dims = PD_entry_dimensions(epa);

    dp = PD_inquire_table_type(hc, tc);
    if (dp == NULL)
       SS_error("VARIABLE NOT IN STRUCTURE CHART - _SX_DIFF_LEAF", 
		SS_mk_string(tc));

    else
       {mem_lst = dp->members;
	if (mem_lst == NULL)
	   ret = _SX_diff_primitives(pfc, nma, nmb, 
				     bfa, bfb, tc, na, dims);
	else
	   {ret = _SX_diff_structs(pfa, pfb, nma, nmb, 
				   bfa, bfb, ta, tb, na);

	    if (dp->n_indirects)
	       ret &= _SX_diff_leaf_indirects(pfa, pfb, pfc, 
					      bfa, bfb, na, 
					      nma, nmb, dp);};

	SFREE(bfa);
	SFREE(bfb);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_DIFF_TREE - compare two data trees
 *               - return TRUE iff the contents of the two trees are identical
 *               - (indirections need not point to the same addresses
 *               - but their contents must compare)
 */

static int _SX_diff_tree(char *nma, char *nmb, 
			 PDBfile *pfa, PDBfile *pfb, 
			 syment *epa, syment *epb, int flag)
   {long i, ni;
    char lnma[MAXLINE], lnmb[MAXLINE];
    int ret;

    if (!_PD_indirection(PD_entry_type(epa)))
       ret = _SX_diff_leaf(nma, nmb, pfa, pfb, epa, epb);

    else
       {ni  = PD_entry_number(epa);
        ret = TRUE;
        for (i = 0L; i < ni; i++)
	    {if (flag)
	        {snprintf(lnma, MAXLINE, "%s[%ld]", 
			  nma, i + pfa->default_offset);
		 snprintf(lnmb, MAXLINE, "%s[%ld]", 
			  nmb, i + pfb->default_offset);}
	     else
		{snprintf(lnma, MAXLINE, "%s", nma);
		 snprintf(lnmb, MAXLINE, "%s", nmb);};

	     ret &= _SX_diff_indirection(lnma, lnmb, pfa, pfb);};};

/* convert FALSE to CONTENTS_DIFFER to be absolutely clear */
    if (ret == FALSE)
       ret = CONTENTS_DIFFER;

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_DIFF_VAR - compare a variable from each of two files
 *              - return values
 *              -   NOT_PRESENT      variable does not exist
 *              -   CANT_SEEK        cannot seek to variable in file
 *              -   CANT_ALLOC       cannot allocate space for variable
 *              -   CANT_READ        cannot read variable from file
 *              -   NOT_SAME_TYPE    types differ
 *              -   NOT_SAME_SIZE    sizes differ
 *              -   NOT_PRIMITIVE    
 *              -   BAD_UNIT_SIZE
 *              -   BAD_NUMBERS      bad floating point values
 *              -   CONTENTS_DIFFER  contents are not the same
 *              -   TRUE             variables are the same in all respects
 */

static int _SX_diff_var(PDBfile *pfa, PDBfile *pfb, 
		        char *nma, char *nmb, 
			long *nia, long *nib)
   {int ret;
    FILE *fpa, *fpb;
    syment *epa, *epb;
    char *s, *typa, *typb;
    char fullpatha[MAXLINE], fullpathb[MAXLINE], *fullpath;

    switch (SETJMP(_SX.diff_err))
       {case ABORT :
	     return(FALSE);
        case ERR_FREE :
	     return(TRUE);
        default :
	     memset(PD_err, 0, MAXLINE);
	     break;};

    SC_signal(SIGFPE, _SX_diff_signal);

    strcpy(fullpatha, _PD_fixname(pfa, nma));
    s = _PD_expand_hyper_name(pfa, fullpatha);
    if (s == NULL)
       return(NOT_PRESENT);
    strcpy(fullpatha, s);
    SFREE(s);

    strcpy(fullpathb, _PD_fixname(pfb, nmb));
    s = _PD_expand_hyper_name(pfb, fullpathb);
    if (s == NULL)
       return(NOT_PRESENT);
    strcpy(fullpathb, s);
    SFREE(s);

    epa = _PD_effective_ep(pfa, fullpatha, FALSE, NULL);
    epb = _PD_effective_ep(pfb, fullpathb, FALSE, NULL);

/* the following added to allow compatibility with older PDB files */
    if ((epa == NULL) && (fullpatha[0] == '/'))
       epa = _PD_effective_ep(pfa, fullpatha + 1, FALSE, NULL);

    if ((epb == NULL) && (fullpathb[0] == '/'))
       epb = _PD_effective_ep(pfb, fullpathb + 1, FALSE, NULL);
    
    if ((epa == NULL) || (epb == NULL))
       {_PD_rl_syment_d(epa);
        _PD_rl_syment_d(epb);
        return(NOT_PRESENT);};

    if (strlen(fullpatha) < strlen(fullpathb))
       fullpath = fullpatha;
    else
       fullpath = fullpathb;

    fpa = pfa->stream;
    fpb = pfb->stream;

/* position the file pointers */
    if (lio_seek(fpa, PD_entry_address(epa), SEEK_SET) ||
        lio_seek(fpb, PD_entry_address(epb), SEEK_SET))
       {_PD_rl_syment_d(epa);
        _PD_rl_syment_d(epb);
        return(CANT_SEEK);};

    typa = PD_entry_type(epa);
    typb = PD_entry_type(epb);

    if (_SX_type_equal(pfa, pfb, typa, typb) == 0)
       {_PD_rl_syment_d(epa);
        _PD_rl_syment_d(epb);
        return(NOT_SAME_TYPE);};

    *nia = PD_entry_number(epa);
    *nib = PD_entry_number(epb);

    if (*nia != *nib)
       {_PD_rl_syment_d(epa);
        _PD_rl_syment_d(epb);
        return(NOT_SAME_SIZE);};

    _PD_digits_tol(pfa, pfb);
    _SC_set_format_defaults();
    _SC_set_user_formats();

    ret = _SX_diff_tree(fullpatha, fullpathb, pfa, pfb, epa, epb, FALSE);
    _PD_rl_syment_d(epa);
    _PD_rl_syment_d(epb);

/* check for FPEs which mean BAD_NUMBERS */
    if (_SX.err_fpe == TRUE)
       {ret     = BAD_NUMBERS;
	_SX.err_fpe = FALSE;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DIFF_VAR - two variables from different files */

object *_SXI_diff_var(object *argl)
   {int ret, samen;
    long sza, szb;
    object *obj;
    PDBfile *pfa, *pfb;
    char *nma, *nmb, *bf;

    nma = NULL;
    nmb = NULL;

    if (SS_consp(argl))
       {SX_GET_OBJECT_FROM_LIST(SX_ipdbfilep(obj), pfa, 
                                FILE_STREAM(PDBfile, obj), argl, 
                                "FIRST ARGUMENT NOT PDBFILE - _SXI_DIFF_VAR");}
    else
       SS_error("BAD FIRST ARGUMENT - _SXI_DIFF_VAR", SS_null);

    if (SS_consp(argl))
       {SX_GET_OBJECT_FROM_LIST(SX_ipdbfilep(obj), pfb, 
                                FILE_STREAM(PDBfile, obj), argl, 
                                "SECOND ARGUMENT NOT PDBFILE - _SXI_DIFF_VAR");}
    else
       SS_error("BAD SECOND ARGUMENT - _SXI_DIFF_VAR", SS_null);

    if (SS_consp(argl))
       {SX_GET_STRING_FROM_LIST(nma, argl, 
                                "THIRD ARGUMENT NOT NAME - _SXI_DIFF_VAR");}
    else
       SS_error("BAD THIRD ARGUMENT - _SXI_DIFF_VAR", SS_null);

    if (SS_consp(argl))
       {SX_GET_STRING_FROM_LIST(nmb, argl, 
                                "FOURTH ARGUMENT NOT NAME - _SXI_DIFF_VAR");}
    else
       SS_error("BAD FOURTH ARGUMENT - _SXI_DIFF_VAR", SS_null);

    if (SS_consp(argl))
       {SX_GET_INTEGER_FROM_LIST(_SC.types.max_digits, argl, 
                                 "FIFTH ARGUMENT NOT INTEGER - _SXI_DIFF_VAR");};

    if (SS_consp(argl))
       {SX_GET_INTEGER_FROM_LIST(SX_disp_individ_diff, argl, 
                                 "SIXTH ARGUMENT NOT INTEGER - _SXI_DIFF_VAR");};

    if (SS_consp(argl))
       {SX_GET_INTEGER_FROM_LIST(SX_promote_flag, argl, 
                                 "SEVENTH ARGUMENT NOT INTEGER - _SXI_DIFF_VAR");};
    
    if ((SX_promote_flag == 1) || (SX_promote_flag == 3))
       SX_promote_fixed = TRUE;
    else
       SX_promote_fixed = FALSE;

    if ((SX_promote_flag == 2) || (SX_promote_flag == 3))
       SX_promote_float = TRUE;
    else
       SX_promote_float = FALSE;

    ret = _SX_diff_var(pfa, pfb, nma, nmb, &sza, &szb);

    samen = ((nma != NULL) && (nmb != NULL) && (strcmp(nma, nmb) == 0));

    switch (ret)
       {case NOT_PRESENT :
	     if (samen == TRUE)
	        bf = SC_dsnprintf(FALSE, 
				  "Variable %s not present in both files", 
				  nma);
	     else
	        bf = SC_dsnprintf(FALSE, 
				  "Variable not present: %s in File 1 or %s in File 2", 
				  nma, nmb);
	     obj = SS_mk_string(bf);
	     break;

        case CANT_SEEK :
	     if (samen == TRUE)
	        bf = SC_dsnprintf(FALSE, 
				  "Can't seek variable %s in both files", 
				  nma);
	     else
	        bf = SC_dsnprintf(FALSE, 
				  "Can't seek variable: %s in File 1 or %s in File 2", 
				  nma, nmb);
	     obj = SS_mk_string(bf);
	     break;

        case CANT_ALLOC :
	     if (samen == TRUE)
	        bf = SC_dsnprintf(FALSE, 
				  "Can't allocate variable %s from both files", 
				  nma);
	     else
	        bf = SC_dsnprintf(FALSE, 
				  "Can't allocate variable: %s from File 1 or %s from File 2", 
				  nma, nmb);
	     obj = SS_mk_string(bf);
	     break;

        case CANT_READ :
	     if (samen == TRUE)
	        bf = SC_dsnprintf(FALSE, 
				  "Can't read variable %s in both files", 
				  nma);
	     else
	        bf = SC_dsnprintf(FALSE, 
				  "Can't read variable: %s in File 1 or %s in File 2", 
				  nma, nmb);
	     obj = SS_mk_string(bf);
	     break;

        case NOT_SAME_TYPE :
	     if (samen == TRUE)
	        bf = SC_dsnprintf(FALSE, 
				  "Variable %s not same type in both files", 
				  nma);
	     else
	        bf = SC_dsnprintf(FALSE, 
				  "Variable %s in File 1 not same type as %s in File 2", 
				  nma, nmb);
	     obj = SS_mk_string(bf);
	     break;

        case NOT_SAME_SIZE :
	     if (samen == TRUE)
	        bf = SC_dsnprintf(FALSE, 
				  "Variable %s not same size: %ld in File 1 and %ld in File 2", 
				  nma, sza, szb);
	     else
	        bf = SC_dsnprintf(FALSE, 
				  "Variables not same size: %s in File 1 is %ld and %s in File 2 is %ld", 
				  nma, sza, nmb, szb);
	     obj = SS_mk_string(bf);
	     break;

        case NOT_PRIMITIVE :
	     if (samen == TRUE)
	        bf = SC_dsnprintf(FALSE, 
				  "Variable %s not primitive type", 
				  nma);
	     else
	        bf = SC_dsnprintf(FALSE, 
				  "Variable not primitive type: %s in File 1 or %s in File 2", 
				  nma, nmb);
	     obj = SS_mk_string(bf);
	     break;

        case BAD_UNIT_SIZE :
	     if (samen == TRUE)
	        bf = SC_dsnprintf(FALSE, 
				  "Bad unit size for variable %s", 
				  nma);
	     else
	        bf = SC_dsnprintf(FALSE, 
				  "Bad unit size for variable %s in File 1 or %s in File 2", 
				  nma, nmb);
	     obj = SS_mk_string(bf);
	     break;

        case BAD_NUMBERS :
	     if (samen == TRUE)
	        bf = SC_dsnprintf(FALSE, 
				  "Variable %s contains bad numbers", 
				  nma);
	     else
	        bf = SC_dsnprintf(FALSE, 
				  "Variable %s in File 1 or %s in File 2 contains bad numbers", 
				  nma, nmb);
	     obj = SS_mk_string(bf);
	     break;

        case CONTENTS_DIFFER :
	     obj = SS_f;
	     break;

        case TRUE :
	     obj = SS_t;
	     break;

        default :
	     bf  = SC_dsnprintf(FALSE, "Internal error");
	     obj = SS_mk_string(bf);
	     break;};

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
