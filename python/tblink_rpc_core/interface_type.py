'''
Created on Aug 20, 2021

@author: mballance
'''

class InterfaceType(object):
    
    def __init__(self, name):
        self.name = name
        self.method_m = {}
        
        pass
    
    def add_method(self, mt):
        self.method_m[mt.name] = mt
        
        