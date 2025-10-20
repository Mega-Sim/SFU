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
#ifndef SEPMASTER_AMC_DATASTRUCT_H
#define SEPMASTER_AMC_DATASTRUCT_H

namespace amc {

//------------------------------------------------------------------------------------------------------
// DRPAM Structure Offset Define
// Description : Axis Structure
//------------------------------------------------------------------------------------------------------
#define TOTAL_AXIS_NUM      4

    typedef struct {
        unsigned int ulStartAddress;
        unsigned int ulTotLen;
        unsigned int uiBlkLen;
        unsigned int ulCrc;
    }__attribute__((packed)) EDUMP_HDR;

    typedef struct //Need to update
    {
        int nMagic;
        int nBlockSize;
        char nBoardNo;
        char nAxisNo;
        char nVerMaj;
        char nVerMin;
    }__attribute__((packed)) UBOOTPARA;

    typedef struct _MoveFrameType {
        double L_Vel; /* CP Motion Velocity */
        double L_Acc; /* CP Motion Accel    */
        double L_Deg; /* ARC Motion Degrees */
        float Ox;
        float Oy;
        float Angle;
        float Deg_Div;
        float Vel[TOTAL_AXIS_NUM]; /* joint speed data */
        int Len;
        double Acc[TOTAL_AXIS_NUM]; /* acceleration data */
        double Dcc[TOTAL_AXIS_NUM]; /* acceleration data */
        int Axis[TOTAL_AXIS_NUM];
        double Pos[TOTAL_AXIS_NUM];
        int MapFunc;
        int Cir_Dir;
    }__attribute__((packed)) MOVE_FRAME_TYPE;

//------------------------------------------------------------------------------------------------------
// DRPAM Axis
// Size:192byte
// Offset : 000 - 191
// Description : Axis Structure
//               AXIS_MAP Offset = 0 + ( Axis_Num * sizeof(DPRAM_AXIS_MAP))
//------------------------------------------------------------------------------------------------------
    struct DPRAM_AXIS_MAP { //Size:192byte - Offset 0:191(Axis*48)   Axis0         Axis1         Axis2         Axis3
        short AD_Board_Exist;                                   //[   1:   0] - [  49:  48] - [  97:  96] - [ 145: 144]
        char AD_In_Pos_Flag;                                    //[   2:   2] - [  50:  50] - [  98:  98] - [ 146: 146]
        char AD_In_Sequence;                                    //[   3:   3] - [  51:  51] - [  99:  99] - [ 147: 147]
        short AD_AxisSource;                                    //[   5:   4] - [  53:  52] - [ 101: 100] - [ 149: 148]
        short AD_AxisState;                                     //[   7:   6] - [  55:  54] - [ 103: 102] - [ 151: 150]
        short AD_Vel;                                           //[   9:   8] - [  57:  56] - [ 105: 104] - [ 153: 152]
        short AD_ActVel;                                        //[  11:  10] - [  59:  58] - [ 107: 106] - [ 155: 154]
        char AD_Frames_Q;                                       //[  12:  12] - [  60:  60] - [ 108: 108] - [ 156: 156]
        char AD_Frames_Interpolation;                           //[  13:  13] - [  61:  61] - [ 109: 109] - [ 157: 157]
        char AD_CharType[2];                                    //[  15:  14] - [  63:  62] - [ 111: 110] - [ 159: 158]
        short AD_IntType[5];                                    //[  25:  16] - [  73:  64] - [ 121: 112] - [ 169: 160]
        int AD_LongType;                                        //[  29:  26] - [  77:  74] - [ 125: 122] - [ 173: 170]
        float AD_FloatType;                                     //[  33:  30] - [  81:  78] - [ 129: 126] - [ 177: 174]
        int AD_LongType1;                                       //[  37:  34] - [  85:  82] - [ 133: 130] - [ 181: 178]
        float AD_FloatType1;                                    //[  41:  38] - [  89:  86] - [ 137: 134] - [ 185: 182]
        char Reserved_42_47[6];                                 //[  47:  42] - [  95:  90] - [ 143: 138] - [ 191: 186] - Reserved
    }__attribute__((packed));
//------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------
// AMC Command Structure
// Size:130byte    -- >>   148byte   ///CIS
// Offset : 192 - 321    -->>        ///CIS
// Description : AMC_CMD structure, Get data from IPC
//               CMD_AXIS Offset = 212 + ( Axis_Num * sizeof(CMD_AXIS))
//------------------------------------------------------------------------------------------------------
    struct CMD_AXIS {//Size:96byte - Offset 212:307(Axis*24)     Axis0         Axis1         Axis2         Axis3
        char CmdType;                                       //[ 212: 212] - [ 236: 236] - [ 260: 260] - [ 284: 284]
                                                            //[ 215: 213] - [ 239: 237] - [ 263: 261] - [ 287: 285]
        int Accel;                                          //[ 219: 216] - [ 243: 240] - [ 267: 264] - [ 291: 288]
        int Decel;                                          //[ 223: 220] - [ 247: 244] - [ 271: 268] - [ 295: 292]
        int Speed;                                          //[ 227: 224] - [ 251: 248] - [ 275: 272] - [ 299: 296]
        int Distance;                                       //[ 231: 228] - [ 255: 252] - [ 279: 276] - [ 303: 300]
        int TriggerCtrlInfo;                                //[ 235: 232] - [ 259: 256] - [ 283: 280] - [ 307: 304]
    };//__attribute__((packed));    ///CIS unpacking

    struct CMD_AXIS_RAW {//Size:96byte - Offset 212:307(Axis*24)     Axis0         Axis1         Axis2         Axis3
        char CmdType;                                       //[ 212: 212] - [ 236: 236] - [ 260: 260] - [ 284: 284]
                                                            //[ 215: 213] - [ 239: 237] - [ 263: 261] - [ 287: 285]
        int Accel;                                          //[ 219: 216] - [ 243: 240] - [ 267: 264] - [ 291: 288]
        int Decel;                                          //[ 223: 220] - [ 247: 244] - [ 271: 268] - [ 295: 292]
        int Speed;                                          //[ 227: 224] - [ 251: 248] - [ 275: 272] - [ 299: 296]
        int Distance;                                       //[ 231: 228] - [ 255: 252] - [ 279: 276] - [ 303: 300]
        int TriggerCtrlInfo;                                //[ 235: 232] - [ 259: 256] - [ 283: 280] - [ 307: 304]
    };//__attribute__((packed));    ///CIS unpacking

    struct DETECT_INFO {//Size:28byte - Offset 308:335
        char UseFollowControlFlag;          //[ 308: 308] - 근접 제어 사용 여부: true 면 사용, false면 미사용; -1 invalid
        char OHTDetectType;                 //[ 309: 309] - OHT Type
        char OBSDetectType;                 //[ 310: 310] - OBS Type
                                            //[ 311: 311] -
        int NodeCount;                      //[ 315: 312] - 노드 개수
        int DistSumOfStraightPath;          //[ 319: 316] - 직진 길이 이어지는 지점까지의 위치 (Cts)
        int FollowCommandType;              //[ 323: 320] -
        int PBSIgnoreFlag;                  //[ 327: 324] -
        int Reserved1;                      //[ 331: 328] -
        int Reserved2;                      //[ 335: 332] -
    };//__attribute__((packed));     ///CIS unpacking

    struct PAUSE_INFO {//Size:8byte - Offset 336:343
        int Pause;                          //[ 339: 336] -
        int Commander;                      //[ 343: 340] -
    };//__attribute__((packed));

    struct OPTIONAL_NODE_INFO { //Size:24byte - Offset 344:367 // yjchoe
        char NodeType;                  //[ 344: 344] - 0x05 : Tag-type Optional Node In, 0x06 : Distance-type Optional Node In, 0x07 : Combo-type Optional Node In
                                        //[ 347: 345] -
        int DistON;                     //[ 351: 348] - Optional Node1 까지의 거리
        int DistAfterON;                //[ 355: 352] - Optional Node1 이후에 패턴이 바뀌는 위치
        char OHTDetectTypeOpt;          //[ 356: 356] - Optional Node1 에서 변경할 Type(대차)
        char OBSDetectTypeOpt;          //[ 357: 357] - Optional Node1 에서 변경할 Type(장애물)
                                        //[ 359: 358] -
        int Reserved1;                  //[ 363: 360] -
        int Reserved2;                  //[ 367: 364] -
    };//__attribute__((packed));
    struct AMC_CMD {//Size:192byte - Offset 192:383
        int Cmd;                            //[ 195: 192] - bit 단위 명령 : CMD_TYPE bit field 참조
        uint32_t Output[4];                 //[ 211: 196] - Output 정보
        CMD_AXIS CmdAxis[4];                //[ 307: 212] - 축 명령 정보
        DETECT_INFO DetectInfo;             //[ 335: 308] - 전방 감지 Type 설정을 위한 정보
        PAUSE_INFO PauseInfo;               //[ 342: 335] -
        OPTIONAL_NODE_INFO OptNodeInfo;     //[ 367: 343]
        int crc16;                          //[ 371: 368]
        int Sound;                          //[ 372: 375] - OHT Main Sound
        int Reserved_cmd1;                  //[ 376: 379]
        int Reserved_cmd2;                  //[ 380: 383]
    }__attribute__((packed));

    struct AMC_CMD_RAW {//Size:192byte - Offset 192:383
        int Cmd;                            //[ 195: 192] - bit 단위 명령 : CMD_TYPE bit field 참조
        uint32_t Output[4];                 //[ 211: 196] - Output 정보
        CMD_AXIS_RAW CmdAxis[4];            //[ 307: 212] - 축 명령 정보
        DETECT_INFO DetectInfo;             //[ 335: 308] - 전방 감지 Type 설정을 위한 정보
        PAUSE_INFO PauseInfo;               //[ 343: 336] -
        OPTIONAL_NODE_INFO OptNodeInfo;     //[ 367: 344]
        int crc16;                          //[ 371: 368]
        int Sound;                          //[ 372: 375] - OHT Main Sound
        int Reserved_cmd1;                  //[ 376: 379]
        int Reserved_cmd2;                  //[ 380: 383]
    }__attribute__((packed));
//------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------
// AMC Data Structure
// Size:256byte
// Offset : 192 - 447
// Description : AMC Data Structure, Send data to IPC from Master
//------------------------------------------------------------------------------------------------------------------------
    struct NODE_CHECK_DATA {//Size : 24byte - Offset 424:447
        int NodeCountOfPreCheck;            //[ 427: 424] - 노드 개수
        int NodeCount;                      //[ 431: 428] - 노드 개수

        // SetHome, setOffset 시 하면 현재 위치를 뺀 값으로 재저장
        int PositionOfDrivingTagPreCheck;   //[ 435: 432] - 주행 노드 사전 감지된 위치
        int PositionOfDrivingTag;           //[ 439: 436] - 주행 노드 감지된 위치
        int PositionOfTransTagFirst;        //[ 443: 440] - 이적재 노드 감지된 위치(Front)
        int PositionOfTransTagFinal;        //[ 447: 444] - 이적재 노드 감지된 위치(Rear)
    }__attribute__((packed));

    struct AMC_DATA {//Size:256byte - 192:447
        int Input[4];                       //[ 207: 192] - InPut 정보
        int Output[4];                      //[ 223: 208] - OutPut 정보

        int Source[4];                      //[ 239: 224] - 축별 source
        int State[4];                       //[ 255: 240]
        char Running[4];                    //[ 259: 256]
        char Inposition[4];                 //[ 263: 260]
        char AmpEnable[4];                  //[ 267: 264]
        int Position[4];                    //[ 283: 268]
        int Velocity[4];                    //[ 299: 284]
        int FollowingError[4];              //[ 315: 300]
        int PositionOfSmallAddCheck[4];     //[ 331: 316]

        int TargetVel[4];                   //[ 347: 332]
        int TargetPos[4];                   //[ 363: 348]
        int FinalPos[4];                    //[ 379: 364]
        int AxisFlag1[4];                   //[ 395: 380]
        int AxisFlag2[4];                   //[ 411: 396]
        int VoltageMonitor[2];              //[ 419: 412]
        int PauseState;                     //[ 423: 420]

        NODE_CHECK_DATA Node_Chk_Data;      //[ 447: 424]
    }__attribute__((packed));

    struct AMC_DATA_RAW {//Size:256byte - 192:447
        uint32_t Input[4];                       //[ 207: 192] - InPut 정보
        uint32_t Output[4];                      //[ 223: 208] - OutPut 정보

        int Source[4];                      //[ 239: 224] - 축별 source
        int State[4];                       //[ 255: 240]
        char Running[4];                    //[ 259: 256]
        char Inposition[4];                 //[ 263: 260]
        char AmpEnable[4];                  //[ 267: 264]
        int Position[4];                    //[ 283: 268]
        int Velocity[4];                    //[ 299: 284]
        int FollowingError[4];              //[ 315: 300]
        int PositionOfSmallAddCheck[4];     //[ 331: 316]

        int TargetVel[4];                   //[ 347: 332]
        int TargetPos[4];                   //[ 363: 348]
        int FinalPos[4];                    //[ 379: 364]
        int AxisFlag1[4];                   //[ 395: 380]
        int AxisFlag2[4];                   //[ 411: 396]
        int VoltageMonitor[2];              //[ 419: 412]
        int PauseState;                     //[ 423: 420]

        NODE_CHECK_DATA Node_Chk_Data;      //[ 447: 424]
    }__attribute__((packed));
//------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------
// Alive check & Long Data Package
// Alive Check
//  - Size : 2byte
//  - Offset : 574-575
// Data Package
//  - Size : 280byte
//  - Offset : 576:855
// Description : Long Data Package
//------------------------------------------------------------------------------------------------------------------------
    struct ALIVE_CHECK {//Size:2Bye - Offset 574-575
        unsigned char Address1;             //[ 574: 574] -
        unsigned char Address2;             //[ 575: 575] -
    }__attribute__((packed));

    struct DOWNLOAD_PARAM {//Size:280byte - Offset 576:855
        unsigned int uiCrc32;               //[ 579: 576] - Total CRC-32
        unsigned int Total_Length;          //[ 583: 580] - Total Byte
        unsigned short Block_Length;        //[ 585: 584] - Total 256byte 단위의 Block count
        unsigned short Block_ID;            //[ 587: 586] - 현재 전송된 Block의 번호, 0부터 시작
        unsigned int Tgt_Addr;              //[ 591: 588] - EEPROM Address
        char Data_Block[256];               //[ 847: 592] - Data Package
        char Reserved[8];                   //[ 855: 848] - Reserved
    }__attribute__((packed));

    struct EXCHANGE_STRING {//Size:280byte - Offset 576:855
        char Str_Length;                    //[ 576: 576] -
        char Str_CheckSum;                  //[ 577: 577] -
        char Str_Reserved;                  //[ 578: 578] -
        char Str_AckLength;                 //[ 579: 579] -
        char Str_AckCheckSum;               //[ 580: 580] -
        char Str_AckErrorNum;               //[ 581: 581] -
        char Str_Body[250];                 //[ 831: 582] -
        char Reserved[24];                  //[ 855: 832] - Reserved
    }__attribute__((packed));

    struct MOVE_X_PARAM {//Size:64Byte - 592:655
        char mode;                          //[ 592: 592] - 0 : pulse/sec^2, 1 : pulse/sec
        //1 : move_p, 2 : move_n, 3 : move_s, 4 : move_ds, 5 : move_dc, 6 : move_dds, 7 : move_ddds
        char cmd;                           //[ 593: 593] -
        short int ax;                       //[ 595: 594] -
        float a[5];                         //[ 615: 596] -
        float v[5];                         //[ 635: 616] -
        float m[5];                         //[ 655: 636] -
    }__attribute__((packed));

    struct MOVE_X {//Size:280byte - Offset 576:855
        char Reserved_0_15[16];             //[ 591: 576] -
        MOVE_X_PARAM Move_X_Param;          //[ 655: 592] -
        char Reserved[200];                 //[ 855: 656] - Reserved
    }__attribute__((packed));

    union DATA_PACKAGE {//Size:280byte - Offset 576:855
        DOWNLOAD_PARAM Dn_Param;            //[ 855: 576]
        EXCHANGE_STRING Exchange_Str;       //[ 855: 576]
        MOVE_X_PARAM Move_X_Param;          //[ 855: 576]
    }__attribute__((packed));

//------------------------------------------------------------------------------------------------------------------------
// Command DPRAM Structure
// Size:540byte
// Offset : 384 - 923
// Description :command structure and IO Data
//------------------------------------------------------------------------------------------------------------------------
    struct COMMAND_DRRAM {//Size : 540byte - offset 384-923
        unsigned short Command;             //[ 385: 384] -
        unsigned short Len;                 //[ 387: 386] -
        int Vel[4];                         //[ 403: 388] -
        char reserved_404_419[16];          //[ 419: 404] - Reserved
        short Acc[4];                       //[ 427: 420] -
        char reserved_428_435[8];           //[ 435: 428] - Reserved
        short Axis[4];                      //[ 443: 436] -
        char reserved_444_451[8];           //[ 451: 444] - Reserved
        int Pos[4];                         //[ 467: 452] -
        char reserved_468_483[16];          //[ 483: 468] - Reserved
        float Ox;                           //[ 487: 484] -
        float Oy;                           //[ 491: 488] -
        float Angle;                        //[ 495: 492] -
        float L_Vel;                        //[ 499: 496] -
        short L_Acc;                        //[ 501: 500] -
        short Cir_Dir;                      //[ 503: 502] -
        float L_Deg;                        //[ 507: 504] -

        int Io_outValue;                    //[ 511: 508] -
        int Io_inValue;                     //[ 515: 512] -
        short AnalogChannel[4];             //[ 523: 516] -
        int Delay;                          //[ 527: 524] -
        short Axis_Num;                     //[ 529: 528] - Axis count
        short Ver;                          //[ 531: 530] - Version
        short Dcc[4];                       //[ 539: 532] -
        char Reserved_156_163[8];           //[ 547: 540] - Reserved
        short Temp_Acc;                     //[ 549: 548] -
        short Temp_Dcc;                     //[ 551: 550] -
        float Temp_Vel;                     //[ 555: 552] -

        short int Spl_Pos[8];               //[ 571: 556] - not used
        char Reserved_186_187[2];           //[ 573: 572] - Reserved

        ALIVE_CHECK Chk_Addr;               //[ 575: 574] -
        DATA_PACKAGE Data;                  //[ 855: 576] -

        int IoinValueR;                     //[ 859: 856] -
        int IoinValue1_1;                   //[ 863: 860] -
        int IoinValue2_1;                   //[ 867: 864] -
        int Io_outValue3;                   //[ 871: 868] -
        int IoinValue3;                     //[ 875: 872] -
        int Io_outValue4;                   //[ 879: 876] -
        int IoinValue4;                     //[ 883: 880] -
        int Io_outValue5;                   //[ 887: 884] -
        int IoinValue5;                     //[ 891: 888] -
        int Io_outValue6;                   //[ 895: 892] -
        int IoinValue6;                     //[ 899: 896] -
        int Io_outValue7;                   //[ 903: 900] -
        int IoinValue7;                     //[ 907: 904] -
        int Io_outValue8;                   //[ 911: 908] -
        int IoinValue8;                     //[ 915: 912] -
        int Io_outValue2;                   //[ 919: 916] -
        int Io_inValue2;                    //[ 923: 920] -
    }__attribute__((packed));
//------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------
// Command & Data structure
// Use the Same Offset
// Size:732byte
// Offset : 192 - 923
// Description :
//------------------------------------------------------------------------------------------------------------------------
    struct CMD {
        AMC_CMD _Amc_Cmd;                   //[ 383: 192]
        //char Reserved_322_383[62];          //[ 383: 322] - Reserved
        //char Reserved_372_383[12];          //[ 383: 372] - Reserved     ///CIS    [62]-->[44]-->[12]
        COMMAND_DRRAM Cmd_Dpram;            //[ 923: 384]
    }__attribute__((packed));

    struct DATA {
        AMC_DATA Data_Dpram;                //[ 447: 192]
        char Reserved_448_923[476];         //[ 923: 448] - Reserved
    }__attribute__((packed));

    union AMC_PACKAGE {//Size:732byte - offset 192 - 923
        CMD _Cmd;                           //[ 923: 192]
        DATA _Data;                         //[ 923: 192]
    }__attribute__((packed));
//------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------
// DRPAM Memory Map structure
//------------------------------------------------------------------------------------------------------------------------
    struct DPRamMemoryMap {//DPRAM Size:1024byte
        DPRAM_AXIS_MAP DPRamAxisMap[4];     //[ 191:   0] - Axis[0] : 0-47, Axis[1] : 48-95, Axis[2] : 96-143, Axis[3] : 144-191
        AMC_PACKAGE _Amc;                   //[ 923: 192]

        char Reserved_924_1007[84];         //[1007 :924] - Reserved
        char Int_Act;                       //[1008:1008] - FPGA가 인터럽트 발생 직후 기록해준 인터페이스용 데이터
        char Int_Cnt;                       //[1009:1009] - FPGA가 인터럽트 발생 직후 기록해준 인터럽트 카운트 갯수 (정상시: "0" 이 읽혀야 함)
        char Watchdog_Time;                 //[1010:1010]
        char Watchdog_Status;               //[1011:1011]
        unsigned char AmpOn_Base[4];        //[1015:1012]
        char Watchdog_Enable;               //[1016:1016]
        char Reserved_1017_1019[3];         //[1017:1019] - Reserved
        char Axis_Num;                      //[1020:1020] - Axis num (0~3)
        char Axis_Command;                  //[1021:1021] - 0으로 지운후 보드검사 명령에 대한 응답이 있는지 확인하는곳
        unsigned char Wakeup_DSP;           //[1022:1022] - Wake up DSP
        unsigned char Ack_DSP;              //[1023:1023]
    public:
        //DPRamMemoryMap() : Wakeup_DSP(*((char*)this + 1022)), Ack_DSP(*((char*)this + 1023)) {}
        void printOffset() {
            printf("#### DPRAM MEMORY MAP #####\n");
#define __MEMBER_OFFSET_PRINT(member)   do{ printf(#member ": %lu\n", (uint64_t)&(this->member) - (uint64_t)this);} while(0)
            __MEMBER_OFFSET_PRINT(DPRamAxisMap[1]);
            __MEMBER_OFFSET_PRINT(_Amc);
            __MEMBER_OFFSET_PRINT(_Amc._Cmd.Cmd_Dpram.Spl_Pos);
            __MEMBER_OFFSET_PRINT(_Amc._Cmd.Cmd_Dpram.Reserved_186_187);
            __MEMBER_OFFSET_PRINT(_Amc._Cmd.Cmd_Dpram.Chk_Addr);
            __MEMBER_OFFSET_PRINT(Int_Act);
            printf("#### ############### #####\n");
        }
    }__attribute__((packed));
//------------------------------------------------------------------------------------------------------------------------

}
#endif //SEPMASTER_AMC_DATASTRUCT_H
