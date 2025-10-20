#include "serial_params.h"


using namespace sephi;


SerialParams::SerialParams(
        BaudRate baud_rate,
        CharacterSize character_size,
        sephi::StopBits stop_bits,
        sephi::Parity parity,
        sephi::FlowControl flow_control)
        : baud_rate_{baud_rate},
          character_size_{character_size},
          stop_bits_{stop_bits},
          parity_{parity},
          flow_control_{flow_control}
{
}

asio_serial_base::baud_rate SerialParams::baud_rate() const
{
    return asio_serial_base::baud_rate{static_cast<unsigned>(baud_rate_)};
}

asio_serial_base::character_size SerialParams::character_size() const
{
    return asio_serial_base::character_size{
            static_cast<unsigned>(character_size_)};
}

asio_serial_base::stop_bits SerialParams::stop_bits() const
{
    return asio_serial_base::stop_bits{
            static_cast<asio_serial_base::stop_bits::type>(stop_bits_)};
}

asio_serial_base::parity SerialParams::parity() const
{
    return asio_serial_base::parity{
            static_cast<asio_serial_base::parity::type>(parity_)};
}

asio_serial_base::flow_control SerialParams::flow_control() const
{
    return asio_serial_base::flow_control{
            static_cast<asio_serial_base::flow_control::type>(flow_control_)};
}