'''
Created on Jul 5, 2021

@author: mballance
'''
import json
import sys

from tblink_rpc_core.json.param2json import Param2Json
from tblink_rpc_core.param_val_int import ParamValInt
from tblink_rpc_core.param_val_map import ParamValMap
from tblink_rpc_core.param_val_str import ParamValStr
from tblink_rpc_core.transport import Transport
from tblink_rpc_core.json.json2param import Json2Param
import asyncio


class JsonTransport(Transport):
    DEBUG_EN = False
    
    def __init__(self, reader, writer):
        super().__init__()
        self.reader = reader
        self.writer = writer
        self.id = 0
        self.outstanding = 0
        self.drain_pending = False
        
    def send_req(self, method, params) -> int:
        if JsonTransport.DEBUG_EN:
            print("--> Python: send_req method=%s" % method)
            sys.stdout.flush()
        
        msg = ParamValMap()
        msg["method"] = ParamValStr(method)
        msg["params"] = params
        id = self.id
        self.id += 1
        msg["id"] = ParamValInt(id)
        
        data = Param2Json().json(msg)
        
        if JsonTransport.DEBUG_EN:
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
        
        if JsonTransport.DEBUG_EN:
            print("<-- Python: send_req method=%s" % method)
            sys.stdout.flush()
        
        return id
    
    def send_notify(self, method, params):
        raise NotImplementedError("send_notify not implemented by " + str(type(self)))
    
    def send_rsp(self, id, result, error):
        if JsonTransport.DEBUG_EN:
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
        
        if JsonTransport.DEBUG_EN:
            print("data: %s" % data)
        
        header = ("Content-Length: %d\r\n\r\n" % len(data)).encode()
        self.writer.write(header)
        self.writer.write(data.encode())
#        await self.writer.drain()

        self.outstanding -= 1
        
        if JsonTransport.DEBUG_EN:
            print("<-- Python: send_rsp id=%d" % id)
            sys.stdout.flush()
    
    async def run(self):
        if JsonTransport.DEBUG_EN:
            print("==> msgloop")
        while True:
            #********************************************************
            #* Read header
            #********************************************************
            try:
                hdr = await self.reader.read(len("Content-Length: "))
            except ConnectionResetError as e:
                hdr = ""
                print("Disconnect(1)")
                sys.stdout.flush()
                break
            
            if len(hdr) == 0:
                print("Disconnect(2)")
                sys.stdout.flush()
                break
           
            hdr_s = hdr.decode()
            if JsonTransport.DEBUG_EN:
                print("hdr=" + hdr_s)
                sys.stdout.flush()
            
            if hdr_s != "Content-Length: ":
                print("Error: unknown header \"%s\"" % hdr_s)
                
            #********************************************************
            #* Read up to first '\n'
            #********************************************************
            size_s = ""
            
            while True:
                c = await self.reader.read(1)
                
                if JsonTransport.DEBUG_EN:
                    print("c=" + str(c))
                
                if c[0] == 0xa:
                    break
                else:
                    size_s += "%c" % c[0]
                    
            if JsonTransport.DEBUG_EN:
                print("size_s=%s" % size_s)
                sys.stdout.flush()
            
            size = int(size_s.strip())
            
            body_s = ""
            
            while len(body_s) < size:
                tmp = await self.reader.read(size-len(body_s))

                #                 
                body_s += tmp.decode().strip()

            if JsonTransport.DEBUG_EN:
                print("Python: body=" + body_s + " len=" + str(len(body_s)))
                sys.stdout.flush()
            
            msg = json.loads(body_s)
            
            if JsonTransport.DEBUG_EN:
                print("Python: msg=" + str(msg))
                sys.stdout.flush()
            
            if "method" in msg.keys():
                # Request
                method = msg["method"]
                id = int(msg["id"])
                if msg["params"] is not None:
                    params = Json2Param().param(msg["params"])
                else:
                    params = ParamValMap()
                    
                if JsonTransport.DEBUG_EN:
                    print("id=%s" % (str(type(id))))
                self.outstanding += 1
                await self.req_f(method, id, params)
                pass
            else:
                # Response
                id = int(msg["id"])
                result = None
                error  = None
                if "result" in msg.keys():
                    result = Json2Param().param(msg["result"])
                elif "error" in msg.keys():
                    result = Json2Param().param(msg["error"])
                else:
                    raise Exception("Unknown response format: %s" % (str(msg)))
                await self.rsp_f(id, result, error)
                self.outstanding -= 1
            

        # Halt the event loop
#        asyncio.get_event_loop().stop()            
        if JsonTransport.DEBUG_EN:
            print("<== msgloop")
            sys.stdout.flush()
        
    async def _drain(self):
        await self.writer.drain()
        self.drain_pending = False
        
