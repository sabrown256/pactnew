/*
 * TRXML.C - command processors for XML spoke in SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "trxml.h"

char
 *XML_S = "XMLfile";

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _XML_FILEP - return TRUE iff TYPE is XML file */

static int _XML_filep(char *type)
   {int rv;

    rv = (strcmp(type, XML_S) == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _XML_CLOSE - close an XML file */

static int _XML_close(PDBfile *file)
   {FILE *fp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    switch (SETJMP(pa->close_err))
       {case ABORT :
	     return(FALSE);
        case ERR_FREE :
	     return(TRUE);
        default :
	     memset(pa->err, 0, MAXLINE);
	     break;};

    fp = file->stream;
    if (lio_close(fp) != 0)
       PD_error("CAN'T CLOSE FILE - _XML_CLOSE", PD_CLOSE);

/* free the space */
    _PD_rl_pdb(file);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _XML_OPEN - open an existing XML File */

static PDBfile *_XML_open(tr_layer *tr, SC_udl *pu, char *name, char *mode)
   {char str[MAXLINE];
    FILE *fp;
    PDBfile *file;

    fp   = pu->stream;
    file = NULL;

    _PD_set_io_buffer(pu);

/* attempt to read an ASCII header */
    if (lio_seek(fp, 0L, SEEK_SET))
       PD_error("FSEEK FAILED TO FIND ORIGIN - _XML_OPEN", PD_OPEN);

    if (lio_gets(str, MAXLINE, fp) != NULL)
       {if (strncmp(str, "<?xml version=\"1.", 17) == 0)
	   file = _XML_parse_xml(pu, mode);};

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PD_REGISTER_XML - install the XML extensions to PDBLib */
 
int PD_register_xml(void)
   {int n;

    n = PD_REGISTER(XML_S, "xml", _XML_filep,
		    NULL, _XML_open, _XML_close, NULL, NULL);
    SC_ASSERT(n >= 0);

    return(TRUE);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

