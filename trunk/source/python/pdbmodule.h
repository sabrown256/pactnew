/*--------------------------------------------------------------------------*/

/* This is generated code.
 * Any edits must be made between the splicer.begin and splicer.end blocks.
 * All other edits will be lost.
 * Once a block is edited remove the 'UNMODIFIED' on the splicer.begin comment
 * to allow the block to be preserved when it is regenerated.
 */
/*
 * Source Version: 9.0
 * Software Release: LLNL-CODE-422942
 *
 * include cpyright.h
 */
/*--------------------------------------------------------------------------*/
/*                              MODULE_HEADER                               */
/*--------------------------------------------------------------------------*/

#ifndef PY_pdb_H
#define PY_pdb_H

#include <Python.h>
#include <pdb_int.h>
#include <pputil.h>

/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.C_declaration) */

#include <patchlevel.h>

#if PY_VERSION_HEX < 0x02050000 && !defined(PY_SSIZE_T_MIN)
typedef int Py_ssize_t;
#define PY_SSIZE_T_MAX INT_MAX
#define PY_SSIZE_T_MIN INT_MIN
#endif

/* DO-NOT-DELETE splicer.end(pdb.C_declaration) */

/*--------------------------------------------------------------------------*/
/*                               OBJECT field                               */
/*--------------------------------------------------------------------------*/

/* #define PP_field_Check(op)    ((op)->ob_type == &PP_field_Type) */

typedef struct s_PP_fieldObject *PP_fieldObjectp;

typedef struct s_PP_fieldObject {
  PyObject_HEAD
  PM_field *opers;
/* DO-NOT-DELETE splicer.begin(pdb.field.extra_object) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.field.extra_object) */
} PP_fieldObject;

/* staticforward PyTypeObject PP_field_Type;*/
extern PyTypeObject PP_field_Type;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.field.C_declaration) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.field.C_declaration) */

/*--------------------------------------------------------------------------*/
/*                           OBJECT mesh_topology                           */
/*--------------------------------------------------------------------------*/

/* #define PP_mesh_topology_Check(op)    ((op)->ob_type == &PP_mesh_topology_Type) */

typedef struct s_PP_mesh_topologyObject *PP_mesh_topologyObjectp;

typedef struct s_PP_mesh_topologyObject {
  PyObject_HEAD
  PM_mesh_topology *mt;
/* DO-NOT-DELETE splicer.begin(pdb.mesh_topology.extra_object) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.mesh_topology.extra_object) */
} PP_mesh_topologyObject;

/* staticforward PyTypeObject PP_mesh_topology_Type;*/
extern PyTypeObject PP_mesh_topology_Type;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.mesh_topology.C_declaration) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.mesh_topology.C_declaration) */

/*--------------------------------------------------------------------------*/
/*                                OBJECT set                                */
/*--------------------------------------------------------------------------*/

/* #define PP_set_Check(op)    ((op)->ob_type == &PP_set_Type) */

typedef struct s_PP_setObject *PP_setObjectp;

typedef struct s_PP_setObject {
  PyObject_HEAD
  PM_set *set;
/* DO-NOT-DELETE splicer.begin(pdb.set.extra_object) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.set.extra_object) */
} PP_setObject;

/* staticforward PyTypeObject PP_set_Type;*/
extern PyTypeObject PP_set_Type;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.set.C_declaration) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.set.C_declaration) */

/*--------------------------------------------------------------------------*/
/*                              OBJECT mapping                              */
/*--------------------------------------------------------------------------*/

/* #define PP_mapping_Check(op)    ((op)->ob_type == &PP_mapping_Type) */

typedef struct s_PP_mappingObject *PP_mappingObjectp;

typedef struct s_PP_mappingObject {
  PyObject_HEAD
  PM_mapping *map;
/* DO-NOT-DELETE splicer.begin(pdb.mapping.extra_object) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.mapping.extra_object) */
} PP_mappingObject;

/* staticforward PyTypeObject PP_mapping_Type;*/
extern PyTypeObject PP_mapping_Type;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.mapping.C_declaration) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.mapping.C_declaration) */

/*--------------------------------------------------------------------------*/
/*                              OBJECT defstr                               */
/*--------------------------------------------------------------------------*/

/* #define PP_defstr_Check(op)    ((op)->ob_type == &PP_defstr_Type) */

typedef struct s_PP_defstrObject *PP_defstrObjectp;

typedef struct s_PP_defstrObject {
  PyObject_HEAD
  defstr *dp;
/* DO-NOT-DELETE splicer.begin(pdb.defstr.extra_object) */
    PP_file *fileinfo;
    hasharr *host_chart;
    PyTypeObject *ctor;
/* DO-NOT-DELETE splicer.end(pdb.defstr.extra_object) */
} PP_defstrObject;

/* staticforward PyTypeObject PP_defstr_Type;*/
extern PyTypeObject PP_defstr_Type;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.defstr.C_declaration) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.defstr.C_declaration) */

/*--------------------------------------------------------------------------*/
/*                              OBJECT pdbdata                              */
/*--------------------------------------------------------------------------*/

/* #define PP_pdbdata_Check(op)    ((op)->ob_type == &PP_pdbdata_Type) */

typedef struct s_PP_pdbdataObject *PP_pdbdataObjectp;

typedef struct s_PP_pdbdataObject {
  PyObject_HEAD
/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.extra_object) */
  PP_HEADER(void)
/* DO-NOT-DELETE splicer.end(pdb.pdbdata.extra_object) */
} PP_pdbdataObject;

/* staticforward PyTypeObject PP_pdbdata_Type;*/
extern PyTypeObject PP_pdbdata_Type;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.C_declaration) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.pdbdata.C_declaration) */

/*--------------------------------------------------------------------------*/
/*                              OBJECT hashtab                              */
/*--------------------------------------------------------------------------*/

/* #define PP_hashtab_Check(op)    ((op)->ob_type == &PP_hashtab_Type) */

typedef struct s_PP_hashtabObject *PP_hashtabObjectp;

typedef struct s_PP_hashtabObject {
  PyObject_HEAD
/* DO-NOT-DELETE splicer.begin(pdb.hashtab.extra_object) */
  PP_HEADER(hasharr)
/* DO-NOT-DELETE splicer.end(pdb.hashtab.extra_object) */
} PP_hashtabObject;

/* staticforward PyTypeObject PP_hashtab_Type;*/
extern PyTypeObject PP_hashtab_Type;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.hashtab.C_declaration) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.hashtab.C_declaration) */

/*--------------------------------------------------------------------------*/
/*                              OBJECT hasharr                              */
/*--------------------------------------------------------------------------*/

/* #define PP_hasharr_Check(op)    ((op)->ob_type == &PP_hasharr_Type) */

typedef struct s_PP_hasharrObject *PP_hasharrObjectp;

typedef struct s_PP_hasharrObject {
  PyObject_HEAD
/* DO-NOT-DELETE splicer.begin(pdb.hasharr.extra_object) */
  PP_HEADER(hasharr)
/* DO-NOT-DELETE splicer.end(pdb.hasharr.extra_object) */
} PP_hasharrObject;

/* staticforward PyTypeObject PP_hasharr_Type;*/
extern PyTypeObject PP_hasharr_Type;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.hasharr.C_declaration) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.hasharr.C_declaration) */

/*--------------------------------------------------------------------------*/
/*                               OBJECT assoc                               */
/*--------------------------------------------------------------------------*/

/* #define PP_assoc_Check(op)    ((op)->ob_type == &PP_assoc_Type) */

typedef struct s_PP_assocObject *PP_assocObjectp;

typedef struct s_PP_assocObject {
  PyObject_HEAD
  pcons *alist;
/* DO-NOT-DELETE splicer.begin(pdb.assoc.extra_object) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.assoc.extra_object) */
} PP_assocObject;

/* staticforward PyTypeObject PP_assoc_Type;*/
extern PyTypeObject PP_assoc_Type;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.assoc.C_declaration) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.assoc.C_declaration) */

/*--------------------------------------------------------------------------*/
/*                              OBJECT memdes                               */
/*--------------------------------------------------------------------------*/

/* #define PP_memdes_Check(op)    ((op)->ob_type == &PP_memdes_Type) */

typedef struct s_PP_memdesObject *PP_memdesObjectp;

typedef struct s_PP_memdesObject {
  PyObject_HEAD
  memdes *desc;
/* DO-NOT-DELETE splicer.begin(pdb.memdes.extra_object) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.memdes.extra_object) */
} PP_memdesObject;

/* staticforward PyTypeObject PP_memdes_Type;*/
extern PyTypeObject PP_memdes_Type;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.memdes.C_declaration) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.memdes.C_declaration) */

/*--------------------------------------------------------------------------*/
/*                              OBJECT PDBfile                              */
/*--------------------------------------------------------------------------*/

/* #define PP_PDBfile_Check(op)    ((op)->ob_type == &PP_PDBfile_Type) */

typedef struct s_PP_PDBfileObject *PP_PDBfileObjectp;

typedef struct s_PP_PDBfileObject {
  PyObject_HEAD
  PDBfile *object;
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.extra_object) */
    PP_file *fileinfo;
    PP_form form;
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.extra_object) */
} PP_PDBfileObject;

/* staticforward PyTypeObject PP_PDBfile_Type;*/
extern PyTypeObject PP_PDBfile_Type;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.C_declaration) */

extern PP_PDBfileObject *PP_vif_obj;
extern PyObject *PP_open_file_dict;

PP_memdesObject *PP_memdes_newobj(PP_memdesObject *obj, memdes *desc);

PP_defstrObject *PP_defstr_newobj(PP_defstrObject *obj, defstr *dp,
                                  PP_file *fileinfo);

PP_hashtabObject *PP_hashtab_newobj(PP_hashtabObject *obj,
                                    hasharr *tab);

PP_hasharrObject *PP_hasharr_newobj(PP_hasharrObject *obj,
                                    hasharr *tab);

PP_pdbdataObject *PP_pdbdata_newobj(PP_pdbdataObject *obj,
                                    void *vr, char *type, long nitems,
                                    dimdes *dims, defstr *dp, PP_file *fileinfo,
                                    PP_defstrObject *dpobj, PyObject *parent);

PP_PDBfileObject *_PP_PDBfile_newobj(PP_PDBfileObject *obj,
                                     PP_file *fileinfo);

/* pppdbr.c */

PyObject *_PP_read_data(PP_file *fileinfo, char *name, char *intype,
                        int nd, long *ind, PP_form *form);

/* ppdefstr.c */

PyTypeObject *PP_defstr_mk_ctor(PP_defstrObject *dpobj);
void _PP_rl_defstr(PP_defstrObject *dpobj);
PP_defstrObject *_PP_defstr_make_singleton(
    PP_defstrObject *self,
    char *name, PyObject *members, PP_file *fileinfo);
PP_defstrObject *_PP_defstr_find_singleton(
    char *name, defstr *dp, PP_file *fileinfo);


PyObject *PP_form_object(void *vr, char *type, long nitems,
                         dimdes *dims, defstr *dp, PP_file *fileinfo,
                         PP_defstrObject *dpobj, PyObject *parent,
                         PP_form *form);

/* DO-NOT-DELETE splicer.end(pdb.PDBfile.C_declaration) */

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

extern void
  init_pdb(void);

#ifdef __cplusplus
}
#endif
#endif  /* !PY_pdb_H */
/*--------------------------------------------------------------------------*/
/*                               MODULE_TAIL                                */
/*--------------------------------------------------------------------------*/
