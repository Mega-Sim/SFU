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
// Created by master on 17. 3. 29.
//

#ifndef SEPMASTER_ECMASTER_APP_H
#define SEPMASTER_ECMASTER_APP_H

#include "ecmaster_typedef.h"
#include "sephi_ipc.h"
#include "ecmaster_config.h"
#include "ecat_data_struct.h"
#include "ecmaster_task.h"
namespace  sephi {
    /**\addtogroup grp_ecmaster
     * \{
     */

#if defined(USE_ACONTIS)
#endif

    // ecmaster command line option class
    class EmCommandLineOption {
    public:
        EcConfigType         eCnfType;
        EcBytePtr            pbyCnfData;
        EcDWord              dwCnfDataLen;
        EcDWord              dwBusCycleTimeUsec;
        EcInt                nVerbose;
        EcDWord              dwDuration;
        EcLinkParams*        apLinkParms[MAX_LINKLAYER];
        EcDWord              dwCpuIndex;
        EcBool               bEnaPerfJobs;
        EcDWord              dwNumLinkLayer;
        EcOsParams           oOsParms;
        EcDcmMode            eDcmMode;
        EcBool               bMailboxOnly;
        EcVoid*              pvCfgFileHandle;
        bool                 emulation_mode;
        bool                 set_mode;
        std::string          enifile; // eni yaml file
        std::string          modelfile; // model yaml file
        std::list<string>    slaves;

    public:
        EmCommandLineOption(int argc, char* argv[]);
        int error();
        void print();
    private:
        bool parse(int argc, char* argv[]);

        bool error_code_;
    };
    /// \}
}
#endif //SEPMASTER_ECMASTER_APP_H
