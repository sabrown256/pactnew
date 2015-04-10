/*
 * NETWORK.H - header to provide standard TCP/IP networking
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#ifndef PCK_NETWORK

#define PCK_NETWORK

#if defined(_WIN32)
# include "network-msw.h"
#elif defined(BEOS)
# include "network-beos.h"
#else
# include "network-unix.h"
#endif

#ifndef INADDR_NONE
# define INADDR_NONE NULL
#endif

#endif
