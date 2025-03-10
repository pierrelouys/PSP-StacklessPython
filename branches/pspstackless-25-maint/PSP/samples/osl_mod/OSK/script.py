# -*- coding: ISO-8859-1 -*-

import osl

osl.initGfx(osl.PF_8888, True)
osl.intraFontInit(osl.INTRAFONT_CACHE_MED)
osl.setQuitOnLoadFailure(True)

bkg = osl.Image('bkg.png', osl.IN_RAM | osl.SWIZZLED, osl.PF_8888)

ifont = osl.Font("flash0:/font/ltn0.pgf")
osl.intraFontSetStyle(ifont, 1.0, osl.RGBA(255,255,255,255), osl.RGBA(0,0,0,0), osl.INTRAFONT_ALIGN_LEFT)
ifont.set()

skip = False
message = ""

while not osl.mustQuit():
    if not skip:
        osl.startDrawing()

        bkg.draw()

        osl.drawString(5, 5, "FPS: %i" % osl.getFPS())

        osl.drawString(180, 130, "Sony OSK test program")
        osl.drawString(180, 160, "Press start to show the OSK")
        osl.drawString(180, 190, message)

        osl.drawString(150, 250, "Press X to quit")

        if osl.oskIsActive():
            osl.drawOsk()
            if osl.getOskStatus() == osl.DIALOG_NONE:
                if osl.oskGetResult() == osl.OSK_CANCEL:
                    message = "Cancel"
                else:
                    userText = osl.oskGetText()
                    message = "You entered: %s" % userText
                osl.endOsk()

        osl.endDrawing()
    osl.endFrame()
    skip = osl.syncFrame()

    if not osl.oskIsActive():
        ctrl = osl.Controller()
        if ctrl.held_cross:
            osl.safeQuit()
        elif ctrl.pressed_start:
            osl.initOsk("Please insert some text", "Initial text", 128, 1, osl.OSK_ENGLISH)

osl.endGfx()
