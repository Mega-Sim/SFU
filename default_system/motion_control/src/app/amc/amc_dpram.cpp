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
// Created by sem1 on 17. 7. 25.
//
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/ServerSocket.h>
#include "amc_dpram.h"
#include "amc_emulator.h"
using namespace sephi;

void printBinary(void *data, int length, const char* title = nullptr, int offs = 0)
{
    if (title != nullptr) {
        printf(">>> %s <<< \n", title);
    }
    for (int i = 0; i < length; i += 16) {
        auto end = min(length, i + 16);
        printf("[%4d] ", i+offs);
        for (int j = i; j < end; j += 4) {
            auto end2 = min(length, j + 4);
            for (int k = j; k < end2; k += 1) {
                printf("%02x ", ((uint8_t*)data)[k]);
            }
            printf(" ");
        }
        printf("\n");
    }
}

namespace amc
{
    using Poco::Net::Socket;
    using Poco::Net::SocketAddress;
    using Poco::Net::ServerSocket;
    using Poco::Net::StreamSocket;
    using Poco::Timespan;

    struct DPRamConnection {
    public:
        DPRamConnection(DPRamStruct *_ram):ram(_ram),serversock{DPRAM_PORT}, connected(false)
        {
            Timespan timeout(2,0); // 2sec timeout
            serversock.setReceiveTimeout(timeout);
        }
        void accept()
        {
            Timespan timeout(2,0); // 2sec timeout
            try {
                streamsock = serversock.acceptConnection();
                streamsock.setReceiveTimeout(timeout);
                streamsock.setSendTimeout(timeout);
                connected = true;
                SEP_LOGT("Connection from %s\n", serversock.address().toString().c_str());
                // send dpram for sync
                //doTx();
            } catch (...) {
                SEP_LOGT("Fail to accept!\n");
            }
        }
        void doRx()
        {
            try {
                int remain = DPRAM_SIZE;
                int offset = 0;
                while(remain > 0) {
                    auto received = streamsock.receiveBytes(ram->mem + offset, remain);
                    if (received == 0) {
                        SEP_LOGT("Connection closed.\n");
                        connected = false;
                        return;
                    }
                    offset += received;
                    remain -= received;
                }
                //SEP_LOGT("RX: Axis[%d] Service request :%d\n",ram->mem[1020] , ram->mem[1022]);
                //if(ram->map._Amc._Cmd._Amc_Cmd.CmdAxis[0].CmdType == 7)
                //    printBinary(ram->mem, 1024, "Received");
            } catch (...) {
                connected = false;
            }
        }

	void doTx()
        {
#ifdef DEBUG_FRAME_COUNT
            static uint8_t frame_counter{0};
            ram->mem[1008] = frame_counter++;
#endif //DEBUG_FRAME_COUNT
            try {
                if (streamsock.sendBytes(ram->mem, DPRAM_SIZE) == 0) {
                    SEP_LOGT("Connection closed.\n");
                    connected = false;
                }
                //SEP_LOGT("DSP No: %d\n", ram->mem[1022]);
            } catch (...) {
                connected = false;
            }

        }

    public:
        DPRamStruct *ram;
        StreamSocket streamsock;
        ServerSocket serversock;
        bool connected;
    };

    DPRamTask::DPRamTask(DPRamStruct *ram, IAmcDispacher *dispatcher)
    {
        task_param_.ram = ram;
        dispatcher_ = dispatcher;
        need_tx_ = false;
        stop_flag = false;
    }

    void DPRamTask::update()
    {
        need_tx_ = true;
    }

    void DPRamTask::svc()
    {
        DPRamConnection conn{task_param_.ram}; // connection (single connection 이므로...)
        Timespan timeout(0,500000); // sec, usec
        Socket::SocketList readList, writeList, exceptList;
        while(!stop_) {
            if (conn.connected) {
                readList.push_back(conn.streamsock);
            } else { //unsigned int gain[5];
                readList.push_back(conn.serversock);
            }

            if (Socket::select(readList, writeList, exceptList, timeout) > 0) {
                if (!conn.connected) {
                    readList.clear();  /// CIS
                    conn.accept();
                    if (conn.connected) {
                        memset(&task_param_.ram->mem, 0, 1024);
                        dispatcher_->dispatch(true);
                        stop_flag = false;
                    }
                } else {

                    conn.doRx();
                    if (conn.connected) {
                        dispatcher_->dispatch(false);
                    }
                }
            }
            if (need_tx_ && conn.connected) {
                conn.doTx();
                need_tx_ = false;
            }
            else if(!conn.connected && stop_flag == false){  //통신 끊김 발생 시 비상 정지 기능
				dispatcher_->stop_all();
                stop_flag = true;
			}
        }
    }
}
