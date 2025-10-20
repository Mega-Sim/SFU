#!/usr/bin/env python
#-*- coding:utf-8 -*-

from ecmaster.ecmaster import initRPC, Slave, EcMaster
from ecmaster.coe_dict import COE


initRPC("localhost")
s0 = Slave(0)
s1= Slave(1)
s2= Slave(2)
s3= Slave(3)

slaves = [ s0, s1, s2, s3 ]

s0.resetAbsEncoder()


# s0.printPowerSource()
# s0.printAlarmCode()
# s0.printErrorReg()
# s0.printOverTravelReg()
#

#s.makeSwitchOnDisabled()
#s1.disableOverTravel()

#s.setNoTuneVelControlMode(1)
#s.setNoTuneLoad(2)
#s.setNoTuneLevel(0)
#s.storeServoParameter()

# s0.printOverDisplacementSetting()
# s0.setOverDisplaceDeg(10)
# s0.printOverDisplacementSetting()
# s0.enableUserParameter()


# for s in [s1, s2]:
#     #s.makeSwitchOnDisabled()
#     s.restoreServoParameter(1)
#     # s2.disableOverTravel()
#     # s2.setNoTuneVelControlMode(0)
#     # s2.setNoTuneLoad(1)
#     # s2.setNoTuneLevel(1)
#     s.storeServoParameter(1)

# s0.printElecGearRatio()
# s1.printElecGearRatio()
# s2.printElecGearRatio()
# s3.printElecGearRatio()


# for s in [s1, s2]:
#     s.setPowerSourceDC()
#     s.storeServoParameter()

# for s in slaves:
#     s.printPowerSource()

#s2.resetAbsEncoder()
#s2.disableOverTravel()
#s2.storeServoParameter()


# s3.setNoTuneVelControlMode(0)
# s3.setNoTuneLoad(1)
# s3.setNoTuneLevel(1)
# s3.storeServoParameter()
# s3.setElecGearRatio(16, 1)
# s3.printElecGearRatio()
# s3.storeServoParameter()
# s3.printElecGearRatio()
#s3.printOverTravelReg()
#s1.printAlarmCode()
#s1.printErrorReg()

#for s in slaves:
#	s.setOverDisplaceDeg(20.0)
#	s.storeServoParameter()


for s in slaves:
    s.printMaxTorque()
    #s.setMaxTorque(8000)
    #s.printAlarmCode()
    #s.printErrorReg()
 #   s.printInPositionWindowSize()
 #  s.setInPositionWindowSize(10000)
    #s.printOverDisplacementSetting()
    #s.printEncoderMode()
    #s.setEncoderMode(asIncremental=True)
    #s.setHomeOffset(0)
    #s.storeServoParameter()
    #s.printHomeOffset()
    #s.printControlAndStatusWord()
    #s.printPosActual()
    #s.printPosTarget()
    #s.clearPosLimit()
    #s.printPosLimit()
    #s.printModeOfOperation()
    pass

# s0.setHomeOffset(0xfdbb0d80)
#s1.setHomeOffset(59848389)
#s1.setHomeOffset(0)
# s2.setHomeOffset(24323571)
#s3.setHomeOffset(0xff1a4556)
#s3.setHomeOffset(94657)
#s3.printHomeOffset()

#s1.setEncoderMode(False)
#s1.storeServoParameter(1)
#s1.resetAbsEncoder()

#s.makeSwitchOnDisabled()
#s.setNoTune(1)
#   s.setNoTuneVelControlMode(0)
#   s.setNoTuneLoad(2)
#   s.setNoTuneLevel(0)
#   s.storeServoParameter()
