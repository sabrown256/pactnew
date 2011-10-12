/*
 * PDFMTB.C - PDB II format methods
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

/* PDBFile II Layout:
 *       
 *       Header (ASCII)
 *         Identifying token
 *         Primitive Data Types
 *         Structure Chart Address
 *         Symbol Table Address
 *       Data
 *       Structure Chart
 *       Symbol Table
 *       Extensions
 *       EOF
 */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_REGEN_STD - regenerate the file standard using the primitive
 *               - type definitions from the extras section
 *               - the type info from the header is incomplete
 */

static void _PD_regen_std(PDBfile *file, char *type,
			  long bpi, PD_byte_order ord)
   {int i, id;
    data_standard *std;

    id = SC_type_id(type, FALSE);
    if (SC_is_type_prim(id) == TRUE)
       {std = file->std;

	if (SC_is_type_fix(id) == TRUE)
           {i = SC_TYPE_FIX(id);
	    std->fx[i].bpi   = bpi;
	    std->fx[i].order = ord;};};

    return;}

/*--------------------------------------------------------------------------*/

/*                               ITAG ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* _PD_WR_ITAG_II - write an itag to the file
 *                - for a NULL pointer do:
 *                -     _PD_wr_itag(file, -1L, 0L, type, -1L, LOC_OTHER)
 *                - for a pointer to data elsewhere do:
 *                -     _PD_wr_itag(file, n, ni, type, addr, LOC_OTHER)
 *                - for a pointer to data here do:
 *                -     _PD_wr_itag(file, n, ni, type, addr, LOC_HERE)
 *                - for a pointer to discontiguous data do:
 *                -     _PD_wr_itag(file, n, ni, type, addr, LOC_BLOCK)
 *                -     then addr is interpreted as the address of the next
 *                -     block of data
 */

static int _PD_wr_itag_ii(PDBfile *file, PD_address *ad, long ni, char *type,
			  int64_t addr, PD_data_location loc)
   {char s[MAXLINE];
    FILE *fp;

    if (file->virtual_internal == FALSE)
       {fp = file->stream;

/* must have a definite large number of digits in address field
 * in order to support relocation
 */
	snprintf(s, MAXLINE, "%ld\001%s\001%32lld\001%d\001\n",
		 ni, type, (long long) addr, loc);

	lio_printf(fp, s);

	if ((ni > 0) && (loc == LOC_HERE))
	   _PD_ptr_wr_syment(file, ad, type, ni, addr);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_ITAG_II - fill an itag from the file */

static int _PD_rd_itag_ii(PDBfile *file, char *p, PD_itag *pi)
   {char t[MAXLINE];
    char *token, *s, *bf;
    FILE *fp;
    PD_smp_state *pa;

    fp = file->stream;
    pa = _PD_get_state(-1);
    bf = pa->tmbf;

    _PD_rfgets(bf, MAXLINE, fp);

    token = SC_strtok(bf, "\001", s);
    if (token == NULL)
       return(FALSE);
    pi->nitems = atol(token);

    pi->type = SC_strtok(NULL, "\001\n", s);
    if (pi->type == NULL)
       return(FALSE);

    token = SC_strtok(NULL, "\001\n", s);
    if (token == NULL)
       {pi->addr = -1;
        pi->flag = TRUE;}
    else
       {pi->addr  = SC_stol(token);
        token = SC_strtok(NULL, "\001\n", s);
        if (token == NULL)
           pi->flag = TRUE;
        else
           pi->flag = atoi(token);};

    snprintf(t, MAXLINE, "%ld\001%s\001%32lld\001%d\001\n",
	     pi->nitems, pi->type, (long long) pi->addr, pi->flag);

    pi->length = strlen(t);

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                               READ ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* _PD_RD_FMT_II - read the primitive data format information from 
 *               - the file header block
 *               - this is largely ignored in favor of the primitive
 *               - types in the extras section
 *               -
 *               - Floating Point Format Descriptor
 *               -   format[0] = # of bits per number
 *               -   format[1] = # of bits in exponent
 *               -   format[2] = # of bits in mantissa
 *               -   format[3] = start bit of sign
 *               -   format[4] = start bit of exponent
 *               -   format[5] = start bit of mantissa
 *               -   format[6] = high order mantissa bit (CRAY needs this)
 *               -   format[7] = bias of exponent
 */

static int _PD_rd_fmt_ii(PDBfile *file)
   {int i, j, n, id;
    int *order;
    long *format;
    char infor[MAXLINE], *p, *s, *pi;
    data_standard *std;
    
/* read the number of bytes worth of format data */
    if (lio_read(infor, (size_t) 1, (size_t) 1, file->stream) != 1)
       PD_error("FAILED TO READ FORMAT HEADER - _PD_RD_FMT_II", PD_OPEN);
    
    n = infor[0] - 1;

/* read the format data */
    if (lio_read(infor+1, (size_t) 1, (size_t) n, file->stream) != n)
       PD_error("FAILED TO READ FORMAT DATA - _PD_RD_FMT_II", PD_OPEN);
    
/* decipher the format data */
    p   = infor + 1;
    std = _PD_copy_standard(file->host_std);

/* get the byte lengths in */
    std->ptr_bytes = *(p++);

    for (id = SC_SHORT_I; id <= SC_LONG_I; id++)
        {i = SC_TYPE_FIX(id);
	 std->fx[i].bpi = *(p++);};

    std->fx[SC_TYPE_FIX(SC_INT8_I)].bpi = 1;

    for (id = SC_FLOAT_I; id <= SC_DOUBLE_I; id++)
        {i = SC_TYPE_FP(id);
	 std->fp[i].bpi = *(p++);};

/* get the fix point byte orders in */
    for (id = SC_SHORT_I; id <= SC_LONG_I; id++)
        {i = SC_TYPE_FIX(id);
	 std->fx[i].order = (PD_byte_order) *(p++);};

    std->fx[SC_TYPE_FIX(SC_INT8_I)].order = std->fx[i].order;

/* get the floating point byte orders in */
    for (id = SC_FLOAT_I; id <= SC_DOUBLE_I; id++)
        {i = SC_TYPE_FP(id);
	 n     = std->fp[i].bpi;
	 order = std->fp[i].order;
	 CREMAKE(order, int, n);
	 SC_mark(order, 1);
	 std->fp[i].order = order;
	 for (j = 0; j < n; j++, *(order++) = *(p++));};

/* get the floating point format data in */
    n = FORMAT_FIELDS - 1;
    for (id = SC_FLOAT_I; id <= SC_DOUBLE_I; id++)
        {i = SC_TYPE_FP(id);
	 format = std->fp[i].format;
	 for (j = 0; j < n; j++, *(format++) = *(p++));};

/* read the biases */
    if (_PD_rfgets(infor, MAXLINE, file->stream) == NULL)
       PD_error("CAN'T READ THE BIASES - _PD_RD_FMT_II", PD_OPEN);
    
    for (id = SC_FLOAT_I, pi = infor; id <= SC_DOUBLE_I; id++, pi = NULL)
        {i = SC_TYPE_FP(id);
	 format    = std->fp[i].format;
	 format[7] = SC_stol(SC_strtok(pi, "\001", s));};

    file->std = std;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PARSE_SYMT_II - process the ascii representation of a PDBfile
 *                   - symbol table, installing it into the in memory
 *                   - binary version
 *
 */

static int _PD_parse_symt_ii(PDBfile *file, char *buf, int flag)
   {int i;
    long mini, leng, bsz;
    int64_t addr, numb;
    char *name, *type, *tmp, *pbf, *s, *local;
    syment *ep;
    dimdes *dims, *next, *prev;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    local = pa->local;
    bsz   = sizeof(pa->local);

    pbf  = buf;
    prev = NULL;
    for (i = 0; _PD_get_token(pbf, local, bsz, '\n') != NULL; i++)
        {pbf  = NULL;
	 name = SC_strtok(local, "\001", s);
	 if (name == NULL)
            break;
	 type = SC_strtok(NULL, "\001", s);
	 numb = SC_stol(SC_strtok(NULL, "\001", s));
	 addr = SC_stol(SC_strtok(NULL, "\001", s));
	 dims = NULL;
	 while ((tmp = SC_strtok(NULL, "\001\n", s)) != NULL)
            {mini = SC_stol(tmp);
	     leng = SC_stol(SC_strtok(NULL, "\001\n", s));
	     next = _PD_mk_dimensions(mini, leng);
	     if (dims == NULL)
                dims = next;

	     else
                {prev->next = next;
		 SC_mark(next, 1);};

	     prev = next;};

	 ep = _PD_mk_syment(type, numb, addr, NULL, dims);
	 _PD_e_install(file, name, ep, flag);};

    SC_ASSERT(i >= 0);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_SYMT_II - read the symbol table from the PDB file into a
 *                - hash table of the PDB for future lookup
 *                - return TRUE if successful
 *                - return FALSE on error
 */

int _PD_rd_symt_ii(PDBfile *file)
   {int rv;
    long symt_sz;
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
    symt_sz     = numb - file->symtaddr;
    pa->tbuffer = CMAKE_N(char, symt_sz + 1);

    bf = pa->tbuffer;

    numb = lio_read(bf, 1, symt_sz, fp);
    if (numb != symt_sz)
       return(FALSE);

    bf[symt_sz] = (char) EOF;

    rv = _PD_parse_symt_ii(file, bf, FALSE);

    if (file->use_itags == FALSE)
       _PD_ptr_open_setup(file);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_BLOCKS_II - read the Blocks extra data for FILE */

static void _PD_rd_blocks_ii(PDBfile *file)
   {long j, n, nt, numb, stride, bsz;
    int64_t addr;
    char *local, *name, *s;
    syment *ep;
    dimdes *dim;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    local = pa->local;
    bsz   = sizeof(pa->local);

    while (_PD_get_token(NULL, local, bsz, '\n'))
       {if (*local == '\002')
	   break;

	name = SC_strtok(local, "\001\n", s);
	n    = SC_stoi(SC_strtok(NULL, " \n", s));
	ep   = PD_inquire_entry(file, name, FALSE, NULL);
	nt   = 0L;
	for (j = 0L; j < n; j++)
	    {addr = SC_stoi(SC_strtok(NULL, " \n", s));
	     numb = SC_stoi(SC_strtok(NULL, " \n", s));
	     if ((addr == 0) || (numb == 0L))
	        {_PD_get_token(NULL, local, bsz, '\n');
		 addr = SC_stoi(SC_strtok(local, " \n", s));
		 numb = SC_stoi(SC_strtok(NULL, " \n", s));};
                         
	     _PD_entry_set_address(ep, j, addr);
	     _PD_entry_set_number(ep, j, numb);

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
	ep->number = nt;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_CHRT_II - read the structure chart from the PDB file into
 *                - the internal structure chart
 *                - return TRUE if successful
 *                - return FALSE on error
 */

int _PD_rd_chrt_ii(PDBfile *file)
   {long sz, chrt_sz, bsz;
    char *nxt, type[MAXLINE], *pbf, *local;
    FILE *fp;
    memdes *desc, *lst, *prev;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    fp = file->stream;

/* read the entire chart into memory to speed processing */
    chrt_sz     = file->symtaddr - file->chrtaddr + 1;
    pa->tbuffer = CMAKE_N(char, chrt_sz);
    if (lio_read(pa->tbuffer, 1, chrt_sz, fp) != chrt_sz)
       return(FALSE);
    pa->tbuffer[chrt_sz-1] = (char) EOF;

    local = pa->local;
    bsz   = sizeof(pa->local);

    prev = NULL;
    pbf  = pa->tbuffer;
    while (_PD_get_token(pbf, type, MAXLINE, '\001'))
       {pbf = NULL;
	if (type[0] == '\002')
           break;

        sz = SC_stol(_PD_get_token(pbf, local, bsz, '\001'));
	SC_ASSERT(sz != 0);

        lst  = NULL;
        while ((nxt = _PD_get_token(pbf, local, bsz, '\001')) != NULL)
           {if (*nxt == '\0')
               break;
            desc = _PD_mk_descriptor(nxt, file->default_offset);
            if (lst == NULL)
               lst = desc;
            else
               prev->next = desc;
            prev = desc;};

/* install the type in both charts */
        _PD_defstr_inst(file, type, STRUCT_KIND, lst,
			NO_ORDER, NULL, NULL, PD_CHART_HOST);};

/* complete the setting of the directory indicator */
    if (pa->has_dirs)
       {PD_defncv(file, "Directory", 1, 0);
	file->current_prefix = CSTRSAVE("/");};
    pa->has_dirs = FALSE;

    _PD_check_casts(file);

    CFREE(pa->tbuffer);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_PRIM_ALIGN_TYPE - set the alignment for TYPE */

static void _PD_rd_prim_align_type(char *type, int al, data_alignment *align)
   {int i, id;

    if ((strcmp(type, "pointer") == 0) || (strcmp(type, "*") == 0))
       align->ptr_alignment = al;

    else
       {id = SC_type_id(type, FALSE);
	if (id == SC_BOOL_I)
	   align->bool_alignment = al;

	else if (SC_is_type_char(id) == TRUE)
	   {i = SC_TYPE_CHAR(id);
	    if (i < N_PRIMITIVE_CHAR)
	       align->chr[i] = al;}

	else if (SC_is_type_fix(id) == TRUE)
	   {i = SC_TYPE_FIX(id);
	    if (i < N_PRIMITIVE_FIX)
	       align->fx[i] = al;}

	else if (SC_is_type_fp(id) == TRUE)
	   {i = SC_TYPE_FP(id);
	    if (i < N_PRIMITIVE_FP)
	       align->fp[i] = al;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_PRIM_TYP_II - read the primitive types from the extras table */

static int _PD_rd_prim_typ_ii(PDBfile *file, char *bf)
   {int ni, align;
    int dc, rec;
    int unsgned, onescmp;
    int *ordr, *aord;
    long i, bpi, conv, bsz;
    long *formt;
    char *token, *type, *origtype, *atype, delim[10], *s, *local;
    multides *tuple;
    PD_type_kind kind;
    PD_byte_order ord;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    dc  = '\001';
    rec = '\002';

    local = pa->local;
    bsz   = sizeof(pa->local);

    snprintf(delim, 10, "%c\n", dc);

    if (bf != NULL)
       _PD_get_token(bf, local, bsz, '\n');

/* initialize the pdb system defs and structure chart */
    _PD_init_chrt(file, FALSE);

    while (_PD_get_token(NULL, local, bsz, '\n'))
       {if (*local == rec)
           break;

        type     = CSTRSAVE(SC_strtok(local, delim, s));
        bpi      = SC_stol(SC_strtok(NULL, delim, s));
        align    = SC_stol(SC_strtok(NULL, delim, s));
        ord      = (PD_byte_order) SC_stol(SC_strtok(NULL, delim, s));
        unsgned  = FALSE;
        onescmp  = FALSE;
        conv     = TRUE;
	kind     = INT_KIND;
        ordr     = NULL;
        formt    = NULL;
	tuple    = NULL;
	origtype = NULL;

	_PD_rd_prim_align_type(type, align, file->align);

        token = SC_strtok(NULL, delim, s);
        if ((token != NULL) && (strcmp(token, "ORDER") == 0))
           {ordr = CMAKE_N(int, bpi);
            for (i = 0L; i < bpi; i++)
                ordr[i] = SC_stol(SC_strtok(NULL, delim, s));};
                    
        token = SC_strtok(NULL, delim, s);
        if ((token != NULL) && (strcmp(token, "FLOAT") == 0))
           {formt = CMAKE_N(long, 8);
            for (i = 0L; i < 8; i++)
                formt[i] = SC_stol(SC_strtok(NULL, delim, s));

	    kind = FLOAT_KIND;}

        else if ((token != NULL) && (strcmp(token, "NO-CONV") == 0))
	   {conv = FALSE;
	    kind = NON_CONVERT_KIND;};

        token = SC_strtok(NULL, delim, s);
        if ((token != NULL) && (strcmp(token, "TUPLE") == 0))
	   {atype = SC_strtok(NULL, delim, s);
	    ni    = SC_stol(SC_strtok(NULL, delim, s));
	    aord  = CMAKE_N(int, ni);
	    for (i = 0L; i < ni; i++)
	        {aord[i] = SC_stol(SC_strtok(NULL, delim, s));
		 if (aord[i] == -1)
		    break;};
	    if (aord[0] == -1)
	       CFREE(aord);
	    tuple = _PD_make_tuple(atype, ni, aord);
	    token = SC_strtok(NULL, delim, s);};

        if ((token != NULL) && (strcmp(token, "UNSGNED") == 0))
	   {unsgned = SC_stol(SC_strtok(NULL, delim, s));
	    token   = SC_strtok(NULL, delim, s);};

        if ((token != NULL) && (strcmp(token, "ONESCMP") == 0))
	   {onescmp = SC_stol(SC_strtok(NULL, delim, s));
	    token   = SC_strtok(NULL, delim, s);};

/* ignore typedef for "REAL" type at the moment for backward compat */
        if ((token != NULL) && (strcmp(type, "REAL") != 0) && 
            (strcmp(token, "TYPEDEF") == 0))
           {origtype = SC_strtok(NULL, delim, s);
            token    = SC_strtok(NULL, delim, s);};

	if (origtype == NULL) 
	   _PD_regen_std(file, type, bpi, ord);

	_PD_defstr_prim_rd(file, type, origtype, kind,
			   tuple, bpi, align, ord,
			   ordr, formt, unsgned, onescmp, conv);

	if (origtype != NULL) 
	   {CFREE(ordr);
	    CFREE(formt);};

        CFREE(type);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_EXT_II - read any extraneous data from the file
 *               - this is essentially a place for expansion of the file
 */

int _PD_rd_ext_ii(PDBfile *file)
   {long bsz;
    char *token, *s, *p, *local;
    data_alignment *palgn;
    data_standard *ps;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    pa->n_casts = 0L;

    palgn = CMAKE(data_alignment);

    file->align          = palgn;
    file->default_offset = 0;
    file->system_version = 0;
    CFREE(file->date);

    local = pa->local;
    bsz   = sizeof(pa->local);

/* read the default offset */
    while (_PD_get_token(NULL, local, bsz, '\n'))
       {token = SC_strtok(local, ":\n", p);
        if (token == NULL)
           break;

/* get the default offset */
        if (strcmp(token, "Offset") == 0)
           {token = SC_strtok(NULL, "\n", p);
            if (token != NULL)
               file->default_offset = atoi(token);}

        else if (strcmp(token, "Struct-Alignment") == 0)
           {token = SC_strtok(NULL, "\n", p);
            if ((token != NULL) && (palgn != NULL))
	       palgn->struct_alignment = atoi(token);}

        else if (strcmp(token, "Longlong-Format-Alignment") == 0)
	   {int i;

	    i = SC_TYPE_FIX(SC_LONG_LONG_I);
	    token = SC_strtok(NULL, "\n", p);
            if (token != NULL)
               {ps = file->std;
		if (ps != NULL)
		   {ps->fx[i].bpi   = token[0];
		    ps->fx[i].order = (PD_byte_order) token[1];};
		if (palgn != NULL)
		   palgn->fx[i] = token[2];};}

        else if (strcmp(token, "Casts") == 0)
           {long n_casts, i;
            char **sa;

            n_casts = N_CASTS_INCR;
            sa      = CMAKE_N(char *, N_CASTS_INCR);
            i       = 0L;
            while (_PD_get_token(NULL, local, bsz, '\n'))
               {if (*local == '\002')
                   break;
                sa[i++] = CSTRSAVE(SC_strtok(local, "\001\n", s));
                sa[i++] = CSTRSAVE(SC_strtok(NULL, "\001\n", s));
                sa[i++] = CSTRSAVE(SC_strtok(NULL, "\001\n", s));
                if (i >= n_casts)
                   {n_casts += N_CASTS_INCR;
                    CREMAKE(sa, char *, n_casts);};};
            pa->cast_lst = sa;
            pa->n_casts  = i;}

        else if (strcmp(token, "Blocks") == 0)
	   _PD_rd_blocks_ii(file);

        else if (strcmp(token, "Checksums") == 0)
	   _PD_block_csum_read(file);

/* read in the primitives */
        else if (strcmp(token, "Primitive-Types") == 0)
           _PD_rd_prim_typ_ii(file, NULL);

        else if (strcmp(token, "Major-Order") == 0)
           {token = SC_strtok(NULL, "\n", p);
            if (token != NULL)
               file->major_order = (PD_major_order) atoi(token);}

        else if (strcmp(token, "Has-Directories") == 0)
           {if (SC_stoi(SC_strtok(NULL, "\n", p)))
               pa->has_dirs = TRUE;}

        else if (strcmp(token, "Dynamic Spaces") == 0)
           {token = SC_strtok(NULL, "\n", p);
            if (token != NULL)
	       file->n_dyn_spaces = SC_stoi(token);}

        else if (strcmp(token, "Use Itags") == 0)
           {token = SC_strtok(NULL, "\n", p);
            if (token != NULL)
	       file->use_itags = SC_stoi(token);}

        else if (strcmp(token, "Previous-File") == 0)
           {token = SC_strtok(NULL, "\n", p);
            if (token != NULL)
               file->previous_file = CSTRSAVE(token);}

        else if (strcmp(token, "Version") == 0)
           {token = SC_strtok(NULL, "|", p);
            if (token != NULL)
               file->system_version = atoi(token);

            token = SC_strtok(NULL, "\n", p);
            if (token != NULL)
               file->date = CSTRSAVE(token);};};

/* release the buffer which held both the symbol table and the extras */
    CFREE(pa->tbuffer);

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                               WRITE ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* _PD_WR_SYMT_II - write out the symbol table (a hash table) into the
 *                - PDB file
 *                - return the disk address if successful
 *                - return -1L on error
 */

static int64_t _PD_wr_symt_ii(PDBfile *file)
   {int n, flag;
    long i, nt, nb, ni, stride;
    int64_t addr, ad;
    char *ty, *nm;
    syment *ep;
    dimdes *lst;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    if (file == NULL)
       return(-1);

    addr = _PD_get_current_address(file, PD_WRITE);
    if (addr == -1)
       return(-1);

    if (pa->tbuffer != NULL)
       _PD_put_string(-1, NULL);

    n = 0;
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
	        {PD_error("BAD BLOCK LIST - _PD_WR_SYMT_II", PD_GENERIC);
		 return(-1L);};};

	 _PD_put_string(n++, 
			"%s\001%s\001%ld\001%lld\001",
			nm, ty, nb, (int64_t) ad);

/* adjust the slowest varying dimension to reflect only the first block */
	 flag = PD_get_major_order(file);
	 for (lst = PD_entry_dimensions(ep);
	      lst != NULL;
	      lst = lst->next)
	     {if ((flag == ROW_MAJOR_ORDER) ||
		  ((flag == COLUMN_MAJOR_ORDER) && (lst->next == NULL)))
		 {stride = nt/(lst->number);
		  stride = (stride == 0) ? 1 : stride;
		  ni     = nb/stride;
		  flag   = FALSE;}
	      else
		 ni = lst->number;

	      _PD_put_string(n++, "%ld\001%ld\001",
			     lst->index_min, ni);};

	 _PD_put_string(n++, "\n");};

/* pad an extra newline to mark the end of the symbol table for _PD_rd_symt */
    _PD_put_string(n++, "\n");

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WR_CASTS_II - write the cast for types in FILE
 *                 - return TRUE iff successful
 */

static int _PD_wr_casts_ii(PDBfile *file)
   {int ok;
    long i;
    char *nm;
    defstr *dp;
    memdes *desc;

    ok = TRUE;

    ok &= _PD_put_string(1, "Casts:\n");

    for (i = 0; SC_hasharr_next(file->host_chart, &i, &nm, NULL, (void **) &dp); i++)
        {for (desc = dp->members; desc != NULL; desc = desc->next)
	     if (desc->cast_memb != NULL)
	        ok &= _PD_put_string(1, "%s\001%s\001%s\001\n",
				     nm, desc->member, desc->cast_memb);};

    ok &= _PD_put_string(1, "\002\n");

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WR_CHRT_II - write out the structure chart into the PDB file
 *                - return a disk address if successful
 *                - return -1L on error
 */

static int64_t _PD_wr_chrt_ii(PDBfile *file, FILE *out, int fh)
   {int n;
    long i;
    int64_t addr;
    char *bf, *nm;
    FILE *fp;
    hasharr *ch;
    defstr *dp;
    memdes *desc;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    fp   = file->stream;
    addr = _PD_get_current_address(file, PD_WRITE);
    if (addr == -1)
       return(-1);

    if (pa->tbuffer != NULL)
       _PD_put_string(-1, NULL);

    if (fh == 0)
       ch = file->chart;
    else
       ch = file->host_chart;

    _PD_rev_chrt(ch);

/* the hash array for the structure chart is one element long */
    n = 0;
    for (i = 0; SC_hasharr_next(ch, &i, &nm, NULL, (void **) &dp); i++)

/* use np->name instead of dp->type or PD_typedef's will not work */
        {_PD_put_string(n++, "%s\001%ld\001", nm, dp->size);

	 for (desc = dp->members; desc != NULL; desc = desc->next)
	     _PD_put_string(n++, "%s\001", desc->member);

	 _PD_put_string(n++, "\n");};

    _PD_put_string(n++, "\002\n");

/* restore the chart because this may be a PD_flush and more types
 * may be added later
 */
    _PD_rev_chrt(ch);

    bf = pa->tbuffer;

/* write the entire chart to the file now */
    if (out != fp)
       lio_write(bf, 1, strlen(bf), out);
    else
       lio_write(bf, 1, strlen(bf), fp);

    _PD_put_string(-1, NULL);

    return(addr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WR_PRIM_TYP_II - write the primitive data types from the given
 *                    - structure chart in a form suitable to the
 *                    - extras table
 *                    - return TRUE iff successful
 */

static int _PD_wr_prim_typ_ii(FILE *fp, hasharr *tab)
   {int ni, ok, dc, rec;
    int *ordr;
    long i, j, n;
    long *formt;
    char *nm;
    defstr *dp;
    multides *tuple;

    ok  = TRUE;
    dc  = '\001';
    rec = '\002';

    ok &= _PD_put_string(1, "Primitive-Types:\n");

    for (i = 0; SC_hasharr_next(tab, &i, &nm, NULL, (void **) &dp); i++)
        {if (dp->members != NULL)
	    continue;

/* use nm instead of dp->type or PD_typedef's won't work!!! */
	 ok &= _PD_put_string(1, "%s%c%ld%c%d%c%d%c",
			      nm, dc,
			      dp->size, dc,
			      dp->alignment, dc,
			      dp->fix.order, dc);

/* write the byte order */
	 ordr = dp->fp.order;
	 if (ordr != NULL)
	    {ok &= _PD_put_string(1, "ORDER%c", dc);
	     n   = dp->size;
	     for (j = 0L; j < n; j++)
	         ok &= _PD_put_string(1, "%d%c", ordr[j], dc);}
	 else
	    ok &= _PD_put_string(1, "DEFORDER%c", dc);

/* write the floating point format */
	 formt = dp->fp.format;
	 if (formt != NULL)
	    {ok &= _PD_put_string(1, "FLOAT%c", dc);
	     for (j = 0L; j < 8; j++)
	         ok &= _PD_put_string(1, "%ld%c", formt[j], dc);}

	 else if (dp->fix.order == NO_ORDER)
	    ok &= _PD_put_string(1, "NO-CONV%c", dc);

	 else
	    ok &= _PD_put_string(1, "FIX%c", dc);

/* write the tuple info */
	 tuple = dp->tuple;
	 if (tuple != NULL)
	    {ni   = tuple->ni;
	     ordr = tuple->order;
	     ok  &= _PD_put_string(1, "TUPLE%c", dc);
	     ok  &= _PD_put_string(1, "%s%c%d%c", tuple->type, dc, ni, dc);
	     if (ordr == NULL)
	        ok &= _PD_put_string(1, "-1%c", dc);
	     else
	        {for (j = 0L; j < ni; j++)
		     ok &= _PD_put_string(1, "%d%c", ordr[j], dc);};};

/* write the unsgned flag */
	 ok &= _PD_put_string(1, "UNSGNED%c", dc);
	 ok &= _PD_put_string(1, "%d%c", dp->unsgned, dc);

/* write the ones complement flag */
	 ok &= _PD_put_string(1, "ONESCMP%c", dc);
	 ok &= _PD_put_string(1, "%d%c", dp->onescmp, dc);

/* write typedef typename
 * NOTE: typedefs are shallow copies to pre-existing defstrs
 */
	 if (strcmp(nm, dp->type) != 0)
	    ok &= _PD_put_string(1, "TYPEDEF%c%s%c", dc, dp->type, dc);

/* end of entry */
	 ok &= _PD_put_string(1, "\n");};

    ok &= _PD_put_string(1, "%c\n", rec);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WR_BLOCKS_II - write out the blocks for all variables in FILE */

static int _PD_wr_blocks_ii(PDBfile *file)
   {int ok;
    long i, j, n, ni;
    int64_t addr;
    char *nm;
    syment *ep;

    ok = TRUE;

    ok &= _PD_put_string(1, "Blocks:\n");

    for (i = 0; SC_hasharr_next(file->symtab, &i, &nm, NULL, (void **) &ep); i++)
        {n = _PD_n_blocks(ep);
	 if (n > 1)
	    {ok &= _PD_put_string(1, "%s\001%ld", nm, n);

	     for (j = 0L; j < n; j++)
	         {if ((j > 0) && ((j % 50) == 0))
		     _PD_put_string(1, "\n");

		  addr = _PD_entry_get_address(ep, j);
		  ni   = _PD_entry_get_number(ep, j);

		  ok &= _PD_put_string(1, " %lld %ld", (int64_t) addr, ni);};

	     ok &= _PD_put_string(1, "\n");};};

    ok &= _PD_put_string(1, "\002\n");

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WR_CSUM_II - write out the checksums for blocks of
 *                - all variables in FILE
 */

static int _PD_wr_csum_ii(PDBfile *file)
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

/* _PD_WR_PRIM_ALIGN - write alignment extras
 *                   - counterpart of _PD_rd_prim_align
 */

static int _PD_wr_prim_align(PDBfile *file)
   {int i, n, id, ap, ok;
    char al[MAXLINE];
    data_alignment *pl;
    data_standard *ps;

    ok = TRUE;

    pl = file->align;
    n  = 0;

/* prepare original C type data for Alignment */
    for (id = SC_CHAR_I; id < SC_WCHAR_I; id++)
        {i = SC_TYPE_CHAR(id);
	 al[n++] = pl->chr[i];};

    al[n++] = pl->ptr_alignment;

    for (id = SC_SHORT_I; id <= SC_LONG_I; id++)
        {i = SC_TYPE_FIX(id);
	 al[n++] = pl->fx[i];};

    for (id = SC_FLOAT_I; id <= SC_DOUBLE_I; id++)
        {i = SC_TYPE_FP(id);
	 al[n++] = pl->fp[i];};

    al[n] = '\0';

    ap = 1;
    for (i = 0; i < n; i++)
        ap *= al[i];

    if (ap == 0) 
       return(FALSE);

    ok &= _PD_put_string(1, "Alignment:%s\n", al);

/* write out struct alignment */
    ok &= _PD_put_string(1, "Struct-Alignment:%d\n",
			 file->align->struct_alignment);


/* write out the long long standard and alignment */
    i  = SC_TYPE_FIX(SC_LONG_LONG_I);
    ps = file->std;
    al[0] = ps->fx[i].bpi;
    al[1] = ps->fx[i].order;
    al[2] = pl->fx[i];
    al[3] = '\0';

    ok &= _PD_put_string(1, "Longlong-Format-Alignment:%s\n", al);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WR_EXT_II - write any extraneous data to the file
 *               - this is essentially a place for expansion of the file
 *               - to complete the definition of a PDB file the following
 *               - rule applies to extensions in the EXTRAS table:
 *               -
 *               -     An extension shall have one of two formats:
 *               -
 *               -        1) <name>:<text>\n
 *               -
 *               -        2) <name>:\n
 *               -           <text1>\n
 *               -           <text2>\n
 *               -              .
 *               -              .
 *               -              .
 *               -           <textn>\n
 *               -           [^B]\n
 *               -
 *               -     anything else is strictly illegal!!!!!!
 *               -     NOTE: the optional ^B is for backward compatibility
 *               -     and is not recommmended.
 */

static int _PD_wr_ext_ii(PDBfile *file, FILE *out)
   {int has_dirs, ok;
    long nbo, nbw;
    char *bf;
    FILE *fp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    if (file == NULL)
       return(FALSE);

    ok = TRUE;

    fp = file->stream;

/* write the default offset */
    ok &= _PD_put_string(1, "Offset:%d\n", file->default_offset);

/* write the alignment data */
    ok &= _PD_wr_prim_align(file);

/* write out the date and version data */
    ok &= _PD_put_string(1, "Version:%d|%s\n",
			 file->system_version, file->date);

/* write out the casts */
    ok &= _PD_wr_casts_ii(file);

/* write out the major order */
    ok &= _PD_put_string(1, "Major-Order:%d\n", file->major_order);

/* write out the previous file name (family) */
    if (file->previous_file != NULL)
       ok &= _PD_put_string(1, "Previous-File:%s\n", file->previous_file);

/* write out the directory indicator flag */
    has_dirs = PD_has_directories(file);
    ok &= _PD_put_string(1, "Has-Directories:%d\n", has_dirs);

    ok &= _PD_put_string(1, "Dynamic Spaces:%ld\n", file->n_dyn_spaces);
    ok &= _PD_put_string(1, "Use Itags:%ld\n", file->use_itags);

/* write out the primitives */
    ok &= _PD_rev_chrt(file->chart);

    ok &= _PD_wr_prim_typ_ii(fp, file->chart);

    ok &= _PD_rev_chrt(file->chart);

/* write out the blocks - this MUST follow at least the Major-Order extra
 * or else the read may improperly reconstruct the dimensions
 */
    ok &= _PD_wr_blocks_ii(file);
    ok &= _PD_wr_csum_ii(file);

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

    _PD_put_string(-1, NULL);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WR_FMT_II - write the primitive data format information to
 *               - the file header block
 *               - this is largely ignored in favor of the primitive
 *               - types in the extras section
 *               - however it must be written out the way versions
 *               - older than 24 expect in order for them to read
 *               - files written by versions 24 or newer
 *               -
 *               - Floating Point Format Descriptor
 *               -   format[0] = # of bits per number
 *               -   format[1] = # of bits in exponent
 *               -   format[2] = # of bits in mantissa
 *               -   format[3] = start bit of sign
 *               -   format[4] = start bit of exponent
 *               -   format[5] = start bit of mantissa
 *               -   format[6] = high order mantissa bit (CRAY needs this)
 *               -   format[7] = bias of exponent
 */

static int _PD_wr_fmt_ii(PDBfile *file)
   {int i, j, n, id, rv, sz, nw;
    int *order;
    long *format, fp_bias[2];
    char outfor[MAXLINE];
    char *nht, *p;
    data_standard *std;

    p   = outfor + 1;
    std = file->std;

    nht = _PD_header_token(2);
    sz  = strlen(nht) + 1;

    if (_PD_set_current_address(file, sz, SEEK_SET, PD_GENERIC) != 0)
       {PD_error("FSEEK FAILED - _PD_WR_FMT_II", PD_GENERIC);
	rv = FALSE;}

    else

/* get the byte lengths in */
       {*(p++) = std->ptr_bytes;

	for (id = SC_SHORT_I; id <= SC_LONG_I; id++)
	    {i = SC_TYPE_FIX(id);
	     *(p++) = std->fx[i].bpi;};

	for (id = SC_FLOAT_I; id <= SC_DOUBLE_I; id++)
	    {i = SC_TYPE_FP(id);
	     *(p++) = std->fp[i].bpi;};

/* get the integral types byte order in */
	for (id = SC_SHORT_I; id <= SC_LONG_I; id++)
	    {i = SC_TYPE_FIX(id);
	     *(p++) = std->fx[i].order;};

/* get the floating point format byte orders in */
	for (id = SC_FLOAT_I; id <= SC_DOUBLE_I; id++)
	    {i = SC_TYPE_FP(id);
	     order = std->fp[i].order;
	     n     = std->fp[i].bpi;
	     for (j = 0; j < n; j++, *(p++) = *(order++));};

/* get the floating point formats in */
	for (id = SC_FLOAT_I; id <= SC_DOUBLE_I; id++)
	    {i = SC_TYPE_FP(id);
	     format = std->fp[i].format;
	     n = FORMAT_FIELDS - 1;
	     for (j = 0; j < n; j++, *(p++) = *(format++));

	     fp_bias[i] = *format;};

	n         = (int) (p - outfor);
	outfor[0] = n;

	nw = lio_write(outfor, (size_t) 1, (size_t) n, file->stream);
	if (nw != n)
	   PD_error("FAILED TO WRITE FORMAT DATA - _PD_WR_FMT_II", PD_CREATE);
    
/* write out the biases */
	snprintf(outfor, MAXLINE, "%ld\001%ld\001\n", fp_bias[0], fp_bias[1]);
	n  = strlen(outfor);
	nw = lio_write(outfor, (size_t) 1, (size_t) n, file->stream);
	if (nw != n)
	   PD_error("FAILED TO WRITE BIASES - _PD_WR_FMT_II", PD_CREATE);

	rv = TRUE;};
    
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_OPEN_II - open an existing PDB file, extract the symbol table and
 *             - structure chart, and return a pointer to the PDB file
 *             - if successful else NULL
 */

static int _PD_open_ii(PDBfile *file)
   {char str[MAXLINE];
    char *token, *s;
    FILE *fp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    fp = pa->ofp;

    file->use_itags = TRUE;

/* read the primitive data type formats which set the standard */
    if (!_PD_rd_fmt_ii(file))
       PD_error("FAILED TO READ FORMATS - PD_OPEN", PD_OPEN);

/* record the current file position as the location of the symbol table
 * address and sequentially the chart address
 */
    file->headaddr = lio_tell(fp);
    if (file->headaddr == -1)
       PD_error("CAN'T FIND HEADER ADDRESS - PD_OPEN", PD_OPEN);

/* read the address of the symbol table and structure chart */
    if (_PD_rfgets(str, MAXLINE, fp) == NULL)
       PD_error("CAN'T READ SYMBOL TABLE ADDRESS - PD_OPEN", PD_OPEN);

    token = SC_strtok(str, "\001", s);
    if (token == NULL)
       PD_error("BAD STRUCTURE CHART ADDRESS - PD_OPEN", PD_OPEN);
    file->chrtaddr = SC_stol(token);

    token = SC_strtok(NULL, "\001", s);
    if (token == NULL)
       PD_error("BAD SYMBOL TABLE ADDRESS - PD_OPEN", PD_OPEN);
    file->symtaddr = SC_stol(token);

/* read the symbol table first so that the file pointer is positioned
 * to the "extra" information, then read the "extra's" to get the
 * alignment data, and finish with the structure chart which needs
 * the alignment data
 */

/* read the symbol table */
    if (lio_seek(fp, file->symtaddr, SEEK_SET))
       PD_error("FSEEK FAILED SYMBOL TABLE - PD_OPEN", PD_OPEN);
    if (!_PD_rd_symt_ii(file))
       PD_error("CAN'T READ SYMBOL TABLE - PD_OPEN", PD_OPEN);

/* read the miscellaneous data */
    if (!_PD_rd_ext_ii(file))
       PD_error("CAN'T READ MISCELLANEOUS DATA - PD_OPEN", PD_OPEN);

/* read the structure chart */
    if (lio_seek(fp, file->chrtaddr, SEEK_SET))
       PD_error("FSEEK FAILED STRUCTURE CHART - PD_OPEN", PD_OPEN);

    if (!_PD_rd_chrt_ii(file))
       PD_error("CAN'T READ STRUCTURE CHART - PD_OPEN", PD_OPEN);

    _PD_read_attrtab(file);

    if (file->use_itags == FALSE)
       _PD_ptr_open_setup(file);

/* position the file pointer to the location of the structure chart */
    if (lio_seek(fp, file->chrtaddr, SEEK_SET))
       PD_error("FSEEK FAILED CHART - PD_OPEN", PD_OPEN);

/* initialize the starting address for writing */
    _PD_SET_ADDRESS(file, file->chrtaddr);

/* if this file already contains a valid checksum, default file checksums to on */
    if (PD_verify(file) == TRUE)
       PD_activate_cksum(file, PD_MD5_FILE);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CREATE_II - initialize a PDB file 
 *               - if MST is TRUE this is a parallel master process
 *               - return TRUE iff successful
 */

static int _PD_create_ii(PDBfile *file, int mst)
   {int nstr;
    char str[MAXLINE];
    char *nht;
    FILE *fp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    fp = pa->cfp;

/* write the ASCII header */
    nht = _PD_header_token(2);

    if (mst == TRUE)
       {lio_printf(fp, "%s\n", nht);

/* write the primitive data type formats */
	if (!_PD_wr_fmt_ii(file))
	   PD_error("FAILED TO WRITE FORMATS - _PD_CREATE_II", PD_CREATE);

/* record the current file position as the location of the symbol table
 * address and sequentially the chart address
 */
	if ((file->headaddr = lio_tell(fp)) == -1)
	   PD_error("CAN'T FIND HEADER ADDRESS - _PD_CREATE_II", PD_CREATE);

	snprintf(str, MAXLINE, "%22ld\001%22ld\001\n", 0L, 0L);
	nstr = strlen(str);

	if (lio_write(str, (size_t) 1, nstr, fp) != nstr)
	   PD_error("FAILED TO WRITE ADDRESSES - _PD_CREATE_II", PD_CREATE);

	file->chrtaddr = file->headaddr + nstr;};

    if (lio_flush(fp))
       PD_error("FFLUSH FAILED AFTER HEADER - _PD_CREATE_II", PD_CREATE);

/* initialize the pdb system defs and structure chart */
    _PD_init_chrt(file, TRUE);

    file->use_itags = TRUE;

    if (mst == TRUE)
       {if (lio_seek(fp, file->chrtaddr, SEEK_SET))
	   PD_error("FAILED TO FIND START OF DATA - _PD_CREATE_II", PD_CREATE);

/* initialize the starting address for writing */
	_PD_SET_ADDRESS(file, file->chrtaddr);};

/* initialize directories */
    if (!PD_def_dir(file))
       PD_error("FAILED TO INITIALIZE DIRECTORIES - _PD_CREATE_II", PD_CREATE);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WRITE_META_II - dump the data description tables containing the current
 *                   - state of the PDB file in format version II form
 *                   - to the file OUT
 */

static int _PD_write_meta_ii(PDBfile *file, FILE *out, int fh)
   {int64_t addr;
    FILE *fp;

    if (out == NULL)
       out = stdout;

    fp = file->stream;

    if (_PD_safe_flush(file) == FALSE)
       PD_error("FFLUSH FAILED BEFORE CHART - _PD_WRITE_META_II", PD_WRITE);

/* seek the place to write the structure chart */
    addr = _PD_eod(file);
    _PD_set_current_address(file, addr, SEEK_SET, PD_WRITE);

    if (fp == out)
       file->chrtaddr = addr;

/* write the structure chart */
    addr = _PD_wr_chrt_ii(file, out, fh);
    if (addr == -1)
       PD_error("CAN'T WRITE STRUCTURE CHART - _PD_WRITE_META_II", PD_WRITE);

/* write the symbol table */
    addr = _PD_wr_symt_ii(file);
    if (addr == -1)
       PD_error("CAN'T WRITE SYMBOL TABLE - _PD_WRITE_META_II", PD_WRITE);

    if (fp == out)
       file->symtaddr = addr;

/* write the extras table */
    if (!_PD_wr_ext_ii(file, out))
       PD_error("CAN'T WRITE MISCELLANEOUS DATA - _PD_WRITE_META_II", PD_WRITE);

    if (lio_tell(fp) == -1)
       PD_error("CAN'T FIND HEADER ADDRESS - _PD_WRITE_META_II", PD_WRITE);

    if (_PD_safe_flush(file) == FALSE)
       PD_error("FFLUSH FAILED AFTER CHART - _PD_WRITE_META_II", PD_WRITE);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FLUSH_II - dump the data description tables containing the current
 *              - state of the PDB file in format version II
 *              - the tables are:
 *              -    structure chart
 *              -    symbol table
 *              -    extras table
 *              - the table addresses are also updated
 */

static int _PD_flush_ii(PDBfile *file)
   {int ok;
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
       {_PD_write_meta_ii(file, file->stream, 0);

	fp = file->stream;

/* update the header */
	_PD_set_current_address(file, file->headaddr, SEEK_SET, PD_WRITE);

	if (file->headaddr != lio_tell(fp))
	   PD_error("FSEEK FAILED TO FIND HEADER - _PD_FLUSH_II", PD_WRITE);

	lio_printf(fp, "%22lld\001%22lld\001\n",
		   (int64_t) file->chrtaddr, (int64_t) file->symtaddr);

/* WARNING: no more writes to the file before the space
 * reserved for the checksum from here on out
 */
	ok = _PD_csum_file_write(file);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SET_FORMAT_II - set the methods for a format version II file */

int _PD_set_format_ii(PDBfile *file)
   {

    file->open          = _PD_open_ii;
    file->create        = _PD_create_ii;
    file->flush         = _PD_flush_ii;

    file->wr_meta       = _PD_write_meta_ii;
    file->wr_symt       = _PD_wr_symt_ii;
    file->parse_symt    = _PD_parse_symt_ii;
    file->wr_prim_types = _PD_wr_prim_typ_ii;
    file->rd_prim_types = _PD_rd_prim_typ_ii;
    file->wr_fmt        = _PD_wr_fmt_ii;

    file->wr_itag       = _PD_wr_itag_ii;
    file->rd_itag       = _PD_rd_itag_ii;

    file->format_version = 2;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

 
