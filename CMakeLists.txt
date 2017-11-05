# cmake -Bbuild Wno-dev
# cmake --build build -- -j5

# version and project name
cmake_minimum_required(VERSION 2.6)
project(Summer)

# compiler
set(CMAKE_CXX_COMPILER_ID "Clang")
# compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -stdlib=libc++ -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/local/opt/openssl/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DASIO_SEPARATE_COMPILATION -DASIO_STANDALONE")
# linker flags 
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/opt/openssl/lib -lssl -lcrypto")


# ccache
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

# generic path 
set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)      
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})        

# header only libraries libraries 
set(HEADER_ONLY_INCLUDE 
    ../lib/asio-1.10.6/include
    ../lib/Catch/single_include
    ../lib/json/src)

# add subdirectories 
set(Http_SOURCE_DIR ${PROJECT_SOURCE_DIR}/Summer)
set(Lib_SOURCE_DIR ${PROJECT_SOURCE_DIR}/lib)
add_subdirectory(${Http_SOURCE_DIR})
add_subdirectory(${Lib_SOURCE_DIR})


