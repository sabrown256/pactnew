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

extern int
 gethostname_msw(char *name, size_t len),
 socket_msw(int domain, int type, int protocol),
 bind_msw(int sockfd, const struct sockaddr *addr, socklen_t addrlen),
 connect_msw(int sockfd, const struct sockaddr *addr, socklen_t addrlen),
 listen_msw(int sockfd, int backlog),
 getsockname_msw(int sockfd, struct sockaddr *addr, socklen_t *addrlen),
 accept_msw(int sockfd, struct sockaddr *addr, socklen_t *addrlen),
 gethost_msw(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

extern uint32_t
 htonl_msw(uint32_t hostlong);

extern uint16_t
 htons_msw(uint16_t hostshort);

extern in_addr_t
 inet_addr_msw(const char *cp);

extern struct hostent
 *gethostbyname_msw(const char *name);


#ifndef NO_DEFINE_MSW_NETWORK_FUNCS

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GETHOSTNAME_MSW - gethostname for MSW */

int gethostname_msw(char *name, size_t len)
   {int rv;

    rv = -1;

    return(rv);}

#define gethostname    gethostname_msw

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SOCKET_MSW - socket for MSW */

int socket_msw(int domain, int type, int protocol)
   {int rv;

    rv = -1;

    return(rv);}

#define socket    socket_msw

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HTONL_MSW - htonl for MSW */

uint32_t htonl_msw(uint32_t hostlong)
   {uint32_t rv;

    rv = 0;

    return(rv);}

#define htonl    htonl_msw

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HTONS_MSW - htons for MSW */

uint16_t htons_msw(uint16_t hostshort)
   {uint16_t rv;

    rv = 0;

    return(rv);}

#define htons    htons_msw

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_MSW - bind for MSW */

int bind_msw(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
   {int rv;

    rv = -1;

    return(rv);}

#define bind    bind_msw

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONNECT_MSW - connect for MSW */

int connect_msw(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
   {int rv;

    rv = -1;

    return(rv);}

#define connect    connect_msw

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LISTEN_MSW - listen for MSW */

int listen_msw(int sockfd, int backlog)
   {int rv;

    rv = -1;

    return(rv);}

#define listen    listen_msw

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GETSOCKNAME_MSW - getsockname for MSW */

int getsockname_msw(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
   {int rv;

    rv = -1;

    return(rv);}

#define getsockname    getsockname_msw

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ACCEPT_MSW - accept for MSW */

int accept_msw(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
   {int rv;

    rv = -1;

    return(rv);}

#define accept    accept_msw

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INET_ADDR_MSW - inet_addr for MSW */

in_addr_t inet_addr_msw(const char *cp)
   {in_addr_t rv;

    rv = -1;

    return(rv);}

#define inet_addr    inet_addr_msw

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GETHOST_MSW - gethost for MSW */

int gethost_msw(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
   {int rv;

    rv = -1;

    return(rv);}

#define gethost    gethost_msw

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GETHOSTBYNAME_MSW - gethostbyname for MSW */

struct hostent *gethostbyname_msw(const char *name)
   {struct hostent *rv;

    rv = NULL;

    return(rv);}

#define gethostbyname    gethostbyname_msw

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

#endif
