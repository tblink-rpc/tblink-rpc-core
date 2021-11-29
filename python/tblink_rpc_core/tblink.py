'''
Created on Nov 27, 2021

@author: mballance
'''
from tblink_rpc_core import native
import os
from ctypes import CDLL
from tblink_rpc_core.launch_type import LaunchType

class TbLink(object):
    
    _inst = None
    
    def __init__(self):
        self.launchtype_m = {}
        self.launchtypes = []
        self.native_tblink = None
        self._setup_native()
    
    @classmethod
    def inst(cls):
        if cls._inst is None:
            cls._inst = TbLink()
        return cls._inst
    
    def getDefaultEP(self):
        return self.native_tblink.getDefaultEP()
    
    def launchTypes(self):
        ret = self.launchtypes.copy()
        ret.extend(self.native_tblink.launchTypes())
        return ret
    
    def addLaunchType(self, lt):
        self.launchtype_m[lt.name()] = lt
        self.launchtypes.append(lt)

    def findLaunchType(self, name) -> LaunchType:
        if name in self.launchtype_m.keys():
            return self.launchtype_m[name]
        else:
            return self.native_tblink.findLaunchType(name)
    
    def _setup_native(self):
        # First, check all the loaded libraries to see if they contain
        # the 'tblink' symbol. This will occur when Python is
        # loaded as an embedded interpreter in an environment that has
        # also independently loaded TbLink
        
        # '/usr/lib64/libgcc_s-8-20200928.so.1', 
        # '/usr/lib64/libstdc++.so.6.0.25', 
        # '/project/fun/tblink-rpc/tblink-bfms-rv/packages/tblink-rpc-core/python/tblink_rpc_core/native.cpython-36m-x86_64-linux-gnu.so', '/usr/lib64/python3.6/lib-dynload/_heapq.cpython-36m-x86_64-linux-gnu.so', '/usr/lib64/libz.so.1.2.11', 
        # '/usr/lib64/libc-2.28.so', 
        # '/usr/lib64/libm-2.28.so', 
        # '/usr/lib64/libutil-2.28.so', 
        # '/usr/lib64/libdl-2.28.so', 
        # '/usr/lib64/libpthread-2.28.so', 
        # '/usr/lib64/libpython3.6m.so.1.0', '/usr/lib64/libcrypto.so.1.1.1g', '/usr/lib64/ld-2.28.so        
        
        paths = []
        paths_s = set()
        # These are libraries that we know are pointless to search
        excludes = ['libgcc', 'libstdc++', 'libc', 'libdl', 'libz', 'libm', 'libutil', 'libpthread', 'ld-', 'libpython']
        with open("/proc/%d/maps" % os.getpid(), "r") as fp:
            while True:
                line = fp.readline()
                
                if not line:
                    break
               
                line = line.strip()

                idx = line.find('/')
                if idx != -1:                
                    path = line[idx:]
                    
                    if not os.path.isfile(path):
                        # File doesn't exist, which probably means that
                        # the path wraps around to the next line
                        line = fp.readline().strip()
                        
                        path += line
                        
                    if path.rfind('.so') != -1:
                        if path not in paths_s:
                            exclude = False
                            for e in excludes:
                                if path.find(e) != -1:
                                    exclude = True
                                    break

                            paths_s.add(path)
                            if not exclude:                                
                                paths.append(path)

        print("Paths: %s" % str(paths))
        
        lib_path = None
        
        for p in filter(lambda x : x.find('tblink') != -1, paths):
            lib = CDLL(p)
            try:
                getattr(lib, 'tblink')
                lib_path = p
                print("Found tblink")
                break
            except Exception as e:
                pass
            
        if lib_path is None:
            for p in filter(lambda x : x.find('tblink') == -1, paths):
                lib = CDLL(p)
                try:
                    getattr(lib, 'tblink')
                    lib_path = p
                    print("Found tblink")
                    break
                except Exception as e:
                    pass

        # Nothing already loaded provides tblink, so load the core library
        if lib_path is None:
            lib_dir = os.path.dirname(os.path.abspath(__file__))
            lib_path = os.path.join(lib_dir, "libtblink_rpc_core.so")
            
        print("lib_path: %s" % lib_path)
        self.native_tblink = native.TbLink(lib_path)        
    
    pass