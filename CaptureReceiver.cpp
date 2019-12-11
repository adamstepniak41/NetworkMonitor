#include <iostream>
#include "CaptureReceiver.h"

void CaptureReceiver::OnThreadStarting() {
    //Starting capturing in another (than currently is being started) thread
    StartCapturing();
}

bool CaptureReceiver::StartCapturing() {
    auto& devices = pcpp::PcapLiveDeviceList::getInstance();
    auto deviceList = devices.getPcapLiveDevicesList();

    pcpp::PcapLiveDevice* wlp7s0 = devices.getPcapLiveDeviceByName(m_captureName);
    if(!wlp7s0){
        std::cout << "Device: wlp7s0 was not found." << std::endl;
        return false;
    }

    if(!wlp7s0->open()){
        std::cout << "Device wlp7s0 could not be opened." << std::endl;
        return false;
    }

    if(!wlp7s0->startCapture(OnPacketArrived, &m_packetQueue)){
        std::cout << "Unable to start capturing." << std::endl;
        return false;
    }
}

void CaptureReceiver::OnPacketArrived(pcpp::RawPacket* pPacket, pcpp::PcapLiveDevice* pDevice, void* userCookie){
    auto packet = std::make_shared<pcpp::Packet>(pPacket);
    auto packetQueue = static_cast<PacketQueue*>(userCookie);

    packetQueue->Enqueue(std::move(packet));
}

CaptureReceiver::CaptureReceiver(const std::string& captureName) {
    m_captureName = captureName;
}

