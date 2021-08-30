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
        self.dflt_ep = None
        pass
    
    def default(self):
        if self.dflt_ep is not None:
            return self.dflt_ep
        else:
            if len(self.endpoints) > 0:
                return self.endpoints[0]
            else:
                raise Exception("No endpoints registered")
    
    def add_endpoint(self, ep):
        self.endpoints.append(ep)
        
        # Notify listeners
        if len(self.listeners) > 0:
            for l in self.listeners.copy():
                l.endpoint_added(ep)
        
    def add_listener(self, l):
        self.listeners.append(l)

    def notify_build_complete(self, ep):
        """Called by an endpoint to notify listeners that build is complete"""
        
        if len(self.listeners) > 0:
            for l in self.listeners.copy():
                l.build_complete(ep)
        
    
    @classmethod
    def inst(cls):
        if cls._inst is None:
            cls._inst = EndpointMgr()
        return cls._inst
    