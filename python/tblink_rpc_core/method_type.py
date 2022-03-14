'''
Created on Aug 20, 2021

@author: mballance
'''

class MethodType(object):
    
    def __init__(self, 
                 name, 
                 id,
                 rtype,
                 is_export,
                 is_blocking,
                 params):
        self._name = name
        self._id = id
        self._rtype = rtype
        self._is_export = is_export
        self._is_blocking = is_blocking
        self._params = params.copy()
        
    def name(self):
        return self._name
    
    def id(self):
        return self._id
    
    def is_export(self):
        return self._is_export
    
    def is_blocking(self):
        return self._is_blocking
    
    def rtype(self):
        return self._rtype
        
    def params(self):
        return self._params

    