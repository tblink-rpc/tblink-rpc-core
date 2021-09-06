'''
Created on Sep 6, 2021

@author: mballance
'''

class ParamDecl(object):
    
    def __init__(self, name, type):
        self._name = name
        self._type = type
        
    def name(self):
        return self._name
    
    def type(self):
        return self._type