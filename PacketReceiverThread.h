//
// Created by adam on 30/11/2019.
//

#ifndef NETWORKMONITOR_PACKETRECEIVERTHREAD_H
#define NETWORKMONITOR_PACKETRECEIVERTHREAD_H

#include <PcapLiveDeviceList.h>
#include <atomic>
#include <thread>
#include "PacketQueue.h"

class PacketReceiverThread {
public:
    PacketReceiverThread(PacketQueue& packetQueue);
    void Start();
    void Stop();
private:
    void MainLoop();

    bool PacketTypeSupported(pcpp::Packet& packet);
    static void OnPacketArrived(pcpp::RawPacket* pPacket, pcpp::PcapLiveDevice* pDevice, void* userCookie);
    bool StartCapturing();

    std::atomic<bool> m_runThread;
    bool m_captureStarted;
    PacketQueue& m_packetQueue;
    std::unique_ptr<std::thread> m_mainThread;
};

#endif //NETWORKMONITOR_PACKETRECEIVERTHREAD_H
