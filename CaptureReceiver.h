#pragma once

#include "PacketReceiver.h"

class CaptureReceiver : public PacketReceiver {
public:
    CaptureReceiver(const std::string& captureName);

private:
    static void OnPacketArrived(pcpp::RawPacket* pPacket, pcpp::PcapLiveDevice* pDevice, void* userCookie);
    bool StartCapturing();
    void OnThreadStarting() override;
    std::string m_captureName;
};

