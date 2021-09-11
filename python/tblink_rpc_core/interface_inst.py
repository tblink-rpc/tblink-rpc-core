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
        self.ep = ep
        self.name = name
        self.iftype = iftype
        self.is_mirror = is_mirror
        self.req_f = req_f
        self.call_id  = 1
        self.rsp_m = {}
        self.pending_call_m = {}
        
    def invoke(self,
               method_t : MethodType,
               params,
               completion_f):
        call_id = self.call_id
        self.call_id += 1
        r_params = self.ep.mkValMap()
        r_params.setVal("ifinst", 
                      self.ep.mkValStr(self.name))
        r_params.setVal("method",
                      self.ep.mkValStr(method_t.name))
        r_params.setVal("call-id", 
                      self.ep.mkValIntU(call_id, 64))
        r_params.setVal("params", params)
        
        self.rsp_m[call_id] = completion_f

        if method_t.is_blocking:
            req_id = self.ep.send_req(
                "tblink.invoke-b",
                r_params)
        else:
            req_id = self.ep.send_req(
                "tblink.invoke-nb",
                r_params,
                self._invoke_rsp_nb)
    
    def invoke_rsp(self, call_id, ret):
        """Called by the client to notify call end"""
        method_t, id = self.pending_call_m[call_id]
        self.pending_call_m.pop(call_id)
        
        if method_t.is_blocking:            
            rsp_params = self.ep.mkValMap()
            rsp_params.setVal("ifinst", self.ep.mkValStr(self.name))
            rsp_params.setVal("call-id", self.ep.mkValIntS(call_id, 32))
            
            if ret is not None:
                rsp_params.setVal("retval", ret)
                
            self.ep.send_req(
                "tblink.invoke-rsp-b",
                rsp_params
                )
        else:
            result = self.ep.mkValMap()
            error = None
            
            result.setVal("call-id", self.ep.mkValIntS(call_id, 32))
            if ret is not None:
                rsp_params.setVal("retval", ret)
                
            self.ep.send_rsp(id, result, error)

    def notify_remote_rsp(self, call_id, ret):
        """Called by the ep to notify completion of a remote call"""
        self.rsp_m[call_id](ret)
        self.rsp_m.pop(call_id)
    
    def _invoke_rsp_nb(self, id, result, error):
        call_id = result.getVal("call-id").val_s()
        retval = None        
        if result.hasKey("retval"):
            retval = result.getVal("retval")
            
        self.rsp_m[call_id](retval)
        self.rsp_m.pop(call_id)
        
    def invoke_local(self, method_t, id, call_id, params):
        self.pending_call_m[call_id] = (method_t, id)
        self.req_f(self, method_t, call_id, params)



 
