'''
Created on Aug 25, 2021

@author: mballance
'''
from tblink_rpc_core.param_val_bool import ParamValBool
from tblink_rpc_core.param_val_int import ParamValInt
from tblink_rpc_core.param_val_map import ParamValMap
from tblink_rpc_core.param_val_str import ParamValStr
from tblink_rpc_core.param_val_vec import ParamValVec


class TransportDualFifo(object):
    
    class TransportEP(object):
   
        def __init__(self, parent, id):
            self.parent = parent
            self.id = id
            self.req_f = None
            self.rsp_f = None
            pass
        
        def init(self, req_f, rsp_f):
            self.req_f = req_f
            self.rsp_f = rsp_f
            
        def send_req(self,
                     method,
                     id,
                     params):
            print("--> [%d] transport.send_req %s" % (self.id, method), flush=True)
            if self.id == 1:
                other_id = 0
            else:
                other_id = 1
            print("  id=%d other=%d" % (self.id, other_id))
            self.parent.ep[other_id].recv_req(
                method,
                id,
                params)
            print("<-- [%d] transport.send_req %s" % (self.id, method), flush=True)
        
        def send_rsp(self,
                     id,
                     result,
                     error):
            print("--> [%d] transport.send_rsp %d" % (self.id, id), flush=True)
            if self.id == 1:
                other_id = 0
            else:
                other_id = 1
            print("  id=%d other=%d" % (self.id, other_id))
            self.parent.ep[other_id].recv_rsp(
                id,
                result,
                error)
            print("<-- [%d] transport.send_rsp %d" % (self.id, id), flush=True)
        
        def recv_req(self, method, id, params):
            print("--> [%d] transport.recv_req: %s" % (self.id, method), flush=True)
            if self.req_f is not None:
                self.req_f(method, id, params)
            else:
                print("Warning: not connected")
                # TODO: 
                pass
            print("<-- [%d] transport.recv_req: %s" % (self.id, method), flush=True)
        
        def recv_rsp(self, id, result, error):
            print("--> [%d] transport.recv_rsp: %d" % (self.id, id), flush=True)
            if self.rsp_f is not None:
                self.rsp_f(id, result, error)
            else:
                print("Warning: not connected")
                # TODO: 
                pass
            print("<-- [%d] transport.recv_rsp: %d" % (self.id, id), flush=True)
        
        def mkValBool(self, v):
            return ParamValBool(v)
        
        def mkValIntS(self, v):
            return ParamValInt(v)
        
        def mkValIntU(self, v):
            return ParamValInt(v)
        
        def mkValMap(self):
            return ParamValMap()
    
        def mkValStr(self, v):
            return ParamValStr(v)
    
        def mkValVec(self):
            return ParamValVec()
            
    def __init__(self):
        self.ep = [
            TransportDualFifo.TransportEP(self, 0),
            TransportDualFifo.TransportEP(self, 1)]
        pass
    
    
    