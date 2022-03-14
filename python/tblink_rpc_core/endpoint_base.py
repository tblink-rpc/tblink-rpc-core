'''
Created on Feb 28, 2022

@author: mballance
'''
from tblink_rpc_core.endpoint import Endpoint, EndpointFlags, comm_mode_e, \
    comm_state_e
from tblink_rpc_core.endpoint_event import EndpointEvent
from tblink_rpc_core.endpoint_listener import EndpointListener
from tblink_rpc_core.event_type_e import EventTypeE
from tblink_rpc_core.interface_impl_factory import InterfaceImplFactory
from tblink_rpc_core.interface_type import InterfaceType
from tblink_rpc_core.interface_type_builder import InterfaceTypeBuilder
from tblink_rpc_core.param_val_bool import ParamValBool
from tblink_rpc_core.param_val_int import ParamValInt
from tblink_rpc_core.param_val_map import ParamValMap
from tblink_rpc_core.param_val_str import ParamValStr
from tblink_rpc_core.param_val_vec import ParamValVec


class EndpointBase(Endpoint):
    
    def __init__(self):
        self._flags = EndpointFlags.Empty
        self._comm_mode = comm_mode_e.Automatic
        self._comm_state = comm_state_e.Waiting
        self._eps = None
        self._endpoint_listener_m = {}
        self._local_interface_type_l = []
        self._local_interface_type_m = {}
        self._peer_interface_type_l = []
        self._peer_interface_type_m = {}
        self._local_interface_inst_l = []
        self._local_interface_inst_m = {}
        self._peer_interface_inst_l = []
        self._peer_interface_inst_m = {}

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
            
    def comm_state(self) -> comm_state_e:
        return self._comm_state

    def update_comm_mode(self, m : comm_mode_e, s : comm_state_e):
        self._comm_mode = m 
        self._comm_state = s
        
    def findInterfaceType(self, name) -> InterfaceType:
        if name in self._local_interface_type_m.keys():
            return self._local_interface_type_m[name]
        else:
            return None

    def newInterfaceTypeBuilder(self, name) -> InterfaceTypeBuilder:
        return InterfaceTypeBuilder(name)
    
    def defineInterfaceType(self, 
                            iftype_b : InterfaceTypeBuilder,
                            impl_f : InterfaceImplFactory,
                            impl_mirror_f : InterfaceImplFactory) -> InterfaceType:
        t = InterfaceType(iftype_b.name)
        t._method_m.update(iftype_b.method_m)
        t._methods.extend(iftype_b.methods)
        
        self._local_interface_type_l.append(t)
        self._local_interface_type_m[iftype_b.name] = t
        
        return t

    def getInterfaceTypes(self):
        return self._local_interface_type_l
    
    def getPeerInterfaceTypes(self):
        return self._peer_interface_type_l
    
    def getInterfaceInsts(self):
        return self._local_interface_inst_l
    
    def getPeerInterfaceInsts(self):
        return self._peer_interface_inst_l 

    def shutdown(self):
        """
        Sends a shutdown-request message to the peer EP
        """
        print("Sending shutdown event")
        self._sendEvent(EndpointEvent(EventTypeE.Terminate))


    def _sendEvent(self, ev):
        items = list(self._endpoint_listener_m.items())
        
        for it in items:
            it[1](ev)
            
    def mkValBool(self, v) -> ParamValBool:
        return ParamValBool(v)
    
    def mkValIntS(self, val, width) -> ParamValInt:
        return ParamValInt(val)
    
    def mkValIntU(self, val, width) -> ParamValInt:
        return ParamValInt(val)
    
    def mkValMap(self) -> ParamValMap:
        return ParamValMap()
    
    def mkValVec(self) -> ParamValVec:
        return ParamValVec()
    
    def mkValStr(self, v) -> ParamValStr:
        return ParamValStr(v)
