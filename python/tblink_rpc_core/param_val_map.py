'''
Created on Jul 5, 2021

@author: mballance
'''
from typing import Dict

class ParamValMap(object):
    
    def __init__(self):
        self.val_m = {}
        
    def __setitem__(self, key, val):
        self.val_m[key] = val
        
    def __getitem__(self, key):
        return self.val_m[key]
    
    def accept(self, v):
        v.visit_map(self)
        
    
        
    