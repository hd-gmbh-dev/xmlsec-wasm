cmake_minimum_required(VERSION 3.28.3)
project(xmlsec-wasm)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
        src/buffer.cpp
        src/buffer.hpp
        src/signed.cpp
        src/signed.hpp
        src/keystore.cpp
        src/keystore.hpp
        src/signer.cpp
        src/signer.hpp
        src/validator.cpp
        src/validator.hpp
        src/signature.cpp
        src/signature.hpp
        src/bindings.cpp
)
add_definitions(-std=c++17)
add_executable(${PROJECT_NAME} ${SOURCES})
message(STATUS "Setting compilation target to WASM")
set(CMAKE_EXECUTABLE_SUFFIX ".js")

include_directories(out/usr/local/include)
include_directories(out/usr/local/include/libxml2)
include_directories(out/usr/local/include/xmlsec1)

add_library(libssl STATIC IMPORTED) # or STATIC instead of SHARED
set_target_properties(libssl PROPERTIES
       IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/out/usr/local/lib/libssl.a"
       INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/out/usr/local/include"
)
target_link_libraries(${PROJECT_NAME} PUBLIC libssl)

add_library(libcrypto STATIC IMPORTED) # or STATIC instead of SHARED
set_target_properties(libcrypto PROPERTIES
       IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/out/usr/local/lib/libcrypto.a"
       INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/out/usr/local/include"
)
target_link_libraries(${PROJECT_NAME} PUBLIC libcrypto)

add_library(xmlsec1 STATIC IMPORTED) # or STATIC instead of SHARED
set_target_properties(xmlsec1 PROPERTIES
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/out/usr/local/lib/libxmlsec1.a"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/out/usr/local/include/xmlsec1"
)
target_link_libraries(${PROJECT_NAME} PUBLIC xmlsec1)

add_library(xmlsec1-openssl STATIC IMPORTED) # or STATIC instead of SHARED
set_target_properties(xmlsec1-openssl PROPERTIES
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/out/usr/local/lib/libxmlsec1-openssl.a"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/out/usr/local/include/xmlsec1"
)
target_link_libraries(${PROJECT_NAME} PUBLIC xmlsec1-openssl)

add_library(libxml2 STATIC IMPORTED) # or STATIC instead of SHARED
set_target_properties(libxml2 PROPERTIES
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/out/usr/local/lib/libxml2.a"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/out/usr/local/include/libxml2"
)
target_link_libraries(${PROJECT_NAME} PUBLIC libxml2)

add_library(libxslt STATIC IMPORTED) # or STATIC instead of SHARED
set_target_properties(libxslt PROPERTIES
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/out/usr/local/lib/libxslt.a"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/out/usr/local/include/libxslt"
)
target_link_libraries(${PROJECT_NAME} PUBLIC libxslt)
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-O3")
set_target_properties(${PROJECT_NAME}
        PROPERTIES LINK_FLAGS "\
        -O3\
        -s EMULATE_FUNCTION_POINTER_CASTS=1\
        -s WASM=1\
        -s ENVIRONMENT=web,worker,node\
        -s MODULARIZE=1\
        -s FILESYSTEM=1\
        -s IMPORTED_MEMORY\
        -s ALLOW_MEMORY_GROWTH=1 \
        -DXMLSEC_CRYPTO_OPENSSL=1 \
        -s STACK_SIZE=10000000\
        -s MAXIMUM_MEMORY=4GB\
        -s NO_EXIT_RUNTIME=0 \
        -s ABORTING_MALLOC=0 \
        -s EXPORT_ES6=1\
        -s EXPORT_NAME=XmlSecWasm\
        --emit-tsd xmlsec-wasm.d.ts \
        --no-entry \
        -lembind \
")
