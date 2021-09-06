'''
Created on Sep 5, 2021

@author: mballance
'''
from tblink_rpc_core.type_int import TypeInt
from tblink_rpc_core.type import Type
from tblink_rpc_core.type_e import TypeE
from tblink_rpc_core.type_map import TypeMap
from tblink_rpc_core.type_vec import TypeVec
from tblink_rpc_core.param_decl import ParamDecl
from tblink_rpc_core.method_type import MethodType


class MethodTypeBuilder(object):
    
    def __init__(self, 
                 name,
                 id,
                 ret_type,
                 is_export,
                 is_blocking):
        self.name = name
        self.id = id
        self.ret_type = ret_type
        self.is_export = is_export
        self.is_blocking = is_blocking
        self.params = []
        
    def add_param(self, name, type):
        self.params.append(ParamDecl(name, type))
        
    def mkMethodType(self):
        return MethodType(
            self.name,
            self.id,
            self.ret_type,
            self.is_export,
            self.is_blocking,
            self.params)
        
        
    
        
    