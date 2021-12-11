#****************************************************************************
#* native_decl.pxd
#*
#* Declarations for C++ types
#****************************************************************************
from libcpp.pair cimport pair as cpp_pair
from libcpp.set cimport set as cpp_set
from libcpp.string cimport string as cpp_string
from libcpp.vector cimport vector as cpp_vector
from libc.stdint cimport intptr_t
from libc.stdint cimport uint32_t
from libc.stdint cimport uint64_t
from libc.stdint cimport int64_t
from libcpp cimport bool
cimport cpython.ref as cpy_ref

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
#* IParamVal
#********************************************************************
cdef extern from "tblink_rpc/IParamVal.h" namespace "tblink_rpc_core":
    cdef enum TypeE:
        Bool "tblink_rpc_core::IParamVal::Bool",
        Int "tblink_rpc_core::IParamVal::Int",
        Map "tblink_rpc_core::IParamVal::Map",
        Str "tblink_rpc_core::IParamVal::Str",
        Vec "tblink_rpc_core::IParamVal::Vec"
        
    cdef cppclass IParamVal:
        TypeE type()
        pass
ctypedef IParamVal *IParamValP
    
#********************************************************************
#* IParamValBool
#********************************************************************
cdef extern from "tblink_rpc/IParamValBool.h" namespace "tblink_rpc_core":
    cdef cppclass IParamValBool(IParamVal):
        bool val()
        IParamValBool *clone()
ctypedef IParamValBool *IParamValBoolP
        
#********************************************************************
#* IParamValInt
#********************************************************************
cdef extern from "tblink_rpc/IParamValInt.h" namespace "tblink_rpc_core":
    cdef cppclass IParamValInt(IParamVal):
        uint64_t val_u()
        int64_t val_s()
        IParamValInt *clone()
ctypedef IParamValInt *IParamValIntP
        
#********************************************************************
#* IParamValMap
#********************************************************************
cdef extern from "tblink_rpc/IParamValMap.h" namespace "tblink_rpc_core":
    cdef cppclass IParamValMap(IParamVal):
        const cpp_set[cpp_string] &keys()
        bool hasKey(const cpp_string &)
        IParamVal *getVal(const cpp_string &)
        void setVal(const cpp_string &, IParamVal *)
        IParamValMap *clone()
ctypedef IParamValMap *IParamValMapP

#********************************************************************
#* IParamValStr
#********************************************************************
cdef extern from "tblink_rpc/IParamValStr.h" namespace "tblink_rpc_core":
    cdef cppclass IParamValStr(IParamVal):
        const cpp_string &val()
        IParamValStr *clone()
ctypedef IParamValStr *IParamValStrP
        
#********************************************************************
#* IParamValVec
#********************************************************************
cdef extern from "tblink_rpc/IParamValVec.h" namespace "tblink_rpc_core":
    cdef cppclass IParamValVec(IParamVal):
        uint32_t size()
        IParamVal *at(uint32_t)
        void push_back(IParamVal *)
        IParamValVec *clone()
ctypedef IParamValVec *IParamValVecP

#********************************************************************
#* IMethod
#********************************************************************
cdef extern from "tblink_rpc/IMethodType.h" namespace "tblink_rpc_core":
    cdef cppclass IMethodType:
        pass
ctypedef IMethodType *IMethodTypeP

#********************************************************************
#* IInterfaceType
#********************************************************************
cdef extern from "tblink_rpc/IInterfaceType.h" namespace "tblink_rpc_core":
    cdef cppclass IInterfaceType:
        const cpp_string &name()
        const cpp_vector[IMethodTypeP] &methods()
        IMethodType *findMethod(const cpp_string &)
ctypedef IInterfaceType *IInterfaceTypeP
                
#********************************************************************
#* IInterfaceInst
#********************************************************************
cdef extern from "tblink_rpc/IInterfaceInst.h" namespace "tblink_rpc_core":
    cdef cppclass invoke_req_f:
        pass
    cdef cppclass invoke_rsp_f:
        pass
    cdef cppclass IInterfaceInst:
        const cpp_string &name()
        IInterfaceType *type()
        bool is_mirror()
        int invoke_nb(
            IMethodType *, IParamValVec *, const invoke_rsp_f &)
ctypedef IInterfaceInst *IInterfaceInstP

#********************************************************************
#* IType
#********************************************************************
cdef extern from "tblink_rpc/IType.h" namespace "tblink_rpc_core":
    cdef cppclass IType:
        pass
    
cdef extern from "tblink_rpc/ITypeInt.h" namespace "tblink_rpc_core":
    cdef cppclass ITypeInt(IType):
        pass
    
cdef extern from "tblink_rpc/ITypeMap.h" namespace "tblink_rpc_core":
    cdef cppclass ITypeMap(IType):
        pass
    
cdef extern from "tblink_rpc/IType.h" namespace "tblink_rpc_core":
    cdef cppclass ITypeStr(IType):
        pass
    
cdef extern from "tblink_rpc/ITypeVec.h" namespace "tblink_rpc_core":
    cdef cppclass ITypeVec(IType):
        pass
    
#********************************************************************
#* IMethodTypeBuilder
#********************************************************************
cdef extern from "tblink_rpc/IMethodTypeBuilder.h" namespace "tblink_rpc_core":
    cdef cppclass IMethodTypeBuilder:
        void add_param(const cpp_string &, IType *)

#********************************************************************
#* IInterfaceTypeBuilder
#********************************************************************
cdef extern from "tblink_rpc/IInterfaceTypeBuilder.h" namespace "tblink_rpc_core":
    cdef cppclass IInterfaceTypeBuilder:
        IMethodTypeBuilder *newMethodTypeBuilder(
            const cpp_string &,
            intptr_t,
            IType *,
            bool,
            bool)
        
        IMethodType *add_method(IMethodTypeBuilder *)
        
        IType *mkTypeBool()
        
        ITypeInt *mkTypeInt(bool, int)
        
        ITypeMap *mkTypeMap(IType *, IType *)
        
        IType *mkTypeStr()
        
        ITypeVec *mkTypeVec(IType *)
        
ctypedef IInterfaceTypeBuilder *IInterfaceTypeBuilderP

#********************************************************************
#* IEndpointEvent
#********************************************************************
cdef extern from "tblink_rpc/IEndpointEvent.h" namespace "tblink_rpc_core":
    cdef enum EventTypeE:
        Unknown "tblink_rpc_core::IEndpointEvent::Unknown"
        
    cdef cppclass IEndpointEvent:
        EventTypeE kind() const
        
#********************************************************************
#* IEndpointListener
#********************************************************************
cdef extern from "tblink_rpc/IEndpointListener.h" namespace "tblink_rpc_core":
    cdef cppclass endpoint_ev_f:
        pass
    
    cdef cppclass IEndpointListener:
        void event(const IEndpointEvent *)
        
#********************************************************************
#* IEndpointServices
#********************************************************************
cdef extern from "tblink_rpc/IEndpointServices.h" namespace "tblink_rpc_core":
    cdef cppclass IEndpointServices:
        void init(IEndpoint *)

#********************************************************************
#* IEndpoint
#********************************************************************
cdef extern from "tblink_rpc/IEndpoint.h" namespace "tblink_rpc_core":
    cdef cppclass time_cb_f:
        pass
    cdef cppclass IEndpoint:
    
        int init(IEndpointServices *, IEndpointListener *)
        
        int is_init()
    
        int build_complete()
        
        int is_build_complete()
        
        int connect_complete()
        
        int is_connect_complete()
        
        IEndpointListener *addListener(const endpoint_ev_f &)
        
        void removeListener(IEndpointListener *)
        
        intptr_t add_time_callback(uint64_t, const time_cb_f &)
        
        void cancel_callback(intptr_t)
        
        IInterfaceType *findInterfaceType(const cpp_string &)
        
        IInterfaceTypeBuilder *newInterfaceTypeBuilder(
            const cpp_string &)
        
        IInterfaceType *defineInterfaceType(
            IInterfaceTypeBuilder *)
        
        IInterfaceInst *defineInterfaceInst(
            IInterfaceType *,
            const cpp_string &,
            bool,
            const invoke_req_f &
            )

        const cpp_vector[IInterfaceTypeP] &getInterfaceTypes()
        
        const cpp_vector[IInterfaceTypeP] &getPeerInterfaceTypes()
        
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
        cpp_pair[IEndpointP,cpp_string] launch(ILaunchParams *, IEndpointServices *)
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
        
        
        
    