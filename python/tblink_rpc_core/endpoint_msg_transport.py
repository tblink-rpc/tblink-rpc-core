'''
Created on Jul 2, 2021

@author: mballance
'''
from asyncio.locks import Lock, Event
from enum import Enum, IntEnum
import sys

from tblink_rpc_core.msgs.build_complete import BuildComplete
from tblink_rpc_core.param_val_map import ParamValMap
from tblink_rpc_core.transport import Transport
from tblink_rpc_core.param_val_int import ParamValInt
from asyncio.coroutines import iscoroutinefunction
import asyncio
from typing import List
from tblink_rpc_core.endpoint_mgr import EndpointMgr
from tblink_rpc_core.interface_type import InterfaceType
from tblink_rpc_core.method_type import MethodType
from tblink_rpc_core.interface_inst import InterfaceInst


class TimeUnit(IntEnum):
    ps = -12
    ns = -9
    us = -6
    ms = -3
    s = 1

class EndpointMsgTransport(object):
    DEBUG_EN = False
    
    def __init__(self, transport):
        self.transport = transport
        self.transport.init(self.recv_req, self.recv_rsp)
        self.time_precision = -9
        self.is_peer_built = False
        self.is_peer_connected = False
        self.is_shutdown = False
        self.iftype_m = {}
        self.ifinst_m = {}
        
        EndpointMgr.inst().add_endpoint(self)
        

    def recv_req(self, method, id, params):
        print("recv_req: %s" % method)
        if method == "tblink.build-complete":
            self.time_precision = params.getVal("time-precision").val_s()
            
            self._load_iftypes(params.getVal("iftypes"))
            self._load_ifinsts(params.getVal("ifinsts"))
            
            self.is_peer_built = True
            
            result = self.transport.mkValMap()
            self.transport.send_rsp(
                id,
                result,
                None)
        elif method == "tblink.connect-complete":
            
            self.is_peer_connected = True
            
            result = self.transport.mkValMap()
            self.transport.send_rsp(
                id,
                result,
                None)
        else:
            print("Error: unhandled method call %s" % method)
        pass
    
    def recv_rsp(self, id, result, error):   
        print("recv_rsp: %d" % id)
        pass
    
    def _load_iftypes(self, iftypes):
        for v in iftypes.keys():
            iftype_i = iftypes.getVal(v)
            iftype = InterfaceType(v)
            
            methods = iftype_i.getVal("methods")
            
            for mkey in methods.keys():
                method_t = methods.getVal(mkey)
                
                id = -1; #iftype_i.getValue("id").val_s()
                signature = method_t.getVal("signature").val()
                is_export = method_t.getVal("is-export").val()
                is_blocking = method_t.getVal("is-blocking").val()
                
                mt = MethodType(
                    mkey,
                    id,
                    signature,
                    is_export,
                    is_blocking)
                
                iftype.add_method(mt)
            self.iftype_m[v] = iftype
        pass
    
    def _load_ifinsts(self, ifinsts):
        print("load_ifinsts")
        for ifname in ifinsts.keys():
            print("ifname: %s" % ifname)
            ifinst_v = ifinsts.getVal(ifname)
            ifinst_type = self.iftype_m[ifinst_v.getVal("type").val()]
            ifinst = InterfaceInst(
                self,
                ifname,
                ifinst_type)
            self.ifinst_m[ifname] = ifinst
        pass
   

