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
#define ERR_CROSSRAILDATA_FORMAT            100
#define ERR_CROSSRAILFILE_NOT_EXIST         101

// *** window errror **
#define ERR_WINDOWEXCEPTION                 110

#define ERR_MAP_DATAINVALID                 121
#define ERR_LAYOUT_PATH                     122
#define ERR_LAYOUT_NODE                     123
#define ERR_FILE_NO_EXIST                   125
#define ERR_FILE_NAME                       126
#define ERR_STEERINGPOS_FORMAT              128
#define ERR_NODE_LOOKDOWN_LIMIT             129

#define ERR_NODE_FIND                       130
#define ERR_PATH_START_NODE_LIMIT           131
#define ERR_PATH_END_NODE_LIMIT             132
#define ERR_PATH_CONNTETNODE_FIND1          135
#define ERR_PATH_CONNTETNODE_FIND2          136
#define ERR_PATH_CONNTETNODE_FIND3          137
#define ERR_PATH_CONNTETNODE_FIND4          138
#define ERR_PATH_CONNTETNODE_FIND5          139

#define ERR_PATHSEARCH_FAIL                 140
#define ERR_TGNODE_NOT_REALNODE             142

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

// HID 에러
// TP OHT에서만 사용
#define ERR_EREC_UNDERVOLTAGE               165  ///< 100V 이하 (9)
#define ERR_EREC_PEAKVOLTAGE                166  ///< 385V 이상 (1) : ()안은 HID로 부터 수신되는 에러코드
#define ERR_EREC_OVERVOLTAGE                167  ///< 380V 이상 (2)
#define ERR_ECAP_PEAKVOLTAGE                168  ///< 385V 이상 (3)
#define ERR_ECAP_OVERVOLTAGE                169  ///< 380V 이상 (4)
#define ERR_EBOOST_PEAKVOLTAGE              170  ///< 385V 이상 (5)
#define ERR_EBOOST_OVERVOLTAGE              171  ///< 380V 이상 (6)
#define ERR_ECAP_UNDERVOLTAGE               172  ///< 185V 이하 (7)
#define ERR_ECAP_LOWVOLTAGE                 173  ///< 160V 이하 (8)
#define ERR_EBOOST_UNDERVOLTAGE             174  ///< 185V 이하 (9)
#define ERR_EBOOST_LOWVOLTAGE               175  ///< 160V 이하 (10)
#define ERR_IAC_OVERCURRENT                 176  ///< 13A/6초 이상 (11)
#define ERR_ICAP_PEAKCURRENT                177  ///< 25A 이상 (12)
#define ERR_ICAP_OVERCURRENT                178  ///< 20A 이상 (13)
#define ERR_IBOOST_PEAKCURRENT              179  ///< 25A 이상 (14)

// 기존에 미리 들어있던 코드라서 다른쪽으로 이동하기가 힘듬(v4.7   v6.0 모두수정해야함)
#define ERR_NOTMATCH_NODE_STEERPOS          180
#define ERR_IBOOST_OVERCURRENT              181  ///< 20A 이상 (15)
#define ERR_EDC_PEAKVOLTAGE                 182  ///< 360V 이상 (16)
#define ERR_EDC_OVERVOLTAGE                 183  ///< 350V 이상 (17)
#define ERR_EDC_UNDERVOLTAGE                184  ///< 185V 이하 (18)
#define ERR_EDC_LOWVOLTAGE                  185  ///< 160V 이하 (19)
#define ERR_IDC_PEAKCURRENT                 186  ///< 18A 이상 (20)
#define ERR_IDC_OVERCURRENT                 187  ///< 15A 이상 (21)
#define ERR_UCAP_OVERLIFE                   188  ///< U-CAP용량저하 (22)
#define ERR_II2C_RW_FAIL                    189  ///< Flash 내 변수 저장쓰기/읽기 오류 (23)
#define ERR_PU_PEAK_TEMP_BY_SENSOR          190  ///< 75도 이상 (50)
#define ERR_OVER_TEMP_BY_SENSOR             191  ///< 65도 이상 (51)
#define ERR_ECO_FUSE_OPEN                   192  ///< Active Low (52)
#define ERR_FUSE_OPEN                       193  ///< Active Low (53)
#define ERR_HEATSINK_OVERTEMP               194  ///< 85도 이상 (54)
#define ERR_UCAP_CELL_OVERVOLTAGE           195     ///< Active Low (55)
#define ERR_UCAP_OVERTEMP                   196     ///< Active Low (56)
#define ERR_EDC_OVERPOWER                   197     ///< 3KW 이상 (57)
#define ERR_EDC_EST_OVERVOLTAGE             198     ///< 360V 이상 (25)

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

// *** HID Error ***
#define ERR_HID_OPEN                        420
#define ERR_HID_WRITE                       421
#define ERR_HID_READTIMEOUT                 422

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

// --- STB Error ----
#define ERR_STB_FOUP_EXIST                  850
#define ERR_STB_FOUP_NOTEXIST               851
#define ERR_STBSEN_NOT_OFF                  853

// --- UTB Error ----
#define ERR_UTB_FOUP_EXIST                  855
#define ERR_UTB_FOUP_NOTEXIST               856
#define ERR_UTBSEN_NOT_OFF                  857

// -- Inner Foup Detect --
#define ERR_INNER_FOUP_NOT_DETECT           854
#define ERR_INNER_FOUP_DETECT               858

// -- Inner MAC Detect --
// by zzang9un 2013.07.08 : MAC 관련 에러 코드 추가
#define ERR_INNER_MAC_NOT_DETECT            859
#define ERR_INNER_MAC_DETECT                860

// --- RF_PIO ERROR ---
#define ERR_RFID_NODATA                     891
#define ERR_RFCHANNEL_NODATA                892
#define ERR_PIO_SETTINGTIMEOVER             893
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

#define ERR_RAILSUPPORT_SEND_MOVE_WORK_CMD  1012
#define ERR_RAILSUPPORT_SEND_MOVE_HOME_CMD  1013
#define ERR_RAILSUPPORT_MOVE_WORK_TIMEOVER  1014
#define ERR_RAILSUPPORT_MOVE_HOME_TIMEOVER  1015
#define ERR_RAILSUPPORT_NOT_WORK_IN_TRANS   1016
#define ERR_RAILSUPPORT_NOT_READY           1017

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

#define ERR_SEN_ABNORMAL_RAILSUPPORT        1062
#define ERR_SEN_ABNORMAL_HAND               1063
#define ERR_SEN_ABNORMAL_SHUTTER_FRONT      1064
#define ERR_SEN_ABNORMAL_SHUTTER_REAR       1065
#define ERR_SEN_ABNORMAL_STEERING_FRONT     1066
#define ERR_SEN_ABNORMAL_STEERING_REAR      1067
#define ERR_SEN_ABNORMAL_STOPTAG_FRONT_OFF  1068

#define ERR_DRV_ABNORMAL_STOP               1069
#define ERR_DRV_ABNORMAL_STOP_STEER_CROSS   1070

#define ERR_DATAMANAGER_NO_ELEMENT          1100
#define ERR_DATAMANAGER_FAIL_MAKE_TLV       1101

#define ERR_DATA_STATION_NOEXIST            1110
#define ERR_DATA_TEACHING_NOEXIST           1111

#define ERR_DATA_UPDATE_INVALIED_FILE_NAME  1200
#define ERR_DATA_UPDATE_EMPTY_FILE          1201
#define ERR_UPDATE_DATA_UPDATE_INVALID_FILE 1202
#define ERR_DATA_UPDATE_COPY_FAIL           1203
#define ERR_DATA_UPDATE_BLOCKED             1204

#define ERR_USE_NOEXIST                     1300    ///< USE Index가 없는 경우

// -------------------------------------------
// Common Error Code
// -------------------------------------------
#define ERR_TAG_NAME                        9000
#define ERR_TAG_CMDTYPE                     9001
#define ERR_TAG_PROTOTYPE                   9002
#define ERR_BODY_LENGTH                     9010
#define ERR_ENCODE_LENGTH                   9020
#define ERR_DECODE_LENGTH                   9021
#define ERR_UNKNOWNDATALEN                  9030

#define ERR_PROTOCOL_DECODEERR              9030

//2013.12.16 doori.shin FBWF 에러 추가
#define ERR_OHT_NOT_EXECUTED				3000
#define ERR_PIPE_OPEN_ERROR					3001
#define ERR_CREATE_PROCESS					3002

#endif



