/*
 * PDBAIB.C - API routines for PDB
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_VERIFY_WRITES - Set the global flag that will verify all file writes
 *                  - by reading the data back after the write
 *                  - and comparing the results.
 *                  - On failure 0 is returned as the number of items written.
 *                  - This a global operation, not per file, because
 *                  - you want to do this with flaky hardware or file systems
 *                  - as such all files are likely to be effected.
 *                  - Return the old value of the flag.
 *
 * #bind PD_verify_writes fortran() scheme() python()
 */

int PD_verify_writes(int st)
   {int ost;

    ost = SC_gs.verify_writes;

    SC_gs.verify_writes = st;

    return(ost);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_CONTENTS - Dump the contents of the structure chart, symbol table,
 *             - and the extras of the PDBfile FILE to the file OUT.
 *             - If FH == 0 write the file chart and if 1
 *             - write the host chart. VERS is currently ignored.
 *
 * #bind PD_contents fortran() scheme() python()
 */

void PD_contents(PDBfile *file ARG(,,cls), FILE *out, int fh, int vers)
   {int rv;

    rv = file->wr_meta(file, out, fh);
    SC_ASSERT(rv == TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_TYPEDEF - Define an alias for a type which exists in the host chart.
 *            - The intended use is to provide a correspondence
 *            - between a type that has been defined to PDBLib (ONAME)
 *            - and a typedef'd type in programs (TNAME).
 *            - This can be used in conjunction with PD_defix and PD_defloat
 *            - to have a primitive type known to both charts.
 *            - Return a pointer to the original type's defstr if
 *            - successful and NULL otherwise.
 *
 * #bind PD_typedef fortran() scheme() python()
 */

defstr *PD_typedef(PDBfile *file ARG(,,cls),
		   const char *oname, const char *tname)
   {defstr *dp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

/* setup for host chart */
    dp = PD_inquire_host_type(file, oname);
    if (dp == NULL)
       snprintf(pa->err, MAXLINE,
		"ERROR: HOST TYPE %s UNKNOWN - PD_TYPEDEF\n", oname);
    else
       {if (PD_inquire_host_type(file, tname) == NULL)
           _PD_d_install(file, tname, dp, PD_CHART_HOST, oname);};

/* setup for file chart */
    dp = PD_inquire_type(file, oname);
    if (dp == NULL)
       snprintf(pa->err, MAXLINE,
		"ERROR: FILE TYPE %s UNKNOWN - PD_TYPEDEF\n", oname);
    else
       {if (PD_inquire_type(file, tname) == NULL)
           _PD_d_install(file, tname, dp, PD_CHART_FILE, oname);};

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEFENUM - Define an enum NAME as a typedef of the
 *            - appropriate size fixed point number.
 *            - Return the new defstr if successful otherwise return NULL.
 *
 * #bind PD_defenum fortran() scheme() python()
 */

defstr *PD_defenum(PDBfile *file ARG(,,cls), const char *name)
   {char *ity;
    defstr *dp;
    enum {A, B, C} t = A;

    if (sizeof(t) == sizeof(int))
       ity = G_INT_S;
    else if (sizeof(t) == sizeof(long))
       ity = G_LONG_S;
    else if (sizeof(t) == sizeof(long long))
       ity = G_LONG_LONG_S;
    else if (sizeof(t) == sizeof(short))
       ity = G_SHORT_S;

    dp = PD_typedef(file, ity, name);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEFNCV - Define a primitive type NAME to PDBfile NAME
 *           - that will not be format converted when reading and writing.
 *           - The new type has BPI bytes per item and alignment ALIGN.
 *           - Do this in both the host and file structure charts.
 *           - Return the new defstr if successful otherwise return NULL.
 *
 * #bind PD_defncv fortran() scheme() python()
 */

defstr *PD_defncv(PDBfile *file ARG(,,cls), const char *name,
		  long bpi, int align)
   {defstr *dp;

/* install in the file chart */
    dp = _PD_defstr(file, PD_CHART_FILE, name, KIND_OTHER,
		    NULL, NULL, bpi, align,
		    NO_ORDER, FALSE, NULL, NULL, FALSE, FALSE);

/* install an independent copy in the host chart - garbage collection! */
    dp = _PD_defstr(file, PD_CHART_HOST, name, KIND_OTHER,
		    NULL, NULL, bpi, align,
		    NO_ORDER, FALSE, NULL, NULL, FALSE, FALSE);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEFCHR - Define a primitive character based type NAME to
 *           - PDBfile FILE.
 *           - The new type has BPI bytes per item, alignment ALIGN,
 *           - byte order ORD and unsigned property UNSGNED.
 *           - This is needed in connection with TEXT
 *           - e.g. SQL date and time types.
 *           - Return the new defstr if successful otherwise return NULL.
 *
 * #bind PD_defchr fortran() scheme() python()
 */

defstr *PD_defchr(PDBfile *file ARG(,,cls), const char *name,
		  long bpi, int align,
		  PD_byte_order ord, int unsgned)
   {defstr *dp;

/* file chart */
    dp = _PD_defstr(file, PD_CHART_FILE, name, KIND_CHAR,
		    NULL, NULL, bpi, align, ord, FALSE,
		    NULL, NULL, unsgned, FALSE);

/* host chart */
    dp = _PD_defstr(file, PD_CHART_HOST, name, KIND_CHAR,
		    NULL, NULL, bpi, align, ord, FALSE,
		    NULL, NULL, unsgned, FALSE);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEFIXNUM - Define a new integer primitive type NAME
 *             - for the PDBfile FILE.
 *             - The new type has BPI bytes per item, alignment ALIGN,
 *             - byte order ORD, unsigned property UNSGNED, and
 *             - ones complement ONESCMP.
 *             - Return the new defstr if successful otherwise return NULL.
 *
 * #bind PD_defixnum fortran() scheme() python()
 */

defstr *PD_defixnum(PDBfile *file ARG(,,cls), const char *name, long bpi,
		    int align, PD_byte_order ord, int unsgned, int onescmp)
   {defstr *dp;

/* file chart */
    dp = _PD_defstr(file, PD_CHART_FILE, name, KIND_INT,
		    NULL, NULL, bpi, align, ord, TRUE, 
                    NULL, NULL, unsgned, onescmp);

/* host chart */
    dp = _PD_defstr(file, PD_CHART_HOST, name, KIND_INT,
		    NULL, NULL, bpi, align, ord, TRUE, 
                    NULL, NULL, unsgned, onescmp);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEFIX - Define a new integer primitive type NAME
 *          - for the PDBfile FILE.
 *          - The new type has BPI bytes per item, alignment ALIGN,
 *          - byte order ORD.
 *          - Return the new defstr if successful otherwise return NULL.
 *
 * #bind PD_defix fortran() scheme() python()
 */

defstr *PD_defix(PDBfile *file ARG(,,cls), const char *name, long bpi,
		 int align, PD_byte_order ord)
   {defstr *dp;

    dp = PD_defixnum(file, name, bpi, align, ord, FALSE, FALSE);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEFLOAT - Define an new floating point primitive type NAME
 *            - for PDBfile FILE.
 *            - The new type has BPI bytes per item, alignment ALIGN,
 *            - byte order ORDR.
 *            - EXPB and MANTB are the bit length of the exponent
 *            - and mantissa respectively.
 *            - SBS, SBE, and SBM are the 0 based start bits of the
 *            - sign, exponent, and mantissa respectively.
 *            - HMB is the high order mantissa bit and BIAS is
 *            - the exponent bias.
 *            - Return the new defstr if successful otherwise return NULL.
 *
 * #bind PD_defloat fortran() scheme() python()
 */

defstr *PD_defloat(PDBfile *file ARG(,,cls), const char *name, long bpi,
		   int align, int *ordr,
		   long expb, long mantb, long sbs, long sbe,
		   long sbm, long hmb, long bias)
   {defstr *dp;
    long *formt;
    int *order;

    formt    = CMAKE_N(long, 8);
    formt[0] = bpi*8;
    formt[1] = expb;
    formt[2] = mantb;
    formt[3] = sbs;
    formt[4] = sbe;
    formt[5] = sbm;
    formt[6] = hmb;
    formt[7] = bias;

    order = CMAKE_N(int, bpi);
    memcpy(order, ordr, sizeof(int)*bpi);

/* file chart */
    dp = _PD_defstr(file, PD_CHART_FILE, name, KIND_FLOAT,
		    NULL, NULL, bpi, align,
		    NO_ORDER, TRUE, order, formt, FALSE, FALSE);

/* host chart */
    dp = _PD_defstr(file, PD_CHART_HOST, name, KIND_FLOAT,
		    NULL, NULL, bpi, align,
		    NO_ORDER, TRUE, order, formt, FALSE, FALSE);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEFSTR_ALT - Define an new derived type NAME for PDBfile FILE.
 *               - The struct has NMEMB members whose C-style
 *               - specification is given in the array of strings MEMBERS.
 *               - The member array elements have the same syntax as for
 *               - PD_defstr.
 *               - Return the new defstr if successful otherwise return NULL.
 *
 * #bind PD_defstr_alt fortran() scheme() python()
 */

defstr *PD_defstr_alt(PDBfile *file ARG(,,cls), const char *name,
		      int nmemb, char **members)
   {int i, doffs;
    char *mbr, *ptype;
    hasharr *fchrt;
    memdes *desc, *lst, *prev;
    defstr *dp, *dp2;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    prev  = NULL;
    lst   = NULL;
    fchrt = file->chart;
    doffs = file->default_offset;
    for (i = 0; i < nmemb; i++)
        {mbr   = members[i];
         desc  = _PD_mk_descriptor(mbr, doffs);
         ptype = desc->base_type;
	 if (SC_hasharr_lookup(fchrt, ptype) == NULL)
	    {if ((strcmp(ptype, name) != 0) || !_PD_indirection(mbr))
	        {snprintf(pa->err, MAXLINE,
			  "ERROR: %s BAD MEMBER TYPE - PD_DEFSTR_ALT\n",
			  mbr);
		 return(NULL);};};

	 dp2 = PD_inquire_table_type(fchrt, ptype);
	 if ((dp2 != NULL)  && !(_PD_indirection(desc->type)))
	    {if (dp2->n_indirects > 0)
	        {snprintf(pa->err, MAXLINE,
			  "ERROR: STATIC MEMBER STRUCT %s CANNOT CONTAIN INDIRECTS\n",
			  ptype);
		 return(NULL);};};

	 if (lst == NULL)
	    lst = desc;
	 else
	    {prev->next = desc;
	     SC_mark(desc, 1);};

	 prev = desc;};

/* install the type in all charts */
    dp = _PD_defstr_inst(file, name, KIND_STRUCT, lst,
			 NO_ORDER, NULL, NULL, PD_CHART_HOST);

    if (dp == NULL)
       PD_error("CAN'T HANDLE PRIMITIVE TYPE - PD_DEFSTR_ALT", PD_GENERIC);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEFSTR - Define an new derived type NAME for PDBfile FILE.
 *           - The remaining arguments are strings each of which
 *           - specifies a member of the struct.
 *           - The list is terminated by the special token LAST.
 *           - Unknown member types result in failure to define a new type.
 *           - Return the new defstr if successful otherwise return NULL.
 *           - The syntax of a member is:
 *           - 
 *           -   <member> := <primitive type> <member name>[(<dimensions>)] |
 *           -               <derived type> <member name>[(<dimensions>)]
 *           -
 *           -   <dimensions> := <non-negative int> |
 *           -                   <non-negative int>,<dimensions> |
 *           -                   <non-negative int>, <dimensions> |
 *           -                   <non-negative int> <dimensions>
 *           -
 *           -   <primitive type> := short | integer | long | float |
 *           -                       double | char | short * | integer *
 *           -                       long * | float * | double * | char *
 *           - 
 *           -   <derived type> := any defstr'd type | any defstr'd type *
 *           -
 *           - Returns NULL if member types are unknown.
 *           -
 *           - NOTE: PD_defstr returns the defstr and PD_defstr_i
 *           - return a status of the operation.
 *
 * #bind PD_defstr python()
 */

defstr *PD_defstr(PDBfile *file ARG(,,cls), const char *name, ...)
   {int n;
    char **members, *mbr;
    defstr *dp;

    SC_VA_START(name);

    members = NULL;
    for (n = 0; TRUE; n++)
        {mbr = SC_VA_ARG(char *);
	 if (*mbr != 0)
	    members = PS_lst_push(members, mbr);
	 else
	    break;};

    dp = PD_defstr_alt(file, name, n, members);

    SC_free_strings(members);

    SC_VA_END;

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEFSTR_I - Define an new derived type NAME for PDBfile FILE.
 *             - The remaining arguments are strings each of which
 *             - specifies a member of the struct.
 *             - The list is terminated by the special token LAST.
 *             - Return the TRUE if successful otherwise return FALSE.
 *             - NOTE: PD_defstr returns the defstr and PD_defstr_i
 *             - return a status of the operation
 */

int PD_defstr_i(PDBfile *file ARG(,,cls), const char *name, ...)
   {int rv, n;
    char **members, *mbr;
    defstr *dp;

    SC_VA_START(name);

    members = NULL;
    for (n = 0; TRUE; n++)
        {mbr = SC_VA_ARG(char *);
	 if (*mbr != 0)
	    members = PS_lst_push(members, mbr);
	 else
	    break;};

    SC_VA_END;

    dp = PD_defstr_alt(file, name, n, members);

    SC_free_strings(members);

    rv = (dp != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEFSTR_S - Define an new derived type NAME for PDBfile FILE.
 *             - MEMBERS is a single string containing ';' delimited
 *             - member specifications.
 *             - The member syntax is as specified in PD_defstr.
 *             - Return the new defstr if successful otherwise return NULL.
 *
 * #bind PD_defstr_s fortran() scheme() python()
 */

defstr *PD_defstr_s(PDBfile *file ARG(,,cls),
		    const char *name, const char *members)
   {int nm;
    char **mbr;
    defstr *dp;

    mbr = SC_tokenize(members, ";");
    SC_ptr_arr_len(nm, mbr);

    dp = PD_defstr_alt(file, name, nm, mbr);

    SC_free_strings(mbr);

    if (dp == NULL)
       PD_error("CAN'T DEFINE STRUCT - PD_DEFSTR_S", PD_GENERIC);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_CHANGE_PRIMITIVE - Change the description of a primitive type ITYP
 *                     - in PDBfile FILE.  ITYP is the SCORE type
 *                     - manager index for the primitive type to be changed.
 *                     - NB is the new number of bytes, ALGN the new
 *                     - alignmnet, FPFMT the new floating format
 *                     - information, and FPORD, the new floating point
 *                     - byte order.
 *                     - Return TRUE if successful otherwise return FALSE.
 *
 * #bind PD_change_primitive fortran() scheme() python()
 */

int PD_change_primitive(PDBfile *file ARG(,,cls),
			int ityp, int nb, int algn,
			long *fpfmt, int *fpord)
   {int err, ifx, ifp;
    data_standard *fstd, *hstd;
    data_alignment *falign, *halign;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    err = TRUE;
    if (file == NULL)
       return(FALSE);

    fstd   = file->std;
    falign = file->align;

    hstd   = file->host_std;
    halign = file->host_align;

    ifx = -1;
    ifp = -1;

    if (SC_is_type_fix(ityp) == TRUE)
       ifx = SC_TYPE_FIX(ityp);

    else if (SC_is_type_fp(ityp) == TRUE)
       ifp = SC_TYPE_FP(ityp);

    else if (SC_is_type_cx(ityp) == TRUE)
       ifp = SC_TYPE_CPX(ityp);

    else
       {snprintf(pa->err, MAXLINE,
		 "UNKNOWN TYPE %d - PD_CHANGE_PRIMITIVE", ityp);
	err = 1;};

    if (ifx != -1)
       {hstd->fx[ifx].bpi = nb;
	halign->fx[ifx]   = algn;
	if (file->mode == PD_CREATE)
	   {fstd->fx[ifx].bpi = nb;
	    falign->fx[ifx]   = algn;};};

    if (ifp != -1)
       {hstd->fp[ifp].order  = fpord;
	hstd->fp[ifp].format = fpfmt;
	halign->fp[ifp]      = algn;
	if (file->mode == PD_CREATE)
	   {fstd->fp[ifp].order  = fpord;
	    fstd->fp[ifp].format = fpfmt;
	    falign->fp[ifp]      = algn;};};

    _PD_setup_chart(file->host_chart, hstd, NULL, halign, NULL,
		    PD_CHART_HOST, TRUE);

/* NOTE: we must change the file chart unconditionally but this only
 * ends up changing the dp->convert flag for this type and this is
 * essential
 */
    _PD_setup_chart(file->chart, fstd, hstd, falign, halign,
		    PD_CHART_FILE, TRUE);

/* if we opened the file in write mode we need to change the formats */
    if (file->mode == PD_CREATE)
       err &= (*file->wr_fmt)(file);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_FP_TOLER - Fill array of tolerances FPTOL from the values in
 *             - PDBfile FILE.  These can be used for
 *             - floating point type comparisons.
 *
 * #bind PD_fp_toler scheme() python()
 */

void PD_fp_toler(PDBfile *file ARG(,,cls), long double *fptol)
   {int i, fpmn[N_PRIMITIVE_FP];
    data_standard *fstd, *hstd;

    fstd = file->std;
    hstd = file->host_std;

    for (i = 0; i < N_PRIMITIVE_FP; i++)
        {fpmn[i]  = min(fstd->fp[i].format[2], hstd->fp[i].format[2]);
	 fptol[i] = POWL(2.0, -((long double) fpmn[i]));};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
