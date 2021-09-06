'''
Created on Sep 5, 2021

@author: mballance
'''
from tblink_rpc_core.type import Type
from tblink_rpc_core.type_e import TypeE

class TypeInt(Type):
    
    def __init__(self, is_signed, width):
        super().__init__(TypeE.Int)
        self._is_signed = is_signed
        self._width = width
        pass
    
    def is_signed(self):
        return self._is_signed
    
    def width(self):
        return self._width
    