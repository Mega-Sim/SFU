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
// Created by sy1135.jeong1696 on 2017-11-04.
//

#ifndef SEPMASTER_ECAT_EMULATOR_H
#define SEPMASTER_ECAT_EMULATOR_H

#include <string>
#include <memory>
#include <list>
#include <Poco/SingletonHolder.h>
#include "sephi_ipc.h"
#include "ecat_slave.h"
#include "simulation_model.h"

namespace sephi
{
    /**\defgroup grp_ecmaster_emul EtherCAT Network Emulator
     * @{
    */
    class EcMasterSvc;
    struct AsyncJobData;
    class OnBoardIOEmulator;

    /** \brief EtherCAT master stack을 emulation한다.

     \details EtherCAT master stack을 모사하여 emulation 기능을 수행한다.
     ECatEmulator의 주요 기능은 다음과 같다.
     - Network configuration : ENI 파일을 분석하여 network 정보를 추출하고 master 정보를 설정한다.
        - process data 정보로부터 pdo data map을 설정한다.
        - vendor id, product code, 및 node address로부터 적절한 slave instance를 생성한다.
        - 모든 설정 파일은 yaml 형식을 따른다. (eni.xml파일의 경우 tools/xml2yaml.py 를 이용하여 eni.yml 생성 가능)
        - 모델 파일은 일정한 구조를 가지는데, 파일 자체가 직관적이며 주석을 포함하므로 설명을 생략한다.
     - PDO buffer emulation
        - pdo data 송/수신을 위한 버퍼를 emulation한다.
        - 버퍼 주소는 getProcessImageInputPtr() 및 getProcessImageOutputPtr() 함수로 얻어낼 수 있다.
     - Data transaction
        - 각 Slave들의 PDO data 송/수신 동작을 emulation하기 위해서 PDO buffer의 해당 slave data를 update하는 handler를 호출한다.
        - PDO transaction은 doRxJobs() 및 doTxJobs()에 구현된다.
        - MailBox 통신은 coeSdoDownload() 및 coeSdoUpload()에 구현된다.
        - MailBox 통신은 PDO와 달리 slave의 mail box list에 직접 insert된다. 그렇지만 insert된 데이터가 사용되는 것은 doTxJobs()가 호출된 시점이다.
     - Simulation model update
        - simulation 모델은 bus cycle에 동기화 되어 업데이트 된다.
        - doTxJobs() 호출에 의해 augmented PDO data에 대한 각 slave node update handler가 호출되고,
        - 각 slave 상태 업데이트가 끝나면 target system update handler가 호출 된다.

     사용법
     - emulator 구동
         - ecmaster command line option에 "-emul" 추가
         \verbatim
         sudo src/ecmaster/ecmaster -f stocker_w_io.xml  -sockraw enp2s0 -dcmmode off -emul
         \endverbatim

     Configuration files
     - yaml eni file : yaml로 변환한 eni 파일; ecmaster 실행파일 폴더
     - model file : eni file 명 + _model;  ecmaster 실행파일 폴더

     Simulation Viewer
     - tools/sim/ 폴더에 있는 python scripts. eg. stocker_view.py
     - grpc를 이용하여 ecmaster로부터 emulator 상태를 수신 받아 visualize 함
     - 설치 의존성
         - python3 : anaconda3 설치 필요
         - grpcio, grpcio_tools : grpc 사용 및 compile 위함
         - vpython : 3D plot을 위함
     - 실행 의존성
         - model 파일: ecmaster에 사용된 model파일과 동일. load_model_config.py 파일의 경로 설정.
         - 실행 순서 : ecmaster가 실행된 후에 viewer를 실행해야 함. (grpc client)
     - 실행 예 (pycharm에서 실행하는 것이 편함)
     \verbatim
     python3 stocker_view.py
     \endverbatim

     */
    class ECatEmulator {
    public:
        ECatEmulator();
        bool initialize(EcMasterSvc *ecmaster, const string& eni_file_name, const string& model_file_name, uint32_t bus_cycle_us = 1000, bool as_observer = false);
        uint8_t* getProcessImageInputPtr();
        uint8_t* getProcessImageOutputPtr();
        CoeValue::Type coeEntryType(int id, int index, int subindex);
        int coeSdoDownload(int id, int index, int subindex, uint8_t *data, int datalen, ThreadCondition *waiter= nullptr);
        int coeSdoUpload(int id, int index, int subindex, uint8_t *data, int datalen, unsigned int* uploadlen= nullptr, ThreadCondition *waiter= nullptr); // timeout은 무시
        int coeSdoUpload_i(int id, int index, int subindex, uint8_t *data, int datalen, int* uploadlen= nullptr, ThreadCondition *waiter= nullptr); // timeout은 무시
        int addAsyncJob(int id, AsyncJobData *job);
        void doAsyncJobs();
        void doRxJobs(); // receives updated stats
        void doTxJobs(); // send new datagram
        bool emulationMode(){ return emulation_mode_enabled_; }
        uint32_t busCycleUs(){ return bus_cycle_us_; }
        void runMenu();
        SimulatorBase* targetSystem() { return target_system_.get();}
        OnBoardIOEmulator* onboardIo() { return onboard_io_.get();}
    private:
        bool loadENI(const string& eni_file_name);
        bool initTargetSystem(const string& model_file_name); // initialize simulation target system
        bool setSlaveInfo();
        bool updateMasterInfo(ECatSlaveInfo& slaveinfo);
    private:
        ECatSlaveModel& getSlaveById(int id);

        std::list<std::unique_ptr<ECatSlaveModel>> slaves_; // slave emulator models. 개수가 많지 않을테니 list로 관리하자.
        uint8_t* rxpdo_; // data to receved by slaves
        uint8_t* txpdo_; // data to send by slaves
        uint8_t rxpdo_buffer_[EM_SHM_SECTION_SIZE]; // data to receved by slaves
        uint8_t txpdo_buffer_[EM_SHM_SECTION_SIZE]; // data to send by slaves
        int rx_bytes_;  // total bytes to rx
        int tx_bytes_;  // total bytes to tx
        bool emulation_mode_enabled_;
        uint32_t bus_cycle_us_;
        EcMasterSvc *master_;
        std::unique_ptr<SimulatorBase> target_system_;
        std::unique_ptr<OnBoardIOEmulator> onboard_io_;
        std::list<std::pair<AsyncJobData*,MailBoxData>> async_jobs_; // async job list

        friend class EcEmulatorMonitor;
        friend class StockerSystem;
        friend class OHTSystem;
    };
    /// @}
    // externs
    extern Poco::SingletonHolder<ECatEmulator> the_ec_emulator;
}

#endif //SEPMASTER_ECAT_EMULATOR_H
