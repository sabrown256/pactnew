/*
 * SCOPE_MAKE.H - support make like functionality
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SCOPE_MAKE

#define PCK_SCOPE_MAKE

#include "cpyright.h"
#include "scope_proc.h"

#ifdef AIX
# define __AR_SMALL__
# define __AR_BIG__
#endif
#include <ar.h>

/*--------------------------------------------------------------------------*/

/*                           DEFINED CONSTANTS                              */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/

typedef struct s_cmdes cmdes;
typedef struct s_ruledes ruledes;
typedef struct s_ruledef ruledef;
typedef struct s_vardef vardef;
typedef struct s_anadep anadep;

struct s_cmdes
   {int n;
    int kind;
    char *target;
    char *dependent;
    char *suffix;
    char **actions;};

struct s_ruledes
   {char *name;          /* the rule name - not always the target */
    char *file;          /* the file containing the rule */
    int line;            /* the line number in the file where rule starts */
    int n_dependencies;
    int n_actions;
    char *depend;        /* temporary holder until substitutions done */
    char *act;           /* temporary holder until substitutions done */
    char **dependencies;
    char **actions;};

struct s_ruledef
   {char name[MAXLINE];
    int line;
    int nc;
    char *text;};

struct s_vardef
   {char name[MAXLINE];
    int line;
    char *text;};

struct s_anadep
   {int need_suffix;
    int need_subst;
    int show_vars;
    int show_rules;
    int verbose;
    hasharr *rules;
    hasharr *variables;
    hasharr *archives;
    hasharr *temp;
    SC_array *suffices;
    SC_array *actions;
    FILE *log;
    char arch[MAXLINE];
    char root[PATH_MAX];
    char exe[PATH_MAX];
    int (*pred)(char *fa, char *fb, int force, anadep *state);};

enum e_SC_rule_cat
   {IMPLICIT  = 1,
    EXPLICIT,
    ARCHIVE};

typedef enum e_SC_rule_cat SC_rule_cat;

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/


/* SCMKDP.C declarations */

extern int
 _SC_is_newer(char *fa, char *fb, int force, anadep *state),
 SC_analyze_dependencies(char *tgt, anadep *state);

extern char
 **SC_action_commands(anadep *state, int recur);


/* SCMKPR.C declarations */

extern anadep
 *SC_make_state(void);

extern void
 _SC_do_substitutions(anadep *state),
 _SC_print_rule_info(anadep *state, int n, ruledes *rd),
 SC_show_make_rules(anadep *state),
 SC_make_def_rule(char *rule, anadep *state),
 SC_make_def_var(char *var, anadep *state),
 SC_free_state(anadep *state);

extern char
  *_SC_subst_macro(char *src, int off,
		   SC_rule_cat whch, char *tgt, char *dep, char *sfx),
 *_SC_subst_str(ruledes *rd, char *src, anadep *state);

extern int
 SC_parse_makefile(char *fname, anadep *state),
 SC_parse_premake(char *fname, anadep *state);

#endif

