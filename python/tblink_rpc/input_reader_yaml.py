'''
Created on Oct 20, 2021

@author: mballance
'''
from tblink_rpc.idl_spec import IDLSpec
from tblink_rpc.input_reader import InputReader
from tblink_rpc.input_spec import InputSpec
from tblink_rpc.yaml_idl_parser import YamlIDLParser


class InputReaderYaml(InputReader):
    
    def __init__(self):
        super().__init__()
        
    def read(self, in_spec:InputSpec)->IDLSpec:
        yaml_p = YamlIDLParser()
        
        for file in in_spec.files:
            with open(file, "r") as fp:
                yaml_p.parse(fp)
                
        return yaml_p.spec