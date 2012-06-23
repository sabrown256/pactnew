/*
 * SCFCNR.C - routines for accessing fcontainer files
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"
#include "scope_make.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FCONTAINER_TYPE - try to determine the type of container file */

static SC_file_type _SC_fcontainer_type(char *name)
   {FILE *fp;
    SC_file_type type;

    type = SC_OTHER;

    fp = fopen(name, "r");
    if (fp != NULL)
       {if (_SC_is_tarfile(fp))
           type = SC_TAR;
        else if (_SC_is_archive(fp))
           type = SC_AR;

        fclose(fp);}

    return(type);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MAKE_FCONTAINER - create and return an fcontainer */

fcontainer *_SC_make_fcontainer(char *name, SC_file_type type, void *handle)
   {fcontainer *cf;

    cf = CMAKE(fcontainer);

    cf->name   = CSTRSAVE(name);
    cf->type   = type;
    cf->handle = handle;
    
    return(cf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REL_ENTRY - release a single fcontainer entry */

static int _SC_rel_entry(haelem *hp, void *a)
   {int ok;
    fcent *ae;

    ok = SC_haelem_data(hp, NULL, NULL, (void **) &ae, TRUE);
    SC_ASSERT(ok == TRUE);

    if (ae != NULL)
       {CFREE(ae->name);
	CFREE(ae);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREE_FCONTAINER - release an fcontainer instance */

void SC_free_fcontainer(fcdes *fc)
   {

    if (fc != NULL)
       {CFREE(fc->name);

	if (fc->file != NULL)
	   fclose(fc->file);

	if (fc->entries != NULL)
	   SC_hasharr_clear(fc->entries, _SC_rel_entry, NULL);

	CFREE(fc);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_UNKNOWN_CONTAINER - return an FCDES if the file NAME if of the form
 *                       -    <name>~<start>:<end>
 *                       - where <start> and <end> specify the address
 *                       - range of the contained file
 */

fcdes *_SC_unknown_container(char *name)
   {int nt, ok;
    int64_t sad, ead;
    char s[MAXLINE];
    char **ta;
    fcdes *fc;
    FILE *fp;

    fc = NULL;

    SC_strncpy(s, MAXLINE, name, -1);
    ta = SC_tokenize(s, "~:");
    if (ta != NULL)
       {SC_ptr_arr_len(nt, ta);

	fp = NULL;
	ok = TRUE;
	if (nt == 3)
	   {fp  = fopen(ta[0], "r");
	    sad = SC_stol(ta[1]);
	    ead = SC_stol(ta[2]);
	    ok &= (ead > sad);};

	if (ok == TRUE)
	   {fcent *ae;
	    hasharr *tab;

	    tab = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY, 0);
    
	    ae = CMAKE(fcent);
	    if (ae != NULL)
	       {ae->name    = CSTRSAVE(name);
		ae->date    = 0;
		ae->uid     = -1;
		ae->gid     = -1;
		ae->size    = ead - sad + 1;
		ae->address = sad;

		SC_strncpy(ae->perm, 8, "0444", 8);

		snprintf(s, MAXLINE, "%ld:%ld", (long) sad, (long) ead);
		SC_hasharr_install(tab, s, ae, "fcent", 3, -1);};

	    fc = CMAKE(fcdes);
	    fc->name    = CSTRSAVE(name);
	    fc->file    = fp;
	    fc->entries = tab;}

	else if (fp != NULL)
	   fclose(fp);

	SC_free_strings(ta);};

    return(fc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_OPEN_FCONTAINER - determine the type of container file NAME
 *                    - and return a handle to it
 *                    - return NULL if container file type cannot
 *                    - be determined
 *                    - if TYPE is not NULL use that as the type of
 *                    - the input file
 */

fcontainer *SC_open_fcontainer(char *name, SC_file_type type,
			       fcdes *(*meth)(char *name, SC_file_type *pt))
   {char cntr[MAXLINE];
    char *p;
    SC_file_type ftype;
    fcontainer *cf;
    fcdes *fc;

    cf = NULL;
    fc = NULL;

    SC_strncpy(cntr, MAXLINE, name, -1);
    p = strchr(cntr, '~');
    if (p != NULL)
       *p = '\0';

    if (meth != NULL)
       fc = meth(cntr, &ftype);

    if (fc == NULL)
       {ftype = (type == SC_UNKNOWN) ? _SC_fcontainer_type(cntr) : type;
	if (ftype == SC_TAR)
	   fc = SC_scan_tarfile(cntr);
	else if (ftype == SC_AR)
	   fc = SC_scan_archive(cntr);};

    if (fc == NULL)
       {ftype = SC_OTHER;
	fc    = _SC_unknown_container(name);};
           
    if (fc != NULL)
       cf = _SC_make_fcontainer(cntr, ftype, fc);

    return(cf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FCONTAINER_LIST - return a list of files in an fcontainer */

char **SC_fcontainer_list(fcontainer *cf, int full)
   {int ne;
    long i, n;
    char ta[MAXLINE], tb[MAXLINE];
    char s[MAXLINE], tm[MAXLINE];
    char **rv, *pt;
    hasharr *tab;
    fcent *te;
    fcdes *fc;

    rv = NULL;

    fc  = cf->handle;
    tab = fc->entries;

    n = SC_hasharr_get_n(tab);
    if (n > 0)
       {rv = CMAKE_N(char *, n + 1);
	ne = 0;
	for (i = 0; SC_hasharr_next(tab, &i, NULL, NULL, (void **) &te); i++)
	    {if (full == TRUE)
	        {SC_strncpy(tm, MAXLINE, ctime(&te->date), -1);
		 pt = tm + 4;
		 SC_LAST_CHAR(pt) = '\0';
		 SC_itos(ta, MAXLINE, te->size, "%8lld");
		 SC_itos(tb, MAXLINE, te->address, "%8lld");
		 snprintf(s, MAXLINE, "%s %d/%d %s %s %s %s",
			  te->perm, te->uid, te->gid, ta,
			  pt, tb, te->name);}
	     else
	        SC_strncpy(s, MAXLINE, te->name, -1);

	     rv[ne++] = CSTRSAVE(s);};

	rv[ne] = NULL;};
            
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LIST_FCONTAINER - print the PDB files in an fcontainer */

void SC_list_fcontainer(FILE *f, fcontainer *cf, int full)
   {int i;
    char **ret;
    
    i   = 0;
    ret = NULL;

    if (cf != NULL)
       {PRINT(f, "   files:\n");

        ret = SC_fcontainer_list(cf, full);
        if (ret != NULL)
           {for (i = 0; ret[i] != NULL; i++)
	        {PRINT(f, "     %s\n", ret[i]);
		 CFREE(ret[i]);};
            CFREE(ret);};}
            
    else 
       PRINT(f, "Invalid file container specified\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CLOSE_FCONTAINER - free resources and close the container */
 
void SC_close_fcontainer(fcontainer *cf)
   {

    if (cf != NULL)
       {SC_free_fcontainer(cf->handle);        
	CFREE(cf->name);
        CFREE(cf);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DESCRIBE_FCONTAINER - return descriptive information about fcontainer */
 
void SC_describe_fcontainer(FILE *f, fcontainer *cf, int full)
   {

    if (cf != NULL)
       {PRINT(f, "File container:\n");
        PRINT(f, "   name: %s\n", cf->name);
        if (cf->type == SC_TAR)
            PRINT(f, "   type: tar\n");
        else if (cf->type == SC_AR)
            PRINT(f, "   type: ar\n");
        
        SC_list_fcontainer(f, cf, full);}

     else 
       PRINT(f, "Invalid file container specified\n");
        
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

