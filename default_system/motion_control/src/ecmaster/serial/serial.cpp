#include "serial.h"

#include <boost/bind.hpp>
#include <sys/ioctl.h>


using boost::asio::buffer;
using boost::system::error_code;

using namespace sephi;

Serial::Serial()
{

}
Serial::~Serial()
{
    io_context_.stop();
    if (runner_.joinable()) {
        runner_.join();
    }
}

void Serial::start(std::string const& port_name,
                   SerialParams params,
                   ConnectionHandler connection_handler,
                   MessageHandler message_handler){
    port_name_ = port_name;
    params_ = params;
    connection_handler_ = std::move(connection_handler);
    message_handler_ = std::move(message_handler);

    do_open();
    runner_ = std::thread{[this] {io_context_.run();}};
}

void Serial::reopen()
{
    close();
    do_open();
}

void Serial::close()
{
    opened_ = false;
    post(io_context_, [this] {port_->close();});
}

error_code Serial::flush()
{
    error_code ec;
    bool is_flushed{false};
#if !defined(BOOST_WINDOWS) && !defined(__CYGWIN__)
    is_flushed = !::tcflush(port_->native_handle(), TCIOFLUSH);
    if (!is_flushed)
        ec = error_code{
                errno,
                asio::error::get_system_category()};
#else
    is_flushed = ::PurgeComm(
        port_->native_handle(),
        PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);
    if (!is_flushed)
        ec = error_code{
            static_cast<int>(::GetLastError()),
            asio::error::get_system_category()};
#endif
    return ec;
}

bool Serial::write(Message const& message)
{
    if (!is_opened())
        return false;

    post(
            io_context_,
            [this, message] {
                auto write_in_progress{!sending_messages_.empty()};
                sending_messages_.emplace_back(message);
                if (!write_in_progress)
                    do_write();
            });
    return true;
}

bool Serial::is_opened() const
{
    return opened_;
}

bool Serial::are_all_messages_sent() const
{
    return sending_messages_.empty();
}

void Serial::do_open()
{
    opened_ = false;
    port_ = std::make_unique<asio::serial_port>(io_context_);
    post(
            io_context_,
            [this] {
                error_code ec;
                port_->open(port_name_, ec);
                handle_open(ec);
            });
}

void Serial::handle_open(error_code const& ec)
{
    if (ec) {
        connection_handler_(is_opened());
        return;
    }

    port_->set_option(params_.baud_rate());
    port_->set_option(params_.character_size());
    port_->set_option(params_.stop_bits());
    port_->set_option(params_.parity());
    port_->set_option(params_.flow_control());
    fd = port_->native_handle();

    auto err_code{flush()};
    if (err_code) {
        connection_handler_(is_opened());
        return;
    }

    opened_ = true;
    connection_handler_(is_opened());
    do_read();
}

void Serial::do_read()
{
    port_->async_read_some(
            buffer(received_packet_.data(), Message::max_size),
            boost::bind(
                    &Serial::handle_read,
                    this,
                    boost::placeholders::_1,
                    boost::placeholders::_2));
}

void Serial::handle_read(error_code const& ec, size_t size)
{
    if (ec) {
        opened_ = false;
        connection_handler_(is_opened());
        return;
    }

    received_packet_.size(size);
    message_handler_(received_packet_);
    do_read();
}

void Serial::do_write()
{
    auto const& message{sending_messages_.front()};
    async_write(
            *port_,
            buffer(message.data(), message.size()),
            boost::bind(
                    &Serial::handle_write,
                    this,
                    boost::placeholders::_1,
                    boost::placeholders::_2));
}

void Serial::handle_write(error_code const& ec, size_t /*size*/)
{
    if (ec) {
        opened_ = false;
        connection_handler_(is_opened());
        return;
    }

    sending_messages_.pop_front();
    if (!sending_messages_.empty())
        do_write();
}

void Serial::set_baudrate(uint32_t baud)
{
	
    port_->set_option(serial_port_base::baud_rate(baud));
}
void Serial::set_data(uint32_t val)
{
    port_->set_option(serial_port_base::character_size(val));
}
void Serial::set_stop(uint32_t val)
{
	if(val == 1)
		port_->set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
	else if(val == 2)
		port_->set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::two));
	else if (val == 3)
		//port_->set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::onepointfive));  // Val이 3이면 stop bit가 onepointfive라고 되어있는데 실제로 3이 날라오면 시리얼 통신이 안돼서 one으루 수정  CIS 
		port_->set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
	else
		port_->set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
}
void Serial::set_parity(uint32_t val)
{
	if(val == 1)
		port_->set_option(serial_port_base::parity(serial_port_base::parity::none));
	else if(val == 2)
		port_->set_option(serial_port_base::parity(serial_port_base::parity::odd));
	else if (val == 3)
		port_->set_option(serial_port_base::parity(serial_port_base::parity::even));
	else
		port_->set_option(serial_port_base::parity(serial_port_base::parity::none));

}

void Serial::setRTS(bool enabled)
{
	
	int data = TIOCM_RTS;
	if(enabled)
		ioctl(fd, TIOCMBIS, &data);
	else
		ioctl(fd, TIOCMBIC, &data);
}

void Serial::setDTR(bool enabled)
{
	
	int data = TIOCM_DTR;
	if(enabled)
		ioctl(fd, TIOCMBIS, &data);
	else
		ioctl(fd, TIOCMBIC, &data);
}

void Serial::setCTS(bool enabled)
{
	
	int data = TIOCM_CTS;
	if(enabled)
		ioctl(fd, TIOCMBIS, &data);
	else
		ioctl(fd, TIOCMBIC, &data);
}

void Serial::setDSR(bool enabled)
{
	
	int data = TIOCM_DSR;
	if(enabled)
		ioctl(fd, TIOCMBIS, &data);
	else
		ioctl(fd, TIOCMBIC, &data);
}

