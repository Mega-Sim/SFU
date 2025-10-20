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
// Created by master on 17. 3. 22.
//

#ifndef IPCTEST_IPC_CONFIG_H
#define IPCTEST_IPC_CONFIG_H

#include <atomic>
#include <unistd.h>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/named_sharable_mutex.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/StreamSocket.h"
#include <memory>
#include <thread>
#include <string>
#include <sstream>
#include <array>
#include <grpc++/grpc++.h>
#include "log.h"

namespace  sephi {
    using string = std::string;
    using namespace boost::interprocess;
    inline const char* SEP_SHM_KEY(){ return "sepshm";}
    inline const char* SEP_MSGQ_KEY(){ return "sepmsgq";}
    inline const char* SEP_DBG_FLAG_KEY() { return "shmdbg";}

    class SharableLockGuard {
        named_sharable_mutex* sharabel_mutex_;
    public:
        SharableLockGuard(named_sharable_mutex *sharabel_mutex) : sharabel_mutex_(sharabel_mutex) {
            sharabel_mutex_->lock_sharable();
        }
        ~SharableLockGuard() {
            sharabel_mutex_->unlock_sharable();
        }
    };

#ifdef __arm__
    typedef bool atomic_bool; // \todo provide atomic operation for bool type
    typedef int  atomic_int;
#else
    typedef std::atomic_bool atomic_bool;
    typedef std::atomic_int  atomic_int;
#endif

    /// sephi ipc 모듈 상수 정의
    enum class SepIPC : unsigned {
        SHM_SIZE = 4 * 1024,    ///< 공유메모리 default size
        MSGQ_SIZE = 100,        ///< size of message queue, i.e. max message count that queue can contain
        MSG_SIZE = 100,         ///< max size of a message
    };


    /// boost.interprocess를 이용해 구현한 공유 메모리 클래스
    class SepShm {
    public:
        SepShm(const char* shmname, int _size = -1); ///< _size = -1 means client
        ~SepShm();

        size_t size() { return region_->get_size(); }
        char *address;
    private:
        bool is_server_;
        string shmname_;
        std::unique_ptr<shared_memory_object> shm_;
        std::unique_ptr<mapped_region> region_;
    };


    struct DebugState{
        enum CONST {
            OHT_DISTANCE_MM   = 0,
            COUNT
        };

        using Data = std::array<int,COUNT>;
        static bool initServer();
        static bool initClient();
        static void set(int id, int val);
        static int get(int id);
    private:
        static std::mutex lock;
        static std::unique_ptr<SepShm> shm;
        static Data* flags;
    };


    /// 현재 시간으로부터 duration초 후의 시각을 posix_time 형태로 반환한다.
    boost::posix_time::ptime getPtimeAfterSecond(float duration);

    /// boost.interprocess를 이용해 구현한 메시지 큐 클래스
    template<class Msg, int size = 100>
    class SepMsgQ {
    public:
        SepMsgQ(const char *name, bool create = false) : creator_(create)
        {
            SEP_LOGT("Creating message %s\n", name);
            if (create) {
                message_queue::remove(name);
                mq_ = std::make_unique<message_queue>(create_only, name, size, sizeof(Msg));
            } else {
                mq_ = std::make_unique<message_queue>(open_only, name);
            }
            name_ = name;
        }

        ~SepMsgQ() { if (creator_) { message_queue::remove(name_.c_str()); }}

        bool put(const Msg &item, float waits = -1.0) {
            if (waits < 0) {
                mq_->send(&item, sizeof(Msg), 0);
                return true;
            } else {
                return mq_->timed_send(&item, sizeof(Msg), 0, getPtimeAfterSecond(waits));
            }
        }

        bool get(Msg &item, float waits = -1.0) {
            unsigned int priority;
            message_queue::size_type received_size;
            if (waits < 0) {
                mq_->receive(&item, sizeof(Msg), received_size, priority);
                return true;
            } else {
                return mq_->timed_receive(&item, sizeof(Msg), received_size, priority, getPtimeAfterSecond(waits));
            }
        }

        bool getUntil(Msg &item, const boost::posix_time::ptime &abs_time) {
            unsigned int priority;
            message_queue::size_type received_size;
            return mq_->timed_receive(&item, sizeof(Msg), received_size, priority, abs_time);
        }

        int getMsgCount() { return mq_->get_num_msg(); }

    private:
        std::unique_ptr<message_queue> mq_;
        string name_;
        bool creator_;
    };

    /// thread간의 event방식의 동기화 클래스
    class ThreadCondition
    {
    public:
        ThreadCondition(): exit_(false){}
        bool wait(float wait_time_in_sec = -1.0 ) { return wait(int64_t(wait_time_in_sec*1e9)); }
        bool wait(int64_t wait_ns) {
            std::unique_lock<std::mutex> lock(cv_m);
            if (wait_ns < 0) {
                cv.wait(lock);
                return true;
            } else {
                return std::cv_status::timeout != cv.wait_for(lock, std::chrono::nanoseconds(wait_ns));
            }
        }
        void wakeup() {
            cv.notify_all();
        }

        bool timeToExit() { return exit_; }
        bool exit() {
            exit_ = true;
            cv.notify_all();
        }
    private:
        std::condition_variable cv;
        std::mutex cv_m;
        atomic_bool exit_;
    };


    /// 프로세스간 event 방식의 동기화 클래스
    class IpcCondition {
    public:
        IpcCondition(const char *name, bool create = false);

        ~IpcCondition();

        bool wait(float wait_time_in_sec = -1.0); // returns false in case of time-out
        void wakeUp();

    private:
        std::unique_ptr<named_mutex> mtx_;
        std::unique_ptr<named_condition> cond_;
        bool is_creator_;
        string name_;
    };


    /// gRPC의 server-side 구현 편의를 위한 템플릿 클래스
    /**
     * @tparam ServiceImpl RPC 요청에대한 대응을 구현한 클래스
     *
     * \see EcMasterRpcImpl
     */
    template<class ServiceImpl>
    class GrpcServer {
        using ServerBuilder = grpc::ServerBuilder;
        using Server = grpc::Server;
    public:
        GrpcServer() : server_(nullptr), thr_(nullptr) { }

        void RunServer(uint16_t port_num) {
            std::stringstream ss;
            ss << "0.0.0.0:" << port_num; // localonly = 127.0.0.1
            service_port_ = ss.str();
            std::string server_address(service_port_.c_str());
            ServiceImpl service;

            ServerBuilder builder;
            // Listen on the given address without any authentication mechanism.
            builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
            // Register "service" as the instance through which we'll communicate with
            // clients. In this case it corresponds to an *synchronous* service.
            builder.RegisterService(&service);
            // Finally assemble the server.
            server_ = builder.BuildAndStart();
            SEP_LOGT( "GRPC Server listening on %s\n", service_port_.c_str());

            // Wait for the server to shutdown. Note that some other thread must be
            // responsible for shutting down the server for this call to ever return.
            server_->Wait();
        }

        // 객체 생성 후 1회만 실행할 것!
        void RunAsThread(uint16_t port_num) {
            thr_ = std::make_unique<std::thread>( [this, port_num](){ this->RunServer(port_num);} );
        }

        void Shutdown() {
            if (server_) server_->Shutdown();
            thr_->join();
        }

    private:
        string service_port_;
        std::unique_ptr<Server> server_;
        std::unique_ptr<std::thread> thr_;
    };


    /// gRPC의 client-side 구현 편의를 위한 템플릿 클래스
    /**
     * @tparam Pkg gRPC client가 상속 받아야 할 인터페이스 패키지
     *
     * \see EcMasterRpcCli
     */
    template<class Pkg>
    class GrpcClient {
        std::shared_ptr<grpc::Channel> createChannel(uint16_t port_num, const char* host)
        {
            std::stringstream ss;
            ss << host << ":" << port_num;
            return grpc::CreateChannel( ss.str(), grpc::InsecureChannelCredentials());
        }
    protected:
        using Channel = grpc::Channel;
        using Status = grpc::Status;
        using ClientContext = grpc::ClientContext;
    public:
        GrpcClient(uint16_t port_num, const char* host = "localhost")
                : stub_(Pkg::NewStub(createChannel(port_num, host))) {}

        GrpcClient(std::shared_ptr<Channel> channel)
                : stub_(Pkg::NewStub(channel)) {}

    protected:
        std::unique_ptr<typename Pkg::Stub> stub_;
    };


    class StreamConnector
    {
    public:
        StreamConnector() : connected_(false) { }
        StreamConnector(const char* _host, const char* _port): sa_(_host, _port) { connect(); }
        StreamConnector(const char* _host, uint16_t _port_num): sa_(_host, _port_num) { connect(); }
        ~StreamConnector() {
            if (connected_) {
                sock_.close();
            }
        }
        int send(void *buff, int len) {
            if (connected_) {
                return sock_.sendBytes(buff, len, 0);
            }
            return -1;
        }
        int receive(void *buff, int len) {
            if (connected_) {
                return sock_.receiveBytes(buff, len, 0);
            }
            return -1;
        }
        bool isConnected() const { return connected_;}
        const auto& socket() { return sock_; }
    private:
        Poco::Net::SocketAddress sa_;
        Poco::Net::StreamSocket sock_;
        bool connected_;
        void connect() {
            try {
                sock_.connect(sa_);
                connected_ = true;
            }
            catch (...)
            {
                connected_ = false;
            }
        }
    };

    /// Realtime task priorities
    enum TaskPrio {
        JOBS_THREAD_PRIO = 99,
        LOG_THREAD_PRIO = 29,
        MAIN_THREAD_PRIO = 39,
        RECV_THREAD_PRIO = 98,
        USER_ROBOT_MODULE_PRIO = 50,
        USER_RT_APP_PRIO = 35,
    };

    /// CPU affinity를 설정한다.
    bool setCpuAffinity(int cpuIndex);
    /// current thread를 realtime thread로 변경한다.
    int addIntoRealtimeSchedule(int sched_priority);
}; // end of namespace sephi

#endif //IPCTEST_IPC_CONFIG_H
