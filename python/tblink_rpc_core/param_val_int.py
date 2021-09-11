'''
Created on Jul 5, 2021

@author: mballance
'''
from tblink_rpc_core.param_val import ParamVal
from tblink_rpc_core.type_e import TypeE

class ParamValInt(ParamVal):
    
    def __init__(self, val):
        super().__init__(TypeE.Int)
        self.val = val
        
    def accept(self, v):
        v.visit_int(self)
        
    def val_s(self):
        return self.val
    
    def val_u(self):
        return self.val
        