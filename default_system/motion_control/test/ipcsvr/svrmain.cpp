#include "sephi_ipc.h"
#include <stdio.h>
#include <signal.h>
#include "log.h"

#include "test_config.h"
#include "myrpc.grpc.pb.h"
using namespace sephi;
using namespace myrpc;

/*
 *  Test Items & scenarios :
 *   - 공유 메모리
 *      server가 공유메모리를 생성
 *      client가 공유메모리에 attach
 *      client가 값을 적고, server에 signaling  (condition 사용)
 *      server가 값을 읽고 echo
 *   - Message Queue
 *      server가 message queue 생성 후 메시지 입력  대기
 *      client가 message queue에 attach
 *      client가 데이터 queue (N items)
 *      server가 dequeue후 echo *
 *   - RPC
 *      add 연산 interface 작성
 *      server구동
 *      client에서 rpc 호출 테스트
 */

bool g_exit = false;

using ServerContext = grpc::ServerContext;
using Status = grpc::Status;
class RpcTestImpl final : public RpcTest::Service {
    Status Process(ServerContext* context, const Config* request,
                    Response* reply) override {
        std::string prefix("Hello ");
        reply->set_message(prefix + request->data());
        SEP_LOGT("server got request!\n");
        //sleep(10);
        //SEP_LOGT("server send response!\n");
        return Status::OK;
    }
};
GrpcServer<RpcTestImpl> svr;

void sig_handler(int signo) {
    g_exit = true;
    svr.Shutdown();
    printf("Ctrl-C!\n");
}


int main() {
    signal(SIGINT, sig_handler);
    SEP_LOGT("Hello, World! Server\n");
    IpcCondition cond("shmcond", true); // create condition object
    SepShm shm(SEP_SHM_KEY(), 0x1000);
    IntegerQ mq("testmq", true);
    MsgType msg;

    SEP_LOGT("Setting hello?\n");
    snprintf((char*)shm.address, 100, "hello?");

    // wait until control-c clicked
    while(!g_exit) {
#ifdef TEST_SHM
        if (cond.wait(2.5)) {
            SEP_TRACE("waken up by client.; message : %s \n", shm.address);
        } else {
            SEP_TRACE("timeout\n");
        }
#elif defined(TEST_MSGQ)
        if (mq.get(msg, 2.0)) {
            SEP_TRACE("mq waken up by client.; message : %d \n", msg);
        } else {
            SEP_TRACE("mq timeout\n");
        }
#elif defined(TEST_RPC)
        //svr.RunServer();
        svr.RunAsThread(50052);
        while(!g_exit) {
            usleep(100000);
        }
#endif
    }

    return 0;
}