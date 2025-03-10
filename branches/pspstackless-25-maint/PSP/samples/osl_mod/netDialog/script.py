# -*- coding: ISO-8859-1 -*-

import osl

osl.initGfx(osl.PF_8888, True)
osl.intraFontInit(osl.INTRAFONT_CACHE_MED)
osl.setQuitOnLoadFailure(True)
#osl.netInit()

bkg = osl.Image('bkg.png', osl.IN_RAM | osl.SWIZZLED, osl.PF_8888)

ifont = osl.Font("flash0:/font/ltn0.pgf")
osl.intraFontSetStyle(ifont, 1.0, osl.RGBA(255,255,255,255), osl.RGBA(0,0,0,0), osl.INTRAFONT_ALIGN_LEFT)
ifont.set()

skip = False
dialog = osl.DIALOG_NONE

while not osl.mustQuit():
    if not skip:
        osl.startDrawing()

        bkg.draw()

        osl.drawString(5, 5, "FPS: %i" % osl.getFPS())

        osl.drawString(180, 130, "Network dialog test program")
        osl.drawString(180, 160, "Press start to show the network dialog")

        osl.drawString(150, 250, "Press X to quit")

        dialog = osl.getDialogType()
        if dialog != osl.DIALOG_NONE:
            osl.drawDialog()
            if osl.getDialogStatus() == osl.DIALOG_NONE:
                osl.endDialog()

        osl.endDrawing()
    osl.endFrame()
    skip = osl.syncFrame()

    if dialog == osl.DIALOG_NONE:
        ctrl = osl.Controller()
        if ctrl.pressed_cross:
            osl.safeQuit()
        elif ctrl.pressed_start:
            osl.initNetDialog()

#osl.netTerm()
osl.endGfx()