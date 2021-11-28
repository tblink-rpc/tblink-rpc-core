#****************************************************************************
#* native.pyx
#*
#* Python interface to the native implementation of TbLink
#****************************************************************************
from libcpp.pair cimport pair as cpp_pair
from libcpp.string cimport string as cpp_string
from libcpp.vector cimport vector as cpp_vector

cimport native_decl 

ctypedef native_decl.IEndpoint *IEndpointP
ctypedef native_decl.ILaunchType *ILaunchTypeP        

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
        
cdef class Endpoint(object):
    cdef native_decl.IEndpoint *_hndl
    
    def __init__(self):
        pass
        
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

cdef class TbLink(object):
    cdef native_decl.ITbLink *_hndl
    
    def __init__(self, lib_path):
        self._hndl = native_decl.get_tblink(lib_path.encode())
    
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

        
