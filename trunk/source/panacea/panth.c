/*
 * PANTH.C - time history data collection for PANACEA
 *         - aims:
 *         -      (1) Portability of interface and data
 *         -      (2) C and FORTRAN API
 *         -      (3) Easy to use but flexible
 *         -      (4) Must be able to manager file familes
 *         -      (5) Must be able to restart from any "time"
 *         -          in a run (controlled by instance index
 *         -          maintained by the application)
 *         -      (6) Work with supplied transposer to ULTRA
 *
 * Source Version: 7.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "panacea_int.h"

typedef struct s_th_info th_info;
typedef struct s_f77_th_record f77_th_record;

struct s_th_info
   {PDBfile *file;
    int rec_count;};

struct s_f77_th_record
   {SC_array *labels;
    SC_array *members;
    char *type;
    char *entry_name;};

int
 _PA_ul_print_flag = FALSE;

char
 PA_err[MAXLINE];

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_TH_OPEN - start a time history file family */

PDBfile *PA_th_open(char *name, char *mode, long size, char *prev)
   {PDBfile *file;

    file = PD_open(name, mode);
    if (file != NULL)
       {if (size == 0L)
           PD_set_max_file_size(file, LONG_MAX);
        else
           PD_set_max_file_size(file, size);

        if (prev != NULL)
           file->previous_file = SC_strsavef(prev,
					     "char*:PA_TH_OPEN:file");};

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_RL_TH - Free the members of a th_record */

static void _PA_rl_th(th_record *thd, int nthd)
   {int i, j, n;
    char **str;

    for (i = 0; i < nthd; i++)
        {if (thd[i].n_members != 0)
	    {str = thd[i].members;
	     n   = SC_MEM_GET_N(char *, str);
	     for (j = 0; j < n; j++)
	         SFREE(str[j]);
	     SFREE(thd[i].members);

	     str = thd[i].labels;
	     n   = SC_MEM_GET_N(char *, str);
	     for (j = 0; j < n; j++)
	         SFREE(str[j]);
	     SFREE(thd[i].labels);

	     SFREE(thd[i].type);
	     SFREE(thd[i].entry_name);
	     
	     thd[i].n_members = 0;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_TH_FAMILY - advance to the next member of the
 *              - time history file family
 */

PDBfile *PA_th_family(PDBfile *file)
   {int i, count;
    char bf[MAXLINE];
    th_record thd;
    syment *ep;
    PDBfile *nf;

    nf = PD_family(file, FALSE);

    if (nf != file)
       {for (i = 0; TRUE; i++)
            {snprintf(bf, MAXLINE, "th%d", i);
             ep = PD_inquire_entry(file, bf, FALSE, NULL);
             if (ep == NULL)
                break;

	     PD_reset_ptr_list(file);
	     PD_reset_ptr_list(nf);

             if (!PD_read(file, bf, &thd))
                break;

             PD_write(nf, bf, "th_record", &thd);

	     _PA_rl_th(&thd, 1);};

        count = 0;
        PD_read(file, "n_types", &count);
        PD_write(nf, "n_types", SC_INT_S, &count);

        PD_close(file);};

    return(nf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_TH_DEF_REC - define a struct that matches a particular data stripe
 *               - since different groups of plots can have different
 *               - plotting frequencies, define one struct for each group
 *               - and this will match the data stripe for that group
 *               - A variable of type th_record will be written out for
 *               - each time domain defined.  It contains the type, member
 *               - names, labels, and name of the time domain struct data.
 *               - The library names these "thddd" starting from 0 and
 *               - strictly in order of the calls to this function
 *               -
 *               - Arguments:
 *               -   NAME    - name of the data entry
 *               -   TYPE    - name of the struct defining the time domain
 *               -   NMEMB   - number of members of the time domain struct
 *               -   MEMBERS - the names of the members of the time
 *               -             domain struct
 *               -   LABELS  - the labels for the curves associated with
 *               -           - the time domain (one less than NMEMB in
 *               -           - length) - this is optional and if not
 *               -           - provided labels will be constructed from
 *               -           - the member names
 */

defstr *PA_th_def_rec(PDBfile *file, char *name, char *type,
		      int nmemb, char **members, char **labels)
   {int i, n, count;
    char bf[MAXLINE], **lbls, **mbrs, *ltyp;
    defstr *dp;
    th_record ht;

#ifdef SUNMOS

    count = 0;
    if (PD_inquire_entry(file, "n_types", TRUE, NULL) != NULL)
       PD_read(file, "n_types", &count);

#else

    if (!PD_read(file, "n_types", &count))
       count = 0;

#endif

    n    = nmemb - 1;
    lbls = FMAKE_N(char *, n, "PA_TH_DEF_REC:lbls");

    if (labels != NULL)
       {for (i = 0; i < n; i++)
            {if (labels[i] == NULL)
                snprintf(bf, MAXLINE, "%s vs %s", members[i+1], members[0]);
             else
                snprintf(bf, MAXLINE, "%s", labels[i]);
             lbls[i] = SC_strsavef(bf, "char*:PA_TH_DEF_REC:lbls[i]");};}

    else
       {for (i = 0; i < n; i++)
            {snprintf(bf, MAXLINE, "%s vs %s", members[i+1], members[0]);
             lbls[i] = SC_strsavef(bf, "char*:PA_TH_DEF_REC:lbls[i]");};};

/* process the members to make them legal for PDB */
    mbrs = FMAKE_N(char *, nmemb, "PA_TH_DEF_REC:mbrs");
    for (i = 0; i < nmemb; i++)
        {ltyp = _PD_member_base_type(members[i]);
         dp = PD_inquire_type(file, ltyp);
         SFREE(ltyp);

         if (dp == NULL)
            {snprintf(bf, MAXLINE, "double %s", members[i]);
             mbrs[i] = SC_strsavef(bf, "char*:PA_TH_DEF_REC:mbrs[i]");}
         else
            mbrs[i] = SC_strsavef(members[i], "char*:PA_TH_DEF_REC:mbrs[i]");};

/* define the th_record type if necessary */
    dp = PD_inquire_type(file, "th_record");
    if (dp == NULL)
       {PD_defstr(file, "th_record",
                  "int n_members",
                  "char **members",
                  "char **labels",
                  "char *type",
                  "char *entry_name",
                  LAST);};

/* write the th_record for this type */
    ht.n_members  = nmemb;
    ht.members    = mbrs;
    ht.labels     = lbls;
    ht.type       = SC_strsavef(type, "char*:PA_TH_DEF_REC:type");
    ht.entry_name = SC_strsavef(name, "char*:PA_TH_DEF_REC:name");

    snprintf(bf, MAXLINE, "th%d", count++);
    PD_write(file, bf, "th_record", &ht);

    SFREE(ht.type);
    SFREE(ht.entry_name);

/* define the type */
    dp = PD_defstr_alt(file, type, nmemb, mbrs);

/* update the counter */
    PD_write(file, "n_types", SC_INT_S, &count);

/* reset the pointer lists */
    PD_reset_ptr_list(file);

/* free the labels */
    for (i = 0; i < n; i++)
        SFREE(lbls[i]);
    SFREE(lbls);

/* free the members */
    for (i = 0; i < nmemb; i++)
        SFREE(mbrs[i]);
    SFREE(mbrs);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_TRUNCATE_ENTRY - truncate the named entry to the specified size
 *                    - in the slowest varying dimension
 *                    - return the new, truncated syment
 */

static syment *_PA_truncate_entry(PDBfile *file, char *name, long indx)
   {long numb, pags, enmb, dlen;
    syment *ep;
    dimdes *odims, *od;

    ep = PD_inquire_entry(file, name, FALSE, NULL);
    if (ep == NULL)
       return(NULL);

    odims = PD_entry_dimensions(ep);

/* check the dimensions for consistency */
    od = NULL;
    if (file->major_order == COLUMN_MAJOR_ORDER)
       for (od = odims; od->next != NULL; od = od->next);

    else if (file->major_order == ROW_MAJOR_ORDER)
       od = odims;

    if (od == NULL)
       return(NULL);

    if (indx < od->index_min)
       PD_error("INSTANCE INDEX LESS THAN MINIMUM - _PA_TRUNCATE_ENTRY",
		PD_WRITE);

    if (indx <= od->index_max)
       {numb = PD_entry_number(ep);
        dlen = od->index_max - od->index_min + 1L;
        pags = numb/dlen;
        enmb = (indx - od->index_min)*pags;

/* fix the dimensions (the instance index is 1 based reguardless of the
 * PDB API being used
 */
        od->index_max = indx - 1;
        od->number    = od->index_max - od->index_min + 1L;

/* if we're going back to the beginning, delete the entry */
        if (enmb == 0)
           {char path[MAXLINE];
	    haelem *hp;
	    hasharr *symt;

	    symt = file->symtab;

	    hp = PD_inquire_symbol(file, name, TRUE, path, symt);
	    _PD_rl_syment(ep);

/* purify complains of free memory read
 * if hp->def not nulled before SC_hasharr_remove
 */
	    hp->def = NULL;
            SC_hasharr_remove(symt, path);

            return(NULL);}

/* fix the blocks */
        else
	   _PD_block_truncate(ep, enmb);};

    return(ep);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_TH_WRITE - write time history data
 *             -
 *             - Arguments:
 *             -   NAME - time domain data entry name (data appended after
 *             -          first call
 *             -   TYPE - time domain struct name
 *             -   INST - instance index (starting index in a linear array
 *             -          of records for this block of records)
 *             -   NR   - number of records in this block
 *             -   VR   - the data which must be NR*sizeof(<TYPE>) long
 */

int PA_th_write(PDBfile *strm, char *name, char *type, int inst, int nr,
		void *vr)
   {long nd, ind[3];
    int ret;
    syment *ep;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    switch (SETJMP(pa->write_err))
       {case ABORT :
	     return(FALSE);
        case ERR_FREE :
	     return(TRUE);
        default :
	     memset(PD_err, 0, MAXLINE);
	     break;};

    ep = _PA_truncate_entry(strm, name, inst);
    if (ep == NULL)
       {nd = 1;
        ind[0] = inst;
        ind[1] = inst + nr - 1;
        ind[2] = 1L;

/* for time history data name and type are the same */
        ret = PD_write_alt(strm, name, type, vr, nd, ind);}

    else
       {nd = 1;
        ind[0] = inst;
        ind[1] = inst + nr - 1;
        ind[2] = 1L;
        ret = PD_append_alt(strm, name, vr, nd, ind);};

    PD_reset_ptr_list(strm);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_TH_WR_MEMBER - write time history data
 *                 -
 *                 - Arguments:
 *                 -   NAME   - time domain data entry name (data appended
 *                 -            after first call
 *                 -   MEMBER - time domain data entry name (data appended
 *                 -            after first call
 *                 -   TYPE - time domain struct name
 *                 -   INST - instance index (starting index in a linear
 *                 -          array of records for this block of records)
 *                 -   VR   - the data which must be NR*sizeof(<TYPE>) long
 */

int PA_th_wr_member(PDBfile *strm, char *name, char *member, char *type,
		    int inst, void *vr)
   {int ret;
    char lname[MAXLINE], *mtype, *ltype;
    syment *ep;
    defstr *dp;
    memdes *desc;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    ret = FALSE;

    switch (SETJMP(pa->write_err))
       {case ABORT :
	     return(FALSE);
        case ERR_FREE :
	     return(TRUE);
        default :
	     memset(PD_err, 0, MAXLINE);
	     break;};

    snprintf(lname, MAXLINE, "%s[%d:%d].%s", name, inst, inst, member);

    ltype = NULL;
    mtype = _PD_member_base_type(type);
    dp    = PD_inquire_type(strm, mtype);
    if (dp == NULL)
       PD_error("CAN'T FIND TYPE - PA_TH_WR_MEMBER", PD_WRITE);
    SFREE(mtype);

    for (desc = dp->members; desc != NULL; desc = desc->next)
        {if ((strcmp(desc->name, member) == 0) ||
	    (strcmp(desc->member, member) == 0))
	   {ltype = desc->type;
	    break;};};

    if (ltype == NULL)
       PD_error("CAN'T FIND MEMBER TYPE - PA_TH_WR_MEMBER", PD_WRITE);

    ep = PD_inquire_entry(strm, name, FALSE, NULL);
    if (ep != NULL)
       {dimdes *od, *odims;

        odims = PD_entry_dimensions(ep);

/* check the dimensions for consistency */
	od = NULL;
	if (strm->major_order == COLUMN_MAJOR_ORDER)
	   for (od = odims; od->next != NULL; od = od->next);

	else if (strm->major_order == ROW_MAJOR_ORDER)
	   od = odims;

	if (od == NULL)
	   return(FALSE);

	if ((od->index_min <= inst) && (inst <= od->index_max))
           ret = PD_write(strm, lname, ltype, vr);

        else if (od->index_max < inst)
	   ret = PD_append(strm, lname, vr);

        else
	   PD_error("ILLEGAL INDEX - PA_TH_WR_MEMBER", PD_WRITE);}

    else
       ret = PD_write(strm, lname, ltype, vr);

    PD_reset_ptr_list(strm);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_TH_WR_IATTR - write instance attribute values
 *                -
 *                - Arguments:
 *                -   VR   - name of the data entry
 *                -   INST - instance index being tagged
 *                -   ATTR - name of the attribute (must be defined)
 *                -   AVL  - pointer to the attribute value data
 */

int PA_th_wr_iattr(PDBfile *strm, char *vr, int inst, char *attr, void *avl)
   {int rv;
    char t[MAXLINE];

    snprintf(t, MAXLINE, "%s(%d)", vr, inst);

    rv = PD_set_attribute(strm, t, attr, avl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_TH_TRANSPOSE - process a family of time history files into ULTRA files
 *
 *                 - NAME: base name of both the TH and ULTRA file families
 *                 - NCPF: maximum number of curves per ULTRA file
 */

/* THIS FUNCTION IS DEPRECATED - USE PA_TH_TRANS_FAMILY INSTEAD */

int PA_th_transpose(char *name, int ncpf)
   {int i, nthf, ret;
    char **thfiles;

    nthf = PA_th_family_list(name, 't', &thfiles);

    ret  = PA_th_trans_files(name, ncpf, nthf, thfiles, 1, FALSE);

    for (i = 0; i < nthf; i++)
        SFREE(thfiles[i]);
    SFREE(thfiles);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_TH_TRANS_FAMILY - Process family of time history files into ULTRA files.
 *
 *                    - NAME: base name of both the TH and ULTRA file families
 *                    - ORD:  1 for normal order and -1 for reverse order
 *                    - NCPF: maximum number of curves per ULTRA file
 */

int PA_th_trans_family(char *name, int ord, int ncpf)
   {int i, nthf, ret;
    char **thfiles;

    nthf = PA_th_family_list(name, 't', &thfiles);

    ret  = PA_th_trans_files(name, ncpf, nthf, thfiles, ord, FALSE);

    for (i = 0; i < nthf; i++)
        SFREE(thfiles[i]);
    SFREE(thfiles);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_TH_FAMILY_LIST - Create a list of all files in a family.
 *                   - Return the number of linked files.
 *
 *                   - NAME:   family name
 *                   - C:      't' for time history or 'u' for ultra files
 *                   - PFILES: pointer to list of files
 */

int PA_th_family_list(char *name, int c, char ***pfiles)
   {int n;
    char bf[MAXLINE];
    SC_array *fa;

    fa = SC_STRING_ARRAY();

    for (n = 0; TRUE; n++)
        {snprintf(bf, MAXLINE, "%s.%c%s", name, c, SC_itoa(n, 36, 2));
         if (!SC_isfile(bf))
            {snprintf(PA_err, MAXLINE, "ERROR: Cannot open file %s", bf);
	     break;};
	 SC_array_string_add_copy(fa, bf);};

    *pfiles = SC_array_done(fa);

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_TH_TRANS_NAME - transpose the explicitly named TH files
 *                  - N is the number of file names
 *                  - NAMES is the array of file names
 *                  - ORD is 1 for normal order and -1 for reverse order
 *                  - NCPF is the number of curves per ULTRA file
 *                  - returns TRUE iff successful
 */

int PA_th_trans_name(int n, char **names, int ord, int ncpf)
   {int i, nthf, ret;
    char **thfiles, *root, bf[MAXLINE], *s;

    nthf = PA_th_name_list(n, names, &thfiles);

    strcpy(bf, names[0]);
    root = SC_strtok(bf, ".", s);

    ret = PA_th_trans_files(root, ncpf, nthf, thfiles, ord, FALSE);

    for (i = 0; i < nthf; i++)
        SFREE(thfiles[i]);
    SFREE(thfiles);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_TH_NAME_LIST - Create a list of all existing files from a list of names.
 *                 - Return the number of files.
 *
 *                 - N:        number of names in original list
 *                 - NAMES:    original list of names
 *                 - PTHFILES: pointer to new list of existing files
 */

int PA_th_name_list(int n, char **names, char ***pthfiles)
   {int i, nthf;
    PDBfile *fp;
    SC_array *fa;

    fa = SC_STRING_ARRAY();

    for (i = 0; i < n; i++)
        {fp = PD_open(names[i], "r");
         if (fp == NULL)
            {snprintf(PA_err, MAXLINE, "ERROR: Cannot open file %s",
                     names[i]);
             break;}
	 SC_array_string_add_copy(fa, names[i]);

         PD_close(fp);};

    nthf      = SC_array_get_n(fa);
    *pthfiles = SC_array_done(fa);

    return(nthf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_TH_TRANS_LINK - transpose the named TH files by following their links
 *                  - N is the number of file/link names
 *                  - NAMES is the array of file/link names
 *                  - ORD is 1 for normal order and -1 for reverse order
 *                  - NCPF is the number of curves per ULTRA file
 *                  - returns TRUE iff successful
 */


int PA_th_trans_link(int n, char **names, int ord, int ncpf)
   {int i, nthf, ret;
    char *root, **thfiles, bf[MAXLINE], *s;

    nthf = PA_th_link_list(n, names, &thfiles);

    strcpy(bf, names[0]);
    root = SC_strtok(bf, ".", s);

    ret = PA_th_trans_files(root, ncpf, nthf, thfiles, ord, FALSE);

    for (i = 0; i < nthf; i++)
        SFREE(thfiles[i]);
    SFREE(thfiles);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_TH_LINK_LIST - Create a list of all files linked to a given list.
 *                 - Return the number of linked files.
 *
 *                 - N:        number of file names in original list
 *                 - NAMES:    original list of file names
 *                 - PTHFILES: pointer to new list of files
 */

int PA_th_link_list(int n, char **names, char ***pthfiles)
   {int i, nthf;
    char bf[MAXLINE];
    PDBfile *fp;
    SC_array *fa;

    fa = SC_STRING_ARRAY();

    for (i = 0; i < n; i++)
        {strcpy(bf, names[i]);
	 while (TRUE)
            {fp = PD_open(bf, "r");
             if (fp == NULL)
                {snprintf(PA_err, MAXLINE, "ERROR: Cannot open file %s", bf);
                 break;}
	     SC_array_string_add_copy(fa, bf);
             if (fp->previous_file != NULL)
                {strcpy(bf, fp->previous_file);
                 PD_close(fp);}
             else
                {PD_close(fp);
                 break;};};};

    strcpy(bf, names[0]);
    nthf      = SC_array_get_n(fa);
    *pthfiles = SC_array_done(fa);

    return(nthf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_SETUP_UF_FAMILY - analyze the given TH file which should be the last
 *                     - one of its family and determine the composition of
 *                     - the corresponding ULTRA file family
 */

static int _PA_setup_uf_family(char *name, char **thfiles,
			       int nthf, int ncpf, int flag)
   {int i, j, k, n_max, nc, np;
    long ind[3];
    char bf[MAXLINE], name2[MAXLINE], tag[MAXLINE], type[MAXLINE], *pl, *nm, *s;
    double ext[2];
    syment *ep;
    dimdes *dp;
    PDBfile *puf, *th;

    if (nthf <= 0)
       return(FALSE);

    strcpy(type, SC_DOUBLE_S);

    n_max       = 10;
    _PA.thd  = FMAKE_N(th_record, n_max, "_PA_SETUP_UF_FAMILY:thd");
    _PA.ndpt = FMAKE_N(dimdes, n_max, "_PA_SETUP_UF_FAMILY:ndpt");
    _PA.ncrv = FMAKE_N(int, n_max, "_PA_SETUP_UF_FAMILY:_PA.ncrv");
    for (k = 0; k < n_max; k++)
        {_PA.thd[k].n_members  = 0;
	 _PA.ndpt[k].index_min = LONG_MAX;
         _PA.ndpt[k].index_max = 0L;};

/* get the defining data from the TH files
 *   # of curves = sum of number of labels for each TH type
 *   # of point per domain = max value of dimension index for each TH type
 *   array of TH types
 */
    _PA.ndom = 0;
    for (j = 0; j < nthf; j++)
        {th = PD_open(thfiles[j], "r");
         if (th == NULL)
            return(FALSE);

         for (i = 0; TRUE; i++)
             {snprintf(bf, MAXLINE, "th%d", i);
              ep = PD_inquire_entry(th, bf, FALSE, NULL);
              if (ep == NULL)
                 break;

	      _PA_rl_th(&_PA.thd[i], 1);

              if (!PD_read(th, bf, &_PA.thd[i]))
                 break;

              ep = PD_inquire_entry(th, _PA.thd[i].entry_name, FALSE, NULL);
              dp = PD_entry_dimensions(ep);

              _PA.ndpt[i].index_min = min(_PA.ndpt[i].index_min, dp->index_min);
              _PA.ndpt[i].index_max = max(_PA.ndpt[i].index_max, dp->index_max);

              _PA.ncrv[i] = SC_MEM_GET_N(char *, _PA.thd[i].labels);

              if (i >= (n_max - 1))
                 {n_max += 10;
                  REMAKE_N(_PA.thd, th_record, n_max);
                  REMAKE_N(_PA.ndpt, dimdes, n_max);
                  for (k = n_max - 10; k < n_max; k++)
                      {_PA.thd[k].n_members = 0;
		       _PA.ndpt[k].index_min = LONG_MAX;
                       _PA.ndpt[k].index_max = 0L;};
                  REMAKE_N(_PA.ncrv, int, n_max);};};

         _PA.ndom = max(_PA.ndom, i);

         PD_close(th);};

    for (i = 0; i < _PA.ndom; i++)
        _PA.ndpt[i].number = _PA.ndpt[i].index_max - _PA.ndpt[i].index_min + 1L;

    _PA.uf = FMAKE_N(PDBfile *, _PA.ndom, "_PA_SETUP_UF_FAMILY:uf");
    ind[0] = 0L;
    ind[2] = 1L;

    strcpy(name2, name);    
    nm = SC_lasttok(name2, "/");
    if (nm != NULL)
       s = nm;
    else
       s = name;

    for (i = 0; i < _PA.ndom; i++)
        {snprintf(bf, MAXLINE, "%s.u%02d", s, i);
         puf = PD_open(bf, "w");
         if (puf == NULL)
            return(FALSE);

         if (flag || _PA_ul_print_flag)
            PRINT(stdout, "Creating ULTRA file %s\n", bf);

         _PA.uf[i] = puf;

/* NOTE: put in something here to map the attributes over */

         nc = _PA.ncrv[i];
         np = _PA.ndpt[i].number;

/* write the curve n_points */
         PD_write(puf, "npts0", SC_INT_S, &np);

/* intialize extrema to dummy values */
	 ext[0] =  HUGE;
	 ext[1] = -HUGE;

/* write dummy x extrema */
         PD_write(puf, "xext0(2)", type, ext);

/* reserve the x values space */
         ind[1] = np - 1;
         PD_defent_alt(puf, "xval0", type, 1, ind);

/* write the tags, labels, and n_points for each file */
         for (j = 0; j < nc; j++)
             {snprintf(tag, MAXLINE,
                       "|labl%d|npts0|xval0|yval%d|xext0|yext%d|",
                      j, j, j);

/* write the curve tag */
              snprintf(bf, MAXLINE, "curve%04d", j);
              ind[1] = strlen(tag);
              PD_write_alt(puf, bf, "char", tag, 1, ind);

/* write the curve label */
              pl = _PA.thd[i].labels[j];
              snprintf(bf, MAXLINE, "labl%d", j);
              ind[1] = strlen(pl);
              PD_write_alt(puf, bf, "char", pl, 1, ind);

/* write dummy y extrema */
	      snprintf(bf, MAXLINE, "yext%d(2)", j);
	      PD_write(puf, bf, type, ext);

/* reserve the y values space */
              snprintf(bf, MAXLINE, "yval%d", j);
              ind[1] = np - 1;
              PD_defent_alt(puf, bf, type, 1, ind);};};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_TRANSPOSE_STRIPE - copy and convert the stripe data which may 
 *                      - be of mixed type (heterogeneous struct) into
 *                      - the curve arrays which are of type double
 */

static int _PA_transpose_stripe(PDBfile *file, double **crve, char *stripe,
			        char *type, char *mix, int na, int nrd)
   {int j, k, nv, ns, rv, sid;
    defstr *dp;
    memdes *desc;
    double *data, *pd;

    dp = PD_inquire_type(file, type);
    ns = 0;

    desc = dp->members;
    if (desc == NULL)
       nv = 1;
    else
       for (nv = 0; desc != NULL; desc = desc->next, nv++);

    if (strcmp(mix, _PA.heterogeneous) == 0)
       {int tmp, incr;
        long mitems, offs, bpm;
        char *ps, *mtype;
        hasharr *tab;

        tab  = file->host_chart;
        offs = 0L;
        ps   = stripe;
        for (k = na; k < nrd; k++)
            {incr  = _PD_align(offs, type, dp->is_indirect, tab, &tmp);
             offs += incr;
             ps   += incr;
             for (j = 0, desc = dp->members;
                  desc != NULL;
                  j++, desc = desc->next)
                 {mitems = desc->number;
                  mtype  = desc->type;
                  bpm    = mitems*_PD_lookup_size(mtype, tab);
                  incr   = _PD_align(offs, mtype, desc->is_indirect, tab, &tmp);
		  sid    = SC_type_id(mtype, FALSE);

/* increment the offsets to the alignments */
                  offs += incr;
                  ps   += incr;

                  pd = &crve[j][k];
                  SC_convert_id(SC_DOUBLE_I, pd, 0, 1, sid, ps, 0, 1, 1, FALSE);

/* increment to the next member */
                  offs += bpm;
                  ps   += bpm;};};}

    else
       {sid  = SC_type_id(mix, FALSE);
        ns   = (nrd - na)*nv;
        data = SC_convert_id(SC_DOUBLE_I, NULL, 0, 1,
			     sid, stripe, 0, 1, ns, FALSE);

        pd = data;
        for (k = na; k < nrd; k++)
            {for (j = 0; j < nv; j++)
                 crve[j][k] = *pd++;};

        SFREE(data);};

    rv = ns*nv;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_TYPE_MIX - report the member type mix in the given type
 *              - if the members all have one type, return its value
 *              - else return HETEROGENEOUS
 */

static char *_PA_type_mix(PDBfile *file, char *type)
   {defstr *dp;
    memdes *desc;

    dp = PD_inquire_type(file, type);
    if (dp == NULL)
       return(NULL);

    desc = dp->members;
    if (desc == NULL)
       return(type);

    strcpy(_PA.type, desc->type);
    for (; desc != NULL; desc = desc->next)
        {if (strcmp(_PA.type, desc->type) != 0)
            return(_PA.heterogeneous);};

    return(_PA.type);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_PROC_REC - process a single time history data set from a single file */

static int _PA_proc_rec(char *name, PDBfile *th, int ncpf, int recn)
   {int i, j, n, nc, nv, nptm;
    long ns, na, nrd, nitems, offs, ind[3];
    int64_t addr;
    char bf[MAXLINE], rname[MAXLINE], type[MAXLINE], *rtyp, *tmix;
    double **crve;
    syment *ep, *en;
    dimdes *dp;
    PDBfile *pduf;
    FILE *fp;
    void *stripe;

    fp = th->stream;

    strcpy(rname, _PA.thd[recn].entry_name);
    ep = PD_inquire_entry(th, rname, FALSE, NULL);
    if (ep == NULL)
       return(FALSE);

    dp   = PD_entry_dimensions(ep);
    rtyp = PD_entry_type(ep);

    tmix = _PA_type_mix(th, rtyp);
    if (tmix == NULL)
       return(FALSE);

    nc = _PA.ncrv[recn];
    nv = nc + 1;

    pduf = _PA.uf[recn];

/* read the data in from the pp file */
    n  = _PD_n_blocks(ep);
    if (n == 1)
       _PD_entry_set_number(ep, 0, PD_entry_number(ep));

    en = _PD_mk_syment(NULL, 0L, 0L, NULL, NULL);

    if (en != NULL)
       {PD_entry_type(en) = rtyp;
	SC_mark(rtyp, 1);};

/* find the maximum number of stripe laid out */
    ns = 0L;
    for (i = 0; i < n; i++)
        {nitems = _PD_entry_get_number(ep, i);
         ns     = max(ns, nitems);};

    stripe = _PD_alloc_entry(th, rtyp, ns);

/* allocate the curve arrays */
    crve = FMAKE_N(double *, nv, "_PA_PROC_REC:crve");
    strcpy(type, SC_DOUBLE_S);

    nptm = _PA.ndpt[recn].number;
    for (i = 0; i < nv; i++)
        crve[i] = FMAKE_N(double, nptm, "_PA_PROC_REC:crve[]");

    na  = 0L;
    nrd = 0L;
    for (i = 0; i < n; i++)
        {addr   = _PD_entry_get_address(ep, i);
         nitems = _PD_entry_get_number(ep, i);

         if (lio_seek(fp, addr, SEEK_SET))
            {snprintf(PD_err, MAXLINE, "ERROR: SEEK FAILED - _PA_PROC_REC");
             return(FALSE);};

         _PD_entry_set_address(en, 0, addr);
         _PD_entry_set_number(en, 0, nitems);
	 PD_entry_number(en) = nitems;

         nrd += _PD_sys_read(th, en, rtyp, stripe);

/* transpose through the copy */
         _PA_transpose_stripe(th, crve, stripe, rtyp, tmix, na, nrd);

         na = nrd;};

    _PD_rl_syment(en);

/* setup the index for all curve data */
    offs = _PA.ndpt[recn].index_min;

    ind[0] = dp->index_min - offs;
    ind[1] = dp->index_max - offs;
    ind[2] = 1L;

/* write out the time plot domain data */
    PD_write_alt(pduf, "xval0", type, crve[0], 1, ind);

    SFREE(crve[0]);

/* write out the time plot range data */
    for (i = 0; i < nc; i++)
        {j = i + 1;
         snprintf(bf, MAXLINE, "yval%d", i);
         PD_write_alt(pduf, bf, type, crve[j], 1, ind);
         SFREE(crve[j]);};

    SFREE(stripe);
    SFREE(crve);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_TH_TRANS_FILES - process a list of time history files into ULTRA files
 *                   - NAME is the base name of both the TH and ULTRA file
 *                   - families
 *                   - NCPF is the maximum number of curves per ULTRA file
 *                   - FLAG TRUE is verbose
 */

int PA_th_trans_files(char *name, int ncpf, int nthf, char **thfiles,
		      int ord, int flag)
   {int i, j, n, ret;
    int n_min, n_inc;
    double *data, ext[2];
    char bf[MAXLINE], type[MAXLINE];
    PDBfile *th;

    if (name == NULL)
       return(FALSE);

/* set up the family of ULTRA files */
    if (!_PA_setup_uf_family(name, thfiles, nthf, ncpf, flag))
       return(FALSE);

/* process the raw data files */
    if (ord == 1)
       {n_min = 0;
        n_inc = ord;}
    else
       {n_min = nthf - 1;
        n_inc = ord;};

    for (j = n_min; (j >= 0) && (j < nthf); j += n_inc)
        {th = PD_open(thfiles[j], "r");
         if (th == NULL)
            return(FALSE);

         for (i = 0; i < _PA.ndom; i++)
             _PA_proc_rec(name, th, ncpf, i);

         if (!PD_close(th))
            {snprintf(PD_err, MAXLINE,
		      "ERROR: TROUBLE CLOSING %s.t%02d - PA_TH_TRANS_FILES",
		      name, j);
             return(FALSE);};};

/* find the extrema */
    strcpy(type, SC_DOUBLE_S);

    for (i = 0; i < _PA.ndom; i++)
	{PD_read(_PA.uf[i], "npts0", &n);
	 data = FMAKE_N(double, n, "PA_TH_TRANS_FILES:data");
	 PD_read(_PA.uf[i], "xval0", data);
	 PM_maxmin(data, ext, ext + 1, n);
	 PD_write(_PA.uf[i], "xext0", type, ext);

	 for (j = 0; j < _PA.ncrv[i]; j++)
	     {snprintf(bf, MAXLINE, "yval%d", j);
	      PD_read(_PA.uf[i], bf, data);
	      PM_maxmin(data, ext, ext + 1, n);
	      snprintf(bf, MAXLINE, "yext%d", j);
	      PD_write(_PA.uf[i], bf, type, ext);};

         SFREE(data)};

/* clean up the memory */
    _PA_rl_th(_PA.thd, _PA.ndom);

    SFREE(_PA.thd);
    SFREE(_PA.ndpt);
    SFREE(_PA.ncrv);

/* close out the ULTRA files */
    ret = TRUE;
    for (i = 0; i < _PA.ndom; i++)
        {if (PD_close(_PA.uf[i]))
            {if (flag)
                PRINT(stdout, "Closing ULTRA file %s.u%02d\n", name, i);}
         else
            {if (flag)
                PRINT(stdout, "Error closing ULTRA file %s.u%02d\n", name, i);
             ret = FALSE;};};

    SFREE(_PA.uf);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_MERGE_FAMILY - Merge the curves from ULTRA files with a given base name
 *                 - into one family of files.
 *
 *                 - BASE:   base name of the merged (target) files.
 *                 - FAMILY: is the base name of the family of files to merge. 
 *                 - NCPF:   approximate number of curves per target file.
 *                 -         (If NCPF = 0, don't family merged file.)
 */

int PA_merge_family(char *base, char *family, int ncpf)
   {int i, n, ret;
    char **files;

    n   = PA_th_family_list(family, 'u', &files);

    ret = PA_merge_files(base, n, files, ncpf);

    for (i = 0; i < n; i++)
        SFREE(files[i]);
    SFREE(files);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_MERGE_FILES - Merge the curves from an arbitrary list of ULTRA files
 *                - into a single family of files.
 *
 *                - BASE:  base name of the merged (target) files.
 *                - N:     number of file names in FILES.
 *                - FILES: list of source files to be merged.
 *                - NCPF:  approximate number of curves per target file.
 *                -        (If NCPF = 0, don't family merged file.)
 *
 *                - NOTE: This algorithm assumes that all curves in a given
 *                - source file share the domain in the first curve. This is
 *                - true for PANACEA time history files, but not in general.
 *                - Also, source files are not split across target files. Both
 *                - of these features are subject to change with some loss of
 *                - efficiency and simplicity.
 */

int PA_merge_files(char *base, int n, char **files, int ncpf)
   {int i, ics, ix, ict, nc, npts, err;
    PDBfile *fpt, *fps;
    char s[MAXLINE], **names;
    double xmin, xmax, ymin, ymax, *px, *py;
    
    if (n <= 0)
       {snprintf(PD_err, MAXLINE, "ERROR: NO FILES SPECIFIED - PA_MERGE_FILES");
	return(FALSE);};

    ict = 0;
    snprintf(s, MAXLINE, "%s.u00", base);
    fpt = PD_open(s, "w");
    if (fpt == NULL)
       return(FALSE);

/*  Force PD_family to create a new file each time it is called below */
    PD_set_max_file_size(fpt, 0L);

    for (i = 0; i < n; i++)
        {fps = PD_open(files[i], "r");
	 if (fps == NULL)
	    {PRINT(stdout, "WARNING: CANNOT OPEN FILE %s - PA_MERGE_FILES\n",
		   files[i]);
	     continue;};
	 nc    = SC_hasharr_get_n(fps->symtab);
	 names = SC_hasharr_dump(fps->symtab, "*curve*", NULL, FALSE);
	 if (names == NULL)
	    {PRINT(stdout, "WARNING: NO CURVES IN FILE %s - PA_MERGE_FILES\n",
		   files[i]);
	     PD_close(fps);
	     continue;};

	 for (ics = 0, ix = ict; (names[ics] != NULL) && (ics < nc); ics++)
	     {err = TRUE;
	      if (ics == 0)
		 {if (PD_read_pdb_curve(fps, names[ics], &px, &py, &npts, s,
					  &xmin, &xmax, &ymin, &ymax, X_AND_Y))
		     err = !PD_wrt_pdb_curve(fpt, s, npts, px, py, ict++);
		  if (err)
		     {PRINT(stdout,
			    "WARNING: SKIPPED BAD FILE %s - PA_MERGE_FILES\n",
			    files[i]);
		      ict--;
		      break;};}
	      else
		 {if (PD_read_pdb_curve(fps, names[ics], &px, &py, &npts, s,
					&xmin, &xmax, &ymin, &ymax, Y_ONLY))
		     err = !PD_wrt_pdb_curve_y(fpt, s, npts, ix, py, ict++);
		  if (err)
		     {PRINT(stdout,
			    "WARNING: SKIPPED CURVE %d FILE %s - PA_MERGE_FILES\n",
			    ics + 1, files[i]);
		      ict--;};};};
	 SFREE(names);

	 if ((ncpf != 0) && (ict >= ncpf) && ((i + 1) < n))
	    {fpt = PD_family(fpt, TRUE);
	     ict = 0;};
	 PD_close(fps);};

    PD_close(fpt);

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                        FORTRAN API ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* PATHOP - open a time history file */

FIXNUM F77_FUNC(pathop, PATHOP)(FIXNUM *pnf, F77_string fname, FIXNUM *pnm,
				F77_string fmode, FIXNUM *psz, FIXNUM *pnp,
				F77_string fprev)
   {int np;
    FIXNUM rv;
    char s[MAXLINE], t[2], u[MAXLINE], *lmode;
    PDBfile *file;

    SC_FORTRAN_STR_C(s, fname, *pnf);

    lmode = SC_F77_C_STRING(fmode);

    t[0] = *lmode;
    t[1] = '\0';

    np = *pnp;
    if (np == 0)
       file = PA_th_open(s, t, (long) *psz, NULL);
    else
       {SC_FORTRAN_STR_C(u, fprev, np);
        file = PA_th_open(s, t, (long) *psz, u);};

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

FIXNUM F77_FUNC(pathfm, PATHFM)(FIXNUM *fileid)
   {FIXNUM ret;
    PDBfile *file, *newfile;

    file = SC_GET_POINTER(PDBfile, *fileid);
    ret = *fileid;    

    newfile = PA_th_family(file);

    if (newfile != file)
       {ret = SC_ADD_POINTER(newfile);
        file = SC_DEL_POINTER(PDBfile, *fileid);}
        
    if (newfile == NULL)
       ret = -1;
    else
       {newfile->major_order    = COLUMN_MAJOR_ORDER;
        newfile->default_offset = 1;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PABREC - begin a time domain struct definition */

FIXNUM F77_FUNC(pabrec, PABREC)(FIXNUM *fileid, FIXNUM *pnf, F77_string fname,
				FIXNUM *pnt, F77_string ftype, FIXNUM *pnd,
				F77_string ftime)
   {size_t nc;
    FIXNUM rv;
    char ltype[MAXLINE], lname[MAXLINE], ltime[MAXLINE];
    f77_th_record *fth;

    nc = *pnt;
    SC_FORTRAN_STR_C(ltype, ftype, nc);

    nc = *pnf;
    SC_FORTRAN_STR_C(lname, fname, nc);

    fth = FMAKE(f77_th_record, "PABREC:fth");
    
    fth->labels     = SC_STRING_ARRAY();
    fth->members    = SC_STRING_ARRAY();
    fth->type       = SC_strsavef(ltype, "char*:PABREC:type");
    fth->entry_name = SC_strsavef(lname, "char*:PABREC:name");

    nc = *pnd;
    SC_FORTRAN_STR_C(ltime, ftime, nc);

    SC_array_string_add_copy(fth->members, ltime);

    rv = SC_ADD_POINTER(fth);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAGRID - return the record id for the named record */

FIXNUM F77_FUNC(pagrid, PAGRID)(FIXNUM *fileid, FIXNUM *pind, FIXNUM *pnn,
				F77_string name, FIXNUM *pnt, F77_string type,
				FIXNUM *prid)
   {int n;
    FIXNUM rv;
    char dname[MAXLINE], *s;
    f77_th_record *fth;
    th_record thd;
    PDBfile *file;

    rv = FALSE;

    file = SC_GET_POINTER(PDBfile, *fileid);
    
    snprintf(dname, MAXLINE, "th%d", (int) *pind);
    if (PD_read(file, dname, &thd) != 0)
       {fth = FMAKE(f77_th_record, "PAGRID:fth");
    
	n = thd.n_members;

	fth->members    = SC_strings_array(n, thd.members);
        fth->labels     = SC_strings_array(n-1, thd.labels);
	fth->type       = thd.type;
	fth->entry_name = thd.entry_name;

	s = thd.entry_name;
	n = strlen(s);
	SC_strncpy(SC_F77_C_STRING(name), *pnn, s, n);
	*pnn = n;

	s = thd.type;
	n = strlen(s);
	SC_strncpy(SC_F77_C_STRING(type), *pnt, s, n);
	*pnt  = n;

	*prid = (FIXNUM) SC_ADD_POINTER(fth);

        rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAAREC - add a member to a time domain struct definition */

FIXNUM F77_FUNC(paarec, PAAREC)(FIXNUM *fileid, FIXNUM *recid, FIXNUM *pnm,
				F77_string fmemb, FIXNUM *pnl, F77_string flabl)
   {int nc;
    FIXNUM rv;
    char lmemb[MAXLINE], llabl[MAXLINE], *s;
    f77_th_record *fth;

    fth  = SC_GET_POINTER(f77_th_record, *recid);

    nc = *pnm;
    SC_FORTRAN_STR_C(lmemb, fmemb, nc);

    nc = *pnl;
    SC_FORTRAN_STR_C(llabl, flabl, nc);

    SC_array_string_add_copy(fth->members, lmemb);

    if (nc > 0)
       s = SC_strsavef(llabl, "char*:PAAREC:s");
    else
       s = NULL;

    SC_array_string_add(fth->labels, s);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAEREC - finish a time domain struct definition */

FIXNUM F77_FUNC(paerec, PAEREC)(FIXNUM *fileid, FIXNUM *recid)
   {int nm, nl;
    FIXNUM rv;
    char **sm, **sl;
    PDBfile *file;
    f77_th_record *fth;
    defstr *dp;
    SC_array *fm, *fl;

    file = SC_GET_POINTER(PDBfile, *fileid);
    fth  = SC_GET_POINTER(f77_th_record, *recid);
    fm   = fth->members;
    fl   = fth->labels;

    nm = SC_array_get_n(fm);
    sm = SC_array_array(fm);

    nl = SC_array_get_n(fl);
    sl = SC_array_array(fl);

    dp = PA_th_def_rec(file, fth->entry_name, fth->type, nm, sm, sl);

    rv = (dp != NULL);

    SFREE(sm);
    SFREE(sl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAWREC - write a time domain record */

FIXNUM F77_FUNC(pawrec, PAWREC)(FIXNUM *fileid, FIXNUM *recid,
				FIXNUM *pinst, FIXNUM *pnr, void *vr)
   {FIXNUM ret;
    PDBfile *file;
    f77_th_record *fth;

    file = SC_GET_POINTER(PDBfile, *fileid);
    fth  = SC_GET_POINTER(f77_th_record, *recid);

    ret = PA_th_write(file, fth->entry_name, fth->type,
                      (int) *pinst, (int) *pnr, vr);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAWMEM - write a time domain record member */

FIXNUM F77_FUNC(pawmem, PAWMEM)(FIXNUM *fileid, FIXNUM *recid, FIXNUM *pnc,
				F77_string mbr, FIXNUM *pinst, FIXNUM *pnr,
				void *vr)
   {FIXNUM ret;
    char memb[MAXLINE];
    PDBfile *file;
    f77_th_record *fth;

    file = SC_GET_POINTER(PDBfile, *fileid);
    fth  = SC_GET_POINTER(f77_th_record, *recid);

    SC_FORTRAN_STR_C(memb, mbr, *pnc);

    ret = PA_th_wr_member(file, fth->entry_name, memb, fth->type,
			  (int) *pinst, vr);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAWRIA - write a time domain record instance attribute */

FIXNUM F77_FUNC(pawria, PAWRIA)(FIXNUM *fileid, FIXNUM *pnv, F77_string fvar,
				FIXNUM *pinst, FIXNUM *pna, F77_string fattr,
				void *avl)
   {FIXNUM ret;
    char lvar[MAXLINE], lattr[MAXLINE];
    PDBfile *file;

    file = SC_GET_POINTER(PDBfile, *fileid);

    SC_FORTRAN_STR_C(lvar, fvar, *pnv);
    SC_FORTRAN_STR_C(lattr, fattr, *pna);

    ret = PA_th_wr_iattr(file, lvar, (int) *pinst, lattr, avl);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATHTR - transpose a time history file family */
/* THIS FUNCTION IS DEPRECATED - USE PATRNF INSTEAD */

FIXNUM F77_FUNC(pathtr, PATHTR)(FIXNUM *pnf, F77_string fname, FIXNUM *pncpf)
   {FIXNUM ret;
    char s[MAXLINE];

    SC_FORTRAN_STR_C(s, fname, *pnf);

    ret = PA_th_trans_family(s, 1, (int) *pncpf);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATRNF - transpose a time history file family
 *        - Return 1 if successful and 0 otherwise.
 */

FIXNUM F77_FUNC(patrnf, PATRNF)(FIXNUM *pnf, F77_string fname, FIXNUM *pord,
				FIXNUM *pncpf)
   {FIXNUM ret;
    char s[MAXLINE];

    SC_FORTRAN_STR_C(s, fname, *pnf);

    ret = PA_th_trans_family(s, (int) *pord, (int) *pncpf);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATHTN - transpose a time history file family by explicit names
 *        - THIS FUNCTION IS DEPRECATED - USE PATRNN INSTEAD
 */

FIXNUM F77_FUNC(pathtn, PATHTN)(F77_string chrs, FIXNUM *pord, FIXNUM *pncpf)
   {int n;
    FIXNUM ret;
    char *pc, **names;

    pc    = SC_F77_C_STRING(chrs);
    names = SC_tokenize(pc, " \t\f\n\r");
    for (n = 0; names[n] != NULL; n++);

    ret = PA_th_trans_name(n, names, (int) *pord, (int) *pncpf);

    SC_free_strings(names);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATRNN - transpose a time history file family by explicit names
 *        - Return 1 if successful and 0 otherwise.
 */

FIXNUM F77_FUNC(patrnn, PATRNN)(FIXNUM *pnchrs, F77_string chrs,
				FIXNUM *pord, FIXNUM *pncpf)
   {int n;
    FIXNUM ret;
    char **names, *pc;

    pc = FMAKE_N(char, *pnchrs + 2, "PATRNN:pc");
    SC_FORTRAN_STR_C(pc, chrs, *pnchrs);

    names = SC_tokenize(pc, " \t\f\n\r");
    for (n = 0; names[n] != NULL; n++);

    ret = PA_th_trans_name(n, names, (int) *pord, (int) *pncpf);

    SC_free_strings(names);

    SFREE(pc);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATHTL - transpose a time history file family by links
 *        - THIS FUNCTION IS DEPRECATED - USE PATRNL INSTEAD
 */

FIXNUM F77_FUNC(pathtl, PATHTL)(F77_string chrs, FIXNUM *pord, FIXNUM *pncpf)
   {int n;
    FIXNUM ret;
    char **names, *pc;

    pc    = SC_F77_C_STRING(chrs);
    names = SC_tokenize(pc, " \t\f\n\r");
    for (n = 0; names[n] != NULL; n++);

    ret = PA_th_trans_link(n, names, (int) *pord, (int) *pncpf);
    
    SC_free_strings(names);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATRNL - transpose a time history file family by links
 *        - Return 1 if successful and 0 otherwise.
 */

FIXNUM F77_FUNC(patrnl, PATRNL)(FIXNUM *pnchrs, F77_string chrs,
				FIXNUM *pord, FIXNUM *pncpf)
   {int n;
    FIXNUM ret;
    char **names, *pc;

    pc = FMAKE_N(char, *pnchrs + 2, "PATRNL:pc");
    SC_FORTRAN_STR_C(pc, chrs, *pnchrs);

    names = SC_tokenize(pc, " \t\f\n\r");
    for (n = 0; names[n] != NULL; n++);

    ret = PA_th_trans_link(n, names, (int) *pord, (int) *pncpf);
    
    SC_free_strings(names);

    SFREE(pc);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAMRGF - FORTRAN interface routine to merge a family of ULTRA files
 *        - Return 1 if successful and 0 otherwise.
 *
 *        - PNB:    Number of characters in BASE.
 *        - BASE:   Base name of target (merged) file family.
 *        - PNF:    Number of characters in FAMILY.
 *        - FAMILY: Base name of source file family.
 *        - PNCPF:  Number of curves per target file.
 */

FIXNUM F77_FUNC(pamrgf, PAMRGF)(FIXNUM *pnb, F77_string base, FIXNUM *pnf,
				F77_string family, FIXNUM *pncpf)
   {FIXNUM ret;
    char s[MAXLINE];
    char t[MAXLINE];

    SC_FORTRAN_STR_C(s, base, *pnb);
    SC_FORTRAN_STR_C(t, family, *pnf);

    ret = PA_merge_family(s, t, (int) *pncpf);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAMRGN - FORTRAN interface routine to merge a list of ULTRA files.
 *        - Return 1 if successful and 0 otherwise.
 *
 *        - PNB:    Number of characters in BASE.
 *        - BASE:   Base name of target (merged) file family.
 *        - PNCHRS: Number of characters in CHRS.
 *        - CHRS:   List of file names.
 *        - PNCPF:  Number of curves per target file.
 */

FIXNUM F77_FUNC(pamrgn, PAMRGN)(FIXNUM *pnb, F77_string base, FIXNUM *pnchrs,
				F77_string chrs, FIXNUM *pncpf)
   {int n;
    FIXNUM ret;
    char **files, *pc, s[MAXLINE];

    pc = FMAKE_N(char, *pnchrs + 2, "PAMRGN:pc");
    SC_FORTRAN_STR_C(pc, chrs, *pnchrs);
    SC_FORTRAN_STR_C(s, base, *pnb);

    files = SC_tokenize(pc, " \t\f\n\r");
    for (n = 0; files[n] != NULL; n++);

    ret = PA_merge_files(s, n, files, (int) *pncpf);

    SC_free_strings(files);

    SFREE(pc);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PAFREC - Free a time domain record */

FIXNUM F77_FUNC(pafrec, PAFREC)(FIXNUM *recid)
   {FIXNUM rv;
    f77_th_record *fth;

    fth = SC_GET_POINTER(f77_th_record, *recid);

    SC_free_array(fth->labels, SC_array_free_n);
    SC_free_array(fth->members, SC_array_free_n);

    SFREE(fth->type);
    SFREE(fth->entry_name);

    SFREE(fth);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
