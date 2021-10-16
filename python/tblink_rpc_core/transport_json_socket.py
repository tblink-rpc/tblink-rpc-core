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
from tblink_rpc_core.transport import Transport
import selectors
import json
from tblink_rpc_core.json.json2param import Json2Param
import select


class TransportJsonSocket(Transport):
    DEBUG_EN = False
    
    def __init__(self, conn):
        self.req_f = None
        self.rsp_f = None
        self.conn = conn
        self.sel = selectors.DefaultSelector()
        self.sel.register(
            self.conn, 
            selectors.EVENT_READ, 
            self._recv_data)
        
        loop = asyncio.get_event_loop()
        loop.add_reader(
            self.conn.fileno(), 
            self._recv_data)
        
        self.drain_pending = False
        self.recv_state = 0
        self.recv_size  = 0
        self.msgbuf_tmp = ""
        pass
    
    def init(self, req_f, rsp_f):
        self.req_f = req_f
        self.rsp_f = rsp_f
        
        # Now that we've been initialized, we can
        # begin monitoring the file descriptor
        pass
   
    def send_req(self, method, id, params) -> int:
        if TransportJsonSocket.DEBUG_EN:
            print("--> Python: send_req method=%s" % method, flush=True)
        
        msg = ParamValMap()
        msg["method"] = ParamValStr(method)
        msg["params"] = params
        msg["id"] = ParamValInt(id)
        
        data = Param2Json().json(msg)
        
        if TransportJsonSocket.DEBUG_EN:
            print("data: %s" % data)
        
        header = ("Content-Length: %d\r\n\r\n" % len(data)).encode()
        self.conn.send(header)
        self.conn.send(data.encode())
#        if not self.drain_pending:
#            asyncio.ensure_future(self._drain())
#            self.drain_pending = True

        if TransportJsonSocket.DEBUG_EN:
            print("<-- Python: send_req method=%s" % method, flush=True)
        
        return id 
    
    def send_rsp(self, id, result, error):
        if TransportJsonSocket.DEBUG_EN:
            print("--> Python: send_rsp id=%d" % id, flush=True)
        
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
        self.conn.send(header)
        self.conn.send(data.encode())

        if TransportJsonSocket.DEBUG_EN:
            print("<-- Python: send_rsp id=%d" % id, flush=True)
            
    def _recv_data(self):
        if TransportJsonSocket.DEBUG_EN:
            print("--> _recv_data", flush=True)

        data = self.conn.recv(1024)
        
        if len(data) == 0:
            raise Exception("Zero-size data received")
        
        if TransportJsonSocket.DEBUG_EN:
            print("  received %d bytes" % len(data))
        
        self.msgbuf_tmp += data.decode()
        
        while len(self.msgbuf_tmp) > 0:
            if TransportJsonSocket.DEBUG_EN:
                print("  msgbuf: %s" % self.msgbuf_tmp)
            if self.recv_state == 0:
                #****************************************************
                #* Receive header
                #****************************************************
                if self.msgbuf_tmp.startswith("Content-Length: "):
                    self.msgbuf_tmp = self.msgbuf_tmp[len("Content-Length: "):]
                    self.recv_state = 1
                else:
                    if len(self.msgbuf_tmp) < len("Content-Length: "):
                        # Wait for more data to arrive
                        break
                    else:
                        print("Error: junk data received: %s" % self.msgbuf_tmp)
                        self.msgbuf_tmp = ""
            elif self.recv_state == 1:
                #****************************************************
                #* Receive message length
                #****************************************************
                newline_idx = self.msgbuf_tmp.find('\n')
                if newline_idx != -1:
                    size_s = self.msgbuf_tmp[0:newline_idx]
                    self.msgbuf_tmp = self.msgbuf_tmp[newline_idx:]
                    self.recv_size = int(size_s.strip())
                    
                    ws=0
                    while ws < len(self.msgbuf_tmp) and self.msgbuf_tmp[ws] in (' ','\r','\t','\n'):
                        ws += 1
                        
                    if ws > 0:
                        self.msgbuf_tmp = self.msgbuf_tmp[ws:]
                    self.recv_state = 2
            elif self.recv_state == 2:
                #****************************************************
                #* Receive message body
                #****************************************************
                if len(self.msgbuf_tmp) >= self.recv_size:
                    body = self.msgbuf_tmp[0:self.recv_size]
                    self.msgbuf_tmp = self.msgbuf_tmp[self.recv_size:]
                    
                    msg = json.loads(body)
                    
                    if "method" in msg.keys():
                        # This is a request
                        id = int(msg['id'])
                        method = msg["method"]
                        params = Json2Param().param(msg["params"])
                        
                        self.req_f(method, id, params)
                    else:
                        # This is a response
                        id = int(msg["id"])
                        result = None
                        error = None
                        
                        if "result" in msg.keys():
                            result = Json2Param().param(msg["result"])
                        if "error" in msg.keys():
                            error = Json2Param().param(msg["error"])
                            
                        self.rsp_f(id, result, error)
                    
                    self.recv_state = 0
                else:
                    print("ran out of space")
                    break
                    
        
        if TransportJsonSocket.DEBUG_EN:
            print("<-- _recv_data", flush=True)

    def process_one_message(self):
#        if TransportJsonSocket.DEBUG_EN:
#            print("--> process_one_message", flush=True)
        try:
            ready_to_read, ready_to_write, in_error = select.select([self.conn], [], [self.conn])
                
#            print("process_one_message: %d %d %d" % (
#                len(ready_to_read), len(ready_to_write), len(in_error)))

            if len(in_error) > 0:
                raise Exception("disconnect")
            elif len(ready_to_read) > 0:
                self._recv_data()
                
            if len(in_error) > 0:
                print("in_error: ", flush=True)
        except Exception:
            print("disconnect")
            raise Exception("disconnect")
         
            
#        if TransportJsonSocket.DEBUG_EN:
#            print("<-- process_one_message", flush=True)
            
    async def process_one_message_a(self):
        if TransportJsonSocket.DEBUG_EN:
            print("--> process_one_message_a", flush=True)
        loop = asyncio.get_event_loop()
        
        events = self.sel.select(0)

        for key, mask in events:
            key.data()

        await asyncio.sleep(0)

        if TransportJsonSocket.DEBUG_EN:
            print("<-- process_one_message_a", flush=True)
            
#    async def _drain(self):
#        await self.writer.drain()
#        self.drain_pending = False
    
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
    
