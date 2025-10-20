//
// Created by 정순용 on 2017. 11. 24..
//
#include <iostream>
#include "telnet_protocol_filter.h"

#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/SocketAddress.h"

int main(int argc, char** argv)
{
    Poco::Net::ServerSocket srv(8080); // does bind + listen
    Poco::Net::StreamSocket ss = srv.acceptConnection();
    uint8_t buff[10*1024];
    int len, outlen;
    TelnetComFilter filter;
    while((len = ss.receiveBytes(buff, sizeof(buff))) > 0){
        printf("recv: %s\n", buff);
        printf("filt: %s\n", filter.filter(buff, len, &outlen));
        while(filter.hasCommand()) {
            printf("POP\n");
            uint32_t val = 0;
            int cmd = filter.popCommand(&val);
            printf("Cmd = %d, val = %d\n", cmd, val);
        }
    }
    return 0;
}