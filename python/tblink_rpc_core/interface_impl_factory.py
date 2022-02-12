'''
Created on Feb 6, 2022

@author: mballance
'''

class InterfaceImplFactory(object):
    
    def createImpl(self):
        raise NotImplementedError("InterfaceImplFactory.createImpl for type %s" % str(type(self)))
