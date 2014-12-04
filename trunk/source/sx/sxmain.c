/*
 * SXMAIN.C - the main routine for SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "scope_ultra.h"
#include "scope_raster.h"
#include "scope_proc.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_WHICH_MODE - decide whether we are in SCHEME, SX,
 *               - ULTRA, or PDBView mode
 */

SX_session_mode SX_which_mode(int c, char **v)
   {int i;
    SX_session_mode rv;

    rv = SX_MODE_SCHEME;

/* process the command line arguments */
    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-p") == 0)
	    rv = SX_MODE_PDBVIEW;
	 else if (strcmp(v[i], "-u") == 0)
	    rv = SX_MODE_ULTRA;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start up a fun filled session of SX right here */

int main(int c, char **v, char **env)
   {int i, n, commnd_flag, tflag, load_rc;
    int load_init, n_files, ret, zsp;
    int upix, script_file, track;
    char *cmd, *code;
    SIGNED char order[MAXLINE];
    double evalt;
    SS_psides *si;
    void (*err)(SS_psides *si, int err);

    PD_register_sql();
    PD_register_hdf5();

    PD_set_fmt_version(3);

    SX_gs.sm = SX_which_mode(c, v);

    switch (SX_gs.sm)
       {case SX_MODE_ULTRA :
	     code = "ULTRA";
	     err  = _UL_quit;
	     si   = UL_init(code, VERSION, c, v, env);
	     break;

        case SX_MODE_PDBVIEW :
        case SX_MODE_SX :
	     code = "PDBView";
	     err  = SX_end;
	     si   = SX_init(code, VERSION, c, v, env);
	     break;

        default :
        case SX_MODE_SCHEME :
	     code = "SX";
	     err  = SX_end;
	     si   = SX_init(code, VERSION, c, v, env);
	     break;};

    SS_init(si, "Aborting with error", err,
	    TRUE, SS_interrupt_handler,
	    TRUE, NULL, 0);

/* NOTE: be able to access remote files
 * this MUST be set before the PD_init_threads uses the current
 * io hooks to initialize the pio hooks
 */
    SC_io_connect(SC_REMOTE);

    PD_init_threads(0, NULL);

    SC_set_io_interrupts(FALSE);

    cmd         = NULL;
    commnd_flag = FALSE;
    script_file = FALSE;
    tflag       = FALSE;
    load_init   = TRUE;
    load_rc     = TRUE;
    track       = TRUE;
    zsp         = 2;
    upix        = FALSE;

/* initialize the file order */
    n_files = 0;

    SX_gs.gr_mode = TRUE;
    SX_gs.qflag   = FALSE;

    si->trap_error = TRUE;

/* connect the I/O functions */
    SS_set_put_line(si, SS_printf);
    SS_set_put_string(si, SS_fputs);
    SC_set_get_line(io_gets);

#if !defined(AIX) && !defined(linux)
    SC_set_io_interrupts(TRUE);
#endif

    if (SS_exe_script(c, v) != NULL)
       {SX_gs.qflag = TRUE;
	script_file = TRUE;
	si->trap_error = FALSE;};

/* process the command line arguments */
    for (i = 1; i < c; i++)
        if (strcmp(v[i], "-pid") == 0)
	   SX_cache_addpid();                     /* curves-PID.a */

        else if (v[i][0] == '-')
	   {switch (v[i][1])
               {case 'e' :
		     si->trap_error = FALSE;
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
                     break;
                case 'q' :                      /* don't display the banner */
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
                case 'u' :                                    /* ULTRA mode */
                     SX_gs.gr_mode = TRUE;
		     SC_set_io_interrupts(FALSE);
                     break;
                case 'w' :           /* use X window for drawing not pixmap */
		     upix = FALSE;
                     break;
		case 'x' :
                     track = FALSE;
                     break;
                case 'z' :                              
                     zsp = SC_stoi(v[++i]);
                     break;
                default :
		     if (script_file == FALSE)
		        printf("ERROR: Bad flag %s\n", v[i]);
                     break;};}

        else if (((SX_gs.sm == SX_MODE_PDBVIEW) || (SX_gs.sm == SX_MODE_ULTRA)) &&
		 (v[i][0] != '(') && !commnd_flag)
           order[n_files++] = -i;

        else
           {commnd_flag = TRUE;
            cmd = SC_dstrcpy(cmd, " ");
            for ( ; i < c; i++)
                {cmd = SC_dstrcat(cmd, v[i]);
                 cmd = SC_dstrcat(cmd, " ");};};

    si->bracket_flag = TRUE;

    SC_zero_space_n(zsp, -2);

    PG_fset_use_pixmap(upix);

    SX_gs.background_color_white = TRUE;

    switch (SX_gs.sm)
       {case SX_MODE_ULTRA :
	     SX_mode_ultra(si, load_init, load_rc, track);
	     break;
        case SX_MODE_PDBVIEW :
	     SX_mode_pdbview(si, load_init, load_rc);
	     break;
        default :
	     SS_load_scm(si, "nature.scm");
	     break;};

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

    si->nsave    = 0;
    si->nrestore = 0;
    si->nsetc    = 0;
    si->ngoc     = 0;

    SC_mem_stats_set(0L, 0L);

    if (commnd_flag)
       ret = !SS_run(si, cmd);

    else
       {if (SX_gs.qflag == FALSE)
	   SS_banner(si, SS_mk_string(si, code));

	SS_repl(si);

        ret = TRUE;};

    CFREE(cmd);

    return(ret);}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
