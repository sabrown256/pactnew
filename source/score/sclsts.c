/*
 * SCLSTS.C - test the lexical scanning routines in SCORE
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

extern int
 F77_FUNC(f77lxr, F77LXR)(void);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i, n, line;
    long sc;
    char msg[MAXLINE];
    char *t, *name;
    SC_lexical_stream *str;

    SC_zero_space_n(0, -2);

    sc = SC_mem_monitor(-1, 2, "sclsts", msg);

    if (c > 0)
       name = v[1];
    else
       name = NULL;

    str = SC_open_lexical_stream(name, 0, 0,
				 F77_FUNC(f77lxr, F77LXR),
				 NULL, NULL, NULL, NULL, NULL, NULL);

    t = str->out_bf;

    line = 1;
    while ((n = SC_scan(str, TRUE)) != -1)
       {if (!SC_blankp(t, "#;c"))
           printf("Error on Line #%d: %s (unknown tokens)\n", line, t);

        for (i = 0; i < n; i++)
            switch (SC_TOKEN_TYPE(str, i))
               {case SC_CMMNT_TOK  :
                     printf("COMMENT: %s\n",
			    SC_TOKEN_STRING(str, i));
		     break;

                case SC_DINT_TOK   :
                case SC_HINT_TOK   :
                case SC_OINT_TOK   :
                     printf("INTEGER: %ld\n",
			    SC_TOKEN_INTEGER(str, i));
		     break;

                case SC_REAL_TOK   :
                     printf("REAL: %11.3e\n",
			    SC_TOKEN_REAL(str, i));
		     break;

                case SC_STRING_TOK :
                case SC_HOLLER_TOK :
                     printf("STRING: %s\n",
			    SC_TOKEN_STRING(str, i));
		     break;

                case SC_IDENT_TOK  :

/* due to some behavior of FLEX which I don't understand
 * comments can get misidentified as sequences of identifiers
 * the consequent clause can go away when this is resolved
 */
		     if ((i == 0) && (*SC_TOKEN_STRING(str, i) == 'c'))
		        {printf("COMMENT:");
			 for (i = 0; i < n; i++)
			     printf(" %s", SC_TOKEN_STRING(str, i));
			 printf("\n");}
                     else
		        printf("IDENTIFIER: %s\n",
			       SC_TOKEN_STRING(str, i));
		     break;

                case SC_KEY_TOK    :
                     printf("KEYWORD: %s\n",
			    SC_TOKEN_STRING(str, i));
		     break;

                case SC_OPER_TOK   :
                     printf("OPERATOR: %s\n",
			    SC_TOKEN_STRING(str, i));
		     break;

                case SC_PRED_TOK   :
                     printf("PREDICATE: %s\n",
			    SC_TOKEN_STRING(str, i));
		     break;

                case SC_DELIM_TOK  :
                     printf("DELIMITER: %s\n",
			    SC_TOKEN_STRING(str, i));
		     break;

                case SC_WSPC_TOK   :
                     printf("WHITE SPACE: \"%s\"\n",
			    SC_TOKEN_STRING(str, i));
		     break;

                default            :
                     printf("UNKNOWN TOKEN TYPE: %d\n",
                            SC_TOKEN_TYPE(str, i));
		     break;};

        line++;};

    SC_close_lexical_stream(str);

    sc = SC_mem_monitor(sc, 2, "sclsts", msg);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
