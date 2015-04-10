/* 
 * GENFILES.C - read outfiles from getout, from_f and from_c, then map
 *            - fortran kinds to C types
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define MAX_TYPES  32
#define MAX_STRING 32
#define MAX_LINE   80
#define MAX_SIZES   5

/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/

typedef struct s_type_data type_data;

struct s_type_data
   {char f_type[MAX_STRING];
    int kind;
    int precision;
    int range;
    int size;
    char c_type[MAX_STRING];
    int active;};

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

/* Must keep an extra entry at the end as the end-of-list flag (active==0) */

type_data
  from_f[MAX_TYPES+1],
  from_c[MAX_TYPES+1];

char
 *c_header = {"/*--------------------------------------------------------------------------*/\n"},
 external_macro[MAX_LINE];

int
 int_sizes[]  = {1, 2, 4, 8, 16, -1},
 real_sizes[] = {4, 8, 16, -1};


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void make_upper(char *in, char *out)
   {char *s, *t;

    for (s = in, t = out; *s != '\0'; s++, t++)
        *t = toupper(*s);

    *t = '\0';

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void add_underscores(char *in, char *out)
   {char *s, *t;

    for (s = in, t = out; *s != '\0'; s++, t++)
        {if (*s == ' ')
	    *t = '_';
         else
	    *t = *s;};

    *t = '\0';

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

type_data *next_entry(type_data *node)
   {int i;

/* find the last node */
    for (i = 0; node->active; node++, i++);

    if (i == MAX_TYPES)
       {printf("too many types\n");
	exit(1);};

    node->active = 1;

    return(node);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COPY_ENTRY - copy from q to p */

void copy_entry(type_data *p, type_data *q)
   {

    strcpy(p->f_type, q->f_type);

    p->kind      = q->kind;
    p->precision = q->precision;
    p->range     = q->range;
    p->size      = q->size;

    strcpy(p->c_type, q->c_type);

    p->active = 1;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIND_SIZE - return the node a type and byte size */

type_data *find_size(type_data *p, char *type, int size)
   {

    for (; p->active; p++)
        {if (p->size != size)
	    continue;

	 if (p->kind == 0)
	    continue;   /* do not return the default kind */

	 if (strcmp(p->f_type, type) != 0)
	    continue;

	 return(p);};

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIND_SIZE_EXP - return the node a type and byte size
 *               - and the largest precision
 *               - this is useful for machines like the CRAY
 *               - where everything is 8 bytes,
 *               - we want the largest kind
 */

type_data *find_size_exp(type_data *p, char *type, int size)
   {type_data *q;

    q = NULL;

    for (; p->active; p++)
        {if (p->size != size)
	    continue;

	 if (p->kind == 0)
	    continue;   /* do not return the default kind */

	 if (strcmp(p->f_type, type) != 0)
	    continue;

	 if (q == NULL)
	    q = p;
	 else if (p->precision >= q->precision)
	    q = p;};

    return(q);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

type_data *find_kind(type_data *p, char *type, int kind)
   {

    for (; p->active; p++)
        {if (p->kind != kind)
	    continue;

	 if (strcmp(p->f_type, type) != 0)
	    continue;

	 return(p);};

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

type_data *find_c_type(type_data *p, char *type)
   {

    for (; p->active; p++)
        {if (strcmp(p->c_type, type) != 0)
	    continue;

	 return(p);};

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void init_sizes(int *sizes)
   {int i;

    for (i = 0; i < MAX_SIZES; i++)
        sizes[i] = -1;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void add_size(int *sizes, int size)
   {int i;

    for (i = 0; i < MAX_SIZES; i++)
        {if (sizes[i] == -1)
	    {sizes[i] = size;
	     return;};};

    printf("sizes too small\n");
    exit(1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int size_used(int *sizes, int size)
   {int i;

    for (i = 0; i < MAX_SIZES; i++)
        {if (sizes[i] == size)
	    return(1);};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIND_DEFAULT_KIND - find the actual kind of the
 *                   - default kind by matching sizes
 */

int find_default_kind(char *type)
   {int size, kind;
    type_data *p;

    size = -1;
    for (p = from_f; p->active; p++)
        {if (p->kind != 0)
	    continue;
	 if (strcmp(p->f_type, type) != 0)
	    continue;
	 size = p->size;
	 break;};

    if (size < 0)
       return(-1);

/* now find a matching size of a non-default kind */
    kind = -1;
    for (p = from_f; p->active; p++)
        {if (p->kind == 0)
	    continue;  /* skip default type */

	 if (strcmp(p->f_type, type) != 0)
	    continue;

	 if (p->size != size)
	    continue;

	 kind = p->kind;
	 break;};

    return(kind);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void read_input(void)
   {int i;
    int d, d1, d2;
    int num_f_types, num_c_types;
    char *s;
    char s1[MAX_STRING], s2[MAX_STRING];
    type_data *p;
    FILE *in;

    for (i = 0; i <= MAX_TYPES; i++)
        {from_f[i].active = 0;
	 from_c[i].active = 0;};

/* read the file generated by C with the byte sizes */
    in = fopen("from_c", "r");
    if (in == NULL)
       {fprintf(stderr, "Error opening from_c\n");
	exit(1);};

/* first line is the fortran name mangling macro */
    if (fgets(external_macro, MAX_LINE, in) == NULL)
       {fprintf(stderr, "Premature EOF reading from_ext\n");
	exit(1);};

    p = from_c;
    while (fscanf(in, "%s %s %d", s1, s2, &d) != EOF)

/* change underscore back to spaces */
       {for (s = s2; *s; s++)
	    {if (*s == '_')
	        *s = ' ';};

	strcpy(p->f_type, s1);
	strcpy(p->c_type, s2);

	p->size   = d;
	p->active = 1;

	num_c_types++;
	if (num_c_types == MAX_TYPES)
	   {fprintf(stderr, "Too many C types\n");
	    exit(1);};

	if (strcmp(s1, "pointer") == 0)
	   break;
	p++;};

    num_f_types = 0;
    p           = from_f;
    while (fscanf(in, "%d", &d) != EOF)
       {if (d == -1)
	   break;

	p->size   = d;
	p->active = 1;

	num_f_types++;
	if (num_f_types == MAX_TYPES)
	   {fprintf(stderr, "Too many fortran types\n");
	    exit(1);};

	p++;};

    fclose(in);

/* read the file generated by fortran,
 * with the corresponding type and kind information
 */

    in = fopen("from_f", "r");
    if (in == NULL)
       {fprintf(stderr, "Error opening from_f\n");
	exit(1);};

    for (p = from_f; p->active; p++)
        {if (fscanf(in, "%s %d %d %d", s1, &d, &d1, &d2) == EOF)
	    {fprintf(stderr, "Premature EOF reading from_f\n");
	     exit(1);};

	 strcpy(p->f_type, s1);
	 p->kind      = d;
	 p->precision = d1;
	 p->range     = d2;};

    fclose(in);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void match_c_types(void)
   {type_data *p, *q;
    char *real = "real";
    char *s;

/* for each C type, find the corresponding fortran kind */
    for (p = from_c; p->active; p++)
        {q = find_size(from_f, p->f_type, p->size);
	 if (q == NULL)
	    p->kind = -1;
	 else
	    p->kind = q->kind;};

/* for each fortran type, find the corresponding C type */
    for (p = from_f; p->active; p++)
        {if (strcmp(p->f_type, "double") == 0)
	    s = real;
         else
	    s = p->f_type;

	 q = find_size(from_c, s, p->size);
	 if (q == NULL)
	    strcpy(p->c_type, "unknown");
	 else
	    strcpy(p->c_type, q->c_type);};
  
/* generate the complex types based on available C types */
    q = find_c_type(from_c, "float");
    if (q != NULL)
       {p = next_entry(from_c);
	copy_entry(p, q);
	strcpy(p->f_type, "complex");
	strcpy(p->c_type, "complex");};

    q = find_c_type(from_c, "double");
    if (q != NULL)
       {p = next_entry(from_c);
	copy_entry(p, q);
	strcpy(p->f_type, "complex");
	strcpy(p->c_type, "double complex");};

    q = find_c_type(from_c, "long double");
    if (q != NULL)
       {p = next_entry(from_c);
	copy_entry(p, q);
	strcpy(p->f_type, "complex");
	strcpy(p->c_type, "long double complex");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void dump_table(void)
   {int i;
    type_data *p;
  
    printf("from_f\n");
    printf("f_type               kind size c_type\n");
    for (i = 0; i <= MAX_TYPES; i++)
        {p = from_f + i;
	 if (p->active)
	    printf("%-20s %4d %4d %-20s\n",
		   p->f_type, p->kind, p->size, p->c_type);};

    printf("\n");

    printf("from_c\n");
    printf("f_type               kind size c_type\n");
    for (i = 0; i <= MAX_TYPES; i++)
        {p = from_c + i;
	 if (p->active)
	    printf("%-20s %4d %4d %-20s\n",
		   p->f_type, p->kind, p->size, p->c_type);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void dump_header(void)
   {int *ip;
    int err;
    int sizes[MAX_SIZES];
    char lower[MAX_STRING], upper[MAX_STRING];
    type_data *p, *q;
    FILE *out;

    out = fopen("f90types.h", "w");
    if (out == NULL)
       {fprintf(stderr, "Error opening f90types.h\n");
	exit(1);};

    fprintf(out, "%s/* Complex structs */\n\n", c_header);

/* if multiple c types have the same size, only create a struct
 * for the first one
 */
    init_sizes(sizes);
    for (p = from_c; p->active; p++)
        {if (strcmp(p->f_type, "real") != 0)
	    continue;
	 if (size_used(sizes, p->size))
	    continue;

	 q = find_size(from_c, "complex", p->size);
	 add_underscores(q->c_type, lower);

	 fprintf(out, "struct _s_f90_%s {\n", lower);
	 fprintf(out, "  %s real;\n", p->c_type);
	 fprintf(out, "  %s imag;\n", p->c_type);
	 fprintf(out, "};\n\n");
	 add_size(sizes, p->size);};

    fprintf(out, "%s/* Default fortran types */\n\n", c_header);

    p = find_kind(from_f, "integer", 0);
    make_upper(p->f_type, upper);
    fprintf(out, "typedef %s f90_%s;\n", p->c_type, p->f_type);
    fprintf(out, "#define F90_%s_S \"%s\"\n", upper, p->c_type);
    fprintf(out, "\n");

    p = find_kind(from_f, "real", 0);
    make_upper(p->f_type, upper);
    fprintf(out, "typedef %s f90_%s;\n", p->c_type, p->f_type);
    fprintf(out, "#define F90_%s_S \"%s\"\n", upper, p->c_type);
    fprintf(out, "\n");

    p = find_kind(from_f, "double", 0);
    make_upper(p->f_type, upper);
    fprintf(out, "typedef %s f90_%s;\n", p->c_type, p->f_type);
    fprintf(out, "#define F90_%s_S \"%s\"\n", upper, p->c_type);
    fprintf(out, "\n");

    p = find_kind(from_f, "real", 0);
    q = find_size(from_c, "complex", p->size);
    add_underscores(q->c_type, lower);
    fprintf(out, "typedef struct _s_f90_%s f90_complex;\n\n", lower);

    fprintf(out, "%s/* Size based fortran types */\n\n", c_header);

    fprintf(out, "/* The suffix indicates the byte size, not the kind\n");
    fprintf(out, " * f90_real8 is 8 bytes wide, it may happen to be kind=8\n");
    fprintf(out, " */\n\n");

    for (ip = int_sizes; *ip != -1; ip++)
        {p = find_size(from_f, "integer", *ip);
	 if (p == NULL)
	    continue;

	 if (strcmp(p->c_type, "unknown") == 0)
	    continue;

	 make_upper(p->f_type, upper);
	 fprintf(out, "typedef %s f90_%s%d;\n", p->c_type, p->f_type, *ip);
	 fprintf(out, "#define F90_%s%d_S \"%s\"\n", upper, *ip,  p->c_type);
	 fprintf(out, "\n");};

    for (ip = real_sizes; *ip != -1; ip++)
        {p = find_size(from_f, "real", *ip);
	 if (p == NULL)
	    continue;

	 if (strcmp(p->c_type, "unknown") == 0)
	    continue;

	 make_upper(p->f_type, upper);
	 fprintf(out, "typedef %s f90_%s%d;\n", p->c_type, p->f_type, *ip);
	 fprintf(out, "#define F90_%s%d_S \"%s\"\n", upper, *ip,  p->c_type);
	 fprintf(out, "\n");};

    for (ip = real_sizes; *ip != -1; ip++)
        {p = find_size(from_f, "real", *ip);
	 if (p == NULL)
	    continue;

	 if (strcmp(p->c_type, "unknown") == 0)
	    continue;

	 q = find_size(from_c, "complex", *ip);
	 add_underscores(q->c_type, lower);
	 fprintf(out, "typedef struct _s_f90_%s f90_complex%d;\n\n", lower, *ip);};

#if 1
    fprintf(out, "%s/* Kind based fortran types */\n\n", c_header);

    for (p = from_f; p->active; p++)
        {if (p->kind == 0)
	    continue;

	 if (strcmp(p->c_type, "unknown") == 0)
	    continue;

	 make_upper(p->f_type, upper);
	 fprintf(out, "typedef %s f90_%s_kind%d;\n", p->c_type, p->f_type, p->kind);
	 fprintf(out, "#define F90_%s_KIND%d_S \"%s\"\n", upper, p->kind, p->c_type);
	 fprintf(out, "\n");};

    for (p = from_f; p->active; p++)
        {if (p->kind == 0)
	    continue;

	 if (strcmp(p->f_type, "real") != 0)
	    continue;

	 if (strcmp(p->c_type, "unknown") == 0)
	    continue;

	 q = find_size(from_c, "complex", p->size);
	 add_underscores(q->c_type, lower);
	 fprintf(out, "typedef struct _s_f90_%s f90_complex_kind%d;\n",
		 lower, p->kind);
	 fprintf(out, "\n");};
#endif

    fprintf(out, "%s/* external name mangling */\n\n", c_header);

    fprintf(out, "%s\n\n", external_macro);

    err = fclose(out);
    if (err != 0)
       {fprintf(stderr, "Error closing f90types.h\n");
	exit(1);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void dump_module(void)
   {int *ip;
    int err;
    char *s, *t;
    char upper[MAX_STRING];
    type_data *p;
    FILE *out;

    out = fopen("f90kinds.inc", "w");
    if (out == NULL)
       {fprintf(stderr, "Error opening f90kinds.inc\n");
	exit(1);};

    fprintf(out, "! fortran_kind_module\n\n");

    fprintf(out, "! The suffix indicates the byte size, not the kind\n");
    fprintf(out, "! real8 is 8 bytes wide, it may happen to be kind=8\n");
    fprintf(out, "! A -1 value indicates that the width is not available.\n\n");

    fprintf(out, "! Integer KIND Parameters:\n");
    fprintf(out, "      integer, parameter :: int_def = %d\n",
	    find_default_kind("integer"));

    for (ip = int_sizes; *ip != -1; ip++)
        {p = find_size_exp(from_f, "integer", *ip);
	 if (p == NULL)
	    fprintf(out, "      integer, parameter :: int%d = -1\n", *ip);
	 else
	    {fprintf(out, "      integer, parameter :: int%d = %d",
		     *ip, p->kind);
	     fprintf(out, "       ! +/- 10**%d\n", p->precision);};};

    fprintf(out, "\n");

    fprintf(out, "! Real KIND Parameters:\n");
    fprintf(out, "      integer, parameter :: real_def =  %d\n",
	    find_default_kind("real"));

    for (ip = real_sizes; *ip != -1; ip++)
        {p = find_size_exp(from_f, "real", *ip);
	 if (p == NULL)
	    fprintf(out, "      integer, parameter :: real%d = -1\n", *ip);
	 else
	    {fprintf(out, "      integer, parameter :: real%d = %d",
		     *ip, p->kind);
	     fprintf(out, "       ! p=%d  r=%d\n", p->precision, p->range);};};

    fprintf(out, "\n");

    fprintf(out, "!\n!  Names chosen for Fortran 200x compatibility with the\n");
    fprintf(out, "!  ISO_C_BINDING module\n!\n");

    for (p = from_c; p->active; p++)
        {if (strcmp(p->c_type, "pointer") == 0)
	    continue;

/* replace spaces with underscores */
	 for (s = p->c_type, t = upper; *s != '\0'; s++, t++)
	     {*t = toupper(*s);
	      if (*t == ' ')
		 *t = '_';};

	 *t = '\0';
	 fprintf(out, "      integer, parameter :: C_%s = %d\n",
		 upper, p->kind);};

    fprintf(out, "\n");

#if 0
! Logical KIND Parameters:
! integer, parameter :: C_BOOL            = 4

! Character KIND Parameters:
! integer, parameter :: C_CHAR            =

#endif

/* find the integer kind that corresponds the the size of a pointer */
    p = find_c_type(from_c, "pointer");
    p = find_size(from_f, "integer", p->size);

#if 0
    fprintf(out, "  integer, parameter :: C_PTR = %d\n",
	    p->kind);
#else
    fprintf(out, "      type C_PTR\n");
    fprintf(out, "        sequence  ! allow it to be put in other sequenced types\n");
    fprintf(out, "        integer(%d) :: addr\n", p->kind);
    fprintf(out, "      end type C_PTR\n");
#endif

    err = fclose(out);
    if (err != 0)
       {fprintf(stderr, "Error closing f90kinds.inc\n");
	exit(1);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int argc, char **argv)
  {

   read_input();
   match_c_types();

   dump_table();
   dump_header();
   dump_module();

   return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
