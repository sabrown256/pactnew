/*
 * SXTABLE.C - ASCII tables for SX
 *
 * Source Version: 4.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

int
 _SX_table_n = 0;

long
 _SX_table_ln = 0;

char
 *SX_table_name = NULL;

static PM_matrix
 *SX_current_table = NULL;

static char
 **SX_current_table_labels = NULL;

/*--------------------------------------------------------------------------*/

/*                                INPUT ROUTINES                            */

/*--------------------------------------------------------------------------*/

static int _SX_get_line_length(FILE *fp)
   {int i, nr, length, count;
    long addrsave;
    char buf[MAX_BFSZ];
    
    length = 0;
    count  = 0;
    
    if (fp != NULL)
       {addrsave = io_tell(fp);
	io_seek(fp, 0, SEEK_SET);

	while ((nr = io_read(buf, sizeof(char), MAX_BFSZ, fp)) > 0)
	   {for (i = 0; i < nr; i++)
	        {if (buf[i] != '\n')
		    count++;
		 else
		    {count++;
		     length = max(count, length);
		     count  = 0;};}
	    if (nr != MAX_BFSZ)
	       break;}

	io_seek(fp, addrsave, SEEK_SET);};

    return(length);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_SETUP_CLABELS - Set up SX_current_table_labels--an array of column
 *                  - label strings.  The input is label, the string containing
 *                  - potential column labels, and nc, the number of columns.
 *                  - The input string will be cracked into tokens, and if there
 *                  - are nc or nc+1 of them they will be stored as nc column
 *                  - labels.  If there are nc+1 tokens the first will be assumed
 *                  - to be a comment character and disregarded.
 */                    

static int _SX_setup_clabels(char *label, int nc, int linelen)  
   {int ntokens, i, rv;

    if (SX_current_table_labels != NULL)
       {SC_free_strings(SX_current_table_labels);
        SX_current_table_labels = NULL;};

    if ((label == NULL) || (label[0] == '\0'))
       rv = FALSE;

    else
       {SX_current_table_labels = SC_tokenize(label, " \t\n\r");

	for (ntokens = 0; SX_current_table_labels[ntokens] != NULL; ntokens++);

	if (ntokens == (nc+1))
/* Assume first token is comment char and take it out of list */
	   {SFREE(SX_current_table_labels[0]);

	    for (i = 0; i < nc; i++)
	        SX_current_table_labels[i] = SX_current_table_labels[i+1];

	    SX_current_table_labels[nc] = NULL;}

/* Anything other than nc tokens means we can't interpret label as column labels */
	else if (ntokens != nc)
	   {SC_free_strings(SX_current_table_labels);
	    SX_current_table_labels = NULL;};

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_FIND_TEXT_TABLE - find the specified table in the given file
 *                    - return TRUE if successful, otherwise return FALSE
 *
 * table-row :  line of one or more tokens, all numeric
 *                or
 *              line of two or more tokens, all numeric except the first
 * first-row :  table-row
 *                and
 *              (previous line not table-row
 *                 or
 *               number-of-tokens(this line) != number-of-tokens(previous line)
 *                 or
 *               number-numeric-tokens(this line) != number-numeric-tokens(previous line))
 * continuation-row :  table-row
 *                       and
 *                     not first-row
 * table :  first-row followed by zero or more continuation-rows
 * nc    :  number of numeric tokens per table-row
 * nr    :  one plus number of continuation-rows
 * 
 */

static int SX_find_text_table(FILE *fp, int n, int linelen, char *linein,
			      int *pfn, int *pnr, int *pnc, int nl,
			      long *paddrt, int nlab, long *paddrl)
   {int i, j, nc, nr, nt, firstnum, nbefore, nafter, nlb, rv;
    long *addr, naddr, addrt, addrl;
    char *token;

    addrl   = -1;
    nbefore = 0;
    nafter  = 0;
    if (nlab > 0)
       nbefore = nlab;
    else
       nafter = -nlab;

    addr = FMAKE_N(long, nbefore+1, "SX_FIND_TEXT_TABLE:addr");

    nr  = 0;
    nc  = 0;
    nlb = 0;

/* skip to specified line */
    for (i = 1; i < nl; i++)
	{addr[nlb++] = io_tell(fp);
	 if (nlb > nbefore)
	    nlb = 0;
	 if (GETLN(linein, linelen, fp) == NULL)
            return(FALSE);};

/* loop over tables */
    for (j = 0; j < n; j++)

/* loop over lines until first-row found or EOF */
        {while (TRUE)
            {nc       = 0;
             firstnum = FALSE;
             addrt    = io_tell(fp);

             addr[nlb++] = addrt;
             if (nlb > nbefore)
                nlb = 0;

             if (GETLN(linein, linelen, fp) == NULL)
                break;

/* loop over tokens */
             for (nt = 0; TRUE; nt++)
                 {token = SC_firsttok(linein, " \t\n\r");
                  if (token == NULL)
                     break;
                  if (nt == 0)
                     {if (SC_numstrp(token))
                         firstnum = TRUE;}
                  else if (!SC_numstrp(token))
                     {nt = 0;
                      break;};};

             nc = firstnum ? nt : nt - 1;
             if (nc > 0)
                break;};
            
/* loop over lines of table */
         for (nr = 1; TRUE; nr++)
             {naddr = io_tell(fp);
              if (GETLN(linein, linelen, fp) == NULL)
                 break;

              for (nt = 0; TRUE; nt++)
                  {token = SC_firsttok(linein, " \t\n\r");
                   if (token == NULL)
                       break;
                   if (nt == 0)
                      {if ((SC_numstrp(token) && !firstnum) ||
			   (!SC_numstrp(token) && firstnum))
                          break;}
                   else
                      if (!SC_numstrp(token))
                         {nt = 0;
                          break;};};

              nt = firstnum ? nt : nt - 1;
              if (nt != nc)
                 break;

/* if this table is before the one we want, buffer up addresses. */
	      if (j < n-1)
		{addr[nlb++] = naddr;
		 if (nlb > nbefore)
		   nlb = 0;};};

         io_seek(fp, naddr, SEEK_SET);};
         
    if (nbefore > 0)
       {nlb--;
        if (nlb < 0)
           nlb += nbefore + 1;
        addrt = addr[nlb];

        nlb -= nbefore;
        if (nlb < 0)
           nlb += nbefore + 1;
        addrl = addr[nlb];};

    if (nafter > 0)
       {addrt = addr[0];
        for (i = 0; i < nafter; i++)
            {addrl = io_tell(fp);
             if (GETLN(linein, linelen, fp) == NULL)
                return(FALSE);};};

    SFREE(addr);

    if ((j == n) && (nc > 0))
       {*pfn    = firstnum;
	*pnc    = nc;
	*pnr    = nr;
	*paddrt = addrt;
	*paddrl = addrl;
	rv = TRUE;}
    else
       rv = FALSE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_READ_TEXT_TABLE - read a table of numbers from an  ASCII input file */

static object *_SXI_read_text_table(object *argl)
   {int i, j, n, nc, nr, nl, fn, nlabel, linelen;
    long addrt, addrl;
    char *name, *token;
    char *label, *linein;
    FILE *fp;
    object *rv;

    if (SX_current_table != NULL)
       {PM_destroy(SX_current_table);
        SX_current_table = NULL;};

    name   = NULL;
    n      = 1;
    nl     = 1;
    nlabel = 0;
    SS_args(argl,
            SC_STRING_I, &name,
            SC_INT_I, &n,
            SC_INT_I, &nlabel,
            SC_INT_I, &nl,
            0);

    SX_table_name = SC_strsavef(name, "char*:_SXI_READ_TEXT_TABLE:tablename");

    name = SC_search_file(NULL, SX_table_name);
    if (name == NULL)
       SS_error("CAN'T FIND FILE - _SXI_READ_TEXT_TABLE", argl);

    fp = io_open(name, "r");
    if (fp == NULL)
       SS_error("CAN'T OPEN FILE - _SXI_READ_TEXT_TABLE", argl);

    linelen = _SX_get_line_length(fp);
    if (linelen <= 0)
       SS_error("CAN'T READ FILE - _SXI_READ_TEXT_TABLE", argl);

    linelen += 10;
    label  = FMAKE_N(char, linelen, "_SXI_READ_TEXT_TABLE:label");
    linein = FMAKE_N(char, linelen, "_SXI_READ_TEXT_TABLE:linein");

    memset(label, 0, linelen);

    if (!SX_find_text_table(fp, n, linelen, linein, &fn, &nr, &nc,
			    nl, &addrt, nlabel, &addrl))
       {SFREE(linein);
        SFREE(label);
        SS_error("REQUESTED TABLE NOT FOUND - _SXI_READ_TEXT_TABLE", argl);}

    SX_current_table = PM_create(nr, nc);

    if (addrl != -1)
       {if (io_seek(fp, addrl, SEEK_SET))
	   {SFREE(linein);
            SFREE(label);
            return(FALSE);}

	GETLN(label, linelen, fp);}
    else
        label[0] = '\0';

    if (io_seek(fp, addrt, SEEK_SET))
       {SFREE(linein);
        SFREE(label);
        return(FALSE);}

    for (i = 1; i <= nr; i++)
        {GETLN(linein, linelen, fp);

         for (j = 1; j <= nc; j++)
             {token = SC_firsttok(linein, " \t\n\r");
              if ((j == 1) && !fn)
                 token = SC_firsttok(linein, " \t\n\r");

              PM_element(SX_current_table, i, j) = ATOF(token);};};

    io_close(fp);

/* store column labels if available */
    _SX_setup_clabels(label, nc, linelen);

    if (SS_interactive == ON)
       {if (label[0] == '\0')
           PRINT(stdout,
                 "\n Table %d : %d rows and %d columns\n\n",
                 n, nr, nc);
        else
           PRINT(stdout,
                 "\n Table %d : %d rows and %d columns\n Label: %s\n\n",
                 n, nr, nc, label);};

    SFREE(linein);
    SFREE(label);

    _SX_table_n = n;
    _SX_table_ln = nl;

    rv = SS_make_list(SC_INT_I, &nr,
		      SC_INT_I, &nc,
		      0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_EXTRACT_VECTOR - extract a vector from the given array */

static double *SX_extract_vector(PM_matrix *a, long o, long s, long n)
   {long i;
    double *val, *src;

    src = a->array + o;
    val = FMAKE_N(double, n, "SX_EXTRACT_VECTOR:val");
    for (i = 0L; i < n; i++)
        val[i] = src[i*s];

    return(val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_INSERT_VECTOR - insert a vector into the given array */

static void SX_insert_vector(PM_matrix *a, double *val, long o, long s, long n)
   {long i;
    double *src;

    src = a->array + o;

    for (i = 0L; i < n; i++)
        src[i*s] = val[i];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_NORMALIZE_VECTOR - normalize a vector */

static void SX_normalize_vector(double *val, long n)
   {long i;
    double xmin, xmax, range;
    
    PM_maxmin(val, &xmin, &xmax, n); 
    if (xmax == xmin)
       {for (i = 0L; i < n; i++)
            val[i] = 0.0;}
    else
       {range = xmax - xmin;
        for (i = 0L; i < n; i++)
            val[i] = (val[i] - xmin)/range;}

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_CNORMALIZE_TABLE - normalize a table columnwise */

static void _SX_cnormalize_table(PM_matrix *a)
   {long i;
    long nrow, ncol;
    double *vect;

    nrow = a->nrow;
    ncol = a->ncol;

    for (i = 0; i < ncol; i++)
        {vect = SX_extract_vector(a, i, ncol, nrow);
         SX_normalize_vector(vect, nrow);
         SX_insert_vector(a, vect, i, ncol, nrow);}

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_LR_ZC_DOMAIN - build a set from information in the current table 
 *                  - using nrow+1 and ncol+1 to generate an LR domain suitable
 *                  - for use in a zone-centered mapping
 */

static PM_set *_SX_lr_zc_domain(char *name)
   {int nd, *maxes;
    PM_set *set;
    void **elements;
    double *x, *y;
    double *extrema;
    int i, nelem;

    nd = 2;
    maxes = FMAKE_N(int, nd, "_SX_LR_ZC_DOMAIN:maxes");
    maxes[0] = SX_current_table->ncol + 1;
    maxes[1] = SX_current_table->nrow + 1;

    set = PM_make_lr_index_domain(name, SC_DOUBLE_S, nd, nd,
				  maxes, NULL, NULL);

    elements = (void **) set->elements;
    x = (double *) elements[0];
    y = (double *) elements[1];

    nelem = maxes[0] * maxes[1];

/* shift the domain by one to make it zero origin */    
    for (i = 0; i < nelem; i++)
        {x[i] -= 1.0;
         y[i] -= 1.0;}
    
    extrema = set->extrema;
 
    for (i = 0; i < (2*nd); i++)
        extrema[i] -= 1.0;

    return(set);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_TABLE_SET - build a set from information in the TABLE using
 *               - specifications in SPECS as follows:
 *               -
 *               - <set-list>  := (name <dims> <component-1> <component-2> ...)
 *               - <dims>      := (d1 d2 ...)
 *               - <component> := (start [n-pts] [stride])
 */

static PM_set *_SX_table_set(object *specs)
   {int i, nd, nde, dv;
    int *maxes;
    long start, step, ne, npts;
    char *name;
    double **elem;
    PM_set *set;
    object *sp, *dims, *comps;

    name = NULL;
    dims = SS_null;
    SS_args(specs,
	    SC_STRING_I, &name,
	    SS_OBJECT_I, &dims,
	    0);

    if (dims == SS_null)
       set = _SX_lr_zc_domain(name);

    else
       {nd    = SS_length(dims);
	maxes = FMAKE_N(int, nd, "_SX_TABLE_SET:maxes");
	for (i = 0; dims != SS_null; dims = SS_cdr(dims), i++)
	    {SS_args(dims,
		     SC_INT_I, &dv,
		     0);
	     maxes[i] = dv;};

	comps = SS_cddr(specs);
	nde   = SS_length(comps);
	elem  = FMAKE_N(double *, nde, "_SX_TABLE_SET:elem");
	ne    = 0;

	for (i = 0; comps != SS_null; comps = SS_cdr(comps), i++)
	    {sp = SS_car(comps);
	     start = 0L;
	     npts  = SX_current_table->nrow;
	     step  = SX_current_table->ncol;
	     SS_args(sp,
		     SC_LONG_I, &start,
		     SC_LONG_I, &npts,
		     SC_LONG_I, &step,
		     0);

	     if ((i > 0) && (npts != ne))
	        SS_error("BAD SPECIFICATION - _SX_TABLE_SET", sp);

	     elem[i] = SX_extract_vector(SX_current_table, start, step, npts);
	     ne = npts;};

	set = PM_mk_set(name, SC_DOUBLE_S, FALSE, ne, nd, nde,
			maxes, elem, NULL,
			NULL, NULL, NULL,
			NULL, NULL, NULL, NULL, NULL);};

    return(set);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_TABLE_SET - build a set from information in the TABLE using
 *              - specifications in SPECS as follows:
 *              -
 *              - <set-list>  := (name <dims> <component-1> <component-2> ...)
 *              - <dims>      := (d1 d2 ...)
 *              - <component> := (start [n-pts] [stride])
 */

static object *SX_table_set(object *specs)
   {object *rv;
    PM_set *set;

    set = _SX_table_set(specs);
    rv  = SX_mk_set(set);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_TABLE_MAP - create and return a graph from the current table using
 *              - the given specifications
 *              -
 *              - (table->pm-mapping <name> <domain-list> <range-list> <centering>)
 *              - <set-list>  := (name <dims> <component-1> <component-2> ...)
 *              - <dims>      := (d1 d2 ...)
 *              - <component> := (start [stop] [stride])
 */

static object *_SXI_table_map(object *argl)
   {char *name, bf[MAXLINE];
    object *dmlst, *rnlst, *rv;
    PM_centering centering;
    PM_set *domain, *range;
    PM_mapping *f;

    name      = NULL;
    centering = N_CENT;

    SS_args(argl,
	    SC_STRING_I, &name,
	    SS_OBJECT_I, &dmlst,
	    SS_OBJECT_I, &rnlst,
            SC_INT_I, &centering,
            0);

    domain = _SX_table_set(dmlst);
    if (SS_nullobjp(rnlst))
       range = NULL;
    else
       range = _SX_table_set(rnlst);

    if (name == NULL)
       {static int tm = 1;

	snprintf(bf, MAXLINE, "TableMap%d", tm++);
	name = bf;};

    f = PM_make_mapping(name, PM_LR_S, domain, range, centering, NULL);

    rv = SX_mk_mapping(f);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                          MAPPING OUTPUT ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* SX_WRT_TEXT_TABLE - write mapping to an ASCII file */

static object *SX_wrt_text_table(object *argl)
   {long i, j, ne, ndd, ndr, nded, nder;
    int *maxes;
    object *outprt;
    double **delem, **relem;
    PM_set *domain, *range;
    PM_mapping *f;
    FILE *fp;

    outprt = SS_car(argl);
    argl   = SS_cdr(argl);

    fp = SS_OUTSTREAM(outprt);

    for ( ; SS_consp(argl); argl = SS_cdr(argl))
        {SS_args(argl,
		 G_MAPPING, &f,
		 0);

         domain = f->domain;
         range  = f->range;

         ndd  = domain->dimension;
         ndr  = range->dimension;
         nded = domain->dimension_elem;
         nder = range->dimension_elem;
         ne   = domain->n_elements;

         delem = (double **) domain->elements;
         relem = (double **) range->elements;

/* print the dimension info */
	 io_printf(fp, "# %s (dimension", f->name);
         maxes = domain->max_index;
	 for (i = 0; i < ndd; i++)
	     io_printf(fp, " %d", maxes[i]);
	 io_printf(fp, ")\n");

/* print the column labels */
         io_printf(fp, "# Indx");
	 for (i = 0; i < nded; i++)
	     io_printf(fp, "     D%ld    ", i+1);
	 for (i = 0; i < nder; i++)
	     io_printf(fp, "     R%ld    ", i+1);
	 io_printf(fp, "\n");

/* print the columns */
	 for (j = 0L; j < ne; j++)
	     {io_printf(fp, "%6ld", j+1);
	      for (i = 0; i < nded; i++)
		  io_printf(fp, " %10.3e", delem[i][j]);
	      for (i = 0; i < nder; i++)
		  io_printf(fp, " %10.3e", relem[i][j]);
	      io_printf(fp, "\n");};

	 io_printf(fp, "\n");};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_WRT_CURRENT_TABLE - write current table to an ASCII file */

static object *SX_wrt_current_table(object *argl)
   {char *fname;
    int i, j, k, nr, nc;
    double *ap;
    FILE *fp;


    if (SX_current_table == NULL)
       return(SS_f);    

    fname = NULL;
    SS_args(argl,
            SC_STRING_I, &fname,
            0);

    if (fname != NULL)
       {fp = io_open(fname, "w");
        if (fp == NULL)
           SS_error("CAN'T OPEN FILE - TV_WRT_CURRENT_TABLE", SS_mk_string(fname));}

/* write the labels if any */
    if (SX_current_table_labels != NULL)
       {io_printf(fp, "#\t");

        i = 0;
        while (SX_current_table_labels[i] != NULL)
           {io_printf(fp, "%s\t", SX_current_table_labels[i]);
            i++;};
        io_printf(fp, "\n");};

/* write the numbers */
    nr = SX_current_table->nrow;
    nc = SX_current_table->ncol;
    ap = SX_current_table->array;

    k = 0;
    for (i = 0; i < nr; i++)
        for (j = 0; j < nc; j++)
            {io_printf(fp, SX_text_output_format, ap[k++]);
             if (j == (nc-1))
                io_printf(fp, "\n");
             else
                io_printf(fp, "\t");};

    io_close(fp);            

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_PRINT_COLUMN - print a column from the current table--assumes 0 based numbering */

static object *SX_print_column(object *argl)
   {long col, nr, nc, j;
    double *val;
    object *rv;

    rv = SS_null;

    if (SX_current_table == NULL)
       SS_error("NO CURRENT TABLE EXISTS - USE read-table TO CREATE", argl);

    else
       {SS_args(argl,
		SC_LONG_I, &col,
		0);
 
	nr = SX_current_table->nrow;
	nc = SX_current_table->ncol;

	if ((col < 0) || (col >= nc))
	  SS_error("COLUMN NUMBER OUT OF RANGE--COLUMN NUMBERING IS 0 BASED",
		   argl);

	val = SX_extract_vector(SX_current_table, col, nc, nr);

	PRINT(stdout, "\n");
	for (j = 0L; j < nr; j++)
	    {PRINT(stdout, " %10.3e\n", val[j]);}

	if (SS_interactive == ON)
	   PRINT(stdout,
                 "\n Column %ld : %ld values\n\n",
                 col, nr);

	rv = SS_make_list(SC_INT_I, &nr,
			  0);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_DEL_LABEL - delete the label for the specified column in current table */

static int _SX_del_label(long *cols, long ncol)
   {int i, j, nc;
    long *scols, ndcol;
    char **temp;

    nc = SX_current_table->ncol;
    temp = FMAKE_N(char *, nc, "_SX_DEL_LABEL:temp");

    for (i = 0, j = 0, scols = cols, ndcol = 0; i < nc; i++)
        {if (i != *scols)
            temp[j++] = SX_current_table_labels[i];
         else
            {SFREE(SX_current_table_labels[i]);
             if (ndcol < ncol)
                {scols++;
                 ndcol++;};};}

    SFREE(SX_current_table_labels);
    SX_current_table_labels = temp;

    return(TRUE);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_DELETE_COLUMN - delete a column from the current table */

static object *SX_delete_column(object *argl)
   {int nr, nc, i, j;
    int done = 0;
    C_array *arr;
    long *data, *sdata, ncol, temp;
    object *rv;

    rv = SS_null;

    if (SX_current_table == NULL)
       SS_error("NO CURRENT TABLE EXISTS - USE read-table TO CREATE", argl);

    else
       {SS_args(argl,
                G_NUM_ARRAY, &arr,
		0);

        data = (long *)arr->data;
        ncol = arr->length;

/* sort the input columns */
        while (!done)
           {done = 1;
            for (i = 0; i < (ncol-1); i++)
                if (data[i] > data[i+1])
                   {temp = data[i];
                    data[i] = data[i+1];
                    data[i+1] = temp;
                    done = 0;};}                  

/* remove duplicate entries */
        sdata = FMAKE_N(long, ncol, "SX_DELETE_COLUMN:sdata");
        
        sdata[0] = data[0];
        for (i = 1, j = 1; i < ncol; i++)
            {if (data[i] != sdata[j-1])
                {sdata[j++] = data[i];};}

	_SX_del_label(sdata, j);
	PM_del_col(SX_current_table, sdata, j);

        SFREE(sdata);

	nr = SX_current_table->nrow;
	nc = SX_current_table->ncol;

	if (SS_interactive == ON)
	   PRINT(stdout,
                 "\n Current Table : %d rows and %d columns\n\n",
                 nr, nc);

	rv = SS_make_list(SC_INT_I, &nr,
			  SC_INT_I, &nc,
			  0);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_SORT_ON_COLUMN - sort the current table on a column */

static object *SX_sort_on_column(object *argl)
   {int col, nr, nc;
    object *rv;

    rv = SS_null;

    if (SX_current_table == NULL)
       SS_error("NO CURRENT TABLE EXISTS - USE read-table TO CREATE", argl);

    else
       {SS_args(argl,
		SC_INT_I, &col,
		0);

	PM_sort_on_col(SX_current_table, col);

	nr = SX_current_table->nrow;
	nc = SX_current_table->ncol;

	if (SS_interactive == ON)
	   PRINT(stdout,
                 "\n Current Table : %d rows and %d columns\n\n",
                 nr, nc);

	rv = SS_make_list(SC_INT_I, &nr,
			  SC_INT_I, &nc,
			  0);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_CNORMALIZE_TABLE - normalize the current table columnwise */

static object *SX_cnormalize_table(object *argl)
   {int nr, nc;
    object *rv;

    rv = SS_null;

    if (SX_current_table == NULL)
       SS_error("NO CURRENT TABLE EXISTS - USE read-table TO CREATE", argl);

    else
       {_SX_cnormalize_table(SX_current_table);

	nr = SX_current_table->nrow;
	nc = SX_current_table->ncol;

	if (SS_interactive == ON)
	   PRINT(stdout,
                 "\n Current Table : %d rows and %d columns\n\n",
                 nr, nc);

	rv = SS_make_list(SC_INT_I, &nr,
			  SC_INT_I, &nc,
			  0);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_COL_LABELS - return the column labels */

static object *SX_col_labels(object *argl)
   {int i, nc;
    object *obj, *lst;

    obj = SS_null;
    lst = SS_null;

    if (SX_current_table != NULL)
       {nc = SX_current_table->ncol;

	if ((SX_current_table != NULL) && (SX_current_table_labels != NULL))
	   {for (i = nc-1; i >= 0; i--)
	        {if (SX_current_table_labels[i] == NULL)
		    break;
		 obj = SS_mk_string(SX_current_table_labels[i]);
		 lst = SS_mk_cons(obj, lst);};};};

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* SX_INSTALL_ASCII_FUNCS - install the ASCII extensions to Scheme */
 
void SX_install_ascii_funcs(void)
   {

    SS_install("read-table*",
               "Procedure: Read nth table that starts at or after specified line in ASCII file\n     Usage: read-table* <file> [<n> [<label-line> [<line>]]]",
               SS_nargs,
               _SXI_read_text_table, SS_PR_PROC);

    SS_install("read-table",
               "Macro: Read nth table that starts at or after specified line in ASCII file\n     Usage: read-table* <file> [<n> [<label-line> [<line>]]]",
               SS_nargs,
               _SXI_read_text_table, SS_UR_MACRO);

    SS_install("table->pm-mapping",
               "Procedure: Extract a mapping from current table\n     Usage: table->pm-mapping <name> <domain-list> <range-list>",
               SS_nargs,
               _SXI_table_map, SS_PR_PROC);

    SS_install("table->pm-set",
               "Procedure: Extract a set from current table\n     Usage: table->pm-set <name> <domain-list>",
               SS_nargs,
               SX_table_set, SS_PR_PROC);

    SS_install("pm-mapping->table",
               "Procedure: Write a mapping to the given file\n     Usage: pm-mapping->table <file> [<mapping>]+",
               SS_nargs,
               SX_wrt_text_table, SS_PR_PROC);

    SS_install("del-column",
               "Procedure: Delete column n from the current table\n     Usage: del-column <n>",
               SS_nargs,
               SX_delete_column, SS_PR_PROC);

    SS_install("sort-on-column",
               "Procedure: Sort the current table on a column\n     Usage: sort-on-column <n>",
               SS_nargs,
               SX_sort_on_column, SS_PR_PROC);

    SS_install("print-column",
               "Procedure: Print column n from the current table\n     Usage: print-column <n>",
               SS_nargs,
               SX_print_column, SS_PR_PROC);

    SS_install("col-normalize",
               "Procedure: Normalize the current table columnwise\n     Usage: col-normalize",
               SS_zargs,
               SX_cnormalize_table, SS_PR_PROC);

    SS_install("col-labels",
               "Procedure: Return the column labels\n     Usage: col-labels",
               SS_zargs,
               SX_col_labels, SS_PR_PROC);

    SS_install("write-table",
               "Macro: Write the current table to the named file\n     Usage: write-table <fname>",
               SS_nargs,
               SX_wrt_current_table, SS_UR_MACRO);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

