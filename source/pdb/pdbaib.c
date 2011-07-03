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

/* PD_VERIFY_WRITES - set global flag that will verify all file writes
 *                  - by reading the data back after the write
 *                  - and comparing the results
 *                  - on failure 0 is returned as the number of items written
 *                  - this a global operation, not per file, because
 *                  - you want to do this with flaky hardware of file systems
 *                  - as such all files are likely to be effected
 *                  - return the old value of the flag
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

/* PD_CONTENTS - dump the contents of the structure chart, symbol table,
 *             - and the extras of any PDBfile *file to the file OUT
 *             - if FH == 0 write the file chart and if 1
 *             - write the host chart
 *
 * #bind PD_contents fortran() scheme() python(contents)
 */

void PD_contents(PDBfile *file ARG(,,cls), FILE *out, int fh, int vers)
   {int rv;

    rv = file->wr_meta(file, out, fh);
    SC_ASSERT(rv == TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_TYPEDEF - define an alias for a type which exists in the host chart
 *            - the intended use is to provide a correspondence
 *            - between a type that has been defined to PDBLib (ONAME)
 *            - and a typedef'd type in programs (TNAME)
 *            - can be used in conjunction with PD_defix and PD_defloat
 *            - to have a primitive type known to both charts
 *            - return a pointer to the original type's defstr if
 *            - successful and NULL otherwise
 *
 * #bind PD_typedef fortran() scheme() python(typedef)
 */

defstr *PD_typedef(PDBfile *file ARG(,,cls), char *oname, char *tname)
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
           _PD_d_install(file, tname, dp, TRUE);};

/* setup for file chart */
    dp = PD_inquire_type(file, oname);
    if (dp == NULL)
       snprintf(pa->err, MAXLINE,
		"ERROR: FILE TYPE %s UNKNOWN - PD_TYPEDEF\n", oname);
    else
       {if (PD_inquire_type(file, tname) == NULL)
           _PD_d_install(file, tname, dp, FALSE);};

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEFNCV - define a primitive type that will not be format converted
 *           - do it in both charts
 *
 * #bind PD_defncv fortran() scheme() python(defcnv)
 */

defstr *PD_defncv(PDBfile *file ARG(,,cls), char *name, long bpi, int align)
   {defstr *dp;

/* install in the file chart */
    dp = _PD_defstr(file, FALSE, name, NON_CONVERT_KIND,
		    NULL, NULL, bpi, align,
		    NO_ORDER, FALSE, NULL, NULL, FALSE, FALSE);

/* install an independent copy in the host chart - garbage collection! */
    dp = _PD_defstr(file, TRUE, name, NON_CONVERT_KIND,
		    NULL, NULL, bpi, align,
		    NO_ORDER, FALSE, NULL, NULL, FALSE, FALSE);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEFCHR - define a primitive character based type
 *           - this is needed in connection with TEXT
 *           - e.g. SQL date and time types
 *
 * #bind PD_defchr fortran() scheme() python(defchr)
 */

defstr *PD_defchr(PDBfile *file ARG(,,cls), char *name,
		  long bpi, int align,
		  PD_byte_order ord, int unsgned)
   {defstr *dp;

/* file chart */
    dp = _PD_defstr(file, FALSE, name, CHAR_KIND,
		    NULL, NULL, bpi, align, ord, FALSE,
		    NULL, NULL, unsgned, FALSE);

/* host chart */
    dp = _PD_defstr(file, TRUE, name, CHAR_KIND,
		    NULL, NULL, bpi, align, ord, FALSE,
		    NULL, NULL, unsgned, FALSE);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEFIXNUM - define an new integer primitive type for the file chart
 *
 * #bind PD_defixnum fortran() scheme() python(defixnum)
 */

defstr *PD_defixnum(PDBfile *file ARG(,,cls), char *name, long bpi,
		    int align, PD_byte_order ord, int unsgned, int onescmp)
   {defstr *dp;

/* file chart */
    dp = _PD_defstr(file, FALSE, name, INT_KIND,
		    NULL, NULL, bpi, align, ord, TRUE, 
                    NULL, NULL, unsgned, onescmp);

/* host chart */
    dp = _PD_defstr(file, TRUE, name, INT_KIND,
		    NULL, NULL, bpi, align, ord, TRUE, 
                    NULL, NULL, unsgned, onescmp);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEFIX - define an new integer primitive type
 *
 * #bind PD_defix fortran() scheme() python(defix)
 */

defstr *PD_defix(PDBfile *file ARG(,,cls), char *name, long bpi,
		 int align, PD_byte_order ord)
   {defstr *dp;

    dp = PD_defixnum(file, name, bpi, align, ord, FALSE, FALSE);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEFLOAT - define an new floating point primitive type
 *
 * #bind PD_defloat fortran() scheme() python(defloat)
 */

defstr *PD_defloat(PDBfile *file ARG(,,cls), char *name, long bpi,
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

    order    = CMAKE_N(int, bpi);
    memcpy(order, ordr, sizeof(int)*bpi);

/* file chart */
    dp = _PD_defstr(file, FALSE, name, FLOAT_KIND,
		    NULL, NULL, bpi, align,
		    NO_ORDER, TRUE, order, formt, FALSE, FALSE);

/* host chart */
    dp = _PD_defstr(file, TRUE, name, FLOAT_KIND,
		    NULL, NULL, bpi, align,
		    NO_ORDER, TRUE, order, formt, FALSE, FALSE);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEFSTR - a structure definition mechanism for PDBLib
 *           -
 *           - sample syntax:
 *           -
 *           -   PD_defstr(<PDB file>, "<struct name>",
 *           -                         "<member1>", "<member2>",
 *           -                 ...     "<membern>", LAST);
 *           -
 *           - where
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
 *           - LAST is a pointer to a integer zero and is specifically
 *           - allocated by PDBLib to be used to terminate argument lists
 *           - which consist of pointers
 *           -
 *           - Returns NULL if member types are unknown
 *
 * #bind PD_defstr python(defstr)
 */

defstr *PD_defstr(PDBfile *file ARG(,,cls), char *name, ...)
   {char *nxt, *ptype;
    int doffs;
    hasharr *fchrt;
    memdes *desc, *lst, *prev;
    defstr *dp, *dp2;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    SC_VA_START(name);

    prev  = NULL;
    lst   = NULL;
    fchrt = file->chart;
    doffs = file->default_offset;
    for (nxt = SC_VA_ARG(char *); (int) *nxt != 0;
         nxt = SC_VA_ARG(char *))
        {desc  = _PD_mk_descriptor(nxt, doffs);
         ptype = desc->base_type;
         if (SC_hasharr_lookup(fchrt, ptype) == NULL)
            if ((strcmp(ptype, name) != 0) || !_PD_indirection(nxt))
               {snprintf(pa->err, MAXLINE,
			 "ERROR: %s BAD MEMBER TYPE - PD_DEFSTR\n",
			 nxt);
                return(NULL);};
         
         dp2 = PD_inquire_table_type(fchrt, ptype);
         if ((dp2 != NULL)  && !(_PD_indirection(desc->type)))
            if (dp2->n_indirects > 0)
               {snprintf(pa->err, MAXLINE,
			 "ERROR: STATIC MEMBER STRUCT %s CANNOT CONTAIN INDIRECTS\n",
			 ptype);
                return(NULL);};

         if (lst == NULL)
            lst = desc;
         else
	    {prev->next = desc;
	     SC_mark(desc, 1);};

         prev = desc;};

    SC_VA_END;

/* install the type in all charts */
    dp = _PD_defstr_inst(file, name, STRUCT_KIND, lst,
			 NO_ORDER, NULL, NULL, FALSE);

    if (dp == NULL)
       PD_error("CAN'T HANDLE PRIMITIVE TYPE - PD_DEFSTR", PD_GENERIC);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEFSTR_ALT - an alternate structure definition mechanism for PDBLib
 *               -
 *               - sample syntax:
 *               -
 *               -   PD_defstr_alt(<PDB file>, "<struct name>", n_members,
 *               -                 <member-array>);
 *               - the member array elements have the same syntax as for
 *               - PD_defstr
 *
 * #bind PD_defstr_alt fortran() scheme() python(defstr_alt)
 */

defstr *PD_defstr_alt(PDBfile *file ARG(,,cls), char *name, int nmemb,
		      char **members)
   {int i, doffs;
    char *nxt, *ptype, *type;
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
        {nxt   = members[i];
         desc  = _PD_mk_descriptor(nxt, doffs);
         type  = CSTRSAVE(nxt);
         ptype = SC_firsttok(type, " \n");
         if (SC_hasharr_lookup(fchrt, ptype) == NULL)
            if ((strcmp(ptype, name) != 0) || !_PD_indirection(nxt))
               {snprintf(pa->err, MAXLINE,
			 "ERROR: %s BAD MEMBER TYPE - PD_DEFSTR_ALT\n",
			 nxt);
                return(NULL);};

         dp2 = PD_inquire_table_type(fchrt, ptype);
         if ((dp2 != NULL)  && !(_PD_indirection(desc->type)))
            if (dp2->n_indirects > 0)
               {snprintf(pa->err, MAXLINE,
			 "ERROR: STATIC MEMBER STRUCT %s CANNOT CONTAIN INDIRECTS\n",
			 ptype);
                return(NULL);};

         CFREE(type);
         if (lst == NULL)
            lst = desc;
         else
	    {prev->next = desc;
	     SC_mark(desc, 1);};

         prev = desc;};

/* install the type in all charts */
    dp = _PD_defstr_inst(file, name, STRUCT_KIND, lst,
			 NO_ORDER, NULL, NULL, FALSE);

    if (dp == NULL)
       PD_error("CAN'T HANDLE PRIMITIVE TYPE - PD_DEFSTR_ALT", PD_GENERIC);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEFSTR_S - define a struct from a string
 *             -
 *             - sample syntax:
 *             -
 *             -   PD_defstr_s(<PDB file>, "<struct name>", n_members,
 *             -               <text>);
 *             - the text consists of semicolon delimited member specifications
 *             - as the individual members in PD_defstr
 *
 * #bind PD_defstr_s fortran() scheme() python(defstr_s)
 */

defstr *PD_defstr_s(PDBfile *file ARG(,,cls), char *name, char *members)
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

/* PD_CHANGE_PRIMITIVE - change the description of a primitive type
 *                     - the type is known by index
 *
 * #bind PD_change_primitive fortran() scheme() python(change_primitive)
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

    _PD_setup_chart(file->host_chart, hstd, NULL, halign, NULL, TRUE, TRUE);

/* NOTE: we must change the file chart unconditionally but this only
 * ends up changing the dp->convert flag for this type and this is
 * essential
 */
    _PD_setup_chart(file->chart, fstd, hstd, falign, halign, FALSE, TRUE);

/* if we opened the file in write mode we need to change the formats */
    if (file->mode == PD_CREATE)
       err &= (*file->wr_fmt)(file);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_FP_TOLER - fill array of tolerances for
 *             - floating point type comparisons
 *             - this is mainly test support
 *
 * #bind PD_fp_toler scheme() python(fp_toler)
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
