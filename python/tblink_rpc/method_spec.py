'''
Created on Oct 17, 2021

@author: mballance
'''
from typing import List, Tuple
from tblink_rpc.type_spec import TypeSpec

class MethodSpec(object):
    
    def __init__(self, 
                 name, 
                 rtype,
                 is_export,
                 is_blocking):
        self.name = name
        self.rtype = rtype
        self.is_export = is_export
        self.is_blocking = is_blocking
        self.params : List[Tuple[str,TypeSpec]] = []
        
    def add_param(self, pname, ptype):
        self.params.append((pname, ptype))
