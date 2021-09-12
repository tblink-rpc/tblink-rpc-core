'''
Created on Jul 5, 2021

@author: mballance
'''

class Transport(object):
    
    def __init__(self):
        self.req_f = None
        self.rsp_f = None
        pass
    
    def init(self, req_f, rsp_f):
        self.req_f = req_f
        self.rsp_f = rsp_f
        
    def send_req(self, method, params) -> int:
        raise NotImplementedError("send_req not implemented by " + str(type(self)))
    
    def send_rsp(self, id, result, error):
        raise NotImplementedError("send_rsp not implemented by " + str(type(self)))
    
    def process_one_message(self):
        raise NotImplementedError("process_one_message not implemented by " + str(type(self)))
    
    async def process_one_message_a(self):
        raise NotImplementedError("process_one_message not implemented by " + str(type(self)))
    
    async def run(self):
        raise NotImplementedError("poll not implemented by " + str(type(self)))
    