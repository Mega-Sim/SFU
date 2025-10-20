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
// Created by sem1 on 17. 11. 6.
//

#include "emul/stocker/stocker_simulator.h"
#include "emul/oht/oht_simulator.h"
#include "emul/onboard_io_emulator.h"
#include "cli_menu.h"
#include "ecat_emulator.h"


namespace  sephi
{
    class EcEmulatorMonitor : public  ICliMenuProvider
    {
    public:
        bool initialize()
        {
            ins_ = the_ec_emulator.get();
            return true;
        }
        bool finalize(){ return true;}

        int dumpSlaveStatus(void *arg) {
            printf(">>> Dump All Slave Status <<<\n");
            for(auto& slave: ins_->slaves_) {
                slave->printStatus();
            }
            return 0;
        }
        int dumpObjectDict(void *arg) {
            printf(">>> Dump All Slave Object Dictionary <<<\n");
            for(auto& slave: ins_->slaves_) {
                slave->dumpObjectDictionary();
            }
            return 0;
        }

        int makeServoFault(void *arg) {
            printf(">>>Make servo fault <<<\n");
            for(auto& slave: ins_->slaves_) {
                auto servo = dynamic_cast<YaskawaServoModel*>(slave.get());
                if (servo != nullptr) {
                    servo->makeFault(0xa11);
                }
            }
            return 0;
        }

        int printTagetSystemStatus(void* arg) {
            ins_->target_system_->printState();
            return 0;
        }

        int changeForkEncoderValue(void *arg) {
            StockerSystem *stk = (StockerSystem *)ins_->target_system_.get();
            int pulse = stk->getEncoderPulseValue(3);
            stk->setEncoderPulseValue(3, pulse + (1<<20));
            return 0;
        }


        int ohtReadSerial(void *arg) {
            OHTSystem* oht = (OHTSystem*)ins_->target_system_.get();
            printf(">> read serial <<\n");
            printf(">> device[front=0,rear=1, onboard=otherwise] port[0~9]: ");
            fflush(stdout);
            int dev = 0; int port = 0; int len = 0;
            char *line = getLine((char*)arg);
            sscanf(line, "%d %d", &dev, &port);
            char buff[1024];
            if (dev==0){
                len = oht->front_io_->receive(port, buff, sizeof(buff));
            } else if (dev==1) {
                len = oht->rear_io_->receive(port, buff, sizeof(buff));
            } else {
                auto onboard = ins_->onboardIo();
                len = onboard->receive(port, buff, sizeof(buff));
            }
            buff[len] = 0;
            printf("received(len=%d)%s\n", len, buff);
            return 0;

        }
        int ohtWriteSerial(void *arg) {
            OHTSystem* oht = (OHTSystem*)ins_->target_system_.get();
            printf(">> write serial <<\n");
            printf(">> device[front=0,rear=1, onboard=otherwise] port[0~3] msg(no white space): ");
            fflush(stdout);
            int dev = 0; int port = 0; int len = 0;
            char buff[1024] = {0};
            char *line = getLine((char*)arg);
            sscanf(line, "%d %d %s", &dev, &port, buff);
            len = strlen(buff);
            if (dev==0){
                len = oht->front_io_->send(port, buff, len);
            } else if (dev==1) {
                len = oht->rear_io_->send(port, buff, len);
            } else {
                auto onboard = ins_->onboardIo();
                len = onboard->send(port, buff, len);
            }
            printf("send(len=%d)%s\n", len, buff);
            return 0;
        }

        int ohtEmoPush(void *arg)
        {
            SEP_LOGT("OHT EMO Pressed");
            OHTSystem* oht = (OHTSystem*)ins_->target_system_.get();
            oht->changeEmoState(true);
            return 0;
        }
        int ohtEmoRelease(void *arg)
        {
            SEP_LOGT("OHT EMO Released");
            OHTSystem* oht = (OHTSystem*)ins_->target_system_.get();
            oht->changeEmoState(false);
            return 0;
        }

        int ohtReadOnboardDI(void *arg)
        {
            auto onboard = ins_->onboardIo();
            printf("Simulator's DI (controller's DO) : %lu\n", *(uint64_t*)onboard->DI());
            return 0;
        }

        int ohtWriteOnboardDO(void *arg)
        {
            printf(">> write Simulator DO (i.e. controller's DI) <<\n");
            printf(">> byte value in hexa format: ");
            fflush(stdout);
            uint8_t val;
            char *line = getLine((char*)arg);
            val = std::stoul(line, nullptr, 16);
            printf("val in decimal = %d\n", val);
            auto onboard = ins_->onboardIo();
            *onboard->DO() = val;
            return  0;
        }

        int ohtSetOHTLocation(void* arg)
        {
            OHTSystem* oht = (OHTSystem*)ins_->target_system_.get();
            uint32_t from, to;
            double offset;
            printf("New oht location (from_node to_node offset):");
            fflush(stdout);
            char *line = getLine((char*)arg);
            sscanf(line, "%u %u %lf", &from, &to, &offset);
            oht->setOhtLocation(from, to, offset);
            return 0;
        }

        int ohtOtherOHTEnable(void* arg)
        {
            OHTSystem* oht = (OHTSystem*)ins_->target_system_.get();
            printf("other oht: start_node end_node, max_speed:");
            fflush(stdout);
            char *line = getLine((char*)arg);
            uint32_t from, to;
            float max_v;
            sscanf(line, "%u %u %f", &from, &to, &max_v);
            oht->other_oht_.setTarget(from, to, max_v);
            oht->other_oht_.enable();
            return 0;
        }
        int ohtOtherOHTDisable(void* arg)
        {
            OHTSystem* oht = (OHTSystem*)ins_->target_system_.get();
            oht->other_oht_.disable();
            return 0;
        }
        int ohtOtherOHTPause(void* arg)
        {
            OHTSystem* oht = (OHTSystem*)ins_->target_system_.get();
            oht->other_oht_.pause();
            return 0;
        }
        int ohtOtherOHTResume(void* arg)
        {
            OHTSystem* oht = (OHTSystem*)ins_->target_system_.get();
            oht->other_oht_.resume();
            return 0;
        }


    private:
        ECatEmulator* ins_;
    };

    typedef CliMenuEntry<EcEmulatorMonitor> MenuEntry;
#define MENU_ITEM(...)  make_unique<MenuEntry>(__VA_ARGS__)


    class TestMenu : public CliMenu<EcEmulatorMonitor> {
    public:
        TestMenu(EcEmulatorMonitor *owner_) : CliMenu<EcEmulatorMonitor>(owner_) {
            initializeMenu();
        }
        int initializeMenu() {
            menu_list.push_back( MENU_ITEM("ECat Emulator Test", 0));
            auto& menu = menu_list.front()->submenu;
            menu.push_back(MENU_ITEM("dump slave object dict", 'd', &EcEmulatorMonitor::dumpObjectDict, (void*)NO_FN_ARG));
            menu.push_back(MENU_ITEM("dump slave status", 's', &EcEmulatorMonitor::dumpSlaveStatus, (void*)NO_FN_ARG));
            menu.push_back(MENU_ITEM("target system status", 't', &EcEmulatorMonitor::printTagetSystemStatus, (void*)NO_FN_ARG));
            menu.push_back(MENU_ITEM("STK manipulation", 'S'));
           
            menu.push_back(MENU_ITEM("OHT manipulation", 'O'));
            auto& ohtmenu = menu.back()->submenu;
            ohtmenu.push_back(MENU_ITEM("serial read", '1', &EcEmulatorMonitor::ohtReadSerial, (void*)NO_FN_ARG));
            ohtmenu.push_back(MENU_ITEM("serial write", '2', &EcEmulatorMonitor::ohtWriteSerial, (void*)NO_FN_ARG));
            ohtmenu.push_back(MENU_ITEM("read onboard di", '3', &EcEmulatorMonitor::ohtReadOnboardDI, (void*)NO_FN_ARG));
            ohtmenu.push_back(MENU_ITEM("write onboard do", '4', &EcEmulatorMonitor::ohtWriteOnboardDO, (void*)NO_FN_ARG));
            ohtmenu.push_back(MENU_ITEM("relocate oht", '5', &EcEmulatorMonitor::ohtSetOHTLocation, (void*)NO_FN_ARG));
            ohtmenu.push_back(MENU_ITEM("press EMO ", '6', &EcEmulatorMonitor::ohtEmoPush, (void*)NO_FN_ARG));
            ohtmenu.push_back(MENU_ITEM("release EMO", '7', &EcEmulatorMonitor::ohtEmoRelease, (void*)NO_FN_ARG));
            ohtmenu.push_back(MENU_ITEM("other oht: enable", 'e', &EcEmulatorMonitor::ohtOtherOHTEnable, (void*)NO_FN_ARG));
            ohtmenu.push_back(MENU_ITEM("other oht: disable", 'd', &EcEmulatorMonitor::ohtOtherOHTDisable, (void*)NO_FN_ARG));
            ohtmenu.push_back(MENU_ITEM("other oht: pause", 'p', &EcEmulatorMonitor::ohtOtherOHTPause, (void*)NO_FN_ARG));
            ohtmenu.push_back(MENU_ITEM("other oht: resume", 'r', &EcEmulatorMonitor::ohtOtherOHTResume, (void*)NO_FN_ARG));


            return push(menu_list.front().get());
        }

    };

    void ECatEmulator::runMenu()
    {

        EcEmulatorMonitor emulator;
        TestMenu menu(&emulator);
        menu.runMenu();
    }

}
