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
// Created by master on 17. 4. 13.
//
#include "rpc/ecmaster.grpc.pb.h"
#include "ecat_data_struct.h"

#ifndef __ECMASTER_RPC_CLIENT_H__
#define __ECMASTER_RPC_CLIENT_H__
namespace  sephi
{
    /**\addtogroup grp_ecmaster
      * \{
      */
    using grpc::Channel;
    using grpc::ClientContext;
    using grpc::Status;

    class EcMasterRpcCli : public GrpcClient<EcMasterRpc> {
    public:
        EcMasterRpcCli() : GrpcClient<EcMasterRpc>(EM_RPC_PORT) {}

        bool requestSlave(int slave_id, ECatSdoMsg::RscResponseMsg& res) {
            // Data we are sending to the server.
            Request request;
            request.set_slave_id(slave_id);
            request.set_pid((int32_t)getpid());

            // Container for the data we expect from the server.
            Response reply;

            // Context for the client. It could be used to convey extra information to
            // the server and/or tweak certain RPC behaviors.
            ClientContext context;

            // The actual RPC.
            SEP_LOGT("begin call\n");
            Status status = stub_->requestSlave(&context, request, &reply);
            SEP_LOGT("end call, slave id = %d, status = %d \n", reply.slave_id(), status.ok());
            if (status.ok()) {
                res.in_pd_offset = reply.in_pd_offset();
                res.out_pd_offset = reply.out_pd_offset();
                res.bus_cycle_us = reply.bus_cycle_us();
            } else {
                res.in_pd_offset = 0xFFFFFFFF;
                res.out_pd_offset = 0xFFFFFFFF;
                res.bus_cycle_us =  0xFFFFFFFF;
            }
            return status.ok();
        }

        bool releaseSlave(int slave_id)
        {
            // Data we are sending to the server.
            Request request;
            request.set_slave_id(slave_id);
            request.set_pid((int32_t)getpid());

            // Container for the data we expect from the server.
            Response reply;

            // Context for the client. It could be used to convey extra information to
            // the server and/or tweak certain RPC behaviors.
            ClientContext context;

            // The actual RPC.
            SEP_LOGT("begin call\n");
            Status status = stub_->releaseSlave(&context, request, &reply);
            SEP_LOGT("end call\n");
            return status.ok();
        }

        bool changeECatStateToOP(int slave_id)
        {
            AdminCommand cmd;
            Result res;
            cmd.set_slave_id(slave_id);
            ClientContext context;
            Status status = stub_->changeECatStateToOP(&context, cmd, &res);
            return res.result();
        }


    };
    extern  Poco::SingletonHolder<EcMasterRpcCli> the_em_rpc_client;
    /// \}
}
#endif // __ECMASTER_RPC_CLIENT_H__