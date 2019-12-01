cmake_minimum_required (VERSION 2.6)
project (NetworkMonitor)

add_definitions("-std=c++14")

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif()

conan_basic_setup(NetworkMonitor)

add_executable(NetworkMonitor NetworkMonitor.cpp PacketQueue.cpp PacketQueue.h PacketReceiverThread.cpp PacketReceiverThread.h)

include_directories(SYSTEM include)
target_link_libraries(NetworkMonitor ${CONAN_PKG_LIBS_PCAPPLUSPLUS} ${CONAN_PKG_LIBS_LIBPCAP})
