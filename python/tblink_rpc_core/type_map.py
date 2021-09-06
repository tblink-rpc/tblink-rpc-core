'''
Created on Sep 6, 2021

@author: mballance
'''
from tblink_rpc_core.type import Type
from tblink_rpc_core.type_e import TypeE

class TypeMap(Type):
    
    def __init__(self, key_t, elem_t):
        super().__init__(TypeE.Map)
        self._key_t = key_t
        self._elem_t = elem_t
        
    def key_t(self) -> Type:
        return self._key_t
    
    def elem_t(self) -> Type:
        return self._elem_t
    