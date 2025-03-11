#include "signed.hpp"

Signed::Signed(std::string data): _data(data)
{
}

Signed::~Signed()
{
    
}

std::string Signed::content()
{
    return this->_data;
}
