/*
 * SCTAR.C - routines for accessing tar files
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h" 

/*
 * TAR format notes:
 *
 * A tar archive consists of one or more 512 byte blocks.  Each member file
 * consists of a (512 byte) header block followed by 0 or more blocks containing
 * the file contents.  The end of the archive is indicated by two blocks filled
 * with binary zeros.  Unused space in the yeader is left as binary zeros.
 * Older Unix compatible tar format
 *
 * Header layout
 *
 * Field Width	Field Name	Meaning
 * 100		name		name of file
 * 8		mode		file mode
 * 8		uid		owner user ID
 * 8		gid		owner group ID
 * 12		size		length of file in bytes
 * 12		mtime		modify time of file
 * 8		chksum		checksum for header
 * 1		link		indicator for links
 * 100		linkname	name of linked file
 *
 * The link field is 1 for a linked file, 2 for a symbolic link and 0 otherwise.
 * A directory is indicated by a trailing slash in its name.
 *
 * Newer USTAR format
 *
 * Header layout
 *
 * Field Width	Field Name	Meaning
 * 100		name
 * 8		mode
 * 8		uid
 * 8		gid
 * 12		size
 * 12		mtime
 * 8		chksum
 * 1		typeflag
 * 100		linkname
 * 6		magic		USTAR indicator
 * 2		version		USTAR version
 * 32		uname		owner user name
 * 32		gname		owner group name
 * 8		devmajor	device major number
 * 8		devminor	device minor number
 * 155		prefix		prefix for file name
 *
 * The magic, uname and gname fields are null-terminated character strings.
 *
 * The fields name, linkname, and prefix are null-terminated unless the full
 * field is used to store a name.  All other fields are zero-filled octal
 * numbers, represented in ASCII.  Trailing nulls are present for these
 * numbers, except for the size, mtime and version fields.
 *
 * The name field contains the name of the archived file.  On USTAR format
 * archives the value of the prefix field, if non-null is prefixed to the
 * name field to allow names longer than 100 characters.
 * The size field is zero if the header describes a link.
 *
 * For USTAR the typeflag is a compatible extension of the link field of
 * the older TAR format.  Recognized values are
 *    0 or null	Regular file
 *    1		Link to another file already archived
 *    2		Symbolic link
 *    3		Character special device
 *    4		Block special device
 *    5		Directory
 *    6		FIFO special file
 *    7		Reserved
 *  A-Z		Available for custom usage
 *
 * In USTAR format the uname and gname fields contain the name of the owner
 * and group of the file respectively.
 *
 */

#define TAR_BLOCKSIZE 512

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MAKE_TARFILE - allocate and initialize an tarfile instance */

static fcdes *_SC_make_tarfile(char *name, FILE *fp, hasharr *tab)
   {fcdes *fc;

    fc = FMAKE(fcdes, "_SC_MAKE_TARFILE:fc");
    fc->name     = SC_strsavef(name, "_SC_MAKE_TARFILE:name");
    fc->file     = fp;
    fc->entries  = tab;

    return(fc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MAKE_TAR_ENTRY - allocate and initialize a file container entry */

static fcent *_SC_make_tar_entry(FILE *fp, int round,
				 char *name, char *date,
				 char *uid, char *gid,
				 char *size, char *mode)
   {int64_t sz, pos;
    char *p;
    fcent *ae;

    sz = SC_strtol(size, &p, 8);
    if (round == TRUE)
       {pos = sz + 1;
	pos = (pos >> 1) << 1;}
    else
       pos = sz;

    pos = ftell(fp);
		
    ae = FMAKE(fcent, "_SC_MAKE_TAR_ENTRY:ae");

    ae->name    = SC_strsavef(name, "_SC_MAKE_TAR_ENTRY:name");
    ae->date    = SC_stoi(date);
    ae->uid     = SC_strtol(uid, &p, 8);
    ae->gid     = SC_strtol(gid, &p, 8);
    ae->size    = sz;
    ae->address = pos;

    SC_strncpy(ae->perm, 8, mode, 8);

    return(ae);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PROCESS_TARHDR - return a FCENT describing an item in a tarfile
 *                    - this should create and fill a fcent
 *                    - it should then advance the file pointer to point
 *                    - to the block that would be the header for the
 *                    - next entry in the tarfile
 */

fcent *_SC_process_tarhdr(FILE *fp, tarhdr *p)
   {int rem;
    size_t size;
    char name[256], csize[12];
    char *pcsize;
    fcent *te;

    te = NULL;

/* check the file size */
    SC_strncpy(csize, 12, p->size, 11);       /* last char is either blank or nul */
    csize[11] = '\0';                         /* make sure it's nul */

 /* get rid of leading spaces */
    pcsize = csize;
    while (*pcsize++ == ' ');
    pcsize--;
    size = _SC_otol(pcsize);    

    if (size <= 0)
       return(NULL);

    else

/* set up the name */
       {if (p->name[99] == '\0')
           strcpy(name, p->name);
        else
           {SC_strncpy(name, 256, p->name, 100);
            if (strlen(p->prefix) > 0)            /* filenames > 100 chars */
	       SC_strcat(name, 256, p->prefix);};
 
	te = _SC_make_tar_entry(fp, FALSE,
				name, p->mtime, p->uid, p->gid,
				p->size, p->mode);};

/* advance file pointer to the next tarhdr (if present) */
    rem = size % TAR_BLOCKSIZE;
    fseek(fp, size + TAR_BLOCKSIZE - rem, SEEK_CUR);

    return(te);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SCAN_TARFILE - return a hasharr containing the table of contents
 *                 - of the tarfile TARF
 */

fcdes *SC_scan_tarfile(char *tarf)
   {int nr, nb;
    char *pp;
    FILE *fp;
    tarhdr p;
    fcent *tae;
    fcdes *fc;
    hasharr *tab;

    fc = NULL;
    tab = NULL;

    fp = fopen(tarf, "r");
    if (fp != NULL)
       {nb  = sizeof(tarhdr);
        tab = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY);
        pp  = (char *) &p;
    
        while (TRUE)
	   {nr = fread(pp, nb, 1, fp);
            if (nr != 1)
	       break;

            tae = _SC_process_tarhdr(fp, &p); 
            if (tae != NULL)
	       SC_hasharr_install(tab, tae->name, tae, "fcent", TRUE, TRUE);};
     
        fc = _SC_make_tarfile(tarf, fp, tab);};

    return(fc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TAR_CHECKSIZE - check that the size field is a legal number
 *                   - it should have only spaces, digits and
 *                   - null termination
 */

int _SC_tar_checksize(tarhdr *thdr)
   {int i, c, ret, ndig; 
    char *pch;           

    ret = TRUE;
    pch = (char *) &thdr->size; 
            
    for (i = 0, ndig = 0; i < 12; i++)
        {c = *pch;
	 if (c == 0)
            {pch++;
             continue;}
         else if (isdigit(c))
            {pch++;
             ndig++;}
         else if (c == 0)
            break;
         else             /* illegal digit */
            {ret = FALSE;
             break;};};
               
    if (ndig == 0)        /* no numbers in size field */
       ret = FALSE;

    return(ret);}
          
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TAR_CHECKNAME - check that the tarhdr name field is all alpha */

int _SC_tar_checkname(tarhdr *thdr)
   {int i, ret;
    char *pch;

    ret = TRUE;
    pch = (char *) &thdr->name;

    for (i = 0; i < 100; i++)
        {if ((*pch == '\0') || (SC_is_print_char(*pch, 0)))
            {pch++;
             continue;}
         else
            {ret = FALSE;
             break;};};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_IS_TARFILE - return TRUE if input file is a tar file */

int _SC_is_tarfile(FILE *fp)
   {int ret, nb, nr;
    int64_t addsave;
    tarhdr p;

    addsave = ftell(fp);
    SC_ASSERT(addsave >= 0);

    fseek(fp, 0, SEEK_SET);

    ret = FALSE;
    nb  = sizeof(tarhdr);
    nr  = fread((char *)&p, nb, 1, fp);

    if (nr == 1)
       {if (strncmp(p.magic, "ustar", 6) == 0)
           ret = TRUE;
        else if (!_SC_tar_checkname(&p))
	   ret = FALSE;
        else if (!_SC_tar_checksize(&p))
	   ret = FALSE;
        else
           ret = TRUE;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
