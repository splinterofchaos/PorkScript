# Lil' test command script
from PPorkScript import *
from PIntrinsicFunctions import *

# Create properties
class Msg(TextObject):
    pass

class NewItem(ItemObject):
    pass

# Create commands
class kill(Command):
    def form1(self, item1):
        if item1.type != 'pItem':
            raise TypeError("Arg must be an item!")
        PPrint(item1.GetAttr('Msg'))
        PReplace(item1,item1.GetAttr('NewItem'))
        
    def form2(self, item1, cWith, item2):
        if item1.type != 'pItem' or item2.type != 'pItem':
            raise TypeError("Arg must be an item!")
        if cWith not in ['with','using']:
            raise TypeError("Incorrect conjunction used!")
        PPrint(item1['Msg'].GetDyn(item2.__id))
        PReplace(item1, item1['NewItem'].GetDyn(item2.__id))
        
    funclist = [form1,form2]
