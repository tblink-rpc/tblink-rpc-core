'''
Created on Oct 24, 2021

@author: mballance
'''
import os

from tblink_rpc.output import Output
from tblink_rpc.output_spec import OutputSpec
from tblink_rpc.output_writer import OutputWriter
from crypt import methods
from tblink_rpc.method_spec import MethodSpec
import types
from tblink_rpc.type_spec import TypeSpec, TypeKind


class OutputWriterCpp(OutputWriter):
    
    def __init__(self):
        super().__init__()
        
    def write(self, out_s:OutputSpec):
        self.out_s = out_s
        out_cpp = out_s.out
        
        if out_cpp is None:
            if out_s.namespace is not None:
                out_cpp = out_s.namespace + ".cpp"
            elif len(out_s.iftypes) == 1:
                out_cpp = out_s.iftypes[0].name + ".cpp"
            else:
                out_cpp = "tblink_rpc_iftypes.cpp"

        if out_cpp.rfind(".") != -1:
            out_h = out_cpp[:out_cpp.rfind(".")] + ".h"
        else:
            out_h = out_cpp + ".h"
            
        print("out_cpp=%s out_h=%s" % (out_cpp, out_h))
        
        out_cpp_fp = open(out_cpp, "w")
        out_h_fp = open(out_h, "w")

        self.out_cpp = Output(out_cpp_fp)
        self.out_h = Output(out_h_fp)
        
        self.out_h.println("/****************************************************************************")
        self.out_h.println(" * Interface-type file %s" % os.path.basename(out_h))
        self.out_h.println(" * Note: generated file. Do not modify")
        self.out_h.println(" ****************************************************************************/")
        self.out_h.println("#pragma once")
        self.out_h.println("#include <functional>")
        self.out_h.println("#include \"tblink_rpc/tblink_rpc.h\"")
        
        self.out_cpp.println("/****************************************************************************")
        self.out_cpp.println(" * Interface-type file %s" % os.path.basename(out_cpp))
        self.out_cpp.println(" * Note: generated file. Do not modify")
        self.out_cpp.println(" ****************************************************************************/")
        self.out_cpp.println("#include \"%s\"" % os.path.basename(out_h))
        self.out_cpp.println("using namespace tblink_rpc_core;")
        
        print("iftypes=%s" % str(out_s.iftypes))
        for iftype in out_s.iftypes:
            self.write_iftype(iftype)
        
        out_cpp_fp.close()
        out_h_fp.close()
        
    def write_iftype(self, iftype):
        self.write_iftype_h(iftype)
        self.write_iftype_cpp(iftype)
        
    def write_iftype_h(self, iftype):
        self.out_h.println("/****************************************************************************")
        self.out_h.println(" * Interface Implementation %s" % iftype.name)
        self.out_h.println(" ****************************************************************************/")
        
        if self.out_s.namespace is not None:
            print("TODO: enclose in a namespace")
        
        self.out_h.println("class %s {" % iftype.name)
        self.out_h.println("public:")
        self.out_h.inc_ind()
        # Constructor/destructor
        self.out_h.println()
        self.out_h.println("%s();" % iftype.name)
        self.out_h.println()
        self.out_h.println("virtual ~%s();" % iftype.name)
        
        # Methods
        self.out_h.println()
        self.out_h.println()
        
        for m in iftype.methods:
            self.define_method_proto(m)
        
        # Instance-definition method
        self.out_h.println("tblink_rpc_core::IInterfaceInst *defineInterfaceInst(")
        self.out_h.inc_ind()
        self.out_h.println("tblink_rpc_core::IEndpoint *ep,")
        self.out_h.println("const std::string          &inst_name,");
        self.out_h.println("bool                       is_mirror);")
        self.out_h.dec_ind()
        
        # Type-registration method
        self.out_h.println("static tblink_rpc_core::IInterfaceType *register_type(")
        self.out_h.inc_ind()
        self.out_h.println("tblink_rpc_core::IEndpoint *ep);")
        self.out_h.dec_ind()

        self.out_h.println()
        
        # Protected Data
        self.out_h.dec_ind()
        self.out_h.println("protected:")
        self.out_h.inc_ind()
        self.out_h.println("tblink_rpc_core::IInterfaceInst      *m_ifinst;")
        for m in iftype.methods:
            self.out_h.println("%s_t m_%s_impl;" % (m.name, m.name))
        
        # TODO: Per-method function
        self.out_h.println()
        
        # Private Implementation
        self.out_h.dec_ind()
        self.out_h.println("private:")
        self.out_h.inc_ind()
        self.out_h.println("void invoke_req(")
        self.out_h.inc_ind()
        self.out_h.println("tblink_rpc_core::IInterfaceInst  *ifinst,")
        self.out_h.println("tblink_rpc_core::IMethodType     *method,")
        self.out_h.println("intptr_t                         call_id,")
        self.out_h.println("IParamValVec                     *params);")
        self.out_h.dec_ind()
        
        self.out_h.println()
        
        self.out_h.dec_ind()
        self.out_h.println("};")
        
        self.out_h.println("")
        
    def write_iftype_cpp(self, iftype):
        
        self.out_cpp.println("/****************************************************************************")
        self.out_cpp.println(" * Interface Type %s" % iftype.name)
        self.out_cpp.println(" ****************************************************************************/")
        
        self.out_cpp.println()
        self.out_cpp.println("%s::%s() {" % (iftype.name, iftype.name))
        self.out_cpp.inc_ind()
        self.out_cpp.println("m_ifinst = 0;")
        self.out_cpp.dec_ind()
        self.out_cpp.println("}")
        self.out_cpp.println()
        
        self.out_cpp.println("%s::~%s() {" % (iftype.name, iftype.name))
        self.out_cpp.inc_ind()
        self.out_cpp.dec_ind()
        self.out_cpp.println("}")
        self.out_cpp.println()
        
        for m in iftype.methods:
            self.define_method_impl(iftype, m)
            self.out_cpp.println()
        
        self.out_cpp.println("IInterfaceInst *%s::defineInterfaceInst(" % iftype.name)
        self.out_cpp.inc_ind()
        self.out_cpp.inc_ind()
        self.out_cpp.println("IEndpoint                 *ep,")
        self.out_cpp.println("const std::string         &inst_name,")
        self.out_cpp.println("bool                      is_mirror) {")
        self.out_cpp.dec_ind()
        self.out_cpp.println("m_ifinst = ep->defineInterfaceInst(")
        self.out_cpp.inc_ind()
        self.out_cpp.println("register_type(ep),")
        self.out_cpp.println("inst_name,")
        self.out_cpp.println("is_mirror,")
        self.out_cpp.println("std::bind(&%s::invoke_req, this," % iftype.name)
        self.out_cpp.inc_ind()
        self.out_cpp.println("std::placeholders::_1,")
        self.out_cpp.println("std::placeholders::_2,")
        self.out_cpp.println("std::placeholders::_3,")
        self.out_cpp.println("std::placeholders::_4);")
        self.out_cpp.dec_ind()
        
        self.out_cpp.dec_ind()
        self.out_cpp.println("return m_ifinst;")
        self.out_cpp.dec_ind()
        self.out_cpp.println("}")
        self.out_cpp.println()
        
        self.out_cpp.println("IInterfaceType *%s::register_type(IEndpoint *ep) {" % (
            iftype.name,))
        self.out_cpp.inc_ind()
        self.out_cpp.println("IInterfaceType *iftype = ep->findInterfaceType(\"%s\");" % iftype.name)
        self.out_cpp.println("IMethodTypeBuilder *method_b = 0;")
        self.out_cpp.println("if (!iftype) {")
        self.out_cpp.inc_ind()
        self.out_cpp.println("IInterfaceTypeBuilder *iftype_b = ep->newInterfaceTypeBuilder(\"%s\");" % iftype.name)
        for i,m in enumerate(iftype.methods):
            self.register_method_type(i, m)
        self.out_cpp.println("iftype = ep->defineInterfaceType(iftype_b);")
        self.out_cpp.dec_ind()
        self.out_cpp.println("}");
        self.out_cpp.println("return iftype;")
        self.out_cpp.dec_ind()
        self.out_cpp.println("}")
        self.out_cpp.println()
        
    def register_method_type(self, method_id, m : MethodSpec):

        self.out_cpp.println("method_b = iftype_b->newMethodTypeBuilder(")
        self.out_cpp.inc_ind()
        self.out_cpp.println("\"%s\"," % m.name)
        self.out_cpp.println("%d," % method_id)
        if m.rtype is None:
            self.out_cpp.println("0,")
        else:
            self.mk_method_type(m.rtype)
            self.out_cpp.write(",\n")
        if m.is_export:
            self.out_cpp.println("true,")
        else:
            self.out_cpp.println("false,")
        if m.is_blocking:
            self.out_cpp.println("true);")
        else:
            self.out_cpp.println("false);")
        self.out_cpp.dec_ind()
        
        # Define method parameters
        for p in m.params:
            self.out_cpp.println("method_b->add_param(")
            self.out_cpp.inc_ind()
            self.out_cpp.println("\"%s\"," % p[0])
            self.mk_method_type(p[1])
            self.out_cpp.write(");\n")
            self.out_cpp.dec_ind()
            
        self.out_cpp.println("ifype_b->add_method(iftype_b);")
        
    def define_method_proto(self, m):
        # Typedef for the signature
        self.out_h.println("/*")
        self.out_h.println(" * API for %s" % m.name)
        self.out_h.println(" */")
        self.out_h.write("%susing %s_t = std::function<" % (
            self.out_h.ind, m.name))
        self.write_ctype(self.out_h, m.rtype)
        self.out_h.write("(")
        for i,p in enumerate(m.params):
            if i > 0:
                self.out_h.write(", ")
            self.write_ctype(self.out_h, p[1])
            self.out_h.write(" %s" % p[0])
        self.out_h.write(")>;\n")
        
        # Prototype for the method
        self.out_h.write(self.out_h.ind)
        self.write_ctype(self.out_h, m.rtype)
        self.out_h.write(" %s(" % m.name)
        for i,p in enumerate(m.params):
            if i > 0:
                self.out_h.write(", ")
            self.write_ctype(self.out_h, p[1])
            self.out_h.write(" %s" % p[0])
        self.out_h.write(");\n");
        
        # Prototype for the set-implementation method
        self.out_h.println("void set_%s_impl(const %s_t &impl) { m_%s_impl = impl; }" % (
            m.name, m.name, m.name))
        
        self.out_h.println()
        self.out_h.println()
        
    def define_method_impl(self, iftype, m):
        
        self.out_cpp.write(self.out_cpp.ind)
        self.write_ctype(self.out_cpp, m.rtype)
        self.out_cpp.write(" %s::%s(" % (iftype.name, m.name))
        for i,p in enumerate(m.params):
            if i > 0:
                self.out_cpp.write(", ")
            self.write_ctype(self.out_cpp, p[1])
            self.out_cpp.write(" %s" % p[0])
        self.out_cpp.write(") {\n")
        self.out_cpp.inc_ind()
        
        # This is an invoke method if the method-type is 
        # export==True and mirror==False or export==False and mirror==True
        if m.is_export:
            self.out_cpp.println("if (!m_ifinst.is_mirror()) {")
        else:
            self.out_cpp.println("if (m_ifinst.is_mirror()) {")
        self.out_cpp.inc_ind()
        self.out_cpp.println("// implements remote-call")
        
        self.out_cpp.dec_ind()
        self.out_cpp.println("} else {")
        self.out_cpp.inc_ind()
        self.out_cpp.println("// implements local-call")
        self.out_cpp.println("if (m_%s_impl) {" % m.name);
        self.out_cpp.inc_ind()
        if m.rtype is None:
            self.out_cpp.write("%sm_%s_impl(" % (self.out_cpp.ind, m.name))
        else:
            self.out_cpp.write("%sreturn m_%s_impl(" % (self.out_cpp.ind, m.name))
        self.out_cpp.write(");\n")

        self.out_cpp.dec_ind()
        self.out_cpp.println("} else {")
        self.out_cpp.inc_ind()
        self.out_cpp.println("fprintf(stdout, \"Error: no implementation provided for %s\\n\");" % iftype.name)
        self.out_cpp.dec_ind()
        self.out_cpp.println("}")
        
        self.out_cpp.dec_ind()
        self.out_cpp.println("}")
        
        self.out_cpp.dec_ind()
        self.out_cpp.println("}")
        
        
    def write_ctype(self, out, t):
        if t is None:
            out.write("void")
        else:
            if t.kind == TypeKind.Bool:
                out.write("bool")
            elif t.kind == TypeKind.Int:
                if t.is_signed:
                    out.write("int%d_t" % t.width)
                else:
                    out.write("uint%d_t" % t.width)
            elif t.kind == TypeKind.Str:
                out.write("const std::string &")

    def mk_method_type(self, t : TypeSpec):
        self.out_cpp.write("%s" % self.out_cpp.ind)
        
        if t.kind == TypeKind.Bool:
            self.out_cpp.write("iftype_b.mkTypeBool()")
        elif t.kind == TypeKind.Int:
            self.out_cpp.write("iftype_b.mkTypeInt(")
            if t.is_signed:
                self.out_cpp.write("true, ")
            else:
                self.out_cpp.write("false, ")
            self.out_cpp.write("%d)" % t.width)
        elif t.kind == TypeKind.Str:
            self.out_cpp.write("iftype_b.mkTypeStr()")
        elif t.kind == TypeKind.Map:
            self.out_cpp.write("iftype_b.mkTypeMap(\n")
            self.out_cpp.inc_ind()
            self.mk_method_type(t.key_t)
            self.out_cpp.write(",\n")
            self.mk_method_type(t.elem_t)
            self.out_cpp.dec_ind()
            self.out_cpp.write(")")
        elif t.kind == TypeKind.Vec:
            self.out_cpp.write("iftype_b.mkTypeVec(\n")
            self.out_cpp.inc_ind()
            self.mk_method_type(t.elem_t)
            self.out_cpp.dec_ind()
            self.out_cpp.write(")")
    
    
    