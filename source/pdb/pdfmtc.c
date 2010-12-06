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

#if 1
#define ITAGS    FALSE
#else
#define ITAGS    TRUE
#endif

/*--------------------------------------------------------------------------*/

/*                               ITAG ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* _PD_WR_ITAG_III - write an itag to the file
 *                 - for a NULL pointer do:
 *                 -     _PD_wr_itag(file, -1L, 0L, type, -1L, LOC_OTHER)
 *                 - for a pointer to data elsewhere do:
 *                 -     _PD_wr_itag(file, n, nitems, type, addr, LOC_OTHER)
 *                 - for a pointer to data here do:
 *                 -     _PD_wr_itag(file, n, nitems, type, addr, LOC_HERE)
 *                 - for a pointer to discontiguous data do:
 *                 -     _PD_wr_itag(file, n, nitems, type, addr, LOC_BLOCK)
 *                 -     then addr is interpreted as the address of the next
 *                 -     block of data
 */

static int _PD_wr_itag_iii(PDBfile *file, long n, long nitems, char *type,
			   int64_t addr, int loc)
   {char s[MAXLINE];
    FILE *fp;

    if (file->virtual_internal == FALSE)
       {fp = file->stream;

	if (file->use_itags == TRUE)

/* must have a definite large number of digits in address field
 * in order to support relocation
 */
	   {snprintf(s, MAXLINE, "\n%s(%ld) %32lld %d;\n",
		     type, nitems, (long long) addr, loc);

	    lio_printf(fp, s);};

	if (loc == LOC_HERE)
	   _PD_ptr_wr_syment(file, n, type, nitems, addr);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_ITAG_III - fill an itag from the file */

static int _PD_rd_itag_iii(PDBfile *file, char *p, PD_itag *pi)
   {int ok;
    long i;
    char t[MAXLINE];
    char *token, *s, *bf;
    FILE *fp;
    PD_smp_state *pa;

    fp = file->stream;
    pa = _PD_get_state(-1);
    bf = pa->tmbf;
	
    if (file->use_itags == TRUE)
       {_PD_rfgets(bf, MAXLINE, fp);
	_PD_rfgets(bf, MAXLINE, fp);

	token = SC_strtok(bf, "(", s);
	if (token == NULL)
	   return(FALSE);
	pi->type = token;

	token = SC_strtok(NULL, ") \n", s);
	if (token == NULL)
	   return(FALSE);
	pi->nitems = atol(token);

	token = SC_strtok(NULL, " \n", s);
	if (token == NULL)
	   {pi->addr = -1;
	    pi->flag = TRUE;}
	else
	   {pi->addr  = SC_stol(token);
	    token = SC_strtok(NULL, " \n", s);
	    if (token == NULL)
	       pi->flag = TRUE;
	    else
	       pi->flag = atoi(token);};

	snprintf(t, MAXLINE, "\n%ld %s %32lld %d;\n",
		 pi->nitems, pi->type, (long long) pi->addr, pi->flag);

	pi->length = strlen(t);}

    else
       {i  = _PD_ptr_index(p);
	ok = _PD_ptr_entry_itag(file, i, pi);};

    return(TRUE);}

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
       {ic = id - SC_CHAR_I;
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
       {ifx = id - SC_SHORT_I;
	if (ifx < N_PRIMITIVE_FIX)
	   {std->fx[ifx].bpi   = nb;
	    std->fx[ifx].order = (PD_byte_order) flg;
	    align->fx[ifx]     = al;};}

/* check for floating point types (proper) */
    else if (SC_is_type_fp(id) == TRUE)
       {ifp = id - SC_FLOAT_I;
	if (ifp < N_PRIMITIVE_FP)
	   {std->fp[ifp].bpi    = nb;
	    std->fp[ifp].order  = ordr;
	    std->fp[ifp].format = formt;
	    align->fp[ifp]      = al;};}

/* check for complex floating point types (proper) */
    else if (SC_is_type_cx(id) == TRUE)
       {icx = id - SC_FLOAT_COMPLEX_I;
	if (icx < N_PRIMITIVE_CPX)
	   {
/*
            std->fp[icx].bpi    = nb;
	    std->fp[icx].order  = ordr;
	    std->fp[icx].format = formt;
	    align->fp[icx]      = al;
 */
	    };}

    else if (strcmp(type, "*") == 0)
       {std->ptr_bytes       = nb;
	align->ptr_alignment = al;}

    else if (id == SC_STRUCT_I)
       align->struct_alignment = al;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_PRIM_TYP_III - read the primitive types from the extras table */

static int _PD_rd_prim_typ_iii(PDBfile *file, char *bf)
   {int ni, align, host_empty;
    int unsgned, onescmp;
    int *ordr, *aord;
    long i, size, conv, bsz;
    long *formt;
    char *token, *type, *origtype, *atype, *s, *local;
    defstr *dp;
    multides *tuple;
    PD_type_kind kind;
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

    while (_PD_get_token(NULL, local, bsz, '\n'))
       {if (*local == '\0')
           break;

        type = SC_strsavef(SC_strtok(local, " \t", s),
			   "char*:_PD_RD_PRIM_TYP_III:type");

        size     = SC_stol(SC_strtok(NULL, " \t", s));
        align    = SC_stol(SC_strtok(NULL, " \t", s));
        ord      = NO_ORDER;
	ordo     = NO_ORDER;
        ordr     = NULL;
        formt    = NULL;
        unsgned  = FALSE;
        onescmp  = FALSE;
        conv     = TRUE;
	kind     = NON_CONVERT_KIND;
	origtype = NULL;
	tuple    = NULL;

        while ((token = SC_strtok(NULL, "(|;\n", s)) != NULL)
	   {token = SC_trim_left(token, " \n");
	    if (strncmp(token, "ORDER", 5) == 0)
	       {ordr = FMAKE_N(int, size, "_PD_RD_PRIM_TYP_III:order");
		for (i = 0L; i < size; i++)
		    {token = SC_strtok(NULL, ",)|", s);
		     if (token == NULL)
		        continue;
		     else if (strcmp(token, "text") == 0)
		        {for (i = 0L; i < size; i++)
			     ordr[i] = i + 1;
			 ordo = TEXT_ORDER;
			 break;}
		     else if (strcmp(token, "big") == 0)
		        {for (i = 0L; i < size; i++)
			     ordr[i] = i + 1;
			 ordo = NORMAL_ORDER;
			 break;}
		     else if (strcmp(token, "little") == 0)
		        {for (i = 0L; i < size; i++)
			     ordr[i] = size - i;
			 ordo = REVERSE_ORDER;
			 break;}

		     else
		        {ordr[i] = SC_stol(token);
			 ordo = SPEC_ORDER;};};}
                    
	    else if (strncmp(token, "FIX", 3) == 0)
	       {SFREE(ordr);
		ord  = ordo;
		kind = INT_KIND;}
                    
	    else if (strncmp(token, "FLOAT", 5) == 0)
	       {formt = FMAKE_N(long, 8, "_PD_RD_PRIM_TYP_III:format");
		for (i = 0L; i < 8; i++)
		    formt[i] = SC_stol(SC_strtok(NULL, ",)|", s));
		kind = FLOAT_KIND;}

	    else if (strncmp(token, "TUPLE", 5) == 0)
	       {atype = SC_strtok(NULL, ",)|", s);
	        ni    = SC_stol(SC_strtok(NULL, ",)|", s));
		aord  = FMAKE_N(int, ni, "_PD_RD_PRIM_TYP_III:aord");
		for (i = 0L; i < ni; i++)
		    aord[i] = SC_stol(SC_strtok(NULL, ",)|", s));
		if (aord[0] == -1)
		   {SFREE(aord);};
		tuple = _PD_make_tuple(atype, ni, aord);}

	    else if (strncmp(token, "CHAR", 4) == 0)
	       kind = CHAR_KIND;
                    
	    else if (strncmp(token, "NO-CONV", 7) == 0)
	       {kind = NON_CONVERT_KIND;
		conv = FALSE;}

	    else if (strncmp(token, "UNSGNED", 7) == 0)
               unsgned = TRUE;

	    else if (strncmp(token, "ONESCMP", 7) == 0)
               onescmp = TRUE;

            else if (strncmp(token, "TYPEDEF", 7) == 0)
	       origtype = SC_strtok(NULL, ")", s);};

	_PD_prim_type_iii(file, type, size, align, ord, ordr, formt);

/* it is either a typedef or a normal type */
        if (origtype != NULL) 
	   {dp = PD_inquire_host_type(file, origtype);
            
	    if (dp != NULL)
               {_PD_d_install(file,  type, _PD_defstr_copy(dp), TRUE);
                host_empty = FALSE;}
            else
               {host_empty = TRUE;}

            dp = PD_inquire_type(file, origtype);
 
            if (dp != NULL)
	       _PD_d_install(file, type, _PD_defstr_copy(dp), FALSE);

/* only the file chart has it - look in there */
            if ((dp != NULL) && host_empty)
               _PD_d_install(file,  type, _PD_defstr_copy(dp), TRUE);

	    SFREE(ordr);
	    SFREE(formt);}

        else 
	   {dp = PD_inquire_host_type(file, type);
	    if ((conv == FALSE) && (dp == NULL))
               _PD_defstr(file, TRUE, type, kind,
			  NULL, tuple,
			  size, align, ord, FALSE,
                          ordr, formt, unsgned, onescmp);

            _PD_defstr(file, FALSE, type, kind,
		       NULL, tuple,
		       size, align, ord, TRUE,
		       ordr, formt, unsgned, onescmp);}

        SFREE(type);};

/* get global type qualifier information */
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
    file->current_prefix = SC_strsavef("/", "char*:_PD_RD_PRIM_TYP_III:prefix");

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_CHRT_III - read the structure chart from the PDB file into
 *                 - the internal structure chart
 *                 - return TRUE if successful
 *                 - return FALSE on error
 */

static int _PD_rd_chrt_iii(PDBfile *file)
   {int hasd;
    long sz, nbc, ncast, icast, bsz;
    char **pl;
    char type[MAXLINE];
    char *nxt, *bf, *p, *local, *member, *cast;
    memdes *desc, *lst, *prev;
    FILE *fp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    fp = file->stream;

/* read the entire chart into memory to speed processing */
    nbc         = file->symtaddr - file->chrtaddr + 1;
    pa->tbuffer = FMAKE_N(char, nbc, "_PD_RD_CHRT_III:tbuffer");

    local = pa->local;
    bsz   = sizeof(pa->local);
    bf    = pa->tbuffer;
    hasd  = pa->has_dirs;

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
    pl    = FMAKE_N(char *, N_CASTS_INCR, "_PD_RD_CHRT_III:cast-list");

    prev = NULL;
    while (_PD_get_token(NULL, local, MAXLINE, '\n'))
       {if (local[0] == '\0')
           break;

	nxt = SC_strtok(local, " \t", p);
	if (nxt != NULL)
	   strcpy(type, nxt);

        nxt = SC_strtok(NULL, " \t()", p);
	if (nxt != NULL)
	   sz = SC_stol(nxt);

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

		member = SC_strtok(nxt, "<-;\n", p);
		cast   = SC_strtok(NULL, "<-; \t\n", p);
		nxt    = SC_strtok(NULL, ";", p);

		member = SC_trim_right(member, " \t");
		member = SC_trim_left(member, " \t");
		if (cast != NULL)
		   cast = SC_trim_left(cast, " \t");

		desc = _PD_mk_descriptor(member, file->default_offset);
		if ((cast == NULL) ||  (*cast == '}'))
		   nxt = cast;
		else
		   {pl[icast]   = SC_strsavef(type,   "char*:_PD_RD_CHRT_III:i0");
		    pl[icast+2] = SC_strsavef(cast,   "char*:_PD_RD_CHRT_III:i2");
		    pl[icast+1] = SC_strsavef(member, "char*:_PD_RD_CHRT_III:i1");
		    icast += 3;
		    if (icast >= ncast)
		       {ncast += N_CASTS_INCR;
			REMAKE_N(pl, char *, ncast);};};

		if (lst == NULL)
		   lst = desc;
		else
		   prev->next = desc;
		prev = desc;

		if ((nxt != NULL) && (*nxt == '}'))
		   break;};};

/* install the type in both charts */
        _PD_defstr_inst(file, type, STRUCT_KIND, lst,
			NO_ORDER, NULL, NULL, FALSE);};

    pa->cast_lst = pl;
    pa->n_casts  = icast;

    _PD_check_casts(file);

    SFREE(pa->tbuffer);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PARSE_SYMT_III - process the asciii representation of a PDBfile
 *                    - symbol table, installing it into the in memory
 *                    - binary version
 *
 */

static int _PD_parse_symt_iii(PDBfile *file, char *buf, int flag)
   {long bsz;
    char *name, *type, *var, *adr, *s, *local;
    int64_t addr, numb;
    syment *ep;
    hasharr *tab;
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

    tab = file->symtab;
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

	type = desc->type;
        name = SC_strtok(desc->name, " \t", s);
	dims = desc->dimensions;

        addr = SC_stol(SC_strtok(adr, " \t", s));
        numb = SC_stol(SC_strtok(NULL, " \t()", s));

/* end of expression */
        _PD_get_token(NULL, local, bsz, '\n');

        ep = _PD_mk_syment(type, numb, addr, NULL, dims);
        _PD_e_install(file, name, ep, flag);

	_PD_rl_descriptor(desc);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_SYMT_III - read the symbol table from the PDB file into a
 *                 - hash table of the PDB for future lookup
 *                 - return TRUE if successful
 *                 - return FALSE on error
 */

static int _PD_rd_symt_iii(PDBfile *file)
   {int rv;
    long nbs;
    int64_t addr, numb;
    char *bf;
    FILE *fp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    fp = file->stream;

/* find the overall file length */
    addr = _PD_get_current_address(file, PD_OPEN);
    _PD_set_current_address(file, 0, SEEK_END, PD_OPEN);
    numb = _PD_get_current_address(file, PD_OPEN);
    _PD_set_current_address(file, addr, SEEK_SET, PD_OPEN);

/* read in the symbol table and extras table as a single block */
    nbs                   = numb - file->symtaddr;
    pa->tbuffer = FMAKE_N(char, nbs + 1, "_PD_RD_SYMT_III:tbuffer");

    bf = pa->tbuffer;

    numb = lio_read(bf, 1, nbs, fp);
    if (numb != nbs)
       return(FALSE);

    bf[nbs] = (char) EOF;

    rv = _PD_parse_symt_iii(file, bf, FALSE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_BLOCKS_III - read the Blocks extra */

static void _PD_rd_blocks_iii(PDBfile *file)
   {long j, n, nt, numb, stride, bsz;
    int64_t addr;
    char *name, *token, *s, *local;
    syment *ep;
    dimdes *dim;
    SC_array *bl;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    local = pa->local;
    bsz   = sizeof(pa->local);

    while (_PD_get_token(NULL, local, bsz, '\n'))
       {if (*local == '\0')
	   break;

	name = SC_strtok(local, " \n", s);
	n    = SC_stoi(SC_strtok(NULL, " \n", s));
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
		     
	   addr = SC_stoi(token);
	   numb = SC_stoi(SC_strtok(NULL, " \n", s));
                         
	   _PD_block_set_desc(addr, numb, bl, j);

	   nt += numb;};

/* adjust the slowest varying dimension to reflect the entire entry */
	dim = PD_entry_dimensions(ep);
	if (dim != NULL)
	   {if (PD_get_major_order(file) == COLUMN_MAJOR_ORDER)
	       for (; dim->next != NULL; dim = dim->next);

	    stride = PD_entry_number(ep)/dim->number;
	    stride = nt/stride;

	    dim->number    = stride;
	    dim->index_max = dim->index_min + stride - 1L;};

/* adjust the number to reflect the entire entry */
	PD_entry_number(ep) = nt;};

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

    file->default_offset = 0;
    file->system_version = 0;
    SFREE(file->date);

/* read the default offset */
    while (_PD_get_token(NULL, local, bsz, '\n'))
       {token = SC_strtok(local, ":\n", p);
        if (token == NULL)
           break;

        if (strcmp(token, "Blocks") == 0)
	   _PD_rd_blocks_iii(file);

        else if (strcmp(token, "Checksums") == 0)
	   _PD_block_csum_read(file);

        else if (strcmp(token, "DynamicSpaces") == 0)
           {token = SC_strtok(NULL, "\n", p);
            if (token != NULL)
	       file->n_dyn_spaces = SC_stoi(token);}

        else if (strcmp(token, "UseItags") == 0)
           {token = SC_strtok(NULL, "\n", p);
            if (token != NULL)
	       file->use_itags = SC_stoi(token);}

        else if (strcmp(token, "PreviousFile") == 0)
           {token = SC_strtok(NULL, "\n", p);
            if (token != NULL)
               file->previous_file = SC_strsavef(token,
						 "char*:_PD_RD_EXT_III:prev");}

        else if (strcmp(token, "Version") == 0)
           {token = SC_strtok(NULL, " \t(", p);
            if (token != NULL)
               file->system_version = atoi(token);

            token = SC_strtok(NULL, "()\n", p);
            if (token != NULL)
               file->date = SC_strsavef(token,
                                        "char*:_PD_RD_EXT_III:date");};};

/* release the buffer which held both the symbol table and the extras */
    SFREE(pa->tbuffer);

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
	    {case CHAR_KIND :
	          ok &= _PD_put_string(l++, "|CHAR");
		  break;
	     case INT_KIND :
		  ok &= _PD_put_string(l++, "|FIX");
		  break;
	     case FLOAT_KIND :
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
       SFREE(pa->tbuffer);

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
	 if (fdp->members != NULL)

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
		  if (hdsc->cast_memb != NULL)
		     _PD_put_string(n++, "\t<- %s", hdsc->cast_memb);

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
    if (out != fp)
       lio_write(bf, 1, strlen(bf), out);
    else
       lio_write(bf, 1, strlen(bf), fp);

    SFREE(pa->tbuffer);

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WR_BLOCKS_III - write out the blocks for all variables in FILE */

static int _PD_wr_blocks_iii(PDBfile *file)
   {int ok;
    long i, j, n, ni;
    int64_t addr;
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

		  ok &= _PD_put_string(1, " %lld %ld", (int64_t) addr, ni);};

	     ok &= _PD_put_string(1, "\n");};};

    ok &= _PD_put_string(1, "\n");

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WR_CSUM_III - write out the checksums for blocks of
 *                 - all variables in FILE
 */

static int _PD_wr_csum_iii(PDBfile *file)
   {int ok;
    long i;
    char *nm;
    syment *ep;

    ok = TRUE;

    if (file->use_cksum & PD_MD5_RW)
       {ok &= _PD_put_string(1, "Checksums:\n");

	for (i = 0; SC_hasharr_next(file->symtab, &i, &nm, NULL, (void **) &ep); i++)
	    _PD_block_csum_write(file, ep, nm);

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
    long i, nt, nb, ni, stride, mn, mx;
    int64_t addr, ad;
    char *ty, *nm;
    syment *ep;
    dimdes *lst;
    FILE *fp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    if (file == NULL)
       return(-1);

    fp   = file->stream;
    addr = _PD_get_current_address(file, PD_WRITE);
    if (addr == -1)
       return(-1);

    if (pa->tbuffer != NULL)
       SFREE(pa->tbuffer);

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
	 for (nd = 0, lst = PD_entry_dimensions(ep);
	      lst != NULL;
	      nd++, lst = lst->next)
	     {if ((flag == ROW_MAJOR_ORDER) ||
		  ((flag == COLUMN_MAJOR_ORDER) && (lst->next == NULL)))
		 {stride = nt/(lst->number);
		  stride = (stride == 0) ? 1 : stride;
		  ni     = nb/stride;
		  flag   = FALSE;}
	      else
		 ni = lst->number;

	      mn = lst->index_min;
	      mx = mn + ni - 1;
	      if (nd == 0)
		 _PD_put_string(n++, "[%ld:%ld", mn, mx);
	      else
		 _PD_put_string(n++, ",%ld:%ld", mn, mx);};

	 if (nd > 0)
	    _PD_put_string(n++, "]");

	 _PD_put_string(n++, 
			" @ %lld (%ld);\n",
			(int64_t) ad, nb);};

/* pad an extra newline to mark the end of the symbol table for _PD_rd_symt */
    _PD_put_string(n++, "\n");

/* write out the blocks - this MUST follow at least the Major-Order extra
 * or else the read may improperly reconstruct the dimensions
 */
    ok &= _PD_wr_blocks_iii(file);
    ok &= _PD_wr_csum_iii(file);

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
    ok &= _PD_csum_reserve(file);

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

    SFREE(pa->tbuffer);

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

static int _PD_open_iii(PDBfile *file)
   {int nb;
    char str[MAXLINE], key[MAXLINE];
    char *s, *ps;
    FILE *fp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    fp = pa->ofp;

    file->use_itags = ITAGS;

/* read the trailer */
    nb = MAXLINE - 1;
    if (lio_seek(fp, (int64_t) -nb, SEEK_END))
       PD_error("FSEEK FAILED TO END - _PD_OPEN_III", PD_OPEN);

    nb = lio_read(str, (size_t) 1, (size_t) nb, fp);
    str[nb] = '\0';

/* checksums can have NULL characters so try getting past them */
    if (strlen(str) < nb)
       ps = str + strlen(str) + 1;
    else
       ps = str;

    strcpy(key, "StructureChartAddress:");
    s = SC_strstr(ps, key);
    if (s == NULL)
       PD_error("FAILED TO MATCH STRUCTURE CHART - _PD_OPEN_III", PD_OPEN);
    file->chrtaddr = SC_stol(s + strlen(key));

    strcpy(key, "SymbolTableAddress:");
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

    if (!_PD_rd_symt_iii(file))
       PD_error("CAN'T READ SYMBOL TABLE - _PD_OPEN_III", PD_OPEN);

/* read the miscellaneous data */
    if (!_PD_rd_ext_iii(file))
       PD_error("CAN'T READ MISCELLANEOUS DATA - _PD_OPEN_III", PD_OPEN);

    _PD_read_attrtab(file);

    if (file->use_itags == FALSE)
       _PD_ptr_open_setup(file);

/* position the file pointer to the location of the structure chart */
    if (lio_seek(fp, file->chrtaddr, SEEK_SET))
       PD_error("FSEEK FAILED CHART - _PD_OPEN_III", PD_OPEN);

/* initialize the starting address for writing */
    _PD_SET_ADDRESS(file, file->chrtaddr);

/* if this file already contains a valid checksum, default file checksums to on */
    if (PD_verify(file) == TRUE)
       PD_activate_cksum(file, PD_MD5_FILE);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CREATE_III - create a PDB file and return an initialized PDBfile
 *                - if successful else NULL
 */

static int _PD_create_iii(PDBfile *file, int mst)
   {FILE *fp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    fp = pa->cfp;

    if (mst == TRUE)
       {file->headaddr = 0L;
	file->chrtaddr = 0L;};

/* initialize the pdb system defs and structure chart */
    _PD_init_chrt(file, TRUE);

    file->use_itags = ITAGS;

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

    if (_PD_safe_flush(file) == FALSE)
       PD_error("FFLUSH FAILED BEFORE CHART - _PD_WRITE_META_III", PD_WRITE);

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
	ok = _PD_csum_file_write(file);

/* write out the chart, symtab addresses, and format version */
	lio_printf(fp, "StructureChartAddress: %lld\n", (int64_t) file->chrtaddr);
	lio_printf(fp, "SymbolTableAddress: %lld\n", (int64_t) file->symtaddr);
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
    file->parse_symt    = _PD_parse_symt_iii;
    file->wr_prim_types = _PD_wr_prim_typ_iii;
    file->rd_prim_types = _PD_rd_prim_typ_iii;
    file->wr_fmt        = _PD_wr_fmt_iii;

    file->wr_itag       = _PD_wr_itag_iii;
    file->rd_itag       = _PD_rd_itag_iii;

    file->format_version = 3;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

 
