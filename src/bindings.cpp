#include <emscripten/bind.h>

#include "buffer.hpp"
#include "signed.hpp"
#include "keystore.hpp"
#include "signer.hpp"
#include "signature.hpp"

EMSCRIPTEN_BINDINGS(xmlsignature) {
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
    emscripten::class_<XmlSignature>("XmlSignature")
            .constructor()
            .function("loadPKCS12", &XmlSignature::loadPKCS12)
            ;
}
