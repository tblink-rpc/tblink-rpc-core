'''
Created on Aug 26, 2021

@author: mballance
'''
from tblink_rpc_core.param_val import ParamVal
from tblink_rpc_core.type_e import TypeE

class ParamValBool(ParamVal):
    
    def __init__(self, val):
        super().__init__(TypeE.Bool)
        self._val = val
        
    def val(self):
        return self._val
        
    def accept(self, v):
        v.visit_bool(self)
        