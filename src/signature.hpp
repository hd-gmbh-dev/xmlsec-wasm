#ifndef XML_SIGNER_H
#define XML_SIGNER_H

#include <string>
#include <map>
#include <openssl/provider.h>
#include <libxslt/security.h>
#include "buffer.hpp"
#include "keystore.hpp"
#include "validator.hpp"

class XmlSignature
{
private:
    OSSL_PROVIDER *_legacy;
    OSSL_PROVIDER *_deflt;
    xsltSecurityPrefsPtr xsltSecPrefs = NULL;
    int _keystore_count = 0;
public:
    XmlSignature();
    ~XmlSignature();
    KeyStore loadPKCS12(Buffer buf);
    Validator validator();
};
#endif