#include "sephi_ipc.h"
#include <stdio.h>
#include <boost/format.hpp>
#include "log.h"
#include "test_config.h"
#include "myrpc.grpc.pb.h"
using namespace sephi;
using namespace myrpc;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class  RpcTestCli: public GrpcClient<RpcTest> {
public:
    RpcTestCli(std::shared_ptr<Channel> channel) : GrpcClient<RpcTest>(channel) {}
    //ClientContext context;

    // Assembles the client's payload, sends it and presents the response back
    // from the server.

    std::string Process(const std::string& user) {
        // Data we are sending to the server.
        Config request;
        request.set_data(user);

        // Container for the data we expect from the server.
        Response reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        // The actual RPC.
        SEP_LOGT("begin call\n");
        Status status = stub_->Process(&context, request, &reply);
        SEP_LOGT("end call\n");

        // Act upon its status.
        if (status.ok()) {
            return reply.message();
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return "RPC failed";
        }
    }

};

int main() {
    SEP_LOGT("Hello, World! Client\n");

#ifdef TEST_SHM
    SepShm shm(SEP_SHM_KEY());
    IpcCondition cond("shmcond");
    SEP_LOGT("%s\n", shm.address);
    snprintf(shm.address, 100, "hello server?\n");
    cond.wakeUp();
#elif defined(TEST_MSGQ)
    IntegerQ mq("testmq");
    const int size = 10000;
    SEP_LOGT("mq client send %d messages\n", size);
    for(MsgType i = 0; i < size; ++i) {
        mq.put(i);
    }
#elif defined(TEST_RPC)
    try {
        RpcTestCli greeter(grpc::CreateChannel("localhost:50052", grpc::InsecureChannelCredentials()));
        std::string user("world");
        std::string reply;
        for(int i = 0; i < 1; i++) {
            user = boost::str(boost::format("count %d") % i);
            reply = greeter.Process(user);
        }
        std::cout << "Greeter received: " << reply << std::endl;
    } catch (boost::interprocess::interprocess_exception& e) {
        std::cout << "EXCEPTION! " << e.what() << std::endl;
    } catch (std::exception &e ) {
        std::cout << "EXCEPTION! " << e.what() << std::endl;
    } catch (...) {
        std::cout << "EXCEPTION! " << std::endl;
    }


#endif
    return 0;
}//

