'''
Created on Jan 17, 2022

@author: mballance
'''
from tblink_rpc_core.tblink_event import TbLinkEvent

class TbLinkListener(object):
    
    def event(self, ev : TbLinkEvent):
        pass