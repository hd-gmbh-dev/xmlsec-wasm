#define XMLSEC_CRYPTO_OPENSSL "1"

#include "signature.hpp"
#include <iostream>
#include <openssl/provider.h>
#include <openssl/evp.h>
#include <openssl/pkcs12.h>
#include <openssl/x509.h>
#include <libxml/parser.h>
#include <xmlsec/xmlsec.h>
#include <xmlsec/xmltree.h>
#include <xmlsec/xmldsig.h>
#include <xmlsec/crypto.h>
#include <xmlsec/openssl/x509.h>
#include <xmlsec/crypto.h>

XmlSignature::XmlSignature()
{
    std::cout << "create xml signature instance" << std::endl;
    /* Load Multiple providers into the default (NULL) library context */
    this->_legacy = OSSL_PROVIDER_load(NULL, "legacy");
    if (this->_legacy == NULL) {
        printf("Failed to load Legacy provider\n");
        exit(EXIT_FAILURE);
    }
    this->_deflt = OSSL_PROVIDER_load(NULL, "default");
    if (this->_deflt == NULL) {
        printf("Failed to load Default provider\n");
        OSSL_PROVIDER_unload(this->_legacy);
        exit(EXIT_FAILURE);
    }

    OpenSSL_add_all_algorithms();

    /* Init libxml and libxslt libraries */
    xmlInitParser();
    xmlLoadExtDtdDefaultValue = XML_DETECT_IDS | XML_COMPLETE_ATTRS;
    xmlSubstituteEntitiesDefault(1);
    xmlIndentTreeOutput = 1;
    xmlSaveNoEmptyTags = 1;
    this->xsltSecPrefs = xsltNewSecurityPrefs();
    xsltSetSecurityPrefs(this->xsltSecPrefs,  XSLT_SECPREF_READ_FILE,        xsltSecurityForbid);
    xsltSetSecurityPrefs(this->xsltSecPrefs,  XSLT_SECPREF_WRITE_FILE,       xsltSecurityForbid);
    xsltSetSecurityPrefs(this->xsltSecPrefs,  XSLT_SECPREF_CREATE_DIRECTORY, xsltSecurityForbid);
    xsltSetSecurityPrefs(this->xsltSecPrefs,  XSLT_SECPREF_READ_NETWORK,     xsltSecurityForbid);
    xsltSetSecurityPrefs(this->xsltSecPrefs,  XSLT_SECPREF_WRITE_NETWORK,    xsltSecurityForbid);
    xsltSetDefaultSecurityPrefs(this->xsltSecPrefs);
    /* Init xmlsec library */
    if(xmlSecInit() < 0) {
        fprintf(stderr, "Error: xmlsec initialization failed.\n");
        exit(EXIT_FAILURE);
    }

    /* Check loaded library version */
    if(xmlSecCheckVersion() != 1) {
        fprintf(stderr, "Error: loaded xmlsec library version is not compatible.\n");
        exit(EXIT_FAILURE);
    }

    /* Init crypto library */
    if(xmlSecCryptoAppInit(NULL) < 0) {
        fprintf(stderr, "Error: crypto initialization failed.\n");
        exit(EXIT_FAILURE);
    }

    /* Init xmlsec-crypto library */
    if(xmlSecCryptoInit() < 0) {
        fprintf(stderr, "Error: xmlsec-crypto initialization failed.\n");
        exit(EXIT_FAILURE);
    }
}

XmlSignature::~XmlSignature()
{
    /* Shutdown xmlsec-crypto library */
    xmlSecCryptoShutdown();

    /* Shutdown crypto library */
    xmlSecCryptoAppShutdown();
 
    /* Shutdown xmlsec library */
    xmlSecShutdown();
 
    xsltFreeSecurityPrefs(xsltSecPrefs);
    xsltCleanupGlobals();
    xmlCleanupParser();
    OSSL_PROVIDER_unload(this->_legacy);
    OSSL_PROVIDER_unload(this->_deflt);
    std::cout << "destroy xml signature instance" << std::endl;
}

KeyStore XmlSignature::loadPKCS12(Buffer buf)
{
    KeyStore result;
    BIO* bio = BIO_new_mem_buf(buf.data(), buf.length());
    if (!bio) {
        std::cerr << "Error creating BIO from memory\n";
        return result;
    }
    // Load PKCS#12 structure from BIO
    PKCS12* p12 = d2i_PKCS12_bio(bio, NULL);
    BIO_free(bio); // Free BIO after reading
    if (!p12) {
        std::cerr << "Error reading PKCS#12 from memory\n";
        return result;
    }
    this->_keystore_count += 1;
    int id = this->_keystore_count;
    return std::move(KeyStore(id, p12));
}

Validator XmlSignature::validator()
{
    xmlSecKeysMngrPtr mngr;
    mngr = xmlSecKeysMngrCreate();
    if(xmlSecCryptoAppDefaultKeysMngrInit(mngr) < 0) {
        fprintf(stderr, "Error: failed to initialize keys manager.\n");
        xmlSecKeysMngrDestroy(mngr);
        return Validator(NULL);
    }
    return Validator(mngr);
}
