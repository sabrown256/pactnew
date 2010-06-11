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

/* FREE_STRINGS - free the strings in the array lst */

void free_strings(char **lst)
   {int i;

    if (lst != NULL)
       {for (i = 0; lst[i] != NULL; i++)
	    {FREE(lst[i]);};};

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

/* NSTRCAT - safe strcat function */

char *nstrcat(char *d, int nc, char *s)
   {int n;

    n = nc - 1 - strlen(d);
    n = min(n, strlen(s));

    strncat(d, s, n);

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NSTRNCPY - this is to strncpy as snprintf is to sprintf
 *          - a safe string copy function
 *          - unlike strncpy this always returns a null terminated string
 */

char *nstrncpy(char *d, size_t nd, char *s, size_t ns)
   {size_t nc;
        
    nc = min(ns, nd-1);
    if (s != NULL)
       {strncpy(d, s, nc);
	d[nc] = '\0';};

    return(d);}

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
    static char bfa[1024*LRG], bfb[1024*LRG];
    char *p, *pr, *ps, *pa, *pb, *r;

/* make sure s is not bfa to avoid buffer overlap errors */
    if (s == bfa)
       {pa = bfb;
	pb = bfa;}
    else
       {pa = bfa;
	pb = bfb;};

    ps = pa;
    strcpy(ps, s);

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
	    break;};

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

    strcpy(d, s);
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

    strcpy(d, s);
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

/* FULL_PATH - make a full path out of NAME */

int full_path(char *path, int nc, char *dir, char *name)
   {int rv;
    char d[LRG], s[LRG];
    char *pn;

    rv = TRUE;

    if (name[0] == '/')
       strncpy(s, name, nc);

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

    strncpy(path, s, nc-1);
    path[nc-1] = '\0';

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PATH_BASE - do what ':r' does to S */

char *path_base(char *s)
   {int nc;
    char *pd;
    static char d[LRG];

    strcpy(d, s);
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

#ifndef NGROUPS_MAX
# define NGROUPS_MAX 16
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
       strncpy(err, strerror(errno), MAXLINE);
    else
       {int ig, ng;
	gid_t gl[NGROUPS_MAX+1];

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
	   {ng   = getgroups(NGROUPS_MAX, gl);
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
	if (fgets(t, MAXLINE, fp) != NULL)
	   rv = (strncmp(t, "#!", 2) == 0);
	fclose(fp);};

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
	   err = fseek(fp, 0, SEEK_END);};

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LS - return an array of file names
 *    - the last entry is NULL and marks the end of the list
 */

char **ls(char *opt, char *fmt, ...)
   {int i;
    char s[MAXLINE];
    static char *lst[LRG];

    VA_START(fmt);
    VSNPRINTF(s, MAXLINE, fmt);
    VA_END;

    i = 0;
    FOREACH(ph, run(FALSE, "ls %s %s 2>&1", opt, s), " \n")
       if (file_exists(ph) == TRUE)
	  lst[i++] = STRSAVE(ph);
    ENDFOR;
    lst[i++] = NULL;

    return(lst);}

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
	     nstrcat(t, LRG, ps);
	     nstrcat(t, LRG, " ");
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
    strncpy(bf, t+n, LRG);
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
	   strcpy(exe, "none");};

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

/* TOKENIZE - return an array of strings obtained by
 *          - tokenizing the string S according to the delimiters DELIM
 *          - the array is terminated by a NULL string
 *          - it can be released by free_strings
 */

char **tokenize(char *s, char *delim)
   {int i, n, c;
    char *p, *t, *ps, **sa;

    sa = NULL;
    n  = strlen(s);
    t  = MAKE_N(char, n+100);
    if (t != NULL)
       {strcpy(t, s);

	for (i = 0, ps = t; ps != NULL; )
	    {if (sa == NULL)
	        sa = MAKE_N(char *, 1000);

	     p = strpbrk(ps, delim);
	     if (p != NULL)
	        {c  = *p;
		 *p = '\0';
		 sa[i++] = STRSAVE(ps);
		 *p = c;
		 ps = p + 1;}
	     else
	        {sa[i++] = STRSAVE(ps);
		 break;};};

	if (sa != NULL)
	   sa[i++] = NULL;

	FREE(t);};

    return(sa);}

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

    if (s[0] != '\0')
       {delim[0] = dlm;
	delim[1] = '\0';
	strncat(s, delim, nc);};

    strncat(s, t, nc);

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
	strcpy(p, s);
	strcat(p, delim);
	strcat(p, t);
	FREE(s);}

    else
       p = STRSAVE(t);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PUSH_TOK_BEG - push token T onto the beginning of string S */

int push_tok_beg(char *s, int nc, int dlm, char *fmt, ...)
   {int rv;
    char t[MAXLINE], bf[LRG], delim[2];

    VA_START(fmt);
    VSNPRINTF(t, MAXLINE, fmt);
    VA_END;

    rv = TRUE;

    strncpy(bf, t, LRG);
    if (s[0] != '\0')
       {delim[0] = dlm;
	delim[1] = '\0';
	strncat(bf, delim, nc);
	strncat(bf, s, nc);};

    strncpy(s, bf, nc);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LST_LENGTH - return the length of the list */

int lst_length(char **lst)
   {int n;

    n = 0;
    if (lst != NULL)
       {for (n = 0; TRUE; n++)
            if (lst[n] == NULL)
               break;};

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LST_PUSH - add S to list LST */

char **lst_push(char **lst, char *fmt, ...)
   {int n, m;
    char s[MAXLINE];
    char *ps;

    if (fmt == NULL)
       ps = NULL;
    else
       {VA_START(fmt);
        VSNPRINTF(s, MAXLINE, fmt);
        VA_END;

        ps = STRSAVE(s);};

    if (lst == NULL)
       {lst = MAKE_N(char *, 100);
        memset(lst, 0, 100*sizeof(char *));};

    n = lst_length(lst);
    if (n % 100 == 98)
       {m = n + 102;
        REMAKE(lst, char *, m);
        memset(lst+n, 0, 102*sizeof(char *));};

    lst[n] = ps;

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
        memset(nlst, 0, m*sizeof(char *));

        for (i = 0; i < n; i++)
            nlst[i] = STRSAVE(lst[i]);};

    return(nlst);}

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

/* MATCH - returns TRUE iff the first string arg matched the
 *       - regular expression defined by the second string arg
 *       -
 *       - regular expression specifiers (so far)
 *       -
 *       -   '*' matches any number of characters
 *       -   '?' matches any single character
 */

int match(char *s, char *patt)
   {int c, rv;
    char *ps, *pp;

    if (patt == NULL)
       rv = TRUE;
    else if (s == NULL)
       rv = FALSE;
    else
       {ps = s;
	pp = patt;
	while ((c = *pp++) != '\0')
	   {switch (c)
	       {case '*' :
		     while (*pp == '*')
		        pp++;
		     c = *pp;
		     while ((ps = strchr(ps, c)) != NULL)
		        {if (match(ps, pp))
			    return(TRUE);
			 ps++;};
		     return(FALSE);

		case '?' :
		     rv = match(++ps, pp);
		     return(rv);

		case '\\' :
		     c = *pp++;

		default :
		     if (*ps++ != c)
		        return(FALSE);};

	    if (((*ps == '\0') && (*pp != '\0') && (*pp != '*')) ||
		((*ps != '\0') && (*pp == '\0')))
	       return(FALSE);};

	rv = (*ps == '\0');};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UNIQUE - remove duplicate tokens from a list */

char *unique(char *lst, int beg, int dlm)
   {int ok;
    char olst[LRG], delim[10];
    char *item, *p;
    static char nlst[LRG];

    if (lst != NULL)
       {strcpy(olst, lst);
	nlst[0] = '\0';

	delim[0] = dlm;
	delim[1] = '\0';

	for (item = olst; item != NULL; item = p)
	    {p = strchr(item, dlm);
	     if (p == NULL)
	        {if (item[0] == '\0')
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

    strcpy(lpth, path);

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
	    strncpy(bf, ps, MAXLINE);}

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
   {int i, ok;
    char lpth[MAXLINE], tp[LRG];
    char **lst;

    strcpy(lpth, strip_quote(path));

    splice_out_path(lpth);

/* add the new item */
    if (dpath[0] == '\0')
       strncpy(dpath, lpth, LRG);

    else

/* path to list */
       {lst = tokenize(lpath, ":");

/* check list for match */
	ok = FALSE;
	for (i = 0; (lst[i] != NULL) && (ok == FALSE); i++)
	    {if (strcmp(lst[i], path) == 0)
	        ok = TRUE;
	     FREE(lst[i]);};
        
	FREE(lst);

	if (ok == FALSE)
	   {if (end == APPEND)
	       {snprintf(tp, LRG, "%s:%s", dpath, lpth);
		strncpy(dpath, tp, LRG);}

	    else if (end == PREPEND)
	       {snprintf(tp, LRG, "%s:%s", lpth, dpath);
		strncpy(dpath, tp, LRG);};};};

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

    k = trim(s, FRONT, dlm);
    p = strpbrk(k, dlm);
    if (p != NULL)
       {*p++ = '\0';

	v = trim(p, FRONT, dlm);
	p = strchr(v, '\n');
	if (p != NULL)
	   *p++ = '\0';}
    else
       v = NULL;

    if (key != NULL)
       *key = k;
    if (val != NULL)
       *val = v;

    return;}

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
	VA_START(fmt);
	VSNPRINTF(msg, MAXLINE, fmt);
	VA_END;
	fprintf(log, "%s\t(%d)\t: %s\n", oper, getpid(), msg);
        fclose(log);};

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
             strncpy(s, uts.machine, nc);
             break;
        case 'n' :
	     strncpy(s, uts.nodename, nc);
             break;
        case 'r' :
	     strncpy(s, uts.release, nc);
             break;
        case 's' :
	     strncpy(s, uts.sysname, nc);
             break;
        case 'v' :
	     strncpy(s, uts.version, nc);
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
	setsid();

/* prevent possibility of reacquiring a controlling terminal
 * the parent will return and exit
 * the child will belong to init and
 * will not be process group or session leader
 */
	st = fork();
	if (st != 0)
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

