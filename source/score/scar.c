/*
 * SCAR.C - routines for accessign archives
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scope_make.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MAKE_ARCHIVE - allocate and initialize an fcdes instance */

static fcdes *_SC_make_archive(char *name, FILE *fp, int nb, hasharr *tab)
   {fcdes *fc;

    fc = FMAKE(fcdes, "_SC_MAKE_ARCHIVE:fc");
    fc->name     = SC_strsavef(name, "_SC_MAKE_ARCHIVE:name");
    fc->file     = fp;
    fc->hdr_size = nb;
    fc->entries  = tab;

    return(fc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MAKE_AR_ENTRY - allocate and initialize a file container entry */

static fcent *_SC_make_ar_entry(FILE *fp, int round,
				char *name, char *date,
				char *uid, char *gid,
				char *size, char *mode,
				off_t *ppos)
   {off_t sz, pos, ada;
    fcent *ae;

    sz = SC_stol(size);
    if (round == TRUE)
       {pos = sz + 1;
	pos = (pos >> 1) << 1;}
    else
       pos = sz;

    ada  = ftell(fp);
    pos += ada;
		
    ae = FMAKE(fcent, "_SC_make_ar_entry:ae");

    ae->name    = SC_strsavef(name, "_SC_MAKE_AR_ENTRY:name");
    ae->date    = SC_stoi(date);
    ae->uid     = SC_stoi(uid);
    ae->gid     = SC_stoi(gid);
    ae->size    = sz;
    ae->address = ada;

    SC_strncpy(ae->perm, 8, mode, 8);

    *ppos = pos;

    return(ae);}

/*--------------------------------------------------------------------------*/

#ifdef AIX

/*--------------------------------------------------------------------------*/

/* SC_SCAN_ARCHIVE - return a hasharr containing the table of contents
 *                 - of the archive ARF
 */

fcdes *SC_scan_archive(char *arf)
   {int nr, nb, nc;
    size_t ad;
    off_t pos;
    char s[MAXLINE];
    char *p;
    FILE *fp;
    fcent *ae;
    fcdes *fc;
    hasharr *tab;

    fc  = NULL;
    tab = NULL;

    fp = fopen(arf, "r");
    if (fp != NULL)
       {nr = fread(s, 1, 8, fp);
	fseek(fp, 0, SEEK_SET);
	if (nr == 8)
	   {tab = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY);

/* for a 64 bit archive */
	    if (strncmp(s, AIAMAGBIG, SAIAMAG) == 0)
	       {FL_HDR_BIG fhd;
		AR_HDR_BIG hdr;

		nb = AR_HSZ_BIG;
		nr = fread(&fhd, FL_HSZ_BIG, 1, fp);
		while (TRUE)
		   {nr = fread(&hdr, nb, 1, fp);
		    if (nr != 1)
		       break;

		    strcpy(s, hdr._ar_name.ar_name);
		    nc = SC_stoi(hdr.ar_namlen);
		    fread(s+2, nc, 1, fp);
                    if (strncmp(s, "`\n", 2) == 0)
		       {strcpy(s, ".SYMTAB");
			p = s;}
		    else
		       p = strtok(s, "`\n");

		    ae = _SC_make_ar_entry(fp, TRUE, p, hdr.ar_date,
					   hdr.ar_uid, hdr.ar_gid,
					   hdr.ar_size, hdr.ar_mode,
					   &pos);

		    SC_hasharr_install(tab, ae->name, ae, "fcent", TRUE, TRUE);

		    ad = SC_stol(hdr.ar_nxtmem);
		    if (ad == 0)
		       break;
		    fseek(fp, ad, SEEK_SET);};}

/* for a 32 bit archive */
	    else
	       {FL_HDR fhd;
		AR_HDR hdr;

		nb = AR_HSZ;
		nr = fread(&fhd, FL_HSZ, 1, fp);

		while (TRUE)
		   {nr = fread(&hdr, nb, 1, fp);
		    if (nr != 1)
		       break;

		    strcpy(s, hdr._ar_name.ar_name);
		    nc = SC_stoi(hdr.ar_namlen);
		    fread(s+2, nc, 1, fp);
                    if (strncmp(s, "`\n", 2) == 0)
		       {strcpy(s, ".SYMTAB");
			p = s;}
		    else
		       p = strtok(s, "`\n");

		    ae = _SC_make_ar_entry(fp, TRUE, p, hdr.ar_date,
					   hdr.ar_uid, hdr.ar_gid,
					   hdr.ar_size, hdr.ar_mode,
					   &pos);

		    SC_hasharr_install(tab, ae->name, ae, "fcent", TRUE, TRUE);

		    ad = SC_stol(hdr.ar_nxtmem);
		    if (ad == 0)
		       break;
		    fseek(fp, ad, SEEK_SET);};};};

	fclose(fp);

	fc = _SC_make_archive(arf, NULL, nb, tab);};

    return(fc);}

/*--------------------------------------------------------------------------*/

# define HAVE_SCAN_ARCHIVE

#endif

#if defined(OSF) && !defined(HAVE_SCAN_ARCHIVE)

/*--------------------------------------------------------------------------*/

/* SC_SCAN_ARCHIVE - return a hasharr containing the table of contents
 *                 - of the archive ARF
 */

fcdes *SC_scan_archive(char *arf)
   {int nr, nb;
    BIGINT pos, ext, org, off;
    char s[MAXLINE], lname[MAXLINE];
    FILE *fp;
    char *p, *name;
    fcent *ae;
    fcdes *fc;
    hasharr *tab;
    struct ar_hdr hdr;

    fc  = NULL;
    tab = NULL;

    fp = fopen(arf, "r");
    if (fp != NULL)
       {nr = fread(s, 1, 8, fp);

	if (nr == 8)
	   {nb = sizeof(struct ar_hdr);
	    p  = (char *) &hdr;

	    tab = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY);
	    ext = 0;

	    while (TRUE)
	       {nr = fread(p, nb, 1, fp);
		if (nr != 1)
		   break;

		if (strncmp(hdr.ar_name, "ARFILENAMES/", 12) == 0)
		   {strcpy(lname, ".EXTENDED");
		    name = lname;
		    ext  = ftell(fp);}
		else if (hdr.ar_name[0] == ' ')
		   {off = SC_stoi(hdr.ar_name+1);
		    pos = ext + off;
		    org = ftell(fp);
		    fseek(fp, pos, SEEK_SET);
		    fgets(lname, MAXLINE, fp);
		    fseek(fp, org, SEEK_SET);
		    name = lname;}
		else
		   {strncpy(lname, hdr.ar_name, 16);
		    lname[16] = '\0';
		    SC_trim_right(lname, " ");
		    name = lname;};

		ae = _SC_make_ar_entry(fp, TRUE, name, hdr.ar_date,
				       hdr.ar_uid, hdr.ar_gid,
				       hdr.ar_size, hdr.ar_mode,
				       &pos);

		SC_hasharr_install(tab, ae->name, ae, "fcent", TRUE, TRUE);

		fseek(fp, pos, SEEK_SET);};};

	fclose(fp);

	fc = _SC_make_archive(arf, NULL, nb, tab);};

    return(fc);}

/*--------------------------------------------------------------------------*/

# define HAVE_SCAN_ARCHIVE

#endif

#if defined(USE_BSD) && !defined(HAVE_SCAN_ARCHIVE)

/*--------------------------------------------------------------------------*/

/* SC_SCAN_ARCHIVE - return a hasharr containing the table of contents
 *                 - of the archive ARF
 */

fcdes *SC_scan_archive(char *arf)
   {int nr, nb, nc, ne, na;
    off_t pos;
    char s[MAXLINE], lname[MAXLINE];
    FILE *fp;
    char *p, *name;
    fcent *ae;
    fcdes *fc;
    hasharr *tab;
    struct ar_hdr hdr;

    fc  = NULL;
    tab = NULL;
    ne  = strlen(AR_EFMT1);
    na  = sizeof(hdr.ar_name);

    fp = fopen(arf, "r");
    if (fp != NULL)
       {nr = fread(s, 1, 8, fp);

	if (nr == 8)
	   {nb = sizeof(struct ar_hdr);
	    p  = (char *) &hdr;

	    tab = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY);

	    while (TRUE)
	       {nr = fread(p, nb, 1, fp);
		if (nr != 1)
		   break;

		if (strncmp(hdr.ar_name, AR_EFMT1, ne) == 0)
		   {nc = SC_stoi(hdr.ar_name + ne);
		    fread(lname, nc, 1, fp);
		    name = lname;}
		else
		   {nc = 0;
		    strncpy(lname, hdr.ar_name, na);
		    lname[na] = '\0';
		    SC_trim_right(lname, " ");
		    name = lname;};

		ae = _SC_make_ar_entry(fp, FALSE, name, hdr.ar_date,
				       hdr.ar_uid, hdr.ar_gid,
				       hdr.ar_size, hdr.ar_mode,
				       &pos);

		pos        -= nc;
		ae->size   -= nc;
		ae->address = pos;

		SC_hasharr_install(tab, ae->name, ae, "fcent", TRUE, TRUE);

		fseek(fp, pos, SEEK_SET);};};

	fclose(fp);

	fc = _SC_make_archive(arf, NULL, nb, tab);};

    return(fc);}

/*--------------------------------------------------------------------------*/

# define HAVE_SCAN_ARCHIVE

#endif

#ifndef HAVE_SCAN_ARCHIVE

/*--------------------------------------------------------------------------*/

/* SC_SCAN_ARCHIVE - return a hasharr containing the table of contents
 *                 - of the archive ARF
 */

fcdes *SC_scan_archive(char *arf)
   {int nr, nb;
    BIGINT ext, org, off;
    off_t pos;
    char s[MAXLINE], lname[MAXLINE];
    FILE *fp;
    char *p, *name;
    fcent *ae;
    fcdes *fc;
    hasharr *tab;
    struct ar_hdr hdr;

    fc  = NULL;
    tab = NULL;

    fp = fopen(arf, "r");
    if (fp != NULL)
       {nr = fread(s, 1, 8, fp);

	if (nr == 8)
	   {nb = sizeof(struct ar_hdr);
	    p  = (char *) &hdr;

	    tab = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY);
	    ext = 0;

	    while (TRUE)
	       {nr = fread(p, nb, 1, fp);
		if (nr != 1)
		   break;

		if (strncmp(hdr.ar_name, "//", 2) == 0)
		   {strcpy(lname, ".EXTENDED");
		    name = lname;
		    ext  = ftell(fp);}
		else if (strncmp(hdr.ar_name, "/ ", 2) == 0)
		   {strcpy(lname, ".SYMTAB");
		    name = lname;}
		else if (hdr.ar_name[0] == '/')
		   {off = SC_stoi(hdr.ar_name+1);
		    pos = ext + off;
		    org = ftell(fp);
		    fseek(fp, pos, SEEK_SET);
		    fread(lname, MAXLINE, 1, fp);
		    fseek(fp, org, SEEK_SET);
		    name = strtok(lname, "/");}
		else
		   name = strtok(hdr.ar_name, "`\n/");

		ae = _SC_make_ar_entry(fp, TRUE, name, hdr.ar_date,
				       hdr.ar_uid, hdr.ar_gid,
				       hdr.ar_size, hdr.ar_mode,
				       &pos);

		SC_hasharr_install(tab, ae->name, ae, "fcent", TRUE, TRUE);

		fseek(fp, pos, SEEK_SET);};};

	fc = _SC_make_archive(arf, fp, nb, tab);};

    return(fc);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* _SC_IS_ARCHIVE - return TRUE if input file is an archive 
 *                - this is currently only a stub
 */

int _SC_is_archive(FILE *fp)
   {int rv, nr;
    off_t oa;
    char s[MAXLINE];

    rv = FALSE;

    if (fp != NULL)
       {oa = ftell(fp);
	fseek(fp, 0, SEEK_SET);

#ifdef AIX
	nr = fread(s, 1, SAIAMAG, fp);
	fseek(fp, 0, SEEK_SET);
	rv = ((nr == SAIAMAG) &&
	      ((strncmp(s, AIAMAG, SAIAMAG) == 0) ||
	       (strncmp(s, AIAMAGBIG, SAIAMAG) == 0)));
#else
	nr = fread(s, 1, SARMAG, fp);
	fseek(fp, 0, SEEK_SET);
	rv = ((nr == SARMAG) && (strncmp(s, ARMAG, SARMAG) == 0));
#endif

	fseek(fp, oa, SEEK_SET);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
