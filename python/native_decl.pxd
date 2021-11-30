#****************************************************************************
#* native_decl.pxd
#*
#* Declarations for C++ types
#****************************************************************************
from libcpp.pair cimport pair as cpp_pair
from libcpp.string cimport string as cpp_string
from libcpp.vector cimport vector as cpp_vector

#********************************************************************
#* ILaunchParams
#********************************************************************
cdef extern from "tblink_rpc/ILaunchParams.h" namespace "tblink_rpc_core":
    cdef cppclass ILaunchParams:
        
        void add_arg(const cpp_string &)
        
        void add_param(
            const cpp_string &,
            const cpp_string &)
        
#********************************************************************
#* IInterfaceInst
#********************************************************************
cdef extern from "tblink_rpc/IInterfaceInst.h" namespace "tblink_rpc_core":
    cdef cppclass IInterfaceInst:
        const cpp_string &name()
        
ctypedef IInterfaceInst *IInterfaceInstP
    
#********************************************************************
#* IEndpoint
#********************************************************************
cdef extern from "tblink_rpc/IEndpoint.h" namespace "tblink_rpc_core":
    cdef cppclass IEndpoint:
    
        int build_complete()
        
        int is_build_complete()
        
        int connect_complete()
        
        int is_connect_complete()
    
        const cpp_vector[IInterfaceInstP] &getInterfaceInsts()
        
        const cpp_vector[IInterfaceInstP] &getPeerInterfaceInsts()
        
        pass

ctypedef IEndpoint *IEndpointP

#********************************************************************
#* ILaunchType
#********************************************************************
cdef extern from "tblink_rpc/ILaunchType.h" namespace "tblink_rpc_core":
    cdef cppclass ILaunchType:
        cpp_string name()
        cpp_pair[IEndpointP,cpp_string] launch(ILaunchParams *)
        ILaunchParams *newLaunchParams()

ctypedef ILaunchType *ILaunchTypeP        

#********************************************************************
#* ITbLink
#********************************************************************
cdef extern from "tblink_rpc/ITbLink.h" namespace "tblink_rpc_core":
    cdef cppclass ITbLink:
    
        IEndpoint *getDefaultEP()
    
        const cpp_vector[ILaunchTypeP] &launchTypes()
    
        ILaunchType *findLaunchType(const cpp_string &id)
        

cdef extern from "tblink_rpc/loader.h":
    cdef ITbLink *get_tblink(const char *path)
        
        
        
    