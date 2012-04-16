/*
 * SORC.C - the PDB source file maker for LR based PANACEA codes
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#undef CODE
#define CODE "S"

#include "cpyright.h"

#include "s.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GEN_SYSTEM - define a PANACEA code data base using the gen_package
 *            - facility
 *            -
 *            -    GENERATOR_COMMANDS - ignored
 *            -
 *            -    DEFINE_STRUCTS     - define non-default data types
 *            -
 *            -    DEFINE_UNITS       - define any units for the 
 *            -                       - package not already defined
 *            -
 *            -    DEFINE_VARIABLES   - define the variables needed by
 *            -                       - the package (with variables
 *            -                       - shared by many packages the one
 *            -                       - that computes the value should
 *            -                       - define it)
 *            -
 *            -    DEFINE_CONTROLS    - define the set of integer,
 *            -                       - floating point, and ASCII
 *            -                       - control scalars
 *            -
 *            -    INTERN_VARIABLES   - ignored
 *            -
 *            -    ASCII DB NAME      - name of ASCII file which contains
 *            -                       - the definitions of the variables,
 *            -                       - units, structures, and controls
 *            -                       - this functionally replaces
 *            -                       - DEFINE_STRUCTS, DEFINE_UNITS,
 *            -                       - DEFINE_VARIABLES, and DEFINE_CONTROLS
 */

void gen_system(void)
   {

    PA_gen_package("global",
                   NULL,
                   NULL,
                   NULL,
                   glo_var,
                   glo_cnt,
                   NULL,
                   NULL);
    PA_gen_package("lag",
                   NULL,
                   NULL,
                   NULL,
                   lag_var,
                   lag_cnt,
                   NULL,
                   NULL);
    PA_gen_package("misc",
                   NULL,
                   NULL,
                   NULL,
                   mis_var,
                   NULL,
                   NULL,
                   NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* MAIN - control the generator process as a stand alone code */
 
int main(int c, char **v)
   {char *fname, s[MAXLINE];

    PG_open_console("B", "MONOCHROME", 1, 0.1, 0.1, 0.8, 0.8);

#ifdef NO_SHELL
    {char *t;

     PRINT(stdout, "Restart Name: ");
     GETLN(s, MAXLINE, stdin);
     SC_strtok(s, "\n\r", t);}
#else
   
    fname = gen_args(c, v);

#endif

    SC_init("S: Exit with error", NULL,
            TRUE, PA_interrupt_handler, NULL,
            TRUE, NULL, 0);

/* print the banner */
    SC_set_banner(" %s  -  %s\n\n", CODE, VERSION);
    SC_banner("");

/* connect function pointers to initial functions */
    SC_set_put_line(fprintf);
    SC_set_get_line(fgets);

/* define the code system for which we are building sources */
    gen_system();
    init_unit();
    PA_def_str(PA_gs.vif);
    PA_variables(TRUE);
    PA_proc_units();
    inst_s();
    srctab = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY, 0);
    PA_gs.source = CSTRSAVE("source");
 
/* create the pdb file */
    strcpy(s, fname);
    SC_strcat(s, MAXLINE, ".src");
    pdsf = PD_create(s);
    if (pdsf == NULL)
       {printf("%s", PD_get_error());
        LONGJMP(SC_gs.cpu, ABORT);};

/* define the source_record for the PDB source file */
   PD_defstr(pdsf, "source_record",
             "char *name",
	     "double time",
             "integer *indexes",
	     "double *data",
	     LAST);

/* read the source file */
    readh(fname);

/* dispatch on commands from the terminal */
    PA_get_commands(stdin, (PFVString) S_gen);
 
    LONGJMP(SC_gs.cpu, ERR_FREE);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

