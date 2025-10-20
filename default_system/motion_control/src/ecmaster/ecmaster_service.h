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

#ifndef SEPMASTER_ECMASTER_SERVICE_H
#define SEPMASTER_ECMASTER_SERVICE_H
#include "ecmaster_app.h"
#include "ecat_cia402_servo_test.h"
#include "ecat_serial.h"
#include "emul/ecat_slave.h"
#include <future>
#include <set>
#include <atomic>
#include <iomanip>
#include <algorithm>
#include <signal.h>
#include <setjmp.h>
#include <fstream>
#ifdef USE_ETHERLAB
#include "slave_pdos.h"
#endif

// define Servo Type
#define YASKAWA     0x59
#define RSA         0x52
#define RSA_D       0x53

class CEmNotification;
namespace sephi {
    /**\defgroup grp_ecmaster EtherCAT Master
     * \brief EtherCAT master server
     * \{
     *
     */

    struct ECatSlaveInfo;
    class RpcTestImpl;
    typedef std::future<bool> AsyncJobHandle;
    class EcMasterSvc {
        using time_point = std::chrono::time_point<std::chrono::system_clock>;
        using ECatMsgQ = SepMsgQ<ECatSdoMsg, EM_MSG_Q_SIZE>;
    public:
        EcMasterSvc();
        ~EcMasterSvc();
        bool initService(std::unique_ptr<EmCommandLineOption> opt); // application 초기화 --> OP 상태까지
        void doPdoJobs(); // 이 함수는 EmTask thread에서 실행됨
        bool initEmulator(bool observer_mode = false); // 이 함수는 emulation mode일 때 실행

    public: // test
        bool runTest();
        bool runTestCSP(int slave_id, int dir); //
        bool runTestCSV(int slave_id, int dir);
        bool runHoistCSP(int slave_id, int dir);
        bool runTestPP(int slave_id, int dir);
        bool runTestPV(int slave_id, int dir);
        bool runTestresetEncoder(int slave_id);
        void doDelayTest(int delay_us = 0);
        void stopAllServo();
        void setAllServoIntoFault();
        void changeSlavePresentState(bool new_state) { slave_present_ = new_state; }
        bool showEncoder(void);
        void showSlaveStates();
        void requestRecoverOpMode(); // network error correction
        void shutdownService(); // 서비스 중단, 리소스 해제
        void getversion(int slave_id);

        int32_t pulse2rpm(int32_t pulseval);
        int32_t rpm2pulse(int32_t rpmval);

        void showVersion();
        short VERSION_ECM;
        short VER1;
        short VER2;
        short VER3;

        void ParamDownload();
        void writeEcmVersion();
        void writeSlaveVersion();
    public: // admin jobs
        ECatSlaveInfo* getSlave(int slave_id);
        CiA402Servo* getServo(int slave_id);

    private:
        bool initMaster(); // master 초기화
        void checkLicense(); // license  check
        std::vector<std::string> csv_read_row(std::istream &in, char delimiter);
        bool configureMaster(); // master 설정
        bool registerClient();
        bool scanNetwork(); // scan slaves
        bool getSlaveInfos(); // fill slaves_
        bool changeECatState(EcState new_state); // change state
        int  getSerialPortCount(const ECatSlaveInfo &slave); // IO slave의 serial port 개수 파악
        bool dumpIoSlaveInfo(const ECatSlaveInfo &slave); // IO slave 정보 파악
        bool doSerialDataTransaction(); // serial data tranaction 수행
        void startSerialOverEtherCATService();
        bool stopSerialOverEtherCATService();

        bool prepareSvc(); // INIT -> PREOP
        bool setupSvc(); // PREOP-> SAFEOP
        void checkECatStates();
        void shutdownMaster();
        void checkError();

        void checkDcmError(); // when fail to transit to SAFEOP
        bool checkDcmStatus(); // when in OP

        int ecatNotifyCallbackHandler(int dwCode, void* pParms);
        bool startMessageHandler();
        void stopMessageHandler();
        bool handleMessages();

        bool handleRequestMessage(const ECatSdoMsg& msg, ECatSdoMsg &res);
        bool handleReleaseMessage(const ECatSdoMsg& msg, ECatSdoMsg &res);
        bool handleSdoMessage(const ECatSdoMsg& msg);
        bool handleBitOpMessage(const ECatSdoMsg& msg);
        bool handleSetMasterAxisMessage(const ECatSdoMsg &msg);
        bool readStockerInData(StockerIoPdoIn &in);
        bool readStockerOutData(StockerIoPdoOut &in);
        bool writeStockerOutData(const StockerIoPdoOut &out);

        bool readOhtFrontInData(OhtFrontIoPdoIn &in);
        bool readOhtRearInData(OhtRearIoPdoIn &in);
        bool readOhtFrontOutData(OhtFrontIoPdoOut &out);
        bool readOhtRearOutData(OhtRearIoPdoOut &out);

        bool writeOhtFrontOutData(const OhtFrontIoPdoOut &out);
        bool writeOhtRearOutData(const OhtRearIoPdoOut &out);

        int nDcmCtlSetVal_; //DCMCtlSetVal: DCM controller set value in nanosec
        int client_id;
        int ecat_state_;
        std::unique_ptr<EmCommandLineOption> opt_;
        std::unique_ptr<CEmNotification> noti_;
        CAtEmLogging logger_;
        #ifdef USE_ETHERLAB
        ec_master_t* ec_master_ = nullptr;
        ec_master_state_t ec_master_state_;
        ec_domain_t* ec_domain_rd_ = nullptr;
        ec_domain_t* ec_domain_wr_ = nullptr;
        ec_domain_state_t ec_domain_state_;
        #endif // USE_ETHERLAB

        std::unique_ptr<EmTask> em_task_;
        uint8_t *in_pdo;
        uint8_t *out_pdo;
        std::deque<ECatSlaveInfo> slaves_;
        std::deque<CiA402Servo> servos_;
        std::deque<ECatSerialSlave> serials_;
        ECatSlaveInfo* stocker_io_;
        ECatSlaveInfo* oht_front_io_;
        ECatSlaveInfo* oht_rear_io_;
        RFC2217ServiceTask rfc2217task_;

        std::unique_ptr<SepShm> shm_;   // shared memory for pdo data

        std::unique_ptr<SepShm> shmforonboardio_;

        std::unique_ptr<ECatMsgQ> msgq_; // msgq for sdo transaction
        std::unique_ptr<IpcCondition> pdo_arrival_event_; // cyclic event
        atomic_bool recover_in_process_; // network errork correction in process
        bool slave_present_; // true if slaves are scanned and present

        std::set<AsyncJobHandle*> async_jobs_; // set of async job handles
        std::unique_ptr<std::thread> sdo_msg_handle_task_;
        std::atomic<bool> sdo_msg_handle_stop_;

        static EcDWord ecatNotifyCallback(EcDWord dwCode, EcNotifyParam* pParms);
        friend class ECatMessageTask;
        friend class EcMasterRpcImpl;
        friend class ECatEmulator;
    };


    struct ECatSlaveInfo {
        int32_t slave_id;   // 0, 1, ... The slave’s ID
        int32_t bus_index;   // 0, 1, ... The slave’s ID to bind bus slave and config slave information
        int16_t module_num;
        int16_t station_address; //1001, 1002, ... The slave’s station address (Value of slave ESC register 0x0010)
        int16_t alias_address;  // 1, 2, ... The slave’s alias address (Value of slave ESC register 0x0012)
        uint32_t vendor_id; //Vendor Identification stored in the E2PROM at offset 0x0008
        uint32_t product_code; //Product Code stored in the E2PROM at offset 0x000A
        uint32_t in_pd_offset; // input process data offset in byte
        uint32_t out_pd_offset; // output process data offset in byte
        uint32_t in_pd_length;  // input process data length in byte
        uint32_t out_pd_length; // output process data length in byte
        SlaveType type; // slave type for mapping
        int32_t revision; // revision info
#ifdef USE_ETHERLAB
        ec_slave_config_t *slave_config; //
        ec_slave_config_state_t slave_config_state{};
        PdoInfo pdo_info;
        uint16_t dc_activation_code;
#endif // USE_ETHERLAB

        int64_t context; // context data (type dependent)
        pid_t user_pid; // user pid
        int32_t following_axis; // 속도 추종 master 축
        ECatSlaveMsgQ* msgq;
    public:
        ECatSlaveInfo():msgq(nullptr){}
        ~ECatSlaveInfo(){ if (msgq != nullptr){ delete msgq; msgq = nullptr;} }
        void print()
        {
            SEP_LOGT("slave_id: %d, station_address: %d, type: %d\n", slave_id, station_address, type);
            SEP_LOGT("pdo map<offs,size>: in <%d:%d> out <%d:%d>\n", in_pd_offset, in_pd_length, out_pd_offset, out_pd_length);
        }
    };

    /// \}
    extern EcMasterSvc* the_EcMasterSvc_ins;
}

// ---- selective logs ----
#define STK_IO_LOG_ENABLE 0

#if STK_IO_LOG_ENABLE
#define STK_IO_LOGT SEP_LOGT
#else
#define STK_IO_LOGT(...)
#endif

#endif //SEPMASTER_ECMASTER_SERVICE_H
