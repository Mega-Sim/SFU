import os
import load_model_config
config = load_model_config.loadStockerModel()
from vpython import *

scene.width = 1024
scene.height = 768

frame_x = config["FrameSize"]["x"]
frame_y = config["FrameSize"]["y"]
frame_z = config["FrameSize"]["z"]
foup_size = 100
scene.center = vector(frame_x, frame_z, frame_y) / 2;

class Frame:
    thickness = 100

    @classmethod
    def init(cls, l, h, w):
        cls.size = vector(l, h, w)
        edges = []
        delta = Frame.thickness / 2;
        edges.append(box(pos=vector(l/2, delta, delta), size=vector(l, Frame.thickness, Frame.thickness)))
        edges.append(box(pos=vector(l/2, h, delta), size=vector(l, Frame.thickness, Frame.thickness)))
        edges.append(box(pos=vector(l/2, delta, w), size=vector(l, Frame.thickness, Frame.thickness)))
        edges.append(box(pos=vector(l/2, h, w), size=vector(l, Frame.thickness, Frame.thickness)))
        edges.append(box(pos=vector(delta, delta, w/2), size=vector(Frame.thickness, Frame.thickness, w)))
        edges.append(box(pos=vector(l, delta, w/2), size=vector(Frame.thickness, Frame.thickness, w)))
        edges.append(box(pos=vector(delta, h, w/2), size=vector(Frame.thickness, Frame.thickness, w)))
        edges.append(box(pos=vector(l, h, w/2), size=vector(Frame.thickness, Frame.thickness, w)))
        edges.append(box(pos=vector(delta, h/2, delta), size=vector(Frame.thickness, h, Frame.thickness)))
        edges.append(box(pos=vector(l, h/2, delta), size=vector(Frame.thickness, h, Frame.thickness)))
        edges.append(box(pos=vector(delta, h/2, w), size=vector(Frame.thickness, h, Frame.thickness)))
        edges.append(box(pos=vector(l, h/2, w), size=vector(Frame.thickness, h, Frame.thickness)))
        edges.append(cylinder( pos=vector(0,0,w/2), axis=vector(1,0,0), length=l, radius=Frame.thickness, color=color.blue))
        edges.append(cylinder(pos=vector(0, h, w / 2), axis=vector(1, 0, 0), length=l, radius=Frame.thickness, color=color.blue))

        # do flags
        height = -200
        cls.flags = {
            "mc": 1,
            "estop": 2,
            "int lock": 3,
            "fork load r": 6,
            "fork load l": 7,
            "door lock" : 10,
            "door close": 11,
            "empt unload": 17,
            "dbl load": 18,
            "safety plc": 20,
            "light curt": 21,
            "plc alarm" : 22
        }
        cls.flag_objects = {}
        i = 1;
        for flag in cls.flags :
            cls.flag_objects[flag] = sphere(pos=vector(-200, frame_z + height*i, frame_y), radius= 100, color = color.gray(0.5))
            label(pos=vector(-300, frame_z + height*i, frame_y), text=flag, opacity=0.3, border=None)
            i += 1

    @classmethod
    def doUpdate(cls, dout):
        for flag in cls.flags:
            cls.flag_objects[flag].color = color.green if dout & (1<<cls.flags[flag]) else  color.gray(0.5)

class Robot:
    offs_x = config["Robot"]["Offset"]["x"]
    offs_z = config["Robot"]["Offset"]["z"]
    offs_y = config["Robot"]["Size"]["fork_offset"]
    offs_r = 90
    def __init__(self, x=0,z=0,r=0,y=0, has_foup=False):
        body_size = vector(500,50,200)
        x += Robot.offs_x
        z += Robot.offs_z
        r += Robot.offs_r
        _r = r/180.0*3.1415926
        self.has_foup = has_foup
        self.size = body_size
        self.pillar = cylinder(pos = vector(x, 0, frame_y/2), axis=vector(0,1,0), length=frame_z, radius=Frame.thickness/2, color=color.orange)
        self.body = box( pos = vector(x, z, frame_y/2), size = body_size,  color = color.green)
        self.body.rotate(angle= _r, axis=vector(0, 1, 0))
        self.fork = cylinder( pos = vector(x, z, frame_y/2), length=Robot.offs_y+y, radius=50, color = color.gray(0.9))
        self.fork.rotate(angle= _r, axis=vector(0, 1, 0))
        self.foup =  sphere(pos = self.fork.pos + vector((self.fork.length-foup_size) * cos(_r),foup_size,(self.fork.length-foup_size) * sin(_r)), radius = foup_size, color= color.cyan, visible=has_foup)
        self.ang = r

    def posUpdate(self, z, x, r, y, has_foup):
        x += Robot.offs_x
        z += Robot.offs_z
        r += Robot.offs_r

        _r = radians(r)
        _r_delta = radians(r-self.ang)

        self.has_foup = has_foup
        self.pillar.pos = vector(x, 0, frame_y/2)
        self.body.pos = vector(x,z,frame_y/2)
        self.body.rotate(angle=_r_delta, axis=vector(0,1,0))
        self.fork.length = Robot.offs_y + y
        self.fork.pos = vector(x, z, frame_y / 2)
        self.fork.rotate(angle=_r_delta, axis=vector(0,1,0))
        self.foup.pos = self.fork.pos + vector((self.fork.length-foup_size) * cos(-_r), foup_size,(self.fork.length-foup_size) * sin(-_r));
        self.foup.visible = has_foup
        self.ang = r

    def crash(self):
        self.fork.color = color.red

class Shelf:
    width = config["ShelfSize"]["x"]
    height = config["ShelfSize"]["thickness"]
    depth = config["ShelfSize"]["y"]
    recess_depth = config["ShelfSize"]["blank_y"]
    recess_width = config["ShelfSize"]["blank_x"]
    pork_depth = config["ShelfSize"]["port_y"]

    def __init__(self, id, z, x, r, port = False, has_foup = False):
        x += Robot.offs_x
        z += Robot.offs_z
        self.has_foup = has_foup

        self.id = id
        y = 0; y_dir = 1
        if r > 90 : # left side shelf
            y = frame_y; y_dir = -1
        delta_x = (Shelf.width - Shelf.recess_width)/2
        shelf_color = color.white
        if port:
            shelf_color = color.magenta

        self.plate = curve( vector(x-Shelf.width/2, z, y),
                            vector(x+Shelf.width/2, z, y),
                            vector(x+Shelf.width/2, z, y+Shelf.depth*y_dir),
                            vector(x + Shelf.width / 2 - delta_x, z, y + Shelf.depth * y_dir),
                            vector(x + Shelf.width / 2 - delta_x, z, y + (Shelf.depth - Shelf.recess_depth) * y_dir),
                            vector(x + Shelf.width / 2 - delta_x - Shelf.recess_width, z, y + (Shelf.depth - Shelf.recess_depth) * y_dir),
                            vector(x + Shelf.width / 2 - delta_x - Shelf.recess_width, z, y + Shelf.depth * y_dir),
                            vector(x - Shelf.width / 2, z, y + Shelf.depth * y_dir),
                            vector(x - Shelf.width / 2, z, y),
                            radius = Shelf.height, color = shelf_color)
        self.foup = sphere(pos=vector(x,z+foup_size,y+200*y_dir), radius = foup_size, color = color.cyan, visible = self.has_foup)

    def update(self, has_foup):
        self.has_foup = has_foup
        self.foup.visible = self.has_foup

Frame.init(frame_x, frame_z, frame_y)
shelves = {}
for shelf in config["Shelves"]:
    shelves[shelf["id"]] = Shelf(shelf["id"], shelf["z"], shelf["x"], shelf["r"], shelf["id"][0]=="P")
robot = Robot()


def centerUpdate(ev):
    if scene.mouse.shift :
        scene.center = scene.mouse.pos

scene.bind('mouseup', centerUpdate)

if __name__ == "__main__":
    while True:
        rate(20)
    os._exit(0)
