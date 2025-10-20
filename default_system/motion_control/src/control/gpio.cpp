/*
 * gpio.cpp
 *
 *  Created on: Mar 7, 2017
 *      Author: jccho
 */

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string>
//#include "log.h"
#include "gpio.h"

#define print_msg(io, msgtype, arg...) \
    flockfile(io); \
    fprintf(io, "["#msgtype"] [%s/%s:%03d] ", __FILE__, __FUNCTION__, __LINE__); \
    fprintf(io, arg); \
    fputc('\n', io); \
    funlockfile(io);
#define pr_err(arg...) print_msg(stderr, ERR, arg)
#define pr_out(arg...) print_msg(stdout, REP, arg)


const int GPIO_OUTPUT           = 0;
const int GPIO_INPUT            = 1;
const int GPIO_HIGH             = 1;
const int GPIO_LOW              = 0;
// 이 상수들은 atlas보드에서는 사용되지 않습니다.
#if 0
const std::string GPIO_NONE     = "none";
const std::string GPIO_FALLING  = "falling";
const std::string GPIO_RISING   = "rising";
const std::string GPIO_BOTH     = "both";
const std::string SYSFS_GPIO_DIR = "/sys/class/gpio";
const int MAX_BUF               = 64;
#endif


// 이 멤버함수는 atlas 보드에서는 역활을 하지 않습니다.
int Gpio::gpio_export(void)
{
	exported_ = true;
	return 0;
}

// 이 멤버함수는 atlas 보드에서는 역확을 하지 않습니다.
int Gpio::gpio_unexport(void)
{
    exported_ = false;
    return 0;
}

// 이 멤버함수는 atlas 보드에서는
// xr17v352_getmpiosel 함수를 사용하여 구현 합니다.
int Gpio::gpio_get_dir(int& rdir)
{
    uint64_t selval;
    int ret;

    ret = xr17v352_getmpiosel(GetDIOHandle(), &selval);
    if (ret != 0) {
        return ret;
    }
    rdir = (selval & (1<<gpio_))?GPIO_INPUT:GPIO_OUTPUT;

    return ret;

#if 0
    char buf[MAX_BUF];
    std::string GPIO_DIR_PATH = SYSFS_GPIO_DIR + "/gpio%d/direction";
    snprintf(buf, sizeof(buf), GPIO_DIR_PATH.c_str(), gpio_);
    int fd = open(buf, O_RDONLY);
    if (fd < 0) {
        pr_err("Can't get GPIO %d pin direction: %s", gpio_, strerror(errno));
        return fd;
    }
    read(fd, buf, MAX_BUF);
    close(fd);
    if (strcmp(buf, "in") == 0) {
        rdir = GPIO_INPUT;
    }
    else {
        rdir = GPIO_OUTPUT;
    }
    return 0;
#endif
}

int Gpio::gpio_set_dir(unsigned int dir)
{
    uint64_t selval;
    int ret;

    ret = xr17v352_getmpiosel(GetDIOHandle(), &selval);
    if (ret != 0) {
        return ret;
    }
    if (dir == GPIO_INPUT) {
        selval |= (1ULL << gpio_);
    } else {
        selval &= ~(1ULL << gpio_);
    }
    ret = xr17v352_setmpiosel(GetDIOHandle(), selval);

    return ret;
#if 0
    char buf[MAX_BUF];
    std::string GPIO_DIR_PATH = SYSFS_GPIO_DIR + "/gpio%d/direction";
    snprintf(buf, sizeof(buf), GPIO_DIR_PATH.c_str(), gpio_);
    int fd = open(buf, O_WRONLY);
    if (fd < 0) {
        pr_err("Can't set GPIO %d pin direction: %s", gpio_, strerror(errno));
        return fd;
    }
    if (dir == GPIO_OUTPUT) {
        write(fd, "out", 4);
    } else {
        write(fd, "in", 3);
    }
    close(fd);
    return 0;
#endif
}

int Gpio::gpio_get_val(unsigned int *val)
{
    static std::mutex lock;
    std::lock_guard<std::mutex> guard(lock);

    ReadXR17V352Data();
	if (mmap_ != nullptr) {
        *val = ((*mmap_) & (1<<mmap_offset_)) ? GPIO_HIGH : GPIO_LOW;
        return 0;
    }

#if 0
    char buf[MAX_BUF];
    std::string GPIO_VAL_PATH = SYSFS_GPIO_DIR + "/gpio%d/value";
    snprintf(buf, sizeof(buf), GPIO_VAL_PATH.c_str(), gpio_);
    int fd = open(buf, O_RDONLY);
    if (fd < 0) {
        pr_err("Can't get GPIO %d pin value: %s", gpio_, strerror(errno));
        return fd;
    }
    read(fd, buf, 1);
    close(fd);
    if (*buf != '0') {
        *val = GPIO_HIGH;
    } else {
        *val = GPIO_LOW;
    }
    return 0;
#endif
}

int Gpio::gpio_set_val(unsigned int val)
{
    static std::mutex lock;
    std::lock_guard<std::mutex> guard(lock);

    if (mmap_ != nullptr) {
        if (val) {
            *mmap_ |= (1UL << mmap_offset_);
        } else {
            *mmap_ &= ~(1UL << mmap_offset_);
        }
        WriteXR17V352Data();
        return 0;
    }

#if 0
    char buf[MAX_BUF];
    std::string GPIO_VAL_PATH = SYSFS_GPIO_DIR + "/gpio%d/value";
    snprintf(buf, sizeof(buf), GPIO_VAL_PATH.c_str(), gpio_);
    int fd = open(buf, O_WRONLY);
    if (fd < 0) {
        pr_err("Can't set GPIO %d pin value: %s", gpio_, strerror(errno));
        return fd;
    }
    if (val == GPIO_HIGH) {
        write(fd, "1", 2);
    } else {
        write(fd, "0", 2);
    }
    close(fd);
    return 0;
#endif
}

// 이 멤버 함수는 atlas보드에서는 역활이 없습니다.
// output
// 0 : success
int Gpio::gpio_open_nonblock(void)
{
	return 0;
#if 0
    char buf[MAX_BUF];
    std::string GPIO_VAL_PATH = SYSFS_GPIO_DIR + "/gpio%d/value";
    snprintf(buf, sizeof(buf), GPIO_VAL_PATH.c_str(), gpio_);
    int fd = open(buf, O_RDWR|O_NONBLOCK);
    if (fd < 0) {
        pr_err("Can't set GPIO %d pin value: %s", gpio_, strerror(errno));
    }
    filed_ = fd;
    return fd;
#endif

}

int Gpio::set_mmap(volatile uint32_t* mm, int offset)
{
    mmap_ = mm;
    mmap_offset_ = offset;
    return 0;
}

// 이 멤버 함수는 atlas보드에서는 역활이 없습니다.
// output
// 0 : success
int Gpio::gpio_close(void)
{
	  return 0;

#if 0
    int ret = close(filed_);
    filed_ = -1;
    return ret;
#endif



}

static std::mutex rwlock;

int Gpio::gpio_read(unsigned int *val)
{
    // static std::mutex lock;
    std::lock_guard<std::mutex> guard(rwlock);

//    std::cout << "Gpio::gpio_read() called" << std::endl;
    ReadXR17V352Data();
    if (mmap_ != nullptr) {
        *val = ((*mmap_) & (1<<mmap_offset_)) ? GPIO_HIGH : GPIO_LOW;
        return 0;
    }
    return -1;

#if 0
    char ch = 0;
    lseek(filed_, 0, SEEK_SET);
    int ret = read(filed_, &ch, 1);
    if (ret != 1) {
        pr_err("Can't read fd: %d", filed_);
        return ret;
    }

    if (ch != '0') {
        *val = GPIO_HIGH;
    } else {
        *val = GPIO_LOW;
    }
    return 0;
#endif
}

int Gpio::gpio_write(unsigned int val)
{
    // static std::mutex lock;
    std::lock_guard<std::mutex> guard(rwlock);

    if (mmap_ != nullptr) {
        if (val) {
            *mmap_ |= (1UL << mmap_offset_);
        } else {
            *mmap_ &= ~(1UL << mmap_offset_);
        }
        WriteXR17V352Data();
        return 0;
    }

#if 0
    int ret;

    lseek(filed_, 0, SEEK_SET);
    if (val == GPIO_HIGH) {
        ret = write(filed_, "1", 2);
    } else {
        ret = write(filed_, "0", 2);
    }
    if (ret == -1) {
        pr_err("Can't write fd: %d", filed_);
        return ret;
    }
    return 0;
#endif
}

int Gpio::gpio_init(void)
{
    int ret = gpio_export();
    if (ret != 0) {
        pr_err("[Gpio] Can't export");
        return ret;
    }
    ret = gpio_set_dir(direction_);
    if (ret != 0) {
        pr_err("[Gpio] Can't set dir");
        return ret;
    }
    ret = gpio_set_val(val_);
    return ret;
}







