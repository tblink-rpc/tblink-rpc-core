#****************************************************************************
#* native.pyx
#*
#* Python interface to the native implementation of TbLink
#****************************************************************************
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

        
