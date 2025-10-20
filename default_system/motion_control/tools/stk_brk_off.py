#-*- coding:utf-8 -*-
from ecmaster.ecmaster import initRPC, StockerIO
from time import sleep

stk = StockerIO()

def changeBrakeState(on):
	print ("Set Stocker Brake", "On" if on else "Off")
	stk.syncOutputBuffer()
	stk.writeDo(3,0,on)
	stk.writeDo(3,1,on)
	stk.writeDo(3,2,on)
	stk.writeDo(3,3,on)
	stk.writeIoData()
	

changeBrakeState(True)
