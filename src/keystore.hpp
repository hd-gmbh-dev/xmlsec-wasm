#ifndef KEYSTORE_H
#define KEYSTORE_H

#include <openssl/pkcs12.h>
#include "signer.hpp"

class KeyStore
{
private:
    PKCS12* _p12 = NULL;
    int _id = -1;
    std::shared_ptr<int> _ref_count = 0;
public:
    KeyStore();
    KeyStore (const KeyStore &obj);
    KeyStore(int id, PKCS12* p12);
    ~KeyStore();
    bool isValid();
    Signer signer(std::string password);
};
#endif