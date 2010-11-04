/*
 * PDPDBW.C - PDB write functionality in PD
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

#define LINE_SIZE 90

/*--------------------------------------------------------------------------*/

#define DISP_DATA(f0, x, nitems, tid, n, ind)                                \
    if ((n > 0L) && (ind != NULL))                                           \
       {DISP_MODE_I(f0, x, tid, n, ind);}                                    \
    else if (nitems == 1L)                                                   \
       {DISP_MODE_1(f0, x, nitems, tid);}                                    \
    else if (nitems < PD_print_controls[3])                                  \
       {DISP_MODE_2(f0, x, nitems, tid);}                                    \
    else                                                                     \
       {DISP_MODE_3(f0, x, nitems, tid);}

/*--------------------------------------------------------------------------*/

#define DISP_MODE_I(f0, x, tid, n, ind)                                      \
    for (i = 0L; i < n; i++)                                                 \
        {j = ind[i];                                                         \
         if (PD_print_controls[5] == 0)                                      \
	    {PRINT(f0, "%s%s%s(%s) = ",                                      \
                   prefix, after, nodename,                                  \
                   PD_index_to_expr(bf, j, dims, mjr, def_off));}            \
         else                                                                \
	    {PRINT(f0, "\na> %s", prefix);};                                    \
         PRINT(f0, PD_print_formats1[tid], x[j]);                            \
         PRINT(f0, "\n");}

/*--------------------------------------------------------------------------*/

#define DISP_MODE_1(f0, x, nitems, tid)                                      \
    if (PD_print_controls[5] == 0)                                           \
       {PRINT(f0, "%s%s%s = ", prefix, before, nodename);}                   \
    else if (PD_print_controls[5] == 1)                                      \
       {PRINT(f0, "%s%s", before, nodename);}                                \
    else                                                                     \
       {PRINT(f0, "        ");};                                             \
    PRINT(f0, PD_print_formats1[tid], *x);                                   \
    PRINT(f0, "\n");

/*--------------------------------------------------------------------------*/

#define DISP_MODE_2(f0, x, nitems, tid)                                      \
    i = 0L;                                                                  \
    j = 0L;                                                                  \
    if (PD_print_controls[5] == 0)                                           \
       {PRINT(f0, "%s%s%s(%s) = ",                                           \
              prefix, before, nodename,                                      \
              PD_index_to_expr(bf, j, dims, mjr, def_off));}                 \
    else if (PD_print_controls[5] == 1)                                      \
       {PRINT(f0, "%s%s ", before, nodename);}                               \
    else                                                                     \
       {PRINT(f0, "        ");};                                             \
    PRINT(f0, PD_print_formats1[tid], x[i]);                                 \
    PRINT(f0, "\n");                                                         \
    j += offset;                                                             \
    for (i = 1L; i < nitems; i++, j += offset)                               \
        {if (PD_print_controls[5] == 0)                                      \
            {PRINT(f0, "%s%s%s(%s) = ",                                      \
                   prefix, after, nodename,                                  \
                   PD_index_to_expr(bf, j, dims, mjr, def_off));}            \
         else if (PD_print_controls[5] == 1)                                 \
           {PRINT(f0, "%s%s ", before, nodename);}                           \
        else                                                                 \
           {PRINT(f0, "        ");};                                         \
         PRINT(f0, PD_print_formats1[tid], x[i]);                            \
         PRINT(f0, "\n");}

/*--------------------------------------------------------------------------*/

#define DISP_MODE_3(f0, x, nitems, tid)                                      \
    i = 0L;                                                                  \
    j = 0L;                                                                  \
    if (PD_print_controls[5] == 0)                                           \
       {PRINT(f0, "%s%s%s\n", prefix, before, nodename);                     \
        sprintf(s, "  (%s)                              ",                   \
		PD_index_to_expr(bf, j, dims, mjr, def_off));                \
	s[nn] = '\0';                                                        \
        PRINT(f0, "%s", s);}                                                 \
    else                                                                     \
       {PRINT(f0, "        ");};                                             \
    for (k = 0; i < nitems; i++, j += offset, k++)                           \
        {if (k >= PD_print_controls[4])                                      \
            {if (PD_print_controls[5] == 0)                                  \
                {sprintf(s, "  (%s)                              ",          \
                         PD_index_to_expr(bf, j, dims, mjr, def_off));       \
		 s[nn] = '\0';                                               \
                 PRINT(f0, "\n%s", s);}                                      \
	     else                                                            \
	        {PRINT(f0, "\n        ");};                                  \
	     k = 0;};                                                        \
         PRINT(f0, PD_print_formats2[tid], x[i]);};                          \
    PRINT(f0, "\n")

/*--------------------------------------------------------------------------*/

int
 PD_short_digits,
 PD_int_digits,
 PD_long_digits,
 PD_long_long_digits,
 PD_float_digits,
 PD_double_digits,
 PD_quad_digits,
 PD_tolerance = 1000;

double
 PD_float_tol,
 PD_double_tol,
 PD_quad_tol;

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
 *PD_print_formats1[8],
 *PD_print_formats2[8],
 *PD_user_formats1[8],
 *PD_user_formats2[8];

static int
 _PD_print_data(FILE *f0,
		char *prefix, char *before, char *after,
		char *nodename, PDBfile *file, void *vr,
		long nitems, char *type, dimdes *dims,
		int mjr, int def_off, int irecursion,
		int n, long *ind),
 _PD_print_indirection(FILE *f0,
		       char *prefix, char *before, char *after,
		       char *nodename, PDBfile *file, char **vr,
		       long nitems, char *type, dimdes *dims,
		       int mjr, int def_off, int irecursion,
		       int n, long *ind);

static int
 _PD_test_recursion(char *type, char *mtype);

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

static void _PD_print_char_kind(FILE *f0, char *prefix,
				char *before, char *after, char *nodename,
				char *vr, long nitems,
				char *type, dimdes *dims, int mjr,
				int quo, int idx, int nn,
				long offset, int def_off, int n, 
				long *ind)

   {int k, max1, max2;
    long i, j;
    char bf[MAXLINE], s[MAXLINE];
    char *cp;

    cp = (char *) vr;

    if (SC_strstr(PD_print_formats1[idx], "%s") != NULL)
       {if ((nitems == 1L) && (offset == 0L))
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

	    i = min(nitems, max1);
	    nitems -= i;
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

		while (nitems > 0L)
		   {i = min(nitems, max2);
		    nitems -= i;
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

		while (nitems > 0L)
		   {i = min(nitems, max2);
		    nitems -= i;
		    SC_strncpy(bf, MAXLINE, cp, i);

		    cp += i;
		    PRINT(f0, "%s%s%s = %s\n", prefix, after,
			  nodename, bf);};};};}

    else
       {DISP_DATA(f0, cp, nitems, idx, n, ind);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_IO_PRINT - display a primitive type
 *              - use N and IND to control printing of values at specific 
 *              - indices
 */

static int _PD_io_print(FILE *f0, char *prefix, char *before, char *after, 
                        char *nodename, PDBfile *file, char *vr, long nitems,
                        char *type, dimdes *dims, int mjr, int def_off, int n, 
                        long *ind)
   {int k, m, idx, nn, isz, status, quo;
    int *ip;
    long offset, i, j, ni;
    long *lp;
    char bf[MAXLINE], s[MAXLINE];
    char *cp, **t;
    short *sp;
    BIGINT *llp;
    float *fp;
    double *dp;
    defstr *pd;

    status = 0;
    offset = 1L;

    nn = 0;
    j  = 0L;
    k  = 0;
    for (i = 0L; i < nitems; i++, j += offset, k++)
        {if (k > PD_print_controls[4])
            {m  = strlen(PD_index_to_expr(bf, j, dims, mjr, def_off));
             nn = max(nn, m);
             k  = 0;};};
    nn += 7;

    memset(s, ' ', LINE_SIZE);

    pd   = PD_inquire_host_type(file, type);
    isz = pd->size;

/* check for floating point types */
    if (pd->format != NULL)
       {if (strcmp(type, "float") == 0)
	   {fp = (float *) vr;
	    DISP_DATA(f0, fp, nitems, 2, n, ind);}

       else if (strcmp(type, "double") == 0)
	  {dp = (double *) vr;
	   DISP_DATA(f0, dp, nitems, 3, n, ind);}

       else if (strcmp(type, "REAL") == 0)
	  {if (sizeof(REAL) == sizeof(double))
	      {dp = (double *) vr;
	       DISP_DATA(f0, dp, nitems, 3, n, ind);}
	   else
	      {fp = (float *) vr;
	       DISP_DATA(f0, fp, nitems, 2, n, ind);};};}

/* only non-floating point types remain
 * user defined integral primitive types
 * can go off the byte size
 */
    else if (pd->convert >= 0)
       {if (strcmp(type, "short") == 0)
	   {sp = (short *) vr;
	    DISP_DATA(f0, sp, nitems, 4, n, ind);}

	else if (strcmp(type, "int") == 0)
	   {ip = (int *) vr;
	    DISP_DATA(f0, ip, nitems, 0, n, ind);}

	else if (strcmp(type, "integer") == 0)
	   {ip = (int *) vr;
	    DISP_DATA(f0, ip, nitems, 0, n, ind);}

	else if (strcmp(type, "long") == 0)
	   {lp = (long *) vr;
	    DISP_DATA(f0, lp, nitems, 1, n, ind);}

	else if (strcmp(type, "long_long") == 0)
	   {llp = (BIGINT *) vr;
	    DISP_DATA(f0, llp, nitems, 6, n, ind);}

/* GOTCHA: This is a feeble first step toward a generalized
 *         print capability for user defined types
 */
	else if (strcmp(type, "char_8") == 0)
	   {if (SC_strstr(PD_print_formats1[5], "%s") != NULL)
	       {cp = (char *) vr;
		t  = FMAKE_N(char *, nitems, "_PD_IO_PRINT:t");
		for (i = 0; i < nitems; i++, cp += isz)
		    {t[i] = FMAKE_N(char, isz + 1,
                                          "_PD_IO_PRINT:t[]");
		     memcpy(t[i], cp, isz);
		     t[i][isz] = '\0';};
		DISP_DATA(f0, t, nitems, 5, n, ind);
		for (i = 0; i < nitems; i++)
		    SFREE(t[i]);
		SFREE(t);}
	    else
	       {cp = (char *) vr;
		DISP_DATA(f0, cp, isz*nitems, 5, n, ind);};}

	else if (strcmp(type, "function") == 0)
	   {if (PD_print_controls[5] == 0)
	       {PRINT(f0, "%s%s%s = <function>\n", prefix, before, nodename);}
	    else if (PD_print_controls[5] == 1)
	       {PRINT(f0, "%s%s = <function>\n", before, nodename);}
	    else
	       {PRINT(f0, "        <function>\n");};}

	else if ((pd->kind == CHAR_KIND) || (isz == sizeof(char)))
	   {if (strcmp(type, "char") == 0)
	       {idx = 5;
		ni  = nitems;
	        quo = TRUE;}
	    else if (pd->kind == CHAR_KIND)
	       {idx = 5;
		ni  = isz;
	        quo = FALSE;}
	    else
	       {idx = 7;
		ni  = nitems;
	        quo = FALSE;};

	    _PD_print_char_kind(f0, prefix, before, after, nodename,
				vr, ni, type, dims, mjr,
				quo, idx, nn, offset, def_off, n, ind);}

        else if (isz == sizeof(short))
	   {sp = (short *) vr;
	    DISP_DATA(f0, sp, nitems, 7, n, ind);}

	else if (isz == sizeof(int))
	   {ip = (int *) vr;
	    DISP_DATA(f0, ip, nitems, 7, n, ind);}

	else if (isz == sizeof(long))
	   {lp = (long *) vr;
	    DISP_DATA(f0, lp, nitems, 7, n, ind);}

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
                          char *nodename, PDBfile *file, void *vr, long nitems,
                          char *type, dimdes *dims, int mjr, int def_off, 
                          int irecursion, int n, long *ind)
   {int status;

    status = 0;

/* if the type is an indirection, follow the pointer */
    if (_PD_indirection(type))
       status = _PD_print_indirection(f0, prefix, before, after, nodename,
				      file, (char **) vr, nitems, type, 
				      dims, mjr, def_off, irecursion,
				      n, ind);
    else
       status = _PD_print_leaf(f0, prefix, before, after, nodename,
			       file,  vr, nitems, type,
			       dims, mjr, def_off, irecursion,
			       n, ind);

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PRINT_INDIRECTION - handle the indirects for the print process */

static int _PD_print_indirection(FILE *f0, char *prefix,
				 char *before, char *after, char *nodename,
				 PDBfile *file, char **vr, long nitems, 
                                 char *type, dimdes *dims,
				 int mjr, int def_off,
                                 int irecursion, int n, long *ind)
   {long i, ditems;
    int min_index, status;
    char *dtype, *s;
    char field[80], bf[MAXLINE];

    status = 0;

    dtype = PD_dereference(SC_strsavef(type,
                           "char*:_PD_PRINT_INDIRECTION:dtype"));

    strcpy(field, nodename);
    s = field + strlen(field);

    if (dims != NULL)
       min_index = dims->index_min;
    else
       min_index = def_off;

    for (i = 0L; i < nitems; i++, vr++)
        {if (nitems > 1)
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
            {if (_PD_indirection(dtype))
                status &= _PD_print_indirection(f0, prefix,
						before, after, field, file,
						(char **) DEREF(vr),
						ditems, dtype, 
						dims, mjr, def_off,
						irecursion, n, ind);
             else
                status &= _PD_print_leaf(f0, prefix,
					 before, after, field, file,
					 DEREF(vr),
					 ditems, dtype,
					 dims, mjr, def_off,
					 irecursion, n, ind);}
         else
	    {if (PD_print_controls[5] == 0)
	        {PRINT(f0, "%s%s%s = (nil)\n",
		       prefix, before, field);}
	     else if (PD_print_controls[5] == 1)
	        {PRINT(f0, "%s%s = (nil)\n",
		       before, field);}
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

int _PD_print_leaf(FILE *f0, char *prefix, char *before, char *after,  
		   char *nodename, PDBfile *file, char *vr, long nitems,
		   char *type, dimdes *dims, int mjr, int def_off, 
		   int irecursion, int n, long *ind)
   {long ii;
    char *s, *s2, *svr;
    defstr *defp;
    memdes *desc, *mem_lst, *next;
    char field[80], mfield[80];
    int size, min_index, nchars, status;
    char *mbefore, *mafter;
    static char before_2[5] = "|__ ";
    static char after_2[5]  = "|   ";
    static char spaces[5]   = "    ";
   
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
       status &= _PD_io_print(f0, prefix,
			      before, after, nodename, file, vr, nitems,
			      type, dims, mjr, file->default_offset,
			      n, ind);
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

        for (svr = vr, ii = 0L; ii < nitems; ii++)
            {if ((nitems > 1) || (dims != NULL))
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

/* _PD_SET_USER_FORMATS - replace edit formats with user specified formats  */

void _PD_set_user_formats(void)
   {

    if (PD_user_formats1[0] != NULL)
       {SFREE(PD_print_formats1[0]);
        PD_print_formats1[0] = SC_strsavef(PD_user_formats1[0],
                               "char*:_PD_SET_USER_FORMATS:format1(0)");};

    if (PD_user_formats2[0] != NULL)
       {SFREE(PD_print_formats2[0]);
        PD_print_formats2[0] = SC_strsavef(PD_user_formats2[0],
                               "char*:_PD_SET_USER_FORMATS:format2(0)");};

    if (PD_user_formats1[1] != NULL)
       {SFREE(PD_print_formats1[1]);
        PD_print_formats1[1] = SC_strsavef(PD_user_formats1[1],
                               "char*:_PD_SET_USER_FORMATS:format1(1)");};

    if (PD_user_formats2[1] != NULL)
       {SFREE(PD_print_formats2[1]);
        PD_print_formats2[1] = SC_strsavef(PD_user_formats2[1],
                               "char*:_PD_SET_USER_FORMATS:format2(1)");};

    if (PD_user_formats1[2] != NULL)
       {SFREE(PD_print_formats1[2]);
        PD_print_formats1[2] = SC_strsavef(PD_user_formats1[2],
                               "char*:_PD_SET_USER_FORMATS:format1(2)");};

    if (PD_user_formats2[2] != NULL)
       {SFREE(PD_print_formats2[2]);
        PD_print_formats2[2] = SC_strsavef(PD_user_formats2[2],
                               "char*:_PD_SET_USER_FORMATS:format2(2)");};

    if (PD_user_formats1[3] != NULL)
       {SFREE(PD_print_formats1[3]);
        PD_print_formats1[3] = SC_strsavef(PD_user_formats1[3],
                               "char*:_PD_SET_USER_FORMATS:format1(3)");};

    if (PD_user_formats2[3] != NULL)
       {SFREE(PD_print_formats2[3]);
        PD_print_formats2[3] = SC_strsavef(PD_user_formats2[3],
                               "char*:_PD_SET_USER_FORMATS:format2(3)");};

    if (PD_user_formats1[4] != NULL)
       {SFREE(PD_print_formats1[4]);
        PD_print_formats1[4] = SC_strsavef(PD_user_formats1[4],
                               "char*:_PD_SET_USER_FORMATS:format1(4)");};

    if (PD_user_formats2[4] != NULL)
       {SFREE(PD_print_formats2[4]);
        PD_print_formats2[4] = SC_strsavef(PD_user_formats2[4],
                               "char*:_PD_SET_USER_FORMATS:format2(4)");};

    if (PD_user_formats1[5] != NULL)
       {SFREE(PD_print_formats1[5]);
        PD_print_formats1[5] = SC_strsavef(PD_user_formats1[5],
                               "char*:_PD_SET_USER_FORMATS:format1(5)");};

    if (PD_user_formats2[5] != NULL)
       {SFREE(PD_print_formats2[5]);
        PD_print_formats2[5] = SC_strsavef(PD_user_formats2[5],
                               "char*:_PD_SET_USER_FORMATS:format2(5)");};

    if (PD_user_formats1[7] != NULL)
       {SFREE(PD_print_formats1[7]);
        PD_print_formats1[7] = SC_strsavef(PD_user_formats1[7],
                               "char*:_PD_SET_USER_FORMATS:format1(7)");};

    if (PD_user_formats2[7] != NULL)
       {SFREE(PD_print_formats2[7]);
        PD_print_formats2[7] = SC_strsavef(PD_user_formats2[7],
                               "char*:_PD_SET_USER_FORMATS:format2(7)");};
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SET_FORMAT_DEFAULTS - set the defaults for the edit formats 
 *                         -   0 = int
 *                         -   1 = long
 *                         -   2 = float
 *                         -   3 = double
 *                         -   4 = short
 *                         -   5 = char
 *                         -   6 = long long
 *                         -   7 = bit
 */

void _PD_set_format_defaults(void)
   {int i;
    char tmp[MAXLINE], *t;
    
/* used for scalars */
    if (PD_print_formats1[0] != NULL)
       SFREE(PD_print_formats1[0]);
    snprintf(tmp, MAXLINE, "%%%dd", PD_int_digits);
    t = SC_strsavef(tmp, "char*:_PD_SET_FORMAT_DEFAULTS:format1(0)");
    PD_print_formats1[0] = t;

    if (PD_print_formats1[1] != NULL)
       SFREE(PD_print_formats1[1]);
    snprintf(tmp, MAXLINE, "%%%dld", PD_long_digits);
    t = SC_strsavef(tmp, "char*:_PD_SET_FORMAT_DEFAULTS:format1(1)");
    PD_print_formats1[1] = t;

    if (PD_print_formats1[2] != NULL)
       SFREE(PD_print_formats1[2]);
    snprintf(tmp, MAXLINE, "%%# .%de", PD_float_digits);
    t = SC_strsavef(tmp, "char*:_PD_SET_FORMAT_DEFAULTS:format1(2)");
    PD_print_formats1[2] = t;

    if (PD_print_formats1[3] != NULL)
       SFREE(PD_print_formats1[3]);
    snprintf(tmp, MAXLINE, "%%# .%de", PD_double_digits);
    t = SC_strsavef(tmp, "char*:_PD_SET_FORMAT_DEFAULTS:format1(3)");
    PD_print_formats1[3] = t;

    if (PD_print_formats1[4] != NULL)
       SFREE(PD_print_formats1[4]);
    snprintf(tmp, MAXLINE, "%%%dd", PD_short_digits);
    t = SC_strsavef(tmp, "char*:_PD_SET_FORMAT_DEFAULTS:format1(4)");
    PD_print_formats1[4] = t;

    if (PD_print_formats1[5] != NULL)
       SFREE(PD_print_formats1[5]);
    t = SC_strsavef("%s", "char*:_PD_SET_FORMAT_DEFAULTS:format1(5)");
    PD_print_formats1[5] = t;

    if (PD_print_formats1[6] != NULL)
       SFREE(PD_print_formats1[6]);

#ifdef NO_LONG_LONG

# ifdef SOLARIS
/* NOTE: the factor of 2 appears to be necesary because the actual long long
 * field width will be twice what is specified - emperically verified on
 * all current platforms with long long
 */
    snprintf(tmp, MAXLINE, "%%%dld", 2*PD_long_long_digits);
# else
    snprintf(tmp, MAXLINE, "%%%dld", PD_long_long_digits);
# endif

#else
    snprintf(tmp, MAXLINE, "%%%dlld", PD_long_long_digits);
#endif

    t = SC_strsavef(tmp, "char*:_PD_SET_FORMAT_DEFAULTS:format1(6)");
    PD_print_formats1[6] = t;

    if (PD_print_formats1[7] != NULL)
       SFREE(PD_print_formats1[7]);
    t = SC_strsavef("%x", "char*:_PD_SET_FORMAT_DEFAULTS:format1(7)");
    PD_print_formats1[7] = t;

/* used for arrays */
    for (i = 0; i < 8; i++)
        {if (PD_print_formats2[i] != NULL)
            SFREE(PD_print_formats2[i]);

         t = SC_strsavef(PD_print_formats1[i],
			 "char*:_PD_SET_FORMAT_DEFAULTS:formats2");
         PD_print_formats2[i] = t;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SET_DIGITS - set the decimal printing precision parameters */

void _PD_set_digits(PDBfile *file)
   {int d;
    long *f;
    double log2;
    data_standard *std;
    

    log2 = log10(2.0);

    std = file->std;

    if (std->fx[0].bpi < 0)
       d = -std->fx[0].bpi;
    else
       d = 8*std->fx[0].bpi;
    PD_short_digits = log2*d + 1; 

    if (std->fx[1].bpi < 0)
       d = -std->fx[1].bpi;
    else
       d = 8*std->fx[1].bpi;
    PD_int_digits   = log2*d + 1; 

    if (std->fx[2].bpi < 0)
       d = -std->fx[2].bpi;
    else
       d = 8*std->fx[2].bpi;
    PD_long_digits  = log2*d + 1; 

    if (std->fx[3].bpi < 0)
       d = -std->fx[3].bpi;
    else
       d = 8*std->fx[3].bpi;
    PD_long_long_digits  = log2*d + 1; 

    f = std->fp[0].format;
    d = min(f[2], PD_tolerance);
    PD_float_tol    = POW(2.0, -((double) d));
    PD_float_digits = log2*d + 1;

    f = std->fp[1].format;
    d = min(f[2], PD_tolerance);
    PD_double_tol    = POW(2.0, -((double) d));
    PD_double_digits = log2*d + 1;

    f = std->fp[2].format;
    d = min(f[2], PD_tolerance);
    PD_quad_tol    = POW(2.0, -((long double) d));
    PD_quad_digits = log2*d + 1;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DIGITS_TOL - compute the number of decimal digits for printing and
 *               - the comparison tolerances for floating point numbers
 */

void _PD_digits_tol(PDBfile *file_a, PDBfile *file_b)
   {int nmb, da, db;
    long *fa, *fb;
    double log2;
    data_standard *stda, *stdb;
    

    log2 = log10(2.0);

    stda = file_a->std;
    stdb = file_b->std;

    if (stda->fx[0].bpi < 0)
       da = -stda->fx[0].bpi;
    else
       da = 8*stda->fx[0].bpi;
    if (stdb->fx[0].bpi < 0)
       db = -stdb->fx[0].bpi;
    else
       db = 8*stdb->fx[0].bpi;
    nmb = max(da, db);
    PD_short_digits = log2*nmb + 1; 

    if (stda->fx[1].bpi < 0)
       da = -stda->fx[1].bpi;
    else
       da = 8*stda->fx[1].bpi;
    if (stdb->fx[1].bpi < 0)
       db = -stdb->fx[1].bpi;
    else
       db = 8*stdb->fx[1].bpi;
    nmb = max(da, db);
    PD_int_digits   = log2*nmb + 1; 

    if (stda->fx[2].bpi < 0)
       da = -stda->fx[2].bpi;
    else
       da = 8*stda->fx[2].bpi;
    if (stdb->fx[2].bpi < 0)
       db = -stdb->fx[2].bpi;
    else
       db = 8*stdb->fx[2].bpi;
    nmb = max(da, db);
    PD_long_digits  = log2*nmb + 1; 

    if (stda->fx[3].bpi < 0)
       da = -stda->fx[3].bpi;
    else
       da = 8*stda->fx[3].bpi;
    if (stdb->fx[3].bpi < 0)
       db = -stdb->fx[3].bpi;
    else
       db = 8*stdb->fx[3].bpi;
    nmb = max(da, db);
    PD_long_long_digits  = log2*nmb + 1; 

    fa  = stda->fp[0].format;
    fb  = stdb->fp[0].format;
    nmb = max(fa[2], fb[2]);
    nmb = min(nmb, PD_tolerance);
    PD_float_tol    = POW(2.0, -((double) nmb));
    PD_float_digits = log2*nmb + 1;

    fa  = stda->fp[1].format;
    fb  = stdb->fp[1].format;
    nmb = max(fa[2], fb[2]);
    nmb = min(nmb, PD_tolerance);
    PD_double_tol    = POW(2.0, -((double) nmb));
    PD_double_digits = log2*nmb + 1;

    fa  = stda->fp[2].format;
    fb  = stdb->fp[2].format;
    nmb = max(fa[2], fb[2]);
    nmb = min(nmb, PD_tolerance);
    PD_quad_tol    = POW(2.0, -((long double) nmb));
    PD_quad_digits = log2*nmb + 1;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
