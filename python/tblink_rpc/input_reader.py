'''
Created on Oct 20, 2021

@author: mballance
'''
from tblink_rpc.input_spec import InputSpec
from tblink_rpc.idl_spec import IDLSpec

class InputReader(object):
    
    def __init__(self):
        pass
    
    def supports(self, in_spec : InputSpec) -> bool:
        """Indicates whether this reader supports 
            a given input 
        """
        pass
    
    def read(self, in_spec : InputSpec) -> IDLSpec:
        raise Exception("Class %s doesn't implement InputReader.read" % str(type(self)))
        