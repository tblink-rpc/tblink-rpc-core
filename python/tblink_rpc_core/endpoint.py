'''
Created on Jul 2, 2021

@author: mballance
'''
from enum import Enum, IntEnum
from tblink_rpc_core.msgs.build_complete import BuildComplete
from tblink_rpc_core.transport import Transport
from tblink_rpc_core.param_val_map import ParamValMap
from asyncio.locks import Lock, Event

class TimeUnit(IntEnum):
    ps = -12
    ns = -9
    us = -6
    ms = -3
    s = 1

class Endpoint(object):
    
    def __init__(self, transport):
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
        
#    def req_f(self, ):
    
    async def notify_init(self):
        pass
    
    async def wait_init(self):
        pass
    
    async def build_complete(self):
        # Send build-complete message
        # Wait for a response
        # Return True/False based on response
        id = await self.transport.send_req("tblink.build-complete", ParamValMap())
        result, error = await self._wait_rsp(id)
        
        print("result,error=%s,%s" % (str(result), str(error)))
        
        pass
    
#    async def wait_build_complete(self):
#        pass
    
    async def connect_complete(self):
        pass
    
    async def wait_time(self, time, units=None):
        pass
    
    async def _req_f(self, method, id, params):
        print("REQ_F: %d %s" % (id, method))
        if method == "tblink.build-complete":
            self.have_build_complete = True
            self.have_build_complete_ev.set()
            await self.transport.send_rsp(id, ParamValMap(), None)
        elif method == "tblink.connect-complete":
            self.have_connect_complete = True
            self.have_connect_complete_ev.set()
            await self.transport.send_rsp(id, ParamValMap(), None)
        else:
            print("TODO:")
        pass
    
    async def _rsp_f(self, id, result, error):
        await self.rsp_m_lock.acquire()
        if id in self.rsp_m.keys():
            ev = self.rsp_m[id][0]
            self.rsp_m[id] = (ev, (result, error))
            ev.set()
        else:
            self.rsp_m[id] = (None, (result, error))
        await self.rsp_m_lock.release()
    
    async def _wait_rsp(self, id):
        await self.rsp_m_lock.acquire()
        if id in self.rsp_m.keys():
            # Already have a message waiting
            ret = self.rsp_m[id][1]
        else:
            ev = Event()
            self.rsp_m[id] = (ev, None)
            await ev.wait()
            
        ret = self.rsp_m[id][1]
        del self.rsp_m[id]
        
        self.rsp_m_lock.release()

        return ret
        