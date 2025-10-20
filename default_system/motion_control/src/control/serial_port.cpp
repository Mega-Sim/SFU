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


#include <iostream>
#include <chrono>
#include "serial_port.h"
using namespace sephi;

SerialPort::SerialPort(void)
{
}

SerialPort::~SerialPort(void)
{
	stop();
}


bool SerialPort::start(const char *com_port_name,
					   	int baud_rate,
						int character_size,
						StopBits::type stopbit,
						Parity::type parity,
						FlowControl::type  flowcontrol)
{
	boost::system::error_code ec;

	if (port_) {
		std::cout << "error : port is already opened..." << std::endl;
		return false;
	}

	port_ = serial_port_ptr(new boost::asio::serial_port(io_service_));
	port_->open(com_port_name, ec);
	if (ec) {
		std::cout << "error : port_->open() failed...com_port_name="
			<< com_port_name << ", e=" << ec.message().c_str() << std::endl;
		port_.reset();
		return false;
	}
    port_name_ = com_port_name;
	// option settings...
	port_->set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
	port_->set_option(boost::asio::serial_port_base::character_size(character_size));
	port_->set_option(boost::asio::serial_port_base::stop_bits(stopbit));
	port_->set_option(boost::asio::serial_port_base::parity(parity));
	port_->set_option(boost::asio::serial_port_base::flow_control(flowcontrol));

	boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service_));
	asyncReadSome();

	return true;
}

void SerialPort::stop()
{


	if (port_) {
		port_->cancel();
		port_->close();
		port_.reset();
	}
	io_service_.stop();
	io_service_.reset();
}

int SerialPort::write(const std::string &buf)
{
	return write(buf.c_str(), buf.size());
}

int SerialPort::write(const char *buf, const int size)
{
	boost::system::error_code ec;

	if (!port_) return -1;
	if (size == 0) return 0;

	auto ret = port_->write_some(boost::asio::buffer(buf, size), ec);
	if (ret == 0) {
		SEP_LOGE("%s write error! %s", port_name_.c_str(), ec.message().c_str() );
	}

	return int(ret);
}

void SerialPort::asyncReadSome()
{
	if (port_.get() == NULL || !port_->is_open()) return;

	port_->async_read_some(
		boost::asio::buffer(read_buf_raw_, SERIAL_PORT_READ_BUF_SIZE),
		boost::bind(
				&SerialPort::onReceive,
			this, boost::asio::placeholders::error, 
			boost::asio::placeholders::bytes_transferred));
}

void SerialPort::onReceive(const boost::system::error_code &ec, size_t bytes_transferred)
{
	SEP_LOGT("Received %d\n", bytes_transferred);

	if (port_.get() == NULL || !port_->is_open()) return;
	if (ec) {
		asyncReadSome();
		return;
	}

	if (queue_.remains() < bytes_transferred) {
		SEP_LOGE("queue buffer is small to write serial data! Data loss!!!");
	}

	queue_.push(read_buf_raw_, bytes_transferred);
	cond_.wakeup();

	asyncReadSome();
}


void SerialPort::clearBuffer()
{
	std::lock_guard<std::mutex> guard(lock_);
	queue_.clear();
}

bool SerialPort::read(char *buf, const int size, int *read_size)
{
	return  readTimeout(buf, size, read_size);
}

bool SerialPort::readTimeout(char *buf, const int size, int *read_size, int64_t timeout_ms, int end_of_line)
{
	std::lock_guard<std::mutex> guard(lock_);
	auto timeout_ns = timeout_ms * 1000000;
	auto t0 = std::chrono::system_clock::now();
	int read_len(0);
	char c;
	while (true) {
		 while(queue_.size() > 0 && read_len < size){
			queue_.pop(&c, 1);
			buf[read_len] = c;
			read_len += 1;
			if (c == end_of_line) break; // end of line
         }
         if (read_len == size || c == end_of_line || timeout_ns == 0) break;
		 if (timeout_ns < 0) {
		     cond_.wait();
		 } else {
             auto wait_ns = timeout_ns - (std::chrono::system_clock::now() - t0).count();
             if ((wait_ns < 0) || !cond_.wait(wait_ns)) break;  // timeout
         }
	}
	*read_size = read_len;
	return (read_len != 0);
}

