'''
Created on Aug 29, 2021

@author: mballance
'''
from tblink_rpc_core.method_type import MethodType
from tblink_rpc_core.method_type_builder import MethodTypeBuilder
from tblink_rpc_core.type import Type
from tblink_rpc_core.type_e import TypeE
from tblink_rpc_core.type_int import TypeInt
from tblink_rpc_core.type_map import TypeMap
from tblink_rpc_core.type_vec import TypeVec


class InterfaceTypeBuilder(object):
    
    def __init__(self, name):
        self.name = name
        self.methods  = []
        self.method_m = {}
        
    def newMethodTypeBuilder(self, 
                             name,
                             id,
                             ret_type,
                             is_export,
                             is_blocking):
        return MethodTypeBuilder(name, id, ret_type, is_export, is_blocking)
    
    def add_method(self, mtb) -> MethodType:
        mt = mtb.mkMethodType()
        if mt.name in self.method_m.keys():
            raise Exception("Method %s already exists in interface type %s" % (
                mt.name, self.name))
        self.method_m[mt.name] = mt
        self.methods.append(mt)
        
        return mt
        
    def mkTypeBool(self) -> Type:
        return Type(TypeE.Bool)
    
    def mkTypeInt(self, is_signed, width) -> Type:
        return TypeInt(is_signed, width)
    
    def mkTypeMap(self, key_t, elem_t) -> Type:
        return TypeMap(key_t, elem_t)
    
    def mkTypeStr(self) -> Type:
        return Type(TypeE.Str)
    
    def mkTypeVec(self, elem_t) -> Type:
        return TypeVec(elem_t)
        