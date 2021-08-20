'''
Created on Aug 20, 2021

@author: mballance
'''

class EndpointMgr(object):
    
    _inst = None
    
    def __init__(self):
        self.endpoints = []
        pass
    
    def add_endpoint(self, ep):
        self.endpoints.append(ep)
    
    @classmethod
    def inst(cls):
        if cls._inst is None:
            cls._inst = EndpointMgr()
        return cls._inst
    