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
// Created by sy1135.jeong1696 on 2017-07-24.
//
#include <ecmaster_service.h>
#include <memory>
#include <algorithm>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Timespan.h>
#include <boost/version.hpp>

#include <string>
#include <fstream> // For file I/O (reading/writing to COM port)
#include <sstream>

#if BOOST_VERSION >= 106400 // 1.64 이후 에서 지원
#include <boost/process.hpp>
#endif
#include "ecat_serial.h"
#include "telnet_protocol_filter.h"
#include "ecmaster_service.h"
#include "emul/ecat_emulator.h"
#include "serial/serial.h"
#include <yaml-cpp/yaml.h>

#include <boost/asio.hpp>
#define SER_LOGT(...) SEP_LOGT(__VA_ARGS__)


namespace sephi
{
    // TCP Server for RFC2217
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    using Poco::Net::Socket;
    using Poco::Net::SocketAddress;
    using Poco::Net::ServerSocket;
    using Poco::Net::StreamSocket;
    using Poco::Timespan;
    using std::unique_ptr;
#if BOOST_VERSION >= 106400 // 1.64 이후 에서 지원
    namespace bp = boost::process;
    static std::deque<bp::child> _redirections;
#endif
	using boost::asio::ip::tcp;


    using namespace std::chrono_literals;

    using std::placeholders::_1;
    using std::placeholders::_2;
    constexpr auto limit{10'000};

    struct RFC2217ServiceConnectionParam {
        ECatSerialSlave* slave;
        uint16_t portnum;   // socket port number for rfc2217
        int serial_port_id; // serial port id of slave device
        string dev;
        BaudRate baud;
    };

#define SERIAL_PORT_ID_ATLAS 10


#define DUMP_MESSAGE(header, msg, len) SER_LOGT("port%d[%s](%d)%s\n", serial_port_id, header, len, msg)
    class RFC2217ServiceConnection{ // This class handles all client connections.
        using Queue = SimpleQueue<ECatSerialData::QUEUE_BUFFER_SIZE, std::mutex>;
    public:
        RFC2217ServiceConnection(const RFC2217ServiceConnectionParam& param):
                serversock{param.portnum}, connected(false),
                slave(param.slave), serial_port_id(param.serial_port_id), portnum(param.portnum)
        {
            //SEP_LOGT("Serial slaveid=%d port=%d, tcp_port=%d\n", slave->getSlaveId(), serial_port_id, param.portnum);

            if(serial_port_id != SERIAL_PORT_ID_ATLAS) {
                rxqueue = slave->getRxQueue(serial_port_id);
                txqueue = slave->getTxQueue(serial_port_id);
            } else{
                SerialParams p(param.baud);
                start(param.dev,p);
            }
        }



        void accept()
        {
            SER_LOGT("Session Connected: this=%p, slave=%p, serial port id=%d\n", this, slave, serial_port_id);
            streamsock = serversock.acceptConnection();
            connected = true;
        }

        void receiveTxData()
        {
            char buff[ECatSerialData::QUEUE_BUFFER_SIZE-1];    // buffer 사이즈가 queue 사이즈와 동일할 경우 push 에러로 데이터 누락됨, 230825
            Timespan timeout(0,5000);
            unsigned int pos = 0;
            try {
                int maxlen  = int(sizeof(buff));
                // soslab은 send data 크기가 커서 예외처리 없이 받을 경우 txqueue에 계속 push 하게 되어 데이터 누락 발생함
                // EtherCAT에서 txqueue 데이터를 전부 pop 하기 전까지 예외처리, slave로 데이터 전송이 완료되면 다시 network 데이터 수신
                if(serial_port_id != SERIAL_PORT_ID_ATLAS)
					if(txqueue->size() != 0) return;
                auto len = streamsock.receiveBytes(buff, maxlen, MSG_DONTWAIT);
                //DUMP_MESSAGE("from main", (const char*)buff, len); // from main
                if (len == 0) {
                    SER_LOGT("Connection closed!\n");
                    connected = false;
                } else {
                    while(pos < len){
                        if(buff[pos] == (char)0xFF){  // IAC (Interpret as Command)
                            if (pos + 1 < len) {
                                if (buff[pos + 1] == (char)0xFF) {  // Escaped 0xFF (Regular Data)
                                    // 일반 데이터에서 이스케이프된 `0xFF 0xFF` 처리
                                    handle_regular_data("\xFF", 1);
                                    pos += 2;  // 이스케이프된 `0xFF 0xFF`를 건너뜀
                                }
                                else if(buff[pos + 1] == (char)0xFA) {
                                    size_t end_pos = find_telnet_end(buff, len, pos);
                                    if(end_pos != std::string::npos){
                                        handle_rfc2217_command(buff, len, pos, end_pos);
                                        pos = end_pos + 1;
                                    }
                                    else{
                                        // 에러 로그
                                        break;
                                    }
                                }
                                else{
                                    handle_telnet_command(buff, len, pos);
                                    pos += 3;
                                }
							}
                        }
                        else{
                            size_t next_pos = find_next_telnet_command(buff, len, pos);
                            auto unescaped_data = unescape_data(&buff[pos], next_pos - pos);
                            handle_regular_data(unescaped_data.data(), unescaped_data.size());
                            pos = next_pos;
                        }
                    }
                } 
            } catch (...) {
                //if (serial_port_id < 10)
                    connected = false;
            }
        }

        void sendRxData()
        {
            if(serial_port_id == SERIAL_PORT_ID_ATLAS) return;
            auto sendlen = rxqueue->size();
            if(sendlen > 0) {
                uint8_t buff[ECatSerialData::QUEUE_BUFFER_SIZE];    // original
                rxqueue->pop(buff, sendlen);
                int ffcnt = 0;
                for (int i = 0; i < sendlen; ++i) {
                    if(buff[i] == 0xff) ffcnt++;
                }
                uint8_t buff_[ECatSerialData::QUEUE_BUFFER_SIZE+ffcnt];   // for telnet
                if (ffcnt != 0) {
                    int pCnt = 0;
                    for (int i = 0; i < sendlen; ++i) {
                        if (buff[i] == 0xff) {          // 0xff is a special character in the Telnet protocol. Try send it twice (0xff, 0xff)
                            buff_[i+pCnt] = buff[i];
                            buff_[i+pCnt+1] = buff[i];
                            pCnt += 1;
                        } else {
                            buff_[i+pCnt] = buff[i];
                        }
                    }
                } else {
                    memcpy(buff_, buff, sendlen);
                }
                try {
                    streamsock.sendBytes(buff_, sendlen + ffcnt);    // to main
                    char data[500] = {0,};
                } catch (...) {
                    connected = false;
                }
            }
        }

        size_t find_telnet_end(const char* data, size_t length, size_t start) {
            for (size_t i = start + 2; i < length; ++i) {
                if (data[i] == (char)0xFF && i + 1 < length && data[i + 1] == (char)0xF0) {  // IAC SE
                    return i + 1;
                }
            }
            return std::string::npos;  // SE를 찾지 못함
        }

        size_t find_next_telnet_command(const char* data, size_t length, size_t start) {
            for (size_t i = start; i < length; ++i) {
                if (data[i] == (char)0xFF) {  // IAC
                    return i;
                }
            }
            return length;  // Telnet 명령어를 찾지 못함
        }

        void handle_regular_data(const char* data, size_t length) {
            std::string regular_data(data, length);
            
            //일반 데이터
            if(serial_port_id != SERIAL_PORT_ID_ATLAS){
                txqueue->push(data, length);
			}
            else{
                Message message{reinterpret_cast<uint8_t const *>(regular_data.c_str()), regular_data.size()};
                write(message);
            }
        }

        bool handle_rfc2217_command(const char* data, size_t length, size_t start, size_t end){
            if(end - start < 4){
				SER_LOGT("(%d)command short\n",portnum);
                return false;
            }
            unsigned char command = data[start + 3];
            switch (command){
                case TelnetComFilter::SET_BAUDRATE: {
                    unsigned int baud_rate = ntohl(*(uint32_t *) &data[start+ 4]);
                    SER_LOGT("(%d)Set Baurate %d\n", portnum, baud_rate);
                    if(serial_port_id != SERIAL_PORT_ID_ATLAS) 
                        slave->setBaudRate(serial_port_id, baud_rate);
                    else{
							client_.set_baudrate(baud_rate);
						}
                    break;
                }
                case TelnetComFilter::SET_DATASIZE: {
                    unsigned char value = data[start + 4];
                    SER_LOGT("(%d)Set Databit %d\n", portnum, value);
                    if(serial_port_id != SERIAL_PORT_ID_ATLAS) 
						slave->setDataBit(serial_port_id, value);
                    else{
							client_.set_data(value);
						}
                    break;
                }
                case TelnetComFilter::SET_STOPSIZE: {
                    unsigned char value = data[start + 4];
                    SER_LOGT("(%d)Set Stopbit %d\n", portnum, value);
                    if(serial_port_id != SERIAL_PORT_ID_ATLAS) 
						slave->setStopBit(serial_port_id, value);
					else{
						client_.set_stop(value);
					}
                    break;
                }
                case TelnetComFilter::SET_PARITY: {
                    unsigned char value = data[start + 4];
                    SER_LOGT("(%d)Set Parity %d\n", portnum, value);
                    if(serial_port_id != SERIAL_PORT_ID_ATLAS) 
						slave->setParity(serial_port_id, value);
					else{
						client_.set_parity(value);
					}
                    break;
                }
                case TelnetComFilter::SET_CONTROL: {
                    if(serial_port_id != SERIAL_PORT_ID_ATLAS)
                        break;
					unsigned char value = data[start + 4];
                    SER_LOGT("(%d)Set control %d\n", portnum, value);
                    if(value == 8)
						client_.setDTR(true);
					else if(value==9)
						client_.setDTR(false);
					else if(value==11)
						client_.setRTS(true);
					else if(value==12)
						client_.setRTS(false);
				
					break;
				}
                default:
                {
						SER_LOGT("(%d)unknown cmd: %d", portnum, command);
						break;
				}
                    // Unkwon data
            }
            
            if(command == 1){
				char response[] = {(char)0xFF, (char)0xFA, (char)0x2C, command+100, data[start + 4], data[start + 5], data[start + 6], data[start + 7], (char)0xFF, (char)0xF0 };							
				streamsock.sendBytes(response, sizeof(response));
			}
			else if(command == 8){   //확인 필요 CIS
				char response[] = {(char)0xFF, (char)0xFA, (char)0x2C, (char)0x09, (char)0xFF, (char)0xF0 };
				streamsock.sendBytes(response, sizeof(response));
				char responses[] = {(char)0xFF, (char)0xFA, (char)0x2C, (char)0x09+100, (char)0xFF, (char)0xF0 };
				streamsock.sendBytes(responses, sizeof(response));
			}
			else{
				char response[] = {(char)0xFF, (char)0xFA, (char)0x2C, command+100, data[start + 4], (char)0xFF, (char)0xF0 };
				streamsock.sendBytes(response, sizeof(response));
			}

            return true; 
        }

        void handle_telnet_command(const char* data, size_t length, size_t start) {
            if(start + 1 < length) {
                unsigned char command = data[start + 1];
                unsigned char option = 0;
                if(start + 2 < length) {
					option = data[start + 2];
				}
				
				switch (command) {
                case 0xFB:  // WILL
                    SER_LOGT("(%d)Telnet 명령어: WILL, 옵션 - %d\n", portnum,  static_cast<int>(option));
                    send_telnet_command(0xFD, option);  // 응답: DO
                    break;

                case 0xFC:  // WONT
                    SER_LOGT("(%d)Telnet 명령어: WONT, 옵션 - %d\n", portnum,  static_cast<int>(option));
                    send_telnet_command(0xFE, option);  // 응답: DONT
                    break;

                case 0xFD:  // DO
                    SER_LOGT("(%d)Telnet 명령어: DO, 옵션 - %d\n", portnum,  static_cast<int>(option));
                    if (option == 0x2C) {  // RFC2217 COM-PORT-OPTION
                        send_telnet_command(0xFB, option);  // 응답: WILL
                    } else {
                        send_telnet_command(0xFC, option);  // 응답: WONT
                    }
                    break;

                case 0xFE:  // DONT
                    SER_LOGT("(%d)Telnet 명령어: DONT, 옵션 - %d\n", portnum,  static_cast<int>(option));
                    send_telnet_command(0xFC, option);    // 응답: WONT
                    break;

                default:
                    SER_LOGT("(%d)알 수 없는 Telnet 명령어: %d\n", portnum,  static_cast<int>(command)) ;
                    break;
                }
			}
        }
        
        void send_telnet_command(unsigned char command, unsigned char option) {
			char response[] = {(char)0xFF, command, option };
            streamsock.sendBytes(response, sizeof(response));
		}
        
        std::vector<char> escape_data(const char* data, size_t length) {
            std::vector<char> escaped_data;
            for (size_t i = 0; i < length; ++i) {
                escaped_data.push_back(data[i]);
                if (data[i] == (char)0xFF) {
                    // `0xFF`를 이스케이프 처리 (Telnet 규칙)
                    escaped_data.push_back((char)0xFF);
                }
            }
            return escaped_data;
        }

        // 수신 데이터에서 Escape 해제
        std::vector<char> unescape_data(const char* data, size_t length) {
            std::vector<char> unescaped_data;
            for (size_t i = 0; i < length; ++i) {
                if (data[i] == (char)0xFF && i + 1 < length && data[i + 1] == (char)0xFF) {
                    // `0xFF 0xFF` -> 단일 `0xFF`로 복원
                    unescaped_data.push_back((char)0xFF);
                    ++i;  // 추가 `0xFF`는 건너뜀
                } else {
                    unescaped_data.push_back(data[i]);
                }
            }
            return unescaped_data;
        }

    
    public:
        ECatSerialSlave *slave;  // slave device instance
        int serial_port_id;      // serial port id in slave device
        int portnum;
        Queue* rxqueue;           // queue buffer for Slave --> IPC; recv from slave
        Queue* txqueue;            // queue buffer for IPC --> Slave; send to slave
        TelnetComFilter protocol_filter; // telnet protocol(rfc2217,854) command filtering
        StreamSocket streamsock;
        ServerSocket serversock;
        bool connected;

    private:
        void start(std::string const& port_name, SerialParams params){

            client_.start(port_name, params,
                          std::bind(&RFC2217ServiceConnection::connection_handler, this, _1),
                          std::bind(&RFC2217ServiceConnection::message_handler, this, _1));
        }
        bool write(Message const& message){
            return client_.write(message);
        }

        void close(){
            client_.close();
        }

        void connection_handler(bool connected)
        {
            connected_ = connected;
            if (connected_) {
                SER_LOGT("(%d)Connected", portnum);
                return;
            }
            SER_LOGT("(%d)Disconnected, try to reconnect", portnum);
            std::this_thread::sleep_for(1s);
            client_.reopen();
        }

        void message_handler(Message const& message)
        {
            Timespan timeout(0,5000);
            if(  message.size() > 0 && connected    ) {
                if(streamsock.poll(timeout,Socket::SelectMode::SELECT_WRITE)){
                    auto escaped_data = escape_data((const char*)message.data(), message.size());
					streamsock.sendBytes((const char*)escaped_data.data(), escaped_data.size());
					
                }
            }
        }
        bool connected_{false};
        Serial client_;
    };

    // all ethercat serial connection list
    static std::deque<RFC2217ServiceConnection> _ecat_serial_connection_list;
    static std::deque<RFC2217ServiceConnection> _com_serial_connection_list;

    struct Atlas_Serial{
        string device;
        BaudRate baud;
        int port;
        Atlas_Serial(string device_, BaudRate baud_, int port_){
            device = device_;
            baud = baud_;
            port = port_;
        }
    };

    std::list<Atlas_Serial> atlas_serial_list;

    static uint16_t generateNewPortNumber()
    {
        static std::mutex mtx;
        std::lock_guard<std::mutex> guard(mtx);
        static uint16_t portnum = RF2217_ECAT_SLAVE_PORT;
        return portnum++;
    }

    bool RFC2217ServiceTask::initialize()
    {
                // 각 포트에 대한 connection 설정
        for(auto& slave : *task_param_.serials) {
			
            for(int i = 0 ; i < slave.portCount(); ++i) {
                RFC2217ServiceConnectionParam param{&slave, generateNewPortNumber(), i}; //{ECastSerialSlave*, portnum, serial_port_id}
                _ecat_serial_connection_list.emplace_back(param);
            }
        }

        atlas_serial_list.push_back(Atlas_Serial("/dev/ttyS4",BaudRate::br115200,40004)); //QRR R
        atlas_serial_list.push_back(Atlas_Serial("/dev/ttyS5",BaudRate::br115200,40015));//BCR Node
        atlas_serial_list.push_back(Atlas_Serial("/dev/ttyS6",BaudRate::br115200,40006));//QRR L
        atlas_serial_list.push_back(Atlas_Serial("/dev/ttyS7",BaudRate::br115200,40016));//BCR Station
        atlas_serial_list.push_back(Atlas_Serial("/dev/ttyS8",BaudRate::br115200,40011));//OBS
        atlas_serial_list.push_back(Atlas_Serial("/dev/ttyS9",BaudRate::br115200,40012));//OHT DETECT


        for(auto serial : atlas_serial_list){
            RFC2217ServiceConnectionParam param{NULL, serial.port, SERIAL_PORT_ID_ATLAS ,serial.device,serial.baud};
            _ecat_serial_connection_list.emplace_back(param);
        }

//
        if (the_ec_emulator.get()->emulationMode()) {
            ECatSerialSlave *slave = (ECatSerialSlave *) the_ec_emulator.get()->onboardIo();
            for(int i = 0 ; i < slave->portCount(); ++i) {
                RFC2217ServiceConnectionParam param{slave, generateNewPortNumber(), i};
                _ecat_serial_connection_list.emplace_back(param);
            }
        }
        if (set_mode) {
            redirect2tty();
            loadSerialConfig();
            set_baudrate();
        }

        return true;
    }

    void RFC2217ServiceTask::finalize()
    {
        exit_.exit();
#if BOOST_VERSION >= 106400 // 1.64 이후 에서 지원
        SER_LOGT("Destroy ethercat serial to tty mappings.");
        for(auto& child : _redirections) {
            child.terminate();
        }
#endif
    }

    void RFC2217ServiceTask::redirect2tty()
    {
#if !AMC_CLIENT_MODE  && BOOST_VERSION >= 106400 // 1.64 이후 에서 지원
        auto socat = bp::search_path("socat");
        if (socat.size() == 0) {
            SEP_LOGW("socat is not installed! install socat like as 'sudo apt install socat'");
            return;
        }
        for(auto& slave : *task_param_.serials) {
            char type(' ');
            int port_base(0);
            if (slave.getSlaveType() == SlaveType::FINE_OHT_FRONT_IO) {
                type = 'F';
                port_base = RF2217_ECAT_SLAVE_PORT + ECatSerialData::SERIAL_PORT_COUNT_PER_SLAVE;
            } else if (slave.getSlaveType() == SlaveType::FINE_OHT_REAR_IO) {
                type = 'R';
                port_base = RF2217_ECAT_SLAVE_PORT;
            } else {
                SEP_LOGE("Unsupported serial slave type!");
                continue;
            }
            for(int i = 0 ; i < slave.portCount(); ++i) {
                const int max_param_size = 128;
                char param_tty[max_param_size], param_tcp[max_param_size];
                snprintf(param_tty,max_param_size,"pty,link=/dev/ttyOht%c%d", type, i);
                snprintf(param_tcp,max_param_size,"tcp:localhost:%d", port_base+i);
                SEP_LOGT(">>> map ethercat serial to tty: %s %s %s", socat.string().c_str(), param_tty, param_tcp);
                _redirections.emplace_back(socat, (const char*)param_tty, (const char*)param_tcp);
            }
        }
#else
        SEP_LOGW("Redirection function requires boost 1.64 or later!. Do redirect manualy using socat.");
#endif
    }

    void RFC2217ServiceTask::loadSerialConfig()
    {
        try {
            string serial_file = "serialconfig.yml";
            auto config = YAML::LoadFile(serial_file)["SerialConfig"];
            for (int i = 0; i < config.size(); ++i) {
                SerialConfig sc;
                auto Port = config[i]["Port"].as<int>();
                sc.baudrate = config[i]["Baudrate"].as<int>();
                sc.databit = config[i]["DataBit"].as<int>();
                sc.stopbit = config[i]["StopBit"].as<int>();
                sc.parity = config[i]["Parity"].as<int>();
                //printf("%d %d %d %d %d\n", Port, sc.baudrate, sc.databit, sc.stopbit, sc.parity);
                serialcfg.insert(std::make_pair(Port, sc));
            }
        } catch(std::exception &e) {
            SEP_TRE("Slave serial config Loading error! : %s", e.what());
        }
    }
    
    void RFC2217ServiceTask::set_baudrate()
    {
        try {
            for(auto& slave : *task_param_.serials) {
                if (slave.getSlaveType() == SlaveType::FINE_OHT_FRONT_IO) {
                    for(int i = 0; i < slave.portCount(); ++i) {
                        auto cfg = serialcfg[i+RF2217_ECAT_SLAVE_PORT+8];
                        slave.setBaudRate(i, cfg.baudrate);
                        slave.setDataBit(i, cfg.databit);
                        slave.setStopBit(i, cfg.stopbit);
                        slave.setParity(i, cfg.parity);
                        //printf("%d %d %d %d\n", cfg.baudrate, cfg.databit, cfg.stopbit, cfg.parity);
                    }
                } else if (slave.getSlaveType() == SlaveType::FINE_OHT_REAR_IO) {
                    for(int i = 0; i < slave.portCount(); ++i) {
                        auto cfg = serialcfg[i+RF2217_ECAT_SLAVE_PORT];
                        slave.setBaudRate(i, cfg.baudrate);
                        slave.setDataBit(i, cfg.databit);
                        slave.setStopBit(i, cfg.stopbit);
                        slave.setParity(i, cfg.parity);
                        //printf("%d %d %d %d\n", cfg.baudrate, cfg.databit, cfg.stopbit, cfg.parity);
                    }
                } else {
                    SEP_LOGE("Unsupported serial slave type!");
                    continue;
                }
            }
        } catch (std::exception &e) {
            SEP_TRE("Slave serial config setting error! : %s", e.what());
        }
	}

    void RFC2217ServiceTask::svc()
    {
        addIntoRealtimeSchedule(MAIN_THREAD_PRIO+1);

        // physical serial port마다 독립된 connection을 가진다고 가정
        // Init : 어느 device 어느 port인지 설정 : 약속된 포맷으로 수신 받아야 함
        // Loop :
        //       check data from IPC --> push to tx buffer
        //       check tx buffer --> push to portdata
        //       check rx buffer --> send to IPC
        Timespan timeout(0,1000); // sec, usec
        //Socket::SocketList readList, writeList, exceptList;
        int size;
        printf("*** RFC2217 Begin Task!\n");
        while(!stop_) {

//        SEP_LOGT("----------------------------Start Session----------------------------------------------------");
            for (auto &port : _ecat_serial_connection_list) {
                if(port.connected) {
                    if (port.streamsock.poll(timeout, Socket::SelectMode::SELECT_READ)) {    // read mode ?
                        port.receiveTxData();
                    }
                    
					if(port.serial_port_id == SERIAL_PORT_ID_ATLAS) {
						continue;
					}
                    if (port.streamsock.poll(timeout, Socket::SelectMode::SELECT_WRITE)) {   // write mode ?
                        if(port.serial_port_id != SERIAL_PORT_ID_ATLAS) {
                            port.sendRxData();
                        }
                    }
                }
                else{
                    if(port.serversock.poll(timeout, Socket::SelectMode::SELECT_READ) || port.serversock.poll(timeout, Socket::SelectMode::SELECT_WRITE)){
                        port.accept();
                    }
                }
            }
        }
            printf("**** RFC2217 End Task!\n");
    }
}
