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
// Created by root on 17. 12. 11.
//

#ifndef SEPMASTER_OHT_CONTROLLER_H
#define SEPMASTER_OHT_CONTROLLER_H

#include "oht_behaviors.h"
#include "oht_io_config.h"

#ifndef AMC_CLIENT_MODE
#define AMC_CLIENT_MODE 1 // 0 : stand-alone(demo), 1: used for mark updater
#endif

// Pulse to MM
#define V81_PULSETOMM_DRIVE 114.7341619
#define V82_PULSETOMM_DRIVE 75.7245469
#define V86_PULSETOMM_DRIVE 75.7245469
#define P4M_PULSETOMM_DRIVE 114.7341619

#define V81_PULSETOMM_HOIST 361.296069
#define V82_PULSETOMM_HOIST 361.296069
#define V86_PULSETOMM_HOIST 244.452393
#define P4M_PULSETOMM_HOIST 407.117096

#define V81_PULSETOMM_SLIDE 682.6666667
#define V82_PULSETOMM_SLIDE 546.1333333
#define V86_PULSETOMM_SLIDE 819.2
#define P4M_PULSETOMM_SLIDE 10000.0

#define IO_SIZE 100
#define OutOffset   1000
#define EtcOffset   2000

class OHTMotionDemo;
namespace amc { class Amc4OHT; }
namespace sephi {
    namespace oht {
        enum OHT_AXES {
            OHT_WHEEL_F,
            OHT_WHEEL_R,
            OHT_HOIST,
            OHT_SLIDE
        };

        class OHTController {
        public:
            enum State {
                DRV_READY       = 0x01,
                DRV_RUN         = 0x10,
                DRV_RUN_STRAGHT = 0x11,
                DRV_RUN_CURVE   = 0x12,
                DRV_MARK        = 0x14,
                DRV_COMPLETE    = 0x00,
                LU_READY        = 0x0100,
                LU_PIO_CHECK,
                LU_SHIFT_OP,
                LU_DOWN,
                LU_LOAD_UNLOAD,
                LU_UP,
                LU_SHIFT_ORG,
                LU_ANITDROP,
                LU_COMPLETE
            };

         public:
            OHTController();
            ~OHTController();
            bool initialize();
            void finalize();

            // motion tasks
            bool localize();
            bool runToNode(uint32_t nodeid);
            bool runToEqStation(uint32_t stid);
            bool runDisplacement(int32_t disp_mm);
            bool stopRun(bool quick = false);
            bool tryMark() { SEP_REQUIRE_IMPL_RET(false);}

            // transfer task
            bool load(bool step = false);
            bool unload(bool step = false);
            bool transStop();
            bool transOrigin();
            bool transPause();
            bool transResume();
            bool transStepRun();


            // status
            //OHTLocation getLocation();
            int getMarkState();
            int getRunState();
            int getCurrentStation();
            double getRemainingDistance();
            double getPosmm(int axis);
            double getPos(int axis);

            bool readRawDi(unsigned portid);
            bool readRawDo(unsigned portid);
            bool writeRawDo(unsigned portid, bool value);
            bool readDi(unsigned portid);
            bool readDo(unsigned portid);
            bool writeDo(unsigned portid, bool value);

            // aux
            void setToInitialPosition();
            bool loadIOConfig(int OHT_ID = 36);
            bool getloadIOStatus() { return loadIOStatus; }
            bool loadIOStatus;
/*
            std::map<string, unsigned> name_map;    // name, num
            std::map<unsigned, unsigned> num_map;  // num, portid

            unsigned getportid(string name);
            unsigned getportid(unsigned num);
            void setportid(string name, unsigned num, unsigned portid);
*/
            OhtIoConfig ioConfig_;
        private:
            bool loadConfig(const char* dir_prefix = nullptr);
            bool initializeIO();
            bool initializeServo();
            bool makeBehaviors();
            bool startService();
            void stopService();
            void svc();
            bool initialized_;
            std::unique_ptr<std::thread> thread_;
            std::mutex lock_;
            bool thread_exit_;
            OHTMap map_;
            OhtConfig config_;
            std::list<std::unique_ptr<Perception>> perceptions_;
            Localization* localizer_;
            //unique_ptr<Driving> drive_;
            //unique_ptr<Transfer> trans_;
            std::set<unsigned> bit_inversion_ports_;
            std::set<unsigned> active_low_ports_;

            //
            unique_ptr<SingleAxisAmcDrive> front_wheel_;
            unique_ptr<SingleAxisAmcDriveRear> rear_wheel_;
            unique_ptr<SingleAxisAmc> slide_;
            unique_ptr<SingleAxisAmc> hoist_;
            std::array<SingleAxisAmc*,4> axes_;
            std::array<EcServoMotor*,4> motors_;
            unique_ptr<EcOHTFrontIo> front_io_;
            unique_ptr<EcOHTRearIo> rear_io_;
            OhtOnboardIO onboard_io_;
            unique_ptr<BcrBL185> driving_bcr_;
            unique_ptr<BcrBL185> trans_bcr_;

            friend class Behavior;
            friend class Localization;
            friend class Monitoring;
            friend class ObstacleDetection;
            friend class Driving;
            friend class Transfer;
            friend class ::OHTMotionDemo;
            friend class amc::Amc4OHT;
        };
    }
}

#endif //SEPMASTER_OHT_CONTROLLER_H
