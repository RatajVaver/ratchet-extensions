cmake_minimum_required(VERSION 3.20)
project(Hash VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

add_library(lua SHARED IMPORTED)
if (WIN32)
    set_target_properties(lua PROPERTIES
            IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/../lib/lua55.lib"
            IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/../lib/lua55.dll"
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../lib"
    )
else ()
    set_target_properties(lua PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/../lib/liblua55.so"
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../lib"
    )
endif ()

add_library(Hash SHARED Hash.cpp)
set_target_properties(Hash PROPERTIES
        OUTPUT_NAME "Hash"
        PREFIX ""
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/../bin/debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/../bin/debug"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../bin"
)

if (WIN32)
    target_link_libraries(Hash PRIVATE lua bcrypt)
else ()
    find_package(OpenSSL REQUIRED)
    target_link_libraries(Hash PRIVATE lua OpenSSL::Crypto)
endif ()
