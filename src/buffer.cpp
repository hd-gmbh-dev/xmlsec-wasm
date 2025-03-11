#include "buffer.hpp"

Buffer::Buffer(std::string data): _data(data)
{
}

Buffer::~Buffer()
{
    
}

size_t Buffer::length()
{
    return this->_data.size();
}

uint8_t* Buffer::data()
{
    return (uint8_t*)this->_data.c_str();
}
