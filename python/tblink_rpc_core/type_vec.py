'''
Created on Sep 6, 2021

@author: mballance
'''
from tblink_rpc_core.type import Type
from tblink_rpc_core.type_e import TypeE

class TypeVec(Type):
    
    def __init__(self, elem_t):
        super().__init__(TypeE.Vec)
        self._elem_t = elem_t
        
    def elem_t(self) -> Type:
        return self._elem_t
    
