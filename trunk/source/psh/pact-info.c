/*
 * PACT-INFO.C - report info about configured PACT
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * include "cpyright.h"
 *
 */

#include "common.h"
#include "libpsh.c"
#include "libinfo.c"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* USAGE - print help */

void usage(void)
   {

    printf("\n");
    printf("Usage: pact-info [-f] [-h] [-incpath] <pttrn>\n");
    printf("            [-l] [-link]\n");
    printf("\n");

    printf("    -f       report full matches as <var>=<val>\n");
    printf("    -h       this help message\n");
    printf("    -incpath report elements needed to compile files\n");
    printf("    -l       report only exact matches of the argument\n");
    printf("    -link    report elements needed to link applications\n");
    printf("    -v       report the PACT version\n");
    printf("    <pttrn>  part or all of name of configuration element\n");
    printf("\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int i, ok, cmpl, ltrl;
    char root[MAXLINE];
    char *ptrn;
    itarget tgt;

    cmpl = FALSE;
    ltrl = FALSE;
    file_path(v[0], root, MAXLINE);
    pop_path(root);
    pop_path(root);

    ok   = 0;
    ptrn = NULL;
    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-f") == 0)
            cmpl = TRUE;

	 else if (strcmp(v[i], "-incpath") == 0)
	    {tgt = INC;
	     break;}

	 else if (strcmp(v[i], "-link") == 0)
	    {tgt = LINK;
	     break;}

	 else if (strcmp(v[i], "-h") == 0)
	    {usage();
	     ok = 1;
	     break;}

	 else if (strcmp(v[i], "-l") == 0)
	    ltrl = TRUE;

	 else if (strcmp(v[i], "-v") == 0)
	    {tgt = VERSION;
	     break;}

	 else
	    {tgt = REGEX;
	     ptrn = v[i];
	     break;};};

    if (ok == 0)
       ok = report_info(root, cmpl, ltrl, tgt, ptrn);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
