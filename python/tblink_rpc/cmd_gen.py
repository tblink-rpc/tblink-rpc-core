'''
Created on Oct 20, 2021

@author: mballance
'''
from tblink_rpc.output_writer_cpp import OutputWriterCpp
from tblink_rpc.input_reader_yaml import InputReaderYaml
from tblink_rpc.input_spec import InputSpec
from tblink_rpc.output_spec import OutputSpec

def cmd_gen(args):
    print("cmd_gen")
    
    os_m = {
        "cpp" : OutputWriterCpp()
        }
    is_m = {
        "default": InputReaderYaml(),
        "yaml": InputReaderYaml()
        }

    in_spec = InputSpec()
    in_spec.files.extend(args.files)

    if hasattr(args, "input_stype") and args.input_style is not None:
        if not args.input_style in is_m.keys():
            raise Exception("Input style %s not recognized" % args.input_style)
        in_p = is_m[args.input_style]
    else:
        # Use default
        in_p = is_m["default"]
    
    if hasattr(args, "output_style") and args.output_style is not None:
        if args.output_style not in os_m.keys():
            raise Exception("Output style %s not recognized" % args.output_style)
        out_p = os_m[args.output_style]
    else:
        raise Exception("Output-style not specified")
   
    print("in_p: %s ; out_p: %s" % (str(in_p), str(out_p)))
    idl_s = in_p.read(in_spec)
    print("idl_s: %s" % str(idl_s))
    
    out_s = OutputSpec()

    if hasattr(args, "namespace") and args.namespace is not None:
        out_s.namespace = args.namespace
        
    if hasattr(args, "output") and args.output is not None:
        out_s.out = args.output

    iftypes = []        
    if hasattr(args, "include") and args.include is not None:
        print("TODO: select specific interfaces")
    else:
        # Start with all types
        iftypes.extend(idl_s.iftypes)
        
    if hasattr(args, "exclude") and args.exclude is not None:
        print("TODO: select specific interfaces")
        
    if len(iftypes) == 0:
        raise Exception("No interface types selected")
        
    out_s.iftypes.extend(iftypes)
    
    out_p.write(out_s)
        
    
    pass

