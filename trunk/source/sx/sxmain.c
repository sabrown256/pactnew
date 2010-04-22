/*
 * SXMAIN.C - the main routine for SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"
#include "scope_raster.h"
#include "syntax.h"
#include "scope_proc.h"

#define SCODE "SX 3.0"
#define PCODE "PDBView 2.0"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start up a fun filled session of SX right here */

int main(int c, char **v, char **env)
   {int i, n, commnd_flag, tflag, pvflag, load_rc;
    int load_init, n_files, ret, zero_space, trap_error;
    int upix, script_file;
    char *cmd, *prog;
    SIGNED char order[MAXLINE];
    double evalt;

/* NOTE: be able to access remote files
 * this MUST be set before the PD_init_threads uses the current
 * io hooks to initialize the pio hooks
 */
    SC_io_connect(SC_REMOTE);

    PD_set_format_version(3);

    PD_init_threads(0, NULL);

    PG_IO_INTERRUPTS(FALSE);

    SC_init("Aborting with error", SX_end,
            TRUE, SS_interrupt_handler,
            TRUE, NULL, 0);

    cmd         = NULL;
    commnd_flag = FALSE;
    script_file = FALSE;
    pvflag      = FALSE;
    tflag       = FALSE;
    load_init   = TRUE;
    trap_error  = TRUE;
    SX_gr_mode  = TRUE;
    load_rc     = TRUE;
    zero_space  = 2;

    upix = FALSE;

#ifndef NO_SHELL

/* initialize the file order */
    n_files = 0;

/* connect the I/O functions */
    SC_set_put_line(SS_printf);
    SC_set_put_string(SS_fputs);
    SC_set_get_line(io_gets);

#ifndef AIX
# ifndef linux
    PG_IO_INTERRUPTS(TRUE);
# endif
#endif

    if (SS_exe_script(c, v) != NULL)
       {SX_qflag    = TRUE;
	script_file = TRUE;
	trap_error  = FALSE;};

/* process the command line arguments */
    for (i = 1; i < c; i++)
        if (v[i][0] == '-')
	   {switch (v[i][1])
               {case 'e' :
		     trap_error = FALSE;
		     break;
                case 'i' :                       /* IO not interrupt driven */
		     PG_IO_INTERRUPTS(FALSE);
                     break;
                case 'l' :                              /* load Scheme file */
                     order[n_files++] = ++i;
                     break;
                case 'm' :                      /* use X pixmap for drawing */
		     upix = TRUE;
                     break; 
                case 'n' :                          /* don't load init file */
                     load_init = FALSE;
                     break;
                case 'p' :                       /* do everything (PDBView) */
#ifdef AIX
                     PG_IO_INTERRUPTS(TRUE);
#endif
                     pvflag = TRUE;
                     break;
                case 'q' :               /* quite mode suppress version and */
                                         /* cannot connect to display       */
                     SX_qflag = TRUE;
                     break;
                case 'r' :                    /* don't load .pdbviewrc file */
                     load_rc = FALSE;
                     break;
                case 's' :                                       /* SX mode */
                     SX_gr_mode = FALSE;
		     PG_IO_INTERRUPTS(FALSE);
                     break;
                case 't' :                                /* time the loads */
                     tflag = TRUE;
                     break;
                case 'w' :           /* use X window for drawing not pixmap */
		     upix = FALSE;
                     break;
                case 'z' :                              
                     zero_space = SC_stoi(v[++i]);
                     break;
                default :
		     if (script_file == FALSE)
		        printf("ERROR: Bad flag %s\n", v[i]);
                     break;};}

        else if ((pvflag) && (v[i][0] != '(') && !commnd_flag)
           order[n_files++] = -i;

        else
           {commnd_flag = TRUE;
            cmd        = SC_dstrcpy(cmd, " ");
            for ( ; i < c; i++)
                {cmd = SC_dstrcat(cmd, v[i]);
                 cmd = SC_dstrcat(cmd, " ");};};

#else

/* assume we always want PDBVIEW mode if there is no shell */
    pvflag = TRUE;

#endif

    SC_zero_space(zero_space);
    SS_set_scheme_env(v[0], NULL);

/* initialize SX
 * the following variables must be initialized before SX_init
 */
    SX_console_type     = SC_strsavef("MONOCHROME",
				      "char*:SXMAIN:console_type");
    SX_console_x        = 0.0;
    SX_console_y        = 0.0;
    SX_console_width    = 0.33;
    SX_console_height   = 0.33;
    SX_background_color_white = TRUE;

/* run in PDBView mode */
    if (pvflag)
       {SX_init(PCODE, VERSION);
        SS_trap_error = trap_error;

	prog = "pdbview";

        SX_init_view();
        SX_install_global_vars();
        SX_init_mappings();
        SX_init_env();

#ifndef NO_SHELL
        if (SX_gr_mode && !SX_qflag)
	   SS_banner(SS_mk_string(PCODE));
#endif

/* load the SCHEME level PDBView functionality */
	if (load_init)
           SX_load_rc("pdbview.scm", load_rc, ".pdbviewrc", "pdbview.rc");}

/* run in vanilla SCHEME mode */
    else
       {SX_init(SCODE, VERSION);

	prog = "sx";

        SS_trap_error = trap_error;};

    SS_env_vars(env, NULL);
    SS_load_scm("nature.scm");

    PG_set_use_pixmap(upix);

/* initialize the available syntax modes */
    DEF_SYNTAX_MODES();

    if (pvflag)
       {if (SX_gr_mode)
	   SX_mode_graphics();
        else
           SX_mode_text();

        PG_expose_device(PG_console_device);};

#ifdef NO_SHELL

/* connect the I/O functions */
    if (SX_gr_mode)
        SS_banner(SS_mk_string(PCODE));

#endif

    SS_define_argv(prog, c, v);

/* read the optionally specified data/scheme files in order */
    for (i = 0; i < n_files; i++)
        {n = order[i];
	 if (n < 0)
	    SX_rd_scm(v[-n]);
	 else
            {evalt = SC_cpu_time();
	     SS_load_scm(v[n]);
	     evalt = SC_cpu_time() - evalt;

	     if (tflag)
                PRINT(STDOUT,
		      "   %s load time: (%10.3e)\n",
		      v[n], evalt);};};

    SS_nsave        = 0;
    SS_nrestore     = 0;
    SS_nsetc        = 0;
    SS_ngoc         = 0;
    SS_bracket_flag = TRUE;

    SC_mem_stats_set(0L, 0L);

    if (commnd_flag)
       ret = !SS_run(cmd);

    else
       {if (SX_gr_mode && !SX_qflag)
	   SS_banner(SS_mk_string(SCODE));
	SS_repl();

        ret = TRUE;};

    SFREE(cmd);

    return(ret);}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
