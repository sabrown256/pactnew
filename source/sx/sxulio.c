/*
 * SXULIO.C - handle I/O for ULTRA type data (curves)
 *
 * Source Version: 4.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

#define write_int(n, fp)   io_write((char *) &n, sizeof(int), 1, fp)
#define read_int(n, fp)    io_read((char *) &n, sizeof(int), 1, fp)

typedef struct s_bin_info bin_info;
typedef struct s_pdb_info pdb_info;

struct s_bin_info
  {FILE *stream;                            /* file pointer for binary file */
   off_t fileaddr;};                                /* disk address of data */

struct s_pdb_info
  {PDBfile *file;                           /* file pointer for binary file */
   char *curve_name;};                            /* curve name in PDB file */

int
 SX_n_curves_read = 0,
 _SX_next_available_number = 1;

FILE
 *SX_out_text,
 *SX_out_bin;

PDBfile
 *SX_out_pdb;
 
static char
 Ultra_Hdr[] = "ULTRA II - BINARY FILE";

static void
 SX_cache_curve(curve *crv, SC_file_type type);

/*--------------------------------------------------------------------------*/

/*                            LOW-LEVEL ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* SX_NEXT_NUMBER - return the index for the next available curve menu slot */

static int SX_next_number(int flag)
   {int i;

    for (i = _SX_next_available_number; i < SX_N_Curves; i++)
        if (SX_number[i] == -1)
           {if (flag)
               _SX_next_available_number = i + 1;
            return(i);};

    SX_enlarge_dataset(NULL);

    i = SX_next_number(flag);

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_TERMDATA - terminate one set of data and prepare for the next
 *             - return TRUE iff successful
 */

static int SX_termdata(int *aryptr, double *xbuff, double *ybuff)
   {int i, j;
    double wc[PG_BOXSZ];
    double *x[PG_SPACEDM];

    if (_SX.dataptr <= 0)
       {_SX.dataptr = 0;
        return(FALSE);};

    if ((xbuff == NULL) || (ybuff == NULL))
       {_SX.dataptr = 0;
        return(FALSE);};

    j = *aryptr;

    REMAKE_N(xbuff, double, _SX.dataptr);
    x[0] = SX_dataset[j].x[0] = xbuff;

    REMAKE_N(ybuff, double, _SX.dataptr);
    x[1] = SX_dataset[j].x[1] = ybuff;

/* bail out if not enough memory */
    if ((x[0] == NULL) || (x[1] == NULL))
       SS_error("INSUFFICIENT MEMORY - SX_TERMDATA", SS_null);

    if ((xbuff[_SX.dataptr-1] == -999) && (ybuff[_SX.dataptr-1] == -999))
       --_SX.dataptr;

    PM_maxmin(SX_dataset[j].x[0], &wc[0], &wc[1], _SX.dataptr);
    PM_maxmin(SX_dataset[j].x[1], &wc[2], &wc[3], _SX.dataptr);

    SX_dataset[j].id = ' ';
    SX_dataset[j].n  = _SX.dataptr;

    PG_box_copy(2, SX_dataset[j].wc, wc);

    SC_CHANGE_VALUE_ALIST(SX_dataset[j].info,
			  int, SC_INTEGER_P_S,
			  "LINE-COLOR", 0);

/* put this curve's data in a cache somewhere (let us defer the question
 * about memory management and also to handle all curve sources the same
 * way
 */
    SX_cache_curve(&SX_dataset[j], SC_ASCII);

    _SX.dataptr  = 0;
    i            = SX_next_number(TRUE);
    SX_number[i] = j;
    *aryptr      = SX_next_space();

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_READ_PDB_CURVE - read a specifed curve (NAME) from the PDBfile, FP,
 *                   - into the curve pointed to by CRV
 *                   - return TRUE if successful and FALSE otherwise
 */

static int SX_read_pdb_curve(PDBfile *fp, char *fname, char *cname,
			     curve *crv, PD_curve_io flag)
   {int n, rv;
    double wc[PG_BOXSZ];
    double *x[PG_SPACEDM];
    char label[MAXLINE];

    rv = FALSE;

    if (PD_read_pdb_curve(fp, cname, &x[0], &x[1], &n, label,
			  &wc[0], &wc[1], &wc[2], &wc[3], flag))
       {crv->x[0] = x[0];
	crv->x[1] = x[1];

/* set up the rest of the curve data */
	if (fname != NULL)
	   {if ((wc[0] == HUGE) && (wc[1] == -HUGE) && (x[0] != NULL))
	       PM_maxmin(x[0], &wc[0], &wc[1], n);

	    if ((wc[2] == HUGE) && (wc[3] == -HUGE) && (x[1] != NULL))
	       PM_maxmin(x[1], &wc[2], &wc[3], n);

	    PG_box_copy(2, crv->wc, wc);

	    crv->id   = ' ';
	    crv->n    = n;
	    crv->text = SC_strsavef(label, "char*:SX_READ_PDB_CURVE:label");
	    crv->file = SC_strsavef(fname, "char*:SX_READ_PDB_CURVE:fname");};

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_WRT_PDB_CURVE - write the specified curve out to the given file
 *                  - as the icurve'th curve in the file
 */

static void SX_wrt_pdb_curve(PDBfile *fp, curve *crv, int icurve)
   {

    if (!PD_wrt_pdb_curve(fp, crv->text, crv->n, crv->x[0], crv->x[1], icurve))
       {PRINT(stdout, "%s\n", PD_err);
        SS_error("CAN'T WRITE THE CURVE - WRT_PDB_CURVE", SS_null);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_ULTRA_TEXT_FILEP - test for ascii ULTRA file
 *                      - return TRUE if FP conforms to this pattern
 */

static int _SX_ultra_text_filep(FILE *fp, int cmnt)
   {char bf[MAXLINE];
    int i, n;
    off_t addr;

    while (TRUE)
       {addr = io_tell(fp);

        if (io_gets(bf, MAXLINE, fp) == NULL)
           break;

        if (bf[0] != cmnt)
           continue;

        n = strlen(bf) - 1;
        for (i = 1; i < n; i++)
            if (!SC_is_print_char(bf[i], 0))
               return(FALSE);

        io_seek(fp, addr, SEEK_SET);

        return(TRUE);};

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_ULTRA_BINARY_FILEP - test for binary file
 *                       - return TRUE if this is an ULTRA binary file
 *                       - that is it MUST be one of
 *                       -   1) PDB file
 *                       -   2) ASCII ULTRA file
 *                       -   3) ULTRA II binary file (not ULTRA I file)
 *                       - the reason is that a random binary file
 *                       - is indistinguishable from an ULTRA I file
 *                       - by any test save to coredump ULTRA II
 */

static int SX_ultra_binary_filep(FILE *fp)
   {char bf[MAXLINE];
    int hdr_sz;

    hdr_sz = strlen(Ultra_Hdr) + 1;
    if (io_read(bf, (size_t) sizeof(char), (size_t) MAXLINE, fp) != MAXLINE)
       return(FALSE);

/* check for ULTRA II binary files */
    if (strncmp(bf, Ultra_Hdr, hdr_sz) == 0)

/* set the file pointer back to the beginning of the data */
       {io_seek(fp, (off_t) hdr_sz, SEEK_SET);
        return(TRUE);};

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_OPEN_FOR_READING - open a file so that it can be read AND have
 *                     - its pointer moved around
 */

static FILE *SX_open_for_reading(char *str, char *mode)
   {FILE *fp;

    fp = io_open(str, mode);
    if (fp == NULL)
       SS_error("CAN'T OPEN FILE - SX_OPEN_FOR_READING", SS_mk_string(str));

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_VALID_ULTRA_FILEP - return #t iff the file is a valid ULTRA II
 *                        - input file
 */

object *_SXI_valid_ultra_filep(object *obj)
   {FILE *fp;
    PDBfile *pfp;
    char fname[MAXLINE], *path;
    object *ret;

    ret = SS_f;

    strcpy(fname, SS_get_string(obj));
    path = SC_search_file(NULL, fname);
    if (path != NULL)

/* test for a PDB file */    
       {pfp = PD_open(path, "r");
	if (pfp != NULL)
	   {int ok;
	    char **cu, **cv, **mo, **mp;

	    cu  = SC_hasharr_dump(pfp->symtab, "curve*", NULL, FALSE);
	    cv  = SC_hasharr_dump(pfp->symtab, "/curve*", NULL, FALSE);
	    mo  = SC_hasharr_dump(pfp->symtab, "Mapping*", NULL, FALSE);
	    mp  = SC_hasharr_dump(pfp->symtab, "/Mapping*", NULL, FALSE);

	    ok = (((cu[0] != NULL) || (cv[0] != NULL)) &&
		  (mo[0] == NULL) &&
		  (mp[0] == NULL));

	    ret = ok ? SS_t : SS_f;

	    SFREE(cu);
	    SFREE(cv);
	    SFREE(mo);
	    SFREE(mp);

	    PD_close(pfp);}

/* test for other file types
 * remake the name first because a strcpy in PD_open clobbers it
 */
	else
	   {fp = SX_open_for_reading(path, BINARY_MODE_R);
	    if (SX_ultra_binary_filep(fp))
	       {io_close(fp);
		ret = SS_t;}

	    else
	       {io_close(fp);

		if (SC_isfile_text(path))
		   {fp = SX_open_for_reading(path, "r");
		    if (_SX_ultra_text_filep(fp, '#'))
		       {io_close(fp);
			ret = SS_t;};};};};

	SFREE(path);};

    return(ret);}

/*--------------------------------------------------------------------------*/

/*                            TOP-LEVEL ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* SX_READ_BIN - read a binary file */

static void SX_read_bin(FILE *fp, char *fname)
   {int n, nc, len, j, i, icurve;
    double wc[PG_BOXSZ];
    char c, bf[MAXLINE];
    bin_info *pbi;

/* remember this file pointer */
    SX_push_open_file(fp);

    icurve = 0;
    while (!feof(fp))
       {j = SX_next_space();
        read_int(len, fp);
	nc = min(len, MAXLINE);
        if (io_read(bf, sizeof(char), nc, fp) != len)
           break;
        bf[len] = '\0';

        SX_dataset[j].text = SC_strsavef(bf, "char*:SX_READ_BIN:text");
        SX_dataset[j].file = SC_strsavef(fname, "char*:SX_READ_BIN:fname");
                
        read_int(n, fp);

        SX_dataset[j].n = n;
        SX_dataset[j].x[0] = FMAKE_N(double, n, "SX_READ_BIN:x[0]");
        SX_dataset[j].x[1] = FMAKE_N(double, n, "SX_READ_BIN:x[1]");
        if ((SX_dataset[j].x[0] == NULL) || (SX_dataset[j].x[1] == NULL))
           SS_error("INSUFFICIENT MEMORY - READ_BIN", SS_null);

/* set up the file info for this curve */
        pbi = FMAKE(bin_info, "SX_READ_BIN:pbi");
        pbi->stream = fp;
        pbi->fileaddr = io_tell(fp);

        SX_dataset[j].file_info = (void *) pbi;
        SX_dataset[j].file_type = SC_BINARY;

        if (io_read((char *) SX_dataset[j].x[0], sizeof(double), n, fp) != n)
           {PRINT(stdout, "WARNING: INCOMPLETE CURVE %d IN BINARY FILE",
                          icurve + 1);
            break;};
        if (io_read((char *) SX_dataset[j].x[1], sizeof(double), n, fp) != n)
           {PRINT(stdout, "WARNING: INCOMPLETE CURVE %d IN BINARY FILE",
                          icurve + 1);
            break;};
                
        PM_maxmin(SX_dataset[j].x[0], &wc[0], &wc[1], n);
        PM_maxmin(SX_dataset[j].x[1], &wc[2], &wc[3], n);

        SX_dataset[j].id   = ' ';
        SX_dataset[j].n    = n;

	PG_box_copy(2, SX_dataset[j].wc, wc);

/* put this curve's data in a cache somewhere (let's us defer the question
 * about memory management) and also handle all curve sources the same way
 */
        SX_cache_curve(&SX_dataset[j], SC_BINARY);

        icurve++;

        i = SX_next_number(TRUE);
        SX_number[i] = j;
        c = io_getc(fp);                                    /* look for eof */
        io_ungetc(c, fp);};

    SX_n_curves_read += icurve;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_READ_TEXT - read an ASCII input file */

static void SX_read_text(FILE *fp, char *fname)
   {char instring[MAXLINE];
    int j, c, icurve;
    char *pin, *text, *s;
    unsigned current_size;                        /* current size of buffer */
    double *xbuff, *ybuff, *x[PG_SPACEDM], xval, yval;

    j           = SX_next_space();
    _SX.dataptr = 0;
    icurve      = 0;

    x[0] = NULL;
    x[1] = NULL;
    while ((io_gets(instring, MAXLINE, fp) != NULL) && (j >= 0))
       {pin = instring;
        c   = *pin;

        if (c == EOF)
           break;

        if (c == '#')

/* don't terminate the first one */
           {if (_SX.dataptr != 0)
               {if (SX_termdata(&j, xbuff, ybuff))
                   icurve++;};

/* strip off leading whitespace from label */
            pin += strspn(pin, "# \t");

/* GOTCHA - if there are multiple consecutive lines beginning with '#'
 *          (i.e. comments), the following code allocates, but never
 *          releases space for text, fname, xbuff, and ybuff.
 */

/* get rid of \n */
            text = SC_strtok(pin, "\n\r", s);
            if (text == NULL)
	       SX_dataset[j].text = SC_strsavef("",
						"char*:SX_READ_TEXT:NULL");
	    else
	       SX_dataset[j].text = SC_strsavef(text,
						"char*:SX_READ_TEXT:text");
            SX_dataset[j].file = SC_strsavef(fname,
					     "char*:SX_READ_TEXT:fname");

/* allocate space for buffer */
            current_size = MAXPTS;
            x[0] = xbuff = FMAKE_N(double, current_size, "SX_READ_TEXT:x[0]");
            x[1] = ybuff = FMAKE_N(double, current_size, "SX_READ_TEXT:x[1]");}
        else
           {if ((x[0] == NULL) || (x[1] == NULL))
               continue;

            for (text = SC_strtok(instring, " \t\n\r", s);
                 text != NULL;
                 text = SC_strtok(NULL, " \t\n\r", s))
                {if (SC_fltstrp(text))
                    xval = ATOF(text);
                 else
                    break;

                 text = SC_strtok(NULL, " \t\n\r", s);
                 if (text == NULL)
                    break;
                 else if (SC_fltstrp(text))
                    yval = ATOF(text);
                 else
                    break;

                 *x[0]++ = xval;
                 *x[1]++ = yval;
                 _SX.dataptr++;

/* get more space if needed */
                 if (_SX.dataptr >= current_size)
                    {current_size += MAXPTS;
                     REMAKE_N(xbuff, double, current_size);
                     REMAKE_N(ybuff, double, current_size);
                     x[0] = xbuff + _SX.dataptr;
                     x[1] = ybuff + _SX.dataptr;};};};};

    if (SX_termdata(&j, xbuff, ybuff))
       icurve++;
    io_close(fp);

    if (icurve == 0)
       PRINT(stdout, "%s FILE HAS NO LEGAL CURVES - SX_READ_TEXT: %s\n\n",
	     "WARNING:", fname);

    SX_n_curves_read += icurve;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_READ_VER1 - Read data from ULTRA I file
 *              - and put it in the available space
 */

object *SX_read_ver1(object *obj)
   {int j, k;
    char fname[MAXLINE], *path;
    FILE *fp;

    j = SX_next_number(FALSE);

    strcpy(fname, SS_get_string(obj));
    path = SC_search_file(NULL, fname);
    if (path == NULL)
       SS_error("CAN'T FIND FILE - SX_READ_VER1", obj);

    fp = SX_open_for_reading(path, "r");
    if (fp == NULL)
       {SS_error("NON EXISTENT FILE - SX_READ_VER1", obj);}

    else

/* if this dies it is because you lied in claiming this to be a
 * version 1 file
 * there is no safety net here - version 1 files are not to be supported
 * in perpetuity!
 */
       {SX_read_bin(fp, path);
	k = SX_next_prefix();
	SX_prefix_list[k] = j;};

    SFREE(path);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_READ_PDB - read the curves from a PDB file */

static void SX_read_pdb(PDBfile *fp, char *fname)
   {int i, j, k, ne, icurve;
    char **names;
    pdb_info *ppi;

    icurve = 0;

/* get an alphabetic list of curves */
    ne    = SC_hasharr_get_n(fp->symtab);
    names = SC_hasharr_dump(fp->symtab, "*curve*", NULL, FALSE);
      
    if (names == NULL)
       SS_error("NO CURVES IN FILE OR INSUFFICIENT MEMORY - SX_READ_PDB",
		SS_null);

    else
       {for (i = 0; (names[i] != NULL) && (i < ne); i++)

/* get the next open space */
	    {j = SX_next_space();

	     if (SX_read_pdb_curve(fp, fname, names[i],
				   &SX_dataset[j], NOR_X_Y) == FALSE)
	        PRINT(stdout,
		      "WARNING: NO DATA READ FOR CURVE %s - SKIPPED\n",
		      names[i]);
	     else
	        {icurve++;

/* set up the file info for this curve */
		 ppi = FMAKE(pdb_info, "SX_READ_PDB:ppi");
		 ppi->file = fp;
		 ppi->curve_name = SC_strsavef(names[i],
					       "char*:SX_READ_PDB:cur_name");
		 SX_dataset[j].file_info = (void *) ppi;
		 SX_dataset[j].file_type = SC_PDB;

/* put this curve's data in a cache somewhere (let's us defer the question
 * about memory management) and also handle all curve sources the same way
 */
		 SX_cache_curve(&SX_dataset[j], SC_PDB);

		 k = SX_next_number(TRUE);
		 SX_number[k] = j;};};

	SX_n_curves_read += icurve;
	SFREE(names);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_READ_DATA - Read data from file and put it in the available space */

object *SX_read_data(object *obj)
   {int j, k;
    char fname[MAXLINE], *path;
    FILE *fp;
    PDBfile *pfp;
    object *rv;

    strcpy(fname, SS_get_string(obj));
    path = SC_search_file(NULL, fname);
    if (path == NULL)
       SS_error("CAN'T FIND FILE - SX_READ_DATA", obj);

    j = SX_next_number(FALSE);

/* test for a PDB file */    
    pfp = PD_open(path, "r");
    if (pfp != NULL)
       {SX_read_pdb(pfp, fname);
        fp = pfp->stream;
        SX_push_open_file(fp);
        k = SX_next_prefix();
        SX_prefix_list[k] = j;

        rv = SS_t;}

    else

/* test for other file types
 * remake the name first because a strcpy in PD_open clobbers it
 */
       {fp = SX_open_for_reading(path, BINARY_MODE_R);

	if (SX_ultra_binary_filep(fp))
	   {SX_read_bin(fp, fname);
	    k = SX_next_prefix();
	    SX_prefix_list[k] = j;
	    rv = SS_t;}

	else
	   {io_close(fp);

	     fp = SX_open_for_reading(path, "r");
	     if (_SX_ultra_text_filep(fp, '#'))
	        {SX_read_text(fp, fname);
		 k = SX_next_prefix();
		 SX_prefix_list[k] = j;
		 rv = SS_t;}

	     else
	        {SS_error("FILE NOT LEGAL ULTRA II FILE - SX_READ_DATA", obj);

		 rv = SS_f;};};};

    SFREE(path);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_CRV_FILE_INFO - report the basic info for a PDB file */

object *SX_crv_file_info(object *obj)
   {char fname[MAXLINE], *path;
    PDBfile *pfp;
    object *o;

    strcpy(fname, SS_get_string(obj));
    path = SC_search_file(NULL, fname);
    if (path == NULL)
       SS_error("CAN'T FIND FILE - SX_CRV_FILE_INFO", obj);

    o = SS_f;

/* test for a PDB file */    
    pfp = PD_open(path, "r");
    if (pfp != NULL)
       {PRINT(stdout, "\n");
        PRINT(stdout, "PDB System Version: %d\n", pfp->system_version);
        PRINT(stdout, "Creation date: %s\n", pfp->date);
        if (pfp->major_order == ROW_MAJOR_ORDER)
           PRINT(stdout, "Written by C interface (row major order)\n");
        else
           PRINT(stdout, "Written by FORTRAN interface (column major order)\n");
        PRINT(stdout, "\n");

        PD_close(pfp);
        o = SS_t;};

    SFREE(path);

    return(o);}

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

static int SX_find_text_table(FILE *fp, int n, int *pfn, int *pnr, int *pnc,
			      long nl, off_t *paddrt, int nlab,
			      off_t *paddrl)
   {int i, j, nc, nr, nt, rv, firstnum, nbefore, nafter, nlb;
    off_t *addr, addrt, naddr, addrl;
    char *token;

    addrl   = -1;
    nbefore = 0;
    nafter  = 0;
    if (nlab > 0)
       nbefore = nlab;
    else
       nafter = -nlab;

    addr = FMAKE_N(off_t, nbefore+1, "SX_FIND_TEXT_TABLE:addr");

    nr  = 0;
    nc  = 0;
    nlb = 0;

/* skip to specified line */
    for (i = 1; i < nl; i++)
	{addr[nlb++] = io_tell(fp);
	 if (nlb > nbefore)
	    nlb = 0;
	 if (io_gets(_SX.line, MAX_BFSZ, fp) == NULL)
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

             if (io_gets(_SX.line, MAX_BFSZ, fp) == NULL)
                break;

/* loop over tokens */
             for (nt = 0; TRUE; nt++)
                 {token = SC_firsttok(_SX.line, " \t\n\r");
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
              if (io_gets(_SX.line, MAX_BFSZ, fp) == NULL)
                 break;

              for (nt = 0; TRUE; nt++)
                  {token = SC_firsttok(_SX.line, " \t\n\r");
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
             if (io_gets(_SX.line, MAX_BFSZ, fp) == NULL)
                return(FALSE);};};

    SFREE(addr);

    rv = FALSE;
    if ((j == n) && (nc > 0))
       {*pfn    = firstnum;
        *pnc    = nc;
        *pnr    = nr;
        *paddrt = addrt;
        *paddrl = addrl;
	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_READ_TEXT_TABLE - read a table of numbers from an  ASCII input file */

object *SX_read_text_table(object *argl)
   {int i, j, n, nc, nr, fn, nlabel, ok;
    off_t addrt, addrl; 
    long nl;
    char *name, *token, label[MAXLINE];
    FILE *fp;
    object *o;

    memset(label, 0, MAXLINE);

    if (_SX.current_table != NULL)
       {PM_destroy(_SX.current_table);
        _SX.current_table = NULL;};

    name   = NULL;
    n      = 1;
    nl     = 1;
    nlabel = 0;
    SS_args(argl,
            SC_STRING_I, &name,
            SC_INTEGER_I, &n,
            SC_INTEGER_I, &nlabel,
            SC_LONG_I, &nl,
            0);

    _SX.table_name = SC_strsavef(name, "char*:SX_READ_TEXT_TABLE:name");

    name = SC_search_file(NULL, _SX.table_name);
    if (name == NULL)
       {if (SS_interactive == ON)
	   PRINT(stdout, "\n No file name given\n");

	return(SS_f);};

    fp = io_open(name, "r");
    if (fp == NULL)
       {if (SS_interactive == ON)
	   PRINT(stdout, "\n Can't open file %s\n", name);

	return(SS_f);};

    ok = SX_find_text_table(fp, n, &fn, &nr, &nc, nl, &addrt, nlabel, &addrl);
    if (ok == FALSE)
       {if (SS_interactive == ON)
	   PRINT(stdout, "\n No table #%d in file %s\n", n, name);

	return(SS_f);};

    SFREE(name);

    _SX.current_table = PM_create(nr, nc);

    if (addrl != -1)
       {if (io_seek(fp, addrl, SEEK_SET))
	   return(NULL);

	io_gets(label, MAXLINE, fp);}
    else
        label[0] = '\0';

    if (io_seek(fp, addrt, SEEK_SET))
       return(NULL);

    for (i = 1; i <= nr; i++)
        {io_gets(_SX.line, MAX_BFSZ, fp);

         for (j = 1; j <= nc; j++)
             {token = SC_firsttok(_SX.line, " \t\n\r");
              if ((j == 1) && !fn)
                 token = SC_firsttok(_SX.line, " \t\n\r");

              PM_element(_SX.current_table, i, j) = ATOF(token);};};

    io_close(fp);

    if (SS_interactive == ON)
       {if (label[0] == '\0')
           PRINT(stdout,
                 "\n Table %d : %d rows and %d columns\n\n",
                 n, nr, nc);
        else
           PRINT(stdout,
                 "\n Table %d : %d rows and %d columns\n Label: %s\n\n",
                 n, nr, nc, label);};

    _SX.table_n  = n;
    _SX.table_ln = nl;

    o = SS_make_list(SC_INTEGER_I, &nr,
		     SC_INTEGER_I, &nc,
		     0);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_EXTRACT_VECTOR - extract a vector from the given array */

static double *SX_extract_vector(PM_matrix *a, int o, int s, int n)
   {int i;
    double *val, *src;

    src = a->array + o;
    val = FMAKE_N(double, n, "SX_EXTRACT_VECTOR:val");
    for (i = 0; i < n; i++)
        val[i] = src[i*s];

    return(val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_TABLE_CURVE - create and return a curve from the current table using
 *                - the given specifications
 *                -
 *                - table-curve <n> [<y-offs> [<y-stride> [<x-offs> [<x-stride>]]]]
 *                -
 */

object *SX_table_curve(object *argl)
   {int k, na, yo, ys, xo, xs;
    double *xa, *ya;
    char label[MAXLINE];
    object *ret;

    yo = 0;
    ys = 1;
    xs = 1;
    xo = -1;
    SS_args(argl,
            SC_INTEGER_I, &na,
            SC_INTEGER_I, &yo,
            SC_INTEGER_I, &ys,
            SC_INTEGER_I, &xo,
            SC_INTEGER_I, &xs,
            0);

    ya = SX_extract_vector(_SX.current_table, yo, ys, na);

    if (xo != -1)
       xa = SX_extract_vector(_SX.current_table, xo, xs, na);
    else
       {xo = 0;
        xa = FMAKE_N(double, na, "SX_TABLE_CURVE:xa");
        for (k = 0; k < na; k++)
            xa[k] = (double) (k+1);};

    snprintf(label, MAXLINE, "Table %d:%ld (%d:%d) vs (%d:%d)",
            _SX.table_n, _SX.table_ln, yo, ys, xo, xs);

    ret = SX_mk_curve(na, xa, ya, label, _SX.table_name, NULL);

    SFREE(ya);
    SFREE(xa);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_TABLE_ATTR - return a list of descriptive table parameters
 *               - 
 *               - (n-rows n-cols table-#)
 */

object *SX_table_attr(void)
   {int nrows, ncols, table_n;
    object *ret;

    if (_SX.current_table == NULL)
       ret = SS_null;

    else
       {nrows   = _SX.current_table->nrow;
        ncols   = _SX.current_table->ncol;
        table_n = _SX.table_n;

        if (SS_interactive == ON)
           PRINT(stdout, "\n Table %d : %d rows and %d columns\n\n",
                 table_n, nrows, ncols);

        ret = SS_make_list(SC_INTEGER_I, &nrows,
                           SC_INTEGER_I, &ncols,
                           SC_INTEGER_I, &table_n,
                           0);};

    return(ret);}

/*--------------------------------------------------------------------------*/

/*                         CURVE OUTPUT ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* SX_WRT_PDB - write curves to a ULTRA PDB file */

static void SX_wrt_pdb(PDBfile *fp, object *argl)
   {int i, j, uncached;
    object *obj;

    uncached = FALSE;

    for ( ; SS_consp(argl); argl = SS_cdr(argl))
        {obj = SS_car(argl);
         j = -1;

         if (SS_numbp(obj))
            {i = SS_INTEGER_VALUE(obj);
             if ((i >= 1) && (i <= SX_n_curves_read))
                j = SX_number[i];
             if ((j != -1) && (SX_dataset[j].x[0] == NULL))
                {uncached = TRUE;
                 SX_uncache_curve(&SX_dataset[j]);};}
         else
            j = SX_get_crv_index_i(obj);

         if (j >= 0)
            SX_wrt_pdb_curve(fp, &SX_dataset[j], _SX.icw++);

         if (uncached)
            {uncached = FALSE;
             SFREE(SX_dataset[j].x[0]);
             SFREE(SX_dataset[j].x[1]);
             SX_zero_curve(j);};};

    PD_flush(fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_WRT_BIN - write curves to an ULTRA binary file
 *            - format for binary files:
 *            -        int i - length of id string
 *            -        id string
 *            -        int n - number of datapoints
 *            -        double n x's
 *            -        double n y's
 */

static void SX_wrt_bin(FILE *fp, object *argl)
   {int i, j, n, uncached, err;
    object *obj;

    uncached = FALSE;

    for ( ; SS_consp(argl); argl = SS_cdr(argl))
        {obj = SS_car(argl);
         j = -1;

         if (SS_numbp(obj))
            {i = (int) *SS_GET(BIGINT, obj);
             if ((i >= 1) && (i <= SX_n_curves_read))
                j = SX_number[i];
             if ((j != -1) && (SX_dataset[j].x[0] == NULL))
                {uncached = TRUE;
                 SX_uncache_curve(&SX_dataset[j]);};}
         else
            j = SX_get_crv_index_i(obj);

         if (j >= 0)
            {n = SX_dataset[j].n;
             i = strlen(SX_dataset[j].text);

             write_int(i, fp);  
             err = io_write(SX_dataset[j].text, sizeof(char), i, fp);
             write_int(n, fp);  
             err = io_write((char *) SX_dataset[j].x[0], sizeof(double), n, fp);
             err = io_write((char *) SX_dataset[j].x[1], sizeof(double), n, fp);};

         if (uncached)
            {uncached = FALSE;
             SFREE(SX_dataset[j].x[0]);
             SFREE(SX_dataset[j].x[1]);
             SX_zero_curve(j);};};

    err = io_flush(fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_WRT_TEXT - write curves to an ASCII file */

static void SX_wrt_text(FILE *fp, object *argl)
   {int i, j, n, uncached;
    double *x, *y;
    object *obj;

    uncached = FALSE;

    for ( ; SS_consp(argl); argl = SS_cdr(argl))
        {obj = SS_car(argl);
         j = -1;

         if (SS_numbp(obj))
            {i = (int) *SS_GET(BIGINT, obj);
             if ((i >= 1) && (i <= SX_n_curves_read))
                j = SX_number[i];
             if ((j != -1) && (SX_dataset[j].x[0] == NULL))
                {uncached = TRUE;
                 SX_uncache_curve(&SX_dataset[j]);};}
         else
            j = SX_get_crv_index_i(obj);

         if (j >= 0)
            {n = SX_dataset[j].n;
             x = SX_dataset[j].x[0];
             y = SX_dataset[j].x[1];

             if (SX_dataset[j].text[0] != '#')
                io_printf(fp, "# ");

             io_printf(fp, "%s\n", SX_dataset[j].text);
             for (i = 0; i < n; i++)
                 {io_printf(fp, SX_text_output_format, x[i]);
                  io_printf(fp, " ");
                  io_printf(fp, SX_text_output_format, y[i]);
                  io_printf(fp, "\n");};};

         if (uncached)
            {uncached = FALSE;
             SFREE(SX_dataset[j].x[0]);
             SFREE(SX_dataset[j].x[1]);
             SX_zero_curve(j);};};

    io_flush(fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_WRITE_DATA - write out curves in specified format */

object *SX_write_data(object *argl)
   {char *mode, *fname, *type;
    SC_file_type imode;
    object *fobj, *frst;

    if (_SX.files == NULL)
       _SX.files = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY);

    SX_prep_arg(argl);
    mode  = NULL;
    fname = NULL;
    SS_args(argl,
	    SC_STRING_I, &mode,
	    SC_STRING_I, &fname,
	    0);

/* sort out the file type */
    if (strcmp(mode, "ascii") == 0)
       {imode = SC_ASCII;
        fobj  = SS_cadr(argl);
        argl  = SS_cddr(argl);}
    else if (strcmp(mode, "bin") == 0)
       {imode = SC_BINARY;
        fobj  = SS_cadr(argl);
        argl  = SS_cddr(argl);}
    else if (strcmp(mode, "pdb") == 0)
       {imode = SC_PDB;
        fobj  = SS_cadr(argl);
        argl  = SS_cddr(argl);}
    else
       {imode = SC_PDB;
	fname = mode;
	mode  = SC_strsavef("pdb", "char*:SX_WRITE_DATA:mode");
        fobj  = SS_car(argl);
        argl  = SS_cdr(argl);};

    if (strcmp(fname, "- no print name -") == 0)
       SS_error("BAD FILE NAME - SX_WRITE_DATA", fobj);

/* check to see whether the file is a first time encounter */
    type = (char *) SC_hasharr_def_lookup(_SX.files, fname);
    if (type == NULL)
       {if (SC_isfile(fname))
           {switch (SX_file_exist_action)
               {case DESTROY :
		     SC_remove(fname);
		     break;

                case APPEND :
		     break;

                case FAIL:
		default :
		     SS_error("FILE ALREADY EXISTS - SX_WRITE_DATA", fobj);
		     break;};};

	SC_hasharr_install(_SX.files, fname, mode, SC_STRING_S, TRUE, TRUE);}

    else
       {if (strcmp(type, mode) != 0)
           SS_error("FILE PREVIOUSLY OPENED WITH ANOTHER TYPE - SX_WRITE_DATA",
		    fobj);};

/* flatten out the curve list */
    frst = SS_car(argl);
    if (strcmp(SS_get_string(frst), "thru") == 0)
       argl = SS_exp_eval(argl);

    switch (imode)
       {case SC_ASCII :
	     {FILE *fp;

/* check for request to write an ASCII file */
	      fp = io_open(fname, "a");
	      if (fp == NULL)
                 SS_error("CAN'T CREATE ASCII FILE - SX_WRITE_DATA", fobj);

	      SX_wrt_text(fp, argl);
	      io_close(fp);};

	     break;

        case SC_BINARY :
	     {FILE *fp;

/* check for request to write an ULTRA binary file */
	      fp = io_open(fname, BINARY_MODE_APLUS);
	      if (fp == NULL)
                 SS_error("CAN'T CREATE BINARY FILE - SX_WRITE_DATA", fobj);

/* if this is the first time with this file, put out the header */
	      if (type == NULL)
		 io_write(Ultra_Hdr, (size_t) sizeof(char),
			  (size_t) (strlen(Ultra_Hdr) + 1), fp);

	      SX_wrt_bin(fp, argl);
	      io_close(fp);};

	     break;

        case SC_PDB :
        default  :
	     {PDBfile *fp;

/* check for request to write a pdb file */
	      fp = PD_open(fname, "a");
	      if (fp == NULL)
		 SS_error("CAN'T OPEN PDB FILE - SX_WRITE_DATA", fobj);

	      SX_wrt_pdb(fp, argl);
	      PD_close(fp);};

	     break;};

    return(SS_f);}

/*--------------------------------------------------------------------------*/

/*                     CURVE MEMORY MANAGEMENT ROUTINES                     */

/*--------------------------------------------------------------------------*/

/* SX_CACHE_ADDPID - add the PID to the cache file name */

void SX_cache_addpid() 
   {

    snprintf(_SX.cache_filename, 32, "curves-%d.a", (int) getpid());
   
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_CACHE_CURVE - figure out what/how to do with the curve data
 *                - in a uniform way for any curve source (ASCII, BINARY,
 *                - PDB) this will have to communicate with SX_SELECT when
 *                - the curves are called up
 */

static void SX_cache_curve(curve *crv, SC_file_type type)
   {char bf[MAXLINE];
    pdb_info *ppi;
    FILE *fp;

    switch (type)
       {case SC_ASCII :
	     if (_SX.cache_file == NULL)
	        {_SX.icc        = 0;
		 _SX.cache_file = PD_create(_SX.cache_filename);
		 if (_SX.cache_file == NULL)
		    SS_error("CAN'T CREATE curves.a - SX_CACHE_CURVE",
			     SS_null);
		 else
		    {PD_close(_SX.cache_file);
#ifdef LINUX
/* GOTCHA: Without this delay the open will fail,
 *         iff file accessed across the network
 */
		     sleep(1);
#endif
		     _SX.cache_file = PD_open(_SX.cache_filename, "a");
		     if (_SX.cache_file == NULL)
		        SS_error("CAN'T OPEN curves.a - SX_CACHE_CURVE",
				 SS_null);
		     else
		        {fp = _SX.cache_file->stream;
			 SX_push_open_file(fp);};};};

	     snprintf(bf, MAXLINE, "curve%04d", _SX.icc);

	     ppi = FMAKE(pdb_info, "SX_CACHE_CURVE:ppi");
	     ppi->file       = _SX.cache_file;
	     ppi->curve_name = SC_strsavef(bf, "char*:SX_CACHE_CURVE:bf");

	     crv->file_info = (void *) ppi;
	     crv->file_type = SC_PDB;

	     SX_wrt_pdb_curve(_SX.cache_file, crv, _SX.icc++);

        case SC_BINARY :
        case SC_PDB    :
	     SFREE(crv->x[0]);
	     SFREE(crv->x[1]);
	     crv->x[0] = NULL;
	     crv->x[1] = NULL;

        default :
	     break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_UNCACHE_CURVE - get the curve data out of wherever it has been stashed
 *                  - and connect it up to the curve
 */

void SX_uncache_curve(curve *crv)
   {int n;
    bin_info *pbi;
    pdb_info *ppi;
    FILE *stream;
    PDBfile *file;

    switch (crv->file_type)
       {case SC_BINARY :
	     pbi = (bin_info *) (crv->file_info);
	     stream = pbi->stream;
	     io_seek(stream, pbi->fileaddr, SEEK_SET);
	     n = crv->n;
	     io_read((char *) crv->x[0], sizeof(double), n, stream);
	     io_read((char *) crv->x[1], sizeof(double), n, stream);
	     break;

        case SC_ASCII :
	     break;

        default  :
        case SC_PDB :
	     ppi = (pdb_info *) (crv->file_info);
	     file = ppi->file;
	     if (SX_read_pdb_curve(file, (void *) NULL,
				   ppi->curve_name, crv, X_AND_Y) == FALSE)
	        SS_error("PDB READ FAILED - SX_UNCACHE-CURVE",
			 SS_null);
	     break;};

    return;}

/*--------------------------------------------------------------------------*/

/*                           FILE_LIST ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* SX_FILE_OPEN - check the given file pointer against any others
 *              - in the list of open files and return TRUE if
 *              - it is there and FALSE otherwise
 */

static int SX_file_open(FILE *fp)
   {pcons *lst;

    for (lst = _SX.file_list; lst != NULL; lst = (pcons *) (lst->cdr))
        if (fp == (FILE *) (lst->car))
           return(TRUE);

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_REMOVE_FILE - check the given file pointer against any others
 *                - in the list of open files and remove it if it is there
 */

static int SX_remove_file(FILE *fp)
   {pcons *lst, *nxt;

/* if the first file on the list is the one, take care of it */
    if (fp == (FILE *) (_SX.file_list->car))
       {nxt = (pcons *) (_SX.file_list->cdr);
	SC_rl_pcons(_SX.file_list, 0);
        _SX.file_list = nxt;
        return(TRUE);};

/* otherwise cdr down the list in search of the file */
    for (lst = _SX.file_list, nxt = (pcons *) (lst->cdr);
         nxt != NULL;
         lst = nxt, nxt = (pcons *) (nxt->cdr))
        if (fp == (FILE *) (nxt->car))
           {lst->cdr = nxt->cdr;
            SC_rl_pcons(nxt, 0);
            return(TRUE);};

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_PUSH_OPEN_FILE - check the given file pointer against any others
 *                   - in the list of open file and cons it on if it is
 *                   - not already there
 */

void SX_push_open_file(FILE *fp)
   {

    if (!SX_file_open(fp))
       _SX.file_list = SC_mk_pcons(NULL, fp, SC_PCONS_P_S, _SX.file_list);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_CLOSE_OPEN_FILES - cdr down the list of open files and close each one
 *                     - NOTE: this should be rewritten to go off _SX.file_list
 *                     -       and save the time of going over the curves!!!
 */

void SX_close_open_files(void)
   {pdb_info *ppi;
    bin_info *pbi;
    FILE *fp;
    int i;

    for (i = 0; i < SX_N_Curves; i++)
        {switch (SX_dataset[i].file_type)
            {case SC_BINARY :
	          pbi = (bin_info *) SX_dataset[i].file_info;
		  fp = pbi->stream;
		  if (SX_file_open(fp))
		     {SX_remove_file(fp);
		      io_close(fp);};
		  SFREE(pbi);
		  break;

             case SC_PDB :
	          {PDBfile *file;
                   FILE *fp;

		   ppi = (pdb_info *) SX_dataset[i].file_info;
		   file = ppi->file;
		   if (file != NULL)
		     {fp = file->stream;
		      if (SX_file_open(fp))
			 {SX_remove_file(fp);
			  PD_close(file);
			  if (file == _SX.cache_file)
                             _SX.cache_file = NULL;};

		      SFREE(ppi->curve_name);
		      SFREE(ppi);};};

		  break;

             default    :
             case SC_ASCII : 
	          break;};

       SX_dataset[i].file_info = NULL;
       SX_dataset[i].file_type = SC_NO_FILE;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
