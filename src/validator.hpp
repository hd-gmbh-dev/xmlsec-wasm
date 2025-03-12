#ifndef VALIDATOR_H
#define VALIDATOR_H
#include <string.h>
#include <memory.h>
#include <xmlsec/xmlsec.h>
#include "buffer.hpp"

class Validator
{
private:
    xmlSecKeysMngrPtr _mngr;
    std::shared_ptr<int> _ref_count = 0;
    int process_file(const char* tmpl_file, int tmp_file_size);
public:
    Validator();
    Validator(xmlSecKeysMngrPtr mngr);
    Validator(const Validator &obj);
    ~Validator();

    int load(Buffer cert);
    bool verify(std::string input);
};
#endif