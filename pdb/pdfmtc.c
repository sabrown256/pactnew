/*
 * PDFMTC.C - PDB III format methods
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

/* PDBFile III Layout:
 *       
 *       Data
 *       Structure Chart
 *       Symbol Table
 *       Extensions
 *       Structure Chart Address
 *       Symbol Table Address
 *       Identifying Token
 *       EOF
 */

#define SYMBOL_TABLE_TAG     "\nSymbolTable:\n"
#define PRIMITIVE_TYPE_TAG   "\nPrimitiveTypes:\n"
#define COMPOUND_TYPE_TAG    "\nCompoundTypes:\n"

/*--------------------------------------------------------------------------*/

/*                               ITAG ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* _PD_WR_ITAG_III - format and write an itag to S */

static int _PD_wr_itag_iii(char *s, int nc, const char *type, int64_t ni,
			   int64_t addr, PD_data_location loc)
   {int rv;
    char t[2][MAXLINE];

    SC_itos(t[0], MAXLINE, ni, NULL);
    SC_itos(t[1], MAXLINE, addr, "%32lld");

    rv = snprintf(s, MAXLINE, "\n%s(%s) %32s %d;\n",
		  type, t[0], t[1], loc);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_ITAG_III - fill an itag from the file */

static char **_PD_rd_itag_iii(const char *bf)
   {int i, na;
    char a[MAXLINE];
    char **ta;

    ta = PS_tokenize(bf, "() \n", 0);

/* check for mis-parse of "char *" into "char" and "*" */
    if (ta[1][0] == '*')
       {na = PS_lst_length(ta);
	snprintf(a, MAXLINE, "%s %s", ta[0], ta[1]);
	CFREE(ta[0]);
	CFREE(ta[1]);
	ta[0] = CSTRSAVE(a);

/* move the other strings down the list */
	for (i = 2; i <= na; i++)
	    ta[i-1] = ta[i];};

    return(ta);}

/*--------------------------------------------------------------------------*/

/*                               READ ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* _PD_PRIM_TYPE_III - save the number of bytes NB, aligment AL, byte
 *                   - order info, FLG and ORDR, and floating point format
 *                   - info, FORMT for TYPE in the
 *                   - data_alignment member of FILE
 */

static void _PD_prim_type_iii(PDBfile *file, char *type, int nb, int al,
			      int flg, int *ordr, long *formt)
   {int id, ic, ifx, ifp, icx;
    PD_text_kind ut;
    data_standard *std;
    data_alignment *align;

    std   = file->std;
    align = file->align;
    id    = SC_type_id(type, FALSE);

/* check for character types (proper) */
    if (SC_is_type_char(id) == TRUE)
       {ic = SC_TYPE_CHAR(id);
	if (ic < N_PRIMITIVE_CHAR)
	   {switch (nb)
	       {case 1 :
		     ut = UTF_8;
		     break;
		case 2 :
		     ut = UTF_16;
		     break;
		case 4 :
		     ut = UTF_32;
		     break;
		default :
		     ut = UTF_NONE;
		     break;};
	    std->chr[ic].bpi = nb;
	    std->chr[ic].utf = ut;
	    align->chr[ic]   = al;};}

/* check for fixed point types (proper) */
    else if (SC_is_type_fix(id) == TRUE)
       {ifx = SC_TYPE_FIX(id);
	if (ifx < N_PRIMITIVE_FIX)
	   {std->fx[ifx].bpi   = nb;
	    std->fx[ifx].order = (PD_byte_order) flg;
	    align->fx[ifx]     = al;};}

/* check for floating point types (proper) */
    else if (SC_is_type_fp(id) == TRUE)
       {ifp = SC_TYPE_FP(id);
	if ((ifp < N_PRIMITIVE_FP) && (std->fp[ifp].format == NULL))
	   {std->fp[ifp].bpi    = nb;
	    std->fp[ifp].order  = ordr;
	    std->fp[ifp].format = formt;
	    align->fp[ifp]      = al;

	    SC_mark(ordr, 1);
	    SC_mark(formt, 1);};}

/* check for complex floating point types (proper) */
    else if (SC_is_type_cx(id) == TRUE)
       {icx = SC_TYPE_CPX(id);
	if (icx < N_PRIMITIVE_CPX)
	   {
/*
            std->fp[icx].bpi    = nb;
	    std->fp[icx].order  = ordr;
	    std->fp[icx].format = formt;
	    align->fp[icx]      = al;

	    SC_mark(ordr, 1);
	    SC_mark(formt, 1);
 */
	    };}

    else if (strcmp(type, "*") == 0)
       {std->ptr_bytes       = nb;
	align->ptr_alignment = al;}

    else if (id == G_STRUCT_I)
       align->struct_alignment = al;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_PRIM_TYP_III - read the primitive types from the extras table */

static int _PD_rd_prim_typ_iii(PDBfile *file, char *bf)
   {int l, it, ni, align;
    int unsgned, onescmp, conv;
    int *ordr, *aord;
    intb i, bpi, bsz;
    long *formt;
    char *token, *type, *origtype, *atype, *s, *local, **sa;
    multides *tuple;
    SC_kind kind;
    PD_byte_order ord, ordo;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    local = pa->local;
    bsz   = sizeof(pa->local);

    if (bf != NULL)
       _PD_get_token(bf, local, bsz, '\n');

    file->std   = _PD_mk_standard(file);
    file->align = _PD_copy_alignment(&WORD4_ALIGNMENT);

/* initialize the pdb system defs and structure chart */
    _PD_init_chrt(file, FALSE);

    for (l = 0; _PD_get_token(NULL, local, bsz, '\n'); l++)
        {if (*local == '\0')
            break;

	 ord      = NO_ORDER;
	 ordo     = NO_ORDER;
	 ordr     = NULL;
	 formt    = NULL;
	 unsgned  = FALSE;
	 onescmp  = FALSE;
	 conv     = TRUE;
	 kind     = KIND_OTHER;
	 tuple    = NULL;
	 atype    = NULL;
	 origtype = NULL;

	 sa = PS_tokenize(local, "(,;|) \t\n", 0);
	 it = 0;

	 type  = _PD_cat_type(sa, &it);
	 bpi   = SC_stol(sa[it++]);
	 align = SC_stol(sa[it++]);
	 for (; sa[it] != NULL; )
	     {token = sa[it++];
	      if (strncmp(token, "ORDER", 5) == 0)
	         {ordr = CMAKE_N(int, bpi);
		  for (i = 0L; i < bpi; i++)
		      {token = sa[it++];
		       if (strcmp(token, "text") == 0)
		          {for (i = 0L; i < bpi; i++)
			       ordr[i] = i + 1;
			   ordo = TEXT_ORDER;
			   break;}
		       else if (strcmp(token, "big") == 0)
		          {for (i = 0L; i < bpi; i++)
			       ordr[i] = i + 1;
			   ordo = NORMAL_ORDER;
			   break;}
		       else if (strcmp(token, "little") == 0)
		          {for (i = 0L; i < bpi; i++)
			       ordr[i] = bpi - i;
			   ordo = REVERSE_ORDER;
			   break;}
		       else
		          {ordr[i] = SC_stol(token);
			   ordo = SPEC_ORDER;};};}
                    
	      else if (strncmp(token, "FIX", 3) == 0)
	         {CFREE(ordr);
		  ord  = ordo;
		  kind = KIND_INT;}
                    
	      else if (strncmp(token, "FLOAT", 5) == 0)
	         {formt = CMAKE_N(long, 8);
		  for (i = 0L; i < 8; i++)
		      formt[i] = SC_stol(sa[it++]);
		  kind = KIND_FLOAT;}

	      else if (strncmp(token, "TUPLE", 5) == 0)
	         {atype = _PD_cat_type(sa, &it);
		  ni    = SC_stol(sa[it++]);
		  aord  = CMAKE_N(int, ni);
		  for (i = 0L; i < ni; i++)
		      {aord[i] = SC_stol(sa[it++]);
		       if (aord[i] == -1)
			  break;};
		  if (aord[0] == -1)
		     CFREE(aord);
		  tuple = _PD_make_tuple(atype, ni, aord);}

	      else if (strncmp(token, "CHAR", 4) == 0)
	         kind = KIND_CHAR;
                    
	      else if (strncmp(token, "NO-CONV", 7) == 0)
	         {kind = KIND_OTHER;
		  conv = FALSE;}

	      else if (strncmp(token, "UNSGNED", 7) == 0)
                 unsgned = TRUE;

	      else if (strncmp(token, "ONESCMP", 7) == 0)
                 onescmp = TRUE;

	      else if (strncmp(token, "TYPEDEF", 7) == 0)
	         origtype = _PD_cat_type(sa, &it);};

	 _PD_prim_type_iii(file, type, bpi, align, ord, ordr, formt);

	 _PD_defstr_prim_rd(file, type, origtype, kind,
			    tuple, bpi, align, ord,
			    ordr, formt, unsgned, onescmp, conv);

/* if these have a reference do not free them */
	 if ((origtype != NULL)  && (SC_ref_count(formt) == 0))
	    {CFREE(ordr);
	     CFREE(formt);};

	 PS_free_strings(sa);

	 CFREE(atype);
	 CFREE(origtype);
	 CFREE(type);};

/* get global type qualifier information */

/* GOTCHA: this was already initialized in _PD_mk_pdb
    file->default_offset = 0;
*/

    while (_PD_get_token(NULL, local, bsz, '\n'))
       {if (*local == '\0')
           break;

        token = SC_strtok(local, " \t:", s);

	if ((token != NULL) && (strcmp(token, "DefaultIndexOffset") == 0))
	   {token = SC_strtok(NULL, " \n", s);
	    if (token != NULL)
	       file->default_offset = SC_stoi(token);}

	else if ((token != NULL) && (strcmp(token, "StructAlignment") == 0))
	   {token = SC_strtok(NULL, " \n", s);
	    if (token != NULL)
	       {align = SC_stoi(token);
		_PD_prim_type_iii(file, "struct", 0, align, 0, NULL, NULL);};}

	else if ((token != NULL) && (strcmp(token, "MajorOrder") == 0))
	   {token = SC_strtok(NULL, " \n", s);
	    if (token != NULL)
	       {if (strcmp(token, "row") == 0)
		   file->major_order = ROW_MAJOR_ORDER;
		else if (strcmp(token, "column") == 0)
		   file->major_order = COLUMN_MAJOR_ORDER;};};};

    pa->has_dirs = TRUE;
    file->current_prefix = CSTRSAVE("/");

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_CHRT_III - read the structure chart from the PDB file into
 *                 - the internal structure chart
 *                 - return TRUE if successful
 *                 - return FALSE on error
 */

static int _PD_rd_chrt_iii(PDBfile *file)
   {long sz, ncast, icast, bsz;
    u_int64_t nbc;
    char **pl;
    char type[MAXLINE];
    char *nxt, *bf, *p, *local, *member, *lmod;
    memdes *desc, *lst, *prev;
    FILE *fp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    fp = file->stream;

/* read the entire chart into memory to speed processing */
    nbc         = file->symtaddr - file->chrtaddr + 1;
    pa->tbuffer = CMAKE_N(char, nbc);

    local = pa->local;
    bsz   = sizeof(pa->local);
    bf    = pa->tbuffer;

    if (lio_read(bf, 1, nbc, fp) != nbc)
       return(FALSE);

    bf[nbc-1] = (char) EOF;

    if (strncmp(bf, PRIMITIVE_TYPE_TAG, strlen(PRIMITIVE_TYPE_TAG)) != 0)
       return(FALSE);

    bf++;

    _PD_get_token(bf, local, bsz, '\n');

/* read in the primitives */
    _PD_rd_prim_typ_iii(file, NULL);

    _PD_get_token(NULL, local, MAXLINE, '\n');
    if (strcmp(local, "CompoundTypes:") != 0)
       return(FALSE);

/* initialize cast data container */
    pa->n_casts = 0L;
    icast = 0L;
    ncast = N_CASTS_INCR;
    pl    = CMAKE_N(char *, N_CASTS_INCR);

    prev = NULL;
    while (_PD_get_token(NULL, local, MAXLINE, '\n'))
       {if (local[0] == '\0')
           break;

	nxt = SC_strtok(local, " \t", p);
	if (nxt != NULL)
	   SC_strncpy(type, MAXLINE, nxt, -1);

        nxt = SC_strtok(NULL, " \t()", p);
	if (nxt != NULL)
	   {sz = SC_stol(nxt);
	    SC_ASSERT(sz != 0);};

	nxt = SC_strtok(NULL, " \t\n", p);

	lst = NULL;
	if ((nxt == NULL) || (*nxt != ';'))
	   {while (_PD_get_token(NULL, local, MAXLINE, '\n'))
	       {if (*local == '\0')
		   break;

		nxt = strchr(local, '{');
		if (nxt == NULL)
		   nxt = local;
		else
		   nxt++;

		member = SC_strtok(nxt, "<;\n", p);
		lmod   = SC_strtok(NULL, ";\n", p);
		nxt    = SC_strtok(NULL, ";", p);

		member = SC_trim_right(member, " \t");
		member = SC_trim_left(member, " \t");
		if (lmod != NULL)
		   lmod = SC_trim_left(lmod, "-= \t");

		desc = _PD_mk_descriptor(member, file->default_offset);
		if ((lmod == NULL) ||  (*lmod == '}'))
		   nxt = lmod;
		else
		   {pl[icast]   = CSTRSAVE(type);
		    pl[icast+1] = CSTRSAVE(member);
		    pl[icast+2] = CSTRSAVE(lmod);
		    icast += 3;
		    if (icast >= ncast)
		       {ncast += N_CASTS_INCR;
			CREMAKE(pl, char *, ncast);};};

		if (lst == NULL)
		   lst = desc;
		else
		   prev->next = desc;
		prev = desc;

		if ((nxt != NULL) && (*nxt == '}'))
		   break;};};

/* install the type in both charts */
        _PD_defstr_inst(file, type, KIND_STRUCT, lst,
			NO_ORDER, NULL, NULL, PD_CHART_HOST);};

    pa->cast_lst = pl;
    pa->n_casts  = icast;

    _PD_cast_check(file);

    CFREE(pa->tbuffer);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PARSE_SYMT_III - process the asciii representation of a PDBfile
 *                    - symbol table, installing it into the in memory
 *                    - binary version
 *
 */

static int _PD_parse_symt_iii(PDBfile *file, char *buf, int flag,
			      char *acc, char *rej)
   {int ok;
    long bsz, nc;
    int64_t addr, numb;
    char *name, *type, *var, *adr, *s, *local;
    char *base;
    syment *ep;
    memdes *desc;
    dimdes *dims;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    local = pa->local;
    bsz   = sizeof(pa->local);

    if (strncmp(buf, SYMBOL_TABLE_TAG, strlen(SYMBOL_TABLE_TAG)) != 0)
       return(FALSE);

    buf++;

    _PD_get_token(buf, local, bsz, '\n');

    base = file->ptr_base;
    nc   = strlen(base);

    while (_PD_get_token(NULL, local, bsz, ';'))
       {if (local[0] == '\0')
	   break;

	adr = strrchr(local, '@');
        if (adr != NULL)
	   {*adr++ = '\0';
	    var = local;}
	else
	   break;

	desc = _PD_mk_descriptor(var, file->default_offset);
	if (desc != NULL)
	   {name = SC_strtok(desc->name, " \t", s);
	    if (name != NULL)
	       {type = desc->type;

/* check to see whether or not so skip this entry */
		ok = _PD_symatch(file, TRUE, name, type, acc, rej);
		if (ok == TRUE)
		   {dims = desc->dimensions;

		    addr = SC_stol(SC_strtok(adr, " \t", s));
		    numb = SC_stol(SC_strtok(NULL, " \t()", s));

/* make and install the entry in the symbol table */
		    ep = _PD_mk_syment(type, numb, addr, NULL, dims);
		    _PD_e_install(file, name, ep, flag);

/* if it is a pointer register it in the pointer array */
		    if (strncmp(name, base, nc) == 0)
		       _PD_ptr_register_entry(file, name, ep);};};

	    _PD_rl_descriptor(desc);};

/* end of expression */
	_PD_get_token(NULL, local, bsz, '\n');};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_SYMT_III - read the symbol table from the PDB file into a
 *                 - hash table of the PDB for future lookup
 *                 - return TRUE if successful
 *                 - return FALSE on error
 */

static int _PD_rd_symt_iii(PDBfile *file, char *acc, char *rej)
   {int rv, alloc;
    long nbs;
    int64_t addr, numb;
    char *bf;
    FILE *fp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    fp = file->stream;

/* find the overall file length */
    addr = _PD_get_current_address(file, PD_OPEN);

    alloc = (file->bfsz == 0);
    if (alloc == TRUE)
       {_PD_set_current_address(file, 0, SEEK_END, PD_OPEN);
	numb       = _PD_get_current_address(file, PD_OPEN);
	file->bfsz = numb - file->symtaddr;};

    nbs = file->bfsz;
    bf  = CMAKE_N(char, nbs + 1);

    pa->tbuffer = bf;

/* read in the symbol table and extras table as a single block */
    _PD_set_current_address(file, file->symtaddr, SEEK_SET, PD_OPEN);
    numb = lio_read(bf, 1, nbs, fp);
    if (numb != nbs)
       return(FALSE);

    bf[nbs] = (char) EOF;

    rv = _PD_parse_symt_iii(file, bf, FALSE, acc, rej);

    _PD_set_current_address(file, addr, SEEK_SET, PD_OPEN);

    if (alloc == FALSE)
       CFREE(pa->tbuffer);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_BLOCKS_III - read the Blocks extra */

static void _PD_rd_blocks_iii(PDBfile *file)
   {long j, n, nt, numb, stride, bsz;
    int64_t addr;
    char *name, *token, *s, *local;
    syment *ep;
    dimdes *dms;
    SC_array *bl;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    local = pa->local;
    bsz   = sizeof(pa->local);

    while (_PD_get_token(NULL, local, bsz, '\n'))
       {if (*local == '\0')
	   break;

	name = SC_strtok(local, " \n", s);
	n    = SC_stol(SC_strtok(NULL, " \n", s));
	ep   = PD_inquire_entry(file, name, FALSE, NULL);
	if (ep == NULL)
	   continue;

	bl = ep->blocks;
	nt = 0L;
	for (j = 0L; j < n; j++)
	    {token = SC_strtok(NULL, " \n", s);
	     if (token == NULL)
	        {_PD_get_token(NULL, local, bsz, '\n');
		 token = SC_strtok(local, " \n", s);};
		     
	     addr = SC_stol(token);
	     numb = SC_stol(SC_strtok(NULL, " \n", s));
                         
	     _PD_block_set_desc(addr, numb, bl, j);

	     nt += numb;};

/* adjust the slowest varying dimension to reflect the entire entry */
	dms = PD_entry_dimensions(ep);
	if (dms != NULL)
	   {if (PD_get_major_order(file) == COLUMN_MAJOR_ORDER)
	       for ( ; dms->next != NULL; dms = dms->next);

	    stride = PD_entry_number(ep)/dms->number;
	    stride = nt/stride;

	    dms->number    = stride;
	    dms->index_max = dms->index_min + stride - 1L;};

/* adjust the number to reflect the entire entry */
	ep->number = nt;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_EXT_III - read any extraneous data from the file
 *                - this is essentially a place for expansion of the file
 */

static int _PD_rd_ext_iii(PDBfile *file)
   {long bsz;
    char *token, *p, *local;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    local = pa->local;
    bsz   = sizeof(pa->local);

    file->system_version = 0;
    CFREE(file->date);

/* read the default offset */
    while (_PD_get_token(NULL, local, bsz, '\n'))
       {token = SC_strtok(local, ":\n", p);
        if (token == NULL)
           break;

        if (strcmp(token, "Blocks") == 0)
	   _PD_rd_blocks_iii(file);

        else if (strcmp(token, "Checksums") == 0)
	   _PD_block_cksum_read(file);

        else if (strcmp(token, "DynamicSpaces") == 0)
           {token = SC_strtok(NULL, "\n", p);
            if (token != NULL)
	       file->n_dyn_spaces = SC_stol(token);}

        else if (strcmp(token, "UseItags") == 0)
           {token = SC_strtok(NULL, "\n", p);
            if (token != NULL)
	       file->use_itags = SC_stoi(token);}

        else if (strcmp(token, "PreviousFile") == 0)
           {token = SC_strtok(NULL, "\n", p);
            if (token != NULL)
               file->previous_file = CSTRSAVE(token);}

        else if (strcmp(token, "Version") == 0)
           {token = SC_strtok(NULL, " \t(", p);
            if (token != NULL)
               file->system_version = atoi(token);

            token = SC_strtok(NULL, "()\n", p);
            if (token != NULL)
               file->date = CSTRSAVE(token);};};

/* release the buffer which held both the symbol table and the extras */
    CFREE(pa->tbuffer);

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                               WRITE ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* _PD_WR_PRIM_TYP_III - write the primitive data types from the given
 *                     - structure chart in a form suitable to the
 *                     - extras table
 *                     - return TRUE iff successful
 */

static int _PD_wr_prim_typ_iii(FILE *fp, hasharr *tab)
   {int ni, ok, l, nofl, rofl;
    int *ordr;
    long i, j, n;
    long *formt;
    char *nm;
    defstr *dp;
    multides *tuple;

    ok = TRUE;
    l  = 0;

    ok &= _PD_put_string(l++, PRIMITIVE_TYPE_TAG);

    for (i = 0; SC_hasharr_next(tab, &i, &nm, NULL, (void **) &dp); i++)
        {if (dp->members != NULL)
	    continue;

/* NOTE: use nm instead of dp->type or PD_typedef's won't work */
	 ok &= _PD_put_string(l++, "   %-20s %6ld %3d  ",
			      nm,
			      dp->size,
			      dp->alignment);

/* write the byte order */
	 ordr = dp->fp.order;
	 if (ordr != NULL)
	    {n    = dp->size;
	     nofl = TRUE;
	     rofl = TRUE;
	     for (j = 0L; j < n; j++)
	         {nofl &= (ordr[j] == j);
		  rofl &= (ordr[j] == (n - j));};
	     if (nofl)
	        ok &= _PD_put_string(l++, "ORDER(big)");
	     else if (rofl)
	        ok &= _PD_put_string(l++, "ORDER(little)");
	     else
	        {ok &= _PD_put_string(l++, "ORDER(");
		 for (j = 0L; j < n; j++)
		     {if (j == 0)
			 ok &= _PD_put_string(l++, "%d", ordr[j]);
		      else
			 ok &= _PD_put_string(l++, ",%d", ordr[j]);};
		 ok &= _PD_put_string(l++, ")");};}

	 else if (dp->fix.order == TEXT_ORDER)
	    ok &= _PD_put_string(l++, "ORDER(text)");

	 else if (dp->fix.order == NORMAL_ORDER)
	    ok &= _PD_put_string(l++, "ORDER(big)");

	 else if (dp->fix.order == REVERSE_ORDER)
	    ok &= _PD_put_string(l++, "ORDER(little)");

	 else if (dp->fix.order == NO_ORDER)
	    ok &= _PD_put_string(l++, "NO-CONV");

/* write the floating point format */
	 formt = dp->fp.format;
	 switch (dp->kind)
	    {case KIND_CHAR :
	          ok &= _PD_put_string(l++, "|CHAR");
		  break;
	     case KIND_INT :
		  ok &= _PD_put_string(l++, "|FIX");
		  break;
	     case KIND_FLOAT :
		  if (formt != NULL)
		     {ok &= _PD_put_string(l++, "|FLOAT(");
		      for (j = 0L; j < 8; j++)
			  {if (j == 0)
			      ok &= _PD_put_string(l++, "%ld", formt[j]);
			   else
			      ok &= _PD_put_string(l++, ",%ld", formt[j]);};
		      ok &= _PD_put_string(l++, ")");};
		  break;
	     default :
		  break;};

/* write the tuple info */
	 tuple = dp->tuple;
	 if (tuple != NULL)
	    {ni   = tuple->ni;
	     ordr = tuple->order;
	     ok  &= _PD_put_string(l++, "|TUPLE(");
	     ok  &= _PD_put_string(l++, "%s,%d,", tuple->type, ni);
	     if (ordr == NULL)
	        ok &= _PD_put_string(l++, "-1");
	     else
	        {for (j = 0L; j < ni; j++)
		     {if (j == 0)
			 ok &= _PD_put_string(l++, "%d", ordr[j]);
		      else
			 ok &= _PD_put_string(l++, ",%d", ordr[j]);};};
	     ok &= _PD_put_string(l++, ")");};

/* write the unsgned flag */
	 if (dp->unsgned == TRUE)
	    ok &= _PD_put_string(l++, "|UNSGNED");

/* write the ones complement flag */
	 if (dp->onescmp == TRUE)
	    ok &= _PD_put_string(l++, "|ONESCMP");

/* write typedef typename
 * NOTE: typedefs are shallow copies to pre-existing defstrs
 */
	 if (strcmp(nm, dp->type) != 0)
	    ok &= _PD_put_string(l++, "|TYPEDEF(%s)", dp->type);

/* end of entry */
	 ok &= _PD_put_string(l++, ";\n");};

    _PD_put_string(l++, "\n");

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WR_CHRT_III - write out the structure chart into the PDB file
 *                 - return a disk address if successful
 *                 - return -1L on error
 */

static int64_t _PD_wr_chrt_iii(PDBfile *file, FILE *out, int wc)
   {int n, nmb, ok;
    long i;
    int64_t addr;
    char *bf, *nm;
    FILE *fp;
    hasharr *fc, *hc;
    defstr *fdp, *hdp;
    memdes *fdsc, *hdsc;
    data_alignment *al;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    fp   = file->stream;
    addr = _PD_get_current_address(file, PD_WRITE);
    if (addr == -1)
       return(-1);

    if (pa->tbuffer != NULL)
       _PD_put_string(-1, NULL);

    if (wc == 0)
       {fc = file->chart;
	al = file->align;}
    else
       {fc = file->host_chart;
	al = file->host_align;};

    hc = file->host_chart;

    ok = TRUE;
    n  = 1;

/* write out the primitives */
    ok &= _PD_rev_chrt(fc);

    ok &= _PD_wr_prim_typ_iii(fp, fc);

/* write the struct special alignment */
    ok &= _PD_put_string(n++, "StructAlignment: %d\n",
			 al->struct_alignment);

/* write the default offset */
    ok &= _PD_put_string(n++, "DefaultIndexOffset: %d\n", file->default_offset);

/* write out the major order */
    if (file->major_order == ROW_MAJOR_ORDER)
       ok &= _PD_put_string(n++, "MajorOrder: row\n");
    else
       ok &= _PD_put_string(n++, "MajorOrder: column\n");

    _PD_put_string(n++, COMPOUND_TYPE_TAG);

/* the hash array for the structure chart is one element long */
    for (i = 0; SC_hasharr_next(fc, &i, &nm, NULL, (void **) &fdp); i++)
        {hdp = PD_inquire_table_type(hc, nm);
	 if ((hdp != NULL) && (fdp->members != NULL))

/* NOTE: use nm instead of fdp->type or PD_typedef's will not work */
	    {_PD_put_string(n++, "   %s (%ld)", nm, fdp->size);

	     for (nmb = 0, fdsc = fdp->members, hdsc = hdp->members;
		  fdsc != NULL;
		  fdsc = fdsc->next, hdsc = hdsc->next, nmb++)
	         {if (nmb == 0)
		     _PD_put_string(n++, "\n      {");
		  else
		     _PD_put_string(n++, "\n       ");

		  _PD_put_string(n++, "%s", fdsc->member);

/* NOTE: only the host chart holds the cast so we had to scan down
 * both at the same time to get here
 */
		  if ((hdsc->cast_memb != NULL) ||
		      (hdsc->size_memb != NULL))
		     {_PD_put_string(n++, "\t<- ");

		      if (hdsc->cast_memb != NULL)
			 _PD_put_string(n++, "%s", hdsc->cast_memb);

		      if (hdsc->size_memb != NULL)
			 {inti is, ns;
			  char **sm;

			  sm = hdsc->size_memb;

			  SC_ptr_arr_len(ns, sm);
			  for (is = 0; is < ns; is++)
			      {if (is == 0)
				  _PD_put_string(n++, " [%s", sm[is]);
			       else
				  _PD_put_string(n++, ",%s", sm[is]);};
			  _PD_put_string(n++, "]");};};

		  _PD_put_string(n++, ";");};

	     if (nmb > 0)
	        _PD_put_string(n++, "}");
	     _PD_put_string(n++, ";\n");};};

/* restore the chart because this may be a PD_flush and more types
 * may be added later
 */
    ok &= _PD_rev_chrt(fc);

    bf = pa->tbuffer;

/* write the entire chart to the file now */
    if (bf != NULL)
       {if (out != fp)
	   lio_write(bf, 1, strlen(bf), out);
	else
	   lio_write(bf, 1, strlen(bf), fp);};

    _PD_put_string(-1, NULL);

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WR_BLOCKS_III - write out the blocks for all variables in FILE */

static int _PD_wr_blocks_iii(PDBfile *file)
   {int ok;
    long i, j, n;
    inti ni;
    int64_t addr;
    char t[2][MAXLINE];
    char *nm;
    syment *ep;
    SC_array *bl;

    ok = TRUE;

    ok &= _PD_put_string(1, "Blocks:\n");

    for (i = 0; SC_hasharr_next(file->symtab, &i, &nm, NULL, (void **) &ep); i++)
        {bl = ep->blocks;
	 n  = _PD_n_blocks(ep);
	 if (n > 1)
	    {ok &= _PD_put_string(1, "%s %ld\n   ", nm, n);

	     for (j = 0L; j < n; j++)
	         {if ((j > 0) && ((j % 50) == 0))
		     _PD_put_string(1, "\n   ");

		  _PD_block_get_desc(&addr, &ni, bl, j);

		  SC_itos(t[0], MAXLINE, addr, NULL);
		  SC_itos(t[1], MAXLINE, ni, NULL);
		  ok &= _PD_put_string(1, " %s %s", t[0], t[1]);};

	     ok &= _PD_put_string(1, "\n");};};

    ok &= _PD_put_string(1, "\n");

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WR_CKSUM_III - write out the checksums for blocks of
 *                  - all variables in FILE
 */

static int _PD_wr_cksum_iii(PDBfile *file)
   {int ok;
    long i;
    char *nm;
    syment *ep;

    ok = TRUE;

    if (file->cksum.use & PD_MD5_RW)
       {ok &= _PD_put_string(1, "Checksums:\n");

	for (i = 0; SC_hasharr_next(file->symtab, &i, &nm, NULL, (void **) &ep); i++)
	    _PD_block_cksum_write(file, ep, nm);

	ok &= _PD_put_string(1, "\n");};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WR_SYMT_III - write out the symbol table (a hash table) into the
 *                 - PDB file
 *                 - return the disk address if successful
 *                 - return -1L on error
 */

static int64_t _PD_wr_symt_iii(PDBfile *file)
   {int n, nd, flag, ok;
    long i, stride;
    inti nb, ni, nt, mn, mx;
    int64_t addr, ad;
    char t[2][MAXLINE];
    char *ty, *nm;
    syment *ep;
    dimdes *dms;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    if (file == NULL)
       return(-1);

    addr = _PD_get_current_address(file, PD_WRITE);
    if (addr == -1)
       return(-1);

    if (pa->tbuffer != NULL)
       _PD_put_string(-1, NULL);

    ok = TRUE;
    n  = 0;

    _PD_put_string(n++, SYMBOL_TABLE_TAG);

    for (i = 0; SC_hasharr_next(file->symtab, &i, &nm, NULL, (void **) &ep); i++)
        {if (ep == NULL)
	    continue;

	 nt = PD_entry_number(ep);
	 ty = PD_entry_type(ep);
	 ad = PD_entry_address(ep);
	 nb = _PD_entry_get_number(ep, 0);
	 if (nb == 0)
	    {if (_PD_n_blocks(ep) == 1)
	        nb = nt;
	     else
	        {PD_error("BAD BLOCK LIST - _PD_WR_SYMT_III", PD_GENERIC);
		 return(-1L);};};

	 _PD_put_string(n++, 
			"   %s %s",
			ty, nm, nb, (int64_t) ad);

/* adjust the slowest varying dimension to reflect only the first block */
	 flag = PD_get_major_order(file);
	 for (nd = 0, dms = PD_entry_dimensions(ep);
	      dms != NULL;
	      nd++, dms = dms->next)
	     {if ((flag == ROW_MAJOR_ORDER) ||
		  ((flag == COLUMN_MAJOR_ORDER) && (dms->next == NULL)))
		 {stride = nt/(dms->number);
		  stride = (stride == 0) ? 1 : stride;
		  ni     = nb/stride;
		  flag   = FALSE;}
	      else
		 ni = dms->number;

	      mn = dms->index_min;
	      mx = mn + ni - 1;
	      SC_itos(t[0], MAXLINE, mn, NULL);
	      SC_itos(t[1], MAXLINE, mx, NULL);

	      if (nd == 0)
		 _PD_put_string(n++, "[%s:%s", t[0], t[1]);
	      else
		 _PD_put_string(n++, ",%s:%s", t[0], t[1]);};

	 if (nd > 0)
	    _PD_put_string(n++, "]");

	 SC_itos(t[0], MAXLINE, ad, NULL);
	 SC_itos(t[1], MAXLINE, nb, NULL);
	 _PD_put_string(n++, 
			" @ %s (%s);\n", t[0], t[1]);};

/* pad an extra newline to mark the end of the symbol table for _PD_rd_symt */
    _PD_put_string(n++, "\n");

/* write out the blocks - this MUST follow at least the Major-Order extra
 * or else the read may improperly reconstruct the dimensions
 */
    ok &= _PD_wr_blocks_iii(file);
    ok &= _PD_wr_cksum_iii(file);

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WR_EXT_III - write any extraneous data to the file
 *                - this is essentially a place for expansion of the file
 *                - to complete the definition of a PDB file the following
 *                - rule applies to extensions in the EXTRAS table:
 *                -
 *                -     An extension shall have one of two formats:
 *                -
 *                -        1) <name>:<text>\n
 *                -
 *                -        2) <name>:\n
 *                -           <text1>\n
 *                -           <text2>\n
 *                -              .
 *                -              .
 *                -              .
 *                -           <textn>\n
 *                -           [^B]\n
 *                -
 *                -     anything else is strictly illegal!!!!!!
 *                -     NOTE: the optional ^B is for backward compatibility
 *                -     and is not recommmended.
 */

static int _PD_wr_ext_iii(PDBfile *file, FILE *out)
   {int ok;
    long nbo, nbw;
    char *bf;
    FILE *fp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    ok = TRUE;

    fp = file->stream;

/* write out the previous file name (family) */
    if (file->previous_file != NULL)
       ok &= _PD_put_string(1, "PreviousFile: %s\n", file->previous_file);

    ok &= _PD_put_string(1, "DynamicSpaces: %ld\n", file->n_dyn_spaces);
    ok &= _PD_put_string(1, "UseItags: %ld\n", file->use_itags);

/* write out the date and version data */
    ok &= _PD_put_string(1, "Version: %d (%s)\n",
			 file->system_version, file->date);

/* NOTE: this MUST be the last extra in the file !!! */
    ok &= _PD_cksum_reserve(file);

/* pad the end of the file with some newlines to smooth over the
 * end of binary file problems on different (ie CRAY) systems
 */
    ok &= _PD_put_string(1, "\n\n");

/* write the symbol table and the extras table to the file now */
    bf  = pa->tbuffer;
    nbo = strlen(bf);
    if (fp != out)
       nbw = lio_write(bf, 1, nbo, out);
    else
       nbw = lio_write(bf, 1, nbo, fp);

    ok &= (nbw == nbo);

    _PD_put_string(-1, NULL);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WR_FMT_III - write the primitive data format information to
 *                - the file header block
 *                - this is a no-op in version III
 */

static int _PD_wr_fmt_iii(PDBfile *file)
   {
    
    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_OPEN_III - open an existing PDB file, extract the symbol table and
 *              - structure chart, and return a pointer to the PDB file
 *              - if successful else NULL
 */

static int _PD_open_iii(PDBfile *file, const char *mode)
   {int nb, ns;
    char str[MAXLINE], key[MAXLINE];
    char *acc, *rej, *s, *ps;
    FILE *fp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    fp = pa->ofp;

    file->use_itags = FALSE;

    _PD_symt_delay_rules(file, 0, &acc, &rej);

/* read the trailer */
    nb = MAXLINE - 1;
    if (lio_seek(fp, (int64_t) -nb, SEEK_END))
       PD_error("FSEEK FAILED TO END - _PD_OPEN_III", PD_OPEN);

    nb = lio_read(str, (size_t) 1, (size_t) nb, fp);
    str[nb] = '\0';

/* checksums can have NULL characters so try getting past them */
    ns = strlen(str);
    if (ns < nb)
       ps = str + ns + 1;
    else
       ps = str;

    SC_strncpy(key, MAXLINE, "StructureChartAddress:", -1);
    s = SC_strstr(ps, key);
    if (s == NULL)
       PD_error("FAILED TO MATCH STRUCTURE CHART - _PD_OPEN_III", PD_OPEN);
    file->chrtaddr = SC_stol(s + strlen(key));

    SC_strncpy(key, MAXLINE, "SymbolTableAddress:", -1);
    s = SC_strstr(ps, key);
    if (s == NULL)
       PD_error("FAILED TO MATCH SYMBOL TABLE - _PD_OPEN_III", PD_OPEN);
    file->symtaddr = SC_stol(s + strlen(key));

/* read the symbol table first so that the file pointer is positioned
 * to the "extra" information, then read the "extra's" to get the
 * alignment data, and finish with the structure chart which needs
 * the alignment data
 */

/* read the structure chart */
    if (lio_seek(fp, file->chrtaddr, SEEK_SET))
       PD_error("FSEEK FAILED STRUCTURE CHART - _PD_OPEN_III", PD_OPEN);

    if (!_PD_rd_chrt_iii(file))
       PD_error("CAN'T READ STRUCTURE CHART - _PD_OPEN_III", PD_OPEN);

/* read the symbol table */
    if (lio_seek(fp, file->symtaddr, SEEK_SET))
       PD_error("FSEEK FAILED SYMBOL TABLE - _PD_OPEN_III", PD_OPEN);

    if (!_PD_rd_symt(file, acc, rej))
       PD_error("CAN'T READ SYMBOL TABLE - _PD_OPEN_III", PD_OPEN);

/* read the miscellaneous data */
    if (!_PD_rd_ext_iii(file))
       PD_error("CAN'T READ MISCELLANEOUS DATA - _PD_OPEN_III", PD_OPEN);

    _PD_read_attrtab(file);

/* position the file pointer to the location of the structure chart */
    if (lio_seek(fp, file->chrtaddr, SEEK_SET))
       PD_error("FSEEK FAILED CHART - _PD_OPEN_III", PD_OPEN);

/* initialize the starting address for writing */
    _PD_SET_ADDRESS(file, file->chrtaddr);

/* if this file already contains a valid checksum, default file checksums to on */
    if (PD_verify(file) == TRUE)
       PD_activate_cksum(file, PD_MD5_FILE);

    CFREE(acc);
    CFREE(rej);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CREATE_III - create a PDB file and return an initialized PDBfile
 *                - if successful else NULL
 */

static int _PD_create_iii(PDBfile *file, const char *mode, int mst)
   {FILE *fp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    fp = pa->cfp;

    if (mst == TRUE)
       {file->headaddr = 0L;
	file->chrtaddr = 0L;};

/* initialize the pdb system defs and structure chart */
    _PD_init_chrt(file, TRUE);

    file->use_itags = FALSE;

    if (mst == TRUE)
       {if (lio_seek(fp, file->chrtaddr, SEEK_SET))
	   PD_error("FAILED TO FIND START OF DATA - _PD_CREATE_III", PD_CREATE);

/* initialize the starting address for writing */
	_PD_SET_ADDRESS(file, file->chrtaddr);};

/* initialize directories */
    if (!PD_def_dir(file))
       PD_error("FAILED TO INITIALIZE DIRECTORIES - _PD_CREATE_III", PD_CREATE);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WRITE_META_III - dump the data description tables containing the
 *                    - state of the PDB file in format version III form
 *                    - to the file OUT
 */

static int _PD_write_meta_iii(PDBfile *file, FILE *out, int fh)
   {int64_t addr;
    FILE *fp;

    if (out == NULL)
       out = stdout;

    fp = file->stream;

/* seek the place to write the structure chart */
    addr = _PD_eod(file);
    _PD_set_current_address(file, addr, SEEK_SET, PD_WRITE);

    if (fp == out)
       file->chrtaddr = addr;

/* write the structure chart */
    addr = _PD_wr_chrt_iii(file, out, fh);
    if (addr == -1)
       PD_error("CAN'T WRITE STRUCTURE CHART - _PD_WRITE_META_III", PD_WRITE);

/* write the symbol table */
    addr = _PD_wr_symt_iii(file);
    if (addr == -1)
       PD_error("CAN'T WRITE SYMBOL TABLE - _PD_WRITE_META_III", PD_WRITE);

    if (fp == out)
       file->symtaddr = addr;

/* write the extras table */
    if (!_PD_wr_ext_iii(file, out))
       PD_error("CAN'T WRITE MISCELLANEOUS DATA - _PD_WRITE_META_III", PD_WRITE);

    if (_PD_safe_flush(file) == FALSE)
       PD_error("FFLUSH FAILED AFTER CHART - _PD_WRITE_META_III", PD_WRITE);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FLUSH_III - dump the data description tables containing the current
 *               - state of the PDB file in format version III form
 *               - the tables are:
 *               -    structure chart
 *               -    symbol table
 *               -    extras table
 *               - the table addresses are also updated
 */

static int _PD_flush_iii(PDBfile *file)
   {int ok;
    char *nht;
    FILE *fp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    if (file->virtual_internal)
       return(TRUE);

    ok = _PD_write_attrtab(file);
    if (ok == FALSE)
       return(ok);

    switch (SETJMP(pa->write_err))
       {case ABORT :
	     return(FALSE);
        case ERR_FREE :
	     return(TRUE);
        default :
	     memset(pa->err, 0, MAXLINE);
	     break;};

    if (IS_PDBFILE(file) == TRUE)
       {_PD_write_meta_iii(file, file->stream, 0);

	nht = _PD_header_token(3);
	fp  = file->stream;

/* WARNING: no more writes to the file before the space
 * reserved for the checksum from here on out
 */
	ok = _PD_cksum_file_write(file);

/* write out the chart, symtab addresses, and format version */
	lio_printf(fp, "StructureChartAddress: %s\n",
		   SC_itos(NULL, 0, file->chrtaddr, NULL));
	lio_printf(fp, "SymbolTableAddress: %s\n",
		   SC_itos(NULL, 0, file->symtaddr, NULL));
	lio_printf(fp, "%s\n", nht);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SET_FORMAT_III - set the methods for a format version III file */

int _PD_set_format_iii(PDBfile *file)
   {

    file->open          = _PD_open_iii;
    file->create        = _PD_create_iii;
    file->flush         = _PD_flush_iii;

    file->wr_meta       = _PD_write_meta_iii;
    file->wr_symt       = _PD_wr_symt_iii;
    file->rd_symt       = _PD_rd_symt_iii;
    file->parse_symt    = _PD_parse_symt_iii;
    file->wr_prim_types = _PD_wr_prim_typ_iii;
    file->rd_prim_types = _PD_rd_prim_typ_iii;
    file->wr_fmt        = _PD_wr_fmt_iii;

    file->wr_itag       = _PD_wr_itag_iii;
    file->rd_itag       = _PD_rd_itag_iii;

    file->format_version = 3;

/* NOTE: an interaction with PDBNet requires this to be
 * TRUE now even though it will be set to FALSE when a file
 * is opened or created
 */
    file->use_itags = TRUE;
	    
    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

 
