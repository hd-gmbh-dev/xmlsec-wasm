
#!/bin/bash

export CMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake
export CC=emcc
export CXX=emcc
export LINK=emcc

mkdir -p out/pkg
cd out/pkg
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_FILE ../..
cmake --build . --target xmlsec-wasm
cp ../../package.json .