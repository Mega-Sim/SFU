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
// Created by root on 17. 12. 5.
//

#include "oht_io_port_map.h"
#include "oht_onboard_io.h"
#include "gpio.h"
#include "gpio_mmap_arm.h" // 2018 12 26

using namespace sephi;
using namespace sephi::oht;
using namespace std::chrono_literals;
using std::this_thread::sleep_for;

#ifdef MASTER_ARM_BOARD
using MasterBoardGpio = MasterArmBoardGpio;
#else
using MasterBoardGpio = MasterBoardGpioDefault;
#endif

static std::map<unsigned,std::unique_ptr<Gpio>> _oht_gpio_map;
int gpio_write(unsigned int val);
static bool initializeGpioMap(bool intialize = false)
{
    SEP_REQUIRE_RET(_oht_gpio_map.size() == 0, true); // already initialized
    using PortInfo = std::pair<int,int>;
    PortInfo gpio_out_list[] = {
            //... out...
            std::make_pair(_obstacle_area1_sel,0),
            std::make_pair(_obstacle_area2_sel,1),
            std::make_pair(_obstacle_area3_sel,2),
            std::make_pair(_obstacle_area4_sel,3),
            std::make_pair(_obstacle_area5_sel,4),

            std::make_pair(_ohtdetect_area1_sel,5),
            std::make_pair(_ohtdetect_area2_sel,6),
            std::make_pair(_ohtdetect_area3_sel,7),
            std::make_pair(_ohtdetect_area4_sel,8),
            std::make_pair(_ohtdetect_area5_sel,9),

            std::make_pair(_oht_en,10),
            std::make_pair(_obs_en,11),
            std::make_pair(qrr1_in1,12),
            std::make_pair(bcr1_in2,13),
            std::make_pair(qrr2_in3,14),
            std::make_pair(bcr2_in4,15),

            std::make_pair(driving_stop_in1,16),
            std::make_pair(trans_stop_in2,17),
    };
    PortInfo gpio_in_list[] = {
            //... in...
            std::make_pair(_ohtdetect_short,0),
            std::make_pair(_ohtdetect_long,1),
            std::make_pair(_ohtdetect_middle,2),
            std::make_pair(_ohtdetect_fail,3),
            std::make_pair(_stop_driving_front,4),
            std::make_pair(_stop_driving_precheck,5),
            std::make_pair(bcr_driving_ok,6),
            std::make_pair(bcr_driving_ng,7),
            std::make_pair(_stop_trans_front,8),
            std::make_pair(_stop_trans_rear,9),
            std::make_pair(bcr_trans_ok,10),
            std::make_pair(bcr_trans_ng,11),
            std::make_pair(oht_status,12),
            std::make_pair(obs_status,13),
            std::make_pair(driving_stop_st1,14),
            std::make_pair(trans_stop_st2,15),
            std::make_pair(qrr1_st1,16),
            std::make_pair(bcr1_st2,17),
            std::make_pair(qrr2_st3,18),
            std::make_pair(bcr2_st4,19),
            std::make_pair(_obstacle_long,20),
            std::make_pair(_obstacle_middle,21),
            std::make_pair(_obstacle_short,22),
            std::make_pair(_obstacle_fail,23),
    };

    for(auto& port_info : gpio_in_list) {
        int port = (port_info.first & 0x00FF0000) >> 16;
        int bit = port_info.first & 0x0000FFFF;
        SEP_LOGT("GPIO[%d,%d / %d] %s added\n", port, bit, (port-1)*32+bit, getOhtIoPortString(port_info.first));
        _oht_gpio_map.emplace(std::make_pair(port_info.first & 0x00FFFFFF,std::make_unique<Gpio>((port-1)*32+bit, GPIO_INPUT,0, port_info.second)));
    }

    for(auto& port_info : gpio_out_list) {
        int port = (port_info.first & 0x00FF0000) >> 16;
        int bit = port_info.first & 0x0000FFFF;
        SEP_LOGT("GPIO[%d,%d / %d] %s added\n", port, bit, (port-1)*32+bit, getOhtIoPortString(port_info.first));
        _oht_gpio_map.emplace(std::make_pair(port_info.first & 0x00FFFFFF,std::make_unique<Gpio>((port-1)*32+bit, GPIO_OUTPUT,0, port_info.second)));
    }

    if (!intialize) return true;

    for(auto& kv : _oht_gpio_map) {
        int port = (kv.first & 0x00FF0000) >> 16;
        int bit = kv.first & 0x0000FFFF;
        if (kv.second->gpio_init() != 0 ) {
            SEP_LOGE("GPIO[%d,%d / %d] initialization failed\n", port, bit, (port-1)*32+bit);
            continue;
        }

        if (kv.second->gpio_open_nonblock() < 0) {
            SEP_LOGE("GPIO[%d,%d / %d] open nonblock failed\n", port, bit, (port-1)*32+bit);
            continue;
        }
        auto mm = kv.second->isDi() ? MasterBoardGpio::din(port) : MasterBoardGpio::dout(port);
        kv.second->set_mmap(mm, bit);
    }
//    auto smart_switch_enable_ports = {limit_in1, limit_in2, _obstacle_power_reset, _ohtdetect_power_reset,
//                                      qrr1_in1, bcr1_in2, bcr2_in4, driving_stop_in1,  trans_stop_in2};
//    for(auto portid : smart_switch_enable_ports) {
//        _oht_gpio_map[portid&0x00FFFFFF]->gpio_set_val(1);
//    }
//
////    // initialize gpio map - 1226
////    SEP_REQUIRE_RET(MasterBoardGpio::initialize(), false);

    return true;
}


bool OhtOnboardIOEmul::initialize()
{
    is_emulation_mode_ = true;
    shm_ = std::make_unique<SepShm>(OHT_ONBOARD_IO_SHM_KEY());

    SEP_REQUIRE_RET(shm_ != nullptr, false);
    char *emul_state = shm_->address + shm_->size() - 4;
//    if (memcmp(emul_state, "real",4)==0) {
        is_emulation_mode_ = false;
//    }

    // memory map
    // [ao0, ao1...] [do0B, do1B] [ai0, ai1...] [di0B, di1B]
    out_pdo_ = (IoPdoOut*) (shm_->address);
    in_pdo_ = (IoPdoIn*) (shm_->address + sizeof(IoPdoOut));

    initialized_ = true;
    return true;
}

OhtOnboardIO::OhtOnboardIO()
    : super(), exit_(false), poll_th_(nullptr)
{
}

OhtOnboardIO::~OhtOnboardIO()
{
    exit_ = true;
    if (poll_th_ != nullptr) {
        poll_th_->join();
    }
}

bool OhtOnboardIO::initialize()
{
    SEP_REQUIRE_RET(super::initialize(), true);
    SEP_LOGD("Onboard DIO offset bytes(app): di = %d, do = %d\n", int((char*)in_pdo_->din - shm_->address), int((char*)out_pdo_->dout - shm_->address) );
    SEP_REQUIRE_RET(initializeGpioMap(!is_emulation_mode_), false);
    if (!is_emulation_mode_) {
        SEP_LOGT("OhtOnboardIO in Real Master Board Mode!!!\n");
//        poll_th_ = std::make_unique<std::thread>([this](){this->poll();});
    } else {
        SEP_LOGT("OhtOnboardIO in Emulation Mode!!!\n");
    }

    return true;
}

void OhtOnboardIO::synchrosizeSharedMemory(uint64_t* pdo, int index, bool val)
{
    // make sync with shared memory data for status displaying
    if (val) {
        *pdo |= (1ULL << index);
    } else {
        *pdo &= ~(1ULL << index);
    }
}


void OhtOnboardIO::poll()
{
    unsigned count(0);
    auto wake_up_at = std::chrono::system_clock::now();
    auto duration = std::chrono::milliseconds(DI_POLL_CYCLE_MS);
    while(!exit_) {
        for(auto& kv : _oht_gpio_map) {
            if (kv.second->isDi()) {
                auto& gpio = kv.second;
                unsigned int val;
                if (gpio->gpio_read(&val) < 0) {
                    SEP_LOGT("Fail to read %s\n", getOhtIoPortString(0x01000000+kv.first));
                }
                synchrosizeSharedMemory((uint64_t*)in_pdo_->din, gpio->get_index(), val == GPIO_HIGH);
                usleep(0); //for scheduling purpose
            }
        }
        wake_up_at += duration;
        std::this_thread::sleep_until(wake_up_at);

        // for debugging
        if (false && ++count % 1000 == 0) {
            SEP_LOGT("DI States : \n");
            for(auto& kv : _oht_gpio_map) {
                if (!kv.second->isDi()) continue;
                SEP_LOGT("[%d] %s : %d\n", kv.second->get_index(), getOhtIoPortString(0x01000000+kv.first),  bool(readDi64()& (1ULL<<kv.second->get_index())));
            }
        }
    }
}


bool OhtOnboardIO::readDi(unsigned portbit)
{
    if (is_emulation_mode_) {
        return readDiCache(portbit);
    } else {
        auto& gpio = _oht_gpio_map[portbit];
        unsigned int val;
        if (gpio->gpio_read(&val) < 0) {
            SEP_LOGT("Fail to read %x\n", portbit);
            return readDiCache(portbit);
        }
        synchrosizeSharedMemory((uint64_t*)in_pdo_->din, gpio->get_index(), val == GPIO_HIGH);
        return (val == GPIO_HIGH);
    }
}

bool OhtOnboardIO::readDiCache(unsigned portbit)
{
    auto& gpio = _oht_gpio_map[portbit];
    return readDi64() & (1ULL << gpio->get_index());
}


bool OhtOnboardIO::readDo(unsigned portbit)
{
    auto& gpio = _oht_gpio_map[portbit];
    return readDo64() & (1ULL << gpio->get_index());
}

bool OhtOnboardIO::writeDo(unsigned portbit, bool _val)
{
    unsigned int val = _val ? GPIO_HIGH : GPIO_LOW;
    auto& gpio = _oht_gpio_map[portbit];
    if (!is_emulation_mode_ && gpio->gpio_write(val) != 0) {
        SEP_LOGE("Fail to write onboard do %x\n", portbit);
        return false;
    }
    synchrosizeSharedMemory((uint64_t*)out_pdo_->dout, gpio->get_index(), _val);
    return true;
}

