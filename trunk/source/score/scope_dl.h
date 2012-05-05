/*
 * SCOPE_DL.H - define dynamic linker scope
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SCOPE_DL

#define PCK_SCOPE_DL

#include "cpyright.h"

#include "scope_proc.h"

#ifdef HAVE_DYNAMIC_LINKER
# include <dlfcn.h>
#endif

/*--------------------------------------------------------------------------*/

/*                             PROCEDURAL MACROS                            */

/*--------------------------------------------------------------------------*/

#define ASSIGN_FNC(_vr, _vl)      (*(void **) (&(_vr)) = _vl)


/*--------------------------------------------------------------------------*/

/*                             TYPEDEFS AND STRUCTS                         */

/*--------------------------------------------------------------------------*/

enum e_objindex
   { OBJ_FUNC, OBJ_SO, OBJ_EXE, OBJ_UNKNOWN };

typedef enum e_objindex objindex;

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

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                            VARIABLE DECLARATIONS                         */

/*--------------------------------------------------------------------------*/

extern sostate
 _SC_dl;

/*--------------------------------------------------------------------------*/

/*                            FUNCTION DECLARATIONS                         */

/*--------------------------------------------------------------------------*/


/* SCDL.C declarations */

extern int
 SC_so_close(char *name),
 SC_so_release(void),
 SC_so_register_func(objindex kind, char *lib, char *name,
		     char *path, char *flags, char *rv, char *argl),
 SC_so_config(char *fname);

extern void
 *SC_so_get(objindex kind, char *tag, ...);


#ifdef __cplusplus
}
#endif

#endif

