/*
 * STANDARDS.C - detect and print out various standards used
 *             - example compilations:
 *             -    gcc -std=c99 -D_XOPEN_SOURCE=600 standards.c -o standards
 *             -    icc -std=c99 -D_XOPEN_SOURCE=600 standards.c -o standards
 *             -    pgcc -c99 -D_XOPEN_SOURCE=600 standards.c -o standards
 *             -    suncc -xc99 -D_XOPEN_SOURCE=600 standards.c -o standards
 *             -    pathcc -std=c99 -D_XOPEN_SOURCE=600 standards.c -o standards
 *             -    xlc -qlanglvl=extc99 -D_XOPEN_SOURCE=600 standards.c -o standards
 *
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#include <stdlib.h>
#include <stdio.h>

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int rv, psxs, xops, fobits;
    char *lrgf, *gnus, *use_isoc99, *use_posix, *use_posix2;
    char *use_posix199309, *use_posix199506, *use_xopen, *use_xopenx;
    char *use_unix98, *use_xopen2k, *use_largefile, *use_largefile64;
    char *use_file, *use_bsd, *use_svid, *use_misc;
    char *use_atfile, *use_gnu, *use_reentrant;

    rv = 0;

    psxs   = -1;
    xops   = -1;
    fobits = -1;

    gnus            = "no";
    lrgf            = "no";
    use_isoc99      = "no";
    use_posix       = "no";
    use_posix2      = "no";
    use_posix199309 = "no";
    use_posix199506 = "no";
    use_xopen       = "no";
    use_xopenx      = "no";
    use_unix98      = "no";
    use_xopen2k     = "no";
    use_largefile   = "no";
    use_largefile64 = "no";
    use_file        = "no";
    use_bsd         = "no";
    use_svid        = "no";
    use_misc        = "no";
    use_atfile      = "no";
    use_gnu         = "no";
    use_reentrant   = "no";

#ifdef __GNUC__
    gnus = "yes";
#endif
#ifdef __USE_ISOC99         /* Define ISO C99 things */
    use_isoc99 = "yes";
#endif
#ifdef __USE_POSIX          /* Define IEEE Std 1003.1 things */
    use_posix = "yes";
#endif
#ifdef __USE_POSIX2         /* Define IEEE Std 1003.2 things */
    use_posix2 = "yes";
#endif
#ifdef __USE_POSIX199309    /* Define IEEE Std 1003.1, and .1b things */
    use_posix199309 = "yes";
#endif
#ifdef __USE_POSIX199506    /* Define IEEE Std 1003.1, .1b, .1c and .1i things */
    use_posix199506 = "yes";
#endif
#ifdef __USE_XOPEN          /* Define XPG things */
    use_xopen = "yes";
#endif
#ifdef __USE_XOPEN_EXTENDED /* Define X/Open Unix things */
    use_xopenx = "yes";
#endif
#ifdef __USE_UNIX98         /* Define Single Unix V2 things */
    use_unix98 = "yes";
#endif
#ifdef __USE_XOPEN2K        /* Define XPG6 things */
    use_xopen2k = "yes";
#endif
#ifdef __USE_LARGEFILE      /* Define correct standard I/O things */
    use_largefile = "yes";
#endif
#ifdef __USE_LARGEFILE64    /* Define LFS things with separate names */
    use_largefile64 = "yes";
#endif
#ifdef __USE_FILE_OFFSET64  /* Define 64bit interface as default */
    use_file = "yes";
#endif
#ifdef __USE_BSD            /* Define 4.3BSD things */
    use_bsd = "yes";
#endif
#ifdef __USE_SVID           /* Define SVID things */
    use_svid = "yes";
#endif
#ifdef __USE_MISC           /* Define things common to BSD and System V Unix */
    use_misc = "yes";
#endif
#ifdef __USE_ATFILE         /* Define *at interfaces and AT_* constants for them */
    use_atfile = "yes";
#endif
#ifdef __USE_GNU            /* Define GNU extensions */
    use_gnu = "yes";
#endif
#ifdef __USE_REENTRANT      /* Define reentrant/thread-safe *_r functions */
    use_reentrant = "yes";
#endif

#ifdef _POSIX_C_SOURCE
    psxs = _POSIX_C_SOURCE;
#endif

#ifdef _XOPEN_SOURCE
    xops = _XOPEN_SOURCE;
#endif

#ifdef _LARGE_FILES
    lrgf = "yes";
#endif

#ifdef _FILE_OFFSET_BITS
    fobits = _FILE_OFFSET_BITS;
#endif

    printf("   ISO C99 : %s\n", use_isoc99);
    printf("   GNU : %s\n", gnus);
    printf("      Extensions : %s\n", use_gnu);

    printf("   UNIX :\n");
    printf("      4.3BSD : %s\n", use_bsd);
    printf("      SVID : %s\n", use_svid);
    printf("      Single Unix V2 : %s\n", use_unix98);
    printf("      Common BSD and System V Unix : %s\n", use_misc);

    printf("   POSIX source: %d\n", psxs);
    printf("      IEEE Std 1003.1 : %s\n", use_posix);
    printf("      IEEE Std 1003.2 : %s\n", use_posix2);
    printf("      IEEE Std 1003.1, and .1b : %s\n", use_posix199309);
    printf("      IEEE Std 1003.1, .1b, .1c and .1i : %s\n", use_posix199506);

    printf("   XOPEN source: %d\n", xops);
    printf("      XPG : %s\n", use_xopen);
    printf("      X/Open Unix : %s\n", use_xopenx);
    printf("      XPG6 : %s\n", use_xopen2k);

    printf("   Large files: %s\n", lrgf);
    printf("      File Offset Bits:     %d\n", fobits);
    printf("      Correct standard I/O : %s\n", use_largefile);
    printf("      LFS with separate names : %s\n", use_largefile64);
    printf("      64bit interface as default : %s\n", use_file);

    printf("   Miscellaneous:\n");
    printf("      AT interfaces and constants : %s\n", use_atfile);
    printf("      Reentrant/thread-safe functions : %s\n", use_reentrant);

    printf("\n");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
