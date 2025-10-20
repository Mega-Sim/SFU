#!/usr/bin/env python3
# -*- coding:utf-8 -*-
import yaml
import os, sys
from datetime import datetime
from functools import reduce
from base_simulator import *


oht_log_file_name = 'oht_sig_log.csv'


class OHTSignalLogger(Simulator):
    windows_size = 1000
    update_cycle = 100
    speed_log_size = int(10*1000/update_cycle) # about 20sec when update cycle = 50ms
    groups = {
        'driving': [
            "_obstacle_long",
            "_obstacle_middle",
            "_obstacle_short",
            "_obstacle_fail",
            "_ohtdetect_long",
            "_ohtdetect_middle",
            "_ohtdetect_short",
            "_ohtdetect_fail",
            "_stop_driving_precheck",
            "_stop_driving_front",
            "_stop_trans_front",
            "_stop_trans_rear",
            "steer_front_left",
            "steer_front_right",
            "steer_rear_left",
            "steer_rear_right",
            # do
            "_obstacle_area1_sel",
            "_obstacle_area2_sel",
            "_obstacle_area3_sel",
            "_obstacle_area4_sel",
            "_ohtdetect_area1_sel",
            "_ohtdetect_area2_sel",
            "_ohtdetect_area3_sel",
            "_ohtdetect_area4_sel",
            "steer_front_dir",
            "steer_front_torque0",
            "steer_front_torque1",
            "steer_rear_dir",
            "steer_rear_torque0",
            "steer_rear_torque1",
            "driving_brake_release",
        ],

        'trans': [
            "slide_home_sensor",
            "_hoist_home_sensor",
            "_foup_detect",
            "_lookdown_detect",
            "_lookdown_error",
            # do
            "_lookdown_power_reset",
            "_lookdown_area1_sel",
            "_lookdown_area2_sel",
            "_lookdown_area3_sel",
            "_lookdown_area4_sel",
            "hoist_ext_brake_release",
            "bcr_trans_ok",
            "bcr_trans_ng",
            "bcr_driving_ok",
            "bcr_driving_ng",
        ],

    'hand': [
            "hand_open_sen",
            "hand_close_sen",
            "hand_gripper_left",
            "hand_gripper_right",
            "hand_push_sen",
            "hand_inposition",
            "hand_alarm",
            "hand_pio_go",
            # do
            "hand_pio_select",
            "hand_reset",
            "hand_close",
            "hand_open",
            "hand_origin",
            "hand_free",
        ],
        'shutter': [
            "shutter_front_open_sen",
            "shutter_front_close_sen",
            "shutter_front_inposition",
            "shutter_front_alarm",
            "shutter_rear_open_sen",
            "shutter_rear_close_sen",
            "shutter_rear_inposition",
            "shutter_rear_alarm",
            # do
            "shutter_front_reset",
            "shutter_front_close",
            "shutter_front_open",
            "shutter_front_origin",
            "shutter_front_free",
            "shutter_rear_reset",
            "shutter_rear_close",
            "shutter_rear_open",
            "shutter_rear_origin",
            "shutter_rear_free",
        ],
        'pio': [
            "eq_pio_load_req",
            "eq_pio_unload_req",
            "eq_pio_ho_avbl",
            "eq_pio_es",
            "eq_pio_right_go",
            "eq_pio_left_go",
            "eq_pio_ready",
            # do
            "eq_pio_cs0",
            "eq_pio_valid",
            "eq_pio_tr_req",
            "eq_pio_busy",
            "eq_pio_complete",
            "_eq_pio_left_select",
            "_eq_pio_right_select",
        ],
        'etc': [
            "cid_reserved2",
            "cid_reserved1",
            "outrigger_home_sen",
            "emo_press",
            "outrigger_work_sen",
            "cid_pass_ok",
            "cid_link_complete",
            "cid_status",
            "cid_interlock",
            "_utb_detect",
            "bumper_press",
            "_stb_detect_left",
            "_stb_detect_right",
            "_oscillation_sensor",
            "_hoist_belt_detect_1",
            "_hoist_belt_detect_2",
            "handtp_connect",
            "_foup_cover_open_detect",
            "_fan_fail",
            "tp_enable",
            "_smps_short_check",
            "pio_input3",
            "pio_input5",
            "_pio_input6",
            # do
            "safety_control",
            "safety_clear",
            "_hand_power_reset",
            "lamp_yellow",
            "lamp_red",
            "lamp_green",
            "cid_reserved_1",
            "cid_left_select",
            "cid_right_select",
            "cid_occup_req",
            "cid_pass_complete",
            "cid_manual",
            "cid_reserved_2",
            "utb_detect_sensor",
        ]
    }
    last_di_flags = {
        'driving': 'steer_rear_right',
        'trans': '_lookdown_error',
        'hand': 'hand_pio_go',
        'shutter': 'shutter_rear_alarm',
        'pio': 'eq_pio_ready',
        'etc': '_pio_input6',
    }

    onboard_do = [
        "_obstacle_area1_sel",
        "_obstacle_area2_sel",
        "_obstacle_area3_sel",
        "_obstacle_area4_sel",
        "_ohtdetect_area1_sel",
        "_ohtdetect_area2_sel",
        "_ohtdetect_area3_sel",
        "_ohtdetect_area4_sel",
        "servo_brake1",
        "servo_brake2",
        "servo_brake3",
        "servo_brake4",
        "limit_in1",
        "limit_in2",
        "obs_enable",
        "oht_enable",
        "qrr1_in1",
        "bcr1_in2",
        "qrr2_in3",
        "bcr2_in4",
        "driving_stop_in1",
        "trans_stop_in2" ]
    onboard_di = [
        "_obstacle_long",
        "_obstacle_middle",
        "_obstacle_short",
        "_obstacle_fail",
        "_ohtdetect_long",
        "_ohtdetect_middle",
        "_ohtdetect_short",
        "_ohtdetect_fail",
        "_stop_driving_front",
        "_stop_driving_precheck",
        "bcr_driving_ok",
        "bcr_driving_ng",
        "_stop_trans_front",
        "_stop_trans_rear",
        "bcr_trans_ok",
        "bcr_trans_ng",
        "home_sensor1",
        "positive_limit1",
        "negative_limit1",
        "home_sensor2",
        "positive_limit2",
        "negative_limit2",
        "home_sensor3",
        "positive_limit3",
        "negative_limit3",
        "home_sensor4",
        "positive_limit4",
        "negative_limit4",
        "limit_st1",
        "limit_st2",
        "obs_status",
        "oht_status",
        "qrr1_st1",
        "bcr1_st2",
        "qrr2_st3",
        "bcr2_st4",
        "driving_stop_st1",
        "trans_stop_st2"
    ]

    all_flags = reduce(lambda x, y: x + y, groups.values())

    time_base = datetime.now()
    time_log = []
    speed_log = []


    @classmethod
    @reconnectable
    def get_status(cls):
        res = cls.stub.getOHTStatus(Empty())
        status = yaml.load(res.ymldata)
        if len(cls.time_log) == 0 :
            tick = (datetime.now()-cls.time_base).total_seconds()
            cls.time_log = [ '%.3f' % (tick + t*cls.update_cycle/1000) for t in range(-cls.speed_log_size, 0)] # 화면 업데이트 주기인 50ms 간격으로 preset
            cls.speed_log = [0 for x in range(cls.speed_log_size)]
        cls.time_log.append('%.3f' % (datetime.now()-cls.time_base).total_seconds())
        cls.speed_log.append(status['oht']['speed'])
        if len(cls.speed_log) > cls.speed_log_size:
            cls.time_log.pop(0)
            cls.speed_log.pop(0)
        return status;

    @classmethod
    def log(cls):
        cls.time_start = datetime.now()
        cls.flags = cls.all_flags
        with open(oht_log_file_name, 'w') as f:
            f.write('time,%s\n' % ','.join(cls.flags))
            while True:
                status = cls.get_status()
                time_diff = datetime.now() - cls.time_start
                f.write('%.3f,' % time_diff.total_seconds())
                f.write('%s\n' % ','.join(map(lambda flag : str(status['oht']['dio'][flag]), cls.flags)))
                f.flush()
                sleep(0.01)


if __name__ == '__main__':
    OHTSignalLogger.log()
