#!/usr/bin/env python3
#-*- coding:utf-8 -*-
import sys, os, grpc
from time import sleep
sys.path.append("./rpc")
from functools import wraps
from rpc.ecmaster_pb2_grpc import EcMasterRpcStub
from rpc.ecmaster_pb2 import Empty, ModelName

HOST = "localhost" #"localhost" #"master3"
#HOST = "masterbd" #"localhost" #"master3"
RPC_PORT = 50051


class SimulatorBase(object):
    channel = grpc.insecure_channel('%s:%d' % (HOST, RPC_PORT))
    stub = EcMasterRpcStub(channel)

def reconnectable(fn):
    @wraps(fn)
    def new(*args, **kwargs):
        while True:
            try:
                return fn(*args, **kwargs)
            except:
                SimulatorBase.stub = EcMasterRpcStub(SimulatorBase.channel)
                sleep(1)
                print("Retry connection....\n")
    return new

class Simulator(SimulatorBase):
    @classmethod
    @reconnectable
    def getSystemName(cls):
        res = cls.stub.getSystemName(Empty())
        return res.name


if __name__ == "__main__":
    print(Simulator.getSystemName())
