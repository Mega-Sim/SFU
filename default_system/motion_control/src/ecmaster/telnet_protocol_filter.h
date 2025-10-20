//
// Created by 정순용 on 2017. 11. 24..
//

#ifndef CPPPRJ_TELNET_PROTOCOL_FILTER_H
#define CPPPRJ_TELNET_PROTOCOL_FILTER_H

#include <cstdint>
#include <array>
#include <list>
#include "sephi_ipc.h"

namespace sephi {
    class TelnetComFilter {
    public:
        enum Constant : uint8_t {
            // sub nego
                SET_BAUDRATE = 1,
            SET_DATASIZE,
            SET_PARITY,
            SET_STOPSIZE,
            SET_CONTROL,
            NOTIFY_LINESTATE,
            NOTIFY_MODEMSTATE,
            FLOWCONTROL_SUSPEND,
            FLOWCONTROL_RESUME,
            SET_LINESTATE_MASK,
            SET_MODEMSTATE_MASK,
            PURGE_DATA,
            // config
            WILL_SEND_COM_OPT,
            WONT_SEND_COM_OPT,
            ACCEPT_COM_OPT,
            REFUSE_COM_OPT,
            // special character
                COM_PORT_OPTION = 44,
            SE = 240,   // End of Subnegotiation param, 240, f0
            NOP,        // No OPeration, 241, f1
            DM,         // data mark, 242, f2
            BRK,        // NVT character BRK, 243, f3
            IP,         // interrupt process, f4
            AO,         // abort output, f5
            AYT,        // are you there, f6
            EC,         // erase character, f7
            EL,         // erase line, f8
            GA,         // go ahead, f9
            SB,         // SuBnegotiation, 250,fa
            WILL,       // desire to begin performing, 251,fb
            WONT,       // refuse to perform,252, fc
            DO,         // request that the other party perform, 253, fd
            DONT,       // request that the other party stop, 254, fe
            IAC,         // interpret as command, 255, ff
        };
        enum {
            BUFFER_SIZE = 1024,
        };
        struct Command {
            Command(): command(0), length(0){}
            std::array<uint8_t, 10> data;
            int command;
            int length;
            uint32_t value; // sub negotiaion value
            void print();
        };

    public:
        TelnetComFilter() : cmd_index_(-1) { cmd_.data[0] = IAC; }

        uint8_t *filter(const uint8_t *stream, int srclen, int *outlen);

        bool hasCommand() { return !commands_.empty(); }
        bool popCommand(Command& cmd);
        // const string
        const static std::array<uint8_t, 3> DO_CONFIG;
        const static std::array<uint8_t, 3> DONT_CONFIG;
        bool IAC_flag;
    private:
        int cmd_index_; // location index of cmd_ buffer
        Command cmd_;
        std::list<Command> commands_;
        uint8_t out_[BUFFER_SIZE]; // filtered output
    };
}
#endif //CPPPRJ_TELNET_PROTOCOL_FILTER_H
