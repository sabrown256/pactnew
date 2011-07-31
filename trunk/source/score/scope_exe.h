/*
 * SCOPE_EXE.H - support executable access
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SCOPE_EXE

# define PCK_SCOPE_EXE

# include <bfd.h>
# include <demangle.h>

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/

typedef struct s_srcloc srcloc;

struct s_srcloc
   {int line;
    char file[MAXLINE];
    char func[MAXLINE];};

typedef struct s_csrcloc csrcloc;

struct s_csrcloc
   {unsigned int line;
    const char *file;
    const char *func;};

typedef struct s_exedes exedes;

struct s_exedes
  {int unwin;               /* unwind inlined functions */
   int showf;     	    /* show function names */
   int demang;	            /* demangle names */
   int tailf;	            /* print base file names only - no dirs */
   int found;               /* internal use */
   char *ename;             /* executable name */
   char *sname;             /* section name - defaults to NULL */
   char *target;            /* format target */
   csrcloc where;           /* temporary, internal use */
   bfd *et;                 /* executable file info */
   asection *es;            /* section info */
   asymbol **symt;	    /* symbol table */
   bfd_vma pc;
   char err[MAXLINE];};     /* latest error message */

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                         FUNCTION DECLARATIONS                            */

/*--------------------------------------------------------------------------*/


/* SCEXE.C declarations */

extern void
 _SC_set_demangle_style(char *opt),
 SC_exe_close(exedes *st);

extern int
 SC_exe_map_addrs(exedes *st, int na, char **ad);

extern srcloc
 SC_exe_map_addr(exedes *st, char *ad);

extern exedes
 *SC_exe_open(char *ename, char *sname, char *tgt,
	      int showf, int bnm, int unw, int dem);

#endif

