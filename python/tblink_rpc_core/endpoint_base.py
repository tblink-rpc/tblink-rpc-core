'''
Created on Feb 28, 2022

@author: mballance
'''
from tblink_rpc_core.endpoint import Endpoint, EndpointFlags
from tblink_rpc_core.endpoint_listener import EndpointListener

class EndpointBase(Endpoint):
    
    def __init__(self):
        self._flags = EndpointFlags.Empty
        self._eps = None
        self._endpoint_listener_m = {}

    def getFlags(self) -> EndpointFlags:
        return self._flags
        
    def setFlag(self, f) -> EndpointFlags:
        self._flags |= f

    def init(self, ep_services : 'EndpointServices') -> bool:
        self._eps = ep_services
        pass
    
    def is_init(self) -> int:
        """
        Returns True when init has been signaled by this EP,
        and init has been signaled by the peer EP
        """
        raise NotImplementedError("is_init for class %s" % str(type(self)))
    
    def build_complete(self) -> bool:
        """
        Notifies the peer EP that build activities are complete
        """
        raise NotImplementedError("build_complete for class %s" % str(type(self)))
            
    def is_build_complete(self) -> int:
        """
        Returns True when build_complete has been signaled by this EP,
        and build_complete has been signaled by the peer EP
        """
        raise NotImplementedError("is_build_complete for class %s" % str(type(self)))

    def connect_complete(self) -> bool:
        """
        Notifies the peer EP that connect activities are complete
        """
        raise NotImplementedError("connect_complete for class %s" % str(type(self)))
            
    def is_connect_complete(self) -> int:
        """
        Returns True when connect_complete has been signaled by this EP,
        and connect_complete has been signaled by the peer EP
        """
        raise NotImplementedError("is_connect_complete for class %s" % str(type(self)))

    def addListener(self, listener_f) -> EndpointListener:
        self._endpoint_listener_m[listener_f] = listener_f
        return listener_f
    
    def removeListener(self, listener_h):
        if listener_h in self._endpoint_listener_m.keys():
            self._endpoint_listener_m.pop(listener_h)
