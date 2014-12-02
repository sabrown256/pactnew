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

#undef SO_MAIN
#define SO_MAIN "_main_"

typedef struct s_sodes sodes;
typedef struct s_sostate sostate;

struct s_sodes
   {objindex kind;
    char *flags;      /* flags for controlling dlsym */
    char *lib;        /* shared object file name */
    char *path;       /* path to shared object file */
    char *rv;         /* return value of function */
    char *name;       /* function name */
    char *argl;       /* function argument list */
    void *so;         /* the shared object handle */
    void *f;};        /* pointer to the function */

struct s_sostate
   {hasharr *tab;};

sostate
 _SC_dl;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SO_REGISTER_FUNC - register a function to be access via
 *                     - a shared object
 *                     - return TRUE iff successful
 */

int SC_so_register_func(objindex kind, char *lib, char *name,
			char *path, char *flags, char *rv, char *argl)
   {int ok;
    sodes *se;

    ok = FALSE;

    if (name != NULL)
       {if (_SC_dl.tab == NULL)
	   {_SC_dl.tab = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY, 0);

/* register the current executable for optimal lookup scoping */
	    SC_so_register_func(OBJ_EXE, NULL, SO_MAIN,
				NULL, NULL, NULL, NULL);};

	se = CMAKE(sodes);
	if (se != NULL)
	   {se->kind  = kind;
	    se->flags = (flags != NULL) ? CSTRSAVE(flags) : NULL;
	    se->lib   = CSTRSAVE(lib);
	    se->path  = CSTRSAVE(path);
	    se->rv    = (rv != NULL) ? CSTRSAVE(rv) : NULL;
	    se->name  = CSTRSAVE(name);
	    se->argl  = (argl != NULL) ? CSTRSAVE(argl) : NULL;
	    se->so    = NULL;
	    se->f     = NULL;

	    SC_hasharr_install(_SC_dl.tab, name, se, "sodes", 3, TRUE);

	    ok = TRUE;};};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SO_CLOSE - close shared object associated with TAG
 *              - return TRUE iff successful
 */

static int _SC_so_close(sodes *se)
   {int st;
    char msg[MAXLINE];

    st = -1;

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

/* _SC_SO_FREE - free the contents of a sodes */

int _SC_so_free(haelem *hp, void *arg)
   {int rv;
    sodes *se;

    rv = TRUE;

    se = (sodes *) hp->def;
    if (se != NULL)
       {rv = _SC_so_close(se);

	CFREE(se->flags);
	CFREE(se->lib);
	CFREE(se->path);
	CFREE(se->rv);
	CFREE(se->name);
	CFREE(se->argl);

	se->kind = OBJ_UNKNOWN;
	se->so   = NULL;
	se->f    = NULL;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SO_RELEASE - release all shared objects and registry */

int SC_so_release(void)
   {int rv;

    rv = TRUE;

    SC_free_hasharr(_SC_dl.tab, _SC_so_free, NULL);
    _SC_dl.tab = NULL;

    return(rv);}

/*--------------------------------------------------------------------------*/

#ifdef HAVE_DYNAMIC_LINKER

/*--------------------------------------------------------------------------*/

/* _SC_SO_PARSE_FLAGS - parse the flags for dlsym from SE */

static int _SC_so_parse_flags(sodes *se)
   {int flag, is, ns;
    char s[MAXLINE];
    char **sa;

    if (se->flags == NULL)
       flag = RTLD_LAZY | RTLD_GLOBAL;
	   
    else
       {SC_strncpy(s, MAXLINE, se->flags, -1);
	sa = SC_tokenize(s, " |");
	SC_ptr_arr_len(ns, sa);

/* parse out the flags */
	flag = 0;
	for (is = 0; is < ns; is++)

/* RTLD_LAZY - resolve on demand */
	    {if (strcmp(sa[is], "lazy") == 0)
	        flag |= RTLD_LAZY;

/* RTLD_NOW - resolve on open */
	     else if (strcmp(sa[is], "now") == 0)
	        flag |= RTLD_NOW;

/* RTLD_GLOBAL - use to resolve other libraries */
	     else if (strcmp(sa[is], "global") == 0)
	        flag |= RTLD_GLOBAL;

/* RTLD_LOCAL - use to resolve this library only */
	     else if (strcmp(sa[is], "local") == 0)
	        flag |= RTLD_LOCAL;};

	SC_free_strings(sa);};

    return(flag);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* _SC_SO_OPEN - open shared object TAG
 *             - TAG is the name under which an object is registered
 *             - return a handle to the shared object
 */

static sodes *_SC_so_open(char *tag)
   {char msg[MAXLINE];
    sodes *se;

    se = (sodes *) SC_hasharr_def_lookup(_SC_dl.tab, tag);
    if (se != NULL)
       {

#ifdef HAVE_DYNAMIC_LINKER
	int flag;
	char *lib;
	void *so;

	lib  = se->lib;
	flag = _SC_so_parse_flags(se);

	so = dlopen(lib, flag);
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

/* SC_SO_CLOSE - close shared object associated with TAG
 *             - return TRUE iff successful
 */

int SC_so_close(char *tag)
   {int st;
    sodes *se;

    se = (sodes *) SC_hasharr_def_lookup(_SC_dl.tab, tag);
    st = _SC_so_close(se);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SO_GET - return the function associated with TAG
 *           - from a shared object KIND
 */

void *SC_so_get(objindex kind, char *tag, ...)
   {char msg[MAXLINE];
    char *fnc;
    sodes *se;
    void *f;

    f = NULL;

    se = _SC_so_open(tag);
    if (se != NULL)
       {SC_VA_START(tag);

/* determine the function name */
	switch (kind)
	   {case OBJ_FUNC :
	         fnc = se->name;
		 break;
	    case OBJ_SO :
	    case OBJ_EXE :
		 fnc = SC_VA_ARG(char *);
		 break;
	    default :
	         fnc = NULL;
		 break;};

	SC_VA_END;

        if (se != NULL)
           {if (se->f != NULL)
	       f = se->f;

	    else if (fnc != NULL)

#ifdef HAVE_DYNAMIC_LINKER
	       {char *s;
		sodes *sem;

/* get the so for the current executable for optimal symbol resolution */
		sem = _SC_so_open(SO_MAIN);

/* clear any existing error */
		dlerror();

		f = dlsym(sem->so, fnc);

		s = dlerror();
		if (s != NULL)
		   snprintf(msg, MAXLINE, "ERROR: %s", s);

		if (se->kind == OBJ_FUNC)
		   se->f = f;};

#else
	       snprintf(msg, MAXLINE, "ERROR: No dynamic linker");
#endif
	   };};

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SO_CONFIG - read a PACT so.conf file FNAME
 *              - an so.conf file contains stanzas of the form
 *              - 
 *              - <name> {
 *              -         kind  = function | so
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
    objindex kind;
    char s[MAXLINE];
    char *lib, *path, *flags, *name, *rv, *argl;
    char *key, *oper, *val, *p, **sa;

    ok = FALSE;

    if ((fname != NULL) && (SC_isfile_text(fname) == TRUE))
       {ok = TRUE;

	sa = SC_file_strings(fname);
        SC_ptr_arr_len(ns, sa);

	kind  = OBJ_FUNC;
	lib   = NULL;
	flags = NULL;
	path  = NULL;
	rv    = NULL;
	name  = NULL;
	argl  = NULL;

        for (is = 0; (is < ns) && (ok == TRUE); is++)
	    {if (SC_blankl(sa[is], "#") == TRUE)
	        continue;

	     SC_strncpy(s, MAXLINE, sa[is], -1);
	     key  = SC_strtok(s, " \t\n", p);
	     oper = SC_strtok(NULL, " \t\n", p);
	     val  = SC_strtok(NULL, " \t\n", p);

	     if ((oper != NULL) && (strchr(oper, '{') != NULL))
	        name = CSTRSAVE(key);

	     else if ((key != NULL) && (strchr(key, '}') != NULL))
	        {ok  &= SC_so_register_func(kind, lib, name,
					    path, flags, rv, argl);
		 kind  = OBJ_FUNC;
		 lib   = NULL;
		 flags = NULL;
		 path  = NULL;
		 rv    = NULL;
		 name  = NULL;
		 argl  = NULL;}

/* ignore expressions with key or val NULL */
	     else if ((key == NULL) || (val == NULL))
	        continue;

	     else if (strcmp(key, "kind") == 0)
	        {if (strcmp(val, "function") == 0)
		   kind = OBJ_FUNC;
	         else if (strcmp(val, "so") == 0)
		   kind = OBJ_SO;}

	     else if (strcmp(key, "lib") == 0)
	        lib = CSTRSAVE(val);

	     else if (strcmp(key, "path") == 0)
	        path = CSTRSAVE(val);

	     else if (strcmp(key, "flags") == 0)
	        flags = CSTRSAVE(val);

	     else if (strcmp(key, "value") == 0)
	        rv = CSTRSAVE(val);

	     else if (strcmp(key, "argl") == 0)
	        argl = CSTRSAVE(val);};

	SC_free_strings(sa);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
