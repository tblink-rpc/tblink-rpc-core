'''
Created on Dec 2, 2021

@author: mballance
'''
from tblink_rpc_core.param_val import ParamVal
from tblink_rpc_core.endpoint import Endpoint

class InvokeRspWaiter(object):
    
    def init(self, ep : Endpoint):
        raise Exception("InvokeRspWaiter.init unimplemented for class %s" % str(type(self)))
    
    async def wait_b(self) -> ParamVal:
        raise Exception("InvokeRspWaiter.wait_b unimplemented for class %s" % str(type(self)))
    
    def wait(self) -> ParamVal:
        raise Exception("InvokeRspWaiter.wait unimplemented for class %s" % str(type(self)))
    
    def invoke_rsp(self, retval : ParamVal):
        raise Exception("InvokeRspWaiter.invoke_rsp unimplemented for class %s" % str(type(self)))
        