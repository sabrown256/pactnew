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
/*--------------------------------------------------------------------------*/

extern int
 gethostname_msw(char *name, size_t len),
 socket_msw(int domain, int type, int protocol),
 bind_msw(int sockfd, const struct sockaddr *addr, socklen_t addrlen),
 connect_msw(int sockfd, const struct sockaddr *addr, socklen_t addrlen),
 listen_msw(int sockfd, int backlog),
 getsockname_msw(int sockfd, struct sockaddr *addr, socklen_t *addrlen),
 getsockopt_msw(int sockfd, int level, int optname,
		void *optval, socklen_t *optlen),
 accept_msw(int sockfd, struct sockaddr *addr, socklen_t *addrlen),
 gethost_msw(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

extern u_int32_t
 htonl_msw(u_int32_t hostlong);

extern u_int16_t
 htons_msw(u_int16_t hostshort);

extern in_addr_t
 inet_addr_msw(const char *cp);

extern struct hostent
 *gethostbyname_msw(const char *name);

/*--------------------------------------------------------------------------*/

#ifndef NO_DEFINE_MSW_NETWORK_FUNCS

/*--------------------------------------------------------------------------*/

/* GETHOSTNAME_MSW - gethostname for MSW */

int gethostname_msw(char *name, size_t len)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SOCKET_MSW - socket for MSW */

int socket_msw(int domain, int type, int protocol)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HTONL_MSW - htonl for MSW */

u_int32_t htonl_msw(u_int32_t hostlong)
   {u_int32_t rv;

    rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HTONS_MSW - htons for MSW */

u_int16_t htons_msw(u_int16_t hostshort)
   {u_int16_t rv;

    rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BIND_MSW - bind for MSW */

int bind_msw(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONNECT_MSW - connect for MSW */

int connect_msw(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LISTEN_MSW - listen for MSW */

int listen_msw(int sockfd, int backlog)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GETSOCKNAME_MSW - getsockname for MSW */

int getsockname_msw(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GETSOCKOPT_MSW - getsockopt for MSW */

int getsockopt_msw(int sockfd, int level, int optname,
		   void *optval, socklen_t *optlen)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ACCEPT_MSW - accept for MSW */

int accept_msw(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INET_ADDR_MSW - inet_addr for MSW */

in_addr_t inet_addr_msw(const char *cp)
   {in_addr_t rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GETHOST_MSW - gethost for MSW */

int gethost_msw(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GETHOSTBYNAME_MSW - gethostbyname for MSW */

struct hostent *gethostbyname_msw(const char *name)
   {struct hostent *rv;

    rv = NULL;

    return(rv);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

#define gethostname      gethostname_msw
#define socket           socket_msw
#define htonl            htonl_msw
#define htons            htons_msw
#define bind             bind_msw
#define connect          connect_msw
#define listen           listen_msw
#define getsockname      getsockname_msw
#define getsockopt       getsockopt_msw
#define accept           accept_msw
#define inet_addr        inet_addr_msw
#define gethost          gethost_msw
#define gethostbyname    gethostbyname_msw

#endif
