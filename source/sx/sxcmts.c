/*
 * SXCMTS.C - test SX_command
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - wrapper for SX_command */

int main(int c, char **v, char **env)
   {int i, rv;
    char *cmd, *file;

/* process the command line arguments */
    for (i = 1; i < c; i++)
        {if (file == NULL)
	    file = v[i];
	 else
           {cmd = SC_dstrcpy(cmd, " ");
            for ( ; i < c; i++)
                {cmd = SC_dstrcat(cmd, v[i]);
                 cmd = SC_dstrcat(cmd, " ");};};};

    rv = SX_command(file, cmd);

    CFREE(cmd);

    return(rv);}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
