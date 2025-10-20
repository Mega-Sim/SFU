
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
// Created by sem on 18. 4. 26.
//

#ifndef SEPMASTER_ECMASTER_TYPEDEF_H_H
#define SEPMASTER_ECMASTER_TYPEDEF_H_H

#if defined(USE_ACONTIS)
#include <AtEthercat.h>
#include "acontis/ecat_notification.h"

#define EcVoid          EC_T_VOID
namespace  sephi {
// extern
    EC_T_BOOL OsDbgMsgHookWrapper(const EC_T_CHAR *szFormat, EC_T_VALIST vaArgs);
    typedef struct _EC_T_MBXTFER EcMailboxTrans;
    typedef EC_T_CNF_TYPE EcConfigType;
    typedef EC_T_BYTE EcByte;
    typedef EC_T_PBYTE EcBytePtr;
    typedef EC_T_DWORD EcDWord;
    typedef EC_T_INT EcInt;
    typedef EC_T_BOOL EcBool;
    typedef EC_T_LINK_PARMS EcLinkParams;
    typedef EC_T_OS_PARMS EcOsParams;
    typedef EC_T_DCM_MODE EcDcmMode;
    typedef EC_T_CHAR EcChar;
    typedef EC_T_NOTIFYPARMS EcNotifyParam;
    typedef EC_T_STATE EcState;
}
#elif defined(USE_ETHERLAB)
#include <ecrt.h>
#include <Poco/SingletonHolder.h>
#define EcDebug         SEP_TRACE
#define EcVoid          void
#define EC_E_NOERROR    0
namespace  sephi {
    typedef int EcConfigType;
    typedef uint8_t EcByte;
    typedef uint8_t *EcBytePtr;
    typedef int32_t EcDWord;
    typedef int EcInt;
    typedef int EcBool;
    typedef char EcChar;
    enum EcState {
        eEcatState_UNKNOWN = 0,                        /*< unknown */
        eEcatState_INIT = 1,                        /*< init */
        eEcatState_PREOP = 2,                        /*< pre-operational */
        eEcatState_SAFEOP = 4,                        /*< safe operational */
        eEcatState_OP = 8,                        /*< operational */
        eEcatState_BOOTSTRAP = 3,                       /*< BootStrap */
    };
    struct EcLinkParams {
    };
    struct EcOsParams {
    };
    enum class EcDcmMode {
        OFF, BUS_SHIFT, MASTER_SHIFT
    };
    struct EcNotifyParam {
    };

    class CEmNotification {
    };

    class CAtEmLogging {
    };

    struct EcMailboxTrans {

    };


    extern ec_master_t *the_ecmaster;
    extern uint32_t the_ecmaster_abort_code;

    inline int ecrt_master_sdo_upload_ex(
            ec_master_t *master, /**< EtherCAT master. */
            uint16_t slave_position, /**< Slave position. */
            uint16_t index, /**< Index of the SDO. */
            uint8_t subindex, /**< Subindex of the SDO. */
            uint8_t *target, /**< Target buffer for the upload. */
            size_t target_size, /**< Size of the target buffer. */
            int *result_size, /**< Uploaded data size. */
            uint32_t *abort_code /**< Abort code of the SDO upload. */
    ) {
        size_t out_size;
        int res = ecrt_master_sdo_upload(master, slave_position, index, subindex, target, target_size, &out_size,
                                         abort_code);
        if (result_size) *result_size = out_size;
        return res;
    }
}
#define ecatCoeSdoDownloadEx(dwSlaveId, wObIndex, byObSubIndex, pbyData, dwDataLen, dwTimeout, dwFlags) \
    (sephi::the_ec_emulator.get()->emulationMode() ?  sephi::the_ec_emulator.get()->coeSdoDownload(dwSlaveId, wObIndex, byObSubIndex, pbyData, dwDataLen, dwFlags) \
        : ecrt_master_sdo_download(the_ecmaster, dwSlaveId, wObIndex, byObSubIndex, pbyData, dwDataLen, &the_ecmaster_abort_code))

#define ecatCoeSdoUploadEx(dwSlaveId, wObIndex, byObSubIndex, pbyData, dwDataLen, pdwOutDataLen, dwTimeout, dwFlags) \
    (sephi::the_ec_emulator.get()->emulationMode() ?  sephi::the_ec_emulator.get()->coeSdoUpload_i(dwSlaveId, wObIndex, byObSubIndex, pbyData, dwDataLen, pdwOutDataLen, dwFlags) \
        : ecrt_master_sdo_upload_ex(the_ecmaster, dwSlaveId, wObIndex, byObSubIndex, pbyData, dwDataLen, pdwOutDataLen, &the_ecmaster_abort_code))

#else // no valid ecmaster
#error("INVALID ECMASTER USE OPTION!")
#endif

#endif //SEPMASTER_ECMASTER_TYPEDEF_H_H
