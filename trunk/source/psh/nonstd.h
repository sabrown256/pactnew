/*
 * NONSTD.H - header to achieve POSIX environment
 *          - from compilers which are not POSIX by default
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * include "cpyright.h"
 *
 */

#ifndef PCK_NONSTD
#define PCK_NONSTD

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifdef HPUX

#include <netinet/in.h>

#define O_NDELAY  0000004

typedef size_t socklen_t;

extern char
 *strdup(const char *s);

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifdef SOLARIS
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifdef __sgi 

extern char
 *strdup(const char *s1);

extern FILE
 *popen(const char *command, const char *type);

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#if defined(BEOS)

typedef long long int64_t;

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

