"""-------------------------------------------------
PorkScript Game Engine (C) 2012 Andy J. Brennan
All Rights Reserved.

 PPorkScript.py --
   Contains all the basic classes that
   make up the scripting portion of the engine.
-------------------------------------------------"""

import PorkPy

class Command():
    """Base Command Class -- All commands derive from here."""
    def __init__(self, args):
        lArgs = []
        posf = 0
        posb = 0
        for x in args:
            posb = posb + 1
            if x == "\n":
                lArgs.append(args[posf:posb-1])
                posf = posb
        self.__defer__(*tuple(lArgs))
    def __defer__(self, *args):
        """Defer arguments to each function until a suitable match is found"""
        called = False
        for func in self.funclist:
            try:
                func(self,*args)
            except TypeError:
                continue
            else:
                called = True
                break
        if not called:
            raise TypeError("No function found for args given!")
    funclist = []

class SimpleObject():
    """Simplest property there is -- merely a value and dynamic versions"""
    __type__ = None
    value = None
    def __init__(self, value=None):
        self.value = value

class ComplexObject(SimpleObject):
    """Complex property types -- maps/items/things that have attributes"""
    def GetAttr(self, i):
        return PorkPy.GetAttr(self.value, i, self.__module)

class MapObject(ComplexObject):
    """Class representing a Map property."""
    __type__ = 'pMap'

class ItemObject(ComplexObject):
    """Class representing an Item property."""
    __type__ = 'pItem'

class BoolObject(SimpleObject):
    """A boolean simple prop."""
    __type__ = 'pBool'

class TextObject(SimpleObject):
    """A string (text) simple prop."""
    __type__ = 'pText'

class NumObject(SimpleObject):
    """A numeric simple prop."""
    __type__ = 'pNum'

class Event():
    """A named event for use in engine state changing."""
    def __init__(self, name=None):
        self.name = name

class PropertyMixIn():
    __parentid__ = ""
    __attrname__ = ""
    __module__ = ""
    def __getitem__(self, key):
        dynversions = PorkPy.GetDynDict(self.__module__, self.__attrname__)
        if key in dynversions:
            return dynversions[key]
        else:
            return self

