#ifndef SIGNER_H
#define SIGNER_H

#include <emscripten/val.h>
#include <openssl/evp.h>
#include <libxml/xmlIO.h>
#include "buffer.hpp"
#include "signed.hpp"

class Signer
{
private:
    EVP_PKEY* _pkey = NULL;
    X509* _cert = NULL;
    STACK_OF(X509)* _ca = NULL;
    std::shared_ptr<int> _ref_count = 0;
    int process_file(const char* tmpl_file, int tmp_file_size, xmlChar **mem, int *size);
public:
    Signer();
    Signer(const Signer &obj);
    Signer(EVP_PKEY* pkey, X509* cert, STACK_OF(X509)* ca);
    ~Signer();
    bool isValid();
    Signed sign(std::string input);
};
#endif