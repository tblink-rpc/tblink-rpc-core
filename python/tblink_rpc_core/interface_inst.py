'''
Created on Aug 21, 2021

@author: mballance
'''
from tblink_rpc_core.method_type import MethodType

class InterfaceInst(object):
    
    def __init__(self,
                 endpoint,
                 name,
                 iftype,
                 is_mirror,
                 req_f=None):
        self.endpoint = endpoint
        self.name = name
        self.iftype = iftype
        self.is_mirror = is_mirror
        self.req_f = req_f
        self.call_id  = 1
        self.rsp_m = {}
        
    def invoke_nb(self,
               method_t : MethodType,
               params,
               completion_f):
        call_id = self.call_id
        self.call_id += 1
        r_params = self.endpoint.mkValMap()
        r_params.setVal("ifinst", 
                      self.endpoint.mkValStr(self.name))
        r_params.setVal("method",
                      self.endpoint.mkValStr(method_t.name))
        r_params.setVal("call-id", 
                      self.endpoint.mkValIntU(call_id))
        r_params.setVal("params", params)
        
        if completion_f is not None:
            self.rsp_m[call_id] = (None, None)

        req_id = self.endpoint.send_req(
            "tblink.invoke-nb",
            r_params)

        if call_id in self.rsp_m.keys() and self.rsp_m[call_id] is not None:
            completion_f(self.rsp_m[call_id][0])
            self.rsp_m.pop(call_id)
        
        pass
    
    async def invoke_b(self,
                 method_t : MethodType,
                 params,
                 completion_f):
        pass
    
    def invoke_rsp(self, call_id, ret):
        
        if call_id in self.rsp_m.keys():
            # TODO: need to clone ret?
            self.rsp_m[call_id] = ret
            
            # TODO: if 
        
            
        pass
        
        