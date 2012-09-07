"""-------------------------------------------------
PorkScript Game Engine (C) 2012 Andy J. Brennan
All Rights Reserved.

PIntrinsicFunctions.py --
   Contians all the basic intrinsic functions
   used to interact with the game engine.
   Implementations are in C++.
-------------------------------------------------"""
import PorkPy

def PPrint(message):
    """Print a Message to Game Screen."""
    PorkPy.pprint(message)
    pass

def PReplace(item1, item2):
    """Replace item1 with item2."""
    PorkPy.replace(item1,item2)
    pass

def PChgMap(newmap):
    """Change the current game map to newmap."""
    PorkPy.chgmap(newmap)
    pass

def PRemoveFromMap(item):
    """Remove 1 of 'item' from the current map."""
    PorkPy.removefm(item)
    pass

def PAddToInventory(item):
    """Add 1 of 'item' to the inventory."""
    PorkPy.addtoinv(item)
    pass

def PSendEvent(event):
    """Send state event to engine."""
    PorkPy.sendevent(event)
    pass
