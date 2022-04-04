#****************************************************************************
#* native.pyx
#*
#* Python interface to the native implementation of TbLink
#****************************************************************************
from libc.stdint cimport intptr_t
from libcpp.cast cimport dynamic_cast
from libcpp.cast cimport static_cast
from libcpp.pair cimport pair as cpp_pair
from libcpp.map cimport map as cpp_map
from libcpp.string cimport string as cpp_string
from libcpp.vector cimport vector as cpp_vector
from cython.operator cimport dereference as deref, preincrement as inc
import asyncio
import sys
from tblink_rpc_core.endpoint import comm_state_e, comm_mode_e, EndpointFlags,\
    TimeUnit
from tblink_rpc_core.event_type_e import EventTypeE
from tblink_rpc_core.tblink_event import TbLinkEventKind
from tblink_rpc_core.interface_impl_factory import InterfaceImplFactory
import traceback
cimport cpython.ref as cpy_ref

import tblink_rpc_core
cimport tblink_rpc_core.native_decl as native_decl

ctypedef native_decl.IEndpoint *IEndpointP
ctypedef native_decl.ILaunchType *ILaunchTypeP        

#********************************************************************
#*  LaunchParams
#********************************************************************
cdef class LaunchParams(object):
    cdef native_decl.ILaunchParams *_hndl
    
    def __init__(self):
        pass
    
    def __dealloc(self):
        pass

    cpdef add_arg(self, arg : str):
        self._hndl.add_arg(arg.encode())
        
    cpdef add_param(self, key : str, val : str):
        self._hndl.add_param(
            key.encode(),
            val.encode())
        
#********************************************************************
#* ParamVal
#********************************************************************
cdef class ParamVal(object):
    cdef native_decl.IParamVal *_hndl
    @staticmethod
    cdef _mk(native_decl.IParamVal *hndl):
        ret = None
        if hndl == NULL:
            return None
        elif hndl.type() == native_decl.TypeE.Bool:
            ret = ParamValBool()
        elif hndl.type() == native_decl.TypeE.Int:
            ret = ParamValInt()
        elif hndl.type() == native_decl.TypeE.Map:
            ret = ParamValMap()
        elif hndl.type() == native_decl.TypeE.Str:
            ret = ParamValStr()
        elif hndl.type() == native_decl.TypeE.Vec:
            ret = ParamValVec()
        else:
            raise Exception("Unknown ParamVal type")
        ret._hndl = hndl
        return ret
    
    cdef native_decl.IParamValBool *asBool(self):
        return dynamic_cast[native_decl.IParamValBoolP](self._hndl)
    cdef native_decl.IParamValInt *asInt(self):
        return dynamic_cast[native_decl.IParamValIntP](self._hndl)
    cdef native_decl.IParamValStr *asStr(self):
        return dynamic_cast[native_decl.IParamValStrP](self._hndl)
    
#********************************************************************
#* ParamValBool
#********************************************************************
cdef class ParamValBool(ParamVal):
    cpdef val(self):
        return self.asBool().val()

#********************************************************************
#* ParamValInt
#********************************************************************
cdef class ParamValInt(ParamVal):
    cpdef val_s(self):
        return self.asInt().val_s()
    cpdef val_u(self):
        return self.asInt().val_u()

#********************************************************************
#* ParamValMap
#********************************************************************
cdef class ParamValMap(ParamVal):
    pass

#********************************************************************
#* ParamValStr
#********************************************************************
cdef class ParamValStr(ParamVal):
    cpdef val(self):
        return self.asStr().val()

#********************************************************************
#* ParamValVec
#********************************************************************
cdef class ParamValVec(ParamVal):

    @staticmethod
    cdef _mk(native_decl.IParamValVec *hndl):
        ret = ParamValVec()
        ret._hndl = hndl
        return ret
    
    cpdef size(self):
        return self.asVec().size()
    
    cpdef at(self, idx):
        return ParamVal._mk(self.asVec().at(idx))
    
    cpdef push_back(self, ParamVal val):
        self.asVec().push_back(val._hndl)
    
    cdef native_decl.IParamValVec *asVec(self):
        return dynamic_cast[native_decl.IParamValVecP](self._hndl)

#********************************************************************
#* interface_inst_rsp_f()
#*
#* Callback method for invocation responses
#********************************************************************
cdef public void interface_inst_rsp_f(obj, native_decl.IParamVal *params) with gil:
    obj(ParamVal._mk(params))
    
#********************************************************************
#* invoke_rsp_closure
#********************************************************************
cdef extern native_decl.invoke_rsp_f invoke_rsp_closure(cpy_ref.PyObject *)

#********************************************************************
#* InterfaceImpl
#********************************************************************
cdef class InterfaceImpl(object):
    cdef native_decl.IInterfaceImpl *_hndl

#********************************************************************
#* InterfaceInst
#********************************************************************
cdef class InterfaceInst(object):

    cdef native_decl.IInterfaceInst *_hndl
    
    def __init__(self):
        pass
    
    cpdef name(self):
        return self._hndl.name().decode()
    
    cpdef type(self):
        print("InterfaceInst.type 0x%08x" % <intptr_t>(self._hndl))
        return InterfaceType._mk(self._hndl.type())
    
    cpdef is_mirror(self):
        return self._hndl.is_mirror()
    
    cpdef invoke(
        self,
        MethodType  method,
        ParamValVec params,
        completion_f):
        self._hndl.invoke(
            method._hndl,
            params.asVec(),
            invoke_rsp_closure(<cpy_ref.PyObject *>(completion_f)))
        
    cpdef invoke_rsp(
        self,
        intptr_t    call_id,
        ParamVal    ret):
        cdef native_decl.IParamVal *ret_p = NULL
        
        if ret is not None:
            ret_p = ret._hndl
            
        self._hndl.invoke_rsp(
            call_id,
            ret_p)
        
    cpdef mkValBool(self, val):
        return ParamValBool._mk(self._hndl.mkValBool(val))
    
    cpdef mkValIntU(self, val, width):
        return ParamValInt._mk(self._hndl.mkValIntU(val, width))
    
    cpdef mkValIntS(self, val, width):
        return ParamValInt._mk(self._hndl.mkValIntS(val, width))
    
    cpdef mkValMap(self):
        return ParamValMap._mk(self._hndl.mkValMap())
    
    cpdef mkValStr(self, val):
        return ParamValStr._mk(self._hndl.mkValStr(val.encode()))
    
    cpdef mkValVec(self):
        return ParamValVec._mk(self._hndl.mkValVec())

    @staticmethod
    cdef _mk(native_decl.IInterfaceInst *hndl):
        ret = InterfaceInst()
        ret._hndl = hndl
        return ret
    
#********************************************************************
#* InterfaceType
#********************************************************************
cdef class InterfaceType(object):
    cdef native_decl.IInterfaceType *_hndl
    
    cpdef name(self):
        print("InterfaceType.name 0x%08x" % <intptr_t>(self._hndl))
        return self._hndl.name().decode()
    
    cpdef methods(self):
        ret = []
        for i in range(self._hndl.methods().size()):
            ret.append(MethodType._mk(self._hndl.methods().at(i)))
        return ret
    
    cpdef findMethod(self, name):
        cdef native_decl.IMethodType *m = self._hndl.findMethod(name.encode())
        if m:
            return MethodType._mk(m)
        else:
            return None
    
    @staticmethod
    cdef _mk(native_decl.IInterfaceType *hndl):
        ret = InterfaceType()
        ret._hndl = hndl
        return ret
    
#********************************************************************
#* InterfaceImplFactory
#********************************************************************
#cdef class InterfaceImplFactory(object):
#    cdef native_decl.IInterfaceImplFactory *_hndl
#    
#    cpdef createImpl(self):
#        pass

#********************************************************************
#* Type
#********************************************************************
cdef class Type(object):
    cdef native_decl.IType *_hndl

    _kind_m = {
        native_decl.TypeKindE.TypeKindBool : tblink_rpc_core.type_e.TypeE.Bool,
        native_decl.TypeKindE.TypeKindInt  : tblink_rpc_core.type_e.TypeE.Int,
        native_decl.TypeKindE.TypeKindStr  : tblink_rpc_core.type_e.TypeE.Str,
        native_decl.TypeKindE.TypeKindVec  : tblink_rpc_core.type_e.TypeE.Vec,
        native_decl.TypeKindE.TypeKindMap  : tblink_rpc_core.type_e.TypeE.Map
        }
        
    cpdef kind(self):
        return Type._kind_m[self._hndl.kind()]
    
    @staticmethod
    cdef _mk(native_decl.IType *hndl):
        if hndl != NULL:
            ret = Type()
            ret._hndl = hndl
            return ret
        else:
            return None
    
cdef class TypeInt(Type):
    pass
    
#********************************************************************
#* MethodTypeBuilder
#********************************************************************
cdef class MethodTypeBuilder(object):
    cdef native_decl.IMethodTypeBuilder *_hndl
    
    cpdef add_param(self, name, Type t):
        return self._hndl.add_param(name.encode(), t._hndl)

    @staticmethod
    cdef _mk(native_decl.IMethodTypeBuilder *hndl):
        ret = MethodTypeBuilder()
        ret._hndl = hndl
        return ret
    
#********************************************************************
#* ParamDecl
#********************************************************************
cdef class ParamDecl(object):
    cdef native_decl.IParamDecl *_hndl
    
    cpdef name(self):
        return self._hndl.name().encode()
    
    cpdef type(self):
        return Type._mk(self._hndl.type())
    
    @staticmethod
    cdef _mk(native_decl.IParamDecl *hndl):
        ret = ParamDecl()
        ret._hndl = hndl
        return ret
    
#********************************************************************
#* MethodType
#********************************************************************
cdef class MethodType(object):
    cdef native_decl.IMethodType *_hndl
    
    cpdef name(self):
        return self._hndl.name().decode()
    
    cpdef id(self):
        return self._hndl.id()
    
    cpdef rtype(self):
        return Type._mk(self._hndl.rtype())
    
    cpdef is_export(self):
        return self._hndl.is_export()
    
    cpdef is_blocking(self):
        return self._hndl.is_blocking()
    
    
    cpdef params(self):
        ret = []
        for i in range(self._hndl.params().size()):
            ret.append(ParamDecl._mk(self._hndl.params().at(i)))
        return ret
    
    @staticmethod
    cdef _mk(native_decl.IMethodType *hndl):
        ret = MethodType()
        ret._hndl = hndl
        return ret
    
#********************************************************************
#* InterfaceTypeBuilder
#********************************************************************
cdef class InterfaceTypeBuilder(object):
    cdef native_decl.IInterfaceTypeBuilder *_hndl
    
    cpdef newMethodTypeBuilder(
        self, 
        name, 
        id,
        Type rtype,
        is_export,
        is_blocking):
        cdef native_decl.IType *rtype_h = NULL
        
        print("newMethodTypeBuilder: %s is_blocking=%s" % (name, str(is_blocking)))

        if rtype is not None:
            rtype_h = rtype._hndl
            
        return MethodTypeBuilder._mk(
            self._hndl.newMethodTypeBuilder(
                name.encode(), id, rtype_h,
                is_export, is_blocking))
        
    cpdef add_method(self, MethodTypeBuilder mtb):
        ret = MethodType._mk(self._hndl.add_method(mtb._hndl))
        print("add_method: %s is_blocking=%s" % (ret.name(), ret.is_blocking()))
        return ret
        
    cpdef mkTypeBool(self):
        return Type._mk(self._hndl.mkTypeBool())
    
    cpdef mkTypeInt(self, is_signed, width):
        return Type._mk(self._hndl.mkTypeInt(is_signed, width))
    
    cpdef mkTypeMap(self, Type ktype, Type etype):
        return Type._mk(self._hndl.mkTypeMap(
            ktype._hndl, etype._hndl))
        
    cpdef mkTypeStr(self):
        return Type._mk(self._hndl.mkTypeStr())
        
    cpdef mkTypeVec(self, Type etype):
        return Type._mk(self._hndl.mkTypeVec(etype._hndl))

#********************************************************************
#* invoke_req_closure
#********************************************************************
cdef extern native_decl.invoke_req_f invoke_req_closure(cpy_ref.PyObject *)

#********************************************************************
#* time_callback_closure
#********************************************************************
cdef extern native_decl.time_cb_f time_callback_closure(cpy_ref.PyObject *)

#********************************************************************
#* endpoint_ev_closure
#********************************************************************
cdef extern native_decl.endpoint_ev_f endpoint_ev_closure(cpy_ref.PyObject *)
        
#********************************************************************
#* EndpointEvent
#********************************************************************
cdef class EndpointEvent(object):
    cdef const native_decl.IEndpointEvent     *_hndl
    
    _kind_m = { 
        native_decl.EventTypeE.Unknown : EventTypeE.Unknown,
        native_decl.EventTypeE.OutInvokeReqB  : EventTypeE.OutInvokeReqB,
        native_decl.EventTypeE.InInvokeRspB   : EventTypeE.InInvokeRspB,
        native_decl.EventTypeE.InInvokeReqB   : EventTypeE.InInvokeReqB,
        native_decl.EventTypeE.OutInvokeRspB  : EventTypeE.OutInvokeRspB,
        native_decl.EventTypeE.OutInvokeReqNB : EventTypeE.OutInvokeReqNB,
        native_decl.EventTypeE.InInvokeRspNB  : EventTypeE.InInvokeRspNB,
        native_decl.EventTypeE.InInvokeReqNB  : EventTypeE.InInvokeReqNB,
        native_decl.EventTypeE.OutInvokeRspNB : EventTypeE.OutInvokeRspNB,
        native_decl.EventTypeE.Terminate      : EventTypeE.Terminate
        }
    
    cpdef kind(self):
        return EndpointEvent._kind_m[self._hndl.kind()]

    @staticmethod
    cdef _mk(const native_decl.IEndpointEvent *hndl):
        ret = EndpointEvent()
        ret._hndl = hndl 
        return ret

#********************************************************************
#* EndpointListener
#********************************************************************
cdef class EndpointListener(object):
    cdef native_decl.IEndpointListener   *_hndl
    cdef object                          ev_f
    
    def __init__(self, ev_f):
        self.ev_f = ev_f
        
#    def __dealloc__(self):
#        del self._hndl
        
    @staticmethod
    cdef _mk(ev_f, native_decl.IEndpointListener *hndl):
        ret = EndpointListener(ev_f)
        ret._hndl = hndl
        return ret
        
#********************************************************************
#* EndpointServices
#********************************************************************
cdef class EndpointServices(object):
    cdef native_decl.IEndpointServices *_hndl
    
# _EndpointFlags_n2p_m = {
#     native_decl.IEndpointFlags.Claimed : EndpointFlags.Claimed,
#     native_decl.IEndpointFlags.LoopbackPri : EndpointFlags.LoopbackPri,
#     native_decl.IEndpointFlags.LoopbackSec : EndpointFlags.LoopbackSec
#     }
#
# _EndpointFlags_p2n_m = {
#     EndpointFlags.Claimed : native_decl.IEndpointFlags.Claimed,
#     EndpointFlags.LoopbackPri : native_decl.IEndpointFlags.LoopbackPri,
#     EndpointFlags.LoopbackSec : native_decl.IEndpointFlags.LoopbackSec
#     }
    
#********************************************************************
#* Endpoint
#********************************************************************
cdef class Endpoint(object):
    cdef native_decl.IEndpoint *_hndl
    cdef object                 ev_listener_m
    
    _req_l = []
    
    def __init__(self):
#        self.cb_m = {}
        self.ev_listener_m = {}
        pass
    
    cpdef getFlags(self):
        flags = EndpointFlags.Empty
        cdef int flags_n = <int>self._hndl.getFlags()
        
        if flags_n & <int>native_decl.IEndpointFlags.Claimed:
            flags |= EndpointFlags.Claimed
        if flags_n & <int>native_decl.IEndpointFlags.LoopbackPri:
            flags |= EndpointFlags.LoopbackPri
        if flags_n & <int>native_decl.IEndpointFlags.LoopbackSec:
            flags |= EndpointFlags.LoopbackSec

        return flags
    
    cpdef setFlag(self, f):
        cdef int flags_n = 0
#        cdef native_decl.IEndpointFlags flags_n = native_decl.IEndpointFlags.Empty
        if f & EndpointFlags.Claimed:
            flags_n |= <int>native_decl.IEndpointFlags.Claimed
        if f & EndpointFlags.LoopbackPri:
            flags_n |= <int>native_decl.IEndpointFlags.LoopbackPri
        if f & EndpointFlags.LoopbackSec:
            flags_n |= <int>native_decl.IEndpointFlags.LoopbackSec

        self._hndl.setFlag(<native_decl.IEndpointFlags>flags_n)
    
    cpdef init(self, EndpointServices services):
        cdef native_decl.IEndpointServices *services_h = NULL
        if services is not None:
            services_h = services._hndl
        
        return self._hndl.init(services_h)
    
    cpdef is_init(self):
        return self._hndl.is_init()
    
    cpdef build_complete(self):
        return self._hndl.build_complete()
    
    cpdef is_build_complete(self):
        return self._hndl.is_build_complete()
    
    cpdef connect_complete(self):
        return self._hndl.connect_complete()
    
    cpdef is_connect_complete(self):
        return self._hndl.is_connect_complete()
    
    _comm_mode_m = {
        comm_mode_e.Automatic : native_decl.Automatic,
        comm_mode_e.Explicit  : native_decl.Explicit
    }
    
    _comm_state_m = {
        comm_state_e.Waiting  : native_decl.Waiting,
        comm_state_e.Released : native_decl.Released
    }
    
    cpdef update_comm_mode(self, comm_mode, comm_state):
        self._hndl.update_comm_mode(
            Endpoint._comm_mode_m[comm_mode],
            Endpoint._comm_state_m[comm_state])
    
    cpdef addListener(self, ev_f):
        cdef native_decl.IEndpointListener *l
    
        l = self._hndl.addListener(
            endpoint_ev_closure(<cpy_ref.PyObject *>(ev_f)))
        ep_l = EndpointListener(ev_f)
        ep_l._hndl = l
#        ret = EndpointListener._mk(ev_f, l)
        self.ev_listener_m[ev_f] = ep_l
        
        return ep_l
    
    cpdef removeListener(self, EndpointListener l):
        self._hndl.removeListener(l._hndl)
        
    cpdef shutdown(self):
        self._hndl.shutdown()
        
    cpdef args(self):
        cdef cpp_vector[cpp_string] _args = self._hndl.args()
        ret = []
        for i in range(_args.size()):
            ret.append(_args.at(i).decode())
        return ret
    
    cpdef time(self):
        return self._hndl.time()
    
    cpdef time_precision(self):
        return {
            0   : TimeUnit.ns, # Return a sensible default pre-initialization
            -12 : TimeUnit.ps,
            -9  : TimeUnit.ns,
            -6  : TimeUnit.us,
            -3  : TimeUnit.ms,
            1   : TimeUnit.s}[self._hndl.time_precision()]
    
    cpdef add_time_callback(self, time, cb):
        cdef intptr_t ret;

        print("add_time_callback")        
        ret = self._hndl.add_time_callback(
            time,
            time_callback_closure(<cpy_ref.PyObject *>(cb)))
        
#        self.cb_m[ret] = cb
        
        return ret
    
    cpdef cancel_callback(self, cb_id):
        self._hndl.cancel_callback(cb_id)
        pass
    
    cpdef findInterfaceType(self, name):
        cdef native_decl.IInterfaceType *iftype = self._hndl.findInterfaceType(name.encode())
        if iftype:
            return InterfaceType._mk(iftype)
        else:
            return None
    
    cpdef newInterfaceTypeBuilder(self, name):
        ret = InterfaceTypeBuilder()
        ret._hndl = self._hndl.newInterfaceTypeBuilder(
            name.encode())
        return ret
    
    cpdef defineInterfaceType(self, 
                              InterfaceTypeBuilder iftype_b,
                              impl_f : InterfaceImplFactory,
                              impl_mirror_f : InterfaceImplFactory):
        cdef native_decl.InterfaceImplFactoryProxy *impl_f_h = NULL
        cdef native_decl.InterfaceImplFactoryProxy *impl_mirror_f_h = NULL
        
        if impl_f is not None:
            impl_f_h = new native_decl.InterfaceImplFactoryProxy(
                <cpy_ref.PyObject *>(impl_f))
        
        if impl_mirror_f is not None:
            impl_mirror_f_h = new native_decl.InterfaceImplFactoryProxy(
                <cpy_ref.PyObject *>(impl_mirror_f))
            pass
        
        return InterfaceType._mk(
            self._hndl.defineInterfaceType(
                iftype_b._hndl,
                impl_f_h,
                impl_mirror_f_h))
        
    cpdef defineInterfaceInst(
            self, 
            InterfaceType t, 
            name, 
            is_mirror,
            req_f):
        # Create a class instance that binds interface_inst_req_f and
        # req_f together
        
        # TODO: complete
        cdef native_decl.IInterfaceImpl *impl_h = new native_decl.InterfaceImplClosure(
            <cpy_ref.PyObject *>(req_f))
        
        if t is None:
            raise Exception("Null type defined for interface %s" % name)
        
        # Hold the closure to prevent garbage collection
        Endpoint._req_l.append(req_f)
        
        ret_h = self._hndl.defineInterfaceInst(
            t._hndl,
            name.encode(),
            is_mirror,
            impl_h)
        
        if ret_h != NULL:
            return InterfaceInst._mk(ret_h)
        else:
            return None
    
    cpdef getInterfaceTypes(self):
        ret = []
        for i in range(self._hndl.getInterfaceTypes().size()):
            ret.append(InterfaceType._mk(self._hndl.getInterfaceTypes().at(i)))
        return ret
    
    cpdef getInterfaceInsts(self):
        ret = []
        for i in range(self._hndl.getInterfaceInsts().size()):
            ret.append(InterfaceInst._mk(self._hndl.getInterfaceInsts().at(i)))
        return ret
    
    cpdef getPeerInterfaceInsts(self):
        ret = []
        for i in range(self._hndl.getPeerInterfaceInsts().size()):
            ret.append(InterfaceInst._mk(self._hndl.getPeerInterfaceInsts().at(i)))
        return ret
    
    async def process_one_message_a(self):
        # TODO: need a more-complementary approach for
        # non-direct endpoint backends
        from tblink_rpc_core.tblink import TbLink
        tblink = TbLink.inst()
        ev = tblink.mkEvent()
        
        def ev_f(e):
            nonlocal ev
            print("process_one_message_a: event")
            ev.set()
            
        l = self.addListener(ev_f)
        print("--> ev.wait()")
        await ev.wait()
        print("<-- ev.wait()")
        self.removeListener(l)
        return 1
    
    cpdef process_one_message(self):
        return self._hndl.process_one_message()
        pass
    
    cpdef mkValBool(self, val):
        return ParamValBool._mk(self._hndl.mkValBool(val))
    
    cpdef mkValIntU(self, val, width):
        return ParamValInt._mk(self._hndl.mkValIntU(val, width))
    
    cpdef mkValIntS(self, val, width):
        return ParamValInt._mk(self._hndl.mkValIntS(val, width))
    
    cpdef mkValMap(self):
        return ParamValMap._mk(self._hndl.mkValMap())
    
    cpdef mkValStr(self, val):
        return ParamValStr._mk(self._hndl.mkValStr(val.encode()))
    
    cpdef mkValVec(self):
        return ParamValVec._mk(self._hndl.mkValVec())
    
    @staticmethod
    cdef Endpoint _mk(native_decl.IEndpoint *hndl):
        ret = Endpoint()
        ret._hndl = hndl
        return ret

#********************************************************************
#*
#********************************************************************
cdef public native_decl.IInterfaceImpl *interface_impl_factory_proxy_createImpl(
    obj_f
    ) with gil:
    cdef native_decl.InterfaceImplClosure *ret
    impl_o = None
    print("interface_impl_factory_proxy_createImpl: obj_f=%s" % str(obj_f))
    sys.stdout.flush()
    if callable(obj_f):
        # Call
        impl_o = obj_f()
    elif hasattr(obj_f, "createImpl"):
        impl_o = obj_f.createImpl()
    else:
        print("Error: interface impl factory is not usable")
        
    print("impl_o=%s" % str(impl_o))
    sys.stdout.flush()
        
    ret = new native_decl.InterfaceImplClosure(<cpy_ref.PyObject *>(impl_o))
    
    return ret

#********************************************************************
#* endpoint_ev_f
#********************************************************************
cdef public void endpoint_ev_callback_f(
    obj,
    const native_decl.IEndpointEvent *ev) with gil:
#    print("--> endpoint_ev_f")
#    sys.stdout.flush()
#    print("  obj=%s" % str(obj))
#    sys.stdout.flush()
    try:
        obj(EndpointEvent._mk(ev))
    except Exception as e:
        print("Exception: %s" % str(e))
        traceback.print_exc()
#    print("<-- endpoint_ev_f")

#********************************************************************
#* time_callback_f
#********************************************************************
cdef public void time_callback_f(obj) with gil:
    print("time_callback_f: obj=%s" % str(obj))
    # TODO: Must deal with 
    print("--> Calling")
    obj()
    print("<-- Calling")

#********************************************************************
#* interface_inst_req_f()
#*
#* Callback method for invocation requets
#********************************************************************
cdef public void interface_inst_req_f(
    obj, 
    native_decl.IInterfaceInst  *inst,
    native_decl.IMethodType     *method,
    intptr_t                    call_id,
    native_decl.IParamValVec    *params) with gil:
    print("--> interface_inst_req_f")
    obj(
        InterfaceInst._mk(inst),
        MethodType._mk(method),
        call_id,
        ParamValVec._mk(params))
    print("<-- interface_inst_req_f")
        
#********************************************************************
#* LaunchType
#********************************************************************
cdef class LaunchType(object):
    cdef native_decl.ILaunchType *_hndl
    
    def __init__(self):
        pass
    
    cpdef name(self):
        return self._hndl.name().decode()
    
    cpdef launch(self, params : LaunchParams, services=None):
        cdef cpp_pair[IEndpointP,cpp_string] result = self._hndl.launch(params._hndl, NULL)
        params._hndl = NULL
        
        if result.first:
            # Valid return
            ep = Endpoint()
            ep._hndl = result.first
            return (ep, result.second.decode())
        else:
            return (None, result.second.decode())
    
    cpdef LaunchParams newLaunchParams(self):
        ret = LaunchParams()
        ret._hndl = self._hndl.newLaunchParams()
        return ret
    
_TbLinkEventKind_n2p_m = {
    native_decl.TbLinkEventKind.AddEndpoint : TbLinkEventKind.AddEndpoint,
    native_decl.TbLinkEventKind.RemEndpoint : TbLinkEventKind.RemEndpoint
    }
    
#********************************************************************
#* TbLinkEvent
#********************************************************************
cdef class TbLinkEvent(object):
    cdef native_decl.ITbLinkEvent *_hndl
    
    cpdef kind(self):
        return _TbLinkEventKind_n2p_m[self._hndl.kind()]
    
    cpdef hndl(self):
        # TODO: can we guess from the kind?
        return None

    @staticmethod
    cdef mk(native_decl.ITbLinkEvent *hndl):
        ret = TbLinkEvent()
        ret._hndl = hndl
        return ret
    

#********************************************************************
#* TbLinkListenerClosure
#********************************************************************
cdef class TbLinkListenerClosure(object):
    cdef native_decl.TbLinkListenerClosure *_hndl
    cdef object _listener
    
    def __init__(self, listener):
        self._hndl = new native_decl.TbLinkListenerClosure(<cpy_ref.PyObject *>(self))
        self._listener = listener
    
    cpdef event(self, TbLinkEvent ev):
        self._listener.event(ev)
        
cdef public void tblink_listener_event(obj, native_decl.ITbLinkEvent *ev) with gil:
    obj.event(TbLinkEvent.mk(ev))

#********************************************************************
#* TbLink
#********************************************************************
cdef class TbLink(object):
    cdef native_decl.ITbLink *_hndl
    cdef dict _ep_listener_m
#    cdef cpp_map[object, TbLinkListenerClosure] _ep_listener_m
    
    def __init__(self, lib_path):
        self._hndl = native_decl.get_tblink(lib_path.encode())
        self._ep_listener_m = {}
        
    cpdef Endpoint getDefaultEP(self):
        cdef IEndpointP ep = self._hndl.getDefaultEP()
        if ep:
            ret = Endpoint()
            ret._hndl = ep
            return ret
        else:
            return None
        
    cpdef addListener(self, l):
        cdef TbLinkListenerClosure c = TbLinkListenerClosure(l)
        self._hndl.addListener(c._hndl)
        self._ep_listener_m[l] = c
        pass
    
    cpdef removeListener(self, l):
        cdef TbLinkListenerClosure c = <TbLinkListenerClosure>self._ep_listener_m[l]
        self._hndl.removeListener(c._hndl)
        self._ep_listener_m.pop(l)
        pass
    
    
    cpdef getEndpoints(self):
        cdef const cpp_vector[IEndpointP] *ep_l = &self._hndl.getEndpoints()
        ret = []
        
        for i in range(ep_l.size()):
            ret.append(Endpoint._mk(ep_l.at(i)))
        
        return ret
        
    
    cpdef findLaunchType(self, id):
        ret = LaunchType()
        ret._hndl = self._hndl.findLaunchType(id.encode())
        
        if ret._hndl:
            return ret
        else:
            return None
        
    cpdef launchTypes(self):
        ret = []
        for i in range(self._hndl.launchTypes().size()):
            t = LaunchType()
            t._hndl = self._hndl.launchTypes().at(i)
            ret.append(t)
            
        return ret
    

        
