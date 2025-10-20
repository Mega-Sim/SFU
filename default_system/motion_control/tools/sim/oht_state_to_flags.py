'''
	src/ecmaster/emul/oht/oht_simulator.cpp:getStatusAsYaml() 함수의 key 값들을 
	tools/sim/oht_signal_view.py 에서 flag list로 뽑아내기 위한 script
'''

flags = '''                                // servo & power
                __ADD_KEY_VALUE(safety_control, front_di_->safety_control);
                __ADD_KEY_VALUE(safety_clear, front_di_->safety_clear);
                __ADD_KEY_VALUE(_hand_power_reset, front_di_->_hand_power_reset);         // (1,3)

                // driving io
                __ADD_KEY_VALUE(_stop_driving_precheck, onboard_do_->_stop_driving_precheck);
                __ADD_KEY_VALUE(_stop_driving_front, onboard_do_->_stop_driving_front);
                __ADD_KEY_VALUE(_stop_trans_front, onboard_do_->_stop_trans_front);
                __ADD_KEY_VALUE(_stop_trans_rear, onboard_do_->_stop_trans_rear);
                __ADD_KEY_VALUE(steer_front_left, front_do_->steer_front_left);
                __ADD_KEY_VALUE(steer_front_right, front_do_->steer_front_right);
                __ADD_KEY_VALUE(steer_rear_left, rear_do_->steer_rear_left);
                __ADD_KEY_VALUE(steer_rear_right, rear_do_->steer_rear_right);

                // trans in
                __ADD_KEY_VALUE(slide_home_sensor, front_do_->slide_home_sensor);
                __ADD_KEY_VALUE(_hoist_home_sensor, rear_do_->_hoist_home_sensor);
                __ADD_KEY_VALUE(hand_open_sen, rear_do_->hand_open_sen);
                __ADD_KEY_VALUE(hand_close_sen, rear_do_->hand_close_sen);
                __ADD_KEY_VALUE(hand_gripper_left, rear_do_->hand_gripper_left);
                __ADD_KEY_VALUE(hand_gripper_right, rear_do_->hand_gripper_right);
                __ADD_KEY_VALUE(hand_push_sen, rear_do_->hand_push_sen);
                __ADD_KEY_VALUE(hand_inposition, rear_do_->hand_inposition);
                __ADD_KEY_VALUE(hand_alarm, rear_do_->hand_alarm);
                __ADD_KEY_VALUE(hand_pio_go, rear_do_->hand_pio_go);
                __ADD_KEY_VALUE(shutter_front_open_sen, front_do_->shutter_front_open_sen);
                __ADD_KEY_VALUE(shutter_front_close_sen, front_do_->shutter_front_close_sen);
                __ADD_KEY_VALUE(shutter_front_inposition, front_do_->shutter_front_inposition);
                __ADD_KEY_VALUE(shutter_front_alarm, front_do_->shutter_front_alarm);
                __ADD_KEY_VALUE(shutter_rear_open_sen, rear_do_->shutter_rear_open_sen);
                __ADD_KEY_VALUE(shutter_rear_close_sen, rear_do_->shutter_rear_close_sen);
                __ADD_KEY_VALUE(shutter_rear_inposition, rear_do_->shutter_rear_inposition);
                __ADD_KEY_VALUE(shutter_rear_alarm, rear_do_->shutter_rear_alarm);
                __ADD_KEY_VALUE(_foup_detect, front_do_->_foup_detect);
                __ADD_KEY_VALUE(_lookdown_detect, front_do_->_lookdown_detect);
                __ADD_KEY_VALUE(_lookdown_error, front_do_->_lookdown_error);

                // trans out
                __ADD_KEY_VALUE(steer_front_dir, front_di_->steer_front_dir);
                __ADD_KEY_VALUE(steer_front_torque0, front_di_->steer_front_torque0);
                __ADD_KEY_VALUE(steer_front_torque1, front_di_->steer_front_torque1);
                __ADD_KEY_VALUE(shutter_front_reset, front_di_->shutter_front_reset);
                __ADD_KEY_VALUE(shutter_front_close, front_di_->shutter_front_close);
                __ADD_KEY_VALUE(shutter_front_open, front_di_->shutter_front_open);
                __ADD_KEY_VALUE(shutter_front_origin, front_di_->shutter_front_origin);
                __ADD_KEY_VALUE(shutter_front_free, front_di_->shutter_front_free);
                __ADD_KEY_VALUE(shutter_rear_reset, rear_di_->shutter_rear_reset);
                __ADD_KEY_VALUE(shutter_rear_close, rear_di_->shutter_rear_close);
                __ADD_KEY_VALUE(shutter_rear_open, rear_di_->shutter_rear_open);
                __ADD_KEY_VALUE(shutter_rear_origin, rear_di_->shutter_rear_origin);
                __ADD_KEY_VALUE(shutter_rear_free, rear_di_->shutter_rear_free);
                __ADD_KEY_VALUE(hand_reset, rear_di_->hand_reset);
                __ADD_KEY_VALUE(hand_close, rear_di_->hand_close);
                __ADD_KEY_VALUE(hand_open, rear_di_->hand_open);
                __ADD_KEY_VALUE(hand_origin, rear_di_->hand_origin);
                __ADD_KEY_VALUE(hand_free, rear_di_->hand_free);
                __ADD_KEY_VALUE(_lookdown_power_reset, front_di_->_lookdown_power_reset);
                __ADD_KEY_VALUE(_lookdown_area2_sel, front_di_->_lookdown_area2_sel);
                __ADD_KEY_VALUE(_lookdown_area3_sel, front_di_->_lookdown_area3_sel);
                __ADD_KEY_VALUE(_lookdown_area4_sel, front_di_->_lookdown_area4_sel);

                // eqpio
                __ADD_KEY_VALUE(eq_pio_load_req, rear_do_->eq_pio_load_req);
                __ADD_KEY_VALUE(eq_pio_unload_req, rear_do_->eq_pio_unload_req);
                __ADD_KEY_VALUE(eq_pio_ho_avbl, rear_do_->eq_pio_ho_avbl);
                __ADD_KEY_VALUE(eq_pio_es, rear_do_->eq_pio_es);
                __ADD_KEY_VALUE(eq_pio_right_go, rear_do_->eq_pio_right_go);
                __ADD_KEY_VALUE(eq_pio_left_go, rear_do_->eq_pio_left_go);
                __ADD_KEY_VALUE(eq_pio_ready, rear_do_->eq_pio_ready);
                __ADD_KEY_VALUE(eq_pio_cs0, rear_di_->eq_pio_cs0);
                __ADD_KEY_VALUE(eq_pio_valid, rear_di_->eq_pio_valid);
                __ADD_KEY_VALUE(eq_pio_tr_req, rear_di_->eq_pio_tr_req);
                __ADD_KEY_VALUE(eq_pio_busy, rear_di_->eq_pio_busy);
                __ADD_KEY_VALUE(eq_pio_complete, rear_di_->eq_pio_complete);
                __ADD_KEY_VALUE(_eq_pio_left_select, rear_di_->_eq_pio_left_select);
                __ADD_KEY_VALUE(_eq_pio_right_select, rear_di_->_eq_pio_right_select);

                // detect sensors
                __ADD_KEY_VALUE(_obstacle_area1_sel, onboard_di_->_obstacle_area1_sel);
                __ADD_KEY_VALUE(_obstacle_area2_sel, onboard_di_->_obstacle_area2_sel);
                __ADD_KEY_VALUE(_obstacle_area3_sel, onboard_di_->_obstacle_area3_sel);
                __ADD_KEY_VALUE(_obstacle_area4_sel, onboard_di_->_obstacle_area4_sel);
                __ADD_KEY_VALUE(_ohtdetect_area1_sel, onboard_di_->_ohtdetect_area1_sel);
                __ADD_KEY_VALUE(_ohtdetect_area2_sel, onboard_di_->_ohtdetect_area2_sel);
                __ADD_KEY_VALUE(_ohtdetect_area3_sel, onboard_di_->_ohtdetect_area3_sel);
                __ADD_KEY_VALUE(_ohtdetect_area4_sel, onboard_di_->_ohtdetect_area4_sel);

                __ADD_KEY_VALUE(_obstacle_long, onboard_do_->_obstacle_long);
                __ADD_KEY_VALUE(_obstacle_middle, onboard_do_->_obstacle_middle);
                __ADD_KEY_VALUE(_obstacle_short, onboard_do_->_obstacle_short);
                __ADD_KEY_VALUE(_obstacle_fail, onboard_do_->_obstacle_fail);
                __ADD_KEY_VALUE(_ohtdetect_long, onboard_do_->_ohtdetect_long);
                __ADD_KEY_VALUE(_ohtdetect_middle, onboard_do_->_ohtdetect_middle);
                __ADD_KEY_VALUE(_ohtdetect_short, onboard_do_->_ohtdetect_short);
                __ADD_KEY_VALUE(_ohtdetect_fail, onboard_do_->_ohtdetect_fail);

                __ADD_KEY_VALUE(lamp_yellow, front_di_->lamp_yellow);
                __ADD_KEY_VALUE(lamp_red, front_di_->lamp_red);
                __ADD_KEY_VALUE(lamp_green, front_di_->lamp_green);
                __ADD_KEY_VALUE(cid_reserved_1, front_di_->cid_reserved_1);
                __ADD_KEY_VALUE(cid_left_select, front_di_->cid_left_select);
                __ADD_KEY_VALUE(cid_right_select, front_di_->cid_right_select);
                __ADD_KEY_VALUE(cid_occup_req, front_di_->cid_occup_req);
                __ADD_KEY_VALUE(cid_pass_complete, front_di_->cid_pass_complete);
                __ADD_KEY_VALUE(cid_manual, front_di_->cid_manual);
                __ADD_KEY_VALUE(cid_reserved_2, front_di_->cid_reserved_2);
                __ADD_KEY_VALUE(utb_detect_sensor, front_di_->utb_detect_sensor);
                __ADD_KEY_VALUE(steer_front_torque0, front_di_->steer_front_torque0);
                __ADD_KEY_VALUE(steer_front_torque1, front_di_->steer_front_torque1);
                __ADD_KEY_VALUE(steer_front_dir, front_di_->steer_front_dir);


                __ADD_KEY_VALUE(cid_reserved2, front_do_->cid_reserved2);
                __ADD_KEY_VALUE(cid_reserved1, front_do_->cid_reserved1);
                __ADD_KEY_VALUE(outrigger_home_sen, front_do_->outrigger_home_sen);
                __ADD_KEY_VALUE(emo_press, front_do_->emo_press);
                __ADD_KEY_VALUE(outrigger_work_sen, front_do_->outrigger_work_sen);
                __ADD_KEY_VALUE(cid_pass_ok, front_do_->cid_pass_ok);
                __ADD_KEY_VALUE(cid_link_complete, front_do_->cid_link_complete);
                __ADD_KEY_VALUE(cid_status, front_do_->cid_status);
                __ADD_KEY_VALUE(cid_interlock, front_do_->cid_interlock);
                __ADD_KEY_VALUE(_utb_detect, front_do_->_utb_detect);
                __ADD_KEY_VALUE(bumper_press, front_do_->bumper_press);
                __ADD_KEY_VALUE(_stb_detect_left, front_do_->_stb_detect_left);
                __ADD_KEY_VALUE(_stb_detect_right, front_do_->_stb_detect_right);
                __ADD_KEY_VALUE(_oscillation_sensor, front_do_->_oscillation_sensor);

                __ADD_KEY_VALUE(hoist_ext_brake_release, rear_di_->hoist_ext_brake_release);
                __ADD_KEY_VALUE(driving_brake_release, rear_di_->driving_brake_release);
                __ADD_KEY_VALUE(steer_rear_torque0, rear_di_->steer_rear_torque0);
                __ADD_KEY_VALUE(steer_rear_torque1, rear_di_->steer_rear_torque1);
                __ADD_KEY_VALUE(steer_rear_dir, rear_di_->steer_rear_dir);

                __ADD_KEY_VALUE(_hoist_belt_detect_1, rear_do_->_hoist_belt_detect_1);
                __ADD_KEY_VALUE(_hoist_belt_detect_2, rear_do_->_hoist_belt_detect_2);
                __ADD_KEY_VALUE(handtp_connect, rear_do_->handtp_connect);
                __ADD_KEY_VALUE(_foup_cover_open_detect, rear_do_->_foup_cover_open_detect);
                __ADD_KEY_VALUE(_fan_fail, rear_do_->_fan_fail);
                __ADD_KEY_VALUE(tp_enable, rear_do_->tp_enable);
                __ADD_KEY_VALUE(_smps_short_check, rear_do_->_smps_short_check);
                __ADD_KEY_VALUE(pio_input3, rear_do_->pio_input3);
                __ADD_KEY_VALUE(pio_input5, rear_do_->pio_input5);
                __ADD_KEY_VALUE(_pio_input6, rear_do_->_pio_input6);

'''

for line in flags.split('\n'):
	line = line.strip()
	if not '__ADD_KEY_VALUE' in line : continue
	flag = line.split('(')[1].split(',')[0]
	print('"%s",' % flag)
	
