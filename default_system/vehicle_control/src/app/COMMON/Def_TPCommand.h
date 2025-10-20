/**
@file    Def_TPCommand.h
$Rev:: 299           $:  Revision of last commit
$Author:: puting     $:  Author of last commit
$Date:: 2012-11-29 1#$:  Date of last commit
@version OHT 7.0
@brief   TP Command에 관련된 Header File
*/
#ifndef Def_TPCommandH
#define Def_TPCommandH
//---------------------------------------------------------------------------

//2013.10.16 ji7412.lee HandTP Status
#define TPCMD_OHT_HANDTP_STATUS     0xA105

// OHT Status 정보 요청
#define TPCMD_OHT_STATUS            0xA104 ///< OHT Status 요청/응답 Cmd

// OHT 초기화 정보 요청
#define TPCMD_OHT_OHTINIT           0xC000 ///< OHT 초기화 정보 요청/응답 Cmd

//2013.08.22 doori.shin
#define TPCMD_OHT_DATA_IOMAP		0xB000	///< Request OHT Data

// -------------------------------------------
// Quick Command 관련
// -------------------------------------------
#define TPCMD_OHT_ESTOP             0xE101 ///< E-Stop 명령
#define TPCMD_OHT_MANUALMODE        0xE102 ///< Manual 전환 명령
#define TPCMD_OHT_AUTOMODE          0xE103 ///< Auto 전환 명령
#define TPCMD_OHT_RESET             0xE105 ///< Reset 명령
#define TPCMD_OHT_FC_PAUSE_SET      0xE106
#define TPCMD_OHT_FC_PAUSE_RESET    0xE107
#define TPCMD_OHT_STATUS_REQ		0xE110	// TP에서 OHT의 상태를 요청한다
#define TPCMD_OHT_INIT_INFO_REQ		0xE112	// TP에서 OHT의 INIT INFO를 요청한다
#define TPCMD_OHT_LINK_INFO_REQ		0xE113  // H/TP에 Link하기 위한 VHL 정보를 요청한다.



// -------------------------------------------
// OHT Assistant
// -------------------------------------------
#define TPCMD_ASSISTANT_CLOSE       0x5060
#define TPCMD_ASSISTANT_EXEC        0x5061

// -------------------------------------------
// 근접제어 기능
// -------------------------------------------
#define TPCMD_OHT_CMDBUFF_SAVE      0x5108
#define TPCMD_CMD_BUFFER_CLEAR      0x5109
#define TPCMD_OHT_REBOOT            0x5110

#define TPCMD_OHT_INFO              0x5180

// -------------------------------------------
// Online Command (Pwin32명령과 동일..)
// -------------------------------------------
// by zzang9un 2013.07.22 : 1111에서 2010으로 변경함
#define TPCMD_MAP_MAKE_AUTO         0x2010 ///< Map Make 명령

#define TPCMD_MAP_MAKE              0x1112 ///< 수동 Map Make 시작 (삭제 예정)
#define TPCMD_STOP_MAP_MAKE         0x1113 ///< 수동 Map Make 정지 (삭제 예정)

#define TPCMD_OHT_NODEDIST_CLEAR    0x1114

#define TPCMD_OHT_CMD_CANCEL        0x5115

#define TPCMD_SET_OUTPUT            0x5116
#define TPCMD_RESET_OUTPUT          0x5117
#define TPCMD_CMCONLINECMD          0x5118

#define TPCMD_OHT_LOGSAVE_DISABLE   0x5119

#define TPCMD_OHT_PCOFF             0x5120

#define TPCMD_CHANGE_MANUAL_LOAD_MAP        0x5121
#define TPCMD_CHANGE_MANUAL_LOAD_PARA       0x5122
#define TPCMD_CHANGE_MANUAL_LOAD_LOC        0x5123
#define TPCMD_CHANGE_MANUAL_LOAD_UTBLOC     0x5126

#define TPCMD_CHANGE_MANUAL_LOAD_STB_L_LOC  0x5124
#define TPCMD_CHANGE_MANUAL_LOAD_STB_R_LOC  0x5127
#define TPCMD_CHANGE_MANUAL_LOAD_EQ_X_LOC   0x5139

#define TPCMD_CHANGE_MANUAL_LOAD_NODECHG        0x5125
#define TPCMD_CHANGE_MANUAL_LOAD_NODEDIS        0x5128
#define TPCMD_CHANGE_MANUAL_LOAD_HADDET         0x5129
#define TPCMD_CHANGE_MANUAL_LOAD_CROSSRAIL      0x5130
#define TPCMD_CHANGE_MANUAL_LOAD_PIODELAYNODES  0x5100

// -------------------------------------------
// Vehicle Jog  Definition 131~149
// -------------------------------------------
#define TPCMD_GO_SETCURNODE         0x5131

#define TPCMD_GO_TARGETNODE         0x1132

#define TPCMD_GO_DISTANCE           0x5133
#define TPCMD_GO_STOP               0x5134
#define TPCMD_GO_SETCURNODE_NUM     0x5135
#define TPCMD_GO_MARK_FORWARD       0x5136
#define TPCMD_GO_MARK_BACKWARD      0x5137
#define TPCMD_GO_MARK_F_BACKWARD    0x5138

#define TPCMD_GO_TRANS_MARK_FORWARD  0x5139
#define TPCMD_GO_TRANS_MARK_BACKWARD 0x5140
#define TPCMD_GO_TRANS_SETCURNODE    0x5141

// QR Mark를 위해 추가 : 임태웅(2013.03.12)
#define TPCMD_GO_QR_LEFT_MARK_FORWARD  0x5142
#define TPCMD_GO_QR_RIGHT_MARK_FORWARD 0x5143

// -------------------------------------------
// FileControl Definition 144~169
// -------------------------------------------
//upload

#if(RFPIO_TYPE == PIOTYPE_E84)
#define TPCMD_RELOAD_PIODELAYNODES      0x5144
#endif

#define TPCMD_RELOAD_CROSSRAILINTFILE   0x5145
#define TPCMD_RELOAD_HANDDETECTEQFILE   0x5146
#define TPCMD_RELOAD_NODEDISFILE        0x5147
#define TPCMD_RELOAD_NODECHGFILE        0x5148
#define TPCMD_RELOAD_PARAFILE           0x5149
#define TPCMD_RELOAD_MAPFILE            0x5150
#define TPCMD_RELOAD_LOCFILE            0x5151
#define TPCMD_DOWN_AMCFILE              0x5152

// by zzang9un 2013.07.10 : Teaching Data 관련 Command
#define TPCMD_TEACHING_SAVE             0x5153  ///< Teaching 정보 저장 요청
#define TPCMD_TEACHING_REQ              0x5162  ///< Teaching 정보 요청

// by zzang9un 2013.07.23 : Map Make 정보
#define TPCMD_MAPMAKE_INFO              0x5154  ///< Map Make 정보를 전송하는 Command

//*** For STB Control ***
#define TPCMD_LOC_STB_L_SAVE            0x5157
#define TPCMD_LOC_STB_R_SAVE            0x5158
#define TPCMD_RELOAD_STB_L_LOCFILE      0x5160
#define TPCMD_RELOAD_STB_R_LOCFILE      0x5161
#define TPCMD_LOC_EQ_X_SAVE             0x5163
#define TPCMD_RELOAD_EQ_X_LOCFILE       0x5164

//*** For UTB Control ***
#define TPCMD_RELOAD_UTBLOCFILE         0x5165

//** 원점 OFFSET SETTING **
#define TPCMD_HOIST_ORIGIN_SETTING      0x5171
#define TPCMD_SHIFT_ORIGIN_SETTING      0x5172
#define TPCMD_ROTATE_ORIGIN_SETTING     0x5173
#define TPCMD_ROTATE_GO_ORIGIN          0x5174
#define TPCMD_HOIST_SETLIMIT_NEG        0x5175
#define TPCMD_HOIST_SETLIMIT_POSI       0x5176
#define TPCMD_SHIFT_SETLIMIT_NEG        0x5177
#define TPCMD_SHIFT_SETLIMIT_POSI       0x5178

// -------------------------------------------
// RF-ID 관련
// -------------------------------------------
#define TPCMD_READ_RFID                 0x5190

// -------------------------------------------
// RF-PIO/TP 관련
// -------------------------------------------
#define TPCMD_SET_RFPIO_CHANNEL         0x5195
#define TPCMD_SET_RFTP_ID               0x5196

// -------------------------------------------
// Use On/Off
// -------------------------------------------
#define TPCMD_USE_ON                    0x5400
#define TPCMD_USE_OFF                   0x5401

// -------------------------------------------
// Vehicle Jog  Definition 500~519
// -------------------------------------------
#define TPCMD_JOG_DRIVING_FORWARD       0x5500
#define TPCMD_JOG_DRIVING_BACKWARD      0x5501
#define TPCMD_JOG_DRIVING_DIST          0x5502
#define TPCMD_JOG_DRIVING_STOP          0x5504

#define TPCMD_RFTPJOG_DRIVING_FORWARD   0x5510
#define TPCMD_RFTPJOG_DRIVING_BACKWARD  0x5511
#define TPCMD_RFTPJOG_DRIVING_STOP      0x5512

// -------------------------------------------
// Robot Jog  Definition 515~559
// #2:Hoist, #3:Shift, #4:Rotate
// -------------------------------------------
#define TPCMD_JOG_HOIST_UP              0x5515  //(+)$T(0000)(0000)(000)

#define TPCMD_JOG_HOIST_DOWN            0x5516  //(-)
#define TPCMD_JOG_HOIST_HOME            0x5517  //$T(0000)
#define TPCMD_JOG_HOIST_STOP            0x5518
#define TPCMD_JOG_HOIST_ABS             0x5519
#define TPCMD_JOG_HOIST_UP_INC          0x5520
#define TPCMD_JOG_HOIST_DOWN_INC        0x5521
#define TPCMD_JOG_HOIST_DIST            0x5522

#define TPCMD_JOG_SHIFT_LEFT            0x5524 //(+)
#define TPCMD_JOG_SHIFT_RIGHT           0x5525 //(-)
#define TPCMD_JOG_SHIFT_HOME            0x5526
#define TPCMD_JOG_SHIFT_STOP            0x5527
#define TPCMD_JOG_SHIFT_ABS             0x5528
#define TPCMD_JOG_SHIFT_LEFT_INC        0x5529
#define TPCMD_JOG_SHIFT_RIGHT_INC       0x5530

#define TPCMD_JOG_ROTATE_CW             0x5534
#define TPCMD_JOG_ROTATE_CCW            0x5535
#define TPCMD_JOG_ROTATE_HOME           0x5536

#define TPCMD_JOG_ROTATE_STOP           0x5537
#define TPCMD_JOG_ROTATE_ABS            0x5538

#define TPCMD_JOG_ROTATE_CW_INC         0x5539

#define TPCMD_JOG_ROTATE_CCW_INC        0x5540

#define TPCMD_JOG_ALLROBOT_HOME         0x5545
#define TPCMD_JOG_ALLROBOT_STOP         0x5546
#define TPCMD_JOG_HOISTSHIFT_HOME       0x5547
#define TPCMD_JOG_SHIFTROTATE_HOME      0x5548
#if(OHT_NAME == OHT_NAME_P4_MASK)
/// mgchoi. OHT_NAME_P4_MASK용 추가 커맨드 정의
#define TPCMD_HOIST_DOWN_UNLOADPOSITION 		0x5549
#define TPCMD_HOIST_DOWN_LOADPOSITION   		0x5550
#define TPCMD_HOIST_UP_LOADPOSITION     		0x5551
#define TPCMD_HOIST_DOWN_ANTIDROP       		0x5552
#define TPCMD_HOIST_UP_HOISTHOME 	    		0x5553
#define TPCMD_HOIST_DOWN_ANTIDROP_EUV			0x5505
#define TPCMD_HOIST_DOWN_ANTIDROP_RSP200		0x5506
#define TPCMD_HOIST_DOWN_ANTIDROP_RSP150		0x5507
#endif
// -------------------------------------------
//Content : Manual Trans Command   560~579
// - Load / Unload / Move Teaching Point
// -------------------------------------------
// by zzang9un 2012.12.01 : 7.0 버전에서는 Load, Unload 두가지로만 구분
#define TPCMD_TRANS_LOAD                0x3560
#define TPCMD_TRANS_UNLOAD              0x3564

// by zzang9un 2014.04.07 : Move Teaching Point 구현
#define TPCMD_MOVE_TEACHPOINT           0x3572

// by Kunjin 2014.12.15 : Hoist home auto set 기능 구현
#define TPCMD_TRANS_HOME_AUTOSET 		0x3580

// by zzang9un 2012.12.01 : 6.5 버전 이적재 명령들은 사용안함
#define TPCMD_MANUAL_UTB_LOAD_R0        0x5555
#define TPCMD_MANUAL_UTB_LOAD_R1        0x5556

#define TPCMD_MANUAL_UTB_UNLOAD_R0      0x5557
#define TPCMD_MANUAL_UTB_UNLOAD_R1      0x5558

#define TPCMD_MANUAL_LOAD_P0_R0         0x5560
#define TPCMD_MANUAL_LOAD_P0_R1         0x5561
#define TPCMD_MANUAL_LOAD_P1_R0         0x5562
#define TPCMD_MANUAL_LOAD_P1_R1         0x5563

#define TPCMD_MANUAL_UNLOAD_P0_R0       0x5564
#define TPCMD_MANUAL_UNLOAD_P0_R1       0x5565
#define TPCMD_MANUAL_UNLOAD_P1_R0       0x5566
#define TPCMD_MANUAL_UNLOAD_P1_R1       0x5567

#define TPCMD_MANUAL_STB_L_LOAD_R0      0x5568
#define TPCMD_MANUAL_STB_L_LOAD_R1      0x5569

#define TPCMD_MANUAL_STB_L_UNLOAD_R0    0x5570
#define TPCMD_MANUAL_STB_L_UNLOAD_R1    0x5571

#define TPCMD_MANUAL_STB_R_LOAD_R0      0x5581
#define TPCMD_MANUAL_STB_R_LOAD_R1      0x5582

#define TPCMD_MANUAL_STB_R_UNLOAD_R0    0x5583
#define TPCMD_MANUAL_STB_R_UNLOAD_R1    0x5584

#define TPCMD_MANUAL_EQ_X_LOAD_P0_R0    0x5568
#define TPCMD_MANUAL_EQ_X_LOAD_P1_R0    0x5569

#define TPCMD_MANUAL_EQ_X_UNLOAD_P0_R0  0x5570
#define TPCMD_MANUAL_EQ_X_UNLOAD_P1_R0  0x5571

#define TPCMD_MOVE_23AXIS_TEACHPOINT    0x5573
#define TPCMD_MANUAL_SCAN_UTB           0x5577
#define TPCMD_MOVE_UTB_TEACHPOINT       0x5578
#define TPCMD_MOVE_UTB_23AXIS_TEACHPOINT   0x5579

#define TPCMD_MOVE_STB_L_TEACHPOINT     0x5574
#define TPCMD_MOVE_STB_L_23AXIS_TEACHPOINT 0x5575
#define TPCMD_MANUAL_SCAN_STB_L         0x5576

#define TPCMD_MOVE_STB_R_TEACHPOINT     0x5585
#define TPCMD_MOVE_STB_R_23AXIS_TEACHPOINT 0x5586
#define TPCMD_MANUAL_SCAN_STB_R         0x5587

#define TPCMD_MOVE_EQ_X_TEACHPOINT      0x5598
#define TPCMD_MOVE_EQ_X_23AXIS_TEACHPOINT 0x5599

// -------------------------------------------
// AMC Output 600~670
// -------------------------------------------
#define TPCMD_IO_TPCMD_START            0x5600

//** Hand ***
#if(OHT_NAME == OHT_NAME_P4_MASK)	//mgchoi. 2022.08.23. P4 MaskOHT용 Hand Command Define
//#define TPCMD_HAND_WORK                 0x5600
//#define TPCMD_HAND_HOME    				0x5601
#define TPCMD_HAND_HOME_IL              0x9999		//temp
#define TPCMD_HAND_WORK_IL              0x9998      //temp
#endif
#define TPCMD_HAND_CLOSE                0x5600
#define TPCMD_HAND_OPEN	    	        0x5601
#define TPCMD_HAND_OPEN_LIMIT           0x5602
#define TPCMD_HAND_HOMING	            0x5603
#define TPCMD_HAND_RESET	    	    0x5604
#define TPCMD_HAND_FREE	    	        0x5605
#define TPCMD_HAND_ENABLE   	        0x5606
#define TPCMD_HAND_PIO_SELECT_ON        0x5607
#define TPCMD_HAND_PIO_SELECT_OFF       0x5608
#define TPCMD_HAND_PWR_ON               0x5609
#define TPCMD_HAND_PWR_OFF              0x5610
#define TPCMD_HAND_PWR_RST              0x5611


//** PIO ***
#define TPCMD_LEFT_PIO_SELECT_ON        0x5612
#define TPCMD_LEFT_PIO_SELECT_OFF       0x5613
#define TPCMD_LEFT_PIO_VALID_ON         0x5614
#define TPCMD_LEFT_PIO_VALID_OFF   	    0x5615
#define TPCMD_LEFT_PIO_CS_0_ON    	    0x5616
#define TPCMD_LEFT_PIO_CS_0_OFF    	    0x5617
#define TPCMD_LEFT_PIO_CS_1_ON    	    0x5618
#define TPCMD_LEFT_PIO_CS_1_OFF    	    0x5619
#define TPCMD_LEFT_PIO_TR_REQ_ON   	    0x5620
#define TPCMD_LEFT_PIO_TR_REQ_OFF  	    0x5621
#define TPCMD_LEFT_PIO_BUSY_ON	  	    0x5622
#define TPCMD_LEFT_PIO_BUSY_OFF	  	    0x5623
#define TPCMD_LEFT_PIO_COMPT_ON	  	    0x5624
#define TPCMD_LEFT_PIO_COMPT_OFF	  	0x5625
#define TPCMD_LEFT_PIO_ALL_OFF          0x5626

#define TPCMD_RIGHT_PIO_SELECT_ON       0x5627
#define TPCMD_RIGHT_PIO_SELECT_OFF      0x5628
#define TPCMD_RIGHT_PIO_VALID_ON   	    0x5629
#define TPCMD_RIGHT_PIO_VALID_OFF   	0x5630
#define TPCMD_RIGHT_PIO_CS_0_ON    	    0x5631
#define TPCMD_RIGHT_PIO_CS_0_OFF    	0x5632
#define TPCMD_RIGHT_PIO_CS_1_ON    	    0x5633
#define TPCMD_RIGHT_PIO_CS_1_OFF    	0x5634
#define TPCMD_RIGHT_PIO_TR_REQ_ON   	0x5635
#define TPCMD_RIGHT_PIO_TR_REQ_OFF  	0x5636
#define TPCMD_RIGHT_PIO_BUSY_ON	  	    0x5637
#define TPCMD_RIGHT_PIO_BUSY_OFF	  	0x5638
#define TPCMD_RIGHT_PIO_COMPT_ON	  	0x5639
#define TPCMD_RIGHT_PIO_COMPT_OFF	  	0x5640
#define TPCMD_RIGHT_PIO_ALL_OFF         0x5641


//** Steering **
#define TPCMD_STEER_LEFT                0x5642
#define TPCMD_STEER_RIGHT               0x5643
#define TPCMD_STEER_F_FREE              0x5644
#define TPCMD_STEER_F_ENABLE            0x5645
#define TPCMD_STEER_R_FREE              0x5646
#define TPCMD_STEER_R_ENABLE            0x5647

#define TPCMD_TRACTION_STEER_LEFT       0x5648
#define TPCMD_TRACTION_STEER_RIGHT      0x5649

//** Shutter **
#define TPCMD_SHUTTER_ALARM_RST         0x5650
#define TPCMD_SHUTTER_CLOSE             0x5651
#define TPCMD_SHUTTER_OPEN              0x5652
#define TPCMD_SHUTTER_ORIGIN            0x5653
#define TPCMD_SHUTTER_FREE              0x5654
#define TPCMD_SHUTTER_ENABLE            0x5655

//** Rail Support **
#define TPCMD_RAILSUPPORT_WORK          0x5656
#define TPCMD_RAILSUPPORT_HOME          0x5657
#define TPCMD_RAILSUPPORT_FREE          0x5658
#define TPCMD_RAILSUPPORT_ENABLE        0x5659

//** MC Control **
#define TPCMD_MC_CONTROL_ON             0x5660
#define TPCMD_MC_CONTROL_OFF            0x5661

//** STB I/F Work **
#define TPCMD_STB_R_WORK_ON             0x5662
#define TPCMD_STB_R_WORK_OFF            0x5663
#define TPCMD_STB_L_WORK_ON             0x5664
#define TPCMD_STB_L_WORK_OFF            0x5665

//** OBSTACLE **
#define TPCMD_OBSTACLE_AREA1            0x5666
#define TPCMD_OBSTACLE_AREA2            0x5667
#define TPCMD_OBSTACLE_AREA3            0x5668
#define TPCMD_OBSTACLE_AREA4            0x5669
#define TPCMD_OBSTACLE_AREA5            0x566A
#define TPCMD_OBSTACLE_AREA6            0x566B
#define TPCMD_OBSTACLE_AREA7            0x566C
#define TPCMD_OBSTACLE_AREA8            0x566D
#define TPCMD_OBSTACLE_AREA9            0x566E
#define TPCMD_OBSTACLE_AREA10           0x566F
#define TPCMD_OBSTACLE_AREA11           0x5670
#define TPCMD_OBSTACLE_AREA12           0x5671
#define TPCMD_OBSTACLE_AREA13           0x5672
#define TPCMD_OBSTACLE_AREA14           0x5673
#define TPCMD_OBSTACLE_AREA15           0x5674

#define TPCMD_OBSTACLE_POWER_ON         0x5681
#define TPCMD_OBSTACLE_POWER_OFF        0x5682
#define TPCMD_OBSTACLE_POWER_RESET      0x5683
#define TPCMD_OBSTACLE_ENABLE           0x5684

//** OHT Detect **
#define TPCMD_OHTDETECT_AREA1           0x5685
#define TPCMD_OHTDETECT_AREA2           0x5686
#define TPCMD_OHTDETECT_AREA3           0x5687
#define TPCMD_OHTDETECT_AREA4           0x5688
#define TPCMD_OHTDETECT_AREA5           0x5689
#define TPCMD_OHTDETECT_AREA6           0x568A
#define TPCMD_OHTDETECT_AREA7           0x568B
#define TPCMD_OHTDETECT_AREA8           0x568C
#define TPCMD_OHTDETECT_AREA9           0x568D
#define TPCMD_OHTDETECT_AREA10          0x568E
#define TPCMD_OHTDETECT_AREA11          0x568F
#define TPCMD_OHTDETECT_AREA12          0x5690
#define TPCMD_OHTDETECT_AREA13          0x5691
#define TPCMD_OHTDETECT_AREA14          0x5692
#define TPCMD_OHTDETECT_AREA15          0x5693

#define TPCMD_OHTDETECT_POWER_ON        0x5700
#define TPCMD_OHTDETECT_POWER_OFF       0x5701
#define TPCMD_OHTDETECT_POWER_RESET     0x5702
#define TPCMD_OHTDETECT_ENABLE          0x5703

//** Lookdown Sensor **
#define TPCMD_LOOKDOWN_AREA1            0x5704
#define TPCMD_LOOKDOWN_AREA2            0x5705
#define TPCMD_LOOKDOWN_AREA3            0x5706
#define TPCMD_LOOKDOWN_AREA4            0x5707
#define TPCMD_LOOKDOWN_AREA5            0x5708
#define TPCMD_LOOKDOWN_AREA6            0x5709
#define TPCMD_LOOKDOWN_AREA7            0x570A
#define TPCMD_LOOKDOWN_AREA8            0x570B
#define TPCMD_LOOKDOWN_AREA9            0x570C
#define TPCMD_LOOKDOWN_AREA10           0x570D
#define TPCMD_LOOKDOWN_AREA11           0x570E
#define TPCMD_LOOKDOWN_AREA12           0x570F
#define TPCMD_LOOKDOWN_AREA13           0x5710
#define TPCMD_LOOKDOWN_AREA14           0x5711
#define TPCMD_LOOKDOWN_AREA15           0x5712
#define TPCMD_LOOKDOWN_POWER_ON         0x5713
#define TPCMD_LOOKDOWN_POWER_OFF        0x5714
#define TPCMD_LOOKDOWN_POWER_RESET      0x5715
#define TPCMD_LOOKDOWN_ENABLE           0x5716
#define TPCMD_LOOKDOWN_DISABLE          0x5717

//** Servo Alarm Reset **
#define TPCMD_SERVO_DRV_F_RESET         0x5724
#define TPCMD_SERVO_DRV_R_RESET         0x5725
#define TPCMD_SERVO_HOIST_RESET         0x5726
#define TPCMD_SERVO_SHIFT_RESET         0x5727
#define TPCMD_SERVO_ROTATE_RESET        0x5728
#define TPCMD_SERVO_RESET               0x5729

#define TPCMD_DRIVING_BRK_RELEASE       0x5730
#define TPCMD_DRIVING_BRK_LOCK          0x5731

#define TPCMD_SAFETY_BD_CLEAR           0x5732

#define TPCMD_SOUND_ON                  0x5733
#define TPCMD_SOUND_OFF                 0x5734

#define TPCMD_TRANS_BCR_READ            0x5735
#define TPCMD_DRIVING_BCR_READ          0x5835

#define TPCMD_LEFT_QRCODE_READ          0x5836
#define TPCMD_RIGHT_QRCODE_READ         0x5837

#define TPCMD_DRIVING_F_SERVO_ON        0x5736
#define TPCMD_DRIVING_F_SERVO_OFF       0x5737
#define TPCMD_DRIVING_R_SERVO_ON        0x5738
#define TPCMD_DRIVING_R_SERVO_OFF       0x5739

#define TPCMD_HOIST_SERVO_ON            0x5740
#define TPCMD_HOIST_SERVO_OFF           0x5741
#define TPCMD_SHIFT_SERVO_ON            0x5742
#define TPCMD_SHIFT_SERVO_OFF           0x5743
#define TPCMD_ROTATE_SERVO_ON           0x5744
#define TPCMD_ROTATE_SERVO_OFF          0x5745

#define TPCMD_ALL_SERVO_ON              0x5746
#define TPCMD_ALL_SERVO_OFF             0x5747

//** Rotate ***

#define TPCMD_ROTATE_0		            0x5748
#define TPCMD_ROTATE_90	    	        0x5749
#define TPCMD_ROTATE_180                0x5750
#define TPCMD_ROTATE_270                0x5751
#define TPCMD_ROTATE_HOMING	            0x5752
#define TPCMD_ROTATE_RESET	            0x5753
#define TPCMD_ROTATE_FREE	            0x5754
#define TPCMD_ROTATE_ENABLE   	        0x5755

#define TPCMD_SHUTTER_MID_CLOSE         0x5756

// 2018.12.21 대차/장애물 센서 데이터 취득
#define TPCMD_DISTANCE_SENSOR_REQUEST_STOP            0x5757
#define TPCMD_PBS_SENSOR_REQUEST_START                0x5758
#define TPCMD_UBG_SENSOR_REQUEST_START                0x5759
#define TPCMD_OBS_SENSOR_REQUEST_START                0x575a
#define TPCMD_UBG_SOSLAB_SENSOR_REQUEST_START         0x575c  // SOSLAB 센서
#define TPCMD_OBS_SOSLAB_SENSOR_REQUEST_START         0x575d  // SOSLAB 센서


#define TPCMD_STEER_FRONT_LEFT          0x576a
#define TPCMD_STEER_FRONT_RIGHT         0x576b
#define TPCMD_STEER_REAR_LEFT           0x576c
#define TPCMD_STEER_REAR_RIGHT          0x576d

#define TPCMD_LSTB_TEACHING_MEASURE     0x576e
#define TPCMD_RSTB_TEACHING_MEASURE     0x576f

#define TPCMD_GET_BLUETOOTH_INFO        0x577a
#define TPCMD_BLUETOOTH_SET_WAIT        0x577b
#define TPCMD_BLUETOOTH_SW_RESET        0x577c
#define TPCMD_BLUETOOTH_HW_RESET        0x577d
#define TPCMD_VMS_CONNECT		        0x577e
#define TPCMD_VMS_DISCONNECT	        0x577f

#define IO_TPCMD_END                    0x57ff

// -------------------------------------------
// Assistant 관련 Command
// -------------------------------------------
#define TPCMD_AST_OHT_MAINPROG_EXEC     0x5900 ///< OHT 프로그램 실행
#define TPCMD_AST_OHT_MAINPROG_CLOSE    0x5901 ///< OHT 프로그램 종료
#define TPCMD_AST_ASSISTANTPROG_EXEC    0x5902 ///< Assistant 실행
#define TPCMD_AST_ASSISTANTPROG_CLOSE   0x5903 ///< Assistant 종료
#define TPCMD_AST_OHT_SCREENT_CATPURE   0x5904
#define TPCMD_AST_OHT_FILE_EXEC         0x5905
#define TPCMD_AST_OHT_FILE_CLOSE        0x5906
#define TPCMD_AST_OHT_FILE_DELETE       0x5907
#define TPCMD_AST_OHT_FOLDER_CREATE     0x5908
#define TPCMD_AST_OHT_FOLDER_REMOVE     0x5909

#define TPRET_AST_OHT_MAINPROG_EXEC		0x5910
#define TPRET_AST_OHT_MAINPROG_CLOSE	0x5911

//2013.12.16 doori.shin FBWF 추가
#define TPCMD_AST_OHT_REQ_FBWF_STATUS	0x5501
#define TPCMD_AST_OHT_REQ_FBWF_ENABLE	0x5502
#define TPCMD_AST_OHT_REQ_FBWF_DISABLE	0x5503
#define TPRET_AST_OHT_ACK_FBWF_STATUS	0x5501

//For check if LogDeleter.bat file exists
#define TPCMD_AST_OHT_REQ_LOGDELETER_EXIST	0x5601
#define TPRET_AST_OHT_ACK_LOGDELETER		0x5601

//Additional Status
#define TPCMD_OHT_ADDITIONAL_STATUS		0x5701

#define TPCMD_OHT_TIMESETTING       0x5801
#define TPRET_OHT_TIMESETTING       0x5802


// **  CID **
#define TPCMD_CID_LEFT_SEL	            0x5761
#define TPCMD_CID_RIGHT_SEL	            0x5762
#define TPCMD_CID_OCCUPREQ	            0x5763
#define TPCMD_CID_PASSCOMPT   	       	0x5764
#define TPCMD_CID_MANUAL              	0x5765
#define TPCMD_CID_OPERATION             0x5766
#define TPCMD_CID_RESERVE_1             0x5767
#define TPCMD_CID_RESERVE_2             0x5768

#define TPCMD_CID_LEFT_SEL_OFF	            0x5771
#define TPCMD_CID_RIGHT_SEL_OFF	            0x5772
#define TPCMD_CID_OCCUPREQ_OFF	            0x5773
#define TPCMD_CID_PASSCOMPT_OFF   	       	0x5774
#define TPCMD_CID_MANUAL_OFF              	0x5775
#define TPCMD_CID_OPERATION_OFF             0x5776
#define TPCMD_CID_RESERVE_1_OFF             0x5777
#define TPCMD_CID_RESERVE_2_OFF             0x5778


#define TPCMD_CID_MONITORING_CMD            0x5780	// Monitoring M Command Send
#define TPCMD_CID_MONITORING_READ           0x5781	// Monitoring M Result Read

// ** Firmware Update **
#define TPCMD_FWU_UPDATE_CMD				0x5790	// 업데이트 지시패킷 Send 
#define TPCMD_FWU_VERSION_REQ				0x5791	// 버전 요청

// 2016.05.25  통과허가 강제개방 명령 추가
#define TPCMD_CID_FORCE_OPEN_CMD			0x5800

#define TPCMD_AUTOTEACHING_FILE           0x5900	// Monitoring M Result Read
#define TPCMD_AUTOTEACHING		          0x5901	// Monitoring M Result Read
#define TPCMD_MAPMAKER_FILE	           	  0x5902	// Monitoring M Result Read
#define TPCMD_AUTOTEACHING_DRIVING_READ	  0x5903	// Monitoring M Result Read
#define TPCMD_AUTOTEACHING_NULLTYPE       0x5904	// Monitoring M Result Read

#define TPCMD_OHT_VIB_DATA_LINK				0x5A05	// TP에서 OHT에 AVS Link 요청
#define TPCMD_OHT_VIB_DATA_REQ_START		0x5A06	// TP에서 OHT에 진동 DATA 전송 시작 요청
#define TPCMD_OHT_VIB_DATA_REQ_STOP			0x5A07	// TP에서 OHT에 진동 DATA 전송 중단 요청
#define TPCMD_OHT_VIB_DATA_CLOSE			0x5A08	// TP에서 OHT에 진동 AVS Close 요청
#define TPCMD_OHT_VIB_DATA_ALIVE	0x5A09  // Main에게 연결상태 확인알림(TP종료나 AVS폼 꺼지면 안보냄)


//---------------------------------------------------------------------------

/**
@brief   Use enum 변수
@author  zzang9un
@date    2013.08.17
*/
typedef enum USE_SET_
{
    USE_SET_BCR               = 0,
    USE_SET_PBSSENSOR         ,
    USE_SET_SOUNDALARM        ,
    USE_SET_LAMPINDI          ,
    USE_SET_SHUTTERCONTROL    ,
    USE_SET_EQPIO             ,
    USE_SET_STBSEN            ,
    USE_SET_HANDLIMITSENSOR   ,
    USE_SET_HANDCONTROL       ,
    USE_SET_STEERINGCONTROL   ,
    USE_SET_UTBSEN            ,
    USE_SET_LOOKDOWNSENSOR    ,
    USE_SET_HANDPUSHSENCHK    ,
    USE_SET_GRIPPERSEN        ,
    USE_SET_HOISTHOMESENSOR   ,
	USE_SET_OSCILLATIONSEN    ,
    USE_SET_ERRSOUND          ,
    USE_SET_DRIVINGSOUND      ,
    USE_SET_TRANSSOUND        ,
    USE_SET_OBSDETECTSOUND    ,
	USE_SET_OHTDETECTSOUND	  ,
    USE_SET_PIOIGNOREERROR    ,
    USE_SET_HOISTHOME_PIOERR  ,
    USE_SET_OBSIGNORESOUND	  ,
	USE_SET_LOCSAVE_TRANS_ING ,
	USE_SET_PUSHABNORMALONCHK ,
	USE_SET_UTBFOUPCHECK      ,
	USE_SET_INNERFOUPDETECTSEN,
	USE_SET_TRANSEQSOUND     ,
	USE_SET_TRANSSTKSOUND    ,
	USE_SET_TRANSSTBSOUND    ,
	USE_SET_NEXTMOTION_34AXIS,
	USE_SET_OSCILLATION_PAUSE
} USE_SET;

/**
@brief   OACIS 정보
@author  puting
@date    2015.11.11
*/
typedef enum OACISCATEGORY_
{
	CATEGORY_STEER 	= 1,	//우선 처리 명령(Auto, Manual, Reset 등 _ Monitoring 정보 포함) : 자동/수동, 기동 On/off, 에러 상관없는 명령들
	CATEGORY_VHLDECT,           //첫째 명령 : Error가 없이, 자동/수동, 기동 On/off에는 상관없는 명령들 : 우선처리중 에러가 아닐때 처리가 가능한 명령들.
	CATEGORY_OBSDECT,			//둘쨰 명령 : Error가 없이 수동 상태에서, 기동 On/off에 상관없는 명령들. : 메뉴얼 동작
	CATEGORY_SHIFTHOME,          //셋째 명령 : Error가 없이, 기동 On, 자동 일떄만 처리되는 명령들.  : OCS 명령들
	CATEGORY_HOISTHOME,
	CATEGORY_SMPSVOLTAGE,

	CATEGORY_ESTOPBUMPER,
	CATEGORY_OSCILATION,
	CATEGORY_LOOKDOWN,
	CATEGORY_FOUPDETECT,
	CATEGORY_FOUPCOVER,
	CATEGORY_STBDETECT,
	CATEGORY_EQPORT

}OACISCATEGORY;


enum LINE_CHECK_INFO
{
	LINE_NONE = 0,
	LINE_WARNNING,
	LINE_ERROR,
};
//---------------------------------------------------------------------------
#endif // Def_TPCommandH


