/*
 * LIBPSH.C - routines supporting simply portable C programs
 *          - which do the same kind of work as shell scripts
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * include "cpyright.h"
 *
 */

#ifndef LIBPSH

#define LIBPSH

#undef TEXT
#define TEXT       10
#define HTML       11

#define FRONT      1
#define BACK       2

#define CMD_LINE   1
#define CMD_OUT    2

#define BOTH       3

#define STACK_FILE      1
#define STACK_PROCESS   2
#define STACK_TOOL      3
#define STACK_GROUP     4

#define PHASE_READ      10
#define PHASE_ANALYZE   11
#define PHASE_WRITE     12

#define UNDEFINED  "--undefd--"

typedef struct s_dir_stack dir_stack;

struct s_dir_stack
   {int n;
    char *dir[N_STACK];};

static char
 epath[LRG],
 lpath[LRG];

static FILE
 *Log;

static dir_stack
 dstck;

extern void
 unamef(char *s, int nc, char *wh);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NSTRSAVE - safe strdup function */

char *nstrsave(char *s)
   {char *d;

    d = NULL;
    if (s != NULL)
       d = strdup(s);

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NSTRNCPY - this is to strncpy as snprintf is to sprintf
 *          - a safe string copy function
 *          - unlike strncpy this always returns a null terminated string
 */

char *nstrncpy(char *d, size_t nd, char *s, size_t ns)
   {size_t nc;
        
    if ((s != NULL) && (d != NULL))
       {nc = min(ns, nd-1);
	nc = max(nc, 0);
	strncpy(d, s, nc);
	d[nc] = '\0';};

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NSTRCAT - safe strcat function */

char *nstrcat(char *d, int nc, char *s)
   {int n, nd, ns;

    if (d != NULL)
       {nd = strlen(d);
	ns = strlen(s);
	n  = nc - 1 - nd;
	n  = min(n, ns);
	n  = max(n, 0);

	nstrncpy(d+nd, n+1, s, -1);
	d[nd+n] = '\0';};

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* VSTRCAT - safe strcat function */

char *vstrcat(char *d, int nc, char *fmt, ...)
   {char s[LRG];

    VA_START(fmt);
    VSNPRINTF(s, LRG, fmt);
    VA_END;

    d = nstrcat(d, nc, s);

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRING_SORT - sort an array of character pointers
 *             - by what they point to
 */

void string_sort(char **v, int n)
   {int gap, i, j;
    char *temp;

    for (gap = n/2; gap > 0; gap /= 2)
        for (i = gap; i < n; i++)
            for (j = i-gap; j >= 0; j -= gap)
                {if (strcmp(v[j], v[j+gap]) <= 0)
                    break;
                 temp     = v[j];
                 v[j]     = v[j+gap];
                 v[j+gap] = temp;};
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LST_LENGTH - return the length of the list */

int lst_length(char **lst)
   {int n;

    n = 0;
    if (lst != NULL)
       {for (n = 0; lst[n] != NULL; n++);};

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LST_PUSH - add S to list LST */

char **lst_push(char **lst, char *fmt, ...)
   {int n, m;
    char s[MAXLINE];

    if (lst == NULL)
       {lst = MAKE_N(char *, 100);
	if (lst != NULL)
	   memset(lst, 0, 100*sizeof(char *));};

    if (lst != NULL)
       {n = lst_length(lst);
	if (n % 100 == 98)
	   {m = n + 102;
	    REMAKE(lst, char *, m);
	    if (lst != NULL)
	       memset(lst+n, 0, 102*sizeof(char *));};

	if (lst != NULL)
	   {if (fmt == NULL)
	       lst[n] = NULL;
	    else
	       {VA_START(fmt);
		VSNPRINTF(s, MAXLINE, fmt);
		VA_END;

		lst[n] = STRSAVE(s);};};};

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LST_ADD - add S to list LST */

char **lst_add(char **lst, char *s)
   {int n, m;

    if (lst == NULL)
       {lst = MAKE_N(char *, 100);
	if (lst != NULL)
	   memset(lst, 0, 100*sizeof(char *));};

    if (lst != NULL)
       {n = lst_length(lst);
	if (n % 100 == 98)
	   {m = n + 102;
	    REMAKE(lst, char *, m);
	    if (lst != NULL)
	       memset(lst+n, 0, 102*sizeof(char *));};

	if (lst != NULL)
	   {if (s == NULL)
	       lst[n] = NULL;
	    else
	       lst[n] = STRSAVE(s);};};

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LST_COPY - return a copy of LST */

char **lst_copy(char **lst)
   {int i, h, n, m;
    char **nlst;

    nlst = NULL;

    if (lst != NULL)
       {n = lst_length(lst);
        h = (n + 99) / 100;
        m = 100 * h;

        nlst = MAKE_N(char *, m);
	if (nlst != NULL)
	   {memset(nlst, 0, m*sizeof(char *));

	    for (i = 0; i < n; i++)
	        nlst[i] = STRSAVE(lst[i]);};};

    return(nlst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LST_UNIQ - remove duplicate entries from LST */

char **lst_uniq(char **lst)
   {int i, j, k, n;
    char *s;

    if (lst != NULL)
       {n = lst_length(lst);
        for (i = 0; i < n; i++)
	    {s = lst[i];
	     for (j = i+1; j < n; j++)
	         {if (strcmp(s, lst[j]) == 0)
		     {for (k = j+1; k < n; k++)
			  lst[k-1] = lst[k];
		      lst[k-1] = NULL;
		      n--;
		      i--;
		      break;};};};};

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LST_FREE - free the list */

void lst_free(char **lst)
   {int i, n;

    if (lst != NULL)
       {n = lst_length(lst);
	for (i = 0; i < n; i++)
	    FREE(lst[i]);

	FREE(lst);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONCATENATE - concatenate tokens SA to string S */

char *concatenate(char *s, int nc, char **sa, char *dlm)
   {int i, ns, nd, n;

    if ((s != NULL) && (nc > 0))
       {s[0] = '\0';

	if (sa != NULL)
	   {for (i = 0; sa[i] != NULL; i++)
	        vstrcat(s, nc, "%s%s", sa[i], dlm);

	    ns = strlen(s);
	    nd = strlen(dlm);
	    n  = ns - nd;
	    n  = max(n, 0);
	    s[n] = '\0';};};

    return(s);}

/*--------------------------------------------------------------------------*/

#ifdef NO_UNSETENV

/*--------------------------------------------------------------------------*/

/* UNSETENV - unset the environment variable VAR */

int unsetenv(char *var)
   {int i, nc, ne, err;
    char s[MAXLINE];
    extern char **environ;

    err = 0;

/* count the environment variables */
    for (ne = 0; environ[ne] != NULL; ne++);

    snprintf(s, MAXLINE, "%s=", var);
    nc = strlen(s);

    for (i = 0; i < ne; i++)
        {if (strncmp(s, environ[i], nc) == 0)
            {ne--;
	     environ[i]  = environ[ne];
             environ[ne] = NULL;
	     break;};};

    return(err);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* TOKENIZE - return an array of strings obtained by
 *          - tokenizing the string S according to the delimiters DELIM
 *          - the array is terminated by a NULL string
 *          - it can be released by free_strings
 */

char **tokenize(char *s, char *delim)
   {int i, n, c, ns;
    char *p, *t, *ps, **sa;

    sa = NULL;
    n  = strlen(s);
    t  = MAKE_N(char, n+100);
    if (t != NULL)
       {nstrncpy(t, n+100, s, -1);

	for (i = 0, ps = t; ps != NULL; )
	    {if (sa == NULL)
	        sa = MAKE_N(char *, 1000);

	     if (sa != NULL)
	        {ns  = strspn(ps, delim);
		 ps += ns;

/* find the next unescaped delimiter
 * we could use strpbrk except for escapes
 */
                 for (p = ps; *p != '\0'; p++)
		     {c = *p;
		      if (c == '\\')
			 p++;
		      else if (strchr(delim, c) != NULL)
			 break;};

		 if (*p != '\0')
		    {c  = *p;
		     *p = '\0';
		     sa[i++] = STRSAVE(ps);
		     *p = c;
		     ps = p + 1;}
		 else
		    {if (IS_NULL(ps) == FALSE)
			sa[i++] = STRSAVE(ps);
		     break;};};};

	if (sa != NULL)
	   sa[i++] = NULL;

	FREE(t);};

    return(sa);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FREE_STRINGS - free the strings in the array lst */

void free_strings(char **lst)
   {int i;

    if (lst != NULL)
       {for (i = 0; lst[i] != NULL; i++)
	    {FREE(lst[i]);};
        FREE(lst);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LAST_CHAR - return a valid last character index into S */

int last_char(char *s)
   {int nc;

    nc = strlen(s) - 1;
    nc = max(0, nc);

    return(nc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NCHAR - return the number of occurences of C in S */

int nchar(char *s, int c)
   {int i, nc;

    for (i = 0, nc = 0; s[i] != '\0'; i++)
        nc += (s[i] == c);
        
    return(nc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UPCASE - map S to upper case */

char *upcase(char *s)
   {size_t i, nc;
        
    nc = strlen(s);
    for (i = 0; i < nc; i++)
        s[i] = toupper((int) s[i]);

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DOWNCASE - map S to lower case */

char *downcase(char *s)
   {size_t i, nc;
        
    nc = strlen(s);
    for (i = 0; i < nc; i++)
        s[i] = tolower((int) s[i]);

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FILL_STRING - fill ragged S name with "." to column N */

char *fill_string(char *s, int n)
   {int nc, nd;
    static char fill[MAXLINE];

    nstrncpy(fill, MAXLINE, s, -1);
    nc = strlen(fill);
    nd = n - nc;

#if 0
    int i;

    nstrcat(fill, MAXLINE, " ");
    for (i = 0; i < nd; i++)
        nstrcat(fill, MAXLINE, ".");

#else

    memset(fill+nc, '.', nd);
    fill[nd] = '\0';

#endif

    return(fill);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRCLEAN - make a copy of S which has any non-printing characters
 *          - replaced by sequences of printable characters
 *          - if the line lenght would be exceeded put '...\n'
 *          - in the end of D
 */

char *strclean(char *d, size_t nd, char *s, size_t ns)
   {int i, j;
    unsigned int c;
    char t[10];
        
    if ((d != NULL) && (s != NULL))
       {c  = strlen(s);
	ns = min(ns, c);

	for (i = 0, j = 0; i < ns; i++)
	    {c = s[i];
	     if (c < ' ')
	        {switch (c)
		    {case '\f' :
		     case '\n' :
		     case '\r' :
		     case '\t' :
		          d[j++] = c;
		          break;
		     default :
		          d[j++] = '^';
		          d[j++] = 'A' + c - 1;
		          break;};}
	     else if (c > '~')
	        {d[j++] = '[';
                 snprintf(t, 10, "%03o", c - 128);
		 d[j++] = t[0];
		 d[j++] = t[1];
		 d[j++] = t[2];}
	     else
	        d[j++] = c;

	    if (j + 7 >= nd)
	       {d[j++] = ' ';
		d[j++] = '.';
		d[j++] = '.';
		d[j++] = '.';
		d[j++] = ' ';
	        break;};};

	d[j++] = '\0';};

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRSTRI - finds first occurance of string2 in string1 (case insensitive)
 *         - if not there returns NULL
 */

char *strstri(char *string1, char *string2)
   {char *s1, *s2, *s3;
        
    s1 = string1;
    while (*s1 != '\0')
       {for ((s2 = string2, s3 = s1);
             (toupper((int) *s2) == toupper((int) *s3)) &&
	     (*s3 != '\0') && (*s2 != '\0');
             (s3++, s2++));

/* if s2 makes it to the end the string is found */
        if (*s2 == '\0')
           return(s1);
        else
           s1++;};

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SUBST - substitute N occurences of A with B in S
 *       - like: sed 's/A/B/g'
 *       - GOTCHA: make the internal buffers dynamic
 */

char *subst(char *s, char *a, char *b, size_t n)
   {int i, o;
    char *p, *pr, *ps, *pa, *pb, *r;
    static char bfa[1024*LRG], bfb[1024*LRG];

    if (s == bfa)
       {pa = bfb;
	pb = bfa;}
    else
       {pa = bfa;
	pb = bfb;};

    ps = pa;
	
    if ((s != NULL) && (IS_NULL(a) == FALSE) && (b != NULL))

/* make sure s is not bfa to avoid buffer overlap errors */
       {strcpy(ps, s);

	o = 0;
	for (i = 0; i < n; i++)
	    {p = strstr(ps+o, a);
	     if (p != NULL)
	        {pr = p + strlen(a);
		 *p = '\0';
		 r  = (i & 1) ? pa : pb;
		 strcpy(r, ps);
		 strcat(r, b);
		 o = strlen(r);
		 strcat(r, pr);
		 ps = r;}
	     else
	        break;};};

    return(ps);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CAT - add NCAT lines FNAME to OUT
 *     - skip the first NSKIP lines
 *     - a value of -1 for NCAT becomes INT_MAX which is the whole file
 */

void cat(FILE *out, size_t nskip, size_t ncat, char *fmt, ...)
   {int i;
    char fname[MAXLINE], s[MAXLINE];
    FILE *in;

    VA_START(fmt);
    VSNPRINTF(fname, MAXLINE, fmt);
    VA_END;

    in = fopen(fname, "r");
    if (in != NULL)
       {for (i = 0; fgets(s, MAXLINE, in) != NULL; i++)
            {if ((nskip <= i) && (i < ncat))
	        fputs(s, out);};

	fclose(in);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COPY - copy the file IN to OUT */

void copy(char *out, char *fmt, ...)
   {char in[MAXLINE];
    FILE *fp;

    VA_START(fmt);
    VSNPRINTF(in, MAXLINE, fmt);
    VA_END;

    fp = fopen(out, "w");
    if (fp != NULL)
       {cat(fp, 0, -1, in);
	fclose(fp);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATH_TAIL - do what ':t' does to S */

char *path_tail(char *s)
   {int nc;
    char *pd;
    static char d[LRG];

    nstrncpy(d, LRG, s, -1);
    nc = strlen(d);
    if (nc >= LRG)
       nc = LRG;
    if (nc < 1)
       nc = 1;

    for (pd = d + nc-1; pd >= d; pd--)
        {if (*pd == '/')
	    break;};

    pd++;

    return(pd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATH_HEAD - do what ':h' does to S */

char *path_head(char *s)
   {int nc;
    char *pd;
    static char d[LRG];

    nstrncpy(d, LRG, s, -1);
    nc = strlen(d);
    if (nc >= LRG)
       nc = LRG;
    if (nc < 1)
       nc = 1;

    for (pd = d + nc-1; pd >= d; pd--)
        {if (*pd == '/')
	    {*pd = '\0';
	     break;};};

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATH_BASE - do what ':r' does to S */

char *path_base(char *s)
   {int nc;
    char *pd;
    static char d[LRG];

    nstrncpy(d, LRG, s, -1);
    nc = strlen(d);
    if (nc >= LRG)
       nc = LRG;
    if (nc < 1)
       nc = 1;

    for (pd = d + nc-1; pd >= d; pd--)
        {if (*pd == '.')
	    {*pd = '\0';
	     break;};};

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATH_SUFFIX - do what :e does and return the suffix of S */

char *path_suffix(char *s)
   {int nc;
    char *pd;
    static char d[LRG];

    nstrncpy(d, LRG, s, -1);
    nc = strlen(d);
    if (nc >= LRG)
       nc = LRG;
    if (nc < 1)
       nc = 1;

    for (pd = d + nc-1; pd >= d; pd--)
        {if (*pd == '.')
	    break;};

    pd++;

    return(pd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FULL_PATH - make a full path out of NAME
 *           - without reference to the PATH environment variable
 */

int full_path(char *path, int nc, char *dir, char *name)
   {int rv;
    char d[LRG], s[LRG];
    char *pn;

    rv = TRUE;

    if (name[0] == '/')
       nstrncpy(s, nc, name, -1);

    else if (dir != NULL)
       snprintf(s, nc, "%s/%s", dir, name);
      
    else
       {getcwd(d, LRG);
        for (pn = name; TRUE; )
	    {if (strncmp(pn, "../", 3) == 0)
	        {pn += 3;
		 nstrncpy(d, LRG, path_head(d), -1);}
	     else if (strncmp(pn, "./", 2) == 0)
	        pn += 2;
	     else
	        break;};
	snprintf(s, nc, "%s/%s", d, pn);};

    nstrncpy(path, nc-1, s, -1);
/*    path[nc-1] = '\0'; */

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATH_SIMPLIFY - remove redundant entries from path type string S */

char *path_simplify(char *s, int delim)
   {int i, j, ok;
    char *t, **sa;
    static char d[LRG];

    d[0] = '\0';

    sa = tokenize(s, " :");
    
    if (sa != NULL)
       {nstrncpy(d, LRG, sa[0], -1);
	for (i = 1; sa[i] != NULL; i++)
	    {t = sa[i];

	     ok = FALSE;
	     for (j = i-1; (j >= 0) && (ok == FALSE); j--)
	         ok = (strcmp(sa[j], t) == 0);

	     if (ok == FALSE)
	        vstrcat(d, LRG, "%c%s", delim, t);};

	free_strings(sa);};

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DIR_EXISTS - return TRUE iff the named directory exists */

int dir_exists(char *fmt, ...)
   {int rv;
    char s[MAXLINE];
    struct stat sb;

    VA_START(fmt);
    VSNPRINTF(s, MAXLINE, fmt);
    VA_END;

    rv = TRUE;

    if (stat(s, &sb) != 0)
       rv = FALSE;

    else if (!(sb.st_mode & S_IFDIR))
       rv = FALSE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FILE_EXISTS - return TRUE iff the named file exists */

int file_exists(char *fmt, ...)
   {int rv;
    char s[MAXLINE];
    struct stat sb;

    VA_START(fmt);
    VSNPRINTF(s, MAXLINE, fmt);
    VA_END;

    rv = FALSE;

    if (stat(s, &sb) == 0)

/* regular file */
       {if (sb.st_mode & S_IFREG)
	   rv = TRUE;

/* character device */
        else if (sb.st_mode & S_IFCHR)
	   rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FILE_EXECUTABLE - return TRUE iff the named file is executable */

#undef NGROUPX
#ifdef NGROUPS_MAX
# define NGROUPX NGROUPS_MAX
#else
# define NGROUPX 16
#endif

int file_executable(char *fmt, ...)
   {int rv, st, muid, mgid, fuid, fgid, only;
    int usrx, grpx, othx, isusr, isgrp, isoth, file;
    char s[MAXLINE], err[MAXLINE];
    struct stat bf;

    VA_START(fmt);
    VSNPRINTF(s, MAXLINE, fmt);
    VA_END;

    only = FALSE;

    muid = getuid();
    if (only == TRUE)
       mgid = getgid();

    rv = FALSE;
    st = stat(s, &bf);
    if (st != 0)
       nstrncpy(err, MAXLINE, strerror(errno), -1);
    else
       {int ig, ng;
	gid_t gl[NGROUPX+1];

	fuid = bf.st_uid;
	fgid = bf.st_gid;
	file = bf.st_mode & S_IFREG;

	isusr = ((bf.st_mode & S_IXUSR) != 0);
	isgrp = ((bf.st_mode & S_IXGRP) != 0);
	isoth = ((bf.st_mode & S_IXOTH) != 0);

/* determine whether we have user permissions */
	usrx = ((muid == fuid) && isusr);

/* determine whether we have group permissions */
	if (only)
	   grpx = (mgid == fgid);
	else
	   {ng   = getgroups(NGROUPX, gl);
	    grpx = 0;
	    for (ig = 0; (grpx == 0) && (ig < ng); ig++)
	        grpx |= (gl[ig] == fgid);};
	grpx &= isgrp;

/* determine whether we have world permissions */
	othx = isoth;

	if (file && (usrx || grpx || othx))
           rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FILE_SCRIPT - return TRUE iff the named file is an executable script */

int file_script(char *fmt, ...)
   {int rv;
    char s[MAXLINE], t[MAXLINE];
    FILE *fp;

    VA_START(fmt);
    VSNPRINTF(s, MAXLINE, fmt);
    VA_END;

    rv = file_executable(s);
    if (rv == TRUE)
       {fp = fopen(s, "r");
	if (fp != NULL)
	   {if (fgets(t, MAXLINE, fp) != NULL)
	       rv = (strncmp(t, "#!", 2) == 0);
	    fclose(fp);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXECUTE - execute a command and return the exit status */

int execute(int err, char *fmt, ...)
   {int rv;
    char s[LRG], cmd[LRG];

    VA_START(fmt);
    VSNPRINTF(s, LRG, fmt);
    VA_END;

    if (err == TRUE)
       snprintf(cmd, LRG, "PATH=.:%s:%s ; %s",
		epath, lpath, s);
    else
       snprintf(cmd, LRG, "PATH=.:%s:%s ; %s 2> /dev/null",
		epath, lpath, s);

    rv = system(cmd);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RUN - execute a command and return the output in a string
 *     - echo the command to the log iff ECHO is TRUE
 */

char *run(int echo, char *fmt, ...)
   {char s[LRG], cmd[LRG];
    char *p;
    FILE *fp;
    static char bf[100*LRG];

    VA_START(fmt);
    VSNPRINTF(s, LRG, fmt);
    VA_END;

    if ((Log != NULL) && (echo & CMD_LINE))
       fprintf(Log, "Command: %s\n", s);

    snprintf(cmd, LRG, "PATH=.:%s:%s ; %s", epath, lpath, s);

    bf[0] = '\0';

    fp = popen(cmd, "r");
    if (fp != NULL)
       {while (TRUE)
	   {p = fgets(s, MAXLINE, fp);
	    if (p == NULL)
	       break;
	    nstrcat(bf, 100*LRG, s);};

	pclose(fp);};

    LAST_CHAR(bf) = '\0';

    if ((Log != NULL) && (echo & CMD_OUT))
       fprintf(Log, "%s\n", bf);

    return(bf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GREP - search FP for S
 *      - S is a simple string (no regular expressions)
 *      - assume FP is an open file an position the pointer
 *      - at the end when finished
 *      - GOTCHA: make the internal buffer dynamic
 */

char *grep(FILE *fp, char *name, char *fmt, ...)
   {int i, clf, err;
    char t[MAXLINE], s[MAXLINE];
    char *p;
    static char r[1024*LRG];

    r[0] = '\0';

    VA_START(fmt);
    VSNPRINTF(s, MAXLINE, fmt);
    VA_END;

    if (fp == NULL)
       {clf = TRUE;
	fp  = fopen(name, "r");}
    else
       clf = FALSE;

    if (fp != NULL)
       {fseek(fp, 0, SEEK_SET);

	for (i = 0; TRUE; i++)
	    {p = fgets(t, MAXLINE, fp);
	     if (p == NULL)
	        break;

	     p = strstr(t, s);
	     if (p != NULL)
	        strcat(r, t);};

	if (clf == TRUE)
	   err = fclose(fp);
	else
	   err = fseek(fp, 0, SEEK_END);

	ASSERT(err == 0);};

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GET_DATE - return the date */

char *get_date(void)
   {time_t t;
    char *p;
    static char s[MAXLINE];

    p = NULL;
    t = time(NULL);
    if (t > 0)
       {strcpy(s, ctime(&t));
	LAST_CHAR(s) = '\0';
	p = s;};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NSLEEP - millisecond sleep */

void nsleep(int ms)
   {

    poll(NULL, 0, ms);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SEPARATOR - write a line of dashes to FP */

void separator(FILE *fp)
   {

    if (fp != NULL)
       fprintf(fp, "--------------------------------------------------------------------------\n\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NOTE - write to FP only */

void note(FILE *fp, int nl, char *fmt, ...)
   {char bf[MEGA];

    if (fp != NULL)
       {VA_START(fmt);
	VSNPRINTF(bf, MEGA, fmt);
	VA_END;

	fputs(bf, fp);
	if (nl == TRUE)
	   fprintf(fp, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NOTED - write to FP and to TTY */

void noted(FILE *fp, char *fmt, ...)
   {char bf[MEGA];

    VA_START(fmt);
    VSNPRINTF(bf, MEGA, fmt);
    VA_END;

    if (fp != NULL)
       {fputs(bf, fp);
	fprintf(fp, "\n");};

    puts(bf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEXT - do the required text printing with \n and \t handled
 *            - properly (some csh's print them literally instead of the
 *            - C way)
 */

void print_text(FILE *fp, char *fmt, ...)
   {char bf[LRG];

    VA_START(fmt);
    VSNPRINTF(bf, LRG, fmt);
    VA_END;

    fprintf(fp, "%s\n", bf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEAN_SPACE - replace '+sp+' with ' '
 *             - this has been used in connection with shell scripts
 */

void clean_space(char *s)
   {char t[LRG];
    char *p, *ps;

    t[0] = '\0';

    for (ps = s; *ps != '\0'; )
        {p = strstr(ps, "+sp+");
	 if (p != NULL)
	    {*p = '\0';
	     vstrcat(t, LRG, "%s ", ps);
	     ps = p + 4;}
	 else
	    {nstrcat(t, LRG, ps);
	     ps += strlen(ps);};};

    strcpy(s, t);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRIP_QUOTE - strip off quotes around T
 *             - also any leading whitespace before the quotes
 */

char *strip_quote(char *t)
   {int n;
    static char bf[LRG];

    n = strspn(t, " \t");
    nstrncpy(bf, LRG, t+n, -1);
    if (bf[0] == '"')
       {for (t = &LAST_CHAR(bf); (*t =='"') && (t != bf); *t-- = '\0');
	t = bf + strspn(bf, "\"");};

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CSETENV - set the environment variable VAR to VAL */

int csetenv(char *var, char *fmt, ...)
   {int err, nc;
    char s[LRG];
    char *t;

    VA_START(fmt);
    VSNPRINTF(s, LRG, fmt);
    VA_END;

    err = 0;
    nc  = strlen(var) + strlen(s) + 2;
    t   = malloc(nc);
    if (t != NULL)
       {snprintf(t, nc, "%s=%s", var, s);
	err = putenv(t);

	note(Log, TRUE, "setenv %s %s", var, s);};

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CUNSETENV - unset the environment variable VAR */

int cunsetenv(char *var)
   {int err;

#if 0
    int nc;
    char *t;

    err = 0;
    nc  = strlen(var) + 12;
    t   = malloc(nc);
    if (t != NULL)
       {snprintf(t, nc, "%s=%s", var, UNDEFINED);
	err = putenv(t);

	note(Log, TRUE, "unsetenv %s", var);};
#else
    err = unsetenv(var);
#endif

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CNOVAL - return a non-NULL, empty string which uniquely identifies
 *        - a non-existent variable value
 */

char *cnoval(void)
   {static char none[1] = "";

    return(none);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CGETENV - get the value of environment variable VAR
 *         - if LIT is TRUE return as is otherwise strip off
 *         - surrounding quotes
 */

char *cgetenv(int lit, char *fmt, ...)
   {char var[MAXLINE];
    char *t;

    VA_START(fmt);
    VSNPRINTF(var, MAXLINE, fmt);
    VA_END;

    t = getenv(var);
    if ((t == NULL) || (strcmp(t, UNDEFINED) == 0))
       t = cnoval();

    else if (lit == FALSE)
       t = strip_quote(t);

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CDEFENV - return TRUE iff the environment variable VAR is defined */

int cdefenv(char *fmt, ...)
   {int rv;
    char var[MAXLINE];
    char *t;

    VA_START(fmt);
    VSNPRINTF(var, MAXLINE, fmt);
    VA_END;

    t  = getenv(var);
    rv = ((t != NULL) && (strcmp(t, UNDEFINED) != 0));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CENV - return a sorted list of environment variables
 *      - exclude variables named in REJ
 */

char **cenv(int sort, char **rej)
   {int i, n;
    char t[LRG];
    char *p, *s, *bf, **ta;

    bf = run(FALSE, "env");
    ta = tokenize(bf, "\n");
    if (ta != NULL)
       {n = lst_length(ta);

/* eliminate the rejected variable entries */
        if (rej != NULL)
	   {int j, nc;

	    for (i = 0; i < n; i++)
	        {for (j = 0; rej[j] != NULL; j++)
		     {nc = strlen(rej[j]);

/* if it matches, free the existing entry, move the last one
 * into this slot and decrement both n and i
 * because this new entry will have to be evaluated
 */
		      if ((strncmp(ta[i], rej[j], nc) == 0) &&
			  (ta[i][nc] == '='))
			 {FREE(ta[i]);
			  ta[i--] = ta[--n];
			  ta[n]   = NULL;
			  break;};};};};
	        

/* fix BackSlash because the escaped newline is a problem */
	for (i = 0; i < n; i++)
	    {if (strncmp(ta[i], "BackSlash=", 10) == 0)
	        {p = strchr(ta[i], '\n');
		 if (p != NULL)
		    {*p++ = '\0';
		     s = STRSAVE(ta[i]);
		     FREE(ta[i]);
		     ta[i] = STRSAVE(p);
		     ta[n++] = s;};
		 break;};};

/* fix PATH because RUN must change the PATH to work */
	for (i = 0; i < n; i++)
	    {if (strncmp(ta[i], "PATH=", 5) == 0)
	        {snprintf(t, LRG, "PATH=%s", cgetenv(FALSE, "PATH"));
		 FREE(ta[i]);
		 ta[i] = STRSAVE(t);
		 break;};};

	if (sort == TRUE)
	   string_sort(ta, n);};

    return(ta);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CMPENV - return a strcmp style value for the
 *        - comparison of the environment variable VAR value and VAL
 */

int cmpenv(char *var, char *val)
   {int rv;
    char *t;

    t = getenv(var);
    if ((t != NULL) && (strcmp(t, UNDEFINED) != 0))
       rv = strcmp(t, val);
    else
       rv = -2;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CINITENV - set the environment variable VAR to VAL
 *          - only if it is undefined
 */

int cinitenv(char *var, char *fmt, ...)
   {int err;
    char s[LRG];

    VA_START(fmt);
    VSNPRINTF(s, LRG, fmt);
    VA_END;

    err = 0;
    if (cdefenv(var) == FALSE)
       err = csetenv(var, s);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CCLEARENV - clear out the environment for safety
 *           - do not remove variables in NULL terminated EXCEPT
 */

int cclearenv(char **except)
   {int i, j, ne, ok, err;
    char s[MAXLINE];
    char *t, **pe, **env;
    extern char **environ;

    err = 0;

/* make a copy of environ because
 * removing elements with unsetenv scrambles environ
 */
    env = NULL;
    for (pe = environ, ne = 0; *pe != NULL; pe++, ne++)
        env = lst_add(env, *pe);
    env = lst_add(env, NULL);

/* traverse env to find names of variables to remove */
    for (i = 0; i < ne; i++)
        {nstrncpy(s, MAXLINE, env[i], -1);
	 t = strchr(s, '=');
	 if (t != NULL)
	    *t = '\0';

	 ok = TRUE;
	 if (except != NULL)
	    {for (j = 0; (except[j] != NULL) && (ok == TRUE); j++)
		 ok = (strcmp(s, except[j]) != 0);};

	 if (ok == TRUE)
	    err |= unsetenv(s);};

    free_strings(env);

/* let us know if there was nothing to do */
    if (i == 0)
       err = -1;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CWHICH - check the path for EXE */

char *cwhich(char *fmt, ...)
   {int ok;
    char prg[MAXLINE], d[LRG];
    char *path;
    static char exe[MAXLINE];

    VA_START(fmt);
    VSNPRINTF(prg, MAXLINE, fmt);
    VA_END;

    if (prg[0] == '/')
       snprintf(exe, MAXLINE, "%s", prg);
       
    else if (strncmp(prg, "./", 2) == 0)
       {getcwd(d, LRG);
	snprintf(exe, MAXLINE, "%s/%s", d, prg+2);}
       
    else if (strncmp(prg, "../", 3) == 0)
       {getcwd(d, LRG);
	snprintf(exe, MAXLINE, "%s/%s", path_head(d), prg+3);}
       
    else
       {ok   = FALSE;
	path = cgetenv(TRUE, "PATH");
	FOREACH(dir, path, " :\n")
	   snprintf(exe, MAXLINE, "%s/%s", dir, prg);
	   if (file_executable(exe) == TRUE)
	      {ok = TRUE;
	       break;};
	ENDFOR;

	if (ok == FALSE)
	   nstrncpy(exe, MAXLINE, "none", -1);};

    return(exe);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* OPEN_FILE - open the specified file */

FILE *open_file(char *mode, char *fmt, ...)
   {char bf[LRG];
    FILE *fp;

    VA_START(fmt);
    VSNPRINTF(bf, LRG, fmt);
    VA_END;

    fp = fopen(bf, mode);

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BLANK_LINE - return TRUE iff the string S represents a blank line */

int blank_line(char *s)
   {int rv;

    rv = ((strspn(s, " \t\f\n") == strlen(s)) || (s[0] == '#'));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PUSH_TOK - push token T onto the end of string S */

int push_tok(char *s, int nc, int dlm, char *fmt, ...)
   {int rv;
    char t[MAXLINE], delim[2];

    VA_START(fmt);
    VSNPRINTF(t, MAXLINE, fmt);
    VA_END;

    rv = TRUE;

    if (IS_NULL(s) == FALSE)
       {delim[0] = dlm;
	delim[1] = '\0';
	nstrcat(s, nc, delim);};

    if (s != NULL)
       nstrcat(s, nc, t);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* APPEND_TOK - append token T onto the end of string S
 *            - this is a dynamic version of push_tok
 */

char *append_tok(char *s, int dlm, char *fmt, ...)
   {int nc;
    char t[MAXLINE], delim[2];
    char *p;

    VA_START(fmt);
    VSNPRINTF(t, MAXLINE, fmt);
    VA_END;

    if (s != NULL)
       {delim[0] = dlm;
	delim[1] = '\0';

	nc = strlen(s) + strlen(t) + strlen(delim) + 10;
	p  = MAKE_N(char, nc);
	if (p != NULL)
	   {nstrncpy(p, nc, s, -1);
	    strcat(p, delim);
	    strcat(p, t);};
	FREE(s);}

    else
       p = STRSAVE(t);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PUSH_TOK_BEG - push token T onto the beginning of string S */

int push_tok_beg(char *s, int nc, int dlm, char *fmt, ...)
   {int rv;
    char t[MAXLINE], bf[LRG];

    VA_START(fmt);
    VSNPRINTF(t, MAXLINE, fmt);
    VA_END;

    rv = TRUE;

/* fill the local buffer BF with the result */
    nstrncpy(bf, LRG, t, -1);
    if (IS_NULL(s) == FALSE)
       vstrcat(bf, LRG, "%c%s", dlm, s);

/* copy BF back to S */
    if (s != NULL)
       nstrncpy(s, nc, bf, -1);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PUSH_DIR - push D onto the directory stack */

int push_dir(char *fmt, ...)
   {int n, rv;
    char d[LRG];

    getcwd(d, LRG);

    n  = dstck.n++;
    dstck.dir[n] = STRSAVE(d);

    VA_START(fmt);
    VSNPRINTF(d, LRG, fmt);
    VA_END;

    rv = chdir(d);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POP_DIR - pop the directory stack */

int pop_dir(void)
   {int n, rv;

    rv = 0;

    n = dstck.n - 1;
    if (n >= 0)
       {rv = chdir(dstck.dir[n]);
	FREE(dstck.dir[n]);};

    dstck.n = max(n, 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POP_PATH - pop the tail of the path off
 *          - return a pointer to the head component
 */

char *pop_path(char *path)
   {char *p;

    p = strrchr(path, '/');
    if (p != NULL)
       *p++ = '\0';
    else
       p = path;

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IS_EXECUTABLE_FILE - if PATH is the name of an executable file
 *                    - and if the length of path is <= NCX
 *                    - copy it into FP and return the length
 */

static int is_executable_file(char *fp, char *path, int ncx)
   {int n, muid, mgid, fuid, fgid;
    int usrx, grpx, othx, file;
    struct stat bf;

    n = -1;

    muid = getuid();
    mgid = getgid();

    n = -1;
    if (stat(path, &bf) == 0)
       {fuid = bf.st_uid;
	fgid = bf.st_gid;
	file = bf.st_mode & S_IFREG;
	usrx = ((muid == fuid) && (bf.st_mode & S_IXUSR));
	grpx = ((mgid == fgid) && (bf.st_mode & S_IXGRP));
	othx = (bf.st_mode & S_IXOTH);
	if (file && (usrx || grpx || othx))
	   {n = strlen(path);
	    if (n <= ncx)
	       {strcpy(fp, path);
		n = 0;};};}

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HANDLE_PATH_DOT - given a directory DIR and path NAME
 *                 - resolve out ./ and ../ elements of NAME
 *                 - return result in PATH
 *                 - DIR may be destroyed in this process
 */

static void handle_path_dot(char *path, char *dir, char *name)
   {char *s;

    if (dir != NULL)
       {s = name;

/* loop over any number of ./ or ../ elements of the path */
	while (s[0] == '.')
	   {if (s[1] == '.')
	       {s += 3;
		pop_path(dir);}
	    else
	       s += 2;};

/* construct the candidate path */
	sprintf(path, "%s/%s", dir, s);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FILE_PATH - given the name of a file return the full path
 *           - with reference to the PATH environment variable
 *           - expand out '.' and '..' constructions
 *           - input is the NAME, output is the PATH which is
 *           - at most NC chars long
 *           - return:
 *           -    0 on success
 *           -    n NAME is there but NC is too few characters
 *           -      n is the number of characters needed to contain
 *           -      the full path
 */

int file_path(char *name, char *path, int nc)
   {int n;
    size_t nb;
    char pathvar[MAXLINE], fp[MAXLINE];
    char *t, *p;

    n  = -1;
    nb = MAXLINE - 1;
    switch (name[0])
       {case '/' :
	     n = is_executable_file(path, name, nc);
             break;

        case '.' :
             t = getcwd(pathvar, nb);
             handle_path_dot(fp, t, name);
	     n = is_executable_file(path, fp, nc);
             break;

        default:
	     p = getenv("PATH");
	     if (p != NULL)
	        {strncpy(pathvar, p, MAXLINE);
		 pathvar[MAXLINE-1] = '\0';

		 for (t = strtok(pathvar, ":");
		      t != NULL;
		      t = strtok(NULL, ":"))
		     {handle_path_dot(fp, t, name);
		      n = is_executable_file(path, fp, nc);
		      if (n == 0)
			 break;};};
             break;};

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LS - return an array of file names
 *    - the last entry is NULL and marks the end of the list
 */

char **ls(char *opt, char *fmt, ...)
   {char s[MAXLINE];
    static char **lst;

    VA_START(fmt);
    VSNPRINTF(s, MAXLINE, fmt);
    VA_END;

    if (opt == NULL)
       opt = "";

    lst = NULL;
    FOREACH(ph, run(FALSE, "ls %s %s 2>&1", opt, s), " \n")
       if (file_exists(ph) == TRUE)
	  lst = lst_add(lst, ph);
    ENDFOR;
    lst = lst_add(lst, NULL);

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MATCH - match the first string arg against the
 *       - regular expression defined by the second string arg
 *       - return -1 if it is lexically less than the pattern
 *       - return  0 if it matches the pattern
 *       - return  1 if it is lexically greater than the pattern
 *       -
 *       - regular expression specifiers (so far)
 *       -
 *       -   '*' matches any number of characters
 *       -   '?' matches any single character
 *       -
 *       - examples:
 *       -    3.2 < 4.*      =>  -1
 *       -    4.2 = 4.*      =>   0
 *       -    5.2 > 4.*      =>   1
 */

#define END_CHECK(ps, pp)                                                    \
   {if ((*ps == '\0') && (*pp != '\0') && (*pp != '*'))                      \
       return(-1);                                                           \
    else if ((*ps != '\0') && (*pp == '\0'))                                 \
       return(1);}

int match(char *s, char *patt)
   {int b, c, rv;
    char *ps, *pp;

    ps = s;
    pp = patt;
	
    if (pp == NULL)
       rv = 0;
    else if (ps == NULL)
       rv = -1;
    else if ((*ps == '\0') && (*pp == '\0'))
       rv = 0;
    else
       {rv = -1;
	while ((c = *pp++) != '\0')
	   {switch (c)
	       {case '*' :
		     while (*pp == '*')
		        pp++;
		     b = 0;
		     c = *pp;
		     while ((ps = strchr(ps, c)) != NULL)
		        {if (match(ps, pp) == 0)
			    return(0);
			 END_CHECK(ps, pp);
			 if ((*ps == '\0') && (*pp == '\0'))
			    break;
			 else
			    b = *(++ps);};
		     c = *(++pp);
		     if (b < c)
		        rv = -1;
		     else
		        rv = 1;
		     return(rv);

		case '?' :
		     rv = match(++ps, pp);
		     return(rv);

		case '\\' :
		     c = *pp++;

		default :
		     b = *ps++;
		     if (b < c)
		        return(-1);
		     else if (b > c)
		        return(1);};

	    END_CHECK(ps, pp);};

	if (*ps == '\0')
	   rv = -1;};

    return(rv);}

#undef END_CHECK

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UNIQUE - remove duplicate tokens from a list */

char *unique(char *lst, int beg, int dlm)
   {int ok;
    char olst[LRG], delim[10];
    char *item, *p;
    static char nlst[LRG];

    nlst[0] = '\0';

    if (lst != NULL)
       {nstrncpy(olst, LRG, lst, -1);

	delim[0] = dlm;
	delim[1] = '\0';

	for (item = olst; item != NULL; item = p)
	    {p = strchr(item, dlm);
	     if (p == NULL)
	        {if (IS_NULL(item) == TRUE)
		    break;}
	     else
	        *p++ = '\0';

	     ok = TRUE;
	     FOREACH(j, nlst, delim)
	        if (strcmp(j, item) == 0)
		   {ok = FALSE;
		    break;}
	     ENDFOR
	     if (ok == TRUE)
	        {if (beg == TRUE)
		    push_tok_beg(nlst, LRG, dlm, item);
		 else
		    push_tok(nlst, LRG, dlm, item);};};};

    return(nlst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SPLICE_OUT_PATH - look thru PATH and remove any
 *                 - occurences of $PATH or ${PATH}
 *                 - while we are at it remove duplicates
 *                 - return the result in PATH
 */

void splice_out_path(char *path)
   {char bf[MAXLINE], lpth[MAXLINE];
    char *ps, *t, *ts, *te;

    nstrncpy(lpth, MAXLINE, path, -1);

/* check for $PATH */
    ps = lpth + strspn(lpth, " \t");
    t  = strstr(ps, "$PATH");
    if (t != NULL)
       {ts = t;
	te = ts + 5;}
    else
       {t  = strstr(ps, "${PATH}");
	ts = (t == NULL) ? NULL : t;
	te = ts + 7;};
            
/* splice out $PATH */
    if (ts != NULL)
       {bf[0] = '\0';

	if (ts != ps)
	   {*(ts-1) = '\0';
	    nstrncpy(bf, MAXLINE, ps, -1);}

	else if (*te == ':')
	   te++;

	if (*te != '\0')
	   nstrcat(bf, MAXLINE, te);

	ps = bf;};

    strcpy(path, unique(ps, FALSE, ':'));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PUSH_PATH - look thru PATH for things to add to destination DPATH */

void push_path(int end, char *dpath, char *path)
   {char lpth[LRG], tp[LRG];

    if (IS_NULL(path) == FALSE)
       {nstrncpy(lpth, LRG, strip_quote(path), -1);

	splice_out_path(lpth);

/* add the new item */
	if (IS_NULL(dpath) == TRUE)
	   nstrncpy(tp, LRG, lpth, -1);

	else
	   {if (end == APPEND)
	       snprintf(tp, LRG, "%s:%s", dpath, lpth);
	    else if (end == PREPEND)
	       snprintf(tp, LRG, "%s:%s", lpth, dpath);};

	nstrncpy(dpath, LRG, path_simplify(tp, ':'), -1);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BUILD_PATH - build up lpath to include all specified programs
 *            - BASE is a string containing colon separated
 *            - directories ala PATH
 */

void build_path(char *base, ...)
   {char exe[MAXLINE], pth[LRG], os[MAXLINE];
    char *s, *t;

    unamef(os, MAXLINE, "s");

    if (strcmp(os, "AIX") == 0)
       snprintf(pth, LRG,
		"/bin:/usr/bin:/sw/bin:/sbin:/usr/sbin:/usr/local/bin:");
    else
       snprintf(pth, LRG,
		"/bin:/usr/bin:/sw/bin:/sbin:/usr/sbin:/usr/xpg4/bin:");

    if (base == NULL)
       nstrcat(pth, LRG, cgetenv(TRUE, "PATH"));
    else
       nstrcat(pth, LRG, base);

    VA_START(base);

    while (TRUE)
       {s = VA_ARG(char *);
	if (s == NULL)
	   break;

	t = NULL;

	FOREACH(dir, pth, ":")
	   snprintf(exe, MAXLINE, "%s/%s", dir, s);
	   if (file_executable(exe) == TRUE)
	      {t = STRSAVE(exe);
	       push_path(PREPEND, lpath, dir);
	       break;};
	ENDFOR;

	if (t == NULL)
	   {printf("\nError: cannot find %s - exiting\n\n", s);
	    exit(1);}
	else
	   free(t);};

    VA_END;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TOUCH - create an empty file */

int touch(char *fmt, ...)
   {int fd;
    char path[LRG];

    VA_START(fmt);
    VSNPRINTF(path, LRG, fmt);
    VA_END;

    fd = creat(path, 0660);
    if (fd > 0)
       close(fd);

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TRIM - trim any characters contained in DELIM from the front or back
 *      - of the string S
 */

char *trim(char *s, int dir, char *delim)
   {int ic, nc, c;

    if (s != NULL)
       {if ((dir & FRONT) != 0)
	   for (; (*s != '\0') && (strchr(delim, *s) != NULL); s++);

/* do nothing with zero length strings */
	if ((dir & BACK) != 0)
	   {nc = strlen(s);
	    for (ic = nc - 1; ic >= 0; ic--)
	        {c = s[ic];
		 if (strchr(delim, c) == NULL)
		    break;
		 s[ic] = '\0';};};};

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* KEY_VAL - parse out a key/value pair from S using DLM delimiters
 *         - avoid use of strtok so as not to conflict with FOREACH
 */

void key_val(char **key, char **val, char *s, char *dlm)
   {char *k, *p, *v;

    k = NULL;
    v = NULL;
    if (s != NULL)
       {k = trim(s, FRONT, dlm);
	p = strpbrk(k, dlm);
	if (p != NULL)
	   {*p++ = '\0';

	    v = trim(p, BOTH, dlm);
/*
	    v = trim(p, FRONT, dlm);
	    p = strchr(v, '\n');
	    if (p != NULL)
	       *p++ = '\0';
 */
	   };};

    if (key != NULL)
       *key = k;
    if (val != NULL)
       *val = v;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DELIMITED - return substring of S between matched delimiters BGN and END
 *           - example:
 *           -   "aa (foo(bar)) zz"  with  "(" and ")"
 *           - should return "foo(bar)"
 *           - NOTE: S will be terminated at the first delimiter instance
 */

char *delimited(char *s, char *bgn, char *end)
   {int nc, ncb, nce;
    char *tb, *te, *val, *ps, *wh;

    tb    = s + strcspn(s, bgn);
    *tb++ = '\0';

    val   = tb;
/*
    tb    = val + strcspn(val, end);
    *tb++ = '\0';
*/
    nc  = 1;
    ncb = strlen(bgn);
    nce = strlen(end);

    for (ps = tb; IS_NULL(ps) == FALSE; )
        {tb = strstr(ps, bgn);
	 te = strstr(ps, end);
	 if ((tb != NULL) && (tb[ncb] != '\'') &&
	     (te != NULL) && (te[nce] != '\''))
	    wh = (tb < te) ? tb : te;
	 else if ((tb != NULL) && (tb[ncb] != '\''))
	    wh = tb;
	 else if ((te != NULL) && (te[nce] != '\''))
	    wh = te;
	 else
	    wh = NULL;

/* no instance of BGN or END */
	 if (wh == NULL)
	    {if (nc == 0)
	        nstrcat(s, LRG, ps);
	     ps = NULL;}

/* if an instance of BGN came first */
	 else if (wh == tb)
	    {nc++;
	     if (nc == 1)
	        val = tb;
	     ps = tb + ncb;}

/* if an instance of END came first */
	 else if (wh == te)
	    {nc--;
	     if (nc == 0)
	        {*te = '\0';
		 ps  = NULL;}
	     else
	        ps = te + nce;};};

    return(val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BLOCK_FD - set the file descriptor to be blocked in ON is TRUE
 *          - otherwise unblocked
 */

int block_fd(int fd, int on)
   {int status;

    status = 0;

    status = fcntl(fd, F_GETFL, status);

/* block */
    if (on == TRUE)
       status = fcntl(fd, F_SETFL, status & ~O_NDELAY);

/* unblock */
    else
       status = fcntl(fd, F_SETFL, status | O_NDELAY);

    if (status == -1)
       status = FALSE;
    else
       status = TRUE;

    return(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FWRITE_SAFE - make fwrite ensure that the requested number of bytes
 *             - is written if at all possible
 *             - return number of bytes written or -1
 */

size_t fwrite_safe(void *s, size_t bpi, size_t nitems, FILE *fp)
   {size_t zc, n, ns, nw;
    char *ps;

    zc = 0;
    ns = nitems;
    nw = 0;
    ps = (char *) s;
    while ((ns > 0) && (zc < 10))
       {n = fwrite(ps, bpi, ns, fp);

	zc = (n == 0) ? zc + 1 : 0;

        if (n < ns)
           fflush(fp);

	ps += bpi*n;
	ns -= n;
        nw += n;};

    return(nw);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LOG_ACTIVITY - log messages to FLOG */

void log_activity(char *flog, int ilog, char *oper, char *fmt, ...)
   {char msg[MAXLINE];
    FILE *log;

    if ((ilog == TRUE) && (flog != NULL))
       {log = fopen(flog, "a");
	if (log != NULL)
	   {VA_START(fmt);
	    VSNPRINTF(msg, MAXLINE, fmt);
	    VA_END;
	    fprintf(log, "%s\t(%d)\t: %s\n", oper, (int) getpid(), msg);
	    fclose(log);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UNAMEF - function to do what uname utility does
 *        - return info in S which is NC long
 *        - WH is one of "s", "n", "m", "r", and "v" corresponding to
 *        - the uname command line args
 */

void unamef(char *s, int nc, char *wh)
   {static int first = TRUE;
    static struct utsname uts;

    if (first == TRUE)
       {first = FALSE;
	uname(&uts);};

    s[0] = '\0';

    switch (*wh)
       {case 'm' :
             nstrncpy(s, nc, uts.machine, -1);
             break;
        case 'n' :
	     nstrncpy(s, nc, uts.nodename, -1);
             break;
        case 'r' :
	     nstrncpy(s, nc, uts.release, -1);
             break;
        case 's' :
	     nstrncpy(s, nc, uts.sysname, -1);
             break;
        case 'v' :
	     nstrncpy(s, nc, uts.version, -1);
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DEMONIZE - turn the running job into a daemon */

int demonize(void)
   {int st, rv, fd;

    rv = FALSE;

/* get into the background by running as a child
 * the parent will return and exit
 */
    st = fork();
    if (st == 0)

/* close stdin, stdout, and stderr */
       {close(0);
	close(1);
	close(2);

/* disassociate from the controlling terminal */
	st = setsid();

/* prevent possibility of reacquiring a controlling terminal
 * the parent will return and exit
 * the child will belong to init and
 * will not be process group or session leader
 */
/*
	st = fork();
	if (st != 0)
*/
	   rv = TRUE;

/* go to direct attached directory so that mounts won't
 * be compromised (i.e. hung)
 * better be using absolute paths from here on out
 */
	chdir("/");

/* whack the umask */
	umask(0);

/* attach stdin, stdout, and stderr to /dev/null */
	fd = open("/dev/null", O_RDWR);        /* stdin */
	dup(fd);                               /* stdout */
	dup(fd);};                             /* stderr */

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FILE_STRINGS - return the text of FP as a NULL terminated list of strings */

char **file_strings(FILE *fp)
   {int i;
    char s[LRG];
    char **sa;

    sa = NULL;
    if (fp != NULL)
       {for (i = 0; fgets(s, LRG, fp) != NULL; i++)
	    {if (LAST_CHAR(s) == '\n')
	        LAST_CHAR(s) = '\0';
	     sa = lst_add(sa, s);};

	sa = lst_add(sa, NULL);};

    return(sa);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FILE_TEXT - return the text of FNAME as a NULL terminated list of strings
 *           - if SORT is TRUE sort the file first
 */

char **file_text(int sort, char *fname, ...)
   {char file[MAXLINE];
    char **sa;
    FILE *in;

    VA_START(fname);
    VSNPRINTF(file, MAXLINE, fname);
    VA_END;

    if (sort == TRUE)
       {run(FALSE, "rm -f %s.srt ; sort %s > %s.srt", file, file, file);
	nstrcat(file, MAXLINE, ".srt");};

    sa = NULL;
	
    in = fopen(file, "r");
    if (in != NULL)
       {sa = file_strings(in);
	fclose(in);};

    if (sort == TRUE)
       run(FALSE, "rm -f %s", file);

    return(sa);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IS_RUNNING - return the TRUE iff PID is running process */

int is_running(int pid)
   {int st, ok, err;

    ok  = TRUE;
    err = 0;
    st  = kill(pid, 0);
    if (st == -1)
       {err = errno;
	if (err == EPERM)
	   ok = TRUE;
	else if (err == ESRCH)
	   ok = FALSE;};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
