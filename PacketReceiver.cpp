//
// Created by adam on 30/11/2019.
//

#include <PcapLiveDevice.h>
#include <iostream>
#include <functional>
#include "PacketReceiver.h"
#include "TcpLayer.h"

PacketReceiver::PacketReceiver(PacketQueue& packetQueue)
: m_packetQueue(packetQueue) {
}

void PacketReceiver::OnPacketArrived(pcpp::RawPacket* pPacket, pcpp::PcapLiveDevice* pDevice, void* userCookie){
    auto packet = std::make_unique<pcpp::Packet>(pPacket);
    auto packetReceiver = static_cast<PacketReceiver*>(userCookie);

    if(packetReceiver->PacketTypeSupported(*packet)){
        std::cout << "Packet inserted to the queue" << std::endl;
        packetReceiver->m_packetQueue.Enqueue(std::move(packet));
    }
}

bool PacketReceiver::PacketTypeSupported(pcpp::Packet &packet) {
    if(packet.isPacketOfType(pcpp::TCP)){
        return true;
    }

    return false;
}

void PacketReceiver::MainLoop() {
    while(m_runThread){
        if(!m_captureStarted){
            m_captureStarted = StartCapturing();
            continue;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // Do the this thread work
    }
}

bool PacketReceiver::StartCapturing() {
    auto& devices = pcpp::PcapLiveDeviceList::getInstance();
    auto deviceList = devices.getPcapLiveDevicesList();
    auto initialized = true;

    pcpp::PcapLiveDevice* wlp7s0 = devices.getPcapLiveDeviceByName("wlp7s0");
    if(!wlp7s0){
        std::cout << "Device: wlp7s0 was not found." << std::endl;
        initialized = false;
    }

    if(!wlp7s0->open()){
        std::cout << "Device wlp7s0 could not be opened." << std::endl;
        initialized = false;
    }

    if(!wlp7s0->startCapture(OnPacketArrived, this)){
        std::cout << "Unable to start capturing." << std::endl;
        initialized = false;
    }

    return initialized;
}

