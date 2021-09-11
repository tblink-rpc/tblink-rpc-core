'''
Created on Sep 9, 2021

@author: mballance
'''
from tblink_rpc_core.type_e import TypeE

class ParamVal(object):
    
    def __init__(self, type : TypeE):
        self._type = type

    def type(self):
        return self._type