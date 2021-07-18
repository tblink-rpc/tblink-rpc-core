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


class TimeUnit(IntEnum):
    ps = -12
    ns = -9
    us = -6
    ms = -3
    s = 1

class Endpoint(object):
    DEBUG_EN = False
    
    def __init__(self, transport):
        self.loop = asyncio.get_event_loop()
        self.transport : Transport = transport
        
        self.transport.init(
            self._req_f,
            self._rsp_f)

        self.rsp_m = {}
        self.rsp_m_lock = Lock()

        self.have_build_complete = False
        self.have_build_complete_ev = Event()
        self.have_connect_complete = False
        self.have_connect_complete_ev = Event()
        
        self.args = None
        
        self.callback_id = 1
        
        self.cb_id_m = {}
        
#    def req_f(self, ):
   
    async def init(self) -> ParamValMap:
        id = self.transport.send_req("tblink.init", ParamValMap())
        
        result, error = await self._wait_rsp(id)
        
        self.args = result["args"]

    async def notify_init(self):
        pass
    
    async def wait_init(self):
        pass
    
    async def build_complete(self):
        # Send build-complete message
        # Wait for a response
        # Return True/False based on response
        id = self.transport.send_req("tblink.build-complete", ParamValMap())
        
        result, error = await self._wait_rsp(id)
        if Endpoint.DEBUG_EN:
            print("result,error=%s,%s" % (str(result), str(error)))
        
        while not self.have_build_complete:
            await self.have_build_complete_ev.wait()
            self.have_build_complete_ev.clear()
            
        
    async def connect_complete(self):
        id = self.transport.send_req("tblink.connect-complete", ParamValMap())
        result, error = await self._wait_rsp(id)

        while not self.have_connect_complete:
            await self.have_connect_complete_ev.wait()
            self.have_connect_complete_ev.clear()
            
    def args(self) -> List[str]:
        """Returns command-line arguments from the peer. Valid after 'init'"""
        return self.args

    async def shutdown(self):
        id = self.transport.send_req("tblink.shutdown", ParamValMap())
        
        # Under some circumstances, it's important to flush the transport
        await self.transport.writer.drain()
#        result, error = await self._wait_rsp(id)
            

    async def add_time_callback(self, time, cb_f) -> int:
        if Endpoint.DEBUG_EN:
            print("--> Python: add_time_callback")
        params = ParamValMap()

        if not iscoroutinefunction(cb_f):
            raise Exception("time callback must be asynchronous")        
        if Endpoint.DEBUG_EN:
            print("cb_f: " + str(type(cb_f)))
        callback_id = self.callback_id
        self.callback_id += 1
        self.cb_id_m[callback_id] = cb_f
        params["time"] = ParamValInt(time)
        params["callback-id"] = ParamValInt(callback_id)
        
        id = self.transport.send_req("tblink.add-time-callback", params)
        result, error = await self._wait_rsp(id)
        
        if Endpoint.DEBUG_EN:
            print("<-- Python: add_time_callback")
        return callback_id
        
    
    async def wait_time(self, time, units=None):
        pass
    
    async def _req_f(self, method, id, params):
        if Endpoint.DEBUG_EN:
            print("Python: REQ_F: %d %s" % (id, method))
        
        if id != -1:
            if method == "tblink.build-complete":
                self.have_build_complete = True
                self.have_build_complete_ev.set()
                self.transport.send_rsp(id, ParamValMap(), None)
            elif method == "tblink.connect-complete":
                self.have_connect_complete = True
                self.have_connect_complete_ev.set()
                self.transport.send_rsp(id, ParamValMap(), None)
            elif method == "tblink.notify-callback":
                callback_id = params["callback-id"].val
                if not callback_id in self.cb_id_m.keys():
                    raise Exception("callback %d not in map" % callback_id)
                cb = self.cb_id_m[callback_id]
                asyncio.ensure_future(cb())
                del self.cb_id_m[callback_id]
                self.transport.send_rsp(id, ParamValMap(), None)
            else:
                print("TODO: %s" % method)
        else:
            if method == "tblink.notify-callback":
                callback_id = params["callback-id"].val
                asyncio.ensure_future(self.cb_id_m[callback_id]())
                del self.cb_id_m[callback_id]
            else:
                print("TODO: %s" % method)
        pass
    
    async def _rsp_f(self, id, result, error):
        if Endpoint.DEBUG_EN:
            print("--> Python: _rsp_f %d" % id)
        sys.stdout.flush()
        
        await self.rsp_m_lock.acquire()
        if id in self.rsp_m.keys():
            ev = self.rsp_m[id][0]
            self.rsp_m[id] = (ev, (result, error))
            ev.set()
        else:
            self.rsp_m[id] = (None, (result, error))
        self.rsp_m_lock.release()
        
        if Endpoint.DEBUG_EN:
            print("<-- Python: _rsp_f %d" % id)
        sys.stdout.flush()
    
    async def _wait_rsp(self, id):
        if Endpoint.DEBUG_EN:
            print("--> Python: wait_rsp %d" % id)
        sys.stdout.flush()
        
        await self.rsp_m_lock.acquire()
        if id in self.rsp_m.keys():
            # Already have a message waiting
            ret = self.rsp_m[id][1]
        else:
            ev = Event()
            self.rsp_m[id] = (ev, None)
            self.rsp_m_lock.release()
            await ev.wait()
            
        await self.rsp_m_lock.acquire()
        ret = self.rsp_m[id][1]
        del self.rsp_m[id]
        
        self.rsp_m_lock.release()

        if Endpoint.DEBUG_EN:
            print("<-- Python: wait_rsp %d" % id)
        sys.stdout.flush()
        return ret
        