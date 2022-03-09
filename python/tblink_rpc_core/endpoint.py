'''
Created on Jul 2, 2021

@author: mballance
'''
from enum import IntEnum, Enum, auto, Flag, IntFlag
from typing import List, Callable

from tblink_rpc_core.interface_inst import InterfaceInst
from tblink_rpc_core.interface_type import InterfaceType
from tblink_rpc_core.interface_type_builder import InterfaceTypeBuilder
from tblink_rpc_core.param_val_bool import ParamValBool
from tblink_rpc_core.param_val_int import ParamValInt
from tblink_rpc_core.param_val_map import ParamValMap
from tblink_rpc_core.param_val_str import ParamValStr
from tblink_rpc_core.param_val_vec import ParamValVec
from tblink_rpc_core.interface_impl_factory import InterfaceImplFactory
from tblink_rpc_core.endpoint_listener import EndpointListener


class TimeUnit(IntEnum):
    ps = -12
    ns = -9
    us = -6
    ms = -3
    s = 1
    
class comm_state_e(Enum):
    Waiting = auto()
    Released = auto()
    
class comm_mode_e(Enum):
    Automatic = auto()
    Explicit = auto()
    
class EndpointFlags(IntFlag):
    Empty = 0
    Claimed = (1 << 0)
    LoopbackPri = (1 << 1)
    LoopbackSec = (1 << 2)

class Endpoint(object):
    DEBUG_EN = False
    
    def getFlags(self) -> EndpointFlags:
        raise NotImplementedError("getFlags for class %s" % str(type(self)))
        
    def setFlag(self, f) -> EndpointFlags:
        raise NotImplementedError("setFlag for class %s" % str(type(self)))
    
    def init(self, ep_services : 'EndpointServices') -> bool:
        raise NotImplementedError("init for class %s" % str(type(self)))
    
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

    def connect_complete(self):
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
        raise NotImplementedError("addListener not implemented for class %s" % str(type(self)))
    
    def removeListener(self, listener_h):
        raise NotImplementedError("removeListener not implemented for class %s" % str(type(self)))
    
    def comm_state(self) -> comm_state_e:
        raise NotImplementedError("comm_state not implemented for class %s" % str(type(self)))

    def update_comm_mode(self, m : comm_mode_e, s : comm_state_e):
        raise NotImplementedError("comm_state not implemented for class %s" % str(type(self)))
        
    
    def findInterfaceType(self, name) -> InterfaceType:
        """
        Finds the interface type named ''name'' and returns it
        if it exists. None is returned if ''name'' does not exist.
        """
        raise NotImplementedError("findInterfaceType for class %s" % str(type(self)))
    
    def newInterfaceTypeBuilder(self, name) -> InterfaceTypeBuilder:
        """
        Returns a new interface type builder for iftype ''name''
        """
        raise NotImplementedError("newInterfaceTypeBuilder for class %s" % str(type(self)))
    
    def defineInterfaceType(self, 
                            iftype_b : InterfaceTypeBuilder,
                            impl_f : InterfaceImplFactory,
                            impl_mirror_f : InterfaceImplFactory) -> InterfaceType:
        """
        Defines a new interface type from the information collected
        in the iftype builder
        """
        raise NotImplementedError("defineInterfaceType for class %s" % str(type(self)))
    
    def defineInterfaceInst(self, 
                            iftype : InterfaceType,
                            inst_name : str,
                            is_mirror : bool,
                            req_f : Callable) -> InterfaceInst:
        """Defines a new interface instance"""
        raise NotImplementedError("defineInterface for class %s" % str(type(self)))
            
    def process_one_message(self):
        """
        Blocks until a single message is processed. This must be used 
        *very* sparingly, but may be done to implement operations 
        that are blocking and non-async from a user perspective.
        In many cases, the Endpoint implementation simply delegates 
        to the underlying transport.
        """
        raise NotImplementedError("process_one_message for class %s" % str(type(self)))
    
    async def process_one_message_a(self):
        """
        Blocks until a single message is processed. This must be used 
        *very* sparingly, but may be done to implement operations 
        that are blocking and non-async from a user perspective.
        In many cases, the Endpoint implementation simply delegates 
        to the underlying transport.
        """
        raise NotImplementedError("process_one_message for class %s" % str(type(self)))
    
    def getInterfaceTypes(self):
        raise NotImplementedError("getInterfaceTypes not implemented for %s" % str(type(self)))
    
    def getPeerInterfaceTypes(self):
        raise NotImplementedError("getPeerInterfaceTypes not implemented for %s" % str(type(self)))
    
    def getInterfaceInsts(self):
        raise NotImplementedError("getInterfaceInsts not implemented for %s" % str(type(self)))
    
    def getPeerInterfaceInsts(self):
        raise NotImplementedError("getPeerInterfaceInsts not implemented for %s" % str(type(self)))
            
    def args(self) -> List[str]:
        """Returns command-line arguments from the peer. Valid after 'init'"""
        raise NotImplementedError("args for class %s" % str(type(self)))
    
    def time(self) -> int:
        """Returns the last known time of the peer EP"""
        raise NotImplementedError("time for class %s" % str(type(self)))
    
    def time_precision(self) -> TimeUnit:
        """
        Returns the time precision of the peer EP. A valid result 
        is returned after is_init_complete returns True
        """
        raise NotImplementedError("time_precision for class %s" % str(type(self)))

    def shutdown(self):
        """
        Sends a shutdown-request message to the peer EP
        """
        raise NotImplementedError("shutdown for class %s" % str(type(self)))
            
    def add_time_callback(self, time, cb_f) -> int:
        """
        Adds a time callback to be satisfied by the peer EP
        """
        raise NotImplementedError("add_time_callback for class %s" % str(type(self)))
    
    def cancel_callback(self, cb_id):
        """
        Removes a previously-registered callback
        """
        raise NotImplementedError("cancel_callback for class %s" % str(type(self)))
    
    def mkValBool(self, v) -> ParamValBool:
        """Creates a boolean param object"""
        raise NotImplementedError("mkValBool for class %s" % str(type(self)))
    
    def mkValIntS(self, val, width) -> ParamValInt:
        raise NotImplementedError("mkValIntS for class %s" % str(type(self)))
    
    def mkValIntU(self, val, width) -> ParamValInt:
        raise NotImplementedError("mkValIntU for class %s" % str(type(self)))
    
    def mkValMap(self) -> ParamValMap:
        raise NotImplementedError("mkValMap for class %s" % str(type(self)))
    
    def mkValVec(self) -> ParamValVec:
        raise NotImplementedError("mkValVec for class %s" % str(type(self)))
    
    def mkValStr(self, v) -> ParamValStr:
        raise NotImplementedError("mkValStr for class %s" % str(type(self)))

