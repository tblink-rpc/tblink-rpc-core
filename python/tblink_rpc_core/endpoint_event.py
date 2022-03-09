'''
Created on Mar 6, 2022

@author: mballance
'''
from tblink_rpc_core.event_type_e import EventTypeE

class EndpointEvent(object):
    
    def __init__(self, kind : EventTypeE):
        self._kind = kind
        
    def kind(self) -> EventTypeE:
        return self._kind