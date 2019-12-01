//
// Created by adam on 01/12/2019.
//

#include <functional>
#include "PacketProcessor.h"
#include <iostream>

PacketProcessor::PacketProcessor(PacketQueue &packetQueue)
: m_packetQueue(packetQueue)
{}

void PacketProcessor::MainLoop() {
    while(m_runThread){
        auto packet = m_packetQueue.Dequeue();
        std::cout << packet->toString() << std::endl;
    }
}
