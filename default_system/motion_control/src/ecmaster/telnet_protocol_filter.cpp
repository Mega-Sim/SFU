//
// Created by 정순용 on 2017. 11. 24..
//

#include "telnet_protocol_filter.h"

namespace  sephi {

    const std::array<uint8_t, 3> TelnetComFilter::DO_CONFIG = {IAC, DO, COM_PORT_OPTION};
    const std::array<uint8_t, 3> TelnetComFilter::DONT_CONFIG = {IAC, DONT, COM_PORT_OPTION};


    void TelnetComFilter::Command::print()
    {
        char buffer[1024] = {0};
        int offset(0);
        for(int i = 0; i < length; ++i) {
            offset += snprintf(buffer+ offset, sizeof(buffer)-offset, "%02x ", data[i]);
        }
        //SEP_LOGT("%s", buffer);
    }

    uint8_t *TelnetComFilter::filter(const uint8_t *stream, int srclen, int *_outlen) {
        int outlen = 0;
        for (int i = 0; i < srclen; i++) {
            uint8_t ch = stream[i];
            // command 종료 조건
            if (cmd_index_>=0) {
                cmd_.data[++cmd_index_] = ch;
                if (cmd_.data[1] == SB) { // sub-nego command
                    if (ch == SE) {
                        //SEP_LOGT("add SB\n");
                        cmd_.length = cmd_index_+1;
                        cmd_.print();
                        commands_.push_back(cmd_);
                        cmd_index_ = -1;
                    }
                } else {
                    if (cmd_index_ == 2) {
                        if (ch == COM_PORT_OPTION || ch == IP || ch == AO || ch == AYT || ch == GA) {
                            //SEP_LOGT("add other\n");
                            cmd_.length = cmd_index_ + 1;
                            cmd_.print();
                            commands_.push_back(cmd_);
                            cmd_index_ = -1;
                        }
                    }
                }
            }
            else if (ch == IAC) {   // 현재 바이트가 0xff인경우 이어지는 바이트를 확인
                if (i == 0 && IAC_flag) {       // IAC_flag : 이전 buffer의 마지막이 0xff로 끝났으니 첫 1바이트는 바로 써도 된다
                    out_[outlen++] = ch;
                    IAC_flag = false;
                }
                else if (i < srclen-1)          // 마지막 바이트를 제외하고 2바이트씩 명령어 여부 체크
                {
                    uint8_t temp_ch = stream[i+1];
                    if (temp_ch == IAC)         // 다음값이 0xff인지 확인하여 앞의 0xff만 사용
                    {
                        out_[outlen++] = ch;
                        i++;
                    } else{
                        cmd_index_ = 0;         // 다음값이 0xff가 아니면 명령어로 사용, ex) 0xf0~0xfe
                        cmd_.data[1] = 0; // invalidate
                    }
                } else {                        // 마지막 바이트가 0xff 한개인 경우
                    IAC_flag = true;
                }
            } else {    // 현재 바이트가 0xff가 아닌경우
                if (i == 0 && IAC_flag && (ch >= SE && ch <= DONT)) {       // 이전 buffer 마지막이 0xff로 끝나고 첫 1바이트가 명령어(0xf0~0xf3)인지 체크
                    cmd_index_ = 0;
                    cmd_.data[1] = 0;
                    cmd_.data[++cmd_index_] = IAC;  // 이전 buffer 마지막의 0xff를 넣어줌
                    IAC_flag = false;
                } else {
                    out_[outlen++] = ch;
                }
            }
        }
        out_[outlen] = 0;
        *_outlen = outlen;
        return out_;
    }


    bool TelnetComFilter::popCommand(Command& cmd) {
        SEP_REQUIRE_RET(!commands_.empty(), false);
        cmd = commands_.front();
        commands_.pop_front();

        if (cmd.data[1] == WILL){
            cmd.command = WILL_SEND_COM_OPT;
        }
        else if (cmd.data[1] == WONT){
            cmd.command = WONT_SEND_COM_OPT;
        }
        else if (cmd.data[1] == DO){
            cmd.command = ACCEPT_COM_OPT;
        }
        else if (cmd.data[1] == DONT){
            cmd.command = REFUSE_COM_OPT;
        }
        else if (cmd.data[1] == SB) {
            cmd.command = cmd.data[3];
            if (cmd.data[3] == SET_BAUDRATE) {
                cmd.value = ntohl(*(uint32_t *) &cmd.data[4]);
            } else {
                cmd.value = cmd.data[4];
            }
        } else {
            throw std::runtime_error("Unknown command!");
        }
        return true;
    }

}
