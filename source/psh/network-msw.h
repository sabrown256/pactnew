/*
 * NETWORK-MSW.H - header to provide standard TCP/IP networking on MS Windows
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#ifndef PCK_NETWORK_MSW

#define PCK_NETWORK_MSW

typedef unsigned long in_addr_t;

#include <ws2tcpip.h>
#include <winsock2.h>

/*--------------------------------------------------------------------------*/

#if 0

/*--------------------------------------------------------------------------*/

htonl
htons
socket
bind
listen
accept
connect
inet_addr
gethostname
getsockname
gethostbyname
select

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

#endif
