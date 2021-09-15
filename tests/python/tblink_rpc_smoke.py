'''
Created on Jul 5, 2021

@author: mballance
'''
from tblink_rpc_testcase import TblinkRpcTestcase
import sys
from tblink_rpc_core.json.json_transport import JsonTransport
import asyncio
from tblink_rpc_core.param_val_map import ParamValMap
from tblink_rpc_core.endpoint import Endpoint

class TblinkRpcSmoke(TblinkRpcTestcase):
    
    def test_smoke(self):
        print("Smoke: ")

        transport = JsonTransport(self.reader, self.writer)
        
        loop = asyncio.get_event_loop()
        
        endpoint = Endpoint(transport)

        # Start the receive loop        
        asyncio.ensure_future(transport.run())

        print("--> build_complete")
        loop.run_until_complete(endpoint.build_complete())
        print("<-- build_complete")
        
        print("--> connect_complete")        
        loop.run_until_complete(endpoint.connect_complete())
        print("<-- connect_complete")        
        
        self.fail("Python assert")