# -*- coding: ISO-8859-1 -*-
import osl, pspos, drawable, random

osl.initGfx(osl.PF_5551, True)
osl.setDithering(True)
osl.setQuitOnLoadFailure(True)
osl.setFrameskip(1)
osl.setKeyAnalogToDPad(80)

bkg = osl.Image('bkg.png', osl.IN_RAM | osl.SWIZZLED, osl.PF_5551)
sprite = osl.Image('sprite.png', osl.IN_RAM | osl.SWIZZLED, osl.PF_5551)

ofont = osl.Font("font.oft")
osl.setTextColor(osl.RGBA(255,255,255,255))
osl.setBkColor(osl.RGBA(255,255,255,0))
ofont.set()

#Sprite animations (move and rotate)
def spriteAnim(obj):
    obj.image.rotate(obj.data["rotate"])
    newX = obj.x + obj.data["moveX"]
    newY = obj.y + obj.data["moveY"]
    sizeX = obj.image.centerX
    sizeY = obj.image.centerY
    if newX + sizeX > 480 or newX - sizeX < 0:
        obj.data["moveX"] = -obj.data["moveX"]
    elif newY + sizeY > 272 or newY - sizeY < 0:
        obj.data["moveY"] = -obj.data["moveY"]
    obj.x += obj.data["moveX"]
    obj.y += obj.data["moveY"]

def addSprite(layer, objName=None):
    if not objName:
        objName = "obj_%2.2i" % len(layer.objects)
    obj = drawable.imageObj(objName)
    obj.image = osl.Image((sprite.sizeX, sprite.sizeY), osl.IN_RAM, osl.PF_5551)
    sprite.copy(obj.image)
    obj.image.swizzle()
    obj.image.setRotationCenter()
    obj.x = random.randint(obj.image.centerX, 480 - obj.image.centerX)
    obj.y = random.randint(obj.image.centerY, 272 - obj.image.centerY)
    obj.data["rotate"] = random.randint(1, 5)
    obj.data["moveX"] = random.randint(1, 5)
    obj.data["moveY"] = random.randint(1, 5)
    obj.animations.append(spriteAnim)
    layer.addObject(obj)


def removeSprite(layer):
    names = layer.objects.keys()
    names.sort()
    if len(names):
        nameObj = names.pop()
        layer.removeObjectByName(nameObj)

layers = []
#Initial sprite objects
sprites = drawable.layer()
for i in xrange(0, 20, 1):
    addSprite(sprites)

layers.append(sprites)

#Initial generic objects
objects = drawable.layer()

#FPS
FPSobj = drawable.stringObj("FPS")
FPSobj.font = ofont
FPSobj.x = 5
FPSobj.y = 5
def FPSAnim(obj):
    obj.string = "FPS: %i" % osl.getFPS()
FPSobj.animations.append(FPSAnim)
objects.addObject(FPSobj)

#CPU freq
CPUobj = drawable.stringObj("CPU")
CPUobj.font = ofont
CPUobj.x = 5
CPUobj.y = 20
def CPUAnim(obj):
    obj.string = "CPU: %i" % pspos.getclock()
CPUobj.animations.append(CPUAnim)
objects.addObject(CPUobj)

#BUS freq
BUSobj = drawable.stringObj("BUS")
BUSobj.font = ofont
BUSobj.x = 5
BUSobj.y = 35
def BUSAnim(obj):
    obj.string = "BUS: %i" % pspos.getbus()
BUSobj.animations.append(BUSAnim)
objects.addObject(BUSobj)

#Objects count
objCount = drawable.stringObj("OBJ")
objCount.font = ofont
objCount.x = 5
objCount.y = 50
objCount.data["layer"] = sprites
objCount.data["objects"] = objects
def objCountAnim(obj):
    obj.string = "Objects: %i" % (len(obj.data["layer"].objects) + len(obj.data["objects"].objects), )
objCount.animations.append(objCountAnim)
objects.addObject(objCount)

#Messages
MSG1obj = drawable.stringObj("MSG1obj")
MSG1obj.font = ofont
MSG1obj.x = 180
MSG1obj.y = 150
MSG1obj.string = "Press up to add an object"
objects.addObject(MSG1obj)

MSG2obj = drawable.stringObj("MSG2obj")
MSG2obj.font = ofont
MSG2obj.x = 180
MSG2obj.y = 165
MSG2obj.string = "Press down to remove an object"
objects.addObject(MSG2obj)

MSG3obj = drawable.stringObj("MSG3obj")
MSG3obj.font = ofont
MSG3obj.x = 150
MSG3obj.y = 250
MSG3obj.string = "Press X to quit"
objects.addObject(MSG3obj)

layers.append(objects)

################################################################################
# Main program
################################################################################
skip = False
while not osl.mustQuit():
    if not skip:
        osl.startDrawing()

        bkg.draw()

        for layer in layers:
            layer.draw()

        osl.endDrawing()
    osl.endFrame()
    skip = osl.syncFrame()

    ctrl = osl.Controller()
    if ctrl.pressed_up:
        addSprite(sprites)
    elif ctrl.pressed_down:
        removeSprite(sprites)
    elif ctrl.pressed_cross:
        osl.safeQuit()

osl.endGfx()
