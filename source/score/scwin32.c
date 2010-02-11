/*
 * SCWIN32.C - MS Windows win32 specific routines
 *           - would cover DOS too
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "score_int.h" 

#include <windows.h>

extern void
 _SC_win32_process(PROCESS *pp);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	    LPSTR lpCmdLine, int iCmdShow)
   {
    
    _PG_hInstance = hInstance;
    _PG_iCmdShow  = iCmdShow;
    
    main(1, NULL);

    return(_PG_msg.wParam);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PATH_DELIMITER_WIN32 - copy the PATH delimiter, ';' to DELIM */

static void _SC_path_delimiter_win32(char *delim)
   {
    
    strcpy(delim, ";");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FILE_LENGTH_WIN32 - return the length of the file PATH */

static BIGINT _SC_file_length_win32(char *path)
   {BIGINT ln;
    FILE *fp;

    ln = 0L;
    fp = io_open(path, "r");
    if (fp != NULL)
       {io_seek(fp, 0, SEEK_END);
	ln = io_tell(fp);
	io_close(fp);};

    return(ln);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_QUERY_MODE_WIN32 - return TRUE if the file NAME can be opened
 *                      - in MODE
 */

static int _SC_query_mode_win32(char *name, char *mode)
   {int ret;
    FILE *fp;

    if (mode == NULL)
       fp = io_open(name, "r");
    else
       fp = io_open(name, mode);

    ret = (fp != NULL);
    if (fp != NULL)
       io_close(fp);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_QUERY_EXEC_WIN32 - return TRUE if the file PATH is executable */

static int _SC_query_exec_win32(char *path)
   {int ret;
    char s[MAXLINE];
    char *base, *sfx;

    strcpy(s, path);
    SC_str_lower(s);

    base = SC_strtok(s, ".", u);
    sfx  = SC_strtok(NULL, "\n", u);
    if (sfx != NULL)
       ret = ((strcmp(sfx, "exe") == 0) || (strcmp(sfx, "bat") == 0))

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

SC_oscapdes
  _SC_os = { _SC_path_delimiter_win32,
	     _SC_file_length_win32,
	     _SC_query_mode_win32,
	     _SC_query_exec_win32,
	     _SC_win32_process };


