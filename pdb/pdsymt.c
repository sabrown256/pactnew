/*
 * PDSYMT.C - routines for delayed symbol table reading
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_IS_VAR_IN_DIR - return TRUE iff VAR is in directory DIR */

int _PD_is_var_in_dir(char *var, char *dir)
   {int ok;
    char *p, *d;

    ok = FALSE;
    if (var != NULL)
       {if (dir == NULL)
	   dir = "/";

	p = strstr(var, dir);

/* if the first part of VAR matches the current directory */
	if (p == var)
	   {p += strlen(dir);

/* if the remainder has no '/' or ends in '/' it is in the current directory */
	    d = strchr(p, '/');
	    if ((d == NULL) || (d[1] == '\0'))
	       ok = TRUE;};};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_ADD_SYM_PTRN - return TRUE iff NAME is not a pointer
 *                  - if TRUE NAME should be added to the symbol table
 *                  - if AD is TRUE decide whether or not to add
 *                  - if AD if FALSE decide whether to remove
 */

int _PD_add_sym_ptrn(PDBfile *file, int ad,
		     const char *name, const char *type,
		     char *acc, char *rej)
   {int ok;

    if (name == NULL)
       ok = FALSE;

    else if (strncmp(name, file->ptr_base, strlen(file->ptr_base)) == 0)
       ok = FALSE;

    else
       ok = (ad == TRUE);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_ADD_SYM_PTRP - return TRUE iff NAME is not a pointer
 *                  - or it is not in the current directory
 *                  - if TRUE NAME should be added to the symbol table
 *                  - if AD is TRUE decide whether or not to add
 *                  - if AD if FALSE decide whether to remove
 */

int _PD_add_sym_ptrp(PDBfile *file, int ad,
		     const char *name, const char *type,
		     char *acc, char *rej)
   {int ok;
    char *p, *s;

    if (name == NULL)
       ok = FALSE;

    else if (strncmp(name, file->ptr_base, strlen(file->ptr_base)) == 0)
       {p = file->current_prefix;
	s = strchr(name, '#');

/* we must add old style pointers without pathnames because
 * we do not know who they belong to
 */
	if (s == NULL)
	   ok = TRUE;
	else
	   {s++;
	    ok = _PD_is_var_in_dir(s, p);};}

    else
       ok = (ad == TRUE);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_ADD_SYM_ANCP - return TRUE iff NAME is not a pointer or
 *                  - it is a pointer in the current directory
 *                  - or its ancestors -  that is, toward root
 *                  - if TRUE NAME should be added to the symbol table
 *                  - if AD is TRUE decide whether or not to add
 *                  - if AD if FALSE decide whether to remove
 */

int _PD_add_sym_ancp(PDBfile *file, int ad,
		     const char *name, const char *type,
		     char *acc, char *rej)
   {int ok;
    char *p, *d, *s;

    if (name == NULL)
       ok = FALSE;

    else if (ad == TRUE)
       {if (file->current_prefix == NULL)
	   p = CSTRSAVE("/");
	else
	   p = CSTRSAVE(file->current_prefix);

	if (strncmp(name, file->ptr_base, strlen(file->ptr_base)) == 0)
	   {s = strchr(name, '#');
	    if (s != NULL)
               {if (file->system_version > 28)
		   s = CSTRSAVE(s+1);
	        else
		   s = SC_dsnprintf(TRUE, "%s_ptr_", s + 1);}
	    else
	       s = CSTRSAVE("/");}
	else
	   s = CSTRSAVE(name);

	ok  = FALSE;
	ok |= (strcmp(s, p) == 0);
	while (ok == FALSE)
	   {d = strrchr(p, '/');
	    if (d != NULL)
	       {d[1] = '\0';
		ok = _PD_is_var_in_dir(s, p);
		d[0] = '\0';}
	    else
	       break;};

	CFREE(p);
	CFREE(s);}

    else
       ok = TRUE;

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_ADD_SYM_DESP - return TRUE iff NAME is not a pointer or
 *                  - it is a pointer in the current directory
 *                  - or it descendants - that is, away from root
 *                  - if TRUE NAME should be added to the symbol table
 *                  - if AD is TRUE decide whether or not to add
 *                  - if AD if FALSE decide whether to remove
 */

int _PD_add_sym_desp(PDBfile *file, int ad,
		     const char *name, const char *type,
		     char *acc, char *rej)
   {int ok, nc, ex;
    char *s, *p;

    if (name == NULL)
       ok = FALSE;

/* if checking to add - do not add anything that is already there */
    else
       {if (ad == TRUE)
	   ex = (PD_inquire_entry(file, name, TRUE, NULL) != NULL);
	else
	   ex = FALSE;

	if (ex == TRUE)
	   ok = FALSE;
	else
	   {if (strncmp(name, file->ptr_base, strlen(file->ptr_base)) == 0)
	       s = SC_dsnprintf(TRUE, "%s_ptr_", strchr(name, '#') + 1);
	    else
	       s = CSTRSAVE(name);

	    p = file->current_prefix;
	    if (p == NULL)
	       p = "/";

	    nc = strlen(p);

	    ok = (strncmp(p, s, nc) == 0);
	    if (ad == FALSE)
	       ok &= (SC_LAST_CHAR(s) != '/');

	    CFREE(s);};};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_ADD_SYM_CURRP - return TRUE iff NAME is not a pointer or
 *                   - it is a pointer in the current directory only
 *                   - if TRUE NAME should be added to the symbol table
 *                   - if AD is TRUE decide whether or not to add
 *                   - if AD if FALSE decide whether to remove
 */

int _PD_add_sym_currp(PDBfile *file, int ad,
		      const char *name, const char *type,
		      char *acc, char *rej)
   {int ok;
    char *s;

    if (name == NULL)
       ok = FALSE;

    else
       {if (strncmp(name, file->ptr_base, strlen(file->ptr_base)) == 0)
	   {s = strchr(name, '#');
	    if (s != NULL)
	       {if (file->system_version > 28)
		   s = CSTRSAVE(s+1);
		else
		   s = SC_dsnprintf(TRUE, "%s_ptr_", s + 1);}
	    else
	       s = CSTRSAVE("/");}
	else
	   s = CSTRSAVE(name);

	if (ad == TRUE)
	   {ok  = _PD_is_var_in_dir(s, file->current_prefix);
	    ok &= (SC_hasharr_lookup(file->symtab, name) == NULL);}
	else
	   ok = (SC_LAST_CHAR(s) != '/');

	CFREE(s);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SYMT_DELAY_RULES - set the standard accept and reject rules for
 *                      - the various delay modes
 *                      - return TRUE iff any rules are set
 *                      - if WHEN is
 *                      -    0 set rules for opening file
 *                      -    1 set rules for changing directory
 */

int _PD_symt_delay_rules(PDBfile *file, int when, char **pa, char **pr)
   {int rv;
    char *acc, *rej;

    rv = FALSE;

    acc = NULL;
    rej = NULL;

    switch (when)

/* for open file */
       {case 0 :
	     if (file->delay_sym == PD_DELAY_PTRS)
	        {rej = SC_dsnprintf(TRUE, "%s*#/*", file->ptr_base);
		 rv  = TRUE;}
	     else if (file->delay_sym == PD_DELAY_ALL)
	        {rej = SC_dsnprintf(TRUE, "*");
		 acc = SC_dsnprintf(TRUE, "*/");
		 rv  = TRUE;};
	     break;

/* for change directory */
	case 1 :
	     if (file->delay_sym == PD_DELAY_PTRS)
	        {acc = SC_dsnprintf(TRUE, "%s*#%s",
				    file->ptr_base, file->current_prefix);
		 rv  = TRUE;}
	     else if (file->delay_sym == PD_DELAY_ALL)
	        {acc = SC_dsnprintf(TRUE, "*%s*", file->current_prefix);
		 rv  = TRUE;};
	     break;};

    if (pa != NULL)
       *pa = acc;
    else
       CFREE(acc);

    if (pr != NULL)
       *pr = rej;
    else
       CFREE(rej);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SYMT_SET_DELAY_MODE - set the file delay mode
 *                         - return the old value
 */

PD_delay_mode _PD_symt_set_delay_mode(PDBfile *file, const char *mode)
   {PD_delay_mode ov;

    ov = PD_DELAY_NONE;

    if (file != NULL)
       {ov = file->delay_sym;

/* if file mode is "rnp" or "wnp" do not read/write pointers
 * rely on itags solely
 * saves memory needed for pointers in symbol table
 * both in RAM and on disk
 */
	if (strstr(mode, "np") != NULL)
	   {file->delay_sym = PD_DELAY_PTRS;

/* if no-pointer mode is requested force format_version 2 */
	    if (strstr(mode, "np") != NULL)
	       _PD_format_version(file, 2);

	    _PD_symt_set_delay_method(file, mode, _PD_add_sym_ptrn);}

/* if file mode is "rp" use delay pointer mode
 * saves memory in symbol table by only
 * bringing in pointers as you cd to the directory in which
 * they reside
 */
	else if (strchr(mode, 'p') != NULL)
	   {file->delay_sym = PD_DELAY_PTRS;
	    _PD_symt_set_delay_method(file, mode, _PD_add_sym_ptrp);}

/* if file mode is "rsa" use delay all mode
 * saves memory in symbol table by only
 * bringing in entries as you cd to the directory in which
 * they reside or in ancestors of that directory
 */
	else if (strstr(mode, "sa") != NULL)
	   {file->delay_sym = PD_DELAY_ALL;
	    _PD_symt_set_delay_method(file, mode, _PD_add_sym_ancp);}

/* if file mode is "rsd" use delay all mode
 * saves memory in symbol table by only
 * bringing in entries as you cd to the directory in which
 * they reside or in descendants of that directory
 */
	else if (strstr(mode, "sd") != NULL)
	   {file->delay_sym = PD_DELAY_ALL;
	    _PD_symt_set_delay_method(file, mode, _PD_add_sym_desp);}

/* if file mode is "rsc" use delay all mode
 * saves memory in symbol table by only
 * bringing in entries as you cd to the directory in which
 * they reside
 */
	else if (strstr(mode, "sc") != NULL)
	   {file->delay_sym = PD_DELAY_ALL;
	    _PD_symt_set_delay_method(file, mode, _PD_add_sym_currp);}

	else
	   _PD_symt_set_delay_method(file, mode, NULL);};

    return(ov);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SYMT_SET_DELAY_METHOD - set the file delay method
 *                           - return the old value
 */

PFSymDelay _PD_symt_set_delay_method(PDBfile *file, const char *mode,
				     PFSymDelay mth)
   {PFSymDelay rv;

    rv = NULL;

/* use global default if MTH is NULL */
    if (mth == NULL)
       mth = _PD.symt_delay;

    if (file != NULL)
       {rv = file->symatch;

/* if file mode is "rp", "rnp", "wnp", "rsx" use a delay mode */
	if ((strchr(mode, 'p') != NULL) ||
	    (strchr(mode, 's') != NULL))
	   file->symatch = mth;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_SET_SYMT_DELAY_METHOD - set the default global delay method
 *                          - return the old value
 */

PFSymDelay PD_set_symt_delay_method(PFSymDelay mth)
   {PFSymDelay rv;

    rv = _PD.symt_delay;
    _PD.symt_delay = mth;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PARE_MATCH - predicate function for SC_hasharr_prune
 *                - if the element name matches remove it from
 *                - the pointer table, free the syment, and
 *                - return TRUE
 */

static int _PD_pare_match(hasharr *ha, haelem *hp, void *a)
   {int rv;
    long nc;
    char *base, *nm;
    syment *ep;
    PDBfile *file;

    file = (PDBfile *) a;
    nm   = hp->name;

    rv = FALSE;

    if (_PD_symatch(file, FALSE, nm, NULL, NULL, NULL) == TRUE)
       {ep = hp->def;

	base = file->ptr_base;
	nc   = strlen(base);
	if (strncmp(nm, base, nc) == 0)
	   _PD_ptr_remove_entry(file, ep, TRUE);

	_PD_rl_syment_d(ep);

	hp->def = NULL;

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PARE_DIRD - predicate function for SC_hasharr_prune
 *               - if the element name ends in slash
 *               - return TRUE
 */

static int _PD_pare_dird(hasharr *ha, haelem *hp, void *a)
   {int rv;
    char *nm;
    syment *ep;

    nm = hp->name;

    rv = (SC_LAST_CHAR(nm) != '/');
    if (rv == TRUE)
       {ep = hp->def;
	_PD_rl_syment_d(ep);
	hp->def = NULL;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PARE_DIRS - predicate function for SC_hasharr_prune
 *               - if the element name ends in slash
 *               - return TRUE
 */

static int _PD_pare_dirs(hasharr *ha, haelem *hp, void *a)
   {int rv;
    char *nm;

    nm = hp->name;

    rv = (SC_LAST_CHAR(nm) != '/');

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_PARE_SYMT - remove symbol table entries added in the last cd
 *               - return the number of entries removed
 */

int _PD_pare_symt(PDBfile *file)
   {int nr, wh;

    nr = 0;

    if (file->symatch != NULL)
       nr = SC_hasharr_prune(file->symtab, _PD_pare_match, file);

    else
       {wh = 0;

	if ((file->delay_sym != PD_DELAY_NONE) &&
	    (strcmp(file->current_prefix, "/") != 0))

/* deep remove including the syment */
	   {if (wh == 1)
	       nr = SC_hasharr_prune(file->symtab, _PD_pare_dird, file);

/* shallow remove excluding the syment */
	    else
	       nr = SC_hasharr_prune(file->symtab, _PD_pare_dirs, file);};};

    return(nr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_RD_SYMT - read entries from the symbol table
 *             - entries are checked first against REJ
 *             - if they match they are not entered in the symbol table
 *             - if not rejected they are checked against ACC
 *             - if they match they are entered in the symbol table
 *             - if REJ is NULL everything goes to the ACC filter
 *             - if ACC is NULL everything passing the REJ filter is accepted
 *             - regular expressions are interpreted as follows:
 *             -   <base>*#/a/b* matches all of:
 *             -      <base>*#/a/b
 *             -      <base>*#/a/b/c
 *             -      <base>*#/a/b/c/
 *             -      <base>*#/a/b/c/d
 *             - where <base> is the file->ptr_base string
 *             - return TRUE iff successful
 */

int _PD_rd_symt(PDBfile *file, char *acc, char *rej)
   {int rv;

    rv = (*file->rd_symt)(file, acc, rej);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
