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
// Created by root on 17. 11. 23.
//

#include "oht_pio.h"
#include <chrono>
using namespace sephi;
using namespace sephi::oht_sim;

// each tp
// tp[0] : L/U req on ~ Tr req on
// tp[1] : ready on ~ busy on
// tp[2] : busy on ~ carrir detect/remove
// tp[3] : L/U req off ~ busy off
// tp[4] : ready off ~ valid off
// tp[5] : valid off ~ valid on : continuous handoff

const std::array<uint32_t, 6> PIOPassiveDevice::tp_max_ms = {2000, 2000, 60000, 60000, 2000, 2000};

// response_time[0] : valid ~ load req
// response_time[1] : tr req ~ ready
// response_time[2] : detect ~ load req off
// response_time[3] : complete ~ ready off
const std::array<uint32_t, 4> PIOPassiveDevice::response_time_ms = {100, 100, 100, 200};

PIOPassiveDevice::TimeoutJob::TimeoutJob(int tpindex, int timeout_ms, void (*_func)(PIOPassiveDevice*))
{
    tp_index = tpindex;
    timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(timeout_ms);
    func = _func;
}

bool PIOPassiveDevice::TimeoutJob::isTimeOut()
{
    return (std::chrono::system_clock::now() >= timeout);
}

PIOPassiveDevice::PIOPassiveDevice()
    : tp_index_(TP_INVALID), ready_(false), transfer_request_(false), error_(false), has_carrier_(false), connected_(false)
{
    resetIo();
}

void PIOPassiveDevice::resetIo()
{
    tp_index_ = TP_INVALID;
    memset(&oht_pio_state_, 0, sizeof(oht_pio_state_));

}

void PIOPassiveDevice::connect()
{
    if (connected_) return;
    in_loading_seq_ = !has_carrier_;
    connected_ = true;
}

void PIOPassiveDevice::disconnect()
{
    if (!connected_) return;
    resetIo();
    connected_ = false;
}

void PIOPassiveDevice::setCSn(int n, bool on)
{
    // rising edge detection
    // \todo multi-port support
    if (n == 0) oht_pio_state_.cs0 = on ? 1 : 0;
    else if (n == 1) oht_pio_state_.cs1 = on ? 1 : 0;
    else if (n == 2) oht_pio_state_.cs2 = on ? 1 : 0;
    else if (n == 3) oht_pio_state_.cs3 = on ? 1 : 0;

}

void PIOPassiveDevice::setValid(bool on)
{
    if (oht_pio_state_.valid == on) return; // detect trigger

    oht_pio_state_.valid = on ? 1 : 0;
    if (on) {
        auto job = [](PIOPassiveDevice*self) {
            self->transfer_request_ = true;
        };
        delayed_jobs_.push_back(TimeoutJob(TP1, response_time_ms[RTP1], job));
    }
}

void PIOPassiveDevice::setTrReq(bool on)
{
    // detect trigger
    if (oht_pio_state_.tr_req == on) return;

    oht_pio_state_.tr_req = on ? 1 : 0;
    if (on) {
        auto job = [](PIOPassiveDevice*self) {
            self->ready_ = true;
        };
        delayed_jobs_.push_back(TimeoutJob(TP2, response_time_ms[RTP2], job));
    }
}

void PIOPassiveDevice::setBusy(bool on)
{
    oht_pio_state_.busy = on ? 1 : 0;
    if (on) {
        tp_index_ = TP3;
        tp_start_ = std::chrono::system_clock::now();
    } else {
        tp_index_ = TP_INVALID;
    }
}

void PIOPassiveDevice::setComplete(bool on)
{
    if (oht_pio_state_.complete == on) return;
    oht_pio_state_.complete = on ? 1 : 0;
    if (on) {
        auto job = [](PIOPassiveDevice*self) {
            self->ready_ = false;
            SEP_LOGT("setComplete: ready = false\n");
        };
        SEP_LOGT("setComplete: complete = true\n");
        delayed_jobs_.push_back(TimeoutJob(TP5, response_time_ms[RTP4], job));
    }
}

bool PIOPassiveDevice::inLoadingSeq()
{
    return in_loading_seq_;
}

bool PIOPassiveDevice::triggerCarrierXferState()
{
    auto job = [](PIOPassiveDevice*self) {
        self->transfer_request_ = false;
        SEP_LOGT("transfer_request_ == false\n");
    };
    if ((in_loading_seq_ && !has_carrier_) || (!in_loading_seq_ && has_carrier_) ) {
        delayed_jobs_.push_back(TimeoutJob(TP4, response_time_ms[RTP3], job));
        has_carrier_ = !has_carrier_;
        return true;
    } else {
        return false;
    }

}


void PIOPassiveDevice::checkTimeoutError()
{
    if (tp_index_ != TP_INVALID) {
        auto elapsed = std::chrono::system_clock::now() - tp_start_;
        if (elapsed.count()*1e-6 > tp_max_ms[tp_index_]) {
            SEP_LOGE("PIO timeout error. TP index = %d, max = %dms, elapsed = %dms\n", tp_index_, tp_max_ms[tp_index_], elapsed.count()*1e-6);
            error_ = true;
        }
    }
}

void PIOPassiveDevice::doDelayedJobs()
{
    auto doJob = [this](TimeoutJob& job) {
        tp_index_ = job.tp_index;
        tp_start_ = std::chrono::system_clock::now();
        job.func(this);
    };

    auto job = delayed_jobs_.begin();
    while(job != delayed_jobs_.end()){
        if (job->isTimeOut()) {
            doJob(*job);
            job = delayed_jobs_.erase(job);
        } else {
            ++job;
        }
    }
}

void PIOPassiveDevice::update()
{
    doDelayedJobs();
    checkTimeoutError();
    if (error_) {
        throw  std::runtime_error("PIO time out error!");
    }
    //printStatus();
}

void PIOPassiveDevice::printStatus()
{
    SEP_LOGT("Eq State: load req = %d, ready = %d, error = %d\n", transfer_request_, ready_, error_);
    SEP_LOGT("OHT state: cs0:%d, valid:%d tr_req:%d busy:%d complete %d\n",
             oht_pio_state_.cs0,
             oht_pio_state_.valid,
             oht_pio_state_.tr_req,
             oht_pio_state_.busy,
             oht_pio_state_.complete
    );
}