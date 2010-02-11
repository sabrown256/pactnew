;
; STATS.SCM - stats driver for PACT
;
; Source Version: 3.0
; Software Release #: LLNL-CODE-422942
;
; #include <cpyright.h>
;

; the following code specific definitions and
; load of standard stats procedures are required

(define dai-root (getenv "DAI_ROOT"))
(define codename "pact")
(define stats-directory (sprintf "%s/%s/stats" dai-root codename))
(define demo-directory stats-directory)

; Code specific list of command lists
(define code-cmd-lists
    (list
    (list '(spl Domain) '(stitle "Domain"))
;    (list '(spl Version) '(stitle "Version"))
    (list '(spl total>) '(stitle "Total"))
    (list '(splnr setup>) '(spl+ setup time !_io_) '(stitle "Setup"))
    (list '(spl setup io_time) '(stitle "Setup I/O Time"))
    (list '(spl setup mbytes recv) '(stitle "Setup I/O MBytes - Received"))
    (list '(spl setup mbytes sent) '(stitle "Setup I/O MBytes - Sent"))
    (list '(spl setup rate recv) '(stitle "Setup I/O Rate - Received"))
    (list '(spl setup rate sent) '(stitle "Setup I/O Rate - Sent"))
    (list '(spl build !_io_ time) '(stitle "Total Build"))
    (list '(spl build_g_) '(stitle "Build - Debug"))
    (list '(spl build_o_ !-s> !-t>) '(stitle "Build - Optimized"))
    (list '(spl build_o_ -s>) '(spl+ build_o_ -t>) '(stitle "Build - Optimized"))
    (list '(spl build io_time) '(stitle "Build I/O Time"))
    (list '(spl build mbytes recv) '(stitle "Build I/O MBytes - Received"))
    (list '(spl build mbytes sent) '(stitle "Build I/O MBytes - Sent"))
    (list '(spl build rate recv) '(stitle "Build I/O Rate - Received"))
    (list '(spl build rate sent) '(stitle "Build I/O Rate - Sent"))
    (list '(spl netinstall) '(stitle "Network Install"))
    (list '(spl hostinstall) '(stitle "Host Install"))
    (list '(spl percent) '(stitle "GAPPS - Percent"))
    (list '(spl quota) '(stitle "GAPPS - Quota"))
    (list '(spl usage) '(stitle "GAPPS - Usage"))
    (list '(spl retry_ok_g) '(stitle "Successful Retries - Debug"))
    (list '(spl retry_ok_o !-s> !-t>) '(stitle "Successful Retries - Optimized"))
    (list '(spl retry_ok_o -s>) '(spl+ retry_ok_o -t>) '(stitle "Successful Retries - Optimized"))
    (list '(spl retry_total_g) '(stitle "Total Retries - Debug"))
    (list '(spl retry_total_o !-s> !-t>) '(stitle "Total Retries - Optimized"))
    (list '(spl retry_total_o -s>) '(spl+ retry_total_o -t>) '(stitle "Total Retries - Optimized"))
    (list '(spl getattr) '(stitle "NFS Getattrs"))
    (list '(spl lookup) '(stitle "NFS Lookups"))
    (list '(spl ops) '(stitle "NFS Ops"))
    (list '(spl read) '(stitle "NFS Reads"))
    (list '(spl setattr) '(stitle "NFS Setattrs"))
    (list '(spl write) '(stitle "NFS Writes"))
    (list '(spl calls) '(stitle "RPC Calls"))
    (list '(spl retrans) '(stitle "RPC Retrans"))
    (list '(spl conc !-s> !-t>) '(stitle "Concurrent Test Flag"))
    (list '(spl conc -s>) '(spl+ conc -t>) '(stitle "Concurrent Test Flag"))
    (list '(spl test_total_g) '(stitle "Test - Debug"))
    (list '(spl test_total_o !-s> !-t>) '(stitle "Test - Optimized"))
    (list '(spl test_total_o -s>) '(spl+ test_total_o -t>) '(stitle "Test - Optimized"))
    (list '(spl test _score>) '(stitle "Test score"))
    (list '(spl test _pml>) '(stitle "Test pml"))
    (list '(spl test _ppc>) '(stitle "Test ppc"))
    (list '(spl test _pdb>) '(stitle "Test pdb"))
    (list '(spl test _pgs>) '(stitle "Test pgs"))
    (list '(spl test _panacea>) '(stitle "Test panacea"))
    (list '(spl test _scheme>) '(stitle "Test scheme"))
    (list '(spl test _sx>) '(stitle "Test sx"))
    (list '(spl test _ultra>) '(stitle "Test ultra"))
    ))

(load "slib.scm")

; the following code specific definitions
; and loading of demos are optional

;(load "sdemo.scm")

; codehelp - code specific help
;(define (codehelp)
;    (printf nil "Additional commands available in %s stats mode:\n\n" codename)
;    (printf nil "")
;    (printf nil "Replace this line with code specific command documentation")
;    (printf nil "\n"))
