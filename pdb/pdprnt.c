/*
 * PDPRNT.C - PDB write functionality in PD
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

#define LINE_SIZE 90

typedef int (*PFPrnt)(FILE *fp, void *p, int i, int mode);

static int
 _PD_print_data(FILE *f0,
		char *prefix, char *before, char *after,
		char *nodename, const PDBfile *file, const void *vr,
		inti ni, char *type, dimdes *dims,
		int mjr, int def_off, int irecursion,
		int n, long *ind);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DISP_MODE_I - print a single item from array X */

static void _PD_disp_mode_i(PD_printdes *prnt, const void *x, int tid,
			    long n, long *ind)
   {long i, j;
    char bf[MAXLINE];
    FILE *fp;

    fp = prnt->fp;

    for (i = 0L; i < n; i++)
        {j = ind[i];
         if (PD_gs.print_ctrl[5] == 0)
	    {PRINT(fp, "%s%s%s(%s) = ",
                   prnt->prefix, prnt->after, prnt->nodename,
                   PD_index_to_expr(bf, j,
				    prnt->dims, prnt->mjr, prnt->def_off));}
         else
	    {PRINT(fp, "\na> %s", prnt->prefix);};

	 io_puts(SC_ntos(bf, MAXLINE, tid, x, j, 1), fp);

         PRINT(fp, "\n");}

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DISP_MODE_1 - print a single item from scalar X
 *                 - according to formats1
 */

static void _PD_disp_mode_1(PD_printdes *prnt, const void *x, int tid)
   {char bf[MAXLINE];
    FILE *fp;

    fp = prnt->fp;

    if (PD_gs.print_ctrl[5] == 0)
       {PRINT(fp, "%s%s%s = ", prnt->prefix, prnt->before, prnt->nodename);}
    else if (PD_gs.print_ctrl[5] == 1)
       {PRINT(fp, "%s%s", prnt->before, prnt->nodename);}
    else
       {PRINT(fp, "        ");};

    io_puts(SC_ntos(bf, MAXLINE, tid, x, 0, 1), fp);

    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DISP_MODE_2 - print a NI items from array X according to formats1
 *                 - where NI is less than the number of items per line
 */

static void _PD_disp_mode_2(PD_printdes *prnt, const void *x,
			    inti ni, int tid)
   {inti i, j;
    char bf[MAXLINE];
    FILE *fp;

    fp = prnt->fp;

    i = 0L;
    j = 0L;
    if (PD_gs.print_ctrl[5] == 0)
       {PRINT(fp, "%s%s%s(%s) = ",
              prnt->prefix, prnt->before, prnt->nodename,
              PD_index_to_expr(bf, j,
			       prnt->dims, prnt->mjr, prnt->def_off));}
    else if (PD_gs.print_ctrl[5] == 1)
       {PRINT(fp, "%s%s ", prnt->before, prnt->nodename);}
    else
       {PRINT(fp, "        ");};

    io_puts(SC_ntos(bf, MAXLINE, tid, x, i, 1), fp);

    PRINT(fp, "\n");

    j += prnt->offset;
    for (i = 1L; i < ni; i++, j += prnt->offset)
        {if (PD_gs.print_ctrl[5] == 0)
            {PRINT(fp, "%s%s%s(%s) = ",
                   prnt->prefix, prnt->after, prnt->nodename,
                   PD_index_to_expr(bf, j,
				    prnt->dims, prnt->mjr, prnt->def_off));}
         else if (PD_gs.print_ctrl[5] == 1)
           {PRINT(fp, "%s%s ", prnt->before, prnt->nodename);}
	 else
           {PRINT(fp, "        ");};

	 io_puts(SC_ntos(bf, MAXLINE, tid, x, i, 1), fp);

         PRINT(fp, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DISP_MODE_3 - print a NI items from array X according to formats1
 *                 - where NI is greater than or equal to
 *                 - the number of items per line
 */

static void _PD_disp_mode_3(PD_printdes *prnt, const void *x,
			    inti ni, int tid)
   {int nn;
    inti i, j, k;
    char bf[MAXLINE], s[MAXLINE];
    FILE *fp;

    fp = prnt->fp;
    nn = prnt->nn;

    i = 0L;
    j = 0L;
    if (PD_gs.print_ctrl[5] == 0)
       {PRINT(fp, "%s%s%s\n", prnt->prefix, prnt->before, prnt->nodename);
        snprintf(s, MAXLINE, "  (%s)                              ",
		 PD_index_to_expr(bf, j,
				  prnt->dims, prnt->mjr, prnt->def_off));
	s[nn] = '\0';
        PRINT(fp, "%s", s);}
    else
       {PRINT(fp, "        ");};

    for (k = 0; i < ni; i++, j += prnt->offset, k++)
        {if (k >= PD_gs.print_ctrl[4])
            {if (PD_gs.print_ctrl[5] == 0)
                {snprintf(s, MAXLINE, "  (%s)                              ",
			  PD_index_to_expr(bf, j,
					   prnt->dims, prnt->mjr, prnt->def_off));
		 s[nn] = '\0';
                 PRINT(fp, "\n%s", s);}
	     else
	        {PRINT(fp, "\n        ");};
	     k = 0;};

	 io_puts(SC_ntos(bf, MAXLINE, tid, x, i, 2), fp);};

    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DISP_DATA - print NI values from X of type indexed by TID */

static void _PD_disp_data(PD_printdes *prnt, const void *x,
			  inti ni, int tid, long n, long *ind)
   {

    if ((n > 0L) && (ind != NULL))
       _PD_disp_mode_i(prnt, x, tid, n, ind);

    else if (ni == 1L)
       _PD_disp_mode_1(prnt, x, tid);

    else if (ni < PD_gs.print_ctrl[3])
       _PD_disp_mode_2(prnt, x, ni, tid);

    else
       _PD_disp_mode_3(prnt, x, ni, tid);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRITE_EXTRAS - write the extra stuff about a PDBfile
 *
 * #bind PD_write_extras fortran() scheme() python()
 */

void PD_write_extras(FILE *f0, const PDBfile *file ARG(,,cls))
   {long i;
    char *date;
    defstr *dp;

    if (_PD.maxfsize == 0)
       SC_fix_lmt(sizeof(int64_t), NULL, &_PD.maxfsize, NULL);

    date = file->date;
    date = (date == NULL) ? "none" : date;

    PRINT(f0, "\n");

    PRINT(f0, "File Name: %s\n", file->name);
    if (file->type == NULL)
       PRINT(f0, "File Type: PDB\n");
    else
       PRINT(f0, "File Type: %s\n", file->type);
    PRINT(f0, "File Creation Date: %s\n", date);
    PRINT(f0, "PDB Version: %d\n", file->system_version);
    PRINT(f0, "PDB Format Version: %d\n", file->format_version);
    PRINT(f0, "Default Offset: %d\n", file->default_offset);
    if (file->major_order == ROW_MAJOR_ORDER)
       PRINT(f0, "Array Order: ROW MAJOR (C)\n");
    else
       PRINT(f0, "Array Order: COLUMN MAJOR (FORTRAN)\n");

/* checksum info */
    if (file->cksum.use != PD_MD5_OFF)
       {PRINT(f0, "Checksums: ");
	if (file->cksum.use & PD_MD5_FILE)
	   PRINT(f0, "file ");
	if (file->cksum.use & PD_MD5_RW)
	   PRINT(f0, "variable ");
	PRINT(f0, "\n");};

/* type info */
    PRINT(f0, "Types Needing Conversion:");
    for (i = 0; SC_hasharr_next(file->chart, &i, NULL, NULL, (void **) &dp); i++)
        {if (dp->convert > 0)
	    PRINT(f0, " %s", dp->type);};
    PRINT(f0, "\n");

    if (file->attrtab != NULL)
       PRINT(f0, "Attribute Table: Yes\n");

    if (file->maximum_size != _PD.maxfsize)
       PRINT(f0, "Maximum family member size: %ld\n", file->maximum_size);

    if (file->previous_file != NULL)
       PRINT(f0, "Previous file in family: %s\n", file->previous_file);

/* symtab and chart info */
    if (file->virtual_internal)
       {PRINT(f0, "Symbol Table Address: 0x%lx\n", file->symtab);
	PRINT(f0, "Structure Chart Address: 0x%lx\n", file->chart);}
    else
       {PRINT(f0, "Header Address: %s\n",
	      SC_itos(NULL, 0, file->headaddr, NULL));
	PRINT(f0, "Symbol Table Address: %s\n",
	      SC_itos(NULL, 0, file->symtaddr, NULL));
	PRINT(f0, "Structure Chart Address: %s\n",
	      SC_itos(NULL, 0, file->chrtaddr, NULL));};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_PRINT_EXTRAS - print the extra stuff about a PDBfile
 *
 * #bind PD_print_extras fortran() scheme() python()
 */

void PD_print_extras(const PDBfile *file ARG(,,cls))
   {

    PD_write_extras(stdout, file);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRITE_SYMENT - write a symbol table entry in human readable form
 *
 * #bind PD_write_syment fortran() scheme() python()
 */

void PD_write_syment(FILE *f0, syment *ep)
   {char t[2][MAXLINE];
    dimdes *dim;

    PRINT(f0, "Type: %s\n", PD_entry_type(ep));
    if (PD_entry_dimensions(ep) != NULL)
       {PRINT(f0, "Dimensions: (");

        for (dim = PD_entry_dimensions(ep); dim != NULL; dim = dim->next)
            {SC_itos(t[0], MAXLINE, dim->index_min, NULL);
	     SC_itos(t[1], MAXLINE, dim->index_max, NULL);
	     PRINT(f0, "%s:%s", t[0], t[1]);
	     if (dim->next != NULL)
	        PRINT(f0, ", ");};

	PRINT(f0, ")\n");};

    PRINT(f0, "Length: %s\n", SC_itos(NULL, 0, PD_entry_number(ep), NULL));
    PRINT(f0, "Address: %s\n", SC_itos(NULL, 0, PD_entry_address(ep), NULL));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_PRINT_SYMENT - print a symbol table entry in human readable form
 *
 * #bind PD_print_syment fortran() scheme() python()
 */

void PD_print_syment(syment *ep)
   {

    PD_write_syment(stdout, ep);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRITE_DEFSTR - write a defstr in human readable form
 *
 * #bind PD_write_defstr fortran() scheme() python()
 */

void PD_write_defstr(FILE *f0, defstr *dp)
   {memdes *lst, *nxt;
    char bg[80];

    SC_strncpy(bg, 80, "Members: {", -1);

    PRINT(f0, "Type: %s\n", dp->type);
    PRINT(f0, "Alignment: %d\n", dp->alignment);
    if (dp->members != NULL)
       {for (lst = dp->members; lst != NULL; lst = nxt)
            {nxt = lst->next;
             if (lst->cast_offs < 0L)
                {if (nxt == NULL)
                    PRINT(f0, "%s%s;}\n", bg, lst->member);
                 else
                    PRINT(f0, "%s%s;\n", bg, lst->member);}
             else
                {if (nxt == NULL)
                    PRINT(f0, "%s%s;}  (cast by %s)\n",
                          bg, lst->member, lst->cast_memb);
                 else
                    PRINT(f0, "%s%s;  (cast by %s)\n",
                          bg, lst->member, lst->cast_memb);};
             SC_strncpy(bg, 80, "          ", -1);};};

    PRINT(f0, "Size in bytes: %ld\n", dp->size);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_PRINT_DEFSTR - print a defstr in human readable form
 *
 * #bind PD_print_defstr fortran() scheme() python()
 */

void PD_print_defstr(defstr *dp)
   {

    PD_write_defstr(stdout, dp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_TEST_RECURSION - test if this is a recursive definition */

static int _PD_test_recursion(const char *type, const char *mtype)
   {int irec;
    char *dtype;

    if (_PD_indirection(mtype))
       {dtype = PD_dereference(CSTRSAVE(mtype));
        if (strcmp(dtype, type) == 0)
           irec = TRUE;
        else
           irec = FALSE;

        CFREE(dtype);}
    else
       irec = FALSE;

    return(irec);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PRKIND_INT_CHAR - print an entry of KIND_CHAR */

static void _PD_print_char_kind(PD_printdes *prnt, const char *vr, inti ni,
				const char *type, int quo, int idx,
				int n, long *ind)
   {int max1, max2;
    inti i, offset;
    char bf[MAXLINE];
    char *cp, *prefix, *before, *after;
    const char *nodename;
    FILE *f0;

    offset   = prnt->offset;
    prefix   = prnt->prefix;
    before   = prnt->before;
    after    = prnt->after;
    nodename = prnt->nodename;
    f0       = prnt->fp;

    cp = (char *) vr;

    if (idx == G_STRING_I)
       {if ((ni == 1L) && (offset == 0L))
	   {if (PD_gs.print_ctrl[5] == 0)
	       {PRINT(f0, "%s%s%s = %c\n",
		      prefix, before, nodename, *cp);}
	    else if (PD_gs.print_ctrl[5] == 1)
	       {PRINT(f0, "%s%s = %c\n",
		      before, nodename, *cp);}
	    else
	       {PRINT(f0, "        %c", *cp);};}

        else
	   {max1 = MAXLINE - 7 - strlen(prefix) -
	           strlen(before) - strlen(nodename);
	    max2 = max1 + strlen(before) - strlen(after);

	    i = min(ni, max1);
	    ni -= i;
	    SC_strncpy(bf, MAXLINE, cp, i);

	    cp += i;

/* with quotes */
	    if (quo == TRUE)
	       {if (PD_gs.print_ctrl[5] == 0)
		   {PRINT(f0, "%s%s%s = \"%s\"\n",
			  prefix, before, nodename, bf);}
	        else if (PD_gs.print_ctrl[5] == 1)
		   {PRINT(f0, "%s%s = \"%s\"\n",
			  before, nodename, bf);}
		else
		   {PRINT(f0, "        \"%s\"\n", bf);};

		while (ni > 0L)
		   {i = min(ni, max2);
		    ni -= i;
		    SC_strncpy(bf, MAXLINE, cp, i);

		    cp += i;
		    PRINT(f0, "%s%s%s = \"%s\"\n", prefix, after,
			  nodename, bf);};}

/* without quotes */
	    else
	       {if (PD_gs.print_ctrl[5] == 0)
		   {PRINT(f0, "%s%s%s = %s\n",
			  prefix, before, nodename, bf);}
	        else if (PD_gs.print_ctrl[5] == 1)
		   {PRINT(f0, "%s%s = %s\n",
			  before, nodename, bf);}
		else
		   {PRINT(f0, "        %s\n", bf);};

		while (ni > 0L)
		   {i = min(ni, max2);
		    ni -= i;
		    SC_strncpy(bf, MAXLINE, cp, i);

		    cp += i;
		    PRINT(f0, "%s%s%s = %s\n", prefix, after,
			  nodename, bf);};};};}

    else
       _PD_disp_data(prnt, vr, ni, idx, n, ind);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_IO_PRINT - display a primitive type
 *              - use N and IND to control printing of values at specific 
 *              - indices
 */

static int _PD_io_print(PD_printdes *prnt, const PDBfile *file,
			const char *vr,
			inti ni, const char *type, int n, long *ind)
   {int k, m, idx, nn, isz, status, quo;
    int id, ifx, ifp, icx;
    inti i, j, offset;
    char bf[MAXLINE], s[MAXLINE];
    char *t;
    defstr *pd;
    FILE *f0;
    char *prefix, *before;
    const char *nodename;

    prefix   = prnt->prefix;
    before   = prnt->before;
    nodename = prnt->nodename;
    f0       = prnt->fp;

    status = 0;
    offset = 1L;

    nn = 0;
    j  = 0L;
    k  = 0;
    for (i = 0L; i < ni; i++, j += offset, k++)
        {if (k > PD_gs.print_ctrl[4])
            {t  = PD_index_to_expr(bf, j,
				   prnt->dims,
				   prnt->mjr,
				   prnt->def_off);
             m  = (t != NULL) ? strlen(t) : 0;
             nn = max(nn, m);
             k  = 0;};};
    nn += 7;

    memset(s, ' ', LINE_SIZE);

    pd = PD_inquire_host_type(file, type);
    if (pd == NULL)
       return(status);

    isz = pd->size;
    id  = SC_type_id(type, FALSE);

    prnt->nn     = nn;
    prnt->offset = offset;

    if (pd->fp.format != NULL)
       {if ((ifp = SC_fp_type_id(type)) != -1)
	   _PD_disp_data(prnt, vr, ni, ifp, n, ind);

	else if ((icx = SC_cx_type_id(type)) != -1)
	   _PD_disp_data(prnt, vr, ni, icx, n, ind);}

/* only non-floating point types remain
 * user defined integral primitive types
 * can go off the byte size
 */
    else if (pd->convert >= 0)
       {if ((ifx = SC_fix_type_id(type, FALSE)) != -1)
	   _PD_disp_data(prnt, vr, ni, ifx, n, ind);

	else if (id == G_BOOL_I)
	  _PD_disp_data(prnt, vr, ni, G_BOOL_I, n, ind);

	else if ((pd->kind == KIND_CHAR) || (isz == sizeof(char)))
	   {if (id == G_CHAR_I)
	       {idx = G_STRING_I;
	        quo = TRUE;}
	    else if (pd->kind == KIND_CHAR)
	       {idx = G_STRING_I;
		ni  = isz;
	        quo = FALSE;}
	    else
	       {idx = G_INT_I;
	        quo = FALSE;};

	    _PD_print_char_kind(prnt, vr, ni, type, quo, idx, n, ind);}

	else if (strcmp(type, "function") == 0)
	   {if (PD_gs.print_ctrl[5] == 0)
	       {PRINT(f0, "%s%s%s = <function>\n", prefix, before, nodename);}
	    else if (PD_gs.print_ctrl[5] == 1)
	       {PRINT(f0, "%s%s = <function>\n", before, nodename);}
	    else
	       {PRINT(f0, "        <function>\n");};}

        else if (SC_type_match_size(KIND_INT, isz) != G_UNKNOWN_I)
	   _PD_disp_data(prnt, vr, ni, G_BIT_I, n, ind);

	else
	   {PRINT(f0, "%s%s%s = ", prefix, before, nodename);
	    PRINT(f0, "<type %s unprintable>\n", type);};}
    else
       {PRINT(f0, "%s%s%s = ", prefix, before, nodename);
        PRINT(f0, "<type %s unprintable>\n", type);};

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRITE_ENTRY - write a data item from a PDB file in a formated way
 *
 * #bind PD_write_entry fortran() scheme() python()
 */

int PD_write_entry(FILE *f0, const PDBfile *file ARG(,,cls),
		   const char *name, const void *vr,
		   syment *ep, int n, long *ind)
   {int status;
    char prefix[80], pathname[MAXLINE];
    char before[2], after[2];
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    switch (SETJMP(pa->print_err))
       {case ABORT :
	     return(-1);
        case ERR_FREE :
	     return(0);
        default :
	     memset(PD_gs.err, 0, MAXLINE);
	     break;};

     status = 0;

    if (ep == NULL)
       ep = PD_inquire_entry(file, name, FALSE, NULL);

    _PD_set_digits(file);
    _SC_set_format_defaults();
    _SC_set_user_formats();

    SC_strncpy(pathname, MAXLINE, name, -1);
    *prefix = '\0';
    *before = '\0';
    *after  = '\0';

    status = _PD_print_data(f0,
			    prefix, before, after, pathname, file, vr,
			    PD_entry_number(ep), PD_entry_type(ep),
			    PD_entry_dimensions(ep),
			    file->major_order, file->default_offset, 0,
			    n, ind);

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_PRINT_ENTRY - print a data item from a PDB file in a formated way
 *
 * #bind PD_print_entry fortran() scheme() python()
 */

int PD_print_entry(const PDBfile *file ARG(,,cls),
		   const char *name, const void *vr, syment *ep)
   {int rv;

    rv = PD_write_entry(stdout, file, name, vr, ep, 0, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PRINT_INDIRECTION - handle the indirects for the print process */

static int _PD_print_indirection(PD_printdes *prnt, const PDBfile *file,
				 char **vr, inti ni, char *type,
                                 int irecursion, int n, long *ind)
   {int min_index, status, def_off;
    inti i, ditems, nc, nr;
    char field[80], bf[MAXLINE];
    char *dtype, *s;
    char *prefix, *before, *after;
    const char *nodename;
    dimdes *dims;
    FILE *f0;

    prefix   = prnt->prefix;
    before   = prnt->before;
    after    = prnt->after;
    nodename = prnt->nodename;
    def_off  = prnt->def_off;
    dims     = prnt->dims;
    f0       = prnt->fp;

    status = 0;

    dtype = PD_dereference(CSTRSAVE(type));

    SC_strncpy(field, 80, nodename, -1);
    nc = strlen(field);
    s  = field + nc;
    nr = 80 - nc;

    if (dims != NULL)
       min_index = dims->index_min;
    else
       min_index = def_off;

    for (i = 0L; i < ni; i++, vr++)
        {if (ni > 1)
            snprintf(s, nr, "(%s)", SC_itos(NULL, 0, i + min_index, NULL));

         ditems = _PD_number_refd(NULL, NULL, NULL, DEREF(vr),
				  dtype, file->host_chart);

         if (ditems == -2L)
            {snprintf(bf, MAXLINE,
                      "UNKNOWN TYPE %s - _PD_PRINT_INDIRECTION",
                     dtype);
             status = -1;
             PD_error(bf, PD_PRINT);};

/* if the type is an indirection, follow the pointer */
         if (ditems > 0)
	    {prnt->nodename = field;
	     prnt->before   = before;
	     if (_PD_indirection(dtype))
	        status &= _PD_print_indirection(prnt, file,
						(char **) DEREF(vr),
						ditems, dtype, 
						irecursion, n, ind);
             else
                status &= _PD_print_leaf(prnt, file, DEREF(vr),
					 ditems, dtype, irecursion, n, ind);}
         else
	    {if (PD_gs.print_ctrl[5] == 0)
	        {PRINT(f0, "%s%s%s = (nil)\n", prefix, before, field);}
	     else if (PD_gs.print_ctrl[5] == 1)
	        {PRINT(f0, "%s%s = (nil)\n", before, field);}
	     else
	        {PRINT(f0, "        (nil)\n");};};

         before = after;};

    CFREE(dtype);

    prnt->nodename = NULL;

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PRINT_DATA - print out variables in a nicely formatted way  */

static int _PD_print_data(FILE *f0, char *prefix, char *before, char *after, 
                          char *nodename, const PDBfile *file,
			  const void *vr, inti ni,
                          char *type, dimdes *dims, int mjr, int def_off, 
                          int irecursion, int n, long *ind)
   {int status;
    PD_printdes prnt;

    prnt.mjr      = mjr;
    prnt.def_off  = def_off;
    prnt.prefix   = prefix;
    prnt.before   = before;
    prnt.after    = after;
    prnt.nodename = nodename;
    prnt.dims     = dims;
    prnt.fp       = f0;
    prnt.nn       = 0;
    prnt.offset   = 0;

    status = 0;

/* if the type is an indirection, follow the pointer */
    if (_PD_indirection(type))
       status = _PD_print_indirection(&prnt, file, (char **) vr, ni, type, 
				      irecursion, n, ind);
    else
       status = _PD_print_leaf(&prnt, file, vr, ni, type,
			       irecursion, n, ind);

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PRINT_MEMBER - print the specified member of a struct */

static int _PD_print_member(FILE *f0, char *prefix,
			    char *mbefore, char *mafter,
			    const PDBfile *file,
			    const char *svr, const char *type,
			    memdes *desc, char *mfield, long nb,
			    int mjr, int def_off, 
			    int *pirec, int n, long *ind)
   {int irecursion, status;
    long mitems, nc, nr;
    char *mtype, *s1;
    const char *mvr;
    defstr *ldp;
    dimdes *mdims;
    static char blank[2] = "";

    mitems = desc->number;
    mdims  = desc->dimensions;

    mvr = svr + desc->member_offs;

    PD_CAST_TYPE(mtype, desc, mvr, svr,
		 PD_error, "BAD CAST - _PD_PRINT_MEMBER", PD_PRINT);

    irecursion = *pirec;

/* handle typedefs */
    ldp = (defstr *) SC_hasharr_def_lookup(file->chart, mtype);

    if ((ldp != NULL) && (strcmp(ldp->type, mtype) != 0))
       mtype = ldp->type;

    if (PD_gs.print_ctrl[2] == 0)
       status = _PD_print_data(f0, prefix, mbefore, mafter, mfield,
			       file, mvr, mitems,
			       mtype, mdims, mjr, def_off, 0,
			       n, ind);
    else
       {if (_PD_test_recursion(type, mtype))
	   {nc = strlen(mfield);
	    s1 = mfield + nc;
	    nr = nb - nc;
	    snprintf(s1, nr, "<%d>", irecursion);
	    status = _PD_print_data(f0, prefix, mbefore, blank, mfield,
				    file, mvr, mitems,
				    mtype, mdims,
				    mjr, def_off, ++irecursion,
				    n, ind);}
        else
	   status = _PD_print_data(f0, prefix, mbefore, mafter, mfield,
				   file, mvr, mitems,
				   mtype, mdims,
				   mjr, def_off, 0,
				   n, ind);};

    *pirec = irecursion;

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PRINT_LEAF - if 'type' is a primitive type, display the data,
 *                - otherwise, lookup the type, and display each member.
 */

int _PD_print_leaf(PD_printdes *prnt, const PDBfile *file,
		   const char *vr, inti ni,
		   const char *type, int irecursion, int n, long *ind)
   {int mjr, def_off;
    int size, min_index, nchars, status;
    inti ii;
    char field[80], mfield[80];
    char *prefix, *before, *after;
    char *s, *s2, *sm, *mbefore, *mafter;
    const char *nodename, *svr;
    defstr *defp;
    dimdes *dims;
    memdes *desc, *mem_lst, *next;
    FILE *f0;
    static char before_2[5] = "|__ ";
    static char after_2[5]  = "|   ";
    static char spaces[5]   = "    ";

    prefix   = prnt->prefix;
    before   = prnt->before;
    after    = prnt->after;
    nodename = prnt->nodename;
    mjr      = prnt->mjr;
    def_off  = prnt->def_off;
    dims     = prnt->dims;
    f0       = prnt->fp;

    status  = 0;
    mem_lst = NULL;
 
/* print out the type */
    defp = PD_inquire_host_type(file, type);  

    if (defp == NULL)
       PD_error("VARIABLE NOT IN STRUCTURE CHART - _PD_PRINT_LEAF",
                PD_PRINT);
    else
       mem_lst = defp->members;

    if (mem_lst == NULL)
       status &= _PD_io_print(prnt, file, vr, ni, type, n, ind);
    else
       {s2     = CSTRSAVE(prefix);
        nchars = strlen(prefix) + strlen(before) + 1;

        if (nchars > 80)
           {s = prefix + strlen(prefix) - (nchars - 80) - 3;
            strcpy(s, "...");}

        s = prefix + strlen(prefix);     /* save end of prefix */
        strcpy(s, before);

        if (PD_gs.write != NULL)
           mem_lst = PD_gs.write(file, vr, defp);

        size = defp->size;

        for (svr = vr, ii = 0L; ii < ni; ii++)
            {if ((ni > 1) || (dims != NULL))
                {if (dims != NULL)
                    min_index = dims->index_min;
                 else
                    min_index = def_off;

                 snprintf(field, 80, "%s(%s)",
			  nodename, SC_itos(NULL, 0, ii + min_index, NULL));}
             else
                SC_strncpy(field, 80, nodename, -1);

/* compute prefix */
	     mbefore = NULL;
             switch (PD_gs.print_ctrl[0])
                {case 0 :
		      SC_strcat(field, 80, ".");
		      mbefore = field;
		      mafter  = field;
		      break;
                 case 1 :
		      mbefore = spaces;
		      mafter  = spaces;
		      PRINT(f0, "%s%s\n", prefix, field);
		      break;
                 case 2 :
		      mbefore = before_2;
		      mafter  = after_2;
		      if (ii > 0L)
			 PRINT(f0, "%s%s\n", prefix, mafter);
		      PRINT(f0, "%s%s\n", prefix, field);
		      if (irecursion > 0)
			 {PRINT(f0, "%s ___|\n", s2);
			  PRINT(f0, "%s%s\n", s2, mafter);};
		      break;};

             strcpy(s, after);

	     sm = _SC.types.suppress_member;

             for (desc = mem_lst; desc != NULL; desc = desc->next)
                 {if (PD_gs.print_ctrl[1] == 0)
                     SC_strncpy(mfield, 80, desc->name, -1);
                  else
                     SC_strncpy(mfield, 80, desc->member, -1);

		  if ((sm == NULL) ||
		      ((sm != NULL) && (SC_regx_match(mfield, sm) == FALSE)))
		     status &= _PD_print_member(f0, prefix, mbefore, mafter,
						file, svr, type, desc,
						mfield, 80,
						mjr, def_off,
						&irecursion, n, ind);

                  next = desc->next;
                  if (next != NULL)
                     if ((PD_gs.print_ctrl[0] == 2) && (next->next == NULL))
                         mafter = spaces;};

             svr += size;};

        CFREE(s2);
        *s = '\0';};

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SET_DIGITS - set the decimal printing precision parameters */

void _PD_set_digits(const PDBfile *file)
   {int i, d, dig;
    int *fix_pre;
    long *f;
    double lg2;
    data_standard *std;
    precisionfp *fp_pre;

    lg2 = log10(2.0);

    fix_pre = _SC.types.fix_precision;
    fp_pre  = _SC.types.fp_precision;
    dig     = _SC.types.max_digits;

    std = file->std;

    for (i = 0; i < N_PRIMITIVE_FIX; i++)
        {if (std->fx[i].bpi < 0)
	    d = -std->fx[i].bpi;
	 else
	    d = 8*std->fx[i].bpi;
	 fix_pre[i] = lg2*d + 1;};

    for (i = 0; i < N_PRIMITIVE_FP; i++)
        {f = std->fp[i].format;
	 if (f != NULL)
	    d = min(f[2], dig);
	 else
	    d = dig;
	 fp_pre[i].tolerance = POWL(2.0L, -((long double) d));
	 fp_pre[i].digits    = lg2*d + 1;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DIGITS_TOL - compute the number of decimal digits for printing and
 *                - the comparison tolerances for floating point numbers
 */

void _PD_digits_tol(const PDBfile *file_a, const PDBfile *file_b)
   {int i, nmb, da, db, dig;
    int *fix_pre;
    long *fa, *fb;
    double lg2;
    data_standard *stda, *stdb;
    precisionfp *fp_pre;

    lg2 = log10(2.0);

    fix_pre = _SC.types.fix_precision;
    fp_pre  = _SC.types.fp_precision;
    dig     = _SC.types.max_digits;

    stda = file_a->std;
    stdb = file_b->std;

    for (i = 0; i < N_PRIMITIVE_FIX; i++)
        {if (stda->fx[i].bpi < 0)
	    da = -stda->fx[i].bpi;
	 else
	    da = 8*stda->fx[i].bpi;
	 if (stdb->fx[i].bpi < 0)
	    db = -stdb->fx[i].bpi;
	 else
	    db = 8*stdb->fx[i].bpi;
	 nmb = max(da, db);
	 fix_pre[i] = lg2*nmb + 1;};

    for (i = 0; i < N_PRIMITIVE_FP; i++)
        {fa  = stda->fp[i].format;
	 fb  = stdb->fp[i].format;
	 nmb = max(fa[2], fb[2]);
	 nmb = min(nmb, dig);
	 fp_pre[i].tolerance = POWL(2.0L, -((long double) nmb));
	 fp_pre[i].digits    = lg2*nmb + 1;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
