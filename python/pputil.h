/*
 * PPUTIL.H - supporting PACT PYTHON bindings
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "ppcommon.h"

/*--------------------------------------------------------------------------*/

/*                           DEFINED CONSTANTS                              */

/*--------------------------------------------------------------------------*/

/* control wether a pact allocated thing is required.
 * usually necessary when writing a pointer out to disk.
 * if just copying data, then the extra pact info (length, etc)
 * is not needed
 */
#define PP_GC_NO  0
#define PP_GC_YES 1

#define MAXDIM 7

/* native types in coercion order */

enum e_PP_types {
    PP_NONE_I,
    PP_CHAR_I,
    PP_UBYTE_I,
    PP_SBYTE_I,
    PP_SHORT_I,
    PP_INT_I,
    PP_LONG_I,
    PP_LONG_LONG_I,
    PP_FLOAT_I,
    PP_DOUBLE_I,
    PP_CFLOAT_I,
    PP_CDOUBLE_I,
    PP_INSTANCE_I,
    PP_PDBDATA_I,
    PP_UNKNOWN_I,
    PP_NUM_TYPES
};

enum e_PP_object_kind
   {AS_NONE    = 0,
    AS_PDBDATA = 1,
    AS_OBJECT  = 2,
    AS_TUPLE   = 3,
    AS_LIST    = 4,
    AS_DICT    = 5,
    AS_ARRAY   = 6};

typedef enum e_PP_object_kind PP_object_kind;

#define PP_HEADER(t)   \
    t *data;           \
    char *type;        \
    long nitems;       \
    dimdes *dims;      \
    defstr *dp;        \
    PP_file *fileinfo; \
    PY_defstr *dpobj;  \
    PyObject *parent;

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

#define PP_error_from_pdb() \
    PyErr_SetString(PP_error_user, PD_get_error())

#define PP_CAST_TYPE(t, d, vg, vl, err_obj, ret_val)                         \
    {if (d->cast_offs < 0L)                                                  \
        t = d->type;                                                         \
     else                                                                    \
        {t = DEREF((char *) vl + d->cast_offs);                              \
         if (t == NULL)                                                      \
            {if (DEREF(vg) != NULL)                                          \
                {PP_error_set_user(err_obj, "bad cast");return ret_val;}     \
             else                                                            \
                t = d->type;};};}

/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/

typedef enum e_PP_types PP_types;

typedef struct s_PP_descr PP_descr;

typedef PyObject *(*PP_newfunc) (void *vr, long nitems, dimdes * dims,
                                 int type, int need_array);
typedef int (*PP_copyfunc) (PyObject *obj, void *vr, long nitems,
                            int type);
typedef int (*PP_getfunc) (PyObject *obj, void *vr, long nitems, PP_types type,
                           int flag);
typedef dimdes *(*PP_dimsfunc) (PyObject *obj);
typedef void (*PP_relfunc) (void *vr);

typedef struct s_PP_type_entry PP_type_entry;
typedef struct s_PP_class_descr PP_class_descr;
typedef struct s_PP_form PP_form;
typedef struct s_PP_file PP_file;

typedef int (*PP_pack_func) (void *vr, PyObject *obj, long nitems, PP_types tc);
typedef PyObject *(*PP_unpack_func) (void *vr, long nitems);
typedef PP_descr *(*PP_get_descr)(PP_file *fileinfo, PyObject *obj);

struct s_PP_descr
   {PP_types typecode;
    int      bpi;
    char    *type;
    dimdes  *dims;
    void    *data;};

struct s_PP_type_entry {
    PP_types        typecode;
    int             sequence;  /* TRUE if type knows how to create a sequence (PyString) */
    PP_descr       *descr;
    PyTypeObject   *obtyp;
    PP_pack_func    pack;
    PP_unpack_func  unpack;
    PP_get_descr    get_descr;
    PP_class_descr *cdescr;    /* Used with Python instances */
};

struct s_PP_class_descr {
    PyTypeObject    *cls;
    PyFunctionObject *ctor;
    char             *type;
    PP_descr         *descr;
};

struct s_PP_form
   {int array_kind;
    int struct_kind;
    int scalar_kind;};

struct s_PP_file {
    PDBfile *file;
    hasharr *type_map;
    hasharr *object_map;
    hasharr *class_map;
    hasharr *deftypes;   /* table of defstr types */
};

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

extern PyObject
 *PP_error_internal,
 *PP_error_user;

extern PDBfile
 *PP_vif;

extern PP_file
 *PP_vif_info;

extern PP_form
 PP_global_form;

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

/* PPARRAYS.C declarations */

extern void
 _PP_init_numpy(void),
 _PP_init_numpy_types(PP_file *fileinfo);

extern int
 iarray_extractor(PyObject *obj, void *arg),
 pplong_extractor(PyObject *obj, void *arg),
 darray_extractor(PyObject *obj, void *arg),
 REAL_array_extractor(PyObject *obj, void *arg);

extern PyObject
 *PP_array_unpack(void *vr, long nitems, PP_types tc);


/* PPHASHARR.C declarations */

extern void
 _PY_hasharr_entry(PP_file *fileinfo);

extern int
 PP_update_hasharr(hasharr *hasharr, PyObject *dict);

extern PyObject
 *_PP_unpack_hasharr(void *p, long nitems),
 *PP_unpack_hasharr_haelem(char *type, void *vr);


/* PPTYPES.C declarations */

extern int
 _PY_pact_constants(PyObject *m),
 _PY_pact_type_system(PyObject *m);

extern void
 _PP_rl_descr(PP_descr *descr),
 PP_register_type(PP_file *fileinfo, PP_type_entry *entry),
 PP_register_object(PP_file *fileinfo, PP_type_entry *entry),
 PP_init_type_map_basic(PP_file *fileinfo),
 PP_init_type_map_instance(PP_file *fileinfo, PP_class_descr *cdescr),
 PP_init_type_map(void),
 _PP_init_file(PDBfile *file),
 _PP_close_file(PP_file *fileinfo);

extern int
 PP_copy_obj_to_mem(PyObject *obj, PP_file *fileinfo, PP_type_entry *entry, long nitems,
                    void *vr, dimdes *dims),
 PP_unpack_sequence(PyObject *obj, PP_type_entry *entry,
		    void **vr, PP_file *fileinfo),
 PP_get_object_data(PyObject *obj, PP_type_entry *entry, long nitems,
		    void **vr, int gc),
 PP_make_data(PyObject *obj, PP_file *fileinfo, char *type,
	      dimdes *dims, void **vr),
 PP_get_object_length(PyObject *obj),
 _PP_rl_class_descr(haelem *hp, void *a),
 PP_rl_type_entry(haelem *hp, void *a),
 _PP_decref_object(haelem *hp, void *a);

extern PP_descr
 *PP_make_descr(PP_types typecode, char *type, long bpi),
 *_PP_mk_descr(PP_types typecode, long bpi, char *type,
               dimdes *dims, void *data),
 *_PP_get_object_descr(PP_file *fileinfo, PyObject *obj, PP_type_entry *entry),
 *PP_get_object_descr(PP_file *fileinfo, PyObject *obj);

PP_type_entry
 *PP_inquire_type(PP_file *fileinfo, char *ctype),
 *PP_inquire_object(PP_file *fileinfo, PyObject *obj),
 *PP_make_type_entry(PP_types typecode, int sequence, PP_descr *descr,
                     PyTypeObject *otype, PP_pack_func pack,
                     PP_unpack_func unpack, PP_get_descr get_descr);

PP_class_descr
 *PP_make_class_descr(PyTypeObject *cls, char *type, PyFunctionObject *ctor);

extern PyObject
 *PP_copy_mem_to_obj(PDBfile *file, void *vr, dimdes *dims, long nitems,
		     char *type, PP_type_entry *entry, PP_form *form,
		     int sequence);

PP_file *_PP_open_vif(char *name);

PP_descr *PP_outtype_descr(PDBfile *fp, PP_descr *descr, char *type);


/* PPUTIL.C declarations */

extern void
 *_PY_opt_generic(void *x, bind_opt wh, void *a),
 PY_self_free(void *o),
 _PP_Py_decref(PyObject *obj),
 PP_free(void *p),
 PP_error_set(PyObject *errobj, PyObject *obj, const char *fmt, ...),
 PP_error_set_user(PyObject *obj, const char *fmt, ...);

extern int
 PY_setup_pact(PyObject *m, PyObject *d),
 PP_alloc_data(char *type, long nitems, PP_file *fileinfo, void **vr),
 PP_num_indirection(char *s),
 PP_dimdes_to_ind(dimdes *dims, long *ind),
 PP_obj_to_ind(PyObject *indobj, long *ind),
 _PP_assign_form(PP_form *form, int array, int structure, int scalar),
 PP_convert_pdbfile(PyObject *obj, void **addr);

extern char
 *PY_get_string(PyObject *o),
 *PP_add_indirection(char *type, int n);

extern dimdes
 *PP_ind_to_dimdes(int nd, long *ind);

extern PyObject
 *PY_strings_tuple(char **sa, int ns, int fr),
 *PY_build_object(char *nm, ...),
 *PP_dimdes_to_obj(dimdes *dims),
 *_PP_find_file_obj(PDBfile *fp);


/* PPPDBR.C declarations */

extern PyObject
 *_PP_read_data(PP_file *fileinfo, char *name, char *intype,
		int nd, long *ind, PP_form *form);


/* PYPDBD.C declarations */

extern long
 _PP_rel_syment(hasharr *host_chart, char *vr, long nitems, char *outtype);


/* PYPDBL.C declarations */

extern PyObject
 *_PP_wr_syment(PP_file *fileinfo, char *outtype, dimdes *dims,
		long nitems, void *vr, PP_form *form);


/* PYPDBR.C declarations */

extern long
 _PP_rd_syment(PyObject *obj, PP_file *fileinfo,
	       char *outtype, dimdes *dims, long nitems, void *vr);


/* STRUCT.C declarations */

extern PP_unpack_func
 _PP_get_unpack_func(PP_types tc);

extern PP_pack_func
 _PP_get_pack_func(PP_types tc);


