#pragma once


#include <deque>

#include <boost/asio.hpp>

#include "message.h"
#include "serial_params.h"


namespace sephi {

    namespace asio = boost::asio;
	using namespace boost::asio;


    class Serial {
    public:

        Serial();
        ~Serial();
        void start(std::string const& port_name,
                SerialParams params,
                ConnectionHandler connection_handler,
                MessageHandler message_handler);

//        void start();
        void reopen();
        void close();
        boost::system::error_code flush();

        bool write(Message const& message);

        bool is_opened() const;
        bool are_all_messages_sent() const;
        //std::unique_ptr<asio::serial_port> port_;
        void set_baudrate(uint32_t baud);
        void set_data(uint32_t val);
        void set_stop(uint32_t val);
        void set_parity(uint32_t val);
        void setRTS(bool enabled);
        void setDTR(bool enabled);
        void setCTS(bool enabled);
        void setDSR(bool enabled);

    private:
        void do_open();
        void handle_open(boost::system::error_code const& ec);

        void do_read();
        void handle_read(
                boost::system::error_code const& ec, size_t size);

        void do_write();
        void handle_write(
                boost::system::error_code const& ec, size_t size);


        bool opened_{false};
        std::string port_name_;
        SerialParams params_;

        std::thread runner_;
        asio::io_context io_context_;
        std::unique_ptr<asio::serial_port> port_;

        ConnectionHandler connection_handler_;
        MessageHandler message_handler_;

        Message received_packet_;
        std::deque<Message> sending_messages_;
        int fd;
    };

}
