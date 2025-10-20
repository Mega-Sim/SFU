#!/usr/bin/env python3
#-*- coding:utf-8 -*-

from base_simulator import *
from stocker_model import robot, shelves, rate, Frame

class StockerSimulator(Simulator):
    @classmethod
    @reconnectable
    def getStatus(cls):
        res = cls.stub.getStockerStatus(Empty())
        return (res.z, res.x, res.r, res.y, res.crash, res.dout)


    @classmethod
    @reconnectable
    def getFoupList(cls):
        res = cls.stub.getFoupLocations(Empty())
        return res.foups.split(',')

    @classmethod
    def loop(cls):
        crashed= False
        while(not crashed):
            rate(24)
            st = cls.getStatus()
            foups = cls.getFoupList()
            #print(st)
            robot.posUpdate(st[0], st[1], st[2], st[3], 'Robot' in foups)
            Frame.doUpdate(st[5])
            for shelf in shelves:
                shelves[shelf].update(shelf in foups)

            crashed = st[4]
        # crashed
        robot.crash()


StockerSimulator.loop()
sleep(1)
os._exit(0)
