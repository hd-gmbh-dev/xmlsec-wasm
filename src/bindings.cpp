#include <emscripten/bind.h>

#include "buffer.hpp"
#include "signed.hpp"
#include "keystore.hpp"
#include "signer.hpp"
#include "signature.hpp"
#include "validator.hpp"

EMSCRIPTEN_BINDINGS(xmlsecwasm) {
    emscripten::class_<Buffer>("Buffer")
            .constructor<std::string>()
            .function("length", &Buffer::length)
            ;
    emscripten::class_<Signed>("Signed")
            .constructor<std::string>()
            .function("content", &Signed::content)
            ;
    emscripten::class_<KeyStore>("KeyStore")
            .function("isValid", &KeyStore::isValid)
            .function("signer", &KeyStore::signer)
            ;
    emscripten::class_<Signer>("Signer")
            .function("isValid", &Signer::isValid)
            .function("sign", &Signer::sign)
            ;
    emscripten::class_<Validator>("Validator")
            .function("load", &Validator::load)
            .function("verify", &Validator::verify)
            ;
    emscripten::class_<XmlSignature>("XmlSignature")
            .constructor()
            .function("loadPKCS12", &XmlSignature::loadPKCS12)
            .function("validator", &XmlSignature::validator)
            ;
}
