'''
Created on Jul 5, 2021

@author: mballance
'''
from tblink_rpc_core.param_val_visitor import ParamValVisitor
from _io import StringIO

class Param2Json(ParamValVisitor):
    
    def __init__(self):
        self.s = None
        pass
    
    def json(self, p):
        self.s = StringIO()
        p.accept(self);
        return self.s.getvalue()
    
    def visit_bool(self, p):
        if p.val():
            self.s.write("true")
        else:
            self.s.write("false")
    
    def visit_int(self, p):
        self.s.write("%d" % p.val)
        
    def visit_map(self, m):
        self.s.write("{")
        for i, key in enumerate(m.val_m.keys()):
            if i > 0:
                self.s.write(", ")
            self.s.write("\"%s\" : " % key)
            m.val_m[key].accept(self)
        self.s.write("}")
            
    def visit_str(self, s):
        self.s.write("\"%s\"" % s.val())
        
    def visit_vec(self, v):
        # TODO:
        self.s.write("[")
        for i in range(v.size()):
            if i > 0:
                self.s.write(", ")
            v.at(i).accept(self)
        self.s.write("]")
