#-*- coding:utf-8 -*-
# COE dictionary addresses
class COE:
    ERROR_REG    = 0x1001
    STORE_PARAM  = 0x1010   # 1: save all 2: save comm. 3: save app. 4: save manufacturer.
    RESTORE_PARAM= 0x1011  # STORE_PARAM과 동일
    SM_RX_PDO_ASSIGN = 0x1C12 # 초기값 x1601
    SM_TX_PDO_ASSIGN = 0x1C13 # 초기값 x1A01
    
    SERVO_PARAM     = 0x2000 # 0x2000~0x26ff
    POWER_DC        = 0x2001 # 0Xdddd
    ABS_ENC_SETUP   = 0x2002 # 절대치 엔코더 사용 d0dd = 다회전 절대치, d1dd = 증분치, d2dd = 1회전 절대치
    CURR_CTRL_MODE  = 0x2009 # dd0d = 전류 제어1 , dd2d 저소음 모드
    POWER_3P        = 0x200B # 0Xddd
    NO_TUNE         = 0x2170 # 무조정 기능 register
    NO_TUNE_TYPE    = 0x214F # 무조정 type
    ELEC_GEAR_NUM   = 0x220E # 전자 기어비 분자
    ELEC_GEAR_DEN   = 0x2210 # 전자 기어비 분모
    ENCODER_PULSE   = 0x2212 # 엔코더 분주 펄스
    OVERTRAVEL_P    = 0x250A # 정방향 over travel
    OVERTRAVEL_N    = 0x250B # 역방향 over travel
    OVER_DISPLACEMENT_ALARM  = 0x2520 # 위치 과대 편차 알람
    USER_PARAM_CONFIG   = 0x2700
    POS_UNIT        = 0x2701
    VEL_UNIT        = 0x2702
    ACC_UNIT        = 0x2703
    TRQ_UNIT        = 0x2704
    SERVO_ADJUST    = 0x2710 # servopack adjusting command

    ALARM_CODE        = 0x603F
    CONTROL_WORD    = 0x6040
    STATUS_WORD     = 0x6041
    QSTOP_OPTION    = 0x605A   # 0~4; 2
    SHUTDOWN_OPTION = 0x605B# 0~1; 0
    DISABLE_OPTION  = 0x605C # 0~1; 1
    HALT_OPTION     = 0x605D  # 0~4; 1
    MODE_OF_OPERATION = 0x6060# 0~10; 0
    MODE_OF_OPERATION_DISP = 0x6061
    POS_DEMAND   = 0x6062   # pos unit
    POS_ACTUAL   = 0x6064   # pos unit
    FOLLOWING_ERROR_WINDOW = 0x6065 # pos unit
    POS_WINDOW   = 0x6067   # pos unit; 30
    VEL_DEMAND   = 0x606B   # vel unit
    VEL_ACTUAL   = 0x606C   # vel unit
    VEL_WINDOW   = 0x606D   # vel unit
    TRQ_TARGET   = 0x6071   # trq unit
    TRQ_MAX      = 0x6072   # trq unit
    TRQ_DEMAND   = 0x6074   # trq unit
    RATED_TRQ    = 0x6076   # mN
    TRQ_ACTUAL   = 0x6077   # trq unit
    POS_TARGET   = 0x607A   # pos unit
    HOME_OFFSET  = 0x607C   # pos unit
    POS_LIMIT    = 0x607D   # pos unit.  1: min 2: max
    PROF_VEL_MAX = 0x607F   # vel unit
    PROF_VEL     = 0x6081   # vel unit
    PROF_ACC     = 0x6083   # acc unit
    PROF_DEC     = 0x6084   # acc unit
    QSTOP_DEC    = 0x6085   # acc unit
    TRQ_SLOPE    = 0x6087   # trq unit / s
    HOME_METHOD  = 0x6098   # 0~35; 35
    VEL_OFFSET   = 0x60B1   # vel unit
    TRQ_OFFSET   = 0x60B2   # TRQ UNIT -2^15~2^15
    TOUCH_FUNC   = 0x60B8   # 0~0xFFFF; 0
    FOLLOWING_ERROR_ACTUAL = 0x60F4 # pos unit
    DIGITAL_INP  = 0x60FD # UDINT
    DIGITAL_OUT  = 0x60FE # 1: output data 2: bitmask
    VEL_TARGET   = 0x60FF # vel unit

class STORE_OPTION: # STORE_OPTIONS : uint8_t
    SAVE_ALL_PARAM = 1
    SAVE_COMM_PARAM = 2    # 0x1000~0x1FFF
    SAVE_APP_PARAM = 3     # 0x2700~0x6FFF
    SAVE_SERVO_PARAM = 4   # 0x2000~0x26FF
    RESTORE_ALL_PARAM = 1
    RESTORE_COMM_PARAM = 2    # 0x1000~0x1FFF
    RESTORE_APP_PARAM = 3     # 0x2700~0x6FFF
    RESTORE_SERVO_PARAM = 4   # 0x2000~0x26FF

class COE_TYPE:
    SINT8 = 0
    UINT8 = 1
    SINT16 = 2
    UINT16 = 3
    SINT32 = 4
    UINT32 = 5