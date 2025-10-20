#pragma once


#include <boost/asio.hpp>


namespace sephi {

    using asio_serial_base = boost::asio::serial_port_base;

    enum class BaudRate {
        br1200 = 1200,
        br2400 = 2400,
        br4800 = 4800,
        br9600 = 9600,
        br14400 = 14400,
        br19200 = 19200,
        br38400 = 38400,
        br57600 = 57600,
        br115200 = 115200
    };

    enum CharacterSize {
        cs5 = 5,
        cs6 = 6,
        cs7 = 7,
        cs8 = 8,
        cs9 = 9
    };

    enum class FlowControl {
        none = asio_serial_base::flow_control::none,
        software = asio_serial_base::flow_control::software,
        hardware = asio_serial_base::flow_control::hardware
    };

    enum class StopBits {
        one = asio_serial_base::stop_bits::one,
        onepointfive = asio_serial_base::stop_bits::onepointfive,
        two = asio_serial_base::stop_bits::two
    };

    enum class Parity {
        none = asio_serial_base::parity::none,
        odd = asio_serial_base::parity::odd,
        even = asio_serial_base::parity::even
    };


    class SerialParams {
    public:
        explicit SerialParams(
                BaudRate baud_rate = BaudRate::br115200,
                CharacterSize character_size = CharacterSize::cs8,
                StopBits stop_bits = StopBits::one,
                Parity parity = Parity::none,
                FlowControl flow_control = FlowControl::none);

        asio_serial_base::baud_rate baud_rate() const;
        asio_serial_base::character_size character_size() const;
        asio_serial_base::stop_bits stop_bits() const;
        asio_serial_base::parity parity() const;
        asio_serial_base::flow_control flow_control() const;

    private:
        BaudRate baud_rate_;
        CharacterSize character_size_;
        StopBits stop_bits_;
        Parity parity_;
        FlowControl flow_control_;
    };

}