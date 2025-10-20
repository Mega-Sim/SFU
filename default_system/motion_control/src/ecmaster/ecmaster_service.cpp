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
// Created by master on 17. 4. 5.
//

#include <future>
#include <iostream>
#include <map>
#include <yaml-cpp/yaml.h>
#include <cstring>
#ifdef USE_ETHERLAB
#include <ecrt.h>
#endif
#include "ecmaster_service.h"
#include "slave_info.h"


namespace sephi {
    EcMasterSvc* the_EcMasterSvc_ins = nullptr;
    EcMasterSvc::EcMasterSvc() : noti_(nullptr), opt_(nullptr), client_id(-1), recover_in_process_(false)
        , stocker_io_(nullptr), ecat_state_(0), out_pdo(nullptr), in_pdo(nullptr)
    {
        msgq_ = std::make_unique<ECatMsgQ>(EM_MSG_Q_KEY(), true);
        shm_ = std::make_unique<SepShm>(EM_SHM_KEY(), EM_SHM_SIZE + 4);   // add servo_type
        shmforonboardio_ = std::make_unique<SepShm>(OHT_ONBOARD_IO_SHM_KEY(), 4096);
        pdo_arrival_event_ = std::make_unique<IpcCondition>(EM_CYCLIC_EVENT_KEY(), true);
        the_EcMasterSvc_ins = this;
    }

    EcMasterSvc::~EcMasterSvc()
    {
        shutdownService();
    }

//    void EcMasterSvc::checkError()
//    {
//        if (ecmaster_last_error == EcMasterError::SLAVE_MISMATCH) {
//            stopAllServo();
//            ecmaster_last_error = EcMasterError::NO_ERROR;
//        }
//    }


    void EcMasterSvc::doPdoJobs()
    {
        static uint32_t  counter = 0;
        static int32_t reset_counter = 10000;

	    if (out_pdo == nullptr) return; // not initialzied
        counter ++;

        //doDelayTest();

        checkECatStates();

#if defined(USE_ACONTIS)
//        if (ecat_state_ != eEcatState_OP) return; 20181226
#elif defined(USE_ETHERLAB)
        //if (ec_master_state_.al_states != 0x08) return;
#endif


#if 0 // for debug
        auto printbin = [](const char *tag, int num, const char* stat) {
            std::bitset<16> b(num);
            SEP_LOGT("%s.word = 0x%04x;  0b%s, %s\n", tag, num, b.to_string().c_str(), stat);
        };
        if ( servos_.size() > 0) {
            printbin("in pdo", static_cast<int>(servos_[0].statusWord()), servos_[0].statusWord().getStateString());
            printbin("out pdo", servos_[0].controlWord().command, servos_[0].controlWord().commandString());
        }
#endif

        // pdo copy
        for(const auto& slave: slaves_){
            if (slave.user_pid == 0 && slave.vendor_id != ecvendor_fine) continue; // not mapped except io slaves
            if (slave.type == CIA402_COMPLIANT_SERVO
                || slave.type == CIA402_COMPLIANT_SERVO2
                || slave.type == CIA402_COMPLIANT_SERVO_D
                || slave.type == CIA402_COMPLIANT_SERVO_T
                || slave.type == FINE_STK_IO) {   // yjchoe

                // copy in
                memcpy(shm_->address + slave.in_pd_offset, in_pdo + slave.in_pd_offset, slave.in_pd_length);
                // copy out
                memcpy(out_pdo + slave.out_pd_offset, shm_->address + EM_SHM_SECTION_SIZE + slave.out_pd_offset,
                           slave.out_pd_length);

                // fault check & reset
                if (slave.type == CIA402_COMPLIANT_SERVO
                    || slave.type == CIA402_COMPLIANT_SERVO2
                    || slave.type == CIA402_COMPLIANT_SERVO_D
                    || slave.type == CIA402_COMPLIANT_SERVO_T) {    // yjchoe
                    auto &servo = servos_[slave.context];
                    if (servo.controlWord().command == Cia402ControlWord::FAULT_RESET) {
                        reset_counter = 0; //20181226
                        //servo.followActual();
                        //((Cia402PdoOut*)(shm_->address + EM_SHM_SECTION_SIZE + slave.out_pd_offset))->position = servo.getPosition();
                    }
                    if (servo.statusWord().fault && (servo.errorcode() == 0xa11 || servo.errorcode() == 0xa12)) {
                        servo.controlWord().command = Cia402ControlWord::FAULT_RESET;
                    }
                }

                // 동기 축 속도 복사
                int32_t following_axis = slave.following_axis;  //AMOHTMTNF-78 25.02.18 
                if (following_axis != -1) {
                    auto &master_axis = servos_[slaves_[following_axis].context];
                    auto &slave_axis = servos_[slave.context];
                    int32_t slaveVelTgt = 0;
                    if (slave_axis.getOperationMode() == static_cast<int>(ServoOpMode::CSV)) {
                       //slave_axis.setVelocityTarget(master_axis.statusWord().fault ? 0 : master_axis.getVelocity());    // 210119 yjchoe
                       //slave_axis.setVelocityTarget(0);
                       if(abs(master_axis.getVelocity()) < 100 * 2) {
                           slaveVelTgt = master_axis.getVelocity(); // 일정속도 이하 도달 시 feedback velocity
                       } else {
                           slaveVelTgt = master_axis.getRefVelocity(); // 일정속도 이상 시 reference velocity
                       }
					   slave_axis.setVelocityTarget(master_axis.statusWord().fault ? 0 : slaveVelTgt);
                    } else if (slave_axis.getOperationMode() == static_cast<int>(ServoOpMode::CST)) {
                       slave_axis.setTorqueTarget(master_axis.statusWord().fault ? 0 : master_axis.getTorque());
                       // slave_axis.setTorqueTarget(0);
                    }
                }
           



            } else if (slave.type == FINE_OHT_FRONT_IO || slave.type == FINE_OHT_REAR_IO) {
                // serial-over-ethercat 데이터는 공유메모리를 통해 corrupt 되는 일이 없도록 IO데이터만 공유메모리로 복사
                // copy in
                memcpy(shm_->address + slave.in_pd_offset, in_pdo + slave.in_pd_offset, slave.in_pd_length - sizeof(ECatSerialData));
                // copy out
                memcpy(out_pdo + slave.out_pd_offset, shm_->address + EM_SHM_SECTION_SIZE + slave.out_pd_offset,
                       slave.out_pd_length - sizeof(ECatSerialData));
            }
        }

        // process serial data transaction
        doSerialDataTransaction();

        // notify new data arrival
        pdo_arrival_event_->wakeUp();
    }
 
    bool EcMasterSvc::initService(std::unique_ptr<EmCommandLineOption> opt)
    {
        // application 초기화
        // option 초기화
        opt_ = std::move(opt);

        system("sudo /etc/init.d/ser2net stop");// ser2net 종료 명령 

        // job task 초기화
        if (opt_->emulation_mode){
            em_task_ = std::make_unique<EmEmulatorTask>();
        } else {
            em_task_ = std::make_unique<EmTask>();
        }
        em_task_->set({this, opt_->dwBusCycleTimeUsec * 1000, opt_->emulation_mode});


        // stack 초기화, network 설정, ...
        SEP_LOGI("Initialize EtherCAT Master\n");
        if (!opt_->emulation_mode) {

            SEP_REQUIRE_RET(initMaster(), false);
#ifdef USE_ACONTIS
            em_task_->start();
            checkLicense();
#endif
            SEP_REQUIRE_RET(configureMaster(), false);
            SEP_REQUIRE_RET(registerClient(), false);
            SEP_REQUIRE_RET(scanNetwork(), false);
            SEP_REQUIRE_RET(changeECatState(eEcatState_INIT), false);
            SEP_REQUIRE_RET(prepareSvc(), false);
            SEP_REQUIRE_RET(changeECatState(eEcatState_PREOP), false);
            SEP_REQUIRE_RET(setupSvc(), false);
#ifdef USE_ETHERLAB
            em_task_->start();
#endif

            if (opt_->bMailboxOnly) {
                SEP_LOGT("NOTE! Mailbox communication only option is enabled.! No Process Data available.\n");
            } else {
                if (!changeECatState(eEcatState_SAFEOP)) {
                    checkDcmError();
                    //return false;
                }
                SEP_REQUIRE_RET(changeECatState(eEcatState_OP), false);
            }
            initEmulator(true); // initialize emulator as observer mode
        } else { // emulation mode
            client_id = 1; // say.
            slave_present_ = true;
            SEP_REQUIRE_RET(initEmulator(), false);
            em_task_->start();
            ecat_state_ = eEcatState_OP;
        }

        // 실시간 thread로 변경
        addIntoRealtimeSchedule(MAIN_THREAD_PRIO);

        // SDO message handle task start
        startMessageHandler();

        //system("echo 'master123' | sudo -S gnome-terminal --working-directory=$(pwd)/src/app/amc -e ./amc_oht");   //CIS
        return true;
    }

    void EcMasterSvc::shutdownService()
    {
        // 서비스 중단, 리소스 해제
        if (client_id == -1) {
            return; // already shutdown
        }
        /* stop servo clients */
        stopAllServo();

        // cleanup ethercat serial
        stopSerialOverEtherCATService();

        stopMessageHandler();
        /* Stop EtherCAT bus --> Set Master state to INIT */

        shutdownMaster();
        if (em_task_ != nullptr) {
            em_task_->stop();
        }

        named_sharable_mutex::remove(EM_PDO_LOCK_KEY());
        SEP_LOGI("EcMasterSvc has been shutdown\n");
    }




    int EcMasterSvc::ecatNotifyCallbackHandler(int dwCode, void* pParms)
    {
        SEP_LOGI("ECAT noti code = %d.  Handler is not implemented yet.\n", dwCode);
        return 0;
    }


    void EcMasterSvc::startSerialOverEtherCATService()
    {
        if (ECAT_SERIAL_SLAVE_ENABLE && serials_.size() > 0) {
            if (opt_->set_mode) {
                rfc2217task_.set_mode = true;
            }


            rfc2217task_.set({&serials_});
            rfc2217task_.initialize();
            rfc2217task_.start();
        }
    }

    bool EcMasterSvc::stopSerialOverEtherCATService()
    {
        if (ECAT_SERIAL_SLAVE_ENABLE && serials_.size() > 0) {
            SEP_LOGI("Clean up EC Master Svc\n");
            rfc2217task_.stop();
        }
        return true;
    }


}
