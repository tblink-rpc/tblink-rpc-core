'''
Created on Oct 16, 2021

@author: mballance
'''
import argparse
from tblink_rpc.cmd_gen import cmd_gen

def get_parser():
    parser = argparse.ArgumentParser()
    
    subparsers = parser.add_subparsers()
    subparsers.required = True
    subparsers.dest = 'command'
    
    # TODO: list and get help on input/output filters
    
    gen_cmd = subparsers.add_parser("gen",
        help="Generate output from input IDL")
    gen_cmd.add_argument("-i", "--include",
        action="append", 
        help="Specifies IDL elements to include")
    gen_cmd.add_argument("-e", "--exclude",
        action="append", 
        help="Specifies IDL elements to exclude")
    gen_cmd.add_argument("-is", "--input-style",
        help="Specifies input style to use")
    gen_cmd.add_argument("-os", "--output-style",
        help="Specifies output style to use")
    gen_cmd.add_argument("-o", "--output", 
        dest="output",
        help="Specifies output path")
    gen_cmd.add_argument("-ns", "--namespace",
        help="Specifies output namespace for targets that support it")
    gen_cmd.add_argument("files", action="append",
        help="Specifies input sources")
    gen_cmd.set_defaults(func=cmd_gen)
    
    return parser


def main():
    parser = get_parser()
    
    args = parser.parse_args()
    
    args.func(args)
    
    pass

if __name__ == "__main__":
    main()
