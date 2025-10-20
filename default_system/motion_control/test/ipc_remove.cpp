//
// Created by sem1 on 17. 7. 26.
//

#include "sephi_ipc.h"
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <stdio.h>
using namespace sephi;

int main(int argc, const char* argv[])
{
    const char* key_names[] = {
            "sephi_ecat_cyclic_event",
            "g_message_queue",
    };

    for(auto name : key_names) {
        try{
            named_mutex::remove(name);
            named_condition::remove(name);
            printf("Removing %s\n", name);
        }catch (...)
        {
            printf("Fail to remove key %s\n", name);
        }
    }

}
