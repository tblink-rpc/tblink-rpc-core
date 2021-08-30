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
from tblink_rpc_core.interface_type_builder import InterfaceTypeBuilder


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
        self.build_connect_ev = asyncio.Event()
        self.is_peer_built = False
        self.is_peer_connected = False
        self.is_shutdown = False
        self.iftype_m = {}
        self.iftypes  = []
        self.ifinst_m = {}
        self.ifinsts  = []
        self.peer_iftype_m = {}
        self.peer_iftypes  = []
        self.peer_ifinst_m = {}
        self.peer_ifinsts  = []
        self.rsp_l = []
        self.req_id = 1
        self.rsp_m = {}
        
        EndpointMgr.inst().add_endpoint(self)
        
    async def build_complete(self):
        print("--> build_complete", flush=True)
        params = self.transport.mkValMap()
        
        params.setVal("iftypes", self._pack_iftypes(self.iftype_m))
        params.setVal("ifinsts", self._pack_ifinsts(self.ifinst_m))
        
        # TODO: should query services
        params.setVal("time-precision", 
                      self.transport.mkValIntS(self.time_precision))
       
        print("--> send_req", flush=True)
        id = await self.send_req(
            "tblink.build-complete",
            params)
        print("<-- send_req", flush=True)

        print("--> wait_rsp %d" % id, flush=True)            
        result, error = await self.wait_rsp(id)        
        print("<-- wait_rsp %d" % id, flush=True)            
        print("<-- build_complete", flush=True)
        
        # Need to wait for receipt of a build-complete message
        # from the peer
        
        if not self.is_peer_built:
            self.build_connect_ev.clear()
            await self.build_connect_ev.wait()
        
        EndpointMgr.inst().notify_build_complete(self)
    
    async def connect_complete(self):
        print("--> connect_complete", flush=True)
        
        # Need to compare interface instances created
        # by both sides to confirm that they match up
        
        for key in self.ifinst_m.keys():
            if key in self.peer_ifinst_m.keys():
                # Check that the types are the same
                local_ifinst = self.ifinst_m[key]
                peer_ifinst = self.peer_ifinst_m[key]
                
                if local_ifinst.iftype.name != peer_ifinst.iftype.name:
                    raise Exception("Type mismatch for interface instance %s: local=%s peer=%s" % (
                        key, local_ifinst.iftype.name, peer_ifinst.iftype.name))
                    
                if local_ifinst.is_mirror == peer_ifinst.is_mirror:
                    raise Exception("Mirror mismatch for interface instance %s: both have is_mirror=%s" % (
                        key, str(local_ifinst.is_mirror)))
                    
            else:
                raise Exception("Interface instance %s not registered by the peer" % key)
            pass
        
        for key in self.peer_ifinst_m.keys():
            if key not in self.ifinst_m.keys():
                raise Exception("Interface instance %s registered by the peer is not registered" % key)
            pass
        
        params = self.transport.mkValMap()
        
        id = await self.send_req(
            "tblink.connect-complete",
            params)
        
        result, error = await self.wait_rsp(id)        
        
        if not self.is_peer_connected:
            self.build_connect_ev.clear()
            await self.build_connect_ev.wait()
        
        print("<-- connect_complete", flush=True)
        pass
    
    def findInterfaceType(self, name):
        if name in self.iftype_m.keys():
            return self.iftype_m[name]
        else:
            return None
        
    def newInterfaceTypeBuilder(self, name):
        return InterfaceTypeBuilder(name)
    
    def defineInterfaceType(self, type_b):
        iftype = InterfaceType(type_b.name)
        
        for m in type_b.methods:
            iftype.add_method(m)
            
        self.iftype_m[iftype.name] = iftype
        
        return iftype
    
    def defineInterfaceInst(self, iftype, inst_name, is_mirror, req_f):
        if iftype is None:
            raise Exception("iftype is None")

        print("defineInterfaceInst %s is_mirror=%s self=%s" % (
            inst_name, str(is_mirror), str(self)))
        ifinst = InterfaceInst(self, inst_name, iftype, is_mirror, req_f)
        self.ifinst_m[inst_name] = ifinst
        
        return ifinst
    
    def peerInterfaceInsts(self):
        return self.peer_ifinsts
    
    def peerInterfaceTypes(self):
        return self.peer_iftypes
    
    async def send_req(self, method, params, exp_rsp=True):
        print("--> ep.send_req", flush=True)
        id = self.req_id
        self.req_id += 1
        
        if exp_rsp:
            self.rsp_m[id] = (None, None, None)
            
        self.transport.send_req(method, id, params)

        print("<-- ep.send_req", flush=True)
        return id

    def recv_req(self, method, id, params):
        print("recv_req: %s" % method)
        if method == "tblink.build-complete":
            self.time_precision = params.getVal("time-precision").val_s()
            
            self.peer_iftype_m = self._load_iftypes(params.getVal("iftypes"))
            self.peer_ifinst_m = self._load_ifinsts(params.getVal("ifinsts"))
            
            for key in self.peer_iftype_m.keys():
                self.peer_iftypes.append(self.peer_iftype_m[key])
            for key in self.peer_ifinst_m.keys():
                self.peer_ifinsts.append(self.peer_ifinst_m[key])
            
            self.is_peer_built = True
            self.build_connect_ev.set()
            
            result = self.transport.mkValMap()
            self.transport.send_rsp(
                id,
                result,
                None)
        elif method == "tblink.connect-complete":
            
            self.is_peer_connected = True
            self.build_connect_ev.set()
            
            result = self.transport.mkValMap()
            self.transport.send_rsp(
                id,
                result,
                None)
        else:
            print("Error: unhandled method call %s" % method)
        pass
    
    def recv_rsp(self, id, result, error):
        print("--> ep.recv_rsp: %d" % id, flush=True)
        
        if result is None and error is None:
            raise Exception("both result and error are None")
        
        if id in self.rsp_m.keys():
            ex = self.rsp_m[id]
            self.rsp_m[id] = (result, error, None)
            
            if ex[2] is not None:
                ex[2].set()
        else:
            print("Note: id %d not in the map" % id)
                
        if len(self.rsp_l) > 0:
            for cb in self.rsp_l.copy():
                cb(id, result, error)
        print("<-- ep.recv_rsp: %d" % id, flush=True)
        
    async def wait_rsp(self, id):
        
        ex = self.rsp_m[id]

        result = None
        error = None
        
        print("ex[0]=%s ex[1]=%s" % (str(ex[0]), str(ex[1])))
        if ex[0] is not None or ex[1] is not None:
            result = ex[0]
            error = ex[1]
            self.rsp_m.pop(id)
        else:
            ev = asyncio.Event()
            self.rsp_m[id] = (None, None, ev)
            
            await ev.wait()
            result = ex[0]
            error = ex[1]
            self.rsp_m.pop(id)

        return (result, error)
    
    def _load_iftypes(self, iftypes):
        iftype_m = {}
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
            iftype_m[v] = iftype
        return iftype_m
    
    def _pack_iftypes(self, iftype_m):
        ret = self.transport.mkValMap()
        
        for key in iftype_m.keys():
            iftype = iftype_m[key]
            iftype_i = self.transport.mkValMap()
            method_m = self.transport.mkValMap()
            
            for method_n in iftype.method_m.keys():
                mt = iftype.method_m[method_n]
                
                mt_i = self.transport.mkValMap()

                # Note, this is with respect to this side                
                mt_i.setVal("is-export", 
                        self.transport.mkValBool(mt.is_export))
                mt_i.setVal("is-blocking", 
                        self.transport.mkValBool(mt.is_task))
                
                method_m.setVal(method_n, mt_i)

            iftype_i.setVal("methods", method_m)                
            ret.setVal(key, iftype_i)
        
        return ret
        
    
    def _load_ifinsts(self, ifinsts):
        ifinst_m = {}
        
        print("load_ifinsts")
        for ifname in ifinsts.keys():
            print("ifname: %s" % ifname)
            ifinst_v = ifinsts.getVal(ifname)
            ifinst_type = self.iftype_m[ifinst_v.getVal("type").val()]
            is_mirror = ifinst_v.getVal("is-mirror").val()
            ifinst = InterfaceInst(
                self,
                ifname,
                ifinst_type,
                is_mirror)
            ifinst_m[ifname] = ifinst
            
        return ifinst_m
    
    def _pack_ifinsts(self, ifinst_m):
        ret = self.transport.mkValMap()
        
        for key in ifinst_m.keys():
            ifinst = ifinst_m[key]
            ifinst_i = self.transport.mkValMap()
            
            ifinst_i.setVal("type", 
                            self.transport.mkValStr(ifinst.iftype.name))
            ifinst_i.setVal("is-mirror",
                            self.transport.mkValBool(ifinst.is_mirror))
            
            ret.setVal(key, ifinst_i)
        
        return ret
   

