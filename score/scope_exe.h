/*
 * SCOPE_EXE.H - support executable access
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SCOPE_EXE

# define PCK_SCOPE_EXE

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

#define SC_exe_open(_en, _sn, _tg, _wf, _b, _u, _d)                          \
   ((_SC.exe.open == NULL) ? NULL :                                          \
                             (_SC.exe.open)(_en, _sn, _tg, _wf, _b, _u, _d))

#define SC_exe_close(_st)                                                    \
   {if (_SC.exe.close != NULL) (_SC.exe.close)(_st);}

#define SC_exe_map_addr(_sl, _st, _ad)                                       \
   {if (_SC.exe.map_addr != NULL) (_SC.exe.map_addr)(_sl, _st, _ad);}

#define SC_exe_map_addrs(_st, _na, _ad)                                      \
   ((_SC.exe.map_addrs == NULL) ? FALSE :                                    \
                                 (_SC.exe.map_addrs)(_st, _na, _ad))

/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/

typedef struct s_exedes exedes;
typedef struct s_exe_apides exe_apides;
typedef struct s_SC_storloc SC_storloc;

/* this is like the SC_srcloc but hold storage for the strings
 * most usages can take the SC_srcloc but some demand this variant
 */
struct s_SC_storloc
   {SC_srcloc loc;
    char file[MAXLINE];
    char func[MAXLINE];};

struct s_exedes
  {int unwin;               /* unwind inlined functions */
   int showf;     	    /* show function names */
   int demang;	            /* demangle names */
   int tailf;	            /* print base file names only - no dirs */
   int found;               /* internal use */
   char *ename;             /* executable name */
   char *sname;             /* section name - defaults to NULL */
   char *target;            /* format target */
   int64_t pc;              /* current address */
   SC_storloc where;        /* srcloc of current adderss - internal use */
   void *et;                /* executable file info */
   void *es;                /* section info */
   void **symt;	            /* symbol table */
   char err[MAXLINE];};     /* latest error message */

struct s_exe_apides
   {exedes *(*open)(char *ename, char *sname, char *tgt,
		    int showf, int bnm, int unw, int dem);
    void (*close)(exedes *st);
    void (*map_addr)(SC_storloc *sl, exedes *st, char *ad);
    int (*map_addrs)(exedes *st, int na, char **ad);};

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                         FUNCTION DECLARATIONS                            */

/*--------------------------------------------------------------------------*/


/* SCEXE.C declarations */

extern void
 _SC_set_demangle_style(char *opt),
 SC_exe_init_api(void);


#endif

