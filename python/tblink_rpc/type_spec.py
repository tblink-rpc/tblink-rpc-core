'''
Created on Oct 17, 2021

@author: mballance
'''
from enum import Enum, auto

class TypeKind(Enum):
    Bool = auto()
    Int = auto()
    Map = auto()
    Str = auto()
    Vec = auto()

class TypeSpec(object):
    
    def __init__(self, kind):
        self.kind = kind
        pass