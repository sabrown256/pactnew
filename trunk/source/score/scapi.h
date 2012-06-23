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

#define SYS_GETUID()                                                         \
   ((_SC_osapi.fgetuid != NULL) ? _SC_osapi.fgetuid() : -1)

#define SYS_GETGID()                                                         \
   ((_SC_osapi.fgetgid != NULL) ? _SC_osapi.fgetgid() : -1)

#define SYS_GETPID()                                                         \
   ((_SC_osapi.fgetpid != NULL) ? _SC_osapi.fgetpid() : -1)

#define SYS_GETPPID()                                                        \
   ((_SC_osapi.fgetppid != NULL) ? _SC_osapi.fgetppid() : -1)

#define SYS_GETPGRP()                                                        \
   ((_SC_osapi.fgetpgrp != NULL) ? _SC_osapi.fgetpgrp() : -1)

#define SYS_TCGETPGRP(_p)                                                    \
   ((_SC_osapi.ftcgetpgrp != NULL) ? _SC_osapi.ftcgetpgrp(_p) : -1)

#define SYS_SETSID()                                                         \
   ((_SC_osapi.fsetsid != NULL) ? _SC_osapi.fsetsid() : -1)

#define SYS_FORK()                                                           \
   ((_SC_osapi.ffork != NULL) ? _SC_osapi.ffork() : -1)

#define SYS_WAITPID(_a, _b, _c)                                              \
   ((_SC_osapi.fwaitpid != NULL) ? _SC_osapi.fwaitpid(_a, _b, _c) : -1)

#define SYS_KILL(_p, _s)                                                     \
   ((_SC_osapi.fkill != NULL) ? _SC_osapi.fkill(_p, _s) : -1)

#define SYS_SCHED_YIELD()                                                    \
   ((_SC_osapi.fsched_yield != NULL) ? _SC_osapi.fsched_yield() : -1)

#define SYS_FSYNC(_p)                                                        \
   ((_SC_osapi.ffsync != NULL) ? _SC_osapi.ffsync(_p) : -1)

#define SYS_POLL(_a, _b, _c)                                                 \
   ((_SC_osapi.fpoll != NULL) ? _SC_osapi.fpoll(_a, _b, _c) : -1)

#define SYS_NANOSLEEP(_a, _b)                                                \
   ((_SC_osapi.fnanosleep != NULL) ? _SC_osapi.fnanosleep(_a, _b) : -1)

#define SYS_SETENV(_a, _b, _c)                                               \
   ((_SC_osapi.fsetenv != NULL) ? _SC_osapi.fsetenv(_a, _b, _c) : -1)

#define SYS_UNSETENV(_a)                                                     \
   ((_SC_osapi.funsetenv != NULL) ? _SC_osapi.funsetenv(_a) : -1)

#define SYS_GETHOSTNAME(_a, _b)                                              \
   ((_SC_osapi.fgethostname != NULL) ? _SC_osapi.fgethostname(_a, _b) : -1)

#define SYS_TTYNAME(_a)                                                      \
   ((_SC_osapi.fttyname != NULL) ? _SC_osapi.fttyname(_a) : NULL)

#define SYS_RANDOM()                                                         \
   ((_SC_osapi.frandom != NULL) ? _SC_osapi.frandom() : -1)

#define SYS_SRANDOM(_p)                                                      \
   {if (_SC_osapi.fsrandom != NULL) _SC_osapi.fsrandom(_p);}


/* variadic macro wrappers */

#define SYS_IOCTL(_a...)                                                     \
   ((_SC_osapi.fioctl != NULL) ? _SC_osapi.fioctl(_a) : -1)

#define SYS_FCNTL(_a...)                                                     \
   ((_SC_osapi.ffcntl != NULL) ? _SC_osapi.ffcntl(_a) : -1)

/*--------------------------------------------------------------------------*/

/*                           STANDARD TYPEDEFS                              */

/*--------------------------------------------------------------------------*/

typedef struct s_syscall_api syscall_api;
typedef struct s_sysaux_api sysaux_api;

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
    void (*fsrandom)(unsigned int seed);
    long (*frandom)(void);};


struct s_sysaux_api
   {int junk;
#if 0
 __SC_os

 _SC_gets
 _SC_printf

 _SC_get_resource_limits
 _SC_set_resource_limits
 _SC_resource_usage

 _SC_init_server
 _SC_init_client

 _SC_open
 _SC_open_remote
 _SC_get_processor_number
 _SC_process_status
 _SC_running_children
 _SC_save_exited_children
 __SC_redir_filedes
 __SC_init_filedes
 __SC_redir_fail
#endif
    };

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                    STANDARD VARIABLE DECLARATIONS                        */

/*--------------------------------------------------------------------------*/

extern syscall_api
  _SC_osapi;

extern sysaux_api
  _SC_osaux;

/*--------------------------------------------------------------------------*/

/*                    STANDARD FUNCTION DECLARATIONS                        */

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif
