'''
Created on Oct 24, 2021

@author: mballance
'''
from tblink_rpc.type_spec import TypeSpec, TypeKind

class TypeSpecMap(TypeSpec):
    
    def __init__(self, key_t, elem_t):
        super().__init__(TypeKind.Map)
        self.key_t = key_t
        self.elem_t = elem_t
        
        