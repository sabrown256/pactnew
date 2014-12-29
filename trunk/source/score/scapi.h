/*
 * SCAPI.H - system call API upon which PACT depends
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_OS_API

#define PCK_OS_API

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                             STANDARD MACROS                              */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                           STANDARD TYPEDEFS                              */

/*--------------------------------------------------------------------------*/

typedef struct s_syscall_api syscall_api;

struct s_syscall_api
   {struct passwd *(*fgetpwuid)(uid_t uid);
    uid_t (*fgetuid)(void);
    gid_t (*fgetgid)(void);

    pid_t (*fgetpid)(void);
    pid_t (*fgetppid)(void);
    pid_t (*fgetpgrp)(void);
    pid_t (*ftcgetpgrp)(int fd);
    pid_t (*fsetsid)(void);
    pid_t (*ffork)(void);
    pid_t (*fwaitpid)(pid_t pid, int *status, int options);

    int (*fkill)(pid_t pid, int sig);
    int (*fsched_yield)(void);

    int (*ffsync)(int fd);
    int (*fioctl)(int d, unsigned long request, ...);
    int (*ffcntl)(int fd, int cmd, ...);

    int (*fpoll)(struct pollfd *fds, nfds_t nfds, int timeout);
    int (*fnanosleep)(const struct timespec *req, struct timespec *rem);

    int (*fsetenv)(const char *name, const char *value, int overwrite);
    int (*funsetenv)(const char *name);
    int (*fgethostname)(char *name, size_t len);
    char *(*fttyname)(int fd);
    void (*fsrandom)(uint32_t seed);
    long (*frandom)(void);};

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                    STANDARD VARIABLE DECLARATIONS                        */

/*--------------------------------------------------------------------------*/

extern syscall_api
  _SC_osapi;

/*--------------------------------------------------------------------------*/

/*                    STANDARD FUNCTION DECLARATIONS                        */

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif
