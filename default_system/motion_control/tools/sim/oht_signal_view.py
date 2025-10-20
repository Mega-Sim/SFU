#!/usr/bin/env python3
# -*- coding:utf-8 -*-
from oht_signal_logger import *
from vpython import *


def center_update(ev):
    if scene.mouse.shift :
        scene.center = scene.mouse.pos


scene.bind('mouseup', center_update)


class OHTSignalViewer(OHTSignalLogger):
    @classmethod
    def init(cls, group):
        cls.group = group
        cls.time_start = datetime.now()
        cls.flags = cls.groups[group]
        cls.graphs = {}
        cls.plots = {}
        cls.plot_data = {}
        cls.time_axis = [0]
        for flag in cls.flags:
            cls.graphs[flag] = graph(title=flag, ymin=0, ymax=1,  height=80)
            cls.plots[flag] = gcurve(graph=cls.graphs[flag], color=color.blue)
            cls.plot_data[flag] = [0]
            cls.plots[flag].data = [(0,0)]
            sleep(0.05)

    @classmethod
    def time_update(cls):
        time_diff = datetime.now() - cls.time_start
        cls.time_axis.append(time_diff.total_seconds())

    @classmethod
    def update(cls, state):
        for flag in state:
            if flag in cls.flags:
                cls.plot_data[flag].append(state[flag])

    @classmethod
    def windowing(cls):
        if len(cls.time_axis) > cls.windows_size:
            cls.time_axis.pop(0)
            for flag in cls.flags:
                cls.plot_data[flag].pop(0)

    @classmethod
    def loop(cls):
        while True:
            rate(100)
            status = cls.get_status()
            cls.time_update()
            cls.update(status['oht']['dio'])
            if 'station' in status:
                cls.update(status['station']['pio'])
            cls.windowing()
            for flag in cls.flags:
                cls.plots[flag].data = zip(cls.time_axis, cls.plot_data[flag])


if len(sys.argv) != 2 and not sys.argv[1] in OHTSignalViewer.groups:
    print("%s <flag group>   # flag group : %s,log" % (sys.argv[0], ','.join(OHTSignalViewer.groups.keys())))
    os._exit(-1)

OHTSignalViewer.init(sys.argv[1])
OHTSignalViewer.loop()
