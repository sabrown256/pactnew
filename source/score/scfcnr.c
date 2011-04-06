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

    ok = SC_haelem_data(hp, NULL, NULL, (void **) &ae);
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

/* SC_OPEN_FCONTAINER - determine the type of container file NAME
 *                    - and return a handle to it
 *                    - return NULL if container file type cannot
 *                    - be determined
 *                    - if TYPE is not NULL use that as the type of
 *                    - the input file
 */

fcontainer *SC_open_fcontainer(char *name, SC_file_type type)
   {SC_file_type ftype;
    fcontainer *cf;
    fcdes *fc;

    cf = NULL;
    fc = NULL;

    ftype = (type == SC_UNKNOWN) ? _SC_fcontainer_type(name) : type;

    if (ftype == SC_TAR)
       fc = SC_scan_tarfile(name);
    else if (ftype == SC_AR)
       fc = SC_scan_archive(name);
    else
       fc = NULL;
           
    if (fc != NULL)
       cf = _SC_make_fcontainer(name, ftype, fc);

    return(cf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FCONTAINER_LIST - return a list of files in an fcontainer */

char **SC_fcontainer_list(fcontainer *cf, int full)
   {int ne;
    long i, n;
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
		 snprintf(s, MAXLINE, "%s %d/%d %8lld %s %8lld %s",
			  te->perm, te->uid, te->gid, (long long) te->size,
			  pt, (long long) te->address, te->name);}
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

    SC_free_fcontainer(cf->handle);        
    CFREE(cf->name);
    CFREE(cf);

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

