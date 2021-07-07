'''
Created on Jul 5, 2021

@author: mballance
'''

class ParamValInt(object):
    
    def __init__(self, val):
        self.val = val
        
    def accept(self, v):
        v.visit_int(self)
        