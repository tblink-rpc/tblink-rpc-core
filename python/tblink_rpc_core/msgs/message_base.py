'''
Created on Jul 6, 2021

@author: mballance
'''
from tblink_rpc_core.param_val_map import ParamValMap

class MessageBase(object):
    
    def __init__(self, method):
        self.method = method
        self.params = ParamValMap()
        
        
    async def send_req(self, transport) -> int:
        return await transport.send_req(
            self.method,
            self.params)
        