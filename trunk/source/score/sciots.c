/*
 * SCIOTS.C - test the io functions
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

#define CHECK(_t)                                                           \
   {int _l;                                                                 \
    _l = _t;                                                                \
    rv &= _l;                                                               \
    if (_l == TRUE)                                                         \
       printf(".");                                                         \
    else                                                                    \
       printf("x");}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO_TEST - test the IO_ functions */

int do_test(char *tst)
   {int c, rv, np, ns, st;
    long l1, l2;
    size_t nw, nr;
    char s[MAXLINE];
    char *p, *t;
    FILE *fp;

    printf("                       %-7s ..", tst);
    rv = TRUE;

    fp = io_open("sciots.txt", "w");
    CHECK(fp != NULL);

    st = io_setvbuf(fp, NULL, _IONBF, 0);
    CHECK(st == 0);

    np = io_printf(fp, "The quick brown fox jumps");
    CHECK(np == 25);

    nw = io_write(" over the lazy dog\n", 1, 19, fp);
    CHECK(nw == 19);

    l1 = io_tell(fp);

    t  = SC_dsnprintf(FALSE, "She sells sea shells ...\n");
    ns = io_puts(t, fp);
    CHECK(ns == strlen(t));

    l2 = io_tell(fp);

    st = io_flush(fp);
    CHECK(st == 0);

    st = io_close(fp);
    CHECK(st == 0);

    printf(" ");

/* re-open the file */
    fp = io_open("sciots.txt", "r+");
    CHECK(fp != NULL);

    st = io_seek(fp, 0, SEEK_SET);
    CHECK(st == 0);

    p = io_gets(s, MAXLINE, fp);
    CHECK((p != NULL) && (strncmp(s, "The quick brown fox", 18) == 0));

    st = io_seek(fp, l1, SEEK_SET);
    CHECK(st == 0);

    nr = io_read(s, 1, 10, fp);
    CHECK(nr == 10);

    st = io_seek(fp, l2, SEEK_SET);
    CHECK(st == 0);

    st = io_eof(fp);
/*    CHECK(st == 0); */

    st = io_seek(fp, l1, SEEK_SET);
    CHECK(st == 0);

    c = io_getc(fp);
    CHECK(c == 'S');

    st = io_ungetc(c, fp);
    CHECK(st == 'S');

    st = io_close(fp);
    CHECK(st == 0);

    if (rv == TRUE)
       printf(" ok\n");
    else
       printf(" ng\n");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test character predicates */

int main(int c, char **v)
   {int i, rv, t1, t2, t3, t4;

    SC_setbuf(stdout, NULL);

/* process the command line arguments */
    t1 = TRUE;
    t2 = TRUE;
    t3 = TRUE;
    t4 = TRUE;
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case '1' :
		      t1 = FALSE;
		      break;
                 case '2' :
		      t2 = FALSE;
		      break;
                 case '3' :
		      t3 = FALSE;
		      break;
                 case '4' :
		      t4 = FALSE;
		      break;

	         case 'h' :
		      printf("\nUsage: scprts [-1] [-2] [-3] [-4] [-h]\n");
		      printf("   Options:\n");
		      printf("      1 - do not test local IO\n");
		      printf("      2 - do not test remote IO\n");
		      printf("      3 - do not test small mapped IO\n");
		      printf("      4 - do not test large mapped IO\n");
		      printf("      h - print this help message\n");
		      printf("\n");
		      exit(1);};}
         else
            break;};

    io_printf(stdout, "\n");

    rv = TRUE;

    if (t1 == TRUE)
       {SC_io_connect(SC_LOCAL);
        rv &= do_test("Local");};

    if (t2 == TRUE)
       {SC_io_connect(SC_REMOTE);
        rv &= do_test("Remote");};

#ifdef HAVE_MMAP
    if (t3 == TRUE)
       {SC_mf_set_hooks();
        rv &= do_test("Small");};

    if (t4 == TRUE)
       {SC_lmf_set_hooks();
        rv &= do_test("Large");};
#endif

    printf("\n");

    rv = !rv;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
