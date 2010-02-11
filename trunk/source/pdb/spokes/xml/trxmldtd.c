/*
 * TRXMLDTD.C - parse XML Document Type Definition (DTD)
 *            - constructs for XML spoke in PDBLib
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "trxml.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _XML_PARSE_ELEMENT - parse XML DTD elements */

static char *_XML_parse_element(FILE *fp, char *s)
   {char *p;

    while (s != NULL)
       {p = strchr(s, '>');
	if (p != NULL)
	   {s = p + 1;
	    break;}
	else
	   s = lio_gets(_XML.bf, MAXLINE, fp);};

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _XML_PARSE_ATTLIST - parse XML DTD attlists */

static char *_XML_parse_attlist(FILE *fp, char *s)
   {char *p;

    while (s != NULL)
       {p = strchr(s, '>');
	if (p != NULL)
	   {s = p + 1;
	    break;}
	else
	   s = lio_gets(_XML.bf, MAXLINE, fp);};

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _XML_PARSE_DOCTYPE - parse XML DTD doctypes */

static char *_XML_parse_doctype(FILE *fp, char *s)
   {char *p;

    while (s != NULL)
       {p = strchr(s, '>');
	if (p != NULL)
	   {s = p + 1;
	    break;}
	else
	   s = lio_gets(_XML.bf, MAXLINE, fp);};

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _XML_PARSE_ENDDOC - parse XML DTD end of doctype */

static char *_XML_parse_enddoc(FILE *fp, char *s)
   {char *p;

    p = NULL;

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _XML_INIT_DTD - initialize parsing table */

void _XML_init_dtd(hasharr *tab)
   {

    SC_hasharr_install(tab, "<!ELEMENT", (void *) _XML_parse_element, "PFXMLParse", TRUE, TRUE);
    SC_hasharr_install(tab, "<!ATTLIST", (void *) _XML_parse_attlist, "PFXMLParse", TRUE, TRUE);
    SC_hasharr_install(tab, "<!DOCTYPE", (void *) _XML_parse_doctype, "PFXMLParse", TRUE, TRUE);
    SC_hasharr_install(tab, "]>",        (void *) _XML_parse_enddoc,  "PFXMLParse", TRUE, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

