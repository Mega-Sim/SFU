#-*- coding:utf-8 -*-
from ecmaster.ecmaster import initRPC, StockerIO
from time import sleep

stk = StockerIO()


def motorPowerOn():
	sleep_dur = 0.5
	stk.writeDo(2,6,True,True) 
	sleep(0.1)
	stk.writeDo(2,5,True,True) 
	sleep(0.1)
	stk.writeDo(2,4,True,True) 
	sleep(sleep_dur)
	stk.writeDo(2,4,False,True) 

motorPowerOn()
