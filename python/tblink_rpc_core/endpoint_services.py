'''
Created on Jul 7, 2021

@author: mballance
'''

class EndpointServices(object):
    
    def __init__(self):
        pass
    
    def shutdown(self): 
        raise NotImplementedError("shutdown not implemented by class %s" % str(type(self)))
    
    def add_time_cb(self, time) -> int:
        raise NotImplementedError("add_time_cb not implemented by class %s" % str(type(self)))
    
    def cancel_callback(self, callback_id):
        raise NotImplementedError("cancel_callback not implemented by class %s" % str(type(self)))
        