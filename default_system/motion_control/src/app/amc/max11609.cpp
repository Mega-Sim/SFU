// clang-format off
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include "Susi4.h"
#include "max11609.h"

using namespace fpx;
using namespace std;
// clang-format on

int max11609::susiinit(void)
{
    int ret = 0;

    if (pSusi == nullptr) {
        pSusi = std::make_unique<susi_atlas>();
        ret = pSusi->init();
    }

    return ret;
}

// input: ch, val
// output: voltage (mV)
float max11609::to_voltage(tChannel ch, uint8_t* val)
{
    uint16_t value;
    float mV;

    value = ((val[0] << 8) + val[1]) & 0x03ff;
    auto ratio = (divres[ch].hr + divres[ch].lr) / divres[ch].lr;
    mV = 1000.0 * value * ratio * vref / resol;

    return mV;
}

int max11609::readchannel(tChannel ch, float& mV)
{
    uint8_t setupval;
    uint8_t confval;
    uint8_t vals[8] {};

    if (ch >= MAIN_50V && ch < END_OF_CHANNEL) {

        setupval = SETUPBYTE(5);
        confval = CONFBYTE(3, ch);

        pSusi->smbsendbyte(addr, setupval);
        // pSusi->smbsendbyte(addr, confval);

        auto status = pSusi->smbreceivebyte(addr, confval, vals);
        if (status != SUSI_STATUS_SUCCESS) {
            mV = 0.0;
            return -1;
        }

        mV = to_voltage(ch, vals);
    } else {
        mV = 0;
    }
    return 0;
}