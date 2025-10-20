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
// Created by sem1 on 17. 5. 8.
//

#include "ecmaster_service.h"
#include "rpc/ecmaster.grpc.pb.h"
#include "emul/ecat_emulator.h"
#include "emul/stocker/stocker_simulator.h"
#include "emul/oht/oht_simulator.h"

namespace sephi
{
    /**\addtogroup grp_ecmaster
     * \{
     */
    extern EcMasterSvc* the_EcMasterSvc_ins;

    // rpc handler
    using ServerContext = grpc::ServerContext;
    using Status = grpc::Status;

    class EcMasterRpcImpl final : public EcMasterRpc::Service
    {

        // get simulation target system name
        Status getSystemName(ServerContext* context, const Empty* request, ModelName* response) override
        {
            response->set_name(the_ec_emulator.get()->targetSystem()->systemName());
            return Status::OK;
        }

        Status getOHTStatus(ServerContext* context, const Empty* request, OHTStatus* response) override
        {
            OHTSystem *target = dynamic_cast<OHTSystem *>(the_ec_emulator.get()->targetSystem());
            SEP_REQUIRE_RET(target != nullptr, Status::CANCELLED);
            response->set_ymldata(target->getStatusAsYaml());
            return Status::OK;
        }

        Status requestSlave(ServerContext *context, const Request *request, Response *reply) override
        {
            ECatSdoMsg msg, res;
            msg.slave_id = request->slave_id();
            msg.req.pid = request->pid();
            SEP_LOGT("EcMaster::requestSlave\n");
            the_EcMasterSvc_ins->handleRequestMessage(msg, res);
            reply->set_slave_id(res.slave_id);
            reply->set_in_pd_offset(res.res.in_pd_offset);
            reply->set_out_pd_offset(res.res.out_pd_offset);
            reply->set_bus_cycle_us(res.res.bus_cycle_us);
            return Status::OK;
        }

        Status releaseSlave(ServerContext *context, const Request *request, Response *reply) override
        {
            ECatSdoMsg msg, res;
            msg.slave_id = request->slave_id();
            msg.req.pid = request->pid();
            SEP_LOGT("EcMaster::releaseSlave\n");
            the_EcMasterSvc_ins->handleReleaseMessage(msg, res);
            reply->set_slave_id(res.slave_id);
            return Status::OK;
        }

        Status ohtRearOutWrite(ServerContext *context, const OhtDataOut *request, Result *reply) override
        {
            uint64_t aout = request->aout();
            uint64_t dout = request->dout();
            OhtRearIoPdoOut out;
            memcpy(out.aout, &aout, sizeof(out.aout));
            memcpy(out.dout, &dout, sizeof(out.dout));
            bool res = the_EcMasterSvc_ins->writeOhtRearOutData(out);
            reply->set_result(res ? 0 : -1);
            return Status::OK;
        }

        Status ohtFrontInRead(ServerContext *context, const Result *opt, OhtDataIn *reply) override
        {
            OhtFrontIoPdoIn in;
            uint64_t ain(0), din(0);
            if (the_EcMasterSvc_ins->readOhtFrontInData(in)) {
                memcpy(&ain, in.ain, sizeof(in.ain));
                memcpy(&din, in.din, sizeof(in.din));
            }
            reply->set_ain(ain);
            reply->set_din(din);

            return Status::OK;
        }

//        Status ohtOnInRead(ServerContext *context, const Result *opt, OhtDataIn *reply) override
//        {
//            OhtOnIoPdoIn in;
//            uint64_t ain(0), din(0);
//            if (the_EcMasterSvc_ins->readOhtOnInData(in)) {
//                memcpy(&ain, in.ain, sizeof(in.ain));
//                memcpy(&din, in.din, sizeof(in.din));
//            }
//            reply->set_ain(ain);
//            reply->set_din(din);
//
//            return Status::OK;
//        }

        Status ohtRearInRead(ServerContext *context, const Result *opt, OhtDataIn *reply) override
        {
            OhtRearIoPdoIn in;
            uint64_t ain(0), din(0);
            if (the_EcMasterSvc_ins->readOhtRearInData(in)) {
                memcpy(&ain, in.ain, sizeof(in.ain));
                memcpy(&din, in.din, sizeof(in.din));
            }
            reply->set_ain(ain);
            reply->set_din(din);

            return Status::OK;
        }

        Status ohtFrontOutRead(ServerContext *context, const Result *opt, OhtDataOut *reply) override
        {
            OhtFrontIoPdoOut out;

            uint64_t aout(0), dout(0);
            if (the_EcMasterSvc_ins->readOhtFrontOutData(out)) {
                memcpy(&aout, out.aout, sizeof(out.aout));
                memcpy(&dout, out.dout, sizeof(out.dout));
            }
            reply->set_aout(aout);
            reply->set_dout(dout);

            return Status::OK;
        }

        Status ohtRearOutRead(ServerContext *context, const Result *opt, OhtDataOut *reply) override
        {
            OhtRearIoPdoOut out;
            uint64_t aout(0), dout(0);
            if (the_EcMasterSvc_ins->readOhtRearOutData(out)) {
                memcpy(&aout, out.aout, sizeof(out.aout));
                memcpy(&dout, out.dout, sizeof(out.dout));
            }
            reply->set_aout(aout);
            reply->set_dout(dout);

            return Status::OK;
        }

        Status changeECatStateToOP(ServerContext* context, const AdminCommand* request, Result* response) override
        {
            CiA402Servo* servo = the_EcMasterSvc_ins->getServo(request->slave_id());
            SEP_LOGT("EcMaster::set slave ECatStateToOP, slave = %d\n", request->slave_id());
            int32_t result = servo->setSlaveEcatState(eEcatState_OP);
            response->set_result(result);
            return Status::OK;
        }


        Status makeSwitchOnDisabled(ServerContext* context, const AdminCommand* request, Result* response) override
        {
            CiA402Servo* servo = the_EcMasterSvc_ins->getServo(request->slave_id());
            int32_t result = -1;
            if (servo != nullptr) {
                result = servo->makeSwitchOnDisabledState();
            }
            response->set_result(result);
            return Status::OK;
        }

        Status enableUserParameter(ServerContext* context, const AdminCommand* request, Result* response) override
        {
            CiA402Servo* servo = the_EcMasterSvc_ins->getServo(request->slave_id());
            int32_t result = -1;
            if (servo != nullptr) {
                result = servo->enableUserParameter();
            }
            response->set_result(result);
            return Status::OK;
        }

        Status resetAbsEncoder(ServerContext* context, const AdminCommand* request, Result* response) override
        {
            CiA402Servo* servo = the_EcMasterSvc_ins->getServo(request->slave_id());
            int32_t result = -1;
            if (servo != nullptr) {
                result = servo->resetAbsEncoder();
            }
            response->set_result(result);
            return Status::OK;
        }


        Status storeParameter(ServerContext* context, const AdminCommand* request, Result* response) override
        {
            CiA402Servo* servo = the_EcMasterSvc_ins->getServo(request->slave_id());
            int32_t result = -1;
            if (servo != nullptr) {
                result = servo->saveParams(static_cast<COE_STORE_OPTIONS>(request->option()));
            }
            response->set_result(result);
            return Status::OK;
        }

        Status restoreDefaultParameter(ServerContext* context, const AdminCommand* request, Result* response) override
        {
            CiA402Servo* servo = the_EcMasterSvc_ins->getServo(request->slave_id());
            int32_t result = -1;
            if (servo != nullptr) {
                result = servo->restoreDefaultParams(static_cast<COE_STORE_OPTIONS>(request->option()));
            }
            response->set_result(result);
            return Status::OK;
        }

        Status readCoeValue(ServerContext* context, const CoeInfo* request, CoeData* response) override
        {
            CiA402Servo* servo = the_EcMasterSvc_ins->getServo(request->slave_id());
            CoeValue val;
            if (servo != nullptr) {
                val = servo->readCoeValue(request->index(), request->subindex());
            } else {
                val.type = CoeValue::UNKNOWN;
            }
            response->set_slave_id(request->slave_id());
            response->set_index(request->index());
            response->set_subindex(request->subindex());
            response->set_type(val.type);
            response->set_value(val.u32);
            return Status::OK;
        }

        Status writeCoeValue(ServerContext* context, const CoeData* request, Result* response) override
        {
            CiA402Servo* servo = the_EcMasterSvc_ins->getServo(request->slave_id());
            bool result = false;
            if (servo != nullptr) {
                CoeValue val;
                val.type = static_cast<CoeValue::Type>(request->type());
                val.u32 = request->value();
                result = servo->writeCoeValue(request->index(), request->subindex(), val);
            }
            response->set_result(result? 0 : -1);
            return Status::OK;
        }

    };

    static GrpcServer<EcMasterRpcImpl> ecmaster_rpc_svr;


    ECatSlaveInfo* EcMasterSvc::getSlave(int slave_id)
    {
        SEP_REQUIRE_RET(slave_id >= 0 && slave_id < slaves_.size(), nullptr);
        return &slaves_[slave_id];
    }

    CiA402Servo* EcMasterSvc::getServo(int slave_id)
    {
        SEP_REQUIRE_RET(slave_id >= 0 && slave_id < slaves_.size(), nullptr);
        return &servos_[slaves_[slave_id].context];
    }

    bool EcMasterSvc::startMessageHandler()
    {
        sdo_msg_handle_stop_ = false;
        sdo_msg_handle_task_ = std::make_unique<std::thread>([this](){
            addIntoRealtimeSchedule(MAIN_THREAD_PRIO);

            SEP_LOGT("Start ECAT Message Task\n");
            while(!sdo_msg_handle_stop_) {
                handleMessages();
            }
            SEP_LOGT("Stop ECAT Message Task\n");
        });

        ecmaster_rpc_svr.RunAsThread(EM_RPC_PORT);
        SEP_LOGT("ecmaster_rpc_svr started\n");
        return sdo_msg_handle_task_ != nullptr;
    }

    void EcMasterSvc::stopMessageHandler()
    {
        ecmaster_rpc_svr.Shutdown();
        if (sdo_msg_handle_task_ != nullptr) {
            sdo_msg_handle_stop_ = true;
            sdo_msg_handle_task_->join();
        }
    }


    /// \}
}
