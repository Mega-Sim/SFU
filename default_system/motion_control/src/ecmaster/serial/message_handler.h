#pragma once


#include <cstdint>
#include <functional>
#include <stdexcept>

namespace sephi {

    class Message;



    using MessageHandler = std::function<void (Message const&)>;
    using ConnectionHandler = std::function<void (bool connected)>;


    namespace tcp {

        using SessionId = uint64_t;

        using ServerMessageHandler =
            std::function<void (SessionId id, Message const&)>;
        using ServerConnectionHandler =
            std::function<void (SessionId id, bool connected)>;

        using ClientMessageHandler = std::function<void (Message const&)>;
        using ClientConnectionHandler = std::function<void (bool connected)>;

    }

    namespace udp {

        using CommunicationErrorHandler =
            std::function<void (std::string const& remote_ip)>;
        using MessageHandler =
            std::function<void (std::string const& remote_ip, Message const&)>;

    }

}