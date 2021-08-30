'''
Created on Aug 22, 2021

@author: mballance
'''
import asyncio
import sys

from tblink_rpc_core.json.param2json import Param2Json
from tblink_rpc_core.param_val_int import ParamValInt
from tblink_rpc_core.param_val_map import ParamValMap
from tblink_rpc_core.param_val_str import ParamValStr
from tblink_rpc_core.param_val_bool import ParamValBool
from tblink_rpc_core.param_val_vec import ParamValVec


class TransportJsonSocket():
    DEBUG_EN = False
    
    def __init__(self):
        self.req_f = None
        self.rsp_f = None
        self.drain_pending = False
        pass
    
    def init(self, req_f, rsp_f):
        self.req_f = req_f
        self.rsp_f = rsp_f
        pass
   
    def send_req(self, method, params) -> int:
        if TransportJsonSocket.DEBUG_EN:
            print("--> Python: send_req method=%s" % method)
            sys.stdout.flush()
        
        msg = ParamValMap()
        msg["method"] = ParamValStr(method)
        msg["params"] = params
        id = self.id
        self.id += 1
        msg["id"] = ParamValInt(id)
        
        data = Param2Json().json(msg)
        
        if TransportJsonSocket.DEBUG_EN:
            print("data: %s" % data)
        
        header = ("Content-Length: %d\r\n\r\n" % len(data)).encode()
        self.writer.write(header)
        self.writer.write(data.encode())        
        if not self.drain_pending:
            asyncio.ensure_future(self._drain())
            self.drain_pending = True
#        await self.writer.drain()

        # Track outstanding activity
        self.outstanding += 1
        
        if TransportJsonSocket.DEBUG_EN:
            print("<-- Python: send_req method=%s" % method)
            sys.stdout.flush()
        
        return id 
    
    def send_rsp(self, id, result, error):
        if TransportJsonSocket.DEBUG_EN:
            print("--> Python: send_rsp id=%d" % id)
            sys.stdout.flush()
        
        msg = ParamValMap()
        msg["id"] = ParamValInt(id)
        if result is not None:
            msg["result"] = result
        elif error is not None:
            msg["error"] = error
        else:
            raise Exception("Neither result nor error provided")
        
        data = Param2Json().json(msg)
        
        if TransportJsonSocket.DEBUG_EN:
            print("data: %s" % data)
        
        header = ("Content-Length: %d\r\n\r\n" % len(data)).encode()
        self.writer.write(header)
        self.writer.write(data.encode())
#        await self.writer.drain()

        self.outstanding -= 1
        
        if TransportJsonSocket.DEBUG_EN:
            print("<-- Python: send_rsp id=%d" % id)
            sys.stdout.flush()
            
    async def _drain(self):
        await self.writer.drain()
        self.drain_pending = False
    
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
    
