/*
 * PDBDIR.C - provides a directory capability for PDBLib
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

enum {DASHA = 1, DASHR = 2, DASHF = 4};

#define INCR(x)                                                              \
    {SC_LOCKON(PD_dir_lock);                                                 \
     x++;                                                                    \
     SC_LOCKOFF(PD_dir_lock);}

SC_THREAD_LOCK(PD_dir_lock);

int
 _PD_link_attribute = FALSE;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_CD - Change the current working directory in PDBfile FILE to DIR.
 *       - DIR may be an absolute or relative path.
 *       - Return TRUE if successful otherwise return FALSE.
 *
 * #bind PD_cd fortran() scheme() python()
 */

int PD_cd(PDBfile *file ARG(,,cls), const char *dir)
   {int rv, ok;
    char name[MAXLINE];
    char *acc, *rej;
    syment *ep;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    pa->err[0] = '\0';

    if (file == NULL)
       {PD_error("BAD FILE ID - PD_CD", PD_GENERIC);
        return(FALSE);};
     
    if (dir == NULL)
       SC_strncpy(name, MAXLINE, "/", -1);
    else
       {SC_strncpy(name, MAXLINE, _PD_fixname(file, dir), -1);
        if (SC_LAST_CHAR(name) != '/')
           SC_strcat(name, MAXLINE, "/");};

    ep = PD_inquire_entry(file, name, FALSE, NULL);
    if (ep == NULL)
       {if (dir == NULL)
           return(FALSE);

        else
           {if (strcmp(name, "/") != 0)
               {SC_LAST_CHAR(name) = '\0';
                ep = PD_inquire_entry(file, name, FALSE, NULL);
                SC_strcat(name, MAXLINE, "/");};

            if (ep == NULL)
               {snprintf(pa->err, MAXLINE,
			 "ERROR: DIRECTORY %s NOT FOUND - PD_CD\n",
			 dir);
                return(FALSE);};};};

    if (strcmp(ep->type, "Directory") != 0)
       {snprintf(pa->err, MAXLINE,
		 "ERROR: BAD DIRECTORY %s - PD_CD\n", dir);
        return(FALSE);}

    else
       {_PD_pare_symt(file);
	if (file->current_prefix != NULL)
           CFREE(file->current_prefix);
        file->current_prefix = CSTRSAVE(name);}

    rv = TRUE;

    ok = _PD_symt_delay_rules(file, 1, &acc, &rej);
    if (ok == TRUE)
       rv = _PD_rd_symt(file, acc, "*");

    CFREE(acc);
    CFREE(rej);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_LN - Create a symbolic link from NEWNAME to entry OLDNAME in
 *       - PDBfile FILE.
 *       - This function simply installs a new symbol table entry. The new
 *       - syment is a copy of the existing one under OLDNAME,
 *       - but with a name NEWNAME.
 *       - Return TRUE if successful otherwise return FALSE.
 *
 * #bind PD_ln fortran() scheme() python()
 */

int PD_ln(PDBfile *file ARG(,,cls), const char *oldname, const char *newname)
   {syment *oldep;
    char newpath[MAXLINE], oldpath[MAXLINE], dirname[MAXLINE];
    char *nname, *s, **avl;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    pa->err[0] = '\0';

    if (file == NULL)
       {PD_error("BAD FILE ID - PD_LN", PD_GENERIC);
        return(FALSE);};

    if (oldname == NULL)
       {PD_error("VARIABLE NAME NULL - PD_LN", PD_GENERIC);
        return(FALSE);};

/* if opened in read-only mode */
    if (file->mode == PD_OPEN)
       {PD_error("FILE OPENED READ-ONLY - PD_LN", PD_GENERIC);
        return(FALSE);};
     
    nname = _PD_var_namef(file, newname, newpath, MAXLINE);
    if (nname != NULL)
       {SC_strncpy(oldpath, MAXLINE, _PD_fixname(file, oldname), -1);

/* make sure the directory in newname already exists */
	SC_strncpy(dirname, MAXLINE, nname, -1);
	s = strrchr(dirname, '/');
	if ((s != NULL) && (PD_has_directories(file)))
	   {s[1] = '\0';
	    if (PD_inquire_entry(file, dirname, FALSE, NULL) == NULL)
	       {SC_LAST_CHAR(dirname) = '\0';
		snprintf(pa->err, MAXLINE,
			 "ERROR: DIRECTORY %s DOES NOT EXIST - PD_LN\n",
			 dirname);
		return(FALSE);};};

	oldep = PD_inquire_entry(file, oldpath, TRUE, NULL);
	if (oldep == NULL)
	   {snprintf(pa->err, MAXLINE,
		     "ERROR: VARIABLE %s NOT FOUND - PD_LN\n", oldname);
	    return(FALSE);};
           
	_PD_e_install(file, nname, oldep, TRUE);

	if (_PD_link_attribute)
	   {if (!PD_inquire_attribute(file, "LINK", NULL))
	       {if (!PD_def_attribute(file, "LINK", G_STRING_S))
		   {PD_error("CANNOT CREATE LINK ATTRIBUTE - PD_LN",
			     PD_GENERIC);
		    return(FALSE);};};

	    avl  = CMAKE(char *);
	    *avl = CSTRSAVE(_PD_fixname(file, oldname));

	    if (!PD_set_attribute(file, nname, "LINK", (void *) avl))
	       {PD_error("CANNOT SET LINK ATTRIBUTE - PD_LN", PD_GENERIC);
		 return(FALSE);};};};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_SETUP_FLAGS - build byte return value by setting appropriate bit
 *                 - fields.
 *
 */

static int _PD_setup_flags(const char *flags)
   {int i, nc, iflags;
    char csave[MAXLINE], *token, *t;

    iflags = 0;

    SC_strncpy(csave, MAXLINE, flags, -1);

    token = SC_strtok(csave, " \t", t);
    while (token != NULL)
       {if (token[0] == '-')
           {nc = strlen(token);
	    for (i = 1; i < nc; i++)
                {if (token[i] == 'a')
                    iflags |= DASHA;
                 else if (token[i] == 'R')
                    iflags |= DASHR;
                 else if (token[i] == 'f')
                    iflags |= DASHF;};};

        token = SC_strtok(NULL, " \t", t);};

    return(iflags);} 
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_IS_HIDDEN - return TRUE if the supplied entry name has
 *               - any elements whose first character is &,
 *               - which signifies that it should be hidden
 */

static int _PD_is_hidden(const char *pattern, const char *entry)
   {int n, match, rv;
    char *amp;

    n = strlen(pattern);
    n = min(n, 1);
    if (pattern[n-1] == '*')
       n--;

    match = strncmp(pattern, entry, n);

    if (match == 0)
       entry += n; 
/*
    if (strstr(entry, pattern) == entry)
        entry += strlen(pattern);
*/
    rv  = FALSE;
    amp = strchr(entry, '&');
    if (amp != NULL)

/* first character of entry */
       {if (amp == entry)
	   rv = TRUE;

/* first character following directory delimiter */
        else if (amp[-1] == '/')
	   rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_LS_EXTR - return a list of all variables and directories of
 *             - the specified type in the specified directory.
 *             - If TYPE is null, all types are returned. If PATH
 *             - is NULL, the root directory is searched. Directories
 *             - are terminated with a slash.
 */

char **_PD_ls_extr(const PDBfile *file, const char *path, const char *type,
		   long size, int *num, int all, const char *flags)
   {int nc, ne, nvars, i, has_dirs, head, pass; 
    int iflags, psx;
    char **varlist, **outlist;
    char *name, *tp, *p;
    char pattern[MAXLINE];
    syment *ep;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);
     
    pa->err[0] = '\0';

    if (file == NULL)
       {PD_error("BAD FILE ID - PD_LS", PD_GENERIC);
        return(NULL);};

    if (num == NULL)
       {PD_error("LAST ARGUMENT NULL - PD_LS", PD_GENERIC);
        return(NULL);};

    *num = 0;

    ne = SC_hasharr_get_n(file->symtab);
    if (ne == 0)
       return(NULL);

    if (flags != NULL)
       iflags = _PD_setup_flags(flags);
    else
       iflags = 0;
    
/* determine if file contains directories and
 * build a pattern which names must match e.g., '/dir/abc*'
 */
    if (PD_has_directories(file))
       {has_dirs = (all == FALSE);
        if (path == NULL)
           {if (strcmp(PD_pwd(file), "/") == 0)
               SC_strncpy(pattern, MAXLINE, "/*", -1);
            else
               snprintf(pattern, MAXLINE, "%s/*", PD_pwd(file));}
        else
           {SC_strncpy(pattern, MAXLINE, _PD_fixname(file, path), -1);
            ep = PD_inquire_entry(file, pattern, FALSE, NULL);
            if ((ep != NULL) && (strcmp(ep->type, "Directory") == 0))
               {if (SC_LAST_CHAR(pattern) == '/')
                   SC_strcat(pattern, MAXLINE, "*");
                else
                   SC_strcat(pattern, MAXLINE, "/*");}
            else
               {if (SC_LAST_CHAR(pattern) != '/')
                   {SC_strcat(pattern, MAXLINE, "/");
                    ep = PD_inquire_entry(file, pattern, FALSE, NULL);
                    if ((ep != NULL) && (strcmp(ep->type, "Directory") == 0))
                       SC_strcat(pattern, MAXLINE, "*");
                    else
                       SC_LAST_CHAR(pattern) = '\0';}
                else
                   {SC_LAST_CHAR(pattern) = '\0';
                    ep = PD_inquire_entry(file, pattern, FALSE, NULL);
                    if ((ep != NULL) && (strcmp(ep->type, "Directory") == 0))
                       SC_strcat(pattern, MAXLINE, "/*");
                    else
                       SC_strcat(pattern, MAXLINE, "/");};};};}
    else
       {has_dirs = FALSE;
        if (path == NULL)
           SC_strncpy(pattern, MAXLINE, "*", -1);
        else
           SC_strncpy(pattern, MAXLINE, path, -1);};
     
/* generate the list of matching names. Note that this returns items which
 * are in the requested directory AND items which are in sub-directories of
 * the requested directory. In other words, all names which BEGIN with the
 * requested pattern are returned.
 */
    nvars   = 0;
    outlist = CMAKE_N(char *, ne + 1);
     
/* the second pass is in case variables were written to the file before
 * the first directory was created. Such variables lack an initial slash.
 * if we have directories 2 passes might double count entries
 */
    psx = 1 + (has_dirs == FALSE);
    for (pass = 1; pass <= psx; pass++)
        {if (pass == 2)
            {if (has_dirs && (strchr(pattern + 1, '/') == NULL))
                memmove(pattern, pattern + 1, strlen(pattern));
             else
                break;};

         varlist = SC_hasharr_dump(file->symtab, pattern, NULL, FALSE);
         if ((varlist == NULL) || (varlist[0] == NULL))
            continue;
     
/* save only those variables which are IN the requested directory
 * (not in sub-directories), and are of the requested type
 */
	 ep = NULL;
         for (i = 0; (i < ne) && (varlist[i] != NULL); i++)

/* with two passes it is possible to double count so exit if we have
 * everything already
 */
	     {if (nvars >= ne)
		 break;
          
/* the entry '/' (the root directory) is a special case. It
 * is not a child of any directory, so should be ignored.
 */
              if (strcmp("/", varlist[i]) == 0)
                 continue;

/* check to see if type of this variable matches request */
              if ((type != NULL) && (type[0] != '*'))
                 {ep = PD_inquire_entry(file, varlist[i], FALSE, NULL);
                  if ((ep == NULL) || (strcmp(ep->type, type) != 0))
		     continue;};

/* check to see if variable size is less than requested */
	      if ((size > -1L) && (ep != NULL) && (PD_entry_number(ep) < size))
		 continue;

/* check to see if entry is hidden and no DASHA flag */
              if ((!(iflags & DASHA)) && _PD_is_hidden(pattern, varlist[i]))
                 continue;

/* if here, then variable is of right type. If this file has directories,
 * check for any more slashes (/'s) in the name. If any are found, this
 * is not a leaf element. NOTE: if directories are not used, slashes are
 * valid charcters in file names.
 */
              if ((has_dirs) && !(iflags & DASHF))
                 {if (pattern[0] != '/')
                     head = 0;
                  else
                     {tp   = strrchr(pattern, '/');
		      nc   = (tp != NULL) ? strlen(tp) : 0;
		      head = strlen(pattern) - nc + 1;};
                  name = &(varlist[i])[head];
                  nc   = strlen(name);

                  if ((nc == 0) || ((pass == 2) && (name[0] == '/')))
                     continue;

		  p = strchr(name, '/');
                  if ((!(iflags & DASHR)) &&
		      (p != NULL) && (p != (name + nc - 1)))
                     continue;}
              else
                 name = varlist[i];
          
/* variable is of right type and is a leaf in the requested directory */
              outlist[nvars++] = name;};
     
         SC_free_strings(varlist);};
     
/* store a null string to terminate list (just a precaution) */
    outlist[nvars] = NULL;

/* unconditionally sort to make PDBView ls work properly even
 * without directories - Yorick files do this
 */
/*    if (has_dirs) */
       SC_string_sort(outlist, nvars);
     
    *num = nvars;
     
    return(outlist);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_LS - List of all the variables and directories of the specified
 *       - type TYPE in the specified directory PATH of the PDBfile FILE.
 *       - If TYPE is NULL, all types are
 *       - returned. If PATH is NULL, the root directory is searched.
 *       - Directories are terminated with a slash.
 *       - Return the number of items found in NUM.
 *       - Return a NULL terminated array of strings if successful
 *       - and NULL otherwise.
 *
 * #bind PD_ls fortran() scheme() python()
 */

char **PD_ls(const PDBfile *file ARG(,,cls),
	     const char *path, const char *type,
	     int *num)
   {char **rv;

    rv = _PD_ls_extr(file, path, type, -1L, num, FALSE, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_LS_ALT - List of all the variables and directories of the specified
 *           - type TYPE in the specified directory PATH of the PDBfile FILE.
 *           - If TYPE is NULL, all types are
 *           - returned. If PATH is NULL, the root directory is searched.
 *           - Directories are terminated with a slash.
 *           - Return the number of items found in NUM.
 *           - FLAGS modify the search:
 *           -    -a  specifies that all entries, including
 *           -        hidden ones (first char == &) should be 
 *           -        returned. 
 *           - Return a NULL terminated array of strings if successful
 *           - and NULL otherwise.
 *
 * #bind PD_ls_alt fortran() scheme() python()
 */

char **PD_ls_alt(const PDBfile *file ARG(,,cls), const char *path,
		 const char *type, int *num, const char *flags)
   {char **rv;

    rv = _PD_ls_extr(file, path, type, -1L, num, FALSE, flags);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_WR_DIR - actually write the directory entry */

static int _PD_wr_dir(PDBfile *file, const char *name)
   {int ret;
    int *dir;

    if (!PD_has_directories(file))
       {if (PD_inquire_type(file, "Directory") == NULL)
	   PD_def_dir(file);};

    dir  = CMAKE(int);
    *dir = _PD.dir_num;
    ret  = PD_write(file, name, "Directory", dir);

    CFREE(dir);

    if (ret)
       INCR(_PD.dir_num);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_DEF_DIR - Define and initialize the directory machinery for
 *            - PDBfile FILE.
 *
 * #bind PD_def_dir fortran() scheme() python()
 */

int PD_def_dir(PDBfile *file ARG(,,cls))
   {int ret;

    ret = TRUE;

    if (PD_defncv(file, "Directory", 1, 0) == NULL)
       ret = FALSE;

/* write out the root directory */
    else
       {ret = _PD_wr_dir(file, "/");
	if (ret)
	   file->current_prefix = CSTRSAVE("/");
        PD_mkdir(file, "/&ptrs");};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_EXIST_PATH - return TRUE iff the parent directory of PATH
 *                - exists
 */

static int _PD_exist_path(const PDBfile *file, const char *path)
   {int ret;
    char head[MAXLINE];
    char *s;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    ret = TRUE;

    SC_strncpy(head, MAXLINE, path, -1);
    SC_LAST_CHAR(head) = '\0';

    s = strrchr(head, '/');
    if (s != NULL)
       {s[1] = '\0';
	if (PD_inquire_entry(file, head, FALSE, NULL) == NULL)
	   {SC_LAST_CHAR(head) = '\0';
	    snprintf(pa->err, MAXLINE,
		     "ERROR: DIRECTORY %s DOES NOT EXIST - PD_EXIST_PATH\n",
		     head);
	    ret = FALSE;};};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_ISDIR - Return TRUE if DIR is a directory of PDBfile FILE
 *          - and otherwise return FALSE.
 *
 * #bind PD_isdir fortran() scheme() python()
 */

int PD_isdir(const PDBfile *file ARG(,,cls), const char *dir)
   {int ret;
    char name[MAXLINE];
    syment *ep;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    pa->err[0] = '\0';

    ret = FALSE;

    if (file == NULL)
       PD_error("BAD FILE ID - PD_ISDIR", PD_GENERIC);

    else if (dir == NULL)
       PD_error("DIRECTORY NAME NULL - PD_ISDIR", PD_GENERIC);
     
    else

/* build an absolute pathname */
       {SC_strncpy(name, MAXLINE, _PD_fixname(file, dir), -1);
	if (SC_LAST_CHAR(name) != '/')
	   SC_strcat(name, MAXLINE, "/");

	ep = PD_inquire_entry(file, name, FALSE, NULL);
	ret = ((ep != NULL) && (strcmp(PD_entry_type(ep), "Directory") == 0));};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_MKDIR - Create a directory DIR in PDBfile FILE.
 *          - The directory may be specified by an absolute or relative path.
 *          - Return TRUE successful and otherwise return FALSE.
 *
 * #bind PD_mkdir fortran() scheme() python()
 */

int PD_mkdir(PDBfile *file ARG(,,cls), const char *dir)
   {int ret;
    char name[MAXLINE];
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    pa->err[0] = '\0';

    ret = FALSE;

    if (file == NULL)
       PD_error("BAD FILE ID - PD_MKDIR", PD_GENERIC);

    else if (dir == NULL)
       PD_error("DIRECTORY NAME NULL - PD_MKDIR", PD_GENERIC);
     
    else

/* build an absolute pathname */
       {SC_strncpy(name, MAXLINE, _PD_fixname(file, dir), -1);
	if (SC_LAST_CHAR(name) != '/')
	   SC_strcat(name, MAXLINE, "/");

/* make sure this directory hasn't already been created */
	if (PD_inquire_entry(file, name, FALSE, NULL) != NULL)

/* since we make /&ptrs when we create the file do not complain
 * if someone tries to create it again
 * PDBView copy operations end up doing this
 */
	   {if (strcmp(name, "/&ptrs/") == 0)
	       ret = TRUE;
	    else
	       snprintf(pa->err, MAXLINE,
			"ERROR: DIRECTORY %s ALREADY EXISTS - PD_MKDIR\n",
			name);}

/* write the directory variable if the parent exists */
	else if (_PD_exist_path(file, name) == TRUE)
	   ret = _PD_wr_dir(file, name);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PD_PWD - Return the current working directory of the PDBfile FILE
 *        - if successful and return FALSE otherwise.
 *
 * #bind PD_pwd fortran() scheme() python()
 */

char *PD_pwd(const PDBfile *file ARG(,,cls))
   {char *cwd, *pre;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    pa->err[0] = '\0';
   
    if (file == NULL)
       {PD_error("BAD FILE ID - PD_PWD", PD_GENERIC);
        return(NULL);};

    cwd = pa->cwd;
    pre = file->current_prefix;

    if ((pre == NULL) || (strcmp(pre, "/") == 0))
       strcpy(cwd, "/");

    else
       {strcpy(cwd, pre);
        SC_LAST_CHAR(cwd) = '\0';};

    return(cwd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PD_FIXNAME - make full pathname from current working directory
 *             - and the given pathname (absolute or relative)
 */

char *_PD_fixname(const PDBfile *file, const char *inname)
   {char *node, *p, *s, *out;
    char tmpstr[MAXLINE];
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    if ((file == NULL) || (inname == NULL))
       return(NULL);

    out = pa->outname;
    SC_strncpy(out, MAXLINE, "/", -1);

    if (!PD_has_directories(file))

/* if no directories, just copy verbatim */
       SC_strncpy(out, MAXLINE, inname, -1);

    else
          
/* break path into slash-separated tokens
 * process each node individually
 * NOTE: all of this code is to eliminate . or .. in paths
 */
       {if (inname[0] != '/')
           SC_strncpy(out, MAXLINE, PD_pwd(file), -1);

        SC_strncpy(tmpstr, MAXLINE, inname, -1);
        node = SC_strtok(tmpstr, "/", s);
          
        while (node != NULL)
           {if (strcmp(".",  node) == 0)
               {/* no-op */}

/* go up one level, unless already at top */
            else if (strcmp("..", node) == 0)
               {if (strcmp("/", out) != 0)
                   {if (SC_LAST_CHAR(out) == '/')
                       SC_LAST_CHAR(out) = '\0';
                    p = strrchr(out, '/');
                    if (p != NULL)
                       p[0] = '\0';};}

/* append to end of current path */
            else
               {if (SC_LAST_CHAR(out) != '/')
                   SC_strcat(out, MAXLINE, "/");
                SC_strcat(out, MAXLINE, node);}

            node = SC_strtok(NULL, "/", s);};

        if ((SC_LAST_CHAR(inname) == '/') &&
            (SC_LAST_CHAR(out) != '/'))
           SC_strcat(out, MAXLINE, "/");};

    return(out);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

