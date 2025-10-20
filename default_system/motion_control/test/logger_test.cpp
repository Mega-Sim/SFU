#include <stdio.h>
#include <iostream>
#include "log.h"

using std::string;

void testfn();

int main(void)
{
    for(int c = 0; c < 100; ++c) {
        printf("send log %d\n", c);
        SEP_LOGD("log test. Hello? %d\n", c);
        usleep(0);
    }
    return 0;

}