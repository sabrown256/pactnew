/*
 * NETWORK.H - header to provide standard TCP/IP networking on UNIX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#ifndef PCK_NETWORK_UNIX

#define PCK_NETWORK_UNIX

#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#ifndef INADDR_NONE
# define INADDR_NONE NULL
#endif

#endif
