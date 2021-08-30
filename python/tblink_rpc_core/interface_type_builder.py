'''
Created on Aug 29, 2021

@author: mballance
'''
from tblink_rpc_core.method_type import MethodType


class InterfaceTypeBuilder(object):
    
    def __init__(self, name):
        self.name = name
        self.methods  = []
        self.method_m = {}
        
    def define_method(self, 
                      name : str, 
                      id : int,
                      signature : str,
                      is_export : bool,
                      is_blocking : bool):
        method_t = MethodType(
            name,
            id,
            signature,
            is_export,
            is_blocking)
        self.methods.append(method_t)
        self.methods_m[name] = method_t
        