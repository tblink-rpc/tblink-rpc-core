'''
Created on Jul 7, 2021

@author: mballance
'''
from tblink_rpc_core.endpoint import Endpoint
from typing import List

class EndpointServices(object):
    
    def __init__(self):
        pass
    
    def init(self, ep : Endpoint):
        pass
    
    def args(self) -> List[str]:
        raise NotImplementedError("args not implemented by class %s" % str(type(self)))
    
    def shutdown(self): 
        raise NotImplementedError("shutdown not implemented by class %s" % str(type(self)))
    
    def add_time_cb(self, time, callback_id) -> int:
        raise NotImplementedError("add_time_cb not implemented by class %s" % str(type(self)))
    
    def cancel_callback(self, callback_id):
        raise NotImplementedError("cancel_callback not implemented by class %s" % str(type(self)))
    
    def time(self) -> int:
        raise NotImplementedError("time not implemented by class %s" % str(type(self)))
    
    def time_precision(self) -> int:
        raise NotImplementedError("time not implemented by class %s" % str(type(self)))
    
    def run_until_event(self):
        raise NotImplementedError("run_until_event not implemented by class %s" % str(type(self)))
        