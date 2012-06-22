/*
 * SCIRIX.H - IRIX support header for PACT codes
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

/* SGI predefines __sgi */
#ifdef __sgi

#ifndef PCK_IRIX

#define PCK_IRIX

#include "cpyright.h"

#if 0
#include <stdarg.h>
#include <sys/types.h>
#endif

#include <sys/sysmp.h>
#include <sys/sysinfo.h> /* for SAGET and MINFO structures */

/*--------------------------------------------------------------------------*/

/*                         STANDARD CONSTANT MACROS                         */

/*--------------------------------------------------------------------------*/

#define HAVE_INT_CTIME

#ifndef PATH_MAX
# define PATH_MAX   4096
#endif

#ifndef NGROUPS_MAX
# define NGROUPS_MAX 16
#endif

/*--------------------------------------------------------------------------*/

/*                           STANDARD TYPEDEFS                              */

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
  typedef void (*PFSignal_handler)(...);
# define SIGNAL_HANDLER_DEFINED
#endif

/*--------------------------------------------------------------------------*/

/*                        STANDARD PROCEDURAL MACROS                        */

/*--------------------------------------------------------------------------*/

#define JMP_BUF            jmp_buf
#define SETJMP(_x)         setjmp(_x)
#define LONGJMP(_x, _v)    longjmp(_x, _v)

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                      STANDARD VARIABLE DECLARATIONS                      */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                      STANDARD FUNCTION DECLARATIONS                      */

/*--------------------------------------------------------------------------*/

extern char
 *strdup(const char *s1),
 *strtok_r(char *s1, const char *s2, char **lasts);

extern int
 vsnprintf(char *dst, size_t nc, const char *fmt, va_list a);

#ifdef __cplusplus
}
#endif

#endif

#endif

