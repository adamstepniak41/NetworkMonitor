How to build?

    conan install . -s build_type=Release --install-folder=cmake-build-release --profile=gcc_x86_64
    cd cmake-build-release
    cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ..
    make

1. Measuring round-trip using TCP SYNC

The most common way to measure round trip using Internet Control Message Protocol (used by ping) is prone to inaccuracy due to:
    • ICMP blocking on firewall by administrators (security reasons)
    • ICMP spoofing
    • Rerouting
    • Low prioritizing ICMP

The time between TCP SYN and it’s corresponding response can be used to measure round-trip.

Source: https://www.networkcomputing.com/data-centers/network-troubleshooting-tcp-syn-analysis
