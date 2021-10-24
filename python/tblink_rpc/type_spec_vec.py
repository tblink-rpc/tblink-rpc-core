'''
Created on Oct 24, 2021

@author: mballance
'''
from tblink_rpc.type_spec import TypeSpec, TypeKind

class TypeSpecVec(TypeSpec):
    
    def __init__(self, elem_t):
        super().__init__(TypeKind.Vec)
        self.elem_t = elem_t
        