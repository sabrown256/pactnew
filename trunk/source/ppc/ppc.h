/*
 * PPC.H - header for Portable Process Control system
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_PPC

#include "cpyright.h"

#define PCK_PPC

#include "pdb.h"

#define PC_SIGIO SC_SIGIO

#define PC_CHILD     SC_CHILD
#define PC_PARENT    SC_PARENT
#define PC_LOCAL     SC_LOCAL
#define PC_REMOTE    SC_REMOTE
#define PC_PARALLEL  SC_PARALLEL

#define PC_APPEND    SC_APPEND
#define PC_SYNC      SC_SYNC

/*--------------------------------------------------------------------------*/

/*                           MESSAGE PASSING SUPPORT                        */

/*--------------------------------------------------------------------------*/

#define PC_open_node      SC_open_node
#define PC_exit_all       SC_exit_all

#define PC_open           PN_open_process
#define PC_open_member    PN_open_member
#define PC_close_member   PN_close_member
#define PC_in             PN_in
#define PC_out            PN_out

/*--------------------------------------------------------------------------*/

/*                             PROCEDURAL MACROS                            */

/*--------------------------------------------------------------------------*/

#define PN_open_member(_x, _y)                                               \
   (PN_gs.oper.open_member != NULL) ?                                        \
   (*PN_gs.oper.open_member)(_x, _y) :                                       \
   NULL

#define PN_close_member(_x)                                                  \
   {if (PN_gs.oper.close_member != NULL)                                     \
       (*PN_gs.oper.close_member)(_x);}

#define PN_size_message(_x, _y, _z)                                          \
   (PN_gs.oper.size_message != NULL) ?                                       \
   (*PN_gs.oper.size_message)(_x, _y, _z) :                                  \
   0L

#define PN_glmn_message(_x)                                                  \
   (PN_gs.oper.glmn_message != NULL) ?                                       \
   (*PN_gs.oper.glmn_message)(_x) :                                          \
   0.0

#define PN_out(_a, _b, _c, _d, _e)                                           \
   (PN_gs.oper.out != NULL) ?                                                \
   (*PN_gs.oper.out)(_a, _b, _c, _d, _e) :                                   \
   0L

#define PN_in(_a, _b, _c, _d, _e)                                            \
   (PN_gs.oper.in != NULL) ?                                                 \
   (*PN_gs.oper.in)(_a, _b, _c, _d, _e) :                                    \
   0L

#define PN_wait(_x)                                                          \
   (PN_gs.oper.wait != NULL) ?                                               \
   (*PN_gs.oper.wait)(_x) :                                                  \
   0L

/*--------------------------------------------------------------------------*/

/*                             TYPEDEFS AND STRUCTS                         */

/*--------------------------------------------------------------------------*/

typedef struct s_PN_par_fnc PN_par_fnc;
typedef struct s_PN_scope_public PN_scope_public;

struct s_PN_par_fnc
   {PROCESS *(*open_member)(char **argv, int *pnn);
    void (*close_member)(PROCESS *pp);
    long (*size_message)(int sp, char *type, int tag);
    double (*glmn_message)(double vi);
    long (*out)(void *vr, char *type, size_t ni, PROCESS *pp, int *filt);
    long (*in)(void *vr, char *type, size_t ni, PROCESS *pp, int *filt);
    long (*wait)(PROCESS *pp);};

struct s_PN_scope_public
   {PN_par_fnc oper;};

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                            VARIABLE DECLARATIONS                         */

/*--------------------------------------------------------------------------*/

extern PN_scope_public
  PN_gs;

/*--------------------------------------------------------------------------*/

/*                            FUNCTION DECLARATIONS                         */

/*--------------------------------------------------------------------------*/


extern PROCESS
 *PN_mk_process(char **argv, char *mode, int type);


/* PNBIN.C declarations */

extern int
 PN_recv_formats(PROCESS *pp),
 PN_send_formats(void);


/* PNPIO.C declarations */

extern int
 PN_process_access(char **argv, char *mode),
 PN_push_message(SC_message *msg, int i, int ni, char *type, char *bf),
 PN_buffer_data_in(PROCESS *pp),
 PN_buffer_data_out(PROCESS *pp, char *bf,
		    int nbi, int block_state);

extern void
 PN_pop_message(int i);


/* PPC.C declarations */

extern PROCESS
 *PN_open_process(char **argv, char **envp, char *mode),
 *PN_mk_process(char **argv, char *mode, int type);


/* PNPARC.C declarations */

extern int
 PN_open_group(char **argv, int *pn);

extern void
 PN_push_pending(PROCESS *pp, int op, char *bf, char *type,
		 size_t ni, void *vr, void *req),
 PN_pop_pending(PROCESS *pp, int *po, char **pbf, char **pty,
		size_t *pni, void **pvr),
 PN_init_communications(void (*init)(PN_par_fnc *p)),
 PN_sync_execution(void);


/* PNMPI.C declarations */

extern void
 PN_init_mpi(PN_par_fnc *p);

#ifdef __cplusplus
}
#endif

#endif


