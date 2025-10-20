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
#include <yaml-cpp/yaml.h>
#include "ecmaster_service.h"
#include "slave_info.h"
#include "acontis/ecat_common_info.h"
#include "acontis/ecat_notification.h"

namespace sephi {
    extern EcMasterSvc *the_EcMasterSvc_ins;

    int ecReceiveFrames(int &bPrevCycProcessed, int &nOverloadCounter) {
        int res = ecatExecJob(eUsrJob_ProcessAllRxFrames, &bPrevCycProcessed);
        if (EC_E_NOERROR != res && EC_E_INVALIDSTATE != res && EC_E_LINK_DISCONNECTED != res) {
            SEP_LOGE("ERROR: emExecJob( eUsrJob_ProcessAllRxFrames): %s (0x%lx)", ecatGetText(res), res);
        }

        if (EC_E_NOERROR == res) {
            if (!bPrevCycProcessed) {
                // it is not reasonable, that more than 5 continuous frames are lost
                nOverloadCounter += 10;
                if (nOverloadCounter >= 50) {
                    SEP_LOGE("Error: System overload: Cycle time too short or huge jitter!");
                } else {
                    SEP_LOGE(
                            "eUsrJob_ProcessAllRxFrames - not all previously sent frames are received/processed (frame loss)!");
                }
            } else {
                // everything o.k.? If yes, decrement overload counter
                if (nOverloadCounter > 0) nOverloadCounter--;
            }
        }

        // handle DCM logging
#ifdef DCM_ENABLE_LOGFILE
        PERF_JOB_START(PERF_DCM_Logfile);
        {
            EC_T_CHAR* pszLog = EC_NULL;

            emDcmGetLog(dwInstanceID, &pszLog);
            if ((EC_NULL != pszLog))
            {
                LogDcm(pszLog);
            }
        }
        PERF_JOB_END(PERF_DCM_Logfile);
#endif

        return res;
    }

    int ecSendFrames(bool &exit_loop) {
        // write output values of current cycle, by sending all cyclic frames
        int res = ecatExecJob(eUsrJob_SendAllCycFrames, EC_NULL);
        if (EC_E_NOERROR != res && EC_E_INVALIDSTATE != res && EC_E_LINK_DISCONNECTED != res) {
            SEP_LOGE("emExecJob( eUsrJob_SendAllCycFrames,    EC_NULL ): %s (0x%lx)", ecatGetText(res), res);
        }

        // remove this code when using licensed version
        if (EC_E_EVAL_EXPIRED == res) {
            SEP_LOGE("Evaluation License Expired!!!");
            exit_loop = true;
        }

        // Execute some administrative jobs. No bus traffic is performed by this function
        res = ecatExecJob(eUsrJob_MasterTimer, EC_NULL);
        if (EC_E_NOERROR != res && EC_E_INVALIDSTATE != res) {
            SEP_LOGE("emExecJob( eUsrJob_MasterTimer,    EC_NULL ): %s (0x%lx)", ecatGetText(res), res);
        }

        // send queued acyclic EtherCAT frames
        res = ecatExecJob(eUsrJob_SendAcycFrames, EC_NULL);
        if (EC_E_NOERROR != res && EC_E_INVALIDSTATE != res && EC_E_LINK_DISCONNECTED != res) {
            SEP_LOGE("emExecJob( eUsrJob_SendAcycFrames,    EC_NULL ): %s (0x%lx)", ecatGetText(res), res);
        }
        return res;
    }

    // check ethercat state at each cycle
    void EcMasterSvc::checkECatStates() {

    }

    EC_T_DWORD EcMasterSvc::ecatNotifyCallback(
            EC_T_DWORD dwCode,  /**< [in]   Notification code */
            EC_T_NOTIFYPARMS *pParms   /**< [in]   Notification parameters */
    ) {
        EC_T_DWORD dwRetVal = EC_E_NOERROR;
        CEmNotification *pNotifyInstance;

        SEP_REQUIRE_RET((EC_NULL != pParms) && (EC_NULL != pParms->pCallerData), -1);

        if ((dwCode == 0x1000F) || ((dwCode != 0x1000F) && (dwCode ==
                                                            0x00010014))) { //EC_NOTIFY_NOT_ALL_DEVICES_OPERATIONAL. 또는 EC_NOTIFY_SLAVE_ERROR_STATUS_INFO 일 때 OP 상태로 Recovery 실시
            the_EcMasterSvc_ins->requestRecoverOpMode();
        }

        /* notification for application ? */
        if ((dwCode >= EC_NOTIFY_APP) && (dwCode <= EC_NOTIFY_APP + EC_NOTIFY_APP_MAX_CODE)) {
            dwRetVal = ((EcMasterSvc *) pParms->pCallerData)->ecatNotifyCallbackHandler(dwCode - EC_NOTIFY_APP, pParms);
        } else {
            dwRetVal = ((EcMasterSvc *) pParms->pCallerData)->noti_->ecatNotify(dwCode, pParms);
        }

        Exit:
        return dwRetVal;
    }

    void EcMasterSvc::shutdownMaster() {
        ecatStop(ECAT_STATE_CHANGE_TIMEOUT_MS);
        ecatUnregisterClient(client_id);
        client_id = -1;
        ecatDeinitMaster();
    }

    void EcMasterSvc::checkDcmError() {
        SEP_LOGE("Cannot start set master state to SAFEOP\n");
        /* most of the time SAFEOP is not reachable due to DCM */
        if (DCM_ENABLED && eDcmMode_Off != opt_->eDcmMode) {
            EC_T_DWORD dwDcmStatus = 0;
            EC_T_INT nDiffCur = 0, nDiffAvg = 0, nDiffMax = 0;

            int dwRes = ecatDcmGetStatus(&dwDcmStatus, &nDiffCur, &nDiffAvg, &nDiffMax);
            if (dwRes == EC_E_NOERROR) {
                if (dwDcmStatus != EC_E_NOERROR) {
                    SEP_LOGE("DCM Status: %s (0x%08X)\n", ecatGetText(dwDcmStatus), dwDcmStatus);
                }
            } else {
                SEP_LOGE("Cannot get DCM status! %s (0x%08X)\n", ecatGetText(dwRes), dwRes);
            }
        }
    }

    bool EcMasterSvc::checkDcmStatus() {
        SEP_LOGI("checkDcmStatus() is not implemented!\n");
        return true;
    }


    bool EcMasterSvc::initMaster() {
        // noti 객체 초기화
        nDcmCtlSetVal_ = DCM_CTRL_SET_VAL;
        noti_ = std::make_unique<CEmNotification>(INSTANCE_MASTER_DEFAULT, &logger_);

        /* set type of OsSleep implementation  (eSLEEP_USLEEP, eSLEEP_NANOSLEEP or eSLEEP_CLOCK_NANOSLEEP) */
        OsSleepSetType(eSLEEP_CLOCK_NANOSLEEP);


        EC_T_BOOL OsDbgMsgHookWrapper(const EC_T_CHAR *szFormat, EC_T_VALIST vaArgs);
        EC_T_INIT_MASTER_PARMS oInitParms;
        EC_T_DWORD dwRetVal = EC_E_NOERROR;
        EC_T_DWORD dwRes = EC_E_NOERROR;
        EC_T_BOOL bRes = EC_FALSE;

        OsMemset(&oInitParms, 0, sizeof(EC_T_INIT_MASTER_PARMS));
        oInitParms.dwSignature = ATECAT_SIGNATURE;
        oInitParms.dwSize = sizeof(EC_T_INIT_MASTER_PARMS);
        oInitParms.pLinkParms = opt_->apLinkParms[0];
        oInitParms.pLinkParmsRed = ((2 == opt_->dwNumLinkLayer) ? opt_->apLinkParms[1] : EC_NULL);
        oInitParms.dwBusCycleTimeUsec = opt_->dwBusCycleTimeUsec;
        oInitParms.dwMaxBusSlaves = MASTER_CFG_ECAT_MAX_BUS_SLAVES;
        oInitParms.dwMaxQueuedEthFrames = MASTER_CFG_MAX_QUEUED_ETH_FRAMES;
        oInitParms.dwMaxSlaveCmdPerFrame = MASTER_CFG_MAX_SLAVECMD_PER_FRAME;
        if (opt_->dwBusCycleTimeUsec < 1000) {
            oInitParms.dwMaxSentQueuedFramesPerCycle = 1;
        } else {
            oInitParms.dwMaxSentQueuedFramesPerCycle = MASTER_CFG_MAX_SENT_QUFRM_PER_CYC;
        }
        oInitParms.dwEcatCmdMaxRetries = MASTER_CFG_ECAT_CMD_MAX_RETRIES;
        oInitParms.dwEoETimeout = MASTER_CFG_EOE_TIMEOUT;
        oInitParms.dwFoEBusyTimeout = MASTER_CFG_FOE_BUSY_TIMEOUT;
        oInitParms.dwLogLevel = opt_->nVerbose;
        oInitParms.pfLogMsgCallBack = OsDbgMsgHookWrapper;
        oInitParms.pOsParms = &opt_->oOsParms;

        dwRes = ecatInitMaster(&oInitParms);
        if (dwRes != EC_E_NOERROR) {
            dwRetVal = dwRes;
            SEP_LOGE("Cannot initialize EtherCAT-Master! (Result = %s 0x%x)", ecatGetText(dwRes), dwRes);
            goto Exit;
        }

        return true;

        Exit:
        return false;
    }
    
    std::vector<std::string> splitCSVLine(const std::string& line){
		std::vector<std::string> result;
		std::stringstream ss(line);
		std::string item;
		
		while(std::getline(ss, item, ',') ){
			result.push_back(item);
		}
		
		return result;
	}	

    void EcMasterSvc::checkLicense() {
        std::string strbuf;
        std::ifstream in("/home/semes/work/bin/acontis_license.csv");

        // print mac address
        ETHERNET_ADDRESS oSrcMacAddress;
        ecatGetSrcMacAddress(&oSrcMacAddress);

        std::stringstream ss[6];
        for (int i = 0; i < 6; i++) {
            ss[i] << std::hex << std::setw(2) << std::setfill('0') << (unsigned int) oSrcMacAddress.b[i];
        }

        std::string mac_addr;
        for (int i = 0; i < 6; i++) {
            mac_addr += ss[i].str();
            if (i < 5) mac_addr += "-";
        }

        std::transform(mac_addr.begin(), mac_addr.end(), mac_addr.begin(), toupper);
        std::cout << "MAC Address : " << mac_addr << std::endl;

        if (in.fail()) {
            printf("File not found\n");
            return;
        }
        
        std::string line;
        while(std::getline(in, line)){
			std::vector<std::string> items = splitCSVLine(line);
									
			for(int i=0; i < items.size(); i++) {
				if(items[i] == mac_addr){
					strbuf = items[i+1];
					auto er = ecatSetLicenseKey((EC_T_CHAR *) strbuf.c_str());
					if (er == 0) SEP_LOGE("EC-Master license OK!, key : %s", strbuf.c_str());
                    else SEP_LOGE("EC-Master license not found!");
                    in.close();
                    return;
				}
					
			}
			std::cout << std::endl;
		}
		in.close();
    }

    bool EcMasterSvc::configureMaster() {
        auto &eCnfType = opt_->eCnfType;
        auto &pbyCnfData = opt_->pbyCnfData;
        auto &dwCnfDataLen = opt_->dwCnfDataLen;
        auto &eDcmMode = opt_->eDcmMode;
        auto &dwBusCycleTimeUsec = opt_->dwBusCycleTimeUsec;
        auto &nVerbose = opt_->nVerbose;

        /* Configure master */
        EC_T_DWORD dwRetVal = EC_E_NOERROR;
        EC_T_DWORD dwRes = ecatConfigureMaster(eCnfType, pbyCnfData, dwCnfDataLen);
        if (dwRes != EC_E_NOERROR) {
            dwRetVal = dwRes;
            SEP_LOGE("Cannot configure EtherCAT-Master! %s (Result = 0x%x)\n", ecatGetText(dwRes), dwRes);
            goto Exit;
        }
        /* configure DC/DCM if master is started with ENI */
        if (pbyCnfData != EC_NULL) {
            /* configure DC */
            EC_T_BOOL bFirstDcmStatus = EC_TRUE;
            EC_T_INT nDcmCtlSetVal =
                    //((dwBusCycleTimeUsec * 2) / 3) * 1000; /* set value in nanosec, 66% of the bus cycle */
                    ((dwBusCycleTimeUsec * 1) / 2) * 1000; /* set value in nanosec, 50% of the bus cycle */
            EC_T_BOOL bDcmCtlOff = EC_FALSE;
            EC_T_DC_CONFIGURE oDcConfigure;

            OsMemset(&oDcConfigure, 0, sizeof(EC_T_DC_CONFIGURE));
            oDcConfigure.dwTimeout = ETHERCAT_DC_TIMEOUT;
            oDcConfigure.dwDevLimit = ETHERCAT_DC_DEV_LIMIT;// (DCM_ENABLED) ? ETHERCAT_DC_DEV_LIMIT : 0;
            oDcConfigure.dwSettleTime = ETHERCAT_DC_SETTLE_TIME;
            if (eDcmMode_MasterRefClock == eDcmMode) {
                oDcConfigure.dwTotalBurstLength = 10000;
                oDcConfigure.dwBurstBulk = 1;
            } else {
                oDcConfigure.dwTotalBurstLength = ETHERCAT_DC_ARMW_BURSTCYCLES;
                /* if the cycle time is below 1000 usec, we have to reduce the number of frames sent within one cycle */
                oDcConfigure.dwBurstBulk = (dwBusCycleTimeUsec < 1000)
                                           ? (ETHERCAT_DC_ARMW_BURSTSPP / 2) : ETHERCAT_DC_ARMW_BURSTSPP;
            }
            /* Enable acyclic distribution if cycle time is above 1000 usec to get DCX in sync */
            oDcConfigure.bAcycDistributionDisabled = (eDcmMode_Dcx == eDcmMode) ? EC_FALSE : EC_TRUE;
            SEP_REQUIRE_RET(ecatDcConfigure(&oDcConfigure) == EC_E_NOERROR, false);
            /* configure DCM */
            EC_T_DCM_CONFIG oDcmConfig;
            EC_T_BOOL bLogEnabled = EC_FALSE;

            if (bDcmCtlOff) {
                if (0 != nVerbose) SEP_LOGI("DCM control loop disabled for diagnosis!\n");
            }
            OsMemset(&oDcmConfig, 0, sizeof(EC_T_DCM_CONFIG));

            switch (eDcmMode) {
                case eDcmMode_Off:
                    oDcmConfig.eMode = eDcmMode_Off;
                    break;
                case eDcmMode_BusShift:
                    oDcmConfig.eMode = eDcmMode_BusShift;
                    oDcmConfig.u.BusShift.nCtlSetVal = nDcmCtlSetVal;
                    oDcmConfig.u.BusShift.dwInSyncLimit =
                            (dwBusCycleTimeUsec * 1000) / 5;    /* 20 % limit in nsec for InSync monitoring */
                    oDcmConfig.u.BusShift.bLogEnabled = bLogEnabled;
                    oDcmConfig.u.BusShift.bCtlOff = bDcmCtlOff;
                    break;
                case eDcmMode_MasterShift:
                    oDcmConfig.eMode = eDcmMode_MasterShift;
                    oDcmConfig.u.MasterShift.nCtlSetVal = nDcmCtlSetVal;
                    oDcmConfig.u.MasterShift.dwInSyncLimit =
                            (dwBusCycleTimeUsec * 1000) / 5;    /* 20 % limit in nsec for InSync monitoring */
                    oDcmConfig.u.MasterShift.bLogEnabled = bLogEnabled;
                    oDcmConfig.u.MasterShift.bCtlOff = bDcmCtlOff;
                    break;
                case eDcmMode_MasterRefClock:
                    oDcmConfig.eMode = eDcmMode_MasterRefClock;
                    oDcmConfig.u.MasterRefClock.nCtlSetVal = nDcmCtlSetVal;
                    oDcmConfig.u.MasterRefClock.bLogEnabled = bLogEnabled;
                    break;
                case eDcmMode_Dcx:
                    oDcmConfig.eMode = eDcmMode_Dcx;
                    /* Mastershift */
                    oDcmConfig.u.Dcx.MasterShift.nCtlSetVal = nDcmCtlSetVal;
                    oDcmConfig.u.Dcx.MasterShift.dwInSyncLimit =
                            (dwBusCycleTimeUsec * 1000) / 5;    /* 20 % limit in nsec for InSync monitoring */
                    oDcmConfig.u.Dcx.MasterShift.bLogEnabled = bLogEnabled;
                    oDcmConfig.u.Dcx.MasterShift.bCtlOff = bDcmCtlOff;
                    /* Dcx Busshift */
                    oDcmConfig.u.Dcx.nCtlSetVal = nDcmCtlSetVal;
                    oDcmConfig.u.Dcx.dwInSyncLimit =
                            (dwBusCycleTimeUsec * 1000) / 5;    /* 20 % limit in nsec for InSync monitoring */
                    oDcmConfig.u.Dcx.bLogEnabled = bLogEnabled;
                    oDcmConfig.u.Dcx.bCtlOff = bDcmCtlOff;
                    oDcmConfig.u.Dcx.dwExtClockTimeout = 1000;
                    oDcmConfig.u.Dcx.wExtClockFixedAddr = 0; /* 0 only when clock adjustment in external mode configured by EcEngineer */
                    break;
                default:
                    dwRetVal = EC_E_NOTSUPPORTED;
                    SEP_LOGE("DCM mode is not supported!\n");
                    goto Exit;

            }
            dwRes = ecatDcmConfigure(&oDcmConfig, 0);
            switch (dwRes) {
                case EC_E_NOERROR:
                    break;
                case EC_E_FEATURE_DISABLED:
                    SEP_LOGE("Cannot configure DCM mode!\n");
                    SEP_LOGE(
                            "Start with -dcmmode off to run the DC demo without DCM, or prepare the ENI file to support the requested DCM mode\n");
                    SEP_LOGE(
                            "In ET9000 for example, select under ""Advanced settings\\Distributed clocks"" ""DC in use"" and ""Slave Mode""\n");
                    SEP_LOGE("to support BusShift and MasterRefClock modes.\n");
                    SEP_LOGE("Please refer to the class A manual for further information\n");
                    dwRetVal = dwRes;
                    goto Exit;
                default:
                    SEP_LOGE("Cannot configure DCM mode! %s (Result = 0x%x)\n", ecatGetText(dwRes), dwRes);
                    dwRetVal = dwRes;
                    goto Exit;
            }
        }

        return true;
        Exit:
        return false;

    }


    bool EcMasterSvc::registerClient() {
        /* Register client */
        EC_T_REGISTERRESULTS oRegisterResults;

        OsMemset(&oRegisterResults, 0, sizeof(EC_T_REGISTERRESULTS));
        SEP_LOGT("Trying to register client\n");
        SEP_REQUIRE_RET(ecatRegisterClient(ecatNotifyCallback, this, &oRegisterResults) == EC_E_NOERROR, false);
        client_id = oRegisterResults.dwClntId;
        noti_->SetClientID(client_id);
        SEP_LOGT("Register client (id = %d) done\n", client_id);
        return true;
    }

    bool EcMasterSvc::getSlaveInfos() {
        EC_T_DWORD dwRes = EC_E_ERROR;
        EC_T_WORD wAutoIncAddress = 0;
        ECatSlaveInfo slaveinfo;

        int inpdo_size_bit = 0;
        int outpdo_size_bit = 0;
        int slave_id = 0;  //CISmulti
        unsigned int servo_type = 0x00;

        /* get information about all bus slaves */
        for (wAutoIncAddress = 0; wAutoIncAddress != 1; wAutoIncAddress--) {
            EC_T_BUS_SLAVE_INFO oBusSlaveInfo;
            EC_T_CFG_SLAVE_INFO oCfgSlaveInfo;

            /* get bus slave information */
            dwRes = emGetBusSlaveInfo(INSTANCE_MASTER_DEFAULT, EC_FALSE, wAutoIncAddress, &oBusSlaveInfo);
            if (EC_E_NOERROR != dwRes) {
                break;
            }

            //Slaveinfo.slave_id = oBusSlaveInfo.dwSlaveId;                            //CISmulti
            slaveinfo.bus_index = (unsigned short) (0 - oBusSlaveInfo.wAutoIncAddress);   //CISmulti
            slaveinfo.module_num = 0;                                                  //CISmulti
            slaveinfo.station_address = oBusSlaveInfo.wStationAddress;
            slaveinfo.alias_address = oBusSlaveInfo.wAliasAddress;
            slaveinfo.vendor_id = oBusSlaveInfo.dwVendorId;
            slaveinfo.product_code = oBusSlaveInfo.dwProductCode;
            slaveinfo.context = -1L;
            slaveinfo.user_pid = 0;
            slaveinfo.following_axis = -1;
            slaveinfo.revision = oBusSlaveInfo.dwRevisionNumber;

            if (slaveinfo.vendor_id == ecvendor_yaskawa && slaveinfo.product_code == 0x02200301 /* sigma 7*/) {
                slaveinfo.type = SlaveType::CIA402_COMPLIANT_SERVO;
                SEP_LOGT("Yaskawa Servo Detected\n");
            } else if (slaveinfo.vendor_id == ecvendor_rsa/* RSA */ &&
                       (slaveinfo.product_code == 0x02020013 ||
                        slaveinfo.product_code == 0x02020012) /* Dual axis servo pack*/) {
                slaveinfo.type = SlaveType::CIA402_COMPLIANT_SERVO_D;
                SEP_LOGT("RSA Dual-Servo Detected\n");
            } else if (slaveinfo.vendor_id == ecvendor_rsa/* RSA */ &&
                       (slaveinfo.product_code == 0x02020002) /* Triple axis servo pack*/) {
                slaveinfo.type = SlaveType::CIA402_COMPLIANT_SERVO_T;
                SEP_LOGT("RSA Triple-Servo Detected\n");
            } else if (slaveinfo.vendor_id == ecvendor_rsa/* RSA */ ||    // yjchoe
                       slaveinfo.product_code == 0x02010013 /* servo pack*/) {
                slaveinfo.type = SlaveType::CIA402_COMPLIANT_SERVO2;
                SEP_LOGT("RSA Servo Detected\n");
            } else if (slaveinfo.vendor_id == ecvendor_fine) {
                if (slaveinfo.product_code == ecprodcode_fine_serial) {
                    slaveinfo.type = SlaveType::FINE_SERIAL;
                    SEP_LOGT("FINE-TEC Serial Board Detected\n");
                } else if (slaveinfo.product_code == ecprodcode_fine_stocker_io) {
                    slaveinfo.type = SlaveType::FINE_STK_IO;
                    SEP_LOGT("STK I/O Board Detected\n");
                } else if (slaveinfo.product_code == ecprodcode_fine_oht_front) {
                    slaveinfo.type = SlaveType::FINE_OHT_FRONT_IO;
                    SEP_LOGT("OHT Front I/O Board Detected\n");
                } else if (slaveinfo.product_code == ecprodcode_fine_oht_rear) {
                    slaveinfo.type = SlaveType::FINE_OHT_REAR_IO;
                    SEP_LOGT("OHT Rear I/O Board Detected\n");
                } else {
                    slaveinfo.type = SlaveType::UNKNOWN;
                }
            } else {
                slaveinfo.type = SlaveType::UNKNOWN;
            }

            /* get cfg slave information (matching bus slave) */
            dwRes = emGetCfgSlaveInfo(INSTANCE_MASTER_DEFAULT, EC_TRUE, oBusSlaveInfo.wStationAddress, &oCfgSlaveInfo);
            if (EC_E_NOERROR == dwRes) {
                slaveinfo.in_pd_offset = oCfgSlaveInfo.dwPdOffsIn / 8;
                slaveinfo.out_pd_offset = oCfgSlaveInfo.dwPdOffsOut / 8;
                slaveinfo.in_pd_length = oCfgSlaveInfo.dwPdSizeIn / 8;
                slaveinfo.out_pd_length = oCfgSlaveInfo.dwPdSizeOut / 8;
                inpdo_size_bit += oCfgSlaveInfo.dwPdSizeIn;
                outpdo_size_bit += oCfgSlaveInfo.dwPdSizeOut;
            } else {
                slaveinfo.in_pd_offset = 0;
                slaveinfo.out_pd_offset = 0;
                slaveinfo.in_pd_length = 0;
                slaveinfo.out_pd_length = 0;
            }
            int AxisCount = 0;
            // 나누기 8을 하는 이유는? bit -> byte 단위 변환
            if (slaveinfo.type == SlaveType::CIA402_COMPLIANT_SERVO_D ||
                slaveinfo.type == SlaveType::CIA402_COMPLIANT_SERVO_T) {
                if (slaveinfo.type == SlaveType::CIA402_COMPLIANT_SERVO_D) {
                    AxisCount = 2;
                } else if (slaveinfo.type == SlaveType::CIA402_COMPLIANT_SERVO_T) {
                    AxisCount = 3;
                }
                // 1축에 대한 in/out pdo length를 구함
                slaveinfo.in_pd_length = oCfgSlaveInfo.dwPdSizeIn / 8 / AxisCount;
                slaveinfo.out_pd_length = oCfgSlaveInfo.dwPdSizeOut / 8 / AxisCount;
                // 각 축별 offset을 구해서 slave 축 생성
                for (int i = 0; i < AxisCount; i++) {
                    slaveinfo.in_pd_offset = (oCfgSlaveInfo.dwPdOffsIn / 8) + (slaveinfo.in_pd_length * i);
                    slaveinfo.out_pd_offset = (oCfgSlaveInfo.dwPdOffsOut / 8) + (slaveinfo.out_pd_length * i);
                    slaveinfo.module_num = i;
                    slaveinfo.slave_id = slave_id++;
                    slaves_.push_back(slaveinfo);
                    servo_type |= (slaveinfo.type == 0 ? YASKAWA : RSA) << (slaveinfo.slave_id * 8);
                }
            } else {
                slaveinfo.slave_id = slave_id++;
                slaves_.push_back(slaveinfo);

                // add servo type, yjchoe
                if (!(slaveinfo.type == SlaveType::FINE_OHT_FRONT_IO || slaveinfo.type == SlaveType::FINE_OHT_REAR_IO))
                    servo_type |= (slaveinfo.type == 0 ? YASKAWA : RSA) << (slaveinfo.slave_id * 8);
                //printf("[slave_id : %d] servo_type = %u\n", slaveinfo.slave_id, servo_type);
            }
        }
        memcpy(shm_->address + shm_->size() - 4, &servo_type, sizeof(servo_type));   // yjchoe

        SEP_REQUIRE_RET(inpdo_size_bit / 8 <= EM_SHM_SECTION_SIZE, EC_FALSE);
        SEP_REQUIRE_RET(outpdo_size_bit / 8 <= inpdo_size_bit, EC_FALSE);
        return EC_TRUE;
    }

    bool EcMasterSvc::scanNetwork() {
        // print slaves
        int dwRes = ecatScanBus(ECAT_SCANBUS_TIMEOUT_MS);
        switch (dwRes) {
            case EC_E_NOERROR:
            case EC_E_BUSCONFIG_MISMATCH:
            case EC_E_LINE_CROSSED:
                PrintSlaveInfos(INSTANCE_MASTER_DEFAULT, &logger_);
                getSlaveInfos();
                break;
            default:
                SEP_LOGE("Cannot scan bus: %s (0x%lx)\n", ecatGetText(dwRes), dwRes);
                return false;
        }

        // print mac address
        ETHERNET_ADDRESS oSrcMacAddress;
        SEP_REQUIRE_RET(ecatGetSrcMacAddress(&oSrcMacAddress) == EC_E_NOERROR, false);
        SEP_LOGI("EtherCAT network adapter MAC: %02X-%02X-%02X-%02X-%02X-%02X\n",
                 oSrcMacAddress.b[0], oSrcMacAddress.b[1], oSrcMacAddress.b[2], oSrcMacAddress.b[3],
                 oSrcMacAddress.b[4], oSrcMacAddress.b[5]);
        slave_present_ = true;
        return true;
    }

    bool EcMasterSvc::changeECatState(EC_T_STATE new_state) {
        SEP_LOGI("Trying to Change master state to %s\n", ecatStateToStr(new_state));
        if(ecatSetMasterState(ECAT_STATE_CHANGE_TIMEOUT_MS, new_state) != EC_E_NOERROR) {
            SEP_LOGE("Fail to change ecat state");
            return false;
        }
        SEP_LOGI("Changed master state to %s\n", ecatStateToStr(new_state));
        noti_->ProcessNotificationJobs();
        ecat_state_ = new_state;
        return true;
    }

    bool EcMasterSvc::prepareSvc() // INIT -> PREOP
    {
        in_pdo = ecatGetProcessImageInputPtr();
        out_pdo = ecatGetProcessImageOutputPtr();

        SEP_LOGI("in_pdo = %p, out_pdo = %p\n", in_pdo, out_pdo);
        SEP_LOGI("Prepare EC Master Svc before change to PREOP\n");
        // eni parsing & construct axis information
        return true;
    }

    bool EcMasterSvc::setupSvc() // PREOP-> SAFEOP
    {
        SEP_LOGI("Setup EC Master Svc before change to SafeOP/OP\n");
        // sync manager 및 fmmu 할당

        EC_T_CFG_SLAVE_INFO oCfgSlaveInfo;

        for (auto &slave : slaves_) {

            // assign message queue
            slave.msgq = new ECatSlaveMsgQ(EM_MSG_Q_KEY_SLAVE(slave.slave_id).c_str(), true);

//            // update pd offset & size
//            if (EC_E_NOERROR ==
//                emGetCfgSlaveInfo(INSTANCE_MASTER_DEFAULT, EC_TRUE, slave.station_address, &oCfgSlaveInfo)) {
//                slave.in_pd_offset = oCfgSlaveInfo.dwPdOffsIn / 8;
//                slave.out_pd_offset = oCfgSlaveInfo.dwPdOffsOut / 8;
//                slave.in_pd_length = oCfgSlaveInfo.dwPdSizeIn / 8;
//                slave.out_pd_length = oCfgSlaveInfo.dwPdSizeOut / 8;
//            } else {
//                SEP_LOGT("Error while do emGetCfgSlaveInfo\n");
//            }  //CISmulti

            // context 설정
            if (slave.type == SlaveType::CIA402_COMPLIANT_SERVO
            || slave.type == SlaveType::CIA402_COMPLIANT_SERVO2
            || slave.type == SlaveType::CIA402_COMPLIANT_SERVO_D
            || slave.type == SlaveType::CIA402_COMPLIANT_SERVO_T) {  // yjchoe
                //servos_.emplace_back(client_id, slave.slave_id, in_pdo + slave.in_pd_offset,
	            servos_.emplace_back(client_id, slave.bus_index, in_pdo + slave.in_pd_offset,
                                     out_pdo + slave.out_pd_offset, opt_->dwBusCycleTimeUsec);
                slave.context = servos_.size() - 1; // servo_ index
            } else if (slave.type == SlaveType::FINE_SERIAL) {
                
                serials_.emplace_back(client_id, slave.slave_id, in_pdo + slave.in_pd_offset,
                                      out_pdo + slave.out_pd_offset, getSerialPortCount(slave), slave.type);
                slave.context = serials_.size() - 1; // serial io index
            
                SEP_LOGT("serial_called");
                 
                dumpIoSlaveInfo(slave);
            } else if (slave.type == SlaveType::FINE_STK_IO) {
                stocker_io_ = &slave;
            } else if (slave.type == SlaveType::FINE_OHT_FRONT_IO) {
                //dumpIoSlaveInfo(slave);
                oht_front_io_ = &slave;
                SEP_LOGT("Front IO : in_pdo = %p, out_pdo = %p\n", in_pdo + slave.in_pd_offset,
                         out_pdo + slave.out_pd_offset);
                //serials_.emplace_back(client_id, slave.slave_id,
                serials_.emplace_back(client_id, slave.bus_index,   // 210726 yjchoe, for dual-servo
                                      in_pdo + slave.in_pd_offset + slave.in_pd_length - sizeof(ECatSerialData),
                                      out_pdo + slave.out_pd_offset + slave.out_pd_length - sizeof(ECatSerialData),
                                      ECatSerialData::OHT_FRONT_SERIAL_COUNT, slave.type);
                slave.context = serials_.size() - 1; // serial io index
            } else if (slave.type == SlaveType::FINE_OHT_REAR_IO) {
                //dumpIoSlaveInfo(slave);
                oht_rear_io_ = &slave;
                SEP_LOGT("Rear IO : in_pdo = %p, out_pdo = %p\n", in_pdo + slave.in_pd_offset,
                         out_pdo + slave.out_pd_offset);
                //serials_.emplace_back(client_id, slave.slave_id,
                serials_.emplace_back(client_id, slave.bus_index,   // 210726 yjchoe, for dual-servo
                                      in_pdo + slave.in_pd_offset + slave.in_pd_length - sizeof(ECatSerialData),
                                      out_pdo + slave.out_pd_offset + slave.out_pd_length - sizeof(ECatSerialData),
                                      ECatSerialData::OHT_REAR_SERIAL_COUNT, slave.type);
                slave.context = serials_.size() - 1; // serial io index
            }

            // dump slave info
            slave.print();

#if 0 // dump all COE objects
            int nVerbose = 2;
                EC_T_BOOL  bStopReading  = EC_FALSE;

                SEP_LOGT("\n\n[[[ Dumping COE Object Information (slave %d)\n", slave.slave_id);
                CoeReadObjectDictionary(INSTANCE_MASTER_DEFAULT, &logger_, nVerbose, &bStopReading, client_id, slave.slave_id, EC_TRUE, COE_SDO_TIMEOUT_MS);
                SEP_LOGT("]]] Dumping COE Object Information (slave %d)\n\n\n", slave.slave_id);
#endif
        }

        // map slave information to axis

        SEP_LOGT("seial_zise called:[%d]", serials_.size());
        // start serial over ehtercat service
        if (serials_.size() > 0) {
            startSerialOverEtherCATService();
                
        }
        return true;
    }
}
