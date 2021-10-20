'''
Created on Oct 19, 2021

@author: mballance
'''
from tblink_rpc.idl_spec import IDLSpec
import yaml
from yaml.loader import FullLoader
from tblink_rpc.interface_type_spec import InterfaceTypeSpec
from tblink_rpc.method_spec import MethodSpec
from _ast import Param

class YamlIDLParser(object):
    
    def __init__(self):
        self.spec = IDLSpec()
        pass
    
    def parse(self, in_s) -> IDLSpec:
        self.spec = IDLSpec()
        
        yaml_m = yaml.load(in_s, Loader=FullLoader)
        
        print("yaml_m: %s" % str(yaml_m))
        
        if "interface-types" in yaml_m.keys():
            self._process_interface_types(yaml_m["interface-types"])
        else:
            print("Warning: no relevant content in the YAML file")
        
        return self.spec    
    
    def _process_interface_types(self, iftypes):
        if not isinstance(iftypes, list):
            raise Exception("Expect a list of interface-type definitions, not %s" % (
                str(iftypes)))

        for iftype in iftypes:
            if not isinstance(iftype, dict):
                raise Exception("Expect a dict of <iftype-name>:, not %s" % (
                    str(iftype)))
            iftype_name = next(iter(iftype.keys()))
            iftype_s = iftype[iftype_name]
            
            iftype_m = InterfaceTypeSpec(iftype_name)
            
            if iftype_s is None:
                raise Exception("No content inside interface-type %s" % (iftype_name))
            
            if not isinstance(iftype_s, dict):
                raise Exception("Expect a dict of interface-type content, not %s" % (
                    str(iftype_s)))
                
            if "methods" not in iftype_s.keys():
                raise Exception("No methods in interface type %s" % iftype_name)
            
            methods_s = iftype_s["methods"]
            
            if not isinstance(methods_s, list):
                raise Exception("Expect methods to be a list, not %s" % str(methods_s))
            
            for method_s in methods_s:
                method = self._process_method(method_s)
                iftype_m.add_method(method)
            
            print("iftype_name: %s" % iftype_name)
            
        return iftype_m
    
    def _process_method(self, method_s):
        
        method_name = next(iter(method_s))
        method = method_s[method_name]
        rtype_m = None
        is_export = False
        is_blocking = False
        
        if "is-export" in method.keys():
            is_export = bool(method["is-export"])
        else:
            print("Warning: method %s doesn't specify is-export" % method_name)
            
        if "is-blocking" in method.keys():
            is_blocking = bool(method["is-blocking"])
        else:
            print("Warning: method %s doesn't specify is-blocking" % method_name)
            
        if "returns" in method.keys():
            rtype_m = self._process_type(method["returns"])
        
        method_m = MethodSpec(
            method_name,
            rtype_m,
            is_export,
            is_blocking)
        
        print("Method %s" % method_name)
        if "parameters" in method.keys() and method["parameters"] is not None:
            parameters_s = method["parameters"]
            
            for param_si in parameters_s:
                pname = next(iter(param_si))
                param_s = param_si[pname]
                ptype = self._process_type(param_s)
                method_m.add_param(pname, ptype)
            
        
        return method_m

    def _process_type(self, type_s):
        return None
        
        
    