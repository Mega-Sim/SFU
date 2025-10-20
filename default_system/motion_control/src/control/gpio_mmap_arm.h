//
// Created by sem on 18. 6. 18.
//

#ifndef SEPMASTER_GPIO_MMAP_ARM_H
#define SEPMASTER_GPIO_MMAP_ARM_H
#include "log.h"
#ifdef MASTER_ARM_BOARD
struct MasterArmBoardGpio {
    static bool initialize();
    static volatile uint32_t* din(int i);
    static volatile uint32_t* dout(int i);
};
#endif

#endif //SEPMASTER_GPIO_MMAP_ARM_H
