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
#include <yaml-cpp/yaml.h>
#include "ecat_emulator.h"
#include "onboard_io_emulator.h"

using namespace sephi;

OnBoardIOEmulator::OnBoardIOEmulator(const char* model_file)
{
    // parent's variables
    slave_id_ = -1;

    // load target system onboard configuration
    SEP_REQUIRE(access(model_file, R_OK)==0);
    auto config = YAML::LoadFile(model_file);
    auto target_system_name = config["TargetSystem"].as<string>();
    auto ioconfig = config["Systems"][target_system_name]["OnboardIO"];

    name_ = ioconfig["shmkey"].as<string>();
    ao_count_ = ioconfig["AI"].as<int>(); // simulator ao <--> controller ai
    ai_count_ = ioconfig["AO"].as<int>();
    do_count_ = ioconfig["DI"].as<int>();
    di_count_ = ioconfig["DO"].as<int>();
    port_count_ = ioconfig["SERIAL"].as<int>();

    ai_bytes_ = ai_count_*2; // 16bit width
    di_bytes_ = (di_count_+7)/8;
    ao_bytes_ = ao_count_*2; // 16bit width
    do_bytes_ = (do_count_+7)/8;

    SEP_LOGD("Onboard DIO Offset bytes(emulator) : di = %d, do = %d\n", ai_bytes_, ai_bytes_ + di_bytes_ + ao_bytes_);
    // initialize shared memory
    // [ai0, ai1...] [di0B, di1B] [ao0, ao1...] [do0B, do1B], [emulate state]
    shm_ = std::make_unique<SepShm>(name_.c_str(), di_bytes_+do_bytes_+ai_bytes_+ao_bytes_+4);
    char *emul_state = shm_->address + shm_->size() - 4;
    memcpy(emul_state, the_ec_emulator.get()->emulationMode() ? "emul" : "real", 4);
    SEP_LOGT("OnBoardIOEmulator : %s mode", the_ec_emulator.get()->emulationMode() ? "emul" : "real");

    // serial buffer initialization
    tx_buffers_.resize(port_count_);
    rx_buffers_.resize(port_count_);
}

// simulator side aio - respond to controller ai/o
int16_t& OnBoardIOEmulator::AI(int port)
{
    SEP_REQUIRE_THROW(port>=0 && port < ai_count_);
    int16_t *ai = (int16_t*)(shm_->address);
    return ai[port];
}

uint8_t* OnBoardIOEmulator::DI()
{


    return (uint8_t*)(shm_->address + ai_bytes_);
}

// simulator side dio - respond to controller di/o
int16_t& OnBoardIOEmulator::AO(int port)
{
    SEP_REQUIRE_THROW(port>=0 && port < ao_count_);
    int16_t *ao = (int16_t*)(shm_->address + ai_bytes_ + di_bytes_);
    return ao[port];
}

uint8_t* OnBoardIOEmulator::DO()
{
    return (uint8_t*)(shm_->address + ai_bytes_ + di_bytes_ + ao_bytes_);
}


int OnBoardIOEmulator::send(int portid, const void* data, int length)
{
    SEP_REQUIRE_THROW(portid>=0 && portid < port_count_);
    return rx_buffers_[portid].push(data, length);
}

int OnBoardIOEmulator::receive(int portid, void* data, int length)
{
    SEP_REQUIRE_THROW(portid>=0 && portid < port_count_);
    return tx_buffers_[portid].pop(data, length);
}

