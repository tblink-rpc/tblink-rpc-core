'''
Created on Sep 6, 2021

@author: mballance
'''
from enum import Enum, auto


class MethodE(Enum):
    BuildComplete   = auto()
    ConnectComplete = auto()
    InvokeNB        = auto()