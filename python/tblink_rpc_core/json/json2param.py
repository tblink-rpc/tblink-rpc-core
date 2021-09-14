'''
Created on Jul 7, 2021

@author: mballance
'''
from tblink_rpc_core.param_val_int import ParamValInt
from tblink_rpc_core.param_val_map import ParamValMap
from tblink_rpc_core.param_val_str import ParamValStr
from tblink_rpc_core.param_val_vec import ParamValVec
from tblink_rpc_core.param_val_bool import ParamValBool


class Json2Param(object):
    
    def __init__(self):
        pass
    
    def param(self, j):
        return self._map(j)
    

    def _map(self, m):
        ret = ParamValMap()

        if m is not None:        
            for key in m.keys():
                v = self._val(m[key])
                ret.setVal(key, v)
                
        return ret
    
    def _val(self, v):
        if type(v) == bool:
            return ParamValBool(bool(v))
        elif type(v) == int:
            return ParamValInt(v)
        elif type(v) == str:
            return ParamValStr(v)
        elif isinstance(v, dict):
            return self._map(v)
        elif isinstance(v, list):
            return self._list(v)
        else:
            raise Exception("unknown elem type " + str(type(v)))
    
    def _list(self, l):
        ret = ParamValVec()
        
        for v in l:
            ret.push_back(self._val(v))
            
        return ret