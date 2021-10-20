'''
Created on Oct 17, 2021

@author: mballance
'''
from tblink_rpc.method_spec import MethodSpec


class InterfaceTypeSpec(object):
    
    def __init__(self, name):
        self.name = name
        self.method_m = {}
        self.methods = []
        
    def add_method(self, m : MethodSpec):
        self.methods.append(m)
        self.method_m[m.name] = m
        