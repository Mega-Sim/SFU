#ifndef SUSI_ATLAS_H
#define SUSI_ATLAS_H

// clang-format off
#include "Susi4.h"
#include <mutex>

// clang-format on

namespace fpx {

class susi_atlas {
public:
    susi_atlas(void) { }
    susi_atlas(const susi_atlas&) = delete;
    susi_atlas& operator=(const susi_atlas&) = delete;
    ~susi_atlas(void) { }

    int init(void)
    {
        SusiStatus_t status;
        int ret = 0;

        status = SusiLibInitialize();

        if (status == SUSI_STATUS_ERROR) {
            ret = status;
            return ret;
        }
        if (status != SUSI_STATUS_SUCCESS && status != SUSI_STATUS_INITIALIZED) {
            ret = status;
            return ret;
        }

        susi_initflag = true;

        return ret;
    }

    bool is_susi_inited(void) { return susi_initflag; }

    SusiStatus_t smbsendbyte(uint8_t Addr, uint8_t Data)
    {
        return SusiSMBSendByte(SmdId, Addr, Data);
    }

    // 중요 합니다.
    // som6869의 susi에서 지원 하는 smb bus는 최대 100kHz까지만 지원 합니다.
    // 그리고, max11609의 데이타를 읽어 오기 위해서는
    // SusiSMBReadWord 함수를 사용해야 합니다.
    SusiStatus_t smbreceivebyte(uint8_t Addr, uint8_t cmd, uint8_t* pData)
    {
        SusiStatus_t status;
        uint16_t val = 0;
        status = SusiSMBReadWord(SmdId, Addr, cmd, &val);
        pData[0] = val & 0xff;
        pData[1] = (val >> 8) & 0xff;
        return status;
    }

    float get_voltage(int id)
    {
        uint32_t getVal;
        float convVal = 0.0;

        if (id >= 0 && id < 3) {
            auto status = SusiBoardGetValue(volt[id], &getVal);
            if (status == SUSI_STATUS_SUCCESS) {
                convVal = static_cast<float>(getVal);
            }
        }

        return convVal;
    }

    float get_cpu_temp(void)
    {
        uint32_t getVal;
        float convVal = 0.0;

        auto status = SusiBoardGetValue(SUSI_ID_HWM_TEMP_CPU, &getVal);
        if (status == SUSI_STATUS_SUCCESS) {
            convVal = SUSI_DECODE_CELCIUS(getVal);
        }
        return convVal;
    }

private:
    static std::mutex mutex_dev;
    bool susi_initflag { false };
    const SusiId_t SmdId { SUSI_ID_SMBUS_EXTERNAL };
    const int32_t volt[4] {
        SUSI_ID_HWM_VOLTAGE_BASE + 5,
        SUSI_ID_HWM_VOLTAGE_BASE + 6,
        SUSI_ID_HWM_VOLTAGE_BASE + 8,
        -1
    };
    const int32_t temp[1] {
        SUSI_ID_HWM_TEMP_CPU
    };
};

}

#endif
