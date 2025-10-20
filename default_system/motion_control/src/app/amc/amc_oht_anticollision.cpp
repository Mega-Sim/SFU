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
// Created by sem on 18. 6. 11.
//

#include "amc_oht.h"

namespace amc {
    
    const std::map<char,uint8_t> obs_setting_map = {
            {OBS_DETECT_CMD_PATH_DIRECTION_NO_USE,       HOKUYO_OBS_DETECT_NO_USE},
            {OBS_DETECT_CMD_PATH_DIRECTION_NORMAL,       HOKUYO_OBS_DETECT_NORMAL_VALUE},
            {OBS_DETECT_CMD_PATH_DIRECTION_LEFT,         HOKUYO_OBS_DETECT_BOTH_VALUE},
            {OBS_DETECT_CMD_PATH_DIRECTION_RIGHT,        HOKUYO_OBS_DETECT_LEFT_VALUE},
            {OBS_DETECT_CMD_PATH_DIRECTION_SHORT,        HOKUYO_OBS_DETECT_RIGHT_VALUE},
            {OBS_DETECT_CMD_PATH_DIRECTION_BOTH,         HOKUYO_OBS_DETECT_RESERVED1},
            {OBS_DETECT_CMD_PATH_DIRECTION_NARROW,       HOKUYO_OBS_DETECT_RESERVED2},
            {OBS_DETECT_CMD_PATH_DIRECTION_RESERVED1,    HOKUYO_OBS_DETECT_RESERVED3},
            {OBS_DETECT_CMD_PATH_DIRECTION_RESERVED2,    HOKUYO_OBS_DETECT_RESERVED4},
            {OBS_DETECT_CMD_PATH_DIRECTION_RESERVED3,    HOKUYO_OBS_DETECT_RESERVED5},
            {OBS_DETECT_CMD_PATH_DIRECTION_RESERVED4,    HOKUYO_OBS_DETECT_RESERVED6},
            {OBS_DETECT_CMD_PATH_DIRECTION_N_LEFT,       HOKUYO_OBS_DETECT_RESERVED7},
            {OBS_DETECT_CMD_PATH_DIRECTION_N_RIGHT,      HOKUYO_OBS_DETECT_RESERVED8},
            {OBS_DETECT_CMD_PATH_DIRECTION_DOUBLE_LEFT,  HOKUYO_OBS_DETECT_RESERVED9},
            {OBS_DETECT_CMD_PATH_DIRECTION_DOUBLE_RIGHT, HOKUYO_OBS_DETECT_RESERVED10},
            {OBS_DETECT_CMD_PATH_DIRECTION_RESERVED5,    HOKUYO_OBS_DETECT_RESERVED11},
    };

    const std::map<char,uint8_t> oht_setting_map = {
            {OHT_DETECT_CMD_PATH_DIRECTION_NO_USE , OHT_DETECT_NO_USE},
            {OHT_DETECT_CMD_PATH_DIRECTION_NORMAL , OHT_DETECT_NORMAL_VALUE},
            {OHT_DETECT_CMD_PATH_DIRECTION_LEFT , OHT_DETECT_LEFT_VALUE},
            {OHT_DETECT_CMD_PATH_DIRECTION_RIGHT , OHT_DETECT_RIGHT_VALUE},
            {OHT_DETECT_CMD_PATH_DIRECTION_SHORT , OHT_DETECT_SHORT_VALUE},
            {OHT_DETECT_CMD_PATH_DIRECTION_BOTH , OHT_DETECT_BOTH_VALUE},
            {OHT_DETECT_CMD_PATH_DIRECTION_NARROW , OHT_DETECT_NARROW_VALUE},
            {OHT_DETECT_CMD_PATH_DIRECTION_NORMAL_HILL, OHT_DETECT_HILL_VALUE},
            {OHT_DETECT_CMD_PATH_DIRECTION_RESERVED2, OHT_DETECT_RESERVED2},
            {OHT_DETECT_CMD_PATH_DIRECTION_RESERVED3, OHT_DETECT_RESERVED3},
            {OHT_DETECT_CMD_PATH_DIRECTION_RESERVED4, OHT_DETECT_RESERVED4},
            {OHT_DETECT_CMD_PATH_DIRECTION_N_RIGHT , OHT_DETECT_NR_VALUE},
            {OHT_DETECT_CMD_PATH_DIRECTION_N_LEFT , OHT_DETECT_NL_VALUE},
            {OHT_DETECT_CMD_PATH_DIRECTION_DOUBLE_LEFT, OHT_DETECT_DOUBLE_LEFT},
            {OHT_DETECT_CMD_PATH_DIRECTION_DOUBLE_RIGHT, OHT_DETECT_DOUBLE_RIGHT},
            {OHT_DETECT_CMD_PATH_DIRECTION_RESERVED5, OHT_DETECT_RESERVED5},
    };

    uint8_t getDetectorSetting(const std::map<char,uint8_t>& mapping, char type, uint8_t onFailure)
    {
        auto res = mapping.find(type);
        return (res != mapping.end()) ? res->second : onFailure;
    }


    void Amc4OHT::setCollisionDetector()
    {
        std::lock_guard<std::mutex> guard(oht_isr_lock_);
        auto amc_cmd = dpram_.Amc_Cmd();
        ChangeNodeSetFlag = controller.localizer_->nodeCounter().getChangeNodeSetFlag();
        ChangeNodeComboSetFlag = controller.localizer_->nodeCounter().getChangeNodeComboSetFlag();
        SetAreaPosition_for_combo = controller.localizer_->nodeCounter().getAreaPosition_for_combo();

        // oht detector, obstacle detector range setting if required
		if(((controller.localizer_->nodeCounter().getCount() >= detector_set_cmd_.NodeCount) && (OHT_Detect_AreaSetting_Flag == true)) || (OHT_Detect_AreaSetting_Flag2 == true))
		{
			// port setting
			if (detector_set_cmd_.FollowCommandType == DETECT_INFO_TYPE_ONLY_UBG || detector_set_cmd_.FollowCommandType == DETECT_INFO_TYPE_BOTH)
			{
			    if (Model_OHT_ID == OHT_NAME_P4MASK) {  // for Mask oht
			        if (Model_OBS_SENSOR_MAKER == HOKUYO) {
                        if(OHT_Detect_AreaSetting == OHT_DETECT_NORMAL_VALUE)       // 14
                            OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_NORMAL_VALUE;    // 14
                        else if(OHT_Detect_AreaSetting == OHT_DETECT_LEFT_VALUE)    // 13
                            OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_LEFT_VALUE;      // 12
                        else if(OHT_Detect_AreaSetting == OHT_DETECT_RIGHT_VALUE)   // 12
                            OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_RIGHT_VALUE;     // 11
                        else if(OHT_Detect_AreaSetting == OHT_DETECT_BOTH_VALUE)    // 10
                            OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_BOTH_VALUE;      // 13
                        else
                            OBS_Detect_AreaSetting = HOKUYO_OBS_DETECT_NO_USE;      // 15
                    } else {
                        if(OHT_Detect_AreaSetting == OHT_DETECT_NORMAL_VALUE)
                            OBS_Detect_AreaSetting = SICK_OBS_DETECT_NORMAL_VALUE;
                        else if(OHT_Detect_AreaSetting == OHT_DETECT_LEFT_VALUE)
                            OBS_Detect_AreaSetting = SICK_OBS_DETECT_LEFT_VALUE;
                        else if(OHT_Detect_AreaSetting == OHT_DETECT_RIGHT_VALUE)
                            OBS_Detect_AreaSetting = SICK_OBS_DETECT_RIGHT_VALUE;
                        else if(OHT_Detect_AreaSetting == OHT_DETECT_BOTH_VALUE)
                            OBS_Detect_AreaSetting = SICK_OBS_DETECT_BOTH_VALUE;
                        else
                            OBS_Detect_AreaSetting = SICK_OBS_DETECT_RESERVED4;
                    }
			    } else {
                    OBS_Detect_AreaSetting = tmp_OBS_Detect_AreaSetting;
			    }
			    // OHT&OBS Sensor Setting
                //SetDetectorOutput(OHT_Detect_AreaSetting, OBS_Detect_AreaSetting, detector_set_cmd_.OHTDetectType, detector_set_cmd_.OBSDetectType);
                OHT_Detect_AreaSetting_For_Curve = OHT_Detect_AreaSetting;
                SetDetectorOutput(OHT_Detect_AreaSetting_For_Curve, OBS_Detect_AreaSetting);
			}

			/*SEP_LOGT("[Area Change]NodeCount : %d[%d], Area : %d",
			        controller.localizer_->nodeCounter().getCount(), detector_set_cmd_.NodeCount, OHT_Detect_AreaSetting);*/

			// reset UseFollowControlFlag to confirm that request is treated
			OHT_Detect_AreaSetting_Flag = false;
			OHT_Detect_AreaSetting_Flag2 = false;

            // optional node
            if(SelectChangeNode) {
                // Enable ChangeNodeMode
                if((OptNodeInfo_.NodeType == OPT_TAG || OptNodeInfo_.NodeType == OPT_DISTANCE || OptNodeInfo_.NodeType == OPT_COMBO)
                   && OptNodeInfo_.DistON > 0) {
                    SetAreaPosition = (int)front_wheel->getPosPulse();
                    EnableChangeNodeMode = true;
                    //SEP_LOGT("[Optional Node] Enable mode!, Type[%d] Pos[%d] DistOn[%d]",
                    //         OptNodeInfo_.NodeType, SetAreaPosition, OptNodeInfo_.DistON);
                }
            }
		}

        if(EnableChangeNodeMode == true) {
            //Based Sensor
            if(OptNodeInfo_.NodeType == OPT_TAG) {
                //센서로 영역 안바궜을때(미감지 등) 거리기반으로 변경해준다
                if((abs(front_wheel->getPosPulse() - SetAreaPosition) >= OptNodeInfo_.DistON + CHANGENODE_DIST_LIMIT)
                   && (ChangeNodeSetFlag == false)) {
                    ChangeNodeSetFlag = true;
                    //SEP_LOGT("[Optional Node] Set flag!, Type[%d] Dist[%d]",
                    //         OptNodeInfo_.NodeType, abs((int)front_wheel->getPosPulse() - SetAreaPosition));
                }
            }
                //Based Distance
            else if(OptNodeInfo_.NodeType == OPT_DISTANCE) {
                //Check Distance(Optional Node 1)
                if((abs(front_wheel->getPosPulse() - SetAreaPosition) >= OptNodeInfo_.DistON)
                   && (ChangeNodeSetFlag == false)) {
                    ChangeNodeSetFlag = true;
                    //SEP_LOGT("[Optional Node] Set flag!, Type[%d] Dist[%d]",
                    //         OptNodeInfo_.NodeType, abs((int)front_wheel->getPosPulse() - SetAreaPosition));
                }
            }
            else if(OptNodeInfo_.NodeType == OPT_COMBO) {
                //센서로 영역 안바궜을때(미감지 등) 거리기반으로 변경해준다
                if(abs(front_wheel->getPosPulse() - SetAreaPosition) >= OptNodeInfo_.DistON + CHANGENODE_DIST_LIMIT
                   && !ChangeNodeComboSetFlag) {
                    SetAreaPosition_for_combo = (int)front_wheel->getPosPulse();
                    ChangeNodeComboSetFlag = true;
                    //SEP_LOGT("[Optional Node] Set combo flag!, Type[%d] Dist[%d], Pos[%d]",
                    //         OptNodeInfo_.NodeType, abs((int)front_wheel->getPosPulse() - SetAreaPosition), SetAreaPosition_for_combo);
                }
                //Check Distance(Optional Node 1)
                if(abs(front_wheel->getPosPulse() - SetAreaPosition_for_combo) >= OptNodeInfo_.DistAfterON
                   && ChangeNodeComboSetFlag && !ChangeNodeSetFlag) {
                    ChangeNodeSetFlag = true;
                    //SEP_LOGT("[Optional Node] Set flag!, Type[%d] dist[%d]",
                    //         OptNodeInfo_.NodeType, abs((int)front_wheel->getPosPulse() - SetAreaPosition_for_combo));
                }
            }

            //Set Area Optional Node 1
            if(ChangeNodeSetFlag == true) {
                // OHT&OBS Sensor Setting
                //SetDetectorOutput(ChangeNodeOHTDetectArea, ChangeNodeOBSDetectArea, OptNodeInfo_.OHTDetectTypeOpt, OptNodeInfo_.OBSDetectTypeOpt);
                SetDetectorOutput(ChangeNodeOHTDetectArea, ChangeNodeOBSDetectArea);
                //SEP_LOGT("[Optional Node] Area Change, NodeCount : %d[%d], Area : %d/%d",
                //         controller.localizer_->nodeCounter().getCount(), detector_set_cmd_.NodeCount, ChangeNodeOHTDetectArea, ChangeNodeOBSDetectArea);

                OHT_Detect_AreaSetting_For_Curve = ChangeNodeOHTDetectArea;
                OBS_Detect_AreaSetting = ChangeNodeOBSDetectArea;

                Init_ChangeNodeInfo();
            }
        }

        // PBS Ignore
        if ((detector_set_cmd_.NodeCount >= OHT_PBS_IgnoreChangeNodeValue) && OHT_PBS_IgnoreSettingFlag) {
            PBS_Ignore_Flag = OHT_PBS_IgnoreFlag;
            OHT_PBS_IgnoreSettingFlag = false;
        }

        controller.localizer_->nodeCounter().setOptionalNode(EnableChangeNodeMode);
        controller.localizer_->nodePrecheckCounter().setOptionalNode(EnableChangeNodeMode);
        controller.localizer_->nodeCounter().setChangeNodeComboSetFlag(ChangeNodeComboSetFlag);
        controller.localizer_->nodeCounter().setChangeNodeSetFlag(ChangeNodeSetFlag);
    }



    // on-dec-done event handler
    int onFrontWheelDecDone(IEventHost*svc, int evid, void *context)
    {
        using time_point = std::chrono::system_clock::time_point;
        time_point& oht_dec_end_time_ = *(time_point*)context;
		oht_dec_end_time_ = std::chrono::system_clock::now();
        return 0;
    }

	void Amc4OHT::registerAntiCollisionControlTimer(void)
    {
        //front_wheel->addEventHandler(RoboticModule::EV_DEC_END, onFrontWheelDecDone, &oht_dec_stop_time_);
	}

	double Amc4OHT::Get_Velocity_by_Area(void)
	{
		OBS_velocity_Level = 0.0;
		OHT_velocity_Level = 0.0;
		int obs_ignore_distance = abs(detector_set_cmd_.DistSumOfStraightPath - (int)front_wheel->getPosPulse());
		//unsigned obs_sensor_val = unsigned(IO_Input[0] >> Model_OBS_DETECT_READ_BIT_SCALE) & 0xF;
		//unsigned oht_sensor_val = unsigned(IO_Input[1] >> Model_OHT_DETECT_READ_BIT_SCALE) & 0xF;
        //obs_sensor_val = CalBit(IO_Input, Model_OBS_DETECT_READ_BIT_SCALE, 4);
        //oht_sensor_val = CalBit(IO_Input, Model_OHT_DETECT_READ_BIT_SCALE, 4);
        unsigned new_obs = CalBit(IO_Input, Model_OBS_DETECT_READ_BIT_SCALE, 4);
        unsigned new_oht = CalBit(IO_Input, Model_OHT_DETECT_READ_BIT_SCALE, 4);

        obsdetect.DetectFilter(new_obs, obs_sensor_val, 2);
        ohtdetect.DetectFilter(new_oht, oht_sensor_val, 2);

		// bit inversion to match polarity logic
        //obs_sensor_val = ~obs_sensor_val & 0xF;    //CIS  // DetectFilter()내 반전 시켜주어 주석처리
        //oht_sensor_val = ~oht_sensor_val & 0xF;   //CIS   // DetectFilter()내 반전 시켜주어 주석처리
        OHT_Sensor_value = oht_sensor_val;  // for low vibration, 210913 yjchoe
//		static char prev_obs = 0x00;
//		static char prev_oht = 0x00;

		/*if(prev_obs != obs_sensor_val || prev_oht != oht_sensor_val) {
			SEP_LOGT("[FollowControl] OHT Detect [0x%x], OBS Detect [0x%x]\n", oht_sensor_val, obs_sensor_val);
			prev_obs = obs_sensor_val;
			prev_oht = oht_sensor_val;
		}*/
		
		switch(obs_sensor_val)
		{
			case HOKUYO_OBS_SPEED_GRADE_8_BIT:
				OBS_velocity_Level = Model_SPEED_GRADE_8_VELOCITY;
				break;
			case HOKUYO_OBS_SPEED_GRADE_7_BIT :
				if((obs_ignore_distance < (Model_PULSETOMM*2000)) && ((OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_NORMAL_VALUE)||(OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_HILL_VALUE)))
					OBS_velocity_Level = Model_SPEED_GRADE_8_VELOCITY;
				else
					OBS_velocity_Level = Model_SPEED_GRADE_7_VELOCITY;
				break;
			case HOKUYO_OBS_SPEED_GRADE_5_BIT :
				if((obs_ignore_distance < (Model_PULSETOMM*1000)) && ((OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_NORMAL_VALUE)||(OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_HILL_VALUE)))
					OBS_velocity_Level = Model_SPEED_GRADE_8_VELOCITY;
				else
					OBS_velocity_Level = Model_SPEED_GRADE_5_VELOCITY;
				break;
			case HOKUYO_OBS_SPEED_GRADE_3_BIT :
				OBS_velocity_Level = Model_SPEED_GRADE_3_VELOCITY;
				break;
			case HOKUYO_OBS_SPEED_GRADE_STEP_OUT :
				OBS_velocity_Level = Model_SPEED_GRADE_8_VELOCITY;
				break;
			default :
				OBS_velocity_Level = Model_SPEED_GRADE_1_VELOCITY;
				break;
		}
		
		if(PBS_Ignore_Flag == true)
		{
			OBS_velocity_Level = Model_SPEED_GRADE_8_VELOCITY;
		}

        static int obsFailCnt = 0;
		if(!((obs_sensor_val >> 3) & 0x1))
		{
			OBS_velocity_Level = Model_SPEED_GRADE_1_VELOCITY;
			if(obsFailCnt == 0) {
				SEP_LOGT("[FollowControl] OBS Detect Seneor Fault!!");
                obsFailCnt++;
			}
		} else {
            obsFailCnt = 0;
		}

		switch(oht_sensor_val)
		{
			case OHT_SPEED_GRADE_8_BIT:
				OHT_velocity_Level = Model_SPEED_GRADE_8_VELOCITY;
				break;

			case OHT_SPEED_GRADE_7_BIT :
				OHT_velocity_Level = Model_SPEED_GRADE_7_VELOCITY;
				break;

			case OHT_SPEED_GRADE_6_BIT :
				OHT_velocity_Level = Model_SPEED_GRADE_6_VELOCITY;
				break;

			case OHT_SPEED_GRADE_5_BIT :
				OHT_velocity_Level = Model_SPEED_GRADE_5_VELOCITY;
				break;

			case OHT_SPEED_GRADE_4_BIT :
				OHT_velocity_Level = Model_SPEED_GRADE_4_VELOCITY;
				break;

			case OHT_SPEED_GRADE_3_BIT :
				OHT_velocity_Level = Model_SPEED_GRADE_3_VELOCITY;
				break;

			default :
				OHT_velocity_Level = Model_SPEED_GRADE_1_VELOCITY;
				break;
		}

        static int ohtFailCnt = 0;
		if(!((oht_sensor_val >> 3) & 0x1))
        {
            OHT_velocity_Level = Model_SPEED_GRADE_1_VELOCITY;
			if(ohtFailCnt == 0) {
				SEP_LOGT("[FollowControl] OHT Detect Seneor Fault!!");
                ohtFailCnt++;
			}
        } else {
            ohtFailCnt = 0;
		}

		if(OHT_velocity_Level <= OBS_velocity_Level)
		{
			Sensor_velocity_Level = OHT_velocity_Level;

			if(Sensor_velocity_Level == Model_SPEED_GRADE_1_VELOCITY)
			{
				AMC_Follow_Control_Status = STATUS_OBS_STOP;
			}
			else
			{
				AMC_Follow_Control_Status = STATUS_NORMAL;
			}
		}
		else
        {
            AMC_Follow_Control_Status = STATUS_PBS_DETECT;
            if (OBS_velocity_Level == Model_SPEED_GRADE_7_VELOCITY)
            {
                if ((OHT_Detect_AreaSetting != OHT_DETECT_NORMAL_VALUE) && (OHT_Detect_AreaSetting != OHT_DETECT_HILL_VALUE))
                {
                    Sensor_velocity_Level = OBS_SENSOR_VEL_LONG;
                }
                else
                {
                    if((Model_OHT_ID == NEO_TRAY) || (Model_OHT_ID == NEO_SSD))
                    {
                        Sensor_velocity_Level = Model_SPEED_GRADE_5_VELOCITY;
                    }
                    else if((Model_OHT_ID == NEO_MAGAZINE) || (Model_OHT_ID == NEO_CASSETTE)
                    || (Model_OHT_ID == FOSB_2WD) || (Model_OHT_ID == OHT_NAME_MASK) || (Model_OHT_ID == OHT_NAME_P4MASK))
                    {
                        Sensor_velocity_Level = Model_SPEED_GRADE_4_VELOCITY;
                    }
                    else
                    {
                        Sensor_velocity_Level = Model_SPEED_GRADE_6_VELOCITY;
                    }
                }
            }
            else if (OBS_velocity_Level == Model_SPEED_GRADE_5_VELOCITY)
            {
                if ((OHT_Detect_AreaSetting != OHT_DETECT_NORMAL_VALUE) && (OHT_Detect_AreaSetting != OHT_DETECT_HILL_VALUE))
                {
                    Sensor_velocity_Level = OBS_SENSOR_VEL_MIDDLE;
                }
                else
                {
                    if((Model_OHT_ID == NEO_TRAY) || (Model_OHT_ID == NEO_SSD))
                    {
                        Sensor_velocity_Level = Model_SPEED_GRADE_3_VELOCITY;
                    }
                    else if((Model_OHT_ID == NEO_MAGAZINE) || (Model_OHT_ID == NEO_CASSETTE)
                    || (Model_OHT_ID == FOSB_2WD) || (Model_OHT_ID == OHT_NAME_MASK) || (Model_OHT_ID == OHT_NAME_P4MASK))
                    {
                        Sensor_velocity_Level = Model_SPEED_GRADE_2_VELOCITY;
                    }
                    else
                    {
                        Sensor_velocity_Level = Model_SPEED_GRADE_4_VELOCITY;
                    }
                }
            }
            else
            {
                Sensor_velocity_Level = Model_SPEED_GRADE_1_VELOCITY;
                AMC_Follow_Control_Status = STATUS_PBS_STOP;
            }
        }
		
		/*static int prev_status = 0;
		if(prev_status != AMC_Follow_Control_Status) {
			SEP_LOGT("[FollowControl] Detect Status : %d  [0 : Normal, 1 : OHT, 2: OBS]", AMC_Follow_Control_Status);
			prev_status = AMC_Follow_Control_Status;
		}*/
		
		if(AMC_Pause_Control == true)
		{
			Sensor_velocity_Level = Model_SPEED_GRADE_1_VELOCITY;
			AMC_Follow_Control_Status = STATUS_OHT_PAUSED;
		}

		if((Servo_Off_type == 1) && (TorqueLimit_Response != 20))
		{
			if((TorqueLimit_Response == 10) && (Sensor_velocity_Level != Model_SPEED_GRADE_1_VELOCITY))
			{
				TorqueLimit_Request = 2;
			}
			Sensor_velocity_Level = Model_SPEED_GRADE_1_VELOCITY;
			AMC_Follow_Control_Status = STATUS_OHT_PAUSED;
		}
		return Sensor_velocity_Level;
	}

	void Amc4OHT::updateAntiCollisionControl_test(void)
	{
		int ax = 0;
		std::lock_guard<std::mutex> guard(oht_isr_lock_);

        //if(runningflag[0] == true && Follows_MoveDS_Flag != true) {   // TODO - running flag -> motion state
        //    Follows_MoveDS_Flag = true;
        //}

		// Small ADD 기능 구현
		// 20.08.11 수정, yjchoe
		for(ax = 0; ax < 4; ax++) {
            if (Marking_Process_Flag[ax] == true) {
                int MarkDirection = 0;
                if(DirectionFlag_For_Marking[ax] == true)
                    MarkDirection = 1;
                else
                    MarkDirection = -1;

                int MarkSensor = GetBit(IO_Input, IoNumber_For_Marking[ax]);
                if ((MarkSensor == IoType_For_Marking[ax]) && (FlagIO_For_Marking[ax] != IoType_For_Marking[ax])) {
					//printf("sensor detect, %d\n", GetBit(IO_Input, IoNumber_For_Marking[ax]));
                    // 센서 인식
                    /*SEP_LOGT("[FollowControl] (Sensor Deteting) Input Num[%d] Input Data[%d] CurPos[%f]",
                             IoNumber_For_Marking[ax], IoType_For_Marking[ax], oht_[ax]->getPosPulse());*/
                    JtPos_During_Marking[ax] = (oht_[ax]->getPosPulse() == 0 ? 1 : oht_[ax]->getPosPulse());
                    POS_For_Marking[ax] = JtPos_During_Marking[ax] + (DistanceAddition_For_Marking[ax] * MarkDirection);

                    if (ax == 0) {
                        if (!Follows_MoveDS_Flag) {
                            Move_Flag = 9;
                            front_wheel->moveToPulse(POS_For_Marking[ax], VEL_For_Marking[ax], ACC_For_Marking[ax], DCC_For_Marking[ax]);
                            /*SEP_LOGT("[FollowControl] Mark #1");
                            SEP_LOGT("[FollowControl] (MoveA_2) CurPos[%f] TargetPos[%d] TargetVel[%f] TargetAcc[%f] TargetDcc[%f]",
                                     oht_[ax]->getPosPulse(), POS_For_Marking[ax], VEL_For_Marking[ax], ACC_For_Marking[ax], DCC_For_Marking[ax]);*/
                            Trace_Execute_1(ax, ACC_For_Marking[ax], DCC_For_Marking[ax], VEL_For_Marking[ax], POS_For_Marking[ax]);
                        } else {
                            //if (ax == 0) {
                                //SEP_LOGT ("[FollowControl] Mark #2");
                                Move_Flag = 8;
                                FollowsACC = ACC_For_Marking[ax];
                                FollowsDCC = DCC_For_Marking[ax];
                                FollowsVEL = VEL_For_Marking[ax];
                                FollowsPOS = POS_For_Marking[ax];
                            //}
                        }
                    } else if(ax == 2) { // Small add for hoist Auto home, 210616 yjchoe
                        Move_Flag = 10;
                        hoist->moveToPulse(POS_For_Marking[ax], VEL_For_Marking[ax], ACC_For_Marking[ax], DCC_For_Marking[ax]);
                        /*SEP_LOGT("[FollowControl] Mark #3");
                        SEP_LOGT("[FollowControl] (MoveA_8) CurPos[%f] TargetPos[%d] TargetVel[%f] TargetAcc[%f] TargetDcc[%f]",
                                oht_[ax]->getPosPulse(), POS_For_Marking[ax], VEL_For_Marking[ax], ACC_For_Marking[ax], DCC_For_Marking[ax]);*/
                        Trace_Execute_1(ax, ACC_For_Marking[ax], DCC_For_Marking[ax], VEL_For_Marking[ax], POS_For_Marking[ax]);
                    }
                    Marking_Process_Flag[ax] = false;
                } else {
                    JtPos_During_Marking[ax] = 0;
                }
                FlagIO_For_Marking[ax] = MarkSensor;

				//printf("IoNumber_For_Marking : %d\n", IoNumber_For_Marking[ax]);
				//printf("sensor detect, %d\n", GetBit(IO_Input, 51));
				//printf("Calbit : %d\n", CalBit(IO_Input, 51, 1));
				//printf("Getbit : %d\n", GetBit(IO_Input[1], 51));
            }
			CurrentSpeedMode(ax, oht_[ax]->getRefVel(), prev_vel[ax]);
			prev_vel[ax] = oht_[ax]->getRefVel();
        }

        CheckNodeCountStatus(); // nodecount == 1 -> precheck = 1

		double currentPos = oht_[0]->getPosPulse();
		double currentVel = oht_[0]->getVelPulse();     // pulse/sec
		double refPos = oht_[0]->getRefPosPulse();
		double refVel = oht_[0]->getRefVelPulse();
        double currtneVel_MPS = currentVel/Model_PULSETOMM/1000;  // m/s
		double refVel_MPS = refVel/Model_PULSETOMM/1000;  // m/s

		if(!Follows_MoveDS_Flag)
		{
			return;
		}

		if(Rear_servo_Off_flag == false)
		{
			Check_Rear_servo_Off();
		}

		Target_Velocity_by_Sensor = Get_Velocity_by_Area();
        //front_wheel->OhtDetectLv(Target_Velocity_by_Sensor/Model_PULSETOMM);

		// 저진동 직선 감속도 관련 flag, yjchoe
        //static int bIsPosSoftStopToMark = 0;
        static int prevSoftStop = 0;
        static int prevStopmotionflag = 0;
        // 최종 타겟까지 거리 확인
        if ((FollowsPOS - currentPos) > (110 * Model_PULSETOMM)) {
            bIsPosSoftStopToMark = 1;
        }
        else
            bIsPosSoftStopToMark = 0;

        /*if (prevSoftStop != bIsPosSoftStopToMark) {
            SEP_LOGT("[FollowControl] Ramaining dist flag : %d [%.0f/%.0f/%.0f]",
                     bIsPosSoftStopToMark, FollowsPOS, currentPos, (FollowsPOS - currentPos));
        }
        prevSoftStop = bIsPosSoftStopToMark;*/

        // 정지거리 마진 확인
        if (currtneVel_MPS > 0.4 || currentSpeedMode[0] == ACC_MODE) {
            bStop_motion_flag = 0;
        } else if ((currentSpeedMode[0] == CRUISE_MODE) && (OHT_Sensor_value == OHT_SPEED_GRADE_3_BIT) && (currtneVel_MPS <= 0.3)) {
            if (bIsPosSoftStopToMark)       bStop_motion_flag = 1;
            else                            bStop_motion_flag = 0;
        }

        /*if(prevStopmotionflag != bStop_motion_flag)
        SEP_LOGT("[FollowControl] Softstop : %d [%d/%x/%.2f]",
                 bStop_motion_flag, currentSpeedMode[0], OHT_Sensor_value, currtneVel_MPS);
        prevStopmotionflag = bStop_motion_flag;*/

		if(PauseFlag) {
		    if(PauseBy == PAUSED_BY_OCS)
		        AMC_Pause_Control = true;
		    else if(PauseBy == PAUSED_BY_TP) {
		        if(AMC_Follow_Control_Status == STATUS_PBS_STOP || AMC_Follow_Control_Status == STATUS_OBS_STOP)
		            AMC_Pause_Control = true;
		        else if(AMC_Pause_Control != true)
		            PauseFlag = PAUSED_NONE;
		    }
		} else
		    AMC_Pause_Control = false;

		if(Target_Velocity_by_Sensor != Model_SPEED_GRADE_8_VELOCITY)
		{
			if(Path_Slope_Status == 1)
			{
				FollowsDCC = PATH_SLOPE_DCC;
				Model_MOVE_S_DCC_BY_SENSOR_NORMAL = PATH_SLOPE_DCC;
			}
			else
			{
				FollowsDCC = Model_FOLLOWSDCC;
                if((Improve_stop_motion_flag == 1) && (bStop_motion_flag == 1)) // yjchoe
				    Model_MOVE_S_DCC_BY_SENSOR_NORMAL = STOP_MOTION_DCC_LOW;
				else
				    Model_MOVE_S_DCC_BY_SENSOR_NORMAL = Model_FOLLOWSDCC;
			}

			if(currentVel < (Model_SPEED_GRADE_4_VELOCITY*0.9)) {
			    if(currentVel < (Target_Velocity_by_Sensor*0.9))
			        FollowsDCC = FollowsDCC_CMD;
			}
		}

        // Dynamic Acc
        tmpFollowsACC = DynamicACC(currtneVel_MPS);
		//tmpFollowsACC = DynamicACC(refVel_MPS);
        //printf("vel : %f, ACC : %f\n", inputVel, tmpFollowsACC);

		if(FollowsVEL >= Target_Velocity_by_Sensor)
		{
			Target_velocity_cmd = Target_Velocity_by_Sensor;
		}
		else
		{
			Target_velocity_cmd = FollowsVEL;
		}

        double stop_time = refVel / Model_MOVE_S_DCC_BY_SENSOR_NORMAL;
        auto stop_dist = sign(refVel)* 0.5 * Model_MOVE_S_DCC_BY_SENSOR_NORMAL * stop_time * stop_time;

        if (refPos + stop_dist > FollowsPOS) {
            overtarget = true;  // 현재 위치에서 3.5m/s^2 정지 시 타겟을 넘어가는 경우
        } else if (currtneVel_MPS <= 0.1) {
            overtarget = false; // 정지 완료시점에 비활성화
        }

		// 정지 모션
		if(Target_velocity_cmd == Model_SPEED_GRADE_1_VELOCITY) {
            MoveStopMotion(OHT_Detect_AreaSetting_For_Curve);
		}
		// 감속 모션
		else {
			if(Rear_servo_Off_flag)
			{
				if(Servo_Off_type == 1)
				{
					if(TorqueLimit_Response == 20)
					{
                        // Servo off/on 로직 삭제
						//Check_Front_Rear_servo_On();
                        MoveDS_Wait_Count_After_Servo_On = 0;
                        MoveDS_Wait_Count_After_Servo_On_Flag = true;
						Rear_servo_Off_flag = false;
						Servo_Off_type = 0;
						TorqueLimit_Response = 0;
                        //Check_Front_Rear_servo_On();
					}
					else
					{
						//SEP_LOGT("[FollowControl] ServoON #1");
						return;
					}
				}
				else
				{
                    // Servo off/on 로직 삭제
					//Check_Front_Rear_servo_On();
                    MoveDS_Wait_Count_After_Servo_On = 0;
                    MoveDS_Wait_Count_After_Servo_On_Flag = true;
					Servo_Off_type = 0;
					Rear_servo_Off_flag = false;
                    //Check_Front_Rear_servo_On();
				}
			}
			/*
            if(MoveDS_Wait_Count_After_Servo_On_Flag)
            {
                if(MoveDS_Wait_Count_After_Servo_On <= 500)
                {
                    MoveDS_Wait_Count_After_Servo_On++;
                    AMC_Stop_count = 0;
                    return;
                }
                else
                {
                    AMC_Stop_count = 0;
                    MoveDS_Wait_Count_After_Servo_On_Flag = false;
                }
            }
            */
			MoveDistanceMotion(OHT_Detect_AreaSetting_For_Curve);
		}

		
		static int nCnt = 0;

/*		if(nCnt++ > 100)
		{
		    if(oht_[0]->getRefVel() != 0) {
                SEP_LOGT("follow_vel:%f, cur_vel:%f, ref_vel:%f", Target_Velocity_by_Sensor, motors[0]->getActual()[1], oht_[0]->getRefVel());
		    }
			nCnt = 0;
		}*/
	}
	/**
     *  1. calculate sensor-driven velocity limit level
     *  2. reschedule speed profile according to the following state transition table
     *      target_v : current motion target, follow_v : given target from IPC
     *                      condition,                          action
     *      S0 --> S1 :  sens_v < target_v(==follow_v)
     *      S1 --> S2 :  sens_v > target_v,  stop deceleration & keep speed for a while to prevent radical change of accelerations
     *      S2 --> S0 :  sens_v > follow_v
     *      S2 --> S1 :  sens_v < target_v
     */
    void Amc4OHT::updateAntiCollisionControl(void)
    {
		auto getObsVelocityLevel = [this](unsigned sensor_val, int ignore_dist) {
			switch(sensor_val){
				case HOKUYO_OBS_SPEED_GRADE_8_BIT: return Model_SPEED_GRADE_8_VELOCITY;
				case HOKUYO_OBS_SPEED_GRADE_7_BIT :
					if((ignore_dist < (Model_PULSETOMM*2000)) && ((OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_NORMAL_VALUE)||(OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_HILL_VALUE)))
						return Model_SPEED_GRADE_8_VELOCITY;
					else
						return Model_SPEED_GRADE_7_VELOCITY;
				case HOKUYO_OBS_SPEED_GRADE_5_BIT :
					if((ignore_dist < (Model_PULSETOMM*1000)) && ((OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_NORMAL_VALUE)||(OHT_Detect_AreaSetting_For_Curve == OHT_DETECT_HILL_VALUE)))
						return Model_SPEED_GRADE_8_VELOCITY;
					else
						return Model_SPEED_GRADE_5_VELOCITY;
				case HOKUYO_OBS_SPEED_GRADE_3_BIT : return Model_SPEED_GRADE_3_VELOCITY;
				case HOKUYO_OBS_SPEED_GRADE_STEP_OUT : return Model_SPEED_GRADE_8_VELOCITY;
			}
			return Model_SPEED_GRADE_1_VELOCITY;
		};

        auto getOhtVelocityLevel = [this](unsigned sensor_val) {
            switch(sensor_val) {
                case OHT_SPEED_GRADE_8_BIT: return Model_SPEED_GRADE_8_VELOCITY;
                case OHT_SPEED_GRADE_7_BIT :return Model_SPEED_GRADE_7_VELOCITY;
                case OHT_SPEED_GRADE_6_BIT :return Model_SPEED_GRADE_6_VELOCITY;
                case OHT_SPEED_GRADE_5_BIT :return Model_SPEED_GRADE_5_VELOCITY;
                case OHT_SPEED_GRADE_4_BIT :return Model_SPEED_GRADE_4_VELOCITY;
                case OHT_SPEED_GRADE_3_BIT :return Model_SPEED_GRADE_3_VELOCITY;
            }
            return Model_SPEED_GRADE_1_VELOCITY;
        };

        auto equal_to = [](double a, double b) { return fabs(a-b)<1e-30; };
        auto greater_than = [](double a, double b) { return a > b + 1e-30; };
        auto greater_than_equal_to = [](double a, double b) { return a > b - 1e-30; };
        auto less_than = [](double a, double b) { return a < b - 1e-30; };

		std::lock_guard<std::mutex> guard(oht_isr_lock_);

		unsigned obs_sensor_val = unsigned(IO_Input[0] >> Model_OBS_DETECT_READ_BIT_SCALE) & 0xF;
		unsigned oht_sensor_val = unsigned(IO_Input[1] >> Model_OHT_DETECT_READ_BIT_SCALE) & 0xF;

		// bit inversion to match polarity logic
        obs_sensor_val = ~obs_sensor_val & 0xF;
		oht_sensor_val = ~oht_sensor_val & 0xF;

		int obs_ignore_distance = abs(detector_set_cmd_.DistSumOfStraightPath - (int)front_wheel->getPosPulse());
		double obs_vel_level = getObsVelocityLevel(obs_sensor_val, obs_ignore_distance);

		if(PBS_Ignore_Flag == true){
			obs_vel_level = Model_SPEED_GRADE_8_VELOCITY;
		}
		if(!((obs_sensor_val >> 3) & 0x1)){ // fault detection
			obs_vel_level = Model_SPEED_GRADE_1_VELOCITY;
		}

        double oht_vel_level = getOhtVelocityLevel(oht_sensor_val);

        if(!((oht_sensor_val >> 3) & 0x1)){ // fault detection
            oht_vel_level = Model_SPEED_GRADE_1_VELOCITY;
        }
        if(Model_OHT_SENSOR_ENABLE == false){
            oht_vel_level = Model_SPEED_GRADE_8_VELOCITY;
        }

        double sensor_vel_level_pps{Model_SPEED_GRADE_1_VELOCITY};

       if (oht_vel_level <= obs_vel_level) {
            sensor_vel_level_pps = oht_vel_level;
            AMC_Follow_Control_Status = equal_to(sensor_vel_level_pps, Model_SPEED_GRADE_1_VELOCITY) ? STATUS_OBS_STOP : STATUS_NORMAL;
        } else {
            AMC_Follow_Control_Status = STATUS_NORMAL;
			if (equal_to(obs_vel_level, Model_SPEED_GRADE_7_VELOCITY)) {
				SEP_LOGT("set to Model_SPEED_GRADE_7_VELOCITY by obs_vel_level");
                if ((OHT_Detect_AreaSetting != OHT_DETECT_NORMAL_VALUE) &&
					(OHT_Detect_AreaSetting != OHT_DETECT_HILL_VALUE)) {
                    sensor_vel_level_pps = VELOCITY_0_5_MPS;
                } else {
                    sensor_vel_level_pps = Model_SPEED_GRADE_6_VELOCITY;
                }
            } else if (equal_to(obs_vel_level, Model_SPEED_GRADE_5_VELOCITY)) {
				SEP_LOGT("set to Model_SPEED_GRADE_5_VELOCITY by obs_vel_level");
				if ((OHT_Detect_AreaSetting != OHT_DETECT_NORMAL_VALUE) &&
                    (OHT_Detect_AreaSetting != OHT_DETECT_HILL_VALUE)) {
                    sensor_vel_level_pps = VELOCITY_0_15_MPS;
                } else {
                    sensor_vel_level_pps = Model_SPEED_GRADE_4_VELOCITY;
                }
            } else {
                SEP_LOGT("set to Model_SPEED_GRADE_1_VELOCITY by obs_vel_level");
                sensor_vel_level_pps = Model_SPEED_GRADE_1_VELOCITY;
                AMC_Follow_Control_Status = STATUS_PBS_STOP;
            }
        }
    
        // sensor_vel_level_pps = oht_vel_level;

        if(AMC_Pause_Control == true) {
            sensor_vel_level_pps = Model_SPEED_GRADE_1_VELOCITY;
			AMC_Follow_Control_Status = STATUS_OHT_PAUSED;
        }

        //마킹 이후 장애물이 detect되면 추가로 앞으로 가는 부분 수정함
        if (front_wheel->getMotionState() == Motion::IDLE) {
            drv_cmd.FollowsVEL = 0;
            drv_cmd.TargetVEL = 0;
        }

		auto &FollowsVEL = drv_cmd.FollowsVEL;
        auto &TargetVEL = drv_cmd.TargetVEL;


        if (equal_to(sensor_vel_level_pps, Model_SPEED_GRADE_8_VELOCITY)) {
            sensor_vel_level_pps = FollowsVEL;
        }


#if 0 ///// for debug [[[ //////
        static unsigned counter = 0;
        if (counter++ % 100 == 0) {
            SEP_LOGT(
                    "obs_sen=%2x oht_sen=%2x TargetVEL = %.1f, FollowVEL=%.1f, obs_vel_level=%.1f oht_vel_level=%.1f sensor_vel =%.1f",
                    obs_sensor_val, oht_sensor_val, TargetVEL, FollowsVEL, obs_vel_level, oht_vel_level, sensor_vel_level_pps);
        }
        return;
#endif ///// for debug ]]]  //////

    
        bool reschedule_required(false);

        auto prev_acc_state = acc_state_;
        if (TargetVEL == FollowsVEL) 
        {
            if ((sensor_vel_level_pps < TargetVEL) || (sensor_vel_level_pps == TargetVEL)) 
            {
                reschedule_required = true;
                TargetVEL = sensor_vel_level_pps;
                acc_state_ = ACC_S1;
            }
            else 
            {
                acc_state_ = ACC_S0;
            }
        } 
        else 
        {
            if (acc_state_ == ACC_S1) 
            {
                if (sensor_vel_level_pps > TargetVEL) 
                {
                    TargetVEL = front_wheel->getVelPulse();
                    reschedule_required = true;
                    acc_state_ = ACC_S2;
                }
                else if (sensor_vel_level_pps < TargetVEL)
                {
                    TargetVEL = sensor_vel_level_pps;
                    reschedule_required = true;
                }
            }
            if (acc_state_ == ACC_S2)
            {
                if (sensor_vel_level_pps > TargetVEL)
                {
                    TargetVEL = (sensor_vel_level_pps >= FollowsVEL) ? FollowsVEL : sensor_vel_level_pps;
                    reschedule_required = true;
                    acc_state_ =  (TargetVEL == FollowsVEL) ? ACC_S0 : ACC_S2;
                } 
                else if (sensor_vel_level_pps < TargetVEL)
                {
                    reschedule_required = true;
                    TargetVEL = sensor_vel_level_pps;
                    acc_state_ = ACC_S1;
                }
            }
        }
        printf("acc_state_ : %d, prev_acc_state : %d, reschedule_required : %d, sensor_vel_level_pps : %f, TargetVEL : %f\n", acc_state_, prev_acc_state, reschedule_required, sensor_vel_level_pps, TargetVEL);
       /*
       if ( acc_state_ != prev_acc_state) 
        { // DebugState::get(DebugState::OHT_DISTANCE_MM) < 400
            SEP_LOGT("ACC S%d -> S%d, obs_sen=%2x oht_sen=%2x TargetVEL = %.1f FollowVEL=%.1f obs_vel_level=%.1f oht_vel_level=%.1f sensor_vel =%.1f",
                     prev_acc_state, acc_state_, obs_sensor_val, oht_sensor_val,
                    TargetVEL, FollowsVEL, obs_vel_level, oht_vel_level, sensor_vel_level_pps);

            if (reschedule_required) 
            {
                if(TargetVEL == 0) 
                {
                   // front_wheel->stopPulse(drv_cmd.TargetDCC, false);
                   printf("follows control move stop %f\n", Model_MOVE_S_DCC_BY_SENSOR_NORMAL);
				   front_wheel->stopPulse(Model_MOVE_S_DCC_BY_SENSOR_NORMAL, false);
                } 
				else
                {
                    printf("follws control move ds\n");
					front_wheel->moveToPulse(drv_cmd.TargetPOS_abs, TargetVEL, drv_cmd.TargetACC, drv_cmd.TargetDCC);
                }
            }
        }
        */
        if(reschedule_required)
        {
            if(oht_sensor_val == OHT_SPEED_GRADE_4_BIT)
            {
				front_wheel->stopPulse(Model_MOVE_S_DCC_BY_SENSOR_NORMAL, false);
            }
        }

    }

    //void Amc4OHT::SetDetectorOutput(int OHTSettingArea, int OBSSettingArea, char OHTDetectType, char OBSDetectType) {
    void Amc4OHT::SetDetectorOutput(int OHTSettingArea, int OBSSettingArea) {
        //OHT_Detect_AreaSetting_For_Curve = OHTSettingArea;
        //OBS_Detect_AreaSetting = OBSSettingArea;

        std::array<int,2> detector_flags = {
                // 비트 반전된 AreaSetting 변수를 보는게 아니라, Raw Data를 Map 기준으로 반전시킴
                // 이로 인해 내부에서 변경된 패턴을 넣어줘도 Main의 패턴만 보고 변경
                // 추가로 다른 패턴이 아래에서 덮어씌워지며 15(1111)이 출력
                //getDetectorSetting(oht_setting_map, OHTDetectType, OHT_DETECT_NO_USE),
                //getDetectorSetting(obs_setting_map, OBSDetectType, HOKUYO_OBS_DETECT_NO_USE)};
                //OHT_Detect_AreaSetting_For_Curve, OBS_Detect_AreaSetting};  // 230202 yjchoe
                OHTSettingArea, OBSSettingArea};
        std::array<unsigned,4> sel_ports[2] = {
                {ioconfig_.getportid("_ohtdetect_area1_sel"),
                 ioconfig_.getportid("_ohtdetect_area2_sel"),
                 ioconfig_.getportid("_ohtdetect_area3_sel"),
                 ioconfig_.getportid("_ohtdetect_area4_sel")},
                {ioconfig_.getportid("_obstacle_area1_sel"),
                 ioconfig_.getportid("_obstacle_area2_sel"),
                 ioconfig_.getportid("_obstacle_area3_sel"),
                 ioconfig_.getportid("_obstacle_area4_sel")}
        };
        // 36 - Obstacle_Area1_Select
        // 37 - Obstacle_Area2_Select
        // 38 - Obstacle_Area3_Select
        // 39 - Obstacle_Area4_Select
        // 42 - OHT_Detect_Area1_Select
        // 43 - OHT_Detect_Area2_Select
        // 44 - OHT_Detect_Area3_Select
        // 45 - OHT_Detect_Area4_Select

        //std::array<int*,2> area_setting = {&OHT_Detect_AreaSetting_For_Curve, &OBS_Detect_AreaSetting};
        for(int i : {0,1}) {
            for (int j : {0, 1, 2, 3}) {
                writeRawDo(sel_ports[i][j], detector_flags[i] & (1 << j));
                //*area_setting[i] |= detector_flags[i] & (1 << j);           // OHT/OBS_Detect_AreaSetting 변수에 덮어씀
            }
        }
    }

    void filter::DetectFilter(unsigned new_val, unsigned &filter_val, unsigned count_limit) {
        new_val = ~new_val & 0xf;
        if (new_val != temp_val) {
            temp_val = new_val;
            cnt = 0;
        }
        else {
            if (++cnt >= count_limit) {
                cnt = 0;
                filter_val = temp_val;
            }
        }
    }
}
