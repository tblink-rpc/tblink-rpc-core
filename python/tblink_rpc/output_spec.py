'''
Created on Oct 20, 2021

@author: mballance
'''
from tblink_rpc.interface_type_spec import InterfaceTypeSpec
from typing import List

class OutputSpec(object):
    
    def __init__(self):
        self.namespace = None
        self.out = None
#        self.stypes : 
        self.iftypes : List[InterfaceTypeSpec] = []
