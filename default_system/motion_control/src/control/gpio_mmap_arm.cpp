//
// Created by sem on 18. 6. 18.
//

#ifdef MASTER_ARM_BOARD

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "gpio_mmap_arm.h"

enum CONSTANTS {
    GPIO_COUNT = 8,
    GPIO_SIZE = 0x2000,
    GPIO1_START_ADDR = 0x4AE10000,
    GPIO1_END_ADDR = GPIO1_START_ADDR + GPIO_SIZE,
    GPIO2_START_ADDR = 0x48055000,
    GPIO2_END_ADDR = GPIO2_START_ADDR + GPIO_SIZE,
    GPIO3_START_ADDR = 0x48057000,
    GPIO3_END_ADDR = GPIO3_START_ADDR + GPIO_SIZE,
    GPIO4_START_ADDR = 0x48059000,
    GPIO4_END_ADDR = GPIO4_START_ADDR + GPIO_SIZE,
    GPIO5_START_ADDR = 0x4805B000,
    GPIO5_END_ADDR = GPIO5_START_ADDR + GPIO_SIZE,
    GPIO6_START_ADDR = 0x4805B000,
    GPIO6_END_ADDR = GPIO6_START_ADDR + GPIO_SIZE,
    GPIO7_START_ADDR = 0x48051000,
    GPIO7_END_ADDR = GPIO7_START_ADDR + GPIO_SIZE,
    GPIO8_START_ADDR = 0x48053000,
    GPIO8_END_ADDR = GPIO8_START_ADDR + GPIO_SIZE,

    GPIO_DATAIN = 0x138,
    GPIO_DATAOUT = 0x13C,
};

struct GpioPhysicalInfo {
    uint32_t    addr;
    uint32_t    size;
};

static const GpioPhysicalInfo gpio_addr_map[GPIO_COUNT] = {
        {GPIO1_START_ADDR, GPIO_SIZE},
        {GPIO2_START_ADDR, GPIO_SIZE},
        {GPIO3_START_ADDR, GPIO_SIZE},
        {GPIO4_START_ADDR, GPIO_SIZE},
        {GPIO5_START_ADDR, GPIO_SIZE},
        {GPIO6_START_ADDR, GPIO_SIZE},
        {GPIO7_START_ADDR, GPIO_SIZE},
        {GPIO8_START_ADDR, GPIO_SIZE},
};
static volatile void *gpiochip_addr[8];
static volatile uint32_t *gpiochip_datain_addr[8];
static volatile uint32_t *gpiochip_dataout_addr[8];

bool MasterArmBoardGpio::initialize()
{
    int fd{-1};
    SEP_REQUIRE_RET( (fd = open("/dev/mem", O_RDWR|O_SYNC)) >= 0, false);

    for (int i=0; i < GPIO_COUNT; i++) {
        SEP_LOGT("GPIO Mapping %X - %X (size: %X)\n",
               gpio_addr_map[i].addr, gpio_addr_map[i].addr+gpio_addr_map[i].size, gpio_addr_map[i].size);
        gpiochip_addr[i] = mmap(0, GPIO_SIZE, PROT_READ|PROT_WRITE,
                                MAP_SHARED, fd, gpio_addr_map[i].addr);
        if (gpiochip_addr[i] == MAP_FAILED) {
            SEP_LOGE("Unable to map GPIO\n");
            return false;
        }

        gpiochip_datain_addr[i] = (uint32_t *)((char*)gpiochip_addr[i] + GPIO_DATAIN);
        gpiochip_dataout_addr[i] = (uint32_t *)((char*)gpiochip_addr[i] + GPIO_DATAOUT);
        SEP_LOGT("GPIO%d CHIP mapped to %p\n", i, gpiochip_addr[i]);
        SEP_LOGT("GPIO%d CHIP DATAIN  mapped to %p\n", i, gpiochip_datain_addr[i]);
        SEP_LOGT("GPIO%d CHIP DATAOUT  mapped to %p\n", i, gpiochip_dataout_addr[i]);
    }

    return true;
}

volatile uint32_t* MasterArmBoardGpio::din(int i)
{
    SEP_REQUIRE_RET(i>=0 && i<GPIO_COUNT, nullptr);
    return gpiochip_datain_addr[i];
}

volatile uint32_t* MasterArmBoardGpio::dout(int i)
{
    SEP_REQUIRE_RET(i>=0 && i<GPIO_COUNT, nullptr);
    return gpiochip_dataout_addr[i];
}

#endif //MASTER_ARM_BOARD
