/*
 * gpio.h
 *
 *  Created on: Mar 8, 2017
 *      Author: jccho
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <string>
#include <mutex>
#include "xr17v352_rt.h"

extern const int GPIO_OUTPUT;
extern const int GPIO_INPUT;
extern const int GPIO_HIGH;
extern const int GPIO_LOW;
extern const std::string GPIO_NONE;
extern const std::string GPIO_FALLING;
extern const std::string GPIO_RISING;
extern const std::string GPIO_BOTH;
extern const std::string SYSFS_GPIO_DIR;
extern const int MAX_BUF;

struct MasterBoardGpioDefault {
    static bool initialize() { return true; }
    static volatile uint32_t* din(int i) { return PseudoMmap(i-1); }
    static volatile uint32_t* dout(int i) { return PseudoMmap(i-1); }
};

class XR17V352InitClass {
public:
    XR17V352InitClass(void) {
       std::cout << "XR17V352InitClass" << std::endl;
       XR17V352Init();
    }
};

class Gpio {
public:
    Gpio(void) =delete;
    Gpio(const int gpio, const int dir, const int val, int index):
            filed_(-1),
            gpio_(gpio),
            direction_(dir),
            val_(val),
            exported_(false),
            index_(index),
            mmap_(nullptr), mmap_offset_(0) {XR17V352Init();};
    Gpio(const Gpio&) =delete;
    Gpio& operator=(const Gpio&) =delete;
    int gpio_export(void);
    int gpio_unexport(void);
    int gpio_get_dir(int& rdir);
    int gpio_set_dir(unsigned int dir);
    int gpio_get_val(unsigned int *val);
    int gpio_set_val(unsigned int val);
    int gpio_open_nonblock(void);
    int set_mmap(volatile uint32_t* mm, int offset);
    int gpio_close(void);
    int gpio_read(unsigned int *val);
    int gpio_write(unsigned int val);
    int gpio_init(void);
    int get_index() const { return index_; }
    bool isDi() const { return direction_ == GPIO_INPUT; }
protected:
    
private:
    int filed_;
    int gpio_;
    int direction_;
    int val_;
    std::string type_;
    bool exported_;
    int index_;
    volatile uint32_t* mmap_;
    int mmap_offset_;
};

#endif /* GPIO_H_ */
