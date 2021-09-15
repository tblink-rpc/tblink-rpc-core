'''
Created on Jul 2, 2021

@author: mballance
'''
import socket
import sys
import os
from unittest.case import TestCase
import asyncio

class TblinkRpcTestcase(TestCase):
    pass

    def setUp(self):
        
        port = int(os.environ["TBLINK_PORT"])
        loop = asyncio.get_event_loop()
        
        self.reader, self.writer = loop.run_until_complete(
            asyncio.open_connection('127.0.0.1', port))
        TestCase.setUp(self)
        
    def tearDown(self):
        TestCase.tearDown(self)
