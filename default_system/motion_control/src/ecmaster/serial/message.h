#pragma once


#include <cstdint>
#include <stdexcept>

#include "message_handler.h"


namespace sephi {
/*
    class MessageSizeTooBig : public std::runtime_error {
    public:
        using Base = runtime_error;

        explicit MessageSizeTooBig(std::string const& what = "")
                : Base{what.c_str()}
        {}

        explicit MessageSizeTooBig(char const* what = "")
                : Base{what}
        {}
    };
*/
    class Message {
    public:
        constexpr static size_t max_size{4096};

        Message();
        Message(uint8_t const* data, size_t size);

        Message& operator=(uint8_t const* data);

        uint8_t* data();
        uint8_t const* data() const;

        size_t size() const;
        void size(size_t size);

    private:
        uint8_t data_[max_size + 1]{};
        size_t size_{};
    };

}