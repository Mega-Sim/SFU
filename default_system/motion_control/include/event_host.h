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
// Created by sem1 on 17. 4. 27.
//

#ifndef SEPMASTER_EVENT_HOST_H_H
#define SEPMASTER_EVENT_HOST_H_H

#include "log.h"
#include <map>
namespace sephi
{
    class IEventHost;
    typedef int (*EventHandler)(IEventHost*svc, int evid, void *context);

    /// event 및 해당 event handler를 매핑하는 클래스
    /**
     *  IEventHost를 상속받아 구현된 객체는 addEventHandler()함수를 활용하여 event-handler map을 만들 수 있다.
     *  event를 발생 시켜야하는 상에서는 setEvent()함수를 이용하여 해당 even handler callback함수가 불리게 한다.
     */
    class IEventHost {
    protected:
        virtual int setEvent(int evid)
        {
            auto handler = ev_map_.find(evid);
            if (handler != ev_map_.end()) {
                return handler->second.first(this, evid, handler->second.second);
            } else {
                //SEP_TRE("No matching handler for id %d\n", evid);
                return -1;
            }
        }
    public:
        virtual ~IEventHost(){};
        bool addEventHandler(int evid, EventHandler handler, void *context = nullptr)  // event overriting할 때 false 반환
        {
            std::lock_guard<std::mutex> guard(mutex_);
            bool ret = (ev_map_.find(evid) == ev_map_.end());
            ev_map_[evid]  = std::make_pair(handler, context);
            return ret;
        }
    private:
        std::map<int,std::pair<EventHandler,void*>> ev_map_;
        std::mutex mutex_;
    };
}
#endif //SEPMASTER_EVENT_HOST_H_H
