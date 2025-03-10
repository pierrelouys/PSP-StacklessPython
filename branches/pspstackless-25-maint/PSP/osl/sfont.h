#ifndef _PSFONT_H
#define _PSFONT_H

#include <Python.h>
#include <oslib/oslib.h>

typedef struct
{
      PyObject_HEAD

      OSL_SFONT *pSFont;
} PySFont;

extern PyTypeObject* PPySFontType;

#endif /* _PSFONT_H */
