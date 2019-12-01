//
// Created by adam on 01/12/2019.
//

#include <functional>
#include "PacketProcessorThread.h"
#include <iostream>

PacketProcessorThread::PacketProcessorThread(PacketQueue &packetQueue)
: m_packetQueue(packetQueue)
{}

void PacketProcessorThread::MainLoop() {
    while(m_runThread){
        auto packet = m_packetQueue.Dequeue();
        std::cout << packet->toString() << std::endl;
    }
}
