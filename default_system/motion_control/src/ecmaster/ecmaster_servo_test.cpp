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
// Created by master on 17. 4. 12.
//
#include "ecmaster_service.h"
#include "ecat_cia402.h"
#include <stdio.h>

namespace  sephi {

    static jmp_buf return_here;
    static void SignalHandler_service( int nSignal )
    {
        longjmp (return_here, 1);
    }
    bool EcMasterSvc::runTest()
    {
        char line[2048];
        int sel = 'q';
        int slave_id = 0;

        // signal handler 등록
        sigset_t SigSet;
        int nSigNum = SIGALRM;
        sigemptyset(&SigSet);
        sigaddset(&SigSet, nSigNum);
        sigprocmask(SIG_BLOCK, &SigSet, NULL);
        signal(SIGINT, SignalHandler_service);
        signal(SIGTERM, SignalHandler_service);

        while(1){
            sleep(1);   // for nohup command, 231117 yjchoe
            if(setjmp (return_here)){
                printf("Signal Handler execute!\n");
                signal(SIGINT, SignalHandler_service);
                signal(SIGTERM, SignalHandler_service);
                break;
            }
            printf(" [0-3] select motor 0~3\n");
            //printf(" [4,5] CSP, +/- dir\n");
            //printf(" [6,7] CSV, +/- dir\n");
            //printf(" [8,9] profile position, +/- dir\n");
            //printf(" [10] PV, +/- dir\n");
            //printf(" [11] Input velocity\n");
            printf(" [a-d]. set ecat state to OP mode a:0, b:2, .. d:3\n");
            //printf(" h. Hoist Move Distance\n");
            printf(" e. show encoder values\n");
            printf(" [D,E] disable/enable servo\n");
            //printf(" I. make 10ms interrupt at PDO cycle.\n");
            printf(" s. show slave states\n");
            printf(" v. show EC-Master Version\n");
            printf(" S. Stop all\n");
            printf(" q. quit.\n");
            printf(" Z. start Master code. \n");
            printf(">> select test>");

            static int startAMC = 0;
            if(startAMC == 0) {
                //system("echo 'oht' | sudo -S gnome-terminal --working-directory=$(pwd) -e ./amc_oht");
                startAMC++;
            }

            fflush(stdout);
            fgets(line, sizeof(line), stdin);
            if (line[0] == 'q') {
                break;
            } else if (line[0] == 'e') {
                showEncoder();
                continue;
            } else if (line[0] == 'Z') {
                system("echo 'oht' | sudo -S gnome-terminal --working-directory=$(pwd) -e ./amc_oht");   //CIS
                continue;
            } else if (line[0] == 'I') {
                doDelayTest(10000);
                continue;
            } else if (line[0] == 's') {
                showSlaveStates();
                continue;
            } else if (line[0] == 'S') {
                stopAllServo();
                continue;
            } else if (line[0] == 'v') {
                showVersion();
                getversion(0);
                getversion(1);
                getversion(2);
                getversion(3);
                getversion(4);
                continue;
            } else if (line[0] >= 'a' && line[0] <= 'd' || line[0] == 'D' || line[0] == 'E') {
                auto &slave = slaves_[slave_id];
                CiA402Servo& axis = servos_[slave.context];
                if (line[0] == 'D') {
                    axis.makeSwitchOnDisabledState();
                } else if (line[0] == 'E') {
                    axis.enableServo();
                } else {
                    printf("Change axis %d to OP mode", slave_id);
                    axis.setSlaveEcatState(eEcatState_OP);
                }
                continue;
			} /*else if (line[0] == 'h') {
                runTestPP(2, 1);
            } else if (line[0] == 't') {
                ParamDownload();
            }*/
 
            sel = atoi(line);
            if (sel >= 0 && sel < 4) {
                slave_id = sel;
                printf("motor %d is selected.\n", slave_id);
            } else {
                switch (sel) {
                    //case 4: runTestCSP(slave_id, 1); break;
                    //case 5: runTestCSP(slave_id, -1); break;
                    //case 6: runTestCSV(slave_id, 1); break;
                    //case 7: runTestCSV(slave_id, -1); break;
                    //case 8: runTestPP(slave_id, 1); break;
                    //case 9: runTestPP(slave_id, -1); break;
                    //case 10: runTestPV(slave_id, 1); break;
                    //case 11: runTestresetEncoder(slave_id); break;
                    default:
                        printf("!!! wrong choice !!!\n");
                }

            }

        }
        printf("Bye~!\n");
        return 0;
    }

    bool EcMasterSvc::runTestresetEncoder(int slave_id)
    {
        // 서비스 제공 시작; OP 상태로 만듦
        auto &slave = slaves_[slave_id];
        CiA402Servo& axis = servos_[slave.context];

        axis.resetAbsEncoder();
        axis.enableUserParameter();

        SEP_LOGI("Test Code ends\n");
    }


    bool EcMasterSvc::runTestCSP(int slave_id, int dir)
    {
        // 서비스 제공 시작; OP 상태로 만듦
        SEP_LOGI("Test Code begins : %s for slave %d\n", __func__, slave_id);

        auto &slave = slaves_[slave_id];
        CiA402Servo& axis = servos_[slave.context];

        SEP_REQUIRE_RET(axis.initServo(ServoOpMode::CSP), false);

        SEP_LOGT("Current operation mode = %d\n", axis.getOperationMode());


        int32_t pos, target;
        for (int i = 0; i < 200; ++i) {
            pos = axis.getPosition();
            axis.setPositionTarget(100+pos);
            usleep(10000);
        }

        pos = axis.getPosition();

        SEP_LOGI("Test Code ends\n");
    }

    bool EcMasterSvc::runHoistCSP(int slave_id, int dir)
    {
		char line[1024] = "";
		auto &slave = slaves_[slave_id];
        CiA402Servo& axis = servos_[slave.context];

		printf(">> Input Distance[pulse], Home[Enter] Current[%d]>", axis.getPosition());
        fflush(stdout);
        fgets(line, sizeof(line), stdin);



        SEP_REQUIRE_RET(axis.initServo(ServoOpMode::CSP), false);

		int32_t target = atoi(line);
        int32_t dist = 0, pos = 0;
		int32_t value = 0;
		pos = axis.getPosition();

        SEP_LOGT("Current operation mode = %d\n", axis.getOperationMode());
		SEP_LOGI("Hoist Move, Position : %d, Target : %d", pos, target);

		while(abs(pos-target) > 100)
		{
			pos = axis.getPosition();

			if(abs(pos-target) < 10000) value = 200;
			else if(abs(pos-target) < 30000) value = 500;
			else value = 1500;

			if(target < pos)		dist = pos - value;
			else if(target > pos)	dist = pos + value;

			axis.setPositionTarget(dist);
			printf("target : %d\n", dist);
			usleep(10000);
		}
		axis.setPositionTarget(target);

        SEP_LOGI("Move Complete!\n");
    }

    bool EcMasterSvc::runTestPP(int slave_id, int dir)
    {
        char line[1024] = "";
        auto &slave = slaves_[slave_id];
        auto &slave_r = slaves_[slave_id+2];
        auto &slave_f = slaves_[slave_id+3];

        SEP_LOGI("Test Code begins : %s for slave %d\n", __func__, slave_id);
        CiA402Servo& axis = servos_[slave.context];
        axis.setCoeSdoValue(0x200A, 2, 0xb);	// P-OT
        axis.setCoeSdoValue(0x200A, 3, 0xb);	// N-OT

        CiA402Servo& axis2 = servos_[slave_r.bus_index];
        bool rear_close = axis2.getCoeSdoValue(slave_r.bus_index, 0x6000, 39);
        bool rear_open = axis2.getCoeSdoValue(slave_r.bus_index, 0x6000, 41);

        CiA402Servo& axis3 = servos_[slave_f.bus_index];
        bool front_close = axis3.getCoeSdoValue(slave_f.bus_index, 0x6000, 28);
        bool front_open = axis3.getCoeSdoValue(slave_f.bus_index, 0x6000, 30);

        // check shutter status
        if(!((!rear_close && rear_open) && (!front_close && front_open))) {
            printf("\n****************************\n");
            printf("Please check shutter status!\n");
            printf("Front Shutter : %s\n",(front_close&&!front_open ? "CLOSE": (front_open&&!front_close ? "OPEN": "UNKNOWN")));
            printf("Rear Shutter : %s\n",(rear_close&&!rear_open ? "CLOSE": (rear_open&&!rear_close ? "OPEN": "UNKNOWN")));
            printf("****************************\n");
            return true;
        }
        // 서비스 제공 시작; OP 상태로 만듦
        SEP_REQUIRE_RET(axis.initServo(ServoOpMode::PP), false);

        SEP_LOGT("Current operation mode = %d\n", axis.getOperationMode());

        printf("\n>> Input Distance[pulse], Current[%d]\n", axis.getPosition());
        printf("  x. exit\n");
        printf("  Enter. Zero position\n");
        printf(">> ");

        fflush(stdout);
        fgets(line, sizeof(line), stdin);

        if(line[0] == 'x') {
            printf("exit move.\n");
            return true;
        }
        int32_t target = atoi(line);
        if (target >= 2000000) {
            printf("Invalid Position Limit 2,000,000!, Please retry!\n\n");
            return true;
        }

        int32_t pos = axis.getPosition();
        axis.setProfilePosition(target);

        printf(">> x. Move Stop >\n");
        fflush(stdout);
        while (!axis.targetReached()) {
            fgets(line, sizeof(line), stdin);
            if (line[0] == 'x') {
                axis.quickStop();
                break;
            } else {
                printf("Position = %d\n", axis.getPosition());
            }
        }
        SEP_LOGT("new pos = %d, target_reached=%d\n", axis.getPosition(), axis.targetReached());
        SEP_LOGI("Test Code ends\n");
    }

    bool EcMasterSvc::runTestCSV(int slave_id, int dir)
    {
        auto &slave = slaves_[slave_id];
        // 서비스 제공 시작; OP 상태로 만듦
        SEP_LOGI("Test Code begins : %s for slave %d\n", __func__, slave_id);
        CiA402Servo& axis = servos_[slave.context];

        SEP_REQUIRE_RET(axis.initServo(ServoOpMode::CSV), false);

        SEP_LOGT("Current operation mode = %d\n", axis.getOperationMode());

        //int32_t vel, target;
        int32_t  target;
        for (int i = 0; i < 600; ++i) {
            target = 4*22839440 * dir;// 2019.2.18 2283944 - > 2*22839440 sj29.lee
            SEP_LOGT("curr pos = %d, target = %d, vel = %d\n", axis.getPosition(), target, axis.getVelocity());
            axis.setVelocityTarget(target);
            usleep(10000);
        }

        // stop
        axis.setVelocityTarget(0);
        while(!axis.targetReached()) usleep(10000);

        SEP_LOGI("Test Code ends\n");
    }

    int32_t EcMasterSvc::pulse2rpm(int32_t pulseval)
    {
        return 0;
    }

    int32_t EcMasterSvc::rpm2pulse(int32_t rpmval)
    {
        double numrotatesec = 0;
        int32_t resultpulse=0;
        numrotatesec = (double)rpmval/60.0;
        resultpulse = numrotatesec * pow(2,20);
        printf("rpm2pulse result: %d\n",resultpulse);
        return resultpulse;
    }

    bool EcMasterSvc::runTestPV(int slave_id, int dir)
    {
        auto &slave = slaves_[slave_id];
        // 서비스 제공 시작; OP 상태로 만듦
        SEP_LOGI("Test Code begins : %s for slave %d\n", __func__, slave_id);
        CiA402Servo& axis = servos_[slave.context];

        SEP_REQUIRE_RET(axis.initServo(ServoOpMode::PV), false);

        SEP_LOGT("Current operation mode = %d\n", axis.getOperationMode());

        axis.setACC_rate(29372); //
        axis.setDCC_rate(43058);
        axis.setMax_Velocity(73429864);//22839440

        int32_t  target=0;
 //     target = 22839440 * dir;

        do{
            printf("Input target velocity: ");
            std::cin >> target;
            std::cout<<std::endl;
            axis.setVelocityTarget(rpm2pulse(target));
            SEP_LOGT("curr pos = %d, target = %d, vel = %d, vel_demand = %d\n", axis.getPosition(), target, axis.getVelocity(), axis.getVelocityDemand());
        }while(target != 0);
 //       axis.setVelocityTarget(target);
//        for(int i = 0; i < 100; ++i) {
 //           SEP_LOGT("curr pos = %d, target = %d, vel = %d, vel_demand = %d\n", axis.getPosition(), target, axis.getVelocity(), axis.getVelocityDemand());
 //           usleep(100000);
 //       }

        // stop
//        axis.setVelocityTarget(0);
//        while(!axis.targetReached()) usleep(10000);

        SEP_LOGI("Set PV mode for test\n");
    }
/*
    bool EcMasterSvc::runTestPV(int slave_id, int dir)
    {
        auto &slave = slaves_[slave_id];
        // 서비스 제공 시작; OP 상태로 만듦
        SEP_LOGI("Test Code begins : %s for slave %d\n", __func__, slave_id);
        CiA402Servo& axis = servos_[slave.context];

        SEP_REQUIRE_RET(axis.initServo(ServoOpMode::PV), false);

        SEP_LOGT("Current operation mode = %d\n", axis.getOperationMode());

        axis.setACC_rate(10000);
        axis.setDCC_rate(10000);
        axis.setMax_Velocity(22839440);

        int32_t  target;
        target = 22839440 * dir;
        axis.setVelocityTarget(target);
        for(int i = 0; i < 100; ++i) {
            SEP_LOGT("curr pos = %d, target = %d, vel = %d, vel_demand = %d\n", axis.getPosition(), target, axis.getVelocity(), axis.getVelocityDemand());
            usleep(100000);
        }

        // stop
        axis.setVelocityTarget(0);
        while(!axis.targetReached()) usleep(10000);

        SEP_LOGI("Test Code ends\n");
    }
*/
    void EcMasterSvc::requestRecoverOpMode()
    {
        if (!recover_in_process_ && slave_present_) {
            recover_in_process_ = true;
            ECatSdoMsg msg(ECatSdoMsg::RECOVER_OP_MODE);
            msgq_->put(msg);
        }
    }

    void EcMasterSvc::stopAllServo()
    {
        printf("[[ Stopping all servos ]]\n");
        SEP_LOGD("[[ Stopping all servos ]]\n");
        for(auto& servo : servos_) {
            servo.quickStop();
        }
        usleep(100000);
    }

    void EcMasterSvc::setAllServoIntoFault()
    {
        printf("[[ Set all servos into Fault state ]]\n");
        SEP_LOGD("[[ Set all servos into Fault state ]]\n");
        for(auto& servo : servos_) {
            servo.statusWord().fault = 1;
        }
    }

    bool EcMasterSvc::showEncoder(void)
    {
        printf("[[ Encoder Values ]]\n");
        for(auto& servo : servos_) {
            double pulse_per_rot = 1 << 20;
            double gear_ratio = 1.0;
            double deg = 360.0*servo.getPosition()/(pulse_per_rot*gear_ratio);
            printf("axis[%d] : %d (%f deg) / %d pps\n", servo.getSlaveId(), servo.getPosition(), deg, servo.getVelocity());
            //printf("           status: 0x%04x, control: 0x%04x\n", servo.statusWord().ref(), servo.controlWord().command);
        }
        return 0;
    }
    void EcMasterSvc::showSlaveStates()
    {
        printf("[[ Slave states ]]\n");
        for(auto& servo : servos_) {
            printf("axis[%d] : status: 0x%04x, control: 0x%04x, %s errcode(%04x)\n",
                   servo.getSlaveId(), servo.statusWord().ref(), servo.controlWord().command,
                   servo.statusWord().getStateString(), servo.errorcode()
            );
        }
    }
    void EcMasterSvc::doDelayTest(int delay_us)
    {
        static int delay(0);
        if (delay_us == 0) {
            usleep(delay);
            delay = 0;
        } else {
            delay = delay_us;
        }
    }

    void EcMasterSvc::showVersion()
    {
        printf("=================================\n");
        printf("EC-Master Version : %d.%02d.%02d\n", VER1, VER2, VER3);
        printf("=================================\n");
    }

    void EcMasterSvc::getversion(int slave_id)
    {
        EcDWord outlen;
        if(slave_id <= 2) {
            char val[120];
            ecatCoeSdoUploadEx(slave_id, 0x100A, 0x0, (uint8_t*)&val, sizeof(val), &outlen, COE_SDO_TIMEOUT_MS, 0);
            printf("Slave[%d] : %s\n", slave_id, val);
            printf("=================================\n");
        } else if (slave_id >= 3) {
            int32_t val;
            ecatCoeSdoUploadEx(slave_id, 0x9000, 0x1, (uint8_t*)&val, sizeof(val), &outlen, COE_SDO_TIMEOUT_MS, 0);
            printf("Slave[%d] : 0x%x\n", slave_id, val);
            printf("=================================\n");
        }
    }

    void EcMasterSvc::writeEcmVersion()
    {
        //VERSION_ECM - ecmaster 버전정보
        string filename;
        filename = "EcmVersion.txt";
        std::ofstream fout(filename);
        fout << VERSION_ECM << std::endl;
    }

    void EcMasterSvc::writeSlaveVersion()
    {
        string filename;
        filename = "SlaveVersion.txt";
        std::ofstream fout(filename);
        EcDWord outlen;

        // ecmaster
        fout << "EC-Master : " << VERSION_ECM << std::endl;

        for (int i = 0; i < 5; i++) {
            if(i <= 2) {
                // RSA F/W
                char val[120];
                ecatCoeSdoUploadEx(i, 0x100A, 0x0, (uint8_t*)&val, sizeof(val), &outlen, COE_SDO_TIMEOUT_MS, 0);
                fout << "Slave Axis[" << i << "] : " << val << std::endl;
            } else if (i >= 3) {
                // Slave B/D F/W
                int32_t val;
                ecatCoeSdoUploadEx(i, 0x9000, 0x1, (uint8_t*)&val, sizeof(val), &outlen, COE_SDO_TIMEOUT_MS, 0);
                fout << "Slave Axis[" << i << "] : " << std::hex << val << std::endl;
            }
        }
        fout << std::endl;
        fout << "*V8.2 // 0-2Axis : RSA Servo, 3-4Axis : Slave B/D" << std::endl;
    }
}
