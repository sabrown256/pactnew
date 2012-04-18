/*
 * SCDL.C - shared object loading
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"
#include "scope_dl.h"

sostate
 _SC_dl;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SO_REGISTER_FUNC - register a function to be access via
 *                     - a shared object
 *                     - return TRUE iff successful
 */

int SC_so_register_func(char *lib, char *path,
			char *rv, char *name, char *argl)
   {int ok;
    sodes *se;

    ok = FALSE;

    if (name != NULL)
       {if (_SC_dl.tab == NULL)
	   _SC_dl.tab = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY, 0);

	se = CMAKE(sodes);
	if (se != NULL)
	   {se->lib  = CSTRSAVE(lib);
	    se->path = CSTRSAVE(path);
	    se->rv   = (rv != NULL) ? CSTRSAVE(rv) : NULL;
	    se->name = CSTRSAVE(name);
	    se->argl = (argl != NULL) ? CSTRSAVE(argl) : NULL;
	    se->so   = NULL;
	    se->f    = NULL;

	    SC_hasharr_install(_SC_dl.tab, name, se, "sodes", 3, TRUE);

	    ok = TRUE;};};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SO_OPEN - open shared object NAME
 *             - return a handle to the shared object
 */

static sodes *_SC_so_open(char *name)
   {char msg[MAXLINE];
    char *lib;
    sodes *se;

    se = (sodes *) SC_hasharr_def_lookup(_SC_dl.tab, name);
    if (se != NULL)
       {lib  = se->lib;

#ifdef HAVE_DYNAMIC_LINKER
	int flag;
	void *so;

	flag = RTLD_LAZY | RTLD_GLOBAL;
	so   = dlopen(lib, flag);
	if (so == NULL)
	   snprintf(msg, MAXLINE, "ERROR: %s", dlerror());
	else
	   se->so = so;
#else
        snprintf(msg, MAXLINE, "ERROR: no dynamic linker");
        se = NULL;
#endif
       };

    return(se);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SO_CLOSE - close shared object associate with function NAME
 *             - return TRUE iff successful
 */

int SC_so_close(char *name)
   {int st;
    char msg[MAXLINE];
    sodes *se;

    st = -1;

    se = _SC_so_open(name);
    if ((se != NULL) && (se->so != NULL))
       {

#ifdef HAVE_DYNAMIC_LINKER
        st = dlclose(se->so);
	if (st != 0)
	   snprintf(msg, MAXLINE, "ERROR: %s", dlerror());
#else
	snprintf(msg, MAXLINE, "ERROR: No dynamic linker");
#endif
        };

/* reverse the sense */
    st = (st == 0);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SO_GET_FUNC - return the function NAME from a shared object */

void *SC_so_get_func(char *name)
   {char msg[MAXLINE];
    sodes *se;
    void *f;

    f = NULL;

    se = _SC_so_open(name);
    if (se != NULL)
       {if (se->f == NULL)
           {

#ifdef HAVE_DYNAMIC_LINKER
	    char *s;

/* clear any existing error */
	    dlerror();

	    se->f = dlsym(se->so, se->name);
/*	    *(void **) (&se->f) = dlsym(se->so, se->name); */

	    s = dlerror();
	    if (s != NULL)
	       snprintf(msg, MAXLINE, "ERROR: %s", s);

#else
	    snprintf(msg, MAXLINE, "ERROR: No dynamic linker");
#endif
	   };

	f = se->f;};

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SO_CONFIG - read a PACT so.conf file FNAME
 *              - an so.conf file contains stanzas of the form
 *              - 
 *              - <name> {
 *              -         kind  = FUNC | LIB
 *              -         lib   = <lib>
 *              -         path  = <path>
 *              -         value = <rv>
 *              -         argl  = <argl>
 *              -         flag  = <flags>         flags to dlopen
 *              - }
 *              - 
 */

int SC_so_config(char *fname)
   {int is, ns, ok;
    char s[MAXLINE];
    char *lib, *path, *name, *rv, *argl;
    char *key, *oper, *val, *p, **sa;

    ok = FALSE;

    if ((fname != NULL) && (SC_isfile_text(fname) == TRUE))
       {ok = TRUE;

	sa = SC_file_strings(fname);
        SC_ptr_arr_len(ns, sa);

        for (is = 0; (is < ns) && (ok == TRUE); is++)
	    {if (SC_blankl(sa[is], "#") == TRUE)
	        continue;

	     SC_strncpy(s, MAXLINE, sa[is], -1);
	     key  = SC_strtok(s, " \t\n", p);
	     oper = SC_strtok(NULL, " \t\n", p);
	     val  = SC_strtok(NULL, " \t\n", p);

	     if (strchr(oper, '{') != NULL)
	        name = CSTRSAVE(key);

	     else if (strchr(key, '}') != NULL)
	        {ok  &= SC_so_register_func(lib, path, rv, name, argl);
		 lib  = NULL;
		 path = NULL;
		 rv   = NULL;
		 name = NULL;
		 argl = NULL;}

	     else if (strcmp(key, "lib") == 0)
	        lib = CSTRSAVE(val);

	     else if (strcmp(key, "path") == 0)
	        path = CSTRSAVE(val);

	     else if (strcmp(key, "value") == 0)
	        rv = CSTRSAVE(val);

	     else if (strcmp(key, "argl") == 0)
	        argl = CSTRSAVE(val);};

	SC_free_strings(sa);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
