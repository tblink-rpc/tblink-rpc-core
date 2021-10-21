'''
Created on Oct 20, 2021

@author: mballance
'''
from tblink_rpc.output_spec import OutputSpec

class OutputWriter(object):
    
    def __init__(self):
        pass
    
    def write(self, out_s : OutputSpec):
        raise Exception("Class %s does not implement OutputWriter.write" % str(type(self)))