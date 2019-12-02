//
// Created by adam on 30/11/2019.
//

#ifndef NETWORKMONITOR_PACKETRECEIVER_H
#define NETWORKMONITOR_PACKETRECEIVER_H

#include <PcapLiveDeviceList.h>
#include <atomic>
#include <thread>
#include <zmq.hpp>
#include "PacketQueue.h"
#include "Thread.h"

class PacketReceiver : public Thread {
public:
    PacketReceiver(PacketQueue& packetQueue, zmq::context_t& context);
private:
    void MainLoop() override;
    void OnThreadStarting() override;

    bool PacketTypeSupported(pcpp::Packet& packet);
    static void OnPacketArrived(pcpp::RawPacket* pPacket, pcpp::PcapLiveDevice* pDevice, void* userCookie);
    void StartCapturing();

    PacketQueue& m_packetQueue;
    zmq::context_t& m_context;
    zmq::socket_t m_socket;
};

#endif //NETWORKMONITOR_PACKETRECEIVER_H
