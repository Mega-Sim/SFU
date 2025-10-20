#-*- coding:utf-8 -*-
from ecmaster.ecmaster import initRPC, StockerIO
from time import sleep

stk = StockerIO()

stk.readIoData()

def printDi():
	print "### DI port status ####"
	for port in range(3,6):
		for bit in range(8):
			try :
				print "%d:%d=%d" % (port, bit, stk.readDi(port, bit)),
				#print "%d" % stk.readDi(port, bit),
			except:
				pass 
		print 

def printDo():
	print "### DO port status ####"
	stk.syncOutputBuffer()
	for port in range(5):
		for bit in range(8):
			try :
				#print "%d:%d=%d" % (port, bit, stk.readDo(port, bit)), 
				print "%d" % stk.readDo(port, bit),
			except:
				pass 
		print 


def changeBrakeState(on):
	print "Set Stocker Brake", "On" if on else "Off"
	stk.syncOutputBuffer()
	stk.writeDo(3,0,on)
	stk.writeDo(3,1,on)
	stk.writeDo(3,2,on)
	stk.writeDo(3,3,on)
	stk.writeIoData()
	

def motorPowerOn():
	sleep_dur = 0.5
	stk.writeDo(2,6,True,True) 
	sleep(0.1)
	stk.writeDo(2,5,True,True) 
	sleep(0.1)
	stk.writeDo(2,4,True,True) 
	sleep(sleep_dur)
	stk.writeDo(2,4,False,True) 

def motorPowerOff():
	sleep_dur = 0.1
	stk.writeDo(2,5,False,True) 
	sleep(sleep_dur)
	stk.writeDo(2,6,False,True) 
#printDo()	
#changeBrakeState(True)
printDo()
stk.printDiStates()
#motorPowerOn()
#motorPowerOff()
