'''
Created on Sep 11, 2021

@author: mballance
'''

class EndpointListener(object):
    
    def init(self, ep : 'Endpoint'):
        pass
    
    def init_complete(self):
        """Called when the initialization phase of the peer is complete"""
        pass
    
    def build_complete(self):
        """Called when the build phase of the peer is complete"""
        pass
    
    def connect_complete(self):
        """Called when the connect phase of the peer is complete"""
        pass
    