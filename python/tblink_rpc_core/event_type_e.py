'''
Created on Dec 28, 2021

@author: mballance
'''
from enum import Enum, auto


class EventTypeE(Enum):
    Unknown = auto()
    OutInvokeReqB = auto()
    InInvokeRspB = auto()
    InInvokeReqB = auto()
    OutInvokeRspB = auto()
    OutInvokeReqNB = auto()
    InInvokeRspNB = auto()
    InInvokeReqNB = auto()
    OutInvokeRspNB = auto()
    Terminate = auto()