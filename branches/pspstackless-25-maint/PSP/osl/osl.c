
/**
 * @file osl.c
 */

/**********************************************************************

  Created: 17 Feb 2007

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

#include <oslib/oslib.h>
#include "osl.h"
#include "image.h"
#include "ctrl.h"
#include "sound.h"
#include "map.h"
#include "font.h"

#ifdef WITH_OSLIB_MOD
#include "sfont.h"
//#include "saveload.h"
#endif

PyObject* osl_Error = NULL;

#define FPS_UPDATE_RATE 0.25
static int FPSCounter		  = 0;
static clock_t lastFPSUpdate  = 0;
static int FPS			      = 0;

//==========================================================================
// Functions

static PyObject* osl_mustQuit(PyObject *self,
                              PyObject *args,
                              PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":mustQuit"))
       return NULL;

    return Py_BuildValue("i", osl_quit);
}

static PyObject* osl_doQuit(PyObject *self,
                            PyObject *args,
                            PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":doQuit"))
       return NULL;

    oslQuit();

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_safeQuit(PyObject *self,
                            PyObject *args,
                            PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":safeQuit"))
       return NULL;

    osl_quit = 1;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_initGfx(PyObject *self,
                             PyObject *args,
                             PyObject *kwargs)
{
    int pf = OSL_PF_8888;
    int db = 1;

    if (!PyArg_ParseTuple(args, "|ii:initGfx", &pf, &db))
       return NULL;

    oslInitGfx(pf, db);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_startDrawing(PyObject *self,
                                  PyObject *args,
                                  PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":startDrawing"))
       return NULL;

    oslStartDrawing();

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_syncDrawing(PyObject *self,
                                 PyObject *args,
                                 PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":syncDrawing"))
       return NULL;

    oslSyncDrawing();

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_endDrawing(PyObject *self,
                                PyObject *args,
                                PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":endDrawing"))
       return NULL;

    oslEndDrawing();

	//FPS:
	FPSCounter++;
    clock_t curticks = clock();
    float tPassed = (float)(curticks - lastFPSUpdate)/(float)CLOCKS_PER_SEC;
	if (tPassed >= FPS_UPDATE_RATE)
	{
		FPS = (int)((float)FPSCounter * (1.0f / tPassed));
		FPSCounter = 0;
        lastFPSUpdate = curticks;
	}

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_endFrame(PyObject *self,
                                PyObject *args,
                                PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":endFrame"))
       return NULL;

    oslEndFrame();

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_waitVSync(PyObject *self,
                               PyObject *args,
                               PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":waitVSync"))
       return NULL;

    oslWaitVSync();

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_swapBuffers(PyObject *self,
                                 PyObject *args,
                                 PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":swapBuffers"))
       return NULL;

    oslSwapBuffers();

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_endGfx(PyObject *self,
                            PyObject *args,
                            PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":endGfx"))
       return NULL;

    oslEndGfx();

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_syncFrame(PyObject *self,
                               PyObject *args,
                               PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":syncFrame"))
       return NULL;

    int result = oslSyncFrame();

    return Py_BuildValue("i", result);
}

static PyObject* osl_setFrameskip(PyObject *self,
							      PyObject *args,
								  PyObject *kwargs)
{
	int frameskip;
    if (!PyArg_ParseTuple(args, "i:setFrameskip",
		                  &frameskip))
       return NULL;

    oslSetFrameskip(frameskip);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_setMaxFrameskip(PyObject *self,
							      PyObject *args,
								  PyObject *kwargs)
{
	int frameskip;
    if (!PyArg_ParseTuple(args, "i:setMaxFrameskip",
		                  &frameskip))
       return NULL;

    oslSetMaxFrameskip(frameskip);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_syncFrameEx(PyObject *self,
                                 PyObject *args,
                                 PyObject *kwargs)
{
    int skip, maxskip, vsync;

    if (!PyArg_ParseTuple(args, "iii:syncFrameEx", &skip, &maxskip, &vsync))
       return NULL;

    oslSyncFrameEx(skip, maxskip, vsync);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_setTransparentColor(PyObject *self,
                                         PyObject *args,
                                         PyObject *kwargs)
{
    unsigned long color;

    if (!PyArg_ParseTuple(args, "k:setTransparentColor", &color))
       return NULL;

    oslSetTransparentColor(color);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_setQuitOnLoadFailure(PyObject *self,
							              PyObject *args,
								          PyObject *kwargs)
{
	int enabled;
    if (!PyArg_ParseTuple(args, "i:setQuitOnLoadFailure",
		                  &enabled))
       return NULL;

    oslSetQuitOnLoadFailure(enabled);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_disableTransparentColor(PyObject *self,
                                             PyObject *args,
                                             PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":disableTransparentColor"))
       return NULL;

    oslDisableTransparentColor();

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_drawLine(PyObject *self,
                              PyObject *args,
                              PyObject *kwargs)
{
    int x1, y1, x2, y2;
    unsigned long color;

    if (!PyArg_ParseTuple(args, "iiiik:drawLine", &x1, &y1, &x2, &y2, &color))
       return NULL;

    oslDrawLine(x1, y1, x2, y2, color);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_drawRect(PyObject *self,
                              PyObject *args,
                              PyObject *kwargs)
{
    int x1, y1, x2, y2;
    unsigned long color;

    if (!PyArg_ParseTuple(args, "iiiik:drawRect", &x1, &y1, &x2, &y2, &color))
       return NULL;

    oslDrawRect(x1, y1, x2, y2, color);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_drawFillRect(PyObject *self,
                                  PyObject *args,
                                  PyObject *kwargs)
{
    int x1, y1, x2, y2;
    unsigned long color;

    if (!PyArg_ParseTuple(args, "iiiik:drawFillRect", &x1, &y1, &x2, &y2, &color))
       return NULL;

    oslDrawFillRect(x1, y1, x2, y2, color);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_drawGradientRect(PyObject *self,
                                      PyObject *args,
                                      PyObject *kwargs)
{
    int x1, y1, x2, y2;
    unsigned long c1, c2, c3, c4;

    if (!PyArg_ParseTuple(args, "iiiikkkk:drawGradientRect",
                          &x1, &y1, &x2, &y2, &c1, &c2, &c3, &c4))
       return NULL;

    oslDrawGradientRect(x1, y1, x2, y2, c1, c2, c3, c4);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_clearScreen(PyObject *self,
                                 PyObject *args,
                                 PyObject *kwargs)
{
    int color;

    if (!PyArg_ParseTuple(args, "i:clearScreen", &color))
       return NULL;

    oslClearScreen(color);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_setScreenClipping(PyObject *self,
                                       PyObject *args,
                                       PyObject *kwargs)
{
    int x1, y1, x2, y2;

    if (!PyArg_ParseTuple(args, "iiii:setScreenClipping",
                          &x1, &y1, &x2, &y2))
       return NULL;

    oslSetScreenClipping(x1, y1, x2, y2);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_saveScreenshot(PyObject *self,
                                    PyObject *args,
                                    PyObject *kwargs)
{
    char *fileName;

    if (!PyArg_ParseTuple(args, "s:saveScreenshot",
                          &fileName))
       return NULL;

    oslWriteImageFilePNG(OSL_SECONDARY_BUFFER, fileName, OSL_WRI_ALPHA);


    Py_INCREF(Py_None);
    return Py_None;
}


static PyObject* osl_setAlpha(PyObject *self,
                              PyObject *args,
                              PyObject *kwargs)
{
    int effect, coeff;

    if (!PyArg_ParseTuple(args, "ii:setAlpha", &effect, &coeff))
       return NULL;

    oslSetAlpha(effect, coeff);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_setDithering(PyObject *self,
					              PyObject *args,
						          PyObject *kwargs)
{
	int enabled;
    if (!PyArg_ParseTuple(args, "i:setDithering",
		                  &enabled))
       return NULL;

    oslSetDithering(enabled);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_messageBox(PyObject *self,
                                PyObject *args,
                                PyObject *kwargs)
{
    char *text, *title;
    int ret;

    int b1, a1;
    int b2 = 0, a2 = 0;
    int b3 = 0, a3 = 0;

    if (!PyArg_ParseTuple(args, "ssii|iiii:messageBox", &text, &title,
                          &b1, &a1, &b2, &a2, &b3, &a3))
       return NULL;

    text = strdup(text);
    title = strdup(title);

    Py_BEGIN_ALLOW_THREADS
       ret = oslMessageBox(text, title, oslMake3Buttons(b1, a1, b2, a2, b3, a3));
    Py_END_ALLOW_THREADS

    free(text);
    free(title);

    return Py_BuildValue("i", ret);
}

static PyObject* osl_waitKey(PyObject *self,
                             PyObject *args,
                             PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":waitKey"))
       return NULL;

    return Py_BuildValue("i", oslWaitKey());
}

static PyObject* osl_kbhit(PyObject *self,
                           PyObject *args,
                           PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":kbhit"))
       return NULL;

    return Py_BuildValue("i", oslKbhit());
}

static PyObject* osl_flushKey(PyObject *self,
                              PyObject *args,
                              PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":flushKey"))
       return NULL;

    oslFlushKey();

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* currentDrawBuffer = NULL;

static PyObject* osl_setDrawBuffer(PyObject *self,
                                   PyObject *args,
                                   PyObject *kwargs)
{
    PyObject* buffer;

    if (!PyArg_ParseTuple(args, "O:setDrawBuffer", &buffer))
       return NULL;

    if (PyInt_Check(buffer))
    {
       oslSetDrawBuffer((OSL_IMAGE*)PyInt_AsLong(buffer));
    }
    else if (PyType_IsSubtype(buffer->ob_type, PPyImageType))
    {
       if (currentDrawBuffer)
       {
          Py_DECREF(currentDrawBuffer);
       }

       oslSetDrawBuffer(((PyImage*)buffer)->pImg);

       Py_INCREF(buffer);
       currentDrawBuffer = buffer;
    }
    else
    {
       PyErr_SetString(PyExc_TypeError, "Argument must be an osl.Image or predefined constant");
       return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_initConsole(PyObject *self,
                                 PyObject *args,
                                 PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":initConsole"))
       return NULL;

    oslInitConsole();

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_print(PyObject *self,
                           PyObject *args,
                           PyObject *kwargs)
{
    char *text;

    if (!PyArg_ParseTuple(args, "s:print", &text))
       return NULL;

    oslPrintf("%s", text);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_cls(PyObject *self,
                         PyObject *args,
                         PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":cls"))
       return NULL;

    oslCls();

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_moveTo(PyObject *self,
                            PyObject *args,
                            PyObject *kwargs)
{
    int x, y;

    if (!PyArg_ParseTuple(args, "ii:moveTo", &x, &y))
       return NULL;

    oslMoveTo(x, y);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_printxy(PyObject *self,
                             PyObject *args,
                             PyObject *kwargs)
{
    int x, y;
    char *text;

    if (!PyArg_ParseTuple(args, "iis:printxy", &x, &y, &text))
       return NULL;

    oslPrintf_xy(x, y, "%s", text);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_setTextColor(PyObject *self,
                                  PyObject *args,
                                  PyObject *kwargs)
{
    unsigned long color;

    if (!PyArg_ParseTuple(args, "k:setTextColor", &color))
       return NULL;

    oslSetTextColor(color);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_setBkColor(PyObject *self,
                                PyObject *args,
                                PyObject *kwargs)
{
    unsigned long color;;

    if (!PyArg_ParseTuple(args, "k:setBkColor", &color))
       return NULL;

    oslSetBkColor(color);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_setBilinearFilter(PyObject *self,
                                       PyObject *args,
                                       PyObject *kwargs)
{
    int enable = 1;

    if (!PyArg_ParseTuple(args, "|i:setBilinearFilter", &enable))
       return NULL;

    oslSetBilinearFilter(enable);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_initAudio(PyObject *self,
                               PyObject *args,
                               PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":initAudio"))
       return NULL;

    oslInitAudio();

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_audioVSync(PyObject *self,
                                PyObject *args,
                                PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":audioVSync"))
       return NULL;

    oslAudioVSync();

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_setKeyAutorepeat(PyObject *self,
                                      PyObject *args,
                                      PyObject *kwargs)
{
    int keys, init, interval;

    if (!PyArg_ParseTuple(args, "iii:setKeyAutorepeat",
                          &keys, &init, &interval))
       return NULL;

    oslSetKeyAutorepeat(keys, init, interval);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_setKeyAnalogToDPad(PyObject *self,
                                        PyObject *args,
                                        PyObject *kwargs)
{
    int sensivity;

    if (!PyArg_ParseTuple(args, "i:setKeyAnalogToDPad",
                          &sensivity))
       return NULL;

    oslSetKeyAnalogToDPad(sensivity);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_setKeyAutorepeatMask(PyObject *self,
                                          PyObject *args,
                                          PyObject *kwargs)
{
    int keys;

    if (!PyArg_ParseTuple(args, "i:setKeyAutorepeatMask",
                          &keys))
       return NULL;

    oslSetKeyAutorepeatMask(keys);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_setKeyAutorepeatInit(PyObject *self,
                                          PyObject *args,
                                          PyObject *kwargs)
{
    int init;

    if (!PyArg_ParseTuple(args, "i:setKeyAutorepeatInit",
                          &init))
       return NULL;

    oslSetKeyAutorepeatInit(init);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_setKeyAutorepeatInterval(PyObject *self,
                                              PyObject *args,
                                              PyObject *kwargs)
{
    int interval;

    if (!PyArg_ParseTuple(args, "i:setKeyAutorepeatInterval",
                          &interval))
       return NULL;

    oslSetKeyAutorepeatInterval(interval);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_flushDataCache(PyObject *self,
                                    PyObject *args,
                                    PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":flushDataCache"))
       return NULL;

    oslFlushDataCache();

    Py_INCREF(Py_None);
    return Py_None;
}


static PyObject* osl_getFPS(PyObject *self,
                            PyObject *args,
                            PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":getFPS"))
       return NULL;

    return Py_BuildValue("i", FPS);
}

//==========================================================================
// text.h

static PyObject* osl_drawString(PyObject *self,
                                PyObject *args,
                                PyObject *kwargs)
{
    char *string;
    int x, y;

    if (!PyArg_ParseTuple(args, "iis:drawString", &x, &y, &string))
       return NULL;

    oslDrawString(x, y, string);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_drawTextBox(PyObject *self,
                                 PyObject *args,
                                 PyObject *kwargs)
{
    int x0, y0, x1, y1, format;
    char *string;

    if (!PyArg_ParseTuple(args, "iiiisi:drawTextBox",
                          &x0, &y0, &x1, &y1, &string, &format))
       return NULL;

    oslDrawTextBox(x0, y0, x1, y1, string, format);

    Py_INCREF(Py_None);
    return Py_None;
}

#ifdef WITH_OSLIB_MOD
//==========================================================================
//OSLib mod misc functions:
static PyObject* osl_setHoldForAnalog(PyObject *self,
                                      PyObject *args,
                                      PyObject *kwargs)
{
    int hold;

    if (!PyArg_ParseTuple(args, "i:setHoldForAnalog",
                          &hold))
       return NULL;

    oslSetHoldForAnalog(hold);

    Py_INCREF(Py_None);
    return Py_None;
}


//==========================================================================
//OSLib mod intrafont functions:
static PyObject* osl_intraFontInit(PyObject *self,
                                   PyObject *args,
                                   PyObject *kwargs)
{
    unsigned int options;

    if (!PyArg_ParseTuple(args, "I:intraFontInit",
                          &options))
       return NULL;

    oslIntraFontInit(options);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_intraFontShutdown(PyObject *self,
                                       PyObject *args,
                                       PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":intraFontShutdown"))
       return NULL;

    oslIntraFontShutdown();

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_intraFontSetStyle(PyObject *self,
                                       PyObject *args,
                                       PyObject *kwargs)
{
	PyFont *font;
	float size;
    unsigned int color, shadowcolor, options;

    if (!PyArg_ParseTuple(args, "OfIII:intraFontSetStyle",
                          &font, &size, &color, &shadowcolor, &options))
       return NULL;

    if (!PyType_IsSubtype(((PyObject*)font)->ob_type, PPyFontType))
    {
       PyErr_SetString(PyExc_TypeError, "Argument must be an osl.Font");
       return NULL;
    }

    oslIntraFontSetStyle(font->pFont, size, color, shadowcolor, options);

    Py_INCREF(Py_None);
    return Py_None;
}

//==========================================================================
//OSLib mod dialog functions:
static PyObject* osl_initMessageDialog(PyObject *self,
                                       PyObject *args,
                                       PyObject *kwargs)
{
	char *message;
    int enableYesno;

    if (!PyArg_ParseTuple(args, "si:initMessageDialog",
                          &message, &enableYesno))
       return NULL;

    oslInitMessageDialog(message, enableYesno);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_initErrorDialog(PyObject *self,
                                     PyObject *args,
                                     PyObject *kwargs)
{
    int error;

    if (!PyArg_ParseTuple(args, "i:initErrorDialog",
                          &error))
       return NULL;

    oslInitErrorDialog(error);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_drawDialog(PyObject *self,
                                PyObject *args,
                                PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":drawDialog"))
       return NULL;

    oslDrawDialog();

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_getDialogType(PyObject *self,
                                   PyObject *args,
                                   PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":getDialogType"))
       return NULL;

    int type = oslGetDialogType();
    return Py_BuildValue("i", type);
}


static PyObject* osl_getDialogStatus(PyObject *self,
                                   PyObject *args,
                                   PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":getDialogStatus"))
       return NULL;

    int status = oslGetDialogStatus();
    return Py_BuildValue("i", status);
}

static PyObject* osl_getDialogButtonPressed(PyObject *self,
                                            PyObject *args,
                                            PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":getDialogButtonPressed"))
       return NULL;

    int button = oslGetDialogButtonPressed();
    return Py_BuildValue("i", button);
}

static PyObject* osl_initNetDialog(PyObject *self,
                                   PyObject *args,
                                   PyObject *kwargs)
{
   if (!PyArg_ParseTuple(args, ":initNetDialog"))
       return NULL;

    oslInitNetDialog();

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_dialogGetResult(PyObject *self,
                                     PyObject *args,
                                     PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":dialogGetResult"))
       return NULL;

    int result = oslDialogGetResult();
    return Py_BuildValue("i", result);
}


static PyObject* osl_endDialog(PyObject *self,
                               PyObject *args,
                               PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":endDialog"))
       return NULL;

    oslEndDialog();

    Py_INCREF(Py_None);
    return Py_None;
}

//==========================================================================
//OSLib mod save and load functions:
/*static PyObject* osl_initSaveDialog(PyObject *self,
                                    PyObject *args,
                                    PyObject *kwargs)
{
	struct oslSaveLoad *saveLoad;

    if (!PyArg_ParseTuple(args, "O:initSaveDialog",
                          &saveLoad))
       return NULL;

    if (!PyType_IsSubtype(((PyObject*)saveLoad)->ob_type, PPySaveLoadType ))
    {
       PyErr_SetString(PyExc_TypeError, "Argument must be an osl.SaveLoad");
       return NULL;
    }

    oslInitSaveDialog(saveLoad);

    Py_INCREF(Py_None);
    return Py_None;
}


static PyObject* osl_initLoadDialog(PyObject *self,
                                    PyObject *args,
                                    PyObject *kwargs)
{
	struct oslSaveLoad *saveLoad;

    if (!PyArg_ParseTuple(args, "O:initLoadDialog",
                          &saveLoad))
       return NULL;

    if (!PyType_IsSubtype(((PyObject*)saveLoad)->ob_type, PPySaveLoadType ))
    {
       PyErr_SetString(PyExc_TypeError, "Argument must be an osl.SaveLoad");
       return NULL;
    }

    oslInitLoadDialog(saveLoad);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_drawSaveLoad(PyObject *self,
                                  PyObject *args,
                                  PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":drawSaveLoad"))
       return NULL;

    oslDrawSaveLoad();

    Py_INCREF(Py_None);
    return Py_None;
}


static PyObject* osl_getSaveLoadStatus(PyObject *self,
                                       PyObject *args,
                                       PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":getSaveLoadStatus"))
       return NULL;

    int status = oslGetLoadSaveStatus();
    return Py_BuildValue("i", status);
}

static PyObject* osl_getSaveLoadType(PyObject *self,
                                     PyObject *args,
                                     PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":getSaveLoadType"))
       return NULL;

    int status = oslGetSaveLoadType();
    return Py_BuildValue("i", status);
}

static PyObject* osl_saveLoadGetResult(PyObject *self,
                                       PyObject *args,
                                       PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":saveLoadGetResult"))
       return NULL;

    int status = oslSaveLoadGetResult();
    return Py_BuildValue("i", status);
}

static PyObject* osl_endSaveLoad(PyObject *self,
                                 PyObject *args,
                                 PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":endSaveLoad"))
       return NULL;

    oslEndSaveLoadDialog();

    Py_INCREF(Py_None);
    return Py_None;
}*/

//==========================================================================
//OSLib mod OSK functions:
static PyObject* osl_initOsk(PyObject *self,
                             PyObject *args,
                             PyObject *kwargs)
{
	char *descStr;
	char *initialStr;
	int textLimit, linesNumber;
	int language = -1;

    if (!PyArg_ParseTuple(args, "ssii|i:initOsk",
                          &descStr, &initialStr, &textLimit, &linesNumber, &language))
       return NULL;

    oslInitOsk(descStr, initialStr, textLimit, linesNumber, language);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_drawOsk(PyObject *self,
                             PyObject *args,
                             PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":drawOsk"))
       return NULL;

    oslDrawOsk();

	Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_oskIsActive(PyObject *self,
                                 PyObject *args,
                                 PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":oskIsActive"))
       return NULL;

    int status = oslOskIsActive();
    return Py_BuildValue("i", status);
}

static PyObject* osl_getOskStatus(PyObject *self,
                                  PyObject *args,
                                  PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":getOskStatus"))
       return NULL;

    int status = oslGetOskStatus();
    return Py_BuildValue("i", status);
}

static PyObject* osl_oskGetResult(PyObject *self,
                                  PyObject *args,
                                  PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":oskGetResult"))
       return NULL;

    int status = oslOskGetResult();
    return Py_BuildValue("i", status);
}


static PyObject* osl_oskGetText(PyObject *self,
                                PyObject *args,
                                PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":oskGetText"))
       return NULL;

	char result[256] = "";
    oslOskGetText(result);
    return Py_BuildValue("s", result);
}

static PyObject* osl_endOsk(PyObject *self,
                            PyObject *args,
                            PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":endOsk"))
       return NULL;

    oslEndOsk();

    Py_INCREF(Py_None);
    return Py_None;
}

//==========================================================================
//OSLib mod network functions:
static PyObject* osl_netInit(PyObject *self,
                             PyObject *args,
                             PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":netInit"))
       return NULL;

    int result = oslNetInit();;

    return Py_BuildValue("i", result);
}

static PyObject* osl_netTerm(PyObject *self,
                             PyObject *args,
                             PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":netTerm"))
       return NULL;

    int result = oslNetTerm();;

    return Py_BuildValue("i", result);
}

//==========================================================================
//OSLib mod browser functions:
/*static PyObject* osl_browserInit(PyObject *self,
                                 PyObject *args,
                                 PyObject *kwargs)
{
	char *url;
	char *downloadDir;
	int browserMemory;
	unsigned int displaymode = PSP_UTILITY_HTMLVIEWER_DISPLAYMODE_SMART_FIT;
	unsigned int options = PSP_UTILITY_HTMLVIEWER_DISABLE_STARTUP_LIMITS | PSP_UTILITY_HTMLVIEWER_ENABLE_FLASH;
	unsigned int interfacemode = PSP_UTILITY_HTMLVIEWER_INTERFACEMODE_FULL;
	unsigned int connectmode = PSP_UTILITY_HTMLVIEWER_CONNECTMODE_MANUAL_ALL;

    if (!PyArg_ParseTuple(args, "ssi|IIII:browserInit", &url, &downloadDir, &browserMemory,
		                                                &displaymode, &options, &interfacemode,
		                                                &connectmode))
       return NULL;

    if (sceKernelTotalFreeMemSize() < browserMemory + 256)
    {
        setHeapSize( -(browserMemory + 256) );
    }
    int result = oslBrowserInit(url, downloadDir, browserMemory, displaymode, options, interfacemode, connectmode);

    return Py_BuildValue("i", result);
}


static PyObject* osl_drawBrowser(PyObject *self,
                                 PyObject *args,
                                 PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":drawBrowser"))
       return NULL;

    oslDrawBrowser();

	Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* osl_browserIsActive(PyObject *self,
                                     PyObject *args,
                                     PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":browserIsActive"))
       return NULL;

    int status = oslBrowserIsActive();
    return Py_BuildValue("i", status);
}

static PyObject* osl_getBrowserStatus(PyObject *self,
                                      PyObject *args,
                                      PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":getBrowserStatus"))
       return NULL;

    int status = oslGetBrowserStatus();
    return Py_BuildValue("i", status);
}


static PyObject* osl_endBrowser(PyObject *self,
                                PyObject *args,
                                PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, ":endBrowser"))
       return NULL;

    oslEndBrowser();

    Py_INCREF(Py_None);
    return Py_None;
}*/
#endif

//==========================================================================
// Macros

static PyObject* osl_RGB(PyObject *self,
                         PyObject *args,
                         PyObject *kwargs)
{
    int r, g, b;

    if (!PyArg_ParseTuple(args, "iii:RGB", &r, &g, &b))
       return NULL;

    return Py_BuildValue("i", RGB(r, g, b));
}

static PyObject* osl_RGB12(PyObject *self,
                           PyObject *args,
                           PyObject *kwargs)
{
    int r, g, b;

    if (!PyArg_ParseTuple(args, "iii:RGB12", &r, &g, &b))
       return NULL;

    return Py_BuildValue("i", RGB12(r, g, b));
}

static PyObject* osl_RGB15(PyObject *self,
                           PyObject *args,
                           PyObject *kwargs)
{
    int r, g, b;

    if (!PyArg_ParseTuple(args, "iii:RGB15", &r, &g, &b))
       return NULL;

    return Py_BuildValue("i", RGB15(r, g, b));
}

static PyObject* osl_RGB16(PyObject *self,
                           PyObject *args,
                           PyObject *kwargs)
{
    int r, g, b;

    if (!PyArg_ParseTuple(args, "iii:RGB16", &r, &g, &b))
       return NULL;

    return Py_BuildValue("i", RGB16(r, g, b));
}

static PyObject* osl_RGBA(PyObject *self,
                          PyObject *args,
                          PyObject *kwargs)
{
    int r, g, b, a;

    if (!PyArg_ParseTuple(args, "iiii:RGBA", &r, &g, &b, &a))
       return NULL;

    return Py_BuildValue("i", RGBA(r, g, b, a));
}

static PyObject* osl_RGBA12(PyObject *self,
                            PyObject *args,
                            PyObject *kwargs)
{
    int r, g, b, a;

    if (!PyArg_ParseTuple(args, "iiii:RGBA12", &r, &g, &b, &a))
       return NULL;

    return Py_BuildValue("i", RGBA12(r, g, b, a));
}

static PyObject* osl_RGBA15(PyObject *self,
                            PyObject *args,
                            PyObject *kwargs)
{
    int r, g, b, a;

    if (!PyArg_ParseTuple(args, "iiii:RGBA15", &r, &g, &b, &a))
       return NULL;

    return Py_BuildValue("i", RGBA15(r, g, b, a));
}

//==========================================================================
// Module

#define DECLFUNC(name, doc) { #name, (PyCFunction)osl_##name, METH_VARARGS, doc }

static PyMethodDef functions[] = {
   DECLFUNC(mustQuit, ""),
   DECLFUNC(safeQuit, ""),
   DECLFUNC(doQuit, ""),

   DECLFUNC(initGfx, ""),
   DECLFUNC(startDrawing, ""),
   DECLFUNC(syncDrawing, ""),
   DECLFUNC(endDrawing, ""),
   DECLFUNC(endFrame, ""),
   DECLFUNC(waitVSync, ""),
   DECLFUNC(swapBuffers, ""),
   DECLFUNC(endGfx, ""),
   DECLFUNC(getFPS, ""),

   DECLFUNC(syncFrame, ""),
   DECLFUNC(syncFrameEx, ""),
   DECLFUNC(setTransparentColor, ""),
   DECLFUNC(disableTransparentColor, ""),

   DECLFUNC(setQuitOnLoadFailure, ""),

   DECLFUNC(setFrameskip, ""),
   DECLFUNC(setMaxFrameskip, ""),

   DECLFUNC(drawLine, ""),
   DECLFUNC(drawRect, ""),
   DECLFUNC(drawFillRect, ""),
   DECLFUNC(drawGradientRect, ""),

   DECLFUNC(saveScreenshot, ""),
   DECLFUNC(clearScreen, ""),
   DECLFUNC(setScreenClipping, ""),

   DECLFUNC(setDithering, ""),
   DECLFUNC(setAlpha, ""),

   DECLFUNC(messageBox, ""),

   DECLFUNC(waitKey, ""),
   DECLFUNC(kbhit, ""),
   DECLFUNC(flushKey, ""),

   DECLFUNC(initConsole, ""),
   DECLFUNC(print, ""),
   DECLFUNC(cls, ""),
   DECLFUNC(moveTo, ""),
   DECLFUNC(printxy, ""),
   DECLFUNC(setTextColor, ""),
   DECLFUNC(setBkColor, ""),

   DECLFUNC(setDrawBuffer, ""),
   DECLFUNC(setBilinearFilter, ""),

   DECLFUNC(initAudio, ""),
   DECLFUNC(audioVSync, ""),

   DECLFUNC(setKeyAutorepeat, ""),
   DECLFUNC(setKeyAutorepeatMask, ""),
   DECLFUNC(setKeyAutorepeatInit, ""),
   DECLFUNC(setKeyAutorepeatInterval, ""),
   DECLFUNC(setKeyAnalogToDPad, ""),

   DECLFUNC(flushDataCache, ""),

   DECLFUNC(drawString, ""),
   DECLFUNC(drawTextBox, ""),

   DECLFUNC(RGB, ""),
   DECLFUNC(RGB12, ""),
   DECLFUNC(RGB15, ""),
   DECLFUNC(RGB16, ""),
   DECLFUNC(RGBA, ""),
   DECLFUNC(RGBA12, ""),
   DECLFUNC(RGBA15, ""),

#ifdef WITH_OSLIB_MOD
   DECLFUNC(setHoldForAnalog, ""),

   DECLFUNC(intraFontInit, ""),
   DECLFUNC(intraFontShutdown, ""),
   DECLFUNC(intraFontSetStyle, ""),

   DECLFUNC(initMessageDialog, ""),
   DECLFUNC(initErrorDialog, ""),
   DECLFUNC(drawDialog, ""),
   DECLFUNC(getDialogType, ""),
   DECLFUNC(getDialogStatus, ""),
   DECLFUNC(getDialogButtonPressed, ""),
   DECLFUNC(initNetDialog, ""),
   DECLFUNC(dialogGetResult, ""),
   DECLFUNC(endDialog, ""),

   DECLFUNC(initOsk, ""),
   DECLFUNC(drawOsk, ""),
   DECLFUNC(oskIsActive, ""),
   DECLFUNC(getOskStatus, ""),
   DECLFUNC(oskGetResult, ""),
   DECLFUNC(oskGetText, ""),
   DECLFUNC(endOsk, ""),

   DECLFUNC(netInit, ""),
   DECLFUNC(netTerm, ""),

   /*DECLFUNC(initSaveDialog, ""),
   DECLFUNC(initLoadDialog, ""),
   DECLFUNC(drawSaveLoad, ""),
   DECLFUNC(getSaveLoadStatus, ""),
   DECLFUNC(getSaveLoadType, ""),
   DECLFUNC(saveLoadGetResult, ""),
   DECLFUNC(endSaveLoad, ""),*/

   /*DECLFUNC(browserInit, ""),
   DECLFUNC(drawBrowser, ""),
   DECLFUNC(browserIsActive, ""),
   DECLFUNC(getBrowserStatus, ""),
   DECLFUNC(endBrowser, ""),*/


#endif

   { NULL }
};

#define ADDINT(name) PyModule_AddIntConstant(mdl, #name, (int)OSL_##name)
#define ADDINTWITHNAME(oslname, name) PyModule_AddIntConstant(mdl, #name, (int)#oslname)

#ifdef WITH_OSLIB_MOD
#define ADDINTRAFONTINT(name) PyModule_AddIntConstant(mdl, #name, (int)#name)
#endif

void initosl(void)
{
    PyObject *mdl;

    if (PyType_Ready(PPyImageType) < 0)
       return;

    if (PyType_Ready(PPyControllerType) < 0)
       return;

    if (PyType_Ready(PPySoundType) < 0)
       return;

    if (PyType_Ready(PPyMapType) < 0)
       return;

    if (PyType_Ready(PPyFontType) < 0)
       return;

#ifdef WITH_OSLIB_MOD
    if (PyType_Ready(PPySFontType) < 0)
       return;

    /*if (PyType_Ready(PPySaveLoadType) < 0)
       return;*/
#endif

    mdl = Py_InitModule3("osl", functions, "");
    if (!mdl)
       return;

    ADDINT(PF_8888);
    ADDINT(PF_5650);
    ADDINT(PF_5551);
    ADDINT(PF_4444);
    ADDINT(PF_8BIT);
    ADDINT(PF_4BIT);

    ADDINT(FX_NONE);
    ADDINT(FX_FLAT);
    ADDINT(FX_RGBA);
    ADDINT(FX_ALPHA);
    ADDINT(FX_ADD);
    ADDINT(FX_SUB);
    ADDINT(FX_COLOR);

    ADDINT(IN_VRAM);
    ADDINT(IN_RAM);
    ADDINT(SWIZZLED);

    ADDINT(DEFAULT_BUFFER);
    ADDINT(SECONDARY_BUFFER);

    ADDINT(MB_OK);
    ADDINT(MB_CANCEL);
    ADDINT(MB_YES);
    ADDINT(MB_NO);
    ADDINT(MB_QUIT);

    ADDINT(KEY_SELECT);
    ADDINT(KEY_START);
    ADDINT(KEY_UP);
    ADDINT(KEY_RIGHT);
    ADDINT(KEY_DOWN);
    ADDINT(KEY_LEFT);
    ADDINT(KEY_L);
    ADDINT(KEY_R);
    ADDINT(KEY_TRIANGLE);
    ADDINT(KEY_CIRCLE);
    ADDINT(KEY_CROSS);
    ADDINT(KEY_SQUARE);
    ADDINT(KEY_HOLD);
    ADDINT(KEY_HOME);
    ADDINT(KEY_NOTE);

    ADDINT(FMT_STREAM);
    ADDINT(FMT_NONE);

#ifdef WITH_OSLIB_MOD
    ADDINT(DIALOG_CANCEL);
    ADDINT(DIALOG_OK);
    ADDINT(DIALOG_NONE);
    ADDINT(DIALOG_MESSAGE);
    ADDINT(DIALOG_ERROR);
    ADDINT(DIALOG_NETCONF);

    /*ADDINT(SAVELOAD_CANCEL);
    ADDINT(SAVELOAD_OK);
    ADDINT(DIALOG_SAVE);
    ADDINT(DIALOG_LOAD);*/

    ADDINT(OSK_CANCEL);
    ADDINT(OSK_OK);

    ADDINT(NET_ERROR_NET);
    ADDINT(NET_ERROR_INET);
    ADDINT(NET_ERROR_RESOLVER);
    ADDINT(NET_ERROR_APCTL);
    ADDINT(NET_ERROR_APCTL);
    ADDINT(NET_ERROR_SSL);
    ADDINT(NET_ERROR_HTTP);
    ADDINT(NET_ERROR_HTTPS);
    ADDINT(NET_ERROR_CERT);
    ADDINT(NET_ERROR_COOKIE);
    ADDINT(ERR_APCTL_GETINFO);
    ADDINT(ERR_APCTL_CONNECT);
    ADDINT(ERR_APCTL_TIMEOUT);
    ADDINT(ERR_APCTL_GETSTATE);
    ADDINT(ERR_RESOLVER_CREATE);
    ADDINT(ERR_RESOLVER_RESOLVING);
    ADDINT(ERR_WLAN_OFF);
    ADDINT(USER_ABORTED);

	ADDINTWITHNAME(JAPANESE, OSK_JAPANESE);
	ADDINTWITHNAME(ENGLISH, OSK_ENGLISH);
	ADDINTWITHNAME(FRENCH, OSK_FRENCH);
	ADDINTWITHNAME(SPANISH, OSK_SPANISH);
	ADDINTWITHNAME(GERMAN, OSK_GERMAN);
	ADDINTWITHNAME(ITALIAN, OSK_ITALIAN);
	ADDINTWITHNAME(DUTCH, OSK_DUTCH);
	ADDINTWITHNAME(PORTUGUESE, OSK_PORTUGUESE);
	ADDINTWITHNAME(RUSSIAN, OSK_RUSSIAN);
	ADDINTWITHNAME(KOREAN, OSK_KOREAN);
	ADDINTWITHNAME(CHINESE_TRADITIONAL, OSK_CHINESE_TRADITIONAL);
	ADDINTWITHNAME(CHINESE_SIMPLIFIED, OSK_CHINESE_SIMPLIFIED);

	ADDINTRAFONTINT(INTRAFONT_ADVANCE_H);
	ADDINTRAFONTINT(INTRAFONT_ADVANCE_V);
	ADDINTRAFONTINT(INTRAFONT_ALIGN_LEFT);
	ADDINTRAFONTINT(INTRAFONT_ALIGN_CENTER);
	ADDINTRAFONTINT(INTRAFONT_ALIGN_RIGHT);
	ADDINTRAFONTINT(INTRAFONT_ALIGN_FULL);
	ADDINTRAFONTINT(INTRAFONT_SCROLL_LEFT);

	ADDINTRAFONTINT(INTRAFONT_SCROLL_SEESAW);
	ADDINTRAFONTINT(INTRAFONT_SCROLL_RIGHT);
	ADDINTRAFONTINT(INTRAFONT_SCROLL_THROUGH);
	ADDINTRAFONTINT(INTRAFONT_WIDTH_VAR);
	ADDINTRAFONTINT(INTRAFONT_WIDTH_FIX);
	ADDINTRAFONTINT(INTRAFONT_ACTIVE);
	ADDINTRAFONTINT(INTRAFONT_CACHE_MED);
	ADDINTRAFONTINT(INTRAFONT_CACHE_LARGE);
	ADDINTRAFONTINT(INTRAFONT_CACHE_ASCII);
	ADDINTRAFONTINT(INTRAFONT_CACHE_ALL);

	ADDINTRAFONTINT(INTRAFONT_STRING_ASCII);
	ADDINTRAFONTINT(INTRAFONT_STRING_CP437);
	ADDINTRAFONTINT(INTRAFONT_STRING_CP850);
	ADDINTRAFONTINT(INTRAFONT_STRING_CP866);
	ADDINTRAFONTINT(INTRAFONT_STRING_SJIS);
	ADDINTRAFONTINT(INTRAFONT_STRING_GBK);
	ADDINTRAFONTINT(INTRAFONT_STRING_KOR);
	ADDINTRAFONTINT(INTRAFONT_STRING_BIG5);
	ADDINTRAFONTINT(INTRAFONT_STRING_CP1251);
	ADDINTRAFONTINT(INTRAFONT_STRING_CP1252);
	ADDINTRAFONTINT(INTRAFONT_STRING_UTF8);

    /*ADDINT(BROWSER_ERROR_MEMORY);
    ADDINT(BROWSER_ERROR_INIT);

	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_DISCONNECTMODE_ENABLE, HTMLVIEWER_DISCONNECTMODE_ENABLE);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_DISCONNECTMODE_DISABLE, HTMLVIEWER_DISCONNECTMODE_DISABLE);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_DISCONNECTMODE_CONFIRM, HTMLVIEWER_DISCONNECTMODE_CONFIRM);

	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_INTERFACEMODE_FULL, HTMLVIEWER_INTERFACEMODE_FULL);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_INTERFACEMODE_LIMITED, HTMLVIEWER_INTERFACEMODE_LIMITED);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_INTERFACEMODE_NONE, HTMLVIEWER_INTERFACEMODE_NONE);

	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_COOKIEMODE_DISABLED, HTMLVIEWER_COOKIEMODE_DISABLED);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_COOKIEMODE_ENABLED, HTMLVIEWER_COOKIEMODE_ENABLED);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_COOKIEMODE_CONFIRM, HTMLVIEWER_COOKIEMODE_CONFIRM);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_COOKIEMODE_DEFAULT, HTMLVIEWER_COOKIEMODE_DEFAULT);

	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_TEXTSIZE_LARGE, HTMLVIEWER_TEXTSIZE_LARGE);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_TEXTSIZE_NORMAL, HTMLVIEWER_TEXTSIZE_NORMAL);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_TEXTSIZE_SMALL, HTMLVIEWER_TEXTSIZE_SMALL);

	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_DISPLAYMODE_NORMAL, HTMLVIEWER_DISPLAYMODE_NORMAL);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_DISPLAYMODE_FIT, HTMLVIEWER_DISPLAYMODE_FIT);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_DISPLAYMODE_SMART_FIT, HTMLVIEWER_DISPLAYMODE_SMART_FIT);

	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_CONNECTMODE_LAST, HTMLVIEWER_CONNECTMODE_LAST);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_CONNECTMODE_MANUAL_ONCE, HTMLVIEWER_CONNECTMODE_MANUAL_ONCE);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_CONNECTMODE_MANUAL_ALL, HTMLVIEWER_CONNECTMODE_MANUAL_ALL);

	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_OPEN_SCE_START_PAGE, HTMLVIEWER_OPEN_SCE_START_PAGE);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_DISABLE_STARTUP_LIMITS, HTMLVIEWER_DISABLE_STARTUP_LIMITS);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_DISABLE_EXIT_DIALOG, HTMLVIEWER_DISABLE_EXIT_DIALOG);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_DISABLE_CURSOR, HTMLVIEWER_DISABLE_CURSOR);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_DISABLE_DOWNLOAD_COMPLETE_DIALOG, HTMLVIEWER_DISABLE_DOWNLOAD_COMPLETE_DIALOG);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_DISABLE_DOWNLOAD_START_DIALOG, HTMLVIEWER_DISABLE_DOWNLOAD_START_DIALOG);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_DISABLE_DOWNLOAD_DESTINATION_DIALOG, HTMLVIEWER_DISABLE_DOWNLOAD_DESTINATION_DIALOG);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_LOCK_DOWNLOAD_DESTINATION_DIALOG, HTMLVIEWER_LOCK_DOWNLOAD_DESTINATION_DIALOG);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_DISABLE_TAB_DISPLAY, HTMLVIEWER_DISABLE_TAB_DISPLAY);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_ENABLE_ANALOG_HOLD, HTMLVIEWER_ENABLE_ANALOG_HOLD);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_ENABLE_FLASH, HTMLVIEWER_ENABLE_FLASH);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_DISABLE_LRTRIGGER, HTMLVIEWER_DISABLE_LRTRIGGER);
	ADDINTWITHNAME(PSP_UTILITY_HTMLVIEWER_CONNECTMODE_MANUAL_ALL, HTMLVIEWER_CONNECTMODE_MANUAL_ALL);*/

#endif

    osl_Error = PyErr_NewException("osl.Error", NULL, NULL);
    PyModule_AddObject(mdl, "Error", osl_Error);

    Py_INCREF(PPyImageType);
    PyModule_AddObject(mdl, "Image", (PyObject*)PPyImageType);

    Py_INCREF(PPyControllerType);
    PyModule_AddObject(mdl, "Controller", (PyObject*)PPyControllerType);

    Py_INCREF(PPySoundType);
    PyModule_AddObject(mdl, "Sound", (PyObject*)PPySoundType);

    Py_INCREF(PPyMapType);
    PyModule_AddObject(mdl, "Map", (PyObject*)PPyMapType);

    Py_INCREF(PPyFontType);
    PyModule_AddObject(mdl, "Font", (PyObject*)PPyFontType);

#ifdef WITH_OSLIB_MOD
    Py_INCREF(PPySFontType);
    PyModule_AddObject(mdl, "SFont", (PyObject*)PPySFontType);

    /*Py_INCREF(PPySaveLoadType);
    PyModule_AddObject(mdl, "SaveLoad", (PyObject*)PPySaveLoadType);*/
#endif
}

#ifdef _GNUC
static const char* _rcsid_osl __attribute__((unused)) = "$Id$";
#endif
