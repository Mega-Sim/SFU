/*
 * SEMES CONFIDENTIAL
 *
 * Copyright (c) 2017 SEMES Co.,Ltd.
 * All Rights Reserved.
 *
 * NOTICE: All information contained herein is, and remains the property
 * of SEMES and its suppliers, if any. The intellectual and technical concepts
 * contained herein are proprietary to SEMES and its suppliers and may be
 * covered by S.Korea and Foreign Patents, patents in process, and are
 * protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material is
 * strictly forbidden unless prior written permission is obtained from SEMES.
 */
//
// Created by master on 17. 4. 18.
//

#include "ec_servo_motor.h"
#include "ecmaster_rpc_client.h"
#include <exception>
#include <algorithm>
#include "sephi_task.h"
#include <time.h>

namespace  sephi
{
    Poco::SingletonHolder<EcMasterRpcCli> the_em_rpc_client;

#if 0
    void printbin(const char *tag, int num, const char* stat)
    {
        std::bitset<16> b(num);
        SEP_LOGT("%s.word = 0x%04x;  0b%s, %s\n", tag, num, b.to_string().c_str(), stat);
    };
#else
#define printbin(...)
#endif


    EcServoMotor::EcServoMotor()
            : ServoMotor(), msg_recvq_(nullptr), shm_(nullptr), out_pdo_(nullptr), in_pdo_(nullptr),
              initialized_(false), enabled_(false), emulation_mode_(ECAT_EMULATION_MODE), enc_offset_(0), servo_type_(1)
    {
    }

    EcServoMotor::~EcServoMotor()
    {
        finalize();
    }

    ECatSlaveMsgQ* EcServoMotor::getSendQ()
    {
        if (ECAT_EMULATION_MODE){
            throw std::runtime_error("Servo is under Emulation Mode");
        }

        static std::mutex mtx;
        static std::unique_ptr<ECatSlaveMsgQ> sendq = nullptr;
        std::lock_guard<std::mutex> guard(mtx);
        if (sendq == nullptr) {
            sendq = std::make_unique<ECatSlaveMsgQ>(EM_MSG_Q_KEY());
        }
        return sendq.get();
    }

    bool EcServoMotor::initialize(int axis, ServoOpMode mode, const char* model,  bool enable_on_init)
    {
        if (initialized_) {
            SEP_LOGT("Already initialized.\n");
        }

        // name set
        {
            char buff[16] = {0,};
            sprintf(buff, "axis_%d", axis);
            name_ = buff;
        }


        if (!emulation_mode_) {
            ECatSdoMsg::RscResponseMsg res;



            // axis id와 slave id가 동일하다고 가정 했음. 필요시에 수정할 것
            SEP_REQUIRE_RET(the_em_rpc_client.get()->requestSlave(axis, res), false);
            SEP_REQUIRE_RET(res.in_pd_offset != 0xFFFFFFFF, false);
            axis_no_ = axis;
            bus_cycle_us_ = res.bus_cycle_us;

            // channel creation
            msg_recvq_ = std::make_unique<ECatSlaveMsgQ>(EM_MSG_Q_KEY_SLAVE(axis).c_str());
            shm_ = std::make_unique<SepShm>(EM_SHM_KEY());

            SEP_REQUIRE_RET(shm_ != nullptr && msg_recvq_ != nullptr, false);
            out_pdo_ = (Cia402PdoOut *) (shm_->address + EM_SHM_SECTION_SIZE + res.out_pd_offset);
            in_pdo_ = (Cia402PdoIn *) (shm_->address + res.in_pd_offset);

            // add servo type
            char *type_state = shm_->address + shm_->size() - (4 - axis);
            //printf("type_state = %d\n", type_state);
            servo_type_ = (memcmp(type_state, "R", 1) == 0);    // 0:Yaskawa, 1:RSA
            printf("[axis_%d] servo_type_ = %s\n", axis, (servo_type_ == 0? "Yaskawa" : "RSA"));
        } else {
            EcServoEmulator::requestSlave(axis,in_pdo_, out_pdo_, pdo_mutex_);
            axis_no_ = axis;
            SEP_LOGI("Servo Motor %s is under EMULATION MODE\n", name_.c_str());
            printf("[Info] Servo Motor %s is under EMULATION MODE\n", name_.c_str());
        }

        initialized_ = true;

        ref_pos_.val =  in_pdo_->position - enc_offset_;

        // 동기 축 reset
        resetMasterAxis();

        // profile acc 설정
        /*setAccLimit(acc_range_.first, acc_range_.second);
        setVelLimit(vel_range_.first, vel_range_.second);
        if (!emulation_mode_) {
            pos_unit_t acc = acc_range_.second;
            coeSdoDownload(COE_QSTOP_DEC, 0, (uint8_t *) &acc, sizeof(acc));
        }*/
        // mode 설정
        SEP_REQUIRE_RET(setServoMode(mode),false);

       /* if (enable_on_init && (axis_no_ == 0 || axis_no_ == 1)) {
            SEP_REQUIRE_RET(enable(), false);
        }

        if(axis_no_ == 2 || axis_no_ == 3){
            SEP_REQUIRE_RET(disable(), false);
            loadHomeOffset();
            usleep(1000);
            SEP_REQUIRE_RET(enable(), false);
        }*/

        return true;
    }

    bool EcServoMotor::finalize()
    {
        if (!emulation_mode_) {
            SEP_REQUIRE_RET(the_em_rpc_client.get()->releaseSlave(axis_no_), false);
            disable();
            shutdown();

            in_pdo_ = nullptr;
            out_pdo_ = nullptr;
            if (shm_ != nullptr) shm_.reset(nullptr);
            if (msg_recvq_ != nullptr) msg_recvq_.reset(nullptr);
        }
        enabled_ = false;
        initialized_ = false;
    }

    bool EcServoMotor::clearStatusBits(uint16_t bits)
    {
        statusWord().ref() &= ~bits;
        return true;
    }

    bool EcServoMotor::setStatusBits(uint16_t bits)
    {
        statusWord().ref() |= bits;
        return true;
    }

    bool EcServoMotor::getStatusBits(uint16_t bits) {
        return (statusWord().ref() >> bits) & 0x01;
    }


    int EcServoMotor::coeSdoUpload(uint16_t index, uint16_t subindex, uint8_t* val, int len)
    {
        if (emulation_mode_) {
            throw std::runtime_error("SDO upload is not supported under Emulation mode");
        }

        ECatSdoMsg msg(ECatSdoMsg::SDO_UPLOAD, axis_no_);
        msg.sdo.index = index;
        msg.sdo.subindex = subindex;
        msg.sdo.length  = len;

        std::lock_guard<std::mutex> guard(msgq_mtx_);
        SEP_REQUIRE_RET(getSendQ()->put(msg), -1);

        ECatSdoMsg res;
        SEP_REQUIRE_RET(msg_recvq_->get(res), -1);
        SEP_REQUIRE_RET(res.command == res.RESPONSE_SDO_UPLOAD, -1);
        SEP_REQUIRE_RET(res.result != -1, -1);

        memcpy(val, res.sdo.data, res.sdo.length);
        return res.sdo.length;
    }

    bool EcServoMotor::coeSdoDownload(uint16_t index, uint16_t subindex, const uint8_t *val, int len)
    {
        if (emulation_mode_) {
            throw std::runtime_error("SDO download is not supported under Emulation mode");
        }
        ECatSdoMsg msg(ECatSdoMsg::SDO_DOWNLOAD, axis_no_);
        msg.sdo.index = index;
        msg.sdo.subindex = subindex;
        msg.sdo.length  = len;
        memcpy(msg.sdo.data, val, len);

        std::lock_guard<std::mutex> guard(msgq_mtx_);
        SEP_REQUIRE_RET(getSendQ()->put(msg), false);

        ECatSdoMsg res;
        SEP_REQUIRE_RET(msg_recvq_->get(res), false);
        SEP_REQUIRE_RET(res.command == res.RESPONSE_SDO_DOWNLOAD, false);
        return res.result != -1;
    }

    ServoOpMode EcServoMotor::getServoMode()
    {
        SEP_REQUIRE_RET(initialized_, ServoOpMode::NO_MODE);
        op_mode_ = static_cast<ServoOpMode>(in_pdo_->op_mode);
        return op_mode_;
    }

    bool EcServoMotor::setServoMode(ServoOpMode new_mode)
    {
        SEP_REQUIRE_RET(initialized_, false);
        if (emulation_mode_) {
            EcServoEmulator::setServoMode(this->axis_no_, new_mode);
        } else {
            uint8_t op_mode = static_cast<uint8_t>(new_mode);
            //SEP_REQUIRE_RET(coeSdoDownload(COE_MODE_OF_OPERATION, 0, &op_mode, 1), false);
            out_pdo_->setopmode = op_mode;  // 210201 yjchoe, pdo set operation mode
            if (op_mode_ != new_mode) { // target clear
                // wait for server state change
                usleep(50000);
                out_pdo_->velocity = 0;
                out_pdo_->position = in_pdo_->position;
                //out_pdo_->trq_offset = 0;
                //out_pdo_->vel_offset = 0;
                out_pdo_->torque = 0;
                // set reference value to new value
                ref_pos_ = in_pdo_->position - enc_offset_;

            }
            if (new_mode != ServoOpMode::CSV && new_mode != ServoOpMode::CST) {
                resetMasterAxis();
            }
        }
        op_mode_ = new_mode;

        return true;
    }

    bool EcServoMotor::setPosLimit(double min, double max)
    {
        SEP_REQUIRE_RET(initialized_, false);
        pos_unit_t _min = unit2pulse(min), _max = unit2pulse(max);
        //printf("%s axis setPosLimit %d, %d\n", name_.c_str(), _min, _max);
        if (!emulation_mode_) {
            if (servo_type_) {
                SEP_REQUIRE_RET(coeSdoDownload(COE_POS_LIMIT, 1, (uint8_t * ) & _min,sizeof(_min)), false);
                SEP_REQUIRE_RET(coeSdoDownload(COE_POS_LIMIT, 2, (uint8_t * ) & _max,sizeof(_max)), false);
            }
            // R사는 Homing 방식 변경으로 COE_HOME_OFFSET 사용하지 않음. Y사 사용 시 구현 필요
            // Y사 사용 시 Hoist 축이 변경되는 경우도 검토 필요
            /*
            if(axis_no_ == 0 || axis_no_ == 1)
            {
                SEP_REQUIRE_RET(coeSdoDownload(COE_POS_LIMIT, 1, (uint8_t * ) & _min,sizeof(_min)), false);
                SEP_REQUIRE_RET(coeSdoDownload(COE_POS_LIMIT, 2, (uint8_t * ) & _max,sizeof(_max)), false);
            } else {
                int32_t home_offset_pulse = 0;
                if(!servo_type_) {
                    SEP_REQUIRE_RET(
                            coeSdoUpload(COE_HOME_OFFSET, 0, (uint8_t * ) & home_offset_pulse,
                                    sizeof(home_offset_pulse)) > 0, false);
                    //printf("home_offset_%d\n", home_offset_pulse);
                }

                int32_t new_limit_min = _min + home_offset_pulse;
                int32_t new_limit_max = _max + home_offset_pulse;

                if(new_limit_min < -(1 << 29))
                    new_limit_min = -(1 << 29);
                if(new_limit_max > (1<<29)-1)
                    new_limit_max = (1<<29)-1;

                SEP_REQUIRE_RET(coeSdoDownload(COE_POS_LIMIT, 1, (uint8_t *) &new_limit_min, sizeof(new_limit_min)),
                                false);
                SEP_REQUIRE_RET(coeSdoDownload(COE_POS_LIMIT, 2, (uint8_t *) &new_limit_max, sizeof(new_limit_max)),
                                false);

                //printf("Homeoffset limit %d,%d,%d\n", home_offset_pulse, new_limit_min, new_limit_max);

//            int32_t user_param = 1;
//            SEP_REQUIRE_RET(coeSdoDownload(COE_USER_PARAM_CONFIG, 0, (uint8_t*)&user_param, sizeof(user_param)), false);
//            int32_t dat = 0x65766173; //evas
//            SEP_REQUIRE_RET(coeSdoDownload(COE_STORE_PARAM, 3, (uint8_t * ) & dat, sizeof(dat)), false);
            }
            */
        } else {
            setReferenceCSP(min);
        }
        parent::setPosLimit(min, max);
        return true;
    }

    bool EcServoMotor::setVelLimit(double min, double max)
    {
        SEP_REQUIRE_RET(initialized_, false);
        parent::setVelLimit(min, max);
        return true;
    }

    bool EcServoMotor::setAccLimit(double min, double max)
    {
        SEP_REQUIRE_RET(initialized_, false);

        acc_unit_t dec = unit2pulse(fabs(min)) * 1e-4;
        acc_unit_t acc = unit2pulse(max) * 1e-4;
        if (!emulation_mode_) {
            //SEP_REQUIRE_RET(coeSdoDownload(COE_PROF_ACC, 0, (uint8_t *) &acc, sizeof(acc_unit_t)), false);
            //SEP_REQUIRE_RET(coeSdoDownload(COE_PROF_DEC, 0, (uint8_t *) &acc, sizeof(acc_unit_t)), false); // not use, yjchoe
        }
        return parent::setAccLimit(min, max);;
    }

    bool EcServoMotor::setMaxTrq(uint16_t trq)
    {
        SEP_REQUIRE_RET(initialized_, false);
        bool rtn = false;
        uint16_t trqlimit_n = 0, trqlimit_p = 0;
        int32_t trqval_n = 0, trqval_p = 0;

        if (!emulation_mode_) {
            if(!servo_type_) {
                SEP_LOGT("Yaskawa setMaxTrq is called!");
                trqlimit_p = 0x2402;
                trqlimit_n = 0x2403;
            } else {
                SEP_LOGT("RSA setMaxTrq is called!");
                trqlimit_p = 0x2407;
                trqlimit_n = 0x2408;
            }
            // set torque limit
            SEP_REQUIRE_RET(coeSdoDownload(trqlimit_p, 0, (uint8_t*) &trq, sizeof(trq)), false);
            SEP_REQUIRE_RET(coeSdoDownload(trqlimit_n, 0, (uint8_t*) &trq, sizeof(trq)), false);
        }
        return true;
    }

    bool EcServoMotor::getMaxTrq(uint16_t trq) {
        bool rtn = false;
        uint16_t trqlimit_n = 0, trqlimit_p = 0;
        int32_t trqval_n = 0, trqval_p = 0;

        if (!emulation_mode_) {
            if(!servo_type_) {
                SEP_LOGT("Yaskawa setMaxTrq is called!");
                trqlimit_p = 0x2402;
                trqlimit_n = 0x2403;
            } else {
                SEP_LOGT("RSA setMaxTrq is called!");
                trqlimit_p = 0x2407;
                trqlimit_n = 0x2408;
            }
            // get torque limit
            coeSdoUpload(trqlimit_n, 0, (uint8_t*)&trqval_n, sizeof(trqval_n));
            coeSdoUpload(trqlimit_p, 0, (uint8_t*)&trqval_p, sizeof(trqval_p));
            // check limit
            if(trqval_p == trq && trqval_n == trq)  rtn = true;
            SEP_LOGT("Get Max Torque Positive[%d], Negative[%d], rtn[%d]", trqval_p, trqval_n , rtn);
        }
        return rtn;
    }


    bool EcServoMotor::setToleranceMargin(ControlTarget mode, double new_val)
    {
        SEP_REQUIRE_RET(initialized_, false);
        if (emulation_mode_) return  true;
        if (mode == ControlTarget::POS) {
            pos_unit_t w = unit2pulse(new_val);
            SEP_REQUIRE_RET(coeSdoDownload(COE_POS_WINDOW, 0, (uint8_t*)&w, sizeof(pos_unit_t)), false);
        } else if (mode == ControlTarget::VEL) {
            vel_unit_t w = unit2pulse(new_val);
            SEP_REQUIRE_RET(coeSdoDownload(COE_VEL_WINDOW, 0, (uint8_t*)&w, sizeof(vel_unit_t)), false);
        } else {
            SEP_REQUIRE_IMPL_RET(false);
        }
        return true;
    }

    bool EcServoMotor::waitForStateReady(int state1) 
   {
        clock_t start, end;
        start = clock();

        Cia402StatusWord *stat = (Cia402StatusWord*)&state1;
        //SEP_LOGT("[0917] waiting for new state %s\n",  stat->getStateString(state1));
        //printbin("#user in pdo", int(statusWord()), statusWord().getStateString());
        int cnt = 1000;

        while(cnt > 0) {
            auto state = statusWord().getState();

            //printbin("#user in pdo", int(statusWord()), statusWord().getStateString());
            if (state == state1 ) {
		        //SEP_LOGT("[0917] state changed : %d", 5000-cnt);
                return true;
            }

            cnt--;
            if (cnt == 0)
            {
		        //SEP_LOGT("[0917] fail to change state : %02X [%02X]", state1, state);
                return false;
            }

            usleep(bus_cycle_us_);
        }
	    //SEP_LOGT("state1 = %d, state = %d, contorl = %d", state1, statusWord().getState(), controlWord().command);
        end = clock();
        SEP_LOGT("[0917] waitForStateReady_time = %d\n", (end-start));
    }

    bool EcServoMotor::clearFaults()
    {
        SEP_REQUIRE_RET(initialized_, false);
        if (emulation_mode_) {
            statusWord().fault = 0;
            return true;
        }

        int timeout_us = COE_SDO_TIMEOUT_MS * 1000;
        printbin(">> user in pdo", int(statusWord()), statusWord().getStateString());
        if (statusWord().fault) {
            SEP_LOGT("%d tring to FAULT_RESET\n", axis_no_);
        }
        //controlWord().command = Cia402ControlWord::SHUTDOWN;	// yjchoe
        controlWord().command &= ~(Cia402ControlWord::FAULT_RESET);	// 210604 yjchoe, shutdown -> servo off
        usleep(bus_cycle_us_*100);
         //SEP_LOGT("Fault set-1\n");
        while(statusWord().fault && timeout_us > 0) {
            controlWord().command = Cia402ControlWord::FAULT_RESET;
            usleep(bus_cycle_us_*10);
            timeout_us -= bus_cycle_us_;
            printbin("user in pdo", int(statusWord()), statusWord().getStateString());
        }
        //SEP_LOGT("Fault reset-2\n");
        return !statusWord().fault;
    }

    // control
    bool EcServoMotor::enable()
    {
        clock_t start, end;
        start = clock();
        SEP_REQUIRE_RET(initialized_, false);
        if (emulation_mode_) {
            enabled_ = true;
            return true;
        }

		// for test, 210623 yjchoe
        int16_t svoffdelay = 100;
		//coeSdoDownload(0x2501, 0, (uint8_t *) &svoffdelay, sizeof(svoffdelay));

        auto printState = [this](const char* msg) {
            std::bitset<16> b(*(uint16_t *)in_pdo_);
            SEP_LOGT("%s [%d].state = %s; 0b%s\n", msg, axis_no_, statusWord().getStateString(), b.to_string().c_str());
        };

        //SEP_LOGD("EcServoMotor(%s)::enable : position = %d", name_.c_str(), in_pdo_->position);

        //setOperationMode(op_mode);
        //SEP_LOGT("bus cycle us = %d\n", bus_cycle_us_);
        usleep(bus_cycle_us_*2);

        auto state_ = statusWord().getState();
        //printState("begin");

        if (state_ == Cia402StatusWord::OPERATION_ENABLED) {
            enabled_ = true;
            return true;
        }

        // clear fault
        //printState("check fault");
        SEP_REQUIRE_RET(clearFaults(), false);

        // reset reference to prevent position mismatch
        //usleep(2000);
        ref_pos_.val =  in_pdo_->position - enc_offset_;
        out_pdo_->position = in_pdo_->position;
        out_pdo_->velocity = 0;
        out_pdo_->torque = 0;

        for(int i = 0; i<5;i++)
        {
            state_ = statusWord().getState();
            //printState("check switch on");
            if (state_ != Cia402StatusWord::SWITCHED_ON) {
                //printState("check ready to switch on");
                if (state_ != Cia402StatusWord::READY_TO_SWITCH_ON) {
                    //printState("wait for SWITCH_ON_DISABLED");
                    // Wait for status change
                    SEP_REQUIRE_RET(waitForStateReady(Cia402StatusWord::SWITCH_ON_DISABLED), false);

                    //printState("wait for READY_TO_SWITCH_ON");
                    // Shutdown
                    controlWord().command = Cia402ControlWord::SHUTDOWN;
                    SEP_REQUIRE_RET(waitForStateReady(Cia402StatusWord::READY_TO_SWITCH_ON), false);

                    // Switch on
                    controlWord().command = Cia402ControlWord::SWITCH_ON;

                    //printState("wait for SWITCHED_ON");
                    // Note: Automatic transition to Enable Operation state after executing SWITCHED ON state functionality.
                    SEP_REQUIRE_RET(waitForStateReady(Cia402StatusWord::SWITCHED_ON), false);
                }
                else
                {
                    // Switch on
                    controlWord().command = Cia402ControlWord::SWITCH_ON;

                    //printState("wait for SWITCHED_ON");
                    // Note: Automatic transition to Enable Operation state after executing SWITCHED ON state functionality.
                    SEP_REQUIRE_RET(waitForStateReady(Cia402StatusWord::SWITCHED_ON), false);
                }
                state_ = statusWord().getState();
            }

            //printState("wait for OPERATION_ENABLED");
            if(abs(in_pdo_->position - out_pdo_->position) >= 100) {
                SEP_LOGT("%d Different position! [%d/%d]\n", axis_no_, in_pdo_->position, out_pdo_->position);
                controlWord().command = Cia402ControlWord::SHUTDOWN;
                return false;
            }

            // Set operation enabled
            controlWord().command = Cia402ControlWord::ENABLE_OP;
            SEP_LOGT("%d Wait for state to be OPERATION_ENABLED\n", axis_no_);
            if(waitForStateReady(Cia402StatusWord::OPERATION_ENABLED) ==  true)
            {
                SEP_LOGT("%d complete_OPERATIONENABLE____%d", axis_no_, i);
                //if( this->axis_no_ == 1)    //CIS
                //    setMasterAxis(0);
                break;
            }
    //              usleep(1000000);
            SEP_LOGT("%d retry_OPERATIONENABLE____%d", axis_no_, i);
        }
        //printState("done");

        enabled_ = true;
        SEP_LOGD("EcServoMotor(%s)::enable : enc_offset = %d, in[%d], out[%d]", name_.c_str(), enc_offset_, in_pdo_->position, out_pdo_->position);

        end = clock();
        SEP_LOGT("Axis[%d] enable_time = %d",axis_no_, (end-start));
        disableStatus = false;  // 210806 yjchoe
        return true;
    }

    bool EcServoMotor::disable()
    {
        clock_t start, end;
        if(disableStatus)   return true;    // 210806 yjchoe

        start = clock();
        SEP_REQUIRE_RET(initialized_, false);

        resetMasterAxis();
        stop();
        int count = 0;

        while(true){

        	if(statusWord().getState()== Cia402StatusWord::SWITCH_ON_DISABLED)
		{
        	    break;
		}

        	count++;

                if(count >= 10)
	    	{
        		break;
        	}
        
        	usleep(1000);

        }	 

        enabled_ = false;

        end = clock();
        SEP_LOGT("[0917] Axis[%d] disable_time = %d\n",this->axis_no_, (end-start));
        disableStatus = true;
        return true;
    }


    bool EcServoMotor::isEnabled()
    {
        enabled_ = statusWord().getState() == ServoState::OPERATION_ENABLED;
        return enabled_;
    }

    bool EcServoMotor::shutdown()
    {
        SEP_REQUIRE_RET(initialized_, false);
        SEP_REQUIRE_RET(!statusWord().fault, false);
        resetMasterAxis();
        // quick stop to make switch on disable state.
        stop();
        return true;
    }

    ServoState EcServoMotor::getServoState()
    {
        SEP_REQUIRE_RET(initialized_, ServoState::UNKNOWN_STATE);
        if (emulation_mode_){
            return ServoState::OPERATION_ENABLED;
        }
        return statusWord().getState();
    }

    bool EcServoMotor::stop()
    {
        SEP_REQUIRE_RET(initialized_, false);
        //controlWord().command = Cia402ControlWord::QUICK_STOP;
        controlWord().command = Cia402ControlWord::SHUTDOWN;
        enabled_ = false;

	    SEP_LOGT("[0917] Axis[%d] stop called",axis_no_);   //CIS

        return true;
    }

    
    bool EcServoMotor::brakeOn()
    {
        SEP_REQUIRE_RET(initialized_, false);
        SEP_REQUIRE_IMPL_RET(false);
    }

    bool EcServoMotor::brakeOff()
    {
        SEP_REQUIRE_RET(initialized_, false);
        SEP_REQUIRE_IMPL_RET(false);
    }

    bool EcServoMotor::isBrakeOn()
    {
        SEP_REQUIRE_RET(initialized_, false);
        SEP_REQUIRE_IMPL_RET(false);

    }

    bool EcServoMotor::setHome()
    {
        SEP_REQUIRE_RET(initialized_, false);
        home_pos_ = getActual()[0];
        return true;
    }

    bool EcServoMotor::isHome()
    {
        SEP_REQUIRE_RET(initialized_, false);
        auto margin = tolerance_[static_cast<int>(ControlTarget::POS)]; // margin
        auto pos = getActual()[0];
        return fabs(home_pos_-pos) <= margin;
    }
    

    void EcServoMotor::printInpdoPos() {
        //printf("in_pdo_->position; ____ %d\n", in_pdo_->position);
        //printf("out_pdo_->position; ____ %d\n", out_pdo_->position);
        //printf("-; ____ %d\n", out_pdo_->position - in_pdo_->position);
    }

    bool EcServoMotor::adjustingCommand(uint8_t AxisType)
    {
    	int nCount = 0;
        int32_t user_param = 1;
        SEP_REQUIRE_RET(coeSdoDownload(COE_USER_PARAM_CONFIG, 0, (uint8_t *) &user_param, sizeof(user_param)),
                        false);
        do{
            ref_pos_.val = 0;
            out_pdo_->position = 0;

            if(nCount > 1000) break;

            nCount++;

            usleep(1000);
        }
        while(in_pdo_->position != 0);
	
	    SEP_LOGT("[0917] Axis[%d] adjustingCommand done",axis_no_);

        return true;
    }

    bool EcServoMotor::adjustingCommandRSA(uint8_t AxisType)
    {
        bool enabled = enabled_;
        bool homingflag = false;
        // Clear Fault
        if(statusWord().fault) {
			SEP_LOGT("[Homing] %d Detect Fault and Clear!, ErrCode : %d", axis_no_, in_pdo_->error_code);
			clearFaults();
		}

        if (enabled) {
            disable();     // Servo Off
            usleep(100000);     // Servo off delay time
        }

        out_pdo_->position = ref_pos_.val = in_pdo_->position;

		SEP_LOGT("[Homing] %d Update Target, In[%d] Out[%d] ref[%d] Vel[%d] Err[%d]",
                 axis_no_, in_pdo_->position, out_pdo_->position, ref_pos_.val, in_pdo_->velocity, in_pdo_->error_code);

		// SDO Homing for RSA
        int32_t user_param = 1;
        int sdo_index = 0;
        if(AxisType == SINGLE_SERVO)	        sdo_index = 0x3001; // 단축
        else if(AxisType == DUAL_SERVO)			sdo_index = 0x2C1C; // 다축(+3C1C)
        coeSdoDownload(sdo_index, 0, (uint8_t *) &user_param, sizeof(user_param));

        for (int i = 0; i < 1000 ; i++) {
            // servo off check bit
            if(statusWord().getBit(12) == 1) {
                homingflag = true;
                SEP_LOGT("[Homing] %d Detect homing reached!,  reached time[%d]", axis_no_, i);
                usleep(10000);     // SDO Download delay time
                out_pdo_->position = ref_pos_.val = in_pdo_->position;
                break;
            } else {
                if(i >= 999) {
                    homingflag = false;
                    bool pos_match = false;
                    if (in_pdo_->position != out_pdo_->position) {  // fail인데 encoder가 reset되는 케이스를 대비
                        out_pdo_->position = ref_pos_.val = in_pdo_->position;
                        pos_match = true;
                    }
                    SEP_LOGT("[Homing] %d Not detect homing reached! Over time! %d", axis_no_, pos_match);
                }
            }
            usleep(1000);
        }

		SEP_LOGT("[Homing] %d After Homing, In[%d] Out[%d] ref[%d] Vel[%d] Err[%d] [%d]",
                 axis_no_, in_pdo_->position, out_pdo_->position, ref_pos_.val, in_pdo_->velocity, in_pdo_->error_code, homingflag);

        return homingflag;
        /*if(abs(in_pdo_->position) < 10 && homingflag == true) {
            enabled = enabled_;
            if (enabled) enable();
            SEP_LOGT("[Homing] Completed!, In[%d] Out[%d] ref[%d] Vel[%d] Err[%d]",
                    in_pdo_->position, out_pdo_->position, ref_pos_.val, in_pdo_->velocity, in_pdo_->error_code);
            return true;
        } else {
            SEP_LOGT("[Homing] fail!, In[%d] Out[%d] ref[%d] Vel[%d] Err[%d]",
                    in_pdo_->position, out_pdo_->position, ref_pos_.val, in_pdo_->velocity, in_pdo_->error_code);
            return false;
        }*/
    }

    bool EcServoMotor::loadHomeOffset()
    {
//        bool enabled = enabled_;
//        if (enabled) disable();

        char filenamebuf[512] = {0};
        sprintf(filenamebuf, "../../../home_offset_%d.txt", axis_no_);
        FILE *home_offset_file;
        if((home_offset_file = fopen(filenamebuf, "r")) == nullptr)
        {
            printf("home_offset_file does not exist\n");
            return false;
        }
        char buff[255];
        fgets(buff, sizeof(buff), home_offset_file);
        fclose(home_offset_file);
        int pulse = atoi(buff);
        int32_t user_param = 1;
        if(!servo_type_) {
            SEP_REQUIRE_RET(coeSdoDownload(COE_HOME_OFFSET, 0, (uint8_t *) &pulse, sizeof(pulse)), false);
            SEP_REQUIRE_RET(coeSdoDownload(COE_USER_PARAM_CONFIG, 0, (uint8_t * ) & user_param, sizeof(user_param)), false);
			
			in_pdo_->position = 0;
			ref_pos_.val = 0;
			out_pdo_->position = 0;
        }

        return true;
    }

    // soft - 0: ABS, 1:INC
    // AxisType - 0: Single, 1: Dual
    bool EcServoMotor::resetEncoder(bool soft, uint8_t AxisType)
    {
        int32_t loopCnt = 0;
        clock_t start, end;
        start = clock();
        int nCount = 0;
        bool enabled = enabled_;
        bool rtn = false;
        int HMCnt = 0;

        SEP_REQUIRE_RET(initialized_, false);
        if (soft) { // Make current pulse to be read zero
            if(!servo_type_) {
                SEP_LOGT("Yaskawa resetEncoder(INC) is called!");
                enabled = enabled_;
                if (enabled) disable();
                rtn = adjustingCommand(AxisType);
                if (enabled) enable();
            } else {
                rtn = adjustingCommandRSA(AxisType);
            }
        }
        else {
            int32_t home_offset_pulse = 0;
            int32_t new_home_offset_pulse = -in_pdo_->position;
            int32_t user_param = 1;
            if(!servo_type_) {
                SEP_LOGT("Yaskawa resetEncoder(ABS) is called!");
                SEP_REQUIRE_RET(coeSdoUpload(COE_HOME_OFFSET, 0, (uint8_t *) &home_offset_pulse, sizeof(home_offset_pulse)) > 0, false);
                new_home_offset_pulse += home_offset_pulse;
                SEP_REQUIRE_RET(coeSdoDownload(COE_HOME_OFFSET, 0, (uint8_t *) &new_home_offset_pulse, sizeof(new_home_offset_pulse)), false);
                SEP_REQUIRE_RET(coeSdoDownload(COE_USER_PARAM_CONFIG, 0, (uint8_t *) &user_param, sizeof(user_param)), false);
                in_pdo_->position = ref_pos_.val = out_pdo_->position = 0;
                rtn = true;
            } else {
                SEP_LOGT("RSA resetEncoder(ABS) is called!");
                //SEP_REQUIRE_RET(coeSdoDownload(COE_HOME_OFFSET, 0, (uint8_t *) &new_home_offset_pulse, sizeof(new_home_offset_pulse)), false);
                rtn = adjustingCommandRSA(AxisType);
            }

            char filenamebuf[512] = {0};
            sprintf(filenamebuf, "../../../home_offset_%d.txt", axis_no_);

            FILE* home_offset_file = fopen(filenamebuf, "w");
            fprintf(home_offset_file, "%d\n", new_home_offset_pulse);
            fclose(home_offset_file);

            //enabled = enabled_;
            if (enabled && rtn) enable();

            SEP_LOGD("EcServoMotor(%s)::enable : position = %d", name_.c_str(), in_pdo_->position);
        }

	    end = clock();
        SEP_LOGT("%d resetEncoder_time = %d\n", axis_no_, (end-start));
        return rtn;
    }

    bool EcServoMotor::setOverloadCondition(double over_trq_duration, double following_error_tolerance, double near_zero_speed)
    {
        overtrq_counter_.reset(int(over_trq_duration * 1000));
        follow_error_tolerance_ = following_error_tolerance;
        near_zero_velocity_ = near_zero_speed;
        return true;
    }

    int EcServoMotor::checkOverload()
    {
        // 과전류 체크
        bool overtrq = (in_pdo_->torque > trq_range_.second) || (in_pdo_->torque < trq_range_.first);
        overtrq_counter_.write(overtrq);
        if (overtrq_counter_.counter() == overtrq_counter_.size() ) {
            //SEP_LOGE("axis[%d] E_SERVO_OVERLOAD!\n", axis_no_);
            return E_SERVO_OVERLOAD;
        }
        // follow error check
        auto pos_error = getPosError();
        auto vel = pulse2unit(in_pdo_->velocity);
        


        if(axis_no_ == 0)
        {
      //     SEP_LOGE("axis[%d] E_FOLLOW_ERROR! pos error = %lf, vel = %lf\n", axis_no_, pos_error, vel);
            if (pos_error > 84*100)
            {
                //SEP_LOGE("axis[%d] E_FOLLOW_ERROR! pos error = %lf vel = %lf\n", axis_no_, pos_error, vel);
                return E_FOLLOW_ERROR;
            }
        }
      //  if(axis_no_ == 2)
      //  {
           // SEP_LOGE("axis[%d] E_FOLLOW_ERROR! pos error = %lf, vel = %lf\n", axis_no_, pos_error, vel);
           // if (pos_error > 11*100)
           // {
           //   SEP_LOGE("axis[%d] E_FOLLOW_ERROR! pos error = %lf vel = %lf\n", axis_no_, pos_error, vel);
           //     return E_FOLLOW_ERROR;
           // }
     //   }
      //  if(axis_no_ == 3)
      //  {
        //  SEP_LOGE("axis[%d] E_FOLLOW_ERROR! pos error = %lf, vel = %lf\n", axis_no_, pos_error, vel);
        //  if(pos_error > 11*00)
        //  {
        //  SEP_LOGE("axis_[%d] E_FOLLOW_ERROR! pos error = %lf, vel = %lf\n", axis_no_, pos_error, vel);
        //  return E_FOLLOW_ERROR;
       //   }
      
       
        return 0;
    }

    bool EcServoMotor::setReferencePP(double p, double* v, double* a, bool immediate) // PP{p, vmax, accmax},
    {
        SEP_REQUIRE_RET(initialized_ && op_mode_ == ServoOpMode::PP, false);
        if (emulation_mode_) {
            throw std::runtime_error("PP is not supported under Emulation mode");
        }
        SEP_REQUIRE_RET(statusWord().fault==0, false);
        //out_pdo_->prof_velocity = (v) ? unit2pulse(*v) : unit2pulse(vel_range_.second);
        out_pdo_->position = unit2pos(p).low() + enc_offset_;

        SEP_LOGT("prof vel = %f deg/s, target = %f deg\n", *v, p);

        const int PP_TARGET_ACKNOWLEDGE_BIT = 12;
        statusWord().setBit(PP_TARGET_ACKNOWLEDGE_BIT, false);
        controlWord().command |= (1 << 4);
        if (immediate) {
            controlWord().command |= ((1<<5) | (1<<9));
        } else {
            controlWord().command &= ~((1<<5) | (1<<9));
        }

        int timeout_us = COE_SDO_TIMEOUT_MS * 1000;
        while(!statusWord().getBit(PP_TARGET_ACKNOWLEDGE_BIT) && timeout_us > 0) {
            usleep(bus_cycle_us_);
            timeout_us -= bus_cycle_us_;
        }
        controlWord().command &= ~(1 << 4);
        statusWord().target_reached = 0; // in_pdo 수신 전에 체크가 일어 날 수 있으므로 clear해 줌
        return statusWord().getBit(PP_TARGET_ACKNOWLEDGE_BIT);
    }

    bool EcServoMotor::setReferenceCSP(double p, double* voffs, double* toffs) //  CSP{p, voff, toff}
    {
        SEP_REQUIRE_RET(initialized_ && op_mode_ == ServoOpMode::CSP, false);
        SEP_REQUIRE_RET(statusWord().fault==0, false);
        ref_pos_ = unit2pos(p);
        if (emulation_mode_) {
            pdo_mutex_->lock();
        }

        out_pdo_->position = ref_pos_.low() + enc_offset_;

        //if (voffs) out_pdo_->vel_offset = unit2pulse(*voffs);
        //if (toffs) out_pdo_->trq_offset =  trq_unit_t(*toffs);
        if (emulation_mode_) {
            in_pdo_->position = out_pdo_->position;
            pdo_mutex_->unlock();
        }
        return true;
    }

    bool EcServoMotor::setReferenceCSV(double v, double* voffs, double* toffs) // CSV
    {
        //SEP_REQUIRE_RET(initialized_ && op_mode_ == ServoOpMode::CSV, false);
        SEP_REQUIRE_RET(statusWord().fault==0, false);
        if (emulation_mode_) {
            pdo_mutex_->lock();
        }

        out_pdo_->velocity = unit2pulse(v);

        if (-EC_SPEED_EPSILON < v && v < EC_SPEED_EPSILON) {
            //SEP_LOGT("vel = %d / %f\n", out_pdo_->velocity, v);
            out_pdo_->velocity = 0;
        }
        //if (voffs) out_pdo_->vel_offset = unit2pulse(*voffs);
        //if (toffs) out_pdo_->torque = trq_unit_t(*toffs);
        if (emulation_mode_) {
            in_pdo_->velocity = out_pdo_->velocity;
            pdo_mutex_->unlock();
        }
        return true;
    }

    const Vector3& EcServoMotor::getActual() // pos, vel, acc
    {
        if (initialized_) {
            if (emulation_mode_) {
                pdo_mutex_->lock();
            }
            int32_t pos_pulse = in_pdo_->position - enc_offset_;
            if (op_mode_ == ServoOpMode::CSP) {
                PosUnit pos(ref_pos_.high(), pos_pulse);
                // detect sign change
                if ( (ref_pos_.val - pos.val) > (1<<30) || (pos.val - ref_pos_.val ) > (1<<30) ) { // high 32bit sign과 low 32bit sign이 달라 튀는 경우임
                    pos.val = pos_pulse;
                }
                actual_[0] = pos2unit(pos);
            } else {
                actual_[0] = pulse2unit(pos_pulse);
            }
            actual_[1] = pulse2unit(in_pdo_->velocity);
            actual_[2] = in_pdo_->torque; // todo: torque conversion이 필요하면 그때 사용할 것
            if (emulation_mode_) {
                pdo_mutex_->unlock();
            }
        }
        return this->actual_;
    }

    double EcServoMotor::getPosError()
    {
        // CSP 일 때만 유효함; multi turn에 의해 생기는 문제는... 2^11바퀴나 차이나는 경우는 없겠지?
        //return (op_mode_ == ServoOpMode::CSP) ? pulse2deg(out_pdo_->position - in_pdo_->position) : pulse2unit(in_pdo_->following_error);
        return pulse2unit(in_pdo_->following_error);
    }

    double EcServoMotor::getVelError()
    {
        // CSV 일 때만 유효함
        return (op_mode_ == ServoOpMode::CSV) ? pulse2unit(out_pdo_->velocity - in_pdo_->velocity) : 0;
    }

    double EcServoMotor::getActPos()
    {
        return in_pdo_->position;
    }

    double EcServoMotor::getActVel()
    {
        return in_pdo_->velocity;
    }

    double EcServoMotor::getRefPulse()
    {
        return out_pdo_->position;
    }

    double EcServoMotor::getRawActErr()
    {
        //return in_pdo_->following_error;
        return (getRefPulse() - getActPos());   // 220415 yjchoe, RSA에서 Following Error 오계산 방지
    }

    double EcServoMotor::getActErr()
    {
        return in_pdo_->following_error;
    }

    double EcServoMotor::getRefPos()
    {
        return pulse2unit(out_pdo_->position + enc_offset_);
    }

    double EcServoMotor::getRefVel()
    {
        // CSV 일 때만 유효함
        //return (op_mode_ == ServoOpMode::CSV)  ? pulse2unit(out_pdo_->velocity) : 0;
        return (op_mode_ == ServoOpMode::CSV || op_mode_ == ServoOpMode::CSP) ? pulse2unit(out_pdo_->velocity) : 0;
    }

    bool EcServoMotor::setMasterAxis(int slave_id)
    {
        if (emulation_mode_) {
            return EcServoEmulator::setMasterAxis(this->axis_no_, slave_id);
        }

        ECatSdoMsg msg(ECatSdoMsg::SET_MASTER_AXIS, axis_no_);
        msg.master_axis = slave_id;

        std::lock_guard<std::mutex> guard(msgq_mtx_);
        SEP_REQUIRE_RET(getSendQ()->put(msg), -1);

        ECatSdoMsg res;
        SEP_REQUIRE_RET(msg_recvq_->get(res), -1);
        SEP_REQUIRE_RET(res.command == res.RESPONSE_SET_VELOCITY_MASTER_AXIS, -1);
        SEP_REQUIRE_RET(res.result != -1, -1);
        return true;
    }

    bool EcServoMotor::resetMasterAxis()
    {
        if (emulation_mode_) {
            return EcServoEmulator::setMasterAxis(this->axis_no_, -1);
        }
        return setMasterAxis(-1);
    }

    uint16_t EcServoMotor::getErrcode()
    {
        return in_pdo_->error_code;
    }

    int32_t EcServoMotor::getLoopbackPos()
    {
        //return in_pdo_->loopback_pos;
    }
}
