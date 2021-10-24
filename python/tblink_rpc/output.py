'''
Created on Oct 24, 2021

@author: mballance
'''

class Output(object):
    
    def __init__(self, fp):
        self.ind = ""
        self.fp = fp
        
    def println(self, line=None):
        if line is not None:
            self.fp.write(self.ind)
            self.fp.write(line)
        self.fp.write("\n")
        
    def write(self, line):
        self.fp.write(line)
        
    def inc_ind(self):
        self.ind += "    "
        
    def dec_ind(self):
        if len(self.ind) > 4:
            self.ind = self.ind[4:]
        else:
            self.ind = ""
            
    
    