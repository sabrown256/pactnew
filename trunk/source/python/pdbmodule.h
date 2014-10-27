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

#include "py_int.h"

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
/*                              OBJECT defstr                               */
/*--------------------------------------------------------------------------*/

/* #define PP_defstr_Check(op)    (PY_TYPE(op) == &PP_defstr_Type) */

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

/* #define PP_pdbdata_Check(op)    (PY_TYPE(op) == &PP_pdbdata_Type) */

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

/* #define PP_hashtab_Check(op)    (PY_TYPE(op) == &PP_hashtab_Type) */

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
/*                               OBJECT assoc                               */
/*--------------------------------------------------------------------------*/

/* #define PP_assoc_Check(op)    (PY_TYPE(op) == &PP_assoc_Type) */

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

/* #define PP_memdes_Check(op)    (PY_TYPE(op) == &PP_memdes_Type) */

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

/* #define PP_PDBfile_Check(op)    (PY_TYPE(op) == &PP_PDBfile_Type) */

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

PY_hasharr *PY_hasharr_newobj(PY_hasharr *obj,
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
