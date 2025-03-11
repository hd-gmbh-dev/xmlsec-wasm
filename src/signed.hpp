#ifndef SIGNED_H
#define SIGNED_H
#include <string>

class Signed
{
private:
    std::string _data;
public:
    Signed(std::string _data);
    ~Signed();

    std::string content();
};
#endif