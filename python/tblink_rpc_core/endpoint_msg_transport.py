'''
Created on Jul 2, 2021

@author: mballance
'''
import asyncio
from asyncio.coroutines import iscoroutinefunction
from asyncio.locks import Lock, Event
from enum import Enum, IntEnum
import sys
from typing import List

from tblink_rpc_core.endpoint_mgr import EndpointMgr
from tblink_rpc_core.interface_inst import InterfaceInst
from tblink_rpc_core.interface_type import InterfaceType
from tblink_rpc_core.interface_type_builder import InterfaceTypeBuilder
from tblink_rpc_core.method_type import MethodType
from tblink_rpc_core.msgs.build_complete import BuildComplete
from tblink_rpc_core.param_val_bool import ParamValBool
from tblink_rpc_core.param_val_int import ParamValInt
from tblink_rpc_core.param_val_map import ParamValMap
from tblink_rpc_core.param_val_str import ParamValStr
from tblink_rpc_core.param_val_vec import ParamValVec
from tblink_rpc_core.transport import Transport
from tblink_rpc_core.type import Type
from tblink_rpc_core.type_e import TypeE
from tblink_rpc_core.type_int import TypeInt
from tblink_rpc_core.type_map import TypeMap
from tblink_rpc_core.type_vec import TypeVec
from tblink_rpc_core.param_decl import ParamDecl


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
        
        self.req_f_m = {
            "tblink.build-complete": self._req_build_complete,
            "tblink.connect-complete": self._req_connect_complete,
            "tblink.invoke-b": self._req_invoke_b,
            "tblink.invoke-nb": self._req_invoke_nb,
            "tblink.invoke-rsp-b": self._req_invoke_rsp_b
        }

        EndpointMgr.inst().add_endpoint(self)
        
    async def build_complete(self):
        if EndpointMsgTransport.DEBUG_EN:
            print("--> build_complete", flush=True)
        params = self.transport.mkValMap()
        
        params.setVal("iftypes", self._pack_iftypes(self.iftype_m))
        params.setVal("ifinsts", self._pack_ifinsts(self.ifinst_m))
        
        # TODO: should query services
        params.setVal("time-precision", 
                      self.transport.mkValIntS(self.time_precision))
       
        if EndpointMsgTransport.DEBUG_EN:
            print("--> send_req_wait_rsp", flush=True)            
        result, error = await self.send_req_wait_rsp(
            "tblink.build-complete",
            params)
        if EndpointMsgTransport.DEBUG_EN:
            print("<-- send_req_wait_rsp", flush=True)
            
        if EndpointMsgTransport.DEBUG_EN:
            print("<-- build_complete", flush=True)
        
        # Need to wait for receipt of a build-complete message
        # from the peer
        
        if not self.is_peer_built:
            self.build_connect_ev.clear()
            await self.build_connect_ev.wait()
        
        EndpointMgr.inst().notify_build_complete(self)
    
    async def connect_complete(self):
        if EndpointMsgTransport.DEBUG_EN:
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
        
        result, error = await self.send_req_wait_rsp(
            "tblink.connect-complete",
            params)
        
        if not self.is_peer_connected:
            self.build_connect_ev.clear()
            await self.build_connect_ev.wait()
        
        if EndpointMsgTransport.DEBUG_EN:
            print("<-- connect_complete", flush=True)
    
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

        ifinst = InterfaceInst(self, inst_name, iftype, is_mirror, req_f)
        self.ifinst_m[inst_name] = ifinst
        
        return ifinst
    
    def peerInterfaceInsts(self):
        return self.peer_ifinsts
    
    def peerInterfaceTypes(self):
        return self.peer_iftypes
    
    def send_req(self, 
                 method, 
                 params, 
                 completion_f=None):
        if EndpointMsgTransport.DEBUG_EN:
            print("--> ep.send_req", flush=True)
        id = self.req_id
        self.req_id += 1
        
        if completion_f:
            self.rsp_m[id] = completion_f
            
        self.transport.send_req(method, id, params)

        if EndpointMsgTransport.DEBUG_EN:
            print("<-- ep.send_req", flush=True)
        return id

    def recv_req(self, method, id, params):
        if EndpointMsgTransport.DEBUG_EN:
            print("recv_req: %s" % method)
        result = None
        error = None
        
        if method in self.req_f_m.keys():
            result, error = self.req_f_m[method](id, params)
        else:
            if EndpointMsgTransport.DEBUG_EN:
                print("Error: unhandled method %s" % method)
            
        if result is not None or error is not None:
            self.transport.send_rsp(
                id,
                result,
                error)
            
    def send_rsp(self, id, result, error):
        self.transport.send_rsp(
            id,
            result,
            error)

    def _req_build_complete(self, id, params):
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
        
        return (result, None)
        
    def _req_connect_complete(self, id, params):
        error = None
        result = self.transport.mkValMap()
        
        self.is_peer_connected = True
        self.build_connect_ev.set()
        
        return (result, error)
            
    def _req_invoke_b(self, id, params):
        error = None
        result = self.transport.mkValMap()
        if EndpointMsgTransport.DEBUG_EN:
            print("_req_invoke_b")
        ifinst_n = params.getVal("ifinst").val()
        method_n = params.getVal("method").val()
        call_id  = params.getVal("call-id").val_s()
        call_params = params.getVal("params")
        
        if ifinst_n not in self.ifinst_m.keys():
            raise Exception("Interface %s not in map" % ifinst_n)
        else:
            ifinst = self.ifinst_m[ifinst_n]

        if method_n not in ifinst.iftype.method_m.keys():
            raise Exception("Method %s not found in iftype %s" % (
                method_n, ifinst.iftype.name))
        else:
            method_t = ifinst.iftype.method_m[method_n]
        
        ifinst.invoke_local(method_t, id, call_id, call_params)

        # Send an 'ok' response. A subsequent request
        # will be sent to complete the call        
        return (result, error)
        pass
    
    def _req_invoke_nb(self, id, params):
        error = None
        result = self.transport.mkValMap()

        if EndpointMsgTransport.DEBUG_EN:
            print("_req_invoke_nb")        
        ifinst_n = params.getVal("ifinst").val()
        method_n = params.getVal("method").val()
        call_id  = params.getVal("call-id").val_s()
        call_params = params.getVal("params")
        
        if ifinst_n not in self.ifinst_m.keys():
            raise Exception("Interface %s not in map" % ifinst_n)
        else:
            ifinst = self.ifinst_m[ifinst_n]

        if method_n not in ifinst.iftype.method_m.keys():
            raise Exception("Method %s not found in iftype %s" % (
                method_n, ifinst.iftype.name))
        else:
            method_t = ifinst.iftype.method_m[method_n]
        
        ifinst.invoke_local(method_t, id, call_id, call_params)

        return (None, None)
    
    def _req_invoke_rsp_b(self, id, params):
        ifinst_n = params.getVal("ifinst").val()
        call_id  = params.getVal("call-id").val_s()

        retval = None        
        if params.hasKey("retval"):
            retval = params.getVal("retval")
        
        if ifinst_n not in self.ifinst_m.keys():
            raise Exception("Interface %s not in map" % ifinst_n)
        else:
            ifinst = self.ifinst_m[ifinst_n]

        ifinst.notify_remote_rsp(call_id, retval)
        
        error = None
        result = self.transport.mkValMap()
        
        return (result, error)
    
    def recv_rsp(self, id, result, error):
        if EndpointMsgTransport.DEBUG_EN:
            print("--> ep.recv_rsp: %d" % id, flush=True)
        
        if result is None and error is None:
            raise Exception("both result and error are None")
        
        if id in self.rsp_m.keys():
            completion_f = self.rsp_m[id]
            self.rsp_m.pop(id)
            completion_f(id, result, error)
        else:
            if EndpointMsgTransport.DEBUG_EN:
                print("Note: id %d not in the map" % id)
                
        if len(self.rsp_l) > 0:
            for cb in self.rsp_l.copy():
                cb(id, result, error)
        if EndpointMsgTransport.DEBUG_EN:
            print("<-- ep.recv_rsp: %d" % id, flush=True)
        
    async def send_req_wait_rsp(self, method, params):
        
        ev = asyncio.Event()
        
        result = None
        error = None
        
        def _rsp_f(id, _result, _error):
            nonlocal result, error, ev
            result = _result
            error  = _error
            ev.set()
            
        self.send_req(method, params, _rsp_f)
        
        if result is not None and error is not None:
            await ev.wait()
        
        return (result, error)
    
    def mkValBool(self, v):
        return ParamValBool(v)
    
    def mkValIntS(self, val, width):
        return ParamValInt(val)
    
    def mkValIntU(self, val, width):
        return ParamValInt(val)
    
    def mkValMap(self):
        return ParamValMap()
    
    def mkValVec(self):
        return ParamValVec()
    
    def mkValStr(self, v):
        return ParamValStr(v)
    
    def _load_iftypes(self, iftypes):
        iftype_m = {}
        for v in iftypes.keys():
            iftype_i = iftypes.getVal(v)
            iftype = InterfaceType(v)
            
            methods = iftype_i.getVal("methods")
            
            for mkey in methods.keys():
                method_t = methods.getVal(mkey)
                
                id = -1; #iftype_i.getValue("id").val_s()
                signature : ParamValMap = method_t.getVal("signature")
                
                if signature.hasKey("rtype"):
                    rtype = self._load_type(signature.getVal("rtype"))
                else:
                    rtype = None

                params = []
                parameters : ParamValVec = signature.getVal("parameters")                    
                for i in range(parameters.size()):
                    pv : ParamValMap = parameters.at(i)
                    pname = pv.getVal("name").val()
                    ptype = self._load_type(pv.getVal("type"))
                    params.append(ParamDecl(pname, ptype))
                
                is_export = method_t.getVal("is-export").val()
                is_blocking = method_t.getVal("is-blocking").val()
                
                
                mt = MethodType(
                    mkey,
                    id,
                    rtype,
                    is_export,
                    is_blocking,
                    params)
                
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
                        self.transport.mkValBool(mt.is_blocking))
                signature = self.transport.mkValMap()
                if mt.rtype is not None:
                    signature.setVal("rtype", self._pack_type(mt.rtype))

                params = self.transport.mkValVec()
                for p in mt.params():
                    p_m = self.transport.mkValMap()
                    p_m.setVal("name", self.transport.mkValStr(p.name()))
                    p_m.setVal("type", self._pack_type(p.type()))
                    params.push_back(p_m)
                    
                signature.setVal("parameters", params)
                mt_i.setVal("signature", signature)
                
                method_m.setVal(method_n, mt_i)

            iftype_i.setVal("methods", method_m)                
            ret.setVal(key, iftype_i)
        
        return ret
        
    
    def _load_ifinsts(self, ifinsts):
        ifinst_m = {}
        
        for ifname in ifinsts.keys():
            if EndpointMsgTransport.DEBUG_EN:
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

    _str2kind_m = {
        "bool" : TypeE.Bool,
        "int"  : TypeE.Int,
        "map"  : TypeE.Map,
        "str"  : TypeE.Str,
        "vec"  : TypeE.Vec
        }    
    def _load_type(self, t : ParamValMap):
        kind = EndpointMsgTransport._str2kind_m[t.getVal("kind").val()]

        ret = None
        
        if kind in (TypeE.Bool,TypeE.Str):
            ret = Type(kind)
        elif kind == TypeE.Int:
            ret = TypeInt(
                t.getVal("is-signed").val(),
                t.getVal("width").val_s())
        elif kind == TypeE.Map:
            key_t = self._load_type(t.getVal("key-type"))
            elem_t = self._load_type(t.getVal("elem-type"))
            ret = TypeMap(key_t, elem_t)
        elif kind == TypeE.Vec:
            elem_t = self._load_type(t.getVal("elem-type"))
            ret = TypeVec(key_t, elem_t)

        if ret is None:
            raise Exception("Failed to load type %s" % str(kind))

        return ret                    
    
    _kind2str_m = {
        TypeE.Bool : "bool",
        TypeE.Int  : "int",
        TypeE.Map  : "map",
        TypeE.Str  : "str",
        TypeE.Vec  : "vec"
        }    
    def _pack_type(self, t : Type):
        ret = self.transport.mkValMap()
        ret.setVal("kind", self.transport.mkValStr(
            EndpointMsgTransport._kind2str_m[t.kind()]))
        
        if t.kind() == TypeE.Int:
            ret.setVal("is-signed", self.transport.mkValBool(t.is_signed()))
            ret.setVal("width", self.transport.mkValIntS(t.width()))
        elif t.kind() == TypeE.Map:
            ret.setVal("key-type", self._pack_type(t.key_t()))
            ret.setVal("elem-type", self._pack_type(t.elem_t()))
        elif t.kind() == TypeE.Vec:
            ret.setVal("elem-type", self._pack_type(t.elem_t()))
            
        return ret
        
