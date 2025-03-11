#define XMLSEC_CRYPTO_OPENSSL 1

#include <iostream>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <xmlsec/xmlsec.h>
#include <xmlsec/xmltree.h>
#include <xmlsec/xmldsig.h>
#include <xmlsec/crypto.h>
#include "validator.hpp"

Validator::Validator()
{
    this->_mngr = NULL;
    this->_ref_count = std::make_shared<int>(0);
}

Validator::Validator(xmlSecKeysMngrPtr mngr)
    :
_mngr(mngr),
_ref_count(std::make_shared<int>(0))
{

}

Validator::Validator (const Validator &obj)
    :
_mngr(obj._mngr),
_ref_count(obj._ref_count)
{
}

Validator::~Validator()
{
    if (this->_ref_count.use_count() == 1 && this->_mngr != NULL)
    {
        xmlSecKeysMngrDestroy(this->_mngr);
        this->_mngr = NULL;
    }
}

int Validator::process_file(const char* tmpl_file, int tmp_file_size) {
    xmlDocPtr doc = NULL;
    xmlNodePtr node = NULL;
    xmlSecDSigCtxPtr dsigCtx = NULL;
    int res = -1;
     /* load file */
    doc = xmlReadMemory(tmpl_file, tmp_file_size, NULL, "UTF-8", XML_PARSE_PEDANTIC | XML_PARSE_NONET);
    if ((doc == NULL) || (xmlDocGetRootElement(doc) == NULL)){
        fprintf(stderr, "Error: unable to parse file \"%s\"\n", tmpl_file);
        goto done;
    }
    node = xmlSecFindNode(xmlDocGetRootElement(doc), xmlSecNodeSignature, xmlSecDSigNs);
    if(node == NULL) {
        fprintf(stderr, "Error: start node not found in \"%s\"\n", tmpl_file);
        goto done;
    }
    /* create signature context */
    dsigCtx = xmlSecDSigCtxCreate(this->_mngr);
    if(dsigCtx == NULL) {
        fprintf(stderr,"Error: failed to create signature context\n");
        goto done;
    }
    /* Verify signature */
    if(xmlSecDSigCtxVerify(dsigCtx, node) < 0) {
        fprintf(stderr,"Error: signature verify\n");
        goto done;
    }

    /* print verification result to stdout */
    if(dsigCtx->status == xmlSecDSigStatusSucceeded) {
        fprintf(stdout, "Signature is OK\n");
        res = 0;
    } else {
        fprintf(stdout, "Signature is INVALID\n");
    }
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


int Validator::load(Buffer buf) {
    return xmlSecCryptoAppKeysMngrCertLoadMemory(this->_mngr, buf.data(), buf.length(), xmlSecKeyDataFormatCertDer, xmlSecKeyDataTypeTrusted);
}

bool Validator::verify(std::string input) {
    return this->process_file(input.c_str(), input.size()) == 0;
}
