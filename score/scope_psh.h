/*
 * SCOPE_PSH.H - header defining PSH wrapper scope
 *
 */

#ifndef PCK_SCOPE_PSH

#define PCK_SCOPE_PSH

#include "cpyright.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* helpers for BLANG bindings */

/* variadic macro example:
 * #define eprintf(...) fprintf(stderr, __VA_ARGS__)
 */

/* ARG - specify default value, in/out, etc
 *     - ignored by CC but parsed by BLANG
 */

#undef ARG
#define ARG(...)

/* MBR - specify cast members to generate PD_cast
 *     - ignored by CC but parsed by BLANG
 */

#undef MBR
#define MBR(...)

/* BIND_OPT - used to communicate between binding language
 *          - and actual PACT C struct methods
 */

enum e_bind_opt
   {BIND_NONE, BIND_ALLOC, BIND_FREE, BIND_LABEL, BIND_PRINT, BIND_ARG};

typedef enum e_bind_opt bind_opt;

/* include the PSH files with SCOPE_SCORE_PREPROC to get in
 * macro definitions, function and variable declarations
 * this one time right here
 * the function and variable definitions will be brought in in scpsh.h
 */

#define SCOPE_SCORE
#define SCOPE_SCORE_PREPROC

#include <network.h>
#include <shell/libio.c>
#include <shell/libfio.c>
#include <shell/libpsh.c>
#include <shell/libhash.c>
#include <shell/libtyp.c>
#include <shell/libstack.c>
#include <shell/libtime.c>
#include <shell/libasync.c>
#include <shell/libeval.c>
#include <shell/libsock.c>
#include <shell/libsrv.c>
#include <shell/libsubst.c>
#include <shell/libdb.c>
#include <shell/libpgrp.c>
#include <shell/libinfo.c>

#undef SCOPE_SCORE

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/


/* LIBASYNC.C declarations */

extern void
 _PS_dbg(unsigned int lvl, const char *fmt, ...),
 _PS_job_io_close(process *pp, io_kind knd),
 _PS_job_grp_attr(int pid, int pgid, int jctl, int g, int t),
 _PS_default_iodes(iodes *pio),
 _PS_init_process(process *pp),
 _PS_job_child_prelim(process *pp),
 PS_job_wait(process *pp),
 PS_asetup(int n, int na),
 PS_anotify(void),
 PS_amap(void (*f)(process *pp, void *a));

extern int
 _PS_fd_close(int fd),
 _PS_ioc_fd(int fd, io_kind k),
 _PS_do_rlimit(char *vr),
 _PS_job_waitr(int pid, int *pw, int opt, struct rusage *pr),
 *PS_list_fds(process_group *pg),
 PS_job_read(int fd, process *pp, int (*out)(int fd, process *pp, char *s)),
 PS_job_write(process *pp, const char *fmt, ...),
 PS_job_poll(process *pp, int to),
 PS_job_response(process *pp, int to, const char *fmt, ...),
 PS_job_signal(process *pp, int sig),
 PS_job_done(process *pp, int sig),
 PS_apoll(int to),
 PS_acheck(void),
 PS_afin(void (*f)(process *pp, void *a)),
 PS_await(unsigned int tf, int dt, char *tag,
	  int (*tty)(char *tag),
	  void (*f)(int i, char *tag, void *a, int nd, int np, int tc, int tf),
	  void *a);

extern FILE
 *_PS_io_file_ptr(process *pp, io_kind knd);

extern sigset_t
 _PS_block_all_sig(int wh);

extern io_device
 _PS_ioc_pair(int *fds, int id);

extern process
 *PS_job_launch(char *cmd, char *mode, void *a),
 *PS_alaunch(int sip, char *cmd, char *mode, void *a,
	     int (*acc)(int fd, process *pp, char *s),
	     int (*rej)(int fd, process *pp, char *s),
	     void (*wt)(process *pp)),
 *PS_arelaunch(process *pp);

extern process_state
 *PS_get_process_state(void);


/* LIBDB.C declarations */

extern void
 PS_reset_db(database *db),
 PS_load_db(database *db, const char *vr, FILE *fp),
 PS_delete_db(database *db, char *var),
 PS_free_db(database *db),
 PS_db_srv_close(database *db);

extern int
 PS_dbset(client *cl, const char *var, const char *fmt, ...),
 PS_dbdef(client *cl, const char *fmt, ...),
 PS_dbcmp(client *cl, char *var, char *val),
 PS_dbcmd(client *cl, char *cmd),
 PS_dbinitv(client *cl, char *var, const char *fmt, ...),
 PS_dbrestore(client *cl, const char *dname),
 PS_save_db(database *db, char **vars, FILE *fp, const char *fmt),
 PS_db_srv_open(client *cl, int init, int dbg, int auth),
 PS_db_srv_save(int fd, database *db),
 PS_db_srv_restart(database *db);

extern char
 *PS_name_db(const char *root),
 **_PS_db_clnt_ex(client *cl, int init, char *req),
 *PS_dbget(client *cl, int lit, const char *fmt, ...),
 *PS_put_db(database *db, char *var, char *val),
 *PS_get_db(database *db, char *var),
 *PS_get_multi_line(char **sa, int ni, int *pi, char *dlm, int qu);

extern database
 *PS_db_srv_create(client *cl, int dbg, int auth),
 *PS_db_srv_load(client *cl, int dbg, int auth);


/* LIBEVAL.C declarations */

extern void
 PS_prune_env(char *tgt, char *info);

extern char
 *PS_expand_regx(char *d, int nd, char *s),
 *PS_expand_env(char *expr, int nc, char *varn, int rnull),
 *PS_eval(char *expr, int nc, char *varn);


/* LIBFIO.C declarations */

extern void
 PS_log_safe(char *fnc, int err, char *type, void *a);

extern int
 PS_open_safe(const char *path, int flags, mode_t mode),
 PS_close_safe(int fd),
 PS_fclose_safe(FILE *fp),
 PS_fflush_safe(FILE *fp),
 PS_block_fd(int fd, int on),
 PS_unlink_safe(char *fmt, ...);

extern int64_t
 PS_wait_fs(char *fn, int64_t ln, int na);

extern ssize_t
 PS_read_safe(int fd, void *s, size_t nb, int req),
 PS_write_safe(int fd, const void *s, size_t nb);

extern size_t
 PS_fread_safe(void *s, size_t bpi, size_t nitems, FILE *fp, int req),
 PS_fwrite_safe(const void *s, size_t bpi, size_t nitems, FILE *fp);

extern FILE
 *PS_fopen_safe(const char *path, const char *mode);


/* LIBHASH.C declarations */

extern void
 PS_hash_clear(hashtab *tab),
 PS_hash_free(hashtab *tab),
 *PS_hash_def_lookup(hashtab *tab, void *key);

extern int
 PS_hash_remove(hashtab *tab, void *key),
 PS_hash_foreach(hashtab *tab, int (*fnc)(hashen *hp, void *a), void *a);

extern char
 **PS_hash_dump(hashtab *tab, char *patt, char *type, int sort);

extern hashtab
 *PS_make_hash_table(int sz);

extern hashen
 *PS_hash_lookup(hashtab *tab, void *key),
 *PS_hash_install(hashtab *tab, void *key, void *obj, char *type,
		  int lookup);


/* LIBINFO.C declarations */

extern int
 PS_report_info(char *root, int cmpl, int ltrl, int dbo,
		itarget tgt, char *ptrn),
 PS_build_makefile(char *root, char *arch, char *mkfile, int vrb);


/* LIBIO.C declarations */

extern int
 PS_ring_clear(io_ring *ring),
 PS_ring_init(io_ring *ring, unsigned int nb),
 PS_ring_ready(io_ring *ring, unsigned char ls),
 PS_ring_push(io_ring *ring, char *s, int nc),
 PS_ring_pop(io_ring *ring, char *s, int nc, unsigned int ls);


/* LIBLOG.C declarations */

extern void
 _PS_cat(FILE *out, size_t nskip, size_t ncat, char *fname),
 PS_separator(FILE *fp),
 PS_note(FILE *fp, const char *fmt, ...),
 PS_noted(FILE *fp, const char *fmt, ...),
 PS_cat(FILE *out, size_t nskip, size_t ncat, const char *fmt, ...),
 PS_finlog(void);

extern FILE
 *PS_initlog(char *mode, const char *fmt, ...);


/* LIBPGRP.C declarations */

extern void
 PS_redirect_io(process_group *pg, int ip, iodes *io),
 PS_fillin_pgrp(process_group *pg),
 PS_close_parent_child(process_group *pg, int tci),
 PS_connect_child_out_in(process_group *pg, int tci),
 PS_transfer_in(process_group *pg, int ia, int ib, io_kind knd),
 PS_connect_child_in_out(process_group *pg, int tci),
 PS_transfer_fnc_child(process_group *pg),
 _PS_post_info(process *pp),
 _PS_pgrp_wait(process *pp),
 _PS_pgrp_work(int i, char *tag, void *a, int nd, int np, int tc, int tf),
 _PS_pgrp_fin(process *pp, void *a),
 PS_register_io_pgrp(process_group *pg);

extern int
 _PS_kind_io(io_kind k),
 _PS_io_kind(int c),
 _PS_io_mode(io_kind knd),
 PS_count_fan_in(process_group *pg),
 PS_count_fan_out(process_group *pg),
 PS_gpoll(int to),
 _PS_deref_job_close(int fd, process *pp),
 _PS_deref_close_fan_in(process *pp),
 _PS_deref_close_fan_out(process *pp),
 _PS_deref_close(process *pp),
 _PS_pgrp_fan_in(int fd, process *pp, char *s),
 _PS_pgrp_fan_out(int fd, process *pp, char *s),
 _PS_pgrp_data_child(int fd, process *pp, char *s),
 _PS_pgrp_send(int fd, process *pp, char *s),
 _PS_pgrp_accept(int fd, process *pp, char *s),
 _PS_pgrp_reject(int fd, process *pp, char *s),
 _PS_deref_io(process *pp),
 _PS_pgrp_tty(char *tag),
 _PS_fnc_wait(process_group *pg, int ip, int st),
 PS_wait_pgrp(process_group *pg),
 PS_show_pgrp(process_group *pg),
 PS_gexeca(const char *db, int c, char **v, char **env,
	   int jctl, PFPCAL (*map)(const char *s)),
 PS_gexecs(const char *db, const char *s, char **env,
	   int jctl, PFPCAL (*map)(const char *s)),
 PS_transfer_ff(FILE *fi, FILE *fo),
 PS_gexec_var(const char *db, io_mode md, FILE **fio,
	      const char *name, int c, char **v),
 PS_gexec_file(const char *db, io_mode md, FILE **fio,
	       const char *name, int c, char **v);

extern proc_bf
 PS_job_foreground(process_group *pg, proc_bf cont),
 PS_job_background(process_group *pg, proc_bf cont);

extern char
 *_PS_name_io(io_kind k),
 **PS_subst_syntax(char **sa);

extern statement
 *PS_parse_statement(const char *s, char **env, char *shell,
		     PFPCAL (*map)(const char *s));

extern process_session
 *PS_make_session(int pgid, int fin, int iact, proc_bf fg),
 *PS_init_session(void);


/* LIBPSH.C declarations */

extern void
 PS_string_sort(char **v, int n),
 PS_lst_free(char **lst),
 PS_free_strings(char **lst),
 PS_copy(const char *out, const char *fmt, ...),
 PS_nsleep(int ms),
 PS_print_text(FILE *fp, const char *fmt, ...),
 PS_clean_space(char *s),
 PS_splice_out_path(char *path),
 PS_push_path(int end, char *dpath, char *path),
 PS_build_path(const char *base, ...),
 PS_key_val(char **key, char **val, char *s, const char *dlm),
 PS_unamef(char *s, int nc, const char *wh),
 PS_log_ctivity(char *flog, int ilog, int ilev,
		const char *oper, const char *fmt, ...);

extern int
 _PS_nsigaction(struct sigaction *oa, int sig, void (*fn)(int sig),
		int flags, va_list __a__),
 _PS_permission_match(struct stat bf, int md, int only),
 PS_nsigaction(struct sigaction *oa, int sig, void (*fn)(int sig),
	       int flags, ...),
 PS_lst_length(char **lst),
 PS_last_char(const char *s),
 PS_strcntc(const char *s, int c, int ex),
 PS_strcnts(const char *s, const char *r, int ex),
 PS_strcpy_tok(char *d, size_t nd, const char *s, size_t ns,
	       const char *b, const char *e, int flags),
 PS_full_path(char *path, int nc, int fl, const char *dir, const char *name),
 PS_file_kind(int knd, int md, const char *fnm),
 PS_dir_exists(const char *fmt, ...),
 PS_file_exists(const char *fmt, ...),
 PS_file_executable(const char *fmt, ...),
 PS_file_script(const char *fmt, ...),
 PS_execute(int err, const char *fmt, ...),
 PS_csetenv(const char *var, const char *fmt, ...),
 PS_cunsetenv(const char *var),
 PS_cdefenv(const char *fmt, ...),
 PS_cmpenv(const char *var, const char *val),
 PS_cinitenv(const char *var, const char *fmt, ...),
 PS_cclearenv(char **except),
 PS_blank_line(const char *s),
 PS_push_tok(char *s, int nc, int dlm, const char *fmt, ...),
 PS_push_tok_beg(char *s, int nc, int dlm, const char *fmt, ...),
 PS_push_dir(const char *fmt, ...),
 PS_pop_dir(void),
 PS_file_path(const char *name, char *path, int nc),
 PS_match(const char *s, const char *patt),
 PS_touch(const char *fmt, ...),
 PS_demonize(void),
 PS_file_strings_push(FILE *fp, char ***psa, int snl, unsigned int nbr),
 PS_strings_file(char **sa, const char *fname, const char *mode),
 PS_is_running(int pid);

extern char
 *PS_nstrsave(const char *s),
 *PS_nstrncpy(char *d, size_t nd, const char *s, size_t ns),
 *PS_nstrcat(char *d, int nc, const char *s),
 *PS_vstrcat(char *d, int nc, const char *fmt, ...),
 **PS_lst_push(char **lst, const char *fmt, ...),
 **PS_lst_add(char **lst, const char *s),
 **PS_lst_copy(char **lst),
 **PS_lst_uniq(char **lst),
 *PS_concatenate(char *s, int nc, char **sa,
		 unsigned int mn, unsigned int mx, const char *dlm),
 *PS_delimited(char *s, const char *bgn, const char *end),
 **PS_tokenize(const char *s, const char *dlm, int flags),
 **PS_tokenized(const char *s, const char *dlm, int flags),
 *PS_upcase(char *s),
 *PS_downcase(char *s),
 *PS_fill_string(char *s, int n),
 *PS_strclean(char *d, size_t nd, const char *s, size_t ns),
 *PS_strstri(char *string1, char *string2),
 *PS_subst(const char *s, const char *a, const char *b, size_t n),
 *PS_path_tail(char *s),
 *PS_path_head(char *s),
 *PS_path_base(char *s),
 *PS_path_suffix(char *s),
 *PS_path_simplify(char *s, int dlm),
 *PS_run(int echo, const char *fmt, ...),
 *PS_grep(FILE *fp, const char *name, const char *fmt, ...),
 *PS_get_date(void),
 *PS_strip_quote(char *t),
 *PS_cnoval(void),
 *PS_cgetenv(int lit, const char *fmt, ...),
 **PS_cenv(int sort, char **rej),
 *PS_cwhich(const char *fmt, ...),
 *PS_append_tok(char *s, int dlm, const char *fmt, ...),
 *PS_pop_path(char *path),
 **PS_ls(const char *opt, const char *fmt, ...),
 *PS_unique(const char *lst, int beg, int dlm),
 *PS_trim(char *s, int dir, const char *delim),
 **PS_file_strings(FILE *fp),
 **PS_file_text(int sort, const char *fname, ...);

extern FILE
 *PS_open_file(const char *mode, const char *fmt, ...);


/* LIBSOCK.C declarations */

extern int
 PS_tcp_set_port(sckades ad, int port),
 PS_tcp_bind_port(int fd, sckades ad, int port, int pmin),
 PS_tcp_accept_connection(int fd, sckades ad, int ao),
 PS_tcp_initiate_connection(sckades ad),
 PS_open_server(client *cl, ckind ioc, int auth),
 PS_close_sock(client *cl),
 PS_read_sock(client *cl, char *s, int nc),
 PS_write_sock(client *cl, const char *s, int nc);

extern char
 *PS_find_conn(const char *root, int ch),
 **PS_parse_conn(const char *root),
 **PS_get_connect_socket(client *cl);

extern sckades
 PS_tcp_get_address(const char *host, int port, in_addr_t haddr);


/* LIBSRV.C declarations */

extern void
 _PS_check_fd(srvdes *sv),
 PS_remove_fd(srvdes *sv, int fd),
 _PS_new_connection(srvdes *sv),
 PS_cl_logger(client *cl, int lvl, const char *fmt, ...),
 PS_free_client(client *cl);

extern int
 _PS_process_act(srvdes *sv, int fd),
 PS_async_server(srvdes *sv),
 PS_verifyx(client *cl, char *ans, char *res),
 PS_comm_read(client *cl, char *s, int nc, int to),
 PS_comm_write(client *cl, char *s, int nc, int to);

extern char
 *PS_name_conn(const char *root),
 *PS_name_log(const char *root),
 **PS_client_ex(client *cl, char *req);

extern client
 *PS_make_client(ckind type, int port, int auth, const char *root, 
		 void (*clog)(client *cl, int lvl, const char *fmt, ...),
		 int (*cauth)(client *cl, int nc, char *ans, char *res));


/* LIBSTACK.C declarations */

extern vstack
 *PS_make_stk(char *type, int n),
 *PS_stk_copy(vstack *stk);

extern int
 PS_stk_length(vstack *stk),
 PS_stk_push(vstack *stk, void *a);

extern void
 *PS_stk_get(vstack *stk, int i),
 *PS_stk_pop(vstack *stk),
 PS_stk_free(vstack *stk),
 PS_stk_uniq(vstack *stk),
 PS_stk_remove(vstack *stk, void *a);


/* LIBSUBST.C declarations */

extern substdes
 *PS_make_substdes(void);

extern int
 PS_polysubst_add(substdes *sd, char *key, char *val),
 PS_polysubst(substdes *sd, char *s, int nc);

extern char
 *PS_polysubst_lookup(int *plev, substdes *sd, char *key);


/* LIBTIME.C declarations */

extern double
 PS_wall_clock_time(void);

extern char
 *PS_time_string(char *ts, int nc, int fmt, double t);


/* LIBTYP.C declarations */

extern type_desc
 *PS_type_table(type_desc *td),
 *PS_lookup_type_info(const char *ty),
 *PS_resolve_type_alias(char *ty);

extern int
 PS_foreach_type(int (*f)(type_desc *td, void *a), void *a);

#ifdef __cplusplus
}
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef PCK_SCPSH

#undef PCK_ANSIC
#undef PCK_COMMON

#undef LIBASYNC
#undef LIBDB
#undef LIBEVAL
#undef LIBFIFO
#undef LIBHASH
#undef LIBSTACK
#undef LIBINFO
#undef LIBIO
#undef LIBPGRP
#undef LIBPSH
#undef LIBSOCK
#undef LIBSRV
#undef LIBSUBST
#undef LIBTIME

/*--------------------------------------------------------------------------*/

/*                                CONSTANT MACROS                           */

/*--------------------------------------------------------------------------*/

/* LIBASYNC.C macros */

#undef JOB_RUNNING
#undef JOB_STOPPED
#undef JOB_CHANGED
#undef JOB_EXITED
#undef JOB_COREDUMPED
#undef JOB_SIGNALED
#undef JOB_KILLED
#undef JOB_DEAD
#undef JOB_RESTART


/*--------------------------------------------------------------------------*/

/*                              FUNCTIONAL MACROS                           */

/*--------------------------------------------------------------------------*/

/* COMMON.H macros */

#undef MAKE
#undef MAKE_N
#undef REMAKE
#undef FREE
#undef STRSAVE
#undef LST_FREE
#undef LAST_CHAR
#undef VA_START
#undef VA_ARG
#undef VSNPRINTF
#undef VA_END
#undef IS_NULL
#undef FOREACH
#undef NEXT
#undef ENDFOR


/* LIBASYNC.H macros */

#undef job_alive
#undef job_running


/* LIBSOCK.C macros */

#undef C_OR_S
#undef CLOG


/* LIBSRV.C macros */

#undef SLOG

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
