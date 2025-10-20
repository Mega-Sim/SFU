/*
 * SEMES CONFIDENTIAL
 *
 * Copyright (c) 2017 SEMES Co.,Ltd.
 * All Rights Reserved.
 *
 * NOTICE: All information contained herein is, and remains the property
 * of SEMES and its suppliers, if any. The intellectual and technical concepts
 * contained herein are proprietary to SEMES and its suppliers and may be
 * covered by S.Korea and Foreigner Patents, patents in process, and are
 * protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material is
 * strictly forbidden unless prior written permission is obtained from SEMES.
 */

//
// Created by sem1 on 17. 11. 13.
//
#include "time_profile.h"
#include "amc_oht.h"

extern void logStackTrace();

namespace amc
{
    Queue CQueue[4] = {0,};
    Queue* AxisQueue[4]={ &CQueue[0], &CQueue[1], &CQueue[2], &CQueue[3] };

    MQueue MCQueue[4] = {0,};
    MQueue* MAxisQueue[4]={ &MCQueue[0], &MCQueue[1], &MCQueue[2], &MCQueue[3] };

    class TimingMeasure {
    public:
        enum {TIMING_MEASURE_COUNT = 5000};
        TimingMeasure(): cycle_("cycle"), exec_{"out", "nodecnt", "movecmd", "etc"} {}
        void measureCycleTiming() {
            auto now = std::chrono::system_clock::now();
            cycle_.probeCycle(now);
        }
        void beginInterval(int i) {
            auto now = std::chrono::system_clock::now();
            exec_[i].beginInterval(now);
        }
        void endInterval(int i) {
            auto now = std::chrono::system_clock::now();
            exec_[i].endInterval(now);
        }

        void logTiming() {
            auto now = std::chrono::system_clock::now();
            if (cycle_.count < TIMING_MEASURE_COUNT) return;
            // output timing stats
            SEP_LOGT("== Status Update TIME  min / ave / max / std (us) ===");
            char buff[128];
            SEP_LOGT("%s", cycle_.statistics(buff, sizeof(buff), 1e6));
            for(int i = 0 ; i < exec_.size(); i++) SEP_LOGT("%s", exec_[i].statistics(buff, sizeof(buff), 1e6));
            cycle_.reset();
            for(auto& e : exec_) e.reset();
        }

    private:
        std::array<TimeProfileItem,4> exec_;
        TimeProfileItem cycle_;
    } tm;

    Amc4OHT::Amc4OHT()
    {
        oht_pulse_ratio_ = 1<<7; // convert encoder res from 20bit to 13bit for IPC
        emo_status = 0;
        stop_ = false;
        FlagFirstNode = 1;

        controller.initialize();
        oht_io_ = make_unique<OhtIo>(&controller);

        config = &controller.config_;

        oht_front_io = controller.front_io_.get();
        oht_rear_io = controller.rear_io_.get();
        oht_onboard_io = &controller.onboard_io_;

        front_wheel = controller.front_wheel_.get();
        rear_wheel  = controller.rear_wheel_.get();
        hoist       = controller.hoist_.get();
        slide       = controller.slide_.get();

        oht_        = {front_wheel, rear_wheel, hoist, slide};
        motors      = {oht_[0]->getMotor(0), oht_[1]->getMotor(0), oht_[2]->getMotor(0),oht_[3]->getMotor(0)};

        printf("----------- LIMITS (wheel, hoist, slide) ----------------\n");
        for(int i = 0; i < 4; ++i) {
            printf("axis :%s \n", oht_[i]->name());
            printf("\t pos limit: [%f ~ %f]mm\n", config->pos_limit[i].first, config->pos_limit[i].second);
            printf("\t vel max: %f mm/s\n", config->v_max[i]);
            printf("\t acc max, dcc max: %f, %f mm/s^2\n", config->a_max[i].second, config->a_max[i].first);
        }
		
		// susi
		//susi = make_unique<susi_atlas>();
        //susi->init();

        // Voltage Monitorung, 210618 yjchoe
        int ret = 0;
        if (pSusi == nullptr) {
            pSusi = std::make_unique<fpx::susi_atlas>();
            ret = pSusi->init();
        }
        if (pMax == nullptr) {
            pMax = std::make_unique<fpx::max11609>(pSusi);
            ret = pMax->susiinit();
        }
        // Alsa, 210830 yjchoe
        sound_ = std::make_unique <amc::Sound>();

        // init
        InitializeOhtModel();

        registerAntiCollisionControlTimer();
        drv_cmd.enable_anticollision = false;

        //cyclic interrupt start
        oht_isr_ = std::make_unique<std::thread>(&Amc4OHT::statusUpdate_thread,this);
        oht_isr_->detach();

        //oht_log_ = std::make_unique<std::thread>(&Amc4OHT::logUpdate_thread,this);
        //oht_log_->detach();  //v10229 로그서버에서 관리하는 방식으로 변경해서 필요 없음 CIS

        detectcnt = 0;
        EnableChangeNodeMode = false;   // 초기화 안하면 NodeCount 안됨
        // Read I/O Config
        //controller.loadIOConfig(Model_OHT_ID);

        // sensor power on
        InitializeIO();

        cmd_flag = false;
        cmd_delay = 0;
    }

    Amc4OHT::~Amc4OHT()
    {
        finalize();
    }


    ///CIS
    const unsigned short crc16tab[256] = {
            0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
            0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
            0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
            0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
            0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
            0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
            0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
            0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
            0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
            0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
            0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
            0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
            0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
            0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
            0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
            0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
            0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
            0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
            0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
            0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
            0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
            0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
            0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
            0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
            0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
            0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
            0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
            0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
            0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
            0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
            0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
            0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
    };

    void Amc4OHT::finalize()
    {
        printf("finalize\n");


        for(auto ax : {OHT_WHEEL_F,OHT_WHEEL_R,OHT_HOIST,OHT_SLIDE}) {
            oht_[ax]->disableServo();
        }

        if ((oht_front_io != nullptr)||(oht_rear_io != nullptr)) {
            OHT_LOGT1("Brake Enable before shutdown\n");
            writeRawDo(ioconfig_.getportid("driving_brake_release"), 0);    //writeRawDo(driving_brake_release, 0);       //DO - 54
            writeRawDo(ioconfig_.getportid("hoist_ext_brake_release"), 0);    //writeRawDo(hoist_ext_brake_release, 0);     //DO - 63
        }



        stop_ = true;

        if (oht_isr_ != nullptr) {
            usleep(2000);
            //oht_isr_->join();
        }
    }

    const MarkDetector* Amc4OHT::getDetector(int di_num)
    {
        if (di_num == Model_DRIVINGFRONTNUM) {
            return &controller.localizer_->nodeCounter();
        } else if (di_num == Model_DRIVINGPRECHECK) {
            return &controller.localizer_->nodePrecheckCounter();
        } else if (di_num == Model_TRANSFRONT) {
            return &controller.localizer_->transFrontDetector();
        } else if (di_num == Model_TRANSREAR) {
            return &controller.localizer_->transStopDetector();
        } else {
            return nullptr;
        }
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : printState
//Target System   : OHT
//Brief           : Display motion status
//Author          : Seungwan An
//Date            : 2017.10.25
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::printState()
    {
        int pos_pulse;
        double vel_rpm;

        for(auto ax : {OHT_WHEEL_F,OHT_WHEEL_R,OHT_HOIST,OHT_SLIDE}) {
            auto pos_pulse = oht_[ax]->getPosPulse();
            auto vel_pulse = oht_[ax]->getVelPulse();
            auto in_pos = motors[ax]->inPosition();
            auto vel_rpm = ( vel_pulse * 60 ) / (1<<13);
            auto motion_state = oht_[ax]->getMotionState();

            printf("motor[%d] : State = %x, Pos = %f[pulse], Vel = %f[pulse], In_Pos = %d, Flag(mo/pos/seq/src) = %d / %d / %d / %04x\n", \
            ax, motion_state, pos_pulse, vel_rpm, in_pos, \
            *(short*)&dpram_.mem[(ax*48)+8], dpram_.mem[(ax*48)+2], dpram_.mem[(ax*48)+3], *(short*)&dpram_.mem[(ax*48)+4]);
        }
        /*
        auto node_count = controller.localizer_->nodeCounter().getCount();
        auto location = controller.localizer_->getLocation();
        printf("node Count : %d, Loc From %d - To %d  Offset : %f\n", node_count, location.from, location.to, location.offset);
        printf("IO In Data %08x %08x %08x %08x\n", IO_Input[0], IO_Input[1], IO_Input[2], IO_Input[3]);
        printf("IO Out Data %08x %08x %08x %08x\n", IO_Output[0], IO_Output[1], IO_Output[2], IO_Output[3]);
        */
        printf("=================================\n");
        printf("AMC Version : %d.%02d.%02d\n", VER1, VER2, VER3);
        printf("=================================\n");
    }


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : statusUpdate_thread (from Follows_1ms)
//Target System   : OHT
//Brief           : Service Request No : 0
//                  update overall oht status every sync cycle time
//Author          : Seungwan An
//Date            : 2017.12.21
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::statusUpdate_thread()
    {
        auto wake_up_at = std::chrono::system_clock::now();
        auto duration = std::chrono::nanoseconds(OHT_ISR_CYCLETIME_NS);
        setCpuAffinity(2);
        addIntoRealtimeSchedule(USER_RT_APP_PRIO);

        while (true) {
            wake_up_at += duration;
            auto now = std::chrono::system_clock::now();
            if (stop_) break;
            if (wake_up_at > now ) {
                std::this_thread::sleep_until(wake_up_at);
            } else {
                wake_up_at = now;
            }

            tm.measureCycleTiming();
            // add you code here
            // place the follows1ms functions here
            statusUpdate1ms();

            Check_refpos();
          //  tm.logTiming();

            // mark update
            controller.localizer_->update();

            //근접제어
			setCollisionDetector();
			
			// test
			updateAntiCollisionControl_test();

			//TODO list
            //1. Power Monitoring
            //2. System Monitoring
            //3. Trace Data

            auto dur = std::chrono::system_clock::now() - wake_up_at;
			if (dur.count() > OHT_ISR_CYCLETIME_NS) {
                SEP_LOGD("Amc4OHT::statusUpdate_thread time over : %.1fms", dur.count()/1000000.0);
            }
            //OHT_LOGT1("Test Log for oht ISR\n");

        }
        SEP_LOGT("Exit OHT_ISR");
    }

    void Amc4OHT::statusUpdate1ms(void)
    {
        //tm.beginInterval(0);
        std::lock_guard<std::mutex> guard(oht_isr_lock_);
        //tm.endInterval(0);

        int axes_no = dpram_.Cmd_Dpram().Axis_Num;  //Z,X,R,W axis

        // 1ms loop count for debug
        if (Count_1ms_INT_4_Debug++ >= 2134567890)  Count_1ms_INT_4_Debug = 0;

        Check_Monitoring_System();
        //check_actpos();
        //tm.beginInterval(1);
        for(int ax = 0; ax < axes_no; ax++) {
            //In motion status
            update_control_State(ax);

            //check servo alarm
            check_Servo(ax);

            //check S/W limit
            check_SwLimit(ax);

            //check Following error
            check_Envelope(ax);

            //check in position state
            check_SWInPosition(ax);

            //check event status
            //check_Event(ax);  //다축서보팩 적용으로 사용하지 않음  CIS

            // M-Trace
            Trace_Execute(ax);

            //check_actpos(ax);

            //TODO - update target velocity
            //Pulse  : (Current Pos - Previous Pos) * (1/Sampling Time)
            //Degree : Pulse * 360 / Enc_Res
            //RPM    : Degree * 60 / 360 = Pulse * 60 / Enc_Res
            auto vel_pulse = oht_[ax]->getVelPulse();


            auto vel_rpm = ( vel_pulse * 60 );

            dpram_.map.DPRamAxisMap[ax].AD_LongType1 = 0;           //Target Velocity[13bit - pulse]
            dpram_.map.DPRamAxisMap[ax].AD_FloatType1 = vel_pulse;  //Actual velocity[13bit - pulse]

            dpram_.map.DPRamAxisMap[ax].AD_Vel = 0;                 //Target Velocity[rpm]
            dpram_.map.DPRamAxisMap[ax].AD_ActVel = vel_rpm;        //Actual velocity[rpm]

            /*OHT_LOGT("Axis[%d] mo_flag : %d / pos_flag : %d / seq_flag : %d / Source : %x\n",\
                ax, *(short*)&dpram_.mem[(ax*48)+8], dpram_.mem[(ax*48)+2], dpram_.mem[(ax*48)+3], *(short*)&dpram_.mem[(ax*48)+4]);*/
        }
        //tm.endInterval(1);
        //tm.beginInterval(2);
        update_Io_InData();
        //tm.endInterval(2);


//        SEP_LOGD("Front Wheel Status = %d\n", dpram_.map.DPRamAxisMap[OHT_WHEEL_F].AD_AxisSource & ST_AMP_POWER_ONOFF);
//        SEP_LOGD("Rear Wheel Status = %d\n", dpram_.map.DPRamAxisMap[OHT_WHEEL_R].AD_AxisSource & ST_AMP_POWER_ONOFF);
//
//        printf("Front Wheel Status = %d\n", dpram_.map.DPRamAxisMap[OHT_WHEEL_F].AD_AxisSource & ST_AMP_POWER_ONOFF);
//        printf("Rear Wheel Status = %d\n", dpram_.map.DPRamAxisMap[OHT_WHEEL_R].AD_AxisSource & ST_AMP_POWER_ONOFF);


    }


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : check_Event
//Target System   : OHT
//Brief           : handle motion by event type
//Author          : Seungwan An
//Date            : 2017.12.21
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::check_Event(int ax)
    {
        //emergent stop motion by event(limit) detection
        static bool stopFlag[4] = {false, };
        if (oht_para_.event_type[ax] == ABORT_EVENT && stopFlag[ax] == false) {
            if(sync_motion_.Flag && ((sync_motion_.Master == ax) || (sync_motion_.Slave == ax))) {
                set_abort_event(sync_motion_.Master);
                set_abort_event(sync_motion_.Slave);

                oht_[sync_motion_.Master]->stop(config->a_max[ax].first, false);
                oht_[sync_motion_.Slave]->stop(config->a_max[ax].first, false);
                stopFlag[sync_motion_.Master] = true;
                stopFlag[sync_motion_.Slave] = true;

                oht_para_.event_type[sync_motion_.Master] = oht_para_.event_type[sync_motion_.Slave] = oht_para_.event_type[ax];
            } else {
                set_abort_event(ax);

                oht_[ax]->stop(config->a_max[ax].first, false);
                stopFlag[ax] = true;
            }
        } else if (oht_para_.event_type[ax] == NO_EVENT && stopFlag[ax] == true) {  // Clear Error
			stopFlag[ax] = false;
		}
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : set_abort_event
//Target System   : OHT
//Brief           : handle motion by event type
//Author          : Seungwan An
//Date            : 2017.12.21
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::set_abort_event(int ax)
    {
        oht_[ax]->disableServo();

        dpram_.map.DPRamAxisMap[ax].AD_AxisSource |= (ST_AMP_POWER_ONOFF);
        dpram_.map.DPRamAxisMap[ax].AD_AxisSource &= (~ST_OUT_OF_FRAMES);

        //Pause.Flag[ax]=0;
        //dpram_.map.DPRamAxisMap[ax].AD_In_Pos_Flag = 1;   // delete, 210617 yjchoe
        //dpram_.map.DPRamAxisMap[ax].AD_In_Sequence = 0;
    }


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : update_control_State
//Target System   : OHT
//Brief           : Update motion control status
//                  Very similar with CheckInPos function in DSP
//Author          : Seungwan An
//Date            : 2017.12.21
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::update_control_State(int ax)
    {
        //In position status, always true when emulation mode
        //dpram_.map.DPRamAxisMap[ax].AD_In_Pos_Flag = (char)motors[ax]->inPosition();

        // In Sequence status
        int low_vel[4] = {10, 10, 180, 50};  // feedback velocity[mm/s]
        if(abs(motors[ax]->getActual()[1]) >= low_vel[ax] && !(char)motors[ax]->inPosition())
            runningflag[ax] = true;
        else
            runningflag[ax] = false;
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : check_Servo
//Target System   : OHT
//Brief           : Update servo status
//Author          : Seungwan An
//Date            : 2017.12.21
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::check_Servo(int ax)
    {
        auto servo_state = motors[ax]->getServoState();
        static int motion_state_[4] = {0, };

        // ST_AMP_FAULT - 0x0008
        if (servo_state == Cia402StatusWord::OPERATION_ENABLED) {
            //dpram_.map.DPRamAxisMap[ax].AD_AxisSource &= ~ST_AMP_FAULT;
            //dpram_.map.DPRamAxisMap[ax].AD_AxisSource &= ~ST_AMP_POWER_ONOFF;
            //dpram_.map.DPRamAxisMap[ax].AD_AxisSource &= ~ST_MUST_REBOOT;
        }
        // ST_AMP_POWER_ONOFF - 0x0800
        else if (servo_state == Cia402StatusWord::SWITCH_ON_DISABLED) {
            dpram_.map.DPRamAxisMap[ax].AD_AxisSource |= ST_AMP_POWER_ONOFF;
        }
        // ST_MUST_REBOOT - 0x8000
        else if ((servo_state == Cia402StatusWord::FAULT) 
        || (servo_state == Cia402StatusWord::FAULT_UNDER_VOLTAGE)
        || (servo_state == Cia402StatusWord::FAULT_REACTION_ACTIVE)) {
			dpram_.map.DPRamAxisMap[ax].AD_AxisSource |= ST_AMP_FAULT;
			dpram_.map.DPRamAxisMap[ax].AD_AxisSource |= ST_AMP_POWER_ONOFF;
            dpram_.map.DPRamAxisMap[ax].AD_AxisSource |= ST_MUST_REBOOT;
            oht_para_.event_type[ax] = ABORT_EVENT;
        }

        // check reset
        if (!reset_status[ax]) {
            if (oht_para_.event_type[ax] == NO_EVENT)
                SEP_LOGT("[ERROR] Axis%d Encoder reset\n", ax);
            dpram_.map.DPRamAxisMap[ax].AD_AxisSource |= ST_MUST_REBOOT;
            oht_para_.event_type[ax] = ABORT_EVENT;
        }

        // check inposition
        if (oht_[ax]->getMotionState() == Motion::IDLE) {
            motion_state_[ax]++;
        } else {
            motion_state_[ax] = 0;
        }
        // Abnormal moving speed
        int abnormalspeed[4] = {500, 500, 175000, 500};  // driving, slide : mps, hoist : pps

        if (!motors[ax]->inPosition() && motion_state_[ax] >= 10 && abs(motors[ax]->getActual()[1]) >= abnormalspeed[ax]) {
            if (oht_para_.event_type[ax] == NO_EVENT)
                SEP_LOGT("[ERROR] Axis%d Abnormal moving %d/%d/%d\n",
                        ax, motors[ax]->inPosition(), oht_[ax]->getMotionState() == Motion::IDLE, (int)motors[ax]->getActual()[1]);
            dpram_.map.DPRamAxisMap[ax].AD_AxisSource |= ST_V_LIMIT;
            dpram_.map.DPRamAxisMap[ax].AD_AxisSource |= ST_MUST_REBOOT;
            oht_para_.event_type[ax] = ABORT_EVENT;
        }
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : check_SWInPosition
//Target System   : OHT
//Brief           : update in position state
//Author          : Seungwan An
//Date            : 2017.12.21
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::check_SWInPosition(int ax)
    {
        if(motors[ax]->inPosition()) {
            dpram_.map.DPRamAxisMap[ax].AD_AxisSource |= ST_SWINPOSITION;
        } else {
            dpram_.map.DPRamAxisMap[ax].AD_AxisSource &= ~ST_SWINPOSITION;
        }
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : check_SwLimit
//Target System   : OHT
//Brief           : update S/W limit status
//Author          : Seungwan An
//Date            : 2017.10.25
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::check_SwLimit(int ax)
    {
        auto state = motors[ax]->getActual();
        auto pos_pulse = oht_[ax]->getPosPulse();
        auto pos_limit = oht_[ax]->getPosLimitPulse();
        static bool stopFlag[4] = {false, };

        //Test Code for error skip by Axis[1]
        if ( ax != OHT_WHEEL_R ) {
            //Check Negative limit
            if (pos_pulse < pos_limit.first) // min
                dpram_.map.DPRamAxisMap[ax].AD_AxisSource |= ST_X_NEG_LIMIT;
            else
                dpram_.map.DPRamAxisMap[ax].AD_AxisSource &= ~ST_X_NEG_LIMIT;

            //Check Positive limit
            if (pos_pulse > pos_limit.second) // max
                dpram_.map.DPRamAxisMap[ax].AD_AxisSource |= ST_X_POS_LIMIT;
            else
                dpram_.map.DPRamAxisMap[ax].AD_AxisSource &= ~ST_X_POS_LIMIT;

            if((dpram_.map.DPRamAxisMap[ax].AD_AxisSource & (ST_X_POS_LIMIT | ST_X_NEG_LIMIT)) && stopFlag[ax] == false) {
                OHT_LOGT1("Axis[%d] SW limit detected! Axis source[0x%x]\n", ax, dpram_.map.DPRamAxisMap[ax].AD_AxisSource & 0xffff);
                oht_para_.event_type[ax] = ABORT_EVENT;
                stopFlag[ax] = true;
            }
            else if (!(dpram_.map.DPRamAxisMap[ax].AD_AxisSource & ST_X_POS_LIMIT)
            && !(dpram_.map.DPRamAxisMap[ax].AD_AxisSource & ST_X_NEG_LIMIT)
            && stopFlag[ax] == true) {    // Clear Axis Source
                stopFlag[ax] = false;
            }
        }
    }

    void Amc4OHT::check_Envelope(int ax)
    {
		static bool errorlimit_flag[4], rawerrorlimit_flag[4] = {false, };
		static bool vellimit_flag[4] = {false, };
        static int limit_rawerr_chk[4], limit_err_chk[4], limit_vel_chk[4] = {0, };

        // Velocity Limit
        ActualVel[ax] = motors[ax]->getActual()[1];
        //if (ax == 2) ActualVel[ax] /= 350;  // hoist
        if ((abs(ActualVel[ax]) > (config->v_max[ax])) && !motors[ax]->inPosition()) {
            limit_vel_chk[ax]++;

			if (limit_vel_chk[ax] >= 5 && !vellimit_flag[ax]) {
				dpram_.map.DPRamAxisMap[ax].AD_AxisSource |= ST_V_LIMIT;
				OHT_LOGT1("Axis[%d] Velocity limit detected! vel[%.2f/%.2f] Axis source[0x%.4x]\n",
				        ax, ActualVel[ax], config->v_max[ax], dpram_.map.DPRamAxisMap[ax].AD_AxisSource);
				oht_para_.event_type[ax] = ABORT_EVENT;
				vellimit_flag[ax] = true;
			}
        } else {
            limit_vel_chk[ax] = 0;
			vellimit_flag[ax] = false;
		}
             
        // Following Error Limit - PDO Data
        if (abs(motors[ax]->getActErr()) > config->f_err[ax] && ax != 1 && runningflag[ax]) {
            limit_err_chk[ax]++;
            
            if (limit_err_chk[ax] >= 5 && !errorlimit_flag[ax]) {
                dpram_.map.DPRamAxisMap[ax].AD_AxisSource |= ST_ERROR_LIMIT;
                OHT_LOGT1("Axis[%d] Following error(PDO) limit detected! err[%.0f/%.0f] Axis source[0x%.4x]\n",
                        ax, motors[ax]->getActErr(), config->f_err[ax], dpram_.map.DPRamAxisMap[ax].AD_AxisSource);
                oht_para_.event_type[ax] = ABORT_EVENT;
                errorlimit_flag[ax] = true;
            }
        } else {
            limit_err_chk[ax] = 0;
            errorlimit_flag[ax] = false;
        }

        // Following Error Limit - Raw data
        if (abs(motors[ax]->getRawActErr()) > config->f_rawerr[ax] && ax != 1 && runningflag[ax]) {
            limit_rawerr_chk[ax]++;

            if (limit_rawerr_chk[ax] >= 5 && !rawerrorlimit_flag[ax]) {
                dpram_.map.DPRamAxisMap[ax].AD_AxisSource |= ST_ERROR_LIMIT;
                OHT_LOGT1("Axis[%d] Following error(Raw) limit detected! err[%.0f/%.0f] Axis source[0x%.4x]\n",
                          ax, motors[ax]->getRawActErr(), config->f_rawerr[ax], dpram_.map.DPRamAxisMap[ax].AD_AxisSource);
                oht_para_.event_type[ax] = ABORT_EVENT;
                rawerrorlimit_flag[ax] = true;
            }
        } else {
            limit_rawerr_chk[ax] = 0;
            rawerrorlimit_flag[ax] = false;
        }
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : update_Io_InData
//Target System   : OHT
//Brief           : update DI status
//Author          : Seungwan An
//Date            : 2017.10.25
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::update_Io_InData()
    {
        //TODO - get the IO_io data
        get_IoIn_RawData(IO_Input);


        //OHT_LOGT1("IO In Data %08x %08x %08x %08x\n", IO_Input[0], IO_Input[1], IO_Input[2], IO_Input[3]);

        dpram_.Cmd_Dpram().Io_inValue   = IO_Input[0];
        dpram_.Cmd_Dpram().Io_inValue2  = IO_Input[1];

        dpram_.Cmd_Dpram().IoinValue1_1 = IO_Input[0];
        dpram_.Cmd_Dpram().IoinValue2_1 = IO_Input[1];
        dpram_.Cmd_Dpram().IoinValue3   = IO_Input[2];
        dpram_.Cmd_Dpram().IoinValue4   = IO_Input[3];
        dpram_.Cmd_Dpram().IoinValue5   = IO_Input[4];
        dpram_.Cmd_Dpram().IoinValue6   = IO_Input[5];
        dpram_.Cmd_Dpram().IoinValue7   = IO_Input[6];
        dpram_.Cmd_Dpram().IoinValue8   = IO_Input[7];
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : get_IoIn_RawData
//Target System   : OHT
//Brief           : update DI status
//Author          : Seungwan An
//Date            : 2017.12.21
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::get_IoIn_RawData(uint32_t* data)
    {
        if (controller.getloadIOStatus()) return;
        Io_Structure io_buffer;
		//std::bitset<32> io_bit;
        for (int i = 0; i < 8; ++i) {
            //io_bit.reset();	// 비트 초기화
			//io_bit = data[i];
			io_buffer.io_all = 0;
            for (int j = 0; j < 32; ++j) {
                auto Num = i*32+j;
                if (Num > ioconfig_.getportsize("Input")) break;
                auto Portid = ioconfig_.getportid(Num);
                if (((Portid >> 24) == 1 || (Portid >> 24) == 3 || (Portid >> 24) == 5)
                && Portid != 0) {
                     //io_bit.set(j, readRawDi(Portid));
                     io_buffer.io_all |= readRawDi(Portid) << j;
                 }
            }
            //data[i] = io_bit.to_ulong();
            data[i] = io_buffer.io_all;
        }
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : get_IoOut_Data
//Target System   : OHT
//Brief           : update DO status
//Author          : Seungwan An
//Date            : 2017.12.22
//////////////////////////////////////////////////////////////////////////////////////////////////////////
 void Amc4OHT::get_IoOut_Data(uint32_t* data)
    {
        if (controller.getloadIOStatus()) return;
        Io_Structure io_buffer;
        //std::bitset<32> io_bit;
        for (int i = 0; i < 8; ++i) {
            //io_bit.reset();	// 비트 초기화
            //io_bit = data[i];
            io_buffer.io_all = 0;
            for (int j = 0; j < 32; ++j) {
                auto Num = i*32+j;
                if (Num > ioconfig_.getportsize("Output")) break;
                Num += OutOffset;
                auto Portid = ioconfig_.getportid(Num);
                if (((Portid >> 24) == 0 || (Portid >> 24) == 2 || (Portid >> 24) == 4)
                    && Portid != 0) {
                    //io_bit.set(j, readRawDo(Portid));
                    io_buffer.io_all |= readRawDo(Portid) << j;
                }
            }
            //data[i] = io_bit.to_ulong();
            data[i] = io_buffer.io_all;
        }
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : set_IoOut_Data
//Target System   : OHT
//Brief           : Write Do value
//Author          : Seungwan An
//Date            : 2017.12.22
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::set_IoOut_Data(uint32_t* data)
    {
        if (controller.getloadIOStatus()) return;
        Io_Structure io_buffer;
        //std::bitset<32> io_bit;
        for (int i = 0; i < 8; ++i) {
            //io_bit.reset();	// 비트 초기화
            //io_bit = data[i];
            io_buffer.io_all = data[i];
            // hoist brake control
            if (i == 2)
                //io_bit[31] = (motors[OHT_HOIST]->isEnabled() ? 1 : 0);
                io_buffer.bits.io_bit_30 = (motors[OHT_HOIST]->isEnabled() ? 1 : 0);
            for (int j = 0; j < 32; ++j) {
                auto Num = i*32+j;
                if (Num > ioconfig_.getportsize("Output")) break;
                Num += OutOffset;
                auto Portid = ioconfig_.getportid(Num);
                if (((Portid >> 24) == 0 || (Portid >> 24) == 2 || (Portid >> 24) == 4)
                    && Portid != 0) {
                    //writeRawDo(Portid, io_bit[j]);
                    writeRawDo(Portid, (io_buffer.io_all >> j) & 0x01);
                }
            }
            //data[i] = io_bit.to_ulong();
        }
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : set_Amp_Enable
//Target System   : OHT
//Brief           : Servo on/off control
//Author          : Seungwan An
//Date            : 2017.12.22
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    int Amc4OHT::set_Amp_Enable(int ax, bool state)
    {
        if (state == true){ //servo on
            short source_mask = (ST_MUST_REBOOT|ST_V_LIMIT|ST_AMP_FAULT|ST_X_NEG_LIMIT|ST_X_POS_LIMIT|ST_ERROR_LIMIT|ST_ENC_OVF);

            if((dpram_.map.DPRamAxisMap[ax].AD_AxisSource & source_mask) || (oht_para_.event_type[ax] != NO_EVENT))
            {
                OHT_LOGT1("Axis[%d] Enable failed, Axis source[0x%x]/State[0x%x]\n",
                        ax, dpram_.map.DPRamAxisMap[ax].AD_AxisSource, oht_para_.event_type[ax]);
                return 0;
            }

            //position limit setting
            //Already set @ amc init

            dpram_.map.DPRamAxisMap[ax].AD_AxisSource &= ~ST_AMP_POWER_ONOFF;

            //Servo On - Set the servo control register(6040, 6041)
            bool is_stop = oht_[ax]->isStopped();
            if (ax == OHT_WHEEL_R)
                is_stop = oht_[OHT_WHEEL_F]->isStopped();
            if (is_stop) {//Servo On/Off chec
                oht_[ax]->enableServo();
            }
            if(ax == OHT_HOIST){
                //OHT_LOGT1("HOIST Brake Enable\n");
                writeRawDo(ioconfig_.getportid("hoist_ext_brake_release"), 1);  //writeRawDo(hoist_ext_brake_release, 1);
            }

            // - clear command queue
            //oht_[ax]->clearCommand();

            //OHT_LOGT1("Axis[%d] [%d]Servo On : %x\n", ax, (uint64_t)&dpram_.mem[(ax*48)+4] - (uint64_t)&dpram_, *(short*)&dpram_.mem[(ax*48)+4]);
        } else { //servo off
            //dpram_.map.DPRamAxisMap[ax].AD_In_Pos_Flag = 1;   // delete, 210617 yjchoe
            //dpram_.map.DPRamAxisMap[ax].AD_In_Sequence = 0;
            dpram_.map.DPRamAxisMap[ax].AD_AxisSource |= ST_AMP_POWER_ONOFF;

            // - frames clear
            //oht_[ax]->clearCommand();
            if(ax == OHT_HOIST){
                //OHT_LOGT1("HOIST Brake disable\n");
                writeRawDo(ioconfig_.getportid("hoist_ext_brake_release"), 0);  //writeRawDo(hoist_ext_brake_release, 0);
            }
            //Servo off - Set the servo control register(6040, 6041)
            bool is_stop = oht_[ax]->isStopped();
            if (ax == OHT_WHEEL_R)
                is_stop = oht_[OHT_WHEEL_F]->isStopped();
            if (is_stop) {//Servo On/Off chec
                oht_[ax]->disableServo();

            }
            //OHT_LOGT1(" Axis[%d] [%d]Servo Off : %x\n", ax, (uint64_t)&dpram_.mem[(ax*48)+4] - (uint64_t)&dpram_, *(short*)&dpram_.mem[(ax*48)+4]);
        }
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : reset_Amp_Fault
//Target System   : OHT
//Brief           : reset amp fault status
//Author          : Seungwan An
//Date            : 2017.12.22
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    int Amc4OHT::reset_Amp_Fault(int ax)
    {
        dpram_.map.DPRamAxisMap[ax].AD_AxisSource &= ~ST_AMP_FAULT;
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : set_Position
//Target System   : OHT
//Brief           : if abnormal encoder value is detected by noise,
//                  update actual encoder value from servopack
//Author          : Seungwan An
//Date            : 2017.12.22
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::set_Position(int ax, int pos)
    {
//        oht_[ax]->resetEncoder(true); // false -> true for test 2018.06.28
        bool is_stop = oht_[ax]->isStopped();
        if (ax == OHT_WHEEL_R)
        //    return;    //CIS
            is_stop = oht_[OHT_WHEEL_F]->isStopped();
        if (is_stop) {//Servo On/Off check
            if (ax == OHT_SLIDE || ax == OHT_HOIST) {
                reset_status[ax] = oht_[ax]->resetEncoder(false);
            } else {
                reset_status[ax] = oht_[ax]->resetEncoder(true, DUAL_SERVO);
                FollowsPOS = 0;
                FollowsPOS_Prev = FollowsPOS;
            }
        }
    }

    void Amc4OHT::set_Move_Dist_mm(int ax, double dist, double vel, double acc, double dcc)
    {
        oht_[ax]->moveDistancePulse(oht_[ax]->mm2pulse(dist), oht_[ax]->mm2pulse(vel), oht_[ax]->mm2pulse(acc),
                                        oht_[ax]->mm2pulse(dcc));
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : do_AxisCmd
//Target System   : OHT
//Brief           : execute Axis command
//Motion Parameter:           CmdType              Speed       Accel & Decel       Distance
//                    AMC_CMD_AXIS_MOVE_P        Pulse/Sec      Pulse/Sec^2           -
//                    AMC_CMD_AXIS_MOVE_N        Pulse/Sec      Pulse/Sec^2           -
//                    AMC_CMD_AXIS_MOVE_S            -          Pulse/Sec^2           -
//                    AMC_CMD_AXIS_MOVE_DS       Pulse/Sec      Pulse/Sec^2         Pulse
//                  AMC_CMD_AXIS_MOVE_TIME_P     Pulse/Sec      Time[msec]            -
//                  AMC_CMD_AXIS_MOVE_TIME_N     Pulse/Sec      Time[msec]            -
//                  AMC_CMD_AXIS_MOVE_TIME_S         -          Time[msec]            -
//                  AMC_CMD_AXIS_MOVE_TIME_DS    Pulse/Sec      Time[msec]          Pulse
//Author          : Seungwan An
//Date            : 2018.01.11
//////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Amc4OHT::do_AxisCmd(AMC_CMD_RAW& amc_cmd, int ax)
	{
        std::lock_guard<std::mutex> guard(oht_isr_lock_);
		double FollowsACC_tmp = 0;
		double FollowsDCC_tmp = 0;
		double FollowsVEL_tmp = 0;
		double FollowsPOS_tmp = 0;
        cmd_flag = true;
/*
		// Hoist small add 수행 후 센서 감지 전 slide 명령 들어올 경우 마크 꺼짐(210917)
		for(int nloop = 0 ; nloop < 3 ; nloop++)
		{
            Marking_Process_Flag[nloop] = false;
			UseSmallAdd_For_Marking[nloop] = false;
            Marking_Process_Trigger_Flag[nloop] = false;
        }
*/
        Marking_Process_Flag[ax] = false;
        //UseSmallAdd_For_Marking[ax] = false;      // not use
        //Marking_Process_Trigger_Flag[ax] = false; // not use

        //OHT_LOGT1("Command Type = %d\n", amc_cmd.CmdAxis[ax].CmdType );

		switch(amc_cmd.CmdAxis[ax].CmdType)
		{
            case AMC_CMD_AXIS_NONE :
                break;

            case AMC_CMD_AXIS_RESET :
                reset_Amp_Fault(ax);
                break;

            case AMC_CMD_AXIS_AMP_ENABLE:
                set_Amp_Enable(ax, true);
                break;

            case AMC_CMD_AXIS_AMP_DISABLE:
                set_Amp_Enable(ax, false);
                break;

            // Move Positive
            case AMC_CMD_AXIS_MOVE_P:
                //OHT_LOGT1(">> AMC_CMD_AXIS_MOVE_P <<\n");

				FollowsPOS_tmp = 50; //initial setting for test
				FollowsVEL_tmp = amc_cmd.CmdAxis[ax].Speed; //Positive speed
				FollowsACC_tmp = amc_cmd.CmdAxis[ax].Accel;
				FollowsDCC_tmp = amc_cmd.CmdAxis[ax].Decel;

				//OHT_LOGT1("Target pos : %f, vel : %f, acc : %f, dcc : %f\n", FollowsPOS_tmp, FollowsVEL_tmp, FollowsACC_tmp, FollowsDCC_tmp);
				oht_[ax]->moveDistancePulse(FollowsPOS_tmp, FollowsVEL_tmp, FollowsACC_tmp, FollowsDCC_tmp);
                Move_Flag = 1;
                Trace_Execute_1(ax, FollowsACC_tmp, FollowsDCC_tmp, FollowsVEL_tmp, FollowsPOS_tmp);
				break;

            // Move Negative
            case AMC_CMD_AXIS_MOVE_N:
                //OHT_LOGT1(">> AMC_CMD_AXIS_MOVE_N <<\n");

				FollowsPOS_tmp = 50; //initial setting for test
				FollowsVEL_tmp = amc_cmd.CmdAxis[ax].Speed; //Positive speed
				FollowsACC_tmp = amc_cmd.CmdAxis[ax].Accel;
				FollowsDCC_tmp = amc_cmd.CmdAxis[ax].Decel;

				//OHT_LOGT1("Target pos : %f, vel : %f, acc : %f, dcc : %f\n", FollowsPOS_tmp, FollowsVEL_tmp, FollowsACC_tmp, FollowsDCC_tmp);
				oht_[ax]->moveDistancePulse(-FollowsPOS_tmp, FollowsVEL_tmp, FollowsACC_tmp, FollowsDCC_tmp);
                Move_Flag = 2;
                Trace_Execute_1(ax, FollowsACC_tmp, FollowsDCC_tmp, FollowsVEL_tmp, FollowsPOS_tmp);
                break;

            // Move Stop
            case AMC_CMD_AXIS_MOVE_S:
                //OHT_LOGT1(">>??????????????????? AMC_CMD_AXIS_MOVE_S <<\n");

				FollowsDCC_tmp = amc_cmd.CmdAxis[ax].Decel;
				//Follows_MoveDS_Flag = false;
                Follow_Enable_Flag = false;
				if (ax == OHT_WHEEL_F) {
					drv_cmd.enable_anticollision = false;
				}
				//OHT_LOGT1("Target dcc : %f\n", FollowsDCC_tmp);
				oht_[ax]->stopPulse(FollowsDCC_tmp, false);

                prevACC[ax] = 0;
                prevDCC[ax] = 0;
                prevVEL[ax] = 0;
                prevPOS[ax] = 0;
                Move_Flag = 3;
                Trace_Execute_1(ax, 0, FollowsDCC_tmp, 0, 0);
                break;
	
		case AMC_CMD_AXIS_MOVE_ESTOP:
			//OHT_LOGT1(">>!!!!!!!!!!!!!!!!!!!! AMC_CMD_AXIS_MOVE_ESTOP <<\n");
			FollowsDCC_tmp = amc_cmd.CmdAxis[ax].Decel;
			Follow_Enable_Flag = false;
			if(ax==OHT_WHEEL_F)
			{
				drv_cmd.enable_anticollision = false;
			}
			oht_[ax]->stopPulse(FollowsDCC_tmp, true);

            prevACC[ax] = 0;
            prevDCC[ax] = 0;
            prevVEL[ax] = 0;
            prevPOS[ax] = 0;
            Move_Flag = 33;
            Trace_Execute_1(ax, 0, FollowsDCC_tmp, 0, 0);
		break;

			// Move Distance
			case AMC_CMD_AXIS_MOVE_DS:
            case AMC_CMD_AXIS_MOVE_TIME_DS:
				//OHT_LOGT1(">> AMC_CMD_AXIS_MOVE_DS (ax=%d) <<\n", ax);
				FollowsACC_tmp = (double)amc_cmd.CmdAxis[ax].Accel;
				FollowsDCC_tmp = (double)amc_cmd.CmdAxis[ax].Decel;
				FollowsVEL_tmp = (double)amc_cmd.CmdAxis[ax].Speed;
				FollowsPOS_tmp = (double)amc_cmd.CmdAxis[ax].Distance;

                TriggerCtrlInfoParsing[ax].Cmd = amc_cmd.CmdAxis[ax].TriggerCtrlInfo;

				// Check duplicate command, 210916 yjchoe
				if ((FollowsACC_tmp == prevACC[ax]) && (FollowsDCC_tmp == prevDCC[ax]) && (FollowsVEL_tmp == prevVEL[ax]) && (FollowsPOS_tmp == prevPOS[ax])) {
                    if (TriggerCtrlInfoParsing[ax].TriggerInfo.UseSmallAdd) {
                        //OHT_LOGT1("[FollowControl] (SMALL ADD) Duplicate Command\n");
                        Marking_Process_Flag[ax] = true;
                    }
                    //OHT_LOGT1(">> Duplicate command handling (ax=%d) <<\n", ax);
                    break;
                }

                //Marking_Process_Flag[ax] = false;
                FollowsDIS = (double)amc_cmd.CmdAxis[ax].Distance - oht_[ax]-> getPosPulse();

                prevACC[ax] = FollowsACC_tmp;
                prevDCC[ax] = FollowsDCC_tmp;
                prevVEL[ax] = FollowsVEL_tmp;
                prevPOS[ax] = FollowsPOS_tmp;
                
                if(ax == OHT_WHEEL_F)
                {
					Check_Front_Rear_servo_On();
                }

				if(TriggerCtrlInfoParsing[ax].TriggerInfo.UseSmallAdd)
				{
					//OHT_LOGT1(">> SMALL ADD Function! <<\n");
					IoNumber_For_Marking[ax] = TriggerCtrlInfoParsing[ax].TriggerInfo.TriggerIONumber;
					DistanceAddition_For_Marking[ax] = TriggerCtrlInfoParsing[ax].TriggerInfo.AdditionalDist;
					DistanceLimit_For_Marking[ax] = amc_cmd.CmdAxis[ax].Distance;
					IoType_For_Marking[ax] = TriggerCtrlInfoParsing[ax].TriggerInfo.IsTriggerOnRisingEdge;

					ACC_For_Marking[ax] = (double)amc_cmd.CmdAxis[ax].Accel;
					DCC_For_Marking[ax] = (double)amc_cmd.CmdAxis[ax].Decel;
					VEL_For_Marking[ax] = (double)amc_cmd.CmdAxis[ax].Speed;
					POS_For_Marking[ax] =  oht_[ax]->getPosPulse() + DistanceAddition_For_Marking[ax];

					// 현재 위치 확인 후 Mark +- 방향 결정
                    if((DistanceLimit_For_Marking[ax] - oht_[ax]->getPosPulse()) >= 0)
                        DirectionFlag_For_Marking[ax] = true;
                    else
                        DirectionFlag_For_Marking[ax] = false;

                    FlagIO_For_Marking[ax] = GetBit(IO_Input, IoNumber_For_Marking[ax]); // 210916 yjchoe

					/*OHT_LOGT1("[FollowControl] (SMALL ADD) IO NUM[%d] IO Type[%d] FlagIO[%d] Target Pos[%d] DistAdd[%d]",
						IoNumber_For_Marking[ax],
						IoType_For_Marking[ax],
						FlagIO_For_Marking[ax],
						DistanceLimit_For_Marking[ax],
						DistanceAddition_For_Marking[ax]);*/

                    oht_[ax]->moveToPulse(DistanceLimit_For_Marking[ax], VEL_For_Marking[ax], ACC_For_Marking[ax], DCC_For_Marking[ax]);
                    //OHT_LOGT1("[FollowControl] CurPos[%.0f] TargetPos[%d] TargetVel[%.0f] TargetAcc[%.0f] TargetDcc[%.0f]",
                    //          oht_[ax]->getPosPulse(), DistanceLimit_For_Marking[ax], VEL_For_Marking[ax], ACC_For_Marking[ax], DCC_For_Marking[ax]);
                    FollowsPOS = DistanceLimit_For_Marking[ax];
                    FollowsVEL = VEL_For_Marking[ax];
                    FollowsACC = ACC_For_Marking[ax];
                    FollowsDCC = DCC_For_Marking[ax];
                    FollowsPOS_Prev = FollowsPOS;
                    FollowsVEL_Prev = FollowsVEL;
                    FollowsACC_Prev = FollowsACC;
                    FollowsDCC_Prev = FollowsDCC;

                    Marking_Process_Flag[ax] = true;
                    Follow_Enable_Flag = true;
                    Move_Flag = 6;
				}
				else
				{
					//OHT_LOGT1("[MoveDS]AMC Target pos : %d(dist), vel : %d, acc : %d, dcc : %d\n", amc_cmd.CmdAxis[ax].Distance, amc_cmd.CmdAxis[ax].Speed, amc_cmd.CmdAxis[ax].Accel, amc_cmd.CmdAxis[ax].Decel);
					//OHT_LOGT1("[MoveDS]Motion Target pos : %.0f(dist-pos), vel : %.0f, acc : %.0f, dcc : %.0f\n", FollowsDIS, FollowsVEL_tmp, FollowsACC_tmp, FollowsDCC_tmp);
					oht_[ax]->moveToPulse(FollowsPOS_tmp, FollowsVEL_tmp, FollowsACC_tmp, FollowsDCC_tmp);

                    //Follow_Enable_Flag = false;
					FollowsPOS = FollowsPOS_tmp;
					FollowsVEL = FollowsVEL_tmp;
					FollowsACC = FollowsACC_tmp;
					FollowsDCC = FollowsDCC_tmp;
					FollowsPOS_Prev = FollowsPOS;
					FollowsVEL_Prev = FollowsVEL;
					FollowsACC_Prev = FollowsACC;
					FollowsDCC_Prev = FollowsDCC;
                    Follow_Enable_Flag = true;
                    Move_Flag = 5;
				}
                //Debug_Flag = Move_Flag;
                Trace_Execute_1(ax, FollowsACC_Prev, FollowsDCC_Prev, FollowsVEL_Prev, FollowsPOS_Prev);
                break;

            // Move Positive
			case AMC_CMD_AXIS_MOVE_TIME_P:
                //OHT_LOGT1(">> AMC_CMD_AXIS_MOVE_TIME_P <<\n");

				FollowsPOS_tmp = 10000;  //initial setting for test
				FollowsVEL_tmp = amc_cmd.CmdAxis[ax].Speed;
				FollowsACC_tmp = (FollowsVEL_tmp * SECTOMSEC) / amc_cmd.CmdAxis[ax].Accel;
				FollowsDCC_tmp = (FollowsVEL_tmp * SECTOMSEC) / amc_cmd.CmdAxis[ax].Decel;

				//OHT_LOGT1("Target pos : %f, vel : %f, acc : %f, dcc : %f\n", FollowsPOS_tmp, FollowsVEL_tmp, FollowsACC_tmp, FollowsDCC_tmp);
				oht_[ax]->moveDistancePulse(FollowsPOS_tmp, FollowsVEL_tmp, FollowsACC_tmp, FollowsDCC_tmp);
				break;

			// Move Negative
			case AMC_CMD_AXIS_MOVE_TIME_N:
				//OHT_LOGT1(">> AMC_CMD_AXIS_MOVE_TIME_N <<\n");
				FollowsPOS_tmp = 10000;  //initial setting for test
				FollowsVEL_tmp = amc_cmd.CmdAxis[ax].Speed; //Positive speed
				FollowsACC_tmp = (FollowsVEL_tmp * SECTOMSEC) / amc_cmd.CmdAxis[ax].Accel;
				FollowsDCC_tmp = (FollowsVEL_tmp * SECTOMSEC) / amc_cmd.CmdAxis[ax].Decel;

				//OHT_LOGT1("Target pos : %f, vel : %f, acc : %f, dcc : %f\n", FollowsPOS_tmp, FollowsVEL_tmp, FollowsACC_tmp, FollowsDCC_tmp);
				oht_[ax]->moveDistancePulse(-FollowsPOS_tmp, FollowsVEL_tmp, FollowsACC_tmp, FollowsDCC_tmp);
                break;

            // Move Stop
            case AMC_CMD_AXIS_MOVE_TIME_S:
                //OHT_LOGT1(">> AMC_CMD_AXIS_MOVE_TIME_S <<\n");

				FollowsVEL_tmp = config->v_max[ax];
				FollowsDCC_tmp = (FollowsVEL_tmp * SECTOMSEC) / amc_cmd.CmdAxis[ax].Decel;

				//OHT_LOGT1("Target dcc : %f\n",FollowsDCC_tmp);
				oht_[ax]->stopPulse(FollowsDCC_tmp, false);
                break;

            case AMC_CMD_AXIS_SET_HOME:
                //OHT_LOGT1(">> Axis[%d] AMC_CMD_AXIS_SET_HOME <<",ax);
                //set_Amp_Enable(ax, true);   // for RSA Homing Mode, 201217 yjchoe
                //set_Position(ax, 0);
                controller.localizer_->resetWindows();
                set_Position(0, 0);
                set_Position(1, 0);

                break;

            case AMC_CMD_AXIS_SET_OFFSET:
                break;

            case AMC_CMD_AXIS_SET_POS_LIMIT:
                break;

            case AMC_CMD_AXIS_SET_NEG_LIMIT:
                break;

            case AMC_CMD_AXIS_RESET_LIMIT:
                break;
		}
    }


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : GetBit
//Target System   : OHT
//Brief           : Get Sensor value
//Author          : Seungwan An
//Date            : 2017.12.21
//////////////////////////////////////////////////////////////////////////////////////////////////////////
	int Amc4OHT::GetBit(int Source, int BitNum)
    {
    	return (Source >> BitNum) & 0x01;
    }

    int Amc4OHT::GetBit(uint32_t *IO_arr, int BitNum) {
        int Num = BitNum / (sizeof(IO_arr) * 4);
        switch (Num) {
            case 0:
                return (IO_arr[Num] >> BitNum) & 0x01;
            case 1:
            case 2:
                return (IO_arr[Num] >> (BitNum - (sizeof(IO_arr) * 4 * Num))) & 0x01;
        }
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : GetPos
//Target System   : OHT
//Brief           : Get Current Position of Axis 0
//Author          : Seungwan An
//Date            : 2017.12.21
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    int Amc4OHT::GetPos(void)
    {
        auto vel_pulse = oht_[0]->getPosPulse();

        return vel_pulse;
    }

    void Amc4OHT::Check_Rear_servo_Off(void)
    {
        //if((motors[OHT_WHEEL_F]->inPosition() == true) && (oht_[OHT_WHEEL_F]->getMotionState() == Motion::IDLE)
        //    && (motors[OHT_WHEEL_R]->inPosition() == true) && (oht_[OHT_WHEEL_R]->getMotionState() == Motion::IDLE))
        if((motors[OHT_WHEEL_F]->inPosition() == true) && (motors[OHT_WHEEL_R]->inPosition() == true))   // yjchoe 200923
        {
            if (++AMC_Stop_count >= Model_AXIS_STOP_COUNT_LIMIT){
                AMC_Stop_count = 0;
                if (Path_Slope_Status == 0){
                    //if ((dpram_.map.DPRamAxisMap[OHT_WHEEL_R].AD_AxisSource & ST_AMP_POWER_ONOFF) != ST_AMP_POWER_ONOFF){
                    /*
                    // Servo off/on 로직 삭제
                    if (motors[OHT_WHEEL_R]->isEnabled() == true) {
                        OHT_LOGT1("AMC Rear axis servo off!\n");
                        set_Amp_Enable(OHT_WHEEL_R, false);
                    }
                    */

                    TorqueLimit_Request = 0;
                    Servo_Off_type = 0;
                } else {
                    TorqueLimit_Request = 1;
                    Servo_Off_type = 1;
                }
                Rear_servo_Off_flag = true;
            }
        } else {
            AMC_Stop_count = 0;
        }
    }
   
    void Amc4OHT::Check_Front_Rear_servo_On(void)
    {
        //OHT_LOGT1("Check Servo Status!\n");
        //if((dpram_.map.DPRamAxisMap[OHT_WHEEL_F].AD_AxisSource & ST_AMP_POWER_ONOFF) == ST_AMP_POWER_ONOFF){
        if (motors[OHT_WHEEL_F]->isEnabled() == false) {
            //OHT_LOGT1("AMC Front axis servo on!\n");
            set_Amp_Enable(OHT_WHEEL_F, true);
        }

        //if((dpram_.map.DPRamAxisMap[OHT_WHEEL_R].AD_AxisSource & ST_AMP_POWER_ONOFF) == ST_AMP_POWER_ONOFF){
        if (motors[OHT_WHEEL_R]->isEnabled() == false) {
            //OHT_LOGT1("AMC Rear axis servo on!\n");
            set_Amp_Enable(OHT_WHEEL_R, true);
        }
        AMC_Stop_count = 0;
        Rear_servo_Off_flag = false;
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : statusUpdate
//Target System   : OHT
//Brief           : Service Request No : 0
//                  update overall oht status
//                  requested by amc.ll every sync cycle time
//Author          : Seungwan An
//Date            : 2017.10.25
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::statusUpdate()
    {

        OHT_LOGT1("statusUpdate handler is called \n");

        int axes_no = dpram_.Cmd_Dpram().Axis_Num;  //Z,X,R,W axis

        for(int ax = 0; ax < axes_no; ax++) {

            //OHT_LOGT1("Axis[%d] mo_flag : %d / pos_flag : %d / seq_flag : %d / Source : %x\n",\
            ax, *(short*)&dpram_.mem[(ax*48)+8], dpram_.mem[(ax*48)+2], dpram_.mem[(ax*48)+3], *(short*)&dpram_.mem[(ax*48)+4]);
        }
    }


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------Service request function from IPC--------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Reserved_Function
//Target System   : OHT
//Brief           : Service Request No : none
//Author          : Seungwan An
//Date            : 2017.11.13 
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::Reserved_Function()
    {
        // Do nothing
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Servo_On
//Target System   : OHT
//Brief           : Service Request No : 24
//Author          : Seungwan An / Ray Lim
//Date            : 2017.11.13 
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::Servo_On()
    {
        char ax = dpram_.map.Axis_Num;
        //OHT_LOGT1("Axis[%d] Servo_On handler is called!",ax);
        if(oht_[ax]->getMotionState() == Motion::IDLE) {
            set_Amp_Enable(ax, true);

            //if(ax == OHT_WHEEL_R)     ///CIS
                //motors[ax]->setMasterAxis(0);  //CIS
        }
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Servo_Off
//Target System   : OHT
//Brief           : Service Request No : 25
//Brief           : Service Request No : 25
//Author          : Seungwan An / Ray Lim
//Date            : 2017.11.13 
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::Servo_Off()
    {
        char ax = dpram_.map.Axis_Num;
        //OHT_LOGT1("Axis[%d] Servo_Off handler is called!\n",ax);
        if(oht_[ax]->getMotionState() == Motion::IDLE) {
            //set_Position(ax, 0);    // for RSA, 201217 yjchoe
            set_Amp_Enable(ax, false);
        }
        writeDo(ioconfig_.getportid("hoist_ext_brake_release"), false);  //writeDo(hoist_ext_brake_release, false);

    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Alarm_Reset
//Target System   : OHT
//Brief           : Service Request No : 26
//Author          : Seungwan An / Ray Lim
//Date            : 2017.11.13 
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::Alarm_Reset()
    {
        OHT_LOGT1("Alarm_Reset handler is called!\n");

        char ax = dpram_.map.Axis_Num;
        if(oht_[ax]->getMotionState() == Motion::IDLE)
            reset_Amp_Fault(ax);
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Alarm_Set
//Target System   : OHT
//Brief           : Service Request No : 27
//Author          : Seungwan An / Ray Lim
//Date            : 2017.11.13
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::Alarm_Set()
    {
        OHT_LOGT1("Alarm_Set handler is called!\n");

        char ax = dpram_.map.Axis_Num;

        auto vel_pulse = oht_[ax]->getPosPulse();

        //short vel_check_rpm = (fabs(vel_pulse*60) / (1<<13));

        //Need to change the state after servo pack tune
        //if(vel_check)   //if velocity is not 0, it means in motion state
        if (oht_[ax]->getMotionState() != Motion::IDLE)
        {//in motion
            OHT_LOGT1("Can't clear status in motion\n");
            //dpram_.map.DPRamAxisMap[ax].AD_In_Sequence = 1; // set in sequence // delete, 210617 yjchoe
        }
        else
        {//not in motion
            OHT_LOGT1("Clear Axis[%d] Status\n", ax);
            dpram_.map.DPRamAxisMap[ax].AD_AxisSource &= (ST_AMP_POWER_ONOFF | ST_OUT_OF_FRAMES | ST_PC_COMMAND);
            dpram_.map.DPRamAxisMap[ax].AD_AxisSource &= ~ ST_PC_COMMAND;
            oht_para_.event_type[ax] = NO_EVENT;
            //dpram_.map.DPRamAxisMap[ax].AD_In_Sequence = 0; // set not in sequence // delete, 210617 yjchoe
            //dpram_.map.DPRamAxisMap[ax].AD_In_Pos_Flag = 1; // set in position

            //oht_[ax]->clearCommand();
        }

    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Clear_Status
//Target System   : OHT
//Brief           : Service Request No : 28
//Author          : Seungwan An / Ray Lim
//Date            : 2017.11.13 
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::Clear_Status()
    {
        char ax = dpram_.map.Axis_Num;
        OHT_LOGT1("Axis[%d] Clear_Status handler is called!",ax);

        auto vel_pulse = oht_[ax]->getPosPulse();

        //short vel_check_rpm = (fabs(vel_pulse*60) / (1<<13));

        //Need to change the state after servo pack tune
        //if(vel_check)   //if velocity is not 0, it means in motion state
        if (oht_[ax]->getMotionState() != Motion::IDLE)
        {//in motion
            OHT_LOGT1("Can't clear status in motion\n");
            //dpram_.map.DPRamAxisMap[ax].AD_In_Sequence = 1; // set in sequence // delete, 210617 yjchoe
        }
        else
        {//not in motion
            OHT_LOGT1("Clear Axis[%d] Status\n", ax);
            dpram_.map.DPRamAxisMap[ax].AD_AxisSource &= (ST_AMP_POWER_ONOFF | ST_OUT_OF_FRAMES | ST_PC_COMMAND);
            dpram_.map.DPRamAxisMap[ax].AD_AxisSource &= ~ ST_PC_COMMAND;
            dpram_.map.DPRamAxisMap[ax].AD_AxisSource &= ~ST_AMP_FAULT;
            dpram_.map.DPRamAxisMap[ax].AD_AxisSource &= ~ST_MUST_REBOOT;
            dpram_.map.DPRamAxisMap[ax].AD_AxisSource &= ~ST_X_NEG_LIMIT;
            dpram_.map.DPRamAxisMap[ax].AD_AxisSource &= ~ST_X_POS_LIMIT;
            dpram_.map.DPRamAxisMap[ax].AD_AxisSource &= ~ST_V_LIMIT;
            dpram_.map.DPRamAxisMap[ax].AD_AxisSource &= ~ST_ERROR_LIMIT;
            oht_para_.event_type[ax] = NO_EVENT;
            //dpram_.map.DPRamAxisMap[ax].AD_In_Sequence = 0; // set not in sequence // delete, 210617 yjchoe
            //dpram_.map.DPRamAxisMap[ax].AD_In_Pos_Flag = 1; // set in position


            oht_[ax]->clearCommand();
            //motors[ax]->disable();
        }
        prevACC[ax] = prevDCC[ax] = prevPOS[ax] = prevVEL[ax] = 0;
        //OHT_LOGT1("Target Axis[%d]", ax);
        //OHT_LOGT1(" / [%d] Axis Source : %x", (uint64_t)&dpram_.mem[(ax*48)+4] - (uint64_t)&dpram_, *(short*)&dpram_.mem[(ax*48)+4]);
        //OHT_LOGT1(" / [%d] in sequence : %d", (uint64_t)&dpram_.mem[(ax*48)+3] - (uint64_t)&dpram_, dpram_.mem[(ax*48)+3]);
        //OHT_LOGT1(" / [%d] in position : %d\n", (uint64_t)&dpram_.mem[(ax*48)+2] - (uint64_t)&dpram_, dpram_.mem[(ax*48)+2]);

    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Get_Frames_Clear
//Target System   : OHT
//Brief           : Service Request No : 66
//Author          : Seungwan An / Ray Lim
//Date            : 2017.11.13 
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::Get_Frames_Clear()
    {
        OHT_LOGT1("Get_Frames_Clear handler is called!\n");
		char ax = dpram_.map.Axis_Num;

        if(oht_[ax]->getMotionState() == Motion::IDLE)
            oht_[ax]->clearCommand();

        //dpram_.map.DPRamAxisMap[ax].AD_In_Sequence = 0; // delete, 210617 yjchoe

        //OHT_LOGT1("Target Axis[%d]", ax);
        //OHT_LOGT1(" / [%d] in sequence : %d\n", (uint64_t)&dpram_.mem[(ax*48)+3] - (uint64_t)&dpram_, dpram_.mem[(ax*48)+3]);

    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Get_SW_Upper_Limit
//Target System   : OHT
//Brief           : Service Request No : 76
//Author          : Seungwan An / Ray Lim
//Date            : 2017.11.13
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::Get_SW_Upper_Limit()
    {
        char ax = dpram_.map.Axis_Num;
        int limit_pulse = dpram_.map.DPRamAxisMap[ax].AD_LongType;  //sw limit - 13bit encoder

        //action - no event, stop, e-stop, abort
        oht_para_.swupper_limit_st[ax] = dpram_.map.DPRamAxisMap[ax].AD_CharType[0];

        oht_[ax]->setPosLimitPulse(oht_[ax]->getPosLimitPulse().first, limit_pulse);
        OHT_LOGT1("Target Axis[%d]", ax);
        OHT_LOGT1(" / [%d] S/W Limit : %d", (uint64_t)&dpram_.mem[(ax*48)+26] - (uint64_t)&dpram_, *(int*)&dpram_.mem[(ax*48)+26]);
        OHT_LOGT1(" / [%d] Event Type : %d\n", (uint64_t)&dpram_.mem[(ax*48)+14] - (uint64_t)&dpram_, dpram_.mem[(ax*48)+14]);

    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Put_SW_Upper_Limit
//Target System   : OHT
//Brief           : Service Request No : 77
//Author          : Seungwan An / Ray Lim
//Date            : 2017.11.13 
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::Put_SW_Upper_Limit()
    {
        OHT_LOGT1("Put_SW_Upper_Limit handler is called!\n");
        char ax = dpram_.map.Axis_Num;

        dpram_.map.DPRamAxisMap[ax].AD_LongType =  oht_[ax]->getPosLimitPulse().second;
        dpram_.map.DPRamAxisMap[ax].AD_CharType[0] = (char)oht_para_.swupper_limit_st[ax];

    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Get_SW_Lower_Limit
//Target System   : OHT
//Brief           : Service Request No : 78
//Author          : Seungwan An / Ray Lim
//Date            : 2017.11.13 
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::Get_SW_Lower_Limit()
    {
        OHT_LOGT1("Get_SW_Lower_Limit handler is called!\n");
		//double sw_limit_max, sw_limit_min;
        char ax = dpram_.map.Axis_Num;
        int limit_pulse = dpram_.map.DPRamAxisMap[ax].AD_LongType;  //sw limit - 13bit encoder

        //action - no event, stop, e-stop, abort
        oht_para_.swlower_limit_st[ax] = dpram_.map.DPRamAxisMap[ax].AD_CharType[0];
        oht_[ax]->setPosLimitPulse(limit_pulse, oht_[ax]->getPosLimitPulse().second);

        OHT_LOGT1("Target Axis[%d]", ax);
        OHT_LOGT1(" / [%d] S/W Limit : %d", (uint64_t)&dpram_.mem[(ax*48)+26] - (uint64_t)&dpram_, *(int*)&dpram_.mem[(ax*48)+26]);
        OHT_LOGT1(" / [%d] Event Type : %d\n", (uint64_t)&dpram_.mem[(ax*48)+14] - (uint64_t)&dpram_, dpram_.mem[(ax*48)+14]);

    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Put_SW_Lower_Limit
//Target System   : OHT
//Brief           : Service Request No : 79
//Author          : Seungwan An / Ray Lim
//Date            : 2017.11.13 
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::Put_SW_Lower_Limit()
    {
        OHT_LOGT1("Put_SW_Lower_Limit handler is called!\n");
        int ax = dpram_.map.Axis_Num;

        dpram_.map.DPRamAxisMap[ax].AD_LongType1 = oht_[ax]->getPosLimitPulse().first;
        dpram_.map.DPRamAxisMap[ax].AD_CharType[0] = (char)oht_para_.swlower_limit_st[ax];

    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Get_SyncMap
//Target System   : OHT
//Brief           : Service Request No : 89
//                  Set the
//Author          : Ray Lim
//Date            : 2017.11.13 
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::Get_SyncMap()
    {
        OHT_LOGT1("Get_SyncMap handler is called!\n");

        sync_motion_.Master = dpram_.Cmd_Dpram().Axis[0];
        sync_motion_.Slave = dpram_.Cmd_Dpram().Axis[1];

        OHT_LOGT1("Master : %d, Slave : %d\n", sync_motion_.Master, sync_motion_.Slave);
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Get_SyncOn
//Target System   : OHT
//Brief           : Service Request No : 90
//Author          : Ray Lim
//Date            : 2017.11.13
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::Get_SyncOn()
    {
        OHT_LOGT1("Get_SyncOn handler is called!\n");

        sync_motion_.Flag = 1;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Get_SyncOff
//Target System   : OHT
//Brief           : Service Request No : 91
//Author          : Ray Lim
//Date            : 2017.11.13
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::Get_SyncOff()
    {
        OHT_LOGT1("Get_SyncOff handler is called!\n");

        sync_motion_.Flag = 0;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Get_SyncGain
//Target System   : OHT
//Brief           : Service Request No : 92
//Author          : Ray Lim
//Date            : 2018.01.10
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::Get_SyncGain()
    {
        OHT_LOGT1("Get_SyncGain handler is called!\n");

        char ax = dpram_.map.Axis_Num;

        sync_motion_.Gain = dpram_.map.DPRamAxisMap[ax].AD_FloatType;

        if(sync_motion_.Gain > 1.03)
            sync_motion_.Gain = 1.03;
        else if(sync_motion_.Gain < 0.97)
            sync_motion_.Gain = 0.97;
    }
////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Exist_Check
//Target System   : OHT
//Brief           : Service Request No : 111
//Author          : Seungwan An / Ray Lim
//Date            : 2017.11.13
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::Exist_Check()
    {
        OHT_LOGT1("Exist_Check handler is called!\n");

        short JPC_AXIS = 4;

        dpram_.Cmd_Dpram().Axis_Num = JPC_AXIS;
        dpram_.map.Axis_Command = 1;
        dpram_.Cmd_Dpram().Ver = VERSION_DSP;

        OHT_LOGT1("[%3d] Axis_Num : %d\n", (uint64_t)&dpram_.Cmd_Dpram().Axis_Num - (uint64_t)&dpram_, *(short*)&dpram_.mem[528]);
        OHT_LOGT1("[%3d] Command : %d\n", (uint64_t)&dpram_.map.Axis_Command - (uint64_t)&dpram_, dpram_.mem[1021]);
        OHT_LOGT1("[%3d] Version : %d\n", (uint64_t)&dpram_.Cmd_Dpram().Ver - (uint64_t)&dpram_, *(short*)&dpram_.mem[530]);
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Set_Oht_Model_id
//Target System   : OHT
//Brief           : Service Request No : 112
//Author          : Seungwan An
//Date            : 2017.11.13
////////////////////////////////////////////////////////////////////////////////////////////////////
	void Amc4OHT::Set_Oht_Model_id()
    {
        Model_OHT_ID = dpram_.map.DPRamAxisMap[0].AD_IntType[0];
        OHT_LOGT1("Set_Oht_Model_id handler is called!  ID:%d\n", Model_OHT_ID);
        bool enabled[AMC_AXIS] = {false, };
        for(int i = 0; i<AMC_AXIS; i++)
        {
            enabled[i] = motors[i]->isEnabled();
            if (enabled[i]) set_Amp_Enable(i, false);   // Model 변경에 의한 기어비 변경이 이뤄져 servo off 후 진행해야 함
            Marking_Process_Flag[i] = false;
        }
        MoveDSExecute = false;
        //Follows_MoveDS_Flag = false;
        controller.loadIOConfig(Model_OHT_ID);     // Read I/O Config and Mapping

        if (Model_OHT_ID == OHT_STD_01 || Model_OHT_ID == OHT_NAME_V81) {
            Model_PULSETOMM = V81_PULSETOMM_DRIVE;
            Model_PULSETOMM_HOIST = V81_PULSETOMM_HOIST;
            Model_PULSETOMM_SLIDE = V81_PULSETOMM_SLIDE;

            /* implimanation required for each Variant */
            Model_OHT_DETECT_WRITE_BIT_MASK  = 0x00003C00;
            Model_OBS_DETECT_WRITE_BIT_MASK  = 0x000000F0;
            Model_OHT_DETECT_WRITE_BIT_SCALE = 10;
            Model_OBS_DETECT_WRITE_BIT_SCALE = 4;
            Model_OHT_DETECT_READ_BIT_SCALE  = 35;
            Model_OBS_DETECT_READ_BIT_SCALE  = 30;

            Model_DRIVINGPRECHECK = 55;
            Model_DRIVINGFRONTNUM = 54;
            Model_TRANSFRONT      = 56;
            Model_TRANSREAR       = 57;
            Model_HOISTHOME       = 73;

            Model_OHT_SENSOR_ENABLE = true;
            Model_OBS_SENSOR_MAKER  = HOKUYO;

            Model_MOVE_S_DCC_BY_SENSOR_NORMAL = CalVelRatio(SPEED_3_5_MPS2, Model_OHT_ID);
            Model_MOVE_S_DCC_BY_SENSOR_CURVE  = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);
            Model_MOVE_S_DCC_BY_SENSOR_ETC	  = CalVelRatio(SPEED_2_5_MPS2, Model_OHT_ID);
            Model_MOVE_DS_ACC_BY_SENSOR_CURVE = CalVelRatio(SPEED_1_0_MPS2, Model_OHT_ID);
            Model_MOVE_DS_DCC_BY_SENSOR_CURVE = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);
            Model_MOVE_DS_DCC_BY_SENSOR_ETC   = CalVelRatio(SPEED_2_5_MPS2, Model_OHT_ID);

            PATH_SLOPE_DCC = CalVelRatio(SPEED_3_0_MPS2, Model_OHT_ID);

            Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
            FollowsDCC_CMD = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;

            Model_OHT_Service = 0;

            Model_FILTER_4_NODECNT = 3;     // 사용 안 함

            Model_SPEED_GRADE_1_VELOCITY = CalVelRatio(VELOCITY_0_0_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_2_VELOCITY = CalVelRatio(VELOCITY_0_3_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_3_VELOCITY = CalVelRatio(VELOCITY_0_3_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_4_VELOCITY = CalVelRatio(VELOCITY_1_0_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_5_VELOCITY = CalVelRatio(VELOCITY_2_2_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_6_VELOCITY = CalVelRatio(VELOCITY_3_3_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_7_VELOCITY = CalVelRatio(VELOCITY_4_1_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_8_VELOCITY = CalVelRatio(VELOCITY_5_2_MPS, Model_OHT_ID);

            OBS_SENSOR_VEL_LONG = CalVelRatio(VELOCITY_0_5_MPS, Model_OHT_ID);
            OBS_SENSOR_VEL_MIDDLE = CalVelRatio(VELOCITY_0_15_MPS, Model_OHT_ID);

            STOP_MOTION_DCC_HIGH = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
            STOP_MOTION_DCC_LOW = CalVelRatio(SPEED_0_5_MPS2, Model_OHT_ID);

            DYNAMIC_ACC_1st = CalVelRatio(SPEED_2_0_MPS2, Model_OHT_ID);
            DYNAMIC_ACC_2nd = CalVelRatio(SPEED_2_0_MPS2, Model_OHT_ID);
            DYNAMIC_ACC_3rd = CalVelRatio(SPEED_1_0_MPS2, Model_OHT_ID);
            DYNAMIC_ACC_4th = CalVelRatio(SPEED_1_0_MPS2, Model_OHT_ID); //CalVelRatio(SPEED_0_7_MPS2, Model_OHT_ID);

            Model_AXIS_STOP_COUNT_LIMIT = 3000;

            Model_DIST_ARRIVED_LEVEL1 = 5000;
            Model_DIST_ARRIVED_LEVEL2 = 1000;

            SelectChangeNode = 1;
            controller.localizer_->nodeCounter().setPulsetoMM(Model_PULSETOMM);
            controller.localizer_->nodePrecheckCounter().setPulsetoMM(Model_PULSETOMM);
            controller.localizer_->transFrontDetector().setPulsetoMM(Model_PULSETOMM);
            controller.localizer_->transStopDetector().setPulsetoMM(Model_PULSETOMM);
            CHANGENODE_DIST_LIMIT = 100 * Model_PULSETOMM;

            // Dynamic ACC
            ACC_CHANGE_SPEED1 = 1.2;
            ACC_CHANGE_SPEED2 = 3.3;
            ACC_CHANGE_SPEED3 = 5.0;
        } else if (Model_OHT_ID == OHT_NAME_V82 || Model_OHT_ID == OHT_NAME_V85S) {
            Model_PULSETOMM = V82_PULSETOMM_DRIVE;
            Model_PULSETOMM_HOIST = V82_PULSETOMM_HOIST;
            Model_PULSETOMM_SLIDE = V82_PULSETOMM_SLIDE;

            /* implimanation required for each Variant */
            Model_OHT_DETECT_WRITE_BIT_MASK  = 0x00003C00;
            Model_OBS_DETECT_WRITE_BIT_MASK  = 0x000000F0;
            Model_OHT_DETECT_WRITE_BIT_SCALE = 10;
            Model_OBS_DETECT_WRITE_BIT_SCALE = 4;
            Model_OHT_DETECT_READ_BIT_SCALE  = 35;
            Model_OBS_DETECT_READ_BIT_SCALE  = 30;

            Model_DRIVINGPRECHECK = 55;
            Model_DRIVINGFRONTNUM = 54;
            Model_TRANSFRONT      = 56;
            Model_TRANSREAR       = 57;
            Model_HOISTHOME       = 73;

            Model_OHT_SENSOR_ENABLE = true;
            Model_OBS_SENSOR_MAKER  = HOKUYO;

            Model_MOVE_S_DCC_BY_SENSOR_NORMAL = CalVelRatio(SPEED_3_5_MPS2, Model_OHT_ID);
            Model_MOVE_S_DCC_BY_SENSOR_CURVE  = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);
            Model_MOVE_S_DCC_BY_SENSOR_ETC	  = CalVelRatio(SPEED_2_5_MPS2, Model_OHT_ID);
            Model_MOVE_DS_ACC_BY_SENSOR_CURVE = CalVelRatio(SPEED_1_0_MPS2, Model_OHT_ID);
            Model_MOVE_DS_DCC_BY_SENSOR_CURVE = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);
            Model_MOVE_DS_DCC_BY_SENSOR_ETC   = CalVelRatio(SPEED_2_5_MPS2, Model_OHT_ID);

            PATH_SLOPE_DCC = CalVelRatio(SPEED_3_0_MPS2, Model_OHT_ID);

            Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
            FollowsDCC_CMD = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;

            Model_OHT_Service = 0;

            Model_FILTER_4_NODECNT = 3;     // 사용 안 함

            Model_SPEED_GRADE_1_VELOCITY = CalVelRatio(VELOCITY_0_0_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_2_VELOCITY = CalVelRatio(VELOCITY_0_3_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_3_VELOCITY = CalVelRatio(VELOCITY_0_3_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_4_VELOCITY = CalVelRatio(VELOCITY_1_0_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_5_VELOCITY = CalVelRatio(VELOCITY_2_2_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_6_VELOCITY = CalVelRatio(VELOCITY_3_3_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_7_VELOCITY = CalVelRatio(VELOCITY_4_1_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_8_VELOCITY = CalVelRatio(VELOCITY_5_2_MPS, Model_OHT_ID);

            OBS_SENSOR_VEL_LONG = CalVelRatio(VELOCITY_0_5_MPS, Model_OHT_ID);
            OBS_SENSOR_VEL_MIDDLE = CalVelRatio(VELOCITY_0_15_MPS, Model_OHT_ID);

            STOP_MOTION_DCC_HIGH = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
            STOP_MOTION_DCC_LOW = CalVelRatio(SPEED_0_5_MPS2, Model_OHT_ID);

            DYNAMIC_ACC_1st = CalVelRatio(SPEED_2_0_MPS2, Model_OHT_ID);
            DYNAMIC_ACC_2nd = CalVelRatio(SPEED_2_0_MPS2, Model_OHT_ID);
            DYNAMIC_ACC_3rd = CalVelRatio(SPEED_1_0_MPS2, Model_OHT_ID);
            DYNAMIC_ACC_4th = CalVelRatio(SPEED_1_0_MPS2, Model_OHT_ID); //CalVelRatio(SPEED_0_7_MPS2, Model_OHT_ID);

            Model_AXIS_STOP_COUNT_LIMIT = 3000;

            Model_DIST_ARRIVED_LEVEL1 = 5000;
            Model_DIST_ARRIVED_LEVEL2 = 1000;

            SelectChangeNode = 1;
            controller.localizer_->nodeCounter().setPulsetoMM(Model_PULSETOMM);
            controller.localizer_->nodePrecheckCounter().setPulsetoMM(Model_PULSETOMM);
            controller.localizer_->transFrontDetector().setPulsetoMM(Model_PULSETOMM);
            controller.localizer_->transStopDetector().setPulsetoMM(Model_PULSETOMM);
            CHANGENODE_DIST_LIMIT = 100 * Model_PULSETOMM;

            // Dynamic ACC
            ACC_CHANGE_SPEED1 = 1.2;
            ACC_CHANGE_SPEED2 = 3.3;
            ACC_CHANGE_SPEED3 = 5.0;
        } else if (Model_OHT_ID == OHT_NAME_V86) {
            Model_PULSETOMM = V86_PULSETOMM_DRIVE;
            Model_PULSETOMM_HOIST = V86_PULSETOMM_HOIST;
            Model_PULSETOMM_SLIDE = V86_PULSETOMM_SLIDE;

            /* implimanation required for each Variant */
            Model_OHT_DETECT_WRITE_BIT_MASK  = 0x00003C00;
            Model_OBS_DETECT_WRITE_BIT_MASK  = 0x000000F0;
            Model_OHT_DETECT_WRITE_BIT_SCALE = 10;
            Model_OBS_DETECT_WRITE_BIT_SCALE = 4;
            Model_OHT_DETECT_READ_BIT_SCALE  = 35;
            Model_OBS_DETECT_READ_BIT_SCALE  = 30;

            Model_DRIVINGPRECHECK = 55;
            Model_DRIVINGFRONTNUM = 54;
            Model_TRANSFRONT      = 56;
            Model_TRANSREAR       = 57;
            Model_HOISTHOME       = 73;

            Model_OHT_SENSOR_ENABLE = true;
            Model_OBS_SENSOR_MAKER  = HOKUYO;

            Model_MOVE_S_DCC_BY_SENSOR_NORMAL = CalVelRatio(SPEED_3_5_MPS2, Model_OHT_ID);
            Model_MOVE_S_DCC_BY_SENSOR_CURVE  = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);
            Model_MOVE_S_DCC_BY_SENSOR_ETC	  = CalVelRatio(SPEED_2_5_MPS2, Model_OHT_ID);
            Model_MOVE_DS_ACC_BY_SENSOR_CURVE = CalVelRatio(SPEED_1_0_MPS2, Model_OHT_ID);
            Model_MOVE_DS_DCC_BY_SENSOR_CURVE = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);
            Model_MOVE_DS_DCC_BY_SENSOR_ETC   = CalVelRatio(SPEED_2_5_MPS2, Model_OHT_ID);

            PATH_SLOPE_DCC = CalVelRatio(SPEED_3_0_MPS2, Model_OHT_ID);

            Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
            FollowsDCC_CMD = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;

            Model_OHT_Service = 0;

            Model_FILTER_4_NODECNT = 3;     // 사용 안 함

            Model_SPEED_GRADE_1_VELOCITY = CalVelRatio(VELOCITY_0_0_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_2_VELOCITY = CalVelRatio(VELOCITY_0_3_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_3_VELOCITY = CalVelRatio(VELOCITY_0_3_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_4_VELOCITY = CalVelRatio(VELOCITY_1_0_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_5_VELOCITY = CalVelRatio(VELOCITY_2_2_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_6_VELOCITY = CalVelRatio(VELOCITY_3_3_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_7_VELOCITY = CalVelRatio(VELOCITY_4_1_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_8_VELOCITY = CalVelRatio(VELOCITY_5_2_MPS, Model_OHT_ID);

            OBS_SENSOR_VEL_LONG = CalVelRatio(VELOCITY_0_5_MPS, Model_OHT_ID);
            OBS_SENSOR_VEL_MIDDLE = CalVelRatio(VELOCITY_0_15_MPS, Model_OHT_ID);

            STOP_MOTION_DCC_HIGH = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
            STOP_MOTION_DCC_LOW = CalVelRatio(SPEED_0_5_MPS2, Model_OHT_ID);

            DYNAMIC_ACC_1st = CalVelRatio(SPEED_2_0_MPS2, Model_OHT_ID);
            DYNAMIC_ACC_2nd = CalVelRatio(SPEED_2_0_MPS2, Model_OHT_ID);
            DYNAMIC_ACC_3rd = CalVelRatio(SPEED_1_0_MPS2, Model_OHT_ID);
            DYNAMIC_ACC_4th = CalVelRatio(SPEED_1_0_MPS2, Model_OHT_ID); //CalVelRatio(SPEED_0_7_MPS2, Model_OHT_ID);

            Model_AXIS_STOP_COUNT_LIMIT = 3000;

            Model_DIST_ARRIVED_LEVEL1 = 5000;
            Model_DIST_ARRIVED_LEVEL2 = 1000;

            SelectChangeNode = 1;
            controller.localizer_->nodeCounter().setPulsetoMM(Model_PULSETOMM);
            controller.localizer_->nodePrecheckCounter().setPulsetoMM(Model_PULSETOMM);
            controller.localizer_->transFrontDetector().setPulsetoMM(Model_PULSETOMM);
            controller.localizer_->transStopDetector().setPulsetoMM(Model_PULSETOMM);
            CHANGENODE_DIST_LIMIT = 100 * Model_PULSETOMM;

            // Dynamic ACC
            ACC_CHANGE_SPEED1 = 1.2;
            ACC_CHANGE_SPEED2 = 3.3;
            ACC_CHANGE_SPEED3 = 5.0;
        } else if (Model_OHT_ID == OHT_NAME_P4MASK) {
            Model_PULSETOMM = P4M_PULSETOMM_DRIVE;
            Model_PULSETOMM_HOIST = P4M_PULSETOMM_HOIST;
            Model_PULSETOMM_SLIDE = P4M_PULSETOMM_SLIDE;

            /* implimanation required for each Variant */
            Model_OHT_DETECT_WRITE_BIT_MASK  = 0x00003C00;
            Model_OBS_DETECT_WRITE_BIT_MASK  = 0x000000F0;
            Model_OHT_DETECT_WRITE_BIT_SCALE = 10;
            Model_OBS_DETECT_WRITE_BIT_SCALE = 4;
            Model_OHT_DETECT_READ_BIT_SCALE  = 35;
            Model_OBS_DETECT_READ_BIT_SCALE  = 30;

            Model_DRIVINGPRECHECK = 55;
            Model_DRIVINGFRONTNUM = 54;
            Model_TRANSFRONT      = 56;
            Model_TRANSREAR       = 57;
            Model_HOISTHOME       = 69;

            Model_OHT_SENSOR_ENABLE = true;
            Model_OBS_SENSOR_MAKER  = HOKUYO;

            Model_MOVE_S_DCC_BY_SENSOR_NORMAL = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);
            Model_MOVE_S_DCC_BY_SENSOR_CURVE  = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);
            Model_MOVE_S_DCC_BY_SENSOR_ETC	  = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);
            Model_MOVE_DS_ACC_BY_SENSOR_CURVE = CalVelRatio(SPEED_1_0_MPS2, Model_OHT_ID);
            Model_MOVE_DS_DCC_BY_SENSOR_CURVE = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);
            Model_MOVE_DS_DCC_BY_SENSOR_ETC   = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);

            PATH_SLOPE_DCC = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);

            Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
            FollowsDCC_CMD = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;

            Model_OHT_Service = 0;

            Model_FILTER_4_NODECNT = 3;     // 사용 안 함

            Model_SPEED_GRADE_1_VELOCITY = CalVelRatio(VELOCITY_0_0_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_2_VELOCITY = CalVelRatio(VELOCITY_0_2_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_3_VELOCITY = CalVelRatio(VELOCITY_0_3_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_4_VELOCITY = CalVelRatio(VELOCITY_0_7_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_5_VELOCITY = CalVelRatio(VELOCITY_1_4_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_6_VELOCITY = CalVelRatio(VELOCITY_2_1_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_7_VELOCITY = CalVelRatio(VELOCITY_2_5_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_8_VELOCITY = CalVelRatio(VELOCITY_3_5_MPS, Model_OHT_ID);

            OBS_SENSOR_VEL_LONG = CalVelRatio(VELOCITY_0_5_MPS, Model_OHT_ID);
            OBS_SENSOR_VEL_MIDDLE = CalVelRatio(VELOCITY_0_15_MPS, Model_OHT_ID);

            STOP_MOTION_DCC_HIGH = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
            STOP_MOTION_DCC_LOW = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);

            DYNAMIC_ACC_1st = CalVelRatio(SPEED_2_0_MPS2, Model_OHT_ID);
            DYNAMIC_ACC_2nd = CalVelRatio(SPEED_2_0_MPS2, Model_OHT_ID);
            DYNAMIC_ACC_3rd = CalVelRatio(SPEED_1_0_MPS2, Model_OHT_ID);
            DYNAMIC_ACC_4th = CalVelRatio(SPEED_0_7_MPS2, Model_OHT_ID); //CalVelRatio(SPEED_0_7_MPS2, Model_OHT_ID);

            Model_AXIS_STOP_COUNT_LIMIT = 3000;

            Model_DIST_ARRIVED_LEVEL1 = 3000;
            Model_DIST_ARRIVED_LEVEL2 = 1000;

            SelectChangeNode = 1;
            controller.localizer_->nodeCounter().setPulsetoMM(Model_PULSETOMM);
            controller.localizer_->nodePrecheckCounter().setPulsetoMM(Model_PULSETOMM);
            controller.localizer_->transFrontDetector().setPulsetoMM(Model_PULSETOMM);
            controller.localizer_->transStopDetector().setPulsetoMM(Model_PULSETOMM);
            CHANGENODE_DIST_LIMIT = 100 * Model_PULSETOMM;

            // Dynamic ACC
            ACC_CHANGE_SPEED1 = 1.2;
            ACC_CHANGE_SPEED2 = 3.3;
            ACC_CHANGE_SPEED3 = 5.0;
        } else if (Model_OHT_ID == OHT_NAME_V82MASK) {
            Model_PULSETOMM = P4M_PULSETOMM_DRIVE;
            Model_PULSETOMM_HOIST = P4M_PULSETOMM_HOIST;
            Model_PULSETOMM_SLIDE = P4M_PULSETOMM_SLIDE;

            /* implimanation required for each Variant */
            Model_OHT_DETECT_WRITE_BIT_MASK  = 0x00003C00;
            Model_OBS_DETECT_WRITE_BIT_MASK  = 0x000000F0;
            Model_OHT_DETECT_WRITE_BIT_SCALE = 10;
            Model_OBS_DETECT_WRITE_BIT_SCALE = 4;
            Model_OHT_DETECT_READ_BIT_SCALE  = 35;
            Model_OBS_DETECT_READ_BIT_SCALE  = 30;

            Model_DRIVINGPRECHECK = 55;
            Model_DRIVINGFRONTNUM = 54;
            Model_TRANSFRONT      = 56;
            Model_TRANSREAR       = 57;
            Model_HOISTHOME       = 69;

            Model_OHT_SENSOR_ENABLE = true;
            Model_OBS_SENSOR_MAKER  = HOKUYO;

            Model_MOVE_S_DCC_BY_SENSOR_NORMAL = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);
            Model_MOVE_S_DCC_BY_SENSOR_CURVE  = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);
            Model_MOVE_S_DCC_BY_SENSOR_ETC	  = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);
            Model_MOVE_DS_ACC_BY_SENSOR_CURVE = CalVelRatio(SPEED_1_0_MPS2, Model_OHT_ID);
            Model_MOVE_DS_DCC_BY_SENSOR_CURVE = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);
            Model_MOVE_DS_DCC_BY_SENSOR_ETC   = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);

            PATH_SLOPE_DCC = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);

            Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
            FollowsDCC_CMD = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;

            Model_OHT_Service = 0;

            Model_FILTER_4_NODECNT = 3;     // 사용 안 함

            Model_SPEED_GRADE_1_VELOCITY = CalVelRatio(VELOCITY_0_0_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_2_VELOCITY = CalVelRatio(VELOCITY_0_2_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_3_VELOCITY = CalVelRatio(VELOCITY_0_3_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_4_VELOCITY = CalVelRatio(VELOCITY_0_7_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_5_VELOCITY = CalVelRatio(VELOCITY_1_4_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_6_VELOCITY = CalVelRatio(VELOCITY_2_1_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_7_VELOCITY = CalVelRatio(VELOCITY_2_5_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_8_VELOCITY = CalVelRatio(VELOCITY_3_5_MPS, Model_OHT_ID);

            OBS_SENSOR_VEL_LONG = CalVelRatio(VELOCITY_0_5_MPS, Model_OHT_ID);
            OBS_SENSOR_VEL_MIDDLE = CalVelRatio(VELOCITY_0_15_MPS, Model_OHT_ID);

            STOP_MOTION_DCC_HIGH = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
            STOP_MOTION_DCC_LOW = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);

            DYNAMIC_ACC_1st = CalVelRatio(SPEED_2_0_MPS2, Model_OHT_ID);
            DYNAMIC_ACC_2nd = CalVelRatio(SPEED_2_0_MPS2, Model_OHT_ID);
            DYNAMIC_ACC_3rd = CalVelRatio(SPEED_1_0_MPS2, Model_OHT_ID);
            DYNAMIC_ACC_4th = CalVelRatio(SPEED_0_7_MPS2, Model_OHT_ID); //CalVelRatio(SPEED_0_7_MPS2, Model_OHT_ID);

            Model_AXIS_STOP_COUNT_LIMIT = 3000;

            Model_DIST_ARRIVED_LEVEL1 = 3000;
            Model_DIST_ARRIVED_LEVEL2 = 1000;

            SelectChangeNode = 1;
            controller.localizer_->nodeCounter().setPulsetoMM(Model_PULSETOMM);
            controller.localizer_->nodePrecheckCounter().setPulsetoMM(Model_PULSETOMM);
            controller.localizer_->transFrontDetector().setPulsetoMM(Model_PULSETOMM);
            controller.localizer_->transStopDetector().setPulsetoMM(Model_PULSETOMM);
            CHANGENODE_DIST_LIMIT = 100 * Model_PULSETOMM;

            // Dynamic ACC
            ACC_CHANGE_SPEED1 = 1.2;
            ACC_CHANGE_SPEED2 = 3.3;
            ACC_CHANGE_SPEED3 = 5.0;
        } else {
            Model_PULSETOMM = V82_PULSETOMM_DRIVE;
            Model_PULSETOMM_HOIST = V82_PULSETOMM_HOIST;
            Model_PULSETOMM_SLIDE = V82_PULSETOMM_SLIDE;

            /* implimanation required for each Variant */
            Model_OHT_DETECT_WRITE_BIT_MASK  = 0x00003C00;
            Model_OBS_DETECT_WRITE_BIT_MASK  = 0x000000F0;
            Model_OHT_DETECT_WRITE_BIT_SCALE = 10;
            Model_OBS_DETECT_WRITE_BIT_SCALE = 4;
            Model_OHT_DETECT_READ_BIT_SCALE  = 35;
            Model_OBS_DETECT_READ_BIT_SCALE  = 30;

            Model_DRIVINGPRECHECK = 55;
            Model_DRIVINGFRONTNUM = 54;
            Model_TRANSFRONT      = 56;
            Model_TRANSREAR       = 57;
            Model_HOISTHOME       = 73;

            Model_OHT_SENSOR_ENABLE = true;
            Model_OBS_SENSOR_MAKER  = HOKUYO;

            Model_MOVE_S_DCC_BY_SENSOR_NORMAL = CalVelRatio(SPEED_3_5_MPS2, Model_OHT_ID);
            Model_MOVE_S_DCC_BY_SENSOR_CURVE  = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);
            Model_MOVE_S_DCC_BY_SENSOR_ETC	  = CalVelRatio(SPEED_2_5_MPS2, Model_OHT_ID);
            Model_MOVE_DS_ACC_BY_SENSOR_CURVE = CalVelRatio(SPEED_1_0_MPS2, Model_OHT_ID);
            Model_MOVE_DS_DCC_BY_SENSOR_CURVE = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);
            Model_MOVE_DS_DCC_BY_SENSOR_ETC   = CalVelRatio(SPEED_2_5_MPS2, Model_OHT_ID);

            PATH_SLOPE_DCC = CalVelRatio(SPEED_3_0_MPS2, Model_OHT_ID);

            Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
            FollowsDCC_CMD = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;

            Model_OHT_Service = 0;

            Model_FILTER_4_NODECNT = 3;     // 사용 안 함

            Model_SPEED_GRADE_1_VELOCITY = CalVelRatio(VELOCITY_0_0_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_2_VELOCITY = CalVelRatio(VELOCITY_0_3_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_3_VELOCITY = CalVelRatio(VELOCITY_0_3_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_4_VELOCITY = CalVelRatio(VELOCITY_1_0_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_5_VELOCITY = CalVelRatio(VELOCITY_2_2_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_6_VELOCITY = CalVelRatio(VELOCITY_3_3_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_7_VELOCITY = CalVelRatio(VELOCITY_4_1_MPS, Model_OHT_ID);
            Model_SPEED_GRADE_8_VELOCITY = CalVelRatio(VELOCITY_5_2_MPS, Model_OHT_ID);

            OBS_SENSOR_VEL_LONG = CalVelRatio(VELOCITY_0_5_MPS, Model_OHT_ID);
            OBS_SENSOR_VEL_MIDDLE = CalVelRatio(VELOCITY_0_15_MPS, Model_OHT_ID);

            STOP_MOTION_DCC_HIGH = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
            STOP_MOTION_DCC_LOW = CalVelRatio(SPEED_0_5_MPS2, Model_OHT_ID);

            DYNAMIC_ACC_1st = CalVelRatio(SPEED_2_0_MPS2, Model_OHT_ID);
            DYNAMIC_ACC_2nd = CalVelRatio(SPEED_2_0_MPS2, Model_OHT_ID);
            DYNAMIC_ACC_3rd = CalVelRatio(SPEED_1_0_MPS2, Model_OHT_ID);
            DYNAMIC_ACC_4th = CalVelRatio(SPEED_1_0_MPS2, Model_OHT_ID); //CalVelRatio(SPEED_0_7_MPS2, Model_OHT_ID);

            Model_AXIS_STOP_COUNT_LIMIT = 3000;

            Model_DIST_ARRIVED_LEVEL1 = 5000;
            Model_DIST_ARRIVED_LEVEL2 = 1000;

            SelectChangeNode = 1;
            controller.localizer_->nodeCounter().setPulsetoMM(Model_PULSETOMM);
            controller.localizer_->nodePrecheckCounter().setPulsetoMM(Model_PULSETOMM);
            controller.localizer_->transFrontDetector().setPulsetoMM(Model_PULSETOMM);
            controller.localizer_->transStopDetector().setPulsetoMM(Model_PULSETOMM);
            CHANGENODE_DIST_LIMIT = 100 * Model_PULSETOMM;

            // Dynamic ACC
            ACC_CHANGE_SPEED1 = 1.2;
            ACC_CHANGE_SPEED2 = 3.3;
            ACC_CHANGE_SPEED3 = 5.0;
        }

        motors[0]->setUserUnit("mm", Model_PULSETOMM);    // ServoMotor, 실제 기어비가 변경되는곳
        motors[1]->setUserUnit("mm", Model_PULSETOMM);
        motors[2]->setUserUnit("mm", Model_PULSETOMM_HOIST);
        motors[3]->setUserUnit("mm", Model_PULSETOMM_SLIDE);
        for(int i = 0; i<AMC_AXIS; i++) {
            if (enabled[i]) set_Amp_Enable(i, true);
        }
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Get_Oht_Model_id
//Target System   : OHT
//Brief           : Service Request No : 113
//Author          : Seungwan An
//Date            : 2017.11.13
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::Get_Oht_Model_id()
    {
        OHT_LOGT1("Get_Oht_Model_id handler is called!\n");

        dpram_.map.DPRamAxisMap[0].AD_IntType[0] = Model_OHT_ID;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : AcceptString
//Target System   : OHT
//Brief           : Service Request No : 152
//Author          : Seungwan An / Ray Lim
//Date            : 2017.11.13
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::AcceptString()
    {
        OHT_LOGT1("AcceptString handler is called!\n");
	auto& Dpram = dpram_;
        EXCHANGE_STRING *pExchange_str;
        char strRcv[250];
        char strSend[250];
        int rdchksum, calcchksum;
        int nrcvbytes, nsendbytes;

        pExchange_str = (EXCHANGE_STRING*)&Dpram.mem[576];
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : UploadParamToIPC
//Target System   : OHT
//Brief           : Service Request No : 153
//Author          : Seungwan An / Ray Lim
//Date            : 2017.11.13
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::UploadParamToIPC()
    {
        OHT_LOGT1("UploadParamToIPC handler is called!\n");
		auto& Dpram = dpram_;
        DOWNLOAD_PARAM *pDL_Param;
        EDUMP_HDR eDumpHdr;
        char *peDumpBody = NULL;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : DnloadParamFromIPC
//Target System   : OHT
//Brief           : Service Request No : 154
//Author          : Seungwan An / Ray Lim
//Date            : 2017.11.13
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::DnloadParamFromIPC()
    {
        OHT_LOGT1("DnloadParamFromIPC handler is called!\n");

		UBOOTPARA *pUBP;
        auto& Dpram = dpram_;
        DOWNLOAD_PARAM *pDL_Param;
        EDUMP_HDR eDumpHdr;
        char *peDumpBody = NULL;

        // Downloading to DSP
        // 1. (TotLen, CRC, BlkLen, BlkID=0) -> DSP
        // 2. BlkID=1..BlkLen, Date Set -> to DSP
        // 3. If no error Return TotLen=0, If error TotLen=-1
        pDL_Param = (DOWNLOAD_PARAM*)&Dpram.mem[576];
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : SetEncoderOffset
//Target System   : OHT
//Brief           : Service Request No : 157
//Author          : Seungwan An / Ray Lim
//Date            : 2017.11.13
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::SetEncoderOffset()
    {
        char ax = dpram_.map.Axis_Num;

        bool is_stop = oht_[ax]->isStopped();
        if (ax == OHT_WHEEL_R)
            is_stop = oht_[OHT_WHEEL_F]->isStopped();
        if (is_stop) {//Servo On/Off check
            ctr_param[ax].EncoderOffset = dpram_.map.DPRamAxisMap[ax].AD_LongType1;
            if (ax == OHT_SLIDE || ax == OHT_HOIST) {
                reset_status[ax] = oht_[ax]->resetEncoder(false);
            }
            else {
                //oht_[OHT_WHEEL_R]->resetEncoder(true);
                reset_status[ax] = oht_[ax]->resetEncoder(true);
                FollowsPOS = 0;
                FollowsPOS_Prev = FollowsPOS;
            }
				
        }
    }


////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : GetEncoderOffset
//Target System   : OHT
//Brief           : Service Request No : 158
//Author          : Ray Lim
//Date            : 2017.12.01
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::GetEncoderOffset()
    {
        OHT_LOGT1("GetEncoderOffset handler is called!\n");

        char ax = dpram_.map.Axis_Num;

        ctr_param[ax].EncoderOffset = 0;
        dpram_.map.DPRamAxisMap[ax].AD_LongType1 = ctr_param[ax].EncoderOffset;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : _dsp_param_flush
//Target System   : OHT
//Brief           : Service Request No : 159
//Author          : Seungwan An
//Date            : 2017.11.13
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::_dsp_param_flush()
    {
        OHT_LOGT1("_dsp_param_flush handler is called!\n");
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : WatchdogOperations
//Target System   : OHT
//Brief           : Service Request No : 160
//                  TODO - this function should be updated based on Real Watchdog in Master B/D
//Author          : Seungwan An
//Date            : 2017.11.13
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::WatchdogOperations()
    {
        int	cmd, nwhere, nrtn;

        OHT_LOGT1("WatchdogOperations handler is called!\n");

        // command 1:enable, 2:disable, 3:get status, 4:set status, 5:trigger
        cmd = dpram_.map.DPRamAxisMap[0].AD_IntType[0];
        nwhere = dpram_.map.DPRamAxisMap[0].AD_IntType[1];
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : FSExchange
//Target System   : OHT
//Brief           : Service Request No : 171
//Author          : Seungwan An
//Date            : 2017.11.13
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::FSExchange()
    {
        OHT_LOGT1("FSExchange handler is called!\n");
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : GetAMCData70
//Target System   : OHT
//Brief           : Service Request No : 180
//Author          : Seungwan An
//Date            : 2017.11.13
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::GetAMCData70()
    {
        //OHT_LOGT1("GetAMCData70 handler is called!\n");

        tm.beginInterval(0);
        // std::lock_guard<std::mutex> guard(oht_isr_lock_);
        const uint32_t SAFETY_CLEAR_MASK = 0x00800000;
        //const uint32_t SAFETY_CONTROL_MASK = 0x01000000;
        //static int Safety_status = 0;
        auto amc_cmd = dpram_.Amc_Cmd();
        //auto amc_data = dpram_.Amc_Data();
        AMC_DATA_RAW amc_data;
        //bool safety_clear_shm = readRawDo(safety_clear);
        bool safety_clear_shm = readRawDo(ioconfig_.getportid("safety_clear"));   // Safety(bumper) clear - 61
        bool safety_clear_cmd = bool(amc_cmd.Output[1] & SAFETY_CLEAR_MASK);
        bool safety_clear_read = safety_clear_shm;

        int temp_crc = amc_cmd.crc16;
        amc_cmd.crc16 = 0;

        if(temp_crc == crc16_ccitt((const char *)&amc_cmd, sizeof(amc_cmd))) {
            //Parse AMC_CMD
            if(GetBit(amc_cmd.Cmd, CMT_TYPE_SET_UBG_SETTING_FLAG)) {
                UBG_Setting_flag = 1;
            } else {
                UBG_Setting_flag = 0;
            }

			sound_->set_oht_status(amc_cmd.Sound);	// Main to AMC Sound Command, 210901 yjchoe

            if(GetBit(amc_cmd.Cmd, CMD_TYPE_SET_OUTBIT)){
                //Safety_status = (amc_cmd.Output[1] & Test_Mask);
                //OHT_LOGT1("[Test_Log-1] Safety_Clear : %x\n", Safety_status);

                unsigned char tmpOBSIO = (((amc_cmd.Output[1] & Model_OBS_DETECT_WRITE_BIT_MASK) >> Model_OBS_DETECT_WRITE_BIT_SCALE) & 0xf);
                unsigned char tmpOHTIO = (((amc_cmd.Output[1] & Model_OHT_DETECT_WRITE_BIT_MASK) >> Model_OHT_DETECT_WRITE_BIT_SCALE) & 0xf);

                if((amc_cmd.Output[3] & 0x8000) == false){
                    //                          - Clear Area Select -                               - Select Area1 -
                    amc_cmd.Output[1] = ((amc_cmd.Output[1] & (~Model_OHT_DETECT_WRITE_BIT_MASK)) | (OHT_Detect_AreaSetting_For_Curve<<Model_OHT_DETECT_WRITE_BIT_SCALE));
                    amc_cmd.Output[1] = ((amc_cmd.Output[1] & (~Model_OBS_DETECT_WRITE_BIT_MASK)) | (OBS_Detect_AreaSetting<<Model_OBS_DETECT_WRITE_BIT_SCALE));
                } else {
                    if(tmpOBSIO != OBS_Detect_AreaSetting) {
                        OBS_Detect_AreaSetting = tmpOBSIO;
                        //OHT_LOGT1("Set manual output, OBS = %x\n", tmpOBSIO);
                    }
                    if(UBG_Setting_flag && (tmpOHTIO != OHT_Detect_AreaSetting_For_Curve)) {
                        OHT_Detect_AreaSetting = OHT_Detect_AreaSetting_For_Curve = tmpOHTIO;
                        //OHT_LOGT1("Set manual output, OHT = %x\n", tmpOHTIO);
                    }
                }

//            if (safety_clear_shm != safety_clear_cmd) {
//                OHT_LOGT1("[safety_clear changed] %d -> %d", safety_clear_shm, safety_clear_cmd);
//                logStackTrace();
//            }
                //if (readRawDo(safety_control) !=  bool(amc_cmd.Output[1] & SAFETY_CONTROL_MASK) ) {
                //    OHT_LOGT1("[safety_control changed]");
                //   logStackTrace();
                //}

                //OHT_LOGT1("[Test_Log] Set IO Out\n");
                set_IoOut_Data(amc_cmd.Output);

                //Safety_status = (amc_cmd.Output[1] & Test_Mask);
                //OHT_LOGT1("[Test_Log-2] Safety_Clear : %x\n", Safety_status);
                //OHT_LOGT1("Set IO Out Data %08x %08x %08x %08x\n", amc_cmd.Output[0], amc_cmd.Output[1], amc_cmd.Output[2], amc_cmd.Output[3]);
            }
            tm.endInterval(0);
            tm.beginInterval(1);

            if(GetBit(amc_cmd.Cmd, CMD_TYPE_CLEAR_NODE_COUNT))
            {
                //OHT_LOGT1("Amc_Command = CMD_TYPE_CLEAR_NODE_COUNT\n" );
                // initiailize
                controller.localizer_->reset();

                amc_data.Node_Chk_Data.NodeCount = 0;
                //if (!readRawDi(_stop_driving_precheck)) {   // precheck 감지 상태 확인
                if (!readRawDi(ioconfig_.getportid("_stop_driving_precheck"))) {   // precheck 감지 상태 확인, Precheck - 55
                    controller.localizer_->nodePrecheckCounter().setCount(1);
                    /*OHT_LOGT1("[Localizer] Set Precheck count after clear count, [%d/%d]",
                              controller.localizer_->nodeCounter().getCount(),
                              controller.localizer_->nodePrecheckCounter().getCount());*/
                } else {
                    controller.localizer_->nodePrecheckCounter().setCount(0);
                }

                amc_data.Node_Chk_Data.PositionOfDrivingTag = 0;
                amc_data.Node_Chk_Data.PositionOfTransTagFirst = 0;
                amc_data.Node_Chk_Data.PositionOfTransTagFinal = 0;
                amc_data.Node_Chk_Data.PositionOfDrivingTagPreCheck = 0;

                //set_Position(OHT_WHEEL_F,0);
                //set_Position(OHT_WHEEL_R,0);
                motors[OHT_WHEEL_R]->setMasterAxis(OHT_WHEEL_F);

                // TODO: follow control related
                AMC_Stop_count = 0;

                OHT_DETECT_N_VALUE_Flag = false;
                OHT_Detect_AreaSetting_Flag = false;
                OHT_Detect_AreaSetting_Flag2 = false;

                Flag_FollowCtrl_Off_Edge = false;

                //PauseFlag = PAUSED_NONE;    //v10229 NodeCount리셋 시 Pause 풀리지 않도록 수정. DSO AMC에도 없는 내용 //CIS

                PBS_Ignore_Flag = false;
                OHT_PBS_IgnoreSettingFlag = false;

                //Follows_MoveDS_Flag = false;
                //Follow_Enable_Flag = false;

                Rear_servo_Off_flag = false;
                MoveDS_Wait_Count_After_Servo_On_Flag = false;
                MoveDS_Wait_Count_After_Servo_On = 0;

                FlagFirstNode = 1;

                //OHT_LOGT1("End of Clear Node");
            }

            tm.endInterval(1);
            tm.beginInterval(2);
            if(GetBit(amc_cmd.Cmd, CMD_TYPE_MOVE_AXIS_0))
            {
                //OHT_LOGT1("Amc_Command = CMD_TYPE_MOVE_AXIS_FRONT\n" );
                //OHT_LOGT1("NodeCount = %d\n", amc_cmd.DetectInfo.NodeCount);
                do_AxisCmd(amc_cmd, 0);
            }

            if(GetBit(amc_cmd.Cmd, CMD_TYPE_MOVE_AXIS_1))
            {
                //OHT_LOGT1("Amc_Command = CMD_TYPE_MOVE_AXIS_REAR\n" );
                do_AxisCmd(amc_cmd, 1);
            }

            if(GetBit(amc_cmd.Cmd, CMD_TYPE_MOVE_AXIS_2))
            {
                //OHT_LOGT1("Amc_Command = CMD_TYPE_MOVE_AXIS_HOIST\n" );
                do_AxisCmd(amc_cmd, 2);
            }

            if(GetBit(amc_cmd.Cmd, CMD_TYPE_MOVE_AXIS_3))
            {
                //OHT_LOGT1("Amc_Command = CMD_TYPE_MOVE_AXIS_SLIDE\n" );
                do_AxisCmd(amc_cmd, 3);
            }

            if(GetBit(amc_cmd.Cmd, CMD_TYPE_SET_FOLLOWS_MOVEDS_RELEASE)){
                //OHT_LOGT1("Amc_Command = CMD_TYPE_SET_FOLLOWS_MOVEDS_RELEASE\n" );
                Follows_MoveDS_Flag = false;
            }

            if(GetBit(amc_cmd.Cmd, CMD_TYPE_SET_FOLLOWS_MOVEDS)){
                //OHT_LOGT1("Amc_Command = CMD_TYPE_SET_FOLLOWS_MOVEDS\n" );
                Follows_MoveDS_Flag = true;
            }

            // change node 추가 필요
            if(GetBit(amc_cmd.Cmd, CMD_TYPE_SET_DETECT_INFO))
            {
                std::lock_guard<std::mutex> guard(oht_isr_lock_);
                /*OHT_LOGT1("Amc_Command = CMD_TYPE_SET_DETECT_INFO\n");
                OHT_LOGT1("UseFollowControlFlag = %d\n", amc_cmd.DetectInfo.UseFollowControlFlag);
                //OHT_LOGT1("Type = %d\n", amc_cmd.DetectInfo.Type);
                OHT_LOGT1("OHTDetectType = %d, OBSDetectType = %d\n", amc_cmd.DetectInfo.OHTDetectType, amc_cmd.DetectInfo.OBSDetectType);
                OHT_LOGT1("DistSumOfStraightPath = %d\n", amc_cmd.DetectInfo.DistSumOfStraightPath);
                OHT_LOGT1("FollowCommandType = %d\n", amc_cmd.DetectInfo.FollowCommandType);
                OHT_LOGT1("PBSIgnoreFlag = %d\n", amc_cmd.DetectInfo.PBSIgnoreFlag);*/

                detector_set_cmd_ = amc_cmd.DetectInfo;

                if ((detector_set_cmd_.FollowCommandType == DETECT_INFO_TYPE_ONLY_UBG) || (detector_set_cmd_.FollowCommandType == DETECT_INFO_TYPE_BOTH))
                {
                    unsigned char DetectOHT = 0, DetectOBS = 0;

                    DetectOHT = (unsigned char)(detector_set_cmd_.OHTDetectType);
                    DetectOBS = (unsigned char)(detector_set_cmd_.OBSDetectType);

                    if (SelectChangeNode) {
                        // Init ChangeNode Info
                        Init_ChangeNodeInfo();
                        OptNodeInfo_ = amc_cmd.OptNodeInfo;
                        controller.localizer_->nodeCounter().setNodeType(OptNodeInfo_.NodeType);

                        if ((OptNodeInfo_.NodeType == OPT_TAG || OptNodeInfo_.NodeType == OPT_DISTANCE || OptNodeInfo_.NodeType == OPT_COMBO)
                        && OptNodeInfo_.DistON > 0) {
                            ChangeNodeOHTDetectArea = ConvertOHTDetectInfoMainToAmc(OptNodeInfo_.OHTDetectTypeOpt);
                            ChangeNodeOBSDetectArea = ConvertOBSDetectInfoMainToAmc(OptNodeInfo_.OBSDetectTypeOpt);
                            //OHT_LOGT1("[Optional Node] DetectOHT:%d, DetectOBS:%d", ChangeNodeOHTDetectArea, ChangeNodeOBSDetectArea);
                        }
                    }

                    OHT_Detect_AreaSetting = ConvertOHTDetectInfoMainToAmc(DetectOHT);
                    tmp_OBS_Detect_AreaSetting = ConvertOBSDetectInfoMainToAmc(DetectOBS);

                    //OHT_LOGT1("DetectOHT:%d  DetectOBS:%d",DetectOHT,DetectOBS);
                }

                //OHT_Detect_ChangeNodeValue = detector_set_cmd_.NodeCount;
                Follows_MoveDS_Flag = detector_set_cmd_.UseFollowControlFlag;

                if(Follows_MoveDS_Flag == true)
                {
                    OHT_Detect_AreaSetting_Flag = true;
                }
                else
                {
                    if(OHT_Detect_AreaSetting == OHT_DETECT_NO_USE)
                    {
                        OHT_Detect_AreaSetting_Flag2 = true;
                    }
                    else
                    {
                        OHT_Detect_AreaSetting_Flag2 = false;
                    }
                    OHT_Detect_AreaSetting_Flag = false;
                }

                if (detector_set_cmd_.FollowCommandType == DETECT_INFO_TYPE_ONLY_PBS || detector_set_cmd_.FollowCommandType == DETECT_INFO_TYPE_BOTH)
                {
                    // todo PBS ignore
                    OHT_PBS_IgnoreChangeNodeValue = detector_set_cmd_.NodeCount;
                    OHT_PBS_IgnoreFlag = detector_set_cmd_.PBSIgnoreFlag;
                    OHT_PBS_IgnoreSettingFlag = true;
                }
            }
            if(GetBit(amc_cmd.Cmd, CMD_TYPE_SET_GEAR_RATIO_OF_DRIVING))    // Change Gear Ratio
            {
                //OHT_LOGT1("Amc_Command = CMD_TYPE_SET_GEAR_RATIO_OF_DRIVING\n" );
                if(TorqueLimit_Request == 1)
                {
                    TorqueLimit_Request = 0;
                    TorqueLimit_Response = 10;
                }
            }

            if(GetBit(amc_cmd.Cmd, CMD_TYPE_SET_DECEL_LIMIT_OF_DRIVING))    // Set Limit Of Dcc
            {
                //OHT_LOGT1("Amc_Command = CMD_TYPE_SET_DECEL_LIMIT_OF_DRIVING\n" );
                if(TorqueLimit_Request == 2)
                {
                    TorqueLimit_Request = 0;
                    TorqueLimit_Response = 20;
                }
            }

            if(GetBit(amc_cmd.Cmd, CMD_TYPE_COMPENSATE_REAR_ENCODER))
            {
                //OHT_LOGT1("Amc_Command = CMD_TYPE_COMPENSATE_REAR_ENCODER\n" );
                //TODO update
            }

            if(GetBit(amc_cmd.Cmd, CMD_TYPE_PAUSE_CONTROL))
            {
                //OHT_LOGT1("Amc_Command = CMD_TYPE_PAUSE_CONTROL\n" );
                PauseBy = amc_cmd.PauseInfo.Commander;
                PauseFlag = amc_cmd.PauseInfo.Pause;
            }

            if(GetBit(amc_cmd.Cmd, CMD_TYPE_SET_SLOPE_TIME))
            {
                //OHT_LOGT1("Amc_Command = CMD_TYPE_SET_SLOPE_TIME\n" );
                Path_Slope_Status = 1;
            }

            if(GetBit(amc_cmd.Cmd, CMD_TYPE_SET_SLOPE_TIME_RELEASE))
            {
                //OHT_LOGT1("Amc_Command = CMD_TYPE_SET_SLOPE_TIME_RELEASE\n" );
                Path_Slope_Status = 0;
            }

            if(GetBit(amc_cmd.Cmd, CMD_TYPE_SET_FINAL_ARRIVAL))
            {
                //OHT_LOGT1("Amc_Command = CMD_TYPE_SET_FINAL_ARRIVAL\n" );
                Final_Arrival_Flag = 1;
            }

            if(GetBit(amc_cmd.Cmd, CMD_TYPE_SET_FINAL_ARRIVAL_RELEASE))
            {
                //OHT_LOGT1("Amc_Command = CMD_TYPE_SET_FINAL_ARRIVAL_RELEASE\n" );
                Final_Arrival_Flag = 0;
            }

            // 강제 NodeCount 명령 추가 필요
            if(GetBit(amc_cmd.Cmd, CMD_TYPE_SET_NODE_COUNT)) {
                //OHT_LOGT1("Amc_Command = CMD_TYPE_SET_NODE_COUNT\n" );
                //if((NodeCount == 0) && ((pCBuf[0]->jtpos/Model_PULSETOMM) <= forcedNodeCountDistLimit) && !KeepEdgeCountStatus)	// KeepEdgeCountStatus - 현재 감지 여부
                if((controller.localizer_->nodeCounter().getCount() == 0) && (oht_[0]->getPosPulse()/Model_PULSETOMM <= forcedNodeCountDistLimit)) {
                    //NodeCount = 1;
                    controller.localizer_->nodeCounter().setCount(1);

                    if(FlagFirstNode == 1) {
                        //PreCheckCount = 1;
                        controller.localizer_->nodePrecheckCounter().setCount(1);
                        FlagFirstNode = 0;
                    }
                }
            }

            if(GetBit(amc_cmd.Cmd, CMD_TYPE_SET_LOW_VIBRATION))
            {
                //OHT_LOGT1("Amc_Command = CMD_TYPE_SET_LOW_VIBRATION\n" );
                LowVibration_Flag = 1;
            }

            if(GetBit(amc_cmd.Cmd, CMD_TYPE_SET_LOW_VIBRATION_RELEASE))
            {
                //OHT_LOGT1("Amc_Command = CMD_TYPE_SET_LOW_VIBRATION_RELEASE\n" );
                LowVibration_Flag = 0;
            }

            if(LowVibration_Flag){
                if(GetBit(amc_cmd.Cmd, CMD_TYPE_SET_SOFT_STOP_MOTION)){
                    //OHT_LOGT1("Amc_Command = CMD_TYPE_SET_SOFT_STOP_MOTION\n" );
                    Improve_stop_motion_flag = 1;
                }
                if(GetBit(amc_cmd.Cmd, CMD_TYPE_SET_SOFT_STOP_MOTION_RELEASE)){
                    //OHT_LOGT1("Amc_Command = CMD_TYPE_SET_SOFT_STOP_MOTION_RELEASE\n" );
                    Improve_stop_motion_flag = 0;
                }
            } else  Improve_stop_motion_flag = 0;

            if(GetBit(amc_cmd.Cmd, CMD_TYPE_CLEAR_FAULT))   // Servopack Alarm Clear
            {
                for(int i = 0; i < 4; i++)
                    oht_[i]->clearFaults();
                OHT_LOGT1("Amc_Command = CMD_TYPE_CLEAR_FAULT");
            }

            if(GetBit(amc_cmd.Cmd, CMD_TYPE_GET_TORQUE))
            {
                auto front_trq = front_wheel->getMaxTrq(300);
                auto rear_trq = rear_wheel->getMaxTrq(300);

                if(front_trq == 1 && rear_trq == 1) {
                    trqflag = 1;
                } else {
                    trqflag = 0;
                }
                OHT_LOGT1("Amc_Command = CMD_TYPE_GET_TORQUE, result[%d]", trqflag);
            }

            if(GetBit(amc_cmd.Cmd, CMD_TYPE_SET_TORQUE))
            {
                if (Path_Slope_Status == 1) {
                    front_wheel->setMaxTrq(280);
                    rear_wheel->setMaxTrq(200);
                } else {
                    front_wheel->setMaxTrq(300);
                    rear_wheel->setMaxTrq(300);
                }
                OHT_LOGT1("Amc_Command = CMD_TYPE_SET_TORQUE, Slope[%d]", Path_Slope_Status);
            }
        }

        tm.endInterval(2);
        tm.beginInterval(3);
        // Fill out the AMC_DATA --> oht_isr_ thread로 대체
        //statusUpdate1ms();

        // TODO - delete the Buffer
        for(char ax=0; ax<AMC_AXIS; ax++)
        {
            amc_data.AmpEnable[ax]      = motors[ax]->isEnabled();

            //amc_data.Inposition[ax]     = dpram_.map.DPRamAxisMap[ax].AD_In_Pos_Flag; // delete, 210617 yjchoe
            amc_data.Inposition[ax]     = (char)motors[ax]->inPosition();
            //amc_data.Running[ax]        = dpram_.map.DPRamAxisMap[ax].AD_In_Sequence;
            amc_data.Running[ax]        = runningflag[ax];

            amc_data.Source[ax]         = dpram_.map.DPRamAxisMap[ax].AD_AxisSource;
            amc_data.State[ax]          = oht_para_.event_type[ax];

            amc_data.TargetVel[ax]      = oht_[ax]->getRefVelPulse();
            //amc_data.TargetPos[ax]      = oht_[ax]->getRefPosPulse(); // yjchoe
            amc_data.TargetPos[ax] = 0;

            amc_data.Velocity[ax]       = oht_[ax]->getVelPulse();

//#ifndef MASTER_ARM_BOARD
               amc_data.Position[ax]       = oht_[ax]->getPosPulse();

            amc_data.FollowingError[ax] = oht_[ax]->getPosErrorPulse();

            //amc_data.FinalPos[ax]       = ctr_param[ax].dProfFinalPos;  // TODO update target final position  //CIS
            if(ax==0)
                amc_data.FinalPos[0]       = FollowsPOS;    //CIS
			amc_data.AxisFlag1[ax]      = 0;                            // Not used
			amc_data.AxisFlag2[ax]      = 0;                            // Not used
			//amc_data.PositionOfSmallAddCheck[ax]  = oht_[ax]->getMarkPosition();
			amc_data.PositionOfSmallAddCheck[ax]  = JtPos_During_Marking[ax];
		}

        amc_data.AxisFlag2[0] = (((motors[0]->getErrcode() << 16) & 0xffff0000) | (motors[1]->getErrcode() & 0x0000ffff));
        amc_data.AxisFlag2[1] = (((motors[2]->getErrcode() << 16) & 0xffff0000) | (motors[3]->getErrcode() & 0x0000ffff));
        amc_data.AxisFlag2[2] = Follows_MoveDS_Flag;    // 0: Ignore detect sensor   1: Normal   //CIS
		amc_data.AxisFlag2[3] = TorqueLimit_Request;    // 0 : init / 1 : Reset / 2 : set

        //amc_data.TargetPos[2] = 1;    ///CIS   low vibration // yjchoe
        // yjchoe
        //amc_data.TargetPos[0] = Motion_Flag[ax];  // Motion_Flag 추가 필요
        amc_data.TargetPos[1] = FollowsDCC_Prev;
        amc_data.TargetPos[2] = LowVibration_Flag;
        amc_data.TargetPos[3] = 0;

        amc_data.AxisFlag1[0] = currentSpeedMode[0];
        amc_data.AxisFlag1[1] = 0; // CRC
        amc_data.AxisFlag1[2] = Improve_stop_motion_flag;
        amc_data.AxisFlag1[3] = UBG_Setting_flag;

        // update DO data
		get_IoOut_Data(IO_Output);
		amc_data.Output[0] = IO_Output[0];
		amc_data.Output[1] = IO_Output[1];
		amc_data.Output[2] = IO_Output[2];
		amc_data.Output[3] = IO_Output[3];
		//amc_cmd.Output[1] = ((amc_cmd.Output[1] & (~Model_OHT_DETECT_WRITE_BIT_MASK)) | (OHT_Detect_AreaSetting_For_Curve<<Model_OHT_DETECT_WRITE_BIT_SCALE));
		//int output_test = (amc_data.Output[1] & Model_OHT_DETECT_WRITE_BIT_MASK) >> Model_OHT_DETECT_WRITE_BIT_SCALE;
		//OHT_LOGT1("AMC Output = %d", output_test);

		//safety_clear_read = bool(amc_data.Output[1] & SAFETY_CLEAR_MASK);
        //if (safety_clear_read != safety_clear_shm) {
		//    OHT_LOGT1("[safety_clear change confirm]");
        //}

        // susi->get_voltage(i)
        for(int i=0; i<3; i++) {
            //dc_voltage[i] = (int)(susi->get_voltage(i) * 1000) / 10000;   // 0: +12V / 1: 5V / 2: 3.3V / 3: X
            dc_voltage[i] = Onboard_Voltage(i) / 10; // 단위 확인 필요        // 0: Main 5V / 1: IO 3.3V / 2: Control 12V / 3: Control 5V / 4: Control 3.3V
        }
        //amc_data.VoltageMonitor[0] = (((dc_voltage[1] << 16) & 0xffff0000) | (dc_voltage[2] & 0x0000ffff));
        //amc_data.VoltageMonitor[1] = (((dc_voltage[3] << 16) & 0xffff0000) | (dc_voltage[0] & 0x0000ffff));
        amc_data.VoltageMonitor[0] = (((dc_voltage[0] << 16) & 0xffff0000) | (dc_voltage[1] & 0x0000ffff));
        amc_data.VoltageMonitor[1] = (((dc_voltage[2] << 16) & 0xffff0000) | (dc_voltage[2] & 0x0000ffff));

        // update DI data
		amc_data.Input[0] = IO_Input[0];
        amc_data.Input[1] = IO_Input[1];
        amc_data.Input[2] = IO_Input[2];
        amc_data.Input[3] = IO_Input[3];
        amc_data.PauseState = AMC_Follow_Control_Status;
        // Follows Data
        amc_data.Node_Chk_Data.NodeCount = controller.localizer_->nodeCounter().getCount();
        CheckNodeCountStatus(); // nodecount == 1 -> precheck = 1
        amc_data.Node_Chk_Data.NodeCountOfPreCheck = controller.localizer_->nodePrecheckCounter().getCount();

        amc_data.Node_Chk_Data.PositionOfDrivingTag = controller.localizer_->nodeCounter().getMarkTrip();
        amc_data.Node_Chk_Data.PositionOfTransTagFirst = controller.localizer_->transFrontDetector().getMarkTrip();
        amc_data.Node_Chk_Data.PositionOfTransTagFinal = controller.localizer_->transStopDetector().getMarkTrip();
        amc_data.Node_Chk_Data.PositionOfDrivingTagPreCheck = controller.localizer_->nodePrecheckCounter().getMarkTrip();

        //amc_data.AxisFlag1[1] = 0;     //  CIS
        amc_data.AxisFlag1[1] = crc16_ccitt((const char *)&amc_data,sizeof(amc_data));  //CIS
        dpram_.Amc_Data() = amc_data;
        //if(amc_cmd.CmdAxis[0].CmdType == AMC_CMD_AXIS_MOVE_DS)
        //OHT_LOGT1("[amc_oht] Pos_TransFront %d, Pos_TransRear %d, Pos_SmallAdd %d", amc_data.Node_Chk_Data.PositionOfTransTagFirst, amc_data.Node_Chk_Data.PositionOfTransTagFinal, amc_data.PositionOfSmallAddCheck[0]);
        tm.endInterval(3);
    }



    /// CIS Calculate crc
    unsigned short Amc4OHT::crc16_ccitt(const char *buf, int len)
    {
        register int counter;
        register unsigned short crc = 0;
        for ( counter = 0; counter < len; counter++)
            crc = (crc<<8) ^ crc16tab[((crc>>8) ^ *(char *)buf++)&0x00FF];
        return crc;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Get_Vel_Curve
//Target System   : OHT
//Brief           : Service Request No : 183
//Author          : Seungwan An
//Date            : 2017.11.13
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::Get_Vel_Curve()
    {
        int ax = dpram_.map.Axis_Num;

        OHT_LOGT1("Get_Vel_Curve handler is called!\n");

        ctr_param[ax].curve_limitst= dpram_.map.DPRamAxisMap[ax].AD_CharType[0];
        ctr_param[ax].curve_limit= dpram_.map.DPRamAxisMap[ax].AD_LongType1;

        OHT_LOGT1("Axis[%d] Limit : %d, Action : %d\n", ax, ctr_param[ax].curve_limit, ctr_param[ax].curve_limitst);
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Get_Actvel_Margin
//Target System   : OHT
//Brief           : Service Request No : 185
//Author          : Seungwan An
//Date            : 2017.11.13
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::Get_Actvel_Margin()
    {
        OHT_LOGT1("Get_Actvel_Margin handler is called!\n");

        int ax = dpram_.map.Axis_Num;

        ctr_param[ax].actvel_margin = dpram_.map.DPRamAxisMap[ax].AD_LongType1;
        ctr_param[ax].actvel_marginst = dpram_.map.DPRamAxisMap[ax].AD_CharType[0];
        ctr_param[ax].actvel_margintime = dpram_.map.DPRamAxisMap[ax].AD_IntType[0];
        ctr_param[ax].limit_curve_chk_cnt = 0;

        OHT_LOGT1("Axis[%d] Limit : %d, Action : %d, Time : %d\n", ax, ctr_param[ax].actvel_margin, ctr_param[ax].actvel_marginst, ctr_param[ax].actvel_margintime);
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : SetIOCount
//Target System   : OHT
//Brief           : Service Request No : 189
//Author          : Seungwan An
//Date            : 2017.11.13
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::SetIOCount()
    {
        OHT_LOGT1("SetIOCount handler is called!\n");

        io_in_count = dpram_.Cmd_Dpram().Io_outValue7; // input io count
        io_out_count = dpram_.Cmd_Dpram().Io_outValue8; // output io count

        OHT_LOGT1("DI : %d, DO : \n", io_in_count, io_out_count);
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : CalBit
//Target System   : OHT
//Brief           : Get Sensor value
//Author          : yjchoe
//Date            : 2020.08.11
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    int Amc4OHT::CalBit(uint32_t *IO_arr, int BIT_Index, int BIT_Size)
    {
        int IO_value = 0, temp = 0;
        int j = 0, Num = 0;
        int Array_size = 32;

        // IO 배열 위치 확인
        Num = BIT_Index / Array_size;
        // BIT 확인
        for(int i = 0; i < BIT_Size; i++)
        {
            temp = (IO_arr[Num] >> ((BIT_Index - (Num * Array_size)) + i)) & 0x1;
            //IO_value += temp * pow(2,j);
            IO_value |= temp << i;
            j++;
            // 배열 끝나면 다음 배열로
            if(((BIT_Index + i +1) % 32) == 0) Num++;
        }
        return IO_value;
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : MoveStopMotion
//Target System   : OHT
//Brief           : Select stop motion for normal or curve
//Author          : yjchoe
//Date            : 2020.08.11
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::MoveStopMotion(int select_curve) {
        unsigned int MoveFlag = 0;
        double Model_MOVE_S_DCC = 0;

        // 직선/커브 확인 후 감속도 결정
        if (select_curve == OHT_DETECT_NORMAL_VALUE) {  // 직선
            if (Path_Slope_Status == 0) {   // 경사 X
                if ((Improve_stop_motion_flag == 1)
                && (bStop_motion_flag == 1)
                && (OHT_Detect_AreaSetting_For_Curve != OHT_DETECT_HILL_VALUE)
                && (AMC_Follow_Control_Status == STATUS_OBS_STOP))
                    Model_MOVE_S_DCC = STOP_MOTION_DCC_LOW;
                else {
                    if (overtarget) {
                        Model_MOVE_S_DCC = CalVelRatio(SPEED_4_0_MPS2, Model_OHT_ID);
                        //SEP_LOGT("[FollowControl] change dec! 3.5 -> 4.0");
                    } else {
                        Model_MOVE_S_DCC = STOP_MOTION_DCC_HIGH;
                    }
                }
                MoveFlag = 1;
            } else {
                Model_MOVE_S_DCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
                MoveFlag = 2;
            }
        } else {    // 곡선
            if ((LowVibration_Flag) // 저진동 & 곡선 & 대차감지 - 1.5
            && (select_curve == OHT_DETECT_LEFT_VALUE || select_curve == OHT_DETECT_RIGHT_VALUE)
            && (AMC_Follow_Control_Status == STATUS_OBS_STOP || AMC_Follow_Control_Status == STATUS_NORMAL)) {
                Model_MOVE_S_DCC = Model_MOVE_S_DCC_BY_SENSOR_CURVE;
                MoveFlag = 3;
            } else {    // 저진동 X & 곡선 외 & 장애물 감지 - 2.5
                Model_MOVE_S_DCC = Model_MOVE_S_DCC_BY_SENSOR_ETC;
                MoveFlag = 4;
            }
        }

        if ((FollowsVEL_Prev != Target_velocity_cmd) || (FollowsDCC_Prev != Model_MOVE_S_DCC) || (stop_cnt < 2)) {
            //SEP_LOGT("[FollowControl] MoveStop_%d, DCC[%.0f], Cnt[%d]", MoveFlag, Model_MOVE_S_DCC, stop_cnt);
            front_wheel->stopPulse(Model_MOVE_S_DCC, false);
            FollowsDCC_Prev = Model_MOVE_S_DCC;
            FollowsVEL_Prev = Target_velocity_cmd;
			stop_cnt++;
            Move_Flag = 3;
            Debug_Flag = MoveFlag;
            Trace_Execute_1(0, FollowsACC_Prev, FollowsDCC_Prev, FollowsVEL_Prev, FollowsPOS_Prev);
        }
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : MoveDistanceMotion
//Target System   : OHT
//Brief           : Select stop motion for normal or curve
//Author          : yjchoe
//Date            : 2020.08.11
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::MoveDistanceMotion(int select_curve) {
        unsigned int MoveFlag = 0;
        double Model_MOVE_DS_ACC = 0, Model_MOVE_DS_DCC = 0;
        stop_cnt = 0;
        static int prev_Final_Arrival_Flag = 0;

        // 메인 명령 처리 직후 연속 처리되는 것을 방지
        if (cmd_flag) {
            if (cmd_delay++ >= 5) {
                cmd_flag = false;
                cmd_delay = 0;
            } else {
                //OHT_LOGT1("Main Command Processing.. wait.. %d", cmd_delay);
                return;
            }
        }

        // 직선/커브 확인 후 가/감속도 결정
        if(select_curve == OHT_DETECT_NORMAL_VALUE){    // 직선
            Model_MOVE_DS_ACC = tmpFollowsACC;
            Model_MOVE_DS_DCC = FollowsDCC;

            double RemainingDist = abs(FollowsPOS - oht_[0]->getPosPulse())/Model_PULSETOMM;
            // 최종 타겟까지 1000~5000mm 이내에서의 최대 속도는 5Lv, 1000mm 이내에서는 4Lv
            if(Final_Arrival_Flag == 1) {
                if((RemainingDist < Model_DIST_ARRIVED_LEVEL1) && (RemainingDist >= Model_DIST_ARRIVED_LEVEL2)) {
                    if (Target_velocity_cmd >= Model_SPEED_GRADE_5_VELOCITY)    Target_velocity_cmd = Model_SPEED_GRADE_5_VELOCITY;
                } else if(RemainingDist < Model_DIST_ARRIVED_LEVEL2) {
                    if (Target_velocity_cmd >= Model_SPEED_GRADE_4_VELOCITY)    Target_velocity_cmd = Model_SPEED_GRADE_4_VELOCITY;
                }
            }
            /*if(prev_Final_Arrival_Flag != Final_Arrival_Flag) {
                SEP_LOGT("[FollowControl] Final Arrival On, Remaining Distance[%d], TargetVel[%f]", RemainingDist, Target_velocity_cmd);
                prev_Final_Arrival_Flag = Final_Arrival_Flag;
            }*/
            MoveFlag = 5;
        } else {    // 곡선
            if ((LowVibration_Flag)
            && (select_curve == OHT_DETECT_LEFT_VALUE || select_curve == OHT_DETECT_RIGHT_VALUE)
            && (AMC_Follow_Control_Status == STATUS_OBS_STOP || AMC_Follow_Control_Status == STATUS_NORMAL)) {
                Model_MOVE_DS_ACC = Model_MOVE_DS_ACC_BY_SENSOR_CURVE;
                Model_MOVE_DS_DCC = Model_MOVE_DS_DCC_BY_SENSOR_CURVE;
                MoveFlag = 6;
            } else {
                Model_MOVE_DS_ACC = Model_MOVE_DS_ACC_BY_SENSOR_CURVE;
                Model_MOVE_DS_DCC = Model_MOVE_DS_DCC_BY_SENSOR_ETC;
                MoveFlag = 7;
            }
        }

        if (Follow_Enable_Flag &&
        (((int)FollowsACC_Prev != (int)Model_MOVE_DS_ACC) || ((int)FollowsDCC_Prev != (int)Model_MOVE_DS_DCC)
        || ((int)FollowsVEL_Prev != (int)Target_velocity_cmd) || (int)(FollowsPOS_Prev != (int)FollowsPOS))) {
            // MoveStop으로 target 넘어가는 정지 발생 후 감지 해제 시 후진 케이스 방지(E1033)
            if (FollowsPOS - oht_[0]->getPosPulse() >= 0) {  // 타겟 >= 현재 -> 전진
                front_wheel->moveToPulse(FollowsPOS, Target_velocity_cmd, Model_MOVE_DS_ACC, Model_MOVE_DS_DCC);
                FollowsPOS_Prev = FollowsPOS;
            }
            else {  // 타겟 < 현재 -> 후진
                FollowsPOS_Prev = FollowsPOS = oht_[0]->getPosPulse();  // 현재 위치로 업데이트
                MoveFlag = 55;
            }
            /*SEP_LOGT("[FollowControl] (MoveA_%d) Pos[%.0f] Pos[%.0f/%.0f] Vel[%.0f/%.0f] Acc[%.0f/%.0f] Dcc[%.0f/%.0f] // P[%d] V[%d] A[%d] D[%d]",
                     MoveFlag, oht_[0]->getPosPulse(),
                     FollowsPOS_Prev, FollowsPOS,
                     FollowsVEL_Prev, Target_velocity_cmd,
                     FollowsACC_Prev, Model_MOVE_DS_ACC,
                     FollowsDCC_Prev, Model_MOVE_DS_DCC,
                     (FollowsPOS_Prev != FollowsPOS),
                     (FollowsVEL_Prev != Target_velocity_cmd),
                     (FollowsACC_Prev != Model_MOVE_DS_ACC),
                     (FollowsDCC_Prev != Model_MOVE_DS_DCC));*/

            FollowsACC_Prev = Model_MOVE_DS_ACC;
            FollowsDCC_Prev = Model_MOVE_DS_DCC;
            FollowsVEL_Prev = Target_velocity_cmd;
            FollowsPOS_Prev = FollowsPOS;
            Move_Flag = 4;
            Debug_Flag = MoveFlag;
            Trace_Execute_1(0, FollowsACC_Prev, FollowsDCC_Prev, FollowsVEL_Prev, FollowsPOS_Prev);
        }
    }



    void Amc4OHT::MoveSmallAdd() {

    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : DynamicACC
//Target System   : OHT
//Brief           : Select Dynamic ACC
//Author          : yjchoe
//Date            : 2020.08.21
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    int Amc4OHT::DynamicACC(double vel) {
        static int rtnValue = FollowsACC;
        static int prev_rtn = 0;
        int ACC_Lv = 0;

        if ((currentSpeedMode[0] == ACC_MODE) && (vel > ACC_CHANGE_SPEED3)) {
            rtnValue = DYNAMIC_ACC_4th;
            ACC_Lv =4;
        } else if ((currentSpeedMode[0] == ACC_MODE) && (vel > ACC_CHANGE_SPEED2 && vel <= ACC_CHANGE_SPEED3)) {
            rtnValue = DYNAMIC_ACC_3rd;
            ACC_Lv = 3;
        } else if ((currentSpeedMode[0] == ACC_MODE) && (vel > ACC_CHANGE_SPEED1 && vel <= ACC_CHANGE_SPEED2)) {
            rtnValue = DYNAMIC_ACC_2nd;
            ACC_Lv = 2;
        } else if ((currentSpeedMode[0] == ACC_MODE || currentSpeedMode[0] == CRUISE_MODE) && (vel <= ACC_CHANGE_SPEED1)) {
            if (Target_Velocity_by_Sensor == Model_SPEED_GRADE_8_VELOCITY) {
                rtnValue = FollowsACC; // 전방에 OHT가 없는 경우
                ACC_Lv = -1;
            } else {
                rtnValue = DYNAMIC_ACC_1st;
                ACC_Lv = 1;
            }
        } else
            rtnValue = rtnValue;

        /*if(prev_rtn != rtnValue) {
            SEP_LOGT("[FollowControl] DynamicACC : %d, vel[%f], Lv[%d]", rtnValue, vel, ACC_Lv);
            //printf("vel = %f, ACC = %d, Lv = %d\n", vel, rtnValue, ACC_Lv);
            prev_rtn = rtnValue;
        }*/
        return rtnValue;
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : CurrentSpeedMode
//Target System   : OHT
//Brief           : speed mode of 4Axis
//Author          : yjchoe
//Date            : 2020.10.22
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::CurrentSpeedMode(int ax, double vel, double prev_vel) {
        double now_v = vel;
        double prev_v = prev_vel;
        double diff_v = now_v - prev_v;
        int prev_SpeedMode = currentSpeedMode[0];
        double acc_level = 0.3;

        if(now_v < 0)   diff_v *= -1;

        if (diff_v > acc_level) {   // 가속
            if(acc_count[ax]++ >= Acc_Offset) {
                currentSpeedMode[ax] = ACC_MODE;
                acc_count[ax] = Acc_Offset;
            } else {
                cruise_count[ax] = 0;
                dcc_count[ax] = 0;
            }
        } else if (diff_v < -acc_level) {   // 감속
            if(dcc_count[ax]++ >= Dcc_Offset) {
                currentSpeedMode[ax] = DCC_MODE;
                dcc_count[ax] = Dcc_Offset;
            } else {
                cruise_count[ax] = 0;
                acc_count[ax] = 0;
            }
        } else if ((diff_v <= acc_level) && (diff_v >= -acc_level)) {   // 등속
            if(cruise_count[ax]++ >= Cruise_Offset) {
                currentSpeedMode[ax] = CRUISE_MODE;
                cruise_count[ax] = Cruise_Offset;
            } else {
                acc_count[ax] = 0;
                dcc_count[ax] = 0;
            }
        } else {
            currentSpeedMode[ax] = NONE_MODE;
            cruise_count[ax] = 0;
            acc_count[ax] = 0;
            dcc_count[ax] = 0;
        }

        //if(motors[ax]->inPosition()) {
        if((int)now_v == 0 && (int)prev_v == 0) { // RefVel 0 -> Stop
            currentSpeedMode[ax] = STOP_MODE;
            cruise_count[ax] = 0;
            acc_count[ax] = 0;
            dcc_count[ax] = 0;
        }
        /*if (currentSpeedMode[0] != prev_SpeedMode)
            SEP_LOGT("[FollowControl] CurrentSpeedMode[0] : %d", currentSpeedMode[0]);*/
        /*
		static int prev_SpeedMode[4] = {0, };
		//if(ax == 0)
        {
			if(currentSpeedMode[ax] != prev_SpeedMode[ax])
				printf("currentSpeedMode = [ax : %d] [Mode : %d] [now_v : %d] [prev_v : %d] [diff_v : %d]\n", ax, currentSpeedMode[ax], now_v, prev_v, diff_v);
			prev_SpeedMode[ax] = currentSpeedMode[ax];
		}
        //printf("currentSpeedMode = [ax : %d] [Mode : %d] [now_v : %f] [prev_v : %f] [diff_v : %f]\n", ax, currentSpeedMode[ax], now_v, prev_v, diff_v);
        */
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : ConvertOHTDetectInfoMainToAmc
//Target System   : OHT
//Brief           : Convert OHT/OBS DetectInfo
//Author          : yjchoe
//Date            : 2020.10.28
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    int Amc4OHT::ConvertOHTDetectInfoMainToAmc(char DetectInfo) {
        char RetVal = OHT_DETECT_NO_USE;

        switch (DetectInfo) {
            case OHT_DETECT_CMD_PATH_DIRECTION_NO_USE:
                RetVal = OHT_DETECT_NO_USE;
                break;
            case OHT_DETECT_CMD_PATH_DIRECTION_NORMAL:
                RetVal = OHT_DETECT_NORMAL_VALUE;
                break;
            case OHT_DETECT_CMD_PATH_DIRECTION_LEFT:
                RetVal = OHT_DETECT_LEFT_VALUE;
                break;
            case OHT_DETECT_CMD_PATH_DIRECTION_RIGHT:
                RetVal = OHT_DETECT_RIGHT_VALUE;
                break;
            case OHT_DETECT_CMD_PATH_DIRECTION_SHORT:
                RetVal = OHT_DETECT_SHORT_VALUE;
                break;
            case OHT_DETECT_CMD_PATH_DIRECTION_BOTH:
                RetVal = OHT_DETECT_BOTH_VALUE;
                break;
            case OHT_DETECT_CMD_PATH_DIRECTION_NARROW:
                RetVal = OHT_DETECT_NARROW_VALUE;
                break;
            case OHT_DETECT_CMD_PATH_DIRECTION_NORMAL_HILL:
                RetVal = OHT_DETECT_HILL_VALUE;
                break;
            case OHT_DETECT_CMD_PATH_DIRECTION_RESERVED2:
                RetVal = OHT_DETECT_RESERVED2;
                break;
            case OHT_DETECT_CMD_PATH_DIRECTION_RESERVED3:
                RetVal = OHT_DETECT_RESERVED3;
                break;
            case OHT_DETECT_CMD_PATH_DIRECTION_RESERVED4:
                RetVal = OHT_DETECT_RESERVED4;
                break;
            case OHT_DETECT_CMD_PATH_DIRECTION_N_LEFT:
                RetVal = OHT_DETECT_NL_VALUE;
                break;
            case OHT_DETECT_CMD_PATH_DIRECTION_N_RIGHT:
                RetVal = OHT_DETECT_NR_VALUE;
                break;
            case OHT_DETECT_CMD_PATH_DIRECTION_DOUBLE_LEFT:
                RetVal = OHT_DETECT_DOUBLE_LEFT;
                break;
            case OHT_DETECT_CMD_PATH_DIRECTION_DOUBLE_RIGHT:
                RetVal = OHT_DETECT_DOUBLE_RIGHT;
                break;
            case OHT_DETECT_CMD_PATH_DIRECTION_RESERVED5:
                RetVal = OHT_DETECT_RESERVED5;
                break;
        }
        return RetVal;
    }

    int Amc4OHT::ConvertOBSDetectInfoMainToAmc(char DetectInfo) {
        char RetVal = HOKUYO_OBS_DETECT_NO_USE;

        switch (DetectInfo) {
            case OBS_DETECT_CMD_PATH_DIRECTION_NO_USE:
                RetVal = HOKUYO_OBS_DETECT_NO_USE;
                break;
            case OBS_DETECT_CMD_PATH_DIRECTION_NORMAL :
                RetVal = HOKUYO_OBS_DETECT_NORMAL_VALUE;
                break;
            case OBS_DETECT_CMD_PATH_DIRECTION_LEFT :
                RetVal = HOKUYO_OBS_DETECT_BOTH_VALUE;
                break;
            case OBS_DETECT_CMD_PATH_DIRECTION_RIGHT :
                RetVal = HOKUYO_OBS_DETECT_LEFT_VALUE;
                break;
            case OBS_DETECT_CMD_PATH_DIRECTION_SHORT :
                RetVal = HOKUYO_OBS_DETECT_RIGHT_VALUE;
                break;
            case OBS_DETECT_CMD_PATH_DIRECTION_BOTH :
                RetVal = HOKUYO_OBS_DETECT_RESERVED1;
                break;
            case OBS_DETECT_CMD_PATH_DIRECTION_NARROW :
                RetVal = HOKUYO_OBS_DETECT_RESERVED2;
                break;
            case OBS_DETECT_CMD_PATH_DIRECTION_RESERVED1 :
                RetVal = HOKUYO_OBS_DETECT_RESERVED3;
                break;
            case OBS_DETECT_CMD_PATH_DIRECTION_RESERVED2 :
                RetVal = HOKUYO_OBS_DETECT_RESERVED4;
                break;
            case OBS_DETECT_CMD_PATH_DIRECTION_RESERVED3 :
                RetVal = HOKUYO_OBS_DETECT_RESERVED5;
                break;
            case OBS_DETECT_CMD_PATH_DIRECTION_RESERVED4 :
                RetVal = HOKUYO_OBS_DETECT_RESERVED6;
                break;
            case OBS_DETECT_CMD_PATH_DIRECTION_N_LEFT :
                RetVal = HOKUYO_OBS_DETECT_RESERVED7;
                break;
            case OBS_DETECT_CMD_PATH_DIRECTION_N_RIGHT :
                RetVal = HOKUYO_OBS_DETECT_RESERVED8;
                break;
            case OBS_DETECT_CMD_PATH_DIRECTION_DOUBLE_LEFT :
                RetVal = HOKUYO_OBS_DETECT_RESERVED9;
                break;
            case OBS_DETECT_CMD_PATH_DIRECTION_DOUBLE_RIGHT :
                RetVal = HOKUYO_OBS_DETECT_RESERVED10;
                break;
            case OBS_DETECT_CMD_PATH_DIRECTION_RESERVED5 :
                RetVal = HOKUYO_OBS_DETECT_RESERVED11;
                break;
        }
        return RetVal;
    }

    void Amc4OHT::Init_ChangeNodeInfo() {
        EnableChangeNodeMode = false;
        controller.localizer_->nodeCounter().setOptionalNode(EnableChangeNodeMode);
        controller.localizer_->nodePrecheckCounter().setOptionalNode(EnableChangeNodeMode);
        SetAreaPosition = 0;
        SetAreaPosition_for_combo = 0;
        controller.localizer_->nodeCounter().setAreaPosition_for_combo(SetAreaPosition_for_combo);
        ChangeNodeSetFlag = false;
        controller.localizer_->nodeCounter().setChangeNodeSetFlag(ChangeNodeSetFlag);
        ChangeNodeComboSetFlag = false;
        controller.localizer_->nodeCounter().setChangeNodeComboSetFlag(ChangeNodeComboSetFlag);
        ChangeNodeOHTDetectArea = 0;
        ChangeNodeOBSDetectArea = 0;
        //OHT_LOGT1("[Optional Node] Initialize info");
    }

    double Amc4OHT::CalVelRatio(double Vel, int OHT_ID) {
        /*if(Model_OHT_ID == OHT_NAME_V90 || Model_OHT_ID == OHT_NAME_V85S) {
            return (Vel / 113.4115 * V82_PULSETOMM);
        } else {
            return Vel;
        }*/
        return (Vel / 113.4115 * Model_PULSETOMM);
    }

    int Amc4OHT::Onboard_Voltage(int ch) {
        float mV;
        pMax->readchannel(static_cast<fpx::tChannel>(ch), mV);
        //cout << "ch:" << ch << " " << static_cast<int>(mV) << " mV" << endl;
        return mV;
    }

    void Amc4OHT::CheckNodeCountStatus() {
        if (FlagFirstNode == 1) {
            // 첫 번째 노드인 경우 NodeCount 값이 증가하면 PreCheck 값도 증가시켜준다.
            if (controller.localizer_->nodeCounter().getCount() == 1 && controller.localizer_->nodePrecheckCounter().getCount() == 0) {
                controller.localizer_->nodePrecheckCounter().setCount(1);
                FlagFirstNode = 0;  // NodeCount Clear -> Flag = 0
                /*OHT_LOGT1("[Localizer] Set Precheck count, [%d/%d]",
                          controller.localizer_->nodeCounter().getCount(),
                          controller.localizer_->nodePrecheckCounter().getCount());*/
            }
        }
    }

    void Amc4OHT::Check_Monitoring_System() {
        static int POWER_Status_Cnt[4] = {0,};
        static bool powerlimit_flag[4] = {false,};
        static bool prev_powerflag[4] = {false,};

        for (int i = 0; i < AMC_AXIS-1; i++) {
            // 0: Main 5V / 1: IO 3.3V / 2: Control 12V / 3: Control 5V / 4: Control 3.3V
            if (dc_voltage[i] > Power_Upper_Limit[i] || dc_voltage[i] < Power_Lower_Limit[i]) {
                if (POWER_Status_Cnt[i]++ > POWER_CHECK_COUNT) {
                    for (int ax = 0; ax < AMC_AXIS; ax++) {
                        dpram_.map.DPRamAxisMap[ax].AD_AxisSource |= ST_SYSTEM_INSEC;
                        oht_para_.event_type[ax] = ABORT_EVENT;
                    }
                    powerlimit_flag[i] = true;
                }
            } else {
                POWER_Status_Cnt[i] = 0;
                powerlimit_flag[i] = false;
                prev_powerflag[i] = false;
            }
            
            
			if (powerlimit_flag[i] != prev_powerflag[i]) {
				OHT_LOGT1("DC Voltage limit detected! [%d/%d/%d] Axis source[0x%.4x]\n",
						dc_voltage[1], dc_voltage[0], dc_voltage[2], dpram_.map.DPRamAxisMap[i].AD_AxisSource);
				prev_powerflag[i] = powerlimit_flag[i];
			}
        }
    }


    void Amc4OHT::QueueInit(Queue *pq)
    {
        pq->front = 0;
        pq->rear = 0;
    }

    int Amc4OHT::QIsEmpty(Queue *pq)
    {
        if(pq->front == pq->rear)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    int Amc4OHT::NextPosIdx(int pos)
    {
        if(pos == (Trace_Data_Buffer_Size-1))
        {
            return 0;
        }
        else
        {
            return pos+1;
        }
    }

    void Amc4OHT::Enqueue(Queue *pq, int data)
    {
        if(NextPosIdx(pq->rear) == pq->front)
        {
            Dequeue(pq);
        }

        pq->rear = NextPosIdx(pq->rear);
        pq->queArr[pq->rear] = data;
    }

    int Amc4OHT::Dequeue(Queue *pq)
    {
        pq->front = NextPosIdx(pq->front);
        return pq->queArr[pq->front];
    }

    void Amc4OHT::MQueueInit(MQueue *pq)
    {
        pq->front = 0;
        pq->rear = 0;
    }

    int Amc4OHT::MQIsEmpty(MQueue *pq)
    {
        if(pq->front == pq->rear)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    int Amc4OHT::MNextPosIdx(int pos)
    {
        if(pos == (MTrace_Data_Buffer_Size-1))
        {
            return 0;
        }
        else
        {
            return pos+1;
        }
    }

    void Amc4OHT::MEnqueue(MQueue *pq, int data)
    {
        if(MNextPosIdx(pq->rear) == pq->front)
        {
            MDequeue(pq);
        }

        pq->rear = MNextPosIdx(pq->rear);
        pq->queArr[pq->rear] = data;
    }

    int Amc4OHT::MDequeue(MQueue *pq)
    {
        pq->front = MNextPosIdx(pq->front);
        return pq->queArr[pq->front];
    }

    void Amc4OHT::Trace_Execute(int ax)
    {
        int traceFlag;

        if (ax == 1)
            traceFlag = !motors[0]->inPosition();   // front축은 속도제어로 inpos가 올라오지 않아 rear축 inpos 사용
        else
            traceFlag = !motors[ax]->inPosition();  //runningflag[ax];    //gIsrSt[ax].bMoving;

        if(trace_on_flag[ax] > 0)
        {
            trace_on_flag[ax]--;
        }

        if(((traceFlag) || (trace_on_flag[ax])) && (!Trace_Update_Status))
        {
            Enqueue(AxisQueue[ax], (int)motors[ax]->getActPos());           // actual position
            Enqueue(AxisQueue[ax], (int)motors[ax]->getRefPulse());         // target position
            Enqueue(AxisQueue[ax], (int)motors[ax]->getActErr());           // following error
            Enqueue(AxisQueue[ax], (int)motors[ax]->getActual()[1]);        // actual velocity
            Enqueue(AxisQueue[ax], (int)oht_[ax]->getRefVel());             // target velocity

            Enqueue(AxisQueue[ax], (int)motors[ax]->getActual()[2]/10);     // torque
            Enqueue(AxisQueue[ax], (int)FollowsPOS);                        // command target position
            Enqueue(AxisQueue[ax], (int)Target_Velocity_by_Sensor/Model_PULSETOMM); // anticollision velocity
            Enqueue(AxisQueue[ax], 0);
            Enqueue(AxisQueue[ax], (int)Count_1ms_INT_4_Debug);  //Count_200us_INT_4_Debug);

            Enqueue(AxisQueue[ax], 0);
            Enqueue(AxisQueue[ax], (int)Debug_Flag);
            Enqueue(AxisQueue[ax], 0);
            Enqueue(AxisQueue[ax], (int)controller.localizer_->nodePrecheckCounter().getCount()*(int)1e4
                                    +(int)controller.localizer_->nodeCounter().getCount());
            Enqueue(AxisQueue[ax], (int)controller.localizer_->transFrontDetector().getCount()+(int)1e4
                                    +(int)controller.localizer_->transStopDetector().getCount());

            Enqueue(AxisQueue[ax], (int)controller.localizer_->nodePrecheckCounter().getRawCount()*(int)1e3
                                    +(int)controller.localizer_->nodeCounter().getRawCount()*(int)1e2
                                    +(int)controller.localizer_->transFrontDetector().getRawCount()*(int)1e1
                                    +(int)controller.localizer_->transStopDetector().getRawCount());
            Enqueue(AxisQueue[ax], (((OHT_Detect_AreaSetting_For_Curve<<16) & 0xffff0000) | (OBS_Detect_AreaSetting & 0xffff)));
            Enqueue(AxisQueue[ax], (((currentSpeedMode[ax]<<16) & 0xffff0000) | (0 & 0xffff)));
            Enqueue(AxisQueue[ax], (int)(OHT_velocity_Level/(Model_PULSETOMM*1e2))*(int)1e6 + (int)(OBS_velocity_Level/(Model_PULSETOMM*1e2))*(int)1e4
                                    + oht_sensor_val*(int)1e2 + obs_sensor_val);
            Enqueue(AxisQueue[ax], IO_Input[ax]);

            if(ax == 0)			Enqueue(AxisQueue[0], IO_Output[0]);
            else if(ax == 1)	Enqueue(AxisQueue[1], IO_Output[1]);
            else if(ax == 2)	Enqueue(AxisQueue[2], IO_Output[2]);
            else				Enqueue(AxisQueue[3], IO_Output[3]);
            if(ax == 0)         Enqueue(AxisQueue[0], JtPos_During_Marking[0]);
            else if(ax == 1)    Enqueue(AxisQueue[1], JtPos_During_Marking[1]);
            else if(ax == 2)    Enqueue(AxisQueue[2], JtPos_During_Marking[2]);
            else                Enqueue(AxisQueue[3], JtPos_During_Marking[3]);
            // 1|A|B|C|D|E|F|G|H|I 순서로 총 9가지 정보 표기
            // 좌측 첫번째 자리는 엑셀에서 모든 자리수 표기위한 숫자, 의미 X
            // 0~9 범위 내로 사용 할 것
            Enqueue(AxisQueue[ax], (int)1e9
                                   + Follows_MoveDS_Flag*(int)1e8 + OHT_PBS_IgnoreFlag*(int)1e7 + Follow_Enable_Flag*(int)1e6
                                   + AMC_Follow_Control_Status*(int)1e5 + bIsPosSoftStopToMark*(int)1e4 + bStop_motion_flag*(int)1e3
                                   + AMC_Pause_Control*(int)1e2 + LowVibration_Flag*(int)1e1 + Improve_stop_motion_flag);
            Enqueue(AxisQueue[ax], dpram_.map.DPRamAxisMap[ax].AD_AxisSource & 0xffff);
            Enqueue(AxisQueue[ax], oht_para_.event_type[ax]);

            if(traceFlag)
            {
                trace_on_flag[ax] = (int)(((Trace_Data_Buffer_Size-1)/Trace_Data_Count_M)*0.1);
            }
        }
    }

    void Amc4OHT::Trace_Execute_1(int ax, int ACC, int DCC, int VEL, int POS)
    {
        MEnqueue(MAxisQueue[ax], (int)Move_Flag);
        MEnqueue(MAxisQueue[ax], (int)ACC);
        MEnqueue(MAxisQueue[ax], (int)DCC);
        MEnqueue(MAxisQueue[ax], (int)VEL);
        MEnqueue(MAxisQueue[ax], (int)POS);

        MEnqueue(MAxisQueue[ax], (int)Debug_Flag);
        MEnqueue(MAxisQueue[ax], 0);
        MEnqueue(MAxisQueue[ax], (int)motors[ax]->getActual()[0]);
        MEnqueue(MAxisQueue[ax], (int)motors[ax]->getActual()[1]);
        MEnqueue(MAxisQueue[ax], Count_1ms_INT_4_Debug);

        MEnqueue(MAxisQueue[ax], (int)oht_[ax]->getRefPosPulse());
        MEnqueue(MAxisQueue[ax], (int)oht_[ax]->getRefVelPulse());
        MEnqueue(MAxisQueue[ax], 0);
        MEnqueue(MAxisQueue[ax], 0);
        MEnqueue(MAxisQueue[ax], 0);

        MEnqueue(MAxisQueue[ax], 0);
        MEnqueue(MAxisQueue[ax], (int)motors[ax]->getServoState());
        MEnqueue(MAxisQueue[ax], (int)dc_voltage[1]);
        MEnqueue(MAxisQueue[ax], (int)dc_voltage[0]);
        MEnqueue(MAxisQueue[ax], (int)dc_voltage[2]);
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : TraceUpdate_1
//Target System   : OHT
//Brief           : Service Request No : 178
//Author          : Seungwan An
//Date            : 2017.11.13
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::TraceUpdate_1()
    {
        //OHT_LOGT1("TraceUpdate_1 handler is called!\n");
        //int* DPRAMBUF = (int *)0xb00014C0;
        int* DPRAMBUF = dpram_.Dpram_buf();
        int i, j, ax;

        Trace_Update_Status = true;

        for( j = 0 ; j < TOTAL_AXIS_NUM ; j++ )
        {
            trace_on_flag[j] = 0;
        }

        //ax = ReadCommandAxis();
        ax = (int)dpram_.map.Axis_Num;

        for(i = 0 ; i < (Trace_Data_Transfer_Buffer_Size_C-1) ; i++)
        {
            if(!MQIsEmpty(MAxisQueue[ax]))
            {
                DPRAMBUF[i] = MDequeue(MAxisQueue[ax]);
            }
            else
            {
                break;
            }
        }

        DPRAMBUF[Trace_Data_Transfer_Buffer_Size_C-1] = i;

        if(i != (Trace_Data_Transfer_Buffer_Size_C-1))
        {
            Trace_Update_Status = false;

            for( j = 0 ; j < TOTAL_AXIS_NUM ; j++ )
            {
                trace_on_flag[j] = 0;
            }
        }
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : TraceUpdate
//Target System   : OHT
//Brief           : Service Request No : 179
//Author          : Seungwan An
//Date            : 2017.11.13
////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::TraceUpdate()
    {
        //OHT_LOGT1("TraceUpdate handler is called!\n");
//        int* DPRAMBUF = (int *)0xb00014C0;
        int* DPRAMBUF = dpram_.Dpram_buf();
        int i, j, ax;

        Trace_Update_Status = true;

        for( j = 0 ; j < TOTAL_AXIS_NUM ; j++ )
        {
            trace_on_flag[j] = 0;
        }

        //ax = ReadCommandAxis();
        ax = (int)dpram_.map.Axis_Num;
        for(i = 0; i < (Trace_Data_Transfer_Buffer_Size_M-1); i++)
        {
            if(!QIsEmpty(AxisQueue[ax]))
            {
                DPRAMBUF[i] = Dequeue(AxisQueue[ax]);
            }
            else
            {
                break;
            }
        }

        DPRAMBUF[Trace_Data_Transfer_Buffer_Size_M-1] = i;
        if(i != Trace_Data_Transfer_Buffer_Size_M-1)
        {
            Trace_Update_Status = false;

            for( j = 0; j < TOTAL_AXIS_NUM; j++ )
            {
                trace_on_flag[j] = 0;
            }
        }
    }

    void Amc4OHT::Check_refpos()
    {
        static int cnt, printcnt = 0;
        static bool flag = false;

        // 타겟 속도가 있을때(주행 중)
        if (motors[0]->getActVel()/Model_PULSETOMM > 200) {
            // 타겟위치가 이전위치랑 같으면 => 스레드 지연 발생
            if (motors[0]->getRefPulse() == prev_pos) {
                // 5회 연속 지연 시 카운트
                if (cnt++ >= 4) {
                    flag = true;
                }
            }
            else {
                if (flag) {
                    detectcnt++;
                    OHT_LOGT1("[Check Motion] delay detect! %d cnt %d", detectcnt, cnt);
                    flag = false;
                }
                cnt = 0;
            }
            prev_pos = motors[0]->getRefPulse(); // 이전 위치 갱신
        }

        if (printcnt++ >= 1000 * 60 * 10) { // 10분
            OHT_LOGT1("[Check Motion] Delay count : %d", detectcnt);
            //printf("count : %d\n", detectcnt);
            printcnt = 0;
        }
    }
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : DoAutopatch
//Target System   : OHT
//Brief           : Service Request No : 199
//Author          : sbhong
//Date            : 2022.04.12
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Amc4OHT::DoAutopatch() {

        patchlib inspatch;
        OHT_LOGT1("start ATLAS Autopatch process!\n");
        inspatch.dopatch();

            /*
            if(inspatch.listcheck() == 0){
                OHT_LOGT1("File check done, Do Autopatch!\n");
                inspatch.dopatch();
                OHT_LOGT1("AMC/EC-Master Autopatch done, restart\n");
                inspatch.restart();
            } else if(inspatch.listcheck() == 1){
                OHT_LOGT1("File check done, Do Autopatch!\n");
                inspatch.dopatch();
                OHT_LOGT1("AMC/EC-Master Autopatch done, run SlaveBD/Servo Autopatch!\n");
                inspatch.doservopatch(); //재시작은 스크립트에서 진행
            } else if(inspatch.listcheck() == 2){
                OHT_LOGT1("File check done, run Servo/SlaveBD Autopatch!\n");
                inspatch.doslavepatch(); //재시작은 스크립트에서 진행
            } else if(inspatch.listcheck() == 3){
                OHT_LOGT1("File check done, run SlaveBD Autopatch!\n");
                inspatch.doslavepatch(); //재시작은 스크립트에서 진행
            } else if(inspatch.listcheck() == 4){
                OHT_LOGT1("Error! No patch file exists!\n");
            } else if(inspatch.listcheck() == 5){
                OHT_LOGT1("Error! Too many patch files!\n");
            }
             */
        }

    //void Amc4OHT::check_actpos(int ax)
    void Amc4OHT::check_actpos()
    {
        int ax = 0;
        if (abs(motors[ax]->getRefPulse() - motors[ax]->getLoopbackPos()) > 1000) {
            //std::cout << "Target:" << motors[ax]->getRefPulse() << " Loopback:" << motors[ax]->getLoopbackPos() << std::endl;
        }
        // 이전 target 저장
        //prev_outpos[ax] = motors[ax]->getRefPulse();
    }

    void Amc4OHT::InitializeOhtModel() {
        OHT_LOGT1("Initialize OHT Model!\n");

        Model_OHT_ID = OHT_NAME_V82;
        OHT_Detect_AreaSetting = OHT_DETECT_NO_USE;
        OHT_Detect_AreaSetting_For_Curve = OHT_Detect_AreaSetting;
        OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_NORMAL_VALUE;
        OHT_Detect_ChangeNodeValue = 0;
        Target_Velocity_by_Sensor = Model_SPEED_GRADE_8_VELOCITY;

        Move_Flag = 0;
        Debug_Flag = 0;
        Count_1ms_INT_4_Debug = 0;
        Trace_Update_Status = false;
        Path_Slope_Status = 0;
        Servo_Off_type = 0;
        Final_Arrival_Flag = 0;
        Improve_stop_motion_flag = 0;
        bStop_motion_flag = 0;
        LowVibration_Flag = 0;
        UBG_Setting_flag = 0;

        /* implimanation required for each Variant */
        Model_OHT_DETECT_WRITE_BIT_MASK  = 0x00003C00;
        Model_OBS_DETECT_WRITE_BIT_MASK  = 0x000000F0;
        Model_OHT_DETECT_WRITE_BIT_SCALE = 10;
        Model_OBS_DETECT_WRITE_BIT_SCALE = 4;
        Model_OHT_DETECT_READ_BIT_SCALE  = 35;
        Model_OBS_DETECT_READ_BIT_SCALE  = 30;

        Model_DRIVINGPRECHECK = 55;
        Model_DRIVINGFRONTNUM = 54;
        Model_TRANSFRONT      = 56;
        Model_TRANSREAR       = 57;
        Model_HOISTHOME       = 73;

        Model_OHT_SENSOR_ENABLE = true;
        Model_OBS_SENSOR_MAKER  = HOKUYO;

        Model_MOVE_S_DCC_BY_SENSOR_NORMAL = CalVelRatio(SPEED_3_5_MPS2, Model_OHT_ID);
        Model_MOVE_S_DCC_BY_SENSOR_CURVE  = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);
        Model_MOVE_S_DCC_BY_SENSOR_ETC	  = CalVelRatio(SPEED_2_5_MPS2, Model_OHT_ID);
        Model_MOVE_DS_ACC_BY_SENSOR_CURVE = CalVelRatio(SPEED_1_0_MPS2, Model_OHT_ID);
        Model_MOVE_DS_DCC_BY_SENSOR_CURVE = CalVelRatio(SPEED_1_5_MPS2, Model_OHT_ID);
        Model_MOVE_DS_DCC_BY_SENSOR_ETC   = CalVelRatio(SPEED_2_5_MPS2, Model_OHT_ID);

        PATH_SLOPE_DCC = CalVelRatio(SPEED_3_0_MPS2, Model_OHT_ID);

        Model_FOLLOWSDCC = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;

        Model_OHT_Service = 0;

        Model_FILTER_4_NODECNT = 3;     // 사용 안 함

        Model_SPEED_GRADE_1_VELOCITY = CalVelRatio(VELOCITY_0_0_MPS, Model_OHT_ID);
        Model_SPEED_GRADE_2_VELOCITY = CalVelRatio(VELOCITY_0_3_MPS, Model_OHT_ID);
        Model_SPEED_GRADE_3_VELOCITY = CalVelRatio(VELOCITY_0_3_MPS, Model_OHT_ID);
        Model_SPEED_GRADE_4_VELOCITY = CalVelRatio(VELOCITY_1_0_MPS, Model_OHT_ID);
        Model_SPEED_GRADE_5_VELOCITY = CalVelRatio(VELOCITY_2_2_MPS, Model_OHT_ID);
        Model_SPEED_GRADE_6_VELOCITY = CalVelRatio(VELOCITY_3_3_MPS, Model_OHT_ID);
        Model_SPEED_GRADE_7_VELOCITY = CalVelRatio(VELOCITY_4_1_MPS, Model_OHT_ID);
        Model_SPEED_GRADE_8_VELOCITY = CalVelRatio(VELOCITY_5_2_MPS, Model_OHT_ID);

        OBS_SENSOR_VEL_LONG = CalVelRatio(VELOCITY_0_5_MPS, Model_OHT_ID);
        OBS_SENSOR_VEL_MIDDLE = CalVelRatio(VELOCITY_0_15_MPS, Model_OHT_ID);

        STOP_MOTION_DCC_HIGH = Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
        STOP_MOTION_DCC_LOW = CalVelRatio(SPEED_0_5_MPS2, Model_OHT_ID);

        DYNAMIC_ACC_1st = CalVelRatio(SPEED_2_0_MPS2, Model_OHT_ID);
        DYNAMIC_ACC_2nd = CalVelRatio(SPEED_2_0_MPS2, Model_OHT_ID);
        DYNAMIC_ACC_3rd = CalVelRatio(SPEED_1_0_MPS2, Model_OHT_ID);
        DYNAMIC_ACC_4th = CalVelRatio(SPEED_1_0_MPS2, Model_OHT_ID); //CalVelRatio(SPEED_0_7_MPS2, Model_OHT_ID);

        Model_AXIS_STOP_COUNT_LIMIT = 3000;

        Model_DIST_ARRIVED_LEVEL1 = 5000;
        Model_DIST_ARRIVED_LEVEL2 = 1000;

        Model_PULSETOMM = V82_PULSETOMM_DRIVE;

        SelectChangeNode = 1;
        controller.localizer_->nodeCounter().setPulsetoMM(Model_PULSETOMM);
        controller.localizer_->nodePrecheckCounter().setPulsetoMM(Model_PULSETOMM);
        controller.localizer_->transFrontDetector().setPulsetoMM(Model_PULSETOMM);
        controller.localizer_->transStopDetector().setPulsetoMM(Model_PULSETOMM);
        CHANGENODE_DIST_LIMIT = 100 * Model_PULSETOMM;

        for(int i = 0; i<4; i++)
        {
            Marking_Process_Flag[i] = false;
            JtPos_During_Marking[i] = 0;

            trace_on_flag[i] = 0;
            QueueInit(AxisQueue[i]);
            MQueueInit(MAxisQueue[i]);
        }

        MoveDSExecute = false;

        Follows_MoveDS_Flag = false;

        FollowsACC_Prev = 0;
        FollowsDCC_Prev = 0;
        FollowsVEL_Prev = 0;
        FollowsPOS_Prev = 0;

        FollowsDCC_CMD = 0;

        FollowsACC = 0;
        FollowsDCC = 0;
        FollowsVEL = 0;
        FollowsPOS = 0;

        for (int i = 0; i < AMC_AXIS; i++) {
            acc_count[i] = 0;
            dcc_count[i] = 0;
            cruise_count[i] = 0;
            prev_vel[i] = 0;
            dc_voltage[i] = 0;
            prevACC[i] = 0;
            prevDCC[i] = 0;
            prevVEL[i] = 0;
            prevPOS[i] = 0;
        }
        Acc_Offset = 30;
        Dcc_Offset = 30;
        Cruise_Offset = 30;

        // Dynamic ACC
        ACC_CHANGE_SPEED1 = 1.2;
        ACC_CHANGE_SPEED2 = 3.3;
        ACC_CHANGE_SPEED3 = 5.0;

        for (int ax = 0; ax < AMC_AXIS; ax++) {
            dpram_.map.DPRamAxisMap[ax].AD_AxisSource = ST_NONE;
            oht_para_.event_type[ax] = NO_EVENT;
            reset_status[ax] = true;
        }
        stop_cnt = 0;
        MoveDS_Wait_Count_After_Servo_On = 0;
        MoveDS_Wait_Count_After_Servo_On_Flag = false;
        Rear_servo_Off_flag = false;

        // 최초 1회 read
        for(int i=0; i<3; i++) {
            // 0: Main 5V / 1: IO 3.3V / 2: Control 12V / 3: Control 5V / 4: Control 3.3V
            dc_voltage[i] = Onboard_Voltage(i) / 10;
        }

        // Power Monitoring
        // 0: Main 5V / 1: IO 3.3V / 2: Control 12V / 3: Control 5V / 4: Control 3.3V
        Power_Upper_Limit[0] = (POWER5P_REF + ((POWER5P_REF*POWER5P_CHECK_Percent)/100));
        Power_Lower_Limit[0] = (POWER5P_REF - ((POWER5P_REF*POWER5P_CHECK_Percent)/100));
        Power_Upper_Limit[1] = (POWER3P3_REF + ((POWER3P3_REF*POWER3P3_CHECK_Percent)/100));
        Power_Lower_Limit[1] = (POWER3P3_REF - ((POWER3P3_REF*POWER3P3_CHECK_Percent)/100));
        Power_Upper_Limit[2] = (POWER12P_REF + ((POWER12P_REF*POWER12P_CHECK_Percent)/100));
        Power_Lower_Limit[2] = (POWER12P_REF - ((POWER12P_REF*POWER12P_CHECK_Percent)/100));
        Power_Upper_Limit[3] = (POWER12P_REF + ((POWER12P_REF*POWER12P_CHECK_Percent)/100));
        Power_Lower_Limit[3] = (POWER12P_REF - ((POWER12P_REF*POWER12P_CHECK_Percent)/100));
    }

    void Amc4OHT::GetVersion()
    {
        int slave_id = dpram_.map.Axis_Num;
        int rtn = 0;
        if(slave_id < TOTAL_AXIS_NUM) {
            char val[120] = {0,};
            char conv_val[120] = {0,};
            int cnt = 0;
            motors[slave_id]->coeSdoUpload(0x100A, 0x0, (uint8_t*)&val, sizeof(val));
            for (int i = 0; i < sizeof(val); i++) {
                if (val[i] == '\0')
                    break;  // 버전 정보 뒤에 쓰레기값 들어와 공백 확인 후 빠져나감
                if ((val[i] >= 0x30 && val[i] <= 0x39))
                    conv_val[cnt++] = val[i];
            }
            rtn = strtol(conv_val, NULL, 16);
        }
        else{
            uint32_t val = 0;
            if (slave_id == TOTAL_AXIS_NUM) {
                oht_rear_io->coeSdoUpload(0x9000, 0x1, (uint8_t*)&val, sizeof(val));
            } else if (slave_id == TOTAL_AXIS_NUM + 1) {
                oht_front_io->coeSdoUpload(0x9000, 0x1, (uint8_t*)&val, sizeof(val));
            }
            rtn = val;
        }
        dpram_.map.DPRamAxisMap[0].AD_LongType = rtn;
    }

    void Amc4OHT::GetEcmVersion()
    {
        string filename;
        char ecmver[100];
        filename = "EcmVersion.txt";
        std::ifstream fin(filename);

        if (fin.fail()) {
            printf("EC-Master version file cannot be found\n");
            return;
        } else {
            fin.getline(ecmver, sizeof(ecmver));
            dpram_.map.DPRamAxisMap[0].AD_LongType = atoi(ecmver);
        }
        fin.close();
    }
    void Amc4OHT::StopAll(){
		
		//if(motors[0]->getRefVel()!=0){   //근접제어에 의해 정지해 있을 수 있기 때문에 이 조건 제거. 무조건 정지 명령 실행
			printf("StopAll!!!!!!!!!!%f\n", motors[0]->getRefVel());
			double FollowsDCC_tmp = 1000;//amc_cmd.CmdAxis[ax].Decel;
			Follow_Enable_Flag = false;
			drv_cmd.enable_anticollision = false;
			oht_[0]->stopPulse(FollowsDCC_tmp, true);

            prevACC[0] = 0;
            prevDCC[0] = 0;
            prevVEL[0] = 0;
            prevPOS[0] = 0;
            Move_Flag = 33;
            Trace_Execute_1(0, 0, FollowsDCC_tmp, 0, 0);	
		//}	
	}
}

