'''
Created on Aug 21, 2021

@author: mballance
'''
from tblink_rpc_core.method_type import MethodType

class InterfaceInst(object):
    
    def __init__(self,
                 ep,
                 name,
                 iftype,
                 is_mirror,
                 req_f=None):
        self._ep = ep
        self._name = name
        self._iftype = iftype
        self._is_mirror = is_mirror
        self._req_f = req_f
        self._call_id  = 1
        self._rsp_m = {}
        self._pending_call_m = {}
        
    def name(self):
        return self._name
    
    def type(self):
        return self._iftype
    
    def is_mirror(self):
        return self._is_mirror
        
    def invoke(self,
               method_t : MethodType,
               params,
               completion_f):
        raise NotImplementedError("invoke not implemented for %s" % str(type(self)))
    
    def invoke_rsp(self, call_id, ret):
        """Called by the client to notify call end"""
        raise NotImplementedError("invoke_rsp not implemented for %s" % str(type(self)))
