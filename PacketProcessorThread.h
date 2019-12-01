#pragma once

#include <atomic>
#include "PacketQueue.h"
#include <thread>

class PacketProcessorThread {
public:
    PacketProcessorThread(PacketQueue& packetQueue);
    void Start();
    void Stop();
private:
    void MainLoop();

    std::atomic<bool> m_runThread;
    PacketQueue& m_packetQueue;
    std::unique_ptr<std::thread> m_mainThread;
};
