/*
 * PDMEMB.C - member handling routines for PDBLib
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

/*--------------------------------------------------------------------------*/

/*                          ACCESSOR FUNCTIONS                              */

/*--------------------------------------------------------------------------*/

/* _PD_MEMBER_TYPE - strip off dimensions and member_name by inserting
 *                 - a '\0' after the type specification
 *                 - and return a pointer to the string
 */

char *_PD_member_type(char *s)
   {char *t, *p, c, bf[MAXLINE], *pt;

    strcpy(bf, s);

/* find a pointer to the last '*' in the string */
    for (p = bf, t = bf; (c = *t) != '\0'; t++)
        if (c == '*')
           p = t;

/* if there was a '*' replace the next character with a '\0' */
    if (p != bf)
       *(++p) = '\0';

/* otherwise the type is not a pointer so return the first token */
    else
       SC_strtok(bf, " \t\n\r", pt);

    return(CSTRSAVE(bf));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MEMBER_BASE_TYPE - extract the base type (no indirections) of the
 *                      - given member and return a copy of it
 */

char *_PD_member_base_type(char *s)
   {char bf[MAXLINE];
    char *token, *p;

    strcpy(bf, s);
    token = SC_strtok(bf, " *", p);

    p = CSTRSAVE(token);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_HYPER_TYPE - adjust type for dimension expression dereferences */

/* GOTCHA: This function is currently a no-op; it returns the original type */

char *_PD_hyper_type(char *name, char *type)
   {

    return(type);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MEMBER_NAME - given the member description extract the name and
 *                 - return a pointer to it
 *                 - new space is allocated for the name
 */

char *_PD_member_name(char *s)
   {char bf[MAXLINE], t[MAXLINE];
    char *pt, *token, *p;

    strcpy(bf, s);

/* shave off the type part */
    token = SC_strtok(bf, " *(", p);
    token = SC_strtok(NULL, "\n", p);

    if (token != NULL)
       {strcpy(t, token);

/* creep up to the name */
	for (pt = t; strchr(" \t*(", *pt) != NULL; pt++);

/* get the name without any dimension info */
	token = SC_strtok(pt, "()[", p);

	p = CSTRSAVE(token);}
    else
       p = NULL;

    return(p);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_VAR_NAME - given the variable name with dimensions extract the 
 *              - variable name and return a pointer to it
 *              - new space is allocated for the name
 */

char *_PD_var_name(char *s)
   {char bf[MAXLINE];
    char *token, *p;

    strcpy(bf, s);
    token = SC_strtok(bf, "([", p);

    p = CSTRSAVE(token);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_VAR_NAMEF - given the variable name with dimensions extract the 
 *               - variable name and return a pointer to it
 */

char *_PD_var_namef(PDBfile *file, char *name, char *bf, int nc)
   {int n;
    char *p, *s;

    if (file == NULL)
       SC_strncpy(bf, nc, name, -1);
    else
       SC_strncpy(bf, nc, _PD_fixname(file, name), -1);

/* NOTE: the code worked like this for many years
 *    s = SC_strtok(bf, ".([", p);
 * but PD_put_set came up with names of the form:
 *    {t=0!00,a(<stuff>)}
 * which does contain () but is not an array reference.
 * So change the logic to look for '[/]{..}'
 */
    n = strlen(bf);
    if ((strchr(bf, '{') != NULL) && (bf[n-1] == '}'))
       s = bf;
    else
       s = SC_strtok(bf, ".([", p);

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_EX_DIMS - extract the dimension information from the given string
 *             - return dimensions interpreted as definition
 *             - e.g., x(i) interpreted as x(defoff:defoff+i-1)
 *             - if there are no dimensions or bad dimensions, return NULL
 *             - to distinguish between the absence of an index expression
 *             - and the presence of one implying 0 length return TRUE
 *             - in PDE iff there is an index expression
 */

dimdes *_PD_ex_dims(char *memb, int defoff, int *pde)
   {int ne;
    long mini, leng;
    char bf[MAXLINE];
    char *token, *maxs;
    dimdes *dims, *next, *prev;

    prev = NULL;
    dims = NULL;
    strcpy(bf, memb);
    token = SC_firsttok(bf, "([\001\n");
    for (ne = 0; 
	 (token = SC_firsttok(bf, ",)[] ")) != NULL;
	 ne++)
        {maxs = strchr(token, ':');
	 if (maxs != NULL)
            {*maxs++ = '\0';
	     mini = atol(token);
	     leng = atol(maxs) - atol(token) + 1L;}
	 else
            {mini = defoff;
	     leng = atol(token);};

	 if (leng <= 0L)
	    {*pde = TRUE;
	     _PD_rl_dimensions(dims);
	     return(NULL);};

	 next = _PD_mk_dimensions(mini, leng);

	 if (dims == NULL)
            dims = next;
	 else
            {prev->next = next;
	     SC_mark(next, 1);};

	 prev = next;};

    *pde = (ne != 0);

    return(dims);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_ADJ_DIMENSIONS - adjust dimension expressions in name for append.
 *                    - for now, indexes for all dimensions must include
 *                    - min and max (stride optional) and must match the
 *                    - post-append dimensions. All but the most slowly
 *                    - varying index must match the pre-append dimensions.
 *                    - because the pre-append dimensions are already known
 *                    - the above conditions on the dimensions not changing
 *                    - may be relaxed in the future for user convenience
 *                    - or to support partial writes in appends.
 */

int _PD_adj_dimensions(PDBfile *file, char *name, syment *ep)
   {int id;
    long i;
    inti imin, imax, istep;
    char head[MAXLINE], expr[MAXLINE], tail[MAXLINE], bf[MAXLINE];
    char expr2[MAXLINE];
    char *token, *smax, *sinc;
    dimdes *dims;
    
    expr[0]  = '\0';
    expr2[0] = '\0';
    dims     = ep->dimensions;

    strcpy(bf, name);
    SC_strncpy(head, MAXLINE, SC_firsttok(bf, "([\001\n"), -1);
    tail[0] = '\0';

    for (id = 0; (token = SC_firsttok(bf, ",)] ")) != NULL; id++)
        {if (token[0] == '.')
	    {strcpy(tail, token);
	     break;};
	 smax = strchr(token, ':');
	 if (smax == NULL)
 	    PD_error("MAXIMUM INDEX MISSING - _PD_ADJ_DIMENSIONS", PD_WRITE);

	 *smax++ = '\0';
	 sinc = strchr(smax, ':');
	 if (sinc != NULL)
	    *sinc++ = '\0';

	 imin = atol(token);
	 imax = atol(smax);
	 if (sinc != NULL)
	    istep = atol(sinc);
	 else
	    istep = 1;

	 if (imin == file->default_offset)
	    {if (((file->major_order == ROW_MAJOR_ORDER) &&
		  (dims == ep->dimensions)) ||
		 ((file->major_order == COLUMN_MAJOR_ORDER) &&
		  (dims->next == NULL)))
	        {i = dims->index_max + 1 - imin;
		 imin += i;
		 imax += i;};};

	 snprintf(expr, MAXLINE, "%s%lld:%lld:%lld,",
		  expr2, (long long) imin, (long long) imax,
		  (long long) istep);
	 strcpy(expr2, expr);
	 dims = dims->next;};

    if (expr[0] != '\0')
       {if (strchr(head, '.') != NULL)
	   PD_error("APPEND TO TOP LEVEL OF STRUCT ONLY - _PD_ADJ_DIMENSIONS",
		    PD_WRITE);
	SC_LAST_CHAR(expr) = '\0';
	snprintf(name, MAXLINE, "%s[%s]%s", head, expr, tail);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MEMBER_ITEMS - return the total number of items as specified by
 *                  - the dimensions in the given member
 *                  - return -1 on error
 */

long _PD_member_items(char *s)
   {char *token, bf[MAXLINE], *t;
    long acc;

    strcpy(bf, s);
    token = SC_strtok(bf, "(\001\n", t);
    acc = 1L;
    while ((token = SC_strtok(NULL, ",) ", t)) != NULL)
       if ((acc *= atol(token)) <= 0)
          return(-1L);

    return(acc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_COMP_NUM - compute the number of elements implied by the dimensions
 *              - of a variable
 */

long _PD_comp_num(dimdes *dims)
   {long acc;
    dimdes *lst;

    for (acc = 1L, lst = dims; lst != NULL; lst = lst->next)
        {acc *= (long) (lst->number);};

    return(acc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_COMP_NIND - compute the number of elements implied by the an
 *               - index array of the type used by the _ALT type functions
 */

long _PD_comp_nind(int nd, long *ind, int str)
   {int i;
    long acc, start, stop, step;

    if (str < 3)
       {for (acc = 1L, i = 0; i < nd; i++)
	    {start = ind[0];
	     stop  = ind[1];
	     ind  += str;
	     acc  *= (stop - start + 1L);};}
    else
       {for (acc = 1L, i = 0; i < nd; i++)
	    {start = ind[0];
	     stop  = ind[1];
	     step  = ind[2];
	     ind  += str;
	     acc  *= (stop - start + step)/step;};};

    return(acc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_STR_SIZE - compute sizeof for the defstruct specified
 *              - return -1 iff some member of the struct is undefined
 */

long _PD_str_size(memdes *str, hasharr *tab)
   {int align, al_max, lal;
    long i, sz, number;
    memdes *desc;

    sz     = 0L;
    al_max = 0;

#ifdef AIX
    {defstr *dp;

     dp = PD_inquire_table_type(tab, str->type);

     if ((dp != NULL) && (dp->members == NULL))
        al_max = _PD_lookup_size(str->type, tab);};
#endif

    for (desc = str; desc != NULL; desc = desc->next)
        {number = desc->number;
         i = _PD_lookup_size(desc->type, tab);
         if (i == -1L)
            return(-1L);

         align = _PD_align(sz, desc->type, desc->is_indirect, tab, &lal);
         if (align == -1)
            return(-1L);

         al_max = max(al_max, lal);

         desc->member_offs = sz + align;

         sz += align + i*number;};

    if (al_max != 0)
       {i  = (sz + al_max - 1)/al_max;
        sz = al_max*i;};

    return(sz);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_ALIGN - return the number of bytes needed to put an object of TYPE
 *           - on the proper byte boundary
 */

int _PD_align(long n, char *type, int is_indirect, hasharr *tab, int *palign)
   {long offset, align, nword;
    defstr *dp;

    if (type == NULL)
       {*palign = 0;
        return(0);};

    if (is_indirect)
       dp = PD_inquire_table_type(tab, "*");
    else
       dp = PD_inquire_table_type(tab, type);

    if (dp == NULL)
       {*palign = -1;
        return(-1);}
    else
       align = dp->alignment;

    if (align != 0)
       {nword  = (n + align - 1)/align;
        offset = align*nword - n;}
    else
       offset = 0;

    *palign = align;

    return(offset);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_LOOKUP_TYPE - look up the type given in structure chart and
 *                 - return the defstr
 */

static defstr *_PD_lookup_type(char *s, hasharr *tab)
   {char bf[MAXLINE];
    char *token, *t;
    defstr *dp;

/* if it's a POINTER handle it now */
    if (strchr(s, '*') != NULL)
       strcpy(bf, "*");
    else
       strcpy(bf, s);

    token = SC_strtok(bf, " ", t);
    dp    = PD_inquire_table_type(tab, token);

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_TYPE_LOOKUP - look up the type given in structure chart and
 *                 - return the defstr
 */

defstr *_PD_type_lookup(PDBfile *file, PD_chart_kind ch, char *s)
   {defstr *dp;

    switch (ch)
       {case PD_CHART_FILE :

#ifdef USE_REQUESTS
	     io_request *req;

             req = &file->req;
	     if ((req->base_type != NULL) && 
		 (strcmp(s, req->base_type) == 0) &&
		 (req->ftype != NULL))
	        dp = file->req.ftype;
	     else
	        {dp = _PD_lookup_type(s, file->chart);
		 SC_mark(file->req.ftype, -1);
		 file->req.ftype = dp;
		 SC_mark(dp, 1);};
#else
	     dp = _PD_lookup_type(s, file->chart);
#endif
	     break;

	case PD_CHART_HOST :

#ifdef USE_REQUESTS
	     io_request *req;

             req = &file->req;
	     if ((req->base_type != NULL) && 
		 (strcmp(s, req->base_type) == 0) &&
		 (req->htype != NULL))
	        dp = file->req.htype;
	     else
	        {dp = _PD_lookup_type(s, file->host_chart);
		 SC_mark(file->req.htype, -1);
		 file->req.htype = dp;
		 SC_mark(dp, 1);};
#else
	     dp = _PD_lookup_type(s, file->host_chart);
#endif
	     break;

	default :
	     dp = NULL;
	     break;};

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_LOOKUP_SIZE - THREADSAFE
 *                 - look up the type given in structure chart and
 *                 - return the size
 */

long _PD_lookup_size(char *s, hasharr *tab)
   {long bpi;
    char *token, bf[MAXLINE], *t;
    defstr *dp;
    multides *tuple;

    bpi = -1L;

/* if it's a POINTER handle it now */
    if (strchr(s, '*') != NULL)
       strcpy(bf, "*");
    else
       strcpy(bf, s);

    token = SC_strtok(bf, " ", t);
    dp    = PD_inquire_table_type(tab, token);
    if (dp != NULL)
       {bpi   = dp->size;
	tuple = dp->tuple;
	if (tuple != NULL)
	   bpi *= tuple->ni;};

    return(bpi);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_MEMBER_LOCATION - return the byte offset (0 based) of the given
 *                     - member from the beginning of the struct
 *                     - don't get rid of this (some applications use it!)
 */

int64_t _PD_member_location(char *s, hasharr *tab, defstr *dp, memdes **pdesc)
   {int64_t addr;
    char name[MAXLINE];
    char *token;
    memdes *desc, *nxt;

    if ((s != NULL) && (tab != NULL) && (dp != NULL))
       {strcpy(name, s);
	token = SC_firsttok(name, ".\001");

	for (addr = 0, desc = dp->members; desc != NULL; desc = nxt)
	    {nxt = desc->next;
	     if ((token != NULL) && (strcmp(desc->name, token) == 0))
	        {addr  += desc->member_offs;
		 *pdesc = desc;
		 dp = PD_inquire_table_type(tab, desc->base_type);
		 if (dp != NULL)
		    {token = SC_firsttok(name, ".\001");
		     if (token == NULL)
		        return(addr);
		     else
		        nxt = dp->members;};};};};

    return(-1);}
                 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_INQUIRE_SYMBOL - look up the entry for the named quantity
 *
 * #bind PD_inquire_symbol fortran() scheme() python()
 */

haelem *PD_inquire_symbol(PDBfile *file ARG(,,cls),
			  char *name, int flag,
			  char *fullname, hasharr *tab)
   {char s[MAXLINE], t[MAXLINE];
    haelem *hp;

    hp = NULL;
    if (name != NULL)
       {if (flag)
	   strcpy(s, _PD_fixname(file, name));
	else
	   strcpy(s, name);

	if (fullname != NULL)
	   strcpy(fullname, s);

	hp = SC_hasharr_lookup(tab, s);
	if (hp == NULL)

/* check case where S begins with '/'
 * it might be under 'foo' rather than '/foo'
 */
	   {if (*s == '/')
	       {if ((s[1] != '\0') && (s[1] != '&'))
		   {snprintf(t, MAXLINE, "%s", s+1);
		    hp = SC_hasharr_lookup(tab, t);};}

/* check case where S does not begin with '/'
 * it might be under '/foo' rather than 'foo'
 */
	    else
	       {snprintf(t, MAXLINE, "/%s", s);
		hp = SC_hasharr_lookup(tab, t);};};};

    return(hp);}
	  
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_QUERY_ENTRY - look up the symbol table entry for the named quantity
 *
 * #bind PD_query_entry fortran() scheme() python()
 */

syment *PD_query_entry(PDBfile *file ARG(,,cls), char *name, char *fullname)
   {haelem *hp;
    syment *ep;

    hp = PD_inquire_symbol(file, name, TRUE, fullname, file->symtab);
    ep = (hp == NULL) ? NULL : (syment *) hp->def;

    return(ep);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_INQUIRE_ENTRY - look up the symbol table entry for the named quantity
 *
 * #bind PD_inquire_entry fortran() scheme() python()
 */

syment *PD_inquire_entry(PDBfile *file ARG(,,cls), char *name,
			 int flag, char *fullname)
   {haelem *hp;
    syment *ep;

    hp = PD_inquire_symbol(file, name, flag, fullname, file->symtab);
    ep = (hp == NULL) ? NULL : (syment *) hp->def;

    return(ep);}
  
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_EFFECTIVE_ENTRY - look up the effective symbol table entry
 *                    - for the named quantity which might be of the form
 *                    -     a.b.c
 *                    -     a->b
 *                    - and all other possibilities
 *
 * #bind PD_effective_entry fortran() scheme() python()
 */

syment *PD_effective_entry(PDBfile *file ARG(,,cls), char *name,
			   int flag, char *fullname)
   {syment *ep;

    ep = _PD_effective_ep(file, name, flag, fullname);
    
    return(ep);}
	  
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_ROW_MAJOR_EXPR - compute and return an index expression given a
 *                    - string BF to store it in, a list PD containing the
 *                    - limits of each dimension, and an offset index,
 *                    - INDX, which is to be converted
 *                    - assume ROW_MAJOR_ORDER
 */

static char *_PD_row_major_expr(char *bf, dimdes *pd, inti indx, int def_off)
   {char tmp[MAXLINE];
    inti ix, m, stride;
    dimdes *pt;

    if (pd == NULL)
       sprintf(bf, "%lld", (long long) (indx + def_off));

    else
       {bf[0] = '\0';

        stride = 1L;
        for (pt = pd; pt != NULL; pt = pt->next)
            stride *= pt->number;

        while (pd != NULL)
           {stride /= pd->number;

            m  = indx / stride;
            ix = m + pd->index_min;

            snprintf(tmp, MAXLINE, "%lld,", (long long) ix);
            strcat(bf, tmp);

            indx -= m*stride;
            pd    = pd->next;};

        SC_LAST_CHAR(bf) = '\0';};

    return(bf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_COL_MAJOR_EXPR - compute and return an index expression given a
 *                    - string BF to store it in, a list PD containing the
 *                    - limits of each dimension, and an offset index,
 *                    - INDX, which is to be converted
 *                    - assume COLUMN_MAJOR_ORDER
 */

static char *_PD_col_major_expr(char *bf, dimdes *pd, inti indx, int def_off)
   {char tmp[MAXLINE];
    inti ix, m, stride;

    if (pd == NULL)
       sprintf(bf, "%lld", (long long) (indx + def_off));
    else
       {bf[0] = '\0';

        while (pd != NULL)
           {stride = pd->number;
            m  = indx - (indx/stride)*stride;
            ix = m + pd->index_min;
            snprintf(tmp, MAXLINE, "%lld,", (long long) ix);
            strcat(bf, tmp);

            indx = (indx - m)/stride;
            pd = pd->next;};

        SC_LAST_CHAR(bf) = '\0';};

    return(bf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_INDEX_TO_EXPR - convert a linear index into
 *                  - an ASCII index expression
 *
 * #bind PD_index_to_expr fortran() scheme() python()
 */

char *PD_index_to_expr(char *bf, long indx, dimdes *dim,
		       int major_order, int def_off)
   {char *p;

    bf[0] = '\0';

    p = NULL;
    if (major_order == COLUMN_MAJOR_ORDER)
       p = _PD_col_major_expr(bf, dim, indx, def_off);

    else if (major_order == ROW_MAJOR_ORDER)
       p = _PD_row_major_expr(bf, dim, indx, def_off);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

