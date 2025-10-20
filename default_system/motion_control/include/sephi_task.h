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
// Created by master on 17. 4. 13.
//

#ifndef SEPMASTER_SEPHI_TASK_H_H
#define SEPMASTER_SEPHI_TASK_H_H

#include "log.h"
#include "sephi_ipc.h"
#include <thread>

namespace sephi {
    /// task parameter 및 start/stop 기능을 가진 task template
    /** Task<Param> 클래스를 상속 받은 후 svc()함수를 override하면 된다.
     * @tparam Param task parameter
     */
    template<class Param>
    class Task {
    public:
        Task() : stop_(false){}
        ~Task()
        {
            if (!stop_) {
                stop();
            }
        }

        bool set(const Param& param) {task_param_ = param;}
        bool start()
        {
            // check if already started
            if (task_ != nullptr) {
                return true;
            }

            // create thread
            stop_ = false;
            task_ = std::make_unique<std::thread>([this](){this->svc();});
            return (task_ != nullptr);
        }
        void stop()
        {
            if (task_ != nullptr && !stop_) {
                stop_ = true;
                finalize();
                task_->join();
                task_.reset(nullptr);
            }
        }

    protected:
        virtual void svc() = 0;
        virtual void finalize() { }
        Param task_param_;
        atomic_bool stop_;
        std::unique_ptr<std::thread> task_;
    };
}
#endif //SEPMASTER_SEPHI_TASK_H_H
