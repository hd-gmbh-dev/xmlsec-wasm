
#define XMLSEC_CRYPTO_OPENSSL 1

#include <iostream>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <openssl/pem.h>
#include <libxslt/xslt.h>
#include <libxslt/security.h>

#include <xmlsec/xmlsec.h>
#include <xmlsec/openssl/evp.h>
#include <xmlsec/xmltree.h>
#include <xmlsec/xmldsig.h>
#include <xmlsec/templates.h>
#include <xmlsec/crypto.h>
#include "signer.hpp"

Signer::Signer()
{
}

Signer::Signer (const Signer &obj)
    :
_pkey(obj._pkey),
_cert(obj._cert),
_ca(obj._ca),
_ref_count(obj._ref_count)
{
}

Signer::Signer(EVP_PKEY* pkey, X509* cert, STACK_OF(X509)* ca)
    :
_pkey(pkey),
_cert(cert),
_ca(ca),
_ref_count(std::make_shared<int>(0))
{
}

Signer::~Signer()
{
    if (this->_ref_count.use_count() == 1 && (this->_cert == NULL || this->_pkey == NULL || this->_ca == NULL))
    {
        if (this->_cert != NULL) {
            X509_free(this->_cert);
            this->_cert = NULL;
        }
        if (this->_pkey != NULL) {
            EVP_PKEY_free(this->_pkey);
            this->_pkey = NULL;
        }
        if (this->_ca) {
            sk_X509_pop_free(this->_ca, X509_free);
            this->_ca = NULL;
        }
    }
}

bool Signer::isValid() {
    return this->_cert != NULL && this->_pkey != NULL;
}


int Signer::process_file(const char* tmpl_file, int tmp_file_size, xmlChar **mem, int *size) {
    xmlDocPtr doc = NULL;
    xmlNodePtr node = NULL;
    xmlSecDSigCtxPtr dsigCtx = NULL;
    xmlSecKeyDataPtr keyData = NULL;
    xmlSecKeyDataPtr x509Data;
    unsigned char * name = NULL;
    int nameLen = 0;
    xmlSecSize nameSize = 0;
    xmlSecKeyPtr signKey = xmlSecKeyCreate();
    int res = -1;

    assert(tmpl_file);
    /* load template */
    doc = xmlReadMemory(tmpl_file, tmp_file_size, NULL, "UTF-8", XML_PARSE_PEDANTIC | XML_PARSE_NONET);
    if ((doc == NULL) || (xmlDocGetRootElement(doc) == NULL)){
        fprintf(stderr, "Error: unable to parse file \"%s\"\n", tmpl_file);
        goto done;
    }

    /* find start node */
    node = xmlSecFindNode(xmlDocGetRootElement(doc), xmlSecNodeSignature, xmlSecDSigNs);
    if(node == NULL) {
        fprintf(stderr, "Error: start node not found in \"%s\"\n", tmpl_file);
        goto done;
    }
    keyData = xmlSecOpenSSLEvpKeyAdopt(this->_pkey);
    res = xmlSecKeySetValue(signKey, keyData);
    if (res != 0)
    {
        fprintf(stderr,"Error: (%d) unable to create key\n", res);
        goto done;
    }
    if(name == NULL) {
        name = X509_alias_get0(this->_cert, &nameLen);
    }
    if(name == NULL) {
        name = X509_keyid_get0(this->_cert, &nameLen);
    }
    if((name != NULL) && (nameLen > 0)) {
        nameSize = (int) nameLen;
        res = xmlSecKeySetNameEx(signKey, name, nameSize);
        if(res < 0) {
            fprintf(stderr,"Error: (%d) unable to set key name\n", res);
            goto done;
        }
    }
    x509Data = xmlSecKeyEnsureData(signKey, xmlSecOpenSSLKeyDataX509Id);
    res = xmlSecOpenSSLKeyDataX509AdoptKeyCert(x509Data, this->_cert);
    if (res != 0)
    {
        fprintf(stderr,"Error: (%d) unable to create public key\n", res);
        goto done;
    }
    dsigCtx = xmlSecDSigCtxCreate(NULL);
    dsigCtx->signKey = signKey;
    if(dsigCtx->signKey == NULL) {
        fprintf(stderr,"Error: failed to load key\n");
        goto done;
    }

    /* sign the template */
    if(xmlSecDSigCtxSign(dsigCtx, node) < 0) {
        fprintf(stderr,"Error: signature failed\n");
        goto done;
    }

    xmlDocDumpFormatMemory(doc, mem, size, 4);

    res = 0;
done:
    /* cleanup */
    if(dsigCtx != NULL) {
        xmlSecDSigCtxDestroy(dsigCtx);
    }

    if(doc != NULL) {
        xmlFreeDoc(doc);
    }
    return(res);
}

Signed Signer::sign(std::string input)
{
    xmlChar *xmlbuff;
    int buffersize;
    if (this->process_file(input.c_str(), input.size(), &xmlbuff, &buffersize) == 0)
    {
        std::string result((const char*)xmlbuff, buffersize);
        return Signed(result);
    } else {
        return Signed("");
    }
}
