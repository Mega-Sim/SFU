#!/usr/bin/env python3
# -*- coding:utf-8 -*-
import yaml
from vpython import *
from base_simulator import *


def centerUpdate(ev):
    if scene.mouse.shift :
        scene.center = scene.mouse.pos


scene.width = 600
scene.height = 1000
scene.center = vector(0, -1500, 0)
scene.bind('mouseup', centerUpdate)


class OHTTransViewer(Simulator):
    width, length, height = 467.6, 638, 682.5 # size in each x,z,y axis
    roof_thickness = 100
    thickness = 50
    slide_thickness = 50
    slide_length = length*.4
    hoist_thickness = 50
    hoist_length = length*.6
    hand_width = width*.8
    hand_thickness = 50
    hand_length = length*.6


    @classmethod
    def build_oht_body(cls):
        cls.body = {}
        cls.body['roof_frame'] = box(pos=vector(0,0,0), size=vector(cls.width, cls.roof_thickness, cls.length), opacity=1)
        cls.body['front_frame'] = box(pos=vector(0,-(cls.height-cls.roof_thickness)/2, -(cls.length-cls.thickness)/2),
                    size=vector(cls.width, cls.height-cls.roof_thickness, cls.thickness), opacity=1)
        cls.body['rear_frame'] = box(pos=vector(0, -(cls.height-cls.roof_thickness)/2, (cls.length-cls.thickness)/2),
                   size=vector(cls.width, cls.height-cls.roof_thickness, cls.thickness), opacity=1)
        cls.body['front_shutter'] = box(pos=vector(0, -cls.height+cls.roof_thickness+cls.thickness/2, -cls.length/2 + cls.thickness),
                                        size=vector(cls.width*.5, cls.thickness/2, 1), color=color.red)
        cls.body['rear_shutter'] = box(pos=vector(0, -cls.height+cls.roof_thickness+cls.thickness/2, cls.length/2 - cls.thickness),
                                        size=vector(cls.width*.5, cls.thickness/2, 1), color=color.red)
        cls.body['slide'] = box(pos=vector(0, -(cls.roof_thickness/2+cls.slide_thickness/2), 0),
                    size = vector(cls.width, cls.slide_thickness, cls.slide_length), color=color.blue)
        cls.body['hoist'] = box(pos=vector(0, -(cls.roof_thickness/2+cls.slide_thickness+cls.hoist_thickness/2), 0),
                    size = vector(cls.width, cls.hoist_thickness, cls.hoist_length), color=color.cyan)
        cls.body['hand'] = box(pos=vector(0, -(cls.roof_thickness/2+cls.slide_thickness+cls.hoist_thickness+cls.hand_thickness/2), 0),
                    size = vector(cls.hand_width, cls.hand_thickness, cls.hand_length), color=color.orange)
        cls.body['push'] = cone(pos=cls.body['hand'].pos + vector(0, -cls.hand_thickness/2, 0), axis=vector(0,-1,0),
                                length=cls.hand_thickness/2, radius=30, color=color.blue)
        cls.body['left_gripper'] = box(pos=cls.body['hand'].pos + vector(0, -cls.hand_thickness, -150),
                                  size = vector(cls.slide_length*.8, cls.slide_thickness, cls.slide_thickness/2), color=color.blue)
        cls.body['right_gripper'] = box(pos=cls.body['hand'].pos + vector(0, -cls.hand_thickness, 150),
                                  size = vector(cls.slide_length*.8, cls.slide_thickness, cls.slide_thickness/2), color=color.blue)
        cls.body['foup'] = cylinder(pos=cls.body['hand'].pos + vector(0, -cls.hand_thickness, 0), axis=vector(0,-1,0), length=300, radius=150, color=color.white)

        cls.body['cables'] = [
            cylinder(pos = cls.body['hoist'].pos + vector(-150,0,0), axis=vector(0,-1,0), length=1, radius=10, color=color.cyan),
            cylinder(pos=cls.body['hoist'].pos + vector(100,0,-100), axis=vector(0, -1, 0), length=1, radius=10, color=color.cyan),
            cylinder(pos=cls.body['hoist'].pos + vector(100,0,100), axis=vector(0, -1, 0), length=1, radius=10, color=color.cyan)]
        cls.offset = {
            'slide': vector(cls.body['slide'].pos),
            'hoist': vector(cls.body['hoist'].pos),
            'hand': vector(cls.body['hand'].pos),
            'front_shutter': vector(cls.body['front_shutter'].pos),
            'rear_shutter': vector(cls.body['rear_shutter'].pos),
            'left_gripper': vector(cls.body['left_gripper'].pos),
            'right_gripper': vector(cls.body['right_gripper'].pos),
        }
        cls.equipment = {}
        cls.equipment['load_port'] = cylinder(pos=vector(0, -3000, 0), radius = 300, axis=vector(0,-1,0),
                                              length=cls.thickness*4, color=color.orange, visible=False)
        cls.equipment['foup'] = cylinder(pos = cls.equipment['load_port'].pos, axis = vector(0,1,0), radius=150, length=300, visible=False)

    @classmethod
    def update_model(cls, slide, hoist, shutter_close, grip_close):
        cls.body['slide'].pos = cls.offset['slide'] + vector(-slide, 0, 0)
        cls.body['hoist'].pos = cls.offset['hoist'] + vector(-slide, 0, 0)
        cls.body['hand'].pos = cls.offset['hand'] + vector(-slide, -hoist, 0)
        cls.body['push'].pos = cls.body['hand'].pos + vector(0, -cls.hand_thickness/2, 0)
        cls.body['push'].color = color.red if cls.pushed else color.blue

        if hoist == 0:
            hoist = 1
        cls.body['cables'][0].length = hoist
        cls.body['cables'][0].pos = cls.body['hoist'].pos + vector(-150, -cls.hoist_thickness/2,0)
        cls.body['cables'][1].length = hoist
        cls.body['cables'][1].pos = cls.body['hoist'].pos + vector(100, -cls.hoist_thickness/2,-100)
        cls.body['cables'][2].length = hoist
        cls.body['cables'][2].pos = cls.body['hoist'].pos + vector(100, -cls.hoist_thickness/2,100)
        if shutter_close :
            shutter = 200
            cls.body['front_shutter'].color = color.red
            cls.body['rear_shutter'].color = color.red
        else:
            shutter = 10
            cls.body['front_shutter'].color = color.blue
            cls.body['rear_shutter'].color = color.blue
        cls.body['front_shutter'].pos.z = cls.offset['front_shutter'].z + shutter/2
        cls.body['front_shutter'].size.z = shutter
        cls.body['rear_shutter'].pos.z = cls.offset['rear_shutter'].z - shutter/2
        cls.body['rear_shutter'].size.z = shutter
        if grip_close:
            grip = 0
            cls.body['left_gripper'].color = color.red
            cls.body['right_gripper'].color = color.red
        else:
            grip = cls.thickness/2
            cls.body['left_gripper'].color = color.blue
            cls.body['right_gripper'].color = color.blue

        cls.body['left_gripper'].pos.y = cls.body['hand'].pos.y - cls.hand_thickness
        cls.body['right_gripper'].pos.y = cls.body['hand'].pos.y - cls.hand_thickness
        cls.body['left_gripper'].pos.z = cls.offset['left_gripper'].z - grip
        cls.body['right_gripper'].pos.z = cls.offset['right_gripper'].z + grip
        cls.body['left_gripper'].pos.x = cls.offset['left_gripper'].x - slide
        cls.body['right_gripper'].pos.x = cls.offset['right_gripper'].x - slide

        if cls.oht_has_foup:
            cls.body['foup'].pos = cls.body['hand'].pos + vector(0, -cls.hand_thickness, 0)
            cls.body['foup'].visible = True
        else:
            cls.body['foup'].visible = False

        if cls.eq_has_foup:
            cls.equipment['foup'].pos = cls.equipment['load_port'].pos
            cls.equipment['foup'].visible = True
        else:
            cls.equipment['foup'].visible = False

    @classmethod
    def init(cls):
        cls.trans_flags = {
            "precheck": 0,
        }
        cls.flags = {}
        cls.flag_objects = {}
        cls.build_oht_body()
        cls.oht_has_foup = False
        cls.eq_has_foup = False
        cls.pushed = False


    @classmethod
    @reconnectable
    def get_status(cls):
        res = cls.stub.getOHTStatus(Empty())
        return yaml.load(res.ymldata)

    @classmethod
    def update(cls, state):
        slide = state['oht']['position']['slide']
        hoist = state['oht']['position']['hoist']
        shutter_close = state['oht']['dio']['shutter_front_close_sen']
        hand_close = state['oht']['dio']['hand_close_sen']
        cls.oht_has_foup = state['oht']['has_foup']
        cls.pushed = state['oht']['dio']['hand_push_sen']
        if 'station' in state:
            cls.eq_has_foup = state['station']['has_foup']
            cls.equipment['load_port'].pos = cls.offset['hand'] + vector(-state['station']['slide'], -state['station']['hoist'] - 350, 0)
            cls.equipment['load_port'].visible = True
        else:
            cls.eq_has_foup = False
            cls.equipment['foup'].visible = False
            cls.equipment['load_port'].visible = False
        cls.update_model(slide, hoist, shutter_close, hand_close)


    @classmethod
    def loop(cls):
        while True:
            rate(30)
            status = cls.get_status()
            cls.update(status)


OHTTransViewer.init()
OHTTransViewer.loop()
