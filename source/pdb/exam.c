/*
 * EXAM.C - diagnostic to examine binary files
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include <signal.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define MAXLINE 255

extern long atol();

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

/*--------------------------------------------------------------------------*/

#ifndef max

/*--------------------------------------------------------------------------*/

/* MAX - the oft wanted maximum */

#define max(a, b) ((a > b) ? (a) : (b))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MIN - companion to max */

#define min(a, b) ((a < b) ? (a) : (b))

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* MAIN - control the exam process */

int main(int c, char **v)
   {FILE *fp, *out;
    long i, start, end;
    char *name;

    out = STDOUT;

/* open the file */
    name = v[1];
    if ((fp = fopen(name, "rb")) == NULL)
       {fprintf(stderr, "EXAM couldn't open file %s\r\n", name);
        exit(1);};
    fprintf(out, "File %s opened\n", name);

    if (c >= 4)
       {start = atol(v[2]);
        end   = atol(v[3]);}
    else if (c == 3)
       {start = atol(v[2]);
        fseek(fp, 0L, SEEK_END);
        end   = ftell(fp) - 1;
        fseek(fp, 0L, SEEK_SET);}
    else if (c == 2)
       {start = 0L;
        fseek(fp, 0L, SEEK_END);
        end   = ftell(fp) - 1;
        fseek(fp, 0L, SEEK_SET);}
    else
       {printf("\nUsage: exam <filename> <start> <end>\n\n");
        exit(1);};
        
    signal(SIGINT, SIG_DFL);

    start = max(0L, start);
    fseek(fp, start, SEEK_SET);

    fprintf(out, "\n\nExamine file %s from byte %ld to byte %ld\n\n",
           name, start, end);
    fprintf(out, "   Byte   Char   Dec   Hex\n\n");
    for (i = start; i <= end; i++)
        {c = fgetc(fp);
         if (c == '\0')
            fprintf(out, "%8ld:   0     %3d    %2x\n", i, c, c);
         else if (c < ' ')
            fprintf(out, "%8ld:  ^%c     %3d    %2x\n", i, '@'+c, c, c);
         else if (c < '\177')
            fprintf(out, "%8ld:   %c     %3d    %2x\n", i, c, c, c);
         else
            fprintf(out, "%8ld:  --     %3d    %2x\n", i, c, c);};

    fclose(fp);
    fprintf(out, "\nFile closed - EXAM exiting\n");

    exit(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
