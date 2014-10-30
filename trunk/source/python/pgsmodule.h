/*
 * Source Version: 2.1
 * Software Release: LLNL-CODE-422942
 *
 * include cpyright.h
 */

#ifndef PY_pgs_H
#define PY_pgs_H

#include "py_int.h"

#define PY_ARRAY_UNIQUE_SYMBOL PP_
#ifdef HAVE_PY_NUMERIC
# include <Numeric/arrayobject.h>
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

typedef struct s_PP_pdbdataObject *PP_pdbdataObjectp;

typedef struct s_PP_pdbdataObject {
  PyObject_HEAD
  PP_HEADER(void)
} PP_pdbdataObject;

extern PyTypeObject PP_pdbdata_Type;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/* PDBPDBDATA.C declarations */

extern PP_pdbdataObject
 *PP_pdbdata_newobj(PP_pdbdataObject *obj,
		    void *vr, char *type, long nitems,
		    dimdes *dims, defstr *dp, PP_file *fileinfo,
		    PY_defstr *dpobj, PyObject *parent);


extern void
  init_pgs(void);

#ifdef __cplusplus
}
#endif
#endif  /* !PY_pgs_H */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
