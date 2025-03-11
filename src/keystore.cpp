#include <iostream>
#include <openssl/pkcs12.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include "keystore.hpp"

KeyStore::KeyStore(): _id(-1), _p12(NULL)
{
    std::cout << "create empty keystore" << std::endl;
}
KeyStore::KeyStore (const KeyStore &obj): _id(obj._id), _p12(obj._p12), _ref_count(obj._ref_count) {
    std::cout << "copy keystore" << std::endl;
}
KeyStore::KeyStore(int id, PKCS12* p12): _id(id), _p12(p12), _ref_count(std::make_shared<int>(0))
{
    std::cout << "create new keystore with id: " << id << std::endl;
}

KeyStore::~KeyStore()
{
    std::cout << "ref count: " << this->_ref_count.use_count() << " ptr: " << this->_p12 << std::endl;
    if (this->_p12 != NULL && this->_ref_count.use_count() == 1) {
        std::cout << "remove keystore with id: " << this->_id << std::endl;
        PKCS12_free(this->_p12);
        this->_p12 = NULL;
    }
}

bool KeyStore::isValid()
{
    return this->_p12 != NULL;
}

Signer KeyStore::signer(std::string password) {
    Signer result;
    EVP_PKEY* pkey = NULL;
    X509* cert = NULL;
    STACK_OF(X509)* ca = NULL;

    // Extract private key, certificate, and CA chain
    if (!PKCS12_parse(this->_p12, password.c_str(), &pkey, &cert, &ca)) {
        std::cerr << "Error parsing PKCS#12 data\n";
        return result;
    }
    // Print extracted certificate
    std::cout << "Extracted Certificate:\n";
    PEM_write_X509(stdout, cert);

    // Print extracted private key (optional)
    std::cout << "Extracted Private Key:\n";
    PEM_write_PrivateKey(stdout, pkey, NULL, NULL, 0, NULL, NULL);

    // Print CA certificates if available
    if (ca) {
        std::cout << "Extracted CA Certificates:\n";
        for (int i = 0; i < sk_X509_num(ca); i++) {
            X509* cacert = sk_X509_value(ca, i);
            PEM_write_X509(stdout, cacert);
        }
    }

    return Signer(pkey, cert, ca);
}