'''
Created on Oct 19, 2021

@author: mballance
'''

class IDLSpec(object):
    
    def __init__(self):
        self.iftype_m = {}
        self.iftypes = []
        pass
    
    def add_iftype(self, iftype):
        if iftype.name in self.iftype_m.keys():
            raise Exception("Duplicate iftype %s" % iftype.name)
        self.iftype_m[iftype.name] = iftype
        self.iftypes.append(iftype)
    
    