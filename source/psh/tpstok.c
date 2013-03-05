/*
 * TPSTOK.C - test strcpy_tok
 *
 * include "cpyright.h"
 *
 */

#include "common.h"
#include "libpsh.c"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int i, nr, ok, rv, flags;
    char *b, *e, *s, *tgt, *a[10], *r[10];
    char t[BFSML];

    flags = 0;

    b = NULL;
    e = NULL;
    s = NULL;
    memset(a, 0, sizeof(a));
    memset(r, 0, sizeof(r));

    tgt = "tok";
    nr  = 0;
    for (i = 1; i < c; i++)
	{if (strcmp(v[i], "-a") == 0)
            a[nr++] = v[i];

	 else if (strcmp(v[i], "-b") == 0)
            b = v[++i];

	 else if (strcmp(v[i], "-d") == 0)
            e = v[++i];

	 else if (strcmp(v[i], "-e") == 0)
            e = v[++i];

	 else if (strcmp(v[i], "-f") == 0)
            {if (strcmp(v[++i], "exp") == 0)
	        flags |= EXPAND_ESC;
	     else if (strcmp(v[++i], "add") == 0)
	        flags |= ADD_DELIMITER;
	     else if (strcmp(v[++i], "tqu") == 0)
	        flags |= TRANSPARENT_QUOTES;
	     else if (strcmp(v[++i], "rqu") == 0)
	        flags |= REMOVE_QUOTES;}

	 else if (strcmp(v[i], "-m") == 0)
            tgt = "str";

	 else if (strcmp(v[i], "-s") == 0)
            s = v[++i];};

/* tokenize the string S */
    if (strcmp(tgt, "tok") == 0)
       {for (i = 0; i < nr; i++)
	    {strcpy_tok(t, BFSML, s, -1, b, e, flags);
	     r[i] = STRSAVE(t);};}
    else if (strcmp(tgt, "str") == 0)
       {for (i = 0; i < nr; i++)
	    {strcpy_str(t, BFSML, s, -1, e, flags);
	     r[i] = STRSAVE(t);};}

/* compare the results R with the expected answers A */
    ok = TRUE;
    for (i = 0; i < nr; i++)
        ok &= (strcmp(r[i], a[i]) == 0);

/* reverse the sense for exit status */
    rv = (ok == FALSE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

