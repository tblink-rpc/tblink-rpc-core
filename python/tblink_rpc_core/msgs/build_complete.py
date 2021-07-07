'''
Created on Jul 6, 2021

@author: mballance
'''
from tblink_rpc_core.msgs.message_base import MessageBase

class BuildComplete(MessageBase):
    
    def __init__(self):
        super().__init__("tblink.build-complete")
        
        
        