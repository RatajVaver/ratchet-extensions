cmake_minimum_required(VERSION 3.20)
project(Hash VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_library(lua SHARED IMPORTED)
set_target_properties(lua PROPERTIES
        IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/../lib/lua55.lib"
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/../lib/lua55.dll"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../lib"
)

add_library(Hash SHARED Hash.cpp)
set_target_properties(Hash PROPERTIES
        OUTPUT_NAME "Hash"
        PREFIX ""
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/../bin/debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../bin"
)
target_link_libraries(Hash PRIVATE lua bcrypt)
target_link_options(Hash PRIVATE /NODEFAULTLIB:LIBCMT)
