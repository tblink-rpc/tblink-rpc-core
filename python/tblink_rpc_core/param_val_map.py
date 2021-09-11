'''
Created on Jul 5, 2021

@author: mballance
'''
from typing import Dict

from tblink_rpc_core.param_val import ParamVal
from tblink_rpc_core.type_e import TypeE


class ParamValMap(ParamVal):
    
    def __init__(self):
        super().__init__(TypeE.Map)
        self.val_m = {}
        
    def keys(self):
        return self.val_m.keys()
        
    def hasKey(self, key):
        return key in self.val_m.keys()
    
    def getVal(self, key):
        if key in self.val_m.keys():
            return self.val_m[key]
        else:
            return None
        
    def setVal(self, key, val):
        self.val_m[key] = val
        
    def __setitem__(self, key, val):
        self.val_m[key] = val
        
    def __getitem__(self, key):
        return self.val_m[key]
    
    def accept(self, v):
        v.visit_map(self)
        
    
        
    