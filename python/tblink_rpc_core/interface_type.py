'''
Created on Aug 20, 2021

@author: mballance
'''
from tblink_rpc_core.method_type_builder import MethodTypeBuilder
from tblink_rpc_core.type import Type
from tblink_rpc_core.type_e import TypeE
from tblink_rpc_core.type_int import TypeInt
from tblink_rpc_core.type_map import TypeMap
from tblink_rpc_core.type_vec import TypeVec
from tblink_rpc_core.method_type import MethodType


class InterfaceType(object):
    
    def __init__(self, name):
        self._name = name
        self.method_m = {}
        self.methods = []
        
    def name(self):
        return self._name
        
    def add_method(self, mt : MethodType):
        if mt.name in self.method_m.keys():
            raise Exception("Duplicate method %s in interface-type %s" % (
                mt.name, self.name))
        self.method_m[mt.name] = mt
        self.methods.append(mt)
        

        