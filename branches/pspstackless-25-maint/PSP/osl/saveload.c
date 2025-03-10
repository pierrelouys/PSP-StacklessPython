
/**
 * @file saveload.c
 */

/**********************************************************************

  Created: 24 Feb 2007

    Copyright (C) 2007 fraca7@free.fr

    This library  is free software; you can  redistribute it and/or
    modify  it under  the terms  of the  GNU Lesser  General Public
    License as  published by  the Free Software  Foundation; either
    version  2.1 of  the License,  or  (at your  option) any  later
    version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY;  without even the implied warranty of
    MERCHANTABILITY or  FITNESS FOR A PARTICULAR  PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You  should have  received a  copy  of the  GNU Lesser  General
    Public License  along with this  library; if not, write  to the
    Free  Software Foundation,  Inc., 59  Temple Place,  Suite 330,
    Boston, MA 02111-1307 USA

**********************************************************************/
// $Id$

#include "saveload.h"
#include "osl.h"

//==========================================================================
// Init & free

static void saveload_dealloc(PySaveLoad  *self)
{
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject* saveload_new(PyTypeObject *type,
                              PyObject *args,
                              PyObject *kwargs)
{
    PySaveLoad *self;

    self = (PySaveLoad*)type->tp_alloc(type, 0);

    if (self)
       self->pSaveLoad = NULL;

    return (PyObject*)self;
}

static int saveload_init(PySaveLoad *self,
                         PyObject *args,
                         PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":__init__"))
       return -1;

    self->pSaveLoad = NULL;

    return 0;
}

//==========================================================================
// Methods


static PyMethodDef saveload_methods[] = {
   { NULL }
};

static PyTypeObject PySaveLoadType = {
   PyObject_HEAD_INIT(NULL)
   0,
   "osl.SaveLoad",
   sizeof(PySaveLoad),
   0,
   (destructor)saveload_dealloc,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
   "SaveLoad objects",
   0,
   0,
   0,
   0,
   0,
   0,
   saveload_methods,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   (initproc)saveload_init,
   0,
   saveload_new,
};

PyTypeObject* PPySaveLoadType = &PySaveLoadType;

#ifdef _GNUC
static const char* _rcsid_saveload __attribute__((unused)) = "$Id$";
#endif
