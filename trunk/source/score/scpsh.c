/*
 * SCPSH.C - expose PSH routines to SCORE and up
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

/* variables */
#define _assert_fail            _PS_assert_fail

/* debugging functions */
#define dprdio			_PS_dprdio
#define dprpio			_PS_dprpio
#define dprgrp			_PS_dprgrp
#define dprgio			_PS_dprgio

/* non-API functions */
#define _block_all_sig 		_PS_block_all_sig
#define _db_clnt_ex 		_PS_db_clnt_ex
#define _db_srv_launch 		_PS_db_srv_launch
#define _dbg 			_PS_dbg
#define _default_iodes 		_PS_default_iodes
#define _deref_close 		_PS_deref_close
#define _deref_close_fan_in 	_PS_deref_close_fan_in
#define _deref_close_fan_out 	_PS_deref_close_fan_out
#define _deref_io 		_PS_deref_io
#define _deref_job_close 	_PS_deref_job_close
#define _do_rlimit 		_PS_do_rlimit
#define _fd_close 		_PS_fd_close
#define _fnc_wait 		_PS_fnc_wait
#define _init_process 		_PS_init_process
#define _io_file_ptr 		_PS_io_file_ptr
#define _io_kind 		_PS_io_kind
#define _io_mode 		_PS_io_mode
#define _ioc_fd 		_PS_ioc_fd
#define _ioc_pair 		_PS_ioc_pair
#define _job_child_prelim 	_PS_job_child_prelim
#define _job_grp_attr 		_PS_job_grp_attr
#define _job_io_close 		_PS_job_io_close
#define _job_waitr 		_PS_job_waitr
#define _kind_io 		_PS_kind_io
#define _name_io 		_PS_name_io
#define _nsigaction 		_PS_nsigaction
#define _pgrp_accept 		_PS_pgrp_accept
#define _pgrp_data_child 	_PS_pgrp_data_child
#define _pgrp_fan_in 		_PS_pgrp_fan_in
#define _pgrp_fan_out 		_PS_pgrp_fan_out
#define _pgrp_fin 		_PS_pgrp_fin
#define _pgrp_reject 		_PS_pgrp_reject
#define _pgrp_send 		_PS_pgrp_send
#define _pgrp_tty 		_PS_pgrp_tty
#define _pgrp_wait 		_PS_pgrp_wait
#define _pgrp_work 		_PS_pgrp_work
#define _post_info 		_PS_post_info

/* API functions */

#define close_safe 		PS_close_safe
#define fclose_safe 		PS_fclose_safe
#define fflush_safe 		PS_fflush_safe
#define fopen_safe 		PS_fopen_safe
#define fread_safe 		PS_fread_safe
#define fwrite_safe 		PS_fwrite_safe
#define log_safe 		PS_log_safe
#define open_safe 		PS_open_safe
#define read_safe               PS_read_safe
#define unlink_safe 		PS_unlink_safe
#define write_safe              PS_write_safe

#define finlog			PS_finlog
#define initlog			PS_initlog
#define note 			PS_note
#define noted 			PS_noted
#define cat 			PS_cat
#define separator 		PS_separator

#define acheck 			PS_acheck
#define afin 			PS_afin
#define alaunch 		PS_alaunch
#define amap 			PS_amap
#define apoll 			PS_apoll
#define append_tok 		PS_append_tok
#define arelaunch 		PS_arelaunch
#define asetup 			PS_asetup
#define async_server 		PS_async_server
#define await 			PS_await
#define blank_line 		PS_blank_line
#define block_fd 		PS_block_fd
#define build_makefile 		PS_build_makefile
#define build_path 		PS_build_path
#define cclearenv 		PS_cclearenv
#define cdefenv 		PS_cdefenv
#define cenv 			PS_cenv
#define cgetenv 		PS_cgetenv
#define cinitenv 		PS_cinitenv
#define cl_logger 		PS_cl_logger
#define clean_space 		PS_clean_space
#define client_ex 		PS_client_ex
#define close_parent_child 	PS_close_parent_child
#define close_sock 		PS_close_sock
#define cmpenv 			PS_cmpenv
#define cnoval 			PS_cnoval
#define comm_read 		PS_comm_read
#define comm_write 		PS_comm_write
#define concatenate 		PS_concatenate
#define connect_child_in_out 	PS_connect_child_in_out
#define connect_child_out_in 	PS_connect_child_out_in
#define copy 			PS_copy
#define count_fan_in 		PS_count_fan_in
#define count_fan_out 		PS_count_fan_out
#define csetenv 		PS_csetenv
#define cunsetenv 		PS_cunsetenv
#define cwhich 			PS_cwhich
#define db_srv_close 		PS_db_srv_close
#define db_srv_create 		PS_db_srv_create
#define db_srv_load 		PS_db_srv_load
#define db_srv_open 		PS_db_srv_open
#define db_srv_restart 		PS_db_srv_restart
#define db_srv_save 		PS_db_srv_save
#define dbcmd 			PS_dbcmd
#define dbcmp 			PS_dbcmp
#define dbdef 			PS_dbdef
#define dbget 			PS_dbget
#define dbinitv 		PS_dbinitv
#define dbrestore 		PS_dbrestore
#define dbset 			PS_dbset
#define delimited 		PS_delimited
#define demonize 		PS_demonize
#define dir_exists 		PS_dir_exists
#define downcase 		PS_downcase
#define eval 			PS_eval
#define execute 		PS_execute
#define expand 			PS_expand
#define file_executable 	PS_file_executable
#define file_exists 		PS_file_exists
#define file_path 		PS_file_path
#define file_script 		PS_file_script
#define file_strings 		PS_file_strings
#define file_strings_push	PS_file_strings_push
#define file_text 		PS_file_text
#define fill_string 		PS_fill_string
#define fillin_pgrp 		PS_fillin_pgrp
#define free_client 		PS_free_client
#define free_db 		PS_free_db
#define free_strings 		PS_free_strings
#define full_path 		PS_full_path
#define get_connect_socket 	PS_get_connect_socket
#define get_date 		PS_get_date
#define get_db 			PS_get_db
#define get_multi_line 		PS_get_multi_line
#define get_process_group_state PS_get_process_group_state
#define gexeca 			PS_gexeca
#define gexecs			PS_gexecs
#define gexec_file 		PS_gexec_file
#define gexec_var 		PS_gexec_var
#define gpoll 			PS_gpoll
#define grep 			PS_grep
#define make_hash_table		PS_make_hash_table
#define hash_remove		PS_hash_remove
#define hash_clear		PS_hash_clear
#define hash_free		PS_hash_free
#define hash_lookup		PS_hash_lookup
#define hash_def_lookup		PS_hash_def_lookup
#define hash_install		PS_hash_install
#define hash_foreach		PS_hash_foreach
#define hash_dump		PS_hash_dump
#define init_session 		PS_init_session
#define is_running 		PS_is_running
#define job_background 		PS_job_background
#define job_done 		PS_job_done
#define job_foreground 		PS_job_foreground
#define job_launch 		PS_job_launch
#define job_poll 		PS_job_poll
#define job_read 		PS_job_read
#define job_response 		PS_job_response
#define job_signal 		PS_job_signal
#define job_wait 		PS_job_wait
#define job_write 		PS_job_write
#define key_val 		PS_key_val
#define last_char 		PS_last_char
#define list_fds 		PS_list_fds
#define load_db 		PS_load_db
#define log_activity 		PS_log_activity
#define ls 			PS_ls
#define lst_add 		PS_lst_add
#define lst_copy 		PS_lst_copy
#define lst_free 		PS_lst_free
#define lst_length 		PS_lst_length
#define lst_push 		PS_lst_push
#define lst_uniq 		PS_lst_uniq
#define make_client 		PS_make_client
#define match 			PS_match
#define name_db 		PS_name_db
#define name_log 		PS_name_log
#define name_conn 		PS_name_conn
#define nsleep 			PS_nsleep
#define nstrcat 		PS_nstrcat
#define nstrncpy 		PS_nstrncpy
#define nstrsave 		PS_nstrsave
#define open_file 		PS_open_file
#define open_server 		PS_open_server
#define parse_conn 		PS_parse_conn
#define parse_statement 	PS_parse_statement
#define path_base 		PS_path_base
#define path_head 		PS_path_head
#define path_simplify 		PS_path_simplify
#define path_suffix 		PS_path_suffix
#define path_tail 		PS_path_tail
#define pop_dir 		PS_pop_dir
#define pop_path 		PS_pop_path
#define print_text 		PS_print_text
#define prune_env 		PS_prune_env
#define push_dir 		PS_push_dir
#define push_path 		PS_push_path
#define push_tok 		PS_push_tok
#define push_tok_beg 		PS_push_tok_beg
#define put_db 			PS_put_db
#define read_sock 		PS_read_sock
#define redirect_io 		PS_redirect_io
#define register_io_pgrp 	PS_register_io_pgrp
#define report_info 		PS_report_info
#define reset_db 		PS_reset_db
#define ring_clear 		PS_ring_clear
#define ring_init 		PS_ring_init
#define ring_pop 		PS_ring_pop
#define ring_push 		PS_ring_push
#define ring_ready 		PS_ring_ready
#define run 			PS_run
#define save_db 		PS_save_db
#define splice_out_path 	PS_splice_out_path
#define strclean 		PS_strclean
#define strcntc                 PS_strcntc
#define strcnts                 PS_strcnts
#define strcpy_tok              PS_strcpy_tok
#define string_sort 		PS_string_sort
#define strings_file 		PS_strings_file
#define strip_quote 		PS_strip_quote
#define strstri 		PS_strstri
#define subst 			PS_subst
#define subst_syntax 		PS_subst_syntax
#define time_string 		PS_time_string
#define tcp_accept_connection	PS_tcp_accept_connection
#define tcp_bind_port 		PS_tcp_bind_port
#define tcp_get_address 	PS_tcp_get_address
#define tcp_set_port 		PS_tcp_set_port
#define tcp_initiate_connection	PS_tcp_initiate_connection
#define tokenize 		PS_tokenize
#define tokenized 		PS_tokenized
#define touch 			PS_touch
#define transfer_ff 		PS_transfer_ff
#define transfer_fnc_child 	PS_transfer_fnc_child
#define transfer_in 		PS_transfer_in
#define trim 			PS_trim
#define unamef 			PS_unamef
#define unique 			PS_unique
#define upcase 			PS_upcase
#define verifyx 		PS_verifyx
#define vstrcat 		PS_vstrcat
#define wall_clock_time 	PS_wall_clock_time
#define write_sock 		PS_write_sock

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define PCK_SCPSH

#define DEFINE_MSW_FUNCS

#include "score_int.h" 

/* include the PSH files with SCOPE_SCORE_COMPILE to get in
 * function and variable definitions
 * this one time right here
 * the macro definitions and function and variable declarations
 * were brought in in scope_psh.h
 */

#define SCOPE_SCORE_COMPILE

#undef SCOPE_SCORE_PREPROC
#undef LIBIO
#undef LIBFIO
#undef LIBLOG
#undef LIBPSH
#undef LIBHASH
#undef LIBTIME
#undef LIBASYNC
#undef LIBEVAL
#undef LIBSOCK
#undef LIBSRV
#undef LIBDB
#undef LIBPGRP
#undef LIBINFO

#undef MAKE
#undef MAKE_N
#undef REMAKE
#undef FREE
#undef STRSAVE

#define MAKE(_t)             CMAKE(_t)
#define MAKE_N(_t, _n)       CMAKE_N(_t, _n)
#define REMAKE(_p, _t, _n)   CREMAKE(_p, _t, _n)
#define FREE(_p)             CFREE(_p)
#define STRSAVE(_s)          CSTRSAVE(_s)

#include <network.h>
#include <shell/libio.c>
#include <shell/libfio.c>
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

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
