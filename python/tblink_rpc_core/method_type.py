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
        self.name = name
        self.id = id
        self.rtype = rtype
        self.is_export = is_export
        self.is_blocking = is_blocking
        self._params = params.copy()
        
    def params(self):
        return self._params

    