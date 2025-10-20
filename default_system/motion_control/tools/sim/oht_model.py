import os
import load_model_config
config = load_model_config.loadOHTModel()
from vpython import *
from oht_map import node_data, link_data, station_data, graph, find_shortest_path, getNodePos, getLink

#scene_map = canvas(width=1200, height=600)

scene_map = scene
scene_map.width = 1200
scene_map.height = 600

def getPosOnLink(fr, to, offset):
    link = getLink(fr, to)
    percent_offset = offset / link['Distance']
    v0 = vector(*getNodePos(link["From Node"]))
    v1 = vector(*getNodePos(link["To Node"]))
    displacement = v1 - v0
    direction = displacement.norm()
    pos = v0 + direction * (displacement.mag * percent_offset)
    return pos


links = {}


# rail 표시
for i in range(len(link_data.index)):
    id,  begin_node, end_node = link_data.index[i], link_data.iat[i,0], link_data.iat[i,1]
    x0, y0, z0 = getNodePos(begin_node)
    x1, y1, z1 = getNodePos(end_node)
    c = color.white
    links[id] = curve(pos=[(x0,y0,z0), (x1,y1,z1)], color=c)
    label(pos=vector(x0,y0,0), text='%d'%begin_node, height=10)

# node display
nodelist = []
for i in range(len(node_data.index)):
    id, x, y, z = node_data.index[i], node_data.iat[i,0], node_data.iat[i,1], node_data.iat[i,2]
    nodelist.append(vector(x,y,z))
points(pos=nodelist, radius=4, color=color.cyan)

# station display
stations = {}
for i in range(len(station_data.index)):
    id, parent, next, offset, type = station_data.index[i], station_data.iat[i,1], station_data.iat[i,2], station_data.iat[i,3], station_data.iat[i,5]
    if (type == "EQ"):
        stations[id] = points(pos = getPosOnLink(parent, next, offset), radius=4, color=color.green)
        pos = getPosOnLink(parent, next, offset)
        pos.y += 200
        label(pos = pos, text='%d' % id, height=10, color=color.green)


# vehicle
class Vehicle:
    width, length, height = 200, 200, 200
    def __init__(self, x0,y0,z0=0):
        self.center = sphere(pos=vector(x0, y0, z0), radius=20, color=color.red)
        self.body = box(pos=vector(x0,y0,z0), size=vector(Vehicle.length, Vehicle.width, Vehicle.height), color=color.red, opacity=0.5)
        self.body.visible = False
        # do flags

    def setAxis(self, axis):
        self.body.axis = axis * 500

    def setPos(self, pos):
        self.body.pos = pos
        self.center.pos = pos

    def move(self, displacement):
        self.body.pos += displacement
        self.center.pos += displacement


def centerUpdate(ev):
    if scene_map.mouse.shift :
        scene_map.center = scene_map.mouse.pos


scene_map.bind(eventtype='mouseup', whattodo=centerUpdate)

if __name__ == "__main__":
    # path example
    start_node = 101
    target_node = 170
    path = find_shortest_path(start_node, target_node)
    oht0 = Vehicle(*getNodePos(start_node))
    for i in range(1, len(path)):
        # for each edge
        start = path[i - 1];
        next = path[i]
        displacement = vector(*getNodePos(next)) - vector(*getNodePos(start))
        length = displacement.mag
        direction = displacement.norm()
        oht0.setAxis(direction)
        oht0.setPos(vector(*getNodePos(start)))
        moved = 0
        dl = 20
        while moved < length:
            oht0.move(direction * dl)
            moved += dl
            rate(100)
