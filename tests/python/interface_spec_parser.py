'''
Created on Oct 16, 2021

@author: mballance
'''
import yaml
from unittest.case import TestCase
from _io import StringIO
from tblink_rpc.yaml_idl_parser import YamlIDLParser

class InterfaceSpecParser(TestCase):
    
    def test_smoke(self):
        spec = """
        interface-types:
            - my_if1:
                methods:
                    - a:
                        is-export: True
                        is-blocking: True
                        returns: uint16
                        parameters:
                            - a: uint32
                            - a: uint64
                    - b:
                        is-export: False
                        is-blocking: True
            - my_if2:
                methods:
                    - a: 
                        is-export: True
                        is-blocking: True
                        returns: uint32
                        parameters:
                    - b:
                        is-export: False
                        is-blocking: True
        """
        
        in_d = StringIO(spec)
        
        YamlIDLParser().parse(in_d)
       
        
        
        
        pass