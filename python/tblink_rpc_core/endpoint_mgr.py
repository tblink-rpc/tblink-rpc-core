'''
Created on Aug 20, 2021

@author: mballance
'''
from typing import List
from tblink_rpc_core.endpoint_mgr_listener import EndpointMgrListener

class EndpointMgr(object):
    
    _inst = None
    
    def __init__(self):
        self.endpoints = []
        self.listeners : List[EndpointMgrListener] = []
        pass
    
    def add_endpoint(self, ep):
        self.endpoints.append(ep)
        
        # Notify listeners
        if len(self.listeners) > 0:
            for l in self.listeners.copy():
                l.endpoint_added(ep)
        
    def add_listener(self, l):
        self.listeners.append(l)
    
    @classmethod
    def inst(cls):
        if cls._inst is None:
            cls._inst = EndpointMgr()
        return cls._inst
    