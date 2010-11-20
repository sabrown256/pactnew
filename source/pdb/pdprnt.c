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

int
 PD_tolerance = 1000;

int
 PD_fix_precision[PD_N_PRIMITIVE_FIX];

precisionfp
 PD_fp_precision[PD_N_PRIMITIVE_FP];

/* print controls
 *  0  -  print prefix: 0 = full path, 1 = space, 2 = tree
 *  1  -  0 = print name, 1 = print type and name
 *  2  -  recursion: 0 = yes, 1 = count
 *  3  -  number of items before going to array mode of display
 *  4  -  number of items per line
 *  5  -  print variable label: 0 = full, 1 = partial, 2 = nothing
 */

long
 PD_print_controls[10] = {0L, 0L, 0L, 20L, 2L, 0L, 0L, 0L, 0L, 0L};

char
 *PD_no_print_member = NULL,
 *PD_user_formats1[PD_N_PRIMITIVES],
 *PD_user_formats2[PD_N_PRIMITIVES];

static char
 *_PD_type_names[] = { "char", "bit",
		       "short", "int", "long", "long_long",
		       "float", "double", "long_double",
		       "float_complex", "double_complex",
		       "long_double_complex" };

static int
 _PD_print_data(FILE *f0,
		char *prefix, char *before, char *after,
		char *nodename, PDBfile *file, void *vr,
		long ni, char *type, dimdes *dims,
		int mjr, int def_off, int irecursion,
		int n, long *ind),
 _PD_print_indirection(PD_printdes *prnt, PDBfile *file, char **vr,
		       long ni, char *type, int irecursion,
		       int n, long *ind);

static int
 _PD_test_recursion(char *type, char *mtype);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DISP_MODE_I - print a single item from array X */

static void _PD_disp_mode_i(PD_printdes *prnt, void *x, int tid,
			    long n, long *ind)
   {long i, j;
    char bf[MAXLINE];
    FILE *fp;

    fp = prnt->fp;

    for (i = 0L; i < n; i++)
        {j = ind[i];
         if (PD_print_controls[5] == 0)
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

/* _PD_DISP_MODE_1 - print a single item from scalar X according to formats1 */

static void _PD_disp_mode_1(PD_printdes *prnt, void *x, int tid)
   {char bf[MAXLINE];
    FILE *fp;

    fp = prnt->fp;

    if (PD_print_controls[5] == 0)
       {PRINT(fp, "%s%s%s = ", prnt->prefix, prnt->before, prnt->nodename);}
    else if (PD_print_controls[5] == 1)
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

static void _PD_disp_mode_2(PD_printdes *prnt, void *x, long ni, int tid)
   {long i, j;
    char bf[MAXLINE];
    FILE *fp;

    fp = prnt->fp;

    i = 0L;
    j = 0L;
    if (PD_print_controls[5] == 0)
       {PRINT(fp, "%s%s%s(%s) = ",
              prnt->prefix, prnt->before, prnt->nodename,
              PD_index_to_expr(bf, j,
			       prnt->dims, prnt->mjr, prnt->def_off));}
    else if (PD_print_controls[5] == 1)
       {PRINT(fp, "%s%s ", prnt->before, prnt->nodename);}
    else
       {PRINT(fp, "        ");};

    io_puts(SC_ntos(bf, MAXLINE, tid, x, i, 1), fp);

    PRINT(fp, "\n");

    j += prnt->offset;
    for (i = 1L; i < ni; i++, j += prnt->offset)
        {if (PD_print_controls[5] == 0)
            {PRINT(fp, "%s%s%s(%s) = ",
                   prnt->prefix, prnt->after, prnt->nodename,
                   PD_index_to_expr(bf, j,
				    prnt->dims, prnt->mjr, prnt->def_off));}
         else if (PD_print_controls[5] == 1)
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

static void _PD_disp_mode_3(PD_printdes *prnt, void *x, long ni, int tid)
   {int nn;
    long i, j, k;
    char bf[MAXLINE], s[MAXLINE];
    FILE *fp;

    fp = prnt->fp;
    nn = prnt->nn;

    i = 0L;
    j = 0L;
    if (PD_print_controls[5] == 0)
       {PRINT(fp, "%s%s%s\n", prnt->prefix, prnt->before, prnt->nodename);
        sprintf(s, "  (%s)                              ",
		PD_index_to_expr(bf, j,
				 prnt->dims, prnt->mjr, prnt->def_off));
	s[nn] = '\0';
        PRINT(fp, "%s", s);}
    else
       {PRINT(fp, "        ");};

    for (k = 0; i < ni; i++, j += prnt->offset, k++)
        {if (k >= PD_print_controls[4])
            {if (PD_print_controls[5] == 0)
                {sprintf(s, "  (%s)                              ",
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

static void _PD_disp_data(PD_printdes *prnt, void *x,
			  long ni, int tid, long n, long *ind)
   {

    if ((n > 0L) && (ind != NULL))
       _PD_disp_mode_i(prnt, x, tid, n, ind);

    else if (ni == 1L)
       _PD_disp_mode_1(prnt, x, tid);

    else if (ni < PD_print_controls[3])
       _PD_disp_mode_2(prnt, x, ni, tid);

    else
       _PD_disp_mode_3(prnt, x, ni, tid);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRITE_EXTRAS - write the extra stuff about a PDBfile */

void PD_write_extras(FILE *f0, PDBfile *file)
   {long i;
    char *date;
    defstr *dp;

    if (_PD.maxfsize == 0)
       SC_fix_lmt(sizeof(BIGINT), NULL, &_PD.maxfsize, NULL);

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

    if (file->virtual_internal)
       {PRINT(f0, "Symbol Table Address: 0x%lx\n", file->symtab);
	PRINT(f0, "Structure Chart Address: 0x%lx\n", file->chart);}
    else
       {PRINT(f0, "Header Address: %lld\n", (BIGINT) file->headaddr);
	PRINT(f0, "Symbol Table Address: %lld\n", (BIGINT) file->symtaddr);
	PRINT(f0, "Structure Chart Address: %lld\n", (BIGINT) file->chrtaddr);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_PRINT_EXTRAS - print the extra stuff about a PDBfile */

void PD_print_extras(PDBfile *file)
   {

    PD_write_extras(stdout, file);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRITE_SYMENT - write a symbol table entry in human readable form */

void PD_write_syment(FILE *f0, syment *ep)
   {dimdes *dim;

    PRINT(f0, "Type: %s\n", PD_entry_type(ep));
    if (PD_entry_dimensions(ep) != NULL)
       {PRINT(f0, "Dimensions: (");

        for (dim = PD_entry_dimensions(ep); dim != NULL; dim = dim->next)
            {PRINT(f0, "%ld:%ld", dim->index_min, dim->index_max);
	     if (dim->next != NULL)
	        PRINT(f0, ", ");};

	PRINT(f0, ")\n");};

    PRINT(f0, "Length: %ld\n", PD_entry_number(ep));
    PRINT(f0, "Address: %lld\n", (BIGINT) PD_entry_address(ep));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_PRINT_SYMENT - print a symbol table entry in human readable form */

void PD_print_syment(syment *ep)
   {

    PD_write_syment(stdout, ep);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRITE_DEFSTR - write a defstr in human readable form */

void PD_write_defstr(FILE *f0, defstr *dp)
   {memdes *lst, *nxt;
    char bg[80];

    strcpy(bg, "Members: {");

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
             strcpy(bg, "          ");};};

    PRINT(f0, "Size in bytes: %ld\n", dp->size);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_PRINT_DEFSTR - print a defstr in human readable form */

void PD_print_defstr(defstr *dp)
   {

    PD_write_defstr(stdout, dp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_TEST_RECURSION - test if this is a recursive definition */

static int _PD_test_recursion(char *type, char *mtype)
   {int irec;
    char *dtype;

    if (_PD_indirection(mtype))
       {dtype = PD_dereference(SC_strsavef(mtype,
                               "char*:_PD_TEST_RECURSION:dtype"));
        if (strcmp(dtype, type) == 0)
           irec = TRUE;
        else
           irec = FALSE;

        SFREE(dtype);}
    else
       irec = FALSE;

    return(irec);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PRINT_CHAR_KIND - print an entry of CHAR_KIND */

static void _PD_print_char_kind(PD_printdes *prnt, char *vr, long ni,
				char *type, int quo, int idx,
				int n, long *ind)
   {int max1, max2;
    int mjr, nn, def_off;
    long i, offset;
    char bf[MAXLINE];
    char *cp, *prefix, *before, *after, *nodename;
    dimdes *dims;
    FILE *f0;

    mjr      = prnt->mjr;
    nn       = prnt->nn;
    def_off  = prnt->def_off;
    offset   = prnt->offset;
    prefix   = prnt->prefix;
    before   = prnt->before;
    after    = prnt->after;
    nodename = prnt->nodename;
    dims     = prnt->dims;
    f0       = prnt->fp;

    cp = (char *) vr;

    if (idx == SC_STRING_I)
       {if ((ni == 1L) && (offset == 0L))
	   {if (PD_print_controls[5] == 0)
	       {PRINT(f0, "%s%s%s = %c\n",
		      prefix, before, nodename, *cp);}
	    else if (PD_print_controls[5] == 1)
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
	       {if (PD_print_controls[5] == 0)
		   {PRINT(f0, "%s%s%s = \"%s\"\n",
			  prefix, before, nodename, bf);}
	        else if (PD_print_controls[5] == 1)
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
	       {if (PD_print_controls[5] == 0)
		   {PRINT(f0, "%s%s%s = %s\n",
			  prefix, before, nodename, bf);}
	        else if (PD_print_controls[5] == 1)
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

static int _PD_io_print(PD_printdes *prnt, PDBfile *file, char *vr,
			long ni, char *type, int n, long *ind)
   {int k, m, idx, nn, isz, status, quo;
    int ifx, ifp, icx;
    long offset, i, j;
    char bf[MAXLINE], s[MAXLINE];
    char *cp, **t;
    defstr *pd;
    FILE *f0;
    char *prefix, *before, *after, *nodename;

    prefix   = prnt->prefix;
    before   = prnt->before;
    after    = prnt->after;
    nodename = prnt->nodename;
    f0       = prnt->fp;

    status = 0;
    offset = 1L;

    nn = 0;
    j  = 0L;
    k  = 0;
    for (i = 0L; i < ni; i++, j += offset, k++)
        {if (k > PD_print_controls[4])
            {m  = strlen(PD_index_to_expr(bf, j,
					  prnt->dims, prnt->mjr, prnt->def_off));
             nn = max(nn, m);
             k  = 0;};};
    nn += 7;

    memset(s, ' ', LINE_SIZE);

    pd  = PD_inquire_host_type(file, type);
    isz = pd->size;

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

/* GOTCHA: This is a feeble first step toward a generalized
 *         print capability for user defined types
 */
	else if (strcmp(type, SC_CHAR_8_S) == 0)
	   {if (SC_strstr(SC_print_formats[SC_INT_I], "%s") != NULL)
	       {cp = (char *) vr;
		t  = FMAKE_N(char *, ni, "_PD_IO_PRINT:t");
		for (i = 0; i < ni; i++, cp += isz)
		    {t[i] = FMAKE_N(char, isz + 1,
                                          "_PD_IO_PRINT:t[]");
		     memcpy(t[i], cp, isz);
		     t[i][isz] = '\0';};
		_PD_disp_data(prnt, t, ni, SC_BIT_I, n, ind);
		for (i = 0; i < ni; i++)
		    SFREE(t[i]);
		SFREE(t);}
	    else
	       _PD_disp_data(prnt, vr, isz*ni, 0, n, ind);}

	else if (strcmp(type, SC_BOOL_S) == 0)
	  _PD_disp_data(prnt, vr, ni, SC_BOOL_I, n, ind);

	else if ((pd->kind == CHAR_KIND) || (isz == sizeof(char)))
	   {if (strcmp(type, SC_CHAR_S) == 0)
	       {idx = SC_STRING_I;
	        quo = TRUE;}
	    else if (pd->kind == CHAR_KIND)
	       {idx = SC_STRING_I;
		ni  = isz;
	        quo = FALSE;}
	    else
	       {idx = SC_INT_I;
	        quo = FALSE;};

	    _PD_print_char_kind(prnt, vr, ni, type, quo, idx, n, ind);}

	else if (strcmp(type, "function") == 0)
	   {if (PD_print_controls[5] == 0)
	       {PRINT(f0, "%s%s%s = <function>\n", prefix, before, nodename);}
	    else if (PD_print_controls[5] == 1)
	       {PRINT(f0, "%s%s = <function>\n", before, nodename);}
	    else
	       {PRINT(f0, "        <function>\n");};}

        else if (isz == sizeof(short))
	  _PD_disp_data(prnt, vr, ni, SC_BIT_I, n, ind);

	else if (isz == sizeof(int))
	   _PD_disp_data(prnt, vr, ni, SC_BIT_I, n, ind);

	else if (isz == sizeof(long))
	   _PD_disp_data(prnt, vr, ni, SC_BIT_I, n, ind);

	else
	   {PRINT(f0, "%s%s%s = ", prefix, before, nodename);
	    PRINT(f0, "<type %s unprintable>\n", type);};}
    else
       {PRINT(f0, "%s%s%s = ", prefix, before, nodename);
        PRINT(f0, "<type %s unprintable>\n", type);};

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_WRITE_ENTRY - write a data item from a PDB file in a formated way */

int PD_write_entry(FILE *f0, PDBfile *file, char *name, void *vr,
		   syment *ep, int n, long *ind)
   {int status;
    char prefix[80], pathname[180];
    char before[2], after[2];
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    switch (SETJMP(pa->print_err))
       {case ABORT :
	     return(-1);
        case ERR_FREE :
	     return(0);
        default :
	     memset(PD_err, 0, MAXLINE);
	     break;};

     status = 0;

    if (ep == NULL)
       ep = PD_inquire_entry(file, name, FALSE, NULL);

    _PD_set_digits(file);
    _PD_set_format_defaults();
    _PD_set_user_formats();

    strcpy(pathname, name);
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

/* PD_PRINT_ENTRY - print a data item from a PDB file in a formated way */

int PD_print_entry(PDBfile *file, char *name, void *vr, syment *ep)
   {int rv;

    rv = PD_write_entry(stdout, file, name, vr, ep, 0, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PRINT_DATA - print out variables in a nicely formatted way  */

static int _PD_print_data(FILE *f0, char *prefix, char *before, char *after, 
                          char *nodename, PDBfile *file, void *vr, long ni,
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

/* _PD_PRINT_INDIRECTION - handle the indirects for the print process */

static int _PD_print_indirection(PD_printdes *prnt, PDBfile *file, char **vr,
				 long ni, char *type,
                                 int irecursion, int n, long *ind)
   {int min_index, status;
    int mjr, def_off;
    long i, ditems;
    char field[80], bf[MAXLINE];
    char *dtype, *s;
    char *prefix, *before, *after, *nodename;
    dimdes *dims;
    FILE *f0;

    prefix   = prnt->prefix;
    before   = prnt->before;
    after    = prnt->after;
    nodename = prnt->nodename;
    mjr      = prnt->mjr;
    def_off  = prnt->def_off;
    dims     = prnt->dims;
    f0       = prnt->fp;

    status = 0;

    dtype = PD_dereference(SC_strsavef(type,
				       "char*:_PD_PRINT_INDIRECTION:dtype"));

    strcpy(field, nodename);
    s = field + strlen(field);

    if (dims != NULL)
       min_index = dims->index_min;
    else
       min_index = def_off;

    for (i = 0L; i < ni; i++, vr++)
        {if (ni > 1)
            sprintf(s, "(%ld)", i + min_index);

         ditems = _PD_number_refd(DEREF(vr), dtype, file->host_chart);

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
	    {if (PD_print_controls[5] == 0)
	        {PRINT(f0, "%s%s%s = (nil)\n", prefix, before, field);}
	     else if (PD_print_controls[5] == 1)
	        {PRINT(f0, "%s%s = (nil)\n", before, field);}
	     else
	        {PRINT(f0, "        (nil)\n");};};

         before = after;};

    SFREE(dtype);

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PRINT_MEMBER - print the specified member of a struct */

static int _PD_print_member(FILE *f0, char *prefix,
			    char *mbefore, char *mafter,  
			    PDBfile *file, char *svr, char *type,
			    memdes *desc, char *mfield,
			    int mjr, int def_off, 
			    int *pirec, int n, long *ind)
   {int irecursion, status;
    long mitems;
    char *mvr, *mtype, *s1;
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

    if (PD_print_controls[2] == 0)
       status = _PD_print_data(f0, prefix, mbefore, mafter, mfield,
			       file, mvr, mitems,
			       mtype, mdims, mjr, def_off, 0,
			       n, ind);
    else
       {if (_PD_test_recursion(type, mtype))
	   {s1 = mfield + strlen(mfield);
	    sprintf(s1,"<%d>", irecursion);
	    status = _PD_print_data(f0, prefix, mbefore, blank, mfield,
				    file, mvr, mitems,
				    mtype, mdims,
				    mjr, def_off, ++irecursion,
				    n, ind);}
        else
	   {status = _PD_print_data(f0, prefix, mbefore, mafter, mfield,
				    file, mvr, mitems,
				    mtype, mdims,
				    mjr, def_off, 0,
				    n, ind);};};

    *pirec = irecursion;

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PRINT_LEAF - if 'type' is a primitive type, display the data,
 *                - otherwise, lookup the type, and display each member.
 */

int _PD_print_leaf(PD_printdes *prnt, PDBfile *file, char *vr, long ni,
		   char *type, int irecursion, int n, long *ind)
   {int mjr, def_off;
    int size, min_index, nchars, status;
    long ii;
    char field[80], mfield[80];
    char *prefix, *before, *after, *nodename;
    char *s, *s2, *svr;
    char *mbefore, *mafter;
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
       {s2     = SC_strsavef(prefix, "char*:_PD_PRINT_LEAF:s2");
        nchars = strlen(prefix) + strlen(before) + 1;

        if (nchars > 80)
           {s = prefix + strlen(prefix) - (nchars - 80) - 3;
            strcpy(s, "...");}

        s = prefix + strlen(prefix);     /* save end of prefix */
        strcpy(s, before);

        if (pdb_wr_hook != NULL)
           mem_lst = (*pdb_wr_hook)(file, vr, defp);

        size = defp->size;

        for (svr = vr, ii = 0L; ii < ni; ii++)
            {if ((ni > 1) || (dims != NULL))
                {if (dims != NULL)
                    min_index = dims->index_min;
                 else
                    min_index = def_off;

                 snprintf(field, 80, "%s(%ld)", nodename, ii + min_index);}
             else
                strcpy(field, nodename);

/* compute prefix */
	     mbefore = NULL;
             switch (PD_print_controls[0])
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

             for (desc = mem_lst; desc != NULL; desc = desc->next)
                 {if (PD_print_controls[1] == 0)
                     strcpy(mfield, desc->name);
                  else
                     strcpy(mfield, desc->member);

		  if ((PD_no_print_member == NULL) ||
		      ((PD_no_print_member != NULL) &&
		       (SC_regx_match(mfield, PD_no_print_member) == FALSE)))
		     status &= _PD_print_member(f0, prefix, mbefore, mafter,
						file, svr, type, desc, mfield,
						mjr, def_off,
						&irecursion, n, ind);

                  next = desc->next;
                  if (next != NULL)
                     if ((PD_print_controls[0] == 2) && (next->next == NULL))
                         mafter = spaces;};

             svr += size;};

        SFREE(s2);
        *s = '\0';};

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SET_USER_DEFAULTS - default user specified formats to NULL */

void _PD_set_user_defaults(void)
   {int i;

    for (i = 0; i < PD_N_PRIMITIVES; i++)
        {if (PD_user_formats1[i] != NULL)
	    {SFREE(PD_user_formats1[i]);};

	 if (PD_user_formats2[i] != NULL)
	    {SFREE(PD_user_formats2[i]);};};

    if (PD_no_print_member != NULL)
       {SFREE(PD_no_print_member);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SET_USER_FORMATS - replace edit formats with user specified formats  */

void _PD_set_user_formats(void)
   {int i;

    for (i = 0; i < PD_N_PRIMITIVES; i++)
        {if (PD_user_formats1[i] != NULL)
	    {SFREE(SC_print_formats[i]);
	     SC_print_formats[i] = SC_strsavef(PD_user_formats1[i],
						"char*:_PD_SET_USER_FORMATS:format1(i)");};

	 if (PD_user_formats2[i] != NULL)
	    {SFREE(SC_print_formata[i]);
	     SC_print_formata[i] = SC_strsavef(PD_user_formats2[i],
						"char*:_PD_SET_USER_FORMATS:format2(i)");};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SET_FORMAT_DEFAULTS - set the defaults for the edit formats 
 *                         -   1 = bit
 *                         -   2 = bool
 *                         -   3 = char
 *                         -   4 = short
 *                         -   5 = int
 *                         -   6 = long
 *                         -   7 = long long
 *                         -   8 = float
 *                         -   9 = double
 *                         -  10 = long double
 *                         -  11 = float complex
 *                         -  12 = double complex
 *                         -  13 = long double complex
 */

void _PD_set_format_defaults(void)
   {int i;
    char tmp[MAXLINE], *t;
    
/* SC_print_formats is used for scalars */

/* fixed point types (proper) */
    for (i = 0; i < PD_N_PRIMITIVE_FIX; i++)
        {if (SC_print_formats[i+SC_SHORT_I] != NULL)
	    SFREE(SC_print_formats[i+SC_SHORT_I]);

	 snprintf(tmp, MAXLINE, "%%%dd", PD_fix_precision[i]);

	 t = SC_strsavef(tmp, "char*:_PD_SET_FORMAT_DEFAULTS:format1(fix)");
	 SC_print_formats[i+SC_SHORT_I] = t;};

/* real floating point types (proper) */
    for (i = 0; i < PD_N_PRIMITIVE_FP; i++)
        {if (SC_print_formats[i+SC_FLOAT_I] != NULL)
	    SFREE(SC_print_formats[i+SC_FLOAT_I]);

	 snprintf(tmp, MAXLINE, "%%# .%de", PD_fp_precision[i].digits);

	 t = SC_strsavef(tmp, "char*:_PD_SET_FORMAT_DEFAULTS:format1(fp)");
	 SC_print_formats[i+SC_FLOAT_I] = t;};

/* complex floating point types (proper) */
    for (i = 0; i < PD_N_PRIMITIVE_FP; i++)
        {if (SC_print_formats[i+SC_FLOAT_COMPLEX_I] != NULL)
	    SFREE(SC_print_formats[i+SC_FLOAT_COMPLEX_I]);

	 snprintf(tmp, MAXLINE, "%%# .%de + %%# .%de*I",
		  PD_fp_precision[i].digits, PD_fp_precision[i].digits);

	 t = SC_strsavef(tmp, "char*:_PD_SET_FORMAT_DEFAULTS:format1(fp)");
	 SC_print_formats[i+SC_FLOAT_COMPLEX_I] = t;};

/* other primitive types */
    if (SC_print_formats[SC_CHAR_I] != NULL)
       SFREE(SC_print_formats[SC_CHAR_I]);

    t = SC_strsavef("%c", "char*:_PD_SET_FORMAT_DEFAULTS:format1(char)");
    SC_print_formats[SC_CHAR_I] = t;

    if (SC_print_formats[SC_BIT_I] != NULL)
       SFREE(SC_print_formats[SC_BIT_I]);

    t = SC_strsavef("%x", "char*:_PD_SET_FORMAT_DEFAULTS:format1(bit)");
    SC_print_formats[SC_BIT_I] = t;

    if (SC_print_formats[SC_BOOL_I] != NULL)
       SFREE(SC_print_formats[SC_BOOL_I]);

    t = SC_strsavef("%s", "char*:_PD_SET_FORMAT_DEFAULTS:format1(bool)");
    SC_print_formats[SC_BOOL_I] = t;

/* SC_print_formata is used for arrays */
    for (i = 0; i < PD_N_PRIMITIVES; i++)
        {if (SC_print_formata[i] != NULL)
            SFREE(SC_print_formata[i]);

         t = SC_strsavef(SC_print_formats[i],
			 "char*:_PD_SET_FORMAT_DEFAULTS:formats2");
         SC_print_formata[i] = t;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SET_DIGITS - set the decimal printing precision parameters */

void _PD_set_digits(PDBfile *file)
   {int i, d;
    long *f;
    double log2;
    data_standard *std;

    log2 = log10(2.0);

    std = file->std;

    for (i = 0; i < PD_N_PRIMITIVE_FIX; i++)
        {if (std->fx[i].bpi < 0)
	    d = -std->fx[i].bpi;
	 else
	    d = 8*std->fx[i].bpi;
	 PD_fix_precision[i] = log2*d + 1;};

    for (i = 0; i < PD_N_PRIMITIVE_FP; i++)
        {f = std->fp[i].format;
	 d = min(f[2], PD_tolerance);
	 PD_fp_precision[i].tolerance = POWL(2.0L, -((long double) d));
	 PD_fp_precision[i].digits    = log2*d + 1;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_DIGITS_TOL - compute the number of decimal digits for printing and
 *                - the comparison tolerances for floating point numbers
 */

void _PD_digits_tol(PDBfile *file_a, PDBfile *file_b)
   {int i, nmb, da, db;
    long *fa, *fb;
    double log2;
    data_standard *stda, *stdb;

    log2 = log10(2.0);

    stda = file_a->std;
    stdb = file_b->std;

    for (i = 0; i < PD_N_PRIMITIVE_FIX; i++)
        {if (stda->fx[i].bpi < 0)
	    da = -stda->fx[i].bpi;
	 else
	    da = 8*stda->fx[i].bpi;
	 if (stdb->fx[i].bpi < 0)
	    db = -stdb->fx[i].bpi;
	 else
	    db = 8*stdb->fx[i].bpi;
	 nmb = max(da, db);
	 PD_fix_precision[i] = log2*nmb + 1;};

    for (i = 0; i < PD_N_PRIMITIVE_FP; i++)
        {fa  = stda->fp[i].format;
	 fb  = stdb->fp[i].format;
	 nmb = max(fa[2], fb[2]);
	 nmb = min(nmb, PD_tolerance);
	 PD_fp_precision[i].tolerance = POWL(2.0L, -((long double) nmb));
	 PD_fp_precision[i].digits    = log2*nmb + 1;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
