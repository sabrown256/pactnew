/*
 * TRXMLP.C - parse XML constructs for XML spoke in SX
 *          - XML grammar
 *          -  <element>       := <tag-start><body><tag-end>
 *          -  <tag-start>     := '<' <id> [<attr>*] '>'
 *          -  <tag-end>       := '</' <id> '>'
 *          -  <body>          := <element>* | <value>
 *          -  <value>         := '' | [~<]* | '<![' text ']>'
 *          -  <attr>          := <key> '=' <quote> text <quote>
 *          -  <key>           := <id>
 *          -  <quote>         := ' | "
 *          -  <id>            := identifier
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "trxml.h"

typedef struct s_parse_state parse_state;

struct s_parse_state
   {int ntag;
    off_t loc;
    off_t tok;
    SC_array *stack;
    long ic;
    long nc;
    char *bf;
    char *value;
    PDBfile *file;};

XML_state
 _XML;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _XML_PARSE_COMMENT - parse XML comments */

static char *_XML_parse_comment(FILE *fp, char *s)
   {char *p;

    while (s != NULL)
       {p = strstr(s, "-->");
	if (p != NULL)
	   {s = p + 3;
	    break;}
	else
	   s = lio_gets(_XML.bf, MAXLINE, fp);};

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _XML_INIT_PARSE - initialize parsing table */

static void _XML_init_parse(void)
   {

    _XML.parse = SC_make_hasharr(HSZSMALL, FALSE, SC_HA_NAME_KEY);

    SC_hasharr_install(_XML.parse, "<!--", (void *) _XML_parse_comment, "PFXMLParse", TRUE, TRUE);

    _XML_init_dtd(_XML.parse);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _XML_PUSH_CHAR - increase the buffer size */

static int _XML_push_char(parse_state *st, int c)
   {int ic, nc;
    char *bf;

    if (st->bf == NULL)
       {st->bf = CMAKE_N(char, MAX_BFSZ);
	st->nc = MAX_BFSZ;
	st->ic = 0;}
    else if (st->ic > st->nc - 10)
       {nc = 2*(st->nc);
	CREMAKE(st->bf, char, nc);
	st->nc = nc;};

    ic = st->ic++;
    bf = st->bf;
    bf[ic++] = c;
    bf[ic++] = '\0';

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _XML_READ_NEXT - find the next tag or value from current location */

static char *_XML_read_next(parse_state *st)
   {int i, id, c, ok;
    char *tag, *p;
    FILE *fp;

    fp = st->file->stream;
    ok = TRUE;

    _XML_push_char(st, 0);

    st->ic  = 0;
    st->tok = st->loc;

    for (i = 0; ok == TRUE; i++)
        {c = io_getc(fp);
	 if (c >= 0)
	    {st->loc++;

	     _XML_push_char(st, c);

	     switch (c)
	        {case '<' :
		      if (i != 0)
			 {p = st->bf + strspn(st->bf, " \t\n\r\f");
			  if (*p != '<')
			     {io_ungetc(c, fp);
			      st->loc--;
			      SC_LAST_CHAR(st->bf) = '\0';
			      ok = FALSE;}
			  else
			     {st->ic = 0;
			      _XML_push_char(st, c);};};
		      break;
		 case '>' :
		      ok = FALSE;
		      break;
		 case '!' :
		      for (id = 1; id > 0; i++)
			  {c = io_getc(fp);
			   if (c >= 0)
			      {st->loc++;
			       _XML_push_char(st, c);
			       switch (c)
				  {case '<' :
				        id++;
					break;
				   case '>' :
				        id--;
					break;};}
			   else
			      return(NULL);};
		      ok = FALSE;
		      break;
		 default :
		      break;};}
	 else
	    return(NULL);};

    tag = st->bf;

    return(tag);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _XML_ENTRY_UNIQ - add NAME to PATH making a unique path name */

static char *_XML_entry_uniq(parse_state *st, char *path, char *name, int lev)
   {int nr, ok;
    long i;
    char s[MAXLINE];
    syment *ep;
    PDBfile *file;

    file = st->file;

    path = SC_dstrcat(path, name);
    nr   = strlen(path);
    if (path[nr-1] != '/')
       {ok = TRUE;
	if (lev != 0)
	   {for (i = 1; ok == TRUE; i++)
	        {ok = FALSE;
		 path[nr] = '\0';
	         snprintf(s, MAXLINE, "-%ld", i);
		 path = SC_dstrcat(path, s);

		 ep = PD_inquire_entry(file, path, FALSE, NULL);
		 ok = (ep != NULL);};};}

    else
       {ep = PD_inquire_entry(file, path, FALSE, NULL);
	if (ep == NULL)
	   {ep = _PD_mk_syment("Directory", 1, 0, NULL, NULL);
	    _PD_e_install(file, path, ep, FALSE);};};

    return(path);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _XML_ENTRY_NAME - make a name for entry */

static char *_XML_entry_name(parse_state *st)
   {int is, ns;
    char s[MAXLINE];
    char *name, *b, *u;
    SC_array *a;

    a  = st->stack;
    ns = SC_array_get_n(a);

    name = NULL;
    for (is = 0; is < ns; is++)
        {b    = *(char **) SC_array_get(a, is);
	 name = _XML_entry_uniq(st, name, "/", is);

         SC_strncpy(s, MAXLINE, b, -1);

	 b    = SC_strtok(s, " \t>", u);
	 name = _XML_entry_uniq(st, name, b, is);};

    SC_trim_right(name, " \t\n\r\f");

    return(name);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _XML_ENTRY_INFO - determine the type and number of items of data in S */

static void _XML_entry_info(char *s, char **ptype, long *pni)
   {int ok;
    long l, ni;
    double d;
    char *type, *pb, *pe;

    ok = FALSE;

/* check for integers */
    if (ok == FALSE)
       {ok = TRUE;
	ni = 0L;
	for (pb = s, pe = NULL; ok == TRUE; pb = pe, ni++)
	    {pb += strspn(pb, ", \t\n\f");
	     if (*pb == '\0')
	        break;

	     l = SC_strtol(pb, &pe, 10);
	     SC_ASSERT(l >= 0);

	     if (pe == pb)
	        ok = FALSE;};
        type = SC_LONG_S;};

/* check for doubles */
    if (ok == FALSE)
       {ok = TRUE;
	ni = 0L;
	for (pb = s, pe = NULL; ok == TRUE; pb = pe, ni++)
	    {pb += strspn(pb, ", \t\n\f");
	     if (*pb == '\0')
	        break;

	     d = SC_strtod(pb, &pe);
	     SC_ASSERT(d != 0.0);

	     if (pe == pb)
	        ok = FALSE;};
        type = SC_DOUBLE_S;};

/* otherwise it is char */
    if (ok == FALSE)
       {type = SC_CHAR_S;
	ni   = strlen(s);};

    type = CSTRSAVE(type);

    *ptype = type;
    *pni   = ni;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _XML_PUSH_VALUE - push a value into the parse state */

static void _XML_push_value(parse_state *st, char *t, int ts, int te)
   {int nc;
    char *s;

    CFREE(st->value);

    s = CSTRSAVE(t+ts);
    nc = strlen(s);
    s[nc-te] = '\0';

    st->value = s;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _XML_END_TAG - pop a tag off the parse state
 *              - and complete the processing of the element
 */

static void _XML_end_tag(parse_state *st, char *t)
   {int nb;
    long ni, addr;
    char s[MAXLINE];
    char *b, *u, *name, *type, *val;
    dimdes *dm;
    syment *ep;
    PDBfile *file;

    file = st->file;

/* NOTE: cannot pop this because _XML_entry_name needs it too */
    b = *(char **) SC_array_get(st->stack, -1);

    SC_strncpy(s, MAXLINE, b, -1);
    b  = SC_strtok(s, " \t>", u);
    nb = strlen(b);

    if ((strncmp(b, t+2, nb) == 0) && (t[2+nb] == '>'))
       {val = st->value;
	if (val != NULL)
	   {name = _XML_entry_name(st);
	    _XML_entry_info(val, &type, &ni);

	    addr = st->tok - strlen(val);

	    dm = _PD_mk_dimensions(1L, ni);
	    ep = _PD_mk_syment(type, ni, addr, NULL, dm);

	    _PD_e_install(file, name, ep, FALSE);

	    CFREE(type);
	    CFREE(name);
	    CFREE(st->value);};}

    else
       PD_error("SYNTAX ERROR ON TAG - _XML_END_TAG", PD_OPEN);

/* pop the stack */
    SC_array_pop(st->stack);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _XML_START_TAG - push a tag onto the parse state
 *                - and begin the processing of an element
 */

static void _XML_start_tag(parse_state *st, char *t)
   {char s[MAXLINE];
    char *p, *u;

    SC_strncpy(s, MAXLINE, t, -1);
    p = SC_strtok(s, "<>", u);

    SC_array_string_add_copy(st->stack, p);

    st->ntag++;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _XML_PARSE_TAGS - read FP and parse the tags */

static void _XML_parse_tags(PDBfile *file)
   {int ok, nt;
    char *t;
    FILE *fp;
    parse_state st;

    fp = file->stream;

    st.loc   = io_tell(fp);
    st.tok   = st.loc;
    st.file  = file;
    st.value = NULL;
    st.ntag  = 0;
    st.ic    = 0;
    st.nc    = 0;
    st.bf    = NULL;
    st.stack = CMAKE_ARRAY(char *, NULL, 0);

    for (ok = TRUE; ok == TRUE; )
        {t = _XML_read_next(&st);
	 if (t == NULL)
	    ok = FALSE;
	 else
	    {nt = strlen(t);
	     if (strncmp(t, "<![", 3) == 0)
	        _XML_push_value(&st, t, 3, 2);

/* GOTCHA: figure out what to do about attributes and empty tags, i.e. <foo/> */
	     else if ((nt > 2) && (strncmp(t+nt-2, "/>", 2) == 0))
	        {}

	     else if (strncmp(t, "</", 2) == 0)
	        _XML_end_tag(&st, t);
	     else if (*t == '<')
	        _XML_start_tag(&st, t);
	     else
	        _XML_push_value(&st, t, 0, 0);};};
	   
    SC_free_array(st.stack, NULL);
    CFREE(st.value);
    CFREE(st.bf);
    st.ic   = 0;
    st.nc   = 0;
    st.file = NULL;
    st.ntag = 0;
    st.loc  = 0;
    st.tok  = 0;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _XML_FIX_ENTRIES - remove integer id from otherwise unique names */

static void _XML_fix_entries(PDBfile *file)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _XML_PARSE_XML - parse an XML file */

PDBfile *_XML_parse_xml(SC_udl *pu, char *mode)
   {PDBfile *file;

    file = _PD_mk_pdb(pu, NULL, NULL, TRUE, NULL, NULL);
    if (file == NULL)
       PD_error("CAN'T ALLOCATE XML - XML_OPEN", PD_OPEN);

    file->default_offset = 0;
    file->major_order    = ROW_MAJOR_ORDER;
    file->type           = CSTRSAVE(XML_S);
    if (*mode == 'a')
       file->mode = PD_APPEND;
    else
       file->mode = PD_OPEN;

    _PD_set_standard(file, &TEXT_STD, &TEXT_ALIGNMENT);

    _PD_init_chrt(file, TRUE);

    file->current_prefix = CSTRSAVE("/");

    PD_set_text_delimiter(file, ",");

    if (_XML.parse == NULL)
       _XML_init_parse();

    _XML_parse_tags(file);

    _XML_fix_entries(file);

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
