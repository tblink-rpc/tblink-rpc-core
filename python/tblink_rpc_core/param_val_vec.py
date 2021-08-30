'''
Created on Aug 26, 2021

@author: mballance
'''

class ParamValVec(object):
    
    def __init__(self):
        self.elems = []
        
    def size(self):
        return len(self.elems)
        
    def at(self, idx):
        return self.elems[idx]
    
    def push_back(self, val):
        self.elems.append(val)
        
    def accept(self):
        self.visit_param_val_vec(self)
        