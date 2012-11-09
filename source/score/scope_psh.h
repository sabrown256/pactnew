/*
 * SCOPE_PSH.H - header defining PSH wrapper scope
 *
 */

#ifndef PCK_SCOPE_PSH

#define PCK_SCOPE_PSH

#include "cpyright.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define SCOPE_SCORE
#define SCOPE_SCORE_PREPROC

#include <network.h>
#include <shell/libio.c>
#include <shell/libpsh.c>
#include <shell/libhash.c>
#include <shell/libtime.c>
#include <shell/libasync.c>
#include <shell/libeval.c>
#include <shell/libsock.c>
#include <shell/libsrv.c>
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
 _PS_dbg(unsigned int lvl, char *fmt, ...),
 _PS_job_io_close(process *pp, io_kind knd),
 _PS_job_grp_attr(process *pp, int g, int t),
 _PS_default_iodes(iodes *pio),
 _PS_init_process(process *pp),
 _PS_job_child_prelim(process *pp),
 PS_job_wait(process *pp),
 PS_asetup(int n, int na),
 PS_amap(void (*f)(process *pp, void *a)),
 PS_afin(void (*f)(process *pp, void *a));

extern int
 _PS_fd_close(int fd),
 _PS_ioc_fd(int fd, io_kind k),
 _PS_do_rlimit(char *vr),
 _PS_job_waitr(int pid, int *pw, int opt, struct rusage *pr),
 *PS_list_fds(process_group *pg),
 PS_job_read(int fd, process *pp, int (*out)(int fd, process *pp, char *s)),
 PS_job_write(process *pp, char *fmt, ...),
 PS_job_poll(process *pp, int to),
 PS_job_response(process *pp, int to, char *fmt, ...),
 PS_job_signal(process *pp, int sig),
 PS_job_done(process *pp, int sig),
 PS_apoll(int to),
 PS_acheck(void),
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

extern process_group_state
 *PS_get_process_group_state(void);



/* LIBDB.C declarations */

extern void
 PS_reset_db(database *db),
 PS_load_db(database *db, char *vr, FILE *fp),
 PS_free_db(database *db),
 PS_db_srv_close(database *db);

extern int
 PS_dbset(client *cl, char *var, char *fmt, ...),
 PS_dbdef(client *cl, char *fmt, ...),
 PS_dbcmp(client *cl, char *var, char *val),
 PS_dbcmd(client *cl, char *cmd),
 PS_dbinitv(client *cl, char *var, char *fmt, ...),
 PS_dbrestore(client *cl, char *dname),
 PS_save_db(database *db, char **vars, FILE *fp, const char *fmt),
 PS_db_srv_open(client *cl, int init, int dbg, int auth),
 PS_db_srv_save(int fd, database *db),
 PS_db_srv_restart(database *db);

extern char
 *PS_name_db(char *root),
 **_PS_db_clnt_ex(client *cl, int init, char *req),
 *PS_dbget(client *cl, int lit, char *fmt, ...),
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
 *PS_expand(char *expr, int nc, char *varn, int rnull),
 *PS_eval(char *expr, int nc, char *varn);


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
 PS_report_info(char *root, int cmpl, int ltrl, itarget tgt, char *ptrn),
 PS_build_makefile(char *root, char *arch, char *mkfile, int vrb);


/* LIBIO.C declarations */

extern int
 PS_ring_clear(io_ring *ring),
 PS_ring_init(io_ring *ring, unsigned int nb),
 PS_ring_ready(io_ring *ring, unsigned char ls),
 PS_ring_push(io_ring *ring, char *s, int nc),
 PS_ring_pop(io_ring *ring, char *s, int nc, unsigned int ls);


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
 PS_register_io_pgrp(process_group *pg),
 PS_job_foreground(process_session *ps, process *pp, int cont),
 PS_job_background(process_session *ps, process *pp, int cont);

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
 PS_gexec(char *db, int c, char **v, char **env, PFPCAL (*map)(char *s)),
 PS_transfer_ff(FILE *fi, FILE *fo),
 PS_gexec_var(char *db, io_mode md, FILE **fio,
	      char *name, int c, char **v),
 PS_gexec_file(char *db, io_mode md, FILE **fio,
	       char *name, int c, char **v);

extern char
 *_PS_name_io(io_kind k),
 **PS_subst_syntax(char **sa);

extern statement
 *PS_parse_statement(char *s, char **env, char *shell,
		     PFPCAL (*map)(char *s));

extern process_session
 *PS_init_session(void);


/* LIBPSH.C declarations */

extern void
 PS_string_sort(char **v, int n),
 PS_lst_free(char **lst),
 PS_free_strings(char **lst),
 PS_cat(FILE *out, size_t nskip, size_t ncat, char *fmt, ...),
 PS_copy(char *out, char *fmt, ...),
 PS_nsleep(int ms),
 PS_separator(FILE *fp),
 PS_note(FILE *fp, int nl, char *fmt, ...),
 PS_noted(FILE *fp, char *fmt, ...),
 PS_print_text(FILE *fp, char *fmt, ...),
 PS_clean_space(char *s),
 PS_splice_out_path(char *path),
 PS_push_path(int end, char *dpath, char *path),
 PS_build_path(char *base, ...),
 PS_key_val(char **key, char **val, char *s, char *dlm),
 PS_log_activity(char *flog, int ilog, int ilev, char *oper, char *fmt, ...),
 PS_unamef(char *s, int nc, char *wh);

extern int
 PS_lst_length(char **lst),
 PS_last_char(char *s),
 PS_nchar(char *s, int c),
 PS_full_path(char *path, int nc, char *dir, char *name),
 PS_dir_exists(char *fmt, ...),
 PS_file_exists(char *fmt, ...),
 PS_file_executable(char *fmt, ...),
 PS_file_script(char *fmt, ...),
 PS_execute(int err, char *fmt, ...),
 PS_csetenv(char *var, char *fmt, ...),
 PS_cunsetenv(char *var),
 PS_cdefenv(char *fmt, ...),
 PS_cmpenv(char *var, char *val),
 PS_cinitenv(char *var, char *fmt, ...),
 PS_cclearenv(char **except),
 PS_blank_line(char *s),
 PS_push_tok(char *s, int nc, int dlm, char *fmt, ...),
 PS_push_tok_beg(char *s, int nc, int dlm, char *fmt, ...),
 PS_push_dir(char *fmt, ...),
 PS_pop_dir(void),
 PS_file_path(char *name, char *path, int nc),
 PS_match(char *s, char *patt),
 PS_touch(char *fmt, ...),
 PS_block_fd(int fd, int on),
 PS_unlink_safe(char *s),
 PS_demonize(void),
 PS_file_strings_push(FILE *fp, char ***psa, int snl, unsigned int nbr),
 PS_strings_file(char **sa, char *fname, char *mode),
 PS_is_running(int pid);

extern size_t
 PS_fwrite_safe(void *s, size_t bpi, size_t nitems, FILE *fp);

extern char
 *PS_nstrsave(char *s),
 *PS_nstrncpy(char *d, size_t nd, char *s, size_t ns),
 *PS_nstrcat(char *d, int nc, char *s),
 *PS_vstrcat(char *d, int nc, char *fmt, ...),
 **PS_lst_push(char **lst, char *fmt, ...),
 **PS_lst_add(char **lst, char *s),
 **PS_lst_copy(char **lst),
 **PS_lst_uniq(char **lst),
 *PS_concatenate(char *s, int nc, char **sa, char *dlm),
 *PS_delimited(char *s, char *bgn, char *end),
 **PS_tokenize(char *s, char *dlm),
 **PS_tokenized(char *s, char *dlm),
 *PS_upcase(char *s),
 *PS_downcase(char *s),
 *PS_fill_string(char *s, int n),
 *PS_strclean(char *d, size_t nd, char *s, size_t ns),
 *PS_strstri(char *string1, char *string2),
 *PS_subst(char *s, char *a, char *b, size_t n),
 *PS_path_tail(char *s),
 *PS_path_head(char *s),
 *PS_path_base(char *s),
 *PS_path_suffix(char *s),
 *PS_path_simplify(char *s, int dlm),
 *PS_run(int echo, char *fmt, ...),
 *PS_grep(FILE *fp, char *name, char *fmt, ...),
 *PS_get_date(void),
 *PS_strip_quote(char *t),
 *PS_cnoval(void),
 *PS_cgetenv(int lit, char *fmt, ...),
 **PS_cenv(int sort, char **rej),
 *PS_cwhich(char *fmt, ...),
 *PS_append_tok(char *s, int dlm, char *fmt, ...),
 *PS_pop_path(char *path),
 **PS_ls(char *opt, char *fmt, ...),
 *PS_unique(char *lst, int beg, int dlm),
 *PS_trim(char *s, int dir, char *delim),
 **PS_file_strings(FILE *fp),
 **PS_file_text(int sort, char *fname, ...);

extern FILE
 *PS_open_file(char *mode, char *fmt, ...);


/* LIBSOCK.C declarations */

extern int
 PS_tcp_set_port(sckades ad, int port),
 PS_tcp_bind_port(int fd, sckades ad, int port, int pmin),
 PS_tcp_accept_connection(int fd, sckades ad, int ao),
 PS_tcp_initiate_connection(int fd, sckades ad),
 PS_open_server(client *cl, ckind ioc, int auth),
 PS_close_sock(client *cl),
 PS_read_sock(client *cl, char *s, int nc),
 PS_write_sock(client *cl, char *s, int nc);

extern char
 **PS_parse_conn(char *root),
 **PS_get_connect_socket(client *cl);

extern sckades
 PS_tcp_get_address(char *host, int port, in_addr_t haddr);


/* LIBSRV.C declarations */

extern void
 PS_cl_logger(client *cl, int lvl, char *fmt, ...),
 PS_async_server(srvdes *sv),
 PS_free_client(client *cl);

extern int
 PS_verifyx(client *cl, char *ans, char *res),
 PS_comm_read(client *cl, char *s, int nc, int to),
 PS_comm_write(client *cl, char *s, int nc, int to);

extern char
 *PS_name_conn(char *root),
 *PS_name_log(char *root),
 **PS_client_ex(client *cl, char *req);

extern client
 *PS_make_client(ckind type, int port, int auth, char *root, 
		 void (*clog)(client *cl, int lvl, char *fmt, ...));


/* LIBTIME.C declarations */

extern double
 PS_wall_clock_time(void);

extern char
 *PS_time_string(char *ts, int nc, int fmt, double t);


#ifdef __cplusplus
}
#endif

#endif
