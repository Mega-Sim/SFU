#ifndef ErrorCodeH
#define ErrorCodeH
// ---------------------------------------------------------------------------
// ********************************************
// 주의 사항
// 1. 에러코드의 Col(열번호) 위치가 일정할 것(임의로 변경 금지)
// 2. 에러코드의 name이 너무 길지 않도록 주의 할 것 
// 2. 되도록 오름차순으로 정렬할 것
// 3. 엑셀 파일에서 텍스트 나누기가 될 수 있도록 신경써서 작성할 것
// ********************************************

#define ERR_LAYOUT_FORMAT                   1
#define ERR_LAYOUT_OHT_LIMIT                2
#define ERR_LAYOUT_PATH_LIMIT               3
#define ERR_LAYOUT_NODE_LIMIT               4
#define ERR_PATH_FORMAT                     6
#define ERR_PATH_ID_LIMIT                   7
#define ERR_PATH_NPATH_FORMAT               8
#define ERR_RFPIO_ID_LIMIT                  9
#define ERR_RFPIO_CS_LIMIT                  10
#define ERR_PATH_DIR_LIMIT                  11
#define ERR_PATH_ALIGN_LIMIT                12
#define ERR_PATH_SPEED_LIMIT                14
#define ERR_CURVE_FORMAT                    15
#define ERR_PATH_START_END_NODE             17
#define ERR_NODE_FORMAT                     28
#define ERR_NODE_ID_LIMIT                   29
#define ERR_NODE_NX_LIMIT                   30
#define ERR_NODE_NY_LIMIT                   31
#define ERR_NODE_STOP_LIMIT                 32
#define ERR_NODE_TRSOUND_LIMIT              36
#define ERR_NODE_OSCILLATION_LIMIT          38
#define ERR_NODE_LOWACCEL_LIMIT             40
#define ERR_NODE_STEER_CHANGE_LIMIT         42
#define ERR_NODE_F_ENDNODE_LIMIT            43
#define ERR_NODE_F_ENDNODE1_LIMIT           44
#define ERR_NODE_F_ENDNODE2_LIMIT           45
#define ERR_NODE_R_ENDNODE_LIMIT            46
#define ERR_NODE_R_ENDNODE1_LIMIT           47
#define ERR_NODE_R_ENDNODE2_LIMIT           48
#define ERR_RFCHANNEL_LIMIT                 49
#define ERR_OBSNODE_NOTCONNECT_NODE         72
#define ERR_PATHDIRECTION                   73
#define ERR_COLLISIONINFO_LIMIT             74


// euv oht error code add
#define ERR_SHIFT_MOVE_TIME_OUT				80
#define ERR_ROTATE_MOVE_TIME_OUT			81



// *** window errror **
#define ERR_WINDOWEXCEPTION                 110

#define ERR_READ_NODE_DISABLE_FILE      	118
#define ERR_READ_PASS_BOX_NODE_FILE      	119

#define ERR_NODE_DISABLE                    120
#define ERR_PATHSEARCH_FAIL                 140
#define ERR_TGNODE_NOT_REALNODE             142
#define ERR_PATHSEARCH_NOT_COUNT            143

#define ERR_ERRCODE_NOTCLEAR                151
#define ERR_CURRNODE_ZERO                   152
#define ERR_CURRNODE_INVALID                153
#define ERR_CURRNODE_NOTREALNODE            154
#define ERR_DRIVING_NOTORIGIN               156
#define ERR_HOIST_NOTORIGIN                 157
#define ERR_SHIFT_NOTORIGIN                 158
#define ERR_ROTATE_NOTORIGIN                159

#define ERR_STEERING_DIFFSTATUS             160
#define ERR_SHUTTER_DIFFSTATUS              161

#define ERR_AUTO_DATAUPLOAD                 162
#define ERR_SHIFT_HOMESEN_NOTON             164
#define ERR_SHIFT_HOME_NOT_POSITIONERROR    165


// ---------------------------------------------------------------------------
// PMAC Abs Reading Error
#define ERR_AMC_VERSION_ERROR               199

// ---------------------------------------------------------------------------
// AMC Error
#define ERR_AMC_BUSY_PROFILE_GEN_MOVEX      209     ///<  move_x 함수에서 프로파일을 생성하는 와중에 기존에 있는 큐가 동작하는 에러를 방지하기 위함.
#define ERR_AMC_ILLEGAL_PARAMETER           210     ///<  move with zero accel or velocity"
#define ERR_AMC_NO_MAP                      211     ///<  The map_axes(...) funcation has not been called"
#define ERR_AMC_AMP_FAULT                   212     ///<  amp fault occured"
#define ERR_AMC_ON_MOTION                   213     ///<  Motion is not completed"
#define ERR_AMC_NON_EXIST                   214     ///<  AMC Board is not exist"
#define ERR_AMC_BOOT_OPEN_ERROR             215     ///<  AMC Boot File Read/Write Error"
#define ERR_AMC_CHKSUM_OPEN_ERROR           216     ///<  AMC Checksum File Read/Write Error"
#define ERR_AMC_WINNT_DRIVER_OPEN_ERROR     217     ///<  AMC Windows NT Driver Open Error"
#define ERR_AMC_ILLEGAL_PARAM_MOVE_DS       218     ///<  Move_ds명령시 거리 부족시 Error
#define ERR_AMC_FUNC_ERR                    219     ///<  Function Error"
#define ERR_AMC_NOTDEFINED                  220     ///<  정의되지 않은 에러
#define ERR_AMC_RCVFORMAT                   221
#define ERR_AMC_ESTOP                       222     ///< ->절대 에러코드 바꾸면 안됨
#define ERR_AMC_INIT                        223
#define ERR_AMC_GETRESPONSE                 224
#define ERR_AMC_NOTOPEN                     225
#define ERR_AMC_INVALID_RESPONSE            226
#define ERR_AMC_COMM                        227
#define ERR_AMC_NOT_INITIALIZED             228     ///<  be sure to call mmc_init(...)"
#define ERR_AMC_TIMEOUT_ERR                 229     ///<  DPRAM Communication Error"
#define ERR_AMC_INVALID_AXIS                230     ///<  axis out of range or not configured"
#define ERR_AMC_ILLEGAL_ANALOG              231     ///<  analog channel < 0 or > 4"
#define ERR_AMC_ILLEGAL_IO                  232     ///<  illegal I/O port"
#define ERR_AMC_RCVFORMAT1                  233
#define ERR_AMC_RCVFORMAT2                  234
#define ERR_AMC_RCVFORMAT3                  235
#define ERR_AMC_RCVFORMAT4                  236
#define ERR_AMC_RCVFORMAT5                  237
#define ERR_AMC_RCVFORMAT6                  238
#define ERR_AMC_WATCHDOG_STOP               239

// Serial Dll Error
#define ERR_SERIAL_DLL_INIT                 240
#define ERR_SERIAL_DLL_FILE_NAME            241
#define ERR_SERIAL_DLL_FILE_NO_EXIST        242

// Mark Error
#define ERR_MARK_COMMAND                    245
#define ERR_MARK_FAIL_DRIVING		        246
#define ERR_MARK_FAIL_TRANS		            247
#define ERR_MARK_FAIL                       248
#define ERR_MARK_FSTOPSEN_OFF               249
#define ERR_MARK_RSTOPSEN_OFF               250

#define ERR_MARK_AMC_FSTOPSEN_OFF           251     ///<  H Command중 문제발생
#define ERR_MARK_AMC_RSTOPSEN_OFF           252     ///<  H Command중 문제발생
#define ERR_MARK_AMC_FAIL_REAR              253     ///<  H Command중 문제발생시 AMC에서 올려주는 에러.."

// Map Make Error
#define ERR_MAPMAKE_COMMAND                 255
#define ERR_START_STOPSEN_ON                256
#define ERR_MAPMAKE_TAGSTOP_TIMEOVER        257
#define ERR_MAPMAKE_NOTFOUNDNODEID          258
#define ERR_MAPMAKE_DATA_FORMAT             259

// HID 에러추가
#define ERR_EBOOST_EST_PEAKVOLTAGE          260     ///<  385V 이상 (24)
#define ERR_EBOOST_EST_OVERVOLTAGE          261     ///<  380V 이상 (25)
#define ERR_EBOOST_EST_UNDERVOLTAGE         262     ///<  100V 이하 (26)
#define ERR_EBOOST_EST_LOWVOLTAGE           263     ///<   75V 이하 (27)
#define ERR_EREC_OVERVOLTAGE_RUN            264     ///<  355V 이상 (28)
#define ERR_EREC_PEAKVOLTAGE_RUN            265     ///<  360V 이상 (29)
#define ERR_ECAP_OVERVOLTAGE_RUN            266     ///<  355V 이상 (30)
#define ERR_ECAP_PEAKVOLTAGE_RUN            267     ///<  360V 이상 (31)
#define ERR_EBOOST_OVERVOLTAGE_RUN          268     ///<  355V 이상 (32)
#define ERR_EBOOST_PEAKVOLTAGE_RUN          269     ///<  360V 이상 (33)
#define ERR_UCAP_CABLE_OPEN1                270     ///<  UCAP Open (34)
#define ERR_UCAP_CABLE_OPEN2                271     ///<  UCAP Open Spare(35)

// PMAC Error
#define ERR_NOTMATCHTGNODE                  291
#define ERR_FSTOPSEN_NOTON                  292
#define ERR_NOTFIND_NODE                    293
#define ERR_STEER_TIMEOVER_NPATH            294
#define ERR_FINALNODE_STOPCMDEXEC           295
#define ERR_F_R_STOPSEN_OFFED               296

#define ERR_NOTMATCH_TAGLENGTH              305
#define ERR_NOTMATCH_TAGLENGTH_FINALNODE    306
#define ERR_STEER_FR_LEFTNOTON              309
#define ERR_STEER_FR_RIGHTNOTON             310
#define ERR_STEER_F_LEFTNOTON               311
#define ERR_STEER_F_RIGHTNOTON              312
#define ERR_STEER_R_LEFTNOTON               313
#define ERR_STEER_R_RIGHTNOTON              314
#define ERR_STEER_NOTSAMESTATUS             315
#define ERR_STEER_CMDEXEC                   316
#define ERR_TARGETDIST                      317
#define ERR_AG_NOTCHANGEDNODE               320
#define ERR_AV_TIMEOVER                     321
#define ERR_DRVTHREAD_SUSPENDED             322
#define ERR_OBS_TIMEOVER                    323
#define ERR_MAKE_COMMAD_ID_INVALID          326
#define ERR_STOPSPEED_CONTROL               328
#define ERR_RUNSPEED_CONTROL                329
#define ERR_WINDOW_REBOOT_FAIL              330
#define ERR_STOP_NOTCHANGESEN               334
#define ERR_CROSSRAIL_OBSTACLE_DETECT       337
#define ERR_AMC_SENDCMDFAIL                 341
#define ERR_PBS_COMMAND                     342
#define ERR_AG_BACKWORD_MOVE                343

// ---------------------------------------------------------------------------
// ** Coolmuscle(Roatae) Error ****
#define ERR_ROTATE_OPEN                     370
#define ERR_ROTATE_NOT_CONNECTED            371
#define ERR_ROTATE_WRITE                    372
#define ERR_ROTATE_NO_RESPONSE              373
#define ERR_ROTATE_INCORRECT_RESPONSE       374
#define ERR_ROTATE_EXEC_TIMEOVER            375

// *** BCR Error ***
#define ERR_BCR_OPEN                        400
#define ERR_BCR_READTIMEOUT                 402
#define ERR_BCR_WRITE                       403
#define ERR_BCR_DATAFORMAT                  404
#define ERR_BCR_READ                        405
#define ERR_BCR_BACK_READTIMEOUT            406
#define ERR_BCR_READ_TRANS            		407

//** RFPIO Error ****
//#define  ERR_RFPIO_OPEN                         415
#define  ERR_RFPIO_WRITE                        416
#define  ERR_RFPIO_READTIMEOUT                  417
#define  ERR_RFPIO_ID_DATAFORMAT                418
#define  ERR_RFPIO_CHA_DATAFORMAT               419

// *** HID Error ***
#define ERR_HID_OPEN                        420
#define ERR_HID_WRITE                       421
#define ERR_HID_READTIMEOUT                 422
#define RERTY_HID_SEND_DATA					433

// *** IPC Temperature Error ***
#define ERR_IPC_OVERTEMPERATURE             458

// *** Pickup Temperature Error ***
#define ERR_TEMPER_OVERTEMPERATURE          456

// ---------------------------------------------------------------------------
// Key Change

#define ERR_CMD_CANCEL                      460
#define ERR_KEY_A2M                         461
#define ERR_TP_ESTOP                        462
#define ERR_OCS_ESTOP                       463
#define ERR_BUMPER_PRESS                    464
#define ERR_ESTOP_PRESS                     465
#define ERR_HANDPIO_GO_OFF                  470

#define ERR_OCS_ESTOP1                      471     ///< OCS 경로에서 벗어난 위치로 OHT가 주행하는 경우 (탈선)
#define ERR_OCS_ESTOP2                      472     ///< 충돌구간에서 타 OHT가 갑자기 인식되는 경우
#define ERR_OCS_ESTOP3                      473     ///< Unload_Assign 상태에서 Carrier가 있는 경우
#define ERR_OCS_ESTOP4                      474     ///< Load_Assign 상태에서 Carrier가 없는 경우
#define ERR_OCS_ESTOP5                      475     ///< STK 포트에서 Unloading 중 RF Data와 반송명령의 Carrier가 틀린 경우

// AMC Error 2
#define ERR_MMC_FILTERING_ERROR             497
#define ERR_MMC_MUTEXLOCK_ERROR             498
#define ERR_MMC_MOVE_LASTDS                 499     ///<  move_ds"  _n  _p  _s 명령 실행 중 마지막번째 이동중에는 앞의 명령이 오더라도 프로파일을 생성 안함

// Servo Alarm
#define ERR_SERVO_DRIVING_ALARM             505
#define ERR_SERVO_DRIVING_F_ALARM           521
#define ERR_SERVO_DRIVING_R_ALARM           522

#define ERR_SERVO_HOIST_ALARM               523
#define ERR_SERVO_SHIFT_ALARM               524
#define ERR_SERVO_ROTATE_ALARM              525

// ** Trans / Scan Error **
#define ERR_TRANS_NODE_DIFF                 550

// AMC Servo/Motor Error
#define ERR_AXIS1_F_HOME_SWITCH             601     ///<  원점(home) 센서 감지
#define ERR_AXIS1_R_HOME_SWITCH             602
#define ERR_AXIS2_HOME_SWITCH               603
#define ERR_AXIS3_HOME_SWITCH               604

#define ERR_AXIS1_F_POS_LIMIT               605     ///<  + 방향 Limit 센서 감지
#define ERR_AXIS1_R_POS_LIMIT               606
#define ERR_AXIS2_POS_LIMIT                 607
#define ERR_AXIS3_POS_LIMIT                 608

#define ERR_AXIS1_F_NEG_LIMIT               609     ///<  - 방향 Limit 센서 감지
#define ERR_AXIS1_R_NEG_LIMIT               610
#define ERR_AXIS2_NEG_LIMIT                 611
#define ERR_AXIS3_NEG_LIMIT                 612

#define ERR_AXIS1_F_AMP_FAULT               613     ///<  AMP Fault 상태 발생
#define ERR_AXIS1_R_AMP_FAULT               614
#define ERR_AXIS2_AMP_FAULT                 615
#define ERR_AXIS3_AMP_FAULT                 616

#define ERR_AXIS1_F_V_LIMIT                 621     ///<  현재속도가 속도 limit보다 클때 발생
#define ERR_AXIS1_R_V_LIMIT                 622     ///<  현재속도가 속도 limit보다 클때 발생
#define ERR_AXIS2_V_LIMIT                   623     ///<  현재속도가 속도 limit보다 클때 발생
#define ERR_AXIS3_V_LIMIT                   624     ///<  현재속도가 속도 limit보다 클때 발생

#define ERR_AXIS1_F_X_NEG_LIMIT             625     ///<  - Position limit를 벗어날때 발생
#define ERR_AXIS1_R_X_NEG_LIMIT             626     ///<  - Position limit를 벗어날때 발생
#define ERR_AXIS2_X_NEG_LIMIT               627     ///<  - Position limit를 벗어날때 발생
#define ERR_AXIS3_X_NEG_LIMIT               628     ///<  - Position limit를 벗어날때 발생

#define ERR_AXIS1_F_X_POS_LIMIT             629     ///<  + Position limit를 벗어날때 발생
#define ERR_AXIS1_R_X_POS_LIMIT             630     ///<  + Position limit를 벗어날때 발생
#define ERR_AXIS2_X_POS_LIMIT               631     ///<  + Position limit를 벗어날때 발생
#define ERR_AXIS3_X_POS_LIMIT               632     ///<  + Position limit를 벗어날때 발생

#define ERR_AXIS1_F_ERROR_LIMIT             633     ///<  위치오차가 Limit 값보다 클때 발생
#define ERR_AXIS1_R_ERROR_LIMIT             634     ///<  위치오차가 Limit 값보다 클때 발생
#define ERR_AXIS2_ERROR_LIMIT               635     ///<  위치오차가 Limit 값보다 클때 발생
#define ERR_AXIS3_ERROR_LIMIT               636     ///<  위치오차가 Limit 값보다 클때 발생

#define ERR_AXIS1_F_PC_COMMAND              637     ///<  EVENT가 발생된 상태일때 발생
#define ERR_AXIS1_R_PC_COMMAND              638     ///<  EVENT가 발생된 상태일때 발생
#define ERR_AXIS2_PC_COMMAND                639     ///<  EVENT가 발생된 상태일때 발생
#define ERR_AXIS3_PC_COMMAND                640     ///<  EVENT가 발생된 상태일때 발생

#define ERR_AXIS1_F_OUT_OF_FRAMES           641     ///<  Frame buffer가 Full 상태일때 발생
#define ERR_AXIS1_R_OUT_OF_FRAMES           642     ///<  Frame buffer가 Full 상태일때 발생
#define ERR_AXIS2_OUT_OF_FRAMES             643     ///<  Frame buffer가 Full 상태일때 발생
#define ERR_AXIS3_OUT_OF_FRAMES             644     ///<  Frame buffer가 Full 상태일때 발생

#define ERR_AXIS1_F_ABS_COMM_ERROR          649     ///<  ABS encoder 통신 Error시 발생
#define ERR_AXIS1_R_ABS_COMM_ERROR          650     ///<  ABS encoder 통신 Error시 발생
#define ERR_AXIS2_ABS_COMM_ERROR            651     ///<  ABS encoder 통신 Error시 발생
#define ERR_AXIS3_ABS_COMM_ERROR            652     ///<  ABS encoder 통신 Error시 발생

#define ERR_AXIS1_F_INPOSITION_STATUS       653     ///<  In position signal 입력 여부
#define ERR_AXIS1_R_INPOSITION_STATUS       654     ///<  In position signal 입력 여부
#define ERR_AXIS2_INPOSITION_STATUS         655     ///<  In position signal 입력 여부
#define ERR_AXIS3_INPOSITION_STATUS         656     ///<  In position signal 입력 여부

#define ERR_AXIS1_F_COLLISION_STATE         661     ///<  충돌방지 Error
#define ERR_AXIS1_R_COLLISION_STATE         662     ///<  충돌방지 Error
#define ERR_AXIS2_COLLISION_STATE           663     ///<  충돌방지 Error
#define ERR_AXIS3_COLLISION_STATE           664     ///<  충돌방지 Error

#define ERR_AXIS1_F_STOP_EVENT              665
#define ERR_AXIS1_R_STOP_EVENT              666
#define ERR_AXIS2_STOP_EVENT                667
#define ERR_AXIS3_STOP_EVENT                668

#define ERR_AXIS1_F_ESTOP_EVENT             669
#define ERR_AXIS1_R_ESTOP_EVENT             670
#define ERR_AXIS2_ESTOP_EVENT               671
#define ERR_AXIS3_ESTOP_EVENT               672

#define ERR_AXIS1_F_ABORT_EVENT             673
#define ERR_AXIS1_R_ABORT_EVENT             674
#define ERR_AXIS2_ABORT_EVENT               675
#define ERR_AXIS3_ABORT_EVENT               676

// OHT Obstacle Sensor Control (RS232)
#define ERR_OBS_SENSOR_FAIL                 693

// OHT Detect Sensor Control (RS232)
#define ERR_DETECT_SENSOR_FAIL              694
#define ERR_UBG_OPEN						695
#define ERR_UBG_NO_DATA						696
#define ERR_UBG_WRITE						697

// SHUTTER ERROR
#define ERR_SHUTTER_CANTMOVE_HAND_OPEN      698
#define ERR_SHUTTER_NOTORIGIN               699
#define ERR_F_SHUTTER_ALARM                 700
#define ERR_R_SHUTTER_ALARM                 701
#define ERR_SHUTTER_NOT_CLOSE               719
#define ERR_SEND_SHUTTER_OPEN_COMMAND       720
#define ERR_SEND_SHUTTER_CLOSE_COMMAND      721
#define ERR_SEND_SHUTTER_RESET_COMMAND      722
#define ERR_SEND_SHUTTER_ORIGIN_COMMAND     723
#define ERR_SHUTTER_OPEN_EXEC_TIMEOVER      724
#define ERR_SHUTTER_CLOSE_EXEC_TIMEOVER     725
#define ERR_SHUTTER_PWRRST_EXEC_TIMEOVER    727
#define ERR_SHR_F_NO_SEN_AND_R_NO_SEN       728
#define ERR_SHR_F_NO_SEN_AND_R_CLOSE_POS    729
#define ERR_SHR_F_CLOSE_POS_AND_R_NO_SEN    730
#define ERR_SHR_F_NO_SEN_AND_R_OPEN_POS     731
#define ERR_SHR_F_CLOSE_POS_AND_R_OPEN_POS  732
#define ERR_SHR_F_OPEN_POS_AND_R_NO_SEN     733
#define ERR_SHR_F_OPEN_POS_AND_R_CLOSE_POS  734
#define ERR_SHR_BOTH_POS_SENING             735
#define ERR_SHR_MISMATCH_FOUP_SENSING       736
#define ERR_SHUTTER_AUTO_INIT_ABNORMAL_CASE 737
#define ERR_SHR_F_CLOSE_POS_AND_R_CLOSE_POS 738
#define ERR_SHR_F_OPEN_POS_AND_R_OPEN_POS   739

// added
#define ERR_SHR_F_NO_SEN_AND_R_MIDDLE_CLOSE_POS    		  	740
#define ERR_SHR_F_MIDDLE_CLOSE_POS_AND_R_NO_SEN           	741
#define ERR_SHR_F_MIDDLE_CLOSE_POS_AND_R_MIDDLE_CLOSE_POS 	742
#define ERR_SHR_F_MIDDLE_CLOSE_POS_AND_R_OPEN_POS  			743
#define ERR_SHR_F_OPEN_POS_AND_R_MIDDLE_CLOSE_POS  			744
#define ERR_SHR_NOT_MIDDLE_CLOSE            754
#define ERR_SHR_MISMATCH_EUVSEN   	    	755
#define ERR_EUVROHT_DIFF_EUVSENSINGS        756
#define ERR_EUVROHT_EUVOFF_MIDCLOSE         757
#define ERR_EUVROHT_EUVON_FULLCLOSE         758



// STEERING ERROR
#define ERR_SEND_STEER_LEFT_COMMAND         760
#define ERR_SEND_STEER_RIGHT_COMMAND        761
#define ERR_STEER_LEFT_EXEC_TIMEOVER        764
#define ERR_STEER_RIGHT_EXEC_TIMEOVER       765
#define ERR_STEER_ORIGIN_EXEC_TIMEOVER      767
#define ERR_STR_F_NO_SEN_AND_R_NO_SEN       768
#define ERR_STR_F_NO_SEN_AND_R_RIGHT_POS    769
#define ERR_STR_F_RIGHT_POS_AND_R_NO_SEN    770
#define ERR_STR_F_NO_SEN_AND_R_LEFT_POS     771
#define ERR_F_RIGHT_POS_AND_R_LEFT_POS      772
#define ERR_F_LEFT_POS_AND_R_NO_SEN         773
#define ERR_F_LEFT_POS_AND_R_RIGHT_POS      774
#define ERR_STR_BOTH_POS_SENING             775
#define ERR_STR_F_RIGHT_POS_AND_R_RIGHT_POS 776
#define ERR_STR_F_LEFT_POS_AND_R_LEFT_POS   777

// ------------------------------------------------------------------------------
// *** IO Control Error ***
#define ERR_HANDPIO_GOSIG_NOT_ON            780

// -------------------------------------------------------------
// **** Location Error *****
// --- EQ Error ----
#define ERR_DUPLICATE_LOCATION              801
#define ERR_LOCATION_FORMAT                 802
#define ERR_LOCATION_NOTFOUND               803
#define ERR_INVALID_NODEID                  804
#define ERR_LOCFILE_NOTEXIST                805
#define ERR_LOCATION_LOAD_FAIL              806
#define ERR_MAPDATA_LOAD_FAIL               807
#define ERR_PARADATA_LOAD_FAIL              810
#define ERR_ASSISTANT_EXEC_FAIL             811
#define ERR_ASSISTANT_CLOSE_FAIL            812
#define ERR_DUPLICATE_EQLOCATION            813
#define ERR_DUPLICATE_UTBLOCATION           815
#define ERR_EQLOCATION_FORMAT               816
#define ERR_UTBLOCATION_FORMAT              818
#define ERR_DUPLICATE_STB_L_LOCATION        819
#define ERR_DUPLICATE_STB_R_LOCATION        820
#define ERR_STB_L_LOCATION_FORMAT           821
#define ERR_STB_R_LOCATION_FORMAT           822
#define ERR_DUPLICATE_EQ_X_LOCATION         823
#define ERR_EQ_X_LOCATION_FORMAT            824

//STB 및 UTB Error---
#define ERR_STB_FOUP_CHECK_EXIST            830
#define ERR_STB_FOUP_CHECK_NOEXIST          831
#define ERR_UTB_FOUP_CHECK_EXIST            832
#define ERR_UTB_FOUP_CHECK_NOEXIST          833
// --- STB Error ----
#define ERR_STB_FOUP_EXIST                  850
#define ERR_STB_FOUP_NOTEXIST               851
#define ERR_STBSEN_NOT_OFF                  853

// --- UTB Error ----
#define ERR_UTB_FOUP_EXIST                  855
#define ERR_UTB_FOUP_NOTEXIST               856
#define ERR_UTBSEN_NOT_OFF                  857


#define ERR_UTB_X_FOUP_EXIST                841
#define ERR_UTB_X_FOUP_NOTEXIST             842
#define ERR_UTB_X_SEN_NOT_OFF               843

// -- Inner Foup Detect --
#define ERR_INNER_FOUP_NOT_DETECT           854
#define ERR_INNER_FOUP_DETECT               858
#if(OHT_NAME==OHT_NAME_P4_MASK)
#define ERR_INNER_CARRIER_NOT_DETECT        859		//mgchoi. 2022.08.23. P4 MaskOHT용
#define ERR_INNER_CARRIER_DETECT            860
#endif
#define ERR_RAILSUPPORT_SEND_MOVE_WORK_CMD  861
#define ERR_RAILSUPPORT_SEND_MOVE_HOME_CMD  862
#define ERR_RAILSUPPORT_MOVE_WORK_TIMEOVER  863
#define ERR_RAILSUPPORT_MOVE_HOME_TIMEOVER  864
#define ERR_RAILSUPPORT_NOT_WORK_IN_TRANS   865
#define ERR_RAILSUPPORT_NOT_READY           866
#define ERR_SEN_ABNORMAL_RAILSUPPORT        868

// -- Rotate Error --
#define  ERR_ROTATE_0_TIMEOVER                  870
#define  ERR_ROTATE_90_TIMEOVER                 871
#define  ERR_ROTATE_180_TIMEOVER                872
#define  ERR_ROTATE_270_TIMEOVER                873
#define  ERR_ROTATE_ORIGIN_TIMEOVER             874
#define  ERR_ROTATE_LIMIT_ON_ERR                875
#define  ERR_ROTATE_MOVE_CONDITION              876
#define  ERR_ROTATE_ALARM                       877
#define  ERR_ROTATE_NOT_90	                    878
#define  ERR_SEND_ROTATE_0_COMMAND              879
#define	 ERR_SEND_ROTATE_90_COMMAND				880
#define	 ERR_SEND_ROTATE_180_COMMAND			881
#define	 ERR_SEND_ROTATE_270_COMMAND			882
#define	 ERR_SEND_ROTATE_ORIGIN_COMMAND			883
#define	 ERR_ROTATE_FREE_STATUS                 884
#define  ERR_ROTATE_LIMIT_OFF_ERR               885
#define	 ERR_ROTATE_MOVE_TIMEOVER_IN_DRIVING	886
#define  ERR_ROTATE_NOT_0	                    887
#define  ERR_ROTATE_NOT_INPOSITION              888

// --- RF_PIO ERROR ---
#define ERR_RFPIO_OPEN						890
#define ERR_RFID_NODATA                     891
#define ERR_RFCHANNEL_NODATA                892
#define ERR_PIO_SETTINGTIMEOVER             893
#define ERR_L_REQ_CONT_OFF					894
#define ERR_U_REQ_CONT_OFF					895
#define ERR_ABORT_ON_ERROR					898
#define ERR_ABORT_NOT_OFF					899
#define ERR_L_REQ_CONT_ON                   901
#define ERR_U_REQ_CONT_ON                   902
#define ERR_HO_AVBL_OFFED1                  903
#define ERR_HO_AVBL_OFFED2                  904
#define ERR_HO_AVBL_OFFED3                  905
#define ERR_L_REQ_ABNORMAL_ON               906
#define ERR_U_REQ_ABNORMAL_ON               907
#define ERR_L_REQ_NOT_ON                    908
#define ERR_L_REQ_NOT_OFF                   909
#define ERR_U_REQ_NOT_ON                    910
#define ERR_U_REQ_NOT_OFF                   911
#define ERR_READY_NOT_ON                    912
#define ERR_READY_NOT_OFF                   913
#define ERR_READY_CONT_ON                   914
#define ERR_ES_OFF_ERROR                    915
#define ERR_HO_AVBL_OFFED                   916

// HAND OPERATION
#define ERR_HAND_PUSHSEN_NOTON              920
#define ERR_HAND_OPEN_TIMEOVER              921
#define ERR_HAND_CLOSE_TIMEOVER             922
#define ERR_HAND_ORIGIN_TIMEOVER            923
#define ERR_HAND_SENSOR_OFFERR              925
#define ERR_HAND_RESET_FAIL                 927
#define ERR_HAND_NOTORIGIN                  929
#define ERR_HAND_INPOSITION_ERROR           931

#define ERR_HAND_CAST_NOTEXIST              932
#define ERR_HAND_CAST_EXIST                 933

#define ERR_HAND_ALARM                      934

#define ERR_HAND_PUSHSEN_ABNORMAL_ON        936
#define ERR_SEND_HAND_OPEN_COMMAND          937
#define ERR_SEND_HAND_CLOSE_COMMAND         938
#define ERR_SEND_HAND_ORIGIN_COMMAND        939

#define ERR_LOOKDOWN_DETECT                 941
#define ERR_LOOKDOWN_SENSOR_FAIL            942
#define ERR_HOISTALIGNSEN_NOT_ON            943
#define ERR_OSCILLATION_ERROR               944
#define ERR_GRIP_SEN_MISMATCH               946
#define ERR_TRANS_AXIS2HOME_TIMEOVER        947
#define ERR_MOVE_DRIVING_IN_TRANS           948

#define ERR_AXIS2_SERVO_OFFED               960

#define ERR_AXIS3_SERVO_OFFED               970

#define ERR_AXIS4_SERVO_OFFED               980

#define ERR_ROTATE_COUNTOVERLOAD            990
#define ERR_ROTATE_OVERSPEED                991
#define ERR_ROTATE_OVERLOAD                 992

#define ERR_SHIFT_NOTHOME                   998

// *** QRR Error ***
#define ERR_QRR_OPEN                        1001
#define ERR_QRR_READTIMEOUT                 1002
#define ERR_QRR_WRITE                       1003
#define ERR_QRR_DATAFORMAT                  1004
#define ERR_QRR_READ                        1005


// *** TCU Error ***
#define ERR_TCU_OPEN                        1006
#define ERR_TCU_READTIMEOUT                 1007
#define ERR_TCU_WRITE                       1008
#define ERR_TCU_DATAFORMAT                  1009
#define ERR_TCU_READ                        1010

#define ERR_STEER_NOT_READY                 1011



#define ERR_READ_EQPIO_TIMEOVER_INI_FILE    1018
#define ERR_READ_TRANS_PARAM_INI_FILE       1019

#define ERR_STB_RIGHT_OHT_DETECT            1020
#define ERR_STB_LEFT_OHT_DETECT             1021

#define ERR_PIO_GO_OFF                      1022

#define ERR_GRIPPER_SEN_ON_IN_HAND_OPEN     1023
#define ERR_GRIPPER_SEN_OFF_IN_HAND_CLOSE   1024
#define ERR_GRIP_SEN_ON_IN_LOADED           1025
#define ERR_GRIP_SEN_OFF_IN_UNLOADED        1026

#define ERR_HAND_LIMIT_SEN_ALL_ON           1027
#define ERR_HAND_LIMIT_SEN_ALL_OFF          1028

#define ERR_HAND_CANNOTOPEN_IN_HOIST_HOME   1029

#define ERR_HOISTALIGNSEN_ABNORMAL_ON       1030

#define ERR_FAN_FAIL                        1031

#define ERR_NODE_COUNT_RESET_NOT_AVAILABLE  1032
#define ERR_DRIVING_CAN_NOT_BECAME_HOME     1033

#define ERR_HAND_OPEN_SEN_ON_BEFORE_LOAD    1034
#define ERR_HAND_CLOSE_SEN_OFF_BEFORE_LOAD  1035

#define ERR_HAND_OPEN_SEN_OFF_IN_LOAD       1036
#define ERR_HAND_CLOSE_SEN_ON_IN_LOAD       1037
#define ERR_HAND_OPEN_SEN_ON_IN_UNLOAD      1038
#define ERR_HAND_CLOSE_SEN_OFF_IN_UNLOAD    1039
#define ERR_HAND_OPEN_SEN_OFF_BEFORE_UNLOAD 1040
#define ERR_HAND_CLOSE_SEN_ON_BEFORE_UNLOAD 1041

#define ERR_DRIVING1_ABNORMAL_MOVING        1042
#define ERR_DRIVING2_ABNORMAL_MOVING        1043
#define ERR_HOIST_ABNORMAL_MOVING           1044
#define ERR_SHIFT_ABNORMAL_MOVING           1045

#define ERR_DRIVING1_OUT_OF_P_LIMIT         1046
#define ERR_DRIVING1_OUT_OF_N_LIMIT         1047
#define ERR_DRIVING2_OUT_OF_P_LIMIT         1048
#define ERR_DRIVING2_OUT_OF_N_LIMIT         1049
#define ERR_HOIST_OUT_OF_P_LIMIT            1050
#define ERR_HOIST_OUT_OF_N_LIMIT            1051
#define ERR_SHIFT_OUT_OF_P_LIMIT            1052
#define ERR_SHIFT_OUT_OF_N_LIMIT            1053

#define ERR_DRIVING_ACCEL_MAX_LIMIT         1054
#define ERR_DRIVING_ACCEL_MIN_LIMIT         1055
#define ERR_DRIVING_DECEL_MAX_LIMIT         1056
#define ERR_DRIVING_DECEL_MIN_LIMIT         1057
#define ERR_DRIVING_SPEED_MAX_LIMIT         1058
#define ERR_DRIVING_SPEED_MIN_LIMIT         1059
#define ERR_DRIVING_DIST_P_LIMIT            1060
#define ERR_DRIVING_DIST_N_LIMIT            1061


#define ERR_SEN_ABNORMAL_HAND               1063
#define ERR_SEN_ABNORMAL_SHUTTER_FRONT      1064
#define ERR_SEN_ABNORMAL_SHUTTER_REAR       1065
#define ERR_SEN_ABNORMAL_STEERING_FRONT     1066
#define ERR_SEN_ABNORMAL_STEERING_REAR      1067
#define ERR_SEN_ABNORMAL_STOPTAG_FRONT_OFF  1068

#define ERR_DRV_ABNORMAL_STOP               1069
#define ERR_DRV_ABNORMAL_STOP_STEER_CROSS   1070
#define ERR_DRV_MOVE_IN_PBS_UBG_DETECT      1071
#define ERR_SHIFTHOMESEN_ABNORMAL_ON       	1072
#define ERR_GRIP_SEN_ON_IN_UNLOADED			1073
#define ERR_DRV_ABNORMAL_STOP_CID           1074
#if(OHT_NAME==OHT_NAME_P4_MASK)
#define ERR_CARRIERTYPE_MISMATCH			1075	//P4 Mask. mgchoi
#endif
#define ERR_DATAMANAGER_NO_ELEMENT          1100
#define ERR_DATAMANAGER_FAIL_MAKE_TLV       1101

#define ERR_DATA_STATION_NOEXIST            1110
#define ERR_DATA_TEACHING_NOEXIST           1111

#define ERR_DATA_UPDATE_INVALIED_FILE_NAME  1200
#define ERR_DATA_UPDATE_EMPTY_FILE          1201
#define ERR_UPDATE_DATA_UPDATE_INVALID_FILE 1202
#define ERR_DATA_UPDATE_COPY_FAIL           1203
#define ERR_DATA_UPDATE_BLOCKED             1204

// -- Inner MAC Detect --
// by zzang9un 2013.07.08 : MAC 관련 에러 코드 추가
#define ERR_INNER_MAC_NOT_DETECT            1220
#define ERR_INNER_MAC_DETECT                1221

#define ERR_FOUP_COVER_OPEN_DETECT          1230

#define ERR_FRONT_BUFFER_DETECT_AFTER_UNLOAD_BUFFER		1250
#define ERR_REAR_BUFFER_DETECT_AFTER_UNLOAD_BUFFER		1251
#define ERR_FRONT_BUFFER_NOT_DETECT_AFTER_LOAD_BUFFER		1252
#define ERR_REAR_BUFFER_NOT_DETECT_AFTER_LOAD_BUFFER		1253

#define ERR_TRANS_AXIS3MOVE_TIMEOVER		1300
#define ERR_HAND_PINPOSITION_ERROR          1301
#define ERR_HOIST_BELT_CUT			        1302

#define ERR_HAND_RIGHT_PUSHSEN_ABNORMAL_ON  1303
#define ERR_HAND_LEFT_PUSHSEN_ABNORMAL_ON   1304
#define ERR_HAND_RIGHT_PINPOSITION_ERROR    1305
#define ERR_HAND_LEFT_PINPOSITION_ERROR     1306
#define ERR_HAND_RIGHT_PUSHSEN_NOTON        1307
#define ERR_HAND_LEFT_PUSHSEN_NOTON         1308
#define ERR_HAND_SEN_MISMATCH				1309
#define ERR_TRANS_AXIS2MOVE_TIMEOVER		1310

#if(OHT_NAME == OHT_NAME_P4_MASK)
//mgchoi. 2022.08.23. P4 MaskOHT용 ERR 추가 Define
#define ERR_HANDRMR_LOADPOS_SEN_BLINK		1320
#define ERR_HANDRMR_UNLOADPOS_SEN_BLINK		1321
#define ERR_HANDRMR_NOTMOVE_PLACEMENTSENON  1322
#define ERR_HANDRMR_NOTHOME_NOTUNLOADPOSE   1324
#define ERR_HANDRMR_NOTHOME_NOTLOADPOSE     1325
#define ERR_HANDRMR_NOTWORK_NOTUNLOADPOSE   1326
#define ERR_HANDRMR_NOTWORK_NOTLOADPOSE     1327

#define ERR_HANDRMR_HOME_SEN_OFF		    1328
#define ERR_HANDRMR_WORK_SEN_ON				1329
#define ERR_HANDRMR_NOTWORK				    1330
#define ERR_HANDRMR_NOTHOME					1331

#define ERR_HANDRMR_LOADPOSITION_SEN_NOT_ON		1332
#define ERR_HANDRMR_UNLOADPOSITION_SEN_NOT_ON   1333
#define ERR_HANDRMR_WORK_TIMEOVER               1334
#define ERR_HANDRMR_WORK_IL_TIMEOVER            1335
#define ERR_HANDRMR_HOME_TIMEOVER               1336
#define ERR_HANDRMR_HOME_IL_TIMEOVER            1337

#define ERR_SEND_HANDRMR_HOME_COMMAND           1338
#define ERR_SEND_HANDRMR_HOME_IL_COMMAND        1339
#define ERR_SEND_HANDRMR_WORK_COMMAND           1340
#define ERR_SEND_HANDRMR_WORK_IL_COMMAND        1341
#define ERR_HANDRMR_NOTORIGIN                   1342

#define ERR_HANDRMR_NOTMOVE_ANTIDROPNOTOPEN  	1343
#define ERR_HANDRMR_NOTMOVE_PLACEMENT_SEN_ON	1344

#define ERR_HOIST_NOTMOVE_SHUTTER_CLOSE     1345
#define ERR_SLIDE_NOTMOVE_SHUTTER_CLOSE     1346

#define ERR_PODFLANGE_CHECK_SENS_OFF        1347

#define ERR_NOTFOUND_LOADPOSITION			1350
#define ERR_NOTFOUND_UNLOADPOSITION			1351
#define ERR_NOTFOUND_LOWERHOISTHOME			1352
#define ERR_NOTFOUND_HOISTHOME				1353

#define ERR_GRIP_SEN_OFF_AT_RMRWORK_LOAD 	    1355
#define ERR_GRIP_SEN_ON_AT_RMRWORK_UNLOAD 	    1356
#define ERR_GRIP_SEN_ON_AT_RMRHOME_LOAD 	    1357
#define ERR_GRIP_SEN_OFF_AT_RMRHOME_UNLOAD 	    1358
#define ERR_GRIP_SEN_OFF_ONMOVE   				1359
#define ERR_GRIP_SEN_OFF_ON_LOADPOSITION		1360

#define ERR_NOCARRIER_HANDGRIP_ON_ONMOVE 		1361
#define ERR_CARRIER_NOTPLACE_ANTIDROP_B_LOAD	1365
#define ERR_CARRIER_PLACE_ANTIDROP_B_UNLOAD		1366

#define ERR_PLACEMENT_SEN_OFF		            1367
#define ERR_PLACEMENT_SEN_ON			        1368
#define ERR_PLACEMENT_SEN_ON_IN_LOADED 	     	1369
#define ERR_PLACEMENT_SEN_OFF_IN_UNLOADED 	    1370
#define ERR_PLACEMENT_SEN_NOT_MATCH				1371

#define ERR_HOIST_NOTHOME_AT_HANDWORK		1372

#define ERR_HOIST_LOWERHOME_SEN_OFF			    1373
#define ERR_HOIST_HOME_SEN_ON_LOWERH_SEN_ON     1374
#define ERR_HOIST_MOVE_TIMEOVER					1375

#define ERR_SHR_NOTMOVE_PODPLACEMENT_ON  	1381
#define ERR_SHR_OPEN_BEFORE_TRANS		 	1382
#define ERR_SHR_NOTMOVE_HOISTHOME_OFF  		1383
#define ERR_SHUTTER_CLOSE_POS				1384
#define ERR_SHUTTER_OPEN_POS                1385

#define ERR_POD_NOTMOVE_IN_UNLOADPOS			1390

#define ERR_LOADPOSITION_SEN_ABNORMAL_ON		1391
#define ERR_UNLOADPOSITION_SEN_ABNORMAL_ON		1392
#define ERR_LOAD_UNLOADPOSITION_SEN_ABNORMAL_ON 1393

#define ERR_TRANS_MOVE_TIMEOVER					1394
#define ERR_TRANS_DOWN_TO2TL_TIMEOVER			1395
#define ERR_TRANS_DOWN_TL2TP_TIMEOVER			1396
#define ERR_TRANS_DOWN_TO2TP_TIMEOVER			1397
#define ERR_TRANS_UP_TP2TL_TIMEOVER				1398

#define ERR_FFU_FAIL							1399
////////////////////////////////////////////////////
#endif

//HID ErrorCode//
#define  ERR_EDC_PEAKVOLTAGE   		 1400
#define  ERR_EDC_OVERVOLTAGE		 1401
#define  ERR_EDC_UNDERVOLTAGE		 1402
#define  ERR_EDC_LOWVOLTAGE	   	 	 1403
#define  ERR_EOUT_PEAKVOLTAGE		 1404
#define  ERR_EOUT_OVERVOLTAGE  	 	 1405
#define  ERR_EOUT_UNDERVOLTAGE 	 	 1406
#define  ERR_EOUT_LOWVOLTAGE		 1407
#define  ERR_ECAP_PEAKVOLTAGE		 1408
#define  ERR_ECAP_OVERVOLTAGE  	 	 1409
#define  ERR_ECAP_UNDERVOLTAGE		 1410
#define  ERR_ECAP_LOWERVOLTAGE		 1411
#define  ERR_IAC_PEAKCURRENT		 1412
#define  ERR_IAC_OVERCURRENT   		 1413
#define  ERR_OUTPUT_PEAKCURRENT		 1414
#define  ERR_OUTPUT_OVERCURRENT		 1415
#define  ERR_OUTPUT_FUSE_OPEN   	 1416
#define  ERR_ECAP_FUSE_OPEN			 1417
#define  ERR_PICKUP_PEAKTEMP    	 1418
#define  ERR_PICKUP_OVERTEMP		 1419
#define  ERR_IGBT_PEAKTEMP	   		 1420
#define  ERR_IGBT_OVERTEMP	  	 	 1421
#define  ERR_ECAP_PEAKTEMP			 1422
#define  ERR_ECAP_OVERTEMP			 1423
#define  ERR_OUTPUT_PEAKPOWER		 1424
#define  ERR_OUTPUT_OVERPOWER		 1425
#define  ERR_SEG_GEN_SIGNAL			 1426
#define  ERR_GATE_DRIVER_FAULT  	 1427
#define  ERR_HEATSINK_OVERTEMP		 1428
#define  ERR_MULTI_SMPS_FAULT		 1429
#define  ERR_PROTECT_BD_CABLE_OPEN	 1430
#define  ERR_SMPS_FAULT_ON			 1431
#define  ERR_RES_CAP_PEAKTEMP1		 1432
#define  ERR_RES_CAP_PEAKTEMP2		 1433
#define  ERR_F_CAP_PEAKTEMP			 1434
#define  ERR_F_L_PEAKTEMP			 1435
#define  ERR_TB_PEAKTEMP			 1436
#define  ERR_ECO_OV					 1437
#define  ERR_HEATSINK_PEAKTEMP		 1438
#define  ERR_REGUL_OPEN				 1439
#define  ERR_PEAKCURRENT_WARN		 1440
#define  ERR_CAP_BANK_DISCONNECT	 1441

// BUFFER ErrorCode
#define ERR_FRONT_BUFFER_WORK_TIMEOVER				1600
#define ERR_FRONT_BUFFER_HOME_TIMEOVER          	1601
#define ERR_FRONT_BUFFER_ORIGIN_TIMEOVER        	1602
#define ERR_FRONT_BUFFER_ALARM                  	1603
#define ERR_FRONT_BUFFER_NOTORIGIN              	1604
#define ERR_SEND_FRONT_BUFFER_WORK_COMMAND      	1605
#define ERR_SEND_FRONT_BUFFER_HOME_COMMAND      	1606
#define ERR_SEND_FRONT_BUFFER_ORIGIN_COMMAND    	1607
#define ERR_FRONT_BUFFER_MOVE_CONDITION				1608
#define ERR_FRONT_BUFFER_CAST_EXIST					1609
#define ERR_FRONT_BUFFER_CAST_NOTEXIST				1610
#define ERR_FRONT_BUFFER_DETECT_SEN_OFF_IN_LOAD 	1611
#define ERR_FRONT_BUFFER_DETECT_SEN_ON_IN_LOAD 		1612
#define ERR_FRONT_BUFFER_DETECT_SEN_OFF_IN_UNLOAD 	1613
#define ERR_FRONT_BUFFER_DETECT_SEN_ON_IN_UNLOAD	1614
#define ERR_FRONT_BUFFER_HOME_SENSOR_OFF            1615
#define ERR_FRONT_BUFFER_LIMIT_SEN_ALL_ON			1616
#define ERR_FRONT_BUFFER_LIMIT_SEN_ALL_OFF			1617
#define ERR_FRONT_BUFFER_INPOSITION_ERROR           1618
#define ERR_FRONT_BUFFER_NOT_HOME					1619

#define ERR_REAR_BUFFER_WORK_TIMEOVER           	1650
#define ERR_REAR_BUFFER_HOME_TIMEOVER           	1651
#define ERR_REAR_BUFFER_ORIGIN_TIMEOVER         	1652
#define ERR_REAR_BUFFER_ALARM                   	1653
#define ERR_REAR_BUFFER_NOTORIGIN               	1654
#define ERR_SEND_REAR_BUFFER_WORK_COMMAND       	1655
#define ERR_SEND_REAR_BUFFER_HOME_COMMAND       	1656
#define ERR_SEND_REAR_BUFFER_ORIGIN_COMMAND     	1657
#define ERR_REAR_BUFFER_MOVE_CONDITION				1658
#define ERR_REAR_BUFFER_CAST_EXIST					1659
#define ERR_REAR_BUFFER_CAST_NOTEXIST				1660
#define ERR_REAR_BUFFER_DETECT_SEN_OFF_IN_LOAD  	1661
#define ERR_REAR_BUFFER_DETECT_SEN_ON_IN_LOAD  		1662
#define ERR_REAR_BUFFER_DETECT_SEN_OFF_IN_UNLOAD 	1663
#define ERR_REAR_BUFFER_DETECT_SEN_ON_IN_UNLOAD		1664
#define ERR_REAR_BUFFER_HOME_SENSOR_OFF             1665
#define ERR_REAR_BUFFER_LIMIT_SEN_ALL_ON			1666
#define ERR_REAR_BUFFER_LIMIT_SEN_ALL_OFF			1667
#define ERR_REAR_BUFFER_INPOSITION_ERROR            1668
#define ERR_REAR_BUFFER_NOT_HOME					1669
#define ERR_BUFFER_TYPE_NONE						1670

//Tray Error Code
#define ERR_CART_FORWARD_TIMEOVER              		1700
#define ERR_CART_BACKWARD_TIMEOVER                  1701
#define ERR_CART_ORIGIN_TIMEOVER                    1702
#define ERR_CART_SLIDING_ALARM                      1703
#define ERR_CART_SLIDING_NOTORIGIN                  1704
#define ERR_CART_BACKWARDSEN_OFF                    1705
#define ERR_CART_FRONTSEN_OFF                       1706
#define ERR_CART_REARSEN_OFF                        1707
#define ERR_CART_FRONT_REARSEN_OFF                  1708
#define ERR_CART_SLIDING_SENSOR_ONERR               1709
#define ERR_CART_SLIDING_SENSOR_OFFERR              1710
#define ERR_CART_TRAY_NOTEXIST                      1711
#define ERR_CART_TRAY_EXIST                         1712
#define ERR_CART_TRAY_DETECTSEN_MISMATCH            1713
#define ERR_CART_SLIDING_INPOSITION_ERROR           1714
#define ERR_CART_LOCKSEN_OFF                        1715
#define ERR_CART_LOCKSEN_ON                         1716
#define ERR_CART_LOCKSEN_ABNORMAL_ON                1717
#define ERR_CART_SEN_ALL_OFF						1718

#define ERR_TRAY_CLAMP_TIMEOVER                     1720
#define ERR_TRAY_UNCLAMP_TIMEOVER                  	1721
#define ERR_TRAY_CLAMP_ORIGIN_TIMEOVER              1722
#define ERR_TRAY_CLAMP_ALARM                        1723
#define ERR_TRAY_CLAMP_NOTHOME                      1724
#define ERR_TRAY_CLAMP_ON						 	1725
#define ERR_TRAY_CLAMP_NOTORIGIN                    1726
#define ERR_TRAY_CLAMP_PUSHSEN_ON                   1727
#define ERR_TRAY_CLAMP_PUSHSEN_OFF                  1728
#define ERR_CLAMPPUSH_UNCLAMP_SENSOR_ON             1729
#define ERR_CLAMPPUSH_UNCLAMP_SENSOR_OFF            1730
#define ERR_TRAY_CLAMP_INPOSITON_ERROR              1731

#define ERR_LIFTER_UP_TIMEOVER                 		1733
#define ERR_LIFTER_DOWN_TIMEOVER                    1734
#define ERR_LIFTER_ORIGIN_TIMEOVER                  1735
#define ERR_LIFTER_ALARM                            1736
#define ERR_LIFTER_NOTUP                            1737
#define ERR_LIFTER_NOTDOWN                          1738
#define ERR_LIFTER_NOTORIGIN                        1739
#define ERR_LIFTER_SENSOR_ONERR                     1740
#define ERR_LIFTER_SENSOR_OFFERR                    1741
#define ERR_LIFTER_INPOSITION_ERROR                 1742

#define ERR_CARTPIO_GOSIG_NOT_ON            	    1743

#define ERR_VMS_OPEN                        		1801
#define ERR_VMS_READTIMEOUT                 		1802
#define ERR_VMS_WRITE                       		1803
#define ERR_VMS_DATAFORMAT                  		1804
#define ERR_VMS_READ                        		1805
#define ERR_VMS_NACK                        		1806
#define ERR_VMS_CONNECT                        		1807

#define ERR_USE_NOEXIST                     2300    ///< USE Index가 없는 경우

// Mark 시 에러
#define ERR_NOTMATCH_TAGLENGTH_FINALNODE_MET_EARLY						2301
#define ERR_NOTMATCH_TAGLENGTH_FINALNODE_MET_LATE						2302
#define ERR_NOTMATCH_TAGLENGTH_FINALNODE_MOVE_BACK_LIMIT				2303
#define ERR_NOTMATCH_TAGLENGTH_FINALNODE_IN_POSITION_ERROR				2304

#define ERR_NOTMATCH_TAGLENGTH_FINALSTATION_TAG_MET_EARLY				2311
#define ERR_NOTMATCH_TAGLENGTH_FINALSTATION_TAG_MET_LATE				2312
#define ERR_NOTMATCH_TAGLENGTH_FINALSTATION_TAG_MOVE_BACK_LIMIT			2313
#define ERR_NOTMATCH_TAGLENGTH_FINALSTATION_IN_POSITION_ERROR			2314

#define ERR_NOTMATCH_TAGLENGTH_FINALSTATION_QR_LEFT_MET_EARLY			2321
#define ERR_NOTMATCH_TAGLENGTH_FINALSTATION_QR_LEFT_MET_LATE			2322
#define ERR_NOTMATCH_TAGLENGTH_FINALSTATION_QR_LEFT_MOVE_BACK_LIMIT		2323

#define ERR_NOTMATCH_TAGLENGTH_FINALSTATION_QR_RIGHT_MET_EARLY			2331
#define ERR_NOTMATCH_TAGLENGTH_FINALSTATION_QR_RIGHT_MET_LATE			2332
#define ERR_NOTMATCH_TAGLENGTH_FINALSTATION_QR_RIGHT_MOVE_BACK_LIMIT	2333


// -------------------------------------------
// Common Error Code
// -------------------------------------------
// by zzang9un 2013.10.28 : 에러 코드 번호 수정
#define ERR_TAG_NAME                        2500
#define ERR_TAG_CMDTYPE                     2501
#define ERR_TAG_PROTOTYPE                   2502
#define ERR_BODY_LENGTH                     2510
#define ERR_ENCODE_LENGTH                   2520
#define ERR_DECODE_LENGTH                   2521
#define ERR_ENCODE_UNKNOWN_DATA_LENGTH      2530
#define ERR_PROTOCOL_DECODE                 2531

//  PIO 표준화 by kks
#define ERR_PIO_READY_ABNORMAL_ON_TD1				2600
#define ERR_PIO_LREQ_ABNORMAL_ON_TD1				2601
#define ERR_PIO_UREQ_ABNORMAL_ON_TD1				2602
#define ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1			2603
#define ERR_PIO_ES_ABNORMAL_OFF_TD1					2604
#define ERR_PIO_HOAVBL_ABNORMAL_OFF_TD1				2605

#define ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA1			2606
#define ERR_PIO_ES_ABNORMAL_OFF_TA1					2607
#define ERR_PIO_HOAVBL_ABNORMAL_OFF_TA1				2608
#define ERR_PIO_UREQ_ABNORMAL_ON_TA1				2609
#define ERR_PIO_LREQ_ABNORMAL_ON_TA1				2610
#define ERR_PIO_READY_ABNORMAL_ON_TA1				2611
#define ERR_PIO_LREQ_ABNORMAL_OFF_TA1				2612
#define ERR_PIO_UREQ_ABNORMAL_OFF_TA1				2613

#define ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA2			2614
#define ERR_PIO_ES_ABNORMAL_OFF_TA2					2615
#define ERR_PIO_HOAVBL_ABNORMAL_OFF_TA2				2616
#define ERR_PIO_UREQ_ABNORMAL_ON_TA2				2617
#define ERR_PIO_LREQ_ABNORMAL_ON_TA2				2618
#define ERR_PIO_LREQ_ABNORMAL_OFF_TA2				2619
#define ERR_PIO_UREQ_ABNORMAL_OFF_TA2				2620
#define ERR_PIO_READY_ABNORMAL_OFF_TA2				2621

#define ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHING	2622
#define ERR_PIO_ES_ABNORMAL_OFF_STRETCHING			2623
#define ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHING		2624
#define ERR_PIO_UREQ_ABNORMAL_ON_STRETCHING			2625
#define ERR_PIO_LREQ_ABNORMAL_ON_STRETCHING			2626
#define ERR_PIO_UREQ_ABNORMAL_OFF_STRETCHING		2627
#define ERR_PIO_READY_ABNORMAL_OFF_STRETCHING		2628


#define ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_STRETCHED	2629
#define ERR_PIO_ES_ABNORMAL_OFF_STRETCHED			2630
#define ERR_PIO_HOAVBL_ABNORMAL_OFF_STRETCHED		2631
#define ERR_PIO_UREQ_ABNORMAL_ON_STRETCHED			2632
#define ERR_PIO_LREQ_ABNORMAL_ON_STRETCHED			2633
#define ERR_PIO_READY_ABNORMAL_OFF_STRETCHIED		2634
#define ERR_PIO_LREQ_ABNORMAL_ON_STRETCHIED			2635
#define ERR_PIO_UREQ_ABNORMAL_OFF_STRETCHIED		2636

#define ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_FOLDING		2637
#define ERR_PIO_ES_ABNORMAL_OFF_FOLDING				2638
#define ERR_PIO_HOAVBL_ABNORMAL_OFF_FOLDING			2639
#define ERR_PIO_UREQ_ABNORMAL_ON_FOLDING			2640
#define ERR_PIO_LREQ_ABNORMAL_ON_FOLDING_U			2641
#define ERR_PIO_LREQ_ABNORMAL_ON_FOLDING_L			2642
#define ERR_PIO_READY_ABNORMAL_OFF_FOLDING			2643

#define ERR_PIO_GO_SIGNAL_ABNORMAL_OFF_TA3			2652
#define ERR_PIO_ES_ABNORMAL_OFF_TA3					2653
#define ERR_PIO_HOAVBL_ABNORMAL_OFF_TA3				2654
#define ERR_PIO_UREQ_ABNORMAL_ON_TA3				2655
#define ERR_PIO_LREQ_ABNORMAL_ON_TA3				2656
#define ERR_PIO_READY_ABNORMAL_ON_TA3				2657

#define ERR_PASSBOX_PIO_GO_SIGNAL_ABNORMAL_OFF				2700
#define ERR_PASSBOX_PIO_ES_SIGNAL_ABNORMAL_OFF				2701
#define ERR_PASSBOX_PIO_PB_AVBL_SIGNAL_ABNORMAL_OFF			2702
#define ERR_PASSBOX_PIO_CLOSE_SIGNAL_INIT_ABNORMAL_ON		2703
#define ERR_PASSBOX_PIO_BUSY_SIGNAL_INIT_ABNORMAL_ON		2704
#define ERR_PASSBOX_PIO_OPEN_SIGNAL_INIT_ABNORMAL_ON		2705
#define ERR_PASSBOX_PIO_CLOSE_SIGNAL_ABNORMAL_OFF			2706
#define ERR_PASSBOX_PIO_BUSY_SIGNAL_ABNORMAL_OFF			2707
#define ERR_PASSBOX_PIO_CLOSE_SIGNAL_ABNORMAL_ON			2708
#define ERR_PASSBOX_PIO_BUSY_SIGNAL_ABNORMAL_ON				2709
#define ERR_PASSBOX_PIO_OPEN_SIGNAL_ABNORMAL_OFF			2710
#define ERR_PASSBOX_PIO_GO_SIGNAL_CHECK_ABNORMAL_OFF		2711
#define ERR_PASSBOX_PIO_ES_SIGNAL_CHECK_ABNORMAL_OFF		2712
#define ERR_PASSBOX_PIO_PB_AVBL_SIGNAL_CHECK_ABNORMAL_OFF	2713

//CID
//#define ERR_CID_Occup_On_Interlock_On				2800
#define ERR_CID_OCCUP_ON_INTERLOCK_ON				2800
#define ERR_CID_CONNECTION_OFF_INTERLOCK_ON			2801

// Error code added, 20161030, Kunjin
#define ERR_CID_INTERLOCK_ON_BEFORE_LINK			2802

#define ERR_CID_Left_Link_CMD						2810
#define ERR_CID_Right_Link_CMD 						2811
#define ERR_CID_Occup_OK_CMD       					2812
#define ERR_CID_Occup_compt_CMD    					2813
#define ERR_CID_Occup_compt_Ack    					2814
#define ERR_CID_OPEN             					2815
#define ERR_CID_WRITE          						2816
#define ERR_CID_READTIMEOUT    						2817


// Elevetor Error Define
#define ERR_LIFTER_OPC_RESET_CMD_SEND_FAIL					2900
#define ERR_LIFTER_OPC_RESET_CMD_RESULT_FAIL				2902
#define ERR_LIFTER_INPUT_REQUEST_CMD_SEND_FAIL				2903
#define ERR_LIFTER_INPUT_REQUEST_CONFIRM_TIME_OUT			2904
#define ERR_LIFTER_OUTPUT_REQUEST_CONFIRM_TIME_OUT			2905
#define ERR_LIFTER_OUTPUT_REQUEST_LEFT_RIGHT_NOT_DEF		2906


#define ERR_MTL_TAKEOUT_TIME_OUT		2920
#define ERR_MTL_TAKEIN_TIME_OUT			2921

#define ERR_MTL_LG_OPC_RESET_CMD_SEND_FAIL					2922
#define ERR_MTL_LG_OPC_RESET_CMD_RESULT_FAIL				2923
#define ERR_MTL_LG_REQUEST_CMD_SEND_FAIL					2924
#define ERR_MTL_LG_REQUEST_CONFIRM_TIME_OUT					2925


// CMD&PassPermit Error Define
#define ERR_STD_CMD_DATA_FAIL								3000
#define ERR_STD_INNER_DATA_FAIL								3001
#define ERR_STD_CMD_CYCLE_FAIL							 	3010
#define ERR_STD_CMD_PATH_FAIL							 	3011
#define ERR_STD_CMD_ST_PATH_FAIL						  	3012
#define ERR_STD_CMD_MD_PATH_FAIL						   	3013
#define ERR_STD_CMD_ED_PATH_FAIL						  	3014
#define ERR_STD_CMD_PATH_FAIL_ETC						 	3015
#define ERR_STD_INNER_PATH_FAIL							 	3016
#define ERR_STD_ST_PASSPERMIT_FAIL							3022
#define ERR_STD_MD_PASSPERMIT_FAIL							3023
#define ERR_STD_ED_PASSPERMIT_FAIL							3024
#define ERR_STD_ETC_PASSPERMIT_FAIL							3025


//ServiceVHL Error
#define ERR_SERVICE_TEMPERATURE_CAPBANK_ERROR		3100
#define ERR_SERVICE_TEMPERATURE_L_HID_ERROR			3101
#define ERR_SERVICE_TEMPERATURE_R_HID_ERROR			3102
#define ERR_SERVICE_TEMPERATURE_CAPBANK_WARNING		3103
#define ERR_SERVICE_TEMPERATURE_L_HID_WARNING		3104
#define ERR_SERVICE_TEMPERATURE_R_HID_WARNING		3105
#define ERR_SUCTION_MOVE_OPEN_POSITION_TIMEOUT		3106
#define ERR_SUCTION_MOVE_CLOSE_POSITION_TIMEOUT		3107
#define ERR_SUCTION_MOVE_MIDDLE_POSITION_TIMEOUT	3108
#define ERR_SUCTION_MOVE_ZERO_POSITION_TIMEOUT		3109
#define ERR_SUCTION_MOVE_POSITION_TIMEOUT			3110
#define ERR_SUCTION_NOT_INPOSITION					3111
#define ERR_SUCTION_INVALID_POSITION				3112
#define ERR_LITZCABLE_DETECT						3113
#define ERR_SERVICE_MAKE_PATHDATA					3114
#define ERR_SERVICE_VISION_SCENE_SETTING			3115
#define ERR_SERVICE_VISION_DETECT_PARTICLE			3116
#define ERR_SERVICE_SUCTION_NOT_ON					3117
#define ERR_SERVICE_OCS_CANCEL						3118
#define ERR_SERVICE_GO_CMD_TIME_OUT					3119
#define ERR_SUCTION_INVALID_IO						3125
#define ERR_SERVICE_MODE_INVALID					3126
#define ERR_FAN2_FAIL                        		3127
#define ERR_FAN3_FAIL                        		3128
#define ERR_SUCTION_NOT_ORIGIN						3129

//CM Error(Serial)
#define ERR_SUCTION_NOT_CONNECTED					3120
#define ERR_SUCTION_WRITE							3121
#define ERR_SUCTION_POSITION_DIFFERENCE				3122
#define ERR_SUCTION_NO_RESPONSE						3123
#define ERR_SUCTION_INCORRECT_RESPONSE				3124

//Slope sensor Error(Serial)
#define ERR_SLOPE_NOT_CONNECTED						3130
#define ERR_SLOPE_WRITE								3131
#define ERR_SLOPE_POSITION_DIFFERENCE				3132
#define ERR_SLOPE_NO_RESPONSE						3133
#define ERR_SLOPE_INCORRECT_RESPONSE				3134
#define ERR_SLOPE_OPEN								3135
#define ERR_SLOPE_EXEC_TIMEOVER						3136
#define ERR_SLOPE_GET_VALUE							3137
#define ERR_SLOPE_READ_VALUE						3138
#define ERR_SLOPE_READ_ABNORMAL_VALUE				3139
#define ERR_SLOPE_VALUE_OVERFLOW					3140
#define ERR_SLOPE_NOT_INIT							3141

#define ERR_SERVICE_VIBRATION_NOT_ON				3150
#define ERR_SERVICE_VIBRATION_NOT_OFF				3151

//Read ServiceParam Inifile error
#define	ERR_SERVICEPARAM_DIFFERENT_COUNT			3160
#define ERR_SERVICEPARAM_INVALID_PATHID				3161
#define ERR_SERVICEPARAM_DUPLICATED_PATHID			3162
#define ERR_SERVICEPARAM_READ_FAIL					3163
//Read DiagParam Inifile Error
#define ERR_DIAGPARAM_READ_FAIL						3165
#define ERR_DIAGPARAM_INVALID_IP  					3166
#define ERR_DIAGPARAM_INVALID_PORT					3167

#define ERR_DIAG_INVALID_DATA_ID					3170


//Auto Teaching관련 에러
#define ERR_HOIST_N_ERROR_AUTOTEACHING				4000
#define ERR_AUTOTEACHING_RESPONCE                   4001
#define ERR_AUTOTEACHING_TIMEOUT                   	4002
#define ERR_AUTOTEACHING_ABNORMAL_HANDPUSH          4003
#define ERR_AUTOTEACHING_ABNORMAL_OUTTRIGGER        4004
#define ERR_AUTOTEACHING_DRIVING_AB			        4005

#define ERR_AUTOTEACHING_TEACHING_FAIL		        4006
#define ERR_AUTOTEACHING_DRIVING_LIMIT		        4007

#define ERR_AUTOTEACHING_NULLTYPE_NO_DATA	       	4008
#define ERR_AUTOTEACHING_IP_DISMATCH	       		4009

#define ERR_DUAL_STAGE_STB_PORT_N_ON_TIMEOUT		4101
#define ERR_DUAL_STAGE_STB_PORT_P_ON_TIMEOUT		4102

#define ERR_DUAL_STAGE_STB_READY_ON_TIMEOUT			4103
#define ERR_DUAL_STAGE_STB_CS_MISMATCH				4104

#define  ERR_DUAL_STAGE_STB_HOAVBL_OFF_TIMEOUT		4105
#define  ERR_DUAL_STAGE_STB_HOAVBL_ABNORMAL_OFF		4106
#define  ERR_DUAL_STAGE_STB_ES_ABNORMAL_OFF			4107



#define  ACQUIRE_E84_TIMEOUT_PIO_READY_ABNORMAL_ON_TD1                  9000
#define  ACQUIRE_E84_TIMEOUT_PIO_LREQ_ABNORMAL_ON_TD1                   9001
#define  ACQUIRE_E84_TIMEOUT_PIO_UREQ_ABNORMAL_ON_TD1                   9002
#define  ACQUIRE_E84_TIMEOUT_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1             9003
#define  ACQUIRE_E84_TIMEOUT_PIO_ES_ABNORMAL_OFF_TD1                    9004
#define  ACQUIRE_E84_TIMEOUT_PIO_HOAVBL_ABNORMAL_OFF_TD1                9005
#define  ACQUIRE_E84_TIMEOUT_PIO_GO_SIGNAL_ABNORMAL_OFF_TA1             9006
#define  ACQUIRE_E84_TIMEOUT_PIO_ES_ABNORMAL_OFF_TA1                    9007
#define  ACQUIRE_E84_TIMEOUT_PIO_HOAVBL_ABNORMAL_OFF_TA1                9008
#define  ACQUIRE_E84_TIMEOUT_PIO_UREQ_ABNORMAL_ON_TA1                   9009
#define  ACQUIRE_E84_TIMEOUT_PIO_LREQ_ABNORMAL_ON_TA1                   9010
#define  ACQUIRE_E84_TIMEOUT_PIO_READY_ABNORMAL_ON_TA1                  9011
#define  ACQUIRE_E84_TIMEOUT_PIO_LREQ_ABNORMAL_OFF_TA1                  9012
#define  ACQUIRE_E84_TIMEOUT_PIO_UREQ_ABNORMAL_OFF_TA1                  9013
#define  ACQUIRE_E84_TIMEOUT_PIO_GO_SIGNAL_ABNORMAL_OFF_TA2             9014
#define  ACQUIRE_E84_TIMEOUT_PIO_ES_ABNORMAL_OFF_TA2                    9015
#define  ACQUIRE_E84_TIMEOUT_PIO_HOAVBL_ABNORMAL_OFF_TA2                9016
#define  ACQUIRE_E84_TIMEOUT_PIO_UREQ_ABNORMAL_ON_TA2                   9017
#define  ACQUIRE_E84_TIMEOUT_PIO_LREQ_ABNORMAL_ON_TA2                   9018
#define  ACQUIRE_E84_TIMEOUT_PIO_LREQ_ABNORMAL_OFF_TA2                  9019
#define  ACQUIRE_E84_TIMEOUT_PIO_UREQ_ABNORMAL_OFF_TA2                  9020
#define  ACQUIRE_E84_TIMEOUT_PIO_READY_ABNORMAL_OFF_TA2                 9021
#define  ACQUIRE_E84_TIMEOUT_PIO_GO_SIGNAL_ABNORMAL_OFF_TA3             9052
#define  ACQUIRE_E84_TIMEOUT_PIO_ES_ABNORMAL_OFF_TA3                    9053
#define  ACQUIRE_E84_TIMEOUT_PIO_HOAVBL_ABNORMAL_OFF_TA3                9054
#define  ACQUIRE_E84_TIMEOUT_PIO_UREQ_ABNORMAL_ON_TA3                   9055
#define  ACQUIRE_E84_TIMEOUT_PIO_LREQ_ABNORMAL_ON_TA3                   9056
#define  ACQUIRE_E84_TIMEOUT_PIO_READY_ABNORMAL_ON_TA3                  9057
#define  DEPOSIT_E84_TIMEOUT_PIO_READY_ABNORMAL_ON_TD1                  9500
#define  DEPOSIT_E84_TIMEOUT_PIO_LREQ_ABNORMAL_ON_TD1                   9501
#define  DEPOSIT_E84_TIMEOUT_PIO_UREQ_ABNORMAL_ON_TD1                   9502
#define  DEPOSIT_E84_TIMEOUT_PIO_GO_SIGNAL_ABNORMAL_OFF_TD1             9503
#define  DEPOSIT_E84_TIMEOUT_PIO_ES_ABNORMAL_OFF_TD1                    9504
#define  DEPOSIT_E84_TIMEOUT_PIO_HOAVBL_ABNORMAL_OFF_TD1                9505
#define  DEPOSIT_E84_TIMEOUT_PIO_GO_SIGNAL_ABNORMAL_OFF_TA1             9506
#define  DEPOSIT_E84_TIMEOUT_PIO_ES_ABNORMAL_OFF_TA1                    9507
#define  DEPOSIT_E84_TIMEOUT_PIO_HOAVBL_ABNORMAL_OFF_TA1                9508
#define  DEPOSIT_E84_TIMEOUT_PIO_UREQ_ABNORMAL_ON_TA1                   9509
#define  DEPOSIT_E84_TIMEOUT_PIO_LREQ_ABNORMAL_ON_TA1                   9510
#define  DEPOSIT_E84_TIMEOUT_PIO_READY_ABNORMAL_ON_TA1                  9511
#define  DEPOSIT_E84_TIMEOUT_PIO_LREQ_ABNORMAL_OFF_TA1                  9512
#define  DEPOSIT_E84_TIMEOUT_PIO_UREQ_ABNORMAL_OFF_TA1                  9513
#define  DEPOSIT_E84_TIMEOUT_PIO_GO_SIGNAL_ABNORMAL_OFF_TA2             9514
#define  DEPOSIT_E84_TIMEOUT_PIO_ES_ABNORMAL_OFF_TA2                    9515
#define  DEPOSIT_E84_TIMEOUT_PIO_HOAVBL_ABNORMAL_OFF_TA2                9516
#define  DEPOSIT_E84_TIMEOUT_PIO_UREQ_ABNORMAL_ON_TA2                   9517
#define  DEPOSIT_E84_TIMEOUT_PIO_LREQ_ABNORMAL_ON_TA2                   9518
#define  DEPOSIT_E84_TIMEOUT_PIO_LREQ_ABNORMAL_OFF_TA2                  9519
#define  DEPOSIT_E84_TIMEOUT_PIO_UREQ_ABNORMAL_OFF_TA2                  9520
#define  DEPOSIT_E84_TIMEOUT_PIO_READY_ABNORMAL_OFF_TA2                 9521
#define  DEPOSIT_E84_TIMEOUT_PIO_GO_SIGNAL_ABNORMAL_OFF_TA3             9552
#define  DEPOSIT_E84_TIMEOUT_PIO_ES_ABNORMAL_OFF_TA3                    9553
#define  DEPOSIT_E84_TIMEOUT_PIO_HOAVBL_ABNORMAL_OFF_TA3                9554
#define  DEPOSIT_E84_TIMEOUT_PIO_UREQ_ABNORMAL_ON_TA3                   9555
#define  DEPOSIT_E84_TIMEOUT_PIO_LREQ_ABNORMAL_ON_TA3                   9556
#define  DEPOSIT_E84_TIMEOUT_PIO_READY_ABNORMAL_ON_TA3                  9557

#endif
