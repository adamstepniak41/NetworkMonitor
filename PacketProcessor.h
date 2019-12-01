#pragma once

#include <atomic>
#include "PacketQueue.h"
#include "Thread.h"
#include <thread>

class PacketProcessor : public Thread {
public:
    PacketProcessor(PacketQueue& packetQueue);
protected:
    void MainLoop() override;

    PacketQueue& m_packetQueue;
};
