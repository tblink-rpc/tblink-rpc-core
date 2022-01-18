'''
Created on Jan 17, 2022

@author: mballance
'''
from enum import Enum, auto

class TbLinkEventKind(Enum):
    AddEndpoint = auto()
    RemEndpoint = auto()
    
    