'''
Created on Aug 20, 2021

@author: mballance
'''

class MethodType(object):
    
    def __init__(self, 
                 name, 
                 id,
                 signature,
                 is_export,
                 is_blocking):
        self.name = name
        self.id = id
        self.signature = signature
        self.is_export = is_export
        self.is_blocking = is_blocking
        pass

    