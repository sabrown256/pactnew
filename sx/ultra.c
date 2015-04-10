/*
 * ULTRA.C - the portable presentation, analysis and manipulation tool
 *
 * Source Version: 4.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scope_ultra.h"
#include "scope_raster.h"
#include "scope_proc.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP_ULTRA - print help */

static void help_ultra(void)
   {

    printf("\n");
    printf("Usage: ultra [-e] [-h] [-i] [-l <file>]* [-m] [-n] [-p] [-q] [-r] [-s] [-u] [-w]\n");
    printf("             <file> | (<scheme-command>)\n");
    printf("\n");

    printf("Options:\n");
    printf("    -e     abort on error instead of returning to the interpreter\n");
    printf("    -h     this help message\n");
    printf("    -i     do not use interrupt driven I/O\n");
    printf("    -l     load file containing commands (Scheme or C syntax)\n");
    printf("    -m     on X systems do drawing to pixmap and then blt\n");
    printf("    -n     do not load the standard interpreted command files\n");
    printf("    -p     add PID to the cache filename\n");
    printf("    -q     do not display the ULTRA banner\n");
    printf("    -r     do not load the .ultrarc file\n");
    printf("    -s     start up in command mode with no graphics window\n");
    printf("    -u     start up in graphics mode with a window (default)\n");
    printf("    -w     on X systems do all drawing directly to the window\n");
    printf("\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start up a fun filled session of ULTRA right here */

int main(int c, char **v, char **env)
   {int i, n, commnd_flag, tflag, load_rc;
    int load_init, n_files, ret, zsp;
    int upix;
    int track;
    SIGNED int order[4096];
    char *cmd;
    double evalt;
    SS_psides *si;

    PD_set_fmt_version(3);

    SC_init_path(1, "ULTRA");

    si = SS_init_scheme(CODE, VERSION, c, v, env, TRUE);

    SS_init(si, "Aborting with error", _UL_quit,
            TRUE, SS_interrupt_handler,
            FALSE, NULL, 0);

/* NOTE: be able to access remote files
 * this MUST be set before the PD_init_threads uses the current
 * io hooks to initialize the pio hooks
 */
    SC_io_connect(SC_REMOTE);

    PD_init_threads(0, NULL);

    SC_set_io_interrupts(FALSE);

    cmd         = NULL;
    commnd_flag = FALSE;
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

/* ULTRA initializations not depending on scheme */
    UL_init_view(si);
    UL_init_hash();
    UL_install_global_vars(si);
    UL_install_funcs(si);

/* ULTRA initializations depending on scheme */
    UL_install_scheme_funcs(si);
    UL_init_curves(si);

    UL_init_env(si);

/* process the command line arguments */
    for (i = 1; i < c; i++)
        if (v[i][0] == '-')
           {switch (v[i][1])
               {case 'e' :
		     si->trap_error = FALSE;
		     break;
		case 'h' :
		     help_ultra();
		     return(1);
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
                case 'p' :     
                     SX_cache_addpid();                     /* curves-PID.a */
                     break;
                case 'q' :                      /* don't display the banner */
                     SX_gs.qflag = TRUE;
                     break;
                case 'r' :                           /* don't load .rc file */
                     load_rc   = FALSE;
                     break;
                case 's' :                                   /* Scheme mode */
                     SX_gs.gr_mode = FALSE;
		     SC_set_io_interrupts(FALSE);
                     break;
                case 't' :                                /* time the loads */
                     tflag = TRUE;
                     break;
                case 'u' :                                    /* Ultra mode */
                     SX_gs.sm      = SX_MODE_ULTRA;
                     SX_gs.gr_mode = TRUE;
                     break;
                case 'w' :           /* use X window for drawing not pixmap */
		     upix = FALSE;
                     break;
		case 'x' :
                     track = FALSE;
                     break;
                case 'z' :                              
                     zsp = SC_stoi(v[++i]);
                     break;};}

        else if ((v[i][0] != '(') && !commnd_flag)
	   order[n_files++] = -i;

        else if (v[i][0] == '(')
           {commnd_flag = TRUE;
            cmd = SC_dstrcpy(cmd, " ");
            for ( ; i < c; i++)
                {cmd = SC_dstrcat(cmd, v[i]);
                 cmd = SC_dstrcat(cmd, " ");};};

    si->bracket_flag = TRUE;

    SC_zero_space_n(zsp, -2);

    PG_fset_use_pixmap(upix);

/* run in Ultra mode */
    if (SX_gs.sm == SX_MODE_ULTRA)
       SX_mode_ultra(si, load_init, load_rc, track);

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
	   SS_banner(si, SS_mk_string(si, CODE));

	SS_repl(si);

	ret = TRUE;};

    CFREE(cmd);

    return(ret);}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

