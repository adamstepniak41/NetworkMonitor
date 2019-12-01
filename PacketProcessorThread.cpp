//
// Created by adam on 01/12/2019.
//

#include <functional>
#include "PacketProcessorThread.h"
#include <iostream>

PacketProcessorThread::PacketProcessorThread(PacketQueue &packetQueue)
: m_packetQueue(packetQueue), m_runThread(false)
{}

void PacketProcessorThread::Start() {
    m_runThread = true;
    m_mainThread.reset(new std::thread(std::bind(&PacketProcessorThread::MainLoop, this)));
}

void PacketProcessorThread::Stop() {
    m_runThread = false;
    m_mainThread->join();
}

void PacketProcessorThread::MainLoop() {
    while(m_runThread){
        auto packet = m_packetQueue.Dequeue();
        std::cout << packet->toString() << std::endl;
    }
}
