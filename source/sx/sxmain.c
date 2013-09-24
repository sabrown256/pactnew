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
#include "scope_proc.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start up a fun filled session of SX right here */

int main(int c, char **v, char **env)
   {int i, n, commnd_flag, tflag, load_rc;
    int load_init, n_files, ret, zsp, trap_error;
    int upix, script_file;
    char *cmd;
    SIGNED char order[MAXLINE];
    double evalt;
    SS_psides *si;

    PD_register_sql();
    PD_register_hdf5();

    PD_set_fmt_version(3);

    si = SX_init(SCODE, VERSION, c, v, env);

    SS_init(si, "Aborting with error", SX_end,
            TRUE, SS_interrupt_handler,
            TRUE, NULL, 0);

/* NOTE: be able to access remote files
 * this MUST be set before the PD_init_threads uses the current
 * io hooks to initialize the pio hooks
 */
    SC_io_connect(SC_REMOTE);

    PD_init_threads(0, NULL);

    SC_set_io_interrupts(FALSE);

    cmd           = NULL;
    commnd_flag   = FALSE;
    script_file   = FALSE;
    tflag         = FALSE;
    load_init     = TRUE;
    trap_error    = TRUE;
    SX_gs.gr_mode = TRUE;
    load_rc       = TRUE;
    zsp           = 2;

    upix = FALSE;

/* initialize the file order */
    n_files = 0;

#ifndef NO_SHELL

/* connect the I/O functions */
    SS_set_put_line(si, SS_printf);
    SS_set_put_string(si, SS_fputs);
    SC_set_get_line(io_gets);

#ifndef AIX
# ifndef linux
    SC_set_io_interrupts(TRUE);
# endif
#endif

    if (SS_exe_script(c, v) != NULL)
       {SX_gs.qflag = TRUE;
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
		     SC_set_io_interrupts(FALSE);
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
                     SC_set_io_interrupts(TRUE);
#endif
		     SX_gs.sm = SX_MODE_PDBVIEW;
                     break;
                case 'q' :               /* quite mode suppress version and */
                                         /* cannot connect to display       */
                     SX_gs.qflag = TRUE;
                     break;
                case 'r' :                    /* don't load .pdbviewrc file */
                     load_rc = FALSE;
                     break;
                case 's' :                                       /* SX mode */
                     SX_gs.gr_mode = FALSE;
		     SC_set_io_interrupts(FALSE);
                     break;
                case 't' :                                /* time the loads */
                     tflag = TRUE;
                     break;
                case 'w' :           /* use X window for drawing not pixmap */
		     upix = FALSE;
                     break;
                case 'z' :                              
                     zsp = SC_stoi(v[++i]);
                     break;
                default :
		     if (script_file == FALSE)
		        printf("ERROR: Bad flag %s\n", v[i]);
                     break;};}

        else if ((SX_gs.sm == SX_MODE_PDBVIEW) &&
		 (v[i][0] != '(') && !commnd_flag)
           order[n_files++] = -i;

        else
           {commnd_flag = TRUE;
            cmd = SC_dstrcpy(cmd, " ");
            for ( ; i < c; i++)
                {cmd = SC_dstrcat(cmd, v[i]);
                 cmd = SC_dstrcat(cmd, " ");};};

#else

/* assume we always want PDBVIEW mode if there is no shell */
    SX_gs.sm = SX_MODE_PDBVIEW;

#endif

    si->trap_error   = trap_error;
    si->bracket_flag = TRUE;

    SC_zero_space_n(zsp, -2);

    PG_fset_use_pixmap(upix);

/* initialize SX
 * the following variables must be initialized before SX_init
 */
    SX_gs.console_type           = CSTRSAVE("MONOCHROME");
    SX_gs.console_x[0]           = 0.0;
    SX_gs.console_x[1]           = 0.0;
    SX_gs.console_dx[0]          = 0.33;
    SX_gs.console_dx[1]          = 0.33;
    SX_gs.background_color_white = TRUE;

/* run in PDBView mode */
    if (SX_gs.sm == SX_MODE_PDBVIEW)
       SX_mode_pdbview(si, load_init, load_rc);

/* run in vanilla SCHEME mode */
    else
       {SC_set_banner(" %s  -  %s\n\n", SCODE, VERSION);
	SS_load_scm(si, "nature.scm");};

#ifdef NO_SHELL

    if (SX_gs.gr_mode)
        SS_banner(si, SS_mk_string(si, PCODE));

#endif

/* read the optionally specified data/scheme files in order */
    for (i = 0; i < n_files; i++)
        {n = order[i];
	 if (n < 0)
	    SX_rd_scm(si, v[-n]);
	 else
            {evalt = SC_cpu_time();
	     SS_load_scm(si, v[n]);
	     evalt = SC_cpu_time() - evalt;

	     if (tflag)
                PRINT(STDOUT,
		      "   %s load time: (%10.3e)\n",
		      v[n], evalt);};};

    if (commnd_flag)
       ret = !SS_run(si, cmd);

    else
       {if ((SX_gs.gr_mode == TRUE) && (SX_gs.qflag == FALSE))
	   SS_banner(si, SS_mk_string(si, SCODE));

	SS_repl(si);

        ret = TRUE;};

    CFREE(cmd);

    return(ret);}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
