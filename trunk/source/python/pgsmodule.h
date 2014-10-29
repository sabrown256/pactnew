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

#ifndef PY_pgs_H
#define PY_pgs_H

#include "py_int.h"
#include "utilpgs.h"

/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pgs.C_declaration) */
/*
 * Source Version: 2.1
 * Software Release: LLNL-CODE-422942
 *
 * include cpyright.h
 *
 */

#define PY_ARRAY_UNIQUE_SYMBOL PP_
#ifdef HAVE_PY_NUMERIC
# include <Numeric/arrayobject.h>
#endif

/* DO-NOT-DELETE splicer.end(pgs.C_declaration) */

/*--------------------------------------------------------------------------*/
/*                              OBJECT pdbdata                              */
/*--------------------------------------------------------------------------*/

/* #define PP_pdbdata_Check(op)    (PY_TYPE(op) == &PP_pdbdata_Type) */

typedef struct s_PP_pdbdataObject *PP_pdbdataObjectp;

typedef struct s_PP_pdbdataObject {
  PyObject_HEAD
  PP_HEADER(void)
} PP_pdbdataObject;

/* staticforward PyTypeObject PP_pdbdata_Type;*/
extern PyTypeObject PP_pdbdata_Type;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*                              OBJECT hashtab                              */
/*--------------------------------------------------------------------------*/

/* #define PP_hashtab_Check(op)    (PY_TYPE(op) == &PP_hashtab_Type) */

typedef struct s_PP_hashtabObject *PP_hashtabObjectp;

typedef struct s_PP_hashtabObject {
  PyObject_HEAD
  PP_HEADER(hasharr)
} PP_hashtabObject;

/* staticforward PyTypeObject PP_hashtab_Type;*/
extern PyTypeObject PP_hashtab_Type;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*                              OBJECT PDBfile                              */
/*--------------------------------------------------------------------------*/

/* #define PP_PDBfile_Check(op)    (PY_TYPE(op) == &PP_PDBfile_Type) */

typedef struct s_PP_PDBfileObject *PP_PDBfileObjectp;

typedef struct s_PP_PDBfileObject {
  PyObject_HEAD
  PDBfile *object;
    PP_file *fileinfo;
    PP_form form;
} PP_PDBfileObject;

/* staticforward PyTypeObject PP_PDBfile_Type;*/
extern PyTypeObject PP_PDBfile_Type;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


extern PP_PDBfileObject *PP_vif_obj;
extern PyObject *PP_open_file_dict;

PY_memdes *PP_memdes_newobj(PY_memdes *obj, memdes *desc);

PP_hashtabObject *PP_hashtab_newobj(PP_hashtabObject *obj,
                                    hasharr *tab);

PP_pdbdataObject *PP_pdbdata_newobj(PP_pdbdataObject *obj,
                                    void *vr, char *type, long nitems,
                                    dimdes *dims, defstr *dp, PP_file *fileinfo,
                                    PY_defstr *dpobj, PyObject *parent);

PP_PDBfileObject *_PP_PDBfile_newobj(PP_PDBfileObject *obj,
                                     PP_file *fileinfo);

/* pppdbr.c */

PyObject *_PP_read_data(PP_file *fileinfo, char *name, char *intype,
                        int nd, long *ind, PP_form *form);

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

extern void
  init_pgs(void);

#ifdef __cplusplus
}
#endif
#endif  /* !PY_pgs_H */
/*--------------------------------------------------------------------------*/
/*                               MODULE_TAIL                                */
/*--------------------------------------------------------------------------*/
