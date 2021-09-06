'''
Created on Sep 5, 2021

@author: mballance
'''
from tblink_rpc_core.type_e import TypeE

class Type(object):
    
    def __init__(self, kind : TypeE):
        self._kind = kind
        
    def kind(self) -> TypeE:
        return self._kind
