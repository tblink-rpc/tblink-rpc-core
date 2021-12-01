#****************************************************************************
#* native.pyx
#*
#* Python interface to the native implementation of TbLink
#****************************************************************************
from libc.stdint cimport intptr_t
from libcpp.pair cimport pair as cpp_pair
from libcpp.string cimport string as cpp_string
from libcpp.vector cimport vector as cpp_vector
from cython.operator cimport dereference as deref, preincrement as inc

cimport native_decl 

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

    cdef add_arg(self, arg : str):
        self._hndl.add_arg(arg.encode())
        
    cdef add_param(self, key : str, val : str):
        self._hndl.add_param(
            key.encode(),
            val.encode())
        
#********************************************************************
#* ParamVal
#********************************************************************
cdef class ParamVal(object):
    cdef native_decl.IParamVal *_hndl
    
#********************************************************************
#* ParamValBool
#********************************************************************
cdef class ParamValBool(ParamVal):
    pass

#********************************************************************
#* ParamValVec
#********************************************************************
cdef class ParamValVec(ParamVal):

    @staticmethod
    cdef _mk(native_decl.IParamValVec *hndl):
        ret = ParamValVec()
        ret._hndl = hndl
        return ret
    pass

#********************************************************************
#* InterfaceInst
#********************************************************************
cdef class InterfaceInst(object):

    cdef native_decl.IInterfaceInst *_hndl
    
    def __init__(self):
        pass
    
    def name(self):
        return self._hndl.name().decode()

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
    
    def name(self):
        return self._hndl.name().decode()
    
    @staticmethod
    cdef _mk(native_decl.IInterfaceType *hndl):
        ret = InterfaceType()
        ret._hndl = hndl
        return ret
    
#********************************************************************
#* Type
#********************************************************************
cdef class Type(object):
    cdef native_decl.IType *_hndl
    
    @staticmethod
    cdef _mk(native_decl.IType *hndl):
        ret = Type()
        ret._hndl = hndl
        return ret
    
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
#* MethodType
#********************************************************************
cdef class MethodType(object):
    cdef native_decl.IMethodType *_hndl
    
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
        return MethodTypeBuilder._mk(
            self._hndl.newMethodTypeBuilder(
                name.encode(), id, rtype._hndl,
                is_export, is_blocking))
        
    cpdef add_method(self, MethodTypeBuilder mtb):
        return MethodType._mk(self._hndl.add_method(mtb._hndl))
        
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
#* Endpoint
#********************************************************************
cdef class Endpoint(object):
    cdef native_decl.IEndpoint *_hndl
    
    def __init__(self):
        pass
    
    cpdef build_complete(self):
        return self._hndl.build_complete()
    
    cpdef is_build_complete(self):
        return self._hndl.is_build_complete()
    
    cpdef connect_complete(self):
        return self._hndl.connect_complete()
    
    cpdef is_connect_complete(self):
        return self._hndl.is_connect_complete()
    
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
    
    cpdef defineInterfaceType(self, InterfaceTypeBuilder iftype_b):
        return InterfaceType._mk(
            self._hndl.defineInterfaceType(iftype_b._hndl))
        
    cpdef defineInterfaceInst(
            self, 
            InterfaceType t, 
            name, 
            is_mirror, 
            req_f):
            # Create a class instance that binds interface_inst_req_f and
            # req_f together
        self._hndl.defineInterfaceInst(
            t._hndl,
            name.encode(),
            is_mirror,
            native_decl.wrapper())
        pass
    
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
    
cdef public void interface_inst_req_f(
    obj, 
    native_decl.IInterfaceInst  *inst,
    native_decl.IMethodType     *method,
    intptr_t                    call_id,
    native_decl.IParamValVec    *params):
    obj(
        InterfaceInst._mk(inst),
        MethodType._mk(method),
        call_id,
        ParamValVec._mk(params))
        
#********************************************************************
#* LaunchType
#********************************************************************
cdef class LaunchType(object):
    cdef native_decl.ILaunchType *_hndl
    
    def __init__(self):
        pass
    
    cpdef name(self):
        return self._hndl.name().decode()
    
    cpdef launch(self, params : LaunchParams):
        cdef cpp_pair[IEndpointP,cpp_string] result = self._hndl.launch(params._hndl)
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

#********************************************************************
#* TbLink
#********************************************************************
cdef class TbLink(object):
    cdef native_decl.ITbLink *_hndl
    
    def __init__(self, lib_path):
        self._hndl = native_decl.get_tblink(lib_path.encode())
        
    cpdef Endpoint getDefaultEP(self):
        cdef IEndpointP ep = self._hndl.getDefaultEP()
        if ep:
            ret = Endpoint()
            ret._hndl = ep
            return ret
        else:
            return None
    
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

        
