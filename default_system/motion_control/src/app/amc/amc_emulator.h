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

#ifndef SEPMASTER_AMC_EMULATOR_H
#define SEPMASTER_AMC_EMULATOR_H

#include <string>
#include <yaml-cpp/yaml.h>
#include <map>
#include "amc_dpram.h"
#include "axes_module.h"
#include "io_board.h"
#include "ecmaster_rpc_client.h"

namespace amc
{
    using sephi::MultiAxes;
    using sephi::EcServoMotor;
    using sephi::EcStockerIo;
    using sephi::EcOHTFrontIo;
    using sephi::EcOHTRearIo;
    using std::unique_ptr;
    using std::make_unique;
    using std::array;
    using std::pair;
    using std::make_pair;
    using std::map;

    class AmcEmulator : public IAmcDispacher {
        typedef void (AmcEmulator::*serviceHandler)(void);
    public:
        AmcEmulator();
        virtual ~AmcEmulator();

        bool start();
        void stop();
        virtual void finalize() { }
        bool dispatch(bool initial_sync = false);
        bool stop_all(void);
        virtual bool loadConfig(const char* config_file_path);

    protected:

        virtual void Reserved_Function(void);

        virtual void PTP_Move(void);                //1 - Stocker
        void CP_Move(void);                         //2

        void PUT_Gain(void);                        //3
        void GET_Gain(void);                        //4
        void PUT_VGain(void);                       //5
        void GET_VGain(void);                       //6

        void GET_Position(void);                    //7
        void PUT_Curr_Pos(void);                    //8
        void PUT_Position(void);                    //9

        void PUT_Error_Pls(void);                   //10

        void GET_In_Position(void);                 //11
        void PUT_In_Position(void);                 //12

        void GET_Error_Limit(void);                 //13
        void PUT_Error_Limit(void);                 //14

        virtual void PUT_Empty_Frame(void);         //15 - stocker
        void GET_Boot_Memory(void);                 //16
        virtual void VEL_Move(void);                //17 - Stocker
        void GET_IO_Port(void);                     //18
        void SET_IO_Bit(void);                      //19
        void RESET_IO_Bit(void);                    //20
        void SET_Delay(void);                       //21
        void PUT_Stepper(void);                     //22
        void GET_Stepper(void);                     //23

        virtual void Servo_On(void);                //24 - Stocker & oht
        virtual void Servo_Off(void);               //25 - Stocker & oht
        virtual void Alarm_Reset(void);             //26 - Stocker & oht
        virtual void Alarm_Set(void);               //27 - Stocker & oht

        virtual void Clear_Status(void);            //28 - Stocker & oht
        virtual void Get_Stop_Event(void);          //29 - Stokcer
        virtual void Get_Stop_Rate(void);           //30 - Stocker
        void Put_Stop_Rate(void);                   //31
        void Get_E_Stop_Event(void);                //32
        void Get_E_Stop_Rate(void);                 //33
        void Put_E_Stop_Rate(void);                 //34

        virtual void Get_Pos_Sn_Event(void);        //35 - Stocker
        virtual void Put_Pos_Sn_Event(void);        //36 - Stocker
        virtual void Get_Neg_Sn_Event(void);        //37 - Stocker
        virtual void Put_Neg_Sn_Event(void);        //38 - Stocker

        virtual void Get_Pos_Level(void);           //39 - Stocker
        virtual void Put_Pos_Level(void);           //40 - Stocker
        virtual void Get_Neg_Level(void);           //41 - Stocker
        virtual void Put_Neg_Level(void);           //42 - Stocker

        virtual void Get_Home_Sn_Event(void);       //43 - Stocker
        virtual void Put_Home_Sn_Event(void);       //44 - Stocker
        virtual void Get_Home_Level(void);          //45 - Stocker
        virtual void Put_Home_Level(void);          //46 - Stocker
        void Get_Index_Req(void);                   //47
        void Put_Index_Req(void);                   //48
        void Get_Int_Stop(void);                    //49
        void Get_Int_E_Stop(void);                  //50

        virtual void Put_Axis_Stat(void);           //51 - Stocker
        void Put_Axis_Source(void);                 //52

        virtual void Get_Amp_Fault(void);           //53 - Stocker
        virtual void Put_Amp_Fault(void);           //54 - Stocker
        void Get_Amp_Fault_Level(void);             //55
        void Put_Amp_Fault_Level(void);             //56
        void Get_Amp_Reset_Level(void);             //57
        void Put_Amp_Reset_Level(void);             //58

        void Get_Dac_Offset(void);                  //59
        void Put_Dac_Offset(void);                  //60

        void Get_Feedback_Device(void);             //61
        void Put_Feedback_Device(void);             //62
        void Get_Voltage_Device(void);              //63
        void Put_Voltage_Device(void);              //64
        void Get_Axis_Boot_Memory(void);            //65
        virtual void Get_Frames_Clear(void);        //66 - Stocker & oht

        void Get_Closed_Loop(void);                 //67
        void Put_Closed_Loop(void);                 //68
        void Get_Amp_On_Level(void);                //69
        void Put_Amp_On_Level(void);                //70

        void Get_IO_Int_Enable(void);               //71
        void Put_Buf_Position(void);                //72
        virtual void Clear_Stop(void);              //73 - Stocker
        void Main_Delay(void);                      //74
        void Get_IO_Trigger(void);                  //75

        virtual void Get_SW_Upper_Limit(void);      //76 - Stocker & oht
        virtual void Put_SW_Upper_Limit(void);      //77 - Stocker & oht
        virtual void Get_SW_Lower_Limit(void);      //78 - Stocker & oht
        virtual void Put_SW_Lower_Limit(void);      //79 - Stocker & oht

        virtual void Put_IO_Out_Data(void);         //80 - Stocker

        void Get_Control_Cfg(void);                 //81
        void Put_Control_Cfg(void);                 //82
        void Get_PosImode(void);                    //83
        void Put_PosImode(void);                    //84
        void Get_VelImode(void);                    //85
        void Put_VelImode(void);                    //86
        void Get_GearRatio(void);                   //87
        void Put_GearRatio(void);                   //88

        virtual void Get_SyncMap(void);             //89 - oht
        virtual void Get_SyncOn(void);              //90 - oht
        virtual void Get_SyncOff(void);             //91 - oht
        virtual void Get_SyncGain(void);            //92 - oht
        void Put_SyncGain(void);                    //93
        void Get_Compensation_Pos(void);            //94

        void Get_Pulse_Mode(void);                  //95
        void Put_Pulse_Mode(void);                  //96
        virtual void Put_Encoder(void);             //97 - Stocker
        void Put_SyncEncoder(void);                 //98
        void Get_Pulse_Ratio(void);                 //99
        void Put_Pulse_Ratio(void);                 //100
        void Get_Interpolation(void);               //101
        void Put_Empty_Interpolation(void);         //102
        void Get_Abs_Encoder(void);                 //103
        void Get_Command(void);                     //104

        //Reserved_Function-------------------------//105
        //Reserved_Function-------------------------//106

        virtual void CP_Line_Move(void);            //107 - Stocker

        //Reserved_Function-------------------------//108
        //Reserved_Function-------------------------//109
        //Reserved_Function-------------------------//110

        virtual void Exist_Check(void);             //111 - Stocker & oht
        virtual void Set_Oht_Model_id(void);        //112 - oht
        virtual void Get_Oht_Model_id(void);        //113 - oht

        //Reserved_Function-------------------------//114
        //Reserved_Function-------------------------//115
        //Reserved_Function-------------------------//116
        //Reserved_Function-------------------------//117
        //Reserved_Function-------------------------//118
        //Reserved_Function-------------------------//119
        //Reserved_Function-------------------------//120
        //Reserved_Function-------------------------//121
        //Reserved_Function-------------------------//122
        //Reserved_Function-------------------------//123
        //Reserved_Function-------------------------//124
        //Reserved_Function-------------------------//125
        //Reserved_Function-------------------------//126
        //Reserved_Function-------------------------//127
        //Reserved_Function-------------------------//128
        //Reserved_Function-------------------------//129
        virtual void statusUpdate(void);            //130 - Stocker
        //Reserved_Function-------------------------//131
        //Reserved_Function-------------------------//132
        //Reserved_Function-------------------------//133
        //Reserved_Function-------------------------//134
        //Reserved_Function-------------------------//135
        //Reserved_Function-------------------------//136
        //Reserved_Function-------------------------//137
        //Reserved_Function-------------------------//138
        //Reserved_Function-------------------------//139

        void DPRAMTest(void);                       //140
        void Put_CleanVoltage(void);                //141

        //Reserved_Function-------------------------//142
        //Reserved_Function-------------------------//143
        //Reserved_Function-------------------------//144
        //Reserved_Function-------------------------//145
        //Reserved_Function-------------------------//146
        //Reserved_Function-------------------------//147
        //Reserved_Function-------------------------//148
        //Reserved_Function-------------------------//149
        //Reserved_Function-------------------------//150
        virtual void ECatReenable(void); // 150

        void SaveLongBlockToEeprom(void);           //151
        virtual void AcceptString(void);            //152 - Stocker & oht

        virtual void UploadParamToIPC(void);        //153 - Stocker & oht
        virtual void DnloadParamFromIPC(void);      //154 - Stocker & oht
        void RunConfigUploadToIPC(void);            //155
        void RunConfigSaveFromIPC(void);            //156

        virtual void SetEncoderOffset(void);        //157 - oht
        virtual void GetEncoderOffset(void);        //158 - oht
        virtual void _dsp_param_flush(void);        //159 - Stocker & oht
        virtual void WatchdogOperations(void);      //160 - oht
        //Reserved_Function-------------------------//161

        void SetEncoderDirection(void);             //162
        void GetEncoderDirection(void);             //163
        void SetSwPause(void);                      //164
        void GetSwPauseEvent(void);                 //165
        void SetSwPauseCheckBit(void);              //166
        void GetSwPauseCheckBit(void);              //167

        void GetMovexCmd(void);                     //168
        void _dsp_clear_amc_error(void);            //169
        virtual void dsp_Get_Error_Status(void);    //170 - stocker

        virtual void FSExchange(void);              //171 - oht
        void AMCFilteringSet(void);                 //172
        void AMCFilteringGet(void);                 //173
        void AMCStatusGet(void);                    //174
        void Torque_Limit(void);                    //175

        virtual void restart_program(void);         //176 - stk
        virtual void set_offset(void);              //177 - stk
        virtual void TraceUpdate_1(void);           //178 - oht
        virtual void TraceUpdate(void);             //179 - oht
        virtual void GetAMCData70(void);            //180 - oht

        void ReloadEncoderPos(void);                //181
        void Put_Vel_Curve(void);                   //182
        virtual void Get_Vel_Curve(void);           //183 - oht
        void Put_Actvel_Margin(void);               //184
        virtual void Get_Actvel_Margin(void);       //185 - oht
        void AMCStatusGet2(void);                   //186

        void SetIoOutput(void);                     //187
        void GetIoOutput(void);                     //188
        virtual void SetIOCount(void);              //189 - oht
        void GetIOCount(void);                      //190

        void ReloadServopackAll(void);              //191

        void GetSystempStatus(void);                //192
        void SetSystemMoniEnable(void);             //193
        void SetSystemMoniDisable(void);            //194
        void GetSystemMoniEnable(void);             //195
        void GetSystemMoniValue(void);              //196
        void SetMoniteringThresholdPercent(void);   //197
        void GetMoniteringThresholdPercent(void);   //198
        virtual void DoAutopatch(void);             //199 - oht
        virtual void GetVersion(void);                      //200 - oht
        virtual void GetEcmVersion(void);                      //201 - oht
        virtual void StopAll(void);  

    protected:
        DPRamStruct dpram_; // ram image
        DPRamTask dpram_task_;
        unique_ptr<MultiAxes> stk_;
        unique_ptr<EcStockerIo> stkio;
        map<int, serviceHandler> service_map_;
        pair<double,double> x;
        array<array< pair<double,double>,3>,4> jogparam_;

        // status
        EcServoMotor& motor_stk(int axis){ *stk_->getMotor(axis);}
    };
}


#define STK_LOGT(...) //SEP_LOGT(__VA_ARGS__)
#define STK_LOGT1(...) //SEP_LOGT(__VA_ARGS__)
#define STK_LOGT2(...) SEP_LOGT(__VA_ARGS__)
#define OHT_LOGT(...) //SEP_LOGT(__VA_ARGS__)
#define OHT_LOGT1(...) SEP_LOGT(__VA_ARGS__)

#endif //SEPMASTER_AMC_EMULATOR_H
