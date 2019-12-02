#pragma once

#include <atomic>
#include "PacketQueue.h"
#include "Thread.h"
#include <thread>
#include <zmq.hpp>

class PacketProcessor : public Thread {
public:
    PacketProcessor(PacketQueue& packetQueue, zmq::context_t& context);
protected:
    void MainLoop() override;
    void OnThreadStarting() override;

    PacketQueue& m_packetQueue;
    zmq::context_t& m_context;
    zmq::socket_t m_socket;
};
