#-*- coding:utf-8 -*-
from ecmaster.ecmaster import initRPC, StockerIO
from time import sleep

stk = StockerIO()

def motorPowerOff():
	sleep_dur = 0.1
	stk.writeDo(2,5,False,True) 
	sleep(sleep_dur)
	stk.writeDo(2,6,False,True) 

motorPowerOff()
