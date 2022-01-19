'''
Created on Jan 17, 2022

@author: mballance
'''
from enum import Enum, auto

class TbLinkEventKind(Enum):
    AddEndpoint = auto()
    RemEndpoint = auto()
    
class TbLinkEvent(object):
    
    def __init__(self, kind : TbLinkEventKind, hndl):
        self._kind = kind
        self._hndl = hndl
        
    def kind(self):
        return self._kind
    
    def hndl(self):
        return self._hndl
