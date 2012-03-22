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

#define PC_open_node SC_open_node

#define PC_MATCH_TYPE   SC_MATCH_TYPE
#define PC_MATCH_TAG    SC_MATCH_TAG
#define PC_MATCH_NODE   SC_MATCH_NODE
#define PC_MATCH_PID    SC_MATCH_PID
#define PC_BLOCK_STATE  SC_BLOCK_STATE
#define PC_BUFFER_SIZE  SC_BUFFER_SIZE

#define PC_READ_MSG     SC_READ_MSG
#define PC_WRITE_MSG    SC_WRITE_MSG

#define PC_TYPE_MATCH   SC_TYPE_MATCH
#define PC_CLASS_MATCH  SC_CLASS_MATCH
#define PC_TYPE_MASK    SC_TYPE_MASK
#define PC_TAG_MASK     SC_TAG_MASK

/*--------------------------------------------------------------------------*/

/*                     INTERRUPT DRIVEN I/O HANDLING                        */

/*--------------------------------------------------------------------------*/

#define PC_ASYNC      SC_ASYNC
#define PC_poll       SC_poll

/*--------------------------------------------------------------------------*/

/*                                PTY HANDLING                              */

/*--------------------------------------------------------------------------*/

#define PC_MASTER_PTY_NAME      SC_MASTER_PTY_NAME
#define PC_MASTER_PTY_LETTERS   SC_MASTER_PTY_LETTERS
#define PC_MASTER_PTY_DIGITS    SC_MASTER_PTY_DIGITS
#define PC_SLAVE_PTY_NAME       SC_SLAVE_PTY_NAME
#define PC_SLAVE_PTY_LETTERS    SC_SLAVE_PTY_LETTERS
#define PC_SLAVE_PTY_DIGITS     SC_SLAVE_PTY_DIGITS

/*--------------------------------------------------------------------------*/

/*                    REMOTE FILE/PROCESS I/O HANDLING                      */

/*--------------------------------------------------------------------------*/

#define PC_FOPEN        SC_FOPEN
#define PC_FSETVBUF     SC_FSETVBUF
#define PC_FCLOSE       SC_FCLOSE
#define PC_FFLUSH       SC_FFLUSH
#define PC_FTELL        SC_FTELL
#define PC_FSEEK        SC_FSEEK
#define PC_FREAD        SC_FREAD
#define PC_FWRITE       SC_FWRITE
#define PC_FPUTS        SC_FPUTS
#define PC_FGETS        SC_FGETS
#define PC_FGETC        SC_FGETC
#define PC_FUNGETC      SC_FUNGETC
#define PC_FEXIT        SC_FEXIT

/*--------------------------------------------------------------------------*/

/*                             PROCEDURAL MACROS                            */

/*--------------------------------------------------------------------------*/

#define _PC_debug                  _SC_debug
#define _PC_diag                   _SC_diag

#define PC_gets                     SC_gets
#define PC_printf                   SC_printf

#define PC_set_attr                 SC_set_attr
#define PC_set_fd_attr              SC_set_fd_attr
#define PC_echo_on_file             SC_echo_on_file
#define PC_echo_on_fd               SC_echo_on_fd
#define PC_echo_off_file            SC_echo_off_file
#define PC_echo_off_fd              SC_echo_off_fd
#define PC_unblock                  SC_unblock
#define PC_unblock_file             SC_unblock_file
#define PC_unblock_fd               SC_unblock_fd
#define PC_block                    SC_block
#define PC_block_file               SC_block_file
#define PC_block_fd                 SC_block_fd
#define PC_io_callback              SC_io_callback
#define PC_io_callback_file         SC_io_callback_file
#define PC_io_callback_fd           SC_io_callback_fd
#define PC_rl_io_callback           SC_rl_io_callback
#define PC_catch_io_interrupts      SC_catch_io_interrupts
#define PC_poll_descriptors         SC_poll_descriptors
#define PC_poll                     SC_poll
#define PC_init_server              SC_init_server
#define PC_init_client              SC_init_client
#define PC_get_number_processors    SC_get_number_processors
#define PC_get_processor_number     SC_get_processor_number

#define PC_set_raw_state            SC_set_raw_state
#define PC_set_cooked_state         SC_set_cooked_state

#define PC_file_access              SC_file_access
#define PC_exit_all                 SC_exit_all
#define PC_io_interrupts_on         SC_gs.io_interrupts_on


#define PC_BINARY_MODEP(x)  (strchr(&((x)[1]), 'b') != NULL)
#define PC_OTHER_HOSTP(x)   (strchr((x), ':') != NULL)
#define PC_OTHER_CPUP(x)    (strchr((x), '@') != NULL)

#define PC_flush    SC_flush
#define PC_close    SC_close
#define PC_status   SC_status
#define PC_read     SC_read
#define PC_write    SC_write
#define PC_alarm    SC_timeout

#define PC_open_member(_x, _y)                                               \
   (PC_gs.oper.open_member != NULL) ?                                        \
   (*PC_gs.oper.open_member)(_x, _y) :                                       \
   NULL

#define PC_close_member(_x)                                                  \
   {if (PC_gs.oper.close_member != NULL)                                     \
       (*PC_gs.oper.close_member)(_x);}

#define PC_size_message(_x, _y, _z)                                          \
   (PC_gs.oper.size_message != NULL) ?                                       \
   (*PC_gs.oper.size_message)(_x, _y, _z) :                                  \
   0L

#define PC_glmn_message(_x)                                                  \
   (PC_gs.oper.glmn_message != NULL) ?                                       \
   (*PC_gs.oper.glmn_message)(_x) :                                          \
   0.0

#define PC_out(_a, _b, _c, _d, _e)                                           \
   (PC_gs.oper.out != NULL) ?                                                \
   (*PC_gs.oper.out)(_a, _b, _c, _d, _e) :                                   \
   0L

#define PC_in(_a, _b, _c, _d, _e)                                            \
   (PC_gs.oper.in != NULL) ?                                                 \
   (*PC_gs.oper.in)(_a, _b, _c, _d, _e) :                                    \
   0L

#define PC_wait(_x)                                                          \
   (PC_gs.oper.wait != NULL) ?                                               \
   (*PC_gs.oper.wait)(_x) :                                                  \
   0L

/*--------------------------------------------------------------------------*/

/*                             TYPEDEFS AND STRUCTS                         */

/*--------------------------------------------------------------------------*/

typedef struct s_PC_par_fnc PC_par_fnc;
typedef struct s_PC_global_state PC_global_state;

struct s_PC_par_fnc
   {PROCESS *(*open_member)(char **argv, int *pnn);
    void (*close_member)(PROCESS *pp);
    long (*size_message)(int sp, char *type, int tag);
    double (*glmn_message)(double vi);
    long (*out)(void *vr, char *type, size_t ni, PROCESS *pp, int *filt);
    long (*in)(void *vr, char *type, size_t ni, PROCESS *pp, int *filt);
    long (*wait)(PROCESS *pp);};

struct s_PC_global_state
   {PC_par_fnc oper;};

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                            VARIABLE DECLARATIONS                         */

/*--------------------------------------------------------------------------*/

extern PC_global_state
  PC_gs;

/*--------------------------------------------------------------------------*/

/*                            FUNCTION DECLARATIONS                         */

/*--------------------------------------------------------------------------*/


extern PROCESS
 *PC_mk_process(char **argv, char *mode, int type);


/* PCBIN.C declarations */

extern int
 PC_recv_formats(PROCESS *pp),
 PC_send_formats(void);


/* PCPIO.C declarations */

extern int
 PC_process_access(char **argv, char *mode),
 PC_push_message(SC_message *msg, int i, int ni, char *type, char *bf),
 PC_buffer_data_in(PROCESS *pp),
 PC_buffer_data_out(PROCESS *pp, char *bf,
		    int nbi, int block_state);

extern void
 PC_pop_message(int i);


/* PPC.C declarations */

extern PROCESS
 *PC_open(char **argv, char **envp, char *mode),
 *PC_mk_process(char **argv, char *mode, int type);

extern int
 PC_io_connect(int flag);


/* PCPARC.C declarations */

extern int
 PC_get_number_processors(void),
 PC_get_processor_number(void),
 PC_open_group(char **argv, int *pn);

extern void
 PC_push_pending(PROCESS *pp, int op, char *bf, char *type,
		 size_t ni, void *vr, void *req),
 PC_pop_pending(PROCESS *pp, int *po, char **pbf, char **pty,
		size_t *pni, void **pvr),
 PC_init_communications(void (*init)(PC_par_fnc *p)),
 PC_sync_execution(void);


/* PCPARMP.C declarations */

extern void
 PC_init_mpi(PC_par_fnc *p);

#ifdef __cplusplus
}
#endif

#endif


