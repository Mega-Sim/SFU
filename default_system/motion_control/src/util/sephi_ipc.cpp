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
// Created by master on 17. 3. 22.
//
#include "sephi_ipc.h"
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sched.h>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


using namespace sephi;

std::mutex sephi::DebugState::lock;
std::unique_ptr<SepShm> sephi::DebugState::shm;
DebugState::Data* sephi::DebugState::flags = nullptr;

bool sephi::DebugState::initServer()
{
    if (flags != nullptr) return true;
    shm = std::make_unique<SepShm>(SEP_DBG_FLAG_KEY(), sizeof(Data));
    SEP_REQUIRE_RET(shm->address != nullptr, false);
    flags = (Data*)shm->address;
    return true;
}

bool sephi::DebugState::initClient()
{
    if (flags != nullptr) return true;
    shm = std::make_unique<SepShm>(SEP_DBG_FLAG_KEY());
    SEP_REQUIRE_RET(shm->address != nullptr, false);
    flags = (Data*)shm->address;
    return true;
}

void sephi::DebugState::set(int id, int val)
{
    std::lock_guard<std::mutex> guard(lock);
    if (flags == nullptr) {
        initClient();
    }
    flags->at(id) = val;
}

int sephi::DebugState::get(int id)
{
    std::lock_guard<std::mutex> guard(lock);
    if (flags == nullptr) {
        initClient();
    }
    return flags->at(id);
}


void logStackTrace()
{
    const int BT_BUF_SIZE = 100;
    int j, nptrs;
    void *buffer[BT_BUF_SIZE];
    char **strings;

    nptrs = backtrace(buffer, BT_BUF_SIZE);
    //printf("backtrace() returned %d addresses\n", nptrs);

    /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
       would produce similar output to the following: */

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        SEP_LOGE("backtrace_symbols error");
        return;
    }
    SEP_LOGI("[[BackTrace]]");
    for (j = 1; j < nptrs; j++)
        SEP_LOGI("%s\n", strings[j]);

    free(strings);
}

bool sephi::setCpuAffinity(int cpuIndex) {
    cpu_set_t cpu_set;

    CPU_ZERO(&cpu_set);
    CPU_SET(cpuIndex, &cpu_set);
    bool res = sched_setaffinity(0, sizeof(cpu_set), &cpu_set); //0 means current thread
    if (!res) {
        SEP_LOGE("Error: Set thread affinitiy, invalid CPU index %d\n", cpuIndex);
    }
    return res;
}


int sephi::addIntoRealtimeSchedule(int sched_priority)
{
    struct timespec ts;
    int nRetval;
    unsigned int dwResult = 0;
    int bHighResTimerAvail;
    struct sched_param schedParam;

    /* request realtime scheduling for the current process
     * This value is overwritten for each individual task
     */
    schedParam.sched_priority = sched_priority; /* 1 lowest priority, 99 highest priority */
    nRetval = sched_setscheduler( 0, SCHED_FIFO, &schedParam );
    if (nRetval == -1)
    {
        SEP_LOGE( "ERROR - cannot change scheduling policy!\n"
                          "root privilege is required or realtime group has to be joined!\n" );
        goto Exit;
    }

    /* disable paging */
    nRetval = mlockall( MCL_CURRENT | MCL_FUTURE );
    if (nRetval == -1)
    {
        SEP_LOGE( "ERROR - cannot disable paging!\n" );
        dwResult = -1;
        goto Exit;
    }

    dwResult = 0;
    Exit:
    return dwResult;
}


boost::posix_time::ptime sephi::getPtimeAfterSecond(float duration)
{
    using namespace boost::posix_time;
    ptime until  = boost::posix_time::microsec_clock::universal_time();

    long ts = long(duration),
            h = ts/3600,
            m = ts%3600 / 60,
            s = ts % 60,
            micros = long((duration - int(duration)) * 1e6);
    time_duration td = hours(h) + minutes(m) + seconds(s) + microseconds(micros);
    until += td;
    return until;
}

IpcCondition::IpcCondition(const char* name, bool create) : is_creator_(create) {
    if (create) {
        named_mutex::remove(name);
        named_condition::remove(name);
        mtx_ = std::make_unique<named_mutex>(create_only, name);
        cond_ = std::make_unique<named_condition>(create_only, name);
    } else {
        mtx_ = std::make_unique<named_mutex>(open_only, name);
        cond_ = std::make_unique<named_condition>(open_only, name);
    }
    name_ = name;
}
IpcCondition::~IpcCondition() {
    if (is_creator_) {
        named_mutex::remove(name_.c_str());
        named_condition::remove(name_.c_str());
    }
}

bool IpcCondition::wait(float wait_time) {
    scoped_lock<named_mutex> lock{*mtx_};
    if (wait_time < 0) {
        cond_->wait(lock);
        return true;
    } else  {
        return cond_->timed_wait(lock, getPtimeAfterSecond(wait_time));
    }
}

void IpcCondition::wakeUp() {
    cond_->notify_all();
}


SepShm::SepShm(const char* shmname, int _size) : is_server_(_size > 0), address(nullptr) {
    shmname_ = shmname;
    if (is_server_)
    {
        shared_memory_object::remove(shmname);
        shm_ = std::make_unique<shared_memory_object>(create_only, shmname, read_write);
        shm_->truncate(static_cast<size_t >(_size));
        region_ = std::make_unique<mapped_region>(*shm_, read_write);
        address = (char*)region_->get_address();
    } else {
        shm_ = std::make_unique<shared_memory_object>(open_only, shmname, read_write);
        region_ = std::make_unique<mapped_region>(*shm_, read_write);
        address = (char*)region_->get_address();
    }
}

SepShm::~SepShm() {
    if (is_server_) {
        shared_memory_object::remove(shmname_.c_str());
    }
    address = nullptr;
}

