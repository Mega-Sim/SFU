
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/NetException.h"
#include "Poco/StreamCopier.h"
#include <iostream>
#include <memory>
#include <string.h>

#define __SEP_TRACE(lvl, ...) do {printf("[%s:%d][%c]", __FILE__, __LINE__, lvl); printf(__VA_ARGS__);} while(0)
#define SEP_TRT(...) __SEP_TRACE('T', __VA_ARGS__)
#define SEP_TRD(...) __SEP_TRACE('D', __VA_ARGS__)
#define SEP_TRI(...) __SEP_TRACE('I', __VA_ARGS__)
#define SEP_TRW(...) __SEP_TRACE('W', __VA_ARGS__)
#define SEP_TRE(...) __SEP_TRACE('E', __VA_ARGS__)
#define SEP_TRACE SEP_TRT
#define SEP_REQUIRE_RET(cond,val) do{ if (!(cond)){SEP_TRACE(#cond); return (val);} } while(0)

using Poco::Net::SocketStream;
using Poco::Net::SocketAddress;
using std::unique_ptr;
#if 0 // !__x86_64__
//using Poco::Net::StreamSocket;
#else
class StreamSocket {
public:
    StreamSocket() {
        sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    }
    ~StreamSocket() {
        if (sockfd_ >= 0) {
            close(sockfd_);
        }
    }
    int connect(const SocketAddress& address) {
        SEP_REQUIRE_RET(sockfd_>=0, -1);
        printf("sa = %p\n", &address);
        printf("port %d\n", address.port());

        printf("port %d\n", address.port());
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        printf("port %d\n", address.port());
#ifdef __MACH__
        server_addr.sin_len = sizeof(server_addr);
#endif
        server_addr.sin_family = AF_INET;
        //server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        //server_addr.sin_port = htons(1111);
        server_addr.sin_addr.s_addr = inet_addr(address.host().toString().c_str());
        printf("port %d\n", address.port());
        server_addr.sin_port = htons(address.port());
        int connect_ret = ::connect(sockfd_, (struct sockaddr *)&server_addr, sizeof(server_addr));
        printf("ret = %d\n", connect_ret);
        SEP_REQUIRE_RET(connect_ret >= 0, -1);

        return  0;
    }
    int shutdownSend() {
        return 0;
    }
    int sendBytes(void* buff, size_t len, uint32_t flags) {
        return ::send(sockfd_, buff, len, flags);
    }

private:
    int sockfd_;
};
#endif
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"

class Connector
{
public:
    Connector():  sa_("localhost", 1111)
    {
        printf("sa_ = %p, sock_ = %p, sizeof SocketAddress = %lu\n", &sa_, &sock_, sizeof(SocketAddress));

        printf("port %d\n", sa_.port());
        try {
            printf("sa = %p\n", &sa_);

            sock_.connect(sa_);

            connected_ = true;
        }
        catch (Poco::Net::NetException& e)
        {
            connected_ = false;
            printf("%s\n", e.displayText().c_str());
        }
    }
    ~Connector()
    {
        if (connected_) {
            sock_.shutdownSend();
        }
        //sock_.close();
    }
    int send(void *buff, int len)
    {
        if (connected_) {
            return sock_.sendBytes(buff, len, 0);
        } else {
            // remove prefix & posfix %%<< %%>>
            char* content = (char*)buff + 4;
            content[len-8] = 0;
            return printf("%s", content);
        }
    }
private:
    StreamSocket sock_;
    SocketAddress sa_;
    bool connected_;
};


int main(int argc, char** argv)
{
    Connector con;
    char message[100];
    strncpy(message, "%%<<hello\n%%>>", sizeof(message));
    con.send((void*)message, strlen(message));

    return 0;
}

