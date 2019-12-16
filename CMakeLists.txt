cmake_minimum_required (VERSION 3.1)
project (NetworkMonitor)

set(CMAKE_CXX_STANDARD 17)

#add_definitions("-std=c++17")

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif()

# Will be used later on when project grows and will send data to a server
#include(${CONAN_LIB_DIRS_PROTOC_INSTALLER}/cmake/protoc/protoc-config.cmake)
#protobuf_generate(TARGET NetworkMonitor)

set(SOURCES PacketReceiver.cpp Thread.cpp CaptureReceiver.cpp CaptureReceiver.h Resolver.cpp)
add_executable(NetworkMonitor NetworkMonitor.cpp ${SOURCES} Resolver.cpp Resolver.h)

target_link_libraries(NetworkMonitor PUBLIC
        ${CONAN_LIBS}
        )

# Make sure that generated headers can be found
target_include_directories(NetworkMonitor
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
        )

# UNIT TESTS
target_include_directories(NetworkMonitor
        PUBLIC ${CONAN_INCLUDE_DIRS_GTEST})

add_executable(UnitTests
        UnitTests.cpp PacketReceiverUnitTest.cpp ResolverUnitTest.cpp ${SOURCES} )

add_test(Name UnitTests
        COMMAND UnitTests)

target_link_libraries(UnitTests
        PUBLIC ${CONAN_LIBS} )
