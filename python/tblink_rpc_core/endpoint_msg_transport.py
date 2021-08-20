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
        for v in iftypes:
            print("v: " + str(v))
        pass
    
    def _load_ifinsts(self, iftypes):
        pass
   

