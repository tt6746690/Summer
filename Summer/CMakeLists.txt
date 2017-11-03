project(Summer)


# include directory and set up lib dependencies
include_directories(src)
include_directories(src/codec)
include_directories(src/middlewares)
include_directories(SYSTEM ${HEADER_ONLY_INCLUDE})

# openssl
find_package(OpenSSL)
if(OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
endif()

# server
file(GLOB SOURCE_FILES
    "./src/*.h"
    "./src/*.cpp"
    "./src/codec/*.h"
    "./src/codec/*.cpp"
    "./src/middlewares/*.h"    
    "./src/middlewares/*.cpp"        
)

# add_library(Summer ${SOURCE_FILES})
add_executable(server main.cpp ${SOURCE_FILES})


# test
file(GLOB TEST_FILES
    "./test/*.cpp"
)

add_executable(test main-test.cpp  ${TEST_FILES} ${SOURCE_FILES})

