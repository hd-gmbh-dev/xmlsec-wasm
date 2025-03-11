#ifndef BUFFER_H
#define BUFFER_H
#include <string>

class Buffer
{
private:
    std::string _data;
public:
    Buffer(std::string _data);
    ~Buffer();

    size_t length();
    uint8_t* data();
};
#endif