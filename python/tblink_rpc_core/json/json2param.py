'''
Created on Jul 7, 2021

@author: mballance
'''
from tblink_rpc_core.param_val_int import ParamValInt
from tblink_rpc_core.param_val_map import ParamValMap
from tblink_rpc_core.param_val_str import ParamValStr


class Json2Param(object):
    
    def __init__(self):
        pass
    
    def param(self, j):
        return self._map(j)
    

    def _map(self, m):
        ret = ParamValMap()
        
        for key in m.keys():
            v = m[key]
            
            print("v: " + str(type(v)))
            if type(v) == int:
                ret[key] = ParamValInt(v)
            elif type(v) == str:
                ret[key] = ParamValStr(v)
            elif isinstance(v, dict):
                ret[key] = self._map(v)
            else:
                raise Exception("unknown elem type " + str(type(v)))
                