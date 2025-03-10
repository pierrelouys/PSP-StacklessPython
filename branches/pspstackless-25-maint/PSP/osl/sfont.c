
/**
 * @file sfont.c
 */

/**********************************************************************

  Created: 05 Aug 2009

    Copyright (C) 200s sakya_tg@yahoo.it

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

#include "sfont.h"
#include "osl.h"

//==========================================================================
// Init & free

static void sfont_dealloc(PySFont *self)
{
    if (self->pSFont)
       oslDeleteSFont(self->pSFont);

    self->ob_type->tp_free((PyObject*)self);
}

static PyObject* sfont_new(PyTypeObject *type,
                          PyObject *args,
                          PyObject *kwargs)
{
    PySFont *self;

    self = (PySFont*)type->tp_alloc(type, 0);

    if (self)
       self->pSFont = NULL;

    return (PyObject*)self;
}

static int sfont_init(PySFont *self,
                      PyObject *args,
                      PyObject *kwargs)
{
    char *filename;
	int format = OSL_PF_8888;

    if (!PyArg_ParseTuple(args, "s|i:__init__", &filename, &format))
       return -1;

    self->pSFont = oslLoadSFontFile(filename, format);

    if (!self->pSFont)
    {
       PyErr_SetString(osl_Error, "Could not load sfont file");
       return -1;
    }

    return 0;
}

//==========================================================================
// Methods

static PyObject* sfont_drawstring(PySFont *self,
                                  PyObject *args,
                                  PyObject *kwargs)
{
	int x,y;
    char *text;
    if (!PyArg_ParseTuple(args, "iis:drawString", &x, &y, &text))
       return NULL;

    int result = oslSFontDrawText(self->pSFont, x, y, text);

    return Py_BuildValue("i", result);
}

static PyObject* sfont_measuretext(PySFont *self,
                                   PyObject *args,
                                   PyObject *kwargs)
{
    char *text;
    if (!PyArg_ParseTuple(args, "s:measureText", &text))
       return NULL;

    int result = oslGetSFontTextWidth(self->pSFont, text);

    return Py_BuildValue("i", result);
}

//==========================================================================

static PyMethodDef sfont_methods[] = {
   { "drawString", (PyCFunction)sfont_drawstring, METH_VARARGS, "" },
   { "measureText", (PyCFunction)sfont_measuretext, METH_VARARGS, "" },

   { NULL }
};

static PyTypeObject PySFontType = {
   PyObject_HEAD_INIT(NULL)
   0,
   "osl.SFont",
   sizeof(PySFont),
   0,
   (destructor)sfont_dealloc,
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
   "SFont objects",
   0,
   0,
   0,
   0,
   0,
   0,
   sfont_methods,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   (initproc)sfont_init,
   0,
   sfont_new,
};

PyTypeObject* PPySFontType = &PySFontType;

#ifdef _GNUC
static const char* _rcsid_sfont __attribute__((unused)) = "$Id$";
#endif
