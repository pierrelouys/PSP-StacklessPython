# -*- coding: ISO-8859-1 -*-

import osl, pspos

osl.initGfx(osl.PF_8888, True)
osl.intraFontInit(osl.INTRAFONT_CACHE_ALL | osl.INTRAFONT_STRING_UTF8)
osl.setQuitOnLoadFailure(True)

bkg = osl.Image('bkg.png', osl.IN_RAM | osl.SWIZZLED, osl.PF_8888)

ofont = osl.Font("font.oft")
ifont = osl.Font("flash0:/font/ltn0.pgf")
osl.setTextColor(osl.RGBA(255,255,255,255))
osl.setBkColor(osl.RGBA(255,255,255,0))

sfont = osl.SFont("sfont.png")

osl.intraFontSetStyle(ifont, 1.0, osl.RGBA(255,255,255,255), osl.RGBA(0,0,0,0), osl.INTRAFONT_ALIGN_LEFT)

skip = False

while not osl.mustQuit():
    if not skip:
        osl.startDrawing()

        bkg.draw()

        FPS = osl.getFPS()
        ofont.set()
        osl.drawString(5, 5,  "FPS: %i" % FPS)
        osl.drawString(5, 20, "CPU: %i" % pspos.getclock())
        osl.drawString(5, 35, "BUS: %i" % pspos.getbus())

        ifont.set()
        osl.drawString(180, 120, "This is IntraFont")
        ofont.set()
        osl.drawString(180, 140, "This is an OFT")

        sfont.drawString(180, 155, "This is a SFont");

        ifont.set()
        osl.drawString(150, 250, "Press X to quit")

        osl.endDrawing()
    osl.endFrame()
    skip = osl.syncFrame()

    ctrl = osl.Controller()
    if ctrl.held_cross:
        osl.safeQuit()

osl.endGfx()
