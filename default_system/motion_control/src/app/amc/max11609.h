#ifndef MAX11609_H
#define MAX11609_H

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "Susi4.h"
#include "susi_atlas.h"

namespace fpx {

#define SETUPBYTE(sel) (uint8_t)((1 << 7) | (sel) << 4 | 0x02)
#define CONFBYTE(scan, cs) (uint8_t)((0 << 7) | (scan) << 5 | (cs) << 1 | 0x01)

typedef enum {
    MAIN_50V = 0,
    IO_33V,
    CONTROL_12V,
    CONTROL_50V,
    CONTROL_33V,
    END_OF_CHANNEL
} tChannel;

typedef struct {
    float hr;
    float lr;
} resisval;

class max11609 {
public:
    max11609(void) = delete;
    max11609(const max11609&) = delete;
    max11609(std::unique_ptr<susi_atlas>& pSusi_) : pSusi(pSusi_) {};
    ~max11609(void) {};
    int readchannel(tChannel ch, float& mV);
    int susiinit(void);

private:
    const uint8_t addr { 0x66 };
    const float vref { 2.048 };
    const float resol { 1024 };
    const resisval divres[END_OF_CHANNEL] {
        { 13000, 5600 },
        { 5600, 4700 },
        { 91000, 13000 },
        { 13000, 5600 },
        { 5600, 4700 },
    };

    float to_voltage(tChannel ch, uint8_t* val);
    std::unique_ptr<susi_atlas>& pSusi;
};

} // namespace fpx

#endif