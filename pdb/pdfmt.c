/*
 * PDFMT.C - generic support for the different PDB format versions
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

#define NewHeadTok           "!<<PDB:"
#define OldHeadTok           "!<><PDB><>!"
#define PDB_ATTRIBUTE_TABLE  "!pdb_att_tab!"

#define BUFINCR   4096

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CAT_TYPE - concatenate type tokens to make a valid C type
 *              - expect the C type tokens to be followed by a
 *              - string representation of a number
 */

char *_PD_cat_type(char **sa, int *pit)
   {int it, nt;
    char t[BFSML];
    char *rv;

    it = *pit;

    for (nt = it; sa[nt] != NULL; nt++)
        {if (isdigit(sa[nt][0]))
	    break;};

    if (nt > it)
       {PS_concatenate(t, BFSML, sa, it, nt, " ");
	rv   = CSTRSAVE(t);
	*pit = nt;}
    else
       {rv   = NULL;
	*pit = -1;};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                           READ/WRITE SUPPORT                             */

/*--------------------------------------------------------------------------*/

/* _PD_GET_TOK - an fgets clone looks for specified char in addition to
 *             - the newline
 *             - it is also guaranteed to not split tokens in the input
 *             - stream
 */

static char *_PD_get_tok(char *s, int n, FILE *fp, int ch)
   {int i, c, nc, LineSep;
    long nb;
    int64_t loc;
    char *ps;

/* reserve the last character for a '\0' terminator */
    nc = n - 1;

/* this is for old NLTSS generated files - sigh! */
    LineSep = 0x1f;
    
/* find the current location and remember it */
    loc = lio_tell(fp);
    nb  = lio_read(s, 1, nc, fp);
    ps  = s;

/* check for EOF and reset the file pointer if at EOF */
    if (((c = *(ps++)) == EOF) || (nb == 0))
       {lio_seek(fp, loc, SEEK_SET);
        *s = '\0';
        return(NULL);};
    ps--;

/* search for \n, EOF, LineSep, or given delimiter */
    n = nb - 1;
    for (i = 0; i < n; i++)
        {c = *(ps++);
         if ((c == '\n') || (c == LineSep) || (c == ch))
            {ps--;
             *ps++ = '\0';
             loc += (long) (ps - s);
             break;}
         else if (c == EOF)
            {ps--;
             *ps++ = '\0';
             loc += (long) (ps - s + 1);
             break;};};

/* if we got a full buffer backup to the last space so as to not split
 * a token
 */
   if ((i >= n) && (c != '\n') && (c != LineSep) && (c == ch))
      {ps--;
       n >>= 1;
       for (; i > n; i--)
           {c = *(--ps);
            loc--;
	    if ((c == '\t') || (c == ' '))
               {*ps = '\0';
                break;};};};

/* reset the file pointer to the end of the string */
    lio_seek(fp, loc, SEEK_SET);

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_GET_TOKEN - get a token from a buffer which is handled like strtok
 *               - each time BF is non-NULL it is remembered for the next
 *               - call
 *               - the return buffer is handled like fgets with the space
 *               - and maximum size being passed in
 *               - this function is guaranteed to not split tokens in the
 *               - input buffer
 */

char *_PD_get_token(char *bf, char *s, int n, int ch)
   {int i;
    char c;
    char *ps;
    PD_smp_state *pa;
    static char leof = EOF;

/* this is for old NLTSS generated files - sigh! */
    static char LineSep = 0x1f;

    pa = _PD_get_state(-1);

    if (bf != NULL)
       pa->tbf = bf;

    ps = s;

/* check for EOF and reset the file pointer if at EOF */
    c     = *pa->tbf++;
    *ps++ = c;
    if ((c == leof) || (n == 0))
       {pa->tbf--;
        *s = '\0';
        return(NULL);};
    ps--;
    pa->tbf--;

/* search for \n, EOF, LineSep, or given delimiter */
    n--;
    for (i = 0; i < n; i++)
        {c = *ps++ = *pa->tbf++;
         if ((c == '\n') || (c == LineSep) || (c == ch))
            {ps--;
             *ps++ = '\0';
             break;}
         else if (c == leof)
            {ps--;
             pa->tbf--;
             *ps++ = '\0';
             break;};};

/* if we got a full buffer backup to the last space so as to not split
 * a token
 */
   if ((i >= n) && (c != '\n') && (c != LineSep) && (c == ch))
      {ps--;
       pa->tbf--;
       n >>= 1;
       for (; i > n; i--)
           {c = *(--ps) = *(--pa->tbf);
	    if ((c == '\t') || (c == ' '))
               {*ps = '\0';
                break;};};};

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RFGETS - our fgets looks for specified line separator in addition to
 *            - the given system version
 *            - it is also guaranteed to not split tokens in the input
 *            - stream
 */

char *_PD_rfgets(char *s, int n, FILE *fp)
   {char *rv;

    rv = _PD_get_tok(s, n, fp, '\n');

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PUT_TEXT - build up the contents of the current output buffer
 *              - return TRUE iff the number of characters added is > 0
 */

int _PD_put_text(int reset, int ns, char *s)
   {int ok;
    long nc, ncx;
    char *bf, *spl;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    nc  = pa->nc;
    ncx = pa->ncx;
    spl = pa->spl;
    bf  = pa->tbuffer;

    if (reset == -1)
       {CFREE(bf);
	spl = NULL;
	nc  = 0;
	ncx = 0;
	ok  = FALSE;}

    else
       {if (bf == NULL)
	   {ncx = BUFINCR;
	    bf  = CMAKE_N(char, ncx);
	    spl = bf;
	    nc  = 0;}

        else if (reset == 0)
	   {spl = bf;
	    nc  = 0;
	    memset(bf, 0, ncx);};
    
	if (nc + ns + 10 > ncx)
	   {ncx += BUFINCR;
	    CREMAKE(bf, char, ncx);
	    spl = bf + strlen(bf);};

	SC_strncpy(spl, ncx, s, ns+1);
	ok = ((ns > 0) && (strcmp(spl, s) == 0));

	spl += ns;
	nc  += ns;};

    pa->nc      = nc;
    pa->ncx     = ncx;
    pa->spl     = spl;
    pa->tbuffer = bf;

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PUT_STRING - build up the contents of the current output buffer
 *                - in an fprintf style
 *                - return TRUE iff the number of characters added is > 0
 */

int _PD_put_string(int reset, const char *fmt, ...)
   {int ok;
    long ns;
    char *s;

    if (reset == -1)
       {s  = NULL;
	ns = 0;}
    else
       {SC_VDSNPRINTF(FALSE, s, fmt);

	ns = strlen(s);};

    ok = _PD_put_text(reset, ns, s);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_CP_TBUFFER - copy the current contents of PA->TBUFFER into a
 *                - newly allocated buffer
 * 
 */

int _PD_cp_tbuffer(char **buf)
   {int nc;
    char *bf;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    bf   = pa->tbuffer;
    nc   = strlen(bf) + 1;
    *buf = CMAKE_N(char, nc);

    strcpy(*buf, bf);

    return(nc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_GET_TBUFFER - return a copy of the current output buffer */

char *_PD_get_tbuffer(void)
   {int nc;
    char *ret, *bf;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    bf  = pa->tbuffer;
    nc  = strlen(bf) + 1;
    ret = CMAKE_N(char, nc);

    SC_strncpy(ret, nc, bf, -1);

    return(ret);}

/*--------------------------------------------------------------------------*/

/*                            FORMAT VERSION                                */

/*--------------------------------------------------------------------------*/

/* _PD_HEADER_TOKEN - return the old or new header token */

char *_PD_header_token(int which)
   {

    if (which < 2)
       SC_strncpy(_PD.id_token, MAXLINE, OldHeadTok, -1);

    else if (which == 2)
       snprintf(_PD.id_token, MAXLINE, "%s%s>>!", NewHeadTok, "II");

    else
       snprintf(_PD.id_token, MAXLINE, "%s%d>>!", NewHeadTok, which);

    return(_PD.id_token);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_IDENTIFY_VERSION - return the format version number
 *                      - corresponding to the tag S
 *                      - or -1 iff the file is not a PDB file
 */

int _PD_identify_version(char *s)
   {int vers;
    char *p, *r, *t;

    vers = -1;

    p = SC_strstr(s, NewHeadTok);
    if (p != NULL)
       {t = SC_strtok(p+7, ">", r);
	if (t != NULL)
	   {if (strcmp(t, "II") == 0)
	       vers = 2;
	    else if (SC_numstrp(t) == TRUE)
	       vers = SC_stoi(t);};}

    else if (strncmp(s, OldHeadTok, strlen(OldHeadTok)) == 0)
       vers = 1;

    return(vers);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_ID_FILE - identify the format version of the FILE FP
 *             - set the methods appropriately and
 *             - return the format version number
 *             - or -1 iff the file is not a PDB file
 */

static int _PD_id_file(FILE *fp)
   {int vers, nb;
    char str[MAXLINE];

    memset(str, 0, MAXLINE);

/* attempt to read an ASCII header */
    if (lio_seek(fp, (int64_t) 0, SEEK_SET))
       PD_error("FSEEK FAILED TO FIND ORIGIN - _PD_IDENTIFY_FILE", PD_OPEN);

    if (_PD_rfgets(str, MAXLINE, fp) == NULL)
       vers = -1;

    else
       {str[MAXLINE-1] = '\0';

/* the first token should be the identifying header token */
	vers = _PD_identify_version(str);

/* if the header does not give us a version, try the trailer */
	if (vers < 1)

/* attempt to read an ASCII trailer */
	   {if (lio_seek(fp, (int64_t) -32, SEEK_END))
	       PD_error("FSEEK FAILED TO END - _PD_IDENTIFY_FILE", PD_OPEN);

	    nb = lio_read(str, (size_t) 1, (size_t) 32, fp);
            SC_ASSERT(nb == 32);

	    str[32] = '\0';

	    vers = _PD_identify_version(str);};};

    return(vers);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_IDENTIFY_FILE - identify the format version of the PDBfile FILE
 *                   - set the methods appropriately and
 *                   - return the format version number
 *                   - or -1 iff the file is not a PDB file
 */

int _PD_identify_file(PDBfile *file)
   {int vers, ok;
    FILE *fp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    fp = pa->ofp;

    vers = _PD_id_file(fp);
    if (vers != -1)
       {ok = _PD_format_version(file, vers);
	SC_ASSERT(ok == TRUE);};

    return(vers);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FORMAT_VERSION - set the format version for FILE to VERS */

int _PD_format_version(PDBfile *file, int vers)
   {

    if ((vers < 1) || (3 < vers))
       vers = PD_gs.default_format_version;

    switch (vers)
       {case 1 :
	     _PD_set_format_i(file);
	     break;

	default :
	case 2 :
	     _PD_set_format_ii(file);
	     break;

        case 3 :
	     _PD_set_format_iii(file);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_ISFILE - Return TRUE if FNAME is a PDB file and
 *           - return FALSE otherwise.
 *
 * #bind PD_isfile fortran() scheme(pd-isfile?) python()
 */

int PD_isfile(const char *fname)
   {int vers, ok;
    FILE *fp;

    ok = FALSE;

    if (fname != NULL)
       {fp = io_open(fname, "r");
	if (fp != NULL)
	   {vers = _PD_id_file(fp);
	    ok   = (vers > 0);

	    io_close(fp);};};

    return(ok);}

/*--------------------------------------------------------------------------*/

/*                             ATTRIUBUTE_TABLE                             */

/*--------------------------------------------------------------------------*/

/* _PD_WRITE_ATTRTAB - write the attribute table out */

int _PD_write_attrtab(PDBfile *file)
   {int ok;
    char *ob;
    syment *ep;
    adloc *oa;

    if (file->attrtab == NULL)
       return(TRUE);

/* write attribute table out in its own space */
    if (PDB_SYSTEM_VERSION > 18)
       {ep = PD_inquire_entry(file, "/&etc", TRUE, NULL);
	if (ep == NULL)
	   PD_mkdir(file, "/&etc");
	PD_cd(file, "/&etc");

	PD_reset_ptr_list(file);

	_PD_ptr_save_al(file, &oa, &ob, "/&etc/ia_");

	ok = PD_write(file, "attributes", "hasharr *", &file->attrtab);

	_PD_ptr_restore_al(file, oa, ob);

	PD_cd(file, NULL);}

    else
       {ok = TRUE;
	if (file->attrtab != NULL)
	   {PD_cd(file, NULL);
	    PD_reset_ptr_list(file);
	    ok = PD_write(file, PDB_ATTRIBUTE_TABLE,
			  "hasharr *", &file->attrtab);};};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_READ_ATTRTAB_A - read the attribute table
 *                    - for PDB files prior to version 19
 */

int _PD_read_attrtab_a(PDBfile *file)
   {int ok;
    char *name;
    syment *ep;
    adloc *oa;

    name = PDB_ATTRIBUTE_TABLE;

    ok = FALSE;
    ep = PD_inquire_entry(file, name, TRUE, NULL);
    if (ep != NULL)
       {_PD_ptr_save_al(file, &oa, NULL, NULL);

	if (!PD_read(file, name, &file->attrtab))
           {PD_close(file);
            PD_error("FAILED TO READ ATTRIBUTE TABLE - _PD_READ_ATTRTAB_A",
		     PD_OPEN);};

/* reset the pointer lists after reading the attribute table */
	_PD_ptr_restore_al(file, oa, NULL);
	if (file->use_itags == FALSE)
	   _PD_ptr_open_setup(file);

	_PD_convert_attrtab(file);
        file->chrtaddr = PD_entry_address(ep);
        _PD_rl_syment(ep);
    
	_PD_MARK_AS_FLUSHED(file, FALSE);

        if (!SC_hasharr_remove(file->symtab, _PD_fixname(file, name)))
	   SC_hasharr_remove(file->symtab, name);

	ok = TRUE;};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_READ_ATTRTAB_B - read the attribute table
 *                    - for PDB files after version 18
 */

int _PD_read_attrtab_b(PDBfile *file)
   {int ok;
    char *name, *ob;
    syment *ep;
    adloc *oa;

    name = "/&etc/attributes";

    ok = FALSE;
    ep = PD_inquire_entry(file, name, TRUE, NULL);
    if (ep != NULL)
       {_PD_ptr_save_al(file, &oa, &ob, "/&etc/ia_");

	if (file->use_itags == FALSE)
	   _PD_ptr_open_setup(file);

	if (!PD_read(file, name, &file->attrtab))
           {PD_close(file);
            PD_error("FAILED TO READ ATTRIBUTE TABLE - _PD_READ_ATTRTAB_B",
		     PD_OPEN);};

	_PD_ptr_restore_al(file, oa, ob);

	_PD_convert_attrtab(file);
        file->chrtaddr = PD_entry_address(ep);
    
	_PD_MARK_AS_FLUSHED(file, FALSE);

        ok = SC_hasharr_remove(file->symtab, _PD_fixname(file, name));
        if (ok == FALSE)
	   SC_hasharr_remove(file->symtab, name);

	ok = TRUE;};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_READ_ATTRTAB - read the attribute table in */

int _PD_read_attrtab(PDBfile *file)
   {int ok;

    ok = _PD_read_attrtab_a(file);
    if ((ok == FALSE) && (PDB_SYSTEM_VERSION > 18))
       ok = _PD_read_attrtab_b(file);

    if (ok == FALSE)
       file->attrtab = NULL;

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

 
