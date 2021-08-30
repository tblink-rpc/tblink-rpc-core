'''
Created on Aug 26, 2021

@author: mballance
'''

class ParamValBool(object):
    
    def __init__(self, val):
        self._val = val
        
    def val(self):
        return self._val
        
    def accept(self, v):
        v.visit_bool(self)
        