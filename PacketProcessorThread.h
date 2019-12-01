#pragma once

#include <atomic>
#include "PacketQueue.h"
#include "Thread.h"
#include <thread>

class PacketProcessorThread : public Thread {
public:
    PacketProcessorThread(PacketQueue& packetQueue);
protected:
    void MainLoop() override;

    PacketQueue& m_packetQueue;
};
