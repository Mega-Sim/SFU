#-*- coding:utf-8 -*-
import sys, os, grpc
from rpc.ecmaster_pb2_grpc import EcMasterRpcStub
from rpc.ecmaster_pb2 import AdminCommand, CoeData, CoeInfo, Result, StockerDataIn, StockerDataOut
from coe_dict import *


RPC_PORT = 50051

class CoeValue:
    def __init__(self, data):
        self.type = data.type
        self.value = data.value
    def hex(self):
        if self.type in [1,2] : return '%02x' % (self.value & 0xFF) # sint8, uint8
        if self.type in [3,4]: return '%04x' % (self.value & 0xFFFF) # sint16, uint16
        if self.type in [5,6]: return '%08x' % (self.value & 0xFFFFFFFF) # sint32, uint32
    def val(self):
        if self.type in [1,2] : return (self.value & 0xFF) # sint8, uint8
        if self.type in [3,4]: return (self.value & 0xFFFF) # sint16, uint16
        if self.type in [5,6]: return (self.value & 0xFFFFFFFF) # sint32, uint32
    def int32(self):
        if self.value >= 0x80000000:
            return -(0xFFFFFFFF - self.value)-1
        else :
            return self.value

class EcMaster:
    channel = None
    stub = None

    @classmethod
    def readCoeValue(cls, id_, index_, subindex_ = 0):
        res = cls.stub.readCoeValue(CoeInfo(slave_id=id_, index=index_, subindex=subindex_))
        return CoeValue(res)
    @classmethod
    def writeCoeValue(cls, id_, index_, subindex_, type_, val_):
        return EcMaster.stub.writeCoeValue(CoeData(slave_id=id_, index=index_, subindex=subindex_, type=type_, value=val_))


def initRPC(host = "localhost", port = RPC_PORT):
    if EcMaster.channel == None :
        EcMaster.channel = grpc.insecure_channel('%s:%d' % (host,RPC_PORT))
        EcMaster.stub = EcMasterRpcStub(EcMaster.channel)

class StockerIO:
	# DI
	# z home,+,- : 3:0, 3:1, 3:2
	# x home,+,- : 3:3, 3:4, 3:5
	# r home,+,- : 3:6, 3:7, 4:0
	# y home,+,- : 4:1, 4:2, 4:3
	# SWV : 4:4
	# Limit ST1, ST2 : 4:5, 4:6
	# IO Pwr ST1,2,3,4 : 4:7, 5:0, 5:1, 5:2

	# DO
	# Z,X,R,Y Brake : 3:0, 3:1, 3:2, 3:3
	# Lim IN 1,2 : 3:4, 3:5
	# IO Pwr EN 1, 2, 3, 4 : 3:6, 3:7, 4:0, 4:1 
	 
    def __init__(self):
        initRPC()
        self.input_ = StockerDataIn()
        self.output_ = StockerDataOut()

    def readIoData(self):
        self.input_ = EcMaster.stub.stockerRead(Result(result = 0))
   #     print 'readIoData:', self.input_

    def printDiStates(self):
        portmap ={ 
            'z' : ((3,0), (3,1), (3,2)),
            'x' : ((3,3), (3,4), (3,5)),
            'r' : ((3,6), (3,7), (4,0)),
            'y' : ((4,1), (4,2), (4,3)) }
        # for k, v in portmap.iteritems() :
        #     print k, 'axis: h=%d, +=%d, -=%d' % (
        #         self.readDi(v[0][0], v[0][1]),
        #         self.readDi(v[1][0], v[1][1]),
        #         self.readDi(v[2][0], v[2][1]))

    def syncOutputBuffer(self):
        self.output_ = EcMaster.stub.stockerOutRead(Result(result=0))

    def writeIoData(self):
        res = EcMaster.stub.stockerWrite(self.output_)
    #    print 'writeIoData:', res

    def getInOffset_(self, port, bit):
        if port < 0 or port > 5:
            raise Exception("Port range error")
        if (bit < 0 or bit > 7) or (port == 5 and bit > 6) :
            raise Exception("Bit range error")
        return port*8 + bit

    def getOutOffset_(self, port, bit):
        if port < 0 or port > 4:
            raise Exception("Port range error")
        if (bit < 0 or bit > 7) or (port == 4 and bit > 5) :
            raise Exception("Bit range error")
        return port*8 + bit

    def slaveId(self):
        return self.id

    def readDi(self, port, bit, update = False):
        if update:
            self.readIoData()
        return bool(self.input_.din & 1 << self.getInOffset_(port,bit))

    def readAi(self, port, update = False):
        if port < 0 or port > 3: raise Exception("Port range error")
        if update:
            self.readIoData()
        return (self.input_.ain & (0xFFFF << (port*16))) >> (port*16)

    def readDo(self, port, bit, update = False):
        if update:
            self.syncOutputBuffer()
        return bool(self.output_.dout & 1 << self.getOutOffset_(port,bit))

    def readAo(self, port, update = False):
        if port < 0 or port > 3: raise Exception("Port range error")
        if update:
            self.syncOutputBuffer()
        return (self.output_.aout & (0xFFFF << (port*16))) >> (port*16)

    def writeDo(self, port, bit, val, update = False):
        if update:
            self.syncOutputBuffer()
    #    print "DO(%d:%d) = %d" % (port, bit, val)
        if val :
            self.output_.dout |= 1 << self.getOutOffset_(port, bit)
        else :
            self.output_.dout &= ~(1 << self.getOutOffset_(port, bit))
        if update:
            return self.writeIoData()
    def writeAo(self, port, val, update = False):
        if port < 0 or port > 3: raise Exception("Port range error")
        if update:
            self.syncOutputBuffer()
        self.output_.aout &= ~(0xFFFF << (port*16))
        self.output_.aout |=  (0xffff & val) << (port*16)
        if update:
            return self.writeIoData()

class Slave:
    def __init__(self, _id):
        self.id = _id


    def makeSwitchOnDisabled(self):
        return EcMaster.stub.makeSwitchOnDisabled(AdminCommand(slave_id=self.id, option=0))

    def storeServoParameter(self, opt = STORE_OPTION.SAVE_SERVO_PARAM):
        res = EcMaster.stub.storeParameter(AdminCommand(slave_id=self.id, option=opt))
     #   print 'Slave[%d]'% self.id,'Store servo param : ', res

    def restoreServoParameter(self, opt = STORE_OPTION.RESTORE_SERVO_PARAM):
        res = EcMaster.stub.restoreDefaultParameter(AdminCommand(slave_id=self.id, option=opt))
    #    print 'Slave[%d]'% self.id,'Restore servo param:', res

    def enableUserParameter(self):
        return EcMaster.stub.enableUserParameter(AdminCommand(slave_id=self.id, option=0))

    def printInPositionWindowSize(self):
        res = EcMaster.readCoeValue(self.id, COE.POS_WINDOW)
   #     print 'Slave[%d]'% self.id,'In-Position Window Size:', res.value

    def printMaxTorque(self):
        res = EcMaster.readCoeValue(self.id, COE.TRQ_MAX)
    #    print 'Slave[%d]'% self.id,'Max Trq:', res.val(), res.hex()

    def setMaxTorque(self, trq):
        res = EcMaster.readCoeValue(self.id, COE.TRQ_MAX)
        EcMaster.writeCoeValue(self.id, COE.TRQ_MAX, 0, res.type, trq)
     #   print 'Slave[%d]'% self.id,"Set Max Trq as ", trq

    def setInPositionWindowSize(self, pulse):
        res = EcMaster.readCoeValue(self.id, COE.POS_WINDOW)
        EcMaster.writeCoeValue(self.id, COE.POS_WINDOW, 0, res.type, pulse)
    #    print 'Slave[%d]'% self.id,"Set In-Position Window Size as ", pulse
        self.storeServoParameter(opt=STORE_OPTION.SAVE_ALL_PARAM)

    def printPosLimit(self):
        res = EcMaster.readCoeValue(self.id, COE.POS_LIMIT, 1)
        min = res.value
        res = EcMaster.readCoeValue(self.id, COE.POS_LIMIT, 2)
        max = res.value
    #    print 'Slave[%d]'% self.id,"Position Limit: %d ~ %d" %(min, max)

    def clearPosLimit(self):
        pos = EcMaster.readCoeValue(self.id, COE.POS_LIMIT, 1)        
        EcMaster.writeCoeValue(self.id, COE.POS_LIMIT, 1, pos.type, 0)
        EcMaster.writeCoeValue(self.id, COE.POS_LIMIT, 2, pos.type, 0)                
        self.storeServoParameter(opt=STORE_OPTION.SAVE_ALL_PARAM)
    #    print "Position Limit: cleared"
#

    def printHomeOffset(self):
        res = EcMaster.readCoeValue(self.id, COE.HOME_OFFSET)
    #    print 'Slave[%d]'% self.id,'Home Offset:', res.value

    def getPos(self):
        res = EcMaster.readCoeValue(self.id, COE.POS_ACTUAL)
        return res.value

    def printPosActual(self):
        res = EcMaster.readCoeValue(self.id, COE.POS_ACTUAL)
     #   print 'Slave[%d]'% self.id,'Actual Position:', res.int32(), '0x%08x' % res.value

    def printPosTarget(self):
        res = EcMaster.readCoeValue(self.id, COE.POS_TARGET)
    #    print 'Slave[%d]'% self.id,'Target Position:', res.int32(), '0x%08x' % res.value

    def setHomeOffset(self, val):
        home = EcMaster.readCoeValue(self.id, COE.HOME_OFFSET)
        EcMaster.writeCoeValue(self.id, COE.HOME_OFFSET, 0, home.type, val)
        self.storeServoParameter(opt=STORE_OPTION.SAVE_ALL_PARAM)


    def printEncoderMode(self):
        res = EcMaster.readCoeValue(self.id, COE.ABS_ENC_SETUP)
        # print 'Slave[%d]'% self.id, 'value:', res.hex(),
        # if res.value & 0x0F00 == 0x0000 :
        #     print 'Used as real encoder type. (maybe absolue mode)'
        # elif res.value & 0x0F00 == 0x0100 :
        #     print 'Used as incremental type'
        # elif res.value & 0x0F00 == 0x0200 :
        #     print 'Used as single turn absolute type'
        # else:
        #     print 'Unknown econder use mode'

    def setEncoderMode(self, asIncremental = True):
        res = EcMaster.readCoeValue(self.id, COE.ABS_ENC_SETUP)
       # print 'Slave[%d] set encoder mode as'% self.id,
        if not asIncremental:
            newval = res.value & 0xF0FF;
   #         print 'real encoder type.'
        else:
            newval = (res.value & 0xF0FF) + 0x0100;
     #       print 'incremental.'
     #   print 'Writing value: 0x%04x' % newval
        return EcMaster.writeCoeValue(self.id, COE.ABS_ENC_SETUP, 0, res.type, newval)


    def resetAbsEncoder(self):
        res =  EcMaster.stub.resetAbsEncoder(AdminCommand(slave_id=self.id, option=0))
    #    print res
        self.storeServoParameter(opt=STORE_OPTION.SAVE_ALL_PARAM)
        return res

    def printAlarmCode(self):
        val = EcMaster.readCoeValue(self.id, COE.ALARM_CODE)
    #    print 'Slave[%d]'% self.id,'Alarm code of slave %d :' %(self.id), '0x'+val.hex()

    def printModeOfOperation(self):
        val = EcMaster.readCoeValue(self.id, COE.MODE_OF_OPERATION_DISP)
    #    print 'Slave[%d]'% self.id,'Mode of Operation of slave %d :' %(self.id), val.value

    def printErrorReg(self):
        val = EcMaster.readCoeValue(self.id, COE.ERROR_REG)
    #    print 'Error Reg Value of slave %d :' %(self.id), '0x'+val.hex()

    def printControlAndStatusWord(self):
    #    print 'Slave[%d]'% self.id,
        val = EcMaster.readCoeValue(self.id, COE.CONTROL_WORD)
    #    print 'control : 0x%04x' % val.value,
        val = EcMaster.readCoeValue(self.id, COE.STATUS_WORD)
    #    print 'status : 0x%04x' % val.value

    def disableOverTravel(self):
        val = EcMaster.readCoeValue(self.id, COE.OVERTRAVEL_P)
        # if val.hex()[0] == '8':
        #     print 'OT+ Reg Value of slave %d :' % (self.id), '0x' + val.hex(), ' Already disabled.'
        # else:
        #     EcMaster.writeCoeValue(self.id, COE.OVERTRAVEL_P, 0, val.type, (val.value&0x0FFF) + 0x8000)
        #     print 'OT+ of slave %d is disabled' % self.id
        #
        # val = EcMaster.readCoeValue(self.id, COE.OVERTRAVEL_N)
        # if val.hex()[3] == '8':
        #     print 'OT- Reg Value of slave %d :' % (self.id), '0x' + val.hex(), ' Already disabled.'
        # else:
        #     EcMaster.writeCoeValue(self.id, COE.OVERTRAVEL_N, 0, val.type, (val.value&0xFFF0) + 0x0008)
        #     print 'OT- of slave %d is disabled' % self.id

    def printOverTravelReg(self):
        val = EcMaster.readCoeValue(self.id, COE.OVERTRAVEL_P)
        # print 'OT+ Reg Value of slave %d :' % (self.id), '0x' + val.hex()
        # val = EcMaster.readCoeValue(self.id, COE.OVERTRAVEL_N)
        # print 'OT- Reg Value of slave %d :' % (self.id), '0x' + val.hex()

    def printPowerSource(self):
        val = EcMaster.readCoeValue(self.id, COE.POWER_DC)
        # if val.hex()[1] == '1' :
        #     print 'Slave %d is set to use DC power' % self.id
        # else:
        #     val = EcMaster.readCoeValue(self.id, COE.POWER_3P)
        #     if val.hex()[1] == '0':
        #         print 'Slave %d is set to use 3 phase AC power' % self.id
        #     else:
        #         print 'Slave %d is set to use single phase AC power' % self.id

    def setPowerSourceDC(self):
        val = EcMaster.readCoeValue(self.id, COE.POWER_DC)
        res = EcMaster.writeCoeValue(self.id, COE.POWER_DC, 0, val.type, (val.value & 0xF0FF) + 0x0100)
     #   print 'Power source changed.'
        self.printPowerSource()

    def printOverDisplacementSetting(self):
        val = EcMaster.readCoeValue(self.id, COE.OVER_DISPLACEMENT_ALARM)
     #   print 'Slave %d) ?�치 과�? ?�차 ?�정: %d units / %f deg' % (self.id, val.value, float(val.value) / (1<<20))

    def setOverDisplaceDeg(self, deg):
        val = EcMaster.readCoeValue(self.id, COE.OVER_DISPLACEMENT_ALARM)
        EcMaster.writeCoeValue(self.id, COE.OVER_DISPLACEMENT_ALARM, 0, val.type, int(deg * (1<<20)))
        val = EcMaster.readCoeValue(self.id, COE.OVER_DISPLACEMENT_ALARM)
    #    print 'Slave %d) ?�치 과�? ?�차 New Setting: %d units / %f deg' % (self.id, val.value, val.value / (1 << 20))

    def printElecGearRatio(self):
     #   print 'Elec Gear Ratio setting (0x220E, 0x2210)'
        # val = EcMaster.readCoeValue(self.id, COE.ELEC_GEAR_NUM)
        # num = val.value
        val = EcMaster.readCoeValue(self.id, COE.ELEC_GEAR_DEN)
        den = val.value
        val = EcMaster.readCoeValue(self.id, COE.ENCODER_PULSE)
        encpulse = val.value
        # print 'Slave%d) Elec Gear : %d / %d ; %d' % (self.id, num, den, encpulse )

     #   print 'Positin Unit setting (0x2701)'
        val = EcMaster.readCoeValue(self.id, COE.POS_UNIT, 1)
        num = val.value
        val = EcMaster.readCoeValue(self.id, COE.POS_UNIT, 2)
        den = val.value
    #    print 'Slave%d) User Uinit : %d / %d' % (self.id, num, den )

    def setElecGearRatio(self, num, den):
     #   print 'Slave%d) setup electrical gear ratio (%d/%d)' %(self.id, num, den)
        EcMaster.writeCoeValue(self.id, COE.ELEC_GEAR_NUM, 0, COE_TYPE.UINT32, num)
        val = EcMaster.readCoeValue(self.id, COE.ELEC_GEAR_DEN)
        EcMaster.writeCoeValue(self.id, COE.ELEC_GEAR_DEN, 0, COE_TYPE.UINT32, den)

    #### tuning ####
    def printNoTuneReg(self):
        val = EcMaster.readCoeValue(self.id, COE.NO_TUNE)
    #    print 'No tune reg val of slave %d :' % (self.id), '0x' + val.hex()

    def setNoTune(self, mode, type = 2):
        mode = int(mode)
        if mode < 0 or mode > 1:
    #        print "No tune mode should be in [0,1]"
            return False
        val = EcMaster.readCoeValue(self.id, COE.NO_TUNE)
        EcMaster.writeCoeValue(self.id, COE.NO_TUNE, 0, val.type, (val.value & 0xFFF0) + mode)
        val = EcMaster.readCoeValue(self.id, COE.NO_TUNE_TYPE)
        EcMaster.writeCoeValue(self.id, COE.NO_TUNE_TYPE, 0, val.type, (val.value & 0xFF0F) + (type << 4))
        self.printNoTuneReg()

    def setNoTuneVelControlMode(self, mode):
        mode = int(mode)
        if mode < 0 or mode > 1:
     #       print "Vel control mode should be in [0,1]"
            return False
        val = EcMaster.readCoeValue(self.id, COE.NO_TUNE)
        EcMaster.writeCoeValue(self.id, COE.NO_TUNE, 0, val.type, (val.value & 0xFF0F) + (mode << 4))
        self.printNoTuneReg()

    def setNoTuneLevel(self, lvl = 1):
        lvl = int(lvl)
        if lvl < 0 or lvl > 7:
    #        print "Tuning level should be in [0~7]"
            return False
        val = EcMaster.readCoeValue(self.id, COE.NO_TUNE)
        EcMaster.writeCoeValue(self.id, COE.NO_TUNE, 0, val.type, (val.value & 0xF0FF) + (lvl << 8) )
        self.printNoTuneReg()
    def setNoTuneLoad(self, load = 1):
        load = int(load)
        if load < 0 or load > 2:
    #        print "Tuning level should be in [0~2]"
            return False
        val = EcMaster.readCoeValue(self.id, COE.NO_TUNE)
        EcMaster.writeCoeValue(self.id, COE.NO_TUNE, 0, val.type, (val.value & 0x0FFF) + (load << 12))
        self.printNoTuneReg()
