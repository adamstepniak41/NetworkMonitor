//
// Created by adam on 30/11/2019.
//

#ifndef NETWORKMONITOR_PACKETRECEIVER_H
#define NETWORKMONITOR_PACKETRECEIVER_H

#include <PcapLiveDeviceList.h>
#include <atomic>
#include <thread>
#include "PacketQueue.h"
#include "Thread.h"

class PacketReceiver : public Thread {
public:
    PacketReceiver(PacketQueue& packetQueue);
private:
    void MainLoop() override;

    bool PacketTypeSupported(pcpp::Packet& packet);
    static void OnPacketArrived(pcpp::RawPacket* pPacket, pcpp::PcapLiveDevice* pDevice, void* userCookie);
    bool StartCapturing();

    bool m_captureStarted;
    PacketQueue& m_packetQueue;
};

#endif //NETWORKMONITOR_PACKETRECEIVER_H
