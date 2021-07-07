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


class JsonTransport(Transport):
    
    def __init__(self, reader, writer):
        super().__init__()
        self.reader = reader
        self.writer = writer
        self.id = 0
        
    async def send_req(self, method, params) -> int:
        msg = ParamValMap()
        msg["method"] = ParamValStr(method)
        msg["params"] = params
        id = self.id
        self.id += 1
        msg["id"] = ParamValInt(id)
        
        data = Param2Json().json(msg)
        
        print("data: %s" % data)
        
        header = ("Content-Length: %d\r\n\r\n" % len(data)).encode()
        self.writer.write(header)
        self.writer.write(data.encode())        
        await self.writer.drain()
        
        return id
    
    async def send_notify(self, method, params):
        raise NotImplementedError("send_notify not implemented by " + str(type(self)))
    
    async def send_rsp(self, id, result, error):
        msg = ParamValMap()
        msg["id"] = ParamValInt(id)
        if result is not None:
            msg["result"] = result
        elif error is not None:
            msg["error"] = error
        else:
            raise Exception("Neither result nor error provided")
    
    async def run(self):
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
            print("hdr=" + hdr_s)
            
            if hdr_s != "Content-Length: ":
                print("Error: unknown header \"%s\"" % hdr_s)
                
            #********************************************************
            #* Read up to first '\n'
            #********************************************************
            size_s = ""
            
            while True:
                c = await self.reader.read(1)
                
                print("c=" + str(c))
                
                if c[0] == 0xa:
                    break
                else:
                    size_s += "%c" % c[0]
                    
            print("size_s=%s" % size_s)
            
            size = int(size_s.strip())
            
            body_s = ""
            
            while len(body_s) < size:
                tmp = await self.reader.read(size-len(body_s))

                #                 
                body_s += tmp.decode().strip()

            print("body=" + body_s + " len=" + str(len(body_s)))
            
            msg = json.loads(body_s)
            
            if "method" in msg.keys():
                # Request
                method = msg["method"]
                id = int(msg["id"])
                if msg["params"] is not None:
                    params = Json2Param().param(msg["params"])
                else:
                    params = ParamValMap()
                print("id=%s" % (str(type(id))))
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
            
            print("msg=" + str(msg))

        # Halt the event loop
#        asyncio.get_event_loop().stop()            
        print("<== msgloop")
        sys.stdout.flush()
