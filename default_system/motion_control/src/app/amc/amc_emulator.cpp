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
// Created by sem1 on 17. 7. 25.
//

#include <functional>
#include "amc_emulator.h"

namespace amc
{
    AmcEmulator::AmcEmulator() :
            dpram_task_(&dpram_, this),
            service_map_{
                {0, &AmcEmulator::Reserved_Function},
                {1, &AmcEmulator::PTP_Move},
                {2, &AmcEmulator::CP_Move},
                {3, &AmcEmulator::PUT_Gain},
                {4, &AmcEmulator::GET_Gain},
                {5, &AmcEmulator::PUT_VGain},
                {6, &AmcEmulator::GET_VGain},
                {7, &AmcEmulator::GET_Position},
                {8, &AmcEmulator::PUT_Curr_Pos},
                {9, &AmcEmulator::PUT_Position},
                {10, &AmcEmulator::PUT_Error_Pls},
                {11, &AmcEmulator::GET_In_Position},
                {12, &AmcEmulator::PUT_In_Position},
                {13, &AmcEmulator::GET_Error_Limit},
                {14, &AmcEmulator::PUT_Error_Limit},
                {15, &AmcEmulator::PUT_Empty_Frame},
                {16, &AmcEmulator::GET_Boot_Memory},
                {17, &AmcEmulator::VEL_Move},
                {18, &AmcEmulator::GET_IO_Port},
                {19, &AmcEmulator::SET_IO_Bit},
                {20, &AmcEmulator::RESET_IO_Bit},
                {21, &AmcEmulator::SET_Delay},
                {22, &AmcEmulator::PUT_Stepper},
                {23, &AmcEmulator::GET_Stepper},
                {24, &AmcEmulator::Servo_On},
                {25, &AmcEmulator::Servo_Off},
                {26, &AmcEmulator::Alarm_Reset},
                {27, &AmcEmulator::Alarm_Set},
                {28, &AmcEmulator::Clear_Status},
                {29, &AmcEmulator::Get_Stop_Event},
                {30, &AmcEmulator::Get_Stop_Rate},
                {31, &AmcEmulator::Put_Stop_Rate},
                {32, &AmcEmulator::Get_E_Stop_Event},
                {33, &AmcEmulator::Get_E_Stop_Rate},
                {34, &AmcEmulator::Put_E_Stop_Rate},
                {35, &AmcEmulator::Get_Pos_Sn_Event},
                {36, &AmcEmulator::Put_Pos_Sn_Event},
                {37, &AmcEmulator::Get_Neg_Sn_Event},
                {38, &AmcEmulator::Put_Neg_Sn_Event},
                {39, &AmcEmulator::Get_Pos_Level},
                {40, &AmcEmulator::Put_Pos_Level},
                {41, &AmcEmulator::Get_Neg_Level},
                {42, &AmcEmulator::Put_Neg_Level},
                {43, &AmcEmulator::Get_Home_Sn_Event},
                {44, &AmcEmulator::Put_Home_Sn_Event},
                {45, &AmcEmulator::Get_Home_Level},
                {46, &AmcEmulator::Put_Home_Level},
                {47, &AmcEmulator::Get_Index_Req},
                {48, &AmcEmulator::Put_Index_Req},
                {49, &AmcEmulator::Get_Int_Stop},
                {50, &AmcEmulator::Get_Int_E_Stop},
                {51, &AmcEmulator::Put_Axis_Stat},
                {52, &AmcEmulator::Put_Axis_Source},
                {53, &AmcEmulator::Get_Amp_Fault},
                {54, &AmcEmulator::Put_Amp_Fault},
                {55, &AmcEmulator::Get_Amp_Fault_Level},
                {56, &AmcEmulator::Put_Amp_Fault_Level},
                {57, &AmcEmulator::Get_Amp_Reset_Level},
                {58, &AmcEmulator::Put_Amp_Reset_Level},
                {59, &AmcEmulator::Get_Dac_Offset},
                {60, &AmcEmulator::Put_Dac_Offset},
                {61, &AmcEmulator::Get_Feedback_Device},
                {62, &AmcEmulator::Put_Feedback_Device},
                {63, &AmcEmulator::Get_Voltage_Device},
                {64, &AmcEmulator::Put_Voltage_Device},
                {65, &AmcEmulator::Get_Axis_Boot_Memory},
                {66, &AmcEmulator::Get_Frames_Clear},
                {67, &AmcEmulator::Get_Closed_Loop},
                {68, &AmcEmulator::Put_Closed_Loop},
                {69, &AmcEmulator::Get_Amp_On_Level},
                {70, &AmcEmulator::Put_Amp_On_Level},
                {71, &AmcEmulator::Get_IO_Int_Enable},
                {72, &AmcEmulator::Put_Buf_Position},
                {73, &AmcEmulator::Clear_Stop},
                {74, &AmcEmulator::Main_Delay},
                {75, &AmcEmulator::Get_IO_Trigger},
                {76, &AmcEmulator::Get_SW_Upper_Limit},
                {77, &AmcEmulator::Put_SW_Upper_Limit},
                {78, &AmcEmulator::Get_SW_Lower_Limit},
                {79, &AmcEmulator::Put_SW_Lower_Limit},
                {80, &AmcEmulator::Put_IO_Out_Data},
                {81, &AmcEmulator::Get_Control_Cfg},
                {82, &AmcEmulator::Put_Control_Cfg},
                {83, &AmcEmulator::Get_PosImode},
                {84, &AmcEmulator::Put_PosImode},
                {85, &AmcEmulator::Get_VelImode},
                {86, &AmcEmulator::Put_VelImode},
                {87, &AmcEmulator::Get_GearRatio},
                {88, &AmcEmulator::Put_GearRatio},
                {89, &AmcEmulator::Get_SyncMap},
                {90, &AmcEmulator::Get_SyncOn},
                {91, &AmcEmulator::Get_SyncOff},
                {92, &AmcEmulator::Get_SyncGain},
                {93, &AmcEmulator::Put_SyncGain},
                {94, &AmcEmulator::Get_Compensation_Pos},
                {95, &AmcEmulator::Get_Pulse_Mode},
                {96, &AmcEmulator::Put_Pulse_Mode},
                {97, &AmcEmulator::Put_Encoder},
                {98, &AmcEmulator::Put_SyncEncoder},
                {99, &AmcEmulator::Get_Pulse_Ratio},
                {100, &AmcEmulator::Put_Pulse_Ratio},
                {101, &AmcEmulator::Get_Interpolation},
                {102, &AmcEmulator::Put_Empty_Interpolation},
                {103, &AmcEmulator::Get_Abs_Encoder},
                {104, &AmcEmulator::Get_Command},
                {105, &AmcEmulator::Reserved_Function},
                {106, &AmcEmulator::Reserved_Function},
                {107, &AmcEmulator::CP_Line_Move},
                {108, &AmcEmulator::Reserved_Function},
                {109, &AmcEmulator::Reserved_Function},
                {110, &AmcEmulator::Reserved_Function},
                {111, &AmcEmulator::Exist_Check},
                {112, &AmcEmulator::Set_Oht_Model_id},	//Diff between STK & OHT
                {113, &AmcEmulator::Get_Oht_Model_id},	//Diff between STK & OHT
                {114, &AmcEmulator::Reserved_Function},
                {115, &AmcEmulator::Reserved_Function},
                {116, &AmcEmulator::Reserved_Function},
                {117, &AmcEmulator::Reserved_Function},
                {118, &AmcEmulator::Reserved_Function},
                {119, &AmcEmulator::Reserved_Function},
                {120, &AmcEmulator::Reserved_Function},
                {121, &AmcEmulator::Reserved_Function},
                {122, &AmcEmulator::Reserved_Function},
                {123, &AmcEmulator::Reserved_Function},
                {124, &AmcEmulator::Reserved_Function},
                {125, &AmcEmulator::Reserved_Function},
                {126, &AmcEmulator::Reserved_Function},
                {127, &AmcEmulator::Reserved_Function},
                {128, &AmcEmulator::Reserved_Function},
                {129, &AmcEmulator::Reserved_Function},
                {130, &AmcEmulator::statusUpdate},
                {131, &AmcEmulator::Reserved_Function},
                {132, &AmcEmulator::Reserved_Function},
                {133, &AmcEmulator::Reserved_Function},
                {134, &AmcEmulator::Reserved_Function},
                {135, &AmcEmulator::Reserved_Function},
                {136, &AmcEmulator::Reserved_Function},
                {137, &AmcEmulator::Reserved_Function},
                {138, &AmcEmulator::Reserved_Function},
                {139, &AmcEmulator::Reserved_Function},
                {140, &AmcEmulator::DPRAMTest},
                {141, &AmcEmulator::Put_CleanVoltage},
                {142, &AmcEmulator::Reserved_Function},
                {143, &AmcEmulator::Reserved_Function},
                {144, &AmcEmulator::Reserved_Function},
                {145, &AmcEmulator::Reserved_Function},
                {146, &AmcEmulator::Reserved_Function},
                {147, &AmcEmulator::Reserved_Function},
                {148, &AmcEmulator::Reserved_Function},
                {149, &AmcEmulator::Reserved_Function},
                {150, &AmcEmulator::ECatReenable},
                {151, &AmcEmulator::SaveLongBlockToEeprom},
                {152, &AmcEmulator::AcceptString},	//Need to change
                {153, &AmcEmulator::UploadParamToIPC},
                {154, &AmcEmulator::DnloadParamFromIPC},
                {155, &AmcEmulator::RunConfigUploadToIPC},
                {156, &AmcEmulator::RunConfigSaveFromIPC},
                {157, &AmcEmulator::SetEncoderOffset},
                {158, &AmcEmulator::GetEncoderOffset},
                {159, &AmcEmulator::_dsp_param_flush},
                {160, &AmcEmulator::WatchdogOperations},
                {161, &AmcEmulator::Reserved_Function},
                {162, &AmcEmulator::SetEncoderDirection},
                {163, &AmcEmulator::GetEncoderDirection},
                {164, &AmcEmulator::SetSwPause},
                {165, &AmcEmulator::GetSwPauseEvent},
                {166, &AmcEmulator::SetSwPauseCheckBit},
                {167, &AmcEmulator::GetSwPauseCheckBit},
                {168, &AmcEmulator::GetMovexCmd},
                {169, &AmcEmulator::_dsp_clear_amc_error},
                {170, &AmcEmulator::dsp_Get_Error_Status},
                {171, &AmcEmulator::FSExchange},
                {172, &AmcEmulator::AMCFilteringSet},
                {173, &AmcEmulator::AMCFilteringGet},
                {174, &AmcEmulator::AMCStatusGet},
                {175, &AmcEmulator::Torque_Limit},
                {176, &AmcEmulator::restart_program},
                {177, &AmcEmulator::set_offset},
                {178, &AmcEmulator::TraceUpdate_1},
                {179, &AmcEmulator::TraceUpdate},
                {180, &AmcEmulator::GetAMCData70},
                {181, &AmcEmulator::ReloadEncoderPos},
                {182, &AmcEmulator::Put_Vel_Curve},
                {183, &AmcEmulator::Get_Vel_Curve},
                {184, &AmcEmulator::Put_Actvel_Margin},
                {185, &AmcEmulator::Get_Actvel_Margin},
                {186, &AmcEmulator::AMCStatusGet2},
                {187, &AmcEmulator::SetIoOutput},
                {188, &AmcEmulator::GetIoOutput},
                {189, &AmcEmulator::SetIOCount},
                {190, &AmcEmulator::GetIOCount},
                {191, &AmcEmulator::ReloadServopackAll},
                {192, &AmcEmulator::GetSystempStatus},
                {193, &AmcEmulator::SetSystemMoniEnable},
                {194, &AmcEmulator::SetSystemMoniDisable},
                {195, &AmcEmulator::GetSystemMoniEnable},
                {196, &AmcEmulator::GetSystemMoniValue},
                {197, &AmcEmulator::SetMoniteringThresholdPercent},
                {198, &AmcEmulator::GetMoniteringThresholdPercent},
                {199, &AmcEmulator::DoAutopatch},
                {200, &AmcEmulator::GetVersion},
                {201, &AmcEmulator::GetEcmVersion},
                {202, &AmcEmulator::StopAll},
        }
    {
        dpram_.map.printOffset();
    }

    AmcEmulator::~AmcEmulator()
    {
        stop();
    }

    bool AmcEmulator::start()
    {
        return dpram_task_.start();
    }

    void AmcEmulator::stop()
    {
        dpram_task_.stop();
        finalize();
    }

    bool AmcEmulator::loadConfig(const char* config_file_path)
    {
        SEP_LOGT("loadConfig is called!\n");
        return true;
    }
    
    bool AmcEmulator::stop_all(){
		auto& map = dpram_.map;
//        if((dpram_.mem[1022] != 180) && (dpram_.mem[1022] != 0) && (dpram_.mem[1022] != 130))
//            OHT_LOGT1(">> Axis[%d] handler No# : %d\n", dpram_.mem[1020], dpram_.mem[1022]);

        auto handler = service_map_.find(202);

        if (handler != service_map_.end()) {
            std::__invoke(handler->second, this); // key:handler->first, value:handler->second
            if (handler->first != 0 && handler->first != 180){}
                //printf("dispatch_%d\n", handler->first);
        } else {
            //SEP_LOGT("No handler is defined");
        }
        dpram_task_.update();

        map.Wakeup_DSP = 0; //Need to set 0 - SW An

        return true;
	}

    bool AmcEmulator::dispatch(bool initial_sync)
    {
        auto& map = dpram_.map;
//        if((dpram_.mem[1022] != 180) && (dpram_.mem[1022] != 0) && (dpram_.mem[1022] != 130))
//            OHT_LOGT1(">> Axis[%d] handler No# : %d\n", dpram_.mem[1020], dpram_.mem[1022]);

        auto handler = service_map_.find(initial_sync ? 130 : map.Wakeup_DSP);

        if (handler != service_map_.end()) {
            std::__invoke(handler->second, this); // key:handler->first, value:handler->second
            if (handler->first != 0 && handler->first != 180){}
                //printf("dispatch_%d\n", handler->first);
        } else {
            //SEP_LOGT("No handler is defined");
        }
        dpram_task_.update();

        map.Wakeup_DSP = 0; //Need to set 0 - SW An

        return true;
    }

    void AmcEmulator::statusUpdate()
    {
        SEP_LOGT("statusUpdate handler is called!\n");
    }

    void AmcEmulator::Reserved_Function()
    {
        SEP_LOGT("Reserved_Function handler is called!\n");
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : PTP_Move
//Target System   : Stocker
//Brief           : Service Request No : 1
//                  Requested by PTP_move(int ax, double pos, double vel, int acc, int dcc, int profile)
//                  PTP motion service
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::PTP_Move()
    {
        SEP_LOGT("PTP_Move handler is called!\n");
    }

    void AmcEmulator::CP_Move()
    {
        SEP_LOGT("CP_Move handler is called!\n");
    }

    void AmcEmulator::PUT_Gain()
    {
        auto& map = dpram_.map;
        int ax;
        unsigned int gain[5];

        ax = map.Axis_Num;

        //_dsp_get_filter(ax, gain);
        // Need to update -SW An

        for (int j = 0; j < 5; j ++)
            map.DPRamAxisMap[ax].AD_IntType[j] = gain[j];
    }

    void AmcEmulator::GET_Gain()
    {
        auto& map = dpram_.map;
        int ax;
        unsigned int gain[5];

        ax = map.Axis_Num;

        for (int j = 0; j < 5; j ++)
            gain[j] = map.DPRamAxisMap[ax].AD_IntType[j];


        //_dsp_set_filter(ax, gain);
        //Need to update - SW An
    }

    void AmcEmulator::PUT_VGain()
    {
        auto& map = dpram_.map;
        int ax;
        unsigned int vgain[5];

        ax = map.Axis_Num;

        //_dsp_get_v_filter(ax, vgain);
        // Need to update -SW An

        for (int j = 0; j < 5; j ++)
            map.DPRamAxisMap[ax].AD_IntType[j] = vgain[j];
    }
    void AmcEmulator::GET_VGain()
    {
        auto& map = dpram_.map;
        int ax;
        unsigned int vgain[5];

        ax = map.Axis_Num;

        for (int j = 0; j < 5; j ++)
            vgain[j] = map.DPRamAxisMap[ax].AD_IntType[j];

        //_dsp_set_v_filter(ax, vgain);
        //Need to update - SW An
    }

    void AmcEmulator::GET_Position()
    {
        auto& map = dpram_.map;
        int ax, pos;

        ax = map.Axis_Num;

        pos = map.DPRamAxisMap[ax].AD_LongType;

        //if(pCBuf[ax]->g_on != 0 )
        //{
        //	__SetPosition(ax, pos);
        //}
    }

    void AmcEmulator::PUT_Curr_Pos()
    {
        auto& map = dpram_.map;
        int ax, lpos;

        ax = map.Axis_Num;

        //lpos = __GetCurPos(ax);
        lpos = 0;// Need to Update

        map.DPRamAxisMap[ax].AD_LongType = lpos;
    }

    void AmcEmulator::PUT_Position()
    {
        auto& map = dpram_.map;
        int ax, ipos;

        ax = map.Axis_Num;

        //ipos = __GetPos(ax);
        ipos = 0; //Need to Update

        map.DPRamAxisMap[ax].AD_LongType = ipos;
    }

    void AmcEmulator::PUT_Error_Pls()
    {
        auto& map = dpram_.map;
        int ax;
        double ferr;

        ax = map.Axis_Num;

        //_dsp_get_error(	ax, &ferr);
        ferr = 0; //Need to update

        //WriteChannel(AxisDPRAM[ax],AD_FloatType1,TMS2IEEE(ferr));
        map.DPRamAxisMap[ax].AD_FloatType1 = ferr;
    }

    void AmcEmulator::GET_In_Position()
    {
        auto& map = dpram_.map;
        int	ax;
        float fpos;

        ax = map.Axis_Num;

        //fpos = IEEE2TMS(ReadChannel(AxisDPRAM[ax],AD_FloatType1));
        fpos = map.DPRamAxisMap[ax].AD_FloatType1;

        //_dsp_set_in_position(ax, fpos);
        //Need to update - SW An
    }

    void AmcEmulator::PUT_In_Position()
    {
        auto& map = dpram_.map;
        int ax;
        double fpos;

        ax = map.Axis_Num;

        //_dsp_get_in_position(ax, &fpos);	// 2007.10.27, ckyu
        fpos = 0;
        //Need to update - SW An

        //WriteChannel(AxisDPRAM[ax],AD_FloatType1,TMS2IEEE(fpos));
        map.DPRamAxisMap[ax].AD_FloatType1 = fpos;
    }

    void AmcEmulator::GET_Error_Limit()
    {
        auto& map = dpram_.map;
        int ax, lchk;
        char cst;

        ax = map.Axis_Num;

        lchk = map.DPRamAxisMap[ax].AD_LongType;
        cst = map.DPRamAxisMap[ax].AD_CharType[0];

        //__SetErrorLimit(ax, lchk, cst);
        //Need to Update
    }

    void AmcEmulator::PUT_Error_Limit()
    {
        auto& map = dpram_.map;
        int ax,lchk;
        char cst;

        ax = map.Axis_Num;

        //__GetErrorLimit(ax, &lchk, &cst);
        lchk = 0;
        cst = 0;//Need to update

        map.DPRamAxisMap[ax].AD_LongType = lchk;
        map.DPRamAxisMap[ax].AD_CharType[0] = cst;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : PUT_Empty_Frame
//Target System   : Stocker
//Brief           : Service Request No : 15
//                  Return true if frames left
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::PUT_Empty_Frame()
    {
        SEP_LOGT("PUT_Empty_Frame handler is called!\n");
    }

    void AmcEmulator::GET_Boot_Memory()
    {
        //ReadBootParam(&BootPara);
        // Need to update - SW An
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : VEL_Move
//Target System   : Stocker
//Brief           : Service Request No : 17
//                  Requested by PTP_Move(int ax double pos, double vel, int acc, int dcc, int profile)
//                  pos = 0, dcc = acc, profile = VELOCITY
//                  Constant Velocity move with an acceleration
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::VEL_Move()
    {
        SEP_LOGT("VEL_Move handler is called!\n");
    }

    void AmcEmulator::GET_IO_Port()
    {
        auto& Dpram = dpram_;
        COMMAND_DRRAM *pCommDPRAM;
        int	ld1, ld2;

        pCommDPRAM = (COMMAND_DRRAM*)&Dpram.mem[384];

        ld1 = pCommDPRAM->Io_outValue;
        ld2 = pCommDPRAM->Io_outValue2;

        //__SetIoOutData(ld1, ld2);
        //Need to update - SW An
    }

    void AmcEmulator::SET_IO_Bit()
    {
        auto& Dpram = dpram_;
        COMMAND_DRRAM *pCommDPRAM;
        int bit;

        pCommDPRAM = (COMMAND_DRRAM*)&Dpram.mem[384];

        bit = pCommDPRAM->Io_outValue;

        //_dsp_set_bit(bit);
        //Need to Update
    }

    void AmcEmulator::RESET_IO_Bit()
    {
        auto& Dpram = dpram_;
        COMMAND_DRRAM *pCommDPRAM;
        int bit;

        pCommDPRAM = (COMMAND_DRRAM*)&Dpram.mem[384];

        bit = pCommDPRAM->Io_outValue;

        //_dsp_reset_bit(bit);
        //Need to Update
    }

    void AmcEmulator::SET_Delay(void)
    {
        auto& map = dpram_.map;
        int	ax;
        int delay;

        ax = map.Axis_Num;

        delay = map.DPRamAxisMap[ax].AD_LongType;

        //_dsp_dwell(ax, delay);	// 2007.10.27, ckyu
        //Need to update - SW An
    }

    void AmcEmulator::PUT_Stepper(void)
    {
        auto& map = dpram_.map;
        int	ax, motortype;

        ax = map.Axis_Num;

        //motortype = _dsp_get_servo(ax);
        motortype = 0;
        //Need to update

        map.DPRamAxisMap[ax].AD_CharType[0] = motortype;

    }

    void AmcEmulator::GET_Stepper(void)
    {
        auto& map = dpram_.map;
        int	ax, motortype;

        ax = map.Axis_Num;

        motortype = map.DPRamAxisMap[ax].AD_CharType[0];

        //_dsp_set_motor_type(ax, motortype);
        //Need to update - SW An
    }
////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Servo_On
//Target System   : Stocker
//Brief           : Service Request No : 24
//                  Requested by set_amp_enable(int axno, int state)
//                  axno - Axis, state - 0 : Servo Off, 1 : Servo On
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::Servo_On()
    {
        SEP_LOGT("Servo_On handler is called!\n");
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Servo_Off
//Target System   : Stocker
//Brief           : Service Request No : 25
//                  Requested by set_amp_enable(int axno, int state)
//                  axno - Axis, state - 0 : Servo Off, 1 : Servo On
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::Servo_Off()
    {
        SEP_LOGT("Servo_Off handler is called!\n");
    }

    void AmcEmulator::Alarm_Reset()
    {
        SEP_LOGT("Alarm_Reset handler is called!\n");
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Alarm_Set
//Target System   : Stocker
//Brief           : Service Request No : 27
//                  requested by amp_fault_set(int axno)
//                  Amp fault preset
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::Alarm_Set()
    {
        SEP_LOGT("Alarm_Set handler is called!\n");
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Clear_Status
//Target System   : Stocker
//Brief           : Service Request No : 28
//                  Requested by clear_status(int ax)
//                  Reset controller status on an Axis
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::Clear_Status()
    {
        SEP_LOGT("Alarm_Set handler is called!\n");
    }

    void AmcEmulator::Get_Stop_Event()
    {
        auto& map = dpram_.map;
        int	ax;

        ax = map.Axis_Num;

        //_dsp_set_stop(ax);
        // Need to update - SW An
    }

    void AmcEmulator::Get_Stop_Rate()
    {
        auto& map = dpram_.map;
        int	ax, rate;

        ax = map.Axis_Num;
        rate = map.DPRamAxisMap[ax].AD_IntType[0];

        //_dsp_set_stop_rate(ax, ReadChannel2(AxisDPRAM[ax],AD_IntType1));
        //_dsp_set_stop_rate(ax, rate);
        //Need to update
    }

    void AmcEmulator::Put_Stop_Rate()
    {
        auto& map = dpram_.map;
        int	ax, rate;

        ax = map.Axis_Num;
        rate = 0;

        //_dsp_get_stop_rate(ax, &rate);
        //Need to update - SW An

        //WriteChannel2(AxisDPRAM[ax], AD_IntType1, rate);
        map.DPRamAxisMap[ax].AD_IntType[0] = rate;
    }

    void AmcEmulator::Get_E_Stop_Event()
    {
        auto& map = dpram_.map;
        int	ax;

        ax = map.Axis_Num;


        //_dsp_set_e_stop(ax);
        //Need to update - SW An
    }

    void AmcEmulator::Get_E_Stop_Rate()
    {
        auto& map = dpram_.map;
        int	ax, rate;

        ax = map.Axis_Num;

        rate = map.DPRamAxisMap[ax].AD_IntType[0];

        //_dsp_set_e_stop_rate(ax, rate);
        //Need to update - SW An
    }

    void AmcEmulator::Put_E_Stop_Rate()
    {
        auto& map = dpram_.map;
        int	ax, rate;

        ax = map.Axis_Num;
        rate = 0;

        //_dsp_get_e_stop_rate(ax, &rate);
        // Need to update - SW An

        map.DPRamAxisMap[ax].AD_IntType[0] = rate;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Get_Pos_Sn_Event
//Target System   : Stocker
//Brief           : Service Request No : 35
//                  Requested by set_positive_limit(int ax, int action)
//                  Set positive limit event
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::Get_Pos_Sn_Event()
    {
        SEP_LOGT("Get_Pos_Sn_Event handler is called!\n");
    }

    void AmcEmulator::Put_Pos_Sn_Event()
    {
        SEP_LOGT("Put_Pos_Sn_Event handler is called!\n");
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Get_Neg_Sn_Event
//Target System   : Stocker
//Brief           : Service Request No : 37
//                  Requested by set_negative_limit(int ax, int action)
//                  Set negative limit event
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::Get_Neg_Sn_Event()
    {
        SEP_LOGT("Get_Neg_Sn_Event handler is called!\n");
    }

    void AmcEmulator::Put_Neg_Sn_Event()
    {
        SEP_LOGT("Put_Neg_Sn_Event handler is called!\n");
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Get_Pos_Level
//Target System   : Stocker
//Brief           : Service Request No : 39
//                  Requested by set_positive_level(int ax, int level)
//                  Set positive sensor level
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::Get_Pos_Level()
    {
        SEP_LOGT("Get_Pos_Level handler is called!\n");
    }

    void AmcEmulator::Put_Pos_Level()
    {
        SEP_LOGT("Put_Pos_Level handler is called!\n");
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Get_Home_Level
//Target System   : Stocker
//Brief           : Service Request No : 41
//                  Requested by set_negative_level(int ax, int level)
//                  Set negative sensor level
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::Get_Neg_Level()
    {
        SEP_LOGT("Get_Neg_Level handler is called!\n");
    }

    void AmcEmulator::Put_Neg_Level()
    {
        SEP_LOGT("Put_Neg_Level handler is called!\n");
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Get_Home_Sn_Event
//Target System   : Stocker
//Brief           : Service Request No : 43
//                  Requested by set_home(int ax, int action)
//                  Set home sensor action
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::Get_Home_Sn_Event()
    {
        SEP_LOGT("Get_Home_Sn_Event handler is called!\n");
    }

    void AmcEmulator::Put_Home_Sn_Event()
    {
        SEP_LOGT("Put_Home_Sn_Event handler is called!\n");
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Get_Home_Level
//Target System   : Stocker
//Brief           : Service Request No : 45
//                  Requested by set_home_level(int ax, int level)
//                  Set home sensor level
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::Get_Home_Level()
    {
        SEP_LOGT("Get_Home_Level handler is called!\n");
    }

    void AmcEmulator::Put_Home_Level()
    {
        SEP_LOGT("Put_Home_Level handler is called!\n");
    }

    void AmcEmulator::Get_Index_Req()
    {
        auto& map = dpram_.map;
        int	ax,index;

        ax = map.Axis_Num;

        index = map.DPRamAxisMap[ax].AD_IntType[0];

        //_dsp_set_index_required(ax, index);
        // Need to update - SW An
    }

    void AmcEmulator::Put_Index_Req()
    {
        auto& map = dpram_.map;
        int	ax,index;

        ax = map.Axis_Num;

        //_dsp_get_index_required(ax, &index);
        index = 0;
        // Need to update - SW An

        map.DPRamAxisMap[ax].AD_IntType[0] = index;
    }

    void AmcEmulator::Get_Int_Stop()
    {
        auto& map = dpram_.map;
        int	ax, en;

        ax = map.Axis_Num;

        en = map.DPRamAxisMap[ax].AD_IntType[0];

        //_dsp_io_interrupt_on_stop(ax, en);
        // Need to update - SW-An
    }

    void AmcEmulator::Get_Int_E_Stop()
    {
        auto& map = dpram_.map;
        int	ax, en;

        ax = map.Axis_Num;

        en = map.DPRamAxisMap[ax].AD_IntType[0];

        //_dsp_io_interrupt_pcirq(ax, en);
        //Need to update - SW An
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Put_Encoder
//Target System   : Stocker
//Brief           : Service Request No : 51
//                  Requested by axis_state(int ax)
//                  Get the cause of an exception event
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::Put_Axis_Stat()
    {
        SEP_LOGT("Put_Axis_Stat handler is called!\n");
    }

    void AmcEmulator::Put_Axis_Source()
    {
        auto& map = dpram_.map;
        int	ax, src;

        ax = map.Axis_Num;

        //src = _dsp_axis_source(ax);
        src = 0; //Need to update

        map.DPRamAxisMap[ax].AD_AxisSource = src;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Get_Amp_Fault
//Target System   : Stocker
//Brief           : Service Request No : 53
//                  Requested by set_amp_fault(int ax, int action)
//                  Set amp fault input action
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::Get_Amp_Fault()
    {
        SEP_LOGT("Get_Amp_Fault handler is called!\n");
    }

    void AmcEmulator::Put_Amp_Fault()
    {
        SEP_LOGT("Put_Amp_Fault handler is called!\n");
    }

    void AmcEmulator::Get_Amp_Fault_Level()
    {
        auto& map = dpram_.map;
        int	ax, lvl;

        ax = map.Axis_Num;

        lvl = map.DPRamAxisMap[ax].AD_IntType[0];

        //_dsp_set_amp_fault_level(ax, lvl);
        // Need to update
    }

    void AmcEmulator::Put_Amp_Fault_Level()
    {
        auto& map = dpram_.map;
        int	ax, lvl;

        ax = map.Axis_Num;

        //_dsp_get_amp_fault_level(ax, &lvl);
        lvl = 0;
        //Need to update - SW An

        map.DPRamAxisMap[ax].AD_IntType[0] = lvl;
    }

    void AmcEmulator::Get_Amp_Reset_Level()
    {
        auto& map = dpram_.map;
        int	ax, lvl;

        ax = map.Axis_Num;

        lvl = map.DPRamAxisMap[ax].AD_IntType[0];

        //_dsp_set_amp_reset_level(ax, lvl);
        //Need to update - SW An
    }

    void AmcEmulator::Put_Amp_Reset_Level()
    {
        auto& map = dpram_.map;
        int	ax, lvl;

        ax = map.Axis_Num;

        //_dsp_get_amp_reset_level(ax, &lvl);
        lvl = 0;
        // Need to update

        map.DPRamAxisMap[ax].AD_IntType[0] = lvl;
    }

    void AmcEmulator::Get_Dac_Offset()
    {
        auto& map = dpram_.map;
        int	ax;

        ax = map.Axis_Num;

        /*
        if ((map.DPRamAxisMap[ax].AD_IntType[2]) == 0)
        {
            //_dsp_set_dac_output(ax, ReadChannel2(AxisDPRAM[ax],AD_IntType1));
            //Need to Update
        }
        else
        {
            //_dsp_set_analog_offset(ax, ReadChannel2(AxisDPRAM[ax],AD_IntType1));
            //Need to Update
        }
        */
    }

    void AmcEmulator::Put_Dac_Offset()
    {
        auto& map = dpram_.map;
        int	ax,volt;

        ax = map.Axis_Num;

        if ((map.DPRamAxisMap[ax].AD_IntType[2]) == 0)
        {
            //_dsp_get_dac_output(ax, &volt);
            volt = 0;//Need to Update
        }
        else
        {
            //_dsp_get_analog_offset(ax, &volt);
            volt = 0;//Need to Update
        }

        map.DPRamAxisMap[ax].AD_IntType[0] = volt;
    }

    void AmcEmulator::Get_Feedback_Device()
    {
        auto& map = dpram_.map;
        int	ax, cfg;

        ax = map.Axis_Num;

        cfg = map.DPRamAxisMap[ax].AD_IntType[0];

        //_dsp_set_feedback(ax, cfg);
        //Need to update - SW An
    }

    void AmcEmulator::Put_Feedback_Device()
    {
        {
            auto& map = dpram_.map;
            int	ax, cfg;

            ax = map.Axis_Num;

            //_dsp_get_feedback(ax, &cfg);
            cfg = 0;
            //Need to update

            map.DPRamAxisMap[ax].AD_IntType[0] = cfg;
        }
    }

    void AmcEmulator::Get_Voltage_Device()
    {
        auto& map = dpram_.map;
        int	ax, device;

        ax = map.Axis_Num;
        device = map.DPRamAxisMap[ax].AD_IntType[0];

        //_dsp_set_unipolar(ax, device);
        // Need to update - SW An
    }

    void AmcEmulator::Put_Voltage_Device()
    {
        auto& map = dpram_.map;
        int	ax, device;

        ax = map.Axis_Num;

        //_dsp_get_unipolar(ax, &dev);
        device = 0;
        //Need to update

        map.DPRamAxisMap[ax].AD_IntType[0] = device;
    }

    void AmcEmulator::Get_Axis_Boot_Memory()
    {
        auto& map = dpram_.map;
        int	ax;

        ax = map.Axis_Num;

        //__Set_Boot_Para(ax);
        //Need to update
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Get_Frames_Clear
//Target System   : Stocker
//Brief           : Service Request No : 66
//                  Requested by frames_clear(int ax)
//                  Reset Frame command
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::Get_Frames_Clear()
    {
        SEP_LOGT("Get_Frames_Clear handler is called!\n");
    }

    void AmcEmulator::Get_Closed_Loop()
    {
        auto& map = dpram_.map;
        int	ax, cfg;

        ax = map.Axis_Num;

        cfg = map.DPRamAxisMap[ax].AD_IntType[0];

        //_dsp_set_closed_loop(ax, cfg);
        //Need to update - SW An
    }

    void AmcEmulator::Put_Closed_Loop()
    {
        auto& map = dpram_.map;
        int	ax, cfg;

        ax = map.Axis_Num;

        //_dsp_get_closed_loop(ax, &cfg);
        cfg = 0;
        //Need to update - SW An

        map.DPRamAxisMap[ax].AD_IntType[0] = cfg;
    }

    void AmcEmulator::Get_Amp_On_Level()
    {
        auto& map = dpram_.map;
        int	ax;

        ax = map.Axis_Num;

        //_dsp_set_amp_enable_level(ax, ReadChannel2(AxisDPRAM[ax],AD_IntType1));
        //map.DPRamAxisMap[ax].AD_IntType[0]; Need to Update
    }

    void AmcEmulator::Put_Amp_On_Level()
    {
        auto& map = dpram_.map;
        int	ax, lvl;

        ax = map.Axis_Num;

        //_dsp_get_amp_enable_level(ax, &lvl);
        lvl = 0;//need to update

        map.DPRamAxisMap[ax].AD_IntType[0] = lvl;
    }

    void AmcEmulator::Get_IO_Int_Enable()
    {
        auto& map = dpram_.map;
        int	ax, Set_Enable;

        ax = map.Axis_Num;

        Set_Enable = map.DPRamAxisMap[ax].AD_IntType[0];

        //_dsp_io_interrupt_enable(ax, Set_Enable);
        //Need to update - SW An
    }

    void AmcEmulator::Put_Buf_Position()
    {
        auto& map = dpram_.map;
        int	ax, pos;

        ax = map.Axis_Num;

        //pos = _dsp_get_buf_position(ax);
        pos = 0;
        //Need to update

        map.DPRamAxisMap[ax].AD_LongType = pos;
    }

    void AmcEmulator::Clear_Stop()
    {
        SEP_LOGT("Get_Pos_Sn_Event handler is called!\n");
    }

    void AmcEmulator::Main_Delay()
    {
        auto& Dpram = dpram_;
        COMMAND_DRRAM *pCommDPRAM;

        pCommDPRAM = (COMMAND_DRRAM*)&Dpram.mem[384];

        int delay = Dpram.map.DPRamAxisMap[0].AD_LongType;

        //__MainDelay(delay);
        //Need to update

        pCommDPRAM->Delay = delay;
    }

    void AmcEmulator::Get_IO_Trigger()
    {
        auto& map = dpram_.map;
        int	ax,bitNo;
        char cst;

        ax = map.Axis_Num;

        bitNo = map.DPRamAxisMap[ax].AD_IntType[0];
        cst = map.DPRamAxisMap[ax].AD_CharType[0];

        //_dsp_io_trigger(ax, bitNo, cst);
        //Need to update
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Get_SW_Upper_Limit
//Target System   : Stocker
//Brief           : Service Request No : 76
//                  Requested by set_positive_sw_limit(int ax, double limit, int action)
//                  Set positive S/W limit & event
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::Get_SW_Upper_Limit()
    {
        SEP_LOGT("Get_SW_Upper_Limit handler is called!\n");
    }


    void AmcEmulator::Put_SW_Upper_Limit()
    {
        SEP_LOGT("Put_SW_Upper_Limit handler is called!\n");
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Get_SW_Lower_Limit
//Target System   : Stocker
//Brief           : Service Request No : 78
//                  Requested by set_negative_sw_limit(int ax, double limit, int action)
//                  Set Negative S/W limit & event
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::Get_SW_Lower_Limit()
    {
        SEP_LOGT("Get_SW_Lower_Limit handler is called!\n");
    }

    void AmcEmulator::Put_SW_Lower_Limit()
    {
        SEP_LOGT("Put_SW_Lower_Limit handler is called!\n");
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Put_IO_Out_Data
//Target System   : Stocker
//Brief           : Service Request No : 80
//                  Requested by get_out64(int port, int value[2])
//                  Get port value
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::Put_IO_Out_Data()
    {
        SEP_LOGT("Alarm_Set handler is called!\n");
    }

    void AmcEmulator::Get_Control_Cfg()
    {
        auto& map = dpram_.map;
        int ax, Control_Cfg;

        ax = map.Axis_Num;
        Control_Cfg = map.DPRamAxisMap[ax].AD_IntType[0];

        //_dsp_set_control(ax, Control_Cfg);
        //Need to update
    }

    void AmcEmulator::Put_Control_Cfg()
    {
        auto& map = dpram_.map;
        int ax, Control_Cfg;

        ax = map.Axis_Num;

        //_dsp_get_control(ax, &Control_Cfg);
        Control_Cfg = 0;
        //Need to update;

        map.DPRamAxisMap[ax].AD_IntType[0] = Control_Cfg;
    }

    void AmcEmulator::Get_PosImode()
    {
        auto& map = dpram_.map;
        int	ax, mode;

        ax = map.Axis_Num;
        mode = map.DPRamAxisMap[ax].AD_IntType[0];

        //_dsp_set_p_integration(ax, mode);
        // Need to update - SW An
    }

    void AmcEmulator::Put_PosImode()
    {
        auto& map = dpram_.map;
        int	ax, mode;

        ax = map.Axis_Num;

        //_dsp_get_p_integration(ax, &mode);
        mode = 0;
        //Need to update

        map.DPRamAxisMap[ax].AD_IntType[0] = mode;
    }

    void AmcEmulator::Get_VelImode()
    {
        auto& map = dpram_.map;
        int	ax, mode;

        ax = map.Axis_Num;
        mode = map.DPRamAxisMap[ax].AD_IntType[0];

        //_dsp_set_v_integration(ax, mode);
        //Need to update
    }

    void AmcEmulator::Put_VelImode()
    {
        auto& map = dpram_.map;
        int	ax, mode;

        ax = map.Axis_Num;

        //_dsp_get_v_integration(ax, &mode);
        mode = 0;
        //Need to update

        map.DPRamAxisMap[ax].AD_IntType[0] = mode;
    }

    void AmcEmulator::Get_GearRatio()
    {
        auto& map = dpram_.map;
        int	ax;
        float ratio;

        ax = map.Axis_Num;
        ratio = map.DPRamAxisMap[ax].AD_FloatType1;

        //_dsp_set_electric_gear(ax, (double)ratio);
        //Need to update
    }

    void AmcEmulator::Put_GearRatio()
    {
        auto& map = dpram_.map;
        int	ax;
        double ratio;

        ax = map.Axis_Num;

        //_dsp_get_electric_gear(ax, &ratio);
        ratio = 0;
        //Need to update

        map.DPRamAxisMap[ax].AD_FloatType1 = (float)ratio;
    }

    void AmcEmulator::Get_SyncMap()
    {
        auto& map = dpram_.map;

        //_dsp_set_sync_map_axes(ReadChannel2(CommDPRAM,CD_Axis), ReadChannel2(CommDPRAM,CD_Axis+2));
        //map._Amc._Cmd.Cmd_Dpram.Axis[0],map._Amc._Cmd.Cmd_Dpram.Axis[1]
    }

    void AmcEmulator::Get_SyncOn()
    {
        //SyncMotion.Flag = 1;
        //need to update
    }

    void AmcEmulator::Get_SyncOff()
    {
        //SyncMotion.Flag = 0;
        //need to update
    }

    void AmcEmulator::Get_SyncGain()
    {
        auto& map = dpram_.map;
        int	ax;
        float syncgain, tmp;

        ax = map.Axis_Num;

        //tmp = IEEE2TMS(ReadChannel(AxisDPRAM[ax], AD_FloatType1));
        tmp = map.DPRamAxisMap[ax].AD_FloatType1;

        if(tmp > 1.03)
        {
            syncgain = 1.03;
        }
        else if(tmp < 0.97)
        {
            syncgain = 0.97;
        }
        else
        {
            syncgain = tmp;
        }

        //_dsp_set_sync_gain(syncgain);
        //need to update
    }

    void AmcEmulator::Put_SyncGain()
    {
        auto& map = dpram_.map;
        int	ax;
        float sgain;

        ax = map.Axis_Num;

        //_dsp_get_sync_gain(&sgain);
        sgain = 1;//need to update

        //WriteChannel(AxisDPRAM[ax],AD_FloatType1, TMS2IEEE(sgain));
        map.DPRamAxisMap[ax].AD_FloatType1 = sgain;
    }

    void AmcEmulator::Get_Compensation_Pos()
    {
        // Not used
    }

    void AmcEmulator::Get_Pulse_Mode()
    {
        auto& map = dpram_.map;
        int	ax, mode;

        ax = map.Axis_Num;
        mode = map.DPRamAxisMap[ax].AD_IntType[0];

        //_dsp_set_step_mode(ax, mode);
        //Need to update
    }

    void AmcEmulator::Put_Pulse_Mode()
    {
        auto& map = dpram_.map;
        int	ax, mode;

        ax = map.Axis_Num;

        //_dsp_get_step_mode(ax, &mode);
        mode = 0;
        //Need to update
        map.DPRamAxisMap[ax].AD_IntType[0] = mode;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Put_Encoder
//Target System   : Stocker
//Brief           : Service Request No : 97
//                  Requested by get_position(int ax, double* pos)
//                  Get actual position
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::Put_Encoder()
    {
        SEP_LOGT("Alarm_Set handler is called!\n");
    }

    void AmcEmulator::Put_SyncEncoder()
    {
        auto& map = dpram_.map;
        int	pos_m,pos_s;

        //__GetSyncEncoder(&pos_m, &pos_s);
        pos_m = 0;//need to update
        pos_s = 0;//need to update

        //WriteChannel(AxisDPRAM[SyncMotion.Master],AD_LongType, pos_m);
        //WriteChannel(AxisDPRAM[SyncMotion.Slave],AD_LongType, pos_s);
        map.DPRamAxisMap[0].AD_LongType = pos_m;
        map.DPRamAxisMap[1].AD_LongType = pos_s;
    }

    void AmcEmulator::Get_Pulse_Ratio()
    {
        auto& map = dpram_.map;
        int	ax, ratio;

        ax = map.Axis_Num;

        ratio = map.DPRamAxisMap[ax].AD_IntType[0];

        //_dsp_set_pulse_ratio(ax, ratio);
        //Need to update
    }

    void AmcEmulator::Put_Pulse_Ratio()
    {
        auto& map = dpram_.map;
        int	ax, ratio;

        ax = map.Axis_Num;

        //_dsp_get_pulse_ratio(ax, &ratio);
        ratio = 0;
        //Need to update

        map.DPRamAxisMap[ax].AD_IntType[0] = ratio;
    }

    void AmcEmulator::Get_Interpolation()
    {
        //__SetInterpolation();
        //Need to update
    }

    void AmcEmulator::Put_Empty_Interpolation()
    {
        auto& map = dpram_.map;
        int	ax;

        ax = map.Axis_Num;

        //WriteChannel2(AxisDPRAM[ax],AD_IntType1,__GetEmptyInterpolation(ax));
        //Need to update
    }

    void AmcEmulator::Get_Abs_Encoder()
    {
        auto& map = dpram_.map;
        int	ax;

        ax = map.Axis_Num;

        map.DPRamAxisMap[ax].AD_CharType[0] = 1;
    }

    void AmcEmulator::Get_Command()
    {
        auto& map = dpram_.map;
        int	ax, pos;

        ax = map.Axis_Num;

        pos = map.DPRamAxisMap[ax].AD_LongType;

        //__PutCommand(ax, pos);
        //Need to update
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : CP_Line_Move
//Target System   : Stocker
//Brief           : Service Request No : 107
//                  Requested by CP_Move(int Axis_len, int profile_Type)
//                  CP motion service
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::CP_Line_Move()
    {
        SEP_LOGT("CP_Line_Move handler is called!\n");
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : Exist_Check
//Target System   : Stocker
//Brief           : Service Request No : 111
//                  requested by MMC_Bd_Num_Chk()
//                  MMC On board number count
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::Exist_Check()
    {
        SEP_LOGT("Exist_Check handler is called!\n");
    }

    void AmcEmulator::Set_Oht_Model_id()
    {
        auto& map = dpram_.map;
        int Model_OHT_ID;

        Model_OHT_ID = map.DPRamAxisMap[0].AD_IntType[0];

        // need to update ?�하 initialize
    }

    void AmcEmulator::Get_Oht_Model_id()
    {
        auto& map = dpram_.map;

        int Model_OHT_ID = 0;
        //need to update

        map.DPRamAxisMap[0].AD_IntType[0] = Model_OHT_ID;
    }

    void AmcEmulator::DPRAMTest()
    {
        //not used;
        //Use Virtual DPRAM
    }

    void AmcEmulator::Put_CleanVoltage()
    {
        //dcleanVoltage = IEEE2TMS(ReadChannel(AxisDPRAM[0], AD_FloatType1));
    }

    void AmcEmulator::ECatReenable()
    {
        SEP_LOGT("ECAT Reenable NOT Implemented!");
    }

    void AmcEmulator::SaveLongBlockToEeprom()
    {
        auto& Dpram = dpram_;
        DOWNLOAD_PARAM *pDL_Param;
        int nBlkID, uiOfs;
        unsigned int uiCrc32, uiTotLen, uiBlkLen;
        char *m_pcaBuff;

        pDL_Param = (DOWNLOAD_PARAM*)&Dpram.mem[576];

        nBlkID = pDL_Param->Block_ID;

        if (nBlkID == 0)
        {
            uiCrc32 = pDL_Param->uiCrc32;
            uiTotLen = pDL_Param->Total_Length;
            uiBlkLen = pDL_Param->Block_Length;


            //if (m_pcaBuff)
            //{
            //	free(m_pcaBuff);
            //}
            //m_pcaBuff = (char *) malloc(mmHdr.uiTotLen);
        }
        //need to update eeprom write

    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : AcceptString
//Target System   : Stocker
//Brief           : Service Request No : 152
//                  Requested by amc_exchange_string(char *pMsg, int SendByte, char pRcvMsg, int *RcvByte)
//                  exchange string between IPC and master
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::AcceptString()
    {
        SEP_LOGT("AcceptString handler is called!\n");
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : UploadParamToIPC
//Target System   : Stocker
//Brief           : Service Request No : 153
//                  Requested by UploadSysParam(char *pMsg, int *pnBytes)
//                  Upload boot parameters to IPC from Master
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::UploadParamToIPC()
    {
        SEP_LOGT("UploadParamToIPC handler is called!\n");
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : DnloadParamFromIPC
//Target System   : Stocker
//Brief           : Service Request No : 154
//                  Requested by DnloadSysParam(char *pMsg, int *pnBytes)
//                  Download boot parameters from IPC and save the data
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::DnloadParamFromIPC()
    {
        SEP_LOGT("DnloadParamFromIPC handler is called!\n");
    }

    void AmcEmulator::RunConfigUploadToIPC()
    {
        //need to update
    }

    void AmcEmulator::RunConfigSaveFromIPC()
    {
        //need to update
    }

    void AmcEmulator::SetEncoderOffset()
    {
        auto& map = dpram_.map;
        int ax, ofs;

        ax = map.Axis_Num;
        ofs = map.DPRamAxisMap[ax].AD_LongType;

        //need to update
        //if (pCBuf[ax]->g_on != 0)	// 2011.10.8, ?�보 off ?�태?�서???�정?��? ?�도�?
        //{
        //	__SetEncoderOffset(ax, ofs);
        //	_dsp_fset_encoder_offset(ax, ofs);
        //}
    }

    void AmcEmulator::GetEncoderOffset()
    {
        auto& map = dpram_.map;
        int ax, ofs;

        ax = map.Axis_Num;

        //ofs = __GetEncoderOffset(ax);
        ofs = 0;//need to update

        map.DPRamAxisMap[ax].AD_LongType1 = ofs;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : _dsp_param_flush
//Target System   : Stocker
//Brief           : Service Request No : 159
//                  Requested by amc_flush_sysparam_to_eeprom()
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::_dsp_param_flush()
    {
        SEP_LOGT("_dsp_param_flush handler is called!\n");
    }

    void AmcEmulator::WatchdogOperations()
    {
        auto& map = dpram_.map;
        int	cmd, nwhere, nrtn;

        // command 1:enable, 2:disable, 3:get status, 4:set status, 5:trigger
        cmd = map.DPRamAxisMap[0].AD_IntType[0];		// watchdog reason
        nwhere = map.DPRamAxisMap[0].AD_IntType[1];	// watchdog reason

        switch (cmd)
        {
            case 1:	// enable
                //_dsp_enable_wdt_reason(nwhere);
                break;

            case 2: // disable
                //_dsp_disable_wdt_reason(nwhere);
                break;

            case 3: // get status
                //nrtn = _dsp_get_wdt_status();
                nrtn = 0;//need to update

                map.DPRamAxisMap[0].AD_IntType[0] = nrtn;
                break;

            case 4: // set status
                //_dsp_set_wdt_status(nwhere);
                break;

            case 5: // triggering
                //_dsp_clr_wdt_reason(nwhere);
                break;
        }
    }

    void AmcEmulator::SetEncoderDirection()
    {
        auto& map = dpram_.map;
        int ax, cwccw;

        ax = map.Axis_Num;
        cwccw = map.DPRamAxisMap[ax].AD_IntType[0];

        //_dsp_set_encoder_direction(ax, cwccw);
        //Need to update
    }

    void AmcEmulator::GetEncoderDirection()
    {
        auto& map = dpram_.map;
        int ax, cwccw;

        ax = map.Axis_Num;

        //cwccw = _dsp_get_encoder_direction(ax);
        cwccw = 0;
        //Need to update

        map.DPRamAxisMap[ax].AD_IntType[0] = cwccw;
    }

    void AmcEmulator::SetSwPause()
    {
        auto& map = dpram_.map;
        int ax, Pause_Flag;

        ax = map.Axis_Num;

        Pause_Flag = map.DPRamAxisMap[ax].AD_IntType[0];

        //_dsp_set_sw_pause(ax, Pause_Flag);
        //Need to update
    }

    void AmcEmulator::GetSwPauseEvent()
    {
        auto& map = dpram_.map;
        int ax, Pause_Flag;

        ax = map.Axis_Num;

        //_dsp_get_sw_pause_event_st(ax, &Pause_Flag);
        Pause_Flag = 0;
        //Need to update
        map.DPRamAxisMap[ax].AD_IntType[0] = Pause_Flag;
    }

    void AmcEmulator::SetSwPauseCheckBit()
    {
        auto& map = dpram_.map;
        int	ax, bit, ActLvl;

        ax = map.Axis_Num;

        bit = map.DPRamAxisMap[ax].AD_IntType[0];
        ActLvl = map.DPRamAxisMap[ax].AD_IntType[1];

        //_dsp_set_sw_pause_checkbit(ax, bit, ActLvl);
        //Need to update
    }

    void AmcEmulator::GetSwPauseCheckBit()
    {
        auto& map = dpram_.map;
        int	ax, bit, ActLvl;

        ax = map.Axis_Num;

        //_dsp_get_sw_pause_checkbit(ax, &bit, &ActLvl);
        bit = 0;
        ActLvl = 0;
        //Need to update

        map.DPRamAxisMap[ax].AD_IntType[0] = bit;
        map.DPRamAxisMap[ax].AD_IntType[1] = ActLvl;
    }

    void AmcEmulator::GetMovexCmd()
    {
#if 0
        char * pMapStartBlock = MAP_DPRAM_ADDRESS + 0x10;
        MOVE_X_PARAM mvxParam;
        char *pPtr = (char *) &mvxParam;

        volatile double FollowsACC_tmp_GetMovexCmd;
        volatile double FollowsDCC_tmp_GetMovexCmd;
        volatile double FollowsVEL_tmp_GetMovexCmd;
        volatile int FollowsPOS_tmp_GetMovexCmd;

        check_pccmd = 0x80;
        amc_move_err =0;

        for (int i = 0; i < sizeof(MOVE_X_PARAM); i ++)
        {
            *pPtr++ = ReadChannel1(pMapStartBlock, i);
        }

        if (pCBuf[mvxParam.ax]->event_st)
        {
            amc_error = MMC_ON_MOTION;
            return;
        }

        if (pCBuf[mvxParam.ax]->g_on != 0)
        {
            return;
        }

        if (SyncMotion.Flag)
        {
            if(SyncMotion.Slave == mvxParam.ax)
            {
                return;
            }
            else if((SyncMotion.Master == mvxParam.ax) && (pCBuf[SyncMotion.Slave]->g_on != 0))
            {
                return;
            }
        }

        switch (mvxParam.cmd)
        {
            case CMD_MOVE_P:
                if (mvxParam.mode == 1)
                {
                    amc_move_err=_dsp_move_pt(mvxParam.ax, mvxParam.a[0]/SECTOMSEC, mvxParam.v[0]);
                }
                else
                {
                    amc_move_err=_dsp_move_p(mvxParam.ax, mvxParam.a[0], mvxParam.v[0]);
                }
                break;

            case CMD_MOVE_N:
                if (mvxParam.mode == 1)
                {
                    amc_move_err=_dsp_move_nt(mvxParam.ax, mvxParam.a[0]/SECTOMSEC, mvxParam.v[0]);
                }
                else
                {
                    amc_move_err=_dsp_move_n(mvxParam.ax, mvxParam.a[0], mvxParam.v[0]);
                }
                break;

            case CMD_MOVE_S:
                if (mvxParam.mode == 1)
                {
                    MoveS_Flag = 1;
                    Follow_Enable_Flag = false;
                    amc_move_err=_dsp_move_st(mvxParam.ax, (mvxParam.a[0]/SECTOMSEC));
                }
                else
                {
                    MoveS_Flag = 2;
                    Follow_Enable_Flag = false;
                    amc_move_err=_dsp_move_s(mvxParam.ax, mvxParam.a[0]);
                }
                break;

            case CMD_MOVE_DS:
                Marking_Process_Flag[mvxParam.ax] = false;

                if (mvxParam.mode == 1)
                {
                    if(mvxParam.ax == 0)
                    {
                        Check_Front_Rear_servo_On();

                        FollowsACC_tmp_GetMovexCmd = (mvxParam.a[0]/SECTOMSEC);
                        FollowsDCC_tmp_GetMovexCmd = (mvxParam.a[1]/SECTOMSEC);
                        FollowsVEL_tmp_GetMovexCmd = mvxParam.v[0];
                        FollowsPOS_tmp_GetMovexCmd = mvxParam.m[0];

                        MoveDS_Flag = 1;
                        amc_move_err=_dsp_move_dst(mvxParam.ax,	FollowsACC_tmp_GetMovexCmd, FollowsDCC_tmp_GetMovexCmd, FollowsVEL_tmp_GetMovexCmd, FollowsPOS_tmp_GetMovexCmd);

                        Follow_Enable_Flag = FALSE;

                        FollowsPOS = FollowsPOS_tmp_GetMovexCmd;
                        FollowsVEL = FollowsVEL_tmp_GetMovexCmd;
                        FollowsACC = SPEED_2_0_MPS2;
                        FollowsDCC = SPEED_3_0_MPS2;

                        FollowsDCC_CMD = FollowsDCC;

                        FollowsPOS_Prev = FollowsPOS;
                        FollowsVEL_Prev = FollowsVEL;
                        FollowsACC_Prev = FollowsACC;
                        FollowsDCC_Prev = FollowsDCC;

                        Follow_Enable_Flag = TRUE;
                    }
                    else
                    {
                        MoveDS_Flag = 2;
                        amc_move_err=_dsp_move_dst(mvxParam.ax,	(mvxParam.a[0]/SECTOMSEC), (mvxParam.a[1]/SECTOMSEC), mvxParam.v[0], mvxParam.m[0]);
                    }
                }
                else
                {
                    if(mvxParam.ax == 0)
                    {
                        Check_Front_Rear_servo_On();

                        FollowsACC_tmp_GetMovexCmd = mvxParam.a[0];
                        FollowsDCC_tmp_GetMovexCmd = mvxParam.a[1];
                        FollowsVEL_tmp_GetMovexCmd = mvxParam.v[0];
                        FollowsPOS_tmp_GetMovexCmd = mvxParam.m[0];

                        MoveDS_Flag = 3;
                        _dsp_move_ds(mvxParam.ax, FollowsACC_tmp_GetMovexCmd, FollowsDCC_tmp_GetMovexCmd, FollowsVEL_tmp_GetMovexCmd, FollowsPOS_tmp_GetMovexCmd);

                        Follow_Enable_Flag = FALSE;

                        FollowsPOS = FollowsPOS_tmp_GetMovexCmd;
                        FollowsVEL = FollowsVEL_tmp_GetMovexCmd;
                        FollowsACC = FollowsACC_tmp_GetMovexCmd;
                        FollowsDCC = FollowsDCC_tmp_GetMovexCmd;

                        FollowsDCC_CMD = FollowsDCC;

                        FollowsPOS_Prev = FollowsPOS;
                        FollowsVEL_Prev = FollowsVEL;
                        FollowsACC_Prev = FollowsACC;
                        FollowsDCC_Prev = FollowsDCC;

                        Follow_Enable_Flag = TRUE;
                    }
                    else
                    {
                        MoveDS_Flag = 4;
                        amc_move_err=_dsp_move_ds(mvxParam.ax,	mvxParam.a[0], mvxParam.a[1], mvxParam.v[0], mvxParam.m[0]);
                    }
                }
                break;
        }
#else
        MOVE_X_PARAM *pMovexParam;
        auto& Dpram = dpram_;
        int amc_move_err;// need to update at motion state machine

        volatile double FollowsACC_tmp_GetMovexCmd;
        volatile double FollowsDCC_tmp_GetMovexCmd;
        volatile double FollowsVEL_tmp_GetMovexCmd;
        volatile int FollowsPOS_tmp_GetMovexCmd;

        amc_move_err = 0;
        pMovexParam = (MOVE_X_PARAM*)&Dpram.mem[592];

        FollowsACC_tmp_GetMovexCmd = pMovexParam->a[0];	//ACC
        FollowsDCC_tmp_GetMovexCmd = pMovexParam->a[1];	//DCC
        FollowsVEL_tmp_GetMovexCmd = pMovexParam->v[0];	//Vel
        FollowsPOS_tmp_GetMovexCmd = pMovexParam->m[0];	//Pos

        //amc_move_err=_dsp_move_ds(mvxParam.ax, mvxParam.a[0], mvxParam.a[1], mvxParam.v[0], mvxParam.m[0]);
        //need to update
#endif
    }

    void AmcEmulator::_dsp_clear_amc_error()
    {
        //need to clear error
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
//Function Name   : dsp_Get_Error_Status
//Target System   : Stocker
//Brief           : Service Request No : 170
//                  get error status
//Author          : Seungwan An
//Revision        : R01(2017.08.07) - Initial version
//                : R02
////////////////////////////////////////////////////////////////////////////////////////////////////
    void AmcEmulator::dsp_Get_Error_Status()
    {
        printf("dsp_Get_Error_Status handler is called!\n");
    }

    void AmcEmulator::FSExchange()
    {
        //need to update
#if 0
        int i;
        char * pMapStartBlock = MAP_DPRAM_ADDRESS;
        char szStr[200];
        HANDLE hFile;
        UINT uiSize;
        SYSTEM_DATA stSystemData;

        sFileCmd.uiCrc32 = ReadChannel(pMapStartBlock, 0);
        sFileCmd.uiTotLen = ReadChannel(pMapStartBlock, 4);
        sFileCmd.usBlkID = ReadChannel2(pMapStartBlock, 8);
        sFileCmd.usCmd16 = ReadChannel2(pMapStartBlock, 10);
        sFileCmd.ucIndex = ReadChannel1(pMapStartBlock, 12);
        sFileCmd.ucData = ReadChannel1(pMapStartBlock, 13);
        sFileCmd.usPackLen = ReadChannel2(pMapStartBlock, 14);

        memset(&sFileCmdAck, 0, sizeof(sFileCmdAck));
        if (sFileCmd.ucData == 0)
        {
            // 명령??경우�??�일명을 가?�온??
            if (sFileCmd.usPackLen > 16)
            {
                for (i = 0; i < sFileCmd.usPackLen - 16; i ++)
                    sFileCmd.uszFileName[i] = ReadChannel1(pMapStartBlock, i + 16);
                sFileCmd.uszFileName[i] = 0;
            }
            UINT uiCrc = _crc32_get_crc((char*) &sFileCmd.uiTotLen, sFileCmd.usPackLen - 4);
            if (uiCrc != sFileCmd.uiCrc32)
            {
                sFileCmdAck.uiValidBytes = 0;
                PushAck(&sFileCmdAck);
                return;
            }

            // ?�속?�작?�야?�는 명령�??�번???�나??명령??구분?�다.
            switch (sFileCmd.usCmd16)
            {
            case FSCMD_FILENUM://0x0100:	// ?�일??�?�� 리턴?�기
                sFileCmdAck.uiTotLen = GetNumberOfFile();
                PushAck(&sFileCmdAck);
                return;
            case FSCMD_FILENAME:	// ?�일?�름 가?�오�?
                if(GetFileName(sFileCmd.ucIndex, szStr+100, 50)==TRUE)
                    strcpy((char *) sFileCmdAck.uszCmd, szStr+100);
                else
                    strcpy((char *) sFileCmdAck.uszCmd, "");
                PushAck(&sFileCmdAck);
                return;
            case FSCMD_FILESIZE:	// ?�일?�기 가?�오�?
                if (sFileCmd.usPackLen <= 16)
                    // ?�일명이 ?�는경우
                    GetFileName(sFileCmd.ucIndex, szStr, 100);
                else
                    strcpy(szStr, (char *) sFileCmd.uszFileName);
                {
                    hFile = FileOpen(szStr, FILE_MODE_READ);
    //				sprintf(sFileCmdAck.uszCmd, "[0102>%d]", FileGetSize(hFile));
                    sFileCmdAck.uiTotLen = FileGetSize(hFile);
                    FileClose(hFile);
                }
                PushAck(&sFileCmdAck);
                return;

            case FSCMD_CPUTYPE_READ://0x0000:	// CPU type ?�기
                sFileCmdAck.uiTotLen = GetSystemData()->m_nCPUType;
    //			sprintf(sFileCmdAck.uszCmd, "[0000>%d]", GetSystemData()->m_nCPUType);
                PushAck(&sFileCmdAck);
                return;

            case FSCMD_CPUTYPE_WRITE://0x0001:	// CPU type ?�기
                memcpy(&stSystemData, GetSystemData(), sizeof(SYSTEM_DATA));
                stSystemData.m_nCPUType = sFileCmd.ucIndex;
                FlashWriteEnable(1);
                WriteSystemData(&stSystemData);
                FlashWriteEnable(0);
                sFileCmdAck.uiValidBytes = sFileCmd.ucIndex;
                PushAck(&sFileCmdAck);
                return;
            case FSCMD_FS_FORMAT://0x0200:	// ?�일?�스???�멧
                _dsp_fs_format();

                sprintf(szStr, "File system format!\r\n");
                SERIAL_WriteBytes(szStr, strlen(szStr));

                sFileCmdAck.uiValidBytes = 1;
                PushAck(&sFileCmdAck);
                return;
            case FSCMD_DEL_FILE://0x0202:	// ?�일??��
                _dsp_fs_deletefile((char *) sFileCmd.uszFileName);
                sFileCmdAck.uiValidBytes = 1;
                PushAck(&sFileCmdAck);
                return;

            case FSCMD_SET_BOOT://0x0203:	// 부?�파???�정
                _dsp_fs_setbootfile((char*) sFileCmd.uszFileName);

                sprintf(szStr, "Set Boot: '%s'\r\n", sFileCmd.uszFileName);
                SERIAL_WriteBytes(szStr, strlen(szStr));

                sFileCmdAck.uiValidBytes = 1;
                PushAck(&sFileCmdAck);
                return;

            case FSCMD_GET_SYSTEM://0x0300:	// ?�스?�정�??�기
                memcpy(&stSystemData, GetSystemData(), sizeof(SYSTEM_DATA));
                sFileCmdAck.uiValidBytes = stSystemData.m_nCPUType;
                sFileCmdAck.SetFileName( stSystemData.m_sRunFileName);
                PushAck(&sFileCmdAck);
                return;

            case FSCMD_FILE_UPLOAD://0x0110:	// ?�일?�용 upload
                strcpy(m_sfd.m_sName, (char*) sFileCmd.uszFileName);
                hFile = FileOpen(m_sfd.m_sName, FILE_MODE_READ);

                if (hFile == NULL) uiSize = 0;
                else {
                    uiSize = FileGetSize(hFile);
                    if (m_sfd.m_pBuffer) free(m_sfd.m_pBuffer);
                    m_sfd.m_nSize = uiSize;
                    m_sfd.m_pBuffer = (unsigned char*)malloc(uiSize);
                    FileRead(hFile, m_sfd.m_pBuffer, uiSize);
                    FileClose(hFile);

                    printf("File upload: @ 0x%p, size=%d\n", m_sfd.m_pBuffer, m_sfd.m_nSize);
                }

                // ?�기�??�려 보내�? ?�음부?�는 ?�기?�?블럭ID�??�려보낸??
                sFileCmdAck.uiTotLen = uiSize;
                PushAck(&sFileCmdAck);
                return;

            case FSCMD_FILE_DNLOAD://0x0111:	// ?�일 ?�?�하�?
                for (i = 0; i < sFileCmd.usPackLen - 16; i ++)
                    sFileCmd.uszFileName[i] = ReadChannel1(pMapStartBlock, 16 + i);
                sFileCmd.uszFileName[i] = 0;

                strcpy((char *) m_sfd.m_sName, (char *) sFileCmd.uszFileName);
                m_sfd.m_nSize = sFileCmd.uiTotLen;

                if (m_sfd.m_pBuffer) free(m_sfd.m_pBuffer);
                m_sfd.m_pBuffer = (UCHAR *) malloc(m_sfd.m_nSize);

                sprintf(szStr, "File download: @ 0x%p, name=%s, size=%d\r\n", m_sfd.m_pBuffer, m_sfd.m_sName, m_sfd.m_nSize);
                SERIAL_WriteBytes(szStr, strlen(szStr));

                sFileCmdAck.uiValidBytes = 1;
                PushAck(&sFileCmdAck);
                return;

            case FSCMD_BL_DNLOAD://0x0112:	// 부?�로???�려보내�?
                for (i = 0; i < sizeof(sFileCmd.uszFileName); i ++)
                    sFileCmd.uszFileName[i] = ReadChannel1(pMapStartBlock, 16 + i);
                sFileCmd.uszFileName[i] = 0;

                strcpy((char *) m_sfd.m_sName, (char *) sFileCmd.uszFileName);
                m_sfd.m_nSize = sFileCmd.uiTotLen;

                if (m_sfd.m_pBuffer) free(m_sfd.m_pBuffer);
                m_sfd.m_pBuffer = (UCHAR *) malloc(m_sfd.m_nSize);
                printf("BL-File download: @ 0x%p, size=%d\n", m_sfd.m_pBuffer, m_sfd.m_nSize);

                sFileCmdAck.uiValidBytes = 1;
                PushAck(&sFileCmdAck);
                return;

            case FSCMD_DUMP_SYSDATA:
                DumpSysData();
                sFileCmdAck.uiValidBytes = 0;
                PushAck(&sFileCmdAck);
                return;

            case FSCMD_DUMP_FILETBL:
                DumpFileTbl();
                sFileCmdAck.uiValidBytes = 0;
                PushAck(&sFileCmdAck);
                return;

            case FSCMD_DUMP_FAT:
                DumpFat();
                sFileCmdAck.uiValidBytes = 0;
                PushAck(&sFileCmdAck);
                return;

            default:
                sFileCmdAck.uiValidBytes = 0;
                PushAck(&sFileCmdAck);
                return;
            }
        }

        // for file upload
        if (sFileCmd.ucData == 1 && sFileCmd.uiTotLen != 0)
        {
            //  IPC�??�일???�용???�린??
            UINT uiOfs = sFileCmd.usBlkID * 256;
            int nLen = min(sFileCmd.uiTotLen - uiOfs, 256);
            if ((sFileCmd.usBlkID % 20) == 0)
            {
                sprintf(szStr, "(0x%p) : Blk=%d, Read=%d, Ofs=%d\r\n", &m_sfd.m_pBuffer[uiOfs], sFileCmd.usBlkID, nLen, uiOfs);
                SERIAL_WriteBytes(szStr, strlen(szStr));
            }

            sFileCmdAck.usPackLen = 16;	// ?�더??길이
            sFileCmdAck.uiTotLen = nLen;	// ?�이?�의 바이?�수
            sFileCmdAck.uiCrc32 = _crc32_get_crc((char *) &m_sfd.m_pBuffer[uiOfs], nLen);
            sFileCmdAck.uiValidBytes = 1;

            PushAck(&sFileCmdAck);
            for (int i = 0; i < nLen; i ++)
                WriteChannel1(pMapStartBlock + EDUMP_OFS_DATABLOCK, i, m_sfd.m_pBuffer[uiOfs + i]);

        }
        // for file download
        else if (sFileCmd.ucData == 2 && sFileCmd.uiTotLen != 0)
        {
            // IPC?�서 ?�일???�용???�려보낸??
            // 모든 ?�킷마다 ?�답??보낸??
            UINT uiOfs = sFileCmd.usBlkID * 256;
            int nLen = min(sFileCmd.uiTotLen - uiOfs, 256);
            UINT uicrc = _crc32_get_crc(pMapStartBlock + EDUMP_OFS_DATABLOCK, nLen);
            if (sFileCmd.uiCrc32 != uicrc)
            {
                sFileCmdAck.uiValidBytes = 0;
                PushAck(&sFileCmdAck);

                sprintf(szStr, "Fail: Tot=%d, Ofs=%d, len=%d\r\n", sFileCmd.uiTotLen, uiOfs, nLen);
                SERIAL_WriteBytes(szStr, strlen(szStr));

                free(m_sfd.m_pBuffer);
                m_sfd.m_pBuffer = NULL;
            } else
            {
                // crc 가 ?�치??경우
                if ((sFileCmd.usBlkID % 20) == 0)
                {
    //				sprintf(szStr, "Sucd(0x%p): Tot=%d, blkid=%d, Ofs=%d, len=%d\r\n", &m_sfd.m_pBuffer[uiOfs], sFileCmd.uiTotLen, sFileCmd.usBlkID, uiOfs, nLen);
    //				SERIAL_WriteBytes(szStr, strlen(szStr));
                }

                if (m_sfd.m_pBuffer)
                {
                    memcpy(&m_sfd.m_pBuffer[uiOfs], pMapStartBlock + EDUMP_OFS_DATABLOCK, nLen);
                    sFileCmdAck.uiValidBytes = 1;
                } else {
                    sFileCmdAck.uiValidBytes = 0;
                }


                if (uiOfs + nLen >= sFileCmd.uiTotLen)
                {

                    // ?�공?��?�?그에 맞는 ?�작???�행?�다.
                    // �?부?�로???�기 ?�는 ?�일 ?�기??..
                    if (sFileCmd.usCmd16 == FSCMD_FILE_DNLOAD)
                    {
                        //11.11.18 syk//  writefile ?�행 ??리턴�?ack , usFiller ?�용?�하????��???�용??
                        sFileCmdAck.usFiller = WriteFile(&m_sfd);
                        if(sFileCmdAck.usFiller ==1)  	sprintf(szStr, "FileWrite: End: Tot=%d, Ofs=%d, len=%d\r\n", sFileCmd.uiTotLen, uiOfs, nLen);
                        if(sFileCmdAck.usFiller ==2)	sprintf(szStr, "ERROR(Appl File Size Over): End: Tot=%d\r\n", sFileCmd.uiTotLen);
                        if(sFileCmdAck.usFiller ==3)	sprintf(szStr, "ERROR(Lack of Space): End: Tot=%d, Ofs=%d, len=%d\r\n", sFileCmd.uiTotLen, uiOfs, nLen);
                    }
                    else if (sFileCmd.usCmd16 == FSCMD_BL_DNLOAD)
                    {
                        WriteBootLoader(&m_sfd);
                        //11.11.18 syk//  writefile ?�행 ??리턴�?ack , usFiller ?�용?�하????��???�용??
                        sFileCmdAck.usFiller =1;
                        sprintf(szStr, "Boot FileWrite: End: Tot=%d, Ofs=%d, len=%d\r\n", sFileCmd.uiTotLen, uiOfs, nLen);
                    }
                    SERIAL_WriteBytes(szStr, strlen(szStr));

                    free(m_sfd.m_pBuffer);
                    m_sfd.m_pBuffer = NULL;
                }
                PushAck(&sFileCmdAck);
            }
        }
#endif
    }

    void AmcEmulator::AMCFilteringSet()
    {
        auto& map = dpram_.map;
        int	ax, nWhere, nType, nFreq;

        ax = map.Axis_Num;

        nWhere = map.DPRamAxisMap[ax].AD_IntType[0];  // 0:position, 1:velocity(torque).
        nType = map.DPRamAxisMap[ax].AD_IntType[1];	  // 0:lpf, 3:notch
        nFreq = map.DPRamAxisMap[ax].AD_IntType[2];

        //Need to update - SW An
#if 0
        if (nWhere == 0)	// position
        {
            if (nType == 0) _dsp_set_position_lpf(ax, nFreq);
            else if (nType == 3) _dsp_set_position_notch_filter(ax, nFreq);
        } else if (nWhere == 1)
        {
            if (nType == 0) _dsp_set_velocity_lpf(ax, nFreq);
            else if (nType == 3) _dsp_set_velocity_notch_filter(ax, nFreq);
        }
#endif
    }

    void AmcEmulator::AMCFilteringGet()
    {
        auto& map = dpram_.map;
        int	ax, nWhere, nType, nFreq;

        ax = map.Axis_Num;

        nWhere = map.DPRamAxisMap[ax].AD_IntType[0];	// 0:position, 1:velocity(torque)
        nType = map.DPRamAxisMap[ax].AD_IntType[1];	    // 0:lpf, 1:hpf, 2:bpf, 3:notch

        //Need to update - SW An
#if 0
        if (nWhere == 0)	// position
        {
            if (nType == 0) _dsp_get_position_lpf(ax, &nFreq);
            else if (nType == 3) _dsp_get_position_notch_filter(ax, &nFreq);
        } else if (nWhere == 1)
        {
            if (nType == 0) _dsp_get_velocity_lpf(ax, &nFreq);
            else if (nType == 3) _dsp_get_velocity_notch_filter(ax, &nFreq);
        }
        nType = map.DPRamAxisMap[ax].AD_IntType[2] = nFreq;
#endif
    }

    void AmcEmulator::AMCStatusGet()
    {
#if 0
        char *ptr = (char *) &sDbgMsgBuff;
        char *pd = (char *) (MAP_DPRAM_ADDRESS + MAP_OFS_DATA256);

        memcpy(pd, ptr, 256);
#endif
    }

    void AmcEmulator::Torque_Limit(void)
    {
        auto& map = dpram_.map;
        int	ax, on1off0;

        ax = map.Axis_Num;

        on1off0 = map.DPRamAxisMap[ax].AD_IntType[0];

        //__Torque_Limit(ax, on1off0);
        //Need to update
    }

    void AmcEmulator::set_offset()
    {
        // set_offset();
    }

    void AmcEmulator::restart_program()
    {
        //restart_program
    }

    void AmcEmulator::TraceUpdate_1()
    {
        //not used
    }

    void AmcEmulator::TraceUpdate()
    {
        //not used
    }

    void AmcEmulator::GetAMCData70()
    {
        //Need to update for OHT version 7.0
    }

    void AmcEmulator::ReloadEncoderPos()
    {
        auto& map = dpram_.map;
        int	ax;

        ax = map.Axis_Num;

        //_ReadServopackPosition(ax, 11, 0);
        //need to update
    }

    void AmcEmulator::Put_Vel_Curve()
    {
        auto& map = dpram_.map;
        int	ax, li;
        char cst;

        ax = map.Axis_Num;

        //__GetVelCurve(ax, &li, &cst);
        li = 0;//need to update
        cst = 0;//need to update
        //Need to update

        map.DPRamAxisMap[0].AD_LongType = li;
        map.DPRamAxisMap[0].AD_CharType[0] =  cst;
    }

    void AmcEmulator::Get_Vel_Curve()
    {
        auto& map = dpram_.map;

        int ax = map.Axis_Num;
        int li = map.DPRamAxisMap[0].AD_LongType;
        char cst = map.DPRamAxisMap[0].AD_CharType[0];

        //__SetVelCurve(ax, li, cst);
        //need to update
    }

    void AmcEmulator::Put_Actvel_Margin()
    {
        auto& map = dpram_.map;
        int	ax, li, time;
        char cst;

        ax = map.Axis_Num;

        //__GetActvelMargin(ax, &li, &cst, &time);
        li = 0;
        time = 0;
        cst = 0;
        //Need to update

        map.DPRamAxisMap[0].AD_CharType[0] = cst;
        map.DPRamAxisMap[0].AD_IntType[0] = time;
        map.DPRamAxisMap[0].AD_LongType = li;
    }

    void AmcEmulator::Get_Actvel_Margin()
    {
        auto& map = dpram_.map;

        int ax = map.Axis_Num;
        int li = map.DPRamAxisMap[0].AD_LongType;
        int time = map.DPRamAxisMap[0].AD_IntType[0];
        char cst = map.DPRamAxisMap[0].AD_CharType[0];

        //__SetActvelMargin(ax, li, cst, time);
        //need to update
    }

    void AmcEmulator::AMCStatusGet2()
    {
        //Not used ..Log?
    }

    void AmcEmulator::SetIoOutput()
    {
        auto& Dpram = dpram_;
        COMMAND_DRRAM *pCommDPRAM;
        int	ld1, ld2, ld3, ld4, ld5, ld6, ld7, ld8;

        pCommDPRAM = (COMMAND_DRRAM*)&Dpram.mem[384];

        ld1 = pCommDPRAM->Io_outValue;
        ld2 = pCommDPRAM->Io_outValue2;
        ld3 = pCommDPRAM->Io_outValue3;
        ld4 = pCommDPRAM->Io_outValue4;
        ld5 = pCommDPRAM->Io_outValue5;
        ld6 = pCommDPRAM->Io_outValue6;
        ld7 = pCommDPRAM->Io_outValue7;
        ld8 = pCommDPRAM->Io_outValue8;

        //__SetIoOutData_256(ld1, ld2, ld3, ld4, ld5, ld6, ld7, ld8);
        //need to update
    }

    void AmcEmulator::GetIoOutput()
    {
        auto& Dpram = dpram_;
        COMMAND_DRRAM *pCommDPRAM;
        int dat[8];

        pCommDPRAM = (COMMAND_DRRAM*)&Dpram.mem[384];

        //amc_error = _dsp_get_io_output(dat);
        //need to update

        pCommDPRAM->Io_outValue = dat[0];
        pCommDPRAM->Io_outValue2 = dat[1];
        pCommDPRAM->Io_outValue3 = dat[2];
        pCommDPRAM->Io_outValue4 = dat[3];
        pCommDPRAM->Io_outValue5 = dat[4];
        pCommDPRAM->Io_outValue6 = dat[5];
        pCommDPRAM->Io_outValue7 = dat[6];
        pCommDPRAM->Io_outValue8 = dat[7];
    }

    void AmcEmulator::SetIOCount()
    {
        auto& map = dpram_.map;
        int	ld1, ld2;

        ld1 = map._Amc._Cmd.Cmd_Dpram.Io_outValue7;
        ld2 = map._Amc._Cmd.Cmd_Dpram.Io_outValue8;

        //__SetIoCount(ld1, ld2);
        //need to update
    }

    void AmcEmulator::GetIOCount()
    {
        auto& map = dpram_.map;
        int dat1,dat2;

        //amc_error = _dsp_get_io_count(&dat1, &dat2);
        dat1 = 0;
        dat2 = 0;

        map._Amc._Cmd.Cmd_Dpram.Io_outValue7 = dat1 ;
        map._Amc._Cmd.Cmd_Dpram.Io_outValue8 = dat2 ;
    }

    void AmcEmulator::ReloadServopackAll()
    {
        //_ReloadServopackTiming(11);
        //need to update
    }

    void AmcEmulator::GetSystempStatus()
    {
        auto& map = dpram_.map;
        int	ax;
        char status;

        ax = map.Axis_Num;

        //_dsp_system_status(&status);
        status = 0;//need to update

        map.DPRamAxisMap[ax].AD_CharType[1] = status;
    }

    void AmcEmulator::SetSystemMoniEnable()
    {
        auto& map = dpram_.map;
        int	ax;
        char axno;

        ax = map.Axis_Num;
        axno = map.DPRamAxisMap[ax].AD_CharType[1];

        //__system_monitering_enable(axno, TRUE);
        //need to update
    }

    void AmcEmulator::SetSystemMoniDisable()
    {
        auto& map = dpram_.map;
        int	ax;
        char axno;

        ax = map.Axis_Num;
        axno = map.DPRamAxisMap[ax].AD_CharType[1];

        //__system_monitering_enable(axno, FALSE);
        //need to update
    }

    void AmcEmulator::GetSystemMoniEnable()
    {
        auto& map = dpram_.map;
        int ax = 1;
        char moni_status;

        //_dsp_get_system_moni_enable(&moni_status);
        moni_status = 0;//need to update

        map.DPRamAxisMap[ax].AD_CharType[1] = (char)moni_status;
    }

    void AmcEmulator::GetSystemMoniValue()
    {
        auto& map = dpram_.map;
        int	ax, value, fpga_data, dac_data;
        char axno;

        ax = map.Axis_Num;
        axno = map.DPRamAxisMap[ax].AD_CharType[1];

        //__g_system_monitering_value(axno, &value, &fpga_data, &dac_data);
        value = 0;//need to update
        fpga_data = 0;//need to update
        dac_data = 0;//need to update

        map.DPRamAxisMap[ax].AD_LongType = value;
        map.DPRamAxisMap[ax].AD_IntType[3] = (short)fpga_data;
        map.DPRamAxisMap[ax].AD_IntType[4] = (short)dac_data;
    }

    void AmcEmulator::SetMoniteringThresholdPercent()
    {
        auto& map = dpram_.map;
        int	ax;
        char axno,percent;

        ax = map.Axis_Num;

        axno = map.DPRamAxisMap[ax].AD_CharType[1];
        percent	= map.DPRamAxisMap[ax].AD_CharType[0];

        //__s_monitering_threshold_percent(axno, percent);
        //need to update
    }

    void AmcEmulator::GetMoniteringThresholdPercent()
    {
        auto& map = dpram_.map;
        int	ax;
        char axno,percent;

        ax = map.Axis_Num;
        axno = map.DPRamAxisMap[ax].AD_CharType[1];
        percent = '\0';

        //__g_monitering_threshold_percent(axno, &percent);
        //need to update

        map.DPRamAxisMap[ax].AD_CharType[1] = percent;
    }

    void AmcEmulator::DoAutopatch()
    {
        SEP_LOGT("Autopatch called!\n");
    }

    void AmcEmulator::GetVersion()
    {
        SEP_LOGT("GetVersion called!\n");
    }

    void AmcEmulator::GetEcmVersion()
    {
        SEP_LOGT("GetEcmVersion called!\n");
    }
    void AmcEmulator::StopAll()
    {
        SEP_LOGT("GetEcmVersion called!\n");
    }
   
}
