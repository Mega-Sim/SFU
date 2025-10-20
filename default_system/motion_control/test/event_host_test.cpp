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
// Created by 정순용 on 2017. 4. 26..
//

#include "event_host.h"
using namespace sephi;

class Test : public IEventHost
{

public:
    std::string name;
    enum EVENT : int {
        EV1,
        EV2,
    };
    Test(const char* name_) {
        name = name_;
    }
    int run(int evid) {
        this->setEvent(evid);
    }
};

static int myHandler(IEventHost*ins, int evid, void *arg)
{
    auto svc = (Test*)ins;
    printf("[%s] Event %d occurred!\n", svc->name.c_str(), evid);
    return 0;
}

int main()
{
    Test t("mytest");
    t.addEventHandler(t.EV1, myHandler);
    t.addEventHandler(t.EV2, myHandler);
    t.addEventHandler(t.EV2, myHandler);

    t.run(t.EV2);
    t.run(t.EV1);
    t.run(3);
    return 0;
}