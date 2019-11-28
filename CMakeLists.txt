cmake_minimum_required (VERSION 2.6)
project (NetworkMonitor)

add_definitions("-std=c++14")

include(.conan/conanbuildinfo.cmake)
conan_basic_setup(NetworkMonitor)

add_executable(NetworkMonitor NetworkMonitor.cpp)

include_directories(SYSTEM include)
target_link_libraries(NetworkMonitor ${CONAN_PKG_LIBS_PCAPPLUSPLUS} ${CONAN_PKG_LIBS_LIBPCAP})
