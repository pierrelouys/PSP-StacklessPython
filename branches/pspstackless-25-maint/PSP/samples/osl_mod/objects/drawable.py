import osl

################################################################################
# Drawable
################################################################################
class drawable(object):
    """Class Drawable"""
    #Constructor
    def __init__(self, name=None):
        self.name = name
        self.visible = True
        self.x = 0
        self.y = 0
        self.animations = []
        self.data = {}


    #Methods:
    def draw(self):
        for animation in self.animations:
            animation(self)


################################################################################
# Image Object
################################################################################
class imageObj(drawable):
    """Class imageObj"""
    #Constructor
    def __init__(self, name=None):
        self.image = None
        drawable.__init__(self, name)

    #Methods:
    def draw(self):
        drawable.draw(self)
        if self.visible and self.image:
            self.image.draw(self.x, self.y)


################################################################################
# String Object
################################################################################
class stringObj(drawable):
    """Class stringObj"""
    #Constructor
    def __init__(self, name=None):
        self.font = None
        self.string = None
        self.textColor = osl.RGBA(255,255,255,255)
        self.backColor = osl.RGBA(255,255,255,0)
        drawable.__init__(self, name)

    #Methods:
    def draw(self):
        drawable.draw(self)
        if self.visible and self.font:
            self.font.set()
            osl.setTextColor(self.textColor)
            osl.setBkColor(self.backColor)
            osl.drawString(self.x, self.y,  self.string)


################################################################################
# Layer
################################################################################
class layer(object):
    """Class Layer"""
    #Constructor
    def __init__(self):
        self.objects = {}

    #Methods:
    def addObject(self, obj):
        if obj.name == None:
            raise ValueError, 'name'
        self.objects[obj.name] = obj
        return True

    def removeObject(self, obj):
        if obj.name == None:
            raise ValueError, 'name'
        if obj.name in self.objects:
            del self.objects[obj.name]
            return True
        return False

    def removeObjectByName(self, objName):
        obj = drawable(objName)
        return self.removeObject(obj)

    def draw(self):
        for key, obj in self.objects.iteritems():
            obj.draw()
