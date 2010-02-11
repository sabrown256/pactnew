/*
 * TRXML.H - the XML support header for SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_XML

#include "cpyright.h"
 
#include "../../pdb_int.h"

#define PCK_XML

/*--------------------------------------------------------------------------*/

/*                              DEFINED CONSTANTS                           */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                          TYPEDEFS AND STRUCTS                            */

/*--------------------------------------------------------------------------*/

typedef char *(*PFXMLParse)(FILE *fp, char *s);

typedef struct s_XML_state XML_state;

struct s_XML_state
   {char bf[MAXLINE];
    hasharr *parse;};

/*--------------------------------------------------------------------------*/

/*                          VARIABLE DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

extern char
 *XML_S;

extern XML_state
 _XML;

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/


/* TRXML.C declarations */


/* TRXMLDTD.C declarations */

extern void
 _XML_init_dtd(hasharr *tab);


/* TRXMLP.C declarations */

extern PDBfile
 *_XML_parse_xml(SC_udl *pu, char *mode);


/* TRXMLXSD.C declarations */

extern void
 _XML_init_xsd(hasharr *tab);


#endif

