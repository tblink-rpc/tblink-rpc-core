'''
Created on Oct 19, 2021

@author: mballance
'''
from tblink_rpc.type_spec import TypeSpec, TypeKind

class TypeSpecInt(TypeSpec):
    
    def __init__(self, width, is_signed):
        super().__init__(TypeKind.Int)
        self.width = width
        self.is_signed = is_signed
        
        