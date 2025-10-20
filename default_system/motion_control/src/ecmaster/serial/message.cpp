#include "message.h"

#include <cstring>
#include <string>


using namespace sephi;

using std::memcpy;


Message::Message()
        : size_{0}
{}

Message::Message(uint8_t const* data, size_t size)
{
    this->size(size);
    memcpy(data_, data, size_);
}

Message& Message::operator=(uint8_t const* data)
{
    //size(std::strlen(reinterpret_cast<char const*>(data)));
    size(std::strlen(reinterpret_cast<char const*>(data)));
    memcpy(data_, data, size_);

    return *this;
}
uint8_t* Message::data()
{
    return data_;
}

uint8_t const* Message::data() const
{
    return data_;
}

size_t Message::size() const
{
    return size_;
}

void Message::size(size_t size)
{
    //if (max_size < size)
        //throw MessageSizeTooBig(std::to_string(size));

    size_ = size;
}
