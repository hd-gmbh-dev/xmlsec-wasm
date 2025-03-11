#!/bin/bash

mkdir -p ./out/build
mkdir -p ./out/usr/local

export NUMCPU="$(nproc)"
export CWD="$(pwd)"
export SRC_PREFIX="$(pwd)/third_party"
export BUILD_PREFIX="$(pwd)/out/build"
export PREFIX="$(pwd)/out/usr/local"
export LIBXML2_SRC="$(pwd)/third_party/libxml2"
export LIBXSLT_SRC="$(pwd)/third_party/libxslt"
export OPENSSL_SRC="$(pwd)/third_party/openssl"
export XMLSEC_SRC="$(pwd)/third_party/xmlsec"
export CC=emcc
export CXX=emcc
export LINK=emcc

rm -rf $BUILD_PREFIX
rm -rf $PREFIX

mkdir -p $BUILD_PREFIX/libxml2-build
mkdir -p $SRC_PREFIX/libxml2/m4
cd $SRC_PREFIX/libxml2
autoreconf -if -Wall
cd $BUILD_PREFIX/libxml2-build
emconfigure $SRC_PREFIX/libxml2/configure --disable-shared \
    --with-minimum --with-http=no --with-ftp=no --with-catalog=no \
    --with-python=no --with-threads=no \
    --with-output --with-c14n --with-zlib=no --with-push \
    --with-schemas --with-schematron --host=wasm32-unknown-emscripten --with-html --with-xptr \
    --prefix=$PREFIX
emmake make -j $NUMCPU
make install

mkdir -p $SRC_PREFIX/libxslt/m4
mkdir -p $BUILD_PREFIX/libxslt-build
cd $SRC_PREFIX/libxslt
autoreconf -if -Wall
cd $BUILD_PREFIX/libxslt-build
emconfigure $LIBXSLT_SRC/configure \
    --disable-shared \
    --host=wasm32-unknown-emscripten \
    --without-python \
    --without-crypto \
    --with-libxml-prefix=$PREFIX \
    --prefix=$PREFIX
emmake make -j $NUMCPU
make install

mkdir -p $BUILD_PREFIX/openssl-build
cd $BUILD_PREFIX/openssl-build
$OPENSSL_SRC/Configure no-apps no-tests no-docs no-hw no-shared no-asm no-threads no-dtls no-engine no-dso linux-x32 -legacy -static --prefix=$PREFIX
sed -i 's|^CROSS_COMPILE.*$|CROSS_COMPILE=|g' Makefile
make -j 10 build_generated libssl.a libcrypto.a
cp -R $OPENSSL_SRC/include/openssl $PREFIX/include
cp -R include/openssl/* $PREFIX/include/openssl
cp $BUILD_PREFIX/openssl-build/libcrypto.a $PREFIX/lib
cp $BUILD_PREFIX/openssl-build/libssl.a $PREFIX/lib

mkdir -p $XMLSEC_SRC/m4
mkdir -p $BUILD_PREFIX/xmlsec-build
cd $XMLSEC_SRC
autoreconf -if -Wall
cd $BUILD_PREFIX/xmlsec-build
ls $XMLSEC_SRC
emconfigure $XMLSEC_SRC/configure --with-libxml=$PREFIX --with-openssl=$PREFIX --with-libxslt=$PREFIX --enable-apps=no --enable-openssl3-engines --enable-static-linking --host=wasm32-unknown-emscripten --prefix=$PREFIX
emmake make -j $NUMCPU
make install
