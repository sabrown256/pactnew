/*
 * SCUDL.C - routines for UDL's
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"

/* Uniform Data Locator
 *
 *    [<proto>://<host>/]<path>[@<addr>][,<attr>]*
 *
 *    <proto>     := http | https | ftp |  (via curl)
 *                   mysql                 (via mysql)
 *                   webdav
 *    <path>      := <file> | <container> '~' <file>
 *    <file>      := full or relative path to file
 *                   relative means relative to $HOME or
 *                   whatever the base path is as determined by
 *                   the server or container in question
 *    <container> := tar | ar
 *    <addr>      := offset into the file - allows accessing
 *                   remote files without having to download
 *                   them in toto
 *    <attr>      := key/value pair specifications
 *                   e.g. tgt=ieee96 or tgt=txt
 */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_UDLP - return TRUE iff S is a UDL */

int _SC_udlp(char *s)
   {int rv;

    rv = (strstr(s, "://") != NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_UDL_CONTAINER - return the contained file iff S is a container file
 *                   - otherwise return NULL
 *                   - truncate S so that it specifies the
 *                   - container only iff TRUNC is TRUE
 */

char *_SC_udl_container(char *s, int trunc)
   {char *fc;

    fc = SC_strstr(s, "~");
    if (fc != NULL)
       {if (trunc == TRUE)
	   *fc = '\0';
	fc++;};

    return(fc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PARSE_UDL - parse S and return a SC_udl instance */

SC_udl *_SC_parse_udl(char *s)
   {char t[MAXLINE];
    char *p, *n, *attr;
    char *proto, *srvr, *path, *entry, *addr;
    char *key, *val;
    pcons *info;
    SC_udl *pu;

    proto  = NULL;
    srvr   = NULL;
    path   = NULL;
    addr   = NULL;
    info   = NULL;
    entry  = NULL;

    pu = CMAKE(SC_udl);
    if (s != NULL)
       {path = s;

/* NOTE: the name can be data as in PN_open so only handle
 * strings with all printable characters
 */
	if ((s[0] != '\0') && (SC_print_charsp(s, TRUE) == TRUE))
	   {SC_strncpy(t, MAXLINE, s, -1);

/* check for protocol and server */
	    p = strstr(t, "://");
	    if (p == NULL)
	       p = t;
	    else
	       {*p    = '\0';
		proto = t;
		srvr  = p + 3;
		p     = strchr(srvr, '/');
		if (p != NULL)
		   *p++ = '\0';

		proto = CSTRSAVE(proto);
		srvr  = CSTRSAVE(srvr);};

/* get rough path */	
	    path = p;

/* check for address */
	    p = strchr(path, '@');
	    if (p != NULL)
	       {*p++ = '\0';
		addr = p;}
	    else
	       {p = strchr(path, ',');
		if (p != NULL)
		   *p++ = '\0';};

/* get attributes */
	    if ((p != NULL) && isalnum((int) (*p)))
	       {for (attr = p; attr != NULL; attr = strchr(n, ','))
		    {n = strchr(attr, ',');
		     if (n != NULL)
		        *n++ = '\0';

		     key = attr;
		     p   = strchr(attr, '=');
		     if (p == NULL)
		        val = NULL;
		     else
		        {*p++ = '\0';
			 val  = CSTRSAVE(p);};

		     info = SC_add_alist(info, key, SC_STRING_S, val);

		     if (n == NULL)
		        break;};};

/* resolve path for container and entry */
	    entry = _SC_udl_container(path, TRUE);
	    path  = CSTRSAVE(path);
	    entry = CSTRSAVE(entry);};};

    if (pu != NULL)
       {pu->stream   = NULL;
	pu->buffer   = NULL;
        pu->mode     = NULL;
	pu->udl      = CSTRSAVE(s);
	pu->protocol = proto;
	pu->server   = srvr;
	pu->path     = path;
	pu->entry    = entry;
	pu->target   = NULL;
	pu->format   = NULL;
	pu->address  = SC_stoi(addr);
	pu->info     = info;

	SC_mark(path, 1);};

    return(pu);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REL_UDL - release a SC_udl instance */

void _SC_rel_udl(SC_udl *pu)
   {

    if (pu != NULL)
       {if (pu->stream != NULL)
	   {io_close(pu->stream);
	    pu->stream = NULL;}
	else
	  CFREE(pu->stream);

	CFREE(pu->buffer);
	CFREE(pu->mode);
        CFREE(pu->udl);
	CFREE(pu->protocol);
	CFREE(pu->server);
	CFREE(pu->path);
	CFREE(pu->entry);
	CFREE(pu->target);
	CFREE(pu->format);
	CFREE(pu->protocol);

	SC_free_alist(pu->info, 3);

        CFREE(pu);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

