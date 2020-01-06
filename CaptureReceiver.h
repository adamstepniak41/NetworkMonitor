#pragma once

#include "PacketDistributor.h"

class CaptureReceiver : public Thread {
public:
    CaptureReceiver(const std::string& captureName);

private:
    void MainLoop();
    static void OnPacketArrived(pcpp::RawPacket* pPacket, pcpp::PcapLiveDevice* pDevice, void* userCookie);
    bool StartCapturing();
    void OnThreadStarting() override;

    std::string m_captureName;
    ThreadSafeQueue<std::shared_ptr<pcpp::Packet> > m_packetQueue;
};

