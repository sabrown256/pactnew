/*
 * TPDLS.C - test to list contents of PDB file
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nTPDLS - test listing contents\n\n");
    PRINT(STDOUT, "Usage: tpdls [-h] [-v #]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       h  - print this help message and exit\n");
    PRINT(STDOUT, "       v  - use format version # (default is 2)\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - list the contents of a file */

int main(int c, char **v)
   {int i, nd, ne, cs, rv;
    long ni;
    long *dims;
    char **names, **list, *fname, *type;
    char msg[MAXLINE];
    syment *ep;
    PDBfile *file;
    static int debug = 0;

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'h' :
		      print_help();
		      return(1);
                 case 'v' :
                      PD_set_fmt_version(SC_stoi(v[++i]));
		      break;};}
         else
            break;};

    cs = SC_mem_monitor(-1, debug, "LS", msg);

    fname = NULL;
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            continue;
	 else
	    fname = v[i];};

    if (fname == NULL)
       {printf("Usage: pdlst <file>\n");
	exit(1);};

    file = PD_open(fname, "r");
    if (file == NULL)
       {printf("Error opening %s\n", fname);
	exit(1);};

    printf("\nSymbol Table:\n");

/* NOTE: this is to have the literal names around for debugging */
    list = SC_hasharr_dump(file->symtab, NULL, NULL, FALSE);

/* print contents of symbol table */
    names = PD_ls(file, NULL, NULL, &ne);
    for (i = 0; i < ne; i++)
	{if (names[i] != NULL)
            {ep = PD_query_entry(file, names[i], NULL);
	     SC_mark(ep, 1);

	     rv = PD_get_entry_info(ep, &type, &ni, &nd, &dims);
	     if (rv)
	        {printf("%20s %10s %8ld %2d (%p)\n",
			names[i], type, ni, nd, dims);
		 PD_rel_entry_info(ep, type, dims);};};};

    CFREE(names);
    SC_free_strings(list);

/* close file */
    PD_close(file);

    cs = SC_mem_monitor(cs, debug, "LS", msg);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
