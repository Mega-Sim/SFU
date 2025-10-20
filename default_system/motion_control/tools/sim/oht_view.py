#!/usr/bin/env python3
#-*- coding:utf-8 -*-
import yaml
from base_simulator import *
from oht_model import *

# customize scale
class OHTSimulator(Simulator):
    @classmethod
    @reconnectable
    def getStatus(cls):
        res = cls.stub.getOHTStatus(Empty())
        return yaml.load(res.ymldata)

    @classmethod
    def getOHTLocation(cls, loc):
        return getPosOnLink(loc['from'], loc['to'], loc['disp']) if loc['to'] != 0xFFFFFFFF else None

    @classmethod
    def getOHTDirection(cls, loc):
        if loc['to'] == 0xFFFFFFFF: return None
        displacement = vector(*getNodePos(loc['to'])) - vector(*getNodePos(loc['from']))
        return displacement.norm()

    @classmethod
    def loop(cls):
        vhl = Vehicle(0,0)
        ovhl = Vehicle(0,0)

        while True:
            rate(24)
            status = cls.getStatus()
            #print(status["oht"]["location"])

            if not vhl.body.visible: vhl.body.visible = True
            vhl.setPos(cls.getOHTLocation(status['oht']['location']))
            vhl.setAxis(cls.getOHTDirection(status['oht']['location']))
            vhl.body.color = color.orange if (status['oht']['speed'] == 0) else color.red

            if 'other_oht' in status['oht'] and status['oht']['other_oht']['enabled'] :
                if not ovhl.body.visible: ovhl.body.visible = True
                new_pos = cls.getOHTLocation(status['oht']['other_oht'])
                new_axis = cls.getOHTDirection(status['oht']['other_oht'])
                if new_pos != None:
                    ovhl.setPos(new_pos)
                    ovhl.setAxis(new_axis)
                #print(status['oht']['other_oht'])
            else:
                ovhl.body.visible = False

OHTSimulator.loop()
sleep(1)
os._exit(0)
